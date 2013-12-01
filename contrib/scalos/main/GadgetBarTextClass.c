// GadgetBarTextClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/newmouse.h>
#include <utility/hooks.h>
#include <libraries/gadtools.h>
#include <datatypes/pictureclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
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
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------
// Revision history :
//
// 20011101	jl	initial history
//----------------------------------------------------------------------------

// local data definitions

struct GadgetBarTextClassInst
	{
	STRPTR gbtcl_Text;
	struct TextFont *gbtcl_Font;
	struct TTFontFamily *gbtcl_TTFont;	// TTengine font
	WORD gbtcl_TextPen;
	WORD gbtcl_BGPen;
	WORD gbtcl_DrawMode;
	WORD gbtcl_Justification;	// GACT_STRINGLEFT, GACT_STRINGRIGHT, GACT_STRINGCENTER
	ULONG gbtcl_SoftStyle;		// FSF_BOLD, FSF_ITALIC, FSF_UNDERLINED, ...
	struct TextExtent gbtcl_Extent;
	ULONG gbtcl_Chars;		// number of chars that fit within the gadget
	BOOL gbtcl_NeedLayout;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT GadgetBarTextClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarText_New(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarText_Dispose(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarText_Set(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarText_Get(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarText_Layout(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarText_Render(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarText_HelpTest(Class *cl, Object *o, Msg msg);
static BOOL GadgetBarText_PointInGadget(const struct ExtGadget *gg, WORD x, WORD y);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

struct ScalosClass *initGadgetBarTextClass(const struct PluginClass *plug)
{
	struct ScalosClass *GadgetBarTextClass;

	GadgetBarTextClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct GadgetBarTextClassInst),
			NULL);

	if (GadgetBarTextClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(GadgetBarTextClass->sccl_class->cl_Dispatcher, GadgetBarTextClassDispatcher);
		}

	return GadgetBarTextClass;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT GadgetBarTextClassDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = GadgetBarText_New(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = GadgetBarText_Dispose(cl, o, msg);
		break;
	case OM_SET:
		Result = GadgetBarText_Set(cl, o, msg);
		break;
	case OM_GET:
		Result = GadgetBarText_Get(cl, o, msg);
		break;
	case GM_RENDER:
		Result = GadgetBarText_Render(cl, o, msg);
		break;
	case GM_LAYOUT:
		Result = GadgetBarText_Layout(cl, o, msg);
		break;
	case GM_HELPTEST:
		Result = GadgetBarText_HelpTest(cl, o, msg);
		break;
	case GM_HITTEST:
		return 0;
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarText_New(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct GadgetBarTextClassInst *inst;
	struct ExtGadget *gg;

	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (NULL == o)
		return 0;

	inst = INST_DATA(cl, o);
	gg = (struct ExtGadget *) o;

	memset(inst, 0, sizeof(struct GadgetBarTextClassInst));

	gg->Width = GetTagData(GA_Width, 0, ops->ops_AttrList);
	gg->Height = GetTagData(GA_Height, 0, ops->ops_AttrList);

	gg->BoundsWidth = gg->Width;
	gg->BoundsHeight = gg->Height;

	inst->gbtcl_NeedLayout = TRUE;

	inst->gbtcl_Text = (STRPTR) GetTagData(GBTDTA_Text, (ULONG) "", ops->ops_AttrList);
	inst->gbtcl_Font = (struct TextFont *) GetTagData(GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font, ops->ops_AttrList);
	inst->gbtcl_TTFont = (struct TTFontFamily *) GetTagData(GBTDTA_TTFont, (ULONG) &ScreenTTFont, ops->ops_AttrList);
	inst->gbtcl_TextPen = GetTagData(GBTDTA_TextPen, PalettePrefs.pal_driPens[TEXTPEN], ops->ops_AttrList);
	inst->gbtcl_BGPen = GetTagData(GBTDTA_BgPen, PalettePrefs.pal_driPens[BACKGROUNDPEN], ops->ops_AttrList);
	inst->gbtcl_DrawMode = GetTagData(GBTDTA_DrawMode, JAM1, ops->ops_AttrList);
	inst->gbtcl_Justification = GetTagData(GBTDTA_Justification, GACT_STRINGCENTER, ops->ops_AttrList);
	inst->gbtcl_SoftStyle = GetTagData(GBTDTA_SoftStyle, FS_NORMAL, ops->ops_AttrList);

	if (inst->gbtcl_Text)
		inst->gbtcl_Text = AllocCopyString(inst->gbtcl_Text);

	d1(KPrintF("%s/%s/%ld: gbtcl_Font=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->gbtcl_Font));
	d1(KPrintF("%s/%s/%ld: tf_YSize=%lu\n", __FILE__, __FUNC__, __LINE__, inst->gbtcl_Font->tf_YSize));
	d1(KPrintF("%s/%s/%ld: ttff=%08lx Normal=%08lx Bold=%08lx Italic=%08lx BoldItalic=%08lx\n", \
		__LINE__, inst->gbtcl_TTFont, inst->gbtcl_TTFont->ttff_Normal, \
		inst->gbtcl_TTFont->ttff_Bold, inst->gbtcl_TTFont->ttff_Italic, \
		inst->gbtcl_TTFont->ttff_BoldItalic));

	if (inst->gbtcl_Text)
		{
		struct RastPort rp;
		struct TextExtent tExt;

		InitRastPort(&rp);
		Scalos_SetFont(&rp, inst->gbtcl_Font, inst->gbtcl_TTFont);
		Scalos_TextExtent(&rp, inst->gbtcl_Text, strlen(inst->gbtcl_Text), &tExt);

		if (gg->Height < tExt.te_Height)
			gg->Height = tExt.te_Height;
		if (gg->BoundsHeight < tExt.te_Height)
			gg->BoundsHeight = tExt.te_Height;

		d1(kprintf("%s/%s/%ld: gg=%08lx  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg, gg->Width, gg->Height));
		}
	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarText_Dispose(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarTextClassInst *inst = INST_DATA(cl, o);

	if (inst->gbtcl_Text)
		FreeCopyString(inst->gbtcl_Text);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBarText_Get(Class *cl, Object *o, Msg msg)
{
	struct opGet *opg = (struct opGet *) msg;
	struct GadgetBarTextClassInst *inst = INST_DATA(cl, o);
	struct Gadget *gg = (struct Gadget *) o;
	ULONG Result = 1;

	d1(KPrintF("%s/%s/%ld: AttrID=%08lx\n", __FILE__, __FUNC__, __LINE__, opg->opg_AttrID));

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
	case GBTDTA_Text:
		*(opg->opg_Storage) = (ULONG) inst->gbtcl_Text;
		break;
	case GBTDTA_TextFont:
		*(opg->opg_Storage) = (ULONG) inst->gbtcl_Font;
		break;
	case GBTDTA_TextPen:
		*(opg->opg_Storage) = inst->gbtcl_TextPen;
		break;
	case GBTDTA_BgPen:
		*(opg->opg_Storage) = inst->gbtcl_BGPen;
		break;
	case GBTDTA_DrawMode:
		*(opg->opg_Storage) = inst->gbtcl_DrawMode;
		break;
	case GBTDTA_Justification:
		*(opg->opg_Storage) = inst->gbtcl_Justification;
		break;
	case GBTDTA_SoftStyle:
		*(opg->opg_Storage) = inst->gbtcl_SoftStyle;
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarText_Set(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarTextClassInst *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;

	if (FindTagItem(GBTDTA_Text, ops->ops_AttrList))
		{
		STRPTR NewText = (STRPTR) GetTagData(GBTDTA_Text, (ULONG) inst->gbtcl_Text, ops->ops_AttrList);

		if (inst->gbtcl_Text)
			FreeCopyString(inst->gbtcl_Text);

		inst->gbtcl_Text = AllocCopyString(NewText);
		inst->gbtcl_NeedLayout = TRUE;
		}

	inst->gbtcl_Font = (struct TextFont *) GetTagData(GBTDTA_TextFont, (ULONG) inst->gbtcl_Font, ops->ops_AttrList);
	inst->gbtcl_TTFont = (struct TTFontFamily *) GetTagData(GBTDTA_TTFont, (ULONG) inst->gbtcl_TTFont, ops->ops_AttrList);
	inst->gbtcl_TextPen = GetTagData(GBTDTA_TextPen, inst->gbtcl_TextPen, ops->ops_AttrList);
	inst->gbtcl_BGPen = GetTagData(GBTDTA_BgPen, inst->gbtcl_BGPen, ops->ops_AttrList);
	inst->gbtcl_DrawMode = GetTagData(GBTDTA_DrawMode, inst->gbtcl_DrawMode, ops->ops_AttrList);
	inst->gbtcl_Justification = GetTagData(GBTDTA_Justification, inst->gbtcl_Justification, ops->ops_AttrList);
	inst->gbtcl_SoftStyle = GetTagData(GBTDTA_SoftStyle, inst->gbtcl_SoftStyle, ops->ops_AttrList);

	if ( FindTagItem(GBTDTA_TextFont, ops->ops_AttrList)
		|| FindTagItem(GBTDTA_TTFont, ops->ops_AttrList)
		|| FindTagItem(GBTDTA_DrawMode, ops->ops_AttrList)
		|| FindTagItem(GBTDTA_SoftStyle, ops->ops_AttrList)
		|| FindTagItem(GA_Width, ops->ops_AttrList)
		|| FindTagItem(GA_Height, ops->ops_AttrList) )
		{
		inst->gbtcl_NeedLayout = TRUE;
		}

	gg->BoundsLeftEdge = GetTagData(GA_Left, gg->BoundsLeftEdge, ops->ops_AttrList);
	gg->BoundsTopEdge = GetTagData(GA_Top, gg->BoundsTopEdge, ops->ops_AttrList);
	gg->BoundsWidth = GetTagData(GA_Width, gg->BoundsWidth, ops->ops_AttrList);
	gg->BoundsHeight = GetTagData(GA_Height, gg->BoundsHeight, ops->ops_AttrList);
	gg->Width = GetTagData(GA_Width, gg->Width, ops->ops_AttrList);
	gg->Height = GetTagData(GA_Height, gg->Height, ops->ops_AttrList);

	d1(kprintf("%s/%s/%ld: gg=%08lx  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg, gg->Width, gg->Height));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBarText_Layout(Class *cl, Object *o, Msg msg)
{
	struct gpLayout *gpl = (struct gpLayout *) msg;
	struct GadgetBarTextClassInst *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  gpl_Initial=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge, gpl->gpl_Initial));

	if (gpl->gpl_Initial)
		{
		struct RastPort rp;
		WORD /* NewWidth, */ NewHeight;

		Scalos_InitRastPort(&rp);

		Scalos_SetFont(&rp, inst->gbtcl_Font, inst->gbtcl_TTFont);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		SetABPenDrMd(&rp,
			inst->gbtcl_TextPen,
			inst->gbtcl_BGPen,
			inst->gbtcl_DrawMode);

		inst->gbtcl_Chars = strlen(inst->gbtcl_Text);

		Scalos_TextExtent(&rp,
			inst->gbtcl_Text,
			inst->gbtcl_Chars,
			&inst->gbtcl_Extent);

		if (inst->gbtcl_Extent.te_Width > gg->Width)
			{
			inst->gbtcl_Chars = Scalos_TextFit(&rp,
				inst->gbtcl_Text,
				inst->gbtcl_Chars,
				&inst->gbtcl_Extent,
				NULL,
				1,
				gg->Width,
				gg->Height + 2);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		NewHeight = inst->gbtcl_Extent.te_Height + 1 + 1;
#if 0
		NewWidth = inst->gbtcl_Extent.te_Width + 1 + 1;

		if (gg->Width < NewWidth)
			gg->Width = gg->BoundsWidth = NewWidth;
#endif
		if (gg->Height < NewHeight)
			gg->Height = gg->BoundsHeight = NewHeight;

		d1(kprintf("%s/%s/%ld: gg=%08lx  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg, gg->Width, gg->Height));

		Scalos_DoneRastPort(&rp);

		inst->gbtcl_NeedLayout = FALSE;
		}

	d1(kprintf("%s/%s/%ld: gg=%08lx  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg, gg->Width, gg->Height));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarText_Render(Class *cl, Object *o, Msg msg)
{
	struct gpRender *gpr = (struct gpRender *) msg;
	struct GadgetBarTextClassInst *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	WORD x, y;

	d1(KPrintF("%s/%s/%ld: o=%08lx  Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, o, gg->LeftEdge, gg->TopEdge));
	d1(kprintf("%s/%s/%ld: gg=%08lx  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg, gg->Width, gg->Height));

	if (inst->gbtcl_NeedLayout)
		{
		DoMethod(o, GM_LAYOUT,
			(ULONG) gpr->gpr_GInfo,
			TRUE);
		}

	Scalos_DoneRastPort(gpr->gpr_RPort);
	Scalos_SetFont(gpr->gpr_RPort, inst->gbtcl_Font, inst->gbtcl_TTFont);

	d1(kprintf("%s/%s/%ld: <%s\n", __FILE__, __FUNC__, __LINE__, inst->gbtcl_Text));
	Scalos_SetSoftStyle(gpr->gpr_RPort, inst->gbtcl_SoftStyle, inst->gbtcl_SoftStyle, inst->gbtcl_TTFont);

	SetABPenDrMd(gpr->gpr_RPort, 
		inst->gbtcl_TextPen,
		inst->gbtcl_BGPen,
		inst->gbtcl_DrawMode);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	x = gg->LeftEdge;
	y = gg->TopEdge;

	switch (inst->gbtcl_Justification)
		{
	case GACT_STRINGLEFT:
		break;
	case GACT_STRINGRIGHT:
		x += gg->Width - inst->gbtcl_Extent.te_Extent.MaxX;
		break;
	case GACT_STRINGCENTER:
		x += (gg->Width - inst->gbtcl_Extent.te_Width) / 2;
		break;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	y += (gg->Height - inst->gbtcl_Extent.te_Height) / 2 + Scalos_GetFontBaseline(gpr->gpr_RPort);

#if 0
	// DEBUG: draw border around text gadget
	Move(gpr->gpr_RPort, gg->LeftEdge, gg->TopEdge);
	Draw(gpr->gpr_RPort, gg->LeftEdge + gg->Width, gg->TopEdge);
	Draw(gpr->gpr_RPort, gg->LeftEdge + gg->Width, gg->TopEdge + gg->Height);
	Draw(gpr->gpr_RPort, gg->LeftEdge, gg->TopEdge + gg->Height);
	Draw(gpr->gpr_RPort, gg->LeftEdge, gg->TopEdge);
#endif

	Move(gpr->gpr_RPort, x, y);
	Scalos_Text(gpr->gpr_RPort, inst->gbtcl_Text, inst->gbtcl_Chars);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarText_HelpTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;

	d1(KPrintF("%s/%s/%ld: Object=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	if (GadgetBarText_PointInGadget((struct ExtGadget *) o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y))
		{
		d1(KPrintF("%s/%s/%ld: Result=GMR_HELPHIT\n", __FILE__, __FUNC__, __LINE__));
		return GMR_HELPHIT;
		}

	d1(KPrintF("%s/%s/%ld: Result=GMR_NOHELPHIT\n", __FILE__, __FUNC__, __LINE__));

	return GMR_NOHELPHIT;
}

//----------------------------------------------------------------------------

static BOOL GadgetBarText_PointInGadget(const struct ExtGadget *gg, WORD x, WORD y)
{
	if (x < 0)
		return FALSE;
	if (y < 0)
		return FALSE;
	if (x >= gg->Width)
		return FALSE;
	if (y >= gg->Height)
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------

