/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_ctx_manager.h"
#include "arosmesa_funcs.h"
#include "hostgl_funcs.h"
#include <proto/exec.h>
#include <aros/debug.h>

/*****************************************************************************

    NAME */

    AROSMesaContext AROSMesaCreateContextTags(

/*  SYNOPSIS */
	long Tag1,
	...)

/*  FUNCTION
        This is the varargs version of mesa.library/AROSMesaCreateContext().
        For information see mesa.library/AROSMesaCreateContext().

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        mesa.library/AROSMesaCreateContext()

    INTERNALS

    HISTORY

*****************************************************************************/
{
  AROS_SLOWSTACKTAGS_PRE(Tag1)
  retval = (IPTR)AROSMesaCreateContext(AROS_SLOWSTACKTAGS_ARG(Tag1));
  AROS_SLOWSTACKTAGS_POST
}



/*****************************************************************************

    NAME */

      AROSMesaContext AROSMesaCreateContext(

/*  SYNOPSIS */ 
      struct TagItem *tagList)

/*  FUNCTION

        Crates a GL rendering context. Whether the rendering will be software
        or hardware based depends on the gallium.library returning a module
        best suited.
 
    INPUTS

        tagList - a pointer to tags to be used during creation.
 
    TAGS

        AMA_Left   - specifies the left rendering offset on the rastport. 
                     Typically equals to window->BorderLeft.

        AMA_Top    - specifies the top rendering offset on the rastport. 
                     Typically equals to window->BorderTop.

        AMA_Right  - specifies the right rendering offset on the rastport. 
                     Typically equals to window->BorderRight.

        AMA_Bottom - specifies the bottom rendering offset on the rastport. 
                     Typically equals to window->BorderBottom.
    
        AMA_Width  - specifies the width of the rendering area. 
                     AMA_Width + AMA_Left + AMA_Right should equal the width of
                     the rastport. The AMA_Width is interchangable at cration 
                     time with AMA_Right. Later durring window resizing, width 
                     is calculated from scalled left, righ and window width.

        AMA_Height - specifies the height of the rendering area. 
                     AMA_Height + AMA_Top + AMA_Bottom should equal the height 
                     of the rastport. The AMA_Height is interchangable at 
                     cration time with AMA_Bottom. Later durring window resizing
                     , height is calculated from scalled top, bottom and window 
                     height.

        AMA_Screen - pointer to Screen onto which scene is to be rendered. When
                     selecting RastPort has lower priority than AMA_Window.

        AMA_Window - pointer to Window onto which scene is to be rendered. Must
                     be provided.

        AMA_RastPort - ignored. Use AMA_Window.

        AMA_DoubleBuf - ignored. All rendering is always double buffered.

        AMA_RGBMode - ignored. All rendering is done in RGB. Indexed modes are 
                      not supported.

        AMA_AlphaFlag - ignored. All rendering is done with alpha channel.

        AMA_NoDepth - disables the depth/Z buffer. Depth buffer is enabled by
                      default and is 16 or 24 bit based on rendering 
                      capabilities.

        AMA_NoStencil - disables the stencil buffer. Stencil buffer is enabled
                        by default.

        AMA_NoAccum - disables the accumulation buffer. Accumulation buffer is
                      enabled by default.

    RESULT

        A valid GL context or NULL of creation was not succesfull.
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    LONG                    screen;
    AROSMesaContext         amesa = NULL;
    LONG                    numreturned;
    Display                 *dsp = NULL;
    const LONG              fbattributessize = 40;
    LONG                    fbattributes[fbattributessize];
#if defined(RENDERER_SEPARATE_X_WINDOW)
    XVisualInfo             *visinfo;
    GLXFBConfig             *windowfbconfigs;
    XSetWindowAttributes    swa;
    LONG                    swamask;
#endif

#if defined(RENDERER_PBUFFER_WPA)
    GLXFBConfig             *pbufferfbconfigs;
    LONG pbufferattributes[] =
    {
        GLX_PBUFFER_WIDTH,   0,
        GLX_PBUFFER_HEIGHT,  0,
        GLX_LARGEST_PBUFFER, False,
        None
    };
#endif

    HostGL_Lock();

    /* Standard AROSMesa initialization */

    /* Allocate arosmesa_context struct initialized to zeros */
    if (!(amesa = (AROSMesaContext)AllocVec(sizeof(struct arosmesa_context), MEMF_PUBLIC | MEMF_CLEAR)))
    {
        D(bug("[HostGL] AROSMesaCreateContext: ERROR - failed to allocate AROSMesaContext\n"));
        goto error_out;
    }
    
    AROSMesaSelectRastPort(amesa, tagList);
    if (!amesa->visible_rp)
    {
        D(bug("[HostGL] AROSMesaCreateContext: ERROR - failed to select visible rastport\n"));
        goto error_out;
    }
    
    AROSMesaStandardInit(amesa, tagList);

    amesa->framebuffer = (struct arosmesa_framebuffer *)AllocVec(sizeof(struct arosmesa_framebuffer), MEMF_PUBLIC | MEMF_CLEAR);
    if (!amesa->framebuffer)
    {
        D(bug("[HostGL] AROSMesaCreateContext: ERROR -  failed to create frame buffer\n"));
        goto error_out;
    }

    AROSMesaRecalculateBufferWidthHeight(amesa);

    if (!HostGL_FillFBAttributes(fbattributes, fbattributessize, tagList))
    {
        D(bug("[HostGL] AROSMesaCreateContext: ERROR -  failed to fill FB attributes\n"));
        goto error_out;
    }

    /* X/GLX initialization */

    /* Get connection with the server */
    dsp = HostGL_GetGlobalX11Display();
    screen = DefaultScreen(dsp);
    
#if defined(RENDERER_SEPARATE_X_WINDOW)
    /* Choose window config */
    windowfbconfigs = GLXCALL(glXChooseFBConfig, dsp, screen, fbattributes, &numreturned);
    
    if (windowfbconfigs == NULL)
    {
        D(bug("[HostGL] AROSMesaCreateContext: ERROR -  failed to retrieve windowfbconfigs\n"));
        goto error_out;
    }
    
    visinfo = GLXCALL(glXGetVisualFromFBConfig, dsp, windowfbconfigs[0]);

    swa.colormap = XCALL(XCreateColormap, dsp, RootWindow(dsp, screen), visinfo->visual, AllocNone);
    swamask = CWColormap;

    /* Create X window */
    amesa->XWindow = XCALL(XCreateWindow, dsp, RootWindow(dsp, screen),
        amesa->left, amesa->top, amesa->framebuffer->width, amesa->framebuffer->height, 0,
        visinfo->depth, InputOutput, visinfo->visual, swamask, &swa);
    
    /* Create GLX window */
    amesa->glXWindow = GLXCALL(glXCreateWindow, dsp, windowfbconfigs[0], amesa->XWindow, NULL);

    /* Map (show) the window */
    XCALL(XMapWindow, dsp, amesa->XWindow);
    
    XCALL(XFlush, dsp);

    /* Create GL context */
    amesa->glXctx = GLXCALL(glXCreateNewContext, dsp, windowfbconfigs[0], GLX_RGBA_TYPE, NULL, True);
#endif

#if defined(RENDERER_PBUFFER_WPA)
    /* Choose window config */
    pbufferfbconfigs = GLXCALL(glXChooseFBConfig, dsp, screen, fbattributes, &numreturned);
    
    if (pbufferfbconfigs == NULL)
    {
        D(bug("[HostGL] AROSMesaCreateContext: ERROR -  failed to retrieve windowfbconfigs\n"));
        goto error_out;
    }

    /* Create GLX Pbuffer */
    pbufferattributes[1] = amesa->framebuffer->width;
    pbufferattributes[3] = amesa->framebuffer->height;
    amesa->glXPbuffer = GLXCALL(glXCreatePbuffer, dsp, pbufferfbconfigs[0], pbufferattributes);
    
    amesa->swapbuffer       = AllocVec(amesa->framebuffer->width * amesa->framebuffer->height * 4, MEMF_ANY);
    amesa->swapbufferline   = AllocVec(amesa->framebuffer->width * 4, MEMF_ANY);

    /* Create GL context */
    amesa->glXctx = GLXCALL(glXCreateNewContext, dsp, pbufferfbconfigs[0], GLX_RGBA_TYPE, NULL, True);
#endif
    
    if (!amesa->glXctx)
    {
        D(bug("[HostGL] AROSMesaCreateContext: ERROR -  failed to create GLX context\n"));
        goto error_out;
    }

    D(bug("[HostGL] TASK: 0x%x, CREATE 0x%x\n", FindTask(NULL), amesa->glXctx));

    HostGL_UnLock();

    return amesa;

error_out:
#if defined(RENDERER_SEPARATE_X_WINDOW)
    if (amesa && amesa->glXWindow) GLXCALL(glXDestroyWindow, dsp, amesa->glXWindow);
    if (amesa && amesa->XWindow) XCALL(XDestroyWindow, dsp, amesa->XWindow);
#endif
#if defined(RENDERER_PBUFFER_WPA)
    if (amesa && amesa->swapbufferline) FreeVec(amesa->swapbufferline);
    if (amesa && amesa->swapbuffer) FreeVec(amesa->swapbuffer);
    if (amesa && amesa->glXPbuffer) GLXCALL(glXDestroyPbuffer, dsp, amesa->glXPbuffer);
#endif
    if (amesa && amesa->framebuffer) FreeVec(amesa->framebuffer);
    if (amesa) AROSMesaDestroyContext(amesa);

    HostGL_UnLock();
    return NULL;
}

