/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "config.h"

void showpalettegad(one)
int one;
{
	int a,b,x,y,w,h;

	b=1<<screen_depth;
	if (b==4) w=54; else w=27;
	if (b==16) h=6; else h=12;
	x=x_off+298;
	y=y_off+34;
	for (a=0;a<b;a++) {
		SetAPen(rp,screen_pens[a].pen);
		RectFill(rp,x,y,x+w-1,y+h-1);
		palettegad_xy[a][0]=x;
		palettegad_xy[a][1]=y;
		palettegad_xy[a][2]=x+w-1;
		palettegad_xy[a][3]=y+h-1;
		if (a==one) drawcompbox(rp,x,y,w,h);
		x+=w;
		if (a==7) {
			x=x_off+298;
			y=y_off+40;
		}
	}
}

void scalecol(col)
ULONG *col;
{
	int and;

	and=(1<<bpg)-1;
	*col&=~and;
	*col>>=(32-bpg);
}

void scalecolup(col)
ULONG *col;
{
	int and;

	*col<<=(32-bpg);
	and=(1<<bpg)-1;
	*col&=~and;
}
