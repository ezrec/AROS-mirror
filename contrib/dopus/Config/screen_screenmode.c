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
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>

int initscreenmodes()
{
    int pal,count,minw,minh,defw,depth,needdef=0,overx,overy,oldcount=0;
    struct Screen scrbuf,*wbscreen;
    char buf[256],namebuf[180];
    struct DimensionInfo *dimension;
    DisplayInfoHandle *handle;
    struct List *pubscreenlist;
    struct PubScreenNode *pubscreen;
    ULONG modeid;
    struct Library *CyberGfxBase;

    dimension=(struct DimensionInfo *)buf;
    count=0; depth=8;

    if (/*version2 &&*/ (wbscreen=LockPubScreen("Workbench"))) {
        modeid=GetVPModeID(&(wbscreen->ViewPort));
        if ((handle=FindDisplayInfo(modeid))) {
            GetDisplayInfoData(handle,buf,256,DTAG_DIMS,0);
            depth=dimension->MaxDepth;
        }
        UnlockPubScreen(NULL,wbscreen);
    }
    GetWBScreen(&scrbuf);

    CyberGfxBase = OpenLibrary("cybergraphics.library",0);
    modeid=INVALID_ID;
    while ((modeid=NextDisplayInfo(modeid))!=INVALID_ID) {
        if (!ModeNotAvailable(modeid) &&
            (handle=FindDisplayInfo(modeid)) &&
            (GetDisplayInfoData(handle,namebuf,128,DTAG_NAME,0)) &&
            (GetDisplayInfoData(handle,buf,256,DTAG_DIMS,0)))
           {
            if (CyberGfxBase && IsCyberModeID(modeid))
             {
              minw = GetCyberIDAttr(CYBRIDATTR_WIDTH,modeid);
              dimension->MinRasterHeight = dimension->MaxRasterHeight = GetCyberIDAttr(CYBRIDATTR_HEIGHT,modeid);
              dimension->MaxDepth = GetCyberIDAttr(CYBRIDATTR_DEPTH,modeid);
              dimension->TxtOScan.MaxX = minw-1;
              dimension->TxtOScan.MinX = 0;
              dimension->TxtOScan.MaxY = dimension->MinRasterHeight-1;
              dimension->TxtOScan.MinY = 0;
             }
            else if (modeid & (HIRES_KEY | HIRESLACE_KEY)) minw=640;
            else {
                minw=dimension->MinRasterWidth;
                if (minw<160) minw*=10;
            }
            minh=dimension->MinRasterHeight; if (minh<200) minh=200;
            defw=(dimension->TxtOScan.MaxX-dimension->TxtOScan.MinX)+1;
            if (minw>=640 && defw>=640) {
                count+=addscreenmode(((struct NameInfo *)namebuf)->Name,
                    minw,minh,
                    dimension->MaxRasterWidth,dimension->MaxRasterHeight,
                    defw,(dimension->TxtOScan.MaxY-dimension->TxtOScan.MinY)+1,
                    dimension->MaxDepth,modeid);
            }
        }
    }
    if (count==0) needdef=1;
    else {
        sortscreenmodes(count,0);
        oldcount=count;
    }
    if ((pubscreenlist=LockPubScreenList())) {
        pubscreen=(struct PubScreenNode *)pubscreenlist->lh_Head;
        while (pubscreen->psn_Node.ln_Succ) {
            if (strcmp(pubscreen->psn_Node.ln_Name,"Workbench")!=0 &&
                strncmp(pubscreen->psn_Node.ln_Name,"DOPUS.",6)!=0 &&
                pubscreen->psn_Screen->Width>=640 &&
                pubscreen->psn_Screen->Height>=200 &&
                pubscreen->psn_Screen->RastPort.BitMap->Depth>1) {

                lsprintf(namebuf,"%s:%s",pubscreen->psn_Node.ln_Name,cfg_string[STR_SCREEN_MODE_USE]);
                count+=addscreenmode(namebuf,
                    640,200,
                    pubscreen->psn_Screen->Width,pubscreen->psn_Screen->Height,
                    pubscreen->psn_Screen->Width,pubscreen->psn_Screen->Height,
                    pubscreen->psn_Screen->RastPort.BitMap->Depth,MODE_PUBLICSCREENUSE);
/*
                modeid=GetVPModeID(&pubscreen->psn_Screen->ViewPort);
                if ((handle=FindDisplayInfo(modeid))) {
                    GetDisplayInfoData(handle,buf,256,DTAG_DIMS,0);
                    a=dimension->MaxDepth;
                }
                else a=4;

                lsprintf(namebuf,"%s:%s",pubscreen->psn_Node.ln_Name,cfg_string[STR_SCREEN_MODE_CLONE]);
                count+=addscreenmode(namebuf,
                    pubscreen->psn_Screen->Width,pubscreen->psn_Screen->Height,
                    0,0,
                    0,0,
                    a,MODE_PUBLICSCREENCLONE);
*/
            }
            pubscreen=(struct PubScreenNode *)pubscreen->psn_Node.ln_Succ;
        }
        UnlockPubScreenList();
    }
    CloseLibrary(CyberGfxBase);

    if (needdef) {
        pal=GfxBase->DisplayFlags&PAL;
        overx=704-GfxBase->NormalDisplayColumns;
        if (overx<0) overx=0;
        overy=((pal)?290:240)-GfxBase->NormalDisplayRows;
        if (overy<0) overy=0;

        count+=addscreenmode(pal?"PAL:High Res":"NTSC:High Res",
            640,200,
            GfxBase->NormalDisplayColumns+overx,GfxBase->NormalDisplayRows+overy,
            GfxBase->NormalDisplayColumns,GfxBase->NormalDisplayRows,
            4,HIRES_KEY);
        count+=addscreenmode(pal?"PAL:High Res Laced":"NTSC:High Res Laced",
            640,200,
            GfxBase->NormalDisplayColumns+overx,(GfxBase->NormalDisplayRows+overy)*2,
            GfxBase->NormalDisplayColumns,GfxBase->NormalDisplayRows*2,
            4,HIRESLACE_KEY);
    }

    count+=addscreenmode(cfg_string[STR_MODE_WORKBENCH_USE],
        640,200,
        scrbuf.Width,scrbuf.Height,
        scrbuf.Width,scrbuf.Height,
        scrbuf.RastPort.BitMap->Depth,MODE_WORKBENCHUSE);
    count+=addscreenmode(cfg_string[STR_MODE_WORKBENCH_CLONE],
        scrbuf.Width,scrbuf.Height,
        0,0,
        0,0,
        depth,MODE_WORKBENCHCLONE);

    if (count>oldcount) sortscreenmodes(count-oldcount,oldcount);
    return(count);
}

int addscreenmode(name,minw,minh,maxw,maxh,defw,defh,depth,mode)
char *name;
UWORD minw,minh,maxw,maxh,defw,defh,depth;
ULONG mode;
{
    struct ScreenMode *scrmode;

    screenmode=firstmode;
    while (screenmode) {
//D(bug("addscreenmode: %s\t%s\n",screenmode->name?screenmode->name:"<NULL>",name?name:"<NULL>"));
        if (LStrCmpI(screenmode->name,name)==0) return(0);
        if (!screenmode->next) break;
        screenmode=screenmode->next;
    }

    if ((scrmode=LAllocRemember(&screenkey,sizeof(struct ScreenMode),MEMF_CLEAR))) {
        if (screenmode) screenmode->next=scrmode;
        else firstmode=scrmode;
        strcpy(scrmode->name,name);
        scrmode->minw=minw; scrmode->minh=minh;
        if (maxw==0) scrmode->maxw=minw; else scrmode->maxw=maxw;
        if (maxh==0) scrmode->maxh=minh; else scrmode->maxh=maxh;
        if (defw==0) scrmode->defw=minw; else scrmode->defw=defw;
        if (defh==0) scrmode->defh=minh; else scrmode->defh=defh;
        scrmode->maxdepth=(depth>8)?8:depth;
D(bug("%s: %ld\n",name,depth);)
        scrmode->mode=mode;
        return(1);
    }
    return(0);
}

struct ScreenMode *showdisplaydesc()
{
    struct ScreenMode *mode;
    char buf[80];

    SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,x_off+238,y_off+140,x_off+509,y_off+179);
    SetAPen(rp,screen_pens[1].pen);
    if (!(mode=getscreenmode(screenmodeview.itemselected))) return(NULL);
    lsprintf(buf,"%-16s: %ld %s %ld",cfg_string[STR_MINIMUM_SIZE],(long int)mode->minw,cfg_string[STR_BY],(long int)mode->minh);
    UScoreText(rp,buf,x_off+240,y_off+147,-1);
    lsprintf(buf,"%-16s: %ld %s %ld",cfg_string[STR_MAXIMUM_SIZE],(long int)mode->maxw,cfg_string[STR_BY],(long int)mode->maxh);
    UScoreText(rp,buf,x_off+240,y_off+155,-1);
    lsprintf(buf,"%-16s: %ld %s %ld",cfg_string[STR_DEFAULT_SIZE],(long int)mode->defw,cfg_string[STR_BY],(long int)mode->defh);
    UScoreText(rp,buf,x_off+240,y_off+163,-1);
    lsprintf(buf,"%-16s: %ld",cfg_string[STR_MAXIMUM_COLORS],(long int)(1<<mode->maxdepth));
    UScoreText(rp,buf,x_off+240,y_off+171,-1);
    return(mode);
}

void fixmodegads(mode)
struct ScreenMode *mode;
{
    lsprintf(screenwidth_buf,"%ld",(long int)config->scrw);
    lsprintf(screenheight_buf,"%ld",(long int)config->scrh);
    if (config->scrdepth<2) config->scrdepth+=2;
    if (config->scrdepth>((mode)?mode->maxdepth:8))
        config->scrdepth=((mode)?mode->maxdepth:8);
    lsprintf(screendepth_buf,"%ld",(long int)(1<<config->scrdepth));
    if (mode && !(screenmodegads[SCREENMODE_WIDTH-300].Flags&GADGDISABLED)) {
        if (!(CheckNumGad(&screenmodegads[SCREENMODE_WIDTH-300],Window,mode->minw,mode->maxw)))
            RefreshStrGad(&screenmodegads[SCREENMODE_WIDTH-300],Window);
    }
    if (mode && !(screenmodegads[SCREENMODE_HEIGHT-300].Flags&GADGDISABLED)) {
        if (!(CheckNumGad(&screenmodegads[SCREENMODE_HEIGHT-300],Window,mode->minh,mode->maxh)))
            RefreshStrGad(&screenmodegads[SCREENMODE_HEIGHT-300],Window);
    }
    EnableGadget(&screenmodegads[SCREENMODE_DEPTH-300],rp,0,0);
    RefreshStrGad(&screenmodegads[SCREENMODE_DEPTH-300],Window);
    if (mode) {
        if (mode->maxdepth==2 /*||
            (version2<OSVER_39 && 
                (mode->mode==MODE_WORKBENCHUSE || mode->mode==MODE_PUBLICSCREENUSE))*/) {
            DisableGadget(&screenmodegads[SCREENMODE_DEPTH-300],rp,0,0);
            FixSliderBody(Window,&screenmodegads[SCREENMODE_SLIDER-300],1,1,0);
        }
        else FixSliderBody(Window,&screenmodegads[SCREENMODE_SLIDER-300],mode->maxdepth-1,1,0);
    }
    FixSliderPot(Window,&screenmodegads[SCREENMODE_SLIDER-300],config->scrdepth-2,mode->maxdepth-1,1,2);
}

void fixdefaultgads(mode)
struct ScreenMode *mode;
{
    if (config->screenflags&SCRFLAGS_DEFWIDTH ||
        mode->mode==MODE_WORKBENCHCLONE ||
        mode->mode==MODE_PUBLICSCREENCLONE) {
        config->screenflags|=SCRFLAGS_DEFWIDTH;
        screenmodegads[SCREENMODE_DEFWIDTH-300].Flags|=SELECTED;
        DisableGadget(&screenmodegads[SCREENMODE_WIDTH-300],rp,0,0);
    }
    else {
        screenmodegads[SCREENMODE_DEFWIDTH-300].Flags&=~SELECTED;
        EnableGadget(&screenmodegads[SCREENMODE_WIDTH-300],rp,0,0);
    }
    if (config->screenflags&SCRFLAGS_DEFHEIGHT ||
        mode->mode==MODE_WORKBENCHCLONE ||
        mode->mode==MODE_PUBLICSCREENCLONE) {
        config->screenflags|=SCRFLAGS_DEFHEIGHT;
        screenmodegads[SCREENMODE_DEFHEIGHT-300].Flags|=SELECTED;
        DisableGadget(&screenmodegads[SCREENMODE_HEIGHT-300],rp,0,0);
    }
    else {
        screenmodegads[SCREENMODE_DEFHEIGHT-300].Flags&=~SELECTED;
        EnableGadget(&screenmodegads[SCREENMODE_HEIGHT-300],rp,0,0);
    }
    if (mode->maxdepth==2 /*||
        (version2<OSVER_39 && 
            (mode->mode==MODE_WORKBENCHUSE || mode->mode==MODE_PUBLICSCREENUSE))*/)
        DisableGadget(&screenmodegads[SCREENMODE_DEPTH-300],rp,0,0);
    else EnableGadget(&screenmodegads[SCREENMODE_DEPTH-300],rp,0,0);

    EnableGadget(&screenmodegads[SCREENMODE_DEFWIDTH-300],rp,2,1);
    EnableGadget(&screenmodegads[SCREENMODE_DEFHEIGHT-300],rp,2,1);
    RefreshGList(&screenmodegads[SCREENMODE_DEFWIDTH-300],Window,NULL,2);
    if (mode->mode==MODE_WORKBENCHCLONE || mode->mode==MODE_PUBLICSCREENCLONE) {
        DisableGadget(&screenmodegads[SCREENMODE_DEFWIDTH-300],rp,2,1);
        DisableGadget(&screenmodegads[SCREENMODE_DEFHEIGHT-300],rp,2,1);
    }
}

void sortscreenmodes(count,off)
int count,off;
{
    int gap,i,j;
    struct ScreenMode *mode1,*mode2;

    for (gap=count/2;gap>0;gap/=2)
        for (i=gap;i<count;i++)
            for (j=i-gap;j>=0;j-=gap) {
                mode1=getscreenmode(j+off);
                mode2=getscreenmode(j+gap+off);
                if (!mode1 || !mode2 || LStrCmpI(mode1->name,mode2->name)<=0) break;
                SwapMem((char *)mode1->name,(char *)mode2->name,sizeof(struct ScreenMode)-4);
            }
}

struct ScreenMode *getscreenmode(m)
int m;
{
    struct ScreenMode *mode;
    int a;

    mode=firstmode;
    for (a=0;a<m;a++) {
        if (!mode) break;
        mode=mode->next;
    }
    return(mode);
}

