// Lasso.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classusr.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

struct LassoWindowNode
	{
	struct Node lwn_Node;	// MUST be first structure member
	struct internalScaWindowTask *lwn_WindowTask;
	};

struct LassoIconNode
	{
	struct Node lin_Node;	// MUST be first structure member
	struct Region *lin_Region;
	struct internalScaWindowTask *lin_WindowTask;
	struct ScaIconNode *lin_IconNode;
	ULONG lin_Flags;
	};

// Values in lin_Flags
#define	LINFLGB_Selected	0	// Icon has been selected by Lasso()
#define	LINFLGF_Selected	(1 << LINFLGB_Selected)

//----------------------------------------------------------------------------

// local functions

static void LassoInit(struct internalScaWindowTask *iwt, UWORD Qualifier);
static void LassoCleanup(struct internalScaWindowTask *iwt);
static ULONG LassoCleanupIconList(struct internalScaWindowTask *iwt);
static void AdjustLassoIcons(struct internalScaWindowTask *iwt,
	WORD StartX, WORD StartY, WORD StopX, WORD StopY);
static void DrawRect(struct RastPort *rp, WORD x1, WORD y1, WORD x2, WORD y2);
static void LassoScrollWindow(struct internalScaWindowTask *iwt, LONG DeltaX, LONG DeltaY);
static ULONG FillLassoIconList(struct internalScaWindowTask *iwt,
	struct internalScaWindowTask *iwtIcon, struct Rectangle *myRect,
	struct Region *Region2, struct Window *win);
static void LassoUpdateIconList(struct internalScaWindowTask *iwt);
static struct LassoWindowNode *LassoAddWindow(struct LassoInfo *li, struct internalScaWindowTask *iwt);

//----------------------------------------------------------------------------

// build list of all visible icons
static void LassoInit(struct internalScaWindowTask *iwt, UWORD Qualifier)
{
	struct Region *Region2;
	struct LassoInfo *li = &iwt->iwt_LassoInfo;

	NewList((struct List *) &li->linf_IconList);
	NewList((struct List *) &li->linf_WindowList);

	Region2 = NewRegion();
	if (NULL == Region2)
		return;

	li->linf_SingleWindow = isAlternateLssoQualifier(Qualifier);

	if (li->linf_SingleWindow)
		{
		// Lasso is limited to current window
		li->linf_Limits.MinX = li->linf_Limits.MaxX = iwt->iwt_WindowTask.wt_Window->LeftEdge + iwt->iwt_InnerLeft;
		li->linf_Limits.MaxX += iwt->iwt_InnerWidth;
		li->linf_Limits.MinY = li->linf_Limits.MaxY = iwt->iwt_WindowTask.wt_Window->TopEdge + iwt->iwt_InnerTop;
		li->linf_Limits.MaxY += iwt->iwt_InnerHeight;

		if (LassoAddWindow(li, iwt))
			{
			OrRectRegion(Region2, &li->linf_Limits);

			ScalosLockIconListShared(iwt);
			FillLassoIconList(iwt, iwt, &li->linf_Limits, Region2,
				iwt->iwt_WindowTask.wt_Window);
			// ScalosUnLockIconList is done in LassoCleanup
			}
		}
	else
		{
		// use entire screen for Lasso
		struct Region *Region1;
		struct Rectangle myRect;
		struct Layer *layer;

		li->linf_Limits.MinX = li->linf_Limits.MaxX = iwt->iwt_WinScreen->LeftEdge;
		li->linf_Limits.MinY = li->linf_Limits.MaxY = iwt->iwt_WinScreen->TopEdge;
		li->linf_Limits.MaxX += iwt->iwt_WinScreen->Width - 1;
		li->linf_Limits.MaxY += iwt->iwt_WinScreen->Height - 1;

		Region1 = NewRegion();
		if (NULL == Region1)
			{
			DisposeRegion(Region2);
			return;
			}

		myRect = li->linf_Limits;

		OrRectRegion(Region1, &myRect);

		d1(kprintf("%s/%s/%ld: Region1: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld  RegionRectangle=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, Region1->bounds.MinX, Region1->bounds.MinY, Region1->bounds.MaxX, \
			Region1->bounds.MaxY, Region1->RegionRectangle));

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		for (layer = iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo->top_layer; layer; layer=layer->back)
			{
			struct Window *win = (struct Window *) layer->Window;
			struct ScaWindowStruct *ws;

			d1(kprintf("%s/%s/%ld: layer=%08lx  Window=%08lx\n", __FILE__, __FUNC__, __LINE__, layer, win));

			for (ws=winlist.wl_WindowStruct; win && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
				{
				d1(kprintf("%s/%s/%ld: ws=%08lx  ws_Window=%08lx  win=%08lx\n", __FILE__, __FUNC__, __LINE__, ws, ws->ws_Window, win));

				if (win == ws->ws_Window)
					{
					ULONG IconsInWindow;
					struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

					myRect.MinX = myRect.MaxX = win->LeftEdge + iwtx->iwt_InnerLeft;
					myRect.MaxX += iwtx->iwt_InnerWidth - 1;

					myRect.MinY = myRect.MaxY = win->TopEdge + iwtx->iwt_InnerTop;
					myRect.MaxY += iwtx->iwt_InnerHeight - 1;

					OrRectRegion(Region2, &myRect);

					d1(kprintf("%s/%s/%ld: Region2: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld  RegionRectangle=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, Region2->bounds.MinX, Region2->bounds.MinY, Region2->bounds.MaxX, \
						Region2->bounds.MaxY, Region2->RegionRectangle));

					AndRegionRegion(Region1, Region2);

					d1(kprintf("%s/%s/%ld: Region2: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld  RegionRectangle=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, Region2->bounds.MinX, Region2->bounds.MinY, Region2->bounds.MaxX, \
						Region2->bounds.MaxY, Region2->RegionRectangle));

					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					if (!TestRegionNotEmpty(Region2))
						break;

					ScalosLockIconListShared(iwtx);

					IconsInWindow = FillLassoIconList(iwt, iwtx, &myRect, Region2, win);

					if (IconsInWindow)
						{
						if (NULL == LassoAddWindow(li, iwtx))
							{
							ScalosUnLockIconList(iwtx);
							}
						}
					else
						{
						ScalosUnLockIconList(iwtx);
						}
					}
				}

			ClearRegion(Region2);
			ClearRectRegion(Region1, &layer->bounds);
			}

		SCA_UnLockWindowList();
		DisposeRegion(Region1);
		}

	DisposeRegion(Region2);
}


static void LassoCleanup(struct internalScaWindowTask *iwt)
{
	ULONG SelectedCount;
	struct LassoWindowNode *lwn;
	struct LassoInfo *li = &iwt->iwt_LassoInfo;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	SelectedCount = LassoCleanupIconList(iwt);

	while ((lwn = (struct LassoWindowNode *) RemTail((struct List *) &li->linf_WindowList)))
		{
		ScalosUnLockIconList(lwn->lwn_WindowTask);
		ScalosFree(lwn);
		}

	IconActive = SelectedCount != 0;
}


static ULONG LassoCleanupIconList(struct internalScaWindowTask *iwt)
{
	ULONG SelectedCount = 0;
	struct LassoIconNode *lin;
	struct LassoInfo *li = &iwt->iwt_LassoInfo;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	while ((lin = (struct LassoIconNode *) RemTail((struct List *) &li->linf_IconList)))
		{
		struct ExtGadget *gg = (struct ExtGadget *) lin->lin_IconNode->in_Icon;

		if (gg->Flags & GFLG_SELECTED)
			SelectedCount++;

		if (lin->lin_Region)
			DisposeRegion(lin->lin_Region);

		ScalosFree(lin);
		}

	return SelectedCount;
}


static void AdjustLassoIcons(struct internalScaWindowTask *iwt,
	WORD StartX, WORD StartY, WORD StopX, WORD StopY)
{
	struct LassoInfo *li = &iwt->iwt_LassoInfo;
	struct LassoIconNode *lin;
	WORD temp;
	struct Rectangle LassoRect;
	struct Region *OverlapRegion = NewRegion();

	if (NULL == OverlapRegion)
		return;

	if (StartX > StopX)
		{
		temp = StartX;
		StartX = StopX;
		StopX = temp;
		}
	if (StartY > StopY)
		{
		temp = StartY;
		StartY = StopY;
		StopY = temp;
		}

	LassoRect.MinX = StartX;
	LassoRect.MaxX = StopX;
	LassoRect.MinY = StartY;
	LassoRect.MaxY = StopY;

	d1(kprintf("%s/%s/%ld: Start x1=%ld  y1=%ld  x2=%ld  y2=%ld\n", __FILE__, __FUNC__, __LINE__, StartX, StartY, StopX, StopY));

	ScaUnlockLayers(iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo);

	for (lin = (struct LassoIconNode *) li->linf_IconList.mlh_Head;
		lin != (struct LassoIconNode *) &li->linf_IconList.mlh_Tail;
		lin = (struct LassoIconNode *) lin->lin_Node.ln_Succ)
		{
		d1(kprintf("%s/%s/%ld:Icon=%08lx <%s>  x1=%ld  y1=%ld  x2=%ld  y2=%ld\n", \
			__FILE__, __FUNC__, __LINE__, lin->lin_IconNode, GetIconName(lin->lin_IconNode), \
			lin->lin_Rect.MinX, lin->lin_Rect.MinY, \
			lin->lin_Rect.MaxX, lin->lin_Rect.MaxY));

		ClearRegion(OverlapRegion);
		OrRectRegion(OverlapRegion, &LassoRect);
		AndRegionRegion(lin->lin_Region, OverlapRegion);

		if (TestRegionNotEmpty(OverlapRegion))
			{
			// Icon is inside lasso area
			d1(kprintf("%s/%s/%ld: Inside\n", __FILE__, __FUNC__, __LINE__));

			lin->lin_Flags |= LINFLGF_Selected;

			ClassSelectIcon(lin->lin_WindowTask->iwt_WindowTask.mt_WindowStruct, lin->lin_IconNode, TRUE);
			}
		else
			{
			d1(kprintf("%s/%s/%ld: Outside\n", __FILE__, __FUNC__, __LINE__));

			// Icon is outside lasso area
			if (lin->lin_Flags & LINFLGF_Selected)
				{
				ClassSelectIcon(lin->lin_WindowTask->iwt_WindowTask.mt_WindowStruct, lin->lin_IconNode, FALSE);
				lin->lin_Flags &= ~LINFLGF_Selected;
				}
			}
		}

	DisposeRegion(OverlapRegion);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	ScaLockLayers(iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo);
}


// Draw Lasso
void Lasso(WORD StartX, WORD StartY, ULONG Mode, struct internalScaWindowTask *iwt)
{
	struct RastPort *rp = &iwt->iwt_WinScreen->RastPort;
	struct LassoInfo *li = &iwt->iwt_LassoInfo;
	ULONG ul;
	WORD x, y;
	WORD EndX, EndY;
	LONG DeltaX = 0;
	LONG DeltaY = 0;

	EndX = iwt->iwt_WinScreen->MouseX;
	EndY = iwt->iwt_WinScreen->MouseY;

	d1(kprintf("%s/%s/%ld: Start x1=%ld  y1=%ld  x2=%ld  y2=%ld  Mode=%08lx\n", __FILE__, __FUNC__, __LINE__, StartX, StartY, EndX, EndY, Mode));

	StartX += iwt->iwt_WindowTask.wt_Window->LeftEdge;
	StartY += iwt->iwt_WindowTask.wt_Window->TopEdge;

	// Limit lasso rectangle to iwt_LassoInfo.linf_Limits
	if (EndX < li->linf_Limits.MinX)
		{
		DeltaX = EndX - li->linf_Limits.MinX;
		EndX = li->linf_Limits.MinX;
		}
	else if (EndX > li->linf_Limits.MaxX)
		{
		DeltaX = EndX - li->linf_Limits.MaxX;
		EndX = li->linf_Limits.MaxX;
		d1(kprintf("%s/%s/%ld: EndX=%ld  MaxX=%ld\n", __FILE__, __FUNC__, __LINE__, EndX, li->linf_Limits.MaxX));
		}
	if (EndY < li->linf_Limits.MinY)
		{
		DeltaY = EndY - li->linf_Limits.MinY;
		EndY = li->linf_Limits.MinY;
		}
	else if (EndY > li->linf_Limits.MaxY)
		{
		DeltaY = EndY - li->linf_Limits.MaxY;
		EndY = li->linf_Limits.MaxY;
		d1(kprintf("%s/%s/%ld: EndY=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, EndY, li->linf_Limits.MaxY));
		}

	SetAPen(rp, 1);
	SetDrMd(rp, COMPLEMENT);

	if (LASSOX_RemoveLasso != (UWORD) li->linf_OldPos.X)
		{
		// remove old lasso

		x = li->linf_OldPos.X;
		y = li->linf_OldPos.Y;

		d1(kprintf("%s/%s/%ld: Erase Old x1=%ld  y1=%ld  x2=%ld  y2=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, StartX, StartY));

		DrawRect(rp, x, y, StartX, StartY);

		d1(KPrintF("%s/%s/%ld: OldDrag.X=%ld  OldDrag.Y=%ld\n", __FILE__, __FUNC__, __LINE__, li->linf_OldPos.X, li->linf_OldPos.Y));
		}

	if (LASSOX_FollowLasso == Mode)
		{
		if ((0 != DeltaX || 0 != DeltaY) && !iwt->iwt_BackDrop)
			{
			LassoScrollWindow(iwt, DeltaX, DeltaY);
			LassoUpdateIconList(iwt);

			SetAPen(rp, 1);
			SetDrMd(rp, COMPLEMENT);
			}

		AdjustLassoIcons(iwt, StartX, StartY, EndX, EndY);
		}

	switch (Mode)
		{
	case LASSOX_RemoveLasso:
		d1(kprintf("%s/%s/%ld: Remove Lasso\n", __FILE__, __FUNC__, __LINE__));
		break;

	case LASSOX_CrawlLasso:
		d1(kprintf("%s/%s/%ld: Crawl Lasso\n", __FILE__, __FUNC__, __LINE__));

		rp->linpatcnt = 0;

		ul = rp->LinePtrn << 16;
		ul >>= CurrentPrefs.pref_DefLineSpeed;
		ul |= ul >> 16;
		rp->LinePtrn = ul;

		d1(KPrintF("%s/%s/%ld: OldDrag.X=%ld  OldDrag.Y=%ld\n", __FILE__, __FUNC__, __LINE__, li->linf_OldPos.X, li->linf_OldPos.Y));

		/* intentionally no break here ! */		
	case LASSOX_FollowLasso:
		li->linf_OldPos.X = x = EndX;
		li->linf_OldPos.Y = y = EndY;

		d1(KPrintF("%s/%s/%ld: OldDrag.X=%ld  OldDrag.Y=%ld\n", __FILE__, __FUNC__, __LINE__, li->linf_OldPos.X, li->linf_OldPos.Y));
		d1(kprintf("%s/%s/%ld: Draw New x1=%ld  y1=%ld  x2=%ld  y2=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, StartX, StartY));

		DrawRect(rp, x, y, StartX, StartY);
		break;
		}
}


// start lasso operation
void BeginLasso(struct internalScaWindowTask *iwt, UWORD Qualifier)
{
	struct LassoInfo *li = &iwt->iwt_LassoInfo;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	iwt->iwt_LassoFlag = TRUE;

	LassoInit(iwt, Qualifier);

	ScalosObtainSemaphore(&LayersSema);

	iwt->iwt_LockFlag = TRUE;
	iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_NeedsTimerMsg;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	ScaLockLayers(iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo);

	li->linf_OldPos.X = LASSOX_RemoveLasso;

	iwt->iwt_WinScreen->RastPort.LinePtrn = CurrentPrefs.pref_DefaultLinePattern;

	Lasso(iwt->iwt_OldMouse.X, iwt->iwt_OldMouse.Y, LASSOX_FollowLasso, iwt);
}


// end lasso operation
void EndLasso(struct internalScaWindowTask *iwt)
{
	if (iwt->iwt_LassoFlag)
		{
		d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

		Lasso(iwt->iwt_OldMouse.X, iwt->iwt_OldMouse.Y, LASSOX_RemoveLasso, iwt);

		LassoCleanup(iwt);

		iwt->iwt_LassoFlag = FALSE;

		iwt->iwt_WinScreen->RastPort.LinePtrn = 0xffff;

		ScaUnlockLayers(iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo);

		iwt->iwt_LockFlag = FALSE;

		// do not turn off WSV_FlagF_NeedsTimerMsg for desktop window (required for screen title update
		if (WSV_Type_DesktopWindow != iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType)
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_NeedsTimerMsg;

		ScalosReleaseSemaphore(&LayersSema);
		}
}


static void DrawRect(struct RastPort *rp, WORD x1, WORD y1, WORD x2, WORD y2)
{
	Move(rp, x1, y1);	// Left-Top
	Draw(rp, x2, y1);	// Right-Top
	Draw(rp, x2, y2);	// Right-Bottom
	Draw(rp, x1, y2);	// Left-Bottom
	Draw(rp, x1, y1);	// Left-Top
}


static void LassoScrollWindow(struct internalScaWindowTask *iwt, LONG DeltaX, LONG DeltaY)
{
	UWORD DeltaMoveFlags = 0;

	ScaUnlockLayers(iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo);

	if (DeltaY)
		DeltaMoveFlags |= SETVIRTF_AdjustRightSlider;
	if (DeltaX)
		DeltaMoveFlags |= SETVIRTF_AdjustBottomSlider;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_DeltaMove, DeltaX, DeltaY);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_SetVirtSize, DeltaMoveFlags);

	ScaLockLayers(iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo);
}


static ULONG FillLassoIconList(struct internalScaWindowTask *iwt,
	struct internalScaWindowTask *iwtIcon, struct Rectangle *myRect,
	struct Region *Region2, struct Window *win)
{
	struct LassoInfo *li = &iwt->iwt_LassoInfo;
	struct Region *Region3;
	ULONG IconsInWindow = 0;

	Region3 = NewRegion();

	d1(kprintf("%s/%s/%ld: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld\n", \
		__FILE__, __FUNC__, __LINE__, myRect->MinX, myRect->MinY, myRect->MaxX, myRect->MaxY));

	if (Region3)
		{
		struct ScaIconNode *in;

		for (in = iwtIcon->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct Rectangle IconRect;
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", \
				__FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
			d1(KPrintF("%s/%s/%ld: Left=%ld  BoundsLeft=%ld  Width=%ld  BoundsWidth=%ld\n", \
				__FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->BoundsLeftEdge, gg->Width, gg->BoundsWidth));

			IconRect.MinX = IconRect.MaxX = win->LeftEdge + gg->LeftEdge - iwtIcon->iwt_WindowTask.wt_XOffset + iwtIcon->iwt_InnerLeft;
			IconRect.MinY = IconRect.MaxY = win->TopEdge + gg->TopEdge - iwtIcon->iwt_WindowTask.wt_YOffset + iwtIcon->iwt_InnerTop;
			IconRect.MaxX += gg->Width - 1;
			IconRect.MaxY += gg->Height - 1;

			d1(kprintf("%s/%s/%ld: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld\n", \
				__FILE__, __FUNC__, __LINE__, IconRect.MinX, IconRect.MinY, IconRect.MaxX, IconRect.MaxY));

			if (IconRect.MinX < myRect->MaxX && IconRect.MinY < myRect->MaxY &&
				IconRect.MaxX >= myRect->MinX &&
				IconRect.MaxY >= myRect->MinY)
				{
				OrRectRegion(Region3, &IconRect);
				AndRegionRegion(Region2, Region3);

				if (TestRegionNotEmpty(Region3))
					{
					struct LassoIconNode *lin;

					d1(kprintf("%s/%s/%ld: in=%08lx <%s>\n", \
						__FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

					lin = (struct LassoIconNode *) ScalosAlloc(sizeof(struct LassoIconNode));
					if (lin)
						{
						IconsInWindow++;

						lin->lin_Flags = 0;
						lin->lin_WindowTask = iwtIcon;
						lin->lin_IconNode = in;

						lin->lin_Region = NewRegion();

						if (lin->lin_Region)
							OrRegionRegion(Region3, lin->lin_Region);

						AddTail((struct List *) &li->linf_IconList,
							&lin->lin_Node);

						d1(kprintf("%s/%s/%ld: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld\n", \
							__FILE__, __FUNC__, __LINE__, lin->lin_Rect.MinX, lin->lin_Rect.MinY, lin->lin_Rect.MaxX, lin->lin_Rect.MaxY));
						}
					}
				}

			ClearRegion(Region3);
			}
		DisposeRegion(Region3);
		}

	return IconsInWindow;
}


// Update the list of visible icons -- required after scrolling the window
static void LassoUpdateIconList(struct internalScaWindowTask *iwt)
{
	struct LassoInfo *li = &iwt->iwt_LassoInfo;
	struct Region *Region2;

	(void) LassoCleanupIconList(iwt);

	Region2 = NewRegion();

	if (Region2)
		{
		OrRectRegion(Region2, &li->linf_Limits);

		ScalosLockIconListShared(iwt);
		FillLassoIconList(iwt, iwt, &li->linf_Limits, Region2,
			iwt->iwt_WindowTask.wt_Window);
		ScalosUnLockIconList(iwt);

		DisposeRegion(Region2);
		}
}


static struct LassoWindowNode *LassoAddWindow(struct LassoInfo *li, struct internalScaWindowTask *iwt)
{
	struct LassoWindowNode *lwn;

	lwn = (struct LassoWindowNode *) ScalosAlloc(sizeof(struct LassoWindowNode));
	if (lwn)
		{
		lwn->lwn_WindowTask = iwt;

		AddTail((struct List *) &li->linf_WindowList,
			&lwn->lwn_Node);
		}

	return lwn;
}

