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

#include <graphics/gfxmacros.h>

#include <clib/alib_protos.h>	/* For Coerce/Do/DoSuperMethod */

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

#ifdef DO_LEVEL_KIND	/* Support code */


/*****************************************************************************/

#define FAILED (0)

/*****************************************************************************/


STATIC VOID
SliderCleanup(struct SliderClassData *sd)
{
	LONG i;

	WaitBlit();

	for(i = 0 ; i < sd->ThumbBitMap.Depth ; i++)
	{
		if(sd->ThumbBitMap.Planes[i] != NULL)
		{
			FreeRaster(sd->ThumbBitMap.Planes[i],sd->ThumbWidth * 2,sd->ThumbHeight);
			sd->ThumbBitMap.Planes[i] = NULL;
		}
	}
}

/****************************************************************************/

#define SCALE_UP(a)		((a) * dri->dri_Resolution.Y)/dri->dri_Resolution.X
#define SCALE_DOWN(a)	((a) * dri->dri_Resolution.X)/dri->dri_Resolution.Y

/****************************************************************************/

LONG
LTP_QuerySliderSize(struct DrawInfo *dri,WORD Reference,WORD freedom,WORD useTicks)
{
	LONG thumbWidth,thumbHeight,separatorSpace,pipSpace,result;

	if(Reference == 0)
		Reference = dri->dri_Font->tf_YSize;

	thumbHeight = (Reference + 1) & ~1;

	if(thumbHeight < 8)
		thumbHeight = 8;

	thumbWidth = (((thumbHeight * 1618) / 1000) + 1) & ~1;

	if(thumbWidth < 8)
		thumbWidth = 8;

	thumbHeight = SCALE_DOWN(thumbHeight);

	if(thumbHeight < 8)
		thumbHeight = 8;

	if(freedom == FREEVERT)
	{
		separatorSpace	= 2;
		pipSpace		= SCALE_UP(4);

		if(useTicks != TICKS_None)
		{
			result = separatorSpace + pipSpace + separatorSpace + thumbWidth + separatorSpace;

			if(useTicks == TICKS_Both)
				result += pipSpace + separatorSpace;
		}
		else
		{
			result = separatorSpace + thumbWidth + separatorSpace;
		}
	}
	else
	{
		separatorSpace	= 2;
		pipSpace		= SCALE_DOWN(4);

		if(useTicks != TICKS_None)
		{
			result = separatorSpace + pipSpace + separatorSpace + thumbHeight + separatorSpace;

			if(useTicks == TICKS_Both)
				result += pipSpace + separatorSpace;
		}
		else
		{
			result = separatorSpace + thumbHeight + separatorSpace;
		}
	}

	return(result);
}

STATIC BOOL
SliderSetup(struct SliderClassData *sd,WORD Reference,LONG Dimension)
{
	struct DrawInfo *dri = sd->DrawInfo;
	BOOL IsVertical = sd->IsVertical;
	BOOL GotPlanes;
	LONG i;

	if(Reference == 0)
		Reference = dri->dri_Font->tf_YSize;

	sd->ThumbHeight = (Reference + 1) & ~1;

	if(sd->ThumbHeight < 8)
		sd->ThumbHeight = 8;

	sd->ThumbWidth = (((sd->ThumbHeight * 1618) / 1000) + 1) & ~1;

	if(sd->ThumbWidth < 8)
		sd->ThumbWidth = 8;

	sd->ThumbHeight = SCALE_DOWN(sd->ThumbHeight);

	if(sd->ThumbHeight < 8)
		sd->ThumbHeight = 8;

	InitBitMap(&sd->ThumbBitMap,min(8,dri->dri_Depth),sd->ThumbWidth * 2,sd->ThumbHeight);

	GotPlanes = TRUE;

	for(i = 0 ; i < sd->ThumbBitMap.Depth ; i++)
	{
		sd->ThumbBitMap.Planes[i] = AllocRaster(sd->ThumbWidth * 2,sd->ThumbHeight);
		if(sd->ThumbBitMap.Planes[i] == NULL)
		{
			GotPlanes = FALSE;
			break;
		}
	}

	if(GotPlanes)
	{
		struct RastPort rp;
		UWORD *pens = dri->dri_Pens;
		LONG width,height;

		width = height = Dimension;

		InitRastPort(&rp);
		rp.BitMap = &sd->ThumbBitMap;

		SetRast(&rp,pens[BACKGROUNDPEN]);

		SetDrMd(&rp,JAM1);

		SetAPen(&rp,pens[SHINEPEN]);

		if(IsVertical)
		{
			Move(&rp,0,(sd->ThumbHeight / 2) - 2);
			Draw(&rp,0,0);
			Draw(&rp,sd->ThumbWidth - 2,0);

			Move(&rp,0,sd->ThumbHeight - 2);
			Draw(&rp,0,sd->ThumbHeight / 2);
			Draw(&rp,sd->ThumbWidth - 2,sd->ThumbHeight / 2);

			SetAPen(&rp,pens[SHADOWPEN]);

			Move(&rp,1,(sd->ThumbHeight / 2) - 1);
			Draw(&rp,sd->ThumbWidth - 1,(sd->ThumbHeight / 2) - 1);
			Draw(&rp,sd->ThumbWidth - 1,1);

			Move(&rp,1,sd->ThumbHeight - 1);
			Draw(&rp,sd->ThumbWidth - 1,sd->ThumbHeight - 1);
			Draw(&rp,sd->ThumbWidth - 1,(sd->ThumbHeight / 2) + 1);

			sd->SeparatorSpace	= 2;
			sd->TickSpace		= SCALE_UP(4);

			if(sd->UseTicks != TICKS_None)
			{
				sd->Width		= sd->SeparatorSpace + sd->TickSpace + sd->SeparatorSpace + sd->ThumbWidth + sd->SeparatorSpace;
				sd->Height		= height;

				sd->TickTop		= sd->SeparatorSpace;
				sd->RailTop		= sd->SeparatorSpace + sd->TickSpace + sd->SeparatorSpace + (sd->ThumbWidth / 2) - 2;
				sd->ThumbTop	= sd->SeparatorSpace + sd->TickSpace + sd->SeparatorSpace;

				if(sd->UseTicks == TICKS_Both)
					sd->Width += sd->TickSpace + sd->SeparatorSpace;
			}
			else
			{
				sd->Width		= sd->SeparatorSpace + sd->ThumbWidth + sd->SeparatorSpace;
				sd->Height		= height;

				sd->TickTop		= sd->SeparatorSpace;
				sd->RailTop		= sd->SeparatorSpace + (sd->ThumbWidth / 2) - 2;
				sd->ThumbTop	= sd->SeparatorSpace;
			}

			sd->ThumbLast = sd->Height - sd->ThumbHeight;

			if(sd->ThumbMax == 0)
				sd->ThumbLeft = 0;
			else
				sd->ThumbLeft = (sd->ThumbLast * sd->ThumbPosition) / sd->ThumbMax;

			ClipBlit(&rp,0,0,&rp,sd->ThumbWidth,0,sd->ThumbWidth,sd->ThumbHeight,0xC0);
			SetAPen(&rp,pens[FILLPEN]);

			RectFill(&rp,sd->ThumbWidth + 1,1,sd->ThumbWidth + sd->ThumbWidth - 2,(sd->ThumbHeight / 2) - 2);
			RectFill(&rp,sd->ThumbWidth + 1,(sd->ThumbHeight / 2) + 1,sd->ThumbWidth + sd->ThumbWidth - 2,sd->ThumbHeight - 2);
		}
		else
		{
			Move(&rp,(sd->ThumbWidth / 2) - 2,0);
			Draw(&rp,0,0);
			Draw(&rp,0,sd->ThumbHeight - 2);

			Move(&rp,sd->ThumbWidth - 2,0);
			Draw(&rp,sd->ThumbWidth / 2,0);
			Draw(&rp,sd->ThumbWidth / 2,sd->ThumbHeight - 2);

			SetAPen(&rp,pens[SHADOWPEN]);

			Move(&rp,(sd->ThumbWidth / 2) - 1,1);
			Draw(&rp,(sd->ThumbWidth / 2) - 1,sd->ThumbHeight - 1);
			Draw(&rp,1,sd->ThumbHeight - 1);

			Move(&rp,sd->ThumbWidth - 1,1);
			Draw(&rp,sd->ThumbWidth - 1,sd->ThumbHeight - 1);
			Draw(&rp,(sd->ThumbWidth / 2) + 1,sd->ThumbHeight - 1);

			sd->SeparatorSpace	= 2;
			sd->TickSpace		= SCALE_DOWN(4);

			if(sd->UseTicks != TICKS_None)
			{
				sd->Width		= width;
				sd->Height		= sd->SeparatorSpace + sd->TickSpace + sd->SeparatorSpace + sd->ThumbHeight + sd->SeparatorSpace;

				sd->TickTop		= sd->SeparatorSpace;
				sd->RailTop		= sd->SeparatorSpace + sd->TickSpace + sd->SeparatorSpace + (sd->ThumbHeight / 2) - 2;
				sd->ThumbTop	= sd->SeparatorSpace + sd->TickSpace + sd->SeparatorSpace;

				if(sd->UseTicks == TICKS_Both)
					sd->Height += sd->TickSpace + sd->SeparatorSpace;
			}
			else
			{
				sd->Width		= width;
				sd->Height		= sd->SeparatorSpace + sd->ThumbHeight + sd->SeparatorSpace;

				sd->TickTop		= sd->SeparatorSpace;
				sd->RailTop		= sd->SeparatorSpace + (sd->ThumbHeight / 2) - 2;
				sd->ThumbTop	= sd->SeparatorSpace;
			}

			sd->ThumbLast = sd->Width - sd->ThumbWidth;

			if(sd->ThumbMax == 0)
				sd->ThumbLeft = 0;
			else
				sd->ThumbLeft = (sd->ThumbLast * sd->ThumbPosition) / sd->ThumbMax;

			ClipBlit(&rp,0,0,&rp,sd->ThumbWidth,0,sd->ThumbWidth,sd->ThumbHeight,0xC0);
			SetAPen(&rp,pens[FILLPEN]);

			RectFill(&rp,sd->ThumbWidth + 1,1,sd->ThumbWidth + (sd->ThumbWidth / 2) - 2,sd->ThumbHeight - 2);
			RectFill(&rp,sd->ThumbWidth + (sd->ThumbWidth / 2) + 1,1,sd->ThumbWidth + sd->ThumbWidth - 2,sd->ThumbHeight - 2);
		}
	}
	else
	{
		SliderCleanup(sd);
	}

	return(GotPlanes);
}

STATIC VOID
DrawTicks(struct RastPort *rp,LONG left,LONG top,UWORD *pens,struct SliderClassData *sd)
{
	if(sd->UseTicks != TICKS_None)
	{
		LONG numTicks,maxTicks;

		if(sd->IsVertical)
			maxTicks = (sd->Height - sd->ThumbHeight + 4) / 4;
		else
			maxTicks = (sd->Width - sd->ThumbWidth + 4) / 4;

		if(sd->NumTicks > 0)
			numTicks = sd->NumTicks;
		else
			numTicks = sd->ThumbMax + 1;

		while(numTicks > maxTicks && numTicks > 1)
			numTicks = numTicks / 2;

		if(numTicks > 1)
		{
			LONG x,y,i;

			if(sd->IsVertical)
			{
				for(i = 0 ; i < numTicks ; i++)
				{
					x = left + sd->TickTop;
					y = top + (i * (sd->Height - sd->ThumbHeight)) / (numTicks - 1) + (sd->ThumbHeight / 2) - 1;

					SetAPen(rp,pens[SHADOWPEN]);
					Move(rp,x,y);
					Draw(rp,x + sd->TickSpace - 1,y);

					SetAPen(rp,pens[SHINEPEN]);
					Move(rp,x,y + 1);
					Draw(rp,x + sd->TickSpace - 1,y + 1);

					if(sd->UseTicks == TICKS_Both)
					{
						x += sd->TickSpace + sd->SeparatorSpace + sd->ThumbWidth + sd->SeparatorSpace;

						SetAPen(rp,pens[SHADOWPEN]);
						Move(rp,x,y);
						Draw(rp,x + sd->TickSpace - 1,y);

						SetAPen(rp,pens[SHINEPEN]);
						Move(rp,x,y + 1);
						Draw(rp,x + sd->TickSpace - 1,y + 1);
					}
				}
			}
			else
			{
				for(i = 0 ; i < numTicks ; i++)
				{
					x = left + (i * (sd->Width - sd->ThumbWidth)) / (numTicks - 1) + (sd->ThumbWidth / 2) - 1;
					y = top + sd->TickTop;

					SetAPen(rp,pens[SHADOWPEN]);
					Move(rp,x,y);
					Draw(rp,x,y + sd->TickSpace - 1);

					SetAPen(rp,pens[SHINEPEN]);
					Move(rp,x + 1,y);
					Draw(rp,x + 1,y + sd->TickSpace - 1);

					if(sd->UseTicks == TICKS_Both)
					{
						y += sd->TickSpace + sd->SeparatorSpace + sd->ThumbHeight + sd->SeparatorSpace;

						SetAPen(rp,pens[SHADOWPEN]);
						Move(rp,x,y);
						Draw(rp,x,y + sd->TickSpace - 1);

						SetAPen(rp,pens[SHINEPEN]);
						Move(rp,x + 1,y);
						Draw(rp,x + 1,y + sd->TickSpace - 1);
					}
				}
			}
		}
	}
}

STATIC VOID
EraseHose(struct RastPort *rp,LONG left,LONG top,LONG dimension,struct SliderClassData *sd)
{
	if(sd->IsVertical)
	{
		UWORD width		= (sd->ThumbWidth - 4) / 2;
		UWORD height	= dimension;

		RectFill(rp,left,                         top,
		            left + width - 1,             top + height - 1);

		RectFill(rp,left + sd->ThumbWidth - width,top,
		            left + sd->ThumbWidth - 1,    top + height - 1);
	}
	else
	{
		UWORD width		= dimension;
		UWORD height	= (sd->ThumbHeight - 4) / 2;

		RectFill(rp,left,top,left + width - 1,top + height - 1);
		RectFill(rp,left,top + sd->ThumbHeight - height,left + width - 1,top + sd->ThumbHeight - 1);
	}
}

STATIC VOID
DrawRailLeft(struct RastPort *rp,LONG left,LONG top,UWORD *pens,struct SliderClassData *sd)
{
	if(sd->IsVertical)
	{
		if(sd->ThumbLeft > 0)
		{
			SetAPen(rp,pens[SHADOWPEN]);
			Move(rp,left + sd->RailTop,top);
			Draw(rp,left + sd->RailTop + 2,top);

			SetAPen(rp,pens[BACKGROUNDPEN]);
			WritePixel(rp,left + sd->RailTop + 3,top);

			EraseHose(rp,left + sd->ThumbTop,top,1,sd);

			if(sd->ThumbLeft > 1)
			{
				EraseHose(rp,left + sd->ThumbTop,top + 1,sd->ThumbLeft - 1,sd);
				RectFill(rp,left + sd->RailTop + 1,top + 1,left + sd->RailTop + 2,top + sd->ThumbLeft - 1);

				SetAPen(rp,pens[SHADOWPEN]);
				Move(rp,left + sd->RailTop,top + 1);
				Draw(rp,left + sd->RailTop,top + sd->ThumbLeft - 1);

				SetAPen(rp,pens[SHINEPEN]);
				Move(rp,left + sd->RailTop + 3,top + 1);
				Draw(rp,left + sd->RailTop + 3,top + sd->ThumbLeft - 1);
			}
		}
	}
	else
	{
		if(sd->ThumbLeft > 0)
		{
			SetAPen(rp,pens[SHADOWPEN]);
			Move(rp,left,top + sd->RailTop);
			Draw(rp,left,top + sd->RailTop + 2);

			SetAPen(rp,pens[BACKGROUNDPEN]);
			WritePixel(rp,left,top + sd->RailTop + 3);

			EraseHose(rp,left,top + sd->ThumbTop,1,sd);

			if(sd->ThumbLeft > 1)
			{
				EraseHose(rp,left + 1,top + sd->ThumbTop,sd->ThumbLeft - 1,sd);
				RectFill(rp,left + 1,top + sd->RailTop + 1,left + sd->ThumbLeft - 1,top + sd->RailTop + 2);

				SetAPen(rp,pens[SHADOWPEN]);
				Move(rp,left + 1,top + sd->RailTop);
				Draw(rp,left + sd->ThumbLeft - 1,top + sd->RailTop);

				SetAPen(rp,pens[SHINEPEN]);
				Move(rp,left + 1,top + sd->RailTop + 3);
				Draw(rp,left + sd->ThumbLeft - 1,top + sd->RailTop + 3);
			}
		}
	}
}

STATIC VOID
DrawRailRight(struct RastPort *rp,LONG left,LONG top,UWORD *pens,struct SliderClassData *sd)
{
	if(sd->IsVertical)
	{
		if(sd->ThumbLeft < sd->ThumbLast)
		{
			SetAPen(rp,pens[SHINEPEN]);
			Move(rp,left + sd->RailTop + 1,top + sd->Height - 1);
			Draw(rp,left + sd->RailTop + 3,top + sd->Height - 1);

			SetAPen(rp,pens[BACKGROUNDPEN]);
			WritePixel(rp,left + sd->RailTop,top + sd->Height - 1);
			EraseHose(rp,left + sd->ThumbTop,top + sd->Height - 1,1,sd);

			if(sd->ThumbLeft < sd->ThumbLast - 1)
			{
				RectFill(rp,left + sd->RailTop + 1,top + sd->ThumbLeft + sd->ThumbHeight,left + sd->RailTop + 2,top + sd->Height - 2);
				EraseHose(rp,left + sd->ThumbTop,top + sd->ThumbLeft + sd->ThumbHeight,sd->ThumbLast - sd->ThumbLeft - 1,sd);

				SetAPen(rp,pens[SHADOWPEN]);
				Move(rp,left + sd->RailTop,top + sd->ThumbLeft + sd->ThumbHeight);
				Draw(rp,left + sd->RailTop,top + sd->Height - 2);

				SetAPen(rp,pens[SHINEPEN]);
				Move(rp,left + sd->RailTop + 3,top + sd->ThumbLeft + sd->ThumbHeight);
				Draw(rp,left + sd->RailTop + 3,top + sd->Height - 2);
			}
		}
	}
	else
	{
		if(sd->ThumbLeft < sd->ThumbLast)
		{
			SetAPen(rp,pens[SHINEPEN]);
			Move(rp,left + sd->Width - 1,top + sd->RailTop + 1);
			Draw(rp,left + sd->Width - 1,top + sd->RailTop + 3);

			SetAPen(rp,pens[BACKGROUNDPEN]);
			WritePixel(rp,left + sd->Width - 1,top + sd->RailTop);
			EraseHose(rp,left + sd->Width - 1,top + sd->ThumbTop,1,sd);

			if(sd->ThumbLeft < sd->ThumbLast - 1)
			{
				RectFill(rp,left + sd->ThumbLeft + sd->ThumbWidth,top + sd->RailTop + 1,left + sd->Width - 2,top + sd->RailTop + 2);
				EraseHose(rp,left + sd->ThumbLeft + sd->ThumbWidth,top + sd->ThumbTop,sd->ThumbLast - sd->ThumbLeft - 1,sd);

				SetAPen(rp,pens[SHADOWPEN]);
				Move(rp,left + sd->ThumbLeft + sd->ThumbWidth,top + sd->RailTop);
				Draw(rp,left + sd->Width - 2,top + sd->RailTop);

				SetAPen(rp,pens[SHINEPEN]);
				Move(rp,left + sd->ThumbLeft + sd->ThumbWidth,top + sd->RailTop + 3);
				Draw(rp,left + sd->Width - 2,top + sd->RailTop + 3);
			}
		}
	}
}

STATIC VOID
EraseSlider(struct RastPort *rp,LONG left,LONG top,UWORD *pens,struct SliderClassData *sd)
{
	EraseRect(rp,left,top,left + sd->Width - 1,top + sd->Height - 1);
}

STATIC VOID
DrawSlider(struct RastPort *rp,LONG left,LONG top,UWORD selected,struct SliderClassData *sd)
{
	LONG offset;

	if(selected)
		offset = sd->ThumbWidth;
	else
		offset = 0;

	if(sd->IsVertical)
		BltBitMapRastPort(&sd->ThumbBitMap,offset,0,rp,left + sd->ThumbTop,top + sd->ThumbLeft,sd->ThumbWidth,sd->ThumbHeight,0xC0);
	else
		BltBitMapRastPort(&sd->ThumbBitMap,offset,0,rp,left + sd->ThumbLeft,top + sd->ThumbTop,sd->ThumbWidth,sd->ThumbHeight,0xC0);
}

/****************************************************************************/

#define G(o)	((struct Gadget *)(o))

/****************************************************************************/

STATIC VOID
DoSetMethod(Object *object,struct GadgetInfo *ginfo,...)
{
	va_list args;

	va_start(args,ginfo);
	DoMethod(object,OM_SET,(struct TagItem *)args,ginfo);
	va_end(args);
}

STATIC VOID
DoUpdateMethod(Object *object,struct GadgetInfo *ginfo,...)
{
	va_list args;

	va_start(args,ginfo);
	DoMethod(object,OM_UPDATE,(struct TagItem *)args,ginfo,OPUF_INTERIM);
	va_end(args);
}

STATIC ULONG
RenderMethod(
	Class *				class,
	Object *			object,
	struct gpRender *	RenderInfo)
{
	struct SliderClassData *sd = INST_DATA(class,object);
	struct RastPort *rp = RenderInfo->gpr_RPort;
	LONG left = G(object)->LeftEdge,top = G(object)->TopEdge;
	UWORD *pens = sd->DrawInfo->dri_Pens;

	SetDrMd(rp,JAM1);

	if(RenderInfo->gpr_Redraw != GREDRAW_UPDATE)
	{
		EraseSlider(rp,left,top,pens,sd);
		DrawTicks(rp,left,top,pens,sd);
	}

	DrawRailLeft(rp,left,top,pens,sd);
	DrawRailRight(rp,left,top,pens,sd);
	DrawSlider(rp,left,top,G(object)->Flags & GFLG_SELECTED,sd);

	if(G(object)->Flags & GFLG_DISABLED)
	{
		STATIC const UWORD Ghosting[2] = { 0x4444,0x1111 };

		SetAPen(rp,pens[SHADOWPEN]);
		SetDrMd(rp,JAM1);

		SetAfPt(rp,(UWORD *)Ghosting,1);
		RectFill(rp,left,top,left + G(object)->Width - 1,top + G(object)->Height - 1);
		SetAfPt(rp,NULL,0);
	}

	return(TRUE);
}

STATIC VOID
DisposeMethod(
	Class *		class,
	Object *	object)
{
	struct SliderClassData *sd = INST_DATA(class,object);

	SliderCleanup(sd);
}

STATIC BOOL
GetMethod(
	Class *			class,
	Object *		object,
	struct opGet *	GetInfo)
{
	struct SliderClassData *sd = INST_DATA(class,object);
	BOOL result;

	switch(GetInfo->opg_AttrID)
	{
		case SLA_Current:

			*GetInfo->opg_Storage = sd->ThumbPosition;
			result = TRUE;
			break;

		case SLA_Position:

			*GetInfo->opg_Storage = sd->ThumbLeft;
			result = TRUE;
			break;

		case SLA_Max:

			*GetInfo->opg_Storage = sd->ThumbMax;
			result = TRUE;
			break;

		case SLA_NumTicks:

			*GetInfo->opg_Storage = sd->NumTicks;
			result = TRUE;
			break;

		default:

			result = DoSuperMethodA(class,object,(Msg)GetInfo);
			break;
	}

	return(result);
}

STATIC BOOL
SetMethod(
	Class *			class,
	Object *		object,
	struct opSet *	SetInfo)
{
	struct SliderClassData *sd = INST_DATA(class,object);
	struct TagItem *tag,*list = SetInfo->ops_AttrList;
	LONG redraw = GREDRAW_UPDATE;
	BOOL rawPosition = FALSE;
	BOOL updatePosition = FALSE;
	BOOL result = FALSE;
	BOOL refresh;
	BOOL disabled;

	disabled = (BOOL)((G(object)->Flags & GFLG_DISABLED) != 0);

	refresh = DoSuperMethodA(class,object,(Msg)SetInfo);

	while((tag = NextTagItem(&list)) != NULL)
	{
		switch(tag->ti_Tag)
		{
			case GA_Disabled:

				if(disabled != (BOOL)(tag->ti_Data != 0))
				{
					refresh = TRUE;
					redraw = GREDRAW_REDRAW;
				}

				break;

			case SLA_Current:

				result = TRUE;

				if(sd->ThumbPosition != tag->ti_Data)
				{
					refresh = TRUE;

					sd->ThumbPosition = tag->ti_Data;
					updatePosition = TRUE;
				}

				break;

			case SLA_Max:

				result = TRUE;

				if(sd->ThumbMax != tag->ti_Data)
				{
					refresh = TRUE;
					redraw = GREDRAW_REDRAW;

					sd->ThumbMax = tag->ti_Data;
					updatePosition = TRUE;
				}

				break;

			case SLA_Position:

				result = TRUE;

				if(sd->ThumbLeft != tag->ti_Data)
				{
					refresh = TRUE;
					rawPosition = TRUE;

					sd->ThumbLeft = tag->ti_Data;
					updatePosition = TRUE;
				}

				break;

			case SLA_NumTicks:

				result = TRUE;

				if(sd->NumTicks != tag->ti_Data)
				{
					sd->NumTicks = tag->ti_Data;
					redraw = GREDRAW_REDRAW;
				}

				break;
		}
	}

	if(refresh)
	{
		struct RastPort *rp;

		if(updatePosition)
		{
			if(rawPosition)
			{
				if(sd->ThumbLeft > sd->ThumbLast)
					sd->ThumbLeft = sd->ThumbLast;

				if(sd->ThumbLast == 0)
					sd->ThumbPosition = 0;
				else
					sd->ThumbPosition = (sd->ThumbLeft * sd->ThumbMax + (sd->ThumbLast / 2)) / sd->ThumbLast;
			}
			else
			{
				if(sd->ThumbMax == 0)
					sd->ThumbLeft = 0;
				else
					sd->ThumbLeft = (sd->ThumbLast * sd->ThumbPosition) / sd->ThumbMax;

				if(sd->ThumbLeft > sd->ThumbLast)
					sd->ThumbLeft = sd->ThumbLast;
			}
		}

		rp = ObtainGIRPort(SetInfo->ops_GInfo);
		if(rp != NULL)
		{
			DoMethod(object,GM_RENDER,SetInfo->ops_GInfo,rp,redraw);

			ReleaseGIRPort(rp);
		}
	}

	return(result);
}

STATIC Object *
NewMethod(
	Class *			class,
	Object *		object,
	struct opSet *	SetInfo)
{
	Object *result;

	result = (Object *)DoSuperMethodA(class,object,(Msg)SetInfo);
	if(result != NULL)
	{
		struct SliderClassData *sd = INST_DATA(class,result);
		BOOL failed = FALSE;
		ULONG freedom = FREEHORIZ;
		struct TagItem *tag,*list = SetInfo->ops_AttrList;
		WORD reference = 0;

		sd->UseTicks = TICKS_None;

		while((tag = NextTagItem(&list)) != NULL)
		{
			switch(tag->ti_Tag)
			{
				case PGA_Freedom:

					freedom = tag->ti_Data;
					break;

				case SLA_DrawInfo:

					sd->DrawInfo = (struct DrawInfo *)tag->ti_Data;
					break;

				case SLA_Current:

					sd->ThumbPosition = tag->ti_Data;
					break;

				case SLA_Max:

					sd->ThumbMax = tag->ti_Data;
					break;

				case SLA_UseTicks:

					sd->UseTicks = tag->ti_Data;
					break;

				case SLA_Reference:

					reference = tag->ti_Data;
					break;
			}
		}

		switch(freedom)
		{
			case FREEVERT:

				sd->IsVertical = TRUE;
				break;

			case FREEHORIZ:

				sd->IsVertical = FALSE;
				break;

			default:

				failed = TRUE;
				break;
		}

		if(sd->DrawInfo == NULL)
			failed = TRUE;

		if(NOT failed)
		{
			if(freedom == FREEVERT && G(result)->Height < 10)
				failed = TRUE;

			if(freedom == FREEHORIZ && G(result)->Width < 10)
				failed = TRUE;
		}

		if(NOT failed)
		{
			LONG dimension,position;

			if(freedom == FREEVERT)
				dimension = G(result)->Height;
			else
				dimension = G(result)->Width;

			if(SliderSetup(sd,reference,dimension) == FAILED)
				failed = TRUE;
			else
			{
				ULONG dimTag,posTag;

				if(freedom == FREEVERT)
				{
					dimTag		= GA_Width;
					dimension	= sd->Width;

					posTag		= GA_Left;
					position	= G(result)->LeftEdge + (G(result)->Width - sd->Width) / 2;
				}
				else
				{
					dimTag		= GA_Height;
					dimension	= sd->Height;

					posTag		= GA_Top;
					position	= G(result)->TopEdge + (G(result)->Height - sd->Height) / 2;
				}

				DoMethod(result,OM_SET,SetInfo->ops_AttrList,NULL);

				DoSetMethod(result,NULL,
					dimTag,	dimension,
					posTag,	position,
				TAG_DONE);
			}
		}

		if(failed)
		{
			CoerceMethod(class,result,OM_DISPOSE);
			result = NULL;
		}
	}

	return(result);
}

STATIC VOID
HandleSlide(
	Object *			object,
	struct SliderClassData *	sd,
	struct gpInput *	InputInfo)
{
	LONG mousePosition;
	BOOL isValid;

	if(sd->IsVertical)
	{
		mousePosition = InputInfo->gpi_Mouse.Y;

		isValid = (BOOL)(mousePosition >= 0 && mousePosition < G(object)->Height &&
		                 InputInfo->gpi_Mouse.X >= 0 && InputInfo->gpi_Mouse.X < G(object)->Width);
	}
	else
	{
		mousePosition = InputInfo->gpi_Mouse.X;

		isValid = (BOOL)(mousePosition >= 0 && mousePosition < G(object)->Width &&
		                 InputInfo->gpi_Mouse.Y >= 0 && InputInfo->gpi_Mouse.Y < G(object)->Height);
	}

	if(isValid)
	{
		LONG direction = 0,size;

		if(sd->IsVertical)
			size = sd->ThumbHeight;
		else
			size = sd->ThumbWidth;

		if(mousePosition < sd->ThumbLeft)
		{
			if(sd->ThumbPosition > 0)
				direction = -1;
		}
		else if (mousePosition >= sd->ThumbLeft + size)
		{
			if(sd->ThumbPosition + 1 <= sd->ThumbMax)
				direction = 1;
		}

		if(direction != 0)
		{
			ULONG newThumbPosition = sd->ThumbPosition + direction;
			LONG thumbLeft;

			if(sd->ThumbMax == 0)
				thumbLeft = 0;
			else
				thumbLeft = (sd->ThumbLast * newThumbPosition) / sd->ThumbMax;

			if(thumbLeft > sd->ThumbLast)
				thumbLeft = sd->ThumbLast;

			if(direction < 0 && thumbLeft < mousePosition ||
			   direction > 0 && thumbLeft + size >= mousePosition)
			{
				if(direction > 0)
					mousePosition -= size - 1;

				DoUpdateMethod(object,InputInfo->gpi_GInfo,
					SLA_Position,	mousePosition,
				TAG_DONE);
			}
			else
			{
				DoUpdateMethod(object,InputInfo->gpi_GInfo,
					SLA_Current,	newThumbPosition,
				TAG_DONE);
			}
		}
	}
}

STATIC ULONG
InputMethod(
	Class *				class,
	Object *			object,
	struct gpInput *	InputInfo)
{
	struct SliderClassData *sd = INST_DATA(class,object);
	ULONG result = GMR_MEACTIVE;

	if(InputInfo->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
	{
		LONG position;

		switch(InputInfo->gpi_IEvent->ie_Code)
		{
			case SELECTUP:

				result = GMR_REUSE | GMR_VERIFY;
				break;

			case IECODE_NOBUTTON:

				if(sd->DirectHit)
				{
					if(sd->IsVertical)
						position = InputInfo->gpi_Mouse.Y + sd->ActiveOffset;
					else
						position = InputInfo->gpi_Mouse.X + sd->ActiveOffset;

					if(position < 0)
						position = 0;
					else if (position > sd->ThumbLast)
						position = sd->ThumbLast;

					if(position != sd->ThumbLeft)
					{
						DoSetMethod(object,InputInfo->gpi_GInfo,
							SLA_Position,	position,
						TAG_DONE);
					}
				}

				break;
		}
	}

	return(result);
}

STATIC ULONG
ActiveMethod(
	Class *				class,
	Object *			object,
	struct gpInput *	InputInfo)
{
	struct SliderClassData *sd = INST_DATA(class,object);
	ULONG result;
	struct RastPort *rp;

	rp = ObtainGIRPort(InputInfo->gpi_GInfo);
	if(rp != NULL)
	{
		if(sd->DirectHit)
		{
			G(object)->Flags |= GFLG_SELECTED;
			DoMethod(object,GM_RENDER,InputInfo->gpi_GInfo,rp,GREDRAW_UPDATE);
		}

		ReleaseGIRPort(rp);

		if(sd->IsVertical)
			sd->ActiveOffset = sd->ThumbLeft - InputInfo->gpi_Mouse.Y;
		else
			sd->ActiveOffset = sd->ThumbLeft - InputInfo->gpi_Mouse.X;

		if(NOT sd->DirectHit)
			HandleSlide(object,sd,InputInfo);

		result = GMR_MEACTIVE;
	}
	else
	{
		result = GMR_NOREUSE;
	}

	return(result);
}

STATIC ULONG
InactiveMethod(
	Class *					class,
	Object *				object,
	struct gpGoInactive *	InactiveInfo)
{
	struct RastPort *rp;

	rp = ObtainGIRPort(InactiveInfo->gpgi_GInfo);
	if(rp != NULL)
	{
		struct SliderClassData *sd = INST_DATA(class,object);

		if(sd->DirectHit)
		{
			G(object)->Flags &= ~GFLG_SELECTED;
			DoMethod(object,GM_RENDER,InactiveInfo->gpgi_GInfo,rp,GREDRAW_UPDATE);
		}

		ReleaseGIRPort(rp);
	}

	return(0);
}

STATIC ULONG
HitTestMethod(
	Class *				class,
	Object *			object,
	struct gpHitTest *	HitInfo)
{
	struct SliderClassData *sd = INST_DATA(class,object);
	ULONG result;

	if(sd->IsVertical)
	{
		if(HitInfo->gpht_Mouse.Y >= sd->ThumbLeft &&
		   HitInfo->gpht_Mouse.Y <  sd->ThumbLeft + sd->ThumbHeight &&
		   HitInfo->gpht_Mouse.X >= sd->ThumbTop &&
		   HitInfo->gpht_Mouse.X <  sd->ThumbTop + sd->ThumbWidth)
		{
			sd->DirectHit = TRUE;
		}
		else
		{
			sd->DirectHit = FALSE;
		}

		result = GMR_GADGETHIT;
	}
	else
	{
		if(HitInfo->gpht_Mouse.X >= sd->ThumbLeft &&
		   HitInfo->gpht_Mouse.X <  sd->ThumbLeft + sd->ThumbWidth &&
		   HitInfo->gpht_Mouse.Y >= sd->ThumbTop &&
		   HitInfo->gpht_Mouse.Y <  sd->ThumbTop + sd->ThumbHeight)
		{
			sd->DirectHit = TRUE;
		}
		else
		{
			sd->DirectHit = FALSE;
		}

		result = GMR_GADGETHIT;
	}

	return(result);
}


/*****************************************************************************/

VOID
LTP_LevelGadgetDrawLabel(LayoutHandle *Handle,ObjectNode *Node,BOOL FullRefresh)
{
	if(Node->Label)
	{
		LevelExtra *Special = &Node->Special.Level;
		LONG Current = Node->Current;

		if(FullRefresh)
			LTP_DrawObjectLabel(Handle,Node);

		if(Special->LevelFormat)
		{
			UBYTE Buffer[256];
			struct RastPort *RPort;
			LONG LabelTop,LabelLeft;
			LONG LevelWidth,LevelLen;

			RPort = &Handle->RPort;

			switch(Special->LevelPlace)
			{
				case PLACETEXT_LEFT:

					LabelLeft	= Node->Left - INTERWIDTH;
					LabelTop	= Node->Top + (Node->Height - Handle->GlyphHeight) / 2;
					break;

				case PLACETEXT_RIGHT:

					LabelLeft	= Node->Left + Node->Width + INTERWIDTH + Special->MaxLevelWidth;
					LabelTop	= Node->Top + (Node->Height - Handle->GlyphHeight) / 2;
					break;

				case PLACETEXT_ABOVE:

					LabelLeft	= Node->Left + (Node->Width - Special->MaxLevelWidth) / 2 + Special->MaxLevelWidth;
					LabelTop	= Node->Top - (Handle->GlyphHeight + INTERHEIGHT);
					break;

				case PLACETEXT_BELOW:

					LabelLeft	= Node->Left + (Node->Width - Special->MaxLevelWidth) / 2 + Special->MaxLevelWidth;
					LabelTop	= Node->Top + Node->Height + INTERHEIGHT;
					break;

				default:

					return;	/* Note: only in case none of the alignment types matches; should never happen! */
			}

			LTP_SetPens(RPort,Handle->TextPen,Handle->BackgroundPen,JAM2);

			if(Special->DispFunc)
				Current = (*Special->DispFunc)(Node->Host,Current);

			SPrintf(Buffer,Special->LevelFormat,Current);

			LevelLen = strlen(Buffer);

			LevelWidth = TextLength(RPort,Buffer,LevelLen);

			LTP_PrintText(RPort,Buffer,LevelLen,LabelLeft - LevelWidth,LabelTop);

			if(LevelWidth < Special->MaxLevelWidth)
				LTP_EraseBox(RPort,LabelLeft - Special->MaxLevelWidth,LabelTop,Special->MaxLevelWidth - LevelWidth,Handle->GlyphHeight);
		}
	}
}

LONG
LTP_GetCurrentLevel(ObjectNode *Node)
{
	LONG Current,Max;

	GetAttr(SLA_Current,Node->Host,(ULONG *)&Current);
	GetAttr(SLA_Max,Node->Host,(ULONG *)&Max);

	if(Node->Special.Level.Freedom == FREEVERT)
		Current = Max - Current;

	return(Current + Node->Special.Level.Plus);
}

/*****************************************************************************/

#ifndef __AROS__
ULONG SAVE_DS ASM
LTP_LevelClassDispatcher(REG(a0) Class *class,REG(a2) Object *object,REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, LTP_LevelClassDispatcher,
	  AROS_UFHA(Class *, class, A0),
	  AROS_UFHA(Object *, object, A2),
	  AROS_UFHA(Msg, msg, A1)
)
#endif
{
        AROS_USERFUNC_INIT
	
	ULONG result;

	switch(msg->MethodID)
	{
		case OM_NEW:

			result = (ULONG)NewMethod(class,object,(struct opSet *)msg);
			break;

		case OM_UPDATE:
		case OM_SET:

			result = SetMethod(class,object,(struct opSet *)msg);
			break;

		case OM_GET:

			result = GetMethod(class,object,(struct opGet *)msg);
			break;

		case GM_RENDER:

			result = RenderMethod(class,object,(struct gpRender *)msg);
			break;

		case GM_HITTEST:

			result = HitTestMethod(class,object,(struct gpHitTest *)msg);
			break;

		case GM_GOINACTIVE:

			result = InactiveMethod(class,object,(struct gpGoInactive *)msg);
			break;

		case GM_GOACTIVE:

			result = ActiveMethod(class,object,(struct gpInput *)msg);
			break;

		case GM_HANDLEINPUT:

			result = InputMethod(class,object,(struct gpInput *)msg);
			break;

		case OM_DISPOSE:

			DisposeMethod(class,object);

			/* Falls through to... */

		default:

			result = DoSuperMethodA(class,object,msg);
			break;
	}

	return(result);
    
        AROS_USERFUNC_EXIT
}

#endif	/* DO_LEVEL_KIND */
