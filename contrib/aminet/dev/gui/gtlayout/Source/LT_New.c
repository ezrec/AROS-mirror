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

#include <clib/alib_protos.h>	/* For NewList */

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/


VOID
LTP_ReplaceLabelShortcut(LayoutHandle *Handle,ObjectNode * Node)
{
	ULONG	Len;
	STRPTR	Label,*Index = Node->Special.Button.Lines;
	BOOL	GotIt = FALSE;

	while(!GotIt)
	{
		if(Index)
		{
			if(!(Label = *Index++))
				break;
		}
		else
		{
			Label = Node->Label;
			GotIt = TRUE;
		}

		Len = 0;

		while(Label[Len])
		{
			if(Label[Len] == '_' && Label[Len + 1])
			{
				STRPTR NewLabel = LTP_Alloc(Handle,strlen(Label));

				if(NewLabel)
				{
					if(Len)
						CopyMem(Label,NewLabel,Len);

					strcpy(&NewLabel[Len],&Label[Len + 1]);

					Node->Special.Button.KeyStroke = &NewLabel[Len];

					Node->Key = ToLower(NewLabel[Len]);

					if(Index)
						Index[-1] = NewLabel;
					else
						Node->Label = NewLabel;
				}

				GotIt = TRUE;

				break;
			}

			Len++;
		}
	}
}


/*****************************************************************************/


#ifndef __AROS__
VOID
LT_AddL(LayoutHandle *handle,LONG type,ULONG labelID,LONG id,...)	/* link library only */
{
	if(handle)
	{
		struct TagItem *tagList;
		va_list varArgs;

		va_start(varArgs,id);

		tagList = (struct TagItem *)varArgs;

		if(handle->LocaleHook)
		{
			STRPTR label;

			label = (STRPTR)CallHookPkt(handle->LocaleHook,handle,(APTR)labelID);

			LT_AddA(handle,type,label,id,tagList);
		}
		else
			handle->Failed = TRUE;

		va_end(varArgs);
	}
}
#endif


/*****************************************************************************/

#ifndef __AROS__
VOID
LT_New(LayoutHandle *handle,...)	/* link library only */
{
	va_list VarArgs;

	va_start(VarArgs,handle);
	LT_NewA(handle,(struct TagItem *)VarArgs);
	va_end(VarArgs);
}
#endif


/*****************************************************************************/


#ifndef __AROS__
VOID
LT_Add(
	LayoutHandle *	Handle,
	LONG			Type,
	STRPTR			Label,
	LONG			ID,
					...)	/* link library only */
{
	va_list VarArgs;

	va_start(VarArgs,ID);
	LT_AddA(Handle,Type,Label,ID,(struct TagItem *)VarArgs);
	va_end(VarArgs);
}
#endif


/*****************************************************************************/


VOID LIBENT
LT_AddA(
	REG(a0) LayoutHandle *		Handle,
	REG(d0) LONG				Type,
	REG(d1) STRPTR				Label,
	REG(d2) LONG				ID,
	REG(a1) struct TagItem *	TagList)
{
	struct TagItem *TagItem;

	if(!Handle || Handle->Failed)
		return;

	if(Type < GENERIC_KIND)
	{
		Handle->Failed = TRUE;
		return;
	}

	if(Type == VERTICAL_KIND || Type == HORIZONTAL_KIND || Handle->TopGroup)
	{
		ObjectNode *Node;
		ULONG IDCMP;

		IDCMP = NULL;

		switch(Type)
		{
			case BUTTON_KIND:

				IDCMP = BUTTONIDCMP;
				break;

			case CHECKBOX_KIND:

				IDCMP = CHECKBOXIDCMP | IDCMP_MOUSEBUTTONS;
				break;

			case LISTVIEW_KIND:

				IDCMP = LISTVIEWIDCMP;
				break;

			case MX_KIND:

				IDCMP = MXIDCMP | IDCMP_MOUSEBUTTONS;
				break;

			case CYCLE_KIND:

				IDCMP = CYCLEIDCMP;
				break;

			case PALETTE_KIND:

				IDCMP = PALETTEIDCMP;
				break;

			case SLIDER_KIND:

				IDCMP = SLIDERIDCMP;
				break;

			case SCROLLER_KIND:

				IDCMP = SCROLLERIDCMP | ARROWIDCMP;
				break;

			case INTEGER_KIND:

				IDCMP = STRINGIDCMP | IDCMP_GADGETDOWN | IDCMP_INTUITICKS;
				break;

			case FRAME_KIND:

				IDCMP = IDCMP_MOUSEBUTTONS;
				break;

			case FRACTION_KIND:
			case STRING_KIND:
			case PASSWORD_KIND:

				IDCMP = STRINGIDCMP | IDCMP_GADGETDOWN;
				break;

			#ifdef DO_LEVEL_KIND
			{
				case LEVEL_KIND:

					IDCMP = IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE | IDCMP_GADGETUP;
					break;
			}
			#endif	/* DO_LEVEL_KIND */

			case END_KIND:

				/* new in 45.1: fail if there is no group to be closed */
				if(Handle->CurrentGroup == NULL)
				{
					Handle->Failed = TRUE;
				}
				else
				{
					Handle->CurrentGroup = Handle->CurrentGroup->Special.Group.ParentGroup;
				}

				return;

			#ifdef DO_POPUP_KIND
			{
				case POPUP_KIND:

					IDCMP = IDCMP_GADGETUP;
					break;
			}
			#endif

			#ifdef DO_TAB_KIND
			{
				case TAB_KIND:

					IDCMP = IDCMP_GADGETUP;
					break;
			}
			#endif
		}

		Handle->IDCMP |= IDCMP;

		if(Node = LTP_CreateObjectNode(Handle,Type,ID,Label))
		{
			struct MinList *ParentList;
			LONG FirstLabel;
			LONG LastLabel;
			LONG *LabelTable;
			LONG *LineTable;
			LONG FirstLine;
			LONG LastLine;
			STRPTR BoxLine;

			BoxLine = NULL;
			LabelTable = LineTable = NULL;
			FirstLabel = LastLabel = FirstLine = LastLine = -1;
			Type = Node->Type;

			if(Type == GROUP_KIND)
			{
				Node->Special.Group.ParentGroup = Handle->CurrentGroup;

				if(!ID)
					Node->ID = Handle->GroupID--;

				if(!Handle->TopGroup)
				{
					Handle->TopGroup = Node;

					ParentList = NULL;
				}
				else
					ParentList = &Handle->CurrentGroup->Special.Group.ObjectList;

				Handle->CurrentGroup = Node;

				NewList((struct List *)&Node->Special.Group.ObjectList);
			}
			else
				ParentList = &Handle->CurrentGroup->Special.Group.ObjectList;

			if(Label && !Node->NoKey)
			{
				LONG Len;

				for(Len = 0 ; Label[Len] ; Len++)
				{
					if(Label[Len] == '_')
					{
						Node->Key = ToLower(Label[Len + 1]);

						break;
					}
				}
			}

			if(TagList)
			{
				struct TagItem *TempList = TagList;
				ULONG ti_Data;

				while(TagItem = NextTagItem(&TempList))
				{
					ti_Data = TagItem->ti_Data;

					switch(TagItem->ti_Tag)
					{
						case LA_BYTE:

							Node->Storage 		= (APTR)ti_Data;
							Node->StorageType	= STORAGE_BYTE;

							LTP_GetStorage(Node);

							break;

						case LA_UBYTE:

							Node->Storage 		= (APTR)ti_Data;
							Node->StorageType	= STORAGE_UBYTE;

							LTP_GetStorage(Node);

							break;

						case LA_WORD:

							Node->Storage 		= (APTR)ti_Data;
							Node->StorageType	= STORAGE_WORD;

							LTP_GetStorage(Node);

							break;

						case LA_UWORD:

							Node->Storage 		= (APTR)ti_Data;
							Node->StorageType	= STORAGE_UWORD;

							LTP_GetStorage(Node);

							break;

						case LA_LONG:

							Node->Storage 		= (APTR)ti_Data;
							Node->StorageType	= STORAGE_LONG;

							LTP_GetStorage(Node);

							break;

						case LA_ULONG:

							Node->Storage	 	= (APTR)ti_Data;
							Node->StorageType	= STORAGE_ULONG;

							LTP_GetStorage(Node);

							break;

						case LA_STRPTR:

							Node->Storage 		= (APTR)ti_Data;
							Node->StorageType	= STORAGE_STRPTR;

							LTP_GetStorage(Node);

							break;

						case LAXB_FullSize:

							if(Type == XBAR_KIND || Type == YBAR_KIND)
								Node->Special.Bar.FullSize = ti_Data;

							break;

						case LA_LabelTable:

							LabelTable = (LONG *)ti_Data;
							break;

						case LA_FirstLabel:

							FirstLabel = ti_Data;
							break;

						case LA_LastLabel:

							LastLabel = ti_Data;
							break;

						case LA_LastGadget:

							if(Type == INTEGER_KIND)
								Node->Special.Integer.LastGadget = ti_Data;

							if(Type == STRING_KIND || Type == PASSWORD_KIND || Type == FRACTION_KIND)
								Node->Special.String.LastGadget = ti_Data;

							break;

						case LA_LabelChars:

							Node->LabelChars = ti_Data;

							break;

						case LA_HistoryLines:

							if(Type == INTEGER_KIND)
								Node->Special.Integer.MaxHistoryLines = ti_Data;

							if(Type == STRING_KIND || Type == PASSWORD_KIND || Type == FRACTION_KIND)
								Node->Special.String.MaxHistoryLines = ti_Data;

							break;

						case LA_HistoryHook:

							if(Type == INTEGER_KIND)
								Node->Special.Integer.HistoryHook = (struct Hook *)ti_Data;

							if(Type == STRING_KIND || Type == FRACTION_KIND)
								Node->Special.String.HistoryHook = (struct Hook *)ti_Data;

							break;

						case LAIN_IncrementerHook:

							if(Type == INTEGER_KIND)
								Node->Special.Integer.IncrementerHook = (struct Hook *)ti_Data;

							if(Type == FRACTION_KIND)
							{
								if(Node->Special.String.IncrementerHook = (struct Hook *)ti_Data)
									Handle->IDCMP |= IDCMP_INTUITICKS;
							}

							break;

						case LA_Min:

							Node->Min = ti_Data;

							break;

						case LA_Max:

							Node->Max = ti_Data;

							break;

						case LABX_Spacing:

							if(Type == BOX_KIND)
								Node->Special.Box.Spacing = ti_Data;

							break;

						case LABX_ReserveSpace:

							if(Type == BOX_KIND)
								Node->Special.Box.ReserveSpace = ti_Data;

							break;

						case LABX_TextPen:

							if(Type == BOX_KIND)
								Node->Special.Box.TextPen = ti_Data;

							break;

						case LABX_BackPen:

							if(Type == BOX_KIND)
								Node->Special.Box.BackPen = ti_Data;

							break;

						case LA_Chars:

							Node->Chars = ti_Data;

							break;

						case LA_LabelPlace:

							/* The group label placement cannot be
							 * changed, and never could (new in V45.1).
							 */
							if(Type != GROUP_KIND)
							{
								Node->LabelPlace = ti_Data;
		
								if(Type == MX_KIND)
								{
									if(ti_Data == PLACE_RIGHT)
										Node->LabelPlace = PLACE_RIGHT;
									else
										Node->LabelPlace = PLACE_LEFT;
								}
							}

							break;

						case LA_ExtraSpace:

							Node->ExtraSpace = (WORD)ti_Data;

							if(Node->ExtraSpace == 1)
								Node->ExtraSpace = 3;

							break;

						case LA_LayoutSpace:

							Node->LayoutSpace = (UWORD)ti_Data;
							break;

						case LA_PageSelector:

							Node->PageSelector = ti_Data;
							break;

						case LABT_ExtraFat:

							if(Node->Type == BUTTON_KIND)
								Node->Special.Button.ExtraFat = ti_Data;

							break;

						case LABT_Smaller:

							if(Node->Type == BUTTON_KIND)
								Node->Special.Button.Smaller = ti_Data;

							break;

						case LABT_DefaultCorrection:

							if(Node->Type == BUTTON_KIND)
								Node->Special.Button.DefaultCorrection = ti_Data;

							break;

						case LABT_Lines:

							if(Node->Type == BUTTON_KIND)
								Node->Special.Button.Lines = (STRPTR *)ti_Data;

							break;

						#ifdef DO_PICKSHORTCUTS
						{
							case LA_NoKey:

								Node->NoKey = ti_Data;
								break;
						}
						#endif

						case LA_DefaultSize:

							Node->DefaultSize = ti_Data;
							break;

						case LA_HighLabel:

							Node->HighLabel = ti_Data;
							break;

						case LAPA_SmallPalette:

							if(Type == PALETTE_KIND)
								Node->Special.Palette.SmallPalette = ti_Data;

							break;

						case LAPA_UsePicker:

							if(Type == PALETTE_KIND)
								Node->Special.Palette.UsePicker = ti_Data;

							break;

						case LA_Lines:

							if(Type == BOX_KIND)
							{
								LONG Count = ti_Data;

								if(Node->Lines != Count)
								{
									LTP_Free(Handle,Node->Special.Box.Lines,sizeof(STRPTR) * Node->Lines);

									Node->Special.Box.Lines = NULL;
								}

								if(!Node->Special.Box.Lines)
								{
									if(Node->Special.Box.Lines = (STRPTR *)LTP_Alloc(Handle,sizeof(STRPTR) * Count))
									{
										LONG i;

										for(i = 0 ; i < Count ; i++)
											Node->Special.Box.Lines[i] = "";
									}
								}
							}

							Node->Lines = ti_Data;

							break;

						case LA_Picker:

							if(Type == TEXT_KIND)
								Node->Special.Text.UsePicker = ti_Data;

							if(Type == STRING_KIND)
								Node->Special.String.UsePicker = ti_Data;

							break;

						case LACY_AutoPageID:

							if(Type == CYCLE_KIND)
								Node->Special.Cycle.AutoPageID = (LONG)ti_Data;

							#ifdef DO_POPUP_KIND
							{
								if(Type == POPUP_KIND)
									Node->Special.Popup.AutoPageID = (LONG)ti_Data;
							}
							#endif

							#ifdef DO_TAB_KIND
							{
								if(Type == TAB_KIND)
									Node->Special.Tab.AutoPageID = (LONG)ti_Data;
							}
							#endif

							if(Type == MX_KIND)
								Node->Special.Radio.AutoPageID = (LONG)ti_Data;

							if(Type == LISTVIEW_KIND)
								Node->Special.List.AutoPageID = (LONG)ti_Data;

							break;

						case LAGR_Spread:

							if(Type == GROUP_KIND)
								Node->Special.Group.Spread = ti_Data;

							break;

						case LAGR_AlignRight:

							if(Type == GROUP_KIND)
								Node->Special.Group.AlignRight = ti_Data;

							break;

						case LAGR_FrameGroup:

							if(Type == GROUP_KIND)
								Node->Special.Group.FrameType = ti_Data;

							break;

						case LAGR_NoIndent:

							if(Type == GROUP_KIND)
								Node->Special.Group.NoIndent = ti_Data;

							break;

						case LAGR_IndentX:

							if(Type == GROUP_KIND)
								Node->Special.Group.IndentX = ti_Data;

							break;

						case LAGR_IndentY:

							if(Type == GROUP_KIND)
								Node->Special.Group.IndentY = ti_Data;

							break;

						case LAGR_Frame:

							if(Type == GROUP_KIND)
								Node->Special.Group.Frame = ti_Data;

							break;

						case LAGR_SameSize:

							if(Type == GROUP_KIND)
								Node->Special.Group.SameSize = ti_Data;

							break;

						case LAGR_LastAttributes:

							if(Type == GROUP_KIND)
								Node->Special.Group.LastAttributes = ti_Data;

							break;

						case LAGR_ActivePage:

							if(Type == GROUP_KIND)
							{
								Node->Special.Group.ActivePage	= ti_Data;
								Node->Special.Group.Paging	= TRUE;
							}

							break;

						case LABT_ReturnKey:

							if(Type == BUTTON_KIND)
								Node->Special.Button.ReturnKey = ti_Data;

							break;

						case LABT_EscKey:

							if(Type == BUTTON_KIND)
								Node->Special.Button.EscKey = ti_Data;

							break;

						case LALV_AdjustForString:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.AdjustForString = ti_Data;

							break;

						case LALV_CursorKey:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.CursorKey = ti_Data;

							break;

						case LALV_TextAttr:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.TextAttr = (struct TextAttr *)ti_Data;

							break;

						case LALV_LockSize:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.LockSize = ti_Data;

							if(Type == TEXT_KIND)
								Node->Special.Text.LockSize = ti_Data;

							break;

						case LALV_MaxGrowX:

							if(Type == LISTVIEW_KIND)
							{
								Handle->GrowView = Node;

								Node->Special.List.MaxGrowX = ti_Data;
							}

							break;

						case LALV_MaxGrowY:

							if(Type == LISTVIEW_KIND)
							{
								Handle->GrowView = Node;

								Node->Special.List.MaxGrowY = ti_Data;
							}

							break;

						case LALV_ResizeX:

							if(Type == LISTVIEW_KIND)
							{
								Handle->ResizeObject = Node;

								Handle->IDCMP |= IDCMP_SIZEVERIFY | IDCMP_NEWSIZE;

								Node->Special.List.ResizeX = ti_Data;
							}

							if(Type == FRAME_KIND)
							{
								Handle->ResizeObject = Node;

								Handle->IDCMP |= IDCMP_SIZEVERIFY | IDCMP_NEWSIZE;

								Node->Special.Frame.ResizeX = ti_Data;
							}

							break;

						case LALV_ResizeY:

							if(Type == LISTVIEW_KIND)
							{
								Handle->ResizeObject = Node;

								Handle->IDCMP |= IDCMP_SIZEVERIFY | IDCMP_NEWSIZE;

								Node->Special.List.ResizeY = ti_Data;
							}

							if(Type == FRAME_KIND)
							{
								Handle->ResizeObject = Node;

								Handle->IDCMP |= IDCMP_SIZEVERIFY | IDCMP_NEWSIZE;

								Node->Special.Frame.ResizeY = ti_Data;
							}

							break;

						case LALV_MinChars:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.MinChars = ti_Data;

							break;

						case LALV_MinLines:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.MinLines = ti_Data;

							break;

						case LALV_FlushLabelLeft:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.FlushLabelLeft = ti_Data;

							break;

						case LALV_Labels:

							if(Type == LISTVIEW_KIND)
							{
								STRPTR *Labels = (STRPTR *)ti_Data;

								if(*Labels)
								{
									struct List *SomeList;

									if(SomeList = LTP_Alloc(Handle,sizeof(struct List)))
									{
										struct Node *SomeNode;

										NewList(SomeList);

										Node->Special.List.Labels = SomeList;

										while(*Labels)
										{
											if(SomeNode = LTP_Alloc(Handle,sizeof(struct Node) + strlen(*Labels) + 1))
											{
												SomeNode->ln_Name = (STRPTR)(SomeNode + 1);

												strcpy(SomeNode->ln_Name,*Labels++);

												AddTail(SomeList,SomeNode);
											}
										}
									}
								}
							}

							break;

						case LA_Link:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.LinkID = ti_Data;

							if(Type == STRING_KIND)
								Node->Special.String.LinkID = ti_Data;

							if(Type == BOOPSI_KIND)
								Node->Special.BOOPSI.Link = (LONG)ti_Data;

							break;

						#ifdef DO_TAPEDECK_KIND
						{
							case LATD_ButtonType:

								if(Type == TAPEDECK_KIND)
									Node->Special.TapeDeck.ButtonType = ti_Data;

								break;

							case LATD_Toggle:

								if(Type == TAPEDECK_KIND)
									Node->Special.TapeDeck.Toggle = ti_Data;

								break;

							case LATD_Pressed:

								if(Type == TAPEDECK_KIND)
									Node->Current = ti_Data;

								break;

							case LATD_Tick:

								if(Type == TAPEDECK_KIND)
									Node->Special.TapeDeck.Tick = ti_Data;

								break;

							case LATD_Smaller:

								if(Type == TAPEDECK_KIND)
									Node->Special.TapeDeck.Smaller = ti_Data;

								break;
						}
						#endif	/* DO_TAPEDECK_KIND */

						#ifdef DO_GAUGE_KIND
						{
							case LAGA_Percent:

								if(Type == GAUGE_KIND)
								{
									LONG Percent = (LONG)ti_Data;

									if(Percent <= 0)
										Node->Current = 0;
									else
									{
										if(Percent > 100)
											Node->Current = 100;
										else
											Node->Current = Percent;
									}
								}

								break;

							case LAGA_NoTicks:

								if(Type == GAUGE_KIND)
									Node->Special.Gauge.NoTicks = ti_Data;

								break;

							case LAGA_Discrete:

								if(Type == GAUGE_KIND)
									Node->Special.Gauge.Discrete = ti_Data;

								break;

							case LAGA_InfoLength:

								if(Type == GAUGE_KIND)
								{
									LONG Length = (LONG)ti_Data;

									if(Length > Node->Special.Gauge.InfoLength)
									{
										STRPTR NewText;

										if(NewText = (STRPTR)LTP_Alloc(Handle,Length + 1))
										{
											if(Node->Special.Gauge.InfoText)
											{
												strcpy(NewText,Node->Special.Gauge.InfoText);

												LTP_Free(Handle,Node->Special.Gauge.InfoText,Node->Special.Gauge.InfoLength + 1);
											}

											NewText[0] = 0;

											Node->Special.Gauge.InfoText	= NewText;
											Node->Special.Gauge.InfoLength	= Length;
										}
									}
								}

								break;

							case LAGA_InfoText:

								if(Type == GAUGE_KIND)
								{
									STRPTR	SomeText	= (STRPTR)ti_Data;
									LONG	Len			= strlen(SomeText);

									if(!Node->Special.Gauge.InfoLength)
									{
										if(Node->Special.Gauge.InfoText = (STRPTR)LTP_Alloc(Handle,Len + 1))
											Node->Special.Gauge.InfoLength = Len;
									}

									if(Node->Special.Gauge.InfoLength)
									{
										if(Len > Node->Special.Gauge.InfoLength)
											Len = Node->Special.Gauge.InfoLength;

										CopyMem(SomeText,Node->Special.Gauge.InfoText,Len);

										Node->Special.Gauge.InfoText[Len] = 0;
									}
								}

								break;
						}
						#endif

						case LALV_ExtraLabels:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.ExtraLabels = (STRPTR *)ti_Data;

							break;

						case LACY_TabKey:

							if(Type == CYCLE_KIND)
								Node->Special.Cycle.TabKey = ti_Data;

							#ifdef DO_POPUP_KIND
							{
								if(Type == POPUP_KIND)
									Node->Special.Popup.TabKey = ti_Data;
							}
							#endif

							#ifdef DO_TAB_KIND
							{
								if(Type == TAB_KIND)
									Node->Special.Tab.TabKey = ti_Data;
							}
							#endif

							if(Type == MX_KIND)
								Node->Special.Radio.TabKey = ti_Data;

							break;

						#ifdef DO_TAB_KIND
						{
							case LATB_FullWidth:

								if(Type == TAB_KIND)
									Node->Special.Tab.FullWidth = ti_Data;

								break;
						}
						#endif

						#ifdef DO_BOOPSI_KIND
						{
							case LABO_FullWidth:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.FullWidth = (WORD)ti_Data;

								break;

							case LABO_ActivateHook:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.ActivateHook = (struct Hook *)ti_Data;

								break;

							case LABO_FullHeight:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.FullHeight = (WORD)ti_Data;

								break;

							case LABO_RelFontHeight:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.RelFontHeight = (WORD)ti_Data;

								break;

							case LABO_TagScreen:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.TagScreen = ti_Data;

								break;

							case LABO_TagCurrent:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.TagCurrent = ti_Data;

								break;

							case LABO_TagLink:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.TagLink = ti_Data;

								break;

							case LABO_ExactWidth:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.ExactWidth = ti_Data;

								break;

							case LABO_ExactHeight:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.ExactHeight = ti_Data;

								break;

							case LABO_TagTextAttr:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.TagTextAttr = ti_Data;

								break;

							case LABO_TagDrawInfo:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.TagDrawInfo = ti_Data;

								break;

							case LABO_ClassInstance:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.ClassInstance = (Class *)ti_Data;

								break;

							case LABO_ClassName:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.ClassName = (STRPTR)ti_Data;

								break;

							case LABO_ClassLibraryName:

								if(Type == BOOPSI_KIND)
									Node->Special.BOOPSI.ClassLibraryName = (STRPTR)ti_Data;

								break;
						}
						#endif	/* DO_BOOPSI_KIND */

						case LAIM_Image:

							if(Node->Type == IMAGE_KIND)
								Node->Special.Image.Image = (struct Image *)ti_Data;

							break;

						case LAIM_BitMap:

							if(Node->Type == IMAGE_KIND)
								Node->Special.Image.BitMap = (struct BitMap *)ti_Data;

							break;

						case LAIM_BitMapMask:

							if(Node->Type == IMAGE_KIND)
								Node->Special.Image.BitMapMask = (PLANEPTR)ti_Data;

							break;

						case LAIM_BitMapLeft:

							if(Node->Type == IMAGE_KIND)
								Node->Special.Image.BitMapLeft = ti_Data;

							break;

						case LAIM_BitMapTop:

							if(Node->Type == IMAGE_KIND)
								Node->Special.Image.BitMapTop = ti_Data;

							break;

						case LAIM_BitMapWidth:

							if(Node->Type == IMAGE_KIND)
								Node->Special.Image.BitMapWidth = ti_Data;

							break;

						case LAIM_BitMapHeight:

							if(Node->Type == IMAGE_KIND)
								Node->Special.Image.BitMapHeight = ti_Data;

							break;

						case LAFR_RefreshHook:

							if(Type == FRAME_KIND)
								Node->Special.Frame.RefreshHook = (struct Hook *)ti_Data;

							break;

						case LAFR_InnerWidth:

							if(Type == FRAME_KIND)
								Node->Special.Frame.InnerWidth = ti_Data;

							break;

						case LAFR_InnerHeight:

							if(Type == FRAME_KIND)
								Node->Special.Frame.InnerHeight = ti_Data;

							break;

						case LA_DrawBox:

							if(Type == FRAME_KIND)
								Node->Special.Frame.DrawBox = ti_Data;

							if(Type == BOX_KIND)
								Node->Special.Box.DrawBox = ti_Data;

							break;

						case LAFR_GenerateEvents:

							if(Type == FRAME_KIND)
								Node->Special.Frame.GenerateEvents = ti_Data;

							break;

						case LABX_AlignText:

							if(Type == BOX_KIND)
								Node->Special.Box.AlignText = ti_Data;

							break;

						case LABX_Line:

							if(Type == BOX_KIND)
								BoxLine = (STRPTR)ti_Data;

							break;

						case LABX_LineID:

							if(Type == BOX_KIND)
							{
								if(Handle->LocaleHook)
									BoxLine = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)ti_Data);
								else
									Handle->Failed = TRUE;
							}

							break;

						case LABX_Labels:

							if(Type == BOX_KIND)
							{
								STRPTR	*Labels = (STRPTR *)ti_Data;
								LONG	 Count	= 0;

								while(Labels[Count])
									Count++;

								if(Node->Lines != Count)
								{
									LTP_Free(Handle,Node->Special.Box.Lines,sizeof(STRPTR) * Node->Lines);

									Node->Special.Box.Lines = NULL;
								}

								if(Count)
								{
									if(!Node->Special.Box.Lines)
									{
										if(Node->Special.Box.Lines = LTP_Alloc(Handle,sizeof(STRPTR) * Count))
										{
											LONG i;

											for(i = 0 ; i < Count ; i++)
												Node->Special.Box.Lines[i] = "";
										}
									}
								}

								Node->Lines = Count;

								Node->Special.Box.Labels = Labels;
							}

							break;

						case LABX_Lines:

							if(Type == BOX_KIND)
							{
								STRPTR	*Lines = (STRPTR *)ti_Data;
								LONG	 Count = 0;

								while(Lines[Count])
									Count++;

								if(Node->Lines && Count > Node->Lines)
									Count = Node->Lines;

								if(Count)
								{
									if(!Node->Special.Box.Lines)
										Node->Special.Box.Lines = LTP_Alloc(Handle,sizeof(STRPTR) * Count);

									if(Node->Special.Box.Lines)
									{
										LONG i;

										for(i = 0 ; i < Count ; i++)
											Node->Special.Box.Lines[i] = Lines[i];
									}
								}

								Node->Lines = Count;
							}

							break;

						case LABX_LineTable:

							if(Type == BOX_KIND)
								LineTable = (LONG *)ti_Data;

							break;

						case LABX_FirstLine:

							if(Type == BOX_KIND)
								FirstLine = ti_Data;

							break;

						case LABX_LastLine:

							if(Type == BOX_KIND)
								LastLine = ti_Data;

							break;

						case GA_Disabled:

							Node->Disabled = ti_Data;
							break;

						case GTCB_Checked:

							if(Type == CHECKBOX_KIND)
								Node->Current = ti_Data;

							break;

						case GTLV_Labels:

							if(Type == LISTVIEW_KIND)
							{
								if(ti_Data)
									Node->Special.List.Labels = (struct List *)ti_Data;
								else
									Node->Special.List.Labels = (struct List *)&LTP_EmptyList;
							}

							break;

						case GTLV_ReadOnly:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.ReadOnly = ti_Data;

							break;

						case GTLV_Selected:
						case LALV_Selected:

							if(Type == LISTVIEW_KIND)
								Node->Current = (LONG)ti_Data;

							break;

						case GTLV_CallBack:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.CallBack = (struct Hook *)ti_Data;

							break;

						case GTLV_MaxPen:

							if(Type == LISTVIEW_KIND)
								Node->Special.List.MaxPen = ti_Data;

							break;

						case GTMX_Labels:

							if(Type == MX_KIND)
								Node->Special.Radio.Choices = (STRPTR *)ti_Data;

							break;

						case GTMX_Active:

							if(Type == MX_KIND)
								Node->Current = ti_Data;

							break;

						case GTMX_TitlePlace:

							if(Type == MX_KIND && Label)
							{
								if(ti_Data == PLACE_RIGHT)
									Node->Special.Radio.TitlePlace = PLACETEXT_RIGHT;
								else
									Node->Special.Radio.TitlePlace = PLACETEXT_LEFT;
							}

							break;

						case GTTX_Text:

							if(Type == TEXT_KIND)
								Node->Special.Text.Text = (STRPTR)ti_Data;

							break;

						case GTTX_FrontPen:

							if(Type == TEXT_KIND)
								Node->Special.Text.FrontPen = (WORD)ti_Data;

							if(Type == NUMBER_KIND)
								Node->Special.Number.FrontPen = (WORD)ti_Data;

							break;

						case GTTX_BackPen:

							if(Type == TEXT_KIND)
								Node->Special.Text.BackPen = (WORD)ti_Data;

							if(Type == NUMBER_KIND)
								Node->Special.Number.BackPen = (WORD)ti_Data;

							break;

						case GTTX_CopyText:

							if(Type == TEXT_KIND)
								Node->Special.Text.CopyText = ti_Data;

							break;

						case GTTX_Border:

							if(Type == TEXT_KIND)
								Node->Special.Text.Border = ti_Data;

							break;

						case GTNM_Number:

							if(Type == NUMBER_KIND)
								Node->Special.Number.Number = ti_Data;

							break;

						case GTNM_MaxNumberLen:

							if(Type == NUMBER_KIND)
								Node->Special.Number.MaxNumberLen = ti_Data;

							break;

						case GTNM_Format:

							if(Type == NUMBER_KIND)
								Node->Special.Number.Format = (STRPTR)ti_Data;

							break;

						case GTNM_Border:

							if(Type == NUMBER_KIND)
								Node->Special.Number.Border = ti_Data;

							break;

						case GTTX_Justification:

							if(Type == NUMBER_KIND)
								Node->Special.Number.Justification = ti_Data;

							if(Type == TEXT_KIND)
								Node->Special.Text.Justification = ti_Data;

							break;

						case LAPU_CentreActive:

							if(Type == POPUP_KIND)
								Node->Special.Popup.CentreActive = ti_Data;

							break;

						case GTCY_Labels:

							if(Type == CYCLE_KIND)
								Node->Special.Cycle.Choices = (STRPTR *)ti_Data;

							#ifdef DO_POPUP_KIND
							{
								if(Type == POPUP_KIND)
									Node->Special.Popup.Choices = (STRPTR *)ti_Data;
							}
							#endif

							#ifdef DO_TAB_KIND
							{
								if(Type == TAB_KIND)
									Node->Special.Tab.Choices = (STRPTR *)ti_Data;
							}
							#endif

							break;

						case GTCY_Active:

							if(Type == CYCLE_KIND)
								Node->Current = ti_Data;

							#ifdef DO_POPUP_KIND
							{
								if(Type == POPUP_KIND)
									Node->Current = ti_Data;
							}
							#endif

							#ifdef DO_TAB_KIND
							{
								if(Type == TAB_KIND)
									Node->Current = ti_Data;
							}
							#endif

							break;

						case GTPA_Depth:

							if(Type == PALETTE_KIND)
							{
								Node->Special.Palette.Depth = ti_Data;

								if(Handle->MaxPen < (1L << Node->Special.Palette.Depth) - 1)
									Handle->MaxPen = (1L << Node->Special.Palette.Depth) - 1;
							}

							break;

						case GTPA_Color:

							if(Type == PALETTE_KIND)
								Node->Current = ti_Data;

							break;

						case GTPA_ColorOffset:

							if(Type == PALETTE_KIND)
								Node->Min = ti_Data;

							break;

						case GTPA_NumColors:

							if(Type == PALETTE_KIND)
							{
								Node->Special.Palette.NumColours = ti_Data;

								if(Node->Special.Palette.ColourTable)
								{
									LONG i;

									for(i = 0 ; i < Node->Special.Palette.NumColours ; i++)
									{
										if(Node->Special.Palette.ColourTable[i] > Handle->MaxPen)
											Handle->MaxPen = Node->Special.Palette.ColourTable[i];
									}
								}
							}

							break;

						case GTPA_ColorTable:

							if(Type == PALETTE_KIND)
							{
								Node->Special.Palette.ColourTable = (UBYTE *)ti_Data;

								if(Node->Special.Palette.NumColours)
								{
									LONG i;

									for(i = 0 ; i < Node->Special.Palette.NumColours ; i++)
									{
										if(Node->Special.Palette.ColourTable[i] > Handle->MaxPen)
											Handle->MaxPen = Node->Special.Palette.ColourTable[i];
									}
								}
							}

							break;

						case GA_RelVerify:

							if(Type == SCROLLER_KIND)
								Node->Special.Scroller.RelVerify = ti_Data;

							break;

						case GA_Immediate:

							if(Type == SCROLLER_KIND)
								Node->Special.Scroller.Immediate = ti_Data;

							break;

						case GTSC_Top:

							if(Type == SCROLLER_KIND)
								Node->Current = ti_Data;

							break;

						case GTSC_Total:

							if(Type == SCROLLER_KIND)
								Node->Max = ti_Data;

							break;

						case GTSC_Visible:

							if(Type == SCROLLER_KIND)
								Node->Special.Scroller.Visible = ti_Data;

							break;

						case GTSC_Arrows:

							if(Type == SCROLLER_KIND)
								Node->Special.Scroller.Arrows = (ti_Data != 0);

							break;

						case LASC_Thin:

							if(Type == SCROLLER_KIND)
								Node->Special.Scroller.Thin = ti_Data;

							break;

						case LASC_FullSize:

							if(Type == SCROLLER_KIND)
							{
								Node->Special.Scroller.FullSize = ti_Data;
								Node->Special.Scroller.Parent = Handle->CurrentGroup;
							}

							break;

						case PGA_Freedom:

							if(Type == SCROLLER_KIND)
								Node->Special.Scroller.Vertical = (ti_Data == LORIENT_VERT);

							#ifdef DO_LEVEL_KIND
							{
								if(Type == LEVEL_KIND)
									Node->Special.Level.Freedom = (ti_Data == FREEVERT) ? FREEVERT : FREEHORIZ;
							}
							#endif /* DO_LEVEL_KIND */

							break;

						#ifdef DO_LEVEL_KIND
						{
							case LAVL_Freedom:

								if(Type == LEVEL_KIND)
									Node->Special.Level.Freedom = (ti_Data == FREEVERT) ? FREEVERT : FREEHORIZ;

								break;

							case LAVL_Ticks:

								if(Type == LEVEL_KIND)
									Node->Special.Level.Ticks = ti_Data;

								break;

							case LAVL_NumTicks:

								if(Type == LEVEL_KIND)
									Node->Special.Level.NumTicks = ti_Data;

								break;
						}
						#endif	/* DO_LEVEL_KIND */

						case GTSL_Min:

							if(Type == SLIDER_KIND || Type == LEVEL_KIND)
								Node->Min = ti_Data;

							break;

						case GTSL_Max:

							if(Type == SLIDER_KIND || Type == LEVEL_KIND)
								Node->Max = ti_Data;

							break;

						case GTSL_Level:

							if(Type == SLIDER_KIND || Type == LEVEL_KIND)
								Node->Current = ti_Data;

							break;

						case GTSL_LevelFormat:

							#ifdef DO_LEVEL_KIND
							{
								if(Type == LEVEL_KIND)
									Node->Special.Level.LevelFormat = (STRPTR)ti_Data;
							}
							#endif	/* DO_LEVEL_KIND */

							if(Type == SLIDER_KIND)
								Node->Special.Slider.LevelFormat = (STRPTR)ti_Data;

							break;

						case GTSL_LevelPlace:

							#ifdef DO_LEVEL_KIND
							{
								if(Type == LEVEL_KIND)
								{
									if(ti_Data == PLACE_RIGHT)
										Node->Special.Level.LevelPlace = PLACETEXT_RIGHT;
									else
										Node->Special.Level.LevelPlace = PLACETEXT_LEFT;
								}
							}
							#endif	/* DO_LEVEL_KIND */

							if(Type == SLIDER_KIND)
							{
								if(ti_Data == PLACE_RIGHT)
									Node->Special.Slider.LevelPlace = PLACETEXT_RIGHT;
								else
									Node->Special.Slider.LevelPlace = PLACETEXT_LEFT;
							}

							break;

						case GTSL_DispFunc:

							#ifdef DO_LEVEL_KIND
							{
								if(Type == LEVEL_KIND)
									Node->Special.Level.DispFunc = (DISPFUNC)ti_Data;
							}
							#endif	/* DO_LEVEL_KIND */

							if(Type == SLIDER_KIND)
								Node->Special.Slider.DispFunc = (DISPFUNC)ti_Data;

							break;

						case LASL_FullCheck:

							if(Type == SLIDER_KIND)
								Node->Special.Slider.FullLevelCheck = ti_Data;

							#ifdef DO_LEVEL_KIND
							{
								if(Type == LEVEL_KIND)
									Node->Special.Level.FullLevelCheck = ti_Data;
							}
							#endif	/* DO_LEVEL_KIND */

							break;

						case GTST_String:

							if(Type == STRING_KIND || Type == PASSWORD_KIND || Type == FRACTION_KIND)
								Node->Special.String.String = (STRPTR)ti_Data;

							break;

						case LAST_Activate:

							if(Type == STRING_KIND || Type == PASSWORD_KIND || Type == FRACTION_KIND)
								Node->Special.String.Activate = ti_Data;

							if(Type == INTEGER_KIND)
								Node->Special.Integer.Activate = ti_Data;

							break;

						case GTST_MaxChars:

							if(Type == STRING_KIND || Type == PASSWORD_KIND || Type == FRACTION_KIND)
								Node->Special.String.MaxChars = ti_Data;

							break;

						case GTST_EditHook:

							if(Type == STRING_KIND)
								Node->Special.String.EditHook = (struct Hook *)ti_Data;
							else
							{
								if(Type == INTEGER_KIND)
								{
									if(Node->Special.Integer.EditHook = (struct Hook *)ti_Data)
										Node->Special.Integer.CustomHook = TRUE;
								}
							}

							break;

						case LAST_ValidateHook:

							if(Type == STRING_KIND || Type == PASSWORD_KIND || Type == FRACTION_KIND)
								Node->Special.String.ValidateHook = (struct Hook *)ti_Data;
							else
							{
								if(Type == INTEGER_KIND)
									Node->Special.Integer.ValidateHook = (struct Hook *)ti_Data;
							}

							break;

						case STRINGA_Justification:

							if(Type == STRING_KIND || Type == PASSWORD_KIND || Type == FRACTION_KIND)
								Node->Special.String.Justification = ti_Data;
							else
							{
								if(Type == INTEGER_KIND)
									Node->Special.Integer.Justification = ti_Data;
							}

							break;

						case LAIN_UseIncrementers:

							if(Type == INTEGER_KIND)
							{
								if(Node->Special.Integer.UseIncrementers = ti_Data)
									Handle->IDCMP |= IDCMP_INTUITICKS;
							}

							break;

						case GTIN_Number:

							if(Type == INTEGER_KIND)
								Node->Special.Integer.Number = ti_Data;

							break;

						case GTIN_MaxChars:

							if(Type == INTEGER_KIND)
								Node->Special.Integer.MaxChars = ti_Data;

							break;
					}
				}
			}

			if(Handle->Failed)
				return;

			switch(Type)
			{
				case STRING_KIND:
				case INTEGER_KIND:
				case FRACTION_KIND:

					if(Node->Special.String.MaxHistoryLines)
					{
						if(!Node->Special.Integer.HistoryHook)
						{
							struct Hook *Hook;

							if(Hook = (struct Hook *)LTP_Alloc(Handle,sizeof(struct Hook) + sizeof(struct MinList)))
							{
								struct MinList *List;

								List = Hook->h_Data = (APTR)(Hook + 1);

								NewList((struct List *)List);

								Hook->h_Entry = (HOOKFUNC)LTP_DefaultHistoryHook;

								Node->Special.Integer.HistoryHook = Hook;
							}
						}

						if(Node->Special.Integer.HistoryHook)
						{
							struct Node	*TextNode;
							struct MinList *List;

							List = (struct List *)Node->Special.Integer.HistoryHook->h_Data;

							Node->Special.Integer.LayoutHandle = Handle;
							Node->Special.Integer.NumHistoryLines = 0;

							TextNode = (struct Node *)List->mlh_Head;

							while(TextNode->ln_Succ)
							{
								Node->Special.Integer.NumHistoryLines++;

								TextNode = TextNode->ln_Succ;
							}
						}
					}

					break;

				#ifdef DO_TAB_KIND
				{
					case TAB_KIND:

						Node->Special.Tab.Parent = Handle->CurrentGroup;
						break;
				}
				#endif

				#ifdef DO_BOOPSI_KIND
				{
					case BOOPSI_KIND:

						Node->Special.BOOPSI.Parent = Handle->CurrentGroup;

						if(Node->Special.BOOPSI.ClassTags = CloneTagItems(TagList))
						{
							STATIC const Tag Exclude[] =
							{
								LA_Chars,
								LA_Lines,
								LABO_TagCurrent,
								LABO_TagTextAttr,
								LABO_TagDrawInfo,
								LABO_ClassInstance,
								LABO_ClassName,
								LABO_ClassLibraryName,
								LABO_ExactWidth,
								LABO_ExactHeight,

								TAG_DONE
							};

							FilterTagItems(Node->Special.BOOPSI.ClassTags,(Tag *)Exclude,TAGFILTER_NOT);
						}
						else
							Handle->Failed = TRUE;

						break;
				}
				#endif	/* DO_BOOPSI_KIND */

				case TEXT_KIND:

					if(Node->Special.Text.CopyText && Node->Special.Text.Text)
					{
						STRPTR text;

						if(text = LTP_Alloc(Handle,strlen(Node->Special.Text.Text) + 1))
						{
							strcpy(text,Node->Special.Text.Text);

							Node->Special.Text.Text = text;
						}
					}

					break;

				case BOX_KIND:

					if(FirstLine != -1 && LastLine != -1)
					{
						if(FirstLine <= LastLine && Handle->LocaleHook)
						{
							LONG Count = LastLine - FirstLine + 1;

							if(Node->Lines && Count > Node->Lines)
								Count = Node->Lines;
							else
								Node->Lines = Count;

							if(Count)
							{
								if(!Node->Special.Box.Lines)
									Node->Special.Box.Lines = LTP_Alloc(Handle,sizeof(STRPTR) * Count);

								if(Node->Special.Box.Lines)
								{
									LONG i;

									for(i = 0 ; i < Count ; i++)
										Node->Special.Box.Lines[i] = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)(FirstLine + i));
								}
							}
						}
						else
							Handle->Failed = TRUE;
					}
					else
					{
						if(LineTable)
						{
							if(Handle->LocaleHook)
							{
								LONG Count = 0;

								while(LineTable[Count] != -1)
									Count++;

								if(Node->Lines && Count > Node->Lines)
									Count = Node->Lines;
								else
									Node->Lines = Count;

								if(Count)
								{
									if(!Node->Special.Box.Lines)
										Node->Special.Box.Lines = LTP_Alloc(Handle,sizeof(STRPTR) * Count);

									if(Node->Special.Box.Lines)
									{
										LONG i;

										for(i = 0 ; i < Count ; i++)
											Node->Special.Box.Lines[i] = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)(LineTable[i]));
									}
								}
							}
							else
								Handle->Failed = TRUE;
						}
						else
						{
							if(BoxLine)
							{
								LONG Count,i;

								for(i = 0, Count = 1 ; i < strlen(BoxLine) ; i++)
								{
									if(BoxLine[i] == '\n')
										Count++;
								}

								if(Node->Lines && Count > Node->Lines)
									Count = Node->Lines;
								else
									Node->Lines = Count;

								if(Count)
								{
									if(!Node->Special.Box.Lines)
										Node->Special.Box.Lines = LTP_Alloc(Handle,sizeof(STRPTR) * Count);

									if(Node->Special.Box.Lines)
									{
										STRPTR From,To;
										LONG Len;

										From = BoxLine;

										for(i = 0 ; i < Count ; i++)
										{
											for(To = From ; *To ; To++)
											{
												if(*To == '\n')
													break;
											}

											Len = (LONG)(To - From);

											if(Node->Special.Box.Lines[i] = LTP_Alloc(Handle,Len + 1))
											{
												if(Len)
													CopyMem(From,Node->Special.Box.Lines[i],Len);

												Node->Special.Box.Lines[i][Len] = 0;

												From = To + 1;
											}
										}
									}
								}
							}
						}
					}

					break;
			}

			if((FirstLabel != -1 && LastLabel != -1) || LabelTable)
			{
				if(!Handle->LocaleHook)
					Handle->Failed = TRUE;
				else
				{
					if(Type == LISTVIEW_KIND)
					{
						struct List *SomeList;

						if(SomeList = (struct List *)LTP_Alloc(Handle,sizeof(struct MinList)))
						{
							struct Node	*SomeNode;
							LONG		 Count = (LastLabel - FirstLabel + 1),i;

							NewList(SomeList);

							Node->Special.List.Labels = SomeList;

							if(LabelTable)
							{
								while(*LabelTable != -1)
								{
									if(SomeNode = LTP_Alloc(Handle,sizeof(struct Node)))
									{
										SomeNode->ln_Name = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)(*LabelTable++));

										AddTail(SomeList,SomeNode);
									}
								}
							}
							else
							{
								for(i = 0 ; i < Count ; i++)
								{
									if(SomeNode = LTP_Alloc(Handle,sizeof(struct Node)))
									{
										SomeNode->ln_Name = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)(FirstLabel + i));

										AddTail(SomeList,SomeNode);
									}
								}
							}
						}
					}
					else
					{
						if(Type == MX_KIND || Type == CYCLE_KIND || Type == POPUP_KIND || Type == TAB_KIND || Type == BOX_KIND || Type == BUTTON_KIND)
						{
							STRPTR	*Labels;
							LONG	 Count,i;

							if(LabelTable)
							{
								LONG *Index = LabelTable;

								for(Count = 0 ; *Index != -1 ; Count++, Index++);
							}
							else
								Count = (LastLabel - FirstLabel + 1);

							if(Labels = LTP_Alloc(Handle,sizeof(STRPTR) * (Count + 1)))
							{
								if(LabelTable)
								{
									for(i = 0 ; i < Count ; i++)
										Labels[i] = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)(*LabelTable++));
								}
								else
								{
									for(i = 0 ; i < Count ; i++)
										Labels[i] = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)(FirstLabel + i));
								}

								Labels[i] = NULL;

								switch(Type)
								{
									case BUTTON_KIND:

										Node->Special.Button.Lines = Labels;
										break;

									case MX_KIND:

										Node->Special.Radio.Choices = Labels;
										break;

									case CYCLE_KIND:

										Node->Special.Cycle.Choices = Labels;
										break;

									#ifdef DO_POPUP_KIND
									{
										case POPUP_KIND:

											Node->Special.Popup.Choices = Labels;
											break;
									}
									#endif

									#ifdef DO_TAB_KIND
									{
										case TAB_KIND:

											Node->Special.Tab.Choices = Labels;
											break;
									}
									#endif

									case BOX_KIND:

										if(!Node->Special.Box.Lines)
										{
											if(Node->Special.Box.Lines = LTP_Alloc(Handle,sizeof(STRPTR) * Count))
											{
												LONG i;

												for(i = 0 ; i < Count ; i++)
													Node->Special.Box.Lines[i] = "";
											}
										}

										Node->Lines = Count;

										Node->Special.Box.Labels = Labels;

										break;
								}
							}
						}
					}
				}
			}

			switch(Type)
			{
				case BUTTON_KIND:

					if(!Node->NoKey && !Node->Special.Button.KeyStroke && (Node->Special.Button.Lines || Node->Special.Button.ReturnKey))
					{
						LTP_ReplaceLabelShortcut(Handle,Node);
					}

					break;

				case BOX_KIND:

					if(Node->Special.Box.ReserveSpace)
					{
						LONG	Size;
						STRPTR	Buffer;

						if(!Node->Special.Box.Lines)
						{
							if(Node->Special.Box.Lines = LTP_Alloc(Handle,sizeof(STRPTR) * Node->Lines))
							{
								LONG i;

								for(i = 0 ; i < Node->Lines ; i++)
									Node->Special.Box.Lines[i] = "";
							}
						}

						if(!Node->Chars)
							Size = 10;
						else
							Size = Node->Chars;

						Node->Special.Box.MaxSize = Size;

						if(Buffer = (STRPTR)LTP_Alloc(Handle,(Size + 1) * Node->Lines))
						{
							LONG i,Len;

							for(i = 0 ; i < Node->Lines ; i++)
							{
								if(Node->Special.Box.Lines[i])
								{
									Len = strlen(Node->Special.Box.Lines[i]);

									if(Len > Size)
										Len = Size;

									CopyMem(Node->Special.Box.Lines[i],Buffer,Len);

									Buffer[Len] = 0;
								}
								else
									Buffer[0] = 0;

								Node->Special.Box.Lines[i] = Buffer;

								Buffer += Size + 1;
							}
						}
					}

					break;

				case XBAR_KIND:
				case YBAR_KIND:

					Node->Special.Bar.Parent = Handle->CurrentGroup;
					break;

				#ifdef DO_PASSWORD_KIND
				{
					case PASSWORD_KIND:
					{
						STRPTR Buffer;

						if(Buffer = LTP_Alloc(Handle,2 * (Node->Special.String.MaxChars + 1)))
						{
							Node->Special.String.RealString	= Buffer;
							Node->Special.String.Original	= Buffer + Node->Special.String.MaxChars + 1;

							if(Node->Special.String.String)
							{
								strcpy(Node->Special.String.RealString,Node->Special.String.String);
								strcpy(Node->Special.String.Original,Node->Special.String.String);
							}
						}

						break;
					}
				}
				#endif

				case FRACTION_KIND:

					Node->Special.String.RealString = LTP_Alloc(Handle,Node->Special.String.MaxChars + 1);
					break;
			}

			if(ParentList)
				AddTail((struct List *)ParentList,(struct Node *)Node);
		}
	}
}


/*****************************************************************************/


/****** gtlayout.library/LT_NewA ******************************************
*
*   NAME
*	LT_NewA -- Add a new object to the user interface tree.
*
*   SYNOPSIS
*	LT_NewA(Handle,Tags);
*	         A0    A1
*
*	VOID LT_NewA(LayoutHandle *,struct TagItem *);
*
*	LT_New(Handle,...);
*
*	VOID LT_New(LayoutHandle *,...);
*
*   FUNCTION
*	LT_NewA() is the routine you use to build the user interface,
*	you give layout directions, design groups, etc. and finally
*	call LT_BuildA() to turn these specifications into a window.
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure.
*
*
*	This routine will accept almost all create-time tag items
*	gadtools.library/CreateGadget will handle. In addition to
*	this there are a bunch of extra gadget objects and extra
*	tag items supported:
*
*	All object types:
*
*	    LA_Type (LONG) - Type of the object to create, must be
*	        one of the following:
*
*	            TEXT_KIND
*	            VERTICAL_KIND
*	            HORIZONTAL_KIND
*	            END_KIND
*	            FRAME_KIND
*	            BOX_KIND
*	            SLIDER_KIND
*	            LISTVIEW_KIND
*	            INTEGER_KIND
*	            STRING_KIND
*	            PASSWORD_KIND
*	            PALETTE_KIND
*	            BUTTON_KIND
*	            CHECKBOX_KIND
*	            NUMBER_KIND
*	            GAUGE_KIND
*	            CYCLE_KIND
*	            POPUP_KIND
*	            TAB_KIND
*	            MX_KIND
*	            XBAR_KIND
*	            YBAR_KIND
*	            TAPEDECK_KIND
*	            LEVEL_KIND
*	            BOOPSI_KIND
*	            BLANK_KIND (V36)
*	            FRACTION_KIND (V38)
*	            IMAGE_KIND (V41)
*
*	    LA_LabelText (STRPTR) - The object label text to use.
*
*	    LA_LabelID (LONG) - The locale text ID of the string to
*	        use as the object label text.
*
*	            NOTE: LT_NewA() will fail if you forget
*	                to select a hook with LAHN_LocaleHook
*	                at LT_CreateHandleTagList.
*
*	    LA_ID (LONG) - The gadget ID to use for this object.
*
*	            NOTE: User ID values *MUST* be greater than 0,
*	                  negative values are reserved for internal
*	                  use.
*
*	    LA_Chars (LONG) - The width of this object measured in
*	        characters. If each character of the user interface
*	        font is 8 pixels wide an object with LA_Chars set to
*	        10 will usually be 80 pixels wide.
*
*	    LA_LabelChars (LONG) - This forces the internal gadget
*	        label width the layout engine calculates during the
*	        layout pass to a specific value. Note: this does
*	        not work well with all objects. (V9)
*
*	    LA_LabelPlace (LONG) - Where to place the gadget label.
*	        Not all objects will support all label positions:
*
*	            PLACE_LEFT - Place label text left of object
*	            PLACE_RIGHT - Place label text right of object
*	            PLACE_ABOVE - Place label text above object
*	            PLACE_IN - Place label text in object
*	            PLACE_BELOW - Place label text below object
*
*	    LA_ExtraSpace (BOOL) - Add extra vertical/horizontal
*	        space before this object.
*	        Default: FALSE
*
*	    LA_ExtraFat (BOOL) - Make this object a bit larger
*	        than its usual size.
*	        Default: FALSE
*
*	    LA_NoKey (BOOL) - Don't let the user interface choose
*	        a keyboard shortcut for this object.
*	        Default: FALSE
*
*	    LA_HighLabel (BOOL) - Use highlight pen when rendering
*	        the gadget label text.
*	        Default: FALSE
*
*	    LA_BYTE (BYTE *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_UBYTE (UBYTE *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_WORD (WORD *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_UWORD (UWORD *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_BOOL (BOOL *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_LONG (LONG *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_ULONG (ULONG *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_STRPTR (STRPTR) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*	    LA_FRACTION (FRACTION *) - Pointer to the variable that holds
*	        the "current value" of the object. The layout engine
*	        will retrieve this value initially during the creation
*	        of the object and maintain it during its lifespan.
*	        This means you do not need to worry about checking
*	        the state of the object, the user interface layout
*	        engine will do it for you.
*
*
*	    BLANK_KIND (transparent placeholder):
*
*	        (requires gtlayout.library V36 or higher)
*
*	        No tags are defined for this type of object.
*
*
*	    BOOPSI_KIND:
*
*	        (requires gtlayout.library V10 or higher)
*
*	        LABO_TagCurrent (Tag) - The Tag ID that represents the
*	            current object value. For PROPGCLASS this would be
*	            PGA_Top.
*
*	        LABO_TagTextAttr (Tag) - The Tag ID that represents the
*	            TextAttr value the object expects. For gadgets this
*	            would be GA_TextAttr.
*
*	        LABO_TagDrawInfo (Tag) - The Tag ID that represents the
*	            DrawInfo value the object expects.
*
*	        LABO_TagLink (Tag) - The Tag ID that represents a pointer
*	            to a different object the object expects. For the
*	            colorwheel.gadget this would be WHEEL_GradientSlider.
*
*	        LABO_TagScreen (Tag) - The Tag ID that represents the
*	            screen the object expects. For the colorwheel.gadget
*	            this would be WHEEL_Screen.
*
*	        LABO_Link (LONG) - The ID of the object this object should
*	            be linked to. This will be resolved later when gadgets
*	            are created.
*
*	                NOTE: Forward references are not resolved, only
*	                    backward references are allowed. This means that
*	                    if you wish to link object A to object B,
*	                    object A must be created before object B
*	                    is created.
*
*	        LABO_ClassInstance (Class *) - This is the first parameter
*	            you would pass to NewObjectA().
*
*	                NOTE: Only classes derived from gadgetclass and
*	                  gadgetclass itself may be used.
*
*	        LABO_ClassName (STRPTR) - This is the second parameter you would
*	            pass to NewObject().
*
*	                NOTE: Only classes derived from gadgetclass and
*	                    gadgetclass itself may be used.
*
*	        LABO_ClassLibraryName (STRPTR) - This tag is particularly useful
*	            for gadget class implementations wrapped into libraries, such as
*	            colorwheel.gadget. When opened, they make the classes they
*	            represent publicly available so subsequent calls to
*	            NewObject(NULL,<Classname>,...) can be made. The
*	            LABO_ClassLibraryName tag will cause gtlayout.library to open
*	            the class library before any calls to NewObject() are made.
*
*	                NOTE: Only classes derived from gadgetclass and
*	                    gadgetclass itself may be used.
*
*	                    The LABO_ClassLibraryName tag requires that you
*	                    specify the class name with the LABO_ClassName. It is
*	                    not enough to just use the LABO_ClassLibraryName tag.
*
*	        LABO_ExactWidth (WORD) - This is the exact width of the object to
*	            use. This effectively overrides whatever you specified using
*	            the LA_Chars tag and keeps gtlayout.library from shrinking and
*	            expanding the object as needed.
*
*	        LABO_ExactHeight (WORD) - This is the exact height of the object to
*	            use. This effectively overrides whatever you specified using
*	            the LA_Lines tag and keeps gtlayout.library from shrinking and
*	            expanding the object as needed.
*
*	        LABO_RelFontHeight (WORD) - This tag affects the height of the
*	            object; when specified, it is derived from the user interface
*	            font height plus the value given with LABO_RelFontHeight.
*
*	        LABO_FullWidth (BOOL) - Use this tag if you wish the object to
*	            cover the entire width of the group it resides within.
*
*	        LABO_FullHeight (BOOL) - Use this tag if you wish the object to
*	            cover the entire height of the group it resides within.
*
*	        LABO_ActivateHook (struct Hook *) - Hook to invoke when the
*	            layout engine decides that this particular object should
*	            be activated. The hook is called with the following
*	            parameters:
*
*	            Success = ActivateFunc(struct Hook *Hook,LayoutHandle *Handle,
*	              D0                                 A0                  A2
*
*	                         Object *object)
*	                                   A1
*
*	            The object pointer actually refers to the instance of the
*	            BOOPSI object created. Return FALSE if your object could not
*	            be activated, TRUE if it worked. If you return TRUE, no special
*	            keyboard event will be generated. (V13)
*
*	        NOTE: All tags passed to LT_New() for BOOPSI_KIND objects are
*	            passed through to NewObjectA() later. The library makes
*	            a copy of the tag item list, so all data valid in the
*	            scope when LT_New() is called must also be valid later
*	            when LT_Build() is invoked.
*
*	            The gadget label is *NOT* passed through to the object,
*	            it effectively receives the label a plain gadtools object
*	            would receive, similar to what happens to FRAME_KIND
*	            objects and the like.
*
*
*	    BOX_KIND (multiline text display):
*
*	        LABX_Chars (LONG) - The width of this object in
*	            characters. The layout routine will try to make
*	            sure that the given number of characters will
*	            fit into a single line of text in this box.
*	            This may be a problem with proportional spaced
*	            fonts.
*               Default: 10
*
*	        LABX_Labels (STRPTR *) - The label texts to display
*	            on the right hand side of the box. Terminate
*	            this array with NULL.
*
*	        LABX_Lines (STRPTR *) - The text to display in the
*	            box. Terminate this array with NULL.
*
*	        LABX_Rows (LONG) - The height of this object in
*	            characters.
*
*	        LABX_AlignText (LONG) - Controls how text is aligned
*	            in box lines:
*
*	                ALIGNTEXT_LEFT - Align text to the left edge
*	                ALIGNTEXT_CENTERED - Centre the text
*	                ALIGNTEXT_RIGHT - Align text to the right edge
*	                ALIGNTEXT_PAD - Pad text lines
*
*	            Default: ALIGNTEXT_LEFT
*
*	        LABX_DrawBox (BOOL) - Draw a recessed bevel box
*	            around the text box.
*	            Default: FALSE
*
*	        LABX_FirstLabel (LONG) - Locale string ID of the first
*	            text to use as a box label. Works in conjunction
*	            with LABX_LastLabel.
*
*	        LABX_LastLabel (LONG) - Locale string ID of the last
*	            text to use as a box label. Works in conjunction
*	            with LABX_FirstLabel. When building the interface the
*	            code will loop from FirstLabel..LastLabel, look
*	            up the corresponding locale strings and use the
*	            data to make up the label text to appear at the
*	            right hand side of the box.
*
*	        LABX_LabelTable (LONG *) - Pointer to an array of IDs
*	            to use for building the box labels. This requires
*	            that a locale hook is provided with the layout handle.
*	            The array is terminated by -1.
*
*	        LABX_ReserveSpace (BOOL) - Allocate extra memory to hold
*	            the contents of the lines displayed. This avoids nasty
*	            side-effects when refreshing this object.
*	            Default: FALSE
*
*	        LABX_FirstLine (LONG) - Locale string ID of the first
*	            text to print inside the box. Works in conjunction
*	            with LABX_LastLine. (V26)
*
*	        LABX_LastLine (LONG) - Locale string ID of the last
*	            text to print inside the box. Works in conjunction
*	            with LABX_FirstLine. (V26)
*
*	        LABX_LineTable (LONG *) - Pointer to an array of IDs
*	            to use for building the box contents lines. This requires
*	            that a locale hook is provided with the layout handle.
*	            The array is terminated by -1. (V28)
*
*	        LABX_Line (STRPTR) - Line to display in the box, may
*	            contain '\n' line break characters, the layout engine
*	            will chop the single line into single consecutive lines
*	            following the '\n' chars. (V31)
*
*	        LABX_LineID (LONG) - Locale ID of line text to display in the
*	            box, may contain '\n' line break characters, the layout
*	            engine will chop the single line into single consecutive lines
*	            following the '\n' chars. (V31)
*
*	        LABX_TextPen (WORD) - Box text colour to use. (V40)
*
*	        LABX_BackPen (WORD) - Box text background colour to use. (V40)
*
*	        LABX_Spacing (UWORD) - Request that additional pixels should
*	            separate lines of text. (V41)
*	            Default: 0
*
*
*	    BUTTON_KIND:
*
*	        LA_Label (STRPTR)
*	        LA_LabelID (LONG) - These two define the button label, i.e.
*	            the text that is printed within the button box. Optionally,
*	            this text may include newline characters ("\n") which will
*	            cause the button text to be broken into several lines.
*	            This particular feature requires gtlayout.library v12 or
*	            higher. Single line label have always been supported.
*
*	        LABT_ReturnKey (BOOL) - Let the user operate this
*	            button by pressing the return key, making it the
*	            so-called default button, or default choice. The
*	            button select box will appear slightly bolder than
*	            normal buttons are.
*
*	                NOTE: there can be only one single button per
*	                    window to sport this feature.
*
*	            Default: FALSE
*
*	        LABT_EscKey (BOOL) - Let the user operate this
*	            button by pressing the Escape key.
*
*	                NOTE: there can be only one single button per
*	                    window to use this feature.
*
*	            Default: FALSE
*
*	        LABT_ExtraFat (BOOL) - Make this button a bit
*	            larger than usual.
*	            Default: FALSE
*
*	        LABT_Lines (STRPTR *) - Use the given string array
*	            to create a multiline gadget label. Terminate the
*	            array with a NULL. (V12)
*
*	        LABT_FirstLine (LONG) - Locale ID of first label line. (V12)
*
*	        LABT_LastLine (LONG) - Locale ID of last label line. (V12)
*
*	        LABT_DefaultCorrection (BOOL) - Make the button slightly
*	            wider and taller so its size matches the default
*	            button. (V21)
*
*	        LABT_Smaller (BOOL) - Make this button a little smaller
*	            than usual. (V21)
*
*
*	    CYCLE_KIND:
*
*	        LACY_FirstLabel (LONG) - Locale string ID of the first
*	            text to use as a label. Works in conjunction
*	            with LACY_LastLabel.
*
*	        LACY_LastLabel (LONG) - Locale string ID of the last
*	            text to use as a label. Works in conjunction
*	            with LACY_FirstLabel. When building the interface the
*	            code will loop from FirstLabel..LastLabel, look
*	            up the corresponding locale strings and use the
*	            data to make up the label text.
*
*	        LACY_LabelTable (LONG *) - Pointer to an array of IDs
*	            to use for building the cycle labels. This requires
*	            that a locale hook is provided with the layout handle.
*	            The array is terminated by -1.
*
*	        LACY_TabKey (BOOL) - Connect this object to the tabulator
*	            key. Press [Tab] to cycle to the next entry, [Shift][Tab]
*	            to cycle to the previous entry. (V9)
*
*	                NOTE: there can be only one single button per
*	                    window to use this feature.
*
*	            Default: FALSE
*
*	        LACY_AutoPageID (LONG) - ID of paged GROUP_KIND object
*	            which will be set to the gadget's current setting.
*	            If this tag is set, you will hear no events from this
*	            object any more. (V7)
*
*	                NOTE: Listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*
*	    FRACTION_KIND:
*
*	        (requires gtlayout.library V38 or higher)
*
*	        This is a special kind of STRING_KIND which comes with a
*	        special input filter. It will allow only for floating point
*	        numbers to be entered, i.e. numbers (`0'-`9') and one
*	        decimal point (`.' or whatever your current locale uses
*	        as the decimal point character). What you will receive as
*	        user input text will be in the following format:
*
*	            (0|#[0-9]).(0|#[0-9])
*
*	        Or in other words, text like this will be returned:
*
*	            0.0
*	            7.0
*	            0.9
*	            146654.0
*
*	        NOTE: While this object type did exist in earlier library
*	              versions support for it was removed in V33. It was
*	              eventually reintroduced with the functionality described
*	              above in V38.
*
*	        LAFR_IncrementerHook (struct Hook *) - Hook to invoke when the
*	            incrementer arrows are used. The hook function will receive
*	            three parameters and has to return a result code:
*
*	                a0 - (struct Hook *)  Pointer to this hook
*	                a2 - (STRPTR) Current value of this object
*	                a1 - (LONG) Either INCREMENTERMSG_Decrement or
*	                            INCREMENTERMSG_Increment
*
*	            If the hook function modifies the current value it must
*	            return TRUE, otherwise FALSE must be returned. If you return
*	            TRUE, then the library will update the object. (V40)
*
*	        This object type accepts all the GTST_#? tag items.
*
*
*	    FRAME_KIND (fixed size general purpose display,
*	        you may render into it):
*
*	        LAFR_InnerWidth (LONG) - Inner width of the
*	            display box.
*
*	        LAFR_InnerHeight (LONG) - Inner height of the
*	            display box.
*
*	        LAFR_DrawBox (BOOL) - Draw a recessed bevel box
*	            around the display box.
*	            Default: FALSE
*
*	        LAFR_RefreshHook (struct Hook *) - Hook to call
*	            when refreshing/redrawing this object. See
*	            gtlayout.h for more information. (V9)
*
*	        LAFR_GenerateEvents (BOOL) - If TRUE, clicking
*	            inside the FRAME_KIND object will generate
*	            IDCMP_GADGETUP/IDCMP_GADGETDOWN events. If
*	            you wish to know where the click occured,
*	            make a copy of the Window->MouseX/Y entries
*	            before you call LT_HandleInput()/LT_GetIMsg(). (V28)
*	            Default: FALSE
*
*	        LAFR_ResizeX (BOOL) - Makes this frame resizable
*	            in the horizontal direction, attaches a sizing
*	            gadget to the window to open and handles window
*	            resize operations automatically. (V44)
*
*	                NOTE: there can be only one single object
*	                    per window to sport this feature. This
*	                    means that you won't be able to have
*	                    both a resizable listview and a resizable
*	                    frame in the same window.
*
*	                    Also listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*	            Default: FALSE
*
*	        LAFR_ResizeY (BOOL) - Makes this frame resizable
*	            in the vertical direction, attaches a sizing
*	            gadget to the window to open and handles window
*	            resize operations automatically. (V44)
*
*	                NOTE: there can be only one single object
*	                    per window to sport this feature. This
*	                    means that you won't be able to have
*	                    both a resizable listview and a resizable
*	                    frame in the same window.
*
*	                    Also listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*	            Default: FALSE
*
*	    GAUGE_KIND (general purpose progress report display):
*
*	        LAGA_Percent (LONG) - Indicator position, can range
*	            from 0..100.
*	            Default: 0
*
*	        LAGA_InfoLength (LONG) - Maximum number of characters
*	            to reserve for text printed in the gauge display.
*	            Default: 0
*
*	        LAGA_InfoText (STRPTR) - Text to print in the gauge
*	            display.
*
*	        LAGA_Tenth (BOOL) - Instead of a continuously growing
*	            bar you will get a set of exactly ten blocks,
*	            each separated by a hairline. (V19)
*	            Default: FALSE
*
*               LAGA_NoTicks (BOOL) - If TRUE suppresses drawing the
*                   ticks below the gauge box.
*                   Default: FALSE
*
*
*	    IMAGE_KIND (display area for Image or BitMap data):
*
*	        (requires gtlayout.library V41 or higher)
*
*	        LAIM_Image (struct Image *) - Image to draw in this
*	            place. When the user interface is built, it will
*	            be drawn using intuition.library/DrawImage().
*
*	        LAIM_BitMap (struct BitMap *) - You can supply a
*	            BitMap instead of an Image which will be drawn
*	            using either graphics.library/BltBitMapRastPort()
*	            or graphics.library/BltMaskBitMapRastPort() if
*	            a mask is provided.
*
*	        LAIM_BitMapLeft (UWORD) - Left corner of the image data
*	            to blit into the window (the second parameter for
*	            BltBitMapRastPort()).
*
*	        LAIM_BitMapTop (UWORD) - Top corner of the image data
*	            to blit into the window (the third parameter for
*	            BltBitMapRastPort()).
*
*	        LAIM_BitMapWidth (UWORD) - Width of the bitmap area to
*	            blit into the window.
*
*	        LAIM_BitMapHeight (UWORD) - Height of the bitmap area to
*	            blit into the window.
*
*	        LAIM_BitMapMask (PLANEPTR) - Pointer to a bit plane mask
*	            to blit the image bitmap through. Must follow the
*	            rules documented for for BltMaskBitMapRastPort().
*	            The blit operation will use the minterm for
*	            transparency.
*
*
*	    INTEGER_KIND:
*
*	        LAIN_LastGadget (BOOL) - Pressing return with this
*	            gadget active will stop activating the next
*	            following string gadget type if TRUE is passed.
*	            Default: FALSE
*
*	        LAIN_Min (LONG) - Minimum accepted numeric value.
*	            Default: -2147483647
*
*	        LAIN_Max (LONG) - Maximum accepted numeric value.
*	            Default:  2147483647
*
*	        LAIN_UseIncrementers (BOOL) - Use TRUE to add incrementer
*	            arrow buttons to the right of the numeric entry field.
*	            These buttons will let you cycle through a set of
*	            numbers to be displayed in the numeric entry field.
*	            Default: FALSE
*
*	        LAIN_HistoryLines (LONG) - Number of numbers entered to
*	            keep as a backlog.
*	            Default: 0
*
*	        LAIN_HistoryHook (struct Hook *) - Hook code to call when
*	            entering a number into the backlog. See gtlayout.h for
*	            more information.
*	            Default: NULL
*
*	        LAIN_IncrementerHook (struct Hook *) - Hook code to call
*	            when cycling through numeric values. See gtlayout.h for
*	            more information.
*	            Default: NULL
*
*	        LAIN_Activate (BOOL) - When the window opens, make this
*	            gadget the active one. (V21)
*
*	                NOTE: There can be only one gadget of this type
*	                    per window.
*
*	            Default: FALSE
*
*
*	    LEVEL_KIND:
*
*	        All tags are supported which SLIDER_KIND supports.
*	        The gadget level display however, can only be aligned
*	        to the left border.
*
*	        LAVL_Freedom (WORD) - Selects the orientation of the
*	            slider body; can be either FREEHORIZ or FREEVERT. (V41)
*
*	            Default: FREEHORIZ
*
*	        LAVL_Ticks (WORD) - Selects if and where to place tick
*	            marks next to the slider body. Can be one of the
*	            the following: TICKS_None (no ticks), TICKS_Left
*	            (place ticks left of the body), TICKS_Above (place
*	            ticks above the body), TICKS_Both (place ticks on
*	            both sides of the body). (V41)
*
*	            Default: TICKS_None
*
*	        LAVL_NumTicks (LONG) - Number of tick marks to draw. (V41)
*
*	        LAVL_Lines (LONG) - Height of the slider, if FREEVERT. (V41)
*
*
*	    LISTVIEW_KIND:
*
*	        LALV_ExtraLabels (STRPTR *) - Place extra line
*	            labels at the right of the box. Terminate
*	            this array with NULL.
*
*	        LALV_Labels (STRPTR *) - The labels to display
*	            inside the box, you can pass this array of
*	            strings in rather than passing an initialized
*	            List of text via GTLV_Labels. Terminate
*	            this array with NULL.
*
*	        LALV_CursorKey (BOOL) - Let the user operate this
*	            listview using the cursor keys.
*
*	                NOTE: there can be only one single listview
*	                    per window to sport this feature.
*
*	            Default: FALSE
*
*	        LALV_Lines (LONG) - The number of text lines this
*	            listview is to display.
*
*	        LALV_Link (LONG) - The Gadget ID of a string gadget
*	            to attach to this listview.
*
*	                NOTE: you need to
*	                    add the Gadget in question before you add the
*	                    listview to refer to it or the layout routine
*	                    will get confused.
*
*	            Passing the value NIL_LINK will create a listview
*	            which displays the currently selected item, otherwise
*	            you will get a read-only list.
*
*	        LALV_FirstLabel (LONG) - Locale string ID of the first
*	            text to use as a list label. Works in conjunction
*	            with LALV_LastLabel.
*
*	        LALV_LastLabel (LONG) - Locale string ID of the last
*	            text to use as a list label. Works in conjunction
*	            with LALV_FirstLabel. When building the interface the
*	            code will loop from FirstLabel..LastLabel, look
*	            up the corresponding locale strings and use the
*	            data to make up the label text to appear in the
*	            list.
*
*	        LALV_LabelTable (LONG *) - Pointer to an array of IDs
*	            to use for building the listview contents. This requires
*	            that a locale hook is provided with the layout handle.
*	            The array is terminated by -1.
*
*	        LALV_MaxGrowX (LONG) - Maximum width of this object
*	            measured in characters. When the first layout pass
*	            is finished and there is still enough space left
*	            to make the listview wider, the width is increased
*	            until it hits the limit specified using this tag.
*
*	                NOTE: there can be only one single listview
*	                    per window to sport this feature.
*
*	            Default: 0
*
*	        LALV_MaxGrowY (LONG) - Maximum height of this object
*	            measured in lines. When the first layout pass is
*	            finished and there is still enough space left to
*	            make the listview higher, the height is increased
*	            until it hits the limit specified using this tag.
*
*	                NOTE: there can be only one single listview
*	                    per window to sport this feature.
*
*	            Default: 0
*
*	        LALV_ResizeX (BOOL) - Makes this listview resizable
*	            in the horizontal direction, attaches a sizing
*	            gadget to the window to open and handles window
*	            resize operations automatically. (V9)
*
*	                NOTE: there can be only one single object
*	                    per window to sport this feature. This
*	                    means that you won't be able to have
*	                    both a resizable listview and a resizable
*	                    frame in the same window.
*
*	                    Also listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*	            Default: FALSE
*
*	        LALV_ResizeY (BOOL) - Makes this listview resizable
*	            in the vertical direction, attaches a sizing
*	            gadget to the window to open and handles window
*	            resize operations automatically. (V9)
*
*	                NOTE: there can be only one single object
*	                    per window to sport this feature. This
*	                    means that you won't be able to have
*	                    both a resizable listview and a resizable
*	                    frame in the same window.
*
*	                    Also listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*	            Default: FALSE
*
*	        LALV_MinChars (WORD) - Minimum width for this
*	            object, measured in characters. Used in
*	            conjunction with LALV_ResizeX. (V9)
*
*	        LALV_MinLines (WORD) - Minimum height for this
*	            object, measured in lines. Used in
*	            conjunction with LALV_ResizeY. (V9)
*
*	        LALV_LockSize (BOOL) - After doing the initial layout
*	            for this object, do not adapt its size again during
*	            subsequent layouts. This is particularly useful if
*	            you have a LISTVIEW_KIND object in a paged group
*	            and keep adding new entries to the list. You need
*	            to specify an object width using LA_Chars, otherwise
*	            the layout engine may make it not wide enough to
*	            display any entries. (V8)
*
*	        LALV_FlushLabelLeft (BOOL) - For a gadget label placed
*	            above the listview align the text to the left edge
*	            of the view. (V9)
*
*	        LALV_TextAttr (struct TextAttr *) - You can specify a
*	            fixed-width font to be used for the list display.
*	            The TextAttr (or TTextAttr) you provide must be ready
*	            to go so the layout code can open the font later.
*
*	            To get the current system default font, which is
*	            guaranteed to be fixed-width, pass ~0 instead of a
*	            pointer to a TextAttr structure. (V10)
*
*	                NOTE: The font *MUST* be fixed-width or the layout
*	                    will fail.
*
*	                     Choose your font in such a way that it matches
*	                     in width and height with the other probably
*	                     proportional-spaced user interface font.
*
*	                     If the layout engine decides to step down in
*	                     font size, all LISTVIEW_KIND objects which were
*	                     configured to use a special fixed-width font
*	                     will `forget' about it. This won't matter much
*	                     as the fonts the engine chooses will always be
*	                     fixed-width anyway.
*
*	        LALV_AutoPageID (LONG) - ID of paged GROUP_KIND object
*	            which will be set to the gadget's current setting.
*	            If this tag is set, you will hear no events from this
*	            object any more. (V23)
*
*	                NOTE: Listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*	        LALV_Selected (LONG) - In this context, this tag is an
*	            alias for GTLV_Selected. See
*	            gtlayout.library/LT_SetAttributes for more information
*	            on how the meaning of this tag differs from this (V34).
*
*
*	    MX_KIND:
*
*	        LAMX_FirstLabel (LONG) - Locale string ID of the first
*	            text to use as a label. Works in conjunction
*	            with LAMX_LastLabel.
*
*	        LAMX_LastLabel (LONG) - Locale string ID of the last
*	            text to use as a label. Works in conjunction
*	            with LAMX_FirstLabel. When building the interface the
*	            code will loop from FirstLabel..LastLabel, look
*	            up the corresponding locale strings and use the
*	            data to make up the label text.
*
*	        LAMX_LabelTable (LONG *) - Pointer to an array of IDs
*	            to use for building the radio labels. This requires
*	            that a locale hook is provided with the layout handle.
*	            The array is terminated by -1.
*
*	        LAMX_TabKey (BOOL) - Connect this object to the tabulator
*	            key. Press [Tab] to cycle to the next entry, [Shift][Tab]
*	            to cycle to the previous entry. (V9)
*
*	                NOTE: there can be only one single button per
*	                    window to use this feature.
*
*	            Default: FALSE
*
*	        LAMX_AutoPageID (LONG) - ID of paged GROUP_KIND object
*	            which will be set to the gadget's current setting.
*	            If this tag is set, you will hear no events from this
*	            object any more. (V7)
*
*	                NOTE: Listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*
*	    PALETTE_KIND:
*
*	        LAPA_SmallPalette (BOOL) - Make the palette display
*	            a bit smaller than usual.
*	            Default: FALSE
*
*	        LAPA_Lines (LONG) - Number of lines the palette
*	            display should cover.
*	            Default: No preference
*
*	        LAPA_UsePicker (BOOL) - This tag effectively changes the
*	            gadget type. Instead of a list of colours to pick from
*	            the user will see a rectangle filled in the selected
*	            colour with a picker button next to it. This gadget
*	            will generate IDCMP_IDCMPUPDATE events when the picker
*	            button is pressed. (V10)
*
*
*	    PASSWORD_KIND (string gadget type which does not
*	        display its contents):
*
*	        LAPW_LastGadget (BOOL) - Pressing return with this
*	            gadget active will stop activating the next
*	            following string gadget type if TRUE is passed.
*	            Default: FALSE
*
*	        LAPW_HistoryLines (LONG) - Number of lines to keep as
*	            a backlog.
*	            Default: 0
*
*	        LAPW_HistoryHook (struct Hook *) - Hook code to call
*	            when entering a line into the backlog. See gtlayout.h
*	            for more information.
*	            Default: NULL
*
*	        LAPW_Activate (BOOL) - When the window opens, make this
*	            gadget the active one. (V21)
*
*	                NOTE: There can be only one gadget of this type
*	                    per window.
*
*	            Default: FALSE
*
*	        This object type accepts all the GTST_#? tag items.
*
*
*	    POPUP_KIND:
*
*	        (This features requires gtlayout.library v22 or higher
*	         and Kickstart 3.0 or higher).
*
*	        LAPU_FirstLabel (LONG) - Locale string ID of the first
*	            text to use as a label. Works in conjunction
*	            with LAPU_LastLabel.
*
*	        LAPU_LastLabel (LONG) - Locale string ID of the last
*	            text to use as a label. Works in conjunction
*	            with LAPU_FirstLabel. When building the interface the
*	            code will loop from FirstLabel..LastLabel, look
*	            up the corresponding locale strings and use the
*	            data to make up the label text.
*
*	        LAPU_LabelTable (LONG *) - Pointer to an array of IDs
*	            to use for building the cycle labels. This requires
*	            that a locale hook is provided with the layout handle.
*	            The array is terminated by -1.
*
*	        LAPU_TabKey (BOOL) - Connect this object to the tabulator
*	            key. Press [Tab] to cycle to the next entry, [Shift][Tab]
*	            to cycle to the previous entry.
*
*	                NOTE: there can be only one single button per
*	                    window to use this feature.
*
*	            Default: FALSE
*
*	        LAPU_AutoPageID (LONG) - ID of paged GROUP_KIND object
*	            which will be set to the gadget's current setting.
*	            If this tag is set, you will hear no events from this
*	            object any more.
*
*	                NOTE: Listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*	        LAPU_CentreActive (BOOL) - If TRUE, the popup menu will
*	            appear with the currently active entry centred below
*	            the mouse pointer. (V31)
*
*	        LAPU_Labels (STRPTR *) - Pointer to NULL-terminated array
*	            of strings that are the choices offered.
*
*
*	    SCROLLER_KIND:
*
*	        LASC_Thin (BOOL) - Make the scroller a bit thinner
*	            than usual.
*	            Default: FALSE
*
*	        GA_RelVerify (BOOL) - Hear every IDCMP_GADGETUP
*	            event from scroller.
*	            Default: TRUE
*
*                        NOTE: This is different from what
*	                     gadtools.library uses.
*
*	        GA_Immediate (BOOL) - Hear every IDCMP_GADGETDOWN
*	            event from scroller
*	            Default: TRUE
*
*                        NOTE: This is different from what
*	                     gadtools.library uses.
*
*
*	    SLIDER_KIND:
*
*	        LASL_FullCheck: TRUE will cause the code to rattle
*	            through all possible slider settings, starting
*	            from the minimum value, ending at the maximum value.
*	            While this may be a good idea for a display
*	            function to map slider levels to text strings
*	            of varying length it might be a problem when
*	            it comes to display a range of numbers from
*	            1 to 40,000: the code will loop through
*	            40,000 iterations trying to find the longest
*	            string.
*
*	            FALSE will cause the code to calculate the
*	            longest level string based only on the
*	            minimum and the maximum value to check.
*	            While this is certainly a good a idea when
*	            it comes to display a range of numbers from
*	            1 to 40,000 as only two values will be
*	            checked the code may fail to produce
*	            accurate results for sliders using display
*	            functions mapping slider levels to strings.
*
*	            Default: TRUE
*
*
*	    STRING_KIND:
*
*	        LAST_LastGadget (BOOL) - Pressing return with this
*	            gadget active will stop activating the next
*	            following string gadget type if TRUE is passed.
*	            Default: FALSE
*
*	        LAST_Link (LONG) - Gadget ID of the listview to attach
*	            this string gadget to.
*
*	                NOTE: you need to add the string gadget before
*	                    you add the listview to refer to it or the
*	                    layout routine will get confused.
*
*	        LAST_Picker (BOOL) - Attach a `select' button to the
*	            right hand side of the string gadget.
*	            Default: FALSE
*
*	        LAST_HistoryLines (LONG) - Number of lines to keep as
*	            a backlog.
*	            Default: 0
*
*	        LAST_HistoryHook (struct Hook *) - Hook code to call
*	            when entering a line into the backlog. See gtlayout.h
*	            for more information.
*	            Default: NULL
*
*	        LAST_Activate (BOOL) - When the window opens, make this
*	            gadget the active one. (V21)
*
*	                NOTE: There can be only one gadget of this type
*	                    per window.
*
*	            Default: FALSE
*
*
*	    TAB_KIND:
*
*	        (This features requires gtlayout.library v24 or higher)
*
*	        LATB_FirstLabel (LONG) - Locale string ID of the first
*	            text to use as a label. Works in conjunction
*	            with LATB_LastLabel.
*
*	        LATB_LastLabel (LONG) - Locale string ID of the last
*	            text to use as a label. Works in conjunction
*	            with LATB_FirstLabel. When building the interface the
*	            code will loop from FirstLabel..LastLabel, look
*	            up the corresponding locale strings and use the
*	            data to make up the label text.
*
*	        LATB_LabelTable (LONG *) - Pointer to an array of IDs
*	            to use for building the cycle labels. This requires
*	            that a locale hook is provided with the layout handle.
*	            The array is terminated by -1.
*
*	        LATB_TabKey (BOOL) - Connect this object to the tabulator
*	            key. Press [Tab] to cycle to the next entry, [Shift][Tab]
*	            to cycle to the previous entry.
*
*	                NOTE: there can be only one single button per
*	                    window to use this feature.
*
*	            Default: FALSE
*
*	        LATB_AutoPageID (LONG) - ID of paged GROUP_KIND object
*	            which will be set to the gadget's current setting.
*	            If this tag is set, you will hear no events from this
*	            object any more.
*
*	                NOTE: Listen to IDCMP_CLOSEWINDOW events
*	                    which may be generated if the layout
*	                    engine runs out of memory when rebuilding
*	                    the user interface.
*
*	        LATB_FullSize (BOOL) - By default a TAB_KIND object
*	            covers the entire width of the group it sits in.
*	            With LATB_FullSize set to true it will cover the
*	            width of the entire Window.
*
*	            Default: FALSE
*
*	        LATB_Labels (STRPTR *) - Pointer to NULL-terminated array
*	            of strings that are the choices offered.
*
*
*	    TAPEDECK_KIND:
*
*	        LATD_ButtonType (LONG) - Select the image to display
*	            in the button, must be one of the following:
*
*	                TDBT_BACKWARD
*	                    "<<" Symbol
*
*	                TDBT_FORWARD
*	                    ">>" Symbol
*
*	                TDBT_PREVIOUS
*	                    "|<" Symbol
*
*	                TDBT_NEXT
*	                    ">|" Symbol
*
*	                TDBT_STOP
*	                    Stop symbol (filled square)
*
*	                TDBT_PAUSE
*	                    "||" pause symbol (broken square)
*
*	                TDBT_RECORD
*	                    Record symbol (filled circle)
*
*	                TDBT_REWIND
*	                    "<" symbol
*
*	                TDBT_EJECT
*	                    Eject symbol (broken upward pointing arrow)
*
*	                TDBT_PLAY
*	                    ">" symbol
*
*	        LATD_Toggle (BOOL) - Make this object a toggle-select
*	            button.
*	            Default: FALSE
*
*	        LATD_Pressed (BOOL) - Make this button appear to be
*	            pressed.
*
*	                NOTE: requires "LATD_Toggle,TRUE" attribute.
*
*	            Default: FALSE
*
*	        LATD_Smaller (BOOL) - Make this button a bit smaller
*	            than usual.
*	            Default: FALSE
*
*	        LATD_Tick (BOOL) - Hear IDCMP_GADGETUP events while the
*	            buttons is being pressed; the IntuiMessage->Code entry
*	            will be 0 while the button is being pressed, and
*	            will be 1 as soon as the button is released. (V12)
*
*	            Default: FALSE
*
*
*	    TEXT_KIND:
*
*	        LATX_Picker (BOOL) - Attach a `select' button to the
*	            right hand side of the text display.
*	            Default: FALSE
*
*	        LATX_LockSize (BOOL) - After doing the initial layout
*	            for this object, do not adapt its size again during
*	            subsequent layouts. This is particularly useful if
*	            you have a TEXT_KIND object in a paged group
*	            and update its contents later. You need
*	            to specify an object width using LA_Chars, otherwise
*	            the layout engine may make it not wide enough to
*	            display any text. (V15)
*
*
*	    VERTICAL_KIND (group to align objects vertically):
*	    HORIZONTAL_KIND (group to align objects horizontally):
*
*	        LAGR_Spread (BOOL) - Place all objects in this
*	            group with roughly the same amount of space
*	            between them.
*	            Default: FALSE
*
*	        LAGR_SameSize (BOOL) - Make all objects in this
*	            group the same size (for vertical groups:
*	            the same height, for horizontal groups:
*	            the same width).
*	            Default: FALSE
*
*	        LAGR_LastAttributes (BOOL) - Try to copy the
*	            size of the previous group for this new
*	            group. May not work if this group turns
*	            out to be larger than the previous group.
*	            Default: FALSE
*
*	        LAGR_ActivePage (LONG) - Organize all child
*	            groups as pages which can be flipped through
*	            using LT_SetAttributes(). You need to
*	            specify the number of the first page to
*	            display, starting from 0.
*
*	                NOTE: Specifying this tag actually enables
*	                    the paging feature. If you omit this
*	                    tag calls to flip to a specific
*	                    page will fail.
*
*	            Default: No paging
*
*	        LAGR_Frame (BOOL) - Draw a recessed frame around
*	            this group, even if there is no group label. (V7)
*
*	        LAGR_IndentX (BOOL) - Add extra horizontal indentation
*	            for this group. (V10)
*
*	        LAGR_IndentY (BOOL) - Add extra vertical indentation
*	            for this group. (V10)
*
*	        LAGR_NoIndent (BOOL) - Inhibit automatic size adjustion
*	            and centring of this group if it is smaller than
*	            the neighbouring groups. (V21)
*
*	        LAGR_SameWidth (WORD) - During the final layout pass,
*	            make this group the same width as the group which
*	            uses the given ID. Not implemented yet. (V25)
*
*	        LAGR_SameHeight (WORD) - During the final layout pass,
*	            make this group the same height as the group which
*	            uses the given ID. Not implemented yet. (V25)
*
*	        LAGR_FrameGroup (UWORD) - Surround this group with a
*	            frame. Must be one of FRAMETYPE_Label or FRAMETYPE_Tab.
*	            FRAMETYPE_Label works like "LAGR_Frame,TRUE," while
*	            FRAMETYPE_Tab is for use with a TAB_KIND object you
*	            must put directly on top of this group or the group
*	            will look a little silly. (V38)
*
*	            Default: FRAMETYPE_None
*
*
*	    XBAR_KIND (horizontal separator bar):
*
*	        LAXB_FullSize (BOOL) - Make this separator bar span
*	            the entire window width.
*
*
*	    YBAR_KIND (vertical separator bar):
*
*	        No tags are defined for this type of object.
*
*
*   RESULT
*	none
*
*   BUGS
*	Up to v25 the SCROLLER_KIND object did not support GA_Immediate
*	or GA_RelVerify. The space for the variables was there, but the
*	code was missing.
*
*	POPUP_KIND objects don't work well in simple refresh windows,
*	as refresh events can get lost. Use a smart refresh window
*	instead or call gadtools.library/LT_CatchUpRefresh() regularly.
*
*   SEE ALSO
*	gadtools.library/CreateGadgetA
*
******************************************************************************
*
*/

VOID LIBENT
LT_NewA(REG(a0) LayoutHandle *handle,REG(a1) struct TagItem *tagList)
{
	if(handle != NULL)
	{
		struct TagItem *List,*Entry;
		STRPTR label;
		LONG type;
		LONG id;

		type	= -1;
		label	= NULL;
		id		= 0;
		List	= tagList;

		while((Entry = NextTagItem(&List)) != NULL)
		{
			switch(Entry->ti_Tag)
			{
				case LA_Type:

					type = Entry->ti_Data;
					break;

				case LA_ID:

					id = Entry->ti_Data;
					break;

				case LA_LabelID:

					if(handle->LocaleHook != NULL)
					{
						label = (STRPTR)CallHookPkt(handle->LocaleHook,handle,(APTR)Entry->ti_Data);
					}
					else
					{
						handle->Failed = TRUE;

						return;
					}

					break;

				case LA_LabelText:

					label = (STRPTR)Entry->ti_Data;
					break;
			}
		}

		LT_AddA(handle,type,label,id,tagList);
	}
}


/*****************************************************************************/


/****** gtlayout.library/LT_EndGroup ******************************************
*
*   NAME
*	LT_EndGroup -- end a group declaration.
*
*   SYNOPSIS
*	LT_EndGroup(Handle);
*	             A0
*
*	VOID LT_EndGroup(LayoutHandle *);
*
*   FUNCTION
*      This is just a short form of
*
*	       LT_New(Handle,
*	           LA_Type, END_KIND,
*	       TAG_DONE);
*
*      It helps to save (some) space.
*
*   INPUTS
*        Handle - Pointer to LayoutHandle.
*
*   RESULT
*	none
*
*   SEE ALSO
*	gtlayout.library/LT_New
*
******************************************************************************
*
*/

VOID LIBENT
LT_EndGroup(REG(a0) LayoutHandle *handle)
{
	if(handle != NULL && NOT handle->Failed)
	{
		/* new in 45.1: fail if there is no group to be closed */
		if(handle->CurrentGroup == NULL)
		{
			handle->Failed = TRUE;
		}
		else
		{
			handle->CurrentGroup = handle->CurrentGroup->Special.Group.ParentGroup;
		}
	}
}
