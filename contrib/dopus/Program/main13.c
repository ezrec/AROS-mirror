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

#include <exec/alerts.h>

void seedate(ds,date,pad)
struct DateStamp *ds;
char *date;
int pad;
{
	char timebuf[16],datebuf[16],buf[40];
	struct DOpusDateTime dt;

	copy_datestamp(ds,&dt.dat_Stamp);
	initdatetime(&dt,datebuf,timebuf,1);

	strncpy(buf,datebuf,9);
	buf[9]=0;
	strcat(buf," ");
	strcat(buf,timebuf);
	buf[19]=0;

	if (pad) {
		if (config->dateformat&DATE_12HOUR) lsprintf(date,"%19s",buf);
		else lsprintf(date,"%18s",buf);
	}
	else strcpy(date,buf);
}

int setdate(name,date)
char *name;
struct DateStamp *date;
{
	struct MsgPort *handler;
	ULONG arg[4];
	BPTR lock,parentlock;
	APTR wsave;
	int rc=0;

#warning Look closer at this __aligned below
	char /* __aligned */ bstrname[40];

	wsave=main_proc->pr_WindowPtr;
	main_proc->pr_WindowPtr=(APTR)-1;

	if ((handler=(struct MsgPort *)DeviceProc(name))) {
		if ((lock=Lock(name,ACCESS_READ))) {
			parentlock=ParentDir(lock);
			UnLock(lock);

			strcpy(&bstrname[1],BaseName(name));
			bstrname[0]=strlen(&bstrname[1]);

			arg[0]=0;
			arg[1]=(ULONG)parentlock;
			arg[2]=(ULONG)bstrname>>2;
			arg[3]=(ULONG)date;
			rc=SendPacket(handler,ACTION_SET_DATE,arg,4);
			UnLock(parentlock);
		}
	}

	main_proc->pr_WindowPtr=wsave;
	return(rc?1:IoErr());
}

void seename(win)
int win;
{
	int tot,bl,a;
	static char buf[256];

	if (win<0 || status_iconified) return;
	if (!dopus_curwin[win]->firstentry ||
		(dopus_curwin[win]->firstentry->type!=ENTRY_DEVICE &&
		dopus_curwin[win]->firstentry->type!=ENTRY_CUSTOM)) {
		if (str_pathbuffer[win][0]==0) {
			if (win==data_active_window)
				SetAPen(main_rp,screen_pens[(int)config->disknameselbg].pen);
			else SetAPen(main_rp,screen_pens[(int)config->disknamebg].pen);
			rectfill(main_rp,
				scrdata_diskname_xpos[win]+2,scrdata_diskname_ypos,
				scrdata_diskname_width[win],scrdata_diskname_height-2);
			SetAPen(main_rp,screen_pens[1].pen);
			return;
		}
		else {
			strcpy(buf,str_pathbuffer[win]);
			dopus_curwin[win]->diskname[0]=0;
			main_proc->pr_WindowPtr=(APTR)-1;
			if (!(a=getroot(buf,NULL))) {
				strcpy(dopus_curwin[win]->diskname,globstring[STR_DIR_NOT_AVAILABLE_TITLE]);
				dopus_curwin[win]->disktot=dopus_curwin[win]->diskfree=dopus_curwin[win]->diskblock=-1;
				displayname(win,1);
				if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
				return;
			}
			strcpy(dopus_curwin[win]->diskname,buf);
/* */
			strcpy(dopus_curwin[win]->volumename,buf);
/* */
			dopus_curwin[win]->diskfree=bytes(str_pathbuffer[win],&tot,&bl);
			dopus_curwin[win]->disktot=tot;
			dopus_curwin[win]->diskblock=bl;
			if (a==ID_WRITE_PROTECTED) dopus_curwin[win]->flags|=DWF_READONLY;
			else dopus_curwin[win]->flags&=~DWF_READONLY;
			if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
		}
	}
	displayname(win,0);
}

void displayname(win,clear)
int win,clear;
{
	int free,b,tot,x,nn=0,len,len2,len3,x1,cx1,cx2;
	static char buf[30],buf2[80],buf3[20];
	double pct;

	if (status_iconified || win<0) return;
	if (str_pathbuffer[win][0]==0) {
		if (dopus_curwin[win]->disktot!=-1 &&
			(!dopus_curwin[win]->firstentry || dopus_curwin[win]->firstentry->type!=ENTRY_DEVICE)) nn=2;
		else nn=1;
	}
	if (nn!=2) {
		free=dopus_curwin[win]->diskfree; tot=dopus_curwin[win]->disktot;
		SetDrMd(main_rp,JAM2);
		StrCombine(buf2,dopus_curwin[win]->diskname,str_space_string,32);
		SetFont(main_rp,scr_font[FONT_NAMES]);

		len=30;
		x1=dotextlength(main_rp,buf2,&len,scrdata_diskname_width[win]-4);

		if (!nn && tot>-1) {
			if (config->showfree&SHOWFREE_BYTES || config->showfree==0) {
				if (dopus_curwin[win]->flags&DWF_READONLY)
					lsprintf(buf," (%ld)",free);
				else lsprintf(buf," %ld",free);
			}
			else if (config->showfree&SHOWFREE_KILO) {
				getsizestring(buf,free);
				if (dopus_curwin[win]->flags&DWF_READONLY) {
					char buf1[30];

					strcpy(buf1,"(");
					strcat(buf1,buf);
					strcat(buf1,")");
					strcpy(buf,buf1);
				}
			}
			else if (config->showfree&SHOWFREE_BLOCKS) {
				if (dopus_curwin[win]->flags&DWF_READONLY)
					lsprintf(buf," (%ld)",dopus_curwin[win]->diskblock);
				else lsprintf(buf," %ld",dopus_curwin[win]->diskblock);
			}
			else if (config->showfree&SHOWFREE_PERCENT) {
				if (tot==0) b=100;
				else {
					pct=free; pct*=100;
					b=(int)(pct/(double)tot);
				}
				if (b>100) b=100;
				if (dopus_curwin[win]->flags&DWF_READONLY)
					lsprintf(buf," (%ld%%)",b);
				else lsprintf(buf," %ld%%",b);
			}
			StrCombine(buf3,buf,str_space_string,14);
			len2=12; len3=strlen(buf);
			FOREVER {
				x=dotextlength(main_rp,buf3,&len2,scrdata_diskname_width[win]-x1-4);
				if (len2>=len3 || (--len)==0) break;
				len2=12; x1=dotextlength(main_rp,buf2,&len,scrdata_diskname_width[win]-4);
			}
			strncpy(buf3,str_space_string,13);
			strcpy(&buf3[len2-len3],buf);
		}
		else x=0;
	}

	if (clear) {
		if (win==data_active_window)
			SetAPen(main_rp,screen_pens[(int)config->disknameselbg].pen);
		else SetAPen(main_rp,screen_pens[(int)config->disknamebg].pen);
		rectfill(main_rp,
			scrdata_diskname_xpos[win]+2,
			scrdata_diskname_ypos,
			scrdata_diskname_width[win],
			scrdata_diskname_height-2);
	}

	if (nn!=2) {
		if (win==data_active_window) {
			SetAPen(main_rp,screen_pens[(int)config->disknameselfg].pen);
			SetBPen(main_rp,screen_pens[(int)config->disknameselbg].pen);
		}
		else {
			SetAPen(main_rp,screen_pens[(int)config->disknamefg].pen);
			SetBPen(main_rp,screen_pens[(int)config->disknamebg].pen);
		}

		Move(main_rp,
			scrdata_diskname_xpos[win]+4,
			scrdata_diskname_ypos+scr_font[FONT_NAMES]->tf_Baseline);
		Text(main_rp,buf2,len);

		cx1=main_rp->cp_x;
		if (x) {
			x1=(scrdata_diskname_xpos[win]+scrdata_diskname_width[win])-x;
			if (x1<scrdata_diskname_xpos[win]+2) x1=scrdata_diskname_xpos[win]+2;
			cx2=x1-1;
			Move(main_rp,x1,scrdata_diskname_ypos+scr_font[FONT_NAMES]->tf_Baseline);
			Text(main_rp,buf3,len2);
		}
		else cx2=scrdata_diskname_xpos[win]+scrdata_diskname_width[win]+1;
		if (!clear && cx1<=cx2) {
			SetAPen(main_rp,main_rp->BgPen);
			RectFill(main_rp,cx1,scrdata_diskname_ypos,cx2,scrdata_diskname_height+scrdata_diskname_ypos-3);
		}
		SetFont(main_rp,scr_font[FONT_GENERAL]);
	}
}

void relabel_disk(rexx,path)
int rexx;
char *path;
{
	struct MsgPort *port;
	char oldname[33],name[33],*bstr;
	ULONG arg;
	char buf[256];
	int a;

	if (rexx) strcpy(buf,rexx_args[0]);
	else strcpy(buf,path);

	if (!(getroot(buf,NULL))) {
		doerror(IoErr());
		return;
	}
	strcat(buf,":");
	if (!(port=(struct MsgPort *) DeviceProc(buf))) return;
	getroot(buf,NULL);

	strcpy(name,buf); strcpy(oldname,name);
	if (!rexx) {
		if (!(whatsit(globstring[STR_ENTER_NEW_DISK_NAME],30,name,NULL))) return;
	}
	else strcpy(name,rexx_args[1]);
	a=strlen(name);
	bstr=(char *) AllocMem(a+2,MEMF_CLEAR);
	bstr[0]=(char)a;
	strcpy(bstr+1,name);
	arg=(ULONG)bstr>>2;
	if (!(SendPacket(port,ACTION_RENAME_DISK,&arg,1))) doerror(IoErr());
	else if (!status_iconified) {
		if ((LStrnCmpI(str_pathbuffer[data_active_window],oldname,strlen(oldname)))==0 &&
			str_pathbuffer[data_active_window][strlen(oldname)]==':') {
			strcpy(buf,name);
			StrConcat(buf,&str_pathbuffer[data_active_window][strlen(oldname)],256);
			strcpy(str_pathbuffer[data_active_window],buf);
			checkdir(str_pathbuffer[data_active_window],&path_strgadget[data_active_window]);
			strcpy(dopus_curwin[data_active_window]->directory,str_pathbuffer[data_active_window]);
		}
		seename(data_active_window);
	}
	FreeMem(bstr,a+2);
}

int getroot(name,ds)
char *name;
struct DateStamp *ds;
{

#if 1
#warning Implement this properly

	BPTR lock;
	struct DosList *dl;
	
	char buf[MAXFILENAMELENGTH];
	
kprintf("!!!! getroot(%s)\n", name);

	lock = Lock(name, ACCESS_READ);
	if (NULL == lock) {
		kprintf("Could not get lock in getroot()\n");
		return 0;
	}
	
	/* Get the volume name */
	
	/* Get lock on root device */
	
#warning AROS ParentDir function is very buggy and causes an infinite loop here
	
/*	SetIoErr(0);
	while ((lock =  ParentDir(lock))) {
		kprintf("ParentDir() called, lock=%p\n", lock);
		if (0 != IoErr()) {
			kprintf("Got error from ParentDir() in getroot()\n");
			return 0;
	
		}
	
	}
*/		
	
	/* Get the name from lock */
	if (0 == NameFromLock(lock, buf, MAXFILENAMELENGTH)) {
		kprintf("Could not get lock for name\n");
		return 0;
	}
	
kprintf("Name from lock %s\n", buf);
	
	
	dl = LockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_READ);
	dl = FindDosEntry(dl, buf, LDF_DEVICES |  LDF_VOLUMES);
	
	if (NULL == dl) {
	
		UnLockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_READ);
		kprintf("Could not get device list\n");
		return 0;
	}
	
	/* Get the device name */
	
	if (dl->dol_DevName) {
kprintf("Testing whether %s is a filesystem\n", dl->dol_DevName);
		if (!IsFileSystem(dl->dol_DevName)) {
			kprintf("!!!! NO FILESYSTEM IN getroot()\n");
			Alert(AT_DeadEnd);
		}
		strcpy(name, dl->dol_DevName);
	}
	
	/* Get the volume date */
	if (ds) {
		CopyMem(&dl->dol_misc.dol_volume.dol_VolumeDate, ds, sizeof (*ds));
	}

	UnLockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_READ);
	
	
#warning ID_VALIDATED is not allways the case

kprintf("getroot(): Name returned: %s\n", name);
	return ID_VALIDATED;

#else
	BPTR lock1;
	struct FileLock *lock2;
	struct InfoData __aligned info;
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
	Info(lock1,&info);
	UnLock(lock1);
	return(info.id_DiskState);
#endif
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

void strtostamp(date,time,ds)
char *date,*time;
struct DateStamp *ds;
{
	struct DOpusDateTime datetime;

	datetime.dat_Format=dateformat(config->dateformat);
	datetime.dat_Flags=DDTF_SUBST|DDTF_CUSTOM;
	datetime.dat_StrDate=date;
	datetime.dat_StrTime=time;
	datetime.custom_months=date_months;
	datetime.custom_shortmonths=date_shortmonths;
	datetime.custom_weekdays=date_weekdays;
	datetime.custom_shortweekdays=date_shortweekdays;
	datetime.custom_special_days=date_special;
	StrToStamp(&datetime);
	copy_datestamp(&datetime.dat_Stamp,ds);
}
