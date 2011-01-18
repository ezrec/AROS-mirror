/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number: 1d51 0x9d510030 to 0x9d5100A0
                                           0x9d5100C0 to 0x9d5100FF

 Copyright (C) 1996-2001 by Gilles Masson
 Copyright (C) 2001-2005 by NList Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

/*
Pool functions for OS2.x which should be part of clib2, but are missing there.

These functions have been taken from the AROS code tree. The semaphore protection
stuff has been removed as it is not needed in this scope. Refer to www.aros.org
for more information about AROS.
*/

#include <exec/memory.h>
#include <exec/alerts.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>

/* Private Pool structure */
struct Pool
{
    struct MinList PuddleList;
    struct MinList BlockList;
    ULONG Requirements;
    ULONG PuddleSize;
    ULONG ThreshSize;
};

struct Block
{
    struct MinNode Node;
    ULONG Size;
};

#define MEMCHUNK_TOTAL		MEM_BLOCKSIZE
#define MEMHEADER_TOTAL		((sizeof(struct MemHeader)+MEMCHUNK_TOTAL-1)&~(MEMCHUNK_TOTAL-1))
#define BLOCK_TOTAL			((sizeof(struct Block)+MEM_BLOCKSIZE-1)&~(MEM_BLOCKSIZE-1))

APTR LibCreatePool(ULONG requirements, ULONG puddleSize, ULONG threshSize)
{
    struct Pool *pool = NULL;

    /* puddleSize must not be smaller than threshSize */
    if(puddleSize >= threshSize)
    {
    	LONG poolstruct_size;

		/* Round puddleSize up to be a multiple of MEMCHUNK_TOTAL. */
		puddleSize = (puddleSize + MEMCHUNK_TOTAL - 1) & ~(MEMCHUNK_TOTAL - 1);

		/*
		    Allocate memory for the Pool structure using the same requirements
		    as for the whole pool (to make it shareable, residentable or
		    whatever is needed).
		*/

		poolstruct_size = sizeof(struct Pool);

		pool=(struct Pool *)AllocMem(poolstruct_size, requirements);
		if(pool != NULL)
		{
		    /* Clear the lists */
		    NewList((struct List *)&pool->PuddleList);
		    NewList((struct List *)&pool->BlockList );

		    /* Set the rest */
		    pool->Requirements = requirements;
		    pool->PuddleSize   = puddleSize;
		    pool->ThreshSize   = threshSize;
		}

    } /* if(puddleSize >= threshSize) */

    return (APTR)pool;
}

void LibDeletePool(APTR poolHeader)
{
    struct Pool *pool = (struct Pool *)poolHeader;

    /* It is legal to DeletePool(NULL) */
    if(pool != NULL)
    {
		struct Block 	*bl;
		void 	    	*p;
		ULONG 	    	size;

		/* Calculate the total size of a puddle including header. */
		size = pool->PuddleSize + MEMHEADER_TOTAL;

		/* Free the list of puddles */
		while((p = RemHead((struct List *)&pool->PuddleList)) != NULL)
		{
		    FreeMem(p, size);
	    }

		/* Free the list of single Blocks */
		while((bl = (struct Block *)RemHead((struct List *)&pool->BlockList)) != NULL)
		{
		    FreeMem(bl, bl->Size);
	    }

		FreeMem(pool, sizeof(struct Pool));
    }
}

APTR LibAllocPooled(APTR poolHeader, ULONG memSize)
{
    struct Pool *pool = (struct Pool *)poolHeader;
    APTR ret = NULL;

    /* If the memSize is bigger than the ThreshSize allocate seperately. */
    if(memSize > pool->ThreshSize)
    {
		struct Block *bl;
		ULONG 	     size;

		/* Get enough memory for the memory block including the header. */
		size = memSize + BLOCK_TOTAL;
		bl = (struct Block *)AllocMem(size, pool->Requirements);

		/* No memory left */
		if(bl == NULL)
		    goto done;

		/* Initialize the header */
		bl->Size = size;

		/* Add the block to the BlockList */
		AddHead((struct List *)&pool->BlockList,(struct Node *)&bl->Node);

		/* Set pointer to allocated memory */
		ret = (UBYTE *)bl + BLOCK_TOTAL;
    }
    else
    {
	struct MemHeader *mh;

	/* Follow the list of MemHeaders */
	mh = (struct MemHeader *)pool->PuddleList.mlh_Head;
	for(;;)
	{
	    /* Are there no more MemHeaders? */
	    if(mh->mh_Node.ln_Succ==NULL)
	    {
			/* Get a new one */
			mh = (struct MemHeader *)
			   AllocMem(pool->PuddleSize + MEMHEADER_TOTAL,
			    	    pool->Requirements);

			/* No memory left? */
			if(mh == NULL)
			    goto done;

			/* Initialize new MemHeader */
			mh->mh_First	    	= (struct MemChunk *)((UBYTE *)mh + MEMHEADER_TOTAL);
			mh->mh_First->mc_Next 	= NULL;
			mh->mh_First->mc_Bytes  = pool->PuddleSize;
			mh->mh_Lower 	    	= mh->mh_First;
			mh->mh_Upper 	    	= (UBYTE *)mh->mh_First+pool->PuddleSize;
			mh->mh_Free  	    	= pool->PuddleSize;

			/* And add it to the list */
			AddHead((struct List *)&pool->PuddleList, (struct Node *)&mh->mh_Node);
			/* Fall through to get the memory */
	    }
	    /* Try to get the memory */
	    ret = Allocate(mh, memSize);

	    /* Got it? */
	    if(ret != NULL)
			break;

	    /* No. Try next MemHeader */
	    mh = (struct MemHeader *)mh->mh_Node.ln_Succ;
	}

	/* Allocate does not clear the memory! */
	if(pool->Requirements & MEMF_CLEAR)
	{
	    ULONG *p= (ULONG *)ret;

	    /* Round up (clearing longs is faster than just bytes) */
	    memSize = (memSize + sizeof(ULONG) - 1) / sizeof(ULONG);

	    /* NUL the memory out */
	    while(memSize--)
			*p++=0;
	}
    }

done:
    /* Everything fine */
    return ret;
}

void LibFreePooled(APTR poolHeader, APTR memory, ULONG memSize)
{
    struct Pool *pool = (struct Pool *)poolHeader;

    if (!memory || !memSize)
    	return;

    /* If memSize is bigger than the ThreshSize it's allocated seperately. */
    if(memSize > pool->ThreshSize)
    {
		struct Block *bl;

		/* Get pointer to header */
		bl = (struct Block *)((UBYTE *)memory - BLOCK_TOTAL);

		/* Remove it from the list */
		Remove((struct Node *)&bl->Node);

		/* And Free the memory */
		FreeMem(bl, bl->Size);
    }
    else
    {
		/* Look for the right MemHeader */
		struct MemHeader *mh = (struct MemHeader *)pool->PuddleList.mlh_Head;

		for(;;)
		{
		    if (!mh)
		    {
				/* memory block is not in pool. */
				Alert(AT_Recovery | AN_MemCorrupt);
				break;
		    }

		    /* The memory must be between the two borders */
		    if(memory >= mh->mh_Lower && memory < mh->mh_Upper)
		    {
				/* Found the MemHeader. Free the memory. */
				Deallocate(mh, memory, memSize);

				/* Is this MemHeader completely free now? */
				if(mh->mh_Free == pool->PuddleSize)
				{
				    /* Yes. Remove it from the list. */
				    Remove(&mh->mh_Node);

				    /* And free it. */
				    FreeMem(mh, pool->PuddleSize + MEMHEADER_TOTAL);
				}
				/* All done. */
				break;
		    }
		    /* Try next MemHeader */
		    mh = (struct MemHeader *)mh->mh_Node.ln_Succ;
		}
    }
}

