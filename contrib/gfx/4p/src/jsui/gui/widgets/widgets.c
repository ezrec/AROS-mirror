/*
 * widgets
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

/*
  Give a man fire, and he'll be warm for the night.
  Set him on fire, and he'll be warm for the rest of his life.
*/

/************************************************************
 * built-in widgets 
 */


int jsui_widget_keyboard	
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    return JSUI_R_O_K;
}

int jsui_widget_textbox		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    return JSUI_R_O_K;
}

int jsui_widget_timetick	
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    return JSUI_R_O_K;
}

