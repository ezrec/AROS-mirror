// AppWindow.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/intuitionbase.h>
#include <datatypes/iconobject.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>
#include <workbench/workbench.h>
#include <exec/nodes.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/scalos.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/wb.h>

#include <defs.h>

#include "wb39.h"
#include "wb39proto.h"

//-------------------------------------------------------------------------------------

struct ProcessData
	{
	struct NotifyRequest pd_NotifyReq;
	BOOL pd_NotifyOk;
	};

//-------------------------------------------------------------------------------------

static void GetDropZoneDropBox(struct myAppWindowDropZone *dz);
static struct Layer *WhichMouseLayer(WORD x, WORD y);
static BOOL PointInDropZone(const struct myAppWindowDropZone *dzz, WORD x, WORD y);
static void SignalDropzoneMouseEnterLeave(APTR draghandle,
	const struct myAppWindowDropZone *dzz, ULONG EnterLeave);
static SAVEDS(void) myProcess(void);
static BOOL myProcessInit(struct ProcessData *pd);
static void myProcessCleanup(struct ProcessData *pd);
static void ServiceMyPort(void);

//-------------------------------------------------------------------------------------

static struct Task *HandshakeTask;
static ULONG HandshakeSignal = -1;
static struct MsgPort *myPort = NULL;
static BOOL myProcessRunning = FALSE;
static struct Process *myProc = NULL;

static const ULONG Maw_ID = MAKE_ID('M','A','W','X');

//-------------------------------------------------------------------------------------

// aus wb39.c
extern struct List AppWindowList;	// list of all AppWindows
extern struct SignalSemaphore AppWindowSema;


#ifdef __AROS__
extern APTR origAddAppWindowA;
extern APTR origRemoveAppWindow;
extern APTR origSCA_DrawDrag;
#else
extern LIBFUNC_P6_DPROTO(struct ScaAppObjNode *, (*origAddAppWindowA),
	D0, ULONG, id,
	D1, ULONG, userdata,
	A0, struct Window *, window,
	A1, struct MsgPort *, msgport,
	A2, struct TagItem *, taglist,
	A6, struct Library *, WorkbenchBase);
extern LIBFUNC_P2_DPROTO(BOOL, (*origRemoveAppWindow),
	A0, struct ScaAppObjNode *, appWindow,
	A6, struct Library *, WorkbenchBase);
extern LIBFUNC_P5_DPROTO(void, (*origSCA_DrawDrag),
	A0, struct DragHandle *, draghandle,
	D0, LONG, X,
	D1, LONG, Y,
	D2, LONG, Flags,
	A6, struct Library *, ScalosBase);
#endif

//-------------------------------------------------------------------------------------

// AddAppWindowDropZoneA
LIBFUNC_P5(struct myAppWindowDropZone *, myAddAppWindowDropZoneA,
	A0, struct myAppWindow *, maw,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0)
{
	struct myAppWindowDropZone *dz;
	struct TagItem *ti;

	(void)WorkbenchBase;

	d1(kprintf(__FUNC__ "/%ld: AppWindow=%08lx  id=%08lx userdata=%08lx  tags=%08lx\n", \
		__LINE__, maw, id, userdata, tags));

	if (NULL == maw)
		return NULL;

	if (Maw_ID != maw->maw_Magic)
		return NULL;

	dz = malloc(sizeof(struct myAppWindowDropZone));
	if (NULL == dz)
		return NULL;

	dz->awz_AppWindow = maw;
	dz->awz_ID = id;
	dz->awz_UserData = userdata;
	dz->awz_Hook = (struct Hook *) GetTagData(WBDZA_Hook, (ULONG)NULL, tags);

	if (ti = FindTagItem(WBDZA_Box, tags))
		{
		dz->awz_MeasureBox = *((struct IBox *) (ti->ti_Data));

		d1(kprintf(__FUNC__ "/%ld: MeasureBox  left=%ld  top=%ld  width=%ld  height=%ld\n", \
			__LINE__, dz->awz_MeasureBox.Left, dz->awz_MeasureBox.Top, dz->awz_MeasureBox.Width, dz->awz_MeasureBox.Height));

		dz->awz_RelRight = FALSE;
		dz->awz_RelBottom = FALSE;
		dz->awz_RelWidth = FALSE;
		dz->awz_RelHeight = FALSE;
		}
	else
		{
		if (ti = FindTagItem(WBDZA_RelRight, tags))
			{
			dz->awz_MeasureBox.Left = ti->ti_Data;
			dz->awz_RelRight = TRUE;
			}
		else
			{
			if (ti = FindTagItem(WBDZA_Left, tags))
				{
				dz->awz_MeasureBox.Left = ti->ti_Data;
				}
			else
				{
				// no LEFT
				free(dz);
				return NULL;
				}
			dz->awz_RelRight = FALSE;
			}
		if (ti = FindTagItem(WBDZA_RelBottom, tags))
			{
			dz->awz_MeasureBox.Top = ti->ti_Data;
			dz->awz_RelBottom = TRUE;
			}
		else
			{
			if (ti = FindTagItem(WBDZA_Top, tags))
				{
				dz->awz_MeasureBox.Top = ti->ti_Data;
				}
			else
				{
				// no TOP
				free(dz);
				return NULL;
				}
			dz->awz_RelBottom = FALSE;
			}
		if (ti = FindTagItem(WBDZA_RelWidth, tags))
			{
			dz->awz_MeasureBox.Width = ti->ti_Data;
			dz->awz_RelWidth = TRUE;
			}
		else
			{
			if (ti = FindTagItem(WBDZA_Width, tags))
				{
				dz->awz_MeasureBox.Width = ti->ti_Data;
				}
			else
				{
				// no WIDTH
				free(dz);
				return NULL;
				}
			dz->awz_RelWidth = FALSE;
			}
		if (ti = FindTagItem(WBDZA_RelHeight, tags))
			{
			dz->awz_MeasureBox.Height = ti->ti_Data;
			dz->awz_RelHeight = TRUE;
			}
		else
			{
			if (ti = FindTagItem(WBDZA_Height, tags))
				{
				dz->awz_MeasureBox.Height = ti->ti_Data;
				}
			else
				{
				// no HEIGHT
				free(dz);
				return NULL;
				}
			dz->awz_RelHeight = FALSE;
			}
		}

	dz->awz_DropBox = dz->awz_MeasureBox;

	GetDropZoneDropBox(dz);

	d1(kprintf(__FUNC__ "/%ld: dz=%08lx  left=%ld  top=%ld  width=%ld  height=%ld\n", \
		__LINE__, dz, dz->awz_DropBox.Left, dz->awz_DropBox.Top, \
		dz->awz_DropBox.Width, dz->awz_DropBox.Height));

	ObtainSemaphore(&maw->maw_Sema);

	AddTail(&maw->maw_DropZoneList, &dz->awz_Node);

	ReleaseSemaphore(&maw->maw_Sema);

	return dz;
}
LIBFUNC_END


// RemoveAppWindowDropZone
LIBFUNC_P3(BOOL, myRemoveAppWindowDropZone,
	A0, struct myAppWindow *, maw,
	A1, struct myAppWindowDropZone *, dz,
	A6, struct Library *, WorkbenchBase, 0)
{
	struct myAppWindowDropZone *dzz;
	BOOL Found = FALSE;

	(void)WorkbenchBase;

	d1(kprintf(__FUNC__ "/%ld: AppWindow=%08lx  dropZone=%08lx\n", \
		__LINE__, maw, dz));

	if (NULL== maw)
		return TRUE;
	if (Maw_ID != maw->maw_Magic)
		return FALSE;
	if (NULL == dz)
		return TRUE;

	ObtainSemaphore(&maw->maw_Sema);

	for (dzz = (struct myAppWindowDropZone *) maw->maw_DropZoneList.lh_Head;
		!Found && dzz != (struct myAppWindowDropZone *) &maw->maw_DropZoneList.lh_Tail;
		dzz = (struct myAppWindowDropZone *) dzz->awz_Node.ln_Succ)
		{
		d1(kprintf(__FUNC__ "/%ld: dz=%08lx  dzz=%08lx\n", __LINE__, dz, dzz));
		if (dz == dzz)
			Found = TRUE;
		}

	if (Found)
		{
		Remove(&dz->awz_Node);	// remove dropzone from list
		free(dz);
		}

	ReleaseSemaphore(&maw->maw_Sema);

	d1(kprintf(__FUNC__ "/%ld: Found=%ld\n", __LINE__, Found));

	return Found;
}
LIBFUNC_END


// AddAppWindowA
LIBFUNC_P6(struct myAppWindow *, myAddAppWindowA,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A0, struct Window *, window,
	A1, struct MsgPort *, msgport,
	A2, struct TagItem *, taglist,
	A6, struct Library *, WorkbenchBase, 0)
{
	struct ScaAppObjNode *aw;
	struct myAppWindow *maw;

	d1(KPrintF(__FUNC__ "/%ld: id=%08lx  userdata=%08lx\n", __LINE__, id, userdata));

	maw = calloc(sizeof(struct myAppWindow), 1);
	d1(KPrintF(__FUNC__ "/%ld: maw=%08lx\n", __LINE__, maw));
	if (NULL == maw)
		{
		return NULL;
		}

	maw->maw_Magic = Maw_ID;
	maw->maw_AppWindow = NULL;
	maw->maw_ActiveDropZone = NULL;
	maw->maw_origUserData = userdata;
	maw->maw_origMsgPort = msgport;

	NewList(&maw->maw_DropZoneList);
	InitSemaphore(&maw->maw_Sema);

	d1(KPrintF(__FUNC__ "/%ld: origAddAppWindowA=%08lx\n", __LINE__, origAddAppWindowA));

	// all AppWindow Messages will be relayed to myPort
#ifdef __AROS__
	aw = (struct ScaAppObjNode *)AROS_CALL5(struct AppWindow *, origAddAppWindowA,
			AROS_LDA(ULONG, id, D0),
			AROS_LDA(ULONG, (ULONG)maw, D1),
			AROS_LDA(struct Window *, window, A0),
			AROS_LDA(struct MsgPort *, msgport, A1),
			AROS_LDA(struct TagItem *, taglist, A2),
			struct Library *, WorkbenchBase);
#else
	aw = (struct ScaAppObjNode *) CALLLIBFUNC_P6(origAddAppWindowA,
		D0, id,
		D1, (ULONG) maw,
		A0, window,
		A1, myPort,
		A2, taglist,
		A6, WorkbenchBase);
#endif

	d1(KPrintF(__FUNC__ "/%ld: aw=%08lx\n", __LINE__, aw));
	if (NULL == aw)
		{
		return NULL;
		}

	maw->maw_AppWindow = aw;

	ObtainSemaphore(&AppWindowSema);
	AddTail(&AppWindowList, &maw->maw_Node);
	ReleaseSemaphore(&AppWindowSema);

	d1(KPrintF(__FUNC__ "/%ld: maw=%08lx\n", __LINE__, maw));

	return maw;
}
LIBFUNC_END


LIBFUNC_P2(BOOL, myRemoveAppWindow,
	A0, struct myAppWindow *, maw,
	A6, struct Library *, WorkbenchBase, 0)
{
	BOOL Result;
	struct myAppWindowDropZone *dz;

	d1(KPrintF(__FUNC__ "/%ld: maw=%08lx\n", __LINE__, maw));

	if (NULL == maw || 0 == TypeOfMem(maw))
		return TRUE;
	if (Maw_ID != maw->maw_Magic)
		{
#ifdef __AROS__
		return AROS_CALL1(BOOL, origRemoveAppWindow,
				AROS_LDA(struct AppWindow *, maw, A0),
				struct Library *, WorkbenchBase);
#else
		return CALLLIBFUNC_P2(origRemoveAppWindow,
			A0, (struct ScaAppObjNode *) maw,
			A6, WorkbenchBase);
#endif
		}

	d1(KPrintF(__FUNC__ "/%ld: maw=%08lx\n", __LINE__, maw));

	ObtainSemaphore(&AppWindowSema);
	Remove(&maw->maw_Node);
	ReleaseSemaphore(&AppWindowSema);

	ObtainSemaphore(&maw->maw_Sema);

#ifdef __AROS__
	Result = AROS_CALL1(BOOL, origRemoveAppWindow,
		AROS_LDA(struct AppWindow *, maw->maw_AppWindow, A0),
		struct Library *, WorkbenchBase);
#else
	Result = CALLLIBFUNC_P2(origRemoveAppWindow,
		A0, maw->maw_AppWindow,
		A6, WorkbenchBase);
#endif

	d1(KPrintF(__FUNC__ "/%ld: Result=%08lx\n", __LINE__, Result));

	// Cleanup all added dropzones
	while (dz = (struct myAppWindowDropZone *) RemHead(&maw->maw_DropZoneList))
		{
		free(dz);
		}

	ReleaseSemaphore(&maw->maw_Sema);

	free(maw);

	return Result;
}
LIBFUNC_END


LIBFUNC_P5(void, mySCA_DrawDrag,
	A0, APTR, draghandle,
	D0, LONG, X,
	D1, LONG, Y,
	D2, LONG, Flags,
	A6, struct Library *, ScalosBase, 0)
{
	struct Layer *mLayer;

	d1(kprintf(__FUNC__ "/%ld: draghandle=%08lx  x=%ld y=%ld\n", __LINE__, draghandle, X, Y));

	mLayer = WhichMouseLayer(X, Y);
	if (mLayer && mLayer->Window)
		{
		struct myAppWindow *maw, *mmaw = NULL;

		ObtainSemaphore(&AppWindowSema);

		for (maw = (struct myAppWindow *) AppWindowList.lh_Head; 
			maw != (struct myAppWindow *) &AppWindowList.lh_Tail; 
			maw = (struct myAppWindow *) maw->maw_Node.ln_Succ)
			{
			if ((struct Window *) maw->maw_AppWindow->an_object == mLayer->Window)
				{
				mmaw = maw;
				break;
				}
			}

		ReleaseSemaphore(&AppWindowSema);

		d1(kprintf(__FUNC__ "/%ld: AppWindow=%08lx\n", __LINE__, mmaw));

		// Mouse is inside AppWindow, now check DropZones
		if (mmaw && !IsListEmpty(&mmaw->maw_DropZoneList))
			{
			struct myAppWindowDropZone *dzz;
			WORD wx, wy;
			struct Window *win = (struct Window *) mmaw->maw_AppWindow->an_object;
			BOOL Found = FALSE;

			wx = X - win->LeftEdge;
			wy = Y - win->TopEdge;

			for (dzz = (struct myAppWindowDropZone *) maw->maw_DropZoneList.lh_Head;
				dzz != (struct myAppWindowDropZone *) &maw->maw_DropZoneList.lh_Tail;
				dzz = (struct myAppWindowDropZone *) dzz->awz_Node.ln_Succ)
				{
				d1(kprintf(__FUNC__ "/%ld: DropZone=%08lx\n", __LINE__, dzz));

				GetDropZoneDropBox(dzz);

				if (PointInDropZone(dzz, wx, wy))
					{
					d1(kprintf(__FUNC__ "/%ld: INSIDE \n", __LINE__));
					if (mmaw->maw_ActiveDropZone != dzz)
						{
#ifdef __AROS__
						AROS_CALL4(VOID, origSCA_DrawDrag,
							AROS_LDA(struct DragHandle *, draghandle, A0),
							AROS_LDA(LONG, 400 + win->WScreen->Width, D0),
							AROS_LDA(LONG, 400 + win->WScreen->Height, D1),
							AROS_LDA(ULONG, (Flags | SCAF_Drag_Hide), D2),
							struct Library *, ScalosBase);        // Hide Bobs
#else
						CALLLIBFUNC_P5(origSCA_DrawDrag,
							A0, draghandle,
							D0, 400 + win->WScreen->Width,
							D1, 400 + win->WScreen->Height,
							D2, (Flags | SCAF_Drag_Hide),
							A6, ScalosBase);        // Hide Bobs
#endif

						if (mmaw->maw_ActiveDropZone)
							{
							SignalDropzoneMouseEnterLeave(draghandle, mmaw->maw_ActiveDropZone, ADZMACTION_Leave);
							}

						mmaw->maw_ActiveDropZone = dzz;
						SignalDropzoneMouseEnterLeave(draghandle, dzz, ADZMACTION_Enter);
						}

					Found = TRUE;
					break;
					}
				}

			if (!Found && mmaw->maw_ActiveDropZone)
				{
				d1(kprintf(__FUNC__ "/%ld: OUTSIDE \n", __LINE__));

#ifdef __AROS__
				AROS_CALL4(VOID, origSCA_DrawDrag,
					AROS_LDA(struct DragHandle *, draghandle, A0),
					AROS_LDA(LONG, 400 + win->WScreen->Width, D0),
					AROS_LDA(LONG, 400 + win->WScreen->Height, D1),
					AROS_LDA(ULONG, (Flags | SCAF_Drag_Hide), D2),
					struct Library *, ScalosBase);
#else
				CALLLIBFUNC_P5(origSCA_DrawDrag,
					A0, draghandle,
					D0, 400 + win->WScreen->Width,
					D1, 400 + win->WScreen->Height,
					D2, (Flags | SCAF_Drag_Hide),
					A6, ScalosBase);        // Hide Bobs
#endif

				SignalDropzoneMouseEnterLeave(draghandle, mmaw->maw_ActiveDropZone, ADZMACTION_Leave);
				mmaw->maw_ActiveDropZone = NULL;
				}
			}
		}

#ifdef __AROS__
	AROS_CALL4(VOID, origSCA_DrawDrag,
		AROS_LCA(struct DragHandle *, draghandle, A0),
		AROS_LCA(LONG, X, D0),
		AROS_LCA(LONG, Y, D1),
		AROS_LCA(ULONG, Flags, D2),
		struct Library *, ScalosBase);
#else
	CALLLIBFUNC_P5(origSCA_DrawDrag,
		A0, draghandle,
		D0, X,
		D1, Y,
		D2, Flags,
		A6, ScalosBase);
#endif
}
LIBFUNC_END


// Calculate awz_DropBox from awz_MeasureBox, taking into account window-relative coordinates
static void GetDropZoneDropBox(struct myAppWindowDropZone *dz)
{
	struct Window *win = (struct Window *) dz->awz_AppWindow->maw_AppWindow->an_object;

	d1(kprintf(__FUNC__ "/%ld: AppWindow=%08lx  win=%08lx\n", \
		__LINE__, dz->awz_AppWindow->maw_AppWindow, win));

	d1(kprintf(__FUNC__ "/%ld: relright=%ld relbottom=%ld relwidth=%ld relheight=%ld\n", \
		__LINE__, dz->awz_RelRight, dz->awz_RelBottom, dz->awz_RelWidth, dz->awz_RelHeight));

	if (dz->awz_RelRight)
		dz->awz_DropBox.Left = win->Width + dz->awz_MeasureBox.Left;
	if (dz->awz_RelBottom)
		dz->awz_DropBox.Top = win->Height + dz->awz_MeasureBox.Top;
	if (dz->awz_RelWidth)
		dz->awz_DropBox.Width = win->Width + dz->awz_MeasureBox.Width;
	if (dz->awz_RelHeight)
		dz->awz_DropBox.Height = win->Height + dz->awz_MeasureBox.Height;
}


static struct Layer *WhichMouseLayer(WORD x, WORD y)
{
	struct Layer *layer = NULL;
	struct Screen *scr;
	extern struct IntuitionBase *IntuitionBase;

	Forbid();

	for (scr = IntuitionBase->FirstScreen; scr; scr = scr->NextScreen)
		{
		d1( kprintf("screen=%lx  Offsetx=%ld ", scr, scr->ViewPort.DyOffset) );
		d1( kprintf("x=%ld  y=%ld\n", scr->MouseX, scr->MouseY) );

		if (scr->MouseY >= 0 && scr->MouseY < scr->Height)
			break;
		}

	Permit();

	if (scr)
		{
		layer = WhichLayer(&scr->LayerInfo, x, y);
		}

	d1(kprintf("Screen=%lx  x=%3ld  y=%3ld\n", scr, IntuitionBase->MouseX,
			IntuitionBase->MouseY) );

	return(layer);
}


static BOOL PointInDropZone(const struct myAppWindowDropZone *dzz, WORD x, WORD y)
{
	if (x < dzz->awz_DropBox.Left || y < dzz->awz_DropBox.Top)
		return FALSE;
	if (x >= dzz->awz_DropBox.Left + dzz->awz_DropBox.Width)
		return FALSE;
	if (y >= dzz->awz_DropBox.Top + dzz->awz_DropBox.Height)
		return FALSE;

	return TRUE;
}


static void SignalDropzoneMouseEnterLeave(APTR draghandle,
	const struct myAppWindowDropZone *dzz, ULONG EnterLeave)
{
	d1(kprintf(__FUNC__ "/%ld: DropZone=%08lx  Hook=%08lx  Action=%ld  x=%ld y=%ld w=%ld h=%ld\n", \
		__LINE__, dzz, dzz->awz_Hook, EnterLeave, dzz->awz_DropBox.Left, \
		dzz->awz_DropBox.Top, dzz->awz_DropBox.Width, dzz->awz_DropBox.Height));

	if (dzz->awz_Hook)
		{
		struct AppWindowDropZoneMsg msg;
		ULONG WasLocked;

		msg.adzm_RastPort = ((struct Window *) dzz->awz_AppWindow->maw_AppWindow->an_object)->RPort;
		msg.adzm_Action = EnterLeave;
		msg.adzm_DropZoneBox = dzz->awz_DropBox;
		msg.adzm_ID = dzz->awz_ID;
		msg.adzm_UserData = dzz->awz_UserData;

		WasLocked = SCA_UnlockDrag(draghandle);
		CallHookPkt(dzz->awz_Hook, NULL, &msg);
		if (WasLocked)
			SCA_LockDrag(draghandle);
		}
}

/* ====================================================== */

BOOL DetachMyProcess(void)
{
	STATIC_PATCHFUNC(myProcess);
	HandshakeTask = FindTask(NULL);
	HandshakeSignal = AllocSignal(-1);
	if (-1 == HandshakeSignal)
		{
//		alarm("AllocSignal() failed");
		return FALSE;
		}

	myProc = CreateNewProcTags(NP_Name, (ULONG) "wb39_plugin",
			NP_Priority, 0,
			NP_Entry, (ULONG) PATCH_NEWFUNC(myProcess),
			NP_StackSize, 16384,
			TAG_END);
	if (myProc == NULL)
		{
//		alarm("TNC_INit: CreateProc() failed");
		return FALSE;
		}

	Wait(1 << HandshakeSignal);		/* Warten auf Ergebnis */

	FreeSignal(HandshakeSignal);
	HandshakeSignal = -1;

	if (!myProcessRunning)
		myProc = NULL;

	return myProcessRunning;
}


void ShutdownMyProcess(void)
{
	if (NULL == myProc)
		return;

	HandshakeTask = FindTask(NULL);
	HandshakeSignal = AllocSignal(-1);
	if (-1 == HandshakeSignal)
		{
//		alarm("AllocSignal() failed");
		return;
		}

	myProcessRunning = FALSE;

	Signal(&myProc->pr_Task, 1 << myPort->mp_SigBit);	// Signal myProc

	Wait(1 << HandshakeSignal);		/* Warten auf Ergebnis */

	FreeSignal(HandshakeSignal);
	HandshakeSignal = -1;

	myProc = NULL;
}


/* ====================================================== */

static SAVEDS(void) myProcess(void)
{
	struct ProcessData pd =
		{
		{ NULL },
		FALSE,
		};

	d1(kprintf("%s/%s/%ld Start TNCProcess\n", __FILE__, __FUNC__, __LINE__);)

	if (myProcessInit(&pd))
		{
		ULONG myPortMask = 1UL << myPort->mp_SigBit;
		ULONG NotifyMask = 1UL << pd.pd_NotifyReq.nr_stuff.nr_Signal.nr_SignalNum;
		ULONG Event;

		d1(kprintf("%s/%s/%ld: myPortMask=%08lx NotifyMask=%08lx\n", __FILE__, __FUNC__, __LINE__,
			myPortMask, NotifyMask));

		myProcessRunning = TRUE;

		if (HandshakeSignal != -1)
			Signal(HandshakeTask, 1 << HandshakeSignal);

		while (myProcessRunning)
			{
			Event = Wait(myPortMask | NotifyMask);

			d1(kprintf("%s/%s/%ld: Event=%08lx\n", __FILE__, __FUNC__, __LINE__,
				Event));

			if (Event & myPortMask)
				{
				ServiceMyPort();
				}
			if (Event & NotifyMask)
				{
				ParseWBPrefs("ENV:sys/Workbench.prefs");
				}
			}
		}

	myProcessRunning = FALSE;

	myProcessCleanup(&pd);

	d1(kprintf("%s/%s/%ld End TNCProcess\n", __FILE__, __FUNC__, __LINE__);)

	Forbid();
	if (HandshakeSignal != -1)
		Signal(HandshakeTask, 1 << HandshakeSignal);
}


static BOOL myProcessInit(struct ProcessData *pd)
{
	myPort = CreateMsgPort();
	if (!myPort)
		{
//		alarm(__FUNC__ ": CreateMsgPort() failed");
		return FALSE;
		}

	memset(&pd->pd_NotifyReq, 0, sizeof(pd->pd_NotifyReq));

	pd->pd_NotifyReq.nr_Name = "ENV:sys/Workbench.prefs";
	pd->pd_NotifyReq.nr_UserData = 0;
	pd->pd_NotifyReq.nr_Flags = NRF_SEND_SIGNAL;
	pd->pd_NotifyReq.nr_stuff.nr_Signal.nr_Task = FindTask(NULL);
	pd->pd_NotifyReq.nr_stuff.nr_Signal.nr_SignalNum = AllocSignal(-1);

	pd->pd_NotifyOk = StartNotify(&pd->pd_NotifyReq);

	d1(kprintf(__FUNC__ "/%ld: NotifyOk=%ld\n", __LINE__, pd->pd_NotifyOk));

	return TRUE;
}


static void myProcessCleanup(struct ProcessData *pd)
{
	if (myPort)
		{
		struct Message *Msg;

		while (Msg = GetMsg(myPort))
			{
			switch (Msg->mn_Node.ln_Type)
				{
			case NT_MESSAGE:
				ReplyMsg(Msg);
				break;

			case NT_REPLYMSG:
				free(Msg);
				break;
				}
			}

		DeleteMsgPort(myPort);
		myPort = NULL;
		}

	if (pd->pd_NotifyOk)
		{
		EndNotify(&pd->pd_NotifyReq);
		pd->pd_NotifyOk = FALSE;
		}
}


static void ServiceMyPort(void)
{
	struct AppMessage *Msg;

	while (Msg = (struct AppMessage *) GetMsg(myPort))
		{
		struct AppMessage *myMsg;
		struct myAppWindow *maw;

		switch (Msg->am_Message.mn_Node.ln_Type)
			{
		case NT_MESSAGE:
			switch (Msg->am_Type)
				{
			case AMTYPE_APPWINDOW:
				maw = (struct myAppWindow *) Msg->am_UserData;

				d1(kprintf(__FUNC__ "/%ld: Msg=%08lx maw=%08lx ID=%08lx\n", __LINE__, Msg, maw, Msg->am_ID));

				myMsg = malloc(sizeof(struct AppMessage));
				if (myMsg)
					{
					*myMsg = *Msg;
					myMsg->am_Message.mn_ReplyPort = myPort;
					myMsg->am_UserData = maw->maw_origUserData;
					myMsg->am_Reserved[7] = (ULONG) Msg;	// Remember original Msg for Replying

					if (maw->maw_ActiveDropZone)
						{
						d1(kprintf(__FUNC__ "/%ld: DropZone dzz=%08lx\n", __LINE__, maw->maw_ActiveDropZone));

						if (PointInDropZone(maw->maw_ActiveDropZone,
								Msg->am_MouseX, Msg->am_MouseY))
							{
							d1(kprintf(__FUNC__ "/%ld: DropZone dzz=%08lx\n", __LINE__, maw->maw_ActiveDropZone));

							myMsg->am_Type = AMTYPE_APPWINDOWZONE;
							myMsg->am_UserData = maw->maw_ActiveDropZone->awz_UserData;
							myMsg->am_ID = maw->maw_ActiveDropZone->awz_ID;
							}
						else
							{
							maw->maw_ActiveDropZone = NULL;
							}
						}

					PutMsg(maw->maw_origMsgPort, (struct Message *) myMsg);
					}
				break;
			default:
				ReplyMsg((struct Message *) Msg);
				break;
				}
			break;

		case NT_REPLYMSG:
			if (MAKE_ID('A','P','P','M') == Msg->am_Reserved[0] && Msg->am_Reserved[7])
				{
				// Reply original Msg
				ReplyMsg((struct Message *) Msg->am_Reserved[7]);
				}
			free(Msg);
			break;
			}
		}
}

#ifdef __amigaos4__
LIBFUNC_P4VA(struct AppWindowDropZone *, myAddAppWindowDropZone,
	A0, struct AppWindow *, maw,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A6, struct Library *, WorkbenchBase)
{
	struct AppWindowDropZone *ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, userdata);

	(void)WorkbenchBase;

	ret = AddAppWindowDropZoneA(maw, id, userdata, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

LIBFUNC_P5VA(struct AppWindow *, myAddAppWindow,
	D0, ULONG, id,
	D1, ULONG, userdata,
	A0, struct Window *, window,
	A1, struct MsgPort *, msgport,
	A6, struct Library *, WorkbenchBase)
{
	struct AppWindow *ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, msgport);

	(void)WorkbenchBase;

	ret = AddAppWindowA(id, userdata, window, msgport, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END
#endif


