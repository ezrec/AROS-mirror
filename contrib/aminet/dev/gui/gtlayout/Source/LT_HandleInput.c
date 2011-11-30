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

#include <clib/keymap_protos.h>
#include <pragmas/keymap_pragmas.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

STATIC LONG
CurrentWrap(ObjectNode *Node,LONG Current)
{
	if(Current > Node->Max)
		Current = Node->Min;
	else
	{
		if(Current < Node->Min)
			Current = Node->Max;
	}

	return(Current);
}

STATIC BOOL
CurrentInBounds(ObjectNode *Node,LONG Current)
{
	return((BOOL)(Node->Min <= Current && Current <= Node->Max && Current != Node->Current));
}

/****** gtlayout.library/LT_HandleInput ******************************************
*
*   NAME
*	LT_HandleInput -- Filter IntuiMessage data.
*
*   SYNOPSIS
*	LT_HandleInput(Handle,Qualifier,Class,Code,Gadget);
*	                 A0      D0      A1    A2    A3
*
*	VOID LT_HandleInput(LayoutHandle *,ULONG,ULONG *,
*	                    UWORD *,struct Gadget **);
*
*   FUNCTION
*	In order to keep track of user interface actions, such as
*	keys getting pressed, sliders getting moved, etc. your
*	code is to call LT_HandleInput() with data copied from the
*	IntuiMessage it has just received and replied.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure.
*
*	Qualifier - The Qualifier value copied from the
*	    IntuiMessage structure.
*
*	Class - Pointer to the ULONG variable which holds the
*	    value copied from the Class entry of the
*	    IntuiMessage structure.
*
*	Code - Pointer to the UWORD variable which holds the
*	    value copied from the Code entry of the
*	    IntuiMessage structure.
*
*	Gadget - Pointer to the Gadget value copied from the
*	    IAddress entry of the IntuiMessage structure.
*
*   RESULT
*	none
*
*   EXAMPLE
*	struct IntuiMessage *IntuiMessage;
*	ULONG Qualifier,Class;
*	UWORD Code;
*	struct Gadget *Gadget;
*
*	for(;;)
*	{
*	    WaitPort(Window->UserPort);
*
*	    while(IntuiMessage = GT_GetIMsg(Window->UserPort))
*	    {
*	        Class = IntuiMessage->Class;
*	        Code = IntuiMessage->Code;
*	        Qualifier = IntuiMessage->Qualifier;
*	        Gadget = IntuiMessage->Gadget;
*
*	        GT_ReplyIMsg(IntuiMessage);
*
*	        LT_HandleInput(Handle,Qualifier,&Class,&Code,&Gadget);
*	    }
*	}
*
*   NOTES
*	For BOOPSI_KIND objects keystroke activation may lead to
*	unexpected results. Your application will hear a IDCMP_GADGETUP
*	event, the IntuiMessage->Code value will hold the ANSI key
*	code of the key the user pressed.
*
*	Do not call this routine before you have actually
*	replied the IntuiMessage received or weird things
*	may happen. This is not a suggestion, it's a threat.
*
******************************************************************************
*
*/

VOID LIBENT
LT_HandleInput(REG(a0) LayoutHandle *Handle,REG(d0) ULONG MsgQualifier,REG(a1) ULONG *MsgClass,REG(a2) UWORD *MsgCode,REG(a3) struct Gadget **MsgGadget)
{
	ObjectNode *Node;
	BOOL Activate;

	if(!Handle)
		return;

	if(Handle->Failed)
	{
		*MsgClass = IDCMP_CLOSEWINDOW;

		if(!Handle->NeedDelay)
			Handle->NeedDelay = TRUE;
		else
			LTP_Delay(0,500000);	// Give the guy a break

		return;
	}

	Activate = FALSE;

	switch(*MsgClass)
	{
		case IDCMP_CHANGEWINDOW:

			if(!(Handle->Window->Flags & WFLG_SIZEGADGET) && (Handle->Window->Flags & WFLG_HASZOOM) && !V39)
			{
				#ifdef DO_BOOPSI_KIND
				{
					if(Handle->BOOPSIList)
						RefreshGList((struct Gadget *)Handle->BOOPSIList,Handle->Window,NULL,(UWORD)-1);
				}
				#endif	/* DO_BOOPSI_KIND */

				RefreshGList(Handle->List,Handle->Window,NULL,(UWORD)-1);

				GT_RefreshWindow(Handle->Window,NULL);

				LTP_DrawGroup(Handle,Handle->TopGroup);
			}

			break;

		case IDCMP_NEWSIZE:

				// Did the user cancel the resize operation?

			if(Handle->SizeVerified && Handle->SizeWidth == Handle->Window->Width && Handle->SizeHeight == Handle->Window->Height)
			{
				Handle->SizeVerified	= FALSE;
				Handle->SizeWidth	= 0;
				Handle->SizeHeight	= 0;

					// Put the gadgets back in

				AddGList(Handle->Window,Handle->List,(UWORD)-1,(UWORD)-1,NULL);
			}
			else
			{
				struct IBox Box;

				Handle->SizeWidth	= 0;
				Handle->SizeHeight	= 0;

				Box.Left	= 0;
				Box.Top		= 0;
				Box.Width	= Handle->Window->Width;
				Box.Height	= Handle->Window->Height;

				LT_LockWindow(Handle->Window);

				if((Handle->ResizeObject != NULL) && (Handle->ResizeObject->Type == LISTVIEW_KIND))
					Handle->ResizeObject->Special.List.IgnoreListContents = TRUE;

				LT_RebuildTags(Handle,TRUE,
					LAWN_Bounds,	&Box,
				TAG_DONE);

				LT_UnlockWindow(Handle->Window);

				if(Handle->Failed)
					*MsgClass = IDCMP_CLOSEWINDOW;
				else
					*MsgClass = 0;
			}

			break;

		case IDCMP_REFRESHWINDOW:

			if(Handle->AutoRefresh)
			{
				LT_BeginRefresh(Handle);

				LT_EndRefresh(Handle,TRUE);

				*MsgClass = 0;
			}

			break;

		case IDCMP_INTUITICKS:

			if(Handle->ActiveIncrementer)
			{
				if(Handle->IncrementerCountdown > 0)
					Handle->IncrementerCountdown--;

				if(Handle->IncrementerCountdown <= 0)
				{
					if(Handle->ActiveIncrementer->Host->Flags & GFLG_SELECTED)
					{
						if(Handle->ActiveIncrementer->Type == TAPEDECK_KIND)
						{
							*MsgClass	= IDCMP_GADGETUP;
							*MsgCode	= 0;
							*MsgGadget	= Handle->ActiveIncrementer->Host;
						}
						else
						{
							ObjectNode *Parent;
							LONG Number;

							if(Handle->IncrementerAccelerate > 0 && !(MsgQualifier & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)))
								Handle->IncrementerAccelerate--;

							if(Handle->IncrementerAccelerate <= 0)
							{
								Handle->IncrementerAccelerate = 10;

								Handle->IncrementerIncrement *= 2;
							}

							*MsgClass	= IDCMP_GADGETUP;
							*MsgCode	= 0;
							*MsgGadget	= Handle->ActiveIncrementer->Special.Incrementer.Parent;

							Parent = (*MsgGadget)->UserData;

							if(Parent->Type == FRACTION_KIND)
							{
								STRPTR String;

								String = (STRPTR)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE);

								if(CallHookPkt(Parent->Special.String.IncrementerHook,(APTR)String,Handle->ActiveIncrementer->Special.Incrementer.Amount < 0 ? (APTR)INCREMENTERMSG_DECREMENT : (APTR)INCREMENTERMSG_INCREMENT))
								{
									LT_SetAttributes(Handle,Parent->ID,
										GTST_String,String,
									TAG_DONE);
								}
							}
							else
							{
								if(Parent->Special.Integer.IncrementerHook)
									Number = CallHookPkt(Parent->Special.Integer.IncrementerHook,(APTR)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE),Handle->ActiveIncrementer->Special.Incrementer.Amount < 0 ? (APTR)INCREMENTERMSG_DECREMENT : (APTR)INCREMENTERMSG_INCREMENT);
								else
									Number = ((LONG)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE)) + Handle->ActiveIncrementer->Special.Incrementer.Amount * Handle->IncrementerIncrement;

								if(Number < Parent->Min)
									Number = Parent->Min;
								else
								{
									if(Number > Parent->Max)
										Number = Parent->Max;
								}

								LT_SetAttributes(Handle,Parent->ID,
									LAPR_Object,	Parent,
									GTIN_Number,	Number,
								TAG_DONE);
							}
						}
					}
					else
					{
						Handle->IncrementerIncrement	= 1;
						Handle->IncrementerAccelerate	= 10;
					}

					if(Handle->IncrementerIncrement == 1)
						Handle->IncrementerCountdown = 2;
				}
			}

			break;

		case IDCMP_RAWKEY:
		{
			UBYTE Buffer[10];
			LONG Key;
			struct InputEvent event;
			BOOL KeyUp;

			if((*MsgCode & ~IECODE_UP_PREFIX) == 95 && Handle->HelpHook)
			{
				if(!(*MsgCode & IECODE_UP_PREFIX))
				{
					ObjectNode *Item;
					struct HelpMsg Message;
					struct IBox Box;

					Item = LTP_FindNode_Position(Handle->TopGroup,Handle->Window->MouseX,Handle->Window->MouseY);

					if(Item == Handle->TopGroup)
					{
						if(Item->ID <= PHANTOM_GROUP_ID)
							Item = NULL;
					}

					if(Item)
					{
						Message.ObjectID = Item->ID;

						Box.Left	= Item->Left;
						Box.Top		= Item->Top;
						Box.Width	= Item->Width;
						Box.Height	= Item->Height;
					}
					else
					{
						Message.ObjectID = -1;

						Box.Left	= 0;
						Box.Top		= 0;
						Box.Width	= Handle->Window->Width;
						Box.Height	= Handle->Window->Height;
					}

					Message.Handle = (APTR)Handle;

					CallHookPkt(Handle->HelpHook,&Message,&Box);
				}

				if(Handle->RawKeyFilter)
					*MsgClass = 0;

				break;
			}

			if((*MsgCode >= 99 && *MsgCode <= 103) || *MsgCode == 96 || *MsgCode == 97)
			{
				if(Handle->RawKeyFilter)
					*MsgClass = 0;

				break;
			}

			event.ie_NextEvent			= NULL;
			event.ie_Code 				= (*MsgCode) & ~IECODE_UP_PREFIX;
			event.ie_Qualifier			= MsgQualifier & ~QUALIFIER_SHIFT;
			event.ie_Class				= IECLASS_RAWKEY;
			event.ie_SubClass			= 0;
			event.ie_position.ie_addr	= (APTR)*MsgGadget;

			Buffer[0] = 0;

			if(MapRawKey(&event,Buffer,9,NULL) < 1)
			{
				if(Handle->RawKeyFilter)
					*MsgClass = 0;

				break;
			}

			if(!(Key = Buffer[0]))
			{
				if(Handle->RawKeyFilter)
					*MsgClass = 0;

				break;
			}

			if((*MsgCode) & IECODE_UP_PREFIX)
				KeyUp = TRUE;
			else
				KeyUp = FALSE;

			if(!KeyUp && Handle->CursorKey && (*MsgCode == CURSORUP || *MsgCode == CURSORDOWN))
			{
				ObjectNode *Node;
				struct Gadget *Gadget;
				LONG NewState;
				LONG CurrentTop = 0;	/* For the sake of the compiler, initialize this */

				Handle->ClickSeconds = Handle->ClickMicros = 0;

				Node		= Handle->CursorKey;
				Gadget		= Node->Host;
				NewState	= Node->Current;

				if(V39)
				{
					GT_GetGadgetAttrs(Gadget,Handle->Window,NULL,
						GTLV_Top,&CurrentTop,
					TAG_DONE);
				}

				if(*MsgCode == CURSORDOWN)
				{
					if(MsgQualifier & QUALIFIER_SHIFT)
					{
						if(V39)
						{
							if(NewState != CurrentTop + Node->Lines - 1)
								NewState = CurrentTop + Node->Lines - 1;
							else
								NewState += Node->Lines;
						}
						else
							NewState += Node->Lines;
					}
					else
					{
						if(MsgQualifier & (QUALIFIER_ALT | QUALIFIER_CONTROL))
							NewState = Node->Max;
						else
							NewState++;
					}
				}
				else
				{
					if(MsgQualifier & QUALIFIER_SHIFT)
					{
						if(V39)
						{
							if(NewState != CurrentTop)
								NewState = CurrentTop;
							else
								NewState -= Node->Lines;
						}
						else
							NewState -= Node->Lines;
					}
					else
					{
						if(MsgQualifier & (QUALIFIER_ALT | QUALIFIER_CONTROL))
							NewState = Node->Min;
						else
							NewState--;
					}
				}

				if(NewState < Node->Min)
					NewState = Node->Min;
				else
				{
					if(NewState > Node->Max)
						NewState = Node->Max;
				}

				if(NewState != Node->Current || Node->Max == Node->Min)
				{
					ULONG WhichTag;

					if(V39)
						WhichTag = GTLV_MakeVisible;
					else
						WhichTag = GTLV_Top;

					GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
						WhichTag,	Node->Current = NewState,
						Node->Special.List.LinkID != -1 ? GTLV_Selected : TAG_IGNORE,Node->Current,
					TAG_DONE);

					LTP_PutStorage(Node);

					*MsgClass	= IDCMP_GADGETUP;
					*MsgCode	= Node->Current;
					*MsgGadget	= Gadget;

					if(Node->Special.List.AutoPageID != -1)
						*MsgClass = 0;

					if(!LTP_NotifyPager(Handle,Node->Special.List.AutoPageID,Node->Current))
						*MsgClass = IDCMP_CLOSEWINDOW;
				}
				else
				{
					if(Handle->RawKeyFilter)
						*MsgClass = 0;
				}

				Handle->ActiveFrame = NULL;

				return;
			}

			if(Key == '\t' && !KeyUp)
			{
				ObjectNode *Node;

				if(Handle->RawKeyFilter)
					*MsgClass = 0;

				if(Node = Handle->TabKey)
				{
					if(!(Node->Disabled))
					{
						LONG Choice;

						if(MsgQualifier & QUALIFIER_SHIFT)
							Choice = Node->Current - 1;
						else
							Choice = Node->Current + 1;

						Choice = CurrentWrap(Node,Choice);

						if(Choice != Node->Current)
						{
							LONG AutoPageID = -1,Type = 0;/* For the sake of the compiler, initialize this */
							BOOL CanBlink = FALSE;

							*MsgClass = IDCMP_GADGETUP;

							switch(Node->Type)
							{
								case CYCLE_KIND:

									AutoPageID	= Node->Special.Cycle.AutoPageID;
									Type		= GTCY_Active;
									CanBlink	= TRUE;
									break;

								#ifdef DO_POPUP_KIND
								{
									case POPUP_KIND:

										AutoPageID	= Node->Special.Popup.AutoPageID;
										Type		= LAPU_Active;
										CanBlink	= TRUE;
										break;
								}
								#endif

								#ifdef DO_TAB_KIND
								{
									case TAB_KIND:

										AutoPageID	= Node->Special.Tab.AutoPageID;
										Type		= LATB_Active;
										break;
								}
								#endif

								case MX_KIND:

									AutoPageID	= Node->Special.Radio.AutoPageID;
									Type		= GTMX_Active;

									*MsgClass = IDCMP_GADGETDOWN;

									break;
							}

							*MsgCode	= (UWORD)Choice;
							*MsgGadget	= Node->Host;

							if(CanBlink)
								LTP_BlinkButton(Handle,*MsgGadget);

							LT_SetAttributes(Handle,Node->ID,
								Type,Choice,
							TAG_DONE);

							if(AutoPageID != -1)
								*MsgClass = 0;

							if(!LTP_NotifyPager(Handle,AutoPageID,Choice))
								*MsgClass = IDCMP_CLOSEWINDOW;
						}
					}
				}

				Handle->ActiveFrame = NULL;

				return;
			}

			if(Key == '\33' && !KeyUp)
			{
				ObjectNode *Node;

				Handle->ClickSeconds = Handle->ClickMicros = 0;

				if(Node = Handle->EscKey)
				{
					if(!Node->Disabled)
					{
						LTP_BlinkButton(Handle,Node->Host);

						*MsgCode	= 0;
						*MsgGadget	= Node->Host;
						*MsgClass	= IDCMP_GADGETUP;
					}
					else
					{
						if(Handle->RawKeyFilter)
							*MsgClass = 0;
					}
				}
				else
				{
					if(Handle->Window->Flags & WFLG_CLOSEGADGET)
					{
						*MsgCode	= 0;
						*MsgGadget	= NULL;
						*MsgClass	= IDCMP_CLOSEWINDOW;
					}
					else
					{
						if(Handle->RawKeyFilter)
							*MsgClass = 0;
					}
				}

				Handle->ActiveFrame = NULL;

				return;
			}
			else
			{
				struct Gadget *Gadget;
				ObjectNode *Node;
				BOOL FoundIt,Forward,Swallow;
				LONG NewValue;
				LONG i;

				if(Handle->ReturnKey && Key == '\r')
				{
					Node = Handle->ReturnKey;

					if(!KeyUp && !Node->Disabled)
					{
						LTP_BlinkButton(Handle,Node->Host);

						*MsgClass	= IDCMP_GADGETUP;
						*MsgCode	= 0;
						*MsgGadget	= Node->Host;
					}
					else
					{
						if(Handle->RawKeyFilter)
							*MsgClass = 0;
					}

					Handle->ClickSeconds = Handle->ClickMicros = 0;

					Handle->ActiveFrame = NULL;

					return;
				}

				if(MsgQualifier & QUALIFIER_SHIFT)
					Forward = FALSE;
				else
					Forward = TRUE;

				FoundIt = FALSE;

				for(i = 0 ; !FoundIt && i < Handle->Count ; i++)
				{
					if(Gadget = Handle->GadgetArray[i])
					{
						if(GETOBJECT(Gadget,Node))
						{
							if(Key == Node->Key)
							{
								if(Node->Disabled)
									break;
								else
								{
									Swallow = FALSE;

									NewValue = Node->Current;

									switch(Node->Type)
									{
										#ifdef DO_BOOPSI_KIND
										{
											case BOOPSI_KIND:

												if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
												{
													if(Node->Special.BOOPSI.ActivateHook)
													{
														if(CallHookPkt(Node->Special.BOOPSI.ActivateHook,(APTR)Handle,(APTR)Node->Host))
														{
															Swallow = TRUE;
															break;
														}
													}

													*MsgClass	= IDCMP_GADGETUP;
													*MsgCode	= Forward ? 1 : (UWORD)-1;
													*MsgGadget	= Gadget;
												}
												else
													Swallow = TRUE;

												break;
										}
										#endif	/* DO_BOOPSI_KIND */

										#ifdef DO_POPUP_KIND
										{
											case POPUP_KIND:

												if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
												{
													if(Forward)
														NewValue++;
													else
														NewValue--;

													NewValue = CurrentWrap(Node,NewValue);

													LTP_BlinkButton(Handle,Node->Host);

													LT_SetAttributes(Handle,0,
														LAPR_Gadget,	Node->Host,
														LAPU_Active,	NewValue,
													TAG_DONE);

													*MsgClass	= IDCMP_GADGETUP;
													*MsgCode	= Node->Current;
													*MsgGadget	= Node->Host;
												}
												else
													Swallow = TRUE;

												break;
										}
										#endif	// DO_POPUP_KIND

										#ifdef DO_TAB_KIND
										{
											case TAB_KIND:

												if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
												{
													if(Forward)
														NewValue++;
													else
														NewValue--;

													NewValue = CurrentWrap(Node,NewValue);

													LT_SetAttributes(Handle,0,
														LAPR_Gadget,	Node->Host,
														LAPU_Active,	NewValue,
													TAG_DONE);

													*MsgClass	= IDCMP_GADGETUP;
													*MsgCode	= Node->Current;
													*MsgGadget	= Node->Host;
												}
												else
													Swallow = TRUE;

												break;
										}
										#endif	// DO_TAB_KIND

										#ifdef DO_TAPEDECK_KIND
										{
											case TAPEDECK_KIND:

												if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
												{
													if(Node->Special.TapeDeck.Toggle)
													{
														LT_SetAttributes(Handle,Node->ID,
															LAPR_Object,	Node,
															LATD_Pressed,	!Node->Current,
														TAG_DONE);
													}
													else
														LTP_BlinkButton(Handle,Gadget);

													if(Gadget->Flags & GFLG_SELECTED)
														*MsgCode = TRUE;
													else
														*MsgCode = FALSE;

													*MsgClass	= IDCMP_GADGETUP;
													*MsgGadget	= Gadget;
												}
												else
													Swallow = TRUE;

												break;
										}
										#endif	/* DO_TAPEDECK_KIND */

										case CHECKBOX_KIND:

											if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
											{
												Node->Current = !Node->Current;

												LTP_PutStorage(Node);

												GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
													GTCB_Checked,Node->Current,
												TAG_DONE);

												*MsgClass	= IDCMP_GADGETUP;
												*MsgCode	= Node->Current;
												*MsgGadget	= Gadget;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case LISTVIEW_KIND:

											if(!KeyUp)
											{
												if(NewValue == -1)
												{
													if(Node->Min != -1)
													{
														if(Forward)
															NewValue = Node->Min;
														else
															NewValue = Node->Max;
													}
												}
												else
												{
													if(Forward)
														NewValue++;
													else
														NewValue--;
												}

												if(CurrentInBounds(Node,NewValue))
												{
													ULONG WhichTag;

													if(V39)
														WhichTag = GTLV_MakeVisible;
													else
														WhichTag = GTLV_Top;

													GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
														WhichTag,Node->Current = NewValue,
														Node->Special.List.LinkID != -1 ? GTLV_Selected : TAG_IGNORE,Node->Current,
													TAG_DONE);

													LTP_PutStorage(Node);

													*MsgClass	= IDCMP_GADGETUP;
													*MsgCode	= Node->Current;
													*MsgGadget	= Gadget;

													if(Node->Special.List.AutoPageID != -1)
													{
														*MsgClass = 0;

														if(!LTP_NotifyPager(Handle,Node->Special.List.AutoPageID,Node->Current))
															*MsgClass = IDCMP_CLOSEWINDOW;
													}
												}
												else
													Swallow = TRUE;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case MX_KIND:

											if(!KeyUp)
											{
												if(Forward)
													NewValue++;
												else
													NewValue--;

												if(CurrentInBounds(Node,NewValue))
												{
													GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
														GTMX_Active,Node->Current = NewValue,
													TAG_DONE);

													LTP_PutStorage(Node);

													*MsgClass	= IDCMP_GADGETDOWN;
													*MsgCode	= Node->Current;
													*MsgGadget	= Gadget;

													if(Node->Special.Radio.AutoPageID != -1)
													{
														*MsgClass = 0;

														if(!LTP_NotifyPager(Handle,Node->Special.Radio.AutoPageID,Node->Current))
															*MsgClass = IDCMP_CLOSEWINDOW;
													}
												}
												else
													Swallow = TRUE;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case TEXT_KIND:

											if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
											{
												if(Node->Special.Text.Picker)
												{
													LTP_BlinkButton(Handle,Node->Special.Text.Picker);

													*MsgClass	= IDCMP_IDCMPUPDATE;
													*MsgCode	= 0;
													*MsgGadget	= Gadget;
												}
												else
													Swallow = TRUE;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case NUMBER_KIND:

											Swallow = TRUE;
											FoundIt = TRUE;

											break;

										case CYCLE_KIND:

											if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
											{
												if(Forward)
													NewValue++;
												else
													NewValue--;

												NewValue = CurrentWrap(Node,NewValue);

												LTP_BlinkButton(Handle,Gadget);

												GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
													GTCY_Active,Node->Current = NewValue,
												TAG_DONE);

												LTP_PutStorage(Node);

												*MsgClass	= IDCMP_GADGETUP;
												*MsgCode	= Node->Current;
												*MsgGadget	= Gadget;

												if(Node->Special.Cycle.AutoPageID != -1)
												{
													*MsgClass = 0;

													if(!LTP_NotifyPager(Handle,Node->Special.Cycle.AutoPageID,Node->Current))
														*MsgClass = IDCMP_CLOSEWINDOW;
												}
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case PALETTE_KIND:

											if(!KeyUp)
											{
												if(Node->Special.Palette.UsePicker)
												{
													if(!(MsgQualifier & IEQUALIFIER_REPEAT))
													{
														LTP_BlinkButton(Handle,Node->Special.Palette.Picker);

														*MsgClass	= IDCMP_IDCMPUPDATE;
														*MsgCode	= 0;
														*MsgGadget	= Gadget;
													}
													else
														Swallow = TRUE;

													FoundIt = TRUE;

													break;
												}

												if(Node->Special.Palette.TranslateBack)
												{
													LONG Index = Node->Special.Palette.TranslateBack[Node->Current];

													if(Index < Node->Min)
														Index = Node->Min;
													else
													{
														if(Index > Node->Max)
															Index = Node->Max;
													}

													if(Forward)
														Index++;
													else
														Index--;

													if(Index < Node->Min)
														Index = Node->Min;
													else
													{
														if(Index > Node->Max)
															Index = Node->Max;
													}

													NewValue = Node->Special.Palette.ColourTable[Index];
												}
												else
												{
													if(Forward)
														NewValue++;
													else
														NewValue--;
												}

												if(CurrentInBounds(Node,NewValue))
												{
													GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
														GTPA_Color,Node->Current = NewValue,
													TAG_DONE);

													LTP_PutStorage(Node);

													*MsgClass	= IDCMP_GADGETUP;
													*MsgCode	= Node->Current;
													*MsgGadget	= Gadget;
												}
												else
													Swallow = TRUE;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case SLIDER_KIND:

											if(KeyUp)
											{
												*MsgClass	= IDCMP_GADGETUP;
												*MsgCode	= Node->Current;
												*MsgGadget	= Gadget;
											}
											else
											{
												if(Forward)
													NewValue++;
												else
													NewValue--;

												if(CurrentInBounds(Node,NewValue))
												{
													GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
														GTSL_Level,Node->Current = NewValue,
													TAG_DONE);

													LTP_PutStorage(Node);

													*MsgClass	= IDCMP_MOUSEMOVE;
													*MsgCode	= Node->Current;
													*MsgGadget	= Gadget;
												}
												else
													Swallow = TRUE;
											}

											FoundIt = TRUE;

											break;

										#ifdef DO_LEVEL_KIND
										{
											case LEVEL_KIND:

												if(KeyUp)
												{
													*MsgClass	= IDCMP_GADGETUP;
													*MsgCode	= Node->Current;
													*MsgGadget	= Gadget;

													LTP_PutStorage(Node);

													LTP_LevelGadgetDrawLabel(Handle,Node,FALSE);
												}
												else
												{
													if(Forward)
														NewValue++;
													else
														NewValue--;

													if(CurrentInBounds(Node,NewValue))
													{
														LT_SetAttributes(Handle,Node->ID,
															LAPR_Object,	Node,
															LAVL_Level,		NewValue,
														TAG_DONE);

														*MsgClass	= IDCMP_MOUSEMOVE;
														*MsgCode	= NewValue;
														*MsgGadget	= Gadget;

														LTP_LevelGadgetDrawLabel(Handle,Node,FALSE);
													}
													else
														Swallow = TRUE;
												}

												FoundIt = TRUE;

												break;
										}
										#endif	/* DO_LEVEL_KIND */

										case SCROLLER_KIND:

											if(KeyUp)
											{
												*MsgClass	= IDCMP_GADGETUP;
												*MsgCode	= Node->Current;
												*MsgGadget	= Gadget;
											}
											else
											{
												if(Forward)
													NewValue++;
												else
													NewValue--;

												if(CurrentInBounds(Node,NewValue))
												{
													GT_SetGadgetAttrs(Gadget,Handle->Window,NULL,
														GTSC_Top,Node->Current = NewValue,
													TAG_DONE);

													LTP_PutStorage(Node);

													*MsgClass	= IDCMP_MOUSEMOVE;
													*MsgCode	= Node->Current;
													*MsgGadget	= Gadget;
												}
												else
													Swallow = TRUE;
											}

											FoundIt = TRUE;

											break;

										case STRING_KIND:
										case PASSWORD_KIND:

											if(!KeyUp)
											{
												if(Node->Special.String.Picker && !Forward)
												{
													LTP_BlinkButton(Handle,Node->Special.String.Picker);

													*MsgClass	= IDCMP_IDCMPUPDATE;
													*MsgCode	= 0;
													*MsgGadget	= Gadget;
												}
												else
												{
													ActivateGadget(Gadget,Handle->Window,NULL);

													*MsgClass	= IDCMP_GADGETDOWN;
													*MsgCode	= 0;
													*MsgGadget	= Gadget;

													Handle->Previous = Gadget;
												}
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case FRACTION_KIND:

											if(!KeyUp)
											{
												if(Node->Special.String.IncrementerHook)
												{
													ObjectNode *incNode,*Parent;
													struct Gadget *Incrementer;
													STRPTR String;

													Parent = (ObjectNode *)Gadget->UserData;

													if(Forward)
													{
														incNode = Node->Special.String.RightIncrementer->UserData;

														Incrementer = Node->Special.String.RightIncrementer;
													}
													else
													{
														incNode = Node->Special.String.LeftIncrementer->UserData;

														Incrementer = Node->Special.String.LeftIncrementer;
													}

													String = (STRPTR)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE);

													if(CallHookPkt(Parent->Special.String.IncrementerHook,(APTR)String,incNode->Special.Incrementer.Amount < 0 ? (APTR)INCREMENTERMSG_DECREMENT : (APTR)INCREMENTERMSG_INCREMENT))
													{
														LTP_BlinkButton(Handle,Incrementer);

														LT_SetAttributes(Handle,Parent->ID,
															GTST_String,String,
														TAG_DONE);

														*MsgClass = IDCMP_GADGETUP;
													}
												}
												else
												{
													ActivateGadget(Gadget,Handle->Window,NULL);

													*MsgClass = IDCMP_GADGETDOWN;
												}

												*MsgCode	= 0;
												*MsgGadget	= Gadget;

												Handle->Previous = Gadget;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case INTEGER_KIND:

											if(!KeyUp)
											{
												if(Node->Special.Integer.UseIncrementers)
												{
													ObjectNode *incNode,*Parent;
													struct Gadget *Incrementer;
													LONG Number;

													Parent = (ObjectNode *)Gadget->UserData;

													if(Forward)
													{
														incNode = Node->Special.Integer.RightIncrementer->UserData;

														Incrementer = Node->Special.Integer.RightIncrementer;
													}
													else
													{
														incNode = Node->Special.Integer.LeftIncrementer->UserData;

														Incrementer = Node->Special.Integer.LeftIncrementer;
													}

													if(Parent->Special.Integer.IncrementerHook)
														Number = CallHookPkt(Parent->Special.Integer.IncrementerHook,(APTR)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE),incNode->Special.Incrementer.Amount < 0 ? (APTR)INCREMENTERMSG_DECREMENT : (APTR)INCREMENTERMSG_INCREMENT);
													else
														Number = ((LONG)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE)) + incNode->Special.Incrementer.Amount;

													if(Node->Min <= Number && Number <= Node->Max && Number != Parent->Special.Integer.Number)
													{
														LTP_BlinkButton(Handle,Incrementer);

														LT_SetAttributes(Handle,Parent->ID,
															LAPR_Object,	Parent,
															GTIN_Number,	Number,
														TAG_DONE);

														*MsgClass = IDCMP_GADGETUP;
													}
												}
												else
												{
													ActivateGadget(Gadget,Handle->Window,NULL);

													*MsgClass = IDCMP_GADGETDOWN;
												}

												*MsgCode	= 0;
												*MsgGadget	= Gadget;

												Handle->Previous = Gadget;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;

										case BUTTON_KIND:

											if(!KeyUp && !(MsgQualifier & IEQUALIFIER_REPEAT))
											{
												LTP_BlinkButton(Handle,Gadget);

												*MsgClass	= IDCMP_GADGETUP;
												*MsgCode	= 0;
												*MsgGadget	= Gadget;
											}
											else
												Swallow = TRUE;

											FoundIt = TRUE;

											break;
									}

									if(Swallow)
									{
										if(Handle->RawKeyFilter)
											*MsgClass = 0;
									}
								}
							}
						}
					}
				}

				if(FoundIt)
					Handle->ClickSeconds = Handle->ClickMicros = 0;
			}

			Handle->ActiveFrame = NULL;

			break;
		}

		case IDCMP_GADGETDOWN:

			if(GETOBJECT((*MsgGadget),Node))
			{
				if(Node->Type != LISTVIEW_KIND)
					Handle->ClickSeconds = Handle->ClickMicros = 0;

				Handle->ActiveFrame = NULL;

				switch(Node->Type)
				{
					#ifdef DO_LEVEL_KIND
					{
						case LEVEL_KIND:

							Handle->CurrentLevel = Node;

							Node->Current = LTP_GetCurrentLevel(Node);

							*MsgCode = Node->Current;

							LTP_LevelGadgetDrawLabel(Handle,Node,FALSE);

							break;
					}
					#endif	/* DO_LEVEL_KIND */

					case STRING_KIND:
					case FRACTION_KIND:
					case INTEGER_KIND:
					case PASSWORD_KIND:

						Handle->Previous = *MsgGadget;
						break;

					#ifdef DO_TAPEDECK_KIND
					{
						case TAPEDECK_KIND:

							if(Node->Special.TapeDeck.Tick)
							{
								Handle->ActiveIncrementer		= Node;
								Handle->IncrementerCountdown	= 2;
								Handle->IncrementerAccelerate	= 10;
								Handle->IncrementerIncrement	= 1;

								*MsgClass	= IDCMP_GADGETUP;
								*MsgCode	= 0;
							}
							else
							{
								if(Node->Special.TapeDeck.Toggle)
								{
									if((*MsgGadget)->Flags & GFLG_SELECTED)
										*MsgCode = TRUE;
									else
										*MsgCode = FALSE;

									Node->Current = *MsgCode;

									LTP_PutStorage(Node);

									if(Handle->Previous && !Node->PageSelector)
										Activate = TRUE;

									*MsgClass = IDCMP_GADGETUP;
								}
							}

							break;
					}
					#endif	/* DO_TAPEDECK_KIND */

					case MX_KIND:

						if(!V39 && Node->Disabled)
						{
							GT_SetGadgetAttrs(*MsgGadget,Handle->Window,NULL,
								GTMX_Active,Node->Current,
							TAG_DONE);

							*MsgClass = 0;
						}
						else
						{
							Node->Current = *MsgCode;

							LTP_PutStorage(Node);

							if(Node->Special.Radio.AutoPageID != -1)
							{
								*MsgClass = 0;

								if(!LTP_NotifyPager(Handle,Node->Special.Radio.AutoPageID,Node->Current))
									*MsgClass = IDCMP_CLOSEWINDOW;
							}

							if(Handle->Previous && !Node->PageSelector)
								Activate = TRUE;
						}

						break;

					case SLIDER_KIND:

						if(Node->Min < 0)
							Node->Current = (WORD)*MsgCode;
						else
							Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;

					case SCROLLER_KIND:

						Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;

					case INCREMENTER_KIND:

						Handle->ActiveIncrementer		= Node;
						Handle->IncrementerCountdown	= 6;
						Handle->IncrementerAccelerate	= 10;
						Handle->IncrementerIncrement	= 1;

						*MsgClass	= IDCMP_GADGETUP;
						*MsgCode	= 0;
						*MsgGadget	= Node->Special.Incrementer.Parent;

						/* ALWAYS */
						{
							struct ObjectNode *Parent;
							LONG Number;

							Parent = (ObjectNode *)(*MsgGadget)->UserData;

							if(Parent->Type == FRACTION_KIND)
							{
								STRPTR String;

								String = (STRPTR)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE);

								if(CallHookPkt(Parent->Special.String.IncrementerHook,(APTR)String,Node->Special.Incrementer.Amount < 0 ? (APTR)INCREMENTERMSG_DECREMENT : (APTR)INCREMENTERMSG_INCREMENT))
								{
									LT_SetAttributes(Handle,Parent->ID,
										GTST_String,String,
									TAG_DONE);
								}
							}
							else
							{
								if(Parent->Special.Integer.IncrementerHook)
									Number = CallHookPkt(Parent->Special.Integer.IncrementerHook,(APTR)LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE),Node->Special.Incrementer.Amount < 0 ? (APTR)INCREMENTERMSG_DECREMENT : (APTR)INCREMENTERMSG_INCREMENT);
								else
									Number = LT_GetAttributes(Handle,Parent->ID,LAPR_Object,Parent,TAG_DONE) + Node->Special.Incrementer.Amount;

								if(Number >= Parent->Min && Number <= Parent->Max)
									LT_SetAttributes(Handle,Parent->ID,LAPR_Object,Parent,GTIN_Number,Number,TAG_DONE);
								else
									*MsgClass = 0;
							}
						}

						break;
				}
			}

			break;

		case IDCMP_MOUSEBUTTONS:
		{
			ObjectNode *Node;
			LONG x,y;

			x = Handle->Window->MouseX;
			y = Handle->Window->MouseY;

			Node = LTP_FindNode_Position(Handle->TopGroup,x,y);

			if(((*MsgCode) & IECODE_UP_PREFIX) && Handle->Previous)
				Activate = TRUE;

			if(Node && !Node->Disabled)
			{
				if(*MsgCode == SELECTDOWN)
				{
					switch(Node->Type)
					{
						case CHECKBOX_KIND:

							Node->Current = !Node->Current;

							LTP_PutStorage(Node);

							GT_SetGadgetAttrs(Node->Host,Handle->Window,NULL,
								GTCB_Checked,Node->Current,
							TAG_DONE);

							*MsgClass	= IDCMP_GADGETUP;
							*MsgCode	= Node->Current;
							*MsgGadget	= Node->Host;

							break;

						case MX_KIND:
						{
							LONG Index = LTP_Find_Clicked_Item(Handle,Node,x,y);

							if(Index != -1)
							{
								Node->Current = Index;

								GT_SetGadgetAttrs(Node->Host,Handle->Window,NULL,
									GTMX_Active,Index,
								TAG_DONE);

								LTP_PutStorage(Node);

								*MsgClass	= IDCMP_GADGETDOWN;
								*MsgCode	= Index;
								*MsgGadget	= Node->Host;

								if(Node->Special.Cycle.AutoPageID != -1)
									*MsgClass = 0;

								if(!LTP_NotifyPager(Handle,Node->Special.Cycle.AutoPageID,Node->Current))
									*MsgClass = IDCMP_CLOSEWINDOW;
							}

							break;
						}

						case FRAME_KIND:

							if(Node->Special.Frame.GenerateEvents)
							{
								if(Node->Special.Frame.DrawBox)
								{
									if(x < Node->Left + 2 || x >= Node->Left + Node->Width - 2 || y < Node->Top + 1 || y >= Node->Top + Node->Height - 2)
										break;
								}

								*MsgClass	= IDCMP_GADGETDOWN;
								*MsgGadget	= &Node->Special.Frame.Dummy;

								Handle->ActiveFrame = Node;
							}

							break;
					}
				}

				if(*MsgCode == SELECTUP)
				{
						// Now this is a real kludge; as of v37 the
						// MX_KIND labels are not part of the gadget
						// itself and a plain SELECTDOWN mouse event
						// is sent as soon as the user clicks on it.
						// This changed with v39 which presumably had
						// some code in there to map mouse clicks on
						// the gadget labels to proper buttonpress
						// events. Now, try as you might the click is
						// swallowed and we have to do with the
						// SELECTUP, sigh...

					if(V39 && Node->Type == MX_KIND)
					{
						LONG Index;

						Index = LTP_Find_Clicked_Item(Handle,Node,x,y);

						if(Index != -1)
						{
							GT_SetGadgetAttrs(Node->Host,Handle->Window,NULL,
								GTMX_Active,Index,
							TAG_DONE);

							Node->Current = Index;

							LTP_PutStorage(Node);

							*MsgClass	= IDCMP_GADGETDOWN;
							*MsgCode	= Index;
							*MsgGadget	= Node->Host;

							if(Node->Special.Cycle.AutoPageID != -1)
								*MsgClass = 0;

							if(!LTP_NotifyPager(Handle,Node->Special.Cycle.AutoPageID,Node->Current))
								*MsgClass = IDCMP_CLOSEWINDOW;
						}
					}

					if(Handle->ActiveIncrementer)
					{
						if(Handle->ActiveIncrementer->Type == TAPEDECK_KIND && Handle->ActiveIncrementer ->Special.TapeDeck.Tick)
						{
							Handle->ActiveIncrementer = NULL;

							*MsgClass	= IDCMP_GADGETUP;
							*MsgCode	= 1;
						}
						else
						{
							if(((ObjectNode *)Handle->ActiveIncrementer->Special.Incrementer.Parent->UserData)->Type == INTEGER_KIND)
							{
								*MsgGadget	= Handle->ActiveIncrementer->Special.Incrementer.Parent;
								*MsgClass	= IDCMP_GADGETUP;
								*MsgCode	= 1;
							}

							Handle->ActiveIncrementer = NULL;
						}
					}

					if(Handle->ActiveFrame)
					{
						*MsgClass	= IDCMP_GADGETUP;
						*MsgGadget	= &Node->Special.Frame.Dummy;

						Handle->ActiveFrame = NULL;
					}
				}
			}

			break;
		}

		case IDCMP_GADGETUP:

			#ifdef DO_LEVEL_KIND
			{
				Handle->CurrentLevel = NULL;
			}
			#endif	/* DO_LEVEL_KIND */

			Handle->ActiveFrame = NULL;

			if(GETOBJECT((*MsgGadget),Node))
			{
				if(Node->Type != LISTVIEW_KIND)
					Handle->ClickSeconds = Handle->ClickMicros = 0;

				switch(Node->Type)
				{
					case PASSWORD_KIND:
					case STRING_KIND:
					case FRACTION_KIND:

						LTP_PutStorage(Node);

						if(*MsgCode == 0x5F && Handle->HelpHook)
						{
							struct HelpMsg Message;
							struct IBox Box;

							Message.ObjectID = Node->ID;

							Box.Left	= Node->Left;
							Box.Top		= Node->Top;
							Box.Width	= Node->Width;
							Box.Height	= Node->Height;

							Message.Handle = (APTR)Handle;

							CallHookPkt(Handle->HelpHook,&Message,&Box);

							*MsgClass = 0;
						}
						else
							Handle->Previous = NULL;

						break;

					case INTEGER_KIND:
					{
						struct StringInfo *StringInfo = (struct StringInfo *)(*MsgGadget)->SpecialInfo;
						LONG Contents = StringInfo->LongInt;

						if(Contents < Node->Min)
							Contents = Node->Min;
						else
						{
							if(Contents > Node->Max)
								Contents = Node->Max;
						}

						LT_SetAttributes(Handle,(*MsgGadget)->GadgetID,GTIN_Number,Contents,TAG_DONE);

						LTP_PutStorage(Node);

						if(*MsgCode == 0x5F && Handle->HelpHook)
						{
							struct HelpMsg Message;
							struct IBox Box;

							Message.ObjectID = Node->ID;

							Box.Left	= Node->Left;
							Box.Top		= Node->Top;
							Box.Width	= Node->Width;
							Box.Height	= Node->Height;

							Message.Handle = (APTR)Handle;

							CallHookPkt(Handle->HelpHook,&Message,&Box);

							*MsgClass = 0;
						}
						else
							Handle->Previous = NULL;

						break;
					}

					case CHECKBOX_KIND:

						if(!V39)
						{
							if((*MsgGadget)->Flags & GFLG_SELECTED)
								*MsgCode = TRUE;
							else
								*MsgCode = FALSE;
						}

						Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;

					#ifdef DO_TAPEDECK_KIND
					{
						case TAPEDECK_KIND:

							if(Node->Special.TapeDeck.Tick)
							{
								Handle->ActiveIncrementer = NULL;

								*MsgClass	= IDCMP_GADGETUP;
								*MsgCode	= 1;
							}
							else
							{
								if(Node->Special.TapeDeck.Toggle)
								{
									if((*MsgGadget)->Flags & GFLG_SELECTED)
										*MsgCode = TRUE;
									else
										*MsgCode = FALSE;

									Node->Current = *MsgCode;

									LTP_PutStorage(Node);
								}
							}

							if(Handle->Previous && !Node->PageSelector)
								Activate = TRUE;

							break;
					}
					#endif	/* DO_TAPEDECK_KIND */

					#ifdef DO_POPUP_KIND
					{
						case POPUP_KIND:

							Node->Current = *MsgCode;

							LTP_PutStorage(Node);

							if(Handle->Previous && !Node->PageSelector)
								Activate = TRUE;

							if(Node->Special.Popup.AutoPageID != -1)
							{
								*MsgClass = 0;

								if(!LTP_NotifyPager(Handle,Node->Special.Popup.AutoPageID,Node->Current))
									*MsgClass = IDCMP_CLOSEWINDOW;
							}

							break;
					}
					#endif

					#ifdef DO_TAB_KIND
					{
						case TAB_KIND:

							Node->Current = *MsgCode;

							LTP_PutStorage(Node);

							if(Handle->Previous && !Node->PageSelector)
								Activate = TRUE;

							if(Node->Special.Tab.AutoPageID != -1)
							{
								*MsgClass = 0;

								if(!LTP_NotifyPager(Handle,Node->Special.Tab.AutoPageID,Node->Current))
									*MsgClass = IDCMP_CLOSEWINDOW;
							}

							break;
					}
					#endif

					case CYCLE_KIND:

						Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						if(Node->Special.Cycle.AutoPageID != -1)
						{
							*MsgClass = 0;

							if(!LTP_NotifyPager(Handle,Node->Special.Cycle.AutoPageID,Node->Current))
								*MsgClass = IDCMP_CLOSEWINDOW;
						}

						break;

					case PALETTE_KIND:

						Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;

					case MX_KIND:

						if(!V39 && Node->Disabled)
						{
							GT_SetGadgetAttrs(*MsgGadget,Handle->Window,NULL,
								GTMX_Active,Node->Current,
							TAG_DONE);

							*MsgClass = 0;
						}
						else
						{
							Node->Current = *MsgCode;

							LTP_PutStorage(Node);

							if(Node->Special.Radio.AutoPageID != -1)
							{
								*MsgClass = 0;

								if(!LTP_NotifyPager(Handle,Node->Special.Radio.AutoPageID,Node->Current))
									*MsgClass = IDCMP_CLOSEWINDOW;
							}

							if(Handle->Previous && !Node->PageSelector)
								Activate = TRUE;
						}

						break;

					case SLIDER_KIND:

						if(Node->Min < 0)
							Node->Current = (WORD)*MsgCode;
						else
							Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;

					#ifdef DO_LEVEL_KIND
					{
						case LEVEL_KIND:

							Node->Current = LTP_GetCurrentLevel(Node);

							*MsgCode	= Node->Current;
							*MsgGadget	= Node->Host;

							LTP_PutStorage(Node);

							LTP_LevelGadgetDrawLabel(Handle,Node,FALSE);

							break;
					}
					#endif	/* DO_LEVEL_KIND */

					case SCROLLER_KIND:

						Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;

					case PICKER_KIND:

						*MsgClass	= IDCMP_IDCMPUPDATE;
						*MsgCode	= 0;
						*MsgGadget	= Node->Special.Picker.Parent;

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;

					case INCREMENTER_KIND:

						if(((ObjectNode *)Handle->ActiveIncrementer->Special.Incrementer.Parent->UserData)->Type == INTEGER_KIND)
						{
							*MsgGadget	= Handle->ActiveIncrementer->Special.Incrementer.Parent;
							*MsgClass	= IDCMP_GADGETUP;
							*MsgCode	= 1;
						}

						Handle->ActiveIncrementer = NULL;
						break;

					case LISTVIEW_KIND:

						if(Node->Current != *MsgCode || Node != Handle->ClickObject)
						{
							CurrentTime(&Handle->ClickSeconds,&Handle->ClickMicros);

							Handle->ClickObject = Node;
						}
						else
						{
							ULONG Seconds,Micros;

							CurrentTime(&Seconds,&Micros);

							if(DoubleClick(Handle->ClickSeconds,Handle->ClickMicros,Seconds,Micros))
								*MsgClass = IDCMP_IDCMPUPDATE;

							Handle->ClickSeconds	= Seconds;
							Handle->ClickMicros	= Micros;
						}

						Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						if(Node->Special.List.AutoPageID != -1)
							*MsgClass = (IPTR)NULL;

						if(!LTP_NotifyPager(Handle,Node->Special.List.AutoPageID,Node->Current))
							*MsgClass = IDCMP_CLOSEWINDOW;

						if(Node->Special.List.Link)
						{
							Handle->Previous = Node->Special.List.Link;

							if(!Node->PageSelector)
								Activate = TRUE;
						}
						else
						{
							if(Handle->Previous && !Node->PageSelector)
								Activate = TRUE;
						}

						break;

					default:

						if(Handle->Previous && !Node->PageSelector)
							Activate = TRUE;

						break;
				}
			}

			break;

		case IDCMP_MOUSEMOVE:

			#ifdef DO_LEVEL_KIND
			{
				if(Handle->CurrentLevel)
					Node = Handle->CurrentLevel;
				else
					(void)GETOBJECT((*MsgGadget),Node);
			}
			#else
			{
				GETOBJECT((*MsgGadget),Node);
			}
			#endif /* DO_LEVEL_KIND */

			if(Node)
			{
				if(Node->Type != LISTVIEW_KIND)
					Handle->ClickSeconds = Handle->ClickMicros = 0;

				switch(Node->Type)
				{
					case SCROLLER_KIND:

						Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						break;

					case SLIDER_KIND:

						if(Node->Min < 0)
							Node->Current = (WORD)*MsgCode;
						else
							Node->Current = *MsgCode;

						LTP_PutStorage(Node);

						break;

					#ifdef DO_LEVEL_KIND
					{
						case LEVEL_KIND:

							Node->Current = LTP_GetCurrentLevel(Node);

							*MsgCode	= Node->Current;
							*MsgGadget  = Node->Host;

							LTP_PutStorage(Node);

							LTP_LevelGadgetDrawLabel(Handle,Node,FALSE);

							break;
					}
					#endif	/* DO_LEVEL_KIND */
				}
			}

			break;

		case IDCMP_ACTIVEWINDOW:

			if(Handle->Previous)
			{
				Activate = TRUE;

				*MsgClass	= IDCMP_GADGETDOWN;
				*MsgCode	= 0;
				*MsgGadget	= Handle->Previous;
			}

			break;

		case IDCMP_INACTIVEWINDOW:

			Handle->ActiveIncrementer = NULL;
			Handle->ActiveFrame = NULL;
			break;
	}

	if(Handle->AutoActivate && Activate && Handle->Previous)
	{
		if(GETOBJECT(Handle->Previous,Node))
		{
			if(!Node->Disabled)
			{
				if(Node->Type == BOOPSI_KIND)
				{
					if(Node->Special.BOOPSI.ActivateHook)
						CallHookPkt(Node->Special.BOOPSI.ActivateHook,(APTR)Handle,(APTR)Node->Host);
				}
				else
					ActivateGadget(Handle->Previous,Handle->Window,NULL);
			}
		}
	}
}
