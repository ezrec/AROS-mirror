/*
 * 3d box
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

/* jsui_widget_3Dbox 
 *
 * dp   pointer to text for the name
 * d1   flags for text justification
 * d2   Raised or lowered flag ( JSUI_3_RAISED | JSUI_3_LOWERED )
*/
int jsui_widget_3Dbox(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    PEN * newpen = NULL;
    PEN * newpenBG = NULL;
    int x,y,w,h;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    switch(msg)
    {
    case(JSUI_MSG_DRAW):
	if (d->dp) 
	{
	    w = strlen(d->dp) * jsui_current_font->w;
	    h = jsui_current_font->h;
	}
	x = d->x;
	y = d->y;

	newpen = Pen_Create();
	newpen->Icolor = JSUI_C_BG;
	
	if (d->d2 == JSUI_3_RAISED)
	{
	    primitive_3d_rect_up(jdr->buffer, 
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		-1);
	} else {
	    primitive_3d_rect_down(jdr->buffer, 
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		-1);
	}

	newpen->Icolor = JSUI_C_FG;

	if (d->dp)
	{
	    y = jdr->vby + d->y - jsui_current_font->h/2;

	    switch(d->d1)
	    {
	    case(JSUI_T_RIGHT):
		x = jdr->vbx + d->x + d->w - 2 - 5 - w;
		break;

	    case(JSUI_T_CENTER):
		x = jdr->vbx + d->x + (d->w/2) + 5 - w/2;
		break;
		
	    case(JSUI_T_JUSTIFIED):
	    case(JSUI_T_LEFT):
		x = jdr->vbx + d->x + 2;
		break;
	    }

	    newpenBG = Pen_Create();
	    newpenBG->Icolor = JSUI_C_BG;
	    newpenBG->text_type = TEXT_TYPE_JAM2;
	    newpen->Icolor = JSUI_C_FG;
	    newpen->text_type = TEXT_TYPE_JAM2;

	    font_render_text(jdr->buffer, newpen, newpenBG, jsui_current_font, x+1, y+1, d->dp);

	    newpen->Icolor = JSUI_C_BRIGHT;
	    newpen->text_type = TEXT_TYPE_JAM1;
	    font_render_text(jdr->buffer, newpen, newpenBG, jsui_current_font, x, y, d->dp);

	    Pen_Destroy(newpenBG);
	}
	Pen_Destroy(newpen);

	jdr->flags |= JSUI_F_DIRTY;
        break;
    }
    return JSUI_R_O_K;
}
