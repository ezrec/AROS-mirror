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

#include "dopus.h"

void do_remember_config(data)
struct RememberData *data;
{
	int a;

	data->copyflags=config->copyflags;
	data->deleteflags=config->deleteflags;
	data->errorflags=config->errorflags;
	CopyMem(config->displaypos,data->displaypos,32);
	data->generalflags=config->generalflags;
	data->iconflags=config->iconflags;
	data->existflags=config->existflags;
	data->sortflags=config->sortflags;
	data->dynamicflags=config->dynamicflags;
	strcpy(data->outputcmd,config->outputcmd);
	strcpy(data->output,config->output);
	data->scrdepth=config->scrdepth;
	data->screenflags=config->screenflags;
	data->screenmode=config->screenmode;
	data->scrw=config->scrw;
	data->scrh=config->scrh;
	data->dirflags=config->dirflags;
	strcpy(data->defaulttool,config->defaulttool);
	data->showdelay=config->showdelay;
	data->viewbits=config->viewbits;
	data->fadetime=config->fadetime;
	data->hiddenbit=config->hiddenbit;
	strcpy(data->showpat,config->showpat);
	strcpy(data->hidepat,config->hidepat);
	data->icontype=config->icontype;
	data->scrclktype=config->scrclktype;
	data->showfree=config->showfree;
	for (a=0;a<NUMFONTS;a++) {
		data->fontsizes[a]=config->fontsizes[a];
		strcpy(data->fontbufs[a],config->fontbufs[a]);
	}
	data->dateformat=config->dateformat;
	data->gadgetrows=config->gadgetrows;
	CopyMem(config->displaylength,data->displaylength,32);
	data->windowdelta=config->windowdelta;
	for (a=0;a<2;a++) {
		data->sortmethod[a]=config->sortmethod[a];
		data->separatemethod[a]=config->separatemethod[a];
	}
	data->wbwinx=config->wbwinx;
	data->wbwiny=config->wbwiny;
	data->scr_winw=config->scr_winw;
	data->scr_winh=config->scr_winh;
	strcpy(data->pubscreen_name,config->pubscreen_name);
	strcpy(data->portname,str_arexx_portname);
}

void do_restore_config(data)
struct RememberData *data;
{
	int a;

	config->copyflags=data->copyflags;
	config->deleteflags=data->deleteflags;
	config->errorflags=data->errorflags;
	CopyMem(data->displaypos,config->displaypos,32);
	config->generalflags=data->generalflags;
	config->iconflags=data->iconflags;
	config->existflags=data->existflags;
	config->sortflags=data->sortflags;
	config->dynamicflags=data->dynamicflags;
	strcpy(config->outputcmd,data->outputcmd);
	strcpy(config->output,data->output);
	config->scrdepth=data->scrdepth;
	config->screenflags=data->screenflags;
	config->screenmode=data->screenmode;
	config->scrw=data->scrw;
	config->scrh=data->scrh;
	config->dirflags=data->dirflags;
	strcpy(config->defaulttool,data->defaulttool);
	config->showdelay=data->showdelay;
	config->viewbits=data->viewbits;
	config->fadetime=data->fadetime;
	config->hiddenbit=data->hiddenbit;
	strcpy(config->showpat,data->showpat);
	strcpy(config->hidepat,data->hidepat);
	config->icontype=data->icontype;
	config->scrclktype=data->scrclktype;
	config->showfree=data->showfree;
	for (a=0;a<NUMFONTS;a++) {
		config->fontsizes[a]=data->fontsizes[a];
		strcpy(config->fontbufs[a],data->fontbufs[a]);
	}
	config->dateformat=data->dateformat;
	config->gadgetrows=data->gadgetrows;
	CopyMem(data->displaylength,config->displaylength,32);
	for (a=0;a<2;a++) {
		config->sortmethod[a]=data->sortmethod[a];
		config->separatemethod[a]=data->separatemethod[a];
	}

	LParsePatternI(config->showpat,config->showpatparsed);
	LParsePatternI(config->hidepat,config->hidepatparsed);
	dosizedirwindows(data->windowdelta);
	config->wbwinx=data->wbwinx;
	config->wbwiny=data->wbwiny;
	config->scr_winw=data->scr_winw;
	config->scr_winh=data->scr_winh;
	strcpy(config->pubscreen_name,data->pubscreen_name);
	change_port_name(data->portname);
}
