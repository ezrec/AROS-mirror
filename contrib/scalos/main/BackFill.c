// BackFill.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/graphics.h>
#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

static void TextWindowStripeFill(struct RastPort *rp,
	struct BackFillMsg *msg, struct internalScaWindowTask *iwt,
	LONG XOffset, LONG YOffset);

//----------------------------------------------------------------------------

// object == (struct RastPort *) result->RastPort
// message == [ (Layer *) layer, (struct Rectangle) bounds,
//              (LONG) offsetx, (LONG) offsety ]

SAVEDS(ULONG) BackFillHookFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg)
{
	struct RastPort rpCopy;
	struct BackFillMsg msgCopy;
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) bfHook->h_Data;
	struct PatternInfo *ptInfo = &iwt->iwt_WindowTask.wt_PatternInfo;

	d1(KPrintF("START " "%s/%s/%ld: ptinfo=bfHook=%08lx  rp=%08lx  msg=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, bfHook, rp, msg));

	d1(KPrintF("%s/%s/%ld: RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));
	d1(KPrintF("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
	d1(KPrintF("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY));

	d1(KPrintF("%s/%s/%ld: ptinf_bitmap=%08lx\n", __FILE__, __FUNC__, __LINE__, ptInfo->ptinf_bitmap));
	if (ptInfo->ptinf_bitmap)
		d1(KPrintF("\n" "%s/%s/%ld: ptinf_bitmap  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
			GetBitMapAttr(ptInfo->ptinf_bitmap, BMA_WIDTH), GetBitMapAttr(ptInfo->ptinf_bitmap, BMA_HEIGHT)));

	d1(KPrintF("%s/%s/%ld: rp->bitmap=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		rp->BitMap, GetBitMapAttr(rp->BitMap, BMA_WIDTH), GetBitMapAttr(rp->BitMap, BMA_HEIGHT)));

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	rpCopy = *rp;
	rpCopy.Layer = NULL;

	// avoid erasing parts of the window frame
	msgCopy = *msg;

	d1(KPrintF("%s/%s/%ld: wt_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

#if defined(__MORPHOS__)
	if (iInfos.xii_Layers3D && iwt->iwt_WindowTask.wt_Window)
		{
		LONG xOffset = iwt->iwt_WindowTask.wt_Window->LeftEdge % 16;

		if (msgCopy.bfm_Rect.MinX < iwt->iwt_InnerLeft + xOffset)
			{
			d1(KPrintF("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
				msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
			d1(KPrintF("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld  LeftEdge=%d\n", __FILE__, __FUNC__, __LINE__, \
				msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY, iwt->iwt_WindowTask.wt_Window->LeftEdge % 16));

			msgCopy.bfm_Rect.MinX = iwt->iwt_InnerLeft + xOffset;
			}
		if (msgCopy.bfm_Rect.MinY < iwt->iwt_InnerTop)
			{
			d1(KPrintF("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
				msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
			msgCopy.bfm_Rect.MinY = iwt->iwt_InnerTop;
			}
		if (msgCopy.bfm_Rect.MaxX > (iwt->iwt_InnerLeft + iwt->iwt_InnerWidth + xOffset - 1))
			{
			d1(KPrintF("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
				msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
			d1(KPrintF("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
				msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY));

			msgCopy.bfm_Rect.MaxX = iwt->iwt_InnerLeft + iwt->iwt_InnerWidth + xOffset - 1;
			d1(KPrintF("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
				msgCopy.bfm_Rect.MinX, msgCopy.bfm_Rect.MinY, msgCopy.bfm_Rect.MaxX, msgCopy.bfm_Rect.MaxY));
			}
		if (msgCopy.bfm_Rect.MaxY > (iwt->iwt_InnerTop + iwt->iwt_InnerHeight - 1))
			{
			d1(KPrintF("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
				msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
			msgCopy.bfm_Rect.MaxY = iwt->iwt_InnerTop + iwt->iwt_InnerHeight - 1;
			}
		}
#endif /* __MORPHOS__ */
	
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	WindowBackFill(&rpCopy, &msgCopy, ptInfo->ptinf_bitmap,
		ptInfo->ptinf_width, ptInfo->ptinf_height,
		iwt->iwt_WinDrawInfo->dri_Pens[BACKGROUNDPEN],
		iwt->iwt_WindowTask.wt_XOffset, iwt->iwt_WindowTask.wt_YOffset,
		NULL);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (!IsIwtViewByIcon(iwt) && CurrentPrefs.pref_TextWindowStriped)
		{
		TextWindowStripeFill(&rpCopy, &msgCopy, iwt,
			iwt->iwt_WindowTask.wt_XOffset,
			iwt->iwt_WindowTask.wt_YOffset);
		}

	d1(KPrintF("\n " "%s/%s/%ld: END finished\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


void WindowBackFill(struct RastPort *rp,
	struct BackFillMsg *msg, struct BitMap *bitmap,
	LONG bmWidth, LONG bmHeight, WORD BGPen,
	LONG XOffset, LONG YOffset, APTR MaskPlane)
{
	d1(KPrintF("%s/%s/%ld: START  bitmap=%08lx\n", __FILE__, __FUNC__, __LINE__, bitmap));

	if (bitmap)
		{
		LONG x, y;
		LONG SrcX, SrcY;
		LONG h;
		LONG msgOffsetX, msgOffsetY;

		d1(kprintf("%s/%s/%ld: XOffset=%ld  YOffset=%ld\n", \
			__FILE__, __FUNC__, __LINE__, XOffset, YOffset));

#if defined(__AROS__)
		msgOffsetX = (msg->bfm_Rect.MinX - msg->bfm_Layer->bounds.MinX) % bmWidth;
		msgOffsetY = (msg->bfm_Rect.MinY - msg->bfm_Layer->bounds.MinY) % bmHeight;
#else
		msgOffsetX = msg->bfm_OffsetX % bmWidth;
		msgOffsetY = msg->bfm_OffsetY % bmHeight;
#endif

		if (YOffset < 0)
			SrcY = bmHeight - (-YOffset % bmHeight);
		else
			SrcY = YOffset;

		SrcY += msgOffsetY;
		SrcY %= bmHeight;

		h = bmHeight - SrcY;

		d1(kprintf("%s/%s/%ld: SrcY=%ld\n", __FILE__, __FUNC__, __LINE__, SrcY));

		d1(kprintf("%s/%s/%ld: MinX=%ld MinY=%ld  MaxX=%ld MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, \
			msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));

		for (y = msg->bfm_Rect.MinY; y <= msg->bfm_Rect.MaxY; )
			{
			LONG w;

			if (XOffset < 0)
				SrcX = bmWidth - (-XOffset % bmWidth);
			else
				SrcX = XOffset;

			SrcX += msgOffsetX;
			SrcX %= bmWidth;

			w = bmWidth - SrcX;

			if ((y + h) > msg->bfm_Rect.MaxY)
				h = 1 + msg->bfm_Rect.MaxY - y;

			d1(kprintf("%s/%s/%ld: SrcX=%ld  y=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, SrcX, y, h));

			for (x = msg->bfm_Rect.MinX; x <= msg->bfm_Rect.MaxX; )
				{
				if (x + w > msg->bfm_Rect.MaxX)
					w = 1 + msg->bfm_Rect.MaxX - x;

				d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, w, h));

				if (rp->Layer)
					{
					if (MaskPlane)
						{
						BltMaskBitMapRastPort(bitmap,
							SrcX, SrcY,
							rp, x, y,
							w, h,
							ABC | ABNC | ANBC,
							MaskPlane);
						}
					else
						{
						BltBitMapRastPort(bitmap,
							SrcX, SrcY,
							rp, x, y,
							w, h,
							ABC | ABNC);
						}
					}
				else
					{
					BltBitMap(bitmap,
						SrcX, SrcY,
						rp->BitMap, x, y,
						w, h,
						ABC | ABNC,
						~0,
						NULL
						);
					}

				x += w;
				w = bmWidth;
				SrcX = 0;
				}

			y += h;
			h = bmHeight;
			SrcY = 0;
			}
		}
	else
		{
		SetAPen(rp, BGPen);
		SetDrMd(rp, JAM1);

		RectFill(rp, msg->bfm_Rect.MinX, msg->bfm_Rect.MinY,
			msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY);
		}

	d1(KPrintF("\n " "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void TextWindowStripeFill(struct RastPort *rp,
	struct BackFillMsg *msg, struct internalScaWindowTask *iwt,
	LONG XOffset, LONG YOffset)
{
	if (msg->bfm_Rect.MinY <= msg->bfm_Rect.MaxY)
		{
		struct ScaIconNode *in = iwt->iwt_WindowTask.wt_IconList;
		ULONG LineHeight;

		if (in)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			LineHeight = gg->Height;
			}
		else
			{
			LineHeight = iwt->iwt_TextWindowLineHeight;
			}

		d1(kprintf("\n"));
		d1(kprintf("%s/%s/%ld: LineHeight=%ld  MinY=%ld  MaxY=%ld  YOffset=%ld iwt_InnerTop=%ld\n", \
			__FILE__, __FUNC__, __LINE__, LineHeight, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxY, \
			YOffset, iwt->iwt_InnerTop));
		d1(kprintf("%s/%s/%ld: bfm_OffsetX=%ld  bfm_OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
			msg->bfm_OffsetX, msg->bfm_OffsetY));

		if (LineHeight > 0)
			{
			LONG yMin;
			ULONG Depth = GetBitMapAttr(rp->BitMap, BMA_DEPTH);
			ULONG Ordinal = (msg->bfm_Rect.MinY + YOffset - iwt->iwt_InnerTop) / LineHeight;

			yMin = Ordinal * LineHeight - YOffset + iwt->iwt_InnerTop;

			while (yMin <= msg->bfm_Rect.MaxY)
				{
				LONG yMax = yMin + LineHeight - 1;

				if (yMin < msg->bfm_Rect.MinY)
					yMin = msg->bfm_Rect.MinY;
				if (yMax > msg->bfm_Rect.MaxY)
					yMax = msg->bfm_Rect.MaxY;

				if (Ordinal & 1)
					{
					d1(kprintf("%s/%s/%ld: Ordinal=%ld  yMin=%ld  yMax=%ld  Depth=%ld\n", __FILE__, __FUNC__, __LINE__, Ordinal, yMin, yMax, Depth));

					if (NULL == CyberGfxBase || Depth <= 8)
						{
						EraseRect(rp,
							msg->bfm_Rect.MinX,
							yMin,
							msg->bfm_Rect.MaxX,
							yMax);
						}
					else
						{
						static const struct ARGB Numerator = { (UBYTE) ~0, 70, 70, 70 };
						static const struct ARGB Denominator =  { (UBYTE) ~0, 80, 80, 80 };

						ScalosGfxARGBRectMult(rp, &Numerator, &Denominator,
							msg->bfm_Rect.MinX,
							yMin,
							msg->bfm_Rect.MaxX,
							yMax);
						}
					}
				Ordinal++;
				yMin = Ordinal * LineHeight - YOffset + iwt->iwt_InnerTop;
				}
			}
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

