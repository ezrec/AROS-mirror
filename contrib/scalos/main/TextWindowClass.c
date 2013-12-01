// TextWindowClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <graphics/rpattr.h>
#include <graphics/gfxmacros.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <datatypes/pictureclass.h>
#include <cybergraphx/cybergraphics.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/cybergraphics.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"
#include <ScalosMcpGfx.h>

//----------------------------------------------------------------------------

#define	SORTMARK_WIDTH		7	// Width of built-in image
#define SORTMARK_HEIGHT		6	// Height of built-in image

#define	SORTMARK_MARGIN_X	2
#define SORTMARK_MARGIN_Y	2

// local data definitions

struct TextWindowClassInstance
	{
	ULONG twci_Dummy;
	struct DatatypesImage *twci_SortAscending;
	struct DatatypesImage *twci_SortDescending;
	ULONG twci_SortMarkHeight;
	ULONG twci_SortMarkWidth;
	};

struct TextWindowSortFunc
	{
	HOOKFUNC twsf_Ascending;	// Sort function for ascending sort
	HOOKFUNC twsf_Descending;       // Sort function for descending sort
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) TextWindowClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_ReadIconList(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_ReadIcon(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_RemIcon(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_DeltaMove(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_SetVirtSize(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_Message(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_New(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_Dispose(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_ReAdjust(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_Cleanup(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_BeginUpdate(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_EndUpdate(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_InsertIcon(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_ImmediateCheckUpdate(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_DrawIcon(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_UpdateIcon(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_UpdateIconTags(Class *cl, Object *o, Msg msg);
static ULONG TextWindowClass_DrawColumnHeaders(Class *cl, Object *o, Msg msg);
static void ReplaceTextIcon(struct internalScaWindowTask *iwt, 
	struct ScaIconNode *inOld, struct ScaIconNode *inNew);
static SAVEDS(ULONG) INTERRUPT ColWidthChangeFunc(struct Hook *hook, Object *o, Msg msg);
static void UpdateObjectForIcon(struct internalScaWindowTask *iwt, CONST_STRPTR IconName);
static void ScheduleRealUpdateIcon(struct internalScaWindowTask *iwt, BPTR DirLock, CONST_STRPTR IconName, ULONG IconType);
static void InitTextWindowWidthArray(struct internalScaWindowTask *iwt,
	struct TextWindowClassInstance *inst);
static void TextWindowDrawSortMark(struct internalScaWindowTask *iwt,
	struct TextWindowClassInstance *inst,
	struct RastPort *rp, const struct Rectangle *TextRect);

static LONG TextIconCompareNameAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareNameDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareSizeAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareSizeDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareDateAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareDateDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareTimeAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareTimeDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareCommentAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareCommentDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareProtAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareProtDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareOwnerAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareOwnerDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareGroupAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareGroupDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareFileTypeAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);
static LONG TextIconCompareFileTypeDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1);

static LONG TextIconCompareIsDrawer(const struct ScaIconNode *in2, const struct ScaIconNode *in1);

//----------------------------------------------------------------------------

// public data items :

// +jl+ 20010312 Array of Text IDs
// These are the column names for text windows
static WORD TextColmNames[] =
	{
	MSGID_COLNAMENAME,
	MSGID_COLSIZENAME,
	MSGID_COLACCESSNAME,
	MSGID_COLDATENAME,
	MSGID_COLTIMENAME,
	MSGID_COLCOMNAME,
	MSGID_COLOWNERNAME,
	MSGID_COLGROUPNAME,
	MSGID_COLFILETYPENAME,
	MSGID_COLVERSIONNAME,
	MSGID_COLICONNAME,
	};
static const size_t TextColmNamesSize = Sizeof(TextColmNames);

// make sure we match TextColmNames[] here!
// and don't forget to update TextIconSortFuncTable[] in ScanDir.c
static const BYTE ViewByTab[] =
	{
	IDTV_ViewModes_Name,
	IDTV_ViewModes_Size,
	IDTV_ViewModes_Protection,
	IDTV_ViewModes_Date,
	IDTV_ViewModes_Time,
	IDTV_ViewModes_Comment,
	IDTV_ViewModes_Owner,
	IDTV_ViewModes_Group,
	IDTV_ViewModes_Type,
	IDTV_ViewModes_Version,
	IDTV_ViewModes_MiniIcon,
	};

//----------------------------------------------------------------------------

// index is one of the IDTV_ViewModes_*** values
static const struct TextWindowSortFunc TextIconSortFuncTable[] =
	{
	{ NULL,	NULL },									// IDTV_ViewModes_Default
	{ NULL, NULL },									// IDTV_ViewModes_Icon
	{ (HOOKFUNC) TextIconCompareNameAscFunc,   (HOOKFUNC) TextIconCompareNameDescFunc },   // IDTV_ViewModes_Name
	{ (HOOKFUNC) TextIconCompareSizeAscFunc,   (HOOKFUNC) TextIconCompareSizeDescFunc },   // IDTV_ViewModes_Size
	{ (HOOKFUNC) TextIconCompareDateAscFunc,   (HOOKFUNC) TextIconCompareDateDescFunc },   // IDTV_ViewModes_Date
	{ (HOOKFUNC) TextIconCompareTimeAscFunc,   (HOOKFUNC) TextIconCompareTimeDescFunc },   // IDTV_ViewModes_Time
	{ (HOOKFUNC) TextIconCompareCommentAscFunc, (HOOKFUNC) TextIconCompareCommentDescFunc },  // IDTV_ViewModes_Comment
	{ (HOOKFUNC) TextIconCompareProtAscFunc,   (HOOKFUNC) TextIconCompareProtDescFunc },   // IDTV_ViewModes_Protection
	{ (HOOKFUNC) TextIconCompareOwnerAscFunc,  (HOOKFUNC) TextIconCompareOwnerDescFunc },  // IDTV_ViewModes_Owner
	{ (HOOKFUNC) TextIconCompareGroupAscFunc,  (HOOKFUNC) TextIconCompareGroupDescFunc },  // IDTV_ViewModes_Group
	{ (HOOKFUNC) TextIconCompareFileTypeAscFunc, (HOOKFUNC) TextIconCompareFileTypeDescFunc }, // IDTV_ViewModes_Type
	{ (HOOKFUNC) TextIconCompareNameAscFunc,   (HOOKFUNC) TextIconCompareNameDescFunc },   // IDTV_ViewModes_Version
	{ (HOOKFUNC) TextIconCompareNameAscFunc,   (HOOKFUNC) TextIconCompareNameDescFunc },   // IDTV_ViewModes_MiniIcon
	};

static const enum ScalosSortOrder DefaultSortOrders[] =
	{
	SortOrder_Ascending,			 // IDTV_ViewModes_Default
	SortOrder_Ascending,			 // IDTV_ViewModes_Icon
	SortOrder_Ascending,			 // IDTV_ViewModes_Name
	SortOrder_Ascending,			 // IDTV_ViewModes_Size
	SortOrder_Descending,			 // IDTV_ViewModes_Date
	SortOrder_Descending,			 // IDTV_ViewModes_Time
	SortOrder_Ascending,			 // IDTV_ViewModes_Comment
	SortOrder_Ascending,			 // IDTV_ViewModes_Protection
	SortOrder_Ascending,			 // IDTV_ViewModes_Owner
	SortOrder_Ascending,			 // IDTV_ViewModes_Group
	SortOrder_Ascending,			 // IDTV_ViewModes_Type
	SortOrder_Ascending,			 // IDTV_ViewModes_Version
	SortOrder_Ascending,			 // IDTV_ViewModes_MiniIcon
	};

//----------------------------------------------------------------------------

struct ScalosClass *initTextWindowClass(const struct PluginClass *plug)
{
	struct ScalosClass *TextWindowClass;

	TextWindowClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct TextWindowClassInstance),
			NULL);

	if (TextWindowClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(TextWindowClass->sccl_class->cl_Dispatcher, TextWindowClass_Dispatcher);
		}

	return TextWindowClass;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) TextWindowClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = TextWindowClass_New(cl, o, msg);
		break;

	case OM_DISPOSE:
		Result = TextWindowClass_Dispose(cl, o, msg);
		break;

	case SCCM_IconWin_ReadIconList:
		Result = TextWindowClass_ReadIconList(cl, o, msg);
		break;

	case SCCM_IconWin_ReadIcon:
		Result = TextWindowClass_ReadIcon(cl, o, msg);
		break;

	case SCCM_IconWin_RemIcon:
		Result = TextWindowClass_RemIcon(cl, o, msg);
		break;

	case SCCM_IconWin_DeltaMove:
		Result = TextWindowClass_DeltaMove(cl, o, msg);
		break;

	case SCCM_IconWin_SetVirtSize:
		Result = TextWindowClass_SetVirtSize(cl, o, msg);
		break;

	case SCCM_Message:
		Result = TextWindowClass_Message(cl, o, msg);
		break;

	case SCCM_TextWin_ReAdjust:
		Result = TextWindowClass_ReAdjust(cl, o, msg);
		break;

	case SCCM_IconWin_CleanUp:
		Result = TextWindowClass_Cleanup(cl, o, msg);
		break;

	case SCCM_TextWin_BeginUpdate:
		Result = TextWindowClass_BeginUpdate(cl, o, msg);
		break;

	case SCCM_TextWin_EndUpdate:
		Result = TextWindowClass_EndUpdate(cl, o, msg);
		break;

	case SCCM_TextWin_InsertIcon:
		Result = TextWindowClass_InsertIcon(cl, o, msg);
		break;

	case SCCM_IconWin_ImmediateCheckUpdate:
		Result = TextWindowClass_ImmediateCheckUpdate(cl, o, msg);
		break;

	case SCCM_IconWin_DrawIcon:
		Result = TextWindowClass_DrawIcon(cl, o, msg);
		break;

	case SCCM_IconWin_UpdateIcon:
		Result = TextWindowClass_UpdateIcon(cl, o, msg);
		break;

	case SCCM_IconWin_UpdateIconTags:
		Result = TextWindowClass_UpdateIconTags(cl, o, msg);
		break;

	case SCCM_TextWin_DrawColumnHeaders:
		Result = TextWindowClass_DrawColumnHeaders(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_ReadIconList(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct TextWindowClassInstance *inst = INST_DATA(cl, o);
	ULONG Result;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  START\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	iwt->iwt_OldShowType = iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll;

	InitTextWindowWidthArray(iwt, inst);

	Result = ReadTextWindowIconList(iwt);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
		SETVIRTF_AdjustRightSlider);

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s> END\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return Result;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_ReadIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_ReadIcon *mri = (struct msg_ReadIcon *) msg;
	HOOKFUNC origColWidthChangeFunc = iwt->iwt_ColumnWidthChangeHook.h_SubEntry;
	UBYTE holdUpdate = iwt->iwt_HoldUpdate;
	ULONG Result;

	d1(KPrintF("%s/%s/%ld: wt_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

	iwt->iwt_ColumnWidthChangeHook.h_SubEntry = (HOOKFUNC) ColWidthChangeFunc;

	Result = (ULONG) TextWindowReadIcon(iwt, mri->mri_Name, mri->mri_ria);

	if (Result && iwt->iwt_HoldUpdate && !holdUpdate)
		{
		// Column sizes have changed, make sure everything is updated
		d1(KPrintF("%s/%s/%ld: Column sizes were changed, EndUpdate\n", __FILE__, __FUNC__, __LINE__));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_EndUpdate, TRUE);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
			SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
		}

	iwt->iwt_ColumnWidthChangeHook.h_SubEntry = origColWidthChangeFunc;

	return Result;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_RemIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_RemIcon *mri = (struct msg_RemIcon *) msg;
	struct ScaIconNode *in;
	struct ScaIconNode *inLast;
	struct ScaIconNode *inStart = NULL;
	struct ExtGadget *gg;
	WORD BoundsHeight = 0;
	ULONG pos;
	STRPTR IconName = AllocCopyString(mri->mri_Name);

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: iwt=%08lx  lock=%08lx  IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, mri->mri_Lock, IconName));

	if (NULL == IconName)
		return 1;

	if (IsIwtViewByIcon(iwt))
		return DoSuperMethodA(cl, o, msg);

	ScalosLockIconListExclusive(iwt);

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: in_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, in->in_Name));

		if (in->in_Name && 0 == Stricmp(IconName, in->in_Name))
			{
			d1(KPrintF("%s/%s/%ld: FOUND in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

			gg = (struct ExtGadget *) in->in_Icon;

			BoundsHeight = gg->BoundsHeight;

			debugLock_d1(in->in_Lock);
			if (in->in_Lock)
				RewriteBackdrop(in);

			inStart = (struct ScaIconNode *) in->in_Node.mln_Succ;

			FreeIconNode(iwt, &iwt->iwt_WindowTask.wt_IconList, in);
			break;
			}
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ReposTextIcons2(iwt);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (in = inLast = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		inLast = in;
		}

	if (iwt->iwt_WindowTask.wt_Window)
		{
		LONG IconTop, IconBottom;

		if (inStart)
			{
			gg = (struct ExtGadget *) inStart->in_Icon;

			IconTop = gg->BoundsTopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset;

			d1(KPrintF("%s/%s/%ld: IconTop=%ld  WIndow Top=%ld\n", __FILE__, __FUNC__, __LINE__, IconTop, iwt->iwt_InnerTop + iwt->iwt_InnerHeight));

			// Redraw icon list if the deleted icon was inside visible area
			if (IconTop <= iwt->iwt_InnerTop + iwt->iwt_InnerHeight)
				RefreshIconList(iwt, inStart, NULL);
			}

		if (inLast)
			{
			gg = (struct ExtGadget *) inLast->in_Icon;

			d1(kprintf("%s/%s/%ld: BoundsTop=%ld\n", \
				__FILE__, __FUNC__, __LINE__, gg->BoundsTopEdge));

			IconTop = gg->BoundsTopEdge + gg->BoundsHeight 
				+ iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset;
			IconBottom = IconTop + gg->BoundsHeight;
			}
		else
			{
			// no icons left, erase entire window area
			IconTop = iwt->iwt_InnerTop;
			IconBottom = IconTop + BoundsHeight;
			}

		d1(kprintf("%s/%s/%ld: IconTop=%ld  IconBottom=%ld  YOffset=%ld\n", \
			__FILE__, __FUNC__, __LINE__, IconTop, IconBottom, iwt->iwt_WindowTask.wt_YOffset));

		if (IconTop < iwt->iwt_InnerTop + iwt->iwt_InnerHeight)
			{
			// if we have less icons than would fit into the window,
			// then erase empty area below icon list
			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort, 
				iwt->iwt_InnerLeft, IconTop,
				iwt->iwt_InnerLeft + iwt->iwt_InnerWidth - 1,
				IconBottom - 1);
			}
		}

	ScalosUnLockIconList(iwt);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
		SETVIRTF_AdjustRightSlider);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	pos = IsIconName(IconName);
	if (pos && ~0 != pos)
		{
		// we have just removed an icon
		// We may need to update the associated object
		UpdateObjectForIcon(iwt, IconName);
		}

	FreeCopyString(IconName);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_DeltaMove(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DeltaMove *mdm = (struct msg_DeltaMove *) msg;
	LONG DeltaX, DeltaY;
	LONG XOffset, YOffset;
	LONG VirtWidth, VirtHeight;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (IsIwtViewByIcon(iwt))
		return DoSuperMethodA(cl, o, msg);

	DeltaX = mdm->mdm_DeltaX;
	DeltaY = mdm->mdm_DeltaY;

	d1(kprintf("%s/%s/%ld: DeltaX=%ld  DeltaY=%ld\n", __FILE__, __FUNC__, __LINE__, DeltaX, DeltaY));

	XOffset = iwt->iwt_WindowTask.wt_XOffset + DeltaX;
	if (XOffset < 0)
		DeltaX -= XOffset;

	VirtWidth = iwt->iwt_InvisibleWidth - XOffset;
	if (VirtWidth < 0)
		{
		DeltaX += VirtWidth;
		if (DeltaX < 0)
			DeltaX = 0;
		}

	YOffset = iwt->iwt_WindowTask.wt_YOffset + DeltaY;
	if (YOffset < 0)
		DeltaY -= YOffset;

	VirtHeight = iwt->iwt_InvisibleHeight - YOffset;
	if (VirtHeight < 0)
		{
		DeltaY += VirtHeight;
		if (DeltaY < 0)
			DeltaY = 0;
		}

	d1(kprintf("%s/%s/%ld: DeltaX=%ld  DeltaY=%ld\n", __FILE__, __FUNC__, __LINE__, DeltaX, DeltaY));

	DoSuperMethod(cl, o, SCCM_IconWin_DeltaMove, DeltaX, DeltaY);

	if (DeltaX)
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);

	// the icon under the mouse pointer probably has changed by deltamove
	if (CurrentPrefs.pref_MarkIconUnderMouse)
		{
		d1(KPrintF("%s/%s/%ld: pref_MarkIconUnderMouse - CALL HighlightIconUnderMouse() function!\n", __FILE__, __FUNC__, __LINE__));
		HighlightIconUnderMouse();
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_SetVirtSize(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_SetVirtSize *msv = (struct msg_SetVirtSize *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (msv->msv_Flags & SETVIRTF_AdjustBottomSlider)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);
		SetMenuOnOff(iwt);
		}

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_Message(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_Message *msm = (struct msg_Message *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (IDCMP_MOUSEBUTTONS == msm->msm_iMsg->Class &&
		IECODE_LBUTTON == msm->msm_iMsg->Code)
		{
		WORD MouseX, MouseY;

		MouseX = msm->msm_iMsg->MouseX;
		MouseY = msm->msm_iMsg->MouseY;

		MouseX -= iwt->iwt_InnerLeft;

		if (MouseX >= 0 && MouseX < iwt->iwt_InnerWidth && MouseY < iwt->iwt_InnerTop
			&& MouseY >= (iwt->iwt_WindowTask.wt_Window->BorderTop + iwt->iwt_GadgetBarHeightTop))
			{
			// Mouse click on column header
			short n;
			WORD *pWidth = iwt->iwt_WidthArray;
			ULONG ViewModeIndex = 0;
			ULONG NewViewMode;

			MouseX += iwt->iwt_WindowTask.wt_XOffset;

			for (n=0; CurrentPrefs.pref_ColumnsArray[n] >=0; n++)
				{
				WORD XPos = pWidth[CurrentPrefs.pref_ColumnsArray[n]];

				ViewModeIndex = CurrentPrefs.pref_ColumnsArray[n];

				if (MouseX <= XPos && MouseX >= 0)
					break;

				if (MouseX >= 0)					
					XPos = -XPos;

				MouseX += XPos;
				}

			NewViewMode = ViewByTab[ViewModeIndex];

			if (NewViewMode != TranslateScalosViewMode(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes))
				{
				ControlBarUpdateViewMode(iwt, NewViewMode);

				// set new view (=sort) mode
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes = NewViewMode;

				// set default sort order
				iwt->iwt_WindowTask.mt_WindowStruct->ws_SortOrder = DefaultSortOrders[NewViewMode];
				}
			else
				{
				// toggle ascending/descending sort
				if (SortOrder_Descending == iwt->iwt_WindowTask.mt_WindowStruct->ws_SortOrder)
					iwt->iwt_WindowTask.mt_WindowStruct->ws_SortOrder = SortOrder_Ascending;
				else
					iwt->iwt_WindowTask.mt_WindowStruct->ws_SortOrder = SortOrder_Descending;
				}

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_ReAdjust);
			}
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_New(Class *cl, Object *o, Msg msg)
{
	d1(KPrintF("%s/%s/%ld: cl=%08lx  o=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o));

	o = (Object *) DoSuperMethodA(cl, o, msg);

	if (o)
		{
		struct RastPort rp;
		struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
		struct TextWindowClassInstance *inst = INST_DATA(cl, o);
		struct TextExtent textExtent;
		WORD Height, FontHeight;

		d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

		inst->twci_SortAscending = CreateDatatypesImage("THEME:Window/SortAscending", 0);
		inst->twci_SortDescending = CreateDatatypesImage("THEME:Window/SortDescending", 0);

		inst->twci_SortMarkWidth = inst->twci_SortAscending
			? inst->twci_SortAscending->dti_BitMapHeader->bmh_Width
			: SORTMARK_WIDTH;
		inst->twci_SortMarkHeight = inst->twci_SortAscending
			? inst->twci_SortAscending->dti_BitMapHeader->bmh_Height
			: SORTMARK_HEIGHT;

		Scalos_InitRastPort(&rp);

		Scalos_SetFont(&rp, iwt->iwt_WinScreen->RastPort.Font, &ScreenTTFont);

		iwt->iwt_TextGadFlag = TRUE;

		Height = inst->twci_SortMarkHeight + 2 * SORTMARK_MARGIN_Y;
		FontHeight = Scalos_GetFontHeight(&rp);
		if (Height < FontHeight)
			Height = FontHeight;

		iwt->iwt_TextWindowGadgetHeight = Height + 2;

		Scalos_SetFont(&rp, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);
		Scalos_TextExtent(&rp, "OÖÄgyM", 6, &textExtent);

		iwt->iwt_TextWindowLineHeight = Scalos_GetFontHeight(&rp);
		iwt->iwt_TextWindowLineHeight = 1 + max(textExtent.te_Height, iwt->iwt_TextWindowLineHeight);

		Scalos_DoneRastPort(&rp);

		iwt->iwt_IconFont = TextWindowFont;
		iwt->iwt_IconTTFont = TextWindowTTFont;
		}

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_Dispose(Class *cl, Object *o, Msg msg)
{
	struct TextWindowClassInstance *inst = INST_DATA(cl, o);

	DisposeDatatypesImage(&inst->twci_SortAscending);
	DisposeDatatypesImage(&inst->twci_SortDescending);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_ReAdjust(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct Hook SortHook;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_HoldUpdate)
		return 0;

	SETHOOKFUNC(SortHook, GetTextIconSortFunction(iwt));
	SortHook.h_Data = NULL;

	d1(kprintf("%s/%s/%ld: IconList=%08lx  SortFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_IconList, SortHook.h_Entry));

	if (SortHook.h_Entry)
		SCA_SortNodes((struct ScalosNodeList *) &iwt->iwt_WindowTask.wt_IconList, &SortHook, SCA_SortType_Best);

	ReposTextIcons2(iwt);
	RefreshIcons(iwt, NULL);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_Cleanup(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaIconNode *in, *inNext;
	struct Hook SortHook;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	LockWindow(iwt);

	ScalosLockIconListExclusive(iwt);

	// move all icon nodes from wt_LateIconList to wt_IconList
	for (in=iwt->iwt_WindowTask.wt_LateIconList; in; in=inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_LateIconList, &iwt->iwt_WindowTask.wt_IconList, in);
		}

	// Sort all icons in wt_IconList
	SETHOOKFUNC(SortHook, GetTextIconSortFunction(iwt));
	SortHook.h_Data = NULL;

	d1(kprintf("%s/%s/%ld: IconList=%08lx  SortFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_IconList, SortHook.h_Entry));

	if (SortHook.h_Entry)
		SCA_SortNodes((struct ScalosNodeList *) &iwt->iwt_WindowTask.wt_IconList, &SortHook, SCA_SortType_Best);

	// Recalculate icon y position and ordinal number
	ReposTextIcons2(iwt);

	// Redraw icons
	RefreshIcons(iwt, NULL);

	ScalosUnLockIconList(iwt);

	UnLockWindow(iwt);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_BeginUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	iwt->iwt_HoldUpdate = TRUE;

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_EndUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_EndUpdate *meu = (struct msg_EndUpdate *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	iwt->iwt_HoldUpdate = FALSE;

	if (meu->meu_RefreshFlag)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_ReAdjust);
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_InsertIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_InsertIcon *iic = (struct msg_InsertIcon *) msg;
	struct ScaIconNode *dummyIconList = NULL;
	struct ScaIconNode *in;
	BOOL Replace = FALSE;
	ULONG pos;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: iwt=%08lx  Icon=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, GetIconName(iic->iic_IconNode)));


	ScalosLockIconListExclusive(iwt);

	// Move new icon to dummyIconList
	SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &dummyIconList, iic->iic_IconNode);

	// first, check if an existing icon should be replaced by the new one
	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (0 == Stricmp((STRPTR) GetIconName(in), (STRPTR) GetIconName(iic->iic_IconNode)))
			{
			Replace = TRUE;
			ReplaceTextIcon(iwt, in, iic->iic_IconNode);
			break;
			}
		}

	d1(KPrintF("%s/%s/%ld: Replace=%ld\n", __FILE__, __FUNC__, __LINE__, Replace));
	if (!Replace)
		{
		struct ScaIconNode *InsertAfter;
		struct Hook CompareHook;

		SETHOOKFUNC(CompareHook, GetTextIconSortFunction(iwt));
		CompareHook.h_Data = NULL;

		if (NULL == iwt->iwt_WindowTask.wt_IconList || 
			((LONG) CallHookPkt(&CompareHook, iwt->iwt_WindowTask.wt_IconList, iic->iic_IconNode) > 0))
			{
			// Insert iic->iic_IconNode at beginning of wt_IconList
			d1(KPrintF("%s/%s/%ld: insert at start of wt_IconList\n", __FILE__, __FUNC__, __LINE__));

			iic->iic_IconNode->in_Node.mln_Pred = NULL;
			iic->iic_IconNode->in_Node.mln_Succ = &iwt->iwt_WindowTask.wt_IconList->in_Node;
			if (iwt->iwt_WindowTask.wt_IconList)
				iwt->iwt_WindowTask.wt_IconList->in_Node.mln_Pred = &iic->iic_IconNode->in_Node;
			iwt->iwt_WindowTask.wt_IconList = iic->iic_IconNode;
			}
		else
			{
			// Find insertion point for new icon
			InsertAfter = NULL;
			for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				InsertAfter = in;

				if (NULL == in->in_Node.mln_Succ || 
					(LONG) CallHookPkt(&CompareHook, in->in_Node.mln_Succ, iic->iic_IconNode) > 0)
					{
					break;
					}
				}

			d1(KPrintF("%s/%s/%ld: iwt=%08lx  insert after Icon=<%s>\n", \
				__FILE__, __FUNC__, __LINE__, iwt, GetIconName(InsertAfter)));

			iic->iic_IconNode->in_Node.mln_Succ = InsertAfter->in_Node.mln_Succ;
			InsertAfter->in_Node.mln_Succ = &iic->iic_IconNode->in_Node;

			if (iic->iic_IconNode->in_Node.mln_Succ)
				((struct ScaIconNode *) iic->iic_IconNode->in_Node.mln_Succ)->in_Node.mln_Pred = &iic->iic_IconNode->in_Node;

			iic->iic_IconNode->in_Node.mln_Pred = &InsertAfter->in_Node;
			}

		if (!iwt->iwt_HoldUpdate)
			{
			ReposTextIcons2(iwt);

			RefreshIconList(iwt, iic->iic_IconNode, NULL);
			}

		ScalosUnLockIconList(iwt);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
			SETVIRTF_AdjustRightSlider);
		}
	else
		{
		ScalosUnLockIconList(iwt);
		}

	pos = IsIconName(GetIconName(iic->iic_IconNode));
	d1(KPrintF("%s/%s/%ld: pos=%08lx\n", __FILE__, __FUNC__, __LINE__, pos));
	if (pos && ~0 != pos)
		{
		// we have just removed an icon
		// We may need to update the associated object
		UpdateObjectForIcon(iwt, GetIconName(iic->iic_IconNode));
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_ImmediateCheckUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	ULONG Success;
	UBYTE holdUpdate = iwt->iwt_HoldUpdate;
	HOOKFUNC origColWidthChangeFunc = iwt->iwt_ColumnWidthChangeHook.h_SubEntry;

	iwt->iwt_ColumnWidthChangeHook.h_SubEntry = (HOOKFUNC) ColWidthChangeFunc;

	Success = TextWindowCheckUpdate(iwt);

	if (Success && iwt->iwt_HoldUpdate && !holdUpdate)
		{
		// Column sizes have changed, make sure everything is updated
		d1(kprintf("%s/%s/%ld: Column sizes were changed, EndUpdate\n", __FILE__, __FUNC__, __LINE__));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_EndUpdate, TRUE);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
			SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
		}

	iwt->iwt_ColumnWidthChangeHook.h_SubEntry = origColWidthChangeFunc;

	return Success;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_DrawIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DrawIcon *mdi = (struct msg_DrawIcon *) msg;
	ULONG IconUserFlags = 0L;
	WORD *WidthArray = NULL;


	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 0;

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, mdi->mdi_IconObject));

	// Skip icons which lie completely outside of visible window range
	if (!IsIconObjVisible(iwt, mdi->mdi_IconObject))
		return 0;

	// Check if we really have a text icon here!
	GetAttr(TIDTA_WidthArray, mdi->mdi_IconObject, (APTR) &WidthArray);
	if (NULL == WidthArray)
		return 0;

	GetAttr(IDTA_UserFlags, mdi->mdi_IconObject, &IconUserFlags);
	d1(KPrintF("%s/%s/%ld: IconUserFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, IconUserFlags));

	if (IconUserFlags & ICONOBJ_USERFLAGF_DrawHighlite)
		{
		SetAttrs(mdi->mdi_IconObject,
			IDTA_TextMode, IDTV_TextMode_Outline,
			TAG_END);
		}
	else if (IconUserFlags & ICONOBJ_USERFLAGF_DrawShadowed)
		{
		SetAttrs(mdi->mdi_IconObject,
//			  IDTA_TextMode, IDTV_TextMode_Shadow,
			TIDTA_TextPenFileNormal, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			TIDTA_TextPenDrawerNormal, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			TAG_END);
		}

	DoMethod(mdi->mdi_IconObject,
		IDTM_Draw,
		iwt->iwt_WinScreen,
		iwt->iwt_WindowTask.wt_Window,
		iwt->iwt_WindowTask.wt_Window->RPort,
		iwt->iwt_WinDrawInfo,
		iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
		iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset,
		0L);

	if (IconUserFlags & (ICONOBJ_USERFLAGF_DrawShadowed | ICONOBJ_USERFLAGF_DrawHighlite))
		{
		SetAttrs(mdi->mdi_IconObject,
			IDTA_TextMode, IDTV_TextMode_Normal,
			TIDTA_TextPenFileNormal, PalettePrefs.pal_PensList[PENIDX_FILETEXT],
			TIDTA_TextPenDrawerNormal, PalettePrefs.pal_PensList[PENIDX_DRAWERTEXT],
			TAG_END);
		}
	return 1;
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_UpdateIcon(Class *cl, Object *o, Msg msg)
{
	struct msg_RemIcon *mri = (struct msg_RemIcon *) msg;

	return DoMethod(o, SCCM_IconWin_UpdateIconTags, mri->mri_Lock, mri->mri_Name, TAG_END);
}

//----------------------------------------------------------------------------

static ULONG TextWindowClass_UpdateIconTags(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_UpdateIconTags *muit = (struct msg_UpdateIconTags *) msg;
	struct DateStamp Now;
	BOOL UpdateNow = TRUE;


	d1(KPrintF("%s/%s/%ld: START Name=<%s>\n", __FILE__, __FUNC__, __LINE__, muit->muit_Name));
	debugLock_d1(muit->muit_Lock);

	DateStamp(&Now);
	SubtractDateStamp(&Now, &iwt->iwt_LastIconUpdateTime);

	d1(KPrintF("%s/%s/%ld: Now: Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
		Now.ds_Days, Now.ds_Minute, Now.ds_Tick));

	if (0 == Now.ds_Days && 0 == Now.ds_Minute && Now.ds_Tick < TICKS_PER_SECOND / 2)
		{
		if (++iwt->iwt_IconUpdateCount > MAX_ICON_UPDATE_COUNT)
			{
			d1(KPrintF("%s/%s/%ld: schedule delayed update\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ScheduleUpdate);
			DateStamp(&iwt->iwt_LastIconUpdateTime);

			UpdateNow = FALSE;
			}
		}

	if (UpdateNow)
		{
		if (BNULL == iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock ||
			LOCK_SAME == ScaSameLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, muit->muit_Lock))
			{
			ULONG IconType;

			IconType = GetTagData(UPDATEICON_IconType, ICONTYPE_NONE, (struct TagItem *) &muit->muit_TagList);

			ScheduleRealUpdateIcon(iwt, muit->muit_Lock, muit->muit_Name, IconType);

			if (IsShowAll(iwt->iwt_WindowTask.mt_WindowStruct))
				{
				STRPTR IconName = ScalosAlloc(1 + strlen(muit->muit_Name) + strlen(".info"));

				if (IconName)
					{
					strcpy(IconName, muit->muit_Name);
					strcat(IconName, ".info");

					ScheduleRealUpdateIcon(iwt, muit->muit_Lock, IconName, IconType);
					ScalosFree(IconName);
					}
				}
			}
		}

	return 1;
}

//----------------------------------------------------------------------------

static void ReplaceTextIcon(struct internalScaWindowTask *iwt, 
	struct ScaIconNode *inOld, struct ScaIconNode *inNew)
{
	struct ScaIconNode *dummyIconList = NULL;
	struct ExtGadget *ggNew = (struct ExtGadget *) inNew->in_Icon;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: iwt=%08lx  inNew=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, GetIconName(inNew)));

	inNew->in_Node = inOld->in_Node;

	SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &dummyIconList, inOld);

	if (inNew->in_Node.mln_Succ)
		((struct ScaIconNode *) inNew->in_Node.mln_Succ)->in_Node.mln_Pred = &inNew->in_Node;
	if (inNew->in_Node.mln_Pred)
		((struct ScaIconNode *) inNew->in_Node.mln_Pred)->in_Node.mln_Succ = &inNew->in_Node;
	else
		iwt->iwt_WindowTask.wt_IconList = inNew;

	if (!iwt->iwt_HoldUpdate)
		{
		ReposTextIcons2(iwt);

		if (iwt->iwt_WindowTask.wt_Window)
			{
			LONG IconTop, IconBottom;

			IconTop = ggNew->BoundsTopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset;
			IconBottom = IconTop + ggNew->BoundsHeight;

			if (IconBottom > iwt->iwt_InnerTop && IconTop < iwt->iwt_InnerTop + iwt->iwt_InnerHeight)
				{
				// Erase old icon
				EraseRect(iwt->iwt_WindowTask.wt_Window->RPort, 
					iwt->iwt_InnerLeft, IconTop,
					iwt->iwt_InnerLeft + iwt->iwt_InnerWidth - 1,
					IconBottom - 1);

				// Draw new icon
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DrawIcon, inNew->in_Icon);
				}
			}
		}

	FreeIconList(iwt, &dummyIconList);
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT ColWidthChangeFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) hook->h_Data;

	(void) o;
	(void) msg;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  START\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_BeginUpdate);

	return 0;
}

//----------------------------------------------------------------------------

static void UpdateObjectForIcon(struct internalScaWindowTask *iwt, CONST_STRPTR IconName)
{
	STRPTR ObjectName = AllocCopyString(IconName);
	ULONG pos;

	if (ObjectName)
		{
		pos = IsIconName(ObjectName);

		if (pos && ~0 != pos)
			{
			*((char *) pos) = '\0';

			// Reread object that belonged to the removed icon
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
				SCCM_IconWin_ReadIcon,
				ObjectName, NULL);
			}

		FreeCopyString(ObjectName);
		}
}

//----------------------------------------------------------------------------

static void ScheduleRealUpdateIcon(struct internalScaWindowTask *iwt, BPTR DirLock, CONST_STRPTR IconName, ULONG IconType)
{
	struct SM_RealUpdateIcon *rui = (struct SM_RealUpdateIcon *) 
		SCA_AllocMessage(MTYP_AsyncRoutine, sizeof(struct SM_RealUpdateIcon) - sizeof(struct ScalosMessage));

	if (rui)
		{
		rui->rui_Args.uid_IconType = IconType;
		rui->rui_AsyncRoutine.smar_EntryPoint = (ASYNCROUTINEFUNC) RealUpdateIcon;
		if (DirLock)
			rui->rui_Args.uid_WBArg.wa_Lock = DupLock(DirLock);
		if (IconName)
			rui->rui_Args.uid_WBArg.wa_Name = AllocCopyString(IconName);

		d1(kprintf("%s/%s/%ld: Args=%08lx\n", __FILE__, __FUNC__, __LINE__, &rui->rui_Args));

		d1(kprintf("%s/%s/%ld: PutMsg iwt=%08lx  Msg=%08lx \n", __FILE__, __FUNC__, __LINE__, iwt, &rui->rui_AsyncRoutine.ScalosMessage.sm_Message));

		PutMsg(iwt->iwt_WindowTask.wt_IconPort, &rui->rui_AsyncRoutine.ScalosMessage.sm_Message);
		}
}

//----------------------------------------------------------------------------

// draw TextWindow column headers
static ULONG TextWindowClass_DrawColumnHeaders(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct Region *origClipRegion = NULL;
	struct Region *TextGadgetClipRegion = NULL;

	(void) msg;

	do	{
		struct TextWindowClassInstance *inst = INST_DATA(cl, o);
		struct Rectangle TextGadgetRect;
		LONG x0;
		struct RastPort *rp;
		BYTE *pColumn;
		WORD Height, FontHeight;

		if (!iwt->iwt_TextGadFlag)
			break;

		if (NULL == iwt->iwt_WindowTask.wt_Window)
			break;

		rp = iwt->iwt_WindowTask.wt_Window->RPort;

		Scalos_SetFont(rp, iwt->iwt_WinScreen->RastPort.Font, &ScreenTTFont);

		Height = inst->twci_SortMarkHeight + 2 * SORTMARK_MARGIN_Y;
		FontHeight = Scalos_GetFontHeight(rp);
		if (Height < FontHeight)
			Height = FontHeight;

		TextGadgetRect.MinX = iwt->iwt_WindowTask.wt_Window->BorderLeft;
		TextGadgetRect.MaxX = iwt->iwt_WindowTask.wt_Window->Width - iwt->iwt_WindowTask.wt_Window->BorderRight - 1;
		TextGadgetRect.MinY = TextGadgetRect.MaxY = iwt->iwt_WindowTask.wt_Window->BorderTop + iwt->iwt_GadgetBarHeightTop;
		TextGadgetRect.MaxY += Height + 1;

		TextGadgetClipRegion = NewRegion();
		if (NULL == TextGadgetClipRegion)
			break;

		if (!OrRectRegion(TextGadgetClipRegion, &TextGadgetRect))
			break;

		origClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, TextGadgetClipRegion);

		pColumn = CurrentPrefs.pref_ColumnsArray;
		x0 = - iwt->iwt_WindowTask.wt_XOffset;

		d1(kprintf("%s/%s/%ld: x0=%ld  MinX=%ld  MaxX=%ld\n", __FILE__, __FUNC__, __LINE__, x0, TextGadgetRect.MinX, TextGadgetRect.MaxX));

		d1(kprintf("%s/%s/%ld: wName=%ld  wSize=%ld  wProt=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_WidthArray[WIDTHARRAY_NAME], iwt->iwt_WidthArray[WIDTHARRAY_SIZE],\
			 iwt->iwt_WidthArray[WIDTHARRAY_PROT]));

		while (TextGadgetRect.MaxX > TextGadgetRect.MinX && *pColumn >= 0)
			{
			struct TextExtent tExt;
			LONG x;
			LONG TextX;
			WORD ColIndex = *pColumn++;
			WORD *pNameWidth = iwt->iwt_WidthArray;
			CONST_STRPTR lp;
			size_t Len;
			BOOL IsSortColumn;

			d1(kprintf("%s/%s/%ld: ColIndex=%ld\n", __FILE__, __FUNC__, __LINE__, ColIndex));

			if (ColIndex < 0)
				break;

			d1(kprintf("%s/%s/%ld: wName=%ld  wSize=%ld  wProt=%ld\n", __FILE__, __FUNC__, __LINE__, pNameWidth[0], pNameWidth[1], pNameWidth[2]));

			x = pNameWidth[ColIndex];
			x0 += x;
			if (x0 < 0)
				continue;

			if (x > x0)
				x = x0;

			x += TextGadgetRect.MinX - 1;
			if (x > TextGadgetRect.MaxX)
				x = TextGadgetRect.MaxX;

			if (*pColumn < 0)
				x = TextGadgetRect.MaxX;
			else
				{
				if (0 == pNameWidth[ColIndex])
					continue;
				}
			// .widthok3:

			d1(kprintf("%s/%s/%ld: x=%ld  MinX=%ld  MaxX=%ld\n", __FILE__, __FUNC__, __LINE__, x, MinX, MaxX));
			d1(KPrintF("%s/%s/%ld: *pColumn=%ld\n", __FILE__, __FUNC__, __LINE__, pColumn[-1]));

			SetAPen(rp, iwt->iwt_WinDrawInfo->dri_Pens[BACKGROUNDPEN]);
			RectFill(rp, TextGadgetRect.MinX, TextGadgetRect.MinY, x, TextGadgetRect.MaxY);

			TextX = TextGadgetRect.MinX + 2;

			IsSortColumn = (ViewByTab[pColumn[-1]] == TranslateScalosViewMode(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)) ? 1 : 0;

			if (IsSortColumn)
				{
				TextWindowDrawSortMark(iwt, inst, rp, &TextGadgetRect);
				TextX += (inst->twci_SortMarkWidth + 2 * SORTMARK_MARGIN_X);
				}

			SetAPen(rp, iwt->iwt_WinDrawInfo->dri_Pens[TEXTPEN]);

			lp = GetLocString(TextColmNames[ColIndex]);

			Len = Scalos_TextFit(rp, lp, strlen(lp), &tExt,
				NULL, 1,
				x - TextGadgetRect.MinX - 1,
				100);

			Move(rp, TextX,
				TextGadgetRect.MinY + Scalos_GetFontBaseline(iwt->iwt_WindowTask.wt_Window->RPort) + 1);

			Scalos_Text(rp, lp, Len);

			McpGfxDrawFrame(rp, TextGadgetRect.MinX, TextGadgetRect.MinY, x, TextGadgetRect.MaxY,
				IA_ShadowPen, iwt->iwt_WinDrawInfo->dri_Pens[SHADOWPEN],
				IA_HighlightPen, iwt->iwt_WinDrawInfo->dri_Pens[SHINEPEN],
				IA_FrameType, FRAME_BUTTON,
				IA_Recessed, IsSortColumn,
				TAG_END);

			TextGadgetRect.MinX = 1 + x;
			}

		Scalos_SetFont(rp, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

		} while (0);

	if (origClipRegion)
		InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, origClipRegion);
	d1(KPrintF("%s/%s/%ld: origClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, origClipRegion));
	if (TextGadgetClipRegion)
		DisposeRegion(TextGadgetClipRegion);

	return 0;
}

//----------------------------------------------------------------------------

// adjust text icons to have same width
void ReposTextIcons2(struct internalScaWindowTask *iwt)
{
	WORD *WidthArray = iwt->iwt_WidthArray;
	short n;
	WORD y;
	WORD Width, BoundsWidth;
	WORD Left;
	struct ScaIconNode *in;
	BOOL NameFound = FALSE;

	for (Left = Width = BoundsWidth = 0, n = 0; CurrentPrefs.pref_ColumnsArray[n] >= 0; n++)
		{
		WORD Ndx = CurrentPrefs.pref_ColumnsArray[n];

		BoundsWidth += WidthArray[Ndx];
		if (TXICOL_Name == Ndx)
			{
			Width += WidthArray[Ndx];
			NameFound = TRUE;
			}
		if (!NameFound)
			Left += WidthArray[Ndx];
		}

	for (y=0, in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		gg->TopEdge = gg->BoundsTopEdge = y;

		if (CurrentPrefs.pref_SelectTextIconName)
			{
			gg->LeftEdge = Left;
                        gg->BoundsLeftEdge = 0;
			gg->Width = Width;
			gg->BoundsWidth = BoundsWidth;
			}
		else
			{
			gg->LeftEdge = gg->BoundsLeftEdge = 0;
			gg->Width = gg->BoundsWidth = BoundsWidth;
			}

		if (iwt->iwt_TextWindowLineHeight < gg->Height)
			iwt->iwt_TextWindowLineHeight = gg->Height;

		y += gg->Height;
		}

	d1(KPrintF("%s/%s/%ld: Left=%ld  Width=%ld  BoundsWidth=%ld\n", __FILE__, __FUNC__, __LINE__, Left, Width, BoundsWidth));
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareNameAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	return Stricmp(in2->in_Name, in1->in_Name);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareNameDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	return Stricmp(in1->in_Name, in2->in_Name);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareSizeAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Size1, Size2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Size, in1->in_Icon, &Size1);
	GetAttr(TIDTA_Size, in2->in_Icon, &Size2);

	return ((LONG) Size2 - (LONG) Size1);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareSizeDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Size1, Size2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Size, in1->in_Icon, &Size1);
	GetAttr(TIDTA_Size, in2->in_Icon, &Size2);

	return ((LONG) Size1 - (LONG) Size2);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareDateAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	struct DateStamp ds1, ds2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Days, in1->in_Icon, (ULONG *) &ds1.ds_Days);
	GetAttr(TIDTA_Days, in2->in_Icon, (ULONG *) &ds2.ds_Days);
	GetAttr(TIDTA_Mins, in1->in_Icon, (ULONG *) &ds1.ds_Minute);
	GetAttr(TIDTA_Mins, in2->in_Icon, (ULONG *) &ds2.ds_Minute);
	GetAttr(TIDTA_Ticks, in1->in_Icon, (ULONG *) &ds1.ds_Tick);
	GetAttr(TIDTA_Ticks, in2->in_Icon, (ULONG *) &ds2.ds_Tick);

	return CompareDates(&ds2, &ds1);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareDateDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	struct DateStamp ds1, ds2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Days, in1->in_Icon, (ULONG *) &ds1.ds_Days);
	GetAttr(TIDTA_Days, in2->in_Icon, (ULONG *) &ds2.ds_Days);
	GetAttr(TIDTA_Mins, in1->in_Icon, (ULONG *) &ds1.ds_Minute);
	GetAttr(TIDTA_Mins, in2->in_Icon, (ULONG *) &ds2.ds_Minute);
	GetAttr(TIDTA_Ticks, in1->in_Icon, (ULONG *) &ds1.ds_Tick);
	GetAttr(TIDTA_Ticks, in2->in_Icon, (ULONG *) &ds2.ds_Tick);

	return CompareDates(&ds1, &ds2);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareTimeAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	struct DateStamp ds1, ds2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	ds1.ds_Days = ds2.ds_Days = 0;
	GetAttr(TIDTA_Mins, in1->in_Icon, (ULONG *) &ds1.ds_Minute);
	GetAttr(TIDTA_Mins, in2->in_Icon, (ULONG *) &ds2.ds_Minute);
	GetAttr(TIDTA_Ticks, in1->in_Icon, (ULONG *) &ds1.ds_Tick);
	GetAttr(TIDTA_Ticks, in2->in_Icon, (ULONG *) &ds2.ds_Tick);

	return CompareDates(&ds2, &ds1);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareTimeDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	struct DateStamp ds1, ds2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	ds1.ds_Days = ds2.ds_Days = 0;
	GetAttr(TIDTA_Mins, in1->in_Icon, (ULONG *) &ds1.ds_Minute);
	GetAttr(TIDTA_Mins, in2->in_Icon, (ULONG *) &ds2.ds_Minute);
	GetAttr(TIDTA_Ticks, in1->in_Icon, (ULONG *) &ds1.ds_Tick);
	GetAttr(TIDTA_Ticks, in2->in_Icon, (ULONG *) &ds2.ds_Tick);

	return CompareDates(&ds1, &ds2);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareCommentAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	CONST_STRPTR Comment1, Comment2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Comment, in1->in_Icon, (APTR) &Comment1);
	GetAttr(TIDTA_Comment, in2->in_Icon, (APTR) &Comment2);

	d1(kprintf("%s/%s/%ld: Comment1=<%s>  Comment2=<%s>\n", __FILE__, __FUNC__, __LINE__, Comment1, Comment2));

	return Stricmp(Comment2, Comment1);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareCommentDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	CONST_STRPTR Comment1, Comment2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Comment, in1->in_Icon, (APTR) &Comment1);
	GetAttr(TIDTA_Comment, in2->in_Icon, (APTR) &Comment2);

	d1(kprintf("%s/%s/%ld: Comment1=<%s>  Comment2=<%s>\n", __FILE__, __FUNC__, __LINE__, Comment1, Comment2));

	return Stricmp(Comment1, Comment2);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareProtAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Prot1, Prot2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Protection, in1->in_Icon, &Prot1);
	GetAttr(TIDTA_Protection, in2->in_Icon, &Prot2);

	return ((LONG) Prot2 - (LONG) Prot1);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareProtDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Prot1, Prot2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Protection, in1->in_Icon, &Prot1);
	GetAttr(TIDTA_Protection, in2->in_Icon, &Prot2);

	return ((LONG) Prot1 - (LONG) Prot2);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareOwnerAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Owner1, Owner2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Owner_UID, in1->in_Icon, &Owner1);
	GetAttr(TIDTA_Owner_UID, in2->in_Icon, &Owner2);

	d1(kprintf("%s/%s/%ld: Owner1=%ld  Owner2=%ld\n", __FILE__, __FUNC__, __LINE__, Owner1, Owner2));

	if (Owner1 != Owner2)
		return ((LONG) Owner2 - (LONG) Owner1);

	return Stricmp(in2->in_Name, in1->in_Name);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareOwnerDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Owner1, Owner2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Owner_UID, in1->in_Icon, &Owner1);
	GetAttr(TIDTA_Owner_UID, in2->in_Icon, &Owner2);

	d1(kprintf("%s/%s/%ld: Owner1=%ld  Owner2=%ld\n", __FILE__, __FUNC__, __LINE__, Owner1, Owner2));

	if (Owner1 != Owner2)
		return ((LONG) Owner1 - (LONG) Owner2);

	return Stricmp(in1->in_Name, in2->in_Name);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareGroupAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Group1, Group2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Owner_GID, in1->in_Icon, &Group1);
	GetAttr(TIDTA_Owner_GID, in2->in_Icon, &Group2);

	d1(kprintf("%s/%s/%ld: Group1=%ld  Group2=%ld\n", __FILE__, __FUNC__, __LINE__, Group1, Group2));

	if (Group1 != Group2)
		return ((LONG) Group2 - (LONG) Group1);

	return Stricmp(in2->in_Name, in1->in_Name);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareGroupDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	ULONG Group1, Group2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_Owner_GID, in1->in_Icon, &Group1);
	GetAttr(TIDTA_Owner_GID, in2->in_Icon, &Group2);

	d1(kprintf("%s/%s/%ld: Group1=%ld  Group2=%ld\n", __FILE__, __FUNC__, __LINE__, Group1, Group2));

	if (Group1 != Group2)
		return ((LONG) Group1 - (LONG) Group2);

	return Stricmp(in1->in_Name, in2->in_Name);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareFileTypeAscFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	const struct TypeNode *IconType1, *IconType2;

	d1(KPrintF("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_TypeNode, in1->in_Icon, (APTR) &IconType1);
	GetAttr(TIDTA_TypeNode, in2->in_Icon, (APTR) &IconType2);

	d1(KPrintF("%s/%s/%ld: IconType1=%08lx  IconType2=%08lx\n", __FILE__, __FUNC__, __LINE__, IconType1, IconType2));

	if (IconType1 != IconType2)
		{
		if (IconType1 <= (struct TypeNode *) WBAPPICON && IconType2 <=  (struct TypeNode *) WBAPPICON)
			{
			return IconType2 - IconType1;
			}
		if (IconType1 <= (struct TypeNode *) WBAPPICON)
			return -1;
		if (IconType2 <= (struct TypeNode *) WBAPPICON)
			return 1;
		if (IS_TYPENODE(IconType1) && IS_TYPENODE(IconType2))
			{
			return Stricmp(IconType2->tn_Name, IconType1->tn_Name);
			}

		}

	return Stricmp(in2->in_Name, in1->in_Name);
}

//----------------------------------------------------------------------------

// compare function for text icon list sorting
// drawers are sorted first
static LONG TextIconCompareFileTypeDescFunc(struct Hook *hook,
	const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	LONG Result;
	const struct TypeNode *IconType1, *IconType2;

	d1(KPrintF("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	Result = TextIconCompareIsDrawer(in2, in1);
	if (0 != Result)
		return Result;

	GetAttr(TIDTA_TypeNode, in1->in_Icon, (APTR) &IconType1);
	GetAttr(TIDTA_TypeNode, in2->in_Icon, (APTR) &IconType2);

	d1(KPrintF("%s/%s/%ld: IconType1=%08lx  IconType2=%08lx\n", __FILE__, __FUNC__, __LINE__, IconType1, IconType2));

	if (IconType1 != IconType2)
		{
		if (IconType1 <= (struct TypeNode *) WBAPPICON && IconType2 <=  (struct TypeNode *) WBAPPICON)
			{
			return IconType1 - IconType2;
			}
		if (IconType2 <= (struct TypeNode *) WBAPPICON)
			return -1;
		if (IconType1 <= (struct TypeNode *) WBAPPICON)
			return 1;
		if (IS_TYPENODE(IconType1) && IS_TYPENODE(IconType2))
			{
			return Stricmp(IconType1->tn_Name, IconType2->tn_Name);
			}

		}

	return Stricmp(in1->in_Name, in2->in_Name);
}

//----------------------------------------------------------------------------

static LONG TextIconCompareIsDrawer(const struct ScaIconNode *in2, const struct ScaIconNode *in1)
{
	if ((in2->in_Flags & INF_File) != (in1->in_Flags & INF_File))
		{
		if (DRAWER_SORT_First == CurrentPrefs.pref_DrawerSortMode)
			{
			// Drawers first
			if (in1->in_Flags & INF_File)
				return -1;
			else
				return 1;
			}
		else if (DRAWER_SORT_Last == CurrentPrefs.pref_DrawerSortMode)
			{
			//Drawers last
			if (in1->in_Flags & INF_File)
				return 1;
			else
				return -1;
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

HOOKFUNC GetTextIconSortFunction(struct internalScaWindowTask *iwt)
{
	HOOKFUNC SortFunc = NULL;
	UBYTE ViewMode = iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes;

	if (ViewMode < Sizeof(TextIconSortFuncTable))
		{
		if (SortOrder_Descending == iwt->iwt_WindowTask.mt_WindowStruct->ws_SortOrder)
			SortFunc = TextIconSortFuncTable[ViewMode].twsf_Descending;
		else
			SortFunc = TextIconSortFuncTable[ViewMode].twsf_Ascending;
		}

	if (NULL == SortFunc)
		{
		if (SortOrder_Descending == iwt->iwt_WindowTask.mt_WindowStruct->ws_SortOrder)
			SortFunc = (HOOKFUNC) TextIconCompareNameDescFunc;
		else
			SortFunc = (HOOKFUNC) TextIconCompareNameAscFunc;
		}

	return SortFunc;
}

//----------------------------------------------------------------------------

static void InitTextWindowWidthArray(struct internalScaWindowTask *iwt,
	struct TextWindowClassInstance *inst)
{
	struct RastPort rp;
	BYTE *pColumn;

	if (!iwt->iwt_TextGadFlag)
		return;

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return;

	memset(iwt->iwt_WidthArray, 0, sizeof(iwt->iwt_WidthArray));

	rp = *iwt->iwt_WindowTask.wt_Window->RPort;

	Scalos_SetFont(&rp, iwt->iwt_WinScreen->RastPort.Font, &ScreenTTFont);

	pColumn = CurrentPrefs.pref_ColumnsArray;

	d1(kprintf("%s/%s/%ld: wName=%ld  wSize=%ld  wProt=%ld\n", \
		__FILE__, __FUNC__, __LINE__, iwt->iwt_WidthArray[WIDTHARRAY_NAME], \
		iwt->iwt_WidthArray[WIDTHARRAY_SIZE], iwt->iwt_WidthArray[WIDTHARRAY_PROT]));

	// set min column widths to fit column titles
	while (*pColumn != -1)
		{
		WORD ColIndex = *pColumn++;
		WORD *pNameWidth = iwt->iwt_WidthArray;
		CONST_STRPTR lp;
		size_t Len;

		d1(kprintf("%s/%s/%ld: ColIndex=%ld\n", __FILE__, __FUNC__, __LINE__, ColIndex));

		if (ColIndex < 0)
			break;

		d1(kprintf("%s/%s/%ld: wName=%ld  wSize=%ld  wProt=%ld\n", __FILE__, __FUNC__, __LINE__, pNameWidth[0], pNameWidth[1], pNameWidth[2]));

		if (ColIndex < TextColmNamesSize)
			{
			lp = GetLocString(TextColmNames[ColIndex]);

			Len = 10 + (inst->twci_SortMarkWidth + 2 * SORTMARK_MARGIN_X) + Scalos_TextLength(&rp, lp, strlen(lp));

			if (Len > pNameWidth[ColIndex])
				pNameWidth[ColIndex] = Len;
			}

		}
}

//----------------------------------------------------------------------------

static void TextWindowDrawSortMark(struct internalScaWindowTask *iwt,
	struct TextWindowClassInstance *inst,
	struct RastPort *rp, const struct Rectangle *TextRect)
{
	WORD x = TextRect->MinX + SORTMARK_MARGIN_X;
	WORD y = TextRect->MinY + SORTMARK_MARGIN_Y;

	if (SortOrder_Descending == iwt->iwt_WindowTask.mt_WindowStruct->ws_SortOrder)
		{
		if (inst->twci_SortDescending)
			{
			DtImageDraw(inst->twci_SortDescending,
				rp,
				x, y,
				inst->twci_SortDescending->dti_BitMapHeader->bmh_Width,
				inst->twci_SortDescending->dti_BitMapHeader->bmh_Height
				);
			}
		else
			{
			SetAPen(rp, iwt->iwt_WinDrawInfo->dri_Pens[SHADOWPEN]);
			Move(rp, x, 			y + SORTMARK_HEIGHT - 1);
			Draw(rp, x, 			y + SORTMARK_HEIGHT - 3);
			Draw(rp, x + 3, 		y);
			Draw(rp, x + 3, 		y);

			SetAPen(rp, iwt->iwt_WinDrawInfo->dri_Pens[SHINEPEN]);
			Move(rp, x + 4, 		y + 1);
			Draw(rp, x + SORTMARK_WIDTH - 1,y + SORTMARK_HEIGHT - 3);
			Draw(rp, x + SORTMARK_WIDTH - 1,y + SORTMARK_HEIGHT - 1);
			Draw(rp, x + 1,			y + SORTMARK_HEIGHT - 1);
			}
		}
	else
		{
		if (inst->twci_SortAscending)
			{
			DtImageDraw(inst->twci_SortAscending,
				rp,
				x, y,
				inst->twci_SortAscending->dti_BitMapHeader->bmh_Width,
				inst->twci_SortAscending->dti_BitMapHeader->bmh_Height
				);
			}
		else
			{
			SetAPen(rp, iwt->iwt_WinDrawInfo->dri_Pens[SHADOWPEN]);
			Move(rp, x + SORTMARK_WIDTH - 2,y);
			Draw(rp, x, 			y);
			Draw(rp, x, 			y + SORTMARK_HEIGHT - 4);
			Draw(rp, x + 3, 		y + SORTMARK_HEIGHT - 1);

			SetAPen(rp, iwt->iwt_WinDrawInfo->dri_Pens[SHINEPEN]);
			Move(rp, x + 3, 		y + SORTMARK_HEIGHT - 1);
			Draw(rp, x + SORTMARK_WIDTH - 1,y + SORTMARK_HEIGHT - 4);
			Draw(rp, x + SORTMARK_WIDTH - 1,y);
			}
		}
}

//----------------------------------------------------------------------------


