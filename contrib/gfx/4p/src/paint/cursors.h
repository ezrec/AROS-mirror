/*
** cursors
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

#ifndef __CURSORS_H__
#define __CURSORS_H__

void cursor_init( void );
void cursor_deinit( void );


#include "SDL.h"


extern SDL_Cursor * arrow;
extern SDL_Cursor * ibeam;
extern SDL_Cursor * crosshairs;
extern SDL_Cursor * eyedropper;
extern SDL_Cursor * paintcan;


#define CURSOR_ARROW()\
		SDL_SetCursor(arrow);\
		SDL_ShowCursor(cursor_visible);

#define CURSOR_IBEAM()\
		SDL_SetCursor(ibeam);\
		SDL_ShowCursor(cursor_visible);

#define CURSOR_CROSSHAIRS()\
		SDL_SetCursor(crosshairs);\
		SDL_ShowCursor(cursor_visible);

#define CURSOR_EYEDROPPER()\
		SDL_SetCursor(eyedropper);\
		SDL_ShowCursor(cursor_visible);

#define CURSOR_PAINTCAN()\
		SDL_SetCursor(paintcan);\
		SDL_ShowCursor(cursor_visible);

#define CURSOR_SHOW()\
		cursor_visible = 1;\
		SDL_ShowCursor(cursor_visible);

#define CURSOR_HIDE()\
		cursor_visible = 0;\
		SDL_ShowCursor(cursor_visible);

#endif
