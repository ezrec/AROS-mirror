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

enum {
	DISKCOPY_VERIFY,
	DISKCOPY_BUMPNAMES,
	DISKCOPY_DISKCOPY,
	DISKCOPY_CANCEL,
	DISKCOPY_CHECK};

struct TagItem
	diskcopy_source_list[]={
		{RO_Type,OBJECT_LISTVIEW},
		{RO_ListViewID,0},
		{RO_Top,1},
		{RO_LeftFine,2},
		{RO_Width,10},
		{RO_Height,6},
		{RO_HeightFine,30},
		{RO_TextNum,STR_DISKCOPY_FROM},
		{RO_TextPos,TEXTPOS_ABOVE},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	diskcopy_destination_list[]={
		{RO_Type,OBJECT_LISTVIEW},
		{RO_ListViewID,1},
		{RO_Top,1},
		{RO_Left,33},
		{RO_LeftFine,-18},
		{RO_Width,10},
		{RO_Height,6},
		{RO_HeightFine,30},
		{RO_TextNum,STR_DISKCOPY_TO},
		{RO_TextPos,TEXTPOS_ABOVE},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	diskcopy_verify_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,DISKCOPY_VERIFY},
		{RO_Left,14},
		{RO_Top,4},
		{RO_TextNum,STR_FORMAT_VERIFY},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_LeftFine,-2},
		{RO_BoolOn,TRUE},
		{TAG_END,0}},

	diskcopy_bumpnames_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,DISKCOPY_BUMPNAMES},
		{RO_Left,14},
		{RO_Top,6},
		{RO_TextNum,STR_BUMP_NAMES},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_LeftFine,-2},
		{RO_BoolOn,TRUE},
		{TAG_END,0}},

	diskcopy_info_box[]={
		{RO_Type,OBJECT_BORDER},
		{RO_BorderType,BORDER_RECESSED},
		{RO_LeftFine,2},
		{RO_Top,7},
		{RO_TopFine,36},
		{RO_Width,43},
		{RO_WidthFine,-4},
		{RO_Height,1},
		{RO_HeightFine,4},
		{TAG_END,0}},

	diskcopy_diskcopy_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,DISKCOPY_DISKCOPY},
		{RO_Top,8},
		{RO_TopFine,45},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_DISKCOPY_DISKCOPY},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	diskcopy_check_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,DISKCOPY_CHECK},
		{RO_Left,15},
		{RO_Top,8},
		{RO_TopFine,45},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_DISKCOPY_CHECK},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	diskcopy_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,DISKCOPY_CANCEL},
		{RO_Left,30},
		{RO_Top,8},
		{RO_TopFine,45},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_FORMAT_EXIT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	*diskcopy_gadgets[]={
		diskcopy_verify_gadget,
		diskcopy_bumpnames_gadget,
		diskcopy_diskcopy_gadget,
		diskcopy_check_gadget,
		diskcopy_cancel_gadget,
		NULL};

void diskop_diskcopy(vis,portname,argc,argv)
struct VisInfo *vis;
char *portname;
int argc;
char *argv[];
{
	struct RequesterBase diskcopyreq;
	struct Window *window;
	struct IntuiMessage *msg;
	Object_Border *infobox;
	struct DOpusListView *sourcelist,*destinationlist,*view;
	struct Gadget *gadlist;
	ULONG class;
	UWORD code,gadgetid;
	APTR iaddress;
	int a,b,gadcount=0,ignorechange=0;
	int source=-1,dest=-1,startverify=0,bumpnames=0,oldsel;
	int startcheck=0;
	struct DOpusRemember *destkey=NULL;

	diskcopyreq.rb_width=43;
	diskcopyreq.rb_height=9;
	diskcopyreq.rb_widthfine=16;
	diskcopyreq.rb_heightfine=61;
	diskcopyreq.rb_leftoffset=8;
	diskcopyreq.rb_topoffset=8;
	diskcopyreq.rb_flags=0;

	fill_out_req(&diskcopyreq,vis);

	diskcopyreq.rb_privateflags=0;
	diskcopyreq.rb_screenname=NULL;

	if (diskcopyreq.rb_screen && !(vis->vi_flags&VISF_BORDERS)) {
		diskcopyreq.rb_flags|=RBF_STRINGS;
		diskcopyreq.rb_title=NULL;
	}
	else {
		diskcopyreq.rb_flags|=RBF_BORDERS|RBF_CLOSEGAD|RBF_STRINGS;
		diskcopyreq.rb_title=string_table[STR_DISKCOPY_DISKCOPY];
	}

	diskcopyreq.rb_extend=NULL;
	diskcopyreq.rb_idcmpflags=0;
	diskcopyreq.rb_string_table=string_table;

	if (!(window=OpenRequester(&diskcopyreq)) ||
		!(gadlist=addreqgadgets(&diskcopyreq,diskcopy_gadgets,0,&gadcount)) ||
		!(infobox=(Object_Border *)
			AddRequesterObject(&diskcopyreq,diskcopy_info_box)) ||
		!(sourcelist=(struct DOpusListView *)
			AddRequesterObject(&diskcopyreq,diskcopy_source_list)) ||
		!(sourcelist->items=get_device_list(&diskcopyreq.rb_memory,NULL)) ||
		!(destinationlist=(struct DOpusListView *)
			AddRequesterObject(&diskcopyreq,diskcopy_destination_list))) {
		CloseRequester(&diskcopyreq);
		return;
	}

	fix_listview(&diskcopyreq,sourcelist);
	fix_listview(&diskcopyreq,destinationlist);

	select_device(sourcelist,NULL);
	get_env("diskcopy",gadlist,gadcount,sourcelist);

	oldsel=sourcelist->itemselected;

	for (a=0;a<argc;a++) {
		if (LStrCmpI(argv[a],"verify")==0) startverify=1;
		else if (LStrnCmpI(argv[a],"bump",4)==0) bumpnames=1;
		else if (LStrnCmpI(argv[a],"check",5)==0) startcheck=1;
		else {
			if (source==-1) {
				for (b=0;sourcelist->items[b];b++) {
					if (LStrCmpI(argv[a],sourcelist->items[b])==0) {
						source=b;
						sourcelist->itemselected=b;
						destinationlist->items=
							get_device_list(&destkey,sourcelist->items[b]);
						break;
					}
				}
			}
			else if (dest==-1 && destinationlist->items) {
				for (b=0;destinationlist->items[b];b++) {
					if (LStrCmpI(argv[a],destinationlist->items[b])==0) {
						dest=b;
						destinationlist->itemselected=b;
						break;
					}
				}
			}
		}
	}

	if (source>-1 && dest==-1) {
		LFreeRemember(&destkey);
		destinationlist->items=NULL;
		sourcelist->itemselected=oldsel;
		startverify=bumpnames=0;
	}

	if (!destinationlist->items) {
		destinationlist->items=
			get_device_list(&destkey,sourcelist->items[sourcelist->itemselected]);
		select_device(destinationlist,sourcelist->items[sourcelist->itemselected]);
		sourcelist->next=destinationlist;
		if (source==-1) get_env("diskcopy",gadlist,gadcount,sourcelist);
	}
	else sourcelist->next=destinationlist;

	if (!(AddListView(sourcelist,2))) {
		CloseRequester(&diskcopyreq);
		LFreeRemember(&destkey);
		return;
	}
	show_sel_item(sourcelist);

	if (startverify) gadlist->Flags|=GFLG_SELECTED;
	else if (dest>-1) gadlist->Flags&=~GFLG_SELECTED;

	if (bumpnames) gadlist->NextGadget->Flags|=GFLG_SELECTED;
	else if (dest>-1) gadlist->NextGadget->Flags&=~GFLG_SELECTED;

	RefreshRequesterObject(&diskcopyreq,NULL);
	RefreshGList(gadlist,window,NULL,gadcount);
	show_diskcopy_info(&diskcopyreq,infobox,sourcelist->items[sourcelist->itemselected]);

	if (dest>-1) {
		if (do_diskcopy(&diskcopyreq,
			infobox,
			sourcelist->items[sourcelist->itemselected],
			destinationlist->items[destinationlist->itemselected],
			(gadlist->Flags&GFLG_SELECTED),
			(gadlist->NextGadget->Flags&GFLG_SELECTED),
			startcheck)) {
			dopus_message(DOPUSMSG_UPDATEDRIVE,
				(APTR)destinationlist->items[destinationlist->itemselected],
				portname);
		}
		RemoveListView(sourcelist,2);
		CloseRequester(&diskcopyreq);
		LFreeRemember(&destkey);
		return;
	}

	FOREVER {
		while (msg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
			if ((view=(struct DOpusListView *)ListViewIDCMP(sourcelist,msg))==
				(struct DOpusListView *)-1) {
				class=msg->Class; code=msg->Code;
				iaddress=msg->IAddress;
				ReplyMsg((struct Message *)msg);

				switch (class) {
					case IDCMP_DISKINSERTED:
					case IDCMP_DISKREMOVED:
						if (ignorechange) ignorechange=0;
						else {
							show_diskcopy_info(&diskcopyreq,
								infobox,
								sourcelist->items[sourcelist->itemselected]);
						}
						break;

					case IDCMP_VANILLAKEY:	
						code=ToUpper(code);
						switch (code) {
							case 'V':
								gadlist->Flags^=GFLG_SELECTED;
								RefreshGList(gadlist,window,NULL,1);
								break;
							case 'B':
								gadlist->NextGadget->Flags^=GFLG_SELECTED;
								RefreshGList(gadlist,window,NULL,1);
								break;
							case 0x1b:
								set_env("diskcopy",gadlist,gadcount,sourcelist);
								RemoveListView(sourcelist,2);
								CloseRequester(&diskcopyreq);
								LFreeRemember(&destkey);
								return;
						}
						break;

					case IDCMP_GADGETDOWN:
					case IDCMP_GADGETUP:
						gadgetid=((struct Gadget *)iaddress)->GadgetID;
					case IDCMP_CLOSEWINDOW:
						if (class==IDCMP_CLOSEWINDOW) gadgetid=DISKCOPY_CANCEL;

						switch (gadgetid) {
							case DISKCOPY_CANCEL:
								set_env("diskcopy",gadlist,gadcount,sourcelist);
								RemoveListView(sourcelist,2);
								CloseRequester(&diskcopyreq);
								LFreeRemember(&destkey);
								return;
							case DISKCOPY_DISKCOPY:
							case DISKCOPY_CHECK:
								if (do_diskcopy(&diskcopyreq,
									infobox,
									sourcelist->items[sourcelist->itemselected],
									destinationlist->items[destinationlist->itemselected],
									(gadlist->Flags&GFLG_SELECTED),
									(gadlist->NextGadget->Flags&GFLG_SELECTED),
									(gadgetid==DISKCOPY_CHECK))) {
									ignorechange=1;
									dopus_message(DOPUSMSG_UPDATEDRIVE,
										(APTR)destinationlist->items[destinationlist->itemselected],
										portname);
								}
								break;
						}
						break;
				}
			}
			else if (view) {
				switch (view->listid) {
					case 0:
						LFreeRemember(&destkey);
						destinationlist->items=get_device_list(&destkey,view->items[view->itemselected]);
						select_device(destinationlist,view->items[view->itemselected]);
						RefreshListView(destinationlist,1);
						show_diskcopy_info(&diskcopyreq,infobox,sourcelist->items[sourcelist->itemselected]);
						break;
				}
			}
		}
		Wait(1<<window->UserPort->mp_SigBit);
	}
}

void show_diskcopy_info(reqbase,border,name)
struct RequesterBase *reqbase;
Object_Border *border;
char *name;
{
	char infobuf[60],sizebuf[20];
	BPTR lock;
	struct FileInfoBlock __aligned fib;
	struct InfoData __aligned info;
	struct Process *myproc;
	APTR wsave;

	myproc=(struct Process *)FindTask(NULL);
	wsave=myproc->pr_WindowPtr;
	myproc->pr_WindowPtr=(APTR)-1;

	border_text(reqbase,border,NULL);

	if (name) {
		if (lock=Lock(name,ACCESS_READ)) {
			Info(lock,&info);
			Examine(lock,&fib);
			UnLock(lock);

			getsizestring(sizebuf,info.id_NumBlocksUsed*info.id_BytesPerBlock);
			lsprintf(infobuf,string_table[STR_DISKCOPY_INFODISPLAY],fib.fib_FileName,sizebuf);

			border_text(reqbase,border,infobuf);
		}
		else border_text(reqbase,border,string_table[STR_DISKCOPY_NODISK]);
	}
	myproc->pr_WindowPtr=wsave;
}

int do_diskcopy(reqbase,border,source,dest,verify,bump,check)
struct RequesterBase *reqbase;
Object_Border *border;
char *source,*dest;
int verify,bump,check;
{
	struct IOExtTD *device_req[2];
	struct MsgPort *device_port[2];
	struct DeviceNode *source_node,*dest_node;
	struct FileSysStartupMsg *startup;
	struct DosEnvec *dosenvec;
	struct Requester busyreq;
	ULONG tracksize,lowtrack[2],numtracks,memtype,deviceunit[2],deviceflags[2],track;
	ULONG trackcount,trackmod,curtrack,offset;
	char devicename[2][40],infobuf[80];
	int a,b,drives,abort=0,err=0,cmpsize,txt=-1;
	char *buffer=NULL,*dev_table[2];
	ULONG *verifybuffer=NULL,*cmpbuffer;
	struct DOpusRemember *memkey=NULL;

	if (!source || !dest ||
		!(source_node=find_device(source)) ||
		!(dest_node=find_device(dest))) return(0);

	if (check) verify=1;
	if (LStrCmpI(source,dest)==0) drives=1;
	else drives=2;

	dev_table[0]=source; dev_table[1]=dest;

	for (a=0;a<2;a++) {
		device_port[a]=NULL;
		device_req[a]=NULL;
	}

	startup=(struct FileSysStartupMsg *)BADDR(source_node->dn_Startup);
	BtoCStr((BPTR)startup->fssm_Device,devicename[0],40);
	deviceunit[0]=startup->fssm_Unit;
	deviceflags[0]=startup->fssm_Flags;

	dosenvec=(struct DosEnvec *)BADDR(startup->fssm_Environ);
	tracksize=(dosenvec->de_SizeBlock<<2)*dosenvec->de_Surfaces*dosenvec->de_BlocksPerTrack;
	lowtrack[0]=dosenvec->de_LowCyl;
	numtracks=dosenvec->de_HighCyl-dosenvec->de_LowCyl+1;
	memtype=dosenvec->de_BufMemType;

	Forbid();

	if (drives==2) trackcount=1;
	else {
		if ((trackcount=AvailMem(memtype|MEMF_LARGEST)/tracksize)>1) {
			if (numtracks%trackcount) {
				trackmod=(numtracks/trackcount)+1;
				trackcount=numtracks/trackmod;
			}
		}
	}

	if (trackcount>0) buffer=LAllocRemember(&memkey,tracksize*trackcount,memtype);

	Permit();

	if (!buffer) {
		border_text(reqbase,border,string_table[STR_MEMORY_ERROR]);
		return(0);
	}

	cmpbuffer=(ULONG *)buffer;

	if (verify) {
		verifybuffer=(ULONG *)LAllocRemember(&memkey,tracksize,memtype);
		cmpsize=tracksize>>2;
	}
	if (check && !verifybuffer) {
		border_text(reqbase,border,string_table[STR_MEMORY_ERROR]);
		return(0);
	}

	startup=(struct FileSysStartupMsg *)BADDR(dest_node->dn_Startup);
	BtoCStr((BPTR)startup->fssm_Device,devicename[1],40);
	deviceunit[1]=startup->fssm_Unit;
	deviceflags[1]=startup->fssm_Flags;

	dosenvec=(struct DosEnvec *)BADDR(startup->fssm_Environ);
	lowtrack[1]=dosenvec->de_LowCyl;

	for (a=0;a<2;a++) {
		if (!(device_port[a]=LCreatePort(NULL,0)) ||
			!(device_req[a]=(struct IOExtTD *)
			LCreateExtIO(device_port[a],sizeof(struct IOExtTD)))) break;
		if (OpenDevice(devicename[a],deviceunit[a],
			(struct IORequest *)device_req[a],deviceflags[a])) {
			LDeleteExtIO((struct IORequest *)device_req[a]);
			device_req[a]=NULL;
			break;
		}
	}

	if (a==2) {
		InitRequester(&busyreq);
		busyreq.Flags=NOISYREQ;
		Request(&busyreq,reqbase->rb_window);
		SetBusyPointer(reqbase->rb_window);

		if (drives==2) {
			border_text(reqbase,border,string_table[STR_CHECKING_DISKS]);

			for (a=0;a<2;a++) {
				if (!(check_disk(reqbase,device_req[a],dev_table[a],(check)?0:a))) break;
				if (a==1 && !check && !(check_blank_disk(reqbase,dev_table[a],
					string_table[STR_DISKCOPY_DISKCOPY])))
					break;
			}
		}
		else a=2;

		if (a==2) {
			inhibit_drive(source,DOSTRUE);
			if (drives==2) inhibit_drive(dest,DOSTRUE);

			for (a=0;a<drives;a++) drive_motor(device_req[a],1);

			for (track=0;track<numtracks;track+=trackcount) {

				offset=(lowtrack[0]+track)*tracksize;

				if (drives==1) {
					lsprintf(infobuf,string_table[STR_DISKCOPY_INSERTSOURCE],source);
					border_text(reqbase,border,infobuf);
					drive_motor(device_req[0],0);
					if (!(check_error(reqbase,infobuf,STR_PROCEED)) ||
						!(check_disk(reqbase,device_req[0],source,0))) {
						err=ERROR_ABORTED;
						break;
					}
					drive_motor(device_req[0],1);
				}

				for (curtrack=0;curtrack<trackcount;curtrack++) {

					lsprintf(infobuf,string_table[STR_DISKCOPY_READING],
						track+curtrack,numtracks-curtrack-track-1);
					border_text(reqbase,border,infobuf);

					if (check_abort(reqbase->rb_window)) {
						abort=1;
						err=ERROR_ABORTED;
						break;
					}

					FOREVER {
						device_req[0]->iotd_Req.io_Command=CMD_READ;
						device_req[0]->iotd_Req.io_Data=(APTR)&buffer[curtrack*tracksize];
						device_req[0]->iotd_Req.io_Offset=offset;
						device_req[0]->iotd_Req.io_Length=tracksize;

						if (!(DoIO((struct IORequest *)device_req[0]))) break;

						lsprintf(infobuf,string_table[STR_DISKCOPY_READERROR],track+curtrack);
						border_text(reqbase,border,infobuf);
						if (!(check_error(reqbase,infobuf,STR_RETRY))) {
							abort=1;
							err=ERROR_FAILED;
							break;
						}
					}

					if (abort) break;
					offset+=tracksize;
				}

				if (abort) break;

				offset=(lowtrack[1]+track)*tracksize;

				if (drives==1) {
					lsprintf(infobuf,string_table[STR_DISKCOPY_INSERTDEST],dest);
					border_text(reqbase,border,infobuf);
					drive_motor(device_req[0],0);
					if (!(check_error(reqbase,infobuf,STR_PROCEED)) ||
						!(check_disk(reqbase,device_req[1],dest,1))) {
						err=ERROR_ABORTED;
						abort=1;
						break;
					}
					drive_motor(device_req[0],1);
				}

				for (curtrack=0;curtrack<trackcount;curtrack++) {

					if (!check) {
						lsprintf(infobuf,string_table[STR_DISKCOPY_WRITING],
							track+curtrack,numtracks-curtrack-track-1);
						border_text(reqbase,border,infobuf);

						if (check_abort(reqbase->rb_window)) {
							abort=1;
							err=ERROR_ABORTED;
							break;
						}

						FOREVER {
							device_req[1]->iotd_Req.io_Command=TD_FORMAT;
							device_req[1]->iotd_Req.io_Data=(APTR)&buffer[curtrack*tracksize];
							device_req[1]->iotd_Req.io_Offset=offset;
							device_req[1]->iotd_Req.io_Length=tracksize;

							if (!(DoIO((struct IORequest *)device_req[1]))) {
								if (!verifybuffer) break;
								device_req[1]->iotd_Req.io_Command=CMD_UPDATE;
								if (!(DoIO((struct IORequest *)device_req[1]))) break;
							}

							lsprintf(infobuf,string_table[STR_DISKCOPY_WRITEERROR],track+curtrack);
							border_text(reqbase,border,infobuf);
							if (!(check_error(reqbase,infobuf,STR_RETRY))) {
								abort=1;
								err=ERROR_FAILED;
								break;
							}
						}

						if (abort) break;
					}

					if (verifybuffer) {
						lsprintf(infobuf,string_table[STR_FORMAT_VERIFYING],
							track+curtrack,numtracks-curtrack-track-1);
						border_text(reqbase,border,infobuf);

						FOREVER {
							device_req[1]->iotd_Req.io_Command=CMD_READ;
							device_req[1]->iotd_Req.io_Data=(APTR)verifybuffer;
							device_req[1]->iotd_Req.io_Offset=offset;
							device_req[1]->iotd_Req.io_Length=tracksize;

							if (!(DoIO((struct IORequest *)device_req[1]))) {
								for (a=0,b=curtrack*cmpsize;a<cmpsize;a++,b++) {
									if (verifybuffer[a]!=cmpbuffer[b]) break;
								}
								if (a==cmpsize) break;
								lsprintf(infobuf,string_table[STR_FORMAT_VERIFYERROR],track+curtrack);
								err=ERROR_VERIFY;
							}
							else {
								lsprintf(infobuf,string_table[STR_DISKCOPY_READERROR],track+curtrack);
								err=ERROR_FAILED;
							}

							border_text(reqbase,border,infobuf);
							if (!(check_error(reqbase,infobuf,STR_RETRY))) {
								abort=1;
								break;
							}
							err=0;
						}

						if (abort) break;
					}

					offset+=tracksize;
				}

			}

			for (a=0;a<drives;a++) drive_motor(device_req[a],0);

			switch (err) {
				case 0:
					if (bump && !check) {
						border_text(reqbase,border,string_table[STR_BUMPING_NAME]);
						inhibit_drive(dest,FALSE);
						bump_disk_name(dest);
					}
					txt=STR_SUCCESS;
					break;
				case ERROR_ABORTED: txt=STR_ABORTED; break;
				case ERROR_FAILED: txt=STR_FAILED_ERROR; break;
				case ERROR_VERIFY: txt=-1; break;
			}

			inhibit_drive(source,FALSE);
			if (drives==2) inhibit_drive(dest,FALSE);
		}
		else txt=STR_ABORTED;

		EndRequest(&busyreq,reqbase->rb_window);
		ClearPointer(reqbase->rb_window);
	}
	else txt=STR_NODEVICE_ERROR;

	if (txt>-1) border_text(reqbase,border,string_table[txt]);

	for (a=0;a<2;a++) {
		if (device_req[a]) {
			CloseDevice((struct IORequest *)device_req[a]);
			LDeleteExtIO((struct IORequest *)device_req[a]);
		}
		if (device_port[a]) LDeletePort(device_port[a]);
	}

	LFreeRemember(&memkey);
	return((txt==STR_SUCCESS));
}

void bump_disk_name(disk)
char *disk;
{
	if (IconBase) {
		struct Process *myproc;
		APTR wsave;
		BPTR lock;

		myproc=(struct Process *)FindTask(NULL);
		wsave=myproc->pr_WindowPtr;
		myproc->pr_WindowPtr=(APTR)-1;

		if (lock=Lock(disk,ACCESS_READ)) {
			struct FileInfoBlock __aligned info;
			char namebuf[32];
			struct MsgPort *port;

			Examine(lock,&info);
			UnLock(lock);

			BumpRevision(namebuf,info.fib_FileName);

			if (port=(struct MsgPort *)DeviceProc(disk)) {
				char __aligned bstr_name[36];
				IPTR arg;

#ifdef __AROS__
				BPTR bstr = MKBADDR(bstr_name);
				CopyMem(namebuf, AROS_BSTR_ADDR(bstr), strlen(namebuf));
				AROS_BSTR_setstrlen(bstr, strlen(namebuf));
				arg=(IPTR)bstr;
#else
				bstr_name[0]=strlen(namebuf);
				strcpy(&bstr_name[1],namebuf);
				arg=(ULONG)bstr_name>>2;
#endif
				SendPacket(port,ACTION_RENAME_DISK,&arg,1);
			}
		}
		myproc->pr_WindowPtr=wsave;
	}
}
