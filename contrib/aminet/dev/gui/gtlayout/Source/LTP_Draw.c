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

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/


VOID
LTP_ResetRenderInfo(struct RastPort *RPort)
{
	SetAfPt(RPort,NULL,0);
	SetDrPt(RPort,0xFFFF);
	SetWrMsk(RPort,0xFF);
}


/*****************************************************************************/


VOID
LTP_GhostBox(struct RastPort *RPort,LONG Left,LONG Top,LONG Width,LONG Height,UWORD Pen)
{
	STATIC const UWORD Ghosting[2] = { 0x4444,0x1111 };

	LTP_SetPens(RPort,Pen,0,JAM1);

	SetAfPt(RPort,(UWORD *)Ghosting,1);
	LTP_FillBox(RPort,Left,Top,Width,Height);
	SetAfPt(RPort,NULL,0);
}


/*****************************************************************************/


VOID
LTP_EraseBox(struct RastPort *rp,LONG left,LONG top,LONG width,LONG height)
{
	if(width > 0 && height > 0)
		EraseRect(rp,left,top,left + width - 1,top + height - 1);
}


/*****************************************************************************/


VOID
LTP_FillBox(struct RastPort *rp,LONG left,LONG top,LONG width,LONG height)
{
	if(width > 0 && height > 0)
		RectFill(rp,left,top,left + width - 1,top + height - 1);
}


/*****************************************************************************/


VOID
LTP_DrawLine(struct RastPort *rp,LONG x0,LONG y0,LONG x1,LONG y1)
{
		/* Recent studies reveal that for horizontal and
		 * vertical lines RectFill() is in most cases faster
		 * than Move()..Draw(). This only works if we don't
		 * rely upon rp->cp_x/cp_y being updated by Draw().
		 * As of v26.11 the library no longer does this.
		 */

	if(x0 == x1 || y0 == y1)
	{
			// If there only were a `C' primitive to swap
			// two variables...

		if(x0 > x1)
			x0 ^= x1, x1 ^= x0, x0 ^= x1;

		if(y0 > y1)
			y0 ^= y1, y1 ^= y0, y0 ^= y1;

		RectFill(rp,x0,y0,x1,y1);
	}
	else
	{
		Move(rp,x0,y0);
		Draw(rp,x1,y1);
	}
}

VOID
LTP_PolyDraw(struct RastPort *rp,LONG totalPairs,LONG left,LONG top,...)
{
	LONG x0,y0,x1,y1;
	va_list args;

	x0 = left;
	y0 = top;

	va_start(args,top);

	while(--totalPairs > 0)
	{
		x1 = va_arg(args, LONG);
		y1 = va_arg(args, LONG);

		LTP_DrawLine(rp,x0,y0,x1,y1);

		x0 = x1;
		y0 = y1;
	}

	va_end(args);
}


/*****************************************************************************/


VOID
LTP_RenderBevel(struct RastPort *rp,UWORD *pens,LONG left,LONG top,LONG width,LONG height,BOOL recessed,WORD thickness)
{
	LONG pen1,pen2;

	if(recessed)
	{
		pen1 = SHADOWPEN;
		pen2 = SHINEPEN;
	}
	else
	{
		pen1 = SHINEPEN;
		pen2 = SHADOWPEN;
	}

	LTP_SetAPen(rp,pens[pen1]);
	LTP_PolyDraw(rp,3,
		left,top + height - 1,
		left,top,
		left + width - 2,top);

	if(thickness > 1)
	{
		LTP_DrawLine(rp,left + 1,top + 1,left + 1,top + height - 2);

		if(thickness > 2)
		{
			LTP_PolyDraw(rp,3,
				left + 2,top + height - 3,
				left + 2,top + 1,
				left + width - 3,top + 1);
		}
	}

	LTP_SetAPen(rp,pens[pen2]);
	LTP_PolyDraw(rp,3,
		left + width - 1,top,
		left + width - 1,top + height - 1,
		left + 1,top + height - 1);

	if(thickness > 1)
	{
		LTP_DrawLine(rp,left + width - 2,top + height - 2,left + width - 2,top + 1);

		if(thickness > 2)
		{
			LTP_PolyDraw(rp,3,
				left + width - 3,top + 2,
				left + width - 3,top + height - 2,
				left + 2,top + height - 2);
		}
	}
}

VOID
LTP_DrawBevel(LayoutHandle *handle,LONG left,LONG top,LONG width,LONG height)
{
	if(handle->BevelImage)
	{
		SetAttrs(handle->BevelImage,
			IA_Width,	width,
			IA_Height,	height,
		TAG_DONE);

		DrawImageState(&handle->RPort,handle->BevelImage,left,top,IDS_NORMAL,handle->DrawInfo);
	}
	else
		LTP_RenderBevel(&handle->RPort,handle->DrawInfo->dri_Pens,left,top,width,height,TRUE,2);
}

VOID
LTP_DrawBevelBox(LayoutHandle *handle,ObjectNode *node)
{
	LTP_DrawBevel(handle,node->Left,node->Top,node->Width,node->Height);
}


/*****************************************************************************/


VOID
LTP_PrintText(struct RastPort *rp,STRPTR text,LONG textLen,LONG x,LONG y)
{
	Move(rp,x,y + rp->TxBaseline);
	Text(rp,text,textLen);
}


/*****************************************************************************/


VOID
LTP_DrawGroove(LayoutHandle *handle,LONG left,LONG top,LONG width,LONG height,LONG from,LONG to)
{
	struct RastPort *rp = &handle->RPort;
	LONG x,y;

	LTP_SetAPen(rp,handle->ShadowPen);

	LTP_PolyDraw(rp,5,
		left + 1,top + 1,
		left + 1,top + height - 2,
		left,top + height - 1,
		left,top,
		from - 2,top);

	if(from < to)
	{
		x = to + 1;
		y = top + 1;
	}
	else
	{
		x = from - 1;
		y = top;
	}

	LTP_PolyDraw(rp,6,
		x,y,
		to + 1,top,
		left + width - 2,top,
		left + width - 3,top + 1,
		left + width - 3,top + height - 2,
		left + 3,top + height - 2);

	LTP_DrawLine(rp,
		left + width - 4,top + 2,
		left + width - 4,top + height - 3);

	LTP_SetAPen(rp,handle->ShinePen);

	if(from < to)
	{
		x = from - 1;
		y = top;
	}
	else
	{
		x = from + 2;
		y = top + 1;
	}

	LTP_PolyDraw(rp,9,
		left + width - 2,top + height - 2,
		left + width - 2,top + 1,
		left + width - 1,top,
		left + width - 1,top + height - 1,
		left + 1,top + height - 1,
		left + 2,top + height - 2,
		left + 2,top + 1,
		from - 1,top + 1,
		x,y);

	LTP_DrawLine(rp,
		to + 2,top + 1,
		left + width - 4,top + 1);

	LTP_DrawLine(rp,
		left + 3,top + 2,
		left + 3,top + height - 3);
}


/*****************************************************************************/


VOID
LTP_DrawGroupLabel(LayoutHandle *handle,ObjectNode *node)
{
	struct RastPort *rp;
	LONG left,top,height;

	left = node->Left + (node->Width - node->LabelWidth) / 2;
	rp = &handle->RPort;

	if(node->Label)
	{
		LONG glyphHeight = handle->GlyphHeight;

		top = node->Top + glyphHeight / 2;
		height = node->Height - (glyphHeight + handle->InterHeight) / 2;
	}
	else
	{
		top = node->Top;
		height = node->Height - handle->InterHeight / 2;
	}

	if(handle->UseGroove)
	{
		SetAttrs(handle->GrooveImage,
			IA_Width,	node->Width - handle->GlyphWidth,
			IA_Height,	height,
		TAG_DONE);

		DrawImageState(rp,handle->GrooveImage,node->Left + handle->GlyphWidth / 2,top,IDS_NORMAL,handle->DrawInfo);

		if(node->Label)
			LTP_EraseBox(rp,left,node->Top,node->LabelWidth,handle->GlyphHeight);
	}
	else
		LTP_DrawGroove(handle,node->Left + handle->GlyphWidth / 2,top,node->Width - handle->GlyphWidth,height,left,left + node->LabelWidth - 1);

	if(node->Label)
	{
		LTP_SetPens(rp,handle->DrawInfo->dri_Pens[HIGHLIGHTTEXTPEN],0,JAM1);
		LTP_PrintText(rp,node->Label,strlen(node->Label),left + handle->GlyphWidth,node->Top);
	}
}

VOID
LTP_DrawGroupFrame(LayoutHandle *handle,ObjectNode *node)
{
	if(handle->GrooveImage)
	{
		SetAttrs(handle->GrooveImage,
			IA_Width,	node->Width,
			IA_Height,	node->Height + 4,
		TAG_DONE);

		DrawImageState(&handle->RPort,handle->GrooveImage,node->Left,node->Top - 4,IDS_NORMAL,handle->DrawInfo);
	}
	else
	{
		struct RastPort *rp;
		LONG left,top,width,height;

		rp	= &handle->RPort;

		left	= node->Left;
		top		= node->Top - 4;
		width	= node->Width;
		height	= node->Height + 4;

		LTP_SetAPen(rp,handle->ShinePen);

		LTP_PolyDraw(rp,4,
			left,top + height - 1,
			left,top,
			left + 1,top,
			left + 1,top + height - 2);

		LTP_SetAPen(rp,handle->ShadowPen);

		LTP_PolyDraw(rp,5,
			left + width - 2,top + height - 2,
			left + width - 2,top,
			left + width - 1,top,
			left + width - 1,top + height - 1,
			left + 1,top + height - 1);
	}
}
