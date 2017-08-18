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

#include "diskop.h"

#define FORMATFLAG_FFS             1
#define FORMATFLAG_INTERNATIONAL   2
#define FORMATFLAG_CACHING         4
#define FORMATFLAG_TRASHCAN        8
#define FORMATFLAG_VERIFY         16

enum {
	FORMAT_NAME,
	FORMAT_FFS,
	FORMAT_INTERNATIONAL,
	FORMAT_CACHING,
	FORMAT_TRASHCAN,
	FORMAT_VERIFY,
	FORMAT_FORMAT,
	FORMAT_QUICKFORMAT,
	FORMAT_CANCEL};

struct TagItem
	format_name_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_STRING},
		{RO_GadgetID,FORMAT_NAME},
		{RO_Left,19},
		{RO_Width,21},
		{RO_Height,1},
		{RO_TextNum,STR_FORMAT_NAME},
		{RO_TextPos,TEXTPOS_LEFT},
		{RO_StringLen,31},
		{TAG_END,0}},
	format_ffs_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,FORMAT_FFS},
		{RO_Left,14},
		{RO_Top,2},
		{RO_TextNum,STR_FORMAT_FFS},
		{RO_TextPos,TEXTPOS_RIGHT},
		{TAG_END,0}},
	format_international_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,FORMAT_INTERNATIONAL},
		{RO_Left,14},
		{RO_Top,3},
		{RO_TopFine,5},
		{RO_TextNum,STR_FORMAT_INTERNATIONAL},
		{RO_TextPos,TEXTPOS_RIGHT},
		{TAG_END,0}},
	format_caching_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,FORMAT_CACHING},
		{RO_Left,14},
		{RO_Top,4},
		{RO_TopFine,10},
		{RO_TextNum,STR_FORMAT_CACHING},
		{RO_TextPos,TEXTPOS_RIGHT},
		{TAG_END,0}},
	format_trashcan_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,FORMAT_TRASHCAN},
		{RO_Left,14},
		{RO_Top,5},
		{RO_TopFine,15},
		{RO_TextNum,STR_FORMAT_TRASHCAN},
		{RO_TextPos,TEXTPOS_RIGHT},
		{TAG_END,0}},
	format_verify_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,FORMAT_VERIFY},
		{RO_Left,14},
		{RO_Top,6},
		{RO_TopFine,20},
		{RO_TextNum,STR_FORMAT_VERIFY},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_BoolOn,TRUE},
		{TAG_END,0}},
	format_format_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,FORMAT_FORMAT},
		{RO_Top,8},
		{RO_TopFine,37},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_FORMAT_FORMAT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	format_quickformat_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,FORMAT_QUICKFORMAT},
		{RO_Left,14},
		{RO_Top,8},
		{RO_TopFine,37},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_FORMAT_QUICKFORMAT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	format_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,FORMAT_CANCEL},
		{RO_Left,28},
		{RO_Top,8},
		{RO_TopFine,37},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_FORMAT_EXIT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	format_device_list[]={
		{RO_Type,OBJECT_LISTVIEW},
		{RO_ListViewID,0},
		{RO_LeftFine,2},
		{RO_Width,10},
		{RO_Height,6},
		{RO_HeightFine,30},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	format_info_box[]={
		{RO_Type,OBJECT_BORDER},
		{RO_BorderType,BORDER_RECESSED},
		{RO_LeftFine,2},
		{RO_Top,7},
		{RO_TopFine,28},
		{RO_Width,41},
		{RO_WidthFine,-4},
		{RO_Height,1},
		{RO_HeightFine,4},
		{TAG_END,0}},
		
	*format_gadgets[]={
		format_name_gadget,
		format_ffs_gadget,
		format_international_gadget,
		format_caching_gadget,
		format_trashcan_gadget,
		format_verify_gadget,
		format_format_gadget,
		format_quickformat_gadget,
		format_cancel_gadget,
		NULL};

void diskop_format(vis,portname,argc,argv)
struct VisInfo *vis;
char *portname;
int argc;
char *argv[];
{
	struct RequesterBase formatreq;
	struct Window *window;
	struct IntuiMessage *msg;
	Object_Border *infobox;
	struct DOpusListView *devicelist,*view;
	struct Gadget *gadlist,*gad,*gad_international=NULL,*gad_caching=NULL;
	ULONG class,mask,flags;
	UWORD code,gadgetid;
	APTR iaddress;
	struct StringExtend stringex;
	char *diskname;
	int a,b,gadcount=0,ignorechange=0;
	int start=-1,quick=0,startflags=FORMATFLAG_TRASHCAN;

	formatreq.rb_width=41;
	formatreq.rb_height=9;
	formatreq.rb_widthfine=16;
	formatreq.rb_heightfine=54;
	formatreq.rb_leftoffset=8;
	formatreq.rb_topoffset=8;
	formatreq.rb_flags=0;

	fill_out_req(&formatreq,vis);

	formatreq.rb_privateflags=0;
	formatreq.rb_screenname=NULL;

	if (formatreq.rb_screen && !(vis->vi_flags&VISF_BORDERS)) {
		formatreq.rb_flags|=RBF_STRINGS;
		formatreq.rb_title=NULL;
	}
	else {
		formatreq.rb_flags|=RBF_BORDERS|RBF_CLOSEGAD|RBF_STRINGS;
		formatreq.rb_title=string_table[STR_FORMAT_FORMAT];
	}

	formatreq.rb_extend=&stringex;
	formatreq.rb_idcmpflags=0;
	formatreq.rb_string_table=string_table;

	for (a=0;a<2;a++) {
		stringex.Pens[a]=vis->vi_stringcol[a];
		stringex.ActivePens[a]=vis->vi_activestringcol[a];
	}
	stringex.InitialModes=0;
	stringex.EditHook=NULL;
	stringex.WorkBuffer=NULL;
	for (a=0;a<4;a++) stringex.Reserved[a]=0;

	if (SysBase->LibNode.lib_Version<39) mask=12;
	else mask=0;

	if (!(window=OpenRequester(&formatreq)) ||
		!(gadlist=addreqgadgets(&formatreq,format_gadgets,mask,&gadcount)) ||
		!(infobox=(Object_Border *)
			AddRequesterObject(&formatreq,format_info_box)) ||
		!(devicelist=(struct DOpusListView *)
			AddRequesterObject(&formatreq,format_device_list)) ||
		!(devicelist->items=get_device_list(&formatreq.rb_memory,NULL))) {
		CloseRequester(&formatreq);
		return;
	}

	fix_listview(&formatreq,devicelist);
	select_device(devicelist,NULL);

	diskname=((struct StringInfo *)gadlist->SpecialInfo)->Buffer;
	strcpy(diskname,string_table[STR_FORMAT_EMPTY]);

	get_env("format",gadlist,gadcount,devicelist);

	flags=0;
	gad=gadlist;
	for (a=0;a<gadcount;a++) {
		if (gad->Activation&GACT_TOGGLESELECT)
			if (gad->Flags&GFLG_SELECTED) flags|=1<<(gad->GadgetID-1);
		gad->Flags&=~GFLG_DISABLED;
		gad=gad->NextGadget;
	}

	if (SysBase->LibNode.lib_Version>38) {
		gad_international=gadlist->NextGadget->NextGadget;
		gad_caching=gad_international->NextGadget;
	}

	for (a=0;a<argc;a++) {
		if (LStrCmpI(argv[a],"quick")==0) quick=1;
		else if (LStrCmpI(argv[a],"ffs")==0) startflags|=FORMATFLAG_FFS;
		else if (LStrCmpI(argv[a],"cache")==0) startflags|=FORMATFLAG_CACHING;
		else if (LStrnCmpI(argv[a],"inter",5)==0) startflags|=FORMATFLAG_INTERNATIONAL;
		else if (LStrCmpI(argv[a],"noicons")==0) startflags&=~FORMATFLAG_TRASHCAN;
		else if (LStrCmpI(argv[a],"verify")==0) startflags|=FORMATFLAG_VERIFY;
		else {
			if (start==-1) {
				for (b=0;devicelist->items[b];b++) {
					if (LStrCmpI(argv[a],devicelist->items[b])==0) {
						start=b;
						devicelist->itemselected=b;
						break;
					}
				}
			}
			else LStrnCpy(diskname,argv[a],30);
		}
	}

	if (start>-1) {
		gad=gadlist;
		for (a=0;a<gadcount;a++) {
			if (startflags&(1<<(gad->GadgetID-1))) gad->Flags|=GFLG_SELECTED;
			else gad->Flags&=~GFLG_SELECTED;
			gad=gad->NextGadget;
		}
	}

	RefreshRequesterObject(&formatreq,NULL);
	RefreshGList(gadlist,window,NULL,gadcount);

	if (flags&FORMATFLAG_CACHING && gad_international) {
		flags|=FORMATFLAG_INTERNATIONAL;
		gad_international->Flags|=GFLG_SELECTED;
		RefreshGList(gad_international,window,NULL,1);
		DisableGadget(gad_international,window->RPort,0,0);
	}

	if (!(AddListView(devicelist,1))) {
		CloseRequester(&formatreq);
		return;
	}
	show_sel_item(devicelist);
	show_device_info(&formatreq,infobox,devicelist->items[devicelist->itemselected]);

	if (start>-1) {
		if (do_format(&formatreq,
			infobox,
			devicelist->items[start],
			diskname,
			startflags,
			quick))
			dopus_message(DOPUSMSG_UPDATEDRIVE,(APTR)devicelist->items[start],portname);
		RemoveListView(devicelist,1);
		CloseRequester(&formatreq);
		return;
	}

	FOREVER {
		while (msg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
			if ((view=(struct DOpusListView *)ListViewIDCMP(devicelist,msg))==
				(struct DOpusListView *)-1) {
				class=msg->Class; code=msg->Code;
				iaddress=msg->IAddress;
				ReplyMsg((struct Message *)msg);

				switch (class) {
					case IDCMP_DISKINSERTED:
					case IDCMP_DISKREMOVED:
						if (ignorechange) ignorechange=0;
						else {
							show_device_info(&formatreq,
								infobox,
								devicelist->items[devicelist->itemselected]);
						}
						break;

					case IDCMP_VANILLAKEY:	
						a=0;
						code=ToUpper(code);
						switch (code) {
							case 'N':
								ActivateStrGad(gadlist,window);
								break;
							case 'F':
								a=1;
								flags^=FORMATFLAG_FFS;
								break;
							case 'I':
								if (SysBase->LibNode.lib_Version>38) {
									a=2;
									flags^=FORMATFLAG_INTERNATIONAL;
								}
								break;
							case 'D':
								if (SysBase->LibNode.lib_Version>38) {
									a=3;
									flags^=FORMATFLAG_CACHING;
								}
								break;
							case 'P':
								if (SysBase->LibNode.lib_Version>38) a=4;
								else a=2;
								flags^=FORMATFLAG_TRASHCAN;
								break;
							case 'V':
								if (SysBase->LibNode.lib_Version>38) a=5;
								else a=3;
								flags^=FORMATFLAG_VERIFY;
								break;

							case 0x1b:
								set_env("format",gadlist,gadcount,devicelist);
								RemoveListView(devicelist,1);
								CloseRequester(&formatreq);
								return;
						}
						if (a) {
							gad=gadlist;
							while (a-- && gad->NextGadget) gad=gad->NextGadget;
							gad->Flags^=GFLG_SELECTED;
							RefreshGList(gad,window,NULL,1);
							if (gad==gad_international) {
								gad_caching->Flags&=~GFLG_SELECTED;
								RefreshGList(gad_caching,window,NULL,1);
								flags&=~FORMATFLAG_CACHING;
							}
							else if (gad==gad_caching) {
								gad_international->Flags&=~GFLG_SELECTED;
								RefreshGList(gad_international,window,NULL,1);
								flags&=~FORMATFLAG_INTERNATIONAL;
							}
						}
						break;

					case IDCMP_GADGETDOWN:
					case IDCMP_GADGETUP:
						gadgetid=((struct Gadget *)iaddress)->GadgetID;
					case IDCMP_CLOSEWINDOW:
						if (class==IDCMP_CLOSEWINDOW) gadgetid=FORMAT_CANCEL;

						switch (gadgetid) {
							case FORMAT_FFS:
								flags^=FORMATFLAG_FFS;
								break;

							case FORMAT_INTERNATIONAL:
								flags^=FORMATFLAG_INTERNATIONAL;
								flags&=~FORMATFLAG_CACHING;
								gad_caching->Flags&=~GFLG_SELECTED;
								RefreshGList(gad_caching,window,NULL,1);
								break;

							case FORMAT_CACHING:
								flags^=FORMATFLAG_CACHING;
								if (flags&FORMATFLAG_CACHING) {
									flags|=FORMATFLAG_INTERNATIONAL;
									gad_international->Flags|=GFLG_SELECTED;
									RefreshGList(gad_international,window,NULL,1);
									DisableGadget(gad_international,window->RPort,0,0);
								}
								else EnableGadget(gad_international,window->RPort,0,0);
								break;

							case FORMAT_TRASHCAN:
								flags^=FORMATFLAG_TRASHCAN;
								break;

							case FORMAT_VERIFY:
								flags^=FORMATFLAG_VERIFY;
								break;

							case FORMAT_CANCEL:
								set_env("format",gadlist,gadcount,devicelist);
								RemoveListView(devicelist,1);
								CloseRequester(&formatreq);
								return;

							case FORMAT_FORMAT:
							case FORMAT_QUICKFORMAT:
								if (do_format(&formatreq,
									infobox,
									devicelist->items[devicelist->itemselected],
									diskname,
									flags,
									(gadgetid==FORMAT_QUICKFORMAT))) {
									ignorechange=1;
									dopus_message(DOPUSMSG_UPDATEDRIVE,
										(APTR)devicelist->items[devicelist->itemselected],
										portname);
								}
								break;
						}
						break;
				}
			}
			else if (view) show_device_info(&formatreq,infobox,view->items[view->itemselected]);
		}
		Wait(1<<window->UserPort->mp_SigBit);
	}
}

void show_device_info(reqbase,border,name)
struct RequesterBase *reqbase;
Object_Border *border;
char *name;
{
	struct DeviceNode *devnode;
	struct DosEnvec *dosenvec;
	int tracks,tracksize,size;
	char infobuf[60],sizebuf[20];

	border_text(reqbase,border,NULL);

	if (!name || !(devnode=find_device(name))) return;

	dosenvec=(struct DosEnvec *)
		BADDR(((struct FileSysStartupMsg *)BADDR(devnode->dn_Startup))->fssm_Environ);

	tracks=dosenvec->de_HighCyl-dosenvec->de_LowCyl+1;
	tracksize=(dosenvec->de_BlocksPerTrack*dosenvec->de_Surfaces)*(dosenvec->de_SizeBlock*4);
	size=tracks*tracksize;

	getsizestring(sizebuf,size);
	lsprintf(infobuf,string_table[STR_FORMAT_INFODISPLAY],tracks,tracksize,sizebuf);

	border_text(reqbase,border,infobuf);
}

void getsizestring(buf,a)
char *buf;
ULONG a;
{
	a/=1024;
	if (a>1073741824) lsprintf(buf,"HUGE");
	else if (a>1048576) {
		getfloatstr((double)((double)a/1048576),buf);
		LStrCat(buf,"G");
	}
	else if (a>1024) {
		getfloatstr((double)((double)a/1024),buf);
		LStrCat(buf,"M");
	}
	else lsprintf(buf,"%ldK",(long int)a);
}

void getfloatstr(f,buf)
double f;
char *buf;
{
	int a,b,c,d;
	char buf1[20];

	a=(int)f; f-=a;
	b=(int)(f*100);
	c=(b/10)*10; d=b-c;
	if (d>4) c+=10;
	if (c==100) {
		c=0; ++a;
	}
	lsprintf(buf1,"%ld",(long int)c); buf1[1]=0;
	lsprintf(buf,"%ld.%s",(long int)a,buf1);
}

int do_format(reqbase,border,device,name,flags,quick)
struct RequesterBase *reqbase;
Object_Border *border;
char *device,*name;
ULONG flags;
char quick;
{
	ULONG dostype;
	struct DeviceHandle handle;
	int suc=1,txt=STR_FAILED_ERROR;
	struct Requester busyreq;

	if (flags&FORMATFLAG_INTERNATIONAL) {
		if (flags&FORMATFLAG_FFS) dostype=ID_INTER_FFS_DISK;
		else dostype=ID_INTER_DOS_DISK;
	}
	else if (flags&FORMATFLAG_CACHING) {
		if (flags&FORMATFLAG_FFS) dostype=ID_FASTDIR_FFS_DISK;
		else dostype=ID_FASTDIR_DOS_DISK;
	}
	else if (flags&FORMATFLAG_FFS) dostype=ID_FFS_DISK;
	else dostype=ID_DOS_DISK;

	if (!(open_device(device,&handle))) txt=STR_NODEVICE_ERROR;
	else {
		InitRequester(&busyreq);
		busyreq.Flags=NOISYREQ;
		Request(&busyreq,reqbase->rb_window);
		SetBusyPointer(reqbase->rb_window);

		border_text(reqbase,border,string_table[STR_CHECKING_DESTINATION]);

		if (check_disk(reqbase,handle.device_req,device,1)) {
			if (check_blank_disk(reqbase,device,string_table[STR_FORMAT_FORMAT])) {

				suc=0;

				inhibit_drive(device,DOSTRUE);
				drive_motor(handle.device_req,1);

				if (!quick) {
					suc=do_raw_format(reqbase,border,
						handle.device_req,
						(handle.dosenvec->de_SizeBlock<<2)*
							handle.dosenvec->de_Surfaces*handle.dosenvec->de_BlocksPerTrack,
						handle.dosenvec->de_LowCyl,
						handle.dosenvec->de_HighCyl-handle.dosenvec->de_LowCyl+1,
						handle.dosenvec->de_BufMemType,
						flags);
				}

				if (!suc) {
					border_text(reqbase,border,string_table[STR_FORMAT_INITIALISING]);

					if (DOSBase->dl_lib.lib_Version>36) {
						if (Format(device,name,dostype)) suc=0;
						else suc=ERROR_FAILED;
					}
					else {
						suc=do_initialise(handle.device_req,name,dostype,
							handle.dosenvec->de_LowCyl*
								handle.dosenvec->de_BlocksPerTrack*
								handle.dosenvec->de_Surfaces,
							(handle.dosenvec->de_HighCyl-handle.dosenvec->de_LowCyl+1)*
								handle.dosenvec->de_BlocksPerTrack*handle.dosenvec->de_Surfaces,
							handle.dosenvec->de_Reserved,
							handle.dosenvec->de_BufMemType,
							flags);
					}
				}

				drive_motor(handle.device_req,0);
				inhibit_drive(device,FALSE);

				switch (suc) {
					case 0:
						if (flags&FORMATFLAG_TRASHCAN) {
							if (SysBase->LibNode.lib_Version<36) Delay(150);
							write_trashcan(reqbase,border,device);
						}
						txt=STR_SUCCESS;
						break;
					case ERROR_FAILED: txt=STR_FAILED_ERROR; break;
					case ERROR_MEMORY: txt=STR_MEMORY_ERROR; break;
					case ERROR_BITMAP: txt=STR_BITMAP_ERROR; break;
					case ERROR_ABORTED: txt=STR_ABORTED; break;
					case ERROR_VERIFY: txt=-1; break;
					default: txt=STR_DEVICE_ERROR; break;
				}
			}
			else txt=STR_ABORTED;
		}
		else txt=STR_ABORTED;

		EndRequest(&busyreq,reqbase->rb_window);
		ClearPointer(reqbase->rb_window);
	}

	close_device(&handle);
	if (txt>-1) border_text(reqbase,border,string_table[txt]);
	return((txt==STR_SUCCESS));
}

int do_initialise(device_req,name,dostype,firstblock,numblocks,reserved,memtype,flags)
struct IOExtTD *device_req;
char *name;
ULONG dostype;
ULONG firstblock,numblocks,reserved;
ULONG memtype;
ULONG flags;
{
	ULONG *bitmap,bitmapsize;
	char *buffer;
	struct BitmapBlock *bitmapblock;
	struct BitmapExtension *bitmapextension;
	struct RootDirectory *rootdirectory;
	struct DOpusRemember *key=NULL;
	LONG error,a;
	LONG bitmapcount,bitmapblocks;
	LONG count,root;
	LONG extensioncount,extensionblocks=0;

	bitmapsize=(numblocks-reserved+31)/32;

	if (!(buffer=LAllocRemember(&key,TD_SECTOR,memtype|MEMF_PUBLIC|MEMF_CLEAR)))
		return(ERROR_MEMORY);

	*(ULONG *)buffer=dostype;

	for (a=0;a<2;a++) {
		if (error=do_writeblock(device_req,buffer,firstblock+a)) {
			LFreeRemember(&key);
			return(error);
		}
		*(ULONG *)buffer=0;
	}

	if (!(bitmap=(ULONG *)LAllocRemember(&key,sizeof(ULONG)*bitmapsize,MEMF_PUBLIC))) {
		LFreeRemember(&key);
		return(ERROR_MEMORY);
	}

	for (a=0;a<bitmapsize;a++) bitmap[a]=~0;

	count=bitmapblocks=(bitmapsize+126)/127;

	if (count>25) {
		if (!(flags&FORMATFLAG_FFS)) {
			LFreeRemember(&key);
			return(ERROR_BITMAP);
		}
		count-=25;

		do {
			++extensionblocks;

			if (count>127) count-=127;
			else count=0;
		}
		while (count);
	}

	root=numblocks>>1;
	rootdirectory=(struct RootDirectory *)buffer;
	for (a=0;a<TD_SECTOR;a++) buffer[a]=0;

	rootdirectory->PrimaryType=2;
	rootdirectory->HashTableSize=128-56;
	rootdirectory->BitmapFlag=DOSTRUE;
	DateStamp(&rootdirectory->LastRootChange);

	if ((a=strlen(name))>31) a=31;

	rootdirectory->DiskName[0]=a;
	CopyMem(name,&rootdirectory->DiskName[1],a);

	DateStamp(&rootdirectory->LastDiskChange);
	DateStamp(&rootdirectory->CreationDate);

	rootdirectory->SecondaryType=1;

	for (a=0;a<bitmapblocks;a++) {
		if (a==25) break;
		rootdirectory->BitmapPointers[a]=root+firstblock+a+1;
	}

	if (bitmapblocks>25) rootdirectory->BitmapExtension=root+firstblock+26;
	rootdirectory->Checksum=-do_checksum((ULONG *)rootdirectory);

	if (error=do_writeblock(device_req,rootdirectory,root+firstblock)) {
		LFreeRemember(&key);
		return(error);
	}

	for (a=root-reserved;a<root-reserved+bitmapblocks+extensionblocks+1;a++)
		bitmap[a/32]&=BitTable[a%32];

	bitmapblock=(struct BitmapBlock *)buffer;

	for (a=0;a<bitmapblocks;a++) {
		CopyMem(&bitmap[a*127],&bitmapblock->Bitmap[0],sizeof(ULONG)*127);
		bitmapblock->Checksum=0;
		bitmapblock->Checksum=-do_checksum((ULONG *)bitmapblock);
		if (error=do_writeblock(device_req,bitmapblock,root+firstblock+a+1)) {
			LFreeRemember(&key);
			return(error);
		}
	}

	if (extensionblocks) {
		bitmapextension=(struct BitmapExtension *)buffer;

		bitmapcount=root+firstblock+1;
		extensioncount=bitmapcount+bitmapblocks;
		bitmapblocks-=25;

		while (bitmapblocks) {
			for (a=0;a<TD_SECTOR;a++) buffer[a]=0;
			for (a=0;a<(bitmapblocks<127?bitmapblocks:127);a++)
				bitmapextension->BitmapPointers[a]=bitmapcount++;
			if (bitmapblocks>127) bitmapblocks-=127;
			else bitmapblocks=0;

			if (bitmapblocks) bitmapextension->BitmapExtension=extensioncount+1;
			if (error=do_writeblock(device_req,bitmapextension,extensioncount++)) {
				LFreeRemember(&key);
				return(error);
			}
		}
	}

	LFreeRemember(&key);
	return(0);
}

int do_raw_format(reqbase,border,device_req,tracksize,lowtrack,numtracks,memtype,flags)
struct RequesterBase *reqbase;
Object_Border *border;
struct IOExtTD *device_req;
ULONG tracksize,lowtrack,numtracks;
ULONG memtype;
ULONG flags;
{
	ULONG *trackbuffer,*verifybuffer;
	struct DOpusRemember *key=NULL;
	ULONG offset;
	int track,a,cmpsize,ret;
	char infobuf[80];

	if (!(trackbuffer=(ULONG *)LAllocRemember(&key,tracksize,memtype|MEMF_CLEAR)))
		return(ERROR_MEMORY);
	if (flags&FORMATFLAG_VERIFY) {
		verifybuffer=(ULONG *)LAllocRemember(&key,tracksize,memtype);
		cmpsize=tracksize>>2;
	}
	else verifybuffer=NULL;

	offset=lowtrack*tracksize;

	for (track=0;track<numtracks;track++) {
		lsprintf(infobuf,string_table[STR_FORMAT_FORMATTING],track,numtracks-track-1);
		border_text(reqbase,border,infobuf);

		FOREVER {
			if (check_abort(reqbase->rb_window)) {
				LFreeRemember(&key);
				return(ERROR_ABORTED);
			}

			device_req->iotd_Req.io_Command=TD_FORMAT;
			device_req->iotd_Req.io_Data=(APTR)trackbuffer;
			device_req->iotd_Req.io_Offset=offset;
			device_req->iotd_Req.io_Length=tracksize;

			if (!(DoIO((struct IORequest *)device_req))) {
				if (!verifybuffer) break;
				device_req->iotd_Req.io_Command=CMD_UPDATE;
				if (!(DoIO((struct IORequest *)device_req))) break;
			}

			lsprintf(infobuf,string_table[STR_FORMAT_FORMATERROR],track);
			border_text(reqbase,border,infobuf);
			if (!(check_error(reqbase,infobuf,STR_RETRY))) {
				LFreeRemember(&key);
				return(ERROR_FAILED);
			}
		}

		if (verifybuffer) {
			lsprintf(infobuf,string_table[STR_FORMAT_VERIFYING],track,numtracks-track-1);
			border_text(reqbase,border,infobuf);

			FOREVER {
				if (check_abort(reqbase->rb_window)) {
					LFreeRemember(&key);
					return(ERROR_ABORTED);
				}

				device_req->iotd_Req.io_Command=CMD_READ;
				device_req->iotd_Req.io_Data=(APTR)verifybuffer;
				device_req->iotd_Req.io_Offset=offset;
				device_req->iotd_Req.io_Length=tracksize;

				if (!(DoIO((struct IORequest *)device_req))) {
					for (a=0;a<cmpsize;a++) {
						if (verifybuffer[a]!=0) break;
					}
					if (a==cmpsize) break;
					lsprintf(infobuf,string_table[STR_FORMAT_VERIFYERROR],track);
					ret=ERROR_VERIFY;
				}
				else {
					lsprintf(infobuf,string_table[STR_FORMAT_FORMATERROR],track);
					ret=ERROR_FAILED;
				}

				border_text(reqbase,border,infobuf);
				if (!(check_error(reqbase,infobuf,STR_RETRY))) {
					LFreeRemember(&key);
					return(ret);
				}
			}
		}

		offset+=tracksize;
	}
	LFreeRemember(&key);
	return(0);
}

void write_trashcan(reqbase,border,device)
struct RequesterBase *reqbase;
Object_Border *border;
char *device;
{
	struct DiskObject *trashcan;
	BPTR lock;
	char name[80];

	if (!IconBase) return;

	border_text(reqbase,border,string_table[STR_FORMAT_MAKINGTRASHCAN]);

	lsprintf(name,"%sTrashcan",device);
	if (!(lock=CreateDir(name))) return;
	UnLock(lock);

	if (IconBase->lib_Version>36 &&
		(trashcan=GetDefDiskObject(WBGARBAGE))) {
		PutDiskObject(name,trashcan);
		FreeDiskObject(trashcan);
	}
	else PutDiskObject(name,&trashcanicon_icon);
}
