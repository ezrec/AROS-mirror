/*
**	Raster.c
**
**	Screen character (raster) buffering routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* DeleteRaster():
	 *
	 *	Free the contents of the character raster.
	 */

VOID
DeleteRaster()
{
	FreeVecPooled(Raster);
	Raster = NULL;

	FreeVecPooled(RasterAttr);
	RasterAttr = NULL;
}

	/* CreateRaster():
	 *
	 *	Create the character raster.
	 */

BOOL
CreateRaster()
{
		/* Width of the screen * 2 (in characters),
		 * extra for double width. The window size
		 * may change, the screen size hopefully
		 * doesn't.
		 */

	RasterWidth		= (Window->WScreen->Width / TextFontWidth) * 2;

		/* Height of the character raster. */

	RasterHeight	= Window->WScreen->Height / TextFontHeight;

		/* Allocate the raster. */

	if(Raster = (STRPTR)AllocVecPooled(RasterWidth * RasterHeight,MEMF_ANY | MEMF_CLEAR))
	{
			/* Allocate the raster attributes. */

		if(RasterAttr = (STRPTR)AllocVecPooled(RasterHeight,MEMF_ANY | MEMF_CLEAR))
			return(TRUE);
	}

	DeleteRaster();

	return(FALSE);
}

	/* RasterEraseScreen(BYTE Mode):
	 *
	 *	Erase parts of the screen.
	 */

VOID
RasterEraseScreen(LONG Mode)
{
	LONG First,Last;

	ObtainSemaphore(&RasterSemaphore);

	switch(Mode)
	{
		case 1:

			First	= 0;
			Last	= CursorY * RasterWidth + CursorX + 1;

			if(CursorY == LastLine)
				SaveRaster(0,CursorY);

			memset(RasterAttr,SCALE_NORMAL,CursorY + 1);

			break;

		case 2:

			First	= 0;
			Last	= RasterHeight * RasterWidth - 1;

			SaveRaster(0,RasterHeight - 1);

			memset(RasterAttr,SCALE_NORMAL,RasterHeight);

			break;

		default:

			First	= CursorY * RasterWidth + CursorX;
			Last	= RasterHeight * RasterWidth - 1;

			if(CursorY == 0)
				SaveRaster(CursorY,RasterHeight - 1);

			memset(&RasterAttr[CursorY],SCALE_NORMAL,RasterHeight - CursorY);

			break;
	}

	RethinkRasterLimit();

	if(Last > First)
		memset(&Raster[First],' ',Last - First);

	ConFontScaleUpdate();

	ReleaseSemaphore(&RasterSemaphore);
}

	/* RasterEraseLine(BYTE Mode):
	 *
	 *	Erase parts of the current cursor line.
	 */

VOID
RasterEraseLine(LONG Mode)
{
	LONG First,Last;

	ObtainSemaphore(&RasterSemaphore);

/*	SaveRaster(CursorY,CursorY); */

	switch(Mode)
	{
			/* From beginning to current cursor position. */

		case 1:

			First	= CursorY * RasterWidth;
			Last	= First + CursorX + 1;

			break;

			/* Entire line. */

		case 2:

			First	= CursorY * RasterWidth;
			Last	= First + RasterWidth - 1;

			break;

			/* From current cursor position towards end. */

		default:

			First	= CursorY * RasterWidth + CursorX;
			Last	= (CursorY + 1) * RasterWidth - 1;

			break;
	}

	if(Last > First)
		memset(&Raster[First],' ',Last - First);

	ReleaseSemaphore(&RasterSemaphore);
}

	/* RasterEraseCharacters(LONG Chars):
	 *
	 *	Erase a number of characters in the current cursor
	 *	line.
	 */

VOID
RasterEraseCharacters(LONG Chars)
{
	if(CursorX < RasterWidth - 1)
	{
		LONG First,Diff;
		UBYTE *To,*From;

/*		SaveRaster(CursorY,CursorY); */

		ObtainSemaphore(&RasterSemaphore);

		First	= CursorY * RasterWidth + CursorX;
		To		= &Raster[First];

		if(CursorX + Chars >= RasterWidth)
		{
			Diff = RasterWidth - 1 - CursorX;

			while(Diff-- > 0)
				*To++ = ' ';
		}
		else
		{
			From = &Raster[First + Chars];
			Diff = RasterWidth - (CursorX + 1 + Chars);

			while(Diff--)
			{
				*To++ = *From;

				*From++ = ' ';
			}
		}

		ReleaseSemaphore(&RasterSemaphore);
	}
}

	/* RasterClearLine(LONG Lines):
	 *
	 *	Clear and remove a number of lines.
	 */

VOID
RasterClearLine(LONG Lines,LONG Top)
{
	if(Lines)
	{
		LONG RegionBottom;

		ObtainSemaphore(&RasterSemaphore);

		if(RegionSet)
			RegionBottom = Bottom;
		else
			RegionBottom = LastLine + 1;

		if(Top + Lines >= RegionBottom + 1)
		{
			SaveRaster(Top,RegionBottom);

			Lines = RegionBottom - Top + 1;

			memset(&Raster[Top * RasterWidth],' ',RasterWidth * Lines);
			memset(&RasterAttr[Top],SCALE_NORMAL,Lines);
		}
		else
		{
			UBYTE *From,*To;
			LONG Max;

			SaveRaster(Top,Top + Lines - 1);

			Max	= (RegionBottom - (Top + Lines)) * RasterWidth;

			From	= &Raster[(Top + Lines) * RasterWidth];
			To		= &Raster[ Top          * RasterWidth];

			while(Max--)
			{
				*To++ = *From;

				*From++ = ' ';
			}

			memset(&RasterAttr[RegionBottom - Lines],SCALE_NORMAL,Lines);

		}

		RethinkRasterLimit();

		ConFontScaleUpdate();

		ReleaseSemaphore(&RasterSemaphore);
	}

}

	/* RasterInsertLine(LONG Lines):
	 *
	 *	Insert a number of lines at the current cursor line.
	 */

VOID
RasterInsertLine(LONG Lines,LONG Top)
{
	if(Lines)
	{
		LONG RegionBottom;

		if(RegionSet)
			RegionBottom = Bottom;
		else
			RegionBottom = LastLine + 1;

		if(Top + Lines >= RegionBottom + 1)
		{
			SaveRaster(Top,RegionBottom);

			Lines = RegionBottom - Top + 1;

			memset(&Raster[Top * RasterWidth],' ',RasterWidth * Lines);
			memset(&RasterAttr[Top],SCALE_NORMAL,Lines);
		}
		else
		{
			UBYTE *FromPtr,*ToPtr;
			LONG From,To,Max;

			SaveRaster(RegionBottom - Lines,RegionBottom);

			ObtainSemaphore(&RasterSemaphore);

			Max	= (RegionBottom - Lines - Top) * RasterWidth;

			From	= (RegionBottom - Lines) * RasterWidth - 1;
			To		=  RegionBottom          * RasterWidth - 1;

			FromPtr	= &Raster[From];
			ToPtr	= &Raster[To];

			while(Max--)
				*ToPtr-- = *FromPtr--;

			memset(&Raster[Top * RasterWidth],' ',Lines * RasterWidth);

			ReleaseSemaphore(&RasterSemaphore);
		}
	}

}

	/* RasterScrollRegion(LONG Direction,LONG RasterTop,LONG RasterBottom,LONG RasterLines):
	 *
	 *	Scroll the contents of the character raster up/down.
	 */

VOID
RasterScrollRegion(LONG Direction,LONG RasterTop,LONG RasterBottom,LONG RasterLines)
{
	LONG Dir = ABS(Direction);

	ObtainSemaphore(&RasterSemaphore);

	SaveRaster(RasterTop,RasterTop + RasterLines - 1);

	if(Dir >= RasterLines)
	{
			/* All that is needed is to delete the lines. */

		memset(&Raster[RasterTop * RasterWidth],' ',RasterLines * RasterWidth);
	}
	else
	{
		LONG First,Last,Max,i;
		UBYTE *From,*To;

		Max = (RasterLines - Dir) * RasterWidth;

		if(Direction < 0)
		{
			First	= (RasterTop + RasterLines - Dir) * RasterWidth - 1;
			Last	= (RasterTop + RasterLines	) * RasterWidth - 1;

			From	= &Raster[First];
			To		= &Raster[Last];

			while(Max--)
				*To-- = *From--;

			for(i = RasterBottom ; i >= (RasterTop + Dir) ; i--)
				RasterAttr[i] = RasterAttr[i - Dir];

			memset(&Raster[RasterTop * RasterWidth],' ',RasterWidth * Dir);

			memset(&RasterAttr[RasterTop],SCALE_NORMAL,Dir);
		}
		else
		{
			First	= RasterTop * RasterWidth + RasterWidth * Dir;
			Last	= RasterTop * RasterWidth;

			From	= &Raster[First];
			To		= &Raster[Last];

			while(Max--)
				*To++ = *From++;

			memset(&Raster[(RasterBottom - Dir) * RasterWidth],' ',RasterWidth * Dir);

			for(i = RasterTop ; i <= (RasterBottom - Dir) ; i++)
				RasterAttr[i] = RasterAttr[i + Dir];

			memset(&RasterAttr[RasterBottom - Dir],SCALE_NORMAL,Dir);
		}
	}

	RethinkRasterLimit();

	ConFontScaleUpdate();

	ReleaseSemaphore(&RasterSemaphore);
}

	/* RasterShiftChar(LONG Size):
	 *
	 *	Shift the characters following the current cursor
	 *	position Size characters to the right.
	 */

VOID
RasterShiftChar(LONG Size)
{
	UBYTE *From,*To;
	LONG First,i;

	ObtainSemaphore(&RasterSemaphore);

	if(CursorX + Size >= RasterWidth - 1)
	{
		i	= RasterWidth - 1 - CursorX;
		To	= &Raster[CursorY * RasterWidth + CursorX];

		while(i-- > 0)
			*To++ = ' ';
	}
	else
	{
		First	= (CursorY + 1) * RasterWidth - 1;
		To		= &Raster[First];

		From	= &Raster[First - Size];
		i		= RasterWidth - Size;

		while(i-- > CursorX)
			*To-- = *From--;

		To		= &Raster[CursorY * RasterWidth + CursorX];

		while(Size--)
			*To++ = ' ';
	}

	ReleaseSemaphore(&RasterSemaphore);
}

	/* RasterPutString(STRPTR String,LONG Length):
	 *
	 *	Put a string into the character raster.
	 */

VOID
RasterPutString(STRPTR String,LONG Length)
{
	ObtainSemaphore(&RasterSemaphore);

	if(Length == 1)
	{
		if(CursorX + 1 < RasterWidth)
			Raster[CursorY * RasterWidth + CursorX] = String[0];
	}
	else
	{
		if(CursorX + Length >= RasterWidth)
			Length = RasterWidth - 1 - CursorX;

		if(Length > 0)
			CopyMem(String,&Raster[CursorY * RasterWidth + CursorX],Length);
	}

	ReleaseSemaphore(&RasterSemaphore);
}

VOID
SaveRasterDummy(LONG UnusedFirst,LONG UnusedLast)
{
}

VOID
SaveRasterReal(LONG First,LONG Last)
{
	STRPTR Line,This;
	LONG Size,i;

	ObtainSemaphore(&RasterSemaphore);

	if(First < 0)
		First = 0;

	if(Last > RasterHeight - 1)
		Last = RasterHeight - 1;

	for(i = First, This = &Raster[(LONG)First * RasterWidth] ; i <= Last ; i++, This += RasterWidth)
	{
		Line = This;
		Size = RasterWidth - 1;

		while(Size > 0 && Line[Size - 1] == ' ')
			Size--;

		AddLine(Line,Size);
	}

	ReleaseSemaphore(&RasterSemaphore);
}
