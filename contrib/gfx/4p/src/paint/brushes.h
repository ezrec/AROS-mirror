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

#ifndef __IBRUSHES_H__
#define __IBRUSHES_H__

////////////////////////////////////////////////////////////////////////////////
// some globals...

extern int brush_size;

extern IPAGE * brush_circle;
extern IPAGE * brush_square;
extern IPAGE * brush_3;
extern IPAGE * brush_5;
extern IPAGE * brush_custom;
extern IPAGE * brush_active;

////////////////////////////////////////////////////////////////////////////////
// main funcitons

void brush_init(void);
void brush_deinit(void);


////////////////////////////////////////////////////////////////////////////////
// the internal resizable brushes...                                           

IPAGE * brush_create_circle(int radius);

IPAGE * brush_create_square(int size);              

/*
 *  scatter brush 3
 *
 *   . @ . . .
 *   . . . . .
 *   . . . . @
 *   . . . . .
 *   . . @ . .
 */

IPAGE * brush_create_scatter3(void);


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


IPAGE * brush_create_scatter5(void);

#endif

