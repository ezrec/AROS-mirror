/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/


#include "arosmesa_internal.h"

#include "main/context.h"
#include "state_tracker/st_public.h"
#include "util/u_inlines.h"

#include <proto/exec.h>
#include <proto/utility.h>
#define DEBUG 0
#include <aros/debug.h>

#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/graphics.h>
#include <graphics/rpattr.h>
#include <proto/oop.h>
#include <hidd/gallium.h>

struct Library * AROSMesaCyberGfxBase = NULL;    /* Base address for cybergfx */
struct Library * AROSMesaOOPBase = NULL;
struct Library * AROSMesaHIDDGalliumBase = NULL;
#define OOPBase AROSMesaOOPBase
OOP_Object * driver = NULL;

/*****************************************************************************/
/*                             PRIVATE FUNCTIONS                             */
/*****************************************************************************/

static void 
aros_destroy_visual(AROSMesaVisual aros_vis)
{
    if (aros_vis)
    {
        FreeVec(aros_vis);
    }
}

static AROSMesaVisual 
aros_new_visual(GLint bpp, struct TagItem *tagList)
{
    AROSMesaVisual aros_vis = NULL;
    GLvisual * vis = NULL;
    GLint  indexBits, redBits, greenBits, blueBits, alphaBits, accumBits;
    UBYTE depthBits, stencilBits;
    struct pHidd_GalliumBaseDriver_QueryDepthStencil qdsmsg = {
    mID : OOP_GetMethodID(IID_Hidd_GalliumBaseDriver, moHidd_GalliumBaseDriver_QueryDepthStencil),
    depthbits : &depthBits,
    stencilbits : &stencilBits
    };
    
    D(bug("[AROSMESA] aros_new_visual\n"));

    /* Color buffer */
    switch(bpp)
    {
        case(16):
            indexBits   = 0;
            redBits     = 5;
            greenBits   = 6;
            blueBits    = 5;
            alphaBits   = 0;
            break;
        case(32):
            indexBits   = 0;
            redBits     = 8;
            greenBits   = 8;
            blueBits    = 8;
            alphaBits   = 8;
            break;
        default:
            D(bug("[AROSMESA] aros_new_visual - ERROR - Unsupported bpp\n"));
            return NULL;
    }

    /* Z-buffer / Stencil buffer */
    OOP_DoMethod(driver, (OOP_Msg)&qdsmsg);
    
    /* Accum buffer */
    accumBits = 16;
    
    
    /* Override default values */
    /* AMA_RGBMode, AMA_DoubleBuf and AMA_AlphaFlag are always GL_TRUE in this implementation */
    stencilBits     = !GetTagData(AMA_NoStencil, GL_FALSE, tagList) ? stencilBits : 0;
    accumBits       = !GetTagData(AMA_NoAccum, GL_FALSE, tagList) ? accumBits : 0;
    depthBits       = !GetTagData(AMA_NoDepth, GL_FALSE, tagList) ? depthBits : 0;

    /* Allocate memory for aros structure */
    aros_vis = AllocVec(sizeof(struct arosmesa_visual), MEMF_PUBLIC | MEMF_CLEAR);

    if (!aros_vis)
        return NULL;

    vis = GET_GL_VIS_PTR(aros_vis);

    /* Initialize mesa structure */
    if(!_mesa_initialize_visual(vis,
                                GL_TRUE,        /* RGB */
                                GL_FALSE,       /* Double buffer - AROSMesa uses front buffer as back buffer */
                                GL_FALSE,       /* stereo */
                                redBits,
                                greenBits,
                                blueBits,
                                alphaBits,
                                indexBits,
                                depthBits,
                                stencilBits,
                                accumBits,
                                accumBits,
                                accumBits,
                                alphaBits ? accumBits : 0,
                                1)) /* FIXME: xlib state_tracker code passes 0 - verify */
    {
        aros_destroy_visual(aros_vis);
        return NULL;
    }

    return aros_vis;
}

static void 
aros_destroy_context(AROSMesaContext amesa)
{
    if (amesa)
    {
        FreeVec(amesa);
    }
}

static AROSMesaFrameBuffer 
aros_new_framebuffer(AROSMesaContext amesa, AROSMesaVisual visual)
{
    AROSMesaFrameBuffer aros_fb = NULL;
    enum pipe_format colorFormat, depthFormat, stencilFormat;
    GLvisual * vis = GET_GL_VIS_PTR(visual);
    
    D(bug("[AROSMESA] aros_new_framebuffer\n"));

    stencilFormat = PIPE_FORMAT_NONE;
    
    switch(vis->redBits)
    {
        case(5):
            colorFormat = PIPE_FORMAT_R5G6B5_UNORM;
            break;
        case(8):
            colorFormat = PIPE_FORMAT_A8R8G8B8_UNORM;
            break;
        default:
            D(bug("[AROSMESA] aros_new_framebuffer - ERROR - Unsupported redBits value\n"));
            return NULL;
    }
    
    switch(vis->depthBits)
    {
        case(0):
            depthFormat = PIPE_FORMAT_NONE;
            break;
        case(16):
            depthFormat = PIPE_FORMAT_Z16_UNORM;
            break;
        case(24):
            depthFormat = PIPE_FORMAT_Z24S8_UNORM;
            break;
        default:
            D(bug("[AROSMESA] aros_new_framebuffer - ERROR - Unsupported depthBits value\n"));
            return NULL;
    }
    
    switch(vis->stencilBits)
    {
        case(0):
            stencilFormat = PIPE_FORMAT_NONE;
            break;
        case(8):
            if (depthFormat == PIPE_FORMAT_Z24S8_UNORM)
                stencilFormat = PIPE_FORMAT_Z24S8_UNORM;
            else
                stencilFormat = PIPE_FORMAT_S8_UNORM;
            break;
        default:
            D(bug("[AROSMESA] aros_new_framebuffer - ERROR - Unsupported stencilBits value\n"));
            return NULL;
    }

    /* Allocate memory for aros structure */
    aros_fb = AllocVec(sizeof(struct arosmesa_framebuffer), MEMF_PUBLIC | MEMF_CLEAR);

    if (!aros_fb)
        return NULL;
    
    /* Create framebuffer */
    aros_fb->stfb = st_create_framebuffer(vis,
                                    colorFormat, depthFormat, stencilFormat,
                                    amesa->width, amesa->height,
                                    (void *) aros_fb);    
    
    return aros_fb;
}

static void
aros_destroy_framebuffer(AROSMesaFrameBuffer aros_fb)
{
    if (aros_fb)
    {
        if (aros_fb->stfb)
        {
            /* So that reference count goes to 0 and buffer is freed */
            st_unreference_framebuffer(aros_fb->stfb);
            aros_fb->stfb = NULL;
        }
        
        FreeVec(aros_fb);
    }
}

static void 
aros_select_rastport(AROSMesaContext amesa, struct TagItem * tagList)
{
    amesa->ScreenInfo.Screen = (struct Screen *)GetTagData(AMA_Screen, 0, tagList);
    amesa->window = (struct Window *)GetTagData(AMA_Window, 0, tagList);
    amesa->visible_rp = (struct RastPort *)GetTagData(AMA_RastPort, 0, tagList);

    if (amesa->ScreenInfo.Screen)
    {
        D(bug("[AROSMESA] aros_select_rastport: Screen @ %x\n", amesa->ScreenInfo.Screen));
        if (amesa->window)
        {
            D(bug("[AROSMESA] aros_select_rastport: Window @ %x\n", amesa->window));
            if (!(amesa->visible_rp))
            {
                /* Use the windows rastport */
                amesa->visible_rp = amesa->window->RPort;
                D(bug("[AROSMESA] aros_select_rastport: Windows RastPort @ %x\n", amesa->visible_rp));
            }
        }
        else
        {
            if (!(amesa->visible_rp))
            {
                /* Use the screens rastport */
                amesa->visible_rp = &amesa->ScreenInfo.Screen->RastPort;
                D(bug("[AROSMESA] aros_select_rastport: Screens RastPort @ %x\n", amesa->visible_rp));
            }
        }
    }
    else
    {
        /* Not passed a screen */
        if (amesa->window)
        {
            D(bug("[AROSMESA] aros_select_rastport: Window @ %x\n", amesa->window));
            /* Use the windows Screen */
            amesa->ScreenInfo.Screen = amesa->window->WScreen;
            D(bug("[AROSMESA] aros_select_rastport: Windows Screen @ %x\n", amesa->ScreenInfo.Screen));

            if (!(amesa->visible_rp))
            {
                /* Use the windows rastport */
                amesa->visible_rp = amesa->window->RPort;
                D(bug("[AROSMESA] aros_select_rastport: Windows RastPort @ %x\n", amesa->visible_rp));
            }
        }
        else
        {
            /* Only Passed A Rastport */
            D(bug("[AROSMESA] aros_select_rastport: Using RastPort only!\n"));
        }
    }

    D(bug("[AROSMESA] aros_select_rastport: Using RastPort @ %x\n", amesa->visible_rp));
}

static GLboolean
_aros_recalculate_buffer_width_height(AROSMesaContext amesa)
{
    GLsizei newwidth = 0;
    GLsizei newheight = 0;
    
    D(bug("[AROSMESA] _aros_recalculate_width_height\n"));
    
    
    amesa->visible_rp_width = 
        amesa->visible_rp->Layer->bounds.MaxX - amesa->visible_rp->Layer->bounds.MinX + 1;

    amesa->visible_rp_height = 
        amesa->visible_rp->Layer->bounds.MaxY - amesa->visible_rp->Layer->bounds.MinY + 1;


    newwidth = amesa->visible_rp_width - amesa->left - amesa->right;
    newheight = amesa->visible_rp_height - amesa->top - amesa->bottom;
    
    if (newwidth < 0) newwidth = 0;
    if (newheight < 0) newheight = 0;
    
    
    if ((newwidth != amesa->width) || (newheight != amesa->height))
    {
        /* The drawing area size has changed. Buffer must change */
        D(bug("[AROSMESA] _aros_recalculate_width_height: current height    =   %d\n", amesa->height));
        D(bug("[AROSMESA] _aros_recalculate_width_height: current width     =   %d\n", amesa->width));
        D(bug("[AROSMESA] _aros_recalculate_width_height: new height        =   %d\n", newheight));
        D(bug("[AROSMESA] _aros_recalculate_width_height: new width         =   %d\n", newwidth));
        
        amesa->width = newwidth;
        amesa->height = newheight;
        
        if (amesa->window)
        {
            struct Rectangle rastcliprect;
            struct TagItem crptags[] =
            {
                { RPTAG_ClipRectangle      , (IPTR)&rastcliprect },
                { RPTAG_ClipRectangleFlags , (RPCRF_RELRIGHT | RPCRF_RELBOTTOM) },
                { TAG_DONE }
            };
        
            D(bug("[AROSMESA] _aros_recalculate_width_height: Clipping Rastport to Window's dimensions\n"));

            /* Clip the rastport to the visible area */
            rastcliprect.MinX = amesa->left;
            rastcliprect.MinY = amesa->top;
            rastcliprect.MaxX = amesa->left + amesa->width;
            rastcliprect.MaxY = amesa->top + amesa->height;
            SetRPAttrsA(amesa->visible_rp, crptags);
        }
        
        return GL_TRUE;
    }
    
    return GL_FALSE;
}

static void
_aros_check_and_update_buffer_size(AROSMesaContext amesa)
{
    if (_aros_recalculate_buffer_width_height(amesa))
        st_resize_framebuffer(amesa->framebuffer->stfb, amesa->width, amesa->height);
}

static BOOL
aros_standard_init(AROSMesaContext amesa, struct TagItem *tagList)
{
    GLint requestedwidth = 0, requestedheight = 0;
    GLint requestedright = 0, requestedbottom = 0;
    
    D(bug("[AROSMESA] aros_standard_init(amesa @ %x, taglist @ %x)\n", amesa, tagList));
    D(bug("[AROSMESA] aros_standard_init: Using RastPort @ %x\n", amesa->visible_rp));

    amesa->visible_rp = CloneRastPort(amesa->visible_rp);

    D(bug("[AROSMESA] aros_standard_init: Cloned RastPort @ %x\n", amesa->visible_rp));

    /* We assume left and top are given or set to 0 */
    amesa->left = GetTagData(AMA_Left, 0, tagList);
    amesa->top = GetTagData(AMA_Top, 0, tagList);
    
    requestedright = GetTagData(AMA_Right, -1, tagList);
    requestedbottom = GetTagData(AMA_Bottom, -1, tagList);
    requestedwidth = GetTagData(AMA_Width, -1 , tagList);
    requestedheight = GetTagData(AMA_Height, -1 , tagList);

    /* Calculate rastport dimensions */
    amesa->visible_rp_width = 
        amesa->visible_rp->Layer->bounds.MaxX - amesa->visible_rp->Layer->bounds.MinX + 1;

    amesa->visible_rp_height = 
        amesa->visible_rp->Layer->bounds.MaxY - amesa->visible_rp->Layer->bounds.MinY + 1;
    
    /* right will be either passed or calculated from width or 0 */
    amesa->right = 0;
    if (requestedright < 0)
    {
        if (requestedwidth >= 0)
        {
            requestedright = amesa->visible_rp_width - amesa->left - requestedwidth;
            if (requestedright < 0) requestedright = 0;
        }
        else
            requestedright = 0;
    }
    amesa->right = requestedright;
    
    /* bottom will be either passed or calculated from height or 0 */
    amesa->bottom = 0;
    if (requestedbottom < 0)
    {
        if (requestedheight >= 0)
        {
            requestedbottom = amesa->visible_rp_height - amesa->top - requestedheight;
            if (requestedbottom < 0) requestedbottom = 0;
        }
        else
            requestedbottom = 0;
    }
    amesa->bottom = requestedbottom;
    
    /* Init screen information */
    if (amesa->ScreenInfo.Screen)
    {
        amesa->ScreenInfo.Depth         = GetCyberMapAttr(amesa->ScreenInfo.Screen->RastPort.BitMap, CYBRMATTR_DEPTH);
        amesa->ScreenInfo.BitsPerPixel  = GetCyberMapAttr(amesa->ScreenInfo.Screen->RastPort.BitMap, CYBRMATTR_BPPIX) * 8;
        amesa->ScreenInfo.Width         = GetCyberMapAttr(amesa->ScreenInfo.Screen->RastPort.BitMap, CYBRMATTR_WIDTH);
        amesa->ScreenInfo.Height        = GetCyberMapAttr(amesa->ScreenInfo.Screen->RastPort.BitMap, CYBRMATTR_HEIGHT);
    }
    
    D(bug("[AROSMESA] aros_standard_init: Context Base dimensions set -:\n"));
    D(bug("[AROSMESA] aros_standard_init:    amesa->visible_rp_width        = %d\n", amesa->visible_rp_width));
    D(bug("[AROSMESA] aros_standard_init:    amesa->visible_rp_height       = %d\n", amesa->visible_rp_height));
    D(bug("[AROSMESA] aros_standard_init:    amesa->width                   = %d\n", amesa->width));
    D(bug("[AROSMESA] aros_standard_init:    amesa->height                  = %d\n", amesa->height));
    D(bug("[AROSMESA] aros_standard_init:    amesa->left                    = %d\n", amesa->left));
    D(bug("[AROSMESA] aros_standard_init:    amesa->right                   = %d\n", amesa->right));
    D(bug("[AROSMESA] aros_standard_init:    amesa->top                     = %d\n", amesa->top));
    D(bug("[AROSMESA] aros_standard_init:    amesa->bottom                  = %d\n", amesa->bottom));
    D(bug("[AROSMESA] aros_standard_init:    amesa->depth                   = %d\n", amesa->ScreenInfo.Depth));

    return TRUE;
}



/*****************************************************************************/
/*                             PUBLIC FUNCTIONS                              */
/*****************************************************************************/



AROSMesaContext AROSMesaCreateContextTags(long Tag1, ...)
{
  return AROSMesaCreateContext((struct TagItem *)&Tag1);
}

AROSMesaContext AROSMesaCreateContext(struct TagItem *tagList)
{
    AROSMesaContext amesa = NULL;
    /* LastError = 0; */ /* FIXME: verify usage of LastError - should it be part of AROSMesaContext ? */
    struct pipe_screen * screen = NULL;
    struct pipe_context * pipe = NULL;
    struct pHidd_GalliumBaseDriver_CreatePipeScreen cpsmsg;

    /* TODO: Move openining/closing of library into library init code */

    /* Try to open cybergraphics.library */
    if (CyberGfxBase == NULL)
    {
        if (!(CyberGfxBase = OpenLibrary((STRPTR)"cybergraphics.library",0)))
            return NULL;
    }
    
    /* Try to open oop.library */
    if (OOPBase == NULL)
    {
        if (!(OOPBase = OpenLibrary((STRPTR)"oop.library", 0)))
            return NULL;
    }

    /* Try to open gallium.hidd */
    if (AROSMesaHIDDGalliumBase == NULL)
    {
        if (!(AROSMesaHIDDGalliumBase = OpenLibrary((STRPTR)"gallium.hidd", 2)))
            return NULL;
    }
    
    if (driver == NULL)
    {
        /* Create gallium driver factory */
        OOP_Object * galliumdriverfactory = 
            OOP_NewObject(NULL, CLID_Hidd_GalliumDriverFactory, NULL);
        
        if (galliumdriverfactory)
        {
            /* Ask for gallium driver */
            driver = HIDD_GalliumDriverFactory_GetDriver(galliumdriverfactory);

            /* Dispose factory */
            OOP_DisposeObject(galliumdriverfactory);
        }
    }

    /* Check if driver is acquired */
    if (!driver)
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - failed to retrieve Gallium3D driver\n"));
        return NULL;
    }
    
    /* Allocate arosmesa_context struct initialized to zeros */
    if (!(amesa = (AROSMesaContext)AllocVec(sizeof(struct arosmesa_context), MEMF_PUBLIC|MEMF_CLEAR)))
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - failed to allocate AROSMesaContext\n"));
        /* LastError = AMESA_OUT_OF_MEM; */ /* FIXME: verify usage of LastError - should it be part of AROSMesaContext ? */
        return NULL;
    }
    
    /* FIXME; shouldn't RastPort be part of framebuffer? */
    aros_select_rastport(amesa, tagList);
    
    /* FIXME: check if any rastport is available */
    
    /* FIXME: later this might me placed in initialization of framebuffer */
    aros_standard_init(amesa, tagList);   
    
    D(bug("[AROSMESA] AROSMesaCreateContext: Creating new AROSMesaVisual\n"));

    if (!(amesa->visual = aros_new_visual(amesa->ScreenInfo.BitsPerPixel, tagList)))
    {
        /* TODO: Route error handling to one place */
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to create AROSMesaVisual\n"));
        FreeVec( amesa );
        /* LastError = AMESA_OUT_OF_MEM; */ /* FIXME: verify usage of LastError - should it be part of AROSMesaContext ? */
        return NULL;
    }

    cpsmsg.mID = OOP_GetMethodID(IID_Hidd_GalliumBaseDriver, moHidd_GalliumBaseDriver_CreatePipeScreen);
    screen = (struct pipe_screen *)OOP_DoMethod(driver, (OOP_Msg)&cpsmsg);
    
    if (!screen)
    {
        /* TODO: Route error handling to one place */
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to create gallium driver screen\n"));
        aros_destroy_visual(amesa->visual);
        FreeVec(amesa);
        return NULL;
    }

    pipe = screen->context_create(screen, NULL);
    
    /* FIXME: If pipe == NULL */
    
    amesa->st = st_create_context(pipe, GET_GL_VIS_PTR(amesa->visual), NULL);
    
    /* Set up some needed pointers */
    amesa->st->ctx->DriverCtx = amesa;
    pipe->priv = amesa;
    
    /* Initial update of buffer dimensions (amesa->width/amesa->height) */
    _aros_recalculate_buffer_width_height(amesa);
    
    
    /* FIXME: Provide rastport to framebuffer ? */
    amesa->framebuffer = aros_new_framebuffer(amesa, amesa->visual);
    
    return amesa;
}

#if defined (AROS_MESA_SHARED)
AROS_LH1(void, AROSMesaMakeCurrent,
    AROS_LHA(AROSMesaContext, amesa, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT
#else
void AROSMesaMakeCurrent(AROSMesaContext amesa)
{
#endif
    SAVE_REG
    
    PUT_MESABASE_IN_REG

    /* FIXME: if passed context is the same as current context, check buffer sizes */
    /* FIXME: if there was old context active, flush it and NULL the pointers to buffers */
    
    if (amesa)
    {
        st_make_current(amesa->st, amesa->framebuffer->stfb, amesa->framebuffer->stfb);
        
        /* Resize must be done here */
        _aros_recalculate_buffer_width_height(amesa);
        st_resize_framebuffer(amesa->framebuffer->stfb, amesa->width, amesa->height);
    }
    else
    {
        /* Detach */
        st_make_current( NULL, NULL, NULL );
    }
        
    RESTORE_REG
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
#else
void AROSMesaSwapBuffers(AROSMesaContext amesa)
{        
#endif
    SAVE_REG
    
    PUT_MESABASE_IN_REG

    struct pipe_surface *surf;

    /* If we're swapping the buffer associated with the current context
    * we have to flush any pending rendering commands first.
    */
    st_notify_swapbuffers(amesa->framebuffer->stfb);

    /* FIXME: should be ST_SURFACE_BACK_LEFT */
    st_get_framebuffer_surface(amesa->framebuffer->stfb, ST_SURFACE_FRONT_LEFT, &surf);

    if (surf) {
        struct pHidd_GalliumBaseDriver_DisplaySurface dsmsg = {
        mID : OOP_GetMethodID(IID_Hidd_GalliumBaseDriver, moHidd_GalliumBaseDriver_DisplaySurface),
        context : amesa->st->pipe,
        rastport : amesa->visible_rp,
        left : amesa->left,
        right : amesa->right,
        top : amesa->top,
        bottom : amesa->bottom,
        width : amesa->width,
        height : amesa->height,
        surface : surf
        };
        OOP_DoMethod(driver, (OOP_Msg)&dsmsg);
    }

    /* Flush. Executes all code posted in DisplaySurface */
    st_flush(amesa->st, PIPE_FLUSH_RENDER_CACHE, NULL);

    _aros_check_and_update_buffer_size(amesa);

    RESTORE_REG
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
#else
void AROSMesaDestroyContext(AROSMesaContext amesa)
{
#endif
    SAVE_REG
    
    PUT_MESABASE_IN_REG

    /* destroy a AROS/Mesa context */
    D(bug("[AROSMESA] AROSMesaDestroyContext(amesa @ %x)\n", amesa));

    if (!amesa)
        return;

    GLcontext * ctx = GET_GL_CTX_PTR(amesa);

    if (ctx)
    {
        struct pHidd_GalliumBaseDriver_DestroyPipeScreen dpsmsg = {
        mID : OOP_GetMethodID(IID_Hidd_GalliumBaseDriver, moHidd_GalliumBaseDriver_DestroyPipeScreen),
        screen : ctx->st->pipe->screen
        };
        
        GET_CURRENT_CONTEXT(cur_ctx);

        if (cur_ctx == ctx)
        {
            /* Unbind if current */
            st_make_current( NULL, NULL, NULL );
        }

        st_finish(ctx->st);

        st_destroy_context(ctx->st);
        
        aros_destroy_framebuffer(amesa->framebuffer);
        aros_destroy_visual(amesa->visual);
        aros_destroy_context(amesa);

        OOP_DoMethod(driver, (OOP_Msg)&dpsmsg);
    }
    
    RESTORE_REG
#if defined (AROS_MESA_SHARED)
    AROS_LIBFUNC_EXIT
#endif
}

#if defined (AROS_MESA_SHARED)
AROS_LH0(AROSMesaContext, AROSMesaGetCurrentContext,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT
#else
AROSMesaContext AROSMesaGetCurrentContext()
{
#endif
    SAVE_REG
    
    PUT_MESABASE_IN_REG

    GET_CURRENT_CONTEXT(ctx);

    RESTORE_REG;
    
    return (AROSMesaContext)ctx;

#if defined (AROS_MESA_SHARED)
    AROS_LIBFUNC_EXIT
#endif
}

