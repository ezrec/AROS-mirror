/*
**	Scroll.c
**
**	Support routines for optimized screen scrolling.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* ScrollFrameUpdate(VOID):
	 *
	 *	Optimize the area to be scrolled by moving the top and
	 *	bottom lines as needed.
	 */

STATIC VOID
ScrollFrameUpdate(VOID)
{
	while(ScrollLineFirst < RasterHeight && !ScrollLines[ScrollLineFirst].Width)
		ScrollLineFirst++;

	while(ScrollLineLast > 0 && !ScrollLines[ScrollLineLast].Width)
		ScrollLineLast--;
}

	/* ScrollLineRectFill():
	 *
	 *	Fill a rectangular portion of the window raster with the help
	 *	of the scrolling information.
	 */

VOID
ScrollLineRectFill(struct RastPort *RPort,LONG MinX,LONG MinY,LONG MaxX,LONG MaxY)
{
	if(MinX < MaxX && MinY < MaxY)
	{
		LONG Start,Stop;
		LONG BackPen;

		if(FgPen == BgPen || (Attributes & ATTR_INVERSE))
			BackPen	= FgPen;
		else
			BackPen = BgPen;

		Start	= MinY / TextFontHeight;
		Stop	= MaxY / TextFontHeight;

		if(BackPen)
		{
			if(UseMasking)
			{
				LONG Mask = BackPen,i;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
						Mask |= ScrollLines[i].ColourMask;

					ScrollLines[i].Left			= 0;
					ScrollLines[i].Right		= LastColumn + 1;
					ScrollLines[i].Width		= TextFontWidth;
					ScrollLines[i].ColourMask	= BackPen;
				}

				SetMask(RPort,Mask);
			}
			else
			{
				LONG i;

				for(i = Start ; i <= Stop ; i++)
				{
					ScrollLines[i].Left			= 0;
					ScrollLines[i].Right		= LastColumn + 1;
					ScrollLines[i].Width		= TextFontWidth;
					ScrollLines[i].ColourMask	= BackPen;
				}
			}

			if(Start < ScrollLineFirst)
				ScrollLineFirst = Start;

			if(Stop > ScrollLineLast)
				ScrollLineLast = Stop;

			ScrollFrameUpdate();
		}
		else
		{
			LONG ScrollLineLeft,ScrollLineRight,ScrollLineWidth;

			ScrollLineLeft	= 32767;
			ScrollLineRight	= 0;
			ScrollLineWidth	= 0;

			if(UseMasking)
			{
				LONG Mask = 0,i;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
					{
						if(ScrollLines[i].Width > ScrollLineWidth)
							ScrollLineWidth = ScrollLines[i].Width;

						if(ScrollLines[i].Left < ScrollLineLeft)
							ScrollLineLeft = ScrollLines[i].Left;

						if(ScrollLines[i].Right > ScrollLineRight)
							ScrollLineRight = ScrollLines[i].Right;

						Mask |= ScrollLines[i].ColourMask;
					}

					ScrollLines[i].Left			= 32767;
					ScrollLines[i].Right		= 0;
					ScrollLines[i].Width		= 0;
					ScrollLines[i].ColourMask	= 0;
				}

				if(Mask)
					SetMask(RPort,Mask);
				else
				{
					ScrollFrameUpdate();

					return;
				}
			}
			else
			{
				LONG i;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
					{
						if(ScrollLines[i].Width > ScrollLineWidth)
							ScrollLineWidth = ScrollLines[i].Width;

						if(ScrollLines[i].Left < ScrollLineLeft)
							ScrollLineLeft = ScrollLines[i].Left;

						if(ScrollLines[i].Right > ScrollLineRight)
							ScrollLineRight = ScrollLines[i].Right;
					}

					ScrollLines[i].Left			= 32767;
					ScrollLines[i].Right		= 0;
					ScrollLines[i].Width		= 0;
					ScrollLines[i].ColourMask	= 0;
				}
			}

			if(ScrollLineWidth)
			{
				LONG Temp;

				if((Temp = ScrollLineLeft * ScrollLineWidth) > MinX)
					MinX = Temp;

				if((Temp = ScrollLineRight * ScrollLineWidth) < MaxX)
					MaxX = Temp;

				if(MaxX == ScrollLineRight)
					MaxX += FontRightExtend;	/* Add margin for italics or boldface. */

				if((Temp = MUL_Y(ScrollLineFirst)) > MinY)
					MinY = Temp;

				if((Temp = MUL_Y(ScrollLineLast + 1) - 1) < MaxY)
					MaxY = Temp;
			}

			if(Start < ScrollLineFirst)
				ScrollLineFirst = Start;

			if(Stop > ScrollLineLast)
				ScrollLineLast = Stop;

			ScrollFrameUpdate();

			if(!ScrollLineWidth)
				return;
		}

			/* And clear the raster. */

		if(MinX < MaxX && MinY < MaxY)
		{
			MinX += WindowLeft;
			MinY += WindowTop;
			MaxX += WindowLeft;
			MaxY += WindowTop;

			if(FgPen == BgPen || (Attributes & ATTR_INVERSE))
				RectFill(RPort,MinX,MinY,MaxX,MaxY);
			else
			{
				SetAPen(RPort,BgPen);
				RectFill(RPort,MinX,MinY,MaxX,MaxY);
				SetAPen(RPort,FgPen);
			}
		}
	}
}

VOID
ScrollLineRectFillNoTabChange(struct RastPort *RPort,LONG MinX,LONG MinY,LONG MaxX,LONG MaxY)
{
	if(MinX < MaxX && MinY < MaxY)
	{
		LONG Start,Stop;
		LONG BackPen;

		if(FgPen == BgPen || (Attributes & ATTR_INVERSE))
			BackPen	= FgPen;
		else
			BackPen = BgPen;

		Start	= MinY / TextFontHeight;
		Stop	= MaxY / TextFontHeight;

		if(BackPen)
		{
			if(UseMasking)
			{
				LONG Mask,i;

				Mask = BackPen;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
						Mask |= ScrollLines[i].ColourMask;
				}

				SetMask(RPort,Mask);
			}

			if(Start < ScrollLineFirst)
				ScrollLineFirst = Start;

			if(Stop > ScrollLineLast)
				ScrollLineLast = Stop;

			ScrollFrameUpdate();
		}
		else
		{
			LONG ScrollLineLeft,ScrollLineRight,ScrollLineWidth;

			ScrollLineLeft	= 32767;
			ScrollLineRight	= 0;
			ScrollLineWidth	= 0;

			if(UseMasking)
			{
				LONG Mask,i;

				Mask = 0;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
					{
						if(ScrollLines[i].Width > ScrollLineWidth)
							ScrollLineWidth = ScrollLines[i].Width;

						if(ScrollLines[i].Left < ScrollLineLeft)
							ScrollLineLeft = ScrollLines[i].Left;

						if(ScrollLines[i].Right > ScrollLineRight)
							ScrollLineRight = ScrollLines[i].Right;

						Mask |= ScrollLines[i].ColourMask;
					}
				}

				if(Mask)
					SetMask(RPort,Mask);
				else
				{
					ScrollFrameUpdate();

					return;
				}
			}
			else
			{
				LONG i;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
					{
						if(ScrollLines[i].Width > ScrollLineWidth)
							ScrollLineWidth = ScrollLines[i].Width;

						if(ScrollLines[i].Left < ScrollLineLeft)
							ScrollLineLeft = ScrollLines[i].Left;

						if(ScrollLines[i].Right > ScrollLineRight)
							ScrollLineRight = ScrollLines[i].Right;
					}
				}
			}

			if(ScrollLineWidth)
			{
				LONG Temp;

				if((Temp = ScrollLineLeft * ScrollLineWidth) > MinX)
					MinX = Temp;

				if((Temp = ScrollLineRight * ScrollLineWidth) < MaxX)
					MaxX = Temp;

				if(MaxX == ScrollLineRight)
					MaxX += FontRightExtend;	/* Add margin for italics or boldface. */

				if((Temp = MUL_Y(ScrollLineFirst)) > MinY)
					MinY = Temp;

				if((Temp = MUL_Y(ScrollLineLast + 1) - 1) < MaxY)
					MaxY = Temp;
			}

			if(Start < ScrollLineFirst)
				ScrollLineFirst = Start;

			if(Stop > ScrollLineLast)
				ScrollLineLast = Stop;

			ScrollFrameUpdate();

			if(!ScrollLineWidth)
				return;
		}

			/* And clear the raster. */

		if(MinX < MaxX && MinY < MaxY)
		{
			MinX += WindowLeft;
			MinY += WindowTop;
			MaxX += WindowLeft;
			MaxY += WindowTop;

			if(FgPen == BgPen || (Attributes & ATTR_INVERSE))
				RectFill(RPort,MinX,MinY,MaxX,MaxY);
			else
			{
				SetAPen(RPort,BgPen);
				RectFill(RPort,MinX,MinY,MaxX,MaxY);
				SetAPen(RPort,FgPen);
			}
		}
	}
}

	/* ScrollLineRaster():
	 *
	 *	Scroll the window raster with the help
	 *	of the scrolling information.
	 */

VOID
ScrollLineRaster(struct RastPort *RPort,LONG DeltaX,LONG DeltaY,LONG MinX,LONG MinY,LONG MaxX,LONG MaxY,BOOL Smooth)
{
	if((DeltaX || DeltaY) && MinX < MaxX && MinY < MaxY)
	{
		LONG Start,Stop;
		LONG BackPen;
		BOOL ResetPen;

		if((Attributes & ATTR_INVERSE) && (FgPen != BgPen))
		{
			SetBPen(RPort,FgPen);

			ResetPen	= TRUE;
			BackPen		= FgPen;
		}
		else
		{
			ResetPen	= FALSE;
			BackPen		= BgPen;
		}

		Start	= MinY / TextFontHeight;
		Stop	= MaxY / TextFontHeight;

		if(BackPen)
		{
			if(UseMasking)
			{
				LONG i,Mask;

				Mask = BackPen;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
						Mask |= ScrollLines[i].ColourMask;
				}

				SetMask(RPort,Mask);
			}

			if(DeltaX)
			{
				if(!ScrollLines[CursorY].Width)
				{
					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
				else
				{
					ScrollLines[CursorY].Left	= 0;
					ScrollLines[CursorY].Right	= (LastPixel + 1) / ScrollLines[CursorY].Width;

					ScrollLines[CursorY].ColourMask |= BackPen;
				}
			}
			else
			{
				LONG i,Lines,Size;

				Lines	= DeltaY / TextFontHeight;
				Size	= (MaxY - MinY + 1) / TextFontHeight;

				if(Lines < 0)
				{
					Lines = -Lines;

					if(Size <= Lines)
					{
						for(i = 0 ; i < Size ; i++)
						{
							ScrollLines[Start + i].Left			= 0;
							ScrollLines[Start + i].Right		= LastColumn + 1;
							ScrollLines[Start + i].ColourMask	= BackPen;
							ScrollLines[Start + i].Width		= TextFontWidth;
						}
					}
					else
					{
						LONG Offset = Size - Lines;

						for(i = 1 ; i <= Offset ; i++)
							ScrollLines[Start + Size - i] = ScrollLines[Start + Size - (i + Lines)];

						for(i = 0 ; i < Lines ; i++)
						{
							ScrollLines[Start + i].Left			= 0;
							ScrollLines[Start + i].Right		= LastColumn + 1;
							ScrollLines[Start + i].ColourMask	= BackPen;
							ScrollLines[Start + i].Width		= TextFontWidth;
						}
					}
				}
				else
				{
					if(Size <= Lines)
					{
						for(i = 0 ; i < Size ; i++)
						{
							ScrollLines[Start + i].Left			= 0;
							ScrollLines[Start + i].Right		= LastColumn + 1;
							ScrollLines[Start + i].ColourMask	= BackPen;
							ScrollLines[Start + i].Width		= TextFontWidth;
						}
					}
					else
					{
						LONG Offset = Size - Lines;

						for(i = 0 ; i < Offset ; i++)
							ScrollLines[Start + i] = ScrollLines[Start + Lines + i];

						for(i = Offset ; i < Size ; i++)
						{
							ScrollLines[Start + i].Left			= 0;
							ScrollLines[Start + i].Right		= LastColumn + 1;
							ScrollLines[Start + i].ColourMask	= BackPen;
							ScrollLines[Start + i].Width		= TextFontWidth;
						}
					}
				}
			}
		}
		else
		{
			LONG ScrollLineLeft,ScrollLineRight,ScrollLineWidth;

			ScrollLineLeft	= 32767;
			ScrollLineRight	= 0;
			ScrollLineWidth	= 0;

			if(UseMasking)
			{
				LONG Mask,i;

				Mask = 0;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
					{
						if(ScrollLines[i].Left < ScrollLineLeft)
							ScrollLineLeft = ScrollLines[i].Left;

						if(ScrollLines[i].Right > ScrollLineRight)
							ScrollLineRight = ScrollLines[i].Right;

						if(ScrollLines[i].Width > ScrollLineWidth)
							ScrollLineWidth = ScrollLines[i].Width;

						Mask |= ScrollLines[i].ColourMask;
					}
				}

				if(Mask)
					SetMask(RPort,Mask);
				else
				{
					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
			}
			else
			{
				if(!DeltaX)
				{
					LONG i;

					for(i = Start ; i <= Stop ; i++)
					{
						if(ScrollLines[i].Width)
						{
							if(ScrollLines[i].Left < ScrollLineLeft)
								ScrollLineLeft = ScrollLines[i].Left;

							if(ScrollLines[i].Right > ScrollLineRight)
								ScrollLineRight = ScrollLines[i].Right;

							if(ScrollLines[i].Width > ScrollLineWidth)
								ScrollLineWidth = ScrollLines[i].Width;
						}
					}
				}
			}

			if(DeltaX)
			{
				if(!ScrollLines[CursorY].Width)
				{
					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
				else
				{
					LONG Last;

					Last = (LastPixel + 1) / ScrollLines[CursorY].Width;

					if(DeltaX > 0)
						ScrollLines[CursorY].Left -= DeltaX / ScrollLines[CursorY].Width;
					else
						ScrollLines[CursorY].Right -= DeltaX / ScrollLines[CursorY].Width;

					if(ScrollLines[CursorY].Right < 0)
						ScrollLines[CursorY].Right = 0;
					else
					{
						if(ScrollLines[CursorY].Right > Last)
							ScrollLines[CursorY].Right = Last;
					}

					if(ScrollLines[CursorY].Left < 0)
						ScrollLines[CursorY].Left = 0;
					else
					{
						if(ScrollLines[CursorY].Left > Last)
							ScrollLines[CursorY].Left = Last;
					}
				}
			}
			else
			{
				LONG Lines,Size,i;

				Lines	= DeltaY / TextFontHeight;
				Size	= (MaxY - MinY + 1) / TextFontHeight;

				if(Lines < 0)
				{
					Lines = -Lines;

					if(Size <= Lines)
					{
						for(i = 0 ; i < Size ; i++)
						{
							ScrollLines[Start + i].Left			= 32767;
							ScrollLines[Start + i].Right		= 0;
							ScrollLines[Start + i].ColourMask	= 0;
							ScrollLines[Start + i].Width		= 0;
						}
					}
					else
					{
						LONG Offset;

						Offset = Size - Lines;

						for(i = 1 ; i <= Offset ; i++)
							ScrollLines[Start + Size - i] = ScrollLines[Start + Size - (i + Lines)];

						for(i = 0 ; i < Lines ; i++)
						{
							ScrollLines[Start + i].Left			= 32767;
							ScrollLines[Start + i].Right		= 0;
							ScrollLines[Start + i].ColourMask	= 0;
							ScrollLines[Start + i].Width		= 0;
						}
					}
				}
				else
				{
					if(Size <= Lines)
					{
						for(i = 0 ; i < Size ; i++)
						{
							ScrollLines[Start + i].Left			= 32767;
							ScrollLines[Start + i].Right		= 0;
							ScrollLines[Start + i].ColourMask	= 0;
							ScrollLines[Start + i].Width		= 0;
						}
					}
					else
					{
						LONG Offset;

						Offset = Size - Lines;

						for(i = 0 ; i < Offset ; i++)
							ScrollLines[Start + i] = ScrollLines[Start + Lines + i];

						for(i = Offset ; i < Size ; i++)
						{
							ScrollLines[Start + i].Left			= 32767;
							ScrollLines[Start + i].Right		= 0;
							ScrollLines[Start + i].ColourMask	= 0;
							ScrollLines[Start + i].Width		= 0;
						}
					}
				}

				if(ScrollLineWidth)
				{
					LONG Temp;

					if((Temp = ScrollLineLeft * ScrollLineWidth) > MinX)
						MinX = Temp;

					if((Temp = ScrollLineRight * ScrollLineWidth) < MaxX)
						MaxX = Temp;

					if(MaxX == ScrollLineRight)
						MaxX += FontRightExtend;	/* Add margin for italics or boldface. */

					if(DeltaY < 0)
					{
						if((Temp = MUL_Y(ScrollLineFirst)) > MinY)
							MinY = Temp;

						if((Temp = MUL_Y(ScrollLineLast + 1) - 1 - DeltaY) < MaxY)
							MaxY = Temp;
					}
					else
					{
						if(DeltaY > 0)
						{
							if((Temp = MUL_Y(ScrollLineFirst) - DeltaY) > MinY)
								MinY = Temp;

							if((Temp = MUL_Y(ScrollLineLast + 1) - 1) < MaxY)
								MaxY = Temp;
						}
					}

					if(MinX < 0)
						MinX = 0;

					if(MaxX < 0)
						MaxX = 0;

					if(MinY < 0)
						MinY = 0;

					if(MaxY < 0)
						MaxY = 0;
				}
				else
				{
					ScrollFrameUpdate();

					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
			}
		}

		if(Start < ScrollLineFirst)
			ScrollLineFirst = Start;

		if(Stop > ScrollLineLast)
			ScrollLineLast = Stop;

		ScrollFrameUpdate();

		if(MinX < MaxX && MinY < MaxY)
		{
			MinX += WindowLeft;
			MaxX += WindowLeft;

			MinY += WindowTop;
			MaxY += WindowTop;

				/* Smooth scrolling requested? */

			if(Smooth && DeltaY)
			{
				LONG Lines,Extra,Direction;

				Lines = ABS(DeltaY);
				Extra = Lines & 1;
				Lines = Lines / 2;

				if(DeltaY > 0)
					Direction = 2;
				else
					Direction = -2;

				while(Lines-- > 0)
				{
					WaitBlit();
					WaitTOF();
					ScrollRaster(RPort,0,Direction,MinX,MinY,MaxX,MaxY);
				}

				if(Extra)
				{
					WaitBlit();
					WaitTOF();
					ScrollRaster(RPort,0,Direction / 2,MinX,MinY,MaxX,MaxY);
				}
			}
			else
				ScrollRaster(RPort,DeltaX,DeltaY,MinX,MinY,MaxX,MaxY);
		}

		if(ResetPen)
			SetBPen(RPort,BgPen);
	}
}

VOID
ScrollLineRasterNoTabChange(struct RastPort *RPort,LONG DeltaX,LONG DeltaY,LONG MinX,LONG MinY,LONG MaxX,LONG MaxY,BOOL Smooth)
{
	if((DeltaX || DeltaY) && MinX < MaxX && MinY < MaxY)
	{
		LONG Start,Stop;
		LONG BackPen;
		BOOL ResetPen;

		if((Attributes & ATTR_INVERSE) && (FgPen != BgPen))
		{
			SetBPen(RPort,FgPen);

			ResetPen	= TRUE;
			BackPen		= FgPen;
		}
		else
		{
			ResetPen	= FALSE;
			BackPen		= BgPen;
		}

		Start	= MinY / TextFontHeight;
		Stop	= MaxY / TextFontHeight;

		if(BackPen)
		{
			if(UseMasking)
			{
				LONG i,Mask = BackPen;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
						Mask |= ScrollLines[i].ColourMask;
				}

				SetMask(RPort,Mask);
			}

			if(DeltaX)
			{
				if(!ScrollLines[CursorY].Width)
				{
					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
			}
		}
		else
		{
			LONG ScrollLineLeft,ScrollLineRight,ScrollLineWidth;

			ScrollLineLeft	= 32767;
			ScrollLineRight	= 0;
			ScrollLineWidth	= 0;

			if(UseMasking)
			{
				LONG i,Mask;

				Mask = 0;

				for(i = Start ; i <= Stop ; i++)
				{
					if(ScrollLines[i].Width)
					{
						if(ScrollLines[i].Left < ScrollLineLeft)
							ScrollLineLeft = ScrollLines[i].Left;

						if(ScrollLines[i].Right > ScrollLineRight)
							ScrollLineRight = ScrollLines[i].Right;

						if(ScrollLines[i].Width > ScrollLineWidth)
							ScrollLineWidth = ScrollLines[i].Width;

						Mask |= ScrollLines[i].ColourMask;
					}
				}

				if(Mask)
					SetMask(RPort,Mask);
				else
				{
					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
			}
			else
			{
				if(!DeltaX)
				{
					LONG i;

					for(i = Start ; i <= Stop ; i++)
					{
						if(ScrollLines[i].Width)
						{
							if(ScrollLines[i].Left < ScrollLineLeft)
								ScrollLineLeft = ScrollLines[i].Left;

							if(ScrollLines[i].Right > ScrollLineRight)
								ScrollLineRight = ScrollLines[i].Right;

							if(ScrollLines[i].Width > ScrollLineWidth)
								ScrollLineWidth = ScrollLines[i].Width;
						}
					}
				}
			}

			if(DeltaX)
			{
				if(!ScrollLines[CursorY].Width)
				{
					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
			}
			else
			{
				if(ScrollLineWidth)
				{
					LONG Temp;

					if((Temp = ScrollLineLeft * ScrollLineWidth) > MinX)
						MinX = Temp;

					if((Temp = ScrollLineRight * ScrollLineWidth) < MaxX)
						MaxX = Temp;

					if(MaxX == ScrollLineRight)
						MaxX += FontRightExtend;	/* Add margin for italics or boldface. */

					if(DeltaY < 0)
					{
						if((Temp = MUL_Y(ScrollLineFirst)) > MinY)
							MinY = Temp;

						if((Temp = MUL_Y(ScrollLineLast + 1) - 1 - DeltaY) < MaxY)
							MaxY = Temp;
					}
					else
					{
						if(DeltaY > 0)
						{
							if((Temp = MUL_Y(ScrollLineFirst) - DeltaY) > MinY)
								MinY = Temp;

							if((Temp = MUL_Y(ScrollLineLast + 1) - 1) < MaxY)
								MaxY = Temp;
						}
					}

					if(MinX < 0)
						MinX = 0;

					if(MaxX < 0)
						MaxX = 0;

					if(MinY < 0)
						MinY = 0;

					if(MaxY < 0)
						MaxY = 0;
				}
				else
				{
					ScrollFrameUpdate();

					if(ResetPen)
						SetBPen(RPort,BgPen);

					return;
				}
			}
		}

		if(Start < ScrollLineFirst)
			ScrollLineFirst = Start;

		if(Stop > ScrollLineLast)
			ScrollLineLast = Stop;

		ScrollFrameUpdate();

		if(MinX < MaxX && MinY < MaxY)
		{
			MinX += WindowLeft;
			MaxX += WindowLeft;

			MinY += WindowTop;
			MaxY += WindowTop;

				/* Smooth scrolling requested? */

			if(Smooth && DeltaY)
			{
				LONG Lines,Extra,Direction;

				Lines = ABS(DeltaY);
				Extra = Lines & 1;
				Lines = Lines / 2;

				if(DeltaY > 0)
					Direction = 2;
				else
					Direction = -2;

				while(Lines-- > 0)
				{
					WaitBlit();
					WaitTOF();
					ScrollRaster(RPort,0,Direction,MinX,MinY,MaxX,MaxY);
				}

				if(Extra)
				{
					WaitBlit();
					WaitTOF();
					ScrollRaster(RPort,0,Direction / 2,MinX,MinY,MaxX,MaxY);
				}
			}
			else
				ScrollRaster(RPort,DeltaX,DeltaY,MinX,MinY,MaxX,MaxY);
		}

		if(ResetPen)
			SetBPen(RPort,BgPen);
	}
}

	/* ScrollLineEraseScreen(BYTE Mode):
	 *
	 *	Erase a part of the screen.
	 */

VOID
ScrollLineEraseScreen(LONG Mode)
{
	LONG i;

	if(BgPen)
	{
		switch(Mode)
		{
				/* Erase from first line to current cursor position (inclusive). */

			case 1:

				ScrollLineFirst = CursorY;

				if(ScrollLineLast < CursorY)
					ScrollLineLast = CursorY;

					/* Reset the lines. */

				for(i = 0 ; i < CursorY ; i++)
				{
					ScrollLines[i].Left			= 0;
					ScrollLines[i].Right		= LastColumn + 1;
					ScrollLines[i].ColourMask	= BgPen;
					ScrollLines[i].Width		= TextFontWidth;
				}

				if(CursorX)
				{
					if(!ScrollLines[CursorY].Width)
						ScrollLines[CursorY].Width = GetFontWidth();

					ScrollLines[CursorY].Left		= 0;
					ScrollLines[CursorY].ColourMask	|= BgPen;
				}

				break;

				/* Erase entire screen. */

			case 2:

				for(i = 0 ; i < RasterHeight ; i++)
				{
					ScrollLines[i].Left			= 0;
					ScrollLines[i].Right		= LastColumn + 1;
					ScrollLines[i].ColourMask	= BgPen;
					ScrollLines[i].Width		= TextFontWidth;
				}

				ScrollLineFirst	= 0;
				ScrollLineLast	= RasterHeight - 1;

				break;

				/* Erase from current cursor position to end of screen. */

			default:

				for(i = CursorY + 1 ; i < RasterHeight ; i++)
				{
					ScrollLines[i].Left			= 0;
					ScrollLines[i].Right		= LastColumn + 1;
					ScrollLines[i].ColourMask	= BgPen;
					ScrollLines[i].Width		= TextFontWidth;
				}

				if(!ScrollLines[CursorY].Width)
					ScrollLines[CursorY].Width = GetFontWidth();

				ScrollLines[CursorY].Right = (LastPixel + 1) / ScrollLines[CursorY].Width;

				if(CursorX)
					ScrollLines[CursorY].ColourMask |= BgPen;
				else
					ScrollLines[CursorY].ColourMask = BgPen;

				if(ScrollLineFirst > CursorY)
					ScrollLineFirst = CursorY;

				ScrollLineLast = RasterHeight - 1;

				break;
		}
	}
	else
	{
		switch(Mode)
		{
				/* Erase from first line to current cursor line (inclusive). */

			case 1:

					/* Reset the lines. */

				for(i = 0 ; i < CursorY ; i++)
				{
					ScrollLines[i].Left			= 32767;
					ScrollLines[i].Right		= 0;
					ScrollLines[i].ColourMask	= 0;
					ScrollLines[i].Width		= 0;
				}

				ScrollLines[CursorY].Left = CursorX;

				if(ScrollLines[CursorY].Right < ScrollLines[CursorY].Left)
				{
					ScrollLines[CursorY].Left	= 32767;
					ScrollLines[CursorY].Right	= 0;
					ScrollLines[CursorY].Width	= 0;

						/* Cleared the entire screen? */

					if(CursorY == RasterHeight - 1)
					{
						ScrollLineFirst	= 32767;
						ScrollLineLast	= 0;
					}
					else
						ScrollLineFirst = CursorY;
				}
				else
					ScrollLineFirst = CursorY;

				if(ScrollLineLast < CursorY)
					ScrollLineLast = CursorY;

				break;

				/* Erase entire screen. */

			case 2:

				for(i = 0 ; i < RasterHeight ; i++)
				{
					ScrollLines[i].Left			= 32767;
					ScrollLines[i].Right		= 0;
					ScrollLines[i].ColourMask	= 0;
					ScrollLines[i].Width		= 0;
				}

				ScrollLineFirst	= 32767;
				ScrollLineLast	= 0;

				break;

				/* Erase from current cursor position to end of screen. */

			default:

				for(i = CursorY + 1 ; i < RasterHeight ; i++)
				{
					ScrollLines[i].Left			= 32767;
					ScrollLines[i].Right		= 0;
					ScrollLines[i].ColourMask	= 0;
					ScrollLines[i].Width		= 0;
				}

				if(CursorX)
				{
					ScrollLines[CursorY].Right = CursorX;

					if(ScrollLines[CursorY].Right < ScrollLines[CursorY].Left)
					{
						ScrollLines[CursorY].Left	= 32767;
						ScrollLines[CursorY].Right	= 0;
						ScrollLines[CursorY].Width	= 0;
					}
				}
				else
				{
					ScrollLines[CursorY].Left	= 32767;
					ScrollLines[CursorY].Right	= 0;
					ScrollLines[CursorY].Width	= 0;
				}

				/* Cleared the entire screen? */

				if(CursorY)
					ScrollLineLast = CursorY;
				else
				{
					if(ScrollLines[CursorY].Right < ScrollLines[CursorY].Left)
					{
						ScrollLineFirst	= 32767;
						ScrollLineLast	= 0;
					}
					else
					{
						if(ScrollLineFirst > CursorY)
							ScrollLineFirst = CursorY;
					}
				}

				break;
		}
	}

	ScrollFrameUpdate();
}

	/* ScrollLineEraseLine(BYTE Mode):
	 *
	 *	Erase parts of the current cursor line.
	 */

VOID
ScrollLineEraseLine(LONG Mode)
{
	if(BgPen)
	{
		switch(Mode)
		{
				/* Erase from left margin to current cursor position (inclusive). */

			case 1:

				ScrollLines[CursorY].Left = 0;

				ScrollLines[CursorY].ColourMask |= BgPen;

				if(!ScrollLines[CursorY].Width)
					ScrollLines[CursorY].Width = GetFontWidth();

				break;

				/* Erase entire line. */

			case 2:

				ScrollLines[CursorY].Width		= GetFontWidth();
				ScrollLines[CursorY].Left		= 0;
				ScrollLines[CursorY].Right		= (LastPixel + 1) / ScrollLines[CursorY].Width;
				ScrollLines[CursorY].ColourMask	= BgPen;

				break;

				/* Erase from current cursor position towards end of line. */

			default:

				if(CursorX)
				{
					if(!ScrollLines[CursorY].Width)
						ScrollLines[CursorY].Width = GetFontWidth();

					ScrollLines[CursorY].ColourMask |= BgPen;
				}
				else
				{
					ScrollLines[CursorY].Width		= GetFontWidth();
					ScrollLines[CursorY].Left		= 0;
					ScrollLines[CursorY].ColourMask	= BgPen;
				}

				ScrollLines[CursorY].Right = (LastPixel + 1) / ScrollLines[CursorY].Width;
				break;
		}
	}
	else
	{
		switch(Mode)
		{
				/* Erase from left margin to current cursor position (inclusive). */

			case 1:

				ScrollLines[CursorY].Left = CursorX;

				if(ScrollLines[CursorY].Left >= ScrollLines[CursorY].Right)
				{
					ScrollLines[CursorY].Left	= 32767;
					ScrollLines[CursorY].Right	= 0;
					ScrollLines[CursorY].Width	= 0;
				}

				break;

				/* Erase entire line. */

			case 2:

				ScrollLines[CursorY].Left	= 32767;
				ScrollLines[CursorY].Right	= 0;
				ScrollLines[CursorY].Width	= 0;

				break;

				/* Erase from current cursor position towards end of line. */

			default:

				if(CursorX)
					ScrollLines[CursorY].Right = CursorX;
				else
				{
					ScrollLines[CursorY].Left	= 32767;
					ScrollLines[CursorY].Right	= 0;
					ScrollLines[CursorY].Width	= 0;
				}

				break;
		}
	}

	ScrollFrameUpdate();
}

	/* ScrollLineEraseCharacters(LONG Chars):
	 *
	 *	Erase a number of characters in the current cursor line.
	 */

VOID
ScrollLineEraseCharacters(LONG Chars)
{
		/* Any characters to erase? */

	if(BgPen)
	{
		if(!ScrollLines[CursorY].Width)
			ScrollLines[CursorY].Width = GetFontWidth();

		ScrollLines[CursorY].Left	= 0;
		ScrollLines[CursorY].Right	= (LastPixel + 1) / ScrollLines[CursorY].Width;

		ScrollLines[CursorY].ColourMask |= BgPen;
	}
	else
	{
		if(ScrollLines[CursorY].Right > Chars)
			ScrollLines[CursorY].Right -= Chars;
		else
			ScrollLines[CursorY].Right = 0;
	}
}

	/* ScrollLineShiftChar(LONG Size):
	 *
	 *	Shift the characters following the current cursor position
	 *	Size characters to the right.
	 */

VOID
ScrollLineShiftChar(LONG Size)
{
		/* Any characters to scroll? */

	if(BgPen)
	{
		if(!ScrollLines[CursorY].Width)
			ScrollLines[CursorY].Width = GetFontWidth();

		ScrollLines[CursorY].Left	= 0;
		ScrollLines[CursorY].Right	= (LastPixel + 1) / ScrollLines[CursorY].Width;

		ScrollLines[CursorY].ColourMask |= BgPen;
	}
	else
	{
		LONG Temp;

		ScrollLines[CursorY].Right += Size;

		if((Temp = ScrollLines[CursorY].Right * ScrollLines[CursorY].Width) > LastPixel)
			ScrollLines[CursorY].Right = (LastPixel + 1) / ScrollLines[CursorY].Width;
		else
		{
			if(ScrollLines[CursorY].Right < 0)
				ScrollLines[CursorY].Right = 0;
		}
	}
}

	/* ScrollLinePutString(LONG Length):
	 *
	 *	Update the line info according to the length of a string
	 *	to be printed.
	 */

VOID
ScrollLinePutString(LONG Length)
{
	if(Length)
	{
		LONG Width;

			/* Which scale is the font we are currently using? */

		if(RasterAttr[CursorY] >= SCALE_ATTR_TOP2X)
		{
				/* Valid length? */

			if(CursorX + Length >= RasterWidth / 2)
				Length = (RasterWidth / 2) - CursorX;

				/* Double width. */

			Width = TextFontWidth * 2;
		}
		else
		{
			if(CurrentCharWidth == SCALE_HALF)
			{
					/* Valid length? */

				if(CursorX + Length >= RasterWidth * 2)
					Length = (RasterWidth * 2) - CursorX;

					/* Half width. */

				Width = TextFontWidth / 2;
			}
			else
			{
					/* Valid length? */

				if(CursorX + Length >= RasterWidth)
					Length = RasterWidth - CursorX;

					/* Normal width. */

				Width = TextFontWidth;
			}
		}

			/* Sensible value? */

		if(Length > 0)
		{
			struct ScrollLineInfo *Alias;

			Alias = &ScrollLines[CursorY];

				/* Update font scale. */

			Alias->Width = Width;

				/* Update right margin. */

			if(CursorX < Alias->Left)
				Alias->Left = CursorX;

				/* Update left margin. */

			if(CursorX + Length > Alias->Right)
				Alias->Right = CursorX + Length;

				/* Update topmost line. */

			if(CursorY < ScrollLineFirst)
				ScrollLineFirst = CursorY;

				/* Update bottommost line. */

			if(CursorY > ScrollLineLast)
				ScrollLineLast = CursorY;

			if(UseMasking)
			{
					/* Update line colour mask. */

				Alias->ColourMask |= FgPen | BgPen;

					/* Set write mask (will affect Text() since it is called
					 * after this routine has finished.
					 */

				SetMask(RPort,Alias->ColourMask);
			}
		}
	}
}
