/* $Id$*/

/*
 * Mesa 3-D graphics library
 * Copyright (C) 1995  Brian Paul  (brianp@ssec.wisc.edu)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "AROSMesa_intern.h"
#include <GL/AROSMesa.h>

#include "AROSMesa_8bit.h"

#ifdef __GNUC__
#include "../aros/misc/ht_colors.h"
#else
#include "/aros/misc/ht_colors.h"
#endif

#include <exec/memory.h>
#include <exec/types.h>
#ifdef __GNUC__
#ifndef __AROS__
#include <inline/exec.h>
#include <inline/intuition.h>
#include <inline/graphics.h>
#include <inline/utility.h>
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#endif
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"

#ifdef AGA
#define c8to32(x) ((x)<<24)
#else
#define c8to32(x) (((((((x)<<8)|(x))<<8)|(x))<<8)|(x))
#endif

#define MAX_POLYGON 300
/* #define AGA 1 */
/* TrueColor-RGBA */
#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

#define DEBUG 1
#include <aros/debug.h>

static void aros8bit_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height);

struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
void destroy_rastport( struct RastPort *rp );
BOOL make_temp_raster( struct RastPort *rp );
void destroy_temp_raster( struct RastPort *rp );
void AllocOneLine(struct arosmesa_context *AROSMesaCreateContext);
void FreeOneLine(struct arosmesa_context *AROSMesaCreateContext);

BOOL alloc_temp_rastport(struct arosmesa_context * c);
void free_temp_rastport(struct arosmesa_context * c);

/*
__inline int RGBA(GLubyte r,GLubyte g,GLubyte b,GLubyte a)
	{
	int pen;
	
*/

/*printf("RGBA(%d,%d,%d,%d)",r,g,b,a);getchar();*/

/*
	pen=ObtainBestPen(amesa->Screen->ViewPort.ColorMap,
			c8to32((ULONG)r),c8to32((ULONG)g),c8to32((ULONG)b),
			OBP_Precision,PRECISION_GUI,TAG_DONE);
	if(pen!=-1)
		{
		amesa->mypen[pen]+=1;
		}
	else
		{
		pen=FindColor(amesa->Screen->ViewPort.ColorMap,
									c8to32((ULONG)r),c8to32((ULONG)g),c8to32((ULONG)b),-1); 
									*/
									/* If all pen is no sharable    take one */
/* kprintf("(no sharable pen)\n",pen);*/
/*

		}
	return pen;
	}
*/ 

#define FIXx(x) (amesa->left + (x))

#define FIXy(y) (amesa->RealHeight-amesa->bottom - (y))

#define FIXxy(x,y) ((amesa->RealWidth*FIXy(y)+FIXx(x)))

/**********************************************************************/
/*****                Miscellaneous device driver funcs           *****/
/**********************************************************************/

static void aros8bit_finish( void )
{
    /* implements glFinish if possible */
}

static void aros8bit_flush( void )
{
    /* implements glFlush if possible */
}

static void aros8bit_clear_index( GLcontext *ctx, GLuint index )
{
    /* implement glClearIndex */
    /* usually just save the value in the context struct */
    /*printf("aros8bit_clear_index=glClearIndex=%d\n",index);*/
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    amesa->clearpixel = amesa->penconv[index];
}

static void aros8bit_clear_color( GLcontext *ctx,
                                GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    /* implement glClearColor */
    /* color components are floats in [0,1] */
    /* usually just save the value in the context struct */
    /*printf("aros8bit_clear_color=glClearColor(%d,%d,%d,%d)\n",r,g,b,a);*/
    /* @@@ TODO FREE COLOR IF NOT USED */
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    amesa->clearpixel=RGBA(amesa,r,g,b,a);
/*  amesa->clearpixel=RGBA(r,g,b,a);  Orginal */  

}

static void aros8bit_clear(GLcontext* ctx, GLbitfield mask,
                                GLboolean all, GLint x, GLint y, GLint width, GLint height)
{
    /*
    * Clear the specified region of the color buffer using the clear color
    * or index as specified by one of the two functions above.
    * If all==GL_TRUE, clear whole buffer
    */
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

#ifdef DEBUGPRINT
printf("aros8bit_clear(%d,%d,%d,%d,%d)\n",all,x,y,width,height);
#endif

    SetAPen(amesa->rp,amesa->clearpixel);
    if(all)
    {
        RectFill(amesa->rp,FIXx(ctx->Viewport.X),FIXy(ctx->Viewport.Y)-ctx->Viewport.Height+1,FIXx(ctx->Viewport.X)+ctx->Viewport.Width-1,FIXy(ctx->Viewport.Y));
        if (amesa->visual->rgb_flag)
        {
/*
            int I;
            for(I=0;I<=255;I++) */
            /* Dealocate pens is in RGB mode */
            /*
            {
                while (amesa->mypen[I]!=0)          */
                /* TODO This may free some others pen also */
                /*
                {
                    amesa->mypen[I]-=1;
                    ReleasePen(amesa->Screen->ViewPort.ColorMap,I);
                }
            }
*/
        }
    }
    else
    {
        if(amesa->rp!=0)
        {
/*      printf("RectFill(0x%x,%d,%d,%d,%d)\n",amesa->rp,FIXx(x),FIXy(y)-height,width,FIXy(y));*/
            RectFill(amesa->rp,FIXx(x),FIXy(y)-height,width,FIXy(y));
        }
        else printf("Serious error amesa->rp=0 detected in aros8bit_clear() in file AROSMesa_8bit.c\n");
    }
}

static void aros8bit_set_index( GLcontext *ctx,GLuint index )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

    /* Set the amesa color index. */
/*printf("aros8bit_set_index(%d)\n",index);*/
    amesa->pixel = amesa->penconv[index];
}

static void aros8bit_set_color( GLcontext *ctx,GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

/*printf("aros8bit_set_color(%d,%d,%d,%d)\n",r,g,b,a);*/

    /* Set the current RGBA color. */
    /* r is in 0..255.RedScale */
    /* g is in 0..255.GreenScale */
    /* b is in 0..255.BlueScale */
    /* a is in 0..255.AlphaScale */
    amesa->pixel = RGBA(amesa,r,g,b,a);
    /*(a << 24) | (r << 16) | (g << 8) | b;*/
}

static GLboolean aros8bit_index_mask( GLcontext *ctx,GLuint mask )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

    /* implement glIndexMask if possible, else return GL_FALSE */
/*printf("aros8bit_index_mask(0x%x)\n",mask);*/
    amesa->rp->Mask = (UBYTE) mask;

    return(GL_TRUE);
}

static GLboolean aros8bit_color_mask( GLcontext *ctx,GLboolean rmask, GLboolean gmask,
                                      GLboolean bmask, GLboolean amask)
{
    /* implement glColorMask if possible, else return GL_FALSE */
    return(GL_FALSE);

}

/**********************************************************************/
/*****            Accelerated point, line, polygon rendering      *****/
/**********************************************************************/

/*
 *  Render a number of points by some hardware/OS accerated method
 */

static void aros8bit_points_function(GLcontext *ctx,GLuint first, GLuint last )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    struct vertex_buffer *VB = ctx->VB;

    int i,col;
    register struct RastPort * rp=amesa->rp;
/* printf("aros8bit_points_function\n");*/

    if (VB->MonoColor) {
        /* draw all points using the current color (set_color) */
        SetAPen(rp,amesa->pixel);
/*      printf("VB->MonoColor\n");*/
        for (i=first;i<=last;i++) {

            if (VB->ClipMask[i]==0) {
                /* compute window coordinate */
                int x, y;
                x =FIXx((GLint) (VB->Win[i][0]));
                y =FIXy((GLint) (VB->Win[i][1]));
                WritePixel(rp,x,y);
/*      printf("WritePixel(%d,%d)\n",x,y);*/
            }
        }
    } else {
        /* each point is a different color */
/*      printf("!VB.MonoColor\n");*/

        for (i=first;i<=last;i++) {
            if ((VB->ClipMask[i]==0)) {
                int x, y;
                x =FIXx((GLint) (VB->Win[i][0]));
                y =FIXy((GLint) (VB->Win[i][1]));
                col=*VB->Color[i];
                SetAPen(rp,amesa->penconv[col]);
                WritePixel(rp,x,y);
/*      printf("WritePixel(%d,%d)\n",x,y);*/
            }
        }
    }
}

static points_func aros8bit_choose_points_function( GLcontext *ctx )
{
/*printf("aros8bit_choose_points_function\n");*/
    /* Examine the current rendering state and return a pointer to a */
    /* fast point-rendering function if possible. */
    if (ctx->Point.Size==1.0 && !ctx->Point.SmoothFlag && ctx->RasterMask==0
           && !ctx->Texture.Enabled /*&&  ETC, ETC */) {
        return aros8bit_points_function;
    }
    else return NULL;
}

 /*
  *  Render a line by some hardware/OS accerated method 
  */

static void aros8bit_line_function( GLcontext *ctx,GLuint v0, GLuint v1, GLuint pv )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    struct vertex_buffer *VB = ctx->VB;
    int x0, y0, x1, y1;
/* printf("aros8bit_line_function\n");*/

    if (VB->MonoColor) SetAPen(amesa->rp,amesa->pixel);
    else SetAPen(amesa->rp,amesa->penconv[*VB->Color[pv]]);

    x0 = FIXx((int) (VB->Win[v0][0]));
    y0 = FIXy((int) (VB->Win[v0][1]));
    x1 = FIXx((int) (VB->Win[v1][0]));
    y1 = FIXy((int) (VB->Win[v1][1]));

    Move(amesa->rp,x0,y0);
    Draw(amesa->rp,x1,y1);
}

static line_func aros8bit_choose_line_function( GLcontext *ctx)
{
    /*printf("aros8bit_choose_line_function\n");*/

    /* Examine the current rendering state and return a pointer to a */
    /* fast line-rendering function if possible. */

    if (ctx->Line.Width==1.0 && !ctx->Line.SmoothFlag && !ctx->Line.StippleFlag
	   && ctx->Light.ShadeModel==GL_FLAT && ctx->RasterMask==0
	   && !ctx->Texture.Enabled /*&&  ETC, ETC */ )
    {
        return aros8bit_line_function;
    }
    else return NULL;
}

/**********************************************************************/
/*****                  Optimized polygon rendering               *****/
/**********************************************************************/

/*
 * Draw a filled polygon of a single color. If there is hardware/OS support
 * for polygon drawing use that here.   Otherwise, call a function in
 * polygon.c to do the drawing.
 */

#warning TODO: removed next [obsolete] function to compile on 2.6
/*static void aros8bit_polygon_function( GLcontext *ctx,GLuint n, GLuint vlist[], GLuint pv )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    struct vertex_buffer *VB = ctx->VB;
    
    int i,j;
    struct RastPort * rp=amesa->rp;
    
    // Render a line by some hardware/OS accerated method
    // printf("aros8bit_polygon_function\n");
    
    if (VB->MonoColor) SetAPen(rp,amesa->pixel);
    else SetAPen(rp,amesa->penconv[*VB->Color[pv]]);
    
    AreaMove(rp, FIXx((int) VB->Win[0][0]), FIXy( (int) VB->Win[0][1]));
    
    for (i=1;i<n;i++) {
        j=vlist[i];
        AreaDraw(rp, FIXx((int) VB->Win[j][0]), FIXy( (int) VB->Win[j][1]));
    }
    AreaEnd(rp );
}*/

#warning TODO: removed next [obsolete] function to compile on 2.6
/*static polygon_func aros8bit_choose_polygon_function( GLcontext *ctx )
{*/
    /* printf("aros8bit_choose_polygon_function\n");*/

    /* Examine the current rendering state and return a pointer to a */
    /* fast polygon-rendering function if possible. */
/*
    if (!ctx->Polygon.SmoothFlag && !ctx->Polygon.StippleFlag
         && ctx->Light.ShadeModel==GL_FLAT && ctx->RasterMask==0
         && !ctx->Texture.Enabled )*/ /*&&  ETC, ETC */
/*  {
        return aros8bit_polygon_function;
    }
    else 
    {
        return NULL;
    }
}
*/

/**********************************************************************/
/*****                 Span-based pixel drawing                   *****/
/**********************************************************************/

/* Write a horizontal span of 32-bit color-index pixels with a boolean mask. */
static void aros8bit_write_ci32_span(const GLcontext *ctx,
                                GLuint n, GLint x, GLint y,
                                const GLuint index[],
                                const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    UBYTE *dp=NULL;
    unsigned long * penconv=amesa->penconv;
    register struct RastPort * rp=amesa->rp;

#ifdef DEBUGPRINT
D(bug("aros8bit_write_ci32_span(%d,%d,%d)\n",n,x,y));
#endif

    y=FIXy(y);
    x=FIXx(x);
    /* JAM: Added correct checking */
    /* if((dp = amesa->imageline) && amesa->tmpras) */
    if((dp = amesa->imageline) && amesa->temprp)
    {                     /* if imageline and temporary rastport have been
                             allocated then use fastversion */
        ant=0;
        for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
            if (mask[i]) {
                ant++;
/*              x++; */
                *dp = penconv[index[i]];
                dp++;
            } else {
                if(ant)
                {
                    /* JAM: Replaced by correct call */
                    /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
                    WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
                }
              dp=amesa->imageline;
              ant=0;
              x++;
            }
        }
        if(ant)
        {
            /* JAM: Replaced by correct call */
            /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
            WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
        }
    } else {            /* Slower */
        for (i=0;i<n;i++,x++) {
            if (mask[i]) {
                 /* draw pixel (x[i],y[i]) using index[i] */
                SetAPen(rp,penconv[index[i]]);
                WritePixel(rp,x,y);
            }
        }
    }
}

/* Write a horizontal span of 8-bit color-index pixels with a boolean mask. */
static void aros8bit_write_ci8_span(const GLcontext *ctx,
                                GLuint n, GLint x, GLint y,
                                const GLubyte index[],
                                const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    UBYTE *dp=NULL;
    unsigned long * penconv=amesa->penconv;
    register struct RastPort * rp=amesa->rp;

#ifdef DEBUGPRINT
D(bug("aros8bit_write_ci8_span(%d,%d,%d)\n",n,x,y));
#endif

    y=FIXy(y);
    x=FIXx(x);
    /* JAM: Added correct checking */
    /* if((dp = amesa->imageline) && amesa->tmpras) */
    if((dp = amesa->imageline) && amesa->temprp)
    {                     /* if imageline and temporary rastport have been
                             allocated then use fastversion */
        ant=0;
        for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
            if (mask[i]) {
                ant++;
/*              x++; */
                *dp = penconv[index[i]];
                dp++;
            } else {
                if(ant)
                {
                    /* JAM: Replaced by correct call */
                    /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
                    WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
                }
              dp=amesa->imageline;
              ant=0;
              x++;
            }
        }
        if(ant)
        {
            /* JAM: Replaced by correct call */
            /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
            WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
        }
    } else {            /* Slower */
        for (i=0;i<n;i++,x++) {
            if (mask[i]) {
                 /* draw pixel (x[i],y[i]) using index[i] */
                SetAPen(rp,penconv[index[i]]);
                WritePixel(rp,x,y);
            }
        }
    }
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current
* color index is used for all pixels.
*/
static void aros8bit_write_mono_ci_span(const GLcontext* ctx,
                               GLuint n,GLint x,GLint y,
                               const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    y=FIXy(y);
    x=FIXx(x);
    SetAPen(amesa->rp,amesa->pixel);

#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_mono_ci_span\n"));
#endif

    i=0;
    while(i<n)
    {
        while(!mask[i] && i<n)
        {
            i++;x++;
        }
    
        if(i<n)
        {
            Move(amesa->rp,x,y);
            while(mask[i] && i<n)
            {
                i++;x++;
            }
            Draw(amesa->rp,x,y);
        }
    }
}

/* Write a horizontal span of RGBA color pixels with a boolean mask. */
static void aros8bit_write_rgba_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                const GLubyte rgba[][4], const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    UBYTE *dp=NULL;

    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_rgba_span(ant=%d,x=%d,y=%d)",n,x,y));
#endif

    if((dp = amesa->imageline) && amesa->temprp)
    {           /* if imageline allocated then use fastversion */
#ifdef DEBUGPRINT
D(bug("FAST "));
#endif
        if (mask)
        {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
            ant=0;
            for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
            {
                if (mask[i])
                {
                        ant++;
                        *dp = RGBA(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]);
                        dp++;
                }
                else
                {
                    if(ant)
                    {
                        WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
/*D(bug("[AMESA:8bit] : WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
                        dp=amesa->imageline;
                        ant=0;
                        x=x+ant;
                    }
                    x++;
                }
            }

            if(ant)
            {
                WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
/*printf("WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y);*/
            }
        }
        else
        {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
            for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
            {
                *dp = RGBA(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]);
                dp++;
            }
            WritePixelLine8(rp,x,y,n,amesa->imageline,amesa->temprp);
        }
    }
    else
    {    /* Slower version */
#ifdef DEBUGPRINT
D(bug("SLOW "));
#endif
        if (mask)
        {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
            /* draw some pixels */
            for (i=0; i<n; i++, x++)
            {
                if (mask[i])
                {
                    /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                    SetAPen(rp,RGBA(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]));
                    WritePixel(rp,x,y);
                }
            }
        }
        else
        {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
            /* draw all pixels */
            for (i=0; i<n; i++, x++)
            {
                /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                SetAPen(rp,RGBA(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]));
                WritePixel(rp,x,y);
            }
        }
    }
}

/* Write a horizontal span of RGB color pixels with a boolean mask. */
static void aros8bit_write_rgb_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                const GLubyte rgba[][3], const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    UBYTE *dp=NULL;

    register struct RastPort * rp = amesa->rp;

    y=FIXy(y);
    x=FIXx(x);
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_color_span(ant=%d,x=%d,y=%d)",n,x,y));
#endif

    if((dp = amesa->imageline) && amesa->temprp)
    {           /* if imageline allocated then use fastversion */
#ifdef DEBUGPRINT
D(bug("FAST "));
#endif
        if (mask)
        {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
            ant=0;
            for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
            {
                if (mask[i])
                {
                        ant++;
                        *dp = RGB(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP]);
                        dp++;
                }
                else
                {
                    if(ant)
                    {
                        WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
/*D(bug("[AMESA:8bit] : WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
                        dp=amesa->imageline;
                        ant=0;
                        x=x+ant;
                    }
                    x++;
                }
            }

            if(ant)
            {
                WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
/*D(bug("[AMESA:8bit] : WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
            }
        }
        else
        {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
            for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
            {
                *dp = RGB(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP]);
                dp++;
            }
            WritePixelLine8(rp,x,y,n,amesa->imageline,amesa->temprp);
        }
    }
    else
    {    /* Slower version */
#ifdef DEBUGPRINT
D(bug("SLOW "));
#endif
        if (mask)
        {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
            /* draw some pixels */
            for (i=0; i<n; i++, x++)
            {
                if (mask[i])
                {
                    /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                    SetAPen(rp,RGB(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP]));
                    WritePixel(rp,x,y);
                }
            }
        }
        else
        {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
            /* draw all pixels */
            for (i=0; i<n; i++, x++)
            {
                /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                SetAPen(rp,RGB(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP]));
                WritePixel(rp,x,y);
            }
        }
    }
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current color
* is used for all pixels.
*/

static void aros8bit_write_mono_rgba_span( const GLcontext *ctx,
                                      GLuint n, GLint x, GLint y,
                                      const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_mono_rgba_span(%d,%d,%d)\n",n,x,y));
#endif

    y=FIXy(y);
    x=FIXx(x);
    SetAPen(rp,amesa->pixel);
    i=0;
    while(i<n)
    {
        while(!mask[i] && i<n)
        {
            i++;
            x++;
        }
    
        if(i<n)
        {
            Move(rp,x,y);
            while(mask[i] && i<n)
            {
                i++;
                x++;
            }
            Draw(rp,x,y);
        }
    }
}

/**********************************************************************/
/*****                   Array-based pixel drawing                *****/
/**********************************************************************/

/* Write an array of 32-bit index pixels with a boolean mask. */
static void aros8bit_write_ci32_pixels( const GLcontext* ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLuint index[], const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_ci32_pixels<\n"));
#endif

    for (i=0; i<n; i++)
    {
        if (mask[i])
        {
            SetAPen(rp,amesa->penconv[index[i]]);
            WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
        }
    }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* index is used for all pixels.
*/
static void aros8bit_write_mono_ci_pixels( const GLcontext* ctx,
                                  GLuint n,
                                  const GLint x[], const GLint y[],
                                  const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_mono_ci_pixels<\n"));
#endif

    SetAPen(rp,amesa->pixel);

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using current index  */
            WritePixel(rp,FIXx(x[i]),FIXy(y[i]));

        }
    }
}

/* Write an array of RGBA pixels with a boolean mask. */
static void aros8bit_write_rgba_pixels( const GLcontext* ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLubyte rgba[][4], const GLubyte mask[] )
{
   AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_rgba_pixels<\n"));
#endif

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
            SetAPen(rp,RGBA(amesa,rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]));
            WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
        }
    }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* is used for all pixels.
*/
static void aros8bit_write_mono_rgba_pixels( const GLcontext* ctx,
                                    GLuint n,
                                    const GLint x[], const GLint y[],
                                    const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_write_mono_rgba_pixels<\n"));
#endif

    SetAPen(rp,amesa->pixel);

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using current color*/
            WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
        }
    }
}

/**********************************************************************/
/*****                    Read spans of pixels                              *****/
/**********************************************************************/

/* Read a horizontal span of color-index pixels. */
static void aros8bit_read_ci32_span( const GLcontext* ctx, GLuint n, GLint x, GLint y,
                            GLuint index[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

    int i;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_read_ci32_span>>\n"));
#endif

    y=FIXy(y);
    x=FIXx(x);

    if(amesa->temprp && amesa->imageline) {
        ReadPixelLine8(amesa->rp,x,y,n,amesa->imageline,amesa->temprp);
        for(i=0; i<n; i++)
            index[i]=amesa->imageline[i];
    } else {
        for (i=0; i<n; i++,x++)
            index[i] = ReadPixel(amesa->rp,x,y);
    }
}

/* Read a horizontal span of color pixels. */
static void aros8bit_read_rgba_span( const GLcontext* ctx,
                            GLuint n, GLint x, GLint y,
                            GLubyte rgba[][4] )
{
   AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,col;
    ULONG ColTab[3];
    struct ColorMap * cm=amesa->Screen->ViewPort.ColorMap;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_read_rgba_span>>\n"));
#endif

    y=FIXy(y);
    x=FIXx(x);

    if(amesa->imageline && amesa->temprp) {
        ReadPixelLine8(amesa->rp,x,y,n,amesa->imageline,amesa->temprp);
        for(i=0; i<n; i++) {
            GetRGB32(cm,amesa->imageline[i],1,ColTab);
            rgba[i][RCOMP] = ColTab[0]>>24;
            rgba[i][GCOMP] = ColTab[1]>>24;
            rgba[i][BCOMP] = ColTab[2]>>24;
            rgba[i][ACOMP] = 255;
        }
    } else
        for (i=0; i<n; i++, x++) {
            col=ReadPixel(amesa->rp,x,y);
            GetRGB32(cm,col,1,ColTab);

            rgba[i][RCOMP] = ColTab[0]>>24;
            rgba[i][GCOMP] = ColTab[1]>>24;
            rgba[i][BCOMP] = ColTab[2]>>24;
            rgba[i][ACOMP] = 255;
        }
}

/**********************************************************************/
/*****                       Read arrays of pixels                          *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void aros8bit_read_ci32_pixels( const GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLuint index[], const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_read_ci32_pixels-\n"));
#endif
	
    for (i=0; i<n; i++) {
        if (mask[i]) {
/*       index[i] = read_pixel x[i], y[i] */
              index[i] = ReadPixel(rp,FIXx(x[i]),FIXy(y[i]));
        }
    }
}

/* Read an array of color pixels. */
static void aros8bit_read_rgba_pixels( const GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLubyte rgba[][4], const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,col;
    register struct RastPort * rp = amesa->rp;

    ULONG ColTab[3];
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_read_rgba_pixels-\n"));
#endif

    for (i=0; i<n; i++)
    {
        if (mask[i])
        {
            col=ReadPixel(rp,FIXx(x[i]),FIXy(y[i]));

            GetRGB32(amesa->Screen->ViewPort.ColorMap,col,1,ColTab);

            rgba[i][RCOMP] = ColTab[0]>>24;
            rgba[i][GCOMP] = ColTab[1]>>24;
            rgba[i][BCOMP] = ColTab[2]>>24;
            rgba[i][ACOMP] = 255;
        }
    }
}

/**********************************************************************/
/**********************************************************************/

GLboolean aros8bit_logicop( GLcontext* ctx, GLenum op )
{
    /* can't implement */
    return GL_FALSE;
}

static void aros8bit_dither( GLcontext* ctx, GLboolean enable )
{
    /* currently unsuported */
}

static GLboolean aros8bit_set_buffer( GLcontext *ctx,GLenum mode )
{
	/* set the current drawing/reading buffer, return GL_TRUE or GL_FALSE */
	/* for success/failure */
	
/*   AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx; */

/* aros8bit_setup_DD_pointers(); in ddsample is this right?????*/
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_set_buffer TODO\n"));
#endif
	
#warning TODO: implemed a set of buffers
    if (mode==GL_FRONT)
    {
        return(GL_TRUE);
    }
    else if (mode==GL_BACK)
    {
        return(GL_TRUE);
    }
    else
    {
        return(GL_FALSE);
    }
}

static const char *aros8bit_renderer_string(void)
{
   return "AROS 8bit";
}

/**********************************************************************/
/**********************************************************************/
	/* Initialize all the pointers in the DD struct.  Do this whenever   */
	/* a new context is made current or we change buffers via set_buffer! */
void aros8bit_standard_DD_pointers( GLcontext *ctx )
{
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : Inside aros8bit_standard_DD_pointers ctx=0x%x  CC=0x%x\n",ctx,CC));
#endif

    ctx->Driver.RendererString = aros8bit_renderer_string;
    ctx->Driver.UpdateState = aros8bit_standard_DD_pointers;
    ctx->Driver.GetBufferSize = aros8bit_Standard_resize;
    ctx->Driver.Finish = aros8bit_finish;
    ctx->Driver.Flush = aros8bit_flush;

    ctx->Driver.ClearIndex = aros8bit_clear_index;
    ctx->Driver.ClearColor = aros8bit_clear_color;
    ctx->Driver.Clear = aros8bit_clear;
    
    ctx->Driver.Index = aros8bit_set_index;
    ctx->Driver.Color = aros8bit_set_color;
    ctx->Driver.IndexMask = aros8bit_index_mask;
    ctx->Driver.ColorMask = aros8bit_color_mask;

    ctx->Driver.LogicOp = aros8bit_logicop;
    ctx->Driver.Dither = aros8bit_dither;
    
    ctx->Driver.SetBuffer = aros8bit_set_buffer;
    //ctx->Driver.GetBufferSize = aros8bit_Standard_resize;

    ctx->Driver.PointsFunc = aros8bit_choose_points_function( ctx );
    ctx->Driver.LineFunc = aros8bit_choose_line_function( ctx );
#warning TODO: Write Triangle operations!!
    ctx->Driver.TriangleFunc = FALSE;

#warning TODO: Replace with RGBA functions [IMPORTANT!]
    /* Pixel/span writing functions: */
    ctx->Driver.WriteRGBASpan        = aros8bit_write_rgba_span;
    ctx->Driver.WriteRGBSpan         = aros8bit_write_rgb_span;
    ctx->Driver.WriteMonoRGBASpan    = aros8bit_write_mono_rgba_span;
    ctx->Driver.WriteRGBAPixels      = aros8bit_write_rgba_pixels;
    ctx->Driver.WriteMonoRGBAPixels  = aros8bit_write_mono_rgba_pixels;
    ctx->Driver.WriteCI32Span        = aros8bit_write_ci32_span;
    ctx->Driver.WriteCI8Span         = aros8bit_write_ci8_span;
    ctx->Driver.WriteMonoCISpan      = aros8bit_write_mono_ci_span;
    ctx->Driver.WriteCI32Pixels      = aros8bit_write_ci32_pixels;
    ctx->Driver.WriteMonoCIPixels    = aros8bit_write_mono_ci_pixels;

    /* Pixel/span reading functions: */
    ctx->Driver.ReadCI32Span        = aros8bit_read_ci32_span;
    ctx->Driver.ReadRGBASpan        = aros8bit_read_rgba_span;
    ctx->Driver.ReadCI32Pixels      = aros8bit_read_ci32_pixels;
    ctx->Driver.ReadRGBAPixels      = aros8bit_read_rgba_pixels;
}

/**********************************************************************/
/*****                  AROS/Mesa Private Functions                        *****/
/**********************************************************************/
/* JAM: temporary rastport allocation and freeing */

BOOL alloc_temp_rastport(struct arosmesa_context * c) {
    struct RastPort * temprp=NULL;
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : alloc_temp_rastport()\n"));
#endif

    if(( temprp = AllocVec( sizeof(struct RastPort), 0L))) {
        CopyMem( c->rp, temprp, sizeof(struct RastPort));
        temprp->Layer = NULL;

        if((temprp->BitMap= AllocBitMap(c->FixedWidth, 1, c->rp->BitMap->Depth,0,c->rp->BitMap))) {
/* temprp->BytesPerRow == (((width+15)>>4)<<1) */
            c->temprp=temprp;
            return TRUE;
        }
        FreeVec(temprp);
    }
    printf("Error allocating temporary rastport");
    return FALSE;
}

void free_temp_rastport(struct arosmesa_context * c) {
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : free_temp_rastport\n"));
#endif
    if(c->temprp) {
        FreeBitMap(c->temprp->BitMap);
        FreeVec(c->temprp);
    }
    c->temprp=NULL;
}

/*
 * Create a new rastport to use as a back buffer.
 * Input:  width, height - size in pixels
 *        depth - number of bitplanes
 */

struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm )
{
    struct RastPort *rp=NULL;
    struct BitMap *bm=NULL;
    
    if ((bm=AllocBitMap(width,height,depth,BMF_CLEAR|BMF_INTERLEAVED,friendbm)))
    {
        if ((rp = (struct RastPort *) malloc( sizeof(struct RastPort))))
        {
            InitRastPort( rp );
            rp->BitMap = bm;
            return rp;
        }
        else
        {
            FreeBitMap(bm);
            return 0;
        }
    }
    else return 0;
}

/*
 * Deallocate a rastport.
 */

void destroy_rastport( struct RastPort *rp )
{
    WaitBlit();
    FreeBitMap( rp->BitMap );
    free( rp );
}

/* 
** Color_buf is a array of pens equals the drawing area
** it's for faster dubbelbuffer rendering
** Whent it's time for bufferswitch just use c2p and copy.
*/

UBYTE* alloc_penbackarray( int width, int height, int bytes)
{
    return(AllocVec(width*height*bytes, MEMF_PUBLIC|MEMF_CLEAR));
}

void destroy_penbackarray(UBYTE *buf)
{
    FreeVec(buf);
}

/*
** Construct a temporary raster for use by the given rasterport.
** Temp rasters are used for polygon drawing.
*/

BOOL make_temp_raster( struct RastPort *rp ) {
    BOOL                OK=TRUE;
    unsigned long       width, height;
    PLANEPTR            p=NULL;
    struct              TmpRas *tmpras=NULL;

    if(rp==0) {
        printf("Zero rp\n");
        return(FALSE);
    }

    width = rp->BitMap->BytesPerRow*8;
    height = rp->BitMap->Rows;

    /* allocate structures */
    if((p = AllocRaster( width, height )))
    {
        if(( tmpras = (struct TmpRas *) AllocVec( sizeof(struct TmpRas), MEMF_ANY))) {
            InitTmpRas( tmpras, p, ((width+15)>>4)*height );
            rp->TmpRas = tmpras;
        } else
        OK=FALSE;
    }
    else return(FALSE);

    if (OK) return(TRUE);
    else {
        printf("Error when allocationg TmpRas\n");
        if (tmpras) FreeVec(tmpras);
        if (p) FreeRaster(p,width, height);
        return(FALSE);
    }
}

static BOOL allocarea(struct RastPort *rp ) {
    BOOL            OK=TRUE;
    struct AreaInfo *areainfo=NULL;
    UWORD           *pattern=NULL;
    APTR            vbuffer=NULL;

    areainfo = (struct AreaInfo *) AllocVec( sizeof(struct AreaInfo),MEMF_ANY );
    if(areainfo!=0) {
        pattern = (UWORD *) AllocVec( sizeof(UWORD),MEMF_ANY);
        if(pattern!=0) {
            *pattern = 0xffff;      /*@@@ org: 0xffffffff*/
            vbuffer = (APTR) AllocVec( MAX_POLYGON * 5 * sizeof(WORD),MEMF_ANY);
            if(vbuffer!=0) {
                /* initialize */
                InitArea( areainfo, vbuffer, MAX_POLYGON );
                /* bind to rastport */
                rp->AreaPtrn = pattern;
                rp->AreaInfo = areainfo;
                rp->AreaPtSz = 0;
            } else OK=FALSE;
        } else OK=FALSE;
    } else OK=FALSE;

    if (OK) return (OK);
    else {
        printf("Error when allocationg AreaBuffers\n");
        if (vbuffer) FreeVec(vbuffer);
        if (pattern) FreeVec(pattern);
        if (areainfo) FreeVec(areainfo);
        return(OK);
    }
}

void freearea(struct RastPort *rp) {
    if (rp->AreaInfo) {
        if (rp->AreaInfo->VctrTbl) FreeVec(rp->AreaInfo->VctrTbl);
        if (rp->AreaPtrn)
        {
            FreeVec(rp->AreaPtrn);
            rp->AreaPtrn=NULL;
        }
        FreeVec(rp->AreaInfo);
        rp->AreaInfo=NULL;
    }
}

/*
** Destroy a temp raster.
*/

void destroy_temp_raster( struct RastPort *rp )
{
    /* bitmap */

    unsigned long width, height;

    width = rp->BitMap->BytesPerRow*8;
    height = rp->BitMap->Rows;

    if (rp->TmpRas)
    {
        if(rp->TmpRas->RasPtr) FreeRaster( rp->TmpRas->RasPtr,width,height );
        FreeVec( rp->TmpRas );
        rp->TmpRas=NULL;
    }
}

void AllocOneLine( struct arosmesa_context *c) {
    if(c->imageline) FreeVec(c->imageline);
    if (c->depth<=8)
    {
        c->imageline = AllocVec((c->width+15) & 0xfffffff0,MEMF_ANY);   /* One Line */
    } else {
        c->imageline = AllocVec(((c->width+3) & 0xfffffff0)*4,MEMF_ANY);   /* One Line */
    }
}

void FreeOneLine( struct arosmesa_context *c) {
    if(c->imageline) {
        FreeVec(c->imageline);
        c->imageline=NULL;
    }
}

/**********************************************************************/
/*****                  AROS/Mesa private init/despose/resize     *****/
/**********************************************************************/

static void
aros8bit_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height)
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

    *width=amesa->width;
    *height=amesa->height;

    if(!((  amesa->width  == (amesa->front_rp->Layer->bounds.MaxX-amesa->front_rp->Layer->bounds.MinX-amesa->left) ) 
    &&  ( amesa->height == (amesa->front_rp->Layer->bounds.MaxY-amesa->front_rp->Layer->bounds.MinY-amesa->bottom)  )))
    {
        FreeOneLine(amesa);
    
        amesa->FixedWidth =amesa->RealWidth =amesa->front_rp->Layer->bounds.MaxX-amesa->front_rp->Layer->bounds.MinX;
        amesa->FixedHeight=amesa->RealHeight=amesa->front_rp->Layer->bounds.MaxY-amesa->front_rp->Layer->bounds.MinY;
    
        *width=amesa->width = amesa->RealWidth-amesa->left;
        *height=amesa->height = amesa->RealHeight-amesa->bottom;
        amesa->depth = GetBitMapAttr(amesa->front_rp->BitMap,BMA_DEPTH);
    
        destroy_temp_raster( amesa->rp); /* deallocate temp raster */
    
        /* JAM: Added free_temp_rastport() */
        free_temp_rastport(amesa);
    
        if (amesa->visual->db_flag) {
            if (amesa->back_rp) {   /* Free double buffer */
                destroy_rastport(amesa->back_rp);
            }
            if((amesa->back_rp = make_rastport(amesa->RealWidth,amesa->RealHeight,amesa->depth,amesa->rp->BitMap))==NULL) {
                amesa->rp = amesa->front_rp;
                printf("To little mem free. Couldn't allocate Dubblebuffer in this size.\n");
            } else {
                amesa->rp=amesa->back_rp;
            }
        }
    
        if(!make_temp_raster( amesa->rp )) printf("Error allocating TmpRasterPort\n");
    
        /* JAM: added alloc_temp_rastport */
        alloc_temp_rastport(amesa);
    
        AllocOneLine(amesa);
    }
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_Standard_resize(ctx=%x) = w:%d x h:%d\n",ctx,*width,*height));
#endif
}

void
aros8bit_Standard_Dispose(struct arosmesa_context *c)
{
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_Standard_Dispose\n"));
#endif

    FreeOneLine(c);
    if (c->depth<=8) {
        FreeCMap(c->Screen);
        freearea(c->rp);
        destroy_temp_raster( c->rp);
        /* JAM: Added free_temp_rastport() */
        free_temp_rastport(c);
    }
    
    if (c->visual->rgb_flag)
    {
        if (c->rgb_buffer)
        {
            printf("free(c->rgb_buffer)\n");
            free( c->rgb_buffer );
        }
    }
    
    if (c->back_rp)
    {
        destroy_rastport( c->back_rp );
        c->back_rp=NULL;
    }
}

void
aros8bit_Standard_SwapBuffer(struct arosmesa_context *amesa)
{
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_Standard_SwapBuffer\n"));
#endif

    if (amesa->back_rp)
    {
        UBYTE minterm = 0xc0;
/*      int x = amesa->left; */
/*      int y = amesa->RealHeight-amesa->bottom-amesa->height; */

        ClipBlit( amesa->back_rp, FIXx(amesa->gl_ctx->Viewport.X), FIXy(amesa->gl_ctx->Viewport.Y)-amesa->gl_ctx->Viewport.Height+1,    /* from */
             amesa->front_rp, FIXx(amesa->gl_ctx->Viewport.X), FIXy(amesa->gl_ctx->Viewport.Y)-amesa->gl_ctx->Viewport.Height+1,  /* to */
             amesa->gl_ctx->Viewport.Width, amesa->gl_ctx->Viewport.Height,  /* size */
             minterm );
        // ClipBlit( amesa->back_rp, x, y,
            /* from */
            // amesa->front_rp, x,y,
            /* to */
            // amesa->width, amesa->height,
            /* size */
            // minterm );

/* TODO Use these cordinates insted more efficent if you only use part of screen
        RectFill(amesa->rp,FIXx(CC.Viewport.X),FIXy(CC.Viewport.Y)-CC.Viewport.Height+1,
                  FIXx(CC.Viewport.X)+CC.Viewport.Width-1,FIXy(CC.Viewport.Y));*/
    }
}

BOOL
aros8bit_Standard_init(struct arosmesa_context *c,struct TagItem *tagList)
{
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : aros8bit_Standard_init\n"));
#endif
    if (!(c->window=(struct Window *)GetTagData(AMA_Window,0,tagList)))
    {
        if (!(c->rp=(struct RastPort *)GetTagData(AMA_RastPort,0,tagList)))
        {
            LastError=AMESA_RASTPORT_TAG_MISSING;
            return(FALSE);
        }

        if (!(c->Screen=(struct Screen *)GetTagData(AMA_Screen,0,tagList)))
        {
            LastError=AMESA_SCREEN_TAG_MISSING;
            return(FALSE);
        }
    }
    else
    {
        c->rp =c->window->RPort;
        c->Screen=c->window->WScreen;
    }

    c->FixedWidth =c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX;
    c->FixedHeight=c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY;

    c->left	= GetTagData(AMA_Left,0,tagList);
    c->bottom= GetTagData(AMA_Bottom,0,tagList);

    c->front_rp =c->rp;
    c->back_rp=NULL;
/*	c->rp = c->front_rp; */

    c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
    c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);

    c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);

/*	c->gl_ctx->BufferWidth = c->width; */
/*	c->gl_ctx->BufferHeight = c->height; */

    c->pixel = 0;	/* current drawing pen */

    AllocCMap(c->Screen);

    if (c->visual->db_flag==GL_TRUE)
    {
#ifdef DEBUGPRINT
D(bug("[AMESA:8bit] : Allocating DOUBLE BUFFERED display inside aros8bit_Standard_init"));
#endif
        if((c->back_rp = make_rastport(c->RealWidth,c->RealHeight,c->depth,c->rp->BitMap))!=NULL)
        {
            c->gl_ctx->Color.DrawBuffer = GL_BACK;
            c->rp = c->back_rp;
        }
        else
        {
            printf("make_rastport Faild\n");
            c->gl_ctx->Color.DrawBuffer = GL_FRONT;
        }
    }
    else c->gl_ctx->Color.DrawBuffer = GL_FRONT;

    AllocOneLine(c); /* A linebuffer for WritePixelLine */

    if (!make_temp_raster( c->rp )) printf("Error allocating TmpRastPort\n");

    alloc_temp_rastport(c);
    allocarea(c->rp);

    c->InitDD=aros8bit_standard_DD_pointers;  /*standard drawing*/
    c->Dispose=aros8bit_Standard_Dispose;
    c->SwapBuffer=aros8bit_Standard_SwapBuffer;

#ifdef DEBUGPRINT
    printf("c->RealWidth =%d\n",c->RealWidth);
    printf("c->RealHeight=%d\n",c->RealHeight);
    printf("c->width =%d\n",c->width);
    printf("c->height=%d\n",c->height);
    printf("c->left  =%d\n",c->left);
    printf("c->bottom=%d\n",c->bottom);
    printf("c->depth =%d\n",c->depth);
#endif

    return(TRUE);
}

/* Disabled for now .. (nicja)
#ifdef AMESA_DOUBLEBUFFFAST
#include "AROSMesa_8bit_db.c"
#endif*/

#undef DEBUGPRINT
