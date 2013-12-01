// FrameImageClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/gfxmacros.h>
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
#include <proto/cybergraphics.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include "debug.h"
#include <proto/scalos.h>
#include <proto/scalosgfx.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "png.h"
#include "scalos_structures.h"
#include "FrameImageClass.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

struct FrameImageClassInst
	{
	ULONG bmicl_Recessed;
	ULONG bmicl_Width;
	ULONG bmicl_Height;
	ULONG bmicl_TopOffset;		// left offset in frame cell until frame starts
	ULONG bmicl_LeftOffset;		// top offset in frame cell until frame starts
	ULONG bmicl_FrameWidth;		// horizontal thickness of frame
	ULONG bmicl_FrameHeight;	// vertical thickness of frame

	struct ARGBHeader bmicl_argbh;

	struct Hook *bmicl_OutsideBackfillHook;
	struct Hook *bmicl_InsideBackfillHook;

	struct BitMap *bmicl_BackgroundBitMap;
	struct BitMap *bmicl_MaskBitMap;
	};

typedef LONG (*GetPenFunc_t)(struct RastPort *, WORD, WORD );

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT FrameImageClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG FrameImage_New(Class *cl, Object *o, Msg msg);
static ULONG FrameImage_Dispose(Class *cl, Object *o, Msg msg);
static ULONG FrameImage_Set(Class *cl, Object *o, Msg msg);
static ULONG FrameImage_Draw(Class *cl, Object *o, Msg msg);
static ULONG FrameImage_FrameBox(Class *cl, Object *o, Msg msg);
static void FindFrameBorders(struct FrameImageClassInst *inst);
static void GenerateMask(Class *cl, Object *o);
static void GenerateMaskTile(const struct ARGBHeader *Src, struct RastPort *rpMask,
	LONG MaskX, LONG MaskY,
	LONG SrcX, LONG SrcY,
	LONG Width, LONG Height);
static void GenerateBackground(Class *cl, Object *o);
static BOOL GetPngPicture(BPTR file, struct ARGBHeader *argbh);
static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size);
static void PngFreeMem(png_structp png_ptr, png_voidp ptr);
static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
static void PngError(png_structp png_ptr, png_const_charp error_message);
static void PngWarning(png_structp png_ptr, png_const_charp warning_message);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

struct ScalosClass *initFrameImageClass(const struct PluginClass *plug)
{
	struct ScalosClass *FrameImageClass;

	FrameImageClass	= SCA_MakeScalosClass(plug->plug_classname,
			plug->plug_superclassname,
			sizeof(struct FrameImageClassInst),
			0 );

	if (FrameImageClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(FrameImageClass->sccl_class->cl_Dispatcher, FrameImageClassDispatcher);
		}

	return FrameImageClass;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT FrameImageClassDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = FrameImage_New(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = FrameImage_Dispose(cl, o, msg);
		break;
	case OM_SET:
		Result = FrameImage_Set(cl, o, msg);
		break;
	case IM_DRAW:
		Result = FrameImage_Draw(cl, o, msg);
		break;
	case IM_FRAMEBOX:
		Result = FrameImage_FrameBox(cl, o, msg);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG FrameImage_New(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct FrameImageClassInst *inst;
	CONST_STRPTR ImageName;
	ULONG Width, Height;
	BOOL Success = FALSE;
	BPTR fd = BNULL;

	do	{
		o = (Object *) DoSuperMethodA(cl, o, msg);
		d1(KPrintF("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		if (NULL == o)
			break;

		if (NULL == CyberGfxBase)
			break;

		inst = INST_DATA(cl, o);

		memset(inst, 0, sizeof(struct FrameImageClassInst));

		inst->bmicl_Recessed = GetTagData(IA_Recessed, FALSE, ops->ops_AttrList);
		inst->bmicl_OutsideBackfillHook = (struct Hook *) GetTagData(FRAMEIMG_OuterBackfillHook, (ULONG) NULL, ops->ops_AttrList);
		inst->bmicl_InsideBackfillHook = (struct Hook *) GetTagData(FRAMEIMG_InnerBackfillHook, (ULONG) NULL, ops->ops_AttrList);

		ImageName = (CONST_STRPTR) GetTagData(FRAMEIMG_ImageName, (ULONG) "", ops->ops_AttrList);
		d1(KPrintF("%s/%s/%ld: o=%08lx  ImageName=<%s>\n", __FILE__, __FUNC__, __LINE__, o, ImageName));

		fd = Open(ImageName, MODE_OLDFILE);
		d1(KPrintF("%s/%s/%ld: o=%08lx  fd=%08lx\n", __FILE__, __FUNC__, __LINE__, o, fd));
		if (BNULL == fd)
			break;

		if (!GetPngPicture(fd, &inst->bmicl_argbh))
			break;

		d1(KPrintF("%s/%s/%ld: o=%08lx  GetPngPicture succeeded\n", __FILE__, __FUNC__, __LINE__, o));

	        // set dimensions in superclass
		Height = inst->bmicl_Height = inst->bmicl_argbh.argb_Height;
		Width = inst->bmicl_argbh.argb_Width / 3;

		// frame templates are always square
		inst->bmicl_Width = inst->bmicl_Height;

		d1(KPrintF("%s/%s/%ld: bmicl_Width=%ld  bmicl_Height=%ld\n", __FILE__, __FUNC__, __LINE__, inst->bmicl_Width, inst->bmicl_Height));

		// we need to find bmicl_LeftOffset and bmicl_TopOffset
		FindFrameBorders(inst);

		inst->bmicl_FrameWidth  = (inst->bmicl_Height) / 3 - inst->bmicl_LeftOffset;
		inst->bmicl_FrameHeight = (inst->bmicl_Width)  / 3 - inst->bmicl_TopOffset;

		d1(KPrintF("%s/%s/%ld: bmicl_FrameWidth=%ld  bmicl_FrameHeight=%ld\n", __FILE__, __FUNC__, __LINE__, inst->bmicl_FrameWidth, inst->bmicl_FrameHeight));

		// set initial width + height
		SetAttrs(o,
			IA_Width, Width,
			IA_Height, Height,
			TAG_END);

		Success = TRUE;
		} while (0);

	if (BNULL != fd)
		Close(fd);
#if 0
	if (Success)
		{
		ULONG x;
		ULONG y = inst->bmicl_Height / 2;
		ULONG *up = (ULONG *) inst->bmicl_argbh.argb_ImageData;

		up += y * inst->bmicl_argbh.argb_Width;
		for (x = 0; x < inst->bmicl_argbh.argb_Width; x++)
			{
			d1(KPrintF("%s/%s/%ld: x=%ld  ARGB=%08lx\n", __FILE__, __FUNC__, __LINE__, x, *up++));
			}
		}
#endif

	if (o && !Success)
		{
		DoMethod(o, OM_DISPOSE);
		o = NULL;
		}

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG FrameImage_Dispose(Class *cl, Object *o, Msg msg)
{
	struct FrameImageClassInst *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld: o=%08lx  \n", __FILE__, __FUNC__, __LINE__, o));

	ScalosGfxFreeARGB(&inst->bmicl_argbh.argb_ImageData);

	if (inst->bmicl_MaskBitMap)
		{
		FreeBitMap(inst->bmicl_MaskBitMap);
		inst->bmicl_MaskBitMap = NULL;
		}
	if (inst->bmicl_BackgroundBitMap)
		{
		FreeBitMap(inst->bmicl_BackgroundBitMap);
		inst->bmicl_BackgroundBitMap = NULL;
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG FrameImage_Set(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct FrameImageClassInst *inst = INST_DATA(cl, o);
	ULONG Result;

	inst->bmicl_Recessed = GetTagData(IA_Recessed, inst->bmicl_Recessed, ops->ops_AttrList);
	inst->bmicl_OutsideBackfillHook = (struct Hook *) GetTagData(FRAMEIMG_OuterBackfillHook, (ULONG) inst->bmicl_OutsideBackfillHook, ops->ops_AttrList);
	inst->bmicl_InsideBackfillHook = (struct Hook *) GetTagData(FRAMEIMG_InnerBackfillHook, (ULONG) inst->bmicl_InsideBackfillHook, ops->ops_AttrList);

	d1(KPrintF("%s/%s/%ld:  o=%08lx  bmicl_OutsideBackfillHook=%08lx  h_Entry=%08lx  h_SubEntry=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, o, inst->bmicl_OutsideBackfillHook, \
                inst->bmicl_OutsideBackfillHook->h_Entry, inst->bmicl_OutsideBackfillHook->h_SubEntry));

	Result = DoSuperMethodA(cl, o, msg);

	if (FindTagItem(IA_Width, ops->ops_AttrList)
		|| FindTagItem(IA_Height, ops->ops_AttrList))
		{
		GenerateBackground(cl, o);
		GenerateMask(cl, o);
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG FrameImage_Draw(Class *cl, Object *o, Msg msg)
{
	struct impDraw *imp = (struct impDraw *) msg;
	struct FrameImageClassInst *inst = INST_DATA(cl, o);
	struct Image *img = (struct Image *) o;
	struct BackFillMsg bfm;
	LONG x, y;

	d1(KPrintF("%s/%s/%ld: o=%08lx  Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, o, img->LeftEdge, img->TopEdge));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld  imp_State=%ld\n", __FILE__, __FUNC__, __LINE__, img->Width, img->Height, imp->imp_State));

	x = img->LeftEdge + imp->imp_Offset.X;
	y = img->TopEdge  + imp->imp_Offset.Y;

	bfm.bfm_Layer = imp->imp_RPort->Layer;

	if (inst->bmicl_argbh.argb_ImageData)
		{
		LONG dx, dy;
		ULONG wh = inst->bmicl_Width / 3;
		LONG w, h;
		LONG ImgLeft;

		ImgLeft	= inst->bmicl_Recessed ? inst->bmicl_Width : 0;
#if 1
		if (inst->bmicl_OutsideBackfillHook)
			{
			// erase area outside of frame

			// Clear top left corner
			bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = x;
			bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = y;
			bfm.bfm_Rect.MaxX += inst->bmicl_FrameWidth - 1;
			bfm.bfm_Rect.MaxY += inst->bmicl_FrameHeight - 1;
			d1(KPrintF("%s/%s/%ld: o=%08lx  bmicl_OutsideBackfillHook=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->bmicl_OutsideBackfillHook));
			CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);
			d1(KPrintF("%s/%s/%ld: o=%08lx  \n", __FILE__, __FUNC__, __LINE__, o));

			// Clear top right corner
			bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = x + img->Width - inst->bmicl_FrameWidth;
			bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = y;
			bfm.bfm_Rect.MaxX += inst->bmicl_FrameWidth - 1;
			bfm.bfm_Rect.MaxY += inst->bmicl_FrameHeight - 1;
			CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);

			// Clear bottom left corner
			bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = x;
			bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = y + img->Height - inst->bmicl_FrameHeight;
			bfm.bfm_Rect.MaxX += inst->bmicl_FrameWidth - 1;
			bfm.bfm_Rect.MaxY += inst->bmicl_FrameHeight - 1;
			CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);

			// Clear bottom right corner
			bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = x + img->Width - inst->bmicl_FrameWidth;
			bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = y + img->Height - inst->bmicl_FrameHeight;
			bfm.bfm_Rect.MaxX += inst->bmicl_FrameWidth - 1;
			bfm.bfm_Rect.MaxY += inst->bmicl_FrameHeight - 1;
			CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);

			for (dx = x + inst->bmicl_FrameWidth, w = img->Width - 2 * inst->bmicl_FrameWidth; w > 0 ; w -= wh)
				{
				LONG dw = wh;

				if (dw > w)
					dw = w;

				// Clear top border
				bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = dx;
				bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = y;
				bfm.bfm_Rect.MaxX += dw - 1;
				bfm.bfm_Rect.MaxY += inst->bmicl_FrameHeight - 1;
				CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);

				// Clear bottom border
				bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = dx;
				bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = y + img->Height - inst->bmicl_FrameHeight;
				bfm.bfm_Rect.MaxX += dw - 1;
				bfm.bfm_Rect.MaxY += inst->bmicl_FrameHeight - 1;
				CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);

			      dx += dw;
			      }

			for (dy = y + inst->bmicl_FrameHeight, h = img->Height - 2 * inst->bmicl_FrameHeight; h > 0; h -= wh)
				{
				LONG dh = wh;

				if (dh > h)
					dh = h;

				// Clear left border
				bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = x;
				bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = dy;
				bfm.bfm_Rect.MaxX += inst->bmicl_FrameWidth - 1;
				bfm.bfm_Rect.MaxY += dh - 1;
				CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);

				// Clear right border
				bfm.bfm_Rect.MinX = bfm.bfm_Rect.MaxX = bfm.bfm_OffsetX = x + img->Width - inst->bmicl_FrameWidth;
				bfm.bfm_Rect.MinY = bfm.bfm_Rect.MaxY = bfm.bfm_OffsetY = dy;
				bfm.bfm_Rect.MaxX += inst->bmicl_FrameWidth - 1;
				bfm.bfm_Rect.MaxY += dh - 1;
				CallHookPkt(inst->bmicl_OutsideBackfillHook, imp->imp_RPort, &bfm);

				dy += dh;
				}
			}

		if (inst->bmicl_BackgroundBitMap)
			{
			// erase area inside of frame
			BltMaskBitMapRastPort(inst->bmicl_BackgroundBitMap,
				0, 0,
				imp->imp_RPort,
				x, y,
				img->Width, img->Height,
				ABC | ABNC | ANBC,
				inst->bmicl_MaskBitMap->Planes[0]);
			}
#endif
		// Draw top left corner
		ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
			x,
			y,
			inst->bmicl_LeftOffset + ImgLeft,
			inst->bmicl_TopOffset,
			inst->bmicl_FrameWidth,
			inst->bmicl_FrameHeight);

		// Draw top right corner
		ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
			x + img->Width - inst->bmicl_FrameWidth,
			y,
			2 * wh + ImgLeft,
			inst->bmicl_TopOffset,
			inst->bmicl_FrameWidth,
			inst->bmicl_FrameHeight);

		// draw bottom left corner
		ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
			x,
			y + img->Height - inst->bmicl_FrameHeight,
			inst->bmicl_LeftOffset + ImgLeft,
			2 * wh,
			inst->bmicl_FrameWidth,
			inst->bmicl_FrameHeight);

		// draw bottom right corner
		ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
			x + img->Width - inst->bmicl_FrameWidth,
			y + img->Height - inst->bmicl_FrameHeight,
			2 * wh + ImgLeft,
			2 * wh,
			inst->bmicl_FrameWidth,
			inst->bmicl_FrameHeight);

		  for (dx = x + inst->bmicl_FrameWidth, w = img->Width - 2 * inst->bmicl_FrameWidth; w > 0 ; w -= wh)
			{
			LONG dw = wh;

			if (dw > w)
				dw = w;

			// draw top border
			ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
				dx,
				y,
				wh + ImgLeft,
				inst->bmicl_TopOffset,
				dw,
				inst->bmicl_FrameHeight);

			// draw bottom border
			ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
				dx,
				y + img->Height - inst->bmicl_FrameHeight,
				wh + ImgLeft,
				2 * wh,
				dw,
				inst->bmicl_FrameHeight);

			dx += dw;
			}

		for (dy = y + inst->bmicl_FrameHeight, h = img->Height - 2 * inst->bmicl_FrameHeight; h > 0; h -= wh)
			{
			LONG dh = wh;

			if (dh > h)
				dh = h;

			// draw left border
			ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
				x,
				dy,
				inst->bmicl_LeftOffset + ImgLeft,
				wh,
				inst->bmicl_FrameWidth,
				dh);

			// draw right border
			ScalosGfxBlitARGBAlpha(imp->imp_RPort, &inst->bmicl_argbh,
				x + img->Width - inst->bmicl_FrameWidth,
				dy,
				2 * wh + ImgLeft,
				wh,
				inst->bmicl_FrameWidth,
				dh);
			dy += dh;
			}
		}
 
	d1(KPrintF("%s/%s/%ld: \n",  __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG FrameImage_FrameBox(Class *cl, Object *o, Msg msg)
{
	struct impFrameBox *imp = (struct impFrameBox *) msg;
	struct FrameImageClassInst *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld: o=%08lx  Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, o));

	if (imp->imp_FrameFlags & FRAMEF_SPECIFY)
		{
		imp->imp_FrameBox->Left   = imp->imp_ContentsBox->Left +
                                    (imp->imp_ContentsBox->Width - imp->imp_FrameBox->Width) / 2;
		imp->imp_FrameBox->Top    = imp->imp_ContentsBox->Top +
                                    (imp->imp_ContentsBox->Height - imp->imp_FrameBox->Height) / 2;
		}
	else
		{
		imp->imp_FrameBox->Left   = imp->imp_ContentsBox->Left   - inst->bmicl_FrameWidth;
		imp->imp_FrameBox->Top    = imp->imp_ContentsBox->Top    - inst->bmicl_FrameHeight;
		imp->imp_FrameBox->Width  = imp->imp_ContentsBox->Width  + 2 * inst->bmicl_FrameWidth;
		imp->imp_FrameBox->Height = imp->imp_ContentsBox->Height + 2 * inst->bmicl_FrameHeight;
		}

	d1(KPrintF("%s/%s/%ld: \n",  __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

static void FindFrameBorders(struct FrameImageClassInst *inst)
{
	LONG x, y;
	ULONG wh = inst->bmicl_Width / 3;
	struct ARGB BgPen;

	inst->bmicl_TopOffset = 0;
	inst->bmicl_LeftOffset = 0;

	BgPen = *inst->bmicl_argbh.argb_ImageData;

	d1(KPrintF("%s/%s/%ld: BgPen=%08lx  Color(0,1)=%08lx\n", __FILE__, __FUNC__, __LINE__, BgPen, GetPenFunc(&rp, 0, 1)));

	for (y = 0; y < wh && 0 == inst->bmicl_TopOffset; y++)
		{
		for (x = 0; x < 2 * inst->bmicl_Width; x++)
			{
			const struct ARGB *pSrc;

			pSrc = inst->bmicl_argbh.argb_ImageData
				+ y * inst->bmicl_argbh.argb_Width
				+ x;

			d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  Pen=%08lx\n",  __FILE__, __FUNC__, __LINE__, x, y, *pSrc));
			if (0 != memcmp(&BgPen, pSrc, sizeof(struct ARGB)))
				{
				d1(KPrintF("%s/%s/%ld: y=%ld\n",  __FILE__, __FUNC__, __LINE__, y));
				inst->bmicl_TopOffset = y;
				break;
				}
			}
		}
	for (x = 0; x < wh && 0 == inst->bmicl_LeftOffset; x++)
		{
		for (y = 0; y < inst->bmicl_Height; y++)
			{
			const struct ARGB *pSrc;

			pSrc = inst->bmicl_argbh.argb_ImageData
				+ y * inst->bmicl_argbh.argb_Width
				+ x;

			d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  Pen=%08lx\n",  __FILE__, __FUNC__, __LINE__, x, y, *pSrc));

			if (0 != memcmp(&BgPen, pSrc, sizeof(struct ARGB)))
				{
				d1(KPrintF("%s/%s/%ld: x=%ld\n",  __FILE__, __FUNC__, __LINE__, x));
				inst->bmicl_LeftOffset = x;
				break;
				}
			}
		}

	d1(KPrintF("%s/%s/%ld: bmicl_LeftOffset=%ld  bmicl_TopOffset=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->bmicl_LeftOffset, inst->bmicl_TopOffset));
}

//----------------------------------------------------------------------------

static void GenerateMask(Class *cl, Object *o)
{
	struct FrameImageClassInst *inst = INST_DATA(cl, o);
	struct Image *img = (struct Image *) o;
	struct RastPort rpMask;

	d1(KPrintF("%s/%s/%ld: dti_BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->bmicl_FrameImage->dti_BitMap));

	InitRastPort(&rpMask);
	rpMask.BitMap =	inst->bmicl_MaskBitMap = AllocBitMap(img->Width, img->Height, 1, BMF_CLEAR, NULL);

	if (rpMask.BitMap)
		{
		LONG dx, dy;
		ULONG wh = inst->bmicl_Width / 3;
		LONG SrcX = 2 * inst->bmicl_Width;
		LONG w, h;

		d1(KPrintF("%s/%s/%ld: SrcX=%ld  wh=%ld\n",  __FILE__, __FUNC__, __LINE__, SrcX, wh));

		SetABPenDrMd(&rpMask, 1, 0, JAM1);

		RectFill(&rpMask, inst->bmicl_FrameWidth,
			inst->bmicl_FrameHeight,
			img->Width - inst->bmicl_FrameWidth - 1,
			img->Height - inst->bmicl_FrameHeight - 1);
 
		// Create Mask for top left corner
		GenerateMaskTile(&inst->bmicl_argbh,
			&rpMask,
			0, 0,
			SrcX + inst->bmicl_LeftOffset, inst->bmicl_TopOffset,
			inst->bmicl_FrameWidth, inst->bmicl_FrameHeight
                        );

		// Create Mask for top right corner
		GenerateMaskTile(&inst->bmicl_argbh,
			&rpMask,
			img->Width - inst->bmicl_FrameWidth, 0,
			SrcX + 2 * wh, inst->bmicl_TopOffset,
			inst->bmicl_FrameWidth, inst->bmicl_FrameHeight
			);

		// Create Mask for bottom left corner
		GenerateMaskTile(&inst->bmicl_argbh,
			&rpMask,
			0, img->Height - inst->bmicl_FrameHeight,
			SrcX + inst->bmicl_LeftOffset, 2 * wh,
			inst->bmicl_FrameWidth, inst->bmicl_FrameHeight
			);

		// Create Mask for bottom right corner
		GenerateMaskTile(&inst->bmicl_argbh,
			&rpMask,
			img->Width - inst->bmicl_FrameWidth, img->Height - inst->bmicl_FrameHeight,
			SrcX + 2 * wh, 2 * wh,
			inst->bmicl_FrameWidth, inst->bmicl_FrameHeight
			);

		  for (dx = inst->bmicl_FrameWidth, w = img->Width - 2 * inst->bmicl_FrameWidth; w > 0 ; w -= wh)
			{
			LONG dw = wh;

			if (dw > w)
				dw = w;

			// Create Mask for top border
			GenerateMaskTile(&inst->bmicl_argbh,
				&rpMask,
				dx, 0,
				SrcX + wh, inst->bmicl_TopOffset,
				dw, inst->bmicl_FrameHeight
				);

			// Create Mask for bottom border
			GenerateMaskTile(&inst->bmicl_argbh,
				&rpMask,
				dx, img->Height - inst->bmicl_FrameHeight,
				SrcX + wh, 2 * wh,
				dw, inst->bmicl_FrameHeight
				);

			dx += dw;
			}

		for (dy = inst->bmicl_FrameHeight, h = img->Height - 2 * inst->bmicl_FrameHeight; h > 0; h -= wh)
			{
			LONG dh = wh;

			if (dh > h)
				dh = h;

			// Create Mask for left border
			GenerateMaskTile(&inst->bmicl_argbh,
				&rpMask,
				0, dy,
				SrcX + inst->bmicl_LeftOffset, wh,
				inst->bmicl_FrameWidth, dh
				);

			// Create Mask for right border
			GenerateMaskTile(&inst->bmicl_argbh,
				&rpMask,
				img->Width - inst->bmicl_FrameWidth, dy,
				SrcX + 2 * wh, wh,
				inst->bmicl_FrameWidth, dh
				);
			dy += dh;
			}
		}
}

//----------------------------------------------------------------------------

static void GenerateMaskTile(const struct ARGBHeader *Src, struct RastPort *rpMask,
	LONG MaskX, LONG MaskY,
	LONG SrcX, LONG SrcY,
	LONG Width, LONG Height)
{
	LONG x, y;

	d1(KPrintF("%s/%s/%ld: START MaskX=%ld  MaskY=%ld\n",  __FILE__, __FUNC__, __LINE__, MaskX, MaskY));
	d1(KPrintF("%s/%s/%ld: SrcX=%ld  SrcY=%ld\n",  __FILE__, __FUNC__, __LINE__, SrcX, SrcY));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld  Depth=%ld\n",  __FILE__, __FUNC__, __LINE__, Width, Height, GetBitMapAttr(rpSrc->BitMap, BMA_DEPTH)));

	for (y = 0; y < Height; y++)
		{
		for (x = 0; x < Width; x++)
			{
			ULONG Pen;
			const struct ARGB *pSrc;

			pSrc = Src->argb_ImageData
				+ (y + SrcY) * Src->argb_Width
				+ (x + SrcX);

			Pen = *((ULONG *) pSrc) & 0x00f0f0f0;
			d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  Pen=%08lx\n",  __FILE__, __FUNC__, __LINE__, SrcX + x, SrcY + y, GetPenFunc(rpSrc, x + SrcX, y + SrcY)));
			if (0x00f0f0f0 != Pen)
				{
				d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  Pen=%08lx\n",  __FILE__, __FUNC__, __LINE__, MaskX + x, MaskY + y, Pen));
				WritePixel(rpMask, x + MaskX, y + MaskY);
				}
			}
		}
}

//----------------------------------------------------------------------------

static void GenerateBackground(Class *cl, Object *o)
{
	struct FrameImageClassInst *inst = INST_DATA(cl, o);
	struct Image *img = (struct Image *) o;

	if (inst->bmicl_BackgroundBitMap)
		{
		FreeBitMap(inst->bmicl_BackgroundBitMap);
		inst->bmicl_BackgroundBitMap = NULL;
		}

	if (inst->bmicl_InsideBackfillHook)
		{
		ULONG ScreenDepth;
		struct BackFillMsg bfm;

		ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

		inst->bmicl_BackgroundBitMap = AllocBitMap(img->Width, img->Height,
			ScreenDepth,
			BMF_MINPLANES,
			iInfos.xii_iinfos.ii_Screen->RastPort.BitMap);

		if (inst->bmicl_BackgroundBitMap)
			{
			struct RastPort rp;

			InitRastPort(&rp);
			rp.BitMap = inst->bmicl_BackgroundBitMap;

			bfm.bfm_OffsetX = img->LeftEdge;
			bfm.bfm_OffsetY = img->TopEdge;
			bfm.bfm_Rect.MinX = 0;
			bfm.bfm_Rect.MinY = 0;
			bfm.bfm_Rect.MaxX = img->Width - 1;
			bfm.bfm_Rect.MaxY = img->Height - 1;
			bfm.bfm_Layer = NULL,

			CallHookPkt(inst->bmicl_InsideBackfillHook, &rp, &bfm);
			}
		}
}

//----------------------------------------------------------------------------

// Given a PNG file buffer decompress the image data and returns an ARGB image array.
// Width and height are filled in with the image dimensions.
// Only 24bit image formats (with or without alpha channel) are supported by choice.
// Since the output buffer is intended for usage with ARGB alpha blit functions,
// if no alpha is present it will be filled with 255.


static BOOL GetPngPicture(BPTR file, struct ARGBHeader *argbh)
{
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	BOOL success = FALSE;


	d1(KPrintF("%s/%s/%ld:  file=%08lx  \n", __FILE__, __FUNC__, __LINE__, file));

	do	{
		png_bytepp row_pointers;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;
		struct ARGB *argb;
		ULONG BytesPerRow;
		ULONG y;

		png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
		      NULL, PngError, PngWarning, NULL, PngAllocMem, PngFreeMem);

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));
		if (NULL == png_ptr)
			break;

		/* Allocate/initialize the memory for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		d1(KPrintF("%s/%s/%ld:  info_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, info_ptr));
		if (NULL == info_ptr)
			break;

		// safety check against jmp_buf size mismatch between libpng and Scalos
		if (NULL == png_jmpbuf(png_ptr))
			break;

		if (setjmp(png_jmpbuf(png_ptr)))
			{
			d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));
			success = FALSE;
			break;
			}

		png_set_read_fn(png_ptr, (void *) file, user_read_data);

		png_set_keep_unknown_chunks(png_ptr,
			PNG_HANDLE_CHUNK_IF_SAFE, NULL, 0);

		png_set_palette_to_rgb(png_ptr);
		png_set_expand_gray_1_2_4_to_8(png_ptr);
		png_set_tRNS_to_alpha(png_ptr);
		png_set_gray_to_rgb(png_ptr);
		png_set_add_alpha(png_ptr, ~0, PNG_FILLER_BEFORE);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		png_read_png(png_ptr, info_ptr,
			PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING
                        | PNG_TRANSFORM_SWAP_ALPHA | PNG_TRANSFORM_EXPAND, NULL);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		png_get_IHDR(png_ptr, info_ptr,
			&width, &height, &bit_depth, &color_type,
			&interlace_type, NULL, NULL);

		d1(KPrintF("%s/%s/%ld:  width=%ld  height=%ld  depth=%ld color_type=%ld\n", __FILE__, __FUNC__, __LINE__, width, height, bit_depth, color_type));

		row_pointers = png_get_rows(png_ptr, info_ptr);
		d1(KPrintF("%s/%s/%ld:  row_pointers=%08lx\n", __FILE__, __FUNC__, __LINE__, row_pointers));
		if (NULL == row_pointers)
			break;

		argbh->argb_Width = width;
		argbh->argb_Height = height;
		argbh->argb_ImageData = ScalosGfxCreateARGB(width, height, NULL);

		d1(KPrintF("%s/%s/%ld:  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, argbh->argb_ImageData));
		if (NULL == argbh->argb_ImageData)
			break;

		d1(KPrintF("%s/%s/%ld:  width=%ld  height=%ld  depth=%ld color_type=%ld\n", __FILE__, __FUNC__, __LINE__, width, height, bit_depth, color_type));

		BytesPerRow = png_get_rowbytes(png_ptr, info_ptr);

		d1(KPrintF("%s/%s/%ld:  BytesPerRow=%lu\n", __FILE__, __FUNC__, __LINE__, BytesPerRow));

		argb = argbh->argb_ImageData;
		for (y = 0; y < height; y++)
			{
			memcpy(argb, row_pointers[y], BytesPerRow);
			argb += width;
			}

		success = TRUE;
	} while (0);

	d1(KPrintF("%s/%s/%ld:  success=%ld\n", __FILE__, __FUNC__, __LINE__, success));

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return success;
}


static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size)
{
	(void) png_ptr;

	return ScalosAlloc(size);
}

static void PngFreeMem(png_structp png_ptr, png_voidp ptr)
{
	(void) png_ptr;
	ScalosFree(ptr);
}


static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	BPTR fh = (BPTR) png_get_io_ptr(png_ptr);

	if (length != Read(fh, data, length))
		{
//		  png_error();
		}
}


void abort(void)
{
	while (1)
		;
}

static void PngError(png_structp png_ptr, png_const_charp error_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, error_message));

	longjmp(png_jmpbuf(png_ptr), 1);
}


static void PngWarning(png_structp png_ptr, png_const_charp warning_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, warning_message));
//	png_default_warning(png_ptr, warning_message);
}

