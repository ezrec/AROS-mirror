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
 * Revision 41.11  2000/05/09 20:34:38  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:41  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:25  mlemos
 * Initial revision.
 *
 *
 */

#include <string.h>

#include "include/olLayout.h"

static olBoolean InsertPartition(
olLayoutDefinition *layout_definition,
olSpanPartitions *span_partitions,
olCardinal start,
olCardinal span,
olWeight weight,
olCardinal partition)
{
	olPartition *partitions=NULL;

	if(span_partitions->UsedPartitions>=span_partitions->AllocatedPartitions)
	{
		olCardinal step;

		for(step=span_partitions->AllocatedPartitions;step;step/=2)
		{
			if(span_partitions->Partitions==span_partitions->DefaultPartitions)
			{
				if((partitions=olMalloc(layout_definition->AllocationContext,(span_partitions->AllocatedPartitions+step)*sizeof(*partitions)))==NULL)
					continue;
				memcpy(partitions,span_partitions->DefaultPartitions,(size_t)span_partitions->UsedPartitions*sizeof(*partitions));
			}
			else
				if((partitions=olRealloc(layout_definition->AllocationContext,span_partitions->Partitions,(span_partitions->AllocatedPartitions+step)*sizeof(*partitions)))==NULL)
					continue;
			break;
		}
		if(step==0)
			return(olFalse);
		span_partitions->Partitions=partitions;
		span_partitions->AllocatedPartitions+=step;
	}
	{
		olCardinal move_partition;

		for(move_partition=span_partitions->UsedPartitions,
			partitions=span_partitions->Partitions+move_partition;
				move_partition>partition;
					move_partition--,partitions--)
			partitions[0]=partitions[-1];
		partitions->Start=start;
		partitions->Span=span;
		partitions->Weight=weight;
		partitions->Position=0;
		partitions->MinimumSize=0;
		span_partitions->UsedPartitions++;
	}
	return(olTrue);
}

static olBoolean InsertSpan(
olLayoutDefinition *layout_definition,
olSpanPartitions *span_partitions,
olCardinal start,
olCardinal span,
olWeight weight)
{
	olCardinal partition;

	for(partition=0;span && partition<span_partitions->UsedPartitions;partition++)
	{
		if(span_partitions->Partitions[partition].Start+span_partitions->Partitions[partition].Span>start)
		{
			olCardinal sub_span;
			olWeight sub_weight;

			if(span_partitions->Partitions[partition].Start>start)
			{
				if((sub_span=span_partitions->Partitions[partition].Start-start)>=span)
				{
					sub_weight=weight;
					weight=0;
					sub_span=span;
					span=0;
				}
				else
				{
					sub_weight=weight*sub_span/span;
					weight-=sub_weight;
					span-=sub_span;
				}
				if(InsertPartition(layout_definition,span_partitions,start,sub_span,sub_weight,partition)==olFalse)
					return(olFalse);
				if(span==0)
					return(olTrue);
				partition++;
				start=span_partitions->Partitions[partition].Start;
			}
			else
			{
				if(start>span_partitions->Partitions[partition].Start)
				{
					sub_span=start-span_partitions->Partitions[partition].Start;
					sub_weight=span_partitions->Partitions[partition].Weight*sub_span/span_partitions->Partitions[partition].Span;
					if(InsertPartition(layout_definition,span_partitions,span_partitions->Partitions[partition].Start,sub_span,sub_weight,partition)==olFalse)
						return(olFalse);
					partition++;
					span_partitions->Partitions[partition].Start=start;
					span_partitions->Partitions[partition].Span-=sub_span;
					span_partitions->Partitions[partition].Weight-=sub_weight;
				}
			}
			if(span_partitions->Partitions[partition].Span==span)
			{
				if(span_partitions->Partitions[partition].Weight<weight)
					span_partitions->Partitions[partition].Weight=weight;
				return(olTrue);
			}
			if(span_partitions->Partitions[partition].Span>span)
			{
				sub_weight=span_partitions->Partitions[partition].Weight*span/span_partitions->Partitions[partition].Span;
				span_partitions->Partitions[partition].Start+=span;
				span_partitions->Partitions[partition].Span-=span;
				span_partitions->Partitions[partition].Weight-=sub_weight;
				if(sub_weight>weight)
					weight=sub_weight;
				return(InsertPartition(layout_definition,span_partitions,start,span,weight,partition));
			}
			sub_span=span_partitions->Partitions[partition].Span;
			sub_weight=weight*sub_span/span;
			if(span_partitions->Partitions[partition].Weight<sub_weight)
				span_partitions->Partitions[partition].Weight=sub_weight;
			weight-=sub_weight;
			span-=sub_span;
			start+=sub_span;
		}
	}
	return((olBoolean)(span ? InsertPartition(layout_definition,span_partitions,start,span,weight,partition) : olTrue));
}

static void olGrowSpan(
olPartition *start_partition,
olPartition *end_partition,
olDimension grow)
{
	olPartition *partition;
	olWeight weight;

	if(grow<=0)
		return;
	for(weight=0,partition=start_partition;partition<end_partition;partition++)
		if(partition->MinimumSize==0)
			weight+=partition->Weight;
	if(!olWeightTooSmall(weight))
	{
		for(partition=start_partition;grow && partition<end_partition;partition++)
			if(partition->MinimumSize==0
			&& !olWeightTooSmall(partition->Weight))
			{
				olDimension sub_grow;

				if(partition->Weight<weight)
				{
					sub_grow=grow*partition->Weight/weight;
					grow-=sub_grow;
					weight-=partition->Weight;
				}
				else
				{
					sub_grow=grow;
					grow=0;
					weight=0;
				}
				partition->MinimumSize+=sub_grow;
			}
	}
	if(grow<=0)
		return;
	for(weight=0,partition=start_partition;partition<end_partition;partition++)
		weight+=partition->Weight;
	if(!olWeightTooSmall(weight))
	{
		for(partition=start_partition;grow && partition<end_partition;partition++)
			if(!olWeightTooSmall(partition->Weight))
			{
				olDimension sub_grow;

				if(partition->Weight!=weight)
				{
					sub_grow=grow*partition->Weight/weight;
					grow-=sub_grow;
					weight-=partition->Weight;
				}
				else
				{
					sub_grow=grow;
					grow=0;
					weight=0;
				}
				partition->MinimumSize+=sub_grow;
			}
	}
	if(grow>0)
	{
		olCardinal partitions;

		partitions=end_partition-start_partition;
		for(partition=start_partition;grow && partition<end_partition;partition++)
		{
			olDimension sub_grow;

			if(partitions>1)
			{
				sub_grow=grow/partitions;
				grow-=sub_grow;
			}
			else
			{
				sub_grow=grow;
				grow=0;
			}
			partition->MinimumSize+=sub_grow;
			partitions--;
		}
	}
}

void olInitializeLayout(layout_definition)
olLayoutDefinition *layout_definition;
{
	layout_definition->RowPartitions.Partitions=layout_definition->RowPartitions.DefaultPartitions;
	layout_definition->ColumnPartitions.Partitions=layout_definition->ColumnPartitions.DefaultPartitions;
	layout_definition->RowPartitions.AllocatedPartitions=
	layout_definition->ColumnPartitions.AllocatedPartitions=DEFAULT_PARTITIONS;
}

void olDisposeLayout(layout_definition)
olLayoutDefinition *layout_definition;
{
	if(layout_definition->RowPartitions.Partitions!=layout_definition->RowPartitions.DefaultPartitions)
	{
		olFree(layout_definition->AllocationContext,layout_definition->RowPartitions.Partitions);
		layout_definition->RowPartitions.Partitions=layout_definition->RowPartitions.DefaultPartitions;
		layout_definition->RowPartitions.AllocatedPartitions=DEFAULT_PARTITIONS;
	}
	if(layout_definition->ColumnPartitions.Partitions!=layout_definition->ColumnPartitions.DefaultPartitions)
	{
		olFree(layout_definition->AllocationContext,layout_definition->ColumnPartitions.Partitions);
		layout_definition->ColumnPartitions.Partitions=layout_definition->ColumnPartitions.DefaultPartitions;
		layout_definition->ColumnPartitions.AllocatedPartitions=DEFAULT_PARTITIONS;
	}
}

olBoolean olComputeMinimumSize(layout_definition)
olLayoutDefinition *layout_definition;
{
	olCardinal element;
	olLayoutElementDefinition *layout_element;

#ifndef TEST_TABLE_LAYOUT
	if(layout_definition->LayoutType!=olLayoutTable)
	{
		olDimension size;

		layout_definition->MinimumWidth=layout_definition->MinimumHeight=0;
		for(element=0;element<layout_definition->ElementCount;element++)
		{
			layout_element=layout_definition->Elements[element];
			switch(layout_definition->LayoutType)
			{
				case olLayoutHorizontal:
					layout_definition->MinimumWidth+=layout_element->MinimumWidth+layout_element->LeftOffset+layout_element->RightOffset;
					size=layout_element->MinimumHeight+layout_element->TopOffset+layout_element->BottomOffset;
					if(layout_definition->MinimumHeight<size)
						layout_definition->MinimumHeight=size;
					break;
				case olLayoutVertical:
					size=layout_element->MinimumWidth+layout_element->LeftOffset+layout_element->RightOffset;
					if(layout_definition->MinimumWidth<size)
						layout_definition->MinimumWidth=size;
					layout_definition->MinimumHeight+=layout_element->MinimumHeight+layout_element->TopOffset+layout_element->BottomOffset;
					break;
				case olLayoutPaged:
					size=layout_element->MinimumWidth+layout_element->LeftOffset+layout_element->RightOffset;
					if(layout_definition->MinimumWidth<size)
						layout_definition->MinimumWidth=size;
					size=layout_element->MinimumHeight+layout_element->TopOffset+layout_element->BottomOffset;
					if(layout_definition->MinimumHeight<size)
						layout_definition->MinimumHeight=size;
					break;
				case olLayoutTable:
					break;
			}
		}
		layout_definition->MinimumWidth+=layout_definition->LeftOffset+layout_definition->RightOffset;
		layout_definition->MinimumHeight+=layout_definition->TopOffset+layout_definition->BottomOffset;
	}
	else
#endif
	{
		olPartition *partition;

#ifdef TEST_TABLE_LAYOUT
		for(element=0;element<layout_definition->ElementCount;element++)
		{
			olLayoutElementDefinition *layout_element;

			layout_element=layout_definition->Elements[element];
			switch(layout_definition->LayoutType)
			{
				case olLayoutHorizontal:
					layout_element->Row=0;
					layout_element->Column=element;
					break;
				case olLayoutVertical:
					layout_element->Row=element;
					layout_element->Column=0;
					break;
				case olLayoutPaged:
					layout_element->Row=
					layout_element->Column=0;
					break;
				case olLayoutTable:
					continue;
			}
			layout_element->RowSpan=1;
			layout_element->ColumnSpan=1;
		}
#endif
		layout_definition->RowPartitions.UsedPartitions=
		layout_definition->ColumnPartitions.UsedPartitions=0;
		for(element=0;element<layout_definition->ElementCount;element++)
		{
			layout_element=layout_definition->Elements[element];
			if(InsertSpan(layout_definition,&layout_definition->RowPartitions,layout_element->Column,layout_element->ColumnSpan,layout_element->HorizontalWeight)==olFalse
			|| InsertSpan(layout_definition,&layout_definition->ColumnPartitions,layout_element->Row,layout_element->RowSpan,layout_element->VerticalWeight)==olFalse)
				return(olFalse);
		}
		layout_definition->FirstSortedRowSpan=
		layout_definition->FirstSortedColumnSpan=NULL;
		for(element=0;element<layout_definition->ElementCount;element++)
		{
			olLayoutElementDefinition **link;

			layout_element=layout_definition->Elements[element];
			for(partition=layout_definition->RowPartitions.Partitions;partition->Start<layout_element->Column;partition++);
			layout_element->StartRowPartition=partition;
			for(layout_element->SpannedRowPartitions=0;partition-layout_definition->RowPartitions.Partitions<layout_definition->RowPartitions.UsedPartitions && partition->Start-layout_element->Column<layout_element->ColumnSpan;partition++,layout_element->SpannedRowPartitions++);
			for(link= &layout_definition->FirstSortedRowSpan;*link;link= &(*link)->NextSortedRowSpan)
				if((*link)->SpannedRowPartitions>layout_element->SpannedRowPartitions)
					break;
			layout_element->NextSortedRowSpan= *link;
			*link=layout_element;
			for(partition=layout_definition->ColumnPartitions.Partitions;partition->Start<layout_element->Row;partition++);
			layout_element->StartColumnPartition=partition;
			for(layout_element->SpannedColumnPartitions=0;partition-layout_definition->ColumnPartitions.Partitions<layout_definition->ColumnPartitions.UsedPartitions && partition->Start-layout_element->Row<layout_element->RowSpan;partition++,layout_element->SpannedColumnPartitions++);
			for(link= &layout_definition->FirstSortedColumnSpan;*link;link= &(*link)->NextSortedColumnSpan)
				if((*link)->SpannedColumnPartitions>layout_element->SpannedColumnPartitions)
					break;
			layout_element->NextSortedColumnSpan= *link;
			*link=layout_element;
		}
		for(layout_element=layout_definition->FirstSortedRowSpan;layout_element;layout_element=layout_element->NextSortedRowSpan)
		{
			olDimension size,minimum_size;

			for(partition=layout_element->StartRowPartition,size=0;partition-layout_element->StartRowPartition<layout_element->SpannedRowPartitions;partition++)
				size+=partition->MinimumSize;
			minimum_size=layout_element->MinimumWidth+layout_element->LeftOffset+layout_element->RightOffset;
			if(minimum_size>size)
				olGrowSpan(layout_element->StartRowPartition,partition,minimum_size-size);
		}
		for(layout_element=layout_definition->FirstSortedColumnSpan;layout_element;layout_element=layout_element->NextSortedColumnSpan)
		{
			olDimension size,minimum_size;

			for(partition=layout_element->StartColumnPartition,size=0;partition-layout_element->StartColumnPartition<layout_element->SpannedColumnPartitions;partition++)
				size+=partition->MinimumSize;
			minimum_size=layout_element->MinimumHeight+layout_element->TopOffset+layout_element->BottomOffset;
			if(minimum_size>size)
				olGrowSpan(layout_element->StartColumnPartition,partition,minimum_size-size);
		}
		layout_definition->MinimumWidth=layout_definition->LeftOffset+layout_definition->RightOffset;
		for(partition=layout_definition->RowPartitions.Partitions;partition-layout_definition->RowPartitions.Partitions<layout_definition->RowPartitions.UsedPartitions;partition++)
			layout_definition->MinimumWidth+=partition->MinimumSize;
		layout_definition->MinimumHeight=layout_definition->TopOffset+layout_definition->BottomOffset;
		for(partition=layout_definition->ColumnPartitions.Partitions;partition-layout_definition->ColumnPartitions.Partitions<layout_definition->ColumnPartitions.UsedPartitions;partition++)
			layout_definition->MinimumHeight+=partition->MinimumSize;
	}
	return(olTrue);
}

static void olLayoutPartitions(
olSpanPartitions *span_partitions,
olPosition position,
olDimension excess)
{
	olPartition *partition;
	olWeight total_weight;

	for(total_weight=0,partition=span_partitions->Partitions;partition-span_partitions->Partitions<span_partitions->UsedPartitions;partition++)
		total_weight+=partition->Weight;
	for(partition=span_partitions->Partitions;partition-span_partitions->Partitions<span_partitions->UsedPartitions;partition++)
	{
		partition->Size=partition->MinimumSize;
		if(excess
		&& !olWeightTooSmall(total_weight)
		&& !olWeightTooSmall(partition->Weight))
		{
			olDimension sub_excess;

			if(partition->Weight!=total_weight)
			{
				sub_excess=excess*partition->Weight/total_weight;
				excess-=sub_excess;
				total_weight-=partition->Weight;
			}
			else
			{
				sub_excess=excess;
				excess=0;
				total_weight=0;
			}
			partition->Size+=sub_excess;
		}
		partition->Position=position;
		position+=partition->Size;
	}
}

void olLayout(layout_definition)
olLayoutDefinition *layout_definition;
{
	olCardinal element;

#ifndef TEST_TABLE_LAYOUT
	if(layout_definition->LayoutType!=olLayoutTable)
	{
		olWeight total_weight=0;

		switch(layout_definition->LayoutType)
		{
			case olLayoutHorizontal:
				for(element=0;element<layout_definition->ElementCount;element++)
					total_weight+=layout_definition->Elements[element]->HorizontalWeight;
				break;
			case olLayoutVertical:
				for(element=0;element<layout_definition->ElementCount;element++)
					total_weight+=layout_definition->Elements[element]->VerticalWeight;
				break;
			case olLayoutPaged:
			case olLayoutTable:
				break;
		}
		for(element=0;element<layout_definition->ElementCount;element++)
		{
			olLayoutElementDefinition *layout_element;

			layout_element=layout_definition->Elements[element];
			switch(layout_definition->LayoutType)
			{
				case olLayoutHorizontal:
					if(element)
					{
						olLayoutElementDefinition *previous_layout_element;

						previous_layout_element=layout_definition->Elements[element-1];
						layout_element->SpaceX=previous_layout_element->SpaceX+previous_layout_element->SpaceWidth;
					}
					else
						layout_element->SpaceX=layout_definition->LeftOffset;
					layout_element->SpaceWidth=layout_element->MinimumWidth+layout_element->LeftOffset+layout_element->RightOffset;
					{
						olDimension excess;

						if((excess=layout_definition->Width-layout_definition->MinimumWidth)
						&& !olWeightTooSmall(total_weight))
						{
							if(layout_element->HorizontalWeight<total_weight)
								layout_element->SpaceWidth+=excess*layout_element->HorizontalWeight/total_weight;
							else
								layout_element->SpaceWidth+=excess;
						}
					}
					layout_element->SpaceHeight=layout_definition->Height-(layout_element->SpaceY=layout_definition->TopOffset)-layout_definition->BottomOffset;
					break;
				case olLayoutVertical:
					layout_element->SpaceWidth=layout_definition->Width-(layout_element->SpaceX=layout_definition->LeftOffset)-layout_definition->RightOffset;
					if(element)
					{
						olLayoutElementDefinition *previous_layout_element;

						previous_layout_element=layout_definition->Elements[element-1];
						layout_element->SpaceY=previous_layout_element->SpaceY+previous_layout_element->SpaceHeight;
					}
					else
						layout_element->SpaceY=layout_definition->TopOffset;
					layout_element->SpaceHeight=layout_element->MinimumHeight+layout_element->TopOffset+layout_element->BottomOffset;
					{
						olDimension excess;

						if((excess=layout_definition->Height-layout_definition->MinimumHeight)
						&& !olWeightTooSmall(total_weight))
						{
							if(layout_element->VerticalWeight<total_weight)
								layout_element->SpaceHeight+=excess*layout_element->VerticalWeight/total_weight;
							else
								layout_element->SpaceHeight+=excess;
						}
					}
					break;
				case olLayoutPaged:
					layout_element->SpaceWidth=layout_definition->Width-(layout_element->SpaceX=layout_definition->LeftOffset)-layout_definition->RightOffset;
					layout_element->SpaceHeight=layout_definition->Height-(layout_element->SpaceY=layout_definition->TopOffset)-layout_definition->BottomOffset;
					break;
				case olLayoutTable:
					break;
			}
		}
	}
	else
#endif
	{
		olLayoutPartitions(&layout_definition->RowPartitions,layout_definition->LeftOffset,layout_definition->Width-layout_definition->MinimumWidth);
		olLayoutPartitions(&layout_definition->ColumnPartitions,layout_definition->TopOffset,layout_definition->Height-layout_definition->MinimumHeight);
		for(element=0;element<layout_definition->ElementCount;element++)
		{
			olLayoutElementDefinition *layout_element;
			olPartition *partition;

			layout_element=layout_definition->Elements[element];
			layout_element->SpaceX=layout_element->StartRowPartition->Position;
			for(layout_element->SpaceWidth=0,partition=layout_element->StartRowPartition;partition-layout_element->StartRowPartition<layout_element->SpannedRowPartitions;partition++)
				layout_element->SpaceWidth+=partition->Size;
			layout_element->SpaceY=layout_element->StartColumnPartition->Position;
			for(layout_element->SpaceHeight=0,partition=layout_element->StartColumnPartition;partition-layout_element->StartColumnPartition<layout_element->SpannedColumnPartitions;partition++)
				layout_element->SpaceHeight+=partition->Size;
		}
	}
}

void olLayoutElement(layout_element)
olLayoutElementDefinition *layout_element;
{
	olDimension excess;

	layout_element->X=layout_element->SpaceX+layout_element->LeftOffset;
	layout_element->Width=layout_element->MinimumWidth;
	if((excess=layout_element->SpaceWidth-layout_element->MinimumWidth-layout_element->LeftOffset-layout_element->RightOffset))
	{
		olWeight total_weight;

		if(!olWeightTooSmall(total_weight=layout_element->LeftWeight+layout_element->WidthWeight+layout_element->RightWeight))
		{
			olDimension sub_excess;

			if(layout_element->WidthWeight<total_weight)
				sub_excess=excess*layout_element->WidthWeight/total_weight;
			else
				sub_excess=excess;
			layout_element->Width+=sub_excess;
			if(excess-=sub_excess)
			{
				if(layout_element->LeftWeight<total_weight)
					excess=excess*layout_element->LeftWeight/total_weight;
				layout_element->X+=excess;
			}
		}
	}
	layout_element->Y=layout_element->SpaceY+layout_element->TopOffset;
	layout_element->Height=layout_element->MinimumHeight;
	if((excess=layout_element->SpaceHeight-layout_element->MinimumHeight-layout_element->TopOffset-layout_element->BottomOffset))
	{
		olWeight total_weight;

		if(!olWeightTooSmall(total_weight=layout_element->TopWeight+layout_element->HeightWeight+layout_element->BottomWeight))
		{
			olDimension sub_excess;

			if(layout_element->HeightWeight<total_weight)
				sub_excess=excess*layout_element->HeightWeight/total_weight;
			else
				sub_excess=excess;
			layout_element->Height+=sub_excess;
			if(excess-=sub_excess)
			{
				if(layout_element->TopWeight<total_weight)
					excess=excess*layout_element->TopWeight/total_weight;
				layout_element->Y+=excess;
			}
		}
	}
}
