/*
 * palette widget
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
#include "jsui.h"

/* jsui_widget_colordisplay
 *
 * d->d1 fg color
 * d->d2 bg color
 * d->dp  callback function of the form:
                void function(int msg, JSUI_DIALOG * d)
 */ 

int jsui_widget_colordisplay		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    PEN * apen = NULL;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    switch(msg)
    {
    case(JSUI_MSG_START):
	break;

    case(JSUI_MSG_END):
	break;

    case(JSUI_MSG_RPRESS):
    case(JSUI_MSG_LPRESS):
	if (d->dp)
	   (*(void (*)(int, JSUI_DIALOG *)) d->dp) (msg, d);
	jsui_widget_colordisplay(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_GOTFOCUS):
	jsui_widget_colordisplay(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_LOSTFOCUS):
	jsui_widget_colordisplay(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_CHAR):
	break;

    case(JSUI_MSG_DRAW):
	// background section
	primitive_3d_rect_down(jdr->buffer, 
	    jdr->vbx + d->x, jdr->vby + d->y, 
	    jdr->vbx + d->x + d->w-1,
	    jdr->vby + d->y + d->h-1,
	    d->d2);

	// foreground section
#ifdef FG3DBOX
	primitive_3d_rect_up(jdr->buffer, 
	    jdr->vbx + d->x          + (d->w/4), 
	    jdr->vby + d->y          + (d->h/4), 
	    jdr->vbx + d->x + d->w-1 - (d->w/4),
	    jdr->vby + d->y + d->h-1 - (d->h/4),
	    d->d1);
#endif
	apen = Pen_Create();
	apen->Icolor = d->d1;

#ifdef FGCRAPPYCIRCLE
	primitive_circle_hollow(
		jdr->buffer, apen,
		primitive_pixel,
		jdr->vbx + d->x + d->w/2,
		jdr->vby + d->y + d->h/2,
		(MIN(d->w, d->h) / 2) - 3
	    );

	primitive_floodfill(
		jdr->buffer, apen,
		jdr->vbx + d->x + d->w/2,
		    jdr->vby + d->y + d->h/2
	    );
#endif
	primitive_rectangle_filled(
	    jdr->buffer, apen,
            jdr->vbx + d->x          + (d->w/4),
            jdr->vby + d->y          + (d->h/4),
            jdr->vbx + d->x + d->w-1 - (d->w/4),
            jdr->vby + d->y + d->h-1 - (d->h/4)
	    );

	Pen_Destroy(apen);
	jdr->flags |= JSUI_F_DIRTY;
    }
    return JSUI_R_O_K;
}




/* jsui_widget_colorpicker
 *
 * d->d1 number of colors
 * d->d2 selected color
 * d->dp  callback function of the form:  
                void function(int msg, JSUI_DIALOG * d, int colornumber)
 * d->dp2 pointer to a jsui_widget_colordisplay
 */

int jsui_widget_colorpicker		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;
    int x,y;
    int cx = -44;
    int cy = -44;
    int xoffs, yoffs;
    int pw, ph;
    int color;
    PEN * apen;

    switch(msg)
    {
    case(JSUI_MSG_START):
	break;

    case(JSUI_MSG_END):
	break;

    case(JSUI_MSG_LPRESS):
    case(JSUI_MSG_RPRESS):
	pw = (d->w - 2)/4;
	ph = (d->h - 2)/8;
	color = 0;

	if (   ( jdr->mx-d->x < 1 || jdr->mx-d->x > d->w-2 )
	    || ( jdr->my-d->y < 1 || jdr->my-d->y > d->h-2 )
	    ) break;

	color = (((jdr->mx-d->x-1) / pw) * 8) + ((jdr->my-d->y-1) / ph);

	if (d->dp)
	{
	   (*(void (*)(int, JSUI_DIALOG *, int)) d->dp) (msg, d, color);
	}
	jsui_widget_colorpicker(JSUI_MSG_DRAW, vjdr, d, c);

	if (d->dp2)
	{
	    JSUI_DIALOG * dp = (JSUI_DIALOG *) d->dp2;
	    if( msg == JSUI_MSG_LPRESS )
		dp->d1 = color;
	    else
		dp->d2 = color;
	    jsui_widget_colordisplay(JSUI_MSG_DRAW, vjdr, dp, 0);
	}
	break;

    case(JSUI_MSG_GOTFOCUS):
	jsui_widget_colorpicker(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_LOSTFOCUS):
	jsui_widget_colorpicker(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_CHAR):
	break;

    case(JSUI_MSG_DRAW):
	// background section
	primitive_3d_rect_down(jdr->buffer, 
	    jdr->vbx + d->x, jdr->vby + d->y, 
	    jdr->vbx + d->x + d->w-1,
	    jdr->vby + d->y + d->h-1,
	    JSUI_C_BG);

	pw = (d->w - 2)/4;
	ph = (d->h - 2)/8;

	apen = Pen_Create();
	for( x=0 ; x < 4 ; x++ )
	{
	    xoffs = jdr->vbx + d->x + (x * pw) + 1;
	   
	    for( y=0 ; y < 8 ; y++ )
	    {
		yoffs = jdr->vby + d->y + (y * ph) + 1;

		primitive_rectangle_filled(
		    jdr->buffer, 
		    apen,
		    xoffs,
		    yoffs,
		    xoffs+pw-1,
		    yoffs+ph-1
		);

		if (apen->Icolor == d->d2)
		{
		    cx = xoffs; 
		    cy = yoffs; 
		}
		apen->Icolor++;
	    }
	}
	if (cx != -44) /* no need to check cy also.. */
	{
	    apen->Icolor = 1;
	    primitive_rectangle_hollow(
			jdr->buffer, apen, primitive_pixel,
			cx-1, cy-1, cx+pw, cy+ph );
	}

	Pen_Destroy(apen);
	jdr->flags |= JSUI_F_DIRTY;
    }
    return JSUI_R_O_K;
}
