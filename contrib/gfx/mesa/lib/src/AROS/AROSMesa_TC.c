/* $Id$ */

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

/*
$Log$
Revision 1.3  2005/01/12 13:09:01  NicJA
tidied remaining debug output

Revision 1.2  2005/01/12 11:11:40  NicJA
removed extremely broken "FAST" double bufering routines for the time being - will be placed into seperate files later..

Revision 1.1  2005/01/11 14:58:29  NicJA
AROSMesa 3.0

- Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
- GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
- Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)

Revision 1.3  2004/01/09 22:00:00 NicJA
* updated to work on Mesa 2.6
* Replaced VB->Unclipped[] with VB->ClipMask[]
*
Revision 1.2  2004/06/29 00:32:39  NicJA
*A few build related fixes (remove windows line endings in make scripts - python doesnt like them).

Still doesnt link to the examples correctly - allthought the linklibs seem to compile as expected??

Revision 1.1.1.1  2003/08/09 00:23:14  chodorowski
Amiga Mesa 2.2 ported to AROS by Nic Andrews. Build with 'make aros'. Not built by default.

 */


/*
TODO:
Dynamic allocate the vectorbuffer for polydrawing. (memory improvment)
implement shared list.
fix resizing bug.
some native asm routine
fast asm line drawin in db mode
fast asm clear       in db mode
read buffer rutines  in db-mode

IDEAS:
*/


/*
 * Note that you'll usually have to flip Y coordinates since Mesa's
 * window coordinates start at the bottom and increase upward.  Most
 * window system's Y-axis increases downward
 *
 * See dd.h for more device driver info.
 * See the other device driver implementations for ideas.
 *
 */

#include "AROSMesa_intern.h"
#include <GL/AROSMesa.h>

#include "AROSMesa_TC.h"

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
#include <inline/cybergraphics.h>
extern struct Library*  CyberGfxBase;
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#endif
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <cybergraphics/cybergraphics.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"

#define MAX_POLYGON 300

#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

#define DEBUG 1
#include <aros/debug.h>

extern void aros8bit_standard_DD_pointers( GLcontext *ctx );

/**********************************************************************/
/*****                Some Usefull code                                     *****/
/**********************************************************************/

/*
    The Drawing area is defined by:

    CC.Viewport.X = x;
    CC.Viewport.Width = width;
    CC.Viewport.Y = y;
    CC.Viewport.Height = height;
*/

static void arosTC_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height);
extern UBYTE* alloc_penbackarray( int width, int height, int bytes);
extern void destroy_penbackarray(UBYTE *buf);
extern struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
extern void destroy_rastport( struct RastPort *rp );
extern void AllocOneLine(struct arosmesa_context *AROSMesaCreateContext);
extern void FreeOneLine(struct arosmesa_context *AROSMesaCreateContext);

/*
 * Convert Mesa window coordinate(s) to AROS screen coordinate(s):
 */

#define FIXx(x) (amesa->left + (x))

#define FIXy(y) (amesa->RealHeight-amesa->bottom - (y))

#define FIXxy(x,y) ((amesa->RealWidth*FIXy(y)+FIXx(x)))

/**********************************************************************/
/*****                Miscellaneous device driver funcs                 *****/
/**********************************************************************/

static void arosTC_finish( void )
{
    /* implements glFinish if possible */
}

static void arosTC_flush( void )
{
    /* implements glFlush if possible */
}

/* implements glClearIndex - Set the color index used to clear the color buffer. */
static void arosTC_clear_index( GLcontext *ctx, GLuint index )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    amesa->clearpixel = amesa->penconv[index];
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_clear_index(c=%x,i=%d) = %x\n",ctx,index,amesa->clearpixel));
#endif
}

/* implements glClearColor - Set the color used to clear the color buffer. */
static void arosTC_clear_color( GLcontext *ctx,
                          GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
#warning TODO: Free pen color if not used
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    amesa->clearpixel=TC_RGBA(r,g,b,a);
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_clear_color(c=%x,r=%d,g=%d,b=%d,a=%d) = %x\n",ctx, r, g, b, a, amesa->clearpixel));
#endif
}

/*
* Clear the specified region of the color buffer using the clear color
* or index as specified by one of the two functions above.
*/
static GLbitfield arosTC_clear(GLcontext* ctx, GLbitfield mask,
                                GLboolean all, GLint x, GLint y, GLint width, GLint height)
{
    /*
    * If all==GL_TRUE, clear whole buffer
    */
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_clear(all:%d,x:%d,y:%d,w:%d,h:%d)\n",all,x,y,width,height));
#endif

    if(amesa->rp!=0)
    {
        if(all) FillPixelArray (amesa->rp,FIXx(ctx->Viewport.X),FIXy(ctx->Viewport.Y)-ctx->Viewport.Height+1,/*FIXx(ctx->Viewport.X)+*/ctx->Viewport.Width/*-1*/,ctx->Viewport.Height/*FIXy(ctx->Viewport.Y)*/,amesa->clearpixel);
        else FillPixelArray (amesa->rp,FIXx(x),FIXy(y)-height,width,FIXy(y),amesa->clearpixel);
    }
    else printf("Serious error amesa->rp=0 detected in arosTC_clear() in file cybmesa.c\n");

#warning TODO: I doubt this is correct. dd.h doesnt explain what this should be...
    return mask;
}

/* Set the current color index. */
static void arosTC_set_index( GLcontext *ctx,GLuint index )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

    /* Set the amesa color index. */
    amesa->pixel = amesa->penconv[index];
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_set_index(c=%x,i=%d) = %x\n", ctx, index,amesa->pixel));
#endif
}

static void arosTC_set_color( GLcontext *ctx,GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

    /* Set the current RGBA color. */
    /* r is in 0..255.RedScale */
    /* g is in 0..255.GreenScale */
    /* b is in 0..255.BlueScale */
    /* a is in 0..255.AlphaScale */
    amesa->pixel = TC_RGBA(r,g,b,a);
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_set_color(c=%x,r=%d,g=%d,b=%d,a=%d) = %x\n",ctx, r, g, b, a, amesa->pixel));
#endif
}

/* Set the index mode bitplane mask. */
static GLboolean arosTC_index_mask( GLcontext *ctx,GLuint mask )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;

    /* implement glIndexMask if possible, else return GL_FALSE */
D(bug("[AMESA:TC] : arosTC_index_mask(0x%x)\n",mask));
    amesa->rp->Mask = (UBYTE) mask;

    return(GL_TRUE);
}

/* Set the RGBA drawing mask. */
static GLboolean arosTC_color_mask( GLcontext *ctx,GLboolean rmask, GLboolean gmask,
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

static void arosTC_fast_points_function(GLcontext *ctx,GLuint first, GLuint last )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    struct vertex_buffer *VB = ctx->VB;
    int i,col;
    register struct RastPort * rp=amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_fast_points_function(f:%d, l:%d)\n", first, last));
#endif

    if (VB->MonoColor) {
        /* draw all points using the current color (set_color) */
/*      D(bug("[AMESA:TC] : VB->MonoColor\n"));*/
        for (i=first;i<=last;i++) {

           if (VB->ClipMask[i]==0) {
                /* compute window coordinate */
                int x, y;
                x =FIXx((GLint) (VB->Win[i][0]));
                y =FIXy((GLint) (VB->Win[i][1]));
                WriteRGBPixel(rp,x,y,amesa->pixel);
/*      printf("WriteRGBPixel(%d,%d)\n",x,y);*/
            }
        }
    } else {
        /* each point is a different color */
/*      D(bug("[AMESA:TC] : !VB.MonoColor\n"));*/

        for (i=first;i<=last;i++) {
            if ((VB->ClipMask[i]==0)) {
                int x, y;
                x =FIXx((GLint) (VB->Win[i][0]));
                y =FIXy((GLint) (VB->Win[i][1]));
                col=*VB->Color[i];
                WriteRGBPixel(rp,x,y,amesa->penconv[col]);
/*      D(bug("[AMESA:TC] : WriteRGBPixel(%d,%d)\n",x,y));*/
            }
        }
    }
}

static points_func arosTC_choose_points_function( GLcontext *ctx )
{
/*D(bug("[AMESA:TC] : arosTC_choose_points_function\n"));*/
    /* Examine the current rendering state and return a pointer to a */
    /* fast point-rendering function if possible. */
    if (ctx->Point.Size==1.0 && !ctx->Point.SmoothFlag && ctx->RasterMask==0
       && !ctx->Texture.Enabled /*&&  ETC, ETC */) {
        return arosTC_fast_points_function;
    }
    else {
        return NULL;
    }
}

/*
*  Render a line by some hardware/OS accerated method 
*/

static line_func arosTC_choose_line_function( GLcontext *ctx)
{
    /*D(bug("[AMESA:TC] : arosTC_choose_line_function\n"));*/

    /* Examine the current rendering state and return a pointer to a */
    /* fast line-rendering function if possible. */

    return NULL;
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
/*static polygon_func arosTC_choose_polygon_function( GLcontext *ctx )
{*/
    /* D(bug("[AMESA:TC] : arosTC_choose_polygon_function\n"));*/

    /* Examine the current rendering state and return a pointer to a */
    /* fast polygon-rendering function if possible. */

/*    return NULL;
}*/

/**********************************************************************/
/*****                 Span-based pixel drawing                   *****/
/**********************************************************************/

/* Write a horizontal span of 32-bit color-index pixels with a boolean mask. */
static void arosTC_write_ci32_span(const GLcontext *ctx,
                                GLuint n, GLint x, GLint y,
                                const GLuint index[],
                                const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp=NULL;
    unsigned long * penconv=amesa->penconv;
    register struct RastPort * rp=amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_ci32_span(n:%d,x:%d,y:%d)\n",n,x,y));
#endif
    
    y=FIXy(y);
    x=FIXx(x);
    if((dp = (ULONG*)amesa->imageline))
    {                     /* if imageline has been
                        allocated then use fastversion */

        ant=0;
        for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
            if (mask[i]) {
                ant++;
/*              x++;*/
                *dp = penconv[index[i]];dp++;
            } else {
                if(ant)
D(bug("[AMESA:TC] : arosTC_write_index_span: WritePixelArray()\n"));
                    WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
                dp=(ULONG*)amesa->imageline;
                ant=0;
                x++;
            }
        }
        if(ant)
D(bug("[AMESA:TC] : arosTC_write_index_span: WritePixelArray()\n"));
            WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);

    } else {            /* Slower */
            for (i=0;i<n;i++,x++) {
                if (mask[i]) {
                 /* draw pixel (x[i],y[i]) using index[i] */
                WriteRGBPixel(rp,x,y,penconv[index[i]]);
            }
        }
    }
}

/* Write a horizontal span of 8-bit color-index pixels with a boolean mask. */
static void arosTC_write_ci8_span(const GLcontext *ctx,
                                GLuint n, GLint x, GLint y,
                                const GLubyte index[],
                                const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp=NULL;
    unsigned long * penconv=amesa->penconv;
    register struct RastPort * rp=amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_ci8_span(n:%d,x:%d,y:%d)\n",n,x,y));
#endif

    y=FIXy(y);
    x=FIXx(x);
    if((dp = (ULONG*)amesa->imageline))
    {                     /* if imageline have been
                        allocated then use fastversion */

        ant=0;
        for (i=0;i<n;i++) { /* draw pixel (x[i],y[i]) using index[i] */
            if (mask[i]) {
                ant++;
/*              x++;*/
                *dp = penconv[index[i]];dp++;
            } else {
                if(ant)
D(bug("[AMESA:TC] : arosTC_write_index_span: WritePixelArray()\n"));
                    WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
                dp=(ULONG*)amesa->imageline;
                ant=0;
                x++;
            }
        }
        if(ant)
D(bug("[AMESA:TC] : arosTC_write_index_span: WritePixelArray()\n"));
            WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);

    } else {            /* Slower */
            for (i=0;i<n;i++,x++) {
                if (mask[i]) {
                 /* draw pixel (x[i],y[i]) using index[i] */
                WriteRGBPixel(rp,x,y,penconv[index[i]]);
            }
        }
    }
}

/* Write a horizontal span of RGBA color pixels with a boolean mask. */
static void arosTC_write_rgba_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                const GLubyte rgba[][4], const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp=NULL;
    register struct RastPort * rp = amesa->rp;

    y=FIXy(y);
    x=FIXx(x);

#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_color_span(ant=%d,x=%d,y=%d)",n,x,y));
#endif

    if((dp = (ULONG*)amesa->imageline))
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
                    *dp = TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]);dp++;
                }
                else
                {
                    if(ant)
                    {
                        WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
/*D(bug("[AMESA:TC] : WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
                        dp=(ULONG*)amesa->imageline;
                        ant=0;
                        x=x+ant;
                    }
                    x++;
                }
            }

            if(ant)
            {
                WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
/*D(bug("[AMESA:TC] : WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
            }
        }
        else
        {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
            for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
            {
                *dp = TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]);dp++;
            }
            WritePixelArray(amesa->imageline,0,0,4*n,rp,x,y,n,1,RECTFMT_ARGB);
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
                    WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]));
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
                WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]));
            }
        }
    }
}

/* Write a horizontal span of RGB color pixels with a boolean mask. */
static void arosTC_write_rgb_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                const GLubyte rgba[][3], const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp=NULL;
    register struct RastPort * rp = amesa->rp;

    y=FIXy(y);
    x=FIXx(x);
    
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_color_span(ant=%d,x=%d,y=%d)",n,x,y));
#endif

    if((dp = (ULONG*)amesa->imageline))
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
                    *dp = TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],0xff);dp++;
                }
                else
                {
                    if(ant)
                    {
                        WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
/*D(bug("[AMESA:TC] : WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
                        dp=(ULONG*)amesa->imageline;
                        ant=0;
                        x=x+ant;
                    }
                    x++;
                }
            }

            if(ant)
            {
                WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
/*D(bug("[AMESA:TC] : WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
            }
        }
        else
        {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
            for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
            {
                *dp = TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],0xff);dp++;
            }
            WritePixelArray(amesa->imageline,0,0,4*n,rp,x,y,n,1,RECTFMT_ARGB);
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
                    WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],0xff));
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
                WriteRGBPixel(rp,x,y,TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],0xff));
            }
        }
    }
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current
* color index is used for all pixels.
*/
static void arosTC_write_mono_ci_span(const GLcontext* ctx,
                               GLuint n,GLint x,GLint y,
                               const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,j;
    y=FIXy(y);
    x=FIXx(x);

#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_mono_ci_span\n"));
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
            j=0;
            while(mask[i] && i<n)
            {
                i++;j++;
            }
            FillPixelArray (amesa->rp,x,y,j,1,amesa->pixel);
        }
    }
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current color
* is used for all pixels.
*/

static void arosTC_write_mono_rgba_span( const GLcontext *ctx,
                                      GLuint n, GLint x, GLint y,
                                      const GLubyte mask[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,j;
    register struct RastPort * rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_mono_rgba_span(n:%d,x:%d,y:%d)\n",n,x,y));
#endif

    y=FIXy(y);
    x=FIXx(x);
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
            j = 0;
            while(mask[i] && i<n)
            {
                i++;
                j++;
            }
            FillPixelArray(rp,x,y,j,1,amesa->pixel);
        }
    }
}

/**********************************************************************/
/*****                    Read spans of pixels                              *****/
/**********************************************************************/

/* Read a horizontal span of color-index pixels. */
static void arosTC_read_ci32_span( const GLcontext* ctx, GLuint n, GLint x, GLint y,
                            GLuint index[])
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;

#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_read_ci32_span>>\n"));
#endif

    y=FIXy(y);
    x=FIXx(x);

    /* Currently this is wrong ARGB-values are NO indexes !!!*/
    if(amesa->imageline) {
        ReadPixelArray(amesa->imageline,0,0,4*n,amesa->rp,x,y,n,1,RECTFMT_ARGB);
        for(i=0; i<n; i++)
            index[i]=((ULONG*)amesa->imageline)[i];
    } else {
        for (i=0; i<n; i++,x++)
            index[i] = ReadRGBPixel(amesa->rp,x,y);
    }
}

static void MyReadPixelArray(UBYTE *a, ULONG b, ULONG c, ULONG d, struct RastPort *e,ULONG f, ULONG g,ULONG h, ULONG i,ULONG j)
{
    ReadPixelArray(a,b,c,d,e,f,g,h,i,j);
}

/* Read a horizontal span of color pixels. */
static void arosTC_read_rgba_span( const GLcontext* ctx,
                            GLuint n, GLint x, GLint y,
                            GLubyte rgba[][4] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    ULONG col;

#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_read_color_span>>\n"));
#endif

    y=FIXy(y);
    x=FIXx(x);

    if(amesa->imageline) {
        MyReadPixelArray(amesa->imageline,0,0,n*4,amesa->rp,x,y,n,1,RECTFMT_ARGB);
        for(i=0; i<n; i++) {
            col = ((ULONG*)amesa->imageline)[i];
            rgba[i][RCOMP] = (col & 0xff0000)>>16;
            rgba[i][GCOMP] = (col & 0xff00)>>8;
            rgba[i][BCOMP] = col & 0xff;
            rgba[i][ACOMP] = (col & 0xff000000)>>24;
        }
    } else
        for (i=0; i<n; i++, x++) {
            col=ReadRGBPixel(amesa->rp,x,y);
            rgba[i][RCOMP] = (col & 0xff0000)>>16;
            rgba[i][GCOMP] = (col & 0xff00)>>8;
            rgba[i][BCOMP] = col & 0xff;
            rgba[i][ACOMP] = (col & 0xff000000)>>24;
            }
}

/**********************************************************************/
/*****                   Array-based pixel drawing                *****/
/**********************************************************************/

/* Write an array of 32-bit index pixels with a boolean mask. */
static void arosTC_write_ci32_pixels( const GLcontext* ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLuint index[], const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_ci32_pixels<\n"));
#endif

    for (i=0; i<n; i++)
    {
        if (mask[i])
        {
            WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),amesa->pixel);
        }
    }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* index is used for all pixels.
*/
static void arosTC_write_mono_ci_pixels( const GLcontext* ctx,
                                  GLuint n,
                                  const GLint x[], const GLint y[],
                                  const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_mono_ci_pixels<\n"));
#endif

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using current index  */
            WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),amesa->pixel);

        }
    }
}

/* Write an array of RGBA pixels with a boolean mask. */
static void arosTC_write_rgba_pixels( const GLcontext* ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLubyte rgba[][4], const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_rgba_pixels<\n"));
#endif

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
            WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),TC_RGBA(rgba[i][RCOMP],rgba[i][GCOMP],rgba[i][BCOMP],rgba[i][ACOMP]));
        }
    }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* is used for all pixels.
*/
static void arosTC_write_mono_rgba_pixels( const GLcontext* ctx,
                                    GLuint n,
                                    const GLint x[], const GLint y[],
                                    const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_write_mono_rgba_pixels<\n"));
#endif

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using current color*/
            WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),amesa->pixel);
        }
    }
}

/**********************************************************************/
/*****                       Read arrays of pixels                          *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void arosTC_read_ci32_pixels( const GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLuint index[], const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_read_ci32_pixels-\n"));
#endif

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*       index[i] = read_pixel x[i], y[i] */
            index[i] = ReadRGBPixel(rp,FIXx(x[i]),FIXy(y[i])); /* that's wrong! we get ARGB!*/
        }
    }
}

/* Read an array of color pixels. */
static void arosTC_read_rgba_pixels( const GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLubyte rgba[][4], const GLubyte mask[] )
{
    AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx;
    int i,col;
    register struct RastPort * rp = amesa->rp;

/*  ULONG ColTab[3];*/
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_read_rgba_pixels-\n"));
#endif

    for (i=0; i<n; i++)
    {
        if (mask[i])
        {
            col=ReadRGBPixel(rp,FIXx(x[i]),FIXy(y[i]));

            rgba[i][RCOMP] = (col&0xff0000)>>16;
            rgba[i][GCOMP] = (col&0xff00)>>8;
            rgba[i][BCOMP] = col&0xff;
            rgba[i][ACOMP] = (col&0xff000000)>>24;
        }
    }
}

/**********************************************************************/
/**********************************************************************/

GLboolean arosTC_logicop( GLcontext* ctx, GLenum op )
{
    /* can't implement */
    return GL_FALSE;
}

static void arosTC_dither( GLcontext* ctx, GLboolean enable )
{
    /* currently unsuported */
}

static GLboolean arosTC_set_buffer( GLcontext *ctx,GLenum mode )
{
    /* set the current drawing/reading buffer, return GL_TRUE or GL_FALSE */
    /* for success/failure */
    
/*   AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx; */
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_set_buffer TODO\n"));
#endif
    
/* TODO implemed a set of buffers */
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

static const char *arosTC_renderer_string(void)
{
   return "AROS TrueColor";
}

/**********************************************************************/
/**********************************************************************/
    /* Initialize all the pointers in the DD struct.  Do this whenever   */
    /* a new context is made current or we change buffers via set_buffer! */
void arosTC_standard_DD_pointers( GLcontext *ctx )
{
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : Inside arosTC_standard_DD_pointers : ctx=0x%x, CC=0x%x\n",ctx,CC));
#endif
    ctx->Driver.RendererString = arosTC_renderer_string;
    ctx->Driver.UpdateState = arosTC_standard_DD_pointers;
    ctx->Driver.GetBufferSize = arosTC_Standard_resize;
    ctx->Driver.Finish = arosTC_finish;
    ctx->Driver.Flush = arosTC_flush;

    ctx->Driver.ClearIndex = arosTC_clear_index;
    ctx->Driver.ClearColor = arosTC_clear_color;
    ctx->Driver.Clear = arosTC_clear;
    
    ctx->Driver.Index = arosTC_set_index;
    ctx->Driver.Color = arosTC_set_color;
    ctx->Driver.IndexMask = arosTC_index_mask;
    ctx->Driver.ColorMask = arosTC_color_mask;

    ctx->Driver.LogicOp = arosTC_logicop;
    ctx->Driver.Dither = arosTC_dither;

    ctx->Driver.SetBuffer = arosTC_set_buffer;
//    ctx->Driver.GetBufferSize = arosTC_Standard_resize;
    
    ctx->Driver.PointsFunc = arosTC_choose_points_function( ctx );
    ctx->Driver.LineFunc = arosTC_choose_line_function( ctx );
#warning TODO: Write Triangle operations!!
    ctx->Driver.TriangleFunc = FALSE;
    
#warning TODO: Replace with RGBA functions [IMPORTANT!]
    /* Pixel/span writing functions: */
    ctx->Driver.WriteRGBASpan        = arosTC_write_rgba_span;
    ctx->Driver.WriteRGBSpan         = arosTC_write_rgb_span;
    ctx->Driver.WriteMonoRGBASpan    = arosTC_write_mono_rgba_span;
    ctx->Driver.WriteRGBAPixels      = arosTC_write_rgba_pixels;
    ctx->Driver.WriteMonoRGBAPixels  = arosTC_write_mono_rgba_pixels;
    ctx->Driver.WriteCI32Span        = arosTC_write_ci32_span;
    ctx->Driver.WriteCI8Span         = arosTC_write_ci8_span;
    ctx->Driver.WriteMonoCISpan      = arosTC_write_mono_ci_span;
    ctx->Driver.WriteCI32Pixels      = arosTC_write_ci32_pixels;
    ctx->Driver.WriteMonoCIPixels    = arosTC_write_mono_ci_pixels;
    
    /* Pixel/span reading functions: */
    ctx->Driver.ReadCI32Span        = arosTC_read_ci32_span;
    ctx->Driver.ReadRGBASpan        = arosTC_read_rgba_span;
    ctx->Driver.ReadCI32Pixels      = arosTC_read_ci32_pixels;
    ctx->Driver.ReadRGBAPixels      = arosTC_read_rgba_pixels;
}

/**********************************************************************/
/*****                  AROS/Mesa private misc procedures        *****/
/**********************************************************************/

static void MyWritePixelArray(void *a,int b,int c,int d,struct RastPort *e,int f,int g, int h, int i, int j)
{
    WritePixelArray(a,b,c,d,e,f,g,h,i,j);
}

/**********************************************************************/
/***** AROSMesaSetOneColor                                       *****/
/**********************************************************************/
void AROSMesaSetOneColor(struct arosmesa_context *c,int index, float r, float g, float b)
{
    if (c->depth>8) {
        c->penconv[index] = TC_RGBA((int)r*255,(int)g*255,(int)b*255,255);
    } else {
        c->penconv[index] = RGBA(c,r*255,g*255,b*255,255);
    }
}

static void
arosTC_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height)
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
        amesa->depth = GetCyberMapAttr(amesa->front_rp->BitMap,CYBRMATTR_DEPTH);

        if (amesa->visual->db_flag)
        {
            if (amesa->back_rp) destroy_rastport(amesa->back_rp); /* Free double buffer */
            if((amesa->back_rp = make_rastport(amesa->RealWidth,amesa->RealHeight,amesa->depth,amesa->rp->BitMap))==NULL) {
                amesa->rp = amesa->front_rp;
                printf("AROSMesa Error : To little mem free. Couldn't allocate Dubblebuffer in this size.\n");
            } else {
                amesa->rp=amesa->back_rp;
            }
        }

        AllocOneLine(amesa);
    }
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_Standard_resize(ctx=%x) = w:%d x h:%d\n",ctx,*width,*height));
#endif
      
#ifdef DEBUGPRINT
        printf("amesa->RealWidth =%d\n",amesa->RealWidth);
        printf("amesa->RealHeight=%d\n",amesa->RealHeight);
        printf("amesa->width =%d\n",amesa->width);
        printf("amesa->height=%d\n",amesa->height);
        printf("amesa->left  =%d\n",amesa->left);
        printf("amesa->bottom=%d\n",amesa->bottom);
        printf("amesa->depth =%d\n",amesa->depth);
#endif
}

/**********************************************************************/
/*****                  AROS/Mesa private init/despose/resize       *****/
/**********************************************************************/

BOOL
arosTC_Standard_init(struct arosmesa_context *c,struct TagItem *tagList)
{
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : arosTC_Standard_init(amctx=%x,taglist=%x)\n",c,tagList));
#endif
    c->window=(struct Window *)GetTagData(AMA_Window,0,tagList);
    if (!c->window)
    {
        c->rp=(struct RastPort *)GetTagData(AMA_RastPort,0,tagList);
        if (!c->rp)
        {
            LastError=AMESA_RASTPORT_TAG_MISSING;
            return(FALSE);
        }
        c->Screen=(struct Screen *)GetTagData(AMA_Screen,0,tagList);
        if (!c->Screen)
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

    c->left = GetTagData(AMA_Left,0,tagList);
    c->bottom= GetTagData(AMA_Bottom,0,tagList);

    c->front_rp =c->rp;
    c->back_rp=NULL;
/*  c->rp = c->front_rp;*/

    c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
    c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);

    if (CyberGfxBase!=NULL && IsCyberModeID(GetVPModeID(&c->Screen->ViewPort)))
    {
        c->depth = GetCyberMapAttr(c->rp->BitMap,CYBRMATTR_DEPTH);
    } 
    else
    {
        c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);
    }

/*  c->gl_ctx->BufferWidth = c->width;*/
/*  c->gl_ctx->BufferHeight = c->height;*/

    c->pixel = 0;   /* current drawing pen */

    if (c->depth<=8)
    {
        AllocCMap(c->Screen);
    }

    if (c->visual->db_flag==GL_TRUE)
    {
#ifdef DEBUGPRINT
D(bug("[AMESA:TC] : Doublebuff inside arosTC_Standard_init"));
#endif
        if((c->back_rp = make_rastport(c->RealWidth,c->RealHeight,c->depth, c->rp->BitMap))!=NULL)
        {
            c->gl_ctx->Color.DrawBuffer = GL_BACK;
            c->rp = c->back_rp;
        }
        else
        {
            printf("make_rastport Failed\n");
            c->gl_ctx->Color.DrawBuffer = GL_FRONT;
        }
    }
    else
    {
      c->gl_ctx->Color.DrawBuffer = GL_FRONT;
    }
    AllocOneLine(c); /* A linebuffer for WritePixelLine */

/* this shall not be invoked att all if not cybergfx
    if (c->depth<=8) {
        if (!make_temp_raster( c->rp ))
            printf("Error allocating TmpRastPort\n");*/
        /* JAM: Added alloc_temp_rastport */
/*        alloc_temp_rastport(c);
        allocarea(c->rp);
    }
*/
    c->InitDD=c->depth<=8 ? aros8bit_standard_DD_pointers : arosTC_standard_DD_pointers;  /*standard drawing*/
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
#include "AROSMesa_TC_db.c"
#endif*/

#undef DEBUGPRINT
