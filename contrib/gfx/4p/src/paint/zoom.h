/*
** zoom
**
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2001 Scott "Jerry" Lawrence
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
#include "jsui.h"

extern IPAGE * page_prezoomed;  /* edit-sized screen */
extern IPAGE * page_zoom;	/* GUI Window-sized screen */

void zoom_clearpage( void );	/* draw a checkbox pattern on page_zoom */

void zoom_init( void );
void zoom_deinit( void );

// this takes the edit page and does some druid magic on it 
// to convert it to screen coordinates.
void zoom_stretch_pages( IPAGE * dest, IPAGE * src );

// these take screen mouse coordinates and convert them back
// to edit page coordinates. 
//   they can return coordinates outside of the edit page.
int zoom_actual_to_zoomed_X( int mouse_x );
int zoom_actual_to_zoomed_Y( int mouse_y );

