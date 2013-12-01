// TextIconHighlightClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/gfxmacros.h>
#include <graphics/rastport.h>
#include <graphics/rpattr.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <utility/hooks.h>
#include <libraries/gadtools.h>
#include <datatypes/pictureclass.h>
#include <cybergraphx/cybergraphics.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "TextIconHighlightClass.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

struct TextIconHighlightClassInst
	{
	WORD tih_FgPen;
	UWORD tih_Radius;
	UBYTE tih_Transparency;
	struct ARGB tih_BaseColor;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT TextIconHighlightClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG TIHighlight_New(Class *cl, Object *o, Msg msg);
static ULONG TIHighlight_Dispose(Class *cl, Object *o, Msg msg);
static ULONG TIHighlight_Get(Class *cl, Object *o, Msg msg);
static ULONG TIHighlight_Set(Class *cl, Object *o, Msg msg);
static ULONG TIHighlight_Draw(Class *cl, Object *o, Msg msg);
static ULONG TIHighlight_DrawSimple(struct TextIconHighlightClassInst *inst, const struct Image *img, struct impDraw *imp);
static ULONG TIHighlight_DrawEnhanced(struct TextIconHighlightClassInst *inst, const struct Image *img, struct impDraw *imp);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

Class *initTextIconHighlightClass(void)
{
	Class *TextIconHighlightClass;

	TextIconHighlightClass = MakeClass( NULL,
			(STRPTR) IMAGECLASS,
			NULL,
			sizeof(struct TextIconHighlightClassInst),
			0 );

	if (TextIconHighlightClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(TextIconHighlightClass->cl_Dispatcher, TextIconHighlightClassDispatcher);
		}

	return TextIconHighlightClass;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT TextIconHighlightClassDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = TIHighlight_New(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = TIHighlight_Dispose(cl, o, msg);
		break;
	case IM_DRAW:
		Result = TIHighlight_Draw(cl, o, msg);
		break;
	case OM_SET:
		Result = TIHighlight_Set(cl, o, msg);
		break;
	case OM_GET:
		Result = TIHighlight_Get(cl, o, msg);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG TIHighlight_New(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct TextIconHighlightClassInst *inst;
	static const struct ARGB DefaultBaseColor = { (UBYTE) ~0, 133, 195, 221 };

	o = (Object *) DoSuperMethodA(cl, o, msg);
	d1(KPrintF("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	if (NULL == o)
		return 0;

	inst = INST_DATA(cl, o);

	memset(inst, 0, sizeof(struct TextIconHighlightClassInst));

	inst->tih_FgPen =  GetTagData(IA_FGPen, PalettePrefs.pal_PensList[PENIDX_FILEBG], ops->ops_AttrList);
	inst->tih_BaseColor = *(const struct ARGB *) GetTagData(TIHA_BaseColor, (ULONG) &DefaultBaseColor, ops->ops_AttrList);
	inst->tih_Radius = GetTagData(TIHA_Radius, 3, ops->ops_AttrList);
	inst->tih_Transparency = GetTagData(TIHA_Transparency, 128, ops->ops_AttrList);

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG TIHighlight_Dispose(Class *cl, Object *o, Msg msg)
{
//	  struct TextIconHighlightClassInst *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld: o=%08lx  \n", __FILE__, __FUNC__, __LINE__, o));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG TIHighlight_Get(Class *cl, Object *o, Msg msg)
{
	struct opGet *opg = (struct opGet *) msg;
	struct TextIconHighlightClassInst *inst = INST_DATA(cl, o);
	ULONG Result = 1;

	if (NULL == opg->opg_Storage)
		return 0;

	switch (opg->opg_AttrID)
		{
	case TIHA_BaseColor:
		*((struct ARGB *) opg->opg_Storage) = inst->tih_BaseColor;
		break;
	case TIHA_Radius:
		*(opg->opg_Storage) = inst->tih_FgPen;
		break;
	case IA_FGPen:
		*(opg->opg_Storage) = inst->tih_Radius;
		break;
	case TIHA_Transparency:
		*(opg->opg_Storage) = inst->tih_Transparency;
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG TIHighlight_Set(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct TextIconHighlightClassInst *inst = INST_DATA(cl, o);

	inst->tih_FgPen        =  GetTagData(IA_FGPen, inst->tih_FgPen, ops->ops_AttrList);
	inst->tih_BaseColor    = *(const struct ARGB *) GetTagData(TIHA_BaseColor, (ULONG) &inst->tih_BaseColor, ops->ops_AttrList);
	inst->tih_Radius       = GetTagData(TIHA_Radius, inst->tih_Radius, ops->ops_AttrList);
	inst->tih_Transparency = GetTagData(TIHA_Transparency, inst->tih_Transparency, ops->ops_AttrList);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG TIHighlight_Draw(Class *cl, Object *o, Msg msg)
{
	struct impDraw *imp = (struct impDraw *) msg;
	struct Image *img = (struct Image *) o;
	struct TextIconHighlightClassInst *inst = INST_DATA(cl, o);
	ULONG Result;
	ULONG ScreenDepth;

	d1(KPrintF("%s/%s/%ld: o=%08lx  Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, o, img->LeftEdge, img->TopEdge));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld  imp_State=%ld\n", __FILE__, __FUNC__, __LINE__, img->Width, img->Height, imp->imp_State));

	ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	if (CyberGfxBase && (ScreenDepth > 8))
		Result = TIHighlight_DrawEnhanced(inst, img, imp);
	else
		Result = TIHighlight_DrawSimple(inst, img, imp);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return Result;
}

//----------------------------------------------------------------------------

static ULONG TIHighlight_DrawSimple(struct TextIconHighlightClassInst *inst, const struct Image *img, struct impDraw *imp)
{
	d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, img->LeftEdge, img->TopEdge));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld  imp_State=%ld\n", __FILE__, __FUNC__, __LINE__, img->Width, img->Height, imp->imp_State));

	SetAPen(imp->imp_RPort, inst->tih_FgPen);
	RectFill(imp->imp_RPort,
		img->LeftEdge, img->TopEdge,
		img->LeftEdge + img->Width - 1,
		img->TopEdge + img->Height - 1);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG TIHighlight_DrawEnhanced(struct TextIconHighlightClassInst *inst, const struct Image *img, struct impDraw *imp)
{
	struct ARGB *argb = NULL;

	d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, img->LeftEdge, img->TopEdge));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld  imp_State=%ld\n", __FILE__, __FUNC__, __LINE__, img->Width, img->Height, imp->imp_State));

	do	{
		WORD TextLeft = 0;
		WORD TextRight = TextLeft + img->Width - 1;
		WORD TextTop = 0;
		WORD TextBottom = TextTop + img->Height - 1;
		struct ARGB Border, Top, Bottom;
		struct ARGBHeader argbh;

		argb = ScalosGfxCreateARGB(img->Width, img->Height, NULL);
		if (NULL == argb)
			break;

		argbh.argb_Width = img->Width;
		argbh.argb_Height = img->Height;
		argbh.argb_ImageData = argb;

		// Read background into argbh
		ReadPixelArray(argbh.argb_ImageData,
			0, 0,
			argbh.argb_Width * sizeof(struct ARGB),
			imp->imp_RPort,
			img->LeftEdge, img->TopEdge,
			img->Width, img->Height,
			RECTFMT_ARGB);

		Border = inst->tih_BaseColor;

		//  1-----2		TextTop
		// /       \            .
		// 8       3            .
		// |       |            .
		// 7       4            .
		//  \     /             .
		//   6---5              TextBottom

		ScalosGfxDrawLine(&argbh, TextLeft + inst->tih_Radius, 	TextTop,			// 1 -> 2
					  TextRight - inst->tih_Radius, TextTop,
				&Border);
		ScalosGfxDrawLine(&argbh, TextRight - inst->tih_Radius,	TextTop, 			// 2 -> 3
					  TextRight, 	         	TextTop + inst->tih_Radius,
				&Border);
		ScalosGfxDrawLine(&argbh, TextRight, 			TextTop + inst->tih_Radius, 	// 3 -> 4
					  TextRight, 	         	TextBottom - inst->tih_Radius,
				&Border);
		ScalosGfxDrawLine(&argbh, TextRight, 	          	TextBottom - inst->tih_Radius, 	// 4 -> 5
					  TextRight - inst->tih_Radius, TextBottom,
				&Border);
		ScalosGfxDrawLine(&argbh, TextRight - inst->tih_Radius, TextBottom,			// 5 -> 6
					  TextLeft + inst->tih_Radius,  TextBottom,
				&Border);
		ScalosGfxDrawLine(&argbh, TextLeft + inst->tih_Radius,  TextBottom,			// 6 -> 7
					  TextLeft, 	          	TextBottom - inst->tih_Radius,
				&Border);
		ScalosGfxDrawLine(&argbh, TextLeft, 	          	TextBottom - inst->tih_Radius, 	// 7 -> 8
					  TextLeft, 	          	TextTop + inst->tih_Radius,
				&Border);
		ScalosGfxDrawLine(&argbh, TextLeft, 	          	TextTop + inst->tih_Radius, 	// 8 -> 1
					  TextLeft + inst->tih_Radius, 	TextTop,
				&Border);

		Top.Red   = (inst->tih_BaseColor.Red   * 79) / 100;
		Top.Green = (inst->tih_BaseColor.Green * 79) / 100;
		Top.Blue  = (inst->tih_BaseColor.Blue  * 79) / 100;
		Top.Alpha = inst->tih_Transparency;

		Bottom.Red   = (inst->tih_BaseColor.Red   * 41) / 100;
		Bottom.Green = (inst->tih_BaseColor.Green * 41) / 100;
		Bottom.Blue  = (inst->tih_BaseColor.Blue  * 41) / 100;
		Bottom.Alpha = inst->tih_Transparency;

		if (!ScalosGfxDrawGradient(&argbh, 2, 2,
			img->Width - 4, img->Height - 4,
			&Top, &Bottom,
			SCALOS_GRADIENT_VERTICAL))
			{
			break;
			}

		ScalosGfxBlitARGBAlpha(imp->imp_RPort, &argbh,
			img->LeftEdge, img->TopEdge,
			0, 0,
			img->Width, img->Height);

		d1(KPrintF("%s/%s/%ld:  iobj_TextPenBgSel=%ld\n", __FILE__, __FUNC__, __LINE__, inst->iobj_TextPenBgSel));
		} while (0);

	if (argb)
		ScalosGfxFreeARGB(&argb);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

