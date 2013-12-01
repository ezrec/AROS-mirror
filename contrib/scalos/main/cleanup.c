// cleanup.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classusr.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/timer.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <intuition/classusr.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>

#include "scalos_structures.h"
#include "Variables.h"
#include "functions.h"

//----------------------------------------------------------------------------

#define TILE_SIZE_X	4
#define TILE_SIZE_Y	4

//----------------------------------------------------------------------------

enum PlacementMode
	{
	PLACEMENT_TopDown,
	PLACEMENT_LeftRight,
	};

struct TilesInfo
	{
	ULONG tli_WindowWidth;		// Width of tile array, in window coordinates
	ULONG tli_WindowHeight;         // Height of tile array, in window coordinates
	ULONG tli_NumberOfRows;		// Total number of rows in tile array
	ULONG tli_NumberOfColumns;      // Total number of columns in tile array
	ULONG tli_BytesPerRow;		// Number of octets per row in tile array
	enum PlacementMode tli_Placement;
	UBYTE tli_TileMemory[1];        // Array of allocated tiles, one bit per tile
	};

//----------------------------------------------------------------------------

static BOOL INLINE TestTilesLocation(const struct TilesInfo *tli, LONG x, LONG y);
static void INLINE MarkTilesLocation(struct TilesInfo *tli, LONG x, LONG y);
static LONG INLINE LimitTo(LONG Value, LONG MinVal, LONG MaxVal);
static struct TilesInfo *CreateTilesInfo(const struct internalScaWindowTask *iwt, 
	enum PlacementMode Placement, const struct Rect32 *AreaBounds);
static void DisposeTilesInfo(struct TilesInfo *tli);
static BOOL IsSpaceFreeInTilesArray(const struct TilesInfo *tli, const struct Rect32 *pos);
static void MarkSpaceInTilesArray(struct TilesInfo *tli, const struct Rect32 *pos);
static void IconPositionToTilePosition(const struct internalScaWindowTask *iwt,
	const struct TilesInfo *tli,
	const struct Rect32 *WindowPosition, struct Rect32 *TilePosition);
static void AddIconToTilesInfo(const struct internalScaWindowTask *iwt,
	struct TilesInfo *tli, const struct ScaIconNode *in);
static void FindFreeLocationInTilesInfoTopDown(const struct internalScaWindowTask *iwt,
     struct TilesInfo *tli, struct ScaIconNode *in, LONG XStart, LONG YStart,
     const struct Rect32 *LayoutAreaBounds, LONG *x, LONG *y);
static void FindFreeLocationInTilesInfoLeftRight(const struct internalScaWindowTask *iwt,
	struct TilesInfo *tli, struct ScaIconNode *in, LONG XStart, LONG YStart,
	const struct Rect32 *LayoutAreaBounds, LONG *x, LONG *y);
static void LayoutIconsTopDown(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds, const struct Rect32 *MaxAreaBounds);
static void SimpleLayoutIconsTopDown(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds);
static void LayoutIconsLeftRight(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds, const struct Rect32 *MaxAreaBounds);
static void SimpleLayoutIconsLeftRight(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds);
static void MoveIconToPositionXY(struct ScaIconNode *in, LONG x, LONG y);
static void GetIconBoundingBox(const struct ScaIconNode *in, struct Rect32 *Bounds);
static void DrawIconList(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList);
static void RedrawIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static void CalculateMaximumIconAreaDimensions(const struct internalScaWindowTask *iwt, struct Rect32 *Bounds);
static void AddPositionedIconsToTiles(const struct internalScaWindowTask *iwt,
	struct TilesInfo *tli);

//----------------------------------------------------------------------------

void IconWindow_UnCleanup(struct internalScaWindowTask *iwt, struct Region *UnCleanUpRegion)
{
	struct ScaIconNode *in, *inNext;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	ScalosLockIconListExclusive(iwt);

	d1(KPrintF("%s/%s/%ld:\n", __LINE__));

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>  inFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), in->in_Flags));

		if (in->in_Flags & INF_FreeIconPosition)
			{
			BOOL MoveIcon = FALSE;
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

			if (UnCleanUpRegion)
				{
				// Do not uncleanup any icon which lies outside of <UnCleanUpRegion>
				struct Rectangle IconRect;

				IconRect.MinX = IconRect.MaxX = gg->BoundsLeftEdge;
				IconRect.MinY = IconRect.MaxY = gg->BoundsTopEdge;
				IconRect.MaxX += gg->BoundsWidth;
				IconRect.MaxY += gg->BoundsHeight;

				if (ScaRectInRegion(UnCleanUpRegion, &IconRect))
					MoveIcon = TRUE;
				}
			else
				{
				MoveIcon = TRUE;
				}

			if (MoveIcon)
				{
				in->in_OldLeftEdge = gg->LeftEdge;
				in->in_OldTopEdge = gg->TopEdge;

				in->in_Flags |= INF_IconVisible;

				SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList,
					&iwt->iwt_WindowTask.wt_LateIconList, in);
				}
			}
		}

	ScalosUnLockIconList(iwt);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void IconWindow_Cleanup(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	TIMESTAMP_d1();

	ScalosLockIconListExclusive(iwt);

	if (iwt->iwt_WindowTask.wt_LateIconList)
		{
		// This list collects all (re)positioned icons
		struct ScaIconNode *RedrawIconList = NULL;
		struct Rect32 LayoutAreaBounds;
		struct ScaIconNode *in, *inNext;
		struct ScaIconNode *LateIconList;
		struct ScaIconNode *RowLayoutIconList = NULL;
		struct Rect32 MaxAreaBounds;

		CalculateMaximumIconAreaDimensions(iwt, &MaxAreaBounds);

		d1(KPrintF("%s/%s/%ld: MaxAreaBounds: MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, MaxAreaBounds.MinX, MaxAreaBounds.MinY, \
			MaxAreaBounds.MaxX, MaxAreaBounds.MaxY));

		LateIconList = iwt->iwt_WindowTask.wt_LateIconList;
		iwt->iwt_WindowTask.wt_LateIconList = NULL;

		ScalosUnLockIconList(iwt);


		// Move all icons with ICONLAYOUT_Rows to RowLayoutIconList
		for (in = LateIconList; in; in = inNext)
			{
			enum IconLayoutMode LayoutMode;
			ULONG IconType;

			inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

			GetAttr(IDTA_Type, in->in_Icon, &IconType);

			if (IconType < sizeof(iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes))
				LayoutMode = iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes[IconType];
			else
				LayoutMode = ICONLAYOUT_Columns;	// default

			if (ICONLAYOUT_Rows == LayoutMode)
				{
				SCA_MoveIconNode(&LateIconList,
					&RowLayoutIconList, in);
				}
			}

		TIMESTAMP_d1();

		if (RowLayoutIconList)
			{
			// Layout all icons with ICONLAYOUT_Rows mode
			LayoutAreaBounds.MinX = CurrentPrefs.pref_CleanupSpace.Left + iwt->iwt_WindowTask.wt_XOffset;
			LayoutAreaBounds.MinY = CurrentPrefs.pref_CleanupSpace.Top + iwt->iwt_WindowTask.wt_YOffset;
			LayoutAreaBounds.MaxX = iwt->iwt_InnerWidth + iwt->iwt_WindowTask.wt_XOffset;
			LayoutAreaBounds.MaxY = SHRT_MAX;

			while (MaxAreaBounds.MaxY > SHRT_MAX)
				{
				LayoutAreaBounds.MaxX *= 2;
				MaxAreaBounds.MaxY /= 2;
				}

			LayoutIconsLeftRight(iwt, &RedrawIconList,
				&RowLayoutIconList, &LayoutAreaBounds, &MaxAreaBounds);
			TIMESTAMP_d1();

			// Redraw all icons in RedrawIconList and move them to wt_IconList
			DrawIconList(iwt, &RedrawIconList);
			}

		TIMESTAMP_d1();

		if (LateIconList)
			{
			// Layout all remaining icons with ICONLAYOUT_Columns mode
			LayoutAreaBounds.MinX = CurrentPrefs.pref_CleanupSpace.Left + iwt->iwt_WindowTask.wt_XOffset;
			LayoutAreaBounds.MinY = CurrentPrefs.pref_CleanupSpace.Top + iwt->iwt_WindowTask.wt_YOffset;
			LayoutAreaBounds.MaxX = SHRT_MAX;
			LayoutAreaBounds.MaxY = iwt->iwt_InnerHeight + iwt->iwt_WindowTask.wt_YOffset;

			while (MaxAreaBounds.MaxX > SHRT_MAX)
				{
				LayoutAreaBounds.MaxY *= 2;
				MaxAreaBounds.MaxX /= 2;
				}

			LayoutIconsTopDown(iwt, &RedrawIconList,
				&LateIconList, &LayoutAreaBounds, &MaxAreaBounds);
			TIMESTAMP_d1();

			// Redraw all icons in RedrawIconList and move them to wt_IconList
			DrawIconList(iwt, &RedrawIconList);
			}
		}
	else
		{
		ScalosUnLockIconList(iwt);
		}

	TIMESTAMP_d1();

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static BOOL INLINE TestTilesLocation(const struct TilesInfo *tli, LONG x, LONG y)
{
	ULONG BitMask = 1 << (x & (CHAR_BIT - 1));
	const UBYTE *p = tli->tli_TileMemory
		+ (y * tli->tli_BytesPerRow) + (x / CHAR_BIT);

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  p=%08lx  BitMask=%02lx\n", \
		__FILE__, __FUNC__, __LINE__, x, y, p, BitMask));

	return (BOOL) (*p & BitMask);
}


static void INLINE MarkTilesLocation(struct TilesInfo *tli, LONG x, LONG y)
{
	ULONG BitMask = 1 << (x & (CHAR_BIT - 1));
	UBYTE *p = tli->tli_TileMemory
		+ (y * tli->tli_BytesPerRow) + (x / CHAR_BIT);

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  p=%08lx  BitMask=%02lx\n", \
		__FILE__, __FUNC__, __LINE__, x, y, p, BitMask));

	*p |= BitMask;
}


static LONG INLINE LimitTo(LONG Value, LONG MinVal, LONG MaxVal)
{
	if (Value < MinVal)
		Value = MinVal;
	if (Value > MaxVal)
		Value = MaxVal;

	return Value;
}


static struct TilesInfo *CreateTilesInfo(const struct internalScaWindowTask *iwt, 
	enum PlacementMode Placement, const struct Rect32 *AreaBounds)
{
	struct TilesInfo *tli;
	LONG TilesArrayWidth, TilesArrayHeight;
	ULONG NumberOfColumns;
	ULONG NumberOfRows;
	size_t TilesLength;
	ULONG BytesPerRow;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	switch (Placement)
		{
	case PLACEMENT_TopDown:
		TilesArrayHeight = iwt->iwt_InnerHeight;
		TilesArrayWidth = 1 + AreaBounds->MaxX;
		break;
	case PLACEMENT_LeftRight:
		TilesArrayWidth = iwt->iwt_InnerWidth;
		TilesArrayHeight = 1 + AreaBounds->MaxY;
		break;
	default:
		return NULL;
		break;
		}

	// make sure tiles array is alway at least as large as inner window area
	if (TilesArrayWidth < iwt->iwt_InnerWidth)
		TilesArrayWidth = iwt->iwt_InnerWidth;
	if (TilesArrayHeight < iwt->iwt_InnerHeight)
		TilesArrayHeight = iwt->iwt_InnerHeight;

	d1(KPrintF("%s/%s/%ld: TilesArrayWidth=%lu  TilesArrayHeight=%lu\n", \
		__FILE__, __FUNC__, __LINE__, TilesArrayWidth, TilesArrayHeight));

	NumberOfColumns = TilesArrayWidth / TILE_SIZE_X;
	NumberOfRows = TilesArrayHeight / TILE_SIZE_Y;

	if (0 == NumberOfColumns || 0 == NumberOfRows)
		return NULL;

	BytesPerRow = (NumberOfColumns + CHAR_BIT - 1) / CHAR_BIT;	// round up to ceiling
	TilesLength = NumberOfRows * BytesPerRow;

	tli = ScalosAlloc(sizeof(struct TilesInfo) + TilesLength);
	if (NULL == tli)
		return NULL;

	tli->tli_NumberOfColumns = NumberOfColumns;
	tli->tli_NumberOfRows = NumberOfRows;
	tli->tli_WindowWidth = TilesArrayWidth;
	tli->tli_WindowHeight = TilesArrayHeight;
	tli->tli_BytesPerRow = BytesPerRow;		// round up to ceiling
	tli->tli_Placement = Placement;

	d1(KPrintF("%s/%s/%ld: NumberOfColumns=%lu  NumberOfRows=%lu  AllocSize=%lu  BytesPerRow=%lu\n", \
		__FILE__, __FUNC__, __LINE__, NumberOfColumns, NumberOfRows, tli->tli_BytesPerRow));

	// mark all tiles as free
	memset(tli->tli_TileMemory, 0, TilesLength);

	d1(KPrintF("%s/%s/%ld: END  tli=%08lx\n", __FILE__, __FUNC__, __LINE__, tli));

	return tli;
}


static void DisposeTilesInfo(struct TilesInfo *tli)
{
	d1(KPrintF("%s/%s/%ld: START  tli=%08lx\n", __FILE__, __FUNC__, __LINE__, tli));
	ScalosFree(tli);
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static BOOL IsSpaceFreeInTilesArray(const struct TilesInfo *tli, const struct Rect32 *pos)
{
	LONG x;

	d1(KPrintF("%s/%s/%ld: START  tli=%08lx  MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tli, pos->MinX, pos->MinY, pos->MaxX, pos->MaxY));

	// areas outside of tiles array are considered occupied
	if (!(pos->MinX >= 0 && pos->MinX < tli->tli_NumberOfColumns)
		|| !(pos->MinY >= 0 && pos->MinY < tli->tli_NumberOfRows)
		|| !(pos->MaxX >= 0 && pos->MaxX < tli->tli_NumberOfColumns)
		|| !(pos->MaxY >= 0 && pos->MaxY < tli->tli_NumberOfRows))
		return FALSE;

	for (x = pos->MinX; x <= pos->MaxX; x++)
		{
		LONG y;

		for (y = pos->MinY; y <= pos->MaxY; y++)
			{
			if (TestTilesLocation(tli, x, y))
				{
				// This tile is occupied
				return FALSE;
				}
			}
		}

	// All tiles required for <pos> are free
	return TRUE;
}


static void MarkSpaceInTilesArray(struct TilesInfo *tli, const struct Rect32 *pos)
{
	struct Rect32 Bounds;
	LONG x;

	d1(KPrintF("%s/%s/%ld: START  tli=%08lx  MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tli, pos->MinX, pos->MinY, pos->MaxX, pos->MaxY));

	// make sure marking is limited to size of tiles array
	Bounds.MinX = LimitTo(pos->MinX, 0, tli->tli_NumberOfColumns);
	Bounds.MaxX = LimitTo(pos->MaxX, 0, tli->tli_NumberOfColumns);
	Bounds.MinY = LimitTo(pos->MinY, 0, tli->tli_NumberOfRows);
	Bounds.MaxY = LimitTo(pos->MaxY, 0, tli->tli_NumberOfRows);

	for (x = Bounds.MinX; x <= Bounds.MaxX; x++)
		{
		LONG y;

		for (y = Bounds.MinY; y <= Bounds.MaxY; y++)
			{
			MarkTilesLocation(tli, x, y);
			}
		}
}

static void IconPositionToTilePosition(const struct internalScaWindowTask *iwt,
	const struct TilesInfo *tli,
	const struct Rect32 *WindowPosition, struct Rect32 *TilePosition)
{
	d1(KPrintF("%s/%s/%ld: START  tli=%08lx  MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tli, WindowPosition->MinX, WindowPosition->MinY, WindowPosition->MaxX, WindowPosition->MaxY));

	TilePosition->MinX = (WindowPosition->MinX + iwt->iwt_WindowTask.wt_XOffset) / TILE_SIZE_X;
	TilePosition->MinY = (WindowPosition->MinY + iwt->iwt_WindowTask.wt_YOffset) / TILE_SIZE_Y;
	TilePosition->MaxX = (WindowPosition->MaxX + iwt->iwt_WindowTask.wt_XOffset + (TILE_SIZE_X - 1)) / TILE_SIZE_X;
	TilePosition->MaxY = (WindowPosition->MaxY + iwt->iwt_WindowTask.wt_YOffset + (TILE_SIZE_Y - 1)) / TILE_SIZE_Y;;

	TilePosition->MinX = LimitTo(TilePosition->MinX, 0, tli->tli_NumberOfColumns - 1);
	TilePosition->MinY = LimitTo(TilePosition->MinY, 0, tli->tli_NumberOfRows - 1);
	TilePosition->MaxX = LimitTo(TilePosition->MaxX, TilePosition->MinX, tli->tli_NumberOfColumns - 1);
	TilePosition->MaxY = LimitTo(TilePosition->MaxY, TilePosition->MinY, tli->tli_NumberOfRows - 1);

	d1(KPrintF("%s/%s/%ld: END  tli=%08lx  MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tli, TilePosition->MinX, TilePosition->MinY, TilePosition->MaxX, TilePosition->MaxY));
}


static void AddIconToTilesInfo(const struct internalScaWindowTask *iwt,
	struct TilesInfo *tli, const struct ScaIconNode *in)
{
	struct Rect32 IconBounds;
	struct Rect32 TilesArrayPos;

	d1(KPrintF("%s/%s/%ld: START  tli=%08lx  in=%08lx <%s>\n", \
		__FILE__, __FUNC__, __LINE__, tli, in, GetIconName((struct ScaIconNode *) in)));

	GetIconBoundingBox(in, &IconBounds);

	d1(KPrintF("%s/%s/%ld: MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IconBounds.MinX, IconBounds.MinY, \
		IconBounds.MaxX, IconBounds.MaxY));

	IconPositionToTilePosition(iwt, tli, &IconBounds, &TilesArrayPos);
	MarkSpaceInTilesArray(tli, &TilesArrayPos);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void FindFreeLocationInTilesInfoTopDown(const struct internalScaWindowTask *iwt,
	struct TilesInfo *tli, struct ScaIconNode *in, LONG XStart, LONG YStart,
	const struct Rect32 *LayoutAreaBounds, LONG *x, LONG *y)
{
	ULONG IconWidth, IconHeight;
	struct Rect32 IconBounds;
	BOOL SpaceIsOccupied;

	d1(KPrintF("%s/%s/%ld: START  tli=%08lx  in=%08lx <%s>  startx=%ld  starty=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tli, in, GetIconName(in), XStart, YStart));
	d1(KPrintF("%s/%s/%ld: tli_NumberOfColumns=%ld  tli_NumberOfRows=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tli->tli_NumberOfColumns, tli->tli_NumberOfRows));

	GetIconBoundingBox(in, &IconBounds);

	IconWidth = 1 + IconBounds.MaxX - IconBounds.MinX;
	IconHeight = 1 + IconBounds.MaxY - IconBounds.MinY;

	IconBounds.MinX = XStart;
	IconBounds.MinY = YStart;
	IconBounds.MaxX = IconBounds.MinX + IconWidth;
	IconBounds.MaxY = IconBounds.MinY + IconHeight;

	d1(KPrintF("%s/%s/%ld: IconBounds  MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IconBounds.MinX, IconBounds.MaxX,\
		IconBounds.MinY, IconBounds.MaxY));
	d1(KPrintF("%s/%s/%ld: LayoutAreaBounds  MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, LayoutAreaBounds->MinX, LayoutAreaBounds->MaxX,\
		LayoutAreaBounds->MinY, LayoutAreaBounds->MaxY));

	do 	{
		struct Rect32 TilePosition;

		SpaceIsOccupied = FALSE;

		IconPositionToTilePosition(iwt, tli, &IconBounds, &TilePosition);

		if (!IsSpaceFreeInTilesArray(tli, &TilePosition))
			{
			SpaceIsOccupied = TRUE;

			IconBounds.MinY += TILE_SIZE_Y;
			IconBounds.MaxY = IconBounds.MinY + IconHeight - 1;

			if (IconBounds.MaxY + CurrentPrefs.pref_CleanupSpace.YSkip > LayoutAreaBounds->MaxY)
				{
				// Icon doesn't fit into column - move to the next one

					{
					d1(KPrintF("%s/%s/%ld: MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
						__FILE__, __FUNC__, __LINE__, IconBounds.MinX, IconBounds.MaxX,\
						IconBounds.MinY, IconBounds.MaxY));
					d1(KPrintF("%s/%s/%ld: TilePosition MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
						__FILE__, __FUNC__, __LINE__, TilePosition.MinX, TilePosition.MaxX,\
						TilePosition.MinY, TilePosition.MaxY));
					}

				IconBounds.MinY = CurrentPrefs.pref_CleanupSpace.Top + TILE_SIZE_Y;
				IconBounds.MaxY = IconBounds.MinY + IconHeight;

				IconBounds.MinX += TILE_SIZE_X;
				IconBounds.MaxX = IconBounds.MinX + IconWidth;

				if (IconBounds.MinX > 32000)
				d1(KPrintF("%s/%s/%ld: MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
					__FILE__, __FUNC__, __LINE__, IconBounds.MinX, IconBounds.MaxX,\
					IconBounds.MinY, IconBounds.MaxY));
				}
			}
		} while (SpaceIsOccupied && (IconBounds.MaxX <= LayoutAreaBounds->MaxX));

	d1(KPrintF("%s/%s/%ld: MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IconBounds.MinX, IconBounds.MaxX,\
		IconBounds.MinY, IconBounds.MaxY));

	*x = IconBounds.MinX;
	*y = IconBounds.MinY;

	d1(KPrintF("%s/%s/%ld: END  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, *x, *y));
}


static void FindFreeLocationInTilesInfoLeftRight(const struct internalScaWindowTask *iwt,
	struct TilesInfo *tli, struct ScaIconNode *in, LONG XStart, LONG YStart,
	const struct Rect32 *LayoutAreaBounds, LONG *x, LONG *y)
{
	ULONG IconWidth, IconHeight;
	struct Rect32 IconBounds;
	BOOL SpaceIsOccupied;

	d1(KPrintF("%s/%s/%ld: START  tli=%08lx  in=%08lx <%s>  startx=%ld  starty=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tli, in, GetIconName(in), XStart, YStart));

	GetIconBoundingBox(in, &IconBounds);

	IconWidth = 1 + IconBounds.MaxX - IconBounds.MinX;
	IconHeight = 1 + IconBounds.MaxY - IconBounds.MinY;

	IconBounds.MinX = XStart;
	IconBounds.MinY = YStart;
	IconBounds.MaxX = IconBounds.MinX + IconWidth;
	IconBounds.MaxY = IconBounds.MinY + IconHeight;

	do 	{
		struct Rect32 TilePosition;

		SpaceIsOccupied = FALSE;

		IconPositionToTilePosition(iwt, tli, &IconBounds, &TilePosition);

		if (!IsSpaceFreeInTilesArray(tli, &TilePosition))
			{
			SpaceIsOccupied = TRUE;

			IconBounds.MinX += TILE_SIZE_X;
			IconBounds.MaxX = IconBounds.MinX + IconWidth - 1;

			if (IconBounds.MaxX + CurrentPrefs.pref_CleanupSpace.XSkip > LayoutAreaBounds->MaxX)
				{
				// Icon doesn't fit into row - move to the next one
				IconBounds.MinX = CurrentPrefs.pref_CleanupSpace.Left + TILE_SIZE_X;
				IconBounds.MaxX = IconBounds.MinX + IconWidth;

				IconBounds.MinY += TILE_SIZE_Y;
				IconBounds.MaxY = IconBounds.MinY + IconHeight;
				}
			}
		} while (SpaceIsOccupied && (IconBounds.MaxY <= LayoutAreaBounds->MaxY));

	d1(KPrintF("%s/%s/%ld: MinX=%ld MaxX=%ld  MinY=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, IconBounds.MinX, IconBounds.MaxX,\
		IconBounds.MinY, IconBounds.MaxY));

	*x = IconBounds.MinX;
	*y = IconBounds.MinY;

	d1(KPrintF("%s/%s/%ld: END  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, *x, *y));
}


static void LayoutIconsTopDown(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds, const struct Rect32 *MaxAreaBounds)
{
	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	// Check which icons have not been positioned
	if (iwt->iwt_WindowTask.wt_IconList)
		{
		LONG x, y;
		struct Rect32 IconBounds;
		struct TilesInfo *tli;

		tli = CreateTilesInfo(iwt, PLACEMENT_TopDown, MaxAreaBounds);
		d1(KPrintF("%s/%s/%ld: tli=%08lx\n", __FILE__, __FUNC__, __LINE__, tli));
		if (tli)
			{
			struct ScaIconNode *in, *inNext;

			// Add all placed icons to tiles array,
			// and mark their positions as occupied
			AddPositionedIconsToTiles(iwt, tli);

			// Start the icon placement at (LayoutAreaBounds->MinX, LayoutAreaBounds->MinY)
			x = LayoutAreaBounds->MinX;
			y = LayoutAreaBounds->MinY;

			// Find best locations for unpositioned icons
			for (in = *UnpositionedIconList; in; in = inNext)
				{
				inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

				d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

				GetIconBoundingBox(in, &IconBounds);

				FindFreeLocationInTilesInfoTopDown(iwt,
					tli, in,
					x,
					LayoutAreaBounds->MinY,
                                        LayoutAreaBounds, &x, &y);

				d1(KPrintF("%s/%s/%ld: in_Icon=%08lx  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, in->in_Icon, x, y));
				MoveIconToPositionXY(in, x, y);

				// move icon to list of positioned icons
				SCA_MoveIconNode(UnpositionedIconList,
					RedrawIconList, in);

				AddIconToTilesInfo(iwt, tli, in);
				}

			DisposeTilesInfo(tli);
			}
		}
	else
		{
		// We only have unpositioned icons,
		// so we don't need to bother whether space is free.
		SimpleLayoutIconsTopDown(iwt, RedrawIconList,
			UnpositionedIconList, LayoutAreaBounds);
		}
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


// Layout icons top-down in columns
// Since we have no placed icons, there's no need to check
// if any area is occupied
static void SimpleLayoutIconsTopDown(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds)
{
	LONG MaxWidth, ColumnWidth, IconWidth, IconHeight;
	LONG x, y;
	struct Rect32 IconBounds;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	x = LayoutAreaBounds->MinX;

	while (*UnpositionedIconList)
		{
		struct ScaIconNode *in, *inNext;
		LONG XCenter;

		y = LayoutAreaBounds->MinY;

		MaxWidth = 0;

		// Calculate width for column
		for (in = *UnpositionedIconList;
			in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			GetIconBoundingBox(in, &IconBounds);

			IconWidth = 1 + IconBounds.MaxX - IconBounds.MinX;
			IconHeight = 1 + IconBounds.MaxY - IconBounds.MinY;

			d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
			d1(KPrintF("%s/%s/%ld: width=%ld  height=%ld\n", __FILE__, __FUNC__, __LINE__, IconWidth, IconHeight));

			// Check if there is still room in the column
			if (y != CurrentPrefs.pref_CleanupSpace.Top && y > LayoutAreaBounds->MaxY - IconHeight)
				{
				break;
				}

			if (MaxWidth < IconWidth)
				{
				MaxWidth = IconWidth;
				}

			y += IconHeight + CurrentPrefs.pref_CleanupSpace.YSkip;
			}

		y = LayoutAreaBounds->MinY;

		XCenter = x + MaxWidth / 2;
		ColumnWidth = MaxWidth;

		// Position icons in column
		for (in = *UnpositionedIconList; in; in = inNext)
			{
			inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

			GetIconBoundingBox(in, &IconBounds);

			IconWidth = 1 + IconBounds.MaxX - IconBounds.MinX;
			IconHeight = 1 + IconBounds.MaxY - IconBounds.MinY;

			d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>  y=%ld\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), y));

			// Check and see if we need to move to a new column
			if (y != CurrentPrefs.pref_CleanupSpace.Top && y > LayoutAreaBounds->MaxY - IconHeight)
				{
				// proceed to next column
				x += ColumnWidth + CurrentPrefs.pref_CleanupSpace.XSkip;
				break;
				}

			MoveIconToPositionXY(in,
				XCenter - IconWidth / 2, y);

			// move icon to list of positioned icons
			SCA_MoveIconNode(UnpositionedIconList,
				RedrawIconList, in);

			y += IconHeight + CurrentPrefs.pref_CleanupSpace.YSkip;
			}
		}
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


// Layout icons left to right, one row at a time.
static void LayoutIconsLeftRight(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds, const struct Rect32 *MaxAreaBounds)
{
	d1(KPrintF("%s/%s/%ld: START  LayoutAreaBounds->MinY=%ld\n", __FILE__, __FUNC__, __LINE__, LayoutAreaBounds->MinY));

	// Check which icons have not been positioned
	if (iwt->iwt_WindowTask.wt_IconList)
		{
		LONG x, y;
		struct Rect32 IconBounds;
		struct TilesInfo *tli;

		tli = CreateTilesInfo(iwt, PLACEMENT_TopDown, MaxAreaBounds);
		d1(KPrintF("%s/%s/%ld: tli=%08lx\n", __FILE__, __FUNC__, __LINE__, tli));
		if (tli)
			{
			struct ScaIconNode *in, *inNext;

			// Add all placed icons to tiles array,
			// and mark their positions as occupied
			AddPositionedIconsToTiles(iwt, tli);

			// Start the icon placement at (LayoutAreaBounds->MinX, LayoutAreaBounds->MinY)
			x = LayoutAreaBounds->MinX;
			y = LayoutAreaBounds->MinY;

			// Find best locations for unpositioned icons
			for (in = *UnpositionedIconList; in; in = inNext)
				{
				inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

				d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

				GetIconBoundingBox(in, &IconBounds);

				FindFreeLocationInTilesInfoLeftRight(iwt,
					tli, in,
					LayoutAreaBounds->MinX,
					y,
                                        LayoutAreaBounds, &x, &y);

				d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));
				MoveIconToPositionXY(in, x, y);

				// move icon to list of positioned icons
				SCA_MoveIconNode(UnpositionedIconList,
					RedrawIconList, in);

				AddIconToTilesInfo(iwt, tli, in);
				}

			DisposeTilesInfo(tli);
			}
		}
	else
		{
		// We only have unpositioned icons,
		// so we don't need to bother whether space is free.
		SimpleLayoutIconsLeftRight(iwt, RedrawIconList,
			UnpositionedIconList, LayoutAreaBounds);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


// Layout icons left to right in rows
// Since we have no placed icons, there's no need to check
// if any area is occupied
static void SimpleLayoutIconsLeftRight(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList, struct ScaIconNode **UnpositionedIconList,
	const struct Rect32 *LayoutAreaBounds)
{
	LONG x, y;
	ULONG MaxHeight, RowHeight;

	d1(KPrintF("%s/%s/%ld: START  LayoutAreaBounds->MinY=%ld\n", __FILE__, __FUNC__, __LINE__, LayoutAreaBounds->MinY));

	y = LayoutAreaBounds->MinY;

	while (*UnpositionedIconList)
		{
		struct Rect32 IconBounds;
		struct ScaIconNode *in, *inNext;
		LONG IconWidth, IconHeight;
		LONG YCenter;

		x = LayoutAreaBounds->MinX;

		MaxHeight = 0;

		// Calculate height for row
		for (in = *UnpositionedIconList;
			in;
                        in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			GetIconBoundingBox(in, &IconBounds);

			IconWidth = 1 + IconBounds.MaxX - IconBounds.MinX;
			IconHeight = 1 + IconBounds.MaxY - IconBounds.MinY;

			d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

			// Check if there is still room in the row
			if (x != CurrentPrefs.pref_CleanupSpace.Left && x > LayoutAreaBounds->MaxX - IconWidth)
				{
				break;
				}

			if (MaxHeight < IconHeight)
				MaxHeight = IconHeight;

			x += IconWidth + CurrentPrefs.pref_CleanupSpace.XSkip;
			}

		x = LayoutAreaBounds->MinX;

		YCenter = y + MaxHeight / 2;
		RowHeight = MaxHeight;

		// Position icons in row
		for (in = *UnpositionedIconList; in; in = inNext)
			{
			inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

			GetIconBoundingBox(in, &IconBounds);

			IconWidth = 1 + IconBounds.MaxX - IconBounds.MinX;
			IconHeight = 1 + IconBounds.MaxY - IconBounds.MinY;

			d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>  y=%ld\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), y));

			// Check and see if we need to move to a new row
			if (x != CurrentPrefs.pref_CleanupSpace.Left && x > LayoutAreaBounds->MaxX - IconWidth)
				{
				// proceed to next row
				y += RowHeight + CurrentPrefs.pref_CleanupSpace.YSkip;
				break;
				}

			MoveIconToPositionXY(in,
				x, YCenter - IconHeight / 2);

			// move icon to list of positioned icons
			SCA_MoveIconNode(UnpositionedIconList,
				RedrawIconList, in);

			x += IconWidth + CurrentPrefs.pref_CleanupSpace.XSkip;
			}
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void MoveIconToPositionXY(struct ScaIconNode *in, LONG x, LONG y)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	WORD BoundsWidth;

	d1(KPrintF("%s/%s/%ld: START  in=%08lx <%s>  x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, in, GetIconName(in), x, y));

	BoundsWidth = gg->LeftEdge - gg->BoundsLeftEdge;

	gg->LeftEdge = gg->BoundsLeftEdge = x;
	gg->LeftEdge += BoundsWidth;
	gg->TopEdge = gg->BoundsTopEdge = y;

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void GetIconBoundingBox(const struct ScaIconNode *in, struct Rect32 *Bounds)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

	if (IsNoIconPosition(gg))
		{
		Bounds->MinX = Bounds->MaxX = 0;
		Bounds->MinY = Bounds->MaxY = 0;
		}
	else
		{
		Bounds->MinX = Bounds->MaxX = gg->BoundsLeftEdge;
		Bounds->MinY = Bounds->MaxY = gg->BoundsTopEdge;
		}

	Bounds->MaxX += gg->BoundsWidth - 1;
	Bounds->MaxY += gg->BoundsHeight - 1;
}


static void DrawIconList(struct internalScaWindowTask *iwt,
	struct ScaIconNode **RedrawIconList)
{
	struct ScaIconNode *in, *inNext;

	for (in = *RedrawIconList; in; in = inNext)
		{
		RedrawIcon(iwt, in);

		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		// Icon has been positioned and redrawn
		// now move it to list of positioned icons
		SCA_MoveIconNode(RedrawIconList,
			&iwt->iwt_WindowTask.wt_IconList, in);
		}
}


static void RedrawIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

	if (gg->LeftEdge != in->in_OldLeftEdge || gg->TopEdge != in->in_OldTopEdge)
		{
		d1(KPrintF("%s/%s/%ld: in_Icon=%08lx  LeftEdge=%ld\n", __FILE__, __FUNC__, __LINE__, in->in_Icon, gg->LeftEdge));

		if (!IsNoIconPosition(gg) && (in->in_Flags & INF_IconVisible))
			{
			struct ScaIconNode inDummy;
			struct ScaIconNode *RemoveList;
			WORD NewLeftEdge = gg->LeftEdge;
			WORD NewTopEdge = gg->TopEdge;
			WORD NewBoundsLeftEdge = gg->BoundsLeftEdge;
			WORD NewBoundsTopEdge = gg->BoundsTopEdge;

			RemoveList = &inDummy;
			inDummy = *in;
			inDummy.in_Node.mln_Succ = NULL;

			gg->LeftEdge = gg->BoundsLeftEdge = in->in_OldLeftEdge;
			gg->TopEdge = gg->BoundsTopEdge = in->in_OldTopEdge;
			gg->BoundsLeftEdge += NewBoundsLeftEdge - NewLeftEdge;
			gg->BoundsTopEdge += NewBoundsTopEdge - NewTopEdge;

			RemoveIcons(iwt, &RemoveList);

			in->in_Flags &= ~INF_IconVisible;

			gg->LeftEdge = NewLeftEdge;
			gg->TopEdge = NewTopEdge;
			gg->BoundsLeftEdge = NewBoundsLeftEdge;
			gg->BoundsTopEdge = NewBoundsTopEdge;
			}

		in->in_OldLeftEdge = gg->LeftEdge;
		in->in_OldTopEdge = gg->TopEdge;

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DrawIcon, in->in_Icon);
		}
}


static void CalculateMaximumIconAreaDimensions(const struct internalScaWindowTask *iwt, struct Rect32 *Bounds)
{
	const struct ScaIconNode *in;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	// To find need size of placement tiles array,
	// calculate theoretical absolute maximum width and height
	// possibly required for the icons.
	// Assume that all icons are layouted either in one huge row or column,
	// and add up all icon's width and height.

	Bounds->MinX = Bounds->MaxX = 2 * CurrentPrefs.pref_CleanupSpace.Left;
	Bounds->MinY = Bounds->MaxY = 2 * CurrentPrefs.pref_CleanupSpace.Top;

	for (in = iwt->iwt_WindowTask.wt_IconList;
		in; in = (const struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		const struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", \
			__FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		Bounds->MaxX += CurrentPrefs.pref_CleanupSpace.XSkip + gg->BoundsWidth + 2 * CurrentPrefs.pref_CleanupSpace.Left;
		Bounds->MaxY += CurrentPrefs.pref_CleanupSpace.YSkip + gg->BoundsHeight + 2 * CurrentPrefs.pref_CleanupSpace.Top;
		}
	for (in = iwt->iwt_WindowTask.wt_LateIconList;
		in; in = (const struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		const struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", \
			__FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		Bounds->MaxX += CurrentPrefs.pref_CleanupSpace.XSkip + gg->BoundsWidth + 2 * CurrentPrefs.pref_CleanupSpace.Left;
		Bounds->MaxY += CurrentPrefs.pref_CleanupSpace.YSkip + gg->BoundsHeight + 2 * CurrentPrefs.pref_CleanupSpace.Top;
		}

	d1(KPrintF("%s/%s/%ld: END  MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, Bounds->MinX, Bounds->MinY, Bounds->MaxX, Bounds->MaxY));

	if (iwt->iwt_WindowTask.wt_XOffset > 0)
		Bounds->MaxX += iwt->iwt_WindowTask.wt_XOffset;
	if (iwt->iwt_WindowTask.wt_YOffset > 0)
		Bounds->MaxY += iwt->iwt_WindowTask.wt_YOffset;

	d1(KPrintF("%s/%s/%ld: END  MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, Bounds->MinX, Bounds->MinY, Bounds->MaxX, Bounds->MaxY));
}


static void AddPositionedIconsToTiles(const struct internalScaWindowTask *iwt,
	struct TilesInfo *tli)
{
	const struct ScaIconNode *in;

	for (in = iwt->iwt_WindowTask.wt_IconList;
		in; in = (const struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		AddIconToTilesInfo(iwt, tli, in);
		}
}


