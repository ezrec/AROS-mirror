/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"

VOID
LTP_PlaceGroups(LayoutHandle *handle,ObjectNode *group,LONG left,LONG top)
{
	if(!handle->Failed)
	{
		ObjectNode	*node;
		LONG		 plusLeft,
					 plusTop,
					 lastSpace;

		if(group->Label || group->Special.Group.Frame || group->Special.Group.FrameType == FRAMETYPE_Label)
		{
			plusLeft = 4 + handle->GlyphWidth + handle->InterWidth + group->Special.Group.ExtraLeft;

			if(group->Label)
				plusTop = handle->GlyphHeight + handle->InterHeight + group->Special.Group.ExtraTop;
			else
				plusTop = 2 + handle->InterHeight + group->Special.Group.ExtraTop;
		}
		else
		{
			plusLeft	= group->Special.Group.ExtraLeft;
			plusTop 	= group->Special.Group.ExtraTop;
		}

		if(group->ExtraSpace)
		{
			if(group->Special.Group.ParentGroup->Special.Group.Horizontal)
				left += handle->InterWidth;
			else
				top += handle->InterHeight;
		}

		group->Left	= left;
		group->Top	= top;

		left	+= plusLeft;
		top		+= plusTop;

		if(group->Special.Group.Horizontal)
		{
			lastSpace = left;

			SCANGROUP(group,node)
			{
				if(LIKE_STRING_KIND(node) && node->Special.String.LinkID != -1)
					handle->Count++;
				else
				{
					if(node->Type == GROUP_KIND)
						LTP_PlaceGroups(handle,node,lastSpace,top);
					else
					{
						node->Left	+= left;
						node->Top	+= top;

						handle->Count++;
					}

					if(!group->Special.Group.Paging)
					{
						lastSpace = node->Left + node->Width + handle->InterWidth;

						if(node->Type == MX_KIND)
						{
							if((node->LabelPlace == PLACETEXT_RIGHT) || (node->LabelPlace == PLACETEXT_LEFT))
								lastSpace += INTERWIDTH + node->Special.Radio.LabelWidth;
						}
					}
				}
			}
		}
		else
		{
			lastSpace = top;

			SCANGROUP(group,node)
			{
				if(LIKE_STRING_KIND(node) && node->Special.String.LinkID != -1)
					handle->Count++;
				else
				{
					if(node->Type == GROUP_KIND)
						LTP_PlaceGroups(handle,node,left,lastSpace);
					else
					{
						node->Left	+= left;
						node->Top	+= top;

						handle->Count++;
					}

					if(!group->Special.Group.Paging)
						lastSpace = node->Top + node->Height + handle->InterHeight;
				}
			}
		}
	}
}
