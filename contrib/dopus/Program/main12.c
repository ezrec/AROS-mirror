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

void setupchangestate()
{
	int unit;
	struct IOStdReq *diskreq;

	disk_change_state=0;
	for (unit=0;unit<4;unit++) {
		if ((diskreq=(struct IOStdReq *)LCreateExtIO(general_port,sizeof(struct IOStdReq)))) {
			if (!(OpenDevice("trackdisk.device",unit,(struct IORequest *)diskreq,0))) {
				diskreq->io_Command=TD_CHANGESTATE;
				DoIO((struct IORequest *)diskreq);
				if (!diskreq->io_Actual) disk_change_state|=1<<unit;
				CloseDevice((struct IORequest *)diskreq);
			}
			LDeleteExtIO((struct IORequest *)diskreq);
		}
	}
}

#define ITEM_NUM 11

static short
	diskinfo_heads[]={
		STR_DISKINFO_DEVICE,
		STR_DISKINFO_NAME,
		STR_DISKINFO_SIZE,
		STR_DISKINFO_USED,
		STR_DISKINFO_FREE,
		STR_DISKINFO_PERCENT,
		STR_DISKINFO_DENSITY,
		STR_DISKINFO_ERRORS,
		STR_DISKINFO_STATUS,
		STR_DISKINFO_DATE,
		STR_DISKINFO_FILESYSTEM};

void dodiskinfo(path)
char *path;
{
#warning  AppIcon stuff not implemented yet

kprintf("dodiskifo() NOT IMPLEMENTED BECAUSE OF MISSING Info() FUNC\n");
#if 0
	struct RastPort *dirp;
	int a,b,c,isd=-1,cx,cy,fail=0,xoffset,yoffset,titwidth;
	static char disktxt[11][60],buf[256],formstr[80],diskname[40],dname[10],buf1[40];
	float f;
	struct DateStamp ds;
	struct MsgPort *prt;
	struct InfoData __aligned infodata;
	struct Gadget contgad;
	char *gad_gads[2];
	BPTR lock;
	UBYTE old_change_state;
	ULONG class;
	USHORT code;
	struct DOpusRemember *key=NULL;

	if (!path || !path[0]) return;

	cx=scr_font[FONT_REQUEST]->tf_XSize;
	cy=scr_font[FONT_REQUEST]->tf_YSize;

	for (a=0;a<ITEM_NUM;a++) {
		disktxt[a][0]='-';
		disktxt[a][1]=0;
	}
	main_proc->pr_WindowPtr=(APTR)-1;
getnewdisk:
	strcpy(diskname,path);
	if (getroot(diskname,&ds)) {
		strcat(diskname,":");
		if ((prt=(struct MsgPort *)DeviceProc(diskname))) {
			if (lock=Lock(diskname,ACCESS_READ)) {
				Forbid();
				get_device_task(lock,disktxt[0],prt);

				for (a=0;a<4;a++) {
					lsprintf(dname,"DF%ld:",a);
					if (LStrCmpI(disktxt[0],dname)==0) {
						isd=a;
						break;
					}
				}
				Permit();
				Info(lock,&infodata);
				UnLock(lock);
				switch (infodata.id_DiskType) {
					case ID_UNREADABLE_DISK:
						strcpy(disktxt[1],globstring[STR_UNREADABLE_DISK]);
						break;
					case ID_NOT_REALLY_DOS:
						strcpy(disktxt[1],globstring[STR_NOT_A_DOS_DISK]);	
						break;
					case ID_KICKSTART_DISK:
						strcpy(disktxt[1],globstring[STR_KICKSTART_DISK]);
						break;
					default:
						strcpy(buf,path);
						strcpy(disktxt[1],diskname);
						disktxt[1][(strlen(disktxt[1])-1)]=0;
						a=infodata.id_NumBlocks*infodata.id_BytesPerBlock;
						lsprintf(buf,"%ld",a); b=strlen(buf);
						lsprintf(buf,"%ld",infodata.id_NumBlocks);
						lsprintf(formstr,"%%%ldld %s; %%%ldld %s; %%s",
							b,globstring[STR_DISKINFO_BYTES],
							strlen(buf),globstring[STR_DISKINFO_BLOCKS]);
						getsizestring(buf,a);
						lsprintf(disktxt[2],formstr,a,infodata.id_NumBlocks,buf);
						a=infodata.id_NumBlocksUsed*infodata.id_BytesPerBlock;
						getsizestring(buf,a);
						lsprintf(disktxt[3],formstr,a,infodata.id_NumBlocksUsed,buf);
						a=(infodata.id_NumBlocks-infodata.id_NumBlocksUsed)*infodata.id_BytesPerBlock;
						getsizestring(buf,a);
						lsprintf(disktxt[4],formstr,a,(infodata.id_NumBlocks-infodata.id_NumBlocksUsed),buf);
						if (infodata.id_NumBlocks==0) f=100;
						else f=((float)(infodata.id_NumBlocksUsed*100)/(float)infodata.id_NumBlocks);
						getfloatstr(f,buf); getfloatstr((float)(100-f),buf1);
						lsprintf(disktxt[5],"%s%% %s; %s%% %s",
							buf,globstring[STR_DISKINFO_FULL],
							buf1,globstring[STR_DISKINFO_SFREE]);
						lsprintf(disktxt[6],"%ld %s",
							infodata.id_BytesPerBlock,
							globstring[STR_DISKINFO_BYTESPERBLOCK]);
						lsprintf(disktxt[7],"%ld",infodata.id_NumSoftErrors);
						switch (infodata.id_DiskState) {
							case ID_VALIDATING:
								strcpy(disktxt[8],globstring[STR_VALIDATING]);
								break;
							case ID_WRITE_PROTECTED:
								strcpy(disktxt[8],globstring[STR_WRITE_PROTECTED]);
								break;
							default:
								strcpy(disktxt[8],globstring[STR_READ_WRITE]);
								break;
						}
						seedate(&ds,disktxt[9],0);
						disktxt[10][0]=(char)((infodata.id_DiskType>>24)&0xff);
						disktxt[10][1]=(char)((infodata.id_DiskType>>16)&0xff);
						disktxt[10][2]=(char)((infodata.id_DiskType>>8)&0xff);
						disktxt[10][3]=(char)(infodata.id_DiskType&0xff)+'0';
						disktxt[10][4]=0;
						switch (infodata.id_DiskType) {
							case ID_DOS_DISK:
								strcat(disktxt[10]," (OFS)");
								break;
							case ID_FFS_DISK:
								strcat(disktxt[10]," (FFS)");
								break;
							case ID_INTER_DOS_DISK:
								strcat(disktxt[10]," (I-OFS)");
								break;
							case ID_INTER_FFS_DISK:
								strcat(disktxt[10]," (I-FFS)");
								break;
							case ID_FASTDIR_DOS_DISK:
								strcat(disktxt[10]," (DC-OFS)");
								break;
							case ID_FASTDIR_FFS_DISK:
								strcat(disktxt[10]," (DC-FFS)");
								break;
							case ID_MSDOS_DISK:
								strcat(disktxt[10]," (MS-DOS)");
								break;
						}
						break;
				}
			}
			else {
				a=IoErr();
				doerror(a);
				switch (a) {
					case ERROR_DEVICE_NOT_MOUNTED:
						strcpy(disktxt[1],globstring[STR_DEVICE_NOT_MOUNTED]);
						break;
					case ERROR_NOT_A_DOS_DISK:
						strcpy(disktxt[1],globstring[STR_NOT_A_DOS_DISK]);
						break;
					case ERROR_NO_DISK:
						strcpy(disktxt[1],globstring[STR_NO_DISK_IN_DRIVE]);
						break;
					default:
						lsprintf(disktxt[1],globstring[STR_DOS_ERROR_CODE],a);
						break;
				}
			}
		}
		else fail=1;
	}
	else fail=1;

	if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
	if (fail) {
		doerror(IoErr());
		return;
	}

	b=0; titwidth=0;
	for (a=0;a<ITEM_NUM;a++) {
		if ((c=strlen(disktxt[a]))>b) b=c;
		if ((c=strlen(globstring[diskinfo_heads[a]]))>titwidth) titwidth=c;
	}

	disk_win.Width=((b+titwidth+3)*cx)+16;
	disk_win.Height=(ITEM_NUM*(cy+1))+19;
	if (config->generalscreenflags&SCR_GENERAL_REQDRAG) {
		disk_win.Flags=WFLG_RMBTRAP|WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_DEPTHGADGET;
		xoffset=Window->WScreen->WBorLeft;
		yoffset=Window->WScreen->WBorTop+Window->WScreen->Font->ta_YSize+1;
		disk_win.Width+=xoffset+Window->WScreen->WBorRight;
		disk_win.Height+=yoffset+Window->WScreen->WBorBottom;
		disk_win.Title="DiskInfo";
	}
	else {
		disk_win.Flags=WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_ACTIVATE;
		xoffset=2;
		yoffset=1;
		disk_win.Width+=4;
		disk_win.Height+=2;
		disk_win.Title=NULL;
	}
	centerwindow(&disk_win);

	contgad.NextGadget=NULL;
	contgad.Width=(strlen(globstring[STR_CONTINUE])+4)*cx;
	contgad.Height=cy+5;
	contgad.LeftEdge=(disk_win.Width-contgad.Width)/2;
	contgad.TopEdge=disk_win.Height-cy-10;
	contgad.Flags=GFLG_GADGHCOMP;
	contgad.Activation=GACT_RELVERIFY;
	contgad.GadgetType=GTYP_BOOLGADGET;
	contgad.GadgetRender=NULL;
	contgad.SelectRender=NULL;
	contgad.GadgetText=NULL;
	contgad.MutualExclude=0;
	contgad.SpecialInfo=NULL;
	contgad.GadgetID=0;

	StrCombine(buf,"_",globstring[STR_CONTINUE],256);
	gad_gads[0]=buf;
	gad_gads[1]=NULL;

	if (!(fontwindow=OpenWindow(&disk_win))) return;

	dirp=fontwindow->RPort;
	setupwindreq(fontwindow);
	SetFont(dirp,scr_font[FONT_REQUEST]);
	for (a=0;a<ITEM_NUM;a++) {
		Move(dirp,xoffset+8,yoffset+5+(a*cy)+scr_font[FONT_REQUEST]->tf_Baseline);
		Text(dirp,globstring[diskinfo_heads[a]],strlen(globstring[diskinfo_heads[a]]));
		Move(dirp,xoffset+8+(titwidth*cx),yoffset+5+(a*cy)+scr_font[FONT_REQUEST]->tf_Baseline);
		Text(dirp," : ",3);
		Text(dirp,disktxt[a],strlen(disktxt[a]));
	}
	AddGadgetBorders(&key,
		&contgad,1,
		screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
	AddGadgets(fontwindow,
		&contgad,
		gad_gads,
		1,
		screen_pens[config->gadgettopcol].pen,
		screen_pens[config->gadgetbotcol].pen,1);
	a=0;

	FOREVER {
		while (IMsg=(struct IntuiMessage *)GetMsg(fontwindow->UserPort)) {
			class=IMsg->Class; code=IMsg->Code;
			ReplyMsg((struct Message *)IMsg);
			switch (class) {
				case IDCMP_DISKREMOVED:
					setupchangestate();
					break;
				case IDCMP_DISKINSERTED:
					if (isd!=-1) {
						old_change_state=disk_change_state;
						setupchangestate();
						if ((old_change_state&(1<<isd))!=(disk_change_state&(1<<isd))) {
							a=2;
							break;
						}
					}
					break;
				case IDCMP_VANILLAKEY:
					if (code!='\r' && LToLower(code)!=LToLower(globstring[STR_CONTINUE][0]))
						break;
					SelectGadget(fontwindow,&contgad);
				case IDCMP_GADGETUP:
					a=1;
					break;
			}
		}
		if (a) break;
		Wait(1<<fontwindow->UserPort->mp_SigBit);
	}
	CloseWindow(fontwindow);
	fontwindow=NULL;
	LFreeRemember(&key);
	if (a==2) goto getnewdisk;
#endif
}

void get_device_task(lock,buffer,port)
BPTR lock;
char *buffer;
struct MsgPort *port;
{

/* AROS: Need to rewrite this because of private sturctures */
#if 0
	struct DeviceList *devlist,*dl;
	struct RootNode *rootnode;
	struct DosInfo *dosinfo;
	struct FileLock *lock2;

	rootnode=(struct RootNode *) DOSBase->dl_Root;
	dosinfo=(struct DosInfo *) BADDR(rootnode->rn_Info);
	devlist=(struct DeviceList *) BADDR(dosinfo->di_DevInfo);
	lock2=(struct FileLock *)BADDR(lock);
	dl=(struct DeviceList *)BADDR(lock2->fl_Volume);

	while (devlist) {
		if (devlist->dl_Type==DLT_DEVICE && devlist->dl_Task==dl->dl_Task) break;
		devlist=(struct DeviceList *) BADDR(devlist->dl_Next);
	}
	if (devlist) BtoCStr((BPTR)devlist->dl_Name,buffer,31);
	else strcpy(buffer,((struct Task *)port->mp_SigTask)->tc_Node.ln_Name);
	strcat(buffer,":");

#else
#warning Implement this
kprintf("!!!!  get_device_task() NOT IMPLEMENTED\n");

	strcpy(buffer, "UNIMPLEMENTED:");

#endif
}

void getsizestring(buf,a)
char *buf;
ULONG a;
{
	a/=1024;
	if (a>1073741824) lsprintf(buf,"HUGE");
	else if (a>1048576) {
		getfloatstr((double)((double)a/1048576),buf);
		strcat(buf,"G");
	}
	else if (a>1024) {
		getfloatstr((double)((double)a/1024),buf);
		strcat(buf,"M");
	}
	else lsprintf(buf,"%ldK",a);
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
	lsprintf(buf1,"%ld",c); buf1[1]=0;
	lsprintf(buf,"%ld.%s",a,buf1);
}
