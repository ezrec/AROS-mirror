/*
**	FastMacroWindow.c
**
**	Fast! macro window support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* The scroller gadget IDs. */

enum { ARROW_UP=1,ARROW_DOWN };

	/* The scroller data. */

struct ScrollerInfo
{
	Object *UpImage,*DownImage;
	Object *Scroller,*UpArrow,*DownArrow;
	LONG MinWidth,MinHeight;
};

	/* The button margins. */

#define MARGIN_X	2
#define MARGIN_Y	2

#define GG_Dummy			(TAG_USER+0x4000)
#define GG_List				(GG_Dummy+1)
#define GG_Top				(GG_Dummy+2)
#define GG_Visible			(GG_Dummy+3)
#define GG_Total			(GG_Dummy+4)
#define GG_TopRow			(GG_Dummy+5)
#define GG_ScrollArrow		(GG_Dummy+6)
#define GG_Reset			(GG_Dummy+7)
#define GG_ItemWidth		(GG_Dummy+8)
#define GG_ItemHeight		(GG_Dummy+9)
#define GG_ResetNoRefresh	(GG_Dummy+10)

struct GridData
{
	struct List *	List;					/* The button list. */
	LONG			ListSize;				/* Size of the list. */

	LONG			ItemWidth;				/* The width of each button. */
	LONG			ItemHeight;				/* The height of each button. */

	LONG			MaxColumns;				/* Maximum number of buttons that can be displayed in a row. */
	LONG			MaxLines;				/* Maximum number of buttons that can be displayed in a column. */

	LONG			LastSelected;			/* The button currently selected. */
	LONG			UnselectThis;			/* The button to unselect. */

	LONG			LastWidth,LastHeight;	/* Last size of the gadget domain. */

	LONG			Top,Visible,Total;		/* Data for the scroller. */

	LONG			From,To,Direction;		/* Data for scrolling and display updates. */
};

	/* The private data. */

STATIC Class 				*GridClass;
STATIC Object				*GridObject;
STATIC struct ScrollerInfo	*GridScroller;

STATIC VOID
DeleteScroller(struct ScrollerInfo *ScrollerInfo)
{
	DisposeObject(ScrollerInfo->DownArrow);
	DisposeObject(ScrollerInfo->UpArrow);
	DisposeObject(ScrollerInfo->Scroller);
	DisposeObject(ScrollerInfo->DownImage);
	DisposeObject(ScrollerInfo->UpImage);

	FreeVecPooled(ScrollerInfo);
}

STATIC struct ScrollerInfo *
CreateScroller(struct Screen *Screen)
{
	struct ScrollerInfo *ScrollerInfo;

	if(ScrollerInfo = (struct ScrollerInfo *)AllocVecPooled(sizeof(struct ScrollerInfo),MEMF_ANY|MEMF_CLEAR))
	{
		struct DrawInfo *DrawInfo;

		if(DrawInfo = GetScreenDrawInfo(Screen))
		{
			Object *SizeImage;
			LONG SizeType;

			if(Screen->Flags & SCREENHIRES)
				SizeType = SYSISIZE_MEDRES;
			else
				SizeType = SYSISIZE_LOWRES;

			if(SizeImage = NewObject(NULL,SYSICLASS,
				SYSIA_Size,		SizeType,
				SYSIA_Which,	SIZEIMAGE,
				SYSIA_DrawInfo,	DrawInfo,
			TAG_DONE))
			{
				ULONG SizeWidth,SizeHeight;

				GetAttr(IA_Width,	SizeImage,&SizeWidth);
				GetAttr(IA_Height,	SizeImage,&SizeHeight);

				DisposeObject(SizeImage);

				if(ScrollerInfo->UpImage = NewObject(NULL,SYSICLASS,
					SYSIA_Size,		SizeType,
					SYSIA_Which,	UPIMAGE,
					SYSIA_DrawInfo,	DrawInfo,
				TAG_DONE))
				{
					ULONG ArrowHeight;

					GetAttr(IA_Height,ScrollerInfo->UpImage,&ArrowHeight);

					if(ScrollerInfo->DownImage = NewObject(NULL,SYSICLASS,
						SYSIA_Size,		SizeType,
						SYSIA_Which,	DOWNIMAGE,
						SYSIA_DrawInfo,	DrawInfo,
					TAG_DONE))
					{
						if(ScrollerInfo->Scroller = NewObject(NULL,PROPGCLASS,
							GA_Top,			Screen->WBorTop + Screen->Font->ta_YSize + 2,
							GA_RelHeight,	-(Screen->WBorTop + Screen->Font->ta_YSize + 2 + SizeHeight + 1 + 2 * ArrowHeight),
							GA_Width,		SizeWidth - 8,
							GA_RelRight,	-(SizeWidth - 5),

							PGA_Freedom,	FREEVERT,
							PGA_NewLook,	TRUE,
							PGA_Borderless,	TRUE,

							PGA_Visible,	1,
							PGA_Total,		1,
						TAG_DONE))
						{
							if(ScrollerInfo->UpArrow = NewObject(NULL,BUTTONGCLASS,
								GA_ID,			ARROW_UP,
								GA_Image,		ScrollerInfo->UpImage,
								GA_RelRight,	-(SizeWidth - 1),
								GA_RelBottom,	-(SizeHeight - 1 + 2 * ArrowHeight),
								GA_Height,		ArrowHeight,
								GA_Width,		SizeWidth,
								GA_Previous,	ScrollerInfo->Scroller,
								GA_RightBorder,	TRUE,
							TAG_DONE))
							{
								if(ScrollerInfo->DownArrow = NewObject(NULL,BUTTONGCLASS,
									GA_ID,			ARROW_DOWN,
									GA_Image,		ScrollerInfo->DownImage,
									GA_RelRight,	-(SizeWidth - 1),
									GA_RelBottom,	-(SizeHeight - 1 + ArrowHeight),
									GA_Height,		ArrowHeight,
									GA_Width,		SizeWidth,
									GA_Previous,	ScrollerInfo->UpArrow,
									GA_RightBorder,	TRUE,
								TAG_DONE))
								{
									FreeScreenDrawInfo(Screen,DrawInfo);

									ScrollerInfo->MinWidth	= (Screen->WBorLeft) + (SizeWidth);
									ScrollerInfo->MinHeight	= (Screen->WBorTop + Screen->Font->ta_YSize + 2) + (4) + (2*ArrowHeight) + (SizeHeight);

									return(ScrollerInfo);
								}
							}
						}
					}
				}
			}

			FreeScreenDrawInfo(Screen,DrawInfo);
		}

		DeleteScroller(ScrollerInfo);
	}

	return(NULL);
}

STATIC LONG
FindHitItem(struct GridData *GridData,UWORD MouseX,UWORD MouseY)
{
	LONG First,Last;
	LONG x,y,i;

	First	= GridData->Top;
	Last	= GridData->Top + GridData->Visible * GridData->MaxColumns;

	if(Last > GridData->ListSize)
		Last = GridData->ListSize;

	for(y = 0, i = First ; i < Last && y < GridData->MaxLines ; y++)
	{
		for(x = 0 ; i < Last && x < GridData->MaxColumns ; x++, i++)
		{
			if(x * GridData->ItemWidth <= MouseX && MouseX < (x+1) * GridData->ItemWidth)
			{
				if(y * GridData->ItemHeight <= MouseY && MouseY < (y+1) * GridData->ItemHeight)
					return(i);
			}
		}
	}

	return(-1);
}

STATIC VOID
ResetGridData(struct GridData *GridData)
{
	memset(GridData,0,sizeof(struct GridData));

	GridData->LastSelected = -1;
	GridData->UnselectThis = -1;
}

STATIC VOID
RecalculateSizes(struct GadgetInfo *GInfo,struct GridData *GridData)
{
	if(GridData->List)
	{
		struct RastPort LocalRastPort,*RPort;
		LONG MaxWidth,Width,Count;
		struct Node *Node;

		InitRastPort(RPort = &LocalRastPort);
		SetFont(RPort,GInfo->gi_DrInfo->dri_Font);

		MaxWidth = 0;

		for(Node = GridData->List->lh_Head, Count = 0 ; Node->ln_Succ ; Node = Node->ln_Succ, Count++)
		{
			Width = TextLength(RPort,Node->ln_Name,strlen(Node->ln_Name));

			if(Width > MaxWidth)
				MaxWidth = Width;
		}

		GridData->ItemWidth		= MARGIN_X + MaxWidth + MARGIN_X;
		GridData->ItemHeight	= MARGIN_Y + RPort->TxHeight + MARGIN_Y;
		GridData->ListSize		= Count;
	}
	else
	{
		GridData->ItemWidth		= 0;
		GridData->ItemHeight	= 0;
		GridData->ListSize		= 0;
	}
}

STATIC VOID
RecalculateArea(struct IBox *Domain,struct Gadget *Gadget,struct GridData *GridData)
{
	LONG Width,Height;

	Width	= Domain->Width		+ Gadget->Width;
	Height	= Domain->Height	+ Gadget->Height;

	GridData->LastWidth		= Domain->Width;
	GridData->LastHeight	= Domain->Height;

	if(GridData->ListSize > 0 && GridData->ItemWidth > 0 && GridData->ItemHeight > 0)
	{
		GridData->MaxColumns	= Width / GridData->ItemWidth;
		GridData->MaxLines		= Height / GridData->ItemHeight;

		if(GridData->MaxColumns > 0)
		{
			LONG Value;

			if((Value = (GridData->ListSize + GridData->MaxColumns - 1) / GridData->MaxColumns) < 1)
				Value = 1;

			GridData->Total = Value;

			if((Value = GridData->Top - (GridData->Top % GridData->MaxColumns)) < 0)
				Value = 0;

			GridData->Top = Value;

			if((Value = (GridData->ListSize - GridData->Top + GridData->MaxColumns - 1) / GridData->MaxColumns) < 1)
				Value = 1;

			if(Value > GridData->MaxLines)
				Value = GridData->MaxLines;

			GridData->Visible = Value;

			while(GridData->Top > 0 && GridData->Visible < GridData->MaxLines && GridData->Total > GridData->Visible)
			{
				if(GridData->Top > GridData->MaxColumns)
					GridData->Top -= GridData->MaxColumns;
				else
					GridData->Top = 0;

				if((Value = (GridData->ListSize - GridData->Top + GridData->MaxColumns - 1) / GridData->MaxColumns) < 1)
					Value = 1;

				if(Value > GridData->MaxLines)
					Value = GridData->MaxLines;

				GridData->Visible = Value;
			}
		}
		else
		{
			GridData->Visible	= 1;
			GridData->Total		= 1;
		}
	}
	else
	{
		GridData->MaxColumns	= 0;
		GridData->MaxLines		= 0;
		GridData->Top			= 0;
		GridData->Visible		= 1;
		GridData->Total			= 1;
	}
}

STATIC VOID
RenderItem(struct RastPort *RPort,UWORD *Pens,struct Node *Node,UWORD Left,UWORD Top,UWORD Width,UWORD Height,BOOL Selected)
{
	if(Node)
	{
		UWORD Shine,Shadow,Fill,TextPen,LabelWidth;
		LONG Delta;

		if(Selected)
		{
			Shine	= Pens[SHADOWPEN];
			Shadow	= Pens[SHINEPEN];
			Fill	= Pens[FILLPEN];
			TextPen	= Pens[FILLTEXTPEN];
		}
		else
		{
			Shine	= Pens[SHINEPEN];
			Shadow	= Pens[SHADOWPEN];
			Fill	= Pens[BACKGROUNDPEN];
			TextPen	= Pens[TEXTPEN];
		}

		LabelWidth = TextLength(RPort,Node->ln_Name,strlen(Node->ln_Name));

		SetABPenDrMd(RPort,Fill,Fill,JAM2);

		if(LabelWidth < Width - 2 * MARGIN_X)
		{
			LONG Rest;

			Delta = (Width - 2 * MARGIN_X - LabelWidth) / 2;

			if(Delta > 0)
				RectFill(RPort,Left + MARGIN_X,Top + MARGIN_Y,Left + MARGIN_X + Delta - 1,Top + Height - (MARGIN_Y + 1));

			Rest = Delta + LabelWidth;

			if(Rest < Width - 2 * MARGIN_X)
				RectFill(RPort,Left + MARGIN_X + Rest,Top + MARGIN_Y,Left + Width - (MARGIN_X + 1),Top + Height - (MARGIN_Y + 1));
		}
		else
			Delta = 0;

		RectFill(RPort,Left + 1,				Top + 1,				Left + MARGIN_X - 1,			Top + Height - 2);
		RectFill(RPort,Left + Width - MARGIN_X,	Top + 1,				Left + Width - 2,				Top + Height - 2);

		RectFill(RPort,Left + MARGIN_X,			Top + 1,				Left + Width - (MARGIN_X + 1),	Top + MARGIN_Y - 1);
		RectFill(RPort,Left + MARGIN_X,			Top + Height - MARGIN_Y,Left + Width - (MARGIN_X + 1),	Top + Height - 2);

		SetAPen(RPort,TextPen);
		Move(RPort,Left + MARGIN_X + Delta,Top + (Height - RPort->TxHeight) / 2 + RPort->TxBaseline);
		Text(RPort,Node->ln_Name,strlen(Node->ln_Name));

		SetAPen(RPort,Shine);
		RectFill(RPort,Left,Top,Left + Width - 1,Top);
		RectFill(RPort,Left,Top + 1,Left,Top + Height - 1);

		SetAPen(RPort,Shadow);
		RectFill(RPort,Left + Width - 1,Top + 1,Left + Width - 1,Top + Height - 1);
		RectFill(RPort,Left + 1,Top + Height - 1,Left + Width - 2,Top + Height - 1);
	}
	else
		EraseRect(RPort,Left,Top,Left + Width - 1,Top + Height - 1);
}

STATIC struct Gadget *
NewMethod(Class *class,struct Gadget *gadget,struct opSet *msg)
{
	struct Gadget *g;

	if(g = (struct Gadget *)DoSuperMethodA(class,(Object *)gadget,(Msg)msg))
	{
		ResetGridData(INST_DATA(class,g));

		CoerceMethod(class,(Object *)g,OM_SET,msg->ops_AttrList,msg->ops_GInfo);
	}

	return(g);
}

STATIC VOID
NotifyGrid(Class *class,Object *object,struct GadgetInfo *GInfo,struct opUpdate *msg,struct GridData *GridData)
{
	struct TagItem Tags[4];
	LONG Top,Total,Visible;

	if(!GridData->Visible || GridData->Total == GridData->Visible || GridData->MaxColumns < 1)
	{
		Top		= 0;
		Total	= 1;
		Visible	= 1;
	}
	else
	{
		Top		= GridData->Top / GridData->MaxColumns;
		Total	= GridData->Total;
		Visible	= GridData->Visible;
	}

	Tags[0].ti_Tag		= GG_TopRow;
		Tags[0].ti_Data	= Top;
	Tags[1].ti_Tag		= GG_Total;
		Tags[1].ti_Data	= Total;
	Tags[2].ti_Tag		= GG_Visible;
		Tags[2].ti_Data	= Visible;
	Tags[3].ti_Tag		= TAG_DONE;

	DoSuperMethod(class,object,OM_NOTIFY,Tags,GInfo,((msg->MethodID == OM_UPDATE) ? (msg->opu_Flags) : 0L));
}

STATIC VOID
SetGrid(Class *class,struct Gadget *gadget,struct opSet *msg,...)
{
	va_list Args;

	va_start(Args,msg);
	CoerceMethod(class,(Object *)gadget,OM_SET,Args,msg->ops_GInfo);
	va_end(Args);
}

STATIC ULONG
SetMethod(Class *class,struct Gadget *gadget,struct opSet *msg)
{
	struct GridData *GridData = INST_DATA(class,gadget);
	struct TagItem *List,*Item;
	struct RastPort *RPort;
	LONG Delta;

	List = msg->ops_AttrList;

	while(Item = NextTagItem(&List))
	{
		switch(Item->ti_Tag)
		{
			case GG_List:

				if(msg->ops_GInfo != NULL)
					CoerceMethod(class,(Object *)gadget,GM_GOINACTIVE,msg->ops_GInfo,0);

				ResetGridData(GridData);

				GridData->List = (struct List *)Item->ti_Data;

				break;

			case GG_Top:

				if(msg->ops_GInfo != NULL && GridData->MaxColumns > 0)
				{
					LONG NewTop;

					NewTop = (LONG)Item->ti_Data;

					if(NewTop + (GridData->MaxLines - 1) * GridData->MaxColumns >= GridData->ListSize)
						NewTop = GridData->ListSize - (GridData->MaxLines - 1) * GridData->MaxColumns - 1;

					if(NewTop < 0)
						NewTop = 0;

					if(NewTop != GridData->Top)
					{
						UWORD HowTo;

						HowTo = GREDRAW_REDRAW;

						if(NewTop % GridData->MaxColumns == GridData->Top % GridData->MaxColumns)
						{
							LONG Delta;

							Delta = (NewTop - GridData->Top) / GridData->MaxColumns;

							CoerceMethod(class,(Object *)gadget,GM_GOINACTIVE,msg->ops_GInfo,0);

							if(ABS(Delta) < GridData->MaxLines - 1)
							{
								HowTo = GREDRAW_UPDATE;

								GridData->Direction = GridData->ItemHeight * Delta;

								if(Delta < 0)
								{
									GridData->From	= NewTop;
									GridData->To	= NewTop + (-Delta) * GridData->MaxColumns - 1;
								}
								else
								{
									GridData->From	= NewTop + (GridData->MaxLines - Delta) * GridData->MaxColumns;
									GridData->To	= NewTop + GridData->MaxLines * GridData->MaxColumns - 1;
								}
							}
						}

						GridData->Top = NewTop;

						if(RPort = ObtainGIRPort(msg->ops_GInfo))
						{
							CoerceMethod(class,(Object *)gadget,GM_RENDER,msg->ops_GInfo,RPort,HowTo);

							ReleaseGIRPort(RPort);
						}

						GridData->Direction = 0;
					}
				}

				break;

			case GG_TopRow:

				SetGrid(class,gadget,msg,
					GG_Top,Item->ti_Data * GridData->MaxColumns,
				TAG_DONE);

				break;

			case GG_ScrollArrow:

				switch(Item->ti_Data)
				{
					case ARROW_UP:

						Delta = -1;
						break;

					case ARROW_DOWN:

						Delta = 1;
						break;

					default:

						Delta = 0;
						break;
				}

				if(Delta != 0)
				{
					SetGrid(class,gadget,msg,
						GG_Top,GridData->Top + Delta * GridData->MaxColumns,
					TAG_DONE);

					NotifyGrid(class,(Object *)gadget,msg->ops_GInfo,(struct opUpdate *)msg,GridData);
				}

				break;

			case GG_ResetNoRefresh:
			case GG_Reset:

				RecalculateSizes(msg->ops_GInfo,GridData);
				RecalculateArea(&msg->ops_GInfo->gi_Domain,gadget,GridData);

				if(Item->ti_Tag == GG_Reset)
				{
					if(RPort = ObtainGIRPort(msg->ops_GInfo))
					{
						CoerceMethod(class,(Object *)gadget,GM_RENDER,msg->ops_GInfo,RPort,GREDRAW_REDRAW);

						ReleaseGIRPort(RPort);
					}

					NotifyGrid(class,(Object *)gadget,msg->ops_GInfo,(struct opUpdate *)msg,GridData);
				}

				break;
		}
	}

	return(DoSuperMethodA(class,(Object *)gadget,(Msg)msg));
}

STATIC ULONG
GetMethod(Class *class,struct Gadget *gadget,struct opGet *msg)
{
	struct GridData *GridData = INST_DATA(class,gadget);

	switch(msg->opg_AttrID)
	{
		case GG_ItemWidth:

			*msg->opg_Storage = GridData->ItemWidth;
			return(1);

		case GG_ItemHeight:

			*msg->opg_Storage = GridData->ItemHeight;
			return(1);

		default:

			return(DoSuperMethodA(class,(Object *)gadget,(Msg)msg));
	}
}

STATIC ULONG
RenderMethod(Class *class,struct Gadget *gadget,struct gpRender *msg)
{
	struct GridData *GridData = INST_DATA(class,gadget);

	if(GridData->List != NULL && GridData->ListSize == 0)
	{
		RecalculateSizes(msg->gpr_GInfo,GridData);
		RecalculateArea(&msg->gpr_GInfo->gi_Domain,gadget,GridData);

		NotifyGrid(class,(Object *)gadget,msg->gpr_GInfo,(struct opUpdate *)msg,GridData);
	}
	else
	{
		if(msg->gpr_GInfo->gi_Domain.Width != GridData->LastWidth || msg->gpr_GInfo->gi_Domain.Height != GridData->LastHeight)
			return(0);
	}

	DB(kprintf("listsize = %ld\n",GridData->ListSize));

	if(GridData->ListSize == 0)
		EraseRect(msg->gpr_RPort,gadget->LeftEdge,gadget->TopEdge,gadget->LeftEdge + gadget->Width - 1,gadget->TopEdge + gadget->Height - 1);
	else
	{
		UWORD Left,Top,Width,Height;
		struct RastPort *RPort;
		struct Node *Node;
		UWORD *Pens;
		LONG i,x,y;

		RPort	= msg->gpr_RPort;
		Pens	= msg->gpr_GInfo->gi_DrInfo->dri_Pens;

		Left	= gadget->LeftEdge;
		Top		= gadget->TopEdge;
		Width	= GridData->ItemWidth;
		Height	= GridData->ItemHeight;

		for(i = 0, Node = GridData->List->lh_Head ; i < GridData->Top && Node->ln_Succ ; i++, Node = Node->ln_Succ);

		if(!Node->ln_Succ)
			Node = NULL;

		DB(kprintf("maxlines %ld maxcolumns %ld node %08lx\n",GridData->MaxLines,GridData->MaxColumns,Node));

		if(msg->gpr_Redraw == GREDRAW_UPDATE)
		{
			if(GridData->Direction == 0)
			{
				for(y = 0 ; y < GridData->MaxLines ; y++)
				{
					for(x = 0 ; x < GridData->MaxColumns ; x++)
					{
						if(i == GridData->LastSelected || i == GridData->UnselectThis)
							RenderItem(RPort,Pens,Node,Left + x * Width,Top + y * Height,Width,Height,(BOOL)(i == GridData->LastSelected));

						if(Node)
						{
							if(Node->ln_Succ->ln_Succ)
								Node = Node->ln_Succ;
							else
								Node = NULL;
						}

						i++;
					}
				}
			}
			else
			{
				ScrollRasterBF(RPort,0,GridData->Direction,gadget->LeftEdge,gadget->TopEdge,gadget->LeftEdge + Width * GridData->MaxColumns - 1,gadget->TopEdge + Height * GridData->MaxLines - 1);

				BeginUpdate(RPort->Layer);
				EndUpdate(RPort->Layer,TRUE);

				for(y = 0 ; y < GridData->MaxLines ; y++)
				{
					for(x = 0 ; x < GridData->MaxColumns ; x++)
					{
						if(GridData->From <= i && i <= GridData->To)
							RenderItem(RPort,Pens,Node,Left + x * Width,Top + y * Height,Width,Height,(BOOL)(i == GridData->LastSelected));

						if(Node)
						{
							if(Node->ln_Succ->ln_Succ)
								Node = Node->ln_Succ;
							else
								Node = NULL;
						}

						i++;
					}
				}
			}
		}
		else
		{
			if(GridData->LastSelected == -1)
			{
				for(y = 0 ; y < GridData->MaxLines ; y++)
				{
					for(x = 0 ; x < GridData->MaxColumns ; x++)
					{
						RenderItem(RPort,Pens,Node,Left + x * Width,Top + y * Height,Width,Height,FALSE);

						if(Node)
						{
							if(Node->ln_Succ->ln_Succ)
								Node = Node->ln_Succ;
							else
								Node = NULL;
						}
					}
				}
			}
			else
			{
				for(y = 0 ; y < GridData->MaxLines ; y++)
				{
					for(x = 0 ; x < GridData->MaxColumns ; x++)
					{
						RenderItem(RPort,Pens,Node,Left + x * Width,Top + y * Height,Width,Height,(BOOL)(i == GridData->LastSelected));

						if(Node)
						{
							if(Node->ln_Succ->ln_Succ)
								Node = Node->ln_Succ;
							else
								Node = NULL;
						}

						i++;
					}
				}
			}
		}
	}

	return(0);
}

STATIC ULONG
HitTestMethod(Class *class,struct Gadget *gadget,struct gpHitTest *msg)
{
	if(FindHitItem(INST_DATA(class,gadget),msg->gpht_Mouse.X,msg->gpht_Mouse.Y) != -1)
		return(GMR_GADGETHIT);
	else
		return(0);
}

STATIC ULONG
GoActiveMethod(Class *class,struct Gadget *gadget,struct gpInput *msg)
{
	struct GridData *GridData = INST_DATA(class,gadget);
	LONG Selected;

	Selected = FindHitItem(GridData,msg->gpi_Mouse.X,msg->gpi_Mouse.Y);

	if(Selected == -1)
		return(GMR_NOREUSE);
	else
	{
		struct RastPort *RPort;

		GridData->UnselectThis = -1;
		GridData->LastSelected = Selected;

		if(RPort = ObtainGIRPort(msg->gpi_GInfo))
		{
			CoerceMethod(class,(Object *)gadget,GM_RENDER,msg->gpi_GInfo,RPort,GREDRAW_UPDATE);

			ReleaseGIRPort(RPort);
		}

		return(GMR_MEACTIVE);
	}
}

STATIC ULONG
GoInactiveMethod(Class *class,struct Gadget *gadget,struct gpGoInactive *msg)
{
	struct GridData *GridData = INST_DATA(class,gadget);

	if(GridData->LastSelected != -1)
	{
		struct RastPort *RPort;

		GridData->UnselectThis = GridData->LastSelected;
		GridData->LastSelected = -1;

		if(RPort = ObtainGIRPort(msg->gpgi_GInfo))
		{
			CoerceMethod(class,(Object *)gadget,GM_RENDER,msg->gpgi_GInfo,RPort,GREDRAW_UPDATE);

			ReleaseGIRPort(RPort);
		}

		GridData->UnselectThis = -1;
	}

	return(0);
}

STATIC ULONG
HandleInputMethod(Class *class,struct Gadget *gadget,struct gpInput *msg)
{
	struct GridData *GridData;
	LONG Selected;

	if(msg->gpi_IEvent->ie_Code == MENUDOWN)
	{
		CoerceMethod(class,(Object *)gadget,GM_GOINACTIVE,msg->gpi_GInfo,0);

		return(GMR_NOREUSE);
	}

	GridData = INST_DATA(class,gadget);

	if(msg->gpi_IEvent->ie_Code == SELECTUP)
	{
		ULONG Result;

		if(GridData->LastSelected == -1)
			Result = GMR_NOREUSE;
		else
		{
			Result = GMR_NOREUSE | GMR_VERIFY;

			*msg->gpi_Termination = GridData->LastSelected;
		}

		CoerceMethod(class,(Object *)gadget,GM_GOINACTIVE,msg->gpi_GInfo,0);

		return(Result);
	}

	if(msg->gpi_IEvent->ie_Class == IECLASS_TIMER)
	{
		if(msg->gpi_Mouse.Y < 0)
		{
			SetGadgetAttrs(gadget,msg->gpi_GInfo->gi_Window,msg->gpi_GInfo->gi_Requester,
				GG_Top,GridData->Top - GridData->MaxColumns,
			TAG_DONE);

			NotifyGrid(class,(Object *)gadget,msg->gpi_GInfo,(struct opUpdate *)msg,GridData);
		}
		else
		{
			if(msg->gpi_Mouse.Y >= GridData->Visible * GridData->ItemHeight)
			{
				SetGadgetAttrs(gadget,msg->gpi_GInfo->gi_Window,msg->gpi_GInfo->gi_Requester,
					GG_Top,GridData->Top + GridData->MaxColumns,
				TAG_DONE);

				NotifyGrid(class,(Object *)gadget,msg->gpi_GInfo,(struct opUpdate *)msg,GridData);
			}
		}
	}

	Selected = FindHitItem(GridData,msg->gpi_Mouse.X,msg->gpi_Mouse.Y);

	if(Selected != GridData->LastSelected)
	{
		struct RastPort *RPort;

		GridData->UnselectThis = GridData->LastSelected;
		GridData->LastSelected = Selected;

		if(RPort = ObtainGIRPort(msg->gpi_GInfo))
		{
			CoerceMethod(class,(Object *)gadget,GM_RENDER,msg->gpi_GInfo,RPort,GREDRAW_UPDATE);

			ReleaseGIRPort(RPort);
		}

		GridData->UnselectThis = -1;
	}

	return(GMR_MEACTIVE);
}

#ifndef __AROS__
STATIC ULONG ASM SAVE_DS
ClassDispatcher(REG(a0) Class *class,REG(a2) struct Gadget *gadget,REG(a1) Msg msg)
#else
AROS_UFH3(STATIC ULONG, ClassDispatcher,
 AROS_UFHA(Class *        , class , A0),
 AROS_UFHA(struct Gadget *, gadget, A2),
 AROS_UFHA(Msg            , msg, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	switch(msg->MethodID)
	{
		case OM_GET:

			return(GetMethod(class,gadget,(struct opGet *)msg));

		case OM_NEW:

			return((ULONG)NewMethod(class,gadget,(struct opSet *)msg));

		case OM_SET:
		case OM_UPDATE:

			return(SetMethod(class,gadget,(struct opSet *)msg));

		case GM_RENDER:

			return(RenderMethod(class,gadget,(struct gpRender *)msg));

		case GM_HITTEST:

			return(HitTestMethod(class,gadget,(struct gpHitTest *)msg));

		case GM_GOACTIVE:

			return(GoActiveMethod(class,gadget,(struct gpInput *)msg));

		case GM_GOINACTIVE:

			return(GoInactiveMethod(class,gadget,(struct gpGoInactive *)msg));

		case GM_HANDLEINPUT:

			return(HandleInputMethod(class,gadget,(struct gpInput *)msg));

		default:

			return(DoSuperMethodA(class,(Object *)gadget,msg));
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

STATIC BOOL
ChangeWindowSize(struct Window *LocalWindow,LONG MinWidth,LONG MinHeight)
{
	ULONG IntuiLock;
	LONG Left,Top,Width,Height;
	BOOL NeedChange;

	NeedChange = FALSE;

	IntuiLock = LockIBase(NULL);

	if(MinWidth > LocalWindow->Width)
	{
		Width = MinWidth;
		NeedChange = TRUE;
	}
	else
		Width = LocalWindow->Width;

	if(MinHeight > LocalWindow->Height)
	{
		Height = MinHeight;
		NeedChange = TRUE;
	}
	else
		Height = LocalWindow->Height;

	if(LocalWindow->LeftEdge + Width > LocalWindow->WScreen->Width)
	{
		Left = LocalWindow->WScreen->Width - Width;
		NeedChange = TRUE;
	}
	else
		Left = LocalWindow->LeftEdge;

	if(LocalWindow->TopEdge + Height > LocalWindow->WScreen->Height)
	{
		Top = LocalWindow->WScreen->Height - Height;
		NeedChange = TRUE;
	}
	else
		Top = LocalWindow->TopEdge;

	UnlockIBase(IntuiLock);

	if(NeedChange)
	{
		struct IntuiMessage *Message;
		BOOL Done;
		LONG i;

		ChangeWindowBox(LocalWindow,Left,Top,Width,Height);

		for(i = 0 ; i < 5*60 ; i++)
		{
			Done = FALSE;

			while(Message = (struct IntuiMessage *)GetMsg(LocalWindow->UserPort))
			{
				if(Message->Class == IDCMP_CHANGEWINDOW)
					Done = TRUE;

				ReplyMsg((struct Message *)Message);
			}

			if(Done)
				break;
			else
				WaitTOF();
		}
	}

	return(NeedChange);
}

STATIC BOOL
WindowSizeControl(struct Window *LocalWindow,Object *LocalObject)
{
	ULONG ItemWidth,ItemHeight;
	BOOL DidReset;

	GetAttr(GG_ItemWidth,	LocalObject,&ItemWidth);
	GetAttr(GG_ItemHeight,	LocalObject,&ItemHeight);

	ItemWidth	+= LocalWindow->BorderLeft	+ LocalWindow->BorderRight;
	ItemHeight	+= LocalWindow->BorderTop	+ LocalWindow->BorderBottom;

	if(ItemWidth < GridScroller->MinWidth)
		ItemWidth = GridScroller->MinWidth;

	if(ItemHeight < GridScroller->MinHeight)
		ItemHeight = GridScroller->MinHeight;

	if(ChangeWindowSize(LocalWindow,ItemWidth,ItemHeight))
	{
		SetGadgetAttrs((struct Gadget *)LocalObject,LocalWindow,NULL,
			GG_Reset,TRUE,
		TAG_DONE);

		DidReset = TRUE;
	}
	else
		DidReset = FALSE;

	WindowLimits(LocalWindow,ItemWidth,ItemHeight,LocalWindow->WScreen->Width,LocalWindow->WScreen->Height);

	return(DidReset);
}

	/* RefreshFastWindow():
	 *
	 *	Refresh the contents of the fast! macro window.
	 */

VOID
RefreshFastWindow()
{
	if(FastWindow)
	{
		SetGadgetAttrs((struct Gadget *)GridObject,FastWindow,NULL,
			GG_ResetNoRefresh,TRUE,
		TAG_DONE);

		EraseWindow(FastWindow,NULL);

		if(!WindowSizeControl(FastWindow,GridObject))
		{
			SetGadgetAttrs((struct Gadget *)GridObject,FastWindow,NULL,
				GG_Reset,TRUE,
			TAG_DONE);
		}
	}
}

	/* CloseFastWindow():
	 *
	 *	Close the fast! macro window and free the associated resources.
	 */

VOID
CloseFastWindow()
{
	CheckItem(MEN_FAST_MACROS_WINDOW,FALSE);

	if(FastWindow)
	{
		PutWindowInfo(WINDOW_FAST,FastWindow->LeftEdge,FastWindow->TopEdge,FastWindow->Width,FastWindow->Height);

		ClearMenuStrip(FastWindow);
		CloseWindowSafely(FastWindow);

		FastWindow = NULL;
	}

	if(GridObject)
	{
		DisposeObject(GridObject);
		GridObject = NULL;
	}

	if(GridScroller)
	{
		DeleteScroller(GridScroller);
		GridScroller = NULL;
	}

	if(GridClass)
	{
		FreeClass(GridClass);
		GridClass = NULL;
	}
}

	/* OpenFastWindow():
	 *
	 *	Open the fast! macro window.
	 */

BOOL
OpenFastWindow()
{
	if(FastWindow)
	{
		ActivateWindow(FastWindow);
		return(TRUE);
	}

	if(GridClass = MakeClass(NULL,GADGETCLASS,NULL,sizeof(struct GridData),NULL))
	{
		InitHook(&GridClass->cl_Dispatcher,(HOOKFUNC)ClassDispatcher,NULL);

		if(GridScroller = CreateScroller(Window->WScreen))
		{
			STATIC struct TagItem GridToPropMap[] =
			{
				GG_TopRow,	PGA_Top,
				GG_Visible,	PGA_Visible,
				GG_Total,	PGA_Total,

				TAG_DONE
			};

			if(GridObject = NewObject(GridClass,NULL,
				GA_RelVerify,	TRUE,
				GG_List,		&FastMacroList,
				ICA_TARGET,		GridScroller->Scroller,
				ICA_MAP,		GridToPropMap,
			TAG_DONE))
			{
				STATIC struct TagItem PropToGridMap[] =
				{
					PGA_Top,	GG_TopRow,

					TAG_DONE
				};

				STATIC struct TagItem ArrowToGridMap[] =
				{
					GA_ID,		GG_ScrollArrow,

					TAG_DONE
				};

				LONG Left,Top,Width,Height;

				Left	= 0;
				Top		= 0;
				Width	= 0;
				Height	= 0;

				GetWindowInfo(WINDOW_FAST,&Left,&Top,&Width,&Height,100,100);

				SetAttrs(GridScroller->Scroller,
					ICA_TARGET,		GridObject,
					ICA_MAP,		PropToGridMap,
				TAG_DONE);

				SetAttrs(GridScroller->UpArrow,
					ICA_TARGET,		GridObject,
					ICA_MAP,		ArrowToGridMap,
				TAG_DONE);

				SetAttrs(GridScroller->DownArrow,
					ICA_TARGET,		GridObject,
					ICA_MAP,		ArrowToGridMap,
				TAG_DONE);

				if(FastWindow = OpenWindowTags(NULL,
					WA_Left,			Left,
					WA_Top,				Top,
					WA_Width,			Width,
					WA_Height,			Height,
					WA_DepthGadget,		TRUE,
					WA_SimpleRefresh,	TRUE,
					WA_CloseGadget,		TRUE,
					WA_DragBar,			TRUE,
					WA_Title,			LocaleString(MSG_FASTMACROS_FAST_MACROS_TXT),
					WA_SizeGadget,		TRUE,
					WA_SizeBRight,		TRUE,
					WA_Gadgets,			GridScroller->Scroller,
					WA_NewLookMenus,	TRUE,
					WA_Activate,		TRUE,
					BackfillTag,		&BackfillHook,
					OpenWindowTag,		Window->WScreen,

					AmigaGlyph ? WA_AmigaKey  : TAG_IGNORE, AmigaGlyph,
					CheckGlyph ? WA_Checkmark : TAG_IGNORE, CheckGlyph,
				TAG_DONE))
				{
					FastWindow->UserPort = Window->UserPort;

					if(ModifyIDCMP(FastWindow,DEFAULT_IDCMP | IDCMP_CLOSEWINDOW | IDCMP_GADGETUP | IDCMP_NEWSIZE | IDCMP_CHANGEWINDOW))
					{
						WindowLimits(FastWindow,FastWindow->Width,FastWindow->Height,FastWindow->WScreen->Width,FastWindow->WScreen->Height);

						SetAttrs(GridObject,
							GA_Left,		FastWindow->BorderLeft,
							GA_Top,			FastWindow->BorderTop,
							GA_RelWidth,	-(FastWindow->BorderLeft	+ FastWindow->BorderRight),
							GA_RelHeight,	-(FastWindow->BorderTop		+ FastWindow->BorderBottom),
						TAG_DONE);

						AddGList(FastWindow,(struct Gadget *)GridObject,(UWORD)-1,1,NULL);
						RefreshGList((struct Gadget *)GridObject,FastWindow,NULL,1);

						WindowSizeControl(FastWindow,GridObject);

						SetMenuStrip(FastWindow,Menu);

						CheckItem(MEN_FAST_MACROS_WINDOW,TRUE);

						return(TRUE);
					}
					else
					{
						FastWindow->UserPort = NULL;

						CloseWindow(FastWindow);

						FastWindow = NULL;
					}
				}
			}
		}
	}

	CloseFastWindow();

	return(FALSE);
}

	/* HandleFastWindowGadget(ULONG MsgClass,UWORD MsgCode):
	 *
	 *	Process fast! macro window input.
	 */

VOID
HandleFastWindowGadget(ULONG MsgClass,UWORD MsgCode)
{
	struct MacroNode *Node;

	switch(MsgClass)
	{
		case IDCMP_GADGETUP:

			if(Node = (struct MacroNode *)GetListNode(MsgCode,&FastMacroList))
			{
				if(Node->Code[0])
					SerialCommand(Node->Code);
			}

			break;

		case IDCMP_NEWSIZE:

			SetGadgetAttrs((struct Gadget *)GridObject,FastWindow,NULL,
				GG_Reset,TRUE,
			TAG_DONE);

			break;
	}
}
