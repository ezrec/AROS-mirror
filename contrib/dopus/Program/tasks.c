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
#include "ppdata.h"

#include <proto/commodities.h>


AROS_UFP2(struct InputEvent *, keyhandler,
    AROS_UFPA(struct InputEvent *,      oldevent,       A0),
    AROS_UFPA(APTR,         userdata,        A1)
);


#define HOTKEY_UNICONIFY   1
#define HOTKEY_ABORT       3
#define HOTKEY_MMB         5
#define HOTKEY_HOTKEY      10

struct Gadget
	abortopgad={
		NULL,0,0,104,0,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,NULL,NULL,0,NULL};

static struct NewWindow
	progresswindow={
		0,0,0,0,255,255,
		IDCMP_GADGETUP|IDCMP_RAWKEY,WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_ACTIVATE,
		NULL,NULL,NULL,NULL,NULL,0,0,0,0,CUSTOMSCREEN};

static struct Window *pwindow;
static struct RastPort *prp;
static struct DOpusRemember *prog_key;
static int prog_barx[2],prog_bary[2],prog_texty[2];
static int prog_xoff,prog_yoff,prog_xextra,prog_yextra,prog_areax;


static struct Interrupt
	hotkey_interrupt={
	
/* AROS: The struct Node is a little different */
#if 0	
		{NULL,NULL,2,52,"hotkeez_port"},
#else
		{NULL,NULL,"hotkeez_port",2,52},
#endif
		NULL,(VOID *)keyhandler};

static struct NewBroker
	hotkey_broker={
		NB_VERSION,
		"Directory Opus",
		"Directory Opus © Jonathan Potter",
		"The most amazing program ever written",
		0,COF_SHOW_HIDE,100,NULL,0};

static IX hotkey_ix={IX_VERSION};

void /* __saveds */ hotkeytaskcode()
{
	int top,sig,waitbits,commodity=0,command,x;
	struct dopustaskmsg *hmsg;
	struct IOStdReq *inreq;
	struct MsgPort *inputport,*idcmpport;
	struct IntuiMessage *msg;
	ULONG class,msgid,msgtype;
	USHORT gadgetid,norm_height,norm_width;
	struct IntuiMessage *dummymsg;
	struct dopushotkey *hotkey;
	CxObj *broker,*hotkey_filter,*mmb_filter=NULL;
	CxMsg *cxmsg;

	norm_height=scrdata_norm_height;
	norm_width=scrdata_norm_width;

	hotkeymsg_port=LCreatePort(NULL,0);
	inputport=LCreatePort(NULL,0);
	idcmpport=LCreatePort(NULL,0);

	if (CxBase && !(status_flags&STATUS_IANSCRAP)) {
		hotkey_broker.nb_Port=inputport;
		if ((broker=CxBroker(&hotkey_broker,NULL))) {

			/* Initialise main hotkey */
			if ((hotkey_filter=set_dopus_filter(
				broker,
				inputport,
				NULL,
				config->hotkeycode,
				config->hotkeyqual,
				HOTKEY_UNICONIFY,1))) {

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
		if (!commodity) {
			add_hotkey_objects(broker,inputport,0);
			DeleteCxObjAll(broker);
		}
	}

	if (!commodity) {
		inreq=(struct IOStdReq *)LCreateExtIO(inputport,sizeof(struct IOStdReq));
		OpenDevice("input.device",NULL,(struct IORequest *)inreq,NULL);
		inreq->io_Data=(APTR)&hotkey_interrupt;
		inreq->io_Command=IND_ADDHANDLER;
		DoIO((struct IORequest *)inreq);
	}

	FOREVER {
		waitbits=1<<hotkeymsg_port->mp_SigBit|1<<idcmpport->mp_SigBit;
		if (commodity) waitbits|=1<<inputport->mp_SigBit;
		else waitbits|=INPUTSIG_UNICONIFY|INPUTSIG_HOTKEY;
		if (pwindow) waitbits|=1<<pwindow->UserPort->mp_SigBit;

		sig=Wait(waitbits);
		command=0;

		while ((dummymsg=(struct IntuiMessage *)GetMsg(idcmpport)))
			FreeMem(dummymsg,sizeof(struct IntuiMessage));

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
		else {
			if (sig&INPUTSIG_UNICONIFY)
				command=HOTKEY_UNICONIFY;
			else if (sig&INPUTSIG_HOTKEY)
				command=HOTKEY_HOTKEY;
		}

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

						if (IntuitionBase->FirstScreen==MainScreen &&
							MainScreen->TopEdge==top &&
							(!IntuitionBase->ActiveWindow ||
								IntuitionBase->ActiveWindow->WScreen==MainScreen)) {
							ScreenToBack(MainScreen);
							if (Window->Parent && Window->Parent->WScreen==IntuitionBase->FirstScreen)
								ActivateWindow(Window->Parent);
							else ActivateWindow(IntuitionBase->FirstScreen->FirstWindow);
						}
						else {
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
				if (status_configuring) continue;
				Signal((struct Task *)main_proc,INPUTSIG_HOTKEY);
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

		while ((hmsg=(struct dopustaskmsg *)GetMsg(hotkeymsg_port))){
			switch (hmsg->command) {
				case TASK_QUIT:
					if (commodity) {
						add_hotkey_objects(broker,inputport,0);
						DeleteCxObjAll(broker);
						while ((cxmsg=(CxMsg *)GetMsg(inputport)))
							ReplyMsg((struct Message *)cxmsg);
					}
					else {
						inreq->io_Data=(APTR)&hotkey_interrupt;
						inreq->io_Command=IND_REMHANDLER;
						DoIO((struct IORequest *)inreq);
						CloseDevice((struct IORequest *)inreq);
						LDeleteExtIO((struct IORequest *)inreq);
					}
					LDeletePort(inputport);
					ReplyMsg((struct Message *)hmsg);
					LDeletePort(hotkeymsg_port);
					LDeletePort(idcmpport);
					if (pwindow) CloseWindow(pwindow);
					LFreeRemember(&prog_key);
					Wait(0);

				case PROGRESS_OPEN:
					if (!pwindow) {
						openprogresswindow(hmsg->data,hmsg->value,hmsg->total,hmsg->flag);
						if (pwindow) SetBusyPointer(pwindow);
					}
					break;

				case PROGRESS_CLOSE:
					if (pwindow) {
						CloseWindow(pwindow);
						pwindow=NULL;
					}
					LFreeRemember(&prog_key);
					break;

				case PROGRESS_UPDATE:
					if (pwindow) {
						if (hmsg->flag==0 || prog_yextra) {
							if (hmsg->value>-1)
								progressbar(prog_barx[hmsg->flag],prog_bary[hmsg->flag],hmsg->value,hmsg->total);
							if (!hmsg->flag || hmsg->data)
								progresstext(prog_texty[hmsg->flag],hmsg->value,hmsg->total,hmsg->data);
						}
					}
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
					if (commodity)
						add_hotkey_objects(broker,inputport,0);
					break;

				case HOTKEY_NEWHOTKEYS:
					if (commodity)
						add_hotkey_objects(broker,inputport,1);
					break;
			}
			ReplyMsg((struct Message *)hmsg);
		}
	}
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
		for (a=0;a<hotkey_count;a++)
			DeleteCxObjAll(filter_table[a]);
		FreeMem(filter_table,hotkey_count*sizeof(CxObj *));
		filter_table=NULL;
	}
	
	hotkey=dopus_firsthotkey;
	hotkey_count=0;
	while (hotkey) {
		++hotkey_count;
		hotkey=hotkey->next;
	}

	if (!add || !hotkey_count ||
		!(filter_table=AllocMem(hotkey_count*sizeof(CxObj *),MEMF_CLEAR))) return;

	hotkey=dopus_firsthotkey;
	a=0;
	while (hotkey) {
		filter_table[a]=
			set_dopus_filter(broker,port,NULL,
				hotkey->code,hotkey->qualifier,
				HOTKEY_HOTKEY+a,1);
		++a;
		hotkey=hotkey->next;
	}
}

CxObj *set_dopus_filter(broker,port,string,code,qual,command,translate)
CxObj *broker;
struct MsgPort *port;
char *string;
USHORT code,qual;
int command;
int translate;
{
	CxObj *filter,*cxobj;

	if ((filter=CxFilter(string))) {
		if (!string) {
			hotkey_ix.ix_Class=IECLASS_RAWKEY;
			if (code==(USHORT)~0) {
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
		else {
			hotkey_ix.ix_Class=IECLASS_RAWMOUSE;
			hotkey_ix.ix_CodeMask=0xff;
			hotkey_ix.ix_QualSame=0;
		}
		SetFilterIX(filter,&hotkey_ix);
		AttachCxObj(broker,filter);
		if ((cxobj=CxSender(port,command)))
			AttachCxObj(filter,cxobj);
		if (translate && (cxobj=CxTranslate(NULL)))
			AttachCxObj(filter,cxobj);
		return(filter);
	}
	return(NULL);
}

void set_hotkey(filter,code,qual)
CxObj *filter;
USHORT code,qual;
{
	if (filter) {
		hotkey_ix.ix_Class=IECLASS_RAWKEY;
		if (qual==0 && code==(USHORT)~0) {
			hotkey_ix.ix_Code=0xffff;
			hotkey_ix.ix_CodeMask=0xffff;
			hotkey_ix.ix_Qualifier=0;
			hotkey_ix.ix_Qualifier=0xffff;
		}
		else {
			if (code==(USHORT)~0) {
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
	char *gadtxt[2];
	int a;

	gadtxt[0]=globstring[STR_ABORT];
	gadtxt[1]=NULL;

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
		progresswindow.Flags=WFLG_RMBTRAP|WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_DEPTHGADGET;
		progresswindow.Title=title;
	}
	else {
		progresswindow.Flags=WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_ACTIVATE;
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
		screen_pens[(int)config->gadgetbotcol].pen,
		screen_pens[(int)config->gadgettopcol].pen);
	SetAPen(prp,screen_pens[(int)config->requestfg].pen);

	abortopgad.LeftEdge=(pwindow->Width-abortopgad.Width)/2;
	abortopgad.Height=font->tf_YSize+4;
	abortopgad.TopEdge=pwindow->Height-pwindow->BorderBottom-9-font->tf_YSize;
	AddGadgetBorders(&prog_key,
		&abortopgad,1,
		screen_pens[(int)config->gadgettopcol].pen,screen_pens[(int)config->gadgetbotcol].pen);
	AddGadgets(pwindow,
		&abortopgad,
		gadtxt,
		1,
		screen_pens[(int)config->gadgettopcol].pen,
		screen_pens[(int)config->gadgetbotcol].pen,1);

	SetAPen(prp,screen_pens[0].pen);
	SetDrMd(prp,JAM2);
	RectFill(prp,
		26+prog_xoff,
		6+prog_yoff,
		(prog_areax=prog_xoff+pwindow->Width-prog_xextra-31),
		prog_yoff+(font->tf_YSize*4)+prog_yextra+5);

	for (a=0;a<2;a++) {
		prog_barx[a]=(((pwindow->Width-356)/2)+28)-font->tf_XSize;
		prog_bary[a]=font->tf_YSize+3+prog_yoff;
		if (a==0) prog_bary[a]+=prog_yextra+3;

		Do3DBox(prp,
			prog_barx[a],prog_bary[a],
			300,font->tf_YSize,
			screen_pens[(int)config->gadgetbotcol].pen,
			screen_pens[(int)config->gadgettopcol].pen);
		SetBPen(prp,screen_pens[0].pen);

		prog_texty[a]=prog_bary[a]+font->tf_YSize+(font->tf_YSize/2)+font->tf_Baseline;
		if (a==0) progresstext(prog_texty[a],value,total,NULL);
		SetAPen(prp,screen_pens[1].pen);
		Move(prp,prog_barx[a]-(TextLength(prp,"0% ",3))-4,prog_bary[a]+font->tf_Baseline);
		Text(prp,"0%",2);
		Move(prp,prog_barx[a]+302+font->tf_XSize,prog_bary[a]+font->tf_Baseline);
		Text(prp,"100%",4);
		progressbar(prog_barx[a],prog_bary[a],value,total);
		if (!flag) break;
	}
}

void progresstext(y,val,total,text)
int y,val,total;
char *text;
{
	char buf[40],*ptr;
	int x,y1,len;

	if (val==-1) {
		if (total) ptr=globstring[STR_ABORTED];
		else ptr=globstring[STR_COMPLETED];
	}
	else {
		if (text) ptr=text;
		else {
			lsprintf(buf,globstring[STR_REMAINING],val,total);
			ptr=buf;
		}
	}
	x=27+((pwindow->Width-prog_xextra-56-TextLength(prp,ptr,(len=strlen(ptr))))/2)+prog_xoff;
	y1=y-prp->Font->tf_Baseline;

	if (x>prog_xoff+32) {
		SetAPen(prp,screen_pens[0].pen);
		RectFill(prp,
			prog_xoff+32,
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
			prog_areax,
			y1+prp->Font->tf_YSize);
	}
}

void progressbar(x,y,val,total)
int x,y,val,total;
{
	int w;
	float f;

	f=(float)val/(float)total;
	if (f>0) {
		if ((w=(int)(300*f))>300) w=300;
		else if (w<1) w=1;
		SetAPen(prp,screen_pens[3].pen);
	}
	else {
		w=300;
		SetAPen(prp,screen_pens[0].pen);
	}
	RectFill(prp,x,y,x+w-1,y+prp->Font->tf_YSize-1);
}

AROS_UFH2(struct InputEvent *, keyhandler,
    AROS_UFHA(struct InputEvent *,      oldevent,       A0),
    AROS_UFHA(APTR,         userdata,        A1)
)
{
    AROS_USERFUNC_INIT

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

    AROS_USERFUNC_EXIT
}

void /* __saveds */ clocktask()
{
	int chipc,fast,wmes,h,m,s,secs,micro,cx,sig,cy,len,ct,chipnum,fastnum,a,active=1;
	USHORT clock_width,clock_height,scr_height;
	char buf[160],date[20],time[20],formstring[160],memstring[160],ampm;
	struct MsgPort *clock_time_port;
	struct timerequest ctimereq;
	struct DOpusDateTime datetime;
	struct dopustaskmsg *cmsg;
	struct RastPort clock_rp;

	clockmsg_port=LCreatePort(NULL,0);
	Forbid();
	CopyMem((char *)main_rp,(char *)&clock_rp,sizeof(struct RastPort));
	SetDrawModes(&clock_rp,config->clockfg,config->clockbg,JAM2);
	SetFont(&clock_rp,scr_font[FONT_CLOCK]);
	scr_height=scrdata_height+scrdata_yoffset;
	clock_width=scrdata_clock_width;
	clock_height=scrdata_clock_height;
	Permit();

	clock_time_port=LCreatePort(0,0);
	OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)&ctimereq,0);
	ctimereq.tr_node.io_Message.mn_ReplyPort=clock_time_port;
	ctimereq.tr_node.io_Command=TR_ADDREQUEST;
	ctimereq.tr_node.io_Flags=0;
	ctimereq.tr_time.tv_secs=0;
	ctimereq.tr_time.tv_micro=2;
	SendIO((struct IORequest *)&ctimereq.tr_node);
	sig=1<<clock_time_port->mp_SigBit|1<<clockmsg_port->mp_SigBit;
	cy=scrdata_clock_ypos+scr_font[FONT_CLOCK]->tf_Baseline-1;
	ct=scr_height-(clock_height-1);
	chipnum=getmaxmem(MEMF_CHIP,MEMF_ANY);
	fastnum=getmaxmem(MEMF_ANY,MEMF_CHIP);
	a=getmaxmem(MEMF_ANY,MEMF_ANY);
	if (fastnum>1) {
		if (config->scrclktype&SCRCLOCK_C_AND_F) {
			lsprintf(memstring,"%lc:%%-%ldld%s %lc:%%-%ldld%s %lc:%%-%ldld%lc",
				globstring[STR_CLOCK_CHIP][0],
				chipnum+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?" ":"K ",
				globstring[STR_CLOCK_FAST][0],
				fastnum+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?" ":"K ",
				globstring[STR_CLOCK_TOTAL][0],
				a+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?0:'K');
		}
		else {
			lsprintf(memstring,"%s%%-%ldld%s %s%%-%ldld%s %s%%-%ldld%lc",
				globstring[STR_CLOCK_CHIP],
				chipnum+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?" ":"K ",
				globstring[STR_CLOCK_FAST],
				fastnum+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?" ":"K ",
				globstring[STR_CLOCK_TOTAL],
				a+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?0:'K');
		}
	}
	else {
		if (config->scrclktype&SCRCLOCK_C_AND_F) {
			lsprintf(memstring,"%lc:%%-%ldld%lc",
				globstring[STR_CLOCK_MEMORY][0],
				chipnum+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?0:'K');
		}
		else {
			lsprintf(memstring,"%s%%-%ldld%lc",
				globstring[STR_CLOCK_MEMORY],
				chipnum+(config->scrclktype&SCRCLOCK_BYTES)?3:0,(config->scrclktype&SCRCLOCK_BYTES)?0:'K');
		}
	}

	FOREVER {
		wmes=Wait(sig);
		if (wmes&1<<clockmsg_port->mp_SigBit) {
			while ((cmsg=(struct dopustaskmsg *)GetMsg(clockmsg_port))) {
				switch (cmsg->command) {
					case TASK_QUIT:
						if (!(CheckIO((struct IORequest *)&ctimereq.tr_node)))
							AbortIO((struct IORequest *)&ctimereq.tr_node);
						WaitIO((struct IORequest *)&ctimereq.tr_node);
						CloseDevice((struct IORequest *)&ctimereq);
						LDeletePort(clock_time_port);
						LDeletePort(clockmsg_port); clockmsg_port=NULL;
						ReplyMsg((struct Message *)cmsg);
						Wait(0);
					case CLOCK_ACTIVE:
						active=cmsg->value;
						break;
				}
				ReplyMsg((struct Message *)cmsg);
			}
		}
		if (CheckIO((struct IORequest *)&ctimereq.tr_node)) {
			WaitIO((struct IORequest *)&ctimereq.tr_node);
			if (active && !(Window->Flags&WFLG_MENUSTATE)) {
				chipc=AvailMem(MEMF_CHIP);
				fast=AvailMem(MEMF_ANY)-chipc;
				if (!(config->scrclktype&SCRCLOCK_BYTES)) chipc/=1024;
				if (!(config->scrclktype&SCRCLOCK_BYTES)) fast/=1024;

				CurrentTime((ULONG *)&secs,(ULONG *)&micro);
				datetime.dat_Stamp.ds_Days=secs/86400; secs-=(datetime.dat_Stamp.ds_Days*86400);
				datetime.dat_Stamp.ds_Minute=secs/60; secs-=(datetime.dat_Stamp.ds_Minute*60);
				datetime.dat_Stamp.ds_Tick=secs*50;
				initdatetime(&datetime,date,time,0);

				if (config->dateformat&DATE_12HOUR) {
					h=datetime.dat_Stamp.ds_Minute/60; m=datetime.dat_Stamp.ds_Minute%60;
					s=datetime.dat_Stamp.ds_Tick/TICKS_PER_SECOND;
					if (h>11) { ampm='P'; h-=12; }
					else ampm='A';
					if (h==0) h=12;
					lsprintf(time,"%2ld:%02ld:%02ld%lc",h,m,s,ampm);
				}

				if (!(config->scrclktype&(SCRCLOCK_MEMORY|SCRCLOCK_CPU|SCRCLOCK_DATE|SCRCLOCK_TIME)))
					lsprintf(formstring,"Directory Opus  Version %s  Compiled %s  %s",
						str_version_string,comp_time,comp_date);
				else {
					formstring[0]=0;
					if (config->scrclktype&SCRCLOCK_MEMORY) {
						if (fastnum>1) lsprintf(buf,memstring,chipc,fast,chipc+fast);
						else lsprintf(buf,memstring,chipc);
						strcat(formstring,buf); strcat(formstring,"  ");
					}
					if (config->scrclktype&SCRCLOCK_CPU) {
						lsprintf(buf,"CPU:%3ld%%  ",(getusage()/10));
						strcat(formstring,buf);
					}
					if (config->scrclktype&SCRCLOCK_DATE) {
						lsprintf(buf,"%-9s  ",date);
						strcat(formstring,buf);
					}
					if (config->scrclktype&SCRCLOCK_TIME) strcat(formstring,time);
				}
				len=strlen(formstring);
				if (len>1 && formstring[len-2]==' ') len-=2;
				cx=(clock_width-dotextlength(&clock_rp,formstring,&len,clock_width-4))/2;
				cx+=scrdata_clock_xpos;
				if (cx<scrdata_clock_xpos) cx=scrdata_clock_xpos;
				Move(&clock_rp,cx,cy); Text(&clock_rp,formstring,len);
				SetAPen(&clock_rp,screen_pens[(int)config->clockbg].pen);
				if (scr_height>ct+1) {
					if (cx>scrdata_clock_xpos) RectFill(&clock_rp,scrdata_clock_xpos,ct,cx-1,scr_height-2);
					if (clock_rp.cp_x<clock_width-1)
						RectFill(&clock_rp,clock_rp.cp_x,ct,clock_width-2,scr_height-2);
				}
				SetAPen(&clock_rp,screen_pens[(int)config->clockfg].pen);
			}
			ctimereq.tr_time.tv_secs=1;
			ctimereq.tr_time.tv_micro=0;
			SendIO((struct IORequest *)&ctimereq.tr_node);
		}
	}
}
