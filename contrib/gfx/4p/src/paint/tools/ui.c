/*
 * ui
 * 
 *   user interface toggle callback handlers
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
#include "SDL.h"
#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "cursors.h"

////////////////////////////////////////////////////////////////////////////////
// ui toggles

void
tools_ui_toggles (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf ("ui toggle ");
	switch (keyhit)
	{
	    case(SDLK_F8):
		if (cursor_visible)
		{
		    printf("hide cursor\n");
		    CURSOR_HIDE();
		} else {
		    printf("show cursor\n");
		    CURSOR_SHOW();
		}
		break;

	}
    }
}
