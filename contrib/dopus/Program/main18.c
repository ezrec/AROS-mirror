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

struct recurse {
	struct recurse *last;
	char *dir,*dest;
	int data;
	APTR data2;
	APTR data3;
	BPTR lock;
	struct FileInfoBlock *info;
};

struct makedirlist {
	struct makedirlist *last,*next;
	char *path;
};

struct recurse *current_recurse;

int recursedir(fdir,fdest,dowhat,fdata)
char *fdir,*fdest;
int dowhat,fdata;
{
	BPTR mylock;
	
	
	/* AROS: Use AllocDosObject() instead
	struct FileInfoBlock __aligned myfinfo;
	struct FileInfoBlock __aligned enfinfo;
	*/
	struct FileInfoBlock *myfinfo;
	struct FileInfoBlock *enfinfo;
	
	char *name,*dir,*dest,*dname,*ddir,*adir,*adest,*ndir,*ndest;
	int suc,cont,ret,a,err,w1=1-data_active_window,adata,depth,b,rtry,data,*pstuff,blocks;
	struct recpath *crec=NULL,*trec;
	struct RecursiveDirectory
		*cur_recurse,*addparent_recurse,*new_rec,*pos_rec,*cur_parent,
		*cur_lastparent=NULL;
	APTR data2=NULL,adata2=NULL,data3=NULL,adata3=NULL;
	struct DOpusRemember *recurserem=NULL;
	struct makedirlist *first_makedir=NULL;

	if (dowhat&R_STARDIR) {
		rec_firstpath=NULL;
		rec_pathkey=NULL;
	}

	if (dowhat&R_GETNAMES) {
		first_recurse=NULL;
		recurse_dir_key=NULL;
		cur_recurse=NULL;
		addparent_recurse=NULL;
	}

	data=fdata;

	current_recurse=NULL; ret=depth=0; recurse_max_depth=0;
	dos_global_bytecount=0;
	dos_global_copiedbytes=0;
	dos_global_deletedbytes=0;
	dos_global_blocksneeded=0;

	if (!(mylock=Lock(fdir,ACCESS_READ))) {
		doerror(IoErr());
		return(-1);
	}
	myfinfo = AllocDosObject(DOS_FIB, NULL);
	enfinfo = AllocDosObject(DOS_FIB, NULL);
	
	assert(NULL != myfinfo);
	assert(NULL != enfinfo);
	
	Examine(mylock,myfinfo);

	if (!(name=LAllocRemember(&recurserem,2560,MEMF_CLEAR))) {
		doerror(IoErr());
		ret=-1;
		goto goaway;
	}
	dir=name+512; dest=name+1024; dname=name+1536; ddir=name+2048;
	if (fdir) strcpy(dir,fdir);
	if (fdest) strcpy(dest,fdest);

	if (dowhat&R_COPY) {
		strcpy(ddir,dest);
		if (!(a=copymakedir(&recurserem,&first_makedir,dest,myfinfo)) || a==-1) {
			UnLock(mylock);
			if (a==0) ret=-3;
			else ret=-10;
			goto goaway;
		}
	}
	cont=ExNext(mylock,myfinfo);

	FOREVER {
		if (status_haveaborted) {
			myabort();
			ret=-10;
			break;
		}
		if (!cont) {
			if (current_recurse) {
				if (mylock) UnLock(mylock);
				strcpy(dname,dir);
				mylock=current_recurse->lock;
				CopyMem((char *)current_recurse->info,(char *)myfinfo,sizeof(struct FileInfoBlock));
				strcpy(dir,current_recurse->dir);
				strcpy(dest,current_recurse->dest);
				data=current_recurse->data;
				data2=current_recurse->data2;
				data3=current_recurse->data3;
				current_recurse=current_recurse->last;

				strcpy(name,dir);
				TackOn(name,myfinfo->fib_FileName,512);

				if (dowhat&R_GETNAMES) {
					cur_recurse=(struct RecursiveDirectory *)data2;
					cur_lastparent=(struct RecursiveDirectory *)data3;
					addparent_recurse=NULL;
				}

				if (dowhat&R_COPY) {
					strcpy(ddir,dest);
					if (config->copyflags&COPY_DATE) {
						TackOn(ddir,myfinfo->fib_FileName,512);
						setdate(ddir,&myfinfo->fib_Date);
						strcpy(ddir,dest);
					}
				}

				a=0;
				if (dowhat&R_COMMENT) {
					FOREVER {
						if (!(SetComment(name,dest))) {
							err=IoErr();
							doerror(err);
							a=checkerror(globstring[STR_COMMENTING],myfinfo->fib_FileName,err);
							if (a==1) continue;
						}
						break;
					}
					if (a==3) break;
				}

				if (dowhat&R_PROTECT) {
					pstuff=(int *)data;
					b=getnewprot(myfinfo->fib_Protection,pstuff[0],pstuff[1]);
					FOREVER {
						if (!(SetProtection(name,b))) {
							err=IoErr();
							doerror(err);
							a=checkerror(globstring[STR_PROTECTING],myfinfo->fib_FileName,err);
							if (a==1) continue;
						}
						break;
					}
					if (a==3) break;
				}

				if (dowhat&R_DATESTAMP) {
					FOREVER {
						if ((err=setdate(name,(struct DateStamp *)data))!=1) {
							doerror(err);
							a=checkerror(globstring[STR_DATESTAMPING],myfinfo->fib_FileName,err);
							if (a==1) continue;
						}
						break;
					}
					if (a==3) break;
				}

				cont=ExNext(mylock,myfinfo);

				if (dowhat&R_DELETE && depth>0) {
					a=strlen(dname);
					if (a>0 && dname[a-1]=='/') dname[a-1]=0;
					if ((a=delfile(dname,BaseName(dname),globstring[STR_DELETING],
						glob_unprotect_all,1))==-1) {
						myabort();
						ret=-10;
						break;
					}
					if (a==2) glob_unprotect_all=1;
				}
				--depth;
				continue;
			}
			else break;
		}
		CopyMem((char *)myfinfo,(char *)enfinfo,sizeof(struct FileInfoBlock));

		strcpy(name,dir);
		TackOn(name,enfinfo->fib_FileName,512);

		if (enfinfo->fib_DirEntryType>0) {
			++dos_global_blocksneeded;
			if (enfinfo->fib_DirEntryType!=4) {
				adir=NULL;
				dofilename(name);
				if (dowhat&R_GETNAMES) {
					if ((new_rec=LAllocRemember(&recurse_dir_key,sizeof(struct RecursiveDirectory),MEMF_CLEAR))) {
						strcpy(new_rec->name,enfinfo->fib_FileName);
						CopyMem((char *)&enfinfo->fib_Date,(char *)&new_rec->date,sizeof(struct DateStamp));
						if (addparent_recurse) {
							addparent_recurse->child=new_rec;
							cur_lastparent=addparent_recurse;
							cur_recurse=new_rec;
						}
						else if (!first_recurse) {
							first_recurse=new_rec;
							cur_recurse=new_rec;
						}
						else if (cur_recurse) {
							if (LStrCmpI(new_rec->name,cur_recurse->name)<0) {
								if (first_recurse==cur_recurse) first_recurse=new_rec;
								if (cur_lastparent && cur_lastparent->child==cur_recurse)
									cur_lastparent->child=new_rec;
								new_rec->next=cur_recurse;
								cur_recurse=new_rec;
							}
							else {
								pos_rec=cur_recurse;
								FOREVER {
									if (!pos_rec->next ||
										(LStrCmpI(new_rec->name,pos_rec->next->name)<0)) {
										new_rec->next=pos_rec->next;
										pos_rec->next=new_rec;
										break;
									}
									pos_rec=pos_rec->next;
								}
							}
						}
						else cur_recurse=new_rec;
						cur_parent=new_rec;
					}
					adir=dir; adest=dest; ndir=name; ndest=dest;
					adata2=(APTR)cur_recurse;
					adata3=(APTR)cur_lastparent;
					addparent_recurse=cur_parent;
				}
				strcpy(dname,dest);
				if (dowhat&R_COPY) {

					dotaskmsg(hotkeymsg_port,PROGRESS_UPDATE,-2,0,enfinfo->fib_FileName,1);

					TackOn(dname,enfinfo->fib_FileName,512);
					adir=dir; adest=dest; adata=data; ndir=name; ndest=dname;
					strcpy(ddir,dname);
					if ((a=copymakedir(&recurserem,&first_makedir,ddir,enfinfo))==-1) {
						ret=-10;
						break;
					}
					else if (a==0) adir=NULL;
				}
				if (dowhat&R_DELETE) {
					rtry=1;
delloop:
					if (!DeleteFile(name)) {
						if (config->deleteflags&8 && rtry==1 && IoErr()==222) {
							rtry=0;
							SetProtection(name,0);
							goto delloop;
						}
						adir=dir; adest=dest; adata=data; ndir=name; ndest=dname;
					}
					else {
						if (config->dynamicflags&1) seename(data_active_window);
						adir=NULL;
					}
				}
				if (dowhat&R_HUNT || dowhat&R_SEARCH || dowhat&R_COMMENT ||
					dowhat&R_PROTECT || dowhat&R_DATESTAMP || dowhat&R_GETBYTES ||
					dowhat&R_STARDIR) {
					adir=dir; adest=dest; adata=data; ndir=name; ndest=dest;
					adata2=data2; adata3=data3;
				}
				if (adir) {
					if (!(addrecurse(&recurserem,adir,adest,adata,adata2,adata3,mylock,enfinfo))) {
						cont=0;
						continue;
					}
					strcpy(dir,ndir);
					strcpy(dest,ndest);
					if (!(mylock=Lock(dir,ACCESS_READ))) {
						cont=0;
						continue;
					}
					Examine(mylock,myfinfo);
					cont=ExNext(mylock, myfinfo);
					++depth;
					if (depth>recurse_max_depth) recurse_max_depth=depth;
					continue;
				}
			}
		}
		cont=ExNext(mylock,myfinfo);

		if (enfinfo->fib_DirEntryType<0) {
			a=0;
			dos_global_bytecount+=enfinfo->fib_Size;
			if (dowhat&R_GETBYTES && data) {
				blocks=(enfinfo->fib_Size+(data-1))/data;
				dos_global_blocksneeded+=blocks+(blocks/72)+1;
			}

			if (dowhat&R_GETNAMES) goto skipgetnam;
			dofilename(name);
			if (dowhat&R_STARDIR) {
				if ((trec=LAllocRemember(&rec_pathkey,sizeof(struct recpath),MEMF_CLEAR)) &&
					(trec->path=LAllocRemember(&rec_pathkey,(strlen(name)+1)-data,MEMF_CLEAR))) {
					trec->next=NULL;
					strcpy(trec->path,&name[data]);
					if (crec) crec->next=trec;
					crec=trec;
					if (!rec_firstpath) rec_firstpath=trec;
				}
			}
			if (!str_filter_parsed[0] ||
				LMatchPatternI(str_filter_parsed,enfinfo->fib_FileName)) {
				if (dowhat&R_COPY) {
					strcpy(dname,ddir);
					TackOn(dname,enfinfo->fib_FileName,512);

					dotaskmsg(hotkeymsg_port,PROGRESS_UPDATE,-2,0,enfinfo->fib_FileName,1);

					a=0;
					FOREVER {
						if (!(a=copyfile(name,dname,&err,-1,NULL,0))) {
							doerror(err);
							a=checkerror(globstring[STR_COPYING],enfinfo->fib_FileName,err);
							if (a==1) continue;
							if (a!=3) seename(data_active_window);
						}
						else if (a==-1) {
							myabort();
							a=3;
							break;
						}
						else {
							if (dowhat==R_COPY) {
								if (config->copyflags&COPY_ARC && !(enfinfo->fib_Protection&FIBF_ARCHIVE))
									SetProtection(name,enfinfo->fib_Protection|FIBF_ARCHIVE);
							}
							dos_global_copiedbytes+=enfinfo->fib_Size;
						}
						if (config->dynamicflags&1) seename(data_active_window);
						break;
					}
					if (a==3) {
						ret=-10;
						break;
					}
					if (config->dynamicflags&1) {
						seename(data_active_window); seename(w1);
					}
				}
				if (dowhat&R_DELETE) {
					if ((a=delfile(name,enfinfo->fib_FileName,globstring[STR_DELETING],
						glob_unprotect_all,1))==-1) {
						myabort();
						ret=-10;
						break;
					}
					if (a==2) glob_unprotect_all=1;
					if (config->dynamicflags&1) seename(data_active_window);
					dos_global_deletedbytes+=enfinfo->fib_Size;
				}
				if (dowhat&R_COMMENT) {
					FOREVER {
						if (!(SetComment(name,dest))) {
							err=IoErr();
							doerror(err);
							a=checkerror(globstring[STR_COMMENTING],enfinfo->fib_FileName,err);
							if (a==1) continue;
						}
						break;
					}
					if (a==3) {
						ret=-10;
						break;
					}
				}
				if (dowhat&R_PROTECT) {
					pstuff=(int *)data;
					b=getnewprot(enfinfo->fib_Protection,pstuff[0],pstuff[1]);
					FOREVER {
						if (!(SetProtection(name,b))) {
							err=IoErr();
							doerror(err);
							a=checkerror(globstring[STR_PROTECTING],enfinfo->fib_FileName,err);
							if (a==1) continue;
						}
						break;
					}
					if (a==3) {
						ret=-10;
						break;
					}
				}
				if (dowhat&R_DATESTAMP) {
					FOREVER {
						if ((err=setdate(name,(struct DateStamp *)data))!=1) {
							err=IoErr();
							doerror(err);
							a=checkerror(globstring[STR_DATESTAMPING],enfinfo->fib_FileName,err);
							if (a==1) continue;
						}
						break;
					}
					if (a==3) {
						ret=-10;
						break;
					}
				}
				if (dowhat&R_HUNT) {
					suc=huntfile(enfinfo->fib_FileName,name,&a);
					ret+=a;
					if (suc) {
						if (suc==-1) {
							myabort();
							ret=-10;
						}
						else ret=suc;
						break;
					}
				}
				if (dowhat&R_SEARCH) {
					suc=filesearch(name,&a,0);
					ret+=a;
					busy();
					if (suc==2) {
						if (!simplerequest(globstring[STR_CONTINUE_WITH_SEARCH],
							globstring[STR_CONTINUE],str_cancelstring,NULL)) {
							okay();
							ret=-2;
							break;
						}
					}
skipgetnam:
					if (suc==-1) {
						myabort();
						ret=-10;
						break;
					}
				}
			}
		}
	}
	if (mylock) UnLock(mylock);
	while (current_recurse) {
		UnLock(current_recurse->lock);
		current_recurse=current_recurse->last;
	}
	if (first_makedir &&
		dowhat==R_COPY &&
		str_filter_parsed[0]) {

		struct makedirlist *last;

		last=first_makedir;
		while (last->next) last=last->next;

		while (last) {
			DeleteFile(last->path);
			last=last->last;
		}
	}
goaway:
	if (recurserem) LFreeRemember(&recurserem);
	
	FreeDosObject(DOS_FIB, myfinfo);
	FreeDosObject(DOS_FIB, enfinfo);
	return(ret);
}

int addrecurse(key,dir,dest,data,data2,data3,lock,info)
struct DOpusRemember **key;
char *dir,*dest;
int data;
APTR data2,data3;
BPTR lock;
struct FileInfoBlock *info;
{
	struct recurse *rec;

	if (!(rec=LAllocRemember(key,sizeof(struct recurse),MEMF_CLEAR)))
		return(0);
	if (current_recurse) rec->last=current_recurse;
	else rec->last=NULL;
	if (!(rec->dir=LAllocRemember(key,strlen(dir)+1,MEMF_CLEAR)) ||
		!(rec->dest=LAllocRemember(key,strlen(dest)+1,MEMF_CLEAR)))
		return(0);
	strcpy(rec->dir,dir); strcpy(rec->dest,dest);
	rec->data=data;
	rec->data2=data2;
	rec->data3=data3;
	rec->lock=lock;
	if (!(rec->info=LAllocRemember(key,sizeof(struct FileInfoBlock),0)))
		return(0);
	CopyMem((char *)info,(char *)rec->info,sizeof(struct FileInfoBlock));
	current_recurse=rec;
	return(1);
}

int copymakedir(key,first,dirname,finfo)
struct DOpusRemember **key;
struct makedirlist **first;
char *dirname;
struct FileInfoBlock *finfo;
{
	int exist,a,err;
	BPTR mylock;

	exist=CheckExist(dirname,NULL);
	if (exist<=0) {
loop:
		if (exist<0 || !(mylock=CreateDir(dirname))) {
			if (exist<0) err=203;
			else err=IoErr();
			doerror(err);
			a=checkerror(globstring[STR_CREATING],finfo->fib_FileName,err);
			if (a==1) goto loop;
			if (a==2) return(0);
			return(-1);
		}
		if (mylock) {
			struct makedirlist *list,*pos;

			UnLock(mylock);

			if (config->copyflags&COPY_DATE) setdate(dirname,&finfo->fib_Date);
			if (config->copyflags&COPY_PROT) SetProtection(dirname,finfo->fib_Protection);
			if (config->copyflags&COPY_NOTE) SetComment(dirname,finfo->fib_Comment);

			if ((list=LAllocRemember(key,sizeof(struct makedirlist),MEMF_CLEAR)) &&
				(list->path=LAllocRemember(key,strlen(dirname)+1,0))) {
				strcpy(list->path,dirname);
				if (!(pos=*first)) *first=list;
				else {
					while (pos->next) pos=pos->next;
					pos->next=list;
					list->last=pos;
				}
			}
		}
	}
	return(1);
}
