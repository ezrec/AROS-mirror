// HistoryGadgetClass.c
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
#include "HistoryGadgetClass.h"

//----------------------------------------------------------------------------

#define	POPUPWINDOW_SHADOW_X		8
#define	POPUPWINDOW_SHADOW_Y		8

#define	ACTIVE_NONE			NULL

#define BUILTIN_HISTORYIMAGEWIDTH     	19
#define BUILTIN_HISTORYIMAGEHEIGHT	8

//----------------------------------------------------------------------------

struct HistoryPopupMessage
	{
	struct GadgetInfo hpm_GInfo;
	struct RastPort hpm_rp;
	struct Node *hpm_PopupWindowNode;
	Class *hpm_OurClass;
	Object *hpm_OurGadget;
	};

//----------------------------------------------------------------------------

// HistoryGadgetClass instance data
struct HistoryGadgetInstance
	{
	struct Node *hgi_Active;	// pointer to active entry

	struct List *hgi_Entries;
	ULONG hgi_NumberOfEntries;

	Object *hgi_Frame;
	Object *hgi_HistoryImage;
	Object *hgi_CheckImage;		// checkmark to indicate active entry in popup window

	struct DatatypesImage *hgi_PopupBgImage;	// optional background image for popup window

	struct Rectangle hgi_EntryRect;	// maximum bounding rectangle of entry strings

	LONG hgi_HBorder;
	LONG hgi_VBorder;

	ULONG hgi_TextLineHeight;	// Height of an entry text line

	LONG hgi_BorderWidth; 		// additional horizontal space required for frame
	LONG hgi_BorderHeight;          // additional vertical space required for frame

	struct Window *hgi_PopupWindow;

	struct Node *hgi_PopupActive;	// number of entry under mouse pointer in popup window

	struct TextFont *hgi_PopupFont;		// font for popup window
	struct TTFontFamily *hgi_PopupTTFont;	// TTengine font for popup window

	ULONG hgi_Hidden;		// Flag: this gadget is hidden

	LONG hgi_WindowX;		// Offset from Gadget Left to popup window left
	LONG hgi_WindowY;		// Offset from gadget top to popup window top
	};

//----------------------------------------------------------------------------

// The functions in this module

static SAVEDS(ULONG) INTERRUPT dispatchHistoryGadgetClass(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetNew(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetDispose(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetSet(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetGet(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetUpdate(Class *cl, Object *o, Msg msg);
static void HistoryGadgetCountEntries(struct HistoryGadgetInstance *inst);
static ULONG HistoryGadgetLayout(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetRender(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetHitTest(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetHelpTest(Class *cl, Object *o, Msg msg);
static BOOL HistoryGadgetPointInGadget(Object *o, WORD x, WORD y);
static ULONG HistoryGadgetGoActive(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetHandleInput(Class *cl, Object *o, Msg msg);
static ULONG HistoryGadgetGoInactive(Class *cl, Object *o, Msg msg);
static void HistoryGadgetAsyncOpenPopupWindow(Class *cl, Object *o, struct GadgetInfo *GInfo);
static void HistoryGadgetAsyncDrawPopupWindowLine(Class *cl, Object *o,
	struct GadgetInfo *GInfo, struct Node *ln);
static void HistoryGadgetAsyncClosePopupWindow(Class *cl, Object *o);
static void HistoryGadgetOpenPopupWindow(struct ScaWindowTask *wt, APTR arg);
static void HistoryGadgetClosePopupWindow(struct ScaWindowTask *wt, APTR arg);
static void HistoryGadgetPopupDrawWindowLine(struct ScaWindowTask *wt, APTR arg);
static void HistoryGadgetPopupGetLineRect(struct HistoryGadgetInstance *inst, struct Node *ln,
	struct Rectangle *Rect);
static struct Node *HistoryGadgetPopupGetNodeFromXY(struct HistoryGadgetInstance *inst, WORD x, WORD y);
static void HistoryGadgetPopupDrawWindowShadow(struct HistoryGadgetInstance *inst,
	struct GadgetInfo *GInfo);

//----------------------------------------------------------------------------


/***********************************************************/
/**	Make the class and set up the dispatcher's hook	**/
/***********************************************************/
struct ScalosClass *initHistoryGadgetClass(const struct PluginClass *plug)
{
	struct ScalosClass *HistoryGadgetClass;

	HistoryGadgetClass = SCA_MakeScalosClass(plug->plug_classname,
			plug->plug_superclassname,
			sizeof(struct HistoryGadgetInstance),
			NULL);

	if (HistoryGadgetClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(HistoryGadgetClass->sccl_class->cl_Dispatcher, dispatchHistoryGadgetClass);
		}

	return HistoryGadgetClass;
}

/**************************************************************************/
/**********	   The HistoryGadgetCLASS class dispatcher         *********/
/**************************************************************************/
static SAVEDS(ULONG) INTERRUPT dispatchHistoryGadgetClass(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld:  Class=%08lx  SuperClass=%08lx  Method=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, cl->cl_Super, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = HistoryGadgetNew(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = HistoryGadgetDispose(cl, o, msg);
		break;
	case OM_SET:
		Result = HistoryGadgetSet(cl, o, msg);
		break;
	case OM_GET:
		Result = HistoryGadgetGet(cl, o, msg);
		break;
	case OM_UPDATE:
		Result = HistoryGadgetUpdate(cl, o, msg);
		break;
	case GM_LAYOUT:
		Result = HistoryGadgetLayout(cl, o, msg);
		break;
	case GM_RENDER:
		Result = HistoryGadgetRender(cl, o, msg);
		break;
	case GM_HITTEST:
		Result = HistoryGadgetHitTest(cl, o, msg);
		break;
	case GM_HELPTEST:
		Result = HistoryGadgetHelpTest(cl, o, msg);
		break;
	case GM_GOACTIVE:
		Result = HistoryGadgetGoActive(cl, o, msg);
		break;
	case GM_HANDLEINPUT:
		Result = HistoryGadgetHandleInput(cl, o, msg);
		break;
	case GM_GOINACTIVE:
		Result = HistoryGadgetGoInactive(cl, o, msg);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG HistoryGadgetNew(Class *cl, Object *o, Msg msg)
{
	BOOL Success = FALSE;

	do	{
		struct opSet *ops = ((struct opSet *) msg);
		struct HistoryGadgetInstance *inst;

		o  = (Object *) DoSuperMethodA(cl, o, msg);
		d1(KPrintF("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		if (NULL == o)
			break;

		/* Initial local instance data */
		inst = INST_DATA(cl, o);
		memset(inst, 0, sizeof(struct HistoryGadgetInstance));

		inst->hgi_Active = (struct Node *) GetTagData(SCAHISTORY_Active, (ULONG) ACTIVE_NONE, ops->ops_AttrList);
		inst->hgi_Entries = (struct List *) GetTagData(SCAHISTORY_Labels, (ULONG) NULL, ops->ops_AttrList);
		inst->hgi_Hidden = GetTagData(GBDTA_Hidden, FALSE, ops->ops_AttrList);

		HistoryGadgetCountEntries(inst);
		d1(KPrintF("%s/%s/%ld:  hgi_NumberOfEntries=%ld\n", __FILE__, __FUNC__, __LINE__, inst->hgi_NumberOfEntries));

		inst->hgi_Frame = NewObject(NULL, FRAMEICLASS,
			IA_FrameType, FRAME_BUTTON,
			IA_EdgesOnly, TRUE,
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  hgi_Frame=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->hgi_Frame));
		if (NULL == inst->hgi_Frame)
			break;

		inst->hgi_CheckImage = NewObject(NULL, SYSICLASS,
			SYSIA_Which, MENUCHECK,
			SYSIA_DrawInfo, iInfos.xii_iinfos.ii_DrawInfo,
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  hgi_CheckImage=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->hgi_CheckImage));
		if (NULL == inst->hgi_CheckImage)
			break;

		// hgi_HistoryImage is optional
		inst->hgi_HistoryImage = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) "THEME:Window/ControlBar/HistoryNormal",
			DTIMG_SelImageName, (ULONG) "THEME:Window/ControlBar/HistorySelected",
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  hgi_HistoryImage=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->hgi_HistoryImage));

		// hgi_PopupBgImage is optional
		inst->hgi_PopupBgImage = CreateDatatypesImage("THEME:Window/ControlBar/HistoryPopupBackground", 0);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  hgi_PopupBgImage=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->hgi_PopupBgImage));

		SetAttrs(o,
			GA_Width, GetTagData(GA_Width, 1, ops->ops_AttrList),
			GA_Height, GetTagData(GA_Height, 1, ops->ops_AttrList),
			TAG_END);

		d1(KPrintF("%s/%s/%ld:  gg->Width=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, ((struct Gadget *) o)->Width, ((struct Gadget *) o)->Height));

		inst->hgi_PopupFont = (struct TextFont *) GetTagData(GBTDTA_TextFont,
			(ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font, ops->ops_AttrList);
		inst->hgi_PopupTTFont = (struct TTFontFamily *) GetTagData(GBTDTA_TTFont,
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

static ULONG HistoryGadgetDispose(Class *cl, Object *o, Msg msg)
{
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);

	HistoryGadgetAsyncClosePopupWindow(cl, o);
	if (inst->hgi_Frame)
		{
		DisposeObject(inst->hgi_Frame);
		inst->hgi_Frame = NULL;
		}
	if (inst->hgi_HistoryImage)
		{
		DisposeObject(inst->hgi_HistoryImage);
		inst->hgi_HistoryImage = NULL;
		}
	if (inst->hgi_CheckImage)
		{
		DisposeObject(inst->hgi_CheckImage);
		inst->hgi_CheckImage = NULL;
		}

	DisposeDatatypesImage(&inst->hgi_PopupBgImage);

	return 0;
}

//----------------------------------------------------------------------------

static ULONG HistoryGadgetSet(Class *cl, Object *o, Msg msg)
{
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);
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
		case SCAHISTORY_Active:
			if (inst->hgi_Active != (struct Node *) tag->ti_Data)
				{
				inst->hgi_Active = (struct Node *) tag->ti_Data;
				NeedRerender = TRUE;
				}
			d1(KPrintF("%s/%s/%ld:  o=%08lx  hgi_Active=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->hgi_Active));
			break;
		case SCAHISTORY_Labels:
			inst->hgi_Entries = (struct List *) tag->ti_Data;
			d1(KPrintF("%s/%s/%ld:  o=%08lx  hgi_Entries=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->hgi_Entries));
			HistoryGadgetCountEntries(inst);
			NeedRerender = TRUE;
			break;
		case GBDTA_Hidden:
			inst->hgi_Hidden = tag->ti_Data;
			break;
		default:
			break;
			}
		}

	if (NeedRerender && !inst->hgi_Hidden)
		{
		struct RastPort *rp;

		d1(KPrintF("%s/%s/%ld:  o=%08lx  hgi_Active=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->hgi_Active));

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

static ULONG HistoryGadgetGet(Class *cl, Object *o, Msg msg)
{
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);
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
	case SCAHISTORY_Active:
		*(opg->opg_Storage) = (ULONG) inst->hgi_Active;
		break;
	case SCAHISTORY_Labels:
		*(opg->opg_Storage) = (ULONG) inst->hgi_Entries;
		break;
	case GBDTA_Hidden:
		*(opg->opg_Storage) = (ULONG) inst->hgi_Hidden;
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG HistoryGadgetUpdate(Class *cl, Object *o, Msg msg)
{
	struct opUpdate *opu = (struct opUpdate *) msg;

	DoMethod(o,
		OM_SET,
		opu->opu_AttrList,
		opu->opu_GInfo);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static void HistoryGadgetCountEntries(struct HistoryGadgetInstance *inst)
{
	struct Node *ln;

	for (inst->hgi_NumberOfEntries = 0, ln = inst->hgi_Entries->lh_Head;
		ln != (struct Node *) &inst->hgi_Entries->lh_Tail;
		ln = ln->ln_Succ)
		{
		inst->hgi_NumberOfEntries++;
		}
	d1(KPrintF("%s/%s/%ld: hgi_NumberOfEntries=%ld\n", __FILE__, __FUNC__, __LINE__, inst->hgi_NumberOfEntries));
}

//----------------------------------------------------------------------------

static ULONG HistoryGadgetLayout(Class *cl, Object *o, Msg msg)
{
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);
	struct gpLayout *gpl = (struct gpLayout *) msg;
	ULONG HistoryWidth = 0, HistoryHeight = 0;
	ULONG Width, Height;
	struct IBox FrameBox = { 0, 0, 20, 15 };
	struct IBox ContentsBox = { 3, 3, 20 - 2 * 3, 15 - 2 * 3 };

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx gpl_GInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gpl->gpl_GInfo));
	d1(KPrintF("%s/%s/%ld:  gg->Width=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

	if (NULL == gpl->gpl_GInfo)
		return DoSuperMethodA(cl, o, msg);

	// get dimensions of History popup image(s)
	if (inst->hgi_HistoryImage)
		{
		ULONG Width = 0, Height = 0;

		GetAttr(IA_Width, inst->hgi_HistoryImage, &Width);
		GetAttr(IA_Height, inst->hgi_HistoryImage, &Height);

		if (Width > HistoryWidth)
			HistoryWidth = Width;
		if (Height > HistoryHeight)
			HistoryHeight = Height;
		}
	else
		{
		// use built-in History image
		HistoryWidth = BUILTIN_HISTORYIMAGEWIDTH;
		HistoryHeight = BUILTIN_HISTORYIMAGEHEIGHT;
		}

	d1(KPrintF("%s/%s/%ld:  gpl_GInfo=%08lx  inst->hgi_Frame=%08lx\n", __FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo, inst->hgi_Frame));

	// calculate offsets for outer frame
	DoMethod(inst->hgi_Frame,
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

	inst->hgi_BorderWidth = (FrameBox.Width - ContentsBox.Width) / 2;
	inst->hgi_BorderHeight = (FrameBox.Height - ContentsBox.Height) / 2;

	inst->hgi_HBorder = 1 + inst->hgi_BorderWidth;
	inst->hgi_VBorder = inst->hgi_BorderHeight;

	// calculate minimum required width and height
	Width = HistoryWidth;
	Height = HistoryHeight;

	d1(KPrintF("%s/%s/%ld:  HistoryHeight=%ld  EntryRect Height=%ld\n", __FILE__, __FUNC__, __LINE__, HistoryHeight, inst->hgi_EntryRect.MaxY - inst->hgi_EntryRect.MinY));
	d1(KPrintF("%s/%s/%ld:  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height));

	SetAttrs(o, GA_Width, Width, TAG_END);
	SetAttrs(o, GA_Height, Height, TAG_END);

	d1(KPrintF("%s/%s/%ld:  gg->Width=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG HistoryGadgetRender(Class *cl, Object *o, Msg msg)
{
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);
	struct gpRender *gpr = (struct gpRender *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	WORD x, y;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));

	// Draw History image
	if (inst->hgi_HistoryImage)
		{
		// Draw History image
		DrawImageState(gpr->gpr_RPort,
			(struct Image *) inst->hgi_HistoryImage,
			gg->LeftEdge,
			gg->TopEdge,
			(gg->Flags & GFLG_SELECTED) ? IDS_SELECTED : IDS_NORMAL,
			gpr->gpr_GInfo->gi_DrInfo);
		}
	else
		{
		WORD h;

		// Draw built-in History image
		h = gg->Height / 2;
		x = gg->LeftEdge + 6;

		SetAPen(gpr->gpr_RPort, gpr->gpr_GInfo->gi_DrInfo->dri_Pens[TEXTPEN]);
		for (y = 0; y < 4; y++)
			{
			RectFill(gpr->gpr_RPort,
				x + y,
				gg->TopEdge + gg->Height - 1 - h - y - 1,
				x + 6 - y,
				gg->TopEdge + gg->Height - 1 - h - y - 1);

			RectFill(gpr->gpr_RPort,
				x + y,
				gg->TopEdge + h + y + 1,
				x + 6 - y,
				gg->TopEdge + h + y + 1);
			}
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG HistoryGadgetHitTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	return (ULONG) (HistoryGadgetPointInGadget(o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y) ? GMR_GADGETHIT : 0);
}

//----------------------------------------------------------------------------

static ULONG HistoryGadgetHelpTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	if (HistoryGadgetPointInGadget(o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y))
		{
		d1(KPrintF("%s/%s/%ld: Result=GMR_HELPHIT\n", __FILE__, __FUNC__, __LINE__));
		return GMR_HELPHIT;
		}

	d1(KPrintF("%s/%s/%ld: Result=GMR_NOHELPHIT\n", __FILE__, __FUNC__, __LINE__));

	return GMR_NOHELPHIT;
}

//----------------------------------------------------------------------------

// x, y are already relative to the Gadget position (0,0) is top left corner!
static BOOL HistoryGadgetPointInGadget(Object *o, WORD x, WORD y)
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

static ULONG HistoryGadgetGoActive(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort *rp;
	ULONG Result;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));

	gg->Flags |= GFLG_SELECTED;

	HistoryGadgetAsyncOpenPopupWindow(cl, o, gpi->gpi_GInfo);

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

static ULONG HistoryGadgetHandleInput(Class *cl, Object *o, Msg msg)
{
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);
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
				if (inst->hgi_PopupWindow)
					{
					struct Node *NewActive;

					NewActive = HistoryGadgetPopupGetNodeFromXY(inst, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y);
					d1(KPrintF("%s/%s/%ld:  NewActive=%08lx\n", __FILE__, __FUNC__, __LINE__, NewActive));
					if (ACTIVE_NONE != NewActive)
						{
						inst->hgi_Active = NewActive;
						}
					}
				else
					{
					if (gpi->gpi_IEvent->ie_Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
						{
						// activate previous entry
						if (inst->hgi_Active != inst->hgi_Entries->lh_Head)
							inst->hgi_Active = inst->hgi_Active->ln_Pred;
						}
					else
						{
						// activate next entry
						if (inst->hgi_Active != inst->hgi_Entries->lh_TailPred)
							inst->hgi_Active = inst->hgi_Active->ln_Succ;
						}
					}

				d1(KPrintF("%s/%s/%ld:  hgi_Active=%ld\n", __FILE__, __FUNC__, __LINE__, inst->hgi_Active));

				*gpi->gpi_Termination = (ULONG) inst->hgi_Active;
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

			if (inst->hgi_PopupWindow)
				{
				struct Node *NewActive;
				
				NewActive = HistoryGadgetPopupGetNodeFromXY(inst, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y);
				d1(KPrintF("%s/%s/%ld:  NewActive=%08lx\n", __FILE__, __FUNC__, __LINE__, NewActive));
				if (NewActive != inst->hgi_PopupActive)
					{
					struct Node *OldActive = inst->hgi_PopupActive;

					inst->hgi_PopupActive = NewActive;

					if (ACTIVE_NONE != OldActive)
						{
						HistoryGadgetAsyncDrawPopupWindowLine(cl, o,
							gpi->gpi_GInfo, OldActive);
						}

					if (ACTIVE_NONE != NewActive)
						{
						HistoryGadgetAsyncDrawPopupWindowLine(cl, o,
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

				if (GMR_GADGETHIT == DoMethodA(o,(APTR) &gpht))
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

static ULONG HistoryGadgetGoInactive(Class *cl, Object *o, Msg msg)
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

	HistoryGadgetAsyncClosePopupWindow(cl, o);

	return 0;
}

//----------------------------------------------------------------------------

static void HistoryGadgetAsyncOpenPopupWindow(Class *cl, Object *o, struct GadgetInfo *GInfo)
{
	struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine,
		sizeof(struct HistoryPopupMessage));

	d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	if (msg)
		{
		struct HistoryPopupMessage *hpm = (struct HistoryPopupMessage *) msg->smar_Args;

		msg->smar_EntryPoint = HistoryGadgetOpenPopupWindow;

		hpm->hpm_GInfo = *GInfo;
		hpm->hpm_rp = *GInfo->gi_RastPort;
		hpm->hpm_GInfo.gi_RastPort = &hpm->hpm_rp;
		hpm->hpm_PopupWindowNode = NULL;
		hpm->hpm_OurClass = cl;
		hpm->hpm_OurGadget = o;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &msg->ScalosMessage.sm_Message);
		}
}

//----------------------------------------------------------------------------

static void HistoryGadgetAsyncDrawPopupWindowLine(Class *cl, Object *o,
	struct GadgetInfo *GInfo, struct Node *ln)
{
	struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine,
		sizeof(struct HistoryPopupMessage));

	d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	if (msg)
		{
		struct HistoryPopupMessage *hpm = (struct HistoryPopupMessage *) msg->smar_Args;

		msg->smar_EntryPoint = HistoryGadgetPopupDrawWindowLine;

		hpm->hpm_GInfo = *GInfo;
		hpm->hpm_rp = *GInfo->gi_RastPort;
		hpm->hpm_GInfo.gi_RastPort = &hpm->hpm_rp;
		hpm->hpm_PopupWindowNode = ln;
		hpm->hpm_OurClass = cl;
		hpm->hpm_OurGadget = o;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &msg->ScalosMessage.sm_Message);
		}
}

//----------------------------------------------------------------------------

static void HistoryGadgetAsyncClosePopupWindow(Class *cl, Object *o)
{
	struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine,
		sizeof(struct HistoryPopupMessage));

	d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	if (msg)
		{
		struct HistoryPopupMessage *hpm = (struct HistoryPopupMessage *) msg->smar_Args;

		msg->smar_EntryPoint = HistoryGadgetClosePopupWindow;

		hpm->hpm_OurClass = cl;
		hpm->hpm_OurGadget = o;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &msg->ScalosMessage.sm_Message);
		}
}

//----------------------------------------------------------------------------

static void HistoryGadgetOpenPopupWindow(struct ScaWindowTask *wt, APTR arg)
{
	struct HistoryPopupMessage *hpm = (struct HistoryPopupMessage *) arg;
	Class *cl = hpm->hpm_OurClass;
	Object *o = hpm->hpm_OurGadget;
	struct GadgetInfo *GInfo = &hpm->hpm_GInfo;
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);
	ULONG Left, Top;
	ULONG Width, Height;
	ULONG GadgetLeft, GadgetTop, GadgetHeight;
	ULONG checkWidth = 0, checkHeight = 0;
	struct Node *ln;
	struct RastPort rp;

	rp = *GInfo->gi_RastPort;

	GetAttr(GA_Height, o, &GadgetHeight);
	GetAttr(GA_Left, o, &GadgetLeft);
	GetAttr(GA_Top, o, &GadgetTop);

	// get dimensions of Check image
	GetAttr(IA_Width, inst->hgi_CheckImage, &checkWidth);
	GetAttr(IA_Height, inst->hgi_CheckImage, &checkHeight);

	HistoryGadgetCountEntries(inst);

	Scalos_SetFont(&rp, inst->hgi_PopupFont, inst->hgi_PopupTTFont);

	// reserve room for bold text
	Scalos_SetSoftStyle(&rp, FSF_BOLD, FSF_BOLD, inst->hgi_PopupTTFont);

	// find largest dimensions of entry string
	inst->hgi_TextLineHeight = Scalos_GetFontHeight(&rp);

	d1(KPrintF("%s/%s/%ld:  hgi_TextLineHeight=%ld\n", __FILE__, __FUNC__, __LINE__, inst->hgi_TextLineHeight));

	inst->hgi_EntryRect.MinX = inst->hgi_EntryRect.MinY = SHRT_MAX;
	inst->hgi_EntryRect.MaxX = inst->hgi_EntryRect.MaxY = SHRT_MIN;

	d1(KPrintF("%s/%s/%ld:  GInfo=%08lx  gi_RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, GInfo, GInfo->gi_RastPort));

	for (ln = inst->hgi_Entries->lh_Head;
		ln != (struct Node *) &inst->hgi_Entries->lh_Tail;
		ln = ln->ln_Succ)
		{
		struct TextExtent textExt;

		Scalos_TextExtent(&rp,
			ln->ln_Name,
			strlen(ln->ln_Name),
			&textExt);

		d1(KPrintF("%s/%s/%ld:  textExt:  <%s>  MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, \
			ln->ln_Name, \
			textExt.te_Extent.MinX, textExt.te_Extent.MaxX, \
			textExt.te_Extent.MinY, textExt.te_Extent.MaxY));

		if (textExt.te_Extent.MinX < inst->hgi_EntryRect.MinX)
			inst->hgi_EntryRect.MinX = textExt.te_Extent.MinX;
		if (textExt.te_Extent.MinY < inst->hgi_EntryRect.MinY)
			inst->hgi_EntryRect.MinY = textExt.te_Extent.MinY;
		if (textExt.te_Extent.MaxX > inst->hgi_EntryRect.MaxX)
			inst->hgi_EntryRect.MaxX = textExt.te_Extent.MaxX;
		if (textExt.te_Extent.MaxY > inst->hgi_EntryRect.MaxY)
			inst->hgi_EntryRect.MaxY = textExt.te_Extent.MaxY;
		}

	d1(KPrintF("%s/%s/%ld:  hgi_EntryRect:  MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
                inst->hgi_EntryRect.MinX, inst->hgi_EntryRect.MaxX, \
		inst->hgi_EntryRect.MinY, inst->hgi_EntryRect.MaxY));

	// Reserve space for checkbox
	inst->hgi_EntryRect.MinX -= checkWidth + 2;
	if ((1 + inst->hgi_EntryRect.MaxY - inst->hgi_EntryRect.MinY) < checkHeight)
		{
		// make sure height is sufficient for checkbox
		inst->hgi_EntryRect.MinY -= checkHeight / 2;
		inst->hgi_EntryRect.MaxY += checkHeight - (checkHeight / 2);
		}

	d1(KPrintF("%s/%s/%ld:  hgi_NumberOfEntries=%ld\n", __FILE__, __FUNC__, __LINE__, inst->hgi_NumberOfEntries));

	Height = inst->hgi_NumberOfEntries * (1 + inst->hgi_TextLineHeight);
	Width  = 1 + inst->hgi_EntryRect.MaxX - inst->hgi_EntryRect.MinX;

	inst->hgi_WindowX = 0;

	Width  += 2 * (1 + inst->hgi_BorderWidth);
	Height += 2 * (1 + inst->hgi_BorderHeight);
	Height--;

	inst->hgi_WindowY = -(Height - GadgetHeight) / 2;

	Left = GadgetLeft + inst->hgi_WindowX + GInfo->gi_Window->LeftEdge;
	Top = GadgetTop + inst->hgi_WindowY + GInfo->gi_Window->TopEdge;

	Width += POPUPWINDOW_SHADOW_X;
	Height += POPUPWINDOW_SHADOW_Y;

	inst->hgi_PopupWindow = OpenWindowTags(NULL,
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
#endif /* WA_FrontWindow */
			WA_IDCMP, 0,
			TAG_END);

	if (inst->hgi_PopupWindow)
		{
		struct Node *ln;

		if (inst->hgi_PopupBgImage)
			{
			FillBackground(inst->hgi_PopupWindow->RPort,
				inst->hgi_PopupBgImage,
				0, 0,
				inst->hgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X - 1,
				inst->hgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y - 1,
				0, 0);
			}
		else
			{
			SetAPen(inst->hgi_PopupWindow->RPort, GInfo->gi_DrInfo->dri_Pens[BACKGROUNDPEN]);
			RectFill(inst->hgi_PopupWindow->RPort,
				0, 0,
				inst->hgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X - 1,
				inst->hgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y - 1);
			}

		// Draw frame around window
		SetAttrs(inst->hgi_Frame,
			IA_Width, inst->hgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X,
			IA_Height, inst->hgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y,
			IA_Recessed, FALSE,
			TAG_END);
		DrawImageState(inst->hgi_PopupWindow->RPort,
			(struct Image *) inst->hgi_Frame,
			0, 0,
			IDS_NORMAL,
			GInfo->gi_DrInfo);

		// Draw window shadow
		HistoryGadgetPopupDrawWindowShadow(inst, GInfo);

		for (ln = inst->hgi_Entries->lh_Head;
			ln != (struct Node *) &inst->hgi_Entries->lh_Tail;
			ln = ln->ln_Succ)
			{
			hpm->hpm_PopupWindowNode = ln;
			HistoryGadgetPopupDrawWindowLine(wt, hpm);
			}

		inst->hgi_PopupWindow->UserPort = GInfo->gi_Window->UserPort;
		ModifyIDCMP(inst->hgi_PopupWindow, IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS);

		ReportMouse(TRUE, inst->hgi_PopupWindow);
		}
}

//----------------------------------------------------------------------------

static void HistoryGadgetClosePopupWindow(struct ScaWindowTask *wt, APTR arg)
{
	struct HistoryPopupMessage *hpm = (struct HistoryPopupMessage *) arg;
	Class *cl = hpm->hpm_OurClass;
	Object *o = hpm->hpm_OurGadget;
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);

	if (inst->hgi_PopupWindow)
		{
		Forbid();
		inst->hgi_PopupWindow->UserPort = NULL;
		ModifyIDCMP(inst->hgi_PopupWindow, 0L);
		Permit();

		CloseWindow(inst->hgi_PopupWindow);
		inst->hgi_PopupWindow = NULL;
		}
}

//----------------------------------------------------------------------------

static void HistoryGadgetPopupDrawWindowLine(struct ScaWindowTask *wt, APTR arg)
{
	struct HistoryPopupMessage *hpm = (struct HistoryPopupMessage *) arg;
	Class *cl = hpm->hpm_OurClass;
	Object *o = hpm->hpm_OurGadget;
	struct Node *ln = hpm->hpm_PopupWindowNode;
	struct GadgetInfo *GInfo = &hpm->hpm_GInfo;
	struct HistoryGadgetInstance *inst = INST_DATA(cl, o);
	struct TextExtent textExt;
	struct RastPort *rp = inst->hgi_PopupWindow->RPort;
	size_t EntryLength;
	LONG x, y;
	struct Rectangle LineRect;
	ULONG checkWidth;
	ULONG Width;

	GetAttr(IA_Width, inst->hgi_CheckImage, &checkWidth);
	checkWidth += 2;	// leave 2 pixels distance between checkmark and text

	HistoryGadgetPopupGetLineRect(inst, ln, &LineRect);
	d1(KPrintF("%s/%s/%ld:  MinX=%ld  MaxXy=%ld  MinY=%ld  MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, LineRect.MinX, LineRect.MaxX, LineRect.MinY, LineRect.MaxY));

	if (ln == inst->hgi_PopupActive)
		{
		SetAPen(rp, GInfo->gi_DrInfo->dri_Pens[FILLPEN]);

		RectFill(rp,
			LineRect.MinX, LineRect.MinY,
			LineRect.MaxX, LineRect.MaxY);
		}
	else
		{
		if (inst->hgi_PopupBgImage)
			{
			FillBackground(inst->hgi_PopupWindow->RPort,
				inst->hgi_PopupBgImage,
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

	EntryLength = strlen(ln->ln_Name);

	Scalos_SetFont(rp, inst->hgi_PopupFont, inst->hgi_PopupTTFont);
	Scalos_SetSoftStyle(rp, (ln == inst->hgi_Active) ? FSF_BOLD : FS_NORMAL, FSF_BOLD, inst->hgi_PopupTTFont);

	Scalos_TextExtent(rp,
		ln->ln_Name,
		EntryLength,
		&textExt);

	SetAPen(rp, GInfo->gi_DrInfo->dri_Pens[(ln == inst->hgi_PopupActive) ? FILLTEXTPEN : TEXTPEN]);
	SetDrMd(rp, JAM1);

	// Center entry inside window area
	Width = inst->hgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X - checkWidth;
	y = LineRect.MinY + Scalos_GetFontBaseline(rp);
	x = checkWidth + (Width - textExt.te_Width) / 2;

	d1(KPrintF("%s/%s/%ld:  x=%ld  checkWidth=%ld  Width=%ld  te_Width=%ld\n", \
		__FILE__, __FUNC__, __LINE__, x, checkWidth, Width, textExt.te_Width));

	// Draw entry text
	Move(rp, x, y);
	Scalos_Text(rp, ln->ln_Name, EntryLength);

	if (ln == inst->hgi_Active)
		{
		// for currently active line, draw checkmark
		ULONG checkHeight;

		GetAttr(IA_Height, inst->hgi_CheckImage, &checkHeight);

		x = LineRect.MinX;
		y = LineRect.MinY + ((1 + LineRect.MaxY - LineRect.MinY) - checkHeight) / 2;

		DrawImageState(rp,
			(struct Image *) inst->hgi_CheckImage,
			x, y,
			IDS_NORMAL,
			GInfo->gi_DrInfo);
		}
}

//----------------------------------------------------------------------------

static void HistoryGadgetPopupGetLineRect(struct HistoryGadgetInstance *inst, struct Node *lnx,
	struct Rectangle *Rect)
{
	ULONG LineNo;
	struct Node *ln;

	for (LineNo = 0, ln = inst->hgi_Entries->lh_Head;
		ln != (struct Node *) &inst->hgi_Entries->lh_Tail;
		ln = ln->ln_Succ , LineNo++)
		{
		if (ln == lnx)
			break;
		}

	Rect->MinY = Rect->MaxY = inst->hgi_BorderHeight + 1
		+ LineNo * (1 + inst->hgi_TextLineHeight);
	Rect->MaxY += inst->hgi_TextLineHeight - 1;

	Rect->MinX = 1 + inst->hgi_BorderWidth;
	Rect->MaxX = inst->hgi_PopupWindow->Width - (1 + inst->hgi_BorderWidth) - 1 - POPUPWINDOW_SHADOW_X;

}

//----------------------------------------------------------------------------

static struct Node *HistoryGadgetPopupGetNodeFromXY(struct HistoryGadgetInstance *inst, WORD x, WORD y)
{
	struct Node *ln;
	WORD n;

	x -= inst->hgi_WindowX;
	y -= inst->hgi_WindowY;

	d1(KPrintF("%s/%s/%ld:  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

	y -= inst->hgi_BorderHeight + 1;

	if (x < (1 + inst->hgi_BorderWidth))
		return ACTIVE_NONE;
	if (x > (inst->hgi_PopupWindow->Width - (1 + inst->hgi_BorderWidth)))
		return ACTIVE_NONE;
	if (y < 0)
		return ACTIVE_NONE;

	y /= 1 + inst->hgi_TextLineHeight;
	if (y >= inst->hgi_NumberOfEntries)
		return ACTIVE_NONE;

	d1(KPrintF("%s/%s/%ld:  LineNo=%ld\n", __FILE__, __FUNC__, __LINE__, y));

	for (n = 0, ln = inst->hgi_Entries->lh_Head;
		ln != (struct Node *) &inst->hgi_Entries->lh_Tail;
		ln = ln->ln_Succ , n++)
		{
		if (n == y)
			return ln;
		}

	return ACTIVE_NONE;
}

//----------------------------------------------------------------------------

static void HistoryGadgetPopupDrawWindowShadow(struct HistoryGadgetInstance *inst,
	struct GadgetInfo *GInfo)
{
	d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

	if (CyberGfxBase && GetBitMapAttr(inst->hgi_PopupWindow->RPort->BitMap, BMA_DEPTH) > 8)
		{
		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		WindowCastShadow(inst->hgi_PopupWindow, POPUPWINDOW_SHADOW_X, POPUPWINDOW_SHADOW_Y);
		}
	else
		{
		struct RastPort *rp = inst->hgi_PopupWindow->RPort;
		static UWORD Pattern[] = { 0x5555, 0xaaaa };

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		SetABPenDrMd(rp, GInfo->gi_DrInfo->dri_Pens[SHADOWPEN], 0, JAM1);
		SetAfPt(rp, Pattern, 1);

		// Render right shadow
		RectFill(rp,
			inst->hgi_PopupWindow->Width - POPUPWINDOW_SHADOW_X,
			POPUPWINDOW_SHADOW_Y,
			inst->hgi_PopupWindow->Width - 1,
			inst->hgi_PopupWindow->Height - 1);

		// Render bottom shadow
		RectFill(rp,
			POPUPWINDOW_SHADOW_X,
			inst->hgi_PopupWindow->Height - POPUPWINDOW_SHADOW_Y,
			inst->hgi_PopupWindow->Width - 1,
			inst->hgi_PopupWindow->Height - 1);
		}
}

//----------------------------------------------------------------------------


