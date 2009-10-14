/*
**    Memory management
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

Why is Feelin memory system so f*cking fast ?

    It's magic, or maybe because puddles are hashed are linked together  in
    a  tiny global hash table (e.g. 4ko) and an address to the chunk in the
    'middle' of the puddle is often used to 'skip' lots of checkings.

*****************************************************************************

$VER: 04.00 (2005/08/05)

    Memory system rewritten  and  greatly  improved  (particuliarly  memory
    disposal).

    Memory is now cleared and filled with a turbo method.

$VER: 03.02 (2005/04/06)

    Removed  an  error   introduced   while   porting   ASM   code   to   C.
    f_chunk_create()  failed allocating the last free memory chunk, making a
    new puddle to be created. I was testing a '<' instead of '<='...

$VER: 03.00 (2005/01/13)

    The whole memory system has finaly been ported to C, even  the  almighty
    F_NewP(), F_Dispose() and F_DisposeP() functions. The end of one era :-)

*/

#include "Private.h"

//#define DB_MEMORY_STATS


//#define DB_STATS_DISPOSE

#define F_USE_PUDDLE_MIDDLE

///DB_STATS_DISPOSE
#ifdef DB_STATS_DISPOSE

struct FeelinStats
{
    uint32                          immediate;
    uint32                          link;
    uint32                          prev;
    uint32                          prev_total_distance;
    uint32                          next;
    uint32                          fucked;

    uint32                          step;
};

STATIC struct FeelinStats stats =
{
    0, 0, 0, 0, 0, 0, 20
};

#endif
//+

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///f_memory_hash_rem
STATIC void f_memory_hash_rem(FPuddle *Puddle, struct in_FeelinBase *FeelinBase)
{
    uint32 h = F_MEMORY_HASH(Puddle->lower);

    FPuddle *prev=NULL;
    FPuddle *node;

    for (node = FeelinBase->hash_puddles[h] ; node ; node = node->hash_next)
    {
        if (node == Puddle)
        {
            if (prev)
            {
                prev->hash_next = Puddle->hash_next;
            }
            else
            {
                FeelinBase->hash_puddles[h] = Puddle->hash_next;
            }

///DB_MEMORY_STATS
            #ifdef DB_MEMORY_STATS
            FeelinBase->numpuddles--;
            #endif
//+

            return;
        }
        prev = node;
    }

    if (!node)
    {
        F_Log(0,"Puddle (0x%08lx) not found",Puddle);
    }
}
//+

/****************************************************************************
*** Functions ***************************************************************
****************************************************************************/

///f_pool_create_a
F_LIB_POOL_CREATE
{
    const struct TagItem *tags = Tagl;
    struct TagItem *item;

    uint32 pool_attributes = MEMF_CLEAR;
    uint32 pool_itemnum = 10;
    uint32 pool_public = FALSE;
    STRPTR pool_name = "Private";
    #ifdef F_USE_MEMORY_WALL
    uint32 pool_wall_size = FeelinBase->debug_memory_wall_size;
    #endif

    FPool *LOD;

    while  ((item = NextTagItem(&tags)))
    switch (item -> ti_Tag)
    {
        case FA_Pool_Attributes:   pool_attributes = item -> ti_Data; break;
        case FA_Pool_ItemSize:     ItemSize = item -> ti_Data; break;
        case FA_Pool_Items:        pool_itemnum = item -> ti_Data; break;
        case FA_Pool_Name:         pool_name = (STRPTR) item -> ti_Data; break;
        case FA_Pool_Public:       pool_public = item -> ti_Data; break;

        default:
        {
            F_Log(FV_LOG_DEV,"F_CreatePoolA() Unknown attribute 0x%08lx (0x%08lx)",item -> ti_Tag);
        }
    }

#ifdef DEBUG_CODE

    if (FF_DEBUG_PUBLICSEMAPHORES & FeelinBase -> debug_flags)
    {
        pool_public = TRUE;
    }

#endif

    LOD = AllocMem(sizeof (FPool),MEMF_PUBLIC | MEMF_CLEAR);

    if (LOD)
    {
        InitSemaphore(&LOD -> Semaphore);

        LOD->flags = pool_attributes;

        #ifdef F_USE_MEMORY_WALL
        LOD->thresh_size = sizeof (FMemChunk) + (((pool_wall_size * 2 + ItemSize + 3) >> 2) << 2);
        #else
        LOD->thresh_size = sizeof (FMemChunk) + (((ItemSize + 3) >> 2) << 2);
        #endif

        LOD->puddle_size = LOD -> thresh_size * pool_itemnum;

        #ifdef F_USE_MEMORY_WALL
        LOD->wall_size = pool_wall_size;
        #endif

        LOD->Semaphore.ss_Link.ln_Name = pool_name;

        Forbid();

        if (pool_public)
        {
            Enqueue(&FeelinBase -> Public.SYS -> SemaphoreList,(struct Node *) &LOD -> Semaphore);
        }

        LOD->next = FeelinBase->pools;
        FeelinBase->pools = LOD;

        Permit();
    }
    return LOD;
}
//+
///f_pool_delete
F_LIB_POOL_DELETE
{
    Forbid();

    if (Pool)
    {

/*** search and remove memory pool *****************************************/

        FPool *prev=NULL;
        FPool *node;

        for (node = FeelinBase->pools ; node ; node = node->next)
        {
            if (node == Pool)
            {
                if (prev)
                {
                    prev->next = node -> next;
                }
                else
                {
                    FeelinBase->pools = node->next;
                }
                break;
            }
            prev = node;
        }

/*** delete pool and all of its puddles ************************************/

        if (node)
        {
            FPuddle *next = Pool->puddles;
            FPuddle *puddle;

//            Printf("*** DELETE POOL (0x%08lx) FIRST 0x%08lx\n",Pool,Pool->puddles);

            while ((puddle = next) != NULL)
            {
                next = puddle->next;

//                Printf("delete puddle (0x%08lx) next (0x%08lx)\n",puddle,next);

                f_memory_hash_rem(puddle, FeelinBase);

                FreeMem(puddle,puddle->size);
            }

            /* remove semaphore */

            if (Pool -> Semaphore.ss_Link.ln_Pred ||
                Pool -> Semaphore.ss_Link.ln_Succ)
            {
                RemSemaphore(&Pool -> Semaphore);
            }

            FreeMem(Pool,sizeof (FPool));
        }
        else
        {
            F_Log(0,"F_DeletePool() Unknown Pool (0x%08lx)",Pool);
        }
    }

    Permit();

    return 0;
}
//+
///f_new
F_LIB_NEW
{
    return F_NewP(FeelinBase -> DefaultPool,Size);
}
//+
///f_newp
F_LIB_NEWP
{
    if (Pool && Size)
    {
        FMemChunk *chunk = NULL;
        FPuddle *puddle;

        Forbid();

        /* round size to multiple of (uint32) */

        #ifdef F_USE_MEMORY_WALL
        Size = sizeof (FMemChunk) + (((Size + Pool->wall_size * 2 + 3) >> 2) << 2);
        #else
        Size = sizeof (FMemChunk) + (((Size + 3) >> 2) << 2);
        #endif

        for (puddle = Pool -> puddles ; puddle ; puddle = puddle -> next)
        {
//            F_Log(0,"try to create a memory chunk - Pool (0x%08lx) Puddle (0x%08lx)",Pool,puddle);

///try to create a memory chunk

            if (puddle -> free >= Size)
            {
                FMemChunk *from = puddle -> lower;
                FMemChunk *next = puddle -> chunks;
                FMemChunk *prev = NULL;

                for (;;)
                {
                    if (next)
                    {
                        if ((uint32)(from) + Size <= (uint32)(next))
                        {
                            from = (FMemChunk *)((uint32)(next) - Size);

                            break;
                        }
                        else
                        {
                            from = (FMemChunk *)((uint32)(next) + next -> size);

                            prev = next;
                            next = next -> next;
                        }
                    }
                    else
                    {
                        if ((uint32)((uint32)(from) + Size) > (uint32)(puddle->upper))
                        {
                            from = NULL;
                        }
                        else
                        {
                            from = (FMemChunk *)((uint32)(puddle->upper) - Size);
                        }

                        break;
                    }
                }

                chunk = from;

                if (chunk)
                {
                    /* enough space was found for a  new  Drop  (or  memory
                    chunk),  the Drop is created from 'from' and the Puddle
                    is updated */

                    chunk -> next = next;
                    chunk -> size = Size;

                    #ifdef F_USE_PUDDLE_MIDDLE

                    /* The 'middle' chunk is updated if the  new  chunk  is
                    located  in  the second part of the puddle. If there is
                    no 'middle' chunk  defined,  the  new  chunk  is  used,
                    otherwise  the  'middle'  chunk  is  adjusted using the
                    lowest chunk. */

                    if ((uint32)(chunk) >= ((uint32)(puddle->upper) - (uint32)(puddle->lower)) / 2 + (uint32)(puddle->lower))
                    {
                        if (puddle->middle)
                        {
                            if (chunk < puddle->middle)
                            {
                                puddle->middle = chunk;
                            }
                        }
                        else
                        {
                            puddle->middle = chunk;
                        }
                    }
                    #endif

                    if (prev)
                    {
                        prev -> next = chunk;
                    }

                    if (puddle->chunks == next)
                    {
                        puddle->chunks = chunk;
                    }

                    puddle -> free -= Size;
                }
                break;
            }
//+
        }

        if (!chunk)
        {
            uint32 p_size = (Size > Pool -> thresh_size) ? Size : Pool -> puddle_size;
            uint32 h;

/*** create empty puddle & its first chunk *********************************/

            /* We don't need to clear the memory  allocated,  because  each
            drop  (memory  chunk) clear its very own memory space if needed
            e.i. when the flag MEMF_CLEAR is set */

            puddle = AllocMem(sizeof (FPuddle) + p_size,Pool -> flags & ~MEMF_CLEAR);

            if (!puddle)
            {
                Permit(); return NULL;
            }

            /* Because puddle's memory was not cleared when  allocated,  we
            need to set *all* puddle's members. */

            puddle->next = Pool->puddles;
            puddle->prev = NULL;
            puddle->lower = (APTR)((uint32)(puddle) + sizeof (FPuddle));
            puddle->upper = (APTR)((uint32)(puddle) + sizeof (FPuddle) + p_size);
            puddle->chunks = puddle->lower;
            puddle->middle = NULL;
            puddle->size = p_size + sizeof (FPuddle);
            puddle->free = p_size - Size;
            puddle->pool = Pool;

/*** add puddle to the pool's chain ****************************************/

            /* In order for new allocations to find free space as  fast  as
            possible,  new  puddles  are  always  linked at the head of the
            chain. */

            if (Pool->puddles)
            {
                Pool->puddles->prev = puddle;
            }

            Pool->puddles = puddle;

/*** add puddle to the memory hash table ***********************************/

            /* For accurate Chunks searches, 'lower' is used  as  hash  key
            instead  of the address of the puddle, because 'lower' is often
            the first chunk of the puddle. */

            h = F_MEMORY_HASH(puddle->lower);

            puddle->hash_next = FeelinBase->hash_puddles[h];
            FeelinBase->hash_puddles[h] = puddle;

///DB_MEMORY_STATS
            #ifdef DB_MEMORY_STATS
            FeelinBase->numpuddles++; // for performance monitoring
            #endif
//+

/*** create first chunk, it really simple **********************************/

            chunk = puddle->lower;
            chunk->size = Size;
            chunk->next = NULL;
        }

        if (chunk)
        {
            chunk = (APTR)((uint32)(chunk) + sizeof (FMemChunk));

            #ifdef F_USE_MEMORY_WALL
///wall before memory

            if (Pool->wall_size)
            {
                uint32 *mem = (uint32 *) chunk;
                uint32 len = Pool->wall_size;
                uint32 len8;

                len >>= 2;
                len8 = (len >> 3) + 1;

                switch (len & 7)
                {
                    do
                    {
                        *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;

                        case 7:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 6:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 5:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 4:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 3:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 2:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 1:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 0:  len8--;
                    }
                    while (len8);
                }

                chunk = (FMemChunk *) mem;
            }

//+
            #endif

            /* if MEMF_CLEAR is set the chunk is filled with zeros */

            if (MEMF_CLEAR & Pool -> flags)
            {
                uint32 *mem = (uint32 *) chunk;
                #ifdef F_USE_MEMORY_WALL
                uint32 len = Size - sizeof (FMemChunk) - Pool->wall_size * 2;
                #else
                uint32 len = Size - sizeof (FMemChunk);
                #endif
                uint32 len8;

                len >>= 2;
                len8 = (len >> 3) + 1;

                switch (len & 7)
                {
                    do
                    {
                        *mem++ = 0;

                        case 7:  *mem++ = 0;
                        case 6:  *mem++ = 0;
                        case 5:  *mem++ = 0;
                        case 4:  *mem++ = 0;
                        case 3:  *mem++ = 0;
                        case 2:  *mem++ = 0;
                        case 1:  *mem++ = 0;
                        case 0:  len8--;
                    }
                    while (len8);
                }
            }

            #ifdef F_USE_MEMORY_WALL
///wall after memory

            if (Pool->wall_size)
            {
                uint32 *mem = (uint32 *)((uint32)(chunk) + Size - sizeof (FMemChunk) - Pool->wall_size * 2);
                uint32 len = Pool->wall_size;
                uint32 len8;

                len >>= 2;
                len8 = (len >> 3) + 1;

                switch (len & 7)
                {
                    do
                    {
                        *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;

                        case 7:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 6:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 5:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 4:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 3:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 2:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 1:  *mem++ = FV_DEBUG_MEMORY_WALL_PATTERN;
                        case 0:  len8--;
                    }
                    while (len8);
                }
            }
//+
            #endif

            Permit();

///DEBUG_CODE / FF_VERBOSE_NEW
            #ifdef DEBUG_CODE
            if (FF_VERBOSE_NEW & FeelinBase -> verbose_flags)
            {
                F_Log(0,"(vb)F_New() Chunk 0x%08lx - Size %8ld - Pool 0x%08lx (%s)",chunk,Size,Pool,Pool -> Semaphore.ss_Link.ln_Name);
            }
            #endif
//+

            return chunk;
        }
        else
        {
            F_Log(FV_LOG_CORE,"F_NewP() Unable to allocate %ld bytes",Size);
        }
    }
    return NULL;
}
//+
///f_dispose
F_LIB_DISPOSE
{
    if (Mem)
    {
///DB_STATS_DISPOSE
        #ifdef DB_STATS_DISPOSE

        uint32 stats_n = 0;
        uint32 stats_n_prev = 0;

        #endif
//+
        #ifdef F_USE_MEMORY_WALL
        uint32 h = F_MEMORY_HASH(Mem);
        #else
        uint32 h = F_MEMORY_HASH(((uint32)(Mem) - sizeof (FMemChunk)));
        #endif

        FPuddle *puddle = NULL;

        FPuddle **stp = &FeelinBase->hash_puddles[-1];
        FPuddle **pos = &FeelinBase->hash_puddles[h];

        Forbid();

        /* We search the puddle which may contains the  Chunk  between  its
        'lower'  and  'upper'  addresses. Puddles are checked from the hash
        point to the first (zero), because if the Chunk is not in  the  'h'
        puddle  (or  in  the  hash  link  chain) its always before it, very
        really after (not to say never). */

        while (pos != stp)
        {
///DB_STATS_DISPOSE
            #ifdef DB_STATS_DISPOSE
            uint32 stats_link_n = 0;
            #endif
//+

            puddle = *pos--;

            while (puddle)
            {
///DB_STATS_DISPOSE
#ifdef DB_STATS_DISPOSE

                stats_n++;

#endif
//+

                if ((uint32)(Mem) >= (uint32)(puddle->lower) &&
                    (uint32)(Mem) <  (uint32)(puddle->upper))
                {
///DB_STATS_DISPOSE
#ifdef DB_STATS_DISPOSE

                    if (stats_n == 1)
                    {
                        stats.immediate++;
                    }
                    else if (stats_n_prev)
                    {
                        stats.prev++;
                        stats.prev_total_distance += stats_n_prev;
                    }
                    else
                    {
                        stats.link++;
                    }

#endif
//+
                    pos = stp;

                    break;
                }
                else
                {
///DB_STATS_DISPOSE
                    #ifdef DB_STATS_DISPOSE
                    stats_link_n++;
                    #endif
//+
                    puddle = puddle->hash_next;
                }
            }
///DB_STATS_DISPOSE
            #ifdef DB_STATS_DISPOSE
            stats_n_prev++;
            #endif
//+
        }

/*** search in following puddles (very rare) *******************************/

        /* We didn't find the Chunk. Maybe it's after the 'h' puddle, let's
        check it out. */

        if (puddle == NULL)
        {
            stp = &FeelinBase->hash_puddles[FV_MEMORY_HASH_SIZE];
            pos = &FeelinBase->hash_puddles[h + 1];

            while (pos != stp)
            {
                puddle = *pos++;

                while (puddle)
                {
                    if ((uint32)(Mem) >= (uint32)(puddle->lower) &&
                        (uint32)(Mem) <  (uint32)(puddle->upper))
                    {
///DB_STATS_DISPOSE
                        #ifdef DB_STATS_DISPOSE
                        stats.next++;
                        #endif
//+
                        pos = stp;

                        break;
                    }
                    else
                    {
                        puddle = puddle->hash_next;
                    }
                }
            }
        }

/*** remove memory chunk ***************************************************/

        if (puddle)
        {
            #ifdef F_USE_MEMORY_WALL
            FMemChunk *real = (FMemChunk *)((uint32)(Mem) - puddle->pool->wall_size - sizeof (FMemChunk));
            #else
            FMemChunk *real = (FMemChunk *)((uint32)(Mem) - sizeof (FMemChunk));
            #endif
            FMemChunk *prev;
            FMemChunk *node;

            #ifdef F_USE_PUDDLE_MIDDLE

            /* If we are lucky, the Chunk to dispose is  after  the  middle
            one.  This  will  save  us a lot of checkings e.g. half of them
            :-). The middle chunk could be our Chunk, but we cannot use  it
            because we need the previous chunk in order to unlink it. */

            if (puddle->middle != NULL && real > puddle->middle)
            {
                prev = puddle->middle;
                node = puddle->middle->next;
            }
            else
            #endif
            {
                prev = NULL;
                node = puddle->chunks;
            }

            while (node)
            {
                if (node == real)
                {
                    puddle -> free += node -> size;

                    #ifdef F_USE_MEMORY_WALL
///check memory wall integrity
                    if (puddle->pool->wall_size)
                    {
                        uint32 *wall = (uint32 *)((uint32)(Mem) - puddle->pool->wall_size);
                        uint32 i;

                        for (i = 0 ; i < puddle->pool->wall_size / 4 ; i++)
                        {
                            if (*wall != FV_DEBUG_MEMORY_WALL_PATTERN)
                            {
                                F_Log(0,"F_Dispose() Memory wall of Chunk (0x%08lx)(%s) altered at (%ld): 0x%08lx",Mem,puddle->pool->Semaphore.ss_Link.ln_Name,(uint32)(wall) - (uint32)(Mem),*wall);
                            }
                            wall++;
                        }

                        wall = (uint32 *)((uint32)(Mem) - puddle->pool->wall_size - sizeof (FMemChunk) + node->size - puddle->pool->wall_size);

                        for (i = 0 ; i < puddle->pool->wall_size / 4 ; i++)
                        {
                            if (*wall != FV_DEBUG_MEMORY_WALL_PATTERN)
                            {
                                F_Log(0,"F_Dispose() Memory wall of Chunk (0x%08lx)(%s) altered at (%ld): 0x%08lx",Mem,puddle->pool->Semaphore.ss_Link.ln_Name,(uint32)(wall) - (uint32)(Mem),*wall);
                            }
                            wall++;
                        }
                    }
///+
                    #endif

                    #ifdef F_USE_PUDDLE_MIDDLE

                    /* If our Chunk is the middle one, we  set  the  middle
                    chunk  to  the  next chunk, which may be NULL. We don't
                    need to do anything else, since  the  middle  chunk  is
                    adjusted to its best when allocating new item. */

                    if (puddle->middle == node)
                    {
                        puddle->middle = node->next;
                    }
                    #endif

///unlink chunk
                    if (puddle->chunks == node)
                    {
                        puddle->chunks = node->next;
                    }
                    else if (prev)
                    {
                        prev->next = node->next;
                    }
//+
///fill memory chunk

//              #ifdef DB_DISPOSE_FILL
/*  	    	stegerg FIXME/CHECKME broken? */
    	    	#if 1
                {
                    uint32 *mem = (uint32 *) node;
                    uint32 len = node->size;
                    uint32 len8;

                    len >>= 2;
                    len8 = (len >> 3) + 1;

                    switch (len & 7)
                    {
                        do
                        {
                            *mem++ = 0xABADF00D;

                            case 7:  *mem++ = 0xABADF00D;
                            case 6:  *mem++ = 0xABADF00D;
                            case 5:  *mem++ = 0xABADF00D;
                            case 4:  *mem++ = 0xABADF00D;
                            case 3:  *mem++ = 0xABADF00D;
                            case 2:  *mem++ = 0xABADF00D;
                            case 1:  *mem++ = 0xABADF00D;
                            case 0:  len8--;
                        }
                        while (len8);
                    }
                }

                #endif
//+
///free empty puddle
                    if (puddle->chunks == NULL)
                    {
                        f_memory_hash_rem(puddle, FeelinBase);

                        if (puddle->next)
                        {
                            puddle->next->prev = puddle->prev;
                        }

                        if (puddle->pool->puddles == puddle)
                        {
                            puddle->pool->puddles = puddle->next;
                        }
                        else if (puddle->prev)
                        {
                            puddle->prev->next = puddle->next;
                        }

                        FreeMem(puddle,puddle -> size);
                    }
//+

                    break;
                }

                prev = node;
                node = node->next;
            }

            if (node == NULL)
            {
                F_Log(0,"F_Dispose() Unknown memory chunk (0x%08lx). Similar puddle ? please report",Mem);
            }
        }
        else
        {
///DB_STATS_DISPOSE
            #ifdef DB_STATS_DISPOSE
            stats.fucked++;
            #endif
//+
            F_Log(0,"F_Dispose() Unknown memory chunk (0x%08lx)",Mem);
        }

        Permit();
    }
///DB_STATS_DISPOSE
    #ifdef DB_STATS_DISPOSE

    if (stats.immediate > stats.step ||
        stats.link > stats.step ||
        stats.prev > stats.step ||
        stats.next > stats.step ||
        stats.fucked > stats.step)
    {
        F_Log(0,"STATS: i (%ld) l (%ld) p (%ld, dis %ld) n (%ld) f (%ld)",stats.immediate,stats.link,stats.prev,stats.prev_total_distance,stats.next,stats.fucked);

        stats.step += 20;
    }
    #endif
//+

    return 0;
}
//+
///f_disposep
F_LIB_DISPOSEP
{
    F_Log(0,"*** F_DisposeP() is deprecated ***");

    F_Dispose(Mem);

    return 0;
}
//+
///f_opool
F_LIB_OPOOL
{
    if (Pool)
    {
        ObtainSemaphore(&Pool -> Semaphore);
    }
}
//+
///f_spool
F_LIB_SPOOL
{
    if (Pool)
    {
        ObtainSemaphoreShared(&Pool -> Semaphore);
    }
}
//+
///f_rpool
F_LIB_RPOOL
{
    if (Pool)
    {
        ReleaseSemaphore(&Pool -> Semaphore);
    }
}
//+

