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
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dosextens.h>

#include <stdio.h>
#include <string.h>
#include "fmnode.h"
#include "fmlocale.h"

extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;

WORD infodata(UBYTE*,UBYTE*,WORD);
void drives(struct FMList*);

void clearlisti(struct List*);
void shellsortlist(struct FMList*,struct FMNode*,struct Library*,WORD,WORD,WORD);

extern struct Window *ikkuna;
extern struct Gadget string1;
extern struct Gadget string2;
extern UBYTE buffer1[];
extern UBYTE buffer2[];
extern struct FMMain fmmain;
extern struct FMList fmlist[];

void getkeybytes(void); /* HC */

UBYTE* siirrabstr(BSTR ptr1,UBYTE *ptr2)
{

  UBYTE *tmpp;
    int i, l = AROS_BSTR_strlen(ptr1);
 
    tmpp = ptr2;
 
    for(i = 0; i < l; i++)
        *tmpp++ = AROS_BSTR_getchar(ptr1, i);
   return tmpp;

/*
UBYTE cnt;
cnt=*ptr1++;
while(cnt--) *ptr2++=*ptr1++;
*ptr2=0;
return ptr2;
*/
}

struct FMNode* adddevice(struct FMList *list,struct DosList *doslist,WORD type)
{
struct FMNode *node;
UBYTE empty[80];
UBYTE *ptr;
struct FMNode *ret=0;

if(doslist->dol_Type!=type) return(0);


#ifndef AROS
if(type==DLT_DEVICE && !doslist->dol_Task) return(0);
#endif
ptr=siirrabstr(doslist->dol_Name,empty);  

*ptr++=':'; *ptr=0;


D(bug("drives.c 80 entering.........\n"));  



if ((node=allocnode())) {
	node->flags=NASSIGN;
	strcpy(NDFILE(node),empty);
	AddTail((struct List*)list,(struct Node*)node);
	ret=node;
}
return(ret);
}

void drives(struct FMList *list)
{
struct Process *proc;
struct DosList *doslist;
APTR ooldwinptr;
struct FMNode *node,*node2;
struct FileInfoBlock *fib;
BPTR lock;
UBYTE *ptr;

D(bug("drives.c 101 entering.........\n"));   
proc=(struct Process*)FindTask(0);
D(bug("drives.c 103 entering.........\n")); 
clearlist(list); list->flags&=~LDIRLIST; list->flags|=LDEVLIST;
if (list->li) {
	list->path[0]=0;
	RefreshGList(&list->li->string,fmmain.ikkuna,0,1);
}
D(bug("drives.c 109 entering.........\n")); 

fib=allocvec(list,sizeof(struct FileInfoBlock),MEMF_CLEAR);
if(!fib) return;
ooldwinptr=proc->pr_WindowPtr;
proc->pr_WindowPtr=(APTR)-1;
doslist=LockDosList(LDF_DEVICES|LDF_READ);
D(bug("drives.c 116 entering.........\n")); 
while ((doslist=NextDosEntry(doslist,LDF_DEVICES))) adddevice(list,doslist,DLT_DEVICE);
UnLockDosList(LDF_DEVICES|LDF_READ);
D(bug("drives.c 120 entering.........\n")); 
node=list->head;
D(bug("drives.c 122 entering.........\n")); 
while(node->succ) {
	if(node->flags==NASSIGN) {
		if(IsFileSystem(NDFILE(node))) {
			if ((lock=Lock(NDFILE(node),SHARED_LOCK))) {
				if (Examine(lock,fib)) {
					ptr=NDCOMM(node);
					strcpy(ptr,fib->fib_FileName);
					ptr+=strlen(ptr);
					*ptr++=':'; *ptr=0;
					infodata(NDFILE(node),NDCOMM(node)+fmmain.commlen/2,0);
				}
				UnLock(lock);
			}
			node->flags=NDEVICE;
			node=node->succ;
		} else {
			node2=node;
			node=node->succ;
			Remove((struct Node*)node2);
			freevecpooled(node2);
		}
	} else {
		node=node->succ;
	}
}
freemem(fib);
shellsortlist(list,list->head->succ,(struct Library *)UtilityBase,fmmain.fileoffset,0,0);

doslist=LockDosList(LDF_ASSIGNS|LDF_READ);
while ((doslist=NextDosEntry(doslist,LDF_ASSIGNS))) adddevice(list,doslist,DLT_DIRECTORY);
UnLockDosList(LDF_ASSIGNS|LDF_READ);
node=list->head->succ;
while(node->flags&NDEVICE) node=node->succ;
shellsortlist(list,node,(struct Library *)UtilityBase,fmmain.fileoffset,0,0);

clearlisti((struct List*)&fmmain.dlist);
duplist(list,&fmmain.dlist);
proc->pr_WindowPtr=ooldwinptr;
list->fmmessage1[0]=0;
list->fmmessage2[0]=0;
fmmessage(list);
list->firstlinew=0;
parselist(list,0);
outputlist(list);
updadirmeter(list);

}

void bdrives(struct FMList *list)
{
struct FMList *dlist;

dlist=&fmmain.dlist;
if(dlist->head->succ) {
	clearlist(list);
	list->flags&=~LDIRLIST;
	list->flags|=LDEVLIST;
	if (list->li) {
		list->path[0]=0;
		RefreshGList(&list->li->string,fmmain.ikkuna,0,1);
	}
	duplist(dlist,list);
	list->fmmessage1[0]=0;
	list->fmmessage2[0]=0;
	fmmessage(list);
	parselist(list,0);
	outputlist(list);
	updadirmeter(list);
	return;
} else {
	drives(list);
}
}
