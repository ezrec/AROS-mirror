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
LTP_LayoutGroup(LayoutHandle *Handle,ObjectNode *Group)
{
	if(!Handle->Failed)
	{
		ObjectNode	*Node;
		ObjectNode	*ReturnKey;
		ObjectNode	*EscKey;
		ObjectNode	*CursorKey;
		ObjectNode	*TabKey;
		ObjectNode	*ActiveString;
		LONG		 Left;
		LONG		 Top;
		LONG		 Width;
		LONG		 Height;
		LONG		 MaxWidth;
		LONG		 MaxHeight;
		BOOL		 DefaultCorrection;

		Left				= 0;
		Top					= 0;

		ReturnKey			= NULL;
		EscKey				= NULL;
		CursorKey			= NULL;
		TabKey				= NULL;
		ActiveString		= NULL;
		DefaultCorrection	= FALSE;
		MaxWidth			= 0;
		MaxHeight			= 0;

		SCANGROUP(Group,Node)
		{
			LTP_DetermineSize(Handle,Node);

			switch(Node->Type)
			{
				case BUTTON_KIND:

					if(Node->Special.Button.ReturnKey)
						ReturnKey = Node;
					else
					{
						if(Node->Special.Button.EscKey)
							EscKey = Node;
					}

					DefaultCorrection |= Node->Special.Button.DefaultCorrection;

					break;

				case CYCLE_KIND:

					if(Node->Special.Cycle.TabKey)
						TabKey = Node;

					break;

				#ifdef DO_POPUP_KIND
				{
					case POPUP_KIND:

						if(Node->Special.Popup.TabKey)
							TabKey = Node;

						break;
				}
				#endif

				#ifdef DO_TAB_KIND
				{
					case TAB_KIND:

						if(Node->Special.Tab.TabKey)
							TabKey = Node;

						break;
				}
				#endif

				case MX_KIND:

					if(Node->Special.Radio.TabKey)
						TabKey = Node;

					break;

				case PASSWORD_KIND:
				case STRING_KIND:
				case FRACTION_KIND:

					if(Node->Special.String.Activate)
						ActiveString = Node;

					break;

				case INTEGER_KIND:

					if(Node->Special.Integer.Activate)
						ActiveString = Node;

					break;

				case LISTVIEW_KIND:

					if(Node->Special.List.CursorKey)
						CursorKey = Node;

					break;
			}

			if(Node->Width > MaxWidth)
				MaxWidth = Node->Width;

			if(Node->Height > MaxHeight)
				MaxHeight = Node->Height;
		}

		if(ReturnKey || DefaultCorrection)
		{
			SCANGROUP(Group,Node)
			{
				#ifdef OLD_STYLE_DEFAULT_KEY
				{
					Node->GroupIndent = TRUE;

					Node->Width		+= 4 + 4;
					Node->Height	+= 2 + 2;
				}
				#else
				{
					Node->Width		+= 1 + 1;
					Node->Height	+= 1 + 1;
				}
				#endif

				if(Node->Width > MaxWidth)
					MaxWidth = Node->Width;

				if(Node->Height > MaxHeight)
					MaxHeight = Node->Height;
			}
		}

		if(Group->Special.Group.SameSize)
		{
			SCANGROUP(Group,Node)
			{
				if(Node->Width < MaxWidth && Node->Type != YBAR_KIND && Node->Type != CHECKBOX_KIND)
				{
					if(Node->Type == GROUP_KIND)
					{
						if(!Node->Special.Group.NoIndent)
						{
							if(Node->Special.Group.AlignRight)
								Node->Special.Group.ExtraLeft += MaxWidth - Node->Width;
							else
								Node->Special.Group.ExtraLeft += (MaxWidth - Node->Width) / 2;
						}
					}

					Node->Width = MaxWidth;
				}

				if(Node->Height < MaxHeight && Node->Type != XBAR_KIND && Node->Type != CHECKBOX_KIND)
				{
					if(Node->Type == GROUP_KIND && !Node->Special.Group.NoIndent)
						Node->Special.Group.ExtraTop += (MaxHeight - Node->Height) / 2;

					Node->Height = MaxHeight;
				}
			}
		}

		if(Group->Special.Group.Horizontal)
		{
			LONG x,y,w,MaxTop = 0,MaxWidth = 0,MaxHeight = 0,Count = 0;

			if(Group->Node.mln_Pred->mln_Pred && Group->Special.Group.LastAttributes)
			{
				ObjectNode *LastGroup = (ObjectNode *)Group->Node.mln_Pred;

				MaxTop		= LastGroup->Special.Group.MaxOffset;
				MaxHeight	= LastGroup->Special.Group.MaxSize;
			}

			SCANGROUP(Group,Node)
			{
				if(!LIKE_STRING_KIND(Node) || Node->Special.String.LinkID == -1)
				{
					if(!Group->Special.Group.Paging && Count++)
					{
						if(Node->ExtraSpace > 0)
							x = Left + Node->ExtraSpace * Handle->InterWidth;
						else
						{
							if(Node->ExtraSpace < 0)
								x = Left - Node->ExtraSpace;
							else
								x = Left + Node->LayoutSpace * Handle->InterWidth;
						}
					}
					else
						x = Left;

					y = Top;
					w = Node->Width;

					switch(Node->LabelPlace)
					{
						case PLACE_LEFT:

							if(Node->Type == MX_KIND)
							{
								x += Node->Special.Radio.LabelWidth + INTERWIDTH;

								if(Node->Label)
								{
									if(Node->Special.Radio.TitlePlace == PLACETEXT_LEFT)
										x += Node->LabelWidth + INTERWIDTH;
									else
										w += INTERWIDTH + Node->LabelWidth;
								}
							}
							else
							{
								if(Node->Label || (Node->Type == BOX_KIND && Node->Special.Box.Labels))
									x += Node->LabelWidth + INTERWIDTH;
							}

							break;

						case PLACE_RIGHT:

							if(Node->Type == MX_KIND)
							{
								w += INTERWIDTH + Node->Special.Radio.LabelWidth;

								if(Node->Label)
								{
									if(Node->Special.Radio.TitlePlace == PLACETEXT_LEFT)
										x += Node->LabelWidth + INTERWIDTH;
									else
										w += INTERWIDTH + Node->LabelWidth;
								}
							}
							else
							{
								if(Node->Label || (Node->Type == BOX_KIND && Node->Special.Box.Labels))
									w += INTERWIDTH + Node->LabelWidth;
							}

							break;

						case PLACE_ABOVE:

							if(Node->Label || (Node->Type == BOX_KIND && Node->Special.Box.Labels))
							{
								if(Node->Type == LISTVIEW_KIND && Node->Special.List.TextAttr && Node->Special.List.FlushLabelLeft)
									y += Node->Special.List.FixedGlyphHeight + INTERHEIGHT;
								else
									y += Handle->GlyphHeight + INTERHEIGHT;
							}

							break;
					}

					if(Node->Type == SLIDER_KIND)
					{
						if(Node->Special.Slider.LevelPlace == PLACETEXT_RIGHT && Node->Special.Slider.LevelFormat)
							w += INTERWIDTH + Node->Special.Slider.LevelWidth;

						if(Node->Special.Slider.LevelPlace == PLACETEXT_LEFT && Node->LabelPlace != PLACE_LEFT && Node->Special.Slider.LevelFormat)
							x += INTERWIDTH + Node->Special.Slider.LevelWidth;
					}

					#ifdef DO_LEVEL_KIND
					{
						if(Node->Type == LEVEL_KIND)
						{
							if(Node->Special.Level.LevelFormat != NULL)
							{
								if(Node->Special.Level.LevelPlace == PLACETEXT_RIGHT)
									w += INTERWIDTH + Node->Special.Level.MaxLevelWidth;

								if(Node->Special.Level.LevelPlace == PLACETEXT_LEFT && Node->LabelPlace != PLACE_LEFT)
									x += INTERWIDTH + Node->Special.Level.MaxLevelWidth;
							}
						}
					}
					#endif	/* DO_LEVEL_KIND */

					if(Node->Type == LISTVIEW_KIND && Node->Special.List.ExtraLabelWidth)
						x += Node->Special.List.ExtraLabelWidth + INTERWIDTH;

					if(y > MaxTop)
						MaxTop = y;

					if(Node->Height > MaxHeight)
						MaxHeight = Node->Height;

					if(Node->LabelPlace == PLACE_BELOW && Node->Label)
					{
						LONG Height;

						if(Node->Type == LISTVIEW_KIND && Node->Special.List.TextAttr && Node->Special.List.FlushLabelLeft)
							Height = Node->Height + INTERHEIGHT + Node->Special.List.FixedGlyphHeight;
						else
							Height = Node->Height + INTERHEIGHT + Handle->GlyphHeight;

						if(Height > MaxHeight)
							MaxHeight = Height;
					}
					else
					{
						if(Node->Height > MaxHeight)
							MaxHeight = Node->Height;
					}

					if(x + w > MaxWidth)
						MaxWidth = x+w;

					Node->Left = x;

					if(!Group->Special.Group.Paging)
						Left = x + w;
				}
			}

			SCANGROUP(Group,Node)
			{
				if(!LIKE_STRING_KIND(Node) || Node->Special.String.LinkID == -1)
				{
					Node->Top = MaxTop;

					if(Node->Type == GROUP_KIND)
						Node->Special.Group.ExtraTop += MaxTop - Top;

					if(Node->Height < MaxHeight)
					{
						register BOOL CanChangeHeight;

						#ifdef DO_BOOPSI_KIND
						{
							CanChangeHeight = (BOOL)(	(Node->Type == PALETTE_KIND && !Node->Special.Palette.UsePicker) ||
														(Node->Type == LISTVIEW_KIND) ||
														(Node->Type == BOOPSI_KIND && !Node->Special.BOOPSI.ExactHeight)
													);
						}
						#else
						{
							CanChangeHeight = (BOOL)(	(Node->Type == PALETTE_KIND && !Node->Special.Palette.UsePicker) ||
														(Node->Type == LISTVIEW_KIND)
													);
						}
						#endif	/* DO_BOOPSI_KIND */

						if(CanChangeHeight)
						{
							Node->Height = MaxHeight;
						}
						else
						{
							if(Node->Type == GROUP_KIND)
							{
								if(Node->Special.Group.Spread)
								{
									LTP_Spread(Handle,Node,Node->Width,MaxHeight);
								}
								else
								{
									if(!Node->Special.Group.NoIndent)
										Node->Special.Group.ExtraTop += (MaxHeight - Node->Height) / 2; /* !FIX! */
								}

								Node->Height = MaxHeight;
							}
						}
					}
				}
			}

			Group->Special.Group.MaxOffset = MaxTop;
			Group->Special.Group.MaxSize   = MaxHeight;

			if(Group->Special.Group.Paging)
				Width = MaxWidth;
			else
				Width = Left;

			Height = MaxTop + MaxHeight;
		}
		else
		{
			LONG x,y,w,MaxLeft = 0,MaxWidth = 0,MaxHeight = 0,Count = 0;

			if(Group->Node.mln_Pred->mln_Pred && Group->Special.Group.LastAttributes)
			{
				ObjectNode *LastGroup = (ObjectNode *)Group->Node.mln_Pred;

				MaxLeft 	= LastGroup->Special.Group.MaxOffset;
				MaxWidth	= LastGroup->Special.Group.MaxSize;
			}

			SCANGROUP(Group,Node)
			{
				if(!LIKE_STRING_KIND(Node) || Node->Special.String.LinkID == -1)
				{
					if(!Group->Special.Group.Paging && Count++)
					{
						if(Node->ExtraSpace > 0)
							y = Top + Node->ExtraSpace * Handle->InterHeight;
						else
						{
							if(Node->ExtraSpace < 0)
								y = Top - Node->ExtraSpace;
							else
								y = Top + Node->LayoutSpace * Handle->InterHeight;
						}
					}
					else
						y = Top;

					w = Node->Width;
					x = Left;

					switch(Node->LabelPlace)
					{
						case PLACE_LEFT:

							if(Node->Type == MX_KIND)
							{
								x += Node->Special.Radio.LabelWidth + INTERWIDTH;

								if(Node->Label)
								{
									if(Node->Special.Radio.TitlePlace == PLACETEXT_LEFT)
										x += Node->LabelWidth + INTERWIDTH;
								}
							}
							else
							{
								if(Node->Label || (Node->Type == BOX_KIND && Node->Special.Box.Labels))
									x += Node->LabelWidth + INTERWIDTH;
							}

							break;

						case PLACE_RIGHT:

							if(Node->Type == MX_KIND)
							{
								w += INTERWIDTH + Node->Special.Radio.LabelWidth;

								if(Node->Label)
								{
									if(Node->Special.Radio.TitlePlace == PLACETEXT_LEFT)
										x += Node->LabelWidth + INTERWIDTH;
									else
										w += INTERWIDTH + Node->LabelWidth;
								}
							}
							else
							{
								if(Node->Label || (Node->Type == BOX_KIND && Node->Special.Box.Labels))
									w += INTERWIDTH + Node->LabelWidth;
							}

							break;

						case PLACE_ABOVE:

							if(Node->Label || (Node->Type == BOX_KIND && Node->Special.Box.Labels))
							{
								if(Node->Type == LISTVIEW_KIND && Node->Special.List.TextAttr && Node->Special.List.FlushLabelLeft)
									y += Node->Special.List.FixedGlyphHeight + INTERHEIGHT;
								else
									y += Handle->GlyphHeight + INTERHEIGHT;
							}

							break;
					}

					if(Node->Type == SLIDER_KIND)
					{
						if(Node->Special.Slider.LevelPlace == PLACETEXT_RIGHT && Node->Special.Slider.LevelFormat)
							w += INTERWIDTH + Node->Special.Slider.LevelWidth;

						if(Node->Special.Slider.LevelPlace == PLACETEXT_LEFT && Node->LabelPlace != PLACE_LEFT && Node->Special.Slider.LevelFormat)
							x += INTERWIDTH + Node->Special.Slider.LevelWidth;
					}

					#ifdef DO_LEVEL_KIND
					{
						if(Node->Type == LEVEL_KIND)
						{
							if(Node->Special.Level.LevelFormat != NULL)
							{
								if(Node->Special.Level.LevelPlace == PLACETEXT_RIGHT)
									w += INTERWIDTH + Node->Special.Level.MaxLevelWidth;

								if(Node->Special.Level.LevelPlace == PLACETEXT_LEFT && Node->LabelPlace != PLACE_LEFT)
									x += INTERWIDTH + Node->Special.Level.MaxLevelWidth;
							}
						}
					}
					#endif	/* DO_LEVEL_KIND */

					if(Node->Type == LISTVIEW_KIND && Node->Special.List.ExtraLabelWidth)
						x += Node->Special.List.ExtraLabelWidth + INTERWIDTH;

					if(x > MaxLeft)
						MaxLeft = x;

					if(w > MaxWidth)
						MaxWidth = w;

					if(y + Node->Height > MaxHeight)
						MaxHeight = y + Node->Height;

					Node->Top = y;

					if(!Group->Special.Group.Paging)
						Top = y + Node->Height;

					if(Node->LabelPlace == PLACE_BELOW && Node->Label)
					{
						if(Node->Type == LISTVIEW_KIND && Node->Special.List.TextAttr && Node->Special.List.FlushLabelLeft)
							Top += INTERHEIGHT + Node->Special.List.FixedGlyphHeight;
						else
							Top += INTERHEIGHT + Handle->GlyphHeight;
					}
				}
			}

			SCANGROUP(Group,Node)
			{
				if(!LIKE_STRING_KIND(Node) || Node->Special.String.LinkID == -1)
				{
					Node->Left = MaxLeft;

					if(Node->Type == GROUP_KIND)
					{
						if(Group->Special.Group.Paging && !Node->Special.Group.NoIndent)
							Node->Special.Group.ExtraTop += (MaxHeight - Node->Height) / 2;

						Node->Special.Group.ExtraLeft += MaxLeft - Left;
					}

					if(Node->Width < MaxWidth)
					{
						if(Node->Type == GROUP_KIND)
						{
							if(Node->Special.Group.Spread)
								LTP_Spread(Handle,Node,MaxWidth,Node->Height);
							else
							{
								if(!Node->Special.Group.NoIndent)
								{
									if(Node->Special.Group.AlignRight)
										Node->Special.Group.ExtraLeft += MaxWidth - Node->Width;
									else
										Node->Special.Group.ExtraLeft += (MaxWidth - Node->Width) / 2;
								}
							}

							Node->Width = MaxWidth;
						}
						else
						{
							register BOOL CanChangeWidth;

							#ifdef DO_BOOPSI_KIND
							{
								CanChangeWidth = (BOOL)(	(Node->Type == LISTVIEW_KIND && !Node->Special.List.LockSize) ||
															(Node->Type == PALETTE_KIND && !Node->Special.Palette.UsePicker) ||
															(Node->Type == TEXT_KIND && !Node->Special.Text.LockSize) ||
															(LIKE_STRING_KIND(Node)) ||
															(Node->Type == BLANK_KIND) ||
															(Node->Type == BOX_KIND) ||
															(Node->Type == BUTTON_KIND) ||
															(Node->Type == CYCLE_KIND) ||
															(Node->Type == GAUGE_KIND) ||
															(Node->Type == INTEGER_KIND) ||
															(Node->Type == NUMBER_KIND) ||
															(Node->Type == SCROLLER_KIND) ||
															(Node->Type == SLIDER_KIND) ||
															(Node->Type == BOOPSI_KIND && !Node->Special.BOOPSI.ExactWidth) ||
															(Node->Type == LEVEL_KIND) ||
															(Node->Type == POPUP_KIND) ||
															(Node->Type == TAB_KIND)
														);
							}
							#else
							{
								CanChangeWidth = (BOOL)(	(Node->Type == LISTVIEW_KIND && !Node->Special.List.LockSize) ||
															(Node->Type == PALETTE_KIND && !Node->Special.Palette.UsePicker) ||
															(Node->Type == TEXT_KIND && !Node->Special.Text.LockSize) ||
															(LIKE_STRING_KIND(Node)) ||
															(Node->Type == BLANK_KIND) ||
															(Node->Type == BOX_KIND) ||
															(Node->Type == BUTTON_KIND) ||
															(Node->Type == CYCLE_KIND) ||
															(Node->Type == GAUGE_KIND) ||
															(Node->Type == INTEGER_KIND) ||
															(Node->Type == NUMBER_KIND) ||
															(Node->Type == SCROLLER_KIND) ||
															(Node->Type == SLIDER_KIND)
														);
							}
							#endif	/* DO_BOOPSI_KIND */

							if(CanChangeWidth)
							{
								if(Node->Label && Node->LabelPlace == PLACE_RIGHT)
									Node->Width = MaxWidth - (INTERWIDTH + Node->LabelWidth);
								else
									Node->Width = MaxWidth;
							}
						}
					}
				}
			}

			Group->Special.Group.MaxOffset  = MaxLeft;
			Group->Special.Group.MaxSize    = MaxWidth;

			if(Group->Special.Group.Paging)
				Height = MaxHeight;
			else
				Height = Top;

			Width = MaxLeft + MaxWidth;
		}

		Group->Left		= 0;
		Group->Top		= 0;
		Group->Width	= Width;
		Group->Height	= Height;

		if(Group->Label || Group->Special.Group.Frame || Group->Special.Group.FrameType != FRAMETYPE_None)
		{
			if(Group->Label)
			{
				Group->LabelWidth = LT_LabelWidth(Handle,Group->Label) + 2 * Handle->GlyphWidth;

				Group->Height += Handle->GlyphHeight + 2 * Handle->InterHeight + 3;
			}
			else
			{
				Group->LabelWidth = 0;

				Group->Height += 2 + 2 * Handle->InterHeight + 3;
			}

			if(Group->LabelWidth > Group->Width)
			{
				if(!Node->Special.Group.NoIndent)
				{
					if(Node->Special.Group.AlignRight)
						Group->Special.Group.ExtraLeft += Group->LabelWidth - Group->Width;
					else
						Group->Special.Group.ExtraLeft += (Group->LabelWidth - Group->Width) / 2;
				}

				Group->Width = Group->LabelWidth;
			}
			else
			{
				if(Group->Special.Group.FrameType == FRAMETYPE_Tab)
				{
					Group->Special.Group.ExtraLeft += Handle->GlyphWidth;
					Group->Width += 2 * Handle->GlyphWidth;
				}
			}

			Group->Width += 2 * (Handle->GlyphWidth + Handle->InterWidth) + 8;
		}

		if(Group->Special.Group.IndentX)
		{
			Group->Width					+= 2 * Handle->InterWidth;
			Group->Special.Group.ExtraLeft	+= Handle->InterWidth;
		}

		if(Group->Special.Group.IndentY)
		{
			Group->Height					+= 2 * Handle->InterHeight;
			Group->Special.Group.ExtraTop	+= Handle->InterHeight;
		}

		if(ReturnKey)
			Handle->ReturnKey = ReturnKey;

		if(EscKey)
			Handle->EscKey = EscKey;

		if(CursorKey)
			Handle->CursorKey = CursorKey;

		if(TabKey)
			Handle->TabKey = TabKey;

		if(ActiveString)
			Handle->ActiveString = ActiveString;
	}
}
