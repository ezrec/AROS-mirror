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

#include <intuition/classes.h>

#include <clib/alib_protos.h>	/* For Coerce/Do/DoSuperMethod */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_POPUP_KIND	/* Support code */

#define PIF_SingleActive	(1 << 0)
#define PIF_ArrowUp			(1 << 1)
#define PIF_ArrowDown		(1 << 2)

STATIC VOID
DrawArrow(
	struct RastPort *	RPort,
	LONG				Left,
	LONG				Top,
	LONG				ArrowWidth,
	LONG				ArrowHeight,
	LONG				Dir)
{
	LONG i,Width,Start,Pos;

	for(i = 0 ; i < ArrowHeight ; i++)
	{
		Width = ((ArrowWidth * (i + 1)) / ArrowHeight) & ~1;

		if(Width < ArrowWidth)
			Width++;

		Start = Left + (ArrowWidth - Width) / 2;

		if(Dir < 0)
			Pos = Top + i;
		else
			Pos = Top + ArrowHeight - 1 - i;

		LTP_DrawLine(RPort,Start,Pos,Start + Width - 1,Pos);
	}
}

STATIC VOID
DrawContainer(
	struct RastPort *	RPort,
	LONG				Left,
	LONG				Top,
	LONG				Width,
	LONG				Height,
	struct GadgetInfo *	GadgetInfo,
	PopInfo *			Info,
	UWORD				Highlight,
	UWORD				Disabled)
{
	LONG Shine,Shadow,Txt,Fill;
	LONG Right;
	UWORD * Pens;

	Pens = GadgetInfo->gi_DrInfo->dri_Pens;

	if(Highlight)
	{
		Shine	= Pens[SHADOWPEN];
		Shadow	= Pens[SHINEPEN];
		Txt		= Pens[FILLTEXTPEN];
		Fill	= Pens[FILLPEN];
	}
	else
	{
		Shine	= Pens[SHINEPEN];
		Shadow	= Pens[SHADOWPEN];
		Txt		= Pens[TEXTPEN];
		Fill	= Pens[BACKGROUNDPEN];
	}

	LTP_SetPens(RPort,Fill,0,JAM1);

	if(Info->FrameImage != NULL)
	{
		DrawImageState(RPort,Info->FrameImage,Left,Top,Highlight ? IDS_SELECTED : IDS_NORMAL,GadgetInfo->gi_DrInfo);
	}
	else
	{
		LTP_FillBox(RPort,Left + 2,Top + 1,Width - 4,Height - 2);
		LTP_RenderBevel(RPort,Pens,Left,Top,Width,Height,Highlight,2);
	}

	Right = Left + Width - Info->MarkLeft;

	LTP_SetAPen(RPort,Shadow);
	LTP_DrawLine(RPort,Right,Top + 2,Right,Top + Height - 3);

	LTP_SetAPen(RPort,Shine);
	LTP_DrawLine(RPort,Right + 1,Top + 2,Right + 1,Top + Height - 3);

	SetFont(RPort,Info->Font);
	LTP_SetAPen(RPort,Txt);

	LTP_DrawAdjustedPicker(RPort,FALSE,Right + 2,Top,Info->PickerWidth,Height,Info->AspectX,Info->AspectY);

	LTP_PrintText(RPort,Info->Labels[Info->Active],Info->ActiveLen,Left + 6,Top + Info->LabelTop);

	if(Disabled)
		LTP_GhostBox(RPort,Left,Top,Width,Height,Pens[SHADOWPEN]);
}

STATIC VOID
DrawOneBox(
	PopInfo *	Info,
	LONG		Top,
	LONG		Index)
{
	struct RastPort	*	RPort;
	LONG				Len;
	LONG				Extra;
	STRPTR				Label;

	Label = Info->Labels[Index];

	RPort = Info->Window->RPort;

	if((Len = strlen(Label)) > Info->MaxLen)
		Len = Info->MaxLen;

	if(Info->CheckGlyph)
		Extra = Info->CheckGlyph->Width + 2;
	else
		Extra = 0;

	LTP_PrintText(RPort,Label,Len,6 + Extra,Top + Info->LineTop);

	if(Extra && Index == Info->InitialActive)
		LTP_DrawCheckGlyph(RPort,6,Top + Info->LineTop,Info->CheckGlyph,Index == Info->LastLabelDrawn);
}

STATIC VOID
DrawOneGlyph(
	PopInfo *	Info,
	LONG		Top,
	LONG		Dir,
	UWORD *		Pens)
{
	struct RastPort *RPort;

	RPort = Info->Window->RPort;

	if(Pens != NULL)
	{
		LTP_SetAPen(RPort,Info->MenuBack);
		LTP_FillBox(RPort,4,Top,Info->SingleWidth,Info->SingleHeight);
		LTP_SetAPen(RPort,Info->MenuText);
	}

	DrawArrow(RPort,6,Top + Info->ArrowTop,Info->ArrowWidth,Info->ArrowHeight,Dir);
}

STATIC VOID
BoxRender(
	PopInfo *	Info,
	UWORD *		Pens,
	LONG		Active,
	BOOL		FullRefresh,
	BOOL		Highlight,
	LONG		Dir)
{
	struct RastPort *RPort = Info->Window->RPort;

	if(FullRefresh)
	{
		LONG Index,Top;
		LONG Width,Height;

		LTP_SetPens(RPort,Info->MenuBack,0,JAM1);

		Width	= Info->Window->Width;
		Height	= Info->Window->Height;

		LTP_FillBox(RPort,2,1,Width - 4,Height - 2);

		LTP_SetAPen(RPort,Info->MenuText);

		LTP_PolyDraw(RPort,5,
			1,1,
			1,Height - 2,
			0,Height - 2,
			0,0,
			Width - 1,0);

		LTP_PolyDraw(RPort,5,
			0,Height - 1,
			Width - 1,Height - 1,
			Width - 1,1,
			Width - 2,1,
			Width - 2,Height - 2);

		Index	= Info->TopMost;
		Top		= 2;
		Height	= Info->BoxHeight;

		Info->Flags &= ~(PIF_ArrowUp | PIF_ArrowDown);

		if(Info->TopMost > 0 && Active != Info->TopMost && Info->BoxLines > 2)
		{
			Info->Flags |= PIF_ArrowUp;

			Index	+= 1;
			Top		+= Info->SingleHeight;
			Height	-= Info->SingleHeight;
		}

		if(Info->TopMost + Info->BoxLines < Info->NumLabels && Active != Info->TopMost + Info->BoxLines - 1 && Info->BoxLines > 2)
		{
			Info->Flags |= PIF_ArrowDown;

			Height -= Info->SingleHeight;
		}

		Info->LastLabelDrawn = Active;	// IMPORTANT: must be set up here to DrawOneBox can find it

		for(NULL ; Index < Info->NumLabels && Height > 0 ; Index++, Top += Info->SingleHeight, Height -= Info->SingleHeight)
		{
			if(Index == Active)
			{
				LTP_SetAPen(RPort,Info->MenuBackSelect);
				LTP_FillBox(RPort,4,Top,Info->SingleWidth,Info->SingleHeight);

				LTP_SetAPen(RPort,Info->MenuTextSelect);
				DrawOneBox(Info,Top,Index);

				LTP_SetAPen(RPort,Info->MenuText);
			}
			else
			{
				DrawOneBox(Info,Top,Index);
			}
		}

		if(Info->Flags & PIF_ArrowUp)
			DrawOneGlyph(Info,2,-1,NULL);

		if(Info->Flags & PIF_ArrowDown)
			DrawOneGlyph(Info,2 + (Info->BoxLines - 1) * Info->SingleHeight,1,NULL);

		Info->LastDrawn = 2 + (Active - Info->TopMost) * Info->SingleHeight;
	}
	else
	{
		if(Info->LastLabelDrawn != -1)
		{
			LONG Top = Info->LastDrawn,Last = Info->LastLabelDrawn;

			Info->LastLabelDrawn = -1;

			LTP_SetPens(RPort,Info->MenuBack,0,JAM1);
			LTP_FillBox(RPort,4,Top,Info->SingleWidth,Info->SingleHeight);
			LTP_SetAPen(RPort,Info->MenuText);

			DrawOneBox(Info,Top,Last);
		}

		Info->LastDrawn = -1;

		if(Active >= 0)
		{
			LONG Top,NewActive;

			NewActive	= Active - Info->TopMost;
			Top			= 2 + NewActive * Info->SingleHeight;

			if(Top >= 2 && Top < Info->BoxHeight + 2)
			{
				LONG Pen;

				if(Dir != 0)
					Highlight = FALSE;

				if(Highlight)
				{
					Pen = Info->MenuBackSelect;

					Info->LastDrawn			= Top;
					Info->LastLabelDrawn	= Active;
				}
				else
				{
					Pen = Info->MenuBack;
				}

				LTP_SetPens(RPort,Pen,0,JAM1);
				LTP_FillBox(RPort,4,Top,Info->SingleWidth,Info->SingleHeight);

				if(Highlight)
					Pen = Info->MenuTextSelect;
				else
					Pen = Info->MenuText;

				LTP_SetAPen(RPort,Pen);

				if(Dir == 0)
					DrawOneBox(Info,Top,Active);
				else
					DrawOneGlyph(Info,Top,Dir,NULL);
			}
		}
	}
}

STATIC ULONG
InputMethod(
	struct IClass *		class,
	struct Gadget *		gadget,
	struct gpInput *	InputInfo)
{
	PopInfo	*Info	= INST_DATA(class,gadget);
	ULONG	 Result	= GMR_MEACTIVE;
	BOOL	 Done	= FALSE;
	BOOL	 Redraw	= FALSE;

	if(InputInfo->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
	{
		UWORD Code = InputInfo->gpi_IEvent->ie_Code;

		if(Code == MENUDOWN)
		{
			Result	= GMR_NOREUSE;
			Done	= TRUE;

			Info->Active = Info->InitialActive;
		}
		else
		{
			if(Code == SELECTUP)
			{
				Done = TRUE;

				if(Info->Active < 0)
				{
					Result = GMR_NOREUSE;

					Info->Active = Info->InitialActive;
				}
				else
				{
					LONG Len;

					Result = GMR_NOREUSE | GMR_VERIFY;

					if(Info->Flags & PIF_SingleActive)
					{
						LONG Width,Height;
						LONG x,y;

						x = InputInfo->gpi_Mouse.X;
						y = InputInfo->gpi_Mouse.Y;

						Width	= gadget->Width;
						Height	= gadget->Height;

						if(x >= 0 && y >= 0 && x < Width && y < Height)
						{
							if(InputInfo->gpi_IEvent->ie_Qualifier & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
								Info->Active--;
							else
								Info->Active++;

							if(Info->Active >= Info->NumLabels)
							{
								Info->Active = 0;
							}
							else
							{
								if(Info->Active < 0)
									Info->Active = Info->NumLabels - 1;
							}
						}
						else
						{
							Result = GMR_NOREUSE;

							Info->Active = Info->InitialActive;
						}
					}

					Len = strlen(Info->Labels[Info->Active]);

					if(Len > Info->MaxLen)
						Len = Info->MaxLen;

					Info->ActiveLen = Len;

					(*InputInfo->gpi_Termination) = Info->Active;
				}
			}
			else
			{
				if(Info->Flags & PIF_SingleActive)
				{
					LONG Width,Height;
					LONG x,y;
					BOOL RefreshIt = FALSE;

					x = InputInfo->gpi_Mouse.X;
					y = InputInfo->gpi_Mouse.Y;

					Width	= gadget->Width;
					Height	= gadget->Height;

					if(x < 0 || y < 0 || x >= Width || y >= Height)
					{
						if(gadget->Flags & GFLG_SELECTED)
							RefreshIt = TRUE;
					}
					else
					{
						if(!(gadget->Flags & GFLG_SELECTED))
							RefreshIt = TRUE;
					}

					if(RefreshIt)
					{
						gadget->Flags ^= GFLG_SELECTED;

						Redraw = TRUE;
					}
				}
			}
		}

		if(Done)
		{
			if(gadget->Flags & GFLG_SELECTED)
			{
				gadget->Flags &= ~GFLG_SELECTED;
				Redraw = TRUE;
			}

			if(Info->Window != NULL)
			{
				CloseWindow(Info->Window);
				Info->Window = NULL;
			}

			LTP_DeleteCheckGlyph(Info->CheckGlyph);
			Info->CheckGlyph = NULL;

			Info->Flags &= ~PIF_SingleActive;
		}
	}

	if(!Done && Info->Window)
	{
		LONG NewActive = 0;
		LONG x,y;
		LONG Dir = 0;
		LONG From = 0,To = 0;	/* Initialize these for the sake of the compiler. */

		x = InputInfo->gpi_GInfo->gi_Screen->MouseX - Info->BoxLeft;
		y = InputInfo->gpi_GInfo->gi_Screen->MouseY - Info->BoxTop;

		if(x < 0 || x >= Info->BoxWidth)
		{
			NewActive = -1;
		}
		else
		{
			LONG Margin;

			if(Info->Flags & (PIF_ArrowUp | PIF_ArrowDown))
				Margin = Info->SingleHeight;
			else
				Margin = (Info->SingleHeight + 3) / 4;

				// Scroll up?

			if(y < Margin)
			{
				if(y < 0)
				{
					NewActive = -1;
				}
				else
				{
						// Topmost line concealed?

					if(Info->TopMost > 0)
					{
						Info->TopMost--;

						From	= 2;
						To		= 2 + Info->BoxHeight - 1;

						Dir = -Info->SingleHeight;
					}
				}
			}
			else
			{
				LONG Last;

				Last = Info->NumLabels - Info->TopMost;

				if(Last < Info->BoxLines)
					Last = Last * Info->SingleHeight;
				else
					Last = Info->BoxHeight;

					// Scroll up?

				if(y >= Last - Margin)
				{
					if(y >= Last)
					{
						NewActive = -1;
					}
					else
					{
							// Last line concealed?

						if(Info->TopMost + Info->BoxLines < Info->NumLabels)
						{
							Info->TopMost++;

							From	= 2;
							To		= 2 + Info->BoxHeight - 1;

							Dir = Info->SingleHeight;
						}
					}
				}
			}

			if(NewActive != -1)
			{
				NewActive = Info->TopMost + y / Info->SingleHeight;

				if(NewActive < 0)
				{
					NewActive = 0;
				}
				else
				{
					if(NewActive >= Info->NumLabels)
						NewActive = Info->NumLabels - 1;
				}
			}
		}

		if(NewActive != Info->Active || Dir != 0)
		{
			LONG	Arrow;
			UWORD *	Pens = InputInfo->gpi_GInfo->gi_DrInfo->dri_Pens;

			if(Dir != 0)
			{
				struct RastPort *RPort = Info->Window->RPort;

				Arrow = (NewActive == Info->TopMost) ? -1 : 1;

				BoxRender(Info,Pens,-1,FALSE,FALSE,0);

				if(Info->Flags & PIF_ArrowUp)
					From += Info->SingleHeight;

				if(Info->Flags & PIF_ArrowDown)
					To -= Info->SingleHeight;

				SetBPen(RPort,Pens[BACKGROUNDPEN]);
				ScrollRaster(RPort,0,Dir,4,From,4 + Info->BoxWidth - 1,To);

				if(Dir < 0)
					BoxRender(Info,Pens,Info->TopMost + 1,FALSE,FALSE,0);
				else
					BoxRender(Info,Pens,Info->TopMost + Info->BoxLines - 2,FALSE,FALSE,0);

				if(!Info->TopMost || Info->BoxLines < 3)
				{
					Info->Flags &= ~PIF_ArrowUp;

					Arrow = 0;
				}
				else
				{
					if(!(Info->Flags & PIF_ArrowUp))
					{
						Info->Flags |= PIF_ArrowUp;

						DrawOneGlyph(Info,2,-1,Pens);
					}
				}

				if(Info->TopMost + Info->BoxLines >= Info->NumLabels || Info->BoxLines < 3)
				{
					Info->Flags &= ~PIF_ArrowDown;

					Arrow = 0;
				}
				else
				{
					if(!(Info->Flags & PIF_ArrowDown))
					{
						Info->Flags |= PIF_ArrowDown;

						DrawOneGlyph(Info,2 + (Info->BoxLines - 1) * Info->SingleHeight,1,Pens);
					}
				}
			}
			else
			{
				Arrow = 0;
			}

			Info->Active = NewActive;

			BoxRender(Info,Pens,NewActive,FALSE,TRUE,Arrow);
		}
	}

	if(Redraw)
	{
		struct RastPort	* RPort;

		RPort = ObtainGIRPort(InputInfo->gpi_GInfo);
		if(RPort != NULL)
		{
			DoMethod((Object *)gadget,GM_RENDER,InputInfo->gpi_GInfo,RPort,GREDRAW_UPDATE);
			ReleaseGIRPort(RPort);
		}
	}

	return(Result);
}

STATIC VOID
SetMethod(
	struct IClass *	class,
	struct Gadget *	gadget,
	struct opSet *	SetInfo)
{
	PopInfo *			Info = INST_DATA(class,gadget);
	struct TagItem *	This;
	LONG				NewActive;
	STRPTR *			NewLabels;
	UWORD				Flags = gadget -> Flags;
	BOOL				NeedRefresh = FALSE;
	LONG				LabelCount;

	DB(kprintf("%s %ld\n",__FUNC__,__LINE__));

	DoSuperMethodA(class,(Object *)gadget,(Msg)SetInfo);

	if((This = FindTagItem(GA_Disabled,SetInfo->ops_AttrList)) != NULL)
	{
		if(This -> ti_Data && !(Flags & GFLG_DISABLED) || !This -> ti_Data && (Flags & GFLG_DISABLED))
			NeedRefresh = TRUE;
	}

	NewLabels = (STRPTR *)GetTagData(PIA_Labels,NULL,SetInfo->ops_AttrList);
	if(NewLabels != NULL)
	{
		for(LabelCount = 0 ; NewLabels[LabelCount] != NULL ; LabelCount++)
			NULL;
	}
	else
	{
		LabelCount = Info->NumLabels;
	}

	if((This = FindTagItem(PIA_Active,SetInfo->ops_AttrList)) != NULL)
	{
		NewActive = (LONG)This->ti_Data;

		if(NewActive < 0)
		{
			NewActive = 0;
		}
		else
		{
			if(NewActive >= LabelCount)
				NewActive = LabelCount - 1;
		}
	}
	else
	{
		NewActive = -1;
	}

	if((NewActive != Info->Active && NewActive != -1) || (NewLabels != NULL))
	{
		if(NewLabels == (STRPTR *)~0)
		{
			Info->Blocked = TRUE;
		}
		else
		{
			struct RastPort *RPort;

			Info->Blocked = FALSE;

			RPort = ObtainGIRPort(SetInfo->ops_GInfo);
			if(RPort != NULL)
			{
				LONG		Len;
				STRPTR *	Labels;
				LONG		MaxLen,ActiveLen;

				if(NewLabels != NULL)
				{
					struct TextExtent Extent;
					LONG i;

					for(i = 0, MaxLen = 0 ; i < LabelCount ; i++)
					{
						Len = TextFit(RPort,NewLabels[i],strlen(NewLabels[i]),&Extent,NULL,1,Info->MaxWidth,32767);

						if(Len > MaxLen)
							MaxLen = Len;
					}

					Labels = NewLabels;
				}
				else
				{
					Labels = Info->Labels;
					MaxLen = Info->MaxLen;
				}

				if(NewActive == -1)
					NewActive = Info->Active;

				if(NewActive >= LabelCount)
					NewActive = LabelCount - 1;

				Len = strlen(Labels[NewActive]);

				if(Len > MaxLen)
					Len = MaxLen;

				ActiveLen = Len;

				Info->Active	= NewActive;
				Info->ActiveLen	= ActiveLen;

				if(NewLabels != NULL)
				{
					Info->Labels	= NewLabels;
					Info->NumLabels	= LabelCount;
					Info->MaxLen	= MaxLen;
				}

				NeedRefresh = TRUE;

				ReleaseGIRPort(RPort);
			}
		}
	}

	if(!(gadget -> Flags & GFLG_DISABLED) && (This = FindTagItem(PIA_Highlight,SetInfo->ops_AttrList)))
	{
		if(This->ti_Data)
			gadget->Flags |= GFLG_SELECTED;
		else
			gadget->Flags &= ~GFLG_SELECTED;

		NeedRefresh = TRUE;
	}

	if(NeedRefresh)
	{
		struct RastPort *RPort;

		if(RPort = ObtainGIRPort(SetInfo->ops_GInfo))
		{
			DoMethod((Object *)gadget,GM_RENDER,SetInfo->ops_GInfo,RPort,GREDRAW_UPDATE);
			ReleaseGIRPort(RPort);
		}
	}
}

STATIC ULONG
RenderMethod(
	struct IClass *		class,
	struct Gadget *		gadget,
	struct gpRender *	RenderInfo)
{
	PopInfo	*Info = INST_DATA(class,gadget);

	DB(kprintf("%s %ld\n",__FUNC__,__LINE__));

	DrawContainer(RenderInfo->gpr_RPort,gadget->LeftEdge,gadget->TopEdge,gadget->Width,gadget->Height,RenderInfo->gpr_GInfo,Info,gadget->Flags & GFLG_SELECTED,gadget->Flags & GFLG_DISABLED);

	return(TRUE);
}

STATIC VOID
DisposeMethod(
	struct IClass *	class,
	Object *		object,
	Msg				msg)
{
	PopInfo *Info = INST_DATA(class,object);

	DB(kprintf("%s %ld\n",__FUNC__,__LINE__));

	if(Info->Font != NULL)
	{
		CloseFont(Info->Font);
		Info->Font = NULL;
	}

	if(Info->Window != NULL)
	{
		CloseWindow(Info->Window);
		Info->Window = NULL;
	}

	LTP_DeleteCheckGlyph(Info->CheckGlyph);
	Info->CheckGlyph = NULL;

	if(Info->FrameImage != NULL)
	{
		DisposeObject(Info->FrameImage);
		Info->FrameImage = NULL;
	}
}

STATIC ULONG
NewMethod(
	struct IClass *	class,
	Object *		object,
	struct opSet *	SetInfo)
{
	DB(kprintf("%s %ld\n",__FUNC__,__LINE__));

	object = (Object *)DoSuperMethodA(class,object,(Msg)SetInfo);
	if(object != NULL)
	{
		PopInfo *			Info = INST_DATA(class,object);
		struct TagItem *	Item;
		struct TagItem *	TagList = SetInfo -> ops_AttrList;
		struct TextAttr	*	Font = NULL;
		LONG				Width = 0;
		LONG				Height = 0;
		LONG				AspectX = 1;
		LONG				AspectY = 1;

		memset(Info,0,sizeof(PopInfo));

		while((Item = NextTagItem(&TagList)) != NULL)
		{
			switch(Item->ti_Tag)
			{
				case GA_Width:
					Width = Item->ti_Data;
					break;

				case GA_Height:
					Height = Item->ti_Data;
					break;

				case PIA_Labels:
					Info->Labels = (STRPTR *)Item->ti_Data;
					break;

				case PIA_Active:
					Info->Active = (LONG)Item->ti_Data;
					break;

				case PIA_Font:
					Font = (struct TextAttr *)Item->ti_Data;
					break;

				case PIA_CentreActive:
					Info->CentreActive = Item->ti_Data;
					break;

				case PIA_AspectX:
					AspectX = Item->ti_Data;
					break;

				case PIA_AspectY:
					AspectY = Item->ti_Data;
					break;
			}
		}

		Info->AspectX = AspectX;
		Info->AspectY = AspectY;

		if(V39)
		{
			Info->FrameImage = (struct Image *)NewObject(NULL,FRAMEICLASS,
				IA_Width,		Width,
				IA_Height,		Height,
				IA_FrameType,	FRAME_BUTTON,
			TAG_DONE);
		}

		if(Font && Info->Labels != NULL && Info->Labels != (STRPTR *)~0 && Width && Height)
		{
			while(Info->Labels[Info->NumLabels])
				Info->NumLabels++;

			if(Info->NumLabels)
			{
				Info->Font = OpenFont(Font);
				if(Info->Font != NULL)
				{
					struct RastPort RPort;

					if(Info->Active < 0)
					{
						Info->Active = 0;
					}
					else
					{
						if(Info->Active >= Info->NumLabels)
							Info->Active = Info->NumLabels - 1;
					}

					InitRastPort(&RPort);
					SetFont(&RPort,Info->Font);

					Info->PickerWidth = LTP_GetPickerWidth(Font->ta_YSize,AspectX,AspectY);

					Info->ArrowWidth	= (TextLength(&RPort,"M",1) & ~1) + 1;
					Info->ArrowHeight	= (2 * RPort.TxHeight) / 3;
					Info->ArrowTop		= (Height - Info->ArrowHeight) / 2;

					Info->MarkWidth	= Info->PickerWidth;
					Info->MarkLeft	= 4 + Info->MarkWidth;

					Info->PopLeft	= 0;
					Info->PopWidth	= Width;

					Info->LabelTop	= (Height - RPort.TxHeight) / 2;

					Width	-= 6 + Info->MarkWidth + 6;
					Height	-= 2 + RPort.TxHeight + 2;

					if(Width > 0 && Height >= 0)
					{
						struct TextExtent	Extent;
						LONG				i,Len,MaxLen;

						for(i = MaxLen = 0 ; i < Info->NumLabels ; i++)
						{
							Len = TextFit(&RPort,Info->Labels[i],strlen(Info->Labels[i]),&Extent,NULL,1,Width,32767);

							if(Len > MaxLen)
								MaxLen = Len;
						}

						if(MaxLen)
						{
							Len = strlen(Info->Labels[Info->Active]);

							if(Len > MaxLen)
								Len = MaxLen;

							Info->ActiveLen = Len;

							Info->MaxLen	= MaxLen;
							Info->MaxWidth	= Width;

							return((ULONG)object);
						}
					}

					CloseFont(Info->Font);
					Info->Font = NULL;
				}
			}
		}

		CoerceMethod(class,object,OM_DISPOSE);
	}

	return(0);
}

STATIC ULONG
ActiveMethod(
	struct IClass *		class,
	struct Gadget *		gadget,
	struct gpInput *	InputInfo)
{
	PopInfo *			Info = INST_DATA(class,gadget);
	struct RastPort *	RPort;
	struct GadgetInfo *	GInfo;

	if(Info->Blocked || (gadget->Flags & GFLG_DISABLED))
		return(GMR_NOREUSE);

	DB(kprintf("%s %ld\n",__FUNC__,__LINE__));

	GInfo = InputInfo->gpi_GInfo;

	RPort = ObtainGIRPort(GInfo);
	if(RPort != NULL)
	{
		LONG Left,Top;

		Left	= gadget->LeftEdge;
		Top		= gadget->TopEdge;

		Info->Window = NULL;

		if(Info->NumLabels > 2 && InputInfo->gpi_Mouse.X < gadget->Width - Info->MarkLeft && V39)
		{
			LONG SingleHeight,ScreenHeight,Width,Height;

			Info->Flags &= ~PIF_SingleActive;
			Info->Window = NULL;

			SetFont(RPort,Info->Font);

			SingleHeight = gadget->Height;
			ScreenHeight = GInfo->gi_Screen->Height;

			Info->LineTop = (SingleHeight - RPort->TxHeight) / 2;

			Width	= Info->PopWidth;
			Height	= 2 + SingleHeight * Info->NumLabels + 2;

			while(Height > SingleHeight && Height > ScreenHeight)
				Height -= SingleHeight;

			if(Height > SingleHeight)
			{
				struct DrawInfo *DrawInfo;
				UWORD *Pens;
				LONG LeftEdge,TopEdge;
				LONG TopMost;
				LONG Plus;

				LeftEdge	= GInfo->gi_Window->LeftEdge + Left + Info->PopLeft;
				TopEdge		= GInfo->gi_Window->TopEdge + Top + Info->LabelTop - (2 + Info->LineTop + Info->Active * SingleHeight);
				TopMost		= 0;

				if(Info->CentreActive)
					Plus = 1;
				else
					Plus = 0;

				while(TopEdge < 0 && TopMost < Info->NumLabels)
				{
					TopEdge	+= SingleHeight;
					TopMost	+= Plus;
				}

				DrawInfo = GInfo->gi_DrInfo;
				Pens = DrawInfo->dri_Pens;

				if(V39)
				{
					Info->MenuText = Pens[BARDETAILPEN];
					Info->MenuBack = Pens[BARBLOCKPEN];
				}
				else
				{
					Info->MenuText = Pens[DETAILPEN];
					Info->MenuBack = Pens[BLOCKPEN];
				}

				Info->CheckGlyph = LTP_CreateCheckGlyph((RPort->TxHeight * DrawInfo->dri_Resolution.Y) / DrawInfo->dri_Resolution.X,RPort->TxHeight,GInfo->gi_Window->RPort->BitMap,Info->MenuText,Info->MenuBack);

				if((Info->CheckGlyph != NULL) && (TopEdge >= 0) && (TopMost < Info->NumLabels))
				{
					while((Height > SingleHeight) && (TopEdge + Height > ScreenHeight))
						Height -= SingleHeight;

					if((Height > SingleHeight) && (TopEdge + Height <= ScreenHeight))
					{
						ReleaseGIRPort(RPort);

						Info->Window = OpenWindowTags(NULL,
							WA_Left,			LeftEdge,
							WA_Top,				TopEdge,
							WA_Width,			Width,
							WA_Height,			Height,
							WA_SimpleRefresh,	TRUE,
							WA_NoCareRefresh,	TRUE,
							WA_AutoAdjust,		FALSE,
							WA_CustomScreen,	GInfo->gi_Screen,
							WA_Borderless,		TRUE,

							V39 ? WA_BackFill : TAG_IGNORE,LAYERS_NOBACKFILL,
						TAG_DONE);
						if(Info->Window != NULL)
						{
							Info->TopMost		= TopMost;

							Info->BoxLeft		= LeftEdge + 4;
							Info->BoxTop		= TopEdge + 2;
							Info->BoxWidth		= Width - 8;
							Info->BoxHeight		= Height - 4;

							Info->BoxLines		= Info->BoxHeight / SingleHeight;

							Info->SingleWidth	= Width - 8;
							Info->SingleHeight	= SingleHeight;

							if(V39)
							{
								STATIC const BYTE RenderPens[] =
								{
									BACKGROUNDPEN,
									FILLPEN,
									TEXTPEN,
									FILLTEXTPEN,
									SHADOWPEN,
									SHINEPEN,
									BARDETAILPEN,
									BARBLOCKPEN
									-1
								};

								LONG i,Pen,Max = 0;

								Info->MenuTextSelect	= Pens[BARBLOCKPEN];
								Info->MenuBackSelect	= Pens[BARDETAILPEN];

								for(i = 0 ; RenderPens[i] != -1 ; i++)
								{
									if((Pen = Pens[RenderPens[i]]) > Max)
										Max = Pen;
								}

								SetMaxPen(Info->Window->RPort,Max);
							}
							else
							{
								Info->MenuTextSelect	= ~Pens[DETAILPEN];
								Info->MenuBackSelect	= ~Pens[BLOCKPEN];
							}

							SetFont(Info->Window->RPort,Info->Font);
						}

						RPort = ObtainGIRPort(GInfo);
						if(RPort == NULL)
						{
							if(Info->Window != NULL)
							{
								CloseWindow(Info->Window);
								Info->Window = NULL;
							}
						}
					}
				}
			}
		}

		if(Info->Window == NULL)
		{
			LTP_DeleteCheckGlyph(Info->CheckGlyph);
			Info->CheckGlyph = NULL;

			Info->Flags |= PIF_SingleActive;
		}

		if(((Info->Flags & PIF_SingleActive) || (Info->Window != NULL)) && (RPort != NULL))
		{
			Info->InitialActive = Info->Active;

			gadget->Flags |= GFLG_SELECTED;

			if(!Info->CheckGlyph)
				DoMethod((Object *)gadget,GM_RENDER,GInfo,RPort,GREDRAW_UPDATE);

			ReleaseGIRPort(RPort);

			if(Info->Window)
				BoxRender(Info,GInfo->gi_DrInfo->dri_Pens,Info->Active,TRUE,TRUE,0);

			return(GMR_MEACTIVE);
		}

		if(RPort != NULL)
			ReleaseGIRPort(RPort);
	}

	return(GMR_NOREUSE);
}

STATIC ULONG
InactiveMethod(
	struct IClass *			class,
	struct Gadget *			gadget,
	struct gpGoInactive *	InactiveInfo)
{
	PopInfo *Info = INST_DATA(class,gadget);

	DB(kprintf("%s %ld\n",__FUNC__,__LINE__));

	gadget->Flags &= ~GFLG_SELECTED;

	if((Info->Window != NULL) || (Info->Flags & PIF_SingleActive))
	{
		struct RastPort	*RPort;

		if(Info->Window)
		{
			CloseWindow(Info->Window);
			Info->Window = NULL;
		}

		if(!Info->CheckGlyph)
		{
			if(RPort = ObtainGIRPort(InactiveInfo->gpgi_GInfo))
			{
				DoMethod((Object *)gadget,GM_RENDER,InactiveInfo->gpgi_GInfo,RPort,GREDRAW_UPDATE);
				ReleaseGIRPort(RPort);
			}
		}

		LTP_DeleteCheckGlyph(Info->CheckGlyph);
		Info->CheckGlyph = NULL;

		Info->Flags &= ~PIF_SingleActive;
	}

	return(0);
}

#ifndef __AROS__
ULONG SAVE_DS ASM
LTP_PopupClassDispatcher(
	REG(a0) struct IClass *	class,
	REG(a2) Object *		object,
	REG(a1) Msg				msg)
#else
AROS_UFH3(ULONG, LTP_PopupClassDispatcher,
	  AROS_UFHA(struct IClass *, class, A0),
	  AROS_UFHA(Object *, object, A2),
	  AROS_UFHA(Msg, msg, A1)
)
#endif
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(NewMethod(class,object,(struct opSet *)msg));

		case OM_UPDATE:
		case OM_SET:
			SetMethod(class,(struct Gadget *)object,(struct opSet *)msg);
			break;

		case GM_RENDER:
			return(RenderMethod(class,(struct Gadget *)object,(struct gpRender *)msg));

		case GM_HITTEST:
			return(GMR_GADGETHIT);

		case GM_GOINACTIVE:
			return(InactiveMethod(class,(struct Gadget *)object,(struct gpGoInactive *)msg));

		case GM_GOACTIVE:
			return(ActiveMethod(class,(struct Gadget *)object,(struct gpInput *)msg));

		case GM_HANDLEINPUT:
			return(InputMethod(class,(struct Gadget *)object,(struct gpInput *)msg));

		case OM_DISPOSE:
			DisposeMethod(class,object,msg);

			// Falls down to the default case...
	}

	return(DoSuperMethodA(class,object,msg));
}

#endif	/* DO_POPUP_KIND */
