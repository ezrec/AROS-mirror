/*
 * text
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

int jsui_widget_text		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    PEN * newpen;
    int x,y,w,h;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    switch(msg)
    {
    case(JSUI_MSG_DRAW):
	if (!d->dp) return JSUI_R_O_K;

	w = strlen(d->dp) * jsui_current_font->w;
	h = jsui_current_font->h;
	x = d->x;
	y = d->y;

	newpen = Pen_Create();
	newpen->Icolor = JSUI_C_BG;
	
	if (d->flags & JSUI_F_NOFRAME)
	{
	    primitive_rectangle_filled( jdr->buffer, newpen,
		jdr->vbx + d->x + 1,
		jdr->vby + d->y + 1,
		jdr->vbx + d->x + d->w,
		jdr->vby + d->y + d->h
		);
	} else {
	    primitive_3d_rect_down(jdr->buffer, 
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		JSUI_C_BG);
	}

	newpen->Icolor = JSUI_C_FG;

	y = jdr->vby + d->y + (d->h/2) - h/2;

	switch(d->d1)
	{
	case(JSUI_T_RIGHT):
	    x = jdr->vbx + d->x + d->w - 2 - w;
	    break;

	case(JSUI_T_CENTER):
	    x = jdr->vbx + d->x + (d->w/2) - w/2;
	    break;
	    
	case(JSUI_T_JUSTIFIED):
	case(JSUI_T_LEFT):
	    x = jdr->vbx + d->x + 2;
	    break;
	}

	font_render_text(jdr->buffer, newpen, newpen, jsui_current_font,
			x, y, d->dp);

	Pen_Destroy(newpen);

	jdr->flags |= JSUI_F_DIRTY;
    }
    return JSUI_R_O_K;
}
