/* Amigamesa.h */

/*
 * Mesa 3-D graphics library
 * Version:  1.2
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
Implementions of new drawing rutines:

you implement a own init for your rutines/hardware
and make some test and calls it from AmigaMesaCreateContext()
(look in the file src/amigamesa.c I'll thing you get it)
Be sure to fill this three ponters out:
	void (*InitDD)( void );                                 // keep track of witch drawing rutines should be used
	void (*Dispose) (struct amigamesa_context *c);  // Use this when AmigaMesaDestroyContext is called 
	void (*SwapBuffer) (void);                              // Use this when AmigaMesaSwapBuffers is called 
where InitDD sets the DD structure in orginal mesa with pointers to drawing rutines
Dispose is called when someone quits/closes down your made inits
SwapBuffer is called when one is changing buffer in dubble buffering mode

Write nice drawing rutines like those in src/amigamesa.c on make sure
that it's those you set in your InitDD rutine.

Add enum for your drawingmode for the taglist and if you need more tags also implement them
If posible some autodetection code in AmigaMesaCreateContext
Add enums and error codes if you neads 

PUT ALL YOUR NEADED DATA IN amigamesa_context->(void *)data in for your gfx driver
private structure.

Send the code to me and I will include it in the main code.
*/

/*
$Id$

$Log$
Revision 1.1  2003/08/09 00:23:26  chodorowski
Initial revision

 * Revision 1.5  1997/06/25  19:16:45  StefanZ
 * *** empty log message ***
 *
 * Revision 1.3  1996/10/06  20:35:11  StefanZ
 * Source bump before Mesa 2.0
 *
 * Revision 1.2  1996/08/14  22:16:31  StefanZ
 * New Api to amigaspecific functions, Added suport for gfx-cards
 *
 * Revision 1.1  1996/06/02  00:15:03   StefanZ
 * Initial revision
 *
 * Revision 1.0  1996/02/21  11:09:45   brianp
 * A copy of amesa.h version 1.4 in a brave atempt to make a amiga interface
 *
 */


/* Example usage:

1. Make a window using Intuition calls

2. Call AMesaCreateContext() to make a rendering context and attach it
	to the window made in step 1.

3. Call AMesaMakeCurrent() to make the context the active one.

4. Make gl* calls to render your graphics.

5. When exiting, call AMesaDestroyContext().

*/


#define ADISP_CYBERGFX

#ifndef AMIGAMESA_H
#define AMIGAMESA_H

#ifdef __cplusplus
extern "C" {
#endif


#include <intuition/intuition.h>
#include "GL/gl.h"
#ifdef __GNUC__
#include "../../src/context.h"
#include "../../src/types.h"
#else
#include "//src/context.h"
#include "//src/types.h"
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif


/*      Drawmode to use         */

#define ADISP_CYBERGFX
#define ADISP_AGA


enum AMesaError {AMESA_OUT_OF_MEM,AMESA_RASTPORT_TAG_MISSING,AMESA_SCREEN_TAG_MISSING,AMESA_WINDOW_TAG_MISSING};


struct amigamesa_visual
{
   GLvisual  *gl_visual;
   GLboolean db_flag;       /* double buffered? */
   GLboolean rgb_flag;      /* RGB mode? */
   GLboolean alpha_flag;   /* Alphacolor? */
   GLuint    depth;             /* bits per pixel (1, 8, 24, etc) */
};



struct amigamesa_buffer
{
   GLframebuffer *gl_buffer;    /* The depth, stencil, accum, etc buffers */
   /* your window handle, etc */
};


/*
 * This is the Amiga/Mesa context structure.  This usually contains
 * info about what window/buffer we're rendering too, the current
 * drawing color, etc.
 */
/*
	GLboolean db_flag;                 */ /* double buffered? *//*
	GLboolean rgb_flag;                */ /* RGB mode? *//*
*/


struct amigamesa_context
{
   GLcontext            *gl_ctx;    /* the core library context */
	struct amigamesa_visual  *visual; /* the visual context */
   struct amigamesa_buffer  *buffer; /* the buffer context */

	struct amigamesa_context *share;

	unsigned long flags;            /*0x1 = own visuel 0x2 = own buffer */

	void * data;                    /* Put your special GFX-driver data here */


	unsigned long pixel;            /* current color index or RGBA pixel value */
	unsigned long clearpixel;   /* pixel for clearing the color buffers */

	/* etc... */
	struct Window *window;          /* Not neaded if in dubbelbuff needed */        /* the Intuition window */
	struct RastPort *front_rp;  /* front rastport */
	struct RastPort *back_rp;       /* back rastport (NULL if SB or RGB) */
	UBYTE* BackArray;                   /*a pen Array big as drawing area for use in dubelbuff mode*/
	struct RastPort *rp;            /* current rastport */
	struct Screen *Screen;          /* current screen*/
	struct TmpRas *tmpras;          /* tmpras rastport */
	struct RastPort *temprp;

	GLuint depth;                       /* bits per pixel (1, 8, 24, etc) */

	GLuint  width, height;              /* drawable area */
	GLint       left, bottom;               /* offsets due to window border */
	GLint       RealWidth,RealHeight;   /* the drawingareas real size*/
	GLint       FixedWidth,FixedHeight; /* The internal buffer real size speeds up the drawing a bit*/

	unsigned long penconv[256];                 /* when allocating index 13 with a color penconv[13] is the acuat system color */
																/* penconv[] is changed if auxSetOneColor(index,r,g,b); is called */ 

	UBYTE *imageline;                   /* One Line for WritePixelRow renders */
	GLuint *rgb_buffer;                 /*back buffer when in RGBA mode OLD DElete?*/

	void (*InitDD)( GLcontext * );                                     /* keep track of witch drawing rutines should be used */
	void (*Dispose) (struct amigamesa_context *c);      /* Use this when AmigaMesaDestroyContext is called */
	void (*SwapBuffer) (struct amigamesa_context *c);   /* Use this when AmigaMesaSwapBuffers is called */
};



typedef struct amigamesa_context *AmigaMesaContext;

/**********************************************************************/
/*****                Some Usefull code                                     *****/
/**********************************************************************/

/*int RGBA(GLubyte r,GLubyte g,GLubyte b,GLubyte a);*/   /* returns an allocated color thts nearest the wanted one */


/**********************************************************************/
/*****                  Amiga/Mesa API Functions                            *****/
/**********************************************************************/
struct amigamesa_visual *AmigaMesaCreateVisualTags(long Tag1, ...);
struct amigamesa_context *AmigaMesaCreateContextTags(long Tag1, ...);
void AmigaMesaSetOneColor(struct amigamesa_context *c,int index, float r, float g, float b);
extern GLenum LastError; /* Last Error generated */
#ifdef __GNUC__
struct amigamesa_visual *AmigaMesaCreateVisual(register struct TagItem *tagList);
struct amigamesa_context *AmigaMesaCreateContext(register struct TagItem *tagList );
void AmigaMesaDestroyContext(register struct amigamesa_context *c );
void AmigaMesaMakeCurrent(register struct amigamesa_context *c ,register struct amigamesa_buffer *b);
void AmigaMesaSwapBuffers(register struct amigamesa_context *amesa);
/* This is on the drawingboard */
BOOL AmigaMesaSetDefs(register struct TagItem *tagList);
GLenum AmigaMesaReportError(register struct amigamesa_context *c );
#else
__asm __saveds struct amigamesa_visual *AmigaMesaCreateVisual(register __a0 struct TagItem *tagList);
__asm __saveds struct amigamesa_context *AmigaMesaCreateContext(register __a0 struct TagItem *tagList );
__asm __saveds void AmigaMesaDestroyContext(register __a0 struct amigamesa_context *c );
__asm __saveds void AmigaMesaMakeCurrent(register __a0 struct amigamesa_context *c ,register __a1    struct amigamesa_buffer *b);
__asm __saveds void AmigaMesaSwapBuffers(register __a0 struct amigamesa_context *amesa);
/* This is on the drawingboard */
__asm __saveds BOOL AmigaMesaSetDefs(register __a0 struct TagItem *tagList);
__asm __saveds GLenum AmigaMesaReportError(register __a0 struct amigamesa_context *c );
#endif





/*
 * Amiga Mesa Attribute tag ID's.  These are used in the ti_Tag field of
 * TagItem arrays passed to AmigaMesaSetDefs() and AmigaMesaCreateContext()
 */
#define AMA_Dummy   (TAG_USER + 32)

/*
Offset to use. WARNING AMA_Left, AMA_Bottom Specifies the low left corner
of the drawing area in deltapixles from the lowest left corner
typical AMA_Left,window->BorderLeft
		  AMA_Bottom,window->BorderBottom + 1
This is since ALL gl drawing actions is specified with this point as 0,0
and with y positive uppwards (like in real graphs).

Untuched (defult) will result in 
AMA_Left=0;
AMA_Bottom=0;
*/
#define AMA_Left        (AMA_Dummy + 0x0001)
#define AMA_Bottom  (AMA_Dummy + 0x0002)

/*
Size in pixels of drawing area if others than the whole rastport.
All internal drawingbuffers will be in this size

Untuched (defult) will result in 
AMA_Width =rp->BitMap->BytesPerRow*8;
AMA_Height=rp->BitMap->Rows;
*/
#define AMA_Width   (AMA_Dummy + 0x0003)
#define AMA_Height  (AMA_Dummy + 0x0004)

/*
This may become unneaded, and code to autodetect the gfx-card should be added


AMA_DrawMode: Specifies the drawing hardware and should be one of
				  AGA,(CYBERGFX,RETINA)
				  Defult value: AGA
if AMESA_AGA Amiga native drawigns
	this has to be filled with data
		AMA_Window = (ptr) Window to draw on
	or
		AMA_Screen =(ptr) Screen to draw on.
		AMA_RastPort =(ptr) RastPort to draw on.

if AMESA_AGA_C2P Amiga native drawing using a chunky buffer
				 thats converted when switching drawbuffer
				 only works on doublebuffered drawings.
	this has to be filled with data
		AMA_DoubleBuf = GL_TRUE
		AMA_Window = (ptr) Window to draw on
	or
		AMA_DoubleBuf = GL_TRUE
		AMA_Screen =(ptr) Screen to draw on.
		AMA_RastPort =(ptr) RastPort to draw on.

else
   here should all needed gfx-card tagitem be specified
*/

enum DrawModeID {AMESA_AGA,AMESA_AGA_C2P /*,AMESA_CYBERGFX,AMESA_RETINA*/};
#define AMA_DrawMode    (AMA_Dummy + 0x0005)
#define AMA_Screen  (AMA_Dummy + 0x0006)
#define AMA_Window  (AMA_Dummy + 0x0007)
#define AMA_RastPort    (AMA_Dummy + 0x0008)

/** booleans **/
/*
AMA_DoubleBuf: If specified it uses double Buffering (change buffer with
					AmigaMesaSwapBuffers()) Turn this on as much as posible
					it will result in smother looking and faster rendering
					Defult value: GL_FALSE
AMA_RGBMode: If specified it uses 24bit when drawing (on non 24bit displays it
				 it emuletes 24bit)
				 Defult value: GL_TRUE
AMA_AlphaFlag: Alphachanel ?
				   Defule value: GL_FALSE
*/
#define AMA_DoubleBuf (AMA_Dummy + 0x0030)
#define AMA_RGBMode  (AMA_Dummy + 0x0031)
#define AMA_AlphaFlag (AMA_Dummy + 0x0032)


/** Special **/
/*
AMA_ShareGLContext: Set the "friend" context (use multiple contexts) 
						  See the GL maual or Mesa to get more info
AMA_Visual: If you want to implement your own amigamesa_visual 
AMA_Buffer: If you want to implement your own amigamesa_buffer
AMA_WindowID: A windowID to use when I alloc AMA_Buffer for you if
				  you didn't supply one.(defult=1)
*/

#define AMA_ShareGLContext  (AMA_Dummy + 0x0040)
#define AMA_Visual          (AMA_Dummy + 0x0041)
#define AMA_Buffer          (AMA_Dummy + 0x0042)
#define AMA_WindowID            (AMA_Dummy + 0x0043)

#ifdef __cplusplus
}
#endif


#endif
