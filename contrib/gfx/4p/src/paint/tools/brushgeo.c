/*
 * brushgeo
 * 
 *   brush geometry callback handlers
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
#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "brushes.h"



////////////////////////////////////////////////////////////////////////////////

void
tools_brush_flip_horizontal (
        int mouse_event,
        int mouse_buttons,
        int keyhit,       
        int x,     
        int y 
)            
{
    if (mouse_event == TOOL_EVENT_INIT)
    {                                  
	if (brush_active)
	{
	    printf("flip brush horizontal\n");
	    Page_Flip_Horizontal(brush_active);
	}
    }
}


void
tools_brush_flip_vertical (
        int mouse_event,
        int mouse_buttons,
        int keyhit,       
        int x,     
        int y 
)            
{
    if (mouse_event == TOOL_EVENT_INIT)
    {                                  
	if (brush_active)
	{
	    printf("flip brush vertical\n");
	    Page_Flip_Vertical(brush_active);
	}
    }
}



void
tools_brush_rotate_90 (
        int mouse_event,
        int mouse_buttons,
        int keyhit,       
        int x,     
        int y 
)            
{
    if (mouse_event == TOOL_EVENT_INIT)
    {                                  
	if (brush_active)
	{
	    printf("rotate brush 90\n");
	    Page_Swap_XY(brush_active);
	    Page_Flip_Horizontal(brush_active);
	}
    }
}



////////////////////////////////////////////////////////////////////////////////

void
tools_brush_halve_size (
        int mouse_event,
        int mouse_buttons,
        int keyhit,       
        int x,     
        int y 
)            
{
    if (mouse_event == TOOL_EVENT_INIT)
    {                                  
	if (brush_active)
	{
	    printf("halve brush size\n");
	    Page_Halve_Size(brush_active);
	}
    }
}


void
tools_brush_double_size (
        int mouse_event,
        int mouse_buttons,
        int keyhit,       
        int x,     
        int y 
)            
{
    if (mouse_event == TOOL_EVENT_INIT)
    {                                  
	if (brush_active)
	{
	    printf("double brush size\n");
	    Page_Double_Horizontal(brush_active);
	    Page_Double_Vertical(brush_active);
	}
    }
}


void
tools_brush_double_horizontal (
        int mouse_event,
        int mouse_buttons,
        int keyhit,       
        int x,     
        int y 
)            
{
    if (mouse_event == TOOL_EVENT_INIT)
    {                                  
	if (brush_active)
	{
	    printf("double horizontal brush size\n");
	    Page_Double_Horizontal(brush_active);
	}
    }
}

void
tools_brush_double_vertical (
        int mouse_event,
        int mouse_buttons,
        int keyhit,       
        int x,     
        int y 
)            
{
    if (mouse_event == TOOL_EVENT_INIT)
    {                                  
	if (brush_active)
	{
	    printf("double vertical brush size\n");
	    Page_Double_Vertical(brush_active);
	}
    }
}
