//
// pmtopography.h
//
// PopupMenu Library - Topographical Menu Map
//
// Copyright (C)2000 Henrik Isaksson <henrik@boing.nu>
// All Rights Reserved.
//
// $Date$
// $Revision$
//

#include "pmpriv.h"
#include "pmtopography.h"

#include <exec/lists.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>


static BOOL PM_MapShadowPart(PMTRList *top, PMSRList *delta,
	const PMSR *shrect, WORD l, WORD t, UWORD ss,
        UWORD direction, UBYTE Type);


//
// PM_AddTopographicRegion - add a rectangular region of height h to the topographic map.
//
BOOL PM_AddTopographicRegion(PMTRList *lst, WORD l, WORD t, WORD r, WORD b, WORD h)
{
	PMTR rect, *tmprect;
	PMTR *worknode, *nextnode;
	BOOL fail=FALSE;

	rect.Left=l;
	rect.Top=t;
	rect.Right=r;
	rect.Bottom=b;
	rect.Height=h;
	rect.n.Length=sizeof(PMTR);

	d1(KPrintF("Adding topographic region: %ld %ld %ld %ld, height: %ld\n", l, t, r, b, h));

	//
	// Step 1: Remove all parts in l that would overlap rect. 
	//
	worknode = (PMTR *)(lst->mlh_Head); /* First node */
	while ((nextnode = (PMTR *)PM_NextNode(worknode)))
		{
		if (PM_RegionOverlap(worknode, &rect))
			{
			PM_UnlinkRegion(lst, worknode);
			PM_SubTopographicRects(lst, worknode, &rect);
			PM_FreeTopographicNode(worknode);
			}
		worknode = nextnode;
		}

	//
	// Step 2: Add rect to l.
	//
	tmprect=PM_CopyTopographicNode(&rect);
	if (tmprect)
		{
		PM_AddRegionToList(lst, tmprect);
		}
	else
		fail = TRUE;

	return (BOOL) !fail;
}

//
// PM_CopyTopographicNode - copy a PMTopographicRect, or allocate a new if A is NULL.
//
PMTR *PM_CopyTopographicNode(PMTR *A)
{
	PMTR *newnode;

	newnode=(PMTR *) AllocVec(sizeof(PMTR), MEMF_ANY);
	if (newnode && A)
		*newnode = *A;

	return newnode;
}

//
// PM_SubTopographicRects - like PM_SubRects but preserves height
//
BOOL PM_SubTopographicRects(PMTRList *dest, PMTR *worknode, PMTR *rect)
{
	PMTR tmprect, *rectptr;
	BOOL fail = FALSE;

	tmprect.Height = worknode->Height;

	tmprect.Left = worknode->Left;
	tmprect.Right = worknode->Right;

	tmprect.n.Length = sizeof(tmprect);

	if (rect->Top>worknode->Top && rect->Top<worknode->Bottom)
		{
		tmprect.Top = worknode->Top;
		tmprect.Bottom = rect->Top;

		rectptr = PM_CopyTopographicNode(&tmprect);
		if (rectptr)
			PM_AddToList((PMDList *)dest, (PMNode *)rectptr);
		else
			fail = TRUE;
		}

	if (rect->Bottom>worknode->Top && rect->Bottom<worknode->Bottom)
		{
		tmprect.Top = rect->Bottom;
		tmprect.Bottom = worknode->Bottom;

		rectptr = PM_CopyTopographicNode(&tmprect);
		if (rectptr)
			PM_AddToList((PMDList *)dest, (PMNode *)rectptr);
		else
			fail = TRUE;
		}

	tmprect.Top = MAX(rect->Top, worknode->Top);
	tmprect.Bottom = MIN(rect->Bottom, worknode->Bottom);

	if (rect->Left>worknode->Left && rect->Left<worknode->Right)
		{
		tmprect.Left = worknode->Left;
		tmprect.Right = rect->Left;

		rectptr = PM_CopyTopographicNode(&tmprect);
		if (rectptr)
			PM_AddToList((PMDList *)dest, (PMNode *)rectptr);
		else
			fail = TRUE;
		}

	if (rect->Right>worknode->Left && rect->Right<worknode->Right)
		{
		tmprect.Left = rect->Right;
		tmprect.Right = worknode->Right;

		rectptr = PM_CopyTopographicNode(&tmprect);
		if (rectptr)
			PM_AddShadowToList((PMDList *)dest, (PMNode *)rectptr);
		else
			fail = TRUE;
		}

	return (BOOL) !fail;
}

//
// PM_MapShadow
//
BOOL PM_MapShadow(PMTRList *top, PMSRList *delta,
    WORD l, WORD t, WORD r, WORD b,
    UWORD menulevel, UWORD direction)
{
	PMSR shrect;
	BOOL Success = TRUE;
	UWORD ss;

	shrect.Left = l;
	shrect.Top = t;
	shrect.Right = r;
	shrect.Bottom = b;

	//
	// The five following calls will create shadows for a lightsource
	// positioned somewhere near the top left corner of the screen.
	//
	// +------------+
	// |            |--+
	// |            |4 |
	// |            |--+
	// |            |  |
	// |            |1 |
	// |            |  |
	// |            |  |
	// +-+-+--------+--+
	//   |5|   2    |3 |
	//   +-+--------+--+
	//

	// Shadow size
	ss = menulevel * 2 + 8;

	d1(KPrintF("%s/%s/%ld: left=%ld  top=%ld  right=%ld  bottom=%ld\n", __FILE__, __FUNC__, __LINE__, l, t, r, b));

	if (direction & PMSHADOW_HORIZ)
		{
		shrect.Right = l + ss;
		}

	if (direction & PMSHADOW_VERT)
		{
		shrect.Bottom = t + ss;
		}

	if ((PMSHADOW_HORIZ | PMSHADOW_VERT) == direction)
		{
		// bottom right corner
		// Shadow 3
		d1(KPrintF("%s/%s/%ld: Shadow 3\n", __FILE__, __FUNC__, __LINE__));
		Success	= PM_MapShadowPart(top, delta, &shrect, l, t, ss, direction, SHADOWFLAG_BOTTOM | SHADOWFLAG_RIGHT);
		}
	else if (direction & PMSHADOW_HORIZ)
		{
		d1(KPrintF("%s/%s/%ld: PMSHADOW_HORIZ\n", __FILE__, __FUNC__, __LINE__));
		// Shadow 4
		shrect.Top = t;
		shrect.Bottom = t + ss;
		Success	= PM_MapShadowPart(top, delta, &shrect, l, t, ss, direction, SHADOWFLAG_RIGHT | SHADOWFLAG_TOP);

		if (Success)
			{
			// Shadow 1
			shrect.Top = t + ss;
			shrect.Bottom = b;
			Success	= PM_MapShadowPart(top, delta, &shrect, l, t, ss, direction, SHADOWFLAG_RIGHT);
			}
		}
	else if (direction & PMSHADOW_VERT)
		{
		d1(KPrintF("%s/%s/%ld: PMSHADOW_VERT\n", __FILE__, __FUNC__, __LINE__));
		// Shadow 5
		shrect.Left = l;
		shrect.Right = l + ss;
		Success	= PM_MapShadowPart(top, delta, &shrect, l, t, ss, direction, SHADOWFLAG_BOTTOM | SHADOWFLAG_LEFT);

		if (Success)
			{
			// Shadow 2
			shrect.Left = l + ss;
			shrect.Right = r;
			Success	= PM_MapShadowPart(top, delta, &shrect, l, t, ss, direction, SHADOWFLAG_BOTTOM);
			}
		}

	return Success;
}


static BOOL PM_MapShadowPart(PMTRList *top, PMSRList *delta,
	const PMSR *shrect, WORD l, WORD t, UWORD ss,
        UWORD direction, UBYTE Type)
{
	PMTR *topnode, *nexttopnode;
	BOOL Success = TRUE;

	d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  Right=%ld  Bottom=%ld\n", \
		__FILE__, __FUNC__, __LINE__, shrect->Left, shrect->Top, shrect->Right, shrect->Bottom));

	topnode = (PMTR *)(top->mlh_Head); /* First node */
	while ((nexttopnode = (PMTR *)PM_NextNode(topnode)))
		{
		PMSR rect, *newnode;
		UWORD dx, dy;

		dx = dy = 0;

		d1(KPrintF("Mapping shadow to region: %ld %ld %ld %ld, height: %ld\n", \
			topnode->Left, topnode->Top, topnode->Right, topnode->Bottom, topnode->Height));

		if (direction & PMSHADOW_HORIZ)
			dx = topnode->Height * 2;
		if (direction & PMSHADOW_VERT)
			dy = topnode->Height * 2;

		rect.pmsr_Type = Type;

		rect.Left = MAX(shrect->Left, topnode->Left);
		rect.Right = MIN(shrect->Right-dx, topnode->Right);
		rect.Top = MAX(shrect->Top, topnode->Top);
		rect.Bottom = MIN(shrect->Bottom-dy, topnode->Bottom);
		rect.n.Length = sizeof(PMSR);
#if 0
		if (direction & PMSHADOW_LEFT)
			{
			if (rect.Left < l + ss - (topnode->Height * 2))
				rect.Left = l + ss - (topnode->Height * 2);
			}

		if (direction & PMSHADOW_TOP)
			{
			if (rect.Top < t+ss-(topnode->Height*2))
				rect.Top = t+ss-(topnode->Height*2);
			}
#endif
		d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  RIght=%ld  Bottom=%ld\n", \
			__FILE__, __FUNC__, __LINE__, rect.Left, rect.Top, rect.Right, rect.Bottom));
		if (rect.Right > rect.Left && rect.Bottom > rect.Top)
			{
			newnode = PM_CopyShadowNode(&rect);
			if (newnode)
				{
				AddHead((struct List *)delta, (struct Node *)newnode);
				}
			else
				{
				Success = FALSE;
				}
			}

		topnode = nexttopnode;
		}

	return Success;
}
