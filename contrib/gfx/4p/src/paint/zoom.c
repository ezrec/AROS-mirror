/*
** zoom
**
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2001 Scott "Jerry" Lawrence
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

This module has dual purposes:

1: provide the JSUI system with a backing page to display behind 
   the windowing widgets

2: provide 4p with Zoom funcionality, since this takes place in
   the same plane ass the above, not to mention that the display
   of the image (page_primary) might need to be zoomed to the 
   screen anyway...
*/

#include "jsui.h"
#include "paint.h"

IPAGE * page_prezoomed = NULL;  /* edit-sized screen */
IPAGE * page_zoom = NULL;	/* GUI Window-sized screen */
PEN * zoom_pen = NULL;


void 
zoom_clearpage(
        void
)
{
    int x, y;
    if (!page_zoom || !zoom_pen) return;
    
    for (x=0 ; x < page_zoom->w ; x++)
    {
        for (y=0 ; y < page_zoom->h ; y++)
        {
            if ( ((x & 4) && (y & 4)) || 
                 (!(x&4)  && !(y&4) )
               )
                zoom_pen->Icolor = 16; // brown
            else
                zoom_pen->Icolor = 0;
            primitive_pixel(page_zoom, zoom_pen, x,y);
        }
    }
}


void
zoom_init(
        void
)
{
    page_zoom      = Page_Create(jsui_screen->w, jsui_screen->h, jsui_screen->ncolors);
    page_prezoomed = Page_Create(page_active->w, page_active->h, page_active->ncolors);
    zoom_pen = Pen_Create();

#ifdef SOYLENT_GREEN_IS_MADE_OF_DOG    
    /* fill the zoom page with black */
    zoom_pen->Icolor = 0;
    primitive_rectangle_filled(page_zoom, zoom_pen, 0,0, page_zoom->w, page_zoom->h);
#endif 

    zoom_clearpage();

#ifdef CHEESE_IS_MADE_OF_MOONDUST
    zoom_pen->Icolor = 1;
    font_render_text_center(page_zoom, zoom_pen, zoom_pen, jsui_current_font,
                    page_zoom->w/2, page_zoom->h-30,
                    "No image loaded in yet.");
#endif 

    jsui_active_background_page = page_zoom;
}


void
zoom_deinit(
        void
)
{
    jsui_active_background_page = NULL;
    
    Page_Destroy(page_zoom);
    Page_Destroy(page_prezoomed);
}

void 
zoom_stretch_pages(
        IPAGE * dest,
        IPAGE * src
)
{
    if (src->dr_x2 < 0 || src->dr_y2 < 0)
        return;
/*
printf("Stretch update:  %d %d  %d %d\n", 
                    src->dr_x1,
                    src->dr_y1,
                    src->dr_x2,
                    src->dr_y2
);
*/
    if (   (dest->w >= (src->w * 2))
        && (dest->h >= (src->h * 2)) )
    {
        Page_DoubleBlit_Image(dest, src->dr_x1*2, src->dr_y1*2,
                    src, 
                    src->dr_x1,
                    src->dr_y1,
                    src->dr_x2,
                    src->dr_y2
                    );
    } else {
        Page_Blit_Image(dest, src->dr_x1, src->dr_y1,
                    src, 
                    src->dr_x1,
                    src->dr_y1,
                    src->dr_x2,
                    src->dr_y2
                    );
    }
                    
    Page_DR_Clean(src);
}


// eventually, these will do the appropriate conversions if the
// screen is zoomed in.  
// we don't need to do any page->mouse positioning, so this is just
// single direction for now.
int
zoom_actual_to_zoomed_X(
        int mouse_x
)
{
    int maxx = page_prezoomed->w;
    if (mouse_x < 0)  mouse_x = 0;
    
    if (   (page_zoom->w >= (page_prezoomed->w * 2))
            && (page_zoom->h >= (page_prezoomed->h * 2)) )
    {
        maxx *= 2;
	mouse_x = mouse_x/2;
    }

    //if (mouse_x >= maxx)
    //    mouse_x = page_prezoomed->w;

    return (mouse_x);
}

int
zoom_actual_to_zoomed_Y(
        int mouse_y
)
{
    int maxy = page_prezoomed->h;
    if (mouse_y < 0)  mouse_y = 0;

    if (   (page_zoom->w >= (page_prezoomed->w * 2))
            && (page_zoom->h >= (page_prezoomed->h * 2)) )
    {
        maxy *=2;
        mouse_y = mouse_y/2;
    }
        
    //if (mouse_y >= maxy)
//mouse_y = page_prezoomed->h;

    return (mouse_y);
}
