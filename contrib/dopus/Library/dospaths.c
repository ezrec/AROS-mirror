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

#include "dopuslib.h"

struct PathList {
	struct PathList *last;
	char path[32];
};

/*****************************************************************************

    NAME */

	AROS_LH2(int, Assign,

/*  SYNOPSIS */
	AROS_LHA(char *, name, 	A0),
	AROS_LHA(char *, dir, 	A1),

/*  LOCATION */
	struct Library *, DOpusBase, 32, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


/* AROS: struct DosInfo is private in AROS, so we have to rewrite the whole thing */
#if 0
	struct RootNode *root;
	struct DosInfo *dosinfo;
	struct DeviceList *dl,*prevdev=NULL,*dev;
	struct FileLock *flock;
	BPTR lock;
	unsigned char *str;
	char buf[40],nname[40];
	int len,found=ASSIGN_NODEV;

	strcpy(nname,name);
	if ((len=strlen(nname))>0 && nname[len-1]==':') nname[len-1]=0;

	root=(struct RootNode *) DOSBase->dl_Root;
	dosinfo=(struct DosInfo *)BADDR(root->rn_Info);
	dev=(struct DeviceList *)BADDR(dosinfo->di_DevInfo);

	while (dev) {
		BtoCStr(dev->dl_Name,buf,40);
		if (LStrCmpI(buf,nname)==0) {
			switch (dev->dl_Type) {
				case DLT_DIRECTORY:
					Forbid();
					if (prevdev==NULL) dosinfo->di_DevInfo=dev->dl_Next;
					else prevdev->dl_Next=dev->dl_Next;
					Permit();
					UnLock(dev->dl_Lock);
					str=(char *)(BADDR(dev->dl_Name));
					FreeMem(str,str[0]+1);
					FreeMem(dev,sizeof(struct DeviceList));
					dev=NULL; found=ASSIGN_OK;
					break;
				case DLT_DEVICE:
				case DLT_VOLUME:
					return(ASSIGN_CANCEL);
			}
		}
		if (!dev) break;
		prevdev=dev;
		dev=(struct DeviceList *)BADDR(dev->dl_Next);
	}

	if (!dir || !dir[0]) return(found);

	if (lock=Lock(dir,ACCESS_READ)) {
		if (DOSBase->dl_lib.lib_Version>=36) {
			if (AssignLock(nname,lock)) return(ASSIGN_OK);
			return(ASSIGN_NODEV);
		}
		if (!(dl=(struct DeviceList *)AllocMem(sizeof(struct DeviceList),MEMF_CLEAR))) {
			UnLock(lock);
			return(ASSIGN_NOMEM);
		}
		if (!(str=AllocMem(len+1,MEMF_CLEAR))) {
			FreeMem(dl,sizeof(struct DeviceList));
			UnLock(lock);
			return(ASSIGN_NOMEM);
		}
		CopyMem(nname,&str[1],len);
		*str=len;
		dl->dl_Name=(BSTR)(((long)str)>>2);
		dl->dl_Lock=lock;
		dl->dl_Type=DLT_DIRECTORY;
		flock=(struct FileLock *)(BADDR(lock));
		dl->dl_Task=flock->fl_Task;
		Forbid();
		dl->dl_Next=dosinfo->di_DevInfo;
		dosinfo->di_DevInfo=(BPTR)(((long)dl)>>2);
		Permit();
		return(ASSIGN_OK);
	}
	return(ASSIGN_NODEV);
	
#else

	if (AssignPath(name, dir)) {
		return ASSIGN_OK;
	} else {
		if (IoErr() == ERROR_NO_FREE_STORE) {
			return ASSIGN_NOMEM;
		} else {
			return ASSIGN_NODEV;
		}
	}
#endif	
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH1(char *, BaseName,

/*  SYNOPSIS */
	AROS_LHA(char *, path, 	A0),

/*  LOCATION */
	struct Library *, DOpusBase, 33, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	int a,b;

	if ((a=strlen(path)-1)<0) return(path);
	if (path[a]=='/' || path[a]==':') if ((--a)<0) return(path);
	for (b=a;b>=0;b--) {
		if (path[b]=='/' || path[b]==':') return(&path[b+1]);
	}
	return(path);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH3(int , PathName,

/*  SYNOPSIS */
	AROS_LHA(BPTR , lock, 	A0),
	AROS_LHA(char *, buf, 	A1),
	AROS_LHA(int , len, 	D0),

/*  LOCATION */
	struct Library *, DOpusBase, 35, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	struct PathList *list=NULL,*temp,*first=NULL;
	
	/* AROS: We have no __aligned keyword, so we have to allocate it 
	
	struct FileInfoBlock __aligned finfo;
	*/
	struct FileInfoBlock *finfo;
	BPTR templock;
	struct DOpusRemember *key=NULL;
	int a,it;
	
	finfo = AllocDosObject(DOS_FIB, NULL);
	if (NULL == finfo)
		return 0;

	buf[0]=it=0;
	do {
		Examine(lock,finfo);
		if (!(temp=(struct PathList *)
			LAllocRemember(&key,sizeof(struct PathList),MEMF_CLEAR))) break;
		if (!first) first=temp;
		temp->last=list;
		list=temp;
		strcpy(list->path,finfo->fib_FileName);
		templock=lock;
		lock=ParentDir(lock);
		if (it) UnLock(templock);
		++it;
	} while (lock);
	a=0;
	while (list) {
		StrConcat(buf,list->path,len);
		if (a==0) StrConcat(buf,":",len);
		else if (list->last) StrConcat(buf,"/",len);
		list=list->last;
		++a;
	}
	LFreeRemember(&key);
	
	FreeDosObject(DOS_FIB, finfo);
	
	return((int)strlen(buf));
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH3(int , TackOn,

/*  SYNOPSIS */
	AROS_LHA(char *, path, 	A0),
	AROS_LHA(char *, file, 	A1),
	AROS_LHA(int , len, 	D0),

/*  LOCATION */
	struct Library *, DOpusBase, 37, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	int a;

	a=strlen(path)-1;
	if (a>=0 && path[a]!=':' && path[a]!='/') StrConcat(path,"/",len);
	if (file) StrConcat(path,file,len);
	return((int)strlen(path));
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH2(int , CompareLock,

/*  SYNOPSIS */
	AROS_LHA(BPTR, l1, 	A0),
	AROS_LHA(BPTR, l2, 	A1),

/*  LOCATION */
	struct Library *, DOpusBase, 34, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	

	return SameLock(l1, l2);

#if 0
	struct FileLock *lock1,*lock2;
	struct DeviceList *dev1,*dev2;
	char name1[40],name2[40];

	lock1=(struct FileLock *)BADDR(l1);
	lock2=(struct FileLock *)BADDR(l2);

	if (lock1->fl_Task==lock2->fl_Task) {
		dev1=(struct DeviceList *)BADDR(lock1->fl_Volume);
		dev2=(struct DeviceList *)BADDR(lock2->fl_Volume);
		BtoCStr(dev1->dl_Name,name1,40);
		BtoCStr(dev2->dl_Name,name2,40);
		if (LStrCmp(name1,name2)==0) {
			if (lock1->fl_Key==lock2->fl_Key) return(LOCK_SAME);
			return(LOCK_SAME_VOLUME);
		}
	}
	return(LOCK_DIFFERENT);

#endif	
	
	AROS_LIBFUNC_EXIT
}

static char *pathlists[7]={
	"Workbench","Initial CLI","Shell Process","New CLI",
	"AmigaShell","New_WShell","Background CLI"};


/*****************************************************************************

    NAME */

	AROS_LH3(int , SearchPathList,

/*  SYNOPSIS */
	AROS_LHA(char *, name, 	A0),
	AROS_LHA(char *, buf, 	A1),
	AROS_LHA(int, len, 	D0),

/*  LOCATION */
	struct Library *, DOpusBase, 52, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	struct Process *proc,*myproc;
	struct CommandLineInterface *cli;
	BPTR *wext,lock,lock1;
	APTR wsave;
	int a,pass=0;

	myproc=(struct Process *)FindTask(0);
	wsave=myproc->pr_WindowPtr;
	myproc->pr_WindowPtr=(APTR)-1;

	if ((lock=Lock(name,ACCESS_READ))) {
		UnLock(lock);
		myproc->pr_WindowPtr=wsave;
		strcpy(buf,name);
		return(1);
	}
	
	Forbid();
	proc=myproc;
	for (a=0;a<8;a++) {
		if (proc) {
			if ((cli=(struct CommandLineInterface *)BADDR(proc->pr_CLI))) {
				for (wext=(BPTR *)BADDR(cli->cli_CommandDir);wext;wext=(BPTR *)BADDR(*wext)) {
					if ((lock1=DupLock(wext[1]))) {
						PathName(lock1,buf,len);
						UnLock(lock1);
						TackOn(buf,name,len);
						if ((lock=Lock(buf,ACCESS_READ))) {
							pass=1;
							UnLock(lock);
							break;
						}
					}
				}
			}
		}
		if (pass) break;
		if (a<7) proc=(struct Process *)FindTask(pathlists[a]);
	}
	Permit();
	myproc->pr_WindowPtr=wsave;
	return(pass);
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH2(int , CheckExist,

/*  SYNOPSIS */
	AROS_LHA(char *, name, 	A0),
	AROS_LHA(int *, size, 	A1),

/*  LOCATION */
	struct Library *, DOpusBase, 53, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)



	int a=0;
	BPTR mylock;
	
	/* AROS: We don't have a __aligned keyword
	struct FileInfoBlock __aligned myfinfo;
	
	*/
	struct FileInfoBlock * myfinfo;
	
	struct Process *myproc;
	APTR wsave;
	
	myfinfo = AllocDosObject(DOS_FIB, NULL);
	if (NULL == myfinfo) {
		return 0;
	}

	myproc=(struct Process *)FindTask(NULL);
	wsave=myproc->pr_WindowPtr;
	myproc->pr_WindowPtr=(APTR)-1;
	if ((mylock=Lock(name,ACCESS_READ))) {
		Examine(mylock,myfinfo);
		UnLock(mylock);
		a=myfinfo->fib_DirEntryType;
		if (size) *size=myfinfo->fib_Size;
	}
	myproc->pr_WindowPtr=wsave;
	
	FreeDosObject(DOS_FIB, myfinfo);
	return(a);
	
	AROS_LIBFUNC_EXIT
}
