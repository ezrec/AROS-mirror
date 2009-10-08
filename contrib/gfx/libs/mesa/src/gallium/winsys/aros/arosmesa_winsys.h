/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "pipe/p_context.h"
#include "../state_trackers/aros/arosmesa_internal.h"

/* FIXME: This is softpipe-only buffer - move to softpipe part */
struct arosmesa_buffer
{
    struct pipe_buffer base;
    void *buffer; /* Real buffer pointer */
    void *data; /* Aligned buffer pointer (inside real buffer) */
    void *mapped;
};

struct arosmesa_driver
{
    struct pipe_screen *(*create_pipe_screen)( void );

    struct pipe_context *(*create_pipe_context)( struct pipe_screen * );

    void (*display_surface)( AROSMesaContext , struct pipe_surface * );
    
    void (*dummy)( struct pipe_screen * );
    
    void (*cleanup)( struct pipe_screen * );
                                    
};

void arosmesa_set_driver (const struct arosmesa_driver * drv);