/*
 * screen
 * 
 *   screen tools callback handlers
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

#include <unistd.h> // for getcwd()  on SunOS

#include <stdio.h>
#include <string.h>
#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "fileio.h"
#include "paint.h"



////////////////////////////////////////////////////////////////////////////////
// clear screen

void
tools_clear_screen (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf ("clear screen\n");
	Page_Undo_Checkpoint(page_active);
	primitive_rectangle_filled(page_active, page_active->bgpen, 0,0, 
		page_active->w, page_active->h);
    }
}


////////////////////////////////////////////////////////////////////////////////
// undo

void
tools_undo (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf ("undo\n");
	Page_Undo(page_active);
    }
}


////////////////////////////////////////////////////////////////////////////////
// swap

void
tools_switch_to_spare (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf ("switch to spare\n");
	if (page_active == page_primary)
	{
	    page_active = page_swap;
	} else {
	    page_active = page_primary;
	}
    }
    Page_DR_Dirtify(page_active, 0, 0, page_active->w-1, page_active->h-1);
    Page_DR_Dirtify(page_rubberband, 0, 0, page_active->w-1, page_active->h-1);
    page_active->pdirty = 1;
}


void
tools_copy_to_spare (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf ("copy to spare\n");
	if (page_active == page_primary)
	{
	    Page_Destroy(page_swap);
	    page_swap = Page_Duplicate(page_active);
	} else {
	    Page_Destroy(page_primary);
	    page_primary = Page_Duplicate(page_swap);
	    page_active = page_primary;
	}
    }
    Page_DR_Dirtify(page_swap, 0, 0, page_swap->w-1, page_swap->h-1);
}



////////////////////////////////////////////////////////////////////////////////

void 
tools_save_page(
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    char dir_buf[1024];
    char file_buf[1024];

    if (mouse_event == TOOL_EVENT_INIT)
    {
	(void) getcwd( dir_buf, 1024 );
	strcat( dir_buf, "/" );
	file_buf[0] = '\0';
	if( do_filereq(actual_screen, "Save an Image...",
		    "Save", dir_buf, file_buf, 1024 ) )
	{
	    strcat( dir_buf, file_buf );
	    file_save_page(page_active, dir_buf);
	}
    }
}


int initted_freq = 0;

void 
tools_load_page(
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    IPAGE * newpage;
    static char dir_buf[1024];
    static char file_buf[1024];
    char fullpath_buf[1024];

    if (mouse_event == TOOL_EVENT_INIT)
    {
	if (initted_freq == 0)
	{
	    (void) getcwd( dir_buf, 1024 );
	    strcat( dir_buf, "/" );
	    file_buf[0] = '\0';
	    initted_freq = 1;
	}
	if( !do_filereq(actual_screen, "Load an Image...",
		    "Load", dir_buf, file_buf, 1024 ) )
	{
	    return;
	}
	    
	strcpy( fullpath_buf, dir_buf );
	if (dir_buf[ strlen(dir_buf)-1 ] != '/')
	{
	    strcat( fullpath_buf, "/" );
	}
	strcat( fullpath_buf, file_buf );

printf("Loading %s\n", fullpath_buf);

	newpage = file_load_page(fullpath_buf);
	if (newpage)
	{
	    if (newpage->ncolors <= 0)
	    {
		printf("cannot load truecolor images. sorry.\n");
		Page_Destroy(newpage);
		return;
	    }

	    if (page_active == page_primary)
	    {
		page_active = newpage;
		Page_Destroy(page_primary);
		page_primary = newpage;
	    } else {
		page_active = newpage;
		Page_Destroy(page_swap);
		page_swap = newpage;
	    }
	}
	Page_DR_Dirtify(page_active, 0, 0, page_active->w-1, page_active->h-1);
	page_active->pdirty = 1;
    }
}

