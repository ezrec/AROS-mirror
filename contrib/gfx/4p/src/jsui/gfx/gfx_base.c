/*
** gfx_base.c
**
**  basic graphics interactions  
**  (everything that deals with SDL video is in here.)
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <string.h>

#include "jsui.h"


/* this one is slower -- it uses 9 pointer dereferences per pixel */
#define __BLIT_LAST_PART \
    r = r >> fmt->Rloss;	\
    r = r << fmt->Rshift;	\
    r &= fmt->Rmask;		\
				\
    g = g >> fmt->Gloss;	\
    g = g << fmt->Gshift;	\
    g &= fmt->Gmask;		\
				\
    b = b >> fmt->Bloss;	\
    b = b << fmt->Bshift;	\
    b &= fmt->Bmask;		

#define BLIT_LAST_PART \
    r = r >> rloss;	\
    r = r << rshift;	\
    r &= rmask;		\
			\
    g = g >> gloss;	\
    g = g << gshift;	\
    g &= gmask;		\
			\
    b = b >> bloss;	\
    b = b << bshift;	\
    b &= bmask;		



void
gfx_screen_blit(
	SDL_Surface * screen,
        IPAGE * base_page
)
{
    IPAGE * rubberband_page = NULL;

    SDL_PixelFormat *fmt;
    Uint32 * screenpixels;
    Uint32 r,g,b, rgb;
    int x,y;
    int dx1, dy1, dx2, dy2;
    int pindex;
    int img_yoffs = 0;
    int img_ypx;
    int scr_yoffs;

    int rloss,rshift,rmask;
    int gloss,gshift,gmask;
    int bloss,bshift,bmask;

    if (!base_page) return;

    // there has _GOT_ to be a better way to do this... but this will
    // work for now i guess.

    // one thing we can be sure of for now is that if any of the four
    // coordinates in a set are -1, then all four of them are.  We always
    // will set all four together.
#ifdef CHEESE_HAS_A_SWEET_APPLE_FLAVOR
    if (0)//rubberband_page)
    {
	if ( (base_page->dr_x1 == -1) &&
	     (rubberband_page->dr_x1 == -1) &&
	     (rubberband_page->odr_x1 == -1) 
	   ) 
	    return;

	// first, combine the rubberband old and new.
	if (rubberband_page->dr_x1 == -1)
	{
	    // new rectangle  (No new tale to tell) so it's just the old
	    dx1 = rubberband_page->odr_x1;
	    dy1 = rubberband_page->odr_y1;
	    dx2 = rubberband_page->odr_x2;
	    dy2 = rubberband_page->odr_y2;
	    
	} 
	if (rubberband_page->odr_x1 == -1) {
	    // no old rectanlge. 
	    dx1 = rubberband_page->dr_x1;
	    dy1 = rubberband_page->dr_y1;
	    dx2 = rubberband_page->dr_x2;
	    dy2 = rubberband_page->dr_y2;
	} 

	if ( (rubberband_page->dr_x1 != -1) && (rubberband_page->odr_x1 != -1))
	{
	    // both old and new.  combine it
	    dx1 = MIN(rubberband_page->odr_x1, rubberband_page->dr_x1);
	    dy1 = MIN(rubberband_page->odr_y1, rubberband_page->dr_y1);
	    dx2 = MAX(rubberband_page->odr_x2, rubberband_page->dr_x2);
	    dy2 = MAX(rubberband_page->odr_y2, rubberband_page->dr_y2);
	}

	// then combine the rubberband with the active page
	// (we don't need to worry about the 'old' dirty rectangle in the
	// active page, since we're not updating it in a way that requires this.

	if (dx1 == -1)
	{
	    // nothing in the rubberbanding layer. just blit the active page.

	    dx1 = base_page->dr_x1;
	    dy1 = base_page->dr_y1;
	    dx2 = base_page->dr_x2;
	    dy2 = base_page->dr_y2;
	} else if (base_page->dr_x1 != -1)
	{
	    // combine rubberband and active
	    dx1 = MIN(base_page->dr_x1, dx1);
	    dy1 = MIN(base_page->dr_y1, dy1);
	    dx2 = MAX(base_page->dr_x2, dx2);
	    dy2 = MAX(base_page->dr_y2, dy2);
	}

    } else {
#endif
	dx1 = base_page->dr_x1;
	dy1 = base_page->dr_y1;
	dx2 = base_page->dr_x2;
	dy2 = base_page->dr_y2;

	if (dx1 == -1  &&  dy1 == -1 &&
	    dx2 == -1  &&  dy2 == -1)
	    return;

    //}

    // some basic range checks now...
    if (dx1 < 0) dx1 = 0;
    if (dy1 < 0) dy1 = 0;
    if (dx2 < 0) dx2 = 0;
    if (dy2 < 0) dy2 = 0;

    if (dx2 >= base_page->w)  dx2 = base_page->w-1;
    if (dy2 >= base_page->h)  dy2 = base_page->h-1;

    
    SDL_ShowCursor(0);
    if ( SDL_LockSurface(screen) >= 0 )
    {
	fmt = screen->format;
	screenpixels = screen->pixels;

	rloss = fmt->Rloss;
	rshift = fmt->Rshift;
	rmask = fmt->Rmask;

	gloss = fmt->Gloss;
	gshift = fmt->Gshift;
	gmask = fmt->Gmask;

	bloss = fmt->Bloss;
	bshift = fmt->Bshift;
	bmask = fmt->Bmask;

	img_yoffs = dy1 * base_page->w;
	scr_yoffs = dy1 * screen->w;

	if (base_page->ncolors > 0)
	{
	    // it's a paletted image...
	    for ( y = dy1 
                ; y < MIN(screen->h, dy2)
                ; y++)
	    {
		for ( x = dx1
                    ; x < MIN(screen->w, dx2)
                    ; x++)
		{
		    img_ypx = img_yoffs + x;

		    if (rubberband_page)
		    {
			pindex = rubberband_page->pimage[img_ypx];

			if (pindex == -1)
			{
			    pindex = base_page->pimage[img_ypx];
			}
		    } else {
			pindex = base_page->pimage[img_ypx];
		    }

		    r = base_page->palette[pindex].r;
		    g = base_page->palette[pindex].g;
		    b = base_page->palette[pindex].b;

		    BLIT_LAST_PART

		    rgb = (r|g|b);

		    screenpixels[scr_yoffs + x] = rgb;
		}
		img_yoffs += base_page->w;
		scr_yoffs += screen->w;
	    }

	} else {

	    // it's a truecolor image...
	    for ( y = dy1 
                ; y < MIN(screen->h, dy2)
                ; y++)
	    {
		for ( x = dx1
                    ; x < MIN(screen->w, dx2)
                    ; x++)
		{
                    img_ypx = img_yoffs + x;

		    r = base_page->timage[img_ypx].r;
		    g = base_page->timage[img_ypx].g;
		    b = base_page->timage[img_ypx].b;

		    BLIT_LAST_PART

		    screenpixels[(scr_yoffs<<1) + (x<<1)] = (r|g|b);
		}
		img_yoffs += base_page->w;
		scr_yoffs += screen->w;
	    }
	}

	SDL_UnlockSurface(screen);
        
        SDL_UpdateRect(screen, base_page->dr_x1, 
                               base_page->dr_y1, 
                               base_page->dr_x2 - base_page->dr_x1,
                               base_page->dr_y2 - base_page->dr_y1);
    }
    SDL_ShowCursor(1);

    Page_DR_Clean(base_page);
    Page_DR_Clean(rubberband_page);
}
