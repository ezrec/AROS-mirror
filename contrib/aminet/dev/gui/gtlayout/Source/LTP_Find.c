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

LONG
LTP_Find_Clicked_Item(LayoutHandle *handle,ObjectNode *radio,LONG x,LONG y)
{
	if(x >= radio->Left + INTERWIDTH)
	{
		LONG i,top = radio->Top,height,single;

		height = ((radio->Height + handle->InterHeight) / radio->Lines);
		single = height - handle->InterHeight;

		for(i = 0 ; i <= radio->Max ; i++)
		{
			if(y >= top && y < top + single)
				return(i);
			else
				top += height;
		}
	}

	return(-1);
}


/*****************************************************************************/


ObjectNode *
LTP_FindNode_Position(ObjectNode *group,LONG x,LONG y)
{
	ObjectNode	*node;
	ULONG		 page;

	SCANPAGE(group,node,page)
	{
		if(node->Type == GROUP_KIND)
		{
			ObjectNode *result;

			if(result = LTP_FindNode_Position(node,x,y))
				return(result);
			else
			{
				if(node->Top <= y && node->Top + node->Height > y && node->Left <= x && node->Left + node->Width > x)
				{
					while(node)
					{
						if(node->ID <= PHANTOM_GROUP_ID)
							node = node->Special.Group.ParentGroup;
						else
							return(node);
					}

					return(NULL);
				}
			}
		}
		else
		{
			if(node->Top <= y && node->Top + node->Height > y)
			{
				switch(node->Type)
				{
					case CHECKBOX_KIND:

						if(node->LabelPlace == PLACE_LEFT)
						{
							if(node->Left - (node->LabelWidth + INTERWIDTH) <= x && node->Left - INTERWIDTH > x)
								return(node);
						}
						else
						{
							if(node->Left + node->Width + INTERWIDTH <= x && node->Left + node->Width + INTERWIDTH + node->LabelWidth > x)
								return(node);
						}

						break;

					case MX_KIND:

						if(node->Left + node->Width + INTERWIDTH <= x && node->Left + node->Width + INTERWIDTH + node->Special.Radio.LabelWidth > x)
							return(node);

						break;

					default:

						if(node->Left <= x && node->Left + node->Width > x)
						{
							switch(node->Type)
							{
								case PICKER_KIND:

									return(node->Special.Picker.Parent->UserData);

								case INCREMENTER_KIND:

									return(node->Special.Incrementer.Parent->UserData);

								default:

									return(node);
							}
						}
				}
			}
		}
	}

	return(NULL);
}


/*****************************************************************************/

STATIC ObjectNode *
LTP_FindGroupNode(ObjectNode *group,LONG id)
{
	ObjectNode *node;
	ObjectNode *result;

	SCANGROUP(group,node)
	{
		switch(node->Type)
		{
			case INCREMENTER_KIND:
			case PICKER_KIND:

				break;

			case GROUP_KIND:

				if(node->ID == id)
					return(node);
				else
				{
					if(result = LTP_FindGroupNode(node,id))
						return(result);
				}

				break;

			default:

				if(node->ID == id)
					return(node);
		}
	}

	return(NULL);
}

ObjectNode *
LTP_FindNode(LayoutHandle *handle,LONG id)
{
	return(LTP_FindGroupNode(handle->TopGroup,id));
}

/*****************************************************************************/

	// This is the new and improved gadget search routine. It now does a
	// binary search rather than the linear search it used to do. Works
	// much better with large number of gadgets, but requires the gadget
	// pointers to be sorted in the order of the GadgetID entries.

struct Gadget *
LTP_FindGadget(LayoutHandle *handle,LONG id)
{
	if(handle->GadgetArray)
	{
		struct Gadget **Tab;
		LONG Left,Mid,Right;

		Tab		= handle->GadgetArray;
		Left	= 0;
		Right	= handle->Index - 1;

		do
		{
			Mid = (Left + Right) / 2;

			if(id < Tab[Mid]->GadgetID)
				Right	= Mid - 1;
			else
				Left	= Mid + 1;
		}
		while(id != Tab[Mid]->GadgetID && Left <= Right);

		if(id == Tab[Mid]->GadgetID)
		{
			ObjectNode *node;
			struct Gadget *g;

			g = Tab[Mid];

			if(node = (ObjectNode *)g->UserData)
			{
				if(node->PointBack == node && node->Host == g)
					return(g);
			}
		}
	}

	return(NULL);
}

struct Gadget *
LTP_FindGadgetLinear(LayoutHandle *handle,LONG id)
{
	if(handle->GadgetArray)
	{
		LONG i;

		for(i = 0 ; i < handle->Index ; i++)
		{
			if(handle->GadgetArray[i]->GadgetID == id)
			{
				ObjectNode *node;
				struct Gadget *g;

				g = handle->GadgetArray[i];

				if(node = (ObjectNode *)g->UserData)
				{
					if(node->PointBack == node && node->Host == g)
						return(g);
				}
			}
		}
	}

	return(NULL);
}
