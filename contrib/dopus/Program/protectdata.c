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
	PROTECT_SET_ALL=10,
	PROTECT_SET_NONE,
	PROTECT_SET_TOGGLE,
	PROTECT_SET_REVERT,
	PROTECT_OKAY,
	PROTECT_ALL,
	PROTECT_CANCEL};

struct RequesterBase protect_req={
	42,9,92,64,
	8,8,
	0,0,0,0,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
	0,0,NULL,IDCMP_GADGETUP|IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS,NULL};

struct TagItem
	protect_bit_template[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,0},
		{RO_Top,0},
		{RO_TopFine,0},
		{RO_TextNum,0},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},

	protect_bits[8][9],

	protect_set_all[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PROTECT_SET_ALL},
		{RO_Left,14},
		{RO_LeftFine,50},
		{RO_TopFine,18},
		{RO_Width,10},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_PROTECT_ALL_BITS},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	protect_set_none[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PROTECT_SET_NONE},
		{RO_Left,14},
		{RO_LeftFine,50},
		{RO_Top,2},
		{RO_TopFine,18},
		{RO_Width,10},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_PROTECT_NONE},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	protect_set_toggle[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PROTECT_SET_TOGGLE},
		{RO_Left,14},
		{RO_LeftFine,50},
		{RO_Top,4},
		{RO_TopFine,18},
		{RO_Width,10},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_PROTECT_TOGGLE},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	protect_set_revert[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PROTECT_SET_REVERT},
		{RO_Left,14},
		{RO_LeftFine,50},
		{RO_Top,6},
		{RO_TopFine,18},
		{RO_Width,10},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_PROTECT_REVERT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	protect_old_text[]={
		{RO_Type,OBJECT_BORDER},
		{RO_BorderType,BORDER_NONE},
		{RO_Left,33},
		{RO_LeftFine,80},
		{RO_Top,1},
		{RO_TopFine,18},
		{RO_Width,8},
		{RO_Height,1},
		{RO_TextNum,STR_PROTECT_OLD},
		{RO_TextPos,TEXTPOS_LEFT},
		{TAG_END,0}},
	protect_new_text[]={
		{RO_Type,OBJECT_BORDER},
		{RO_BorderType,BORDER_NONE},
		{RO_Left,33},
		{RO_LeftFine,80},
		{RO_Top,3},
		{RO_TopFine,18},
		{RO_Width,8},
		{RO_Height,1},
		{RO_TextNum,STR_PROTECT_NEW},
		{RO_TextPos,TEXTPOS_LEFT},
		{TAG_END,0}},
	protect_mask_text[]={
		{RO_Type,OBJECT_BORDER},
		{RO_BorderType,BORDER_NONE},
		{RO_Left,33},
		{RO_LeftFine,80},
		{RO_Top,5},
		{RO_TopFine,18},
		{RO_Width,8},
		{RO_Height,1},
		{RO_TextNum,STR_PROTECT_MASK},
		{RO_TextPos,TEXTPOS_LEFT},
		{TAG_END,0}},

	protect_okay_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PROTECT_OKAY},
		{RO_Top,8},
		{RO_TopFine,45},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_OKAY},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	protect_all_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PROTECT_ALL},
		{RO_Left,15},
		{RO_LeftFine,36},
		{RO_Top,8},
		{RO_TopFine,45},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_ALL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	protect_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PROTECT_CANCEL},
		{RO_Left,31},
		{RO_LeftFine,67},
		{RO_Top,8},
		{RO_TopFine,45},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_CANCEL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}};

struct TagItem
	*protect_gadgets[]={
		protect_bits[0],
		protect_bits[1],
		protect_bits[2],
		protect_bits[3],
		protect_bits[4],
		protect_bits[5],
		protect_bits[6],
		protect_bits[7],
		protect_set_all,
		protect_set_none,
		protect_set_toggle,
		protect_set_revert,
		protect_okay_gadget,
		protect_all_gadget,
		protect_cancel_gadget,
		NULL};

char prot_template[9];

int getprotectdata(prot,mask)
int *prot,*mask;
{
	ULONG class;
	USHORT gadgetid,code;
	int oldprot,a,x,y;
	struct Gadget *gadlist;
	Object_Border *old_text,*new_text,*mask_text;
	struct Window *pwindow;

	for (a=0;a<8;a++) prot_template[a]=globstring[STR_PROTBIT_ALLBITS][7-a];
	prot_template[8]=0;

	fix_requester(&protect_req,globstring[STR_SELECT_PROTECTION_BITS]);

	for (a=0;a<8;a++) {
		CopyMem((char *)protect_bit_template,
			(char *)protect_bits[a],
			sizeof(protect_bit_template));	

		protect_bits[a][2].ti_Data=7-a;
		protect_bits[a][3].ti_Data=a;
		protect_bits[a][4].ti_Data=a*5;
		protect_bits[a][5].ti_Data=STR_PROTECT_HIDDEN+a;
	}

	if (!(pwindow=OpenRequester(&protect_req)) ||
		!(gadlist=addreqgadgets(&protect_req,protect_gadgets,0,NULL)) ||
		!(old_text=AddRequesterObject(&protect_req,protect_old_text)) ||
		!(new_text=AddRequesterObject(&protect_req,protect_new_text)) ||
		!(mask_text=AddRequesterObject(&protect_req,protect_mask_text))) {
		CloseRequester(&protect_req);
		return(0);
	}
	RefreshRequesterObject(&protect_req,NULL);

	do_prot_display(*prot,old_text,NULL);
	do_prot_display(*prot,new_text,gadlist);
	do_prot_display((*mask)^15,mask_text,NULL);
	oldprot=*prot;

	FOREVER {
		while ((IMsg=(struct IntuiMessage *)GetMsg(pwindow->UserPort))) {
			class=IMsg->Class; code=IMsg->Code;
			x=IMsg->MouseX; y=IMsg->MouseY;
			if (class==IDCMP_GADGETUP)
				gadgetid=((struct Gadget *) IMsg->IAddress)->GadgetID;
			ReplyMsg((struct Message *) IMsg);

			switch (class) {
				case IDCMP_MOUSEBUTTONS:
					if (code==SELECTDOWN &&
						x>=mask_text->ob_left && x<(mask_text->ob_left+mask_text->ob_width) &&
						y>=mask_text->ob_top && y<(mask_text->ob_top+mask_text->ob_height)) {
						a=7-((x-mask_text->ob_left)/protect_req.rb_font->tf_XSize);
						*mask^=(1<<a);
						do_prot_display((*mask)^15,mask_text,NULL);
					}
					break;

				case IDCMP_VANILLAKEY:
					for (a=0;a<8;a++) {
						if (tolower(code)==prot_template[a]) {
							if (isupper(code)) {
								*mask^=(1<<a);
								do_prot_display((*mask)^15,mask_text,NULL);
							}
							else {
								*prot^=(1<<a);
								do_prot_display(*prot,new_text,gadlist);
							}
							break;
						}
					}
					gadgetid=0;
					switch (code) {
						case '\r':
							gadgetid=PROTECT_OKAY;
							break;
						case 0x1b:
							gadgetid=PROTECT_CANCEL;
							break;
						case 'l':
							gadgetid=PROTECT_ALL;
							break;
					}
					if (!gadgetid) break;
					hilite_req_gadget(pwindow,gadgetid);

				case IDCMP_GADGETUP:
					if (gadgetid<8) {
						*prot^=(1<<gadgetid);
						do_prot_display(*prot,new_text,NULL);
						break;
					}
					switch (gadgetid) {
						case PROTECT_SET_ALL:
							*prot=240;
							do_prot_display(*prot,new_text,gadlist);
							break;
						case PROTECT_SET_NONE:
							*prot=15;
							do_prot_display(*prot,new_text,gadlist);
							break;
						case PROTECT_SET_TOGGLE:
							*prot=(~*prot)&255;
							do_prot_display(*prot,new_text,gadlist);
							break;
						case PROTECT_SET_REVERT:
							*prot=oldprot;
							do_prot_display(*prot,new_text,gadlist);
							break;

						case PROTECT_OKAY:
						case PROTECT_ALL:
						case PROTECT_CANCEL:
							CloseRequester(&protect_req);
							return((gadgetid==PROTECT_CANCEL)?0:
								((gadgetid==PROTECT_ALL)?2:1));
					}
					break;
			}
		}
		Wait(1<<pwindow->UserPort->mp_SigBit);
	}
}

void do_prot_display(bits,border,gadgets)
int bits;
Object_Border *border;
struct Gadget *gadgets;
{
	char bitbuf[10];
	int gadnum;
	struct Gadget *gad;

	getprot(bits,bitbuf);

	ObjectText(&protect_req,
		border->ob_left,
		border->ob_top,
		border->ob_width,
		border->ob_height,
		bitbuf,
		TEXTPOS_CENTER);

	if (gadgets) {
		gad=gadgets;
		for (gadnum=0;gadnum<8 && gad;gadnum++) {
			if (bits&(1<<gad->GadgetID)) {
				if (gadnum<4) gad->Flags|=GFLG_SELECTED;
				else gad->Flags&=~GFLG_SELECTED;
			}
			else {
				if (gadnum<4) gad->Flags&=~GFLG_SELECTED;
				else gad->Flags|=GFLG_SELECTED;
			}
			gad=gad->NextGadget;
		}
		RefreshGList(gadgets,protect_req.rb_window,NULL,8);
	}
}
