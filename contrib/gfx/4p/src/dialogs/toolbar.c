/*
** toolbar.c
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@umlautllama.com
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsui.h"
#include "paint.h"
#include "tool_cb.h"


extern IPAGE toolbar_bitmap;

int
bcb( JSUI_DIALOG * d, int id, int msg )
{
    if (!d) return( JSUI_R_O_K );

    if (d->d1 & JSUI_B_ON_R)
    {
	switch(id)
	{
	case(6): jsui_dialog_broadcast(PAINT_MSG_SETAIRBRUSH, 0); return 0; break;
	case(7): jsui_dialog_broadcast(PAINT_MSG_FILLEDRECTANGLE, 0); return 0; break;
	case(8): jsui_dialog_broadcast(PAINT_MSG_FILLEDCIRCLE, 0); return 0; break;
	case(9): jsui_dialog_broadcast(PAINT_MSG_FILLEDOVAL, 0); return 0; break;
	case(10): jsui_dialog_broadcast(PAINT_MSG_FILLEDPOLYLINE, 0); return 0; break;
	case(11): jsui_dialog_broadcast(PAINT_MSG_OLDBRUSH, 0); return 0; break;
	case(13): jsui_dialog_broadcast(PAINT_MSG_SETGRID, 0); return 0; break;
	case(14): jsui_dialog_broadcast(PAINT_MSG_SETMIRROR, 0); return 0; break;
	case(16): jsui_dialog_broadcast(PAINT_MSG_ZOOMOUT, 0); return 0; break;
	}
    }

    if (d->d1 & JSUI_B_ON_L)
    {
	switch(id)
	{
	case(1): jsui_dialog_broadcast(PAINT_MSG_DOTTEDDRAW, 0); break;
	case(2): jsui_dialog_broadcast(PAINT_MSG_CONTINUOUSDRAW, 0); break;
	case(3): jsui_dialog_broadcast(PAINT_MSG_VECTORDRAW, 0); break;
	case(4): jsui_dialog_broadcast(PAINT_MSG_CURVEDRAW, 0); break;
	case(5): jsui_dialog_broadcast(PAINT_MSG_FLOODFILL, 0); break;
	case(6): jsui_dialog_broadcast(PAINT_MSG_AIRBRUSH, 0); break;
	case(7): jsui_dialog_broadcast(PAINT_MSG_RECTANGLE, 0); break;
	case(8): jsui_dialog_broadcast(PAINT_MSG_CIRCLE, 0); break;
	case(9): jsui_dialog_broadcast(PAINT_MSG_OVAL, 0); break;
	case(10): jsui_dialog_broadcast(PAINT_MSG_POLYLINE, 0); break;
	case(11): jsui_dialog_broadcast(PAINT_MSG_SNAGBRUSH, 0); break;
	case(12): jsui_dialog_broadcast(PAINT_MSG_TEXT, 0); break;
	case(13): jsui_dialog_broadcast(PAINT_MSG_GRID, 1); break;
	case(14): jsui_dialog_broadcast(PAINT_MSG_MIRROR, 1); break;
	case(15): jsui_dialog_broadcast(PAINT_MSG_MAGNIFY, 0); break;
	case(16): jsui_dialog_broadcast(PAINT_MSG_ZOOMIN, 0); break;
	case(17): jsui_dialog_broadcast(PAINT_MSG_UNDO, 0); break;
	case(18): jsui_dialog_broadcast(PAINT_MSG_CLEARSCREEN, 0); break;
	}
    }

    return( JSUI_R_O_K );
}

IPAGE_XREF pr_dotted[3] = {
    { &toolbar_bitmap,  1, 1, 11, 11, 1 },
    { &toolbar_bitmap, 26, 1, 11, 11, 1 },
    { &toolbar_bitmap, 26, 1, 11, 11, 1 }
};

IPAGE_XREF pr_continuous[3] = {
    { &toolbar_bitmap, 13, 1, 11, 11, 1 },
    { &toolbar_bitmap, 38, 1, 11, 11, 1 },
    { &toolbar_bitmap, 38, 1, 11, 11, 1 }
};

IPAGE_XREF pr_vector[3] = {
    { &toolbar_bitmap,  1, 13, 11, 11, 1 },
    { &toolbar_bitmap, 26, 13, 11, 11, 1 },
    { &toolbar_bitmap, 26, 13, 11, 11, 1 }
};

IPAGE_XREF pr_arc[3] = {
    { &toolbar_bitmap, 13, 13, 11, 11, 1 },
    { &toolbar_bitmap, 38, 13, 11, 11, 1 },
    { &toolbar_bitmap, 38, 13, 11, 11, 1 }
};

IPAGE_XREF pr_flood[3] = {
    { &toolbar_bitmap,  1, 25, 11, 11, 1 },
    { &toolbar_bitmap, 26, 25, 11, 11, 1 },
    { &toolbar_bitmap, 26, 25, 11, 11, 1 }
};

IPAGE_XREF pr_air[3] = {
    { &toolbar_bitmap, 13, 25, 11, 11, 1 },
    { &toolbar_bitmap, 38, 25, 11, 11, 1 },
    { &toolbar_bitmap, 38, 25, 11, 11, 1 }
};

IPAGE_XREF pr_rect[3] = {
    { &toolbar_bitmap,  1, 37, 11, 11, 1 },
    { &toolbar_bitmap, 26, 37, 11, 11, 1 },
    { &toolbar_bitmap, 51, 37, 11, 11, 1 }
};

IPAGE_XREF pr_circle[3] = {
    { &toolbar_bitmap, 13, 37, 11, 11, 1 },
    { &toolbar_bitmap, 38, 37, 11, 11, 1 },
    { &toolbar_bitmap, 63, 37, 11, 11, 1 }
};

IPAGE_XREF pr_oval[3] = {
    { &toolbar_bitmap,  1, 49, 11, 11, 1 },
    { &toolbar_bitmap, 26, 49, 11, 11, 1 },
    { &toolbar_bitmap, 51, 49, 11, 11, 1 }
};

IPAGE_XREF pr_poly[3] = {
    { &toolbar_bitmap, 13, 49, 11, 11, 1 },
    { &toolbar_bitmap, 38, 49, 11, 11, 1 },
    { &toolbar_bitmap, 63, 49, 11, 11, 1 }
};

IPAGE_XREF pr_brush[3] = {
    { &toolbar_bitmap,  1, 61, 11, 11, 1 },
    { &toolbar_bitmap, 26, 61, 11, 11, 1 },
    { &toolbar_bitmap, 26, 61, 11, 11, 1 }
};

IPAGE_XREF pr_text[3] = {
    { &toolbar_bitmap, 13, 61, 11, 11, 1 },
    { &toolbar_bitmap, 38, 61, 11, 11, 1 },
    { &toolbar_bitmap, 38, 61, 11, 11, 1 }
};

IPAGE_XREF pr_grid[3] = {
    { &toolbar_bitmap,  1, 73, 11, 11, 1 },
    { &toolbar_bitmap, 26, 73, 11, 11, 1 },
    { &toolbar_bitmap, 26, 73, 11, 11, 1 }
};
IPAGE_XREF pr_mirror[3] = {
    { &toolbar_bitmap, 13, 73, 11, 11, 1 },
    { &toolbar_bitmap, 38, 73, 11, 11, 1 },
    { &toolbar_bitmap, 38, 73, 11, 11, 1 }
};

IPAGE_XREF pr_mag[3] = {
    { &toolbar_bitmap,  1, 85, 11, 11, 1 },
    { &toolbar_bitmap, 26, 85, 11, 11, 1 },
    { &toolbar_bitmap, 26, 85, 11, 11, 1 }
};
IPAGE_XREF pr_zoom[3] = {
    { &toolbar_bitmap, 13, 85, 11, 11, 1 },
    { &toolbar_bitmap, 38, 85, 11, 11, 1 },
    { &toolbar_bitmap, 38, 85, 11, 11, 1 }
};

IPAGE_XREF pr_undo[3] = {
    { &toolbar_bitmap,  1, 97, 11, 11, 1 },
    { &toolbar_bitmap, 26, 97, 11, 11, 1 },
    { &toolbar_bitmap, 26, 97, 11, 11, 1 }
};
IPAGE_XREF pr_clear[3] = {
    { &toolbar_bitmap, 13, 97, 11, 11, 1 },
    { &toolbar_bitmap, 38, 97, 11, 11, 1 },
    { &toolbar_bitmap, 38, 97, 11, 11, 1 }
};

JSUI_DIALOG toolbar_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)
                (key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
    { jsui_widget_frame, 0, 63, 51, 225, 0, 0, 
		JSUI_FRAME_EAST, 0, "", NULL, NULL  },
    // SD  VC  Fa  RC  EP  BA  G/  MZ  UK

    // dotted draw
    {jsui_widget_imagebutton,  1, 1, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_dotted, NULL, (void *) bcb },
    // continuous draw
    {jsui_widget_imagebutton, 26, 1, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_continuous, NULL, (void *) bcb },

    // vector
    {jsui_widget_imagebutton,  1,25, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_vector, NULL, (void *) bcb },
    // arc
    {jsui_widget_imagebutton, 26,25, 24,24, 0, JSUI_F_DISABLED, 
				JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_arc, NULL, (void *) bcb },

    // floodfill
    {jsui_widget_imagebutton,  1,50, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_flood, NULL, (void *) bcb },
    // airbrush
    {jsui_widget_imagebutton, 26,50, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_air, NULL, (void *) bcb },

    // rectangle
    {jsui_widget_imagebutton,  1,75, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_rect, NULL, (void *) bcb },
    // circle
    {jsui_widget_imagebutton, 26,75, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_circle, NULL, (void *) bcb },

    // ellipse
    {jsui_widget_imagebutton,  1,100, 24,24, 0, JSUI_F_DISABLED,
				JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_oval, NULL, (void *) bcb },
    // polyline
    {jsui_widget_imagebutton, 26,100, 24,24, 0, JSUI_F_DISABLED,
				JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_poly, NULL, (void *) bcb },

    // brush snagger
    {jsui_widget_imagebutton, 1,125, 24,24, 0, 0, 0,
				0, pr_brush, NULL, (void *) bcb },
    // Text tool
    {jsui_widget_imagebutton,26,125, 24,24, 0, 0, JSUI_B_RADIO|JSUI_B_STICKY, 
				1, pr_text, NULL, (void *) bcb },

    // grid tool
    {jsui_widget_imagebutton, 1,150, 24,24, 0, JSUI_F_DISABLED,
				JSUI_B_STICKY, 
				0, pr_grid, NULL, (void *) bcb },
    // mirror tool
    {jsui_widget_imagebutton,26,150, 24,24, 0, JSUI_F_DISABLED,
				JSUI_B_STICKY, 
				0, pr_mirror, NULL, (void *) bcb },

    // Magnify
    {jsui_widget_imagebutton, 1,175, 24,24, 0, JSUI_F_DISABLED, 0,
				0, pr_mag, NULL, (void *) bcb },
    // Zoom
    {jsui_widget_imagebutton,26,175, 24,24, 0, JSUI_F_DISABLED, 0,
				0, pr_zoom, NULL, (void *) bcb },

    // undo
    {jsui_widget_imagebutton, 1,200, 24,24, 0, 0, 
				0,
				0, pr_undo, NULL, (void *) bcb },
    // clear
    {jsui_widget_imagebutton,26,200, 24,24, 0, 0,
				0,
				0, pr_clear, NULL, (void *) bcb },

    JSUI_DLG_END
};

