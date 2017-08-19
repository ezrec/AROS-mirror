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

static unsigned char displenmap[]={DISPLAY_NAME,DISPLAY_COMMENT,DISPLAY_FILETYPE,DISPLAY_OWNER,DISPLAY_GROUP};

int rexxdisp(msg,cmd,command)
struct RexxMsg *msg;
struct CommandList *cmd;
char *command;
{
    int a,b,c,f,val,function,win,statval,retval;
    long long d;
    char buf[520],*buf1,pad[8],buf2[10],*buf3,*ptr,*retbuf;
    unsigned char ch;
    BPTR lock;
    struct DateStamp ds;
    struct dopusgadgetbanks *bank;
    struct Directory *entry;
    unsigned char cval;
    struct dopusfiletype *ftype;
    struct DirectoryWindow *dirwin;

    buf[0]=0;
    if (cmd) func_global_function=cmd->function;
    for (a=0;a<16;a++) {
        rexx_args[a][0]=0;
        rexx_arg_value[a]=0;
    }

    if (!command || !command[0]) rexx_argcount=0;
    else rexx_argcount=parse(command);

    for (a=0;a<rexx_argcount;a++) {
        for (b=0;b<256;b++) {
            if ((ch=rexx_args[a][b])==0) break;
            if ((unsigned char)ch==160) rexx_args[a][b]=' ';
        }
    }

    if (rexx_argcount==0 && func_global_function!=FUNC_PRINT) rexx_global_flag=0;
    else {
        rexx_global_flag=1;
        switch (func_global_function) {
            case FUNC_ERRORHELP:
                rexx_arg_value[0]=atoi(rexx_args[0]);
                return(0);
            case FUNC_PROTECT:
                rexx_arg_value[0]=getprotval(rexx_args[1]);
                if (rexx_argcount>2) rexx_arg_value[1]=getprotval(rexx_args[2])^15;
                else rexx_arg_value[1]=0;
                return(0);
            case FUNC_SELECT:
                rexx_arg_value[0]=1;
                for (a=0;a<rexx_argcount;a++) {
                    if ((Stricmp(rexx_args[a],"onlyfiles"))==0) rexx_arg_value[0]=2;
                    else if ((Stricmp(rexx_args[a],"onlydirs"))==0) rexx_arg_value[0]=3;
                    else if ((Stricmp(rexx_args[a],"name"))==0) rexx_arg_value[1]=0;
                    else if ((Stricmp(rexx_args[a],"date"))==0) rexx_arg_value[1]=1;
                    else if ((Stricmp(rexx_args[a],"protection"))==0) rexx_arg_value[1]=2;
                }
                return(0);
            case FUNC_SEARCH:
                if (rexx_argcount>2) {
                    search_flags=SEARCH_NOCASE;
                    for (a=2;a<rexx_argcount;a++) {
                        if ((Stricmp(rexx_args[a],"ucnlc"))==0) search_flags&=~SEARCH_NOCASE;
                        else if ((Stricmp(rexx_args[a],"wild"))==0) search_flags|=SEARCH_WILDCARD;
                        else if ((Stricmp(rexx_args[a],"byword"))==0) search_flags|=SEARCH_ONLYWORDS;
                    }
                }
                return(0);
        }
    }

    function=func_global_function;
    func_global_function=0;
    switch (function) {
        case FUNC_SETVAR:
/*            if (!system_version2) {
                rexx_set_return(msg,10,NULL);
            }
            else*/ if (rexx_argcount<2) {
                rexx_set_return(msg,20,NULL);
            }
            else {
                int a;

                buf[0]=0;
                for (a=1;a<rexx_argcount;a++) {
                    StrConcat(buf,rexx_args[a],520);
                    if (a<rexx_argcount-1) StrConcat(buf," ",520);
                }

                if (SetVar(rexx_args[0],buf,-1,GVF_LOCAL_ONLY)) {
                    rexx_set_return(msg,0,NULL);
                }
                else {
                    rexx_set_return(msg,IoErr(),NULL);
                }
            }
            break;

        case FUNC_DEVICELIST:
            if (rexx_argcount<1 ||
                (win=atoi(rexx_args[0]))<0 || win>1)
                win=data_active_window;
            dodevicelist(win);
            break;

        case FUNC_DISKINFO:
            if (rexx_argcount<1) ptr=str_pathbuffer[data_active_window];
            else ptr=rexx_args[0];
            dodiskinfo(ptr);
            break;

        case FUNC_VERIFY:
        case FUNC_NOTIFY:
            if (rexx_argcount) {
                ++command;
                do_title_string(command,buf,1,NULL);
                command=buf;
            }
            else if (function==FUNC_VERIFY) command=globstring[STR_REALLY_PROCEED];
            else break;

            if (!(simplerequest(command,
                (function==FUNC_VERIFY)?str_okaystring:globstring[STR_CONTINUE],
                (function==FUNC_VERIFY)?str_cancelstring:NULL,
                NULL))) {
                status_flags|=STATUS_VERIFYFAIL;
                myabort();
                a=0;
            }
            else a=1;
            rexx_return(msg,a);
            return(1);

        case FUNC_CHECKABORT:
            rexx_return(msg,status_rexxabort);
            status_rexxabort=0;
            return(1);

        case FUNC_SCANDIR:
            if (rexx_argcount>0) {
                if (rexx_argcount<2 || (win=atoi(rexx_args[1]))<0 || win>1)
                    win=data_active_window;
                strcpy(str_pathbuffer[win],rexx_args[0]);
                if (!status_iconified) startgetdir(win,SGDFLAGS_CANMOVEEMPTY|SGDFLAGS_CANCHECKBUFS);
            }
            break;

        case FUNC_CLEARWINDOW:
            if (rexx_argcount<1 || (win=atoi(rexx_args[0]))<0 || win>1)
                win=data_active_window;
            checkcurrentbuffer(win);
            str_pathbuffer[win][0]=0;
            dopus_curwin[win]->directory[0]=0;
            checkdir(str_pathbuffer[win],&path_strgadget[win]);
            freedir(dopus_curwin[win],win);
            displaydir(win);
            seename(win);
            break;

        case FUNC_GETSTRING:
            buf[0]=0;
            if (rexx_argcount==0) command=globstring[STR_ENTER_A_STRING];
            else {
                command=rexx_args[0];
                if (rexx_argcount>1) strcpy(buf,rexx_args[1]);
            }
            if (!(whatsit(command,256,buf,NULL))) rexx_set_return(msg,1,NULL);
            else rexx_set_return(msg,0,buf);
            if (!status_iconified) unbusy();
            return(1);

        case FUNC_LOADCONFIG:
            if (rexx_argcount>0 && checknewconfig() && checkwindowquit()) {
                busy();
                dostatustext(globstring[STR_READING_CONFIG]);
                shutthingsdown(0);
                strcpy(str_config_basename,rexx_args[0]);
                if ((ptr=strstri(BaseName(str_config_basename),".CFG"))) *ptr=0;
                read_configuration(0);
                SetUp(0);
                okay();
            }
            else a=0;
            rexx_return(msg,a);
            return(1);

        case FUNC_LOADSTRINGFILE:
/*            if (rexx_argcount>0 && (CheckExist(rexx_args[0],NULL))<0) {
                busy();
                dostatustext(globstring[STR_READING_CONFIG]);
                readstrings(rexx_args[0]);
                okay();
                a=1;
            }
            else*/ a=0;
            rexx_return(msg,a);
            return(1);

        case FUNC_REQUEST:
            if (rexx_argcount==0) command=globstring[STR_PLEASE_CHOOSE];
            else ++command;
            rexx_return(msg,(simplerequest(command,str_okaystring,str_cancelstring,NULL)));
            if (!status_iconified) unbusy();
            return(1);

        case FUNC_DOPUSTOFRONT:
            dopustofront();
            break;

        case FUNC_DOPUSTOBACK:
            dopustoback();
            break;

        case FUNC_GETALL:
        case FUNC_GETFILES:
        case FUNC_GETDIRS:
        case FUNC_GETSELECTEDALL:
        case FUNC_GETSELECTEDFILES:
        case FUNC_GETSELECTEDDIRS:
            if (rexx_argcount<2 || (win=atoi(rexx_args[1]))<0 || win>1)
                win=data_active_window;
            if (rexx_argcount<1) {
                pad[0]=' ';
                pad[1]=0;
            }
            else {
                LStrnCpy(pad,rexx_args[0],7);
                pad[7]=0;
            }
            val=strlen(pad);
            a=d=0;
            switch (function) {
                case FUNC_GETSELECTEDALL:
                    function=FUNC_GETALL; d=1;
                    break;
                case FUNC_GETSELECTEDFILES:
                    function=FUNC_GETFILES; d=1;
                    break;
                case FUNC_GETSELECTEDDIRS:
                    function=FUNC_GETDIRS; d=1;
                    break;
            }
            entry=dopus_curwin[win]->firstentry;
            while (entry) {
                if ((function==FUNC_GETALL ||
                    (function==FUNC_GETFILES && entry->type<=ENTRY_FILE) ||
                    (function==FUNC_GETDIRS && entry->type>=ENTRY_DIRECTORY)) &&
                    (!d || entry->selected)) {
                    if (entry->type!=ENTRY_CUSTOM || entry->subtype!=CUSTOMENTRY_USER)
                        a+=strlen(entry->name)+val;
                    else if (entry->size&CUSTENTRY_CANSELECT) a+=9+val;
                }
                entry=entry->next;
            }
            if (a>0) {
                if ((buf1=AllocMem(a+2,MEMF_CLEAR))) {
                    entry=dopus_curwin[win]->firstentry;
                    while (entry) {
                        if ((function==FUNC_GETALL ||
                            (function==FUNC_GETFILES && entry->type<=ENTRY_FILE) ||
                            (function==FUNC_GETDIRS && entry->type>=ENTRY_DIRECTORY)) && 
                            (!d || entry->selected)) {
                            if (entry->type!=ENTRY_CUSTOM || entry->subtype!=CUSTOMENTRY_USER ||
                                entry->size&CUSTENTRY_CANSELECT) {
                                strcat(buf1,entry->name);
                                strcat(buf1,pad);
                            }
                        }
                        entry=entry->next;
                    }
                    rexx_set_return(msg,0,buf1);
                    FreeMem(buf1,a+2);
                }
                else rexx_set_return(msg,10,NULL);
            }
            else rexx_set_return(msg,0,NULL);
            return(1);

        case FUNC_GETNEXTSELECTED:
            if (rexx_argcount<1 || (win=atoi(rexx_args[0]))<0 || win>1)
                win=data_active_window;
            entry=dopus_curwin[win]->firstentry;
            while (entry) {
                if (entry->selected) {
                    ptr=NULL;
                    if (entry->type==ENTRY_CUSTOM) ptr = (entry->dispstr ? entry->dispstr : entry->comment);
                    if (!ptr) ptr = entry->name;
                    rexx_set_return(msg,0,ptr);
                    return(1);
                }
                entry=entry->next;
            }
            break;

        case FUNC_GETENTRY:
            a=atoi(rexx_args[0]);
            if (rexx_argcount<2 || (win=atoi(rexx_args[1]))<0 || win>1)
                win=data_active_window;
            ptr=NULL;
            entry=dopus_curwin[win]->firstentry;
            while (a && entry) {
                if (a==1) {
                    if (entry->type==ENTRY_CUSTOM) {
                        if (entry->dispstr) ptr=entry->dispstr;
                        else ptr=entry->comment;
                    }
                    if (!ptr) ptr=entry->name;
                    break;
                }
                --a;
                entry=entry->next;
            }
            rexx_set_return(msg,0,ptr?ptr:"");
            return(1);
            break;

        case FUNC_GETFILETYPE:
            if ((ftype=checkfiletype(rexx_args[0],-1,0))) {
                if (rexx_args[1][0]=='1') strcpy(buf,ftype->typeid);
                else strcpy(buf,ftype->type);
            }
            else buf[0]=0;
            rexx_set_return(msg,0,buf);
            return(1);

        case FUNC_OTHERWINDOW:
            makeactive(1-data_active_window,0);
            rexx_return(msg,data_active_window);
            global_swap_window=TRUE;
            return(1);

        case FUNC_STATUS:
            statval=atoi(rexx_args[0]);
            d=0; buf1=buf3=NULL;
            if ((Stricmp(rexx_args[1],"set"))==0) {
                f=1; c=0;
                val=atoi(rexx_args[2]); buf3=rexx_args[2];
            }
            else if ((Stricmp(rexx_args[2],"set"))==0) {
                c=atoi(rexx_args[1]);
                f=1;
                val=atoi(rexx_args[3]); buf3=rexx_args[3];
            }
            else {
                val=c=atoi(rexx_args[1]);
                f=0;
            }
            switch (statval) {
                case RXSTATUS_CURRENT_DIRECTORY:
                    if (f) {
                        if (!(lock=Lock(buf3,ACCESS_READ))) break;
                        UnLock(CurrentDir(lock));
                        break;
                    }
                    if (expand_path("",buf)) buf1=buf;
                    break;
                case RXSTATUS_VERSION:
                    buf1=str_version_string;
                    break;
                case RXSTATUS_ACTIVE_WINDOW:
                    if (f && !status_iconified) makeactive(val,0);
                    else d=data_active_window;
                    break;
                case RXSTATUS_NUMBER_OF_FILES:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->filetot;
                    break;
                case RXSTATUS_NUMBER_OF_DIRS:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->dirtot;
                    break;
                case RXSTATUS_NUMBER_OF_ENTRIES:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->total;
                    break;
                case RXSTATUS_NUMBER_SELECTED_FILES:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->filesel;
                    break;
                case RXSTATUS_NUMBER_SELECTED_DIRS:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->dirsel;
                    break;
                case RXSTATUS_NUMBER_SELECTED_ENTRIES:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->filesel+dopus_curwin[c]->dirsel;
                    break;
                case RXSTATUS_TOTAL_BYTES:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->bytestot;
                    break;
                case RXSTATUS_TOTAL_SELECTED_BYTES:
                    if (c<0 || c>1) c=data_active_window;
                    d=dopus_curwin[c]->bytessel;
                    break;
                case RXSTATUS_SELECT_PATTERN:
                    if (f) strcpy(str_select_pattern[c],buf3);
                    else buf1=str_select_pattern[c];
                    break;
                case RXSTATUS_DIRECTORY_NAME:
                    if (c<0 || c>1) c=data_active_window;
                    if (f) {
                        strcpy(str_pathbuffer[c],buf3);
                        if (!status_iconified) startgetdir(c,0);
                    }
                    else buf1=str_pathbuffer[c];
                    break;
                case RXSTATUS_DISK_NAME:
                    if (c<0 || c>1) c=data_active_window;
                    buf1=dopus_curwin[c]->diskname;
                    break;
                case RXSTATUS_DISK_FREE_BYTES:
                    if (c<0 || c>1) c=data_active_window;
#ifdef __SASC__
                    d=dopus_curwin[c]->diskfree.lo; // HUX ???
#else
                    d=dopus_curwin[c]->diskfree;
#endif
                    break;
                case RXSTATUS_DISK_TOTAL_BYTES:
                    if (c<0 || c>1) c=data_active_window;
#ifdef __SASC__
                    d=dopus_curwin[c]->disktot.lo; // HUX ???
#else
                    d=dopus_curwin[c]->disktot;
#endif
                    break;
                case RXSTATUS_BUFFERED_DIRECTORY_NAME:
                case RXSTATUS_BUFFERED_DISKNAME:
                case RXSTATUS_BUFFERED_DISK_FREE_BYTES:
                case RXSTATUS_BUFFERED_DISK_TOTAL_BYTES:
                    if (c<0 || c>1) c=data_active_window;
                    dirwin=dopus_firstwin[c];
                    b=atoi(rexx_args[2]);
                    for (a=0;a<b;a++) {
                        if (!dirwin) break;
                        dirwin=dirwin->next;
                    }
                    if (dirwin) {
                        switch (statval) {
                            case RXSTATUS_BUFFERED_DIRECTORY_NAME:
                                buf1=dirwin->directory;
                                break;
                            case RXSTATUS_BUFFERED_DISKNAME:
                                buf1=dirwin->diskname;
                                break;
                            case RXSTATUS_BUFFERED_DISK_FREE_BYTES:
#ifdef __SASC__
                                d=dirwin->diskfree.lo; // HUX ???
#else
                                d=dirwin->diskfree;
#endif
                                break;
                            case RXSTATUS_BUFFERED_DISK_TOTAL_BYTES:
#ifdef __SASC__
                                d=dirwin->disktot.lo; // HUX ???
#else
                                d=dirwin->disktot;
#endif
                                break;
                        }
                    }
                    break;
                case RXSTATUS_BUFFER_DISPLAYED:
                    if (c<0 || c>1) c=data_active_window;
                    dirwin=dopus_firstwin[c];
                    for (a=0;a<data_buffer_count[c];a++) {
                        if (!dirwin || dirwin==dopus_curwin[c]) break;
                        dirwin=dirwin->next;
                    }
                    if (dirwin) d=a;
                    else d=-1;
                    if (f && val>=0 && val<data_buffer_count[c]) {
                        for (a=0;a<val;a++) {
                            if (!dirwin) break;
                            dirwin=dirwin->next;
                        }
                        if (dirwin && !status_iconified) go_to_buffer(c,dirwin);
                    }
                    break;
                case RXSTATUS_DISPLAY_OFFSET:
                    if (c<0 || c>1) c=data_active_window;
                    if (f) {
                        dopus_curwin[c]->offset=val-1;
                        if (dopus_curwin[c]->offset<0) dopus_curwin[c]->offset=0;
                        else if (dopus_curwin[c]->offset>dopus_curwin[c]->total-scrdata_dispwin_lines)
                            dopus_curwin[c]->offset=dopus_curwin[c]->total-scrdata_dispwin_lines;
                        if (!status_iconified) {
                            fixvertprop(c);
                            displaydir(c);
                        }
                    }
                    else d=dopus_curwin[c]->offset+1;
                    break;
                case RXSTATUS_ENTRIES_PER_PAGE:
                    d=scrdata_dispwin_lines;
                    break;
                case RXSTATUS_CONFIGURATION_CHANGED:
                    if (f) config_changed=val;
                    else d=config_changed;
                    break;
                case RXSTATUS_OKAY_STRING:
                    if (f) { strcpy(str_okaystring,buf3); D(bug("buf3 = \"%s\"\n",buf3));}
                    else buf1=str_okaystring;
                    break;
                case RXSTATUS_CANCEL_STRING:
                    if (f) strcpy(str_cancelstring,buf3);
                    else buf1=str_cancelstring;
                    break;
                case RXSTATUS_ICONIFIED:
                    d=status_iconified;
                    break;
                case RXSTATUS_TOP_TEXT_JUSTIFY:
                    if (f) {
                        if (val>-1 && val<3) scrdata_statustext_pos=val;
                    }
                    else d=scrdata_statustext_pos;
                    break;
                case RXSTATUS_CONFIGURATION_ADDRESS:
                    lsprintf(buf,"%ld",(long int)config);
                    buf1=buf;
                    break;
                case RXSTATUS_FIRST_NODE:
                    if (f) {
                        if (c==0) dopus_firsttype=(struct dopusfiletype *)val;
                        else if (c==1) {
                            dopus_firstgadbank=(struct dopusgadgetbanks *)val;
                            dopus_curgadbank=dopus_firstgadbank;
                            data_gadgetrow_offset=0;
                        }
                        else if (c==2) dopus_firsthotkey=(struct dopushotkey *)val;
                    }
                    else {
                        if (c==0) lsprintf(buf,"%ld",(long int)dopus_firsttype);
                        else if (c==1) lsprintf(buf,"%ld",(long int)dopus_firstgadbank);
                        else if (c==2) lsprintf(buf,"%ld",(long int)dopus_firsthotkey);
                        buf1=buf;
                    }
                    break;
                case RXSTATUS_CURRENT_BANK_NUMBER:
                    bank=dopus_firstgadbank;
                    for (a=0;bank && bank!=dopus_curgadbank;a++) bank=bank->next;
                    if (bank!=dopus_curgadbank) a=-1;
                    lsprintf(buf,"%ld",(long int)a);
                    buf1=buf;
                    break;
            }
            if (buf1) rexx_set_return(msg,0,buf1);
            else rexx_return(msg,d);
            return(1);

        case FUNC_FILEINFO:
            if (rexx_argcount==0) break;
            if (rexx_argcount>1) pad[0]=rexx_args[1][0];
            else pad[0]=' ';
            if (rexx_argcount<3 || (win=atoi(rexx_args[2]))<0 || win>1)
                win=data_active_window;
            if ((entry=findfile(dopus_curwin[win],rexx_args[0],NULL))) {
                getprot(entry->protection,buf2);
                lsprintf(buf,"%s%lc%ld%lc%ld%lc%ld%lc%ld%lc%ld%lc%ld%lc%s%lc%s%lc",
                    entry->name,pad[0],(long int)entry->size,pad[0],(long int)entry->subtype,pad[0],
                    (long int)entry->type,pad[0],(long int)entry->selected,pad[0],
                    (long int)entry->date.ds_Days,pad[0],
                    (long int)(entry->date.ds_Minute*60)+(entry->date.ds_Tick/50),pad[0],
                    entry->comment,pad[0],buf2,pad[0]);
                rexx_set_return(msg,0,buf);
                return(1);
            }
            break;

        case FUNC_SCROLLV:
            if (status_iconified || rexx_argcount<1) break;
            if (rexx_argcount<2 ||
                (win=atoi(rexx_args[1]))<0 || win>1) win=data_active_window;

            dopus_curwin[win]->offset+=atoi(rexx_args[0]);
            if (dopus_curwin[win]->offset>
                (dopus_curwin[win]->total-scrdata_dispwin_lines)) {
                dopus_curwin[win]->offset=
                    dopus_curwin[win]->total-scrdata_dispwin_lines;
            }
            if (dopus_curwin[win]->offset<0) dopus_curwin[win]->offset=0;

            fixvertprop(win);
            displaydir(win);
            rexx_return(msg,dopus_curwin[win]->offset);
            return(1);

        case FUNC_SCROLLH:
            if (status_iconified || rexx_argcount<1) break;
            if (rexx_argcount<2 ||
                (win=atoi(rexx_args[1]))<0 || win>1) win=data_active_window;

            dopus_curwin[win]->hoffset+=atoi(rexx_args[0]);
            if (dopus_curwin[win]->hoffset>
                (dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win])) {
                dopus_curwin[win]->hoffset=
                    dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win];
            }
            if (dopus_curwin[win]->hoffset<0) dopus_curwin[win]->hoffset=0;

            refreshwindow(win,1);
            rexx_return(msg,dopus_curwin[win]->hoffset);
            return(1);

        case FUNC_SCROLLTOSHOW:
            if (status_iconified || rexx_argcount<1) break;
            if (rexx_argcount<2 ||
                (win=atoi(rexx_args[1]))<0 || win>1) win=data_active_window;

            if (!(entry=findfile(dopus_curwin[win],rexx_args[0],NULL))) {
                if (_isdigit(rexx_args[0][0])) {
                    entry=dopus_curwin[win]->firstentry;
                    a=atoi(rexx_args[0]);
                    while (a-- && entry) entry=entry->next;
                }
            }
            if (entry) displaydirgiven(win,entry,1);
            rexx_return(msg,(entry)?1:0);
            return(1);

        case FUNC_NEXTDRIVES:
            nextdrives();
            break;

        case FUNC_PATTERNMATCH:
            if (rexx_argcount<2) break;
            LParsePatternI(rexx_args[0],buf);
            rexx_return(msg,LMatchPatternI(buf,rexx_args[1]));
            return(1);

        case FUNC_BUSY:
            if (rexx_argcount<1 || status_iconified) break;
            if ((Strnicmp(rexx_args[0],"on",2))==0) busy();
            else if ((Strnicmp(rexx_args[0],"off",3))==0) unbusy();
            break;

        case FUNC_SELECTENTRY:
            if (rexx_argcount<1) break;
            entry=dopus_curwin[data_active_window]->firstentry;
            b=atoi(rexx_args[0]);
            for (a=0;a<b;a++) {
                if (!entry) break;
                entry=entry->next;
            }
            if (!entry ||
                (entry->type==ENTRY_CUSTOM && entry->subtype==CUSTOMENTRY_USER &&
                !(entry->size&CUSTENTRY_CANSELECT))) {
                rexx_return(msg,-1);
                return(1);
            }
            goto doentryselection;
        case FUNC_SELECTFILE:
            if (status_iconified) strcpy(rexx_args[2],"0");
            if (!(entry=findfile(dopus_curwin[data_active_window],rexx_args[0],NULL))) d=-1;
            else {
doentryselection:
                d=entry->selected;
                if (entry->selected) {
                    if (entry->type>=ENTRY_DIRECTORY) {
                        --dopus_curwin[data_active_window]->dirsel;
                        if (entry->size!=-1) dopus_curwin[data_active_window]->bytessel-=entry->size;
                    }
                    else {
                        --dopus_curwin[data_active_window]->filesel;
                        dopus_curwin[data_active_window]->bytessel-=entry->size;
                    }
                }
                if (rexx_argcount>1) a=atoi(rexx_args[1]);
                else a=-1;
                if (a==-1) a=1-entry->selected;
                entry->selected=a;
                if (entry->selected) {
                    if (entry->type>=ENTRY_DIRECTORY) {
                        ++dopus_curwin[data_active_window]->dirsel;
                        if (entry->size!=-1) dopus_curwin[data_active_window]->bytessel+=entry->size;
                    }
                    else {
                        ++dopus_curwin[data_active_window]->filesel;
                        dopus_curwin[data_active_window]->bytessel+=entry->size;
                    }
                }
                if (rexx_argcount>2 && atoi(rexx_args[2]) && !status_iconified) {
                    refreshwindow(data_active_window,0);
                    if (entry->type!=ENTRY_CUSTOM) doselinfo(data_active_window);
                }
            }
            rexx_return(msg,d);
            return(1);

        case FUNC_DISPLAYDIR:
            if (status_iconified) break;
            if (rexx_argcount<1 || (win=atoi(rexx_args[0]))<0 || win>1)
                win=data_active_window;
            refreshwindow(win,1);
            break;

        case FUNC_ADDFILE:
            a=atoi(rexx_args[3]);
            if (a<0) a=0;
            ds.ds_Days=a/86400; a%=86400;
            ds.ds_Minute=a/60; a%=60;
            ds.ds_Tick=a*50;

            if ((b=atoi(rexx_args[2]))==0) d=0;
            else {
                if (status_iconified) strcpy(rexx_args[7],"0");
                d=(int)addfile(dopus_curwin[data_active_window],data_active_window,
                    rexx_args[0],
                    atoi(rexx_args[1]),
                    b,
                    &ds,
                    rexx_args[4],
                    getprotval(rexx_args[5]),
                    0,
                    atoi(rexx_args[7]),
                    NULL,NULL,0,0);
                fixprop(data_active_window);
                doposprop(data_active_window);
            }
            rexx_return(msg,d?1:0);
            return(1);

        case FUNC_REMOVEFILE:
            if (status_iconified) strcpy(rexx_args[1],"0");
            if ((entry=findfile(dopus_curwin[data_active_window],rexx_args[0],NULL))) {
                removefile(entry,dopus_curwin[data_active_window],data_active_window,atoi(rexx_args[1]));
                a=1;
            }
            else a=0;
            rexx_return(msg,a);
            return(1);

/* AddCustEntry "text" <userdata> <fgpen> <bkpen> <selectable> <show> <before> */

        case FUNC_ADDCUSTENTRY:
            if (status_iconified) strcpy(rexx_args[2],"0");
            DateStamp(&ds);
            buf2[0]=0;
            if (dopus_curwin[data_active_window]->firstentry &&
                (dopus_curwin[data_active_window]->firstentry->type!=ENTRY_CUSTOM ||
                dopus_curwin[data_active_window]->firstentry->subtype!=CUSTOMENTRY_USER)) a=0;
            else {
                if ((b=strlen(rexx_args[0]))>=MAXDISPLAYLENGTH) {
                    b=MAXDISPLAYLENGTH-1;
                    rexx_args[0][b]=0;
                }
                if (!dopus_curwin[data_active_window]->firstentry) {
                    dopus_curwin[data_active_window]->hlen=scrdata_dispwin_nchars[data_active_window];
                    fixhorizprop(data_active_window);
                }
                if (b>dopus_curwin[data_active_window]->hlen) {
                    dopus_curwin[data_active_window]->hlen=b;
                    fixhorizprop(data_active_window);
                }
                if (rexx_argcount>1) d=atoi(rexx_args[1]);
                else d=0;
                if (rexx_argcount>2) {
                    cval=atoi(rexx_args[2]);
                    c=cval;
                }
                else c=0x000000ff;
                if (rexx_argcount>3) {
                    cval=atoi(rexx_args[3]);
                    c|=cval<<8;
                }
                else c|=0x0000ff00;
                if (rexx_args[0][0] && rexx_argcount>4 && atoi(rexx_args[4])) c|=CUSTENTRY_CANSELECT;
                entry=NULL;
                if (rexx_argcount>6) {
                    if ((f=atoi(rexx_args[6]))>-1) {
                        entry=dopus_curwin[data_active_window]->firstentry;
                        while (entry) {
                            if (f--==0) break;
                            entry=entry->next;
                        }
                        if (entry->last) entry=entry->last;
                        else entry=(struct Directory *)-1;
                    }
                }
                if ((a=(int)addfile(dopus_curwin[data_active_window],
                    data_active_window,buf2,c,ENTRY_CUSTOM,
                    &ds,rexx_args[0],d,CUSTOMENTRY_USER,
                    (rexx_argcount>5)?atoi(rexx_args[5]):0,NULL,entry,0,0))) {
                    if (b>dopus_curwin[data_active_window]->hlen) dopus_curwin[data_active_window]->hlen=b;
                }
            }
            rexx_return(msg,a?1:0);
            return(1);

        case FUNC_REMOVEENTRY:
            if (rexx_argcount<1) break;
            if (status_iconified) strcpy(rexx_args[1],"0");
            entry=dopus_curwin[data_active_window]->firstentry;
            b=atoi(rexx_args[0]);
            for (a=0;a<b;a++) {
                if (!entry) break;
                entry=entry->next;
            }
            if (!entry) a=0;
            else {
                removefile(entry,dopus_curwin[data_active_window],data_active_window,atoi(rexx_args[1]));
                a=1;
            }
            rexx_return(msg,a);
            return(1);

        case FUNC_SETWINTITLE:
            if (rexx_argcount<2 || (win=atoi(rexx_args[1]))<0 || win>1)
                win=data_active_window;
            LStrnCpy(dopus_curwin[win]->diskname,rexx_args[0],32);
#ifdef __SASC__
                                dopus_curwin[win]->diskfree = i64_atoi( "-1" );
                dopus_curwin[win]->disktot = i64_atoi( "-1" );
                dopus_curwin[win]->diskblock = -1;
#else
            dopus_curwin[win]->diskfree=
                dopus_curwin[win]->disktot=
                dopus_curwin[win]->diskblock=-1;
#endif
            if (!status_iconified) displayname(win,1);
            break;

        case FUNC_ADDCUSTHANDLER:
            if (rexx_argcount<2 || (win=atoi(rexx_args[1]))<0 || win>1)
                win=data_active_window;
            strcpy(buf,dopus_curwin[win]->custhandler);
            LStrnCpy(dopus_curwin[win]->custhandler,rexx_args[0],32);
            rexx_set_return(msg,0,buf);
            return(1);

        case FUNC_COPYWINDOW:
            if (status_iconified) break;
            if (rexx_argcount<1 || (win=atoi(rexx_args[0]))<0 || win>1)
                win=data_active_window;
            copydirwin(dopus_curwin[win],dopus_curwin[1-win],1-win);
            break;

        case FUNC_SWAPWINDOW:
            if (status_iconified) break;
            swapdirwin();
            break;

        case FUNC_TOPTEXT:
            if (status_iconified) break;
            if (rexx_argcount>0) dostatustext(command+1);
            break;

        case FUNC_DIRTREE:
            if (status_iconified) break;
            if (rexx_argcount<1 || (win=atoi(rexx_args[0]))<0 || win>1)
                win=data_active_window;
            dotree(win);
            break;

        case FUNC_MODIFY:
        case FUNC_QUERY:
            if (rexx_argcount<1) break;
            for (val=0;;val++) {
                if (modifynames[val][0]==0) {
                    val=-1;
                    break;
                }
                if (Stricmp(modifynames[val],rexx_args[0])==0) break;
            }
            b=atoi(rexx_args[1]);

            if (function==FUNC_QUERY) {
                retval=0;
                retbuf=NULL;

                switch (val) {
                    case MOD_COPYFLAGS:
                        retval=config->copyflags;
                        break;

                    case MOD_DELETEFLAGS:
                        retval=config->deleteflags;
                        break;

                    case MOD_ERRORFLAGS:
                        retval=config->errorflags;
                        break;

                    case MOD_LISTFORMAT:
                        buf[0]=0;
                        if (b<0 || b>1) b=data_active_window;
                        for (a=0;a<=DISPLAY_LAST;a++) {
                            if (config->displaypos[b][a]==-1) break;
                            lsprintf(buf2,"%ld ",(long int)config->displaypos[b][a]);
                            strcat(buf,buf2);
                        }
                        retbuf=buf;
                        break;

                    case MOD_GENERALFLAGS:
                        retval=config->generalflags;
                        break;

                    case MOD_ICONFLAGS:
                        retval=config->iconflags;
                        break;

                    case MOD_REPLACEFLAGS:
                        retval=config->existflags;
                        break;

                    case MOD_SORTFLAGS:
                        retval=config->sortflags;
                        break;

                    case MOD_UPDATEFLAGS:
                        retval=config->dynamicflags;
                        break;

                    case MOD_SCRDEPTH:
                        retval=config->scrdepth;
                        break;

                    case MOD_SCREENFLAGS:
                        retval=config->screenflags;
                        break;

                    case MOD_SCREENMODE:
                        retval=config->screenmode;
                        break;

                    case MOD_SCRW:
                        retval=config->scrw;
                        break;

                    case MOD_SCRH:
                        retval=config->scrh;
                        break;

                    case MOD_DIRFLAGS:
                        retval=config->dirflags;
                        break;

                    case MOD_DEFAULTTOOL:
                        retbuf=config->defaulttool;
                        break;

                    case MOD_SHOWDELAY:
                        retval=config->showdelay;
                        break;

                    case MOD_VIEWPLAYFLAGS:
                        retval=config->viewbits;
                        break;

                    case MOD_FADETIME:
                        retval=config->fadetime;
                        break;

                    case MOD_HIDDENBIT:
                        retval=config->hiddenbit;
                        break;

                    case MOD_SHOWPATTERN:
                        retbuf=config->showpat;
                        break;

                    case MOD_HIDEPATTERN:
                        retbuf=config->hidepat;
                        break;

                    case MOD_ICONIFYFLAGS:
                        retval=config->icontype;
                        break;

                    case MOD_SCREENCLOCKFLAGS:
                        retval=config->scrclktype;
                        break;

                    case MOD_SHOWFREEFLAGS:
                        retval=config->showfree;
                        break;

                    case MOD_FONT:
                        if (b<0 || b>=FONT_LAST) break;
                        lsprintf(buf,"%s/%ld",config->fontbufs[b],(long int)config->fontsizes[b]);
                        retbuf=buf;
                        break;

                    case MOD_BANKNUMBER:
                        bank=dopus_firstgadbank;
                        for (a=0;bank && bank!=dopus_curgadbank;a++) bank=bank->next;
                        retval=a+1;
                        break;

                    case MOD_DATEFORMAT:
                        retval=config->dateformat;
                        break;

                    case MOD_GADGETROWS:
                        retval=config->gadgetrows;
                        break;

                    case MOD_DISPLAYLENGTH:
                        buf[0]=0;
                        if (b<0 || b>1) b=data_active_window;
                        lsprintf(buf,"%ld %ld %ld %ld %ld",
                            (long int)config->displaylength[b][DISPLAY_NAME],
                            (long int)config->displaylength[b][DISPLAY_COMMENT],
                            (long int)config->displaylength[b][DISPLAY_FILETYPE],
                            (long int)config->displaylength[b][DISPLAY_OWNER],
                            (long int)config->displaylength[b][DISPLAY_GROUP]);
                        retbuf=buf;
                        break;

                    case MOD_WINDOWDELTA:
                        retval=config->windowdelta;
                        break;

                    case MOD_SORTMETHOD:
                        if (b<0 || b>1) b=data_active_window;
                        retval=config->sortmethod[b];
                        break;

                    case MOD_SEPARATEMETHOD:
                        if (b<0 || b>1) b=data_active_window;
                        retval=config->separatemethod[b];
                        break;

                    case MOD_FILTER:
                        retbuf=str_filter;
                        break;

                    case MOD_WINDOWXY:
                        if (!status_iconified && status_publicscreen && Window)
                            lsprintf(buf,"%ld %ld",(long int)Window->LeftEdge,(long int)Window->TopEdge);
                        else lsprintf(buf,"%ld %ld",(long int)config->wbwinx,(long int)config->wbwiny);
                        retbuf=buf;
                        break;

                    case MOD_WINDOWSIZE:
                        if (!status_iconified && status_publicscreen && Window)
                            lsprintf(buf,"%ld %ld",(long int)Window->Width,(long int)Window->Height);
                        else lsprintf(buf,"%ld %ld",(long int)config->scr_winw,(long int)config->scr_winh);
                        retbuf=buf;
                        break;

                    case MOD_PUBSCREEN:
                        retbuf=config->pubscreen_name;
                        break;

                    case MOD_WINDOWXYWH:
                        if (!status_iconified && status_publicscreen && Window) {
                            lsprintf(buf,"%ld %ld %ld %ld",
                                (long int)Window->LeftEdge,(long int)Window->TopEdge,(long int)Window->Width,(long int)Window->Height);
                        }
                        else {
                            lsprintf(buf,"%ld %ld %ld %ld",
                                (long int)config->wbwinx,(long int)config->wbwiny,(long int)config->scr_winw,(long int)config->scr_winh);
                        }
                        retbuf=buf;
                        break;

                    case MOD_SCREENNAME:
                        retbuf=get_our_pubscreen();
                        break;

                    case MOD_PORTNAME:
                        retbuf=str_arexx_portname;
                        break;

                    case MOD_OUTPUTCMD:
                        retbuf=config->outputcmd;
                        break;

                    case MOD_OUTPUTWINDOW:
                        retbuf=config->output;
                        break;
                }

                if (retbuf) rexx_set_return(msg,0,retbuf);
                else rexx_return(msg,retval);
                return(1);
            }

            switch (val) {
                case MOD_COPYFLAGS:
                    modify(&config->copyflags);
                    break;

                case MOD_DELETEFLAGS:
                    modify(&config->deleteflags);
                    break;

                case MOD_ERRORFLAGS:
                    modify(&config->errorflags);
                    break;

                case MOD_LISTFORMAT:
                    if (b<0 || b>1) b=data_active_window;
                    for (a=0;a<=DISPLAY_LAST;a++) {
                        if (rexx_argcount<=a+2) config->displaypos[b][a]=-1;
                        else config->displaypos[b][a]=atoi(rexx_args[a+2]);
                    }
                    break;

                case MOD_GENERALFLAGS:
                    modify(&config->generalflags);
                    break;

                case MOD_ICONFLAGS:
                    modify(&config->iconflags);
                    break;

                case MOD_REPLACEFLAGS:
                    modify(&config->existflags);
                    break;

                case MOD_SORTFLAGS:
                    modify(&config->sortflags);
                    sortdir(dopus_curwin[0],0);
                    sortdir(dopus_curwin[1],1);
                    break;

                case MOD_UPDATEFLAGS:
                    modify(&config->dynamicflags);
                    break;

                case MOD_SCRDEPTH:
                    if (b<2) b=2;
                    else if (b>8) b=8;
                    config->scrdepth=b;
                    break;

                case MOD_SCREENFLAGS:
                    modify(&config->screenflags);
                    break;

                case MOD_SCREENMODE:
                    config->screenmode=b;
                    break;

                case MOD_SCRW:
                    config->scrw=b;
                    break;

                case MOD_SCRH:
                    config->scrh=b;
                    break;

                case MOD_DIRFLAGS:
                    modify(&config->dirflags);
                    break;

                case MOD_HELPFILE:
                    readhelp(rexx_args[1]);
                    break;

                case MOD_DEFAULTTOOL:
                    strcpy(config->defaulttool,rexx_args[1]);
                    break;

                case MOD_SHOWDELAY:
                    config->showdelay=b;
                    break;

                case MOD_VIEWPLAYFLAGS:
                    modify(&config->viewbits);
                    break;

                case MOD_FADETIME:
                    config->fadetime=b;
                    break;

                case MOD_HIDDENBIT:
                    modify(&config->hiddenbit);
                    break;

                case MOD_SHOWPATTERN:
                    strcpy(config->showpat,rexx_args[1]);
                    LParsePatternI(config->showpat,config->showpatparsed);
                    break;

                case MOD_HIDEPATTERN:
                    strcpy(config->hidepat,rexx_args[1]);
                    LParsePatternI(config->hidepat,config->hidepatparsed);
                    break;

                case MOD_ICONIFYFLAGS:
                    modify(&config->icontype);
                    break;

                case MOD_SCREENCLOCKFLAGS:
                    modify(&config->scrclktype);
                    break;

                case MOD_SHOWFREEFLAGS:
                    modify(&config->showfree);
                    break;

                case MOD_FONT:
                    if (b<0 || b>=FONT_LAST) break;
                    strcpy(config->fontbufs[b],rexx_args[2]);
                    if (!strstri(config->fontbufs[b],".font"))
                        strcat(config->fontbufs[b],".font");
                    if (rexx_argcount>3) config->fontsizes[b]=atoi(rexx_args[3]);
                    break;

                case MOD_BANKNUMBER:
                    bank=dopus_firstgadbank;
                    if ((--b)<0) b=0;
                    for (a=0;a<b;a++) {
                        if (!bank) break;
                        bank=bank->next;
                    }
                    if (bank && bank!=dopus_curgadbank) {
                        dopus_curgadbank=bank;
                        data_gadgetrow_offset=0;
                        drawgadgets(0,0);
                        fixgadgetprop();
                    }
                    break;

                case MOD_DATEFORMAT:
                    modify(&config->dateformat);
                    break;

                case MOD_GADGETROWS:
                    if (b<0 || b>6 || b==4 || b==5) break;
                    config->gadgetrows=b;
                    break;

                case MOD_DISPLAYLENGTH:
                    if (b<0 || b>1) b=data_active_window;
                    for (a=0;a<6;a++) {
                        if (rexx_argcount<=a+2) break;
                        config->displaylength[b][displenmap[a]]=atoi(rexx_args[a+2]);
                        if (config->displaylength[b][displenmap[a]]<20) config->displaylength[b][displenmap[a]]=20;
                        else if (config->displaylength[b][displenmap[a]]>MAX_DISPLAYLENGTH) config->displaylength[b][displenmap[a]]=MAX_DISPLAYLENGTH;
                    }
                    break;

                case MOD_WINDOWDELTA:
                    dosizedirwindows(b);
                    break;

                case MOD_SORTMETHOD:
                    if (b<0 || b>1) b=data_active_window;
                    config->sortmethod[b]=atoi(rexx_args[2]);
                    sortdir(dopus_curwin[b],b);
                    break;

                case MOD_SEPARATEMETHOD:
                    if (b<0 || b>1) b=data_active_window;
                    config->separatemethod[b]=atoi(rexx_args[2]);
                    break;

                case MOD_FILTER:
                    strcpy(str_filter,rexx_args[1]);
                    if (str_filter[0]) {
                        screen_gadgets[SCRGAD_TINYFILTER].Flags|=GFLG_SELECTED;
                        LParsePatternI(str_filter,str_filter_parsed);
                    }
                    else {
                        screen_gadgets[SCRGAD_TINYFILTER].Flags&=~GFLG_SELECTED;
                        str_filter_parsed[0]=0;
                    }
                    if (Window && config->generalscreenflags&SCR_GENERAL_TINYGADS)
                        dolittlegads(&screen_gadgets[SCRGAD_TINYFILTER],&globstring[STR_TINY_BUTTONS][6],1);
                    break;

                case MOD_WINDOWXY:
                  {
                    int x,y;

                    if (rexx_argcount<2) {
                        x=config->wbwinx;
                        y=config->wbwiny;
                    }
                    else {
                        if (b>-1) x=config->wbwinx=b;
                        else if (Window) x=Window->LeftEdge;
                        else x=config->wbwinx;

                        if (rexx_argcount>2 && (b=atoi(rexx_args[2]))>-1) y=config->wbwiny=b;
                        else if (Window) y=config->wbwiny=Window->TopEdge;
                        else y=config->wbwiny;
                    }
                    if (!status_iconified && status_publicscreen && Window) {
                        if (Window->Width+x>Window->WScreen->Width)
                            x=Window->WScreen->Width-Window->Width;
                        if (x<0) x=0;
                        if (Window->Height+y>Window->WScreen->Height)
                            y=Window->WScreen->Height-Window->Height;
                        if (y<0) y=0;
                        MoveWindow(Window,x-Window->LeftEdge,y-Window->TopEdge);
                    }
                    break;
                  }
                case MOD_WINDOWSIZE:
                  {
                    int w,h;

                    if (rexx_argcount<2) {
                        w=config->scr_winw;
                        h=config->scr_winh;
                    }
                    else {
                        if (b>-1) w=config->scr_winw=b;
                        else if (Window) w=Window->Width;
                        else w=config->scr_winw;

                        if (rexx_argcount>2 && (b=atoi(rexx_args[2]))>-1) h=config->scr_winh=b;
                        else if (Window) h=config->scr_winh=Window->Height;
                        else h=config->scr_winh;
                    }
                    if (!status_iconified && status_publicscreen && Window) {
                        int dx=0,dy=0;

                        if (w<scr_min_width) w=scr_min_width;
                        else if (w>Window->WScreen->Width) w=Window->WScreen->Width;
                        if (h<scr_min_height) h=scr_min_height;
                        else if (h>Window->WScreen->Height) h=Window->WScreen->Height;

                        if (Window->LeftEdge+w>Window->WScreen->Width)
                            dx=(Window->WScreen->Width-w)-Window->LeftEdge;

                        if (Window->TopEdge+h>Window->WScreen->Height)
                            dy=(Window->WScreen->Height-h)-Window->TopEdge;

                        if (w!=Window->Width || h!=Window->Height) {
                            MoveWindow(Window,dx,dy);
                            SizeWindow(Window,w-Window->Width,h-Window->Height);

                            config->scr_winw=w;
                            config->scr_winh=h;

                            if (!MainScreen) {
                                config->scrw=config->scr_winw;
                                config->scrh=config->scr_winh;
                            }
                            config->screenflags&=~(SCRFLAGS_DEFWIDTH|SCRFLAGS_DEFHEIGHT);
                            remclock();
                            SetUp(0);
                            dostatustext(str_last_statustext);
                        }
                    }
                    break;
                  }
                case MOD_PUBSCREEN:
                    strcpy(config->pubscreen_name,rexx_args[1]);
                    break;

                case MOD_WINDOWXYWH:
                    if (rexx_argcount<4 || !system_version2) break;
                    config->wbwinx=b;
                    config->wbwiny=atoi(rexx_args[2]);
                    config->scr_winw=atoi(rexx_args[3]);
                    config->scr_winh=atoi(rexx_args[4]);
                    if (!status_iconified && status_publicscreen && Window)
                        ChangeWindowBox(Window,
                            config->wbwinx,config->wbwiny,
                            config->scr_winw,config->scr_winh);
                    break;

                case MOD_PORTNAME:
                    change_port_name(rexx_args[1]);
                    break;

                case MOD_OUTPUTCMD:
                    strcpy(config->outputcmd,rexx_args[1]);
                    break;

                case MOD_OUTPUTWINDOW:
                    strcpy(config->output,rexx_args[1]);
                    break;
            }
            break;

        default:
            func_global_function=function;
D(bug("rexx_args(%s,%s,%s)\n",rexx_args[0]?rexx_args[0]:"<NULL>",rexx_args[1]?rexx_args[1]:"<NULL>",rexx_args[2]?rexx_args[2]:"<NULL>"));
            break;
    }
    if (!func_global_function) rexx_return(msg,0);
    return(0);
}

int parse(buf)
unsigned char *buf;
{
    int len,argno,argpos,a,withinquote,havequoted;
D(bug("parse cmd(%s)\n",buf));
    len=strlen(buf);
    for (a=len-1;a>=0;a--) if (_isspace(buf[a])==0) break;
    len=a;
    argno=argpos=withinquote=havequoted=0;

    for (a=1;a<=len;a++) {
        ;
D(bug("_isspace(%lc) = %ld\n",buf[a],_isspace(buf[a])));
        if (buf[a]=='\"') {
            if (withinquote==1) withinquote=0;
            else if (withinquote==0) {
                withinquote=1;
                havequoted=1;
            }
            else rexx_args[argno][argpos++]=buf[a];
        }
        else if (buf[a]=='\'') {
            if (withinquote==2) withinquote=0;
            else if (withinquote==0) {
                withinquote=2;
                havequoted=1;
            }
            else rexx_args[argno][argpos++]=buf[a];
        }
        else if (_isspace(buf[a])>0) {
            if (withinquote) rexx_args[argno][argpos++]=buf[a];
            else {
                if (argpos>0 || havequoted) {
                    rexx_args[argno++][argpos]=0;
D(bug("arg %ld complete: (%s)\n",argno-1,rexx_args[argno-1]));
                    argpos=havequoted=0;
                }
            }
        }
        else rexx_args[argno][argpos++]=buf[a];
        if (argpos==255) {
            rexx_args[argno++][255]=0;
            argpos=havequoted=0;
        }
        if (argno==16) break;
    }
    if (argno<16 && argpos<256) {
         rexx_args[argno][argpos]=0;
D(bug("arg %ld complete: (%s)\n",argno,rexx_args[argno]));
    }
    else if (argno==16) argno=15;
/*
    if (rexx_args[0][0]==0) argno=-1;
*/
    return(argno+1);
}

char *dosstatus(f,buf,buf1)
int f;
char *buf,*buf1;
{
    if (f) strcpy(buf,buf1);
    return(buf);
}

void dopustofront()
{
    int top;

    if (!status_iconified) {
        if (MainScreen) {
            ScreenToFront(MainScreen);
            if (!status_configuring && config->screenflags&SCRFLAGS_HALFHEIGHT)
                top=scrdata_norm_height;
            else top=0;
            MoveScreen(MainScreen,0,top-MainScreen->TopEdge);
        }
        if (Window) {
            if (!Window->Flags&WFLG_BACKDROP) WindowToFront(Window);
            ActivateWindow(Window);
        }
    }
}

void dopustoback()
{
    if (!status_iconified) {
        if (MainScreen) ScreenToBack(MainScreen);
        else if (Window) WindowToBack(Window);
        if (Window->Parent) ActivateWindow(Window->Parent);
    }
}

int checkkeyword(keywords,num,rem)
char **keywords;
int num,rem;
{
    int a;

    if (!keywords) return(0);
    for (a=0;;a++) {
        if (!keywords[a] || !keywords[a][0]) return(0);
        if (Stricmp(rexx_args[num],keywords[a])==0) {
            rexx_arg_value[a]=1;
            if (rem) removeargstring(num);
            return(1);
        }
    }
    return(0);
}

void removeargstring(num)
int num;
{
    int a;

    for (a=num+1;a<16;a++) {
        CopyMem(rexx_args[a],rexx_args[a-1],256);
        rexx_args[15][0]=0;
    }
}

void modify(flag)
unsigned char *flag;
{
    int a;

    for (a=1;a<rexx_argcount;a++) {
        if (rexx_args[a][0]=='+') {
            (*flag) |= atoi(&rexx_args[a][1]);
        }
        else if (rexx_args[a][0]=='-') {
            *(flag) &= ~(atoi(&rexx_args[a][1]));
        }
        else (*flag)=atoi(rexx_args[a]);
    }
}

void rexx_return(msg,num)
struct RexxMsg *msg;
long long num;
{
    char numbuf[24];

    sprintf(numbuf,"%qd",num);
    rexx_set_return(msg,0,numbuf);
}
