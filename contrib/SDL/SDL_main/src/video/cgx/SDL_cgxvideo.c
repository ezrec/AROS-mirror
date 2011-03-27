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

/*
 * CGX based SDL video driver implementation by Gabriele Greco
 * gabriele.greco@aruba.it
 */

#include "SDL_endian.h"
#include "SDL_timer.h"
#include "SDL_thread.h"
#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"
#include "SDL_cgxgl_c.h"
#include "SDL_cgxvideo.h"
#include "SDL_cgxwm_c.h"
#include "SDL_amigamouse_c.h"
#include "SDL_amigaevents_c.h"
#include "SDL_cgxmodes_c.h"
#include "SDL_cgximage_c.h"
#include "SDL_cgxaccel_c.h"

#include <aros/macros.h>

struct Library *CyberGfxBase=NULL;

/* Initialization/Query functions */
static int 			CGX_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Surface *CGX_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int 			CGX_ToggleFullScreen(_THIS, int on);
static void 		CGX_UpdateMouse(_THIS);
static int 			CGX_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void 		CGX_VideoQuit(_THIS);
static int 			CGX_ResizeFullScreen(_THIS);
static int 			CGX_EnterFullScreen(_THIS);
static int 			CGX_LeaveFullScreen(_THIS);

/* Gamma correction functions */
int CGX_SetGamma(_THIS, float red, float green, float blue);
int CGX_GetGamma(_THIS, float *red, float *green, float *blue);
int CGX_SetGammaRamp(_THIS, Uint16 *ramp);

/* CGX driver bootstrap functions */
static int 				CGX_Available(void);
static void 			CGX_DeleteDevice(SDL_VideoDevice *device);
static SDL_VideoDevice *CGX_CreateDevice(int devindex);

/* Bootstrap */
VideoBootStrap CGX_bootstrap = {
	"CGX", "AmigaOS CyberGraphics", CGX_Available, CGX_CreateDevice
};


static SDL_VideoDevice *CGX_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	
	if ( device )
	{
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)SDL_malloc((sizeof *device->hidden));
		device->gl_data = (struct SDL_PrivateGLData *)SDL_malloc((sizeof *device->gl_data));
	}
	
	if ( (device == NULL) || (device->hidden == NULL) || (device->gl_data == NULL) )
	{
		D(bug("Unable to create video device!\n"));
		SDL_OutOfMemory();
		CGX_DeleteDevice(device);
		return(0);
	}
	
	SDL_memset(device->hidden, 0, sizeof(*device->hidden));
	SDL_memset(device->gl_data, 0, sizeof(*device->gl_data));

	/* Set the driver flags */
	device->handles_any_size	= 1;

	/* Set the function pointers */
	device->VideoInit			= CGX_VideoInit;
	device->ListModes			= CGX_ListModes;
	device->SetVideoMode 		= CGX_SetVideoMode;
	device->ToggleFullScreen 	= CGX_ToggleFullScreen;
	device->UpdateMouse 		= CGX_UpdateMouse;
	device->SetColors 			= CGX_SetColors;
	device->UpdateRects 		= NULL;
	device->VideoQuit 			= CGX_VideoQuit;
	device->AllocHWSurface 		= CGX_AllocHWSurface;
#ifndef NO_AMIGAHWSURF
	device->CheckHWBlit 		= CGX_CheckHWBlit;
	device->SetHWColorKey 		= CGX_SetHWColorKey;
#else
	device->CheckHWBlit 		= NULL;
	device->SetHWColorKey 		= NULL;
#endif
	device->FillHWRect 			= CGX_FillHWRect;
	device->SetHWAlpha 			= NULL;
	device->LockHWSurface 		= CGX_LockHWSurface;
	device->UnlockHWSurface 	= CGX_UnlockHWSurface;
	device->FlipHWSurface 		= CGX_FlipHWSurface;
	device->FreeHWSurface 		= CGX_FreeHWSurface;
	device->SetGamma 			= CGX_SetGamma;
	device->GetGamma 			= CGX_GetGamma;
	device->SetGammaRamp 		= CGX_SetGammaRamp;
	device->GetGammaRamp 		= NULL;
#if SDL_VIDEO_OPENGL
	device->GL_LoadLibrary 		= CGX_GL_LoadLibrary;
	device->GL_GetProcAddress 	= CGX_GL_GetProcAddress;
	device->GL_GetAttribute 	= CGX_GL_GetAttribute;
	device->GL_MakeCurrent 		= CGX_GL_MakeCurrent;
	device->GL_SwapBuffers 		= CGX_GL_SwapBuffers;
#endif
	device->SetIcon 			= CGX_SetIcon;
	device->SetCaption 			= CGX_SetCaption;
	device->IconifyWindow 		= NULL; /* CGX_IconifyWindow; */
	device->GrabInput 			= amiga_GrabInput; /* CGX_GrabInput;*/
	device->GetWMInfo 			= CGX_GetWMInfo;
	device->FreeWMCursor 		= amiga_FreeWMCursor;
	device->CreateWMCursor 		= amiga_CreateWMCursor;
	device->ShowWMCursor 		= amiga_ShowWMCursor;
	device->WarpWMCursor 		= amiga_WarpWMCursor;
	device->CheckMouseMode 		= amiga_CheckMouseMode;
	device->InitOSKeymap 		= amiga_InitOSKeymap;
	device->PumpEvents 			= amiga_PumpEvents;

	device->free 				= CGX_DeleteDevice;

	return device;
}


static void DestroyScreen(_THIS)
{
  	if(currently_fullscreen)
	{
		if(this->hidden->dbuffer)
		{
			D(bug("Freeing double buffering stuff..."));

			this->hidden->dbuffer=0;
			if(this->hidden->SB[1]) FreeScreenBuffer(SDL_Display,this->hidden->SB[1]);
			if(this->hidden->SB[0]) FreeScreenBuffer(SDL_Display,this->hidden->SB[0]);

			D(bug("Screen buffers OK..."));			
			
			if(this->hidden->safeport)
			{
				while(GetMsg(this->hidden->safeport)!=NULL);
				DeleteMsgPort(this->hidden->safeport);
			}
			D(bug("safeport OK..."));			
			if(this->hidden->dispport)
			{
				while(GetMsg(this->hidden->dispport)!=NULL);
				DeleteMsgPort(this->hidden->dispport);
			}
			D(bug("dispport OK..."));			

			this->hidden->SB[0]=this->hidden->SB[1]=NULL;

			if(SDL_RastPort && SDL_RastPort != &SDL_Display->RastPort) FreeRastPort(SDL_RastPort);
			SDL_RastPort=NULL;
			D(bug("Rastport OK..."));			
		}
		D(bug("Closing screen..."));
		CloseScreen(GFX_Display);
		currently_fullscreen=0;
	}
	else if(GFX_Display)
		UnlockPubScreen(NULL,GFX_Display);

	GFX_Display = NULL;
	D(bug("Ok\n"));
}

static int CGX_Available(void)
{
	struct Library *l;

	l = OpenLibrary("cybergraphics.library",0L);

	if ( l != NULL ) {
		D(bug("CGX video device AVAILABLE\n"));
		CloseLibrary(l);
	}
	D(else bug("**CGX video device UNAVAILABLE\n"));

	return(l != NULL);
}

static void CGX_DeleteDevice(SDL_VideoDevice *device)
{
	if ( device ) {
		if ( device->hidden ) {
			SDL_free(device->hidden);
		}
		if ( device->gl_data ) {
			SDL_free(device->gl_data);
		}
		SDL_free(device);
	}
}

void MakeBitMask(_THIS, int format,int *bpp, Uint32 *Rmask, Uint32 *Gmask, Uint32 *Bmask, Uint32 *Amask)
{
	if (this->hidden->depth==*bpp)
	{
		D(bug("REAL pixel format: "));

		switch(format) 
		{
			case PIXFMT_LUT8:
				D(bug("PIXFMT_LUT8\n"));
				*Rmask = 0;
				*Gmask = 0;
				*Bmask = 0;
				*Amask = 0;
				break;
			case PIXFMT_BGR15:
				D(bug("PIXFMT_BGR15\n"));
				*Rmask = AROS_BE2WORD(0x001f);
				*Gmask = AROS_BE2WORD(0x03e0);
				*Bmask = AROS_BE2WORD(0x7c00);
				*Amask = 0;
				break;
			case PIXFMT_RGB15:
				D(bug("PIXFMT_RGB15\n"));
				*Rmask = AROS_BE2WORD(0x7c00);
				*Gmask = AROS_BE2WORD(0x03e0);
				*Bmask = AROS_BE2WORD(0x001f);
				*Amask = 0;
				break;
			case PIXFMT_RGB15PC:
				D(bug("PIXFMT_RGB15PC\n"));
				*Rmask = AROS_BE2WORD(0x007c);
				*Gmask = AROS_BE2WORD(0xe003);
				*Bmask = AROS_BE2WORD(0x1f00);
				*Amask = 0;
				break;
			case PIXFMT_BGR15PC:
				D(bug("PIXFMT_BGR15PC\n"));
				*Rmask = AROS_BE2WORD(0x1f00);
				*Gmask = AROS_BE2WORD(0xe003);
				*Bmask = AROS_BE2WORD(0x007c);
				*Amask = 0;
				break;
			case PIXFMT_RGB16:
				D(bug("PIXFMT_RGB16\n"));
				*Rmask = AROS_BE2WORD(0xf800);
				*Gmask = AROS_BE2WORD(0x07e0);
				*Bmask = AROS_BE2WORD(0x001f);
				*Amask = 0;
				break;
			case PIXFMT_BGR16:
				D(bug("PIXFMT_BGR16\n"));
				*Rmask = AROS_BE2WORD(0x001f);
				*Gmask = AROS_BE2WORD(0x07e0);
				*Bmask = AROS_BE2WORD(0xf800);
				*Amask = 0;
				break;
			case PIXFMT_RGB16PC:
				D(bug("PIXFMT_RGB16PC\n"));
				*Rmask = AROS_BE2WORD(0x00f8);
				*Gmask = AROS_BE2WORD(0xe007);
				*Bmask = AROS_BE2WORD(0x1f00);
				*Amask = 0;
				break;
			case PIXFMT_BGR16PC:
				D(bug("PIXFMT_BGR16PC\n"));
				*Rmask = AROS_BE2WORD(0x1f00);
				*Gmask = AROS_BE2WORD(0xe007);
				*Bmask = AROS_BE2WORD(0x00f8);
				*Amask = 0;
				break;
			case PIXFMT_RGB24:
				D(bug("PIXFMT_RGB24\n"));
				*Rmask = AROS_BE2LONG(0x00ff0000);
				*Gmask = AROS_BE2LONG(0x0000ff00);
				*Bmask = AROS_BE2LONG(0x000000ff);
				*Amask = 0;
				break;
			case PIXFMT_BGR24:
				D(bug("PIXFMT_BGR24\n"));
				*Rmask = AROS_BE2LONG(0x000000ff);
				*Gmask = AROS_BE2LONG(0x0000ff00);
				*Bmask = AROS_BE2LONG(0x00ff0000);
				*Amask = 0;
				break;
			case PIXFMT_ARGB32:
				D(bug("PIXFMT_ARGB32\n"));
				*Rmask = AROS_BE2LONG(0x00ff0000);
				*Gmask = AROS_BE2LONG(0x0000ff00);
				*Bmask = AROS_BE2LONG(0x000000ff);
				*Amask = AROS_BE2LONG(0xff000000);
				break;
			case PIXFMT_BGRA32:
				D(bug("PIXFMT_BGRA32\n"));
				*Rmask = AROS_BE2LONG(0x0000ff00);
				*Gmask = AROS_BE2LONG(0x00ff0000);
				*Bmask = AROS_BE2LONG(0xff000000);
				*Amask = AROS_BE2LONG(0x000000ff);
				break;
			case PIXFMT_RGBA32:
				D(bug("PIXFMT_RGBA32\n"));
				*Rmask = AROS_BE2LONG(0xff000000);
				*Gmask = AROS_BE2LONG(0x00ff0000);
				*Bmask = AROS_BE2LONG(0x0000ff00);
				*Amask = AROS_BE2LONG(0x000000ff);
				break;
			default:
				D(bug("Unknown pixel format! Default to RGB24\n"));
				*Rmask = AROS_BE2LONG(0x00ff0000);
				*Gmask = AROS_BE2LONG(0x0000ff00);
				*Bmask = AROS_BE2LONG(0x000000ff);
				*Amask = 0;
				break;
		}
	} 
	else 
	{
		D(bug("DIFFERENT from screen.\nAllocated screen format: "));

		switch(*bpp)
		{
			case 32:
				D(bug("ARGB32\n"));
				*Rmask = AROS_BE2LONG(0x00ff0000);
				*Gmask = AROS_BE2LONG(0x0000ff00);
				*Bmask = AROS_BE2LONG(0x000000ff);
				*Amask = AROS_BE2LONG(0xff000000);
				break;
			case 24:
				D(bug("0RGB32\n"));
				*Rmask = AROS_BE2LONG(0x00ff0000);
				*Gmask = AROS_BE2LONG(0x0000ff00);
				*Bmask = AROS_BE2LONG(0x000000ff);
				*Amask = 0;
				break;
			case 16:
			case 15:
				D(bug("Not supported, switching to 24bit!\n"));
				*bpp=24;
				MakeBitMask(this, format, bpp, Rmask, Gmask, Bmask, Amask);
				break;
			default:
				D(bug("This is a chunky display\n"));
				*Rmask = 0;
				*Gmask = 0;
				*Bmask = 0;
				*Amask = 0;
				break;
		}
	}
	D(bug("Mask created :\n R = 0x%08X; G = 0x%08X; B = 0x%08X; A = 0x%08X\n", *Rmask, *Gmask, *Bmask, *Amask));
	
}

static int CGX_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	int i;
	int form;

	D(bug("VideoInit... Opening libraries\n"));

	if(!IntuitionBase) {
		if( !(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",39L))) {
			SDL_SetError("Couldn't open intuition V39+");
			return -1;
		}
	}

	if(!GfxBase) {
		if( !(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",39L))) {
			SDL_SetError("Couldn't open graphics V39+");
			return -1;
		}
	}

	if(!CyberGfxBase) {
		if( !(CyberGfxBase=OpenLibrary("cybergraphics.library",40L))) {
			SDL_SetError("Couldn't open cybergraphics.");
			return(-1);
		}
	}

	D(bug("Library intialized, locking screen...\n"));

	SDL_Display = LockPubScreen(NULL);

	if ( SDL_Display == NULL ) {
		D(bug("Cannot lock display...\n"));
		SDL_SetError("Couldn't lock the display");
		return(-1);
	}
	this->info.current_w = SDL_Display->Width;
	this->info.current_h = SDL_Display->Height;

	D(bug("Checking if we are using a CGX native display...\n"));
	if(!IsCyberModeID(GetVPModeID(&SDL_Display->ViewPort)))
	{
		Uint32 okid=BestCModeIDTags(CYBRBIDTG_NominalWidth,	SDL_Display->Width,
									CYBRBIDTG_NominalHeight, SDL_Display->Height,
									CYBRBIDTG_Depth, 8,
									TAG_DONE);

		D(bug("Default visual is not CGX native!\n"));

		UnlockPubScreen(NULL,SDL_Display);

		GFX_Display=NULL;

		if(okid!=INVALID_ID)
		{
			GFX_Display=OpenScreenTags(	NULL,
										SA_Width,SDL_Display->Width,
										SA_Height,SDL_Display->Height,
										SA_Depth,8,
										SA_Quiet,TRUE,
										SA_ShowTitle,FALSE,
										SA_DisplayID,okid,
										TAG_DONE);
		}

		if(!GFX_Display)
		{
			SDL_SetError("Unable to open a suited CGX display");
			return -1;
		}
		else SDL_Display=GFX_Display;

	}
	else GFX_Display = SDL_Display;

	D(bug("Before GetVideoModes....\n"));
	/* Get the available video modes */
	if(CGX_GetVideoModes(this) < 0) return -1;

	/* Determine the default screen depth: Use the default visual (or at least one with the same depth) */
	for(i = 0; i < this->hidden->nvisuals; i++)
	    if(this->hidden->visuals[i].depth == GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_DEPTH)) break;

	/* default visual was useless, take the deepest one instead */
	if(i == this->hidden->nvisuals) i = 0;

	D(bug("Init: Setting screen depth to: %ld\n",this->hidden->visuals[i].depth));
	
	SDL_Visual 				= this->hidden->visuals[i].visual;
	this->hidden->depth 	= this->hidden->visuals[i].depth;
	vformat->BitsPerPixel 	= this->hidden->visuals[i].depth;
	vformat->BytesPerPixel 	= this->hidden->visuals[i].bpp / 8;

	// In this case I use makebitmask in a way that I'm sure I'll get PIXFMT pixel mask
	form = GetCyberIDAttr(CYBRIDATTR_PIXFMT,SDL_Visual);
	if ( vformat->BitsPerPixel > 8 )
	{
		MakeBitMask(this,form,&this->hidden->depth, &vformat->Rmask, &vformat->Gmask, &vformat->Bmask, &vformat->Amask);
		vformat->Amask = 0;
	}

	/* Create the blank cursor */
	SDL_BlankCursor = AllocMem(16,MEMF_CHIP|MEMF_CLEAR);

	/* Fill in some window manager capabilities */
	this->info.wm_available = 1;
	this->info.blit_fill    = 1;
#ifndef NO_AMIGAHWSURF
	this->info.hw_available = 1;
	this->info.blit_hw      = 1;
	this->info.blit_hw_CC   = 1;
	this->info.blit_hw_A    = 1;
	this->info.blit_sw      = 1;
	this->info.blit_sw_CC   = 0;
	this->info.blit_sw_A    = 1;
#endif
	this->info.video_mem    = AvailMem(MEMF_FAST) / 4096000 * 1000; /* how to get Video Memory size? */
	
	SDL_RastPort = &SDL_Display->RastPort;
	
	/* We're done! */
	D(bug("End of CGX_VideoInit\n"));

	return(0);
}

void CGX_DestroyWindow(_THIS, SDL_Surface *screen)
{
	D(bug("Destroy Window...\n"));

	/* Hide the managed window */
	int was_fullscreen = 0;
	this->hidden->window_active = 0;

	/* Clean up OpenGL */
	if ( screen ) screen->flags &= ~(SDL_OPENGL|SDL_OPENGLBLIT);

	if ( screen && (screen->flags & SDL_FULLSCREEN) )
	{
		was_fullscreen = 1;
		screen->flags &= ~SDL_FULLSCREEN;
	}

	/* Free the colormap entries */
	if ( SDL_Pens )
	{
		if (	this->screen
			&& 	this->hidden
			&&	GFX_Display
			&&	this->screen->format
			&&	this->hidden->depth==8
			&&	!was_fullscreen)
		{
			int numcolors = 1 << this->screen->format->BitsPerPixel;
			unsigned long pen;

			if(numcolors>256) numcolors=256;

			D(bug("Releasing %d pens...\n", numcolors));
			for ( pen=0; pen<numcolors; pen++ )
				if(SDL_Pens[pen]>=0) ReleasePen(GFX_Display->ViewPort.ColorMap,SDL_Pens[pen]);
		}
		SDL_free(SDL_Pens);
		SDL_Pens = NULL;
	}

	/* Destroy the output window */
	if ( SDL_Window )
	{
#ifndef NO_AMIGAHWSURF
		if (this->hidden->temprp) FreeRastPort(this->hidden->temprp);
		this->hidden->temprp = NULL;
		if (this->hidden->BlitBitMap) FreeBitMap(this->hidden->BlitBitMap);
		this->hidden->BlitBitMap = NULL;
#endif
		CloseWindow(SDL_Window);
		SDL_Window=NULL;
	}
}

static void CGX_SetSizeHints(_THIS, int w, int h, Uint32 flags)
{
	if ( flags & SDL_RESIZABLE )
		WindowLimits(SDL_Window, 32, 32,4096,4096);
	else
		WindowLimits(SDL_Window, w,h,w,h);

	if ( flags & SDL_FULLSCREEN ) 
		flags &= ~SDL_RESIZABLE;
	else if ( SDL_getenv("SDL_VIDEO_CENTERED") )
		ChangeWindowBox(SDL_Window,
						(SDL_Display->Width - w - SDL_Window->BorderLeft-SDL_Window->BorderRight)/2,
						(SDL_Display->Height - h - SDL_Window->BorderTop-SDL_Window->BorderBottom)/2,
						w+SDL_Window->BorderLeft+SDL_Window->BorderRight,
						h+SDL_Window->BorderTop+SDL_Window->BorderBottom);
}

int CGX_CreateWindow(	_THIS, SDL_Surface *screen,
						int w, int h, int bpp, Uint32 flags)
{
	Uint32 form, rb, gb, bb, ab;

	D(bug("CGX_CreateWindow %dx%d/%d flags:%lx\n", w, h, bpp, flags));

	/* If a window is already present, destroy it and start fresh */
	if ( SDL_Window ) CGX_DestroyWindow(this, screen);

	SDL_Window = 0;

	/* Allocate the new pixel format for this video mode */
	form = GetCyberIDAttr(CYBRIDATTR_PIXFMT,SDL_Visual);

	if((flags & SDL_HWSURFACE) && (bpp!=this->hidden->depth))
	{
			bpp = this->hidden->depth;
			D(bug("Accel forces bpp to be equal (%ld)\n",bpp));
	}

	/* 
		With this call if needed I'll revert the wanted bpp to a bpp best suited for the display, actually occurs
		only with requested format 15/16bit and display format != 15/16bit
	 */
	D(bug("BEFORE screen allocation: bpp:%ld (real:%ld)\n",bpp,this->hidden->depth));
 
	MakeBitMask(this,form,&bpp, &rb, &gb, &bb, &ab);

	if ( ! SDL_ReallocFormat(screen, 8 * GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_BPPIX), rb, gb, bb, 0) )
		return -1;

	D(bug("AFTER screen allocation: bpp:%ld (real:%ld)\n",bpp,this->hidden->depth));

	/* Create the appropriate colormap */
	if ( GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_PIXFMT)==PIXFMT_LUT8 || bpp==8 )
	{
	    int ncolors,i;
	    D(bug("Pens palette allocation...\n"));

	    /* Allocate the pixel flags */
	    if(bpp>=8)
			ncolors=256;
	    else
			ncolors = 1 << screen->format->BitsPerPixel;

	    SDL_Pens = (Sint32 *)SDL_malloc(ncolors * sizeof(Sint32));

	    if(SDL_Pens == NULL)
		{
			SDL_OutOfMemory();
			return -1;
	    }

	    for(i=0;i<ncolors;i++) SDL_Pens[i]=-1;

	    /* always allocate a private colormap on non-default visuals */
	    if(bpp==8) flags |= SDL_HWPALETTE;

	    if ( flags & SDL_HWPALETTE ) screen->flags |= SDL_HWPALETTE;
	} 

	if( flags & SDL_FULLSCREEN )
	{
		SDL_Window = OpenWindowTags(NULL,
									WA_Width,SDL_Display->Width,
									WA_Height,SDL_Display->Height,
									WA_Flags,WFLG_ACTIVATE|WFLG_RMBTRAP|WFLG_BORDERLESS|WFLG_BACKDROP|WFLG_REPORTMOUSE,
									WA_IDCMP,IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE,
									WA_BackFill, (ULONG)LAYERS_NOBACKFILL,
									WA_CustomScreen,(ULONG)SDL_Display,
									TAG_DONE);
		FillPixelArray(SDL_Window->RPort,SDL_Window->BorderLeft,SDL_Window->BorderTop,SDL_Display->Width,SDL_Display->Height,0x00000000);
		D(bug("Opening backdrop window %ldx%ld on display %lx!\n",SDL_Display->Width,SDL_Display->Height,SDL_Display));
	}
	else
	{
		ULONG left, top;
		
		left = (SDL_Display->Width - w) / 2;
		top  = (SDL_Display->Height - h) / 2;
		
		SDL_Window = OpenWindowTags(NULL,
									WA_InnerWidth,w,
									WA_InnerHeight,h,
									WA_Left, left,
									WA_Top, top,
									WA_Flags,	WFLG_REPORTMOUSE|WFLG_ACTIVATE|WFLG_RMBTRAP |
												((flags&SDL_NOFRAME) ? 0 : (WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_DRAGBAR | 
												((flags&SDL_RESIZABLE) ? WFLG_SIZEGADGET|WFLG_SIZEBBOTTOM : 0))),
									WA_IDCMP,	IDCMP_RAWKEY|IDCMP_CLOSEWINDOW|IDCMP_MOUSEBUTTONS|IDCMP_NEWSIZE|
												IDCMP_MOUSEMOVE|IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW,
									WA_BackFill, (ULONG)LAYERS_NOBACKFILL,
									WA_PubScreen,(ULONG)SDL_Display,
									WA_GimmeZeroZero, ( flags & SDL_OPENGL )?TRUE:FALSE,
									TAG_DONE);
		FillPixelArray(SDL_Window->RPort,SDL_Window->BorderLeft,SDL_Window->BorderTop,w,h,0x00000000);
		D(bug("Opening WB window of size: %ldx%ld!\n",w,h));
	}

	if(!SDL_Window) return -1;
	
	this->hidden->window_active = 1;

#ifndef NO_AMIGAHWSURF
	if (	(flags & SDL_DOUBLEBUF)
		&&	(flags & SDL_FULLSCREEN)
		&&	(this->hidden->dbuffer) )
	{
		D(bug("Double Buffering in use\n",w,h));
		if(SDL_RastPort = CreateRastPort())
		{
			SDL_RastPort->BitMap=this->hidden->SB[1]->sb_BitMap;
			screen->flags |= SDL_DOUBLEBUF;
		}
		else
			return -1;
	}
	else
	{
		// If there is no double buffering we create a Bitmap to fake double buffering.
		D(bug("No Double Buffering : Using BlitBitMap hack\n",w,h));
		this->hidden->BlitBitMap = AllocBitMap(	w, 
												h,
												GetBitMapAttr(SDL_Window->RPort->BitMap, BMA_DEPTH),
												flags & SDL_FULLSCREEN ? BMF_CLEAR| BMF_MINPLANES : BMF_CLEAR | BMF_MINPLANES | BMF_DISPLAYABLE,
												SDL_Window->RPort->BitMap);
												
		if (!this->hidden->BlitBitMap) return -1;
		
		screen->flags &= ~SDL_DOUBLEBUF;
		SDL_RastPort=SDL_Window->RPort;
	}
#else
	screen->flags &= ~SDL_DOUBLEBUF;
	SDL_RastPort=SDL_Window->RPort;
#endif
	
	if (flags & SDL_HWSURFACE)
		screen->flags |= SDL_HWSURFACE;
	else
		screen->flags &= ~SDL_HWSURFACE;

	CGX_SetSizeHints(this, w, h, flags);
	this->info.current_w = w;
	this->info.current_h = h;
	
	if ( flags & SDL_FULLSCREEN )
	{
		screen->flags |= SDL_FULLSCREEN;
		currently_fullscreen=1;
	}
	else
		screen->flags &= ~SDL_FULLSCREEN;

	screen->w = w;
	screen->h = h;
	screen->pitch = SDL_CalculatePitch(screen);
	CGX_ResizeImage(this, screen, flags);

	/* Make OpenGL Context if needed*/
	if(flags & SDL_OPENGL)
	{
		if(this->gl_data->gl_active == 0)
		{
			if(CGX_GL_Init(this) < 0)
				return -1;
			else
				screen->flags |= SDL_OPENGL;
		}
		else
		{
			if(CGX_GL_Update(this) < 0)
				return -1;
			else
				screen->flags |= SDL_OPENGL;
		}
	}
    
    return 0;
}

int CGX_ResizeWindow(_THIS, SDL_Surface *screen, int width, int height, Uint32 flags)
{
	D(bug("CGX_ResizeWindow\n"));

	/* Resize the window manager window */
	CGX_SetSizeHints(this, width, height, flags);
	this->info.current_w = width;
	this->info.current_h = height;

	ChangeWindowBox(	SDL_Window,
						SDL_Window->LeftEdge,
						SDL_Window->TopEdge,
						width+SDL_Window->BorderLeft+SDL_Window->BorderRight,
						height+SDL_Window->BorderTop+SDL_Window->BorderBottom);
	screen->w = width;
	screen->h = height;
	screen->pitch = SDL_CalculatePitch(screen);
#ifndef NO_AMIGAHWSURF
	if (this->hidden->BlitBitMap)
	{
		FreeBitMap(this->hidden->BlitBitMap);
		this->hidden->BlitBitMap = AllocBitMap(	width, 
												height,
												GetBitMapAttr(SDL_Window->RPort->BitMap, BMA_DEPTH),
												flags & SDL_FULLSCREEN ? BMF_CLEAR | BMF_MINPLANES : BMF_CLEAR |BMF_MINPLANES | BMF_DISPLAYABLE,
												SDL_Window->RPort->BitMap);
	}
#endif
	CGX_ResizeImage(this, screen, flags);

	return 0;
}

static SDL_Surface *CGX_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags)
{
	Uint32 saved_flags;
	int needcreate=0;

	D(bug("CGX_SetVideoMode current:%lx, bpp:%ld, flags:%lx\n",current, bpp, flags));

	/* Lock the event thread, in multi-threading environments */
	SDL_Lock_EventThread();

	// Check if we need to close an already existing videomode...
	if(   current 
	   && current->flags&SDL_FULLSCREEN 
	   && !(flags&SDL_FULLSCREEN))
	{
		unsigned long i;
		D(bug("Discovered we need to re-create window, flags:%lx, current:%lx\n", flags, current->flags));

		needcreate=1;

		D(bug("Destroying image, window & screen!\n"));

		CGX_DestroyImage(this,current);
		CGX_DestroyWindow(this,current);
		DestroyScreen(this);
		GFX_Display=SDL_Display=LockPubScreen(NULL);

		bpp=this->hidden->depth=GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_DEPTH);

		for ( i = 0; i < this->hidden->nvisuals; i++ )
		{
			if ( this->hidden->visuals[i].depth == bpp ) /* era .depth */
				break;
		}
		if ( i == this->hidden->nvisuals )
		{
			SDL_SetError("No matching visual for requested depth");
			return NULL;	/* should never happen */
		}
		SDL_Visual = this->hidden->visuals[i].visual;

		D(bug("Setting screen depth to: %ld\n",this->hidden->depth));
	}
	
	/* Check the combination of flags we were passed */
	if ( flags & SDL_FULLSCREEN )
	{
		int i;
		int buildnewscreen = 1;
		
		needcreate=1;
		
		/* Clear fullscreen flag if not supported */
		if (current && current->flags&SDL_FULLSCREEN ) 
		{
			if (current->w != width || current->h != height || (this->hidden && this->hidden->depth!=bpp))
			{
				D(bug("Deleting previous window...\n"));
				CGX_DestroyImage(this,current);
				CGX_DestroyWindow(this,current);
				DestroyScreen(this);
			}
			else
				buildnewscreen = 0;
		}

		if (buildnewscreen)
		{
			Uint32 okid;

			this->hidden->dbuffer = 0;
			GFX_Display=NULL;			
			okid = BestCModeIDTags(CYBRBIDTG_NominalWidth, width,
										CYBRBIDTG_NominalHeight, height,
										CYBRBIDTG_Depth, bpp,
										TAG_DONE);
										
			if ((okid == INVALID_ID) && (bpp == 32))
			{
				D(bug("Failed to open a 32bits screen trying 24bits instead\n"));
				bpp = 24;
				okid = BestCModeIDTags(CYBRBIDTG_NominalWidth, width,
											CYBRBIDTG_NominalHeight, height,
											CYBRBIDTG_Depth, bpp,
											TAG_DONE);				
			}

			D(bug("Opening screen %dx%d/%d (id:%lx)...\n", width, height, bpp, okid));

			if(okid!=INVALID_ID)
				GFX_Display=OpenScreenTags(	NULL,
											SA_Width,width,
											SA_Height,height,
											SA_Quiet,TRUE,
											SA_ShowTitle,FALSE,
											SA_Depth,bpp,
											SA_DisplayID,okid,
											TAG_DONE);

			if (!GFX_Display)
			{
				D(bug("OpenScreenTags failed!\n"));
				GFX_Display=SDL_Display;
				flags &= ~SDL_FULLSCREEN;
				flags &= ~SDL_DOUBLEBUF;
			}
			else
			{
				SDL_Display=GFX_Display;
	
				D(bug("Screen opened: %d x %d.\n", GFX_Display->Width, GFX_Display->Height));

				if(flags&SDL_DOUBLEBUF)
				{
#ifndef NO_AMIGAHWSURF
					APTR lock = NULL;
					/* check if surface can be locked, if not => Surface is SW and there is no double buffering!*/
					lock = LockBitMapTags(	SDL_Display->RastPort.BitMap,
											TAG_DONE);
											
					if (lock)
					{
						UnLockBitMap(lock);

						D(bug("Start of DBuffering allocations...\n"));
						
						this->hidden->safeport=CreateMsgPort();
						this->hidden->dispport=CreateMsgPort();
								
						this->hidden->SB[0]=AllocScreenBuffer(SDL_Display,NULL,SB_SCREEN_BITMAP);
						this->hidden->SB[1]=AllocScreenBuffer(SDL_Display,NULL,SB_COPY_BITMAP);
						
						if (	this->hidden->safeport
							&&	this->hidden->dispport
							&&	this->hidden->SB[0]
							&&	this->hidden->SB[1])
						{
							D(bug("Screen buffers Allocated...\n"));
							
							this->hidden->safe_sigbit=1L<< this->hidden->safeport->mp_SigBit;
							this->hidden->disp_sigbit=1L<< this->hidden->dispport->mp_SigBit;

							this->hidden->SB[0]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort=this->hidden->safeport;
							this->hidden->SB[0]->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort=this->hidden->dispport;
							this->hidden->SB[1]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort=this->hidden->safeport;
							this->hidden->SB[1]->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort=this->hidden->dispport;
							
							this->hidden->dbuffer = 1;
							if (current) current->flags |= SDL_DOUBLEBUF;
							D(bug("Dbuffering enabled!\n"));
						}
						else
						{
							if (this->hidden->SB[0]) FreeScreenBuffer(SDL_Display,this->hidden->SB[0]);
							if (this->hidden->SB[1]) FreeScreenBuffer(SDL_Display,this->hidden->SB[1]);
							if (this->hidden->safeport) DeleteMsgPort (this->hidden->safeport);
							if (this->hidden->dispport) DeleteMsgPort (this->hidden->dispport);
							this->hidden->SB[0] = NULL;
							this->hidden->SB[1] = NULL;
							this->hidden->safeport = NULL;
							this->hidden->dispport = NULL;
							
							flags &= ~SDL_DOUBLEBUF;
							this->hidden->dbuffer = 0;
							D(bug("Dbuffering failed!\n"));
						}
					}
					else
					{
						flags &= ~SDL_DOUBLEBUF;
						this->hidden->dbuffer = 0;
						D(bug("Dbuffering failed!\n"));
					}
#else
					flags &= ~SDL_DOUBLEBUF;
#endif
				}
			}

			D(bug("Screen bitmap: %ld (%ld), bpp %ld\n",
					GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_BPPIX),
					GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_DEPTH),                        
					bpp));
		}

		bpp=this->hidden->depth = GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_DEPTH);
		D(bug("Setting screen depth to: %ld\n",this->hidden->depth));

		for ( i = 0; i < this->hidden->nvisuals; i++ )
			if ( this->hidden->visuals[i].depth == bpp ) /* era .depth */
				break;

		if ( i == this->hidden->nvisuals )
		{
			SDL_SetError("No matching visual for requested depth");
			return NULL;	/* should never happen */
		}
		SDL_Visual = this->hidden->visuals[i].visual;
	}

	/* Set up the window */
	saved_flags = current->flags;

	if (SDL_Window 
		&& ((saved_flags&SDL_OPENGL) == (flags&SDL_OPENGL))
	    && !needcreate)
	{
		if (CGX_ResizeWindow(this, current, width, height, flags) < 0)
			current = NULL;
		else
			current->flags |= (flags&SDL_RESIZABLE); // Resizable only if the user asked it
	}
	else
	{
		D(if (SDL_Window) bug("bpp: %d (current %d)\n",bpp, current->format->BitsPerPixel));
		
		if (CGX_CreateWindow(this,current,width,height,bpp,flags) < 0)
			current = NULL;
		else
			current->flags |= (flags&SDL_RESIZABLE); // Resizable only if the user asked it
	}

	/* Release the event thread */
	SDL_Unlock_EventThread();

	/* We're done! */
	return(current);
}

static int CGX_ToggleFullScreen(_THIS, int on)
{
	Uint32 event_thread;

	/* Don't lock if we are the event thread */
	event_thread = SDL_EventThreadID();
	if (event_thread && (SDL_ThreadID() == event_thread)) event_thread = 0;
	if (event_thread) SDL_Lock_EventThread();

	if (on)
	{
		this->screen->flags |= SDL_FULLSCREEN;
		CGX_EnterFullScreen(this);
	} 
	else
	{
		this->screen->flags &= ~SDL_FULLSCREEN;
		CGX_LeaveFullScreen(this);
	}

	CGX_RefreshDisplay(this);
	
	if (event_thread) SDL_Unlock_EventThread();

	SDL_ResetKeyboard();
	return(1);
}

/* Update the current mouse state and position */
static void CGX_UpdateMouse(_THIS)
{
	/* Lock the event thread, in multi-threading environments */
	SDL_Lock_EventThread();

	if(currently_fullscreen)
	{
			SDL_PrivateAppActive(1, SDL_APPMOUSEFOCUS);
			SDL_PrivateMouseMotion(0, 0, SDL_Display->MouseX, SDL_Display->MouseY);
	}
	else
	{
		if(	SDL_Display->MouseX>=(SDL_Window->LeftEdge+SDL_Window->BorderLeft) && SDL_Display->MouseX<(SDL_Window->LeftEdge+SDL_Window->Width-SDL_Window->BorderRight) &&
			SDL_Display->MouseY>=(SDL_Window->TopEdge+SDL_Window->BorderLeft) && SDL_Display->MouseY<(SDL_Window->TopEdge+SDL_Window->Height-SDL_Window->BorderBottom)
			)
		{
			SDL_PrivateAppActive(1, SDL_APPMOUSEFOCUS);
			SDL_PrivateMouseMotion(0, 0, SDL_Display->MouseX-SDL_Window->LeftEdge-SDL_Window->BorderLeft,
										SDL_Display->MouseY-SDL_Window->TopEdge-SDL_Window->BorderTop);
		}
		else
		{
			SDL_PrivateAppActive(0, SDL_APPMOUSEFOCUS);
		}
	}
	SDL_Unlock_EventThread();
}

static int CGX_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	int      i;

	if ( (this->screen->flags & SDL_HWPALETTE) && currently_fullscreen )
	{
		ULONG  cmap[256*3+2];

		cmap[0]=(ncolors<<16);
		cmap[0]+=firstcolor;

		for ( i=0; i<ncolors; i++ )
		{
			cmap[i*3+1] = colors[i].r<<24;
			cmap[i*3+2] = colors[i].g<<24;
			cmap[i*3+3] = colors[i].b<<24;
		}
		cmap[ncolors*3+1]=0;
		LoadRGB32(&GFX_Display->ViewPort,cmap);
	} 
	else
	{
		// Pens are not needed on 8bit screen with hwpalette
		unsigned long pen;

		if ( SDL_Pens == NULL ) {
			D(bug("SetColors without colormap!"));
			return(0);
		}

		if(this->hidden->depth==8)
		{
			for ( pen=firstcolor; pen<ncolors+firstcolor; pen++ )
			{
				if(SDL_Pens[pen]>=0)
					ReleasePen(GFX_Display->ViewPort.ColorMap,SDL_Pens[pen]);
			}

			/* Try to allocate all the colors */
			for ( i=0; i<ncolors; i++ )
			{
				SDL_Pens[i+firstcolor]=ObtainBestPenA(GFX_Display->ViewPort.ColorMap,colors[i].r<<24,colors[i].g<<24,colors[i].b<<24,NULL);
			}
		}
		else
		{
			for(i=0;i<ncolors;i++)
				SDL_Pens[i+firstcolor]=(colors[i].r<<16)+(colors[i].g<<8)+colors[i].b;
		}
	}
	return 1;
}

/* 
	Note:  If we are terminated, this could be called in the middle of
	another SDL video routine -- notably UpdateRects.
*/
static void CGX_VideoQuit(_THIS)
{
	/* Shutdown everything that's still up */
	/* The event thread should be done, so we can touch SDL_Display */
	D(bug("CGX_VideoQuit\n"));

	if ( SDL_Display != NULL )
	{
		/* Clean up OpenGL */
		if(this->gl_data->gl_active == 1)
		{
			CGX_GL_Quit(this);
		}
		/* Start shutting down the windows */
		D(bug("Destroying image...\n"));
		CGX_DestroyImage(this, this->screen);
		D(bug("Destroying window...\n"));
		CGX_DestroyWindow(this, this->screen);
		/* Otherwise SDL_VideoQuit will try to free it! */
		SDL_VideoSurface=NULL;
		CGX_FreeVideoModes(this);

		/* Free that blank cursor */
		if ( SDL_BlankCursor != NULL )
		{
			FreeMem(SDL_BlankCursor,16);
			SDL_BlankCursor = NULL;
		}

		D(bug("Destroying screen...\n"));

		if ( GFX_Display != NULL )
			DestroyScreen(this);

		SDL_Display = NULL;
	}

	D(bug("Closing libraries...\n"));

	if( CyberGfxBase)
	{
		CloseLibrary(CyberGfxBase);
		CyberGfxBase=NULL;
	}

	if (IntuitionBase)
	{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase=NULL;
	}
	if (GfxBase)
	{
		CloseLibrary((struct Library *)GfxBase);
		GfxBase=NULL;
	}

	if (SDL_ConReq)
	{
		DeleteIORequest(SDL_ConReq);
		SDL_ConReq = NULL;
	}
	
	if (SDL_ConPort)
	{
		DeleteMsgPort(SDL_ConPort);
		SDL_ConPort = NULL;
	}

	if ( this->screen && (this->screen->flags & SDL_HWSURFACE) )
	{
		/* Direct screen access, no memory buffer */
		this->screen->pixels = NULL;
	}
	D(bug("End of CGX_VideoQuit.\n"));

}

int CGX_ResizeFullScreen(_THIS)
{
    int x, y;
    int real_w, real_h;

    if ( currently_fullscreen )
	{
		/* Not supported yet! */
		D(bug("Resize Full Screen not supported yet\n"));
    }
    return(1);
}

static void set_best_resolution(_THIS, int width, int height)
{
	Uint32 idok;
	int depth=8;

	if(SDL_Display)	depth=GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_DEPTH);

	idok=BestCModeIDTags(	CYBRBIDTG_NominalWidth,width,
							CYBRBIDTG_NominalHeight,height,
							CYBRBIDTG_Depth,depth,
							TAG_DONE);

	if(idok!=INVALID_ID)
	{
		if(SDL_Display)
		{
			if(currently_fullscreen)
				CloseScreen(SDL_Display);
			else
				UnlockPubScreen(NULL,SDL_Display);
		}
		SDL_Display=GFX_Display=OpenScreenTags(	NULL,
												SA_Width,width,
												SA_Height,height,
												SA_Depth,depth,
												SA_DisplayID,idok,
												SA_ShowTitle,FALSE,
												TAG_DONE);
	}
}


int CGX_EnterFullScreen(_THIS)
{
    int okay;
	Uint32 saved_flags;

	D(bug("CGX_EnterFullScreen\n"));
	
    okay = 1;
    saved_flags = this->screen->flags;

    if ( ! currently_fullscreen )
	{
        int real_w, real_h;

        /* Map the fullscreen window to blank the screen */
		real_w = SDL_Window->Width-SDL_Window->BorderLeft-SDL_Window->BorderRight;
		real_h = SDL_Window->Height-SDL_Window->BorderBottom-SDL_Window->BorderTop;

		CGX_DestroyWindow(this,this->screen);
		set_best_resolution(this, real_w,real_h);

		currently_fullscreen = 1;
		this->screen->flags = saved_flags;

		CGX_CreateWindow(this,this->screen,real_w,real_h,GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_DEPTH),this->screen->flags);

        /* Set the new resolution */
        okay = CGX_ResizeFullScreen(this);
        if ( ! okay ) 
		{
            CGX_LeaveFullScreen(this);
        }
    }
    return(okay);
}

int CGX_LeaveFullScreen(_THIS)
{
	D(bug("CGX_LeaveFullScreen\n"));

    if ( currently_fullscreen )
	{
		int width,height;
		if ( SDL_Window )
		{
			CloseWindow(SDL_Window);
			SDL_Window=NULL;
		}
		CloseScreen(SDL_Display);

		GFX_Display=SDL_Display=LockPubScreen(NULL);

		currently_fullscreen = 0;

		CGX_CreateWindow(this,this->screen,this->screen->w,this->screen->h,GetCyberMapAttr(SDL_Display->RastPort.BitMap,CYBRMATTR_DEPTH),this->screen->flags);
		CGX_ResizeImage(this,this->screen,0L);
    }

    return(0);
}


/* Gamma correction functions (Not supported) */
int CGX_SetGamma(_THIS, float red, float green, float blue)
{
    SDL_SetError("Gamma correction not supported");
    return -1;
}

int CGX_GetGamma(_THIS, float *red, float *green, float *blue)
{
    SDL_SetError("Gamma correction not supported");
    return -1;
}

int CGX_SetGammaRamp(_THIS, Uint16 *ramp)
{
    SDL_SetError("Gamma correction not supported");
    return(-1);
}
