/*
**	Marker.c
**
**	Text block marker routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* BM_Draw():
	 *
	 *	Redraw or remove marked regions.
	 */

VOID
BM_Draw(struct BlockMarker *Marker)
{
		/* Is the first line the same as the last line? If so,
		 * continue and mark only single characters. Else,
		 * determine first line and column to mark and last
		 * line and last column.
		 */

	if(Marker->FirstLine != Marker->LastLine)
	{
		LONG	First	= Marker->FirstLine - Marker->Top,
				Last	= Marker->LastLine  - Marker->Top,
				Lines;

			/* Is the first line visible? If so, mark it. */

		if(First >= 0)
			ToggleSelect(Marker,Marker->FirstColumn,First,Marker->Width - Marker->FirstColumn,1);
		else
		{
			ToggleSelect(Marker,0,0,Marker->Width,1);

			First = 0;
		}

			/* Is the last line visible? If so, mark it. */

		if(Last >= 0 && Last < Marker->Height)
			ToggleSelect(Marker,0,Last,Marker->LastColumn,1);
		else
			Last = Marker->Height;

			/* Determine the number of lines the selection spans. */

		if((Lines = Last - First - 1) > 0)
			ToggleSelect(Marker,0,First + 1,Marker->Width,Lines);
	}
	else
	{
			/* Is the first column different from the last column? */

		if(Marker->FirstColumn != Marker->LastColumn)
		{
				/* Is the line visible? If so, mark it. */

			if(Marker->Top <= Marker->LastLine && Marker->LastLine < Marker->Top + Marker->Height)
				ToggleSelect(Marker,Marker->FirstColumn,Marker->FirstLine - Marker->Top,Marker->LastColumn - Marker->FirstColumn,1);
		}
	}
}

	/* BM_ClearMark(struct BlockMarker *Marker):
	 *
	 *	Free block marker memory.
	 */

VOID
BM_ClearMark(struct BlockMarker *Marker)
{
	BM_Draw(Marker);

	FreeVecPooled(Marker);
}

	/* BM_SetMark():
	 *
	 *	Create block marker structure.
	 */

struct BlockMarker *
BM_SetMark(struct RastPort *RPort,LONG Width,LONG Height,LONG LeftEdge,LONG TopEdge,LONG Top,LONG Lines,LONG X,LONG Y,LONG TextFontWidth,LONG TextFontHeight)
{
	if(Height && Lines)
	{
		struct BlockMarker *Marker;

			/* Allocate marker buffer. */

		if(Marker = (struct BlockMarker *)AllocVecPooled(sizeof(struct BlockMarker),MEMF_ANY | MEMF_CLEAR))
		{
				/* Fill in the RastPort. */

			Marker->RPort = RPort;

				/* Fill in the canvas left and top edge. */

			Marker->LeftEdge	= LeftEdge;
			Marker->TopEdge		= TopEdge;

				/* Fill in current display window top and number of
				 * lines in the buffer.
				 */

			Marker->Top		= Top;
			Marker->Lines	= Lines;

				/* Fill in width and height of the display window. */

			Marker->Width	= Width;
			Marker->Height	= Height;

				/* Fill in the marker anchor point. */

			Marker->FirstColumn	= X;
			Marker->LastColumn	= X;
			Marker->FirstLine	= Y + Top;
			Marker->LastLine	= Y + Top;

				/* Fill in current mouse position. */

			Marker->LastX	= X;
			Marker->LastY	= Y;

			Marker->OriginX	= X;
			Marker->OriginY	= Y + Top;

				/* Remember text font dimensions. */

			Marker->TextFontWidth	= TextFontWidth;
			Marker->TextFontHeight	= TextFontHeight;

			Marker->WriteMask = RPort->Mask;
		}

			/* Return marker buffer. */

		return(Marker);
	}
	else
		return(NULL);
}

	/* BM_ExtendMark(struct BlockMarker *Marker,LONG X,LONG Y,LONG Delta):
	 *
	 *	Extend current block marker. This routine was
	 *	first written by me, but Martin Berndt rewrote it
	 *	after we both realised that it would not work
	 *	properly.
	 */

VOID
BM_ExtendMark(struct BlockMarker *Marker,LONG X,LONG Y,LONG Delta)
{
	if(Marker->LastX != X || Marker->LastY != Y)
	{
		LONG	OldCharPos,NewCharPos,
				CharStart,CharEnd,
				CharOrigin,
				OriginY,
				Lines;
		BOOL	Crossed;

		OriginY = Marker->OriginY - Marker->Top;

			/* Deal with illegal X position. */

		if(X < 0)
			X = 0;

		if(X > Marker->Width)
			X = Marker->Width;

			/* Deal with illegal Y position. */

		if(Y < 0)
			Y = 0;

		if(Y >= Marker->Height)
			Y = Marker->Height - 1;

		if(Y + Marker->Top >= Marker->Lines)
			Y = Marker->Lines - Marker->Top - 1;

			/* Is the Y position larger than the last line? If so,
			 * truncate it.
			 */

		if(Y > Marker->Lines - Marker->Top)
			Y -= Marker->Lines - Marker->Top;

			/* Select the text. */

		OldCharPos	= (Marker->LastY + Marker->Top) * Marker->Width + Marker->LastX;
		NewCharPos	= (Y + Marker->Top) * Marker->Width + X;

		CharStart	= Marker->FirstLine * Marker->Width + Marker->FirstColumn;
		CharEnd		= Marker->LastLine * Marker->Width + Marker->LastColumn;

		CharOrigin	= Marker->OriginY * Marker->Width + Marker->OriginX;

		Crossed		= (OldCharPos < CharOrigin) ^ (NewCharPos < CharOrigin);

		if(NewCharPos > OldCharPos)
		{
			if(Delta && Y < OriginY)
				ToggleSelect(Marker,0,Y,Marker->Width,1);

			if(Crossed)
			{
				if((Lines = OriginY - Marker->LastY - 1) >= 0)
				{
					ToggleSelect(Marker,Marker->LastX,Marker->LastY,Marker->Width - Marker->LastX,1);

					if(Lines > 0)
						ToggleSelect(Marker,0,Marker->LastY + 1,Marker->Width,Lines);

					if(Marker->OriginX)
						ToggleSelect(Marker,0,OriginY,Marker->OriginX,1);
				}
				else
				{
					if(Delta)
						ToggleSelect(Marker,0,OriginY,Marker->LastX,1);
					else
						ToggleSelect(Marker,Marker->LastX,OriginY,Marker->OriginX - Marker->LastX,1);
				}

				Marker->FirstColumn	= Marker->OriginX;
				Marker->FirstLine 	= Marker->OriginY;

				Marker->LastX		= Marker->OriginX;
				Marker->LastY		= OriginY;
			}
			else
			{
				if(OldCharPos < CharOrigin)
				{
					if((Lines = Y - Marker->LastY - 1) >= 0)
					{
						ToggleSelect(Marker,Marker->LastX,Marker->LastY,Marker->Width - Marker->LastX,1);

						if(Lines > 0)
							ToggleSelect(Marker,0,Marker->LastY + 1,Marker->Width,Lines);

						ToggleSelect(Marker,0,Y,X,1);
					}
					else
					{
						if(Delta)
							ToggleSelect(Marker,0,Y,X,1);
						else
							ToggleSelect(Marker,Marker->LastX,Y,X - Marker->LastX,1);
					}

					Marker->FirstColumn	= X;
					Marker->FirstLine	= Y + Marker->Top;
				}
			}

			if(NewCharPos > CharEnd)
			{
				if((Lines = Y - Marker->LastY - 1) >= 0)
				{
					ToggleSelect(Marker,Marker->LastX,Marker->LastY,Marker->Width - Marker->LastX,1);

					if(Lines > 0)
						ToggleSelect(Marker,0,Marker->LastY + 1,Marker->Width,Lines);

					ToggleSelect(Marker,0,Y,X,1);
				}
				else
				{
					if(Delta)
						ToggleSelect(Marker,0,Y,X,1);
					else
						ToggleSelect(Marker,Marker->LastX,Y,X - Marker->LastX,1);
				}

				Marker->LastColumn	= X;
				Marker->LastLine	= Y + Marker->Top;
			}
		}
		else
		{
			if(Delta && Y > OriginY)
				ToggleSelect(Marker,0,Y,Marker->Width,1);

			if(Crossed)
			{
				if((Lines = Marker->LastY - OriginY - 1) >= 0)
				{
					ToggleSelect(Marker,0,Marker->LastY,Marker->LastX,1);

					if(Lines > 0)
						ToggleSelect(Marker,0,OriginY + 1,Marker->Width,Lines);

					ToggleSelect(Marker,Marker->OriginX,OriginY,Marker->Width - Marker->OriginX,1);
				}
				else
				{
					if(Delta)
						ToggleSelect(Marker,Marker->LastX,OriginY,Marker->Width - Marker->LastX,1);
					else
						ToggleSelect(Marker,Marker->OriginX,OriginY,Marker->LastX - Marker->OriginX,1);
				}

				Marker->LastColumn	= Marker->OriginX;
				Marker->LastLine	= Marker->OriginY;

				Marker->LastX		= Marker->OriginX;
				Marker->LastY		= OriginY;
			}
			else
			{
				if(OldCharPos > CharOrigin)
				{
					if((Lines = Marker->LastY - Y - 1) >= 0)
					{
						if(Marker->LastX)
							ToggleSelect(Marker,0,Marker->LastY,Marker->LastX,1);

						if(Lines > 0)
							ToggleSelect(Marker,0,Y + 1,Marker->Width,Lines);

						ToggleSelect(Marker,X,Y,Marker->Width - X,1);
					}
					else
					{
						if(Delta)
							ToggleSelect(Marker,X,Y,Marker->Width - X,1);
						else
							ToggleSelect(Marker,X,Y,Marker->LastX - X,1);
					}

					Marker->LastColumn	= X;
					Marker->LastLine	= Y + Marker->Top;
				}
			}

			if(NewCharPos < CharStart)
			{
				if((Lines = Marker->LastY - Y - 1) >= 0)
				{
					if(Marker->LastX)
						ToggleSelect(Marker,0,Marker->LastY,Marker->LastX,1);

					if(Lines > 0)
						ToggleSelect(Marker,0,Y + 1,Marker->Width,Lines);

					ToggleSelect(Marker,X,Y,Marker->Width - X,1);
				}
				else
				{
					if(Delta)
						ToggleSelect(Marker,X,Y,Marker->Width - X,1);
					else
						ToggleSelect(Marker,X,Y,Marker->LastX - X,1);
				}

				Marker->FirstColumn	= X;
				Marker->FirstLine	= Y + Marker->Top;
			}
		}

		Marker->LastX = X;
		Marker->LastY = Y;
	}
}


	/* ToggleSelect(struct BlockMarker *Marker,LONG Left,LONG Top,LONG Width,LONG Height):
	 *
	 *	Toggle selection subroutine.
	 */

VOID
ToggleSelect(struct BlockMarker *Marker,LONG Left,LONG Top,LONG Width,LONG Height)
{
	if(Width && Height)
	{
		struct RastPort *RPort = Marker->RPort;

		ULONG	OldAPen	= ReadAPen(RPort),
				OldBPen	= ReadBPen(RPort),
				OldDrMd	= ReadDrMd(RPort),
				Mask;

		Left	= Marker->LeftEdge + Left * Marker->TextFontWidth;
		Top		= Marker->TopEdge + Top * Marker->TextFontHeight;

		if(Kick30)
			Mask = (1L << GetBitMapAttr(RPort->BitMap,BMA_DEPTH)) - 1;
		else
			Mask = (1L << RPort->BitMap->Depth) - 1;

		SetPens(RPort,Mask,OldBPen,JAM1 | COMPLEMENT);

		FillBox(RPort,Left,Top,Width * Marker->TextFontWidth,Height * Marker->TextFontHeight);

		SetPens(RPort,OldAPen,OldBPen,OldDrMd);
	}
}

	/* WriteTrimmedString(struct IFFHandle *Handle,STRPTR String,LONG Len,BOOL NeedClipConversion):
	 *
	 *	Write a string to the clipboard, sans trailing spaces.
	 */

VOID
WriteTrimmedString(struct IFFHandle *Handle,STRPTR String,LONG Len,BOOL NeedClipConversion)
{
	while(Len > 0 && String[Len - 1] == ' ')
		Len--;

	if(Len)
	{
		if(NeedClipConversion)
		{
			UBYTE	Buffer[256];
			LONG	Size,i;
			STRPTR	Dest;
			UBYTE	c;

			do
			{
				Size = MIN(Len,sizeof(Buffer));

				Len -= Size;

				for(i = 0, Dest = Buffer ; i < Size ; i++)
				{
					if(c = ISOConversion[*String++])
						*Dest++ = c;
					else
						*Dest++ = ' ';
				}

				if(Dest > Buffer)
				{
					Size = (ULONG)Dest - (ULONG)&Buffer[0];

					while(Size > 0 && Buffer[Size - 1] == ' ')
						Size--;

					if(Size > 0)
						WriteChunkBytes(Handle,Buffer,Size);
				}
			}
			while(Len > 0);
		}
		else
			WriteChunkBytes(Handle,String,Len);
	}
}

	/* ClipPage(struct BlockMarker *Marker,BOOL Append,BOOL NeedClipConversion):
	 *
	 *	Send the entire marked page to the clipboard.
	 */

STATIC VOID
ClipPage(struct BlockMarker *Marker,BOOL Append,BOOL NeedClipConversion)
{
	struct IFFHandle *Handle;
	APTR Buffer;
	LONG Size;

	if(Append)
		GetClipContents(Config->ClipConfig->ClipboardUnit,&Buffer,&Size);
	else
	{
		Buffer	= NULL;
		Size	= 0;
	}

	if(Handle = OpenIFFClip(Config->ClipConfig->ClipboardUnit,MODE_NEWFILE))
	{
		if(!PushChunk(Handle,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN))
		{
			if(!PushChunk(Handle,0,ID_CHRS,IFFSIZE_UNKNOWN))
			{
				LONG i,Lines = Marker->LastLine - Marker->FirstLine - 1;

				if(Buffer)
				{
					WriteChunkBytes(Handle,Buffer,Size);

					FreeVecPooled(Buffer);
				}

				WriteTrimmedString(Handle,&Raster[RasterWidth * Marker->FirstLine + Marker->FirstColumn],Marker->Width - Marker->FirstColumn,NeedClipConversion);
				WriteChunkBytes(Handle,"\n",1);

				if(Lines > 0)
				{
					STRPTR Line = &Raster[(Marker->FirstLine + 1) * RasterWidth];

					for(i = 0 ; i < Lines ; i++)
					{
						WriteTrimmedString(Handle,Line,Marker->Width,NeedClipConversion);
						WriteChunkBytes(Handle,"\n",1);

						Line += RasterWidth;
					}
				}

				WriteTrimmedString(Handle,&Raster[RasterWidth * Marker->LastLine],Marker->LastColumn,NeedClipConversion);
				WriteChunkBytes(Handle,"\n",1);

				PopChunk(Handle);
			}

			PopChunk(Handle);
		}

		CloseIFFClip(Handle);
	}
}

	/* MarkWord(LONG MouseX,LONG MouseY):
	 *
	 *	Mark a single word on the main screen (double-click).
	 */

VOID
MarkWord(LONG MouseX,LONG MouseY)
{
	LONG FirstX,FirstY;

	FirstX = (MouseX * CharCellDenominator) / (TextFontWidth * CharCellNominator);
	FirstY = MouseY / TextFontHeight;

	if(FirstX > LastPrintableColumn)
		FirstX = LastPrintableColumn;

	if(FirstY > LastLine)
		FirstY = LastLine;

	SafeObtainSemaphoreShared(&RasterSemaphore);

	if(Raster[FirstY * RasterWidth + FirstX] != ' ' && Raster[FirstY * RasterWidth + FirstX])
	{
		STRPTR	Line = &Raster[FirstY * RasterWidth];
		LONG	LastX;

		LastX = FirstX;

		while(FirstX > 0 && Line[FirstX - 1] != ' ')
			FirstX--;

		while(LastX < LastPrintableColumn && Line[LastX + 1] != ' ')
			LastX++;

		ObtainSemaphore(&TerminalSemaphore);

		if(WindowMarker = BM_SetMark(Window->RPort,LastPrintableColumn + 1,LastLine + 1,WindowLeft,WindowTop,0,LastLine + 1,FirstX,FirstY,(TextFontWidth * CharCellNominator) / CharCellDenominator,TextFontHeight))
		{
			SetMask(RPort,DepthMask);

			Marking = TRUE;

			ReportMouse(TRUE,Window);

			BM_ExtendMark(WindowMarker,LastX + 1,FirstY,0);

			SetClipMenu(TRUE);
		}

		ReleaseSemaphore(&TerminalSemaphore);
	}

	ReleaseSemaphore(&RasterSemaphore);
}

	/* SetMarker(LONG MouseX,LONG MouseY):
	 *
	 *	Anchor a marker to the current mouse position.
	 */

VOID
SetMarker(LONG MouseX,LONG MouseY)
{
	LONG FirstX,FirstY;

	FirstX = (MouseX * CharCellDenominator) / (TextFontWidth * CharCellNominator);
	FirstY = MouseY / TextFontHeight;

	if(FirstX > LastPrintableColumn)
		FirstX = LastPrintableColumn;

	if(FirstY > LastLine)
		FirstY = LastLine;

	ObtainSemaphore(&TerminalSemaphore);

	if(WindowMarker = BM_SetMark(Window->RPort,LastPrintableColumn + 1,LastLine + 1,WindowLeft,WindowTop,0,LastLine + 1,FirstX,FirstY,(TextFontWidth * CharCellNominator) / CharCellDenominator,TextFontHeight))
	{
		SetMask(RPort,DepthMask);

		Marking = TRUE;

		ReportMouse(TRUE,Window);

		SetClipMenu(TRUE);
	}

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* MoveMarker(LONG MouseX,LONG MouseY):
	 *
	 *	Move the marker with the mouse.
	 */

VOID
MoveMarker(LONG MouseX,LONG MouseY)
{
	if(WindowMarker)
	{
		ULONG EffectiveWidth;

		ObtainSemaphore(&TerminalSemaphore);

		EffectiveWidth = (TextFontWidth * CharCellNominator) / CharCellDenominator;

		BM_ExtendMark(WindowMarker,(MouseX + EffectiveWidth - 1) / EffectiveWidth,MouseY / TextFontHeight,0);

		ReleaseSemaphore(&TerminalSemaphore);
	}
}

	/* DropMarker():
	 *
	 *	Drop the window marker, restore the window contents.
	 */

VOID
DropMarker()
{
	if(WindowMarker)
	{
		struct RastPort	*RPort = WindowMarker->RPort;
		LONG Mask = WindowMarker->WriteMask;

		ObtainSemaphore(&TerminalSemaphore);

		BM_ClearMark(WindowMarker);

		ReleaseSemaphore(&TerminalSemaphore);

		SetMask(RPort,Mask);

		ReportMouse(FALSE,Window);

		WindowMarker = NULL;

		Marking = FALSE;

		SetClipMenu(FALSE);
	}
}

	/* FreeMarker():
	 *
	 *	Free the main window marker.
	 */

VOID
FreeMarker()
{
	if(WindowMarker)
	{
		struct RastPort	*RPort = WindowMarker->RPort;
		LONG Mask = WindowMarker->WriteMask;

		FreeVecPooled(WindowMarker);

		WindowMarker = NULL;

		SetMask(RPort,Mask);

		ReportMouse(FALSE,Window);

		Marking = FALSE;

		SetClipMenu(FALSE);
	}
}

	/* ClipMarker(BOOL Append):
	 *
	 *	Transfer the marked area to the clipboard.
	 */

VOID
ClipMarker(BOOL Append)
{
	if(WindowMarker)
	{
		SafeObtainSemaphoreShared(&RasterSemaphore);

		SetWait(Window);

		if(WindowMarker->FirstColumn == WindowMarker->Width)
		{
			WindowMarker->FirstLine++;

			WindowMarker->FirstColumn = 0;
		}

		if(WindowMarker->LastColumn == 0)
		{
			WindowMarker->LastLine--;

			WindowMarker->LastColumn = WindowMarker->Width;
		}

		if(WindowMarker->FirstLine <= WindowMarker->LastLine)
		{
			if(WindowMarker->FirstLine != WindowMarker->LastLine || WindowMarker->FirstColumn != WindowMarker->LastColumn)
			{
				if(WindowMarker->FirstLine == WindowMarker->LastLine)
				{
					if(Config->TerminalConfig->FontMode != FONT_STANDARD)
					{
						UBYTE	Buffer[256];
						LONG	Len,i;
						STRPTR	Dest,Source;
						LONG	Size;
						LONG	c;

						Source = &Raster[RasterWidth * WindowMarker->FirstLine + WindowMarker->FirstColumn];
						Size = WindowMarker->LastColumn - WindowMarker->FirstColumn;

						if(Append)
						{
							do
							{
								Len = MIN(Size,sizeof(Buffer));

								Size -= Len;

								for(i = 0, Dest = Buffer ; i < Len ; i++)
								{
									if(c = ISOConversion[*Source++])
										*Dest++ = c;
									else
										*Dest++ = ' ';
								}

								if(Dest > Buffer)
									AddClip(Buffer,(ULONG)Dest - (ULONG)&Buffer[0]);
							}
							while(Size > 0);
						}
						else
						{
							BOOL FirstWrite = TRUE;

							do
							{
								Len = MIN(Size,sizeof(Buffer));

								Size -= Len;

								for(i = 0, Dest = Buffer ; i < Len ; i++)
								{
									if(c = ISOConversion[*Source++])
										*Dest++ = c;
									else
										*Dest++ = ' ';
								}

								if(Dest > Buffer)
								{
									if(FirstWrite)
									{
										FirstWrite = FALSE;

										SaveClip(Buffer,(ULONG)Dest - (ULONG)&Buffer[0]);
									}
									else
										AddClip(Buffer,(ULONG)Dest - (ULONG)&Buffer[0]);
								}
							}
							while(Size > 0);
						}
					}
					else
					{
						if(Append)
							AddClip(&Raster[RasterWidth * WindowMarker->FirstLine + WindowMarker->FirstColumn],WindowMarker->LastColumn - WindowMarker->FirstColumn);
						else
							SaveClip(&Raster[RasterWidth * WindowMarker->FirstLine + WindowMarker->FirstColumn],WindowMarker->LastColumn - WindowMarker->FirstColumn);
					}
				}
				else
					ClipPage(WindowMarker,Append,Config->TerminalConfig->FontMode != FONT_STANDARD);
			}
		}

		ClrWait(Window);

		ReleaseSemaphore(&RasterSemaphore);

		DropMarker();
	}
}
