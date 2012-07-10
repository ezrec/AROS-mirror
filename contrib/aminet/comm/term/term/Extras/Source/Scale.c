/*
**	Scale.c
**
**	Single scaled character output routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Some static data required by the bitmap scaling routine. */

STATIC struct RastPort		*ScaleRPort;
STATIC struct BitMap		*ScaleSrcBitMap,
							*ScaleDstBitMap;
STATIC struct BitScaleArgs	*ScaleArgs;

STATIC WORD					 ScaleCache		= -1,
							 ScaleType		= 42,
							 ScaleConfig	= 42;

	/* DeleteScale():
	 *
	 *	Frees all the data associated with font scaling.
	 */

VOID
DeleteScale()
{
	FreeVecPooled(ScaleArgs);
	ScaleArgs = NULL;

	DeleteBitMap(ScaleDstBitMap);
	ScaleDstBitMap = NULL;

	DeleteBitMap(ScaleSrcBitMap);
	ScaleSrcBitMap = NULL;

	FreeVecPooled(ScaleRPort);
	ScaleRPort = NULL;
}

	/* CreateScale():
	 *
	 *	Sets up the data required for real-time font scaling
	 *	(bitmaps, rastports, etc.).
	 */

BOOL
CreateScale(struct Window *Parent)
{
		/* Create a RastPort to render into. */

	if(ScaleRPort = (struct RastPort *)AllocVecPooled(sizeof(struct RastPort),MEMF_ANY | MEMF_CLEAR))
	{
		LONG MaxWidth,PlaneWidth,PlaneHeight;
		LONG Depth;

			/* Initialize it. */

		InitRastPort(ScaleRPort);

		if(GFX)
			MaxWidth = GFX->tf_XSize;
		else
			MaxWidth = 0;

		if(TextFontWidth > MaxWidth)
			MaxWidth = TextFontWidth;

			/* Remember dimensions. */

		PlaneWidth	= MaxWidth;
		PlaneHeight	= TextFontHeight;

			/* Check the depth of the original screen. */

		Depth = GetBitMapDepth(Parent->RPort->BitMap);

			/* Create the bitmap to render into. */

		if(ScaleSrcBitMap = (struct BitMap *)CreateBitMap(PlaneWidth,PlaneHeight,Depth,BMF_CLEAR,Parent->RPort->BitMap))
		{
				/* Create the bitmap to place the scaled font data into. */

			if(ScaleDstBitMap = (struct BitMap *)CreateBitMap(PlaneWidth * 2,PlaneHeight * 2,Depth,BMF_CLEAR,Parent->RPort->BitMap))
			{
					/* Put the source bitmap into the source RastPort. */

				ScaleRPort->BitMap = ScaleSrcBitMap;

					/* Install the fonts. */

				SetFont(ScaleRPort,CurrentFont);

					/* Set the default rendering pens. */

				SetPens(ScaleRPort,1,0,JAM2);

					/* Allocate space for the bitmap scaling arguments. */

				if(ScaleArgs = (struct BitScaleArgs *)AllocVecPooled(sizeof(struct BitScaleArgs),MEMF_ANY | MEMF_CLEAR))
				{
						/* Initialize the structure. */

					ScaleArgs->bsa_SrcWidth		= TextFontWidth;
					ScaleArgs->bsa_SrcHeight	= TextFontHeight;

					ScaleArgs->bsa_YSrcFactor	= 1;

					ScaleArgs->bsa_SrcBitMap	= ScaleSrcBitMap;
					ScaleArgs->bsa_DestBitMap	= ScaleDstBitMap;

					return(TRUE);
				}
			}
		}
	}

	return(FALSE);
}

	/* PrintScaled(UBYTE Char,UBYTE Scale):
	 *
	 *	This is the big one: since VT100 supports a number of
	 *	font sizes (double height, double width, 132 columns),
	 *	the appropriate characters are scaled in real-time before
	 *	they are displayed.
	 */

VOID
PrintScaled(STRPTR Buffer,LONG Size,LONG Scale)
{
	LONG SrcY = 0,DestX = 0,DestY = 0,SizeX = 0;

		/* Determine the scale of the destination character. */

	if(CurrentCharWidth == SCALE_HALF)
	{
			/* Determine scale to be used. */

		switch(Scale)
		{
				/* Half width. */

			case SCALE_ATTR_NORMAL:

				ScaleArgs->bsa_XDestFactor	= 1;
				ScaleArgs->bsa_YDestFactor	= 1;
				ScaleArgs->bsa_XSrcFactor	= 2;

				SrcY	= 0;
				DestX	= MUL_X(CursorX) / 2;
				SizeX	= TextFontWidth / 2;

				ScaleCache = -1;

				break;

				/* Half width, double height (top bits). */

			case SCALE_ATTR_TOP2X:

				ScaleArgs->bsa_XDestFactor	= 1;
				ScaleArgs->bsa_YDestFactor	= 2;
				ScaleArgs->bsa_XSrcFactor	= 1;

				SrcY	= 0;
				DestX	= MUL_X(CursorX);
				SizeX	= TextFontWidth;

				ScaleCache = -1;

				break;

				/* Half width, double height (bottom bits). */

			case SCALE_ATTR_BOT2X:

				ScaleArgs->bsa_XDestFactor	= 1;
				ScaleArgs->bsa_YDestFactor	= 2;
				ScaleArgs->bsa_XSrcFactor	= 1;

				SrcY	= TextFontHeight;
				DestX	= MUL_X(CursorX);
				SizeX	= TextFontWidth;

				ScaleCache = -1;

				break;
		}
	}
	else
	{
			/* Determine scale to be used. */

		switch(Scale)
		{
				/* Double height (top bits). */

			case SCALE_ATTR_TOP2X:

				ScaleArgs->bsa_XDestFactor	= 2;
				ScaleArgs->bsa_YDestFactor	= 2;
				ScaleArgs->bsa_XSrcFactor	= 1;

				SrcY	= 0;
				DestX	= MUL_X(CursorX) * 2;
				SizeX	= TextFontWidth * 2;

				ScaleCache = -1;

				break;

				/* Double height (bottom bits). */

			case SCALE_ATTR_BOT2X:

				ScaleArgs->bsa_XDestFactor	= 2;
				ScaleArgs->bsa_YDestFactor	= 2;
				ScaleArgs->bsa_XSrcFactor	= 1;

				SrcY	= TextFontHeight;
				DestX	= MUL_X(CursorX) * 2;
				SizeX	= TextFontWidth * 2;

				ScaleCache = -1;

				break;

				/* Double width. */

			case SCALE_ATTR_2X:

				ScaleArgs->bsa_XDestFactor	= 2;
				ScaleArgs->bsa_YDestFactor	= 1;
				ScaleArgs->bsa_XSrcFactor	= 1;

				SrcY	= 0;
				DestX	= MUL_X(CursorX) * 2;
				SizeX	= TextFontWidth * 2;

				ScaleCache = -1;

				break;
		}
	}

	ScaleType	= Scale;
	ScaleConfig	= CurrentCharWidth;

		/* Look for the font type to scale. */

	if(ScaleRPort->Font != CurrentFont)
	{
		SetFont(ScaleRPort,CurrentFont);

		ScaleArgs->bsa_SrcWidth = TextFontWidth;

		ScaleCache = -1;
	}

		/* Set the appropriate colours. */

	if(ReadAPen(ScaleRPort) != ReadAPen(RPort))
	{
		SetAPen(ScaleRPort,ReadAPen(RPort));

		ScaleCache = -1;
	}

	if(ReadBPen(ScaleRPort) != ReadBPen(RPort))
	{
		SetBPen(ScaleRPort,ReadBPen(RPort));

		ScaleCache = -1;
	}

		/* Calculate topmost line to write to. */

	DestY = MUL_Y(CursorY);

	if(CurrentFont == GFX)
	{
		LONG Mode = 1;

			/* Run down the buffer... */

		while(Size--)
		{
			if(GfxTable[*Buffer] == Mode)
			{
				if(*Buffer != ScaleCache)
				{
					ScaleCache = *Buffer;

						/* Print the character to be scaled into the
						 * invisible drawing area.
						 */

					PlaceText(ScaleRPort,0,0,Buffer++,1);

						/* Scale the font. */

					BitMapScale(ScaleArgs);
				}
				else
					Buffer++;

					/* Render the character. */

				BltBitMapRastPort(ScaleDstBitMap,0,SrcY,RPort,WindowLeft + DestX,WindowTop + DestY,SizeX,TextFontHeight,MINTERM_COPY);
			}
			else
			{
				ScaleCache = *Buffer;

				if(Mode)
					SetFont(ScaleRPort,TextFont);
				else
					SetFont(ScaleRPort,GFX);

				Mode ^= 1;

					/* Print the character to be scaled into the
					 * invisible drawing area.
					 */

				PlaceText(ScaleRPort,0,0,Buffer++,1);

					/* Scale the font. */

				BitMapScale(ScaleArgs);

					/* Render the character. */

				BltBitMapRastPort(ScaleDstBitMap,0,SrcY,RPort,WindowLeft + DestX,WindowTop + DestY,SizeX,TextFontHeight,MINTERM_COPY);
			}

			DestX += SizeX;
		}

		if(!Mode)
			SetFont(ScaleRPort,GFX);
	}
	else
	{
			/* Run down the buffer... */

		while(Size--)
		{
			if(*Buffer != ScaleCache)
			{
				ScaleCache = *Buffer;

					/* Print the character to be scaled into the
					 * invisible drawing area.
					 */

				PlaceText(ScaleRPort,0,0,Buffer++,1);

					/* Scale the font. */

				BitMapScale(ScaleArgs);
			}
			else
				Buffer++;

				/* Render the character. */

			BltBitMapRastPort(ScaleDstBitMap,0,SrcY,RPort,WindowLeft + DestX,WindowTop + DestY,SizeX,TextFontHeight,MINTERM_COPY);

			DestX += SizeX;
		}
	}
}
