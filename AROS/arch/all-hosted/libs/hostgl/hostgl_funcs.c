/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_funcs.h"

#define SETFBATTR(attribute, value) \
    fbattributes[i++] = attribute;  \
    fbattributes[i++] = value;

BOOL HostGL_FillFBAttributes(LONG * fbattributes, LONG size)
{
    LONG i = 0;

#if defined(RENDERER_SEPARATE_X_WINDOW)
    SETFBATTR(GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT)
    SETFBATTR(GLX_DOUBLEBUFFER, True);
#endif

#if defined(RENDERER_PBUFFER_WPA)
    SETFBATTR(GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT)
    SETFBATTR(GLX_DOUBLEBUFFER, False);
#endif

    SETFBATTR(GLX_RENDER_TYPE, GLX_RGBA_BIT)
    SETFBATTR(GLX_RED_SIZE, 1)
    SETFBATTR(GLX_GREEN_SIZE, 1)
    SETFBATTR(GLX_BLUE_SIZE, 1)
    
    SETFBATTR(None, None)

    return TRUE;
}

