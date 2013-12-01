// DrawIcon.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfxmacros.h>
#include <graphics/rpattr.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

//----------------------------------------------------------------------------

// local functions

static void DrawTransparentIconObject(struct internalScaWindowTask *iwt, Object *IconObj, ULONG Transparency);
static void DrawTransparentIconObjectK(struct internalScaWindowTask *iwt, Object *IconObj, const struct ARGB *K);
static BOOL IconsOverlap(const struct ScaIconNode *in1, const struct ScaIconNode *in2);
static void DrawIconObjectDisabled(struct internalScaWindowTask *iwt, Object *IconObj);
static void DrawIconObjectOverlay(struct internalScaWindowTask *iwt, Object *IconObj);

//----------------------------------------------------------------------------

void EraseIconObject(struct internalScaWindowTask *iwt, Object *IconObj)
{
	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	// Skip icons which lie completely outside of visible window range
	if (!IsIconObjVisible(iwt, IconObj))
		return;

	DoMethod(IconObj,
		IDTM_Erase,
		iwt->iwt_WinScreen,
		iwt->iwt_WindowTask.wt_Window,
		iwt->iwt_WindowTask.wt_Window->RPort,
		iwt->iwt_WinDrawInfo,
		iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
		iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset,
		0L);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

void DrawIconObject(struct internalScaWindowTask *iwt, Object *IconObj, ULONG Flags)
{
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;
	ULONG IconUserFlags = 0L;
	ULONG IconOverlayType = ICONOVERLAY_None;
	LONG BoundsLeft, BoundsRight, BoundsTop, BoundsBottom;
	ULONG ScreenDepth;

	d1(KPrintF("%s/%s/%ld: START IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	BoundsLeft = gg->BoundsLeftEdge + iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset;
	BoundsTop  = gg->BoundsTopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset;
	BoundsRight = BoundsLeft + gg->BoundsWidth - 1;
	BoundsBottom = BoundsTop + gg->BoundsHeight - 1;

	d1(kprintf("%s/%s/%ld: IconObj=%08lx  left=%ld top=%ld right=%ld bottom=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IconObj, BoundsLeft, BoundsTop, BoundsRight, BoundsBottom));

	// Skip icons which lie completely outside of visible window range
	if (BoundsRight < 0 || BoundsBottom < 0)
		return;
	if (BoundsLeft - iwt->iwt_InnerLeft > iwt->iwt_InnerWidth
		|| BoundsTop - iwt->iwt_InnerTop > iwt->iwt_InnerHeight)
		return;

	ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	GetAttr(IDTA_UserFlags, IconObj, &IconUserFlags);
	GetAttr(IDTA_OverlayType, IconObj, &IconOverlayType);

	d1(KPrintF("%s/%s/%ld: IconUserFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, IconUserFlags));
	d1(KPrintF("%s/%s/%ld: IconOverlayType=%lu\n", __FILE__, __FUNC__, __LINE__, IconOverlayType));

	if (IconUserFlags & ICONOBJ_USERFLAGF_DrawHighlite)
		{
		// Draw IconObj in highlighted state
		d1(KPrintF("%s/%s/%ld: Draw in highlighted state IconObj=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, IconObj));

		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
			BoundsLeft, BoundsTop,
			BoundsRight, BoundsBottom);

		DrawTransparentIconObjectK(iwt, IconObj, &CurrentPrefs.pref_IconHighlightK);
		}
	else if (IconUserFlags & ICONOBJ_USERFLAGF_DrawShadowed)
		{
		// Draw IconObj in shadowed state
		d1(KPrintF("%s/%s/%ld: Draw in shadowed state IconObj=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, IconObj));

		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
			BoundsLeft, BoundsTop,
			BoundsRight, BoundsBottom);

		DrawTransparentIconObject(iwt, IconObj, CurrentPrefs.pref_IconShadowTransparency);
		}
	else
		{
		// Draw IconObj in regular state (selected or normal)

		d1(KPrintF("%s/%s/%ld: Draw in regular state IconObj=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, IconObj));

		if (CyberGfxBase && (ScreenDepth > 8) 
			&& (CurrentPrefs.pref_DefaultIconTransparency > 0)
			&& (CurrentPrefs.pref_DefaultIconTransparency < 100)
			&& IsIwtViewByIcon(iwt)
			&& (IconUserFlags & ICONOBJ_USERFLAGF_DefaultIcon)
			&& !(IconUserFlags & ICONOBJ_USERFLAGF_Thumbnail))
			{
			// Draw default icon
			d1(KPrintF("%s/%s/%ld: draw via DrawTransparentIconObject\n", __FILE__, __FUNC__, __LINE__));

			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
				BoundsLeft, BoundsTop,
				BoundsRight, BoundsBottom);

			DrawTransparentIconObject(iwt, IconObj, CurrentPrefs.pref_DefaultIconTransparency);
			}
		else
			{
			d1(KPrintF("%s/%s/%ld: draw via IDTM_Draw\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(IconObj,
				IDTM_Draw,
				iwt->iwt_WinScreen,
				iwt->iwt_WindowTask.wt_Window,
				iwt->iwt_WindowTask.wt_Window->RPort,
				iwt->iwt_WinDrawInfo,
				iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
				iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset,
				Flags);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}
		}
	if (ICONOVERLAY_None != IconOverlayType)
		{
		// Draw IconOverlay
		d1(KPrintF("%s/%s/%ld: Draw IconOverlay\n", __FILE__, __FUNC__, __LINE__));
		DrawIconObjectOverlay(iwt, IconObj);
		}
	if (gg->Flags & GFLG_DISABLED)
		{
		// Draw IconObj in ghosted state
		d1(KPrintF("%s/%s/%ld: GFLG_DISABLED\n", __FILE__, __FUNC__, __LINE__));
		DrawIconObjectDisabled(iwt, IconObj);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void DrawIconObjectTransparent(struct internalScaWindowTask *iwt, Object *IconObj,
	ULONG ScreenDepth, struct RastPort *rp, struct RastPort *rpMask,
	ULONG transparency, BOOL RealTransparency)
{
	LONG bmWidth;
	ULONG bmFlags, bmDepth;
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;
	struct BitMap *allocBM;
	struct BitMap *maskBM;
	BOOL DrawGhosted = TRUE;
	WORD *WidthArray = NULL;

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	if (!IsIconObjVisible(iwt, IconObj))
		{
		d1(kprintf("%s/%s/%ld: IconObj=%08lx invisible\n", __FILE__, __FUNC__, __LINE__, IconObj));
		return;
		}

	// Check whether we have a text icon
	GetAttr(TIDTA_WidthArray, IconObj, (APTR) &WidthArray);

	SetABPenDrMd(rp, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawModeSel);
	SetABPenDrMd(rpMask, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, JAM1);

	rp->BitMap = allocBM = AllocBitMap(gg->BoundsWidth, gg->BoundsHeight,
		ScreenDepth,
		BMF_MINPLANES | BMF_CLEAR,
		iwt->iwt_WinScreen->RastPort.BitMap);
	if (NULL == rp->BitMap)
		return;

	d1(KPrintF("%s/%s/%ld: BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, allocBM));

	if (WidthArray)
		{
		// Draw text icon
		DoMethod(IconObj, IDTM_Draw,
			iwt->iwt_WinScreen,
			NULL,
			rp,
			iwt->iwt_WinDrawInfo,
			gg->LeftEdge - gg->BoundsLeftEdge,
			gg->TopEdge - gg->BoundsTopEdge,
			IODRAWF_AbsolutePos | IODRAWF_NoAlpha);
		}
	else
		{
		// Draw icon (image only)
		DoMethod(IconObj, IDTM_Draw,
			iwt->iwt_WinScreen,
			NULL,
			rp,
			iwt->iwt_WinDrawInfo,
			gg->LeftEdge - gg->BoundsLeftEdge,
			gg->TopEdge - gg->BoundsTopEdge,
			IODRAWF_NoText | IODRAWF_AbsolutePos | IODRAWF_NoAlpha);
		}

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx  CyberGfxBase=%08lx  RealTransparency=%ld  ScreenDepth=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IconObj, CyberGfxBase, RealTransparency, ScreenDepth));

	if (CyberGfxBase && RealTransparency && ScreenDepth > 8)
		{
		// real transparency
		const UBYTE *AlphaChannel = NULL;

		d1(KPrintF("%s/%s/%ld: Real transparency\n", __FILE__, __FUNC__, __LINE__));

		if (gg->Flags & GFLG_SELECTED)
			GetAttr(IDTA_SelAlphaChannel, IconObj, (APTR) &AlphaChannel);
		if (NULL == AlphaChannel)	// not GFLG_SELECTED or no IDTA_SelAlphaChannel present
			GetAttr(IDTA_AlphaChannel, IconObj, (APTR) &AlphaChannel);

		d1(KPrintF("%s/%s/%ld: AlphaChannel=%08lx\n", __FILE__, __FUNC__, __LINE__, AlphaChannel));

		ScalosGfxBlitIconTags(iwt->iwt_WindowTask.wt_Window->RPort, rp,
				iwt->iwt_InnerLeft + gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset,
				iwt->iwt_InnerTop + gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset,
				gg->BoundsWidth, gg->BoundsHeight,
				SCALOSGFX_BlitIconAlpha, (ULONG) AlphaChannel,
				SCALOSGFX_BlitIconTransparency, (transparency * 255) / 100,
				TAG_END);

		DrawGhosted = FALSE;

		WaitBlit();
		}

	bmWidth = gg->BoundsWidth;
	bmFlags = GetBitMapAttr(allocBM, BMA_FLAGS);
	bmDepth = GetBitMapAttr(allocBM, BMA_DEPTH);

	d1(kprintf("%s/%s/%ld: allocBM Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, bmFlags));

	if (bmFlags & BMF_INTERLEAVED)
		bmWidth *= bmDepth;

	d1(kprintf("%s/%s/%ld: bmWidth=%ld\n", __FILE__, __FUNC__, __LINE__, bmWidth));

	maskBM = AllocBitMap(bmWidth, gg->BoundsHeight, 1, BMF_CLEAR, NULL);

	d1(kprintf("%s/%s/%ld: BoundsWidth=%ld  BoundsHeight=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gg->BoundsWidth, gg->BoundsHeight));

	d1(KPrintF("%s/%s/%ld: maskBM=%08lx\n", __FILE__, __FUNC__, __LINE__, maskBM));
	if (maskBM)
		{
		struct BitMap *MaskBMSelected = NULL;
		LONG LeftSpace, TopSpace, RightSpace, BottomSpace;
		WORD Width, Height;

		LeftSpace = TopSpace = RightSpace = BottomSpace = 0;

		Width = gg->Width - LeftSpace - RightSpace;
		Height = gg->Height - TopSpace - BottomSpace;

		d1(kprintf("%s/%s/%ld: maskBM->Planes[0]=%08lx\n", __FILE__, __FUNC__, __LINE__, maskBM->Planes[0]));

		rpMask->BitMap = maskBM;

		GetAttr((gg->Flags & GFLG_SELECTED) ? IDTA_MaskBM_Selected : IDTA_MaskBM_Normal,
			IconObj, (APTR) &MaskBMSelected);

		d1(kprintf("%s/%s/%ld: Mask=%08lx  x=%ld  y=%ld  xb=%ld  yb=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, MaskBMSelected, \
			gg->LeftEdge, gg->TopEdge, \
			gg->BoundsLeftEdge, gg->BoundsTopEdge,
			gg->Width, gg->Height));
		d1(KPrintF("%s/%s/%ld: MaskBMSelected=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskBMSelected));

		if (MaskBMSelected)
			{
			d1(kprintf("%s/%s/%ld: LeftSpace=%ld  TopSpace=%ld\n", __FILE__, __FUNC__, __LINE__, LeftSpace, TopSpace));
			d1(kprintf("%s/%s/%ld: w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height));

			BltBitMap(MaskBMSelected, 0, 0, maskBM,
				gg->LeftEdge - gg->BoundsLeftEdge + LeftSpace,
				gg->TopEdge - gg->BoundsTopEdge + TopSpace,
				Width, Height,
				ABC | ABNC, ~0, NULL);
			}
		else
			{
			d1(kprintf("%s/%s/%ld: x1=%ld  y1=%ld  x2=%ld  y2=%ld\n", __FILE__, __FUNC__, __LINE__, \
				gg->LeftEdge - gg->BoundsLeftEdge,\
				gg->TopEdge - gg->BoundsTopEdge,\
				gg->Width - 1, gg->Height - 1));

			RectFill(rpMask,
				gg->LeftEdge - gg->BoundsLeftEdge + LeftSpace,
                                gg->TopEdge - gg->BoundsTopEdge + TopSpace,
				gg->LeftEdge - gg->BoundsLeftEdge + LeftSpace + Width - 1,
				gg->TopEdge - gg->BoundsTopEdge + TopSpace + Height - 1);

			d1(kprintf("%s/%s/%ld: Mask=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
				maskBM->Planes[0][0], maskBM->Planes[0][1], \
				maskBM->Planes[0][2], maskBM->Planes[0][3], \
				maskBM->Planes[0][4], maskBM->Planes[0][5], \
				maskBM->Planes[0][6], maskBM->Planes[0][7]));
			}

		if (DrawGhosted)
			{
			// ghost icon image
			static UWORD Pattern[] = { 0x5555, 0xaaaa };

			d1(kprintf("%s/%s/%ld: DrawGhosted\n", __FILE__, __FUNC__, __LINE__));

			SetABPenDrMd(rpMask, 0, 0, JAM1);
			SetAfPt(rpMask, Pattern, 1);
			RectFill(rpMask, 0, 0,
				gg->BoundsWidth - 1,
				gg->BoundsHeight - 1);
			}

		BltMaskBitMapRastPort(allocBM, 0, 0,
			iwt->iwt_WindowTask.wt_Window->RPort,
			iwt->iwt_InnerLeft + gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset,
			iwt->iwt_InnerTop + gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset,
			gg->BoundsWidth, gg->BoundsHeight,
			ABC|ABNC|ANBC, rpMask->BitMap->Planes[0]);

		d1(kprintf("%s/%s/%ld: BitMap maskBM=%08lx  BytesPerRow=%ld  Rows=%ld  Flags=%02lx  Width=%ld  Depth=%ld\n", \
			__FILE__, __FUNC__, __LINE__, maskBM, maskBM->BytesPerRow, maskBM->Rows, maskBM->Flags, bmWidth, maskBM->Depth));
		d1(kprintf("%s/%s/%ld: BitMap AllocBM=%08lx  BytesPerRow=%ld  Rows=%ld  Flags=%02lx  Width=%ld  Depth=%ld\n", \
			__FILE__, __FUNC__, __LINE__, allocBM, allocBM->BytesPerRow, allocBM->Rows, allocBM->Flags, gg->BoundsWidth, allocBM->Depth));

		WaitBlit();
		FreeBitMap(maskBM);
		}

	if (NULL == WidthArray)
		{
		// Draw icon text (w/o shadow effect)
		DoMethod(IconObj,
			IDTM_Draw,
			iwt->iwt_WinScreen,
			iwt->iwt_WindowTask.wt_Window,
			iwt->iwt_WindowTask.wt_Window->RPort,
			iwt->iwt_WinDrawInfo,
			iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
			iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset,
			IODRAWF_NoImage);

		WaitBlit();
		}
	FreeBitMap(allocBM);

	d1(KPrintF("\n" "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void DrawIconObjectK(struct internalScaWindowTask *iwt, Object *IconObj,
	ULONG ScreenDepth, struct RastPort *rp, struct RastPort *rpMask,
	const struct ARGB *K)
{
	LONG bmWidth;
	ULONG bmFlags, bmDepth;
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;
	struct BitMap *allocBM;
	struct BitMap *maskBM;
	BOOL DrawGhosted = TRUE;

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	if (!IsIconObjVisible(iwt, IconObj))
		{
		d1(kprintf("%s/%s/%ld: IconObj=%08lx invisible\n", __FILE__, __FUNC__, __LINE__, IconObj));
		return;
		}

	SetABPenDrMd(rp, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawModeSel);
	SetABPenDrMd(rpMask, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, JAM1);

	rp->BitMap = allocBM = AllocBitMap(gg->BoundsWidth, gg->BoundsHeight,
		ScreenDepth,
		BMF_MINPLANES | BMF_CLEAR,
		iwt->iwt_WinScreen->RastPort.BitMap);
	d1(KPrintF("%s/%s/%ld: BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, allocBM));
	if (NULL == rp->BitMap)
		return;

	// Draw icon (image only)
	DoMethod(IconObj, IDTM_Draw,
		iwt->iwt_WinScreen,
		NULL,
		rp,
		iwt->iwt_WinDrawInfo,
		gg->LeftEdge - gg->BoundsLeftEdge,
		gg->TopEdge - gg->BoundsTopEdge,
		IODRAWF_NoText | IODRAWF_AbsolutePos | IODRAWF_NoAlpha);

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	if (CyberGfxBase && ScreenDepth > 8)
		{
		// real transparency
		const UBYTE *AlphaChannel = NULL;

		d1(KPrintF("%s/%s/%ld: Real transparency\n", __FILE__, __FUNC__, __LINE__));

		if (gg->Flags & GFLG_SELECTED)
			GetAttr(IDTA_SelAlphaChannel, IconObj, (APTR) &AlphaChannel);
		if (NULL == AlphaChannel)	// not GFLG_SELECTED or no IDTA_SelAlphaChannel present
			GetAttr(IDTA_AlphaChannel, IconObj, (APTR) &AlphaChannel);

		d1(KPrintF("%s/%s/%ld: AlphaChannel=%08lx\n", __FILE__, __FUNC__, __LINE__, AlphaChannel));

		ScalosGfxBlitIconTags(iwt->iwt_WindowTask.wt_Window->RPort, rp,
				iwt->iwt_InnerLeft + gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset,
				iwt->iwt_InnerTop + gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset,
				gg->BoundsWidth, gg->BoundsHeight,
				SCALOSGFX_BlitIconAlpha, (ULONG) AlphaChannel,
				SCALOSGFX_BlitIconHilight, (ULONG) K,
				TAG_END);

		DrawGhosted = FALSE;

		WaitBlit();
		}

	bmWidth = gg->BoundsWidth;
	bmFlags = GetBitMapAttr(allocBM, BMA_FLAGS);
	bmDepth = GetBitMapAttr(allocBM, BMA_DEPTH);

	d1(kprintf("%s/%s/%ld: allocBM Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, bmFlags));

	if (bmFlags & BMF_INTERLEAVED)
		bmWidth *= bmDepth;

	d1(kprintf("%s/%s/%ld: bmWidth=%ld\n", __FILE__, __FUNC__, __LINE__, bmWidth));

	maskBM = AllocBitMap(bmWidth, gg->BoundsHeight, 1, BMF_CLEAR, NULL);

	d1(kprintf("%s/%s/%ld: BoundsWidth=%ld  BoundsHeight=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gg->BoundsWidth, gg->BoundsHeight));

	d1(KPrintF("%s/%s/%ld: maskBM=%08lx\n", __FILE__, __FUNC__, __LINE__, maskBM));
	if (maskBM)
		{
		struct BitMap *MaskBMSelected = NULL;
		LONG LeftSpace, TopSpace, RightSpace, BottomSpace;
		WORD Width, Height;

		LeftSpace = TopSpace = RightSpace = BottomSpace = 0;

		Width = gg->Width - LeftSpace - RightSpace;
		Height = gg->Height - TopSpace - BottomSpace;

		d1(KPrintF("%s/%s/%ld: maskBM->Planes[0]=%08lx\n", __FILE__, __FUNC__, __LINE__, maskBM->Planes[0]));

		rpMask->BitMap = maskBM;

		GetAttr((gg->Flags & GFLG_SELECTED) ? IDTA_MaskBM_Selected : IDTA_MaskBM_Normal,
			IconObj, (APTR) &MaskBMSelected);

		d1(KPrintF("%s/%s/%ld: Mask=%08lx  x=%ld  y=%ld  xb=%ld  yb=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, MaskBMSelected, \
			gg->LeftEdge, gg->TopEdge, \
			gg->BoundsLeftEdge, gg->BoundsTopEdge,
			gg->Width, gg->Height));

		if (MaskBMSelected)
			{
			d1(kprintf("%s/%s/%ld: LeftSpace=%ld  TopSpace=%ld\n", __FILE__, __FUNC__, __LINE__, LeftSpace, TopSpace));
			d1(kprintf("%s/%s/%ld: w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height));

			BltBitMap(MaskBMSelected, 0, 0, maskBM,
				gg->LeftEdge - gg->BoundsLeftEdge + LeftSpace,
				gg->TopEdge - gg->BoundsTopEdge + TopSpace,
				Width, Height,
				ABC | ABNC, ~0, NULL);
			}
		else
			{
			d1(kprintf("%s/%s/%ld: x1=%ld  y1=%ld  x2=%ld  y2=%ld\n", __FILE__, __FUNC__, __LINE__, \
				gg->LeftEdge - gg->BoundsLeftEdge,\
				gg->TopEdge - gg->BoundsTopEdge,\
				gg->Width - 1, gg->Height - 1));

			RectFill(rpMask,
				gg->LeftEdge - gg->BoundsLeftEdge + LeftSpace,
                                gg->TopEdge - gg->BoundsTopEdge + TopSpace,
				gg->LeftEdge - gg->BoundsLeftEdge + LeftSpace + Width - 1,
				gg->TopEdge - gg->BoundsTopEdge + TopSpace + Height - 1);

			d1(kprintf("%s/%s/%ld: Mask=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
				maskBM->Planes[0][0], maskBM->Planes[0][1], \
				maskBM->Planes[0][2], maskBM->Planes[0][3], \
				maskBM->Planes[0][4], maskBM->Planes[0][5], \
				maskBM->Planes[0][6], maskBM->Planes[0][7]));
			}

		if (DrawGhosted)
			{
			// ghost icon image
			static UWORD Pattern[] = { 0x5555, 0xaaaa };

			d1(kprintf("%s/%s/%ld: DrawGhosted\n", __FILE__, __FUNC__, __LINE__));

			SetABPenDrMd(rpMask, 0, 0, JAM1);
			SetAfPt(rpMask, Pattern, 1);
			RectFill(rpMask, 0, 0,
				gg->BoundsWidth - 1,
				gg->BoundsHeight - 1);
			}

		BltMaskBitMapRastPort(allocBM, 0, 0,
			iwt->iwt_WindowTask.wt_Window->RPort,
			iwt->iwt_InnerLeft + gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset,
			iwt->iwt_InnerTop + gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset,
			gg->BoundsWidth, gg->BoundsHeight,
			ABC|ABNC|ANBC, rpMask->BitMap->Planes[0]);

		d1(kprintf("%s/%s/%ld: BitMap maskBM=%08lx  BytesPerRow=%ld  Rows=%ld  Flags=%02lx  Width=%ld  Depth=%ld\n", \
			__FILE__, __FUNC__, __LINE__, maskBM, maskBM->BytesPerRow, maskBM->Rows, maskBM->Flags, bmWidth, maskBM->Depth));
		d1(kprintf("%s/%s/%ld: BitMap AllocBM=%08lx  BytesPerRow=%ld  Rows=%ld  Flags=%02lx  Width=%ld  Depth=%ld\n", \
			__FILE__, __FUNC__, __LINE__, allocBM, allocBM->BytesPerRow, allocBM->Rows, allocBM->Flags, gg->BoundsWidth, allocBM->Depth));

		WaitBlit();
		FreeBitMap(maskBM);
		}

	// Draw icon text (w/o shadow effect)
	DoMethod(IconObj,
		IDTM_Draw,
		iwt->iwt_WinScreen,
		iwt->iwt_WindowTask.wt_Window,
		iwt->iwt_WindowTask.wt_Window->RPort,
		iwt->iwt_WinDrawInfo,
		iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
		iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset,
		IODRAWF_NoImage);

	WaitBlit();
	FreeBitMap(allocBM);

	d1(KPrintF("\n" "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void DrawTransparentIconObject(struct internalScaWindowTask *iwt, Object *IconObj, ULONG Transparency)
{
	// Draw IconObj in shadowed state
	ULONG ScreenDepth;
	struct RastPort rpMask;
	struct RastPort rp;

	d1(KPrintF("\n" "%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	Scalos_InitRastPort(&rp);
	Scalos_SetFont(&rp, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

	Scalos_InitRastPort(&rpMask);
	Scalos_SetFont(&rpMask, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

	ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	DrawIconObjectTransparent(iwt, IconObj, ScreenDepth, &rp, &rpMask, Transparency, TRUE);

	WaitBlit();

	Scalos_DoneRastPort(&rpMask);
	Scalos_DoneRastPort(&rp);

	d1(KPrintF("\n" "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void DrawTransparentIconObjectK(struct internalScaWindowTask *iwt, Object *IconObj, const struct ARGB *K)
{
	// Draw IconObj in highlighted state
	ULONG ScreenDepth;
	struct RastPort rpMask;
	struct RastPort rp;

	d1(kprintf("\n" "%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	Scalos_InitRastPort(&rp);
	Scalos_SetFont(&rp, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

	Scalos_InitRastPort(&rpMask);
	Scalos_SetFont(&rpMask, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

	ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	DrawIconObjectK(iwt, IconObj, ScreenDepth, &rp, &rpMask, K);

	WaitBlit();

	Scalos_DoneRastPort(&rpMask);
	Scalos_DoneRastPort(&rp);
}


void  ScalosBlurPixelArray(struct ARGB *Dest, const struct ARGB *Src,
	UWORD SizeX, UWORD SizeY, const UBYTE *Mask)
{
	ULONG y;
	const ULONG Blur = 4;
	ULONG Divisor = Blur + 24;
	ULONG MaskBytesPerRow;

	MaskBytesPerRow = ((SizeX + 15) & ~0x0f) / 8;

	for (y = 0; y < SizeY; y++)
		{
		ULONG x;
		const struct ARGB *SrcPtr = Src;
		const struct ARGB *Upper, *Lower;
		const struct ARGB *Upper2, *Lower2;
		struct ARGB *DestPtr = Dest;
		UWORD BitMask = 0x0080;
		const UBYTE *MaskPtr2 = Mask;

		Upper = Upper2 = Lower = Lower2 = SrcPtr;
		if (y > 0)
			Upper -= SizeX;
		if (y > 1)
			Upper2 -= 2 * SizeX;
		if (y < (SizeY - 1))
			Lower += SizeX;
		if (y < (SizeY - 2))
			Lower2 += SizeX;

		for (x = 0; x < SizeX; x++)
			{
			if (*MaskPtr2 & BitMask)
				{
				*DestPtr = *SrcPtr;
				}
			else
				{
				ULONG Left, Right;
				ULONG Left2, Right2;

				Left   = (x > 0)           ? -1 : 0;
				Left2  = (x > 1)           ? -2 : 0;
				Right  = (x < (SizeX - 1)) ?  1 : 0;
				Right2 = (x < (SizeX - 2)) ?  2 : 0;

				DestPtr->Red   = (Upper2[Left2].Red   + Upper2[Left].Red   +        Upper2[0].Red   + Upper2[Right].Red   + Upper2[Right2].Red   +
						   Upper[Left2].Red   +  Upper[Left].Red   +         Upper[0].Red   +  Upper[Right].Red   +  Upper[Right2].Red   +
						  SrcPtr[Left2].Red   +	SrcPtr[Left].Red   + Blur * SrcPtr[0].Red   + SrcPtr[Right].Red   + SrcPtr[Right2].Red   +
						   Lower[Left2].Red   +  Lower[Left].Red   +         Lower[0].Red   +  Lower[Right].Red   +  Lower[Right2].Red   +
						  Lower2[Left2].Red   +	Lower2[Left].Red   +        Lower2[0].Red   + Lower2[Right].Red   + Lower2[Right2].Red   ) / Divisor;

				DestPtr->Green = (Upper2[Left2].Green + Upper2[Left].Green +        Upper2[0].Green + Upper2[Right].Green + Upper2[Right2].Green +
						   Upper[Left2].Green +  Upper[Left].Green +         Upper[0].Green +  Upper[Right].Green +  Upper[Right2].Green +
						  SrcPtr[Left2].Green +	SrcPtr[Left].Green + Blur * SrcPtr[0].Green + SrcPtr[Right].Green + SrcPtr[Right2].Green +
						   Lower[Left2].Green +  Lower[Left].Green +         Lower[0].Green +  Lower[Right].Green +  Lower[Right2].Green +
						  Lower2[Left2].Green +	Lower2[Left].Green +        Lower2[0].Green + Lower2[Right].Green + Lower2[Right2].Green ) / Divisor;

				DestPtr->Blue  = (Upper2[Left2].Blue  + Upper2[Left].Blue  +        Upper2[0].Blue  + Upper2[Right].Blue  + Upper2[Right2].Blue  +
						   Upper[Left2].Blue  +  Upper[Left].Blue  +         Upper[0].Blue  +  Upper[Right].Blue  +  Upper[Right2].Blue  +
						  SrcPtr[Left2].Blue  +	SrcPtr[Left].Blue  + Blur * SrcPtr[0].Blue  + SrcPtr[Right].Blue  + SrcPtr[Right2].Blue  +
						   Lower[Left2].Blue  +  Lower[Left].Blue  +         Lower[0].Blue  +  Lower[Right].Blue  +  Lower[Right2].Blue  +
						  Lower2[Left2].Blue  +	Lower2[Left].Blue  +        Lower2[0].Blue  + Lower2[Right].Blue  + Lower2[Right2].Blue  ) / Divisor;
				}

			SrcPtr++;
			Upper++;
			Upper2++;
			Lower++;
			Lower2++;
			DestPtr++;

			BitMask >>= 1;
			if (0 == BitMask)
				{
				BitMask = 0x0080;
				MaskPtr2++;
				}
			}

		Mask += MaskBytesPerRow;
		Src += SizeX;
		Dest += SizeX;
		}
}


ULONG ScalosReadPixelArray(APTR DestRect, UWORD DestMod, struct RastPort *rp,
		UWORD SrcX, UWORD SrcY, UWORD SizeX, UWORD SizeY)
{
	return ReadPixelArray(DestRect, 0, 0, DestMod,
			rp, SrcX, SrcY,
			SizeX, SizeY,
			RECTFMT_ARGB);
}


ULONG ScalosWritePixelArray(APTR SrcRect, UWORD SrcMod, struct RastPort *rp,
		UWORD DestX, UWORD DestY, UWORD SizeX, UWORD SizeY)
{
	return WritePixelArray(SrcRect, 0, 0, SrcMod,
		rp, DestX, DestY,
		SizeX, SizeY, RECTFMT_ARGB);
}


BOOL IsIconObjVisible(const struct internalScaWindowTask *iwt, const Object *IconObj)
{
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;

	if ((gg->BoundsLeftEdge + gg->BoundsWidth - iwt->iwt_WindowTask.wt_XOffset < 0)
		|| (gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset > iwt->iwt_InnerWidth)
		|| (gg->BoundsTopEdge + gg->BoundsHeight - iwt->iwt_WindowTask.wt_YOffset < 0)
		|| (gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset > iwt->iwt_InnerHeight) )
		return FALSE;

	return TRUE;
}


// Erase all icons from <IconList> in window
void RemoveIcons(struct internalScaWindowTask *iwt, struct ScaIconNode **IconList)
{
	struct Rectangle WindowRect;
	struct Region *ClipRegion;
	struct ScaIconNode *in;

	if (NULL == IconList)
		return;
	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return;

	d1(KPrintF("%s/%s/%ld: START  DragFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragFlags));
	d1(KPrintF("%s/%s/%ld: IconList=%08lx  *IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, IconList, *IconList));

	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
		{
		d1(kprintf("%s/%s/%ld: WindowLocked\n", __FILE__, __FUNC__, __LINE__));
		iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
		return;
		}

	ClipRegion = NewRegion();
	d1(KPrintF("%s/%s/%ld: ClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, ClipRegion));
	if (NULL == ClipRegion)
		return;

	for (in=*IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
		struct Rectangle IconRect;

		d1(KPrintF("%s/%s/%ld: Icon=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
		d1(kprintf("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, \
			gg->BoundsLeftEdge, gg->BoundsTopEdge));

		in->in_Flags &= ~INF_IconVisible;	// this icon is no longer visible

		IconRect.MaxX = IconRect.MinX = gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset + iwt->iwt_InnerLeft;
		IconRect.MaxX += gg->BoundsWidth - 1;
		IconRect.MinY = IconRect.MaxY = gg->BoundsTopEdge  - iwt->iwt_WindowTask.wt_YOffset + iwt->iwt_InnerTop;
		IconRect.MaxY += gg->BoundsHeight - 1;

		d1(kprintf("%s/%s/%ld: IconRect  MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, IconRect.MinX, IconRect.MaxX, \
			IconRect.MinY, IconRect.MaxY));

		OrRectRegion(ClipRegion, &IconRect);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	WindowRect.MinX = WindowRect.MaxX = iwt->iwt_InnerLeft;
	WindowRect.MaxX += iwt->iwt_InnerWidth - 1;
	WindowRect.MinY = WindowRect.MaxY = iwt->iwt_InnerTop;
	WindowRect.MaxY += iwt->iwt_InnerHeight - 1;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	AndRectRegion(ClipRegion, &WindowRect);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
		{
		d1(KPrintF("%s/%s/%ld: WindowLocked\n", __FILE__, __FUNC__, __LINE__));
		iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
		}
	else
		{
		struct Region *oldClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, ClipRegion);

		d1(KPrintF("%s/%s/%ld: DragFlags=%08lx  ClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragFlags, ClipRegion));

		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort, 0, 0,
			iwt->iwt_WindowTask.wt_Window->Width - 1,
			iwt->iwt_WindowTask.wt_Window->Height - 1);

		InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, oldClipRegion);
		d1(kprintf("%s/%s/%ld: oldClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, oldClipRegion));
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DisposeRegion(ClipRegion);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// now redraw all icons which have been overlapped by one of the removed icons
	if (ScalosAttemptLockIconListShared(iwt))
		{
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
			struct ScaIconNode *in2;

			d1(KPrintF("%s/%s/%ld: IconObj=%08lx  class=%08lx\n", __FILE__, __FUNC__, __LINE__, in->in_Icon, OCLASS(in->in_Icon)));

			for (in2=*IconList; in2; in2 = (struct ScaIconNode *) in2->in_Node.mln_Succ)
				{
				if (in2 == in)
					break;
				if (in2->in_Icon == in->in_Icon)
					break;

				d1(kprintf("%s/%s/%ld: CheckOverlap  in1=%08lx <%s>  in2=%08lx <%s>\n", \
					__FILE__, __FUNC__, __LINE__, in, GetIconName(in), in2, GetIconName(in2)));

				if (IconsOverlap(in, in2))
					{
					d1(kprintf("%s/%s/%ld: Redraw in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
					d1(KPrintF("%s/%s/%ld: IconObj=%08lx  class=%08lx\n", __FILE__, __FUNC__, \
						__LINE__, in->in_Icon, OCLASS(in->in_Icon)));

					if ((gg->Flags & GFLG_SELECTED) && NULL == gg->SelectRender)
						{
						DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
							SCCM_IconWin_LayoutIcon, in->in_Icon, IOLAYOUTF_SelectedImage);
						}
					if (!(gg->Flags & GFLG_SELECTED) && NULL == gg->GadgetRender)
						{
						DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
							SCCM_IconWin_LayoutIcon, in->in_Icon, IOLAYOUTF_NormalImage);
						}

					DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DrawIcon, in->in_Icon);
					}
				}
			}

		ScalosUnLockIconList(iwt);
		}
	d1(KPrintF("\n" "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static BOOL IconsOverlap(const struct ScaIconNode *in1, const struct ScaIconNode *in2)
{
	const struct ExtGadget *gg1 = (struct ExtGadget *) in1->in_Icon;
	const struct ExtGadget *gg2 = (struct ExtGadget *) in2->in_Icon;

	if (gg1->BoundsLeftEdge < (gg2->BoundsLeftEdge + gg2->BoundsWidth)
		&& (gg1->BoundsLeftEdge + gg1->BoundsWidth) >= gg2->BoundsLeftEdge
		&& gg1->BoundsTopEdge < (gg2->BoundsTopEdge + gg2->BoundsHeight)
		&& (gg1->BoundsTopEdge + gg1->BoundsHeight) >= gg2->BoundsTopEdge)
			return TRUE;

	return FALSE;
}



	// Draw IconObj in ghosted state
static void DrawIconObjectDisabled(struct internalScaWindowTask *iwt, Object *IconObj)
{
	static UWORD Pattern[] = { 0x4444, 0x1111 };
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;
	struct BitMap *MaskBM = NULL;
	struct RastPort rp;

	d1(kprintf("%s/%s/%ld: Draw disabled IconObj=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, IconObj));

	if (gg->Flags & GFLG_SELECTED)
		GetAttr(IDTA_MaskBM_Selected, IconObj, (APTR) &MaskBM);
	else
		GetAttr(IDTA_MaskBM_Normal, IconObj, (APTR) &MaskBM);

	Scalos_InitRastPort(&rp);

	rp.BitMap = AllocBitMap(gg->Width, gg->Height,
		iwt->iwt_WinScreen->RastPort.BitMap->Depth,
		BMF_CLEAR | BMF_MINPLANES | BMF_DISPLAYABLE,
		iwt->iwt_WinScreen->RastPort.BitMap);

	if (rp.BitMap)
		{
		// Blit icon image from screen into rp
		ClipBlit(iwt->iwt_WindowTask.wt_Window->RPort,
			gg->LeftEdge + iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
			gg->TopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset,
			&rp,
			0, 0,
			gg->Width, gg->Height,
			ABC | ABNC);

		SetRPAttrs(&rp,
			RPTAG_APen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[TEXTPEN],
			RPTAG_DrMd, JAM1,
			TAG_END);
		SetAfPt(&rp, Pattern, 1);

		if (MaskBM)
			{
			// Fill rp through mask with "Ghost" pattern
			BltPattern(&rp,
				MaskBM->Planes[0],
				0, 0,
				gg->Width, gg->Height,
				MaskBM->BytesPerRow);
			}
		else
			{
			RectFill(&rp,
				0, 0,
				gg->Width - 1,
				gg->Height - 1);
			}

		// Blit rp back to screen
		ClipBlit(&rp, 0, 0,
			iwt->iwt_WindowTask.wt_Window->RPort,
			gg->LeftEdge + iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
			gg->TopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset,
			gg->Width, gg->Height,
			ABC | ABNC);

		WaitBlit();
		FreeBitMap(rp.BitMap);
		}

	Scalos_DoneRastPort(&rp);
}


// Draw IconOverlay
static void DrawIconObjectOverlay(struct internalScaWindowTask *iwt, Object *IconObj)
{
	ULONG IconOverlayType = ICONOVERLAY_None;
	struct DatatypesImage *dti;
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;
	LONG InnerLeft = 0;
	LONG InnerBottom = 0;
	LONG x, y;
	ULONG n;

	GetAttr(IDTA_OverlayType, IconObj, &IconOverlayType);
	GetAttr(IDTA_InnerLeft, IconObj, (ULONG *) &InnerLeft);
	GetAttr(IDTA_InnerBottom, IconObj, (ULONG *) &InnerBottom);

	x = gg->LeftEdge + InnerLeft + iwt->iwt_InnerLeft
			- iwt->iwt_WindowTask.wt_XOffset;
	y = gg->TopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset
			+ gg->Height - InnerBottom;

	d1(KPrintF("%s/%s/%ld: IconObj=%08lx  IconOverlayType=%ld\n", __FILE__, __FUNC__, __LINE__, IconObj, IconOverlayType));

	for (n = 0; n < ICONOVERLAY_MAX; n++)
		{
		if (IconOverlayType & (1 << n))
			dti = iwt->iwt_IconOverlays[n];
		else
			dti = NULL;

		d1(KPrintF("%s/%s/%ld: dti=%08lx\n", __FILE__, __FUNC__, __LINE__, dti));
		if (dti)
			{
			LONG y0 = y - dti->dti_BitMapHeader->bmh_Height;

			d1(KPrintF("%s/%s/%ld: w=%ld  h=%ld\n", \
				__FILE__, __FUNC__, __LINE__, dti->dti_BitMapHeader->bmh_Width, dti->dti_BitMapHeader->bmh_Height));
#if 1
			if (dti->dti_MaskPlane)
				{
				BltMaskBitMapRastPort(dti->dti_BitMap,
					0, 0,
					iwt->iwt_WindowTask.wt_Window->RPort,
					x, y0,
					dti->dti_BitMapHeader->bmh_Width,
					dti->dti_BitMapHeader->bmh_Height,
					ABC | ABNC | ANBC,
					dti->dti_MaskPlane);
				}
			else
				{
				struct RastPort SrcRp;

				InitRastPort(&SrcRp);
				SrcRp.BitMap = dti->dti_BitMap;

				ClipBlit(&SrcRp,
					0, 0,
					iwt->iwt_WindowTask.wt_Window->RPort,
					x, y0,
					dti->dti_BitMapHeader->bmh_Width,
					dti->dti_BitMapHeader->bmh_Height,
					0xc0);
				}
#else
			DoMethod(dti->dti_ImageObj,
				DTM_DRAW,
				iwt->iwt_WindowTask.wt_Window->RPort,
				x, y0,
				dti->dti_BitMapHeader->bmh_Width,
				dti->dti_BitMapHeader->bmh_Height,
				0, 0,
				NULL
				);
#endif

			x += 1 + dti->dti_BitMapHeader->bmh_Width;
			}
		}
}

