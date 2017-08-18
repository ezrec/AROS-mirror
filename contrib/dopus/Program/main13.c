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

#ifdef __SASC__
#include <i64.h>
#endif

void seedate(ds,date,pad)
struct DateStamp *ds;
char *date;
int pad;
{
    char timebuf[16],datebuf[16]/*,buf[40]*/;
    struct DOpusDateTime dt;

    copy_datestamp(ds,&dt.dat_Stamp);
    initdatetime(&dt,datebuf,timebuf,1);

    lsprintf(date,"%s %s",datebuf,timebuf);
//D(bug("date: %s, time: %s\n",datebuf,timebuf));
/*
    strncpy(buf,datebuf,9);
    buf[9]=0;
    strcat(buf," ");
    strcat(buf,timebuf);
    buf[19]=0;

    if (pad) {
        if (config->dateformat&DATE_12HOUR) lsprintf(date,"%19s",buf);
        else lsprintf(date,"%18s",buf);
    }
    else strcpy(date,buf);
*/
}

int setdate(name,date)
char *name;
struct DateStamp *date;
{
/*    char __aligned bstrname[FILEBUF_SIZE+4];
    struct MsgPort *handler;
    ULONG arg[4];
    BPTR lock,parentlock;
    APTR wsave;
*/
    int rc=/*0*/SetFileDate(name,date);
/*
    wsave=main_proc->pr_WindowPtr;
    main_proc->pr_WindowPtr=(APTR)-1;

    if (handler=(struct MsgPort *)DeviceProc(name)) {
        if (lock=Lock(name,ACCESS_READ)) {
            parentlock=ParentDir(lock);
            UnLock(lock);

            strcpy(&bstrname[1],FilePart(name));
            bstrname[0]=strlen(&bstrname[1]);

            arg[0]=0;
            arg[1]=(IPTR)parentlock;
            arg[2]=(IPTR)MKBADDR(bstrname);
            arg[3]=(IPTR)date;
            rc=SendPacket(handler,ACTION_SET_DATE,arg,4);
            UnLock(parentlock);
        }
    }

    main_proc->pr_WindowPtr=wsave;
*/
    return(rc?1:IoErr());
}

void seename(win)
int win;
{
    long long tot;
    int bl,a;
    static char buf[256];

    if (win<0 || status_iconified) return;
    if (!dopus_curwin[win]->firstentry ||
        (dopus_curwin[win]->firstentry->type!=ENTRY_DEVICE &&
        dopus_curwin[win]->firstentry->type!=ENTRY_CUSTOM)) {
        if (str_pathbuffer[win][0]==0) {
            if (win==data_active_window)
                SetAPen(main_rp,screen_pens[config->disknameselbg].pen);
            else SetAPen(main_rp,screen_pens[config->disknamebg].pen);
            rectfill(main_rp,
                scrdata_diskname_xpos[win]+2,scrdata_diskname_ypos,
                scrdata_diskname_width[win],scrdata_diskname_height-2);
            SetAPen(main_rp,screen_pens[1].pen);
            return;
        }
        else {
            if (dopus_curwin[win]->flags & DWF_ARCHIVE)
             {
              if (dopus_curwin[win]->arcname)
               {
                char *c;

//D(bug("path: %s",dopus_curwin[win]->arcname));
                c = strstr(dopus_curwin[win]->directory,FilePart(dopus_curwin[win]->arcname));
                if (c) for (a = 0; c && (*c != '/') && (a < 31); c++) dopus_curwin[win]->diskname[a++] = *c;
                dopus_curwin[win]->diskname[a] = 0;
                displayname(win,1);
                return;
               }
             }
            strcpy(buf,str_pathbuffer[win]);
            dopus_curwin[win]->diskname[0]=0;
            main_proc->pr_WindowPtr=(APTR)-1;
            if (!(a=getroot(buf,NULL))) {
                strcpy(dopus_curwin[win]->diskname,globstring[STR_DIR_NOT_AVAILABLE_TITLE]);
#ifdef __SASC__
                dopus_curwin[win]->disktot = i64_atoi( "-1" );
                dopus_curwin[win]->diskfree = i64_atoi( "-1" );
                dopus_curwin[win]->diskblock = -1;
#else
                dopus_curwin[win]->disktot=dopus_curwin[win]->diskfree=dopus_curwin[win]->diskblock=-1;
#endif
                displayname(win,1);
                if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
                return;
            }
            strcpy(dopus_curwin[win]->diskname,buf);
/* */
            strcpy(dopus_curwin[win]->volumename,buf);
/* */
            dopus_curwin[win]->diskfree=bytes(str_pathbuffer[win],&tot,&bl);
            dopus_curwin[win]->disktot=tot;
            dopus_curwin[win]->diskblock=bl;
            if (a==ID_WRITE_PROTECTED) dopus_curwin[win]->flags|=DWF_READONLY;
            else dopus_curwin[win]->flags&=~DWF_READONLY;
            if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
        }
    }
    displayname(win,0);
}

void displayname(win,clear)
int win,clear;
{
#ifdef __SASC__
    bigint free, tot, temp1;
#else
    long long free, tot;
#endif
    int b,x,nn=0,len,len2,len3,x1,cx1,cx2;
    static char buf[30],buf2[80],buf3[20];
    double pct;

    if (status_iconified || win<0) return;
    if (str_pathbuffer[win][0]==0) {
#ifdef __SASC__
        bigint temp1;

        temp1 = i64_atoi( "-1" );

        if ( i64_cmp( dopus_curwin[win]->disktot, temp1 ) != I64_EQUAL &&
#else
        if (dopus_curwin[win]->disktot!=-1 &&
#endif
            (!dopus_curwin[win]->firstentry || dopus_curwin[win]->firstentry->type!=ENTRY_DEVICE)) nn=2;
        else nn=1;
    }
    if (nn!=2) {
        free=dopus_curwin[win]->diskfree; tot=dopus_curwin[win]->disktot;
        SetDrMd(main_rp,JAM2);

        StrCombine(buf2,dopus_curwin[win]->diskname,str_space_string,80);
        SetFont(main_rp,scr_font[FONT_NAMES]);

        len=30;
        x1=dotextlength(main_rp,buf2,&len,scrdata_diskname_width[win]-4);

#ifdef __SASC__
                temp1 = i64_atoi( "-1" );
        if (!nn && ( i64_cmp( tot, temp1 ) == I64_GREATER )) {
#else
        if (!nn && tot>-1) {
#endif
            if (config->showfree&SHOWFREE_BYTES || config->showfree==0) {
#ifdef __SASC__
                char buf1[30];

                i64_itoa( free, buf1, 30 );
                if (dopus_curwin[win]->flags&DWF_READONLY)
                    lsprintf(buf," (%s)",buf1);
                else lsprintf(buf," %s",buf1);
#else
                if (dopus_curwin[win]->flags&DWF_READONLY) sprintf(buf," (%qd)",free);
                else sprintf(buf," %qd",free);
#endif
            }
            else if (config->showfree&SHOWFREE_KILO) {
                getsizestring(buf,free);
                if (dopus_curwin[win]->flags&DWF_READONLY)
                {
                  char buf1[30];

                  lsprintf(buf1,"(%s)",buf);
                  strcpy(buf,buf1);
                }
            }
            else if (config->showfree&SHOWFREE_BLOCKS) {
                if (dopus_curwin[win]->flags&DWF_READONLY)
                    lsprintf(buf," (%ld)",(long int)dopus_curwin[win]->diskblock);
                else lsprintf(buf," %ld",(long int)dopus_curwin[win]->diskblock);
            }
            else if (config->showfree&SHOWFREE_PERCENT) {
#ifdef __SASC__
                                temp1 = i64_atoi( "0" );

                if ( i64_cmp( tot, temp1 ) == I64_EQUAL ) b=100;
                else {
                    pct=free.lo; pct*=100; // HUX ??
                    b=(int)(pct/(double)tot.lo); // HUX ??
                }
#else
                if (tot==0) b=100;
                else {
                    pct=free; pct*=100;
                    b=(int)(pct/(double)tot);
                }
#endif
                if (b>100) b=100;
                if (dopus_curwin[win]->flags&DWF_READONLY)
                    lsprintf(buf," (%ld%%)",(long int)b);
                else lsprintf(buf," %ld%%",(long int)b);
            }
            StrCombine(buf3,buf,str_space_string,14);
            len2=12; len3=strlen(buf);
            FOREVER {
                x=dotextlength(main_rp,buf3,&len2,scrdata_diskname_width[win]-x1-4);
                if (len2>=len3 || (--len)==0) break;
                len2=12; x1=dotextlength(main_rp,buf2,&len,scrdata_diskname_width[win]-4);
            }
            strncpy(buf3,str_space_string,13);
            strcpy(&buf3[len2-len3],buf);
        }
        else x=0;
    }

    if (clear) {
        if (win==data_active_window)
            SetAPen(main_rp,screen_pens[config->disknameselbg].pen);
        else SetAPen(main_rp,screen_pens[config->disknamebg].pen);
        rectfill(main_rp,
            scrdata_diskname_xpos[win]+2,
            scrdata_diskname_ypos,
            scrdata_diskname_width[win],
            scrdata_diskname_height-2);
    }

    if (nn!=2) {
        if (win==data_active_window) {
            SetAPen(main_rp,screen_pens[config->disknameselfg].pen);
            SetBPen(main_rp,screen_pens[config->disknameselbg].pen);
        }
        else {
            SetAPen(main_rp,screen_pens[config->disknamefg].pen);
            SetBPen(main_rp,screen_pens[config->disknamebg].pen);
        }

        Move(main_rp,
            scrdata_diskname_xpos[win]+4,
            scrdata_diskname_ypos+scr_font[FONT_NAMES]->tf_Baseline);
        Text(main_rp,buf2,len);

        cx1=main_rp->cp_x;
        if (x) {
            x1=(scrdata_diskname_xpos[win]+scrdata_diskname_width[win])-x;
            if (x1<scrdata_diskname_xpos[win]+2) x1=scrdata_diskname_xpos[win]+2;
            cx2=x1-1;
            Move(main_rp,x1,scrdata_diskname_ypos+scr_font[FONT_NAMES]->tf_Baseline);
            Text(main_rp,buf3,len2);
        }
        else cx2=scrdata_diskname_xpos[win]+scrdata_diskname_width[win]+1;
        if (!clear && cx1<=cx2) {
            SetAPen(main_rp,main_rp->BgPen);
            RectFill(main_rp,cx1,scrdata_diskname_ypos,cx2,scrdata_diskname_height+scrdata_diskname_ypos-3);
        }
        SetFont(main_rp,scr_font[FONT_GENERAL]);
    }
}

void relabel_disk(rexx,path)
int rexx;
char *path;
{
//    struct MsgPort *port;
    char oldname[36],name[36]/*,*bstr*/;
//    ULONG arg;
    char buf[256];
//    int a;

    strcpy(buf,rexx?rexx_args[0]:path);
D(bug("relabel(%ld,%s)\n",rexx,buf));
    if (!(getroot(buf,NULL))) {
        doerror(-1);
        return;
    }
    strcat(buf,":");
//    if (!(port=(struct MsgPort *) DeviceProc(buf))) return;
    getroot(buf,NULL);

    strcpy(name,buf); strcpy(oldname,name);
    if (!rexx) {
        if (!(whatsit(globstring[STR_ENTER_NEW_DISK_NAME],30,name,NULL))) return;
    }
    else strcpy(name,rexx_args[1]);

    if (name[strlen(name)-1] == ':') name[strlen(name)-1] = 0;

    strcat(buf,":");
D(bug("Relabel(%s,%s)\n",buf,name));
    if (! Relabel(buf,name)) doerror(-1);
/*
    a=strlen(name);
    bstr=(char *) AllocMem(a+2,MEMF_CLEAR))
    bstr[0]=(char)a;
    strcpy(bstr+1,name);
    arg=(IPTR)MKBADDR(bstr);
    if (!(SendPacket(port,ACTION_RENAME_DISK,&arg,1))) doerror(-1);
*/
    else if ((!status_iconified) && (dopus_curwin[data_active_window] != dopus_specialwin[data_active_window])) {
        if ((Strnicmp(str_pathbuffer[data_active_window],oldname,strlen(oldname)))==0 &&
            str_pathbuffer[data_active_window][strlen(oldname)]==':') {
            strcpy(buf,name);
            StrConcat(buf,&str_pathbuffer[data_active_window][strlen(oldname)],256);
            strcpy(str_pathbuffer[data_active_window],buf);
            checkdir(str_pathbuffer[data_active_window],&path_strgadget[data_active_window]);
            strcpy(dopus_curwin[data_active_window]->directory,str_pathbuffer[data_active_window]);
        }
        seename(data_active_window);
    }
//  FreeMem(bstr,a+2);
}

int getroot(name,ds)
char *name;
struct DateStamp *ds;
{
    struct InfoData __aligned info;
    BPTR lock1;
    struct FileLock *lock2;
    char *p;
    struct DeviceList *dl;
    int a;

    if (!(lock1=Lock(name,ACCESS_READ))) return(0);
    lock2=(struct FileLock *) BADDR(lock1);
    for (a=0;a<FILEBUF_SIZE;a++) name[a]=0;
    dl=(struct DeviceList *)BADDR(lock2->fl_Volume);
    p=(char *) BADDR(dl->dl_Name);
#ifdef __AROS__
    if (p) LStrnCpy(name,AROS_BSTR_ADDR(p),AROS_BSTR_strlen(p));
#else
    if (p) LStrnCpy(name,p+1,*p);
#endif
    if (ds) CopyMem((char *)&dl->dl_VolumeDate,(char *)ds,sizeof(struct DateStamp));
    Info(lock1,&info);
    UnLock(lock1);
    return(info.id_DiskState);
}

BPTR getrootlock(lock1)
BPTR lock1;
{
    BPTR lock2;

    while ((lock2=ParentDir(lock1))) {
        UnLock(lock1);
        lock1=lock2;
    }
    return(lock1);
}

void strtostamp(date,time,ds)
char *date,*time;
struct DateStamp *ds;
{
    struct DOpusDateTime datetime;
/*
    datetime.dat_Stamp.ds_Days = 0;
    datetime.dat_Stamp.ds_Minute = 0;
    datetime.dat_Stamp.ds_Tick = 0;
*/
    datetime.dat_Format=dateformat(config->dateformat);
    datetime.dat_Flags=0/*DDTF_SUBST|DDTF_CUSTOM*/;
    datetime.dat_StrDate=date;
    datetime.dat_StrTime=time;
//    datetime.custom_months=date_months;
//    datetime.custom_shortmonths=date_shortmonths;
//    datetime.custom_weekdays=date_weekdays;
//    datetime.custom_shortweekdays=date_shortweekdays;
//    datetime.custom_special_days=date_special;
//    StrToStamp(&datetime);
    StrToDate((struct DateTime *)&datetime);
//    D(bug("strtostamp(%s,%s)=%ld\n",date,time,StrToDate((struct DateTime *)&datetime)));
    copy_datestamp(&datetime.dat_Stamp,ds);
}
