// ControlBar.c
// $Date$
// $Revision$


#ifdef __amigaos4__
#define INTUITION_PRE_V36_NAMES
#endif
#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <workbench/workbench.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"
#include "DtImageClass.h"
#include "FrameImageClass.h"
#include "CycleGadgetClass.h"
#include "HistoryGadgetClass.h"

//----------------------------------------------------------------------------

struct ControlBarMember;	// forward

typedef void (*CBM_DISABLEFUNC)(struct internalScaWindowTask *, struct ControlBarMember *);

struct ControlBarMember
	{
	struct Node cbm_Node;
	struct ControlBarGadgetEntry cbm_Entry;
	enum SCPGadgetTypes cbm_GadgetType;
	Object *cbm_Gadget;
	Object *cbm_NormalImage;
	Object *cbm_SelectedImage;
	Object *cbm_FrameImage;
	const struct ScalosMenuCommand *cbm_smcom;	// for action buttons, holds pointer to MenuCommandTable entry
	CBM_DISABLEFUNC	cbm_DisableFunc;
	};

//----------------------------------------------------------------------------

static ULONG ControlbarCodeFromViewMode(UBYTE ViewByType);
static struct ControlBarMember *ControlBarCreateViewByCycle(struct internalScaWindowTask *iwt,
	const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateShowModeCycle(struct internalScaWindowTask *iwt,
	const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateSeparator(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateActionButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateBackButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateForwardButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateParentButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateBrowseButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static struct ControlBarMember *ControlBarCreateButton(CONST_STRPTR NormalImgName,
	CONST_STRPTR SelectedImgName, CONST_STRPTR DisabledImgName,
	ULONG GadgetID, ULONG FallbackSysImg, CBM_DISABLEFUNC DisableFunc);
static struct ControlBarMember *ControlBarCreateHistoryGadget(struct internalScaWindowTask *iwt,
	const struct ControlBarGadgetEntry *cgy, ULONG GadgetID);
static UWORD ControlBarQueryGadgetID(struct ExtGadget *gg);
static void FreeControlBarMember(struct ControlBarMember *Member);
static struct ControlBarMember *ControlBarFindMember(struct internalScaWindowTask *iwt,
	ULONG GadgetID);
static void ControlBarParentDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm);
static void ControlBarBackDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm);
static void ControlBarForwardDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm);
static void ControlBarHistoryListDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm);
static void ControlBarAddGadgets(struct internalScaWindowTask *iwt, const struct List *CbGadgetsList);

//----------------------------------------------------------------------------

BOOL ControlBarAdd(struct internalScaWindowTask *iwt)
{
	ULONG Height = 0;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		const struct List *CbGadgetsList;
		struct Window *win;

		if (iwt->iwt_ControlBar)
			break;			// ControlBar already present
		if (iwt->iwt_BackDrop)
			break;			// no ControlBar for backdrop windows

		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_MoreFlags & WSV_MoreFlagF_NoControlBar)
			break;

		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_BrowserMode)
			CbGadgetsList = &ControlBarGadgetListBrowser;
		else
			CbGadgetsList = &ControlBarGadgetListNormal;

		if (IsListEmpty((struct List *) CbGadgetsList))
			break;

		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Window)
			win = iwt->iwt_WindowTask.mt_WindowStruct->ws_Window;
		else
			win = iInfos.xii_iinfos.ii_MainWindowStruct->ws_Window;

		d1(KPrintF("%s/%s/%ld: StatusBar BgPen=%ld\n", __FILE__, __FUNC__, __LINE__, \
			PalettePrefs.pal_PensList[PENIDX_STATUSBAR_BG]));

		iwt->iwt_ControlBar = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBar.sca",
			GBDTA_WindowTask, (ULONG) iwt,
			GBDTA_Position, GBPOS_Top,
			GA_RelSpecial, TRUE,
			GA_GadgetHelp, TRUE,
			GA_ID, GADGETID_CONTROLBAR,
			GBDTA_BGPen, PalettePrefs.pal_PensList[PENIDX_STATUSBAR_BG],
			GBDTA_BackgroundImageName, (ULONG) "THEME:Window/ControlBar/Background",
			GA_RelVerify, TRUE,
			GA_GadgetHelp, TRUE,
			TAG_END);

		d1(KPrintF("%s/%s/%ld: iwt_ControlBar=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ControlBar));
		if (NULL == iwt->iwt_ControlBar)
			break;

		ControlBarAddGadgets(iwt, CbGadgetsList);

		d1(KPrintF("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, Height));

		if (iwt->iwt_WindowTask.wt_Window)
			AddGList(iwt->iwt_WindowTask.wt_Window, iwt->iwt_ControlBar, ~0, 1, NULL);

		// Layout Control bar so we can query the required height
		DoGadgetMethod(iwt->iwt_ControlBar,
			win, NULL,
			GM_LAYOUT,
			NULL,		// GadgetInfo is filled in by DoGadgetMethod
			TRUE);

		GetAttr(GA_Height, (Object *) iwt->iwt_ControlBar, &Height);

		d1(KPrintF("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, Height));
		d1(KPrintF("%s/%s/%ld: iwt_ExtraHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ExtraHeight));
		d1(KPrintF("%s/%s/%ld: iwt_InnerTop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerTop));

		iwt->iwt_ExtraHeight += Height;
		iwt->iwt_InnerTop += iwt->iwt_GadgetBarHeightTop;
		iwt->iwt_InnerHeight -= iwt->iwt_InnerTop;

		d1(KPrintF("%s/%s/%ld: iwt_GadgetBarHeightTop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadgetBarHeightTop));
		d1(KPrintF("%s/%s/%ld: iwt_ExtraHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ExtraHeight));
		d1(KPrintF("%s/%s/%ld: iwt_InnerTop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerTop));

		d1(KPrintF("%s/%s/%ld: iwt_ExtraHeight=%ld  iwt_GadgetBarHeightTop=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_ExtraHeight, iwt->iwt_GadgetBarHeightTop));
		d1(KPrintF("%s/%s/%ld: iwt_InnerTop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerTop));

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

void ControlBarRemove(struct internalScaWindowTask *iwt)
{
	struct ControlBarMember *Member;

	iwt->iwt_HighlightedControlBarGadget = NULL;

	if (iwt->iwt_ControlBar)
		{
		d1(kprintf("%s/%s/%ld: iwt_ExtraHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ExtraHeight));

		if (iwt->iwt_WindowTask.wt_Window)
			RemoveGList(iwt->iwt_WindowTask.wt_Window, iwt->iwt_ControlBar, 1);

		d1(kprintf("%s/%s/%ld: iwt_InnerBottom=%ld  iwt_SkipBottom=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_InnerBottom, iwt->iwt_SkipBottom));

		d1(KPrintF("%s/%s/%ld: iwt_GadgetBarHeightTop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadgetBarHeightTop));
		d1(KPrintF("%s/%s/%ld: iwt_ExtraHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ExtraHeight));
		d1(KPrintF("%s/%s/%ld: iwt_InnerTop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerTop));

		iwt->iwt_ExtraHeight -= iwt->iwt_GadgetBarHeightTop;
		iwt->iwt_InnerTop -= iwt->iwt_GadgetBarHeightTop;
		iwt->iwt_InnerHeight += iwt->iwt_GadgetBarHeightTop;

		d1(KPrintF("%s/%s/%ld: iwt_ExtraHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ExtraHeight));
		d1(KPrintF("%s/%s/%ld: iwt_InnerTop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerTop));

		DisposeObject((Object *) iwt->iwt_ControlBar);
		iwt->iwt_ControlBar = NULL;

		d1(kprintf("%s/%s/%ld: iwt_InnerBottom=%ld  iwt_SkipBottom=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_InnerBottom, iwt->iwt_SkipBottom));
		}

	while ((Member = (struct ControlBarMember *) RemHead(&iwt->iwt_ControlBarMemberList)))
		{
		FreeControlBarMember(Member);
		}
}

//----------------------------------------------------------------------------

void ControlBarRebuild(struct internalScaWindowTask *iwt)
{
	const struct List *CbGadgetsList;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_BrowserMode)
		CbGadgetsList = &ControlBarGadgetListBrowser;
	else
		CbGadgetsList = &ControlBarGadgetListNormal;

	if (IsListEmpty((struct List *) CbGadgetsList) || (iwt->iwt_WindowTask.mt_WindowStruct->ws_MoreFlags & WSV_MoreFlagF_NoControlBar))
		{
		// No control bar desired for window - remove it if present
		ControlBarRemove(iwt);
		}
	else
		{
		if (iwt->iwt_ControlBar)
			{
			// control bar is already present - apply modifications
			struct ControlBarMember *Member;
			ULONG OldHeight = 0, NewHeight = 0;
			ULONG OldHeightTop = iwt->iwt_GadgetBarHeightTop;
			ULONG OldInnerTop = iwt->iwt_InnerTop;

			iwt->iwt_HighlightedControlBarGadget = NULL;

			GetAttr(GA_Height, (Object *) iwt->iwt_ControlBar, &OldHeight);

			d1(KPrintF("%s/%s/%ld: OldHeight=%ld\n", __FILE__, __FUNC__, __LINE__, OldHeight));

			while ((Member = (struct ControlBarMember *) RemHead(&iwt->iwt_ControlBarMemberList)))
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_RemFromControlBar,
					Member->cbm_Gadget,
					TAG_END);
				FreeControlBarMember(Member);
				}

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			ControlBarAddGadgets(iwt, CbGadgetsList);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// Layout Control bar so we can query the required height
			DoGadgetMethod(iwt->iwt_ControlBar,
				iwt->iwt_WindowTask.wt_Window, NULL,
				GM_LAYOUT,
				NULL,		// GadgetInfo is filled in by DoGadgetMethod
				FALSE);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			GetAttr(GA_Height, (Object *) iwt->iwt_ControlBar, &NewHeight);

			d1(KPrintF("%s/%s/%ld: NewHeight=%ld\n", __FILE__, __FUNC__, __LINE__, NewHeight));

			if (NewHeight != OldHeight)
				{
				iwt->iwt_ExtraHeight += NewHeight - OldHeight;
				iwt->iwt_InnerTop    += iwt->iwt_GadgetBarHeightTop - OldHeightTop;
				iwt->iwt_InnerHeight -= iwt->iwt_InnerTop - OldInnerTop;
				}
			}
		else
			{
			// control bar was not present - add it now
			ControlBarAdd(iwt);
			}
		}

	DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

void ControlBarUpdateViewMode(struct internalScaWindowTask *iwt, UBYTE ViewByType)
{
	struct ControlBarMember *Member;

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		if (SCPGadgetType_ViewBy == Member->cbm_GadgetType)
			{
			SetGadgetAttrs((struct Gadget *) Member->cbm_Gadget,
				iwt->iwt_WindowTask.wt_Window, NULL,
				SCACYCLE_Active, ControlbarCodeFromViewMode(ViewByType),
				TAG_END);
			}
		}
}

//----------------------------------------------------------------------------

static ULONG ControlbarCodeFromViewMode(UBYTE ViewByType)
{
	ULONG Code;

	switch (ViewByType)
		{
	case IDTV_ViewModes_Default:
		if (IsViewByIcon(ViewByType))
			Code = 0;
		else
			Code = 1;
		break;
	case IDTV_ViewModes_Icon:
		Code = 0;
		break;
	case IDTV_ViewModes_Name:
		Code = 1;
		break;
	case IDTV_ViewModes_Size:
		Code = 2;
		break;
	case IDTV_ViewModes_Date:
		Code = 3;
		break;
	case IDTV_ViewModes_Time:
		Code = 4;
		break;
	case IDTV_ViewModes_Comment:
		Code = 5;
		break;
	case IDTV_ViewModes_Protection:
		Code = 6;
		break;
	case IDTV_ViewModes_Type:
		Code = 7;
		break;
	default:
		Code = 1;
		break;
		}

	return Code;
}

//----------------------------------------------------------------------------

UBYTE ControlBarViewModeFromCode(ULONG Code)
{
	UBYTE ViewMode;

	switch (Code)
		{
	case 0:
		ViewMode = IDTV_ViewModes_Icon;
		break;
	case 1:
		ViewMode = IDTV_ViewModes_Name;
		break;
	case 2:
		ViewMode = IDTV_ViewModes_Size;
		break;
	case 3:
		ViewMode = IDTV_ViewModes_Date;
		break;
	case 4:
		ViewMode = IDTV_ViewModes_Time;
		break;
	case 5:
		ViewMode = IDTV_ViewModes_Comment;
		break;
	case 6:
		ViewMode = IDTV_ViewModes_Protection;
		break;
	case 7:
		ViewMode = IDTV_ViewModes_Type;
		break;
	default:
		ViewMode = IDTV_ViewModes_Default;
		break;
		}

	return ViewMode;
}

//----------------------------------------------------------------------------

void ControlBarUpdateShowMode(struct internalScaWindowTask *iwt)
{
	struct ControlBarMember *Member;

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		if (SCPGadgetType_ShowMode == Member->cbm_GadgetType)
			{
			SetGadgetAttrs((struct Gadget *) Member->cbm_Gadget,
				iwt->iwt_WindowTask.wt_Window, NULL,
				SCACYCLE_Active, IsShowAll(iwt->iwt_WindowTask.mt_WindowStruct) ? 1 : 0,
				TAG_END);
			}
		}
}

//----------------------------------------------------------------------------

void ControlBarUpdateHistory(struct internalScaWindowTask *iwt)
{
	struct ControlBarMember *Member;

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		if (SCPGadgetType_History == Member->cbm_GadgetType)
			{
			SetGadgetAttrs((struct Gadget *) Member->cbm_Gadget,
				iwt->iwt_WindowTask.wt_Window, NULL,
				SCAHISTORY_Active, (ULONG) iwt->iwt_CurrentHistoryEntry,
				TAG_END);
			}
		}
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateViewByCycle(struct internalScaWindowTask *iwt,
	const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	static const ULONG CycleLabelMsgIds[] =
		{
		MSGID_CONTROLBAR_VIEWBY_ICON,
		MSGID_CONTROLBAR_VIEWBY_NAME,
		MSGID_CONTROLBAR_VIEWBY_SIZE,
		MSGID_CONTROLBAR_VIEWBY_DATE,
		MSGID_CONTROLBAR_VIEWBY_TIME,
		MSGID_CONTROLBAR_VIEWBY_COMMENT,
		MSGID_CONTROLBAR_VIEWBY_PROTECTION,
		MSGID_CONTROLBAR_VIEWBY_TYPE,
		};
	static CONST_STRPTR CycleLabels[1 + Sizeof(CycleLabelMsgIds)];
	struct ControlBarMember *Member;
	BOOL Success = FALSE;

	if (NULL == CycleLabels[0])
		{
		ULONG n;

		for (n = 0; n < Sizeof(CycleLabelMsgIds); n++)
			CycleLabels[n] = GetLocString(CycleLabelMsgIds[n]);
		}

	do	{
		Member = ScalosAlloc(sizeof(struct ControlBarMember));
		if (NULL == Member)
			break;

		Member->cbm_NormalImage = NULL;
		Member->cbm_SelectedImage = NULL;
		Member->cbm_smcom = NULL;	// for action buttons, holds pointer to MenuCommandTable entry
		Member->cbm_DisableFunc = NULL;

		// Frame image is optional
		Member->cbm_FrameImage = SCA_NewScalosObjectTags("FrameImage.sca",
			FRAMEIMG_ImageName, (ULONG) "THEME:Window/ControlBar/ViewByCycleFrame",
			TAG_END);
		d1(KPrintF("%s/%s/%ld: FrameImage=%08lx\n", __FILE__, __FUNC__, __LINE__, Frame));

		Member->cbm_Gadget = SCA_NewScalosObjectTags("CycleGadget.sca",
			SCACYCLE_Labels, (ULONG) CycleLabels,
			SCACYCLE_Active, ControlbarCodeFromViewMode(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes),
			SCACYCLE_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			SCACYCLE_PopupTextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			SCACYCLE_PopupTTFont, CurrentPrefs.pref_UseScreenTTFont ? (ULONG) &ScreenTTFont : 0,
			Member->cbm_FrameImage ? SCACYCLE_FrameImage : TAG_IGNORE, (ULONG) Member->cbm_FrameImage,
			GA_ID, GadgetID,
			GA_RelVerify, TRUE,
			GA_GadgetHelp, TRUE,
			ICA_TARGET, ICTARGET_IDCMP,
			TAG_END);
		if (NULL == Member->cbm_Gadget)
			break;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		FreeControlBarMember(Member);
		Member = NULL;
		}

	return Member;
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateShowModeCycle(struct internalScaWindowTask *iwt,
	const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	static const ULONG ShowModeLabelMsgIds[] =
		{
		MSGID_CONTROLBAR_SHOW_ONLYICONS,
		MSGID_CONTROLBAR_SHOW_ALLFILES,
		};
	static CONST_STRPTR ShowModeLabels[1 + Sizeof(ShowModeLabelMsgIds)];
	struct ControlBarMember *Member;
	BOOL Success = FALSE;

	if (NULL == ShowModeLabels[0])
		{
		ULONG n;

		for (n = 0; n < Sizeof(ShowModeLabelMsgIds); n++)
			ShowModeLabels[n] = GetLocString(ShowModeLabelMsgIds[n]);
		}

	do	{
		Member = ScalosAlloc(sizeof(struct ControlBarMember));
		if (NULL == Member)
			break;

		Member->cbm_NormalImage = NULL;
		Member->cbm_SelectedImage = NULL;
		Member->cbm_smcom = NULL;	// for action buttons, holds pointer to MenuCommandTable entry
		Member->cbm_DisableFunc = NULL;

		// Frame image is optional
		Member->cbm_FrameImage = SCA_NewScalosObjectTags("FrameImage.sca",
			FRAMEIMG_ImageName, (ULONG) "THEME:Window/ControlBar/ShowModeCycleFrame",
			TAG_END);
		d1(KPrintF("%s/%s/%ld: FrameImage=%08lx\n", __FILE__, __FUNC__, __LINE__, Frame));

		Member->cbm_Gadget = SCA_NewScalosObjectTags("CycleGadget.sca",
			SCACYCLE_Labels, (ULONG) ShowModeLabels,
			SCACYCLE_Active, IsShowAll(iwt->iwt_WindowTask.mt_WindowStruct) ? 1 : 0,
			SCACYCLE_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			SCACYCLE_PopupTextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			SCACYCLE_PopupTTFont, CurrentPrefs.pref_UseScreenTTFont ? (ULONG) &ScreenTTFont : 0,
			Member->cbm_FrameImage ? SCACYCLE_FrameImage : TAG_IGNORE, (ULONG) Member->cbm_FrameImage,
			GA_ID, GadgetID,
			GA_RelVerify, TRUE,
			GA_GadgetHelp, TRUE,
			ICA_TARGET, ICTARGET_IDCMP,
			TAG_END);
		if (NULL == Member->cbm_Gadget)
			break;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		FreeControlBarMember(Member);
		Member = NULL;
		}

	return Member;
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateSeparator(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	struct ControlBarMember *Member;
	BOOL Success = FALSE;

	do	{
		Member = ScalosAlloc(sizeof(struct ControlBarMember));
		if (NULL == Member)
			break;

		memset(Member, 0, sizeof(struct ControlBarMember));

		Member->cbm_Gadget = SCA_NewScalosObjectTags("SeparatorGadget.sca",
			GA_Width, 15,
			GA_Height, 3,
			GA_BottomBorder, TRUE,
			GA_ID, GadgetID,
			GA_Immediate, FALSE,
			GA_RelVerify, FALSE,
			GA_GadgetHelp, TRUE,
			TAG_END);
		if (NULL == Member->cbm_Gadget)
			break;

		Member->cbm_DisableFunc = NULL;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		FreeControlBarMember(Member);
		Member = NULL;
		}

	return Member;
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateActionButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	struct ControlBarMember *Member = NULL;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: START  cgy_Action=<%s>\n", __FILE__, __FUNC__, __LINE__, cgy->cgy_Action));

	do	{
		ULONG n;
		const struct ScalosMenuCommand *smcom = NULL;

		for (n = 0; MenuCommandTable[n].smcom_CommandName; n++)
			{
			if (0 == stricmp(cgy->cgy_Action, MenuCommandTable[n].smcom_CommandName))
				{
				smcom = &MenuCommandTable[n];
				break;
				}
			}

		d1(KPrintF("%s/%s/%ld: smcom=%08lx\n", __FILE__, __FUNC__, __LINE__, smcom));
		if (NULL == smcom)
			break;		// unknown command string

		Member = ScalosAlloc(sizeof(struct ControlBarMember));
		d1(KPrintF("%s/%s/%ld: Member=%08lx\n", __FILE__, __FUNC__, __LINE__, Member));
		if (NULL == Member)
			break;

		memset(Member, 0, sizeof(struct ControlBarMember));

		Member->cbm_smcom = smcom;

		// imgNormal is always required
		Member->cbm_NormalImage	= NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) cgy->cgy_NormalImage,
			DTIMG_SelImageName, (ULONG) cgy->cgy_SelectedImage,
			DTIMG_DisabledImageName, (ULONG) cgy->cgy_DisabledImage,
			TAG_END);
		d1(KPrintF("%s/%s/%ld: cbm_NormalImage=%08lx\n", __FILE__, __FUNC__, __LINE__, Member->cbm_NormalImage));
		if (NULL == Member->cbm_NormalImage)
			break;

		Member->cbm_Gadget = SCA_NewScalosObjectTags("ButtonGadget.sca",
			GA_Immediate, TRUE,
			GA_BottomBorder, TRUE,
			GA_ID, GadgetID,
			GA_RelVerify, TRUE,
			GA_GadgetHelp, TRUE,
			GA_Image, (ULONG) Member->cbm_NormalImage,
			TAG_END);
		d1(KPrintF("%s/%s/%ld: cbm_Gadget=%08lx\n", __FILE__, __FUNC__, __LINE__, Member->cbm_Gadget));
		if (NULL == Member->cbm_Gadget)
			break;

		Member->cbm_DisableFunc = NULL;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		FreeControlBarMember(Member);
		Member = NULL;
		}

	return Member;
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateBackButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	return ControlBarCreateButton("ButtonBackNormal", "ButtonBackSelected",
		"ButtonBackDisabled",
		GadgetID, LEFTIMAGE, ControlBarBackDisable);
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateForwardButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	return ControlBarCreateButton("ButtonForwardNormal", "ButtonForwardSelected",
		"ButtonForwardDisabled",
		GadgetID, RIGHTIMAGE, ControlBarForwardDisable);
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateParentButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	return ControlBarCreateButton("ButtonUpNormal", "ButtonUpSelected",
		"ButtonUpDisabled",
		GadgetID, UPIMAGE, ControlBarParentDisable);
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateBrowseButton(const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	return ControlBarCreateButton("BrowseNormal", "BrowseSelected",
		"BrowseDisabled",
		GadgetID, UPIMAGE, NULL);
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateButton(CONST_STRPTR NormalImgName,
	CONST_STRPTR SelectedImgName, CONST_STRPTR DisabledImgName,
	ULONG GadgetID, ULONG FallbackSysImg, CBM_DISABLEFUNC DisableFunc)
{
	STRPTR ImageNameNrm;
	STRPTR ImageNameSel = NULL;
	STRPTR ImageNameDisabled = NULL;
	struct ControlBarMember *Member = NULL;
	BOOL Success = FALSE;

	do	{
		ImageNameNrm = AllocPathBuffer();
		if (NULL == ImageNameNrm)
			break;

		ImageNameSel= AllocPathBuffer();
		if (NULL == ImageNameSel)
			break;

		ImageNameDisabled = AllocPathBuffer();
		if (NULL == ImageNameDisabled)
			break;

		Member = ScalosAlloc(sizeof(struct ControlBarMember));
		if (NULL == Member)
			break;

		d1(KPrintF("%s/%s/%ld: NormalImgName=<%s>\n", __FILE__, __FUNC__, __LINE__, NormalImgName));

		memset(Member, 0, sizeof(struct ControlBarMember));

		stccpy(ImageNameNrm, "THEME:Window/ControlBar", Max_PathLen);
		AddPart(ImageNameNrm, NormalImgName, Max_PathLen);

		stccpy(ImageNameSel, "THEME:Window/ControlBar", Max_PathLen);
		AddPart(ImageNameSel, SelectedImgName, Max_PathLen);

		stccpy(ImageNameDisabled, "THEME:Window/ControlBar", Max_PathLen);
		AddPart(ImageNameDisabled, DisabledImgName, Max_PathLen);

		// imgNormal is always required
		Member->cbm_NormalImage	= NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) ImageNameNrm,
			DTIMG_SelImageName, (ULONG) ImageNameSel,
			DTIMG_DisabledImageName, (ULONG) ImageNameDisabled,
			TAG_END);
		d1(KPrintF("%s/%s/%ld: cbm_NormalImage=%08lx\n", __FILE__, __FUNC__, __LINE__, Member->cbm_NormalImage));
		if (NULL == Member->cbm_NormalImage)
			{
			// no Theme image could be found - fall back to sys image
			Member->cbm_NormalImage	= NewObject(NULL, SYSICLASS,
					SYSIA_Which, FallbackSysImg,
					SYSIA_DrawInfo, (ULONG) iInfos.xii_iinfos.ii_DrawInfo,
					TAG_END);
			}
		d1(KPrintF("%s/%s/%ld: cbm_NormalImage=%08lx\n", __FILE__, __FUNC__, __LINE__, Member->cbm_NormalImage));
		if (NULL == Member->cbm_NormalImage)
			break;

		Member->cbm_Gadget = SCA_NewScalosObjectTags("ButtonGadget.sca",
			GA_Immediate, TRUE,
			GA_BottomBorder, TRUE,
			GA_ID, GadgetID,
			GA_RelVerify, TRUE,
			GA_GadgetHelp, TRUE,
			GA_Image, (ULONG) Member->cbm_NormalImage,
			TAG_END);
		d1(KPrintF("%s/%s/%ld: cbm_Gadget=%08lx\n", __FILE__, __FUNC__, __LINE__, Member->cbm_Gadget));
		if (NULL == Member->cbm_Gadget)
			break;

		Member->cbm_DisableFunc = DisableFunc;

		Success = TRUE;
		} while (0);

	if (ImageNameNrm)
		FreePathBuffer(ImageNameNrm);
	if (ImageNameSel)
		FreePathBuffer(ImageNameSel);
	if (ImageNameDisabled)
		FreePathBuffer(ImageNameDisabled);
	if (!Success)
		{
		FreeControlBarMember(Member);
		Member = NULL;
		}

	return Member;
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarCreateHistoryGadget(struct internalScaWindowTask *iwt,
	const struct ControlBarGadgetEntry *cgy, ULONG GadgetID)
{
	struct ControlBarMember *Member;
	BOOL Success = FALSE;

	do	{
		Member = ScalosAlloc(sizeof(struct ControlBarMember));
		if (NULL == Member)
			break;

		memset(Member, 0, sizeof(struct ControlBarMember));

		// imgNormal is always required
		Member->cbm_NormalImage	= NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) "THEME:Window/ControlBar/HistoryNormal" ,
			DTIMG_SelImageName, (ULONG) "THEME:Window/ControlBar/HistorySelected" ,
			DTIMG_DisabledImageName, (ULONG) "THEME:Window/ControlBar/HistoryDisabled" ,
			TAG_END);
		if (NULL == Member->cbm_NormalImage)
			{
			// no Theme image could be found - fall back to sys image
			Member->cbm_NormalImage	= NewObject(NULL, SYSICLASS,
					SYSIA_Which, MXIMAGE,
					SYSIA_DrawInfo, (ULONG) iInfos.xii_iinfos.ii_DrawInfo,
					TAG_END);
			}
		if (NULL == Member->cbm_NormalImage)
			break;

		ScalosObtainSemaphoreShared(&iwt->iwt_WindowHistoryListSemaphore);

		Member->cbm_Gadget = SCA_NewScalosObjectTags("HistoryGadget.sca",
			GA_Immediate, TRUE,
			GA_BottomBorder, TRUE,
			GA_ID, GadgetID,
			GA_RelVerify, TRUE,
			GA_GadgetHelp, TRUE,
			GA_Image, (ULONG) Member->cbm_NormalImage,
			SCAHISTORY_Labels, (ULONG) &iwt->iwt_HistoryList,
			SCAHISTORY_Active, (ULONG) iwt->iwt_CurrentHistoryEntry,
			TAG_END);
		if (NULL == Member->cbm_Gadget)
			break;

		ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

		Member->cbm_DisableFunc = ControlBarHistoryListDisable;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		FreeControlBarMember(Member);
		Member = NULL;
		}

	return Member;
}

//----------------------------------------------------------------------------

struct ControlBarGadgetEntry *ControlBarFindGadgetByID(struct internalScaWindowTask *iwt, UWORD GadgetID)
{
	struct ControlBarMember *Member;

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) Member->cbm_Gadget;

		if (GadgetID == gg->GadgetID)
			return &Member->cbm_Entry;
		}

	return NULL;
}

//----------------------------------------------------------------------------

static UWORD ControlBarQueryGadgetID(struct ExtGadget *gg)
{
	ULONG Code = 0;

	GetAttr(GBDTA_LastActive, (Object *) gg, &Code);
	d1(KPrintF("%s/%s/%ld: Code=%04lx\n", __FILE__, __FUNC__, __LINE__, Code));

	return (UWORD) Code;
}
//----------------------------------------------------------------------------

UWORD ControlBarQueryGadgetType(struct internalScaWindowTask *iwt, struct ExtGadget *gg)
{
	ULONG GadgetID = ControlBarQueryGadgetID(gg);
	struct ControlBarGadgetEntry *cgy;

	d1(KPrintF("%s/%s/%ld: GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, GadgetID));

	cgy = ControlBarFindGadgetByID(iwt, GadgetID);
	d1(KPrintF("%s/%s/%ld: cgy=%08lx\n", __FILE__, __FUNC__, __LINE__, cgy));
	if (cgy)
		return (UWORD) cgy->cgy_GadgetType;

	return (UWORD) ~0;
}

//----------------------------------------------------------------------------

static void FreeControlBarMember(struct ControlBarMember *Member)
{
	if (Member)
		{
		// cbm_Gadget is freed by GadgetBar
		if (Member->cbm_NormalImage)
			{
			DisposeObject(Member->cbm_NormalImage);
			Member->cbm_NormalImage = NULL;
			}
		if (Member->cbm_SelectedImage)
			{
			DisposeObject(Member->cbm_SelectedImage);
			Member->cbm_SelectedImage = NULL;
			}
		if (Member->cbm_FrameImage)
			{
			SCA_DisposeScalosObject(Member->cbm_FrameImage);
			Member->cbm_FrameImage = NULL;
			}

		FreeCopyString(Member->cbm_Entry.cgy_NormalImage);
		FreeCopyString(Member->cbm_Entry.cgy_SelectedImage);
		FreeCopyString(Member->cbm_Entry.cgy_DisabledImage);
		FreeCopyString(Member->cbm_Entry.cgy_HelpText);

		ScalosFree(Member);
		}
}

//----------------------------------------------------------------------------

static struct ControlBarMember *ControlBarFindMember(struct internalScaWindowTask *iwt,
	ULONG GadgetID)
{
	struct ControlBarMember *Member;

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) Member->cbm_Gadget;

		if (gg && gg->GadgetID == GadgetID)
			return Member;
		}

	return NULL;	// not found
}

//----------------------------------------------------------------------------

// Update disable state of control bar gadgets
void SetControlBarOnOff(struct internalScaWindowTask *iwt)
{
	struct ControlBarMember *Member;

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		if (Member->cbm_Gadget)
			{
			if (Member->cbm_DisableFunc)
				{
				Member->cbm_DisableFunc(iwt, Member);
				}
			else if (Member->cbm_smcom && Member->cbm_smcom->smcom_Enable)
				{
				struct Gadget *gg = (struct Gadget *) Member->cbm_Gadget;

				if (Member->cbm_smcom->smcom_Enable(iwt, NULL))
					{
					if (gg->Flags & GFLG_DISABLED)
						{
						SetGadgetAttrs(gg,
							iwt->iwt_WindowTask.wt_Window, NULL,
							GA_Disabled, FALSE,
							TAG_END);
						}
					}
				else
					{
					if (!(gg->Flags & GFLG_DISABLED))
						{
						SetGadgetAttrs(gg,
							iwt->iwt_WindowTask.wt_Window, NULL,
							GA_Disabled, TRUE,
							TAG_END);
						}
					}
				}
			}
		}

}

//----------------------------------------------------------------------------

static void ControlBarParentDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm)
{
	SetGadgetAttrs((struct Gadget *) cbm->cbm_Gadget,
		iwt->iwt_WindowTask.wt_Window, NULL,
		GA_Disabled, (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_RootWindow),
		TAG_END);
}

//----------------------------------------------------------------------------

static void ControlBarBackDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm)
{
	BOOL Disable;
	
	ScalosObtainSemaphoreShared(&iwt->iwt_WindowHistoryListSemaphore);

	Disable = IsListEmpty(&iwt->iwt_HistoryList)	   // should not happen
		|| NULL == iwt->iwt_CurrentHistoryEntry		// should not happen
		|| iwt->iwt_CurrentHistoryEntry == (struct WindowHistoryEntry *) iwt->iwt_HistoryList.lh_Head;

	ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

	d1(KPrintF("%s/%s/%ld: IsListEmpty=%ld  iwt_CurrentHistoryEntry=%08lx  Disable=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IsListEmpty(&iwt->iwt_HistoryList), iwt->iwt_CurrentHistoryEntry, Disable));

	SetGadgetAttrs((struct Gadget *) cbm->cbm_Gadget,
		iwt->iwt_WindowTask.wt_Window, NULL,
		GA_Disabled, Disable,
		TAG_END);
}

//----------------------------------------------------------------------------

static void ControlBarForwardDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm)
{
	BOOL Disable;

	ScalosObtainSemaphoreShared(&iwt->iwt_WindowHistoryListSemaphore);

	Disable = IsListEmpty(&iwt->iwt_HistoryList)	   // should not happen
		|| NULL == iwt->iwt_CurrentHistoryEntry		// should not happen
		|| iwt->iwt_CurrentHistoryEntry == (struct WindowHistoryEntry *) iwt->iwt_HistoryList.lh_TailPred;

	ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

	SetGadgetAttrs((struct Gadget *) cbm->cbm_Gadget,
		iwt->iwt_WindowTask.wt_Window, NULL,
		GA_Disabled, Disable,
		TAG_END);
}

//----------------------------------------------------------------------------

static void ControlBarHistoryListDisable(struct internalScaWindowTask *iwt, struct ControlBarMember *cbm)
{
	BOOL Disable;

	ScalosObtainSemaphoreShared(&iwt->iwt_WindowHistoryListSemaphore);

	Disable = IsListEmpty(&iwt->iwt_HistoryList);

	ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

	SetGadgetAttrs((struct Gadget *) cbm->cbm_Gadget,
		iwt->iwt_WindowTask.wt_Window, NULL,
		GA_Disabled, Disable,
		TAG_END);
}

//----------------------------------------------------------------------------

void ControlBarSwitchHistoryEntry(struct internalScaWindowTask *iwt)
{
	struct ControlBarMember *Member;

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		if (SCPGadgetType_History == Member->cbm_GadgetType)
			{
			struct WindowHistoryEntry *whe = NULL;
			struct Node *ln = NULL;
			BPTR pLock;

			GetAttr(SCAHISTORY_Active, Member->cbm_Gadget, (APTR) &ln);

			pLock = Lock(ln->ln_Name, ACCESS_READ);
			if (pLock)
				{
				whe = WindowFindHistoryEntry(iwt, pLock);
				UnLock(pLock);
				}
			if (whe)
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_NewPath,
					whe->whe_Node.ln_Name,
					SCA_ShowAllMode, whe->whe_ViewAll,
					SCA_ViewModes, whe->whe_Viewmodes,
					SCA_XOffset, whe->whe_XOffset,
					SCA_YOffset, whe->whe_YOffset,
					SCA_IconList, (SCANDIR_OK == whe->whe_ScanDirResult) ? whe->whe_IconList : NULL,
					TAG_END);
				ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
				}
			else
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_NewPath,
					ln->ln_Name,
					TAG_END);
				}
			}
		}
}

//----------------------------------------------------------------------------

struct ExtGadget *ControlBarFindGadget(struct internalScaWindowTask *iwt,
	ULONG GadgetID)
{
	struct ControlBarMember *Member;
	
	Member = ControlBarFindMember(iwt, GadgetID);

	return Member ? (struct ExtGadget *) Member->cbm_Gadget : NULL;
}

//----------------------------------------------------------------------------

void ControlBarActionButton(struct internalScaWindowTask *iwt, struct ExtGadget *gg)
{
	ULONG GadgetID = ControlBarQueryGadgetID(gg);
	struct ControlBarGadgetEntry *cgy;

	cgy = ControlBarFindGadgetByID(iwt, GadgetID);
	if (cgy)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_MenuCommand,
			cgy->cgy_Action,
			NULL,
			0);
		}
}

//----------------------------------------------------------------------------

static void ControlBarAddGadgets(struct internalScaWindowTask *iwt, const struct List *CbGadgetsList)
{
	ULONG GadgetID = CBAR_GadgetID;
	struct ControlBarMember *Member;
	struct ControlBarGadgetEntry *cgy;

	for (cgy = (struct ControlBarGadgetEntry *) CbGadgetsList->lh_Head;
		cgy != (struct ControlBarGadgetEntry *) &CbGadgetsList->lh_Tail;
		cgy = (struct ControlBarGadgetEntry *) cgy->cgy_Node.ln_Succ)
		{
		struct ControlBarMember *cbm;
		ULONG CurrentGadgetID = GadgetID++;	// create unique ID for each gadget

		switch (cgy->cgy_GadgetType)
			{
		case SCPGadgetType_BackButton:
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = AllocCopyString(GetLocString(MSGID_TOOLTIP_GADGET_CONTROLBAR_BACK));
			cbm = ControlBarCreateBackButton(cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_BackButton=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_ForwardButton:
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = AllocCopyString(GetLocString(MSGID_TOOLTIP_GADGET_CONTROLBAR_FORWARD));
			cbm = ControlBarCreateForwardButton(cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ForwardButton=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_UpButton:
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = AllocCopyString(GetLocString(MSGID_TOOLTIP_GADGET_CONTROLBAR_PARENT));
			cbm = ControlBarCreateParentButton(cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_UpButton=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_History:
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = AllocCopyString(GetLocString(MSGID_TOOLTIP_GADGET_CONTROLBAR_HISTORY));
			cbm = ControlBarCreateHistoryGadget(iwt, cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_History=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_BrowseButton:
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = AllocCopyString(GetLocString(MSGID_TOOLTIP_GADGET_CONTROLBAR_BROWSE));
			cbm = ControlBarCreateBrowseButton(cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_BrowseButton=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_ViewBy:
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = AllocCopyString(GetLocString(MSGID_TOOLTIP_GADGET_CONTROLBAR_VIEWBY));
			cbm = ControlBarCreateViewByCycle(iwt, cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ViewBy=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_ShowMode:
			FreeCopyString(cgy->cgy_HelpText);
			cgy->cgy_HelpText = AllocCopyString(GetLocString(MSGID_TOOLTIP_GADGET_CONTROLBAR_SHOWMODE));
			cbm = ControlBarCreateShowModeCycle(iwt, cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ShowMode=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_Separator:
			cbm = ControlBarCreateSeparator(cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_Separator=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		case SCPGadgetType_ActionButton:
			cbm = ControlBarCreateActionButton(cgy, CurrentGadgetID);
			d1(KPrintF("%s/%s/%ld: SCPGadgetType_ActionButton=%08lx\n", __FILE__, __FUNC__, __LINE__, cbm));
			break;
		default:
			cbm = NULL;
			break;
			}

		if (cbm)
			{
			cbm->cbm_GadgetType = cgy->cgy_GadgetType;

			// Clone ControlBarGadgetEntry
			// done't use a reference or pointer here since
			// it would become invalid when preferences are reloaded
			cbm->cbm_Entry = *cgy;
			cbm->cbm_Entry.cgy_NormalImage   = AllocCopyString(cgy->cgy_NormalImage);
			cbm->cbm_Entry.cgy_SelectedImage = AllocCopyString(cgy->cgy_SelectedImage);
			cbm->cbm_Entry.cgy_DisabledImage = AllocCopyString(cgy->cgy_DisabledImage);
			cbm->cbm_Entry.cgy_HelpText      = AllocCopyString(cgy->cgy_HelpText);

			AddHead(&iwt->iwt_ControlBarMemberList, &cbm->cbm_Node);
			}
		}

	for (Member = (struct ControlBarMember *) iwt->iwt_ControlBarMemberList.lh_Head;
		Member != (struct ControlBarMember *) &iwt->iwt_ControlBarMemberList.lh_Tail;
		Member = (struct ControlBarMember *) Member->cbm_Node.ln_Succ)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_AddToControlBar,
			Member->cbm_Gadget,
			TAG_END);
		}

}

//----------------------------------------------------------------------------


