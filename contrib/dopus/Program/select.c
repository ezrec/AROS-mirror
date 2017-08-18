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

static struct Directory *selectedentry;

void doselection(win,state)
int win,state;
{
    int
        a,b,c,d,e,x,y,ox,oy,fa,la,juststart,atot,drag,offx,offy,candrag,
        comp=-1,oc,gad,multidrag=0,type,okrepeatdrag=0;
    ULONG class;
    UWORD code,qual;
    char buf[40],*ptr;
    struct Directory *next;

    candrag=drag=(config->generalflags&GENERAL_DRAG && drag_bob_buffer)?1:0;
    /*a=0;*/ atot=dopus_curwin[win]->total-scrdata_dispwin_lines;
    a=makeactive(win,1);
    if (dopus_curwin[win]->total==0) return;
    ox=x=Window->MouseX; oy=y=Window->MouseY; selectedentry=NULL;
    if (!a) {
        a=(y-scrdata_dirwin_ypos[data_active_window])/scrdata_font_ysize;
        if (drag) {
            offx=x-scrdata_dirwin_xpos[data_active_window];
            if (offx<0) offx=0;
            else if (offx>drag_sprite.Width*16) offx=drag_sprite.Width*16;
            offy=y-(scrdata_dirwin_ypos[data_active_window]+(a*scrdata_font_ysize));
        }
        if ((b=select(win,a))==2) {
            return;
        }
        if (!b || !selectedentry) drag=0;
        a=juststart=0;
        Window->Flags|=WFLG_REPORTMOUSE;
        FOREVER {
            while (getintuimsg()) {
gotmsgloop:
                class=IMsg->Class; code=IMsg->Code; qual=IMsg->Qualifier;
                ReplyMsg((struct Message *) IMsg);
getcoordsloop:
                x=Window->MouseX; y=Window->MouseY;
                okrepeatdrag=0;
                if (class==MOUSEBUTTONS) {
                    if (code==MENUDOWN && candrag && selectedentry) {
                        status_haveaborted=status_justabort=0;
                        if (drag!=2) goto startdragging;
                        else {
                            drag=0;
                            goto enddragging;
                        }
                    }
                    else if (code==SELECTUP) {
                        a=1;
                        break;
                    }
                }
                else if (class==MOUSEMOVE) {
                    if (drag==2) {
                        if (x!=ox || y!=oy) {
                            if (scr_gadget_rows && dopus_curgadbank) {
                                oc=gadgetfrompos(x,y); gad=oc+(data_gadgetrow_offset*7);
                                if (oc>-1 && !(isvalidgad(&dopus_curgadbank->gadgets[gad]))) oc=-1;
                                if (oc!=comp) {
                                    RemIBob(&drag_bob,main_rp,ViewPortAddress(Window));
                                    if (comp>-1) dragcompgad(comp,0);
                                    if (oc>-1 && isvalidgad(&dopus_curgadbank->gadgets[gad])) {
                                        dragcompgad(oc,1);
                                        comp=oc;
                                    }
                                    else comp=-1;
                                    AddBob(&drag_bob,main_rp);
                                }
                            }
                            drag_sprite.X=x-offx; drag_sprite.Y=y-offy;
                            SortGList(main_rp);
                            DrawGList(main_rp,ViewPortAddress(Window));
                            WaitTOF();
                        }
                        ox=x; oy=y;
                    }
                    else {
                        fa=(y-scrdata_dirwin_ypos[data_active_window])/scrdata_font_ysize; la=(oy-scrdata_dirwin_ypos[data_active_window])/scrdata_font_ysize;
                        if (fa==la && drag && selectedentry) {
                            if (x<scrdata_dirwin_xpos[data_active_window] || x>scrdata_dirwin_xpos[data_active_window]+scrdata_dirwin_width[data_active_window]) {
startdragging:
                                ptr=NULL;
                                if (qual&IEQUALIFIER_ANYSHIFT && dopus_curwin[win]->filesel>1) {
                                    multidrag=1;
                                    lsprintf(buf,globstring[STR_MULTI_DRAG],dopus_curwin[win]->filesel);
                                    ptr=buf;
                                    type=-1;
                                }
                                else {
                                    if (selectedentry->type==ENTRY_CUSTOM) {
                                        switch (selectedentry->subtype) {
                                            case CUSTOMENTRY_BUFFERLIST:
                                                ptr=selectedentry->comment;
                                                break;
                                            case CUSTOMENTRY_DIRTREE:
                                                ptr=selectedentry->dispstr;
                                                break;
                                            case CUSTOMENTRY_USER:
                                                if (selectedentry->size&CUSTENTRY_CANSELECT)
                                                    ptr=selectedentry->comment;
                                                break;
                                        }
                                    }
                                    else ptr=selectedentry->name;
                                    type=selectedentry->type;
                                }
                                if (ptr) {
                                    dotaskmsg(clockmsg_port,CLOCK_ACTIVE,0,0,NULL,0);
                                    SetAPen(&drag_bob_rastport,
                                        (type<=ENTRY_FILE ||
                                            (type==ENTRY_DEVICE &&
                                            (selectedentry->size==DLT_DEVICE || selectedentry->size==DLT_VOLUME)))?
                                        screen_pens[config->filesselfg].pen:
                                            screen_pens[config->dirsselfg].pen);
                                    SetRast(&drag_bob_rastport,
                                        (type<=ENTRY_FILE ||
                                            (type==ENTRY_DEVICE &&
                                            (selectedentry->size==DLT_DEVICE || selectedentry->size==DLT_VOLUME)))?
                                        screen_pens[config->filesselbg].pen:
                                            screen_pens[config->dirsselbg].pen);
                                    Move(&drag_bob_rastport,0,scr_font[FONT_DIRS]->tf_Baseline);
                                    c=(drag_sprite.Width*16)/scrdata_font_xsize;
                                    d=strlen(ptr);
                                    if (d>c) e=d-c;
                                    else {
                                        e=0;
                                        c=d;
                                    }
                                    Text(&drag_bob_rastport,&ptr[e],c);
                                    drag_sprite.X=x-offx; drag_sprite.Y=y-offy;
                                    AddBob(&drag_bob,main_rp);
                                    SortGList(main_rp);
                                    DrawGList(main_rp,ViewPortAddress(Window));
                                    WaitTOF();
                                    drag=2; y=-1;
                                }
                            }
                        }
                        else {
                            if (drag) drag=0;
                            if (la<fa) {
                                ++fa; ++la;
                                d=la; la=fa; fa=d;
                            }
                            if (y>=scrdata_dispwin_ypos && y<scrdata_dispwin_height+scrdata_dispwin_ypos) {
                                if (state) {
                                    for (d=fa;d<la;d++) defselect(win,d,b);
                                }
                            }
                            else if (y<scrdata_dispwin_ypos) {
                                if (juststart!=-1) {
                                    if (fa<0) fa=0; if (la>=scrdata_dispwin_lines) la=scrdata_dispwin_lines;
                                    if (state) {
                                        for (d=fa;d<la;d++) defselect(win,d,b);
                                        defselect(win,0,b);
                                    }
                                    juststart=-1;
                                }
                                if (dopus_curwin[win]->offset==0) continue;
                                verticalscroll(win,-1);
                                if (state) defselect(win,0,b);
                                okrepeatdrag=1;
                            }
                            else if (y>=scrdata_dispwin_height+scrdata_dispwin_ypos) {
                                if (juststart!=1) {
                                    if (fa<0) fa=0; if (la>=scrdata_dispwin_lines) la=scrdata_dispwin_lines;
                                    if (state) {
                                        for (d=fa;d<la;d++) defselect(win,d,b);
                                        defselect(win,scrdata_dispwin_lines-1,b);
                                    }
                                    juststart=1;
                                }
                                if (dopus_curwin[win]->offset==atot) continue;
                                verticalscroll(win,1);
                                if (state) defselect(win,scrdata_dispwin_lines-1,b);
                                okrepeatdrag=1;
                            }
                        }
                        oy=y;
                    }
                }
            }
            if (a) break;
            if (getintuimsg()) goto gotmsgloop;
            if (okrepeatdrag) goto getcoordsloop;
            Wait(1<<Window->UserPort->mp_SigBit);
        }
        if (drag==2) {
enddragging:
            RemIBob(&drag_bob,main_rp,ViewPortAddress(Window));
            if (comp>-1) dragcompgad(comp,0);
            dotaskmsg(clockmsg_port,CLOCK_ACTIVE,1,0,NULL,0);
            if (x>=scrdata_dirwin_xpos[1-data_active_window] &&
                x<=scrdata_dirwin_xpos[1-data_active_window]+scrdata_dirwin_width[1-data_active_window] &&
                y>=scrdata_dirwin_ypos[1-data_active_window] &&
                y<=scrdata_dirwin_ypos[1-data_active_window]+scrdata_dirwin_height && drag==2) {
                if (multidrag) {
                    last_selected_entry=dopus_curwin[win]->firstentry;
                    while (last_selected_entry) {
                        if (status_haveaborted || status_justabort) {
                            myabort();
                            break;
                        }
                        next=last_selected_entry->next;
                        if (last_selected_entry->type<=ENTRY_FILE && last_selected_entry->selected) {
                            CurrentTime(&time_previous_sec,&time_previous_micro);
                            time_current_sec=time_previous_sec;
                            time_current_micro=time_previous_micro;
                            makeactive(1-data_active_window,1);
                        }
                        last_selected_entry=next;
                    }
                }
                else {
                    time_previous_sec=time_current_sec;
                    time_previous_micro=time_current_micro;
                    last_selected_entry=selectedentry;
                    makeactive(1-data_active_window,1);
                }
            }
            else if (comp>-1) {
                gad=comp+(data_gadgetrow_offset*7);
                if (dopus_curgadbank && isvalidgad(&dopus_curgadbank->gadgets[gad])) {
                    if (multidrag) {
                        last_selected_entry=NULL;
                        func_single_file[0]=0;
                        status_flags|=STATUS_GLOBALFILE;
                    }
                    else {
                        last_selected_entry=selectedentry;
                        strcpy(func_single_file,selectedentry->name);
                        if (selectedentry->selected) unselect(data_active_window,selectedentry);
                    }
                    dofunctionstring(dopus_curgadbank->gadgets[gad].function,
                        dopus_curgadbank->gadgets[gad].name,
                        NULL,(struct dopusfuncpar *)&dopus_curgadbank->gadgets[gad].which);
                    status_flags&=~STATUS_GLOBALFILE;
                    if (!multidrag) checkselection(selectedentry);
                }
            }
        }
        Window->Flags&=~WFLG_REPORTMOUSE;
    }
}

void dormbscroll(win)
int win;
{
    ULONG class;
    UWORD code;
    int x,y,d,__unused a,__unused ret=0;
    __unused struct Directory *temp;

    if (data_active_window!=win) {
        makeactive(win,0);
        ret=1;
    }
#ifdef _USE_SMALL_Q
    if (dopus_curwin[win]->total>0 && status_flags&STATUS_IANSCRAP) {
        CurrentTime(&time_current_sec,&time_current_micro);
        y=Window->MouseY;
        a=(y-scrdata_dirwin_ypos[data_active_window]-2)/scrdata_font_ysize;
        if (a<scrdata_dispwin_lines) {
            a+=dopus_curwin[win]->offset;
            if (a<=dopus_curwin[win]->total) {
                temp=dopus_curwin[win]->firstentry;
                while (a--) temp=temp->next;
                if (temp && temp->type<=ENTRY_FILE && temp->selected &&
                    (DoubleClick(time_previous_sec,time_previous_micro,
                        time_current_sec,time_current_micro)) &&
                    last_selected_entry==temp &&
                    config->generalflags&GENERAL_DOUBLECLICK) {
                    unselect(win,temp);
                    ftype_doubleclick(str_pathbuffer[win],temp->name,0);
                    unbusy();
                    time_previous_sec=0;
                    return;
                }
            }
        }
        if (ret) return;
    }
#endif
    FOREVER {
        while (!getintuimsg()) {
            x=Window->MouseX; y=Window->MouseY;
            d=0;
            if (y<scr_scroll_borders[win].MinY) {
                verticalscroll(win,-1);
                if (y>scr_scroll_borders[win].MinY-(scrdata_font_ysize*4)) d=1;
            }
            else if (y>scr_scroll_borders[win].MaxY) {
                verticalscroll(win,1);
                if (y<scr_scroll_borders[win].MaxY+(scrdata_font_ysize*4)) d=1;
            }
            else if (x<scr_scroll_borders[win].MinX) horizontalscroll(win,-1);
            else if (x>scr_scroll_borders[win].MaxX) horizontalscroll(win,1);
            if (d) Delay(d);
        }
        class=IMsg->Class; code=IMsg->Code;
        ReplyMsg((struct Message *) IMsg);
        if (class==MOUSEBUTTONS && code==MENUUP) break;
    }
}

int select(win,o)
int win,o;
{
    int a,dbclick/*=0*/,sel=1,foundcount;
    char sbuf[FILEBUF_SIZE],*dir;
    struct Directory *temp,*temp2;

    a=o+dopus_curwin[win]->offset;
    if (o>=scrdata_dispwin_lines || a>=dopus_curwin[win]->total) return(0);
    temp=dopus_curwin[win]->firstentry;
    while (a--) temp=temp->next;
    selectedentry=temp;
    if (temp && ((ENTRYTYPE(temp->type)!=ENTRY_CUSTOM) ||
        (temp->subtype!=CUSTOMENTRY_USER) || (temp->size&CUSTENTRY_CANSELECT))) {
        sel=temp->selected;
        temp->selected=sel?FALSE:TRUE;
        updateselectinfo(temp,win,1);
        if (!status_iconified) {
            display_entry(temp,win,
                scrdata_dirwin_xpos[win]-dopus_curwin[win]->hoffset+1,
                scrdata_font_baseline+(scrdata_font_ysize*o)+scrdata_dirwin_ypos[win]);
            if (config->iconflags&ICONFLAG_AUTOSELECT) {
                StrCombine(sbuf,temp->name,".info",FILEBUF_SIZE-1);
                if ((temp2=findfile(dopus_curwin[win],sbuf,&foundcount))) {
                    if (temp2->selected!=temp->selected) {
                        temp2->selected=temp->selected;
                        updateselectinfo(temp2,win,1);
                        if (foundcount>=dopus_curwin[win]->offset &&
                            foundcount<dopus_curwin[win]->offset+scrdata_dispwin_lines) {
                            o=foundcount-dopus_curwin[win]->offset;
                            display_entry(temp2,win,
                                scrdata_dirwin_xpos[win]-dopus_curwin[win]->hoffset+1,
                                scrdata_font_baseline+(scrdata_font_ysize*o)+scrdata_dirwin_ypos[win]);
                        }
                    }
                }
            }
        }

/*
        if (last_selected_entry!=temp ||
            !(dbclick=DoubleClick(time_previous_sec,time_previous_micro,
                time_current_sec,time_current_micro))) {
            last_selected_entry=temp;
        }
        time_previous_sec=time_current_sec;
        time_previous_micro=time_current_micro;
*/
        
        if (sel) {
            dbclick=DoubleClick(time_previous_sec,time_previous_micro,
                time_current_sec,time_current_micro);
        }
        else {
            dbclick=-1;
            time_previous_sec=time_current_sec;
            time_previous_micro=time_current_micro;
        }

        switch (ENTRYTYPE(temp->type)) {
            case ENTRY_DEVICE:
            case ENTRY_DIRECTORY:
                if (dbclick==-1) last_selected_entry=temp;
                else if (dbclick && last_selected_entry==temp) {
                    dir=dopus_curwin[win]->directory;
                    if (!(dopus_curwin[win]->flags & DWF_ARCHIVE)) advancebuf(win,1);
                    if (temp->type==ENTRY_DEVICE) strcpy(str_pathbuffer[win],temp->name);
                    else
                    {
                     if (temp->subtype == ST_SOFTLINK)
                      {
                       struct FileInfoBlock __aligned fib;
                       struct DevProc *dp;
                       BPTR ld, lf;
                       char linkbuf[512],buf[256];

                       strcpy(linkbuf,dir);
                       do
                        {
//D(bug("SoftLink! path: %s\t name: %s\n",linkbuf,temp->name));
                         ld = Lock(linkbuf,ACCESS_READ);
                         if (dp = GetDeviceProc(linkbuf,NULL))
                          {
//D(bug("DeviceProc: %lX\n",dp));
                           if (ReadLink(dp->dvp_Port,ld,temp->name,buf,256))
                            {
                             AddPart(linkbuf,buf,512);
//D(bug("Resolved: %s\n",linkbuf));
                             if (lf = Lock(linkbuf,ACCESS_READ))
                              {
//D(bug("Locked\n"));
                               NameFromLock(lf,linkbuf,512);
                               if (Examine(lf,&fib))
                                {
//D(bug("Examined: type: %ld, name: %s\n",fib.fib_DirEntryType,fib.fib_FileName));
                                 if (fib.fib_DirEntryType == ST_SOFTLINK) *PathPart(linkbuf) = 0;
                                }
                               UnLock(lf);
                              }
                             else
                              {
//D(bug("Lock() failed!\n"));
                               fib.fib_DirEntryType = ST_USERDIR;
                              }
                            }
                           FreeDeviceProc(dp);
                           UnLock(ld);
                          }
                        } while (fib.fib_DirEntryType == ST_SOFTLINK);
                       strcpy(str_pathbuffer[win],linkbuf);
                      }
                     else
                      {
                       strcpy(str_pathbuffer[win],dir);
                       TackOn(str_pathbuffer[win],temp->name,256);
                      }
                    }
D(bug("Get dir: %s\n",str_pathbuffer[win]));
                    startgetdir(win,SGDFLAGS_CANMOVEEMPTY|SGDFLAGS_CANCHECKBUFS);
                    time_previous_sec=0;
                    return(2);
                }
                break;

            case ENTRY_CUSTOM:
                if (dbclick==-1) last_selected_entry=temp;
                else if (dbclick && last_selected_entry==temp) {
                    if (temp->subtype==CUSTOMENTRY_BUFFERLIST ||
                        temp->subtype==CUSTOMENTRY_DIRTREE) {
                        if (temp->subtype==CUSTOMENTRY_BUFFERLIST)
                            bringinbuffer(last_selected_entry->dispstr,win,1);
                        else {
                            strcpy(str_pathbuffer[win],last_selected_entry->dispstr);
                            startgetdir(win,SGDFLAGS_CANMOVEEMPTY|SGDFLAGS_CANCHECKBUFS);
                        }
                        time_previous_sec=0;
                        return(2);
                    }
                    else if (temp->subtype==CUSTOMENTRY_USER) {
                        userentrymessage(dopus_curwin[win],temp,USERENTRY_DOUBLECLICK);
                        time_previous_sec=0;
                        return(2);
                    }
                }
                break;

            case ENTRY_FILE:
                if (dbclick==-1) last_selected_entry=temp;
                else if (dbclick && last_selected_entry==temp) {
                    if (config->generalflags&GENERAL_DOUBLECLICK) {
                        if (dopus_curwin[win]->flags & DWF_ARCHIVE)
                         {
                          char path[256], tempname[FILEBUF_SIZE];

                          strcpy(path,"T:");
                          if (unarcfiledir(dopus_curwin[win],path,tempname,temp->name))
                           {
                            ftype_doubleclick(path,tempname,1);
                            AddPart(path,tempname,256);
                            removetemparcfile(path);
                           }
                         }
                        else ftype_doubleclick(str_pathbuffer[win],temp->name,1);
                        unbusy();
                        time_previous_sec=0;
                        return(2);
                    }
                }
                break;
        }
    }
    return(!sel);
}

int unselect(win,file)
int win;
struct Directory *file;
{
    int a=0;
    struct Directory *t;

    if (win==-1 || !file->selected) return(0);
    t=dopus_curwin[win]->firstentry;
    while (t!=file && t) {
        ++a;
        t=t->next;
    }
    if (!t) return(0);
    switch (ENTRYTYPE(file->type)) {
        case ENTRY_DIRECTORY:
        case ENTRY_DEVICE:
            --dopus_curwin[win]->dirsel;
            if (file->type>0 && file->size!=-1) dopus_curwin[win]->bytessel-=file->size;
            break;
        case ENTRY_FILE:
            --dopus_curwin[win]->filesel;
            dopus_curwin[win]->bytessel-=file->size;
            break;
    }
    file->selected=FALSE;
    if (a<dopus_curwin[win]->offset ||
        a>(dopus_curwin[win]->offset+scrdata_dispwin_lines-1)) return(0);
    a-=dopus_curwin[win]->offset;
    if (!status_iconified) {
        display_entry(t,win,
            scrdata_dirwin_xpos[win]-dopus_curwin[win]->hoffset+1,
            scrdata_font_baseline+(scrdata_font_ysize*a)+scrdata_dirwin_ypos[win]);
    }
    return(1);
}

void defselect(win,o,state)
int win,o,state;
{
    int a,foundcount;
    struct Directory *temp,*temp2;
    char sbuf[FILEBUF_SIZE];

    if (o<0 || o>scrdata_dispwin_lines-1) return;
    a=o+dopus_curwin[win]->offset;
    if (a>=dopus_curwin[win]->total) return;
    temp=dopus_curwin[win]->firstentry;
    while (a--) temp=temp->next;
    if (state) selectedentry=temp;
    else selectedentry=NULL;
    if (temp && ((ENTRYTYPE(temp->type)!=ENTRY_CUSTOM) ||
        (temp->subtype!=CUSTOMENTRY_USER) || (temp->size&CUSTENTRY_CANSELECT))) {
        if (temp->selected!=state) {
            temp->selected=state;
            updateselectinfo(temp,win,1);
            if (!status_iconified) {
                display_entry(temp,win,
                    scrdata_dirwin_xpos[win]-dopus_curwin[win]->hoffset+1,
                    scrdata_font_baseline+(scrdata_font_ysize*o)+scrdata_dirwin_ypos[win]);
            }
        }
        if (config->iconflags&ICONFLAG_AUTOSELECT) {
            StrCombine(sbuf,temp->name,".info",FILEBUF_SIZE-1);
            if ((temp2=findfile(dopus_curwin[win],sbuf,&foundcount))) {
                if (temp2->selected!=temp->selected) {
                    temp2->selected=temp->selected;
                    updateselectinfo(temp2,win,1);
                    if (foundcount>=dopus_curwin[win]->offset &&
                        foundcount<dopus_curwin[win]->offset+scrdata_dispwin_lines) {
                        if (!status_iconified) {
                            o=foundcount-dopus_curwin[win]->offset;
                            display_entry(temp2,win,
                                scrdata_dirwin_xpos[win]-dopus_curwin[win]->hoffset+1,
                                scrdata_font_baseline+(scrdata_font_ysize*o)+scrdata_dirwin_ypos[win]);
                        }
                    }
                }
            }
        }
    }
}

void globalselect(win,all)
int win,all;
{
    struct Directory *temp;

    if ((temp=dopus_curwin[win]->firstentry) && temp->type!=ENTRY_CUSTOM) {
        while (temp) {
/*
            if (temp->type!=ENTRY_CUSTOM || temp->subtype!=CUSTOMENTRY_USER ||
                temp->size&CUSTENTRY_CANSELECT) temp->selected=all;
*/
            temp->selected=all;
            temp=temp->next;
        }
        if (all) {
            dopus_curwin[win]->filesel=dopus_curwin[win]->filetot;
            dopus_curwin[win]->dirsel=dopus_curwin[win]->dirtot;
            dopus_curwin[win]->bytessel=dopus_curwin[win]->bytestot;
        }
        else {
            dopus_curwin[win]->filesel=0;
            dopus_curwin[win]->dirsel=0;
            dopus_curwin[win]->bytessel=0;
        }
        doselinfo(win);
    }
}

void updateselectinfo(temp,win,show)
struct Directory *temp;
int win,show;
{
    if (temp->selected) {
        switch (ENTRYTYPE(temp->type)) {
            case ENTRY_DIRECTORY:
                if (temp->size!=-1) dopus_curwin[win]->bytessel+=temp->size;
            case ENTRY_DEVICE:
                ++dopus_curwin[win]->dirsel;
                break;
            case ENTRY_FILE:
                ++dopus_curwin[win]->filesel;
                dopus_curwin[win]->bytessel+=temp->size;
                break;
        }
        if (show) {
            if (config->generalflags&GENERAL_DISPLAYINFO) doinfodisplay(temp,TRUE);
            else doselinfo(data_active_window);
        }
    }
    else {
        switch (ENTRYTYPE(temp->type)) {
            case ENTRY_DIRECTORY:
                if (temp->size!=-1) dopus_curwin[win]->bytessel-=temp->size;
            case ENTRY_DEVICE:
                --dopus_curwin[win]->dirsel;
                break;
            case ENTRY_FILE:
                --dopus_curwin[win]->filesel;
                dopus_curwin[win]->bytessel-=temp->size;
                break;
        }
        if (show) doselinfo(data_active_window);
    }
}

void globaltoggle(win)
int win;
{
    struct Directory *temp;

    temp=dopus_curwin[win]->firstentry;
    while (temp) {
        switch (ENTRYTYPE(temp->type)) {
            case ENTRY_FILE:
                if (temp->selected) {
                    dopus_curwin[win]->bytessel-=temp->size;
                    --dopus_curwin[win]->filesel;
                    temp->selected=0;
                }
                else {
                    dopus_curwin[win]->bytessel+=temp->size;
                    ++dopus_curwin[win]->filesel;
                    temp->selected=1;
                }
                break;
            case ENTRY_DIRECTORY:
                if (temp->selected) {
                    if (temp->size>0) dopus_curwin[win]->bytessel-=temp->size;
                    --dopus_curwin[win]->dirsel;
                    temp->selected=0;
                }
                else {
                    if (temp->size>0) dopus_curwin[win]->bytessel+=temp->size;
                    ++dopus_curwin[win]->dirsel;
                    temp->selected=1;
                }
                break;
        }
        temp=temp->next;
    }
    doselinfo(win);
}

void doselect(rexx)
int rexx;
{
    char buf[160];
    int boobs,prot[2];
    struct DateStamp ds1,ds2;
    int selecttype;

    if (dopus_curwin[data_active_window]->total==0 ||
        dopus_curwin[data_active_window]->firstentry->type==ENTRY_CUSTOM) return;

    if (!rexx) {
        boobs=getselectdata(str_select_pattern,&selecttype);
    }
    else {
        boobs=rexx_arg_value[0]; selecttype=rexx_arg_value[1];
        strcpy(str_select_pattern[selecttype],rexx_args[0]);
    }
    if (boobs) {
        if (dopus_curwin[data_active_window]->total==0) return;
        switch (selecttype) {
            case 0:
                LParsePatternI(str_select_pattern[0],buf);
                wildselect(buf,boobs,1,WILDSELECT_NAME);
                findfirstsel(data_active_window,-2);
                break;
            case 1:
                if (str_select_pattern[1][0]==0) break;
                getseldatestamps(str_select_pattern[1],&ds1,&ds2);
                dateselect(&ds1,&ds2,boobs,1);
                findfirstsel(data_active_window,-2);
                break;
            case 2:
                if (str_select_pattern[2][0]==0) break;
                getprotselvals(str_select_pattern[2],prot);
                protselect(prot[0],prot[1],boobs,1);
                findfirstsel(data_active_window,-2);
                break;
            case 3:
                LParsePatternI(str_select_pattern[3],buf);
                wildselect(buf,boobs,1,WILDSELECT_COMMENT);
                findfirstsel(data_active_window,-2);
                break;
        }
    }
}

void getseldatestamps(buf,ds1,ds2)
char *buf;
struct DateStamp *ds1,*ds2;
{
    char datebuf[2][85],timebuf[2][85],*ptr;
    int a/*,b*/;

    datebuf[0][0]=datebuf[1][0]=timebuf[0][0]=timebuf[1][0]=a/*=b*/=0;
    ptr=parsedatetime(buf/*str_select_pattern[1]*/,datebuf[0],timebuf[0],&a);
    switch (a) {
        case 0:
            strcpy(datebuf[1],datebuf[0]);
            strcpy(timebuf[1],"23:59:59");
            break;
        case -1:
            strcpy(datebuf[1],datebuf[0]);
            strcpy(timebuf[1],timebuf[0]);
            break;
        default:
            parsedatetime(ptr,datebuf[1],timebuf[1],&a/*&b*/);
            break;
    }
D(bug("getseldatestamps(%s,%s,%s,%s)\n",datebuf[0],timebuf[0],datebuf[1],timebuf[1]));
    strtostamp(datebuf[0],timebuf[0],ds1);
    strtostamp(datebuf[1],timebuf[1],ds2);
    ds1->ds_Tick=((ds1->ds_Tick/50)*50);
    ds2->ds_Tick=((ds2->ds_Tick/50)*50)+49;
D(bug("getseldatestamp: ds1=%ld,%ld,%ld\n",ds1->ds_Days,ds1->ds_Minute,ds1->ds_Tick));
D(bug("getseldatestamp: ds2=%ld,%ld,%ld\n",ds2->ds_Days,ds2->ds_Minute,ds2->ds_Tick));
}

void getprotselvals(buf,prot)
char *buf;
int *prot;
{
    int a,b,c,notflag=0;
    char ch;

    prot[0]=prot[1]=0;
    b=strlen(buf);
    for (a=0;a<b;a++) {
        ch=ToUpper(buf[a]);
        if (ch=='-') notflag=1;
        else if (ch=='+') notflag=0;
        else {
            for (c=0;c<8;c++) {
                if (ch==str_protbits[c]) {
                    prot[notflag]|=(1<<(7-c));
                    break;
                }
            }
        }
    }
}

void wildselect(wild,boobs,and,mode)
char *wild;
int boobs,and,mode;
{
    char buf[256];
    struct Directory *temp;

    temp=dopus_curwin[data_active_window]->firstentry;
#ifdef __SASC__
    if (!and)
    {
            dopus_curwin[data_active_window]->dirsel=dopus_curwin[data_active_window]->filesel= 0;
            dopus_curwin[data_active_window]->bytessel=0;
        }
#else
    if (!and) dopus_curwin[data_active_window]->dirsel=dopus_curwin[data_active_window]->filesel=dopus_curwin[data_active_window]->bytessel=0;
#endif
    while (temp) {
        if (temp->type!=ENTRY_CUSTOM || temp->subtype!=CUSTOMENTRY_USER ||
            temp->size&CUSTENTRY_CANSELECT) {
            if (!and) temp->selected=0;
            if (temp->type==ENTRY_CUSTOM) {
                if (!temp->selected && temp->comment) {
                    StrToUpper(temp->comment,buf);
                    if (LMatchPatternI(wild,buf)) wildselectthisone(temp,data_active_window,boobs);
                }
            }
            else {
                if (!temp->selected) {
                    StrToUpper((mode==WILDSELECT_NAME)?temp->name:temp->comment,buf);
                    if (LMatchPatternI(wild,buf)) wildselectthisone(temp,data_active_window,boobs);
                }
            }
        }
        temp=temp->next;
    }
    refreshwindow(data_active_window,0);
    doselinfo(data_active_window);
}

void dateselect(ds1,ds2,boobs,and)
struct DateStamp *ds1,*ds2;
int boobs,and;
{
    struct Directory *temp;

    temp=dopus_curwin[data_active_window]->firstentry;
    if (!and) {
        dopus_curwin[data_active_window]->dirsel=
            dopus_curwin[data_active_window]->filesel=
            dopus_curwin[data_active_window]->bytessel=0;
    }
    while (temp) {
        if (temp->type!=ENTRY_CUSTOM || temp->subtype!=CUSTOMENTRY_USER ||
            temp->size&CUSTENTRY_CANSELECT) {
            if (!and) temp->selected=0;
            if (!temp->selected &&
                CompareDate(&(temp->date),ds1)>=0 && CompareDate(ds2,&(temp->date))>=0)
                wildselectthisone(temp,data_active_window,boobs);
        }
        temp=temp->next;
    }
    refreshwindow(data_active_window,0);
    doselinfo(data_active_window);
}

void protselect(protyes,protno,boobs,and)
int protyes,protno,boobs,and;
{
    struct Directory *temp;
    int prot;

    temp=dopus_curwin[data_active_window]->firstentry;
#ifdef __SASC__
    if (!and)
    {
            dopus_curwin[data_active_window]->dirsel=dopus_curwin[data_active_window]->filesel=0;
            dopus_curwin[data_active_window]->bytessel=0;
        }
#else
    if (!and) dopus_curwin[data_active_window]->dirsel=dopus_curwin[data_active_window]->filesel=dopus_curwin[data_active_window]->bytessel=0;
#endif
    while (temp) {
        if (temp->type!=ENTRY_CUSTOM || temp->subtype!=CUSTOMENTRY_USER ||
            temp->size&CUSTENTRY_CANSELECT) {
            if (!and) temp->selected=0;
            if (!temp->selected) {
                prot=((~temp->protection)&15)+(temp->protection&~15);
                if ((prot&protyes)==protyes && ((prot&~protyes)&protno)==0)
                    wildselectthisone(temp,data_active_window,boobs);
            }
        }
        temp=temp->next;
    }
    refreshwindow(data_active_window,0);
    doselinfo(data_active_window);
}

void wildselectthisone(temp,win,boobs)
struct Directory *temp;
int win,boobs;
{
    if (boobs==1 && temp->type!=ENTRY_DEVICE) temp->selected=1;
    else if (boobs==2 && temp->type<=ENTRY_FILE) temp->selected=1;
    else if (boobs==3 && temp->type>=ENTRY_DIRECTORY) temp->selected=1;
    if (temp->selected) {
        if (temp->type>=ENTRY_DIRECTORY) {
            ++dopus_curwin[win]->dirsel;
            if (temp->size!=-1) dopus_curwin[win]->bytessel+=temp->size;
        }
        else if (temp->type<=ENTRY_FILE) {
            ++dopus_curwin[win]->filesel;
            dopus_curwin[win]->bytessel+=temp->size;
        }
    }
}

void doselinfo(win)
int win;
{
    char b1[24],b2[24];

    if (!dopus_curwin[win]->firstentry || dopus_curwin[win]->firstentry->type!=ENTRY_CUSTOM) {
        buildkmgstring(b1,/*(long long)*/dopus_curwin[win]->bytessel,config->listerdisplayflags[win] & SIZE_KMG);
        buildkmgstring(b2,/*(long long)*/dopus_curwin[win]->bytestot,config->listerdisplayflags[win] & SIZE_KMG);
D(bug("doselinfo(): b1 = %s, b2 = %s\n",b1,b2));
        lsprintf(str_select_info,globstring[STR_DIRS_FILES_BYTES_COUNT],
            dopus_curwin[win]->dirsel,dopus_curwin[win]->dirtot,dopus_curwin[win]->filesel,
            dopus_curwin[win]->filetot,b1,b2);
    }
    else {
        switch (dopus_curwin[win]->firstentry->subtype) {
            case CUSTOMENTRY_DIRTREE:
                lsprintf(str_select_info,globstring[STR_ENTRIES_IN_TREE],dopus_curwin[win]->total);
                break;
            case CUSTOMENTRY_BUFFERLIST:
                lsprintf(str_select_info,globstring[STR_DIRS_IN_BUFFERS],dopus_curwin[win]->total);
                break;
            case CUSTOMENTRY_USER:
                lsprintf(str_select_info,globstring[STR_USER_ENTRIES],dopus_curwin[win]->total);
                break;
        }
    }
    dostatustext(str_select_info);
}

int makeactive(win,state)
int win,state;
{
    if (data_active_window!=win) {
        data_active_window=win;
        return(doactive(state,1));
    }
    return(0);
}

int doactive(state,showinfo)
int state,showinfo;
{
    int oe;
    char buf[256];
    struct dopusfiletype *type;
    struct dopusfuncpar par;
    struct Directory *entry;

    if (status_iconified) return(0);
    if (showinfo) doselinfo(data_active_window);
    if (last_selected_entry &&
        last_selected_entry->type<=ENTRY_FILE) {
        if (state && config->generalflags&GENERAL_DOUBLECLICK) {
            if (DoubleClick(time_previous_sec,time_previous_micro,
                time_current_sec,time_current_micro)) {
                data_active_window=1-data_active_window;
                unselect(data_active_window,last_selected_entry);
                entry=last_selected_entry;
                dostatustext(globstring[STR_INTERROGATING_FILE]);
                StrCombine(buf,str_pathbuffer[data_active_window],last_selected_entry->name,256);
                busy();
                if ((type=checkfiletype(buf,FTFUNC_CLICKMCLICK,0))) {
                    char title[256];

                    strcpy(func_single_file,last_selected_entry->name);
                    par.which=type->which[FTFUNC_CLICKMCLICK];
                    par.stack=type->stack[FTFUNC_CLICKMCLICK];
                    par.key=par.qual=0; par.type=3;
                    par.pri=type->pri[FTFUNC_CLICKMCLICK];
                    par.delay=type->delay[FTFUNC_CLICKMCLICK];

                    if (type->actionstring[FTFUNC_CLICKMCLICK][0]) {
                        do_title_string(type->actionstring[FTFUNC_CLICKMCLICK],title,0,NULL);
                        dostatustext(title);
                    }
                    else title[0]=0;

                    dofunctionstring(type->function[FTFUNC_CLICKMCLICK],
                        last_selected_entry->name,title,&par);
                    unbusy();
                    checkselection(entry);
                    return(1);
                }
                unbusy();
                return(0);
            }
        }
        last_selected_entry=NULL;
    }
    for (oe=0;oe<2;oe++) displayname(oe,1);
    if (last_selected_entry &&
        last_selected_entry->type>=ENTRY_DEVICE && state) {
        if (DoubleClick(time_previous_sec,time_previous_micro,
            time_current_sec,time_current_micro)) {
            if (last_selected_entry->type==ENTRY_CUSTOM &&
                last_selected_entry->subtype==CUSTOMENTRY_USER) {
                if (last_selected_entry->size&CUSTENTRY_CANSELECT) {
                    userentrymessage(dopus_curwin[1-data_active_window],last_selected_entry,USERENTRY_CLICKMCLICK);
                    time_previous_sec=0;
                }
            }
            else {
                unselect(1-data_active_window,last_selected_entry);
                advancebuf(data_active_window,1);
                if (last_selected_entry->type==ENTRY_CUSTOM) {
                    if (last_selected_entry->subtype==CUSTOMENTRY_BUFFERLIST) {
                        bringinbuffer(last_selected_entry->dispstr,data_active_window,1);
                        time_previous_sec=0;
                        return(1);
                    }
                    else strcpy(str_pathbuffer[data_active_window],last_selected_entry->dispstr);
                }
                else {
                    strcpy(str_pathbuffer[data_active_window],dopus_curwin[1-data_active_window]->directory);
                    TackOn(str_pathbuffer[data_active_window],last_selected_entry->name,256);
                }
                startgetdir(data_active_window,SGDFLAGS_CANMOVEEMPTY|SGDFLAGS_CANCHECKBUFS);
                time_previous_sec=0;
                return(1);
            }
        }
    }
    return(0);
}

void unbyte(win)
int win;
{
    struct Directory *entry;

    entry=dopus_curwin[win]->firstentry;
    while (entry) {
        if (entry->selected && ENTRYTYPE(entry->type)==ENTRY_DIRECTORY &&
            entry->size!=-1) {
            setdirsize(entry,-1,win);
        }
        if (entry->next) entry=entry->next;
        else entry=NULL;
    }
    refreshwindow(win,0);
    if (win==data_active_window) doselinfo(win);
}

void checkselection(entry)
struct Directory *entry;
{
    struct Directory *temp;

    last_selected_entry=NULL; func_single_file[0]=0;
    temp=dopus_curwin[data_active_window]->firstentry;
    while (temp) {
        if (temp==entry) break;
        temp=temp->next;
    }
    if (temp && temp->selected) unselect(data_active_window,temp);
}
