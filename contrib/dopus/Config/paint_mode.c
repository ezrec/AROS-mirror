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

int get_paint_colours(fg,bg,type)
int *fg,*bg,type;
{
	struct DOpusListView dummylist;
	struct Window *window;
	struct IntuiMessage *msg;
	ULONG class;
	USHORT code;
	int x,y,gadgetid;

	dummylist.w=330;
	dummylist.h=37;

	listokaygad[1].NextGadget=NULL;

	setup_list_window(&requestwin,&dummylist,listokaygad,2);

	requestwin.Title=cfg_string[STR_PAINT_SELECT_COLOURS];
	if (!(window=openwindow(&requestwin))) return(0);
	AddGadgets(window,
		listokaygad,
		listviewgads,
		2,
		screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen,1);
	setupcolourbox(window->RPort,dummylist.x+7,dummylist.y+13,*fg,*bg);
	showfuncob(window->RPort,cfg_string[STR_EXAMPLE],*fg,*bg,type,dummylist.x+101,dummylist.y+3);

	busy();

	FOREVER {
		while ((msg=(struct IntuiMessage *)GetMsg(window->UserPort))) {
			class=msg->Class;
			code=msg->Code;
			x=msg->MouseX;
			y=msg->MouseY;
			if (class==IDCMP_GADGETUP)
				gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
			ReplyMsg((struct Message *)msg);

			switch (class) {
				case IDCMP_MOUSEBUTTONS:	
					if (code==SELECTDOWN)
						docolourgad(window->RPort,
							dummylist.x+7,dummylist.y+13,
							x,y,
							cfg_string[STR_EXAMPLE],fg,bg,type);
					break;

				case IDCMP_GADGETUP:
					CloseWindow(window);
					unbusy();
					return(gadgetid);
			}
		}
		Wait(1<<window->UserPort->mp_SigBit);
	}
}

void do3dbox(x,y,w,h)
int x,y,w,h;
{
	Do3DBox(rp,x,y,w,h,screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
}

void fix_slider(gad)
struct Gadget *gad;
{
	gad->MutualExclude=screen_pens[0].pen;
	((struct Image *)gad->GadgetRender)->PlaneOnOff=screen_pens[1].pen;
}

struct Window *openwindow(newwin)
struct NewWindow *newwin;
{
	struct Window *win;

	if ((win=OpenWindow(newwin))) {
		if (tfont) SetFont(win->RPort,tfont);
		SetAPen(win->RPort,screen_pens[0].pen);
		RectFill(win->RPort,
			win->BorderLeft,win->BorderTop,
			win->Width-win->BorderRight-1,
			win->Height-win->BorderBottom-1);
		SetAPen(win->RPort,screen_pens[1].pen);
		SetBPen(win->RPort,screen_pens[0].pen);
	}
	return(win);
}
