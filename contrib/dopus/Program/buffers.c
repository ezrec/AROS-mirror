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

/* Allocate or free directory buffers as necessary to achieve desired
   buffer count in each window */

void allocdirbuffers(newnum)
int newnum;
{
    int win,a,num;
    struct DirectoryWindow *dir,*next;

    for (win=0;win<2;win++) {
        if (data_buffer_count[win]>newnum) {
            dir=dopus_firstwin[win];
            for (a=0;a<newnum;a++) dir=dir->next;
            dir->last->next=dopus_firstwin[win];
            dopus_firstwin[win]->last=dir->last;
            num=data_buffer_count[win];
            for (a=newnum;a<num;a++) {
                next=dir->next;
                freedir(dir,-1);
                if (dopus_curwin[win]==dir) dopus_curwin[win]=dopus_firstwin[win];
                FreeMem(dir,sizeof(struct DirectoryWindow));
                if ((--data_buffer_count[win])<1) break;
                dir=next;
            }
        }
        else if (data_buffer_count[win]<newnum) {
            dir=dopus_firstwin[win];
            for (a=0;a<data_buffer_count[win]-1;a++) dir=dir->next;
            for (a=data_buffer_count[win];a<newnum;a++) {
                if (next=AllocMem(sizeof(struct DirectoryWindow),MEMF_CLEAR)) {
                    if (!dir) {
                        dopus_firstwin[win]=next;
                        dopus_curwin[win]=next;
                        dir=next;
                    }
                    dir->next=next;
                    next->last=dir;
                    next->next=dopus_firstwin[win];
                    dopus_firstwin[win]->last=next;
                    next->number=a;
                    next->hlen=145;
                    ++data_buffer_count[win];
                    dir=next;
                }
            }
        }
    }
}

/* Searches both buffer lists for a given directory, and displays it in
   the active window if found (copying it over if necessary) */

int bringinbuffer(dirbuf,win,read)
char *dirbuf;
int win,read;
{
    int a,otherwin;
    struct DirectoryWindow *dir;

    if (status_iconified || !dirbuf) return(0);
    checkcurrentbuffer(win);
    if (!(findbuffer(dirbuf,win,0,0))) {
        otherwin=1-data_active_window;
        dir=dopus_firstwin[otherwin];
        for (a=0;a<data_buffer_count[otherwin];a++,dir=dir->next) {
            if ((Stricmp(dirbuf,dir->directory))==0) {
                incrementbuf(data_active_window,1,0);
                copydirwin(dir,dopus_curwin[data_active_window],data_active_window);
                check_old_buffer(data_active_window);
                return(1);
            }
        }
        if (read) {
            strcpy(str_pathbuffer[data_active_window],dirbuf);
            startgetdir(data_active_window,SGDFLAGS_CANMOVEEMPTY);
        }
        else return(0);
    }
    return(1);
}

/* Searches forward from the current buffer to find an empty one, or one
   with the same name (it looks first for one with the same name).
   If a buffer is not found, it uses the current buffer */

void findemptybuffer(win)
int win;
{
    struct DirectoryWindow *dir=dopus_curwin[win],*oldestdir;
    int a,ok=0;
    char fullbuf[256];

    if (dir->flags & DWF_ARCHIVE) return;
//D(bug("findemptybuffer()\n"));
    expand_path(str_pathbuffer[win],fullbuf);
    checkdir(str_pathbuffer[win],NULL);
    checkdir(fullbuf,NULL);

    for (a=0;a<data_buffer_count[win];a++) {
        if (Stricmp(dir->directory,str_pathbuffer[win])==0 ||
            Stricmp(dir->directory,fullbuf)==0) {
            ok=1;
            break;
        }
        dir=dir->next;
    }
    if (!ok) {
        oldestdir=dir=dopus_curwin[win];
        for (a=0;a<data_buffer_count[win];a++) {
            if (!dir->directory[0])
             {
              oldestdir=dir;
              break;
             }
            if (CompareDates(&oldestdir->dirstamp,&dir->dirstamp)<0) oldestdir=dir;
            dir=dir->next;
        }
        dir=oldestdir;
    }
    dopus_curwin[win]=dir;
}

/* Sets up and reads a new directory */

void startgetdir(win,flags)
int win,flags;
{
    checkcurrentbuffer(win);
//D(bug("startgetdir %s\ncurrent: %s\n",str_pathbuffer[win],dopus_curwin[win]->directory));
    if (!(config->dirflags&DIRFLAGS_CHECKBUFS && (flags&SGDFLAGS_CANCHECKBUFS)) ||
        !(findbuffer(str_pathbuffer[win],win,1,0))) {
        if ((config->dirflags&DIRFLAGS_FINDEMPTY) && (flags&SGDFLAGS_CANMOVEEMPTY))
            findemptybuffer(win);
        if (str_pathbuffer[win][0]==0) expand_path("",str_pathbuffer[win]);
        checkdir(str_pathbuffer[win],&path_strgadget[win]);
//D(bug("path: %s\n",str_pathbuffer[win]));
        strcpy(dopus_curwin[win]->directory,str_pathbuffer[win]);
        getdir(dopus_curwin[win],win,(flags&SGDFLAGS_REREADINGOLD));
    }
    seename(win);
    refreshwindow(win,0);
}

/* Moves forward or backward a buffer and refreshes display */

void incrementbuf(win,dir,show)
int win,dir,show;
{
    advancebuf(win,dir);
    strcpy(str_pathbuffer[win],dopus_curwin[win]->directory);
    checkdir(str_pathbuffer[win],&path_strgadget[win]);
    checksize(win);
    check_old_buffer(win);
    if (show) {
        seename(win);
        refreshwindow(win,1);
        doselinfo(win);
    }
    startnotify(win);
}

/* Copies the contents of one window to another */

void copydirwin(sourcewin,destwin,dest)
struct DirectoryWindow *sourcewin,*destwin;
int dest;
{
    struct Directory *copy;

    if (status_iconified) return;
    busy();
    freedir(destwin,dest);
    copy=sourcewin->firstentry;
    strcpy(destwin->realdevice,sourcewin->realdevice);
    while (copy) {
        if (!(addfile(destwin,dest,copy->name,copy->size,copy->type,&copy->date,
            copy->comment,copy->protection,copy->subtype,FALSE,copy->dispstr,NULL,
            copy->owner_id,copy->group_id))) break;
        copy=copy->next;
    }
    strcpy(str_pathbuffer[dest],sourcewin->directory);
    strcpy(destwin->directory,sourcewin->directory);
    strcpy(destwin->diskname,sourcewin->diskname);
    destwin->hlen=sourcewin->hlen;
    destwin->flags=sourcewin->flags;
    copy_datestamp(&sourcewin->dirstamp,&destwin->dirstamp);
    refreshwindow(dest,1);
    checkdir(str_pathbuffer[dest],&path_strgadget[dest]);
    checksize(dest);
    last_selected_entry=NULL;
    okay();
    unbusy();
}

/* Swaps the contents of the two windows */

void swapdirwin()
{
    int tmp;
    struct DirectoryWindow tempwin;

    if (status_iconified) return;

    CopyMem((char *)dopus_curwin[0],(char *)&tempwin,sizeof(struct DirectoryWindow));
    tempwin.next=dopus_curwin[1]->next; tempwin.number=dopus_curwin[1]->number;
    tempwin.last=dopus_curwin[1]->last;
    dopus_curwin[1]->next=dopus_curwin[0]->next; dopus_curwin[1]->number=dopus_curwin[0]->number;
    dopus_curwin[1]->last=dopus_curwin[0]->last;
    CopyMem((char *)dopus_curwin[1],(char *)dopus_curwin[0],sizeof(struct DirectoryWindow));
    CopyMem((char *)&tempwin,(char *)dopus_curwin[1],sizeof(struct DirectoryWindow));

    for (tmp=0;tmp<2;tmp++) {
        strcpy(str_pathbuffer[tmp],dopus_curwin[tmp]->directory);
        refreshwindow(tmp,1);
        checkdir(str_pathbuffer[tmp],&path_strgadget[tmp]);
        checksize(tmp);
    }
    okay();
}

/* Moves forwards or backwards a directory */

void advancebuf(win,dir)
int win,dir;
{
    int a;

    endnotify(win);
    if (!checkcurrentbuffer(win)) {
        if (dir>0) {
            for (a=0;a<dir;a++) dopus_curwin[win]=dopus_curwin[win]->next;
        }
        else {
            for (a=dir;a<0;a++) dopus_curwin[win]=dopus_curwin[win]->last;
        }
    }
}

/* Clears the contents of all unshown buffers */

void clearbuffers()
{
    int a,win;
    struct DirectoryWindow *dir;

    busy();
    for (win=0;win<2;win++) {
        dir=dopus_firstwin[win];
        for (a=0;a<data_buffer_count[win];a++) {
            if (dopus_curwin[win]!=dir) {
                freedir(dir,-1);
                dir->directory[0]=0;
            }
            dir=dir->next;
        }
    }
    unbusy();
    dostatustext(globstring[STR_BUFFERS_CLEARED]);
}

/* Searches backwards for a named buffer and moves to it if found */

struct DirectoryWindow *findbuffer(dirbuf,win,canchecklocks,onlyreturn)
char *dirbuf;
int win,canchecklocks,onlyreturn;
{
    struct FileInfoBlock __aligned fblock;
    int a,founddir=0,ret=0,try,checklocks=0;
    struct DirectoryWindow *dir;
    BPTR lock=0,testlock;
    char tempbuf[300];

//D(bug("findbuffer()\n"));
    if (status_iconified) return(NULL);
    dir=dopus_curwin[win];
    if (dir->flags & DWF_ARCHIVE) return NULL;
    main_proc->pr_WindowPtr=(APTR)-1;

    strcpy(tempbuf,dirbuf);

    if (config->dirflags&DIRFLAGS_EXPANDPATHS) {
        canchecklocks=0;
        expand_path(dirbuf,tempbuf);
    }

    TackOn(tempbuf,NULL,300);

    for (try=0;try<2;try++) {
        if (checklocks && !(lock=Lock(tempbuf,ACCESS_READ))) break;

        for (a=0;a<data_buffer_count[win];a++,dir=dir->last) {
            if (dir->directory[0]) {
                if (checklocks &&
                    (testlock=Lock(dir->directory,ACCESS_READ))) {
                    if (CompareLock(lock,testlock)==LOCK_SAME) founddir=1;
                    UnLock(testlock);
                    if (!founddir) continue;
                }
                if (founddir || (Stricmp(tempbuf,dir->directory))==0) {
                    if (!(lockandexamine(tempbuf,&fblock)) ||
                        (CompareDates(&fblock.fib_Date,&dir->dirstamp)!=0)) continue;
                    if (!onlyreturn) go_to_buffer(win,dir);
                    ret=1;
                    break;
                }
            }
        }
        if (ret) break;
        if (canchecklocks) checklocks=1;
        else break;
    }
    UnLock(lock);
    if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
    return((ret)?dir:NULL);
}

/* Checks all buffers for a pathname and changes it to the new
   pathname if found */

void renamebuffers(old,new)
char *old,*new;
{
    int a,win;
    struct DirectoryWindow *dir;

    for (win=0;win<2;win++) {
        dir=dopus_firstwin[win];
        for (a=0;a<data_buffer_count[win];a++) {
            if (replacepart(dir->directory,old,new) && dopus_curwin[win]==dir) {
                strcpy(str_pathbuffer[win],dir->directory);
                checkdir(str_pathbuffer[win],&path_strgadget[win]);
            }
            dir=dir->next;
        }
    }
}

int replacepart(string,old,new)
char *string,*old,*new;
{
    int oldlen,stringlen;
    char tempbuf[256];

    oldlen=strlen(old);
    if (Strnicmp(string,old,oldlen)) return(0);
    if ((stringlen=strlen(string))>oldlen) {
        stringlen-=oldlen;
        strncpy(tempbuf,&string[oldlen],stringlen);
        tempbuf[stringlen]=0;
    }
    else stringlen=0;
    strcpy(string,new);
    if (stringlen>0) StrConcat(string,tempbuf,256);
    return(1);
}

/* Generates a list of all available buffers in a window */

void dolistbuffers(destwin)
int destwin;
{
    int a,b,c,win;
    struct DateStamp ds;
    struct DirectoryWindow *dir;
    char name[2];

    if (status_iconified) return;
    makespecialdir(destwin,globstring[STR_BUFFER_LIST_TITLE]);

    busy(); b=0/*scrdata_dispwin_nchars[destwin]*/; name[0]=0;
    DateStamp(&ds);
    for (win=0;win<2;win++) {
        dir=dopus_firstwin[win];
        for (a=0;a<data_buffer_count[win];a++) {
            if (dir->directory[0]) {
                addfile(dopus_curwin[destwin],destwin,name,0,
                    ENTRY_CUSTOM,&ds,dir->directory,0,
                    CUSTOMENTRY_BUFFERLIST,FALSE,dir->directory,NULL,0,0);
                if ((c=strlen(dir->directory))>b) b=c;
//D(bug("dolistbuffers: %ld - (%s)\n",c,dir->directory));
            }
            dir=dir->next;
        }
    }
    b*=scrdata_font_xsize;
    b+=2;
    if (b<scrdata_dispwin_nchars[destwin]) b=scrdata_dispwin_nchars[destwin];
    dopus_curwin[destwin]->hlen=b;
D(bug("dolistbuffers: %ld, %ld\n",dopus_curwin[destwin]->hlen,scrdata_dispwin_nchars[destwin]));
    refreshwindow(destwin,3);
    unbusy();
    okay();
}

/* Check if buffer is a special buffer, and return to normal if so */

int checkcurrentbuffer(win)
int win;
{
    if (dopus_curwin[win]==dopus_specialwin[win]) {
        dopus_curwin[win]=dopus_oldwin[win];
        return(1);
    }
    return(0);
}

/* Send message to custom entry handler */

void userentrymessage(dir,entry,type)
struct DirectoryWindow *dir;
struct Directory *entry;
int type;
{
    struct RexxMsg *msg,*reply;
    struct MsgPort *port;
    int fail=0;

    /* If no customhandler installed or rexx library is not there, return */
    if (!dir->custhandler[0] || !RexxSysBase) return;

    /* Attempt to create the message packet */
    if (!(msg=CreateRexxMsg(general_port,NULL,str_arexx_portname))) fail=1;
    else {
        /* Set message type (and argument count of 4) */
        msg->rm_Action=RXFUNC|4;

        /* Fill out argument pointers.

           Arg0   = operation type
           Arg1   = entry number
           Arg2   = entry text
           Arg3   = userdata          */

        msg->rm_Args[0]=(IPTR)type;
        msg->rm_Args[1]=(IPTR)entry->name;
        msg->rm_Args[2]=(IPTR)entry->comment;
        msg->rm_Args[3]=(IPTR)entry->protection;

        /* Fill in the message correctly.
           Arguments are integer/string/string/integer
                        1       0      0      1       = 9 */

        if (!(FillRexxMsg(msg,4,9))) fail=1;
    }

    /* If we failed to create the message properly, release it and return */
    if (fail) {
        if (msg) DeleteRexxMsg(msg);
        doerror(ERROR_NO_FREE_STORE);
        return;
    }

    /* Forbid() to find the port */
    Forbid();
    if (!(port=FindPort(dir->custhandler))) {
        char message[200];

        /* Port was not out there; put up an error requester */
        lsprintf(message,globstring[STR_CUSTPORT_NOT_FOUND],dir->custhandler);
        simplerequest(message,str_okaystring,NULL);

        /* Clear message and return */
        ClearRexxMsg(msg,4);
        DeleteRexxMsg(msg);
        Permit();
        return;
    }

    /* Turn busy pointer on and send the message */
    busy();
    PutMsg(port,(struct Message *)msg);

    Permit();
    SetSignal(0,INPUTSIG_ABORT);

    FOREVER {
        /* If abort sequence hit, break out immediately. The message is now
           lost to us, we can never free it */
        if ((Wait(1<<general_port->mp_SigBit|INPUTSIG_ABORT))&INPUTSIG_ABORT) {
            status_haveaborted=status_justabort=0;
            break;
        }

        /* If message has been replied, free it and break out */
        if (reply=(struct RexxMsg *)GetMsg(general_port)) {
            ClearRexxMsg(msg,4);
            DeleteRexxMsg(msg);
            break;
        }
    }
    unbusy();
    okay();
}

/* Opens the "special" dir in the window for a custom list */

void makespecialdir(win,title)
int win;
char *title;
{
    freedir(dopus_specialwin[win],win);
    if (dopus_curwin[win]!=dopus_specialwin[win]) {
        dopus_oldwin[win]=dopus_curwin[win];
        dopus_curwin[win]=dopus_specialwin[win];
    }
    strcpy(dopus_curwin[win]->diskname,title);
    str_pathbuffer[win][0]=0;
    checkdir(str_pathbuffer[win],&path_strgadget[win]);
    freedir(dopus_curwin[win],win);
#ifdef __SASC__
    dopus_curwin[win]->diskfree = i64_atoi( "-1" );
    dopus_curwin[win]->disktot = i64_atoi( "-1" );
    dopus_curwin[win]->diskblock = -1;
#else
    dopus_curwin[win]->diskfree=dopus_curwin[win]->disktot=dopus_curwin[win]->diskblock=-1;
#endif
}

/* Checks buffers to see if they need re-reading */

void check_old_buffer(win)
int win;
{
    int reread=0;

    if (config->dirflags&DIRFLAGS_REREADOLD && dopus_curwin[win]->directory[0]) {
        if (dopus_curwin[win]->firstentry &&
            ((dopus_curwin[win]->firstentry->type==ENTRY_CUSTOM) ||
            (dopus_curwin[win]->firstentry->type==ENTRY_DEVICE) ||
            (dopus_curwin[win]->flags & DWF_ARCHIVE))) return;
        else {
            struct FileInfoBlock __aligned testinfo;

            main_proc->pr_WindowPtr=(APTR)-1;
            if (lockandexamine(dopus_curwin[win]->directory,&testinfo)) {
                if (CompareDate(&dopus_curwin[win]->dirstamp,&testinfo.fib_Date)<0)
                    reread=1;
                else if (!(config->dirflags&DIRFLAGS_EXPANDPATHS) &&
                    dopus_curwin[win]->volumename[0]) {

                    char rootname[256];

                    strcpy(rootname,dopus_curwin[win]->directory);
                    if (getroot(rootname,NULL) &&
                        (strcmp(rootname,dopus_curwin[win]->volumename))!=0)
                        reread=1;
                }
            }
            if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
        }
        if (reread) {
            struct DirWindowPars notifypars;

            notifypars.reselection_list=NULL;
            makereselect(&notifypars,win);
            startgetdir(win,SGDFLAGS_REREADINGOLD);
            doreselect(&notifypars,0);
            makereselect(&notifypars,-1);
        }
    }
}

/* Refresh the display of a window */

void refreshwindow(win,type)
int win,type;
{
    if (win>-1) {
        if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
            dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
        if (dopus_curwin[win]->offset<0)
            dopus_curwin[win]->offset=0;

        if (type&1) {
            fixprop(win);
            fixhorizprop(win);
        }
        if (type&2) displayname(win,1);
        dopus_curwin[win]->oldoff=dopus_curwin[win]->oldhoff=-1;
        displaydir(win);
    }
}

/* Move to a buffer */

void go_to_buffer(win,dir)
int win;
struct DirectoryWindow *dir;
{
    endnotify(win);
    dopus_curwin[win]=dir;
    strcpy(str_pathbuffer[win],dir->directory);
    checkdir(str_pathbuffer[win],&path_strgadget[win]);
    checksize(win);
    check_old_buffer(win);
    refreshwindow(win,1);
    doselinfo(win);
    startnotify(win);
}
