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
#include "searchdata.h"

enum {
	SEARCH_TEXT,
	SEARCH_CASE,
	SEARCH_WILD,
	SEARCH_ONLYWORD,
	SEARCH_OKAY,
	SEARCH_CANCEL};

struct TagItem
	search_title[]={
		{RO_Type,OBJECT_BORDER},
		{RO_BorderType,BORDER_NONE},
		{RO_LeftFine,4},
		{RO_Width,38},
		{RO_WidthFine,32},
		{RO_TopFine,-4},
		{RO_Height,1},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_TextNum,STR_ENTER_SEARCH_STRING},
		{TAG_END,0}},

	search_text_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,SEARCH_TEXT},
		{RO_Top,1},
		{RO_LeftFine,4},
		{RO_Width,38},
		{RO_WidthFine,32},
		{RO_Height,1},
		{RO_StringLen,80},
		{RO_StringBuf,0},
		{RO_StringUndo,(ULONG)str_undobuffer},
		{TAG_END,0}},

	search_case_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,SEARCH_CASE},
		{RO_Top,2},
		{RO_TopFine,6},
		{RO_TextNum,STR_SEARCH_NO_CASE},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{RO_BoolOn,FALSE},
		{TAG_END,0}},
	search_wild_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,SEARCH_WILD},
		{RO_Top,3},
		{RO_TopFine,11},
		{RO_TextNum,STR_SEARCH_WILD},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{RO_BoolOn,FALSE},
		{TAG_END,0}},
	search_onlyword_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,SEARCH_ONLYWORD},
		{RO_Top,4},
		{RO_TopFine,16},
		{RO_TextNum,STR_SEARCH_ONLYWORD},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{RO_BoolOn,FALSE},
		{TAG_END,0}},

	search_okay_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,SEARCH_OKAY},
		{RO_Top,5},
		{RO_TopFine,22},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_Width,10},
		{RO_TextNum,STR_OKAY},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	search_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,SEARCH_CANCEL},
		{RO_Left,28},
		{RO_LeftFine,38},
		{RO_Top,5},
		{RO_TopFine,22},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_Width,10},
		{RO_TextNum,STR_CANCEL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	*search_gadgets[]={
		search_text_gadget,
		search_case_gadget,
		search_wild_gadget,
		search_onlyword_gadget,
		search_okay_gadget,
		search_cancel_gadget,
		NULL};

int get_search_data(buffer,flagptr,window,font)
char *buffer;
int *flagptr;
struct Window *window;
struct TextFont *font;
{
	ULONG class;
	USHORT gadgetid;
	struct Window *swindow;
	struct Gadget *gadlist;
	struct RequesterBase search_req;
	struct StringExtend extend;
	int a,flags;

	flags=*flagptr;

	search_req.rb_width=38;
	search_req.rb_height=6;
	search_req.rb_widthfine=68;
	search_req.rb_heightfine=40;
	search_req.rb_leftoffset=14;
	search_req.rb_topoffset=8;

	search_req.rb_fg=
		(window->UserData)?window->UserData[SEARCH_COL_FG]:screen_pens[(int)config->requestfg].pen;
	search_req.rb_bg=
		(window->UserData)?window->UserData[SEARCH_COL_BG]:screen_pens[(int)config->requestbg].pen;
	search_req.rb_shine=
		(window->UserData)?window->UserData[SEARCH_COL_SHINE]:screen_pens[(int)config->gadgettopcol].pen;
	search_req.rb_shadow=
		(window->UserData)?window->UserData[SEARCH_COL_SHADOW]:screen_pens[(int)config->gadgetbotcol].pen;

	search_req.rb_font=font;
	search_req.rb_screen=window->WScreen;
	search_req.rb_extend=&extend;

	extend.Font=font;
	extend.Pens[0]=
		(window->UserData)?window->UserData[SEARCH_COL_STRINGFG]:screen_pens[(int)config->stringfgcol].pen;
	extend.Pens[1]=
		(window->UserData)?window->UserData[SEARCH_COL_STRINGBG]:screen_pens[(int)config->stringbgcol].pen;
	extend.ActivePens[0]=
		(window->UserData)?window->UserData[SEARCH_COL_STRINGSELFG]:screen_pens[(int)config->stringselfgcol].pen;
	extend.ActivePens[1]=
		(window->UserData)?window->UserData[SEARCH_COL_STRINGSELBG]:screen_pens[(int)config->stringselbgcol].pen;
	extend.InitialModes=0;
	extend.EditHook=NULL;
	extend.WorkBuffer=NULL;
	for (a=0;a<4;a++) extend.Reserved[a]=0;

	search_req.rb_idcmpflags=IDCMP_GADGETUP|IDCMP_MOUSEBUTTONS;

	search_req.rb_screenname=NULL;
	search_req.rb_memory=NULL;
	search_req.rb_objects=NULL;
	search_req.rb_privateflags=0;
	search_req.rb_privatedata=NULL;

	if (config->generalscreenflags&SCR_GENERAL_REQDRAG) {
		search_req.rb_title=globstring[STR_ENTER_SEARCH_STRING];
		search_req.rb_flags=RBF_BORDERS|RBF_STRINGS;
	}
	else {
		search_req.rb_title=NULL;
		search_req.rb_flags=RBF_STRINGS;
	}

	search_req.rb_flags|=RBF_WINDOWCENTER;
	search_req.rb_window=window;

	search_req.rb_string_table=globstring;

	set_reqobject(search_text_gadget,RO_StringBuf,(ULONG)buffer);
	set_reqobject(search_case_gadget,RO_BoolOn,(flags&SEARCH_NOCASE)?TRUE:FALSE);
	set_reqobject(search_wild_gadget,RO_BoolOn,(flags&SEARCH_WILDCARD)?TRUE:FALSE);
	set_reqobject(search_onlyword_gadget,RO_BoolOn,(flags&SEARCH_ONLYWORDS)?TRUE:FALSE);

	if (!(swindow=OpenRequester(&search_req)) ||
		!(gadlist=addreqgadgets(&search_req,search_gadgets,0,NULL))) {
		CloseRequester(&search_req);
		return(0);
	}
	AddRequesterObject(&search_req,search_title);
	RefreshRequesterObject(&search_req,NULL);

	ActivateStrGad(gadlist,swindow);

	FOREVER {
		while ((IMsg=(struct IntuiMessage *)GetMsg(swindow->UserPort))) {
			class=IMsg->Class;
			if (class==IDCMP_GADGETUP)
				gadgetid=((struct Gadget *) IMsg->IAddress)->GadgetID;
			ReplyMsg((struct Message *) IMsg);

			switch (class) {
				case IDCMP_MOUSEBUTTONS:
					ActivateStrGad(gadlist,swindow);
					break;

				case IDCMP_GADGETUP:
					switch (gadgetid) {
						case SEARCH_CASE:
							flags^=SEARCH_NOCASE;
							ActivateStrGad(gadlist,swindow);
							break;
						case SEARCH_WILD:
							flags^=SEARCH_WILDCARD;
							ActivateStrGad(gadlist,swindow);
							break;
						case SEARCH_ONLYWORD:
							flags^=SEARCH_ONLYWORDS;
							ActivateStrGad(gadlist,swindow);
							break;

						case SEARCH_TEXT:
						case SEARCH_OKAY:
						case SEARCH_CANCEL:
							*flagptr=flags;
							CloseRequester(&search_req);
							return((gadgetid!=SEARCH_CANCEL));
					}
					break;
			}
		}
		Wait(1<<swindow->UserPort->mp_SigBit);
	}
}
