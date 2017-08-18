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
	INSTALL_FFS,
	INSTALL_INSTALL,
	INSTALL_NOBOOT,
	INSTALL_CANCEL};

struct TagItem
	install_device_list[]={
		{RO_Type,OBJECT_LISTVIEW},
		{RO_ListViewID,0},
		{RO_LeftFine,2},
		{RO_Width,10},
		{RO_Height,6},
		{RO_HeightFine,30},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	install_ffs_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_CHECK},
		{RO_GadgetID,INSTALL_FFS},
		{RO_Left,14},
		{RO_TextNum,STR_FORMAT_FFS},
		{RO_TextPos,TEXTPOS_RIGHT},
		{RO_ChkCenter,TRUE},
		{TAG_END,0}},

	install_install_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,INSTALL_INSTALL},
		{RO_Top,8},
		{RO_TopFine,37},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_INSTALL_INSTALL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	install_noboot_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,INSTALL_NOBOOT},
		{RO_Left,14},
		{RO_Top,8},
		{RO_TopFine,37},
		{RO_Width,13},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_INSTALL_NOBOOT},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},
	install_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,INSTALL_CANCEL},
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

	install_info_box[]={
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

	*install_gadgets[]={
		install_ffs_gadget,
		install_install_gadget,
		install_noboot_gadget,
		install_cancel_gadget,
		NULL};

void diskop_install(vis,argc,argv)
struct VisInfo *vis;
int argc;
char *argv[];
{
	struct RequesterBase installreq;
	struct Window *window;
	struct IntuiMessage *msg;
	Object_Border *infobox;
	struct DOpusListView *devicelist,*view;
	struct Gadget *gadlist;
	ULONG class,mask;
	UWORD code,gadgetid;
	APTR iaddress;
	int gadcount=0,ignorechange=0,a,b;
	int start=-1,startffs=0,startnoboot=0;

	installreq.rb_width=41;
	installreq.rb_height=9;
	installreq.rb_widthfine=16;
	installreq.rb_heightfine=54;
	installreq.rb_leftoffset=8;
	installreq.rb_topoffset=8;
	installreq.rb_flags=0;

	fill_out_req(&installreq,vis);

	installreq.rb_privateflags=0;
	installreq.rb_screenname=NULL;

	if (installreq.rb_screen && !(vis->vi_flags&VISF_BORDERS)) {
		installreq.rb_flags|=RBF_STRINGS;
		installreq.rb_title=NULL;
	}
	else {
		installreq.rb_flags|=RBF_BORDERS|RBF_CLOSEGAD|RBF_STRINGS;
		installreq.rb_title=string_table[STR_INSTALL_INSTALL];
	}

	installreq.rb_extend=NULL;
	installreq.rb_idcmpflags=0;
	installreq.rb_string_table=string_table;

	if (SysBase->LibNode.lib_Version<36) mask=1;
	else mask=0;

	if (!(window=OpenRequester(&installreq)) ||
		!(gadlist=addreqgadgets(&installreq,install_gadgets,mask,&gadcount)) ||
		!(infobox=(Object_Border *)
			AddRequesterObject(&installreq,install_info_box)) ||
		!(devicelist=(struct DOpusListView *)
			AddRequesterObject(&installreq,install_device_list)) ||
		!(devicelist->items=get_device_list(&installreq.rb_memory,"DF0:"))) {
		CloseRequester(&installreq);
		return;
	}

	fix_listview(&installreq,devicelist);
	get_env("install",gadlist,gadcount,devicelist);

	for (a=0;a<argc;a++) {
		if (LStrCmpI(argv[a],"ffs")==0 && !mask) startffs=1;
		else if (LStrCmpI(argv[a],"noboot")==0) startnoboot=1;
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
		}
	}

	if (!(AddListView(devicelist,1))) {
		CloseRequester(&installreq);
		return;
	}
	show_sel_item(devicelist);

	if (start>-1) {
		if (startffs) gadlist->Flags|=GFLG_SELECTED;
		else gadlist->Flags&=~GFLG_SELECTED;
	}

	RefreshRequesterObject(&installreq,NULL);
	RefreshGList(gadlist,window,NULL,gadcount);
	show_install_info(&installreq,infobox,devicelist->items[devicelist->itemselected]);

	if (start>-1) {
		do_install(&installreq,infobox,
			devicelist->items[devicelist->itemselected],
			startnoboot,startffs);
		RemoveListView(devicelist,1);
		CloseRequester(&installreq);
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
							show_install_info(&installreq,
								infobox,
								devicelist->items[devicelist->itemselected]);
						}
						break;

					case IDCMP_VANILLAKEY:	
						code=ToUpper(code);
						switch (code) {
							case 'F':
								if (!mask) {
									gadlist->Flags^=GFLG_SELECTED;
									RefreshGList(gadlist,window,NULL,1);
								}
								break;
							case 0x1b:
								set_env("install",gadlist,gadcount,devicelist);
								RemoveListView(devicelist,1);
								CloseRequester(&installreq);
								return;
						}
						break;

					case IDCMP_GADGETDOWN:
					case IDCMP_GADGETUP:
						gadgetid=((struct Gadget *)iaddress)->GadgetID;
					case IDCMP_CLOSEWINDOW:
						if (class==IDCMP_CLOSEWINDOW) gadgetid=INSTALL_CANCEL;

						switch (gadgetid) {
							case INSTALL_CANCEL:
								set_env("install",gadlist,gadcount,devicelist);
								RemoveListView(devicelist,1);
								CloseRequester(&installreq);
								return;
							case INSTALL_INSTALL:
							case INSTALL_NOBOOT:
								if (do_install(&installreq,infobox,
									devicelist->items[devicelist->itemselected],
									(gadgetid==INSTALL_NOBOOT),(!mask && gadlist->Flags&GFLG_SELECTED)))
									ignorechange=1;
								break;
						}
						break;
				}
			}
			else if (view) {
				show_install_info(&installreq,
					infobox,view->items[view->itemselected]);
			}
		}
		Wait(1<<window->UserPort->mp_SigBit);
	}
}

void show_install_info(reqbase,border,name)
struct RequesterBase *reqbase;
Object_Border *border;
char *name;
{
	struct DeviceHandle handle;
	ULONG *buffer,tracksize,dostype;
	char infobuf[80];
	int a,sum,lastsum,ffs;

	border_text(reqbase,border,NULL);
	strcpy(infobuf,string_table[STR_INSTALL_NOINFO]);

	if (open_device(name,&handle)) {
		handle.device_req->iotd_Req.io_Command=TD_CHANGESTATE;
		DoIO((struct IORequest *)handle.device_req);
		if (handle.device_req->iotd_Req.io_Actual)
			lsprintf(infobuf,string_table[STR_NODISKPRESENT],name);
		else {
			tracksize=(handle.dosenvec->de_SizeBlock<<2)*
				handle.dosenvec->de_Surfaces*handle.dosenvec->de_BlocksPerTrack,
			drive_motor(handle.device_req,1);
			if (buffer=(ULONG *)AllocMem(TD_SECTOR*2,handle.dosenvec->de_BufMemType)) {
				handle.device_req->iotd_Req.io_Command=CMD_READ;
				handle.device_req->iotd_Req.io_Data=(APTR)buffer;
				handle.device_req->iotd_Req.io_Offset=handle.dosenvec->de_LowCyl*tracksize;
				handle.device_req->iotd_Req.io_Length=TD_SECTOR*2;
				if (!(DoIO((struct IORequest *)handle.device_req))) {
					dostype=buffer[0];
					for (a=0,sum=0;a<TD_SECTOR>>1;a++) {
						lastsum=sum;
						sum+=buffer[a];
						if (lastsum>sum) ++sum;
					}
					if (sum!=0) strcpy(infobuf,string_table[STR_INSTALL_NOTBOOTABLE]);
					else {
						if (dostype&1) ffs=1;
						else ffs=0;

						if (install_compare_block(buffer,bootblock_13,13)) {
							lsprintf(infobuf,string_table[STR_INSTALL_NORMAL],
								"1.3",(ffs)?"FFS":"OFS");
						}
						else if (install_compare_block(buffer,bootblock_20,24)) {
							lsprintf(infobuf,string_table[STR_INSTALL_NORMAL],
								"2.0+",(ffs)?"FFS":"OFS");
							if ((dostype&0xfffffffe)==ID_INTER_DOS_DISK)
								strcat(infobuf," (Inter)");
							else if ((dostype&0xfffffffe)==ID_FASTDIR_DOS_DISK)
								strcat(infobuf," (Cache)");
						}
						else strcpy(infobuf,string_table[STR_INSTALL_NONSTANDARD]);
					}
				}
				FreeMem(buffer,TD_SECTOR*2);
			}
		}
		drive_motor(handle.device_req,0);
	}
	close_device(&handle);
	border_text(reqbase,border,infobuf);
}

int install_compare_block(block,testblock,size)
ULONG *block,*testblock,size;
{
	int a;

	for (a=3;a<size;a++)
		if (block[a]!=testblock[a])
			return(0);
	return(1);
}

int do_install(reqbase,border,device,noboot,forceffs)
struct RequesterBase *reqbase;
Object_Border *border;
char *device;
int noboot,forceffs;
{
	struct DeviceHandle handle;
	ULONG dostype;
	ULONG *buffer,tracksize;
	BPTR lock;
	struct InfoData __aligned data;
	int a,err=STR_SUCCESS,sum,lastsum;
	struct Requester busyreq;

	InitRequester(&busyreq);
	busyreq.Flags=NOISYREQ;
	Request(&busyreq,reqbase->rb_window);
	SetBusyPointer(reqbase->rb_window);

	if (open_device(device,&handle)) {
		border_text(reqbase,border,string_table[STR_CHECKING_DESTINATION]);
		drive_motor(handle.device_req,1);
		tracksize=(handle.dosenvec->de_SizeBlock<<2)*
			handle.dosenvec->de_Surfaces*handle.dosenvec->de_BlocksPerTrack;

		if (check_disk(reqbase,handle.device_req,device,1)) {
			if (lock=Lock(device,ACCESS_READ)) {
				Info(lock,&data);
				UnLock(lock);

				dostype=data.id_DiskType;

				border_text(reqbase,border,string_table[STR_INSTALL_INSTALLINGDISK]);

				if ((dostype&~(0xff))==0x444f5300) {
					if (forceffs) dostype|=1;
					if (buffer=(ULONG *)AllocMem(TD_SECTOR*2,handle.dosenvec->de_BufMemType|MEMF_CLEAR)) {

						if (noboot) buffer[0]=dostype;
						else {
							if (SysBase->LibNode.lib_Version<36)
								CopyMem((char *)bootblock_13,(char *)buffer,sizeof(bootblock_13));
							else CopyMem((char *)bootblock_20,(char *)buffer,sizeof(bootblock_20));

							buffer[0]=dostype;

							for (a=0,sum=0;a<TD_SECTOR>>1;a++) {
								lastsum=sum;
								sum+=buffer[a];
								if (lastsum>sum) ++sum;
							}

							buffer[1]=~sum;
						}

						inhibit_drive(device,DOSTRUE);

						FOREVER {
							handle.device_req->iotd_Req.io_Command=CMD_WRITE;
							handle.device_req->iotd_Req.io_Data=(APTR)buffer;
							handle.device_req->iotd_Req.io_Offset=handle.dosenvec->de_LowCyl*tracksize;
							handle.device_req->iotd_Req.io_Length=TD_SECTOR*2;

							if (!(DoIO((struct IORequest *)handle.device_req))) {
								handle.device_req->iotd_Req.io_Command=CMD_UPDATE;
								if (!(DoIO((struct IORequest *)handle.device_req))) break;
							}

							border_text(reqbase,border,string_table[STR_INSTALL_ERRORWRITING]);
							if (!(check_error(reqbase,string_table[STR_INSTALL_ERRORWRITING],STR_RETRY))) {
								err=-1;
								break;
							}
						}

						inhibit_drive(device,FALSE);
						FreeMem(buffer,TD_SECTOR*2);
					}
					else err=STR_MEMORY_ERROR;
				}
				else err=STR_INVALID_DISK;
			}
			else err=STR_INVALID_DISK;
		}
		else err=STR_ABORTED;

		drive_motor(handle.device_req,0);
	}
	else err=STR_NODEVICE_ERROR;

	if (err>-1) border_text(reqbase,border,string_table[err]);

	close_device(&handle);

	EndRequest(&busyreq,reqbase->rb_window);
	ClearPointer(reqbase->rb_window);
	return((err==STR_SUCCESS));
}
