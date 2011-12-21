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

struct PathList2 {
    struct PathList2 *last;
    char path[FILEBUF_SIZE];
};

int __saveds DoAssign(register char *name __asm("a0"), register char *dir __asm("a1"))
{
//    struct RootNode *root;
//    struct DosInfo *dosinfo;
//    struct DeviceList *dl,*prevdev=NULL,*dev;
//    struct FileLock *flock;
    BPTR lock;
//    unsigned char *str;
    char /*buf[FILEBUF_SIZE],*/nname[FILEBUF_SIZE];
    int len,found/*=ASSIGN_NODEV*/;

    LStrCpy(nname,name);
    if (len=strlen(nname)==0) return(ASSIGN_NODEV);
    if (nname[len-1]==':') nname[len-1]=0;
/*
    root=(struct RootNode *) DOSBase->dl_Root;
    dosinfo=(struct DosInfo *)BADDR(root->rn_Info);
    dev=(struct DeviceList *)BADDR(dosinfo->di_DevInfo);

    while (dev) {
        BtoCStr(dev->dl_Name,buf,FILEBUF_SIZE);
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
*/
    found = AssignLock(nname,NULL)?ASSIGN_OK:ASSIGN_NODEV;

    if (!dir || !dir[0]) return(found);

    if (lock=Lock(dir,ACCESS_READ)) {
//        if (DOSBase->dl_lib.lib_Version>=36) {
            if (AssignLock(nname,lock)) return(ASSIGN_OK);
            return(ASSIGN_NODEV);
/*
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
*/
    }
    return(ASSIGN_NODEV);
}

char * __saveds DoBaseName(register char *path __asm("a0"))
{
    return FilePart(path);
/*
    int a,b;

    if ((a=strlen(path)-1)<0) return(path);
    if (path[a]=='/' || path[a]==':') if ((--a)<0) return(path);
    for (b=a;b>=0;b--) {
        if (path[b]=='/' || path[b]==':') return(&path[b+1]);
    }
    return(path);
*/
}

int __saveds DoPathName(register BPTR lock __asm("a0"), register char *buf __asm("a1"), register int len __asm("d0"))
{
    struct PathList2 *list=NULL,*temp,*first=NULL;
    struct FileInfoBlock __aligned finfo;
    BPTR templock;
    struct DOpusRemember *key=NULL;
    int a,it;

    buf[0]=it=0;
    do {
        Examine(lock,&finfo);
        if (!(temp=(struct PathList2 *)
            DoAllocRemember(&key,sizeof(struct PathList2),MEMF_CLEAR))) break;
        if (!first) first=temp;
        temp->last=list;
        list=temp;
        LStrCpy(list->path,finfo.fib_FileName);
        templock=lock;
        lock=ParentDir(lock);
        if (it) UnLock(templock);
        ++it;
    } while (lock);
    a=0;
    while (list) {
        DoStrConcat(buf,list->path,len);
        if (a==0) DoStrConcat(buf,":",len);
        else if (list->last) DoStrConcat(buf,"/",len);
        list=list->last;
        ++a;
    }
    DoFreeRemember(&key);
    return((int)strlen(buf));
}

int __saveds DoTackOn(register char *path __asm("a0"), register char *file __asm("a1"), register int len __asm("d0"))
{
    int a;

    a=strlen(path)-1;
    if (a>=0 && path[a]!=':' && path[a]!='/') DoStrConcat(path,"/",len);
    if (file) DoStrConcat(path,file,len);
    return((int)strlen(path));
}

int __saveds DoCompareLock(register BPTR l1 __asm("a0"), register BPTR l2 __asm("a1"))
{
    return SameLock(l1,l2);
/*
    struct FileLock *lock1,*lock2;
    struct DeviceList *dev1,*dev2;
    char name1[FILEBUF_SIZE],name2[FILEBUF_SIZE];

    lock1=(struct FileLock *)BADDR(l1);
    lock2=(struct FileLock *)BADDR(l2);

    if (lock1->fl_Task==lock2->fl_Task) {
        dev1=(struct DeviceList *)BADDR(lock1->fl_Volume);
        dev2=(struct DeviceList *)BADDR(lock2->fl_Volume);
        BtoCStr(dev1->dl_Name,name1,FILEBUF_SIZE);
        BtoCStr(dev2->dl_Name,name2,FILEBUF_SIZE);
        if (LStrCmp(name1,name2)==0) {
            if (lock1->fl_Key==lock2->fl_Key) return(LOCK_SAME);
            return(LOCK_SAME_VOLUME);
        }
    }
    return(LOCK_DIFFERENT);
*/
}

struct PathList
 {
  BPTR nextPath;
  BPTR pathLock;
 };

const static char *pathlists[7]={
    "Workbench","Initial CLI","Shell Process","New CLI",
    "AmigaShell","New_WShell","Background CLI"};

int __saveds DoSearchPathList(register char *name __asm("a0"), register char *buf __asm("a1"), register int len __asm("d0"))
{
    struct Process *proc,*myproc;
    struct CommandLineInterface *cli;
    struct PathList *wext;
    BPTR lock,lock1;
    APTR wsave;
    int a,pass=0;

    myproc=(struct Process *)FindTask(0);
    wsave=myproc->pr_WindowPtr;
    myproc->pr_WindowPtr=(APTR)-1;

    if (lock=Lock(name,ACCESS_READ)) {
        UnLock(lock);
        myproc->pr_WindowPtr=wsave;
        LStrCpy(buf,name);
        return(1);
    }
    
    Forbid();
    proc=myproc;
    for (a=0;a<8;a++) {
        if (proc) {
            if ((cli=(struct CommandLineInterface *)BADDR(proc->pr_CLI))) {
                for (wext=(struct PathList *)BADDR(cli->cli_CommandDir); wext; wext=(struct PathList *)BADDR(wext->nextPath)) {
                    if ((lock1=DupLock(wext->pathLock))) {
                        DoPathName(lock1,buf,len);
                        UnLock(lock1);
                        DoTackOn(buf,name,len);
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
}

int __saveds DoCheckExist(register char *name __asm("a0"), register int *size __asm("a1"))
{
    int a=0;
    BPTR lock;
    struct Process *myproc;
    APTR wsave;

//D(bug("CheckExist(%s)\n",name);Delay(50);)
    myproc = (struct Process *)FindTask(NULL);
    wsave = myproc->pr_WindowPtr;
    myproc->pr_WindowPtr = (APTR)-1;
    if ((lock = Lock(name,ACCESS_READ)))
     {
      struct FileInfoBlock __aligned fib;

      Examine(lock,&fib);

      if (size) *size = fib.fib_Size;

      a = fib.fib_DirEntryType;

      UnLock(lock);
     }
    myproc->pr_WindowPtr = wsave;

    return(a);
}
