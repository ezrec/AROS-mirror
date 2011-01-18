#include <string.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/reqtools.h>

extern struct IntuitionBase *IntuitionBase;

void bigwindow(void)
{
	struct Window *w;
	struct Screen *s;
	int hoch,breit,x,y;

	Forbid();
	
	s=IntuitionBase->ActiveScreen;

	breit=s->Width;
	hoch=s->Height;

	w=IntuitionBase->ActiveWindow;
	if(!w){Permit();return;}

	x=w->MaxWidth == -1 ? breit : w->MaxWidth;
	y=w->MaxHeight == -1 ? hoch : w->MaxHeight;

	ChangeWindowBox(w,w->LeftEdge,w->TopEdge,x,y);

	Permit();
}

void smallwindow(void)
{
	struct Window *w;

	Forbid();
	
	w=IntuitionBase->ActiveWindow;

	if(w)	ChangeWindowBox(w,w->LeftEdge,w->TopEdge,w->MinWidth,w->MinHeight);

	Permit();
}

void zipwindow(void)
{
	struct Window *w;

	Forbid();
	
	w=IntuitionBase->ActiveWindow;
	if(w)	ZipWindow(w);

	Permit();
}

void activatefront(void)
{
	struct Window *w,*aw;
	struct Screen *s;
	struct Layer *l;

	Forbid();
	s=IntuitionBase->FirstScreen;
	
	if(!s){Permit();return;}		/* Kein Screen da --> */
	
	aw=w=s->FirstWindow;

	if(!w){Permit();return;}		/* Kein Window da --> */

	l=w->WLayer;

	while(l->front)l=l->front;						/* Vordersten Layer suchen */
	while((w->WLayer!=l) && w)w=w->NextWindow;	/* Window dazu suchen */
	
	Permit();

	if(w)ActivateWindow(w);	/* Bei Backdrop Window ist ScreenTitleLayer */
	else	if(aw)ActivateWindow(aw);		/* vor dem WindowLayer */
}

void worktofront(void)
{
	struct Window *w;
	struct Screen *s;
	WBenchToFront();

	Forbid();
	s=IntuitionBase->FirstScreen;
	
	while(s)
	{
		if(strcmp(s->Title,"Workbench Screen")==0)break;
		s=s->NextScreen;
	}
	
	if(s==0){Permit();return;}
	
	w=s->FirstWindow;

	while(w)
	{
	if(w->Flags&WFLG_WBENCHWINDOW)break;
		w=w->NextWindow;
	};

	Permit();

	ActivateWindow(w);
	WindowToFront(w);

}

void cycle(void)
{
	struct Window *w,*nw;
	struct Layer *l,*nl;

	Forbid();
	nw=w=IntuitionBase->ActiveScreen->FirstWindow;
	l=w->WLayer;
	
	if(!w){Permit();return;}
	
	while(l->front)l=l->front;						/* Vordersten Layer suchen */
	while((w->WLayer!=l) && w)w=w->NextWindow;	/* Window dazu suchen */
	
	if((nl=l->back))
	{
		while((nw->WLayer!=nl) && nw)nw=nw->NextWindow;
	}
	
	Permit();
	if(!w)	return;
	WindowToBack(w);

	if((!nl) || (!nw)) ActivateWindow(w);
	else 	
	{
		if	(	(nw->TopEdge==11)	&&	(nw->LeftEdge==0)	&&
			(nw->Width==640)	&&	(nw->Height==245)	)
			ActivateWindow(w);
		else
			ActivateWindow(nw);
	}
}

void activetofront(void)
{
	struct Window *w;
	
	Forbid();
	if((w=IntuitionBase->ActiveWindow))WindowToFront(w);
	Permit();
}

void closewindow(void)
{
	struct IOStdReq *InputIO;
	struct MsgPort *InputMP;
	struct InputEvent *FakeEvent;
	
	if((InputMP=CreateMsgPort()))
	{
		if((FakeEvent=AllocMem(sizeof(struct InputEvent),MEMF_PUBLIC)))
		{
			if((InputIO=CreateIORequest(InputMP,sizeof(struct IOStdReq))))
			{
				if(!OpenDevice("input.device",NULL,(struct IORequest *)InputIO,NULL))
				{
					FakeEvent->ie_NextEvent=0;
					FakeEvent->ie_Class=IECLASS_CLOSEWINDOW;
					
					InputIO->io_Data=(APTR)FakeEvent;
					InputIO->io_Length=sizeof(struct InputEvent);
					InputIO->io_Command=IND_WRITEEVENT;
					DoIO((struct IORequest *)InputIO);
				}
				else 	rtEZRequest("Unable to open input.device !", "Continue", NULL, NULL);
				DeleteIORequest((struct IORequest *)InputIO);
			}
			else 	rtEZRequest("CreateIORequest failed !", "Continue", NULL, NULL);
			FreeMem(FakeEvent,sizeof(struct InputEvent));
		}
		else 	rtEZRequest ("No memory for InputEvent !", "Continue", NULL, NULL);
		DeleteMsgPort(InputMP);
	}
	else 	rtEZRequest ("Unable to create MsgPort for InputEvent !", "Continue", NULL, NULL);

}
