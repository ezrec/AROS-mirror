/*
 * @(#) $Header$
 *
 * BGUI library
 *
 * (C) Copyright 2000 BGUI Developers Team.
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.11  2000/05/09 20:34:57  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:58  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:58  mlemos
 * Initial revision.
 *
 *
 */

#ifndef OL_LAYOUT_H
#define OL_LAYOUT_H

#include <stdlib.h>
#include <float.h>

#include <exec/types.h>

#include "include/MemoryPool.h"

typedef LONG olPosition;
typedef ULONG olDimension;
typedef double olWeight;
typedef ULONG olCardinal;
typedef BOOL olBoolean;

#define olTrue TRUE
#define olFalse FALSE

#define olWeightTooSmall(weight) (((olWeight)(weight))<=FLT_MIN)

#define olCUSTOM_MEMORY_ALLOCATION
#define olMemoryAllocationContext oBGUI_POOL

#define olMalloc(allocation_context,size) oBGUIPoolAllocate(allocation_context,size)
#define olRealloc(allocation_context,memory,size) oBGUIPoolReallocate(allocation_context,memory,size)
#define olFree(allocation_context,memory) oBGUIPoolFree(allocation_context,memory)

typedef enum
{
	olLayoutHorizontal,
	olLayoutVertical,
	olLayoutPaged,
	olLayoutTable
}
olLayoutType;

typedef struct
{
	olCardinal Start;
	olCardinal Span;
	olWeight Weight;
	olPosition Position;
	olDimension MinimumSize;
	olDimension Size;
}
olPartition;

#define DEFAULT_PARTITIONS 8

typedef struct olSpanPartitions
{
	olPartition *Partitions;
	olPartition DefaultPartitions[DEFAULT_PARTITIONS];
	olCardinal UsedPartitions;
	olCardinal AllocatedPartitions;
}
olSpanPartitions;

typedef struct olLayoutElementDefinition
{
	/* Parameters */
	olWeight HorizontalWeight;
	olWeight VerticalWeight;
	olWeight WidthWeight;
	olWeight HeightWeight;
	olWeight LeftWeight;
	olWeight TopWeight;
	olWeight RightWeight;
	olWeight BottomWeight;
	olDimension LeftOffset;
	olDimension TopOffset;
	olDimension RightOffset;
	olDimension BottomOffset;
	/* Parameters or work variables */
	olCardinal Row;
	olCardinal Column;
	olCardinal RowSpan;
	olCardinal ColumnSpan;
	/* Inputs */
	olDimension MinimumWidth;
	olDimension MinimumHeight;
	olDimension NominalWidth;
	olDimension NominalHeight;
	olDimension MaximumWidth;
	olDimension MaximumHeight;
	/* Outputs and Inputs */
	olPosition SpaceX;
	olPosition SpaceY;
	olDimension SpaceWidth;
	olDimension SpaceHeight;
	/* Outputs */
	olPosition X;
	olPosition Y;
	olDimension Width;
	olDimension Height;
	/* Work variables */
	struct olLayoutElementDefinition *NextSortedRowSpan;
	struct olLayoutElementDefinition *NextSortedColumnSpan;
	olCardinal SpannedRowPartitions;
	olCardinal SpannedColumnPartitions;
	olPartition *StartRowPartition;
	olPartition *StartColumnPartition;
}
olLayoutElementDefinition;

typedef struct
{
	/* Parameters */
#ifdef olCUSTOM_MEMORY_ALLOCATION
	olMemoryAllocationContext AllocationContext;
#endif
	olLayoutElementDefinition **Elements;
	olCardinal ElementCount;
	olLayoutType LayoutType;
	olDimension LeftOffset;
	olDimension TopOffset;
	olDimension RightOffset;
	olDimension BottomOffset;
	/* Outputs and Inputs */
	olDimension MinimumWidth;
	olDimension MinimumHeight;
	olDimension NominalWidth;
	olDimension NominalHeight;
	olDimension MaximumWidth;
	olDimension MaximumHeight;
	/* Inputs */
	olDimension Width;
	olDimension Height;
	/* Work variables */
	olSpanPartitions RowPartitions;
	olSpanPartitions ColumnPartitions;
	struct olLayoutElementDefinition *FirstSortedRowSpan;
	struct olLayoutElementDefinition *FirstSortedColumnSpan;
}
olLayoutDefinition;

void olInitializeLayout(olLayoutDefinition *layout_definition);
void olDisposeLayout(olLayoutDefinition *layout_definition);
olBoolean olComputeMinimumSize(olLayoutDefinition *layout_definition);
void olLayout(olLayoutDefinition *layout_definition);
void olLayoutElement(olLayoutElementDefinition *layout_element);

#endif
