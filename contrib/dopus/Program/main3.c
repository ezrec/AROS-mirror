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

#define OLD_ACTION_Dummy            20000

#define OLD_ACTION_SET_USER         (OLD_ACTION_Dummy+0)
#define OLD_ACTION_SET_GROUP        (OLD_ACTION_Dummy+1)
#define OLD_ACTION_UID_TO_USERINFO  (OLD_ACTION_Dummy+2)
#define OLD_ACTION_GID_TO_GROUPINFO (OLD_ACTION_Dummy+3)

#define ACTION_UID_TO_USERINFO      1037
#define ACTION_GID_TO_GROUPINFO     1038

int doparent(str)
char *str;
{
	int i,b,c;

	c=strlen(str);
	if (c==0) return(0);
	i=c-1;
	if (str[i]==':') {
		char tempbuf[256];

		expand_path(str,tempbuf);
		if ((LStrCmpI(str,tempbuf))==0 ||
			!(doparent(tempbuf))) return(0);
		strcpy(str,tempbuf);
		return(1);
	}
	if (str[i]=='/') --i;
	for (b=i;b>-1;b--) {
		if (str[b]=='/') { ++b; break; }
		if (str[b]==':') { ++b; break; }
	}
	if (b<0) expand_path("",str);
	else str[b]=0;
	return(1);
}

int doroot(str)
char *str;
{
	int i,b,f=0,c;

	c=strlen(str);
	if (c==0) return(0);
	i=c;
	for (b=0;b<i;b++) if (str[b]==':') { ++b; f=1; break; }
	if (f==1 && b==i) {
		char tempbuf[256];

		expand_path(str,tempbuf);
		if ((LStrCmpI(str,tempbuf))==0 ||
			!(doroot(tempbuf))) return(0);
		strcpy(str,tempbuf);
		return(1);
	}
	else if (f>0) str[b]=0;
	else expand_path("",str);
	return(1);
}

int addfile(dir,win,name,size,type,date,comment,protection,subtype,show,dispstr,addafter,ownerid,groupid)
struct DirectoryWindow *dir;
int win;
char *name;
int size,type;
struct DateStamp *date;
char *comment;
int protection,subtype,show;
char *dispstr;
struct Directory *addafter;
UWORD ownerid,groupid;
{
	struct Directory *addposition=NULL,*entry,*workfirst,*newentry;
	int a,b,reverse=0,check=0,sortmethod,endwhile,selsortmethod;
	char *description=NULL,*owner=NULL,*group=NULL,*ptr;

	if (status_iconified && status_flags&STATUS_ISINBUTTONS) return(1);

	if (config->sortflags&(1<<win)) reverse=1;

	selsortmethod=config->sortmethod[win];
	if (selsortmethod<0 || selsortmethod>DISPLAY_LAST) selsortmethod=DISPLAY_NAME;

	if (type==ENTRY_DEVICE) sortmethod=DISPLAY_NAME;
	else if (type>=ENTRY_DIRECTORY) {
		switch (selsortmethod) {
			case DISPLAY_SIZE:
			case DISPLAY_FILETYPE:
				sortmethod=DISPLAY_NAME;
				break;
			case DISPLAY_OWNER:
			case DISPLAY_GROUP:
			case DISPLAY_NETPROT:
				if (!AccountsBase) {
					sortmethod=DISPLAY_NAME;
					break;
				}
			default:
				sortmethod=selsortmethod;
				break;
		}
	}
	else {
		if (!AccountsBase &&
			(selsortmethod==DISPLAY_OWNER ||
			selsortmethod==DISPLAY_GROUP ||
			selsortmethod==DISPLAY_NETPROT))
			sortmethod=DISPLAY_NAME;
		else sortmethod=selsortmethod;
	}

	if (win<0 || !dir || (type!=ENTRY_CUSTOM && protection&128 && config->hiddenbit))
		return(-1);

	if (type<=ENTRY_FILE) {
		if (config->showpat[0] && !(LMatchPatternI(config->showpatparsed,name)))
			return(-1);
		if (config->hidepat[0] && (LMatchPatternI(config->hidepatparsed,name)))
			return(-1);
	}

	if (type==ENTRY_CUSTOM) {
		workfirst=dir->firstentry;
		if (addafter) addposition=addafter;
		else {
			switch (subtype) {
				case CUSTOMENTRY_BUFFERLIST:
					entry=dir->firstentry;
					while (entry) {
						if (inter_sort(comment,entry->comment)<=0) {
							if (entry->last) addposition=entry->last;
							else addposition=(struct Directory *)-1;
							break;
						}
						entry=entry->next;
					}
					break;
			}
		}
	}
	else {
		if (type!=ENTRY_DEVICE) {
			for (a=0;a<DISPLAY_LAST+1;a++) {
				if (config->displaypos[win][a]==DISPLAY_FILETYPE && !description)
					description=getfiledescription(name,win);
				else if (AccountsBase) {
					if (config->displaypos[win][a]==DISPLAY_OWNER && !owner && ownerid) {
						if (dir->last_owner==ownerid) owner=dir->owner_name;
						else if (user_info &&
							!(EnvoyPacket(dir->realdevice,
								ACTION_UID_TO_USERINFO,OLD_ACTION_UID_TO_USERINFO,ownerid,user_info))) {
							owner=user_info->ui_UserName;
							dir->last_owner=ownerid;
							strcpy(dir->owner_name,owner);
						}
					}
					else if (config->displaypos[win][a]==DISPLAY_GROUP && !group && groupid) {
						if (dir->last_group==groupid) group=dir->group_name;
						else if (group_info &&
							!(EnvoyPacket(dir->realdevice,
								ACTION_GID_TO_GROUPINFO,OLD_ACTION_GID_TO_GROUPINFO,groupid,group_info))) {
							group=group_info->gi_GroupName;
							dir->last_group=groupid;
							strcpy(dir->group_name,group);
						}
					}
				}
			}
		}
		workfirst=addposition=NULL;

		if (addafter) {
			if (!(workfirst=addafter->next)) workfirst=addposition=addafter;
		}
		else {
			switch (config->separatemethod[win]) {
				case SEPARATE_DIRSFIRST:
					if (dir->firstfile || dir->firstentry) {
						if (type<=ENTRY_FILE) {
							workfirst=dir->firstfile;
							if (!workfirst) {
								workfirst=dir->firstentry;
								while (workfirst->next) workfirst=workfirst->next;
								addposition=workfirst;
							}
						}
						else workfirst=dir->firstentry;
					}
					check=1;
					break;
				case SEPARATE_FILESFIRST:
					if (dir->firstdir || dir->firstentry) {
						if (type>=ENTRY_DEVICE) {
							workfirst=dir->firstdir;
							if (!workfirst) {
								workfirst=dir->firstentry;
								while (workfirst->next) workfirst=workfirst->next;
								addposition=workfirst;
							}
						}
						else workfirst=dir->firstentry;
					}
					check=1;
					break;
				default:
					workfirst=dir->firstentry;
					break;
			}
		}

		if (workfirst && !addposition) {
			entry=workfirst; endwhile=0;
			while (entry) {
				if (check) {
					switch (config->separatemethod[win]) {
						case SEPARATE_DIRSFIRST:
							if (entry->type<=ENTRY_FILE && type>=ENTRY_DEVICE) {
								if (entry->last) addposition=entry->last;
								else addposition=(struct Directory *)-1;
								endwhile=1;
							}
							break;
						case SEPARATE_FILESFIRST:
							if (type<=ENTRY_FILE && entry->type>=ENTRY_DEVICE) {
								if (entry->last) addposition=entry->last;
								else addposition=(struct Directory *)-1;
								endwhile=1;
							}
							break;
					}
				}
				switch (sortmethod) {
					case -1:
						if (!entry->next) {
							addposition=entry;
							endwhile=2;
						}
						break;

					case DISPLAY_NAME:
sortname:
						if ((reverse && (namesort(name,entry->name)>=0)) ||
							(!reverse && (namesort(name,entry->name)<=0))) endwhile=1;
						break;

					case DISPLAY_SIZE:
						if (size==entry->size) goto sortname;
						if ((reverse && size>=entry->size) || (!reverse && size<=entry->size))
							endwhile=1;
						break;

					case DISPLAY_PROTECT:
						a=(entry->protection&255)^15;
						b=(protection&255)^15;
						if (a==b) goto sortname;
						if ((reverse && b>=a) || (!reverse && b<=a)) endwhile=1;
						break;

					case DISPLAY_DATE:
						if ((a=CompareDate(date,(&entry->date)))==0) goto sortname;
						if ((reverse && a>=0) || (!reverse && a<=0)) endwhile=1;
						break;

					case DISPLAY_COMMENT:
						if (comment && entry->comment)
							a=LStrCmpI(comment,entry->comment);
						else if (comment) a=1;
						else if (entry->comment) a=-1;
						else a=0;
						if (a==0) goto sortname;
						if ((reverse && a>=0) || (!reverse && a<=0)) endwhile=1;
						break;

					case DISPLAY_FILETYPE:
						if (description && entry->description)
							a=LStrCmpI(description,entry->description);
						else if (description) a=1;
						else if (entry->description) a=-1;
						else a=0;
						if (a==0) goto sortname;
						if ((reverse && a>=0) || (!reverse && a<=0)) endwhile=1;
						break;

					case DISPLAY_OWNER:
						if (owner && entry->network && entry->network->owner)
							a=LStrCmpI(owner,entry->network->owner);
						else if (owner) a=1;
						else if (entry->network && entry->network->owner) a=-1;
						else a=0;
						if (a==0) goto sortname;
						if ((reverse && a>=0) || (!reverse && a<=0)) endwhile=1;
						break;

					case DISPLAY_GROUP:
						if (group && entry->network && entry->network->group)
							a=LStrCmpI(group,entry->network->group);
						else if (group) a=1;
						else if (entry->network && entry->network->group) a=-1;
						else a=0;
						if (a==0) goto sortname;
						if ((reverse && a>=0) || (!reverse && a<=0)) endwhile=1;
						break;

					case DISPLAY_NETPROT:
						a=entry->protection&(~255);
						b=protection&(~255);
						if (a==b) goto sortname;
						if ((reverse && b>=a) || (!reverse && b<=a)) endwhile=1;
						break;
				}
				if (endwhile) {
					if (endwhile==1) {
						if (entry->last) addposition=entry->last;
						else addposition=(struct Directory *)-1;
					}
					break;
				}
				entry=entry->next;
			}
		}
		else workfirst=NULL;
	}

	if (!(newentry=LibAllocPooled(dir_memory_pool,sizeof(struct Directory))))
		return(0);
	newentry->last=NULL;
	newentry->next=NULL;
	newentry->comment=NULL;
	newentry->dispstr=NULL;
	newentry->description=NULL;
	newentry->network=NULL;

	if (addposition==(struct Directory *)-1) {	/* Add at head of list */
		newentry->next=workfirst;
		if (workfirst) workfirst->last=newentry;
		switch (config->separatemethod[win]) {
			case SEPARATE_DIRSFIRST:
				if (type<=ENTRY_FILE) dir->firstfile=newentry;
				break;
			case SEPARATE_FILESFIRST:
				if (type>=ENTRY_DEVICE) dir->firstdir=newentry;
				break;
		}
		dir->firstentry=newentry;
	}
	else if (addposition) {	/* Add after this entry */
		if (addposition->next) {
			newentry->next=addposition->next;
			addposition->next->last=newentry;
		}
		newentry->last=addposition;
		addposition->next=newentry;
		if (addposition->type>=ENTRY_DEVICE && type<=ENTRY_FILE)
			dir->firstfile=newentry;
		else if (addposition->type<=ENTRY_FILE && type>=ENTRY_DEVICE)
			dir->firstdir=newentry;
	}
	else {
		if (workfirst) {	/* Add at end of list */
			while (workfirst->next) workfirst=workfirst->next;
			workfirst->next=newentry;
			newentry->last=workfirst;
		}
		else {	/* Starting a new list */
			dir->firstentry=newentry;
			if (type<=ENTRY_FILE) dir->firstfile=newentry;
			else dir->firstdir=newentry;
		}
	}

	if (type==ENTRY_CUSTOM) {
		if (dispstr && dispstr[0]) {
			if (!(newentry->dispstr=LibAllocPooled(dir_memory_pool,strlen(dispstr)+1)))
				return(0);
			strcpy(newentry->dispstr,dispstr);
		}
		lsprintf(newentry->name,"%ld",dir->total);
	}
	else {
		LStrnCpy(newentry->name,name,30);
		if (type!=ENTRY_DEVICE && description &&
			(newentry->description=LibAllocPooled(dir_memory_pool,strlen(description)+1))) {
			strcpy(newentry->description,description);
		}
		if (AccountsBase &&
			(newentry->network=LibAllocPooled(dir_memory_pool,sizeof(struct NetworkStuff)))) {

			if (owner && (newentry->network->owner=LibAllocPooled(dir_memory_pool,strlen(owner)+1)))
				strcpy(newentry->network->owner,owner);

			if (group && (newentry->network->group=LibAllocPooled(dir_memory_pool,strlen(group)+1)))
				strcpy(newentry->network->group,group);

			ptr=newentry->network->net_protbuf;
			for (a=FIBB_GRP_READ,b=0;a>=FIBB_GRP_DELETE;a--,b++) {
				if (!(protection&(1<<a))) *ptr++=globstring[STR_PROTBIT_RWED][0];
				else *ptr++=globstring[STR_PROTBIT_RWED][b+4];
			}
			*ptr++=' ';
			for (a=FIBB_OTR_READ,b=0;a>=FIBB_OTR_DELETE;a--,b++) {
				if (!(protection&(1<<a))) *ptr++=globstring[STR_PROTBIT_RWED][0];
				else *ptr++=globstring[STR_PROTBIT_RWED][b+4];
			}
		}
	}
	newentry->size=size;
	newentry->owner_id=ownerid;
	newentry->group_id=groupid;

	if ((newentry->type=type)==ENTRY_DEVICE) {
		switch (size) {
			case DLT_DEVICE: comment="<DEV>"; break;
			case DLT_DIRECTORY: comment="<ASN>"; break;
			case DLT_VOLUME: comment="<VOL>"; break;
			case DLT_LATE: comment="<DFR>"; break;
			case DLT_NONBINDING: comment="<NBD>"; break;
		}
	}
	if (date) {
		newentry->date.ds_Days=date->ds_Days;
		newentry->date.ds_Minute=date->ds_Minute;
		newentry->date.ds_Tick=date->ds_Tick;
	}

	if (comment && comment[0]) {
		if ((newentry->comment=LibAllocPooled(dir_memory_pool,strlen(comment)+1)))
			strcpy(newentry->comment,comment);
		else if (type==ENTRY_CUSTOM) return(0);
	}

	newentry->protection=protection;
	if (type!=ENTRY_CUSTOM) getprot(protection,newentry->protbuf);

	if (date) seedate(date,newentry->datebuf,1);
	newentry->subtype=subtype;
	newentry->selected=FALSE;
	switch (ENTRYTYPE(type)) {
		case ENTRY_FILE:
			++dir->filetot;
			dir->bytestot+=size;
			break;
		case ENTRY_DIRECTORY:
			if (size>-1) dir->bytestot+=size;
		case ENTRY_DEVICE:
			++dir->dirtot;
			break;
	}
	++dir->total;

	if (newentry->next) {
		if (newentry->name[0] && (LStrCmpI(newentry->name,newentry->next->name))==0)
			removefile(newentry->next,dir,win,0);
		else if (type==ENTRY_CUSTOM && subtype==CUSTOMENTRY_BUFFERLIST &&
			(LStrCmpI(newentry->comment,newentry->next->comment))==0)
			removefile(newentry->next,dir,win,0);
	}
	if (show && type!=ENTRY_DEVICE) {
		refreshwindow(win,1);
		doposprop(win);
	}
	return((int)newentry);
}

int namesort(str1,str2)
char *str1,*str2;
{
	int n1,n2;

	if (isdigit(str1[0]) && isdigit(str2[0])) {
		n1=atoi(str1); n2=atoi(str2);
		if (n1!=n2) return((n1-n2));
		str1=getstrafternum(str1);
		str2=getstrafternum(str2);
	}
	return(inter_sort(str1,str2));
}

int inter_sort(str1,str2)
char *str1,*str2;
{
	if (UtilityBase) return(Stricmp(str1,str2));
	return(LStrCmpI(str1,str2));
}

char *getstrafternum(str)
char *str;
{
	char *ptr=str;

	while (*ptr) {
		if (!isdigit(*ptr)) break;
		++ptr;
	}
	return(ptr);
}

void removefile(file,dir,win,show)
struct Directory *file;
struct DirectoryWindow *dir;
int win,show;
{
	int ty=file->type,sz=file->size,se=file->selected;

	if (win<0 || !dir || (status_iconified && status_flags&STATUS_ISINBUTTONS)) return;

	if (file==last_selected_entry) last_selected_entry=NULL;

	if (file==dir->firstfile) {
		if (file->next) {
			dir->firstfile=file->next;
			if (file->last) dir->firstfile->last=file->last;
			else dir->firstfile->last=NULL;
		}
		else dir->firstfile=NULL;
	}
	else if (file==dir->firstdir) {
		if (file->next) {
			dir->firstdir=file->next;
			if (file->last) dir->firstdir->last=file->last;
			else dir->firstdir->last=NULL;
		}
		else dir->firstdir=NULL;
	}
	if (file==dir->firstentry) {
		if (file->next) {
			dir->firstentry=file->next;
			if (dir->firstentry->next) dir->firstentry->next->last=dir->firstentry;
			dir->firstentry->last=NULL;
		}
		else dir->firstentry=NULL;
	}
	else {
		if (file->next) {
			file->last->next=file->next;
			file->next->last=file->last;
		}
		else file->last->next=NULL;
	}
	if (func_single_entry && func_single_entry==file) func_entry_deleted=1;
	free_file_memory(file);
	--dir->total;
	if (ty<0) {
		--dir->filetot; dir->bytestot-=sz;
		if (se) {
			--dir->filesel;
			dir->bytessel-=sz;
		}
	}
	else {
		--dir->dirtot;
		if (ty>0 && sz>0) dir->bytestot-=sz;
		if (se) {
			--dir->dirsel;
			if (ty>0 && sz>0) dir->bytessel-=sz;
		}
	}
	if (dir->offset>(dir->total-scrdata_dispwin_lines))
		dir->offset=dir->total-scrdata_dispwin_lines;
	if (dir->offset<0) dir->offset=0;
	if (show) refreshwindow(win,1);
}

int dorun(name,state,workbench)
char *name;
int state,workbench;
{
	char argbuf[256],louise[80],buf[256];
	int rec,len;
	struct dopusfuncpar par;

kprintf("dorun(%s, %d, %d)\n", name, state, workbench);
	argbuf[0]=0;
	if (workbench) {
		buf[0]=FC_WORKBENCH;
		buf[1]=0;
	}
	else buf[0]=0;
	StrConcat(buf,"\"",256);
	StrConcat(buf,name,256);
	StrConcat(buf,"\"",256);

	len=256-strlen(buf);
	if (state) {
		lsprintf(louise,globstring[STR_ENTER_ARGUMENTS_FOR],BaseName(name));
kprintf("louise: %s\n", louise);
		if (!(rec=whatsit(louise,len,argbuf,globstring[STR_SKIP]))) {
			myabort();
kprintf("dorun failed\n");
			return(-1);
		}
		if (rec==2) {
		    return(-2);
kprintf("dorun failed\n");
		    
		}
kprintf("rec: %d\n", rec);
	}
	else rec=1;
	if (!rec) {
		myabort();
kprintf("dorun failed\n");
		return(0);
	}
	if (argbuf[0]) {
		StrConcat(buf," ",256);
		StrConcat(buf,argbuf,256);
	}
kprintf("calling defaultpar\n");
	defaultpar(&par);
kprintf("calling dofunctionstring\n");
	dofunctionstring(buf,NULL,BaseName(name),&par);
kprintf("dorun succeeded\n");
	return(1);
}

void busy()
{
	if (!(status_flags&STATUS_BUSY)) {
		size_gadgets[0].GadgetType=GTYP_BOOLGADGET;
		size_gadgets[1].GadgetType=GTYP_BOOLGADGET;
		if (!Window->FirstRequest) {
			InitRequester(&dopus_busy_requester);
			dopus_busy_requester.Flags=NOISYREQ;
			Request(&dopus_busy_requester,Window);
		}
		endnotifies();
		status_flags|=STATUS_BUSY;
	}
	SetBusyPointer(Window);
}

void unbusy()
{
	if (status_flags&STATUS_BUSY) {
		struct Message *msg;

		if (Window->FirstRequest) EndRequest(&dopus_busy_requester,Window);
		flushidcmp();
		while ((msg=GetMsg(count_port))) ReplyMsg(msg);
		startnotifies();
		size_gadgets[0].GadgetType=GTYP_SIZING;
		size_gadgets[1].GadgetType=GTYP_SIZING;
		status_flags&=~STATUS_BUSY;
	}
	ClearPointer(Window);
}

void flushidcmp()
{
	while (getintuimsg()) ReplyMsg((struct Message *)IMsg);
}

void setnullpointer(wind)
struct Window *wind;
{
	SetPointer(wind,null_pointer,1,16,0,0);
}

void free_file_memory(file)
struct Directory *file;
{
	if (file) {
		if (file->comment)
			LibFreePooled(dir_memory_pool,file->comment,strlen(file->comment)+1);

		if (file->type==ENTRY_CUSTOM && file->dispstr)
			LibFreePooled(dir_memory_pool,file->dispstr,strlen(file->dispstr)+1);

		if (file->description)
			LibFreePooled(dir_memory_pool,file->description,strlen(file->description)+1);

		if (file->network) {
			if (file->network->owner)
				LibFreePooled(dir_memory_pool,file->network->owner,strlen(file->network->owner)+1);

			if (file->network->group)
				LibFreePooled(dir_memory_pool,file->network->group,strlen(file->network->group)+1);

			LibFreePooled(dir_memory_pool,file->network,sizeof(struct NetworkStuff));
		}

		LibFreePooled(dir_memory_pool,file,sizeof(struct Directory));
	}
}
