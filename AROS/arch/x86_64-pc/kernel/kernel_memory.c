
#include <aros/macros.h>
#include <aros/multiboot.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/memheaderext.h>

#include "kernel_base.h"
#include "kernel_debug.h"
#include "kernel_mmap.h"

#include "tlsf.h"

#define D(x)


BOOL krnAddMemory(struct MemHeaderExt **mhe, IPTR start, IPTR end,
        IPTR mh_Start, const struct MemRegion *reg)
{
    D(nbug("krnAddMemory(%p, %p, %p, %p, %p)\n", *mhe, start, end, mh_Start, reg));

    /* Does MemHeaderExt exist? If not, create it */
    if (*mhe == NULL)
    {
        /* Align up start address to nearest page boundary */
        start = (start + 4095) & ~4095;

        /* Align down end address to the page boundary */
        end = end & 4095;

        D(nbug("MemHeaderExt non-existing. Creating it...\n"));
        krnCreateTLSFMemHeader(reg->name, reg->pri, start, end - start, reg->flags);
        *mhe = (struct MemHeaderExt *)start;
        D(nbug("got %p\n", *mhe));

        if (*mhe == NULL)
            return FALSE;
    }
    else
    {
        D(nbug("adding memory (%p - %p) to MemHeader %p\n",
                start, end-1, *mhe));
        tlsf_add_memory(*mhe, (void *)start, end - start);
    }

    return TRUE;
}

/*
 * Build conventional memory lists out of multiboot memory map structure.
 * Will add all MemHeaders to the specified list in the same order they
 * were created, not in the priority one.
 * Memory breakup is specified by an array of MemRegion structures.
 *
 * The algorithm is the following:
 * 1. Traverse MemRegion array. For each region repeat all of the following:
 * 2. Set starting address (cur_start) to the beginning of the region.
 * 3. Traverse the entire memory map, locating the lowest fitting chunk.
 * 4. If we have found a chunk in (3), we add it to the memory list.
 * 5. If there's a gap between this chunk and the previously added one, we also start a new MemHeader.
 * 6, Set cur_start to the end of this repeat the process from step (3).
 *
 * This effectively sorts memory map entries in ascending order and merges adjacent chunks into single MemHeaders.
 */
void mmap_InitMemory(struct mb_mmap *mmap_addr, unsigned long mmap_len, struct MinList *memList,
                     IPTR klo, IPTR khi, IPTR reserve, const struct MemRegion *reg)
{
    while (reg->name)
    {
        struct MemHeaderExt *mhe = NULL;
        IPTR phys_start = ~0;
        IPTR cur_start  = reg->start;
        IPTR chunk_start;
        IPTR chunk_end;
        unsigned int chunk_type;

        D(nbug("[MMAP] Processing region 0x%p - 0x%p (%s)...\n", reg->start, reg->end, reg->name));

        do
        {
            struct mb_mmap *mmap = mmap_addr;
            unsigned long len = mmap_len;

            chunk_start = ~0;
            chunk_end   = 0;
            chunk_type  = 0;

            while (len >= sizeof(struct mb_mmap))
            {
                IPTR start = mmap->addr;
                IPTR end   = 0;

                end = mmap->addr + mmap->len;

                if ((cur_start < end) && (reg->end > start))
                {
                    if (cur_start > start)
                        start = cur_start;
                    if (reg->end < end)
                        end = reg->end;

                    if (start < chunk_start)
                    {
                        chunk_start = start;
                        chunk_end   = end;
                        chunk_type  = mmap->type;

                        if (chunk_start == cur_start)
                        {
                            /*
                             * Terminate search early if the found chunk is in the beginning of the region
                             * to consider. There will be no better match.
                             */
                            break;
                        }
                    }
                }

                /* Go to the next chunk */
                len -= mmap->size + 4;
                mmap = (struct mb_mmap *)(mmap->size + (IPTR)mmap + 4);
            }

            if (chunk_end)
            {
                /* Have a chunk to add. Either reserved or free. */

                if (mhe && (chunk_start > cur_start))
                {
                    /*
                     * There is a physical gap in the memory. Add current MemHeader to the list and reset pointers
                     * in order to begin a new one.
                     */
                    D(nbug("[MMAP] Physical gap   0x%p - 0x%p\n", cur_start, chunk_start));

                    ADDTAIL(memList, mhe);
                    mhe = NULL;
                    phys_start = ~0;
                }

                if (phys_start == ~0)
                    phys_start = chunk_start;

                if (chunk_type == MMAP_TYPE_RAM)
                {
                    /* Take reserved space into account */
                    if (reserve > chunk_start)
                        chunk_start = reserve;

                    D(nbug("[MMAP] Usable   chunk 0x%p - 0x%p\n", chunk_start, chunk_end));

                    /*
                     * Now let's add the chunk. However, this is the right place to remember about klo and khi.
                     * Area occupied by kickstart must appear to be preallocated. This way our chunk can be
                     * split into up to three chunks, one of which will be occupied by the KS.
                     */
                    if ((klo >= chunk_end) || (khi <= chunk_start))
                    {

                        /* If the kickstart is placed outside of this region, just add it as it is */
                        krnAddMemory(&mhe, chunk_start, chunk_end, phys_start, reg);
                    }
                    else
                    {
                        /* Have some usable space above the kickstart ? */
                        if (klo > chunk_start)
                            krnAddMemory(&mhe, chunk_start, klo, phys_start, reg);

                        /* Have some usable space below the kickstart ? */
                        if (khi < chunk_end)
                            krnAddMemory(&mhe, khi, chunk_end, phys_start, reg);
                    }
                }
                else if (mhe)
                {
                    /* Just expand physical MemHeader area, but do not add the chunk as free */
                    D(nbug("[MMAP] Reserved chunk 0x%p - 0x%p\n", chunk_start, chunk_end));

                    mhe->mhe_MemHeader.mh_Upper = (APTR)chunk_end;
                }

                if (chunk_end == reg->end)
                {
                    /* Terminate early if we have reached the end of region */
                    break;
                }

                cur_start = chunk_end;
            }

        } while (chunk_end);

        /* Add the last MemHeader if exists */
        if (mhe)
            ADDTAIL(memList, mhe);

        reg++;
    }
}

struct mb_mmap *mmap_FindRegion(IPTR addr, struct mb_mmap *mmap, unsigned long len)
{
    while (len >= sizeof(struct mb_mmap))
    {
        IPTR end;

#ifdef __i386__
        /* We are on i386, ignore high memory */
        if (mmap->addr_high)
            return NULL;

        if (mmap->len_high)
            end = 0x80000000;
        else
#endif
        end = mmap->addr + mmap->len;

        /* Returh chunk pointer if matches */
        if ((addr >= mmap->addr) && (addr < end))
            return mmap;

        /* Go to the next chunk */
        len -= mmap->size + 4;
        mmap = (struct mb_mmap *)(mmap->size + (IPTR)mmap + 4);
    }
    return NULL;
}

/* Validate the specified region via memory map */
BOOL mmap_ValidateRegion(unsigned long addr, unsigned long len, struct mb_mmap *mmap, unsigned long mmap_len)
{
    /* Locate a memory region */
    struct mb_mmap *region = mmap_FindRegion(addr, mmap, mmap_len);

    /* If it exists, and free for usage... */
    if (region && region->type == MMAP_TYPE_RAM)
    {
        IPTR end = region->addr + region->len;

        /* Make sure it covers the whole our specified area */
        if (addr + len < end)
            return TRUE;

    }
    return FALSE;
}
