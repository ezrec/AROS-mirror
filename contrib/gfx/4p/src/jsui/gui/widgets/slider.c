/*
 * slider
 * 
 */

/*
 * JSUI - Jerry's SDL User Interface
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

#include <string.h>
#include <ctype.h>
#include "jsui.h"

#define SLIDER_HANDLESIZE  (10)
#define SLIDER_HANDLETHICK (4)
#define SLIDER_TRACK_INSET (8)

int jsui_widget_slider		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    switch(msg)
    {
    case (JSUI_MSG_LPRESS):
    case (JSUI_MSG_LDRAG):
	if (jdr && d)
	{

	    if (d->w > d->h)
	    {
		// horizontal slider
		int xx = jdr->mx - d->x - SLIDER_TRACK_INSET;
		if (xx < 0)
		    xx = 0;

		if (xx > d->w - (2 * SLIDER_TRACK_INSET)) 
		    xx = d->w - (2 * SLIDER_TRACK_INSET);

		d->d2 = (int) (
			(double)xx /
			(double)(d->w - (2 * SLIDER_TRACK_INSET)) *
			(double)d->d1 );
	    } else {
		// vertical slider
		int yy = jdr->my - d->y - SLIDER_TRACK_INSET;
		if (yy < 0)
		    yy = 0;

		if (yy > d->h - (2 * SLIDER_TRACK_INSET)) 
		    yy = d->h - (2 * SLIDER_TRACK_INSET);

		d->d2 = (int) (
			(double)yy /
			(double)(d->h - (2 * SLIDER_TRACK_INSET)) *
			(double)d->d1 );
	    }

	    if (d->dp)
	    {
		//int (*func)(void *, int) = (int (*)(void *, int)) d->dp;
		(*(int (*)(void *, int)) d->dp) (d->dp3, d->d2);

		//(void)func(d->dp3, d->d2);
	    }

	    // then force a redraw
	    jsui_widget_slider(JSUI_MSG_DRAW, vjdr, d, c);
	}
	break;


    case (JSUI_MSG_GOTFOCUS):
	jsui_widget_slider(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case (JSUI_MSG_LOSTFOCUS):
	jsui_widget_slider(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case (JSUI_MSG_DRAW):
	if (!jdr) return JSUI_R_O_K;

	if (d->flags & JSUI_F_NOFRAME)
	{
	    primitive_rectangle_fourcolor(jdr->buffer, 
		primitive_pixel,
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		JSUI_C_BG,
		JSUI_C_BG, JSUI_C_BG, JSUI_C_BG, JSUI_C_BG);
	} else {
	    primitive_3d_rect_down(jdr->buffer, 
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		//(d->flags & JSUI_F_GOTFOCUS)?JSUI_C_HILITE:JSUI_C_BG,
		JSUI_C_BG);
	}

	if (d->flags & JSUI_F_GOTFOCUS)
	{
	    primitive_rectangle_fourcolor(jdr->buffer, 
		    primitive_pixel_dotted,
		    jdr->vbx + d->x + 2, 
		    jdr->vby + d->y + 2,
		    jdr->vbx + d->x + d->w-1 - 2,
		    jdr->vby + d->y + d->h-1 - 2,
		    (d->flags & JSUI_F_NOFRAME)?JSUI_C_BG:-1,
		    JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK);
	}

	if (d->w > d->h)
	{
	    // horizontal slider
	    int hpos;
	    primitive_3d_rect_down(jdr->buffer, 
		jdr->vbx + d->x + SLIDER_TRACK_INSET, 
		jdr->vby + d->y + d->h/2-1, 

		jdr->vbx + d->x + d->w-1 - SLIDER_TRACK_INSET,
		jdr->vby + d->y + d->h/2+1,
		-1);

	    // hpos = current/max * width

	    hpos = (int)((double)d->d2 
			  / (double)d->d1 
			  * (double)(d->w - 2 * SLIDER_TRACK_INSET))
			+ SLIDER_TRACK_INSET;


	    primitive_3d_rect_up(jdr->buffer, 
		jdr->vbx + d->x + hpos - (SLIDER_HANDLESIZE/2), 
		jdr->vby + d->y + SLIDER_HANDLETHICK -1,
		jdr->vbx + d->x + hpos + (SLIDER_HANDLESIZE/2), 
		jdr->vby + d->y + d->h - SLIDER_HANDLETHICK,
		(d->flags & JSUI_F_GOTFOCUS)?JSUI_C_HILITE:JSUI_C_BG );

	} else {
	    // vertical slider
	    int vpos;

	    primitive_3d_rect_down(jdr->buffer, 
		jdr->vbx + d->x + d->w/2-1, 
		jdr->vby + d->y + SLIDER_TRACK_INSET,

		jdr->vbx + d->x + d->w/2+1,
		jdr->vby + d->y + d->h-1 -SLIDER_TRACK_INSET,
		-1);

	    // hpos = current/max * width

	    vpos = (int)((double)d->d2 
			 / (double)d->d1
			 * (double)(d->h - 2 * SLIDER_TRACK_INSET)) 
			+ SLIDER_TRACK_INSET;

	    primitive_3d_rect_up(jdr->buffer, 
		jdr->vbx + d->x + SLIDER_HANDLETHICK -1,
		jdr->vby + d->y + vpos - (SLIDER_HANDLESIZE/2), 
		jdr->vbx + d->x + d->w - SLIDER_HANDLETHICK,
		jdr->vby + d->y + vpos + (SLIDER_HANDLESIZE/2), 
		(d->flags & JSUI_F_GOTFOCUS)?JSUI_C_HILITE:JSUI_C_BG);
	}
	
	if(jdr)
	    jdr->flags |= JSUI_F_DIRTY;
    }

    return JSUI_R_O_K;
}
