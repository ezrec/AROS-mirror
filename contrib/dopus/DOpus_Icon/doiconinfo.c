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

struct TagItem
	iconinfo_save_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,II_SAVE},
		{RO_LeftFine,40},
		{RO_TopFine,0},
		{RO_WidthFine,100},
		{RO_HeightFine,12},
		{RO_TextNum,STR_SAVE},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	iconinfo_skip_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,II_SKIP},
		{RO_LeftFine,220},
		{RO_TopFine,0},
		{RO_WidthFine,100},
		{RO_HeightFine,12},
		{RO_TextNum,STR_SKIP},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	iconinfo_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,II_CANCEL},
		{RO_LeftFine,400},
		{RO_TopFine,0},
		{RO_WidthFine,100},
		{RO_HeightFine,12},
		{RO_TextNum,STR_CANCEL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	iconinfo_edittooltype_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,II_TOOLTYPE},
		{RO_LeftFine,118},
		{RO_TopFine,166},
		{RO_WidthFine,400},
		{RO_HeightFine,8},
		{RO_StringLen,128},
		{TAG_END,0}},
	iconinfo_deletetooltype_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,II_DELETE},
		{RO_LeftFine,8},
		{RO_TopFine,164},
		{RO_WidthFine,100},
		{RO_HeightFine,12},
		{RO_TextNum,STR_DELETE},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	iconinfo_newtooltype_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,II_NEW},
		{RO_LeftFine,8},
		{RO_TopFine,150},
		{RO_WidthFine,100},
		{RO_HeightFine,12},
		{RO_TextNum,STR_NEW},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	iconinfo_tooltype_list[]={
		{RO_Type,OBJECT_LISTVIEW},
		{RO_ListViewID,0},
		{RO_LeftFine,116},
		{RO_TopFine,121},
		{RO_WidthFine,388},
		{RO_HeightFine,40},
		{RO_TextNum,STR_TOOLTYPES},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	iconinfo_stack_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,II_STACK},
		{RO_LeftFine,102},
		{RO_TopFine,64},
		{RO_WidthFine,48},
		{RO_HeightFine,8},
		{RO_StringLen,12},
		{TAG_END,0}},

	iconinfo_comment_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,II_COMMENT},
		{RO_LeftFine,118},
		{RO_TopFine,108},
		{RO_WidthFine,400},
		{RO_HeightFine,8},
		{RO_StringLen,80},
		{RO_TextNum,STR_COMMENT},
		{RO_TextPos,TEXTPOS_LEFT},
		{TAG_END,0}},

	iconinfo_defaulttool_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,II_DEFAULTTOOL},
		{RO_LeftFine,118},
		{RO_TopFine,108},
		{RO_WidthFine,400},
		{RO_HeightFine,8},
		{RO_StringLen,257},
		{RO_TextNum,STR_DEFAULT_TOOL},
		{RO_TextPos,TEXTPOS_LEFT},
		{TAG_END,0}},

	iconinfo_protectbit_template[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,0},
		{RO_LeftFine,408},
		{RO_TopFine,0},
		{RO_TextNum,0},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_BoolOn,FALSE},
		{TAG_END,0}},

	*iconinfo_general_gadgets[]={
		iconinfo_save_gadget,
		iconinfo_skip_gadget,
		iconinfo_cancel_gadget,
		NULL},

	*iconinfo_short_general_gadgets[]={
		NULL},

	*iconinfo_tooltype_gadgets[]={
		iconinfo_edittooltype_gadget,
		iconinfo_deletetooltype_gadget,
		iconinfo_newtooltype_gadget,
		NULL};

struct TextAttr topaz={"topaz.font",8,0,0};

struct IntuiText
	save_text={1,0,JAM1,1,1,&topaz,NULL,NULL},
	skip_text={1,0,JAM1,1,1,&topaz,NULL,NULL},
	cancel_text={1,0,JAM1,1,1,&topaz,NULL,NULL};

struct MenuItem
	save_item={
		NULL,0,0,100+COMMWIDTH,10,ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ,0,
		(APTR)&save_text,NULL,'S',NULL,0xffff},
	skip_item={
		&save_item,0,11,100+COMMWIDTH,10,ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ,0,
		(APTR)&skip_text,NULL,'K',NULL,0xffff},
	cancel_item={
		&skip_item,0,22,100+COMMWIDTH,10,ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ,0,
		(APTR)&cancel_text,NULL,'C',NULL,0xffff};

struct Menu
	icon_menu={NULL,8,0,100,0,MENUENABLED,NULL,&cancel_item};

int doiconinfo(vis,name)
struct VisInfo *vis;
char *name;
{
	struct DiskObject *dobj;
	struct DOpusListView *view;
	char
		buf[256],namebuf[256],inamebuf[33],*ptr,date[11],time[11],*olddeftool,
		**ttarray,**oldtooltypes,**tttemp;
	struct DOpusDateTime datetime;
	ULONG class;
	UWORD code;
	int a,b,x,y,gadgetid,ret,gotid=0,/*newdef,*/ttcount=0,curtt=-1,compflag=0;
	BPTR lock;
	struct InfoData __aligned infodata;
	struct FileInfoBlock __aligned fileinfo;
	struct RequesterBase icon_req;
	struct StringExtend stringex;
	struct Window *window;
	struct RastPort *rp;
	struct Gadget
		//*icongad,
		*gad,	
		*tooltypegads=NULL,
		*protectbitgads=NULL,
		*stackgad=NULL,
		*commentgad=NULL,
		*defaulttoolgad=NULL;
	char *stack_buf,*comment_buf,*tooltype_buf,*defaulttool_buf;
	struct DOpusListView *tooltypelist;
	struct IntuiMessage *msg;
	struct Rectangle icon_rec;
	int textxoff,textyoff,small=0;

	ttarray=NULL;
	tooltypelist=NULL;

	strcpy(namebuf,name); if ((ptr=strstr(namebuf,".info"))) *ptr=0;
	if (!(dobj=GetDiskObject(namebuf))) return(-2);
	//icongad=(struct Gadget *)&(dobj->do_Gadget);
	oldtooltypes=(char **)dobj->do_ToolTypes;

	switch (dobj->do_Type) {
		case WBDISK:
			strcpy(buf,name);
			doroot(buf);
			doroot(namebuf);
			if (!(getroot(buf,&(datetime.dat_Stamp)))) {
				FreeDiskObject(dobj);
				return(-2);
			}
		case WBGARBAGE: icon_req.rb_heightfine=150; break;
		case WBTOOL:
		case WBPROJECT:
		case WBDRAWER: icon_req.rb_heightfine=196; break;
		default:
			FreeDiskObject(dobj);
			return(-2);
	}

	if (lock=Lock(namebuf,ACCESS_READ)) {
		Examine(lock,&fileinfo);
		Info(lock,&infodata);
		UnLock(lock);

		strcpy(inamebuf,BaseName(namebuf));

		if (dobj->do_Type!=WBDISK)
			CopyMem((char *)&(fileinfo.fib_Date),
				(char *)&(datetime.dat_Stamp),
				sizeof(struct DateStamp));

		datetime.dat_Format=FORMAT_DOS;
		datetime.dat_StrDay=NULL;
		datetime.dat_StrDate=date;
		datetime.dat_StrTime=time;
		datetime.dat_Flags=0;
		StampToStr(&datetime);

		strcpy(buf,BaseName(name)); if (ptr=strstr(buf,".info")) *ptr=0;
		gotid=1;
	}
	else if (dobj->do_Type==WBDISK) {
		FreeDiskObject(dobj);
		return(-2);
	}
	else {
		strcpy(inamebuf,BaseName(name)); if (ptr=strstr(inamebuf,".info")) *ptr=0;
		strcpy(date,"???"); time[0]=0;
	}

	if (dobj->do_Type==WBPROJECT && gotid) icon_req.rb_heightfine+=14;

	iconinfo_save_gadget[4].ti_Data=icon_req.rb_heightfine-16;
	iconinfo_skip_gadget[4].ti_Data=icon_req.rb_heightfine-16;
	iconinfo_cancel_gadget[4].ti_Data=icon_req.rb_heightfine-16;

	icon_req.rb_width=0;
	icon_req.rb_height=0;
	icon_req.rb_widthfine=540;
	icon_req.rb_leftoffset=4;
	icon_req.rb_topoffset=0;
	icon_req.rb_flags=0;

	fill_out_req(&icon_req,vis);

	icon_req.rb_privateflags=0;
	icon_req.rb_screenname=NULL;

	if (icon_req.rb_screen && !(vis->vi_flags&VISF_BORDERS)) {
		icon_req.rb_flags|=RBF_STRINGS;
		icon_req.rb_title=NULL;
	}
	else {
		icon_req.rb_flags|=RBF_BORDERS|RBF_CLOSEGAD|RBF_STRINGS;
		icon_req.rb_title=inamebuf;
	}

	icon_req.rb_extend=&stringex;
	icon_req.rb_idcmpflags=
		IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEBUTTONS|
		IDCMP_RAWKEY|IDCMP_MOUSEMOVE|IDCMP_CLOSEWINDOW|IDCMP_MENUPICK;
	icon_req.rb_string_table=string_table;

	for (a=0;a<2;a++) {
		stringex.Pens[a]=vis->vi_stringcol[a];
		stringex.ActivePens[a]=vis->vi_activestringcol[a];
	}
	stringex.InitialModes=0;
	stringex.EditHook=NULL;
	stringex.WorkBuffer=NULL;
	for (a=0;a<4;a++) stringex.Reserved[a]=0;

	for (a=0;a<3;a++) {
		if (window=OpenRequester(&icon_req)) break;
		CloseRequester(&icon_req);
		switch (a) {
			case 0:
				icon_req.rb_flags&=~RBF_BORDERS;
				icon_req.rb_title=NULL;
				break;
			case 1:
				icon_req.rb_heightfine-=17;
				small=1;
				break;
			case 2:
				FreeDiskObject(dobj);
				return(0);
		}
	}

	if (!(addreqgadgets(&icon_req,
		((small)?iconinfo_short_general_gadgets:iconinfo_general_gadgets)))) {
		CloseRequester(&icon_req);
		FreeDiskObject(dobj);
		return(0);
	}

	textxoff=window->BorderLeft+5;
	textyoff=window->BorderTop;

	icon_rec.MinX=170+textxoff;
	icon_rec.MinY=16+textyoff;
	icon_rec.MaxX=369+textxoff;
	icon_rec.MaxY=85+textyoff;

	rp=window->RPort;
	if (rp->Font->tf_YSize>8 || rp->Font->tf_Flags&FPF_PROPORTIONAL) {
		struct TextFont *icon_font;

		if (icon_font=OpenFont(&topaz)) {
			SetFont(rp,icon_font);
			icon_req.rb_font=icon_font;
			icon_req.rb_privateflags=RBP_CLOSEFONT;
		}
	}

	if (small) {
		window->Flags&=~WFLG_RMBTRAP;
		if (IntuitionBase->LibNode.lib_Version>=39) {
			save_text.FrontPen=1;
			skip_text.FrontPen=1;
			cancel_text.FrontPen=1;
		}
		else {
			save_text.FrontPen=0;
			skip_text.FrontPen=0;
			cancel_text.FrontPen=0;
		}
		save_text.IText=string_table[STR_SAVE];
		skip_text.IText=string_table[STR_SKIP];
		cancel_text.IText=string_table[STR_CANCEL];
		icon_menu.MenuName=string_table[STR_ICON];
		SetMenuStrip(window,&icon_menu);
	}


	if (gotid && dobj->do_Type!=WBDISK) {
		struct TagItem *iconinfo_protectbit_gadgets[7];

		for (a=0;a<6;a++) {
			if (iconinfo_protectbit_gadgets[a]=(struct TagItem *)
				LAllocRemember(&icon_req.rb_memory,sizeof(iconinfo_protectbit_template),MEMF_CLEAR)) {
				CopyMem((char *)iconinfo_protectbit_template,
					(char *)iconinfo_protectbit_gadgets[a],
					sizeof(iconinfo_protectbit_template));
				iconinfo_protectbit_gadgets[a][2].ti_Data=a+10;
				iconinfo_protectbit_gadgets[a][4].ti_Data=15+(a*12);
				iconinfo_protectbit_gadgets[a][5].ti_Data=STR_SCRIPT+a;
				if (fileinfo.fib_Protection&protect_flags[a]) {
					if (protect_flags[a]>15) iconinfo_protectbit_gadgets[a][7].ti_Data=TRUE;
				}
				else if (protect_flags[a]<16) iconinfo_protectbit_gadgets[a][7].ti_Data=TRUE;
			}
		}

		iconinfo_protectbit_gadgets[6]=NULL;
		protectbitgads=addreqgadgets(&icon_req,iconinfo_protectbit_gadgets);
	}

	if (dobj->do_Type==WBTOOL || dobj->do_Type==WBPROJECT) {
		struct TagItem *temp_gadgets[2];

		temp_gadgets[0]=iconinfo_stack_gadget;
		temp_gadgets[1]=NULL;

		if (stackgad=addreqgadgets(&icon_req,temp_gadgets)) {
			stack_buf=((struct StringInfo *)stackgad->SpecialInfo)->Buffer;
			if (dobj->do_StackSize<4096) dobj->do_StackSize=4096;
			lsprintf(stack_buf,"%ld",(long int)dobj->do_StackSize);
		}
	}

	if (dobj->do_Type==WBDISK || dobj->do_Type==WBPROJECT || gotid) {
		struct TagItem *temp_gadgets[2];

		temp_gadgets[1]=NULL;

		if (gotid && dobj->do_Type!=WBDISK) {
			temp_gadgets[0]=iconinfo_comment_gadget;
			if (commentgad=addreqgadgets(&icon_req,temp_gadgets)) {
				comment_buf=((struct StringInfo *)commentgad->SpecialInfo)->Buffer;
				strcpy(comment_buf,fileinfo.fib_Comment);
			}
		}
		if (dobj->do_Type==WBDISK || dobj->do_Type==WBPROJECT) {
			if (commentgad) {
				iconinfo_defaulttool_gadget[4].ti_Data+=14;
				iconinfo_edittooltype_gadget[4].ti_Data+=14;
				iconinfo_deletetooltype_gadget[4].ti_Data+=14;
				iconinfo_newtooltype_gadget[4].ti_Data+=14;
				iconinfo_tooltype_list[3].ti_Data+=14;
			}
			temp_gadgets[0]=iconinfo_defaulttool_gadget;
			if (defaulttoolgad=addreqgadgets(&icon_req,temp_gadgets)) {
				defaulttool_buf=((struct StringInfo *)defaulttoolgad->SpecialInfo)->Buffer;
				if (dobj->do_DefaultTool) strcpy(defaulttool_buf,dobj->do_DefaultTool);
			}
		}
	}
	else {
		if (dobj->do_Type!=WBDISK) {
			UScoreText(rp,string_table[STR_COMMENT],
				117+textxoff-((strlen(string_table[STR_COMMENT])+1)*8),
				114+textyoff,-1);
		}
		UScoreText(rp,"---",118+textxoff,114+textyoff,-1);
	}

	Do3DBox(rp,
		icon_rec.MinX,icon_rec.MinY,
		(icon_rec.MaxX-icon_rec.MinX+1),(icon_rec.MaxY-icon_rec.MinY+1),
		vis->vi_shine,vis->vi_shadow);
	drawicon(window,dobj,FALSE,&icon_rec);

	StrCombine(buf,inamebuf," (",256);
	strcat(buf,icon_type_names[dobj->do_Type-1]);
	strcat(buf,")");

	SetAPen(rp,vis->vi_fg);
	UScoreText(rp,buf,((540-(strlen(buf)*8))/2)+textxoff,11+textyoff,-1);

	y=40+textyoff;
	for (a=0;a<6;a++) {
		ptr=specific_gadtext[dobj->do_Type-1][a];
		if (ptr) {
			if (a<4) x=94+textxoff;
			else x=109+textxoff;
			UScoreText(rp,ptr,x-(strlen(ptr)*8)-8,y,-1);
			Text(rp,":",1);
		}
		y+=10;
		if (a==3) y+=20;
		else if (a>3) y+=4;
	}

	UScoreText(rp,date,118+textxoff,100+textyoff,-1);
	Text(rp," ",1);
	Text(rp,time,strlen(time));

	switch (dobj->do_Type) {
		case WBDISK:
			lsprintf(buf,"%ld",(long int)infodata.id_NumBlocks);
			UScoreText(rp,buf,102+textxoff,40+textyoff,-1);
			lsprintf(buf,"%ld",(long int)infodata.id_NumBlocksUsed);
			UScoreText(rp,buf,102+textxoff,50+textyoff,-1);
			lsprintf(buf,"%ld",(long int)infodata.id_NumBlocks-infodata.id_NumBlocksUsed);
			UScoreText(rp,buf,102+textxoff,60+textyoff,-1);
			lsprintf(buf,"%ld",(long int)infodata.id_BytesPerBlock);
			UScoreText(rp,buf,102+textxoff,70+textyoff,-1);

			switch (infodata.id_DiskState) {
				case ID_VALIDATING:
					strcpy(buf,string_table[STR_VALIDATING]);
					break;
				case ID_WRITE_PROTECTED:
					strcpy(buf,string_table[STR_WRITE_PROTECTED]);
					break;
				default:
					strcpy(buf,string_table[STR_READ_WRITE]);
					break;
			}

			UScoreText(rp,buf,380+textxoff+((160-(strlen(buf)*8))/2),53+textyoff,-1);
			break;

		case WBPROJECT:
		case WBTOOL:
			Move(rp,102+textxoff,40+textyoff);
			if (gotid) {
				lsprintf(buf,"%ld",(long int)fileinfo.fib_NumBlocks);
				Text(rp,buf,strlen(buf));
			}
			else Text(rp,"---",3);
			Move(rp,102+textxoff,50+textyoff);
			if (gotid) {
				lsprintf(buf,"%ld",(long int)fileinfo.fib_Size);
				Text(rp,buf,strlen(buf));
			}
			else Text(rp,"---",3);

		case WBDRAWER:
			if ((ttarray=oldtooltypes)) {
				for (ttcount=0;;ttcount++) if (!ttarray[ttcount]) break;
				if (ttarray=AllocMem((ttcount+1)*4,MEMF_CLEAR)) {
					for (a=0;a<ttcount;a++) {
						if (oldtooltypes[a] &&
							(ttarray[a]=AllocMem(strlen(oldtooltypes[a])+1,0)))
							strcpy(ttarray[a],oldtooltypes[a]);
					}
				}
			}
			else ttcount=0;
			if (tooltypelist=(struct DOpusListView *)
				AddRequesterObject(&icon_req,iconinfo_tooltype_list)) {

				fix_listview(&icon_req,tooltypelist);
				tooltypelist->items=ttarray;
				if (AddListView(tooltypelist,1)) {
					if (tooltypegads=addreqgadgets(&icon_req,iconinfo_tooltype_gadgets))
						tooltype_buf=((struct StringInfo *)tooltypegads->SpecialInfo)->Buffer;
				}
				else tooltypelist=NULL;
			}
			break;
	}

	RefreshRequesterObject(&icon_req,NULL);
	RefreshGList(window->FirstGadget,window,NULL,-1);
	if (tooltypegads) offttgads(window,tooltypegads);

	FOREVER {
		while (msg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
			if (!tooltypelist ||
				(view=ListViewIDCMP(tooltypelist,msg))==(struct DOpusListView *)-1) {

				class=msg->Class;
				code=msg->Code;
				x=window->MouseX;
				y=window->MouseY;

				if (class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN)
					gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
				ReplyMsg((struct Message *)msg);

				switch (class) {
					case IDCMP_CLOSEWINDOW:
						ret=0;
						goto endreq;

					case IDCMP_MOUSEBUTTONS:
						if (code==SELECTDOWN) {
							if (x>=icon_rec.MinX && x<=icon_rec.MaxX &&
								y>=icon_rec.MinY && y<=icon_rec.MaxY) {
hiliteimage:
								compflag = 1 - compflag;
								drawicon(window,dobj,compflag,&icon_rec);
							}
						}
						break;

					case IDCMP_RAWKEY:
						if (protectbitgads) {
							gad=protectbitgads;
							for (a=0;a<6;a++) {
								if (!gad) break;
								if (code==protect_keys[a]) {
									gad->Flags^=SELECTED;
									RefreshGList(gad,window,NULL,1);
									break;
								}
								gad=gad->NextGadget;
							}
						}
						switch (code) {
							case 0x45:
							case 0x33:
								ret=-1;
								goto endreq;
							case 0x27:
								ret=0;
								goto endreq;
							case 0x40:
								goto hiliteimage;
						}
						break;

					case IDCMP_MENUPICK:
						switch (ITEMNUM(code)) {
							case 0:
								ret=-1;
								goto endreq;
							case 1:
								ret=0;
								goto endreq;
							case 2:
								goto saveicon;
						}
						break;

					case IDCMP_GADGETUP:
						switch (gadgetid) {

							case II_DELETE:
deletettype:
								if (ttcount>0 && curtt>-1 && curtt<ttcount && tooltypelist) {
									if (ttarray) {
										if (ttcount>1 && !(tttemp=AllocMem(ttcount*4,MEMF_CLEAR)))
											break;
										if (ttarray[curtt]) FreeMem(ttarray[curtt],strlen(ttarray[curtt])+1);
										if (ttcount==1) {
											FreeMem(ttarray,8);
											ttarray=NULL;
											goto deletedtype;
										}
										if (curtt>0) CopyMem((char *)ttarray,(char *)tttemp,curtt*4);
										if (curtt<(ttcount-1))
											CopyMem((char *)&ttarray[curtt+1],(char *)&tttemp[curtt],
												(ttcount-1-curtt)*4);
										FreeMem(ttarray,(ttcount+1)*4); ttarray=tttemp;
									}
deletedtype:
									tooltypelist->items=ttarray;
									--ttcount;
									curtt=-1;
									RefreshListView(tooltypelist,1);
									tooltype_buf[0]=0;
									RefreshGList(tooltypegads,window,NULL,1);
									offttgads(window,tooltypegads);
								}
								break;
							case II_NEW:
								if (!(tooltypegads->Flags&GADGDISABLED)) {
									tooltype_buf[0]=0;
									RefreshStrGad(tooltypegads,window);
								}
								else if (tooltypelist) {
									if (!(tttemp=AllocMem((ttcount+2)*4,MEMF_CLEAR))) break;
									if (ttarray) {
										CopyMem((char *)ttarray,(char *)tttemp,ttcount*4);
										FreeMem(ttarray,(ttcount+1)*4);
									}
									ttarray=tttemp;
									curtt=ttcount++;
									ttarray[curtt]=AllocMem(1,MEMF_CLEAR);
									tooltypelist->items=ttarray;
									RefreshListView(tooltypelist,1);
									onttgads(window,tooltypegads);
									tooltype_buf[0]=0;
								}
								ActivateStrGad(tooltypegads,window);
								break;

							case II_TOOLTYPE:
								if (!tooltype_buf[0]) goto deletettype;
								if (curtt<ttcount && ttarray) {
									if (ttarray[curtt]) FreeMem(ttarray[curtt],strlen(ttarray[curtt])+1);
									if ((ttarray[curtt]=AllocMem(strlen(tooltype_buf)+1,0)))
										strcpy(ttarray[curtt],tooltype_buf);
								}
								curtt=-1;
								tooltype_buf[0]=0;
								RefreshGList(tooltypegads,window,NULL,1);
								offttgads(window,tooltypegads);
								if (tooltypelist) RefreshListView(tooltypelist,1);
								break;

							case II_SAVE:
saveicon:
								strcpy(namebuf,name);
								if ((ptr=strstr(namebuf,".info"))) *ptr=0;

								if ((dobj->do_Type==WBTOOL || dobj->do_Type==WBPROJECT) &&
									stackgad) {
									dobj->do_StackSize=atoi(stack_buf);
									if (dobj->do_StackSize<4096) dobj->do_StackSize=4096;
								}

								if (defaulttoolgad) {
									olddeftool=dobj->do_DefaultTool;
									if (dobj->do_DefaultTool=
										LAllocRemember(&icon_req.rb_memory,strlen(defaulttool_buf)+1,0)) {
										//newdef=1;
										strcpy(dobj->do_DefaultTool,defaulttool_buf);
									}
									//else newdef=0;
								}
								if (commentgad) {
									comment_buf[79]=0;
									SetComment(namebuf,comment_buf);
								}

								dobj->do_ToolTypes=(STRPTR *)ttarray;
								PutDiskObject(namebuf,dobj);

								if (defaulttoolgad) dobj->do_DefaultTool=olddeftool;

								dobj->do_ToolTypes=(STRPTR *)oldtooltypes;
								if (protectbitgads) {
									b=15;
									gad=protectbitgads;
									for (a=0;a<6;a++) {
										if (!gad) break;
										if (gad->Flags&SELECTED) {
											if (protect_flags[a]<16) b&=~protect_flags[a];
											else b|=protect_flags[a];
										}
										gad=gad->NextGadget;
									}
									SetProtection(namebuf,b);
								}
								ret=1;
								goto endreq;

							case II_SKIP:
								ret=0;
								goto endreq;

							case II_CANCEL:
								ret=-1;
								goto endreq;
						}
						break;
				}
			}
			else if (view && view->listid==0) {
				a=view->itemselected;
				curtt=-1;
				if (a<ttcount && ttarray && ttarray[a] && tooltypegads) {
					curtt=a;
					strcpy(tooltype_buf,ttarray[a]);
					onttgads(window,tooltypegads);
					ActivateStrGad(tooltypegads,window);
				}
			}
		}
		Wait(1<<window->UserPort->mp_SigBit);
	}
endreq:
	if (ttarray) {
		for (a=0;a<ttcount;a++) {
			if (ttarray[a]) FreeMem(ttarray[a],strlen(ttarray[a])+1);
		}
		FreeMem(ttarray,(ttcount+1)*4);
	}
	if (tooltypelist) RemoveListView(tooltypelist,1);
	if (small) ClearMenuStrip(window);
	CloseRequester(&icon_req);
	FreeDiskObject(dobj);
	return(ret);
}

void drawicon(window,dobj,selected,rect)
struct Window *window;
struct DiskObject *dobj;
BOOL selected;
struct Rectangle *rect;
{
	struct Region *reg,*oldreg;
	int x,y,w,h;

	struct Rectangle size = {0};

	if
	(
		GetIconRectangle
		(
			window->RPort, dobj, NULL, &size, 
			ICONDRAWA_Borderless, TRUE, 
			TAG_DONE 
		)
	)
	{
		w = size.MaxX - size.MinX + 1;
		h = size.MaxY - size.MinY + 1;

	}

	x = ((rect->MaxX - rect->MinX + 1) - w)/2;
	y = ((rect->MaxY - rect->MinY + 1) - h)/2;
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	if (reg=NewRegion()) {
		OrRectRegion(reg,rect);
		oldreg=InstallClipRegion(window->WLayer,reg);

		LayoutIcon(dobj, window->WScreen, TAG_END);

		DrawIconState
		(
			window->RPort, dobj, NULL, 
			x+rect->MinX, y+rect->MinY, selected ? IDS_SELECTED : IDS_NORMAL, 

			ICONDRAWA_Frameless,       TRUE,
			ICONDRAWA_Borderless,      TRUE,
			ICONDRAWA_EraseBackground, TRUE,
			TAG_DONE
		);
		InstallClipRegion(window->WLayer,oldreg);
		DisposeRegion(reg);
	}
}

void offttgads(window,gads)
struct Window *window;
struct Gadget *gads;
{
	int a;

	for (a=0;a<2;a++) {
		DisableGadget(gads,window->RPort,2,1);
		gads=gads->NextGadget;
	}
}

void onttgads(window,gads)
struct Window *window;
struct Gadget *gads;
{
	int a;

	for (a=0;a<2;a++) {
		EnableGadget(gads,window->RPort,2,1);
		gads=gads->NextGadget;
	}
}

int doroot(str)
char *str;
{
	int i,b,f=0,c;

	c=strlen(str);
	if (c==0) return(0);
	i=c;
	for (b=0;b<i;b++) if (str[b]==':') { ++b; f=1; break; }
	if (f==1 && b==i) return(0);
	else if (f>0) str[b]=0;
	return(1);
}

BPTR getrootlock(lock1)
BPTR lock1;
{
    BPTR lock2;

    while ((lock2=ParentDir(lock1))) {
        UnLock(lock1);
        lock1=lock2;
    }
    return(lock1);
}

int getroot(name,ds)
char *name;
struct DateStamp *ds;
{
#ifdef __AROS__
        struct DosList *dl;
        BPTR           lock;
        BOOL           success;
        char           buf[MAXFILENAMELENGTH];

        //kprintf("!!!! getroot(%s)\n", name);

        lock = Lock(name, ACCESS_READ);
        if (NULL == lock)
        {
            //kprintf("Could not get lock in getroot()\n");
            return 0;
        }

        /* Get lock on root device */

        SetIoErr(0);
        lock = getrootlock(lock);

        /* Get the name from lock */
        success = NameFromLock(lock, buf, MAXFILENAMELENGTH);

        UnLock(lock);

        if (!success)
        {
            //kprintf("Could not get lock for name\n");
            return 0;
        }

        dl = LockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_READ);
        dl = FindDosEntry(dl, buf, LDF_DEVICES |  LDF_VOLUMES);

        if (NULL == dl) {

            UnLockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_READ);
            kprintf("Could not get device list\n");
            return 0;
        }

        /* Get the volume name */

        BtoCStr(dl->dol_Name, name, 32);

        /* Get the volume date */
        if (ds)
        {
            CopyMem(&dl->dol_misc.dol_volume.dol_VolumeDate, ds, sizeof (*ds));
        }

        UnLockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_READ);

        return 1;

#else
	BPTR lock1;
	struct FileLock *lock2;
	char *p;
	struct DeviceList *dl;
	int a;

	if (!(lock1=Lock(name,ACCESS_READ))) return(0);
	lock2=(struct FileLock *) BADDR(lock1);
	for (a=0;a<32;a++) name[a]=0;
	dl=(struct DeviceList *)BADDR(lock2->fl_Volume);
	p=(char *) BADDR(dl->dl_Name);
	if (p) LStrnCpy(name,p+1,*p);
	if (ds) CopyMem((char *)&dl->dl_VolumeDate,(char *)ds,sizeof(struct DateStamp));
	UnLock(lock1);
	return(1);
#endif
}

