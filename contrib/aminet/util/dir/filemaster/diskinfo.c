/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
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
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <aros/debug.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <stdio.h>
#include <string.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct DosLibrary *DOSBase;

WORD finddevice(BPTR lock,struct DosList**,struct DosList**);

void __saveds diskinfo(void)
{
struct ProcMsg *pm;
BPTR lock=0;
struct DosList *doslist;
struct DosList *vollist;
struct FMList *list;
struct FMNode *node;
struct FileSysStartupMsg *fssm;
struct InfoData *id;
struct DosEnvec *de;
UBYTE text[1000];
UBYTE *ptr1,*ptr2;
UBYTE volname[32];
UBYTE devname[32];
UBYTE disktype[32];
ULONG dtype;
UBYTE cdate[32];
ULONG tsecs;
WORD apu1,apu2,apu3;

extern struct FMMain fmmain;

pm=sinitproc();
priority(pm->cmc);
list=fmmain.sourcedir;
fmmain.wincnt++;
if (!(setalloc(list,1))) {
	initproc(0,0);
	goto endi;
}
initproc(list,pm->cmc->label);

if (!(id=allocmem(sizeof(struct InfoData)))) goto diend;
ptr2=list->path;
if (!(list->flags&LDIRLIST)) {
	node=findselnodeall(list);
	if (node) {
		ptr2=NDFILE(node);
		node->flags&=~NSELECTED;
		outputlistline(list,node);
	} else {
		strcpymsg(list->fmmessage1,MSG_MAIN_NOSOURCEFILE);
		fmmessage(list);
		goto diend;
	}
}
if ((lock=fmlock(list,ptr2))) {
	if (Info(lock,id)) {
		doslist=LockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);
		if(finddevice(lock,&doslist,&vollist)) {

			fssm=(struct FileSysStartupMsg*)BADDR(doslist->dol_misc.dol_handler.dol_Startup);
			de=(struct DosEnvec*)BADDR(fssm->fssm_Environ); 

			if (fssm) {
				switch(id->id_DiskState)
				{
				case ID_WRITE_PROTECTED:
				ptr1=getstring(MSG_DISKINFO_WPROT);
				break;
				case ID_VALIDATING:
				ptr1=getstring(MSG_DISKINFO_VALIDATING);
				break;
				case ID_VALIDATED:
				ptr1=getstring(MSG_DISKINFO_VALIDATED);
				break;
				default:
				ptr1=getstring(MSG_DISKINFO_UNKNOWN);
				break;
				}
				siirrabstr(vollist->dol_Name,volname);
				siirrabstr(doslist->dol_Name,devname);   
 
				tsecs=de->de_BlocksPerTrack;
				longtodatestring(cdate,dstolong(&vollist->dol_misc.dol_volume.dol_VolumeDate));
				dtype=de->de_DosType;
				ptr2=disktype;
				apu2=24;
				for(apu1=0;apu1<4;apu1++) {
					apu3=(dtype>>apu2)&0xff;
					if(apu3>=32) {
						*ptr2++=apu3;
					} else {
						sformat(ptr2,"\\\%ld",apu3);
						ptr2+=strlen(ptr2);
					}
					apu2-=8;
				}
				*ptr2=0;
sformatmsg(text,MSG_DISKINFO,id->id_NumSoftErrors,fssm->fssm_Unit,ptr1,id->id_NumBlocks,id->id_NumBlocksUsed,id->
id_BytesPerBlock,dtype,disktype,volname,devname,cdate,BADDR(fssm->fssm_Device)+1,de->de_Surfaces,tsecs,de->de_Reserved+de->
de_PreAlloc,de->de_LowCyl,de->de_HighCyl,de->de_NumBuffers,de->de_BufMemType,de->de_MaxTransfer,de->de_Mask);

				UnLockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);
				endproc(list);	
				reqinfowindow(pm->cmc->label,text,guiLEFT,MSG_OK,1,0);
				goto diend;
				}
		}
		UnLockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);
	}
}
diend:
if (lock) UnLock(lock);
freemem(id);
endproc(list);
endi:
fmmain.wincnt--;
deinitproc(pm);
}


WORD finddevice(BPTR lock,struct DosList **dl,struct DosList **vl)
{
struct DosList *doslist;
struct DosList *vollist;

doslist=*dl;
#ifndef AROS
vollist=(struct DosList*)(BADDR(((struct FileLock*)BADDR(lock))->fl_Volume));
while(doslist=NextDosEntry(doslist,LDF_DEVICES)) {
	if (doslist->dol_Task==vollist->dol_Task) {
		*dl=doslist;
		*vl=vollist;
		return(1);
	}
}
#else
D(bug("diskinfo.c 188...........\n"));
vollist=(struct DosList*)((((struct DeviceList*)(lock))->dl_Lock));   
D(bug("diskinfo.c 201...........\n"));

while((doslist=NextDosEntry(doslist,LDF_DEVICES))) {
	 //if (doslist->dol_Task==vollist->dol_Task) {
                *dl=doslist;
                *vl=vollist;
                //return(1);
//}

D(bug("diskinfo.c 196...........\n"));
}
#endif

return(0);
}
