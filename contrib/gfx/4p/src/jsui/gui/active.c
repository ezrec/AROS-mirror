/*
** active
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

/*
 * another linked list library.
 *  how many times must i write this?
 */

#include <stdlib.h>
#include "jsui.h"

JSUI_DIALOG_RUNNING * jsui_active_list = NULL;

void
jsui_dialog_list_free(
    void
)
{
    JSUI_DIALOG_RUNNING * jdrtmp = jsui_active_list;

    while (jsui_active_list)
    {
	jdrtmp = jsui_active_list;

	jsui_active_list = jsui_active_list->next; /* it helps to do this before you free it */

        if (jdrtmp->buffer)
            Page_Destroy(jdrtmp->buffer);
	if (jdrtmp)
            free(jdrtmp);
    }
    
    jsui_active_list = NULL;
}


JSUI_DIALOG_RUNNING *
jsui_dialog_find(
    JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * jdrtmp = jsui_active_list;

    while (jdrtmp)
    {
	if (jdrtmp->dialog == dialog)
	    return jdrtmp;

	jdrtmp = jdrtmp->next;
    }
    return NULL;
}

int
jsui_is_running(
	JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * jdr = jsui_dialog_find( dialog );

    if (jdr == NULL)  return( 0 );
    return( 1 );
}


#define JSUI_FRAME_THICKNESS	(1)
#define JSUI_FRAME_DRAGBAR_SIZE	(12)


int
jsui_dialog_run(
    JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * newjdr;

    newjdr = jsui_dialog_find(dialog);
    if (newjdr)
    {
	jsui_dialog_raise(dialog);
    } else {

	newjdr = (JSUI_DIALOG_RUNNING *)malloc(sizeof(JSUI_DIALOG_RUNNING));

	if (!newjdr) return -1;

	newjdr->dialog = dialog;
	newjdr->buffer = Page_Create(dialog->w + (2 * JSUI_FRAME_THICKNESS),
				     dialog->h + JSUI_FRAME_DRAGBAR_SIZE +
				     (2 * JSUI_FRAME_THICKNESS) ,32);
	newjdr->flags  = JSUI_F_DIRTY;
	newjdr->x      = dialog->x;
	newjdr->y      = dialog->y;
	newjdr->focus_obj = 0;
	newjdr->vbx    = JSUI_FRAME_THICKNESS;
	newjdr->vby    = JSUI_FRAME_THICKNESS+JSUI_FRAME_DRAGBAR_SIZE;
	newjdr->mx     = -1;
	newjdr->my     = -1;
	newjdr->mb     = 0;

	newjdr->prev   = NULL;
	newjdr->next   = jsui_active_list;
	if (jsui_active_list)
	{
	    jsui_active_list->prev = newjdr;
	}
	jsui_active_list = newjdr;
    }

    newjdr->flags |= JSUI_F_DIRTY;

    (void) jsui_dialog_send_message(dialog, JSUI_MSG_START, 0);
    (void) jsui_dialog_send_message(dialog, JSUI_MSG_DRAW, 0);

    return 0;
}


int
jsui_dialog_run_modal(
    JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * newjdr;
    int retval = jsui_dialog_run( dialog );

    newjdr = jsui_dialog_find(dialog);
    if (newjdr)
    {
	newjdr->flags |= JSUI_F_MODAL;
    }
    return retval;
}


JSUI_DIALOG_RUNNING *
__jsui_dr_find_and_remove(
    JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * jdr = jsui_dialog_find(dialog);
    if (!jdr) return NULL;

    // it was found.  remove it from the list.
    if (jdr->next)
    {
	jdr->next->prev = jdr->prev;
    }

    if (jdr->prev)
    {
	jdr->prev->next = jdr->next;
    }

    if( jdr == jsui_active_list )
    {
	jsui_active_list = jdr->next;
    }

    jdr->prev = NULL;
    jdr->next = NULL;

    return(jdr);
}


void 
jsui_dialog_close(
    JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * jdr;

    jdr = __jsui_dr_find_and_remove(dialog);

    if (!jdr) return;
    (void) jsui_dialog_send_message(dialog, JSUI_MSG_END, 0);

    if (jsui_active_background_page)
    {
	Page_DR_Dirtify(jsui_active_background_page,
			jdr->x, jdr->y,
			jdr->x+jdr->buffer->w,
			jdr->y+jdr->buffer->h);
    }

    if (jdr->buffer)
        Page_Destroy(jdr->buffer);
    free(jdr);
}


void
jsui_dialog_raise(
    JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * jdr;

    jdr = __jsui_dr_find_and_remove(dialog);
    if (!jdr) return;

    // then add it at the top
    jdr->prev = NULL;
    jdr->next = jsui_active_list;

    if (jsui_active_list)
        jsui_active_list->prev = jdr;

    jsui_active_list = jdr;

    (void) jsui_dialog_send_message(dialog, JSUI_MSG_DRAW, 0);
    jdr->flags |= JSUI_F_DIRTY;
}


void
jsui_dialog_lower(
    JSUI_DIALOG * dialog
)
{
    JSUI_DIALOG_RUNNING * jdr;
    JSUI_DIALOG_RUNNING * tjdr;
    
    jdr = __jsui_dr_find_and_remove(dialog);
    if (!jdr) return;

    // now find the end of the list...
    tjdr = jsui_active_list;
    if (tjdr)
    {
	while (tjdr->next)
	{
	    tjdr = tjdr->next;
	}

	// and shove it at the bottom
	tjdr->next = jdr;
	jdr->prev = tjdr;
    } else {
	jsui_active_list = jdr;
    }
    jsui_active_list->flags |= JSUI_F_DIRTY;
}

