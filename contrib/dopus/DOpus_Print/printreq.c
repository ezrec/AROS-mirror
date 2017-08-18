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

#include "print.h"

enum {
	PRINT_FILEREQUESTER,
	PRINT_FILE,

	PRINT_TOPMARGIN,
	PRINT_BOTTOMMARGIN,
	PRINT_LEFTMARGIN,
	PRINT_RIGHTMARGIN,
	PRINT_TABSIZE,
	PRINT_PITCH,
	PRINT_QUALITY,
	PRINT_EJECTPAGE,

	PRINT_HEADERFOOTER,
	HEADFOOT_TITLE,
	HEADFOOT_DATE,
	HEADFOOT_PAGENUMBER,
	HEADFOOT_TITLESTRING,
	HEADFOOT_TEXTSTYLE,

	OUTPUT_PRINTER,
	OUTPUT_FILE,
	OUTPUT_FILEREQUESTER,
	OUTPUT_FILESTRING,

	PRINT_PRINT,
	PRINT_EXIT,

	PRINT_NUM_GADGETS};

struct TagItem
	print_filerequester_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_TINYGLASS},
		{RO_GadgetID,PRINT_FILEREQUESTER},
		{RO_TopFine,-5},
		{RO_Left,5},
		{RO_WidthFine,24},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_FILE},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	print_file_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,PRINT_FILE},
		{RO_Left,5},
		{RO_LeftFine,30},
		{RO_TopFine,-3},
		{RO_Width,35},
		{RO_WidthFine,-14},
		{RO_Height,1},
		{RO_StringLen,256},
		{TAG_END,0}},

	print_topmargin_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_INTEGER},
		{RO_GadgetID,PRINT_TOPMARGIN},
		{RO_Left,14},
		{RO_LeftFine,1},
		{RO_Top,1},
		{RO_TopFine,4},
		{RO_Width,3},
		{RO_Height,1},
		{RO_TextNum,STR_TOP_MARGIN},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_StringLen,4},
		{TAG_END,0}},
	print_bottommargin_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_INTEGER},
		{RO_GadgetID,PRINT_BOTTOMMARGIN},
		{RO_Left,14},
		{RO_LeftFine,1},
		{RO_Top,2},
		{RO_TopFine,11},
		{RO_Width,3},
		{RO_Height,1},
		{RO_TextNum,STR_BOTTOM_MARGIN},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_StringLen,4},
		{TAG_END,0}},
	print_leftmargin_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_INTEGER},
		{RO_GadgetID,PRINT_LEFTMARGIN},
		{RO_Left,14},
		{RO_LeftFine,1},
		{RO_Top,3},
		{RO_TopFine,18},
		{RO_Width,3},
		{RO_Height,1},
		{RO_TextNum,STR_LEFT_MARGIN},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_StringLen,4},
		{TAG_END,0}},
	print_rightmargin_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_INTEGER},
		{RO_GadgetID,PRINT_RIGHTMARGIN},
		{RO_Left,14},
		{RO_LeftFine,1},
		{RO_Top,4},
		{RO_TopFine,25},
		{RO_Width,3},
		{RO_Height,1},
		{RO_TextNum,STR_RIGHT_MARGIN},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_StringLen,4},
		{TAG_END,0}},

	print_tabsize_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_INTEGER},
		{RO_GadgetID,PRINT_TABSIZE},
		{RO_Left,32},
		{RO_Top,1},
		{RO_TopFine,4},
		{RO_Width,3},
		{RO_Height,1},
		{RO_TextNum,STR_TAB_SIZE},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_StringLen,3},
		{TAG_END,0}},
	print_pitch_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CYCLE},
		{RO_GadgetID,PRINT_PITCH},
		{RO_Top,2},
		{RO_TopFine,8},
		{RO_Left,32},
		{RO_LeftFine,-4},
		{RO_Width,8},
		{RO_WidthFine,24},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_TextNum,STR_PRINT_PITCH},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	print_quality_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CYCLE},
		{RO_GadgetID,PRINT_QUALITY},
		{RO_Top,3},
		{RO_TopFine,15},
		{RO_Left,32},
		{RO_LeftFine,-4},
		{RO_Width,8},
		{RO_WidthFine,24},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_TextNum,STR_QUALITY},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	print_ejectpage_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,PRINT_EJECTPAGE},
		{RO_Top,4},
		{RO_TopFine,24},
		{RO_Left,20},
		{RO_TextNum,STR_EJECT},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_BoolOn,TRUE},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},

	print_headerfooter_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CYCLE},
		{RO_GadgetID,PRINT_HEADERFOOTER},
		{RO_Top,5},
		{RO_TopFine,30},
		{RO_Width,8},
		{RO_WidthFine,24},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_TextNum,STR_CONFIGURATION},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	headfoot_title_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,HEADFOOT_TITLE},
		{RO_Top,6},
		{RO_TopFine,41},
		{RO_LeftFine,20},
		{RO_TextNum,STR_TITLE},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},
	headfoot_date_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,HEADFOOT_DATE},
		{RO_Top,7},
		{RO_TopFine,46},
		{RO_LeftFine,20},
		{RO_TextNum,STR_DATE},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},
	headfoot_pagenumber_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,HEADFOOT_PAGENUMBER},
		{RO_Top,8},
		{RO_TopFine,51},
		{RO_LeftFine,20},
		{RO_TextNum,STR_PAGE_NUMBER},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},
	headfoot_titlestring_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,HEADFOOT_TITLESTRING},
		{RO_Left,8},
		{RO_LeftFine,49},
		{RO_Top,6},
		{RO_TopFine,41},
		{RO_Width,32},
		{RO_WidthFine,-33},
		{RO_Height,1},
		{RO_StringLen,40},
		{TAG_END,0}},
	headfoot_textstyle_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CYCLE},
		{RO_GadgetID,HEADFOOT_TEXTSTYLE},
		{RO_Left,31},
		{RO_LeftFine,-4},
		{RO_Top,8},
		{RO_TopFine,50},
		{RO_Width,9},
		{RO_WidthFine,24},
		{RO_Height,1},
		{RO_HeightFine,6},
		{RO_TextNum,STR_TEXT_STYLE},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	output_printer_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_RADIO},
		{RO_GadgetID,OUTPUT_PRINTER},
		{RO_Top,10},
		{RO_TopFine,61},
		{RO_LeftFine,22},
		{RO_TextNum,STR_PRINTER},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},
	output_file_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_RADIO},
		{RO_GadgetID,OUTPUT_FILE},
		{RO_Top,11},
		{RO_TopFine,65},
		{RO_LeftFine,22},
		{RO_TextNum,STR_FILE},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},
	output_filerequester_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_TINYGLASS},
		{RO_GadgetID,OUTPUT_FILEREQUESTER},
		{RO_Left,8},
		{RO_LeftFine,45},
		{RO_Top,11},
		{RO_TopFine,62},
		{RO_WidthFine,24},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	output_filestring_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,OUTPUT_FILESTRING},
		{RO_Left,8},
		{RO_LeftFine,75},
		{RO_Top,11},
		{RO_TopFine,64},
		{RO_Width,32},
		{RO_WidthFine,-59},
		{RO_Height,1},
		{RO_StringLen,256},
		{TAG_END,0}},

	print_print_gadget[]={		
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PRINT_PRINT},
		{RO_Top,12},
		{RO_TopFine,70},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_PRINT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	print_exit_gadget[]={		
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PRINT_EXIT},
		{RO_Left,28},
		{RO_LeftFine,20},
		{RO_Top,12},
		{RO_TopFine,70},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_EXIT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	print_output_text[]={
		{RO_Type,OBJECT_TEXT},
		{RO_Top,9},
		{RO_TopFine,57},
		{RO_LeftFine,-1},
		{RO_TextNum,STR_OUTPUT_TO},
		{TAG_END,0}},

	*print_gadgets[]={
		print_filerequester_gadget,
		print_file_gadget,
		print_topmargin_gadget,
		print_bottommargin_gadget,
		print_leftmargin_gadget,
		print_rightmargin_gadget,
		print_tabsize_gadget,
		print_pitch_gadget,
		print_quality_gadget,
		print_ejectpage_gadget,
		print_headerfooter_gadget,
		headfoot_title_gadget,
		headfoot_date_gadget,
		headfoot_pagenumber_gadget,
		headfoot_titlestring_gadget,
		headfoot_textstyle_gadget,
		output_printer_gadget,
		output_file_gadget,
		output_filerequester_gadget,
		output_filestring_gadget,
		print_print_gadget,
		print_exit_gadget,
		NULL};

char
	*text_pitch[3],
	*text_styles[6],
	*text_quality[2],
	*header_footer_text[2];

void do_print(vis,portname,argslist,argc,argv)
struct VisInfo *vis;
char *portname;
struct DOpusArgsList *argslist;
int argc;
char *argv[];
{
	struct RequesterBase printreq;
	struct IntuiMessage *msg;
	struct Window *window;
	struct Gadget **gadgets,*refgad;
	struct AppWindow *appwindow=NULL;
	struct MsgPort *appport=NULL;
	struct AppMessage *appmsg;
	ULONG class;
	UWORD code,gadgetid,qual;
	APTR iaddress;
	struct StringExtend stringex;
	PrintData *printdata;
	int a,headerfooter=0,waitbits;
	struct Requester busyreq;
	char *print_file_buffer,
		*print_topmargin_buffer,
		*print_bottommargin_buffer,
		*print_leftmargin_buffer,
		*print_rightmargin_buffer,
		*print_tabsize_buffer,
		*output_filestring_buffer;

	for (a=0;a<3;a++) text_pitch[a]=string_table[STR_PICA+a];
	for (a=0;a<6;a++) text_styles[a]=string_table[STR_NORMAL+a];
	for (a=0;a<2;a++) {
		text_quality[a]=string_table[STR_DRAFT+a];
		header_footer_text[a]=string_table[STR_HEADER+a];
	}

	printreq.rb_width=40;
	printreq.rb_height=13;
	printreq.rb_flags=0;

	fill_out_req(&printreq,vis);

	printreq.rb_privateflags=0;
	printreq.rb_screenname=NULL;

	if (printreq.rb_screen && !(vis->vi_flags&VISF_BORDERS)) {
		printreq.rb_flags|=RBF_STRINGS;
		printreq.rb_title=NULL;
		printreq.rb_widthfine=45;
		printreq.rb_heightfine=86;
		printreq.rb_leftoffset=13;
		printreq.rb_topoffset=9;
	}
	else {
		printreq.rb_flags|=RBF_BORDERS|RBF_CLOSEGAD|RBF_STRINGS;
		printreq.rb_title=string_table[STR_PRINT];
		printreq.rb_widthfine=41;
		printreq.rb_heightfine=83;
		printreq.rb_leftoffset=11;
		printreq.rb_topoffset=7;
	}

	printreq.rb_extend=&stringex;
	printreq.rb_idcmpflags=0;
	printreq.rb_string_table=string_table;

	for (a=0;a<2;a++) {
		stringex.Pens[a]=vis->vi_stringcol[a];
		stringex.ActivePens[a]=vis->vi_activestringcol[a];
	}
	stringex.InitialModes=0;
	stringex.EditHook=NULL;
	stringex.WorkBuffer=NULL;
	for (a=0;a<4;a++) stringex.Reserved[a]=0;

	if (!(printdata=AllocMem(sizeof(PrintData),MEMF_CLEAR))) return;

	get_print_default(printdata);
	get_print_env(printdata);

	for (a=0;a<2;a++) {
		if (window=OpenRequester(&printreq)) break;
		CloseRequester(&printreq);

		switch (a) {
			case 0:
				printreq.rb_flags&=~RBF_BORDERS;
				printreq.rb_title=NULL;
				break;

			default:
				FreeMem(printdata,sizeof(PrintData));
				return;
		}
	}

	if (!(gadgets=LAllocRemember(&printreq.rb_memory,sizeof(struct Gadget *)*PRINT_NUM_GADGETS,0)) ||
		!(addreqgadgets(&printreq,print_gadgets,gadgets)) ||
		!(AddRequesterObject(&printreq,print_output_text))) {
		CloseRequester(&printreq);
		FreeMem(printdata,sizeof(PrintData));
		return;
	}

	RefreshRequesterObject(&printreq,NULL);

	print_file_buffer=
		((struct StringInfo *)gadgets[PRINT_FILE]->SpecialInfo)->Buffer;
	print_topmargin_buffer=
		((struct StringInfo *)gadgets[PRINT_TOPMARGIN]->SpecialInfo)->Buffer;
	print_bottommargin_buffer=
		((struct StringInfo *)gadgets[PRINT_BOTTOMMARGIN]->SpecialInfo)->Buffer;
	print_leftmargin_buffer=
		((struct StringInfo *)gadgets[PRINT_LEFTMARGIN]->SpecialInfo)->Buffer;
	print_rightmargin_buffer=
		((struct StringInfo *)gadgets[PRINT_RIGHTMARGIN]->SpecialInfo)->Buffer;
	print_tabsize_buffer=
		((struct StringInfo *)gadgets[PRINT_TABSIZE]->SpecialInfo)->Buffer;
	output_filestring_buffer=
		((struct StringInfo *)gadgets[OUTPUT_FILESTRING]->SpecialInfo)->Buffer;

	if (portname) {
		argslist->file_data=print_file_buffer;
		dopus_message(DOPUSMSG_GETNEXTFILE,(APTR)argslist,portname);
	}
	else if (argc>0) strcpy(print_file_buffer,argv[0]);
	else print_file_buffer[0]=0;

	if (printdata->top_margin<1) printdata->top_margin=1;
	lsprintf(print_topmargin_buffer,"%ld",(long int)printdata->top_margin);
	lsprintf(print_bottommargin_buffer,"%ld",(long int)printdata->bottom_margin);
	if (printdata->left_margin<1) printdata->left_margin=1;
	lsprintf(print_leftmargin_buffer,"%ld",(long int)printdata->left_margin);
	lsprintf(print_rightmargin_buffer,"%ld",(long int)printdata->right_margin);
	lsprintf(print_tabsize_buffer,"%ld",(long int)printdata->tab_size);

	strcpy(output_filestring_buffer,printdata->output_file);

	DoCycleGadget(gadgets[PRINT_PITCH],window,text_pitch,printdata->print_pitch);
	DoCycleGadget(gadgets[PRINT_QUALITY],window,text_quality,printdata->text_quality);

	if (printdata->print_flags&PRINTFLAG_EJECT)
		gadgets[PRINT_EJECTPAGE]->Flags|=GFLG_SELECTED;
	else gadgets[PRINT_EJECTPAGE]->Flags&=~GFLG_SELECTED;

	if (printdata->print_flags&PRINTFLAG_FILE) {
		gadgets[OUTPUT_PRINTER]->Flags&=~GFLG_SELECTED;
		gadgets[OUTPUT_FILE]->Flags|=GFLG_SELECTED;
	}
	else {
		gadgets[OUTPUT_PRINTER]->Flags|=GFLG_SELECTED;
		gadgets[OUTPUT_FILE]->Flags&=~GFLG_SELECTED;
	}

	show_headerfooter_gadgets(gadgets,window,printdata,headerfooter);

	if (portname) {
		OffGadget(gadgets[PRINT_FILEREQUESTER],window,NULL);
		OffGadget(gadgets[PRINT_FILE],window,NULL);
	}

	if (!portname && WorkbenchBase &&
		(appport=LCreatePort(NULL,0)))
		appwindow=AddAppWindowA(0,0,window,appport,NULL);

	FOREVER {
		if (appwindow) {
			while (appmsg=(struct AppMessage *)GetMsg(appport)) {
				if (appmsg->am_Type==AMTYPE_APPWINDOW) {
					if (appmsg->am_NumArgs>0 && appmsg->am_ArgList[0].wa_Lock) {
						char buf[256];

						PathName(appmsg->am_ArgList[0].wa_Lock,buf,256);
						TackOn(buf,appmsg->am_ArgList[0].wa_Name,256);
						if (CheckExist(buf,NULL)<0) {
							strcpy(print_file_buffer,buf);
							RefreshStrGad(gadgets[PRINT_FILE],window);
						}
						else DisplayBeep(NULL);
					}
					else DisplayBeep(NULL);
				}
				ReplyMsg((struct Message *)appmsg);
			}
		}
		while (msg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
			class=msg->Class; code=msg->Code; qual=msg->Qualifier;
			iaddress=msg->IAddress;
			ReplyMsg((struct Message *)msg);

			switch (class) {
				case IDCMP_GADGETDOWN:
					gadgetid=((struct Gadget *)iaddress)->GadgetID;

					switch (gadgetid) {
						case OUTPUT_PRINTER:
							if (!(gadgets[OUTPUT_PRINTER]->Flags&GFLG_SELECTED)) {
								gadgets[OUTPUT_PRINTER]->Flags|=GFLG_SELECTED;
								RefreshGList(gadgets[OUTPUT_PRINTER],window,NULL,1);
							}
							gadgets[OUTPUT_FILE]->Flags&=~GFLG_SELECTED;
							RefreshGList(gadgets[OUTPUT_FILE],window,NULL,1);
							printdata->print_flags&=~PRINTFLAG_FILE;
							break;

						case OUTPUT_FILE:
							if (!(gadgets[OUTPUT_FILE]->Flags&GFLG_SELECTED)) {
								gadgets[OUTPUT_FILE]->Flags|=GFLG_SELECTED;
								RefreshGList(gadgets[OUTPUT_FILE],window,NULL,1);
							}
							gadgets[OUTPUT_PRINTER]->Flags&=~GFLG_SELECTED;
							RefreshGList(gadgets[OUTPUT_PRINTER],window,NULL,1);
							printdata->print_flags|=PRINTFLAG_FILE;
							break;
					}
					break;

				case IDCMP_GADGETUP:
					gadgetid=((struct Gadget *)iaddress)->GadgetID;
					refgad=NULL;

					switch (gadgetid) {
						case PRINT_TOPMARGIN:
							if (check_print_gadget(window,gadgets[PRINT_TOPMARGIN],
								1,atoi(print_bottommargin_buffer)-5,code))
								refgad=(struct Gadget *)iaddress;
							printdata->top_margin=atoi(print_topmargin_buffer);
							break;
						case PRINT_BOTTOMMARGIN:
							if (check_print_gadget(window,gadgets[PRINT_BOTTOMMARGIN],
								atoi(print_topmargin_buffer)+5,999,code))
								refgad=(struct Gadget *)iaddress;
							printdata->bottom_margin=atoi(print_bottommargin_buffer);
							break;
						case PRINT_LEFTMARGIN:
							if (check_print_gadget(window,gadgets[PRINT_LEFTMARGIN],
								1,atoi(print_rightmargin_buffer)-1,code))
								refgad=(struct Gadget *)iaddress;
							printdata->left_margin=atoi(print_leftmargin_buffer);
							break;
						case PRINT_RIGHTMARGIN:
							if (check_print_gadget(window,gadgets[PRINT_RIGHTMARGIN],
								atoi(print_leftmargin_buffer)+1,999,code))
								refgad=(struct Gadget *)iaddress;
							printdata->right_margin=atoi(print_rightmargin_buffer);
							break;

						case PRINT_TABSIZE:
							if (check_print_gadget(window,gadgets[PRINT_TABSIZE],
								1,10,code))
								ActivateStrGad(gadgets[PRINT_TOPMARGIN],window);
							printdata->tab_size=atoi(print_tabsize_buffer);
							break;

						case PRINT_PITCH:
							if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) {
								if ((printdata->print_pitch--)==0)
									printdata->print_pitch=PITCH_COUNT-1;
							}
							else {
								if ((++printdata->print_pitch)==PITCH_COUNT)
									printdata->print_pitch=0;
							}
							DoCycleGadget(gadgets[PRINT_PITCH],window,
								text_pitch,printdata->print_pitch);
							break;

						case PRINT_QUALITY:
							printdata->text_quality=1-printdata->text_quality;
							DoCycleGadget(gadgets[PRINT_QUALITY],window,
								text_quality,printdata->text_quality);
							break;

						case PRINT_HEADERFOOTER:
							read_headerfooter_gadgets(gadgets,window,printdata,headerfooter);
							headerfooter=1-headerfooter;
							show_headerfooter_gadgets(gadgets,window,printdata,headerfooter);
							break;

						case HEADFOOT_TEXTSTYLE:
							if (qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) {
								if ((printdata->headfoot[headerfooter].text_style--)==0)
									printdata->headfoot[headerfooter].text_style=STYLE_COUNT-1;
							}
							else {
								if ((++printdata->headfoot[headerfooter].text_style)==STYLE_COUNT)
									printdata->headfoot[headerfooter].text_style=0;
							}
							DoCycleGadget(gadgets[HEADFOOT_TEXTSTYLE],window,
								text_styles,printdata->headfoot[headerfooter].text_style);
							break;

						case HEADFOOT_TITLESTRING:
							RefreshStrGad(gadgets[HEADFOOT_TITLESTRING],window);
							break;

						case OUTPUT_FILEREQUESTER:
							if (!(get_file_byrequest(gadgets[OUTPUT_FILESTRING],window,1)))
								break;
						case OUTPUT_FILESTRING:
							strcpy(printdata->output_file,output_filestring_buffer);
							RefreshStrGad(gadgets[OUTPUT_FILESTRING],window);
							if (gadgets[OUTPUT_PRINTER]->Flags&GFLG_SELECTED) {
								gadgets[OUTPUT_PRINTER]->Flags&=~GFLG_SELECTED;
								gadgets[OUTPUT_FILE]->Flags|=GFLG_SELECTED;
								RefreshGList(gadgets[OUTPUT_PRINTER],window,NULL,2);
								printdata->print_flags|=PRINTFLAG_FILE;
							}
							break;

						case PRINT_FILEREQUESTER:
							get_file_byrequest(gadgets[PRINT_FILE],window,0);
							break;

						case PRINT_EJECTPAGE:
							if (gadgets[PRINT_EJECTPAGE]->Flags&GFLG_SELECTED)
								printdata->print_flags|=PRINTFLAG_EJECT;
							else printdata->print_flags&=~PRINTFLAG_EJECT;
							break;

						case PRINT_PRINT:
							read_headerfooter_gadgets(gadgets,window,printdata,headerfooter);

							SetBusyPointer(window);
							if (appwindow) RemoveAppWindow(appwindow);

							while (print_file_buffer[0]) {
								InitRequester(&busyreq);
								busyreq.Width=window->RPort->Font->tf_XSize*22;
								busyreq.Height=window->RPort->Font->tf_YSize*7;
								busyreq.LeftEdge=(window->Width-busyreq.Width)/2;
								busyreq.TopEdge=(window->Height-busyreq.Height)/2;
								busyreq.Flags=NOISYREQ;
								busyreq.BackFill=printreq.rb_bg;

								if (!(printfile(&printreq,print_file_buffer,printdata,&busyreq)))
									break;

								if (portname) {
									dopus_message(DOPUSMSG_UNSELECTFILE,(APTR)argslist,portname);
									dopus_message(DOPUSMSG_GETNEXTFILE,(APTR)argslist,portname);
								}
								else break;
							}

							if (appwindow)
								appwindow=AddAppWindowA(0,0,window,appport,NULL);

							ClearPointer(window);
							if (portname) {
								gadgetid=PRINT_EXIT;
								code=0x1b;
							}
							break;

						case PRINT_EXIT:
							code=0x1b;
							break;
					}
					if (refgad) {
						RefreshStrGad(refgad,window);
						if (code!=0x9) activate_next_gadget(refgad,window);
					}
					if (gadgetid!=PRINT_EXIT) break;

				case IDCMP_VANILLAKEY:
					if (code!=0x1b) break;

				case IDCMP_CLOSEWINDOW:
					if (appwindow) RemoveAppWindow(appwindow);
					if (appport) LDeletePort(appport);
					read_headerfooter_gadgets(gadgets,window,printdata,headerfooter);
					CloseRequester(&printreq);
					set_print_env(printdata);
					FreeMem(printdata,sizeof(PrintData));
					return;
			}
		}
		waitbits=1<<window->UserPort->mp_SigBit;
		if (appwindow) waitbits|=1<<appport->mp_SigBit;
		Wait(waitbits);
	}
}

void get_print_default(printdata)
PrintData *printdata;
{
	struct Preferences prefs;

	GetPrefs(&prefs,sizeof(struct Preferences));

	printdata->top_margin=1;
	printdata->bottom_margin=prefs.PaperLength;
	printdata->left_margin=prefs.PrintLeftMargin;
	printdata->right_margin=prefs.PrintRightMargin;

	printdata->tab_size=8;
	switch (prefs.PrintPitch) {
		case ELITE:
			printdata->print_pitch=PITCH_ELITE;
			break;
		case FINE:
			printdata->print_pitch=PITCH_FINE;
			break;
	}

	printdata->print_flags=PRINTFLAG_EJECT;
}

void show_headerfooter_gadgets(gadgets,window,printdata,headerfooter)
struct Gadget **gadgets;
struct Window *window;
PrintData *printdata;
int headerfooter;
{
	DoCycleGadget(gadgets[PRINT_HEADERFOOTER],window,header_footer_text,
		headerfooter);
	DoCycleGadget(gadgets[HEADFOOT_TEXTSTYLE],window,text_styles,
		printdata->headfoot[headerfooter].text_style);
	strcpy(((struct StringInfo *)gadgets[HEADFOOT_TITLESTRING]->SpecialInfo)->Buffer,
		printdata->headfoot[headerfooter].headfoot_title);

	if (printdata->headfoot[headerfooter].headfoot_flags&HEADFOOTFLAG_TITLE)
		gadgets[HEADFOOT_TITLE]->Flags|=GFLG_SELECTED;
	else gadgets[HEADFOOT_TITLE]->Flags&=~GFLG_SELECTED;

	if (printdata->headfoot[headerfooter].headfoot_flags&HEADFOOTFLAG_DATE)
		gadgets[HEADFOOT_DATE]->Flags|=GFLG_SELECTED;
	else gadgets[HEADFOOT_DATE]->Flags&=~GFLG_SELECTED;

	if (printdata->headfoot[headerfooter].headfoot_flags&HEADFOOTFLAG_PAGE)
		gadgets[HEADFOOT_PAGENUMBER]->Flags|=GFLG_SELECTED;
	else gadgets[HEADFOOT_PAGENUMBER]->Flags&=~GFLG_SELECTED;

	RefreshGList(gadgets[0],window,NULL,-1);
}

void read_headerfooter_gadgets(gadgets,window,printdata,headerfooter)
struct Gadget **gadgets;
struct Window *window;
PrintData *printdata;
int headerfooter;
{
	strcpy(printdata->headfoot[headerfooter].headfoot_title,
		((struct StringInfo *)gadgets[HEADFOOT_TITLESTRING]->SpecialInfo)->Buffer);

	if (gadgets[HEADFOOT_TITLE]->Flags&GFLG_SELECTED)
		printdata->headfoot[headerfooter].headfoot_flags|=HEADFOOTFLAG_TITLE;
	else printdata->headfoot[headerfooter].headfoot_flags&=~HEADFOOTFLAG_TITLE;

	if (gadgets[HEADFOOT_DATE]->Flags&GFLG_SELECTED)
		printdata->headfoot[headerfooter].headfoot_flags|=HEADFOOTFLAG_DATE;
	else printdata->headfoot[headerfooter].headfoot_flags&=~HEADFOOTFLAG_DATE;

	if (gadgets[HEADFOOT_PAGENUMBER]->Flags&GFLG_SELECTED)
		printdata->headfoot[headerfooter].headfoot_flags|=HEADFOOTFLAG_PAGE;
	else printdata->headfoot[headerfooter].headfoot_flags&=~HEADFOOTFLAG_PAGE;
}

int check_print_gadget(window,gad,min,max,code)
struct Window *window;
struct Gadget *gad;
int min,max;
UWORD code;
{
	if (CheckNumGad(gad,window,min,max)) {
		DisplayBeep(NULL);
		if (code!=0x9) ActivateStrGad(gad,window);
		return(0);
	}
	return(1);
}
