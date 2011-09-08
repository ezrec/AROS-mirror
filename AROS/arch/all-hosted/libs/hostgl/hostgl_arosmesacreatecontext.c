/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_funcs.h"
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
#if defined(RENDERER_SEPARATE_X_WINDOW)
    XVisualInfo             *visinfo;
    GLXFBConfig             *windowfbconfigs;
    LONG                    numreturned;
    XSetWindowAttributes    swa;
    LONG                    swamask;
    LONG windowfbattributes[] = 
    {
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER,  True,
        GLX_RED_SIZE,      1,
        GLX_GREEN_SIZE,    1, 
        GLX_BLUE_SIZE,     1,
        None
    };

#endif

#if defined(RENDERER_PBUFFER_WPA)
    GLXFBConfig             *pbufferfbconfigs;
    LONG                    numreturned;
    LONG pbufferfbattributes[] = 
    {
        GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER,  False,
        GLX_RED_SIZE,      1,
        GLX_GREEN_SIZE,    1, 
        GLX_BLUE_SIZE,     1,
        None
    };
    LONG pbufferattributes[] =
    {
        GLX_PBUFFER_WIDTH,   0,
        GLX_PBUFFER_HEIGHT,  0,
        GLX_LARGEST_PBUFFER, False,
        None
    };
#endif

    /* Standard AROSMesa initialization */

    /* Allocate arosmesa_context struct initialized to zeros */
    if (!(amesa = (AROSMesaContext)AllocVec(sizeof(struct arosmesa_context), MEMF_PUBLIC | MEMF_CLEAR)))
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - failed to allocate AROSMesaContext\n"));
        return NULL;
    }
    
    AROSMesaSelectRastPort(amesa, tagList);
    if (!amesa->visible_rp)
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - failed to select visible rastport\n"));
        goto error_out;
    }
    
    AROSMesaStandardInit(amesa, tagList);

    amesa->framebuffer = (struct arosmesa_framebuffer *)AllocVec(sizeof(struct arosmesa_framebuffer), MEMF_PUBLIC | MEMF_CLEAR);
    if (!amesa->framebuffer)
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to create frame buffer\n"));
        goto error_out;
    }

    AROSMesaRecalculateBufferWidthHeight(amesa);


    /* X/GLX initialization */

    /* Open connection with the server */
    amesa->XDisplay = XCALL(XOpenDisplay, NULL);
    screen = DefaultScreen(amesa->XDisplay);
    
#if defined(RENDERER_SEPARATE_X_WINDOW)
    /* Choose window config */
    windowfbconfigs = GLXCALL(glXChooseFBConfig, amesa->XDisplay, screen, windowfbattributes, &numreturned);
    
    if (windowfbconfigs == NULL)
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to retrieve windowfbconfigs\n"));
        goto error_out;
    }
    
    visinfo = GLXCALL(glXGetVisualFromFBConfig, amesa->XDisplay, windowfbconfigs[0]);

    swa.colormap = XCALL(XCreateColormap, amesa->XDisplay, RootWindow(amesa->XDisplay, screen), visinfo->visual, AllocNone);
    swamask = CWColormap;

    /* Create X window */
    amesa->XWindow = XCALL(XCreateWindow, amesa->XDisplay, RootWindow(amesa->XDisplay, screen),
        amesa->left, amesa->top, amesa->framebuffer->width, amesa->framebuffer->height, 0,
        visinfo->depth, InputOutput, visinfo->visual, swamask, &swa);
    
    /* Create GLX window */
    amesa->glXWindow = GLXCALL(glXCreateWindow, amesa->XDisplay, windowfbconfigs[0], amesa->XWindow, NULL);

    /* Map (show) the window */
    XCALL(XMapWindow, amesa->XDisplay, amesa->XWindow);
    
    XCALL(XFlush, amesa->XDisplay);

    /* Create GL context */
    amesa->glXctx = GLXCALL(glXCreateNewContext, amesa->XDisplay, windowfbconfigs[0], GLX_RGBA_TYPE, NULL, True);
#endif

#if defined(RENDERER_PBUFFER_WPA)
    /* Choose window config */
    pbufferfbconfigs = GLXCALL(glXChooseFBConfig, amesa->XDisplay, screen, pbufferfbattributes, &numreturned);
    
    if (pbufferfbconfigs == NULL)
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to retrieve windowfbconfigs\n"));
        goto error_out;
    }

    /* Create GLX Pbuffer */
    pbufferattributes[1] = amesa->framebuffer->width;
    pbufferattributes[3] = amesa->framebuffer->height;
    amesa->glXPbuffer = GLXCALL(glXCreatePbuffer, amesa->XDisplay, pbufferfbconfigs[0], pbufferattributes);
    
    amesa->swapbuffer       = AllocVec(amesa->framebuffer->width * amesa->framebuffer->height * 4, MEMF_ANY);
    amesa->swapbufferline   = AllocVec(amesa->framebuffer->width * 4, MEMF_ANY);

    /* Create GL context */
    amesa->glXctx = GLXCALL(glXCreateNewContext, amesa->XDisplay, pbufferfbconfigs[0], GLX_RGBA_TYPE, NULL, True);
#endif
    
    if (!amesa->glXctx)
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to create GLX context\n"));
        goto error_out;
    }

    return amesa;

error_out:
#if defined(RENDERER_SEPARATE_X_WINDOW)
    if (amesa->glXWindow) GLXCALL(glXDestroyWindow, amesa->XDisplay, amesa->glXWindow);
    if (amesa->XWindow) XCALL(XDestroyWindow, amesa->XDisplay, amesa->XWindow);
#endif
#if defined(RENDERER_PBUFFER_WPA)
    if (amesa->swapbufferline) FreeVec(amesa->swapbufferline);
    if (amesa->swapbuffer) FreeVec(amesa->swapbuffer);
    if (amesa->glXPbuffer) GLXCALL(glXDestroyPbuffer, amesa->XDisplay, amesa->glXPbuffer);
#endif
    if (amesa->XDisplay) XCALL(XCloseDisplay, amesa->XDisplay);
    if (amesa->framebuffer) FreeVec(amesa->framebuffer);
    if (amesa) AROSMesaDestroyContext(amesa);
    return NULL;
}

