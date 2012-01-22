/*
    Copyright 2011-2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_ctx_manager.h"
#include "hostgl_types.h"
#include <proto/exec.h>

/*****************************************************************************

    NAME */

      AROS_LH3(void, AROSMesaGetConfig,

/*  SYNOPSIS */ 
      AROS_LHA(AROSMesaContext, amesa, A0),
      AROS_LHA(GLenum, pname, D0),
      AROS_LHA(GLint *, params, A1),

/*  LOCATION */
      struct Library *, MesaBase, 13, Mesa)

/*  FUNCTION

        Gets value of selected parameter
 
    INPUTS

        pname - enum value of parameter

        params - pointer to integer where the value is to be put

    RESULT

        None
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    Display * dpy = NULL;

    if (amesa == NULL)
    {
        *params = -1;
        return;
    }

    HostGL_Lock();
    HostGL_UpdateGlobalGLXContext();

    dpy = HostGL_GetGlobalX11Display();

    switch(pname)
    {
        case GL_RED_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_RED_SIZE, params);
            break;
        case GL_GREEN_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_GREEN_SIZE, params);
            break;
        case GL_BLUE_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_BLUE_SIZE, params);
            break;
        case GL_ALPHA_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_ALPHA_SIZE, params);
            break;
        case GL_DOUBLEBUFFER:
            *params = 1;
            break;
        case GL_DEPTH_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_DEPTH_SIZE, params);
            break;
        case GL_STENCIL_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_STENCIL_SIZE, params);
            break;
        case GL_ACCUM_RED_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_ACCUM_RED_SIZE, params);
            break;
        case GL_ACCUM_GREEN_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_ACCUM_GREEN_SIZE, params);
            break;
        case GL_ACCUM_BLUE_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_ACCUM_BLUE_SIZE, params);
            break;
        case GL_ACCUM_ALPHA_BITS:
            GLXCALL(glXGetFBConfigAttrib, dpy, amesa->framebuffer->fbconfigs[0], GLX_ACCUM_ALPHA_SIZE, params);
            break;
        case GL_STEREO:
            *params = 0;
            break;
        default:
            *params = -1;
    }

    HostGL_UnLock();

    AROS_LIBFUNC_EXIT
}


