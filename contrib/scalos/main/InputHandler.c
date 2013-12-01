// InputHandler.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/interrupts.h>
#include <utility/hooks.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <intuition/intuitionbase.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

M68KFUNC_P2_PROTO(struct InputEvent *, ScalosInputHandler,
	A0, struct InputEvent *, eventList,
	A1, APTR, isData);
static BOOL PretestPopupMenu(const struct InputEvent *ie);
static BOOL SendPopupMenuMsg(struct internalScaWindowTask *iwt, const struct InputEvent *ie);
static BOOL IsMenuPending(const struct Screen *scr);

//----------------------------------------------------------------------------

// local data items
static BOOL inputOpen = FALSE;
static BOOL handlerAdded = FALSE;
static struct IOStdReq  *inputRequest = NULL;
static struct MsgPort   *inputPort = NULL;
static struct Interrupt *inputHandler = NULL;

//----------------------------------------------------------------------------

BOOL InitInputHandler(void)
{
///
	STATIC_PATCHFUNC(ScalosInputHandler)

	do	{
		inputPort = CreateMsgPort();
		if (NULL == inputPort)
			break;

		inputHandler = ScalosAlloc(sizeof(struct Interrupt));
		if (NULL == inputHandler)
			break;

		inputRequest = (struct IOStdReq *) CreateExtIO(inputPort, sizeof(struct IOStdReq));
		if (NULL == inputRequest)
			break;

		if (0 == OpenDevice("input.device", 0, (struct IORequest *) inputRequest, 0))
			inputOpen = TRUE;

		if (!inputOpen)
			break;

		inputHandler->is_Code = (VOID (*)()) PATCH_NEWFUNC(ScalosInputHandler);
		inputHandler->is_Data = NULL;
                inputHandler->is_Node.ln_Pri = 100;
		inputHandler->is_Node.ln_Name = (STRPTR) "Scalos Input Handler";

                inputRequest->io_Data = (APTR) inputHandler;
                inputRequest->io_Command = IND_ADDHANDLER;

                DoIO((struct IORequest *) inputRequest);
                handlerAdded = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return handlerAdded;
///
}


void CleanupInputHandler(void)
{
///
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (inputOpen)
		{
		if (handlerAdded)
			{
                	inputRequest->io_Data = (APTR) inputHandler;
        	        inputRequest->io_Command = IND_REMHANDLER;

	                DoIO((struct IORequest *) inputRequest);
			}

		CloseDevice((struct IORequest *) inputRequest);
		}
	if (inputRequest)
		{
		DeleteExtIO((struct IORequest *) inputRequest);
		inputRequest = NULL;
		}
	if (inputPort)
		{
		DeleteMsgPort(inputPort);
		inputPort = NULL;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
///
}


/*
* The event list gets passed to you in  a0.
* The is_Data field is passed to you in a1.
*
* On exit you must return the event list in d0.  In this way
* you could add or remove items from the event list.
*/
M68KFUNC_P2(struct InputEvent *, ScalosInputHandler,
	A0, struct InputEvent *, eventList,
	A1, APTR, isData)
{
	struct InputEvent *ie;

	(void) isData;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	for (ie = eventList; ie; ie=ie->ie_NextEvent)
		{
		switch (ie->ie_Class)
			{
		case IECLASS_RAWMOUSE:
			if (IECODE_RBUTTON == ie->ie_Code)
				{
				d1(KPrintF("%s/%s/%ld: ie=%08lx  class=%02lx  subclass=%02lx  code=%04lx  qual=%04lx\n", \
					__FILE__, __FUNC__, __LINE__, ie, ie->ie_Class, ie->ie_SubClass, ie->ie_Code, ie->ie_Qualifier));
				if (PretestPopupMenu(ie))
					ie->ie_Class = IECLASS_NULL;
				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				}
			break;
			}
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return eventList;
}
M68KFUNC_END


static BOOL PretestPopupMenu(const struct InputEvent *ie)
{
	BOOL DoPopupMenu = FALSE;
	struct internalScaWindowTask *iwtUnderPointer = NULL;
	struct ScaIconNode *inUnderPointer = NULL;

	do	{
		struct Screen *MouseScreen;
		struct Window *win;

		if (!PopupMenuFlag)
			break;

		if (NULL == PopupMenuBase)
			break;

		if (NULL == iInfos.xii_iinfos.ii_Screen)
			break;

		d1(KPrintF("%s/%s/%ld: Class=%02lx  SubClass=%02lx  Code=%04lx  Qual=%04lx\n", \
			__FILE__, __FUNC__, __LINE__, ie->ie_Class, ie->ie_SubClass, ie->ie_Code, ie->ie_Qualifier));

		// don't bother checking for popup menus while user is moving around windows
		if (ie->ie_Qualifier & (IEQUALIFIER_LEFTBUTTON | IEQUALIFIER_MIDBUTTON))
			break;

		MouseScreen = SearchMouseScreen(ie->ie_position.ie_xy.ie_x, ie->ie_position.ie_xy.ie_y);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (MouseScreen != iInfos.xii_iinfos.ii_Screen)
			break;

		if (MouseScreen->LayerInfo.Lock.ss_NestCount != 0)
			break;

		if (IsMenuPending(MouseScreen))
			break;

		QueryObjectUnderPointer(&iwtUnderPointer, &inUnderPointer, NULL, &win);

		if (NULL == iwtUnderPointer)
			break;

		if (inUnderPointer)
			{
			// Pointer is over icon
			ULONG IconType;

			GetAttr(IDTA_Type, inUnderPointer->in_Icon, &IconType);

			if (WBAPPICON != IconType || inUnderPointer->in_SupportFlags)
				{
				DoPopupMenu = SendPopupMenuMsg(iwtUnderPointer, ie);
				}
			}
		else
			{
			// Pointer over Scalos window
			WORD x, y;
			WORD xw, yw;
			const struct Gadget *gad;
			const struct Gadget *DepthArrangeGad = NULL;
			WORD RightMostPos = SHRT_MIN;
			struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

			// Try to find depth arrange gadget
			for (gad=iwtUnderPointer->iwt_WindowTask.wt_Window->FirstGadget; gad; gad=gad->NextGadget)
				{
				if (0 == gad->TopEdge && (gad->Flags & GFLG_RELRIGHT) && gad->LeftEdge < 0)
					{
					d1(KPrintF("%s/%s/%ld: RightMostPos=%ld  gad->LeftEdge=%ld\n", \
						__FILE__, __FUNC__, __LINE__, RightMostPos, gad->LeftEdge));

					if (RightMostPos < gad->LeftEdge)
						{
						DepthArrangeGad = gad;
						RightMostPos = gad->LeftEdge;
						}
					}
				}
			d1(KPrintF("%s/%s/%ld: RightMostPos=%ld\n", __FILE__, __FUNC__, __LINE__, RightMostPos));

			xw = MouseScreen->MouseX - iwtUnderPointer->iwt_WindowTask.wt_Window->LeftEdge;
			yw = MouseScreen->MouseY - iwtUnderPointer->iwt_WindowTask.wt_Window->TopEdge;
			x = xw - iwtUnderPointer->iwt_InnerLeft;
			y = yw - iwtUnderPointer->iwt_InnerTop;

			d1(KPrintF("%s/%s/%ld: ie_x=%ld  ie_y=%ld  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, ie->ie_position.ie_xy.ie_x, ie->ie_position.ie_xy.ie_y, x, y));

			if ((y < 0 || !CurrentPrefs.pref_FullPopupFlag || (iwtUnderPointer == iwtMain)) && x >= 0
				&& !PointInGadget(xw, yw, iwtUnderPointer->iwt_WindowTask.wt_Window, DepthArrangeGad)
				&& x < iwtUnderPointer->iwt_WindowTask.wt_Window->Width
				&& y < iwtUnderPointer->iwt_WindowTask.wt_Window->Height)
				{
				if (PopupMenus[(iwtUnderPointer == iwtMain) ? SCPOPUPMENU_Desktop : SCPOPUPMENU_Window])
					DoPopupMenu = SendPopupMenuMsg(iwtUnderPointer, ie);
				else
					{
					// No popup menu if menu not configured
					DoPopupMenu = FALSE;
					}
				}
			}

		d1(KPrintF("%s/%s/%ld: ie=%08lx  class=%02lx  subclass=%02lx  code=%04lx  qual=%04lx  DoPopupMenu=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ie, ie->ie_Class, ie->ie_SubClass, ie->ie_Code, ie->ie_Qualifier, DoPopupMenu));
		} while (0);

	if (iwtUnderPointer)
		{
		if (inUnderPointer)
			ScalosUnLockIconList(iwtUnderPointer);
		SCA_UnLockWindowList();
		}

	return DoPopupMenu;
}


static BOOL SendPopupMenuMsg(struct internalScaWindowTask *iwt, const struct InputEvent *ie)
{
///
	struct SM_DoPopupMenu *sMsg;
	BOOL Success = FALSE;

	sMsg = (struct SM_DoPopupMenu *) SCA_AllocMessage(MTYP_DoPopupMenu, 0);
	d1(KPrintF("%s/%s/%ld: sMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, sMsg));
	if (sMsg)
		{
		sMsg->smdpm_InputEvent = *ie;

		Success = TRUE;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		PutMsg(iwt->iwt_WindowTask.wt_IconPort,
			&sMsg->ScalosMessage.sm_Message);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}

	return Success;
///
}


BOOL PointInGadget(WORD x, WORD y, const struct Window *win, const struct Gadget *gad)
{
///
	struct Rectangle GadgetRect;

	if (NULL == gad)
		return FALSE;

	GadgetRect.MinX = gad->LeftEdge;
	GadgetRect.MinY = gad->TopEdge;

	if (gad->Flags & GFLG_RELRIGHT)
		GadgetRect.MinX += win->Width;
	if (gad->Flags & GFLG_RELBOTTOM)
		GadgetRect.MinY += win->Height;

	GadgetRect.MaxX = GadgetRect.MinX + gad->Width - 1;
	GadgetRect.MaxY = GadgetRect.MinY + gad->Height - 1;

	if (gad->Flags & GFLG_RELWIDTH)
		GadgetRect.MaxX += win->Width;
	if (gad->Flags & GFLG_RELHEIGHT)
		GadgetRect.MaxY += win->Height;

	d1(KPrintF("%s/%s/%ld: x=%ld y=%ld  MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, x, y, GadgetRect.MinX, GadgetRect.MaxX, \
		GadgetRect.MinY, GadgetRect.MaxY));

	return (BOOL) ((x >= GadgetRect.MinX) && (x <= GadgetRect.MaxX)
		&& (y >= GadgetRect.MinY) && (y <= GadgetRect.MaxY));
///
}


static BOOL IsMenuPending(const struct Screen *scr)
{
#if defined(__MORPHOS__)
	// Workaround against "menu-open-lockup":
	// when a menu has been opened with a short RMB click (RMB not held!),
	// and RMB is clicked somewhere on Scalos windows' area, the menu cannot
	// be closed and intuition is completely locked up.
	// Unfortunately, there seems to be not system-conforming way to detect
	// pending (open) menu state, so this nasty hack had to be implemented.
	// This hack relies on MorphOS menu implementation with windows,
	// and specific properties of those windows. It it likely that is
	// will stop working with any new MorphOS release :(
	const struct Window *win;

	for (win = scr->FirstWindow; win; win = win->NextWindow)
		{
		d1(kprintf("%s/%s/%ld: win=%08lx  Title=%08lx  <%s>  w=%ld  h=%ld  Flags=%08lx  IDCMPFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			win, win->Title, win->Title ? win->Title : (UBYTE *) "", win->Width, win->Height, win->Flags, win->IDCMPFlags));

		if ((win->Flags == (WFLG_RMBTRAP | WFLG_NW_EXTENDED | WFLG_REPORTMOUSE | WFLG_BORDERLESS))
			&& (NULL == win->Title) && (0 == win->IDCMPFlags))
			{
			d1(kprintf("%s/%s/%ld: win=%08lx  <%s>  w=%ld  h=%ld  Flags=%08lx  IDCMPFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				win, win->Title ? win->Title : (UBYTE *) "", win->Width, win->Height, win->Flags, win->IDCMPFlags));
			return TRUE;
			}
		}
#endif //__MORPHOS__

	return FALSE;
}


