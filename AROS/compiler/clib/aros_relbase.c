/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <exec/alerts.h>
#include <proto/exec.h>

#define RELBASE_MAGIC	0x524c4253	/* RLBS */
struct RelBase {
	ULONG magic;
	ULONG depth;
	APTR  lib[];
};

void *aros_get_relbase(void)
{
	struct Task *task = FindTask(NULL);
	struct RelBase *base;

	if (task == NULL)
		Alert(AN_MemCorrupt);
	
	base = task->tc_SPLower;
	if (base->magic != RELBASE_MAGIC)
		Alert(AN_StackProbe);

	return base->lib[base->depth];
}

void *aros_set_relbase(void *lib)
{
	struct Task *task = FindTask(NULL);
	struct RelBase *base;
	void *old;
	
	if (task == NULL)
		Alert(AN_MemCorrupt);
	
	base = task->tc_SPLower;

	if (base->magic != RELBASE_MAGIC) {
		base->magic = RELBASE_MAGIC;
		base->depth = 0;
		base->lib[0] = NULL;
	}

	old = base->lib[base->depth];
	base->lib[base->depth] = lib;
	
	return old;
}

void aros_push_relbase(void *lib)
{
	struct Task *task = FindTask(NULL);
	struct RelBase *base;

	if (task == NULL)
		Alert(AN_MemCorrupt);
	
	base = task->tc_SPLower;

	if (base->magic != RELBASE_MAGIC)
		Alert(AN_StackProbe);

	if (base->depth >= ((task->tc_SPReg - task->tc_SPLower)/sizeof(APTR)))
		Alert(AN_StackProbe);

	base->lib[++base->depth] = lib;
}

void *aros_pop_relbase(void)
{
	struct Task *task = FindTask(NULL);
	struct RelBase *base;

	if (task == NULL)
		Alert(AN_MemCorrupt);
	
	base = task->tc_SPLower;
	if (base->depth == 0)
		Alert(AN_StackProbe);

	if (base->magic != RELBASE_MAGIC)
		Alert(AN_StackProbe);

	return base->lib[base->depth--];
}
