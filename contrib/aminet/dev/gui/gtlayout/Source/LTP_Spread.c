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
LTP_Spread(LayoutHandle *Handle,ObjectNode *Group,LONG Width,LONG Height)
{
	ObjectNode *Node;
	LONG Count = 0;

	SCANGROUP(Group,Node)
	{
		Count++;
	}

	if(Group->Special.Group.Horizontal)
	{
		LONG Room = Width - Group->Width,Index = 0;
		LONG Left,Width;

		if(Count > 1)
		{
			Left = 0;

			Room += --Count * Handle->InterWidth;
		}
		else
			Left = Room / 2;

		SCANGROUP(Group,Node)
		{
			if(!LIKE_STRING_KIND(Node) || Node->Special.String.LinkID == -1)
			{
				Width = Node->Width;

				switch(Node->LabelPlace)
				{
					case PLACE_LEFT:

						if(Node->Type == MX_KIND)
						{
							Left += Node->Special.Radio.LabelWidth + INTERWIDTH;

							if(Node->Label)
							{
								if(Node->Special.Radio.TitlePlace == PLACETEXT_LEFT)
									Left += Node->LabelWidth + INTERWIDTH;
								else
									Width += INTERWIDTH + Node->LabelWidth;
							}
						}
						else
						{
							if(Node->Label)
								Left += Node->LabelWidth + INTERWIDTH;
						}

						break;

					case PLACE_RIGHT:

						if(Node->Type == MX_KIND)
						{
							Width += INTERWIDTH + Node->Special.Radio.LabelWidth;

							if(Node->Label)
							{
								if(Node->Special.Radio.TitlePlace == PLACETEXT_LEFT)
									Left += Node->LabelWidth + INTERWIDTH;
								else
									Width += INTERWIDTH + Node->LabelWidth;
							}
						}
						else
						{
							if(Node->Label)
								Width += INTERWIDTH + Node->LabelWidth;
						}

						break;
				}

				switch(Node->Type)
				{
					case LISTVIEW_KIND:

						if(Node->Special.List.ExtraLabelWidth)
							Left += Node->Special.List.ExtraLabelWidth + INTERWIDTH;

						break;

					case SLIDER_KIND:

						switch(Node->Special.Slider.LevelPlace)
						{
							case PLACETEXT_RIGHT:

								Width += INTERWIDTH + Node->Special.Slider.LevelWidth;
								break;

							case PLACETEXT_LEFT:

								if(Node->LabelPlace != PLACE_LEFT)
									Left += INTERWIDTH + Node->Special.Slider.LevelWidth;

								break;
						}

						break;

					#ifdef DO_LEVEL_KIND
					{
						case LEVEL_KIND:

							if(Node->Special.Level.LevelFormat != NULL)
							{
								switch(Node->Special.Level.LevelPlace)
								{
									case PLACETEXT_RIGHT:

										Width += INTERWIDTH + Node->Special.Level.MaxLevelWidth;
										break;

									case PLACETEXT_LEFT:

										if(Node->LabelPlace != PLACE_LEFT)
											Left += INTERWIDTH + Node->Special.Level.MaxLevelWidth;

										break;
								}
							}

							break;
					}
					#endif	/* DO_LEVEL_KIND*/
				}

				Node->Left = Left;

				Left += Width + ((Room * (Index + 1)) / Count - (Room * Index) / Count);

				Index++;
			}
		}
	}
	else
	{
		LONG Room = Height - Group->Height,Index = 0;
		LONG Height,Top,y;

		if(Count > 1)
		{
			Top = 0;

			Room += --Count * Handle->InterHeight;
		}
		else
			Top = Room / 2;

		SCANGROUP(Group,Node)
		{
			if(!LIKE_STRING_KIND(Node) || Node->Special.String.LinkID == -1)
			{
				if(Count++)
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

				if(Node->LabelPlace == PLACE_ABOVE && Node->Label)
					y += Handle->GlyphHeight + INTERHEIGHT;

				Node->Top = y;

				if(Node->LabelPlace == PLACE_BELOW && Node->Label)
					Height = Node->Height + INTERHEIGHT + Handle->GlyphHeight;
				else
					Height = Node->Height;

				Top = y + Height + ((Room * (Index + 1)) / Count - (Room * Index) / Count);

				Index++;
			}
		}
	}
}
