// idcmp.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <datatypes/pictureclass.h>
#include <dos/dostags.h>
#include <guigfx/guigfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/guigfx.h>
#include <proto/datatypes.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/palette.h>
#include <scalos/pattern.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

#include "IconImageClass.h"
#include "CycleGadgetClass.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

static ULONG IDCMPMouseButtons(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPMouseMove(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPGadgetDown(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPGadgetUp(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPMenuPick(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPDiskRemoved(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPActiveWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPInactiveWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPRawKey(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPUpdate(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPIntuiTicks(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPChangeWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPNewSize(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static ULONG IDCMPGadgetHelp(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
#if defined(IDCMP_EXTENDEDMOUSE)
static ULONG IDCMPExtendedMouse(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
#endif //defined(IDCMP_EXTENDEDMOUSE)

static void ArrowRight(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static void ArrowLeft(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static void ArrowDown(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static void ArrowUp(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static void SetPropBottom(struct internalScaWindowTask *iwt);
static void SetPropSide(struct internalScaWindowTask *iwt);
static void MousePress(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static void MouseRelease(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
static void OldIconsOff(struct internalScaWindowTask *iwt, Object *IconObj, 
	LONG XOffset, LONG YOffset, UWORD Qualifier, BOOL keepSelected);
static LONG SafeStrCmp(CONST_STRPTR str1, CONST_STRPTR str2);
static void IDCMPBeginDrag(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);

//----------------------------------------------------------------------------

// local data items

//----------------------------------------------------------------------------

// public data items

const struct IDCMPTableEntry IDCMPFuncTable[] =
	{
	// Attention: entries MUST be kept in ascending order, due to use of bsearch()
//	{ IDCMP_SIZEVERIFY,	NULL 			},
	{ IDCMP_NEWSIZE,	IDCMPNewSize		},
	{ IDCMP_REFRESHWINDOW,	IDCMPRefreshWindow	},
	{ IDCMP_MOUSEBUTTONS,	IDCMPMouseButtons	},
	{ IDCMP_MOUSEMOVE,	IDCMPMouseMove		},
	{ IDCMP_GADGETDOWN,	IDCMPGadgetDown		},
	{ IDCMP_GADGETUP,	IDCMPGadgetUp		},
//	{ IDCMP_REQSET,		NULL			},
	{ IDCMP_MENUPICK,	IDCMPMenuPick		},
//	{ IDCMP_CLOSEWINDOW,	NULL			},
	{ IDCMP_RAWKEY,		IDCMPRawKey		},
//	{ IDCMP_REQVERIFY,	NULL			},
//	{ IDCMP_REQCLEAR,	NULL			},
//	{ IDCMP_MENUVERIFY,	NULL			},
//	{ IDCMP_NEWPREFS,	NULL			},
	{ IDCMP_DISKINSERTED,	IDCMPDiskInserted	},
	{ IDCMP_DISKREMOVED,	IDCMPDiskRemoved	},
//	{ IDCMP_WBENCHMESSAGE,	NULL			},
	{ IDCMP_ACTIVEWINDOW,	IDCMPActiveWindow	},
	{ IDCMP_INACTIVEWINDOW,	IDCMPInactiveWindow	},
//	{ IDCMP_DELTAMOVE,	NULL			},
//	{ IDCMP_VANILLAKEY,	NULL			},
	{ IDCMP_INTUITICKS,	IDCMPIntuiTicks		},
	{ IDCMP_IDCMPUPDATE,	IDCMPUpdate	      	},
//	{ IDCMP_MENUHELP,	NULL			},
	{ IDCMP_CHANGEWINDOW,	IDCMPChangeWindow	},
	{ IDCMP_GADGETHELP,	IDCMPGadgetHelp		},
//	{ IDCMP_LONELYMESSAGE,	NULL			},
#if defined(IDCMP_EXTENDEDMOUSE)
	{ IDCMP_EXTENDEDMOUSE, IDCMPExtendedMouse },
#endif //defined(IDCMP_EXTENDEDMOUSE)
	};

const size_t IDCMPFuncTableSize = Sizeof(IDCMPFuncTable);       // number of entries in IDCMPFuncTable[

//----------------------------------------------------------------------------

ULONG IDCMPRefreshWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(KPrintF("%s/%s/%ld: START  iwt=%08lx  <%s>  IDCMPWindow=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iMsg->IDCMPWindow));

	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
		{
		iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		return 0;
		}

	BeginRefresh(iwt->iwt_WindowTask.wt_Window);

	if (iwt->iwt_TextGadFlag)
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);

	if (iwt->iwt_WindowTask.wt_IconList)
		{
		struct Rectangle winRect;

		winRect.MinX = winRect.MaxX = iwt->iwt_InnerLeft;
		winRect.MaxX += iwt->iwt_InnerWidth - 1;
		winRect.MinY = winRect.MaxY = iwt->iwt_InnerTop;
		winRect.MaxY += iwt->iwt_InnerHeight - 1;

		d1(kprintf("%s/%s/%ld: WinRect MinX=%ld MinY=%ld  MaxX=%ld  MaxY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, winRect.MinX, winRect.MinY, winRect.MaxX, winRect.MaxY));

		if (NULL == iwt->iwt_DamagedRegion)
			iwt->iwt_DamagedRegion = NewRegion();

		if (iwt->iwt_DamagedRegion)
			{
			OrRegionRegion(iwt->iwt_WindowTask.wt_Window->WLayer->DamageList, iwt->iwt_DamagedRegion);
			AndRectRegion(iwt->iwt_DamagedRegion, &winRect);

			d1(KPrintF("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

			if (ScalosAttemptLockIconListShared(iwt))
				{
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_RefreshPending;

				RefreshIcons(iwt, iwt->iwt_DamagedRegion);

				DisposeRegion(iwt->iwt_DamagedRegion);
				iwt->iwt_DamagedRegion = NULL;

				ScalosUnLockIconList(iwt);
				}
			else
				{
				// Postpone refresh until next SCCM_Ping
				d1(KPrintF("%s/%s/%ld: postpone refresh  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_RefreshPending;
				}
			}
		}

	d1(KPrintF("%s/%s/%ld: WLayer=%08lx  Flags=%04lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window->WLayer,
		iwt->iwt_WindowTask.wt_Window->WLayer->Flags));

#if defined(__MORPHOS__)
	if (!iInfos.xii_Layers3D)
		RefreshWindowFrame(iwt->iwt_WindowTask.wt_Window);
#endif /* __MORPHOS__ */

	EndRefresh(iwt->iwt_WindowTask.wt_Window, TRUE);

	d1(kprintf("%s/%s/%ld: END  iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 0;
}


static ULONG IDCMPMouseButtons(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	iwt->iwt_LockCount.tv_secs = iMsg->Seconds;
	iwt->iwt_LockCount.tv_micro = iMsg->Micros;

	d1(KPrintF("%s/%s/%ld: START Code=%04lx\n", __FILE__, __FUNC__, __LINE__, iMsg->Code));

	ResetToolTips(iwt);

	switch (iMsg->Code)
		{
	case SELECTDOWN:
		d1(KPrintF("%s/%s/%ld: SELECTDOWN\n", __FILE__, __FUNC__, __LINE__));
		MousePress(iwt, iMsg);
		break;

	case SELECTUP:
		d1(KPrintF("%s/%s/%ld: SELECTUP\n", __FILE__, __FUNC__, __LINE__));
		MouseRelease(iwt, iMsg);
		break;

	case MIDDLEDOWN:
		d1(KPrintF("%s/%s/%ld: MIDDLEDOWN\n", __FILE__, __FUNC__, __LINE__));
		if (CurrentPrefs.pref_MMBMoveFlag)
			{
			AbortFunctions(iwt);

			if (!iwt->iwt_BackDrop)
				{
				iwt->iwt_MoveGadId = VGADGETID_MMBMOUSEMOVE;

				iwt->iwt_OldMouse.X = iMsg->MouseX;
				iwt->iwt_OldMouse.Y = iMsg->MouseY;

				ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_MOUSEMOVE);
				}
			}
		break;

	case MIDDLEUP:
		d1(KPrintF("%s/%s/%ld: MIDDLEUP\n", __FILE__, __FUNC__, __LINE__));
		if (CurrentPrefs.pref_MMBMoveFlag)
			{
			iwt->iwt_MoveGadId = VGADGETID_IDLE;

			ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);
			}
		break;
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG IDCMPMouseMove(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	ResetToolTips(iwt);

	iInfos.xii_GlobalGadgetUnderPointer.ggd_iwt = NULL;
	iInfos.xii_GlobalGadgetUnderPointer.ggd_Gadget = NULL;
	iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_unknown;
	iInfos.xii_GlobalGadgetUnderPointer.ggd_cgy = NULL;
	iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetTextHook = NULL;

	switch (iwt->iwt_MoveGadId)
		{
	case VGADGETID_WAITFORDRAG:
		IDCMPBeginDrag(iwt, iMsg);
		break;

	case VGADGETID_DRAGMOUSEMOVE:
		IDCMPDragMouseMove(iwt, iMsg);
		break;

	case VGADGETID_MMBMOUSEMOVE:
		IDCMPMMBMouseMove(iwt);
		break;

	case GADGETID_HORIZSLIDER:
		SetPropBottom(iwt);
		break;

	case GADGETID_VERTSLIDER:
		SetPropSide(iwt);
		break;

	case VGADGETID_LASSO:
		if (iwt->iwt_LassoFlag)
			{
			Lasso(iwt->iwt_OldMouse.X, iwt->iwt_OldMouse.Y, LASSOX_FollowLasso, iwt);
			}
		else
			{
			WORD deltaX, deltaY;

			deltaX = iMsg->MouseX - iwt->iwt_OldMouse.X;
			deltaY = iMsg->MouseY - iwt->iwt_OldMouse.Y;

			if (abs(deltaX) > 8 || abs(deltaY) > 8)
				BeginLasso(iwt, iMsg->Qualifier);
			}
		break;
		}

	return 0;
}


static ULONG IDCMPGadgetDown(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	struct ExtGadget *gad;
//	UWORD Code;

	ResetToolTips(iwt);

	if (NULL == iwt->iwt_WindowTask.wt_IconList)
		{
		iwt->iwt_MoveGadId = VGADGETID_IDLE;
		return 0;
		}

	gad = (struct ExtGadget *) iMsg->IAddress;
	iwt->iwt_MoveGadId = gad->GadgetID;

	switch (iwt->iwt_MoveGadId)
		{
	case GADGETID_HORIZSLIDER:
	case GADGETID_VERTSLIDER:
		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_MOUSEMOVE | IDCMP_INTUITICKS);
		break;

	case GADGETID_RIGHTBUTTON:
		ArrowRight(iwt, iMsg);
		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_INTUITICKS);
		break;

	case GADGETID_LEFTBUTTON:
		ArrowLeft(iwt, iMsg);
		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_INTUITICKS);
		break;

	case GADGETID_UPBUTTON:
		ArrowUp(iwt, iMsg);
		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_INTUITICKS);
		break;

	case GADGETID_DOWNBUTTON:
		ArrowDown(iwt, iMsg);
		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_INTUITICKS);
		break;

	case GADGETID_CONTROLBAR:
		//Code = StatusBarQueryGadgetID(iwt, gad, iMsg->MouseX, iMsg->MouseY);
		d1(KPrintF("%s/%s/%ld: GADGETID_CONTROLBAR  Code=%ld\n", __FILE__, __FUNC__, __LINE__, Code));
		break;

	case GADGETID_STATUSBAR:
		//Code = StatusBarQueryGadgetID(iwt, gad, iMsg->MouseX, iMsg->MouseY);
		d1(KPrintF("%s/%s/%ld: GADGETID_STATUSBAR  Code=%ld\n", __FILE__, __FUNC__, __LINE__, Code));
		break;

	
        default:
		d1(KPrintF("%s/%s/%ld: unknown GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, gad->GadgetID));
		break;
		}

	return 0;
}


static ULONG IDCMPGadgetUp(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	struct ExtGadget *gad = (struct ExtGadget *) iMsg->IAddress;
	UWORD Code;

	ResetToolTips(iwt);

	d1(KPrintF("%s/%s/%ld: >GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, gad->GadgetID));

	switch (gad->GadgetID)
		{
	case GADGETID_HORIZSLIDER:
		d1(KPrintF("%s/%s/%ld: GADGETID_HORIZSLIDER\n", __FILE__, __FUNC__, __LINE__));
		SetPropBottom(iwt);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustBottomSlider);
		break;

	case GADGETID_VERTSLIDER:
		d1(KPrintF("%s/%s/%ld: GADGETID_VERTSLIDER\n", __FILE__, __FUNC__, __LINE__));
		SetPropSide(iwt);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider);
		break;

	case GADGETID_ICONIFY:
	case ETI_Iconify:
		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);
		ReplyMsg(&iMsg->ExecMessage);
		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Iconify);

		iwt->iwt_MoveGadId = VGADGETID_IDLE;

		return 1;
		break;

	case GADGETID_CONTROLBAR:
		d1(KPrintF("%s/%s/%ld: GADGETID_CONTROLBAR\n", __FILE__, __FUNC__, __LINE__));
		Code = ControlBarQueryGadgetType(iwt, gad);
		d1(KPrintF("%s/%s/%ld: code=%ld\n", __FILE__, __FUNC__, __LINE__, code));
		switch (Code)
			{
		case SCPGadgetType_BrowseButton:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_BrowseButton\n", __FILE__, __FUNC__, __LINE__));
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Browse);
			break;
		case SCPGadgetType_UpButton:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_UpButton\n", __FILE__, __FUNC__, __LINE__));
			if (!(iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_RootWindow)
				&& iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
				{
				BPTR parentLock;

				parentLock = ParentDir(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
				debugLock_d1(parentLock);
				if (parentLock)
					{
					STRPTR ParentName;

					ParentName = AllocPathBuffer();
					d1(KPrintF("%s/%s/%ld: ParentName=%08lx\n", __FILE__, __FUNC__, __LINE__, ParentName));
					if (ParentName)
						{
						if (NameFromLock(parentLock, ParentName, Max_PathLen - 1))
							{
							d1(KPrintF("%s/%s/%ld: ParentName=<%s>\n", __FILE__, __FUNC__, __LINE__, ParentName));
							WindowNewPath(iwt, ParentName);
							}

						FreePathBuffer(ParentName);
						}

					UnLock(parentLock);
					}
				}
			break;
		case SCPGadgetType_BackButton:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_BackButton\n", __FILE__, __FUNC__, __LINE__));
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_History_Back);
			break;
		case SCPGadgetType_ForwardButton:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ForwardButton\n", __FILE__, __FUNC__, __LINE__));
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_History_Forward);
			break;
		case SCPGadgetType_ViewBy:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ViewBy  Code=%ld\n", __FILE__, __FUNC__, __LINE__, iMsg->Code));
			ViewWindowBy(iwt, ControlBarViewModeFromCode(iMsg->Code));
			break;
		case SCPGadgetType_ShowMode:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ShowMode  Code=%ld\n", __FILE__, __FUNC__, __LINE__, iMsg->Code));
			SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
				SCCA_IconWin_ShowType, (0 == iMsg->Code) ? DDFLAGS_SHOWICONS : DDFLAGS_SHOWALL,
				TAG_END);
			break;
		case SCPGadgetType_History:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_History  Code=%ld\n", __FILE__, __FUNC__, __LINE__, iMsg->Code));
			ControlBarSwitchHistoryEntry(iwt);
			break;
		case SCPGadgetType_ActionButton:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ActionButton  Code=%ld\n", __FILE__, __FUNC__, __LINE__, iMsg->Code));
			ControlBarActionButton(iwt, gad);
			break;
		default:
			d1(KPrintF("%s/%s/%ld: GADGETID_CONTROLBAR  unknown Code=%ld\n", __FILE__, __FUNC__, __LINE__, Code));
			break;
			}
		break;

	case GADGETID_STATUSBAR:
		//Code = StatusBarQueryGadgetID(iwt, gad, iMsg->MouseX, iMsg->MouseY);
		d1(KPrintF("%s/%s/%ld: GADGETID_STATUSBAR  Code=%ld\n", __FILE__, __FUNC__, __LINE__, Code));
		break;

	default:
		d1(KPrintF("%s/%s/%ld: unknown GadgetID=%ld  Code=%ld\n", __FILE__, __FUNC__, __LINE__, gad->GadgetID, iMsg->Code));
		break;
		}

	ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);
	iwt->iwt_MoveGadId = VGADGETID_IDLE;

	return 0;
}


static ULONG IDCMPMenuPick(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	WORD code = iMsg->Code;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx iMsg=%08lx  code=%04lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iMsg, code));

	ReplyMsg(&iMsg->ExecMessage);

	while ((WORD) MENUNULL != code)
		{
		struct MenuItem *mItem = ItemAddress(MainMenu, code);
		struct ScalosMenuTree *mTree;

		if (NULL == mItem)
			break;

		mTree = (struct ScalosMenuTree *) GTMENUITEM_USERDATA(mItem);
		if (mTree)
			{
			struct AppObject *appo = (struct AppObject *) mTree;

			if (ID_SC == appo->appo_Kennung)
				{
				struct AppMessage *appMsg;
				ULONG NumberOfWbArgs;
				ULONG MsgSize;

				NumberOfWbArgs = SCA_CountWBArgs(NULL);
				MsgSize = sizeof(struct AppMessage) + NumberOfWbArgs * sizeof(struct WBArg);

				appMsg = AllocVec(MsgSize, MEMF_CLEAR | MEMF_PUBLIC);
				if (appMsg)
					{
					appMsg->am_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
					appMsg->am_Message.mn_Length = MsgSize;
					appMsg->am_Type = AMTYPE_APPMENUITEM;
					appMsg->am_UserData = appo->appo_userdata;
					appMsg->am_ID = appo->appo_id;
					appMsg->am_Version = AM_VERSION;
					appMsg->am_MouseX = iwt->iwt_WindowTask.wt_Window->MouseX;
					appMsg->am_MouseY = iwt->iwt_WindowTask.wt_Window->MouseY;
					CurrentTime(&appMsg->am_Seconds, &appMsg->am_Micros);
					appMsg->am_Reserved[0] = ID_APPM;
					appMsg->am_ArgList = (struct WBArg *) (appMsg + 1);	// follows directly after appMsg

					appMsg->am_NumArgs = SCA_MakeWBArgs(appMsg->am_ArgList, NULL, NumberOfWbArgs);

					if (0 == appMsg->am_NumArgs)
						appMsg->am_ArgList = NULL;

					PutMsg(appo->appo_msgport, &appMsg->am_Message);
					}
				}
			else
				{
				RunMenuCommand(iwt, mTree, NULL, (mItem->Flags & CHECKED) ? 1 : 0);
				}
			}

		code = mItem->NextSelect;
		}

	return 1;
}


ULONG IDCMPDiskInserted(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(KPrintF("%s/%s/%ld: START iwt_DevListObject=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DevListObject));

	if (iwt->iwt_DevListObject)
		{
		struct ScaDeviceIcon *nodeList = NULL;
		struct ScaDeviceIcon *di, *diNext;
		struct ScaIconNode *iconList = NULL;
		BOOL IsReadOnly;

		DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_Generate, &nodeList);

		for (di=iwt->iwt_DevIconList; di; di = (struct ScaDeviceIcon *) di->di_Node.mln_Succ)
			{
			di->di_Flags |= DIBF_Remove;
			}

		for (di = nodeList; di; di = diNext)
			{
			struct ScaDeviceIcon *di2, *diNext2;

			diNext = (struct ScaDeviceIcon *) di->di_Node.mln_Succ;

			d1(kprintf("%s/%s/%ld: di=%08lx  <%s>  diNext=%08lx  di_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, di, di->di_Device, diNext, di->di_Flags));

			for (di2 = iwt->iwt_DevIconList; di2; di2 = diNext2)
				{
				diNext2 = (struct ScaDeviceIcon *) di2->di_Node.mln_Succ;

				if (di->di_Handler == di2->di_Handler)
					{
					d1(kprintf("%s/%s/%ld: di2=%08lx  <%s>  DiskType=%08lx  di_Flags=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, di2, di2->di_Device, di2->di_Info->id_DiskType, di2->di_Flags));

					if (di->di_Flags & DIBF_DontDisplay)
						{
						// device di2 is no longer valid
						// DIBF_Remove stays set
						d1(kprintf("%s/%s/%ld: invalid di2=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, di2, di2->di_Device));
						}
					else if (di2->di_Flags & DIBF_DontDisplay && !(di->di_Flags & DIBF_DontDisplay))
						{
						d1(kprintf("%s/%s/%ld: Add di2=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, di2, di2->di_Device));

						di2->di_Flags &= ~(DIBF_DontDisplay | DIBF_Remove);
						RemoveDeviceIcon(iwt, di2, &iconList);

						DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_FreeDevNode, di2);
						SCA_FreeNode((struct ScalosNodeList *)(APTR) &iwt->iwt_DevIconList, &di2->di_Node);
						}
					else
						{
						if (0 == SafeStrCmp(di->di_Volume, di2->di_Volume)
							&& di->di_Info->id_DiskType == di2->di_Info->id_DiskType)
							{
							// device is already present in iwt_DevIconList
							// and there are no changes.
							// remove new devicenode from nodeList

							di2->di_Flags &= ~DIBF_Remove;

							d1(kprintf("%s/%s/%ld: Remove di=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, di, di->di_Device));

							DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_FreeDevNode, di);
							SCA_FreeNode((struct ScalosNodeList *)(APTR) &nodeList, &di->di_Node);

							d1(kprintf("%s/%s/%ld: di=%08lx  diNext=%08lx\n", __FILE__, __FUNC__, __LINE__, di, diNext));
							break;
							}
						else
							{
							// device seems to have changed
							// move devicenode to iconlist and remember it for re-reading

							d1(kprintf("%s/%s/%ld: Remove di2=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, di2, di2->di_Device));
							RemoveDeviceIcon(iwt, di2, &iconList);

							d1(kprintf("%s/%s/%ld: di=%08lx  diNext=%08lx\n", __FILE__, __FUNC__, __LINE__, di, diNext));
							}
						}
					}
				d1(kprintf("%s/%s/%ld: di=%08lx  diNext=%08lx\n", __FILE__, __FUNC__, __LINE__, di, diNext));
				}
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		for (di = iwt->iwt_DevIconList; di; di = diNext)
			{
			diNext = (struct ScaDeviceIcon *) di->di_Node.mln_Succ;

			d1(kprintf("%s/%s/%ld: di=%08lx  <%s>  diNext=%08lx  di_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, di, di->di_Device, diNext, di->di_Flags));
			
			if (di->di_Flags & DIBF_Remove)
				{
				d1(kprintf("%s/%s/%ld: Remove icon <%s>\n", __FILE__, __FUNC__, __LINE__, di->di_Device));
				RemoveDeviceIcon(iwt, di, &iconList);
				}
			}

		d1(kprintf("%s/%s/%ld: iconList=%08lx\n", __FILE__, __FUNC__, __LINE__, iconList));

		if (iconList)
			{
			RemoveIcons(iwt, &iconList);
			FreeIconList(iwt, &iconList);
			}

		d1(\
		{\
			struct ScaIconNode *in;

			kprintf("%s/%s/%ld:  --------start----------\n", __FILE__, __FUNC__, __LINE__);

			for (in=iwt->iwt_WindowTask.wt_IconList; in; in=(struct ScaIconNode *) in->in_Node.mln_Succ)
				kprintf("%s/%s/%ld:  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in));

			kprintf("%s/%s/%ld:  ---------end----------\n", __FILE__, __FUNC__, __LINE__);
		});

		d1(kprintf("%s/%s/%ld: nodeList=%08lx\n", __FILE__, __FUNC__, __LINE__, nodeList));

		if (nodeList)
			{
			for (di = nodeList; di; di = diNext)
				{
				diNext = (struct ScaDeviceIcon *) di->di_Node.mln_Succ;

				d1(kprintf("%s/%s/%ld: di=%08lx  <%s>  diNext=%08lx  di_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, di, di->di_Device, diNext, di->di_Flags));

				if (!(di->di_Flags & (DIBF_DontDisplay | DIBF_InfoPending)))
					{
					d1(kprintf("%s/%s/%ld: add icon di=%08lx  <%s>  disktype=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, di, di->di_Device, di->di_Info->id_DiskType));
					DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_DeviceWin_ReadIcon, di);

					SCA_MoveDevIconNode(&nodeList, &iwt->iwt_DevIconList, di);
					}
				}

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
			}

		// Now remove all unused entries from nodeList
		while (nodeList)
			{
			d1(kprintf("%s/%s/%ld: free entry  nodeList=%08lx  <%s>  di_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, nodeList, nodeList->di_Device, nodeList->di_Flags));
			DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_FreeDevNode, nodeList);
			SCA_FreeNode((struct ScalosNodeList *)(APTR) &nodeList, &nodeList->di_Node);
			}

		if (WSV_Type_DeviceWindow != iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType)
			{
				struct FileLock *fLock = BADDR(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

			iwt->iwt_OldFileSys = SetFileSysTask(fLock->fl_Task);

			IsReadOnly = !ClassIsDiskWritable(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

			if (IsReadOnly != iwt->iwt_ReadOnly)
				{
				SetIconWindowReadOnly(iwt, IsReadOnly);
				iwt->iwt_ReadOnly = IsReadOnly;
				}
			}
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG IDCMPDiskRemoved(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (!iwt->iwt_BackDrop)
		{
		iwt->iwt_WinTitle = (STRPTR) DoMethod(iwt->iwt_TitleObject, SCCM_Title_Generate);
		DoMethod(iwt->iwt_TitleObject, SCCM_Window_SetTitle, iwt->iwt_WinTitle);
		}

	return 0;
}


static ULONG IDCMPActiveWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	ULONG Result = 0;

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_ActivatePending)
		{
		d1(kprintf("%s/%s/%ld: iwt_PopupMenuPending=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_PopupMenuPending));

		if (iwt->iwt_PopupMenuPending)
			{
			ReplyMsg(&iMsg->ExecMessage);
			Result = 1;

			// Now show pending popup menu
			DoMethodA(iwt->iwt_WindowTask.mt_MainObject, (Msg) iwt->iwt_PopupMenuPending);

			ScalosFree(iwt->iwt_PopupMenuPending);
			iwt->iwt_PopupMenuPending = NULL;
			}
		}

	SetAttrs(iwt->iwt_WindowTask.mt_WindowObject,
		SCCA_Window_Transparency, iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityActive,
		TAG_END);

	iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_ActivatePending;

	SetMenuOnOff(iwt);

	return Result;
}


static ULONG IDCMPInactiveWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	SetAttrs(iwt->iwt_WindowTask.mt_WindowObject,
		SCCA_Window_Transparency, iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityInactive,
		TAG_END);

	return 0;
}


static ULONG IDCMPRawKey(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_RawKey, iMsg);

	return 0;
}


static ULONG IDCMPUpdate(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	ULONG GadgetID;
	struct ControlBarGadgetEntry *cgy;

	d1(KPrintF("%s/%s/%ld: Class=%08lx  Code=%ld  IAddress=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iMsg->Class, iMsg->Code, iMsg->IAddress));

	GadgetID = GetTagData(GA_ID, 0, iMsg->IAddress);
	d1(KPrintF("%s/%s/%ld: GadgetID=%ld  Code=%ld\n", __FILE__, __FUNC__, __LINE__, GadgetID, iMsg->Code));

	cgy = ControlBarFindGadgetByID(iwt, GadgetID);
	d1(KPrintF("%s/%s/%ld: cgy=%08lx  cgy_GadgetType=%ld\n", __FILE__, __FUNC__, __LINE__, cgy, cgy ? cgy->cgy_GadgetType : 0));
	if (cgy)
		{
		ULONG Code;

		Code = GetTagData(SCACYCLE_Active, 0, iMsg->IAddress);

		switch (cgy->cgy_GadgetType)
			{
		case SCPGadgetType_ViewBy:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ViewBy  Code=%ld\n", __FILE__, __FUNC__, __LINE__, Code));
			ViewWindowBy(iwt, ControlBarViewModeFromCode(Code));
			break;
		case SCPGadgetType_ShowMode:
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ShowMode  Code=%ld\n", __FILE__, __FUNC__, __LINE__, Code));
			SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
				SCCA_IconWin_ShowType, (0 == Code) ? DDFLAGS_SHOWICONS : DDFLAGS_SHOWALL,
				TAG_END);
			break;
		default:
			d1(KPrintF("%s/%s/%ld: unknown GadgetID=%ld  Code=%ld\n", __FILE__, __FUNC__, __LINE__, GadgetID, Code));
			break;
			}
		}


	return 0;
}


// IntuiTicks are issued about 10 times per second
static ULONG IDCMPIntuiTicks(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	iwt->iwt_LockCount.tv_secs = iMsg->Seconds;
	iwt->iwt_LockCount.tv_micro = iMsg->Micros;

	d1(KPrintF("%s/%s/%ld: Class=%08lx  Code=%ld  IAddress=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iMsg->Class, iMsg->Code, iMsg->IAddress));

	switch (iwt->iwt_MoveGadId)
		{
	case VGADGETID_WAITFORDRAG:
		break;

	case VGADGETID_DRAGMOUSEMOVE:
		d1(KPrintF("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		if (iMsg->Qualifier & (IEQUALIFIER_RBUTTON | IEQUALIFIER_MIDBUTTON))
			{
			AbortFunctions(iwt);
			}
		else
			{
			IDCMPDragIntuiTicks(iwt, iMsg);
			}
		break;

	case VGADGETID_LASSO:
		if (iwt->iwt_LassoFlag)
			Lasso(iwt->iwt_OldMouse.X, iwt->iwt_OldMouse.Y, LASSOX_CrawlLasso, iwt);
		break;

	default:
		if (iMsg->Qualifier & IEQUALIFIER_LEFTBUTTON)
			{
			struct Gadget *gad = NULL;

			if (iwt->iwt_MoveGadId > VGADGETID_IDLE)
				{
				gad = iwt->iwt_WindowTask.wt_Window->FirstGadget;
				while (gad && gad->GadgetID != iwt->iwt_MoveGadId)
					gad = gad->NextGadget;
				}

			if (gad)
				{
				WORD gadLeft, gadTop, gadWidth, gadHeight;

				gadLeft = gad->LeftEdge;
				if (gad->Flags & GFLG_RELRIGHT)
					gadLeft += iwt->iwt_WindowTask.wt_Window->Width;

				gadTop = gad->TopEdge;
				if (gad->Flags & GFLG_RELBOTTOM)
					gadTop += iwt->iwt_WindowTask.wt_Window->Height;

				gadWidth = gad->Width;
				if (gad->Flags & GFLG_RELWIDTH)
					gadWidth += iwt->iwt_WindowTask.wt_Window->Width;

				gadHeight = gad->Height;
				if (gad->Flags & GFLG_RELHEIGHT)
					gadHeight += iwt->iwt_WindowTask.wt_Window->Height;

				if (iwt->iwt_WindowTask.wt_Window->MouseX >= gadLeft 
					&& iwt->iwt_WindowTask.wt_Window->MouseX < gadLeft + gadWidth
					&& iwt->iwt_WindowTask.wt_Window->MouseY >= gadTop
					&& iwt->iwt_WindowTask.wt_Window->MouseY < gadTop + gadHeight)
					{
					switch (iwt->iwt_MoveGadId)
						{
					case GADGETID_RIGHTBUTTON:
						ArrowRight(iwt, iMsg);
						break;
					case GADGETID_LEFTBUTTON:
						ArrowLeft(iwt, iMsg);
						break;
					case GADGETID_DOWNBUTTON:
						ArrowDown(iwt, iMsg);
						break;
					case GADGETID_UPBUTTON:
						ArrowUp(iwt, iMsg);
						break;
						}
					}
				}
			}
		else
			{
			ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);
			}
		break;
		}

	return 0;
}


static ULONG IDCMPChangeWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(KPrintF("%s/%s/%ld: START  iwt=%08lx  <%s>  IDCMPWindow=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iMsg->IDCMPWindow));
	d1(KPrintF("%s/%s/%ld: Class=%08lx  Code=%ld  IAddress=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iMsg->Class, iMsg->Code, iMsg->IAddress));

	if (CWCODE_MOVESIZE == iMsg->Code)
		{
		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_ChangeWindow);
		}
#if defined(__MORPHOS__)
	else if (2 == iMsg->Code)
		{
		// MorphOS 2.3 "Enhanced Display Engine" special
		// dynmic window resizing generates tons of this msg
		// and application is reponsible to actually change the window size
		WORD WinX = ((ULONG) iMsg->IAddress) >> 16;
		WORD WinY = ((ULONG) iMsg->IAddress) & 0x0000ffff;
		UWORD NewWidth, NewHeight;

		d1(KPrintF("%s/%s/%ld: Class=%08lx  Code=%ld  IAddress=%08lx  MouseX=%ld  MouseY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iMsg->Class, iMsg->Code, iMsg->IAddress,\
			iMsg->MouseX, iMsg->MouseY));

		NewWidth = iMsg->MouseX - WinX;
		NewHeight = iMsg->MouseY - WinY;

		d1(KPrintF("%s/%s/%ld: NewWidth=%ld  NewHeight=%ld\n", __FILE__, __FUNC__, __LINE__, NewWidth, NewHeight));

		if ((abs(NewWidth - iwt->iwt_WindowTask.wt_Window->Width) > 2) ||
			(abs(NewHeight - iwt->iwt_WindowTask.wt_Window->Height) > 2) )
			{
			ChangeWindowBox(iwt->iwt_WindowTask.wt_Window,
				WinX, WinY,
				NewWidth,
				NewHeight);
			}
		}
	else if (3 == iMsg->Code)
		{
		// MorphOS 2.3 "Enhanced Display Engine" special
		// LMB has been released - end of window sizing
		d1(KPrintF("%s/%s/%ld: Class=%08lx  Code=%ld  IAddress=%08lx  MouseX=%ld  MouseY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iMsg->Class, iMsg->Code, iMsg->IAddress,\
			iMsg->MouseX, iMsg->MouseY));
		}
#endif //defined(__MORPHOS__)

	return 0;
}


static ULONG IDCMPNewSize(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(KPrintF("%s/%s/%ld: START  iwt=%08lx  <%s>  IDCMPWindow=%08lx  Code=%04lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iMsg->IDCMPWindow, iMsg->Code));

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Width != iMsg->IDCMPWindow->Width ||
		iwt->iwt_WindowTask.mt_WindowStruct->ws_Height != iMsg->IDCMPWindow->Height)
		{
		WORD OldInnerWidth = iwt->iwt_InnerWidth;
		WORD OldInnerHeight = iwt->iwt_InnerHeight;

		d1(KPrintF("%s/%s/%ld: iw=%ld  ih=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerWidth, iwt->iwt_InnerHeight));

		RefreshWindowFrame(iwt->iwt_WindowTask.wt_Window);

		// Calculate new inner window sizes
		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);

		d1(KPrintF("%s/%s/%ld: iw=%ld  ih=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerWidth, iwt->iwt_InnerHeight));

		RedrawResizedWindow(iwt, OldInnerWidth, OldInnerHeight);

		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_DynamicResize);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG IDCMPGadgetHelp(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	struct Gadget *gg = (struct Gadget *) iMsg->IAddress;

	d1(KPrintF("%s/%s/%ld: Class=%08lx  Code=%04lx  Qual=%04lx IAddress=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iMsg->Class, iMsg->Code, iMsg->Qualifier, iMsg->IAddress));

	iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetTextHook = NULL;
	iInfos.xii_GlobalGadgetUnderPointer.ggd_iwt = iwt;
	iInfos.xii_GlobalGadgetUnderPointer.ggd_Gadget = gg;

	if (NULL == gg)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp Window\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_unknown;
		}
	else if (gg == iwt->iwt_PropBottom)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp PropBottom\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_BottomScroller;
		}
	else if (gg == iwt->iwt_PropSide)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp PropSide\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_RightScroller;
		}
	else if (gg == iwt->iwt_GadgetRightArrow)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp GadgetRightArrow\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_RightArrow;
		}
	else if (gg == iwt->iwt_GadgetLeftArrow)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp GadgetLeftArrow\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_LeftArrow;
		}
	else if (gg == iwt->iwt_GadgetDownArrow)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp GadgetDownArrow\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_DownArrow;
		}
	else if (gg == iwt->iwt_GadgetUpArrow)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp GadgetUpArrow\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_UpArrow;
		}
	else if (gg == iwt->iwt_IconifyGadget)
		{
		d1(KPrintF("%s/%s/%ld: GadgetHelp Iconify\n", __FILE__, __FUNC__, __LINE__));
		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_Iconify;
		}
	else if (gg == iwt->iwt_StatusBar)
		{
		UWORD Code = iMsg->Code;

		d1(KPrintF("%s/%s/%ld: GadgetHelp StatusBar  Code=%04lx\n", __FILE__, __FUNC__, __LINE__, iMsg->Code));

		if (GADGETID_STATUSBAR == Code)
			{
			Code = StatusBarQueryGadgetID(iwt, (struct ExtGadget *) gg, iMsg->MouseX, iMsg->MouseY);
			d1(KPrintF("%s/%s/%ld: Code=%04lx\n", __FILE__, __FUNC__, __LINE__, Code));
			}
		
		switch (Code)
			{
		case SBAR_GadgetID_Text:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar Text\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_Text;
			break;
		case SBAR_GadgetID_PadLock:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar Padlock\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_ReadOnly;
			break;
		case SBAR_GadgetID_Reading:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar Reading\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_Reading;
			break;
		case SBAR_GadgetID_Typing:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar Typing\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_Typing;
			break;
		case SBAR_GadgetID_ShowAll:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar ShowAll\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_ShowAll;
			break;
		case SBAR_GadgetID_ThumbnailsAlways:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar ThumbnailsAlways\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_ThumbnailsAlways;
			break;
		case SBAR_GadgetID_ThumbnailsAsDefault:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar ThumbnailsAsDefault\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_ThumbnailsAsDefault;
			break;
		case SBAR_GadgetID_ThumbnailsGenerate:
			d1(kprintf("%s/%s/%ld: GadgetHelp StatusBar ThumbnailsGenerate\n", __FILE__, __FUNC__, __LINE__));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_StatusBar_ThumbnailsGenerate;
			break;

		default:
			d1(KPrintF("%s/%s/%ld: unknown StatusBar GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, Code));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_unknown;
			break;
			}
		}
	else if (gg == iwt->iwt_ControlBar)
		{
		UWORD Code = iMsg->Code;

		d1(KPrintF("%s/%s/%ld: GadgetHelp ControlBar  Code=%04lx\n", __FILE__, __FUNC__, __LINE__, iMsg->Code));

		if (GADGETID_CONTROLBAR == Code)
			{
			iInfos.xii_GlobalGadgetUnderPointer.ggd_cgy = ControlBarFindGadgetByID(iwt,
				StatusBarQueryGadgetID(iwt, (struct ExtGadget *)gg, iMsg->MouseX, iMsg->MouseY));
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_ControlBar;
			d1(KPrintF("%s/%s/%ld: Code=%04lx\n", __FILE__, __FUNC__, __LINE__, Code));
			}
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: Reading=%08lx Typing=%08lx Text=%08lx ReadAll=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, iwt_StatusBarMembers[STATUSBARGADGET_Reading], \
			iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing],\
			iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText], \
			iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll]));

		d1(KPrintF("%s/%s/%ld: GadgetHelp gg=%08lx\n", __FILE__, __FUNC__, __LINE__));

		iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_unknown;
		}

	return 0;
}


#if defined(IDCMP_EXTENDEDMOUSE)
static ULONG IDCMPExtendedMouse(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	const struct IntuiWheelData *iwd;
	ULONG n;
	struct IntuiMessage imc;

	d1(KPrintF("%s/%s/%ld: Class=%08lx  Code=%04lx  Qual=%04lx IAddress=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iMsg->Class, iMsg->Code, iMsg->Qualifier, iMsg->IAddress));

	iwd = (const struct IntuiWheelData *) iMsg->IAddress;

	d1(KPrintF("%s/%s/%ld: WheelX=%ld  WheelY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, iwd->WheelX, iwd->WheelY));

	imc.Class = IDCMP_RAWKEY;
	imc.Qualifier = iMsg->Qualifier;
	imc.IAddress = NULL;
	imc.MouseX = iMsg->MouseX;
	imc.MouseY = iMsg->MouseY;

	if (iwd->WheelX < 0)
		{
		imc.Code = NM_WHEEL_LEFT;
		for (n = 0; n < -iwd->WheelX; n++)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_RawKey, &imc);
			}
		}
	else if (iwd->WheelX > 0)
		{
		imc.Code = NM_WHEEL_RIGHT;
		for (n = 0; n < iwd->WheelX; n++)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_RawKey, &imc);
			}
		}
	if (iwd->WheelY < 0)
		{
		imc.Code = NM_WHEEL_UP;
		for (n = 0; n < -iwd->WheelY; n++)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_RawKey, &imc);
			}
		}
	else if (iwd->WheelY > 0)
		{
		imc.Code = NM_WHEEL_DOWN;
		for (n = 0; n < iwd->WheelY; n++)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_RawKey, &imc);
			}
		}

	return 0;
}
#endif //defined(IDCMP_EXTENDEDMOUSE)


static void ArrowRight(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	LONG deltaX;

	if (iMsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
		deltaX = 1;
	else
//		deltaX = (iwt->iwt_InnerWidth + 11) / 12;
		deltaX = 10;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, deltaX, 0);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustBottomSlider);
}


static void ArrowLeft(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	LONG deltaX;

	if (iMsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
		deltaX = -1;
	else
//		deltaX = -(iwt->iwt_InnerWidth + 11) / 12;
		deltaX = -10;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, deltaX, 0);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustBottomSlider);
}


static void ArrowDown(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	LONG deltaY;

	if (iMsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
		deltaY = 1;
	else
//		deltaY = (iwt->iwt_InnerHeight + 11) / 12;
		deltaY = 10;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, 0, deltaY);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider);
}


static void ArrowUp(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	LONG deltaY;

	if (iMsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
		deltaY = -1;
	else
//		deltaY = -(iwt->iwt_InnerHeight + 11) / 12;
		deltaY = -10;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, 0, deltaY);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider);
}


static void SetPropBottom(struct internalScaWindowTask *iwt)
{
	ULONG Top;

	GetAttr(PGA_Top, (Object *) iwt->iwt_PropBottom, &Top);

	Top = (Top << iwt->iwt_BottomScrollerScale) + iwt->iwt_MinX;

	if (Top != iwt->iwt_WindowTask.wt_XOffset)
		{
		if (ScalosAttemptLockIconListShared(iwt))
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, Top - iwt->iwt_WindowTask.wt_XOffset, 0);
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 0);
			ScalosUnLockIconList(iwt);
			}
		else
			{
			struct SM_DeltaMove *smdm;

			smdm = (struct SM_DeltaMove *) SCA_AllocMessage(MTYP_DeltaMove, 0);

			if (smdm)
				{
				smdm->smdm_DeltaX = Top - iwt->iwt_WindowTask.wt_XOffset;
				smdm->smdm_DeltaY = 0;
				smdm->smdm_AdjustSlider = 0;

				PutMsg(iwt->iwt_WindowTask.mt_WindowStruct->ws_MessagePort, &smdm->ScalosMessage.sm_Message);
				}
			}
		}
}


static void SetPropSide(struct internalScaWindowTask *iwt)
{
	ULONG Top;

	GetAttr(PGA_Top, (Object *) iwt->iwt_PropSide, &Top);

	Top = (Top << iwt->iwt_SideScrollerScale) + iwt->iwt_MinY;

	if (Top != iwt->iwt_WindowTask.wt_YOffset)
		{
		if (ScalosAttemptLockIconListShared(iwt))
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, 0, Top - iwt->iwt_WindowTask.wt_YOffset);
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 0);
			ScalosUnLockIconList(iwt);
			}
		else
			{
			struct SM_DeltaMove *smdm;

			smdm = (struct SM_DeltaMove *) SCA_AllocMessage(MTYP_DeltaMove, 0);

			if (smdm)
				{
				smdm->smdm_DeltaX = 0;
				smdm->smdm_DeltaY = Top - iwt->iwt_WindowTask.wt_YOffset;
				smdm->smdm_AdjustSlider = 0;

				PutMsg(iwt->iwt_WindowTask.mt_WindowStruct->ws_MessagePort, &smdm->ScalosMessage.sm_Message);
				}
			}
		}
}


static void MousePress(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(KPrintF("%s/%s/%ld: START iwt_MoveGadId=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_MoveGadId));

	if (VGADGETID_IDLE == iwt->iwt_MoveGadId)
		{
		struct ScaIconNode *in;

		ResetToolTips(iwt);

		SCA_ScreenTitleMsg(NULL, NULL);

		if (ScalosAttemptLockIconListShared(iwt))
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			in = CheckMouseIcon(&iwt->iwt_WindowTask.wt_IconList, iwt, 
				iMsg->MouseX - iwt->iwt_InnerLeft + iwt->iwt_WindowTask.wt_XOffset,
				iMsg->MouseY - iwt->iwt_InnerTop + iwt->iwt_WindowTask.wt_YOffset);

			d1(KPrintF("%s/%s/%ld: in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));
			if (in)
				{
				struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
				struct ExtGadget *ggLast = (struct ExtGadget *) iwt->iwt_LastIcon;

				if (gg == ggLast 
					&& gg->LeftEdge == iwt->iwt_LastPos.X 
					&& gg->TopEdge == iwt->iwt_LastPos.Y
					&& DoubleClick(iwt->iwt_LastTime.tv_secs, iwt->iwt_LastTime.tv_micro,
						iMsg->Seconds, iMsg->Micros))
					{
					// double-click on icon
					iwt->iwt_LastIcon = NULL;

					d1(KPrintF("%s/%s/%ld: double-click on icon\n", __FILE__, __FUNC__, __LINE__));

					// icon doesn' stay selected after double click
					OldIconsOff(iwt, NULL, iwt->iwt_WindowTask.wt_XOffset,
						iwt->iwt_WindowTask.wt_YOffset, iMsg->Qualifier, CurrentPrefs.pref_EasyMultiselectFlag);

					ScalosUnLockIconList(iwt);
					SetMenuOnOff(iwt);

					DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Open, in, 0L);
					}
				else
					{
					BOOL select = TRUE;

					d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  wt_Window=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iwt->iwt_WindowTask.wt_Window));

					if (CurrentPrefs.pref_EasyMultiselectFlag || (iMsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)))
						{
						// +jl+ 20010318 toggle selection
						select = !(gg->Flags & GFLG_SELECTED);
						}

					if (!CurrentPrefs.pref_EasyMultiselectFlag)
						{
						OldIconsOff(iwt, in->in_Icon, iwt->iwt_WindowTask.wt_XOffset, 
							iwt->iwt_WindowTask.wt_YOffset, iMsg->Qualifier, CurrentPrefs.pref_EasyMultiDragFlag);
						}

					ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, select);
					AdjustIconActive(iwt);

					iwt->iwt_LastIcon = in->in_Icon;

					iwt->iwt_LastPos.X = gg->LeftEdge;
					iwt->iwt_LastPos.Y = gg->TopEdge;

					iwt->iwt_LastTime.tv_secs = iMsg->Seconds;
					iwt->iwt_LastTime.tv_micro = iMsg->Micros;

					iwt->iwt_OldMouse.X = iMsg->MouseX;
					iwt->iwt_OldMouse.Y = iMsg->MouseY;

					iwt->iwt_MoveGadId = VGADGETID_WAITFORDRAG;
					ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_MOUSEMOVE);

					ScalosUnLockIconList(iwt);
					SetMenuOnOff(iwt);
					}
				}
			else
				{
				struct Region *oldClipRegion;

				iwt->iwt_LastIcon = NULL;

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				oldClipRegion = (struct Region *) DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_InitClipRegion);

				OldIconsOff(iwt, NULL, iwt->iwt_WindowTask.wt_XOffset, 
					iwt->iwt_WindowTask.wt_YOffset, iMsg->Qualifier, CurrentPrefs.pref_EasyMultiselectFlag);

				DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, oldClipRegion);

				iwt->iwt_MoveGadId = VGADGETID_LASSO;

				ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_INTUITICKS | IDCMP_MOUSEMOVE);

				iwt->iwt_LockFlag = TRUE;
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_NeedsTimerMsg;

				iwt->iwt_OldMouse.X = iMsg->MouseX;
				iwt->iwt_OldMouse.Y = iMsg->MouseY;

				ScalosUnLockIconList(iwt);
				SetMenuOnOff(iwt);
				}
			}
		}
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void MouseRelease(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(KPrintF( "%s/%s/%ld: START iwt_MoveGadId=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_MoveGadId));

	if (iwt->iwt_MoveGadId)
		{
		BOOL DragDrop = FALSE;
		T_TIMEVAL ClickDelay;

		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);

		switch (iwt->iwt_MoveGadId)
			{
		case VGADGETID_WAITFORDRAG:
			d1(KPrintF( "%s/%s/%ld: VGADGETID_WAITFORDRAG\n", __FILE__, __FUNC__, __LINE__));
			break;

		case VGADGETID_DRAGMOUSEMOVE:
			EndDrag(iwt);

			ClickDelay.tv_secs = iMsg->Seconds - iwt->iwt_LastTime.tv_secs;
			ClickDelay.tv_micro = iMsg->Micros - iwt->iwt_LastTime.tv_micro;
			if (iMsg->Micros < iwt->iwt_LastTime.tv_micro)
				{
				ClickDelay.tv_micro += 1000000;
				ClickDelay.tv_secs += 1;
				}

			if (((ClickDelay.tv_secs > CurrentPrefs.pref_ClickDelay.tv_secs) ||
				(((ClickDelay.tv_secs == CurrentPrefs.pref_ClickDelay.tv_secs) && (ClickDelay.tv_micro > CurrentPrefs.pref_ClickDelay.tv_micro))
				&& (iwt->iwt_OldMouse.X != iMsg->MouseX || iwt->iwt_OldMouse.Y != iMsg->MouseY))))
				{
				struct Layer *MouseLayer;

				MouseLayer = WhichLayer(iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo,
					iMsg->MouseX + iwt->iwt_WindowTask.wt_Window->LeftEdge,
					iMsg->MouseY + iwt->iwt_WindowTask.wt_Window->TopEdge);

				if (MouseLayer && MouseLayer->Window)
					{
					DragDrop = TRUE;
					DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DragDrop, 
						MouseLayer->Window, iMsg->MouseX, iMsg->MouseY, iMsg->Qualifier);
					}
				}

			if (!DragDrop)
				{
				IconWin_EndDrag(iwt);
				RestoreDragIcons(iwt);
				}

			EndDragUnlock(iwt);
			break;

		case VGADGETID_LASSO:
			d1(KPrintF( "%s/%s/%ld: VGADGETID_LASSO\n", __FILE__, __FUNC__, __LINE__));
			EndLasso(iwt);
			d1(KPrintF( "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			SetMenuOnOff(iwt);
			d1(KPrintF( "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}

		iwt->iwt_MoveGadId = VGADGETID_IDLE;
		iwt->iwt_LockFlag = FALSE;

		if (!iwt->iwt_NeedsTimerFlag)
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_NeedsTimerMsg;
		}
	d1(KPrintF( "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void AbortFunctions(struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: iwt_MoveGadId=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_MoveGadId));

	switch (iwt->iwt_MoveGadId)
		{
	case VGADGETID_DRAGMOUSEMOVE:
		ClosePopupWindows(TRUE);
		EndDrag(iwt);
		IconWin_EndDrag(iwt);
		RestoreDragIcons(iwt);

		EndDragUnlock(iwt);

		if (iwt->iwt_LastIcon)
			{
			ClassSelectIconObj(iwt->iwt_WindowTask.mt_WindowStruct, iwt->iwt_LastIcon, FALSE);
			iwt->iwt_LastIcon = NULL;
			}
		break;

	case VGADGETID_LASSO:
		EndLasso(iwt);
		break;

	default:
		break;
		}

	iwt->iwt_MoveGadId = VGADGETID_IDLE;
	iwt->iwt_LockFlag = FALSE;

	if (iwt->iwt_NeedsTimerFlag)
		iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_NeedsTimerMsg;

	AdjustIconActive(iwt);
	SetMenuOnOff(iwt);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void RemoveDeviceIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di,
	struct ScaIconNode **DeleteableIconList)
{
	struct ScaIconNode *in, *inNext;

	ScalosLockIconListExclusive(iwt);

	d1(kprintf("%s/%s/%ld: Remove DeviceIcon <%s>\n", __FILE__, __FUNC__, __LINE__, di->di_Device));

	d1(\
	{\
		struct ScaIconNode *in;

		kprintf("%s/%s/%ld:  --------start----------\n", __FILE__, __FUNC__, __LINE__);

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in=(struct ScaIconNode *) in->in_Node.mln_Succ)
			kprintf("%s/%s/%ld:  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in));

		kprintf("%s/%s/%ld:  ---------end----------\n", __FILE__, __FUNC__, __LINE__);
	});

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		if (in->in_Icon == di->di_Icon)
			{
			struct ScaBackdropIcon *sbi;

			d1(kprintf("%s/%s/%ld:  Found  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
			d1(kprintf("%s/%s/%ld:  innext=%08lx  IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, inNext, in->in_IconList));

			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, DeleteableIconList, in);

			for (sbi=in->in_IconList; sbi; sbi = (struct ScaBackdropIcon *) sbi->sbi_Node.mln_Succ)
				{
				struct ScaIconNode *in2, *in2Next;

				d1(kprintf("%s/%s/%ld:  sbi=%08lx\n", __FILE__, __FUNC__, __LINE__, sbi));

				for (in2=iwt->iwt_WindowTask.wt_IconList; in2; in2=in2Next)
					{
					in2Next = (struct ScaIconNode *) in2->in_Node.mln_Succ;

					if (in2->in_Icon == sbi->sbi_Icon)
						{
						// move <in2> to <DeleteableIconList>
						d1(kprintf("%s/%s/%ld:  in2next=%08lx\n", __FILE__, __FUNC__, __LINE__, in2Next));

						if (in2 == inNext)
							inNext = (struct ScaIconNode *) in2->in_Node.mln_Succ;

						d1(kprintf("%s/%s/%ld:  Found  in2=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in2, GetIconName(in2)));
						SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, DeleteableIconList, in2);
						break;
						}
					}
				}

			in->in_DeviceIcon = NULL;

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			FreeBackdropIconList(&in->in_IconList);

			DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_FreeDevNode, di);

			SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_DevIconList, &di->di_Node);
			}
		}

	d1(\
	{\
		struct ScaIconNode *in;

		kprintf("%s/%s/%ld:  --------start----------\n", __FILE__, __FUNC__, __LINE__);

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in=(struct ScaIconNode *) in->in_Node.mln_Succ)
			kprintf("%s/%s/%ld:  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in));

		kprintf("%s/%s/%ld:  ---------end----------\n", __FILE__, __FUNC__, __LINE__);
	});

	ScalosUnLockIconList(iwt);
}


static void OldIconsOff(struct internalScaWindowTask *iwt, Object *IconObj, 
	LONG XOffset, LONG YOffset, UWORD Qualifier, BOOL KeepSelected)
{
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;
	struct ScaWindowStruct *ws;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
		return;

	if (KeepSelected && gg && (gg->Flags & GFLG_SELECTED))
		return;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

		if (iwtx->iwt_WindowTask.wt_Window)
			{
			if (ScalosAttemptLockIconListShared(iwtx))
				{
				struct Region *oldClipRegion = NULL;
				BOOL ClipRegionSet = FALSE;
				struct ScaIconNode *in;

				for (in=iwtx->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					if (in->in_Icon != IconObj)
						{
						if (!ClipRegionSet)
							{
							oldClipRegion = (struct Region *) DoMethod(iwtx->iwt_WindowTask.mt_WindowObject, SCCM_Window_InitClipRegion);
							ClipRegionSet = TRUE;
							}

						ClassSelectIcon(ws, in, FALSE);
						}
					}

				if (ClipRegionSet)
					DoMethod(iwtx->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, oldClipRegion);

				ScalosUnLockIconList(iwtx);
				}
			}
		AdjustIconActive(iwtx);
		}

	SCA_UnLockWindowList();

	AdjustIconActive(iwt);
}


static LONG SafeStrCmp(CONST_STRPTR str1, CONST_STRPTR str2)
{
	if (str1 && str2)
		return strcmp(str1, str2);

	if (NULL == str1 && NULL == str2)
		return 0;

	return 1;
}


static void IDCMPBeginDrag(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg)
{
	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (abs(iwt->iwt_OldMouse.X - iMsg->MouseX) +
		abs(iwt->iwt_OldMouse.Y - iMsg->MouseY) > 2)
		{
		// mouse has been moved with LMB pressed, so we assume user wants to start dragging an icon
		d1(KPrintF("%s/%s/%ld: mouse moved\n", __FILE__, __FUNC__, __LINE__));

		if (ScalosAttemptLockIconListExclusive(iwt))
			{
			struct ScaIconNode *in;

			in = CheckMouseIcon(&iwt->iwt_WindowTask.wt_IconList, iwt,
				iwt->iwt_OldMouse.X - iwt->iwt_InnerLeft + iwt->iwt_WindowTask.wt_XOffset,
				iwt->iwt_OldMouse.Y - iwt->iwt_InnerTop + iwt->iwt_WindowTask.wt_YOffset);

			d1(KPrintF("%s/%s/%ld: in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));
			if (in)
				{
				struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

				d1(KPrintF("%s/%s/%ld: SelectRender=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->SelectRender));
				if (gg->SelectRender)
					{
					iwt->iwt_MoveGadId = VGADGETID_DRAGMOUSEMOVE;
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags | IDCMP_INTUITICKS | IDCMP_MOUSEMOVE);

					iwt->iwt_LockFlag = TRUE;
					iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_NeedsTimerMsg;

					iwt->iwt_DragFlags = 0;

					InitDrag(iMsg, iwt);

					// Keep icon list locked (ahared!) until EndDrag()
					ScalosUnLockIconList(iwt);
					ScalosLockIconListShared(iwt);
					}
				else
					{
					ScalosUnLockIconList(iwt);
					}
				}
			else
				{
				// no icon present, leave VGADGETID_WAITFORDRAG state
				iwt->iwt_MoveGadId = VGADGETID_IDLE;
				ScalosUnLockIconList(iwt);
				}
			}
		}
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

