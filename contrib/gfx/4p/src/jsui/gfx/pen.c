/*
 * pen
 *
 *   drawing pen functions
 *
 *   1999.12.30
 *   Scott "Jerry" Lawrence
 *   jsl@absynth.com
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

#include <stdlib.h>
#include "jsui.h"


PEN *
Pen_Create(
	void
)
{
    PEN * pen = (PEN *) malloc(sizeof(PEN));

    pen->RGBcolor.r = 0;
    pen->RGBcolor.g = 0;
    pen->RGBcolor.b = 0;

    pen->Icolor = 0;

    pen->text_type = TEXT_TYPE_JAM1;
    return(pen);
}


void
Pen_Destroy(
	PEN * pen
)
{
    if (pen)
	free(pen);
}


