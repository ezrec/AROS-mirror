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

#include <GL/AmigaMesa.h>
#ifdef	ADISP_AGA

#include "ADisp_AGA.h"

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"

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

#ifdef AGA
#define c8to32(x) ((x)<<24)
#else
#define c8to32(x) (((((((x)<<8)|(x))<<8)|(x))<<8)|(x))
#endif

#define MAX_POLYGON 300
/* #define AGA 1 */
/* TrueColor-RGBA */
#define TC_RGBA(r,g,b,a) ((((((a<<8)|r)<<8)|g)<<8)|b)

/* #define DEBUGPRINT */

#ifdef DEBUGPRINT
#define DEBUGOUT(x) printf(x);
#else
#define DEBUGOUT(x) /*printf(x);*/
#endif

static void Amiga_Standard_resize_db( GLcontext *ctx,GLuint *width, GLuint *height);
static void Amiga_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height);

struct RastPort *make_rastport( int width, int height, int depth, struct BitMap *friendbm );
void destroy_rastport( struct RastPort *rp );
BOOL make_temp_raster( struct RastPort *rp );
void destroy_temp_raster( struct RastPort *rp );
void AllocOneLine(struct amigamesa_context *AmigaMesaCreateContext);
void FreeOneLine(struct amigamesa_context *AmigaMesaCreateContext);

BOOL alloc_temp_rastport(struct amigamesa_context * c);
void free_temp_rastport(struct amigamesa_context * c);


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

static void afinish( void )
{
	/* implements glFinish if possible */
}


static void aflush( void )
{
	/* implements glFlush if possible */
}


static void aclear_index( GLcontext *ctx, GLuint index )
{
   /* implement glClearIndex */
   /* usually just save the value in the context struct */
   /*printf("aclear_index=glClearIndex=%d\n",index);*/
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
   amesa->clearpixel = amesa->penconv[index];
}


static void aclear_color( GLcontext *ctx,
						  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   /* implement glClearColor */
   /* color components are floats in [0,1] */
   /* usually just save the value in the context struct */
   /*printf("aclear_color=glClearColor(%d,%d,%d,%d)\n",r,g,b,a);*/
   /* @@@ TODO FREE COLOR IF NOT USED */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
   amesa->clearpixel=RGBA(amesa,r,g,b,a);
/*  amesa->clearpixel=RGBA(r,g,b,a);  Orginal */  

}




static void aclear( GLcontext *ctx,GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
/*
 * Clear the specified region of the color buffer using the clear color
 * or index as specified by one of the two functions above.
 * If all==GL_TRUE, clear whole buffer
 */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

#ifdef DEBUGPRINT
printf("aclear(%d,%d,%d,%d,%d)\n",all,x,y,width,height);
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
		else
			printf("Serius error amesa->rp=0 detected in aclear() in file amigamesa.c\n");
		}
	
}

static void aclear_db(GLcontext *ctx, GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
/*
 * Clear the specified region of the color buffer using the clear color
 * or index as specified by one of the two functions above.
 * If all==GL_TRUE, clear whole buffer
 */
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
/*  int i; */
	register UBYTE *db;
	register UBYTE col=amesa->clearpixel;


#ifdef DEBUGPRINT
printf("aclear_db(%d,%d,%d,%d,%d)\n",all,x,y,width,height);
#endif



/*  SetAPen(amesa->rp,amesa->clearpixel);*/
	if(all)
		{
		int x1,y1,x2,y2;
/*      RectFill(amesa->rp,FIXx(CC.Viewport.X),FIXy(CC.Viewport.Y)-CC.Viewport.Height+1,FIXx(CC.Viewport.X)+CC.Viewport.Width-1,FIXy(CC.Viewport.Y));*/

		x1=0;
		y1=0;
		x2=amesa->RealWidth;
		y2=amesa->RealHeight;
		db=amesa->BackArray;
		for(y1=0;y1<y2;y1++)
			{
			for(x1=0;x1<x2;x1++)
				{
				*db=col;
				db++;
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
				amesa->BackArray[x1+y1*amesa->RealWidth]=col;
				}
			}
		}
}



static void aset_index( GLcontext *ctx,GLuint index )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

	/* Set the amesa color index. */
/*printf("aset_index(%d)\n",index);*/
	amesa->pixel = amesa->penconv[index];
}



static void aset_color( GLcontext *ctx,GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

/*printf("aset_color(%d,%d,%d,%d)\n",r,g,b,a);*/

		/* Set the current RGBA color. */
		/* r is in 0..255.RedScale */
		/* g is in 0..255.GreenScale */
		/* b is in 0..255.BlueScale */
		/* a is in 0..255.AlphaScale */
	amesa->pixel = RGBA(amesa,r,g,b,a);
	/*(a << 24) | (r << 16) | (g << 8) | b;*/
}


static GLboolean aindex_mask( GLcontext *ctx,GLuint mask )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

	/* implement glIndexMask if possible, else return GL_FALSE */
/*printf("aindex_mask(0x%x)\n",mask);*/
	amesa->rp->Mask = (UBYTE) mask;

	return(GL_TRUE);
}

static GLboolean acolor_mask( GLcontext *ctx,GLboolean rmask, GLboolean gmask,
									  GLboolean bmask, GLboolean amask)
{
	/* implement glColorMask if possible, else return GL_FALSE */
	return(GL_FALSE);

}



/**********************************************************************/
/*****            Accelerated point, line, polygon rendering        *****/
/**********************************************************************/

/*
 *  Render a number of points by some hardware/OS accerated method
 */

static void afast_points_function(GLcontext *ctx,GLuint first, GLuint last )
{


	AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	struct vertex_buffer *VB = ctx->VB;

	int i,col;
	register struct RastPort * rp=amesa->rp;
/* printf("afast_points_function\n");*/

		if (VB->MonoColor) {
		/* draw all points using the current color (set_color) */
		SetAPen(rp,amesa->pixel);
/*      printf("VB->MonoColor\n");*/
		for (i=first;i<=last;i++) {
			if (VB->Unclipped[i]) {
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
			/*if (VB.Unclipped[i])*/ {
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



static points_func achoose_points_function( GLcontext *ctx )
{
/*printf("achoose_points_function\n");*/
	/* Examine the current rendering state and return a pointer to a */
	/* fast point-rendering function if possible. */
   if (ctx->Point.Size==1.0 && !ctx->Point.SmoothFlag && ctx->RasterMask==0
	   && !ctx->Texture.Enabled /*&&  ETC, ETC */) {
	  return afast_points_function;
   }
   else {
	  return NULL;
   }
}

static points_func achoose_points_function_db( GLcontext *ctx )
	{
	return NULL;
	}


 /*
  *  Render a line by some hardware/OS accerated method 
  */

static void afast_line_function( GLcontext *ctx,GLuint v0, GLuint v1, GLuint pv )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
	int x0, y0, x1, y1;
/* printf("afast_line_function\n");*/

	if (VB->MonoColor)
		{
		SetAPen(amesa->rp,amesa->pixel);
		}
	else
		{
		SetAPen(amesa->rp,amesa->penconv[*VB->Color[pv]]);
		}
	x0 = FIXx((int) (VB->Win[v0][0]));
	y0 = FIXy((int) (VB->Win[v0][1]));
	x1 = FIXx((int) (VB->Win[v1][0]));
	y1 = FIXy((int) (VB->Win[v1][1]));

	Move(amesa->rp,x0,y0);
	Draw(amesa->rp,x1,y1);
}



static line_func achoose_line_function( GLcontext *ctx)
{
	/*printf("achoose_line_function\n");*/

	/* Examine the current rendering state and return a pointer to a */
	/* fast line-rendering function if possible. */

   if (ctx->Line.Width==1.0 && !ctx->Line.SmoothFlag && !ctx->Line.StippleFlag
	   && ctx->Light.ShadeModel==GL_FLAT && ctx->RasterMask==0
	   && !ctx->Texture.Enabled /*&&  ETC, ETC */ )
		{
		return afast_line_function;
		}
	else 
		{

		return NULL;
		}
}

static line_func achoose_line_function_db( GLcontext *ctx)
	{
	return NULL;
	}

/*
 * Draw a filled polygon of a single color. If there is hardware/OS support
 * for polygon drawing use that here.   Otherwise, call a function in
 * polygon.c to do the drawing.
 */

static void afast_polygon_function( GLcontext *ctx,GLuint n, GLuint vlist[], GLuint pv )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	struct vertex_buffer *VB = ctx->VB;

	int i,j;
	struct RastPort * rp=amesa->rp;

	/* Render a line by some hardware/OS accerated method */
	/* printf("afast_polygon_function\n");*/

	if (VB->MonoColor)
		SetAPen(rp,amesa->pixel);
	else
		SetAPen(rp,amesa->penconv[*VB->Color[pv]]);


	AreaMove(rp, FIXx((int) VB->Win[0][0]), FIXy( (int) VB->Win[0][1]));


	for (i=1;i<n;i++) {
		j=vlist[i];
		AreaDraw(rp, FIXx((int) VB->Win[j][0]), FIXy( (int) VB->Win[j][1]));

	}
	AreaEnd(rp );
}





static polygon_func achoose_polygon_function( GLcontext *ctx )
{
	/* printf("achoose_polygon_function\n");*/

	/* Examine the current rendering state and return a pointer to a */
	/* fast polygon-rendering function if possible. */

   if (!ctx->Polygon.SmoothFlag && !ctx->Polygon.StippleFlag
	   && ctx->Light.ShadeModel==GL_FLAT && ctx->RasterMask==0
	   && !ctx->Texture.Enabled /*&&  ETC, ETC */ )
	  {
		return afast_polygon_function;
		}
	else 
		{
		return NULL;
		}
}

/**********************************************************************/
/*****                  Optimized polygon rendering               *****/
/**********************************************************************/

/* Outcomment all*/

/*
 * Useful macros:
 */

/*
#define PACK_RGBA(R,G,B,A)  (((R) << 24) | ((G) << 16) | ((B) << 8) | (A))
#define PACK_ABGR(R,G,B,A)  (((A) << 24) | ((B) << 16) | ((G) << 8) | (R))
*/

/* #define PACK_RGBA(R,G,B,A) RGBA(amesa,R,G,B,A)*/
/* #define PACK_ABGR(R,G,B,A) RGBA(R,G,B,A) */

/* Help to fix this orginal in the top
__inline GLint FIXx(GLint x)    return(amesa->left + x);
__inline GLint FIXy(GLint y)    return(amesa->RealHeight-amesa->bottom - y);
__inline GLint FIXxy(GLint x,GLint y) return((amesa->FixedWidth*FIXy(y)+FIXx(x)));
		db=(ULONG)(amesa->BackArray)+(ULONG)FIXxy(x1,y1);

*/

/*                                       
#define PIXELADDR1(X,Y)  ((GLubyte *)  amesa->buffer +  (Y)*amesa->width + (X))
#define PIXELADDR4(X,Y)   ((GLuint *)  amesa->buffer +  (Y)*amesa->width + (X))
#define PIXELADDR4B(X,Y)  ((GLubyte *) amesa->buffer + ((Y)*amesa->width + (X))*4)
*/

/*#define PIXELADDR1(X,Y)  ((GLubyte *)  (amesa->BackArray) + FIXxy(X,Y))
#define PIXELADDR4(X,Y)   ((GLuint *)  (amesa->BackArray) + FIXxy(X,Y))*/
/* #define PIXELADDR4B(X,Y)  ((GLubyte *) (amesa->BackArray) + FIXxy(X,Y)*4) */



/*static GLint lx[MAX_HEIGHT], rx[MAX_HEIGHT];*/    /* X bounds */
/*static GLfixed lz[MAX_HEIGHT], rz[MAX_HEIGHT];*/  /* Z values */
/*static GLfixed lr[MAX_HEIGHT], rr[MAX_HEIGHT];*/  /* Red */
/*static GLfixed lg[MAX_HEIGHT], rg[MAX_HEIGHT];*/  /* Green */
/*static GLfixed lb[MAX_HEIGHT], rb[MAX_HEIGHT];*/  /* Blue */
/*static GLfixed la[MAX_HEIGHT], ra[MAX_HEIGHT];*/  /* Alpha */



/*
 * Smooth-shaded, z-less polygon, RGBA byte order.
 */
/*static void smooth_RGBA_z_polygon( GLuint n, GLuint vlist[], GLuint pv )
{
#define INTERP_COLOR
#define INTERP_ALPHA
#define INTERP_Z

#define INNER_CODE                      \
   GLint i;                         \
   GLubyte *img = PIXELADDR4(xmin,y);                   \
   for (i=0;i<len;i++) {                    \
	  if (FixedToUns(fz) < *zptr) {             \
		 *img = PACK_RGBA( FixedToInt(fr), FixedToInt(fg),  \
				   FixedToInt(fb), FixedToInt(fa) );    \
		 *zptr = FixedToUns(fz);                \
	  }                             \
	  fr += fdrdx;  fg += fdgdx;  fb += fdbdx;  fa += fdadx;    \
	  fz += fdzdx;  zptr++;  img++;             \
   }

#include "polytemp.h"
}



*/
/*
 * Smooth-shaded, z-less polygon.
 */
/*static void flat_RGBA_z_polygon( GLuint n, GLuint vlist[], GLuint pv )
{
#define INTERP_COLOR
#define INTERP_ALPHA
#define INTERP_Z

#define SETUP_CODE          \
   GLubyte r = VB.Color[pv][0];     \
   GLubyte g = VB.Color[pv][1];     \
   GLubyte b = VB.Color[pv][2];     \
   GLubyte a = VB.Color[pv][3];     \
   GLubyte pixel = PACK_RGBA(r,g,b,a );

#define INNER_CODE          \
   GLint i;             \
   GLubyte *img = PIXELADDR4(xmin,y);       \
   for (i=0;i<len;i++) {        \
	  if (FixedToUns(fz) < *zptr) { \
	 *img = pixel;          \
		 *zptr = FixedToUns(fz);    \
	  }                 \
	  fz += fdzdx;  zptr++;  img++; \
   }

#include "polytemp.h"
}



*/
/*
 * Return pointer to an accelerated polygon function if possible.
 */
/*static polygon_func choose_polygon_function_os( GLcontext *ctx)
{
   GLuint i4 = 1;
   GLubyte *i1 = (GLubyte *) &i4;
   GLint little_endian = *i1;

   if (CC.Polygon.SmoothFlag)     return NULL;
   if (CC.Polygon.StippleFlag)    return NULL;
   if (CC.Texture.Enabled)        return NULL;

   if (CC.RasterMask==DEPTH_BIT
	   && CC.Depth.Func==GL_LESS
	   && CC.Depth.Mask==GL_TRUE
*/
/*     && amesa->format==GL_RGBA*/
/*) {
	  if (CC.Light.ShadeModel==GL_SMOOTH) {
		 if (little_endian) {
			return smooth_ABGR_z_polygon;
		 }
		 else {
			return smooth_RGBA_z_polygon;
		 }
	  }
	  else {
		 return flat_RGBA_z_polygon;
	  }
   }
   return NULL;
}

END outcomment all*/ 


static polygon_func achoose_polygon_function_db( GLcontext *ctx )
{
	return NULL;
}


/**********************************************************************/
/*****              Write spans of pixels                                   *****/
/**********************************************************************/

static void awrite_index_span_db(GLcontext *ctx, GLuint n, GLint x, GLint y,
											const GLuint index[],
											const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

	int i/* ,ant*/;
	register UBYTE *db;
	register unsigned long * penconv=amesa->penconv;


#ifdef DEBUGPRINT
printf("awrite_index_span_db(%d,%d,%d)\n",n,x,y);
#endif

db=amesa->BackArray+FIXxy(x,y);

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
			*db=penconv[index[i]];
			db++;
			}
	}

static void awrite_color_span_db(GLcontext *ctx, GLuint n, GLint x, GLint y,
										const GLubyte red[], const GLubyte green[],
										const GLubyte blue[], const GLubyte alpha[],
										const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i/*,ant*/;
	register UBYTE *db;

#ifdef DEBUGPRINT
printf("awrite_color_span_db(%d,%d,%d)\n",n,x,y);
#endif

db=amesa->BackArray+FIXxy(x,y);
/* Slower */
	if (mask)
		{
		for (i=0;i<n;i++)
			{
			if (mask[i])
				{
				/* draw pixel (x[i],y[i]) using index[i] */
				*db=RGBA(amesa,red[i],green[i],blue[i],alpha[i]);
				}
			db++;
			}
		}
	else
		{
		for (i=0;i<n;i++)
			{
			/* draw pixel (x[i],y[i]) using index[i] */
			*db=RGBA(amesa,red[i],green[i],blue[i],alpha[i]);
			db++;
			}
		}
	
}
static void awrite_monoindex_span_db( GLcontext *ctx,GLuint n, GLint x, GLint y,
											 const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i/*,ant*/;
	register UBYTE *db;

#ifdef DEBUGPRINT
printf("awrite_monoindex_span_db(%d,%d,%d)\n",n,x,y);
#endif

db=amesa->BackArray+FIXxy(x,y);

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
		*db=amesa->pixel;
		db++;
		}
}


static void awrite_index_span(GLcontext *ctx, GLuint n, GLint x, GLint y,
										const GLuint index[],
										const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,ant;
	UBYTE *dp;
	unsigned long * penconv=amesa->penconv;
	register struct RastPort * rp=amesa->rp;

/*printf("awrite_index_span(%d,%d,%d)\n",n,x,y);*/
DEBUGOUT("awrite_index_span(n,x,y)\n")

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
					/* JAM: Replaced by correct call */
					/* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
DEBUGOUT("awrite_index_span: WritePixelLine8()\n")
					WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
				dp=amesa->imageline;
				ant=0;
				x++;
			}
		}
		if(ant)
			/* JAM: Replaced by correct call */
			/* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
DEBUGOUT("awrite_index_span: WritePixelLine8()\n")
			WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);

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


static void awrite_color_span( GLcontext *ctx,GLuint n, GLint x, GLint y,
										const GLubyte red[], const GLubyte green[],
										const GLubyte blue[], const GLubyte alpha[],
										const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,ant;
	UBYTE *dp;

	register struct RastPort * rp = amesa->rp;

/*DEBUGOUT("awrite_color_span(n,x,y) ")*/
	
	y=FIXy(y);
	x=FIXx(x);
/*printf("awrite_color_span(ant=%d,x=%d,y=%d)",n,x,y);*/

	if((dp = amesa->imageline) && amesa->temprp)
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
					*dp = RGBA(amesa,red[i],green[i],blue[i],alpha[i]);
					dp++;
					}
				else
					{
					if(ant)
						{
						WritePixelLine8(rp,x,y,ant,amesa->imageline,amesa->temprp);
/*printf("WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y);*/
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
DEBUGOUT("nomask\n")
			for (i=0;i<n;i++)                   /* draw pixel (x[i],y[i])*/
				{
				*dp = RGBA(amesa,red[i],green[i],blue[i],alpha[i]);
				dp++;
				}
			WritePixelLine8(rp,x,y,n,amesa->imageline,amesa->temprp);
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
					SetAPen(rp,RGBA(amesa,red[i],green[i],blue[i],alpha[i]));
					WritePixel(rp,x,y);
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
				SetAPen(rp,RGBA(amesa,red[i],green[i],blue[i],alpha[i]));
				WritePixel(rp,x,y);
				}
			}
		}
}




static void awrite_monoindex_span(GLcontext *ctx,GLuint n,GLint x,GLint y,const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	y=FIXy(y);
	x=FIXx(x);
	SetAPen(amesa->rp,amesa->pixel);


DEBUGOUT("awrite_monoindex_span\n")


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

static void awrite_monocolor_span( GLcontext *ctx,GLuint n, GLint x, GLint y,
											 const GLubyte mask[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;

/*printf("awrite_monocolor_span(%d,%d,%d)\n",n,x,y);*/
DEBUGOUT("awrite_monocolor_span(n,x,y)\n")

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
/*****                    Read spans of pixels                              *****/
/**********************************************************************/

/* Here we should check if the size of the colortable is <256 */

static void aread_index_span( GLcontext *ctx,GLuint n, GLint x, GLint y, GLuint index[])
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

	int i;

DEBUGOUT("aread_index_span>>\n")


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

static void aread_color_span( GLcontext *ctx,GLuint n, GLint x, GLint y,
									  GLubyte red[], GLubyte green[],
									  GLubyte blue[], GLubyte alpha[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,col;
	ULONG ColTab[3];
	struct ColorMap * cm=amesa->Screen->ViewPort.ColorMap;

DEBUGOUT("aread_color_span>>\n")


	y=FIXy(y);
	x=FIXx(x);

	if(amesa->imageline && amesa->temprp) {
		ReadPixelLine8(amesa->rp,x,y,n,amesa->imageline,amesa->temprp);
		for(i=0; i<n; i++) {
			GetRGB32(cm,amesa->imageline[i],1,ColTab);
			red[i]  = ColTab[0]>>24;
			green[i] = ColTab[1]>>24;
			blue[i] = ColTab[2]>>24;
			alpha[i]=255;
		}
	} else
		for (i=0; i<n; i++, x++) {
			col=ReadPixel(amesa->rp,x,y);
			GetRGB32(cm,col,1,ColTab);

			red[i]  = ColTab[0]>>24;
			green[i] = ColTab[1]>>24;
			blue[i] = ColTab[2]>>24;
			alpha[i]=255;
			}
}




/**********************************************************************/
/*****                Write arrays of pixels                                *****/
/**********************************************************************/

static void awrite_index_pixels_db( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
										  const GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

	int i;
/*  register struct RastPort * rp = amesa->rp; */
DEBUGOUT("awrite_index_pixels<\n")

	for (i=0; i<n; i++)
		{
		if (mask[i])
			{
			amesa->BackArray[FIXxy(x[i],y[i])]=amesa->penconv[index[i]];
			}
		}
}

static void awrite_monoindex_pixels_db( GLcontext *ctx,GLuint n,
												const GLint x[], const GLint y[],
												const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
/*  register struct RastPort * rp = amesa->rp; */
DEBUGOUT("awrite_index_pixels<\n")

	for (i=0; i<n; i++)
		{
		if (mask[i])
			{
			amesa->BackArray[FIXxy(x[i],y[i])]=amesa->pixel;
			}
		}
}

static void awrite_color_pixels_db(GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
											  const GLubyte r[], const GLubyte g[],
											  const GLubyte b[], const GLubyte a[],
											  const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
/*  register struct RastPort * rp = amesa->rp; */
DEBUGOUT("awrite_index_pixels<\n")

	for (i=0; i<n; i++)
		{
		if (mask[i])
			{
			amesa->BackArray[FIXxy(x[i],y[i])]=RGBA(amesa,r[i],g[i],b[i],a[i]);
			}
		}
}


static void awrite_index_pixels(GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
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
			SetAPen(rp,amesa->penconv[index[i]]);
			WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
			}
		}
}


static void awrite_monoindex_pixels(GLcontext *ctx, GLuint n,
												const GLint x[], const GLint y[],
												const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;

DEBUGOUT("awrite_monoindex_pixels<\n");

		SetAPen(rp,amesa->pixel);

	for (i=0; i<n; i++) {
		if (mask[i]) {
/*        write pixel x[i], y[i] using current index  */
		WritePixel(rp,FIXx(x[i]),FIXy(y[i]));

		}
	}
}



static void awrite_color_pixels( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
										  const GLubyte r[], const GLubyte g[],
										  const GLubyte b[], const GLubyte a[],
										  const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;

DEBUGOUT("awrite_color_pixels<\n")

	for (i=0; i<n; i++) {
		if (mask[i]) {
/*        write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
		SetAPen(rp,RGBA(amesa,r[i],g[i],b[i],a[i]));
		WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
		}
	}
}



static void awrite_monocolor_pixels(GLcontext *ctx, GLuint n,
												const GLint x[], const GLint y[],
												const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;

DEBUGOUT("awrite_monocolor_pixels<\n")

		SetAPen(rp,amesa->pixel);

	for (i=0; i<n; i++) {
		if (mask[i]) {
/*        write pixel x[i], y[i] using current color*/
		WritePixel(rp,FIXx(x[i]),FIXy(y[i]));
		}
	}
}




/**********************************************************************/
/*****                       Read arrays of pixels                          *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void aread_index_pixels( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
										 GLuint index[], const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i;
	register struct RastPort * rp = amesa->rp;

DEBUGOUT("aread_index_pixels-\n")
	

  for (i=0; i<n; i++) {
	  if (mask[i]) {
/*       index[i] = read_pixel x[i], y[i] */
		index[i] = ReadPixel(rp,FIXx(x[i]),FIXy(y[i]));
	  }
  }
}

static void aread_color_pixels( GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
										 GLubyte red[], GLubyte green[],
										 GLubyte blue[], GLubyte alpha[],
										 const GLubyte mask[] )
{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;
	int i,col;
	register struct RastPort * rp = amesa->rp;

	ULONG ColTab[3];

DEBUGOUT("aread_color_pixels-\n")
		

	for (i=0; i<n; i++)
		{
		if (mask[i])
			{
			col=ReadPixel(rp,FIXx(x[i]),FIXy(y[i]));

			GetRGB32(amesa->Screen->ViewPort.ColorMap,col,1,ColTab);

			red[i]  = ColTab[0]>>24;
			green[i] = ColTab[1]>>24;
			blue[i] = ColTab[2]>>24;
			alpha[i]=255;
		}
	}
}



static GLboolean aset_buffer( GLcontext *ctx,GLenum mode )
{
	/* set the current drawing/reading buffer, return GL_TRUE or GL_FALSE */
	/* for success/failure */
	
/*   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx; */


/* amiga_setup_DD_pointers(); in ddsample is this right?????*/
DEBUGOUT("aset_buffer TODO\n")
	

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
void amiga_standard_DD_pointers( GLcontext *ctx )
{
/*   AmigaMesaContext amesa = ctx->DriverCtx; */

DEBUGOUT("Inside amiga_standard_DD_pointers\n")
#ifdef DEBUGPRINT
printf("ctx=0x%x\n",ctx);
printf("CC=0x%x\n",CC);
#endif

/*  ctx=CC;  ** BRUTE FORCE OVERERIDE REMOVE PLEASE */


   ctx->Driver.UpdateState = amiga_standard_DD_pointers;
   ctx->Driver.ClearIndex = aclear_index;
   ctx->Driver.ClearColor = aclear_color;
   ctx->Driver.Clear = aclear;

   ctx->Driver.Index = aset_index;
   ctx->Driver.Color = aset_color;

   ctx->Driver.IndexMask = aindex_mask;
   ctx->Driver.ColorMask = acolor_mask;

   ctx->Driver.SetBuffer = aset_buffer;
   ctx->Driver.GetBufferSize = Amiga_Standard_resize;

   ctx->Driver.PointsFunc = achoose_points_function( ctx );
   ctx->Driver.LineFunc = achoose_line_function( ctx );
/*   ctx->Driver.PolygonFunc = achoose_polygon_function( ctx ); */
	ctx->Driver.TriangleFunc = FALSE;  /*TODO */

   /* Pixel/span writing functions: */
   ctx->Driver.WriteColorSpan       = awrite_color_span;
   ctx->Driver.WriteMonocolorSpan   = awrite_monocolor_span;
   ctx->Driver.WriteColorPixels     = awrite_color_pixels;
   ctx->Driver.WriteMonocolorPixels = awrite_monocolor_pixels;
   ctx->Driver.WriteIndexSpan       = awrite_index_span;
   ctx->Driver.WriteMonoindexSpan   = awrite_monoindex_span;
   ctx->Driver.WriteIndexPixels     = awrite_index_pixels;
   ctx->Driver.WriteMonoindexPixels = awrite_monoindex_pixels;

   /* Pixel/span reading functions: */
   ctx->Driver.ReadIndexSpan = aread_index_span;
   ctx->Driver.ReadColorSpan = aread_color_span;
   ctx->Driver.ReadIndexPixels = aread_index_pixels;
   ctx->Driver.ReadColorPixels = aread_color_pixels;
}

void amiga_Faster_DD_pointers( GLcontext *ctx )
{
/*   AmigaMesaContext amesa = ctx->DriverCtx; */

	ctx=CC;  /* BRUTE FORCE OVERERIDE REMOVE PLEASE */

   ctx->Driver.UpdateState = amiga_Faster_DD_pointers;
   ctx->Driver.ClearIndex = aclear_index;
   ctx->Driver.ClearColor = aclear_color;
   ctx->Driver.Clear = aclear_db;

   ctx->Driver.Index = aset_index;
   ctx->Driver.Color = aset_color;

   ctx->Driver.IndexMask = aindex_mask;
   ctx->Driver.ColorMask = acolor_mask;

   ctx->Driver.SetBuffer = aset_buffer;
   ctx->Driver.GetBufferSize = Amiga_Standard_resize_db;

   ctx->Driver.PointsFunc = achoose_points_function_db( ctx );
   ctx->Driver.LineFunc = achoose_line_function_db( ctx );
/*   ctx->Driver.PolygonFunc = achoose_polygon_function_db( ctx ); */
	ctx->Driver.TriangleFunc = FALSE;  /* TODO */

   /* Pixel/span writing functions: */
   ctx->Driver.WriteColorSpan       = awrite_color_span_db;
   ctx->Driver.WriteMonocolorSpan   = awrite_monoindex_span_db; /* same */
   ctx->Driver.WriteColorPixels     = awrite_color_pixels_db;
   ctx->Driver.WriteMonocolorPixels = awrite_monoindex_pixels_db;   /* same */
   ctx->Driver.WriteIndexSpan       = awrite_index_span_db;
   ctx->Driver.WriteMonoindexSpan   = awrite_monoindex_span_db;
   ctx->Driver.WriteIndexPixels     = awrite_index_pixels_db;
   ctx->Driver.WriteMonoindexPixels = awrite_monoindex_pixels_db;

   /* Pixel/span reading functions: */
   ctx->Driver.ReadIndexSpan = aread_index_span;
   ctx->Driver.ReadColorSpan = aread_color_span;
   ctx->Driver.ReadIndexPixels = aread_index_pixels;
   ctx->Driver.ReadColorPixels = aread_color_pixels;
}
/**********************************************************************/
/*****                  Amiga/Mesa Private Functions                        *****/
/**********************************************************************/
/* JAM: temporary rastport allocation and freeing */

BOOL alloc_temp_rastport(struct amigamesa_context * c) {
	struct RastPort * temprp;

DEBUGOUT("alloc_temp_rastport()\n")
	if( temprp = AllocVec( sizeof(struct RastPort), 0L)) {
		CopyMem( c->rp, temprp, sizeof(struct RastPort));
		temprp->Layer = NULL;

		if(temprp->BitMap= AllocBitMap(c->FixedWidth, 1,
										 c->rp->BitMap->Depth,0,c->rp->BitMap)) {
/* temprp->BytesPerRow == (((width+15)>>4)<<1) */
			c->temprp=temprp;
			return TRUE;
		}
		FreeVec(temprp);
	}
	printf("Error allocating temporary rastport");
	return FALSE;
}

 void free_temp_rastport(struct amigamesa_context * c) {
DEBUGOUT("free_temp_rastport\n")
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
	struct RastPort *rp;
	struct BitMap *bm;

	if (bm=AllocBitMap(width,height,depth,BMF_CLEAR|BMF_INTERLEAVED,friendbm))
		{
		if (rp = (struct RastPort *) malloc( sizeof(struct RastPort)))
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
	else
		return 0;
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
 * Construct a temporary raster for use by the given rasterport.
 * Temp rasters are used for polygon drawing.
 */

BOOL make_temp_raster( struct RastPort *rp ) {
	BOOL    OK=TRUE;
	unsigned long width, height;
	PLANEPTR p;
	struct TmpRas *tmpras;

	if(rp==0) {
		printf("Zero rp\n");
		return(FALSE);
	}
		width = rp->BitMap->BytesPerRow*8;
		height = rp->BitMap->Rows;

	/* allocate structures */
		if(p = AllocRaster( width, height )) {
			if( tmpras = (struct TmpRas *) AllocVec( sizeof(struct TmpRas),
													MEMF_ANY)) {
				InitTmpRas( tmpras, p, ((width+15)>>4)*height );
				rp->TmpRas = tmpras;
			} else
			OK=FALSE;
	} else
		return(FALSE);

	if (OK)
		return(TRUE);
	else {
		printf("Error when allocationg TmpRas\n");
		if (tmpras)
			FreeVec(tmpras);
		if (p)
			FreeRaster(p,width, height);
		return(FALSE);
		}
}

static BOOL allocarea(struct RastPort *rp ) {
	BOOL    OK=TRUE;
	struct AreaInfo *areainfo;
	UWORD *pattern;
	APTR vbuffer;

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
			} else
				OK=FALSE;
		} else
				OK=FALSE;
	} else
			OK=FALSE;

	if (OK)
		return (OK);
	else {
		printf("Error when allocationg AreaBuffers\n");
		if (vbuffer)
			FreeVec(vbuffer);
		if (pattern)
			FreeVec(pattern);
		if (areainfo)
			FreeVec(areainfo);
		return(OK);
		}
}

void freearea(struct RastPort *rp) {
	if (rp->AreaInfo) {
		if (rp->AreaInfo->VctrTbl)
			FreeVec(rp->AreaInfo->VctrTbl);
		if (rp->AreaPtrn) {
			FreeVec(rp->AreaPtrn);
			rp->AreaPtrn=NULL;
		}
		FreeVec(rp->AreaInfo);
		rp->AreaInfo=NULL;
	}
}

/*
 * Destroy a temp raster.
 */


void destroy_temp_raster( struct RastPort *rp )
	{
	/* bitmap */

	unsigned long width, height;

	width = rp->BitMap->BytesPerRow*8;
	height = rp->BitMap->Rows;

	if (rp->TmpRas)
		{
		if(rp->TmpRas->RasPtr)
			FreeRaster( rp->TmpRas->RasPtr,width,height );
		FreeVec( rp->TmpRas );
		rp->TmpRas=NULL;

		}
}


void AllocOneLine( struct amigamesa_context *c) {
	if(c->imageline)
		FreeVec(c->imageline);
	if (c->depth<=8) {
		c->imageline = AllocVec((c->width+15) & 0xfffffff0,MEMF_ANY);   /* One Line */
	} else {
		c->imageline = AllocVec(((c->width+3) & 0xfffffff0)*4,MEMF_ANY);   /* One Line */
	}
}

void FreeOneLine( struct amigamesa_context *c) {
	if(c->imageline) {
		FreeVec(c->imageline);
		c->imageline=NULL;
	}
}
/**********************************************************************/
/*****                  Amiga/Mesa private init/despose/resize       *****/
/**********************************************************************/

static void
Amiga_Standard_resize( GLcontext *ctx,GLuint *width, GLuint *height)
	{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

DEBUGOUT("Amiga_Standard_resize\n")
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

		if(!make_temp_raster( amesa->rp ))
			printf("Error allocating TmpRasterPort\n");

		/* JAM: added alloc_temp_rastport */
		alloc_temp_rastport(amesa);

	
		AllocOneLine(amesa);
		}
	}

void
Amiga_Standard_Dispose(struct amigamesa_context *c)
	{
DEBUGOUT("Amiga_Standard_Dispose\n")
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
Amiga_Standard_SwapBuffer(struct amigamesa_context *amesa)
{
DEBUGOUT("Amiga_Standard_SwapBuffer\n")

		if (amesa->back_rp)
			{
			UBYTE minterm = 0xc0;
/*          int x = amesa->left; */
/*          int y = amesa->RealHeight-amesa->bottom-amesa->height; */
		
			ClipBlit( amesa->back_rp, FIXx(amesa->gl_ctx->Viewport.X), FIXy(amesa->gl_ctx->Viewport.Y)-amesa->gl_ctx->Viewport.Height+1,    /* from */
						 amesa->front_rp, FIXx(amesa->gl_ctx->Viewport.X), FIXy(amesa->gl_ctx->Viewport.Y)-amesa->gl_ctx->Viewport.Height+1,  /* to */
						 amesa->gl_ctx->Viewport.Width, amesa->gl_ctx->Viewport.Height,  /* size */
						 minterm );
/*          ClipBlit( amesa->back_rp, x, y, */

/* from */

/*
						 amesa->front_rp, x,y,  */
						 
						 /* to */
						 
						 /*
						 amesa->width, amesa->height,  */
						 
						 /* size */
						 
						 /*
						 minterm );
*/
	/* TODO Use these cordinates insted more efficent if you only use part of screen
			RectFill(amesa->rp,FIXx(CC.Viewport.X),FIXy(CC.Viewport.Y)-CC.Viewport.Height+1,FIXx(CC.Viewport.X)+CC.Viewport.Width-1,FIXy(CC.Viewport.Y));*/
			}
}


static void
Amiga_Standard_resize_db( GLcontext *ctx,GLuint *width, GLuint *height)
	{
   AmigaMesaContext amesa = (AmigaMesaContext) ctx->DriverCtx;

DEBUGOUT("Amiga_Standard_resize_db\n")

	*width=amesa->width;
	*height=amesa->height;
	
	if(!((  amesa->width  == (amesa->rp->Layer->bounds.MaxX-amesa->rp->Layer->bounds.MinX-amesa->left) ) 
	&&  ( amesa->height == (amesa->rp->Layer->bounds.MaxY-amesa->rp->Layer->bounds.MinY-amesa->bottom)  )))
		{
		/* JAM: Added free_temp_rastport() */
		free_temp_rastport(amesa);

		amesa->RealWidth =amesa->rp->Layer->bounds.MaxX-amesa->rp->Layer->bounds.MinX;
		amesa->RealHeight=amesa->rp->Layer->bounds.MaxY-amesa->rp->Layer->bounds.MinY;
		amesa->FixedWidth =((amesa->RealWidth+15)>>4)<<4;
		amesa->FixedHeight=amesa->RealHeight+1;


		*width=amesa->width = amesa->RealWidth-amesa->left;
		*height=amesa->height = amesa->RealHeight-amesa->bottom;
/*      amesa->left = 0; */
/*      amesa->bottom = 0; */
		amesa->depth = GetBitMapAttr(amesa->rp->BitMap,BMA_DEPTH);

		if (amesa->visual->db_flag && amesa->BackArray)
			{
			destroy_penbackarray(amesa->BackArray);
			if(!(amesa->BackArray=alloc_penbackarray(amesa->RealWidth,amesa->RealHeight,amesa->depth<=8 ? 1 : 4)))
				{
				printf("AmigaMesa Error Can't allocate new PenArray in that size.\n");
/*              Amiga_Standard_init(amesa); */

				}
			}

		/* JAM: added alloc_temp_rastport */
		alloc_temp_rastport(amesa);
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



void
Amiga_Standard_Dispose_db(struct amigamesa_context *c)
	{
DEBUGOUT("Amiga_Standard_Dispose_db\n")
/*  printf("I'm closing down\n");getchar(); */
	if (c->depth<=8) {
		FreeCMap(c->Screen);

  /*    JAM: Added free_temp_rastport()*/
		free_temp_rastport(c);
	}

	if (c->BackArray)
		{
		destroy_penbackarray(c->BackArray);
		}
	}

static void
Amiga_Standard_SwapBuffer_db(struct amigamesa_context *amesa)
{
DEBUGOUT("Amiga_Standard_SwapBuffer_db\n")
 
/*		WriteChunkyPixels(amesa->rp,amesa->left,amesa->RealHeight-amesa->height,amesa->RealWidth,amesa->RealHeight-amesa->bottom,amesa->BackArray,amesa->RealWidth); */

		WritePixelArray8(amesa->rp,amesa->left+amesa->gl_ctx->Viewport.X,
		                             amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
		                             amesa->left+amesa->gl_ctx->Viewport.X+amesa->gl_ctx->Viewport.Width,
		                             amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Y,
		                             amesa->BackArray,
		                             amesa->temprp);
/*		printf("WritePixelArray8(%d,%d,%d,%d);\n",
											  amesa->left+amesa->gl_ctx->Viewport.X,
		                             amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Height-amesa->gl_ctx->Viewport.Y,
		                             amesa->left+amesa->gl_ctx->Viewport.X+amesa->gl_ctx->Viewport.Width,
		                             amesa->RealHeight-amesa->bottom-amesa->gl_ctx->Viewport.Y);
*/

}




BOOL
Amiga_Standard_init_db(struct amigamesa_context *c,struct TagItem *tagList)
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

	c->RealWidth =c->rp->Layer->bounds.MaxX-c->rp->Layer->bounds.MinX;
	c->RealHeight=c->rp->Layer->bounds.MaxY-c->rp->Layer->bounds.MinY;
	c->FixedWidth =((c->RealWidth+15)>>4)<<4;
	c->FixedHeight=c->RealHeight+1;


	c->left = GetTagData(AMA_Left,0,tagList);
	c->bottom = GetTagData(AMA_Bottom,0,tagList);

	c->width = GetTagData(AMA_Width,c->RealWidth-c->left,tagList);
	c->height= GetTagData(AMA_Height,c->RealHeight-c->bottom,tagList);

	c->depth = GetBitMapAttr(c->rp->BitMap,BMA_DEPTH);
/*
	c->gl_ctx->BufferWidth = c->width;
	c->gl_ctx->BufferHeight = c->height;
*/
/*	c->pixel = 0;	** current drawing pen */

	AllocCMap(c->Screen);    /* colormap */
	/* JAM: added alloc_temp_rastport */
	alloc_temp_rastport(c);


	if (c->visual->db_flag)
		{
		c->BackArray=alloc_penbackarray(c->FixedWidth,c->FixedHeight,c->depth<=8 ? 1 : 4);
#ifdef DEBUGPRINT
printf("byte array at -------->0x%x ends at 0x%x size=0x%x (%d)\n",c->BackArray,c->BackArray+(((c->RealWidth+15)>>4)<<4)*(c->RealHeight+1),(((c->RealWidth+15)>>4)<<4)*(c->RealHeight+1),(((c->RealWidth+15)>>4)<<4)*(c->RealHeight+1));
#endif
		c->gl_ctx->Color.DrawBuffer = GL_BACK;

		c->InitDD=amiga_Faster_DD_pointers;  /*fast drawing*/
		c->Dispose=Amiga_Standard_Dispose_db;
		c->SwapBuffer=Amiga_Standard_SwapBuffer_db;
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


BOOL
Amiga_Standard_init(struct amigamesa_context *c,struct TagItem *tagList)
	{
DEBUGOUT("Amiga_Standard_init\n")
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
DEBUGOUT("Dubelbuff inside amigastandard")
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
	else
		{
		c->gl_ctx->Color.DrawBuffer = GL_FRONT;
		}
	AllocOneLine(c); /* A linebuffer for WritePixelLine */

	if (!make_temp_raster( c->rp ))
		printf("Error allocating TmpRastPort\n");

	alloc_temp_rastport(c);
	allocarea(c->rp);

	c->InitDD=amiga_standard_DD_pointers;  /*standard drawing*/
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
#endif

/* outdefines the whole file if ADISP_AGA isn't define*/
