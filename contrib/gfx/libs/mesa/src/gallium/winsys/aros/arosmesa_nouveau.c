/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_winsys.h"
#include "nouveau/nouveau_winsys.h"

#include <proto/exec.h>

static struct pipe_winsys *
arosmesa_create_nouveau_pipe_winsys( void )
{
    struct pipe_winsys *ws = NULL;
    
    ws = (struct pipe_winsys *)AllocVec(sizeof(struct pipe_winsys), MEMF_PUBLIC|MEMF_CLEAR);
    
    if (!ws)
        return NULL;
    
    /* FIXME: add functions to winsys */
    
    return ws;
}

static struct nouveau_winsys *
arosmesa_create_nouveau_winsys( struct pipe_winsys * ws )
{
    struct nouveau_winsys *nvws = NULL;
    
    nvws = (struct nouveau_winsys *)AllocVec(sizeof(struct nouveau_winsys), MEMF_PUBLIC|MEMF_CLEAR);
    
    if (!nvws)
        return NULL;
    
    nvws->ws = ws;
    
    /* FIXME: add functions to winsys */
    
    return nvws;
}

static struct pipe_screen *
arosmesa_create_nouveau_screen( void )
{
    struct pipe_winsys *ws = NULL;
    struct nouveau_winsys *nvws = NULL;
    struct pipe_screen *screen = NULL;

    ws = arosmesa_create_nouveau_pipe_winsys();
    if (!ws) 
    {
        /* FIXME: implement deallocation */
        return NULL;
    }
    
    nvws = arosmesa_create_nouveau_winsys(ws);
    if (!ws) 
    {
        /* FIXME: implement deallocation */
        return NULL;
    }
    
    screen = nv40_screen_create(ws, nvws);
    if (!screen)
    {
        /* FIXME: implement deallocation */
        return NULL;
    }

    return screen;
}

static struct pipe_context *
arosmesa_create_nouveau_context( struct pipe_screen *screen )
{
    struct pipe_context *pipe;

    pipe = nv40_create(screen, 0); /* FIXME: correct second parameter */
    
    if (!pipe)
    {
        /* FIXME: implement deallocation */
        return NULL;
    }

    return pipe;
}
struct arosmesa_driver arosmesa_nouveau_driver = 
{
   .create_pipe_screen = arosmesa_create_nouveau_screen,
   .create_pipe_context = arosmesa_create_nouveau_context,
   .display_surface = NULL
};



