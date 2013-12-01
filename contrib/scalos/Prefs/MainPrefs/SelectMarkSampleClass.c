// SelectMarkSampleClass.c
// $Date$


#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>
#include <intuition/imageclass.h>
#include <cybergraphx/cybergraphics.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>
#include <proto/scalosgfx.h>

#include <clib/alib_protos.h>
#include <proto/intuition.h>

#include <defs.h>
#include <scalos/scalosgfx.h>
#include "SelectMarkSampleClass.h"
#include "ScalosPrefs.h"

// -------------------------------------------------------------------------

#define SUPERCLASS 	MUIC_Area

#define MIN_WIDTH	50
#define	MIN_HEIGHT	20

struct SelectMarkSampleInstance
{
	WORD smsi_FgPen;
	UWORD smsi_Radius;
	UBYTE smsi_Transparency;
	struct ARGB smsi_BaseColor;
	CONST_STRPTR smsi_SampleText;
	struct TextExtent smsi_TextExtent;
	struct ARGBHeader smsi_Background;
	struct ARGBHeader smsi_Foreground;
};

// -------------------------------------------------------------------------

static ULONG mNew(Class *cl, Object *o, struct opSet *ops);
static ULONG mDispose(Class *cl, Object *o,Msg msg);
static ULONG mSet(Class *cl, Object *o, struct opSet *ops);
static ULONG mGet(Class *cl, Object *o, struct opGet *opg);
static ULONG mAskMinMax(Class *cl, Object *o,struct MUIP_AskMinMax *msg);
static ULONG mDraw(Class *cl, Object *o,struct MUIP_Draw *msg);
DISPATCHER_PROTO(SelectMarkSampleClass);

// -------------------------------------------------------------------------

static ULONG mNew(Class *cl, Object *o, struct opSet *ops)
{
	struct SelectMarkSampleInstance *inst = NULL;
	BOOL Success = FALSE;

	do	{
		static const struct ARGB DefaultBaseColor = { (UBYTE) ~0, 133, 195, 221 };

		o = (Object *) DoSuperMethodA(cl, o, (Msg) ops);
		if (NULL == o)
			break;

		inst = INST_DATA(cl,o);

		memset(inst, 0, sizeof(struct SelectMarkSampleInstance));

		inst->smsi_FgPen =  GetTagData(IA_FGPen, 1, ops->ops_AttrList);
		inst->smsi_BaseColor = *(const struct ARGB *) GetTagData(TIHA_BaseColor, (ULONG) &DefaultBaseColor, ops->ops_AttrList);
		inst->smsi_Radius = GetTagData(TIHA_Radius, 3, ops->ops_AttrList);
		inst->smsi_Transparency = GetTagData(TIHA_Transparency, 128, ops->ops_AttrList);
		inst->smsi_SampleText = (CONST_STRPTR) GetTagData(TIHA_DemoString, (ULONG) "", ops->ops_AttrList);

		Success = TRUE;
		} while (0);

	if (!Success && o)
		{
		mDispose(cl, o, (Msg) ops);
		o = NULL;
		}

	return((ULONG)o);
}

// -------------------------------------------------------------------------

static ULONG mDispose(Class *cl, Object *o, Msg msg)
{
	struct SelectMarkSampleInstance *inst = INST_DATA(cl,o);

	ScalosGfxFreeARGB(&inst->smsi_Background.argb_ImageData);
	ScalosGfxFreeARGB(&inst->smsi_Foreground.argb_ImageData);

	return DoSuperMethodA(cl, o, msg);
}

// -------------------------------------------------------------------------

static ULONG mSet(Class *cl, Object *o, struct opSet *ops)
{
	struct SelectMarkSampleInstance *inst = INST_DATA(cl,o);
	struct ARGB oldBaseColor = inst->smsi_BaseColor;
	UWORD oldRadius = inst->smsi_Radius;
	UBYTE oldTransparency = inst->smsi_Transparency;

	inst->smsi_FgPen        = GetTagData(IA_FGPen, inst->smsi_FgPen, ops->ops_AttrList);
	inst->smsi_BaseColor    = *(const struct ARGB *) GetTagData(TIHA_BaseColor, (ULONG) &inst->smsi_BaseColor, ops->ops_AttrList);
	inst->smsi_Radius       = GetTagData(TIHA_Radius, inst->smsi_Radius, ops->ops_AttrList);
	inst->smsi_Transparency = GetTagData(TIHA_Transparency, inst->smsi_Transparency, ops->ops_AttrList);
	inst->smsi_SampleText   = (CONST_STRPTR) GetTagData(TIHA_DemoString, (ULONG) inst->smsi_SampleText, ops->ops_AttrList);

	if (inst->smsi_Radius != oldRadius || inst->smsi_Transparency != oldTransparency
		|| 0 != memcmp(&oldBaseColor, &inst->smsi_BaseColor, sizeof(struct ARGB)) )
		{
		MUI_Redraw(o, MADF_DRAWUPDATE);
		}

	return DoSuperMethodA(cl, o, (Msg) ops);
}

// -------------------------------------------------------------------------

static ULONG mGet(Class *cl, Object *o, struct opGet *opg)
{
	struct SelectMarkSampleInstance *inst = INST_DATA(cl,o);
	ULONG Result = 1;

	switch (opg->opg_AttrID)
		{
	case TIHA_BaseColor:
		*((struct ARGB *) opg->opg_Storage) = inst->smsi_BaseColor;
		break;
	case TIHA_Radius:
		*(opg->opg_Storage) = inst->smsi_FgPen;
		break;
	case IA_FGPen:
		*(opg->opg_Storage) = inst->smsi_Radius;
		break;
	case TIHA_Transparency:
		*(opg->opg_Storage) = inst->smsi_Transparency;
		break;
	case TIHA_DemoString:
		*(opg->opg_Storage) = (ULONG) inst->smsi_SampleText;
		break;
	default:
		Result = DoSuperMethodA(cl, o, (Msg) opg);
		break;
		}

	return Result;
}

// -------------------------------------------------------------------------

static ULONG mAskMinMax(Class *cl, Object *o,struct MUIP_AskMinMax *msg)
{
	struct SelectMarkSampleInstance *inst = INST_DATA(cl,o);
	struct MUI_MinMax *mi;
	struct TextFont *tf;

	DoSuperMethodA(cl, o, (Msg) msg);

	mi = msg->MinMaxInfo;

	tf = _font(o);
	d1(kprintf("%s/%s/%ld: tf=%08lx\n", __FILE__, __FUNC__, __LINE__, tf));
	if (tf)
		{
		struct RastPort rp;

		InitRastPort(&rp);
		SetFont(&rp, tf);

		TextExtent(&rp, inst->smsi_SampleText, strlen(inst->smsi_SampleText), &inst->smsi_TextExtent);

		mi->MinHeight += (2 + inst->smsi_TextExtent.te_Height) * 3;
		mi->DefHeight += (2 + inst->smsi_TextExtent.te_Height) * 3;

		mi->DefWidth += inst->smsi_TextExtent.te_Width + 4;
		}
	else
		{
		mi->MinHeight += MIN_HEIGHT;
		mi->DefHeight += MIN_HEIGHT;

		mi->DefWidth += MIN_WIDTH;
		}

	mi = msg->MinMaxInfo;

	mi->MaxWidth = MUI_MAXMAX;
	mi->MaxHeight = MUI_MAXMAX;

	d1(KPrintF("%s/%s/%ld: MinW=%ld  MaxW=%ld  MinH=%ld  MaxH=%ld  DefW=%ld  DefH=%ld\n", \
		__FILE__, __FUNC__, __LINE__, mi->MinWidth, mi->MaxWidth, mi->MinHeight, mi->MaxHeight, mi->DefWidth, mi->DefHeight));

	return 0;
}

// -------------------------------------------------------------------------

static ULONG mDraw(Class *cl, Object *o,struct MUIP_Draw *msg)
{
	struct SelectMarkSampleInstance *inst = INST_DATA(cl,o);

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	DoSuperMethodA(cl, o, (Msg) msg);

	d1(kprintf("%s/%s/%ld: flags=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->flags));
	d1(KPrintF("%s/%s/%ld: o=%08lx  Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, o, _mleft(o), _mtop(o)));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, _mwidth(o), _mheight(o)));

	do	{
		WORD TextLeft = (_mwidth(o) - inst->smsi_TextExtent.te_Width) / 2;
		WORD TextRight = TextLeft + inst->smsi_TextExtent.te_Width + 4;
		WORD TextTop = (_mheight(o) - inst->smsi_TextExtent.te_Height) / 2;
		WORD TextBottom = TextTop + inst->smsi_TextExtent.te_Height + 4;
		struct ARGB Border, Top, Bottom;

		if (msg->flags & MADF_DRAWOBJECT)
			{
			ScalosGfxFreeARGB(&inst->smsi_Background.argb_ImageData);

			inst->smsi_Background.argb_Width = _mwidth(o);
			inst->smsi_Background.argb_Height = _mheight(o);

			inst->smsi_Background.argb_ImageData = ScalosGfxCreateARGB(_mwidth(o), _mheight(o), NULL);
			if (NULL == inst->smsi_Background.argb_ImageData)
				break;

			SetFont(_rp(o), _font(o));
			SetDrMd(_rp(o), JAM1);

			Move(_rp(o),
				_mleft(o) + TextLeft + 2,
				_mtop(o)  + TextTop  + _font(o)->tf_Baseline + 2 );
			Text(_rp(o), inst->smsi_SampleText, strlen(inst->smsi_SampleText));

			Move(_rp(o),
				_mleft(o) + TextLeft + 2,
				_mtop(o)  + TextTop  + _font(o)->tf_Baseline + 2 - (inst->smsi_TextExtent.te_Height + 2) );
			Text(_rp(o), inst->smsi_SampleText, strlen(inst->smsi_SampleText));

			Move(_rp(o),
				_mleft(o) + TextLeft + 2,
				_mtop(o)  + TextTop  + _font(o)->tf_Baseline + 2 + (inst->smsi_TextExtent.te_Height + 2) );
			Text(_rp(o), inst->smsi_SampleText, strlen(inst->smsi_SampleText));

			// Read background into inst->smsi_Background
			ReadPixelArray(inst->smsi_Background.argb_ImageData,
				0, 0,
				_mwidth(o) * sizeof(struct ARGB),
				_rp(o),
				_mleft(o), _mtop(o),
				_mwidth(o), _mheight(o),
				RECTFMT_ARGB);
			}

		if (_mwidth(o) != inst->smsi_Foreground.argb_Width || _mheight(o) != inst->smsi_Foreground.argb_Height)
			{
			inst->smsi_Foreground.argb_Width = _mwidth(o);
			inst->smsi_Foreground.argb_Height = _mheight(o);
			ScalosGfxFreeARGB(&inst->smsi_Foreground.argb_ImageData);
			}

		if (NULL == inst->smsi_Foreground.argb_ImageData)
			inst->smsi_Foreground.argb_ImageData = ScalosGfxCreateARGB(_mwidth(o), _mheight(o), NULL);
		if (NULL == inst->smsi_Foreground.argb_ImageData)
			break;

		// Read background into inst->smsi_Foreground
		ScalosGfxBlitARGB(&inst->smsi_Foreground,
			&inst->smsi_Background,
			0, 0,
			0, 0,
			inst->smsi_Foreground.argb_Width, inst->smsi_Foreground.argb_Height);

		Border = inst->smsi_BaseColor;

		//  1-----2		TextTop
		// /       \            .
		// 8       3            .
		// |       |            .
		// 7       4            .
		//  \     /             .
		//   6---5              TextBottom

		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextLeft + inst->smsi_Radius, TextTop,				// 1 -> 2
				TextRight - inst->smsi_Radius,TextTop,
				&Border);
		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextRight - inst->smsi_Radius,TextTop, 				// 2 -> 3
				TextRight,             	      TextTop + inst->smsi_Radius,
				&Border);
		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextRight, 		      TextTop + inst->smsi_Radius,	// 3 -> 4
				TextRight,             	      TextBottom - inst->smsi_Radius,
				&Border);
		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextRight,                    TextBottom - inst->smsi_Radius,	// 4 -> 5
				TextRight - inst->smsi_Radius,TextBottom,
				&Border);
		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextRight - inst->smsi_Radius,TextBottom,			// 5 -> 6
				TextLeft + inst->smsi_Radius, TextBottom,
				&Border);
		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextLeft + inst->smsi_Radius, TextBottom,			// 6 -> 7
				TextLeft,                     TextBottom - inst->smsi_Radius,
				&Border);
		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextLeft,                     TextBottom - inst->smsi_Radius,	// 7 -> 8
				TextLeft,                     TextTop + inst->smsi_Radius,
				&Border);
		ScalosGfxDrawLine(&inst->smsi_Foreground,
				TextLeft,                     TextTop + inst->smsi_Radius,	// 8 -> 1
				TextLeft + inst->smsi_Radius, TextTop,
				&Border);

		Top.Red   = (inst->smsi_BaseColor.Red   * 79) / 100;
		Top.Green = (inst->smsi_BaseColor.Green * 79) / 100;
		Top.Blue  = (inst->smsi_BaseColor.Blue  * 79) / 100;
		Top.Alpha = inst->smsi_Transparency;

		Bottom.Red   = (inst->smsi_BaseColor.Red   * 41) / 100;
		Bottom.Green = (inst->smsi_BaseColor.Green * 41) / 100;
		Bottom.Blue  = (inst->smsi_BaseColor.Blue  * 41) / 100;
		Bottom.Alpha = inst->smsi_Transparency;

		if (msg->flags & MADF_DRAWUPDATE)
			{
			// Restore drawing area background
			ScalosGfxBlitARGBAlpha(_rp(o), &inst->smsi_Background,
				_mleft(o) + TextLeft, _mtop(o) + TextTop,
				TextLeft, TextTop,
				1 + TextRight - TextLeft,
				1 + TextBottom - TextTop);
			}
		if (!ScalosGfxDrawGradient(&inst->smsi_Foreground,
			TextLeft + 2,
			TextTop + 2,
			(1+ TextRight - TextLeft) - 4,
			(1 + TextBottom - TextTop) - 4,
			&Top, &Bottom,
			SCALOS_GRADIENT_VERTICAL))
			{
			break;
			}

		ScalosGfxBlitARGBAlpha(_rp(o), &inst->smsi_Foreground,
			_mleft(o) + TextLeft, _mtop(o) + TextTop,
			TextLeft, TextTop,
			1 + TextRight - TextLeft,
			1 + TextBottom - TextTop);

		SetFont(_rp(o), _font(o));
		SetDrMd(_rp(o), JAM1);

		Move(_rp(o),
			_mleft(o) + TextLeft + 2,
			_mtop(o)  + TextTop  + _font(o)->tf_Baseline + 2 );
		Text(_rp(o), inst->smsi_SampleText, strlen(inst->smsi_SampleText));

		d1(KPrintF("%s/%s/%ld:  iobj_TextPenBgSel=%ld\n", __FILE__, __FUNC__, __LINE__, inst->iobj_TextPenBgSel));
		} while (0);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
	return 0;
}

// -------------------------------------------------------------------------

DISPATCHER(SelectMarkSampleClass)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		d1(kprintf("%s/%s/%ld: OM_NEW\n", __FILE__, __FUNC__, __LINE__));
		Result = mNew(cl, obj, (struct opSet *) msg);
		break;

	case OM_DISPOSE:
		d1(kprintf("%s/%s/%ld: OM_DISPOSE\n", __FILE__, __FUNC__, __LINE__));
		Result = mDispose(cl, obj, msg);
		break;

	case OM_SET:
		d1(kprintf("%s/%s/%ld: OM_SET\n", __FILE__, __FUNC__, __LINE__));
		Result = mSet(cl, obj, (struct opSet *) msg);
		break;

	case OM_GET:
		d1(kprintf("%s/%s/%ld: OM_GET\n", __FILE__, __FUNC__, __LINE__));
		Result = mGet(cl, obj, (struct opGet *) msg);
		break;

	case MUIM_AskMinMax:
		d1(kprintf("%s/%s/%ld: MUIM_AskMinMax\n", __FILE__, __FUNC__, __LINE__));
		Result = mAskMinMax(cl, obj, (struct MUIP_AskMinMax *) msg);
		break;

	case MUIM_Draw:
		d1(kprintf("%s/%s/%ld: MUIM_Draw\n", __FILE__, __FUNC__, __LINE__));
		Result = mDraw(cl, obj, (struct MUIP_Draw *) msg);
		break;

	default:
		d1(KPrintF("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

// -------------------------------------------------------------------------

struct MUI_CustomClass *InitSelectMarkSampleClass(void)
{
	return MUI_CreateCustomClass(NULL,
		SUPERCLASS,
		NULL,
		sizeof(struct SelectMarkSampleInstance),
		DISPATCHER_REF(SelectMarkSampleClass));
}

void CleanupSelectMarkSampleClass(struct MUI_CustomClass *mcc)
{
	MUI_DeleteCustomClass(mcc);
}

// -------------------------------------------------------------------------

