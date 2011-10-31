/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/utility.h>
#include "hostgl_funcs.h"

#define SETFBATTR(attribute, value)     \
    {                                   \
        fbattributes[i++] = attribute;  \
        fbattributes[i++] = value;      \
    }

BOOL HostGL_FillFBAttributes(LONG * fbattributes, LONG size, struct TagItem *tagList)
{
    LONG i = 0;
    BOOL noDepth, noStencil, noAccum;
    noStencil   = GetTagData(AMA_NoStencil, GL_FALSE, tagList);
    noAccum     = GetTagData(AMA_NoAccum, GL_FALSE, tagList);
    noDepth     = GetTagData(AMA_NoDepth, GL_FALSE, tagList);

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
    SETFBATTR(GLX_ALPHA_SIZE, 0)

    if (noDepth)
        SETFBATTR(GLX_DEPTH_SIZE, 0)
    else
        SETFBATTR(GLX_DEPTH_SIZE, 24)

    if (noStencil)
        SETFBATTR(GLX_STENCIL_SIZE, 0)
    else
        SETFBATTR(GLX_STENCIL_SIZE, 8)

    if (noAccum)
    {
        SETFBATTR(GLX_ACCUM_RED_SIZE, 0)
        SETFBATTR(GLX_ACCUM_GREEN_SIZE, 0)
        SETFBATTR(GLX_ACCUM_BLUE_SIZE, 0)
        SETFBATTR(GLX_ACCUM_ALPHA_SIZE, 0)
    }
    else
    {
        SETFBATTR(GLX_ACCUM_RED_SIZE, 16)
        SETFBATTR(GLX_ACCUM_GREEN_SIZE, 16)
        SETFBATTR(GLX_ACCUM_BLUE_SIZE, 16)
        SETFBATTR(GLX_ACCUM_ALPHA_SIZE, 16)
    }

    SETFBATTR(None, None)

    return TRUE;
}

