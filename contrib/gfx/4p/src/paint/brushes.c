/*
** brushes
**
**  internal brushes
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
#include "brushes.h"
#include "tools.h"


// globals
IPAGE * brush_circle = NULL;
IPAGE * brush_square = NULL;
IPAGE * brush_3      = NULL;
IPAGE * brush_5      = NULL;
IPAGE * brush_custom = NULL;
IPAGE * brush_active = NULL;
                             
int brush_size = 1;

////////////////////////////////////////////////////////////////////////////////
// main funcitons

void
brush_init(
	void
)
{
    brush_3 = brush_create_scatter3();
    brush_5 = brush_create_scatter5();

    brush_circle = NULL;
    brush_square = NULL;
    brush_custom = NULL;
    brush_active = NULL;

    brush_size = 1;
    drawing_mode = DRAW_STYLE_COLOR;
}


void
brush_deinit(
	void
)
{
    if (brush_circle)	Page_Destroy(brush_circle);
    if (brush_square)	Page_Destroy(brush_square);
    if (brush_3)	Page_Destroy(brush_3);
    if (brush_5)	Page_Destroy(brush_5);
    if (brush_custom)	Page_Destroy(brush_custom);
}

            

////////////////////////////////////////////////////////////////////////////////
/*
 *  scatter brush 3
 *
 *   . @ . . .
 *   . . . . .
 *   . . . . @
 *   . . . . .
 *   . . @ . .
 */

IPAGE *
brush_create_scatter3(
	void
)
{
    IPAGE * newpage;

    newpage = Page_Create(5, 5, 2);

    newpage->fgpen->Icolor = 1;

    primitive_pixel(newpage, newpage->fgpen, 1, 0);
    primitive_pixel(newpage, newpage->fgpen, 4, 2);
    primitive_pixel(newpage, newpage->fgpen, 2, 4);
    
    Page_Alpha_Fill(newpage, 1);
    Page_Alpha_Chromakey_Index(newpage, 0, 0);

    Page_Handle_Center(newpage);

    return newpage;
}



/*
 *  scatter brush 5
 *
 *   . . . . @ . . . .
 *   . . . . . . . . .
 *   . . . . . . . . .
 *   . . . . . . . . @
 *   @ . . . @ . . . .
 *   . . . . . . . . .
 *   . . . . . . . . .
 *   . . . . . . . . .
 *   . . . . . @ . . .
 */


IPAGE *
brush_create_scatter5(
	void
)
{
    IPAGE * newpage;

    newpage = Page_Create(9, 9, 2);

    newpage->fgpen->Icolor = 1;

    primitive_pixel(newpage, newpage->fgpen, 4, 0);
    primitive_pixel(newpage, newpage->fgpen, 8, 3);
    primitive_pixel(newpage, newpage->fgpen, 0, 4);
    primitive_pixel(newpage, newpage->fgpen, 4, 4);
    primitive_pixel(newpage, newpage->fgpen, 5, 8);
    
    Page_Alpha_Fill(newpage, 1);
    Page_Alpha_Chromakey_Index(newpage, 0, 0);

    Page_Handle_Center(newpage);

    return newpage;
}


////////////////////////////////////////////////////////////////////////////////
// the internal resizable brushes...

IPAGE *
brush_create_circle(
	int radius
)
{
    IPAGE * circlebrush;

    circlebrush = Page_Create( radius * 2 + 1, radius * 2 + 1, 2);
    circlebrush->fgpen->Icolor = 1;

    primitive_circle_hollow(circlebrush, circlebrush->fgpen, primitive_pixel,
				radius, radius, radius);

    primitive_floodfill(circlebrush, circlebrush->fgpen, radius, radius);

    Page_Alpha_Fill(circlebrush, 1);
    Page_Alpha_Chromakey_Index(circlebrush, 0,0);

    Page_Handle_Center(circlebrush);

    return (circlebrush);
}


IPAGE *
brush_create_square(
	int size
)
{
    IPAGE * squarebrush;

    squarebrush = Page_Create(size, size, 2);
    squarebrush->fgpen->Icolor = 1;

    primitive_rectangle_filled(squarebrush, squarebrush->fgpen, 
				0, 0, size, size);

    Page_Alpha_Fill(squarebrush, 1);

    Page_Handle_Center(squarebrush);

    return (squarebrush);
}
