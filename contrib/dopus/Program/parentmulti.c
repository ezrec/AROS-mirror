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

enum {
	PM_CANCEL,
	PM_OKAY
};

struct RequesterBase pm_req={
	50,12,32,19,
	8,6,
	0,0,0,0,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
	0,0,NULL,0,NULL};

struct TagItem
	pm_lister[]={
		{RO_Type,OBJECT_LISTVIEW},
		{RO_ListViewID,0},
		{RO_Top,1},
		{RO_Width,50},
		{RO_Height,10},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	pm_okay_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PM_OKAY},
		{RO_Top,11},
		{RO_TopFine,5},
		{RO_Left,4},
		{RO_LeftFine,-2},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_TextNum,STR_OKAY},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	pm_cancel_gadget[]={
		{RO_Type,OBJECT_GADGET},
		{RO_GadgetType,GADGET_BOOLEAN},
		{RO_GadgetID,PM_CANCEL},
		{RO_Top,11},
		{RO_TopFine,5},
		{RO_Width,12},
		{RO_Height,1},
		{RO_HeightFine,4},
		{RO_Left,34},
		{RO_LeftFine,18},
		{RO_TextNum,STR_CANCEL},
		{RO_TextPos,TEXTPOS_CENTER},
		{RO_HighRecess,TRUE},
		{TAG_END,0}},

	*pm_gadgets[]={
		pm_okay_gadget,
		pm_cancel_gadget,
		NULL};

int do_parent_multi(path)
char *path;
{
	ULONG class;
	USHORT gadgetid;
	struct Window *rwindow;
	struct Gadget *gadlist;
	struct DOpusListView *listview,*view;
	BPTR lock,parentlock,temp;
	char buf[256],**table;
	int a,count,lastsel=-1;
	ULONG lastseconds=0,lastmicros=0,seconds,micros;

	if (!(lock=Lock(path,ACCESS_READ))) return(0);

	fix_requester(&pm_req,globstring[STR_SELECT_A_DIRECTORY]);

	if (!(rwindow=OpenRequester(&pm_req)) ||
		!(gadlist=addreqgadgets(&pm_req,pm_gadgets,0,NULL)) ||
		!(listview=(struct DOpusListView *)
			AddRequesterObject(&pm_req,pm_lister))) {
		CloseRequester(&pm_req);
		UnLock(lock);
		return(0);
	}

	listview->window=rwindow;
	listview->flags|=DLVF_LEAVE|DLVF_SLOW|DLVF_TTOP;
	listview->sliderwidth=8;
	listview->slidercol=pm_req.rb_fg;
	listview->sliderbgcol=pm_req.rb_bg;
	listview->textcol=pm_req.rb_fg;
	listview->boxhi=pm_req.rb_shine;
	listview->boxlo=pm_req.rb_shadow;
	listview->arrowfg=pm_req.rb_fg;
	listview->arrowbg=pm_req.rb_bg;
	listview->itemfg=pm_req.rb_fg;
	listview->itembg=pm_req.rb_bg;
	listview->title=globstring[STR_PARENT_MULTI];

	SetBusyPointer(rwindow);

	parentlock=DupLock(lock);
	count=0;
	while (parentlock) {
		++count;
		count+=get_multi_volume(parentlock,NULL,NULL);
		temp=parentlock;
		parentlock=ParentDir(parentlock);		
		UnLock(temp);
	}

	if (!(table=LAllocRemember(&pm_req.rb_memory,(count+1)*sizeof(char *),MEMF_CLEAR))) {
		CloseRequester(&pm_req);
		return(0);
	}

	parentlock=DupLock(lock);
	count=0;
	while (parentlock) {
		PathName(parentlock,buf,256);
		if ((table[count]=LAllocRemember(&pm_req.rb_memory,(a=(strlen(buf)+3)),0))) {
			strcpy(table[count],buf);
			TackOn(table[count],NULL,a-1);
			++count;
		}
		count+=get_multi_volume(parentlock,&table[count],&pm_req.rb_memory);
		temp=parentlock;
		parentlock=ParentDir(parentlock);		
		UnLock(temp);
	}

	UnLock(lock);

	listview->items=table;

	if (!(AddListView(listview,1))) {
		CloseRequester(&pm_req);
		return(0);
	}

	RefreshRequesterObject(&pm_req,NULL);

	ClearPointer(rwindow);

	if (table[0]) strcpy(buf,table[0]);

	FOREVER {
		while ((IMsg=(struct IntuiMessage *)GetMsg(rwindow->UserPort))) {
			seconds=IMsg->Seconds;
			micros=IMsg->Micros;
			if ((view=(struct DOpusListView *)ListViewIDCMP(listview,IMsg))==
				(struct DOpusListView *)-1) {
				if ((class=IMsg->Class)==IDCMP_VANILLAKEY && IMsg->Code==0x1b) {
					class=IDCMP_GADGETUP;
					gadgetid=PM_CANCEL;
				}
				else if (class==IDCMP_GADGETUP)
					gadgetid=((struct Gadget *) IMsg->IAddress)->GadgetID;
				ReplyMsg((struct Message *) IMsg);

				switch (class) {
					case IDCMP_GADGETUP:
						RemoveListView(listview,1);	
						CloseRequester(&pm_req);
						if (gadgetid==PM_OKAY) strcpy(path,buf);
						return((int)gadgetid);
				}
			}
			else if (view) {
				if (table[view->itemselected]) {
					if (LStrnCmp(table[view->itemselected],"  + ",4)==0)
						strcpy(buf,&table[view->itemselected][4]);
					else strcpy(buf,table[view->itemselected]);
					if (view->itemselected==lastsel &&
						(DoubleClick(lastseconds,lastmicros,seconds,micros))) {
						RemoveListView(listview,1);	
						CloseRequester(&pm_req);
						strcpy(path,buf);
						return(PM_OKAY);
					}
					lastseconds=seconds;
					lastmicros=micros;
					lastsel=view->itemselected;
				}
			}
		}
		Wait(1<<rwindow->UserPort->mp_SigBit);
	}
}

int get_multi_volume(lock,table,key)
BPTR lock;
char **table;
struct DOpusRemember **key;
{

/* AROS: Some structs here are private in AROS so we rewrite the thing */
#if 0
	struct DosList *doslist;
	struct RootNode *rootnode;
	struct DosInfo *dosinfo;
	struct AssignList *list;
	char buf[256];
	int count=0,tabcount=0,a;

	if (!system_version2) return(0);

	Forbid();

	rootnode=(struct RootNode *)DOSBase->dl_Root;
	dosinfo=(struct DosInfo *)BADDR(rootnode->rn_Info);
	doslist=(struct DosList *)BADDR(dosinfo->di_DevInfo);

	while (doslist) {
		if (doslist->dol_Type==DLT_DIRECTORY &&
			(SameLock(lock,doslist->dol_Lock)==LOCK_SAME)) {
			list=doslist->dol_misc.dol_assign.dol_List;
			while (list) {
				if (table) {
					PathName(list->al_Lock,buf,256);
					if ((table[tabcount]=LAllocRemember(key,(a=(strlen(buf)+7)),0))) {
						StrCombine(table[tabcount],"  + ",buf,a-1);
						TackOn(table[tabcount],NULL,a-1);
						++tabcount;
					}
				}
				++count;
				list=list->al_Next;
			}
		}
		doslist=(struct DosList *)BADDR(doslist->dol_Next);
	}

	Permit();
	if (table) return(tabcount);
	else return(count);
#else
	struct DosList *doslist;
	struct AssignList *list;
	char buf[256];
	int count=0,tabcount=0,a;

	if (!system_version2) return(0);

	Forbid();

	doslist = LockDosList(LDF_READ | LDF_ALL);

	while ((doslist = NextDosEntry(doslist, LDF_ALL))) {
		if (doslist->dol_Type==DLT_DIRECTORY &&
			(SameLock(lock,doslist->dol_Lock)==LOCK_SAME)) {
			list=doslist->dol_misc.dol_assign.dol_List;
			while (list) {
				if (table) {
					PathName(list->al_Lock,buf,256);
					if ((table[tabcount]=LAllocRemember(key,(a=(strlen(buf)+7)),0))) {
						StrCombine(table[tabcount],"  + ",buf,a-1);
						TackOn(table[tabcount],NULL,a-1);
						++tabcount;
					}
				}
				++count;
				list=list->al_Next;
			}
		}
	}
	UnLockDosList(LDF_READ | LDF_ALL);

	Permit();
	if (table) return(tabcount);
	else return(count);
#endif
}
