/*
** ui.c
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


/*************************************************************  
 * basic theory of operation...

- Init will
    - setup the screen onto which everything will be displayed.
    - setup the z-ordered list of dialogs
    - setup the proper palette to be used
    - start the timer

- Deinit will
    - free all things allocated in init
    - kill the timer

- when you 'run' a dialog, 
    - add a new DIALOG_RUNNING element onto the top of the active list
    - fill it in with this dialog
    - creates a new IPAGE buffer for this element
    - call the new dialog's MSG_START

- the active list contains all objects, sorted by z location.  
    - zero z is the background
    - negative z is hidden by the background, and is not redrawn and gets no 
      events other than timetick and idle events

- when you 'exit' a dialog, it removes it from the list

- when you 'hide' a dialog, it throws it in negative z

- the 'main' will init the system, then poll for events

- on SDL events, it figures out where the mouse is, by collision checking
  the mouse position with the z-ordered dialog list.  when it finds the 
  right dialog, it figures out which item gets the focus, and sends it info
    - if the dialog grabs focus, it returns and falls out, otherwise continue

 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "SDL.h"
#include "jsui.h"
#include "version.h"
#include "active.h"

/* Is the cursor visible? */
int cursor_visible = 1;

IPAGE * jsui_screen = NULL;

IPAGE * jsui_active_background_page = NULL;

void timing_init(void);
void timing_deinit(void);

extern int _jsui_reset_filter;
int jsui_filter_events( const SDL_Event *event );

SDL_Surface *actual_screen = NULL;

int
jsui_setup(
	SDL_Surface * pvscreen,
        filterEventsFallback fallback
)
{
    if (!pvscreen) return -1;
    timing_init();

    // unprime the pump...
    _jsui_reset_filter = 1;
    (void)jsui_filter_events(NULL);

    /* Set an event filter that discards everything but QUIT */
    _fallback = fallback;
    SDL_SetEventFilter(jsui_filter_events);

    // create a new page, jsui_screen the same size as the screen
    jsui_screen = Page_Create(pvscreen->w, pvscreen->h, 32);
    if (!jsui_screen) return -1;

    palette_free(jsui_screen->palette);
    jsui_screen->palette = palette_generate_default(32);

    actual_screen = pvscreen;
    
    return 0;
}

void jsui_dialog_list_free(void);

void
jsui_upset(
	void
)
{
    // turn off events..
    _fallback = NULL;
    actual_screen = NULL;
    SDL_SetEventFilter(NULL);

    timing_deinit();
    jsui_dialog_list_free();

    // destroy the custom page
    if (jsui_screen)
	Page_Destroy(jsui_screen);
    jsui_screen = NULL;
    jsui_active_background_page = NULL;
}

// Is there an easier way to do this?
int
_jsui_point_in_region(
        int x1, int y1,
        int xb1, int yb1, int xb2, int yb2
)
{
    if ( (x1 >= xb1) && (x1 <= xb2) &&
         (y1 >= yb1) && (y1 <= yb2) )
            return 1;
    return 0;
}

int
_jsui_regions_overlapping(
        int xa1, int ya1, int xa2, int ya2,
        int xb1, int yb1, int xb2, int yb2
)
{
    // if any of the four corners of A are in B, it is overlapping 
    if ( _jsui_point_in_region(xa1, ya1,   xb1, yb1, xb2, yb2)  ||
         _jsui_point_in_region(xa2, ya2,   xb1, yb1, xb2, yb2)  ||
         _jsui_point_in_region(xa1, ya2,   xb1, yb1, xb2, yb2)  ||
         _jsui_point_in_region(xa2, ya1,   xb1, yb1, xb2, yb2) )
    {
	return 1;
    }

    // if any of the four corners of B are in A, it is overlapping too.
    if ( _jsui_point_in_region(xb1, yb1,   xa1, ya1, xa2, ya2)  ||
         _jsui_point_in_region(xb2, yb2,   xa1, ya1, xa2, ya2)  ||
         _jsui_point_in_region(xb1, yb2,   xa1, ya1, xa2, ya2)  ||
         _jsui_point_in_region(xb2, yb1,   xa1, ya1, xa2, ya2) )
    {
	return 1;
    }

    // if the regions overlap without one set of points being in the other
    // set...  ie; two rectangles whose centers overlap

    if ( ( (xa1 < xb1) && (xa2 > xb2)
	   &&
	   (yb1 < ya1) && (yb2 > ya2)
         )
         ||
    	 ( (xb1 < xa1) && (xb2 > xa2)
           &&
           (ya1 < yb1) && (ya2 > yb2)
         )
	)
    {
	return 1;
    }


    return 0;
}

void 
jsui_draw_active(
	IPAGE * buffer,
	JSUI_DIALOG_RUNNING *jdr)
{
    JSUI_DIALOG_RUNNING * tjdr = jdr;

    if (!jdr) return;

    // first go down to the bottom
    while (tjdr->next)   tjdr = tjdr->next;

    // then as we come back up, from the first dialog 
    // we redraw all dialogs that are either dirty, 
    // or over the dirty region in the background.
    // -- this isn't the most efficient solution, 
    //    but it's better than redrawing everything all the time...
    while (tjdr)
    {
	if ( (tjdr->flags & JSUI_F_DIRTY) ||
                _jsui_regions_overlapping(  buffer->dr_x1,
                                            buffer->dr_y1,
                                            buffer->dr_x2,
                                            buffer->dr_y2,
                                            
                                            tjdr->x, tjdr->y, 
                                            tjdr->x+tjdr->buffer->w,
                                            tjdr->y+tjdr->buffer->h)
            )
	{
	    tjdr->flags ^= JSUI_F_DIRTY;

	    // blit it out
            Page_Blit_Image(buffer, tjdr->x, tjdr->y, 
			    tjdr->buffer, 0,0,
                            MIN(tjdr->buffer->w,  buffer->w - tjdr->x - 1),
                            MIN(tjdr->buffer->h,  buffer->h - tjdr->y - 1)
                            );
	}
	tjdr = tjdr->prev;
    }
}

int cccc = 3;
extern IPAGE * page_primary;

extern int menupoll;

void
jsui_poll(
	SDL_Surface * pvscreen
)
{
    SDL_Event event;

    // first, reset the dirty rectangle in the jsui_screen
    Page_DR_Clean(jsui_screen);

    // then send the events down to the dialogs...
    (void)jsui_filter_events(NULL);
    if (SDL_PollEvent(&event) )
    {
	switch (event.type)
	{
	    case SDL_QUIT:
		exit(0);
	    default:
		/* This should never happen */
		//printf("Warning: Event %d wasn't filtered\n", event.type);
		break;
	}
    }

if (!menupoll)
{

    // then, blit the backing screen from the active_screen pointer, if it
    // exists.  If not, draw a black rectangle there.
    if (jsui_active_background_page)
    {   
	if (jsui_active_background_page->pdirty)
	{
	    Page_Copy_Palette(jsui_screen, jsui_active_background_page);
	    jsui_active_background_page->pdirty = 0;

	    // 'dirty' the screen.
	    Page_DR_Dirtify( jsui_screen, 0, 0, 
				jsui_screen->w-1, jsui_screen->h-1);
	}
        

        // update loop:
        //  1. scale changes from prezoomed_page to zoom_page 
	//     (happens in paint update() )
        //  2. draw any changes or dirty regions from 
	//     active_background to jsui_screen
        //  3. blit the dirty part of the background page.

#ifdef SHOW_THE_UPDATE_REGION
PEN * fgpen = Pen_Create();
cccc = (cccc < 32)? cccc+1 : 3;
fgpen->Icolor = cccc;
primitive_rectangle_filled(
	jsui_screen, fgpen,
	jsui_active_background_page->dr_x1, jsui_active_background_page->dr_y1,
	jsui_active_background_page->dr_x2, jsui_active_background_page->dr_y2
	);
Pen_Destroy(fgpen);
#else
        // copy over the palette... (this is a bad way of doing this...)
        //Page_Copy_Palette(jsui_screen, jsui_active_background_page);
        
        if (jsui_active_background_page->dr_x2 > 0)
        {
            if (   (jsui_screen->w >= (jsui_active_background_page->w * 2))
                && (jsui_screen->h >= (jsui_active_background_page->h * 2))
               )
            {
                Page_DoubleBlit_Image(jsui_screen, 
                            jsui_active_background_page->dr_x1*2,
                            jsui_active_background_page->dr_y1*2,
                            
                            jsui_active_background_page,
                            jsui_active_background_page->dr_x1,
                            jsui_active_background_page->dr_y1, 
                            jsui_active_background_page->dr_x2,
                            jsui_active_background_page->dr_y2
                            );
            } else {
                Page_Blit_Image(jsui_screen, 
                            jsui_active_background_page->dr_x1,
                            jsui_active_background_page->dr_y1,
                            
                            jsui_active_background_page,
                            jsui_active_background_page->dr_x1,
                            jsui_active_background_page->dr_y1, 
                            jsui_active_background_page->dr_x2,
                            jsui_active_background_page->dr_y2
                            );
            }
        }
#endif                            
        Page_DR_Clean(jsui_active_background_page);
    } else {
        /* no background screen... all bets are off.. */
        PEN * fgpen = Pen_Create();
        fgpen->Icolor = 2;
        primitive_rectangle_filled(
                            jsui_screen, fgpen,
                            jsui_screen->dr_x1, jsui_screen->dr_y1,
                            jsui_screen->dr_x2, jsui_screen->dr_y2
                            );
        Pen_Destroy(fgpen);
    }
    
    //  3. redraw any window that:
    //		is dirty
    //		overlaps the region of the background that was dirty
    //		overlaps another window that was redrawn
    
    // -- basically, for each dialog on the screen, if it overlaps the dirty rectangle on 
    //    the screen, blit it out.  otherwise, skip it.
    jsui_draw_active(jsui_screen, jsui_active_list);
}
    gfx_screen_blit(pvscreen, jsui_screen);
}



// "run" a popup, it gets focus, but can lose focus.
int
jsui_dialog_run_popup(
	JSUI_DIALOG * dlg
)
{
    return JSUI_R_O_K;
}


// "run" a menu
int
jsui_dialog_run_menu(
	JSUI_MENU * menu, 
	int x, 
	int y
)
{
    return JSUI_R_O_K;
}


////////////////////////////////////////////////////////////////////////////////


JSUI_DIALOG *
jsui_focus_find_active(
	int screen_x,
	int screen_y
)
{
    JSUI_DIALOG_RUNNING * jdr = jsui_active_list;
    
    // we need to go from the top down on this one, since that's the 
    // way focus works... duh.
    while(jdr)
    {
	//printf ("%d %d\n", jdr->x, jdr->y);
	if (jdr->buffer)
	{
	    if ( (screen_x >= jdr->x) &&
		 (screen_x <  jdr->x + jdr->buffer->w) &&
		 (screen_y >= jdr->y) &&
		 (screen_y <  jdr->y + jdr->buffer->h) 
	       )
		return( jdr->dialog );
	}

	if (jdr->flags & JSUI_F_MODAL)
	    return( jdr->dialog );

	jdr = jdr->next;
    }
    return(NULL);
}


int
jsui_focus_find_widget(
	JSUI_DIALOG * dlg,
	int dialog_x,
	int dialog_y
)
{
    int d;
    if (!dlg) return -1;

    for (d=1 ; dlg[d].proc != NULL ; d++)
    {
	if ( (dialog_x >= dlg[d].x) &&
	     (dialog_x <  dlg[d].x + dlg[d].w) &&
             (dialog_y >= dlg[d].y) &&
	     (dialog_y <  dlg[d].y + dlg[d].h)

	     && !(dlg[d].flags & JSUI_F_NOFOCUS)  // important
	   )
	    return(d);
    }

    // we didn't find any focus, so instead return 0, which is the frame itself.
    return(0);
}


