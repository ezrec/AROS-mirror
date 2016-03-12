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
LTP_GetPickerWidth(
	LONG	fontHeight,
	LONG	aspectX,
	LONG	aspectY)
{
	LONG margin,width;

	width = (fontHeight * aspectY) / aspectX;

	if(width < 8)
		width = 8;

	margin = 2 + (width + 15) / 16;

	return((LONG)(margin + (((115 * width) / 150) & ~1) + 1 + margin));
}

LONG
LTP_GetPickerSize(LayoutHandle *Handle)
{
	return(LTP_GetPickerWidth(Handle->TextAttr->ta_YSize,Handle->AspectX,Handle->AspectY));
}

VOID
LTP_DetermineSize(LayoutHandle *Handle,ObjectNode *Node)
{
	if(!Handle->Failed)
	{
		LONG i,Len,Width,Max,Plus;

		if(Node->Label)
		{
			LONG LabelWidth = LT_LabelWidth(Handle,Node->Label);

			if(Node->LabelChars * Handle->GlyphWidth > LabelWidth)
				LabelWidth = Node->LabelChars * Handle->GlyphWidth;

			Node->LabelWidth = LabelWidth;
		}
		else
			Node->LabelWidth = 0;

		switch(Node->Type)
		{
			#ifdef DO_BOOPSI_KIND
			{
				case BOOPSI_KIND:

					if(Node->Special.BOOPSI.ExactWidth)
						Node->Width = Node->Special.BOOPSI.ExactWidth;
					else
						Node->Width = Node->Chars * Handle->GlyphWidth;

					if(Node->Special.BOOPSI.ExactHeight)
						Node->Height = Node->Special.BOOPSI.ExactHeight;
					else
						Node->Height = Node->Lines * Handle->GlyphHeight;

					if(Node->Special.BOOPSI.RelFontHeight)
						Node->Height = Handle->GlyphHeight + Node->Special.BOOPSI.RelFontHeight;

					break;
			}
			#endif	/* DO_BOOPSI_KIND */

			#ifdef DO_LEVEL_KIND
			{
				case LEVEL_KIND:

					if(Node->Special.Level.Freedom == FREEHORIZ)
					{
						Node->Width		= 6 + Node->Chars * Handle->GlyphWidth + 6;
						Node->Height	= LTP_QuerySliderSize(Handle->DrawInfo,Handle->GlyphHeight,FREEHORIZ,Node->Special.Level.Ticks);
					}
					else
					{
						Node->Height	= Node->Lines * Handle->GlyphHeight;
						Node->Width		= LTP_QuerySliderSize(Handle->DrawInfo,Handle->GlyphHeight,FREEVERT,Node->Special.Level.Ticks);
					}

					if(Node->DefaultSize && Node->Height < 3 + Handle->GlyphHeight + 3)
						Node->Height = 3 + Handle->GlyphHeight + 3;

					LTP_GetStorage(Node);

					if(Node->Current > Node->Max)
					{
						Node->Current = Node->Max;

						LTP_PutStorage(Node);
					}

					if(Node->Current < Node->Min)
					{
						Node->Current = Node->Min;

						LTP_PutStorage(Node);
					}

					if(!Node->Special.Level.MaxLevelWidth || Handle->Rescaled)
					{
						LTP_LevelWidth(Handle,Node->Special.Level.LevelFormat,Node->Special.Level.DispFunc,Node->Min,Node->Max,&Node->Special.Level.MaxLevelWidth,NULL,Node->Special.Level.FullLevelCheck);

						Node->Special.Level.MaxLevelWidth += TextLength(&Handle->RPort," ",1);
					}

					if(!Node->Special.Level.Plus)
						Node->Special.Level.Plus = Node->Min;

					if(Node->LabelChars * Handle->GlyphWidth > Node->LabelWidth)
						Node->LabelWidth = Node->LabelChars * Handle->GlyphWidth;

					if(Node->Special.Level.LevelPlace == PLACETEXT_LEFT &&
					   Node->LabelPlace == PLACE_LEFT &&
					   Node->Special.Level.LevelFormat != NULL)
					{
						Node->LabelWidth += Node->Special.Level.MaxLevelWidth;
					}

					break;
			}
			#endif	/* DO_LEVEL_KIND */

			case GROUP_KIND:

				LTP_LayoutGroup(Handle,Node);

				break;

			case XBAR_KIND:

				Node->Width		= Handle->GlyphWidth;
				Node->Height	= 6;

				break;

			case YBAR_KIND:

				Node->Width		= 6;
				Node->Height	= Handle->GlyphHeight;

				if(Node->DefaultSize)
					Node->Height = 3 + Handle->GlyphHeight + 3;

				break;

			case IMAGE_KIND:

				if(Node->Special.Image.Image != NULL)
				{
					Node->Width		= Node->Special.Image.Image->Width;
					Node->Height	= Node->Special.Image.Image->Height;
				}
				else if (Node->Special.Image.BitMap != NULL)
				{
					Node->Width		= Node->Special.Image.BitMapWidth;
					Node->Height	= Node->Special.Image.BitMapHeight;
				}
				else
				{
					Handle->Failed = TRUE;
				}

				break;

			case FRAME_KIND:
			{
				LONG Width	= Node->Special.Frame.InnerWidth;
				LONG Height	= Node->Special.Frame.InnerHeight;

				if(Node->Special.Frame.PlusWidth > 0 || Node->Special.Frame.PlusHeight > 0)
				{
					Width	+= Node->Special.Frame.PlusWidth * Handle->GlyphWidth;
					Height	+= Node->Special.Frame.PlusHeight * Handle->GlyphHeight;
				}

				if(Node->Special.Frame.DrawBox)
				{
					Node->Width		= 4 + Width + 4;
					Node->Height	= 2 + Height + 2;
				}
				else
				{
					Node->Width		= Width;
					Node->Height	= Height;
				}

				break;
			}

			case BOX_KIND:

				Node->LabelWidth = 0;

				if(Node->Special.Box.Labels)
				{
					for(i = 0 ; i < Node->Lines ; i++)
					{
						if((Width = TextLength(&Handle->RPort,Node->Special.Box.Labels[i],strlen(Node->Special.Box.Labels[i]))) > Node->LabelWidth)
							Node->LabelWidth = Width;
					}
				}

				if(Node->LabelChars)
				{
					if(Node->LabelChars * Handle->GlyphWidth > Node->LabelWidth)
						Node->LabelWidth = Node->LabelChars * Handle->GlyphWidth;
					else
						Node->LabelWidth = ((Node->LabelWidth + Node->LabelChars - 1) / Node->LabelChars) * Node->LabelChars;
				}

				Max = Node->Chars;

				if(Node->Special.Box.Lines)
				{
					LONG MaxWidth = 0;

					for(i = 0 ; i < Node->Lines ; i++)
					{
						if(Node->Special.Box.Lines[i])
						{
							if(Len = strlen(Node->Special.Box.Lines[i]))
							{
								if((Width = TextLength(&Handle->RPort,Node->Special.Box.Lines[i],Len)) > MaxWidth)
									MaxWidth = Width;
							}
						}
					}

					if(Max < (MaxWidth + Handle->GlyphWidth - 1) / Handle->GlyphWidth)
						Max = (MaxWidth + Handle->GlyphWidth - 1) / Handle->GlyphWidth;
				}

				Node->Height	= 2 + Node->Lines * (Handle->GlyphHeight + Node->Special.Box.Spacing) + 2;
				Node->Width		= 4 + Max * Handle->GlyphWidth + 4;

				if(Node->DefaultSize)
					Node->Height = 3 + Node->Lines * (Handle->GlyphHeight + Node->Special.Box.Spacing) + 3;

				Node->Height -= Node->Special.Box.Spacing;

				break;

			case BLANK_KIND:

				Node->Width			= Handle->GlyphWidth * max(1,Node->Chars);
				Node->Height		= Handle->GlyphHeight;
				Node->Label			= "";
				Node->LabelWidth	= 0;
				Node->LabelPlace	= PLACE_IN;

				if(Node->DefaultSize)
					Node->Height = 3 + Handle->GlyphHeight + 3;

				break;

			case BUTTON_KIND:

				if(!Node->Special.Button.Lines && Node->Label)
				{
					LONG i,Len = strlen(Node->Label),Count = 0;

					for(i = 0 ; i < Len ; i++)
					{
						if(Node->Label[i] == '\n')
							Count++;
					}

					if(Count)
					{
						STRPTR *Lines;

						if(Lines = (STRPTR *)LTP_Alloc(Handle,(Count + 2) * sizeof(STRPTR) + Len + 1))
						{
							STRPTR String = (STRPTR)(&Lines[Count + 2]);

							strcpy(String,Node->Label);

							Node->Special.Button.Lines = Lines;

							if(Node->Special.Button.KeyStroke)
								Node->Special.Button.KeyStroke = &String[((IPTR)Node->Special.Button.KeyStroke) - ((IPTR)Node->Label)];

							do
							{
								*Lines++ = String;

								for(i = 0 ; String[i] ; i++)
								{
									if(String[i] == '\n')
									{
										String[i] = 0;

										String = &String[i + 1];

										break;
									}
								}
							}
							while(Count--);
						}
						else
							break;

						LTP_ReplaceLabelShortcut(Handle,Node);
					}
				}

				if(Node->Special.Button.Lines)
				{
					STRPTR	*Index = Node->Special.Button.Lines;
					LONG	 Count = 0,Width,MaxWidth = 0,Height;

					while(*Index)
					{
						Count++;

						if((Width = LT_LabelWidth(Handle,*Index)) > MaxWidth)
							MaxWidth = Width;

						Index++;
					}

					Node->Special.Button.LineCount = Count;

					if(Node->Chars * Handle->GlyphWidth > MaxWidth)
						MaxWidth = Node->Chars * Handle->GlyphWidth;

					if(Node->Lines > Count)
						Count = Node->Lines;

					Height = Count * Handle->GlyphHeight;

					if(Node->Special.Button.ExtraFat)
					{
						Node->Width		= 6 + Handle->GlyphWidth + MaxWidth + Handle->GlyphWidth + 6;
						Node->Height	= 2 + (2 * Height + Handle->GlyphHeight) / 2 + 2;
					}
					else
					{
						if(Node->Special.Button.Smaller)
						{
							Node->Width		= 4 + MaxWidth + 4;
							Node->Height	= 2 + Height + 2;
						}
						else
						{
							Node->Width		= 6 + MaxWidth + 6;
							Node->Height	= 3 + Height + 3;
						}
					}

					Node->Label = "";
				}
				else
				{
					if(Node->Chars * Handle->GlyphWidth > Node->LabelWidth)
						Node->Width = Node->Chars * Handle->GlyphWidth;
					else
						Node->Width = Node->LabelWidth;

					if(Node->Lines)
					{
						if(Node->Special.Button.ExtraFat)
						{
							Node->Width		= 6 + Handle->GlyphWidth + Node->Width + Handle->GlyphWidth + 6;
							Node->Height	= 2 + ((Node->Lines + 1) * Handle->GlyphHeight) / 2 + 2;
						}
						else
						{
							if(Node->Special.Button.Smaller)
							{
								Node->Width		= 4 + Node->Width + 4;
								Node->Height	= 2 + (Node->Lines * Handle->GlyphHeight) + 2;
							}
							else
							{
								Node->Width		= 6 + Node->Width + 6;
								Node->Height	= 3 + (Node->Lines * Handle->GlyphHeight) + 3;
							}
						}
					}
					else
					{
						if(Node->Special.Button.ExtraFat)
						{
							Node->Width		= 6 + Handle->GlyphWidth + Node->Width + Handle->GlyphWidth + 6;
							Node->Height	= 2 + (3 * Handle->GlyphHeight) / 2 + 2;
						}
						else
						{
							if(Node->Special.Button.Smaller)
							{
								Node->Width		= 4 + Node->Width + 4;
								Node->Height	= 2 + Handle->GlyphHeight + 2;
							}
							else
							{
								Node->Width		= 6 + Node->Width + 6;
								Node->Height	= 3 + Handle->GlyphHeight + 3;
							}
						}
					}
				}

				Node->LabelWidth = 0;
				Node->LabelPlace = PLACE_IN;

				break;

			case CHECKBOX_KIND:

				Node->Width = Node->Height = Handle->RPort.TxBaseline + ((Handle->RPort.TxBaseline + 2) / 3) * 2;

				if(Node->Height < CHECKBOX_HEIGHT)
					Node->Height = CHECKBOX_HEIGHT;

				if(Node->DefaultSize)
					Node->Height = 3 + Handle->GlyphHeight + 3;

				if(!V39)
				{
					if(Node->Width < CHECKBOX_WIDTH)
						Node->Width = CHECKBOX_WIDTH;
				}
				else
					Node->Width = (Node->Height * Handle->AspectY) / Handle->AspectX;

				break;

			case LISTVIEW_KIND:
			{
				struct TextFont	*Font,*OldFont;
				LONG			 GlyphWidth;

				if(Node->Lines < 1)
				{
					Handle->Failed = TRUE;

					return;
				}

				if(Node->Special.List.TextAttr)
				{
					if(!(Font = LTP_OpenFont(Node->Special.List.TextAttr)))
					{
						Handle->Failed = TRUE;

						return;
					}

						// Is this a fixed-width font?

					if(Font->tf_Flags & FPF_PROPORTIONAL)
					{
							// We don't want it

						CloseFont(Font);

						Handle->Failed = TRUE;

						return;
					}

					OldFont = Handle->RPort.Font;

					SetFont(&Handle->RPort,Font);

					GlyphWidth = Handle->RPort.TxWidth;

					Node->Special.List.FixedGlyphWidth	= Handle->RPort.TxWidth;
					Node->Special.List.FixedGlyphHeight	= Handle->GlyphHeight;
				}
				else
				{
					Font		= NULL;
					OldFont		= NULL;	/* For the sake of the compiler, make sure this is initialized */
					GlyphWidth	= Handle->GlyphWidth;
				}

				i		= 0;
				Max		= 0;
				Width	= Node->Chars * GlyphWidth;

				if((IPTR)Node->Special.List.Labels == 0xFFFFFFFF)
					Node->Min = Node->Max = -1;
				else
				{
					if(Node->Special.List.Labels)
					{
						struct Node *Item;

						SCANLIST(Node->Special.List.Labels,Item)
						{
							i++;

							if((Len = TextLength(&Handle->RPort,Item->ln_Name,strlen(Item->ln_Name))) > Max)
								Max = Len;
						}
					}

					if(Node->Special.List.IgnoreListContents)
						Max = 0;

					Node->Max = i - 1;
				}

				if(Max > Width && !Node->Special.List.SizeLocked)
				{
					Width = Max;

					Node->Chars = (Width + GlyphWidth - 1) / GlyphWidth;

					Width = Node->Chars * GlyphWidth;
				}

				if(Node->Special.List.LockSize)
					Node->Special.List.SizeLocked = TRUE;

				if(!V39)
					Node->Width = (4 + Width + 4) + (2 + 2 * Handle->GlyphWidth + 2) + 8;
				else
					Node->Width = (4 + Width + 4) + (2 + 2 * Handle->GlyphWidth + 2);

				Node->Height = 2 + Node->Lines * Handle->GlyphHeight + 2;

				if(Node->Special.List.AdjustForString)
					Node->Height += Handle->InterHeight + 3 + Handle->GlyphHeight + 3;

				if(Node->Special.List.ReadOnly)
				{
					if(Node->Special.List.LinkID != -1)
					{
						if(!V39)
							Node->Height += 2 + Handle->GlyphHeight + 2;
					}
				}
				else
				{
					if(Node->Special.List.LinkID == NIL_LINK)
					{
						if(!V39)
							Node->Height += 2 + Handle->GlyphHeight + 2;
					}
					else
					{
						if(Node->Special.List.LinkID != -1 && Node->Special.List.LinkID != NIL_LINK)
							Node->Height += 3 + Handle->GlyphHeight + 3;
					}
				}

					// Switch back to the window font

				if(Font)
				{
					SetFont(&Handle->RPort,OldFont);

					CloseFont(Font);
				}

				if(Node->Special.List.ExtraLabels)
				{
					for(i = Max = 0 ; Node->Special.List.ExtraLabels[i] ; i++)
					{
						if((Len = TextLength(&Handle->RPort,Node->Special.List.ExtraLabels[i],strlen(Node->Special.List.ExtraLabels[i]))) > Max)
							Max = Len;
					}

					Node->Special.List.ExtraLabelWidth = Max;
				}

				break;
			}

			case MX_KIND:

				i	= 0;
				Max	= 0;

				while(Node->Special.Radio.Choices[i])
				{
					if((Width = TextLength(&Handle->RPort,Node->Special.Radio.Choices[i],strlen(Node->Special.Radio.Choices[i]))) > Max)
						Max = Width;

					i++;
				}

				Node->Max = i - 1;

				Node->Special.Radio.LabelWidth = Max;

				if(Node->LabelChars * Handle->GlyphWidth > Node->LabelWidth)
					Node->LabelWidth = Node->LabelChars * Handle->GlyphWidth;

				Node->Height = max(MX_HEIGHT,Handle->GlyphHeight);

				if(!V39)
					Node->Width = MX_WIDTH;
				else
				{
					Node->Width = (Node->Height * Handle->AspectY) / Handle->AspectX;

					if(Node->Height < MX_HEIGHT)
					{
						Node->Height = MX_HEIGHT;
						Node->Width  = (Node->Height * Handle->AspectX) / Handle->AspectY;
					}
				}

				Node->Lines  = i;
				Node->Height = Node->Lines * (Node->Height + Handle->InterHeight) - Handle->InterHeight;

				break;

			#ifdef DO_GAUGE_KIND
			{
				case GAUGE_KIND:

					Width = TextLength(&Handle->RPort,"0%100%",6) + 2 * Handle->GlyphWidth;

					if(Handle->GlyphWidth * Node->Chars > Width)
						Width = Handle->GlyphWidth * Node->Chars;

					Node->Width = 6 + Width + 6;

					if(Node->Special.Gauge.NoTicks)
						Node->Height = 3 + Handle->GlyphHeight + 3;
					else
						Node->Height = 3 + Handle->GlyphHeight + 2 + Handle->InterHeight + Handle->GlyphHeight + 3;

					if(Node->Special.Gauge.Discrete)
					{
						Width = ((Node->Width - 6 - 6) + 9) / 10;

						Node->Width = 2 + Width * 10 + 2;
					}

					break;
			}
			#endif

			#ifdef DO_TAPEDECK_KIND
			{
				case TAPEDECK_KIND:
				{
					LONG Width,Height,OrigWidth,Attempt = 4;

					Width = TextLength(&Handle->RPort,"AA",2);

					if(Width < 2 * Handle->GlyphWidth)
						Width = 2 * Handle->GlyphWidth;

					if(Width < 2 * Handle->GlyphHeight)
						Width = 2 * Handle->GlyphHeight;

					if(Node->Special.TapeDeck.Smaller)
						Width = (3 * Width) / 4;

					if(Width & 1)
						Width++;

					OrigWidth = Width;

					do
					{
						Width	= 1 + ((((OrigWidth + 5) / 6) * 6) & ~1);
						Height	= (Handle->AspectX * Width) / (Handle->AspectY * 2);

						OrigWidth += 2;
						Attempt--;
					}
					while(Height < 6 && Attempt > 0);

					if(!(Height & 1))
						Height++;

					if(Node->Special.TapeDeck.ButtonType == TDBT_BACKWARD || Node->Special.TapeDeck.ButtonType == TDBT_FORWARD)
						Node->Special.TapeDeck.ButtonWidth = Width;
					else
						Node->Special.TapeDeck.ButtonWidth = Width / 2;

					Node->Special.TapeDeck.ButtonHeight = Height;

					if(Handle->GlyphWidth * Node->Chars > Width)
						Width = Handle->GlyphWidth * Node->Chars;

					Node->Width		= 6 + Width + 6;
					Node->Height	= 3 + Height + 3;

					break;
				}
			}
			#endif	/* DO_TAPEDECK_KIND */

			case CYCLE_KIND:

					// NOTE: This does not include the width of the
					//       cycle glyph which is by default 26 pixels
					//       wide.

				Node->Width = 6 + Node->Chars * Handle->GlyphWidth + 6;

				Width = 0;

				if(Node->Special.Cycle.Choices)
				{
					for(i = 0 ; Node->Special.Cycle.Choices[i] ; i++)
					{
						if((Len = TextLength(&Handle->RPort,Node->Special.Cycle.Choices[i],strlen(Node->Special.Cycle.Choices[i]))) > Width)
							Width = Len;
					}

					Node->Max = i - 1;
				}

				Max = 6 + 20 + Width + 6;	/* Let's see if the interior is large enough for the glyph. */

				if(Node->Width < Max)
					Node->Width = Max;

					// Add the remainder of the cycle glyph

				Node->Height = 3 + Handle->GlyphHeight + 3;

				break;

			#ifdef DO_POPUP_KIND
			{
				case POPUP_KIND:

						// NOTE: This is the number of pixels the popup glyph
						//       will need. It will not enter the calculation
						//       until the maximum size of the hit box is
						//       calculated.

					Plus = (4 + ((TextLength(&Handle->RPort,"M",1) & ~1) + 1) + 2 + 4) + (Handle->GlyphHeight * Handle->DrawInfo->dri_Resolution.Y) / Handle->DrawInfo->dri_Resolution.X + 2;

					if(Plus < 20)
						Plus = 20;

					Node->Width = 6 + Node->Chars * Handle->GlyphWidth + 6;

					Width = 0;

					if(Node->Special.Popup.Choices)
					{
						for(i = 0 ; Node->Special.Popup.Choices[i] ; i++)
						{
							Len = TextLength(&Handle->RPort,
							                 Node->Special.Popup.Choices[i],
							                 strlen(Node->Special.Popup.Choices[i]));
							if(Len > Width)
								Width = Len;
						}

						Node->Max = i - 1;
					}

					Max = 6 + Plus + Width + 6;	/* Let's see if the interior is large enough for the glyph. */

					if(Node->Width < Max)
						Node->Width = Max;

						// Add the glyph width

					Node->Height = 3 + Handle->GlyphHeight + 3;

					break;
			}
			#endif

			#ifdef DO_TAB_KIND
			{
				case TAB_KIND:

					if(!Node->Special.Tab.Choices)
						Handle->Failed = TRUE;
					else
					{
						struct IBox Box;

						Width = Node->Chars * Handle->GlyphWidth;

						for(i = 0 ; Node->Special.Tab.Choices[i] ; i++);

						Node->Max = i - 1;

						Node->Label = NULL;

						if(!LTP_ObtainTabSize(&Box,
							TIA_Labels,		Node->Special.Tab.Choices,
							TIA_DrawInfo,	Handle->DrawInfo,
							TIA_Font,		Handle->TextAttr,
							TIA_SizeType,	GDOMAIN_MINIMUM,
						TAG_DONE))
							Handle->Failed = TRUE;
						else
						{
							if(Width < Box.Width)
								Width = Box.Width;

							Node->Width		= Width;
							Node->Height	= Box.Height;
						}
					}

					break;
			}
			#endif

			case PALETTE_KIND:

				if(Node->Special.Palette.UsePicker)
				{
					Node->Height = 3 + Handle->GlyphHeight + 3;
					Node->Width = (Node->Height * Handle->AspectY) / Handle->AspectX;

					Node->Special.Palette.IndicatorWidth = Node->Width;

					Node->Width += LTP_GetPickerSize(Handle);
				}
				else
				{
					if(Node->Special.Palette.NumColours)
						Node->Max = Node->Min + Node->Special.Palette.NumColours - 1;
					else
					{
						if(!Node->Special.Palette.Depth)
							Node->Special.Palette.Depth = 1;

						Node->Special.Palette.NumColours = 1L << Node->Special.Palette.Depth;

						Node->Max = Node->Min + Node->Special.Palette.NumColours - 1;
					}

					if(Node->Special.Palette.SmallPalette)
					{
						Node->Width		= Handle->GlyphWidth * (Node->Max - Node->Min + 1) + 4;
						Node->Height	= 1 + Handle->GlyphHeight + 1;

						if((Node->Width - 4) / (Node->Max - Node->Min + 1) < 8 && Node->Height < 2 * 8)
							Node->Width = 8 * (Node->Max - Node->Min + 1) + 4;

						Node->Width += 2 * Handle->GlyphWidth;
					}
					else
					{
						Node->Width		= 2 * Handle->GlyphWidth * (Node->Max - Node->Min + 1);
						Node->Height	= 2 * Handle->GlyphHeight;
					}

					if(Node->Width < 20)
						Node->Width = 20;

					if(Node->Chars * Handle->GlyphWidth > Node->Width)
						Node->Width = Node->Chars * Handle->GlyphWidth;

					if(Node->Special.Palette.NumColours > 16)
					{
						Node->Height	*= 2;
						Node->Width		/= 2;
					}

					if(Node->Lines)
					{
						if(!Node->Chars)
							Node->Width = Handle->GlyphWidth;
						else
							Node->Width = Node->Chars * Handle->GlyphWidth;

						Node->Height = Node->Lines * Handle->GlyphHeight;
					}

					if(Node->Special.Palette.ColourTable && !Node->Special.Palette.TranslateBack)
					{
						if(Node->Special.Palette.TranslateBack = LTP_Alloc(Handle,256))
						{
							LONG i;

							for(i = Node->Min ; i <= Node->Max ; i++)
								Node->Special.Palette.TranslateBack[Node->Special.Palette.ColourTable[i]] = i;
						}
					}
				}

				break;

			case SCROLLER_KIND:

				if(Node->Special.Scroller.Vertical)
				{
					if(Node->Special.Scroller.FullSize)
						Node->Lines = 1;

					if(Node->Special.Scroller.Thin)
						Node->Width = 6 + Handle->GlyphWidth + 6;
					else
						Node->Width = 6 + (3 * Handle->GlyphWidth) / 2 + 6;

					Node->Height = 2 + Node->Lines * Handle->GlyphHeight;

					if(Node->Special.Scroller.Arrows)
					{
						LONG ScrollerHeight = (Handle->AspectX * Node->Width) / Handle->AspectY;

						if(Node->Height < 2 + Node->Lines * Handle->GlyphHeight + 2 * ScrollerHeight)
							Node->Height = 2 + Node->Lines * Handle->GlyphHeight + 2 * ScrollerHeight;

						Node->Special.Scroller.ArrowSize = ScrollerHeight;
					}
				}
				else
				{
					if(Node->Special.Scroller.FullSize)
						Node->Chars = 1;

					Node->Width  = 6 + Node->Chars * Handle->GlyphWidth + 6;
					Node->Height = 2 + Handle->GlyphHeight;

					if(Node->DefaultSize)
						Node->Height = 3 + Handle->GlyphHeight + 3;

					if(Node->Special.Scroller.Arrows)
					{
						LONG ScrollerWidth = (Handle->AspectY * Node->Height) / Handle->AspectX;

						if(Node->Width < 6 + Handle->GlyphWidth + 2 * ScrollerWidth + 6)
							Node->Width = 6 + Handle->GlyphWidth + 2 * ScrollerWidth + 6;

						Node->Special.Scroller.ArrowSize = ScrollerWidth;
					}
				}

				break;

			case SLIDER_KIND:
			{
				BOOL CheckIt;

				Node->Width		= 6 + Node->Chars * Handle->GlyphWidth + 6;
				Node->Height	= 2 + Handle->GlyphHeight + 2;

				if(Node->DefaultSize)
					Node->Height = 3 + Handle->GlyphHeight + 3;

				CheckIt = (!Node->Special.Slider.MaxLevelLen || !Node->Special.Slider.LevelWidth || Handle->Rescaled);

				if(Node->Special.Slider.LevelFormat && CheckIt)
					LTP_LevelWidth(Handle,Node->Special.Slider.LevelFormat,Node->Special.Slider.DispFunc,Node->Min,Node->Max,&Node->Special.Slider.LevelWidth,&Node->Special.Slider.MaxLevelLen,Node->Special.Slider.FullLevelCheck);

				if(Node->Special.Slider.LevelFormat && !V40)
				{
					if(Node->Special.Slider.MaxLevelLen * Handle->RPort.TxWidth < Node->Special.Slider.LevelWidth)
						Node->Special.Slider.MaxLevelLen = (Node->Special.Slider.LevelWidth + Handle->RPort.TxWidth - 1) / Handle->RPort.TxWidth;
				}

				if(Node->Special.Slider.LevelFormat && Node->Special.Slider.LevelPlace == PLACETEXT_LEFT && Node->LabelPlace == PLACE_LEFT && Node->Label && CheckIt)
				{
					LONG	space,len;
					STRPTR	Buffer;

					space = TextLength(&Handle->RPort," ",1);
					space = (Node->Special.Slider.LevelWidth + space - 1) / space;

					if(!V40 && (Handle->RPort.TxFlags & FPF_PROPORTIONAL))
					{
						Node->Special.Slider.MaxLevelLen++;

						space++;
					}

					if(Node->Special.Slider.OriginalLabel)
						LTP_Free(Handle,Node->Label,strlen(Node->Label) + 1);
					else
						Node->Special.Slider.OriginalLabel = Node->Label;

					len = strlen(Node->Special.Slider.OriginalLabel);

					if(Buffer = LTP_Alloc(Handle,len + space + 1))
					{
						strcpy(Buffer,Node->Special.Slider.OriginalLabel);

						while(space > 0)
						{
							Buffer[len] = ' ';
							len++;
							space--;
						}

						Buffer[len] = 0;

						Node->Label			= Buffer;
						Node->LabelWidth	= LT_LabelWidth(Handle,Node->Label);
					}
					else
						break;
				}

				if(Node->LabelChars * Handle->GlyphWidth > Node->LabelWidth)
					Node->LabelWidth = Node->LabelChars * Handle->GlyphWidth;

				break;
			}

			case TEXT_KIND:

				if(Node->Special.Text.Text && !Node->Special.Text.SizeLocked)
					Max = TextLength(&Handle->RPort,Node->Special.Text.Text,strlen(Node->Special.Text.Text));
				else
					Max = 0;

				Width = Node->Chars * Handle->GlyphWidth;

				if(!Node->Special.Text.SizeLocked && Node->Special.Text.LockSize)
				{
					LONG Chars = (Width + Handle->GlyphWidth - 1) / Handle->GlyphWidth;

					if(Chars > Node->Chars)
						Node->Chars = Chars;

					Max = Width = Node->Chars * Handle->GlyphWidth;
				}

				if(Max > Width)
					Width = Max;

				Node->Width		= 6 + Width + 6;
				Node->Height	= 3 + Handle->GlyphHeight + 3;

				if(Node->Special.Text.UsePicker)
					Node->Width += LTP_GetPickerSize(Handle);

				if(Node->Special.Text.LockSize)
					Node->Special.Text.SizeLocked = TRUE;

				break;

			case NUMBER_KIND:
			case INTEGER_KIND:
			case STRING_KIND:
			case PASSWORD_KIND:
			case FRACTION_KIND:

				Node->Width		= 6 + Node->Chars * Handle->GlyphWidth + 6;
				Node->Height	= 3 + Handle->GlyphHeight + 3;

				if(Node->Type == STRING_KIND && Node->Special.String.UsePicker)
					Node->Width += LTP_GetPickerSize(Handle);

				if((Node->Type == INTEGER_KIND && Node->Special.Integer.UseIncrementers) || (Node->Type == FRACTION_KIND && Node->Special.String.IncrementerHook))
					Node->Width += 2 * (4 + Handle->GlyphWidth + 4);

				break;
		}

		if(Node->LabelWidth > Node->Width && (Node->LabelPlace == PLACE_ABOVE || Node->LabelPlace == PLACE_BELOW))
			Node->Width = Node->LabelWidth;
	}
}
