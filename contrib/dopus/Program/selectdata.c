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

enum {
	SELECT_CANCEL,
	SELECT_OKAY,
	SELECT_SELECTBASE,
	SELECT_SELECTTYPE};

static char sel_patternbuf[80];

struct RequesterBase select_req={
	36,3,50,27,
	8,6,
	0,0,0,0,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
	0,0,NULL,0,NULL};

struct TagItem
	select_base_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CYCLE},
		{RO_GadgetID,SELECT_SELECTBASE},
		{RO_Top,1},
		{RO_TopFine,2},
		{RO_Width,6},
		{RO_WidthFine,24},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	select_pattern_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,SELECT_OKAY},
		{RO_Top,1},
		{RO_TopFine,4},
		{RO_Height,1},
		{RO_HeightFine,2},
		{RO_Left,6},
		{RO_LeftFine,30},
		{RO_Width,30},
		{RO_StringBuf,(ULONG)sel_patternbuf},
		{RO_StringLen,79},
		{RO_StringUndo,(ULONG)str_undobuffer},
		{TAG_END,0}},

	select_okay_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,SELECT_OKAY},
		{RO_Top,2},
		{RO_TopFine,11},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_Width,10},
		{RO_TextNum,STR_OKAY},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	select_type_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CYCLE},
		{RO_GadgetID,SELECT_SELECTTYPE},
		{RO_Top,2},
		{RO_TopFine,11},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_Width,14},
		{RO_WidthFine,20},
		{RO_Left,11},
		{RO_LeftFine,6},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	select_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,SELECT_CANCEL},
		{RO_Top,2},
		{RO_TopFine,11},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_Width,10},
		{RO_Left,26},
		{RO_LeftFine,34},
		{RO_TextNum,STR_CANCEL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	select_title_text[]={
		{RO_Type,OBJECT_BORDER},
		{RO_BorderType,BORDER_NONE},
		{RO_Width,36},
		{RO_WidthFine,34},
		{RO_TopFine,-1},
		{RO_Height,1},
		{RO_TextNum,STR_ENTER_SELECT_PATTERN},
		{RO_TextPos,TEXTPOS_CENTER},
		{TAG_END,0}},

	*select_gadgets[]={
		select_base_gadget,
		select_pattern_gadget,
		select_type_gadget,
		select_okay_gadget,
		select_cancel_gadget,
		NULL};

int select_base=0,select_type=0;

int getselectdata(buffer,selbase)
char **buffer;
int *selbase;
{
	ULONG class;
	USHORT gadgetid;
	struct Window *swindow;
	struct Gadget *gadlist,*sel_type_gad;
	char *select_base_array[3],*select_type_array[3];
	int a,ret=0;

	fix_requester(&select_req,globstring[STR_ENTER_SELECT_PATTERN]);

	for (a=0;a<3;a++) {
		select_type_array[a]=globstring[STR_SELECT_ALL_ENTRIES+a];
		select_base_array[a]=globstring[STR_SELECT_NAME+a]; 
	}
	strcpy(sel_patternbuf,buffer[select_base]);

	if (!(swindow=OpenRequester(&select_req)) ||
		!(gadlist=addreqgadgets(&select_req,select_gadgets,0,NULL)) ||
		!(AddRequesterObject(&select_req,select_title_text))) {
		CloseRequester(&select_req);
		return(0);
	}
	RefreshRequesterObject(&select_req,NULL);

	sel_type_gad=gadlist->NextGadget->NextGadget;
	DoCycleGadget(gadlist,swindow,select_base_array,select_base);
	DoCycleGadget(sel_type_gad,swindow,select_type_array,select_type);

	ActivateStrGad(gadlist->NextGadget,swindow);

	FOREVER {
		while ((IMsg=(struct IntuiMessage *)GetMsg(swindow->UserPort))) {
			class=IMsg->Class;
			if (class==IDCMP_GADGETUP)
				gadgetid=((struct Gadget *) IMsg->IAddress)->GadgetID;
			ReplyMsg((struct Message *) IMsg);

			switch (class) {
				case IDCMP_MOUSEBUTTONS:
					ActivateStrGad(gadlist->NextGadget,swindow);
					break;

				case IDCMP_GADGETUP:
					switch (gadgetid) {
						case SELECT_SELECTBASE:
							strcpy(buffer[select_base],sel_patternbuf);
							if (++select_base>2) select_base=0;
							DoCycleGadget(gadlist,swindow,select_base_array,select_base);
							strcpy(sel_patternbuf,buffer[select_base]);
							RefreshStrGad(gadlist->NextGadget,swindow);
							ActivateStrGad(gadlist->NextGadget,swindow);
							break;
						case SELECT_SELECTTYPE:
							if (++select_type>2) select_type=0;
							DoCycleGadget(sel_type_gad,swindow,select_type_array,select_type);
							ActivateStrGad(gadlist->NextGadget,swindow);
							break;
						case SELECT_OKAY:
							ret=select_type+1;
						case SELECT_CANCEL:
							CloseRequester(&select_req);
							strcpy(buffer[select_base],sel_patternbuf);
							*selbase=select_base;
							return(ret);
					}
					break;
			}
		}
		Wait(1<<swindow->UserPort->mp_SigBit);
	}
}
