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

void show_slider_setup(x,y,num)
int x,y,num;
{
	int pos,sx,bx;

	x+=x_off;
	y+=y_off;

	if (config->slider_pos&(1<<num)) {
		pos=1;
		bx=x;
		sx=x+171;
	}
	else {
		pos=0;
		bx=x+20;
		sx=x;
	}

	SetAPen(rp,screen_pens[1].pen);
	SetBPen(rp,screen_pens[0].pen);

	DoCycleGadget(&screen_sliders_gadgets[num],Window,left_right_cycle,pos);

	Do3DBox(rp,
		sx,y,
		16,94,
		screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);

	Do3DBox(rp,
		sx,y+96,
		16,13,
		screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);

	Do3DBox(rp,
		sx,y+111,
		16,13,
		screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);

	SetAPen(rp,screen_pens[(int)config->sliderbgcol].pen);
	RectFill(rp,
		sx,y,
		sx+15,y+93);

	SetAPen(rp,screen_pens[(int)config->slidercol].pen);
	RectFill(rp,
		sx+2,y+1,
		sx+13,y+92);

	DoArrow(rp,
		sx,y+96,
		16,13,
		screen_pens[(int)config->arrowfg].pen,screen_pens[(int)config->arrowbg].pen,0);

	DoArrow(rp,
		sx,y+111,
		16,13,
		screen_pens[(int)config->arrowfg].pen,screen_pens[(int)config->arrowbg].pen,1);

	Do3DBox(rp,
		bx,y,
		167,124,
		screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);

	SetAPen(rp,screen_pens[(int)config->filesbg].pen);
	RectFill(rp,
		bx,y,
		bx+166,y+123);
}
