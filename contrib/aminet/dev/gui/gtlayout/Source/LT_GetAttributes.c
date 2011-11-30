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

#ifndef __AROS__
LONG
LT_GetAttributes(LayoutHandle *Handle,LONG ID,...)
{
	va_list VarArgs;
	LONG	Result;

	va_start(VarArgs,ID);
	Result = LT_GetAttributesA(Handle,ID,(struct TagItem *)VarArgs);
	va_end(VarArgs);

	return(Result);
}
#endif


/*****************************************************************************/


/****** gtlayout.library/LT_GetAttributesA ******************************************
*
*   NAME
*	LT_GetAttributesA -- Inquire information on a gadget.
*
*   SYNOPSIS
*	Value = LT_GetAttributesA(Handle,ID,Tags);
*	  D0                        A0   D0  A1
*
*	LONG LT_GetAttributesA(LayoutHandle *,LONG ID,struct TagItem *);
*
*	Value = LT_GetAttributes(Handle,ID,...);
*
*	LONG LT_GetAttributes(LayoutHandle *,LONG ID,...);
*
*   FUNCTION
*	All objects created by the user interface layout code posess
*	certain unique properties. The LT_GetAttributes() function will
*	will inquire this information and return it. The implementation
*	differs from gadtools.library/GT_GetGadgetAttrs in that you
*	can inquire only a small subset of the object properties possible
*	via the taglist passed in.
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure.
*
*	ID - ID number of the object to inquire information about. This
*	    is the same value you passed via LA_ID to LT_New() when
*	    you created this object.
*
*	Tags - TagItem list to receive information about the object
*	    in question.
*
*
*	LA_Left (LONG *) - Left edge of object.
*
*	LA_Top (LONG *) - Top edge of object.
*
*	LA_Width (LONG *) - Width of object.
*
*	LA_Height (LONG *) - Height of object.
*
*	LA_Chars (LONG *) - Width of object measured in character
*	    widths. (V9)
*
*	LA_Lines (LONG *) - Height of object measured in character
*	    heights. (V9)
*
*	LA_LabelLeft (LONG *) - Left edge of label text. (V9)
*
*	LA_LabelTop (LONG *) - Top edge of label text. (V9)
*
*	LABO_Object (Object *) - Returns a pointer to the BOOPSI object
*	    the BOOPSI_KIND object is based upon. (V10)
*
*	        NOTE: Don't unlink the object or dispose of it or
*	            terrible things are bound to happen.
*
*	LAST_CursorPosition (LONG *) - Current position of the cursor
*	    in STRING_KIND and FRACTION_KIND objects. (V
*
*   RESULT
*	The result depends on the object type:
*
*	    VERTICAL_KIND:
*	    HORIZONTAL_KIND:
*
*	        active page
*
*	    SCROLLER_KIND:
*
*	        current GTSC_Top value
*
*	    TAPEDECK_KIND:
*
*	        current LATD_Pressed value
*
*	    LEVEL_KIND:
*
*	        current LAVL_Level state
*
*	    CHECKBOX_KIND:
*
*	        current GTCB_Checked state
*
*	    LISTVIEW_KIND:
*
*	        current GTLV_Selected state
*
*	    MX_KIND:
*
*	        current GTMX_Active state
*
*	    CYCLE_KIND:
*
*	        current GTCY_Active state
*
*	    POPUP_KIND:
*
*	        current LAPU_Active state
*
*	    TAB_KIND:
*
*	        current LATB_Active state
*
*	    PALETTE_KIND:
*
*	        current GTPA_Color state
*
*	    SLIDER_KIND:
*
*	        current GTSL_Level state
*
*	    GAUGE_KIND:
*
*	        current LAGA_Percent state
*
*	    STRING_KIND:
*
*	        pointer to current string
*
*	    PASSWORD_KIND:
*
*	        pointer to current string
*
*	    INTEGER_KIND:
*
*	        number currently entered
*
*	    BOOPSI_KIND:
*
*	        whatever the object thinks is its
*	        current value
*
******************************************************************************
*
*/

IPTR LIBENT
LT_GetAttributesA(REG(a0) LayoutHandle *Handle,REG(d0) LONG ID,REG(a1) struct TagItem *TagList)
{
	if(Handle)
	{
		struct TagItem	*ThisTag;
		ObjectNode		*Node = NULL;

		if(ThisTag = FindTagItem(LAPR_Gadget,TagList))
			Node = ((struct Gadget *)ThisTag->ti_Data)->UserData;

		if(!Node)
		{
			if(ThisTag = FindTagItem(LAPR_Object,TagList))
				Node = (ObjectNode *)ThisTag->ti_Data;
		}

		if(!Node)
			Node = LTP_FindNode(Handle,ID);

		if(Node)
		{
			struct Gadget *Gadget = Node->Host;

			if(TagList)
			{
				struct TagItem	*TagItem;
				const struct TagItem *TempPtr = TagList;
				IPTR			*Value;

				while(TagItem = NextTagItem(&TempPtr))
				{
					Value = (IPTR *)TagItem->ti_Data;

					switch(TagItem->ti_Tag)
					{
						case LA_Left:

							if(Node->Type == FRAME_KIND && Node->Special.Frame.DrawBox)
								*Value = Node->Left + 4;
							else
								*Value = Node->Left;

							break;

						case LA_Top:

							if(Node->Type == FRAME_KIND && Node->Special.Frame.DrawBox)
								*Value = Node->Top + 2;
							else
								*Value = Node->Top;

							break;

						case LA_Width:

							if(Node->Type == FRAME_KIND && Node->Special.Frame.DrawBox)
								*Value = Node->Width - 8;
							else
								*Value = Node->Width;

							break;

						case LA_Height:

							if(Node->Type == FRAME_KIND && Node->Special.Frame.DrawBox)
								*Value = Node->Height - 4;
							else
								*Value = Node->Height;

							break;

						case LAST_CursorPosition:

							*Value = 0;

							if(Node->Type == STRING_KIND || Node->Type == FRACTION_KIND)
							{
								if(Gadget != NULL)
								{
									struct StringInfo *StringInfo = Gadget->SpecialInfo;

									*Value = StringInfo->BufferPos;
								}
							}

							break;

						#ifdef DO_BOOPSI_KIND
						{
							case LABO_Object:

								if(Node->Type == BOOPSI_KIND)
									*Value = (IPTR)Node->Host;
								else
									*Value = (IPTR)NULL;

								break;
						}
						#endif	/* DO_BOOPSI_KIND */

						case LA_Chars:

							*Value = Node->Chars;
							break;

						case LA_Lines:

							*Value = Node->Lines;
							break;

						case LA_LabelLeft:

							switch(Node->LabelPlace)
							{
								case PLACE_LEFT:

									*Value	= Node->Left - (INTERWIDTH + Node->LabelWidth);
									break;

								case PLACE_RIGHT:

									*Value	= Node->Left + Node->Width + INTERWIDTH;
									break;

								case PLACE_ABOVE:

									*Value	= Node->Left + (Node->Width - Node->LabelWidth) / 2;
									break;

								case PLACE_BELOW:

									*Value	= Node->Left + (Node->Width - Node->LabelWidth) / 2;
									break;

								case PLACE_IN:

									*Value	= Node->Left + (Node->Width - Node->LabelWidth) / 2;
									break;
							}

							break;

						case LA_LabelTop:

							switch(Node->LabelPlace)
							{
								case PLACE_LEFT:

									*Value	= Node->Top + (Node->Height - Handle->GlyphHeight) / 2;
									break;

								case PLACE_RIGHT:

									*Value	= Node->Top + (Node->Height - Handle->GlyphHeight) / 2;
									break;

								case PLACE_ABOVE:

									*Value	= Node->Top - (Handle->GlyphHeight + INTERHEIGHT);
									break;

								case PLACE_BELOW:

									*Value	= Node->Top + Node->Height + INTERHEIGHT;
									break;

								case PLACE_IN:

									*Value	= Node->Top + (Node->Height - Handle->GlyphHeight) / 2;
									break;
							}

							break;
					}
				}
			}

			switch(Node->Type)
			{
#ifdef DO_POPUP_KIND
				case POPUP_KIND:
#endif	/* DO_POPUP_KIND */
#ifdef DO_TAB_KIND
				case TAB_KIND:
#endif	/* DO_TAB_KIND */
#ifdef DO_GAUGE_KIND
				case GAUGE_KIND:
#endif	/* DO_GAUGE_KIND */
#ifdef DO_TAPEDECK_KIND
				case TAPEDECK_KIND:
#endif	/* DO_TAPEDECK_KIND */
#ifdef DO_LEVEL_KIND
				case LEVEL_KIND:
#endif	/* DO_LEVEL_KIND */

				case CHECKBOX_KIND:
				case LISTVIEW_KIND:
				case MX_KIND:
				case CYCLE_KIND:
				case PALETTE_KIND:
				case SLIDER_KIND:
				case SCROLLER_KIND:

					return(Node->Current);

				#ifdef DO_BOOPSI_KIND
				{
					case BOOPSI_KIND:
					{
						IPTR Storage;

						if(Gadget && GetAttr(Node->Special.BOOPSI.TagCurrent,(Object *)Gadget,&Storage))
							return((IPTR)Storage);
						else
							return((IPTR)(NULL));
					}
				}
				#endif	/* DO_BOOPSI_KIND */

				case VERTICAL_KIND:
				case HORIZONTAL_KIND:

					return((LONG)Node->Special.Group.ActivePage);

				case STRING_KIND:
				case FRACTION_KIND:

					if(Gadget)
					{
						if(Node->Type == FRACTION_KIND)
						{
							LTP_CopyFraction(Node->Special.String.RealString,((struct StringInfo *)Gadget->SpecialInfo)->Buffer);

							return((IPTR)Node->Special.String.RealString);
						}
						else
						{
							struct StringInfo *StringInfo;

							StringInfo = (struct StringInfo *)Gadget->SpecialInfo;

							return((IPTR)StringInfo->Buffer);
						}
					}
					else
					{
						STRPTR String = Node->Special.String.String;

						if(!String)
							String = "";

						if(Node->Type == FRACTION_KIND)
						{
							LTP_CopyFraction(Node->Special.String.RealString,String);

							String = Node->Special.String.RealString;
						}

						return((IPTR)String);
					}

					break;

				#ifdef DO_PASSWORD_KIND
				{
					case PASSWORD_KIND:

						return((IPTR)Node->Special.String.RealString);
				}
				#endif

				case INTEGER_KIND:

					if(Gadget != NULL)
					{
						struct StringInfo *	StringInfo;
						LONG				Contents;

						StringInfo = (struct StringInfo *)Gadget->SpecialInfo;

						Contents = StringInfo->LongInt;

						if(Contents < Node->Min)
							Contents = Node->Min;
						else
						{
							if(Contents > Node->Max)
								Contents = Node->Max;
						}

						LT_SetAttributes(Handle,0,
							LAPR_Object,	Node,
							GTIN_Number,	Contents,
						TAG_DONE);

						Node->Special.Integer.Number = Contents;
					}

					return(Node->Special.Integer.Number);

				case GROUP_KIND:

					return((IPTR)Node->Special.Group.ActivePage);
			}
		}
		else
		{
			return((IPTR)Handle->UserData);
		}
	}

	return (IPTR)(NULL);
}
