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
#ifndef __AROS__    
#include <devices/newmouse.h>
#else
#include <devices/rawkeycodes.h>
#endif
//#include <proto/powerpacker.h>
#ifdef WITHPCHG
  #ifdef __PPC__
     #include <hardware/custom.h>
     struct Custom *custom = 0x00DFF000;
  #endif
  #include <clib/pchglib_protos.h>
#endif

enum {
    PRINT_ASPECT,
    PRINT_IMAGE,
    PRINT_SHADE,
    PRINT_PLACE,
    PRINT_FORMFD,
    PRINT_TITLE,
    PRINT_OKAY,
    PRINT_CANCEL,
    PRINT_ABORT};

#define DIMBUFSIZE sizeof(struct DimensionInfo)

struct AnimFrame {
    struct AnimFrame *next;
    struct AnimHeader *animheader;
    unsigned char *delta;
    unsigned char *cmap;
    int cmapsize;
};

struct DOpusAnim {
    struct AnimFrame initial_frame;
    struct AnimFrame *first_frame;
    struct AnimFrame *current_frame;
    struct AnimFrame *last_frame;
    struct BitMap *frame_bm[2],
                  *initialframe_bm;
    struct Image frameimage;
    int framecount,
        framenum,
        framedisp,
        speed,
        framespersec,
        type;
};

static struct NewWindow iffwin = {
        0,0,0,0,
        255,255,
        IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS|IDCMP_INACTIVEWINDOW,
        WFLG_RMBTRAP|WFLG_BORDERLESS|WFLG_SIMPLE_REFRESH|WFLG_NOCAREREFRESH,
        NULL,NULL,NULL,NULL,NULL,
        0,0,0,0,CUSTOMSCREEN};

static char *picbuffer,*picturename;
static unsigned char *bodyptr;
static ULONG buffersize,bufferpos;

static ULONG *colourtable_8;
static UWORD *copperlist,*colourtable_4;

static struct ViewPort *ivp;

static int numcolours,currange,copperheight,
           screenwidth,screenheight,
           bitmapwidth,bitmapheight,
           viewflags;
static int palette32,got_dpan;
static char doublebuffer;
static char specialformat;

static struct BitMapHeader bmhd;

static struct BitMap *iffbm[2];
static struct Window *iffwindow;

#ifdef WITHPCHG
static struct PCHGHeader *pchghead;
static char *sham;
#endif

#ifdef DO_DBUF
static struct DBufInfo *dbufinfo;
static struct MsgPort *dbufport;
#endif

static char iffscreenname[34];

static struct DOpusAnim anim;

static inline void dotitle(struct BitMapHeader *bmhd)
{
    char title[100],modes[140],cols[30];

    getcolstring(cols);
    getviewmodes(modes);
    lsprintf(title,"%ld x %ld x %ld (page %ld x %ld) %s cols (%s)",
        (long int)bmhd->bmh_Width,
        (long int)bmhd->bmh_Height,
        (long int)bmhd->bmh_Depth,
        (long int)bmhd->bmh_PageWidth,
        (long int)bmhd->bmh_PageHeight,
        cols,modes);
    dostatustext(title);
}

int LoadPic(name)
char *name;
{
    struct DOpusRemember *iffkey;
    int retcode,
        a,
        gotbody = 0,
        brush = 0,
        scroll = 1,
        mp = 1,
        isanim = 0,
        extflag = 0,
        colourptr = -1,
        coloursize,
        imagewidth, imageheight,
        minwidth, minheight, maxwidth, maxheight,
        depth,
        __unused coppersize,
        viewmode,
        maxframes;
    UWORD colourlist[4];
    ULONG chunkid,
          chunksize,
          chunkbuf[3];
    struct DimensionInfo *dimension;
    DisplayInfoHandle *handle;
    char dimbuf[DIMBUFSIZE],
         *ptr;
    DPAnimChunk dpan;
    struct AnimFrame *cur_frame = NULL,
                     *frame;
    CRange cyclerange[6];
	APTR dto = NULL;
	BOOL dt_ok = 0;

    picturename = name;
    viewmode = -1;
    anim.speed = 50000;
    anim.framespersec = 20;
    maxframes = 65535;
    copperlist = NULL;
    iffwindow = NULL;
    iffscreen = NULL;
    iffscreenname[0] = 0;
    copperheight = 0;
    currange = 0;
    doublebuffer = 0;
    bodyptr = NULL;
    bufferpos = 0;
    iffkey = NULL;
    anim.framecount = 0;
    specialformat = 0;
    anim.initialframe_bm = NULL;
    palette32 = 0;
    got_dpan = 0;
    anim.type = 0;
    anim.first_frame = NULL;
    anim.last_frame = NULL;
    anim.initial_frame.cmap = NULL;
#ifdef WITHPCHG
    pchghead = NULL;
    sham = NULL;
#endif
#ifdef DO_DBUF
    dbufinfo = NULL;
    dbufport = NULL;
#endif
    for (a = 0; a < 2; a++) iffbm[a] = anim.frame_bm[a] = NULL;

    if (readfile(name,&picbuffer,(int *)&buffersize)) return(0);

    if (! chunkread(chunkbuf,sizeof(ULONG)*3) ||
        chunkbuf[0] != ID_FORM ||
        ((chunkbuf[2] != ID_ANIM) && (chunkbuf[2] != ID_ILBM)))
     {
DTload:
      if (DataTypesBase)
       {
        if ((dto = NewDTObject(name,DTA_GroupID,GID_PICTURE,
  //                                 PDTA_Remap, FALSE,
                                   PDTA_FreeSourceBitMap, TRUE,
                                   TAG_END)))
         {
          struct BitMapHeader *dto_bmhd;

          if (GetDTAttrs (dto,
                      PDTA_ModeID,            &viewmode,
                      PDTA_BitMapHeader,      &dto_bmhd,
                      PDTA_ColorRegisters,    &colourptr,
                      PDTA_NumColors,         &coloursize,
                      TAG_DONE)==4)
           {
D(bug("viewmode(0) = %08lx\n",viewmode));
            bmhd = *dto_bmhd;
            coloursize*=3;
            picbuffer=NULL;

            dt_ok = TRUE;
  //D(KDump(colourptr,coloursize));
           }
         }
       }
      if (! dt_ok)
       {
        retcode=IFFERR_NOTILBM;
        goto endiff;
       }
     }
    if (! dt_ok)
     {
      if (chunkbuf[2] == ID_ANIM) isanim = 1;

      while (bufferpos<=buffersize) {
          if (!chunkread(&chunkid,sizeof(int)) ||
              !chunkread(&chunksize,sizeof(int)) ||
              ((bufferpos+chunksize)>buffersize)) break;
          switch (chunkid) {
              case ID_FORM:
                  bufferpos+=4;
                  continue;
              case ID_DPAN:
                  if (!isanim) break;
                  if (!(chunkread(&dpan,sizeof(DPAnimChunk)))) {
                      retcode=IFFERR_BADIFF;
                      goto endiff;
                  }
                  chunksize=0;
                  maxframes=dpan.nframes;
                  if ((anim.framespersec=dpan.framespersecond)>0)
                      anim.speed=1000000/dpan.framespersecond;
                  got_dpan=1;
                  break;
              case ID_ANHD:
                  if (isanim && anim.framecount<maxframes &&
                      (frame=LAllocRemember(&iffkey,sizeof(struct AnimFrame),MEMF_CLEAR))) {
                      frame->animheader=(struct AnimHeader *)(picbuffer+bufferpos);
                      if (frame->animheader->ah_Interleave!=1) doublebuffer=1;
                      if (!anim.type) anim.type=frame->animheader->ah_Operation;

                      if (cur_frame) cur_frame->next=frame;
                      else anim.first_frame=frame;
                      cur_frame=frame;
                  }
                  break;
              case ID_DLTA:
                  if (isanim && anim.framecount<maxframes && cur_frame) {
                      cur_frame->delta=picbuffer+bufferpos;
                      ++anim.framecount;
                  }
                  break;
              case ID_BMHD:
                  if (!chunkread(&bmhd,sizeof(struct BitMapHeader))) {
                      retcode=IFFERR_BADIFF;
                      goto endiff;
                  }
                  if ((! isanim) && (bmhd.bmh_Depth > 8)) goto DTload;
                  chunksize=0;
                  break;
              case ID_CMAP:
                  if (colourptr==-1) {
                      colourptr=bufferpos; coloursize=chunksize;
                      anim.initial_frame.cmap=(unsigned char *)&picbuffer[bufferpos];
                      anim.initial_frame.cmapsize=chunksize;
                  }
                  else if (isanim && anim.framecount<maxframes && cur_frame) {
                      cur_frame->cmap=(unsigned char *)&picbuffer[bufferpos];
                      cur_frame->cmapsize=chunksize;
                  }
                  break;
              case ID_GRAB:
                  brush=1;
                  break;
              case ID_CAMG:
                  if (!chunkread(&viewmode,sizeof(int))) {
                      retcode=IFFERR_BADIFF;
                      goto endiff;
                  }
                  chunksize=0;
D(bug("viewmode(0) = %08lx\n",viewmode));
                  break;
              case ID_CTBL:
                  if ((copperlist=LAllocRemember(&iffkey,chunksize,MEMF_CLEAR))) {
                      coppersize=chunksize;
                      copperheight=chunksize/32;
                      CopyMem((char *)&picbuffer[bufferpos],(char *)copperlist,chunksize);
                      CopyMem((char *)&picbuffer[bufferpos],(char *)colourlist,8);
                  }
                  break;
              case ID_DYCP:
                  mp=0;
                  break;
#ifdef WITHPCHG
              case ID_SHAM:
                  if (!(pchghead=PCHG_SHAM2PCHG((UWORD *)&picbuffer[bufferpos],chunksize,(viewmode&LACE)?2:1))) {
                      retcode=IFFERR_NOMEM;
                      goto endiff;
                  }
                  sham=&picbuffer[bufferpos];
                  specialformat=1;
                  break;
              case ID_PCHG:
                  pchghead=(struct PCHGHeader *)&picbuffer[bufferpos];
                  specialformat=1;
                  break;
#endif
              case ID_CRNG:
                  if (currange<6) {
                      CopyMem((char *)&picbuffer[bufferpos],(char *)&cyclerange[currange],sizeof(CRange));
                      if (cyclerange[currange].rate<=CRNG_NORATE ||
                          cyclerange[currange].low==cyclerange[currange].high)
                          cyclerange[currange].active=0;
                      if (cyclerange[currange].rate>CRNG_NORATE)
                          cyclerange[currange].rate=16384/cyclerange[currange].rate;
                      else cyclerange[currange].rate=0;
                      ++currange;
                  }
                  break;
              case ID_BODY:
                  if (!gotbody) {
                      gotbody=1;
                      bodyptr=picbuffer+bufferpos;
                  }
                  break;
          }
          if (chunksize) {
              bufferpos+=chunksize;
              if (chunksize&1) ++bufferpos;
          }
      }

      if (!gotbody) {
          retcode=IFFERR_BADIFF;
          goto endiff;
      }

      if (copperlist) specialformat=1;
     }

    depth = bmhd.bmh_Depth;

    if (colourptr>-1) {
        numcolours=coloursize/3;

        if (/*system_version2>=OSVER_39 &&*/ !specialformat)
            colourtable_8=LAllocRemember(&iffkey,(coloursize+2)*4,MEMF_CLEAR);

        colourtable_4=LAllocRemember(&iffkey,numcolours*sizeof(UWORD),MEMF_CLEAR);
    }

    if (!colourtable_8 && !colourtable_4) {
        retcode=IFFERR_BADIFF;
        goto endiff;
    }

    if ((config->viewbits&VIEWBITS_8BITSPERGUN || bmhd.bmh_Pad&BMHF_CMAPOK) &&
        colourtable_8) palette32=1;

    build_palettes(&picbuffer[colourptr],coloursize,colourtable_4,colourtable_8);

    imagewidth = bmhd.bmh_Width;
    screenwidth = bmhd.bmh_Width;
    imageheight = bmhd.bmh_Height;
    screenheight = bmhd.bmh_Height;

    minwidth = 320;
    minheight = 200;

    maxwidth = 368;
    maxheight = (scrdata_is_pal ? 281 : 241);

    if (viewmode==-1 || brush) {
        viewflags=viewmode; viewmode=0;
        if (bmhd.bmh_PageWidth>maxwidth) viewmode|=HIRES;
        if (bmhd.bmh_PageHeight>maxheight) viewmode|=LACE;
        viewflags=0;
    }

    if (!(ModeNotAvailable(viewmode))) extflag = 1;
D(bug("extflag,viewmode(1) = %ld,%08lx\n",extflag,viewmode));
    if (config->viewbits & VIEWBITS_BESTMODEID)
     {
      ULONG newmode;

      if (!(ModeNotAvailable((newmode=BestModeID(
//                    BIDTAG_NominalWidth,imagewidth,
//                    BIDTAG_NominalHeight,imageheight,
              BIDTAG_DesiredWidth,imagewidth,
              BIDTAG_DesiredHeight,imageheight,
              BIDTAG_Depth,depth,
              BIDTAG_SourceID,viewmode,
              BIDTAG_MonitorID,GetVPModeID(&Window->WScreen->ViewPort) & MONITOR_ID_MASK,
              TAG_END))))) viewmode=newmode;
D(bug("extflag,viewmode(2) = %ld,%08lx\n",extflag,viewmode));
     }

    if ((handle=FindDisplayInfo(viewmode))) {
        if (!(a=GetDisplayInfoData(handle,dimbuf,DIMBUFSIZE,DTAG_NAME,0))) {
            DisplayInfoHandle *temphandle;
            ULONG tempviewmode;

            tempviewmode=viewmode;
            tempviewmode&=~(HAM_KEY|EXTRAHALFBRITE_KEY);

            if ((temphandle=FindDisplayInfo(tempviewmode)))
                a=GetDisplayInfoData(temphandle,dimbuf,DIMBUFSIZE,DTAG_NAME,0);
        }
        if (a && (((struct NameInfo *)dimbuf)->Name)) {
            strcpy(iffscreenname,((struct NameInfo *)dimbuf)->Name);
            if ((ptr=strchr(iffscreenname,':'))) strcpy(iffscreenname,ptr+1);
        }
        GetDisplayInfoData(handle,dimbuf,DIMBUFSIZE,DTAG_DIMS,0);
        dimension=(struct DimensionInfo *)dimbuf;
        maxwidth=(dimension->StdOScan.MaxX-dimension->StdOScan.MinX)+1;
        maxheight=(dimension->StdOScan.MaxY-dimension->StdOScan.MinY)+1;
        if (depth>dimension->MaxDepth) {
            retcode=IFFERR_BADMODE;
            goto endiff;
        }
    }
    else extflag=0;

    viewflags=0;
    if ((viewmode&SUPER_KEY)==SUPER_KEY) viewflags|=SUPERHIRES|HIRES;
    else if (viewmode&HIRES_KEY) viewflags|=HIRES;
    if (viewmode&LORESLACE_KEY) viewflags|=LACE;
    if (viewmode&HAM_KEY) viewflags|=HAM;
    if (viewmode&EXTRAHALFBRITE_KEY) viewflags|=EXTRA_HALFBRITE;

    if (!extflag) {
        viewmode=viewflags;
D(bug("extflag,viewmode(3) = %ld,%08lx\n",extflag,viewmode));
        viewmode&=0x0000ffff;
        viewmode&=~(SPRITES|GENLOCK_AUDIO|GENLOCK_VIDEO|VP_HIDE|DUALPF|PFBA|EXTENDED_MODE);
        viewflags=viewmode;
D(bug("extflag,viewmode(4) = %ld,%08lx\n",extflag,viewmode));

        if (viewflags&HIRES) {
            maxwidth *= (viewflags&SUPERHIRES) ? 4 : 2;
            minwidth *= 2;
        }
    }

    if (screenwidth < minwidth) screenwidth = minwidth;
    else if (screenwidth > maxwidth) screenwidth = maxwidth;

    if (!extflag && viewflags & LACE) {
        maxheight *= 2;
        minheight *= 2;
    }
    if (screenheight < minheight) screenheight = minheight;
    else if (screenheight > maxheight) screenheight = maxheight;

    bitmapwidth = (imagewidth < screenwidth) ? screenwidth : imagewidth;
    bitmapheight = (imageheight < screenheight) ? screenheight : imageheight;

    if ((bitmapwidth > screenwidth) || (bitmapheight > screenheight)) scroll=3;

    dotitle(&bmhd);

    if (isanim && anim.first_frame) {
        if (!(anim.frame_bm[0]=AllocBitMap(bitmapwidth,bitmapheight,bmhd.bmh_Depth,BMF_DISPLAYABLE|BMF_CLEAR,NULL))) {
            retcode=IFFERR_NOMEM;
            goto endiff;
        }
        if (doublebuffer) {
            if ((anim.frame_bm[1]=AllocBitMap(bitmapwidth,bitmapheight,bmhd.bmh_Depth,BMF_DISPLAYABLE|BMF_CLEAR,NULL))) {
                anim.initialframe_bm=AllocBitMap(bitmapwidth,bitmapheight,bmhd.bmh_Depth,BMF_DISPLAYABLE|BMF_CLEAR,NULL);
#ifdef DO_DBUF
                if (/*system_version2>=OSVER_39 && */
                    (dbufinfo=AllocDBufInfo(ivp))) {
                    if (dbufport=LCreatePort(NULL,0)) {
                        dbufinfo->dbi_SafeMessage.mn_ReplyPort=dbufport;
                        dbufinfo->dbi_SafeMessage.mn_Length=sizeof(struct Message);
                    }
                }
#endif
            }
        }
    }

    if (doublebuffer) {
        for (a=0;a<2;a++) {
            if (anim.frame_bm[a]) {
                if (!(iffbm[a]=LAllocRemember(&iffkey,sizeof(struct BitMap),MEMF_CLEAR))) {
                    retcode=IFFERR_NOMEMORY;
                    goto endiff;
                }
                CopyMem((char *)anim.frame_bm[a],(char *)iffbm[a],sizeof(struct BitMap));
            }
        }
    }
    else
     {
      if (!dt_ok)
       {
        iffbm[0]=AllocBitMap(bitmapwidth,bitmapheight,depth,BMF_DISPLAYABLE|BMF_CLEAR,NULL);

        if (iffbm[0] == NULL)
         {
          retcode = IFFERR_NOMEMORY;
          goto endiff;
         }
       }
     }

D(bug("extflag,viewmode(5) = %ld,%lx\n",extflag,viewmode));
    if (!(iffscreen = OpenScreenTags(NULL,
                          SA_Width,      bitmapwidth,
                          SA_Height,     bitmapheight,
                          SA_Depth,      depth,
                          dt_ok?SA_Type:SA_BitMap,     dt_ok?CUSTOMSCREEN:(IPTR)iffbm[0],
                          SA_Behind,     TRUE,
                          SA_DisplayID,  viewmode,
                          SA_AutoScroll, TRUE,
                          SA_Overscan,   OSCAN_MAX,
                          SA_SharePens,  TRUE,
                          SA_ShowTitle,  FALSE,
                          SA_Colors32,   colourtable_8,
                 //            SA_ErrorCode,  (Tag)&err,
                          TAG_END)))
     {
      retcode = IFFERR_NOSCREEN;
      goto endiff;
     }

    ivp = &(iffscreen->ViewPort);
    if (!dt_ok) LoadRGB4(ivp,nullpalette,1<<(depth>8?8:depth));

    iffwin.Screen = iffscreen;
    iffwin.Width = iffscreen->Width;
    iffwin.Height = iffscreen->Height;

    if (!(iffwindow=OpenWindow(&iffwin))) {
        retcode=IFFERR_NOSCREEN;
        goto endiff;
    }

    if (dt_ok)
     {
      SetDTAttrs(dto,NULL,NULL,PDTA_Screen,iffscreen,TAG_END);
      DoDTMethod(dto, iffwindow, NULL, DTM_PROCLAYOUT, NULL, 1);
      GetDTAttrs(dto, PDTA_DestBitMap, &iffbm[0],TAG_END);
D(bug("PDTA_DestBitMap: %08lx\n",iffbm[0]));
      if (!(iffbm[0]))
       {
        GetDTAttrs(dto, PDTA_BitMap, &iffbm[0],TAG_END);
D(bug("PDTA_BitMap: %08lx\n",iffbm[0]));
       }
//     if (!(iffbm[0])) GetDTAttrs(dto, PDTA_ClassBitMap, (Tag)&iffbm[0],TAG_END);
//D(bug("PDTA_ClassBitMap: %08lx\n",iffbm[0]));
      if (!(iffbm[0]))
       {
        retcode=IFFERR_NOMEMORY;
        goto endiff;
       }
#ifndef __AROS__
D(KDump(iffbm[0],sizeof(struct BitMap)));
#endif
      BltBitMapRastPort(iffbm[0],0,0,iffwindow->RPort,0,0,bitmapwidth,bitmapheight,0xC0);
     }

    setnullpointer(iffwindow);

    if (isanim && anim.first_frame) {
        anim.framenum=1;
        anim.framedisp=0;
        anim.current_frame=anim.first_frame;

        rletobuffer(bodyptr,bmhd.bmh_Width,bmhd.bmh_Height,anim.frame_bm[0],bmhd.bmh_Masking,bmhd.bmh_Compression);

        if (doublebuffer) {
            if (anim.frame_bm[1]) {
                BltBitMap(anim.frame_bm[0],0,0,anim.frame_bm[1],0,0,bmhd.bmh_Width,bmhd.bmh_Height,0xc0,0xff,NULL);
                if (anim.initialframe_bm)
                    BltBitMap(anim.frame_bm[0],0,0,anim.initialframe_bm,0,0,bmhd.bmh_Width,bmhd.bmh_Height,0xc0,0xff,NULL);
                WaitBlit();
            }
        }
        else {
            anim.frameimage.LeftEdge=(bitmapwidth-bmhd.bmh_Width)/2;
            anim.frameimage.TopEdge=(bitmapheight-bmhd.bmh_Height)/2;
            doanimframe();
        }
    }
    else if (!dt_ok) readpic(&bmhd,bodyptr,iffbm[0]);

    if (numcolours>ivp->ColorMap->Count) numcolours=ivp->ColorMap->Count;

    if (!copperlist) {
        if (viewflags&HAM
#ifdef WITHPCHG
 || pchghead
#endif
) {
            if (palette32) LoadRGB32(ivp,colourtable_8);
            else LoadRGB4(ivp,colourtable_4,numcolours);
        }
    }
#ifdef WITHPCHG
    if (pchghead) {
        if (sham) PCHG_SetUserCopList(0,0,ivp,pchghead,&pchghead[1],sham);
        else PCHG_ParsePCHG(pchghead,ivp);
    }
#endif
    ActivateWindow(iffwindow);
    ScreenToFront(iffscreen);

    if (copperlist) {
        struct View *view = ViewAddress();

        if ((copperheight>>1)>scrdata_norm_height) view->DyOffset=14+(/*system_version2?*/8/*:0*/);
        if (mp) LoadRGB4(ivp,colourlist,4);
        if (!InitDHIRES(mp)) {
        }
    }
    else if ((depth<=8) && !specialformat && !(viewflags&HAM)) {
        if (palette32) FadeRGB32(iffscreen,&colourtable_8[1],numcolours,1,config->fadetime);
        else FadeRGB4(iffscreen,colourtable_4,numcolours,1,config->fadetime);
    }

    if (currange) initcycle(ivp,colourtable_4,numcolours,cyclerange,currange);

    retcode = (WaitForMouseClick(scroll,iffwindow) ? -1 : 1);

    if (checkcycling()) togglecycle();
    stopcycle();

    if (specialformat) {
        ScreenToBack(iffscreen);
        FreeVPortCopLists(ivp);
    }
    else if ((depth<=8) && a!=-3 && !(viewflags&HAM)) {
        if (palette32) FadeRGB32(iffscreen,&colourtable_8[1],numcolours,-1,config->fadetime);
        else FadeRGB4(iffscreen,colourtable_4,numcolours,-1,config->fadetime);
    }

endiff:
#ifdef DO_DBUF
    FreeDBufInfo(dbufinfo);
    if (dbufport) LDeletePort(dbufport);
#endif
    ScreenToFront(blankscreen ? blankscreen : Window->WScreen);

    if (iffwindow) {
        CloseWindow(iffwindow);
        iffwindow = NULL;
    }
    if (iffscreen) {
        CloseScreen(iffscreen);
        iffscreen = NULL;
    }
    if (dt_ok)
     {
      DisposeDTObject(dto);
      dto = NULL;
     }
    else
     {
      for (a = 0; a < 2; a++)
       {
        FreeBitMap(iffbm[a]);
        FreeBitMap(anim.frame_bm[a]);
       }
      FreeBitMap(anim.initialframe_bm);
     }

    ActivateWindow(Window);
#ifdef WITHPCHG
    if (pchghead && sham) {
        FreeMem(pchghead,sizeof(struct PCHGHeader)+((pchghead->LineCount+31)/32)*4);
    }
#endif
    LFreeRemember(&iffkey);
    FreeVec(picbuffer);

    anim.first_frame = NULL;
    colourtable_8 = NULL;
    colourtable_4 = NULL;

    return(retcode);
}

void rletobuffer(source,sourcewidth,sourceheight,dest,mask,comp)
unsigned char *source;
int sourcewidth,sourceheight;
struct BitMap *dest;
char mask,comp;
{
    struct RLEinfo picinfo;

    picinfo.sourceptr=source;
    picinfo.destplanes=dest->Planes;
    picinfo.imagebpr=((sourcewidth+15)/16)*2;
    picinfo.imageheight=sourceheight;
    picinfo.imagedepth=dest->Depth;
    picinfo.destbpr=dest->BytesPerRow;
    picinfo.destheight=dest->Rows;
    picinfo.destdepth=dest->Depth;
    picinfo.masking=mask;
    picinfo.compression=comp;
    picinfo.offset=0;

    decoderle(&picinfo);
}

void readpic(bmhd,source,bmap)
struct BitMapHeader *bmhd;
unsigned char *source;
struct BitMap *bmap;
{
    int rowbytes,bmrbytes,rows;
    int byteoff=0;
    struct RLEinfo picinfo;

    rowbytes=((bmhd->bmh_Width+15)/16)*2;
    bmrbytes=((GetBitMapAttr(bmap,BMA_WIDTH)+15)/16)*2;

    if ((rows=bmhd->bmh_Height)>bmap->Rows) rows=bmap->Rows;

    picinfo.sourceptr=source;
    picinfo.destplanes=bmap->Planes;
    picinfo.imagebpr=rowbytes;
    picinfo.imageheight=rows;
    picinfo.imagedepth=bmhd->bmh_Depth;
    picinfo.destbpr=bmrbytes;
    picinfo.destheight=bmap->Rows;
    picinfo.destdepth=bmap->Depth;
    picinfo.masking=bmhd->bmh_Masking;
    picinfo.compression=bmhd->bmh_Compression;
    picinfo.offset=byteoff;

    decoderle(&picinfo);
}

void decoderle(rleinfo)
struct RLEinfo *rleinfo;
{
    register char *source,*dest;
    register int copy,col;
    register char count;
    register int plane,row,bmoffset,planes;

    planes=rleinfo->imagedepth;
    if (rleinfo->masking==mskHasMask) ++planes;
    source=rleinfo->sourceptr;

    switch (rleinfo->compression) {
        case cmpByteRun1:
            bmoffset=rleinfo->offset;
            for (row=0;row<rleinfo->imageheight;row++) {
                for (plane=0;plane<planes;plane++) {
                    if (plane<rleinfo->destdepth && rleinfo->destplanes[plane])
                        dest=(char *)(rleinfo->destplanes[plane]+bmoffset);
                    else dest=NULL;

                    for (col=0;col<rleinfo->imagebpr;) {
                        if ((count=*source++)>=0) {
                            copy=count+1;
                            col+=copy;
                            if (dest) {
                                while (copy--) *dest++=*source++;
                            }
                            else source+=copy;
                        }
                        else if (count!=-128) {
                            copy=1-count;
                            col+=copy;
                            if (dest) {
                                while (copy--) *dest++=*source;
                            }
                            ++source;
                        }
                    }
                }
                bmoffset+=rleinfo->destbpr;
            }
            break;
        case cmpNone:
            bmoffset=rleinfo->offset;
            for (row=0;row<rleinfo->imageheight;row++) {
                for (plane=0;plane<planes;plane++) {
                    if (plane<rleinfo->destdepth)
                        CopyMem(source,(char *)rleinfo->destplanes[plane]+bmoffset,rleinfo->imagebpr);
                    source+=rleinfo->imagebpr;
                }
                bmoffset+=rleinfo->destbpr;
            }
            break;
    }
}

void doanimframe()
{
    struct AnimFrame *frame;

    if (anim.last_frame) {
        frame=anim.last_frame;
        anim.last_frame=anim.last_frame->next;
    }
    else {
        frame=&anim.initial_frame;
        anim.last_frame=anim.first_frame;
    }

    if (frame->cmap) {
        build_palettes(frame->cmap,frame->cmapsize,colourtable_4,colourtable_8);
        if (palette32) LoadRGB32(ivp,colourtable_8);
        else LoadRGB4(ivp,colourtable_4,numcolours);
    }

    if (doublebuffer && iffbm[1]) {
#ifdef DO_DBUF
        if (dbufport) {
            ChangeVPBitMap(ivp,iffbm[anim.framedisp],dbufinfo);
            WaitPort(dbufport);
            GetMsg(dbufport);
        }
        else {
            iffscreen->RastPort.BitMap=iffbm[anim.framedisp];
            iffscreen->ViewPort.RasInfo->BitMap=iffbm[anim.framedisp];
            MakeScreen(iffscreen);
            RethinkDisplay();
        }
#else
        iffscreen->RastPort.BitMap=iffbm[anim.framedisp];
        iffscreen->ViewPort.RasInfo->BitMap=iffbm[anim.framedisp];
        MakeScreen(iffscreen);
        RethinkDisplay();
#endif
    }
    else {
        BltBitMap(anim.frame_bm[anim.framedisp],0,0,
            iffwindow->RPort->BitMap,anim.frameimage.LeftEdge,anim.frameimage.TopEdge,
            bmhd.bmh_Width,bmhd.bmh_Height,0xc0,0xff,NULL);
        WaitBlit();
    }
    if (anim.frame_bm[1]) anim.framedisp=1-anim.framedisp;

    if (!anim.current_frame) {
        if (anim.frame_bm[1]) {
            if (anim.initialframe_bm) {
                BltBitMap(anim.initialframe_bm,0,0,
                    anim.frame_bm[anim.framedisp],0,0,
                    bmhd.bmh_Width,bmhd.bmh_Height,0xc0,0xff,NULL);
                WaitBlit();
            }
            else rletobuffer(bodyptr,bmhd.bmh_Width,bmhd.bmh_Height,
                anim.frame_bm[anim.framedisp],bmhd.bmh_Masking,bmhd.bmh_Compression);
        }
        if (got_dpan || anim.framenum==0) {
            anim.current_frame=anim.first_frame;
            anim.last_frame=anim.first_frame;
            anim.framenum=1;
        }
        else anim.framenum=0;
    }

    if (anim.framenum>0) {
        switch (anim.current_frame->animheader->ah_Operation) {
            case 0:
                if (anim.current_frame->delta)
                    rletobuffer(anim.current_frame->delta,bmhd.bmh_Width,bmhd.bmh_Height,
                        anim.frame_bm[anim.framedisp],bmhd.bmh_Masking,bmhd.bmh_Compression);
                break;
            case 5:
                doriff(anim.current_frame->delta,anim.frame_bm[anim.framedisp],
                    (anim.current_frame->animheader->ah_Flags&2 || !anim.frame_bm[1]),bmhd.bmh_Width,0);
                break;
            case 7:
                doriff7(anim.current_frame->delta,anim.frame_bm[anim.framedisp],
                    bmhd.bmh_Width,(anim.current_frame->animheader->ah_Flags&1)?2:1);
                break;
            case 8:
                doriff(anim.current_frame->delta,anim.frame_bm[anim.framedisp],
                    1,bmhd.bmh_Width,(anim.current_frame->animheader->ah_Flags&1)?2:1);
                break;
        }
        ++anim.framenum;
        anim.current_frame=anim.current_frame->next;
    }
}

void doriff(delta,image,xor,sourcewidth,size)
unsigned char *delta;
struct BitMap *image;
int xor,sourcewidth,size;
{
    int plane,*dptr;
    unsigned char *data;

    sourcewidth=((sourcewidth+15)/16)*2;

    if ((dptr=(int *)delta)) {
        for (plane=0;plane<image->Depth;plane++) {
            if (dptr[plane]) {
                data=(unsigned char *)delta+dptr[plane];
                if (size==0) {
                    if (xor) decode_riff_xor(data,image->Planes[plane],image->BytesPerRow,sourcewidth);
                    else decode_riff_set(data,image->Planes[plane],image->BytesPerRow,sourcewidth);
                }
                else if (size==1)
                    decode_riff_short((UWORD *)data,image->Planes[plane],image->BytesPerRow,sourcewidth);
                else decode_riff_long((ULONG *)data,image->Planes[plane],image->BytesPerRow,sourcewidth);
            }
        }
    }
}

void doriff7(delta,image,sourcewidth,size)
unsigned char *delta;
struct BitMap *image;
int sourcewidth,size;
{
    int plane,*dptr;
    unsigned char *data;
    unsigned char *opcode;

    sourcewidth=((sourcewidth+15)/16)*2;

    if ((dptr=(int *)delta)) {
        for (plane=0;plane<image->Depth;plane++) {
            if (dptr[plane] && dptr[plane+8]) {
                opcode=(unsigned char *)delta+dptr[plane];
                data=(unsigned char *)delta+dptr[plane+8];
                if (size==1)
                    decode_riff7_short((UWORD *)data,opcode,image->Planes[plane],image->BytesPerRow,sourcewidth);
                else decode_riff7_long((ULONG *)data,opcode,image->Planes[plane],image->BytesPerRow,sourcewidth);
            }
        }
    }
}

void decode_riff_xor(delta,image,rowbytes,sourcebytes)
unsigned char *delta;
char *image;
int rowbytes,sourcebytes;
{
    int column,opcount;
    unsigned char *data,ch;

    for (column=0;column<sourcebytes;column++) {
        data=image+column;
        opcount=*delta++;
        while (opcount--) {
            ch=*delta++;
            if (ch) {
                if (!(ch&128)) data+=rowbytes*ch;
                else {
                    ch&=127;
                    while (ch--) {
                        *data^=*delta++;
                        data+=rowbytes;
                    }
                }
            }
            else {
                ch=*delta++;
                while (ch--) {
                    *data^=*delta;
                    data+=rowbytes;
                }
                ++delta;
            }
        }
    }
}

void decode_riff_set(delta,image,rowbytes,sourcebytes)
unsigned char *delta;
char *image;
int rowbytes,sourcebytes;
{
    int column,opcount;
    unsigned char *data,ch;

    for (column=0;column<sourcebytes;column++) {
        data=image+column;
        opcount=*delta++;
        while (opcount--) {
            ch=*delta++;
            if (ch) {
                if (!(ch&128)) data+=rowbytes*ch;
                else {
                    ch&=127;
                    while (ch--) {
                        *data=*delta++;
                        data+=rowbytes;
                    }
                }
            }
            else {
                ch=*delta++;
                while (ch--) {
                    *data=*delta;
                    data+=rowbytes;
                }
                ++delta;
            }
        }
    }
}

void decode_riff_short(delta,image,rowbytes,sourcebytes)
UWORD *delta;
unsigned char *image;
int rowbytes,sourcebytes;
{
    int column,opcount;
    unsigned char *data;
    UWORD ch;

    sourcebytes/=2;

    for (column=0;column<sourcebytes;column++) {
        data=(unsigned char *)(image+(column*2));
        opcount=*delta++;
        while (opcount--) {
            ch=*delta++;
            if (ch) {
                if (!(ch&0x8000)) data+=rowbytes*ch;
                else {
                    ch&=0x7fff;
                    while (ch--) {
                        *(UWORD *)data=*delta++;
                        data+=rowbytes;
                    }
                }
            }
            else {
                ch=*delta++;
                while (ch--) {
                    *(UWORD *)data=*delta;
                    data+=rowbytes;
                }
                ++delta;
            }
        }
    }
}

void decode_riff_long(delta,image,rowbytes,sourcebytes)
ULONG *delta;
unsigned char *image;
int rowbytes,sourcebytes;
{
    int column,opcount;
    unsigned char *data;
    ULONG ch;

    sourcebytes/=4;

    for (column=0;column<sourcebytes;column++) {
        data=(unsigned char *)(image+(column*4));
        opcount=*delta++;
        while (opcount--) {
            ch=*delta++;
            if (ch) {
                if (!(ch&0x80000000)) data+=rowbytes*ch;
                else {
                    ch&=0x7fffffff;
                    while (ch--) {
                        *(ULONG *)data=*delta++;
                        data+=rowbytes;
                    }
                }
            }
            else {
                ch=*delta++;
                while (ch--) {
                    *(ULONG *)data=*delta;
                    data+=rowbytes;
                }
                ++delta;
            }
        }
    }
}

void decode_riff7_short(delta,opcode,image,rowbytes,sourcebytes)
UWORD *delta;
unsigned char *opcode;
unsigned char *image;
int rowbytes,sourcebytes;
{
    int column,opcount;
    unsigned char *data,ch;

    sourcebytes/=2;

    for (column=0;column<sourcebytes;column++) {
        data=(unsigned char *)(image+(column*2));
        opcount=*opcode++;
        while (opcount--) {
            ch=*opcode++;
            if (ch) {
                if (!(ch&0x80)) data+=rowbytes*ch;
                else {
                    ch&=0x7f;
                    while (ch--) {
                        *(UWORD *)data=*delta++;
                        data+=rowbytes;
                    }
                }
            }
            else {
                ch=*opcode++;
                while (ch--) {
                    *(UWORD *)data=*delta;
                    data+=rowbytes;
                }
                ++delta;
            }
        }
    }
}

void decode_riff7_long(delta,opcode,image,rowbytes,sourcebytes)
ULONG *delta;
unsigned char *opcode;
unsigned char *image;
int rowbytes,sourcebytes;
{
    int column,opcount;
    unsigned char *data,ch;

    sourcebytes/=4;

    for (column=0;column<sourcebytes;column++) {
        data=(unsigned char *)(image+(column*4));
        opcount=*opcode++;
        while (opcount--) {
            ch=*opcode++;
            if (ch) {
                if (!(ch&0x80)) data+=rowbytes*ch;
                else {
                    ch&=0x7f;
                    while (ch--) {
                        *(ULONG *)data=*delta++;
                        data+=rowbytes;
                    }
                }
            }
            else {
                ch=*opcode++;
                while (ch--) {
                    *(ULONG *)data=*delta;
                    data+=rowbytes;
                }
                ++delta;
            }
        }
    }
}

int WaitForMouseClick(tits,wind)
int tits;
struct Window *wind;
{
    struct IntuiMessage *msg;
    ULONG class;
    UWORD code,qual;
    int ticks,waitfor=0,pon=0,waitbits,retcode=1,gottimer=0,origspeed,startanim,delta;
    struct timerequest treq;

    while ((msg=(struct IntuiMessage *) GetMsg(wind->UserPort)))
        ReplyMsg((struct Message *)msg);
    if (!anim.first_frame && config->showdelay>0) {
        waitfor=config->showdelay*10; ticks=0;
    }

    origspeed=anim.speed;

    waitbits=1<<wind->UserPort->mp_SigBit;
    if (anim.first_frame) {
        if (!(OpenDevice(TIMERNAME,UNIT_VBLANK,&treq.tr_node,0))) {
            treq.tr_node.io_Message.mn_ReplyPort=general_port;
            treq.tr_node.io_Command=TR_ADDREQUEST;
            treq.tr_node.io_Flags=0;
            treq.tr_time.tv_secs=0;
            treq.tr_time.tv_micro=10;
            SendIO(&treq.tr_node);
            waitbits|=1<<general_port->mp_SigBit;
            gottimer=1;
        }
        if (!(startanim=(!(config->viewbits&VIEWBITS_PAUSEANIMS))) && doublebuffer)
            doanimframe();
    }

    FOREVER {
        if (anim.first_frame) {
            while (GetMsg(general_port));
            if (CheckIO(&treq.tr_node)) {
                WaitIO(&treq.tr_node);
                treq.tr_time.tv_secs=0;
                treq.tr_time.tv_micro=anim.speed;
                SendIO(&treq.tr_node);
                if (startanim) doanimframe();
            }
        }
        while ((msg=(struct IntuiMessage *) GetMsg(wind->UserPort))) {
            class=msg->Class; code=msg->Code; qual=msg->Qualifier;
            ReplyMsg((struct Message *) msg);
            switch (class) {
                case IDCMP_INTUITICKS:
                    if (waitfor && (++ticks)>=waitfor) retcode=0;
                    break;
                case IDCMP_RAWKEY:
                    if (code>=0x50 && code<=0x59) { // F1-F10
                        code-=0x50;
                        anim.speed=(code*10000)+10;
                        break;
                    }
                    switch (code) {
                        case 0x0b:                  // -
                            if ((anim.speed+=10000)>1000000) anim.speed=1000000;
                            break;
                        case 0x0c:                  // =
                            if ((anim.speed-=10000)<10) anim.speed=10;
                            break;
                        case 0x0d:                  // '\'
                            anim.speed=origspeed;
                            break;
                        case 0x21:                  // S
                            startanim=1-startanim;
                            break;
                        case 0x36:                  // N
                            if (anim.first_frame) {
                                startanim=0;
                                doanimframe();
                            }
                            break;
                        case CURSOR_LEFT:
                        case RAWKEY_NM_WHEEL_LEFT:
                        case 0x2d:                  // NUM-4
                        case CURSOR_RIGHT:
                        case RAWKEY_NM_WHEEL_RIGHT:
                        case 0x2f:                  // NUM-6
                            if (tits!=3) break;
                            if (qual&IEQUALIFIER_CONTROL) delta=bitmapwidth;
                            else if (qual&IEQUALIFIER_ANYSHIFT) delta=screenwidth-20;
                            else delta=2;
                            if (code==CURSOR_LEFT || code==0x2d || code== RAWKEY_NM_WHEEL_LEFT) delta=-delta;

                            MoveScreen(wind->WScreen,-delta,0);
                            break;
                        case CURSOR_UP:
                        case RAWKEY_NM_WHEEL_UP:
                        case 0x3e:                  // NUM-8
                        case CURSOR_DOWN:
                        case RAWKEY_NM_WHEEL_DOWN:
                        case 0x1e:                  // NUM-2
                            if (tits!=3) break;
                            if (qual&IEQUALIFIER_CONTROL) delta=bitmapheight;
                            else if (qual&IEQUALIFIER_ANYSHIFT) delta=screenheight-20;
                            else delta=2;
                            if (code==CURSOR_UP || code==RAWKEY_NM_WHEEL_UP || code==0x3e) {
                                delta=-delta;
                                if (wind->WScreen->TopEdge > delta) delta = wind->WScreen->TopEdge;
                            }
                            MoveScreen(wind->WScreen,0,-delta);
                            break;
                        case 0x44:                  // ENTER
                            retcode = ((tits == 2) ? -2 : 0);
                            break;
                        case 0x45:                  // ESC
                            retcode=-1;
                            break;
                        case 0x10:                  // Q
                            if (!(qual&IEQUALIFIER_REPEAT)) retcode=0;
                            break;
                        case 0x42:                  // TAB
                            if (tits&1 && currange) togglecycle();
                            break;
                        case 0x5f:                  // HELP
                        case 0x19:                  // P
                        case 0x40:                  // SPACE
                            if (!(qual&IEQUALIFIER_REPEAT)) {
                                ModifyIDCMP(wind,(wind->IDCMPFlags&~IDCMP_INACTIVEWINDOW)|IDCMP_ACTIVEWINDOW);
                                if (tits&1) gfxprint(wind,&iffscreen->RastPort,0,0,bitmapwidth,bitmapheight,1);
                                else gfxprint(wind,wind->RPort,0,0,wind->Width,wind->Height,0);
                            }
                            break;
                        case 0x39:                  // .
                            if (pon) setnullpointer(wind);
                            else ClearPointer(wind);
                            pon^=1;
                            break;
                        case 0x1a:                  // [
                            if (colourtable_4 && !specialformat)
                                LoadRGB4(ivp,colourtable_4,numcolours);
                            break;
                        case 0x1b:                  // ]
                            if (colourtable_8 && !specialformat)
                                LoadRGB32(ivp,colourtable_8);
                            break;
                    }
                    break;
                case IDCMP_ACTIVEWINDOW:
                    ModifyIDCMP(wind,(wind->IDCMPFlags&~IDCMP_ACTIVEWINDOW)|IDCMP_INACTIVEWINDOW);
                    break;
                case IDCMP_INACTIVEWINDOW:
                    if (IntuitionBase->ActiveScreen!=wind->WScreen) {
                        retcode=-3;
                        break;
                    }
                    code=SELECTDOWN;
                case IDCMP_MOUSEBUTTONS:
                    if (code==SELECTDOWN) retcode=0;
                    else if (code==MENUDOWN) retcode=-1;
                    break;
            }
            if (retcode!=1) break;
        }
        if (retcode!=1) break;
        Wait(waitbits);
    }
    if (anim.first_frame && gottimer) {
        if (!(CheckIO(&treq.tr_node)))
            AbortIO(&treq.tr_node);
        WaitIO(&treq.tr_node);
        CloseDevice(&treq.tr_node);
        while (GetMsg(general_port));
    }
    return(retcode);
}

int chunkread(buf,nbytes)
void *buf;
ULONG nbytes;
{
    if (bufferpos+nbytes>buffersize) return(0);
    CopyMem((char *)&picbuffer[bufferpos],(char *)buf,nbytes);
    bufferpos+=nbytes;
    return(1);
}

void getcolstring(str)
char *str;
{
    if (viewflags&HAM || specialformat) {
        if (bmhd.bmh_Depth==8) strcpy(str,"256K");
        else strcpy(str,"4096");
    }
    else lsprintf(str,"%ld",(long int)(1<<bmhd.bmh_Depth));
}

void gfxprint(wind,rast,x,y,w,h,iff)
struct Window *wind;
struct RastPort *rast;
int x,y,w,h,iff;
{
    static struct NewScreen printscreen={
            0,0,320,0,2,
            0,1,0,
            CUSTOMSCREEN|SCREENQUIET|SCREENBEHIND|NS_EXTENDED,
            &screen_attr,NULL,NULL,NULL};
    static struct NewWindow printwin={
            0,0,320,0,0,0,
            IDCMP_GADGETUP|IDCMP_VANILLAKEY,
            WFLG_RMBTRAP|WFLG_BORDERLESS,
            NULL,NULL,NULL,NULL,NULL,
            0,0,0,0,CUSTOMSCREEN};
    static struct Image
        printcheckimage={7,2,13,7,1,NULL,1,0,NULL},
        printnullcheckimage={7,2,13,7,1,NULL,0,0,NULL};
    static struct Gadget
        printgadgets[8]={
            {&printgadgets[1],142,104,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
            NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_ASPECT,NULL},
            {&printgadgets[2],142,120,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
            NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_IMAGE,NULL},
            {&printgadgets[3],142,136,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
            NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_SHADE,NULL},
            {&printgadgets[4],142,152,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
            NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_PLACE,NULL},
            {&printgadgets[5],18,168,26,11,GFLG_GADGIMAGE|GFLG_GADGHIMAGE|GFLG_SELECTED,GACT_RELVERIFY|GACT_TOGGLESELECT,GTYP_BOOLGADGET,
            (APTR)&printnullcheckimage,(APTR)&printcheckimage,NULL,GAD_CHECK,NULL,PRINT_FORMFD,NULL},
            {&printgadgets[6],142,168,26,11,GFLG_GADGIMAGE|GFLG_GADGHIMAGE,GACT_RELVERIFY|GACT_TOGGLESELECT,GTYP_BOOLGADGET,
            (APTR)&printnullcheckimage,(APTR)&printcheckimage,NULL,GAD_CHECK,NULL,PRINT_TITLE,NULL},
            {&printgadgets[7],20,186,100,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
            NULL,NULL,NULL,0,NULL,PRINT_OKAY,NULL},
            {NULL,196,186,100,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
            NULL,NULL,NULL,0,NULL,PRINT_CANCEL,NULL}},
        abortprintgad={
            NULL,60,0,200,80,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
            NULL,NULL,NULL,0,NULL,PRINT_ABORT,NULL};
    static char
        print_gads_sel[4]={0,0,0,0},
        print_gads_max[4]={1,1,2,1};
    static UWORD basepalette[4]={0xaaa,0x000,0xfff,0x679};
    static char firsttime;

    struct IntuiMessage *msg;
    struct Window *pwin;
    struct Screen *pscr;
    struct RastPort *prp;
    struct DOpusRemember *pkey=NULL;
    struct PrinterData *pd;
    struct Preferences *prefs;
    union printerIO *print_request; /* IO request for Print operation */
    APTR save;
    ULONG class;
    UWORD code,gadgetid,qual;
    int a,/*b,*/abort=0,goff,fnum;
    char buf[200],modes[140],*ptr,pactcode[6],cols[30],title[120];
    char
        *printgadtxt[9],
        *printabortgadtxt[2]={NULL,NULL},
        *print_aspect_txt[2],
        *print_image_txt[2],
        *print_shade_txt[3],
        *print_placement_txt[2],
        **print_gads_txt[4]={
            print_aspect_txt,
            print_image_txt,
            print_shade_txt,
            print_placement_txt};

    if (scrdata_is_pal) {
        printscreen.Height=printwin.Height=256;
        goff=28;
    }
    else {
        printscreen.Height=printwin.Height=200;
        goff=0;
    }

    for (a=0;a<7;a++) {
        if (a==5) printgadtxt[a]=globstring[STR_PRINTTITLE];
        else if (a==6) printgadtxt[a]=globstring[STR_PRINT];
        else printgadtxt[a]=globstring[STR_ASPECT+a];
        if (a<6) {
            pactcode[a] = getkeyshortcut(printgadtxt[a]);
/*            for (b=0;;b++) {
                if (!printgadtxt[a][b]) break;
                if (printgadtxt[a][b]=='_') {
                    pactcode[a]=ToLower(printgadtxt[a][b+1]);
                    break;
                }
            }*/
        }
    }

    printgadtxt[7]=str_cancelstring;
    printgadtxt[8]=NULL;
    printabortgadtxt[0]=globstring[STR_ABORT_PRINT];
    for (a=0;a<2;a++) {
        print_aspect_txt[a]=globstring[STR_PORTRAIT+a];
        print_image_txt[a]=globstring[STR_POSITIVE+a];
        print_placement_txt[a]=globstring[STR_CENTER+a];
    }
    for (a=0;a<3;a++) print_shade_txt[a]=globstring[STR_BLACK_WHITE+a];

    /*if (system_version2 >= OSVER_39)*/
     {
      class = BestModeID(BIDTAG_NominalWidth, printscreen.Width,
                         BIDTAG_NominalHeight, printscreen.Height,
                         BIDTAG_Depth, printscreen.Depth,
                         BIDTAG_MonitorID, GetVPModeID(&Window->WScreen->ViewPort) & MONITOR_ID_MASK,
                         TAG_END);
D(bug("Print screen ModeID: %lx\n",class));
      if (class == INVALID_ID) class = LORES_KEY;
     }
    if (!(pscr=OpenScreenTags(&printscreen,
                   SA_DisplayID, class,
                   SA_Pens,      (IPTR)scr_drawinfo,
                   TAG_END)))
       return;
    LoadRGB4(&(pscr->ViewPort),basepalette,4);
    printwin.Screen=pscr;
    if (!(pwin=OpenWindow(&printwin))) {
        CloseScreen(pscr);
        return;
    }
    prp=pwin->RPort;
    SetFont(prp,scr_font[FONT_GENERAL]);
    SetAPen(prp,1);
    Do3DFrame(prp,0,4+goff,320,88,globstring[STR_DESCRIPTION],2,1);
    Do3DFrame(prp,0,98+goff,320,86,globstring[STR_PRINT_CONTROL],2,1);

    if (iff) {
        ptr=BaseName(picturename);
        getcolstring(cols);
        lsprintf(buf,"%12s : %s",
            globstring[STR_FILE],
            ptr);
        iffinfotxt(prp,buf,7,19+goff);

        lsprintf(buf,"%12s : %ld x %ld",
            globstring[STR_IMAGE_SIZE],
            (long int)bmhd.bmh_Width,
            (long int)bmhd.bmh_Height);
        iffinfotxt(prp,buf,7,35+goff);

        if (anim.first_frame) {
            if (anim.framenum<2) fnum=1;
            else fnum=anim.framenum-1;
            lsprintf(buf,"%12s : %ld %s %ld @ %ld/sec",
                globstring[STR_NUM_FRAMES],
                (long int)fnum,
                globstring[STR_OF],
                (long int)(anim.framecount+(1-got_dpan)),
                (long int)anim.framespersec);
            iffinfotxt(prp,buf,7,43+goff);

            lsprintf(buf,"%12s : ANIM Op %ld",
                globstring[STR_ANIM_TYPE],
                (long int)anim.type);
            iffinfotxt(prp,buf,7,51+goff);

            lsprintf(title,"IFF ANIM : %s   %s %ld %s %ld   %ld x %ld x %s\n\n",
                ptr,
                globstring[STR_FRAME],
                (long int)fnum,
                globstring[STR_OF],
                (long int)(anim.framecount+1),
                (long int)bmhd.bmh_Width,
                (long int)bmhd.bmh_Height,
                cols);
        }
        else {
            lsprintf(buf,"%12s : %ld x %ld",
                globstring[STR_PAGE_SIZE],
                (long int)bmhd.bmh_PageWidth,
                (long int)bmhd.bmh_PageHeight);
            iffinfotxt(prp,buf,7,43+goff);

            lsprintf(buf,"%12s : %ld x %ld",
                globstring[STR_SCREEN_SIZE],
                (long int)iffscreen->Width,
                (long int)iffscreen->Height);
            iffinfotxt(prp,buf,7,51+goff);

            lsprintf(title,"IFF ILBM : %s   %ld x %ld x %s\n\n",
                ptr,
                (long int)bmhd.bmh_Width,
                (long int)bmhd.bmh_Height,
                cols);
        }
        lsprintf(buf,"%12s : %ld",
            globstring[STR_DEPTH],
            (long int)bmhd.bmh_Depth);
        iffinfotxt(prp,buf,7,67+goff);

        lsprintf(buf,"%12s : %s",
            globstring[STR_COLOURS],
            cols);
        iffinfotxt(prp,buf,7,75+goff);

        getviewmodes(modes);
        lsprintf(buf,"%12s : %s",
            globstring[STR_SCREEN_MODES],
            modes);
        iffinfotxt(prp,buf,7,83+goff);
    }
    else if (show_global_font) {
        char fontname[40],*ptr;

        strcpy(fontname,show_global_font->tf_Message.mn_Node.ln_Name);
        if ((ptr=strstri(fontname,".font"))) *ptr=0;

        lsprintf(buf,"%12s : %s",
            globstring[STR_FONT],
            fontname);
        iffinfotxt(prp,buf,7,19+goff);

        lsprintf(buf,"%12s : %ld",
            globstring[STR_FONT_SIZE],
            (long int)show_global_font->tf_YSize);
        iffinfotxt(prp,buf,7,35+goff);

        lsprintf(buf,"%12s : %ld (%ld - %ld)",
            globstring[STR_NUM_CHARS],
            (long int)((show_global_font->tf_HiChar-show_global_font->tf_LoChar)+1),
            (long int)show_global_font->tf_LoChar,
            (long int)show_global_font->tf_HiChar);
        iffinfotxt(prp,buf,7,51+goff);

        lsprintf(buf,"%12s :",
            globstring[STR_FONT_STYLE]);
        if ((show_global_font->tf_Style&15)==0) strcat(buf," NORMAL");
        else {
            if (show_global_font->tf_Style&FSF_UNDERLINED) strcat(buf," ULINED");
            if (show_global_font->tf_Style&FSF_BOLD) strcat(buf," BOLD");
            if (show_global_font->tf_Style&FSF_ITALIC) strcat(buf," ITALIC");
            if (show_global_font->tf_Style&FSF_EXTENDED) strcat(buf," EXTEND");
        }
        iffinfotxt(prp,buf,7,67+goff);

        lsprintf(buf,"%12s :",globstring[STR_FONT_FLAGS]);
        if (show_global_font->tf_Flags&FPF_TALLDOT) strcat(buf," TALL");
        if (show_global_font->tf_Flags&FPF_WIDEDOT) strcat(buf," WIDE");
        if (show_global_font->tf_Flags&FPF_PROPORTIONAL)
            strcat(buf," PROP");
        iffinfotxt(prp,buf,7,75+goff);

        lsprintf(title,"Font : %s/%ld   %ld chars (%ld - %ld)\n\n",
            fontname,(long int)show_global_font->tf_YSize,
            (long int)((show_global_font->tf_HiChar-show_global_font->tf_LoChar)+1),
            (long int)show_global_font->tf_LoChar,
            (long int)show_global_font->tf_HiChar);
    }
    else if (show_global_icon) {
        lsprintf(buf,"%12s : %s",
            globstring[STR_ICON],
            show_global_icon_name);
        iffinfotxt(prp,buf,7,27+goff);

        lsprintf(buf,"%12s : %s",
            globstring[STR_ICON_TYPE],
            icon_type_names[show_global_icon->do_Type-1]);
        iffinfotxt(prp,buf,7,43+goff);

        lsprintf(buf,"%12s : %s",
            globstring[STR_ICON_ALTERNATE],
            (show_global_icon->do_Gadget.Flags&GFLG_GADGHIMAGE)?
            globstring[STR_YES]:globstring[STR_NO]);
        iffinfotxt(prp,buf,7,59+goff);

        if (show_global_icon->do_DefaultTool) {
            lsprintf(buf,"%12s : %s",
                globstring[STR_ICON_DEFAULTTOOL],
                show_global_icon->do_DefaultTool);
            iffinfotxt(prp,buf,7,75+goff);
        }
        lsprintf(title,"Workbench %s : %s\n\n",
            globstring[STR_ICON],
            show_global_icon_name);
    }

    for (a=0;a<8;a++) printgadgets[a].TopEdge+=goff;
    printcheckimage.ImageData=(UWORD *)DOpusBase->pdb_check;
    AddGadgetBorders(&pkey,printgadgets,4,2,1);
    AddGadgetBorders(&pkey,&printgadgets[PRINT_OKAY],2,2,1);
    AddGadgets(pwin,printgadgets,printgadtxt,8,2,1,1);

    if (!firsttime) {
        struct Preferences prefs;

        GetPrefs(&prefs,sizeof(struct Preferences));
        print_gads_sel[PRINT_ASPECT]=prefs.PrintAspect;
        print_gads_sel[PRINT_IMAGE]=prefs.PrintImage;
        print_gads_sel[PRINT_SHADE]=prefs.PrintShade;

        firsttime=1;
    }

    for (a=0;a<4;a++) {
        DoCycleGadget(&printgadgets[a],
            pwin,
            print_gads_txt[a],
            print_gads_sel[a]);
    }

    ScreenToFront(pscr);
    ActivateWindow(pwin);
    save=main_proc->pr_WindowPtr;
    main_proc->pr_WindowPtr=(APTR)pwin;

    Wait(1<<pwin->UserPort->mp_SigBit);

    FOREVER {
        while ((msg=(struct IntuiMessage *) GetMsg(pwin->UserPort))) {
            class=msg->Class; code=msg->Code; qual=msg->Qualifier;
            if (class==IDCMP_GADGETUP)
                gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
            ReplyMsg((struct Message *) msg);
            switch (class) {
                case IDCMP_VANILLAKEY:
                    if (qual&IEQUALIFIER_REPEAT) break;
                    code = ToLower(code);
                    for (a=0;a<6;a++) {
                        if (code==pactcode[a] /*|| code==ToUpper(pactcode[a])*/) {
//                        if (ToLower(code)==pactcode[a]) {
                            if (a<4) {
//                                if (code==ToUpper(pactcode[a])) qual|=IEQUALIFIER_LSHIFT;
                                SelectGadget(pwin,&printgadgets[a]);
                                gadgetid=a;
                                goto docyclegads;
                            }
                            else if (a==4) {
                                printgadgets[4].Flags^=GFLG_SELECTED;
                                RefreshGList(&printgadgets[4],pwin,NULL,1);
                            }
                            else if (a==5) {
                                printgadgets[5].Flags^=GFLG_SELECTED;
                                RefreshGList(&printgadgets[5],pwin,NULL,1);
                            }
                        }
                    }
                    if (code=='\r' || code==getkeyshortcut(globstring[STR_PRINT])) {
                        SelectGadget(pwin,&printgadgets[6]);
                        goto doprint;
                    }
                    else if (code=='' || code==' ' || code==getkeyshortcut(str_cancelstring)) {
                        SelectGadget(pwin,&printgadgets[7]);
                        goto endprint;
                    }
                    break;

                case IDCMP_GADGETUP:
                    if (gadgetid<PRINT_FORMFD) {
docyclegads:
                        if (qual&IEQUALIFIER_ANYSHIFT) {
                            if ((--print_gads_sel[gadgetid])<0)
                                print_gads_sel[gadgetid]=print_gads_max[gadgetid];
                        }
                        else {
                            if ((++print_gads_sel[gadgetid])>print_gads_max[gadgetid])
                                print_gads_sel[gadgetid]=0;
                        }
                        DoCycleGadget(&printgadgets[gadgetid],
                            pwin,
                            print_gads_txt[gadgetid],
                            print_gads_sel[gadgetid]);
                    }
                    else switch (gadgetid) {
                        case PRINT_CANCEL:
                            goto endprint;
                        case PRINT_OKAY:
                            goto doprint;
                    }
                    break;
            }
        }
    }
doprint:
    RemoveGList(pwin,printgadgets,-1);
    SetAPen(prp,0);
    RectFill(prp,0,0,319,pwin->Height-1);
    SetAPen(prp,1);
    abortprintgad.TopEdge=(pwin->Height-80)/2;
    AddGadgetBorders(&pkey,&abortprintgad,1,2,1);
    AddGadgets(pwin,&abortprintgad,printabortgadtxt,1,2,1,1);
    SetBusyPointer(pwin);
    if ((print_request=(union printerIO *) LCreateExtIO(general_port,sizeof(union printerIO)))) {
        if (!(OpenDevice("printer.device",0,(struct IORequest *)print_request,0))) {
            pd=(struct PrinterData *)print_request->iodrp.io_Device;
            prefs=&pd->pd_Preferences;
            prefs->PrintAspect=print_gads_sel[PRINT_ASPECT];
            prefs->PrintImage=print_gads_sel[PRINT_IMAGE];
            prefs->PrintShade=print_gads_sel[PRINT_SHADE];
            prefs->PrintXOffset=0;

            print_request->ios.io_Command=CMD_WRITE;
            print_request->ios.io_Data="\033#1";
            print_request->ios.io_Length=-1;
            while (DoIO((struct IORequest *)print_request)) {
                reqoverride=pwin;
                a=simplerequest(globstring[STR_ERROR_INITIALISING_PRINTER],
                    globstring[STR_TRY_AGAIN],str_cancelstring,NULL);
                reqoverride=NULL;
                if (!a) goto closeprinter;
            }

            if (printgadgets[5].Flags&GFLG_SELECTED) {
                print_request->ios.io_Command=CMD_WRITE;
                print_request->ios.io_Data=title;
                print_request->ios.io_Length=-1;
                if (DoIO((struct IORequest *)print_request)) goto closeprinter;
            }

            print_request->iodrp.io_Command=PRD_DUMPRPORT;
            print_request->iodrp.io_RastPort=rast;
            print_request->iodrp.io_ColorMap=wind->WScreen->ViewPort.ColorMap;
            //if (system_version2) {
                print_request->iodrp.io_Modes=GetVPModeID(&(wind->WScreen->ViewPort));
            /*}
            else {
                print_request->iodrp.io_Modes=wind->WScreen->ViewPort.Modes;
            }*/
            print_request->iodrp.io_SrcX=x;
            print_request->iodrp.io_SrcY=y;
            print_request->iodrp.io_SrcWidth=w;
            print_request->iodrp.io_SrcHeight=h;
            print_request->iodrp.io_Special=SPECIAL_FULLCOLS|SPECIAL_ASPECT|SPECIAL_NOFORMFEED;
            if (!print_gads_sel[PRINT_PLACE])
                print_request->iodrp.io_Special|=SPECIAL_CENTER;
            if (!(printgadgets[4].Flags&GFLG_SELECTED))
                print_request->iodrp.io_Special|=SPECIAL_NOFORMFEED;

            SendIO((struct IORequest *)print_request);
            FOREVER {
                Wait(1<<pwin->UserPort->mp_SigBit|1<<general_port->mp_SigBit);
                while (GetMsg(general_port));
                while ((msg=(struct IntuiMessage *) GetMsg(pwin->UserPort))) {
                    class=msg->Class;
                    if (class==IDCMP_GADGETUP)
                        gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
                    ReplyMsg((struct Message *) msg);
                    if (class==IDCMP_GADGETUP && gadgetid==PRINT_ABORT) {
                        reqoverride=pwin;
                        abort=simplerequest(globstring[STR_REALLY_ABORT_PRINT],
                            globstring[STR_ABORT],globstring[STR_CONTINUE],NULL);
                        reqoverride=NULL;
                        if (abort) AbortIO((struct IORequest *)print_request);
                        break;
                    }
                }
                if (abort || (CheckIO((struct IORequest *)print_request))) break;
            }
            WaitIO((struct IORequest *)print_request);
            if (!abort && printgadgets[4].Flags&GFLG_SELECTED) {
                print_request->ios.io_Command=CMD_WRITE;
                print_request->ios.io_Data="\f";
                print_request->ios.io_Length=-1;
                DoIO((struct IORequest *)print_request);
            }
closeprinter:
            CloseDevice((struct IORequest *)print_request);
        }
        LDeleteExtIO((struct IORequest *)print_request);
    }
endprint:
    for (a=0;a<8;a++) printgadgets[a].TopEdge-=goff;
    main_proc->pr_WindowPtr=save;
    ActivateWindow(iffwindow);
    ScreenToBack(pscr);
    CloseWindow(pwin);
    CloseScreen(pscr);
    LFreeRemember(&pkey);
}

int InitDHIRES(mp)
int mp;
{
#ifdef __AROS__
#warning Not implemented because of copper usage
//bug("InitDHIRES(): NOT IMPLEMENTED BECAUSE OF COPPER MACRO USAGE\n");
return 0;

#else
    int line,creg,top,oscan,lace;
    struct UCopList *ucop;

    if (!(ucop=(struct UCopList *)AllocMem(sizeof(struct UCopList),MEMF_CLEAR)))
        return(0);

    lace=(viewflags&LACE)?1:0;
    top=copperheight;
    if (lace) top/=2;

    if (mp) {
        if (screenwidth>640) oscan=14;
        else oscan=16;
        CINIT(ucop,top*13);
        for (line=0;line<top;line++) {
            CWAIT(ucop,(line-1)<<lace,112);
            for (creg=4;creg<oscan;creg++) {
                CMove(ucop,(long *)(0xdff180+(creg*2)),copperlist[(line<<(4+lace))+creg]);
                CBump(ucop);
            }
        }
    }
    else {
        CINIT(ucop,copperheight*17);
        for (line=0;line<copperheight;line++) {
            CWAIT(ucop,line-1,122);
            for (creg=4;creg<16;creg++) {
                CMove(ucop,(long *)(0xdff180+(creg*2)),copperlist[(line<<4)+creg]);
                CBump(ucop);
            }
        }
    }

    CEND(ucop);
    Forbid();
    ivp->UCopIns=ucop;
    Permit();
    RethinkDisplay();
    return(1);
#endif
}

void getviewmodes(modes)
char *modes;
{
    if (iffscreenname[0]) strcpy(modes,iffscreenname);
    else {
        if (viewflags&SUPERHIRES) {
            strcpy(modes,"SUPERHIRES");
            if (bmhd.bmh_Width>1280) strcat(modes," OSCAN");
        }
        else if (viewflags&HIRES) {
            strcpy(modes,"HIRES");
            if (bmhd.bmh_Width>640) strcat(modes," OSCAN");
        }
        else {
            strcpy(modes,"LORES");
            if (bmhd.bmh_Width>320) strcat(modes," OSCAN");
        }
        if (viewflags&LACE) strcat(modes," LACE");
    }
    if (copperlist) strcat(modes," DHIRES");
#ifdef WITHPCHG
    else if (pchghead) {
        if (sham) strcat(modes," SHAM");
        else strcat(modes," PCHG");
    }
#endif
    else 
    {
        if (viewflags&HAM) strcat(modes," HAM");
        else if (viewflags&EXTRA_HALFBRITE) strcat(modes," EHB");
    }
}

void iffinfotxt(r,buf,x,y)
struct RastPort *r;
char *buf;
int x,y;
{
    int a,l;

    a=strlen(buf); l=(316-x)/8;
    Move(r,x,y);
    Text(r,buf,(a>l)?l:a);
}

void build_palettes(colourdata,coloursize,ctable4,ctable8)
unsigned char *colourdata;
int coloursize;
UWORD *ctable4;
ULONG *ctable8;
{
    int a,b;
    struct ColorRegister rgb;

    if (ctable8) {
        for (a=0,b=1;a<coloursize;a++,b++)
/*
            ctable8[b]=(colourdata[a]<<24)|0x00ffffff;
*/
            ctable8[b]=
                ((colourdata[a]<<24)&0xff000000)|
                ((colourdata[a]<<16)&0x00ff0000)|
                ((colourdata[a]<<8)&0x0000ff00)|
                ((colourdata[a]&0x000000ff));

        ctable8[0]=numcolours<<16;
        ctable8[(numcolours*3)+1]=0;
    }

    if (ctable4) {
        for (a=0;a<numcolours;a++) {
            rgb.red   = *(colourdata++)>>4;
            rgb.green = *(colourdata++)>>4;
            rgb.blue  = *(colourdata++)>>4;
            ctable4[a]=(rgb.red<<8)+(rgb.green<<4)+rgb.blue;
        }
    }
}
