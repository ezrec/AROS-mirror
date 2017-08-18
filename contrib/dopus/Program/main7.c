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
#include <proto/inovamusic.h>
#endif
#include <proto/ahi.h>
#include <workbench/icon.h>

#ifndef SAMPLETYPE_Stereo
#define SAMPLETYPE_Stereo SVX_STEREO
#endif

#define RAWKEY_ESC 0x45
#define RAWKEY_Q   0x10
#define RAWKEY_X   0x32

static struct DOpusRemember *audio_key;      /* Memory key for 8SVX player */

static struct MsgPort *audio_port[2];        /* 8SVX Sound player Ports */

static struct IOAudio *audio_req1[2];        /* Audio IO Request block #1 */
static struct IOAudio *audio_req2[2];        /* Audio IO Request block #2 */

static UBYTE *audiodata;                     /* Audio data to play */
static ULONG audio_size;                     /* Size of audio data */

struct Library      *AHIBase;
static struct MsgPort      *AHImp;
static struct AHIRequest   *AHIio;
static BYTE                 AHIDevice = -1;
static struct AHIAudioCtrl *actrl;

BOOL useAHI = FALSE;

int showpic(fullname,np)
char *fullname;
int np;
{
    int res,a;
    char buf[256];

    if (checkexec(fullname) && checkisfont(fullname,buf))
        return((showfont(buf,atoi(BaseName(fullname)),np)));

    a=strlen(fullname);
    if (a>5 && Stricmp(&fullname[a-5],".info")==0) {
        if ((res=readicon(fullname,np))==0) return(1);
        if (res==-2) {
            doerror(-1);
            return(0);
        }
        if (res==-3) {
            dostatustext(globstring[STR_NO_CHIP_FOR_ICON]);
            return(0);
        }
        return(res);
    }

    if (!(res=LoadPic(fullname))) doerror(-1);
    else if (res==IFFERR_NOTILBM) dostatustext(globstring[STR_FILE_NOT_ILBM]);
    else if (res==IFFERR_BADIFF) dostatustext(globstring[STR_ERROR_IN_IFF]);
    else if (res==IFFERR_NOMEMORY) dostatustext(globstring[STR_NO_CHIP_FOR_PICTURE]);
    else if (res==IFFERR_BADMODE) dostatustext(globstring[STR_SCREEN_MODE_UNAVAILABLE]);
    else if (res==IFFERR_NOSCREEN) dostatustext(globstring[STR_UNABLE_TO_OPEN_WINDOW]);
    if (res==-1) return(-1);
    else if (!res || res<-1) return(0);
    return(1);
}

int checkisfont(pathname,fontname)
char *pathname,*fontname;
{
    int a;
    char fontsize[36],fontpath[256],*ptr;

    strcpy(fontpath,pathname);
    if ((ptr=BaseName(fontpath))) {
        strcpy(fontsize,ptr);
        *(--ptr)=0;
        if ((ptr=BaseName(fontpath))) {
            for (a=0;;a++) {
                if (!(_isdigit(fontsize[a]))) break;
            }
            if (!fontsize[a]) {
                strcat(fontpath,".font");
                if (CheckExist(fontpath,NULL)) {
                    strcpy(fontname,fontpath);
                    return(1);
                }
            }
        }
    }
    return(0);
}

int readicon(name,np)
char *name;
int np;
{
    struct DiskObject *dobj;
    struct Gadget *gad;
    struct Image *icon_image[2];
    UWORD coltab[256];
    int fred,ret,width,height,x,y,x1,y1,imagenum,depth;

    if (!IconBase) return(0);

    name[(strlen(name)-5)]=0;
    if (IconBase->lib_Version >= 44) dobj=GetIconTags(name, ICONGETA_Screen, NULL, ICONGETA_GenerateImageMasks, FALSE, TAG_END);
    else dobj=GetDiskObject(name);

    if (dobj == NULL) return(-2);

    gad=&(dobj->do_Gadget);

    if (IconBase->lib_Version >= 44)
     {
      icon_image[0]=(struct Image *)1;
      depth = 8;
     }
    else
     {
      icon_image[0]=(struct Image *)gad->GadgetRender;
      depth=icon_image[0]->Depth;
D(bug("icon depth: %ld\n",icon_image[0]->Depth));
//      if (depth>4) depth=4;
     }
    if (!icon_image[0] || !(setupfontdisplay(depth,coltab))) {
        FreeDiskObject(dobj);
        return(-3);
    }
    if (IconBase->lib_Version >= 44)
     {
//      long isPMicon=0;

//      IconControl(dobj,ICONCTRLA_IsPaletteMapped,&isPMicon,TAG_END);
      LoadRGB4(&fontscreen->ViewPort,coltab,256);
      LayoutIconA(dobj,fontscreen,NULL);
      LoadRGB4(&fontscreen->ViewPort,nullpalette,256);
      icon_image[0]=(struct Image *)(gad->GadgetRender);
     }

    if (gad->Flags&GADGHIMAGE) icon_image[1]=(struct Image *)gad->SelectRender;
    else icon_image[1]=icon_image[0];

    for (x=0;x<2;x++) icon_image[x]->NextImage=NULL;

    imagenum=0;

    ScreenToFront(fontscreen);
    ActivateWindow(fontwindow);

    width=fontscreen->Width;
    height=fontscreen->Height;

    DrawImage(fontwindow->RPort,icon_image[0],
        ((width-icon_image[0]->Width)/2),
        ((height-icon_image[0]->Height)/2));

    FadeRGB4(fontscreen,coltab,(1<<depth),1,config->fadetime);
    show_global_icon=dobj;
    show_global_icon_name=BaseName(name);

    FOREVER {
        if ((fred=WaitForMouseClick(2,fontwindow))==-2) {
            imagenum=1-imagenum;
            x=(width-icon_image[imagenum]->Width)/2;
            y=(height-icon_image[imagenum]->Height)/2;
            x1=x+icon_image[imagenum]->Width;
            y1=y+icon_image[imagenum]->Height;
            DrawImage(fontwindow->RPort,icon_image[imagenum],x,y);
            drawrecaround(fontwindow->RPort,/*0,0,*/x,y,x1,y1,width,height);
        }
        else {
            if (fred==0 || fred==-3) ret=TRUE;
            else ret=-1;
            break;
        }
    }
    show_global_icon=NULL;
    if (fred!=-3) FadeRGB4(fontscreen,coltab,(1<<depth),-1,config->fadetime);
//cleanicon: // HUX - this label is unused
    FreeDiskObject(dobj);
    cleanup_fontdisplay();
    return(ret);
}

void drawrecaround(r,/*l,t,*/x,y,x1,y1,width,height)
struct RastPort *r;
int /*l,t,*/x,y,x1,y1,width,height;
{
    UBYTE o;

    o=GetAPen(r); SetAPen(r,0);
    if (x>0) RectFill(r,/*l*/0,/*t*/0,/*l+*/x-1,/*t+*/height-1);
    if (x1 < /*l+*/width) RectFill(r,/*l+*/x1,/*t*/0,/*l+*/width-1,/*t+*/height-1);
    if (y>0) RectFill(r,/*l*/0,/*t*/0,/*l+*/width-1,/*t+*/y-1);
    if (y1 < /*t+*/height) RectFill(r,/*l*/0,/*t+*/y1,/*l+*/width-1,/*t+*/height-1);
    SetAPen(r,o);
}

/******************************************************************************
**** OpenAHI ******************************************************************
******************************************************************************/

/* Open the device for low-level usage */

BOOL OpenAHI(void)
{
  if((AHImp = CreateMsgPort())) {
    if((AHIio = (struct AHIRequest *)CreateIORequest(AHImp,sizeof(struct AHIRequest)))) {
      AHIio->ahir_Version = 4;
      if(!(AHIDevice = OpenDevice(AHINAME, AHI_NO_UNIT,(struct IORequest *) AHIio,0))) {
        AHIBase = (struct Library *) AHIio->ahir_Std.io_Device;
        return TRUE;
      }
    }
  }
  return FALSE;
}


/******************************************************************************
**** CloseAHI *****************************************************************
******************************************************************************/

/* Close the device */

void CloseAHI(void)
{
  AHI_FreeAudio(actrl);
  actrl = NULL;
  if(! AHIDevice) CloseDevice((struct IORequest *)AHIio);
  AHIDevice=-1;
  DeleteIORequest((struct IORequest *)AHIio);
  AHIio=NULL;
  DeleteMsgPort(AHImp);
  AHImp=NULL;
}

BYTE AHIsignal = -1;
static int AHIloops;

#ifdef __MORPHOS__
ULONG AHISoundFunc(void);

struct EmulLibEntry GATE_AHISoundFunc = { TRAP_LIB, 0, (void (*)(void))AHISoundFunc };

ULONG AHISoundFunc(void)
 {
  struct AHIAudioCtrl *actrl = (struct AHIAudioCtrl *)REG_A2;
  struct AHISoundMessage *smsg = (struct AHISoundMessage *)REG_A1;
#else
__saveds ULONG AHISoundFunc(
                   register struct AHIAudioCtrl *actrl __asm("a2"),
                   register struct AHISoundMessage *smsg __asm("a1"))
{
#endif
    if (AHIloops)  // Will it work for stereo sounds too?
     {
      AHI_SetSound(smsg->ahism_Channel,AHI_NOSOUND,0,0,actrl,0);
      Signal(actrl->ahiac_UserData,1L<<AHIsignal);
     }
    else AHIloops++;
//D(bug("AHISoundFunc() called\n"));
    return 0;
}

static struct Hook AHISoundHook;

int doplay8svx(fname,loop)
char *fname;
int loop;
{
    struct VoiceHeader *vhdr=NULL;
    UBYTE *p8data;
    ULONG size,class,rsize,chan=0,pan=Unity/2,waitbits,sigs;
    UWORD code;
    char *psample,*ry,*compressbuf;
    ChunkHeader *p8chunk;
    int a,b,stereo,*vxcheck,finish,playsize;
    struct IOAudio *audioptr[2];
    UBYTE *playdata[2];
/*
    for (a=0;a<2;a++) {
        audio_port[a]=NULL;
        audio_req1[a]=audio_req2[a]=NULL;
    }
    audiodata=NULL;
*/
    status_flags&=~STATUS_AUDIOLED;

    if ((a=readfile(fname,(char **)&audiodata,(int *)&audio_size))) {
        if (a==-1) return(0);
        return(-2);
    }
    vxcheck=(int *)audiodata;
    if (audio_size<12 || vxcheck[0]!=ID_FORM || vxcheck[2]!=ID_8SVX) { // Raw data
        size=audio_size;
        psample=p8data=audiodata;
    }
    else {                                                             // 8SVX sample
        p8data=audiodata+12;
        size=0;
        while (p8data<audiodata+vxcheck[1]) {
            p8chunk=(ChunkHeader *)p8data;
            p8data+=sizeof(ChunkHeader);
            switch (p8chunk->ckID) {
                case ID_VHDR:
                    vhdr=(struct VoiceHeader *)p8data;
                    break;
                case ID_BODY:
                    psample=p8data;
                    size=p8chunk->ckSize;
                    break;
                case ID_CHAN:
                    chan = *((ULONG *)p8data);
                    break;
                case ID_PAN:
                    pan = *((ULONG *)p8data);
                    break;
            }
            p8data+=p8chunk->ckSize;
            if (p8chunk->ckSize&1) ++p8data;
        }
    }

    if (size>vxcheck[1]) {
        size/=2; chan=0;
    }

    if (vhdr) {
        if (vhdr->vh_Compression==CMP_FIBDELTA) {
            size-=2;
            if (!(compressbuf=LAllocRemember(&audio_key,size*2,MEMF_ANY))) return(-2);
            DUnpack(psample+2,size,compressbuf,psample[1]);
            psample=compressbuf; size*=2;
        }
    }

    if (chan==SAMPLETYPE_Stereo) {
        size/=2;
        stereo=size;
    }
    else stereo=0;

    if (useAHI)
     {
D(bug("Trying to play through AHI\n"));
      if (OpenAHI())
       {
D(bug("AHI opened\n"));
#ifdef __MORPHOS__
        AHISoundHook.h_Entry = &GATE_AHISoundFunc;
#else
        AHISoundHook.h_Entry = AHISoundFunc;
#endif
        if ((actrl = AHI_AllocAudio(AHIA_AudioID,         AHI_DEFAULT_ID,
                                    AHIA_MixFreq,         AHI_DEFAULT_FREQ,
                                    AHIA_Channels,        stereo?2:1,
                                    AHIA_Sounds,          stereo?2:1,
                                    loop?TAG_IGNORE:AHIA_SoundFunc, (IPTR)&AHISoundHook,
                                    AHIA_UserData,        FindTask(NULL),
                                    TAG_DONE)))
         {
          struct AHISampleInfo sample = { AHIST_M8S, psample, size /*/ AHI_SampleFrameSize(AHIST_M8S)*/ };
          UWORD snd0,snd1=1;

D(bug("AHI_AllocAudio() succeeded\n"));
          snd0 = AHI_LoadSound(0, AHIST_SAMPLE, &sample, actrl);
          if (stereo)
           {
            sample.ahisi_Address = psample+stereo;
            snd1 = AHI_LoadSound(1, AHIST_SAMPLE, &sample, actrl);
           }
D(bug("LoadSound(0)=%ld, LoadSound(1)=%ld\n",snd0,snd1));
          if ((snd0 != AHI_NOSOUND) && (snd1 != AHI_NOSOUND))
           {
            if ((AHIsignal=AllocSignal(-1))!=-1)
             {
              struct TagItem ahitags2[] =
              {
                { AHIP_BeginChannel, 1 },
                { AHIP_Freq        , vhdr?vhdr->vh_SamplesPerSec:10000 },
                { AHIP_Vol         , vhdr?vhdr->vh_Volume:Unity },
                { AHIP_Pan         , 0L },
                { AHIP_Sound       , 1 },
                { AHIP_EndChannel  , 0 },
                { TAG_END }
              };

              AHI_ControlAudio(actrl, AHIC_Play, TRUE, TAG_END);

              AHI_Play(actrl,
                AHIP_BeginChannel, 0,
                AHIP_Freq        , vhdr?vhdr->vh_SamplesPerSec:10000,
                AHIP_Vol         , vhdr?vhdr->vh_Volume:Unity,
                AHIP_Pan         , stereo?Unity:pan,
                AHIP_Sound       , 0,
                AHIP_EndChannel  , NULL,
                stereo?TAG_MORE:TAG_END, (IPTR)ahitags2);
D(bug("Playing through AHI\n"));
             }
            else CloseAHI();
           }
          else CloseAHI();
         }
        else CloseAHI();
       }
      else CloseAHI();
     }
    if (actrl == NULL)
     {
      static UBYTE audiochannels[2][4]={{8+1,1,8,4},{4+2,2,4,8}};

D(bug("Trying to play through audio.device\n"));
      for (a=0;a<2;a++) {
          if (!(audio_req1[a]=LAllocRemember(&audio_key,sizeof(struct IOAudio),MEMF_CLEAR)) ||
              !(audio_req2[a]=LAllocRemember(&audio_key,sizeof(struct IOAudio),MEMF_CLEAR)) ||
              !(audio_port[a]=LCreatePort(NULL,0))) return(-2);
      }

      for (a=0;a<2;a++) {
          audio_req1[a]->ioa_Request.io_Message.mn_ReplyPort=audio_port[a];
          audio_req1[a]->ioa_Request.io_Message.mn_Node.ln_Pri=75;
          audio_req1[a]->ioa_Data=audiochannels[a];
          audio_req1[a]->ioa_Length=sizeof(audiochannels[a]);
          if (OpenDevice("audio.device",0,(struct IORequest *)audio_req1[a],0)) return(-6);
      }

      playsize=(size<25600)?size:25600;
      for (a=0;a<2;a++) {
          if (!(playdata[a]=LAllocRemember(&audio_key,playsize,MEMF_CHIP)))
              return(-2);
      }

      for (a=0;a<2;a++) {
          audio_req1[a]->ioa_Request.io_Command=CMD_WRITE;
          audio_req1[a]->ioa_Request.io_Flags=ADIOF_PERVOL;
          audio_req1[a]->ioa_Volume=64*(a?Unity-pan:pan)/Unity;
          audio_req1[a]->ioa_Period=data_colorclock/(vhdr?vhdr->vh_SamplesPerSec:10000);
          audio_req1[a]->ioa_Cycles=1;
          CopyMem((char *)audio_req1[a],(char *)audio_req2[a],sizeof(struct IOAudio));
          audio_req1[a]->ioa_Data=(UBYTE *)playdata[a];
          audio_req2[a]->ioa_Data=(UBYTE *)playdata[a]+playsize/2/*12800*/;
          audioptr[a]=audio_req2[a];
      }
     }

    if (config->viewbits&VIEWBITS_FILTEROFF) {
        if (filteroff()) status_flags|=STATUS_AUDIOLED;
        else status_flags&=~STATUS_AUDIOLED;
    }

    waitbits = 1<<Window->UserPort->mp_SigBit;
    if (actrl == NULL) waitbits |= 1<<audio_port[0]->mp_SigBit | 1<<audio_port[1]->mp_SigBit;
    else if (!loop) waitbits |= 1<<AHIsignal;
/*
    if (size<=25600) {
        for (a=0;a<2;a++) CopyMem(psample+(a*stereo),(char *)playdata[a],playsize);
        FOREVER {
            for (b=0;b<2;b++) {
                audio_req1[b]->ioa_Length=size;
                BeginIO((struct IORequest *)audio_req1[b]);
            }
            a=0;
            FOREVER {
                Wait(waitbits);
                while (getintuimsg()) {
                    class=IMsg->Class; code=IMsg->Code;
                    ReplyMsg((struct Message *)IMsg);
                    if (class==IDCMP_MOUSEBUTTONS) {
                        if (code==SELECTDOWN) return(1);
                        else if (code==MENUDOWN) return(-1);
                    }
                    else if (class==IDCMP_RAWKEY) {
                        if (code==RAWKEY_ESC) return(-1);
                        else if (code==RAWKEY_Q || code==RAWKEY_X) return(1);
                    }
                }
                for (b=0;b<2;b++) if (GetMsg(audio_port[b])) ++a;
                if (a>=2) break;
            }
            if (!loop) return(1);
        }
    }
    else
*/
    {
        ry=psample;
        rsize=size;
        finish=0;
        FOREVER {
            if (actrl == NULL) {
                size=rsize;
                psample=ry;
                for (a=0;a<2;a++) {
                    CopyMem(psample+(a*stereo),(char *)playdata[a],playsize);
    //                CopyMem((char *)audio_req1[a],(char *)audio_req2[a],sizeof(struct IOAudio));
    //                audio_req1[a]->ioa_Data=(UBYTE *)playdata[a];
    //                audio_req2[a]->ioa_Data=(UBYTE *)playdata[a]+12800;
                      audio_req1[a]->ioa_Length=audio_req2[a]->ioa_Length=playsize/2/*12800*/;
                      audioptr[a]=audio_req2[a];
                }
                psample+=playsize;
                size-=playsize/*25600*/;
                for (a=0;a<2;a++) BeginIO((struct IORequest *)audio_req1[a]);
                for (a=0;a<2;a++) BeginIO((struct IORequest *)audio_req2[a]);
                a=0;
            }
            FOREVER {
                sigs = Wait(waitbits);
                while (getintuimsg()) {
                    class=IMsg->Class; code=IMsg->Code;
                    ReplyMsg((struct Message *)IMsg);
                    if (class==IDCMP_MOUSEBUTTONS) {
                        if (code==SELECTDOWN) return(1);
                        else if (code==MENUDOWN) return(-1);
                    }
                    else if (class==IDCMP_RAWKEY) {
                        if (code==RAWKEY_ESC) return(-1);
                        else if (code==RAWKEY_Q || code==RAWKEY_X) return(1);
                    }
                }
                if (actrl == NULL) {
                    for (b=0;b<2;b++) {
                        if (GetMsg(audio_port[b]) && !(a&(1<<b))) {
                            if (size>0) {
                                if (audioptr[b]==audio_req1[b]) audioptr[b]=audio_req2[b];
                                else audioptr[b]=audio_req1[b];
                                audioptr[b]->ioa_Length=(size<(playsize/2/*12800*/)?size:(playsize/2)/*12800*/);
                                CopyMem(psample+(b*stereo),(char *)audioptr[b]->ioa_Data,audioptr[b]->ioa_Length);
                            }
                            a|=1<<b;
                        }
                    }
                    if (a==3) {
                        if (size<=0) {
                            if (finish) {
                                finish=0;
                                break;
                            }
                            finish=1;
                        }
                        else {
                            size-=audioptr[0]->ioa_Length;
                            psample+=playsize/2/*12800*/;
                            for (b=0;b<2;b++) BeginIO((struct IORequest *)audioptr[b]);
                        }
                        a=0;
                    }
                }
                else if (sigs & (1<<AHIsignal))
                 {
//D(bug("AHIsignal received\n"));
                  break;
                 }
            }
            if (!loop) return(1);
        }
    }
}

void kill8svx()
{
    int a;

    if (actrl)
     {
      AHI_ControlAudio(actrl, AHIC_Play, FALSE, TAG_DONE);
      FreeSignal(AHIsignal);
      CloseAHI();
     }
    else
     {
      for (a=0;a<2;a++) {
          if (audio_req2[a] && audio_req2[a]->ioa_Request.io_Device) {
              audio_req2[a]->ioa_Request.io_Command=CMD_FLUSH;
              DoIO((struct IORequest *)audio_req2[a]);
          }
          if (audio_req1[a] && audio_req1[a]->ioa_Request.io_Device) {
              audio_req1[a]->ioa_Request.io_Command=CMD_FLUSH;
              DoIO((struct IORequest *)audio_req1[a]);
              CloseDevice((struct IORequest *)audio_req1[a]);
          }
          audio_req1[a]=NULL;
          audio_req2[a]=NULL;
      }
      for (a=0;a<2;a++) {
          if (audio_port[a]) {
              while (GetMsg(audio_port[a]));
              LDeletePort(audio_port[a]);
              audio_port[a]=NULL;
          }
      }
     }
    if (audiodata && audio_size) FreeVec(audiodata);
    LFreeRemember(&audio_key);
    audiodata=NULL;
    if (status_flags&STATUS_AUDIOLED) {
        filteron();
        status_flags&=~STATUS_AUDIOLED;
    }
}

void handle8svxerror(res)
int res;
{
    switch (res) {
        case 0: doerror(-1); break;
        case -2: doerror(ERROR_NO_FREE_STORE); break;
        case -3:
        case -4: dostatustext(globstring[STR_ERROR_IN_IFF]); break;
        case -6: dostatustext(globstring[STR_CANT_ALLOCATE_AUDIO]); break;
    }
}

void dosound(type)
int type;
{
    if (type) {
      struct IOAudio audio;
      static UBYTE achannels[8]={1+2,1+4,2+8,4+8,1,2,4,8};
      int a;

D(bug("beepwave at %lx\n",beepwave));
      if (useAHI)
       {
D(bug("Trying to play through AHI\n"));
        if (OpenAHI())
         {
D(bug("AHI opened\n"));
          if ((actrl = AHI_AllocAudio(AHIA_AudioID,  AHI_DEFAULT_ID,
                                      AHIA_MixFreq,  AHI_DEFAULT_FREQ,
                                      AHIA_Channels, 1,
                                      AHIA_Sounds,   1,
                                      TAG_DONE)))
           {
            struct AHISampleInfo sample = { AHIST_M8S, beepwave, 16 };

D(bug("AHI_AllocAudio() succeeded\n"));
            if (AHI_LoadSound(0, AHIST_SAMPLE, &sample, actrl) != AHI_NOSOUND)
             {
D(bug("Playing through AHI\n"));
              AHI_ControlAudio(actrl, AHIC_Play, TRUE, TAG_END);

              for (a=0;a<11;a++)
               {
                AHI_Play(actrl,
                    AHIP_BeginChannel, 0,
                    AHIP_Freq        , (a%2)?6000:9000,
                    AHIP_Vol         , 0x10000L,
                    AHIP_Pan         , 0x8000L,
                    AHIP_Sound       , 0,
                    AHIP_EndChannel  , NULL,
                    TAG_END);
                Delay(6);
               }

              AHI_ControlAudio(actrl, AHIC_Play, FALSE, TAG_DONE);

              CloseAHI();

              return;
             }
            else CloseAHI();
           }
          else CloseAHI();
         }
        else CloseAHI();
       }
D(bug("Trying to play through audio.device\n"));
      audio.ioa_Request.io_Message.mn_ReplyPort=general_port;
      audio.ioa_Request.io_Message.mn_Node.ln_Pri=90;
      audio.ioa_Data=achannels;
      audio.ioa_Length=sizeof(achannels);
      if (OpenDevice("audio.device",0,(struct IORequest *)&audio,0)==0) {
          audio.ioa_Request.io_Command=CMD_WRITE;
          audio.ioa_Request.io_Flags=ADIOF_PERVOL;
          audio.ioa_Volume=64;
          audio.ioa_Data=(UBYTE *)beepwave;
          audio.ioa_Length=16;
          audio.ioa_Cycles=60;

          for (a=0;a<11;a++) {
              audio.ioa_Period=(a%2)?600:400;
              BeginIO((struct IORequest *)&audio);
              WaitIO((struct IORequest *)&audio);
          }

          CloseDevice((struct IORequest *)&audio);
      }
      else dostatustext(globstring[STR_CANT_ALLOCATE_AUDIO]);
    }
    else DisplayBeep(NULL);
}

int playmod(name)
char *name;
{
#ifndef __AROS__
    int a;

    if ((a=PlayModule(name,1))) {
        switch (a) {
            case ML_NOMEM: doerror(ERROR_NO_FREE_STORE); break;
            case ML_BADMOD:
                dostatustext(globstring[STR_NOT_ST_MOD]);
                break;
            case ML_NOMOD: doerror(ERROR_OBJECT_NOT_FOUND); break;
/*
            default:
                doerror(0);
                break;
*/
        }
        FlushModule();
        return(0);
    }
#endif
    return(1);
}

int check_is_module(name)
char *name;
{
#ifndef __AROS__
    int a;

    if (MUSICBase) {
        a=IsModule(name);
        if (a>0 && a<100) return(1);
    }
#endif
    return(0);
}

static char codetodelta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

char DUnpack(source,n,dest,x)
char *source;
int n;
char *dest,x;
{
    unsigned char d;
    int i,lim;

    lim=n<<1;
    for (i=0;i<lim;++i) {
        d=source[i>>1];
        if (i&1) d&=0xf;
        else d>>=4;
        x+=codetodelta[d];
        dest[i]=x;
    }
    return(x);
}

int EnvoyPacket(device,action,action2,data,buffer)
char *device;
ULONG action,action2;
UWORD data;
APTR buffer;
{
    struct MsgPort *handler;
    ULONG args[2];

    if (!(handler=(struct MsgPort *)DeviceProc(device))) return(-1);

    args[0]=data;
    args[1]=(ULONG)buffer;
    if (!(SendPacket(handler,action,args,2))) {
        if (action2) return(!(SendPacket(handler,action2,args,2)));
        return(1);
    }
    return(0);
}

int filteroff(void)                          //_filteroff:   ;int
{
  char *filter_register = (char *)0xBFE001L;

  if( (*filter_register) & 2 )               //  btst.b #1,$bfe001
                                             //  bne alreadyon
    return(0);                               //alreadyon:
                                             //  moveq.l #0,d0
                                             //  rts
  (*filter_register) |= 2;                   //  bset.b #1,$bfe001
  return(1);                                 //  moveq.l #1,d0
                                             //  rts
}

void filteron(void)                          //_filteron:    ;void
{
  char *filter_register = (char *)0xBFE001L;

  (*filter_register) &= (~2);                //  bclr.b #1,$bfe001
                                             //  rts
}
