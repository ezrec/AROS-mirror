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
	RENAME_CANCEL,
	RENAME_OKAY,
	RENAME_SKIP,
	RENAME_OLDNAME,
	RENAME_NEWNAME};

struct RequesterBase rename_req={
	40,5,24,30,
	8,6,
	0,0,0,0,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
	0,0,NULL,0,NULL};

char oldname_buffer[40],newname_buffer[40];

struct TagItem
	rename_oldname_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,RENAME_OLDNAME},
		{RO_LeftFine,4},
		{RO_Top,1},
		{RO_TopFine,2},
		{RO_Width,40},
		{RO_Height,1},
		{RO_TextNum,STR_RENAME},
		{RO_TextPos,TEXTPOS_ABOVE},
		{RO_StringBuf,(ULONG)oldname_buffer},
		{RO_StringLen,32},
		{RO_StringUndo,(ULONG)str_undobuffer},
		{TAG_END,0}},
	rename_newname_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,RENAME_NEWNAME},
		{RO_LeftFine,4},
		{RO_Top,3},
		{RO_TopFine,8},
		{RO_Width,40},
		{RO_Height,1},
		{RO_TextNum,STR_AS},
		{RO_TextPos,TEXTPOS_ABOVE},
		{RO_StringBuf,(ULONG)newname_buffer},
		{RO_StringLen,32},
		{RO_StringUndo,(ULONG)str_undobuffer},
		{TAG_END,0}},
	rename_okay_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,RENAME_OKAY},
		{RO_Top,4},
		{RO_TopFine,16},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_OKAY},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	rename_skip_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,RENAME_SKIP},
		{RO_Top,4},
		{RO_TopFine,16},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_Left,14},
		{RO_LeftFine,4},
		{RO_TextNum,STR_SKIP},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	rename_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,RENAME_CANCEL},
		{RO_Top,4},
		{RO_TopFine,16},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_Left,28},
		{RO_LeftFine,8},
		{RO_TextNum,STR_CANCEL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	*rename_gadgets[]={
		rename_oldname_gadget,
		rename_newname_gadget,
		rename_okay_gadget,
		rename_skip_gadget,
		rename_cancel_gadget,
		NULL};

int getrenamedata(src,dst)
char *src,*dst;
{
	ULONG class;
	USHORT code,gadgetid,qual;
	struct Window *rwindow;
	struct Gadget *gadlist;

	fix_requester(&rename_req,globstring[STR_ENTER_NEW_NAME]);

	strcpy(oldname_buffer,src); strcpy(newname_buffer,dst);

	if (!(rwindow=OpenRequester(&rename_req)) ||
		!(gadlist=addreqgadgets(&rename_req,rename_gadgets,0,NULL))) {
		CloseRequester(&rename_req);
		return(0);
	}
	RefreshRequesterObject(&rename_req,NULL);

	Delay(3);
	ActivateStrGad(gadlist->NextGadget,rwindow);

	FOREVER {
		while ((IMsg=(struct IntuiMessage *)GetMsg(rwindow->UserPort))) {
			class=IMsg->Class; code=IMsg->Code; qual=IMsg->Qualifier;
			if (class==IDCMP_GADGETUP)
				gadgetid=((struct Gadget *) IMsg->IAddress)->GadgetID;
			ReplyMsg((struct Message *) IMsg);

			switch (class) {
				case IDCMP_MOUSEBUTTONS:
					gadgetid=RENAME_OLDNAME;

				case IDCMP_GADGETUP:
					switch (gadgetid) {
						case RENAME_OLDNAME:
							ActivateStrGad(gadlist->NextGadget,rwindow);
							break;
						case RENAME_NEWNAME:
							if (qual&IEQUALIFIER_ANYSHIFT) {
								ActivateStrGad(gadlist,rwindow);
								break;
							}
							else if (code==0x9) break;
							gadgetid=RENAME_OKAY;

						case RENAME_OKAY:
							strcpy(src,oldname_buffer);
							strcpy(dst,newname_buffer);
						case RENAME_SKIP:
						case RENAME_CANCEL:
							CloseRequester(&rename_req);
							return((int)gadgetid);
					}
					break;
			}
		}
		Wait(1<<rwindow->UserPort->mp_SigBit);
	}
}
