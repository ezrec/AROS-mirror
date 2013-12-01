// CycleGadgetClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/rpattr.h>
#include <libraries/dos.h>
#include <workbench/startup.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <intuition/screens.h>
#include <intuition/newmouse.h>
#include <datatypes/pictureclass.h>
#include <intuition/cghooks.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>
#include <proto/scalosgfx.h>

#include <clib/alib_protos.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>

#include <defs.h>
#include <scalos/GadgetBar.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"
#include "DtImageClass.h"
#include "CycleGadgetClass.h"
#include "FrameImageClass.h"

//----------------------------------------------------------------------------

#define	POPUPWINDOW_SHADOW_X		8
#define	POPUPWINDOW_SHADOW_Y		8

#define	ACTIVE_NONE			~0

#define BUILTIN_CYCLEIMAGEWIDTH     	19
#define BUILTIN_CYCLEIMAGEHEIGHT	8

//----------------------------------------------------------------------------

struct CyclePopupMessage
	{
	struct GadgetInfo cpm_GInfo;
	struct RastPort cpm_rp;
	ULONG cpm_LineNo;
	Class *cpm_OurClass;
	Object *cpm_OurGadget;
	};

//----------------------------------------------------------------------------

// CycleGadgetClass instance data
struct CycleGadgetInstance
	{
	ULONG cgi_Active;		// number of active label
	CONST_STRPTR *cgi_Labels;
	ULONG cgi_NumberOfLabels;

	Object *cgi_CreatedFrame;	// Frame object created by ourselves
	Object *cgi_ButtonFrame;	// Frame object to draw around gadget (may be identical to cgi_CreatedFrame)
	Object *cgi_WindowFrame;	// Frame object around popup window
	Object *cgi_CycleImage;
	Object *cgi_CheckImage;		// checkmark to indicate active label in popup window

	struct DatatypesImage *cgi_Background;
	struct DatatypesImage *cgi_PopupBgImage;	// optional background image for popup window

	struct Rectangle cgi_LabelRect;	// maximum bounding rectangle of label strings

	LONG cgi_HBorder;
	LONG cgi_VBorder;

	LONG cgi_TextLeftEdge;		// x starting position of label area
	LONG cgi_SeparatorLeftEdge;	// x starting position of separator bar

	ULONG cgi_TextLineHeight;	// Height of a label text line

	LONG cgi_ButtonBorderWidth; 	// additional horizontal space required for button frame
	LONG cgi_ButtonBorderHeight;    // additional vertical space required for button frame

	LONG cgi_WindowBorderWidth; 	// additional horizontal space required for window frame
	LONG cgi_WindowBorderHeight;    // additional vertical space required for window frame

	ULONG cgi_Hidden;		// Flag: this gadget is hidden

	struct Window *cgi_PopupWindow;

	ULONG cgi_PopupActive;		// number of label under mouse pointer in popup window

	LONG cgi_WindowX;		// Offset from Gadget Left to popup window left
	LONG cgi_WindowY;		// Offset from gadget top to popup window top

	struct TextFont *cgi_GadgetFont;	// Font for Gadget rendering

	struct TextFont *cgi_PopupFont;		// font for popup window
	struct TTFontFamily *cgi_PopupTTFont;	// TTengine font for popup window

	struct Hook *cgi_OutsideBackFillHook;  // Backfill hook
	struct Hook cgi_InsideBackFillHook;	// Backfill hook for inside of cycle gadget
	};

//----------------------------------------------------------------------------

// The functions in this module

static SAVEDS(ULONG) INTERRUPT dispatchCycleGadgetClass(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetNew(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetDispose(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetSet(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetGet(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetUpdate(Class *cl, Object *o, Msg msg);
static void CycleGadgetCountLabels(struct CycleGadgetInstance *inst);
static ULONG CycleGadgetLayout(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetRender(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetHitTest(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetHelpTest(Class *cl, Object *o, Msg msg);
static BOOL CycleGadgetPointInGadget(Object *o, WORD x, WORD y);
static ULONG CycleGadgetGoActive(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetHandleInput(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetGoInactive(Class *cl, Object *o, Msg msg);
static ULONG CycleGadgetHandleMouseWheel(Class *cl, Object *o, Msg msg);
static void CycleGadgetAsyncOpenPopupWindow(Class *cl, Object *o, struct GadgetInfo *GInfo);
static void CycleGadgetAsyncDrawPopupWindowLine(Class *cl, Object *o,
	struct GadgetInfo *GInfo, ULONG LineNo);
static void CycleGadgetAsyncClosePopupWindow(Class *cl, Object *o);
static void CycleGadgetOpenPopupWindow(struct ScaWindowTask *wt, APTR arg);
static void CycleGadgetClosePopupWindow(struct ScaWindowTask *wt, APTR arg);
static void CycleGadgetPopupDrawWindowLine(struct ScaWindowTask *wt, APTR arg);
static void CycleGadgetPopupGetLineRect(struct CycleGadgetInstance *inst, ULONG LineNo,
	struct Rectangle *Rect);
static ULONG CycleGadgetPopupGetLineNoFromXY(struct CycleGadgetInstance *inst, WORD x, WORD y);
static void CycleGadgetPopupDrawWindowShadow(struct CycleGadgetInstance *inst,
	struct GadgetInfo *GInfo);
static SAVEDS(ULONG) CycleGadget_BackFillFunc(struct Hook *bfHook,
	struct RastPort *rp, struct BackFillMsg *msg);

//----------------------------------------------------------------------------


/***********************************************************/
/**	Make the class and set up the dispatcher's hook	**/
/***********************************************************/
struct ScalosClass *initCycleGadgetClass(const struct PluginClass *plug)
{
	struct ScalosClass *CycleGadgetClass;

	CycleGadgetClass = SCA_MakeScalosClass(plug->plug_classname,
			plug->plug_superclassname,
			sizeof(struct CycleGadgetInstance),
			NULL);

	if (CycleGadgetClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(CycleGadgetClass->sccl_class->cl_Dispatcher, dispatchCycleGadgetClass);
		}

	return CycleGadgetClass;
}

/**************************************************************************/
/**********	   The CycleGadgetCLASS class dispatcher         *********/
/**************************************************************************/
static SAVEDS(ULONG) INTERRUPT dispatchCycleGadgetClass(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld:  Class=%08lx  SuperClass=%08lx  Method=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, cl->cl_Super, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = CycleGadgetNew(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = CycleGadgetDispose(cl, o, msg);
		break;
	case OM_SET:
		Result = CycleGadgetSet(cl, o, msg);
		break;
	case OM_GET:
		Result = CycleGadgetGet(cl, o, msg);
		break;
	case OM_UPDATE:
		Result = CycleGadgetUpdate(cl, o, msg);
		break;
	case GM_LAYOUT:
		Result = CycleGadgetLayout(cl, o, msg);
		break;
	case GM_RENDER:
		Result = CycleGadgetRender(cl, o, msg);
		break;
	case GM_HITTEST:
		Result = CycleGadgetHitTest(cl, o, msg);
		break;
	case GM_HELPTEST:
		Result = CycleGadgetHelpTest(cl, o, msg);
		break;
	case GM_GOACTIVE:
		Result = CycleGadgetGoActive(cl, o, msg);
		break;
	case GM_HANDLEINPUT:
		Result = CycleGadgetHandleInput(cl, o, msg);
		break;
	case GM_GOINACTIVE:
		Result = CycleGadgetGoInactive(cl, o, msg);
		break;
	case GBCL_HANDLEMOUSEWHEEL:
		Result = CycleGadgetHandleMouseWheel(cl, o, msg);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetNew(Class *cl, Object *o, Msg msg)
{
	BOOL Success = FALSE;

	do	{
		struct opSet *ops = ((struct opSet *) msg);
		struct CycleGadgetInstance *inst;

		o  = (Object *) DoSuperMethodA(cl, o, msg);
		d1(KPrintF("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		if (NULL == o)
			break;

		/* Initial local instance data */
		inst = INST_DATA(cl, o);
		memset(inst, 0, sizeof(struct CycleGadgetInstance));

		inst->cgi_Active = GetTagData(SCACYCLE_Active, 0, ops->ops_AttrList);
		inst->cgi_Labels = (CONST_STRPTR *) GetTagData(SCACYCLE_Labels, (ULONG) NULL, ops->ops_AttrList);
		inst->cgi_Hidden = GetTagData(GBDTA_Hidden, FALSE, ops->ops_AttrList);

		CycleGadgetCountLabels(inst);
		d1(KPrintF("%s/%s/%ld:  cgi_NumberOfLabels=%ld\n", __FILE__, __FUNC__, __LINE__, inst->cgi_NumberOfLabels));
		// we require at least one label
		if (0 == inst->cgi_NumberOfLabels)
			break;

		inst->cgi_WindowFrame = inst->cgi_CreatedFrame = NewObject(NULL, FRAMEICLASS,
			IA_FrameType, FRAME_BUTTON,
			IA_EdgesOnly, TRUE,
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_CreatedFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_CreatedFrame));
		if (NULL == inst->cgi_CreatedFrame)
			break;

		inst->cgi_ButtonFrame = (Object *) GetTagData(SCACYCLE_FrameImage, (ULONG) inst->cgi_CreatedFrame, ops->ops_AttrList);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_ButtonFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_ButtonFrame));

		inst->cgi_CheckImage = NewObject(NULL, SYSICLASS,
			SYSIA_Which, MENUCHECK,
			SYSIA_DrawInfo, iInfos.xii_iinfos.ii_DrawInfo,
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_CheckImage=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_CheckImage));
		if (NULL == inst->cgi_CheckImage)
			break;

		// cgi_CycleImage is optional
		inst->cgi_CycleImage = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) "THEME:Window/ControlBar/CycleNormal",
			DTIMG_SelImageName, (ULONG) "THEME:Window/ControlBar/CycleSelected",
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_CycleImage=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_CycleImage));

		// cgi_Background is optional
		inst->cgi_Background = CreateDatatypesImage("THEME:Window/ControlBar/CycleBackground", 0);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_Background=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_Background));

		// cgi_PopupBgImage is optional
		inst->cgi_PopupBgImage = CreateDatatypesImage("THEME:Window/ControlBar/CyclePopupBackground", 0);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_PopupBgImage=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_PopupBgImage));

		SetAttrs(o,
			GA_Width, GetTagData(GA_Width, 1, ops->ops_AttrList),
			GA_Height, GetTagData(GA_Height, 1, ops->ops_AttrList),
			TAG_END);

		d1(KPrintF("%s/%s/%ld:  gg->Width=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, ((struct Gadget *) o)->Width, ((struct Gadget *) o)->Height));

		SETHOOKFUNC(inst->cgi_InsideBackFillHook, CycleGadget_BackFillFunc);
		inst->cgi_InsideBackFillHook.h_Data = inst;

		inst->cgi_GadgetFont = (struct TextFont *) GetTagData(SCACYCLE_TextFont,
			(ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font, ops->ops_AttrList);
		inst->cgi_PopupFont = (struct TextFont *) GetTagData(SCACYCLE_PopupTextFont,
			(ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font, ops->ops_AttrList);
		inst->cgi_PopupTTFont = (struct TTFontFamily *) GetTagData(SCACYCLE_PopupTTFont,
			CurrentPrefs.pref_UseScreenTTFont ? (ULONG) &ScreenTTFont : 0, ops->ops_AttrList);

		Success = TRUE;
		} while (0);

	if (o && !Success)
		{
		DoMethod(o, OM_DISPOSE);
		o = NULL;
		}

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetDispose(Class *cl, Object *o, Msg msg)
{
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);

	CycleGadgetAsyncClosePopupWindow(cl, o);
	if (inst->cgi_CreatedFrame)
		{
		DisposeObject(inst->cgi_CreatedFrame);
		inst->cgi_CreatedFrame = NULL;
		}
	if (inst->cgi_CycleImage)
		{
		DisposeObject(inst->cgi_CycleImage);
		inst->cgi_CycleImage = NULL;
		}
	if (inst->cgi_CheckImage)
		{
		DisposeObject(inst->cgi_CheckImage);
		inst->cgi_CheckImage = NULL;
		}

	DisposeDatatypesImage(&inst->cgi_PopupBgImage);
	DisposeDatatypesImage(&inst->cgi_Background);

	return 0;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetSet(Class *cl, Object *o, Msg msg)
{
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;
	struct TagItem *TagList = ops->ops_AttrList;
	struct TagItem *tag;
	BOOL NeedRerender = FALSE;
	ULONG Result;

	Result = DoSuperMethodA(cl, o, msg);

	while ((tag = NextTagItem(&TagList)))
		{
		switch (tag->ti_Tag)
			{
		case SCACYCLE_Active:
			if (inst->cgi_Active != tag->ti_Data)
				{
				inst->cgi_Active = tag->ti_Data;
				NeedRerender = TRUE;
				}
			d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_Active=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_Active));
			break;
		case SCACYCLE_Labels:
			inst->cgi_Labels = (CONST_STRPTR *) tag->ti_Data;
			d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_Labels=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_Labels));
			CycleGadgetCountLabels(inst);
			NeedRerender = TRUE;
			break;
		case GBDTA_BackfillHook:
			// Forward backfill hook to frame object
			inst->cgi_OutsideBackFillHook = (struct Hook *) tag->ti_Data;
			d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_OutsideBackFillHook=%08lx  h_Entry=%08lx  h_SubEntry=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, o, inst->cgi_OutsideBackFillHook, \
				inst->cgi_OutsideBackFillHook->h_Entry, inst->cgi_OutsideBackFillHook->h_SubEntry));
			SetAttrs(inst->cgi_ButtonFrame,
				FRAMEIMG_OuterBackfillHook, inst->cgi_OutsideBackFillHook,
				FRAMEIMG_InnerBackfillHook, &inst->cgi_InsideBackFillHook,
				TAG_END);
			d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_OutsideBackFillHook=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_OutsideBackFillHook));
			break;
		case SCACYCLE_TextFont:
			inst->cgi_GadgetFont = (struct TextFont *) tag->ti_Data;
			break;
		case SCACYCLE_PopupTextFont:
			inst->cgi_PopupFont = (struct TextFont *) tag->ti_Data;
			break;
		case SCACYCLE_PopupTTFont:
			inst->cgi_PopupTTFont = (struct TTFontFamily *) tag->ti_Data;
			break;
		case GBDTA_Hidden:
			inst->cgi_Hidden = tag->ti_Data;
			break;
		default:
			break;
			}
		}

	if (NeedRerender && !inst->cgi_Hidden)
		{
		struct RastPort *rp;

		if (inst->cgi_Active >= inst->cgi_NumberOfLabels)
			inst->cgi_Active = inst->cgi_NumberOfLabels - 1;

		d1(KPrintF("%s/%s/%ld:  o=%08lx  cgi_Active=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->cgi_Active));

		rp = ObtainGIRPort(ops->ops_GInfo);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  rp=%08lx\n", __FILE__, __FUNC__, __LINE__, o, rp));
		if (rp)
			{
			DoMethod(o,
				GM_RENDER,
				ops->ops_GInfo,
				rp,
				GREDRAW_UPDATE);
			ReleaseGIRPort(rp);
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetGet(Class *cl, Object *o, Msg msg)
{
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct opGet *opg = (struct opGet *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	ULONG Result = 1;

	if (NULL == opg->opg_Storage)
		return 0;

	switch (opg->opg_AttrID)
		{
	case GA_Left:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->LeftEdge;
		break;
	case GA_Top:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->TopEdge;
		break;
	case GA_Width:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->Width;
		break;
	case GA_Height:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->Height;
		break;
	case SCACYCLE_Active:
		*(opg->opg_Storage) = inst->cgi_Active;
		break;
	case SCACYCLE_Labels:
		*(opg->opg_Storage) = (ULONG) inst->cgi_Labels;
		break;
	case SCACYCLE_TextFont:
		*(opg->opg_Storage) = (ULONG) inst->cgi_GadgetFont;
		break;
	case SCACYCLE_PopupTextFont:
		*(opg->opg_Storage) = (ULONG) inst->cgi_PopupFont;
		break;
	case SCACYCLE_PopupTTFont:
		*(opg->opg_Storage) = (ULONG) inst->cgi_PopupTTFont;
		break;
	case GBDTA_Hidden:
		*(opg->opg_Storage) = (ULONG) inst->cgi_Hidden;
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetUpdate(Class *cl, Object *o, Msg msg)
{
	struct opUpdate *opu = (struct opUpdate *) msg;

	DoMethod(o,
		OM_SET,
		opu->opu_AttrList,
		opu->opu_GInfo);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static void CycleGadgetCountLabels(struct CycleGadgetInstance *inst)
{
	CONST_STRPTR *Labels;

	inst->cgi_NumberOfLabels = 0;
	for (Labels = inst->cgi_Labels; Labels && *Labels; Labels++)
		{
		inst->cgi_NumberOfLabels++;
		}
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetLayout(Class *cl, Object *o, Msg msg)
{
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct gpLayout *gpl = (struct gpLayout *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort rp;
	ULONG cycleWidth = 0, cycleHeight = 0;
	ULONG checkWidth = 0, checkHeight = 0;
	ULONG Width, Height;
	ULONG n;
	struct IBox FrameBox = { 0, 0, 20, 15 };
	struct IBox ContentsBox = { 3, 3, 20 - 2 * 3, 15 - 2 * 3 };

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx gpl_GInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gpl->gpl_GInfo));
	d1(KPrintF("%s/%s/%ld:  gg->Width=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

	if (NULL == gpl->gpl_GInfo)
		return DoSuperMethodA(cl, o, msg);

	// get dimensions of Check image
	GetAttr(IA_Width, inst->cgi_CheckImage, &checkWidth);
	GetAttr(IA_Height, inst->cgi_CheckImage, &checkHeight);

	// get dimensions of Cycle image(s)
	if (inst->cgi_CycleImage)
		{
		ULONG Width = 0, Height = 0;

		GetAttr(IA_Width, inst->cgi_CycleImage, &Width);
		GetAttr(IA_Height, inst->cgi_CycleImage, &Height);

		if (Width > cycleWidth)
			cycleWidth = Width;
		if (Height > cycleHeight)
			cycleHeight = Height;
		}
	else
		{
		// use built-in cycle image
		cycleWidth = BUILTIN_CYCLEIMAGEWIDTH;
		cycleHeight = BUILTIN_CYCLEIMAGEHEIGHT;
		}

	d1(KPrintF("%s/%s/%ld:  o=%08lx  cycleWidth=%ld  cycleHeight=%ld\n", __FILE__, __FUNC__, __LINE__, o, cycleWidth, cycleHeight));

	if (gpl->gpl_GInfo->gi_RastPort)
		rp = *gpl->gpl_GInfo->gi_RastPort;
	else
		InitRastPort(&rp);

	Scalos_SetFont(&rp, inst->cgi_GadgetFont, NULL);

	// find largest dimensions of label string
	inst->cgi_TextLineHeight = Scalos_GetFontHeight(&rp);

	d1(KPrintF("%s/%s/%ld:  cgi_TextLineHeight=%ld\n", __FILE__, __FUNC__, __LINE__, inst->cgi_TextLineHeight));

	inst->cgi_LabelRect.MinX = inst->cgi_LabelRect.MinY = SHRT_MAX;
	inst->cgi_LabelRect.MaxX = inst->cgi_LabelRect.MaxY = SHRT_MIN;

	d1(KPrintF("%s/%s/%ld:  gpl_GInfo=%08lx  gi_RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo, gpl->gpl_GInfo->gi_RastPort));
	d1(KPrintF("%s/%s/%ld:  Font=%08lx\n", __FILE__, __FUNC__, __LINE__, rp.Font));

	for (n = 0; n < inst->cgi_NumberOfLabels; n++)
		{
		struct TextExtent textExt;

		Scalos_TextExtent(&rp,
			inst->cgi_Labels[n],
			strlen(inst->cgi_Labels[n]),
			&textExt);

		d1(KPrintF("%s/%s/%ld:  textExt:  <%s>  MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, \
			inst->cgi_Labels[n], \
			textExt.te_Extent.MinX, textExt.te_Extent.MaxX, \
			textExt.te_Extent.MinY, textExt.te_Extent.MaxY));

		if (textExt.te_Extent.MinX < inst->cgi_LabelRect.MinX)
			inst->cgi_LabelRect.MinX = textExt.te_Extent.MinX;
		if (textExt.te_Extent.MinY < inst->cgi_LabelRect.MinY)
			inst->cgi_LabelRect.MinY = textExt.te_Extent.MinY;
		if (textExt.te_Extent.MaxX > inst->cgi_LabelRect.MaxX)
			inst->cgi_LabelRect.MaxX = textExt.te_Extent.MaxX;
		if (textExt.te_Extent.MaxY > inst->cgi_LabelRect.MaxY)
			inst->cgi_LabelRect.MaxY = textExt.te_Extent.MaxY;
		}

	d1(KPrintF("%s/%s/%ld:  cgi_LabelRect:  MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                inst->cgi_LabelRect.MinX, inst->cgi_LabelRect.MaxX, \
		inst->cgi_LabelRect.MinY, inst->cgi_LabelRect.MaxY));

	// Reserve space for checkbox
	inst->cgi_LabelRect.MinX -= checkWidth + 2;
	if ((1 + inst->cgi_LabelRect.MaxY - inst->cgi_LabelRect.MinY) < checkHeight)
		{
		// make sure height is sufficient for checkbox
		LONG YDiff = checkHeight - (1 + inst->cgi_LabelRect.MaxY - inst->cgi_LabelRect.MinY);

		if (YDiff & 0x01)
			YDiff++;	// round up to even number

		inst->cgi_LabelRect.MinY -= YDiff / 2;
		inst->cgi_LabelRect.MaxY += YDiff / 2;
		}

	d1(KPrintF("%s/%s/%ld:  gpl_GInfo=%08lx  inst->cgi_ButtonFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo, inst->cgi_ButtonFrame));
	d1(KPrintF("%s/%s/%ld:  cgi_LabelRect:  MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                inst->cgi_LabelRect.MinX, inst->cgi_LabelRect.MaxX, \
		inst->cgi_LabelRect.MinY, inst->cgi_LabelRect.MaxY));

	// calculate offsets for outer frame
	DoMethod(inst->cgi_ButtonFrame,
		IM_FRAMEBOX,
		&ContentsBox,
		&FrameBox,
		gpl->gpl_GInfo->gi_DrInfo,
		0);

	d1(KPrintF("%s/%s/%ld: FrameBox: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                FrameBox.Left, FrameBox.Top, FrameBox.Width, FrameBox.Height));
	d1(KPrintF("%s/%s/%ld: ContentsBox: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                ContentsBox.Left, ContentsBox.Top, ContentsBox.Width, ContentsBox.Height));

	inst->cgi_ButtonBorderWidth = (FrameBox.Width - ContentsBox.Width) / 2;
	inst->cgi_ButtonBorderHeight = (FrameBox.Height - ContentsBox.Height) / 2;

	inst->cgi_HBorder = 1 + inst->cgi_ButtonBorderWidth;
	inst->cgi_VBorder = inst->cgi_ButtonBorderHeight;

	d1(KPrintF("%s/%s/%ld:  cgi_HBorder=%ld  cgi_VBorder=%ld\n", __FILE__, __FUNC__, __LINE__, inst->cgi_HBorder, inst->cgi_VBorder));

	inst->cgi_TextLeftEdge = (inst->cgi_HBorder + 1) + cycleWidth + 2 * 1 + 2 + 1;

	// calculate minimum required width and height
	Width = 2 * (inst->cgi_HBorder + 1) + cycleWidth + 2 * 1
		+ 2 + 1		// room for separator bar
		+ (inst->cgi_LabelRect.MaxX - inst->cgi_LabelRect.MinX);

	Height = max((inst->cgi_LabelRect.MaxY - inst->cgi_LabelRect.MinY), cycleHeight)
		+ 2 + (inst->cgi_VBorder + 1);

	d1(KPrintF("%s/%s/%ld:  cgi_LabelRect:  MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                inst->cgi_LabelRect.MinX, inst->cgi_LabelRect.MaxX, \
		inst->cgi_LabelRect.MinY, inst->cgi_LabelRect.MaxY));
	d1(KPrintF("%s/%s/%ld:  CycleHeight=%ld  Labelrect Height=%ld\n", __FILE__, __FUNC__, __LINE__, cycleHeight, inst->cgi_LabelRect.MaxY - inst->cgi_LabelRect.MinY));
	d1(KPrintF("%s/%s/%ld:  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height));

	if (gg->Width < Width)
		SetAttrs(o, GA_Width, Width, TAG_END);
	if (gg->Height < Height)
		SetAttrs(o, GA_Height, Height, TAG_END);

	d1(KPrintF("%s/%s/%ld:  gg->Width=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetRender(Class *cl, Object *o, Msg msg)
{
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct gpRender *gpr = (struct gpRender *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort rp;
	struct TextExtent textExt;
	size_t LabelLength;
	ULONG CycleImageHeight;
	ULONG SeparatorYOffset;
	WORD x, y, y2;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));

	if (gpr->gpr_RPort)
		rp = *gpr->gpr_RPort;
	else
		InitRastPort(&rp);

	Scalos_SetFont(&rp, inst->cgi_GadgetFont, NULL);

	if (inst->cgi_Background)
		{
		d1(KPrintF("%s/%s/%ld:  cgi_Background=%08lx  dti_BitMap=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, inst->cgi_Background, inst->cgi_Background->dti_BitMap));
		FillBackground(&rp, inst->cgi_Background,
			gg->LeftEdge + inst->cgi_HBorder / 2,
			gg->TopEdge + inst->cgi_VBorder / 2,
			gg->LeftEdge + gg->Width - inst->cgi_HBorder / 2 - 1,
			gg->TopEdge + gg->Height - inst->cgi_VBorder / 2 - 1,
			0, 0);
		}
	else
		{
		EraseRect(&rp,
			gg->LeftEdge + inst->cgi_HBorder / 2,
			gg->TopEdge + inst->cgi_VBorder / 2,
			gg->LeftEdge + gg->Width - inst->cgi_HBorder / 2 - 1,
			gg->TopEdge + gg->Height - inst->cgi_VBorder / 2 - 1);
		}

	// Draw outer frame
	SetAttrs(inst->cgi_ButtonFrame,
		IA_Width, gg->Width,
		IA_Height, gg->Height,
		IA_Recessed, (gg->Flags & GFLG_SELECTED),
		TAG_END);
	DrawImageState(&rp,
		(struct Image *) inst->cgi_ButtonFrame,
		gg->LeftEdge, gg->TopEdge,
		IDS_NORMAL,
		gpr->gpr_GInfo->gi_DrInfo);

	// Draw cycle image
	if (inst->cgi_CycleImage)
		{
		GetAttr(IA_Height, inst->cgi_CycleImage, &CycleImageHeight);

		d1(KPrintF("%s/%s/%ld:  cgi_HBorder=%ld  cgi_VBorder=%ld\n", __FILE__, __FUNC__, __LINE__, inst->cgi_HBorder, inst->cgi_VBorder));

		// Draw cycle image vertically centered
		DrawImageState(&rp,
			(struct Image *) inst->cgi_CycleImage,
			gg->LeftEdge + inst->cgi_HBorder + 1,
			gg->TopEdge + (gg->Height - CycleImageHeight) / 2,
			(gg->Flags & GFLG_SELECTED) ? IDS_SELECTED : IDS_NORMAL,
			gpr->gpr_GInfo->gi_DrInfo);
		}
	else
		{
		WORD h;

		// Draw built-in cycle image
		h = gg->Height / 2;
		x = gg->LeftEdge + inst->cgi_HBorder + 1 + 6;
		CycleImageHeight = gg->Height - 2;

		SetAPen(&rp, gpr->gpr_GInfo->gi_DrInfo->dri_Pens[TEXTPEN]);
		for (y = 0; y < 4; y++)
			{
			RectFill(&rp,
				x + y,
				gg->TopEdge + gg->Height - 1 - h - y - 1,
				x + 6 - y,
				gg->TopEdge + gg->Height - 1 - h - y - 1);

			RectFill(&rp,
				x + y,
				gg->TopEdge + h + y + 1,
				x + 6 - y,
				gg->TopEdge + h + y + 1);
			}
		}

	// Draw separator bar
	inst->cgi_SeparatorLeftEdge = inst->cgi_TextLeftEdge - 1 - 2;
	SeparatorYOffset = (gg->Height - CycleImageHeight) / 2;
	x = gg->LeftEdge + inst->cgi_SeparatorLeftEdge;
	y = y2 = gg->TopEdge + SeparatorYOffset;
	y2 += CycleImageHeight;

	SetAPen(&rp, gpr->gpr_GInfo->gi_DrInfo->dri_Pens[SHINEPEN]);
	RectFill(&rp, x + 1, y, x + 1, y2 - 1);
	SetAPen(&rp, gpr->gpr_GInfo->gi_DrInfo->dri_Pens[SHADOWPEN]);
	RectFill(&rp, x, y, x, y2 - 1);

	LabelLength = strlen(inst->cgi_Labels[inst->cgi_Active]);

	Scalos_TextExtent(&rp,
		inst->cgi_Labels[inst->cgi_Active],
		LabelLength,
		&textExt);

	x = gg->LeftEdge + inst->cgi_TextLeftEdge;
	y = gg->TopEdge;

	// Center label inside label area
	x += ((inst->cgi_LabelRect.MaxX - inst->cgi_LabelRect.MinX) - textExt.te_Width) / 2;
	y += (gg->Height - textExt.te_Height) / 2 + Scalos_GetFontBaseline(&rp);

	// Draw Label
	SetAPen(&rp, gpr->gpr_GInfo->gi_DrInfo->dri_Pens[TEXTPEN]);
	SetDrMd(&rp, JAM1);
	Move(&rp, x, y);
	Scalos_Text(&rp, inst->cgi_Labels[inst->cgi_Active], LabelLength);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetHitTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	return (ULONG) (CycleGadgetPointInGadget(o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y) ? GMR_GADGETHIT : 0);
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetHelpTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	if (CycleGadgetPointInGadget(o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y))
		{
		d1(KPrintF("%s/%s/%ld: Result=GMR_HELPHIT\n", __FILE__, __FUNC__, __LINE__));
		return GMR_HELPHIT;
		}

	d1(KPrintF("%s/%s/%ld: Result=GMR_NOHELPHIT\n", __FILE__, __FUNC__, __LINE__));

	return GMR_NOHELPHIT;
}

//----------------------------------------------------------------------------

// x, y are already relative to the Gadget position (0,0) is top left corner!
static BOOL CycleGadgetPointInGadget(Object *o, WORD x, WORD y)
{
	ULONG Width, Height;

	GetAttr(GA_Width, o, &Width);
	GetAttr(GA_Height, o, &Height);

	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, Width, Height));

	if (x < 0 || x >= Width)
		return FALSE;
	if (y < 0 || y >= Height)
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetGoActive(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct RastPort *rp;
	ULONG Result;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));

	gg->Flags |= GFLG_SELECTED;

	if (gpi->gpi_Mouse.X > inst->cgi_TextLeftEdge)
		{
		CycleGadgetAsyncOpenPopupWindow(cl, o, gpi->gpi_GInfo);
		}
	rp = ObtainGIRPort(gpi->gpi_GInfo);
	if (rp)
		{
		DoMethod(o,
			GM_RENDER,
			gpi->gpi_GInfo,
			rp,
			GREDRAW_UPDATE);
		ReleaseGIRPort(rp);
		Result = GMR_MEACTIVE;
		}
	else
		{
		Result = GMR_NOREUSE;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetHandleInput(Class *cl, Object *o, Msg msg)
{
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct gpInput *gpi = (struct gpInput *) msg;
	ULONG Result = GMR_MEACTIVE;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  ie_Class=%08lx\n", \
		 __FILE__, __FUNC__, __LINE__, o, gpi->gpi_IEvent->ie_Class));

	if (IECLASS_RAWMOUSE == gpi->gpi_IEvent->ie_Class)
		{
		struct ExtGadget *gg = (struct ExtGadget *) o;
		struct RastPort *rp;
		struct gpHitTest gpht;

		d1(KPrintF("%s/%s/%ld:  Code=%08lx\n", __FILE__, __FUNC__, __LINE__, gpi->gpi_IEvent->ie_Code));

		switch (gpi->gpi_IEvent->ie_Code)
			{
		case SELECTUP:
			d1(KPrintF("%s/%s/%ld:  SELECTUP  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->Flags));
			if (gg->Flags & GFLG_SELECTED)
				{
				if (inst->cgi_PopupWindow)
					{
					ULONG NewActive;

					NewActive = CycleGadgetPopupGetLineNoFromXY(inst, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y);
					if (ACTIVE_NONE != NewActive)
						{
						inst->cgi_Active = NewActive;
						}
					}
				else
					{
					if (gpi->gpi_IEvent->ie_Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
						{
						// activate previous label
						if (inst->cgi_Active > 0)
							inst->cgi_Active--;
						else
							inst->cgi_Active = inst->cgi_NumberOfLabels - 1;
						}
					else
						{
						// activate next label
						inst->cgi_Active++;
						if (inst->cgi_Active >= inst->cgi_NumberOfLabels)
							inst->cgi_Active = 0;
						}
					}

				d1(KPrintF("%s/%s/%ld:  cgi_Active=%ld\n", __FILE__, __FUNC__, __LINE__, inst->cgi_Active));

				*gpi->gpi_Termination = inst->cgi_Active;
				Result = GMR_NOREUSE | GMR_VERIFY;
				}
		        else
				{
				Result = GMR_NOREUSE;
				}
			break;

		case IECODE_NOBUTTON:
			d1(KPrintF("%s/%s/%ld:  IECODE_NOBUTTON\n", __FILE__, __FUNC__, __LINE__));
			d1(KPrintF("%s/%s/%ld:  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y));

			if (inst->cgi_PopupWindow)
				{
				ULONG NewActive;
				
				NewActive = CycleGadgetPopupGetLineNoFromXY(inst, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y);
				if (NewActive != inst->cgi_PopupActive)
					{
					ULONG OldActive = inst->cgi_PopupActive;

					inst->cgi_PopupActive = NewActive;

					if (ACTIVE_NONE != OldActive)
						{
						CycleGadgetAsyncDrawPopupWindowLine(cl, o,
							gpi->gpi_GInfo, OldActive);
						}

					if (ACTIVE_NONE != NewActive)
						{
						CycleGadgetAsyncDrawPopupWindowLine(cl, o,
							gpi->gpi_GInfo, NewActive);
						}

					if (ACTIVE_NONE != NewActive)
						{
						d1(KPrintF("%s/%s/%ld:  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->Flags));
						if (!(gg->Flags & GFLG_SELECTED))
							{
							gg->Flags |= GFLG_SELECTED;

							rp = ObtainGIRPort(gpi->gpi_GInfo);
							if (rp)
								{
								DoMethod(o,
									GM_RENDER,
									gpi->gpi_GInfo,
									rp,
									GREDRAW_UPDATE);
								ReleaseGIRPort(rp);
								}
							}
						}
					else
						{
						d1(KPrintF("%s/%s/%ld:  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->Flags));
						if (gg->Flags & GFLG_SELECTED)
							{
							gg->Flags &= ~GFLG_SELECTED;

							rp = ObtainGIRPort(gpi->gpi_GInfo);
							if (rp)
								{
								DoMethod(o,
									GM_RENDER,
									gpi->gpi_GInfo,
									rp,
									GREDRAW_UPDATE);
								ReleaseGIRPort(rp);
								}
							}
						}
					}
				}
			else
				{
				// we cannot use inline parameters here since gpht_Mouse.X and gpht_Mouse.Y are WORD
				gpht.MethodID = GM_HITTEST;
				gpht.gpht_GInfo = gpi->gpi_GInfo;
				gpht.gpht_Mouse.X = gpi->gpi_Mouse.X;
				gpht.gpht_Mouse.Y = gpi->gpi_Mouse.Y;

				if (GMR_GADGETHIT == DoMethodA(o, (Msg)(APTR) &gpht))
					{
					d1(KPrintF("%s/%s/%ld:  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->Flags));
					if (!(gg->Flags & GFLG_SELECTED))
						{
						gg->Flags |= GFLG_SELECTED;

						rp = ObtainGIRPort(gpi->gpi_GInfo);
						if (rp)
							{
							DoMethod(o,
								GM_RENDER,
								gpi->gpi_GInfo,
								rp,
								GREDRAW_UPDATE);
							ReleaseGIRPort(rp);
							}
						}
					}
				else
					{
					d1(KPrintF("%s/%s/%ld:  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->Flags));
					if (gg->Flags & GFLG_SELECTED)
						{
						gg->Flags &= ~GFLG_SELECTED;

						rp = ObtainGIRPort(gpi->gpi_GInfo);
						if (rp)
							{
							DoMethod(o,
								GM_RENDER,
								gpi->gpi_GInfo,
								rp,
								GREDRAW_UPDATE);
							ReleaseGIRPort(rp);
							}
						}
					}
				}
                        break;

		case MENUDOWN:
			d1(KPrintF("%s/%s/%ld:  MENUDOWN\n", __FILE__, __FUNC__, __LINE__));
			Result = GMR_REUSE;
			break;
		default:
			break;
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetGoInactive(Class *cl, Object *o, Msg msg)
{
	struct gpGoInactive *gpgi = (struct gpGoInactive *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort *rp;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));

	gg->Flags &= ~GFLG_SELECTED;

	rp = ObtainGIRPort(gpgi->gpgi_GInfo);
	if (rp)
		{
		DoMethod(o,
			GM_RENDER,
			gpgi->gpgi_GInfo,
			rp,
			GREDRAW_UPDATE);
		ReleaseGIRPort(rp);
		}

	CycleGadgetAsyncClosePopupWindow(cl, o);

	return 0;
}

//----------------------------------------------------------------------------

static ULONG CycleGadgetHandleMouseWheel(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	BOOL Updated = FALSE;

	d1(KPrintF("%s/%s/%ld: START Object=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y));

	switch (gpi->gpi_IEvent->ie_Code)
		{
	case NM_WHEEL_UP:
		// activate previous label
		if (inst->cgi_Active > 0)
			inst->cgi_Active--;
		else
			inst->cgi_Active = inst->cgi_NumberOfLabels - 1;
		Updated = TRUE;
		break;
	case NM_WHEEL_DOWN:
		// activate next label
		inst->cgi_Active++;
		if (inst->cgi_Active >= inst->cgi_NumberOfLabels)
			inst->cgi_Active = 0;
		Updated = TRUE;
		break;
	default:
		break;
		}

	if (Updated)
		{
		struct TagItem tl[3];
		struct RastPort *rp;

		d1(KPrintF("%s/%s/%ld: Updated  cgi_Active=%ld\n", __FILE__, __FUNC__, __LINE__, inst->cgi_Active));

		tl[0].ti_Tag = SCACYCLE_Active;
		tl[0].ti_Data = inst->cgi_Active;
		tl[1].ti_Tag = GA_ID;
		tl[1].ti_Data = gg->GadgetID;
		tl[2].ti_Tag = TAG_END;
		tl[2].ti_Data = 0;

		rp = ObtainGIRPort(gpi->gpi_GInfo);
		if (rp)
			{
			DoMethod(o,
				GM_RENDER,
				gpi->gpi_GInfo,
				rp,
				GREDRAW_UPDATE);
			ReleaseGIRPort(rp);
			}

		DoMethod(o,
			OM_NOTIFY,
			tl,
			gpi->gpi_GInfo,
			0);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


//----------------------------------------------------------------------------

static void CycleGadgetAsyncOpenPopupWindow(Class *cl, Object *o, struct GadgetInfo *GInfo)
{
	struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine,
		sizeof(struct CyclePopupMessage));

	d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	if (msg)
		{
		struct CyclePopupMessage *cpm = (struct CyclePopupMessage *) msg->smar_Args;

		msg->smar_EntryPoint = CycleGadgetOpenPopupWindow;

		cpm->cpm_GInfo = *GInfo;
		cpm->cpm_rp = *GInfo->gi_RastPort;
		cpm->cpm_GInfo.gi_RastPort = &cpm->cpm_rp;
		cpm->cpm_OurClass = cl;
		cpm->cpm_OurGadget = o;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &msg->ScalosMessage.sm_Message);
		}
}

//----------------------------------------------------------------------------

static void CycleGadgetAsyncDrawPopupWindowLine(Class *cl, Object *o,
	struct GadgetInfo *GInfo, ULONG LineNo)
{
	struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine,
		sizeof(struct CyclePopupMessage));

	d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	if (msg)
		{
		struct CyclePopupMessage *cpm = (struct CyclePopupMessage *) msg->smar_Args;

		msg->smar_EntryPoint = CycleGadgetPopupDrawWindowLine;

		cpm->cpm_GInfo = *GInfo;
		cpm->cpm_rp = *GInfo->gi_RastPort;
		cpm->cpm_GInfo.gi_RastPort = &cpm->cpm_rp;
		cpm->cpm_LineNo = LineNo;
		cpm->cpm_OurClass = cl;
		cpm->cpm_OurGadget = o;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &msg->ScalosMessage.sm_Message);
		}
}

//----------------------------------------------------------------------------

static void CycleGadgetAsyncClosePopupWindow(Class *cl, Object *o)
{
	struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine,
		sizeof(struct CyclePopupMessage));

	d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	if (msg)
		{
		struct CyclePopupMessage *cpm = (struct CyclePopupMessage *) msg->smar_Args;

		msg->smar_EntryPoint = CycleGadgetClosePopupWindow;

		cpm->cpm_OurClass = cl;
		cpm->cpm_OurGadget = o;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &msg->ScalosMessage.sm_Message);
		}
}

//----------------------------------------------------------------------------

static void CycleGadgetOpenPopupWindow(struct ScaWindowTask *wt, APTR arg)
{
	struct CyclePopupMessage *cpm = (struct CyclePopupMessage *) arg;
	Class *cl = cpm->cpm_OurClass;
	Object *o = cpm->cpm_OurGadget;
	struct GadgetInfo *GInfo = &cpm->cpm_GInfo;
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	ULONG Left, Top;
	ULONG Width, Height;
	ULONG GadgetLeft, GadgetTop, GadgetHeight;
	struct IBox FrameBox = { 0, 0, 20, 15 };
	struct IBox ContentsBox = { 3, 3, 20 - 2 * 3, 15 - 2 * 3 };


	// calculate offsets for outer frame
	DoMethod(inst->cgi_WindowFrame,
		IM_FRAMEBOX,
		&ContentsBox,
		&FrameBox,
		GInfo->gi_DrInfo,
		0);

	d1(KPrintF("%s/%s/%ld: FrameBox: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                FrameBox.Left, FrameBox.Top, FrameBox.Width, FrameBox.Height));
	d1(KPrintF("%s/%s/%ld: ContentsBox: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                ContentsBox.Left, ContentsBox.Top, ContentsBox.Width, ContentsBox.Height));

	inst->cgi_WindowBorderWidth = (FrameBox.Width - ContentsBox.Width) / 2;
	inst->cgi_WindowBorderHeight = (FrameBox.Height - ContentsBox.Height) / 2;

	GetAttr(GA_Width, o, &Width);
	GetAttr(GA_Height, o, &GadgetHeight);
	GetAttr(GA_Left, o, &GadgetLeft);
	GetAttr(GA_Top, o, &GadgetTop);

	Height = inst->cgi_NumberOfLabels * (1 + inst->cgi_TextLineHeight);

	inst->cgi_WindowX = inst->cgi_TextLeftEdge;

	Width -= inst->cgi_TextLeftEdge;
	Width += 2 * (1 + inst->cgi_WindowBorderWidth);
	Height += 2 * (1 + inst->cgi_WindowBorderHeight);
	Height--;

	inst->cgi_WindowY = -(Height - GadgetHeight) / 2;

	Left = GadgetLeft + inst->cgi_WindowX + GInfo->gi_Window->LeftEdge;
	Top = GadgetTop + inst->cgi_WindowY + GInfo->gi_Window->TopEdge;

	Width += POPUPWINDOW_SHADOW_X;
	Height += POPUPWINDOW_SHADOW_Y;

	inst->cgi_PopupWindow = OpenWindowTags(NULL,
			WA_Left, Left,
			WA_Top, Top,
			WA_Width, Width,
			WA_Height, Height,
			WA_SizeGadget, FALSE,
			WA_DragBar, FALSE,
			WA_CloseGadget, FALSE,
			WA_DepthGadget, FALSE,
			WA_Borderless, TRUE,
			WA_Activate, FALSE,
			WA_AutoAdjust, TRUE,
			WA_CustomScreen, GInfo->gi_Screen,
			WA_ReportMouse, TRUE,
			WA_BackFill, LAYERS_NOBACKFILL,
#if defined(WA_FrontWindow)
			WA_FrontWindow, TRUE,
#elif defined(WA_StayTop)
			WA_StayTop, TRUE,
#endif //defined(WA_StayTop)
			WA_IDCMP, 0,
			TAG_END);

	if (inst->cgi_PopupWindow)
		{
		ULONG n;

		if (inst->cgi_PopupBgImage)
			{
			FillBackground(inst->cgi_PopupWindow->RPort,
				inst->cgi_PopupBgImage,
				0, 0,
				inst->cgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X - 1,
				inst->cgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y - 1,
				0, 0);
			}
		else
			{
			SetAPen(inst->cgi_PopupWindow->RPort, GInfo->gi_DrInfo->dri_Pens[BACKGROUNDPEN]);
			RectFill(inst->cgi_PopupWindow->RPort,
				0, 0,
				inst->cgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X - 1,
				inst->cgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y - 1);
			}

		// Draw frame around window
		SetAttrs(inst->cgi_WindowFrame,
			IA_Width, inst->cgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X,
			IA_Height, inst->cgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y,
			IA_Recessed, FALSE,
			TAG_END);
		DrawImageState(inst->cgi_PopupWindow->RPort,
			(struct Image *) inst->cgi_WindowFrame,
			0, 0,
			IDS_NORMAL,
			GInfo->gi_DrInfo);

		// Draw window shadow
		CycleGadgetPopupDrawWindowShadow(inst, GInfo);

		for (n = 0; n < inst->cgi_NumberOfLabels; n++)
			{
			cpm->cpm_LineNo = n;
			CycleGadgetPopupDrawWindowLine(wt, cpm);
			}

		inst->cgi_PopupWindow->UserPort = GInfo->gi_Window->UserPort;
		ModifyIDCMP(inst->cgi_PopupWindow, IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS);

		ReportMouse(TRUE, inst->cgi_PopupWindow);
		}
}

//----------------------------------------------------------------------------

static void CycleGadgetClosePopupWindow(struct ScaWindowTask *wt, APTR arg)
{
	struct CyclePopupMessage *cpm = (struct CyclePopupMessage *) arg;
	Class *cl = cpm->cpm_OurClass;
	Object *o = cpm->cpm_OurGadget;
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);

	if (inst->cgi_PopupWindow)
		{
		Forbid();
		inst->cgi_PopupWindow->UserPort = NULL;
		ModifyIDCMP(inst->cgi_PopupWindow, 0L);
		Permit();

		CloseWindow(inst->cgi_PopupWindow);
		inst->cgi_PopupWindow = NULL;
		}
}

//----------------------------------------------------------------------------

static void CycleGadgetPopupDrawWindowLine(struct ScaWindowTask *wt, APTR arg)
{
	struct CyclePopupMessage *cpm = (struct CyclePopupMessage *) arg;
	Class *cl = cpm->cpm_OurClass;
	Object *o = cpm->cpm_OurGadget;
	ULONG LineNo = cpm->cpm_LineNo;
	struct GadgetInfo *GInfo = &cpm->cpm_GInfo;
	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct TextExtent textExt;
	struct RastPort *rp = inst->cgi_PopupWindow->RPort;
	size_t LabelLength;
	LONG x, y;
	struct Rectangle LineRect;
	ULONG checkWidth;
	ULONG Width;

	GetAttr(IA_Width, inst->cgi_CheckImage, &checkWidth);
	checkWidth += 2;	// leave 2 pixels distance between checkmark and text

	CycleGadgetPopupGetLineRect(inst, LineNo, &LineRect);

	if (LineNo == inst->cgi_PopupActive)
		{
		SetAPen(rp, GInfo->gi_DrInfo->dri_Pens[FILLPEN]);
		RectFill(rp,
			LineRect.MinX, LineRect.MinY,
			LineRect.MaxX, LineRect.MaxY);
		}
	else
		{
		if (inst->cgi_PopupBgImage)
			{
			FillBackground(inst->cgi_PopupWindow->RPort,
				inst->cgi_PopupBgImage,
				LineRect.MinX, LineRect.MinY,
				LineRect.MaxX, LineRect.MaxY,
				LineRect.MinX, LineRect.MinY);
			}
		else
			{
			SetAPen(rp, GInfo->gi_DrInfo->dri_Pens[BACKGROUNDPEN]);
			RectFill(rp,
				LineRect.MinX, LineRect.MinY,
				LineRect.MaxX, LineRect.MaxY);
			}
		}

	LabelLength = strlen(inst->cgi_Labels[LineNo]);

	d1(KPrintF("%s/%s/%ld:  gi_RastPort Font=%08lx\n", __FILE__, __FUNC__, __LINE__, GInfo->gi_RastPort->Font));
	d1(KPrintF("%s/%s/%ld:  gi_Window   Font=%08lx\n", __FILE__, __FUNC__, __LINE__, GInfo->gi_Window->RPort->Font));
	Scalos_SetFont(rp, inst->cgi_PopupFont, inst->cgi_PopupTTFont);

	Scalos_TextExtent(rp,
		inst->cgi_Labels[LineNo],
		LabelLength,
		&textExt);

	SetAPen(rp, GInfo->gi_DrInfo->dri_Pens[(LineNo == inst->cgi_PopupActive) ? FILLTEXTPEN : TEXTPEN]);
	SetDrMd(rp, JAM1);

	// Center label inside window area
	Width = inst->cgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X - checkWidth;
	y = LineRect.MinY + Scalos_GetFontBaseline(rp);
	x = checkWidth + (Width - textExt.te_Width) / 2;

	// Draw Label text
	Move(rp, x, y);
	Scalos_Text(rp, inst->cgi_Labels[LineNo], LabelLength);

	if (LineNo == inst->cgi_Active)
		{
		// for currently active line, draw checkmark
		ULONG checkHeight;

		GetAttr(IA_Height, inst->cgi_CheckImage, &checkHeight);

		x = LineRect.MinX;
		y = LineRect.MinY + ((1 + LineRect.MaxY - LineRect.MinY) - checkHeight) / 2;

		DrawImageState(rp,
			(struct Image *) inst->cgi_CheckImage,
			x, y,
			IDS_NORMAL,
			GInfo->gi_DrInfo);
		}
}

//----------------------------------------------------------------------------

static void CycleGadgetPopupGetLineRect(struct CycleGadgetInstance *inst, ULONG LineNo,
	struct Rectangle *Rect)
{
	Rect->MinY = Rect->MaxY = inst->cgi_WindowBorderHeight + 1
		+ LineNo * (1 + inst->cgi_TextLineHeight);
	Rect->MaxY += inst->cgi_TextLineHeight - 1;

	Rect->MinX = 1 + inst->cgi_WindowBorderWidth;
	Rect->MaxX = inst->cgi_PopupWindow->Width - (1 + inst->cgi_WindowBorderWidth) - 1 - POPUPWINDOW_SHADOW_X;

}

//----------------------------------------------------------------------------

static ULONG CycleGadgetPopupGetLineNoFromXY(struct CycleGadgetInstance *inst, WORD x, WORD y)
{
	x -= inst->cgi_WindowX;
	y -= inst->cgi_WindowY;

	d1(KPrintF("%s/%s/%ld:  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

	y -= inst->cgi_WindowBorderHeight + 1;

	if (x < (1 + inst->cgi_WindowBorderWidth))
		return ACTIVE_NONE;
	if (x > (inst->cgi_PopupWindow->Width - (1 + inst->cgi_WindowBorderWidth)))
		return ACTIVE_NONE;
	if (y < 0)
		return ACTIVE_NONE;

	y /= 1 + inst->cgi_TextLineHeight;
	if (y >= inst->cgi_NumberOfLabels)
		return ACTIVE_NONE;

	d1(KPrintF("%s/%s/%ld:  LineNo=%ld\n", __FILE__, __FUNC__, __LINE__, y));

	return y;
}

//----------------------------------------------------------------------------

static void CycleGadgetPopupDrawWindowShadow(struct CycleGadgetInstance *inst,
	struct GadgetInfo *GInfo)
{
	d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

	if (CyberGfxBase && GetBitMapAttr(inst->cgi_PopupWindow->RPort->BitMap, BMA_DEPTH) > 8)
		{
		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		WindowCastShadow(inst->cgi_PopupWindow, POPUPWINDOW_SHADOW_X, POPUPWINDOW_SHADOW_Y);
		}
	else
		{
		struct RastPort *rp = inst->cgi_PopupWindow->RPort;
		static UWORD Pattern[] = { 0x5555, 0xaaaa };

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		SetABPenDrMd(rp, GInfo->gi_DrInfo->dri_Pens[SHADOWPEN], 0, JAM1);
		SetAfPt(rp, Pattern, 1);

		// Render right shadow
		RectFill(rp,
			inst->cgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X,
			POPUPWINDOW_SHADOW_Y,
			inst->cgi_PopupWindow->Width - 1,
			inst->cgi_PopupWindow->Height - 1);

		// Render bottom shadow
		RectFill(rp,
			POPUPWINDOW_SHADOW_X,
			inst->cgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y,
			inst->cgi_PopupWindow->Width - 1,
			inst->cgi_PopupWindow->Height - 1);
		}
}

//----------------------------------------------------------------------------
 
// object == (struct RastPort *) result->RastPort
// message == [ (Layer *) layer, (struct Rectangle) bounds,
//              (LONG) offsetx, (LONG) offsety ]

static SAVEDS(ULONG) CycleGadget_BackFillFunc(struct Hook *bfHook,
	struct RastPort *rp, struct BackFillMsg *msg)
{
	struct CycleGadgetInstance *inst = (struct CycleGadgetInstance *) bfHook->h_Data;

	d1(KPrintF("%s/%s/%ld: START RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));
	d1(kprintf("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
	d1(kprintf("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY));

	if (inst->cgi_Background)
		{
		WindowBackFill(rp, msg, inst->cgi_Background->dti_BitMap,
			inst->cgi_Background->dti_BitMapHeader->bmh_Width,
			inst->cgi_Background->dti_BitMapHeader->bmh_Height,
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN],
			0, 0,
			NULL);
		}
	else
		{
		WindowBackFill(rp, msg, NULL,
			0, 0,
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN],
			0, 0,
			NULL);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------


