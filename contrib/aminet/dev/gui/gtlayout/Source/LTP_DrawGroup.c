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

/****************************************************************************/

#include <hardware/blit.h>

/****************************************************************************/

#include "Assert.h"

/****************************************************************************/

#define MINTERM_B_OR_C      (ABC | ABNC | NABC | NABNC | ANBC | NANBC)
#define MINTERM_B_EQUALS_C  (ABC | ANBNC | NABC | NANBNC)

/****************************************************************************/

VOID
LTP_DrawObjectLabel(LayoutHandle *Handle,ObjectNode *Node)
{
	if(Node->Label)
	{
		LONG Left,Top,GlyphHeight = Handle->GlyphHeight;

		switch(Node->LabelPlace)
		{
			case PLACE_LEFT:

				Left	= Node->Left;
				Top		= Node->Top + (Node->Height - GlyphHeight) / 2;

				break;

			case PLACE_RIGHT:

				Left	= Node->Left + Node->Width + INTERWIDTH + INTERWIDTH + Node->LabelWidth;
				Top		= Node->Top + (Node->Height - GlyphHeight) / 2;

				break;

			case PLACE_ABOVE:

				Left	= Node->Left + (Node->Width - Node->LabelWidth) / 2 + INTERWIDTH + Node->LabelWidth;
				Top		= Node->Top - (GlyphHeight + INTERHEIGHT);

				break;

			case PLACE_BELOW:

				Left	= Node->Left + (Node->Width - Node->LabelWidth) / 2 + INTERWIDTH + Node->LabelWidth;
				Top		= Node->Top + Node->Height + INTERHEIGHT;

				break;

			default:

				return;	/* Only if none of the placement types match; should never happen! */
		}

		LTP_PrintLabel(Handle,Node,Left,Top);
	}
}

VOID
LTP_DrawGroup(LayoutHandle *Handle,ObjectNode *Group)
{
	ObjectNode *Node;
	ULONG page;
	LONG i;

	if(Group->Label || Group->Special.Group.Frame || Group->Special.Group.FrameType == FRAMETYPE_Label)
		LTP_DrawGroupLabel(Handle,Group);
	else
	{
		if(Group->Special.Group.FrameType == FRAMETYPE_Tab)
			LTP_DrawGroupFrame(Handle,Group);
	}

	SCANPAGE(Group,Node,page)
	{
		switch(Node->Type)
		{
			case BLANK_KIND:

				break;

			#ifdef OLD_STYLE_DEFAULT_KEY
			{
				case BUTTON_KIND:

					if(Handle->ReturnKey == Node)
						LTP_DrawBevelBox(Handle,Node);

					break;
			}
			#endif

			#ifdef DO_GAUGE_KIND
			{
				case GAUGE_KIND:
				{
					LONG Percent = Node->Current;

					Node->Current = 0;

					LTP_DrawGauge(Handle,Node,Percent,TRUE);

					break;
				}
			}
			#endif

			case PALETTE_KIND:

				if(Node->Special.Palette.UsePicker)
					LTP_DrawPalette(Handle,Node);

				break;

			case GROUP_KIND:

				if(Node->Label)
					LTP_DrawGroupLabel(Handle,Node);

				LTP_DrawGroup(Handle,Node);

				break;

			case XBAR_KIND:
			{
				LONG Top = Node->Top + ((Node->Height + 1) / 2) - 1;
				struct RastPort *RPort = &Handle->RPort;

				if(Node->Special.Bar.FullSize)
				{
					LTP_SetAPen(RPort,Handle->ShadowPen);
					LTP_DrawLine(RPort,Handle->Window->BorderLeft,Top,Handle->Window->Width - (Handle->Window->BorderRight + 1),Top);

					LTP_SetAPen(RPort,Handle->ShinePen);
					LTP_DrawLine(RPort,Handle->Window->BorderLeft,Top + 1,Handle->Window->Width - (Handle->Window->BorderRight + 1),Top + 1);
				}
				else
				{
					LONG Room,Left;

					Left = Node->Special.Bar.Parent->Left;

					if(Node->Special.Bar.Parent->Label)
						Room = 2 * Handle->GlyphWidth;
					else
						Room = 0;

					LTP_SetAPen(RPort,Handle->ShadowPen);
					LTP_PolyDraw(RPort,3,
						Left + Room,Top + 1,
						Left + Room,Top,
						Left + Node->Special.Bar.Parent->Width - 1 - 1 - Room,Top);

					LTP_SetAPen(RPort,Handle->ShinePen);
					LTP_PolyDraw(RPort,3,
						Left + Room + 1,Top + 1,
						Left + Node->Special.Bar.Parent->Width - 1 - Room,Top + 1,
						Left + Node->Special.Bar.Parent->Width - 1 - Room,Top);
				}

				break;
			}

			case YBAR_KIND:
			{
				LONG Left = Node->Left + ((Node->Width + 1) / 2) - 1,Room;
				struct RastPort *RPort = &Handle->RPort;

				if(Node->Special.Bar.Parent->Label)
					Room = Handle->GlyphHeight + Handle->InterHeight;
				else
					Room = 0;

				LTP_SetAPen(RPort,Handle->ShadowPen);
				LTP_PolyDraw(RPort,3,
					Left + 1,	Node->Special.Bar.Parent->Top + Room,
					Left,		Node->Special.Bar.Parent->Top + Room,
					Left,		Node->Special.Bar.Parent->Top + Node->Special.Bar.Parent->Height - 1 - 1 - Room);

				LTP_SetAPen(RPort,Handle->ShinePen);
				LTP_PolyDraw(RPort,3,
					Left + 1,	Node->Special.Bar.Parent->Top + Room + 1,
					Left + 1,	Node->Special.Bar.Parent->Top + Node->Special.Bar.Parent->Height - 1 - Room,
					Left,		Node->Special.Bar.Parent->Top + Node->Special.Bar.Parent->Height - 1 - Room);

				if((Handle->AspectY + Handle->AspectX - 1) / Handle->AspectX >= 2)
				{
					LTP_SetAPen(RPort,Handle->ShadowPen);
					LTP_DrawLine(RPort,	Left - 1,	Node->Special.Bar.Parent->Top + Room,
								Left - 1,	Node->Special.Bar.Parent->Top + Node->Special.Bar.Parent->Height - 1 - Room);

					LTP_SetAPen(RPort,Handle->ShinePen);
					LTP_DrawLine(RPort,	Left + 2,	Node->Special.Bar.Parent->Top + Room,
								Left + 2,	Node->Special.Bar.Parent->Top + Node->Special.Bar.Parent->Height - 1 - Room);
				}

				break;
			}

			case IMAGE_KIND:

				if(Node->Special.Image.Image != NULL)
					DrawImage(&Handle->RPort,Node->Special.Image.Image,Node->Left,Node->Top);
				else
				{
					UWORD	left	= Node->Special.Image.BitMapLeft,
							top		= Node->Special.Image.BitMapTop,
							width	= Node->Special.Image.BitMapWidth,
							height	= Node->Special.Image.BitMapHeight;

					if(Node->Special.Image.BitMapMask == NULL)
					{
						BltBitMapRastPort(Node->Special.Image.BitMap,left,top,
						                  &Handle->RPort,Node->Left,Node->Top,
						                  width,height,0xC0);
					}
					else
					{
						struct BitMap *source = Node->Special.Image.BitMap;
						struct RastPort *destination = &Handle->RPort;
						BOOL isInterleaved = FALSE;

						if(V39)
						{
							if(GetBitMapAttr(Node->Special.Image.BitMap,BMA_FLAGS) & BMF_INTERLEAVED)
								isInterleaved = TRUE;
						}

						if(isInterleaved)
						{
							struct BitMap mask;
							LONG i;

							InitBitMap(&mask,8,GetBitMapAttr(Node->Special.Image.BitMap,BMA_WIDTH),GetBitMapAttr(Node->Special.Image.BitMap,BMA_HEIGHT));
							for(i = 0 ; i < 8 ; i++)
								mask.Planes[i] = Node->Special.Image.BitMapMask;

							BltBitMapRastPort(source,left,top,destination,Node->Left,Node->Top,width,height,MINTERM_B_EQUALS_C);
							BltBitMapRastPort(&mask,left,top,destination,Node->Left,Node->Top,width,height,MINTERM_B_OR_C);
							BltBitMapRastPort(source,left,top,destination,Node->Left,Node->Top,width,height,MINTERM_B_EQUALS_C);
						}
						else
						{
							BltMaskBitMapRastPort(source,left,top,destination,Node->Left,Node->Top,width,height,(ABC | ABNC | ANBC),Node->Special.Image.BitMapMask);
						}
					}
				}

				LTP_DrawObjectLabel(Handle,Node);
				break;

			case FRAME_KIND:

				if(Node->Special.Frame.DrawBox)
					LTP_DrawBevelBox(Handle,Node);

				if(Node->Special.Frame.RefreshHook)
				{
					RefreshMsg Message;

					Message.ID		= Node->ID;
					Message.Left	= Node->Left;
					Message.Top		= Node->Top;
					Message.Width	= Node->Width;
					Message.Height	= Node->Height;

					if(Node->Special.Frame.DrawBox)
					{
						Message.Left	+= 4;
						Message.Top		+= 2;
						Message.Width	-= 8;
						Message.Height	-= 4;
					}

					CallHookPkt(Node->Special.Frame.RefreshHook,Handle,&Message);
				}

				// FALL THROUGH TO...

			case BOOPSI_KIND:

				// FALL THROUGH TO...

			case POPUP_KIND:

				// FALL THROUGH TO...

			case TAPEDECK_KIND:

				LTP_DrawObjectLabel(Handle,Node);
				break;

			case MX_KIND:

				if(Node->Label && !V39)
					LTP_PrintLabel(Handle,Node,Node->Left,Node->Top + (Node->Height - Handle->GlyphHeight) / 2 + 1);

				break;

			#ifdef DO_LEVEL_KIND
			{
				case LEVEL_KIND:

					LTP_LevelGadgetDrawLabel(Handle,Node,TRUE);
					break;
			}
			#endif	/* DO_LEVEL_KIND */

			case LISTVIEW_KIND:
			{
				struct RastPort *RPort = &Handle->RPort;

				if(Node->Special.List.ExtraLabels)
				{
					LTP_SetPens(RPort,Handle->TextPen,0,JAM1);

					for(i = 0 ; Node->Special.List.ExtraLabels[i] ; i++)
					{
						LONG Len = strlen(Node->Special.List.ExtraLabels[i]);

						LTP_PrintText(RPort,Node->Special.List.ExtraLabels[i],Len,Node->Left - (TextLength(RPort,Node->Special.List.ExtraLabels[i],Len) + INTERWIDTH),Node->Top + 2 + i * Handle->GlyphHeight);
					}
				}

				if(Node->Label)
				{
					if(Node->LabelPlace == PLACE_ABOVE && Node->Special.List.FlushLabelLeft)
					{
						struct TextFont *Font = NULL,*OldFont = NULL;

						if(Node->Special.List.TextAttr)
						{
							if(Font = LTP_OpenFont(Node->Special.List.TextAttr))
							{
								OldFont = RPort->Font;

								SetFont(RPort,Font);
							}
							else
								break;
						}

						LTP_PrintLabel(Handle,Node,Node->Left + 2 + INTERWIDTH + Node->LabelWidth,Node->Top - (Handle->GlyphHeight + INTERHEIGHT));

						if(Font)
						{
							SetFont(RPort,OldFont);

							CloseFont(Font);
						}
					}
					else
					{
						if(Node->Special.List.TextAttr)
							LTP_DrawObjectLabel(Handle,Node);
					}
				}

				break;
			}

			case BOX_KIND:
			{
				struct RastPort *RPort = &Handle->RPort;

				Node->Special.Box.Parent = Group;

				if(Node->Special.Box.DrawBox)
					LTP_DrawBevelBox(Handle,Node);

				for(i = 0 ; i < Node->Lines ; i++)
				{
					if(Node->Special.Box.Labels && Node->Special.Box.Labels[i])
					{
						LONG Len = strlen(Node->Special.Box.Labels[i]);

						LTP_SetPens(RPort,Handle->TextPen,0,JAM1);
						LTP_PrintText(RPort,Node->Special.Box.Labels[i],Len,Node->Left - (TextLength(RPort,Node->Special.Box.Labels[i],Len) + INTERWIDTH),Node->Top + 2 + i * (Handle->GlyphHeight + Node->Special.Box.Spacing));
					}

					LTP_PrintBoxLine(Handle,Node,i);
				}

				break;
			}
		}
	}
}
