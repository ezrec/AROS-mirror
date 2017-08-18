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

int checkexec(name)
char *name;
{
#ifdef __AROS__
    struct FileInfoBlock __aligned fib;

    lockandexamine(name, &fib);
    return !(fib.fib_Protection & FIBF_EXECUTE);
//    kprintf("!!!!!!!! IMPROVE THE checkexec() function in main8.c\n");
//    return TRUE;
//#warning Get a better check here
#else
    int file,check;

    if (!(file=Open(name,MODE_OLDFILE))) return(0);
    Read(file,(char *)&check,4);
    Close(file);
    return((check==0x3f3));
#endif
}

void newcli(char *spec)
{
    char buf[200];

    lsprintf(buf,"%s \"%s\"",config->outputcmd,spec?spec:config->output);
    if (!(Execute(buf,0,nil_file_handle))) doerror(-1);
    else {
        WBenchToFront();
        okay();
    }
}

void getprot(protect,buffer)
int protect;
char *buffer;
{
    int a;

    protect ^= 0x000F;
    for (a=7;a>=0;a--) buffer[7-a] = (protect&(1<<a)) ? str_protbits[7-a] : '-';
    buffer[8]=0;
//D(bug("protection: %lx = %s\n",protect,buffer));
}

int getprotval(buf)
char *buf;
{
    int a,b,c,prot=15;

    a=strlen(buf);
    for (b=0;b<a;b++) {
        buf[b]=ToUpper(buf[b]);
        for (c=0;c<8;c++) {
            if (buf[b]==str_protbits[7-c]) {
                if (c<4) prot&=~(1<<c);
                else prot|=(1<<c);
                break;
            }
        }
    }
    return(prot);
}

int checkexistreplace(sourcename,destname,date,allabort,all)
char *sourcename,*destname;
struct DateStamp *date;
int allabort,all;
{
    struct FileInfoBlock __aligned d_fib;
    struct FileInfoBlock __aligned s_fib;
    char buf[400],datebuf1[40],datebuf2[40];
    int a,suc_dfib;
    struct DateStamp ds;

//D(bug("checkexistreplace(%s,%s,...)\n",sourcename,destname));
    if (!(lockandexamine(sourcename,&s_fib))) return(REPLACE_OK);
    if (!(suc_dfib=lockandexamine(destname,&d_fib))) return(REPLACE_OK);

//HUX Prevent directories and files from being treated differently
    if (suc_dfib && d_fib.fib_DirEntryType>0) {
        if (s_fib.fib_DirEntryType<0) {
            doerror(ERROR_OBJECT_EXISTS);
            return(REPLACE_ABORT);
        }
        return(REPLACE_OK);
    }
//HUX

    if (config->existflags&REPLACE_ALWAYS) return(REPLACE_OK);
    else if (config->existflags&REPLACE_NEVER) {
        doerror(ERROR_OBJECT_EXISTS);
//        return(3/*0*/);
    }
    else if (config->existflags&REPLACE_OLDER) {
        if (!date) {
            DateStamp(&ds);
            date=&ds;
        }
        if (suc_dfib) {
            if (CompareDate(date,&(d_fib.fib_Date))>0) return(REPLACE_OK);
            doerror(ERROR_OBJECT_EXISTS);
        }
//        return(3/*0*/);
    }
    else if (config->existflags&REPLACE_ASK) {
        doerror(ERROR_OBJECT_EXISTS);
        if (sourcename==destname || !suc_dfib) {
            lsprintf(buf,globstring[STR_FILE_EXISTS_REPLACE],BaseName(destname));
        }
        else {
            seedate(&s_fib.fib_Date,datebuf1,0);
            seedate(&d_fib.fib_Date,datebuf2,0);

// HUX begin (Depends on what we're overwriting)
            if ( d_fib.fib_DirEntryType > 0 && s_fib.fib_DirEntryType > 0 )
            {
                /* Both entries are directories */
                lsprintf( buf, globstring[ STR_FILE_EXISTS_REPLACE ], BaseName( destname ) );
            }
            else if ( d_fib.fib_DirEntryType > 0 )
            {
                /* Destination is directory, source is file */
                lsprintf( buf, globstring[STR_REPLACE_DIR_WITH_FILE], BaseName( destname ),
                s_fib.fib_Size, datebuf1, datebuf2 );
            }
            else if ( s_fib.fib_DirEntryType > 0 )
            {
                /* Source is directory, destination is file */
                lsprintf( buf, globstring[STR_REPLACE_FILE_WITH_DIR], BaseName( destname ),
                datebuf1, d_fib.fib_Size, datebuf2 );
            }
            else
            {
                /* Both entries are files */
                lsprintf( buf, globstring[ STR_OLD_NEW_FILE_REPLACE ], BaseName( destname ),
                s_fib.fib_Size, datebuf1, d_fib.fib_Size, datebuf2 );
            }
// HUX: end
/*
            lsprintf(buf,globstring[STR_OLD_NEW_FILE_REPLACE],BaseName(destname),
                s_fib.fib_Size,datebuf1,d_fib.fib_Size,datebuf2);
*/
        }
        do
         {
/*        if (all)*/ a=simplerequest(buf,
                                 globstring[STR_REPLACE],                        // REPLACE_OK
                                 globstring[STR_ABORT],                          // REPLACE_ABORT
                                 globstring[allabort ? STR_REPLACE_ALL : STR_TRY_AGAIN], // REPLACE_ALL/REPLACE_RETRY
                                 globstring[STR_RENAME_REQ],
                                 "\n",
                                 globstring[STR_SKIP],                           // REPLACE_SKIP
                                 globstring[STR_SKIP_ALL],                       // REPLACE_SKIPALL
                                 NULL);
/*
        else a=simplerequest(buf,globstring[STR_REPLACE],                          // REPLACE_OK
                                 globstring[STR_SKIP],                             // REPLACE_ABORT
                                 globstring[allabort ? STR_ABORT : STR_TRY_AGAIN], // REPLACE_ALL/REPLACE_RETRY
                                 NULL,NULL,NULL);
*/

          if (a == REPLACE_RENAME)
           {
            char dname[FILEBUF_SIZE];

            strcpy(dname,BaseName(sourcename));

            if (whatsit(globstring[STR_ENTER_NEW_NAME],FILEBUF_SIZE,dname,NULL))
             {
              *BaseName(destname) = 0;
              strcat(destname,dname);
             }
           }
          else break;
         }
        while(CheckExist(destname,NULL));
D(bug("checkexistreplace() returned %ld\n",a));
        return(a);
    }
    return REPLACE_SKIP/*0*/;
}

int lockandexamine(name,fib)
char *name;
struct FileInfoBlock *fib;
{
    BPTR lock;

    fib->fib_OwnerUID=fib->fib_OwnerGID=0;
    if (!(lock=Lock(name,ACCESS_READ))) return(0);
    Examine(lock,fib);
    UnLock(lock);
    return(1);
}

void layoutcenter(off)
int off;
{
    int a,max;
    int tiny_extra;

    removewindowgadgets(Window);

    scrdata_diskname_width[0]=scrdata_diskname_width[1]=(scrdata_width/2)-4;
    scrdata_dispwin_width[0]=scrdata_dispwin_width[1]=(scrdata_width-24-config->sliderwidth*2)/2;
    scrdata_dirwin_center=((scrdata_width/2)-2)+scrdata_xoffset;

    max=(scrdata_dirwin_center-((config->sliderwidth*2)+(scrdata_font_xsize*10)))/*/scrdata_font_xsize*/;
    if (off>max) off=max;
    else if (off<-max) off=-max;

    scrdata_diskname_width[0]+=/*scrdata_font_xsize**/off;
    scrdata_diskname_width[1]-=/*scrdata_font_xsize**/off;
    scrdata_dispwin_width[0]+=/*scrdata_font_xsize**/off;
    scrdata_dispwin_width[1]-=/*scrdata_font_xsize**/off;

    scrdata_dispwin_center=scrdata_xoffset+scrdata_diskname_width[0]+4;

    for (a=0;a<2;a++) {
        vert_propgad[a].Width=config->sliderwidth;
        vert_propgad[a].Height=scrdata_prop_height;
    }

    if (config->slider_pos&1) {
        vert_propgad[0].LeftEdge=scrdata_dispwin_center-config->sliderwidth-4;
        scrdata_dispwin_left[0]=scrdata_xoffset;
    }
    else {
        vert_propgad[0].LeftEdge=scrdata_xoffset+4;
        scrdata_dispwin_left[0]=vert_propgad[0].LeftEdge+vert_propgad[0].Width+4;
    }

    if (config->slider_pos&2) {
        scrdata_dispwin_left[1]=scrdata_dispwin_center;
        vert_propgad[1].LeftEdge=scrdata_dispwin_left[1]+scrdata_dispwin_width[1]+8;
        tiny_extra=config->sliderwidth+8;
    }
    else {
        vert_propgad[1].LeftEdge=scrdata_dispwin_center+4;
        scrdata_dispwin_left[1]=vert_propgad[1].LeftEdge+vert_propgad[0].Width+4;
        tiny_extra=0;
    }

    scrdata_dispwin_xpos[0]=scrdata_dispwin_left[0];
    scrdata_dispwin_xpos[1]=scrdata_dispwin_left[1];

    scrdata_diskname_xpos[0]=scrdata_xoffset;
    scrdata_diskname_xpos[1]=scrdata_xoffset+scrdata_diskname_width[0]+4;

    for (a=0;a<2;a++) {
        scrdata_dispwin_nchars[a]=scrdata_dispwin_width[a]/*/scrdata_font_xsize*/;
//        if ((scrdata_dispwin_nchars[a]*scrdata_font_xsize)>scrdata_dispwin_width[a]-2)
//            --scrdata_dispwin_nchars[a];
        scrdata_dirwin_width[a]=scrdata_dispwin_nchars[a]/**scrdata_font_xsize*/;
        scrdata_dirwin_xpos[a]=scrdata_dispwin_xpos[a]+2+((scrdata_dispwin_width[a]-scrdata_dirwin_width[a])/2);
    }

    for (a=0;a<2;a++) {
        screen_gadgets[SCRGAD_MOVEUP1+a].TopEdge=scrdata_dispwin_ypos-1;
        screen_gadgets[SCRGAD_MOVEDOWN1+a].TopEdge=
            scrdata_dispwin_ypos+
            scrdata_prop_height+
            config->arrowsize[0]+4;

        switch (config->arrowpos[0]) {
            case ARROWS_INSIDE:
                screen_gadgets[SCRGAD_MOVEUP1+a].TopEdge=
                    scrdata_dispwin_ypos+
                    scrdata_prop_height+3;
                vert_propgad[a].TopEdge=scrdata_dispwin_ypos+1;
                break;
            case ARROWS_OUTSIDE:
                screen_gadgets[SCRGAD_MOVEDOWN1+a].TopEdge=scrdata_dispwin_ypos+config->arrowsize[0];
                vert_propgad[a].TopEdge=scrdata_dispwin_ypos+((config->arrowsize[0]*2)+3);
                break;
            case ARROWS_EACHSIDE:
                vert_propgad[a].TopEdge=scrdata_dispwin_ypos+config->arrowsize[0]+2;
                break;
        }
    }

    for (a=0;a<2;a++) {
        path_strgadget[a].TopEdge=scrdata_dirwin_bottom+30+config->sliderheight;
        path_strgadget[a].Width=scrdata_dispwin_width[a]-((config->arrowsize[2]*2)+20);
        path_strgadget[a].Height=scrdata_string_height;

        horiz_propgad[a].TopEdge=scrdata_dirwin_bottom+26;
        horiz_propgad[a].Width=scrdata_dispwin_width[a]-((config->arrowsize[1]*2)+20);
        horiz_propgad[a].Height=config->sliderheight;

        screen_gadgets[SCRGAD_TINYBUFFERLIST+a].TopEdge=scrdata_dirwin_bottom+24;
        screen_gadgets[SCRGAD_TINYBUFFERLIST+a].Height=config->sliderheight+4;
        screen_gadgets[SCRGAD_TINYSELECT+a].TopEdge=scrdata_dirwin_bottom+28+config->sliderheight;
        screen_gadgets[SCRGAD_TINYSELECT+a].Height=scrdata_string_height+4;
        screen_gadgets[SCRGAD_MOVEUP1+a].LeftEdge=
            screen_gadgets[SCRGAD_MOVEDOWN1+a].LeftEdge=vert_propgad[a].LeftEdge-4;
        screen_gadgets[SCRGAD_LEFTPARENT+a].Height=scrdata_dispwin_height;
        screen_gadgets[SCRGAD_LEFTPARENT+a].TopEdge=scrdata_dispwin_ypos;
    }

    switch (config->arrowpos[1]) {
        case ARROWS_INSIDE:
            horiz_propgad[0].LeftEdge=scrdata_xoffset+4;
            screen_gadgets[SCRGAD_MOVELEFT1].LeftEdge=
                horiz_propgad[0].LeftEdge+horiz_propgad[0].Width+4;
            screen_gadgets[SCRGAD_MOVERIGHT1].LeftEdge=
                horiz_propgad[0].LeftEdge+horiz_propgad[0].Width+config->arrowsize[1]+12;

            screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge=
                scrdata_dispwin_xpos[1]+tiny_extra;
            screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge=
                screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge+config->arrowsize[1]+8;
            horiz_propgad[1].LeftEdge=
                screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge+config->arrowsize[1]+12;
            break;

        case ARROWS_OUTSIDE:
            screen_gadgets[SCRGAD_MOVELEFT1].LeftEdge=scrdata_xoffset;
            screen_gadgets[SCRGAD_MOVERIGHT1].LeftEdge=scrdata_xoffset+config->arrowsize[1]+8;
            horiz_propgad[0].LeftEdge=scrdata_xoffset+(config->arrowsize[1]*2)+20;

            horiz_propgad[1].LeftEdge=
                scrdata_dispwin_xpos[1]+4+tiny_extra;
            screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge=
                horiz_propgad[1].LeftEdge+horiz_propgad[1].Width+4;
            screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge=
                screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge+
                config->arrowsize[1]+8;
            break;

        case ARROWS_EACHSIDE:
            screen_gadgets[SCRGAD_MOVELEFT1].LeftEdge=scrdata_xoffset;
            horiz_propgad[0].LeftEdge=scrdata_xoffset+config->arrowsize[1]+12;
            screen_gadgets[SCRGAD_MOVERIGHT1].LeftEdge=
                scrdata_xoffset+
                horiz_propgad[0].Width+config->arrowsize[1]+16;

            screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge=
                scrdata_dispwin_xpos[1]+tiny_extra;
            horiz_propgad[1].LeftEdge=
                screen_gadgets[SCRGAD_MOVELEFT2].LeftEdge+config->arrowsize[1]+12;
            screen_gadgets[SCRGAD_MOVERIGHT2].LeftEdge=
                horiz_propgad[1].LeftEdge+horiz_propgad[1].Width+4;
            break;
    }

    switch (config->arrowpos[2]) {
        case ARROWS_INSIDE:
            path_strgadget[0].LeftEdge=scrdata_xoffset+4;
            screen_gadgets[SCRGAD_BUFFERLEFT1].LeftEdge=
                path_strgadget[0].LeftEdge+path_strgadget[0].Width+4;
            screen_gadgets[SCRGAD_BUFFERRIGHT1].LeftEdge=
                path_strgadget[0].LeftEdge+path_strgadget[0].Width+config->arrowsize[2]+12;

            screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge=
                scrdata_dispwin_xpos[1]+tiny_extra;
            screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge=
                screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge+config->arrowsize[2]+8;
            path_strgadget[1].LeftEdge=
                screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge+config->arrowsize[2]+12;
            break;

        case ARROWS_OUTSIDE:
            screen_gadgets[SCRGAD_BUFFERLEFT1].LeftEdge=scrdata_xoffset;
            screen_gadgets[SCRGAD_BUFFERRIGHT1].LeftEdge=scrdata_xoffset+config->arrowsize[2]+8;
            path_strgadget[0].LeftEdge=scrdata_xoffset+(config->arrowsize[2]*2)+20;

            path_strgadget[1].LeftEdge=scrdata_dispwin_xpos[1]+4+tiny_extra;
            screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge=
                path_strgadget[1].LeftEdge+path_strgadget[1].Width+4;
            screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge=
                screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge+
                config->arrowsize[2]+8;
            break;

        case ARROWS_EACHSIDE:
            screen_gadgets[SCRGAD_BUFFERLEFT1].LeftEdge=scrdata_xoffset;
            path_strgadget[0].LeftEdge=scrdata_xoffset+config->arrowsize[2]+12;
            screen_gadgets[SCRGAD_BUFFERRIGHT1].LeftEdge=
                scrdata_xoffset+path_strgadget[0].Width+config->arrowsize[2]+16;

            screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge=
                scrdata_dispwin_xpos[1]+tiny_extra;
            path_strgadget[1].LeftEdge=
                screen_gadgets[SCRGAD_BUFFERLEFT2].LeftEdge+config->arrowsize[2]+12;
            screen_gadgets[SCRGAD_BUFFERRIGHT2].LeftEdge=
                path_strgadget[1].LeftEdge+path_strgadget[1].Width+4;
            break;
    }

    for (a=0;a<4;a++) {
        screen_gadgets[SCRGAD_BUFFERLEFT1+a].TopEdge=scrdata_dirwin_bottom+28+config->sliderheight;
        screen_gadgets[SCRGAD_BUFFERLEFT1+a].Height=scrdata_string_height+4;
        screen_gadgets[SCRGAD_MOVELEFT1+a].TopEdge=scrdata_dirwin_bottom+24;
        screen_gadgets[SCRGAD_MOVELEFT1+a].Height=config->sliderheight+4;
        screen_gadgets[SCRGAD_MOVEUP1+a].Width=
            screen_gadgets[SCRGAD_TINYBUFFERLIST+a].Width=config->sliderwidth+8;
    }

    screen_gadgets[SCRGAD_MOVEUP1].Height=config->arrowsize[0]+1;
    screen_gadgets[SCRGAD_MOVEDOWN1].Height=config->arrowsize[0]+1;
    screen_gadgets[SCRGAD_MOVEUP2].Height=config->arrowsize[0]+1;
    screen_gadgets[SCRGAD_MOVEDOWN2].Height=config->arrowsize[0]+1;

    screen_gadgets[SCRGAD_MOVELEFT1].Width=config->arrowsize[1]+8;
    screen_gadgets[SCRGAD_MOVERIGHT1].Width=config->arrowsize[1]+8;
    screen_gadgets[SCRGAD_MOVELEFT2].Width=config->arrowsize[1]+8;
    screen_gadgets[SCRGAD_MOVERIGHT2].Width=config->arrowsize[1]+8;

    screen_gadgets[SCRGAD_BUFFERLEFT1].Width=config->arrowsize[2]+8;
    screen_gadgets[SCRGAD_BUFFERRIGHT1].Width=config->arrowsize[2]+8;
    screen_gadgets[SCRGAD_BUFFERLEFT2].Width=config->arrowsize[2]+8;
    screen_gadgets[SCRGAD_BUFFERRIGHT2].Width=config->arrowsize[2]+8;

    screen_gadgets[SCRGAD_TINYBUFFERLIST].LeftEdge=
        screen_gadgets[SCRGAD_TINYSELECT].LeftEdge=
        scrdata_dispwin_center-config->sliderwidth-8;
    screen_gadgets[SCRGAD_TINYRESELECT].LeftEdge=
        screen_gadgets[SCRGAD_TINYAREXX].LeftEdge=
        scrdata_dispwin_center;

    screen_gadgets[SCRGAD_LEFTPARENT].LeftEdge=0;
    screen_gadgets[SCRGAD_RIGHTPARENT].LeftEdge=scrdata_xoffset+(scrdata_width-2);

    for (a=SCRGAD_TINYBUFFERLIST;a<=SCRGAD_BUFFERRIGHT2;a++) {
        screen_gadgets[a].GadgetRender=NULL;
        screen_gadgets[a].SelectRender=NULL;
    }
}

void fix_rmb_gadgets()
{
    int a;

    mainrmbgads[0].x=scrdata_gadget_offset;
    mainrmbgads[0].y=scrdata_gadget_ypos-1;
    mainrmbgads[0].w=scrdata_drive_width;
    mainrmbgads[0].h=scr_gadget_rows*scrdata_gadget_height;

    if (drive_bank_unsel_border) {
        mainrmbgads[0].flags|=RGF_ALTBORD;
        mainrmbgads[0].high_bord[0]=drive_bank_unsel_border;
        mainrmbgads[0].high_bord[1]=drive_bank_sel_border;
    }
    else mainrmbgads[0].flags&=~RGF_ALTBORD;

    for (a=0;a<4;a++)
        makermbgad(&mainrmbgads[a+1],&screen_gadgets[SCRGAD_TINYBUFFERLIST+a],1);
    for (a=0;a<2;a++)
        makermbgad(&mainrmbgads[a+5],&screen_gadgets[SCRGAD_LEFTPARENT+a],0);
}

void dosizedirwindows(size)
int size;
{
    ULONG class;
    UWORD code;
    int x,ox,fail=0,max;

    max=(scrdata_dirwin_center-((config->sliderwidth*2)+(scrdata_font_xsize*10)))/*/scrdata_font_xsize*/;
    if (size!=65536) {
        if (size>max) size=max;
        else if (size<-max) size=-max;
        ox=size; fail=-1;
    }
    else {
        if ((scrdata_width-24-config->sliderwidth*2)<scrdata_font_xsize*24) return;

        SetDrMd(main_rp,COMPLEMENT);
        sizedirwindow_gfx((ox=config->windowdelta));
        Window->Flags|=WFLG_REPORTMOUSE;

        FOREVER {
            while (getintuimsg()) {
                if (IMsg->MouseX<scrdata_dirwin_center) {
                    x=(IMsg->MouseX-(scrdata_dirwin_center+4))/*/scrdata_font_xsize*/;
                }
                else {
                    x=(IMsg->MouseX-scrdata_dirwin_center)/*/scrdata_font_xsize*/;
                }

                if (x>max) x=max;
                else if (x<-max) x=-max;
                class=IMsg->Class; code=IMsg->Code;
                ReplyMsg((struct Message *)IMsg);
                if (class==IDCMP_MOUSEBUTTONS) {
                    if (code==MENUDOWN) {
                        fail=1;
                        break;
                    }
                    if (code==SELECTUP) {
                        fail=-1;
                        break;
                    }
                }
                else if (class==IDCMP_MOUSEMOVE) {
                    if (x!=ox) {
                        sizedirwindow_gfx(ox);
                        sizedirwindow_gfx((ox=x));
                    }
                }
                else if (class==IDCMP_RAWKEY) {
                    if (code==0x45) {
                        fail=-1;
                        sizedirwindow_gfx(ox);
                        sizedirwindow_gfx((ox=0));
                        break;
                    }
                }
            }
            if (fail) break;
            Wait(1<<Window->UserPort->mp_SigBit);
        }
        sizedirwindow_gfx(ox);
        SetDrMd(main_rp,JAM2);
        Window->Flags&=~WFLG_REPORTMOUSE;
    }
    if (fail==-1 && ox!=config->windowdelta && !status_iconified) {
        busy();
        layoutcenter((config->windowdelta=ox));
        fix_rmb_gadgets();
        SetAPen(main_rp,screen_pens[0].pen);
        rectfill(main_rp,
            scrdata_dispwin_center-config->sliderwidth-7,
            scrdata_dispwin_ypos-1,
            (config->sliderwidth*2)+13,
            scrdata_dispwin_height+2);
        RectFill(main_rp,
            scrdata_xoffset,
            horiz_propgad[0].TopEdge-2,
            scrdata_xoffset+scrdata_width-1,
            path_strgadget[0].TopEdge+path_strgadget[0].Height+1);
        setupdisplay(0);
        for (x=0;x<2;x++) RefreshStrGad(&path_strgadget[x],Window);
        okay();
        unbusy();
    }
}

void sizedirwindow_gfx(dx)
int dx;
{
    int x;

    x=scrdata_dirwin_center+(dx/**scrdata_font_xsize*/);
    rectfill(main_rp,x,scrdata_dispwin_ypos,4,scrdata_dispwin_height);
}

void fix_requester(req,title)
struct RequesterBase *req;
char *title;
{
    req->rb_fg=screen_pens[config->requestfg].pen;
    req->rb_bg=screen_pens[config->requestbg].pen;
    req->rb_shine=screen_pens[config->gadgettopcol].pen;
    req->rb_shadow=screen_pens[config->gadgetbotcol].pen;
    req->rb_font=scr_font[FONT_REQUEST];
    req->rb_screen=Window->WScreen;
    req->rb_extend=&requester_stringex;
    fix_stringex(&requester_stringex);
    if (config->generalscreenflags&SCR_GENERAL_REQDRAG) {
        req->rb_title=title;
        req->rb_flags=RBF_BORDERS|RBF_STRINGS;
    }
    else {
        req->rb_title=NULL;
        req->rb_flags=RBF_STRINGS;
    }
    req->rb_string_table=globstring;
    if (status_publicscreen) {
        req->rb_window=Window;
        req->rb_flags|=RBF_WINDOWCENTER;
    }
    else req->rb_flags&=~RBF_WINDOWCENTER;
}

void fix_stringex(extend)
struct StringExtend *extend;
{
    extend->Font=scr_font[FONT_REQUEST];
    extend->Pens[0]=screen_pens[config->stringfgcol].pen;
    extend->Pens[1]=screen_pens[config->stringbgcol].pen;
    extend->ActivePens[0]=screen_pens[config->stringselfgcol].pen;
    extend->ActivePens[1]=screen_pens[config->stringselbgcol].pen;
}

struct Gadget *addreqgadgets(reqbase,gadgets,mask,count)
struct RequesterBase *reqbase;
struct TagItem **gadgets;
int mask,*count;
{
    int gad;
    struct Gadget *gadget=NULL,*newgadget,*firstgadget=NULL;

    for (gad=0;;gad++) {
        if (!gadgets[gad]) break;
        if (gad>31 || !(mask&(1<<gad))) {
            if (!(newgadget=(struct Gadget *)
                AddRequesterObject(reqbase,gadgets[gad]))) return(NULL);
            if (gadget) gadget->NextGadget=newgadget;
            else firstgadget=newgadget;
            gadget=newgadget;
            if (count) ++*count;
        }
    }
    if (firstgadget) AddGadgets(reqbase->rb_window,firstgadget,NULL,gad,reqbase->rb_shine,reqbase->rb_shadow,1);
    return(firstgadget);
}

void set_reqobject(object,tagmatch,data)
struct TagItem *object;
ULONG tagmatch,data;
{
    int tag;

    for (tag=0;;tag++) {
        if (object[tag].ti_Tag==TAG_END) break;
        if (object[tag].ti_Tag==tagmatch) {
            object[tag].ti_Data=data;
            break;
        }
    }
}

void FadeRGB32(screen,cmap,count,dir,fadetime)
struct Screen *screen;
ULONG *cmap;
int count,dir,fadetime;
{
    ULONG *tempmap;
    int i,j,s,t,n,mod,val,orval,maxval;
    struct timerequest treq;
    struct MsgPort *fade_port=NULL;
    struct DisplayInfo dinfo;

    if (!(tempmap=AllocMem(((count*3)+2)*sizeof(ULONG),MEMF_CLEAR)) ||
        fadetime<1 ||
        !(fade_port=LCreatePort(NULL,0)) ||
        !(GetDisplayInfoData(NULL,
            (char *)&dinfo,sizeof(struct DisplayInfo),
            DTAG_DISP,GetVPModeID(&screen->ViewPort))) ||
        (OpenDevice(TIMERNAME,UNIT_VBLANK,&treq.tr_node,0))) {
        if (tempmap) {
            CopyMem((char *)cmap,(char *)&tempmap[1],count*3*sizeof(ULONG));
            tempmap[0]=count<<16;
            LoadRGB32(&screen->ViewPort,tempmap);
        }
        else load_palette(screen,cmap);
    }
    else {
        maxval=1<<dinfo.RedBits;
        if (dinfo.RedBits<4) i=1;
        else i=maxval>>4;

        tempmap[0]=count<<16;

        if (dir==1) {
            s=maxval-1; t=-1; n=-i;
            LoadRGB32(&screen->ViewPort,tempmap);
        }
        else {
            s=1; t=maxval; n=i;
        }
        mod=32-dinfo.RedBits;
        orval=(1<<mod)-1;

        treq.tr_node.io_Message.mn_ReplyPort=fade_port;
        treq.tr_node.io_Command=TR_ADDREQUEST;
        treq.tr_node.io_Flags=0;

        for (i=s;i+=n;) {
            if (dir==1) {
                if (i<=t) break;
            }
            else {
                if (i>=t) break;
            }
            for (j=0;j<count*3;j++) {
                val=(cmap[j]>>mod)-i;
                if (val<0) val=0;
                else if (val>=maxval) val=maxval-1;
                tempmap[j+1]=(val<<mod)|orval;
            }

            treq.tr_time.tv_secs=0;
            treq.tr_time.tv_micro=fadetime*22000;
            SendIO(&treq.tr_node);
            WaitBOVP(&screen->ViewPort);
            LoadRGB32(&screen->ViewPort,tempmap);
            WaitIO(&treq.tr_node);
        }
        if (dir>0) CopyMem((char *)cmap,(char *)&tempmap[1],count*3*sizeof(ULONG));
        else for (j=0;j<count*3;j++) tempmap[j+1]=0;
        LoadRGB32(&screen->ViewPort,tempmap);
        CloseDevice(&treq.tr_node);
    }
    if (fade_port) LDeletePort(fade_port);
    if (tempmap) FreeMem(tempmap,((count*3)+2)*sizeof(ULONG));
}

void FadeRGB4(screen,cmap,count,dir,fadetime)
struct Screen *screen;
UWORD *cmap;
int count,dir,fadetime;
{
    UWORD *tempmap=NULL;
    char red,green,blue;
    int i,j,s,t,n;
    struct timerequest treq;
    struct MsgPort *fade_port=NULL;

    if (dir==1) {
        s=15; t=-1; n=-1;
    }
    else {
        s=1; t=16; n=1;
    }

    if (fadetime<1 ||
        !(tempmap=AllocMem(count*sizeof(UWORD),MEMF_CLEAR)) ||
        !(fade_port=LCreatePort(NULL,0)) ||
        (OpenDevice(TIMERNAME,UNIT_VBLANK,&treq.tr_node,0))) {
        LoadRGB4(&screen->ViewPort,cmap,count);
    }
    else {
        treq.tr_node.io_Message.mn_ReplyPort=fade_port;
        treq.tr_node.io_Command=TR_ADDREQUEST;
        treq.tr_node.io_Flags=0;

        if (dir==1) LoadRGB4(&screen->ViewPort,tempmap,count);
        for (i=s;i!=t;i+=n) {
            for (j=0;j<count;j++) {
                red=((cmap[j]>>8)&0xf)-i;
                green=((cmap[j]>>4)&0xf)-i;
                blue=(cmap[j]&0xf)-i;
                if (red<0) red=0;
                if (green<0) green=0;
                if (blue<0) blue=0;
                tempmap[j]=(UWORD)red<<8|(UWORD)green<<4|(UWORD)blue;
            }
            treq.tr_time.tv_secs=0;
            treq.tr_time.tv_micro=fadetime*20000;
            SendIO(&treq.tr_node);
            WaitBOVP(&screen->ViewPort);
            LoadRGB4(&screen->ViewPort,tempmap,count);
            WaitIO(&treq.tr_node);
        }
        CloseDevice(&treq.tr_node);
    }
    if (fade_port) LDeletePort(fade_port);
    if (tempmap) FreeMem(tempmap,count*sizeof(UWORD));
}

void removewindowgadgets(window)
struct Window *window;
{
    if (window && window->FirstGadget) {
        struct Gadget *gad;

        FOREVER {
            gad=window->FirstGadget;
            while (gad) {
                if (!(gad->GadgetType&GTYP_SYSGADGET)) {
                    RemoveGList(window,gad,1);
                    break;
                }
                gad=gad->NextGadget;
            }
            if (!gad) break;
        }
    }
}
