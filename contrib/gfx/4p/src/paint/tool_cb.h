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

#ifndef __TOOL_CB_H__
#define __TOOL_CB_H__

/*
 * these callbacks will have the option of drawing to the current active page
 * 	IPAGE * page_active
 * or to the rubberband-page for transient visualization:
 *	IPAGE * page_rubberband
 *
 * the callbacks will be given the x and y on the original image, not 
 * screen coords.
 * 
 * When the screen update happens, it will blit the rubberband differences over
 * the active page to the screen buffer.
 *
 * Imagery applied to the rubberband page will not be applied to the active page
 *
 * some examples:
 *
 *  dotted draw:
 *	IDLE:		apply pixel at x,y to page_rubberband
 *	STARTDOWN:	apply pixel at x,y to page_active
 *	DRAGGING:	apply pixel at x,y to page_active
 *	RELEASE:	ignore
 *
 *  lined draw:
 *	IDLE:		apply pixel at x,y to page_rubberband
 *	STARTDOWN:	'remember' this position, set it as the last position
 *	DRAGGING:	draw a line from the last position to x,y on page_active
 *	RELEASE:	draw a line from the last position to x,y on page_active
 *
 *  rectangle:
 *	IDLE:		apply pixel at x,y to page_rubberband
 *	STARTDOWN:	'remember' this position, set it as the first corner
 *	DRAGGING:	draw rectangle from first corner to x,y on page_rubberband
 * 
 *  Flood Fill:
 *	IDLE:		apply pixel at x,y to page_rubberband
 *	STARTDOWN:	apply fill function at x,y to page_active
 *	DRAGGING:	ignore
 *	RELEASE:	ignore
 */


////////////////////////////////////////////////////////////////////////////////
// callback stuff...

#define TOOL_EVENT_NONE		(0)	/* no event */
#define TOOL_EVENT_INIT         (1)     /* tool was selected */
#define TOOL_EVENT_DEINIT       (2)     /* another tool was selected */
#define TOOL_EVENT_IDLE         (3)     /* tool is idle, only move events */

#define TOOL_EVENT_STARTDOWN    (4)     /* mouse button has been pressed down */
#define TOOL_EVENT_DRAGGING     (5)     /* mouse is moving with button down */
#define TOOL_EVENT_DRAG_STAT	(6)     /* mouse is still with button down */
#define TOOL_EVENT_RELEASE      (7)     /* user released the mouse button */   

#define TOOL_EVENT_COMPLETE	(8)	/* complete the task */
#define TOOL_EVENT_ABORT	(9)	/* abort the task */


#define TOOL_BUTTON_NULL	(0)
#define TOOL_BUTTON1		(1)
#define TOOL_BUTTON2		(2)
#define TOOL_BUTTON3		(3)

#define TOOL_BUTTON_FG		(1)
#define TOOL_BUTTON_BG		(3)


typedef void (*tool_interaction_callback)
(
    int mouse_event,
    int mouse_buttons,
    int keyhit,
    int x,
    int y
);


////////////////////////////////////////////////////////////////////////////////
// the structures and globals we need to care about

// for the 'changes mode' keyword:

#define CB_ONESHOT	(0)
#define CB_TOOL		(1)

typedef struct __tool_cblist{
    int shortcut_key;
    int changes_mode;
    tool_interaction_callback cb_func;
    struct __tool_cblist * next;
} TOOL_CBLIST;

extern TOOL_CBLIST * tool_list;

extern TOOL_CBLIST * tool_current;  // the currently active tool


////////////////////////////////////////////////////////////////////////////////
// list maintenance methods...

// use this to register a callback function...
void
tool_callback_register(
	int shortcut_key, 
	int changes_mode,
	tool_interaction_callback cb_func
);

// use this one to dispose of the list at cleanup time...
void
tool_callback_free(
	void
);


////////////////////////////////////////////////////////////////////////////////
// runtime methods

// use this to switch modes
void 
tool_switch_mode(
	int shortcut_key,
	int ancillary_value
);


void
tool_previous_mode(
        void
);


// use this to call the proper function...
void
tool_event_valve(
	int mouse_event,
	int mouse_buttons,
	int key_hit,
	int x,
	int y
);


#endif
