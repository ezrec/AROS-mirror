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

#ifdef DO_GAUGE_KIND	/* Support code */

VOID
LTP_GaugeFill(struct RastPort *rp,LONG pen,ObjectNode *node,LONG left,LONG right,LONG height)
{
	if(right)
	{
		LTP_SetAPen(rp,pen);
		LTP_FillBox(rp,node->Left + 2 + left,node->Top + 1,right - left,height);
	}
}

VOID
LTP_DrawTick(LayoutHandle *Handle,ObjectNode *Node,LONG Left,LONG Top)
{
	struct RastPort *rp = &Handle->RPort;

	Left += Node->Left;

	LTP_SetAPen(rp,Handle->ShadowPen);
	LTP_DrawLine(rp,Left,Top,Left,Top + 1);

	LTP_SetAPen(rp,Handle->ShinePen);
	LTP_DrawLine(rp,Left + 1,Top,Left + 1,Top + 1);
}

VOID
LTP_DrawGauge(LayoutHandle *handle,ObjectNode *node,LONG percent,BOOL fullRefresh)
{
	struct RastPort *rp = &handle->RPort;
	LONG height = node->Height - (1 + 1);
	LONG width = node->Width - (2 + 2);
	UWORD *Pens = handle->DrawInfo->dri_Pens;
	LONG fillPen = Pens[FILLPEN];
	LONG backPen = Pens[BACKGROUNDPEN];
	BOOL continuous;

	if(fullRefresh)
	{
		LTP_EraseBox(rp,node->Left,node->Top,node->Width,node->Height);

		if(node->Special.Gauge.NoTicks)
			LTP_DrawBevelBox(handle,node);
		else
		{
			LONG top = node->Top + node->Height - (handle->GlyphHeight + 2);

			LTP_DrawBevel(handle,node->Left,node->Top,node->Width,node->Height - (handle->InterHeight + handle->GlyphHeight + 2));

			LTP_SetPens(rp,handle->TextPen,0,JAM1);

			LTP_PrintText(rp,"0%",2,node->Left,top);
			LTP_PrintText(rp,"100%",4,node->Left + node->Width - TextLength(rp,"100%",4),top);

			if(node->Width >= TextLength(rp,"0%50%100%",9) + 4 * handle->GlyphWidth)
			{
				LONG i,left,total = node->Width - 2;

				LTP_PrintText(rp,"50%",3,node->Left + (node->Width - TextLength(rp,"50%",3)) / 2,top);

				top -= handle->InterHeight;

				for (i = 0 ; i < 5 ; i++)
				{
					left = (total * i) / 4;

					LTP_DrawTick(handle,node,left,top);
				}
			}
			else
			{
				LONG i,left,total = node->Width - 2;

				top -= handle->InterHeight;

				for (i = 0; i < 3; i++)
				{
					left = (total * i) / 2;

					LTP_DrawTick(handle,node,left,top);
				}
			}
		}
	}

	if(node->Special.Gauge.Discrete)
	{
		if(width < 60)
			continuous = TRUE;
		else
			continuous = FALSE;
	}
	else
		continuous = TRUE;

	if(!node->Special.Gauge.NoTicks)
		height -= handle->InterHeight + handle->GlyphHeight + 2;

	if(node->Special.Gauge.InfoLength && node->Special.Gauge.InfoText[0])
	{
		struct TextExtent Extent;
		LONG len;
		LONG textPen;
		LONG mode;
		LONG right;

		if(continuous)
			LTP_GaugeFill(rp,fillPen,node,0,right = (width * percent) / 100,height);
		else
		{
			LONG count;
			LONG left;

			left = right = 0;

			if(count = (10 * percent) / 100)
			{
				LONG chunk = width / 10,i;

				for(i = 0 ; i < count ; i++)
				{
					LTP_SetAPen(rp,fillPen);
					LTP_FillBox(rp,node->Left + 2 + left,node->Top + 1,(chunk - 1),height);

					left += chunk;
					right += chunk;

					LTP_SetAPen(rp,backPen);
					LTP_FillBox(rp,node->Left + 2 + right - 1,node->Top + 1,1,height);
				}
			}
		}

		if(right < width)
			LTP_GaugeFill(rp,backPen,node,right,width,height);

		if(handle->TextPen == fillPen)
			textPen = Pens[FILLTEXTPEN];
		else
			textPen = handle->TextPen;

		if(textPen == fillPen || textPen == backPen)
			mode = JAM1 | COMPLEMENT;
		else
			mode = JAM1;

		LTP_SetPens(rp,textPen,0,mode);

		len = TextFit(rp,node->Special.Gauge.InfoText,strlen(node->Special.Gauge.InfoText),&Extent,NULL,1,width,32767);

		LTP_PrintText(rp,node->Special.Gauge.InfoText,len,node->Left + 2 + (width - Extent.te_Width) / 2,node->Top + 1 + (height - handle->GlyphHeight) / 2);

		if(mode != JAM1)
			SetDrMd(rp,JAM1);
	}
	else
	{
		LONG left = 0,right = 0,pen = 0;	/* Initialize these for the sake of the compiler. */
		BOOL drawIt = FALSE;

		if(continuous)
		{
			if(node->Current < percent)
			{
				left	= (width * node->Current) / 100;
				right	= (width * percent) / 100;
				pen		= fillPen;
				drawIt	= TRUE;
			}
			else
			{
				if(node->Current > percent)
				{
					left	= (width * percent) / 100;
					right	= (width * node->Current) / 100;
					pen		= backPen;
					drawIt	= TRUE;
				}
			}
		}
		else
		{
			LONG count;

			count = (10 * percent) / 100;

			if(fullRefresh || count != node->Special.Gauge.LastPercentage)
			{
				node->Special.Gauge.LastPercentage = count;

				right = 0;

				if(count)
				{
					LONG chunk = width / 10,i;

					left = 0;

					for(i = 0 ; i < count ; i++)
					{
						LTP_SetAPen(rp,fillPen);
						LTP_FillBox(rp,node->Left + 2 + left,node->Top + 1,(chunk - 1),height);

						left += chunk;
						right += chunk;

						LTP_SetAPen(rp,backPen);
						LTP_FillBox(rp,node->Left + 2 + right - 1,node->Top + 1,1,height);
					}
				}

				if(right < width)
				{
					left	= right;
					right	= width;
					pen		= backPen;
					drawIt	= TRUE;
				}
			}
		}

		if(drawIt)
			LTP_GaugeFill(rp,pen,node,left,right,height);
	}

	if(node->Disabled)
	{
		UWORD Height;

		if(node->Special.Gauge.NoTicks)
			Height = 0;
		else
			Height = handle->InterHeight + handle->GlyphHeight + 2;

		LTP_GhostBox(rp,node->Left + 2,node->Top + 1,node->Width - 4,node->Height - 2 - Height,Pens[SHADOWPEN]);
	}

	node->Current = percent;

	LTP_PutStorage(node);
}

#endif	/* DO_GAUGE_KIND */

