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

int bytes(name,total,block)
char *name;
int *total,*block;
{

#warning Unimplemented function Info()
kprintf("AROS: bytes() NOT IMPLEMENTED BECAUSE OF MISSING Info() CALL\n");
*total = 0;
*block = 0;
return 0;

#if 0	
	int free=-1;
	BPTR mylock,testlock;
	struct InfoData __aligned infodata;

	if (!(mylock=Lock(name,ACCESS_READ))) {
		*total=0; *block=0;
		return(0);
	}
	Info(mylock,&infodata);
	if (ramdisk_lock) {
		testlock=DupLock(mylock);
		testlock=getrootlock(testlock);
		if (CompareLock(testlock,ramdisk_lock)==LOCK_SAME) free=AvailMem(0);
		UnLock(testlock);
		if (free>-1) {
			*total=free;
			*block=free/infodata.id_BytesPerBlock;
			UnLock(mylock);
			return(free);
		}
	}
	*total=infodata.id_BytesPerBlock*infodata.id_NumBlocks;
	free=*total-(infodata.id_BytesPerBlock*infodata.id_NumBlocksUsed);
	*block=infodata.id_NumBlocks-infodata.id_NumBlocksUsed;
	UnLock(mylock);
	return(free);
#endif
}

struct TagItem obtain_tags[]={
	{OBP_Precision,PRECISION_EXACT},
	{OBP_FailIfBad,TRUE},
	{TAG_DONE,0}};

void get_colour_table()
{
	int a;

	for (a=0;a<16;a++) {
		screen_pens[a].red=config->new_palette[(a*3)];
		screen_pens[a].green=config->new_palette[(a*3)+1];
		screen_pens[a].blue=config->new_palette[(a*3)+2];
		screen_pens[a].pen=a;
		screen_pens[a].alloc=0;
	}

#warning Unimplemented function FindColor()

kprintf("get_colour_table(): Unimplemented function FindColor()\n");
#if 0
	if (system_version2>=OSVER_39) {
		int num;
		struct ColorMap *cm;

		num=1<<config->scrdepth;
		cm=Window->WScreen->ViewPort.ColorMap;

		for (a=0;a<num;a++) {
			if ((screen_pens[a].pen=
				ObtainBestPenA(cm,
					screen_pens[a].red,
					screen_pens[a].green,
					screen_pens[a].blue,
					(MainScreen)?obtain_tags:NULL))==-1) {
					screen_pens[a].pen=FindColor(cm,
						screen_pens[a].red,
						screen_pens[a].green,
						screen_pens[a].blue,
						-1);
				}
				else screen_pens[a].alloc=1;
		}
	}
#endif
}

void free_colour_table()
{
	if (system_version2>=OSVER_39) {
		int a;
		struct ColorMap *cm;

		cm=Window->WScreen->ViewPort.ColorMap;

		for (a=0;a<16;a++) {
			if (screen_pens[a].alloc) {
				ReleasePen(cm,screen_pens[a].pen);
				screen_pens[a].alloc=0;
			}
		}
	}
}

void SetDrawModes(r,fg,bg,mode)
struct RastPort *r;
char fg,bg,mode;
{
	if (system_version2>=OSVER_39) {
		SetABPenDrMd(r,
			screen_pens[(int)fg].pen,
			screen_pens[(int)bg].pen,
			mode);
	}
	else {
		SetAPen(r,screen_pens[(int)fg].pen);
		SetBPen(r,screen_pens[(int)bg].pen);
		SetDrMd(r,mode);
	}
}

void do3dbox(r,x,y,w,h)
struct RastPort *r;
int x,y,w,h;
{
	Do3DBox(r,x,y,w,h,screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
}
