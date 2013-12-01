// Scalos_Cx.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <intuition/intuitionbase.h>
#include <libraries/commodities.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/commodities.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

#if defined(__MORPHOS__)
#define	TITLEBAR_ON	FALSE
#define	TITLEBAR_OFF	TRUE
#else
#define	TITLEBAR_ON	TRUE
#define	TITLEBAR_OFF	FALSE
#endif

//----------------------------------------------------------------------------

// local functions

static SAVEDS(void) INTERRUPT RawMouseMoveHandler(CxMsg *cxm, CxObj *co);
static SAVEDS(void) INTERRUPT RawMouseButtonsHandler(CxMsg *cxm, CxObj *co);
static SAVEDS(void) PopScreenTitle(const struct Screen *MouseScreen);
static void HighlightControlbarGadgets(BOOL QoSuccess,
	struct internalScaWindowTask *iwtUnderPointer, struct ScaIconNode *iconUnderPointer);

//----------------------------------------------------------------------------

// local data items :

static CxObj *ScalosCxBroker;
STATIC_PATCHFUNC(RawMouseMoveHandler)
STATIC_PATCHFUNC(RawMouseButtonsHandler)

static struct internalScaWindowTask *LastIwtUnderPointer = NULL;

//----------------------------------------------------------------------------

// public data items :

BYTE ScreenBarEnabled;	//is the title bar visible ?

//----------------------------------------------------------------------------


BOOL InitCx(void)
{
///
	static struct NewBroker newBroker = 
		{
		NB_VERSION,				// nb_Version
		"Scalos",				// nb_Name
		"Scalos Screen Title Popup Hook",	// nb_Title
		"The Workbench replacement!",		// nb_Descr
		NBU_DUPLICATE,				// nb_Unique,
		0,					// nb_Flags
		15,					// nb_Pri
		NULL,					// nb_Port
		0					// nb_ReservedChannel
		};
	static IX RawMouseMoveIX =
		 {
		/* An input expression to match any RAWMOUSE event */
		IX_VERSION,	/* required */
		IECLASS_RAWMOUSE,

				/* debug: am i listening right?  */
		0,		/* Code   */
		0,		/* CodeMask   */

		0,		/* qualifier I am interested in  */
		IEQUALIFIER_LEFTBUTTON | IEQUALIFIER_MIDBUTTON | IEQUALIFIER_RBUTTON,   // ix_QualMask

		0		/* synonyms irrelevant */
		};
	static IX RawMouseButtons =
		 {
		/* An input expression to match any RAWMOUSE event */
		IX_VERSION,	/* required */
		IECLASS_RAWMOUSE,

				/* debug: am i listening right?  */
		0,		/* Code   */
		0,		/* CodeMask   */

		IEQUALIFIER_LEFTBUTTON,	  /* qualifier I am interested in  */
		IEQUALIFIER_LEFTBUTTON,   // ix_QualMask

		0		/* synonyms irrelevant */
		};
	BOOL Success = FALSE;
	CxObj *mFilterMouseMove = NULL;
	CxObj *mFilterMouseButtons = NULL;

	if (ScalosCxBroker)
		return TRUE;

	do	{
		newBroker.nb_Port = iInfos.xii_iinfos.ii_MainMsgPort;

		ScalosCxBroker = CxBroker(&newBroker, NULL);
		if (NULL == ScalosCxBroker)
			break;

		mFilterMouseMove = CxFilter(NULL);
		if (NULL == mFilterMouseMove)
			break;

		mFilterMouseButtons = CxFilter(NULL);
		if (NULL == mFilterMouseButtons)
			break;

		SetFilterIX(mFilterMouseMove, &RawMouseMoveIX);
		SetFilterIX(mFilterMouseButtons, &RawMouseButtons);

		AttachCxObj(mFilterMouseMove, CxCustom(PATCH_NEWFUNC(RawMouseMoveHandler), 0L));
		if (CxObjError(mFilterMouseMove))
			break;

		AttachCxObj(mFilterMouseButtons, CxCustom(PATCH_NEWFUNC(RawMouseButtonsHandler), 0L));
		if (CxObjError(mFilterMouseButtons))
			break;

		AttachCxObj(ScalosCxBroker, mFilterMouseMove);
		AttachCxObj(ScalosCxBroker, mFilterMouseButtons);

		ActivateCxObj(ScalosCxBroker, TRUE);

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		if (mFilterMouseMove)
			DeleteCxObjAll(mFilterMouseMove);
		if (mFilterMouseButtons)
			DeleteCxObjAll(mFilterMouseButtons);
		if (ScalosCxBroker)
			{
			DeleteCxObjAll(ScalosCxBroker);
			ScalosCxBroker = NULL;
			}
		}

	return Success;
///
}


void CleanupCx(void)
{
///
	if (ScalosCxBroker)
		{
		DeleteCxObjAll(ScalosCxBroker);
		ScalosCxBroker = NULL;
		}
///
}


static SAVEDS(void) INTERRUPT RawMouseMoveHandler(CxMsg *cxm, CxObj *co)
{
///
	struct InputEvent *ie;

	d1(KPrintF( "%s/%s/%ld: START CxMsg=%08lx, CxObj=%08lx\n", __FILE__, __FUNC__, __LINE__, cxm, co));

	// i KNOW that all messages getting this far are CXM_IEVENT
	ie = (struct InputEvent *) CxMsgData(cxm);

	do	{
		struct Screen *MouseScreen;

		if (NULL == iInfos.xii_iinfos.ii_Screen)
			break;

		d1(KPrintF("%s/%s/%ld: Class=%ld  SubClass=%ld  Code=%04lx\n", __FILE__, __FUNC__, __LINE__, ie->ie_Class, ie->ie_SubClass, ie->ie_Code));

		MouseScreen = SearchMouseScreen(ie->ie_position.ie_xy.ie_x, ie->ie_position.ie_xy.ie_y);

		if (MouseScreen != iInfos.xii_iinfos.ii_Screen)
			break;

		if (MouseScreen->LayerInfo.Lock.ss_NestCount != 0)
			break;

		PopScreenTitle(MouseScreen);

		if (CurrentPrefs.pref_MarkIconUnderMouse)
			HighlightIconUnderMouse();
		} while (0);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


static SAVEDS(void) INTERRUPT RawMouseButtonsHandler(CxMsg *cxm, CxObj *co)
{
///
	struct InputEvent *ie;

	d1(KPrintF("%s/%s/%ld: START CxMsg=%08lx, CxObj=%08lx\n", __FILE__, __FUNC__, __LINE__, cxm, co));

	// i KNOW that all messages getting this far are CXM_IEVENT
	ie = (struct InputEvent *) CxMsgData(cxm);

	do	{
		struct Screen *MouseScreen;
		struct ScaIconNode *iconUnderPointer;
		struct internalScaWindowTask *iwtUnderPointer;
		struct Window *foreignWindow;
		BOOL QoSuccess;

		if (NULL == iInfos.xii_iinfos.ii_Screen)
			break;

		d1(KPrintF("%s/%s/%ld: Class=%ld  SubClass=%ld  Code=%04lx\n", __FILE__, __FUNC__, __LINE__, ie->ie_Class, ie->ie_SubClass, ie->ie_Code));

		MouseScreen = SearchMouseScreen(ie->ie_position.ie_xy.ie_x, ie->ie_position.ie_xy.ie_y);

		if (MouseScreen != iInfos.xii_iinfos.ii_Screen)
			break;

		if (MouseScreen->LayerInfo.Lock.ss_NestCount != 0)
			break;

		QoSuccess = QueryObjectUnderPointer(&iwtUnderPointer, &iconUnderPointer, NULL, &foreignWindow);

		if (QoSuccess && iwtUnderPointer && (NULL == iconUnderPointer)
			&& iwtUnderPointer->iwt_ControlBar
			&& iwtUnderPointer->iwt_HighlightedControlBarGadget)
			{
			EraseGadgetClickMark(iwtUnderPointer, iwtUnderPointer->iwt_HighlightedControlBarGadget);
			iwtUnderPointer->iwt_HighlightedControlBarGadget = NULL;
			}

		if (iwtUnderPointer)
			{
			if (iconUnderPointer)
				ScalosUnLockIconList(iwtUnderPointer);
			SCA_UnLockWindowList();
			}
		} while (0);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


struct Screen *SearchMouseScreen(LONG x, LONG y)
{
///
	struct Screen *scr;
	ULONG ILock;

	d1(KPrintF( "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	Forbid();
	ILock = ScaLockIBase(0);

	for (scr = IntuitionBase->FirstScreen; scr; scr = scr->NextScreen)
		{
		d1( KPrintF("%s/%s/%ld: screen=%lx  Offsetx=%ld ", __FILE__, __FUNC__, __LINE__, scr, scr->ViewPort.DyOffset) );
		d1( KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, scr->MouseX, scr->MouseY) );

		if (scr->MouseY >= 0 && scr->MouseY < scr->Height)
			break;
		}

	ScaUnlockIBase(ILock);
	Permit();

	d1(KPrintF("%s/%s/%ld: scr=%08lx\n", __FILE__, __FUNC__, __LINE__, scr));

	return scr;
///
}


void HighlightIconUnderMouse(void)
{
///
	struct ScaIconNode *iconUnderPointer;
	struct internalScaWindowTask *iwtUnderPointer;
	struct Window *foreignWindow;
	static struct internalScaWindowTask *LastIwtUnderPointer = NULL;
	static struct ScaIconNode *LastIconUnderPointer = NULL;
	BOOL QoSuccess;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	QoSuccess = QueryObjectUnderPointer(&iwtUnderPointer, &iconUnderPointer, NULL, &foreignWindow);

	d1(KPrintF("%s/%s/%ld: QoSuccess=%ld  iconUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, QoSuccess, iconUnderPointer));

	HighlightControlbarGadgets(QoSuccess, iwtUnderPointer, iconUnderPointer);

	if (QoSuccess && LastIconUnderPointer != iconUnderPointer)
		{
		d1(KPrintF("%s/%s/%ld: iconUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, iconUnderPointer));

		if (LastIconUnderPointer)
			{
			// Remove highlight from previously marked icon
			struct ScaWindowStruct *ws;
			BOOL Found = FALSE;

			SCA_LockWindowList(SCA_LockWindowList_Shared);

			// check if LastIwtUnderPointer still exists
			for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
				{
				if (ws->ws_WindowTask == &LastIwtUnderPointer->iwt_WindowTask)
					{
					Found = TRUE;
					break;
					}
				}

			if (Found)
				{
				// make sure the specified icon is still present
				if (ScalosAttemptLockIconListShared(LastIwtUnderPointer))
					{
					struct ScaIconNode *in;

					for (in = LastIwtUnderPointer->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
						{
						if (in == LastIconUnderPointer)
							{
							struct SM_RedrawIconObj *sMsg;

							sMsg = (struct SM_RedrawIconObj *) SCA_AllocMessage(MTYP_RedrawIconObj, 0);
							d1(KPrintF("%s/%s/%ld: sMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, sMsg));
							if (sMsg)
								{
								sMsg->smrio_IconObject = LastIconUnderPointer->in_Icon;
								sMsg->smrio_Flags = SMRIOFLAGF_Highlight | SMRIOFLAGF_HightlightOff;
								PutMsg(LastIwtUnderPointer->iwt_WindowTask.wt_IconPort,
									&sMsg->ScalosMessage.sm_Message);
								}
							break;
							}
						}

					ScalosUnLockIconList(LastIwtUnderPointer);
					}
				}

			SCA_UnLockWindowList();
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (iconUnderPointer)
			{
			// ignore icon under pointer if the active window is not a Scalos window
			if (NULL == FindActiveScalosWindow())
				{
				ScalosUnLockIconList(iwtUnderPointer);
				iconUnderPointer = NULL;
				}
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (iconUnderPointer)
			{
			// Highlight icon under mouse
			struct SM_RedrawIconObj *sMsg;

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			sMsg = (struct SM_RedrawIconObj *) SCA_AllocMessage(MTYP_RedrawIconObj, 0);
			d1(KPrintF("%s/%s/%ld: sMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, sMsg));
			if (sMsg)
				{
				sMsg->smrio_IconObject = iconUnderPointer->in_Icon;
				sMsg->smrio_Flags = SMRIOFLAGF_Highlight | SMRIOFLAGF_HightlightOn;
				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				PutMsg(iwtUnderPointer->iwt_WindowTask.wt_IconPort,
					&sMsg->ScalosMessage.sm_Message);
				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				}
			}

		LastIwtUnderPointer = iwtUnderPointer;
		LastIconUnderPointer = iconUnderPointer;
		}

	if (iwtUnderPointer)
		{
		if (iconUnderPointer)
			ScalosUnLockIconList(iwtUnderPointer);
		SCA_UnLockWindowList();
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


// Make sure we have no dangling LastIwtUnderPointer pointer after window has been closed!
void InvalidateLastIwtUnderPointer(struct internalScaWindowTask *iwt)
{
	if (LastIwtUnderPointer == iwt)
		LastIwtUnderPointer = NULL;
}


static void HighlightControlbarGadgets(BOOL QoSuccess,
	struct internalScaWindowTask *iwtUnderPointer, struct ScaIconNode *iconUnderPointer)
{
	struct ExtGadget *ggOld = NULL;
	struct ExtGadget *ggNew = NULL;
	struct internalScaWindowTask *iwtOld;
///
	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	d1(KPrintF("%s/%s/%ld: QoSuccess=%ld  iconUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, QoSuccess, iconUnderPointer));
	d1(KPrintF("%s/%s/%ld: iwtUnderPointer=%08lx  LastIwtUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtUnderPointer, LastIwtUnderPointer));

	if (iwtUnderPointer == LastIwtUnderPointer)
		{
		if (iwtUnderPointer)
			ggOld = iwtUnderPointer->iwt_HighlightedControlBarGadget;
		iwtOld = iwtUnderPointer;
		}
	else
		{
		if (LastIwtUnderPointer)
			ggOld = LastIwtUnderPointer->iwt_HighlightedControlBarGadget;
		iwtOld = LastIwtUnderPointer;

		LastIwtUnderPointer = iwtUnderPointer;
		}

	if (QoSuccess && iwtUnderPointer && NULL == iconUnderPointer && iwtUnderPointer->iwt_ControlBar)
		{
		WORD MouseX = iInfos.xii_iinfos.ii_Screen->MouseX;
		WORD MouseY = iInfos.xii_iinfos.ii_Screen->MouseY;
		UWORD Code;

		MouseX -= iwtUnderPointer->iwt_WindowTask.wt_Window->LeftEdge;
		MouseY -= iwtUnderPointer->iwt_WindowTask.wt_Window->TopEdge;

		Code = StatusBarQueryGadgetID(iwtUnderPointer,
			(struct ExtGadget *) iwtUnderPointer->iwt_ControlBar, MouseX, MouseY);

		d1(KPrintF("%s/%s/%ld: MouseX=%ld  MouseY=%ld  Code=%lu\n", __FILE__, __FUNC__, __LINE__, MouseX, MouseY, Code));

		ggNew = ControlBarFindGadget(iwtUnderPointer, Code);
		d1(KPrintF("%s/%s/%ld: ggNew=%08lx\n", __FILE__, __FUNC__, __LINE__, ggNew));

		if (ggNew)
			{
			if ((ggNew->Flags & GFLG_DISABLED)
				|| !(ggNew->Activation & (GACT_RELVERIFY | GACT_IMMEDIATE))
				|| (ggNew->Activation & GACT_ACTIVEGADGET))
				ggNew = NULL;
			}
		}
	else
		{
		}

	d1(KPrintF("%s/%s/%ld: ggOld=%08lx  ggNew=%08lx  iwtOld=%08lx\n", __FILE__, __FUNC__, __LINE__, ggOld, ggNew, iwtOld));

	if (ggOld != ggNew)
		{
		if (ggOld && iwtOld)
			{
			EraseGadgetClickMark(iwtOld, ggOld);
			iwtOld->iwt_HighlightedControlBarGadget = NULL;
			}
		if (ggNew)
			{
			DisplayGadgetClickMark(iwtUnderPointer, ggNew);
			iwtUnderPointer->iwt_HighlightedControlBarGadget = ggNew;
			}
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


static SAVEDS(void) PopScreenTitle(const struct Screen *MouseScreen)
{
///
	static UBYTE OnCount;
	BOOL BarVisible = (MouseScreen->Flags & SHOWTITLE);

	d1(KPrintF("%s/%s/%ld: MouseScreen=%08lx  Flags=%08lx  BarLayer=%08lx  BarVisible=%04lx\n", \
		__FILE__, __FUNC__, __LINE__, MouseScreen, MouseScreen->Flags, MouseScreen->BarLayer, BarVisible));

	if (CurrentPrefs.pref_PopTitleFlag)
		{
		if (MouseScreen->MouseY >= 0 && MouseScreen->MouseY < MouseScreen->BarHeight)
			{
			OnCount = 0;

			if (!BarVisible)
				{
				struct SM_ShowTitle *sMsg;

				d1(KPrintF("%s/%s/%ld: Screen Title ON\n", __FILE__, __FUNC__, __LINE__));

				sMsg = (struct SM_ShowTitle *) SCA_AllocMessage(MTYP_ShowTitle, 0);
				if (sMsg)
					{
					sMsg->smst_showTitle = TITLEBAR_ON;
					PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &sMsg->ScalosMessage.sm_Message);
					}
				}
			}
		else
			{
			if (BarVisible || OnCount < 3)
				{
				struct SM_ShowTitle *sMsg;

				d1(KPrintF("%s/%s/%ld: Screen Title OFF\n", __FILE__, __FUNC__, __LINE__));

				sMsg = (struct SM_ShowTitle *) SCA_AllocMessage(MTYP_ShowTitle, 0);
				if (sMsg)
					{
					sMsg->smst_showTitle = TITLEBAR_OFF;
					PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &sMsg->ScalosMessage.sm_Message);
					}

				OnCount++;
				}
			}
		}
	else
		{
		// Screen title is always on or always off

		if (CurrentPrefs.pref_FullBenchFlag)
			{
			// Screen title should be always off
			if (BarVisible)
				{
				struct SM_ShowTitle *sMsg;

				d1(KPrintF("%s/%s/%ld: Screen Title OFF\n", __FILE__, __FUNC__, __LINE__));

				sMsg = (struct SM_ShowTitle *) SCA_AllocMessage(MTYP_ShowTitle, 0);
				if (sMsg)
					{
					sMsg->smst_showTitle = TITLEBAR_OFF;
					PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &sMsg->ScalosMessage.sm_Message);
					}
				}
			}
		else
			{
			// Screen title should be always on
			if (!BarVisible)
				{
				struct SM_ShowTitle *sMsg;

				d1(KPrintF("%s/%s/%ld: Screen Title ON\n", __FILE__, __FUNC__, __LINE__));

				sMsg = (struct SM_ShowTitle *) SCA_AllocMessage(MTYP_ShowTitle, 0);
				if (sMsg)
					{
					sMsg->smst_showTitle = TITLEBAR_ON;
					PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &sMsg->ScalosMessage.sm_Message);
					}
				}
			}
		}
///
}


