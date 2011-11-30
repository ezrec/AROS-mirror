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

#ifdef DO_BOOPSI_KIND	/* Support code */

/*****************************************************************************/

#include <dos/dosextens.h>

/*****************************************************************************/

STATIC VOID
LocalAddPart(STRPTR Base,STRPTR Name)
{
	LONG Len = strlen(Base);

	if(Base[Len - 1] != ':')
		strcat(Base,"/");

	strcat(Base,Name);
}

STATIC struct Library *
OpenClass(STRPTR Name)
{
	struct Library *Result;

	Result = OpenLibrary(Name,0);
	if(Result == NULL)
	{
		struct Process *	Process = (struct Process *)FindTask(NULL);
		UBYTE				LocalBuffer[256];
		LONG				i,Len;

		Len = strlen(Name);

		for(i = Len - 1 ; i >= 0 ; i--)
		{
			if(Name[i] == '/' || Name[i] == ':')
			{
				Name = &Name[i + 1];

				break;
			}
		}

		if(Process->pr_Task.tc_Node.ln_Type == NT_PROCESS && Process->pr_HomeDir != BNULL)
		{
			strcpy(LocalBuffer,"PROGDIR:Classes/Gadgets");
			LocalAddPart(LocalBuffer,Name);

			Result = OpenLibrary(LocalBuffer,0);
		}

		if(!Result)
		{
			strcpy(LocalBuffer,":Classes/Gadgets");
			LocalAddPart(LocalBuffer,Name);

			if(!(Result = OpenLibrary(LocalBuffer,0)))
			{
				strcpy(LocalBuffer,"Gadgets");
				LocalAddPart(LocalBuffer,Name);

				if(!(Result = OpenLibrary(LocalBuffer,0)))
					Result = OpenLibrary(Name,0);
			}
		}
	}

	return(Result);
}

#endif	/* DO_BOOPSI_KIND */


/*****************************************************************************/


VOID
LTP_LayoutGadgets(LayoutHandle *Handle,ObjectNode *Group,LONG Left,LONG Top,LONG PlusWidth,LONG PlusHeight)
{
	if(!Handle->Failed)
	{
		STATIC const struct TagItem NoUnderscoreVanillaTags[] =
		{
			{TAG_DONE}
		};

		STATIC const struct TagItem VanillaTags[] =
		{
			{GT_Underscore, '_'},
			{TAG_DONE}
		};

		STATIC const struct TagItem DisabledTags[] =
		{
			{GT_Underscore, '_' },
			{GA_Disabled,	TRUE},
			{TAG_DONE}
		};

		STATIC const struct TagItem NoUnderscoreDisabledTags[] =
		{
			{GA_Disabled,	TRUE},
			{TAG_DONE}
		};

		struct NewGadget	NewGadget;

		struct Gadget *		Gadget;
		struct Gadget *		Extra;
		struct TagItem *	Tags;
		ULONG				page;
		ObjectNode *		Node;

		if(!Handle->List)
		{
			if(!(Handle->Previous = CreateContext(&Handle->List)))
			{
				Handle->Failed = TRUE;

				return;
			}
		}

		NewGadget.ng_TextAttr   = Handle->TextAttr;
		NewGadget.ng_VisualInfo = Handle->VisualInfo;

		Group->Special.Group.Visible = TRUE;

		SCANPAGE(Group,Node,page)
		{
			if(Node->Type != FRAME_KIND && Node->Type != IMAGE_KIND && Node->Type != BLANK_KIND && Node->Type != BOX_KIND && Node->Type != XBAR_KIND && Node->Type != YBAR_KIND)
			{
				Extra = NULL;

				if(Node->GroupIndent)
				{
					NewGadget.ng_LeftEdge 	= Node->Left + 4;
					NewGadget.ng_TopEdge	= Node->Top + 2;
					NewGadget.ng_Width		= Node->Width - (4 + 4);
					NewGadget.ng_Height		= Node->Height - (2 + 2);
				}
				else
				{
					NewGadget.ng_LeftEdge 	= Node->Left;
					NewGadget.ng_TopEdge	= Node->Top;
					NewGadget.ng_Width		= Node->Width;
					NewGadget.ng_Height		= Node->Height;
				}

				NewGadget.ng_GadgetText	= Node->Label ? Node->Label : (STRPTR)"";
				NewGadget.ng_GadgetID 	= Node->ID;
				NewGadget.ng_UserData 	= Node;

				if(Node->NoKey)
				{
					if(Node->Disabled)
					{
						if(!V39 && (Node->Type == MX_KIND || Node->Type == LISTVIEW_KIND))
							Tags = (struct TagItem *)NoUnderscoreVanillaTags;
						else
							Tags = (struct TagItem *)NoUnderscoreDisabledTags;
					}
					else
						Tags = (struct TagItem *)NoUnderscoreVanillaTags;
				}
				else
				{
					if(Node->Disabled)
					{
						if(!V39 && (Node->Type == MX_KIND || Node->Type == LISTVIEW_KIND))
							Tags = (struct TagItem *)VanillaTags;
						else
							Tags = (struct TagItem *)DisabledTags;
					}
					else
						Tags = (struct TagItem *)VanillaTags;
				}

				switch(Node->LabelPlace)
				{
					case PLACE_LEFT:

						NewGadget.ng_Flags = PLACETEXT_LEFT;
						break;

					case PLACE_RIGHT:

						NewGadget.ng_Flags = PLACETEXT_RIGHT;
						break;

					case PLACE_ABOVE:

						NewGadget.ng_Flags = PLACETEXT_ABOVE;
						break;

					case PLACE_BELOW:

						NewGadget.ng_Flags = PLACETEXT_BELOW;
						break;

					case PLACE_IN:

						NewGadget.ng_Flags = PLACETEXT_IN;
						break;

					default:

						NewGadget.ng_Flags = 0;
						break;
				}

				if(Node->HighLabel)
					NewGadget.ng_Flags |= NG_HIGHLABEL;

				Gadget = NULL; /* Always initialize this before entering the switch table. */

				switch(Node->Type)
				{
					#ifdef DO_BOOPSI_KIND
					{
						case BOOPSI_KIND:
						{
							struct Gadget *Link;

							if(Node->Special.BOOPSI.Link != NIL_LINK && Node->Special.BOOPSI.TagLink)
							{
								if(!(Link = LTP_FindGadgetLinear(Handle,Node->Special.BOOPSI.Link)))
								{
									Handle->Failed = TRUE;

									return;	/* Note: must return since we are in a loop that
											 *       creates all gadgets for the current page.
											 */
								}
							}
							else
								Link = NULL;

							if(Node->Special.BOOPSI.ClassLibraryName)
							{
								if(!(Node->Special.BOOPSI.ClassBase = OpenClass(Node->Special.BOOPSI.ClassLibraryName)))
								{
									break;
								}
							}

							if(Node->Special.BOOPSI.FullWidth)
							{
								NewGadget.ng_LeftEdge	= Node->Special.BOOPSI.Parent->Left;
								NewGadget.ng_Width		= Node->Special.BOOPSI.Parent->Width;
							}

							if(Node->Special.BOOPSI.FullHeight)
							{
								NewGadget.ng_TopEdge	= Node->Special.BOOPSI.Parent->Top;
								NewGadget.ng_Height		= Node->Special.BOOPSI.Parent->Height;
							}

							Gadget = NewObject(Node->Special.BOOPSI.ClassInstance,Node->Special.BOOPSI.ClassName,
								GA_Left,		NewGadget.ng_LeftEdge,
								GA_Top,			NewGadget.ng_TopEdge,
								GA_Width,		NewGadget.ng_Width,
								GA_Height,		NewGadget.ng_Height,
								GA_Disabled,	Node->Disabled,
								GA_ID,			Node->ID,
								GA_UserData,	Node,

								Link ? Node->Special.BOOPSI.TagLink : TAG_IGNORE,									Link,
								Handle->BOOPSIPrevious ? GA_Previous : TAG_IGNORE,									Handle->BOOPSIPrevious,
								Node->Special.BOOPSI.TagTextAttr ? Node->Special.BOOPSI.TagTextAttr : TAG_IGNORE,	Handle->TextAttr,
								Node->Special.BOOPSI.TagDrawInfo ? Node->Special.BOOPSI.TagDrawInfo : TAG_IGNORE,	Handle->DrawInfo,
								Node->Special.BOOPSI.TagScreen ? Node->Special.BOOPSI.TagScreen : TAG_IGNORE,		Handle->Screen,
								Node->Special.BOOPSI.ClassTags ? TAG_MORE : TAG_DONE,								Node->Special.BOOPSI.ClassTags);

							break;
						}
					}
					#endif /* DO_BOOPSI_KIND */

					#ifdef DO_POPUP_KIND
					{
						case POPUP_KIND:

							Gadget = NewObject(LTP_PopupClass,NULL,
								GA_Left,			NewGadget.ng_LeftEdge,
								GA_Top,				NewGadget.ng_TopEdge,
								GA_Width,			NewGadget.ng_Width,
								GA_Height,			NewGadget.ng_Height,
								GA_Disabled,		Node->Disabled,
								GA_ID,				Node->ID,
								GA_UserData,		Node,
								GA_RelVerify,		TRUE,
								PIA_Labels,			Node->Special.Popup.Choices,
								PIA_Active,			Node->Current,
								PIA_Font,			Handle->TextAttr,
								PIA_CentreActive,	Node->Special.Popup.CentreActive,
								PIA_AspectX,		Handle->AspectX,
								PIA_AspectY,		Handle->AspectY,

								Handle->BOOPSIPrevious ? GA_Previous : TAG_IGNORE,Handle->BOOPSIPrevious,
							TAG_DONE);

							break;
					}
					#endif /* DO_POPUP_KIND */

					#ifdef DO_TAB_KIND
					{
						case TAB_KIND:

							if(Node->Special.Tab.FullWidth)
							{
								NewGadget.ng_LeftEdge	= Handle->TopGroup->Left	- Left;
								NewGadget.ng_Width		= Handle->TopGroup->Width	+ PlusWidth;

								NewGadget.ng_LeftEdge	+= Handle->Screen->WBorLeft;
								NewGadget.ng_Width		-= Handle->Screen->WBorLeft + Handle->Screen->WBorRight;
							}
							else
							{
								NewGadget.ng_LeftEdge	= Node->Special.Tab.Parent->Left;
								NewGadget.ng_Width		= Node->Special.Tab.Parent->Width;
							}

							Gadget = NewObject(LTP_TabClass,NULL,
								GA_Left,		NewGadget.ng_LeftEdge,
								GA_Top,			NewGadget.ng_TopEdge,
								GA_Width,		NewGadget.ng_Width,
								GA_Height,		NewGadget.ng_Height,
								GA_Disabled,	Node->Disabled,
								GA_ID,			Node->ID,
								GA_UserData,	Node,
								GA_RelVerify,	TRUE,
								TIA_Labels,		Node->Special.Popup.Choices,
								TIA_Index,		Node->Current,
								TIA_Font,		Handle->TextAttr,
								TIA_DrawInfo,	Handle->DrawInfo,
								TIA_Screen,		Handle->Screen,

								Handle->BOOPSIPrevious ? GA_Previous : TAG_IGNORE,Handle->BOOPSIPrevious,
							TAG_DONE);

							break;
					}
					#endif	// DO_TAB_KIND

					#ifdef DO_LEVEL_KIND
					{
						case LEVEL_KIND:
						{
							LONG Current;

							if(Node->Special.Level.Freedom == FREEHORIZ)
								Current = Node->Current;
							else
								Current = Node->Max - Node->Current;

							Gadget = NewObject(LTP_LevelClass,NULL,
								GA_Left,		NewGadget.ng_LeftEdge,
								GA_Top,			NewGadget.ng_TopEdge,
								GA_Width,		NewGadget.ng_Width,
								GA_Height,		NewGadget.ng_Height,
								GA_Disabled,	Node->Disabled,
								GA_ID,			Node->ID,
								GA_UserData,	Node,
								GA_RelVerify,	TRUE,
								GA_Immediate,	TRUE,
								GA_FollowMouse,	TRUE,
								PGA_Freedom,	Node->Special.Level.Freedom,
								SLA_UseTicks,	Node->Special.Level.Ticks,
								SLA_NumTicks,	Node->Special.Level.NumTicks,
								SLA_DrawInfo,	Handle->DrawInfo,
								SLA_Current,	Current		- Node->Special.Level.Plus,
								SLA_Max,		Node->Max	- Node->Special.Level.Plus,
								SLA_Reference,	Handle->GlyphHeight,

								Handle->BOOPSIPrevious ? GA_Previous : TAG_IGNORE,Handle->BOOPSIPrevious,
							TAG_DONE);

							if(Gadget != NULL)
							{
								if(Node->Special.Level.Freedom == FREEHORIZ)
								{
									SetAttrs(Gadget,
										GA_Top, NewGadget.ng_TopEdge + (Node->Height - Gadget->Height) / 2,
									TAG_DONE);
								}
								else
								{
									SetAttrs(Gadget,
										GA_Left, NewGadget.ng_LeftEdge + (Node->Width - Gadget->Width) / 2,
									TAG_DONE);
								}
							}

							break;
						}
					}
					#endif	/* DO_LEVEL_KIND */

					case BUTTON_KIND:
					{
						register BOOL IsNonStandardButton;

						#ifdef OLD_STYLE_DEFAULT_KEY
						{
							IsNonStandardButton = (BOOL)(Node->Special.Button.Lines != NULL);
						}
						#else
						{
							IsNonStandardButton = (BOOL)(	(Node->Special.Button.Lines != NULL) ||
															(Node->Special.Button.ReturnKey != 0)
														);
						}
						#endif /* OLD_STYLE_DEFAULT_KEY */

						if(IsNonStandardButton)
						{
							#ifndef OLD_STYLE_DEFAULT_KEY
							{
								if(Node->Special.Button.ReturnKey)
									Handle->TextAttr->ta_Style |= FSF_BOLD;
							}
							#endif	/* !OLD_STYLE_DEFAULT_KEY */

							if(Node->Special.Button.ButtonImage = NewObject(LTP_ImageClass,NULL,
								IIA_ImageType,	IMAGECLASS_MULTILINEBUTTON,
								IIA_Lines,		Node->Special.Button.Lines,
								IIA_LineCount,	Node->Special.Button.LineCount,
								IIA_KeyStroke,	Node->Special.Button.KeyStroke,
								IIA_Emboss,		Node->Special.Button.ReturnKey,
								IIA_Label,		Node->Label,
								IIA_Font,		Handle->TextAttr,
								IA_Width,		NewGadget.ng_Width,
								IA_Height,		NewGadget.ng_Height,
							TAG_DONE))
							{
								NewGadget.ng_GadgetText	= "";
								NewGadget.ng_Flags		= 0;

								if(Gadget = CreateGadgetA(GENERIC_KIND,Handle->Previous,&NewGadget,NULL))
								{
									Gadget->GadgetType		|= GTYP_BOOLGADGET;

									Gadget->Flags 			= GFLG_GADGIMAGE | GFLG_GADGHIMAGE;
									Gadget->Activation		= GACT_RELVERIFY;

									Gadget->GadgetRender	= Node->Special.Button.ButtonImage;
									Gadget->SelectRender	= Node->Special.Button.ButtonImage;

									if(Node->Disabled)
										Gadget->Flags |= GFLG_DISABLED;
								}
							}

							#ifndef OLD_STYLE_DEFAULT_KEY
							{
								if(Node->Special.Button.ReturnKey)
									Handle->TextAttr->ta_Style &= ~FSF_BOLD;
							}
							#endif	/* !OLD_STYLE_DEFAULT_KEY */
						}
						else
						{
							Gadget = CreateGadgetA(BUTTON_KIND,Handle->Previous,&NewGadget,Tags);
						}

						break;
					}

					case GROUP_KIND:

						LTP_LayoutGadgets(Handle,Node,Left,Top,PlusWidth,PlusHeight);

						continue;

					case CHECKBOX_KIND:

						Gadget = CreateGadget(CHECKBOX_KIND,Handle->Previous,&NewGadget,
							GTCB_Checked,	Node->Current,
							GTCB_Scaled,	TRUE,
						TAG_MORE,Tags);

						break;

					case LISTVIEW_KIND:
					{
						struct TextFont *LocalFont = NULL;
						ULONG WhichTag;
						struct Gadget * LinkedGadget;

						if(!Node->Special.List.ReadOnly && Node->Special.List.LinkID != -1 && Node->Special.List.LinkID != NIL_LINK)
							LinkedGadget = Node->Special.List.Link = LTP_FindGadgetLinear(Handle,Node->Special.List.LinkID);
						else
							LinkedGadget = NULL;

						if((NewGadget.ng_Flags & PLACETEXT_ABOVE) && Node->Label && Node->Special.List.FlushLabelLeft)
							NewGadget.ng_GadgetText = "";

						if(Node->Special.List.TextAttr)
						{
								// Use the system default font?

							if(Node->Special.List.TextAttr == (struct TextAttr *)~0)
							{
								Forbid();

								LTP_GetDefaultFont(&Handle->FixedFont);

								if(!(LocalFont = OpenFont((struct TextAttr *)&Handle->FixedFont)))
								{
									Permit();

									Handle->Failed = TRUE;

									break;
								}

								NewGadget.ng_TextAttr = (struct TextAttr *)&Handle->FixedFont;
							}
							else
							{
								NewGadget.ng_TextAttr = Node->Special.List.TextAttr;
							}

							NewGadget.ng_GadgetText = "";
						}

						if(V39)
							WhichTag = GTLV_MakeVisible;
						else
							WhichTag = GTLV_Top;

						Gadget = CreateGadget(LISTVIEW_KIND,Handle->Previous,&NewGadget,
							GTLV_Selected,		Node->Current,
							GTLV_ReadOnly,		Node->Special.List.ReadOnly,
							GTLV_ScrollWidth,	2 + 2 * Handle->GlyphWidth + 2,

							Node->Current >= 0 ? WhichTag : TAG_IGNORE,							Node->Current,
							Node->Special.List.Labels ? GTLV_Labels : TAG_IGNORE,				Node->Special.List.Labels,
							Node->Special.List.LinkID != -1 ? GTLV_ShowSelected : TAG_IGNORE,	LinkedGadget,
							Node->Special.List.CallBack ?	GTLV_CallBack : TAG_IGNORE,			Node->Special.List.CallBack,
							Node->Special.List.MaxPen ?	GTLV_MaxPen : TAG_IGNORE,				Node->Special.List.MaxPen,
						TAG_MORE,Tags);

						if(Node->Special.List.TextAttr)
						{
							NewGadget.ng_TextAttr = Handle->TextAttr;

							if(LocalFont)
							{
								CloseFont(LocalFont);

								Permit();
							}
						}

						break;
					}

					case MX_KIND:

						NewGadget.ng_Height = Handle->GlyphHeight;

						Gadget = CreateGadget(MX_KIND,Handle->Previous,&NewGadget,
							GTMX_Labels,	Node->Special.Radio.Choices,
							GTMX_Active,	Node->Current,
							GTMX_Spacing,	Handle->InterHeight,
							GTMX_Scaled,	TRUE,

							Node->Label ? GTMX_TitlePlace : TAG_IGNORE,  Node->Special.Radio.TitlePlace,
						TAG_MORE,Tags);

						break;

					case TEXT_KIND:

						if(Node->Special.Text.UsePicker)
							NewGadget.ng_Width -= LTP_GetPickerSize(Handle);

						if(Gadget = CreateGadget(TEXT_KIND,Handle->Previous,&NewGadget,
							GTTX_Border,		Node->Special.Text.Border,
							GTTX_Justification,	Node->Special.Text.Justification,
							GTTX_Clipped,		TRUE,

							Node->Special.Text.Text ? GTTX_Text : TAG_IGNORE,Node->Special.Text.Text,
							Node->Special.Text.FrontPen != -1 ? GTTX_FrontPen : TAG_IGNORE,Node->Special.Text.FrontPen,
							Node->Special.Text.BackPen != -1 ? GTTX_BackPen : TAG_IGNORE,Node->Special.Text.BackPen,
						TAG_MORE,Tags))
						{
							if(Node->Special.Text.UsePicker)
							{
								Handle->Previous = Gadget;

								Extra = LTP_CreateExtraObject(Handle,Node,Gadget,&NewGadget,IMAGECLASS_PICKER,0);

								if(!Extra)
									Gadget = NULL;

								Node->Special.Text.Picker = Extra;
							}
						}

						break;

					#ifdef DO_GAUGE_KIND
					{
						case GAUGE_KIND:

							NewGadget.ng_Height -= Handle->InterHeight + Handle->GlyphHeight + 2;

							Gadget = CreateGadgetA(TEXT_KIND,Handle->Previous,&NewGadget,NULL);

							break;
					}
					#endif

					#ifdef DO_TAPEDECK_KIND
					{
						case TAPEDECK_KIND:

							if(Node->Special.TapeDeck.ButtonType >= TDBT_BACKWARD && Node->Special.TapeDeck.ButtonType < TDBTLAST)
							{
								LONG	ImageType;
								LONG	Activation,
										Flags;

								if(Node->Special.TapeDeck.Toggle)
									Activation = GACT_TOGGLESELECT | GACT_IMMEDIATE;
								else
								{
									if(Node->Special.TapeDeck.Tick)
									{
										Activation = GACT_RELVERIFY | GACT_IMMEDIATE;

										Handle->IDCMP |= IDCMP_GADGETDOWN | IDCMP_GADGETUP | IDCMP_INTUITICKS;
									}
									else
										Activation = GACT_RELVERIFY;
								}

								if(Node->Current)
									Flags = GFLG_GADGIMAGE | GFLG_GADGHIMAGE | GFLG_SELECTED;
								else
									Flags = GFLG_GADGIMAGE | GFLG_GADGHIMAGE;

								switch(Node->Special.TapeDeck.ButtonType)
								{
									case TDBT_BACKWARD:

										ImageType = IMAGECLASS_BACKWARD;
										break;

									case TDBT_FORWARD:

										ImageType = IMAGECLASS_FORWARD;
										break;

									case TDBT_PREVIOUS:

										ImageType = IMAGECLASS_PREVIOUS;
										break;

									case TDBT_NEXT:

										ImageType = IMAGECLASS_NEXT;
										break;

									case TDBT_RECORD:

										ImageType = IMAGECLASS_RECORD;
										break;

									case TDBT_PLAY:

										ImageType = IMAGECLASS_PLAY;
										break;

									case TDBT_STOP:

										ImageType = IMAGECLASS_STOP;
										break;

									case TDBT_PAUSE:

										ImageType = IMAGECLASS_PAUSE;
										break;

									case TDBT_REWIND:

										ImageType = IMAGECLASS_REWIND;
										break;

									case TDBT_EJECT:

										ImageType = IMAGECLASS_EJECT;
										break;

									default:	/* Should never happen! */

										ImageType = -1;
										break;
								}

								if(ImageType != -1)
								{
									if(Node->Special.TapeDeck.ButtonImage = NewObject(LTP_ImageClass,NULL,
										IIA_ImageType,	ImageType,
										IIA_GlyphWidth, Node->Special.TapeDeck.ButtonWidth,
										IIA_GlyphHeight,Node->Special.TapeDeck.ButtonHeight,
										IA_Width,		NewGadget.ng_Width,
										IA_Height,		NewGadget.ng_Height,
									TAG_DONE))
									{
										NewGadget.ng_GadgetText	= "";
										NewGadget.ng_Flags		= 0;

										if(Gadget = CreateGadgetA(GENERIC_KIND,Handle->Previous,&NewGadget,NULL))
										{
											Gadget->GadgetType		|= GTYP_BOOLGADGET;
											Gadget->Flags 			|= Flags;
											Gadget->Activation		|= Activation;

											Gadget->GadgetRender	 = Node->Special.TapeDeck.ButtonImage;
											Gadget->SelectRender	 = Node->Special.TapeDeck.ButtonImage;

											if(Node->Disabled)
												Gadget->Flags |= GFLG_DISABLED;
										}
									}
								}
							}

							break;
					}
					#endif	/* DO_TAPEDECK_KIND */

					case NUMBER_KIND:

						Gadget = CreateGadget(NUMBER_KIND,Handle->Previous,&NewGadget,
							GTNM_Number,		Node->Special.Number.Number,
							GTNM_Border,		Node->Special.Number.Border,
							GTNM_Justification,	Node->Special.Number.Justification,
							GTNM_Border,		Node->Special.Number.Border,
							GTNM_Format,		Node->Special.Number.Format ? Node->Special.Number.Format : (STRPTR)"%ld",
							GTNM_Clipped,		TRUE,
							GTNM_MaxNumberLen,	Node->Special.Number.MaxNumberLen,

							Node->Special.Number.FrontPen != -1 ? GTNM_FrontPen : TAG_IGNORE,Node->Special.Number.FrontPen,
							Node->Special.Number.BackPen != -1 ? GTNM_BackPen : TAG_IGNORE,Node->Special.Number.BackPen,
						TAG_MORE,Tags);

						break;

					case CYCLE_KIND:

						Gadget = CreateGadget(CYCLE_KIND,Handle->Previous,&NewGadget,
							GTCY_Labels,		Node->Special.Cycle.Choices,
							GTCY_Active,		Node->Current,
						TAG_MORE,Tags);

						break;

					case PALETTE_KIND:

						if(Node->Special.Palette.UsePicker)
						{
							NewGadget.ng_Width = Node->Special.Palette.IndicatorWidth;

							if(Gadget = CreateGadget(TEXT_KIND,Handle->Previous,&NewGadget,TAG_MORE,Tags))
							{
								Handle->Previous = Gadget;

								Extra = LTP_CreateExtraObject(Handle,Node,Gadget,&NewGadget,IMAGECLASS_PICKER,0);

								if(!Extra)
									Gadget = NULL;

								Node->Special.Palette.Picker = Extra;
							}
						}
						else
						{
							Gadget = CreateGadget(PALETTE_KIND,Handle->Previous,&NewGadget,
								GTPA_Depth,				Node->Special.Palette.Depth,
								GTPA_Color,				Node->Current,
								GTPA_ColorOffset,		Node->Min,
								GTPA_IndicatorWidth,	max(10,Node->Width / (Node->Max - Node->Min + 1)),
								GTPA_IndicatorHeight,	Node->Height,

								Node->Special.Palette.ColourTable ? GTPA_ColorTable : TAG_IGNORE,Node->Special.Palette.ColourTable,
							TAG_MORE,Tags);
						}

						break;

					case SLIDER_KIND:

						Gadget = CreateGadget(SLIDER_KIND,Handle->Previous,&NewGadget,
							GTSL_Min,			Node->Min,
							GTSL_Max,			Node->Max,
							GTSL_Level,			Node->Current,
							GTSL_MaxLevelLen,	Node->Special.Slider.MaxLevelLen,
							GTSL_LevelPlace,	Node->Special.Slider.LevelPlace,
							GTSL_LevelFormat,	Node->Special.Slider.LevelFormat ? Node->Special.Slider.LevelFormat : (STRPTR)"%ld",
							GA_RelVerify,		TRUE,

							V40 ? GTSL_MaxPixelLen : TAG_IGNORE,						Node->Special.Slider.LevelWidth,
							V40 ? GTSL_Justification : TAG_IGNORE,						GTJ_RIGHT,
							Node->Special.Slider.DispFunc ? GTSL_DispFunc : TAG_IGNORE,	Node->Special.Slider.DispFunc,
						TAG_MORE,Tags);

						break;

					case SCROLLER_KIND:

						if(Node->Special.Scroller.FullSize)
						{
							if(Node->Special.Scroller.Vertical)
							{
								NewGadget.ng_TopEdge	= Node->Special.Scroller.Parent->Top;
								NewGadget.ng_Height		= Node->Special.Scroller.Parent->Height;
							}
							else
							{
								NewGadget.ng_LeftEdge	= Node->Special.Scroller.Parent->Left;
								NewGadget.ng_Width		= Node->Special.Scroller.Parent->Width;
							}
						}

						Gadget = CreateGadget(SCROLLER_KIND,Handle->Previous,&NewGadget,
							GA_RelVerify,	Node->Special.Scroller.RelVerify,
							GA_Immediate,	Node->Special.Scroller.Immediate,
							GTSC_Top,		Node->Current,
							GTSC_Total,		Node->Max,
							GTSC_Visible,	Node->Special.Scroller.Visible,
							PGA_Freedom,	Node->Special.Scroller.Vertical ? LORIENT_VERT : LORIENT_HORIZ,

							Node->Special.Scroller.Arrows ? GTSC_Arrows : TAG_IGNORE,Node->Special.Scroller.ArrowSize,
						TAG_MORE,Tags);

						break;

					case FRACTION_KIND:
					case STRING_KIND:
					{
						STRPTR String;

						String = Node->Special.String.String;

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

							if(Node->Special.String.IncrementerHook)
							{
								NewGadget.ng_Width -= 4 + Handle->GlyphWidth + 4;
								NewGadget.ng_Width -= 4 + Handle->GlyphWidth + 4;
							}
						}

						if(Node->Special.String.LinkID != -1)
						{
							ObjectNode *ListView = LTP_FindNode(Handle,Node->Special.String.LinkID);

							if(ListView)
							{
								NewGadget.ng_LeftEdge	= ListView->Left;
								NewGadget.ng_TopEdge	= ListView->Top + ListView->Height;
								NewGadget.ng_Width		= ListView->Width;
							}
						}

						if(Node->Special.String.UsePicker)
							NewGadget.ng_Width -= LTP_GetPickerSize(Handle);

						if(Gadget = CreateGadget(STRING_KIND,Handle->Previous,&NewGadget,
							GTST_String,			String,
							GTST_MaxChars,			Node->Special.String.MaxChars,
							GTST_EditHook,			Node->Special.String.EditHook ? Node->Special.String.EditHook : Handle->StandardEditHook,
							GA_Immediate,			TRUE,
							STRINGA_Justification,	Node->Special.String.Justification,
						TAG_MORE,Tags))
						{
							if(!V39)
								Gadget->Activation |= GACT_IMMEDIATE;

							if(Node->Special.String.UsePicker)
							{
								Handle->Previous = Gadget;

								Extra = LTP_CreateExtraObject(Handle,Node,Gadget,&NewGadget,IMAGECLASS_PICKER,0);

								if(!Extra)
									Gadget = NULL;

								Node->Special.String.Picker = Extra;
							}
							else
							{
								if(Node->Type == FRACTION_KIND && Node->Special.String.IncrementerHook)
								{
									Handle->Previous = Gadget;

									if(Node->Special.String.LeftIncrementer = LTP_CreateExtraObject(Handle,Node,Gadget,&NewGadget,IMAGECLASS_LEFTINCREMENTER,-1))
									{
										Handle->Previous = Node->Special.String.LeftIncrementer;

										if(Node->Special.String.RightIncrementer = LTP_CreateExtraObject(Handle,Node,Gadget,&NewGadget,IMAGECLASS_RIGHTINCREMENTER,1))
											Extra = Node->Special.String.RightIncrementer;
										else
											Gadget = NULL;
									}
									else
										Gadget = NULL;
								}
							}
						}

						break;
					}

					#ifdef DO_PASSWORD_KIND
					{
						case PASSWORD_KIND:
						{
							LONG Len;

							if(Len = strlen(Node->Special.String.RealString))
								memset(Node->Special.String.Original,'·',Len);

							Node->Special.String.Original[Len] = 0;

							if(Gadget = CreateGadget(STRING_KIND,Handle->Previous,&NewGadget,
								GTST_String,			Node->Special.String.Original,
								GTST_MaxChars,			Node->Special.String.MaxChars,
								GTST_EditHook,			&Handle->PasswordEditHook,
								GA_Immediate,			TRUE,
								STRINGA_Justification,	Node->Special.String.Justification,
							TAG_MORE,Tags))
							{
								if(!V39)
									Gadget->Activation |= GACT_IMMEDIATE;
							}

							break;
						}
					}
					#endif

					case INTEGER_KIND:

						if(Node->Special.Integer.UseIncrementers)
						{
							NewGadget.ng_Width -= 4 + Handle->GlyphWidth + 4;
							NewGadget.ng_Width -= 4 + Handle->GlyphWidth + 4;

							if(Node->Special.Integer.IncrementerHook)
							{
								LONG Value;

								Value = (LONG)CallHookPkt(Node->Special.Integer.IncrementerHook,(APTR)(IPTR)Node->Special.Integer.Number,(APTR)(IPTR)INCREMENTERMSG_INITIAL);

								if(Value < Node->Min)
									Value = Node->Min;
								else
								{
									if(Value > Node->Max)
										Value = Node->Max;
								}

								Node->Special.Integer.Number = Value;
							}
						}

						#ifndef DO_HEXHOOK
						{
							Gadget = CreateGadget(INTEGER_KIND,Handle->Previous,&NewGadget,
								GTIN_Number,			Node->Special.Integer.Number,
								GTIN_MaxChars,			Node->Special.Integer.MaxChars,
								GTIN_EditHook,			Node->Special.Integer.EditHook ? Node->Special.Integer.EditHook : Handle->StandardEditHook,
								GA_Immediate,			TRUE,
								STRINGA_Justification,	Node->Special.Integer.Justification,
							TAG_MORE,Tags);
						}
						#else
						{
							if(Node->Special.Integer.EditHook)
							{
								Gadget = CreateGadget(INTEGER_KIND,Handle->Previous,&NewGadget,
									GTIN_Number,			Node->Special.Integer.Number,
									GTIN_MaxChars,			Node->Special.Integer.MaxChars,
									GTIN_EditHook,			Node->Special.Integer.EditHook,
									GA_Immediate,			TRUE,
									STRINGA_Justification,	Node->Special.Integer.Justification,
								TAG_MORE,Tags);
							}
							else
							{
								UBYTE LocalBuffer[20];

								SPrintf(LocalBuffer,"%ld",Node->Special.Integer.Number);

								Gadget = CreateGadget(STRING_KIND,Handle->Previous,&NewGadget,
									GTST_String,			LocalBuffer,
									GTST_MaxChars,			Node->Special.Integer.MaxChars,
									GTST_EditHook,			&Handle->HexEditHook,
									GA_Immediate,			TRUE,
									STRINGA_Justification,	Node->Special.Integer.Justification,
								TAG_MORE,Tags);
							}

							if(Gadget)
							{
								struct StringInfo *StringInfo = (struct StringInfo *)Gadget->SpecialInfo;

								StringInfo->LongInt = Node->Special.Integer.Number;
							}
						}
						#endif /* DO_HEXHOOK */

						if(Gadget)
						{
							if(!V39)
								Gadget->Activation |= GACT_IMMEDIATE;

							if(Node->Special.Integer.UseIncrementers)
							{
								Handle->Previous = Gadget;

								if(Node->Special.Integer.LeftIncrementer = LTP_CreateExtraObject(Handle,Node,Gadget,&NewGadget,IMAGECLASS_LEFTINCREMENTER,-1))
								{
									Handle->Previous = Node->Special.Integer.LeftIncrementer;

									if(Node->Special.Integer.RightIncrementer = LTP_CreateExtraObject(Handle,Node,Gadget,&NewGadget,IMAGECLASS_RIGHTINCREMENTER,1))
										Extra = Node->Special.Integer.RightIncrementer;
									else
										Gadget = NULL;
								}
								else
									Gadget = NULL;
							}
						}

						break;
				}

				if(Gadget != NULL)
				{
					Handle->GadgetArray[Handle->Index++] = Gadget;

					Node->Host = Gadget;

					#ifdef DO_BOOPSI_KIND
					{
						if(Node->Type == BOOPSI_KIND || Node->Type == POPUP_KIND || Node->Type == TAB_KIND || Node->Type == LEVEL_KIND)
						{
							Handle->BOOPSIPrevious = (Object *)Gadget;

							if(!Handle->BOOPSIList)
								Handle->BOOPSIList = (Object *)Gadget;
						}
						else
						{
							if(Extra)
								Handle->Previous = Extra;
							else
								Handle->Previous = Gadget;
						}
					}
					#else
					{
						if(Extra)
							Handle->Previous = Extra;
						else
							Handle->Previous = Gadget;
					}
					#endif	/* DO_BOOPSI_KIND */
				}
				else
				{
					Handle->Failed = TRUE;

					return;	/* Note: must return since we are in a loop that
							 *       creates all gadgets for the current page.
							 */
				}
			}
		}
	}
}
