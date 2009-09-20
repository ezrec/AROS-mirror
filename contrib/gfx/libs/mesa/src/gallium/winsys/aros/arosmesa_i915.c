/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_winsys.h"


static struct pipe_context *
arosmesa_create_i915_context( struct pipe_screen *screen )
{
    struct pipe_context *pipe;

    /* FIXME: implement */
    
    if (pipe == NULL)
        goto fail;

    return pipe;

fail:
    /* FIXME: Free stuff here */
    return NULL;
}

struct arosmesa_driver arosmesa_i915_driver = 
{
   .create_pipe_screen = NULL,
   .create_pipe_context = arosmesa_create_i915_context,
   .display_surface = NULL
};



