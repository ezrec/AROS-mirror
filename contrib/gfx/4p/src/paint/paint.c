/*
** paint.c
**
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
#include "jsui.h"
#include "cursors.h"
#include "tools.h"
#include "brushes.h"
#include "fileio.h"
#include "tool_cb.h"
#include "zoom.h"

#include "dialogs.h"

/* the rubberband page overlays the primary, getting redrawn every frame */
IPAGE * page_rubberband = NULL;

/* the primary is the page that gets drawn */
IPAGE * page_primary = NULL;

/* the swap page is the page that is the backup page */
IPAGE * page_swap = NULL;

/* this points to either primary or swap */
IPAGE * page_active = NULL;

/* this pen gets changed in color every few clock ticks */
PEN * pen_cycle = NULL;

FONT_DEFINITION * current_font = NULL;

Uint8  video_bpp;
Uint32 video_flags;
Uint32 video_width;
Uint32 video_height;
Uint32 image_width;
Uint32 image_height;

void 
paint_init_screens(
	void
)
{
    /* create some images for now */
    page_primary = Page_Create(image_width, image_height, 32);

    palette_free( page_primary->palette );
    page_primary->palette = palette_generate_default( 32 );

    primitive_rectangle_filled(page_primary, page_primary->bgpen, 0,0,
				page_primary->w, page_primary->h);


    page_swap    = Page_Create(image_width, image_height, 32);

    palette_free( page_swap->palette );
    page_swap->palette = palette_generate_default( 32 );

    primitive_rectangle_filled(page_swap, page_primary->bgpen, 0,0,
				page_swap->w, page_swap->h);


#ifdef AUTOLOAD
    /* eventually, we'll dynamically create these in here, but 
       for now, we'll just load up some static images... */
    page_primary = file_load_page("Images/dpaint");
    page_swap    = file_load_page("Images/KingTut");
#endif
}


void
paint_init_rubberband(
	void
)
{
    page_rubberband = Page_Create(page_primary->w, page_primary->h, 
			page_primary->ncolors);
    Page_Negative(page_rubberband);
}


void
paint_init(
        void
)
{
    cursor_init();
    tools_init();
    brush_init();

    pen_cycle = Pen_Create();
    pen_cycle->Icolor = 0;

    paint_init_screens();
    page_active = page_primary;

    paint_init_rubberband();

    current_font = &jsui_standard_font;

    /* init zoom lastly */
    zoom_init();

    /* and start up the important dialogs... */
    jsui_dialog_run(menus_dialog);
    jsui_dialog_run(colors_dialog);
    jsui_dialog_run(toolbar_dialog);
    jsui_dialog_run(brushes_dialog);

    /* now pass some messages around */
    jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 0);
    jsui_dialog_broadcast(PAINT_MSG_DOTTEDDRAW, 0);
    jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    jsui_dialog_broadcast(PAINT_MSG_FGPALETTESET, 1);
    jsui_dialog_broadcast(PAINT_MSG_BGPALETTESET, 0);
}


void
paint_deinit(
        void
)
{
    Page_Destroy(page_rubberband);
    Page_Destroy(page_primary);
    Page_Destroy(page_swap);

    zoom_deinit();
    brush_deinit();
    tool_callback_free();
    cursor_deinit();
    
    Pen_Destroy(pen_cycle);
}




void 
paint_update(
        void
)
{
    static int last_dr_x1 = -1;
    static int last_dr_y1 = -1;
    static int last_dr_x2 = -1;
    static int last_dr_y2 = -1;

    int this_dr_x1;
    int this_dr_y1;
    int this_dr_x2;
    int this_dr_y2;

    if (!page_zoom || !page_active) return;

    // account for a modified palette...
    if (page_active->pdirty)
    {
	Page_Copy_Palette( page_zoom, page_active );
	page_active->pdirty = 0;
    }

    // this = extents( active, rubberband, last)
    this_dr_x1 =  extent_low_3(page_active->dr_x1, page_rubberband->dr_x1, last_dr_x1);
    this_dr_y1 =  extent_low_3(page_active->dr_y1, page_rubberband->dr_y1, last_dr_y1);
    this_dr_x2 = extent_high_3(page_active->dr_x2, page_rubberband->dr_x2, last_dr_x2) +1;
    this_dr_y2 = extent_high_3(page_active->dr_y2, page_rubberband->dr_y2, last_dr_y2) +1;

    // blit active.this  to prezoom
    Page_Blit_Image(page_prezoomed, 
                    this_dr_x1, this_dr_y1,
                    page_active, 
                    this_dr_x1, this_dr_y1,
                    this_dr_x2, this_dr_y2
                    ); 

    // draw rubberband.this to prezoom
    Page_Draw_Image(page_prezoomed,
                    this_dr_x1, this_dr_y1,
                    page_rubberband, 
                    this_dr_x1, this_dr_y1,
                    this_dr_x2, this_dr_y2,
                    DRAW_STYLE_MATTE
                    );

    // last = extents( active, rubberband )
    last_dr_x1 = extent_low_2(page_active->dr_x1, page_rubberband->dr_x1);
    last_dr_y1 = extent_low_2(page_active->dr_y1, page_rubberband->dr_y1);
    last_dr_x2 = extent_high_2(page_active->dr_x2, page_rubberband->dr_x2);
    last_dr_y2 = extent_high_2(page_active->dr_y2, page_rubberband->dr_y2);

//printf ("   this update:  %d %d  %d %d\n", this_dr_x1, this_dr_y1, this_dr_x2, this_dr_y2);
//printf ("   last update:  %d %d  %d %d\n", last_dr_x1, last_dr_y1, last_dr_x2, last_dr_y2);

    //NOTE: somewhere along the way, this got munged, so we'll just hardcode it in...
    page_prezoomed->dr_x1 = this_dr_x1;
    page_prezoomed->dr_y1 = this_dr_y1;
    page_prezoomed->dr_x2 = this_dr_x2;
    page_prezoomed->dr_y2 = this_dr_y2;

    // and wipe any dirty rectangles
    Page_DR_Clean(page_active);
    // also erase the rubberband layer here...
    Page_Negative(page_rubberband);
    //memset(page_rubberband->pimage, -1, sizeof(int) * page_rubberband->w * page_rubberband->h);

#ifdef VISUALIZE_THE_COOL_UPDATE_REGION
    if (page_prezoomed->fgpen->Icolor < 0) page_prezoomed->fgpen->Icolor=0;
    if (++page_prezoomed->fgpen->Icolor > 30)  page_prezoomed->fgpen->Icolor=0;
    primitive_rectangle_filled(
        page_prezoomed, page_prezoomed->fgpen, 
        this_dr_x1, this_dr_y1, this_dr_x2, this_dr_y2);
#endif

    // expand prezoom to zoom
    zoom_stretch_pages(page_zoom, page_prezoomed);
    Page_DR_Clean(page_prezoomed);

    // and then blit page_zoom to jsui_active_page
    // this happens in jsui_poll() in jsuibase.c
    // for now, we'll just make sure that the pointer is set properly
    jsui_active_background_page = page_zoom;
    
    //Page_Copy_Palette(page_zoom, page_active);
}
