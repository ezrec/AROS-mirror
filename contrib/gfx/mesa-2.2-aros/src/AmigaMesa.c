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

#define AROS

/*
$Log$
Revision 1.2  2004/02/25 02:46:04  NicJA
updates updates updates... now builds with the standard aros mmakefile.src system - use make contrib-mesa2 to build

N.B - theres something wrong somewhere causing some demos,samples and book files to not compile (if you can see the problem please fix it ;))

Revision 1.1.1.1  2003/08/09 00:23:14  chodorowski
Amiga Mesa 2.2 ported to AROS by Nic Andrews. Build with 'make aros'. Not built by default.

 * Revision 1.16  1997/06/25  19:16:56  StefanZ
 * New drawing rutines:
 * - Now in separate files
 * - cyberGfx added.
 *
 * Revision 1.15  1996/10/13  20:54:59  StefanZ
 * A few nasty bugfixes.
 *
 * Revision 1.14  1996/10/07  00:11:07  StefanZ
 * Mesa 2.0 Fixed
 *
 * Revision 1.13  1996/10/06  20:31:50  StefanZ
 * Source Bump before Mesa 2.0
 *
 * Revision 1.12  1996/08/14  22:17:32  StefanZ
 * New API to amigacalls (uses taglist)
 * Made it more flexible to add gfx-card support.
 * Fast doublebuff rutines
 * minor bugfixes
 *
 * Revision 1.10    1996/06/12  13:06:00  StefanZ
 * BugFix and encatments by Jorge Acereda (JAM)
 *
 * Revision 1.9  1996/06/11  15:53:02   StefanZ
 * Little speedup, VerryFast colorallocation by Stefan Burstöm
 *
 * Revision 1.8  1996/06/01  23:57:47   StefanZ
 * Started to use RCS to keep track of code.
 *
 * Revision 1.7  1996/05/21  23:08:42   StefanZ
 * A few bug and enforcer fixes
 *
 * Revision 1.6  1996/04/29  22:14:31   StefanZ
 * BugFixes reported by by Daniel Jönsson
 *
 * Revision 1.5  1996/03/14  23:54:33   StefanZ
 * Doublebuffer & Tmprastport seams to work (big speed improvment)
 * a fastpolydraw is also implemented
 *
 * Revision 1.4  1996/03/07  16:55:04   StefanZ
 * Much of the code works now (RGB mode is simulated) Doublebuffers... (didn't work)
 *
 * Revision 1.3  1996/02/29  02:12:45   StefanZ
 * First sight of colors (even the right ones) maglight.c works
 *
 * Revision 1.2  1996/02/25  13:11:16   StefanZ
 * First working version. Draws everything with the same color
 * (Colormaping is now urgent needed)
 *
 * Revision 1.1  1996/02/23  22:01:15   StefanZ
 * Made changes to match latest version of ddsample 1.5
 *
 * Revision 1.0  1996/02/21  11:01:15   StefanZ
 * File created from ddsample.c ver 1.3 and amesa.c ver 1.5
 * in a brave atempt to rebuild the amiga version
 *
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
 Make the gl a sharedlibrary. (Have ben started look in /amiga)
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


#include <exec/memory.h>
#include <exec/types.h>
#ifdef __GNUC__
#ifndef AROS
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

#ifdef ADISP_CYBERGFX
#ifdef __GNUC__
#include <proto/cybergraphics.h>
#else
#include <inline/cybergraphics.h>
#endif
#endif

#include <GL/AmigaMesa.h>

#ifdef ADISP_CYBERGFX
#include "ADisp_Cyb.h"
#endif

#ifdef ADISP_AGA
#include "ADisp_AGA.h"
#endif


#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"


/* #define DEBUGPRINT */

#ifdef DEBUGPRINT
#define DEBUGOUT(x) printf(x);
#else
#define DEBUGOUT(x) /*printf(x);*/
#endif

/**********************************************************************/
/*****                Internal Data                                         *****/
/**********************************************************************/
/* struct amigamesa_context * amesa = NULL; */
GLenum LastError;                /* The last error generated*/
struct Library *CyberGfxBase = NULL; /* optional base address for cybergfx */

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



/**********************************************************************/
/**********************************************************************/

/* extern void cyb_Faster_DD_pointers( GLcontext *ctx); */
/* extern void cyb_standard_DD_pointers( GLcontext *ctx); */


/*
long _STI_200_InitMesaLibrary(void)
	{
	if(!GfxBase)
		{
**  printf("Opening graphics.library\n"); 
		if (!(GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",39)))
			{
			printf("MesaOpenGL Error\nCouldent open graphics.library v39\n");
			return(FALSE);
			}
		}
** printf("GfxBase=0x%x\n",GfxBase);
	return(TRUE);
	}

void _STD_200_DisposeMesaLibrary(void)
	{
	if (GfxBase)    ** @@@ TODO Open and close gfxlibrary when open and flush lib.
		CloseLibrary((struct Library *)GfxBase);
	GfxBase=NULL;
	}
*/

/**********************************************************************/
/*****                  Amiga/Mesa API Functions                  *****/
/**********************************************************************/
/*
 * Implement the client-visible Amiga/Mesa interface functions defined
 * in Mesa/include/GL/Amigamesa.h
 *
 **********************************************************************/
/*
 * Implement the public Amiga/Mesa interface functions defined
 * in Mesa/include/GL/AmigaMesa.h
 */
 
struct amigamesa_visual *
AmigaMesaCreateVisualTags(long Tag1, ...)
{
	return AmigaMesaCreateVisual(&Tag1);
}

#ifdef __GNUC__
struct amigamesa_visual *
AmigaMesaCreateVisual(register struct TagItem *tagList)
#else
__asm __saveds struct amigamesa_visual *
AmigaMesaCreateVisual(register __a0 struct TagItem *tagList)
#endif
{
   struct amigamesa_visual *v;
   int  index_bits;
   GLfloat redscale, greenscale, bluescale, alphascale;;

   v = (struct amigamesa_visual *)AllocVec(sizeof(struct amigamesa_visual),MEMF_PUBLIC|MEMF_CLEAR);
   if (!v) {
	  return NULL;
   }

	v->rgb_flag=GetTagData(AMA_RastPort,GL_TRUE,tagList);
	v->db_flag=GetTagData(AMA_DoubleBuf,GL_FALSE,tagList);
	v->alpha_flag=GetTagData(AMA_AlphaFlag,GL_FALSE,tagList);


   if (v->rgb_flag) {
	  /* RGB(A) mode */
	  redscale =   greenscale = bluescale = alphascale = 255;
	  index_bits = 0;
   }
   else {
	  /* color index mode */
	  redscale = 0.0;
	  greenscale = 0.0;
	  bluescale = 0.0;
	  alphascale = 0.0;
	  index_bits = 8;           /* @@@ TODO */
   }

   /* Create core visual */
   v->gl_visual = gl_create_visual( v->rgb_flag, 
									v->alpha_flag,
									v->db_flag,
												16,        /* depth_size */
												8,         /* stencil_size */
												16,        /* accum_size */
									redscale, greenscale,
									bluescale, alphascale,
									index_bits );

   return v;
}

void AmigaMesaDestroyVisual( struct amigamesa_visual *v )
{
   gl_destroy_visual( v->gl_visual );
   FreeVec( v );
}




struct amigamesa_buffer *AmigaMesaCreateBuffer( struct amigamesa_visual *visual,int windowid)
{
   struct amigamesa_buffer *b;

   b = (struct amigamesa_buffer *) AllocVec(sizeof(struct amigamesa_buffer),MEMF_PUBLIC|MEMF_CLEAR);
   if (!b) {
	  return NULL;
   }

   b->gl_buffer = gl_create_framebuffer( visual->gl_visual);

   /* other stuff */

   return b;
}



void AmigaMesaDestroyBuffer( struct amigamesa_buffer *b )
{
	gl_destroy_framebuffer( b->gl_buffer );
   FreeVec( b );
}


struct amigamesa_context *AmigaMesaCreateContextTags(long Tag1, ...)
{
	return AmigaMesaCreateContext(&Tag1);
}

#ifdef __GNUC__
struct amigamesa_context *
AmigaMesaCreateContext(register struct TagItem *tagList)
#else
__asm __saveds struct amigamesa_context *
AmigaMesaCreateContext(register __a0 struct TagItem *tagList)
#endif
{
	/* Create a new Amiga/Mesa context */
	/* Be sure to initialize the following in the core Mesa context: */
	/* DrawBuffer, ReadBuffer */    /* @@@ IMPLEMENTERA ???*/


/*  GLfloat redscale,greenscale,bluescale,alphascale; */
/*  int         I; */
/*	int drawMode; */
	struct amigamesa_context *c;

	/* try to open cybergraphics.library */
	if (CyberGfxBase==NULL)
		{
			CyberGfxBase = OpenLibrary((UBYTE*)"cybergraphics.library",0);
		}

		/* allocate amigamesa_context struct initialized to zeros */
	c = (struct amigamesa_context *) AllocVec(sizeof(struct amigamesa_context),MEMF_PUBLIC|MEMF_CLEAR);
	if (!c)
		{
		LastError=AMESA_OUT_OF_MEM;
		return(NULL);
		}


	c->visual=(struct amigamesa_visual *)GetTagData(AMA_Visual,NULL,tagList);
	c->buffer=(struct amigamesa_buffer *)GetTagData(AMA_Buffer,NULL,tagList);

	if(!c->visual)
		{
		if (!(c->visual=AmigaMesaCreateVisual(tagList)))
			{
			LastError=AMESA_OUT_OF_MEM;
			return NULL;
			}
		c->flags=c->flags||0x1;
		}

	if(!c->buffer)
		{
		if(!(c->buffer=AmigaMesaCreateBuffer( c->visual,GetTagData(AMA_WindowID,1,tagList))))
			{
			LastError=AMESA_OUT_OF_MEM;
			return NULL;
			}
		c->flags=c->flags||0x2;
		}


	c->share=(struct amigamesa_context *)GetTagData(AMA_ShareGLContext,NULL,tagList);
/*  sharetmp=c->share; */
	c->gl_ctx = gl_create_context(  c->visual->gl_visual,
												NULL,
/*                                              sharetmp ? sharetmp->gl_ctx : NULL,*/    /* TODO */
												(void *) c  );

/*	drawMode=GetTagData(AMA_DrawMode,AMESA_AGA,tagList);  */
	if(c->visual->db_flag==GL_TRUE)
		{
DEBUGOUT("This is doubelbuffered\n")
#ifdef ADISP_CYBERGFX
		if (CyberGfxBase)
			{
			if (Cyb_Standard_init_db(c,tagList))
				return c;
			else
				{
				gl_destroy_context( c->gl_ctx );
				FreeVec( c );
				return NULL;
				}
#ifdef ADISP_AGA
			}
		else
			{
#endif
#endif
#ifdef ADISP_AGA
//			if (Amiga_Standard_init_db(c,tagList))   Not realy finishid
			if (Amiga_Standard_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( c->gl_ctx );
				FreeVec( c );
				return NULL;
				}
#endif
#ifdef ADISP_CYBERGFX
			}
#endif
		}
	else  /* allways fallback on AGA when unknown drawmode */
		{
DEBUGOUT("This is NOT doubelbuffered\n")
#ifdef ADISP_CYBERGFX
		if (CyberGfxBase)
			{
			if (Cyb_Standard_init(c,tagList))
				return c;
			else
				{
				gl_destroy_context( c->gl_ctx );
				FreeVec( c );
				return NULL;
				}
#ifdef ADISP_AGA
			}
		else
			{
#endif
#endif
#ifdef ADISP_AGA

			if (Amiga_Standard_init(c,tagList))   /* Add CyberGfx init here also (if ...)*/
				return c;
			else
				{
				gl_destroy_context( c->gl_ctx );
				FreeVec( c );
				return NULL;
				}
#endif
#ifdef ADISP_CYBERGFX
			}
#endif
		}
}


#ifdef __GNUC__
void AmigaMesaDestroyContext(register struct amigamesa_context *c )
#else
__asm __saveds void AmigaMesaDestroyContext(register __a0 struct amigamesa_context *c )
#endif
{
	/* destroy a Amiga/Mesa context */

/*
	if (c==amesa)
		amesa=NULL;
*/
	(*c->Dispose)( c );

	if(c->flags&&0x1)
		AmigaMesaDestroyVisual(c->visual);
	if(c->flags&&0x2)
		AmigaMesaDestroyBuffer(c->buffer);
	
	gl_destroy_context( c->gl_ctx );
	FreeVec( c );
}


#ifdef __GNUC__
void AmigaMesaMakeCurrent(register struct amigamesa_context *amesa,register struct amigamesa_buffer *b )
#else
__asm __saveds void AmigaMesaMakeCurrent(register __a0 struct amigamesa_context *amesa,register __a1    struct amigamesa_buffer *b )
#endif
{
	/* Make the specified context the current one */
	/* the order of operations here is very important! */

//   Current = amesa;

   if (amesa && b) {
//printf("amesa->gl_ctx=0x%x\n",amesa->gl_ctx);
	   gl_make_current( amesa->gl_ctx,b->gl_buffer );
//printf("CC=0x%x\n",CC);

		(*amesa->InitDD)(amesa->gl_ctx);                            /* Call Driver_init_rutine */


	  if (amesa->gl_ctx->Viewport.Width==0) {
		 /* initialize viewport to window size */
//         gl_viewport( amesa->gl_ctx, 0, 0, amesa->width, amesa->height );
DEBUGOUT("glViewport");
			glViewport( 0, 0, amesa->width, amesa->height );
DEBUGOUT("glViewport");
	  }
	else
		{
	   gl_make_current( NULL,NULL);
		}
   }
}



#ifdef __GNUC__
void AmigaMesaSwapBuffers(register struct amigamesa_context *amesa)
#else
__asm __saveds void AmigaMesaSwapBuffers(register __a0 struct amigamesa_context *amesa)
#endif
	{                /* copy/swap back buffer to front if applicable */
	(*amesa->SwapBuffer)( amesa );
	}




/* This is on the drawingboard */
/* Mostly for when future changes the library is still intact*/

#ifdef __GNUC__
BOOL AmigaMesaSetDefs(register struct TagItem *tagList)
#else
__asm __saveds BOOL AmigaMesaSetDefs(register __a0 struct TagItem *tagList)
#endif
	{
/*
	struct TagItem *tag;
		
	tagValue=AMA_DrawMode;
	tag = FindTagItem(tagValue,tagList);
	if (tag)
		printf("Tag=0x%x, is 0x%x/n",tagValue,tag->ti_Data);
	else
		printf("Tag=0x%x is not specified/n",tagValue);
*/
	return FALSE;
	}


/*
 Maybe a report error rutine ??? like:
*/

#ifdef __GNUC__
GLenum AmigaMesaReportError(register struct amigamesa_context *c )
#else
__asm __saveds GLenum AmigaMesaReportError(register __a0 struct amigamesa_context *c )
#endif
	{
	GLenum error;
	error=LastError;
#warning :TODO fix next line
        //LastError=NULL;
	return(error);
	}


#undef DEBUGPRINT
