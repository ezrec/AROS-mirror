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

#include "iconinfo.h"

void main(argc,argv)
int argc;
char *argv[];
{
	struct VisInfo vis;
	char *dummy_args[16],*port,stringname[80];
	struct DOpusStartup *startup=NULL;
	int a,arg;
	struct StringData stringdata;

	if (argc==0) {
		int arg;

		startup=(struct DOpusStartup *)argv;
		for (arg=0;arg<startup->wbstartup.sm_NumArgs && arg<16;arg++) {
			dummy_args[arg]=startup->wbstartup.sm_ArgList[arg].wa_Name;
			++argc;
		}
		for (;arg<16;arg++) dummy_args[arg]=NULL;
		argv=dummy_args;
	}

	DOpusBase=NULL;

	if (argc<2 ||
		!(DOpusBase=(struct DOpusBase *)OpenLibrary("dopus.library",18)) ||
		!(IconBase=OpenLibrary("icon.library",0))) {
		if (DOpusBase) CloseLibrary((struct Library *)DOpusBase);
		_exit(0);
	}

	IntuitionBase=DOpusBase->IntuitionBase;
	GfxBase=DOpusBase->GfxBase;
	LayersBase=(struct Library *)DOpusBase->LayersBase;

	if (argc>2 && argv[1][0]=='&') {
		arg=2;
		port=&argv[1][1];
	}
	else {
		arg=1;
		port=NULL;
	}
	get_vis_info(&vis,port);

	stringdata.default_table=default_strings;
	stringdata.string_count=STR_STRING_COUNT;
	stringdata.string_table=NULL;
	stringdata.string_buffer=NULL;
	stringdata.min_version=STRING_VERSION;

	if (vis.vi_language)
		lsprintf(stringname,"DOpus:Modules/S/DM_Icon_%s.STR",vis.vi_language);
	else stringname[0]=0;

	if (ReadStringFile(&stringdata,stringname)) {
		string_table=stringdata.string_table;

		setup_strings();

		a=doiconinfo(&vis,argv[arg]);
		if (startup) startup->retcode=a;
	}
	FreeStringFile(&stringdata);

	CloseLibrary(IconBase);
	CloseLibrary((struct Library *)DOpusBase);
	_exit(0);
}

void get_vis_info(vis,portname)
struct VisInfo *vis;
char *portname;
{
	vis->vi_fg=1; vis->vi_bg=0;
	vis->vi_shine=2; vis->vi_shadow=1;
	vis->vi_font=NULL;
	vis->vi_screen=NULL;
	vis->vi_stringcol[0]=1; vis->vi_stringcol[1]=0;
	vis->vi_activestringcol[0]=1; vis->vi_activestringcol[1]=0;
	vis->vi_flags=VISF_8POINTFONT|VISF_WINDOW;
	vis->vi_language=NULL;

	if (dopus_message(DOPUSMSG_GETVIS,(APTR)vis,portname)) return;

	if (IntuitionBase->LibNode.lib_Version>35) {
		struct DrawInfo *drinfo;
		struct Screen *pub;

		if (pub=LockPubScreen(NULL)) {
			drinfo=GetScreenDrawInfo(pub);
			vis->vi_shine=drinfo->dri_Pens[SHINEPEN];
			vis->vi_shadow=drinfo->dri_Pens[SHADOWPEN];
			vis->vi_fg=drinfo->dri_Pens[TEXTPEN];
			vis->vi_bg=drinfo->dri_Pens[BACKGROUNDPEN];
			FreeScreenDrawInfo(pub,drinfo);
			UnlockPubScreen(NULL,pub);
		}
	}
}

dopus_message(cmd,data,portname)
int cmd;
APTR data;
char *portname;
{
	struct MsgPort *port,*replyport;
	struct DOpusMessage msg;

	Forbid();
	if (portname && portname[0] &&
		(port=FindPort(portname)) &&
		(replyport=LCreatePort(NULL,0))) {
		msg.msg.mn_Node.ln_Type=NT_MESSAGE;
		msg.msg.mn_Node.ln_Name=NULL;
		msg.msg.mn_ReplyPort=replyport;
		msg.msg.mn_Length=(UWORD)sizeof(struct DOpusMessage);
		msg.command=cmd;
		msg.data=data;
		PutMsg(port,(struct Message *)&msg);
		Permit();
		WaitPort(replyport);
		GetMsg(replyport);
		LDeletePort(replyport);
		return(1);
	}
	Permit();
	return(0);
}

void fill_out_req(req,vis)
struct RequesterBase *req;
struct VisInfo *vis;
{
	req->rb_fg=vis->vi_fg;
	req->rb_bg=vis->vi_bg;
	req->rb_shine=vis->vi_shine;
	req->rb_shadow=vis->vi_shadow;
	req->rb_font=vis->vi_font;
	req->rb_flags&=~RBF_WINDOWCENTER;
	if (vis->vi_screen) {
		if (vis->vi_flags&VISF_WINDOW) {
			req->rb_screen=((struct Window *)vis->vi_screen)->WScreen;
			req->rb_window=(struct Window *)vis->vi_screen;
			req->rb_flags|=RBF_WINDOWCENTER;
		}
		else req->rb_screen=vis->vi_screen;
	}
	else req->rb_screen=NULL;
}

struct Gadget *addreqgadgets(reqbase,gadgets)
struct RequesterBase *reqbase;
struct TagItem **gadgets;
{
	int gad;
	struct Gadget *gadget=NULL,*newgadget,*firstgadget;

	for (gad=0;;gad++) {
		if (!gadgets[gad]) break;
		if (!(newgadget=(struct Gadget *)
			AddRequesterObject(reqbase,gadgets[gad]))) return(NULL);
		if (gadget) gadget->NextGadget=newgadget;
		else firstgadget=newgadget;
		gadget=newgadget;
	}
	AddGadgets(reqbase->rb_window,firstgadget,NULL,gad,reqbase->rb_shine,reqbase->rb_shadow,1);
	return(firstgadget);
}

void fix_listview(reqbase,list)
struct RequesterBase *reqbase;
struct DOpusListView *list;
{
	list->window=reqbase->rb_window;
	list->flags|=DLVF_SLOW;
	list->sliderwidth=8;
	list->slidercol=reqbase->rb_fg;
	list->sliderbgcol=reqbase->rb_bg;
	list->textcol=reqbase->rb_fg;
	list->boxhi=reqbase->rb_shine;
	list->boxlo=reqbase->rb_shadow;
	list->arrowfg=reqbase->rb_fg;
	list->arrowbg=reqbase->rb_bg;
	list->itemfg=reqbase->rb_fg;
	list->itembg=reqbase->rb_bg;
}

void setup_strings()
{
	int a,b;

	for (a=0;a<5;a++)
		for (b=0;b<6;b++) specific_gadtext[a][b]=NULL;
	for (a=0;a<5;a++) specific_gadtext[0][a]=string_table[STR_BLOCKS+a];
	specific_gadtext[1][4]=string_table[STR_LAST_CHANGED];

	for (a=2;a<4;a++) {
		specific_gadtext[a][0]=string_table[STR_BLOCKS];
		specific_gadtext[a][1]=string_table[STR_BYTES];
		specific_gadtext[a][3]=string_table[STR_STACK];
		specific_gadtext[a][4]=string_table[STR_LAST_CHANGED];
	}

	specific_gadtext[4][4]=string_table[STR_LAST_CHANGED];

	for (a=0;a<8;a++) icon_type_names[a]=string_table[STR_DISK+a];
}
