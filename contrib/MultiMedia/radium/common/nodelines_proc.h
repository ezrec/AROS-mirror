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


#ifndef TRACKER_INCLUDE

extern void MakeNodeLines(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	Place *p1,
	Place *p2,
	float x1,float x2,
	float minx,float maxx,
	int startx,int endx,
	void *extrainfo,
	void (*ReturnNodeLineData)(
		struct Tracker_Windows *window,
		struct WBlocks *wblock,
		void *extrainfo,
		int firstlast,
		int realline,
		int u_y1,int u_y2,int u_x1,int u_x2
	)
);

#endif
