/*
 * page
 * 
 *   This file will abstract out the idea of a graphical 'page'
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

/*
 * a page is an image that is loaded into the program, or is created from
 * scratch.  features of a 'page' are:
 *	- undo buffer/queue
 *      - regions changed since the undo buffer
 *      - its own palette
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsui.h"

//#define QUICKER_BLIT (1)

//  ncolors == 0 if truecolor
IPAGE *
Page_Create(
	int w,
	int h,
	int ncolors
)
{
    static unsigned long id = 0;
    IPAGE * tPage = NULL;

    if (!w || !h)  return NULL;

    tPage = (IPAGE *) malloc (sizeof (IPAGE));
    if (!tPage) return NULL;

    tPage->id = id++;

    tPage->w = w;
    tPage->h = h;

    tPage->bgpen = Pen_Create();
    tPage->bgpen->Icolor     = 0;
    tPage->bgpen->RGBcolor.r = 0;
    tPage->bgpen->RGBcolor.g = 0;
    tPage->bgpen->RGBcolor.b = 0;

    tPage->fgpen = Pen_Create();
    tPage->fgpen->Icolor     = 1;
    tPage->fgpen->RGBcolor.r = 255;
    tPage->fgpen->RGBcolor.g = 255;
    tPage->fgpen->RGBcolor.b = 255;

    tPage->ncolors = ncolors;

    tPage->timage = NULL;
    tPage->pimage = NULL;
    tPage->palette = NULL;
    tPage->pdirty = 0;

    tPage->undo_timage = NULL;
    tPage->undo_pimage = NULL;
    tPage->undo_palette = NULL;

    tPage->alpha  = (int *)malloc(sizeof(int) * w * h);
    memset(tPage->alpha, 255, sizeof(int) * w * h);

    if (ncolors > 0)
    {
	// it's a paletted (indexed) image
	tPage->palette = (COLEL *)malloc(sizeof(COLEL) * ncolors);
	tPage->pimage  = (int *)malloc(sizeof(int) * w * h);
	tPage->undo_palette = (COLEL *)malloc(sizeof(COLEL) * ncolors);
	tPage->undo_pimage  = (int *)malloc(sizeof(int) * w * h);
	tPage->pdirty = 1;

	// now zero the buffers
	memset(tPage->palette, 0, sizeof(COLEL) * ncolors);
	memset(tPage->pimage,  0, sizeof(int) * w * h);
	memset(tPage->undo_palette, 0, sizeof(COLEL) * ncolors);
	memset(tPage->undo_pimage,  0, sizeof(int) * w * h);

    } else {
	// it's a truecolor image
	tPage->timage = (COLEL *)malloc(sizeof(COLEL) * w * h);
	tPage->undo_timage = (COLEL *)malloc(sizeof(COLEL) * w * h);

	// now zero the buffers
	memset(tPage->timage, 0, sizeof(COLEL) * w * h);
	memset(tPage->undo_timage, 0, sizeof(COLEL) * w * h);
    }

    tPage->dr_x1 = tPage->dr_y1 = 0;
    tPage->dr_x2 = w-1;
    tPage->dr_y2 = h-1;

    tPage->handle_x=0;
    tPage->handle_y=0;

    tPage->color_cycle = NULL;

    return tPage;
}


void
Page_Destroy(
	IPAGE * page
)
{
    if (page)
    {
	if (page->undo_palette)  free(page->undo_palette);
	if (page->undo_pimage)   free(page->undo_pimage);
	if (page->undo_timage)   free(page->undo_timage);

	if (page->palette)  free(page->palette);
	if (page->pimage)   free(page->pimage);
	if (page->timage)   free(page->timage);
	if (page->alpha)    free(page->alpha);

	Pen_Destroy(page->fgpen);
	Pen_Destroy(page->bgpen);

	// don't forget to free the ranges as well...

	free(page);
    }
}


IPAGE *
Page_Duplicate(
	IPAGE * page
)
{
    IPAGE * tPage;

    if (!page) return NULL;

    tPage = Page_Create(page->w, page->h, page->ncolors);

    if (!tPage) return NULL;

    memcpy (tPage->alpha, page->alpha, page->w * page->h * sizeof(int));

    if (page->ncolors > 0)
    {
	// indexed.  copy the image and the palette
	memcpy (tPage->pimage, page->pimage, page->w * page->h * sizeof(int));
	memcpy (tPage->palette, page->palette, page->ncolors * sizeof(COLEL));

    } else {
	// truecolor.  copy just the image.
	memcpy (tPage->timage, page->timage, page->w * page->h * sizeof(COLEL));
    }

    return tPage;
}

////////////////////////////////////////////////////////////////////////////////
// Dirty rectangle functions

void
Page_DR_Dirtify(
	IPAGE * page,
	int rx1, int ry1,
	int rx2, int ry2
)
{
    int x1,y1,x2,y2;
	
    if (!page)
	return;

    // rearrange so that x1,y1 is upper left  and x2,y2 is lower right
    x1 = MIN (rx1, rx2);
    y1 = MIN (ry1, ry2);
    x2 = MAX (rx1, rx2);
    y2 = MAX (ry1, ry2);

    // since these are now sorted, we can just bounds check them easily
    // negative numbers are illegal in here.
    if ((y2 < 0)  ||  (x2 < 0))
    {
	return;
    }
    
    // or instead, how about something that works;
    //if (x2 < 0) return;
    
    // clip the dirty region to the page
    //  incidently, it helps if you compare x's with w's and y's with h's.
    //  (2001.09.05 bug)
    if (x1<0) x1 = 0;  if (x1 >= page->w) x1 = page->w -1;
    if (y1<0) y1 = 0;  if (y1 >= page->h) y1 = page->h -1;
    if (x2<0) x2 = 0;  if (x2 >= page->w) x2 = page->w -1;
    if (y2<0) y2 = 0;  if (y2 >= page->h) y2 = page->h -1;

    // if there was no old setting, force the new one
    // otherwise, we'll compare what's there with the new one.
    // (do this for all four ordinates.
    if (page->dr_x1 < 0)         page->dr_x1 = x1;
    else if (page->dr_x1 > x1)   page->dr_x1 = x1;

    if (page->dr_y1 < 0)         page->dr_y1 = y1;
    else if (page->dr_y1 > y1)   page->dr_y1 = y1;

    if (page->dr_x2 < 0)         page->dr_x2 = x2;
    else if (page->dr_x2 < x2)   page->dr_x2 = x2;

    if (page->dr_y2 < 0)         page->dr_y2 = y2;
    else if (page->dr_y2 < y2)   page->dr_y2 = y2;
}

void
Page_DR_Clean(
	IPAGE * page
)
{
    if (!page) return;

    page->dr_x1 = page->dr_y1 = page->dr_x2 = page->dr_y2 = -1;
}


////////////////////////////////////////////////////////////////////////////////

void
Page_Negative(
    IPAGE * page
)
{
    // this invalidates a rubberband page, and forces a redraw
    if (!page) return;

    if (page->dr_x2 == -1 && page->dr_y2 == -1) return;

    if (page->ncolors > 0)
    {
        if (page->pimage);
            memset(page->pimage, -1, sizeof(int) * page->w * page->h);
    } else {
        if (page->timage);
            memset(page->timage, -1, sizeof(COLEL) * page->w * page->h);
    }

    page->dr_x1 =
    page->dr_x2 =
    page->dr_y1 =
    page->dr_y2 = -1;
}

////////////////////////////////////////////////////////////////////////////////

void
Page_Alpha_Fill(
	IPAGE * page,
	int set_to
)
{
    if (!page || !page->alpha)  return;

    memset(page->alpha, set_to, sizeof(int) * page->w * page->h);
}


void 
Page_Alpha_Chromakey_Index(
	IPAGE * page,
	int key,
	int set_to
)
{
    int bpos;

    if (!page || !page->alpha || !page->pimage) return;
    
    for (bpos = 0 ; bpos < page->w * page->h ; bpos++)
    {
	if (page->pimage[bpos] == key)
	{
	    page->alpha[bpos] = set_to;
	}
    }
}


void 
Page_Alpha_Chromakey_RGB(
	IPAGE * page,
	COLEL key,
	int set_to
)
{
    int bpos;

    if (!page || !page->alpha || !page->timage) return;
    
    for (bpos = 0 ; bpos < page->w * page->h ; bpos++)
    {
	if ( (page->timage[bpos].r == key.r) &&
	     (page->timage[bpos].g == key.g) &&
	     (page->timage[bpos].b == key.b) )
	{
	    page->alpha[bpos] = set_to;
	}
    }
}

////////////////////////////////////////////////////////////////////////////////

void
Page_Blit_Image(
	IPAGE * destination, 
	int x, int y, 
	IPAGE * source,
	int x1, int y1,
	int x2, int y2
)
{
    int dx = x;
    int dy = y;
    int sx, sy;
    int wb;

    if (!destination || !source)  return;
    if (x == -1 && y ==  -1)
        return;

    if (source->ncolors > 0)
    {
        wb = MIN( (x2-x1), (x + source->w) );
        wb = MIN( wb, (x1 + destination->w) );
        wb *= sizeof(source->pimage[0]);  // width in bytes
        
	for (sy = y1 ; 
                (sy < y2) && 
                (sy < source->h) &&
                (dy < destination->h)
                ; sy++)
	{
#ifdef QUICKER_BLIT
            // this should hopefully speed things up a little...
            //  -- thanks, pat!  :D
            memcpy(&destination->pimage[(dy*destination->w)+x],
                   &source->pimage[(sy*source->w)+x1],
                   wb * sizeof(int) );
#else
            dx = x;
	    for (sx = x1 ; sx < x2 ; sx++)
	    {
		destination->pimage[(dy*destination->w)+dx] =
				source->pimage[(sy*source->w)+sx];
		dx++;
	    }

#endif
            dy++;
	} 

    } else {
        // NOTE:  if we ever add in 32 bit support, this can be optimized like the 
        //	  above.  whee.
	for (sy = y1 ; sy < y2 ; sy++)
	{
	    dx = x;
	    for (sx = x1 ; sx < x2 ; sx++)
	    {
		destination->timage[(dy*destination->w)+dx].r =
				source->timage[(sy*source->w)+sx].r;
		destination->timage[(dy*destination->w)+dx].g =
				source->timage[(sy*source->w)+sx].g;
		destination->timage[(dy*destination->w)+dx].b =
				source->timage[(sy*source->w)+sx].b;
		dx++;
	    }
	    dy++;
	} 
    }

    Page_DR_Dirtify(destination, x, y, x+x2-x1, y+y2-y1);
}


// this following function is so very unoptimized. hehe.
void
Page_DoubleBlit_Image(
	IPAGE * destination, 
	int x, int y, 
	IPAGE * source,
	int x1, int y1,
	int x2, int y2
)
{
    int dx = x;
    int dy = y;
    int sx, sy;
  
    if (!destination || !source)  return;
    if (x == -1 && y ==  -1)
        return;
    
    dy = y;
        
    if (source->ncolors > 0)
    {
	for (sy = y1 ; 
                (sy < y2) && 
                (sy < source->h) &&
                (dy < destination->h)
                ; sy++)
	{
            dx = x;
	    for (sx = x1 ; sx < x2 ; sx++)
	    {
                destination->pimage[((dy+0)*destination->w)+dx] =
                destination->pimage[((dy+1)*destination->w)+dx] =
                destination->pimage[((dy+0)*destination->w)+dx+1] =
                destination->pimage[((dy+1)*destination->w)+dx+1] =
                        source->pimage[(sy*source->w)+sx];
		dx+=2;
	    }
            
            dy+=2;
            //dy = dy + (destination->w); 
	} 

    }
    Page_DR_Dirtify(destination, x, y, x+(x2-x1)*2, y+(y2-y1)*2);
}


void
Page_Draw_Image(
	IPAGE * destination, 
	int x, int y, 
	IPAGE * source,
	int x1, int y1,
	int x2, int y2,
	int draw_mode
)
{
    int dx = x;
    int dy = y;
    int sx, sy;
    int icolor;

    if (!destination || !source)  return;

    if (source->ncolors > 0)
    {
	for (sy = y1 ; (sy < y2) && (dy < destination->h); sy++)
	{
	    dx = x;

	    if (dy >=0)
	    for (sx = x1 ; (sx < x2) && (dx < destination->w) ; sx++)
	    {
		switch (draw_mode)
		{
		    case(DRAW_STYLE_MATTE):
			if (source->alpha[(sy*source->w)+sx] != 0)
			{
			    icolor = source->pimage[(sy*source->w)+sx];
			} else {
			    icolor = -1;
			}
			break;

		    case(DRAW_STYLE_COLOR):
		    case(DRAW_STYLE_FGCOLOR):
			if (source->alpha[(sy*source->w)+sx] != 0)
			{
			    icolor = destination->fgpen->Icolor;
			} else {
			    icolor = -1;
			}
			break;

		    case(DRAW_STYLE_BGCOLOR):
			if (source->alpha[(sy*source->w)+sx] != 0)
			{
			    icolor = destination->bgpen->Icolor;
			} else {
			    icolor = -1;
			}
			break;

		    case(DRAW_STYLE_REPLACE):
			icolor = source->pimage[(sy*source->w)+sx];
			break;

		    default:
			icolor = destination->fgpen->Icolor;
			break;
		}

		if (icolor != -1)
		{
		    if (dx >=0)
		    destination->pimage[(dy*destination->w)+dx] = icolor;
		}
		dx++;
	    }
	    dy++;
	} 

    } else {
	for (sy = y1 ; sy < y2 ; sy++)
	{
	    dx = x;
	    for (sx = x1 ; sx < x2 ; sx++)
	    {
		destination->timage[(dy*destination->w)+dx].r =
				source->timage[(sy*source->w)+sx].r;
		destination->timage[(dy*destination->w)+dx].g =
				source->timage[(sy*source->w)+sx].g;
		destination->timage[(dy*destination->w)+dx].b =
				source->timage[(sy*source->w)+sx].b;
		dx++;
	    }
	    dy++;
	} 
    }

    Page_DR_Dirtify(destination, x, y, x+x2, y+y2);
}

// this following function is so very unoptimized also. hehe.
// a memcpy would do wonders in here...
void
Page_VariBlit_Image(
	IPAGE * destination, 
	int idx, int idy, 
	IPAGE * source,
	int isx, int isy,
        int *skips, int *widths,
        int maxw, int h
)
{
    int ch, pc;
    int dx, dy;
    int sx, sy;
  
    if (!destination || !source || !skips || !widths)  return;
    
    if (source->ncolors > 0)
    {
	for (ch = 0 ; ch < h ; ch++ )
	{
            for (pc = 0 ; pc < widths[ch] ; pc++)
            {
                dx = idx+pc+skips[ch];
                dy = idy+ch;
                sx = isx+pc+skips[ch];
                sy = isx+ch;
                destination->pimage[((dy)*destination->w)+dx] =
                     source->pimage[((sy)*source->w)+sx] ;
            }
	} 

    }
    Page_DR_Dirtify(destination, dx, dy, dx+maxw, dy+h);
}


void
Page_Copy_Palette(
	IPAGE * destination,
	IPAGE * source
)
{
    int color;
    if (!destination || destination->ncolors < 0 ||
	!source      || source->ncolors < 0 )
		return;

    if (!destination->palette || !source->palette)
	    return;

    for (color=0 ; color < MIN(destination->ncolors, source->ncolors) ; color++)
    {
	destination->palette[color].r = source->palette[color].r;
	destination->palette[color].g = source->palette[color].g;
	destination->palette[color].b = source->palette[color].b;
    }
    destination->pdirty = 1;
    Page_DR_Dirtify(destination, 0, 0, destination->w-1, destination->h-1);
}



void
Page_Copy_Pens(
	IPAGE * destination,
	IPAGE * source
)
{
    if (!destination || !destination->bgpen || !destination->fgpen ||
	!source      || !source->bgpen      || !source->fgpen)
		return;

    memcpy(destination->fgpen, source->fgpen, sizeof(PEN));
    memcpy(destination->bgpen, source->bgpen, sizeof(PEN));
}


////////////////////////////////////////////////////////////////////////////////

void
Page_Handle_Center(
	IPAGE * page
)
{
    if (!page) return;

    page->handle_x = page->w >> 1;
    page->handle_y = page->h >> 1;

}


IPAGE *
Page_Cutout_Brush(
	IPAGE * page,
	int x1, int y1,
	int x2, int y2,
	int excl_brush,
	PEN * wipe_original
)
{
    IPAGE * newbrush;
    int ax1 = MIN(x1, x2);
    int ay1 = MIN(y1, y2);
    int ax2 = MAX(x1, x2);
    int ay2 = MAX(y1, y2);

    if (!page) return NULL;

    if (excl_brush)
    {
	ax2--;
	ay2--;
    };

    newbrush = Page_Create(ax2-ax1+1, ay2-ay1+1, page->ncolors);

    Page_Copy_Palette(newbrush, page);
    Page_Copy_Pens(newbrush, page);

    Page_Blit_Image(newbrush, 0, 0,
		    page, ax1, ay1, ax2+1, ay2+1);

    Page_Alpha_Fill(newbrush, 1);
    Page_Alpha_Chromakey_Index(newbrush,
			newbrush->bgpen->Icolor, 0);

    Page_Handle_Center(newbrush);

    // and in case it was right-button selected...
    if (wipe_original)
    {
	Page_Undo_Checkpoint(page);
	primitive_rectangle_filled(page, page->bgpen,
		ax1, ay1, ax2, ay2);
    }

    return (newbrush);
}


////////////////////////////////////////////////////////////////////////////////

void
Page_Undo_Checkpoint(
	IPAGE * page
)
{
    if (!page) return;

    printf("undo checkpoint\n");
    if (page->ncolors > 0)
    {
	memcpy (page->undo_pimage, page->pimage, 
			page->w * page->h * sizeof(int));
    } else {
	memcpy (page->undo_timage, page->undo_timage, 
			page->w * page->h * sizeof(COLEL));
    }
}


void
Page_Undo(
	IPAGE * page
)
{
    int   * temp_pimage;
    COLEL * temp_timage;

    if (!page) return;

    if (page->ncolors > 0)
    {
	temp_pimage = page->pimage;
	page->pimage = page->undo_pimage;
	page->undo_pimage = temp_pimage;
    } else {
	temp_timage = page->timage;
	page->timage = page->undo_timage;
	page->undo_timage = temp_timage;
    }

    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}



////////////////////////////////////////////////////////////////////////////////


void 
Page_Flip_Horizontal (
	IPAGE * page
)
{
    int x,y;
    int * alpha;
    int * pimage;
    COLEL * timage;

    if (!page) return;

    if (page->alpha)
    {
	alpha = (int *)malloc(sizeof(int) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
		alpha[(y*page->w)+(page->w-1-x)] = page->alpha[(y*page->w)+x];
	    }
	}
	free(page->alpha);
	page->alpha = alpha;
    }

    if (page->ncolors > 0)
    {
	if (!page->pimage) return;

	pimage = (int *)malloc(sizeof(int) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
                pimage[(y*page->w)+(page->w-1-x)] 
			= page->pimage[(y*page->w)+x];
	    }
	}
	free(page->pimage);
	page->pimage = pimage;
	
    } else {
	if (!page->timage) return;

	timage = (COLEL *)malloc(sizeof(COLEL) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
                timage[(y*page->w)+(page->w-1-x)].r
			= page->timage[(y*page->w)+x].r;
                timage[(y*page->w)+(page->w-1-x)].g
			= page->timage[(y*page->w)+x].g;
                timage[(y*page->w)+(page->w-1-x)].b
			= page->timage[(y*page->w)+x].b;
	    }
	}
	free(page->timage);
	page->timage = timage;
    }
    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}


void 
Page_Flip_Vertical (
	IPAGE * page
)
{
    int x,y;
    int * alpha;
    int * pimage;
    COLEL * timage;

    if (!page) return;

    if (page->alpha)
    {
	alpha = (int *)malloc(sizeof(int) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
		alpha[((page->h-1-y)*page->w)+x] = page->alpha[(y*page->w)+x];
	    }
	}
	free(page->alpha);
	page->alpha = alpha;
    }

    if (page->ncolors > 0)
    {
	if (!page->pimage) return;

	pimage = (int *)malloc(sizeof(int) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
                pimage[((page->h-1-y)*page->w)+x] 
			= page->pimage[(y*page->w)+x];
	    }
	}
	free(page->pimage);
	page->pimage = pimage;
	
    } else {
	if (!page->timage) return;

	timage = (COLEL *)malloc(sizeof(COLEL) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
                timage[((page->h-1-y)*page->w)+x].r
			= page->timage[(y*page->w)+x].r;
                timage[((page->h-1-y)*page->w)+x].g
			= page->timage[(y*page->w)+x].g;
                timage[((page->h-1-y)*page->w)+x].b
			= page->timage[(y*page->w)+x].b;
	    }
	}
	free(page->timage);
	page->timage = timage;
    }
    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}


void
Page_Swap_XY(
	IPAGE * page
)
{
    int x,y;
    int w,h,yw=0;
    int * alpha;
    int * pimage;
    COLEL * timage;

    if (!page) return;

    w = page->w;
    h = page->h;

    if (page->alpha)
    {
	alpha = (int *)malloc(sizeof(int) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
		alpha[x*h+y] = page->alpha[y*w+x];
	    }
	}
	free(page->alpha);
	page->alpha = alpha;
    }

    if (page->ncolors > 0)
    {
	if (!page->pimage) return;

	pimage = (int *)malloc(sizeof(int) * page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    yw = y*w;
	    for(x=0 ; x < page->w ; x++)
	    {
		pimage[x*h+y] = page->pimage[y*w+x];
	    }
	}
	free(page->pimage);
	page->pimage = pimage;
	
    } else {
	if (!page->timage) return;

	timage = (COLEL *)malloc(sizeof(COLEL) * page->w * page->h);
	memset(timage, 0, sizeof(COLEL)*page->w * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
		timage[x*h+y].r = page->timage[y*w+x].r;
		timage[x*h+y].g = page->timage[y*w+x].g;
		timage[x*h+y].b = page->timage[y*w+x].b;
	    }
	}
	free(page->timage);
	page->timage = timage;
    }

    x = page->w;
    y = page->h;

    page->w = y;
    page->h = x;

    Page_Handle_Center(page);

    if (page->undo_timage) free(page->undo_timage);  page->undo_timage = NULL;
    if (page->undo_pimage) free(page->undo_pimage);  page->undo_pimage = NULL;

    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}



////////////////////////////////////////
void 
Page_Double_Horizontal(
	IPAGE * page
)
{
    int x,y;
    int w,yw=0;
    int * alpha;
    int * pimage;
    COLEL * timage;

    if (!page) return;

    w = page->w;

    if (page->alpha)
    {
	alpha = (int *)malloc(sizeof(int) * page->w*2 * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    yw = y*w;
	    for(x=0 ; x < page->w ; x++)
	    {
		alpha[yw*2+(x*2)]   = page->alpha[yw+x];
		alpha[yw*2+(x*2)+1] = page->alpha[yw+x];
	    }
	}
	free(page->alpha);
	page->alpha = alpha;

    }

    if (page->ncolors > 0)
    {
	if (!page->pimage) return;

	pimage = (int *)malloc(sizeof(int) * page->w*2 * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    yw = y*w;
	    for(x=0 ; x < page->w ; x++)
	    {
		pimage[yw*2+(x*2)]   = page->pimage[yw+x];
		pimage[yw*2+(x*2)+1] = page->pimage[yw+x];
	    }
	}
	free(page->pimage);
	page->pimage = pimage;
	
    } else {
	if (!page->timage) return;

	timage = (COLEL *)malloc(sizeof(COLEL) * page->w*2 * page->h);
	memset(timage, 0, sizeof(COLEL)*page->w*2 * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
		timage[yw*2+(x*2)].r   = page->timage[yw+x].r;
		timage[yw*2+(x*2)+1].r = page->timage[yw+x].r;
		timage[yw*2+(x*2)].g   = page->timage[yw+x].g;
		timage[yw*2+(x*2)+1].g = page->timage[yw+x].g;
		timage[yw*2+(x*2)].b   = page->timage[yw+x].b;
		timage[yw*2+(x*2)+1].b = page->timage[yw+x].b;
	    }
	}
	free(page->timage);
	page->timage = timage;
    }

    page->w *= 2;
    page->handle_x *= 2;

    if (page->undo_timage) free(page->undo_timage);  page->undo_timage = NULL;
    if (page->undo_pimage) free(page->undo_pimage);  page->undo_pimage = NULL;

    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}



void 
Page_Double_Vertical(
	IPAGE * page
)
{
    int x,y;
    int w,yw=0;
    int * alpha;
    int * pimage;
    COLEL * timage;

    if (!page) return;

    w = page->w;

    if (page->alpha)
    {
	alpha = (int *)malloc(sizeof(int) * page->w * page->h*2);

	for(y=0 ; y < page->h ; y++)
	{
	    yw = y*w;
	    for(x=0 ; x < page->w ; x++)
	    {
		alpha[yw*2+x]   = page->alpha[yw+x];
		alpha[yw*2+x+w] = page->alpha[yw+x];
	    }
	}
	free(page->alpha);
	page->alpha = alpha;

    }

    if (page->ncolors > 0)
    {
	if (!page->pimage) return;

	pimage = (int *)malloc(sizeof(int) * page->w * page->h*2);

	for(y=0 ; y < page->h ; y++)
	{
	    yw = y*w;
	    for(x=0 ; x < page->w ; x++)
	    {
		pimage[yw*2+x]   = page->pimage[yw+x];
		pimage[yw*2+x+w] = page->pimage[yw+x];
	    }
	}
	free(page->pimage);
	page->pimage = pimage;
	
    } else {
	if (!page->timage) return;

	timage = (COLEL *)malloc(sizeof(COLEL) * page->w*2 * page->h);

	for(y=0 ; y < page->h ; y++)
	{
	    for(x=0 ; x < page->w ; x++)
	    {
		timage[yw*2+x].r   = page->timage[yw+x].r;
		timage[yw*2+x+w].r = page->timage[yw+x].r;
		timage[yw*2+x].g   = page->timage[yw+x].g;
		timage[yw*2+x+w].g = page->timage[yw+x].g;
		timage[yw*2+x].b   = page->timage[yw+x].b;
		timage[yw*2+x+w].b = page->timage[yw+x].b;
	    }
	}
	free(page->timage);
	page->timage = timage;
    }

    page->h *= 2;
    page->handle_y *= 2;

    if (page->undo_timage) free(page->undo_timage);  page->undo_timage = NULL;
    if (page->undo_pimage) free(page->undo_pimage);  page->undo_pimage = NULL;

    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}



////////////////////////////////////////////////////////////////////////////////

void 
Page_Halve_Size(
	IPAGE * page
)
{
    int x,y;
    int w,yw=0;
    int * alpha;
    int * pimage;
    COLEL * timage;

    if (!page) return;

    w = page->w/2;

    if (page->alpha)
    {
	alpha = (int *)malloc(sizeof(int) * page->w * page->h / 4);
	memset(alpha, 0, sizeof(int) * page->w * page->h / 4);

	for(y=0 ; y < page->h/2 ; y++)
	{
	    yw = y*w;
	    for(x=0 ; x < page->w/2 ; x++)
	    {
		alpha[yw+x] = page->alpha[(page->w*y*2)+x*2];
	    }
	}
	free(page->alpha);
	page->alpha = alpha;

    }

    if (page->ncolors > 0)
    {
	if (!page->pimage) return;

	pimage = (int *)malloc(sizeof(int) * page->w * page->h/4);
	memset(pimage, 0, sizeof(int) * page->w * page->h / 4);

	for(y=0 ; y < page->h/2; y++)
	{
	    yw = y*w;
	    for(x=0 ; x < page->w/2 ; x++)
	    {
		pimage[yw+x] = page->pimage[(page->w*y*2)+x*2];
	    }
	}
	free(page->pimage);
	page->pimage = pimage;
	
    } else {
	if (!page->timage) return;

	timage = (COLEL *)malloc(sizeof(COLEL) * page->w*2 * page->h);

	for(y=0 ; y < page->h/2 ; y++)
	{
	    for(x=0 ; x < page->w/2 ; x++)
	    {
		timage[yw+x].r = page->timage[(page->w*y*2)+x*2].r;
		timage[yw+x].g = page->timage[(page->w*y*2)+x*2].g;
		timage[yw+x].b = page->timage[(page->w*y*2)+x*2].b;
	    }
	}
	free(page->timage);
	page->timage = timage;
    }

    page->h = page->h/2;
    page->w = page->w/2;

    Page_Handle_Center(page);

    if (page->undo_timage) free(page->undo_timage);  page->undo_timage = NULL;
    if (page->undo_pimage) free(page->undo_pimage);  page->undo_pimage = NULL;

    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}


