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

void showupdownslider()
{
	int x,y,ay[2],a,height;

	if (config->arrowsize[0]<8) config->arrowsize[0]=8;
	else if (config->arrowsize[0]>20) config->arrowsize[0]=20;

	height=49-(config->arrowsize[0]*2);

	switch (config->arrowpos[0]) {
		case 0:
			y=y_off+35; ay[0]=y+height+4; ay[1]=ay[0]+config->arrowsize[0];
			break;
		case 1:
			ay[0]=y_off+35; ay[1]=ay[0]+config->arrowsize[0]; y=ay[1]+config->arrowsize[0];
			break;
		case 2:
			ay[0]=y_off+35; y=ay[0]+config->arrowsize[0]; ay[1]=y+height+4;
			break;
	}

	if (config->sliderwidth<8) config->sliderwidth=8;
	else if (config->sliderwidth>108) config->sliderwidth=108;
	if (config->sliderwidth%2) ++config->sliderwidth;

	x=x_off+244-(config->sliderwidth/2);

	for (a=0;a<2;a++) {
		DoArrow(rp,
			x-2,ay[a],
			config->sliderwidth+4,config->arrowsize[0]-2,
			screen_pens[(int)config->arrowfg].pen,screen_pens[(int)config->arrowbg].pen,a);
		do3dbox(x-2,ay[a],config->sliderwidth+4,config->arrowsize[0]-2);
	}

	Do3DBox(rp,
		x,y+1,
		config->sliderwidth,height,
		screen_pens[(int)config->sliderbgcol].pen,screen_pens[(int)config->sliderbgcol].pen);
	SetAPen(rp,screen_pens[(int)config->slidercol].pen);
	RectFill(rp,x,y+1,x+config->sliderwidth-1,y+height);
	do3dbox(x-2,y,config->sliderwidth+4,height+2);

	SetAPen(rp,screen_pens[0].pen);
	RectFill(rp,
		x_off+184,y_off+33,
		x-5,y_off+87);
	RectFill(rp,
		x+config->sliderwidth+4,y_off+33,
		x_off+304,y_off+87);
}

void showleftrightslider()
{
	int y,x,ax[2],a,width;

	if (config->arrowsize[1]<8) config->arrowsize[1]=8;
	else if (config->arrowsize[1]>20) config->arrowsize[1]=20;

	width=97-(config->arrowsize[1]*2);

	switch (config->arrowpos[1]) {
		case 0:
			x=x_off+317; ax[0]=329+width; ax[1]=ax[0]+config->arrowsize[1]+6;
			break;
		case 1:
			ax[0]=x_off+317; ax[1]=ax[0]+config->arrowsize[1]+6; x=ax[1]+config->arrowsize[1]+6;
			break;
		case 2:
			ax[0]=x_off+317; x=ax[0]+config->arrowsize[1]+6; ax[1]=x+width+8;
			break;
	}

	if (config->sliderheight<5) config->sliderheight=5;
	else if (config->sliderheight>49) config->sliderheight=49;
	if (!(config->sliderheight%2)) ++config->sliderheight;

	y=(y_off+60)-(config->sliderheight/2);

	for (a=0;a<2;a++) {
		DoArrow(rp,
			ax[a],y-1,
			config->arrowsize[1]+2,config->sliderheight+2,
			screen_pens[(int)config->arrowfg].pen,screen_pens[(int)config->arrowbg].pen,3-a);
		do3dbox(ax[a],y-1,config->arrowsize[1]+2,config->sliderheight+2);
	}

	Do3DBox(rp,
		x+2,y,
		width,config->sliderheight,
		screen_pens[(int)config->sliderbgcol].pen,screen_pens[(int)config->sliderbgcol].pen);
	SetAPen(rp,screen_pens[(int)config->slidercol].pen);
	RectFill(rp,x+2,y,x+width+1,y+config->sliderheight-1);
	do3dbox(x,y-1,width+4,config->sliderheight+2);

	SetAPen(rp,screen_pens[0].pen);
	RectFill(rp,
		x_off+313,y_off+33,
		x_off+433,y-3);
	RectFill(rp,
		x_off+313,y+config->sliderheight+2,
		x_off+433,y_off+87);
}

void showstringgadget()
{
	int x,ax[2],a,y,width;

	if (config->arrowsize[2]<8) config->arrowsize[2]=8;
	else if (config->arrowsize[2]>20) config->arrowsize[2]=20;

	width=96-(config->arrowsize[2]*2);

	switch (config->arrowpos[2]) {
		case 0:
			x=x_off+448; ax[0]=459+width; ax[1]=ax[0]+config->arrowsize[2]+6;
			break;
		case 1:
			ax[0]=x_off+446; ax[1]=ax[0]+config->arrowsize[2]+6; x=ax[1]+config->arrowsize[2]+8;
			break;
		case 2:
			ax[0]=x_off+446; x=ax[0]+config->arrowsize[2]+8; ax[1]=x+width+7;
			break;
	}

	if (config->stringheight<5) config->stringheight=5;
	else if (config->stringheight>49) config->stringheight=49;

	y=(y_off+60)-(config->stringheight/2);

	for (a=0;a<2;a++) {
		DoArrow(rp,
			ax[a],y-1,
			config->arrowsize[2]+2,config->stringheight+2,
			screen_pens[(int)config->arrowfg].pen,screen_pens[(int)config->arrowbg].pen,3-a);
		do3dbox(ax[a],y-1,config->arrowsize[2]+2,config->stringheight+2);
	}

	SetAPen(rp,screen_pens[(int)config->stringbgcol].pen);
	RectFill(rp,x,y,x+width,y+config->stringheight-1);
	Do3DStringBox(rp,
		x,y,
		width+1,config->stringheight,
		screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);

	SetAPen(rp,screen_pens[0].pen);
	RectFill(rp,
		x_off+442,y_off+33,
		x_off+562,y-3);
	RectFill(rp,
		x_off+442,y+config->stringheight+2,
		x_off+562,y_off+87);
}

void showarrowborders(active,old)
int active,old;
{
	if (old>-1) {
		SetAPen(rp,screen_pens[0].pen);
		drawarrowrec(old);
		SetAPen(rp,screen_pens[1].pen);
	}
	SetDrMd(rp,COMPLEMENT);
	drawarrowrec(active);
	SetDrMd(rp,JAM2);
}

void drawarrowrec(a)
int a;
{
	a*=129; a+=x_off+178;
	RectFill(rp,a,y_off+30,a+132,y_off+31);
	RectFill(rp,a,y_off+32,a+3,y_off+88);
	RectFill(rp,a+129,y_off+32,a+132,y_off+88);
	RectFill(rp,a,y_off+89,a+132,y_off+90);
}

void fixarrowgads(active)
int active;
{
	int a;

	for (a=0;a<3;a++) {
		if (config->arrowpos[active]==a) arrowgadgets[a].Flags|=SELECTED;
		else arrowgadgets[a].Flags&=~SELECTED;
	}
	RefreshGList(arrowgadgets,Window,NULL,3);
}
