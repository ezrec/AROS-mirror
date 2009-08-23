/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "aros_driver_functions.h"

#include <aros/debug.h>
#include <proto/cybergraphics.h>
#include <graphics/rpattr.h>
#include <proto/graphics.h>


#include "arosmesa_internal.h"

#include <GL/arosmesa.h>
#include "swrast/swrast.h"
#include "tnl/tnl.h"
#include "swrast_setup/swrast_setup.h"
#include "vbo/vbo.h"
#include "main/framebuffer.h"

static const GLubyte * _aros_get_string(GLcontext *ctx, GLenum name)
{
    D(bug("[AROSMESA] aros_renderer_string()\n"));

    (void) ctx;
    switch (name)
    {
        case GL_RENDERER:
            return "AROS TrueColor Rasterizer";
        default:
            return NULL;
    }
}

/* implements glClearColor - Set the color used to clear the color buffer. */
static void _aros_clear_color( GLcontext *ctx, const GLfloat color[4] )
{
    AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
    amesa->clearpixel = TC_ARGB((GLbyte)(color[RCOMP] * 255), 
                                (GLbyte)(color[GCOMP] * 255), 
                                (GLbyte)(color[BCOMP] * 255), 
                                (GLbyte)(color[ACOMP] * 255));
    
    D(bug("[AROSMESA] aros_clear_color(c=%x,r=%d,g=%d,b=%d,a=%d) = %x\n", 
        ctx, (GLbyte)(color[RCOMP] * 255), (GLbyte)(color[GCOMP] * 255), (GLbyte)(color[BCOMP] * 255),
        (GLbyte)(color[ACOMP] * 255), amesa->clearpixel));
}


/*
* Clear the specified region of the color buffer using the clear color
* or index as specified by one of the two functions above.
*/
static void _aros_clear(GLcontext* ctx, GLbitfield mask)
{
    AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;

    D(bug("[AROSMESA] _aros_clear(x:%d,y:%d,w:%d,h:%d)\n", ctx->Viewport.X, 
        ctx->Viewport.Y, ctx->Viewport.Width, ctx->Viewport.Height));

    if ((mask & (BUFFER_BIT_FRONT_LEFT)))
    {  
        if(amesa->renderbuffer != NULL)
        {
            D(bug("[AROSMESA] _aros_clear: front_rp->Clearing viewport (ALL)\n"));
            ULONG i;
            ULONG size = GET_GL_RB_PTR(amesa->renderbuffer)->Width * 
                            GET_GL_RB_PTR(amesa->renderbuffer)->Height;

            ULONG * dp = (ULONG*)amesa->renderbuffer->buffer;
            for (i = 0; i < size; i++, dp++)
                *dp = amesa->clearpixel;

            mask &= ~BUFFER_BIT_FRONT_LEFT;
        }
        else
        {
            D(bug("[AROSMESA] _aros_clear: Serious ERROR amesa->front_rp = NULL detected\n"));
        }
    }

    if ((mask & (BUFFER_BIT_BACK_LEFT)))
    {  
        /* Noop */
        mask &= ~BUFFER_BIT_BACK_LEFT;
    }
  
    if (mask)
        _swrast_Clear( ctx, mask ); /* Remaining buffers to be cleared .. */
}

static void _aros_update_state(GLcontext *ctx, GLbitfield new_state)
{
    _swrast_InvalidateState(ctx, new_state);
    _swsetup_InvalidateState(ctx, new_state);
    _vbo_InvalidateState(ctx, new_state);
    _tnl_InvalidateState(ctx, new_state);
}

static void _aros_viewport(GLcontext *ctx, GLint x, GLint y, GLsizei w, GLsizei h)
{
    AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;

    D(bug("[AROSMESA] _aros_viewport\n"));
    
    _aros_recalculate_width_height(ctx, w, h);
    _mesa_resize_framebuffer(ctx, GET_GL_FB_PTR(amesa->framebuffer), amesa->width, amesa->height);
}


void
_aros_recalculate_width_height(GLcontext *ctx, GLsizei requestedwidth, GLsizei requestedheight)
{
    AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
    
    GLsizei maxwidth = 0;
    GLsizei maxheight = 0;
    
    D(bug("[AROSMESA] _aros_recalculate_width_height\n"));
    
    
    amesa->visible_rp_width = 
        amesa->visible_rp->Layer->bounds.MaxX - amesa->visible_rp->Layer->bounds.MinX + 1;

    amesa->visible_rp_height = 
        amesa->visible_rp->Layer->bounds.MaxY - amesa->visible_rp->Layer->bounds.MinY + 1;


    maxwidth = (amesa->visible_rp_width - amesa->left - amesa->right) * amesa->width_initial_ratio;
    maxheight = (amesa->visible_rp_height - amesa->top - amesa->bottom) * amesa->height_initial_ratio;
    
    if (maxwidth < 0) maxwidth = 0;
    if (maxheight < 0) maxheight = 0;
    
    D(bug("[AROSMESA] _aros_recalculate_width_height: requstedwidth     =   %d\n", requestedwidth));
    D(bug("[AROSMESA] _aros_recalculate_width_height: requstedheight    =   %d\n", requestedheight));
    D(bug("[AROSMESA] _aros_recalculate_width_height: maxwidth          =   %d\n", maxwidth));
    D(bug("[AROSMESA] _aros_recalculate_width_height: maxheight         =   %d\n", maxheight));
    
    
    if (requestedwidth < 0)
        amesa->width = maxwidth;
    else
        amesa->width = requestedwidth;
    
    if (requestedheight < 0)
        amesa->height = maxheight;
    else
        amesa->height = requestedheight;
    
    /* Clip to drawable area sizes */
    if (amesa->width > maxwidth) amesa->width = maxwidth;
    if (amesa->height > maxheight) amesa->height = maxheight;

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

}



void _aros_init_driver_functions(struct dd_function_table * functions)
{
    functions->GetString = _aros_get_string;
    functions->UpdateState = _aros_update_state;
    functions->GetBufferSize = NULL;
    functions->Clear = _aros_clear;
    functions->ClearColor = _aros_clear_color;
    functions->Viewport = _aros_viewport;
}

