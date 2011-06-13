/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <proto/exec.h>

#include <aros/libcall.h>

#define DEBUG 1
#include <aros/debug.h>

void *aros_get_relbase(void)
{
    D({
        struct Task *task = FindTask(NULL);
        struct __AROS_RelBase *base;

        if (task == NULL)
            Alert(AN_MemCorrupt);

        base = (struct __AROS_RelBase *)task->tc_SPLower;

        if (!__RELBASE_IS_MAGIC(base))
            Alert(AN_StackProbe);
    })

    return __aros_get_relbase();
}

void *aros_set_relbase(void *libbase)
{
    D({
        struct Task *task = FindTask(NULL);
        struct __AROS_RelBase *base;

        if (task == NULL)
            Alert(AN_MemCorrupt);

        base = (struct __AROS_RelBase *)task->tc_SPLower;

        if (!__RELBASE_IS_MAGIC(base))
            Alert(AN_StackProbe);
    })

    return __aros_set_relbase(libbase);
}

void aros_push_relbase(void *libbase)
{
    D({
        struct Task *task = FindTask(NULL);
        struct __AROS_RelBase *base;

        if (task == NULL)
            Alert(AN_MemCorrupt);

        base = (struct __AROS_RelBase *)task->tc_SPLower;

        if (!__RELBASE_IS_MAGIC(base))
            Alert(AN_StackProbe);

        if (base->depth == 255 ||
            base->depth >= ((task->tc_SPReg - task->tc_SPLower)/sizeof(APTR))
        )
            Alert(AN_StackProbe);
    })

    __aros_push_relbase(libbase);
}

void aros_push2_relbase(void *libbase, void *ptr)
{
    D({
        struct Task *task = FindTask(NULL);
        struct __AROS_RelBase *base;

        if (task == NULL)
            Alert(AN_MemCorrupt);

        base = (struct __AROS_RelBase *)task->tc_SPLower;

        if (__RELBASE_IS_MAGIC(base))
        {
            if (base->depth >= 254 ||
                base->depth >= ((task->tc_SPReg - task->tc_SPLower)/sizeof(APTR))
            )
		Alert(AN_StackProbe);
        }
    })

    __aros_push_relbase(ptr);
    __aros_push_relbase(libbase);
}

void *aros_pop_relbase(void)
{
    return __aros_pop_relbase();
}

void *aros_pop2_relbase(void)
{
    __aros_pop_relbase();
    return __aros_pop_relbase();
}
