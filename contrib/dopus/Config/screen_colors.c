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

#include "config.h"

void showcolourvals(pen,prop)
int pen,prop;
{
    int a/*,c*/;
    ULONG /*b,d,*/cbuf[3];

    SetAPen(rp,pen);
    RectFill(rp,
        x_off+252,y_off+34,
        x_off+287,y_off+45);
    /*if (version2>=OSVER_39)*/ {
        GetRGB32(vp->ColorMap,pen,1,cbuf);
        for (a=0;a<3;a++) {
            scalecol(&cbuf[a]);
            lsprintf(palette_buf[a],"%ld",(long int)cbuf[a]);
            if (prop) FixSliderPot(Window,&palettegads[a],cbuf[a],1<<bpg,1,1);
            ShowSlider(Window,&palettegads[a]);
            RefreshGList(&palettegads[a+3],Window,NULL,1);
        }
    }
/*    else {
        b=GetRGB4(vp->ColorMap,pen);
        for (a=0,c=8;a<3;a++,c-=4) {
            d=(b>>c)&0xf;
            lsprintf(palette_buf[a],"%ld",d);
            if (prop) FixSliderPot(Window,&palettegads[a],d,1<<bpg,1,1);
            RefreshStrGad(&palettegads[a+3],Window);
        }
    }*/
}

void updatecolourvals(pen,show)
int pen,show;
{
    ULONG new[3],old[3];
    int a,/*b,*/ok=0;

    if (show==-1) {
        for (a=0;a<3;a++) {
            new[a]=atoi(palette_buf[a]);
            if (new[a]<0) new[a]=0;
            else if (new[a]>=(1<<bpg)) new[a]=(1<<bpg)-1;
            /*if (version2>=OSVER_39)*/ scalecolup(&new[a]);
        }
        ok=1;   
    }
    else {
        /*if (version2>=OSVER_39)*/ {
            GetRGB32(vp->ColorMap,pen,1,old);
            for (a=0;a<3;a++) {
                scalecol(&old[a]);
                if ((new[a]=GetSliderPos(&palettegads[a],1<<bpg,1))!=old[a]) ok=1;
                scalecolup(&new[a]);
            }
        }
/*        else {
            for (a=0,b=8;a<3;a++,b-=4)
                old[a]=(GetRGB4(vp->ColorMap,pen)>>b)&0xf;
            for (a=0;a<3;a++)
                if ((new[a]=GetSliderPos(&palettegads[a],1<<bpg,1))!=old[a]) ok=1;
         }*/
    }
    if (ok) {
        /*if (version2>=OSVER_39)*/ SetRGB32(vp,pen,new[0],new[1],new[2]);
//        else SetRGB4(vp,pen,new[0],new[1],new[2]);
    }
    showcolourvals(pen,show);
    get_palette(Screen,config->new_palette,1<<((screen_depth > 4) ? 4 : screen_depth));
}

void showscreencolours(which,fg,bg,special)
int which;
char **fg,**bg;
int special;
{
    int a,x,y,h;
    char buf[2];

    if (which&COLOURS_3DBOXES) {
        do3dbox(x_off+151,y_off+88,203,41);
        do3dbox(x_off+394,y_off+88,203,41);
        Do3DStringBox(rp,
            x_off+153,y_off+143,
            163,8,
            screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
        Do3DStringBox(rp,
            x_off+432,y_off+143,
            163,8,
            screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
        solidblock(rp,
            coloursgadgets[COLBIT_3DBOXES],
            x_off+162,y_off+168,
            208,10,1,0,3+special);
        if (fg) {
            *fg=&config->gadgettopcol; *bg=&config->gadgetbotcol;
        }
    }       
    if (which&COLOURS_STATUS) {
        solidblock(rp,
            coloursgadgets[COLBIT_STATUS],
            x_off+151,y_off+68,
            446,8,config->statusfg,config->statusbg,3+special);
        if (fg) {
            *fg=&config->statusfg; *bg=&config->statusbg;
        }
    }
    if (which&COLOURS_SELDISKNAME) {
        solidblock(rp,
            coloursgadgets[COLBIT_SELDISKNAME],
            x_off+151,y_off+78,
            221,8,config->disknameselfg,config->disknameselbg,1+special);
        if (fg) {
            *fg=&config->disknameselfg; *bg=&config->disknameselbg;
        }
    }
    if (which&COLOURS_DISKNAME) {
        solidblock(rp,
            coloursgadgets[COLBIT_DISKNAME],
            x_off+376,y_off+78,
            221,8,config->disknamefg,config->disknamebg,1+special);
        if (fg) {
            *fg=&config->disknamefg; *bg=&config->disknamebg;
        }
    }
    if (!special) {
        if (which&COLOURS_SELDIRECTORY) {
            solidblock(rp,
                coloursgadgets[COLBIT_SELDIRECTORY],
                x_off+151,y_off+88,
                203,8,config->dirsselfg,config->dirsselbg,0);
            if (fg) {
                *fg=&config->dirsselfg; *bg=&config->dirsselbg;
            }
        }
        if (which&COLOURS_DIRECTORY) {
            solidblock(rp,
                coloursgadgets[COLBIT_DIRECTORY],
                x_off+151,y_off+96,
                203,8,config->dirsfg,config->dirsbg,0);
            if (fg) {
                *fg=&config->dirsfg; *bg=&config->dirsbg;
            }
        }
        if (which&COLOURS_SELFILE) {
            solidblock(rp,
                coloursgadgets[COLBIT_SELFILE],
                x_off+151,y_off+104,
                203,8,config->filesselfg,config->filesselbg,0);
            if (fg) {
                *fg=&config->filesselfg; *bg=&config->filesselbg;
            }
        }
        if (which&COLOURS_FILE) {
            solidblock(rp,
                coloursgadgets[COLBIT_FILE],
                x_off+151,y_off+112,
                203,8,config->filesfg,config->filesbg,0);
            solidblock(rp,
                NULL,
                x_off+151,y_off+120,
                203,9,config->filesfg,config->filesbg,0);
            solidblock(rp,
                NULL,
                x_off+394,y_off+88,
                203,41,config->filesfg,config->filesbg,0);
            if (fg) {
                *fg=&config->filesfg; *bg=&config->filesbg;
            }
        }
    }
    if (which&COLOURS_SLIDERS) {
        do3dbox(x_off+151,y_off+131,167,9);
        do3dbox(x_off+430,y_off+131,167,9);
        do3dbox(x_off+358,y_off+88,14,23);
        do3dbox(x_off+376,y_off+88,14,23);
        if (!special) {
            SetAPen(rp,screen_pens[config->sliderbgcol].pen);
            RectFill(rp,
                x_off+151,y_off+131,
                x_off+317,y_off+139);
            RectFill(rp,
                x_off+430,y_off+131,
                x_off+596,y_off+139);

            RectFill(rp,
                x_off+358,y_off+88,
                x_off+371,y_off+110);
            RectFill(rp,
                x_off+376,y_off+88,
                x_off+389,y_off+110);

            SetAPen(rp,screen_pens[config->slidercol].pen);
            RectFill(rp,
                x_off+153,y_off+132,
                x_off+259,y_off+138);
            RectFill(rp,
                x_off+432,y_off+132,
                x_off+540,y_off+138);

            RectFill(rp,
                x_off+360,y_off+89,
                x_off+369,y_off+102);
            RectFill(rp,
                x_off+378,y_off+89,
                x_off+387,y_off+102);
        }
        if (fg) {
            *fg=&config->slidercol; *bg=&config->sliderbgcol;
        }
    }
    if (which&COLOURS_ARROWS) {
        x=x_off+322; y=y_off+131;
        h=9;
        for (a=0;a<4;a++) {
            do3dbox(x,y,14,h);
            do3dbox(x+18,y,14,h);
            DoArrow(rp,x,y,14,h,
                screen_pens[config->arrowfg].pen,screen_pens[config->arrowbg].pen,3);
            DoArrow(rp,x+18,y,14,h,
                screen_pens[config->arrowfg].pen,screen_pens[config->arrowbg].pen,2);

            if (a==1) {
                x=x_off+322;
                y+=h+2;
                ++h;
            }
            else x+=72;
        }

        for (a=0;a<2;a++) {
            do3dbox(x_off+358,y_off+113+(a*9),14,7);
            do3dbox(x_off+376,y_off+113+(a*9),14,7);
            if (!special) {
                DoArrow(rp,
                    x_off+358,y_off+113+(a*9),
                    14,7,
                    screen_pens[config->arrowfg].pen,screen_pens[config->arrowbg].pen,a);
                DoArrow(rp,
                    x_off+376,y_off+113+(a*9),
                    14,7,
                    screen_pens[config->arrowfg].pen,screen_pens[config->arrowbg].pen,a);
            }
        }
        if (fg) {
            *fg=&config->arrowfg; *bg=&config->arrowbg;
        }
    }
    if (which&COLOURS_TINYGADS) {
        buf[1]=0;
        buf[0]=cfg_string[STR_COLOURS_TINY_GADS][0];
        solidblock(rp,
            buf,
            x_off+358,y_off+131,
            14,9,config->littlegadfg,config->littlegadbg,3+special);
        buf[0]=cfg_string[STR_COLOURS_TINY_GADS][1];
        solidblock(rp,
            buf,
            x_off+376,y_off+131,
            14,9,config->littlegadfg,config->littlegadbg,3+special);
        buf[0]=cfg_string[STR_COLOURS_TINY_GADS][2];
        solidblock(rp,
            buf,
            x_off+358,y_off+142,
            14,10,config->littlegadfg,config->littlegadbg,3+special);
        buf[0]=cfg_string[STR_COLOURS_TINY_GADS][3];
        solidblock(rp,
            buf,
            x_off+376,y_off+142,
            14,10,config->littlegadfg,config->littlegadbg,3+special);

        for (a=0;a<6;a++) {
            buf[0]=cfg_string[STR_COLOURS_TINY_GADS][4+a];
            solidblock(rp,
                buf,
                x_off+493+(a*18),y_off+154,
                14,8,config->littlegadfg,config->littlegadbg,3+special);
        }
        if (fg) {
            *fg=&config->littlegadfg; *bg=&config->littlegadbg;
        }
    }
    if (which&COLOURS_CLOCK) {
        solidblock(rp,
            coloursgadgets[COLBIT_CLOCK],
            x_off+151,y_off+154,
            338,8,config->clockfg,config->clockbg,3+special);
        if (fg) {
            *fg=&config->clockfg; *bg=&config->clockbg;
        }
    }
    if (which&COLOURS_REQUESTER) {
        solidblock(rp,
            coloursgadgets[COLBIT_REQUESTER],
            x_off+378,y_off+168,
            208,10,config->requestfg,config->requestbg,3+special);
        if (fg) {
            *fg=&config->requestfg; *bg=&config->requestbg;
        }
    }
    if (which&COLOURS_STRINGS) {
        solidblock(rp,
            cfg_string[STR_COLOR_PATHNAME],
            x_off+153,y_off+143,
            163,8,/*version2?*/config->stringfgcol/*:1*/,/*version2?*/config->stringbgcol/*:0*/,0);
        if (fg) {
            *fg=&config->stringfgcol; *bg=&config->stringbgcol;
        }
    }
    if (which&COLOURS_SELSTRINGS) {
        solidblock(rp,
            cfg_string[STR_COLOR_SELPATHNAME],
            x_off+432,y_off+143,
            163,8,/*version2?*/config->stringselfgcol/*:1*/,/*version2?*/config->stringselbgcol/*:0*/,0);
        if (fg) {
            *fg=&config->stringselfgcol; *bg=&config->stringselbgcol;
        }
    }
    SetAPen(rp,screen_pens[1].pen);
    SetBPen(rp,screen_pens[0].pen);
}

void solidblock(r,txt,x,y,w,h,fg,bg,bord)
struct RastPort *r;
char *txt;
int x,y,w,h,fg,bg,bord;
{
    if (bord&1)
        Do3DBox(r,x,y,w,h,screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    if (!(bord&4)) {
        SetAPen(r,screen_pens[bg].pen);
        RectFill(r,x,y,x+w-1,y+h-1);
        if (txt) {
            SetAPen(r,screen_pens[fg].pen);
            SetBPen(r,screen_pens[bg].pen);
            UScoreText(r,txt,(bord&2)?x+((w-(strlen(txt)*8))/2):x,y+((h-8)/2)+6,-1);
            SetBPen(rp,screen_pens[0].pen);
        }
    }
}

void showcolourscycle(fg,bg,sel)
int fg,bg,sel;
{
    SetAPen(rp,screen_pens[1].pen);
    DoCycleGadget(&coloursgad,Window,coloursgadgets,sel);
    if (sel==COLBIT_3DBOXES) showfuncob(rp,cfg_string[STR_EXAMPLE],1,0,CFG_GADGET,x_off+302,y_off+23);
    else showfuncob(rp,cfg_string[STR_EXAMPLE],fg,bg,CFG_GADGET,x_off+302,y_off+23);
}
