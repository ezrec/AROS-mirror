/*
 * @(#) $Header$
 *
 * BGUI library
 *
 * (C) Copyright 2000 BGUI Developers Team.
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:20:59  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:47  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:50  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:44  mlemos
 * Initial revision.
 *
 *
 */

#include <proto/exec.h>
#include <exec/memory.h>

#include "include/MemoryPool.h"

typedef struct oBGUIMemoryBlock
{
	struct oBGUIMemoryBlock *NextBlock;
	unsigned long Size;
	char Pad[8];
}
*oBGUIMemoryBlock,**oBGUIMemoryPool;

void *oBGUIPoolAllocate(pool,size)
oBGUI_POOL pool;
unsigned long size;
{
	oBGUIMemoryPool the_pool;
	oBGUIMemoryBlock block;

	if(pool==0
	|| size==0
	|| (block=AllocMem(sizeof(*block)+size,MEMF_PUBLIC))==0)
		return(0);
	the_pool=pool;
	block->NextBlock= *the_pool;
	block->Size=size;
	*the_pool=block;
	return(block+1);
}

void *oBGUIPoolReallocate(pool,memory,size)
oBGUI_POOL pool;
void *memory;
unsigned long size;
{
	oBGUIMemoryPool the_pool;
	oBGUIMemoryBlock block,new_block;

	for(the_pool=pool,block=((oBGUIMemoryBlock)memory)-1;*the_pool && *the_pool!=block;the_pool= &(*the_pool)->NextBlock);
	if(*the_pool==0)
		return(0);
	if(size<block->Size)
		return(block+1);
	if((new_block=AllocMem(sizeof(*block)+size,MEMF_PUBLIC))==0)
		return(0);
	new_block->NextBlock=block->NextBlock;
	new_block->Size=size;
	*the_pool=new_block;
	FreeMem(block,sizeof(*block)+block->Size);
	return(new_block+1);
}

int oBGUIPoolFree(pool,memory)
oBGUI_POOL pool;
void *memory;
{
	oBGUIMemoryPool the_pool;
	oBGUIMemoryBlock block;

	for(the_pool=pool,block=((oBGUIMemoryBlock)memory)-1;*the_pool && *the_pool!=block;the_pool= &(*the_pool)->NextBlock);
	if(*the_pool==0)
		return(0);
	*the_pool=block->NextBlock;
	FreeMem(block,sizeof(*block)+block->Size);
	return(1);
}

oBGUI_POOL oBGUICreatePool()
{
	oBGUIMemoryPool the_pool;

	if((the_pool=AllocMem(sizeof(*the_pool),MEMF_PUBLIC)))
		*the_pool=0;
	return(the_pool);
}

void oBGUIDestroyPool(pool)
oBGUI_POOL pool;
{
	oBGUIMemoryPool the_pool;
	oBGUIMemoryBlock next_block;

	for(the_pool=pool;*the_pool;*the_pool=next_block)
	{
		next_block=(*the_pool)->NextBlock;
		FreeMem(*the_pool,sizeof(**the_pool)+(*the_pool)->Size);
	}
	FreeMem(the_pool,sizeof(*the_pool));
}
