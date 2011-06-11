/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <aros/libcall.h>

void *aros_get_relbase(void)
{
    return AROS_GET_RELBASE;
}

void *aros_set_relbase(void *libbase)
{
    return AROS_SET_RELBASE(libbase);
}

void aros_push_relbase(void *libbase)
{
    AROS_PUSH_RELBASE(libbase);
}

void aros_push2_relbase(void *ptr, void *libbase)
{
    AROS_PUSH_RELBASE(ptr);
    AROS_PUSH_RELBASE(libbase);
}

void *aros_pop_relbase(void)
{
    return AROS_POP_RELBASE;
}

void *aros_pop2_relbase(void)
{
    AROS_POP_RELBASE;
    return AROS_POP_RELBASE;
}
