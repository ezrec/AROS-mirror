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

void do_path_completion(win,qual)
int win;
USHORT qual;
{
	char path[256],match[32],*ptr;
	BPTR lock;
	
	/* AROS: We use AllocDosObject() instead
	struct FileInfoBlock __aligned finfo;
	*/

	struct FileInfoBlock *finfo;
	
	struct complete_entry *entry,*curentry,*addpos;
	int new=0,a;
	
	finfo = AllocDosObject(DOS_FIB, NULL);
	assert(NULL != finfo);

	strcpy(path,str_pathbuffer[win]);
	a=strlen(path);
	if (a>0 && (path[a-1]=='/' || path[a-1]==':')) match[0]=0;
	else if ((ptr=BaseName(path))) {
		LStrnCpy(match,ptr,31);
		match[31]=0;
		*ptr=0;
	}
	else {
		DisplayBeep(NULL);	
		goto exit;
	}

	if (LStrCmp(completion[win].path,path)!=0) new=1;
	else if (LStrCmp(completion[win].match,match)!=0) {
		entry=completion[win].firstentry;
		while (entry) {
			if (LStrCmpI(match,entry->name)==0) break;
			entry=entry->next;
		}
		if (!entry) new=1;
		else if (entry!=completion[win].currententry) {
			completion[win].currententry=entry;
			goto docomplete;
		}
	}

	if (new) {
		strcpy(completion[win].match,match);
		strcpy(completion[win].path,path);
		LFreeRemember(&completion[win].memory);
		completion[win].firstentry=NULL;
		completion[win].currententry=NULL;

		busy();
		if ((lock=Lock(path,ACCESS_READ))) {
			Examine(lock,finfo);
			if (finfo->fib_DirEntryType>0) {
				while (ExNext(lock,finfo)) {
					if (status_haveaborted) break;
					if (finfo->fib_DirEntryType>0 &&
						(!match[0] || LStrnCmpI(finfo->fib_FileName,match,strlen(match))==0)) {
						if ((entry=LAllocRemember(&completion[win].memory,
							sizeof(struct complete_entry),MEMF_CLEAR))) {
							strcpy(entry->name,finfo->fib_FileName);
							addpos=completion[win].firstentry;
							curentry=NULL;
							while (addpos) {
								curentry=addpos;
								if (LStrCmpI(addpos->name,entry->name)>0) break;
								addpos=addpos->next;
							}
							if (addpos) {
								entry->next=addpos;
								entry->last=addpos->last;
								addpos->last=entry;
								if (entry->last) entry->last->next=entry;
								if (addpos==completion[win].firstentry)
									completion[win].firstentry=entry;
							}
							else {
								if (curentry) {
									curentry->next=entry;
									entry->last=curentry;
								}
								else completion[win].firstentry=entry;
							}
						}
					}
				}
			}
			UnLock(lock);
		}
		unbusy();
	}

	if (completion[win].currententry) {
		if (qual&IEQUALIFIER_ANYSHIFT)
			completion[win].currententry=completion[win].currententry->last;
		else completion[win].currententry=completion[win].currententry->next;
	}

	if (!completion[win].currententry) {
		if (qual&IEQUALIFIER_ANYSHIFT) {
			entry=completion[win].firstentry;
			while (entry && entry->next) entry=entry->next;
			completion[win].currententry=entry;
		}
		else completion[win].currententry=completion[win].firstentry;
		if (!completion[win].currententry) {
			DisplayBeep(NULL);
			ActivateStrGad(&path_strgadget[win],Window);
			goto exit;
		}
	}
docomplete:
	strcpy(str_pathbuffer[win],path);
	TackOn(str_pathbuffer[win],completion[win].currententry->name,256);
	RefreshStrGad(&path_strgadget[win],Window);
	ActivateStrGad(&path_strgadget[win],Window);
exit:
	FreeDosObject(DOS_FIB, finfo);
}
