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

#include "SDL_endian.h"
#include "SDL_cgximage_c.h"
#include "SDL_cgxaccel_c.h"
#include "../../main/aros/mydebug.h"

/* Various screen update functions available */
static void CGX_SWScreenUpdate(_THIS, int numrects, SDL_Rect *rects);
static void CGX_HWScreenUpdate(_THIS, int numrects, SDL_Rect *rects);

ULONG CGX_MaskToRECTFMT(ULONG A, ULONG R, ULONG G, ULONG B)
{
	ULONG GlobMask = AROS_BE2LONG((A & 0x11111111) | (R & 0x22222222) | (G & 0x44444444) | (B & 0x88888888));
	ULONG rectfmt = 0;
	
	switch (GlobMask)
	{
		case 0x22448811: rectfmt = RECTFMT_RGBA; 	break;
		case 0x11224488: rectfmt = RECTFMT_ARGB; 	break;
		case 0x00000000: rectfmt = RECTFMT_LUT8; 	break;
		case 0x00002048: rectfmt = RECTFMT_RGB15; 	break;
		case 0x00000842: rectfmt = RECTFMT_BGR15; 	break;
		case 0x00004820: rectfmt = RECTFMT_RGB15PC; break;
		case 0x00004208: rectfmt = RECTFMT_BGR15PC; break;
		case 0x00002448: rectfmt = RECTFMT_RGB16; 	break;
		case 0x00008C42: rectfmt = RECTFMT_BGR16; 	break;
		case 0x00004824: rectfmt = RECTFMT_RGB16PC; break;
		case 0x0000428C: rectfmt = RECTFMT_BGR16PC; break;
		case 0x88442211: rectfmt = RECTFMT_BGRA32; 	break;
		case 0x11884422: rectfmt = RECTFMT_ABGR32; 	break;
		case 0x00224488: rectfmt = RECTFMT_0RGB32; 	break;
		case 0x88442200: rectfmt = RECTFMT_BGR032; 	break;
		case 0x22448800: rectfmt = RECTFMT_RGB032; 	break;
		case 0x00884422: rectfmt = RECTFMT_0BGR32; 	break;
		default:         rectfmt = RECTFMT_RAW; 	break;
//		case 0x00224488: rectfmt = RECTFMT_RGB; 	break;
//		case 0x00884422: rectfmt = RECTFMT_BGR24; 	break;
	}
	
	return(rectfmt);
}

int CGX_SetupImage(_THIS, SDL_Surface *screen)
{
    SDL_Image = NULL;

	D(bug("CGX_SetupImage\n"));
	D(bug("BitsPerPixel: %d\n",screen->format->BitsPerPixel));
	D(bug("BytesPerPixel: %d\n",screen->format->BytesPerPixel));
	D(bug("Rmask: 0x%08X\n",screen->format->Rmask));
	D(bug("Gmask: 0x%08X\n",screen->format->Gmask));
	D(bug("Bmask: 0x%08X\n",screen->format->Bmask));
	D(bug("Amask: 0x%08X\n",screen->format->Amask));	
	
    if(screen->flags & SDL_HWSURFACE)
	{
#ifndef NO_AMIGAHWSURF
        ULONG pitch;
        APTR pixels;
        APTR lock;

		if (this->hidden->dbuffer)
			lock = LockBitMapTags(
					SDL_RastPort->BitMap,
					LBMI_BASEADDRESS, (IPTR)&pixels,
					LBMI_BYTESPERROW, (IPTR)&pitch,
					TAG_DONE);
		else
			lock = LockBitMapTags(
					this->hidden->BlitBitMap,
					LBMI_BASEADDRESS, (IPTR)&pixels,
					LBMI_BYTESPERROW, (IPTR)&pitch,
					TAG_DONE);

        if (lock)
		{
            UnLockBitMap(lock);		
            if (!screen->hwdata)
                screen->hwdata = SDL_malloc(sizeof(struct private_hwdata));

            if (screen->hwdata)
			{
                screen->hwdata->lock      	= NULL;
                screen->hwdata->allocated 	= 0;
                screen->hwdata->mask      	= NULL;
                screen->hwdata->bmap      	= this->hidden->dbuffer ? SDL_RastPort->BitMap : this->hidden->BlitBitMap;
				screen->hwdata->update 		= 0;
                screen->hwdata->videodata 	= this;			
                screen->pixels 				= pixels; 
                screen->pitch  				= GetCyberMapAttr(screen->hwdata->bmap, CYBRMATTR_XMOD);;
				screen->hwdata->pixfmt		= GetCyberMapAttr(screen->hwdata->bmap, CYBRMATTR_PIXFMT);
				
				this->UpdateRects			= CGX_HWScreenUpdate;
				
                D(bug("HW video image configured (%lx, pitch %ld, pixfmt 0x%08X).\n", screen->pixels, screen->pitch, screen->hwdata->pixfmt));
				D(bug("Bitmap : %s\n",this->hidden->dbuffer ?"RastPort->BitMap":"BlitBitMap"));
				
                return 0;
            }
            D(bug("Creating system accel struct FAILED.\n"));
        }
#endif
        D(bug("HW surface not available, falling back to SW surface.\n"));

        screen->flags &= ~SDL_HWSURFACE;
	}

    screen->hwdata=NULL;

    D(bug("h = %d, pitch = %d\n", screen->h, screen->pitch));

    screen->pixels = SDL_malloc(screen->h*screen->pitch);

    if ( screen->pixels == NULL )
    {
        SDL_OutOfMemory();
        return -1;
    }

    SDL_Image = screen->pixels;

    if ( SDL_Image == NULL )
    {
        SDL_SetError("Couldn't create XImage");
        return -1;
    }

    this->UpdateRects = CGX_SWScreenUpdate;

    return 0;
}

void CGX_DestroyImage(_THIS, SDL_Surface *screen)
{
	if ( SDL_Image ) 
	{
		SDL_free(SDL_Image);
		SDL_Image = NULL;
	}
	if ( screen ) 
	{
		screen->pixels = NULL;

		if(screen->hwdata) 
		{
			SDL_free(screen->hwdata);
			screen->hwdata=NULL;
		}
	}
}

int CGX_ResizeImage(_THIS, SDL_Surface *screen, Uint32 flags)
{
	int retval;

	D(bug("CGX_ResizeImage(%lx,%lx)\n",this,screen));

	CGX_DestroyImage(this, screen);

	if ( flags & SDL_OPENGL ) 
        retval = 0;			/* No image when using GL */
	else
		retval = CGX_SetupImage(this, screen);

	return(retval);
}

static void CGX_HWScreenUpdate(_THIS, int numrects, SDL_Rect *rects)
{
	D(bug("CGX_HWScreenUpdate\n"));
	CGX_FlipHWSurface(this, this->screen);
}

static void CGX_SWScreenUpdate(_THIS, int numrects, SDL_Rect *rects)
{
	int i,format, dx = 0, dy = 0;

	D(bug("CGX_SWScreenUpdate:\t"));
	
	format=CGX_MaskToRECTFMT(	this->screen->format->Amask,
								this->screen->format->Rmask,
								this->screen->format->Gmask,
								this->screen->format->Bmask);
	D(bug("format = 0x%08X\n", format));
	
	if (currently_fullscreen)
	{
		// center display on screen
		dx = (SDL_Display->Width - this->screen->w)/2;
		dy = (SDL_Display->Height - this->screen->h)/2;
	}
	
	if (format==RECTFMT_LUT8)
	{
		for ( i=0; i<numrects; ++i ) {
			if ( ! rects[i].w ) { /* Clipped? */
				continue;
			}
			WriteLUTPixelArray(	this->screen->pixels,
								rects[i].x, 
								rects[i].y,
								this->screen->pitch,
								SDL_RastPort,
								SDL_Pens,
								SDL_Window->BorderLeft+rects[i].x + dx,
								SDL_Window->BorderTop+rects[i].y + dy,
								rects[i].w,
								rects[i].h,
								CTABFMT_XRGB8);
		}
	}
	else
	{
		for ( i=0; i<numrects; ++i ) {
			if ( ! rects[i].w ) { /* Clipped? */
				continue;
			}
			WritePixelArray(	this->screen->pixels,
								rects[i].x,
								rects[i].y,
								this->screen->pitch,
								SDL_RastPort,
								SDL_Window->BorderLeft+rects[i].x + dx,
								SDL_Window->BorderTop+rects[i].y + dy,
								rects[i].w,
								rects[i].h,
								format);
		}
	}
}

void CGX_RefreshDisplay(_THIS)
{
	SDL_Rect screenrect;
	
	/* Don't refresh a display that doesn't have an image (like GL) */
	if ( ! SDL_Image ) return;

	screenrect.x = 0;
	screenrect.y = 0;
	screenrect.w = this->screen->w;
	screenrect.h = this->screen->h;
	
	this->UpdateRects(this, 1, &screenrect);
}
