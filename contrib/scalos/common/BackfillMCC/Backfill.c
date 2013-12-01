// Backfill.c
// $Date$
// $Revision$

#ifdef __AROS__
#define MUIMASTER_YES_INLINE_STDARG
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <libraries/mui.h>
#include <datatypes/pictureclass.h>
#include <utility/utility.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>

#include <clib/alib_protos.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <scalos/pattern.h>

#include <defs.h>

#include "Backfill.h"
#include "BitMapMCC.h"
#include "debug.h"

//----------------------------------------------------------------------------

#define	max(x, y)	((x) > (y) ? (x) : (y))
#define	min(x, y)	((x) > (y) ? (y) : (x))

//----------------------------------------------------------------------------

struct BackFillInst
	{
	Object *bfi_BitMapObject;
	UWORD bfi_Width;
	UWORD bfi_Height;
	BOOL bfi_GenericBitMapObject;
	};

//----------------------------------------------------------------------------

struct MUI_CustomClass *BackfillClass;
static ULONG Signature = 0x4711;

//----------------------------------------------------------------------------

DISPATCHER_PROTO(Backfill);
static ULONG BackFillNew(Class *cl, Object *o, Msg msg);
static ULONG BackFillDispose(Class *cl, Object *o, Msg msg);
static ULONG BackFillClassAskMinMax(Class *cl, Object *o, Msg msg);
static ULONG BackFillSet(Class *cl, Object *o, Msg msg);
static ULONG BackFillDraw(Class *cl, Object *o, Msg msg);
static void CloneBitMapObject(Class *cl, Object *o, Object *NewBitMapObj);
static void ForceRelayout(struct IClass *cl, Object *obj);

//----------------------------------------------------------------------------

DISPATCHER(Backfill)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = BackFillNew(cl, obj, msg);
		break;

	case OM_DISPOSE:
		Result = BackFillDispose(cl, obj, msg);
		break;

	case OM_SET:
		Result = BackFillSet(cl, obj, msg);
		break;

	case MUIM_Draw:
		Result = BackFillDraw(cl, obj, msg);
		break;

	case MUIM_AskMinMax:
		Result = BackFillClassAskMinMax(cl, obj, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

//----------------------------------------------------------------------------

static ULONG BackFillNew(Class *cl, Object *o, Msg msg)
{
	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (o)
		{
		struct opSet *ops = (struct opSet *) msg;
//		struct BackFillInst *inst = INST_DATA(cl, o);
		Object *NewBitMapObj;

		set(o, MUIA_FillArea, TRUE);

		NewBitMapObj = (Object *) GetTagData(BFA_BitmapObject, (ULONG) NULL, ops->ops_AttrList);
		CloneBitMapObject(cl, o, NewBitMapObj);
		}

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG BackFillDispose(Class *cl, Object *o, Msg msg)
{
//	  struct BackFillInst *inst = INST_DATA(cl, o);

//	  if (inst->bfi_BitMapObject)
//		  {
//		  MUI_DisposeObject(inst->bfi_BitMapObject);
//		  inst->bfi_BitMapObject = NULL;
//		  }

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG BackFillClassAskMinMax(Class *cl, Object *o, Msg msg)
{
	struct MUIP_AskMinMax *amm = (struct MUIP_AskMinMax *) msg;
	struct BackFillInst *inst = INST_DATA(cl, o);
	UWORD Size;
	ULONG Result;

	Result = DoSuperMethodA(cl, o, msg);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: o=%08lx\n", __LINE__, o));

	// our BitMap images are always square
	Size = max(inst->bfi_Width, inst->bfi_Height);

	amm->MinMaxInfo->MinWidth = Size + _subwidth(o);
	amm->MinMaxInfo->MinHeight = Size + _subheight(o);
	amm->MinMaxInfo->MaxWidth = Size + _subwidth(o);
	amm->MinMaxInfo->MaxHeight = Size + _subheight(o);
	amm->MinMaxInfo->DefWidth = Size + _subwidth(o);
	amm->MinMaxInfo->DefHeight = Size + _subheight(o);

	return Result;
}

//----------------------------------------------------------------------------

static ULONG BackFillSet(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct BackFillInst *inst = INST_DATA(cl, o);
	BOOL DoRedraw = FALSE;
	BOOL DoRelayout = FALSE;
	ULONG Result;

	if (FindTagItem(BFA_BitmapObject, ops->ops_AttrList))
		{
		Object *NewBitMapObj;

		NewBitMapObj = (Object *) GetTagData(BFA_BitmapObject, (ULONG) inst->bfi_BitMapObject, ops->ops_AttrList);

		CloneBitMapObject(cl, o, NewBitMapObj);
		DoRedraw = TRUE;
		}

	Result = DoSuperMethodA(cl, o, msg);

	if (DoRelayout)
		ForceRelayout(cl, o);
	else if (DoRedraw)
		MUI_Redraw(o, MADF_DRAWOBJECT);

	return Result;
}

//----------------------------------------------------------------------------

static ULONG BackFillDraw(Class *cl, Object *o, Msg msg)
{
	struct BackFillInst *inst = INST_DATA(cl, o);
	struct MUIP_Draw *mDraw = (struct MUIP_Draw *) msg;
	ULONG ShowMe = 0;

	get(o, MUIA_ShowMe, &ShowMe);

	if (!ShowMe)
		return 0;

	DoSuperMethodA(cl, o, msg);

	if (mDraw->flags & MADF_DRAWOBJECT)
		{
		if (inst->bfi_BitMapObject && inst->bfi_GenericBitMapObject)
			{
			struct BitMap *bm = NULL;

			get(inst->bfi_BitMapObject, MUIA_Bitmap_RemappedBitmap, &bm);

			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: BitMapObject=%08lx  BitMap=%08lx\n", \
				__LINE__, inst->bfi_BitMapObject, inst->bfi_BitMap));

			if (bm)
				{
				WORD Width, Height;
				WORD MinX, MinY;
				UWORD SizeX, SizeY;

				MinX   =   _left(o) +   _addleft(o);
				MinY   =    _top(o) +    _addtop(o);
				Width  =  _width(o) -  _subwidth(o);
				Height = _height(o) - _subheight(o);

				SizeX = min(Width, inst->bfi_Width);
				SizeY = min(Height, inst->bfi_Height);

				MinX += (Width - inst->bfi_Width) / 2;
				MinY += (Height - inst->bfi_Height) / 2;

				BltBitMapRastPort(bm,
					0, 0,
					_rp(o),
					MinX, MinY,
					SizeX, SizeY,
					0xC0);
				}
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static void CloneBitMapObject(Class *cl, Object *o, Object *NewBitMapObj)
{
	struct BackFillInst *inst = INST_DATA(cl, o);

	if (inst->bfi_BitMapObject)
		{
		// delete existing bfi_BitMapObject
		DoMethod(o, MUIM_Group_InitChange);
		DoMethod(o, OM_REMMEMBER, inst->bfi_BitMapObject);
		DoMethod(o, MUIM_Group_ExitChange);

		MUI_DisposeObject(inst->bfi_BitMapObject);
		inst->bfi_BitMapObject = NULL;
		}

	if (NewBitMapObj)
		{
		ULONG Width = 0;

		// check whether this is a BitmapObject by querying MUIA_Bitmap_Width
		if (get(NewBitMapObj, MUIA_Bitmap_Width, &Width))
			{
			ULONG Height = 0;
			struct BitMap *NewBitMap = NULL;
			ULONG *SourceColors = NULL;

			inst->bfi_GenericBitMapObject = TRUE;

			get(NewBitMapObj, MUIA_Bitmap_Width, &Width);
			get(NewBitMapObj, MUIA_Bitmap_Height, &Height);
			get(NewBitMapObj, MUIA_Bitmap_Bitmap, &NewBitMap);
			get(NewBitMapObj, MUIA_Bitmap_SourceColors, &SourceColors);

			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: BitmapObject BitMap=%08lx  SourceColors=%08lx\n", \
				__LINE__, NewBitMap, SourceColors));

			// Remember image dimensions
			inst->bfi_Width = Width;
			inst->bfi_Height = Height;

			inst->bfi_BitMapObject = BitmapObject,
				MUIA_Bitmap_Width, Width,
				MUIA_Bitmap_Height, Height,
				MUIA_Bitmap_Bitmap, (IPTR) NewBitMap,
				MUIA_Bitmap_SourceColors, (IPTR) SourceColors,
				MUIA_Bitmap_UseFriend, TRUE,
				MUIA_Bitmap_Precision, PRECISION_ICON,
				End;
			}
		else
			{
			// this is a BitMapPicClass object
			struct BitMap *bm = NULL;
			ULONG *ColorTable = NULL;
			UBYTE *BitMapArray = NULL;
			struct Screen *screen = NULL;
			ULONG Height = 0;

			inst->bfi_GenericBitMapObject = FALSE;

			get(NewBitMapObj, MUIA_ScaBitMappic_BitMap, &bm);
			get(NewBitMapObj, MUIA_ScaBitMappic_ColorTable, &ColorTable);
			get(NewBitMapObj, MUIA_ScaBitMappic_Width, &Width);
			get(NewBitMapObj, MUIA_ScaBitMappic_Height, &Height);
			get(NewBitMapObj, MUIA_ScaBitmappic_BitMapArray, &BitMapArray);
			get(NewBitMapObj, MUIA_ScaBitMappic_Screen, &screen);

			inst->bfi_BitMapObject = BitMapPicObject,
				MUIA_ScaBitMappic_BitMap, (IPTR) bm,
				MUIA_ScaBitMappic_ColorTable, (IPTR) ColorTable,
				MUIA_ScaBitMappic_Width, Width,
				MUIA_ScaBitMappic_Height, Height,
				MUIA_ScaBitMappic_Screen, (IPTR) screen,
				MUIA_ScaBitmappic_BitMapArray, (IPTR) BitMapArray,
				End;

			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: bfi_BitMapObject=%08lx\n", __LINE__, inst->bfi_BitMapObject));
			}
		}

	if (inst->bfi_BitMapObject)
		{
		DoMethod(o, MUIM_Group_InitChange);
		DoMethod(o, OM_ADDMEMBER, inst->bfi_BitMapObject);
		DoMethod(o, MUIM_Group_ExitChange);
		}
}

//----------------------------------------------------------------------------

struct MUI_CustomClass *InitBackfillClass(void)
{
	if (0x4711 == Signature++)
		{
		BackfillClass = MUI_CreateCustomClass(NULL, MUIC_Group, NULL,
			sizeof(struct BackFillInst), DISPATCHER_REF(Backfill));
		}

	return BackfillClass;
}

void CleanupBackfillClass(void)
{
	if (BackfillClass)
		{
		MUI_DeleteCustomClass(BackfillClass);
		BackfillClass = NULL;
		Signature = 0x4711;
		}
}

//----------------------------------------------------------------------------

static void ForceRelayout(struct IClass *cl, Object *obj)
{
	Object *WindowObj;
	Object *RootObj = NULL;

	if (muiRenderInfo(obj))
		{
		WindowObj = _win(obj);
		get(WindowObj, MUIA_Window_RootObject, &RootObj);

		if (RootObj)
			{
			// force relayout
			DoMethod(RootObj, MUIM_Group_InitChange);
			DoMethod(RootObj, MUIM_Group_ExitChange);
			}
		}
}

//----------------------------------------------------------------------------

