/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/


#include "arosmesa_internal.h"

#include <exec/memory.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>

#include "aros_driver_functions.h"
#include "aros_rb_functions.h"
#include "aros_fb_functions.h"
#include "aros_visual_functions.h"
#include "aros_context_functions.h"
#include "aros_swrast_functions.h"

#include <aros/debug.h>
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>

#include <graphics/rpattr.h>

#include <GL/arosmesa.h>

#include <stdlib.h>
#include <stdio.h>

#include "glheader.h"
#include <GL/gl.h>
#include "context.h"
#include "extensions.h"
#include "framebuffer.h"
#include "swrast/swrast.h"
#include "swrast_setup/swrast_setup.h"
#include "tnl/tnl.h"
#include "tnl/t_context.h"
#include "tnl/t_pipeline.h"
#include "drivers/common/driverfuncs.h"
#include "vbo/vbo.h"

#include "renderbuffer.h"

/**********************************************************************/
/*****        Internal Data                     *****/
/**********************************************************************/

GLenum         LastError;                       /* The last error generated*/
struct Library * AROSMesaCyberGfxBase = NULL;    /* Base address for cybergfx */

/**********************************************************************/
/*****          AROS/Mesa API Functions          *****/
/**********************************************************************/
/*
 * Implement the client-visible AROS/Mesa interface functions defined
 * in Mesa/include/GL/AROSmesa.h
 *
 **********************************************************************/
/*
 * Implement the public AROS/Mesa interface functions defined
 * in Mesa/include/GL/AROSMesa.h
 */

//UBYTE Version[]="$VER: AROSMesa (linklib) 3.4.2";




/* Main AROSMesa API Functions */
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <cybergraphx/cybergraphics.h>

#if (AROS_BIG_ENDIAN == 1)
#define AROS_PIXFMT RECTFMT_ARGB32   /* Big Endian Archs. */
#else
#define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
#endif

static BOOL
aros_Standard_init(AROSMesaContext amesa, struct TagItem *tagList)
{
	struct Rectangle rastcliprect;
	struct TagItem crptags[] =
	{
	    { RPTAG_ClipRectangle      , (IPTR)&rastcliprect },
	    { RPTAG_ClipRectangleFlags , (RPCRF_RELRIGHT | RPCRF_RELBOTTOM) },
	    { TAG_DONE }
	};

    D(bug("[AROSMESA] aros_Standard_init(amesa @ %x, taglist @ %x)\n", amesa, tagList));

    if (!(amesa->visible_rp))
    {
        D(bug("[AROSMESA] aros_Standard_init: WARNING - NULL RastPort in context\n"));
        if (!(amesa->visible_rp = (struct RastPort *)GetTagData(AMA_RastPort, 0, tagList)))
        {
            D(bug("[AROSMESA] aros_Standard_init: ERROR - Launched with NULL RastPort\n"));
            return FALSE;
        }
    }	  
    
    D(bug("[AROSMESA] aros_Standard_init: Using RastPort @ %x\n", amesa->visible_rp));

    amesa->visible_rp = CloneRastPort(amesa->visible_rp);

    D(bug("[AROSMESA] aros_Standard_init: Cloned RastPort @ %x\n", amesa->visible_rp));

    amesa->visible_rp_width = 
        amesa->visible_rp->Layer->bounds.MaxX - amesa->visible_rp->Layer->bounds.MinX + 1;

    amesa->visible_rp_height = 
        amesa->visible_rp->Layer->bounds.MaxY - amesa->visible_rp->Layer->bounds.MinY + 1;

    amesa->left = GetTagData(AMA_Left, 0, tagList);
    amesa->right = GetTagData(AMA_Right, 0, tagList);
    amesa->top = GetTagData(AMA_Top, 0, tagList);
    amesa->bottom = GetTagData(AMA_Bottom, 0, tagList);

    amesa->width = GetTagData(AMA_Width, (amesa->visible_rp_width - amesa->left - amesa->right), tagList);
    amesa->height = GetTagData(AMA_Height, (amesa->visible_rp_height - amesa->top - amesa->bottom), tagList);

    amesa->clearpixel = 0;   /* current drawing/clearing pens */

    if (amesa->window)
    {
        D(bug("[AROSMESA] aros_Standard_init: Clipping Rastport to Window's dimensions\n"));
        /* Clip the rastport to the visible area */
        rastcliprect.MinX = amesa->left;
        rastcliprect.MinY = amesa->top;
        rastcliprect.MaxX = amesa->left + amesa->width;
        rastcliprect.MaxY = amesa->top + amesa->height;
        SetRPAttrsA(amesa->visible_rp, crptags);
    }

    D(bug("[AROSMESA] aros_Standard_init: Context Base dimensions set -:\n"));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->visible_rp_width    = %d\n", amesa->visible_rp_width));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->visible_rp_height   = %d\n", amesa->visible_rp_height));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->width               = %d\n", amesa->width));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->height              = %d\n", amesa->height));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->left                = %d\n", amesa->left));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->right               = %d\n", amesa->right));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->top                 = %d\n", amesa->top));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->bottom              = %d\n", amesa->bottom));
    D(bug("[AROSMESA] aros_Standard_init:    amesa->depth               = %d\n", amesa->depth));

    if (amesa->Screen)
    {
        if (amesa->depth == 0)
        {
            D(bug("[AROSMESA] aros_Standard_init: WARNING - Illegal RastPort Depth, attempting to correct\n"));
            amesa->depth = GetCyberMapAttr(amesa->visible_rp->BitMap, CYBRMATTR_DEPTH);
        }
    }

    return TRUE;
}


AROSMesaContext AROSMesaCreateContextTags(long Tag1, ...)
{
  return AROSMesaCreateContext((struct TagItem *)&Tag1);
}

AROSMesaContext AROSMesaCreateContext(struct TagItem *tagList)
{
    AROSMesaContext amesa = NULL;
    GLcontext * ctx = NULL;
    LastError = 0;
    struct dd_function_table functions;
    GLboolean db_flag = GL_FALSE;


    D(bug("[AROSMESA] AROSMesaCreateContext(taglist @ %x)\n", tagList));

    /* try to open cybergraphics.library */
    if (CyberGfxBase == NULL)
    {
        if (!(CyberGfxBase = OpenLibrary((UBYTE*)"cybergraphics.library",0)))
            return NULL;
    }

    if (!(amesa = (AROSMesaContext)GetTagData(AMA_Context, GL_FALSE, tagList)))
    {
        

        /* allocate arosmesa_context struct initialized to zeros */
        if (!(amesa = (AROSMesaContext)AllocVec(sizeof(struct arosmesa_context), MEMF_PUBLIC|MEMF_CLEAR)))
        {
            D(bug("[AROSMESA] AROSMesaCreateContext: Failed to allocate AROSMesaContext\n"));
            LastError = AMESA_OUT_OF_MEM;
            return NULL;
        }
        
        ctx = GET_GL_CTX_PTR(amesa);

        D(bug("[AROSMESA] AROSMesaCreateContext: AROSMesaContext Allocated @ %x\n", amesa));
        D(bug("[AROSMESA] AROSMesaCreateContext:          gl_ctx Allocated @ %x\n", ctx));

        /* CHANGE TO WORK EXCLUSIVELY WITH RASTPORTS! */
        amesa->Screen = (struct Screen *)GetTagData(AMA_Screen, 0, tagList);
        amesa->window = (struct Window *)GetTagData(AMA_Window, 0, tagList);
        amesa->visible_rp = (struct RastPort *)GetTagData(AMA_RastPort, 0, tagList);

        if (amesa->Screen)
        {
            D(bug("[AROSMESA] AROSMesaCreateContext: Screen @ %x\n", amesa->Screen));
            if (amesa->window)
            {
                D(bug("[AROSMESA] AROSMesaCreateContext: window @ %x\n", amesa->window));
                if (!(amesa->visible_rp))
                {
                    /* Use the windows rastport */
                    amesa->visible_rp = amesa->window->RPort;
                    D(bug("[AROSMESA] AROSMesaCreateContext: Windows RastPort @ %x\n", amesa->visible_rp));
                }
            }
            else
            {
                if (!(amesa->visible_rp))
                {
                    /* Use the screens rastport */
                    amesa->visible_rp = &amesa->Screen->RastPort;
                    D(bug("[AROSMESA] AROSMesaCreateContext: Screens RastPort @ %x\n", amesa->visible_rp));
                }
            }
        }
        else
        {
            /* Not passed a screen */
            if (amesa->window)
            {
                D(bug("[AROSMESA] AROSMesaCreateContext: Window @ %x\n", amesa->window));
                /* Use the windows Screen */
                amesa->Screen = amesa->window->WScreen;
                D(bug("[AROSMESA] AROSMesaCreateContext: Windows Screen @ %x\n", amesa->Screen));

                if (!(amesa->visible_rp))
                {
                    /* Use the windows rastport */
                    amesa->visible_rp = amesa->window->RPort;
                    D(bug("[AROSMESA] AROSMesaCreateContext: Windows RastPort @ %x\n", amesa->visible_rp));
                }
            }
            else
            {
    		    /* Only Passed A Rastport */
                D(bug("[AROSMESA] AROSMesaCreateContext: Experimental: Using RastPort only!\n"));
            }
        }

        D(bug("[AROSMESA] AROSMesaCreateContext: Using RastPort @ %x\n", amesa->visible_rp));

        D(bug("[AROSMESA] AROSMesaCreateContext: Creating new AROSMesaVisual\n"));
        if (((BOOL)GetTagData(AMA_DoubleBuf, TRUE, tagList) == TRUE))
            db_flag = GL_TRUE;

        if (!(amesa->visual = aros_new_visual(db_flag)))
        {
            D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to create AROSMesaVisual\n"));
            FreeVec( amesa );
            LastError = AMESA_OUT_OF_MEM;
            return NULL;
        }

        D(bug("[AROSMESA] AROSMesaCreateContext: [ASSERT] RastPort @ %x\n", amesa->visible_rp));

        /* build table of device driver functions */
        _mesa_init_driver_functions(&functions);
        _aros_init_driver_functions(&functions);


   

        if (!_mesa_initialize_context(  ctx,
                                        GET_GL_VIS_PTR(amesa->visual),
                                        NULL,
                                        &functions,
                                        (void *) amesa))
        {
	        aros_delete_visual(amesa->visual);
            FreeVec(amesa);
            return NULL;
        }

        _mesa_enable_sw_extensions(ctx);
        _mesa_enable_1_3_extensions(ctx);
        _mesa_enable_1_4_extensions(ctx);
        _mesa_enable_1_5_extensions(ctx);
        _mesa_enable_2_0_extensions(ctx);
        _mesa_enable_2_1_extensions(ctx);


        if ((aros_Standard_init(amesa, tagList)) == FALSE)
        {
            goto amccontextclean;
        }

  
        if(!(amesa->framebuffer = aros_new_framebuffer(GET_GL_VIS_PTR(amesa->visual))))
        {
            LastError = AMESA_OUT_OF_MEM;
            goto amccontextclean;
        }
    
        amesa->renderbuffer = aros_new_renderbuffer();   

        _mesa_add_renderbuffer(GET_GL_FB_PTR(amesa->framebuffer), BUFFER_FRONT_LEFT, 
            GET_GL_RB_PTR(amesa->renderbuffer));
        
        /* Set draw buffer as front */
        ctx->Color.DrawBuffer[0] = GL_FRONT;


        /* AROSMesa is always "double buffered" */
        /* if (amesa->visual->db_flag == GL_TRUE) */

        _mesa_add_soft_renderbuffers(GET_GL_FB_PTR(amesa->framebuffer),
                                   GL_FALSE, /* color */
                                   (GET_GL_VIS_PTR(amesa->visual))->haveDepthBuffer,
                                   (GET_GL_VIS_PTR(amesa->visual))->haveStencilBuffer,
                                   (GET_GL_VIS_PTR(amesa->visual))->haveAccumBuffer,
                                   GL_FALSE, /* alpha */
                                   GL_FALSE /* aux */ );


    } /* if(!(amesa = (AROSMesaContext)GetTagData(AMA_Context, GL_FALSE, tagList))) */
 
    ctx = GET_GL_CTX_PTR(amesa);

    _swrast_CreateContext(ctx);
    _vbo_CreateContext(ctx);
    _tnl_CreateContext(ctx);
    _swsetup_CreateContext(ctx);

    aros_swrast_initialize(ctx);

    _swsetup_Wakeup(ctx);


    /* use default TCL pipeline */
    TNL_CONTEXT(ctx)->Driver.RunPipeline = _tnl_run_pipeline;

    return amesa;

amccontextclean:
    if (amesa->visual)
        aros_delete_visual(amesa->visual);

    aros_delete_context(amesa);

    return NULL;
}












#if defined (AROS_MESA_SHARED)
AROS_LH1(void, AROSMesaMakeCurrent,
    AROS_LHA(AROSMesaContext, amesa, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT
    
    PUT_MESABASE_IN_REG
#else
void AROSMesaMakeCurrent(AROSMesaContext amesa)
{
#endif
    
    /* Make the specified context the current one */
    /* the order of operations here is very important! */
    D(bug("[AROSMESA] AROSMesaMakeCurrent(amesa @ %x)\n", amesa));


    if (amesa)
    {
        struct gl_framebuffer * fb = GET_GL_FB_PTR(amesa->framebuffer);
        GLcontext * ctx = GET_GL_CTX_PTR(amesa);
        _glapi_check_multithread();

        //osmesa->rb->Width = osmesa->rb->Height = 0;

        /* Set the framebuffer's size.  This causes the
        * osmesa_renderbuffer_storage() function to get called.
        */
        _mesa_resize_framebuffer(ctx, fb, amesa->width, amesa->height);
        fb->Initialized = GL_TRUE; /* XXX TEMPORARY? */

        _mesa_make_current(ctx, fb, fb);

        /* Remove renderbuffer attachment, then re-add.  This installs the
        * renderbuffer adaptor/wrapper if needed (for bpp conversion).
        */

        /* this updates the visual's red/green/blue/alphaBits fields */
        _mesa_update_framebuffer_visual(fb);

        /* update the framebuffer size */
        _mesa_resize_framebuffer(ctx, fb, amesa->width, amesa->height);


        D(bug("[AROSMESA] AROSMesaMakeCurrent: set current mesa context/buffer\n"));

        D(bug("[AROSMESA] AROSMesaMakeCurrent: initialised rasterizer driver functions\n"));
    }
#if defined (AROS_MESA_SHARED)
    AROS_LIBFUNC_EXIT
#endif
}

#if defined (AROS_MESA_SHARED)
AROS_LH1(void, AROSMesaSwapBuffers,
    AROS_LHA(AROSMesaContext, amesa, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT
    
    PUT_MESABASE_IN_REG
#else
void AROSMesaSwapBuffers(AROSMesaContext amesa)
{        
#endif
    /* copy/swap back buffer to front if applicable */

    D(bug("[AROSMESA] AROSMesaSwapBuffers(amesa @ %x)\n", amesa));

    WritePixelArray(
        amesa->renderbuffer->buffer, 
        0,
        0,
        4 * GET_GL_RB_PTR(amesa->renderbuffer)->Width, 
        amesa->visible_rp, 
        amesa->left, 
        amesa->top, 
        amesa->width, 
        amesa->height, 
        AROS_PIXFMT);

#if defined (AROS_MESA_SHARED)
    AROS_LIBFUNC_EXIT
#endif
}


#if defined (AROS_MESA_SHARED)
AROS_LH1(void, AROSMesaDestroyContext,
    AROS_LHA(AROSMesaContext, amesa, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT
    
    PUT_MESABASE_IN_REG
#else
void AROSMesaDestroyContext(AROSMesaContext amesa)
{
#endif
    /* destroy a AROS/Mesa context */
    D(bug("[AROSMESA] AROSMesaDestroyContext(amesa @ %x)\n", amesa));

    if (!amesa)
        return;

    GLcontext * ctx = GET_GL_CTX_PTR(amesa);

    if (ctx)
    {
        _mesa_make_current(NULL, NULL, NULL);


        _swsetup_DestroyContext(ctx);
        _swrast_DestroyContext(ctx);
        _tnl_DestroyContext(ctx);
        _vbo_DestroyContext(ctx);


        aros_delete_visual(amesa->visual);
        aros_delete_framebuffer(amesa->framebuffer);

        aros_delete_renderbuffer(amesa->renderbuffer);

        aros_delete_context(amesa);
    }
#if defined (AROS_MESA_SHARED)
    AROS_LIBFUNC_EXIT
#endif
}


#if defined (AROS_MESA_SHARED)
AROS_LH0(AROSMesaContext, AROSMesaGetCurrentContext,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT
    
    PUT_MESABASE_IN_REG
#else
AROSMesaContext AROSMesaGetCurrentContext()
{
#endif
    GET_CURRENT_CONTEXT(ctx);
    
    return ctx;

#if defined (AROS_MESA_SHARED)
    AROS_LIBFUNC_EXIT
#endif
}
