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

int  main(argc,argv)
int argc;
char *argv[];
{
	struct DOpusRemember *memkey=NULL;
	struct VisInfo *vis;
	char **dummy_args,*port,*opbuf,*stringname;
	struct StringData *stringdata;
	int arg;

	if (!(DOpusBase=(struct DOpusBase *)OpenLibrary("dopus.library",18)))
#ifdef __AROS__
		exit(0);
#else
#ifdef __SASC_60
		__exit(0);
#else
		_exit(0);
#endif
#endif

	if ((vis=LAllocRemember(&memkey,sizeof(struct VisInfo),MEMF_CLEAR)) &&
		(dummy_args=LAllocRemember(&memkey,sizeof(char *)*16,MEMF_CLEAR)) &&
		(opbuf=LAllocRemember(&memkey,80,MEMF_CLEAR)) &&
		(stringname=LAllocRemember(&memkey,80,MEMF_CLEAR)) &&
		(stringdata=LAllocRemember(&memkey,sizeof(struct StringData),MEMF_CLEAR))) {

		IntuitionBase=DOpusBase->IntuitionBase;
		GfxBase=DOpusBase->GfxBase;
		IconBase=OpenLibrary("icon.library",0);

		if (argc==0) {
			struct WBStartup *startup;
			int arg;

			startup=(struct WBStartup *)argv;
			for (arg=0;arg<startup->sm_NumArgs && arg<16;arg++) {
				dummy_args[arg]=startup->sm_ArgList[arg].wa_Name;
				++argc;
			}
			argv=dummy_args;

			if (IconBase) {
				struct DiskObject *dobj;
				char *str;

				if (dobj=GetDiskObject(argv[0])) {
					if (str=FindToolType(dobj->do_ToolTypes,"OPERATION")) {
						LStrnCpy(opbuf,str,80);
						argv[1]=opbuf;
						if (argc<2) argc=2;
					}
					FreeDiskObject(dobj);
				}
			}
		}

		if (argc>1) {
			if (argc>2 && argv[2][0]=='&') {
				arg=3;
				port=&argv[2][1];
			}
			else {
				arg=2;
				port=NULL;
			}
			get_vis_info(vis,port);

			stringdata->default_table=default_strings;
			stringdata->string_count=STR_STRING_COUNT;
			stringdata->min_version=STRING_VERSION;

			if (vis->vi_language)
				lsprintf(stringname,"DOpus:Modules/S/DM_Disk_%s.STR",vis->vi_language);
			else stringname[0]=0;

			if (ReadStringFile(stringdata,stringname)) {
				string_table=stringdata->string_table;

				switch (argv[1][0]) {
					case 'f':
					case 'F':
						diskop_format(vis,port,argc-arg,&argv[arg]);
						break;
					case 'd':
					case 'D':
						diskop_diskcopy(vis,port,argc-arg,&argv[arg]);
						break;
					case 'i':
					case 'I':
						diskop_install(vis,argc-arg,&argv[arg]);
						break;
				}
			}
			FreeStringFile(stringdata);
		}

		if (IconBase) CloseLibrary(IconBase);
	}

	LFreeRemember(&memkey);

	CloseLibrary((struct Library *)DOpusBase);
#ifdef __AROS__
		exit(0);
#else
#ifdef __SASC_60
		__exit(0);
#else
		_exit(0);
#endif
#endif
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
	vis->vi_flags=VISF_WINDOW;
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

int dopus_message(cmd,data,portname)
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

struct Gadget *addreqgadgets(reqbase,gadgets,mask,count)
struct RequesterBase *reqbase;
struct TagItem **gadgets;
int mask,*count;
{
	int gad;
	struct Gadget *gadget=NULL,*newgadget,*firstgadget;

	for (gad=0;;gad++) {
		if (!gadgets[gad]) break;
		if (!(mask&(1<<gad))) {
			if (!(newgadget=(struct Gadget *)
				AddRequesterObject(reqbase,gadgets[gad]))) return(NULL);
			if (gadget) gadget->NextGadget=newgadget;
			else firstgadget=newgadget;
			gadget=newgadget;
			++*count;
		}
	}
	AddGadgets(reqbase->rb_window,firstgadget,NULL,gad,reqbase->rb_shine,reqbase->rb_shadow,1);
	return(firstgadget);
}

int error_rets[]={1,0};

int check_error(reqbase,str,gadtxt)
struct RequesterBase *reqbase;
char *str;
int gadtxt;
{
	struct DOpusSimpleRequest req;
	char *error_gads[3];

	req.text=str;
	error_gads[0]=string_table[gadtxt];
	error_gads[1]=string_table[STR_CANCEL];
	error_gads[2]=NULL;
	req.gads=error_gads;
	req.rets=error_rets;
	req.hi=reqbase->rb_shine;
	req.lo=reqbase->rb_shadow;
	req.fg=reqbase->rb_fg;
	req.bg=reqbase->rb_bg;
	req.strbuf=NULL;
	req.flags=SRF_RECESSHI;
	if (reqbase->rb_flags&RBF_BORDERS) req.flags|=SRF_BORDERS;
	req.font=reqbase->rb_font;
	req.title="DOpus_Disk";
	return(DoSimpleRequest(reqbase->rb_window,&req));
}

ULONG do_checksum(sector)
ULONG *sector;
{
	ULONG sum;
	int i;

	for (sum=0,i=0;i<(TD_SECTOR>>2);i++) sum+=sector[i];
	return(sum);
}

int do_writeblock(device_req,buffer,offset)
struct IOExtTD *device_req;
APTR buffer;
ULONG offset;
{
	device_req->iotd_Req.io_Command=CMD_WRITE;
	device_req->iotd_Req.io_Data=buffer;
	device_req->iotd_Req.io_Length=TD_SECTOR;
	device_req->iotd_Req.io_Offset=offset*TD_SECTOR;
	return((int)DoIO((struct IORequest *)device_req));
}

void inhibit_drive(device,state)
char *device;
ULONG state;
{
#if 0
	struct MsgPort *handler;

	if (DOSBase->dl_lib.lib_Version<36) {
		if (handler=(struct MsgPort *)DeviceProc(device))
			SendPacket(handler,ACTION_INHIBIT,&state,1);
	}
	else Inhibit(device,state);
#else
	Inhibit(device,state);
#endif
}

void border_text(reqbase,border,infobuf)
struct RequesterBase *reqbase;
Object_Border *border;
char *infobuf;
{
	struct RastPort *rp;

	rp=reqbase->rb_window->RPort;
	SetAPen(rp,reqbase->rb_bg);
	RectFill(rp,border->ob_left,border->ob_top,
		border->ob_left+border->ob_width-1,border->ob_top+border->ob_height-1);
	SetAPen(rp,reqbase->rb_fg);

	if (infobuf) {
		ObjectText(reqbase,
			border->ob_left,
			border->ob_top,
			border->ob_width,
			border->ob_height,
			infobuf,
			TEXTPOS_CENTER|TEXTPOS_F_NOUSCORE);
	}
}

struct DeviceNode *find_device(name)
char *name;
{
#ifndef __AROS__
	struct RootNode *rootnode;
	struct DosInfo *dosinfo;
	struct DeviceNode *devnode;
	char matchname[32],devname[32];
	int a;

	if (!name) return(NULL);

	for (a=0;name[a];a++) {
		if (name[a]==':') break;
		matchname[a]=name[a];
	}
	matchname[a]=0;

	Forbid();

	rootnode=(struct RootNode *) DOSBase->dl_Root;
	dosinfo=(struct DosInfo *) BADDR(rootnode->rn_Info);
	devnode=(struct DeviceNode *) BADDR(dosinfo->di_DevInfo);

	while (devnode) {
		if (devnode->dn_Type==DLT_DEVICE && devnode->dn_Task) {
			BtoCStr((BPTR)devnode->dn_Name,devname,32);
			if (strcmp(devname,matchname)==0) {
				Permit();
				return(devnode);
			}
		}
		devnode=(struct DeviceNode *) BADDR(devnode->dn_Next);
	}

	Permit();
	return(NULL);
#else
	struct DosList *dl;

	if (dl = LockDosList(LDF_DEVICES | LDF_READ))
		dl = (APTR)FindDosEntry(dl,name,LDF_DEVICES);
	UnLockDosList(LDF_DEVICES | LDF_READ);
	return (struct DeviceNode *)dl;
#endif
}

char **get_device_list(key,alike)
struct DOpusRemember **key;
char *alike;
{
//    struct RootNode *rootnode;
//    struct DosInfo *dosinfo;
    struct DeviceNode *devnode,*alikenode=NULL;
    int count=1;
    char **listtable,devname[32];
    
    if (alike) alikenode=find_device(alike);
/*
    Forbid();   
        
    rootnode=(struct RootNode *) DOSBase->dl_Root;
    dosinfo=(struct DosInfo *) BADDR(rootnode->rn_Info);
    devnode=(struct DeviceNode *) BADDR(dosinfo->di_DevInfo);
*/
    devnode = (struct DeviceNode *)LockDosList(LDF_READ | LDF_DEVICES);
      
    while ((devnode = (struct DeviceNode *)NextDosEntry((struct DosList *)devnode,LDF_DEVICES))) {
        if (/*devnode->dn_Type==DLT_DEVICE && devnode->dn_Task &&*/
            devnode->dn_Startup>512) {
            if (!alikenode || like_devices(devnode,alikenode)) 
                ++count;
        }
//        devnode=(struct DeviceNode *) BADDR(devnode->dn_Next);
    }
          
    if ((listtable=LAllocRemember(key,count*sizeof(APTR),MEMF_CLEAR))) {
        devnode = (struct DeviceNode *)LockDosList(LDF_READ | LDF_DEVICES);
//        devnode=(struct DeviceNode *) BADDR(dosinfo->di_DevInfo);
        count=0;
        while ((devnode = (struct DeviceNode *)NextDosEntry((struct DosList *)devnode,LDF_DEVICES))) {
//        while (devnode) {
            if (/*devnode->dn_Type==DLT_DEVICE && devnode->dn_Task &&*/
                devnode->dn_Startup>512) {
                if (!alikenode || like_devices(devnode,alikenode)) {
                    BtoCStr((BPTR)devnode->dn_Name,devname,32);
                    strcat(devname,":");
                    if ((listtable[count]=LAllocRemember(key,strlen(devname)+5,0)))
                        strcpy(listtable[count],devname);
                    ++count;
                }
            }
//            devnode=(struct DeviceNode *) BADDR(devnode->dn_Next);
        }
        UnLockDosList(LDF_READ | LDF_DEVICES);
        sort_device_list(listtable);
    }
    UnLockDosList(LDF_READ | LDF_DEVICES);

//    Permit();
    return(listtable);
}

void sort_device_list(table)
char **table;
{
	int count,gap,i,j;
	char *temp;

	for (count=0;table[count];count++);

	for (gap=count/2;gap>0;gap/=2)
		for (i=gap;i<count;i++)
			for (j=i-gap;j>=0;j-=gap) {
				if (LStrCmpI(table[j],table[j+gap])<=0) break;
				temp=table[j];
				table[j]=table[j+gap];
				table[j+gap]=temp;
			}
}

int check_disk(reqbase,device_req,name,prot)
struct RequesterBase *reqbase;
struct IOExtTD *device_req;
char *name;
int prot;
{
	char errorbuf[80];

	FOREVER {
		device_req->iotd_Req.io_Command=TD_CHANGESTATE;
		DoIO((struct IORequest *)device_req);
		if (device_req->iotd_Req.io_Actual) {
			lsprintf(errorbuf,string_table[STR_NODISKPRESENT],name);
			if (!(check_error(reqbase,errorbuf,STR_RETRY))) return(0);
		}
		else break;
	}
	if (prot) {
		FOREVER {
			device_req->iotd_Req.io_Command=TD_PROTSTATUS;
			DoIO((struct IORequest *)device_req);
			if (device_req->iotd_Req.io_Actual) {
				lsprintf(errorbuf,string_table[STR_DISKWRITEPROTECTED],name);
				if (!(check_error(reqbase,errorbuf,STR_RETRY))) return(0);
			}
			else break;
		}
	}
	return(1);
}

int check_abort(window)
struct Window *window;
{
	struct IntuiMessage *msg;
	int abort=0;

	while (msg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
		if (msg->Class==IDCMP_MOUSEBUTTONS && msg->Code==MENUDOWN)
			abort=1;
		ReplyMsg((struct Message *)msg);
	}
	return(abort);
}

int check_blank_disk(reqbase,device,action)
struct RequesterBase *reqbase;
char *device,*action;
{
	BPTR lock;
	struct InfoData __aligned info;
	struct FileInfoBlock __aligned fib;
	struct Process *myproc;
	APTR wsave;
	char blank=0,*message,diskname[40],sizebuf[20];
	int ret=1;

	if (!(message=AllocMem(400,MEMF_CLEAR))) return(1);

	myproc=(struct Process *)FindTask(NULL);
	wsave=myproc->pr_WindowPtr;
	myproc->pr_WindowPtr=(APTR)-1;

	if (lock=Lock(device,ACCESS_READ)) {
		Info(lock,&info);
		switch (info.id_DiskType) {
			case ID_UNREADABLE_DISK:
			case ID_NOT_REALLY_DOS:
				blank=1;
				break;
			default:
				Examine(lock,&fib);
				strcpy(diskname,fib.fib_FileName);
				if (!(ExNext(lock,&fib))) blank=1;
				break;
		}
		UnLock(lock);
	}
	else blank=1;

	myproc->pr_WindowPtr=wsave;

	if (!blank && action) {
		getsizestring(sizebuf,info.id_NumBlocksUsed*info.id_BytesPerBlock);

		lsprintf(message,string_table[STR_DISK_NOT_BLANK],
			device,diskname,sizebuf,action);

		if (!(check_error(reqbase,message,STR_PROCEED))) ret=0;
	}
	FreeMem(message,400);
	return(ret);
}

void set_env(action,gadget,count,list)
char *action;
struct Gadget *gadget;
int count;
struct DOpusListView *list;
{
	BPTR file;
	int listid=MAKE_ID('L','I','S','T'); // FIXME: endiannes?
	UWORD len;
	char envname[80],null=0;

	lsprintf(envname,"ENV:DOpus_%s.prefs",action);
	if (!(file=Open(envname,MODE_NEWFILE))) return;

	while (gadget && count--) {
		if (gadget->Activation&GACT_TOGGLESELECT) {
			Write(file,&gadget->GadgetID,sizeof(UWORD));
			Write(file,&gadget->GadgetType,sizeof(UWORD));
			switch (gadget->GadgetType) {
				case GTYP_BOOLGADGET:
					Write(file,&gadget->Flags,sizeof(UWORD));
					break;
				case GTYP_STRGADGET:
					len=strlen(((struct StringInfo *)gadget->SpecialInfo)->Buffer)+1;
					Write(file,&len,sizeof(UWORD));
					Write(file,((struct StringInfo *)gadget->SpecialInfo)->Buffer,len);
					if (len%2) Write(file,&null,1);
					break;
			}
		}
		else ++count;
		gadget=gadget->NextGadget;
	}
	while (list) {
		Write(file,&listid,sizeof(ULONG));
		Write(file,list->items[list->itemselected],
			(len=(strlen(list->items[list->itemselected])+1)));
		if (len%2) Write(file,&null,1);
		list=list->next;
	}
	Close(file);
}

void get_env(action,firstgadget,count,list)
char *action;
struct Gadget *firstgadget;
int count;
struct DOpusListView *list;
{
	BPTR file;
	int size,a,b,*lbuf;
	char envname[80],*nptr;
	struct Gadget *gadget;
	UWORD gadgettype,gadgetid,len,*buf;

	lsprintf(envname,"ENV:DOpus_%s.prefs",action);
	if (!(file=Open(envname,MODE_OLDFILE))) return;

	Seek(file,0,OFFSET_END);
	size=Seek(file,0,OFFSET_BEGINNING);

	if (!(buf=(UWORD *)AllocMem(size,MEMF_CLEAR))) {
		Close(file);
		return;
	}

	Read(file,(char *)buf,size);
	Close(file);

	for (a=0;a<size/2;) {
		lbuf=(int *)&buf[a];
		if (lbuf[0]==MAKE_ID('L','I','S','T')) { // FIXME: endiannes?
			a+=2;
			nptr=(char *)&buf[a];
			if (list) {
				for (b=0;list->items[b];b++) {
					if (LStrCmpI(nptr,list->items[b])==0) {
						list->itemselected=b;
						break;
					}
				}
				list=list->next;
			}
			b=strlen(nptr)+1;
			if (b%2) ++b;
			a+=b/2;
			continue;
		}
		gadgetid=buf[a++];
		gadgettype=buf[a++];
		gadget=firstgadget;
		while (gadget) {
			if (gadget->GadgetID==gadgetid && gadget->GadgetType==gadgettype)
				break;
			gadget=gadget->NextGadget;
		}
		if (!count--) gadget=NULL;
		switch (gadgettype) {
			case GTYP_BOOLGADGET:
				if (gadget) gadget->Flags=buf[a];
				++a;
				break;
			case GTYP_STRGADGET:
				len=buf[a++];
				if (gadget) strcpy(((struct StringInfo *)gadget->SpecialInfo)->Buffer,(char *)&buf[a]);
				if (len%2) ++len;
				a+=len/2;
				break;
		}
	}
	FreeMem(buf,size);
}

void fix_listview(reqbase,list)
struct RequesterBase *reqbase;
struct DOpusListView *list;
{
	list->window=reqbase->rb_window;
	list->flags|=DLVF_LEAVE|DLVF_SLOW;
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

void select_device(list,exclude)
struct DOpusListView *list;
char *exclude;
{
	int def=-1,item;

	for (item=0;list->items[item];item++) {
		if (!exclude || LStrCmpI(list->items[item],exclude)) {
			if (list->items[item][0]=='D' &&
				list->items[item][1]=='F' &&
				list->items[item][3]==':') {
				def=item;
				break;
			}
			if (def==-1) def=item;
		}
	}
	if (def==-1) def=0;
	list->itemselected=def;
	list->topitem=def;
}

int like_devices(node,likenode)
struct DeviceNode *node,*likenode;
{
	struct DosEnvec *envec,*likeenvec;
	struct FileSysStartupMsg *startup,*likestartup;

	startup=(struct FileSysStartupMsg *)BADDR(node->dn_Startup);
	likestartup=(struct FileSysStartupMsg *)BADDR(likenode->dn_Startup);
	envec=(struct DosEnvec *)BADDR(startup->fssm_Environ);
	likeenvec=(struct DosEnvec *)BADDR(likestartup->fssm_Environ);

	if (envec->de_SizeBlock!=likeenvec->de_SizeBlock ||
		envec->de_Surfaces!=likeenvec->de_Surfaces ||
		envec->de_BlocksPerTrack!=likeenvec->de_BlocksPerTrack ||
		((envec->de_HighCyl-envec->de_LowCyl))!=
			((likeenvec->de_HighCyl-likeenvec->de_LowCyl)))
		return(0);
	return(1);
}

int open_device(device,handle)
char *device;
struct DeviceHandle *handle;
{
	struct DeviceNode *devnode;
	char devicename[40];

	handle->startup=NULL;
	handle->dosenvec=NULL;
	handle->device_port=NULL;
	handle->device_req=NULL;

	if (!device || !(devnode=find_device(device))) return(0);

	handle->startup=(struct FileSysStartupMsg *)BADDR(devnode->dn_Startup);
	handle->dosenvec=(struct DosEnvec *)BADDR(handle->startup->fssm_Environ);
	BtoCStr((BPTR)handle->startup->fssm_Device,devicename,40);

	if (!(handle->device_port=LCreatePort(NULL,0))) return(0);
	if (handle->device_req=(struct IOExtTD *)
		LCreateExtIO(handle->device_port,sizeof(struct IOExtTD))) {
		if (!(OpenDevice(devicename,handle->startup->fssm_Unit,
			(struct IORequest *)handle->device_req,handle->startup->fssm_Flags)))
			return(1);
		LDeleteExtIO((struct IORequest *)handle->device_req);
		handle->device_req=NULL;
	}
	return(0);
}

void close_device(handle)
struct DeviceHandle *handle;
{
	if (handle->device_req) {
		CloseDevice((struct IORequest *)handle->device_req);
		LDeleteExtIO((struct IORequest *)handle->device_req);
		handle->device_req=NULL;
	}
	if (handle->device_port) {
		LDeletePort(handle->device_port);
		handle->device_port=NULL;
	}
}

void drive_motor(req,state)
struct IOExtTD *req;
int state;
{
	req->iotd_Req.io_Command=TD_MOTOR;
	req->iotd_Req.io_Length=state;
	DoIO((struct IORequest *)req);
}

void show_sel_item(list)
struct DOpusListView *list;
{
	while (list) {
		if (list->itemselected<list->topitem ||
			list->itemselected>(list->topitem+list->lines-1)) {
			list->topitem=list->itemselected;
			RefreshListView(list,1);
		}
		list=list->next;
	}
}
