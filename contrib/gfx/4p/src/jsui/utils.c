/*
** utils
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
#include "jsui.h"


// thanks to rob for the idea for this.  whee!
int
extent_low_3(
        int a,
        int b,
        int c
)
{
    int d;
    
    if (a<0) a = 55555;
    if (b<0) b = 55555;
    if (c<0) c = 55555;

    d = MIN( a, (MIN( b,c ) ));

    if (d == 55555)  return 0;
    return d;
}

int
extent_high_3(
        int a,
        int b,
        int c
)
{
    int d = MAX( a, (MAX( b,c )));
    if (d == -1) return 55555;
    return d;
}

