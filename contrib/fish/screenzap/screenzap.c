/*
 *               ScreenZap    Version 2.3
 *                  by Lars R. Clausen
 *     Hack this code as you want to, as long as I get the credit for
 *                      ScreenZap
 */
 
#include <intuition/intuitionbase.h>
#include <graphics/gfxbase.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <aros/oldprograms.h>

struct Window *myw;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

WORD Pairs1[10][2] =
{{2,2}, {0,4}, {0,19}, {4,23}, {305,23}, {309,19}, {309,4}, {305,0}, {4,0},
{2,2}};

WORD Pairs2[5][2]={{3,2}, {3,21}, {306,21}, {306,2}, {3,2}};

struct Border GadBord2 =
{ 0,0,3,1,JAM1,10,&Pairs1[0][0],NULL};

struct Border GadBord1 =
{ 0,0,1,2,JAM1,5,&Pairs2[0][0],&GadBord2};

struct IntuiText GadText[4] =
{{3,2,JAM2,38,8,NULL,(UBYTE *)" Kill Screens Behind WorkBench ",NULL},
 {3,2,JAM2,38,8,NULL,(UBYTE *)" Kill Screens Except WorkBench ",NULL},
 {3,2,JAM2,86,8,NULL,(UBYTE *)" Kill Front Screen ",NULL},
 {3,2,JAM2,14,8,NULL,(UBYTE *)" Kill Front Window On Front Screen ",NULL}
};
 
struct Gadget Gad[4]=
{
	{ &Gad[1], 7, 12, 310, 24, GADGHCOMP, RELVERIFY | GADGIMMEDIATE ,
	BOOLGADGET, (APTR) &GadBord1, NULL, &GadText[0], NULL, NULL, 0, NULL},
	{ &Gad[2], 7, 39, 310, 24, GADGHCOMP, RELVERIFY | GADGIMMEDIATE ,
	BOOLGADGET, (APTR) &GadBord1, NULL, &GadText[1], NULL, NULL, 1, NULL},
	{ &Gad[3], 323, 12, 310, 24, GADGHCOMP, RELVERIFY | GADGIMMEDIATE ,
	BOOLGADGET, (APTR) &GadBord1, NULL, &GadText[2], NULL, NULL, 2, NULL},
	{ NULL, 323, 39, 310, 24, GADGHCOMP, RELVERIFY | GADGIMMEDIATE ,
	BOOLGADGET, (APTR) &GadBord1, NULL, &GadText[3], NULL, NULL, 3, NULL}
};

struct NewWindow nw =
{
	0,10,640,66,0,1,CLOSEWINDOW | GADGETUP,
	WINDOWCLOSE | ACTIVATE | WINDOWDEPTH | WINDOWDRAG | SMART_REFRESH | NOCAREREFRESH,
	&Gad[0], NULL,(UBYTE *)"Click To Zap Window Or Screen",
	NULL, NULL,295,50,295,50,WBENCHSCREEN
};

/* Remove and reply any IntuiMessages hanging off of a port
 * which were addressed to a particular window.
 * Note that technique does not rely on ln_Succ of a message
 * after it has been replied.
 */
void StripIntuiMessages( mp, win )
struct MsgPort *mp;
struct Window *win;
{
	struct IntuiMessage *msg;
	struct Node *succ;

	msg = (struct IntuiMessage *) mp->mp_MsgList.lh_Head;

	while( (succ = msg->ExecMessage.mn_Node.ln_Succ) != NULL ) {

	if( msg->IDCMPWindow ==  win ) {
		/* Intuition is about to rudely free this message.
		 * Make sure that we have politely sent it back.
		 */
		Remove( (struct Node *) msg );

		ReplyMsg( (struct Message *) msg );
	}

	msg = (struct IntuiMessage *) succ;
	}
}

void CloseWindowSafely( win )
struct Window *win;
{
	 /* we forbid here to keep out of race conditions with Intuition */
    /* Forbid(); */
	
	 /* send back any messages for this window 
	  * that have not yet been processed
	  */
	 StripIntuiMessages( win->UserPort, win );

	 /* clear UserPort so Intuition will not free it */
	 win->UserPort = NULL;

	 /* tell inuition to stop sending more messages */
	 ModifyIDCMP( win, 0L );

	 /* turn tasking back on */
    /* Permit(); */
	 /* Inserted by L. Clausen to get out menus; Remove it to get to original
	    routine */
    if (win->MenuStrip!=NULL) ClearMenuStrip(win);

	 /* and really close the window */
	 CloseWindow( win );
}

/* bail out! */
void abort(nr)
int nr;
{
	CloseWindow(myw);
	CloseLibrary((struct Library *)IntuitionBase);
	CloseLibrary((struct Library *)GfxBase);
	exit(nr);
}

/* open everything needed */
void OpenIt()
{
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0L);
	if (IntuitionBase == 0)
	{
	   exit(1);
	}
	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0L);
	if (GfxBase == 0)
	{
	   abort(1);
	}
	myw = OpenWindow(&nw);
	if (myw == 0)
	{
	   abort(2);
	}
	SetWindowTitles(myw,"Click To Zap Window Or Screen","ScreenZapper V2.3 (c) 1989 Lars R. Clausen. This Program is PD.");
}

/* Zap screen - nr 0: All behind workbench, nr 1: All except workbench,
   nr 2: Front screen.
	This is not multitasking-friendly, but I need the system for myself. */
void ScreenZap(nr)
int nr;
{
	struct Screen *Curs,*Nexts;
	struct Window *Curw;
	int x,y=0;
	char *Title;

	Forbid();
	if (nr==0) Curs = myw->WScreen;
	else Curs = IntuitionBase->FirstScreen;
	if (Curs==myw->WScreen) Curs=Curs->NextScreen;
	for (x=0; Curs!=NULL; x++)
	{
		if (Curs->FirstWindow!=NULL)  /* first close all windows */
		{
			for (Curw=Curs->FirstWindow;Curw->NextWindow!=NULL;Curw=Curs->FirstWindow)
			{
				for (;Curw->NextWindow!=NULL; Curw=Curw->NextWindow);
				CloseWindowSafely(Curw);
				y++;
			}
			CloseWindowSafely(Curw);
			y++;
		}
		Nexts = Curs->NextScreen;
		CloseScreen(Curs);
		if (Nexts==myw->WScreen) Nexts=Nexts->NextScreen;
		Curs = Nexts;
		if (nr==2) Curs=NULL;
	}
	Permit();
	if ((x>1) && (y>1)) sprintf(&Title,"%d Screens, %d Windows Zapped.",x,y);
	if ((x>1) && (y==1)) sprintf(&Title,"%d Screens, 1 Window Zapped.",x);
	if ((x==1) && (y>1)) sprintf(&Title,"1 Screen, %d Windows Zapped.",y);
	if ((x==1) && (y==1)) Title="1 Screen & 1 Window Zapped.";
	if ((x>1) && (y==0)) sprintf(&Title,"%d Screens Zapped.",x);
	if ((x==1) && (y==0)) Title = "1 Screen Zapped.";
	if (x==0) Title = "Nothing Zapped";
	SetWindowTitles(myw,Title,"ScreenZapper V2.3 (c) 1989 Lars R. Clausen. This Program is PD.");
}

/* To kill the front window */
void WindowZap()
{
	struct Screen *Curs;
	struct Window *Curw;
	struct IntuiMessage *UserMsg;
	
	Forbid();
	Curs = IntuitionBase->FirstScreen;
	Curw = (struct Window *)Curs->LayerInfo.top_layer->Window;
	if (Curw)
	{
		if (Curw==myw)
		{
			SetWindowTitles(myw,"That's ME! Use CloseGadget to exit, anything else to continue.",
							 "ScreenZapper V2.3 (c) 1989 Lars R. Clausen. This Program is PD.");
			WaitPort(myw->UserPort);
			UserMsg=(struct IntuiMessage *)GetMsg(myw->UserPort);
			if (UserMsg->Class==CLOSEWINDOW) abort(0);
			else
			SetWindowTitles(myw,"Thanks! Maybe the window has gone - try a Screen kill.",
							 "ScreenZapper V2.3 (c) 1989 Lars R. Clausen. This Program is PD.");
		}
		else
		{
			CloseWindowSafely(Curw);
			SetWindowTitles(myw,"Window is now zapped, ready again",
								 "ScreenZapper V2.3 (c) 1989 Lars R. Clausen. This Program is PD.");
		}
	}
	Permit();
}

void main()
{
	struct IntuiMessage *msg;
	struct Gadget *gad;
	int nr;

	OpenIt();
	WaitPort(myw->UserPort);
	for (msg=(struct IntuiMessage *)GetMsg(myw->UserPort);msg->Class == GADGETUP;msg=(struct IntuiMessage *)GetMsg(myw->UserPort))
	{
		gad=(struct Gadget *)msg->IAddress;
		nr=gad->GadgetID;
		ReplyMsg((struct Message *)msg);
		if (nr<3) ScreenZap(nr);
		else WindowZap();
		WaitPort(myw->UserPort);
	}
	ReplyMsg((struct Message *)msg);
	abort(0);
}
