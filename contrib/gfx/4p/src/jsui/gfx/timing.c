/*
** timing
**
**  stuff that deals with the timing... 
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

#include <stdio.h>

#include <SDL.h>

unsigned long timing_ticks =0;


#define TIMING_TPS   (10)		/* ticks per second */
#define TIMING_CONST (1000/TIMING_TPS)


/*
 * ticktock
 *
 *  the timing interrupt callback. 
 *  This handles our main timing resolution.
 */
static Uint32 ticktock(Uint32 interval)
{
    ++timing_ticks;
    return(interval);
}


void
timing_init(
	void
)
{
    SDL_SetTimer(TIMING_CONST, ticktock);
}


void
timing_deinit(
	void
)
{
    SDL_SetTimer(0, NULL);
}
