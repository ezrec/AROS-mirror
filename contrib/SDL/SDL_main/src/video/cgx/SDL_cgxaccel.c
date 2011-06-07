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
#include "SDL_cgxaccel_c.h"
#include "SDL_cgxvideo.h"
#include "../SDL_blit.h"

#include <aros/macros.h>

/* HW bliting functions */
static int CGX_HWtoHWBlit(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect);
static int CGX_HWtoHWBlitA(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect);
static int CGX_HWtoHWBlitCC(SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect);
static int CGX_SWtoHWBlit(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect);
static int CGX_SWtoHWBlitA(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect);
static int CGX_SWtoHWBlitCC(SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect);

int CGX_AllocHWSurface(_THIS, SDL_Surface *surface)
{
#ifndef NO_AMIGAHWSURF
	D(bug("Alloc HW surface...%ld x %ld x %ld!\n",surface->w,surface->h,this->hidden->depth));

	if(surface==SDL_VideoSurface)
	{
		D(bug("Allocation skipped, it's system one!\n"));
		return 0;
	}

	if(!surface->hwdata)
	{
		if(!(surface->hwdata=SDL_malloc(sizeof(struct private_hwdata))))
			return -1;
	}

	surface->hwdata->mask=NULL;
	surface->hwdata->pixarrayalpha=NULL;
	surface->hwdata->lock=NULL;
	surface->hwdata->videodata=this;
	surface->hwdata->allocated=0;

	if((surface->hwdata->bmap=AllocBitMap(surface->w,surface->h,this->hidden->depth,BMF_MINPLANES,SDL_Display->RastPort.BitMap)))
	{
		surface->hwdata->allocated=1;
		surface->hwdata->pixfmt = GetCyberMapAttr(surface->hwdata->bmap,CYBRMATTR_PIXFMT);
		surface->flags|=SDL_HWSURFACE;
		
		D(bug("PIXFMT = 0x%08X\t",surface->hwdata->pixfmt));
		D(bug("...OK\n"));
		return 0;
	}
	else
	{
		SDL_free(surface->hwdata);
		surface->hwdata=NULL;
	}
#endif
	return(-1);
}

void CGX_FreeHWSurface(_THIS, SDL_Surface *surface)
{
#ifndef NO_AMIGAHWSURF
	if(surface && surface!=SDL_VideoSurface && surface->hwdata)
	{
		D(bug("Free hw surface.\n"));

		if(surface->hwdata->mask)
			SDL_free(surface->hwdata->mask);

		if(surface->hwdata->pixarrayalpha)
			SDL_free(surface->hwdata->pixarrayalpha);			

		if(surface->hwdata->bmap&&surface->hwdata->allocated)
			FreeBitMap(surface->hwdata->bmap);

		SDL_free(surface->hwdata);
		surface->hwdata=NULL;
		surface->pixels=NULL;
		D(bug("end of free hw surface\n"));
	}
#endif
	return;
}

int CGX_LockHWSurface(_THIS, SDL_Surface *surface)
{
#ifndef NO_AMIGAHWSURF
	if (surface->hwdata)
	{
		D(bug("Locking a bitmap..."));
		if(!surface->hwdata->lock)
		{	
			Uint32 pitch;

			if((surface->hwdata->lock = LockBitMapTags(surface->hwdata->bmap,
					LBMI_BASEADDRESS,(ULONG)&surface->pixels,
					LBMI_BYTESPERROW,(ULONG)&pitch,TAG_DONE)) != NULL)
			{
				D(bug("Done...\n"));
				surface->pitch=pitch;
			}
			else
			{
				D(bug("Couldn't lock surface!\n"));
				return -1;
			}
		}
		D(else bug("Already locked!!!\n"));
	}
#endif
	return(0);
}

void CGX_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
#ifndef NO_AMIGAHWSURF
	if((surface->hwdata) && (surface->hwdata->lock))
	{
		D(bug("Unlocking a bitmap...\n"));
		UnLockBitMap(surface->hwdata->lock);
		surface->hwdata->lock = NULL;
	
		/* refresh display if pending */
		if ((surface->hwdata->bmap == this->hidden->BlitBitMap) && surface->hwdata->update)
		{
			surface->hwdata->update = 0;
			BltBitMapRastPort(	this->hidden->BlitBitMap,
								0,
								0,
								SDL_RastPort,
								SDL_Window->BorderLeft,
								SDL_Window->BorderTop,
								surface->w,
								surface->h,
								0xc0);
		}
	}
#endif
}

int CGX_FlipHWSurface(_THIS, SDL_Surface *surface)
{
	static int current=0;
	static BOOL SafeDisp=TRUE;
	static BOOL SafeChange=TRUE;
	
	if(this->hidden->dbuffer)
	{
		D(bug("Double Buffering Flip to surface %d\n", current^1));
	
		if(!SafeChange)
		{
			Wait(this->hidden->disp_sigbit);
			while(GetMsg(this->hidden->dispport)!=NULL);
			SafeChange=TRUE;
		}

		if(ChangeScreenBuffer(SDL_Display,this->hidden->SB[current^1]))
		{
			surface->hwdata->bmap=SDL_RastPort->BitMap=this->hidden->SB[current]->sb_BitMap;
			SafeChange=FALSE;
			SafeDisp=FALSE;
			current^=1;
		}

		if(!SafeDisp)
		{
			Wait(this->hidden->safe_sigbit);
			while(GetMsg(this->hidden->safeport)!=NULL);
			SafeDisp=TRUE;
		}

	}
	else if (this->hidden->BlitBitMap)
	{
		D(bug("[SDL] CGX_FakeFlipSurface - blit offscreen bitmap\n"));

		if (this->screen->hwdata->lock)
			this->screen->hwdata->update = 1;
		else
			BltBitMapRastPort(	this->hidden->BlitBitMap,
								0,
								0,
								SDL_RastPort,
								SDL_Window->BorderLeft,
								SDL_Window->BorderTop,
								this->screen->w,
								this->screen->h,
								0xc0);
	}
	return(0);
}


int CGX_FillHWRect(_THIS,SDL_Surface *dst,SDL_Rect *dstrect,Uint32 color)
{
	ULONG col;
	
	D(bug("CGX_FillHWRect\n"));

	if(dst->hwdata)
	{
		if (dst->format->BitsPerPixel > 8)
			col =     (((color & dst->format->Amask) >> dst->format->Ashift) << (dst->format->Aloss + 24))
					| (((color & dst->format->Rmask) >> dst->format->Rshift) << (dst->format->Rloss + 16))
					| (((color & dst->format->Gmask) >> dst->format->Gshift) << (dst->format->Gloss + 8))
					| (((color & dst->format->Bmask) >> dst->format->Bshift) <<  dst->format->Bloss);
		else
			col = color;
	
		if(!SDL_temprp) SDL_temprp = CreateRastPort();
		SDL_temprp->BitMap=(struct BitMap *)dst->hwdata->bmap;

		FillPixelArray(SDL_temprp,dstrect->x,dstrect->y,dstrect->w,dstrect->h,color);
	}
	return 0;
}

#ifndef NO_AMIGAHWSURF
int CGX_CheckHWBlit(_THIS,SDL_Surface *src,SDL_Surface *dst)
{
	int result = 0;
	
	if (dst->hwdata)
	{
		if(src->hwdata) 
		{
			// SOURCE is a HW Bitmap
			switch (src->flags & (SDL_SRCALPHA|SDL_SRCCOLORKEY))
			{
				case (SDL_SRCALPHA|SDL_SRCCOLORKEY):
					if ( CGX_SetHWColorKey(this, src, src->format->colorkey) < 0 ) 
					{
						D(bug("CheckHW blit : HW to HW CC+ALPHA... NOK!\n"));
						src->flags &= ~SDL_HWACCEL;
					}
					else
					{
						D(bug("CheckHW blit : HW to HW CC+ALPHA... OK!\n"));
						src->flags |= SDL_HWACCEL;
						src->map->hw_blit = CGX_HWtoHWBlitA;
						result = 1;
					}
					break;
				
				case (SDL_SRCCOLORKEY):
					if ( CGX_SetHWColorKey(this, src, src->format->colorkey) < 0 ) 
					{
						D(bug("CheckHW blit : HW to HW CC... NOK!\n"));
						src->flags &= ~SDL_HWACCEL;
					}
					else
					{
						D(bug("CheckHW blit : HW to HW CC... OK!\n"));
						src->flags |= SDL_HWACCEL;
						src->map->hw_blit = CGX_HWtoHWBlitCC;
						result = 1;
					}
					break;
					
				case (SDL_SRCALPHA):
					if ( CGX_CreateAlphaPixMap(this, src) < 0 ) 
					{
						D(bug("CheckHW blit : HW to HW ALPHA... NOK!\n"));
						src->flags &= ~SDL_HWACCEL;
					}
					else
					{
						D(bug("CheckHW blit : HW to HW ALPHA... OK!\n"));
						src->flags |= SDL_HWACCEL;
						src->map->hw_blit = CGX_HWtoHWBlitA;
						result = 1;
					}
					break;
	
				default:
					D(bug("CheckHW blit : HW to HW OK!\n"));
					src->flags |= SDL_HWACCEL;
					src->map->hw_blit = CGX_HWtoHWBlit;
					result = 1;
					break;
			}
		}
		else
		{
			// SOURCE is a SW pixel array
			switch (src->flags & (SDL_SRCALPHA|SDL_SRCCOLORKEY))
			{
				case (SDL_SRCALPHA|SDL_SRCCOLORKEY):
					if (CGX_ColorKeyToAlpha( this, src, src->format->colorkey) < 0)
					{
						D(bug("CheckHW blit : SW to HW CC+ALPHA... NOK!\n"));
						src->flags &= ~SDL_HWACCEL;
					}
					else
					{
						D(bug("CheckHW blit : SW to HW CC+ALPHA... OK!\n"));
						src->flags |= SDL_HWACCEL;
						src->map->hw_blit = CGX_SWtoHWBlitA;
						result = 1;
					}
					break;
				
				case (SDL_SRCCOLORKEY):
					if (CGX_ColorKeyToAlpha( this, src, src->format->colorkey) < 0)
					{
						D(bug("CheckHW blit : SW to HW CC... NOK!\n"));
						src->flags &= ~SDL_HWACCEL;
					}
					else
					{
						D(bug("CheckHW blit : SW to HW CC... OK!\n"));
						src->flags |= SDL_HWACCEL;
						src->map->hw_blit = CGX_SWtoHWBlitCC;
						result = 1;
					}
					break;
			
				case (SDL_SRCALPHA):
					D(bug("CheckHW blit : SW to HW ALPHA... OK!\n"));
					src->flags |= SDL_HWACCEL;
					src->map->hw_blit = CGX_SWtoHWBlitA;
					result = 1;
					break;
					
				default:
					D(bug("CheckHW blit : SW to HW... OK!\n"));
					src->flags |= SDL_HWACCEL;
					src->map->hw_blit = CGX_SWtoHWBlit;
					result = 1;
					break;
			}
		}
	}
	else
	{
		src->flags &= ~SDL_HWACCEL;
		D(bug("HW blitting not available\n"));
	}

	return result;
}

int CGX_CreateAlphaPixMap(_THIS,SDL_Surface *surface)
{
	/*
		We are creating a pixmap in system memory with alpha channel (RGBA format)
		Doesn't work yet for 8 bits per pixel format!
	*/
	int result = -1;
	int i;
	ULONG *pix;
	
	D(bug("CGX_CreateAlphaPixMap\n"));
	
	if   ( (surface->hwdata)
		&& (surface->format->BytesPerPixel > 1))
	{
		if (surface->hwdata->pixarrayalpha) SDL_free(surface->hwdata->pixarrayalpha);
		if ((surface->hwdata->pixarrayalpha = SDL_malloc(surface->w * surface->h * 4)) != NULL)
		{
			if(!SDL_temprp) SDL_temprp = CreateRastPort();
			SDL_temprp->BitMap=(struct BitMap *)surface->hwdata->bmap;
		
			ReadPixelArray(	surface->hwdata->pixarrayalpha,
							0,
							0,
							surface->w * 4,
							SDL_temprp,
							0,
							0,
							surface->w,
							surface->h,
							RECTFMT_ARGB32);

			if (surface->hwdata->pixarrayalpha)
			{
				/* if source data is without alpha channel it has to be set manually to 255 */
				if (  (surface->hwdata->pixfmt != PIXFMT_ARGB32)
				   || (surface->hwdata->pixfmt != PIXFMT_BGRA32)
				   || (surface->hwdata->pixfmt != PIXFMT_RGBA32))
				{
					pix = (ULONG *)surface->hwdata->pixarrayalpha;
					for (i = 0; i < surface->w * surface->h; i++) pix[i] |= AROS_BE2LONG(0xFF000000);
				}
				result = 0;
			}
		}
	}
	
	return result;
}

int CGX_SetHWColorKey(_THIS,SDL_Surface *surface, Uint32 key)
{
	/*
		We are creating a pixmap in system memory with alpha channel (RGBA format)
		The ColorKey is encoded in the Alpha channel
		Doesn't work yet for 8 bits per pixel format!
	*/
	int result = -1;
	ULONG ARGBKey = 0x00000000;
	ULONG *currentpix;
	int pixnum=0;
	
	D(bug("CGX_SetHWColorKey\n"));
	
	result = CGX_CreateAlphaPixMap(this,surface);
	
	if (result == 0)
	{
		/* Convert ColorKey to ARGB format (Do not take Alpha channel into account)*/
		ARGBKey = (((key & surface->format->Rmask) >> surface->format->Rshift) << (surface->format->Rloss + 16))
				| (((key & surface->format->Gmask) >> surface->format->Gshift) << (surface->format->Gloss + 8))
				| (((key & surface->format->Bmask) >> surface->format->Bshift) << (surface->format->Bloss + 0));
				
		currentpix = (ULONG *)surface->hwdata->pixarrayalpha;
		for (pixnum = 0; pixnum<surface->w*surface->h; pixnum++)
		{
			if ((currentpix[pixnum] & AROS_BE2LONG(0x00FFFFFF)) == AROS_BE2LONG(ARGBKey))
			{
				/* Keep color but put alpha channel to fully transparent (0 : TBC)*/
				 currentpix[pixnum] = (currentpix[pixnum] & AROS_BE2LONG(0x00FFFFFF));
			}
		}
	}
	return result;
}

int CGX_ColorKeyToAlpha(_THIS,SDL_Surface *surface, Uint32 key)
{
	/* Includes the ColorKey in the Alpha Channel of a SW surface */
	int result = -1;
	ULONG ARGBKey = 0x00000000;
	ULONG *currentpix;
	int pixnum=0;
	
	D(bug("CGX_ColorKeyToAlpha\n"));
#warning Potential endianess issue here!
	
	/* Only works for 32bits surfaces in ARGB format */
	if	(  (surface->format->BytesPerPixel == 4)
		&& (surface->format->Rmask == AROS_BE2LONG(0x00FF0000))
		&& (surface->format->Gmask == AROS_BE2LONG(0x0000FF00))
		&& (surface->format->Bmask == AROS_BE2LONG(0x000000FF)) )
	{
		/* Convert ColorKey to ARGB format (Do not take Alpha channel into account)*/
		ARGBKey = (((key & surface->format->Rmask) >> surface->format->Rshift) << (surface->format->Rloss + 16))
				| (((key & surface->format->Gmask) >> surface->format->Gshift) << (surface->format->Gloss + 8))
				| (((key & surface->format->Bmask) >> surface->format->Bshift) << (surface->format->Bloss + 0));
				
				
		currentpix = (ULONG *)surface->pixels;
		for (pixnum = 0; pixnum<surface->w*surface->h; pixnum++)
		{
			if ((currentpix[pixnum] & AROS_BE2LONG(0x00FFFFFF)) == AROS_BE2LONG(ARGBKey))
			{
				/* Keep color but put alpha channel to fully transparent (0 : TBC)*/
				currentpix[pixnum] = (currentpix[pixnum] & AROS_BE2LONG(0x00FFFFFF));
			}
		}
		result = 0;
	}
	return result;
}

static int CGX_HWtoHWBlit(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect)
{
	D(bug("Accelerated HW to HW blit\n"));

	BltBitMap(	src->hwdata->bmap,
				srcrect->x,
				srcrect->y,
				dst->hwdata->bmap,
				dstrect->x,
				dstrect->y,
				srcrect->w,
				srcrect->h,
				(ABC|ABNC),
				0xff,
				NULL);

	return 0;
}

static int CGX_HWtoHWBlitA(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect)
{
	struct SDL_VideoDevice 	*this = dst->hwdata->videodata;

	D(bug("Accelerated HW to HW blit with Alpha channel\n"));

	if (src->hwdata->pixarrayalpha)
	{
		if(!SDL_temprp) SDL_temprp = CreateRastPort();
		SDL_temprp->BitMap=(struct BitMap *)dst->hwdata->bmap;
		
		WritePixelArrayAlpha(	src->hwdata->pixarrayalpha, 
								srcrect->x, 
								srcrect->y, 
								src->w * 4, 
								SDL_temprp,
								dstrect->x,
								dstrect->y,
								srcrect->w,
								srcrect->h,
								0x01010101 * src->format->alpha );
	}
	return 0;
}

static int CGX_HWtoHWBlitCC(SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect)
{
	struct SDL_VideoDevice 	*this = dst->hwdata->videodata;

	D(bug("Accelerated HW to HW blit with ColorKey\n"));
	
	if (src->hwdata->pixarrayalpha)
	{
		if(!SDL_temprp) SDL_temprp = CreateRastPort();
		SDL_temprp->BitMap=(struct BitMap *)dst->hwdata->bmap;
		
		WritePixelArrayAlpha(	src->hwdata->pixarrayalpha, 
								srcrect->x, 
								srcrect->y, 
								src->w * 4, 
								SDL_temprp,
								dstrect->x,
								dstrect->y,
								srcrect->w,
								srcrect->h,
								0xFFFFFFFF);
	}
	return 0;
}

static int CGX_SWtoHWBlit(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect)
{
	ULONG format;
	struct SDL_VideoDevice 	*this = dst->hwdata->videodata;
	
	D(bug("Accelerated SW to HW blit\n"));
	
	format=CGX_MaskToRECTFMT(	src->format->Amask,
								src->format->Rmask,
								src->format->Gmask,
								src->format->Bmask);

	if(!SDL_temprp) SDL_temprp = CreateRastPort();
	SDL_temprp->BitMap=(struct BitMap *)dst->hwdata->bmap;
	
	WritePixelArray(	src->pixels, 
						srcrect->x, 
						srcrect->y, 
						src->pitch, 
						SDL_temprp,
						dstrect->x,
						dstrect->y,
						srcrect->w,
						srcrect->h,
						format);

	return 0;
}

static int CGX_SWtoHWBlitA(	SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect)
{
	/*
	WritePixelArrayAlpha only works with pixels array in ARGB format
	That's why we've forced the use of this format when 32 bit surface is requested
	This however will have to be checked if really working or not
	*/	
	struct SDL_VideoDevice 	*this = dst->hwdata->videodata;

	D(bug("Accelerated SW to HW blit with ALPHA\n"));
	
	return 0;
	
	if(!SDL_temprp) SDL_temprp = CreateRastPort();
	SDL_temprp->BitMap=(struct BitMap *)dst->hwdata->bmap;
	
	WritePixelArrayAlpha(	src->pixels, 
							srcrect->x, 
							srcrect->y, 
							src->pitch, 
							SDL_temprp,
							dstrect->x,
							dstrect->y,
							srcrect->w,
							srcrect->h,
							0x01010101 * src->format->alpha );

	return 0;
}

static int CGX_SWtoHWBlitCC(SDL_Surface *src, SDL_Rect *srcrect,
							SDL_Surface *dst, SDL_Rect *dstrect)
{
	struct SDL_VideoDevice 	*this = dst->hwdata->videodata;

	D(bug("Accelerated SW to HW blit with CC\n"));

	if(!SDL_temprp) SDL_temprp = CreateRastPort();
	SDL_temprp->BitMap=(struct BitMap *)dst->hwdata->bmap;
	
	WritePixelArrayAlpha(	src->pixels, 
							srcrect->x, 
							srcrect->y, 
							src->pitch, 
							SDL_temprp,
							dstrect->x,
							dstrect->y,
							srcrect->w,
							srcrect->h,
							0xFFFFFFFF );
	return 0;
}

#endif
