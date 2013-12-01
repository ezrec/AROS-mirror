// ScaLibrary.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/asl.h>
#include <devices/timer.h>
#include <libraries/gadtools.h>
#include <dos/dostags.h>
#include <cybergraphx/cybergraphics.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/gadtools.h>
#include <proto/iconobject.h>
#include <proto/cybergraphics.h>
#include <proto/asl.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/menu.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"
#include "ScaLibrary.h"
#include "Scalos_rev.h"

//----------------------------------------------------------------------------

// Scalos.library initialization stuff

const char libIdString[] = "scalos.library " VERS_REV_STR " release " RELEASE " (" DATE ")";
const char scalosLibName[] = SCALOSNAME;

// local data structures

enum CheckProject_REQresult	{ CHKPRJ_Cancel=0, CHKPRJ_SelectTool };

typedef void (*NodeSortFuncType)(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook);

//----------------------------------------------------------------------------

// local functions

static struct List *AllocList(void);
static void FreeList(struct List *pl);
static struct List *GetProgramList(void);
static struct List *GetMenuCommandList(void);
static char CheckMenuCommandString(CONST_STRPTR CmdString);
static BOOL WaitForReply(struct Process *newProc, struct WblMessage *wblMsg, 
	ULONG WaitDelay, CONST_STRPTR ProgramName, BOOL Arg0Locked);
static BOOL CheckProject(Object *IconObject, struct WblInput *wbli, 
	BOOL *CliStart, BOOL *RexxStart, BOOL *Arg0Locked);
static struct ScalosClass *FindScalosClass(CONST_STRPTR ClassName);
static BOOL RemoveAppIcon(struct internalScaWindowTask *iwt, struct AppObject *appObj);
static BOOL RemoveAppWindow(struct internalScaWindowTask *iwt, struct AppObject *appObj);
static BOOL RemoveAppMenuItem(struct internalScaWindowTask *iwt, struct AppObject *appObj);
static void NewScreenTitle(CONST_STRPTR NewTitle);
static ULONG MakeWindowWBArgs(struct ScaWindowStruct *ws, 
	struct ScaIconNode *inExclude,
	struct WBArg **wbArg, ULONG ArgCount, ULONG MaxArg);
static void NodeBubbleSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook);
static void NodeSelectionSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook);
static void NodeInsertionSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook);
static void NodeQuickSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook);
static void quickSortList(struct ScalosNodeList *nodeList, struct Hook *compareHook,
	struct MinNode *firstNode, struct MinNode *lastNode);
static struct MinNode *PartitionNodeList(struct ScalosNodeList *nodeList, struct Hook *compareHook,
	struct MinNode **leftNode, struct MinNode **rightNode);
static STRPTR ChkPrjRequestFile(struct WBArg *arg);
static ULONG CountWindowWBArgs(struct ScaWindowStruct *ws,
	struct ScaIconNode *inExclude);

//----------------------------------------------------------------------------

LIBFUNC_P6(struct AppObject *, sca_NewAddAppIcon,
	D0, ULONG, ID, 
	D1, ULONG, UserData, 
	A0, Object *, iconObj, 
	A1, struct MsgPort *, msgPort, 
	A2, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase)
{
	static const struct TagItem AppIconSupportsMap[] =
		{
		{ WBAPPICONA_SupportsOpen,		INF_SupportsOpen	},
		{ WBAPPICONA_SupportsCopy,		INF_SupportsCopy	},
		{ WBAPPICONA_SupportsRename,		INF_SupportsRename	},
		{ WBAPPICONA_SupportsInformation,	INF_SupportsInformation	},
		{ WBAPPICONA_SupportsSnapshot,		INF_SupportsSnapshot	},
		{ WBAPPICONA_SupportsUnSnapshot,	INF_SupportsUnSnapshot	},
		{ WBAPPICONA_SupportsLeaveOut,		INF_SupportsLeaveOut	},
		{ WBAPPICONA_SupportsPutAway,		INF_SupportsPutAway	},
		{ WBAPPICONA_SupportsDelete,		INF_SupportsDelete	},
		{ WBAPPICONA_SupportsFormatDisk,	INF_SupportsFormatDisk	},
		{ WBAPPICONA_SupportsEmptyTrash,	INF_SupportsEmptyTrash	},
		{ TAG_DONE,				0			},
		};
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
	struct AppObject *appo;
	struct ScaIconNode *in;

	(void)ScalosBase;

	d1(kprintf("\n" "%s/%s/%ld: ID=%08lx  UserData=%08lx  IconObj=%08lx  MsgPort=%08lx  TagList=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, ID, UserData, iconObj, msgPort, TagList));

	ScalosObtainSemaphore(iwt->iwt_AppListSemaphore);

	do	{
		struct ExtGadget *gg = (struct ExtGadget *) iconObj;
		struct Hook *renderHook;

		appo = (struct AppObject *) SCA_AllocStdNode((struct ScalosNodeList *) &iwt->iwt_AppList, NTYP_AppObject);
		if (NULL == appo)
			break;

		appo->appo_Kennung = ID_SC;
		appo->appo_type = APPTYPE_AppIcon;
		appo->appo_id = ID;
		appo->appo_userdata = UserData;
		appo->appo_msgport = msgPort;
		appo->appo_object.appoo_IconObject = iconObj;
		appo->appo_TagList = CloneTagItems(TagList);

		renderHook = (struct Hook *) GetTagData(WBAPPICONA_RenderHook, (ULONG)NULL, TagList);

		SetAttrs(iconObj,
			IDTA_Type, WBAPPICON,
			IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
			IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
			IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
			IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
			IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
			IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			IDTA_FrameType, CurrentPrefs.pref_FrameType,
			IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
			IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
			IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
			IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
			IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
			IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
			IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
			IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
			IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
			IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
			IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
			IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
			IDTA_TextMode, CurrentPrefs.pref_TextMode,
			IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
			IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
			IDTA_Font, (ULONG) iwt->iwt_IconFont,
			IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
			IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
			IDTA_RenderHook, (ULONG) renderHook,
			IDTA_SizeConstraints, (ULONG) &iInfos.xii_iinfos.ii_AppWindowStruct->ws_IconSizeConstraints,
			IDTA_ScalePercentage, iInfos.xii_iinfos.ii_AppWindowStruct->ws_IconScaleFactor,
			TAG_END);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_LayoutIcon, iconObj, IOLAYOUTF_NormalImage);

		ScalosLockIconListExclusive(iwt);

		in = (struct ScaIconNode *) SCA_AllocStdNode(
			(struct ScalosNodeList *) (IsNoIconPosition(gg) ? &iwt->iwt_WindowTask.wt_LateIconList : &iwt->iwt_WindowTask.wt_IconList),
			NTYP_IconNode);

		if (NULL == in)
			{
			SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_AppList, &appo->appo_Node);
			appo = NULL;
			ScalosUnLockIconList(iwt);
			break;
			}

		d1(kprintf("%s/%s/%ld: in=%08lx  Pred=%08lx  Succ=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			in, in->in_Node.mln_Pred, in->in_Node.mln_Succ));

		in->in_Icon = iconObj;
		in->in_FileType = (struct TypeNode *) WBAPPICON;

		in->in_SupportFlags = PackBoolTags(in->in_SupportFlags, TagList, (struct TagItem *) AppIconSupportsMap);

		ScalosUnLockIconList(iwt);

		d1(kprintf("%s/%s/%ld: in=%08lx <%s>  Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, \
			in, in->in_Name, gg->LeftEdge, gg->TopEdge));

		if (!IsNoIconPosition(gg))
			{
			d1(kprintf("%s/%s/%ld: \n", __LINE__));

			if (!CheckPosition(iwt, in) && !CheckOverlap(iwt, in))
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DrawIcon, iconObj);
			}

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
		} while (0);

	ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);

	return appo;
}
LIBFUNC_END

LIBFUNC_P3(ULONG, sca_ScalosControl,
	A0, CONST_STRPTR, name,
	A1, struct TagItem *, taglist,
	A6, struct ScalosBase *, ScalosBase)
{
	ULONG TagsSuccessfullyProcessed = 0;
	struct TagItem *TState = taglist;
	struct TagItem *Tag;
	BOOL DoUpdateAll = FALSE;

	(void) ScalosBase;

	d1(kprintf("%s/%s/%ld: name=%08lx <%s>  taglist=%08lx\n", __FILE__, __FUNC__, __LINE__, name ? name : (STRPTR) "", taglist));

	(void) name;

	while ((Tag = NextTagItem(&TState)))
		{
		LONG ErrorCode = RETURN_OK;

		switch (Tag->ti_Tag)
			{
		case SCALOSCTRLA_GetDefaultStackSize:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetDefaultStackSize ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				{
				*((ULONG *) Tag->ti_Data) = CurrentPrefs.pref_DefaultStackSize;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetDefaultStackSize:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetDefaultStackSize ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data >= 8192L)
				{
				CurrentPrefs.pref_DefaultStackSize = Tag->ti_Data;
				}
			else
				ErrorCode = ERROR_BAD_NUMBER;
			break;

		case SCALOSCTRLA_GetProgramList:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetProgramList ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				{
				*((struct List **) Tag->ti_Data) = GetProgramList();

				if (NULL == *((struct List **) Tag->ti_Data))
					ErrorCode = ERROR_NO_FREE_STORE;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_FreeProgramList:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_FreeProgramList ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			FreeList((struct List *) Tag->ti_Data);
			break;

		case SCALOSCTRLA_GetMenuCommandList:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetMenuCommandList ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				{
				*((struct List **) Tag->ti_Data) = GetMenuCommandList();

				if (NULL == *((struct List **) Tag->ti_Data))
					ErrorCode = ERROR_NO_FREE_STORE;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_FreeMenuCommandList:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_FreeMenuCommandList ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			FreeList((struct List *) Tag->ti_Data);
			break;

		case SCALOSCTRLA_GetCloseWBDisabled:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetCloseWBDisabled ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = (CurrentPrefs.pref_DisableCloseWorkbench != 0);
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetCloseWBDisabled:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetCloseWBDisabled ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			CurrentPrefs.pref_DisableCloseWorkbench = (Tag->ti_Data != 0);
			break;

		case SCALOSCTRLA_GetSplashEnable:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetSplashEnable ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = (CurrentPrefs.pref_EnableSplash != 0);
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetSplashEnable:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetSplashEnable ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			CurrentPrefs.pref_EnableSplash = (Tag->ti_Data != 0);
			break;

		case SCALOSCTRLA_GetToolTipEnable:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetToolTipEnable ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = (CurrentPrefs.pref_EnableTooltips != 0);
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetToolTipEnable:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetToolTipEnable ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			CurrentPrefs.pref_EnableTooltips = (Tag->ti_Data != 0);
			break;

		case SCALOSCTRLA_GetToolTipDelay:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetToolTipDelay ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = CurrentPrefs.pref_ToolTipDelaySeconds;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetToolTipDelay:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetToolTipDelay ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data > 0 && Tag->ti_Data < 600)
				CurrentPrefs.pref_ToolTipDelaySeconds = Tag->ti_Data;
			else
				ErrorCode = ERROR_BAD_NUMBER;
			break;

		case SCALOSCTRLA_GetOldDragIconMode:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetOldDragIconMode ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = (CurrentPrefs.pref_UseOldDragIcons != 0);
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetOldDragIconMode:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetOldDragIconMode ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			CurrentPrefs.pref_UseOldDragIcons = (Tag->ti_Data != 0);
			break;

		case SCALOSCTRLA_GetTypeRestartTime:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetTypeRestartTime ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = CurrentPrefs.pref_TypeRestartTime;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetTypeRestartTime:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetTypeRestartTime ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data > 0)
				CurrentPrefs.pref_TypeRestartTime = Tag->ti_Data;
			else
				ErrorCode = ERROR_BAD_NUMBER;
			break;

		case SCALOSCTRLA_GetEmulationMode:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetEmulationMode ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = MainWindowTask->emulation != 0;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_GetStatusBarEnable:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetStatusBarEnable ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = (CurrentPrefs.pref_DisplayStatusBar != 0);
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetStatusBarEnable:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetStatusBarEnable ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			CurrentPrefs.pref_DisplayStatusBar = (Tag->ti_Data != 0);
			break;

		case SCALOSCTRLA_GetStripedTextWindows:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetStripedTextWindows ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = (CurrentPrefs.pref_TextWindowStriped != 0);
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetStripedTextWindows:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetStripedTextWindows ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			CurrentPrefs.pref_TextWindowStriped = (Tag->ti_Data != 0);
			break;

		case SCALOSCTRLA_GetWindowDropMarkMode:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetWindowDropMarkMode ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = CurrentPrefs.pref_WindowDropMarkMode;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetWindowDropMarkMode:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetWindowDropMarkMode ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data >= IDTV_WinDropMark_Never && Tag->ti_Data <= IDTV_WinDropMark_Always)
				CurrentPrefs.pref_WindowDropMarkMode = Tag->ti_Data;
			else
				ErrorCode = ERROR_BAD_NUMBER;
			break;

		case SCALOSCTRLA_GetDisplayDragCount:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetDisplayDragCount ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = (CurrentPrefs.pref_ShowDDCountText != 0);
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetDisplayDragCount:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetDisplayDragCount ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			CurrentPrefs.pref_ShowDDCountText = (Tag->ti_Data != 0);
			break;

		case SCALOSCTRLA_GetSupportedIconTypes:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetSupportedIconTypes ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = CurrentPrefs.pref_SupportedIconTypes;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetSupportedIconTypes:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetSupportedIconTypes ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (CurrentPrefs.pref_SupportedIconTypes != Tag->ti_Data)
				{
				DoUpdateAll = TRUE;
				CurrentPrefs.pref_SupportedIconTypes = Tag->ti_Data;
				}
			break;

		case SCALOSCTRLA_GetCopyHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetCopyHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((struct Hook **) Tag->ti_Data) = globalCopyHook;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetCopyHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetCopyHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			ScalosObtainSemaphore(&CopyHookSemaphore);
			globalCopyHook = (struct Hook *) Tag->ti_Data;
			ScalosReleaseSemaphore(&CopyHookSemaphore);
			break;

		case SCALOSCTRLA_GetCopyBuffSize:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetCopyBuffSize ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((ULONG *) Tag->ti_Data) = CurrentPrefs.pref_CopyBuffLen;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetCopyBuffSize:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetCopyBuffSize ti_Data=%lu\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data >= 4096 && Tag->ti_Data <= 0x400000)
				CurrentPrefs.pref_CopyBuffLen = Tag->ti_Data;
			else
				ErrorCode = ERROR_BAD_NUMBER;
			break;

		case SCALOSCTRLA_GetDeleteHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetDeleteHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((struct Hook **) Tag->ti_Data) = globalDeleteHook;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetDeleteHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetDeleteHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (ScalosAttemptSemaphore(&DeleteHookSemaphore))
				{
				globalDeleteHook = (struct Hook *) Tag->ti_Data;
				ScalosReleaseSemaphore(&DeleteHookSemaphore);
				}
			else
				ErrorCode = ERROR_OBJECT_IN_USE;
			break;

		case SCALOSCTRLA_GetTextInputHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetTextInputHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((struct Hook **) Tag->ti_Data) = globalTextInputHook;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_SetTextInputHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_SetTextInputHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (ScalosAttemptSemaphore(&TextInputHookSemaphore))
				{
				globalTextInputHook = (struct Hook *) Tag->ti_Data;
				ScalosReleaseSemaphore(&TextInputHookSemaphore);
				}
			else
				ErrorCode = ERROR_OBJECT_IN_USE;
			break;

		case SCALOSCTRLA_AddCloseWBHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_AddCloseWBHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				{
				BOOL Found = FALSE;
				struct Hook *hook;
				struct Hook *newHook = (struct Hook *) Tag->ti_Data;
			
				ScalosObtainSemaphore(&CloseWBHookListSemaphore);

				for (hook = (struct Hook *) globalCloseWBHookList.lh_Head;
					hook != (struct Hook *) &globalCloseWBHookList.lh_Tail;
					hook = (struct Hook *) hook->h_MinNode.mln_Succ)
					{
					if (hook == newHook)
						{
						Found = TRUE;
						break;
						}
					}

				if (Found)
					ErrorCode = ERROR_OBJECT_EXISTS;
				else
					AddTail(&globalCloseWBHookList, (struct Node *) newHook);

				ScalosReleaseSemaphore(&CloseWBHookListSemaphore);
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_RemCloseWBHook:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_RemCloseWBHook ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				{
				BOOL Found = FALSE;
				struct Hook *hook;
				struct Hook *oldHook = (struct Hook *) Tag->ti_Data;
			
				ScalosObtainSemaphore(&CloseWBHookListSemaphore);

				for (hook = (struct Hook *) globalCloseWBHookList.lh_Head;
					hook != (struct Hook *) &globalCloseWBHookList.lh_Tail;
					hook = (struct Hook *) hook->h_MinNode.mln_Succ)
					{
					if (hook == oldHook)
						{
						Found = TRUE;
						break;
						}
					}

				if (Found)
					Remove((struct Node *) oldHook);
				else
					ErrorCode = ERROR_OBJECT_NOT_FOUND;

				ScalosReleaseSemaphore(&CloseWBHookListSemaphore);
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case SCALOSCTRLA_GetSemaphoreList:
			d1(kprintf("%s/%s/%ld: SCALOSCTRLA_GetSemaphoreList ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Data));
			if (Tag->ti_Data)
				*((struct ScalosSemaphoreList **) Tag->ti_Data) = GlobalSemaphoreList;
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		default:
			d1(kprintf("%s/%s/%ld: unknown ti_Tag=%08lx  ti_Data=%08lx\n", __FILE__, __FUNC__, __LINE__, Tag->ti_Tag, Tag->ti_Data));
			ErrorCode = ERROR_ACTION_NOT_KNOWN;
			break;
			}

		d1(kprintf("%s/%s/%ld: ErrorCode=%ld\n", __FILE__, __FUNC__, __LINE__, ErrorCode));

		if (RETURN_OK == ErrorCode)
			TagsSuccessfullyProcessed++;
		else
			SetIoErr(ErrorCode);
		}

	if (DoUpdateAll && iInfos.xii_iinfos.ii_MainWindowStruct)
		{
		UpdateAllProg((struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask, NULL);
		}

	d1(kprintf("%s/%s/%ld: TagsSuccessfullyProcessed=%lu\n", __FILE__, __FUNC__, __LINE__, TagsSuccessfullyProcessed));

	return TagsSuccessfullyProcessed;
}
LIBFUNC_END


static struct List *AllocList(void)
{
	struct List *pl = ScalosAlloc(sizeof(struct List));

	if (pl)
		{
		pl->lh_Type = 0;

		NewList(pl);
		}

	return pl;
}


static void FreeList(struct List *pl)
{
	struct Node *xNode;

	if (NULL == pl)
		return;

	while ((xNode = RemHead(pl)))
		{
		if (xNode->ln_Name)
			{
			ScalosFree(xNode->ln_Name);
			xNode->ln_Name = NULL;
			}
		ScalosFree(xNode);
		}

	free(pl);
}


static struct List *GetProgramList(void)
{
	struct List *pl = AllocList();

	if (pl)
		{
		struct Node *wbNode;

		ScalosObtainSemaphoreShared(&WBStartListSema);

		for (wbNode = WBStartList.lh_Head; wbNode != (struct Node *) &WBStartList.lh_Tail; wbNode = wbNode->ln_Succ)
			{
			struct Node *newNode = ScalosAlloc(sizeof(struct Node));

			d1(kprintf("%s/%s/%ld: wbNode=%08lx  Name=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				wbNode, wbNode->ln_Name));

			if (newNode)
				{
				newNode->ln_Type = NT_USER;
				newNode->ln_Name = ScalosAlloc(strlen(wbNode->ln_Name) + 1);
				if (newNode->ln_Name)
					strcpy(newNode->ln_Name, wbNode->ln_Name);

				AddTail(pl, newNode);
				}
			}

		ScalosReleaseSemaphore(&WBStartListSema);
		}

	return pl;
}


static struct List *GetMenuCommandList(void)
{
	struct List *pl = AllocList();

	if (pl)
		{
		const struct ScalosMenuCommand *cmd;

		for (cmd = &MenuCommandTable[0]; cmd->smcom_CommandName; cmd++)
			{
			struct Node *newNode = ScalosAlloc(sizeof(struct Node));

			if (newNode)
				{
				newNode->ln_Type = NT_USER;
				newNode->ln_Name = ScalosAlloc(strlen(cmd->smcom_CommandName) + 1);
				if (newNode->ln_Name)
					strcpy(newNode->ln_Name, cmd->smcom_CommandName);

				AddTail(pl, newNode);
				}
			}
		}

	return pl;
}


// SCA_NewAddAppMenuItem()
// AddAppMenuItemA()
LIBFUNC_P6(struct AppObject *, sca_NewAddAppMenuItem,
	D0, ULONG, ID, 
	D1, ULONG, UserData, 
	A0, CONST_STRPTR, MenuText, 
	A1, struct MsgPort *, msgPort, 
	A2, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
	struct Menu *theNewMenu = NULL;
	struct AppObject *appo;
	BOOL WinListLocked = FALSE;
	BOOL Success = FALSE;

	(void)ScalosBase;

	ScalosObtainSemaphore(iwt->iwt_AppListSemaphore);

	d1(kprintf("\n" "%s/%s/%ld: ID=%08lx  UserData=%08lx  MenuText=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, ID, UserData, MenuText));

	do	{
		CONST_STRPTR CmdString;
		CONST_STRPTR lpc;
		char CommKey[2];
		struct AppMenuInfo **SubKey, **TitleKey;

		appo = (struct AppObject *) SCA_AllocStdNode((struct ScalosNodeList *) &iwt->iwt_AppList, NTYP_AppObject);
		if (NULL == appo)
			break;

		CmdString = (STRPTR) GetTagData(WBAPPMENUA_CommandKeyString, (ULONG) "", TagList);

		SubKey = (struct AppMenuInfo **) GetTagData(WBAPPMENUA_GetKey, (ULONG)NULL, TagList);
		TitleKey = (struct AppMenuInfo **) GetTagData(WBAPPMENUA_GetTitleKey, (ULONG)NULL, TagList);

		appo->appo_Kennung = ID_SC;
		appo->appo_type = APPTYPE_AppMenuItem;
		appo->appo_id = ID;
		appo->appo_userdata = UserData;
		appo->appo_msgport = msgPort;
		appo->appo_object.appoo_MenuInfo = ScalosAlloc(sizeof(struct AppMenuInfo));
		appo->appo_userdata2 = NULL;

		if (NULL == appo->appo_object.appoo_MenuInfo)
			break;

		memset(CommKey, 0, sizeof(CommKey));
		memset(appo->appo_object.appoo_MenuInfo, 0, sizeof(struct AppMenuInfo));

		CommKey[0] = CheckMenuCommandString(CmdString);

		NewList(&appo->appo_object.appoo_MenuInfo->ami_ChildList);

		appo->appo_object.appoo_MenuInfo->ami_Magic = ID_SAMI;
		appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_CommKey = *CommKey ? AllocCopyString(CommKey) : NULL;
		appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Flags = 0;
		appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_MutualExclude = 0L;
		appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_UserData = appo;
		appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Type = NM_ITEM;

		d1(kprintf("%s/%s/%ld: TitleKey=%08lx  CommKey=%02lx\n", __FILE__, __FUNC__, __LINE__, appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_CommKey));
		d1(kprintf("%s/%s/%ld: MenuInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, appo->appo_object.appoo_MenuInfo));
		d1(kprintf("%s/%s/%ld: TitleKey=%08lx  SubKey=%08lx\n", __FILE__, __FUNC__, __LINE__, TitleKey, SubKey));

		if (TitleKey)
			{
			appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Type = NM_TITLE;

			*TitleKey = appo->appo_object.appoo_MenuInfo;
			}
		else if (SubKey)
			{
			*SubKey = appo->appo_object.appoo_MenuInfo;
			}

		appo->appo_object.appoo_MenuInfo->ami_Parent = (struct AppMenuInfo *) GetTagData(WBAPPMENUA_UseKey, (ULONG)NULL, TagList);

		if (appo->appo_object.appoo_MenuInfo->ami_Parent)
			{
			if (!(TypeOfMem(appo->appo_object.appoo_MenuInfo->ami_Parent) & MEMF_PUBLIC)
				|| ID_SAMI != appo->appo_object.appoo_MenuInfo->ami_Parent->ami_Magic)
				{
				appo->appo_object.appoo_MenuInfo->ami_Parent = NULL;
				}
			}

		d1(kprintf("%s/%s/%ld: ami_Parent=%08lx\n", __FILE__, __FUNC__, __LINE__, appo->appo_object.appoo_MenuInfo->ami_Parent));

		if (appo->appo_object.appoo_MenuInfo->ami_Parent)
			{
			if (NM_TITLE == appo->appo_object.appoo_MenuInfo->ami_Parent->ami_NewMenu.nm_Type)
				{
				appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Type = NM_ITEM;
				}
			else
				{
				appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Type = NM_SUB;

				if (SubKey)
					break;		// cascading of submenus is not allowed
				}
			}

		d1(kprintf("%s/%s/%ld: nm_Type=%ld\n", __FILE__, __FUNC__, __LINE__, appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Type));

		// starting with V44, any menu
		// label consisting entirely of '-', '_' or '~' characters will
		// result in a separator bar to be added in place of a textual item.

		appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Label = NM_BARLABEL;
		for (lpc=MenuText; *lpc; lpc++)
			{
			if (*lpc != '-' && *lpc != '_' && *lpc != '~')
				{
				d1(kprintf("%s/%s/%ld: lpc=<%s>\n", __FILE__, __FUNC__, __LINE__, lpc));
				appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Label = AllocCopyString(MenuText);
				break;
				}
			}

		d1(kprintf("%s/%s/%ld: nm_Label=%08lx\n", __FILE__, __FUNC__, __LINE__, appo->appo_object.appoo_MenuInfo->ami_NewMenu.nm_Label));

		Success = AppMenu_AddItem(appo->appo_object.appoo_MenuInfo);
		} while (0);

	if (WinListLocked)
		SCA_UnLockWindowList();

	ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (Success)
		AppMenu_ResetMenu();
	else
		{
		if (theNewMenu)
			FreeMenus(theNewMenu);
		if (appo)
			{
			AppMenu_DisposeAppMenuInfo(appo->appo_object.appoo_MenuInfo);
			SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_AppList, &appo->appo_Node);
			}
		}

	return appo;
}
LIBFUNC_END


// check if menu command character *CmdString is already in use
static char CheckMenuCommandString(CONST_STRPTR CmdString)
{
	char CmdChar;
	struct Menu *theMenu;

	if (NULL == CmdString)
		return 0;

	CmdChar = *CmdString;
	if ('\0' == CmdChar)
		return 0;

	CmdChar = ToUpper(CmdChar);

	for (theMenu=MainMenu; theMenu; theMenu=theMenu->NextMenu)
		{
		struct MenuItem *mItem;

		for (mItem=theMenu->FirstItem; mItem; mItem=mItem->NextItem)
			{
			struct MenuItem *mSub;

			if ((mItem->Flags & COMMSEQ) && mItem->Command == CmdChar)
				return 0;

			for (mSub=mItem->SubItem; mSub; mSub=mSub->NextItem)
				{
				if ((mSub->Flags & COMMSEQ) && mSub->Command == CmdChar)
					return 0;
				}
			}
		}

	return CmdChar;
}


// SCA_WBStart()
LIBFUNC_P4(BOOL, sca_WBStart,
	A0, struct WBArg *, ArgArray,
	A1, struct TagItem *, TagList, 
	D0, ULONG, NumArgs,
	A6, struct ScalosBase *, ScalosBase)
{
	struct WblMessage *wblArg = NULL;
	BOOL Success = FALSE;
	struct Process *newProc;
	BOOL cliStart = FALSE;
	BOOL RexxStart = FALSE;
	BOOL Arg0Locked = FALSE;

	(void) ScalosBase;

	d1(KPrintF("\n" "%s/%s/%ld: START ArgArray=%08lx  TagList=%08lx  NumArgs=%ld\n", \
		__FILE__, __FUNC__, __LINE__, ArgArray, TagList, NumArgs));

	do	{
		Object *IconObject = NULL;
		ULONG WaitDelay;
		ULONG n;

		if (NumArgs < 1)
			break;

		wblArg = AllocVec(sizeof(struct WblMessage) + (1 + NumArgs) * sizeof(struct WBArg), MEMF_CLEAR | MEMF_PUBLIC);
		if (NULL == wblArg)
			break;

		d1(kprintf("%s/%s/%ld: wblArg=%08lx\n", __FILE__, __FUNC__, __LINE__, wblArg));

		wblArg->wbl_Magic = ID_WBLM;
		wblArg->wbl_IO.wbl_Input.wbli_Flags = GetTagData(SCA_Flags, 0, TagList);
		wblArg->wbl_IO.wbl_Input.wbli_Priority = GetTagData(SCA_Priority, 0, TagList);
		wblArg->wbl_IO.wbl_Input.wbli_StackSize = GetTagData(SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize, TagList);
		wblArg->wbl_IO.wbl_Input.wbli_NumArgs = NumArgs;

		d1(kprintf("%s/%s/%ld: Flags=%08lx  StackSize=%ld\n", __FILE__, __FUNC__, __LINE__, \
			wblArg->wbl_IO.wbl_Input.wbli_Flags, wblArg->wbl_IO.wbl_Input.wbli_StackSize));

		for (n=0; n<NumArgs; n++)
			{
			wblArg->wbl_IO.wbl_Input.wbli_ArgList[n].wa_Lock = ArgArray[n].wa_Lock;
			wblArg->wbl_IO.wbl_Input.wbli_ArgList[n].wa_Name = AllocCopyString(ArgArray[n].wa_Name);

			d1(kprintf("%s/%s/%ld: Arg[%ld]:  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
				n, wblArg->wbl_IO.wbl_Input.wbli_ArgList[n].wa_Name));
			debugLock_d1(wblArg->wbl_IO.wbl_Input.wbli_ArgList[n].wa_Lock);
			}

		WaitDelay = GetTagData(SCA_WaitTimeout, CurrentPrefs.pref_DoWaitDelay, TagList);

		d1(kprintf("%s/%s/%ld: WaitDelay=%ld\n", __FILE__, __FUNC__, __LINE__, WaitDelay));

		if (!(wblArg->wbl_IO.wbl_Input.wbli_Flags & SCAF_WBStart_NoIcon))
			{
			IconObject = (Object *) GetTagData(SCA_IconObject, (ULONG)NULL, TagList);

			d1(KPrintF("%s/%s/%ld: IconObject=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObject));

			if (NULL == IconObject)
				{
				BPTR oldDir = CurrentDir(wblArg->wbl_IO.wbl_Input.wbli_ArgList[0].wa_Lock);

				IconObject = NewIconObjectTags(wblArg->wbl_IO.wbl_Input.wbli_ArgList[0].wa_Name,
					TAG_END);

				if (IconObject)
					{
					GetAttr(IDTA_Stacksize, IconObject, &wblArg->wbl_IO.wbl_Input.wbli_StackSize);

					if (!CheckProject(IconObject, &wblArg->wbl_IO.wbl_Input, &cliStart, &RexxStart, &Arg0Locked))
						{
						DisplayScreenTitleError(NULL, MSGID_ERROR_NO_DEFAULTTOOL);
						break;
						}

					d1(KPrintF("%s/%s/%ld: cliStart=%ld  RexxStart=%ld\n", __FILE__, __FUNC__, __LINE__, cliStart, RexxStart));

					DisposeIconObject(IconObject);
					}

				CurrentDir(oldDir);
				}
			else
				{
				if (!CheckProject(IconObject, &wblArg->wbl_IO.wbl_Input, &cliStart, &RexxStart, &Arg0Locked))
					{
					DisplayScreenTitleError(NULL, MSGID_ERROR_NO_DEFAULTTOOL);
					break;
					}

				GetAttr(IDTA_Stacksize, IconObject, &wblArg->wbl_IO.wbl_Input.wbli_StackSize);
				}
			}

		if (wblArg->wbl_IO.wbl_Input.wbli_StackSize < CurrentPrefs.pref_DefaultStackSize)
			wblArg->wbl_IO.wbl_Input.wbli_StackSize = CurrentPrefs.pref_DefaultStackSize;

		d1(KPrintF("%s/%s/%ld: cliStart=%ld  RexxStart=%ld\n", __FILE__, __FUNC__, __LINE__, cliStart, RexxStart));

		if (cliStart)
			{
			d1(KPrintF("%s/%s/%ld: CliStart\n", __FUNC__, __LINE__));

			Success = CLIStart(wblArg->wbl_IO.wbl_Input.wbli_ArgList[0].wa_Lock,
				wblArg->wbl_IO.wbl_Input.wbli_ArgList[0].wa_Name, 
				IconObject, 
				wblArg->wbl_IO.wbl_Input.wbli_StackSize);
			}
		else if (RexxStart)
			{
			d1(KPrintF("%s/%s/%ld: RexxStart\n", __FUNC__, __LINE__));
			Success = ArexxToolStart(&MainWindowTask->mwt,
				wblArg->wbl_IO.wbl_Input.wbli_ArgList[0].wa_Lock, 
				wblArg->wbl_IO.wbl_Input.wbli_ArgList[0].wa_Name);
			}
		else
			{
			STATIC_PATCHFUNC(WblTask)
			ULONG Flags = wblArg->wbl_IO.wbl_Input.wbli_Flags;

			d1(KPrintF("%s/%s/%ld: WBStart\n", __FILE__, __FUNC__, __LINE__));

			newProc = CreateNewProcTags(NP_CurrentDir, NULL,
				NP_WindowPtr, NULL,
				NP_Priority, wblArg->wbl_IO.wbl_Input.wbli_Priority,
				NP_Name, (ULONG) "WBL",
				NP_Entry, (ULONG) PATCH_NEWFUNC(WblTask),
				TAG_END);
			if (NULL == newProc)
				break;

			d1(kprintf("%s/%s/%ld: newProc=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, newProc, Flags));

			if (Flags & SCAF_WBStart_Wait)
				{
				d1(KPrintF("%s/%s/%ld: WaitForReply\n", __FUNC__, __LINE__));
				Success = WaitForReply(newProc, wblArg, WaitDelay, ArgArray[0].wa_Name, Arg0Locked);
				d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
				}
			else
				{
				PutMsg(&newProc->pr_MsgPort, &wblArg->wbl_Message);
				Success = TRUE;
				}

			wblArg = NULL;		// don't free wblArg
			}
		} while (0);

	d1(kprintf("%s/%s/%ld: wblArg=%08lx\n", __FILE__, __FUNC__, __LINE__, wblArg));

	if (wblArg)
		FreeVec(wblArg);

	return Success;
}
LIBFUNC_END


static BOOL WaitForReply(struct Process *newProc, struct WblMessage *wblMsg, 
	ULONG WaitDelay, CONST_STRPTR ProgramName, BOOL Arg0Locked)
{
	BOOL Success = FALSE;
	struct MsgPort *ReplyPort;
	T_TIMEREQUEST *TimerIO = NULL;
	BOOL DeviceOpened = FALSE;
	BOOL TimerPending = FALSE;
	struct DoWait *dw;

	do	{
		BOOL Waiting;

		ReplyPort = CreateMsgPort();

		d1(kprintf("%s/%s/%ld: ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, ReplyPort));
		if (NULL == ReplyPort)
			break;

		TimerIO = (T_TIMEREQUEST *) CreateIORequest(ReplyPort, sizeof(T_TIMEREQUEST));

		d1(kprintf("%s/%s/%ld: TimerIO=%08lx\n", __FILE__, __FUNC__, __LINE__, TimerIO));
		if (NULL == TimerIO)
			break;

		if (0 != OpenDevice(TIMERNAME, UNIT_VBLANK, &TimerIO->tr_node, 0L))
			break;

		d1(kprintf("%s/%s/%ld: OpenDevice OK\n", __LINE__));
		DeviceOpened = TRUE;

		ScalosObtainSemaphore(&DoWaitSemaphore);

		dw = (struct DoWait *) SCA_AllocNode((struct ScalosNodeList *)(APTR) &DoWaitList, sizeof(struct DoWait));

		d1(kprintf("%s/%s/%ld: dw=%08lx\n", __FILE__, __FUNC__, __LINE__, dw));
		if (NULL == dw)
			{
			ScalosReleaseSemaphore(&DoWaitSemaphore);
			break;
			}

		dw->dwai_msgport = ReplyPort;
		dw->dwai_wblmsg = wblMsg;

		d1(kprintf("%s/%s/%ld: wblmsg=%08lx\n", __FILE__, __FUNC__, __LINE__, dw->dwai_wblmsg));

		ScalosReleaseSemaphore(&DoWaitSemaphore);

		wblMsg->wbl_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;

		Success = TRUE;
		d1(kprintf("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, &wblMsg->wbl_Message));
		PutMsg(&newProc->pr_MsgPort, &wblMsg->wbl_Message);

		do	{
			struct Message *msg;

			Waiting = FALSE;

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__, WaitDelay));

			if (WaitDelay != 0)
				{
				TimerIO->tr_time.tv_secs = WaitDelay;
				TimerIO->tr_time.tv_micro = 0;
				TimerIO->tr_node.io_Command = TR_ADDREQUEST;

				TimerPending = TRUE;
				BeginIO(&TimerIO->tr_node);
				}

			// wait for program completion or timeout
			do	{
				WaitPort(ReplyPort);
				msg = GetMsg(ReplyPort);
				} while (NULL == msg);

			d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

			if (msg == &TimerIO->tr_node.io_Message)
				{
				// Program has not yet finished, timeout
				TimerPending = FALSE;
				WaitIO(&TimerIO->tr_node);

				if (UseRequestArgs(NULL, MSGID_DOWAITERRNAME, MSGID_GADGETSNAME,
					1,
					ProgramName))
					{
					Waiting = TRUE;
					}
				}
			else
				{
				// program has finished
				struct DoWaitFinished *dwf = (struct DoWaitFinished *) msg;

				d1(kprintf("%s/%s/%ld: program finished dwf=%08lx\n", __FILE__, __FUNC__, __LINE__, dwf));

				FreeVec(dwf);
				}
			} while (Waiting);

		d1(kprintf("%s/%s/%ld: \n", __LINE__));

		ScalosObtainSemaphore(&DoWaitSemaphore);
		SCA_FreeNode((struct ScalosNodeList *)(APTR) &DoWaitList, &dw->dwai_Node);
		ScalosReleaseSemaphore(&DoWaitSemaphore);

		d1(kprintf("%s/%s/%ld: \n", __LINE__));
		} while (0);

	d1(kprintf("%s/%s/%ld: \n", __LINE__));

	if (!Success)
		{
		ULONG n;

		d1(kprintf("%s/%s/%ld: \n", __LINE__));

		for (n=0; n<wblMsg->wbl_IO.wbl_Input.wbli_NumArgs; n++)
			{
			FreeCopyString(wblMsg->wbl_IO.wbl_Input.wbli_ArgList[n].wa_Name);
			wblMsg->wbl_IO.wbl_Input.wbli_ArgList[n].wa_Name = NULL;
			}
		if (Arg0Locked)
			UnLock(wblMsg->wbl_IO.wbl_Input.wbli_ArgList[0].wa_Lock);
		}
	if (TimerIO)
		{
		d1(kprintf("%s/%s/%ld: \n", __LINE__));

		if (TimerPending)
			{
			AbortIO(&TimerIO->tr_node);
			WaitIO(&TimerIO->tr_node);
			}
		if (DeviceOpened)
			CloseDevice(&TimerIO->tr_node);

		DeleteIORequest((APTR)TimerIO);
		}
	if (ReplyPort)
		DeleteMsgPort(ReplyPort);

	d1(kprintf("%s/%s/%ld: finished  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


static BOOL CheckProject(Object *IconObject, struct WblInput *wbli, BOOL *cliStart, BOOL *RexxStart, BOOL *Arg0Locked)
{
	Object *DefaultIconObject = NULL;
	STRPTR Path = NULL;
	ULONG IconType;
	ULONG ul;
	STRPTR tt;

	tt = NULL;
	if (DoMethod(IconObject, IDTM_FindToolType, "TOOLPRI", &tt))
		{
		d1(KPrintF("%s/%s/%ld: TOOLPRI=<%s>\n", __FILE__, __FUNC__, __LINE__, tt));

		if (DoMethod(IconObject, IDTM_GetToolTypeValue, tt, &ul))
			wbli->wbli_Priority = ul;
		}

	tt = NULL;
	if (DoMethod(IconObject, IDTM_FindToolType, "CLI", &tt))
		*cliStart = TRUE;

	tt = NULL;
	if (DoMethod(IconObject, IDTM_FindToolType, "REXX", &tt))
		*RexxStart = TRUE;

	GetAttr(IDTA_Type, IconObject, &IconType);

	d1(KPrintF("%s/%s/%ld: IconType=%08lx\n", __FILE__, __FUNC__, __LINE__, IconType));

	if (WBPROJECT == IconType && !(wbli->wbli_Flags & SCAF_WBStart_NoIconCheck))
		{
		ULONG n;
		BPTR oldDir;
		STRPTR pp;
		char ch;
		STRPTR DefaultTool = NULL;

		GetAttr(IDTA_DefaultTool, IconObject, (APTR) &DefaultTool);

		if (NULL == DefaultTool)
			{
			// If no default tool is present, try to get a default icon
			// and if present, use default tool from the default icon
			DefaultIconObject = ReturnDefIconObjTags(wbli->wbli_ArgList[0].wa_Lock,
				wbli->wbli_ArgList[0].wa_Name,
				TAG_END);

			if (NULL != DefaultIconObject)
				GetAttr(IDTA_DefaultTool, DefaultIconObject, (APTR) &DefaultTool);
			}

		d1(KPrintF("%s/%s/%ld: DefaultTool=%08lx\n", __FILE__, __FUNC__, __LINE__, DefaultTool));
		if (NULL == DefaultTool)
			{
			STRPTR IconName = (STRPTR) "";
			ULONG ReqResult;

			GetAttr(DTA_Name, IconObject, (APTR) &IconName);

			ReqResult = UseRequestArgs(NULL, MSGID_NODEFAULTTOOL_REQ, MSGID_NODEFAULTTOOL_REQ_GADGETS,
				1,
				IconName);

			d1(KPrintF("%s/%s/%ld: ReqResult=%ld\n", __FILE__, __FUNC__, __LINE__, ReqResult));

			switch (ReqResult)
				{
			case CHKPRJ_Cancel:
				break;

			case CHKPRJ_SelectTool:
				DefaultTool = Path = ChkPrjRequestFile(&wbli->wbli_ArgList[0]);
				break;
				}
			}

		if (NULL == DefaultTool)
			{
			if (DefaultIconObject)
				DisposeIconObject(DefaultIconObject);
			return FALSE;
			}

		d1(KPrintF("%s/%s/%ld: DefaultTool=<%s>\n", __FILE__, __FUNC__, __LINE__, DefaultTool));

		// make room for new first WBarg
		for (n=wbli->wbli_NumArgs; n>0; n--)
			{
			wbli->wbli_ArgList[n] = wbli->wbli_ArgList[n-1];
			}
		wbli->wbli_NumArgs++;

		oldDir = CurrentDir(wbli->wbli_ArgList[0].wa_Lock);

		pp = PathPart(DefaultTool);
		ch = *pp;
		*pp = '\0';

		d1(KPrintF("%s/%s/%ld: PathPart=<%s>\n", __FILE__, __FUNC__, __LINE__, pp));

		wbli->wbli_ArgList[0].wa_Lock = Lock(DefaultTool, ACCESS_READ);

		*pp = ch;

		wbli->wbli_ArgList[0].wa_Name = AllocCopyString(FilePart(DefaultTool));

		d1(KPrintF("%s/%s/%ld: ArgList[0].wa_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, wbli->wbli_ArgList[0].wa_Name));
		debugLock_d1(wbli->wbli_ArgList[0].wa_Lock);

		wbli->wbli_Flags |= SCAF_WBStart_PathSearch;
		*Arg0Locked = TRUE;

		d1(KPrintF("%s/%s/%ld: Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, wbli->wbli_Flags));

		CurrentDir(oldDir);
		}

	if (DefaultIconObject)
		DisposeIconObject(DefaultIconObject);
	if (Path)
		FreePathBuffer(Path);

	return TRUE;
}


// SCA_MakeScalosClass() implementation
LIBFUNC_P5(struct ScalosClass *, sca_MakeScalosClass,
	A0, CONST_STRPTR, ClassName, 
	A1, CONST_STRPTR, SuperClassName, 
	D0, UWORD, InstSize, 
	A2, APTR, DispFunc,
	A6, struct ScalosBase *, ScalosBase)
{
	struct ScalosClass *NewScalosClass = NULL;
	Class *NewClass = NULL;
	ScalosObtainSemaphore(&ClassListSemaphore);

	(void)ScalosBase;

	d1(kprintf("%s/%s/%ld: ClassName=<%s>  SuperClassName=<%s>  InstSize=%lu  DispFunc=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, ClassName, SuperClassName, InstSize, DispFunc));

	do	{
		struct ScalosClass *ClassList = NULL;
		struct ScalosClass *SuperClass;
		size_t SuperClassLen = strlen(SuperClassName);

		if (SuperClassLen < 4 || 0 != strcmp(SuperClassName + SuperClassLen - 4, ".sca"))
			{
			// All classes whose names don't end with ".sca" are inherited from
			// standard BOOPSI classes instead of other Scalos classes!
			d1(KPrintF("%s/%s/%ld: inherit from BOOPSI: SuperClassName=<%s>\n", __FILE__, __FUNC__, __LINE__, SuperClassName));
			NewClass = MakeClass(NULL, (STRPTR) SuperClassName, NULL, InstSize, 0);
			}
		else
			{
			// try to find specified superclass (upper/lower case does matter!)
			d1(KPrintF("%s/%s/%ld: inherit from Scalos: SuperClassName=<%s>\n", __FILE__, __FUNC__, __LINE__, SuperClassName));
			SuperClass = FindScalosClass(SuperClassName);

			d1(kprintf("%s/%s/%ld: SuperClass=%08lx\n", __FILE__, __FUNC__, __LINE__, SuperClass));
			if (NULL == SuperClass)
				break;

			NewClass = MakeClass(NULL, NULL, SuperClass->sccl_class, InstSize, 0);
			}
		if (NULL == NewClass)
			break;

		d1(kprintf("%s/%s/%ld: NewClass=%08lx\n", __FILE__, __FUNC__, __LINE__, NewClass));

		NewScalosClass = (struct ScalosClass *) SCA_AllocStdNode((struct ScalosNodeList *)(APTR) &ClassList, NTYP_PluginClass);
		if (NULL == NewScalosClass)
			break;

#if defined(__MORPHOS__)
		NewScalosClass->sccl_EmulEntry.Trap = TRAP_LIB;
		NewScalosClass->sccl_EmulEntry.Func = DispFunc;
		NewClass->cl_Dispatcher.h_Entry = (APTR) &NewScalosClass->sccl_EmulEntry;

#else
		NewClass->cl_Dispatcher.h_Entry = (HOOKFUNC) DispFunc;

#endif /* __MORPHOS__ */

		d1(kprintf("%s/%s/%ld: NewScalosClass=%08lx\n", __FILE__, __FUNC__, __LINE__, NewScalosClass));

		NewClass->cl_UserData = (ULONG) NewScalosClass;
		NewScalosClass->sccl_class = NewClass;
		NewScalosClass->sccl_name = (STRPTR) ClassName;

		ScalosClassList->sccl_Node.mln_Pred = &NewScalosClass->sccl_Node;
		NewScalosClass->sccl_Node.mln_Succ = &ScalosClassList->sccl_Node;
		ScalosClassList = NewScalosClass;

		NewClass = NULL;	// don't free NewClass
		} while (0);

	ScalosReleaseSemaphore(&ClassListSemaphore);

	if (NewClass)
		{
		FreeClass(NewClass);
		}

	return NewScalosClass;
}
LIBFUNC_END


// SCA_FreeScalosClass() implementation
LIBFUNC_P2(BOOL, sca_FreeScalosClass,
	A0, struct ScalosClass *, sccl,
	A6, struct ScalosBase *, ScalosBase)
{
	BOOL Success = FALSE;

	(void)ScalosBase;

	ScalosObtainSemaphore(&ClassListSemaphore);

	if (sccl)
		{
		FreeClass(sccl->sccl_class);
		sccl->sccl_class = NULL;

		SCA_FreeNode((struct ScalosNodeList *)(APTR) &ScalosClassList, &sccl->sccl_Node);

		Success = TRUE;
		}

	ScalosReleaseSemaphore(&ClassListSemaphore);

	return Success;
}
LIBFUNC_END


//SCA_NewScalosObject()
LIBFUNC_P3(Object *, sca_NewScalosObject,
	A0, CONST_STRPTR, ClassName,
	A1, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase)
{
	struct ScalosClass *cl;
	Object *o = NULL;

	(void) ScalosBase;

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));

	ScalosObtainSemaphoreShared(&ClassListSemaphore);

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));

	// try to find specified superclass (upper/lower case does matter!)
	cl = FindScalosClass(ClassName);
	if (cl)
		{
		d1(KPrintF("%s/%s/%ld: cl=%08lx  sccl_class=%08lx  dispatcher=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, cl, cl->sccl_class, cl, &cl->sccl_class->cl_Dispatcher));
		d1(KPrintF("%s/%s/%ld: dispatcher: entry=%08lx  subentry=%08lx  data=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, cl->sccl_class->cl_Dispatcher.h_Entry, \
			cl->sccl_class->cl_Dispatcher.h_SubEntry, \
			cl->sccl_class->cl_Dispatcher.h_Data));

		o = NewObjectA(cl->sccl_class, NULL, TagList);
		}

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));

	ScalosReleaseSemaphore(&ClassListSemaphore);

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));

	return o;
}
LIBFUNC_END


//SCA_DisposeScalosObject()
LIBFUNC_P2(void, sca_DisposeScalosObject,
	A0, Object *, o,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	DisposeObject(o);
}
LIBFUNC_END


static struct ScalosClass *FindScalosClass(CONST_STRPTR ClassName)
{
	struct ScalosClass *cl;

	d1(kprintf("%s/%s/%ld: ClassName=<%s>\n", __FILE__, __FUNC__, __LINE__, ClassName));

	// try to find specified superclass (upper/lower case does matter!)
	for (cl=ScalosClassList; cl; cl = (struct ScalosClass *) cl->sccl_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: cl=%08lx  <%s>  sccl_class=%08lx  dispatcher=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, cl, cl->sccl_name, cl->sccl_class, cl, &cl->sccl_class->cl_Dispatcher));

		if (0 == strcmp(cl->sccl_name, ClassName))
			{
			d1(kprintf("%s/%s/%ld: dispatcher: entry=%08lx  subentry=%08lx  data=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, cl->sccl_class->cl_Dispatcher.h_Entry, \
				cl->sccl_class->cl_Dispatcher.h_SubEntry, \
				cl->sccl_class->cl_Dispatcher.h_Data));

			return cl;
			}
		}

	d1(kprintf("%s/%s/%ld: return NULL\n", __LINE__));

	return NULL;
}


// SCA_RemoveAppObject()
LIBFUNC_P2(BOOL, sca_RemoveAppObject,
	A0, struct AppObject *, appObj,
	A6, struct ScalosBase *, ScalosBase)
{
	BOOL Success = TRUE;
	struct ScaWindowStruct *ws = iInfos.xii_iinfos.ii_AppWindowStruct;
	struct internalScaWindowTask *iwt;

	(void) ScalosBase;

	if (NULL == ws)
		return FALSE;

	iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;

	ScalosObtainSemaphore(iwt->iwt_AppListSemaphore);

	switch (appObj->appo_type)
		{
	case APPTYPE_AppIcon:
		Success = RemoveAppIcon(iwt, appObj);
		break;
	case APPTYPE_AppWindow:
		Success = RemoveAppWindow(iwt, appObj);
		break;
	case APPTYPE_AppMenuItem:
		Success = RemoveAppMenuItem(iwt, appObj);
		break;
		}

	ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);

	return Success;
}
LIBFUNC_END


static BOOL RemoveAppIcon(struct internalScaWindowTask *iwt, struct AppObject *appObj)
{
	struct ScaIconNode *in, *inNext;
	struct ScaIconNode *iconList = NULL;
	struct AppObject *appo;
	BOOL Found = FALSE;

	d1(kprintf("%s/%s/%ld: START iconList=%08lx\n", __FILE__, __FUNC__, __LINE__, iconList));

	ScalosLockIconListExclusive(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; !Found && in; in = inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		if (in->in_Icon == appObj->appo_object.appoo_IconObject)
			{
			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &iconList, in);
			Found = TRUE;
			}
		}
	for (in=iwt->iwt_WindowTask.wt_LateIconList; !Found && in; in = inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		if (in->in_Icon == appObj->appo_object.appoo_IconObject)
			{
			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_LateIconList, &iconList, in);
			Found = TRUE;
			}
		}

	d1(kprintf("%s/%s/%ld: iconList=%08lx\n", __FILE__, __FUNC__, __LINE__, iconList));

	if (iconList)
		{
		d1(kprintf("%s/%s/%ld: iconList=%08lx\n", __FILE__, __FUNC__, __LINE__, iconList));
		RemoveIcons(iwt, &iconList);

		for (in=iconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			// appo_object.appoo_IconObject will be freed later
			d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
			in->in_Icon = NULL;
			}

		FreeIconList(iwt, &iconList);
		}

	ScalosUnLockIconList(iwt);

	// ensure appObj is really member of iwt_AppList
	for (appo = iwt->iwt_AppList; appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
		{
		if (appo == appObj)
			{
			SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_AppList, &appObj->appo_Node);
			break;
			}
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}


static BOOL RemoveAppWindow(struct internalScaWindowTask *iwt, struct AppObject *appObj)
{
	struct AppObject *appo;

	// ensure appObj is really member of iwt_AppList
	for (appo = iwt->iwt_AppList; appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
		{
		if (appo == appObj)
			{
			SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_AppList, &appObj->appo_Node);
			break;
			}
		}

	return TRUE;
}


static BOOL RemoveAppMenuItem(struct internalScaWindowTask *iwt, struct AppObject *appObj)
{
	BOOL Success;
	struct AppObject *appo;

	Success = AppMenu_RemoveItem(appObj->appo_object.appoo_MenuInfo);

	d1(kprintf("%s/%s/%ld: \n", __LINE__));

	if (Success)
		AppMenu_ResetMenu();

	// ensure appObj is really member of iwt_AppList
	for (appo = iwt->iwt_AppList; appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
		{
		if (appo == appObj)
			{
			SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_AppList, &appObj->appo_Node);
			break;
			}
		}

	return Success;
}


//	SCA_ScreenTitleMsg( Format, Args )
//	                      A0     A1
//
//	void SCA_ScreenTitleMsg ( CONST_STRPTR Format, APTR Args);
LIBFUNC_P3(void, sca_ScreenTitleMsg,
	A0, CONST_STRPTR, Format, 
	A1, APTR, Args,
	A6, struct ScalosBase *, ScalosBase)
{
	static BOOL fCustomTitle = FALSE;

	(void) ScalosBase;

	d1(kprintf("%s/%s/%ld: Format=%08lx\n", __FILE__, __FUNC__, __LINE__, Format));

	if (NULL == Format)
		{
		// Restore standard screen title
		if (fCustomTitle)
			{
			fCustomTitle = FALSE;

			ScreenTitleBuffer = (STRPTR) DoMethod(ScreenTitleObject, SCCM_Title_Generate);

			NewScreenTitle(ScreenTitleBuffer);
			}
		}
	else if (((CONST_STRPTR) ~0) == Format)
		{
		// to recover the last non-standard title
		fCustomTitle = TRUE;

		NewScreenTitle(LastScreenTitleBuffer);
		}
	else
		{
		fCustomTitle = TRUE;

		d1(kprintf("%s/%s/%ld: Format=<%s>\n", __FILE__, __FUNC__, __LINE__, Format));

		ScaFormatStringArgs(ScreenTitleBuffer, INT_MAX, Format, Args);
		strcpy(LastScreenTitleBuffer, ScreenTitleBuffer);

		NewScreenTitle(ScreenTitleBuffer);
		}

	d1(kprintf("%s/%s/%ld: ScreenTitleBuffer=<%s>  LastScreenTitleBuffer=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, ScreenTitleBuffer, LastScreenTitleBuffer));
}
LIBFUNC_END


static void NewScreenTitle(CONST_STRPTR NewTitle)
{
	if (SCA_LockWindowList(SCA_LockWindowList_AttemptShared))
		{
		struct ScaWindowStruct *ws;

		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (ws->ws_Window && !(ws->ws_Flags & WSV_FlagF_TaskSleeps))
				{
				LockedSetWindowTitles(ws->ws_Window, (CONST_STRPTR) ~0, NewTitle);
				}
			}

		SCA_UnLockWindowList();
		}
}


//	AppObject = SCA_NewAddAppWindow (ID, UserData, Window, MessagePort, Taglist)
//	   D0                            D0     D1       A0        A1         A2
//
//	struct AppObject *SCA_NewAddAppWindow( ULONG ID, ULONG UserData, 
//		struct Window *, struct MsgPort *, 
//		struct TagItem *Taglist);
LIBFUNC_P6(struct AppObject *, sca_NewAddAppWindow,
	D0, ULONG, ID,
	D1, ULONG, UserData, 
	A0, struct Window *, win,
	A1, struct MsgPort *, msgPort, 
	A2, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
	struct AppObject *appo;

	(void) ScalosBase;

	ScalosObtainSemaphore(iwt->iwt_AppListSemaphore);

	do	{
		appo = (struct AppObject *) SCA_AllocStdNode((struct ScalosNodeList *) &iwt->iwt_AppList, NTYP_AppObject);
		if (NULL == appo)
			break;

		appo->appo_Kennung = ID_SC;
		appo->appo_type = APPTYPE_AppWindow;
		appo->appo_id = ID;
		appo->appo_userdata = UserData;
		appo->appo_msgport = msgPort;
		appo->appo_object.appoo_Window = win;
		appo->appo_TagList = CloneTagItems(TagList);
		} while (0);

	ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);

	d1(KPrintF("%s/%s/%ld: appo=%08lx\n", __FILE__, __FUNC__, __LINE__, appo));

	return appo;
}
LIBFUNC_END


//	Node = SCA_AllocNode (NodeList, Size)
//	 D0                      A0      D0
//
//	struct MinNode *SCA_AllocNode (struct ScalosNodeList *, ULONG Size);
LIBFUNC_P3(struct MinNode *, sca_AllocNode,
	A0, struct ScalosNodeList *, nodeList,
	D0, ULONG, ExtraSize,
	A6, struct ScalosBase *, ScalosBase)
{
	struct MinNode *newNode;

	(void) ScalosBase;

	newNode = ScalosAllocNode(sizeof(struct MinNode) + ExtraSize);

	if (newNode)
		{
		if (nodeList->snl_MinNode)
			{
			// append new node at end of list
			while (nodeList->snl_MinNode->mln_Succ)
				nodeList = (struct ScalosNodeList *) &nodeList->snl_MinNode->mln_Succ;

			newNode->mln_Pred = nodeList->snl_MinNode;
			nodeList->snl_MinNode->mln_Succ = newNode;
			}
		else
			{
			nodeList->snl_MinNode = newNode;
			}
		}

	return newNode;
}
LIBFUNC_END


//	SCA_FreeNode (NodeList, Node)
//	                 A0      A1
//
//	void SCA_FreeNode (struct ScalosNodeList *, struct MinNode *);
LIBFUNC_P3(void, sca_FreeNode,
	A0, struct ScalosNodeList *, nodeList,
	A1, struct MinNode *, oldNode,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	if (oldNode && nodeList->snl_MinNode)
		{
		if (oldNode->mln_Succ)
			oldNode->mln_Succ->mln_Pred = oldNode->mln_Pred;
		if (oldNode->mln_Pred)
			oldNode->mln_Pred->mln_Succ = oldNode->mln_Succ;
		else
			nodeList->snl_MinNode = oldNode->mln_Succ;

		ScalosFreeNode(oldNode);
		}
}
LIBFUNC_END


//	SCA_FreeAllNodes (NodeList)
//	                    A0
//
//	void SCA_FreeAllNodes (struct ScalosNodeList *);
LIBFUNC_P2(void, sca_FreeAllNodes,
	A0, struct ScalosNodeList *, nodeList,
	A6, struct ScalosBase *, ScalosBase)
{
	struct MinNode *oldNode = nodeList->snl_MinNode;

	(void) ScalosBase;

	while (oldNode)
		{
		struct MinNode *nextNode = oldNode->mln_Succ;

		ScalosFreeNode(oldNode);
		oldNode = nextNode;
		}

	nodeList->snl_MinNode = NULL;
}
LIBFUNC_END


//	Node = SCA_AllocStdNode (NodeList, NodeType)
//	 D0                        A0        D0
//
//	struct MinNode *SCA_AllocStdNode (struct ScalosNodeList *, ULONG NodeType);
LIBFUNC_P3(struct MinNode *, sca_AllocStdNode,
	A0, struct ScalosNodeList *, nodeList,
	D0, ULONG, NodeType,
	A6, struct ScalosBase *, ScalosBase)
{
	static const UWORD NodeSizeTable[] =
		{
		sizeof(struct ScaIconNode) - sizeof(struct MinNode),
		sizeof(struct ScaWindowStruct) - sizeof(struct MinNode),
		sizeof(struct DragNode) - sizeof(struct MinNode),
		sizeof(struct PatternNode) - sizeof(struct MinNode),
		sizeof(struct ScalosArg) - sizeof(struct MinNode),
		sizeof(struct ScaDeviceIcon) - sizeof(struct MinNode),
		sizeof(struct ScalosMenuTree) - sizeof(struct MinNode),
		sizeof(struct ScalosMenuTree) - sizeof(struct MinNode),
		sizeof(struct AppObject) - sizeof(struct MinNode),
		sizeof(struct MenuInfo) - sizeof(struct MinNode),
		sizeof(struct ScalosClass) - sizeof(struct MinNode),
		sizeof(struct PluginClass) - sizeof(struct MinNode),
		};

	(void)ScalosBase;

	if (NodeType >= sizeof(NodeSizeTable)/sizeof(NodeSizeTable[0]))
		return NULL;

	return SCA_AllocNode(nodeList, NodeSizeTable[NodeType]);
}
LIBFUNC_END


//	ScalosMessage SCA_AllocMessage( messagetype, additional_size)
//	   D0                               D0            D1
//
//	struct ScalosMessage *SCA_AllocMessage ( ULONG messagetype, UWORD additional_size );
LIBFUNC_P3(struct ScalosMessage *, sca_AllocMessage,
	D0, ULONG, MessageType,
	D1, UWORD, AdditionalSize,
	A6, struct ScalosBase *, ScalosBase)
{
	static const UWORD MsgSizeTable[] =
		{
		0,	  					//0	no message
		sizeof(struct SM_DropProc),			//1	MTYP_DropProc
		sizeof(struct SM_CloseWindow),			//2	MTYP_CloseWindow
		sizeof(struct SM_StartWindow),			//3	MTYP_StartWindow
		sizeof(struct SM_Timer),			//4	MTYP_Timer
		sizeof(struct SM_Requester),			//5	MTYP_Requester
		sizeof(struct SM_NewPattern),			//6	MTYP_NewPattern
		sizeof(struct SM_Sleep),			//7	MTYP_Sleep
		sizeof(struct SM_Wakeup),			//8	MTYP_Wakeup
		sizeof(struct SM_AsyncRoutine),			//9	MTYP_AsyncRoutine
		sizeof(struct SM_RunProcess),			//10	MTYP_RunProcess
		sizeof(struct SM_AppSleep),			//11	MTYP_AppSleep
		sizeof(struct SM_AppWakeup),			//12	MTYP_AppWakeup
		sizeof(struct SM_Redraw),			//13	MTYP_Redraw
		sizeof(struct SM_Update),			//14	MTYP_Update
		sizeof(struct SM_UpdateIcon),			//15	MTYP_UpdateIcon
		sizeof(struct SM_AddIcon),			//16	MTYP_AddIcon
		sizeof(struct SM_RemIcon),			//17	MTYP_RemIcon
		sizeof(struct SM_ARexxRoutine),			//18	MTYP_ARexxRoutine
		sizeof(struct SM_Iconify),			//19	MTYP_Iconify
		sizeof(struct SM_UnIconify),			//20	MTYP_UnIconify
		sizeof(struct SM_AsyncBackFill),		//21	MTYP_AsyncBackFill
		sizeof(struct SM_ShowTitle),			//22	MTYP_ShowTitle
		sizeof(struct SM_RunMenuCmd),			//23	MTYP_RunMenuCmd
		sizeof(struct SM_ShowPopupMenu),		//24	MTYP_ShowPopupMenu
		sizeof(struct SM_ShowStatusBar),		//25	MTYP_ShowStatusBar
		sizeof(struct SM_RedrawIcon),			//26    MTYP_RedrawIcon
		sizeof(struct SM_DoPopupMenu),			//27    MTYP_DoPopupMenu
		sizeof(struct SM_RedrawIconObj),		//28    MTYP_RedrawIconObj
		sizeof(struct SM_NewPreferences),		//29    MTYP_NewPreferences
		sizeof(struct SM_DeltaMove),			//30    MTYP_DeltaMove
		sizeof(struct SM_SetThumbnailImage_Remapped),	//31    MTYP_SetThumbnailImage_Remapped
		sizeof(struct SM_SetThumbnailImage_ARGB),	//32    MTYP_SetThumbnailImage_ARGB
		sizeof(struct SM_NewWindowPath),		//33    MTYP_NewWindowPath
		sizeof(struct SM_PrefsChanged),			//34    MTYP_PrefsChanged
		sizeof(struct SM_StartChildProcess),		//35    MTYP_StartChildProcess
		sizeof(struct SM_RootEvent),			//36    MTYP_RootEvent
		sizeof(struct SM_ShowControlBar),		//37	MTYP_ShowStatusBar
		};
	struct ScalosMessage *newMsg;
	size_t Length;

	(void) ScalosBase;

	if (MessageType < MTYP_DropProc || MessageType >= Sizeof(MsgSizeTable))
		return NULL;

	Length = MsgSizeTable[MessageType] + AdditionalSize;

	newMsg = ScalosAllocMessage(Length);

	if (newMsg)
		{
		memset(newMsg, 0, Length);

		newMsg->sm_Message.mn_Length = Length;
		newMsg->sm_Signature = ID_IMSG;
		newMsg->sm_MessageType = MessageType;
		}

	return newMsg;
}
LIBFUNC_END


//	SCA_FreeMessage( message )
//	                   A1
//
//	void SCA_FreeMessage ( struct ScalosMessage *);
LIBFUNC_P2(void, sca_FreeMessage,
	A1, struct ScalosMessage *, msg,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	if (msg && ID_IMSG == msg->sm_Signature)
		{
		d1(if (MTYP_AsyncBackFill == msg->sm_MessageType) \
			KPrintF("%s/%s/%ld: msg=%08lx  ReplyPort=%08lx  MsgType=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, msg, msg->sm_Message.mn_ReplyPort, msg->sm_MessageType));
		ScalosFreeMessage(msg);
		}
}
LIBFUNC_END


//	SCA_UpdateIcon( WindowType, SCA_UpdateIcon, UpdateIcon_Size )
//	                   D0           A0             D1
//
//	void SCA_UpdateIcon ( UBYTE WindowType, struct ScaUpdateIcon_IW *,
//		 ULONG ui_SIZE );
LIBFUNC_P4(void, sca_UpdateIcon,
	D0, UBYTE, WindowType, 
	A0, struct ScaUpdateIcon_IW *, uiiw,
	D1, ULONG, ui_SIZE,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ui_SIZE;
	(void) ScalosBase;

	d1(KPrintF("%s/%s/%ld: START  ui_iw_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, uiiw->ui_iw_Name));
	debugLock_d1(uiiw->ui_iw_Lock);

	if (SCA_LockWindowList(SCA_LockWindowList_AttemptShared))
		{
		struct ScaWindowStruct *ws;

		d1(KPrintF("%s/%s/%ld: SCA_LockWindowList success!\n", __FILE__, __FUNC__, __LINE__));

		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (((WSV_Type_IconWindow == WindowType && WSV_Type_DeviceWindow == ws->ws_WindowType)
				|| ws->ws_WindowType == WindowType)
				&& ws->ws_Lock
				&& LOCK_SAME == SameLock(ws->ws_Lock, uiiw->ui_iw_Lock))
				{
				struct SM_UpdateIcon *smui;

				do	{
					smui = (struct SM_UpdateIcon *) SCA_AllocMessage(MTYP_UpdateIcon, 0);
					d1(KPrintF("%s/%s/%ld: smui=%08lx\n", __FILE__, __FUNC__, __LINE__, smui));
					if (NULL == smui)
						break;

					smui->smui_DirLock = DupLock(uiiw->ui_iw_Lock);
					if ((BPTR)NULL == smui->smui_DirLock)
						break;

					if (ui_SIZE >= offsetof(struct ScaUpdateIcon_IW, ui_IconType) + sizeof(uiiw->ui_IconType))
						smui->smui_IconType = uiiw->ui_IconType;
					else
						smui->smui_IconType = ICONTYPE_NONE;

					smui->smui_IconName = AllocVec(1 + strlen(uiiw->ui_iw_Name), MEMF_PUBLIC);
					if (NULL == smui->smui_IconName)
						break;

					strcpy((STRPTR) smui->smui_IconName, uiiw->ui_iw_Name);

					debugLock_d1(smui->smui_DirLock);
					d1(KPrintF("%s/%s/%ld: smui_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, smui->smui_IconName));

					PutMsg(ws->ws_MessagePort, &smui->ScalosMessage.sm_Message);
					smui = NULL;
					} while (0);

				if (smui)
					{
					if (smui->smui_IconName)
						FreeCopyString((STRPTR) smui->smui_IconName);
					if (smui->smui_DirLock)
						UnLock(smui->smui_DirLock);
					SCA_FreeMessage(&smui->ScalosMessage);
					}
				}

			}

		SCA_UnLockWindowList();
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}
LIBFUNC_END


//	number = SCA_FreeWBArgs( Buffer, Number, Flags )
//	                           A0      D0     D1
//
//	void SCA_FreeWBArgs ( struct WBArg *Buffer, ULONG Number, 
//		ULONG Flags);
LIBFUNC_P4(void, sca_FreeWBArgs,
	A0, struct WBArg *, wbArg,
	D0, ULONG, NumArgs,
	D1, ULONG, Flags,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	d1(KPrintF("%s/%s/%ld: START NumArgs=%lu  wbArg=%08lx\n", __FILE__, __FUNC__, __LINE__, NumArgs, wbArg));

	while (NumArgs >= 1)
		{
		d1(KPrintF("%s/%s/%ld: wa_Lock=%08lx  wa_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, wbArg->wa_Lock, wbArg->wa_Name));

		if ((Flags & SCAF_FreeLocks) && wbArg->wa_Lock)
			{
			UnLock(wbArg->wa_Lock);
			wbArg->wa_Lock = (BPTR)NULL;
			}
		if ((Flags & SCAF_FreeNames) && wbArg->wa_Name)
			{
			FreeCopyString(wbArg->wa_Name);
			wbArg->wa_Name = NULL;
			}

		NumArgs--;
		wbArg++;
		}
	d1(KPrintF("%s/%s/%ld: END", __LINE__));
}
LIBFUNC_END


//	number = SCA_MakeWBArgs( Buffer, Iconnode, ArgsSize )
//	                           A0       A1        D0
//
//	ULONG SCA_MakeWBArgs ( struct WBArg *Buffer, 
//		struct ScaIconNode *Iconnode, ULONG ArgsSize);
LIBFUNC_P4(ULONG, sca_MakeWBArgs,
	A0, struct WBArg *, wbArg, 
	A1, struct ScaIconNode *, in,
	D0, ULONG, MaxArg,
	A6, struct ScalosBase *, ScalosBase)
{
	struct ScaWindowStruct *ws;
	ULONG ArgCount = 0;
	struct WBArg *pWBArg = wbArg;

	(void) ScalosBase;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
			{
			ArgCount = MakeWindowWBArgs(ws, in, &pWBArg, ArgCount, MaxArg);
			}
		}

	d1(KPrintF("%s/%s/%ld: ArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, ArgCount));

	if (NULL == in && 0 == ArgCount)
		{
		for (ws=winlist.wl_WindowStruct; (ArgCount < MaxArg) && ws;
				ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps) 
				&& ws->ws_Window
				&& ws != iInfos.xii_iinfos.ii_MainWindowStruct
				&& (ws->ws_Window->Flags & WFLG_WINDOWACTIVE))
				{
				ArgCount += DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_MakeWBArg, NULL, wbArg);
				}
			}
		}

	SCA_UnLockWindowList();

	d1(KPrintF("%s/%s/%ld: END ArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, ArgCount));

	return ArgCount;
}
LIBFUNC_END


static ULONG MakeWindowWBArgs(struct ScaWindowStruct *ws, 
	struct ScaIconNode *inExclude,
	struct WBArg **wbArg, ULONG ArgCount, ULONG MaxArg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
	struct ScaIconNode *in;

	ScalosLockIconListShared(iwt);

	for (in = iwt->iwt_WindowTask.wt_IconList; (ArgCount < MaxArg) && in;
			in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		if (in != inExclude && (gg->Flags & GFLG_SELECTED))
			{
			ArgCount += DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg, in, *wbArg);
			(*wbArg)++;
			}
		}

	ScalosUnLockIconList(iwt);

	return ArgCount;
}

//	Windowlist SCA_LockWindowList ( accessmode )
//	 D0                                D0
LIBFUNC_P2(struct ScaWindowList *, sca_LockWindowList,
	D0, LONG, accessmode,
	A6, struct ScalosBase *, ScalosBase)
{
	struct ScaWindowList *Result = &winlist;

	(void) ScalosBase;

	switch (accessmode)
		{
	case SCA_LockWindowList_Shared:
		ScalosObtainSemaphoreShared(&WinListSemaphore);
		break;
	case SCA_LockWindowList_Exclusiv:
		ScalosObtainSemaphore(&WinListSemaphore);
		break;
	case SCA_LockWindowList_AttemptShared:
		if (!ScalosAttemptSemaphoreShared(&WinListSemaphore))
			Result = NULL;
		break;
	case SCA_LockWindowList_AttemptExclusive:
		if (!ScalosAttemptSemaphore(&WinListSemaphore))
			Result = NULL;
		break;
	default:
		break;
		}

	return Result;
}
LIBFUNC_END


//	void SCA_UnLockWindowList ( void );
LIBFUNC_P1(void, sca_UnLockWindowList,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	ScalosReleaseSemaphore(&WinListSemaphore);
}
LIBFUNC_END

//	SCA_MoveNode (SrcNodeList, DestNodeList, Node)
//	                  A0            A1        D0
LIBFUNC_P4(void, sca_MoveNode,
	A0, struct ScalosNodeList *, src,
	A1, struct ScalosNodeList *, dest, 
	D0, struct MinNode *, node,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	d1(kprintf("\n" "%s/%s/%ld: srcList=%08lx  destList=%08lx  node=%08lx  "\
		"ln_Succ=%08lx ln_Pred=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, src, dest, node, node->mln_Succ, node->mln_Pred));

	if (node->mln_Pred)
		src = (struct ScalosNodeList *) node->mln_Pred;

	src->snl_MinNode = node->mln_Succ;

	if (node->mln_Succ)
		node->mln_Succ->mln_Pred = node->mln_Pred;

	node->mln_Pred = NULL;

	if (dest->snl_MinNode)
		{
		while (dest->snl_MinNode)
			dest = (struct ScalosNodeList *) dest->snl_MinNode;

		node->mln_Pred = (struct MinNode *) dest;
		}

	dest->snl_MinNode = node;
	node->mln_Succ = NULL;
}
LIBFUNC_END


//	SCA_SwapNodes ( SrcNode, DestNode, NodeList)
//	                  A0        A1        A2
LIBFUNC_P4(void, sca_SwapNodes,
	A0, struct MinNode *, srcNode,
	A1, struct MinNode *, destNode, 
	A2, struct ScalosNodeList *, nodeList,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	d1(kprintf("\n" "%s/%s/%ld: srcNode=%08lx  ln_Succ=%08lx  ln_Pred=%08lx  "\
		"destNode=%08lx  ln_Succ=%08lx ln_Pred=%08lx  *nodeList=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, srcNode, srcNode->mln_Succ, srcNode->mln_Pred, destNode, \
		destNode->mln_Succ, destNode->mln_Pred, nodeList->snl_MinNode));

	if (srcNode == destNode)
		return;

	if (srcNode->mln_Succ == destNode)
		{
		// .nextnode
		srcNode->mln_Succ = destNode->mln_Succ;
		destNode->mln_Succ = srcNode;
		destNode->mln_Pred = srcNode->mln_Pred;
		srcNode->mln_Pred = destNode;
		}
	else if (srcNode->mln_Pred == destNode)
		{
		// .prevnode
		destNode->mln_Succ = srcNode->mln_Succ;
		srcNode->mln_Succ = destNode;
		srcNode->mln_Pred = destNode->mln_Pred;
		destNode->mln_Pred = srcNode;
		}
	else
		{
		struct MinNode *node;

		node = destNode->mln_Succ;
		destNode->mln_Succ = srcNode->mln_Succ;
		srcNode->mln_Succ = node;
		node = destNode->mln_Pred;
		destNode->mln_Pred = srcNode->mln_Pred;
		srcNode->mln_Pred = node;
		}

	if (srcNode->mln_Succ)
		srcNode->mln_Succ->mln_Pred = srcNode;
	if (srcNode->mln_Pred)
		srcNode->mln_Pred->mln_Succ = srcNode;
	else
		nodeList->snl_MinNode = srcNode;

	if (destNode->mln_Succ)
		destNode->mln_Succ->mln_Pred = destNode;
	if (destNode->mln_Pred)
		destNode->mln_Pred->mln_Succ = destNode;
	else
		nodeList->snl_MinNode = destNode;

	d1(kprintf("%s/%s/%ld: srcNode=%08lx  ln_Succ=%08lx  ln_Pred=%08lx  "\
		"destNode=%08lx  ln_Succ=%08lx ln_Pred=%08lx  *nodeList=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, srcNode, srcNode->mln_Succ, srcNode->mln_Pred, destNode, \
		destNode->mln_Succ, destNode->mln_Pred, nodeList->snl_MinNode));
}
LIBFUNC_END


//	SCA_SortNodes (NodeList, CompareFunc, SortType)
//	                 A0          A1          D0
LIBFUNC_P4(void, sca_SortNodes,
	A0, struct ScalosNodeList *, nodeList,
	A1, struct Hook *, compareHook, 
	D0, ULONG, SortType,
	A6, struct ScalosBase *, ScalosBase)
{
	static const NodeSortFuncType sortingFunctions[] =
		{
		NodeBubbleSort,
		NodeSelectionSort,
		NodeInsertionSort,
		NodeQuickSort
		};
	struct MinNode *lastNode;
	ULONG nodeCount;

	(void) ScalosBase;

	for (nodeCount=0, lastNode=nodeList->snl_MinNode; lastNode && lastNode->mln_Succ; lastNode=lastNode->mln_Succ)
		{
		nodeCount++;
		}

	d1(KPrintF("%s/%s/%ld: nodeCount=%lu\n", __FILE__, __FUNC__, __LINE__, nodeCount));

	if (nodeCount < 1)
		return;

	switch (SortType)
		{
	case SCA_SortType_Bubble:
	case SCA_SortType_Selection:
	case SCA_SortType_Insertion:
	case SCA_SortType_Quick:
		break;

	case SCA_SortType_Best:
		if (nodeCount < 100)
			SortType = SCA_SortType_Selection;
		else
			SortType = SCA_SortType_Quick;
		break;

	default:
		return;		// invalid SortType
		break;
		}

	(sortingFunctions[SortType])(nodeList, lastNode, nodeCount, compareHook);
}
LIBFUNC_END


LIBFUNC_P3(Object *, sca_GetDefIconObject,
	A0, BPTR, dirLock,
	A1, CONST_STRPTR, name,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	return ReturnDefIconObj(dirLock, name, NULL);
}
LIBFUNC_END


LIBFUNC_P4(Object *, sca_GetDefIconObjectA,
	A0, BPTR, dirLock,
	A1, CONST_STRPTR, name,
	A2, struct TagItem *, tagList,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	return ReturnDefIconObj(dirLock, name, tagList);
}
LIBFUNC_END


LIBFUNC_P3(struct ScaWindowStruct *, sca_OpenDrawerByName,
	A0, CONST_STRPTR, path,
	A1, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	return OpenDrawerByName(path, TagList);
}
LIBFUNC_END

static void NodeBubbleSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook)
{
	struct MinNode *node = nodeList->snl_MinNode;

	d1(KPrintF("%s/%s/%ld: nodeCount=%lu\n", __FILE__, __FUNC__, __LINE__, nodeCount));

	while (1)
		{
		BOOL Sorted = TRUE;

		while (node->mln_Succ)
			{
			if ((LONG) CallHookPkt(compareHook, node, node->mln_Succ) > 0)
				{
				d1(kprintf("%s/%s/%ld: SWAP leftNode=%08lx  rightNode=%08lx\n", __FILE__, __FUNC__, __LINE__, node, node->mln_Succ));
				SCA_SwapNodes(node, node->mln_Succ, nodeList);
				Sorted = FALSE;
				}
			else
				node = node->mln_Succ;
			}

		if (Sorted)
			break;

		node = nodeList->snl_MinNode;
		}
	d1(KPrintF("%s/%s/%ld: \n", __LINE__));
}


static void NodeSelectionSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook)
{
	struct MinNode *firstNode = nodeList->snl_MinNode;
	struct MinNode *leftNode = firstNode;

	d1(KPrintF("%s/%s/%ld: nodeCount=%lu\n", __FILE__, __FUNC__, __LINE__, nodeCount));

	while (1)
		{
		struct MinNode *rightNode = leftNode->mln_Succ;

		while (1)
			{
			if ((LONG) CallHookPkt(compareHook, rightNode, leftNode) < 0)
				{
				struct MinNode *tempNode = leftNode;

				d1(kprintf("%s/%s/%ld: SWAP leftNode=%08lx  rightNode=%08lx\n", __FILE__, __FUNC__, __LINE__, leftNode, rightNode));
				SCA_SwapNodes(leftNode, rightNode, nodeList);

				leftNode = rightNode;
				rightNode = tempNode;
				}

			rightNode = rightNode->mln_Succ;
			if (NULL == rightNode)
				break;
			}

		leftNode = leftNode->mln_Succ;
		if (NULL == leftNode->mln_Succ)
			break;
		}

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));
}


static void NodeInsertionSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook)
{
	struct MinNode *firstNode = nodeList->snl_MinNode;
	struct MinNode *rightNode;

	d1(KPrintF("%s/%s/%ld: nodeCount=%lu\n", __FILE__, __FUNC__, __LINE__, nodeCount));

	rightNode = firstNode->mln_Succ;

	while (1)
		{
		struct MinNode *leftNode = rightNode;
		struct MinNode *rightNext = rightNode->mln_Succ;

		while (1)
			{
			d1(kprintf("%s/%s/%ld: leftNode=%08lx  firstNode=%08lx\n", __FILE__, __FUNC__, __LINE__, leftNode, firstNode));

			if (NULL == leftNode->mln_Pred)
				break;

			if ((LONG) CallHookPkt(compareHook, leftNode, leftNode->mln_Pred) < 0)
				{
				d1(kprintf("%s/%s/%ld: SWAP node1=%08lx  node2=%08lx\n", __FILE__, __FUNC__, __LINE__, leftNode->mln_Pred, leftNode));
				SCA_SwapNodes(leftNode->mln_Pred, leftNode, nodeList);
				}
			else
				break;
			}
		rightNode = rightNext;
		if (rightNext == NULL)
			break;
		}
	d1(KPrintF("%s/%s/%ld: \n", __LINE__));
}


static void NodeQuickSort(struct ScalosNodeList *nodeList, 
	struct MinNode *lastNode, ULONG nodeCount, struct Hook *compareHook)
{
	d1(KPrintF("%s/%s/%ld: \n", __LINE__));

	quickSortList(nodeList, compareHook, nodeList->snl_MinNode, lastNode);

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));
}


static void quickSortList(struct ScalosNodeList *nodeList, struct Hook *compareHook,
	struct MinNode *firstNode, struct MinNode *lastNode)
{
	struct MinNode *middleNode, *middleNodeNext;

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));

	if (firstNode == lastNode)
		return;

	middleNode = PartitionNodeList(nodeList, compareHook, &firstNode, &lastNode);

	d1(kprintf("%s/%s/%ld: middleNode=%08lx\n", __FILE__, __FUNC__, __LINE__, middleNode));

	middleNodeNext = middleNode->mln_Succ;

	if (middleNode != firstNode)
		quickSortList(nodeList, compareHook, firstNode, middleNode);
	if (middleNode != lastNode)
		quickSortList(nodeList, compareHook, middleNodeNext, lastNode);

	d1(KPrintF("%s/%s/%ld: \n", __LINE__));
}


static struct MinNode *PartitionNodeList(struct ScalosNodeList *nodeList, struct Hook *compareHook,
	struct MinNode **leftNode, struct MinNode **rightNode)
{
	struct MinNode *pivotNode;
	struct MinNode *iNode, *jNode;
	BOOL Done;
	int nodecount;
	struct MinNode	*tempNode;

	/*********************************************************************
	 * partition list [leftNode..rightNode], and return pointer to pivot *
	 *********************************************************************/

	d1(kprintf("%s/%s/%ld: START *leftNode=%08lx  *rightNode=%08lx\n", __FILE__, __FUNC__, __LINE__, *leftNode, *rightNode));

	/* Find centre node in list */
	for(pivotNode = *leftNode, nodecount = 0, tempNode=*leftNode; 
		tempNode && tempNode != *rightNode; 
		tempNode = tempNode->mln_Succ)
	{
		nodecount ^= 1;
		if(nodecount == 0)
		{
			pivotNode = pivotNode->mln_Succ;
		}
	}

	Done = FALSE;
	iNode = *leftNode;
	jNode = *rightNode;

	/* scan from both ends, swapping when needed */
	/* care must be taken not to address outside [leftNode..rightNode] with pointers */

	while (1)
		{
		struct MinNode *tempNode;

		d1(kprintf("%s/%s/%ld: iNode=%08lx  jNode=%08lx  pivotNode=%08lx\n", __FILE__, __FUNC__, __LINE__, iNode, jNode, pivotNode));
		d1(kprintf("%s/%s/%ld: *leftNode=%08lx  *rightNode=%08lx\n", __FILE__, __FUNC__, __LINE__, *leftNode, *rightNode));

		d1({\
		struct MinNode *node = nodeList->snl_MinNode;\
			while (node)\
				{\
				kprintf("    node=%08lx  prev=%08lx  next=%08lx\n", node, node->mln_Pred, node->mln_Succ);\
				node = node->mln_Succ;\
				}\
		});
		while ((LONG) CallHookPkt(compareHook, jNode, pivotNode) > 0)
			{
			if (iNode == jNode)
				Done = TRUE;
			jNode = jNode->mln_Pred;
			}
		if (Done)
			return jNode;
		while ((LONG) CallHookPkt(compareHook, iNode, pivotNode) < 0)
			{
			if (iNode == jNode)
				Done = TRUE;
			iNode = iNode->mln_Succ;
			}

		if (iNode == jNode)
			return jNode;
		if (Done)
			return jNode;

		d1(kprintf("%s/%s/%ld: iNode=%08lx  jNode=%08lx\n", __FILE__, __FUNC__, __LINE__, iNode, jNode));

		if (iNode->mln_Succ == jNode)
			Done = TRUE;
		if (jNode->mln_Pred == iNode)
			Done = TRUE;

		/* swap iNode, jNode */
		d1(kprintf("%s/%s/%ld: SWAP node1=%08lx  node2=%08lx\n", __FILE__, __FUNC__, __LINE__, iNode, jNode));
		SCA_SwapNodes(iNode, jNode, nodeList);

		// adjust swapped leftNode, rightNode
		if (iNode == *leftNode)
			*leftNode = jNode;
		else if (jNode == *leftNode)
			*leftNode = iNode;

		if (iNode == *rightNode)
			*rightNode = jNode;
		else if (jNode == *rightNode)
			*rightNode = iNode;

		/* examine next element */
		tempNode = iNode;
		if(jNode != *rightNode)
			iNode = jNode->mln_Succ;
		else
			Done = TRUE;

		if(tempNode != *leftNode)
			jNode = tempNode->mln_Pred;
		else
			Done = TRUE;

		if(iNode->mln_Pred == jNode)
			Done = TRUE;
		}
}


//	SCA_RemapBitmap( SrcBitmap, DestBitmap, PenArray )
//	                     A0        A1         A2
LIBFUNC_P4(void, sca_RemapBitmap,
	A0, struct BitMap *, bmSrc, 
	A1, struct BitMap *, bmDest,
	A2, const BYTE *, penArray,
	A6, struct ScalosBase *, ScalosBase)
{
	// no longer supported
	(void) bmSrc;
	(void) bmDest;
	(void) penArray;
	(void) ScalosBase;
}
LIBFUNC_END



LIBFUNC_P1(ULONG, sca_CountWBArgs,
	A0, struct ScaIconNode *, in)
{
	struct ScaWindowStruct *ws;
	ULONG ArgCount = 0;

	d1(KPrintF("%s/%s/%ld: START in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
			{
			ArgCount += CountWindowWBArgs(ws, in);
			}
		}

	if (NULL == in && 0 == ArgCount)
		{
		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps)
				&& ws->ws_Window
				&& ws != iInfos.xii_iinfos.ii_MainWindowStruct
				&& (ws->ws_Window->Flags & WFLG_WINDOWACTIVE))
				{
				ArgCount += DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_CountWBArg, NULL);
				}
			}
		}

	SCA_UnLockWindowList();

	d1(KPrintF("%s/%s/%ld: END ArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, ArgCount));

	return ArgCount;
}
LIBFUNC_END


static STRPTR ChkPrjRequestFile(struct WBArg *arg)
{
	STRPTR Path;
	struct FileRequester *fileReq = NULL;
	BOOL Success = FALSE;

	do	{
		Path = AllocPathBuffer();
		if (NULL == Path)
			break;

		if (!NameFromLock(arg->wa_Lock, Path, Max_PathLen))
			break;

		// AllocAslRequest()
		fileReq = AllocAslRequestTags(ASL_FileRequest,
				ASLFR_PrivateIDCMP, TRUE,
				ASLFR_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				ASLFR_TitleText, (ULONG) GetLocString(MSGID_REQ_SELECTDEFTOOL),
				ASLFR_DoSaveMode, FALSE,
				ASLFR_RejectIcons, TRUE,
				TAG_END);

		if (NULL == fileReq)
			break;

		// AslRequest()
		Success = AslRequestTags(fileReq,
				ASLFR_InitialFile, (ULONG) arg->wa_Name,
				ASLFR_InitialDrawer, (ULONG) Path,
				TAG_END);

		if (Success)
			{
			stccpy(Path, fileReq->fr_Drawer, Max_PathLen);
			AddPart(Path, fileReq->fr_File, Max_PathLen);
			}
		} while (0);

	if (Path)
		{
		if (!Success)
			{
			FreePathBuffer(Path);
			Path = NULL;
			}
		}
	if (fileReq)
		FreeAslRequest(fileReq);

	return Path;
}


static ULONG CountWindowWBArgs(struct ScaWindowStruct *ws,
	struct ScaIconNode *inExclude)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
	struct ScaIconNode *in;
	ULONG ArgCount = 0;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosLockIconListShared(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in;
			in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		if (in != inExclude && (gg->Flags & GFLG_SELECTED))
			{
			ArgCount += DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CountWBArg, in);
			}
		}

	ScalosUnLockIconList(iwt);

	d1(KPrintF("%s/%s/%ld: END ArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, ArgCount));

	return ArgCount;
}


