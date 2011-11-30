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

/*****************************************************************************/

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/


VOID
LTP_Erase(LayoutHandle *Handle)
{
	struct Window *Window = Handle->Window;

	EraseRect(&Handle->RPort,Window->BorderLeft,Window->BorderTop,Window->Width - (Window->BorderRight + 1),Window->Height - (Window->BorderBottom + 1));
}


/*****************************************************************************/


/****** gtlayout.library/LT_RebuildTagList ******************************************
*
*   NAME
*	LT_RebuildTagList -- Rebuild the user interface after modifying it.
*
*   SYNOPSIS
*	Success = LT_RebuildTagList(Handle,Clear,TagList);
*	   D0                         A0    D0     A1
*
*	BOOL LT_RebuildTagList(LayoutHandle *,BOOL,struct TagItem *);
*
*	Success = LT_RebuildTags(Handle,Clear,...);
*
*	BOOL LT_RebuildTags(LayoutHandle *,BOOL,...);
*
*   FUNCTION
*	Certain aspects of the user interface can be changed at run time,
*	such as button labels. This routine will let you rebuild the interface
*	based upon the data supplied at creation time and your subsequent
*	changes. Before you make any vital changes, it is recommended to
*	lock the window using LT_LockWindow() in order to avoid clashes
*	with the Intuition and GadTools subsystems.
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure.
*
*	Clear - Pass in TRUE if you wish to have the window contents
*	    cleared before they are rebuild. This will introduce
*	    some visual hashing.
*
*	TagList - Attributes controlling the layout process.
*
*
*	Valid tags include:
*
*	LAWN_Bounds (struct IBox *) - Boundaries in which the window
*	    should be centered.
*
*	LAWN_ExtraWidth (LONG) - Extra space to add to the window
*	    width.
*
*	LAWN_ExtraHeight (LONG) - Extra height to add to the window
*	    height.
*
*   RESULT
*	Success - TRUE indicates that the interface was rebuilt,
*	    FALSE indicates trouble; it is recommended to
*	    call LT_DeleteHandle() on your LayoutHandle as
*	    soon as possible as the previous operation may
*	    have left the user interface in an inoperable
*	    state.
*
******************************************************************************
*
*/

BOOL LIBENT
LT_RebuildTagList(REG(a0) LayoutHandle *handle,REG(d0) BOOL clear,REG(a1) struct TagItem *TagParams)
{
	struct TagItem	*item;
	const struct TagItem	*list;
	struct IBox		*bounds = NULL;
	LONG			 RightEdge,
					 BottomEdge;

	list = TagParams;

	while(item = NextTagItem(&list))
	{
		switch(item->ti_Tag)
		{
			case LAWN_Bounds:

				bounds = (struct IBox *)item->ti_Data;
				break;
		}
	}

	if(handle)
	{
		struct Gadget	*gadget,
						*next;
		ObjectNode		*node;
		LONG			 left,top;
		struct IBox		 newBounds;
		struct Image	**ImagePtr;

		if(!handle->SizeVerified)
		{
			LTP_StripGadgets(handle,handle->List);

			#ifdef DO_BOOPSI_KIND
			{
				LTP_StripGadgets(handle,(struct Gadget *)handle->BOOPSIList);
			}
			#endif	/* DO_BOOPSI_KIND */
		}
		gadget = handle->List;

		while(gadget)
		{
			if(GETOBJECT(gadget,node))
			{
				LTP_PutStorage(node);

				ImagePtr = NULL;

				switch(node->Type)
				{
					#ifdef DO_GAUGE_KIND
					{
						case GAUGE_KIND:

							node->Special.Gauge.LastPercentage = -1;
							break;
					}
					#endif	/* DO_GAUGE_KIND */

					case BUTTON_KIND:

						ImagePtr = &node->Special.Button.ButtonImage;
						break;

					case PICKER_KIND:

						ImagePtr = &node->Special.Picker.Image;
						break;

					case TAPEDECK_KIND:

						ImagePtr = &node->Special.TapeDeck.ButtonImage;
						break;

					case INCREMENTER_KIND:

						ImagePtr = &node->Special.Incrementer.Image;
						break;

					case STRING_KIND:
					case FRACTION_KIND:

						if(!node->Special.String.Backup)
							node->Special.String.Backup = (STRPTR)LTP_Alloc(handle,node->Special.String.MaxChars + 1);

						if(node->Special.String.Backup)
						{
							strcpy(node->Special.String.Backup,((struct StringInfo *)gadget->SpecialInfo)->Buffer);

							node->Special.String.String = node->Special.String.Backup;
						}

						node->Special.String.Picker = NULL;

						node->Special.String.LeftIncrementer	= NULL;
						node->Special.String.RightIncrementer	= NULL;

						break;

					case TEXT_KIND:

						node->Special.Text.Picker = NULL;
						break;

					case INTEGER_KIND:

						node->Special.Integer.LeftIncrementer	= NULL;
						node->Special.Integer.RightIncrementer	= NULL;

						break;
				}

				if(ImagePtr)
				{
					DisposeObject(*ImagePtr);

					*ImagePtr = NULL;
				}

				node->Host = NULL;
			}

			gadget = gadget->NextGadget;
		}

		FreeGadgets(handle->List);

		handle->List = NULL;

		#ifdef DO_BOOPSI_KIND
		{
			gadget = (struct Gadget *)handle->BOOPSIList;

			while(gadget)
			{
				next = gadget->NextGadget;

				if(GETOBJECT(gadget,node))
				{
					DisposeObject(gadget);

					if(node->Type == BOOPSI_KIND)
					{
						if(node->Special.BOOPSI.ClassBase)
						{
							CloseLibrary(node->Special.BOOPSI.ClassBase);

							node->Special.BOOPSI.ClassBase = NULL;
						}
					}

					node->Host = NULL;
				}

				gadget = next;
			}

			handle->BOOPSIList = NULL;
			handle->BOOPSIPrevious = NULL;
		}
		#endif	/* DO_BOOPSI_KIND */

		LTP_Free(handle,handle->GadgetArray,sizeof(struct Gadget *) * handle->Count);

		handle->GadgetArray = NULL;

		handle->Count = handle->Index = 0;

		LTP_ResetGroups(handle->TopGroup);

		if(!bounds)
		{
			bounds = &newBounds;

			newBounds.Left		= 0;
			newBounds.Top		= 0;
			newBounds.Width		= handle->Window->Width;
			newBounds.Height	= handle->Window->Height;
		}

		left		= handle->Window->BorderLeft;
		top			= handle->Window->BorderTop;
		RightEdge	= handle->Window->BorderRight;
		BottomEdge	= handle->Window->BorderBottom;

		if(!handle->FlushLeft)
		{
			left		+= handle->InterWidth;
			RightEdge	+= handle->InterWidth;
		}

		if(!handle->FlushTop)
		{
			top			+= handle->InterHeight;
			BottomEdge	+= handle->InterHeight;
		}

		LTP_CreateGadgets(handle,bounds,left,top,left + RightEdge,top + BottomEdge);

		if(handle->Failed)
		{
			handle->SizeVerified = FALSE;

			LTP_Erase(handle);

			RefreshWindowFrame(handle->Window);

			return(FALSE);
		}

		if(clear)
		{
			LTP_Erase(handle);

			if(!handle->SizeVerified)
				RefreshWindowFrame(handle->Window);
		}

		handle->SizeVerified = FALSE;

		LTP_AddGadgets(handle);

		LTP_SelectInitialActiveGadget(handle);

		return(TRUE);
	}
	else
		return(FALSE);
}


/*****************************************************************************/


#ifndef __AROS__
BOOL
LT_RebuildTags(LayoutHandle *Handle,BOOL Clear,...)
{
	va_list VarArgs;
	BOOL	Result;

	va_start(VarArgs,Clear);
	Result = LT_RebuildTagList(Handle,Clear,(struct TagItem *)VarArgs);
	va_end(VarArgs);

	return(Result);
}
#endif


/*****************************************************************************/


BOOL LIBENT
LT_Rebuild(REG(a0) LayoutHandle *handle,REG(a1) struct IBox *bounds,REG(a2) LONG extraWidth,REG(d0) LONG extraHeight,REG(d1) BOOL clear)
{
	return(LT_RebuildTags(handle,clear,
		LAWN_Bounds,bounds,
	TAG_DONE));
}
