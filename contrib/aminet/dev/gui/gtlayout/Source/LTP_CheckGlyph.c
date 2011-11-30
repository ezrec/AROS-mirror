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

#include <exec/memory.h>

/*****************************************************************************/

#include "Assert.h"

#define VECTOR_BYTES(n)	(((n) * 5 + 1) & ~1)

VOID
LTP_DrawCheckGlyph(struct RastPort *RPort,LONG Left,LONG Top,struct CheckGlyph *Glyph,BOOL Selected)
{
	struct BitMap *BitMap;

	if(Selected)
		BitMap = Glyph->Selected;
	else
		BitMap = Glyph->Plain;

	BltBitMapRastPort(BitMap,0,0,RPort,Left,Top,Glyph->Width,Glyph->Height,0xC0);
}

VOID
LTP_DeleteCheckGlyph(struct CheckGlyph *Glyph)
{
	if(Glyph)
	{
		WaitBlit();

		LTP_DeleteBitMap(Glyph->Plain,FALSE);
		LTP_DeleteBitMap(Glyph->Selected,FALSE);

		FreeVec(Glyph);
	}
}

struct CheckGlyph *
LTP_CreateCheckGlyph(LONG Width,LONG Height,struct BitMap *Friend,LONG BackPen,LONG GlyphPen)
{
	struct CheckGlyph *Glyph;
	struct RastPort *RPort;

	Glyph = NULL;

	if((RPort = (struct RastPort *)AllocVec(sizeof(struct RastPort) + sizeof(struct TmpRas) + sizeof(struct AreaInfo) + VECTOR_BYTES(11),MEMF_ANY | MEMF_CLEAR)))
	{
		PLANEPTR Plane;

		if((Plane = AllocRaster(Width,Height)))
		{
			struct BitMap *BitMaps[2] = { NULL, NULL };	/* For the sake of the compiler, make sure this is initialized */
			LONG i,Depth,Max;

			Depth = 8;	/* For the sake of the compiler, make sure this is initialized */

			if(BackPen > GlyphPen)
				Max = BackPen;
			else
				Max = GlyphPen;

			for(i = 1 ; i <= 8 ; i++)
			{
				if((1 << i) > Max)
				{
					Depth = i;
					break;
				}
			}

			for(i = 0 ; i < 2 ; i++)
				BitMaps[i] = LTP_CreateBitMap(Width,Height,Depth,Friend,FALSE);

			if(BitMaps[0] && BitMaps[1])
			{
				if((Glyph = (struct CheckGlyph *)AllocVec(sizeof(struct CheckGlyph),MEMF_ANY | MEMF_CLEAR)))
				{
					struct TmpRas	*TmpRas;
					struct AreaInfo	*AreaInfo;
					LONG			 j;
					LONG			 RadiusX,
									 RadiusY;

					TmpRas		= (struct TmpRas *)(RPort + 1);
					AreaInfo	= (struct AreaInfo *)(TmpRas + 1);

					Glyph->Width	= Width;
					Glyph->Height	= Height;
					Glyph->Plain	= BitMaps[0];
					Glyph->Selected	= BitMaps[1];

					InitTmpRas(TmpRas,Plane,RASSIZE(Width,Height));
					InitArea(AreaInfo,(UBYTE *)(AreaInfo + 1),11);

					InitRastPort(RPort);

					RPort->TmpRas	= TmpRas;
					RPort->AreaInfo	= AreaInfo;

					RadiusX = Width / 4;
					RadiusY = Height / 4;

					for(j = 0 ; j < 2 ; j++)
					{
						RPort->BitMap = BitMaps[j];

						if(j)
						{
							SetRast(RPort,BackPen);
							SetAPen(RPort,GlyphPen);
						}
						else
						{
							SetRast(RPort,GlyphPen);
							SetAPen(RPort,BackPen);
						}

						AreaEllipse(RPort,Width / 2,Height / 2,RadiusX,RadiusY);
						AreaEnd(RPort);
					}
				}
			}

			WaitBlit();

			if(!Glyph)
			{
				LTP_DeleteBitMap(BitMaps[0],FALSE);
				LTP_DeleteBitMap(BitMaps[1],FALSE);
			}

			FreeRaster(Plane,Width,Height);
		}

		FreeVec(RPort);
	}

	return(Glyph);
}
