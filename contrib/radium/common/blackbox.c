/* Copyright 2000 Kjetil S. Matheussen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */








#include "nsmtracker.h"


void MakeBlackBox(
	struct Tracker_Windows *window,
	int u_x,
	int u_y,
	int width,
	struct TrackReallineElements *tre
){
	int size=max(window->minnodesize,window->fontwidth/4);

	tre->y1=max(0,u_y-size);
	tre->y2=min(window->fontheight-1,u_y+size);
	tre->x1=max(0,u_x-size);
	tre->x2=min(width,u_x+size);
}


