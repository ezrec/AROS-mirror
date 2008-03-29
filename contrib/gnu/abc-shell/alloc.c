/*
 * area-based allocation built on malloc/free
 */

#include "sh.h"

#define ICELLS  200             /* number of Cells in small Block */

typedef union Cell Cell;
typedef struct Block Block;

/*
 * The Cells in a Block are organized as a set of objects.
 * Each object (pointed to by dp) begins with the block it is in
 * (dp-2)->block, then has a size in (dp-1)->size, which is
 * followed with "size" data Cells.  Free objects are
 * linked together via dp->next.
 */

#define NOBJECT_FIELDS  2       /* the block and size `fields' */

union Cell {
        size_t  size;
        Cell   *next;
        Block  *block;
        struct {int _;} junk;   /* alignment */
        double djunk;           /* alignment */
};

struct Block {
        Block  *next;           /* list of Blocks in Area */
        Block  *prev;           /* previous block in list */
        Cell   *freelist;       /* object free list */
        Cell   *last;           /* &b.cell[size] */
        Cell    cell [1];       /* [size] Cells for allocation */
};

static Block aempty = {&aempty, &aempty, aempty.cell, aempty.cell};

static void ablockfree(Block *, Area *);
static void *asplit(Area *, Block *, Cell *, Cell *, int);

/* create empty Area */
Area *
ainit(Area *ap)
{
        ap->freelist = &aempty;
        return ap;
}

/* free all object in Area */
void
afreeall(Area *ap)
{
        Block *bp;
        Block *tmp;

        bp = ap->freelist;
        if (bp != NULL && bp != &aempty) {
                do {
                        tmp = bp;
                        bp = bp->next;
                        free((void*)tmp);
                } while (bp != ap->freelist);
                ap->freelist = &aempty;
        }
}

/* allocate object from Area */
void *
alloc(size_t size, Area *ap)
{
        int cells, acells;
        Block *bp = 0;
        Cell *fp = 0, *fpp = 0;

        if (size <= 0)
                internal_errorf(1, "allocate bad size");
        cells = (unsigned)(size + sizeof(Cell) - 1) / sizeof(Cell);

        /* allocate at least this many cells */
        acells = cells + NOBJECT_FIELDS;

        /*
         * Only attempt to track small objects - let malloc deal
         * with larger objects. (this way we don't have to deal with
         * coalescing memory, or with releasing it to the system)
         */
        if (cells <= ICELLS) {
                /* find free Cell large enough */
                for (bp = ap->freelist; ; bp = bp->next) {
                        for (fpp = NULL, fp = bp->freelist;
                             fp != bp->last; fpp = fp, fp = fp->next)
                        {
                                if ((fp-1)->size >= cells)
                                        goto Found;
                        }
                        /* wrapped around Block list, create new Block */
                        if (bp->next == ap->freelist) {
                                bp = 0;
                                break;
                        }
                }
                /* Not much free space left?  Allocate a big object this time */
                acells += ICELLS;
        }
        if (bp == 0) {
                bp = (Block*) malloc(offsetof(Block, cell) +
                                     acells * sizeof(Cell));
                if (bp == NULL)
                        internal_errorf(1, "cannot allocate");
                if (ap->freelist == &aempty) {
                        ap->freelist = bp->next = bp->prev = bp;
                } else {
                        bp->next = ap->freelist->next;
                        ap->freelist->next->prev = bp;
                        ap->freelist->next = bp;
                        bp->prev = ap->freelist;
                }
                bp->last = bp->cell + acells;
                /* initial free list */
                fp = bp->freelist = bp->cell + NOBJECT_FIELDS;
                (fp-1)->size = acells - NOBJECT_FIELDS;
                (fp-2)->block = bp;
                fp->next = bp->last;
                fpp = NULL;
        }

  Found:
        return asplit(ap, bp, fp, fpp, cells);
}

/* Do the work of splitting an object into allocated and (possibly) unallocated
 * objects.  Returns the `allocated' object.
 */
static void *
asplit(Area *ap, Block *bp, Cell *fp, Cell *fpp, int cells)
{
        Cell *dp = fp;  /* allocated object */
        int split = (fp-1)->size - cells;

        if (split < 0)
                internal_errorf(1, "allocated object too small");
        if (split <= NOBJECT_FIELDS) {  /* allocate all */
                fp = fp->next;
        } else {                /* allocate head, free tail */
                Cell *next = fp->next; /* needed, as cells may be 0 */
                ap->freelist = bp; /* next time, start looking for space here */
                (fp-1)->size = cells;
                fp += cells + NOBJECT_FIELDS;
                (fp-1)->size = split - NOBJECT_FIELDS;
                (fp-2)->block = bp;
                fp->next = next;
        }
        if (fpp == NULL)
                bp->freelist = fp;
        else
                fpp->next = fp;
        return (void*) dp;
}

/* change size of object -- like realloc */
void *
aresize(void *ptr, size_t size, Area *ap)
{
        int cells;
        Cell *dp = (Cell*) ptr;
        int oldcells = dp ? (dp-1)->size : 0;

        if (size <= 0)
                internal_errorf(1, "allocate bad size");
        /* New size (in cells) */
        cells = (unsigned)(size - 1) / sizeof(Cell) + 1;

        /* Is this a large object?  If so, let malloc deal with it
         * directly (unless we are crossing the ICELLS border, in
         * which case the alloc/free below handles it - this should
         * cut down on fragmentation, and will also keep the code
         * working (as it assumes size < ICELLS means it is not
         * a `large object').
         */
        if (oldcells > ICELLS && cells > ICELLS 
            && ((dp-2)->block->last == dp+oldcells) && (((Cell*)((dp-2)->block+1))+NOBJECT_FIELDS == dp) /* don't destroy blocks which have grown! */
           ) {
                Block *bp = (dp-2)->block;
                Block *nbp;
                /* Saved in case realloc fails.. */
                Block *next = bp->next, *prev = bp->prev;

                if (bp->freelist != bp->last)
                        internal_errorf(1, "allocation resizing free pointer");
                nbp = realloc((void *) bp,
                              offsetof(Block, cell) +
                              (cells + NOBJECT_FIELDS) * sizeof(Cell));
                if (!nbp) {
                        /* Have to clean up... */
                        /* NOTE: If this code changes, similar changes may be
                         * needed in ablockfree().
                         */
                        if (next == bp) /* only block */
                                ap->freelist = &aempty;
                        else {
                                next->prev = prev;
                                prev->next = next;
                                if (ap->freelist == bp)
                                        ap->freelist = next;
                        }
                        internal_errorf(1, "cannot re-allocate");
                }
                /* If location changed, keep pointers straight... */
                if (nbp != bp) {
                        if (next == bp) /* only one block */
                                nbp->next = nbp->prev = nbp;
                        else {
                                next->prev = nbp;
                                prev->next = nbp;
                        }
                        if (ap->freelist == bp)
                                ap->freelist = nbp;
                        dp = nbp->cell + NOBJECT_FIELDS;
                        (dp-2)->block = nbp;
                }
                (dp-1)->size = cells;
                nbp->last = nbp->cell + cells + NOBJECT_FIELDS;
                nbp->freelist = nbp->last;

                return (void*) dp;
        }

        /* Check if we can just grow this cell
         * (need to check that cells < ICELLS so we don't make an
         * object a `large' - that would mess everything up).
         */
        if (dp && cells > oldcells) {
                Cell *fp, *fpp;
                Block *bp = (dp-2)->block;
                int need = cells - oldcells - NOBJECT_FIELDS;

                /* XXX if we had a flag in an object indicating
                 * if the object was free/allocated, we could
                 * avoid this loop (perhaps)
                 */
                for (fpp = NULL, fp = bp->freelist;
                     fp != bp->last
                     && dp + oldcells + NOBJECT_FIELDS <= fp
                     ; fpp = fp, fp = fp->next)
                {
                        if (dp + oldcells + NOBJECT_FIELDS == fp
                            && (fp-1)->size >= need)
                        {
                                Cell *np = asplit(ap, bp, fp, fpp, need);
                                /* May get more than we need here */
                                (dp-1)->size += (np-1)->size + NOBJECT_FIELDS;
                                return ptr;
                        }
                }
        }

        /* Check if we can just shrink this cell
         * (if oldcells > ICELLS, this is a large object and we leave
         * it to malloc...)
         * Note: this also handles cells == oldcells (a no-op).
         */
        if (dp && cells <= oldcells) {
                int split;

                split = oldcells - cells;
                if (split <= NOBJECT_FIELDS) /* cannot split */
                        ;
                else {          /* shrink head, free tail */
                        Block *bp = (dp-2)->block;

                        (dp-1)->size = cells;
                        dp += cells + NOBJECT_FIELDS;
                        (dp-1)->size = split - NOBJECT_FIELDS;
                        (dp-2)->block = bp;
                        afree((void*)dp, ap);
                }
                /* ACHECK() done in afree() */
                return ptr;
        }

        /* Have to do it the hard way... */
        ptr = alloc(size, ap);
        if (dp != NULL) {
                size_t s = (dp-1)->size * sizeof(Cell);
                if (s > size)
                        s = size;
                memcpy(ptr, dp, s);
                afree((void *) dp, ap);
        }
        return ptr;
}

void
afree(void *ptr, Area *ap)
{
        Block *bp;
        Cell *fp, *fpp;
        Cell *dp = (Cell*)ptr;

        if (ptr == 0)
                internal_errorf(1, "freeing null pointer");
        bp = (dp-2)->block;

        /* If this is a large object, just free it up... */
        /* Release object... */
        if ((dp-1)->size > ICELLS
            && (bp->last == dp + (dp-1)->size) && (((Cell*)(bp+1))+NOBJECT_FIELDS == dp) /* don't free non-free blocks which have grown! */
           ) {
                ablockfree(bp, ap);
                return;
        }

        if (dp < &bp->cell[NOBJECT_FIELDS] || dp >= bp->last)
                internal_errorf(1, "freeing memory outside of block (corrupted?)");

        /* find position in free list */
        /* XXX if we had prev/next pointers for objects, this loop could go */
        for (fpp = NULL, fp = bp->freelist; fp < dp; fpp = fp, fp = fp->next)
                ;

        if (fp == dp)
                internal_errorf(1, "freeing free object");

        /* join object with next */
        if (dp + (dp-1)->size == fp-NOBJECT_FIELDS) { /* adjacent */
                (dp-1)->size += (fp-1)->size + NOBJECT_FIELDS;
                dp->next = fp->next;
        } else                  /* non-adjacent */
                dp->next = fp;

        /* join previous with object */
        if (fpp == NULL)
                bp->freelist = dp;
        else if (fpp + (fpp-1)->size == dp-NOBJECT_FIELDS) { /* adjacent */
                (fpp-1)->size += (dp-1)->size + NOBJECT_FIELDS;
                fpp->next = dp->next;
        } else                  /* non-adjacent */
                fpp->next = dp;

        /* If whole block is free (and we have some other blocks
         * around), release this block back to the system...
         */
        if (bp->next != bp && bp->freelist == bp->cell + NOBJECT_FIELDS
            && bp->freelist + (bp->freelist-1)->size == bp->last
            /* XXX and the other block has some free memory? */
            )
                ablockfree(bp, ap);
}

static void
ablockfree(Block *bp, Area *ap)
{
        /* NOTE: If this code changes, similar changes may be
         * needed in alloc() (where realloc fails).
         */

        if (bp->next == bp) /* only block */
                ap->freelist = &aempty;
        else {
                bp->next->prev = bp->prev;
                bp->prev->next = bp->next;
                if (ap->freelist == bp)
                        ap->freelist = bp->next;
        }
        free((void*) bp);
}

