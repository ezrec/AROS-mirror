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
VOID
LT_SetAttributes(LayoutHandle *handle,LONG id,...)
{
	va_list VarArgs;

	va_start(VarArgs,id);
	LT_SetAttributesA(handle,id,(struct TagItem *)VarArgs);
	va_end(VarArgs);
}
#endif

/*****************************************************************************/


VOID
LTP_AddAllAndRefreshThisGadget(LayoutHandle *Handle,struct Gadget *Gadget)
{
	AddGList(Handle->Window,Handle->List,(UWORD)-1,(UWORD)-1,NULL);
	RefreshGList(Gadget,Handle->Window,NULL,1);
}

VOID
LTP_FixState(LayoutHandle *Handle,BOOL State,struct Gadget *Gadget,UWORD Bit)
{
	if(Gadget)
	{
		LTP_StripGadgets(Handle,Handle->List);

		if(State)
			Gadget->Flags |=  Bit;
		else
			Gadget->Flags &= ~Bit;

		LTP_AddAllAndRefreshThisGadget(Handle,Gadget);
	}
}

BOOL
LTP_NotifyPager(LayoutHandle *Handle,LONG ID,LONG Page)
{
	if(ID != -1)
	{
		LT_SetAttributes(Handle,ID,
			LAGR_ActivePage,Page,
		TAG_DONE);

		if(Handle->Failed)
			return(FALSE);
	}

	return(TRUE);
}


/*****************************************************************************/


/****** gtlayout.library/LT_SetAttributesA ******************************************
*
*   NAME
*	LT_SetAttributesA -- Change object attributes
*
*   SYNOPSIS
*	LT_SetAttributesA(Handle,ID,Tags);
*	                    A0   D0  A1
*
*	VOID LT_SetAttributes(LayoutHandle *,LONG,struct TagItem *);
*
*	LT_SetAttributes(Handle,ID,...);
*
*	VOID LT_SetAttributes(LayoutHandle *,LONG,...);
*
*   FUNCTION
*	This routine passes the tag item list it gets directly
*	over to GT_SetGadgetAttrsA(), so any tag items valid for
*	gadtools.library can be used here as well. Some filtering
*	may be done in order to stop objects from getting redrawn
*	if this is not absolutely necessary.
*
*   INPUTS
*	Handle - Pointer to LayoutHandle.
*
*	ID - ID number of the object to change. This is the same value
*	    you passed via LA_ID to LT_New() when you created this object.
*
*	Tags - Attributes controlling object states.
*
*
*	All gadtools.library tags are allowed, but not all are supported.
*	In addition to these tags a few additional tag values are
*	supported:
*
*	LAHN_AutoActivate (BOOL) - Set to TRUE if you want the interface
*	    to always keep a string gadget active if possible. Hitting
*	    the return key will then cause the next following string
*	    gadget to get activated, either cycling through all the
*	    string gadgets available or stopping at the next string
*	    gadget to have the LAST_LastGadget attribute set.
*
*	LAHN_UserData (APTR) - Store user specific data in the
*	    LayoutHandle->UserData entry. (V9)
*
*	LAHN_RawKeyFilter (BOOL) - Discard unprocessed IDCMP_RAWKEY
*	    events. (V13)
*	    Default: TRUE
*
*	LAHN_LocaleHook (struct Hook *) - The hook to call when
*	    locale string IDs are to be mapped to strings. The
*	    hook function is called with the following parameters:
*
*	    String = HookFunc(struct Hook *Hook,struct LayoutHandle *Handle,
*	      D0                            A0                         A2
*	                      LONG ID)
*	                           A1
*
*	    The function is to look up the string associated with the ID
*	    passed in and return the string.
*
*	LAHN_ExitFlush (BOOL) - When the LayoutHandle is finally disposed
*	    of with LT_DeleteHandle() all variables maintained by the
*	    input handling code will be flushed. For example, if you
*	    would use the LA_STRPTR tag for STRING_KIND objects the
*	    last string gadget contents would be copied into the buffer
*	    pointed to by LA_STRPTR. If you do not want to use this
*	    feature, disable it with "LAHN_ExitFlush,FALSE". (V9)
*	    Default: TRUE
*
*	GAUGE_KIND:
*
*	    LAGA_Percent (LONG) - Percentage of the gauge to fill.
*
*	    LAGA_InfoText (STRPTR) - Text to be printed within the
*	        gauge display, such as a percentage number.
*
*	BOX_KIND:
*
*	    LABX_Index (LONG) - The number of the line to change, this
*	        tag works in conjunction with the LABX_Text tag.
*
*	    LABX_Text (STRPTR) - The text to put into the line indicated
*	        by the LABX_Index tag.
*	        As of v26 the LABX_Index tag may be omitted, the library
*	        will then assume the line index will be 0.
*
*	    LABX_Lines (STRPTR *) - The text to set for the box contents,
*	        terminate the text array with NULL.
*
*	HORIZONTAL_KIND:
*	VERTICAL_KIND:
*
*	    LAGR_ActivePage (LONG) - Index number of page to display
*	        within the group.
*
*	            NOTE: requires that this group was created
*	                with the LAGR_ActivePage attribute set.
*
*	INTEGER_KIND:
*
*	    LAIN_Min (LONG) - Minimum allowed value for this
*	        object.
*
*	    LAIN_Max (LONG) - Maximum allowed value for this
*	        object.
*
*	LISTVIEW_KIND:
*
*	    LALV_Selected (LONG) - Combines GTLV_Selected and
*	        GTLV_Top (for Kickstart V37) or GTLV_MakeVisible
*	        (for Kickstart V39 and greater). This means, the
*	        list display will be changed in order to show
*	        the item to be selected. (V34)
*
*	LEVEL_KIND:
*
*	    LAVL_NumTicks (WORD) - Number of tick marks to draw
*	       next to the slider body.
*
*	PASSWORD_KIND:
*
*	    LAPW_String (STRPTR) - Secret text to use
*
*	POPUP_KIND
*
*	    LAPU_Labels (STRPTR *) - To block access to the popup
*	        menu, for example before you free the current list
*	        of labels, you can pass ~0 as the list parameter. (V25)
*
*	STRING_KIND:
*
*	    LAST_CursorPosition (LONG) - Repositions the cursor,
*	        pass -1 to move it to the end of the string. (V7)
*
*	TAPEDECK_KIND:
*
*	    LATD_Pressed (BOOL) - TRUE to make this button shown
*	        as pressed, FALSE to show it in depressed state.
*
*	BOOPSI_KIND:
*
*	    All tags are passed straight through to SetGadgetAttrs(..).
*
*	All objects:
*
*	    LA_LabelText (STRPTR) - New gadget label text to use.
*
*	    LA_LabelID (LONG) - Locale text ID to use for this object.
*
*   RESULT
*	none
*
*   SEE ALSO
*	gadtools.library/GT_SetGadgetAttrsA()
*	intuition.library/SetGadgetAttrsA
*
******************************************************************************
*
*/

VOID LIBENT
LT_SetAttributesA(REG(a0) LayoutHandle *handle,REG(d0) LONG id,REG(a1) struct TagItem *TagList)
{
	if(handle && TagList)
	{
		struct Gadget	*Gadget = NULL;
		struct TagItem	*ThisTag;
		struct TagItem	*ThisList = TagList;
		ObjectNode		*Node = NULL;

		while(ThisTag = NextTagItem(&ThisList))
		{
			switch(ThisTag->ti_Tag)
			{
				case LH_AutoActivate:

					handle->AutoActivate = ThisTag->ti_Data;
					break;

				case LH_RawKeyFilter:

					handle->RawKeyFilter = ThisTag->ti_Data;
					break;

				case LH_UserData:

					handle->UserData = (APTR)ThisTag->ti_Data;
					break;

				case LH_ExitFlush:

					handle->ExitFlush = ThisTag->ti_Data;
					break;

				case LH_LocaleHook:

					handle->LocaleHook = (struct Hook *)ThisTag->ti_Data;
					break;

				case LAPR_Gadget:

					Gadget = (struct Gadget *)ThisTag->ti_Data;
					break;

				case LAPR_Object:

					Node = (ObjectNode *)ThisTag->ti_Data;
					break;
			}
		}

		if(Node)
			Gadget = Node->Host;
		else
		{
			if(Gadget)
			{
				if(Node = (ObjectNode *)Gadget->UserData)
				{
					if(Node->Host != Gadget || Node->PointBack != Node)
						Node = NULL;
				}
			}
		}

		if(!Gadget)
		{
			if(Gadget = LTP_FindGadget(handle,id))
			{
				if(Node = (ObjectNode *)Gadget->UserData)
				{
					if(Node->Host != Gadget || Node->PointBack != Node)
						Node = NULL;
				}
			}
			else
				Node = LTP_FindNode(handle,id);
		}

		if(Node)
		{
			STATIC const Tag Filter[2] = { GA_Disabled,TAG_DONE };

			struct TagItem	*NewTags = NULL;
			ULONG			 Exclude = 0;

			switch(Node->Type)
			{
				#ifdef DO_PASSWORD_KIND
				{
					case PASSWORD_KIND:

						if(ThisTag = FindTagItem(GTST_String,TagList))
						{
							STRPTR String;
							LONG Len;

							String = (STRPTR)TagList->ti_Data;

							if(String)
								Len = strlen(String);
							else
							{
								String = "";
								Len = 0;
							}

							Exclude = GTST_String;

							strcpy(Node->Special.String.RealString,String);

							if(Len)
							{
								memset(Node->Special.String.Original,'·',Len);
								Node->Special.String.Original[Len] = 0;
							}

							GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
								GTST_String,Node->Special.String.Original,
							TAG_DONE);
						}

						break;
				}
				#endif

				#ifdef DO_BOOPSI_KIND
				{
					case BOOPSI_KIND:

						if(ThisTag = FindTagItem(GA_Disabled,TagList))
							Node->Disabled = ThisTag->ti_Data;

						if(Node->Special.BOOPSI.TagCurrent)
						{
							if(ThisTag = FindTagItem(Node->Special.BOOPSI.TagCurrent,TagList))
								Node->Current = ThisTag->ti_Data;
						}

						if(Node->Host)
							SetGadgetAttrsA(Node->Host,handle->Window,NULL,TagList);

						return;
				}
				#endif	/* DO_BOOPSI_KIND */

				case STRING_KIND:
				case FRACTION_KIND:

					if(ThisTag = FindTagItem(GTST_String,TagList))
					{
						Exclude = GTST_String;

						if(Gadget)
						{
							STRPTR String;

							String = (STRPTR)ThisTag->ti_Data;

							if(Node->Type == FRACTION_KIND)
							{
								UBYTE DecimalPoint;

								LTP_CopyFraction(Node->Special.String.RealString,String);
								String = Node->Special.String.RealString;

								DecimalPoint = LTP_DecimalPoint[0];

								if(DecimalPoint != '.')
								{
									LONG i;

									for(i = 0 ; i < strlen(String) ; i++)
									{
										if(String[i] == '.')
										{
											String[i] = DecimalPoint;
											break;
										}
									}
								}
							}

							GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
								GTST_String,String,
							TAG_DONE);
						}
						else
						{
							if(!Node->Special.String.Backup)
								Node->Special.String.Backup = (STRPTR)LTP_Alloc(handle,Node->Special.String.MaxChars + 1);

							if(Node->Special.String.Backup)
							{
								if(ThisTag->ti_Data)
									strcpy(Node->Special.String.Backup,(STRPTR)ThisTag->ti_Data);
								else
									Node->Special.String.Backup[0] = 0;

								Node->Special.String.String = Node->Special.String.Backup;
							}
						}
					}

					if(Gadget)
					{
						if(ThisTag = FindTagItem(LAST_CursorPosition,TagList))
						{
							struct StringInfo	*StringInfo = Gadget->SpecialInfo;
							LONG				 Position,Len;

							LTP_StripGadgets(handle,handle->List);

							Position = (LONG)ThisTag->ti_Data;

							Len = strlen(StringInfo->Buffer);

							if(Position == -1)
								Position = Len;
							else
							{
								if(Position < 0)
									Position = 0;
								else
								{
									if(Position > Len)
										Position = Len;
								}
							}

							StringInfo->BufferPos = Position;

							LTP_AddAllAndRefreshThisGadget(handle,Gadget);
						}
					}

					break;

				#ifdef DO_LEVEL_KIND
				{
					case LEVEL_KIND:
					{
						LevelExtra	*Special	= &Node->Special.Level;
						LONG		 Level		= Node->Current,
									 Min		= Node->Min,
									 Max		= Node->Max,
									 Plus		= Special->Plus;
						BOOL		 ChangeIt	= FALSE;

						if(ThisTag = FindTagItem(GA_Disabled,TagList))
						{
							if(Node->Disabled != ThisTag->ti_Data)
							{
								Node->Disabled = ThisTag->ti_Data;

								ChangeIt = TRUE;
							}
						}

						if(ThisTag = FindTagItem(LAVL_Level,TagList))
						{
							if((LONG)ThisTag->ti_Data != Level)
							{
								Level = (LONG)ThisTag->ti_Data;

								ChangeIt = TRUE;
							}
						}

						if(ThisTag = FindTagItem(LAVL_Min,TagList))
						{
							if((LONG)ThisTag->ti_Data != Min)
							{
								Plus = Min = (LONG)ThisTag->ti_Data;

								ChangeIt = TRUE;
							}
						}

						if(ThisTag = FindTagItem(LAVL_Max,TagList))
						{
							if((LONG)ThisTag->ti_Data != Max)
							{
								Max = (LONG)ThisTag->ti_Data;

								ChangeIt = TRUE;
							}
						}

						if(ThisTag = FindTagItem(LAVL_NumTicks,TagList))
						{
							WORD Value = (WORD)ThisTag->ti_Data;

							if(Value != Node->Special.Level.Ticks)
							{
								Node->Special.Level.NumTicks = Value;
								ChangeIt = TRUE;
							}
						}

						if(Max < Min)
						{
							Max = Min;
							ChangeIt = TRUE;
						}

						if(Level > Max)
						{
							Level = Max;
							ChangeIt = TRUE;
						}

						if(Level < Min)
						{
							Level = Min;
							ChangeIt = TRUE;
						}

						if(ChangeIt)
						{
							Node->Current	= Level;
							Node->Min		= Min;
							Node->Max		= Max;
							Special->Plus	= Plus;

							LTP_PutStorage(Node);

							if(Gadget && handle->Window)
							{
								LONG Current;

								if(Node->Special.Level.Freedom == FREEVERT)
									Current = Node->Max - Node->Current;
								else
									Current = Node->Current;

								SetGadgetAttrs(Gadget,handle->Window,NULL,
									GA_Disabled,	Node->Disabled,
									SLA_Current,	Current		- Plus,
									SLA_Max,		Node->Max	- Plus,
									SLA_NumTicks,	Node->Special.Level.NumTicks,
								TAG_DONE);

								LTP_LevelGadgetDrawLabel(handle,Node,FALSE);
							}
						}

						break;
					}
				}
				#endif	/* DO_LEVEL_KIND */

				case CHECKBOX_KIND:

					if(ThisTag = FindTagItem(GTCB_Checked,TagList))
					{
						if((Node->Current && ThisTag->ti_Data) || (!Node->Current && !ThisTag->ti_Data))
							Exclude = GTCB_Checked;
						else
						{
							Node->Current = ThisTag->ti_Data;

							LTP_PutStorage(Node);
						}
					}

					break;

				#ifdef DO_TAPEDECK_KIND
				{
					case TAPEDECK_KIND:

						if(ThisTag = FindTagItem(LATD_Pressed,TagList))
						{
							if(Node->Current != ThisTag->ti_Data && Node->Special.TapeDeck.Toggle)
							{
								Node->Current = ThisTag->ti_Data;

								LTP_FixState(handle,Node->Current,Gadget,GFLG_SELECTED);
							}
						}

						break;
				}
				#endif	/* DO_TAPEDECK_KIND */

				#ifdef DO_GAUGE_KIND
				{
					case GAUGE_KIND:
					{
						LONG Percent 		= (LONG)Node->Current;
						BOOL NeedRefresh	= FALSE;
						BOOL FullRefresh	= FALSE;

						if(ThisTag = FindTagItem(LAGA_Percent,TagList))
						{
							Percent = (LONG)ThisTag->ti_Data;

							if(Percent < 0)
								Percent = 0;
							else
							{
								if(Percent > 100)
									Percent = 100;
							}

							if(Percent != (LONG)Node->Current)
								NeedRefresh = TRUE;
						}

						if(ThisTag = FindTagItem(LAGA_InfoText,TagList))
						{
							STRPTR SomeText = (STRPTR)ThisTag->ti_Data;

							if(Node->Special.Gauge.InfoLength)
							{
								LONG Len = strlen(SomeText);

								if(Len > Node->Special.Gauge.InfoLength)
									Len = Node->Special.Gauge.InfoLength;

								CopyMem(SomeText,Node->Special.Gauge.InfoText,Len);

								Node->Special.Gauge.InfoText[Len] = 0;

								NeedRefresh = TRUE;
							}
						}

						if(ThisTag = FindTagItem(GA_Disabled,TagList))
						{
							if(Node->Disabled != ThisTag->ti_Data)
							{
								FullRefresh = Node->Disabled;
								NeedRefresh = TRUE;

								Node->Disabled = ThisTag->ti_Data;
							}
						}

						if(NeedRefresh && Gadget)
							LTP_DrawGauge(handle,Node,Percent,FullRefresh);
					}

					return;
				}
				#endif

				case LISTVIEW_KIND:

					if(ThisTag = FindTagItem(GTLV_Labels,TagList))
					{
						Node->Special.List.Labels = (struct List *)ThisTag->ti_Data;

						if(ThisTag->ti_Data == (ULONG)~0)
							Node->Min = Node->Max = -1;
						else
						{
							struct List	*List;
							LONG		 Count = 0;
							struct Node	*Item;

							if(ThisTag->ti_Data)
								List = (struct List *)ThisTag->ti_Data;
							else
							{
								STATIC const Tag Filter[2] = { GTLV_Labels,TAG_DONE };

								LONG Current = (LONG)GetTagData(GTLV_Selected,(ULONG)Node->Current,TagList);

								if(!NewTags)
								{
									if(!(NewTags = CloneTagItems(TagList)))
										return;
								}

								FilterTagItems(NewTags,(Tag *)Filter,TAGFILTER_NOT);

								List = (struct List *)&LTP_EmptyList;

								GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
									GTLV_Labels,	List,
									GTLV_Selected,	Current,
								TAG_DONE);

								Node->Special.List.Labels = List;

								DB(kprintf("GTLV_Labels: current = %ld\n",Current));
							}

							SCANLIST(List,Item)
							{
								Count++;
							}

							Node->Min = 0;

							if(Count)
								Node->Max = Count - 1;
							else
								Node->Max = 0;
						}
					}

					if(ThisTag = FindTagItem(GTLV_Selected,TagList))
					{
						Node->Current = (LONG)ThisTag->ti_Data;

						if(Gadget)
						{
							Exclude = GTLV_Selected;

							GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
								GTLV_Selected,		Node->Current,
								GTLV_Labels,		Node->Special.List.Labels,
							TAG_DONE);
						}

						LTP_PutStorage(Node);

						if(!LTP_NotifyPager(handle,Node->Special.List.AutoPageID,Node->Current))
							return;
					}

					if(ThisTag = FindTagItem(LALV_Selected,TagList))
					{
						Node->Current = (LONG)ThisTag->ti_Data;

						if(Gadget)
						{
							ULONG WhichTag;

							if(V39)
								WhichTag = GTLV_MakeVisible;
							else
								WhichTag = GTLV_Top;

							Exclude = LALV_Selected;

							GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
								GTLV_Selected,		Node->Current,
								GTLV_Labels,		Node->Special.List.Labels,

								(Node->Current < 0) ? TAG_DONE : TAG_IGNORE,0,

								WhichTag,			Node->Current,
							TAG_DONE);
						}

						LTP_PutStorage(Node);

						if(!LTP_NotifyPager(handle,Node->Special.List.AutoPageID,Node->Current))
							return;
					}

					if(ThisTag = FindTagItem(GA_Disabled,TagList))
					{
						if(!V39)
						{
							if(!NewTags)
							{
								if(!(NewTags = CloneTagItems(TagList)))
									return;
							}

							FilterTagItems(NewTags,(Tag *)Filter,TAGFILTER_NOT);
						}
					}

					break;

				case MX_KIND:

					if(ThisTag = FindTagItem(GTMX_Active,TagList))
					{
						if(Node->Current == ThisTag->ti_Data)
							Exclude = GTMX_Active;
						else
						{
							Node->Current = ThisTag->ti_Data;

							LTP_PutStorage(Node);

							if(!LTP_NotifyPager(handle,Node->Special.Radio.AutoPageID,Node->Current))
								return;
						}
					}

					if(!V39)
					{
						if(FindTagItem(GA_Disabled,TagList))
						{
							if(!(NewTags = CloneTagItems(TagList)))
								return;
							else
								FilterTagItems(NewTags,(Tag *)Filter,TAGFILTER_NOT);
						}
					}

					break;

				case CYCLE_KIND:

					if(ThisTag = FindTagItem(GTCY_Active,TagList))
					{
						if(Node->Current == (LONG)ThisTag->ti_Data)
							Exclude = GTCY_Active;
						else
						{
							Node->Current = (LONG)ThisTag->ti_Data;

							LTP_PutStorage(Node);

							if(!LTP_NotifyPager(handle,Node->Special.Cycle.AutoPageID,Node->Current))
								return;
						}
					}

					if(ThisTag = FindTagItem(GTCY_Labels,TagList))
					{
						STRPTR	*Strings;
						LONG	 Count = 0;

						if(Strings = (STRPTR *)ThisTag->ti_Data)
						{
							while(Strings[Count])
								Count++;
						}

						if(Count)
							Node->Max = Count - 1;
						else
							Node->Max = 0;
					}

					break;

				#ifdef DO_POPUP_KIND
				{
					case POPUP_KIND:
					{
						BOOL NewCurrent = FALSE,NewLabels = FALSE;

						if(ThisTag = FindTagItem(GA_Disabled,TagList))
							Node->Disabled = ThisTag->ti_Data;

						if(ThisTag = FindTagItem(LAPU_Labels,TagList))
						{
							STRPTR	*Strings;
							LONG	 Count = 0;

							if(Strings = (STRPTR *)ThisTag->ti_Data)
							{
								while(Strings[Count])
									Count++;
							}

							if(Count)
								Node->Max = Count - 1;
							else
								Node->Max = 0;

							Node->Special.Popup.Choices = (STRPTR *)ThisTag->ti_Data;

							DB(kprintf("max: %ld\n",Node->Max));

							NewLabels = TRUE;
						}

						if(ThisTag = FindTagItem(LAPU_Active,TagList))
						{
							DB(kprintf("current: %ld tag: %ld\n",Node->Current,ThisTag->ti_Data));

							if(Node->Current != ThisTag->ti_Data)
							{
								Node->Current = ThisTag->ti_Data;

								LTP_PutStorage(Node);

								if(!LTP_NotifyPager(handle,Node->Special.Popup.AutoPageID,Node->Current))
									return;

								NewCurrent = TRUE;
							}
						}

						if(Node->Host)
						{
							SetGadgetAttrs(Node->Host,handle->Window,NULL,
								NewCurrent ? PIA_Active : TAG_IGNORE,	Node->Current,
								NewLabels ? PIA_Labels : TAG_IGNORE,	Node->Special.Popup.Choices,
							TAG_MORE,TagList);
						}

						return;
					}
				}
				#endif

				#ifdef DO_TAB_KIND
				{
					case TAB_KIND:
					{
						if(ThisTag = FindTagItem(GA_Disabled,TagList))
							Node->Disabled = ThisTag->ti_Data;

						if(ThisTag = FindTagItem(LATB_Active,TagList))
						{
							DB(kprintf("current: %ld tag: %ld\n",Node->Current,ThisTag->ti_Data));

							if(Node->Current != ThisTag->ti_Data)
							{
								Node->Current = ThisTag->ti_Data;

								LTP_PutStorage(Node);

								if(!LTP_NotifyPager(handle,Node->Special.Tab.AutoPageID,Node->Current))
									return;
								else
								{
									if(Node->Host)
									{
										SetGadgetAttrs(Node->Host,handle->Window,NULL,
											TIA_Index,Node->Current,
										TAG_MORE,TagList);
									}
								}
							}
						}

						return;
					}
				}
				#endif

				case PALETTE_KIND:

					if(ThisTag = FindTagItem(GTPA_Color,TagList))
					{
						if(Node->Current == ThisTag->ti_Data)
							Exclude = GTPA_Color;
						else
						{
							Node->Current = ThisTag->ti_Data;

							LTP_PutStorage(Node);

							if(Node->Special.Palette.UsePicker)
							{
								if(Gadget)
									LTP_DrawPalette(handle,Node);

								return;
							}
						}
					}

					break;

				case INTEGER_KIND:

					if(ThisTag = FindTagItem(GTIN_Number,TagList))
					{
						LONG num = ThisTag->ti_Data;

						if(num < Node->Min)
							num = Node->Min;
						else
						{
							if(num > Node->Max)
								num = Node->Max;
						}

						if(Gadget)
						{
							#ifdef DO_HEXHOOK
							{
								if(!Node->Special.Integer.EditHook || Node->Special.Integer.CustomHook)
								{
									UBYTE				 buffer[20];
									struct StringInfo	*stringInfo;

									SPrintf(buffer,"%ld",num);

									Exclude = GTIN_Number;

									GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
										GTST_String,	buffer,
										GTIN_Number,	num,
									TAG_DONE);

									stringInfo = (struct StringInfo *)Gadget->SpecialInfo;

									stringInfo->LongInt = num;
								}
								else
								{
									UBYTE				 buffer[40];
									struct StringInfo	*stringInfo;
									STRPTR				 Index;
									LONG				 Value,Number = num,
														 Scale,Sign;

									stringInfo = (struct StringInfo *)Gadget->SpecialInfo;

									Index = stringInfo->Buffer;

									while(*Index && *Index == ' ')
										Index++;

									switch(Index[0])
									{
										case '$':

											SPrintf(buffer,"$%lx",num);
											break;

										case '&':

											if(Number < 0)
											{
												Sign = -1;
												Number = -Number;
											}
											else
												Sign = 1;

											for(Value = 0, Scale = 1 ; Number ; Number /= 8, Scale *= 10)
												Value += (Number & 7) * Scale;

											SPrintf(buffer,"&%ld",(LONG)(Sign * Value));
											break;

										case '%':

											if(Number < 0)
											{
												Sign = -1;
												Number = -Number;
											}
											else
												Sign = 1;

											for(Value = 0, Scale = 1 ; Number ; Number /= 2, Scale *= 10)
												Value += (Number & 1) * Scale;

											SPrintf(buffer,"%%%ld",(LONG)(Sign * Value));
											break;

										case '0':

											if(Index[1] == 'x')
											{
												SPrintf(buffer,"0x%lx",num);
												break;
											}

											// Fall through to...

										default:

											SPrintf(buffer,"%ld",num);
											break;
									}

									Exclude = GTIN_Number;

									GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
										GTST_String,buffer,
									TAG_DONE);

									stringInfo->LongInt = num;
								}
							}
							#else
							{
								GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
									GTIN_Number,num,
								TAG_DONE);
							}
							#endif
						}

						Node->Special.Integer.Number = num;

						LTP_PutStorage(Node);
					}

					break;

				case NUMBER_KIND:

					if(ThisTag = FindTagItem(GTNM_Number,TagList))
						Node->Special.Number.Number = ThisTag->ti_Data;

					break;

				case SLIDER_KIND:

					if(ThisTag = FindTagItem(GTSL_Level,TagList))
					{
						if(Node->Current == ThisTag->ti_Data)
							Exclude = GTSL_Level;
						else
						{
							Node->Current = ThisTag->ti_Data;

							LTP_PutStorage(Node);
						}
					}

					if(ThisTag = FindTagItem(GTSL_Min,TagList))
					{
						Node->Min = ThisTag->ti_Data;

						if(Node->Current < Node->Min)
						{
							Node->Current = Node->Min;

							LTP_PutStorage(Node);
						}
					}

					if(ThisTag = FindTagItem(GTSL_Max,TagList))
					{
						Node->Max = ThisTag->ti_Data;

						if(Node->Current > Node->Max)
						{
							Node->Current = Node->Max;

							LTP_PutStorage(Node);
						}
					}

					break;

				case SCROLLER_KIND:

					if(ThisTag = FindTagItem(GTSC_Top,TagList))
					{
						if(Node->Current == ThisTag->ti_Data)
							Exclude = GTSC_Top;
						else
						{
							Node->Current = ThisTag->ti_Data;

							LTP_PutStorage(Node);
						}
					}

					if(ThisTag = FindTagItem(GTSC_Total,TagList))
					{
						Node->Max = ThisTag->ti_Data;

						if(Node->Current > Node->Max)
						{
							Node->Current = Node->Max;

							LTP_PutStorage(Node);
						}
					}

					if(ThisTag = FindTagItem(GTSC_Visible,TagList))
						Node->Special.Scroller.Visible = ThisTag->ti_Data;

					break;

				case BOX_KIND:
				{
					BOOL Visible;

					if(Node->Special.Box.Parent)
						Visible = Node->Special.Box.Parent->Special.Group.Visible;
					else
						Visible = FALSE;

					if(ThisTag = FindTagItem(LABX_Text,TagList))
					{
						STRPTR	Text = (STRPTR)ThisTag->ti_Data;
						LONG	Index;

						Index = (LONG)GetTagData(LABX_Index,0,TagList);

						if(Index >= 0 && Index < Node->Lines)
						{
							if(Node->Special.Box.ReserveSpace)
							{
								LONG Len = strlen(Text);

								if(Len > Node->Special.Box.MaxSize)
									Len = Node->Special.Box.MaxSize;

								CopyMem(Text,Node->Special.Box.Lines[Index],Len);

								Node->Special.Box.Lines[Index][Len] = 0;
							}
							else
								Node->Special.Box.Lines[Index] = Text;
						}

						if(Visible)
							LTP_PrintBoxLine(handle,Node,Index);
					}

					if(ThisTag = FindTagItem(LABX_Lines,TagList))
					{
						STRPTR	*Lines = (STRPTR *)ThisTag->ti_Data;
						LONG	 i;

						if(Node->Special.Box.ReserveSpace)
						{
							LONG Len;

							for(i = 0 ; i < Node->Lines ; i++)
							{
								if(Lines[i])
								{
									Len = strlen(Lines[i]);

									if(Len > Node->Special.Box.MaxSize)
										Len = Node->Special.Box.MaxSize;

									CopyMem(Lines[i],Node->Special.Box.Lines[i],Len);

									Node->Special.Box.Lines[i][Len] = 0;

									if(Visible)
										LTP_PrintBoxLine(handle,Node,i);
								}
								else
									break;
							}
						}
						else
						{
							for(i = 0 ; i < Node->Lines ; i++)
							{
								if(Lines[i])
								{
									Node->Special.Box.Lines[i] = Lines[i];

									if(Visible)
										LTP_PrintBoxLine(handle,Node,i);
								}
								else
									break;
							}
						}
					}

					break;
				}

				case TEXT_KIND:

					if(ThisTag = FindTagItem(GTTX_Text,TagList))
					{
						STRPTR text = (STRPTR)ThisTag->ti_Data;

						if(!text)
							text = "";

						if(Node->Special.Text.CopyText)
						{
							LONG len;

							if(Node->Special.Text.Text)
								len = strlen(Node->Special.Text.Text) + 1;
							else
								len = 0;

							if(len)
								LTP_Free(handle,Node->Special.Text.Text,len);

							len = strlen(text);

							if(Node->Special.Text.Text = LTP_Alloc(handle,len + 1))
								strcpy(Node->Special.Text.Text,text);
						}
						else
							Node->Special.Text.Text = text;
					}

					if(ThisTag = FindTagItem(GTTX_FrontPen,TagList))
						Node->Special.Text.FrontPen = (WORD)ThisTag->ti_Data;

					if(ThisTag = FindTagItem(GTTX_BackPen,TagList))
						Node->Special.Text.BackPen = (WORD)ThisTag->ti_Data;

					break;

				case GROUP_KIND:

					if(Node->Special.Group.Paging)
					{
						if(ThisTag = FindTagItem(LAGR_ActivePage,TagList))
						{
							ObjectNode *node;

							node = Node;

							if(node->Type == GROUP_KIND)
							{
								if(node->Special.Group.ActivePage != ThisTag->ti_Data)
								{
									LONG	Left	= node->Left,
											Top		= node->Top,
											Width	= node->Width,
											Height	= node->Height;

									if(node->Label || node->Special.Group.Frame || node->Special.Group.FrameType == FRAMETYPE_Label)
									{
										Left	+= 4 + handle->GlyphWidth;
										Width	-= 2 * (4 + handle->GlyphWidth);

										if(node->Label)
										{
											Top		+= handle->GlyphHeight;
											Height	-= handle->GlyphHeight + 3;
										}
										else
										{
											Top		+= 2;
											Height	-= 5;
										}
									}
									else
									{
										if(node->Special.Group.FrameType == FRAMETYPE_Tab)
										{
											Width	-= 2 * 2;
											Left	+= 2;
											Height	-= 1;
										}
									}

									node->Special.Group.ActivePage = ThisTag->ti_Data;

									LT_LockWindow(handle->Window);

									LTP_EraseBox(&handle->RPort,Left,Top,Width,Height);

									LT_RebuildTagList(handle,FALSE,NULL);

									LT_UnlockWindow(handle->Window);
								}
							}
						}
					}

					break;
			}

			if(ThisTag = FindTagItem(LA_LabelText,TagList))
				Node->Label = (STRPTR)ThisTag->ti_Data;

			if(ThisTag = FindTagItem(LA_LabelID,TagList))
			{
				if(handle->LocaleHook)
					Node->Label = (STRPTR)CallHookPkt(handle->LocaleHook,handle,(APTR)ThisTag->ti_Data);
			}

			if(ThisTag = FindTagItem(GA_Disabled,TagList))
			{
				if((Node->Disabled && ThisTag->ti_Data) || (!Node->Disabled && !ThisTag->ti_Data))
				{
					if(!NewTags)
						NewTags = CloneTagItems(TagList);

					if(NewTags)
						FilterTagItems(NewTags,(Tag *)Filter,TAGFILTER_NOT);
				}
				else
				{
					Node->Disabled = ThisTag->ti_Data;

					if(Gadget)
					{
						struct Gadget *gad;

						switch(Node->Type)
						{
							case TEXT_KIND:

								gad = Node->Special.Text.Picker;
								break;

							case STRING_KIND:

								gad = Node->Special.String.Picker;
								break;

							case INTEGER_KIND:

								LTP_FixState(handle,Node->Disabled,Node->Special.Integer.LeftIncrementer,GFLG_DISABLED);

								gad = Node->Special.Integer.RightIncrementer;
								break;

							case FRACTION_KIND:

								LTP_FixState(handle,Node->Disabled,Node->Special.String.LeftIncrementer,GFLG_DISABLED);

								gad = Node->Special.String.RightIncrementer;
								break;

							case TAPEDECK_KIND:

								gad = Node->Host;

								Gadget = NULL;

								break;

							case PALETTE_KIND:

								gad = Node->Special.Palette.Picker;
								break;

							case BUTTON_KIND:

								if(Node->Special.Button.ButtonImage)
								{
									gad = Node->Host;

									Gadget = NULL;

									break;
								}

								// FALLS THROUGH TO

							default:

								gad = NULL;
								break;
						}

						LTP_FixState(handle,Node->Disabled,gad,GFLG_DISABLED);
					}
				}
			}

			if(Exclude)
			{
				Tag Filter[2];

				Filter[0] = Exclude;
				Filter[1] = TAG_DONE;

				if(!NewTags)
					NewTags = CloneTagItems(TagList);

				if(NewTags)
					FilterTagItems(NewTags,Filter,TAGFILTER_NOT);
			}

			if(Gadget)
			{
				struct TagItem *tags = TagList;

				if(NewTags)
					tags = NewTags;

				if(!V39 && Node->Disabled && Node->Type == SLIDER_KIND)
				{
					GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
						GA_Disabled,	FALSE,
					TAG_MORE,tags);

					GT_SetGadgetAttrs(Gadget,handle->Window,NULL,
						GA_Disabled,	TRUE,
					TAG_DONE);
				}
				else
					GT_SetGadgetAttrsA(Gadget,handle->Window,NULL,tags);
			}

			if(LIKE_STRING_KIND(Node))
				LTP_PutStorage(Node);

			FreeTagItems(NewTags);
		}
	}
}
