/* $Id$*/
/*
 * (c) Copyright 1993, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */

/* Mesa tkAROS by Stefan Z (d94sz@efd.lth.se)*/

/*$Log$
 *Revision 1.2  2005/01/13 08:59:00  NicJA
 *fixed a couple more rendering issues, and corrected mouse coordinate passing from tk
 *
/*Revision 1.1  2005/01/11 14:58:28  NicJA
/*AROSMesa 3.0
/*
/*- Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
/*- GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
/*- Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
/*
/*Revision 1.3  2005/01/08 02:59:10  NicJA
/*Fixed problems causing mesa2 to not compile/the demos to have unresolved symbols.  seperated mmafile.src into seperate files for each component.  can someone please check the mmakefile.src'c in demo and samples since they dont copy the data files as expected ..
/*
/*Revision 1.2  2004/06/29 00:32:39  NicJA
/*A few build related fixes (remove windows line endings in make scripts - python doesnt like them).
/*
/*Still doesnt link to the examples correctly - allthought the linklibs seem to compile as expected??
/*
/*Revision 1.1.1.1  2003/08/09 00:23:01  chodorowski
/*Amiga Mesa 2.2 ported to AROS by Nic Andrews. Build with 'make aros'. Not built by default.
/*
 * Revision 1.8  1997/06/25  19:17:47  StefanZ
 * bumped to mesa 2.2
 *
 * Revision 1.5  1996/10/07  00:18:11  StefanZ
 * Mesa 2.0 Fixed
 *
 * Revision 1.4  1996/08/14  22:53:16  StefanZ
 * rev 1.3 tk fixes was from George 'Wulf' Krämer
 *
 * Revision 1.3  1996/08/14  22:23:31  StefanZ
 * Modified due to api change in AmigaMesa
 * Implemented div. Input modifier and changed windowhandling
 *
 * Revision 1.2  1996/06/02  00:03:03  StefanZ
 * Started to use RCS to keep track of code.
 *
 */
/*
History:

1.0 960315 Now almost evetything is implemented
1.1 960425 Fixed problem with double closeclicks (Thanx to Daniel Jönsson)
1.2 960731 Modified due to api change in AmigaMesa
1.3 Implemented div. Input modifier and changed windowhandling (Georg 'Wulf' Krämer)

TODO:
Exposefunc
*/

//#define USE_CLIP_LAYER
#undef USE_CLIP_LAYER


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <intuition/intuition.h>
#include <devices/inputevent.h>
#ifdef __GNUC__
#ifndef __AROS__
#include <inline/exec.h>
#include <inline/intuition.h>
#include <inline/graphics.h>

#ifdef USE_CLIP_LAYER
#include <inline/layers.h>
#endif
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#ifdef USE_CLIP_LAYER
#include <proto/layers.h>
#endif
#endif
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#ifdef USE_CLIP_LAYER
#include <proto/layers.h>
#endif
#endif

#include "gltk.h"
#include "../../lib/include/GL/AROSMesa.h"



#define static

#if defined(__cplusplus) || defined(c_plusplus)
#define class c_class
#endif

#if DBG
#define TKASSERT(x)                                     \
if ( !(x) ) {                                           \
	PrintMessage("%s(%d) Assertion failed %s\n",        \
		__FILE__, __LINE__, #x);                        \
}
#else
#define TKASSERT(x)
#endif  /* DBG */

/* Some Bitmasks for Amigaevents */
#define ControlMask         (IEQUALIFIER_CONTROL)
#define ShiftMask           (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)
#define MousePressedMask    (SELECTDOWN | MENUDOWN | MIDDLEDOWN)

/******************************************************************************/

struct wnd 
{
    int x, y, width, height;
    GLenum type;
    struct arosmesa_context *context;
};

struct wnd win={0,0,100,100,TK_INDEX,NULL};

/*
struct Library *IntuitionBase;
struct Library *GfxBase;
*/

struct Screen *screen = NULL;
struct Window *tkhwnd = NULL;

#ifdef USE_CLIP_LAYER
struct Region *tkclipreg = NULL;
#endif

/* Local prototypes */
struct Region *clipWindow(struct Window *win, LONG minX, LONG minY, LONG maxX, LONG maxY);
struct Region *clipWindowToBorders(struct Window *win);


GLboolean tkPopupEnable = TRUE;

/* Fixed palette support.  */
/*
#define BLACK   PALETTERGB(0,0,0)
#define WHITE   PALETTERGB(255,255,255)
#define NUM_STATIC_COLORS   (COLOR_BTNHIGHLIGHT - COLOR_SCROLLBAR + 1)
*/
static void (*ExposeFunc)(int, int)              = NULL;
static void (*ReshapeFunc)(GLsizei, GLsizei)     = NULL;
static void (*DisplayFunc)(void)                 = NULL;
static GLenum (*KeyDownFunc)(int, GLenum)        = NULL;
static GLenum (*MouseDownFunc)(int, int, GLenum) = NULL;
static GLenum (*MouseUpFunc)(int, int, GLenum)   = NULL;
static GLenum (*MouseMoveFunc)(int, int, GLenum) = NULL;
static void (*IdleFunc)(void)                    = NULL;

/*
 *  Prototypes for the debugging functions go here
 */

#define DBGFUNC 0
#if DBGFUNC

static void DbgPrintf( const char *Format, ... );
static void pwi( void );
static void pwr(RECT *pr);
/*static void ShowPixelFormat(HDC hdc);*/

#endif

#ifdef USE_CLIP_LAYER
/* Get from clipping example of the RKM */
/*
** clipWindow()
** Clip a window to a specified rectangle (given by upper left and
** lower right corner.)  the removed region is returned so that it
** may be re-installed later.
*/
struct Region *clipWindow(struct Window *win,
	LONG minX, LONG minY, LONG maxX, LONG maxY)
{
struct Region    *new_region=NULL;
struct Rectangle  my_rectangle;

/* set up the limits for the clip */
my_rectangle.MinX = minX;
my_rectangle.MinY = minY;
my_rectangle.MaxX = maxX;
my_rectangle.MaxY = maxY;

/* get a new region and OR in the limits. */
if (NULL != (new_region = NewRegion()))
	{
	if (FALSE == OrRectRegion(new_region, &my_rectangle))
		{
		DisposeRegion(new_region);
		new_region = NULL;
		}
	}

/* Install the new region, and return any existing region.
** If the above allocation and region processing failed, then
** new_region will be NULL and no clip region will be installed.
*/
return(InstallClipRegion(win->WLayer, new_region));
}

/*
** clipWindowToBorders()
** clip a window to its borders.
** The removed region is returned so that it may be re-installed later.
*/
struct Region *clipWindowToBorders(struct Window *win)
{
return(clipWindow(win, win->BorderLeft, win->BorderTop,
	win->Width - win->BorderRight - 1, win->Height - win->BorderBottom - 1));
}
#endif

float tkRGBMap[8][3] = 
	{
	{   0, 0, 0 },
	{   1, 0, 0 },
	{   0, 1, 0 },
	{   1, 1, 0 },
	{   0, 0, 1 },
	{   1, 0, 1 },
	{   0, 1, 1 },
	{   1, 1, 1 }
	};

int atk_setIDCMPs(void)
	{
	int mask;
	mask=IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE;
	if (ReshapeFunc)
		mask |=IDCMP_NEWSIZE;
	if (KeyDownFunc)
		mask |=(IDCMP_RAWKEY | IDCMP_VANILLAKEY);
	if (MouseDownFunc||MouseUpFunc)
		mask |=IDCMP_MOUSEBUTTONS;
	if (MouseMoveFunc)
		mask |= IDCMP_MOUSEMOVE;

	return(mask);
	}

/* TODO (*ExposeFunc)(int, int)   */

void atk_modifyIDCMP(void)
	{
	if(tkhwnd)
		{
		ModifyIDCMP( tkhwnd, atk_setIDCMPs() );
		}
	}


int atk_FixKeyRAW(char c)
	{
	int key;
	switch (c) 
		{
		/*
	  case XK_Return:   key = TK_RETURN;    break;
	  case XK_Escape:   key = TK_ESCAPE;    break;
		*/
	  case CURSORLEFT:   key = TK_LEFT;      break;
	  case CURSORUP:     key = TK_UP;        break;
	  case CURSORRIGHT:  key = TK_RIGHT;     break;
	  case CURSORDOWN:   key = TK_DOWN;      break;
	  default:      key = GL_FALSE;     break;
	}
	return(key);
	}


/***************************************************************
 *                                                             *
 *  Exported Functions go here                                 *
 *                                                             *
 ***************************************************************/


void tkNewCursor(GLint id, GLubyte *shapeBuf, GLubyte *maskBuf, GLenum fgColor,
	 GLenum bgColor, GLint hotX, GLint hotY)
{
/* cursor.c */ 
}

void tkSetCursor(GLint id)
{
/* cursor.s*/
}

void tkErrorPopups(GLboolean bEnable)
{
	tkPopupEnable = bEnable;
}

void tkCloseWindow(void)
	{
	if (win.context) 
		{
		AROSMesaDestroyContext(win.context);
		win.context=NULL;
		}
#ifdef USE_CLIP_LAYER
	if (tkclipreg)
		{
		tkclipreg = InstallClipRegion(tkhwnd->WLayer, tkclipreg);
		DisposeRegion(tkclipreg);
		tkclipreg = NULL;
		}
#endif
	if (tkhwnd)
		{
		CloseWindow(tkhwnd);
		tkhwnd=NULL;
		}
	/* we don't open them so don't close them /
	if (GfxBase)
		{
		CloseLibrary(GfxBase);
		GfxBase=NULL;
		}
	if (IntuitionBase)
		{
		CloseLibrary(IntuitionBase);
		IntuitionBase=NULL;
		}
	*/
	}



//#define PL printf("%d\n",__LINE__);

void tkExec(void)
	{
	struct IntuiMessage *msg=NULL;
	BOOL done = FALSE;
	BOOL press=FALSE;
	int key,id;
	/* Redraw handling changed by Wulf 11. Aug. 96 to ensure the
	   same handling under all systems */
	GLenum Redraw=GL_FALSE;

//printf("tkExec\n");

	if (ReshapeFunc)
		{
		(*ReshapeFunc)(tkhwnd->Width-tkhwnd->BorderLeft-tkhwnd->BorderRight-2, tkhwnd->Height-tkhwnd->BorderTop-tkhwnd->BorderBottom-2);
		}

	if (DisplayFunc)
		{
		//printf("DisplayFunk\n");
		(*DisplayFunc)();
		}


	while (!done)
		{
		if ((msg = (struct IntuiMessage *)GetMsg(tkhwnd->UserPort)))
			{
			id=msg->Class;
									/*  printf("msg->Class=0x%x\n",id);*/
			// after ReplyMsg, you are nolonger allowed to use msg !
			switch (id)
				{
				case IDCMP_NEWSIZE:
								/* Sizes should be adjusted by border, but painting isn't adjusted :-( */
						win.width=  tkhwnd->Width-tkhwnd->BorderLeft-tkhwnd->BorderRight-2;
						win.height= tkhwnd->Height-tkhwnd->BorderTop-tkhwnd->BorderBottom-2;
#ifdef USE_CLIP_LAYER
						if (tkclipreg) // Get rid of last clipping region
							{
							DisposeRegion(InstallClipRegion(msg->IDCMPWindow->WLayer,NULL));
							}
						clipWindowToBorders(msg->IDCMPWindow);
#endif
						ReplyMsg((struct Message *)msg);
						if (ReshapeFunc)
							(*ReshapeFunc)(win.width, win.height);
						Redraw = GL_TRUE;
						break;
				case IDCMP_RAWKEY:
						key= atk_FixKeyRAW(msg->Code);
						if (key && KeyDownFunc)
							{
							GLenum mask;
							mask = 0;
							if (msg->Qualifier & ControlMask)
								mask |= TK_CONTROL;
							if (msg->Qualifier & ShiftMask)
								mask |= TK_SHIFT;
							ReplyMsg((struct Message *)msg);
							Redraw = (*KeyDownFunc)(key, mask);
							} else
								ReplyMsg((struct Message *)msg);
						break;
				case IDCMP_VANILLAKEY:
						if (KeyDownFunc)
							{
							GLenum mask;
							mask = 0;
							if (msg->Qualifier & ControlMask)
								mask |= TK_CONTROL;
							if (msg->Qualifier & ShiftMask)
								mask |= TK_SHIFT;
							key = msg->Code;
							ReplyMsg((struct Message *)msg);
							Redraw = (*KeyDownFunc)(key, mask);
							} else
								ReplyMsg((struct Message *)msg);
						break;
				case IDCMP_MOUSEMOVE:
						if (MouseMoveFunc) {
							int x = msg->MouseX,y = msg->MouseY;
							ReplyMsg((struct Message *)msg);
							Redraw = (*MouseMoveFunc)(x,(y-tkhwnd->BorderBottom),press);
						} else
							ReplyMsg((struct Message *)msg);
						break;                  
				case IDCMP_MOUSEBUTTONS:
					 /* Buttonhandling changed Wulf 11.08.96 */
				/* printf("code=%x\n",(WORD)msg->Code); */
						{
						GLenum mask = 0;
						int x = msg->MouseX, y = msg->MouseY;
						switch (msg->Code)
							{
							case SELECTDOWN: case SELECTUP: mask |= TK_LEFTBUTTON; break;
							case MIDDLEDOWN: case MIDDLEUP: mask |= TK_MIDDLEBUTTON; break;
							case MENUDOWN: case MENUUP:     mask |= TK_RIGHTBUTTON; break;
							}
						if (msg->Qualifier & ControlMask)
							mask |= TK_CONTROL;
						if (msg->Qualifier & ShiftMask)
							mask |= TK_SHIFT;
						if ((msg->Code & MousePressedMask))
							{
							press=mask;
							if (MouseDownFunc) {
								ReplyMsg((struct Message *)msg);
								Redraw = (*MouseDownFunc)(x,(y-tkhwnd->BorderBottom),mask);
							} else
								ReplyMsg((struct Message *)msg);
							}
						else
							{
							if (MouseUpFunc)
								{
								ReplyMsg((struct Message *)msg);
								Redraw = (*MouseUpFunc)(x,(y-tkhwnd->BorderBottom),mask);
								} else
								ReplyMsg((struct Message *)msg);
							press=0;
							}
						}
						break;
				case IDCMP_REFRESHWINDOW:   /* Not entierly tested! Wulf 11.08.96 */
						BeginRefresh(msg->IDCMPWindow);  /* Refresh Gadgets */
						EndRefresh(msg->IDCMPWindow, TRUE);
						ReplyMsg((struct Message *)msg);
						if (ExposeFunc)
							(*ExposeFunc)(win.width, win.height);
						Redraw = GL_TRUE;
						break;
				case IDCMP_CLOSEWINDOW:
						ReplyMsg((struct Message *)msg);
						done = TRUE;
						break;
				default:
						ReplyMsg((struct Message *)msg);
						break;
				}
			}
								/* TODO Fill this with tests and call apropriate functions */
			if (IdleFunc)
				{
				(*IdleFunc)();
				Redraw = GL_TRUE;
				}
			if ((Redraw==GL_TRUE) && DisplayFunc) /* Redraw handling changed by Wulf 11. Aug. 96 */
				{
				(*DisplayFunc)();
				Redraw = GL_FALSE;
				}
			if(!done && !IdleFunc)
				{
				WaitPort(tkhwnd->UserPort);
				}
		}
	tkQuit();      /* do not return after quit */
}


void tkExposeFunc(void (*Func)(int, int))
	{
/*  printf("ExposeFunc\n");*/
	ExposeFunc = Func;
	atk_modifyIDCMP();
	}

void tkReshapeFunc(void (*Func)(GLsizei, GLsizei))
	{
/*  printf("ReshapeFunc\n");*/
	ReshapeFunc = Func;
	atk_modifyIDCMP();
	}

void tkDisplayFunc(void (*Func)(void))
	{
/*  printf("DisplayFunc\n");*/
	DisplayFunc = Func;
	}

void tkKeyDownFunc(GLenum (*Func)(int, GLenum))
	{
/*  printf("KeyDownFunc\n");*/
	KeyDownFunc = Func;
	atk_modifyIDCMP();
	}

void tkMouseDownFunc(GLenum (*Func)(int, int, GLenum))
	{
/*  printf("MouseDownFunc\n");*/
	MouseDownFunc = Func;
	atk_modifyIDCMP();
	}

void tkMouseUpFunc(GLenum (*Func)(int, int, GLenum))
	{
/*  printf("MouseUpFunc\n");*/
	MouseUpFunc = Func;
	atk_modifyIDCMP();
	}

void tkMouseMoveFunc(GLenum (*Func)(int, int, GLenum))
	{
/*  printf("MouseMoveFunc\n");*/
	MouseMoveFunc = Func;
	atk_modifyIDCMP();
	}

void tkIdleFunc(void (*Func)(void))
	{
/*  printf("IdleFunc\n");*/
	IdleFunc = Func;
	}

void tkInitPosition(int x, int y, int width, int height)
	{
	win.x=x;
	win.y=y;
	win.width=width;
	win.height=height;
	}

void tkInitDisplayMode(GLenum type)
	{
	win.type = type;
	}

GLenum tkInitWindow(char *title)
	{
	GLenum   Result =GL_FALSE,
				RGB_Flag    =GL_TRUE,
				DB_Flag =GL_FALSE;

	/* autocode opens and close libs
	if (IntuitionBase = OpenLibrary("intuition.library",37))
		{
		if (GfxBase = OpenLibrary("graphics.library",37))
			{ */
			if (!(screen = LockPubScreen("MESA")))
				screen = LockPubScreen(NULL);
			if (screen)
				{
											/* open the window on the public screen */


				tkhwnd = OpenWindowTags(NULL,
							WA_Left,  win.x,            WA_Top,    win.x,
							WA_InnerWidth, win.width,   WA_InnerHeight, win.height,
							WA_DragBar,                 TRUE,
							WA_CloseGadget,         TRUE,
							WA_SizeGadget,          TRUE,
							WA_DepthGadget,         TRUE,
							WA_SmartRefresh,            TRUE,
							WA_ReportMouse,         TRUE,
							WA_NoCareRefresh,           TRUE,
							WA_RMBTrap,                 TRUE, /* Detect right mouse events, no Menus */
							WA_SizeBBottom,         TRUE,
							WA_MinWidth,100,        WA_MinHeight,30,
							WA_MaxWidth,-1,         WA_MaxHeight,-1,
							WA_IDCMP,               atk_setIDCMPs(),
							WA_Flags,               WFLG_SIZEGADGET | WFLG_DRAGBAR,
							WA_Title,               title,
							WA_PubScreen,               screen,
							TAG_END);
											/* Unlock the screen.  The window now acts as a lock on
											** the screen, and we do not need the screen after the
											** window has been closed.  */
				UnlockPubScreen(NULL, screen);
				if (tkhwnd)
					{
#ifdef USE_CLIP_LAYER
					tkclipreg = clipWindowToBorders(tkhwnd);
#endif
					if (win.type & TK_INDEX)
						{
						RGB_Flag=GL_FALSE;
						}
					if (win.type & TK_DOUBLE)
						{
						DB_Flag=GL_TRUE;
						}

					win.context=AROSMesaCreateContextTags(
							AMA_DrawMode,                   AMESA_AGA,
							AMA_Window,(unsigned long)      tkhwnd,
							AMA_RastPort,(unsigned long)    tkhwnd->RPort,
							AMA_Screen,(unsigned long)      tkhwnd->WScreen,
							AMA_DoubleBuf,                  DB_Flag,
							AMA_RGBMode,                    RGB_Flag,

							AMA_Left,                       tkhwnd->BorderLeft,
							AMA_Right,                      tkhwnd->BorderRight+1,
							AMA_Top,                        tkhwnd->BorderTop,
							AMA_Bottom,                     tkhwnd->BorderBottom+1,
							TAG_DONE,0);

					AROSMesaMakeCurrent(win.context,win.context->buffer);
					return GL_TRUE;
					}
				else
					{
					printf("Failed to open window.\n");
					return(Result);
					}
				}
/*          }   
		}  */
}



/******************************************************************************/

/*
 * You cannot just call DestroyWindow() here.  The programs do not expect
 * tkQuit() to return;  DestroyWindow() just sends a WM_DESTROY message
 */

void tkQuit(void)
{
	tkCloseWindow();
   exit(0);    /*TODO*/
}

/******************************************************************************/

void tkSetOneColor(int index, float r, float g, float b)
{
	AROSMesaSetOneColor(win.context,index,r,g,b);
}

void tkSetFogRamp(int density, int startIndex)
{
/* TODO */
printf("tkSetFogRamp(%d,%d) TODO\n",density,startIndex);
}

void tkSetGreyRamp(void)
{
 /* TODO */
printf("tkSetGreyRamp() TODO\n");

}

void tkSetRGBMap( int Size, float *Values )
{
/* TODO */
printf("tkSetRGBMap(%d,%f) TODO\n",Size,*Values);
}

void tkSetOverlayMap(int size, float *rgb)
{
printf("tkSetOverlayMap(%d,%f) TODO\n",size,rgb);
}
/******************************************************************************/

void tkSwapBuffers(void)
{
  AROSMesaSwapBuffers(win.context);
}

/******************************************************************************/

GLint tkGetColorMapSize(void)
{
 /* TODO */
	printf("tkGetColorMapSize() TODO\n");
	return(255);
}

void tkGetMouseLoc(int *x, int *y)
{
	*x = tkhwnd->MouseX;
	*y = (tkhwnd->MouseY - tkhwnd->BorderBottom);
}


GLenum tkGetDisplayMode(void)
{
	return win.type;
}

GLenum tkSetWindowLevel(GLenum level)
{
printf("tkSetWindowLevel(%d) TODO\n",level);
return GL_FALSE;
}

/*
TK_RGBImageRec *tkRGBImageLoad(char *fileName)
{
printf("tkRGBImageLoad(%s) TODO\n",fileName);
}
*/

void tkGetSystem(TKenum type, void *ptr)
{
printf("tkGetSystem(%d,*ptr) TODO\n",type);
/* getset.c */
}
#undef USE_CLIP_LAYER
