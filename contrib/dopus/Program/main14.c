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

void doassign(rexx)
int rexx;
{
	int a,blank=0,tot;
	char buf[32],buf1[256];
	struct Directory *dev,*next;

	if (status_iconified) return;

	buf[0]=0;
	if (checkdevtot(dopus_curwin[data_active_window])) {
		dev=dopus_curwin[data_active_window]->firstentry;
		while (dev) {
			next=dev->next;
			if (dev->selected && dev->type==ENTRY_DEVICE && dev->size==DLT_DIRECTORY) {
				tot=0;
				if (!blank) {
					expand_path(dev->name,buf1);
					if ((!whatsit(globstring[STR_ENTER_ASSIGN_NAME],256,buf1,(char *)-1))) {
						myabort();
						return;
					}
					if (buf1[0]==0) blank=1;
				}
				strcpy(buf,dev->name); buf[(strlen(buf)-1)]=0;
				if (blank) a=Assign(buf,NULL);
				else a=Assign(buf,buf1);
				if (a) dostatustext(globstring[STR_CANT_CANCEL_ASSIGN]);
				else {
					tot=1;
					if (blank) removefile(dev,dopus_curwin[data_active_window],data_active_window,TRUE);
					else unselect(data_active_window,dev);
				}
			}
			dev=next;
		}
		if (tot) okay();
		return;
	}
	if (str_pathbuffer[data_active_window][0]==0) return;
	if (!rexx) {
		if ((!whatsit(globstring[STR_ENTER_DEVICE_NAME],31,buf,NULL))) {
			myabort();
			return;
		}
	}
	else strcpy(buf,rexx_args[0]);
	for (a=0;a<strlen(buf);a++)
		if (buf[a]==':' || buf[a]=='/') buf[a]=0;
	if (!(Assign(buf,str_pathbuffer[data_active_window]))) okay();
	else dostatustext(globstring[STR_ASSIGN_FAILED]);
}

int checkdest(w)
int w;
{
	if (!status_iconified && w>-1 && str_pathbuffer[w][0]==0) {
		dostatustext(globstring[STR_NO_DESTINATION_SELECTED]);
		simplerequest(globstring[STR_NO_DESTINATION_SELECTED],
			globstring[STR_CONTINUE],NULL);
		return(0);
	}
	return(1);
}

int checksame(src,dst,type)
char *src,*dst;
int type;
{
	BPTR lk1,lk2;
	int ret,a;

	if (!(lk1=Lock(src,ACCESS_READ))) return(1);
	if (!(lk2=Lock(dst,ACCESS_READ))) {
		UnLock(lk1);
		return(1);
	}
	if ((ret=(CompareLock(lk1,lk2)))==LOCK_SAME) {
		if (type==0) a=STR_CANT_COPY_DIR_TO_ITSELF;
		else a=STR_CANT_OVERCOPY_FILES;
		dostatustext(globstring[a]);
		simplerequest(globstring[a],globstring[STR_CONTINUE],NULL);
		ret=0;
	}
	else ret=1;
	UnLock(lk1);
	UnLock(lk2);
	return(ret);
}

int expand_path(path,buffer)
char *path,*buffer;
{
	BPTR lock;
	APTR save;
	int suc;

	save=main_proc->pr_WindowPtr;
	main_proc->pr_WindowPtr=(APTR)-1;
	buffer[0]=0;
	if ((lock=Lock(path,ACCESS_READ))) {
		PathName(lock,buffer,256);
		UnLock(lock);
		suc=1;
	}
	else suc=0;
	main_proc->pr_WindowPtr=save;
	return(suc);
}
