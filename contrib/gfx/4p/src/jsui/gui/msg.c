/*
** activelist.c
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
#include "active.h"


// tell the dialog something
int
jsui_dialog_send_message (
	JSUI_DIALOG * dlg, 
	int msg,
	int c
)
{
    JSUI_DIALOG_RUNNING * jdr;
    int pos;

    if (!dlg)
	return JSUI_R_O_K;

    jdr = jsui_dialog_find(dlg);
    if (!jdr)
	return JSUI_R_O_K;

    for (pos=0 ; dlg[pos].proc ; pos++)
    {
	dlg[pos].proc(msg, jdr, &dlg[pos], c);
    }

    return JSUI_R_O_K;
}


// tell the dialog something specific
int
jsui_dialog_tell_msginfo (
	JSUI_DIALOG * basedlg,
	int which_one,
	int msg,
	int c
)
{
    JSUI_DIALOG_RUNNING * jdr;
    int ret;

    if (!basedlg)
	return JSUI_R_O_K;
    
    jdr = jsui_dialog_find(basedlg);
    if (!jdr)
        return JSUI_R_O_K;

    if (basedlg[which_one].proc)
	ret = basedlg[which_one].proc(msg, jdr, &basedlg[which_one], c);

    return( ret );
}

// tell the dialog something
int
jsui_dialog_tell_message (
	JSUI_DIALOG * basedlg,
	int which_one,
	int msg
)
{
    return( jsui_dialog_tell_msginfo(basedlg, which_one, msg, 0) );
}


void
jsui_dialog_broadcast(
	int msg,
	int value
)
{
    JSUI_DIALOG_RUNNING * jdrtmp = jsui_active_list;
    int dpos;

    while (jdrtmp->next)
    {
	jdrtmp = jdrtmp->next;
    }

    while (jdrtmp)
    {
	for(dpos=0 ; jdrtmp->dialog[dpos].proc ; dpos++)
	    jsui_dialog_send_message(&jdrtmp->dialog[dpos], msg, value);
	jdrtmp = jdrtmp->prev;
    }
    if (_fallback)  _fallback(msg,value,0,0);
}
