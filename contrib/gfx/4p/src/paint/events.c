
/*
** events.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "zoom.h"
#include "cursors.h"


////////////////////////////////////////////////////////////////////////////////

int last_paint_bstate = 0;
int last_paint_x = 0;
int last_paint_y = 0;

void
PaintFilterEvents(
        int event,
        int vx,
        int vy,
        int vb
)
{
    // this only takes the basic UI inputs, massages them a little, then
    // makes one of two types of calls:
    //		tool_event_valve( ... )		 -- on mouse input
    //		tool_switch_mode( ... )		 -- on keyboard input

    int px = zoom_actual_to_zoomed_X( vx );
    int py = zoom_actual_to_zoomed_Y( vy );
    
    if (pen_cycle)
    {
        pen_cycle->Icolor = timing_ticks & 0x01f;

    }

#ifdef EXTRA_OUTPUT_WANTED    
    if (event != JSUI_MSG_IDLE)
    printf("%d   (%d, %d) [%c%c%c]\n",	event, vx, vy,
            (vb&4)?'L':' ',
            (vb&2)?'M':' ',
            (vb&1)?'R':' ');
#endif

    // massage the button to be something useful to us...
    if (vb&4) vb = TOOL_BUTTON1;
    else if (vb&2) vb = TOOL_BUTTON2;
    else if (vb&1) vb = TOOL_BUTTON3;
    
    switch(event)
    {
        case(JSUI_MSG_IDLE):	// user is doing nothing
            tool_event_valve(TOOL_EVENT_IDLE, 0,
                                tool_current->shortcut_key, px, py);
            break;
            
        case(JSUI_MSG_LPRESS):	// user has just pressed a button
        case(JSUI_MSG_RPRESS):
            if (last_paint_bstate == vb)
            {
                tool_event_valve(TOOL_EVENT_DRAG_STAT, last_paint_bstate, 
				tool_current->shortcut_key, px, py);
            } else {
                last_paint_bstate = vb;
                tool_event_valve(TOOL_EVENT_STARTDOWN, last_paint_bstate, 
				tool_current->shortcut_key, px, py);
            }
            break;
        
        case(JSUI_MSG_LDRAG):	// user has a button down and is dragging it
        case(JSUI_MSG_RDRAG):
            if (px == last_paint_x && py == last_paint_y)
            {
                tool_event_valve(TOOL_EVENT_DRAG_STAT, last_paint_bstate,
                                tool_current->shortcut_key, px, py);
            } else {
                tool_event_valve(TOOL_EVENT_DRAGGING, last_paint_bstate,
                                tool_current->shortcut_key, px, py);
            }
            break;
            
        case(JSUI_MSG_LRELEASE): // user has released a button
        case(JSUI_MSG_RRELEASE):
                tool_event_valve(TOOL_EVENT_RELEASE, last_paint_bstate, 
				tool_current->shortcut_key, px, py);
                last_paint_bstate = 0;
            break;
        
        case(JSUI_MSG_CHAR):	// user pressed a key
                if (jsui_key_shift)
                {
                    switch(vx)
                    {
                        case('['):
                            tool_switch_mode('{', vx);
                            break;
        
                        case(']'):
                            tool_switch_mode('}', vx);
                            break;
        
                        default:
                            tool_switch_mode(toupper(vx), vx);
                            break;
                    }
                } else {
                    tool_switch_mode(vx, vx);
                }
            break;
        
        case(JSUI_MSG_MOUSEMOVE): // user is moving the mouse (no buttons)
            tool_event_valve(TOOL_EVENT_IDLE, vb, 
                            tool_current->shortcut_key, px, py);
	    break;

	default:
	    if (event >= JSUI_MSG_USER)
	    {
		tool_switch_mode(event, vx);
	    }
	    break;
    }
    last_paint_x = px;
    last_paint_y = py;
}
