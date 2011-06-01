/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#ifndef _SDL_cgxvideo_h
#define _SDL_cgxvideo_h


#include <exec/exec.h>
#include <cybergraphx/cybergraphics.h>
#include <graphics/scale.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/console.h>
#include <devices/rawkeycodes.h>

#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../../main/aros/mydebug.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this

/* Private display data */
struct SDL_PrivateVideoData {
    struct Screen 	*Public_Display; 	/* Used for events and window management */
    struct Screen 	*GFX_Display;		/* Used for graphics and colormap stuff */
    struct Window 	*SDL_Window;		/* The SDL Window */
	struct RastPort *RP;
	
    unsigned char *BlankCursor;			/* The invisible cursor */

    /* The variables used for displaying graphics */
    Uint8 *Image;						/* The image for our window */
	
    /* The current list of available video modes */
    SDL_Rect **modelist;

    /* available visuals of interest to us, sorted deepest first */
    struct {
		Uint32 visual;
		int depth;						/* number of significant bits/pixel */
		int bpp;						/* pixel quantum in bits */
    } visuals[5];						/* at most entries for 8, 15, 16, 24, 32*/
    int nvisuals;

    Uint32 vis;						/* current visual in use */
    int depth;						/* current visual depth (not bpp) */
    int currently_fullscreen;		/* Fullscreen flag */
	
	/* Support for internal mouse warping */
	int window_active;				/* Active Window flag */
	int GrabMouse;					/* Flag indicating that mouse shall be grabbed */
	int CursorVisible;				/* Flag indicating that cursor is visible */
	int LastMouseX;
	int LastMouseY;
	
	/* Double buffering handling */
	int dbuffer;					/* flag indicating that double buffering is in use */
	struct ScreenBuffer *SB[2];		/* Double buffered screens */
	struct MsgPort 	*safeport;
	struct MsgPort 	*dispport;
	ULONG safe_sigbit;
	ULONG disp_sigbit;
	struct BitMap *BlitBitMap;		/* Used to fake double buffering in Window mode, also used in FullScreen if AllocScreenBuffer fails */
	
	/* Temporary rastport used for HW bliting to non display surface */
	struct RastPort *temprp;
	
    /* Colormap handling */
    Sint32 *Pens;					/* A list of Pens that have been allocated, the size depends on the screen format */
};

/* Old variable names */
#define SDL_Display				(this->hidden->Public_Display)
#define GFX_Display				(this->hidden->GFX_Display)
#define SDL_Visual				(this->hidden->vis)
#define SDL_Window				(this->hidden->SDL_Window)
#define SDL_BlankCursor			(this->hidden->BlankCursor)
#define SDL_Image				(this->hidden->Image)
#define SDL_modelist			(this->hidden->modelist)
#define SDL_RastPort			(this->hidden->RP)
#define currently_fullscreen	(this->hidden->currently_fullscreen)
#define SDL_DisplayColormap		(this->hidden->GFX_Display->ViewPort.ColorMap)
#define SDL_Pens				(this->hidden->Pens)
#define SDL_temprp				(this->hidden->temprp)

/* Used to get the X cursor from a window-manager specific cursor */
// extern Cursor SDL_GetWMXCursor(WMcursor *cursor);

extern int CGX_CreateWindow(_THIS, SDL_Surface *screen,
			    int w, int h, int bpp, Uint32 flags);
extern int CGX_ResizeWindow(_THIS,
			SDL_Surface *screen, int w, int h, Uint32 flags);

extern void CGX_DestroyWindow(_THIS, SDL_Surface *screen);

extern struct Library 			*CyberGfxBase;
extern struct IntuitionBase 	*IntuitionBase;
extern struct GfxBase 			*GfxBase;
extern struct ExecBase 			*SysBase;
extern struct DosLibrary 		*DOSBase;

extern struct IOStdReq 			*SDL_ConReq;
extern struct MsgPort			*SDL_ConPort;

/* Private data for HW surfaces */
struct private_hwdata
{
	struct BitMap 			*bmap;				// Pointer to HW surface bitmap
	APTR 					lock;				// Pointer to lock
	struct SDL_VideoDevice 	*videodata;			// Needed to keep track of window BorderLeft and BorderTop while bliting
	APTR 					mask;				// Mask used for ColorKey bliting
	APTR					pixarrayalpha;		// Pixel array with alpha channel used for Alpha and ColorKey bliting
	ULONG					pixfmt;				// pixfmt of the surface
	BOOL 					allocated;			// Set to 1 if the bitmap has been allocated (remains 0 for the display surface)
	BOOL					update;				// Flag used to force pending display update due to locks on destination surface after unlocking
};

#endif /* _SDL_cgxvideo_h */

