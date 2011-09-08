/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef HOSTGL_TYPES_H
#define HOSTGL_TYPES_H

#include <GL/arosmesa.h>

#include "x11_hostlib.h"
#include "glx_hostlib.h"

#define RENDERER_SEPARATE_X_WINDOW

struct arosmesa_framebuffer
{
    ULONG                       width;
    ULONG                       height;
    BOOL                        resized;
};

/* AROSMesa context */
struct arosmesa_context
{
    Display     *XDisplay;
#if defined(RENDERER_SEPARATE_X_WINDOW)
    Window      XWindow;
    GLXWindow   glXWindow;
#endif
    GLXContext  glXctx;

    struct arosmesa_framebuffer *framebuffer;

    struct Window               *window;                /* Intuition window */
    struct Screen               *Screen;
    ULONG                       BitsPerPixel;
    
    /* Rastport 'visible' to user (window rasport, screen rastport)*/
    struct RastPort             *visible_rp;
    /* Rastport dimentions */
    ULONG                       visible_rp_width;       /* the rastport drawing area full size*/
    ULONG                       visible_rp_height;      /* the rastport drawing area full size*/

    /* Buffer information */
    ULONG                      top, bottom;            /* offsets due to window border */
    ULONG                      left, right;            /* offsets due to window border */    
};

#endif /* HOSTGL_INTERNAL_H */
