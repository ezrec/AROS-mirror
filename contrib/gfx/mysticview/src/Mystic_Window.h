#ifndef MYSTIC_WINDOW_H
#define	MYSTIC_WINDOW_H 1

/*********************************************************************
----------------------------------------------------------------------

	MysticView 
	Window

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

#include <intuition/intuition.h>
#include <workbench/workbench.h>

#include "Mystic_Global.h"


struct mvwindow
{
	struct SignalSemaphore semaphore;

	struct Window *window;
	struct Menu *menu;

//	struct MsgPort *appmsgport;
//	ULONG appSignal;
	struct AppWindow *appwindow;

//	struct DiskObject *diskobject;

	ULONG idcmpSignal;

	LONG backpen;

	UWORD winleft, wintop;
	UWORD winwidth, winheight;
	UWORD innerwidth, innerheight;
	UWORD innerleft, innertop;

	WORD otherwinpos[4];			/* alternate window position x,y,w,h */

	APTR drawhandle;			/* rastport's drawhandle */

	ULONG idcmpmask;

	int windowbusycount;

};


#include "Mystic_InitExit.h"
#include "Mystic_Screen.h"
#include "Mystic_Settings.h"

/*--------------------------------------------------------------------

	prototypes

--------------------------------------------------------------------*/

extern struct mvwindow *CreateMVWindow(struct mview *mv, struct mvscreen *scr, struct mainsettings *mvs);
extern void DeleteMVWindow(struct mview *mv, struct mvwindow *win);
extern void UpdateWindowParameters(struct mvwindow *win);

extern void WindowBusy(struct mvwindow *win);
extern void WindowFree(struct mvwindow *win);

#endif
