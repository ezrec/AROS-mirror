/*
 * tool_cb
 * 
 *   callback mechanism for drawing tools
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

#include <stdio.h>
#include <stdlib.h>
#include "jsui.h"
#include "tool_cb.h"

////////////////////////////////////////////////////////////////////////////////

TOOL_CBLIST * tool_list = NULL;     // the list of all available tools

TOOL_CBLIST * tool_current = NULL;  // the currently active tool

TOOL_CBLIST * tool_previous = NULL;  // the previously active tool

////////////////////////////////////////////////////////////////////////////////


// for when we call the valve, we want to respond with the last buttons, x & y
int cb_lastb = 0;
int cb_lastx = 0;
int cb_lasty = 0;

// use this to register a callback function...
void
tool_callback_register(
	int shortcut_key, 
	int changes_mode,
	tool_interaction_callback cb_func
)
{
    TOOL_CBLIST * tl;

    if (!cb_func) return;

    // malloc space
    tl = (TOOL_CBLIST *) malloc (sizeof(TOOL_CBLIST));

    // fill it with data
    tl->shortcut_key = shortcut_key;
    tl->cb_func = cb_func;
    tl->changes_mode = changes_mode;
    tl->next = tool_list;

    // pop it on the list
    tool_list = tl;

    if (!tool_current)  tool_current = tool_list;
}



// use this one to dispose of the list at cleanup time...
void
tool_callback_free(
	void
)
{
    TOOL_CBLIST * tl;

    while (tool_list)
    {
	tl = tool_list;
	tool_list = tool_list->next;
	free (tl);
    }
}




// use this to switch modes
void 
tool_switch_mode(
        int shortcut_key,
	int ancillary_value
)
{
    TOOL_CBLIST * tl = tool_list;
    TOOL_CBLIST * tbak;

    while (tl)
    {
	if (shortcut_key == tl->shortcut_key)
	{
	    if (shortcut_key >= JSUI_MSG_USER)
	    {
		shortcut_key = ancillary_value;
	    }

	    if (tl->changes_mode == CB_TOOL)
	    {
		// we change the mode of the system
		tool_event_valve(TOOL_EVENT_DEINIT, cb_lastb, 0, 
					cb_lastx, cb_lasty);
		tool_previous = tool_current;
		tool_current = tl;
		tool_event_valve(TOOL_EVENT_INIT, cb_lastb, shortcut_key, 
					cb_lastx, cb_lasty);
	    } else {
		// we do a one-shot event, like a requestor, etc.
		// EDIT MODE DOES NOT CHANGE.
		tbak = tool_current;
		tool_current = tl;
		tool_event_valve(TOOL_EVENT_INIT, cb_lastb, shortcut_key, 
					cb_lastx, cb_lasty);
		tool_current = tbak;
	    }
	    return;
	}
	tl = tl->next;
    }
}


// for forcing back to the previous tool...
void
tool_previous_mode(
	void
)
{
    TOOL_CBLIST * temp;

    tool_event_valve(TOOL_EVENT_DEINIT, cb_lastb, 
			tool_current->shortcut_key, 
			cb_lastx, cb_lasty);
    temp = tool_current;
    tool_current = tool_previous;
    tool_previous = temp;
    tool_event_valve(TOOL_EVENT_INIT, cb_lastb, 
			tool_current->shortcut_key, 
			cb_lastx, cb_lasty);
}


// use this to call the proper function...
void 
tool_event_valve(
        int mouse_event,
        int mouse_buttons,
	int keyhit,
        int x,
        int y
)
{
    cb_lastb = mouse_buttons;
    cb_lastx = x;
    cb_lasty = y;

    if (tool_current)
    {
	if (tool_current->cb_func)
	{
	   tool_current->cb_func(mouse_event, mouse_buttons, keyhit, x, y);
	}
    }
}

