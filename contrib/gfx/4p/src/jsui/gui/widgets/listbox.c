/*
 * listbox
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


/* jsui_widget_list
 * d1  selected item
 * d2  scroll position
 * dp  pointer to a null-terminated list of strings
 */



int jsui_widget_list		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    PEN * newpen;
    int in_container = 0;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;
    int (*proc)(JSUI_DIALOG *, int, int, int, char *);
    int (*recurse)(int, void *, JSUI_DIALOG *, int); 
    int ret = JSUI_R_O_K;
    int voffs = 0;
    int ccc = 0;
    int argc;
    char ** textlist = NULL;

    recurse = d->proc;

    if ( (jdr->mx >= d->x) && (jdr->mx < d->x+d->w) &&
         (jdr->my >= d->y) && (jdr->my < d->y+d->h) )
	in_container = 1;

    switch(msg)
    {
    case (JSUI_MSG_IDLE):
    case (JSUI_MSG_RDRAG):
    case (JSUI_MSG_LDRAG):
    case (JSUI_MSG_MOUSEMOVE):
	if (in_container)
	{
	    int sd1 = d->d1;
	    int sd2 = d->d2;

	    // d1 = selected item
	    // d2 = scroll position
	    if(jdr->my < 2) d->d1 = d->d2-1;

	    if(jdr->mx > d->w - 15)  d->d1 = -1;

	    d->d1 = d->d2 + (jdr->my -2 -7) / 15;

	    if ( (d->d1 != sd1) || (d->d2 != sd2) )
	    {
		recurse(JSUI_MSG_DRAW, vjdr, d, c);
	    }
	}
	break;

    case (JSUI_MSG_RPRESS):
    case (JSUI_MSG_RRELEASE):
    case (JSUI_MSG_LPRESS):
    case (JSUI_MSG_LRELEASE):
	if (d->dp && d->dp3)
	{
	    proc = (int (*)(JSUI_DIALOG*, int, int, int, char *))d->dp3;
	    textlist = (char **) d->dp;
	    ret = proc(d, jdr->focus_obj, msg, d->d1, textlist[d->d1]);
	}
	break;


    case (JSUI_MSG_TIMETICK):
	break;

    case (JSUI_MSG_GOTFOCUS):
	recurse(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case (JSUI_MSG_LOSTFOCUS):
	recurse(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case (JSUI_MSG_DRAW):
	if (!jdr) return JSUI_R_O_K;

	primitive_3d_rect_down(jdr->buffer,
	    jdr->vbx + d->x, jdr->vby + d->y, 
	    jdr->vbx + d->x + d->w-1,
	    jdr->vby + d->y + d->h-1,
	    JSUI_C_BG);

	// up "button"
	primitive_3d_rect_up(jdr->buffer,
	    jdr->vbx + d->x + d->w-15, 
	    jdr->vby + d->y + 1, 
	    jdr->vbx + d->x + d->w-2,
	    jdr->vby + d->y + 16,
	    JSUI_C_BG);

	// center "area"
	primitive_3d_rect_down(jdr->buffer,
	    jdr->vbx + d->x + d->w-15, 
	    jdr->vby + d->y + 17, 
	    jdr->vbx + d->x + d->w-2,
	    jdr->vby + d->y + d->h - 17,
	    JSUI_C_BG);

	// down "button"
	primitive_3d_rect_up(jdr->buffer,
	    jdr->vbx + d->x + d->w-15, 
	    jdr->vby + d->y + d->h - 16, 
	    jdr->vbx + d->x + d->w-2,
	    jdr->vby + d->y + d->h-2,
	    JSUI_C_BG);

	newpen = Pen_Create();
	if (d->flags & JSUI_F_DISABLED)
	    newpen->Icolor = JSUI_C_DISABLED;
	else
	    newpen->Icolor = JSUI_C_LOLITE;

	font_render_text_center(jdr->buffer, newpen, newpen, 
		jsui_current_font,
		jdr->vbx + d->x + d->w - 8,
		jdr->vby + d->y + 7,
		"^");

	font_render_text_center(jdr->buffer, newpen, newpen, 
		jsui_current_font,
		jdr->vbx + d->x + d->w - 7,
		jdr->vby + d->y + d->h - 12,
		"v");

	if (d->dp)
	{
	    textlist = (char **) d->dp;
	    for (argc = 0 ; textlist[argc] ; argc++); 

	    if (d->d1 >= argc)  d->d1 = argc-1;
	}

	if (newpen && d && jdr->buffer && d->dp)
	{
	    voffs = 5;

	    if (d->d2 < 0) d->d2 = 0;

	    for (argc = 0 ; textlist[argc] ; argc++); 

	    ccc = d->d2;
	    while (textlist[ccc] && 
		   (ccc < (d->h-5)/15)
		  )
	    {
/*
		if (ccc == d->d1)
		    newpen->Icolor = JSUI_C_HILITE;
		else 
		    newpen->Icolor = JSUI_C_LOLITE;
*/
		font_render_text(jdr->buffer, newpen, newpen, 
			jsui_current_font,
			jdr->vbx + d->x + 5,
			jdr->vby + d->y + 5 + (ccc*15),
			textlist[ccc]);
		ccc++;
	    }
	}
	Pen_Destroy(newpen);

	if (d->flags & JSUI_F_GOTFOCUS && !(d->flags & JSUI_F_DISABLED))
	{
	    if ( (d->d1 >= d->d2) && d->dp)
		primitive_rectangle_fourcolor(jdr->buffer, 
		    primitive_pixel_dotted,
		    jdr->vbx + d->x + 2, 
		    jdr->vby + d->y + 2 + ((d->d1 - d->d2) * 15),
		    jdr->vbx + d->x + d->w-1 - 2 - 15,
		    jdr->vby + d->y + ((d->d1 - d->d2) * 15) + 15,
		    -1,
		    JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK);
	}

	jdr->flags |= JSUI_F_DIRTY;
	break;

	case (JSUI_MSG_START):
	    printf("list Start\n");
	    break;

	case (JSUI_MSG_END):
	    printf("list End\n");
	    break;
    }

    return( ret );
}
