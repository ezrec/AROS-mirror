/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/


#include "arosmesa_winsys.h"

extern struct arosmesa_driver arosmesa_softpipe_driver;
extern struct arosmesa_driver arosmesa_nouveau_driver;

struct arosmesa_driver * current_driver = NULL;

#define USE_NVIDIA_DRIVER 1

struct arosmesa_driver * arosmesa_get_driver( void )
{
    if (current_driver == NULL)
    {
    /* TODO: select driver */

    /* Use softpipe fallback */
    current_driver = &arosmesa_softpipe_driver;

#if USE_NVIDIA_DRIVER == 1
    current_driver = &arosmesa_nouveau_driver;
#endif
    }
    
    /* TODO: one-time initialize driver */
    
    return current_driver;
}

/* FIXME: selection, initialization(+protect visible framebuffer) and cleanup 
 * are one time actions and should be done at global library init / deinit */
