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
Revision 1.2  2004/06/29 00:32:39  NicJA
A few build related fixes (remove windows line endings in make scripts - python doesnt like them).

Still doesnt link to the examples correctly - allthought the linklibs seem to compile as expected??

Revision 1.1.1.1  2003/08/09 00:23:14  chodorowski
Amiga Mesa 2.2 ported to AROS by Nic Andrews. Build with 'make aros'. Not built by default.

 */


/*
TODO:
Dynamic allocate the vectorbuffer for polydrawing. (memory improvment)
implement shared list.
fix resizing bug.
some native asm rutine
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

#include <GL/AmigaMesa.h>

#ifdef ADISP_CYBERGFX
/* This ifdef outcomment the whole file if you havent specified ADISP_CYBERGFX flag */

#include "ADisp_Cyb.h"

#ifdef __GNUC__
#include "../amiga/misc/ht_colors.h"
#else
#include "/amiga/misc/ht_colors.h"
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

/*#define DEBUGPRINT*/

#ifdef DEBUGPRINT
#define DEBUGOUT(x) printf(x);
#else
#define DEBUGOUT(x) /*printf(x);*/
#endif

extern void amiga_Faster_DD_pointers( GLcontext *ctx );
extern void amiga_standard_DD_pointers( GLcontext *ctx );

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

static void Cyb_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height);
static void Cyb_Standard_resize_db( GLcontext *ctx,GLuint *width, GLuint *height);
extern UBYTE* alloc_penbackarray( int width, int height, int bytes);
extern void destroy_penbackarray(UBYTE *buf);
extern struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
extern void destroy_rastport( struct RastPort *rp );
extern void AllocOneLine(struct amigamesa_context *AmigaMesaCreateContext);
extern void FreeOneLine(struct amigamesa_context *AmigaMesaCreateContext);
/*#define RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)*/

#define FIXx(x) (amesa->left + (x))

#define FIXy(y) (amesa->RealHeight-amesa->bottom - (y))

#define FIXxy(x,y) ((amesa->RealWidth*FIXy(y)+FIXx(x)))

/**********************************************************************/
/*****                Miscellaneous device driver funcs                 *****/
/**********************************************************************/

static void cyb_finish( void )
{
    /* implements glFinish if possible */
}


static void cyb_flush( void )
{
    /* implements glFlush if possible */
}


static void cyb_clear_index( GLcontext *ctx, GLuint index )
{
   /* implement glClearIndex */
   /* usually just save the value in the context struct */
   /*printf("aclear_index=glClearIndex=%d\n",index);*/
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
   amesa->clearpixel = amesa->penconv[index];
}


static void cyb_clear_color( GLcontext *ctx,
                          GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   /* implement glClearColor */
   /* color components are floats in [0,1] */
   /* usually just save the value in the context struct */
   /*printf("aclear_color=glClearColor(%d,%d,%d,%d)\n",r,g,b,a);*/
   /* @@@ TODO FREE COLOR IF NOT USED */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
   amesa->clearpixel=TC_RGBA(r,g,b,a);
}




static void cyb_clear( GLcontext *ctx,GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
/*
 * Clear the specified region of the color buffer using the clear color
 * or index as specified by one of the two functions above.
 * If all==GL_TRUE, clear whole buffer
 */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DEBUGPRINT
printf("cyb_clear(%d,%d,%d,%d,%d)\n",all,x,y,width,height);
#endif


    if(all)
        {
        FillPixelArray (amesa->rp,FIXx(ctx->Viewport.X),FIXy(ctx->Viewport.Y)-ctx->Viewport.Height+1,/*FIXx(ctx->Viewport.X)+*/ctx->Viewport.Width/*-1*/,ctx->Viewport.Height/*FIXy(ctx->Viewport.Y)*/,amesa->clearpixel);
        }
    else
        {
        if(amesa->rp!=0)
            {
            FillPixelArray (amesa->rp,FIXx(x),FIXy(y)-height,width,FIXy(y),amesa->clearpixel);
            }
        else
            printf("Serius error amesa->rp=0 detected in cyb_clear() in file cybmesa.c\n");
        }
    
}

static void cyb_clear_db(GLcontext *ctx, GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
/*
 * Clear the specified region of the color buffer using the clear color
 * or index as specified by one of the two functions above.
 * If all==GL_TRUE, clear whole buffer
 */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
/*  int i; */
    register ULONG *db;
    register ULONG col=amesa->clearpixel;


#ifdef DEBUGPRINT
printf("cyb_clear_db(%d,%d,%d,%d,%d)\n",all,x,y,width,height);
#endif

    if(all)
        {
        int x1,y1,x2,y2;
/*      RectFill(amesa->rp,FIXx(CC.Viewport.X),FIXy(CC.Viewport.Y)-CC.Viewport.Height+1,FIXx(CC.Viewport.X)+CC.Viewport.Width-1,FIXy(CC.Viewport.Y));*/

        x1=0;
        y1=0;
        x2=amesa->RealWidth;
        y2=amesa->RealHeight;
        db=(ULONG *)amesa->BackArray;
        for(y1=0;y1<y2;y1++)
            {
            for(x1=0;x1<x2;x1++)
                {
                *db=col;db++;
                }
            }
        }
/*  return(((((amesa->RealWidth+15)>>4)<<4)*FIXy(y)+FIXx(x)));*/
    else
        {
        int x1,y1,x2,y2;
/*      RectFill(amesa->rp,FIXx(x),FIXy(y)-height,width,FIXy(y));*/
        x2=FIXx(x+width);
        y2=FIXy(y);

        for(y1=FIXy(y)-height;y1<y2;y1++)
            {
            for(x1=FIXx(x);x1<x2;x1++)
                {
/*printf("clear byte 0x%x\n",&(amesa->BackArray[x1+y1*(((amesa->RealWidth+15)>>4)<<4)]));*/
                ((ULONG*)amesa->BackArray)[x1+y1*amesa->RealWidth]=col;
                }
            }
        }
}



static void cyb_set_index( GLcontext *ctx,GLuint index )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    /* Set the amesa color index. */
/*printf("aset_index(%d)\n",index);*/
    amesa->pixel = amesa->penconv[index];
}



static void cyb_set_color( GLcontext *ctx,GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

/*printf("cyb_set_color(%d,%d,%d,%d)\n",r,g,b,a);*/

        /* Set the current RGBA color. */
        /* r is in 0..255.RedScale */
        /* g is in 0..255.GreenScale */
        /* b is in 0..255.BlueScale */
        /* a is in 0..255.AlphaScale */
    amesa->pixel = TC_RGBA(r,g,b,a);
}


static GLboolean cyb_index_mask( GLcontext *ctx,GLuint mask )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    /* implement glIndexMask if possible, else return GL_FALSE */
/*printf("cyb_index_mask(0x%x)\n",mask);*/
    amesa->rp->Mask = (UBYTE) mask;

    return(GL_TRUE);
}

static GLboolean cyb_color_mask( GLcontext *ctx,GLboolean rmask, GLboolean gmask,
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

static void cyb_fast_points_function(GLcontext *ctx,GLuint first, GLuint last )
{


    AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    struct vertex_buffer *VB = ctx->VB;

    int i,col;
    register struct RastPort * rp=amesa->rp;
/* printf("cyb_fast_points_function\n");*/

        if (VB->MonoColor) {
        /* draw all points using the current color (set_color) */
/*      printf("VB->MonoColor\n");*/
        for (i=first;i<=last;i++) {
            if (VB->Unclipped[i]) {
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
/*      printf("!VB.MonoColor\n");*/

        for (i=first;i<=last;i++) {
            /*if (VB.Unclipped[i])*/ {
                int x, y;
                x =FIXx((GLint) (VB->Win[i][0]));
                y =FIXy((GLint) (VB->Win[i][1]));
                col=*VB->Color[i];
            WriteRGBPixel(rp,x,y,amesa->penconv[col]);
/*      printf("WriteRGBPixel(%d,%d)\n",x,y);*/
            }
        }
    }
}



static points_func cyb_choose_points_function( GLcontext *ctx )
{
/*printf("cyb_choose_points_function\n");*/
    /* Examine the current rendering state and return a pointer to a */
    /* fast point-rendering function if possible. */
   if (ctx->Point.Size==1.0 && !ctx->Point.SmoothFlag && ctx->RasterMask==0
       && !ctx->Texture.Enabled /*&&  ETC, ETC */) {
      return cyb_fast_points_function;
   }
   else {
      return NULL;
   }
}

static points_func cyb_choose_points_function_db( GLcontext *ctx )
    {
    return NULL;
    }


 /*
  *  Render a line by some hardware/OS accerated method 
  */

static line_func cyb_choose_line_function( GLcontext *ctx)
{
    /*printf("cyb_choose_line_function\n");*/

    /* Examine the current rendering state and return a pointer to a */
    /* fast line-rendering function if possible. */

    return NULL;
}

static line_func cyb_choose_line_function_db( GLcontext *ctx)
    {
    return NULL;
    }

/*
 * Draw a filled polygon of a single color. If there is hardware/OS support
 * for polygon drawing use that here.   Otherwise, call a function in
 * polygon.c to do the drawing.
 */

static polygon_func cyb_choose_polygon_function( GLcontext *ctx )
{
    /* printf("cyb_choose_polygon_function\n");*/

    /* Examine the current rendering state and return a pointer to a */
    /* fast polygon-rendering function if possible. */

    return NULL;
}

/**********************************************************************/
/*****                  Optimized polygon rendering               *****/
/**********************************************************************/


static polygon_func cyb_choose_polygon_function_db( GLcontext *ctx )
{
    return NULL;
}


/**********************************************************************/
/*****              Write spans of pixels                                   *****/
/**********************************************************************/

static void cyb_write_index_span_db(GLcontext *ctx, GLuint n, GLint x, GLint y,
                                            const GLuint index[],
                                            const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    int i/* ,ant*/;
    register ULONG *db;
    register unsigned long * penconv=amesa->penconv;


#ifdef DEBUGPRINT
printf("cyb_write_index_span_db(%d,%d,%d)\n",n,x,y);
#endif

    db=&((ULONG*)amesa->BackArray)[FIXxy(x,y)];

    /* Slower */
    if(mask)
        {
        for (i=0;i<n;i++)
            {
            if(mask[i])
                {
                /* draw pixel (x[i],y[i]) using index[i] */
                *db=penconv[index[i]];
                }
                db++;
            }
        }
    else
        for (i=0;i<n;i++)
            {
            /* draw pixel (x[i],y[i]) using index[i] */
            *db=penconv[index[i]];db++;
            }
    }


static void cyb_write_color_span_db(GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte red[], const GLubyte green[],
                                        const GLubyte blue[], const GLubyte alpha[],
                                        const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i/*,ant*/;
    register ULONG *db;

#ifdef DEBUGPRINT
printf("cyb_write_color_span_db(%d,%d,%d)\n",n,x,y);
#endif

    db=&((ULONG*)amesa->BackArray)[FIXxy(x,y)];
/* Slower */
    if (mask)
        {
        for (i=0;i<n;i++)
            {
            if (mask[i])
                {
                /* draw pixel (x[i],y[i]) using index[i] */
                *db=TC_RGBA(red[i],green[i],blue[i],alpha[i]);
                }
                db++;
            }
        }
    else
        {
        for (i=0;i<n;i++)
            {
            /* draw pixel (x[i],y[i]) using index[i] */
            *db=TC_RGBA(red[i],green[i],blue[i],alpha[i]);db++;
            }
        }
    
}
static void cyb_write_monoindex_span_db( GLcontext *ctx,GLuint n, GLint x, GLint y,const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i/*,ant*/;
    register ULONG *db;

#ifdef DEBUGPRINT
printf("awrite_monoindex_span_db(%d,%d,%d)\n",n,x,y);
#endif

db=&((ULONG*)amesa->BackArray)[FIXxy(x,y)];

    /* Slower */
if(mask)
    {
    for (i=0;i<n;i++)
        {
        if (mask[i])
            {
            /* draw pixel (x[i],y[i]) using index[i] */
            *db=amesa->pixel;
            }
            db++;
        }
    }
else
    for (i=0;i<n;i++)
        {
        /* draw pixel (x[i],y[i]) using index[i] */
        *db=amesa->pixel;db++;
        }
}


static void cyb_write_index_span(GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLuint index[],
                                        const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp;
    unsigned long * penconv=amesa->penconv;
    register struct RastPort * rp=amesa->rp;

/*printf("cyb_write_index_span(%d,%d,%d)\n",n,x,y);*/
DEBUGOUT("cyb_write_index_span(n,x,y)\n")

    y=FIXy(y);
    x=FIXx(x);
    if(dp = (ULONG*)amesa->imageline)
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
DEBUGOUT("cyb_write_index_span: WritePixelArray()\n")
                    WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
                dp=(ULONG*)amesa->imageline;
                ant=0;
                x++;
            }
        }
        if(ant)
DEBUGOUT("cyb_write_index_span: WritePixelArray()\n")
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


static void cyb_write_color_span( GLcontext *ctx,GLuint n, GLint x, GLint y,
                                        const GLubyte red[], const GLubyte green[],
                                        const GLubyte blue[], const GLubyte alpha[],
                                        const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,ant;
    ULONG *dp;

    register struct RastPort * rp = amesa->rp;

/*DEBUGOUT("cyb_write_color_span(n,x,y) ")*/
    
    y=FIXy(y);
    x=FIXx(x);
/*printf("cyb_write_color_span(ant=%d,x=%d,y=%d)",n,x,y);*/

    if(dp = (ULONG*)amesa->imageline)
    {           /* if imageline allocated then use fastversion */
DEBUGOUT("FAST ")
        if (mask)
            {
DEBUGOUT("mask\n")
            ant=0;
            for (i=0;i<n;i++)           /* draw pixel (x[i],y[i]) */
                {
                if (mask[i])
                    {
                    ant++;
                    *dp = TC_RGBA(red[i],green[i],blue[i],alpha[i]);dp++;
                    }
                else
                    {
                    if(ant)
                        {
                        WritePixelArray(amesa->imageline,0,0,4*ant,rp,x,y,ant,1,RECTFMT_ARGB);
/*printf("WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y);*/
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
/*printf("WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y);*/
                }
            }
        else
            {
DEBUGOUT("nomask\n")
            for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
                {
                *dp = TC_RGBA(red[i],green[i],blue[i],alpha[i]);dp++;
                }
            WritePixelArray(amesa->imageline,0,0,4*n,rp,x,y,n,1,RECTFMT_ARGB);
            }
        
        }
    else
        {    /* Slower version */
DEBUGOUT("FAST\n")
        if (mask)
            {
DEBUGOUT("mask\n")
            /* draw some pixels */
            for (i=0; i<n; i++, x++)
                {
                if (mask[i])
                    {
                    /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                    WriteRGBPixel(rp,x,y,TC_RGBA(red[i],green[i],blue[i],alpha[i]));
                    }
                }
            }
        else
            {
DEBUGOUT("nomask\n")
            /* draw all pixels */
            for (i=0; i<n; i++, x++)
                {
                /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
                WriteRGBPixel(rp,x,y,TC_RGBA(red[i],green[i],blue[i],alpha[i]));
                }
            }
        }
}




static void cyb_write_monoindex_span(GLcontext *ctx,GLuint n,GLint x,GLint y,const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,j;
    y=FIXy(y);
    x=FIXx(x);


DEBUGOUT("cyb_write_monoindex_span\n")


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

static void cyb_write_monocolor_span( GLcontext *ctx,GLuint n, GLint x, GLint y,
                                             const GLubyte mask[])
{
    AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,j;
    register struct RastPort * rp = amesa->rp;

/*printf("cyb_write_monocolor_span(%d,%d,%d)\n",n,x,y);*/
DEBUGOUT("cyb_write_monocolor_span(n,x,y)\n")

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

/* Here we should check if the size of the colortable is <256 */

static void cyb_read_index_span( GLcontext *ctx,GLuint n, GLint x, GLint y, GLuint index[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    int i;

DEBUGOUT("cyb_read_index_span>>\n")


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

static void cyb_read_color_span( GLcontext *ctx,GLuint n, GLint x, GLint y,
                                      GLubyte red[], GLubyte green[],
                                      GLubyte blue[], GLubyte alpha[] )
{
    AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    ULONG col;

DEBUGOUT("cyb_read_color_span>>\n")


    y=FIXy(y);
    x=FIXx(x);

    if(amesa->imageline) {
        MyReadPixelArray(amesa->imageline,0,0,n*4,amesa->rp,x,y,n,1,RECTFMT_ARGB);
        for(i=0; i<n; i++) {
            col = ((ULONG*)amesa->imageline)[i];
            red[i] = (col & 0xff0000)>>16;
            green[i] = (col & 0xff00)>>8;
            blue[i] = col & 0xff;
            alpha[i]= (col & 0xff000000)>>24;
        }
    } else
        for (i=0; i<n; i++, x++) {
            col=ReadRGBPixel(amesa->rp,x,y);
            red[i] = (col & 0xff0000)>>16;
            green[i] = (col & 0xff00)>>8;
            blue[i] = col & 0xff;
            alpha[i]= (col & 0xff000000)>>24;
            }
}




/**********************************************************************/
/*****                Write arrays of pixels                      *****/
/**********************************************************************/

static void cyb_write_index_pixels_db( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                          const GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

    int i;
/*  register struct RastPort * rp = amesa->rp;*/
DEBUGOUT("awrite_index_pixels<\n")

    for (i=0; i<n; i++)
        {
        if (mask[i])
            {
            ((ULONG*)amesa->BackArray)[FIXxy(x[i],y[i])]=amesa->penconv[index[i]];
            }
        }
}

static void cyb_write_monoindex_pixels_db( GLcontext *ctx,GLuint n,
                                                const GLint x[], const GLint y[],
                                                const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
/*  register struct RastPort * rp = amesa->rp;*/
DEBUGOUT("awrite_index_pixels<\n")

    for (i=0; i<n; i++)
        {
        if (mask[i])
            {
            ((ULONG*)amesa->BackArray)[FIXxy(x[i],y[i])]=amesa->pixel;
            }
        }
}

static void cyb_write_color_pixels_db(GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte r[], const GLubyte g[],
                                              const GLubyte b[], const GLubyte a[],
                                              const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
/*  register struct RastPort * rp = amesa->rp;*/
DEBUGOUT("cyb_write_index_pixels<\n")

    for (i=0; i<n; i++)
        {
        if (mask[i])
            {
            ((ULONG*)amesa->BackArray)[FIXxy(x[i],y[i])]=TC_RGBA(r[i],g[i],b[i],a[i]);
            }
        }
}


static void cyb_write_index_pixels(GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                          const GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;
DEBUGOUT("awrite_index_pixels<\n")

    for (i=0; i<n; i++)
        {
        if (mask[i])
            {
            WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),amesa->penconv[index[i]]);
            }
        }
}


static void cyb_write_monoindex_pixels(GLcontext *ctx, GLuint n,
                                                const GLint x[], const GLint y[],
                                                const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;

DEBUGOUT("cyb_write_monoindex_pixels<\n");

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using current index  */
        WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),amesa->pixel);

        }
    }
}



static void cyb_write_color_pixels( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                          const GLubyte r[], const GLubyte g[],
                                          const GLubyte b[], const GLubyte a[],
                                          const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;

DEBUGOUT("cyb_write_color_pixels<\n")

    for (i=0; i<n; i++) {
        if (mask[i]) {
/*        write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
        WriteRGBPixel(rp,FIXx(x[i]),FIXy(y[i]),TC_RGBA(r[i],g[i],b[i],a[i]));
        }
    }
}



static void cyb_write_monocolor_pixels(GLcontext *ctx, GLuint n,
                                                const GLint x[], const GLint y[],
                                                const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;

DEBUGOUT("cyb_write_monocolor_pixels<\n")

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
static void cyb_read_index_pixels( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                         GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i;
    register struct RastPort * rp = amesa->rp;

DEBUGOUT("cyb_read_index_pixels-\n")
    

  for (i=0; i<n; i++) {
      if (mask[i]) {
/*       index[i] = read_pixel x[i], y[i] */
        index[i] = ReadRGBPixel(rp,FIXx(x[i]),FIXy(y[i])); /* that's wrong! we get ARGB!*/
      }
  }
}

static void cyb_read_color_pixels( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                         GLubyte red[], GLubyte green[],
                                         GLubyte blue[], GLubyte alpha[],
                                         const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
    int i,col;
    register struct RastPort * rp = amesa->rp;

/*  ULONG ColTab[3];*/

DEBUGOUT("cyb_read_color_pixels-\n")
        

    for (i=0; i<n; i++)
        {
        if (mask[i])
            {
            col=ReadRGBPixel(rp,FIXx(x[i]),FIXy(y[i]));

            red[i]  = (col&0xff0000)>>16;
            green[i] = (col&0xff00)>>8;
            blue[i] = col&0xff;
            alpha[i]= (col&0xff000000)>>24;
        }
    }
}


/**********************************************************************/
/**********************************************************************/

static GLboolean cyb_set_buffer( GLcontext *ctx,GLenum mode )
{
    /* set the current drawing/reading buffer, return GL_TRUE or GL_FALSE */
    /* for success/failure */
    
/*   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx; */


DEBUGOUT("cyb_set_buffer TODO\n")
    

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




/**********************************************************************/
/**********************************************************************/
    /* Initialize all the pointers in the DD struct.  Do this whenever   */
    /* a new context is made current or we change buffers via set_buffer! */
void cyb_standard_DD_pointers( GLcontext *ctx )
{
/*   AmigaMesaContext amesa = ctx->DriverCtx;*/

DEBUGOUT("Inside cyb_standard_DD_pointers\n")
#ifdef DEBUGPRINT
printf("ctx=0x%x\n",ctx);
printf("CC=0x%x\n",CC);
#endif

/*  ctx=CC;  ** BRUTE FORCE OVERERIDE REMOVE PLEASE */


   ctx->Driver.UpdateState = cyb_standard_DD_pointers;
   ctx->Driver.ClearIndex = cyb_clear_index;
   ctx->Driver.ClearColor = cyb_clear_color;
   ctx->Driver.Clear = cyb_clear;

   ctx->Driver.Index = cyb_set_index;
   ctx->Driver.Color = cyb_set_color;

   ctx->Driver.IndexMask = cyb_index_mask;
   ctx->Driver.ColorMask = cyb_color_mask;

   ctx->Driver.SetBuffer = cyb_set_buffer;
   ctx->Driver.GetBufferSize = Cyb_Standard_resize;

   ctx->Driver.PointsFunc = cyb_choose_points_function( ctx );
   ctx->Driver.LineFunc = cyb_choose_line_function( ctx );
/*   ctx->Driver.PolygonFunc = cyb_choose_polygon_function( ctx );*/
    ctx->Driver.TriangleFunc = FALSE;  /*TODO*/

   /* Pixel/span writing functions: */
   ctx->Driver.WriteColorSpan       = cyb_write_color_span;
   ctx->Driver.WriteMonocolorSpan   = cyb_write_monocolor_span;
   ctx->Driver.WriteColorPixels     = cyb_write_color_pixels;
   ctx->Driver.WriteMonocolorPixels = cyb_write_monocolor_pixels;
   ctx->Driver.WriteIndexSpan       = cyb_write_index_span;
   ctx->Driver.WriteMonoindexSpan   = cyb_write_monoindex_span;
   ctx->Driver.WriteIndexPixels     = cyb_write_index_pixels;
   ctx->Driver.WriteMonoindexPixels = cyb_write_monoindex_pixels;

   /* Pixel/span reading functions: */
   ctx->Driver.ReadIndexSpan = cyb_read_index_span;
   ctx->Driver.ReadColorSpan = cyb_read_color_span;
   ctx->Driver.ReadIndexPixels = cyb_read_index_pixels;
   ctx->Driver.ReadColorPixels = cyb_read_color_pixels;
}

void cyb_Faster_DD_pointers( GLcontext *ctx )
{
    ctx=CC;  /* BRUTE FORCE OVERERIDE REMOVE PLEASE */

   ctx->Driver.UpdateState = cyb_Faster_DD_pointers;
   ctx->Driver.ClearIndex = cyb_clear_index;
   ctx->Driver.ClearColor = cyb_clear_color;
   ctx->Driver.Clear = cyb_clear_db;

   ctx->Driver.Index = cyb_set_index;
   ctx->Driver.Color = cyb_set_color;

   ctx->Driver.IndexMask = cyb_index_mask;
   ctx->Driver.ColorMask = cyb_color_mask;

   ctx->Driver.SetBuffer = cyb_set_buffer;
   ctx->Driver.GetBufferSize = Cyb_Standard_resize_db;

   ctx->Driver.PointsFunc = cyb_choose_points_function_db( ctx );
   ctx->Driver.LineFunc = cyb_choose_line_function_db( ctx );
/*   ctx->Driver.PolygonFunc = cyb_choose_polygon_function_db( ctx );*/
    ctx->Driver.TriangleFunc = FALSE;  /*TODO*/

   /* Pixel/span writing functions: */
   ctx->Driver.WriteColorSpan       = cyb_write_color_span_db;
   ctx->Driver.WriteMonocolorSpan   = cyb_write_monoindex_span_db; /* same */
   ctx->Driver.WriteColorPixels     = cyb_write_color_pixels_db;
   ctx->Driver.WriteMonocolorPixels = cyb_write_monoindex_pixels_db;   /* same */
   ctx->Driver.WriteIndexSpan       = cyb_write_index_span_db;
   ctx->Driver.WriteMonoindexSpan   = cyb_write_monoindex_span_db;
   ctx->Driver.WriteIndexPixels     = cyb_write_index_pixels_db;
   ctx->Driver.WriteMonoindexPixels = cyb_write_monoindex_pixels_db;

   /* Pixel/span reading functions: */
   ctx->Driver.ReadIndexSpan = cyb_read_index_span;
   ctx->Driver.ReadColorSpan = cyb_read_color_span;
   ctx->Driver.ReadIndexPixels = cyb_read_index_pixels;
   ctx->Driver.ReadColorPixels = cyb_read_color_pixels;
}


/**********************************************************************/
/*****                  Amiga/Mesa private misc procedures        *****/
/**********************************************************************/

static void MyWritePixelArray(void *a,int b,int c,int d,struct RastPort *e,int f,int g, int h, int i, int j)
{
    WritePixelArray(a,b,c,d,e,f,g,h,i,j);
}

/**********************************************************************/
/***** AmigaMesaSetOneColor                                       *****/
/**********************************************************************/
void AmigaMesaSetOneColor(struct amigamesa_context *c,int index, float r, float g, float b)
{
    if (c->depth>8) {
        c->penconv[index] = TC_RGBA((int)r*255,(int)g*255,(int)b*255,255);
    } else {
        c->penconv[index] = RGBA(c,r*255,g*255,b*255,255);
    }
}



static void
Cyb_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height)
    {
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

DEBUGOUT("Cyb_Standard_resize\n")
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

        AllocOneLine(amesa);
        }
    }

static void
Cyb_Standard_resize_db( GLcontext *ctx,GLuint *width, GLuint *height)
    {
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

DEBUGOUT("Cyb_Standard_resize_db\n")

    *width=amesa->width;
    *height=amesa->height;

    if(!((  amesa->width  == (amesa->rp->Layer->bounds.MaxX-amesa->rp->Layer->bounds.MinX-amesa->left) )
    &&  ( amesa->height == (amesa->rp->Layer->bounds.MaxY-amesa->rp->Layer->bounds.MinY-amesa->bottom)  )))
        {

        amesa->RealWidth =amesa->rp->Layer->bounds.MaxX-amesa->rp->Layer->bounds.MinX;
        amesa->RealHeight=amesa->rp->Layer->bounds.MaxY-amesa->rp->Layer->bounds.MinY;
        amesa->FixedWidth =(amesa->RealWidth+3)&0xfffffffc;
        amesa->FixedHeight=amesa->RealHeight+1;


        *width=amesa->width = amesa->RealWidth-amesa->left;
        *height=amesa->height = amesa->RealHeight-amesa->bottom;
/*      amesa->left = 0;*/
/*      amesa->bottom = 0;*/
        amesa->depth = GetCyberMapAttr(amesa->rp->BitMap,CYBRMATTR_DEPTH);

        if (amesa->visual->db_flag && amesa->BackArray)
            {
            destroy_penbackarray(amesa->BackArray);
            if(!(amesa->BackArray=alloc_penbackarray(amesa->RealWidth,amesa->RealHeight,4)))
                {
                printf("AmigaMesa Error Can't allocate new PenArray in that size.\n");
/*              Amiga_Standard_init(amesa);*/

                }
            }

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
    }



/**********************************************************************/
/*****                  Amiga/Mesa private init/despose/resize       *****/
/**********************************************************************/

static void
Cyb_Standard_SwapBuffer_db(struct amigamesa_context *amesa)
{
DEBUGOUT("Amiga_Standard_SwapBuffer_db\n")

    if (amesa->depth>8) {
        MyWritePixelArray(amesa->BackArray,amesa->gl_ctx->Viewport.X,amesa->RealHeight-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
                        amesa->RealWidth*4,/*(amesa->left+amesa->gl_ctx->Viewport.X+amesa->gl_ctx->Viewport.Width)*4,*/
                        amesa->rp,amesa->left+amesa->gl_ctx->Viewport.X,
                        amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
                        /*amesa->left+amesa->gl_ctx->Viewport.X+*/amesa->gl_ctx->Viewport.Width,
                        amesa->gl_ctx->Viewport.Height/*amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Y*/,
                        RECTFMT_ARGB);
/*      printf("WritePixelArray(%d,%d,%d,%d);\n",
                                              amesa->left+amesa->gl_ctx->Viewport.X,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
                                     amesa->left+amesa->gl_ctx->Viewport.X+amesa->gl_ctx->Viewport.Width,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Y);*/
    } else {
/*      WriteChunkyPixels(amesa->rp,amesa->left,amesa->RealHeight-amesa->height,amesa->RealWidth,amesa->RealHeight-amesa->bottom,amesa->BackArray,amesa->RealWidth);*/

        WriteChunkyPixels/*WritePixelArray8*/(amesa->rp,amesa->left+amesa->gl_ctx->Viewport.X,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
                                     amesa->left+amesa->gl_ctx->Viewport.X+amesa->gl_ctx->Viewport.Width-1,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Y-1,
                                     amesa->BackArray+(amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y)*amesa->RealWidth+amesa->left+amesa->gl_ctx->Viewport.X,
                                     amesa->RealWidth/*amesa->temprp*/);
/*        WritePixelArray8(amesa->rp,amesa->left+amesa->gl_ctx->Viewport.X,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
                                     amesa->left+amesa->gl_ctx->Viewport.X+amesa->gl_ctx->Viewport.Width,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Y,
                                     amesa->BackArray,
                                     amesa->temprp);*/
/*      printf("WritePixelArray8(%d,%d,%d,%d);\n",
                                              amesa->left+amesa->gl_ctx->Viewport.X,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
                                     amesa->left+amesa->gl_ctx->Viewport.X+amesa->gl_ctx->Viewport.Width,
                                     amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Y);
*/
    }

}


BOOL
Cyb_Standard_init(struct amigamesa_context *c,struct TagItem *tagList)
    {
DEBUGOUT("Cyber_Standard_init\n")
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
DEBUGOUT("Dubelbuff inside amigastandard")
        if((c->back_rp = make_rastport(c->RealWidth,c->RealHeight,c->depth, c->rp->BitMap))!=NULL)
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
    c->InitDD=c->depth<=8 ? amiga_standard_DD_pointers : cyb_standard_DD_pointers;  /*standard drawing*/
    c->Dispose=Amiga_Standard_Dispose;
    c->SwapBuffer=Amiga_Standard_SwapBuffer;


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


BOOL
Cyb_Standard_init_db(struct amigamesa_context *c,struct TagItem *tagList)
    {
DEBUGOUT("Amiga_Standard_init_db\n")
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

    if (CyberGfxBase!=NULL && IsCyberModeID(GetVPModeID(&c->Screen->ViewPort)))
        {
        c->depth = GetCyberMapAttr(c->rp->BitMap,CYBRMATTR_DEPTH);
        } 
    else
        {
        c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);
        }
    c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX;
    c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY;
    if (c->depth<=8)
        {
        c->FixedWidth =((c->RealWidth+15)>>4)<<4;
        }
    else
        {
        c->FixedWidth =(c->RealWidth+3)&0xfffffffc;
        }
    c->FixedHeight=c->RealHeight+1;


    c->left = GetTagData(AMA_Left,0,tagList);
    c->bottom = GetTagData(AMA_Bottom,0,tagList);

    c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
    c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);

/*
    c->gl_ctx->BufferWidth = c->width;
    c->gl_ctx->BufferHeight = c->height;
*/
/*  c->pixel = 0;   ** current drawing pen */

    if (c->depth<=8) {
        AllocCMap(c->Screen);    /* colormap */
        /* JAM: added alloc_temp_rastport */
        alloc_temp_rastport(c);
    }

    if (c->visual->db_flag)
        {
        c->BackArray=alloc_penbackarray(c->RealWidth,c->RealHeight,c->depth<=8 ? 1 : 4);
#ifdef DEBUGPRINT
printf("byte array at -------->0x%x ends at 0x%x size=0x%x (%d)\n",c->BackArray,c->BackArray+(((c->RealWidth+15)>>4)<<4)*(c->RealHeight+1),(((c->RealWidth+15)>>4)<<4)*(c->RealHeight+1),(((c->RealWidth+15)>>4)<<4)*(c->RealHeight+1));
#endif
        c->gl_ctx->Color.DrawBuffer = GL_BACK;

        c->InitDD=c->depth<=8 ? amiga_Faster_DD_pointers : cyb_Faster_DD_pointers;  /*fast drawing*/
        c->Dispose=Amiga_Standard_Dispose_db;
        c->SwapBuffer=Cyb_Standard_SwapBuffer_db;
        }
    else
        {
        c->gl_ctx->Color.DrawBuffer = GL_FRONT;
        return(FALSE);
        }
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




#undef DEBUGPRINT
#endif  /* the outcomment of cyberGfx */

