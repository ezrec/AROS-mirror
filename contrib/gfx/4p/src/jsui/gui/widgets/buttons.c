/*
 * buttons
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


/* jsui_widget_button
 *  
 * dp   pointer to text for "off"
 * dp2  pointer to text for "on" 
 * dp3  pointer to callback function. 
 * d1   button flags
 * d2   radio group
 */             
int jsui_widget_button		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    PEN * newpen;
    int in_container = 0;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;
    int (*proc)(JSUI_DIALOG *, int, int); 
    int (*recurse)(int, void *, JSUI_DIALOG *, int); 
    int ret = JSUI_R_O_K;
    int left = 0;

    recurse = d->proc;

    if ( (jdr->mx >= d->x) && (jdr->mx < d->x+d->w) &&
         (jdr->my >= d->y) && (jdr->my < d->y+d->h) )
	in_container = 1;

    switch(msg)
    {
    case (JSUI_MSG_TRIGGER_L):
    case (JSUI_MSG_LRELEASE):
	left = 1;

    case (JSUI_MSG_TRIGGER_R):
    case (JSUI_MSG_RRELEASE):
	if (d->flags & JSUI_F_DISABLED) break;
	if ( in_container 
		|| msg == JSUI_MSG_TRIGGER_R 
		|| msg == JSUI_MSG_TRIGGER_L)
	{
	    if (d->d1 & JSUI_B_RADIO)
	    {
		// radio button
		if (d->d2)
		{
		    ret = jsui_dialog_send_message(jdr->dialog, 
				JSUI_MSG_RADIO, d->d2 );
		}

		d->d1 |= (left)? JSUI_B_ON_L : JSUI_B_ON_R;

	    } else {
		// regular button - toggle state
		if (d->d1 & JSUI_B_STICKY)
		{
		    if (d->d1 & JSUI_B_ON)
			d->d1 &= ~JSUI_B_ON;
		    else
			d->d1 |= (left)?  JSUI_B_ON_L : JSUI_B_ON_R;
		} else {
		    d->d1 |= (left)? JSUI_B_ON_L : JSUI_B_ON_R;
		}
	    }
	    recurse(JSUI_MSG_DRAW, vjdr, d, c);
            
            if (d->dp3 && 
		(msg == JSUI_MSG_RRELEASE || msg == JSUI_MSG_LRELEASE)
	       )
            {
                proc = (int (*)(JSUI_DIALOG*, int, int))d->dp3;
            	ret = proc(d, jdr->focus_obj, msg);
            }

	    if (!(d->d1 & JSUI_B_STICKY))
	    {
		d->d1 &= ~JSUI_B_ON;
		recurse(JSUI_MSG_DRAW, vjdr, d, c);
	    }
	    return( ret );
        }
	break;

#ifdef NEVER
    case (JSUI_MSG_RRELEASE):
	if ( d->flags & JSUI_F_DISABLED ) break;
	if ( in_container )
	{
            if (d->dp3)
            {
                proc = (int (*)(JSUI_DIALOG*, int, int))d->dp3;
            	ret = proc(d, jdr->focus_obj, msg);
            	return ( ret );
            }
	}
	break;
#endif

    case (JSUI_MSG_RPRESS):
    case (JSUI_MSG_RDRAG):
	if (d->flags & JSUI_F_DISABLED) break;

	if ( in_container )
	{
	    d->d1 |= JSUI_B_FORCEON_R;
	    recurse(JSUI_MSG_DRAW, vjdr, d, c);
	    d->d1 &= ~JSUI_B_FORCEON_R;
	} else {
	    recurse(JSUI_MSG_DRAW, vjdr, d, c);
	    return( JSUI_R_O_K ); 
	}
	break;

    case (JSUI_MSG_LPRESS):
    case (JSUI_MSG_LDRAG):
	if (d->flags & JSUI_F_DISABLED) break;

	if ( in_container )
	{
	    d->d1 |= JSUI_B_FORCEON_L;
	    recurse(JSUI_MSG_DRAW, vjdr, d, c);
	    d->d1 &= ~JSUI_B_FORCEON_L;
	} else {
	    recurse(JSUI_MSG_DRAW, vjdr, d, c);
	    return( JSUI_R_O_K ); 
	}
	break;

    case (JSUI_MSG_RADIO):
	if ((d->d2) && (c == d->d2))
	{
	    d->d1 &= ~JSUI_B_ON;
	    recurse(JSUI_MSG_DRAW, vjdr, d, c);
            if (d->dp3)
            {
                proc = (int (*)(JSUI_DIALOG*, int, int))d->dp3;
            	ret =  proc(d, jdr->focus_obj, msg);
            	return ( ret );
            }
	}
	break;

    case (JSUI_MSG_TIMETICK):
	printf("Button Time Tick\n");
	break;

    case (JSUI_MSG_GOTFOCUS):
	recurse(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case (JSUI_MSG_LOSTFOCUS):
	recurse(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case (JSUI_MSG_DRAW):
	if (!jdr) return JSUI_R_O_K;

	if ( (d->d1 & JSUI_B_ON && d->d1 & JSUI_B_STICKY)  ||
		d->d1 & JSUI_B_FORCEON_L ||
		d->d1 & JSUI_B_FORCEON_R
	   )
	{ 
	    primitive_3d_rect_down(jdr->buffer,
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		JSUI_C_BG);
	} else {
	    primitive_3d_rect_up(jdr->buffer, 
		jdr->vbx + d->x, jdr->vby + d->y, 
		jdr->vbx + d->x + d->w-1,
		jdr->vby + d->y + d->h-1,
		JSUI_C_BG);
	}

	newpen = Pen_Create();
	if (d->flags & JSUI_F_DISABLED)
	    newpen->Icolor = JSUI_C_DISABLED;
	else
	    newpen->Icolor = JSUI_C_LOLITE;
	if (newpen && d && jdr->buffer)
	{
	    font_render_text_center(jdr->buffer, newpen, newpen, 
			jsui_current_font,
			jdr->vbx + d->x + (d->w/2),
			jdr->vby + d->y + (d->h/2) - jsui_current_font->h/2,
			(d->d1 & JSUI_B_ON)?((d->dp2)?d->dp2:d->dp):d->dp);
	    Pen_Destroy(newpen);
	}

	if (d->flags & JSUI_F_GOTFOCUS && !(d->flags & JSUI_F_DISABLED))
	{
	    primitive_rectangle_fourcolor(jdr->buffer, 
		    primitive_pixel_dotted,
		    jdr->vbx + d->x + 2, 
		    jdr->vby + d->y + 2,
		    jdr->vbx + d->x + d->w-1 - 2,
		    jdr->vby + d->y + d->h-1 - 2,
		    -1,
		    JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK);
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
	break;

#ifdef NEVER
	case (JSUI_MSG_START):
	    printf("Button Start\n");
	    break;

	case (JSUI_MSG_END):
	    printf("Button End\n");
	    break;

	case (JSUI_MSG_KEY):
	    printf("Button Key\n");
	    break;

	case (JSUI_MSG_CHAR):
	    printf("Button Char\n");
	    break;

	case (JSUI_MSG_UCHAR):
	    printf("Button UChar\n");
	    break;

	case (JSUI_MSG_XCHAR):
	    printf("Button XChar\n");
	    break;

	case (JSUI_MSG_WANTFOCUS):
	    printf("Button Want Focus\n");
	    break;
#endif
    }

    return( ret );
}


#ifdef NEVEr
/*
 *  jsui_widget_imagebutton 
 * dp   pointer to an array of ref structures (idle)
 *         dp[0] = idle
 *         dp[1] = left click
 *         dp[2] = right click
 *         (set dp to null for no image (which would be stupid))
 *         (set elements to NULL for no image)
 * d1   same as above
 * d2   same as above
 */ 

typedef struct pageref { 
    IPAGE * img_buffer;
    int x;      // x offset to start of structure
    int y;      // y offset to start of structure
    int w;      // width of display area 
    int h;      // height of display area
    int pd;     // post-double the pixels. (after range clip, double the size)  
};
#endif

int jsui_widget_imagebutton(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    if (msg == JSUI_MSG_DRAW)
    {
	if (!jdr) return JSUI_R_O_K;

	// draw the bitmap
	if (d->dp)
	{
	    IPAGE_XREF *ix = (IPAGE_XREF *) d->dp;
	    int ii;

	    if (d->d1 & JSUI_B_ON_L)  ii = 1;
	    else  if (d->d1 & JSUI_B_ON_R)  ii = 2; 
	    else  ii = 0;

	    if (d->d1 & JSUI_B_FORCEON_R)  ii = 2;
	    if (d->d1 & JSUI_B_FORCEON_L)  ii = 1;

	    if (ix[ii].pd == 1) {
		Page_DoubleBlit_Image(
		    jdr->buffer,
		    jdr->vbx + d->x + 1,
		    jdr->vby + d->y + 1,
		    ix[ii].img_buffer,
		    ix[ii].x,  ix[ii].y,
		    ix[ii].x + ix[ii].w, 
		    ix[ii].y + ix[ii].h
		);
	    } else {
		Page_Blit_Image(
		    jdr->buffer,
		    jdr->vbx + d->x + 1,
		    jdr->vby + d->y + 1,
		    ix[ii].img_buffer,
		    ix[ii].x,  ix[ii].y,
		    ix[ii].x + ix[ii].w, 
		    ix[ii].y + ix[ii].h
		);
	    } 
	}

	// draw the 3dbox
	if (  (!(d->flags & JSUI_F_DISABLED))
	    &&(!(d->flags & JSUI_F_NOFRAME))
	    )
	{
	    if ( (d->d1 & JSUI_B_ON && d->d1 & JSUI_B_STICKY)  ||
		    d->d1 & JSUI_B_FORCEON_L ||
		    d->d1 & JSUI_B_FORCEON_R
	       )
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

	// draw the focus box
	if (d->flags & JSUI_F_GOTFOCUS 
	    && (!(d->flags & JSUI_F_DISABLED))
	    && (!(d->flags & JSUI_F_NOFOCUSBOX))
	    )
	{
	    primitive_rectangle_fourcolor(jdr->buffer, 
		    primitive_pixel_dotted,
		    jdr->vbx + d->x + 2, 
		    jdr->vby + d->y + 2,
		    jdr->vbx + d->x + d->w-1 - 2,
		    jdr->vby + d->y + d->h-1 - 2,
		    -1,
		    JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_DARK);
	}

	if (d->flags & JSUI_F_DISABLED)
	{
	    primitive_rectangle_checkerboard(
		    jdr->buffer,
		    jdr->vbx + d->x + 1, 
		    jdr->vby + d->y + 1,
		    jdr->vbx + d->x + d->w - 1,
		    jdr->vby + d->y + d->h - 1,
		    JSUI_C_BG
	    );
	}

	
	jdr->flags |= JSUI_F_DIRTY;
    } else {
	return( jsui_widget_button(msg, vjdr, d, c) );
    }
    return( JSUI_R_O_K );
}

