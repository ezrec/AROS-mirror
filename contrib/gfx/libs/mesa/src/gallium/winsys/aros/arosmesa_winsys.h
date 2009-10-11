/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "pipe/p_context.h"
#include "../state_trackers/aros/arosmesa_internal.h"

/* This is the future HIDD interface */
struct arosmesa_driver
{
    struct pipe_screen *(*create_pipe_screen)( void );

    struct pipe_context *(*create_pipe_context)( struct pipe_screen * );

    void (*display_surface)( AROSMesaContext , struct pipe_surface * );
    
    void (*cleanup)( struct pipe_screen * );
    
    /* Return values of depht/stencil buffer bits supported by driver */
    void (*query_depth_stencil)( int color, int * depth, int * stencil );

    void (*protect_visible_screen)( struct pipe_screen * , int, int, int);

};

void arosmesa_set_driver (const struct arosmesa_driver * drv);
