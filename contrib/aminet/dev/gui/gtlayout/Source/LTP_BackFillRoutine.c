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

	// Layer backfill hook support

struct LayerMsg
{
	struct Layer *		Layer;
	struct Rectangle	Bounds;
};


#ifndef __AROS__
VOID SAVE_DS ASM
LTP_BackfillRoutine(
	REG(a0) struct Hook *		Hook,
	REG(a2) struct RastPort *	RPort,
	REG(a1) struct LayerMsg *	Bounds
)
#else
AROS_UFH3(void, LTP_BackfillRoutine,
	  AROS_UFHA(struct Hook *, Hook, A0),
	  AROS_UFHA(struct RastPort *, RPort, A2),
	  AROS_UFHA(struct LayerMsg *, Bounds, A1)
)
#endif
{
	LayoutHandle *Handle = (LayoutHandle *)Hook->h_Data;

		// Make a copy of the rastport

	Handle->BackfillRastPort = *RPort;

		// Zap the layer pointer, we have to do this

	Handle->BackfillRastPort.Layer = NULL;

		// Now for the colours and drawing mode

	LTP_SetPens(&Handle->BackfillRastPort,Handle->BackgroundPen,0,JAM2);

		// Clear the area

	RectFill(&Handle->BackfillRastPort,
		Bounds->Bounds.MinX,Bounds->Bounds.MinY,
		Bounds->Bounds.MaxX,Bounds->Bounds.MaxY);
}

/*
VOID SAVE_DS ASM
_LTP_BackfillRoutine(REG(a0) struct Hook *Hook,REG(a1) struct LayerMsg *Bounds,REG(a2) struct RastPort *RPort)
{
	STATIC UWORD __chip EscherData[216] =
	{
		0xFFF0,0xFFE7,0xF703,0xFF00,0xFFFF,0xFFF9,0xF801,0xFF00,
		0xFFFF,0xFFFE,0xF000,0xFF00,0xFFFF,0xFCFF,0x6000,0x7F00,
		0xFFFF,0xFF3F,0x8020,0x7F00,0xFFFF,0xFFCF,0xC000,0x9F00,
		0xFFFF,0xFFF7,0x8001,0x0700,0xFFFF,0xFFFB,0x8001,0x0100,
		0xFFFF,0xFFFD,0x0001,0x0000,0xFFFF,0xFFFE,0x007F,0xFF00,
		0xFFFF,0xFFFF,0x00FF,0xFF00,0xFFFF,0xFFFF,0x00FF,0xFF00,
		0xFE7F,0xFFFF,0x00FF,0xFF00,0x7F9F,0xFFFF,0x807F,0xFE00,
		0x9FEF,0xFFFF,0x807F,0xFF00,0xE7F7,0xFFFF,0x803F,0x9F00,
		0xFBF8,0x07FF,0xA00F,0xE700,0xFDE0,0x007F,0xB007,0xF900,
		0xFEC0,0x009F,0xB801,0xFE00,0x7F00,0x0067,0xBC00,0xFF00,
		0xBE00,0x0079,0x9E00,0x3F00,0xD800,0x003C,0x6F00,0x1F00,
		0xE000,0x003D,0xF700,0x0F00,0xE006,0x001E,0xF700,0x0700,
		0xC018,0x001E,0xFA00,0x0700,0x8020,0x000F,0x7000,0x0300,
		0x0040,0x600F,0x0000,0x0300,0x0081,0x8000,0x0000,0x0200,
		0xF902,0x0000,0x0000,0x0100,0xFF04,0x0000,0x0000,0x0700,
		0xFF88,0x0C00,0x0000,0x0F00,0xBFD0,0x3000,0x0000,0x1F00,
		0xFFE0,0xC000,0x0000,0x6F00,0xFFE1,0x0000,0x0001,0xF700,
		0xFFE2,0x0000,0x0007,0xF700,0x1FE4,0x0000,0x0000,0x0000,
		0x0FE8,0x0000,0x0000,0x0000,0x0FF0,0x0000,0x0000,0x0000,
		0x0FE0,0x0000,0x0000,0x0000,0x1FE0,0x0000,0x0000,0x0000,
		0x3FE0,0x0000,0x3030,0x0000,0x7FA0,0x0000,0xC0C0,0xC000,
		0xFFA0,0x0001,0x0103,0x0000,0xFF20,0x0002,0x0204,0x0300,
		0xFE20,0x1FFC,0x0408,0x0F00,0xFC20,0x63FF,0x8810,0x1F00,
		0xF821,0x87FF,0xF020,0x3F00,0xF066,0x0FFF,0xF840,0x7F00,
		0xE09C,0x1FFF,0xFC40,0xFF00,0xE104,0x1FFF,0xFE81,0xFF00,
		0xE108,0x3FF9,0xFF81,0xFF00,0xF208,0x3FFE,0x7FC3,0xFF00,
		0xFA10,0x7FFF,0x9FE3,0xFF00,0xFF10,0x7F9F,0xEFF3,0xFF00
	};

	struct RastPort RastPort;

	LayoutHandle *Handle = (LayoutHandle *)Hook->h_Data;

	LONG	PatternWidth,
			PatternHeight;

	LONG	FromMinX,FromMinY,FromMaxX,FromMaxY,
			SaveMinX,SaveMaxX,SaveToMinX,
			ToMinX,ToMinY;
	LONG	Width,Height;
	LONG	Offset;
	LONG	Modulo;

	InitRastPort(&RastPort);
	RastPort.BitMap = RPort->BitMap;
	SetABPenDrMd(&RastPort,Handle->DrawInfo->dri_Pens[FILLPEN],Handle->DrawInfo->dri_Pens[BACKGROUNDPEN],JAM2);

	Modulo = ((56 + 15) >> 4) * 2;

		// Pick up the initial data

	PatternWidth	= 56;
	PatternHeight	= 54;

	ToMinX			= Bounds->Bounds.MinX;
	ToMinY			= Bounds->Bounds.MinY;

	FromMinX		= Bounds->Bounds.MinX;
	FromMinY		= Bounds->Bounds.MinY;

	FromMaxX		= Bounds->Bounds.MaxX;
	FromMaxY		= Bounds->Bounds.MaxY;

		// NOTEZ-BIEN: the following layer offset adjustments are for
		//             window layer backfill only, for screen layer
		//             backfills the Layer pointer would be invalid.

		// Adjust for window left edge

	if(Offset = Bounds->Layer->bounds.MinX % PatternWidth)
	{
		FromMinX += Offset;
		FromMaxX += Offset;
	}

		// Adjust for window top edge

	if(Offset = Bounds->Layer->bounds.MinY % PatternHeight)
	{
		FromMinY += Offset;
		FromMaxY += Offset;
	}

		// Save these for later

	SaveMinX	= FromMinX;
	SaveMaxX	= FromMaxX;
	SaveToMinX	= ToMinX;

	do
	{
		do
		{
				// Unwrap the left edge

			if(PatternWidth <= FromMinX)
			{
				LONG OldMinX = FromMinX;

				FromMinX %= PatternWidth;
				FromMaxX -= (OldMinX - FromMinX);
			}

				// Unwrap the top edge

			if(PatternHeight <= FromMinY)
			{
				LONG OldMinY = FromMinY;

				FromMinY %= PatternHeight;
				FromMaxY -= (OldMinY - FromMinY);
			}

				// Stay within horizontal bounds

			if((Width = FromMaxX) >= PatternWidth)
				Width = PatternWidth - 1;

			Width = Width - FromMinX + 1;

				// Stay within vertical bounds

			if((Height = FromMaxY) >= PatternHeight)
				Height = PatternHeight - 1;

			Height = Height - FromMinY + 1;

				// Blast in the image

			BltTemplate(&((PLANEPTR)EscherData)[FromMinY * Modulo + ((FromMinX & ~0xF) / 8)],FromMinX & 0xF,Modulo,&RastPort,ToMinX,ToMinY,Width,Height);

				// Move up to next column

			FromMinX	+= Width;
			ToMinX 		+= Width;
		}
		while(FromMaxX >= FromMinX);

			// Return to first column

		FromMinX	= SaveMinX;
		FromMaxX	= SaveMaxX;
		ToMinX		= SaveToMinX;

			// Move up to next row

		Height = PatternHeight - 1 - FromMinY + 1;

		FromMinY	+= Height;
		ToMinY		+= Height;
	}
	while(FromMaxY >= PatternHeight);
}
*/
