/*
 * progress
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

#include "jsui.h"

int jsui_widget_progress		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    int val;
    int * pval;
    PEN * newpen;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    switch(msg)
    {
    case(JSUI_MSG_DRAW):
	if (!jdr) return JSUI_R_O_K;

	primitive_3d_rect_down(jdr->buffer, 
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		JSUI_C_BG);

	newpen = Pen_Create();
	newpen->Icolor = JSUI_C_PROG;

	if (d->w > d->h)
	{
	    int hpos;
	    // horizontal box

	    pval = d->dp;
	    if (d->dp)
		val = *pval;
	    else
		val = d->d2;

	    hpos = (int)((double)val
			  / (double)d->d1 
			  * (double)(d->w - 2));
	    if (hpos > 0)
	    {
		if (hpos > d->w-2)  hpos = d->w-2;

		if (d->flags & JSUI_F_FLIP)
                {
		    hpos = d->w-1 - hpos;
		    primitive_rectangle_filled( jdr->buffer, newpen,
			jdr->vbx + d->x + hpos,
			jdr->vby + d->y + 1,
			jdr->vbx + d->x + d->w -2,
			jdr->vby + d->y + d->h -2
			);

                } else {
		    primitive_rectangle_filled( jdr->buffer, newpen,
			jdr->vbx + d->x + 1,
			jdr->vby + d->y + 1,
			jdr->vbx + d->x + hpos,
			jdr->vby + d->y + d->h -2
			);
		} 
	    }
	} else {
	    // vertical box
	    int vpos;

	    // hpos = current/max * width

	    pval = d->dp;
	    if (d->dp)
		val = *pval;
	    else
		val = d->d2;

	    vpos = (int)((double)val
			 / (double)d->d1
			 * (double)(d->h - 2)); 

	    if (vpos > 0)
	    {
		if (vpos > d->h-2)  vpos = d->h-2;

		if (d->flags & JSUI_F_FLIP)
		{
		    vpos = d->h-1 - vpos;
		    primitive_rectangle_filled( jdr->buffer, newpen,
			jdr->vbx + d->x + 1,
			jdr->vby + d->y + vpos,
			jdr->vbx + d->x + d->w-2,
			jdr->vby + d->y + d->h-2
			);

		} else {
		    primitive_rectangle_filled( jdr->buffer, newpen,
			jdr->vbx + d->x + 1,
			jdr->vby + d->y + 1,
			jdr->vbx + d->x + d->w-2,
			jdr->vby + d->y + vpos
			);
		} 
	    }
	}
	Pen_Destroy(newpen);

	jdr->flags |= JSUI_F_DIRTY;
	
    }
    return JSUI_R_O_K;
}
