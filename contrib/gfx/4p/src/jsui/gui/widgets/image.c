/*
 * image
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


/*
 *  jsui_widget_image
 * dp   pointer to an IPAGE
 * d1   doublescale the image?
 * d2   lowered box instead of raised box?
 */ 

int jsui_widget_image(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    IPAGE * dp = d->dp;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    if (msg == JSUI_MSG_DRAW)
    {
	if (!jdr) return JSUI_R_O_K;

	// draw the bitmap
	if (d->dp)
	{
	    if (d->d1 == 1) {
		Page_DoubleBlit_Image(
		    jdr->buffer,
		    jdr->vbx + d->x + 1,
		    jdr->vby + d->y + 1,
		    dp,
		    d->x, d->y,
		    d->w/2, d->h/2
		);
	    } else {
		Page_Blit_Image(
		    jdr->buffer,
		    jdr->vbx + d->x + 1,
		    jdr->vby + d->y + 1,
		    dp,
		    0, 0,
		    d->w, d->h
		);
	    } 
	}

	// draw the 3dbox
	if (!(d->flags & JSUI_F_NOFRAME))
	{
	    if ( d->d2 )
	    { 
		primitive_3d_rect_down(jdr->buffer,
		    jdr->vbx + d->x, jdr->vby + d->y, 
		    jdr->vbx + d->x + d->w-1,
		    jdr->vby + d->y + d->h-1,
		    -1);
	    } else {
		primitive_3d_rect_up(jdr->buffer, 
		    jdr->vbx + d->x, jdr->vby + d->y, 
		    jdr->vbx + d->x + d->w-1,
		    jdr->vby + d->y + d->h-1,
		    -1);
	    }
	}

	if (d->flags & JSUI_F_DISABLED)
	{
	    primitive_rectangle_checkerboard(
		    jdr->buffer,
		    jdr->vbx + d->x + 2, 
		    jdr->vby + d->y + 2,
		    jdr->vbx + d->x + d->w-1 - 2,
		    jdr->vby + d->y + d->h-1 - 2,
		    JSUI_C_BG
	    );
	}
	
	jdr->flags |= JSUI_F_DIRTY;
    }
    return( JSUI_R_O_K );
}

