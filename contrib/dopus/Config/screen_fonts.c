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

#define ABS(x) ((x)<0?-(x):(x))

int initfontlist(num,move,flags)
int num,move,flags;
{
    int a,b,dist=0,one,val,size=-1;
    char name[80],*ptr;

    if (!fontlist) return(0);
    if (!flags) fontlistview.items=fontlist;
    else if (flags==FFLAG_8ONLY) fontlistview.items=only8list;
    else if (flags==FFLAG_NOPROP) fontlistview.items=noproplist;
    strcpy(name,config->fontbufs[num]);
    if (ptr=strstri(name,".font")) *ptr=0;
    for (a=0;;a++) {
        if (!fontlistview.items[a]) break;
        if (strcmp(name,fontlistview.items[a])==0) {
            fontlistview.itemselected=a;
            if (move) {
                if (a<fontlistview.topitem || a>fontlistview.topitem+7)
                    fontlistview.topitem=a;
            }
            if (fontsizelist) {
                if (!flags) fontsizelistview.items=fontsizelist[a];
                else if (flags==FFLAG_8ONLY) fontsizelistview.items=only8sizelist;
                else if (flags==FFLAG_NOPROP) fontsizelistview.items=nopropsizelist[a];
                if (fontsizelistview.items) {
                    one=-1;
                    for (b=0;;b++) {
                        if (!fontsizelistview.items[b]) break;
                        if ((val=atoi(fontsizelistview.items[b]))==(int)config->fontsizes[num]) {
                            fontsizelistview.itemselected=b;
                            if (move) {
                                if (b<fontsizelistview.topitem ||
                                    b>fontsizelistview.topitem+fontsizelistview.lines)
                                    fontsizelistview.topitem=b;
                            }
                            one=-1;
                            break;
                        }
                        else if (one==-1 || ABS((val-(int)config->fontsizes[num]))<dist) {
                            dist=ABS((val-(int)config->fontsizes[num]));
                            one=b;
                        }
                    }
                    if (one!=-1) {
                        /*if (version2)*/ {
                            size=config->fontsizes[num];
                            lsprintf(fontsize_buf,"%ld",(long int)size);
                            RefreshStrGad(&fontsizegadget,Window);
                            fontsizelistview.itemselected=-1;
                        }
/*                        else {
                            size=atoi(fontsizelistview.items[one]);
                            fontsizelistview.itemselected=one;
                            if (move) {
                                if (one<fontsizelistview.topitem ||
                                    one>fontsizelistview.topitem+fontsizelistview.lines-1)
                                    fontsizelistview.topitem=one;
                            }
                        }*/
                    }
                    else size=(int)config->fontsizes[num];
                }
            }
            break;
        }
    }
    RefreshListView(&fontlistview,2);
    if (size>-1) {
        strcat(name,".font");
        dofontdemo(name,size);
    }
    return(size);
}

void sortfontlist(avail,num,type)
struct AvailFonts *avail;
int num,type;
{
    int gap,i,j,k;

    for (gap=num/2;gap>0;gap/=2)
        for (i=gap;i<num;i++)
            for (j=i-gap;j>=0;j-=gap) {
                k=j+gap;
                if (type==0) {
                    if (LStrCmpI((char *)avail[j].af_Attr.ta_Name,
                        (char *)avail[k].af_Attr.ta_Name)<=0) break;
                }
                else {
                    if (LStrCmpI((char *)avail[j].af_Attr.ta_Name,
                        (char *)avail[k].af_Attr.ta_Name)) break;
                    if (avail[j].af_Attr.ta_YSize<=avail[k].af_Attr.ta_YSize) break;
                }
                SwapMem((char *)&avail[j],(char *)&avail[k],sizeof(struct AvailFonts));
            }
}

void dofontdemo(name,size)
char *name;
int size;
{
    struct TextFont *font;
    struct Region *reg,*oldreg;
    struct Rectangle clip;
    char buf[2][256];
    int y,c,l,a;

    clip.MinX=x_off+146;
    clip.MinY=y_off+93;
    clip.MaxX=x_off+611;
    clip.MaxY=y_off+167;
    Do3DBox(rp,
        x_off+146,y_off+93,
        468,76,
        screen_pens[config->gadgetbotcol].pen,screen_pens[config->gadgettopcol].pen);
    SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,x_off+146,y_off+93,x_off+613,y_off+167);
    SetAPen(rp,screen_pens[1].pen);
    busy();
    UScoreText(rp,
        cfg_string[STR_LOADING_FONT],
        x_off+329,y_off+133,
        -1);
    if (font=getfont(name,&size,0)) {
        SetAPen(rp,screen_pens[0].pen);
        RectFill(rp,
            x_off+146,y_off+94,
            x_off+613,y_off+167);
        SetAPen(rp,screen_pens[1].pen);
        SetFont(rp,font);
        if (reg=NewRegion()) {
            OrRectRegion(reg,&clip);
            oldreg=InstallClipRegion(Window->WLayer,reg);
            l=255;
        }
        else l=464/font->tf_XSize;
        y=font->tf_Baseline;
        c='a';
        for (a=0;a<256;a++) {
            buf[0][a]=c++;
            if (c=='{') c='0';
        }
        for (a=0;a<256;a++) {
            if (isupper(buf[0][a])) buf[1][a]=ToLower(buf[0][a]);
            else buf[1][a]=ToUpper(buf[0][a]);
        }
        a=0;
        while (y<76+font->tf_Baseline) {
            Move(rp,x_off+148,y+y_off+94);
            Text(rp,buf[a],l);
            a=a?0:1;
            y+=font->tf_YSize;
        }
        if (reg) {
            InstallClipRegion(Window->WLayer,oldreg);
            DisposeRegion(reg);
        }
        SetFont(rp,tfont);
        CloseFont(font);
    }
    else UScoreText(rp,
        cfg_string[STR_FONT_COULD_NOT_BE_LOADED],
        x_off+289,y_off+133,
        -1);
    unbusy();
}
