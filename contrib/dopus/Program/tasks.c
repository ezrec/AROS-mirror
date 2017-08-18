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
#include <proto/sysinfo.h>
#endif

#ifdef __AROS__
AROS_UFP2(struct InputEvent *, keyhandler,
    AROS_UFPA(struct InputEvent *, oldevent, A0),
    AROS_UFPA(APTR, userdata, A1)
);
#endif


#define HOTKEY_UNICONIFY   1
#define HOTKEY_ABORT     3
#define HOTKEY_MMB     5
#define HOTKEY_HOTKEY    10

struct ProgressBar
 {
  int barX, barY;
  int descY;
  int curr,max;
  int last_w;
  BOOL incignore;
  BOOL hide;
 };

void progressbar(struct ProgressBar *bar);

#ifdef INPUTDEV_HOTKEY
#ifdef __MORPHOS__
static struct InputEvent *keyhandler(void);

static struct EmulLibEntry GATE_keyhandler = { TRAP_LIB, 0, (void (*)(void))keyhandler };
#endif
#endif
struct Gadget
  abortopgad={
    NULL,0,0,104,0,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
    NULL,NULL,NULL,0,NULL,0,NULL};

static struct NewWindow
  progresswindow={
    0,0,0,0,255,255,
    IDCMP_GADGETUP|IDCMP_RAWKEY,WFLG_RMBTRAP|WFLG_ACTIVATE,
    NULL,NULL,NULL,NULL,NULL,0,0,0,0,CUSTOMSCREEN};

static struct Window *pwindow;
static struct RastPort *prp;
static struct DOpusRemember *prog_key;
static struct ProgressBar bar[2];
//int prog_barx[2],prog_bary[2],prog_texty[2],prog_val[2],prog_tot[2];
static int prog_xoff,prog_yoff,prog_xextra,prog_yextra,prog_areax;
#ifdef INPUTDEV_HOTKEY
static struct Interrupt
  hotkey_interrupt={
#if (AROS_FLAVOUR && AROS_FLAVOUR_BINCOMPAT || !defined(__AROS__))
    {NULL,NULL,2,52,"hotkeez_port"},
#else
    {NULL,NULL,"hotkeez_port",2,52},
#endif
#ifdef __MORPHOS__
    NULL,(VOID *)&GATE_keyhandler};
#else
    NULL,(VOID *)keyhandler};
#endif
#endif
static struct NewBroker
  hotkey_broker={
    NB_VERSION,
    NULL,
    "Directory Opus © Jonathan Potter",
    "The most amazing program ever written",
    0,COF_SHOW_HIDE,100,NULL,0};

static IX hotkey_ix={IX_VERSION};

void __saveds hotkeytaskcode()
{
  char cxname[20];
  int top,__unused sig,waitbits,commodity=0,command,x,run=1;
  struct dopustaskmsg *hmsg;
#ifdef INPUTDEV_HOTKEY
  struct IOStdReq *inreq;
#endif
  struct MsgPort *inputport/*,*idcmpport*/;
  struct IntuiMessage *msg;
  ULONG class,msgid,msgtype;
  UWORD gadgetid/*,norm_height,norm_width*/;
//  struct IntuiMessage *dummymsg;
  struct dopushotkey *hotkey;
  CxObj *broker,*hotkey_filter,*mmb_filter=NULL;
  CxMsg *cxmsg;
/*
  norm_height=scrdata_norm_height;
  norm_width=scrdata_norm_width;
*/
  hotkeymsg_port=LCreatePort(NULL,0);
  inputport=LCreatePort(NULL,0);
//  idcmpport=LCreatePort(NULL,0);

  if (CxBase
#ifdef _USE_SMALL_Q
      && !(status_flags&STATUS_IANSCRAP)
#endif
     ) {
    strcpy(cxname,"Directory Opus");
    if (system_dopus_runcount)
     {
      char tmp[8];

      lsprintf(tmp," (%ld)",(long int)(system_dopus_runcount+1));
      strcat(cxname,tmp);
     }
    hotkey_broker.nb_Name = cxname;
    hotkey_broker.nb_Port=inputport;
    if ((broker=CxBroker(&hotkey_broker,NULL))) {

      /* Initialise main hotkey */
      if ((hotkey_filter=set_dopus_filter(broker,inputport,NULL,config->hotkeycode,config->hotkeyqual,HOTKEY_UNICONIFY,1))) {

        hotkey_ix.ix_Code=IECODE_LBUTTON;
        hotkey_ix.ix_Qualifier=IEQUALIFIER_RBUTTON;
        hotkey_ix.ix_QualMask=0xffff&~(IEQUALIFIER_LEFTBUTTON|IEQUALIFIER_RELATIVEMOUSE|IEQUALIFIER_CAPSLOCK);
        if (set_dopus_filter(broker,inputport,"rawmouse lbutton",
          0,0,HOTKEY_ABORT,0)) {

          hotkey_ix.ix_Code=IECODE_RBUTTON;
          hotkey_ix.ix_Qualifier=IEQUALIFIER_LEFTBUTTON;
          hotkey_ix.ix_QualMask=0xffff&~(IEQUALIFIER_RBUTTON|IEQUALIFIER_RELATIVEMOUSE|IEQUALIFIER_CAPSLOCK);
          if (set_dopus_filter(broker,inputport,"rawmouse rbutton",
            0,0,HOTKEY_ABORT,0)) {

            if (config->hotkeyflags&HOTKEY_USEMMB) {
              hotkey_ix.ix_Code=IECODE_MBUTTON;
              hotkey_ix.ix_Qualifier=IEQUALIFIER_MIDBUTTON;
              hotkey_ix.ix_QualMask=0xffff&~(IEQUALIFIER_RELATIVEMOUSE|IEQUALIFIER_CAPSLOCK);
              mmb_filter=set_dopus_filter(broker,inputport,"rawmouse midbutton",
                0,0,HOTKEY_MMB,1);
            }

            add_hotkey_objects(broker,inputport,1);
            set_hotkey(hotkey_filter,config->hotkeycode,config->hotkeyqual);

            ActivateCxObj(broker,1);
            commodity=1;
          }
        }
      }
    }
    if (broker && (!commodity)) {
      add_hotkey_objects(broker,inputport,0);
      DeleteCxObjAll(broker);
    }
  }
#ifdef INPUTDEV_HOTKEY
  if (!commodity) {
    inreq=(struct IOStdReq *)LCreateExtIO(inputport,sizeof(struct IOStdReq));
    OpenDevice("input.device",NULL,(struct IORequest *)inreq,NULL);
    inreq->io_Data=(APTR)&hotkey_interrupt;
    inreq->io_Command=IND_ADDHANDLER;
    DoIO((struct IORequest *)inreq);
  }
#endif
  waitbits=1<<hotkeymsg_port->mp_SigBit/*|1<<idcmpport->mp_SigBit*/;
  if (commodity) waitbits|=1<<inputport->mp_SigBit;
#ifdef INPUTDEV_HOTKEY
  else waitbits|=INPUTSIG_UNICONIFY|INPUTSIG_HOTKEY;
#endif
  while(run) {
    sig=Wait(waitbits);
    command=0;
//D(bug("signal received\n"));
/*
    while ((dummymsg=(struct IntuiMessage *)GetMsg(idcmpport)))
      FreeMem(dummymsg,sizeof(struct IntuiMessage));
*/
    if (commodity) {
      while ((cxmsg=(CxMsg *)GetMsg(inputport))) {
        msgid=CxMsgID(cxmsg);
        msgtype=CxMsgType(cxmsg);
        ReplyMsg((struct Message *)cxmsg);
        switch (msgtype) {
          case CXM_IEVENT:
            if ((command=msgid)>=HOTKEY_HOTKEY) {
              x=command-HOTKEY_HOTKEY; command=HOTKEY_HOTKEY;
              hotkey=dopus_firsthotkey;
              while (x-- && hotkey) hotkey=hotkey->next;
              dopus_globalhotkey=hotkey;
            }
            break;

          case CXM_COMMAND:
            switch (msgid) {
              case CXCMD_KILL:
                command=HOTKEY_HOTKEY;
                dopus_globalhotkey=(struct dopushotkey *)-1;
                break;

              case CXCMD_DISABLE:
                ActivateCxObj(broker,0);
                break;

              case CXCMD_ENABLE:
                ActivateCxObj(broker,1);
                break;

              case CXCMD_APPEAR:
                command=HOTKEY_UNICONIFY;
                break;

              case CXCMD_DISAPPEAR:
                command=HOTKEY_HOTKEY;
                dopus_globalhotkey=(struct dopushotkey *)-2;
                break;
            }
            break;
        }
      }
    }
#ifdef INPUTDEV_HOTKEY
    else {
      if (sig&INPUTSIG_UNICONIFY)
        command=HOTKEY_UNICONIFY;
      else if (sig&INPUTSIG_HOTKEY)
        command=HOTKEY_HOTKEY;
    }
#endif
    switch (command) {
      case HOTKEY_ABORT:
        if (IntuitionBase->ActiveWindow==Window) {
          status_haveaborted=status_rexxabort=1;
          Signal((struct Task *)main_proc,INPUTSIG_ABORT);
        }
        break;

      case HOTKEY_MMB:
        if (!(config->hotkeyflags&HOTKEY_USEMMB)) break;
      case HOTKEY_UNICONIFY:
        if (status_configuring==-1) break;
        if (status_iconified==1) Signal((struct Task *)main_proc,INPUTSIG_UNICONIFY);
        else if (status_iconified==0) {
          if (MainScreen) {
            if (!status_configuring && config->screenflags&SCRFLAGS_HALFHEIGHT)
              top=main_scr.TopEdge;
            else top=0;

            if ((IntuitionBase->FirstScreen==MainScreen) &&
                (MainScreen->TopEdge==top) &&
                (!IntuitionBase->ActiveWindow || (IntuitionBase->ActiveWindow->WScreen==MainScreen)))
             {
              ScreenToBack(MainScreen);
              if (Window->Parent && Window->Parent->WScreen==IntuitionBase->FirstScreen)
                ActivateWindow(Window->Parent);
              else ActivateWindow(IntuitionBase->FirstScreen->FirstWindow);
             }
            else
             {
              struct Window *window;

              ScreenToFront(MainScreen);
              if (MainScreen->FirstWindow==ansiread_window) window=Window;
              else window=MainScreen->FirstWindow;

              if (!(window->Flags&WFLG_BACKDROP)) WindowToFront(window);
              ActivateWindow(window);
             }
            MoveScreen(MainScreen,0,top-MainScreen->TopEdge);
          }
          else {
            if (IntuitionBase->ActiveWindow==Window &&
              IntuitionBase->FirstScreen==Window->WScreen &&
              Window->WScreen->LayerInfo.top_layer==Window->RPort->Layer) {
              WindowToBack(Window);
              if (Window->Parent) ActivateWindow(Window->Parent);
            }
            else {
              ScreenToFront(Window->WScreen);
              WindowToFront(Window);
              ActivateWindow(Window);
            }
          }
        }
        break;

      case HOTKEY_HOTKEY:
        if (!status_configuring) Signal((struct Task *)main_proc,INPUTSIG_HOTKEY);
        break;
    }

    if (pwindow) {
      while ((msg=(struct IntuiMessage *)GetMsg(pwindow->UserPort))) {
        if ((class=msg->Class)==IDCMP_GADGETUP)
          gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
        ReplyMsg((struct Message *)msg);
        if (class==IDCMP_GADGETUP && gadgetid==0) {
          status_haveaborted=status_rexxabort=1;
          Signal((struct Task *)main_proc,INPUTSIG_ABORT);
        }
      }
    }

    while ((hmsg=(struct dopustaskmsg *)GetMsg(hotkeymsg_port))) {

#define BAR_ID (hmsg->flag)

      switch (hmsg->command) {
        case PROGRESS_UPDATE:
          if (pwindow) {
//D(bug("UPDATE: BAR_ID=%ld\n",BAR_ID));
            if (hmsg->value>-1)
             {
              bar[BAR_ID].curr = hmsg->value;
              bar[BAR_ID].max = hmsg->total;

              progressbar(&bar[BAR_ID]);
             }
            if (bar[BAR_ID].hide) break;
            if (BAR_ID==0 || hmsg->data)
              progresstext(bar[BAR_ID].descY,hmsg->value,hmsg->total,hmsg->data);
          }
          break;

        case PROGRESS_INCREASE:
          if (pwindow) {
            if (bar[BAR_ID].incignore) break;

//D(bug("INCREASE: BAR_ID=%ld\n",BAR_ID));
            bar[BAR_ID].curr += hmsg->value;
            if (bar[BAR_ID].curr > bar[BAR_ID].max) bar[BAR_ID].curr = bar[BAR_ID].max;

            progressbar(&bar[BAR_ID]);
            if (bar[BAR_ID].hide) break;
            progresstext(bar[BAR_ID].descY,bar[BAR_ID].curr,bar[BAR_ID].max,NULL);
          }
          break;

        case PROGRESS_OPEN:
          if (!pwindow) {
            openprogresswindow(hmsg->data,hmsg->value,hmsg->total,BAR_ID);
            if (pwindow)
             {
              SetBusyPointer(pwindow);
              waitbits |= 1<<pwindow->UserPort->mp_SigBit;
             }
          }
          break;

        case PROGRESS_CLOSE:
          if (pwindow) {
            waitbits &= ~(1<<pwindow->UserPort->mp_SigBit);
            CloseWindow(pwindow);
            pwindow=NULL;
          }
          LFreeRemember(&prog_key);
          break;

        case TASK_QUIT:
          run = 0;
          break;

        case HOTKEY_HOTKEYCHANGE:
          if (commodity) {
            set_hotkey(hotkey_filter,config->hotkeycode,config->hotkeyqual);

            if (config->hotkeyflags&HOTKEY_USEMMB) {
              if (!mmb_filter) {
                hotkey_ix.ix_Code=IECODE_MBUTTON;
                hotkey_ix.ix_Qualifier=IEQUALIFIER_MIDBUTTON;
                hotkey_ix.ix_QualMask=0xffff&~(IEQUALIFIER_RELATIVEMOUSE|IEQUALIFIER_CAPSLOCK);
                mmb_filter=set_dopus_filter(broker,inputport,"rawmouse midbutton",
                  0,0,HOTKEY_MMB,1);
              }
            }
            else if (mmb_filter) {
              DeleteCxObjAll(mmb_filter);
              mmb_filter=NULL;
            }
          }
          break;

        case HOTKEY_KILLHOTKEYS:
          if (commodity) add_hotkey_objects(broker,inputport,0);
          break;

        case HOTKEY_NEWHOTKEYS:
          if (commodity) add_hotkey_objects(broker,inputport,1);
          break;
      }
      ReplyMsg((struct Message *)hmsg);
    }
  }

 if (commodity) {
D(bug("Removing commodity objects\n"));
   add_hotkey_objects(broker,inputport,0);
   DeleteCxObjAll(broker);
   while ((cxmsg=(CxMsg *)GetMsg(inputport)))
     ReplyMsg((struct Message *)cxmsg);
 }
#ifdef INPUTDEV_HOTKEY
 else {
   inreq->io_Data=(APTR)&hotkey_interrupt;
   inreq->io_Command=IND_REMHANDLER;
   DoIO((struct IORequest *)inreq);
   CloseDevice((struct IORequest *)inreq);
   LDeleteExtIO((struct IORequest *)inreq);
 }
#endif
 if (pwindow) CloseWindow(pwindow);
 LFreeRemember(&prog_key);
 LDeletePort(inputport);
// ReplyMsg((struct Message *)hmsg);
 LDeletePort(hotkeymsg_port);
// LDeletePort(idcmpport);
 Wait(0);
}

void add_hotkey_objects(broker,port,add)
CxObj *broker;
struct MsgPort *port;
int add;
{
  static CxObj **filter_table;
  static int hotkey_count;
  struct dopushotkey *hotkey;
  int a;

  if (filter_table) {
    for (a = 0; a < hotkey_count; a++) DeleteCxObjAll(filter_table[a]);
    FreeMem(filter_table,hotkey_count*sizeof(CxObj *));
    filter_table = NULL;
  }

  if (add == 0) return;

  for (hotkey = dopus_firsthotkey, hotkey_count = 0; hotkey; hotkey = hotkey->next)
    hotkey_count++;
  if (hotkey_count==0) return;

  filter_table = AllocMem(hotkey_count*sizeof(CxObj *),MEMF_CLEAR);
  if (filter_table == NULL) return;

  for (a = 0, hotkey = dopus_firsthotkey; hotkey; hotkey = hotkey->next, a++)
    filter_table[a] = set_dopus_filter(broker,port,NULL,
        hotkey->code,hotkey->qualifier,
        HOTKEY_HOTKEY+a,1);
}

CxObj *set_dopus_filter(broker,port,string,code,qual,command,translate)
CxObj *broker;
struct MsgPort *port;
char *string;
UWORD code,qual;
int command;
int translate;
{
  CxObj *filter;

  if ((filter=CxFilter(string))) {
    if (!string) {
      set_hotkey(filter,code,qual);
/*
      hotkey_ix.ix_Class=IECLASS_RAWKEY;
      if (code==(UWORD)~0) {
        hotkey_ix.ix_Code=0;
        hotkey_ix.ix_CodeMask=0;
      }
      else {
        hotkey_ix.ix_Code=code;
        hotkey_ix.ix_CodeMask=0xff;
      }
      hotkey_ix.ix_Qualifier=qual&VALID_QUALIFIERS;
      hotkey_ix.ix_QualMask=VALID_QUALIFIERS;
*/
    }
    else {
      hotkey_ix.ix_Class=IECLASS_RAWMOUSE;
      hotkey_ix.ix_CodeMask=0xff;
      hotkey_ix.ix_QualSame=0;
      SetFilterIX(filter,&hotkey_ix);
    }
    AttachCxObj(broker,filter);
    AttachCxObj(filter,CxSender(port,command));
    if (translate) AttachCxObj(filter,CxTranslate(NULL));

    return(filter);
  }
  return(NULL);
}

void set_hotkey(filter,code,qual)
CxObj *filter;
UWORD code,qual;
{
if (filter) {
    hotkey_ix.ix_Class=IECLASS_RAWKEY;
    if (qual==0 && code==(UWORD)~0) {
      hotkey_ix.ix_Code=0xffff;
      hotkey_ix.ix_CodeMask=0xffff;
//      hotkey_ix.ix_Qualifier=0;
      hotkey_ix.ix_Qualifier=0xffff;
    }
    else {
      if (code==(UWORD)~0) {
        hotkey_ix.ix_Code=0;
        hotkey_ix.ix_CodeMask=0;
      }
      else {
        hotkey_ix.ix_Code=code;
        hotkey_ix.ix_CodeMask=0xff;
      }
      hotkey_ix.ix_Qualifier=qual&VALID_QUALIFIERS;
      hotkey_ix.ix_QualMask=VALID_QUALIFIERS;
    }
    SetFilterIX(filter,&hotkey_ix);
  }
}

void openprogresswindow(title,value,total,flag)
char *title;
int value,total,flag;
{
  struct TextFont *font;
  char *gadtxt[] = { globstring[STR_ABORT], NULL };
  int a,incignore = 0;

  if (flag & 0x80)
   {
    incignore = 1;
    flag &= ~0x80;
D(bug("PROGRESS_INCREASE commands will be ignored\n"));
   }

  if (config->generalscreenflags&SCR_GENERAL_REQDRAG) {
    prog_xoff=Window->WScreen->WBorLeft+2;
    prog_yoff=Window->WScreen->WBorTop+Window->WScreen->Font->ta_YSize+2;
    prog_xextra=prog_xoff+Window->WScreen->WBorRight-2;
  }
  else {
    prog_xoff=2;
    prog_yoff=1;
    prog_xextra=0;
  }

  progresswindow.Width=384+(scr_font[FONT_REQUEST]->tf_XSize*8)+prog_xextra;

  if (progresswindow.Width>Window->WScreen->Width) {
    font=scr_font[FONT_GENERAL];
    progresswindow.Width=432;
    if (config->generalscreenflags&SCR_GENERAL_REQDRAG)
      progresswindow.Width+=prog_xextra;
  }
  else font=scr_font[FONT_REQUEST];

  progresswindow.Height=(font->tf_YSize*5)+22;
  if (flag) {
    progresswindow.Height+=(font->tf_YSize*2)+8;
    prog_yextra=(font->tf_YSize*2)+8;
  }
  else prog_yextra=0;

  if (config->generalscreenflags&SCR_GENERAL_REQDRAG) {
    progresswindow.Height+=prog_yoff+Window->WScreen->WBorBottom-1;
    progresswindow.Flags|=WFLG_DRAGBAR|WFLG_DEPTHGADGET;
    progresswindow.Title=title;
  }
  else {
    progresswindow.Flags|=WFLG_BORDERLESS;
    progresswindow.Title=NULL;
  }

  centerwindow(&progresswindow);
  if (!(pwindow=OpenWindow(&progresswindow))) return;
  prp=pwindow->RPort;
  setupwindreq(pwindow);
  SetFont(prp,font);

  Do3DBox(prp,
    26+prog_xoff,
    6+prog_yoff,
    pwindow->Width-prog_xextra-56,
    (font->tf_YSize*4)+prog_yextra,
    screen_pens[config->gadgetbotcol].pen,
    screen_pens[config->gadgettopcol].pen);
  SetAPen(prp,screen_pens[config->requestfg].pen);

  abortopgad.LeftEdge=(pwindow->Width-abortopgad.Width)/2;
  abortopgad.Height=font->tf_YSize+4;
  abortopgad.TopEdge=pwindow->Height-pwindow->BorderBottom-9-font->tf_YSize;
  AddGadgetBorders(&prog_key,
    &abortopgad,1,
    screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
  AddGadgets(pwindow,
    &abortopgad,
    gadtxt,
    1,
    screen_pens[config->gadgettopcol].pen,
    screen_pens[config->gadgetbotcol].pen,1);

  SetAPen(prp,screen_pens[0].pen);
  SetDrMd(prp,JAM2);
  RectFill(prp,
    26+prog_xoff,
    6+prog_yoff,
    (prog_areax=prog_xoff+pwindow->Width-prog_xextra-31),
    prog_yoff+(font->tf_YSize*4)+prog_yextra+5);

  for (a=0;a<2;a++) {
    bar[a].last_w = 0;
    bar[a].curr = value;
    bar[a].max = total;
    bar[a].incignore = incignore;
    bar[a].hide = (a == 0) ? 0 : !flag;
    bar[a].barX=(((pwindow->Width-356)/2)+28)-font->tf_XSize;
    bar[a].barY=font->tf_YSize+3+prog_yoff;
    if (a==0) bar[a].barY += prog_yextra+3;

    if (! bar[a].hide)
     {
      Do3DBox(prp,
        bar[a].barX,bar[a].barY,
        300,font->tf_YSize,
        screen_pens[config->gadgetbotcol].pen,
        screen_pens[config->gadgettopcol].pen);

      bar[a].descY=bar[a].barY+font->tf_YSize+(font->tf_YSize/2)+font->tf_Baseline;

      SetAPen(prp,screen_pens[1].pen);
      SetBPen(prp,screen_pens[0].pen);
      Move(prp,bar[a].barX-(TextLength(prp,"0% ",3))-4,bar[a].barY+font->tf_Baseline);
      Text(prp,"0%",2);
      Move(prp,bar[a].barX+302+font->tf_XSize,bar[a].barY+font->tf_Baseline);
      Text(prp,"100%",4);

      if (a==0) progresstext(bar[a].descY,value,total,NULL);
      progressbar(&bar[a]);
     }
  }
}

void progresstext(y,val,total,text)
int y,val,total;
char *text;
{
  char buf[80],*ptr;
  int x,y1,len;

D(bug("progresstext(Y=%ld,V=%ld,Vmax=%ld,%s)\n",y,val,total,text?text:"<NULL>"));
  if (val==-1) ptr = globstring[total?STR_ABORTED:STR_COMPLETED];
  else {
    if (text) LStrnCpy(buf,text,(pwindow->Width-prog_xextra-56)/prp->Font->tf_XSize);
    else lsprintf(buf,globstring[STR_REMAINING],val,total);
    ptr=buf;
  }
  x=26+((pwindow->Width-prog_xextra-56-TextLength(prp,ptr,(len=strlen(ptr))))/2)+prog_xoff;
  y1=y-prp->Font->tf_Baseline;

  if (x>prog_xoff+26) {
    SetAPen(prp,screen_pens[0].pen);
    RectFill(prp,
      prog_xoff+26,
      y1,
      x-1,
      y1+prp->Font->tf_YSize);
  }

  SetAPen(prp,screen_pens[1].pen);
  Move(prp,x,y);
  Text(prp,ptr,len);

  if (prp->cp_x<=prog_areax) {
    SetAPen(prp,screen_pens[0].pen);
    RectFill(prp,
      prp->cp_x,
      y1,
      prog_areax-1,
      y1+prp->Font->tf_YSize);
  }
D(bug("progresstext() ends here\n"));
}

void progressbar(struct ProgressBar *bar)
{
  int w;
  BOOL draw;

  if (bar->hide) return;
D(bug("progressbar(Y=%ld,V=%ld,Vmax=%ld)\n",bar->barY,bar->curr,bar->max));
  if (bar->curr>0) {
    float f=(float)(bar->curr)/(float)(bar->max);

    if ((w=(int)(300*f))>300) w=300;
    else if (w<1) w=1;
    draw = (w != bar->last_w);
    SetAPen(prp,screen_pens[3].pen);
  }
  else {
    draw=TRUE;
    w=300;
    SetAPen(prp,screen_pens[0].pen);
  }
  if (draw)
   {
    RectFill(prp,bar->barX,bar->barY,bar->barX+w-1,bar->barY+prp->Font->tf_YSize-1);
    bar->last_w = w;
   }
D(bug("progressbar() ends here\n"));
}

#ifdef INPUTDEV_HOTKEY
#ifdef __MORPHOS__
struct InputEvent *keyhandler(void)
 {
  struct InputEvent *oldevent = (struct InputEvent *)REG_A0;
//  APTR userdata = (APTR)REG_A1;
#else
#ifdef __AROS__
AROS_UFH2(struct InputEvent *, keyhandler,
    AROS_UFHA(struct InputEvent *, oldevent, A0),
    AROS_UFHA(APTR, userdata, A1)
)
{
    AROS_USERFUNC_INIT   
#else
struct InputEvent * __saveds keyhandler(register struct InputEvent *oldevent __asm("a0"),register APTR userdata __asm("a1"))
{
#endif
#endif

  int wakeup=0;
  struct dopushotkey *hotkey;

  if (oldevent->ie_Class==IECLASS_RAWKEY) {
    if ((config->hotkeycode==(UWORD)~0 || oldevent->ie_Code==config->hotkeycode) &&
      (!config->hotkeyqual || (oldevent->ie_Qualifier&VALID_QUALIFIERS)==config->hotkeyqual))
      wakeup=1;
    else {
      hotkey=dopus_firsthotkey;
      while (hotkey) {
        if (oldevent->ie_Code==hotkey->code &&
          (oldevent->ie_Qualifier&VALID_QUALIFIERS)==hotkey->qualifier) {
          dopus_globalhotkey=hotkey;
          Signal(hotkey_task,INPUTSIG_HOTKEY);
          oldevent->ie_Class=IECLASS_NULL;
          break;
        }
        hotkey=hotkey->next;
      }
    }
  }
  else if (oldevent->ie_Class==IECLASS_RAWMOUSE) {
    if ((oldevent->ie_Code&~IECODE_UP_PREFIX)==IECODE_LBUTTON) {
      if (oldevent->ie_Qualifier&IEQUALIFIER_RBUTTON) wakeup=2;
    }
    else if (oldevent->ie_Code==IECODE_RBUTTON &&
      oldevent->ie_Qualifier&IEQUALIFIER_LEFTBUTTON) wakeup=2;
    else if (config->hotkeyflags&HOTKEY_USEMMB &&
      oldevent->ie_Code==IECODE_MBUTTON) wakeup=1;
  }
  if (wakeup) {
    if (wakeup==1 && hotkey_task) {
      Signal(hotkey_task,INPUTSIG_UNICONIFY);
      oldevent->ie_Class=IECLASS_NULL;
    }
    else if (wakeup==2) {
      if (IntuitionBase->ActiveWindow==Window) {
        status_haveaborted=status_rexxabort=1;
        Signal((struct Task *)main_proc,INPUTSIG_ABORT);
      }
    }
  }
  return(oldevent);

#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
#endif

static char *Kstr = "K  ";

void __saveds clocktask()
{
  ULONG chipc,fast,wmes,h,m,s,/*secs,micro,*/cx,sig,cy,len,ct,chipnum,fastnum,a,active=1,usage;
  UWORD clock_width,clock_height,scr_height;
  char buf[160],date[20],time[20],formstring[160],memstring[160],ampm;
  struct MsgPort *clock_time_port;
  struct timerequest ctimereq;
  struct DOpusDateTime datetime = {0};
  struct dopustaskmsg *cmsg;
  struct RastPort clock_rp;
#ifndef __AROS__    
  struct SI_CpuUsage sicpu;
#endif

  Forbid();
  CopyMem((char *)main_rp,(char *)&clock_rp,sizeof(struct RastPort));
  SetDrawModes(&clock_rp,config->clockfg,config->clockbg,JAM2);
  SetFont(&clock_rp,scr_font[FONT_CLOCK]);
  scr_height=scrdata_height+scrdata_yoffset;
  clock_width=scrdata_clock_width;
  clock_height=scrdata_clock_height;
  ct=scr_height-(clock_height-1);
  cy=scrdata_clock_ypos+scr_font[FONT_CLOCK]->tf_Baseline-1;
  Permit();

  clockmsg_port=LCreatePort(NULL,0);
  clock_time_port=LCreatePort(0,0);

  OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)&ctimereq,0);
  ctimereq.tr_node.io_Message.mn_ReplyPort=clock_time_port;
  ctimereq.tr_node.io_Command=TR_ADDREQUEST;
  ctimereq.tr_node.io_Flags=0;
  ctimereq.tr_time.tv_secs=0;
  ctimereq.tr_time.tv_micro=2;
  SendIO(&ctimereq.tr_node);

  chipnum=getmaxmem(MEMF_CHIP/*,MEMF_ANY*/);
  fastnum=getmaxmem(MEMF_FAST/*ANY,MEMF_CHIP*/);
  a=getmaxmem(MEMF_ANY/*,MEMF_ANY*/);

  m = (config->scrclktype&SCRCLOCK_BYTES)?3:0;
  s = (config->scrclktype&SCRCLOCK_BYTES)?1:0;

  if (config->scrclktype&SCRCLOCK_C_AND_F) {
    lsprintf(memstring,"%lc:%%-%ldld%s",globstring[STR_CLOCK_CHIP][0],chipnum+m,Kstr+s);
    if (fastnum>1) {
      lsprintf(memstring+strlen(memstring),"%lc:%%-%ldld%s",globstring[STR_CLOCK_FAST][0],fastnum+m,Kstr+s);
      lsprintf(memstring+strlen(memstring),"%lc:%%-%ldld%s",globstring[STR_CLOCK_TOTAL][0],a+m,Kstr+s);
    }
  }
  else {
    lsprintf(memstring,"%s%%-%ldld%s",globstring[STR_CLOCK_CHIP],chipnum+m,Kstr+s);
    if (fastnum>1) {
      lsprintf(memstring+strlen(memstring),"%s%%-%ldld%s",globstring[STR_CLOCK_FAST],fastnum+m,Kstr+s);
      lsprintf(memstring+strlen(memstring),"%s%%-%ldld%s",globstring[STR_CLOCK_TOTAL],a+m,Kstr+s);
    }
  }

  if (!(config->scrclktype&(SCRCLOCK_MEMORY|SCRCLOCK_CPU|SCRCLOCK_DATE|SCRCLOCK_TIME)))
    lsprintf(formstring,"Directory Opus  Version %s  Compiled %s  %s",
      str_version_string,comp_time,comp_date);

  sig = 1<<clock_time_port->mp_SigBit | 1<<clockmsg_port->mp_SigBit;

  FOREVER {
    wmes=Wait(sig);
    if (wmes&1<<clockmsg_port->mp_SigBit) {
      while ((cmsg=(struct dopustaskmsg *)GetMsg(clockmsg_port))) {
        switch (cmsg->command) {
          case TASK_QUIT:
            if (!(CheckIO(&ctimereq.tr_node)))
              AbortIO(&ctimereq.tr_node);
            WaitIO(&ctimereq.tr_node);
            CloseDevice((struct IORequest *)&ctimereq);
            LDeletePort(clock_time_port);
            LDeletePort(clockmsg_port); clockmsg_port=NULL;
            ReplyMsg((struct Message *)cmsg);
//            Wait(0);
            Forbid();
            return;
          case CLOCK_ACTIVE:
            active=cmsg->value;
            break;
        }
        ReplyMsg((struct Message *)cmsg);
      }
    }
    if (wmes&1<<clock_time_port->mp_SigBit) {
//    if (CheckIO((struct IORequest *)&ctimereq.tr_node)) {
//      WaitIO((struct IORequest *)&ctimereq.tr_node);
      if (active && !(Window->Flags&WFLG_MENUSTATE)) {
        if (scr_height>ct+1) {
          if (config->scrclktype&(SCRCLOCK_MEMORY|SCRCLOCK_CPU|SCRCLOCK_DATE|SCRCLOCK_TIME)) {
            formstring[0]=0;
            if (config->scrclktype&SCRCLOCK_MEMORY) {
              chipc=AvailMem(MEMF_CHIP);
              fast=AvailMem(MEMF_FAST/*ANY*/)/*-chipc*/;
              if (!(config->scrclktype&SCRCLOCK_BYTES))
               {
                chipc/=1024;
                fast/=1024;
               }
              /*if (fastnum>1)*/ lsprintf(buf,memstring,chipc,fast,chipc+fast);
  //            else lsprintf(buf,memstring,chipc);
              strcat(formstring,buf);
            }
            if (config->scrclktype&SCRCLOCK_CPU) {
              if (sysinfo)
               {
#ifndef __AROS__
                GetCpuUsage(sysinfo,&sicpu);
                usage = 100 * sicpu.used_cputime_lastsec / sicpu.used_cputime_lastsec_hz;
#endif
               }
              else usage = getusage()/*/10*/;

              lsprintf(buf,"CPU:%3ld%%  ",(long int)usage);
              strcat(formstring,buf);
            }
            if (config->scrclktype&(SCRCLOCK_DATE|SCRCLOCK_TIME)) {
              DateStamp(&(datetime.dat_Stamp));
              initdatetime(&datetime,date,time,0);

              if (config->scrclktype&SCRCLOCK_DATE) {
                lsprintf(buf,"%-9s  ",date);
                strcat(formstring,buf);
              }
              if (config->scrclktype&SCRCLOCK_TIME) {
                if (config->dateformat&DATE_12HOUR) {
                  h=datetime.dat_Stamp.ds_Minute/60; m=datetime.dat_Stamp.ds_Minute%60;
                  s=datetime.dat_Stamp.ds_Tick/TICKS_PER_SECOND;
                  if (h>11) { ampm='P'; h-=12; }
                  else ampm='A';
                  if (h==0) h=12;
                  lsprintf(time,"%2ld:%02ld:%02ld%lc",(long int)h,(long int)m,(long int)s,ampm);
                }
                strcat(formstring,time);
              }
            }
          }
          len=strlen(formstring);
          if (len>1 && formstring[len-2]==' ') len-=2;
          cx=(clock_width-dotextlength(&clock_rp,formstring,(int *)&len,clock_width-4))/2;
          cx+=scrdata_clock_xpos;
          if (cx<scrdata_clock_xpos) cx=scrdata_clock_xpos;

          SetAPen(&clock_rp,screen_pens[config->clockfg].pen);
          Move(&clock_rp,cx,cy);
          Text(&clock_rp,formstring,len);
          SetAPen(&clock_rp,screen_pens[config->clockbg].pen);

          if (cx>scrdata_clock_xpos)
            RectFill(&clock_rp,scrdata_clock_xpos,ct,cx-1,scr_height-2);
          if (clock_rp.cp_x<clock_width-1)
            RectFill(&clock_rp,clock_rp.cp_x,ct,clock_width-2,scr_height-2);
        }
      }
      ctimereq.tr_time.tv_secs=1;
      ctimereq.tr_time.tv_micro=0;
      SendIO(&ctimereq.tr_node);
    }
  }
}
