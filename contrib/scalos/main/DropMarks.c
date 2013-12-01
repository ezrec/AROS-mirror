// DropMarks.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <workbench/startup.h>
#include <cybergraphx/cybergraphics.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/utility.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

static void DrawWindowDropMark(struct internalScaWindowTask *iwt, struct Region *clipRegion);
static void DrawIconDropMark(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static void DrawGadgetClickMark(struct internalScaWindowTask *iwt, struct ExtGadget *gg);
static BOOL SaveToDropMarkInfo(struct DropMarkInfo *dmi,
	struct RastPort *SrcRp, ULONG Thickness);
static void RestoreFromDropMarkInfo(struct DropMarkInfo *dmi, struct RastPort *SrcRp);
static struct DropMarkInfo *NewIconDropMarkInfo(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in);
static struct DropMarkInfo *GetIconDropMark(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in);

//----------------------------------------------------------------------------

// local data items :

static const UWORD DropMarkPattern = 0xf0f0;

//----------------------------------------------------------------------------


void CleanupDropMarkInfoList(struct List *dmList)
{
	struct DropMarkInfo *dmi;

	while ((dmi = (struct DropMarkInfo *) RemHead(dmList)))
		{
		CleanupDropMarkInfo(dmi);
		ScalosFree(dmi);
		}
}

void CleanupDropMarkInfo(struct DropMarkInfo *dmi)
{
	ULONG n;

	for (n = 0; n < 4; n++)
		{
		if (dmi->dmi_SaveBM[n])
			{
			FreeBitMap(dmi->dmi_SaveBM[n]);
			dmi->dmi_SaveBM[n] = NULL;
			}
		}
}

void DisplayWindowDropMark(struct internalScaWindowTask *iwt)
{
	if (!(iwt->iwt_DragFlags & DRAGFLAGF_WindowMarked))
		{
		BOOL ShowMark = FALSE;

		switch (CurrentPrefs.pref_WindowDropMarkMode)
			{
		case IDTV_WinDropMark_Never:
			break;
		case IDTV_WinDropMark_WindowedOnly:
			if (!iwt->iwt_BackDrop)
				ShowMark = TRUE;
			break;
		case IDTV_WinDropMark_Always:
			ShowMark = TRUE;
			break;
			}

		if (ShowMark)
			{
			iwt->iwt_DragFlags |= DRAGFLAGF_WindowMarked;
			d1(KPrintF("%s/%s/%ld: ShowMark = TRUE/%ld: iwt=%08lx title=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
			DrawWindowDropMark(iwt, NULL);
			}
		}
}


void EraseWindowDropMark(struct internalScaWindowTask *iwt)
{
	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowMarked)
		{
		d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		RestoreFromDropMarkInfo(&iwt->iwt_WindowDropMarkInfo,
			iwt->iwt_WindowTask.wt_Window->RPort);

		d1(kprintf("%s/%s/%ld: iwt=%08lx title=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
			iwt->iwt_DragFlags &= ~DRAGFLAGF_WindowMarked;
		}
}


void DisplayIconDropMark(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (!(in->in_Flags & INF_DropMarkVisible))
		{
		DrawIconDropMark(iwt, in);
		in->in_Flags |= INF_DropMarkVisible;
		d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
		}
}


void EraseIconDropMark(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (in->in_Flags & INF_DropMarkVisible)
		{
		struct DropMarkInfo *dmi;

		d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		dmi = GetIconDropMark(iwt, in);
		d1(KPrintF("%s/%s/%ld: dmi=%08lx\n", __FILE__, __FUNC__, __LINE__, dmi));
		if (dmi)
			{
			RestoreFromDropMarkInfo(dmi, iwt->iwt_WindowTask.wt_Window->RPort);
			d1(KPrintF("%s/%s/%ld: dmi=%08lx\n", __FILE__, __FUNC__, __LINE__, dmi));
			ScalosFree(dmi);
			d1(KPrintF("%s/%s/%ld: dmi=%08lx\n", __FILE__, __FUNC__, __LINE__, dmi));
			}

		in->in_Flags &= ~INF_DropMarkVisible;
		d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
		}
}


void DisplayGadgetClickMark(struct internalScaWindowTask *iwt, struct ExtGadget *gg)
{
	d1(KPrintF("%s/%s/%ld: gg=%08lx\n", __FILE__, __FUNC__, __LINE__, gg));
	DrawGadgetClickMark(iwt, gg);
}


void EraseGadgetClickMark(struct internalScaWindowTask *iwt, struct ExtGadget *gg)
{
	struct DropMarkInfo *dmi;

	d1(KPrintF("%s/%s/%ld: gg=%08lx\n", __FILE__, __FUNC__, __LINE__, gg));

	dmi = GetIconDropMark(iwt, (struct ScaIconNode *) gg);
	if (dmi)
		{
		struct Region *origClipRegion;

		origClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->RPort->Layer, NULL);

		RestoreFromDropMarkInfo(dmi, iwt->iwt_WindowTask.wt_Window->RPort);
		ScalosFree(dmi);

		InstallClipRegion(iwt->iwt_WindowTask.wt_Window->RPort->Layer, origClipRegion);
		}
}


void RedrawWindowDropMark(struct internalScaWindowTask *iwt, struct Region *clipRegion)
{
	struct RastPort rp;
	struct Region *oldClipRegion;

	rp = *iwt->iwt_WindowTask.wt_Window->RPort;

	SetABPenDrMd(&rp,
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN],
		JAM2);

	oldClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, clipRegion);

	rp.LinePtrn = DropMarkPattern;

	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX+1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX-1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY);

	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY+1);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY-1);

	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX-1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX+1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY);

	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY-1);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY+1);


	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX+2, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY+1);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX-2, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY+1);

	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX-1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY+1);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX-1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY-1);

	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX-2, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY-1);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX+2, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY-1);

	Move(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX+1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY-1);
	Draw(&rp, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX+1, iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY+1);

	InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, oldClipRegion);
}


static void DrawWindowDropMark(struct internalScaWindowTask *iwt, struct Region *clipRegion)
{
	d1(kprintf("%s/%s/%ld: Started: iwt=%08lx  title=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: iwt=%08lx  ClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_Window->WLayer->ClipRegion));

	iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX = iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinX = iwt->iwt_InnerLeft + 2;
	iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxX += iwt->iwt_InnerWidth - 4 - 1;
	iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY = iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MinY = iwt->iwt_InnerTop + 2;
	iwt->iwt_WindowDropMarkInfo.dmi_Rectangle.MaxY += iwt->iwt_InnerHeight - 4 - 1;

	if (!SaveToDropMarkInfo(&iwt->iwt_WindowDropMarkInfo,
			iwt->iwt_WindowTask.wt_Window->RPort, 3))
		return;

	RedrawWindowDropMark(iwt, clipRegion);

	d1(kprintf("%s/%s/%ld: Finished: oldClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, oldClipRegion));
}


static void DrawIconDropMark(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	struct RastPort rp;
	struct DropMarkInfo *dmi;

	d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	do	{
		dmi = NewIconDropMarkInfo(iwt, in);
		if (NULL == dmi)
			break;

		dmi->dmi_Rectangle.MinX = iwt->iwt_InnerLeft + gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset - 2;
		dmi->dmi_Rectangle.MinY = iwt->iwt_InnerTop + gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset - 2;
		dmi->dmi_Rectangle.MaxX = dmi->dmi_Rectangle.MinX + gg->BoundsWidth - 1 + 2 + 2;
		dmi->dmi_Rectangle.MaxY = dmi->dmi_Rectangle.MinY + gg->BoundsHeight - 1 + 2 + 2;

		rp = *iwt->iwt_WindowTask.wt_Window->RPort;

		if (!SaveToDropMarkInfo(dmi, &rp, 1))
			break;

		SetABPenDrMd(&rp,
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN],
			JAM2);

		rp.LinePtrn = DropMarkPattern;

		Move(&rp, dmi->dmi_Rectangle.MinX, dmi->dmi_Rectangle.MinY);
		Draw(&rp, dmi->dmi_Rectangle.MaxX, dmi->dmi_Rectangle.MinY);
		Draw(&rp, dmi->dmi_Rectangle.MaxX, dmi->dmi_Rectangle.MaxY);
		Draw(&rp, dmi->dmi_Rectangle.MinX, dmi->dmi_Rectangle.MaxY);
		Draw(&rp, dmi->dmi_Rectangle.MinX, dmi->dmi_Rectangle.MinY);

		dmi = NULL;
		} while (0);

	if (dmi)
		{
		Remove(&dmi->dmi_Node);
		CleanupDropMarkInfo(dmi);
		ScalosFree(dmi);
		}
}


static void DrawGadgetClickMark(struct internalScaWindowTask *iwt, struct ExtGadget *gg)
{
	struct DropMarkInfo *dmi;
	struct Region *origClipRegion = NULL;
	struct RastPort rp;

	d1(KPrintF("%s/%s/%ld: gg=%08lx\n", __FILE__, __FUNC__, __LINE__, gg));

	do	{
		static const LONG Border = 1;
		rp = *iwt->iwt_WindowTask.wt_Window->RPort;

		dmi = NewIconDropMarkInfo(iwt, (struct ScaIconNode *) gg);
		if (NULL == dmi)
			{
			d1(KPrintF("%s/%s/%ld: NewIconDropMarkInfo failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		dmi->dmi_Rectangle.MinX = gg->LeftEdge - Border;
		dmi->dmi_Rectangle.MinY = gg->TopEdge - Border;
		dmi->dmi_Rectangle.MaxX = dmi->dmi_Rectangle.MinX + gg->Width  - 1 + Border + Border;
		dmi->dmi_Rectangle.MaxY = dmi->dmi_Rectangle.MinY + gg->Height - 1 + Border + Border;

		origClipRegion = InstallClipRegion(rp.Layer, NULL);

		if (!SaveToDropMarkInfo(dmi, &rp, 1))
			{
			d1(KPrintF("%s/%s/%ld: SaveToDropMarkInfo  failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		d1(KPrintF("%s/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, dmi->dmi_Rectangle.MinX, dmi->dmi_Rectangle.MinY,\
			dmi->dmi_Rectangle.MaxX, dmi->dmi_Rectangle.MaxY));


		Move(&rp, dmi->dmi_Rectangle.MinX, dmi->dmi_Rectangle.MinY);

		SetAPen(&rp, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);
		Draw(&rp, dmi->dmi_Rectangle.MaxX, dmi->dmi_Rectangle.MinY);
		SetAPen(&rp, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);
		Draw(&rp, dmi->dmi_Rectangle.MaxX, dmi->dmi_Rectangle.MaxY);
		Draw(&rp, dmi->dmi_Rectangle.MinX, dmi->dmi_Rectangle.MaxY);
		SetAPen(&rp, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);
		Draw(&rp, dmi->dmi_Rectangle.MinX, dmi->dmi_Rectangle.MinY);

		dmi = NULL;
		} while (0);

	if (origClipRegion)
		InstallClipRegion(rp.Layer, origClipRegion);

	if (dmi)
		{
		Remove(&dmi->dmi_Node);
		CleanupDropMarkInfo(dmi);
		ScalosFree(dmi);
		}
}


static BOOL SaveToDropMarkInfo(struct DropMarkInfo *dmi,
	struct RastPort *SrcRp, ULONG Thickness)
{
	BOOL Success = FALSE;

	CleanupDropMarkInfo(dmi);

	do	{
		WORD Width = 1 + (dmi->dmi_Rectangle.MaxX - dmi->dmi_Rectangle.MinX);
		WORD Height = 1 + (dmi->dmi_Rectangle.MaxY - dmi->dmi_Rectangle.MinY);
		struct RastPort rp;
		ULONG Depth;

		dmi->dmi_Thickness = Thickness;

		Depth = GetBitMapAttr(SrcRp->BitMap, BMA_DEPTH);
		d1(KPrintF("%s/%s/%ld: Depth=%ld\n", __FILE__, __FUNC__, __LINE__, Depth));
		d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height));
	
		dmi->dmi_SaveBM[0] = AllocBitMap(Width, Thickness, Depth, BMF_MINPLANES, SrcRp->BitMap);
		if (NULL == dmi->dmi_SaveBM[0])
			break;
		dmi->dmi_SaveBM[1] = AllocBitMap(Thickness, Height, Depth, BMF_MINPLANES, SrcRp->BitMap);
		if (NULL == dmi->dmi_SaveBM[1])
			break;
		dmi->dmi_SaveBM[2] = AllocBitMap(Width, Thickness, Depth, BMF_MINPLANES, SrcRp->BitMap);
		if (NULL == dmi->dmi_SaveBM[2])
			break;
		dmi->dmi_SaveBM[3] = AllocBitMap(Thickness, Height, Depth, BMF_MINPLANES, SrcRp->BitMap);
		if (NULL == dmi->dmi_SaveBM[3])
			break;

		Success = TRUE;
		d1(KPrintF("%s/%s/%ld: dmi_SaveBM=%08lx\n", __FILE__, __FUNC__, __LINE__, dmi->dmi_SaveBM[0]));

		InitRastPort(&rp);

		rp.BitMap = dmi->dmi_SaveBM[0];
		ClipBlit(SrcRp,
			dmi->dmi_Rectangle.MinX,
                        dmi->dmi_Rectangle.MinY,
			&rp,
			0, 0,
			Width, Thickness,
			ABC | ABNC);
		rp.BitMap = dmi->dmi_SaveBM[1];
		ClipBlit(SrcRp,
			dmi->dmi_Rectangle.MaxX - (Thickness - 1),
                        dmi->dmi_Rectangle.MinY,
			&rp,
			0, 0,
			Thickness, Height,
			ABC | ABNC);
		rp.BitMap = dmi->dmi_SaveBM[2];
		ClipBlit(SrcRp,
			dmi->dmi_Rectangle.MinX,
                        dmi->dmi_Rectangle.MaxY - (Thickness - 1),
			&rp,
			0, 0,
			Width, Thickness,
			ABC | ABNC);
		rp.BitMap = dmi->dmi_SaveBM[3];
		ClipBlit(SrcRp,
			dmi->dmi_Rectangle.MinX,
                        dmi->dmi_Rectangle.MinY,
			&rp,
			0, 0,
			Thickness, Height,
			ABC | ABNC);
		} while (0);

	return Success;
}


static void RestoreFromDropMarkInfo(struct DropMarkInfo *dmi, struct RastPort *DestRp)
{
	d1(KPrintF("%s/%s/%ld: ss_Owner=%08lx  ss_QueueCount=%ld  ss_NestCount=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iInfos.xii_iinfos.ii_Screen->LayerInfo.Lock.ss_Owner, \
		iInfos.xii_iinfos.ii_Screen->LayerInfo.Lock.ss_QueueCount, \
		iInfos.xii_iinfos.ii_Screen->LayerInfo.Lock.ss_NestCount));

	if (dmi && dmi->dmi_SaveBM[0])
		{
		WORD Width = 1 + (dmi->dmi_Rectangle.MaxX - dmi->dmi_Rectangle.MinX);
		WORD Height = 1 + (dmi->dmi_Rectangle.MaxY - dmi->dmi_Rectangle.MinY);
		struct RastPort rp;

		d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height));

		InitRastPort(&rp);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		rp.BitMap = dmi->dmi_SaveBM[0];
		ClipBlit(&rp,
			0, 0,
			DestRp,
			dmi->dmi_Rectangle.MinX,
			dmi->dmi_Rectangle.MinY,
			Width, dmi->dmi_Thickness,
			ABC | ABNC);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		rp.BitMap = dmi->dmi_SaveBM[1];
		ClipBlit(&rp,
			0, 0,
			DestRp,
			dmi->dmi_Rectangle.MaxX - (dmi->dmi_Thickness - 1),
			dmi->dmi_Rectangle.MinY,
			dmi->dmi_Thickness, Height,
			ABC | ABNC);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		rp.BitMap = dmi->dmi_SaveBM[2];
		ClipBlit(&rp,
			0, 0,
			DestRp,
			dmi->dmi_Rectangle.MinX,
			dmi->dmi_Rectangle.MaxY - (dmi->dmi_Thickness - 1),
			Width, dmi->dmi_Thickness,
			ABC | ABNC);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		rp.BitMap = dmi->dmi_SaveBM[3];
		ClipBlit(&rp,
			0, 0,
			DestRp,
			dmi->dmi_Rectangle.MinX,
			dmi->dmi_Rectangle.MinY,
			dmi->dmi_Thickness, Height,
			ABC | ABNC);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		WaitBlit();
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		CleanupDropMarkInfo(dmi);
                }
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static struct DropMarkInfo *NewIconDropMarkInfo(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in)
{
	struct DropMarkInfo *dmi;

	dmi = ScalosAlloc(sizeof(struct DropMarkInfo));
	if (dmi)
		{
		memset(dmi, 0, sizeof(struct DropMarkInfo));
		dmi->dmi_Icon = in;
		AddTail(&iwt->iwt_IconDropMarkInfoList, &dmi->dmi_Node);
		}

	return dmi;
}


static struct DropMarkInfo *GetIconDropMark(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in)
{
	struct DropMarkInfo *dmi;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, in));

	for (dmi = (struct DropMarkInfo *) iwt->iwt_IconDropMarkInfoList.lh_Head;
		dmi != (struct DropMarkInfo *) &iwt->iwt_IconDropMarkInfoList.lh_Tail;
		dmi = (struct DropMarkInfo *) dmi->dmi_Node.ln_Succ)
		{
		if (dmi->dmi_Icon == in)
			{
			Remove(&dmi->dmi_Node);
			return dmi;
			}
		}

	return NULL;
}
