// DragDropBobs.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <graphics/gfxmacros.h>
#include <hardware/blit.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <cybergraphx/cybergraphics.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/scalosprefs.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"


//----------------------------------------------------------------------------

#define	NO_BOB_POSITION						0x8000

#define	PATH_POINTER_ICONS					"THEME:PointerIcons/"
#define	PATH_POINTER_ICONS_FALLBACK				"ENV:Scalos/"

#define	NAME_FORBIDDEN		PATH_POINTER_ICONS 		"forbidden"
#define	NAME_COPYING		PATH_POINTER_ICONS 		"copying"
#define	NAME_MAKELINK		PATH_POINTER_ICONS 		"makelink"
#define	NAME_MOVING		PATH_POINTER_ICONS 		"moving"

#define	NAME_FORBIDDEN_FALLBACK	PATH_POINTER_ICONS_FALLBACK	"forbidden"
#define	NAME_COPYING_FALLBACK	PATH_POINTER_ICONS_FALLBACK	"copying"
#define	NAME_MAKELINK_FALLBACK	PATH_POINTER_ICONS_FALLBACK	"makelink"
#define	NAME_MOVING_FALLBACK	PATH_POINTER_ICONS_FALLBACK	"moving"

//----------------------------------------------------------------------------

// local functions

static void AppendDragNode(struct internalScaWindowTask *iwt, struct RastPort *rp,
	struct ScaIconNode *in, LONG n, LONG x, LONG y, LONG x0, LONG y0);
static struct ScaBob *InitDrag_ImageOnly(struct DragNode *dgn, 
	struct ScaBob **BobList,
	struct RastPort *rp, struct internalScaWindowTask *iwt, 
	LONG x0, LONG y0);
static void InitDrag_ImageAndText(struct DragNode *dgn, 
	struct ScaBob **BobList,
	struct RastPort *rp, struct internalScaWindowTask *iwt,
	LONG x0, LONG y0);
static struct ScaBob *internalAddBob(struct DragHandle *dh, 
	struct ScaBob **BobList,
	struct BitMap *bm, APTR Mask, const UBYTE *AlphaChannel,
	ULONG Width, ULONG Height, 
	ULONG BoundsWidth, ULONG BoundsHeight, 
	WORD LeftBorder,
	LONG XOffset, LONG YOffset);
static void DrawIconListShadows(struct internalScaWindowTask *iwt, struct ScaIconNode **IconList);
static void FreeBobNode(struct ScalosNodeList *nodeList, struct ScaBob *bob);
static void RemoveCustomBobsFromScreen(struct ScaBob **BobList);
static void FreeCustomBobNode(struct ScalosNodeList *nodeList, struct ScaBob *bob);
static struct ScaBob *sca_AddBobCustom(struct DragHandle *dh, 
	struct ScaBob **BobList,
	struct BitMap *bm, APTR Mask, const UBYTE *AlphaChannel,
	ULONG Width, ULONG Height, 
	ULONG BoundsWidth, ULONG BoundsHeight, 
	WORD LeftBorder,
	LONG XOffset, LONG YOffset);
static void BlitBob(struct ScaBob *bob, struct BitMap *bmSrc, struct BitMap *bmDest);
static LONG CompareLeftUpFunc(const struct ScaBob *bob1, const struct ScaBob *bob2);
static void sca_DrawDragCustom(struct DragHandle *dh, LONG x, LONG y, ULONG Flags);
static void RestoreBuffer(struct DragHandle *dh,
	struct BitMap *DestBM,
	LONG x, LONG y, LONG Width, LONG Height);
static void RestoreBuffer2(struct DragHandle *dh,
	struct BitMap *DestBM,
	LONG x, LONG y, LONG Width, LONG Height);
static BOOL ClipBlitIn(struct Screen *Scr, struct BitMap *DestBM,
	LONG SrcX, LONG SrcY, LONG Width, LONG Height);
static BOOL ClipBlitOut(struct Screen *Scr, struct BitMap *SrcBM,
	LONG DstX, LONG DstY, LONG Width, LONG Height);
static void BlitBob2(struct DragHandle *dh, 
	struct ScaBob *bob, struct BitMap *bm, 
	LONG SrcX, LONG SrcY, LONG DestX, LONG DestY, 
	LONG Width, LONG Height);
static void BlitTrans(struct DragHandle *dh, 
	const struct ScaBob *bob, struct BitMap *SrcBM, 
	LONG SrcX, LONG SrcY, LONG DestX, LONG DestY, 
	LONG Width, LONG Height, const struct BitMap *MaskBM);
static void OptimizeBobs(struct DragHandle *dh);
static void MergeBobs(struct RastPort *TempRp, struct DragHandle *dh, 
	struct ScaBob **NewBobList, struct ScaBob *NewBob);
static void MergeBobsAlpha(struct RastPort *TempRp, struct DragHandle *dh, 
	struct ScaBob **NewBobList, struct ScaBob *NewBob);
static void SwapLong(LONG *x1, LONG *x2);
static void FreeBobList(struct DragHandle *dh);
static void FreeCustomBobList(struct ScaBob **BobList);
static struct ScaBob *AllocBob2(struct DragHandle *dh, struct ScalosNodeList *BobList,
	LONG x, LONG y, LONG Width, LONG Height, LONG BoundsWidth, LONG BoundsHeight);
static BOOL IsBobPosition(WORD x);
static void FreeSpeedBitMapAlloc(struct DragHandle *dh);
static BOOL AddSpecialIcon(struct internalScaWindowTask *iwt, struct RastPort *rp, 
	CONST_STRPTR IconFileName, struct ScaBob **BobPtr);
static BOOL AppendSpecialDragNode(struct DragHandle *dh,
	struct internalScaWindowTask *iwt, struct RastPort *rp,
	struct ScaIconNode *in, struct ScaBob **BobPtr);
static void CheckSpecialBobCustom(struct DragHandle *dh, ULONG Flags, 
	ULONG MaskBit, struct ScaBob **bob, CONST_STRPTR FirstName, CONST_STRPTR FallbackName);
static void CheckSpecialBob(struct DragHandle *dh, ULONG Flags, 
	ULONG MaskBit, struct ScaBob **bob, CONST_STRPTR FirstName, CONST_STRPTR FallbackName);
static BOOL AddInfoTextBob(struct internalScaWindowTask *iwt);
static void CountDragObjects(struct DragHandle *drgh, const struct ScaIconNode *in);
static void BlitARGBMaskAlpha(ULONG SrcWidth, ULONG SrcHeight, 
		const struct ARGB *Src, LONG SrcLeft, LONG SrcTop,
		ULONG DestWidth, struct ARGB *Dest, LONG DestLeft, LONG DestTop,
		const struct BitMap *MaskBM, ULONG Trans,
		const UBYTE *Alpha, LONG AlphaLeft, ULONG AlphaWidth);
//static void DumpBitMap(struct BitMap *bm);

//----------------------------------------------------------------------------

// local data

static struct Hook CompareLeftUpHook = 
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(CompareLeftUpFunc),	// h_Entry + h_SubEntry
	NULL,					// h_Data
	};

//----------------------------------------------------------------------------


void InitDrag(struct IntuiMessage *iMsg, struct internalScaWindowTask *iwt)
{
	struct ExtGadget *mgg = (struct ExtGadget *) iwt->iwt_LastIcon;	// Icon directly under mouse
	struct ScaIconNode *in;
	struct RastPort rp;
	LONG x, y;
	LONG x0 = 0, y0 = 0;	// Left/Top Edge of icon directly under mouse
	LONG n;

	iwt->iwt_WinUnderPtr = NULL;		// +jl+ 20010405
	iwt->iwt_IconUnderPtr = NULL;
	iwt->iwt_ObjectUnderMouse = OUM_nothing;

	iwt->iwt_RemIconsFlag = FALSE;

	iwt->iwt_StartDragMouseX = iMsg->MouseX;
	iwt->iwt_StartDragMouseY = iMsg->MouseY;

	x = iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset - iMsg->MouseX;
	y = iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset - iMsg->MouseY;

	switch (CurrentPrefs.pref_DragType)
		{
	case DRAGTYPE_ImageOnly:
		x0 = x + mgg->LeftEdge;
		y0 = y + mgg->TopEdge;
		break;

	case DRAGTYPE_ImageAndText:
		x0 = x + mgg->BoundsLeftEdge;
		y0 = y + mgg->BoundsTopEdge;
		break;
		}

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  MouseX=%ld  MouseY=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, iMsg->MouseX, iMsg->MouseY));

	Scalos_InitRastPort(&rp);
	Scalos_SetFont(&rp, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);
	SetABPenDrMd(&rp, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawModeSel);

	SCA_FreeAllNodes((struct ScalosNodeList *) &iwt->iwt_DragNodeList);

	iwt->iwt_myDragHandle = SCA_InitDrag(iwt->iwt_WinScreen);
	if (NULL == iwt->iwt_myDragHandle)
		return;

	iwt->iwt_myDragHandle->drgh_iwt = iwt;

	iwt->iwt_IconListDDLocked = TRUE;
	ScalosLockIconListExclusive(iwt);

	iwt->iwt_myDragHandle->drgh_FileCount = 0;
	iwt->iwt_myDragHandle->drgh_DrawerCount = 0;
	iwt->iwt_myDragHandle->drgh_DeviceCount = 0;

	ScalosObtainSemaphore(&iwt->iwt_myDragHandle->drgh_BobListSemaphore);

	if (CurrentPrefs.pref_UseOldDragIcons)
		{
		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			if (gg->Flags & GFLG_SELECTED)
				{
				// count devices, drawers and files
				CountDragObjects(iwt->iwt_myDragHandle, in);

				switch (CurrentPrefs.pref_DragType)
					{
				case DRAGTYPE_ImageOnly:
					x0 = x + gg->LeftEdge;
					y0 = y + gg->TopEdge;
					break;

				case DRAGTYPE_ImageAndText:
					x0 = x + gg->BoundsLeftEdge;
					y0 = y + gg->BoundsTopEdge;
					break;
					}

				AppendDragNode(iwt, &rp, in, 0, x, y, x0, y0);
				ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, FALSE);
				}
			}
		}
	else
		{
		// add every selected icon except icon under mouse
		for (n=1, in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			if (gg != mgg && (gg->Flags & GFLG_SELECTED))
				{
				// count devices, drawers and files
				CountDragObjects(iwt->iwt_myDragHandle, in);

				AppendDragNode(iwt, &rp, in, n, x, y, x0 + 5 * n, y0 - 5 * n);
				ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, FALSE);
				n++;
				}
			}

		// Finally, add icon under mouse
		// so that it is being drawn on top of the bob stack
		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			if (gg == mgg)
				{
				// count devices, drawers and files
				CountDragObjects(iwt->iwt_myDragHandle, in);

				AppendDragNode(iwt, &rp, in, 0, x, y, x0, y0);
				ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, FALSE);
				break;
				}
			}
		}

	ScalosUnLockIconList(iwt);

	if (CurrentPrefs.pref_ShowDDCountText)
		AddInfoTextBob(iwt);

	ScalosReleaseSemaphore(&iwt->iwt_myDragHandle->drgh_BobListSemaphore);

	if (NULL == iwt->iwt_DragNodeList)
		{
		SCA_EndDrag(iwt->iwt_myDragHandle);
		iwt->iwt_myDragHandle = NULL;

		EndDragUnlock(iwt);
		}

	UpdateIconCount(iwt);

	Scalos_DoneRastPort(&rp);
}


static void AppendDragNode(struct internalScaWindowTask *iwt, struct RastPort *rp,
	struct ScaIconNode *in, LONG n, LONG x, LONG y, LONG x0, LONG y0)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	struct DragNode *dgn = (struct DragNode *) SCA_AllocStdNode((struct ScalosNodeList *) &iwt->iwt_DragNodeList, NTYP_DragNode);

	d1(KPrintF("%s/%s/%ld: DragNode=%08lx\n", __FILE__, __FUNC__, __LINE__, dgn));
	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  x0=%ld  y0=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, x0, y0));

	if (dgn)
		{
		dgn->drgn_iconnode = in;
		dgn->drgn_icon = in->in_Icon;

		dgn->drgn_x = x + gg->LeftEdge;
		dgn->drgn_y = y + gg->TopEdge;

		dgn->drgn_DeltaX = -x0;
		dgn->drgn_DeltaY = -y0;

		if (n < 3)
			{
			if (in->in_Flags & INF_TextIcon)
				{
				InitDrag_ImageOnly(dgn, &iwt->iwt_myDragHandle->drgh_boblist, rp, iwt, x0, y0);
				}
			else
				{
				switch (CurrentPrefs.pref_DragType)
					{
				case DRAGTYPE_ImageOnly:
					InitDrag_ImageOnly(dgn, &iwt->iwt_myDragHandle->drgh_boblist, rp, iwt, x0, y0);
					break;
				case DRAGTYPE_ImageAndText:
					InitDrag_ImageAndText(dgn, &iwt->iwt_myDragHandle->drgh_boblist, rp, iwt, x0, y0);
					break;
					}
				}
			}
		}
}


static struct ScaBob *InitDrag_ImageOnly(struct DragNode *dgn, 
	struct ScaBob **BobList,
	struct RastPort *rp, struct internalScaWindowTask *iwt,
	LONG x0, LONG y0)
{
	struct ScaIconNode *in = dgn->drgn_iconnode;
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	struct ScaBob *bob;
	WORD Width, Height;
	WORD ggWidth;
	struct BitMap *MaskBMSelected = NULL;
	struct BitMap *MaskBM = NULL;
	PLANEPTR MaskPlane = NULL;
	const UBYTE *AlphaChannel = NULL;
	LONG LeftSpace, TopSpace, RightSpace, BottomSpace;

	LeftSpace = TopSpace = RightSpace = BottomSpace = 0;

	if (in->in_Flags & INF_TextIcon)
		{
		Width = ggWidth = iwt->iwt_WidthArray[WIDTHARRAY_NAME];
		d1(KPrintF("%s/%s/%ld: Width=%0ld\n", __FILE__, __FUNC__, __LINE__, Width));
		}
	else
		{
		ggWidth = gg->Width;
		Width = gg->Width - LeftSpace - RightSpace;
		}

	Height = gg->Height - TopSpace - BottomSpace;

	rp->BitMap = AllocBitMap(ggWidth, gg->Height, 
		GetBitMapAttr(iwt->iwt_WinScreen->RastPort.BitMap, BMA_DEPTH),
		BMF_CLEAR | BMF_DISPLAYABLE | BMF_MINPLANES,
		iwt->iwt_WinScreen->RastPort.BitMap);
	if (NULL == rp->BitMap)
		return NULL;

	DoMethod(in->in_Icon, IDTM_Draw, iwt->iwt_WinScreen, 
		NULL, rp, iwt->iwt_WinDrawInfo, 
		0, 0,
		IODRAWF_AbsolutePos | IODRAWF_NoText | IODRAWF_NoAlpha);

	GetAttr((gg->Flags & GFLG_SELECTED) ? IDTA_MaskBM_Selected : IDTA_MaskBM_Normal, 
		in->in_Icon, (APTR) &MaskBMSelected);

	d1(KPrintF("%s/%s/%ld: MaskBMSelected=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskBMSelected));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

	if (MaskBMSelected)
		{
		// adapt size of mask plane (Width,Height) to icon size (ggWidth,gg->Height)

		MaskBM = AllocBitMap(ggWidth, gg->Height, 1, BMF_MINPLANES | BMF_CLEAR, NULL);

		if (MaskBM)
			{
			d1(KPrintF("%s/%s/%ld: Mask=%08lx  x=%ld  y=%ld  xb=%ld  yb=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, MaskBMSelected, \
				gg->LeftEdge, gg->TopEdge, \
				gg->BoundsLeftEdge, gg->BoundsTopEdge,
				Width, Height));

			d1(DumpBitMap(MaskBMSelected));

			BltBitMap(MaskBMSelected,
				0, 0,
                                MaskBM,
				LeftSpace, TopSpace,
				Width, Height,
				ABC | ABNC, ~0, NULL);

			d1(DumpBitMap(MaskBM));
#if 0
			{
			struct RastPort rp;

			InitRastPort(&rp);
			rp.BitMap = MaskBM;
			SetRast(&rp, 1);
			}
#endif
			MaskPlane = MaskBM->Planes[0];

			d1(KPrintF("%s/%s/%ld: MaskPlane: %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", \
				__FILE__, __FUNC__, __LINE__, MaskPlane[0], MaskPlane[1], MaskPlane[2], MaskPlane[3],\
				MaskPlane[4], MaskPlane[5], MaskPlane[6], MaskPlane[7]); \
			kprintf("%s/%s/%ld: MaskPlane: %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", \
				__FILE__, __FUNC__, __LINE__, MaskPlane[8], MaskPlane[9], MaskPlane[10], MaskPlane[11], \
				MaskPlane[12], MaskPlane[13], MaskPlane[14], MaskPlane[15]));
			}
		}

	GetAttr(IDTA_AlphaChannel, in->in_Icon, (APTR) &AlphaChannel);

	d1(if (AlphaChannel)\
		{\
		kprintf("%s/%s/%ld: AlphaChannel: %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", \
			__FILE__, __FUNC__, __LINE__, AlphaChannel[0], AlphaChannel[1], AlphaChannel[2], AlphaChannel[3],\
			AlphaChannel[4], AlphaChannel[5], AlphaChannel[6], AlphaChannel[7]);\
		kprintf("%s/%s/%ld: AlphaChannel: %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", \
			__FILE__, __FUNC__, __LINE__, AlphaChannel[8], AlphaChannel[9], AlphaChannel[10], AlphaChannel[11],\
			AlphaChannel[12], AlphaChannel[13], AlphaChannel[14], AlphaChannel[15]);\
		kprintf("%s/%s/%ld: AlphaChannel: %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", \
			__FILE__, __FUNC__, __LINE__, AlphaChannel[16], AlphaChannel[17], AlphaChannel[18], AlphaChannel[19],\
			AlphaChannel[20], AlphaChannel[21], AlphaChannel[22], AlphaChannel[23]);\
		kprintf("%s/%s/%ld: AlphaChannel: %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", \
			__FILE__, __FUNC__, __LINE__, AlphaChannel[24], AlphaChannel[25], AlphaChannel[26], AlphaChannel[27],\
			AlphaChannel[28], AlphaChannel[29], AlphaChannel[30], AlphaChannel[31]);\
		});

	bob = internalAddBob(iwt->iwt_myDragHandle, 
		BobList,
		rp->BitMap, MaskPlane, AlphaChannel,
		ggWidth, gg->Height,
		gg->BoundsWidth, gg->BoundsHeight,
		gg->LeftEdge - gg->BoundsLeftEdge,
		x0, y0
		);

	if (MaskBM)
		FreeBitMap(MaskBM);

	FreeBitMap(rp->BitMap);

	return bob;
}


static void InitDrag_ImageAndText(struct DragNode *dgn, 
	struct ScaBob **BobList,
	struct RastPort *rp, struct internalScaWindowTask *iwt,
	LONG x0, LONG y0)
{
	struct ScaIconNode *in = dgn->drgn_iconnode;
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	const UBYTE *AlphaChannel = NULL;
	struct BitMap *allocBM;
	struct BitMap *MaskBM;

	rp->BitMap = allocBM = AllocBitMap(gg->BoundsWidth, gg->BoundsHeight, 
		GetBitMapAttr(iwt->iwt_WinScreen->RastPort.BitMap, BMA_DEPTH),
		BMF_CLEAR | BMF_DISPLAYABLE | BMF_MINPLANES,
		iwt->iwt_WinScreen->RastPort.BitMap);
	if (NULL == rp->BitMap)
		return;
	
	DoMethod(in->in_Icon, IDTM_Draw, iwt->iwt_WinScreen, 
		NULL, rp, iwt->iwt_WinDrawInfo, 
		0, 0, IODRAWF_AbsolutePos | IODRAWF_NoAlpha);

	GetAttr(IDTA_AlphaChannel, in->in_Icon, (APTR) &AlphaChannel);

	MaskBM = AllocBitMap(gg->BoundsWidth, gg->BoundsHeight, 1, BMF_CLEAR, NULL);
	rp->BitMap = MaskBM;
	if (MaskBM)
		{
		struct BitMap *MaskBMSelected = NULL;

		SetAPen(rp, 1);
		SetBPen(rp, 1);

		GetAttr((gg->Flags & GFLG_SELECTED) ? IDTA_MaskBM_Selected : IDTA_MaskBM_Normal, 
			in->in_Icon, (APTR) &MaskBMSelected);

		d1(kprintf("%s/%s/%ld: Mask=%08lx  x=%ld  y=%ld  xb=%ld  yb=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, MaskBMSelected, \
			gg->LeftEdge, gg->TopEdge, \
			gg->BoundsLeftEdge, gg->BoundsTopEdge,
			gg->Width, gg->Height));

		if (MaskBMSelected)
			{
			LONG LeftSpace, TopSpace, RightSpace, BottomSpace;
			WORD Width, Height;

			LeftSpace = TopSpace = RightSpace = BottomSpace = 0;

			Width = gg->Width - LeftSpace - RightSpace;
			Height = gg->Height - TopSpace - BottomSpace;

			BltBitMap(MaskBMSelected, 0, 0, MaskBM,
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

			RectFill(rp, gg->LeftEdge - gg->BoundsLeftEdge,
				gg->TopEdge - gg->BoundsTopEdge,
				gg->LeftEdge - gg->BoundsLeftEdge + gg->Width - 1,
				gg->TopEdge - gg->BoundsTopEdge + gg->Height - 1);
			}
// .maskok2:
		SetAttrs(in->in_Icon,
			IDTA_TextPen, 1,
			IDTA_TextPenSel, 1,
			IDTA_TextPenShadow, 1,
			IDTA_TextPenOutline, 1,
			IDTA_TextDrawMode, JAM1,
			TAG_END);

		// add icon text to mask
		DoMethod(in->in_Icon, IDTM_Draw, iwt->iwt_WinScreen, 
			NULL, rp, iwt->iwt_WinDrawInfo, 0, 0, 
			IODRAWF_NoImage | IODRAWF_AbsolutePos | IODRAWF_NoAlpha);

		// restore original settings
		SetAttrs(in->in_Icon,
			IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
			IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
			IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
			IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
			IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
			TAG_END);

		SetABPenDrMd(rp, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawModeSel);
		}

// .maskok:
	internalAddBob(iwt->iwt_myDragHandle, BobList, 
		allocBM, rp->BitMap->Planes[0], AlphaChannel,
		gg->BoundsWidth, gg->BoundsHeight,
		gg->BoundsWidth, gg->BoundsHeight,
		0,
		x0, y0
		);

	if (MaskBM)
		FreeBitMap(MaskBM);
	FreeBitMap(allocBM);
}


void RestoreDragIcons(struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (IsIwtViewByIcon(iwt) && CurrentPrefs.pref_AutoRemoveFlag && iwt->iwt_RemIconsFlag && iwt->iwt_DragIconList)
		{
		struct ScaIconNode *FirstIcon = iwt->iwt_DragIconList;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		while (iwt->iwt_DragIconList)
			{
			d1(kprintf("%s/%s/%ld: Icon=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragIconList, GetIconName(iwt->iwt_DragIconList)));

			SCA_MoveIconNode(&iwt->iwt_DragIconList, &iwt->iwt_WindowTask.wt_IconList, iwt->iwt_DragIconList);
			}

		RefreshIconList(iwt, FirstIcon, NULL);
		}
	iwt->iwt_RemIconsFlag = FALSE;
}


void DrawDrag(ULONG DrawFlags, struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (IsIwtViewByIcon(iwt) && CurrentPrefs.pref_AutoRemoveFlag && !iwt->iwt_RemIconsFlag)
		{
		struct DragNode *dn;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		for (dn = iwt->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: Icon=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, dn->drgn_iconnode, GetIconName(dn->drgn_iconnode)));

			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &iwt->iwt_DragIconList, dn->drgn_iconnode);
			}

		ScalosObtainSemaphore(&iwt->iwt_UpdateSemaphore);

		RemoveIcons(iwt, &iwt->iwt_DragIconList);
		DrawIconListShadows(iwt, &iwt->iwt_DragIconList);

		ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);

		iwt->iwt_RemIconsFlag = TRUE;
		}

	if (!(DrawFlags & SCAF_Drag_NoDrawDrag))
		{
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		SCA_DrawDrag(iwt->iwt_myDragHandle, iwt->iwt_WinScreen->MouseX,
			iwt->iwt_WinScreen->MouseY, DrawFlags & ~SCAF_Drag_NoDrawDrag);
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void DrawIconListShadows(struct internalScaWindowTask *iwt, struct ScaIconNode **IconList)
{
	struct ScaIconNode *in;
	ULONG ScreenDepth;
	struct RastPort rpMask;
	struct RastPort rp;

	d1(kprintf("\n" "%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	Scalos_InitRastPort(&rp);
	Scalos_SetFont(&rp, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

	Scalos_InitRastPort(&rpMask);
	Scalos_SetFont(&rpMask, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

	ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	iwt->iwt_IconShadowVisible = TRUE;

	for (in = *IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
		DrawIconObjectTransparent(iwt, in->in_Icon, ScreenDepth, &rp, &rpMask, 
			CurrentPrefs.pref_IconShadowTransparency, CurrentPrefs.pref_RealTransFlag);
		}

	Scalos_DoneRastPort(&rpMask);
	Scalos_DoneRastPort(&rp);
}


void DragRefreshIcons(struct internalScaWindowTask *iwt)
{
	struct RastPort *rp = iwt->iwt_WindowTask.wt_Window->RPort;
	struct DragNode *dn;

	SetABPenDrMd(rp, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawMode);

	for (dn = iwt->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		{
		if (dn->drgn_icon)
			{
			DoMethod((Object *) dn->drgn_icon, 
				IDTM_Draw, iwt->iwt_WinScreen, 
				iwt->iwt_WindowTask.wt_Window, rp, 
				iwt->iwt_WinDrawInfo, 
				iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset,
				iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset, 
				0
				);
			}
		}
}


LIBFUNC_P8(BOOL, sca_AddBob,
	A0, struct DragHandle *, dh, 
	A1, struct BitMap *, bm, 
	A2, APTR, Mask, 
	D0, ULONG, Width, 
	D1, ULONG, Height, 
	D2, LONG, XOffset, 
	D3, LONG, YOffset,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	return (BOOL) (internalAddBob(dh, 
		&dh->drgh_boblist, 
		bm, Mask, NULL, 
		Width, Height, 
		Width, Height, 
		0,
		XOffset, YOffset) != NULL);
}
LIBFUNC_END


static struct ScaBob *internalAddBob(struct DragHandle *dh, 
	struct ScaBob **BobList,
	struct BitMap *bm, APTR Mask, const UBYTE *AlphaChannel,
	ULONG Width, ULONG Height, 
	ULONG BoundsWidth, ULONG BoundsHeight,
	WORD LeftBorder,
	LONG XOffset, LONG YOffset)
{
	struct ScaBob *Result = NULL;
	struct ScaBob *bob;

	if (dh->drgh_flags & DRGHF_CustomBob)
		{
		return sca_AddBobCustom((struct DragHandle *) dh, BobList, bm, 
			Mask, AlphaChannel, 
			Width, Height, 
			BoundsWidth, BoundsHeight,
			LeftBorder,
			XOffset, YOffset);
		}

	d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, XOffset, YOffset, Width, Height));

	do	{
		struct BitMap tempBM;
		ULONG n, PlaneSize;
		ULONG AllocLen;
		UBYTE *bp;
		UWORD Pattern, EndMask;
		UWORD *pw1, *pw2, *pwm;
		UWORD y, xMax;

		bob = (struct ScaBob *) SCA_AllocNode((struct ScalosNodeList *) BobList, 
				sizeof(struct ScaBob) - sizeof(struct MinNode));
		if (NULL == bob)
			break;

		bob->scbob_draghandle = dh;
		bob->scbob_width = Width;
		bob->scbob_height = Height;
		bob->scbob_BoundsWidth = BoundsWidth;
		bob->scbob_BoundsHeight = BoundsHeight;
		bob->scbob_LeftBorder = LeftBorder;
		bob->scbob_x = XOffset;
		bob->scbob_y = YOffset;
		bob->scbob_Bob.scbob_Bob1.scbob1_bob.BobVSprite = &bob->scbob_Bob.scbob_Bob1.scbob1_vsprite;
		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.VSBob = &bob->scbob_Bob.scbob_Bob1.scbob1_bob;

		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.Width = ((Width + 15) & ~15) / 16;
		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.Height = Height;
		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.Depth = bm->Depth;

		tempBM.BytesPerRow = ((Width + 15) & ~15) / 8;
		tempBM.Rows = Height;
		tempBM.Flags = 0;
		tempBM.pad = 0;
		tempBM.Depth = bm->Depth;

		PlaneSize = tempBM.BytesPerRow * tempBM.Rows;
		AllocLen = PlaneSize * bm->Depth;

		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.ImageData = AllocVec(AllocLen,
			 MEMF_PUBLIC | ChipMemAttr() | MEMF_CLEAR);
		if (NULL == bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.ImageData)
			break;

		bp = (UBYTE *) bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.ImageData;
		for (n=0; n<8; n++)
			{
			if (n < bm->Depth)
				{
				tempBM.Planes[n] = bp;
				bp += PlaneSize;
				}
			else
				tempBM.Planes[n] = NULL;
			}

		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.PlanePick = ~(~0 << bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.Depth);

		BltBitMap(bm, 0, 0, &tempBM, 0, 0, 
			bob->scbob_width, bob->scbob_height,
			ABC | ABNC, ~0, NULL);

		bob->scbob_Bob.scbob_Bob1.scbob1_bob.SaveBuffer = AllocVec(AllocLen, MEMF_PUBLIC | ChipMemAttr());
		if (NULL == bob->scbob_Bob.scbob_Bob1.scbob1_bob.SaveBuffer)
			break;

		bob->scbob_Bob.scbob_Bob1.scbob1_shadow1 = AllocVec(PlaneSize, MEMF_PUBLIC | ChipMemAttr());
		if (NULL == bob->scbob_Bob.scbob_Bob1.scbob1_shadow1)
			break;

		bob->scbob_Bob.scbob_Bob1.scbob1_shadow2 = AllocVec(PlaneSize, MEMF_PUBLIC | ChipMemAttr());
		if (NULL == bob->scbob_Bob.scbob_Bob1.scbob1_shadow2)
			break;

		xMax = ((bob->scbob_width + 15) & ~15) / 16;
		Pattern = 0xaaaa;

		pw1 = (UWORD *) bob->scbob_Bob.scbob_Bob1.scbob1_shadow1;
		pw2 = (UWORD *) bob->scbob_Bob.scbob_Bob1.scbob1_shadow2;
		pwm = (UWORD *) Mask;

		d1(kprintf("%s/%s/%ld: PlaneSize=%ld  Width=%ld  xMax=%ld\n", __FILE__, __FUNC__, __LINE__, PlaneSize, Width, xMax));

		EndMask = ~0 << (((bob->scbob_width + 15) & ~15) - bob->scbob_width);

		for (y=0; y<bob->scbob_height; y++)
			{
			UWORD x;

			for (x=0; x < xMax - 1; x++)
				{
				*pw1++ = 0xffff;
				*pw2++ = Pattern;

				if (Mask)
					{
					pw1[-1] &= *pwm;
					pw2[-1] &= *pwm;
					pwm++;
					}
				}
			*pw1++ = EndMask;
			*pw2 = EndMask;
			*pw2++ &= Pattern;

			if (Mask)
				{
				pw1[-1] &= *pwm;
				pw2[-1] &= *pwm;
				pwm++;
				}

			Pattern ^= 0xffff;
			}

		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.Flags = SAVEBACK | OVERLAY;
		bob->scbob_Bob.scbob_Bob1.scbob1_bob.ImageShadow = bob->scbob_Bob.scbob_Bob1.scbob1_shadow1;

		// only add bobs which are to be added to drgh_boblist
		// (e.g. do not add special bobs !!)
		if (BobList == &dh->drgh_boblist)
			AddBob(&bob->scbob_Bob.scbob_Bob1.scbob1_bob, &dh->drgh_screen->RastPort);

		Result = bob;
		bob = NULL;
		} while (0);

	if (bob)
		FreeBobNode((struct ScalosNodeList *) BobList, bob);

	return Result;
}


static void FreeBobNode(struct ScalosNodeList *nodeList, struct ScaBob *bob)
{
	if (NULL == bob)
		return;

	if (bob->scbob_Bob.scbob_Bob1.scbob1_shadow1)
		{
		FreeVec(bob->scbob_Bob.scbob_Bob1.scbob1_shadow1);
		bob->scbob_Bob.scbob_Bob1.scbob1_shadow1 = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob1.scbob1_shadow2)
		{
		FreeVec(bob->scbob_Bob.scbob_Bob1.scbob1_shadow2);
		bob->scbob_Bob.scbob_Bob1.scbob1_shadow2 = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob1.scbob1_bob.SaveBuffer)
		{
		FreeVec(bob->scbob_Bob.scbob_Bob1.scbob1_bob.SaveBuffer);
		bob->scbob_Bob.scbob_Bob1.scbob1_bob.SaveBuffer = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.ImageData)
		{
		FreeVec(bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.ImageData);
		bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.ImageData = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel
		&& (bob->scbob_Flags & BOBFLAGF_FreeAlphaChannel))
		{
		ScalosFree((UBYTE *) bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel);
		bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel = NULL;
		bob->scbob_Flags &= ~BOBFLAGF_FreeAlphaChannel;
		}

	SCA_FreeNode(nodeList, &bob->scbob_Node);
}


static void FreeCustomBobNode(struct ScalosNodeList *nodeList, struct ScaBob *bob)
{
	if (NULL == bob)
		return;

	if (bob->scbob_Bob.scbob_Bob2.scbob2_bitmap)
		{
		FreeBitMap(bob->scbob_Bob.scbob_Bob2.scbob2_bitmap);
		bob->scbob_Bob.scbob_Bob2.scbob2_bitmap = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap)
		{
		FreeBitMap(bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap);
		bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2)
		{
		FreeBitMap(bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2);
		bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2 = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob2.scbob2_shadow1)
		{
		FreeBitMap(bob->scbob_Bob.scbob_Bob2.scbob2_shadow1);
		bob->scbob_Bob.scbob_Bob2.scbob2_shadow1 = NULL;
		}
	if (bob->scbob_Bob.scbob_Bob2.scbob2_shadow2)
		{
		FreeBitMap(bob->scbob_Bob.scbob_Bob2.scbob2_shadow2);
		bob->scbob_Bob.scbob_Bob2.scbob2_shadow2 = NULL;
		}

	SCA_FreeNode(nodeList, &bob->scbob_Node);
}


static struct ScaBob *sca_AddBobCustom(struct DragHandle *dh, 
	struct ScaBob **BobList,
	struct BitMap *bm, APTR Mask, const UBYTE *AlphaChannel,
	ULONG Width, ULONG Height, 
	ULONG BoundsWidth, ULONG BoundsHeight, 
	WORD LeftBorder,
	LONG XOffset, LONG YOffset)
{
	struct ScaBob *bob;
	struct ScaBob *Result = NULL;
	struct BitMap *tempBM = NULL;

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, XOffset, YOffset, Width, Height));

	do	{
		static UWORD Pattern[] = { 0xaaaa, 0x5555 };
		struct RastPort rpMask;

		bob = AllocBob2(dh, (struct ScalosNodeList *) BobList,
			XOffset, YOffset, Width, Height, BoundsWidth, BoundsHeight);
		if (NULL == bob)
			break;

		bob->scbob_LeftBorder = LeftBorder;
		bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel = AlphaChannel;

		tempBM = AllocBitMap(bob->scbob_width, bob->scbob_height, 1, BMF_MINPLANES | BMF_CLEAR, NULL);
		if (NULL == tempBM)
			break;

		InitRastPort(&rpMask);
		rpMask.BitMap = tempBM;

		d1(KPrintF("%s/%s/%ld: Mask=%08lx  BytesPerRow=%ld  Rows=%ld\n", __FILE__, __FUNC__, __LINE__, Mask, tempBM->BytesPerRow, tempBM->Rows));

		if (Mask)
			{
			SetABPenDrMd(&rpMask, 1, 0, JAM2);
			BltTemplate((const PLANEPTR) Mask,
				0,
				2 * ((bob->scbob_width + 15) / 16),
				&rpMask,
				0, 0,
				bob->scbob_width,
				bob->scbob_height );
			}
		else
			{
			SetRast(&rpMask, 1);
			}

		d1(DumpBitMap(tempBM));
		d1(KPrintF("%s/%s/%ld: Mask=%04lx %04lx %04lx %04lx %04lx %04lx %04lx %04lx \n", __FILE__, __FUNC__, __LINE__, \
			((UWORD *) Mask)[0], ((UWORD *) Mask)[1], ((UWORD *) Mask)[2], ((UWORD *) Mask)[3], \
			((UWORD *) Mask)[4], ((UWORD *) Mask)[5], ((UWORD *) Mask)[6], ((UWORD *) Mask)[7]));

		d1(KPrintF("%s/%s/%ld: Planes[0]=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
			tempBM->Planes[0][0], tempBM->Planes[0][1], \
			tempBM->Planes[0][2], tempBM->Planes[0][3], \
			tempBM->Planes[0][4], tempBM->Planes[0][5], \
			tempBM->Planes[0][6], tempBM->Planes[0][7]));

		BlitBob(bob, tempBM, bob->scbob_Bob.scbob_Bob2.scbob2_shadow1);

		SetABPenDrMd(&rpMask, 0, 1, JAM1);
		SetAfPt(&rpMask, Pattern, 2);
		RectFill(&rpMask,
			0, 0,
			bob->scbob_width - 1, bob->scbob_height - 1);

		d1(DumpBitMap(tempBM));
		d1(KPrintF("%s/%s/%ld: Planes[0]=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
			tempBM->Planes[0][0], tempBM->Planes[0][1], \
			tempBM->Planes[0][2], tempBM->Planes[0][3], \
			tempBM->Planes[0][4], tempBM->Planes[0][5], \
			tempBM->Planes[0][6], tempBM->Planes[0][7]));

		BlitBob(bob, tempBM, bob->scbob_Bob.scbob_Bob2.scbob2_shadow2);

		d1(kprintf("%s/%s/%ld: Shadow1=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][0], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][1], \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][2], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][3], \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][4], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][5], \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][6], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0][7]));
		d1(kprintf("%s/%s/%ld: Shadow2=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][0], bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][1], \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][2], bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][3], \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][4], bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][5], \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][6], bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0][7]));

		BlitBob(bob, bm, bob->scbob_Bob.scbob_Bob2.scbob2_bitmap);

		SCA_SortNodes((struct ScalosNodeList *) BobList, &CompareLeftUpHook, SCA_SortType_Best);

		Result = bob;
		bob = NULL;
		} while (0);

	if (bob)
		FreeCustomBobNode((struct ScalosNodeList *) BobList, bob);
	if (tempBM)
		FreeBitMap(tempBM);

	return Result;
}


static void BlitBob(struct ScaBob *bob, struct BitMap *bmSrc, struct BitMap *bmDest)
{
	BltBitMap(bmSrc, 0, 0,
		bmDest, 0, 0,
		bob->scbob_width, bob->scbob_height,
		ABC | ABNC, ~0, NULL);
	WaitBlit();
}


static LONG CompareLeftUpFunc(const struct ScaBob *bob1, const struct ScaBob *bob2)
{
	d1(kprintf("%s/%s/%ld: bob2=%08lx  bob2=%08lx\n", __FILE__, __FUNC__, __LINE__, bob1, bob2));

	if (bob2->scbob_x < bob1->scbob_x)
		return 1;
	else if (bob2->scbob_x > bob1->scbob_x)
		return -1;

	if (bob2->scbob_y < bob1->scbob_y)
		return 1;
	else if (bob2->scbob_y > bob1->scbob_y)
		return -1;

	return 0;
}


BOOL SuspendDrag(struct DragHandle *dh, struct internalScaWindowTask *iwt)
{
	BOOL WasLocked = FALSE;

	d1(kprintf("%s/%s/%ld: START dh=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, dh, iwt->iwt_WinTitle));

	if (dh)
		{
		if (!(dh->drgh_flags & DRGHF_LockSuspended))
			{
			ClassHideDragBobs(iwt, dh);
			SetWindowPointer(iwt->iwt_WindowTask.wt_Window,
				WA_BusyPointer, TRUE,
				WA_PointerDelay, 0,
				TAG_END);
			WasLocked = SCA_UnlockDrag(dh);
			dh->drgh_flags |= DRGHF_LockSuspended;
			}
		}

	d1(kprintf("%s/%s/%ld: END dh=%08lx  WasLocked=%ld\n", __FILE__, __FUNC__, __LINE__, dh, WasLocked));

	return WasLocked;
}


void ResumeDrag(struct DragHandle *dh, struct internalScaWindowTask *iwt, BOOL wasLocked)
{
	d1(kprintf("%s/%s/%ld: START dh=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, dh, iwt->iwt_WinTitle));

	if (dh)
		{
		if (dh->drgh_flags & DRGHF_LockSuspended)
			{
			SetWindowPointer(iwt->iwt_WindowTask.wt_Window,
				WA_Pointer, NULL,
				TAG_END);
			dh->drgh_flags &= ~DRGHF_LockSuspended;
			ReLockDrag(dh, iwt, wasLocked);
			}
		}
	else
		{
		SetWindowPointer(iwt->iwt_WindowTask.wt_Window,
			WA_Pointer, NULL,
			TAG_END);
		}
	d1(kprintf("%s/%s/%ld: END dh=%08lx\n", __FILE__, __FUNC__, __LINE__, dh));
}


void ReLockDrag(struct DragHandle *dh, struct internalScaWindowTask *iwt, BOOL wasLocked)
{
	d1(kprintf("%s/%s/%ld: START dh=%08lx\n", __FILE__, __FUNC__, __LINE__, dh));

	if (wasLocked && (iInfos.xii_GlobalDragHandle == dh) && dh)
		{
		BOOL Locked2;
		BOOL Locked1 = dh->drgh_flags & DRGHF_LayersLocked;

		d1(kprintf("%s/%s/%ld: dh=%08lx  drgh_flags=%08lx\n", __FILE__, __FUNC__, __LINE__, dh, dh->drgh_flags));

#if defined(__MORPHOS__)
		if (DOSBase->dl_lib.lib_Version >= 51)
			{
			WaitTOF();
			if (iwt)
				WaitBOVP(&iwt->iwt_WinScreen->ViewPort);
			}
#endif //defined(__MORPHOS__)
		SCA_LockDrag(dh);

		Locked2 = dh->drgh_flags & DRGHF_LayersLocked;

		if (iwt && Locked2 && !Locked1)
			{
			SCA_DrawDrag(iwt->iwt_myDragHandle,
				iwt->iwt_WinScreen->MouseX,
				iwt->iwt_WinScreen->MouseY,
				0);
			}

		d1(kprintf("%s/%s/%ld: dh=%08lx  drgh_flags=%08lx\n", __FILE__, __FUNC__, __LINE__, dh, dh->drgh_flags));
		}
	d1(kprintf("%s/%s/%ld: END dh=%08lx\n", __FILE__, __FUNC__, __LINE__, dh));
}


LIBFUNC_P2(void, sca_LockDrag,
	A0, struct DragHandle *, dh,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	if (dh)
		{
		d1(kprintf("%s/%s/%ld: dh=%08lx  drgh_flags=%08lx  Task=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
			dh, dh->drgh_flags, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

		if (!(dh->drgh_flags & DRGHF_LockSuspended) && !(dh->drgh_flags & DRGHF_LayersLocked))
			{
			d1(kprintf("%s/%s/%ld: dh=%08lx  ScaLockScreenLayers\n", __FILE__, __FUNC__, __LINE__, dh));
			ScalosObtainSemaphore(&LayersSema);
			ScaLockScreenLayers();
			dh->drgh_flags |= DRGHF_LayersLocked;
			d1(kprintf("%s/%s/%ld: dh=%08lx  ScaLockScreenLayers\n", __FILE__, __FUNC__, __LINE__, dh));
			}
		}
}
LIBFUNC_END


LIBFUNC_P2(ULONG, sca_UnlockDrag,
	A0, struct DragHandle *, dh,
	A6, struct ScalosBase *, ScalosBase)
{
	ULONG WasLocked = FALSE;

	(void) ScalosBase;

	if (dh)
		{
		d1(kprintf("%s/%s/%ld: dh=%08lx  drgh_flags=%08lx  Task=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
			dh, dh->drgh_flags, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));
		WasLocked = TRUE;

		if (dh->drgh_flags & DRGHF_LayersLocked)
			{
			d1(kprintf("%s/%s/%ld: dh=%08lx  ScaUnlockScreenLayers\n", __FILE__, __FUNC__, __LINE__, dh));
			dh->drgh_flags &= ~DRGHF_LayersLocked;
			ScaUnlockScreenLayers();
			ScalosReleaseSemaphore(&LayersSema);
			d1(kprintf("%s/%s/%ld: dh=%08lx  ScaUnlockScreenLayers\n", __FILE__, __FUNC__, __LINE__, dh));
			}
		}

	d1(kprintf("%s/%s/%ld: END  WasLocked=%ld\n", __FILE__, __FUNC__, __LINE__, WasLocked));

	return WasLocked;
}
LIBFUNC_END


LIBFUNC_P5(void, sca_DrawDrag,
	A0, struct DragHandle *, dh,
	D0, LONG, XOffset, 
	D1, LONG, YOffset,
	D2, ULONG, Flags,
	A6, struct ScalosBase *, ScalosBase)
{
	(void) ScalosBase;

	d1(KPrintF("%s/%s/%ld: dh=%08lx  x=%ld  y=%ld  Flags=%08lx\n", \
		__FILE__, __FUNC__, __FUNC__, __LINE__, dh, XOffset, YOffset, Flags));

	if (NULL == dh)
		return;

	if (dh->drgh_flags & DRGHF_LockSuspended)
		return;

	ScalosObtainSemaphore(&dh->drgh_BobListSemaphore);

	switch (CurrentPrefs.pref_DragTranspMode)
		{
	case DRAGTRANSPMODE_SolidAndTransp:
		Flags ^= SCAF_Drag_Transparent;
		break;
	case DRAGTRANSPMODE_Solid:
		Flags &= ~SCAF_Drag_Transparent;
		break;
	case DRAGTRANSPMODE_Transp:
		Flags |= SCAF_Drag_Transparent;
		break;
		}

	d1(KPrintF("%s/%s/%ld: dh=%08lx  x=%ld  y=%ld  Flags=%08lx\n", \
		__FILE__, __FUNC__, __FUNC__, __LINE__, dh, XOffset, YOffset, Flags));

	if ((dh->drgh_flags & DRGHF_CustomBob) && !(dh->drgh_flags & DRGHF_BobsOptimized))
		{
		OptimizeBobs((struct DragHandle *) dh);
		dh->drgh_flags |= DRGHF_BobsOptimized;
		}

	if (!(dh->drgh_flags & DRGHF_LayersLocked))
		{
		SCA_LockDrag(dh);
		}

	if (dh->drgh_flags & DRGHF_CustomBob)
		{
		sca_DrawDragCustom((struct DragHandle *) dh, XOffset, YOffset, Flags);
		}
	else
		{
		struct ScaBob *bob;

		CheckSpecialBob(dh, Flags & SCAF_Drag_NoDropHere, DRGHF_NoDropIndicatorVisible, 
			&dh->drgh_Bob_NoDropIndicator, NAME_FORBIDDEN, NAME_FORBIDDEN_FALLBACK);
		CheckSpecialBob(dh, Flags & SCAF_Drag_IndicateCopy, DRGHF_CopyIndicatorVisible, 
			&dh->drgh_Bob_CopyIndicator, NAME_COPYING, NAME_COPYING_FALLBACK);
		CheckSpecialBob(dh, Flags & SCAF_Drag_IndicateMakeLink, DRGHF_MakeLinkIndicatorVisible, 
			&dh->drgh_Bob_MakeLinkIndicator, NAME_MAKELINK, NAME_MAKELINK_FALLBACK);
		CheckSpecialBob(dh, Flags & SCAF_Drag_ForceMove, DRGHF_ForceMoveIndicatorVisible, 
			&dh->drgh_Bob_ForceMoveIndicator, NAME_MOVING, NAME_MOVING_FALLBACK);

		for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
			{
			bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.X = XOffset + bob->scbob_x;
			bob->scbob_Bob.scbob_Bob1.scbob1_vsprite.Y = YOffset + bob->scbob_y;

			if ((Flags & SCAF_Drag_Transparent) && !(bob->scbob_Flags & BOBFLAGF_NeverTransparent))
				bob->scbob_Bob.scbob_Bob1.scbob1_bob.ImageShadow = bob->scbob_Bob.scbob_Bob1.scbob1_shadow2;
			else
				bob->scbob_Bob.scbob_Bob1.scbob1_bob.ImageShadow = bob->scbob_Bob.scbob_Bob1.scbob1_shadow1;
			}

		if ((Flags & SCAF_Drag_Hide) && !(dh->drgh_flags & DRGF_BobsHidden))
			{
			// Hide all bobs
			for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
				{
				RemIBob(&bob->scbob_Bob.scbob_Bob1.scbob1_bob, &dh->drgh_screen->RastPort, &dh->drgh_screen->ViewPort);
				}
			dh->drgh_flags |= DRGF_BobsHidden;
			}
		if (!(Flags & SCAF_Drag_Hide) && (dh->drgh_flags & DRGF_BobsHidden))
			{
			// Show hidden bobs
			for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
				{
				AddBob(&bob->scbob_Bob.scbob_Bob1.scbob1_bob, &dh->drgh_screen->RastPort);
				}
			dh->drgh_flags &= ~DRGF_BobsHidden;
			}

		SortGList(&dh->drgh_screen->RastPort);
		DrawGList(&dh->drgh_screen->RastPort, &dh->drgh_screen->ViewPort);
		}

	ScalosReleaseSemaphore(&dh->drgh_BobListSemaphore);
}
LIBFUNC_END


static void sca_DrawDragCustom(struct DragHandle *dh, LONG x, LONG y, ULONG Flags)
{
	struct ScaBob *bob;

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  drgh_flags=%08lx  Flags=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, x, y, dh->drgh_flags, Flags));

	dh->drgh_flags &= ~DRGHF_BlitRealTransp;

	if (Flags & SCAF_Drag_Hide)
		{
		x = NO_BOB_POSITION;
		y = NO_BOB_POSITION;
		dh->drgh_flags |= DRGF_BobsHidden;
		}

	if (dh->drgh_flags & DRGHF_RealTranspPossible
		&& (Flags & SCAF_Drag_Transparent))
		{
		dh->drgh_flags |= DRGHF_BlitRealTransp;
		d1(KPrintF("%s/%s/%ld: DRGHF_BlitRealTransp\n", __FILE__, __FUNC__, __LINE__));
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	CheckSpecialBobCustom(dh, Flags & SCAF_Drag_NoDropHere, DRGHF_NoDropIndicatorVisible, 
		&dh->drgh_Bob_NoDropIndicator, NAME_FORBIDDEN, NAME_FORBIDDEN_FALLBACK);
	CheckSpecialBobCustom(dh, Flags & SCAF_Drag_IndicateCopy, DRGHF_CopyIndicatorVisible, 
		&dh->drgh_Bob_CopyIndicator, NAME_COPYING, NAME_COPYING_FALLBACK);
	CheckSpecialBobCustom(dh, Flags & SCAF_Drag_IndicateMakeLink, DRGHF_MakeLinkIndicatorVisible, 
		&dh->drgh_Bob_MakeLinkIndicator, NAME_MAKELINK, NAME_MAKELINK_FALLBACK);
	CheckSpecialBobCustom(dh, Flags & SCAF_Drag_ForceMove, DRGHF_ForceMoveIndicatorVisible, 
		&dh->drgh_Bob_ForceMoveIndicator, NAME_MOVING, NAME_MOVING_FALLBACK);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (bob = dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
		{
		bob->scbob_Bob.scbob_Bob2.scbob2_xpos = x + bob->scbob_x;
		bob->scbob_Bob.scbob_Bob2.scbob2_ypos = y + bob->scbob_y;

		if ((Flags & SCAF_Drag_Transparent) 
				&& !(dh->drgh_flags & DRGHF_BlitRealTransp)
				&& !(bob->scbob_Flags & BOBFLAGF_NeverTransparent))
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow = bob->scbob_Bob.scbob_Bob2.scbob2_shadow2;
		else
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow = bob->scbob_Bob.scbob_Bob2.scbob2_shadow1;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (!(dh->drgh_flags & DRGHF_SpeedBitMapAlloc))
		{
		LONG MaxWidth = 0, MaxHeight = 0;

		for (bob = dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
			{
			if (bob->scbob_width > MaxWidth)
				MaxWidth = bob->scbob_width;
			if (bob->scbob_height > MaxHeight)
				MaxHeight = bob->scbob_height;
			}
		if (0 == MaxWidth)
			return;

		dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap = AllocBitMap(MaxWidth, MaxHeight,
			dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_bitmap->Depth,
			BMF_MINPLANES | BMF_DISPLAYABLE,
			dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_bitmap);
		if (NULL == dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap)
			return;

		dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2 = AllocBitMap(MaxWidth, MaxHeight,
			dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_bitmap->Depth,
			BMF_MINPLANES | BMF_DISPLAYABLE,
			dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_bitmap);
		if (NULL == dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2)
			return;

		dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer = AllocVec(MaxWidth * MaxHeight * sizeof(struct ARGB), MEMF_PUBLIC);
		if (NULL == dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer)
			return;

		dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2 = AllocVec(MaxWidth * MaxHeight * sizeof(struct ARGB), MEMF_PUBLIC);
		if (NULL == dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2)
			return;

		dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap = AllocBitMap(MaxWidth * 2, MaxHeight * 2,
			dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_bitmap->Depth,
			BMF_MINPLANES | BMF_DISPLAYABLE,
			dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_bitmap);
		if (NULL == dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap)
			return;

		dh->drgh_flags |= DRGHF_SpeedBitMapAlloc;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_lastxpos == dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_xpos &&
			dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_lastypos == dh->drgh_boblist->scbob_Bob.scbob_Bob2.scbob2_ypos &&
			dh->drgh_LastDragFlags == Flags)
		return;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
		{
		if (ClipBlitIn(dh->drgh_screen, 
			bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2,
			bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
			bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
			bob->scbob_width,
			bob->scbob_height))
			{
			RestoreBuffer(dh, 
				bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2,
				bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
				bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
				bob->scbob_width,
				bob->scbob_height);
			}
		else
			{
			// .nobackup:
			bob->scbob_Bob.scbob_Bob2.scbob2_xpos = (WORD) NO_BOB_POSITION;
			bob->scbob_Bob.scbob_Bob2.scbob2_ypos = (WORD) NO_BOB_POSITION;
			}
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, bob->scbob_Bob.scbob_Bob2.scbob2_xpos, bob->scbob_Bob.scbob_Bob2.scbob2_ypos));
		d1(kprintf("%s/%s/%ld: lastx=%ld  lasty=%ld\n", __FILE__, __FUNC__, __LINE__, bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos, bob->scbob_Bob.scbob_Bob2.scbob2_lastypos));
		d1(kprintf("%s/%s/%ld: lastx=%ld  isBobPos=%ld\n", __FILE__, __FUNC__, __LINE__, bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos, IsBobPosition(bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos)));

		if (IsBobPosition(bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos))
			{
			LONG dx = bob->scbob_Bob.scbob_Bob2.scbob2_xpos - bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos;
			LONG dy = bob->scbob_Bob.scbob_Bob2.scbob2_ypos - bob->scbob_Bob.scbob_Bob2.scbob2_lastypos;

			if (dx < 0)
				dx = -dx;
			if (dy < 0)
				dy = -dy;

			d1(kprintf("%s/%s/%ld: dx=%ld  dy=%ld\n", __FILE__, __FUNC__, __LINE__, dx, dy));

			if (dx >= bob->scbob_width || dy >= bob->scbob_height || !IsBobPosition(bob->scbob_Bob.scbob_Bob2.scbob2_xpos))
				{
				// .restore:

				// icons have been moved more than bob->scbob_width or bob->scbob_height

				d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
					bob->scbob_Bob.scbob_Bob2.scbob2_xpos, bob->scbob_Bob.scbob_Bob2.scbob2_ypos, bob->scbob_width, bob->scbob_height));

				// get background at new position
				ClipBlitIn(dh->drgh_screen, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
					bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
					bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
					bob->scbob_width,
					bob->scbob_height);

				RestoreBuffer(dh, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
					bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
					bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
					bob->scbob_width,
					bob->scbob_height);

				RestoreBuffer2(dh, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
					bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
					bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
					bob->scbob_width,
					bob->scbob_height);

				BltBitMap(bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap, 0, 0,
					dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2, 0, 0,
					bob->scbob_width,
					bob->scbob_height,
					ABC | ABNC, ~0, NULL);

				RestoreBuffer2(dh, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2,
					bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos, 
					bob->scbob_Bob.scbob_Bob2.scbob2_lastypos,
					bob->scbob_width,
					bob->scbob_height);

				// erase bob at old position
				ClipBlitOut(dh->drgh_screen, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2,
					bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos, 
					bob->scbob_Bob.scbob_Bob2.scbob2_lastypos,
					bob->scbob_width,
					bob->scbob_height);

				if (IsBobPosition(bob->scbob_Bob.scbob_Bob2.scbob2_xpos))
					{
					d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, bob->scbob_Bob.scbob_Bob2.scbob2_xpos, bob->scbob_Bob.scbob_Bob2.scbob2_ypos));

					// draw bob at new position
					ClipBlitOut(dh->drgh_screen, 
						dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
						bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
						bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
						bob->scbob_width,
						bob->scbob_height);
					}
				}
			else
				{
				// .oldyislower:
				LONG xStart, yStart;

				dx += bob->scbob_width;
				dy += bob->scbob_height;

				xStart = min(bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos, bob->scbob_Bob.scbob_Bob2.scbob2_xpos);
				yStart = min(bob->scbob_Bob.scbob_Bob2.scbob2_lastypos, bob->scbob_Bob.scbob_Bob2.scbob2_ypos);

				d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, xStart, yStart, dx, dy));

				// Copy from Screen to speedbitmap
				ClipBlitIn(dh->drgh_screen, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
					xStart, yStart,
					dx, dy);

				RestoreBuffer(dh, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
					xStart, yStart,
					dx, dy);

				RestoreBuffer2(dh, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
					xStart, yStart,
					dx, dy);

				d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, xStart, yStart));

				// Copy from speedbitmap to Screen
				ClipBlitOut(dh->drgh_screen, 
					dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
					xStart, yStart,
					dx, dy);
				}
			}
		else
			{
			// .norestore:
			ClipBlitIn(dh->drgh_screen, 
				dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
				bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
				bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
				bob->scbob_width,
				bob->scbob_height);

			BlitBob2(dh, bob, bob->scbob_Bob.scbob_Bob2.scbob2_bitmap, 0, 0, 0, 0, 
				bob->scbob_width, bob->scbob_height);

			d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, bob->scbob_Bob.scbob_Bob2.scbob2_xpos, bob->scbob_Bob.scbob_Bob2.scbob2_ypos));

			ClipBlitOut(dh->drgh_screen, 
				dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap,
				bob->scbob_Bob.scbob_Bob2.scbob2_xpos,
				bob->scbob_Bob.scbob_Bob2.scbob2_ypos,
				bob->scbob_width,
				bob->scbob_height);
			}
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
		{
		struct	BitMap *temp;

		bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos = bob->scbob_Bob.scbob_Bob2.scbob2_xpos;
		bob->scbob_Bob.scbob_Bob2.scbob2_lastypos = bob->scbob_Bob.scbob_Bob2.scbob2_ypos;

		temp = bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap;
		bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap = bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2;
		bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2 = temp;
		}

	dh->drgh_LastDragFlags = Flags;
}


static void RestoreBuffer(struct DragHandle *dh,
	struct BitMap *DestBM,
	LONG x, LONG y, LONG Width, LONG Height)
{
	LONG xMax, yMax;
	struct ScaBob *bob;

	d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

	xMax = x + Width - 1;
	yMax = y + Height - 1;

	for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: bob=%08lx  xpos=%ld  ypos=%ld  xMax=%ld  yMax=%ld\n", \
			__FILE__, __FUNC__, __LINE__, bob, bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos, bob->scbob_Bob.scbob_Bob2.scbob2_lastypos, xMax, yMax));

		if (IsBobPosition(bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos)
			&& bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos <= xMax
			&& bob->scbob_Bob.scbob_Bob2.scbob2_lastypos <= yMax
			&& (bob->scbob_width + bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos - 1) >= x
			&& (bob->scbob_height + bob->scbob_Bob.scbob_Bob2.scbob2_lastypos - 1) >= y)
			{
			LONG SrcX, SrcY, DstX, DstY, SizeX, SizeY;

			DstX = 0;
			SizeX = bob->scbob_width;
			SrcX = x - bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos;

			if (SrcX < 0)
				{
				SrcX = -SrcX;

				SwapLong(&SrcX, &DstX);

				SizeX = Width - DstX;

				if (SizeX > bob->scbob_width)
					SizeX = bob->scbob_width;
				}
			else
				{
				SizeX -= SrcX;
				if (SizeX > Width)
					SizeX = Width;
				}

			DstY = 0;
			SizeY = bob->scbob_height;
			SrcY = y - bob->scbob_Bob.scbob_Bob2.scbob2_lastypos;

			if (SrcY < 0)
				{
				SrcY = -SrcY;

				SwapLong(&SrcY, &DstY);

				SizeY = Height - DstY;

				if (SizeY > bob->scbob_height)
					SizeY = bob->scbob_height;
				}
			else
				{
				SizeY -= SrcY;
				if (SizeY > Height)
					SizeY = Height;
				}

			d1(kprintf("%s/%s/%ld: SrcX=%ld  SrcY=%ld  DstX=%ld  DstY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
				SrcX, SrcY, DstX, DstY, SizeX, SizeY));

			BltBitMap(bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap, SrcX, SrcY,
				DestBM, DstX, DstY,
				SizeX, SizeY,
				ABC | ABNC, ~0, NULL);
			}
		}
}


static void RestoreBuffer2(struct DragHandle *dh,
	struct BitMap *DestBM,
	LONG x, LONG y, LONG Width, LONG Height)
{
	struct ScaBob *bob;
	LONG xMax = x + Width - 1;
	LONG yMax = y + Height - 1;

	d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

	for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: bob=%08lx  xpos=%ld  ypos=%ld  xMax=%ld  yMax=%ld\n", \
			__FILE__, __FUNC__, __LINE__, bob, bob->scbob_Bob.scbob_Bob2.scbob2_xpos, bob->scbob_Bob.scbob_Bob2.scbob2_ypos, xMax, yMax));

		if (IsBobPosition(bob->scbob_Bob.scbob_Bob2.scbob2_xpos)
			&& bob->scbob_Bob.scbob_Bob2.scbob2_xpos <= xMax 
			&& bob->scbob_Bob.scbob_Bob2.scbob2_ypos <= yMax 
			&& (bob->scbob_Bob.scbob_Bob2.scbob2_xpos + bob->scbob_width - 1) >= x
			&& (bob->scbob_Bob.scbob_Bob2.scbob2_ypos + bob->scbob_height - 1) >= y)
			{
			LONG SrcX, SrcY, DstX, DstY, SizeX, SizeY;

			DstX = 0;
			SizeX = bob->scbob_width;
			SrcX = x - bob->scbob_Bob.scbob_Bob2.scbob2_xpos;

			if (SrcX < 0)
				{
				SrcX = -SrcX;

				SwapLong(&DstX, &SrcX);

				SizeX = Width - DstX;

				if (SizeX > bob->scbob_width)
					SizeX = bob->scbob_width;
				}
			else
				{
				SizeX -= SrcX;
				if (SizeX > Width)
					SizeX = Width;
				}

			DstY = 0;
			SizeY = bob->scbob_height;
			SrcY = y - bob->scbob_Bob.scbob_Bob2.scbob2_ypos;

			if (SrcY < 0)
				{
				SrcY = -SrcY;

				SwapLong(&DstY, &SrcY);

				SizeY = Height - DstY;

				if (SizeY > bob->scbob_height)
					SizeY = bob->scbob_height;
				}
			else
				{
				SizeY -= SrcY;
				if (SizeY > Height)
					SizeY = Height;
				}

			d1(kprintf("%s/%s/%ld: SrcX=%ld  SrcY=%ld  DstX=%ld  DstY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
				SrcX, SrcY, DstX, DstY, SizeX, SizeY));

			if (SizeX < bob->scbob_width || SizeY < bob->scbob_height)
				{
				// .doubleblit:
				d1(kprintf("%s/%s/%ld: .doubleblit\n", __FILE__, __FUNC__, __LINE__));

				BltBitMap(DestBM, DstX, DstY,
					dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap, SrcX, SrcY,
					SizeX, SizeY,
					ABC | ABNC, ~0, NULL);

				dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport.BitMap = dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap;

				BlitTrans(dh, bob, bob->scbob_Bob.scbob_Bob2.scbob2_bitmap,
					0, 0,
					0, 0,
					bob->scbob_width, bob->scbob_height,
					bob->scbob_Bob.scbob_Bob2.scbob2_shadow
					);

				d1(kprintf("%s/%s/%ld: SrcX=%ld  SrcY=%ld  DstX=%ld  DstY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
					SrcX, SrcY, DstX, DstY, SizeX, SizeY));

				BltBitMap(dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap, SrcX, SrcY,
					DestBM, DstX, DstY,
					SizeX, SizeY,
					ABC | ABNC, ~0, NULL);
				}
			else
				{
				dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport.BitMap = DestBM;

				d1(kprintf("%s/%s/%ld: SrcX=%ld  SrcY=%ld  DstX=%ld  DstY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
					SrcX, SrcY, DstX, DstY, SizeX, SizeY));
				d1(kprintf("%s/%s/%ld: SrcBM=%08lx  DestBM=%08lx  Mask=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					bob->scbob_Bob.scbob_Bob2.scbob2_bitmap, DestBM, bob->scbob_Bob.scbob_Bob2.scbob2_shadow->Planes[0]));

				BlitTrans(dh, bob, bob->scbob_Bob.scbob_Bob2.scbob2_bitmap,
					SrcX, SrcY,
					DstX, DstY,
					SizeX, SizeY,
					bob->scbob_Bob.scbob_Bob2.scbob2_shadow
					);
				}
			}
		}
}


// Blit from Screen to an internal Bitmap
static BOOL ClipBlitIn(struct Screen *Scr, struct BitMap *DestBM,
	LONG SrcX, LONG SrcY, LONG Width, LONG Height)
{
	LONG DstX, DstY;
	struct RastPort rp;

	d1(kprintf("%s/%s/%ld: SrcX=%ld  SrcY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		SrcX, SrcY, Width, Height));

	Scalos_InitRastPort(&rp);
	rp.BitMap = DestBM;

	DstX = -SrcX;
	if (DstX < 0)
		{
		LONG Right = (SrcX + Width - 1) - (Scr->Width - 1);

		if (Right >= 0)
			{
			Width -= Right;

			if (Width < 0)
				return FALSE;
			}

		DstX = 0;
		}
	else
		{
		Width -= DstX;

		if (Width < 0)
			return FALSE;

		SrcX = 0;
		}

	DstY = -SrcY;
	if (DstY < 0)
		{
		LONG Bottom = (SrcY + Height - 1) - (Scr->Height - 1);

		if (Bottom >= 0)
			{
			Height -= Bottom;

			if (Height < 0)
				return FALSE;
			}

		DstY = 0;
		}
	else
		{
		Height -= DstY;

		if (Height < 0)
			return FALSE;

		SrcY = 0;
		}

	d1(kprintf("%s/%s/%ld: SrcX=%ld  SrcY=%ld  DstX=%ld  DstY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		SrcX, SrcY, DstX, DstY, Width, Height));

	ClipBlit(&Scr->RastPort, SrcX, SrcY,
		&rp, DstX, DstY,
		Width, Height,
		ABC | ABNC);

	Scalos_DoneRastPort(&rp);

	return TRUE;
}


// Blit from an internal Bitmap to the Screen
static BOOL ClipBlitOut(struct Screen *Scr, struct BitMap *SrcBM,
	LONG DstX, LONG DstY, LONG Width, LONG Height)
{
	LONG SrcX, SrcY;
	struct RastPort rp;

	d1(kprintf("%s/%s/%ld: DstX=%ld  DstY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		DstX, DstY, Width, Height));

	Scalos_InitRastPort(&rp);
	rp.BitMap = SrcBM;

	SrcX = -DstX;
	if (SrcX < 0)
		{
		LONG Right = (DstX + Width - 1) - (Scr->Width - 1);

		if (Right >= 0)
			{
			Width -= Right;

			if (Width < 0)
				return FALSE;
			}

		SrcX = 0;
		}
	else
		{
		Width -= SrcX;

		if (Width < 0)
			return FALSE;

		DstX = 0;
		}

	SrcY = -DstY;
	if (SrcY < 0)
		{
		LONG Bottom = (DstY + Height - 1) - (Scr->Height - 1);

		if (Bottom >= 0)
			{
			Height -= Bottom;

			if (Height < 0)
				return FALSE;
			}

		SrcY = 0;
		}
	else
		{
		Height -= SrcY;

		if (Height < 0)
			return FALSE;

		DstY = 0;
		}

	d1(kprintf("%s/%s/%ld: DestRp=%08lx  SrcX=%ld  SrcY=%ld  DstX=%ld  DstY=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		&Scr->RastPort, SrcX, SrcY, DstX, DstY, Width, Height));

	ClipBlit(&rp, SrcX, SrcY,
		&Scr->RastPort, DstX, DstY,
		Width, Height,
		ABC | ABNC);

	Scalos_DoneRastPort(&rp);

	return TRUE;
}


static void BlitBob2(struct DragHandle *dh, 
	struct ScaBob *bob, struct BitMap *bm, 
	LONG SrcX, LONG SrcY, LONG DestX, LONG DestY, 
	LONG Width, LONG Height)
{
	dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport.BitMap = dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap;

	BlitTrans(dh, bob, bm, 
		SrcX, SrcY, DestX, DestY,
		Width, Height,
		bob->scbob_Bob.scbob_Bob2.scbob2_shadow);
}


// DestX, DestY : Offsets into RastPort->BitMap

static void BlitTrans(struct DragHandle *dh, 
	const struct ScaBob *bob, struct BitMap *SrcBM,
	LONG SrcX, LONG SrcY, LONG DestX, LONG DestY, 
	LONG Width, LONG Height, 
	const struct BitMap *MaskBM)
{
	d1(kprintf("%s/%s/%ld: SrcX=%ld  SrcY=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
		SrcX, SrcY, Width, Height));

	if (dh->drgh_flags & DRGHF_RealTranspPossible)
		{
		ULONG Transp;
		ULONG DestMod = Width * sizeof(struct ARGB);
		struct BitMap *oldBitMap;

		if ((dh->drgh_flags & DRGHF_BlitRealTransp) && !(bob->scbob_Flags & BOBFLAGF_NeverTransparent))
			Transp = (CurrentPrefs.pref_DragTransparency * 255) / 100;
		else
			Transp = ALPHA_OPAQUE;	// opaque

		oldBitMap = dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport.BitMap;
		dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport.BitMap = SrcBM;

		// read background from SrcBM into drgh_Specific.drghs_DragHandle2.drgh2_transbuffer
		ScalosReadPixelArray(dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer, DestMod,
			&dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport, SrcX, SrcY,
			Width, Height);

		dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport.BitMap = oldBitMap;

		// read icon image into drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2
		ScalosReadPixelArray(dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2, DestMod,
			&dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport, DestX, DestY,
			Width, Height);

		d1(kprintf("%s/%s/%ld: AlphaChannel=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel));

		if (bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel)
			{
			BlitARGBMaskAlpha(Width, Height, 
				dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer, 
				0, 0, 
				Width, 
				dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2, 0, 0,
				MaskBM,
				Transp, 
				bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel, 
				bob->scbob_LeftBorder, bob->scbob_BoundsWidth);
			}
		else
			{
			BlitARGBMask(Width, Height,
				dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer, 
				0, 0,
				Width, 
				dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2, 0, 0,
				MaskBM,
				Transp);
			}

		// write back to icon image
		ScalosWritePixelArray(dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2, DestMod,
			&dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport, DestX, DestY,
			Width, Height);
		}
	else
		{
		BltMaskBitMapRastPort(SrcBM, SrcX, SrcY,
			&dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport, DestX, DestY,
			Width, Height,
			ABC|ABNC|ANBC, MaskBM->Planes[0]);
		}
}


static void OptimizeBobs(struct DragHandle *dh)
{
	LONG xMin, yMin, xMax, yMax;
	struct ScaBob *bob;

	d1(kprintf("%s/%s/%ld: OptimizeBobs START\n", __FILE__, __FUNC__, __LINE__));

	xMin = yMin = SHRT_MAX;
	xMax = yMax = SHRT_MIN;

	bob = dh->drgh_boblist;
	if (bob && bob->scbob_Node.mln_Succ)
		{
		LONG Width, Height;
		struct ScaBob *newBob;
		struct ScaBob *nbList = NULL;
		BOOL NeedAlpha = FALSE;

		for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: bob=%08lx NoOptimize=%ld\n", __FILE__, __FUNC__, __LINE__, \
				bob, bob->scbob_Flags & BOBFLAGF_NoOptimize ));
			if (!(bob->scbob_Flags & BOBFLAGF_NoOptimize))
				{
				if (bob->scbob_x < xMin)
					xMin = bob->scbob_x;
				if (bob->scbob_y < yMin)
					yMin = bob->scbob_y;
				if (bob->scbob_x + bob->scbob_width > xMax)
					xMax = bob->scbob_x + bob->scbob_width;
				if (bob->scbob_y + bob->scbob_height > yMax)
					yMax = bob->scbob_y + bob->scbob_height;

				if (bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel)
					NeedAlpha = TRUE;
				}
			}

		d1(kprintf("%s/%s/%ld: xMin=%ld  xMax=%ld  yMin=%ld  yMax=%ld\n", __FILE__, __FUNC__, __LINE__, \
			xMin, xMax, yMin, yMax));

		Width = 1 + xMax - xMin;
		Height = 1 + yMax - yMin;

		newBob = AllocBob2(dh, (struct ScalosNodeList *)(APTR) &nbList, xMin, yMin, Width, Height, Width, Height);
		if (newBob)
			{
			struct RastPort rp;

			Scalos_InitRastPort(&rp);

			if (NeedAlpha)
				MergeBobsAlpha(&rp, dh, &nbList, newBob);
			else
				MergeBobs(&rp, dh, &nbList, newBob);

			FreeCustomBobList(&dh->drgh_boblist);
			dh->drgh_boblist = nbList;

			Scalos_DoneRastPort(&rp);
			}
		}

	d1(kprintf("%s/%s/%ld: OptimizeBobs END\n", __FILE__, __FUNC__, __LINE__));

	dh->drgh_flags |= DRGHF_BobsOptimized;
}


static void MergeBobs(struct RastPort *TempRp, struct DragHandle *dh, 
	struct ScaBob **NewBobList, struct ScaBob *NewBob)
{
	struct ScaBob *bob;

	for (bob=dh->drgh_boblist; bob; )
		{
		struct ScaBob *NextBob = (struct ScaBob *) bob->scbob_Node.mln_Succ;

		d1(kprintf("%s/%s/%ld: bob=%08lx NoOptimize=%ld\n", __FILE__, __FUNC__, __LINE__, \
			bob, bob->scbob_Flags & BOBFLAGF_NoOptimize ));

		if (bob->scbob_Flags & BOBFLAGF_NoOptimize)
			{
			SCA_MoveBobNode(&dh->drgh_boblist, NewBobList, bob);
			}
		else
			{
			LONG DstX = bob->scbob_x - NewBob->scbob_x;
			LONG DstY = bob->scbob_y - NewBob->scbob_y;

			d1(kprintf("%s/%s/%ld: bob=%08lx  DstX=%ld  DstY=%ld\n", __FILE__, __FUNC__, __LINE__, \
				bob, DstX, DstY));

			TempRp->BitMap = NewBob->scbob_Bob.scbob_Bob2.scbob2_bitmap;
			BltMaskBitMapRastPort(bob->scbob_Bob.scbob_Bob2.scbob2_bitmap, 0, 0,
				TempRp, DstX, DstY,
				bob->scbob_width, bob->scbob_height,
				ABC | ABNC | ANBC, bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0]
				);

			TempRp->BitMap = NewBob->scbob_Bob.scbob_Bob2.scbob2_shadow1;
			BltMaskBitMapRastPort(bob->scbob_Bob.scbob_Bob2.scbob2_shadow1, 0, 0,
				TempRp, DstX, DstY,
				bob->scbob_width, bob->scbob_height,
				ABC | ABNC | ANBC, bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0]
				);

			TempRp->BitMap = NewBob->scbob_Bob.scbob_Bob2.scbob2_shadow2;
			BltMaskBitMapRastPort(bob->scbob_Bob.scbob_Bob2.scbob2_shadow2, 0, 0,
				TempRp, DstX, DstY,
				bob->scbob_width, bob->scbob_height,
				ABC | ABNC | ANBC, bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0]
				);
			}

		bob = NextBob;
		}

}


static void MergeBobsAlpha(struct RastPort *TempRp, struct DragHandle *dh, 
	struct ScaBob **NewBobList, struct ScaBob *NewBob)
{
	struct ARGB *OldBobBuffer;
	struct ARGB *NewBobBuffer = NULL;

	do	{
		struct ScaBob *bob;
		size_t AllocSize;
		UBYTE *NewAlphaChannel;
		UWORD NewBobDestMod = NewBob->scbob_width * sizeof(struct ARGB);

		d1(kprintf("%s/%s/%ld: NewBob Width=%ld  Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, NewBob->scbob_width, NewBob->scbob_height));

		// First, move all bobs away that are not to be merged
		for (bob=dh->drgh_boblist; bob; )
			{
			struct ScaBob *NextBob = (struct ScaBob *) bob->scbob_Node.mln_Succ;

			d1(kprintf("%s/%s/%ld: bob=%08lx NoOptimize=%ld\n", __FILE__, __FUNC__, __LINE__, \
				bob, bob->scbob_Flags & BOBFLAGF_NoOptimize ));

			if (bob->scbob_Flags & BOBFLAGF_NoOptimize)
				{
				SCA_MoveBobNode(&dh->drgh_boblist, NewBobList, bob);
				}
			bob = NextBob;
			}

		AllocSize = NewBob->scbob_width * NewBob->scbob_height * sizeof(struct ARGB);
		OldBobBuffer = ScalosAlloc(AllocSize);
		d1(kprintf("%s/%s/%ld: OldBobBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, OldBobBuffer));
		if (NULL == OldBobBuffer)
			break;

		NewBobBuffer = ScalosAlloc(AllocSize);
		d1(kprintf("%s/%s/%ld: NewBobBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, NewBobBuffer));
		if (NULL == NewBobBuffer)
			break;

		// Allocate Alpha channel for new bob
		NewBob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel = NewAlphaChannel = ScalosAlloc(NewBob->scbob_width * NewBob->scbob_height);
		if (NULL == NewBob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel)
			break;
		NewBob->scbob_Flags |= BOBFLAGF_FreeAlphaChannel;

		d1(kprintf("%s/%s/%ld: AlphaChannel=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			NewBob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel));

		memset(NewAlphaChannel, 0, NewBob->scbob_width * NewBob->scbob_height);


		// Merge <scbob2_bitmap>

		memset(NewBobBuffer, 0, AllocSize);
		for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
			{
			LONG DstX = bob->scbob_x - NewBob->scbob_x;
			LONG DstY = bob->scbob_y - NewBob->scbob_y;
			UWORD DestMod = bob->scbob_width * sizeof(struct ARGB);
			ULONG y;
			const UBYTE *AlphaSrcPtr = bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel;
			UBYTE *AlphaDestPtr = NewAlphaChannel + DstY * NewBob->scbob_width;
			const UBYTE *Mask = bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0];
			ULONG MaskBytesPerRow = ((bob->scbob_width + 15) & ~0x0f) / 8;

			d1(kprintf("%s/%s/%ld: bob=%08lx  AlphaSrcPtr=%08lx\n", __FILE__, __FUNC__, __LINE__, bob, AlphaSrcPtr));
			d1(kprintf("%s/%s/%ld: bob=%08lx  width=%ld  height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, bob, bob->scbob_width, bob->scbob_height));

			d1(kprintf("%s/%s/%ld: bob=%08lx  DstX=%ld  DstY=%ld  OldBobBuffer=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, bob, DstX, DstY, OldBobBuffer));

			TempRp->BitMap = bob->scbob_Bob.scbob_Bob2.scbob2_bitmap;

			ScalosReadPixelArray(OldBobBuffer, DestMod, TempRp,
				0, 0, bob->scbob_width, bob->scbob_height);

			if (AlphaSrcPtr)
				{
				BlitARGBMaskAlpha(bob->scbob_width, bob->scbob_height,
					OldBobBuffer, 0, 0,
					NewBob->scbob_width, 
					NewBobBuffer, DstX, DstY,
					bob->scbob_Bob.scbob_Bob2.scbob2_shadow1, 
					ALPHA_OPAQUE,
					AlphaSrcPtr,
					bob->scbob_LeftBorder, bob->scbob_BoundsWidth);
				}
			else
				{
				BlitARGBMask(bob->scbob_width, bob->scbob_height,
					OldBobBuffer, 0, 0,
					NewBob->scbob_width, 
					NewBobBuffer, DstX, DstY,
					bob->scbob_Bob.scbob_Bob2.scbob2_shadow1, 
					ALPHA_OPAQUE);
				}

			// calculate combined Alpha for merged bob
			for (y = 0; y < bob->scbob_height; y++)
				{
				UBYTE *AlphaLineDestPtr = AlphaDestPtr + DstX;
				ULONG x;

				if (AlphaSrcPtr)
					{
					const UBYTE *AlphaLineSrcPtr = AlphaSrcPtr + bob->scbob_LeftBorder;

					for (x = 0; x < bob->scbob_width; x++, AlphaLineSrcPtr++, AlphaLineDestPtr++)
						{
						ULONG OldBobTransparency = ALPHA_OPAQUE - *AlphaLineSrcPtr;
						ULONG NewBobTransparency = ALPHA_OPAQUE - *AlphaLineDestPtr;

						NewBobTransparency = ALPHA_OPAQUE - ((OldBobTransparency * NewBobTransparency) >> 8);
						if (NewBobTransparency > UCHAR_MAX)
							NewBobTransparency = UCHAR_MAX;

						*AlphaLineDestPtr = NewBobTransparency;
						}

					AlphaSrcPtr += bob->scbob_BoundsWidth;
					}
				else
					{
					// <bob> has no Alpha channel
					UWORD BitMask = 0x0080;
					const UBYTE *MaskPtr2 = Mask;

					for (x = 0; x < bob->scbob_width; x++, AlphaLineDestPtr++)
						{
						if (*MaskPtr2 & BitMask)
							*AlphaLineDestPtr = ALPHA_OPAQUE;

						BitMask >>= 1;
						if (0 == BitMask)
							{
							BitMask = 0x0080;
							MaskPtr2++;
							}
						}

					Mask += MaskBytesPerRow;
					}

				AlphaDestPtr += NewBob->scbob_width;
				}
			d1(kprintf("%s/%s/%ld: AlphaDestPtr=%08lx  Offset=%lu\n", \
				__FILE__, __FUNC__, __LINE__, AlphaDestPtr, AlphaDestPtr - NewAlphaChannel));
			}

		TempRp->BitMap = NewBob->scbob_Bob.scbob_Bob2.scbob2_bitmap;
		ScalosWritePixelArray(NewBobBuffer, NewBobDestMod, TempRp,
			0, 0, NewBob->scbob_width, NewBob->scbob_height);



		// Now merge <scbob2_shadow1> and <scbob2_shadow2>

		for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
			{
			LONG DstX = bob->scbob_x - NewBob->scbob_x;
			LONG DstY = bob->scbob_y - NewBob->scbob_y;

			d1(kprintf("%s/%s/%ld: bob=%08lx  DstX=%ld  DstY=%ld\n", __FILE__, __FUNC__, __LINE__, \
				bob, DstX, DstY));

			TempRp->BitMap = NewBob->scbob_Bob.scbob_Bob2.scbob2_shadow1;
			BltMaskBitMapRastPort(bob->scbob_Bob.scbob_Bob2.scbob2_shadow1, 0, 0,
				TempRp, DstX, DstY,
				bob->scbob_width, bob->scbob_height,
				ABC | ABNC | ANBC, bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0]
				);

			TempRp->BitMap = NewBob->scbob_Bob.scbob_Bob2.scbob2_shadow2;
			BltMaskBitMapRastPort(bob->scbob_Bob.scbob_Bob2.scbob2_shadow2, 0, 0,
				TempRp, DstX, DstY,
				bob->scbob_width, bob->scbob_height,
				ABC | ABNC | ANBC, bob->scbob_Bob.scbob_Bob2.scbob2_shadow2->Planes[0]
				);
			}
		} while (0);

	if (NewBobBuffer)
		ScalosFree(NewBobBuffer);
	if (OldBobBuffer)
		ScalosFree(OldBobBuffer);
}


static void SwapLong(LONG *x1, LONG *x2)
{
	LONG temp = *x1;

	*x1 = *x2;
	*x2 = temp;
}


// Free all bobs in BobList
static void FreeBobList(struct DragHandle *dh)
{
	struct ScaBob *bob;

	if (!(dh->drgh_flags & DRGF_BobsHidden))
		{
		for (bob=dh->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
			{
			RemIBob(&bob->scbob_Bob.scbob_Bob1.scbob1_bob, &dh->drgh_screen->RastPort, &dh->drgh_screen->ViewPort);
			}

		WaitBOVP(&dh->drgh_screen->ViewPort);
		}

	while (dh->drgh_SpecialBobList)
		FreeBobNode((struct ScalosNodeList *) &dh->drgh_SpecialBobList, dh->drgh_SpecialBobList);

	while (dh->drgh_boblist)
		FreeBobNode((struct ScalosNodeList *) &dh->drgh_boblist, dh->drgh_boblist);
}


static void RemoveCustomBobsFromScreen(struct ScaBob **BobList)
{
	struct ScaBob *bob;

	for (bob=*BobList; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
		{
		if (bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap && IsBobPosition(bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos))
			{
			// bob needs to be removed from screen
			ClipBlitOut(bob->scbob_draghandle->drgh_screen,
				bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap,
				bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos, bob->scbob_Bob.scbob_Bob2.scbob2_lastypos,
				bob->scbob_width, bob->scbob_height);
			}
		}

	WaitBlit();
}


// Free all custom bobs in BobList
static void FreeCustomBobList(struct ScaBob **BobList)
{
	RemoveCustomBobsFromScreen(BobList);

	while (*BobList)
		FreeCustomBobNode((struct ScalosNodeList *) BobList, *BobList);
}


static struct ScaBob *AllocBob2(struct DragHandle *dh, struct ScalosNodeList *BobList,
	LONG x, LONG y, LONG Width, LONG Height, LONG BoundsWidth, LONG BoundsHeight)
{
	ULONG Depth, ScreenWidth, Flags;
	BOOL Success = FALSE;
	struct ScaBob *bob;

	do	{
		bob = (struct ScaBob *) SCA_AllocNode(BobList, sizeof(struct ScaBob) - sizeof(struct MinNode));
		if (NULL == bob)
			break;

		bob->scbob_draghandle = (struct DragHandle *) dh;
		bob->scbob_width = Width;
		bob->scbob_height = Height;
		bob->scbob_BoundsWidth = BoundsWidth;
		bob->scbob_BoundsHeight = BoundsHeight;
		bob->scbob_x = x;
		bob->scbob_y = y;
		bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel = NULL;
		bob->scbob_Bob.scbob_Bob2.scbob2_lastxpos = (WORD) NO_BOB_POSITION;
		bob->scbob_Bob.scbob_Bob2.scbob2_lastypos = (WORD) NO_BOB_POSITION;

		Depth = GetBitMapAttr(dh->drgh_screen->RastPort.BitMap, BMA_DEPTH);
		if (0 == Depth)
			break;

		bob->scbob_Bob.scbob_Bob2.scbob2_bitmap = AllocBitMap(bob->scbob_width, bob->scbob_height,
			Depth, BMF_MINPLANES | BMF_DISPLAYABLE, dh->drgh_screen->RastPort.BitMap);
		if (NULL == bob->scbob_Bob.scbob_Bob2.scbob2_bitmap)
			break;

		bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap = AllocBitMap(bob->scbob_width, bob->scbob_height,
			Depth, BMF_MINPLANES | BMF_DISPLAYABLE, dh->drgh_screen->RastPort.BitMap);
		if (NULL == bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap)
			break;

		bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2 = AllocBitMap(bob->scbob_width, bob->scbob_height,
			Depth, BMF_MINPLANES | BMF_DISPLAYABLE, dh->drgh_screen->RastPort.BitMap);
		if (NULL == bob->scbob_Bob.scbob_Bob2.scbob2_backbitmap2)
			break;

		ScreenWidth = GetBitMapAttr(bob->scbob_Bob.scbob_Bob2.scbob2_bitmap, BMA_WIDTH);
		Flags = GetBitMapAttr(bob->scbob_Bob.scbob_Bob2.scbob2_bitmap, BMA_FLAGS);
		if (Flags & BMF_INTERLEAVED)
			{
			Depth = GetBitMapAttr(bob->scbob_Bob.scbob_Bob2.scbob2_bitmap, BMA_DEPTH);
			ScreenWidth *= Depth;
			}

		d1(kprintf("%s/%s/%ld: ScreenWidth=%ld\n", __FILE__, __FUNC__, __LINE__, ScreenWidth));

		bob->scbob_Bob.scbob_Bob2.scbob2_shadow1 = AllocBitMap(ScreenWidth, bob->scbob_height, 1, BMF_CLEAR, NULL);
		if (NULL == bob->scbob_Bob.scbob_Bob2.scbob2_shadow1)
			break;

		d1(kprintf("%s/%s/%ld: Shadow1: BytesPerRow=%ld  Rows=%ld  Depth=%ld  FLags=%02lx\n", __FILE__, __FUNC__, __LINE__, \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->BytesPerRow, bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Rows, \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Depth, bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Flags));
		d1(kprintf("%s/%s/%ld: Planes: %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx \n", __FILE__, __FUNC__, __LINE__, \
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[0], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[1],
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[2], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[3],
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[4], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[5],
			bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[6], bob->scbob_Bob.scbob_Bob2.scbob2_shadow1->Planes[7]));

		bob->scbob_Bob.scbob_Bob2.scbob2_shadow2 = AllocBitMap(ScreenWidth, bob->scbob_height, 1, BMF_CLEAR, NULL);
		if (NULL == bob->scbob_Bob.scbob_Bob2.scbob2_shadow2)
			break;

		Success= TRUE;
		} while (0);

	if (!Success && bob)
		{
		FreeCustomBobNode(BobList, bob);
		bob = NULL;
		}

	return bob;
}


static BOOL IsBobPosition(WORD x)
{
	return (BOOL) ((UWORD) x != NO_BOB_POSITION);
}


static void FreeSpeedBitMapAlloc(struct DragHandle *dh)
{
	if (dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap)
		{
		FreeBitMap(dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap);
		dh->drgh_Specific.drghs_DragHandle2.drgh2_speedbitmap = NULL;
		}
	if (dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap)
		{
		FreeBitMap(dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap);
		dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap = NULL;
		}
	if (dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2)
		{
		FreeBitMap(dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2);
		dh->drgh_Specific.drghs_DragHandle2.drgh2_bufferbitmap2 = NULL;
		}

	if (dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer)
		{
		FreeVec(dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer);
		dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer = NULL;
		}
	if (dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2)
		{
		FreeVec(dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2);
		dh->drgh_Specific.drghs_DragHandle2.drgh2_transbuffer2 = NULL;
		}

	dh->drgh_flags &= ~DRGHF_SpeedBitMapAlloc;
}


LIBFUNC_P2(void, sca_EndDrag,
	A0, struct DragHandle *, dh,
	A6, struct ScalosBase *, ScalosBase)
{
	(void)ScalosBase;

	d1(kprintf("%s/%s/%ld: START  dh=%08lx\n", __FILE__, __FUNC__, __LINE__, dh));

	if (dh == iInfos.xii_GlobalDragHandle)
		{
		iInfos.xii_GlobalDragHandle = NULL;

		if (dh->drgh_flags & DRGHF_CustomBob)
			{
			FreeCustomBobList(&dh->drgh_boblist);	// remove+free all bobs

			FreeCustomBobList(&dh->drgh_SpecialBobList);	// free special bobs

			FreeSpeedBitMapAlloc(dh);

			Scalos_DoneRastPort(&dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport);
			}
		else
			{
			FreeBobList(dh);	// remove+free all bobs

			dh->drgh_screen->RastPort.GelsInfo = dh->drgh_Specific.drghs_DragHandle1.drgh1_oldginfo;
			}

		while (dh->drgh_SpecialDragList)
			{
			d1(kprintf("%s/%s/%ld: Free dgn=%08lx\n", __FILE__, __FUNC__, __LINE__, dh->drgh_SpecialDragList));
			SCA_FreeNode((struct ScalosNodeList *) &dh->drgh_SpecialDragList, &dh->drgh_SpecialDragList->drgn_Node);
			}
		while (dh->drgh_SpecialIconList)
			{
			if (dh->drgh_SpecialIconList->in_Icon)
				DisposeIconObject(dh->drgh_SpecialIconList->in_Icon);

			SCA_FreeNode((struct ScalosNodeList *) &dh->drgh_SpecialIconList, &dh->drgh_SpecialIconList->in_Node);
			}

		d1(kprintf("%s/%s/%ld: dh=%08lx  drgh_flags=%08lx\n", __FILE__, __FUNC__, __LINE__, dh, dh->drgh_flags));
		if (dh->drgh_flags & DRGHF_LayersLocked)
			{
			SCA_UnlockDrag(dh);
			}

		ScalosFree(dh);

		if (!CurrentPrefs.pref_EnableDropMenu)
			ClosePopupWindows(FALSE);
		}
	d1(kprintf("%s/%s/%ld: END  dh=%08lx\n", __FILE__, __FUNC__, __LINE__, dh));
}
LIBFUNC_END


LIBFUNC_P2(struct DragHandle *, sca_InitDrag,
	A0, struct Screen *, Scr,
	A6, struct ScalosBase *, ScalosBase)
{
	ULONG Depth;

	(void) ScalosBase;

	if (NULL == iInfos.xii_GlobalDragHandle)
		{
		struct DragHandle *dh;

		if (NULL == Scr)
			Scr = iInfos.xii_iinfos.ii_Screen;

		Depth = GetBitMapAttr(Scr->RastPort.BitMap, BMA_DEPTH);

		dh = ScalosAlloc(sizeof(struct DragHandle));
		if (NULL == dh)
			return NULL;

		memset(dh, 0, sizeof(struct DragHandle));

		dh->drgh_screen = Scr;

		if (DRAGMETHOD_Custom == CurrentPrefs.pref_DragMethod || Depth > 8)
			{
			dh->drgh_flags |= DRGHF_CustomBob;

			Scalos_InitRastPort(&dh->drgh_Specific.drghs_DragHandle2.drgh2_rastport);

			if (CyberGfxBase && CurrentPrefs.pref_RealTransFlag && Depth > 8)
				dh->drgh_flags |= DRGHF_RealTranspPossible;

			d1(KPrintF("%s/%s/%ld: CyberGfxBase=%08lx  pref_RealTransFlag=%ld  Depth=%ld\n", \
				__FILE__, __FUNC__, __LINE__, CyberGfxBase, CurrentPrefs.pref_RealTransFlag, Depth));

			ScalosInitSemaphore(&dh->drgh_BobListSemaphore);

			iInfos.xii_GlobalDragHandle = dh;
			}
		else
			{
			dh->drgh_Specific.drghs_DragHandle1.drgh1_gelsinfo.leftmost = 0;
			dh->drgh_Specific.drghs_DragHandle1.drgh1_gelsinfo.topmost = 0;
			dh->drgh_Specific.drghs_DragHandle1.drgh1_gelsinfo.rightmost = Scr->Width - 1;
			dh->drgh_Specific.drghs_DragHandle1.drgh1_gelsinfo.bottommost = Scr->Height - 1;

			InitGels(&dh->drgh_Specific.drghs_DragHandle1.drgh1_vshead, 
				&dh->drgh_Specific.drghs_DragHandle1.drgh1_vstail, 
				&dh->drgh_Specific.drghs_DragHandle1.drgh1_gelsinfo);

			dh->drgh_Specific.drghs_DragHandle1.drgh1_oldginfo = dh->drgh_screen->RastPort.GelsInfo;
			dh->drgh_screen->RastPort.GelsInfo = &dh->drgh_Specific.drghs_DragHandle1.drgh1_gelsinfo;

			ScalosInitSemaphore(&dh->drgh_BobListSemaphore);

			iInfos.xii_GlobalDragHandle = dh;
			}
		}

	return iInfos.xii_GlobalDragHandle;
}
LIBFUNC_END


static BOOL AddSpecialIcon(struct internalScaWindowTask *iwt, struct RastPort *rp, 
	CONST_STRPTR IconFileName, struct ScaBob **BobPtr)
{
	struct ScaIconNode *in;
	struct DragHandle *dh = iwt->iwt_myDragHandle;

	in = (struct ScaIconNode *) SCA_AllocStdNode((struct ScalosNodeList *) &dh->drgh_SpecialIconList, NTYP_IconNode);
	if (NULL == in)
		return FALSE;

	in->in_Icon = NewIconObjectTags((STRPTR) IconFileName,
				IDTA_Text, (ULONG) "",
				TAG_END);

	d1(kprintf("%s/%s/%ld: in=%08lx  IconObj=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Icon, IconFileName));

	if (NULL == in->in_Icon)
		{
		SCA_FreeNode((struct ScalosNodeList *) &dh->drgh_SpecialIconList, &in->in_Node);
		return FALSE;
		}

	DoMethod(in->in_Icon, IDTM_Layout, iwt->iwt_WinScreen, 
		iwt->iwt_WindowTask.wt_Window, iwt->iwt_WindowTask.wt_Window->RPort, 
		iwt->iwt_WinDrawInfo, IOLAYOUTF_NormalImage);

	return AppendSpecialDragNode(dh, iwt, rp, in, BobPtr);
}


static BOOL AppendSpecialDragNode(struct DragHandle *dh,
	struct internalScaWindowTask *iwt, struct RastPort *rp,
	struct ScaIconNode *in, struct ScaBob **BobPtr)
{
	BOOL Success = FALSE;
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	struct DragNode *dgn = (struct DragNode *) SCA_AllocStdNode((struct ScalosNodeList *) &dh->drgh_SpecialDragList, NTYP_DragNode);

	d1(KPrintF("%s/%s/%ld: dgn=%08lx\n", __FILE__, __FUNC__, __LINE__, dgn));

	if (dgn)
		{
		struct ScaBob *bob;
		WORD bx, by;

		bx = - gg->Width / 2;
		by = - gg->Height / 2;

		dgn->drgn_iconnode = in;
		dgn->drgn_icon = in->in_Icon;

		dgn->drgn_x = 0;
		dgn->drgn_y = 0;

		// create new Bob at end of drgh_boblist
		bob = InitDrag_ImageOnly(dgn, &iwt->iwt_myDragHandle->drgh_SpecialBobList, rp, iwt, bx, by);

		// find bob
		if (bob)
			{
			*BobPtr = bob;

			bob->scbob_Flags |= BOBFLAGF_NeverTransparent;

			Success = TRUE;
			}
		}

	return Success;
}


// Add or remove special indicator bob
// used with custom bob routines
static void CheckSpecialBobCustom(struct DragHandle *dh, ULONG Flags, 
	ULONG MaskBit, struct ScaBob **bob, CONST_STRPTR FirstName, CONST_STRPTR FallbackName)
{
	BOOL changed = FALSE;

	if (Flags && !(dh->drgh_flags & MaskBit))
		{
		// Add Indicator
		if (NULL == *bob)
			{
			struct RastPort rp;

			InitRastPort(&rp);
			if (!AddSpecialIcon(dh->drgh_iwt, &rp, FirstName, bob))
				AddSpecialIcon(dh->drgh_iwt, &rp, FallbackName, bob);
			}

		if (*bob)
			{
			RemoveCustomBobsFromScreen(&dh->drgh_boblist);

			SCA_MoveBobNode(&dh->drgh_SpecialBobList, &dh->drgh_boblist, *bob);

			dh->drgh_flags |= MaskBit;

			(*bob)->scbob_Bob.scbob_Bob2.scbob2_xpos = (*bob)->scbob_Bob.scbob_Bob2.scbob2_lastxpos = (WORD) NO_BOB_POSITION;
			(*bob)->scbob_Bob.scbob_Bob2.scbob2_ypos = (*bob)->scbob_Bob.scbob_Bob2.scbob2_lastypos = (WORD) NO_BOB_POSITION;

			changed = TRUE;
			}
		}
	if (!Flags && (dh->drgh_flags & MaskBit))
		{
		// Remove Indicator
		d1(kprintf("%s/%s/%ld: bob=%08lx\n", __FILE__, __FUNC__, __LINE__, *bob));

		if ((*bob)->scbob_Bob.scbob_Bob2.scbob2_backbitmap && IsBobPosition((*bob)->scbob_Bob.scbob_Bob2.scbob2_lastxpos))
			{
			// bob needs to be removed from screen
			ClipBlitOut((*bob)->scbob_draghandle->drgh_screen,
				(*bob)->scbob_Bob.scbob_Bob2.scbob2_backbitmap,
				(*bob)->scbob_Bob.scbob_Bob2.scbob2_lastxpos, (*bob)->scbob_Bob.scbob_Bob2.scbob2_lastypos,
				(*bob)->scbob_width, (*bob)->scbob_height);
			}

		SCA_MoveBobNode(&dh->drgh_boblist, &dh->drgh_SpecialBobList, *bob);

		RemoveCustomBobsFromScreen(&dh->drgh_boblist);

		(*bob)->scbob_Bob.scbob_Bob2.scbob2_xpos = (*bob)->scbob_Bob.scbob_Bob2.scbob2_lastxpos = (WORD) NO_BOB_POSITION;
		(*bob)->scbob_Bob.scbob_Bob2.scbob2_ypos = (*bob)->scbob_Bob.scbob_Bob2.scbob2_lastypos = (WORD) NO_BOB_POSITION;

		dh->drgh_flags &= ~MaskBit;
		changed = TRUE;
		}
	if (changed)
		FreeSpeedBitMapAlloc(dh);
}


// Add or remove special indicator bob
// used with system bob routines
static void CheckSpecialBob(struct DragHandle *dh, ULONG Flags, 
	ULONG MaskBit, struct ScaBob **bob, CONST_STRPTR FirstName, CONST_STRPTR FallbackName)
{
	if (Flags && !(dh->drgh_flags & MaskBit))
		{
		// Add Indicator
		if (NULL == *bob)
			{
			struct RastPort rp;

			InitRastPort(&rp);
			if (!AddSpecialIcon(dh->drgh_iwt, &rp, FirstName, bob))
				AddSpecialIcon(dh->drgh_iwt, &rp, FallbackName, bob);
			}
		if (*bob)
			{
			SCA_MoveBobNode(&dh->drgh_SpecialBobList, &dh->drgh_boblist, *bob);

			dh->drgh_flags |= MaskBit;

			if (!(dh->drgh_flags & DRGF_BobsHidden))
				AddBob(&(*bob)->scbob_Bob.scbob_Bob1.scbob1_bob, &dh->drgh_screen->RastPort);
			}
		}
	if (!Flags && (dh->drgh_flags & MaskBit))
		{
		// Remove Indicator
		if (!(dh->drgh_flags & DRGF_BobsHidden))
			RemIBob(&(*bob)->scbob_Bob.scbob_Bob1.scbob1_bob, &dh->drgh_screen->RastPort, &dh->drgh_screen->ViewPort);

		SCA_MoveBobNode(&dh->drgh_boblist, &dh->drgh_SpecialBobList, *bob);

		dh->drgh_flags &= ~MaskBit;
		}
}


void EndDrag(struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: iwt=%08lx  DragFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_DragFlags));
	d1(kprintf("%s/%s/%ld: WinUnderPtr=%08lx  DragFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_WinUnderPtr, iwt->iwt_WinUnderPtr ? iwt->iwt_WinUnderPtr->iwt_DragFlags : 0l));

	SCA_EndDrag(iwt->iwt_myDragHandle);	// side effect: does UnlockLayers()

	iwt->iwt_myDragHandle = NULL;
}


void EndDragUnlock(struct internalScaWindowTask *iwt)
{
	SCA_FreeAllNodes((struct ScalosNodeList *) &iwt->iwt_DragNodeList);

	if (iwt->iwt_IconListDDLocked)
		{
		iwt->iwt_IconListDDLocked = FALSE;
		ScalosUnLockIconList(iwt);
		}
}


static BOOL AddInfoTextBob(struct internalScaWindowTask *iwt)
{
	BOOL Success = FALSE;
	STRPTR TextBuffer[3] = { NULL, NULL, NULL };
	struct BitMap *bm = NULL;
	APTR Mask = NULL;
	short n;

	do	{
		struct RastPort rp;
		struct TextExtent tExt[3];
		WORD Width, Height;
		WORD x, y;
		LONG Len, yMax;
		struct ScaBob *bob;

		memset(tExt, 0, sizeof(tExt));

		for (n=0; n<3; n++)
			{
			TextBuffer[n] = AllocPathBuffer();
			if (NULL == TextBuffer[n])
				break;

			strcpy(TextBuffer[n], "");
			}

		if (iwt->iwt_myDragHandle->drgh_DrawerCount + iwt->iwt_myDragHandle->drgh_DeviceCount
			+  iwt->iwt_myDragHandle->drgh_FileCount > 1)
			{
			if (iwt->iwt_myDragHandle->drgh_FileCount > 0)
				{
				ScaFormatStringMaxLength(TextBuffer[0], Max_PathLen,
					GetLocString(MSGID_MULTIDRAG), 
					iwt->iwt_myDragHandle->drgh_FileCount,
					(ULONG) GetLocString(1 == iwt->iwt_myDragHandle->drgh_FileCount ? MSGID_MULTIDRAG_FILE_1 : MSGID_MULTIDRAG_FILE_2)
					);
				}
			if (iwt->iwt_myDragHandle->drgh_DrawerCount > 0)
				{
				ScaFormatStringMaxLength(TextBuffer[1], Max_PathLen,
					GetLocString(MSGID_MULTIDRAG), 
					iwt->iwt_myDragHandle->drgh_DrawerCount,
					(ULONG) GetLocString(1 == iwt->iwt_myDragHandle->drgh_DrawerCount ? MSGID_MULTIDRAG_DRAWER_1 : MSGID_MULTIDRAG_DRAWER_2)
					);
				}
			if (iwt->iwt_myDragHandle->drgh_DeviceCount > 0)
				{
				ScaFormatStringMaxLength(TextBuffer[2], Max_PathLen,
					GetLocString(MSGID_MULTIDRAG), 
					iwt->iwt_myDragHandle->drgh_DeviceCount,
					(ULONG) GetLocString(1 == iwt->iwt_myDragHandle->drgh_DeviceCount ? MSGID_MULTIDRAG_DEVICE_1 : MSGID_MULTIDRAG_DEVICE_2)
					);
				}
			}

		for (Len=0, n=0; n<3; n++)
			{
			Len += strlen(TextBuffer[n]);
			}

		if (0 == Len)
			break;

		// find position for text bob (just below all othe bobs)
		yMax = -1;
		for (bob=iwt->iwt_myDragHandle->drgh_boblist; bob; bob = (struct ScaBob *) bob->scbob_Node.mln_Succ)
			{
			if (bob->scbob_y + bob->scbob_height > yMax)
				yMax = bob->scbob_y + bob->scbob_height;
			}

		Scalos_InitRastPort(&rp);
		Scalos_SetFont(&rp, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);
		SetABPenDrMd(&rp, PalettePrefs.pal_PensList[PENIDX_DRAGINFOTEXT_TEXT],
			PalettePrefs.pal_PensList[PENIDX_DRAGINFOTEXT_BG], JAM1);

		for (n=0; n<3; n++)
			{
			if (*TextBuffer[n])
				Scalos_TextExtent(&rp, TextBuffer[n], strlen(TextBuffer[n]), &tExt[n]);
			}

		Width = Height = 0;
		for (n=0; n<3; n++)
			{
			if (tExt[n].te_Width > Width)
				Width = tExt[n].te_Width;

			Height += tExt[n].te_Height;
			}

		bm = AllocBitMap(Width, Height,
			GetBitMapAttr(iwt->iwt_myDragHandle->drgh_screen->RastPort.BitMap, BMA_DEPTH),
			BMF_STANDARD,
			iwt->iwt_myDragHandle->drgh_screen->RastPort.BitMap);
		if (NULL == bm)
			break;

		rp.BitMap = bm;

		SetRast(&rp, PalettePrefs.pal_PensList[PENIDX_DRAGINFOTEXT_BG]);

		y = Scalos_GetFontBaseline(&rp);
		for (n=0; n<3; n++)
			{
			x = (Width - tExt[n].te_Width) / 2 - tExt[n].te_Extent.MinX;

			Move(&rp, x, y);
			Scalos_Text(&rp, TextBuffer[n], strlen(TextBuffer[n]));

			y += tExt[n].te_Height;
			}

		Success = SCA_AddBob(iwt->iwt_myDragHandle,
			bm, Mask,
			Width, Height,
			- Width / 2, yMax + 1);

		bob = internalAddBob(iwt->iwt_myDragHandle,
			&iwt->iwt_myDragHandle->drgh_boblist,
			bm, Mask, NULL,
			Width, Height,
			Width, Height,
			0,
			- Width / 2, yMax + 1);
		if (bob)
			{
			Success = TRUE;
			bob->scbob_Flags |= BOBFLAGF_NeverTransparent | BOBFLAGF_NoOptimize;
			}

		Scalos_DoneRastPort(&rp);
		} while (0);

	if (bm)
		FreeBitMap(bm);

	for (n=0; n<3; n++)
		{
		if (TextBuffer[n])
			FreePathBuffer(TextBuffer[n]);
		}

	return Success;
}


static void CountDragObjects(struct DragHandle *drgh, const struct ScaIconNode *in)
{
	if (in->in_DeviceIcon)
		drgh->drgh_DeviceCount++;
	else
		{
		if (in->in_Flags & INF_File)
			drgh->drgh_FileCount++;
		else
			drgh->drgh_DrawerCount++;
		}
}


void BlitARGB(ULONG SrcWidth, ULONG SrcHeight,
		const struct ARGB *Src, LONG SrcLeft, LONG SrcTop,
		ULONG DestWidth, struct ARGB *Dest, LONG DestLeft, LONG DestTop,
		ULONG Trans)
{
	ULONG y;
	ULONG a;
	ULONG mla;


	a = Trans + 1;
	mla = 257 - a;

	Src += SrcTop * SrcWidth;
	Dest += DestTop * DestWidth;

	for (y=0; y<SrcHeight; y++)
		{
		ULONG x;
		const struct ARGB *SrcPtr = Src + SrcLeft;
		struct ARGB *DestPtr = Dest + DestLeft;

		for (x=0; x<SrcWidth; x++)
			{
			d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

			switch (a)
				{
			case 0:
			case 1:
				break;
			case ALPHA_OPAQUE:
				*DestPtr = *SrcPtr;
				break;
			default:
				DestPtr->Red   = (a * SrcPtr->Red   + mla * DestPtr->Red)   >> 8;
				DestPtr->Green = (a * SrcPtr->Green + mla * DestPtr->Green) >> 8;
				DestPtr->Blue  = (a * SrcPtr->Blue  + mla * DestPtr->Blue)  >> 8;
				break;
				}

			SrcPtr++;
			DestPtr++;
			}

		Src += SrcWidth;
		Dest += DestWidth;

		d1(KPrintF("%s/%s/%ld: ny=%ld\n", __FILE__, __FUNC__, __LINE__, ny));
		}
}


void BlitARGBMask(ULONG SrcWidth, ULONG SrcHeight,
		const struct ARGB *Src, LONG SrcLeft, LONG SrcTop,
		ULONG DestWidth, struct ARGB *Dest, LONG DestLeft, LONG DestTop,
		const struct BitMap *MaskBM, ULONG Trans)
{
	UWORD MaskBytesPerRow = MaskBM->BytesPerRow;
	const UBYTE *Mask = MaskBM->Planes[0];
	ULONG y;
	ULONG a;


	d1(kprintf("%s/%s/%ld: Mask=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
		Mask[0], Mask[1], Mask[2], Mask[3], \
		Mask[4], Mask[5], Mask[6], Mask[7]));
	d1(kprintf("%s/%s/%ld: Mask=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
		Mask[8], Mask[9], Mask[10], Mask[11], \
		Mask[12], Mask[13], Mask[14], Mask[15]));

	d1(kprintf("%s/%s/%ld: Mask=%08lx  MaskBytesPerRow=%ld\n", __FILE__, __FUNC__, __LINE__, Mask, MaskBytesPerRow ));

	a = Trans + 1;

	if ( a > 1)
		{
		ULONG	mla = 257 - a;

		Src += SrcTop * SrcWidth;
		Dest += DestTop * DestWidth;

		for (y=0; y<SrcHeight; y++)
			{
			ULONG x;
			UWORD BitMask = 0x0080;
			const UBYTE *MaskPtr2 = Mask;
			const struct ARGB *SrcPtr = Src + SrcLeft;
			struct ARGB *DestPtr = Dest + DestLeft;

			for (x=0; x<SrcWidth; x++)
				{
				d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  Mask=%08lx  *MaskPtr2=%02lx  BitMask=%02lx\n", __FILE__, __FUNC__, __LINE__, x, y, Mask, *MaskPtr2, BitMask));

				if (*MaskPtr2 & BitMask)
					{
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  *Mask=%02lx  BitMask=%02lx\n", __FILE__, __FUNC__, __LINE__, x, y, *Mask, BitMask));

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*DestPtr = *SrcPtr;
						break;
					default:
						DestPtr->Red   = (a * SrcPtr->Red   + mla * DestPtr->Red)   >> 8;
						DestPtr->Green = (a * SrcPtr->Green + mla * DestPtr->Green) >> 8;
						DestPtr->Blue  = (a * SrcPtr->Blue  + mla * DestPtr->Blue)  >> 8;
						break;
						}
					}

				SrcPtr++;
				DestPtr++;

				BitMask >>= 1;
				if (0 == BitMask)
					{
					BitMask = 0x0080;
					MaskPtr2++;
					}
				}

			Mask += MaskBytesPerRow;
			Src += SrcWidth;
			Dest += DestWidth;

			d1(kprintf("%s/%s/%ld: ny=%ld  Mask=%08lx\n", __FILE__, __FUNC__, __LINE__, ny, mask));
			}
		}
}


static void BlitARGBMaskAlpha(ULONG SrcWidth, ULONG SrcHeight, 
		const struct ARGB *Src, LONG SrcLeft, LONG SrcTop,
		ULONG DestWidth, struct ARGB *Dest, LONG DestLeft, LONG DestTop,
		const struct BitMap *MaskBM, ULONG Trans,
		const UBYTE *Alpha, LONG AlphaLeft, ULONG AlphaWidth)
{
	UWORD MaskBytesPerRow = MaskBM->BytesPerRow;
	const UBYTE *Mask = MaskBM->Planes[0];
	ULONG y;


	d1(kprintf("%s/%s/%ld: Mask=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
		Mask[0], Mask[1], Mask[2], Mask[3], \
		Mask[4], Mask[5], Mask[6], Mask[7]));
	d1(kprintf("%s/%s/%ld: Mask=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n", __FILE__, __FUNC__, __LINE__, \
		Mask[8], Mask[9], Mask[10], Mask[11], \
		Mask[12], Mask[13], Mask[14], Mask[15]));

	d1(kprintf("%s/%s/%ld: SrcWidth=%ld  BoundsWidth=%ld  Left=%ld\n", __FILE__, __FUNC__, __LINE__, SrcWidth, BoundsWidth, Left));

	Src += SrcTop * SrcWidth;
	Dest += DestTop * DestWidth;

	d1(kprintf("%s/%s/%ld: SrcLeft=%ld  SrcTop=%ld  DestLeft=%ld  DestTop=%08ld\n", \
		__FILE__, __FUNC__, __LINE__, SrcLeft, SrcTop, DestLeft, DestTop));
	d1(kprintf("%s/%s/%ld: Src=%08lx  Dest=%08lx\n", __FILE__, __FUNC__, __LINE__, Src, Dest));

	for (y=0; y < SrcHeight; y++)
		{
		ULONG x;
		UWORD BitMask = 0x0080;
		const UBYTE *MaskPtr2 = Mask;
		const struct ARGB *SrcPtr = Src + SrcLeft;
		struct ARGB *DestPtr = Dest + DestLeft;
		const UBYTE *AlphaPtr = Alpha + AlphaLeft;

		for (x=0; x < SrcWidth; x++)
			{
			d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  MaskPtr2=%08lx  *MaskPtr2=%02lx  BitMask=%02lx\n", \
				__FILE__, __FUNC__, __LINE__, x, y, MaskPtr2, *MaskPtr2, BitMask));

			if (*MaskPtr2 & BitMask)
				{
				ULONG a;
				ULONG mla;

				d1(kprintf("%s/%s/%ld: x=%ld  y=%ld  *Mask=%02lx  BitMask=%02lx\n", __FILE__, __FUNC__, __LINE__, x, y, *Mask, BitMask));

				a = (ULONG) *AlphaPtr;
				a = ((Trans * a) >> 8) + 1;

				switch (a)
					{
				case 0:
				case 1:
					break;
				case ALPHA_OPAQUE:
					*DestPtr = *SrcPtr;
					break;
				default:
					mla = 257 - a;
					DestPtr->Red   = (a * SrcPtr->Red   + mla * DestPtr->Red)   >> 8;
					DestPtr->Green = (a * SrcPtr->Green + mla * DestPtr->Green) >> 8;
					DestPtr->Blue  = (a * SrcPtr->Blue  + mla * DestPtr->Blue)  >> 8;
					break;
					}
				}

			SrcPtr++;
			DestPtr++;
			AlphaPtr++;

			BitMask >>= 1;
			if (0 == BitMask)
				{
				BitMask = 0x0080;
				MaskPtr2++;
				}
			}

		Alpha += AlphaWidth;
		Mask += MaskBytesPerRow;
		Src += SrcWidth;
		Dest += DestWidth;

		d1(kprintf("%s/%s/%ld: ny=%ld  Mask=%08lx\n", __FILE__, __FUNC__, __LINE__, ny, mask));
		}
}

//----------------------------------------------------------------------------
#if 0
static void DumpBitMap(struct BitMap *bm)
{
	struct RastPort rp;
	ULONG y;
	LONG Width, Height, Depth;

	Width = GetBitMapAttr(bm, BMA_WIDTH);
	Height = GetBitMapAttr(bm, BMA_HEIGHT);
	Depth = GetBitMapAttr(bm, BMA_DEPTH);

	KPrintF("%s/%s/%ld: START Width=%ld  Height=%ld  Depth=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height, Depth);

	InitRastPort(&rp);
	rp.BitMap = bm;

	for (y = 0; y < Height; y++)
		{
		ULONG x;

		for (x = 0; x < Width; x++)
			{
			ULONG px = ReadPixel(&rp, x, y);

			KPrintF("%ld ", px);
			}

		KPrintF("\n");
		}

	KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__);
}
#endif
//----------------------------------------------------------------------------


