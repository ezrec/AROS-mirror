/*
**	Emulation.c
**
**	Terminal emulation (parsing and processing) routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Flag indicating whether the cursor has already been
	 * erased or not.
	 */

STATIC BOOL					CursorEnabled,
							CursorInvisible;

	/* Cursor handling data. */

STATIC WORD					LastCursorX = -1,
							LastCursorY = -1;

STATIC LONG					DestX,DestY,XSize,FontByColumn;

STATIC BOOL					CursorGhosted;

	/* Backup style. */

STATIC ULONG				StyleType = FS_NORMAL;

	/* Cursor backup data. */

STATIC struct CursorData	CursorBackup;
STATIC BOOL					CursorBackupValid;

	/* A couple of internally referenced variables. */

STATIC WORD					CharsInBuffer,
							ScanStep;
STATIC UBYTE				SaveBuffer[MAX_SCAN_SIZE + 1];
STATIC STRPTR				Arnie;

STATIC BOOL					PrintFormFeed,
							PrintFullScreen;

	/* UpdatePens(VOID):
	 *
	 *	Update colour and style.
	 */

VOID
UpdatePens()
{
	LONG ForePen,BackPen,Attrs,TextFlags,Mask = 0;

		/* Update the colour mask. */

	switch(Config->ScreenConfig->ColourMode)
	{
		case COLOUR_AMIGA:

			Mask = 3;

			break;

		case COLOUR_EIGHT:

			Mask = 7;

			break;

		case COLOUR_SIXTEEN:

			Mask = 15;

			break;

		case COLOUR_MONO:

			Mask = 1;

			break;
	}

	if(Mask > DepthMask)
		Mask = DepthMask;

		/* Convert the colours and the text attributes */

	ForePen	= PenTable[ForegroundPen];
	BackPen	= PenTable[BackgroundPen];
	Attrs	= TextAttributeTable[Attributes];

		/* Choose a sensible colour */

	if(ForePen > Mask)
	{
		if(BackPen > Mask)
		{
			if(BackPen <= ForePen)
			{
				ForePen = GetPenIndex(SafeTextPen);
				BackPen = 0;
			}
			else
			{
				ForePen = 0;
				BackPen = GetPenIndex(SafeTextPen);
			}
		}
		else
		{
			if(GetPenIndex(SafeTextPen) == BackPen)
				ForePen = 0;
			else
				ForePen = GetPenIndex(SafeTextPen);
		}
	}
	else
	{
		if(BackPen > Mask)
		{
			if(!ForePen)
				BackPen = GetPenIndex(SafeTextPen);
			else
				BackPen = 0;
		}
	}

		/* Take care of the text attributes */

	TextFlags = 0;

	if(Attrs & ATTR_UNDERLINE)
		TextFlags |= FSF_UNDERLINED;

	if(Attributes & ATTR_HIGHLIGHT)
	{
		if(Config->ScreenConfig->ColourMode == COLOUR_SIXTEEN)
			ForePen |= 8;
		else
			TextFlags |= FSF_BOLD;
	}

	if((Attributes & ATTR_BLINK) && (Config->TerminalConfig->EmulationMode == EMULATION_ANSIVT100))
	{
		switch(Config->ScreenConfig->ColourMode)
		{
			case COLOUR_AMIGA:

				ForePen = 3;

				break;

			case COLOUR_EIGHT:

				ForePen |= 8;

				break;

			case COLOUR_SIXTEEN:

				if(Screen && DepthMask > 15)
					ForePen |= 16;

				break;

			case COLOUR_MONO:

				ForePen = GetPenIndex(SafeTextPen);

				break;
		}
	}

		/* Make sure that monochrome text renders properly */

	if(Config->ScreenConfig->ColourMode == COLOUR_MONO)
	{
			/* Out of bounds? */

		if(ForePen > 1)
		{
				/* If it's #7 then it's white, else it's black */

			if(ForePen == 7)
				ForePen = GetPenIndex(SafeTextPen);
			else
				ForePen = 0;
		}

		if(BackPen > 1)
		{
			if(BackPen == 7)
				BackPen = GetPenIndex(SafeTextPen);
			else
				BackPen = 0;
		}

			/* Oops... the text should be readable */

		if(ForePen == BackPen)
		{
				/* Inverse video? */

			if(BackPen)
			{
				ForePen = 0;
				BackPen = GetPenIndex(SafeTextPen);
			}
			else
			{
				ForePen = GetPenIndex(SafeTextPen);
				BackPen = 0;
			}
		}
	}

	if(Attrs & ATTR_INVERSE)
	{
		LONG Help;

		Help	= ForePen;
		ForePen	= BackPen;
		BackPen	= Help;
	}

	if(TextFlags != StyleType)
	{
		SetSoftStyle(RPort,TextFlags,~0);

		StyleType = TextFlags;
	}

	FgPen = MappedPens[0][ForePen];
	BgPen = MappedPens[0][BackPen];

	if(FgPen != ReadAPen(RPort))
		SetAPen(RPort,FgPen);

	if(BgPen != ReadBPen(RPort))
		SetBPen(RPort,BgPen);
}

	/* GetFontWidth():
	 *
	 *	Get the font width of the current line.
	 */

LONG
GetFontWidth()
{
	if(RasterAttr[CursorY] == SCALE_ATTR_NORMAL)
	{
		if(CurrentCharWidth == SCALE_HALF)
			return((LONG)(TextFontWidth / 2));
		else
			return(TextFontWidth);
	}
	else
	{
		if(CurrentCharWidth == SCALE_HALF)
			return(TextFontWidth);
		else
			return((LONG)(TextFontWidth * 2));
	}
}

	/* RethinkRasterLimit():
	 *
	 *	Take care of the extreme left column position
	 *	permitted.
	 */

VOID
RethinkRasterLimit()
{
	LONG Y;

	if(CursorY > LastLine)
		Y = LastLine;
	else
	{
		if(CursorY < 0)
			Y = 0;
		else
			Y = CursorY;
	}

	if(RasterAttr[Y] == SCALE_ATTR_NORMAL)
	{
		if(CurrentCharWidth == SCALE_HALF)
			FontByColumn = (LastColumn + 1) * 2 - 1;
		else
			FontByColumn = LastColumn;
	}
	else
	{
		if(CurrentCharWidth == SCALE_HALF)
			FontByColumn = LastColumn;
		else
			FontByColumn = ((LastColumn + 1) / 2) - 1;
	}
}

	/* ScrollRegion(LONG Direction):
	 *
	 *	Scroll the current scroll region up or down.
	 */

VOID
ScrollRegion(LONG Direction)
{
	LONG RegionTop,RegionBottom,RegionLines;
	LONG Dir,MinY,MaxY;

	if(Direction < 0)
		Dir = -Direction;
	else
		Dir = Direction;

	if(RegionSet)
	{
		MinY 			= MUL_Y(Top);
		MaxY			= MUL_Y(Bottom + 1) - 1;

		RegionTop		= Top;
		RegionBottom	= Bottom + 1;
		RegionLines		= Bottom - Top + 1;
	}
	else
	{
		MinY			= 0;
		MaxY 			= MUL_Y(LastLine + 1) - 1;

		RegionTop		= 0;
		RegionBottom	= LastLine + 1;
		RegionLines		= LastLine + 1;
	}

	BackupRender();

	RasterScrollRegion(Direction,RegionTop,RegionBottom,RegionLines);

	if(Config->EmulationConfig->ScrollMode == SCROLL_JUMP)
	{
		if(Dir > RegionLines)
			ScrollLineRectFill(RPort,0,MinY,LastPixel,MaxY);
		else
		{
			if(Direction > 0)
				ScrollLineRaster(RPort,0,MUL_Y(Direction),0,MinY,LastPixel,MaxY,FALSE);
			else
				ScrollLineRaster(RPort,0,-MUL_Y(-Direction),0,MinY,LastPixel,MaxY,FALSE);
		}
	}
	else
	{
		if(Dir > RegionLines)
		{
			if(Direction > 0)
				Direction = RegionLines;
			else
				Direction = -RegionLines;
		}

		if(Direction > 0)
			ScrollLineRaster(RPort,0,MUL_Y(Direction),0,MinY,LastPixel,MaxY,TRUE);
		else
			ScrollLineRaster(RPort,0,-MUL_Y(-Direction),0,MinY,LastPixel,MaxY,TRUE);
	}

	BackupRender();
}

	/* LastChar(STRPTR Buffer):
	 *
	 *	Return the last character in a string.
	 */

STATIC UBYTE
LastChar(STRPTR Buffer)
{
	LONG Offset = 0;

	while(Buffer[Offset])
		Offset++;

	return(Buffer[Offset - 1]);
}

	/* ReadValue(STRPTR Buffer,BYTE *Value):
	 *
	 *	Parse a buffer for numbers and return a pointer
	 *	to the next buffer element to contain additional
	 *	information.
	 */

STATIC STRPTR
ReadValue(STRPTR Buffer,WORD *Value)
{
	while(*Buffer && *Buffer != ';' && (*Buffer < '0' || *Buffer > '9'))
		Buffer++;

	if(*Buffer)
	{
		*Value = 0;

		while(*Buffer >= '0' && *Buffer <= '9')
			*Value = (*Value * 10) + (*Buffer++ - '0');
	}
	else
		*Value = -1;

	if(*Buffer == ';' || *Buffer == ' ')
		return(Buffer + 1);
	else
		return(NULL);
}

	/* EmulationSerWrite(STRPTR String,LONG Length):
	 *
	 *	Write text to the serial line.
	 */

STATIC VOID
EmulationSerWrite(STRPTR String,LONG Length)
{
	if(FindTask(NULL) == SpecialQueue->SigTask)
		SerWrite(String,Length);
	else
	{
		struct DataMsg *Msg;

		if(Length == -1)
			Length = strlen(String);

		if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg) + Length + 1))
		{
			Msg->Type = DATAMSGTYPE_WRITE;
			Msg->Data = (STRPTR)(Msg + 1);
			Msg->Size = Length;

			CopyMem(String,Msg->Data,Length + 1);

			PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
		}
	}
}

	/* RethinkCursorPosition():
	 *
	 *	Calculate the new cursor position.
	 */

STATIC VOID
RethinkCursorPosition(VOID)
{
	if(CursorY != LastCursorY || CursorX != LastCursorX)
	{
		STATIC LONG X,Y;

		if(CursorY != LastCursorY)
		{
			if(CursorY > LastLine)
				Y = LastLine;
			else
			{
				if(CursorY < 0)
					Y = 0;
				else
					Y = CursorY;
			}

			if(RasterAttr[Y] == SCALE_ATTR_NORMAL)
			{
				if(CurrentCharWidth == SCALE_HALF)
					FontByColumn = ((LastColumn + 1) * 2) - 1;
				else
					FontByColumn = LastColumn;
			}
			else
			{
				if(CurrentCharWidth == SCALE_HALF)
					FontByColumn = LastColumn;
				else
					FontByColumn = ((LastColumn + 1) / 2) - 1;
			}

			DestY = MUL_Y(Y);

			LastCursorY = CursorY;
		}

		LastCursorX = CursorX;

		if(CursorX > FontByColumn)
			X = FontByColumn;
		else
		{
			if(CursorX < 0)
				X = 0;
			else
				X = CursorX;
		}

		if(CurrentCharWidth == SCALE_NORMAL)
		{
			if(RasterAttr[Y] == SCALE_ATTR_NORMAL)
			{
					/* Normal width */

				DestX = MUL_X(X);
				XSize = TextFontWidth;
			}
			else
			{
					/* Double width */

				DestX = MUL_X(X) * 2;
				XSize = TextFontWidth * 2;
			}
		}
		else
		{
			if(RasterAttr[CursorY] == SCALE_ATTR_NORMAL)
			{
					/* Half width */

				DestX = MUL_X(X) / 2;
				XSize = TextFontWidth / 2;
			}
			else
			{
					/* Twice the half width */

				DestX = MUL_X(X);
				XSize = TextFontWidth;
			}
		}
	}
}

	/* ToggleCursor():
	 *
	 *	(Re)draw the cursor image.
	 */

STATIC VOID
ToggleCursor(VOID)
{
	if(Config->TerminalConfig->EmulationMode != EMULATION_EXTERNAL)
	{
		ULONG OldAPen,OldBPen,OldDrMd,OldMask;
		LONG Left,Top;

		OldAPen	= ReadAPen(RPort);
		OldBPen	= ReadBPen(RPort);
		OldDrMd	= ReadDrMd(RPort);
		OldMask = GetMask(RPort);

		Left	= WindowLeft + DestX;
		Top		= WindowTop + DestY;

		SetPens(RPort,DepthMask,OldBPen,JAM1 | COMPLEMENT);
		SetMask(RPort,DepthMask);
		FillBox(RPort,Left,Top,XSize,TextFontHeight);
		SetPens(RPort,OldAPen,OldBPen,OldDrMd);
		SetMask(RPort,OldMask);
	}
}

	/* RedrawCursor():
	 *
	 *	Change the appearance of the cursor.
	 */

STATIC VOID
RedrawCursor(VOID)
{
	ObtainSemaphore(&TerminalSemaphore);

	RethinkCursorPosition();

	if(CursorGhosted)
	{
		SetAfPt(RPort,(UWORD *)&Crosshatch,1);

		ToggleCursor();

		SetAfPt(RPort,NULL,0);
	}
	else
		ToggleCursor();

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* DoCancel():
	 *
	 *	Cancel any currently scanned sequence.
	 */

BOOL
DoCancel()
{
	InSequence		= FALSE;
	CharsInBuffer	= 0;
	ScanStep		= 0;

	return(FALSE);
}

	/* ParseCode(LONG c):
	 *
	 *	Input:	A character to be passed through the ANSI code
	 *		parser.
	 *
	 *	Output:	FALSE if input characters did form a valid ANSI
	 *		control sequence or if input characters did not
	 *		form an ANSI control sequence at all.
	 *
	 *		TRUE if input characters did possibly introduce
	 *		a valid ANSI control sequence.
	 */

BOOL
ParseCode(LONG c)
{
		/* ScanStep = 0:	This is the first character
		 *					to introduce a control sequence.
		 */

	if(!ScanStep)
	{
		LONG i;

			/* Scan all available codes and try to find
			 * a match.
			 */

		for(i = 0 ; i < NumCodes ; i++)
		{
				/* This character may introduce a
				 * control sequence.
				 */

			if(ANSICode[i].FirstChar == c)
			{
					/* If this is a single
					 * character control sequence
					 * call the appropriate function
					 * and exit immediately.
					 */

				if(ANSICode[i].ExactSize == 1)
				{
					if(Config->TerminalConfig->EmulationMode != EMULATION_ATOMIC)
					{
						SaveBuffer[CharsInBuffer++] = c;
						SaveBuffer[CharsInBuffer  ] = 0;

						(*ANSICode[i].Func)(SaveBuffer);
					}

					CharsInBuffer = ScanStep = 0;

					return(FALSE);
				}
				else
				{
						/* The length of this control
						 * sequence is greater than
						 * a single character. Save
						 * the input character and
						 * return.
						 */

					ScanStep = i;

					SaveBuffer[CharsInBuffer++] = c;

						/* Where to stop. */

					Arnie = ANSICode[i].Terminator;

					return(TRUE);
				}
			}
		}
	}
	else
	{
		if(CharsInBuffer < MAX_SCAN_SIZE)
		{
			if(Arnie)
			{
				LONG i;

					/* Scan the remaining codes for a match. */

				for(i = ScanStep ; i < NumCodes ; i++)
				{
						/* This sequence begins with the
						 * same character the parser was
						 * initialized with, so let's take
						 * a look at it.
						 */

					if(ANSICode[i].FirstChar == SaveBuffer[0])
					{
							/* This character is supposed to
							 * terminate the sequence, so exit.
							 */

						if(Arnie[c])
						{
							if(Config->TerminalConfig->EmulationMode != EMULATION_ATOMIC)
							{
								SaveBuffer[CharsInBuffer++] = c;
								SaveBuffer[CharsInBuffer  ] = 0;

								(*ANSICode[i].Func)(SaveBuffer);
							}

							CharsInBuffer = ScanStep = 0;

							Arnie = NULL;

							return(FALSE);
						}
						else
						{
								/* If this character is part of
								 * a legal sequence store it
								 * and return.
								 */

							if(ANSICode[i].Match[c])
							{
								ScanStep = i;

								SaveBuffer[CharsInBuffer++] = c;

								return(TRUE);
							}
						}
					}
				}
			}
			else
			{
				LONG i;

				for(i = ScanStep ; i < NumCodes ; i++)
				{
						/* This sequence begins with the
						 * same character the parser was
						 * initialized with, so let's take
						 * a look at it.
						 */

					if(ANSICode[i].FirstChar == SaveBuffer[0])
					{
							/* This character is supposed to
							 * terminate the sequence, so exit.
							 */

						if(ANSICode[i].LastChar == c || (!ANSICode[i].LastChar && CharsInBuffer == 2 && ANSICode[i].ExactSize == 3))	/* Special case for VT52 */
						{
							if(Config->TerminalConfig->EmulationMode != EMULATION_ATOMIC)
							{
								SaveBuffer[CharsInBuffer++] = c;
								SaveBuffer[CharsInBuffer  ] = 0;

								(*ANSICode[i].Func)(SaveBuffer);
							}

							CharsInBuffer = ScanStep = 0;

							return(FALSE);
						}
						else
						{
								/* If this character is part of
								 * a legal sequence store it
								 * and return.
								 */

							if(ANSICode[i].Match[c])
							{
								ScanStep = i;

								SaveBuffer[CharsInBuffer++] = c;

								return(TRUE);
							}
						}
					}
				}
			}
		}
	}

		/* Return failure. */

	CharsInBuffer = ScanStep = 0;

	Arnie = NULL;

	return(FALSE);
}

	/* NormalCursor():
	 *
	 *	Enable normal (filled) cursor image.
	 */

VOID
NormalCursor()
{
	ObtainSemaphore(&TerminalSemaphore);

	if(CursorGhosted)
	{
		if(CursorEnabled && !CursorInvisible)
		{
			SetAfPt(RPort,(UWORD *)&Crosshatch,1);

			ToggleCursor();

			SetAfPt(RPort,NULL,0);

			ToggleCursor();
		}

		CursorGhosted = FALSE;
	}

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* GhostCursor():
	 *
	 *	Enable ghosted (checkered) cursor image.
	 */

VOID
GhostCursor()
{
	ObtainSemaphore(&TerminalSemaphore);

	if(!CursorGhosted)
	{
		if(CursorEnabled && !CursorInvisible)
		{
			ToggleCursor();

			SetAfPt(RPort,(UWORD *)&Crosshatch,1);

			ToggleCursor();

			SetAfPt(RPort,NULL,0);
		}

		CursorGhosted = TRUE;
	}

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* RepositionCursor():
	 *
	 *	Redraw the cursor at the new position.
	 */

VOID
RepositionCursor()
{
	ObtainSemaphore(&TerminalSemaphore);

	RethinkCursorPosition();

	Move(RPort,WindowLeft + DestX,WindowTop + DestY + TextFontBase);

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* ClearCursor():
	 *
	 *	Clear the cursor image.
	 */

VOID
ClearCursor()
{
	ObtainSemaphore(&TerminalSemaphore);

	if(CursorEnabled && !CursorInvisible)
	{
		RedrawCursor();

		CursorEnabled = FALSE;
	}

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* DrawCursor():
	 *
	 *	Explicitely (re-)draw the cursor image.
	 */

VOID
DrawCursor()
{
	ObtainSemaphore(&TerminalSemaphore);

	if(!CursorEnabled && !CursorInvisible)
	{
		RedrawCursor();

		CursorEnabled = TRUE;
	}

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* BackupRender():
	 *
	 *	Save current draw modes, pen and position or restore
	 *	the data.
	 */

VOID
BackupRender()
{
	STATIC BOOL		Called;
	STATIC ULONG	DrMd,FgPen,BgPen;
	STATIC UWORD	OldX,OldY;
	STATIC UBYTE	Style;

	if(!Called)
	{
		DrMd	= ReadDrMd(RPort);
		FgPen	= ReadAPen(RPort);
		BgPen	= ReadBPen(RPort);

		OldX	= RPort->cp_x - WindowLeft;
		OldY	= RPort->cp_y - WindowTop;

		Style	= StyleType;

		Called	= TRUE;
	}
	else
	{
		SetPens(RPort,FgPen,BgPen,DrMd);

		Move(RPort,OldX + WindowLeft,OldY + WindowTop);

		if(Style != StyleType)
		{
			SetSoftStyle(RPort,Style,~0);

			StyleType = Style;
		}

		Called = FALSE;
	}
}

	/* ShiftChar(LONG Size):
	 *
	 *	Simulate character insertion at the current cursor
	 *	position by shifting the whole line Size times eight pixels
	 *	to the right.
	 */

VOID
ShiftChar(LONG Size)
{
	LONG DeltaX,MinX,MinY;

	MinY = MUL_Y(CursorY);

	if(CurrentCharWidth == SCALE_NORMAL)
	{
		if(RasterAttr[CursorY] == SCALE_ATTR_NORMAL)
		{
			DeltaX	= MUL_X(Size);
			MinX	= MUL_X(CursorX);
		}
		else
		{
			DeltaX	= MUL_X(Size) * 2;
			MinX	= MUL_X(CursorX) * 2;
		}
	}
	else
	{
		if(RasterAttr[CursorY] == SCALE_ATTR_NORMAL)
		{
			DeltaX	= MUL_X(Size) / 2;
			MinX	= MUL_X(CursorX) / 2;
		}
		else
		{
			DeltaX	= MUL_X(Size);
			MinX	= MUL_X(CursorX);
		}
	}

	if(MinX < WindowWidth)
	{
		BackupRender();

		ScrollLineRasterNoTabChange(RPort,-DeltaX,0,MinX,MinY,LastPixel,MinY + TextFontHeight - 1,FALSE);

		BackupRender();
	}
}

	/* Ignore():
	 *
	 *	Do nothing, return immediately.
	 */

VOID
Ignore()
{
}

	/* ScrollDown(STRPTR Buffer):
	 *
	 *	Scroll the current region down.
	 */

VOID
ScrollDown(STRPTR Buffer)
{
	WORD Value;

	ReadValue(Buffer,&Value);

	if(Value < 1)
		Value = 1;

	ScrollRegion(-Value);
}

	/* ScrollUp(STRPTR Buffer):
	 *
	 *	Scroll the current region up.
	 */

VOID
ScrollUp(STRPTR Buffer)
{
	WORD Value;

	ReadValue(Buffer,&Value);

	if(Value < 1)
		Value = 1;

	ScrollRegion(Value);
}

	/* CursorScrollDown():
	 *
	 *	Move cursor down and scroll region if necessary.
	 */

VOID
CursorScrollDown()
{
	DownLine();

	RepositionCursor();
}

VOID
DownLine()
{
	UBYTE InRegion;
	LONG Hit;

	InRegion = TRUE;
	Hit      = LastLine;

	if(RegionSet)
	{
		if(CursorY <= Bottom)
			Hit = Bottom;
		else
			InRegion = FALSE;
	}

	if(CursorY == Hit)
	{
		if(InRegion)
			ScrollRegion(1);
	}
	else
	{
		CursorY++;

		if(CursorY > LastLine)
			CursorY = LastLine;

		ConFontScaleUpdate();
	}
}

	/* CursorScrollUp():
	 *
	 *	Move cursor up and scroll region if necessary.
	 */

VOID
CursorScrollUp()
{
	BOOL InRegion;
	LONG Hit;

	InRegion	= TRUE;
	Hit			= 0;

	if(RegionSet)
	{
		if(CursorY >= Top)
			Hit = Top;
		else
			InRegion = FALSE;
	}

	if(CursorY == Hit)
	{
		if(InRegion)
			ScrollRegion(-1);
	}
	else
	{
		if(--CursorY < 0)
			CursorY = 0;

		ConFontScaleUpdate();
	}

	RepositionCursor();
}

	/* NextLine():
	 *
	 *	Do something like CR+LF.
	 */

VOID
NextLine()
{
	CursorX = 0;

	DownLine();

	RepositionCursor();
}

	/* SaveCursor():
	 *
	 *	Save cursor position and rendering attributes.
	 */

VOID
SaveCursor()
{
	CursorBackup.Charset		= Charset;
	CursorBackup.Attributes		= Attributes;
	CursorBackup.CursorX		= CursorX;
	CursorBackup.CursorY		= CursorY;
	CursorBackup.Style			= StyleType;
	CursorBackup.FgPen			= ForegroundPen;
	CursorBackup.BgPen			= BackgroundPen;
	CursorBackup.CurrentFont	= CurrentFont;
	CursorBackup.CharMode[0]	= CharMode[0];
	CursorBackup.CharMode[1]	= CharMode[1];

	CursorBackupValid = TRUE;
}

	/* FontStuff(STRPTR Buffer):
	 *
	 *	Set the drawing font (standard characters/line).
	 */

VOID
FontStuff(STRPTR Buffer)
{
	BOOL Changed;

	Changed = FALSE;

	if(Buffer[0] == '(')
	{
		switch(LastChar(Buffer))
		{
			case 'A':
			case 'B':

				if(CharMode[0] != TABLE_ASCII && !Charset)
					Changed = TRUE;

				CharMode[0] = TABLE_ASCII;

				break;

			case '0':

				if(CharMode[0] != TABLE_GFX && !Charset)
					Changed = TRUE;

				CharMode[0] = TABLE_GFX;

				break;
		}
	}

	if(Buffer[0] == ')')
	{
		switch(LastChar(Buffer))
		{
			case 'A':
			case 'B':

				if(CharMode[1] != TABLE_ASCII && Charset == 1)
					Changed = TRUE;

				CharMode[1] = TABLE_ASCII;

				break;

			case '0':

				if(CharMode[1] != TABLE_GFX && Charset == 1)
					Changed = TRUE;

				CharMode[1] = TABLE_GFX;

				break;
		}
	}

	if(Changed)
	{
		BackupRender();

		if(Charset)
			DoShiftIn();
		else
			DoShiftOut();

		BackupRender();
	}
}

	/* LoadCursor():
	 *
	 *	Load cursor position and rendering attributes.
	 */

VOID
LoadCursor()
{
	Charset		= CursorBackup.Charset;

	CharMode[0]	= CursorBackup.CharMode[0];
	CharMode[1]	= CursorBackup.CharMode[1];

	if(CurrentFont != CursorBackup.CurrentFont)
	{
		CurrentFont = CursorBackup.CurrentFont;

		SetFont(RPort,CurrentFont);

		ConOutputUpdate();
	}

	ForegroundPen	= CursorBackup.FgPen;
	BackgroundPen	= CursorBackup.BgPen;
	Attributes		= CursorBackup.Attributes;
	CursorX			= CursorBackup.CursorX;
	CursorY			= CursorBackup.CursorY;

	UpdatePens();

	ConFontScaleUpdate();

	RepositionCursor();
}

	/* ScaleFont(STRPTR Buffer):
	 *
	 *	Select a new font scale.
	 */

VOID
ScaleFont(STRPTR Buffer)
{
	if(!Config->EmulationConfig->FontLocked)
	{
		LONG NewScale,Scale;

		Scale = RasterAttr[CursorY];

		NewScale = Scale;

		switch(LastChar(Buffer))
		{
			case '3':

				NewScale = SCALE_ATTR_TOP2X;

				break;

			case '4':

				NewScale = SCALE_ATTR_BOT2X;

				break;

			case '5':

				NewScale = SCALE_NORMAL;

				break;

			case '6':

				NewScale = SCALE_ATTR_2X;

				break;
		}

		if(Scale != NewScale)
		{
			LONG RightMargin,CursorXSave;
			UBYTE *RasterPtr;

			RasterPtr	= &Raster[CursorY * RasterWidth];
			RightMargin	= LastColumn + 1;
			CursorXSave	= CursorX;

			if(NewScale != SCALE_ATTR_NORMAL)
			{
					/* Twice the normal character width */

				if(CurrentCharWidth == SCALE_NORMAL)
					RightMargin /= 2;
			}
			else
			{
					/* Half the normal character width */

				if(CurrentCharWidth != SCALE_NORMAL)
					RightMargin *= 2;
			}

			RasterAttr[CursorY] = NewScale;

			RethinkRasterLimit();

			ConFontScaleUpdate();

			if(FontScalingRequired)
			{
				CursorX = 0;

				PrintScaled(RasterPtr,RightMargin,NewScale);
			}
			else
				PlaceText(RPort,WindowLeft,WindowTop + MUL_Y(CursorY),RasterPtr,RightMargin);

			if(CursorXSave >= RightMargin)
				CursorX = RightMargin - 1;
			else
				CursorX = CursorXSave;
		}

		RepositionCursor();
	}
}

	/* AlignmentTest():
	 *
	 *	Perform screen alignment test, fill the screen with `E's.
	 */

VOID
AlignmentTest()
{
	STRPTR Buffer;

	if(Buffer = AllocVecPooled(LastColumn + 1,MEMF_ANY))
	{
		LONG i;

		memset(Buffer,'E',LastColumn + 1);

		EraseScreen("2");

		if(FontScalingRequired)
		{
			for(i = 0 ; i <= LastLine ; i++)
			{
				CursorX = 0;
				CursorY = i;

				RasterAttr[i] = SCALE_ATTR_NORMAL;

				RasterPutString(Buffer,LastColumn + 1);
				ScrollLinePutString(LastColumn + 1);

				Move(RPort,WindowLeft,WindowTop + MUL_Y(i) + TextFontBase);
				PrintScaled(Buffer,LastColumn + 1,SCALE_ATTR_NORMAL);
			}
		}
		else
		{
			for(i = 0 ; i <= LastLine ; i++)
			{
				CursorX = 0;
				CursorY = i;

				RasterPutString(Buffer,LastColumn + 1);
				ScrollLinePutString(LastColumn + 1);

				PlaceText(RPort,WindowLeft,WindowTop + MUL_Y(i),Buffer,LastColumn + 1);
			}
		}

		CursorX = CursorY = 0;

		RethinkRasterLimit();

		RepositionCursor();

		FreeVecPooled(Buffer);

		ConFontScaleUpdate();
	}
}

	/* SetTab():
	 *
	 *	Set a tabulator stop at the current position.
	 */

VOID
SetTab()
{
	if(CursorX < TabStopMax)
		TabStops[CursorX] = TRUE;
}

	/* RequestTerminal(STRPTR Buffer):
	 *
	 *	Return the current terminal position.
	 */

VOID
RequestTerminal(STRPTR Buffer)
{
	STRPTR String;

	String = NULL;

	DB(kprintf("|%s|\n",Buffer));

	switch(Buffer[0])
	{
			/* Make ourselves known as a VT200
			 * terminal.
			 */

		case '[':

			if(Buffer[1] != '>')
			{
				switch(Config->EmulationConfig->TerminalType)
				{
					case TERMINAL_VT100:
					case TERMINAL_VT101:

						String = "\033[?1;0c";	/* Vanilla VT100 */
						break;

					case TERMINAL_VT102:

						String = "\033[?6c";
						break;

					case TERMINAL_VT200:

						String = "\233?62;1;2;6;7;8;9c";
						break;
				}
			}
			else
				String = "\233>1;10;0c";	/* Let's hope this command will never get called when responding in VT10x mode */

			break;

			/* This is an old VT52 status request type,
			 * we will identify ourselves as a VT100
			 * terminal
			 */

		case 'Z':

			switch(Config->EmulationConfig->TerminalType)
			{
				case TERMINAL_VT100:

					String = "\033/Z";
					break;

				case TERMINAL_VT101:

					String = "\033[?1;0c";
					break;

				case TERMINAL_VT102:

					String = "\033[?6c";
					break;

				case TERMINAL_VT200:

					String = "\233?62;1;2;6;7;8;9c";
					break;
			}

			break;
	}

	DB(kprintf("out |%s|\n",String ? String+1 : "«NULL»"));

	if(String)
		EmulationSerWrite(String,-1);
}

	/* RequestTerminalParams(STRPTR Buffer):
	 *
	 *	Well, actually I don't really know what this routine actually
	 *	does. I copied the parameters from the vttest program.
	 */

VOID
RequestTerminalParams(STRPTR Buffer)
{
	STATIC WORD ParityTable[][2] =
	{
		{ PARITY_NONE,	1, },
		{ PARITY_ODD,		4, },
		{ PARITY_EVEN,	5, },
		{ -1,				1 },
	};

	STATIC WORD SpeedTable[][2] =
	{
		{ 50,		0, },
		{ 75,		8, },
		{ 110,	16, },
		{ 132,	24, },
		{ 150,	32, },
		{ 200,	40, },
		{ 300,	48, },
		{ 600,	56, },
		{ 1200,	64, },
		{ 1800,	72, },
		{ 2000,	80, },
		{ 2400,	88, },
		{ 3600,	96, },
		{ 4800,	104, },
		{ 9600,	112, },
		{ 19200,	120, },
		{ -1 },
	};

	UBYTE LocalBuffer[40];
	STRPTR String;
	WORD Value;
	LONG Parity;
	LONG Bits;
	LONG Speed = 0;
	LONG Delta,d;
	LONG i;

	DB(kprintf("|%s|\n",Buffer));

	ReadValue(Buffer,&Value);

	switch(Value)
	{
		case 1:

			String = "\033[3x";
			break;

		default:

			for(i = 0 ; ; i++)
			{
				if(ParityTable[i][0] == -1 || ParityTable[i][0] == Config->SerialConfig->Parity)
				{
					Parity = ParityTable[i][1];
					break;
				}
			}

			if(Config->SerialConfig->BitsPerChar == 8)
				Bits = 1;
			else
				Bits = 2;

			Delta = -1;

			for(i = 0 ; SpeedTable[i][0] != -1 ; i++)
			{
				if((d = SpeedTable[i][0] - Config->SerialConfig->BaudRate) < 0)
					d = (-d);

				if(Delta == -1 || d < Delta)
				{
					Speed = SpeedTable[i][1];
					Delta = d;
				}
			}

			LimitedSPrintf(sizeof(LocalBuffer),String = LocalBuffer,"\033[2;%ld;%ld;%ld;%ld;1;0x",Parity,Bits,Speed,Speed);
			break;
	}

	DB(kprintf("out |%s|\n",String ? String+1 : "«NULL»"));

	if(String)
		EmulationSerWrite(String,-1);
}

	/* SoftReset():
	 *
	 *	Plain and simple: reset the text rendering colours, style and the
	 *	font being used. This works similar to the Reset() call which
	 *	also clears the screen.
	 */

VOID
SoftReset()
{
	ObtainSemaphore(&TerminalSemaphore);

		/* Are we running on an external emulation? */

	if(XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
	{
		XEmulatorResetTextStyles(XEM_IO);
		XEmulatorResetCharset(XEM_IO);
	}
	else
	{
		if(!Config->EmulationConfig->FontLocked)
			CurrentCharWidth = SCALE_NORMAL;

			/* Reset the text rendering colours. */

		if(!Config->EmulationConfig->LockColour)
		{
			if(Config->ScreenConfig->ColourMode == COLOUR_SIXTEEN)	/* Special case */
				ForegroundPen = 7;
			else
				ForegroundPen = GetPenIndex(SafeTextPen);

			BackgroundPen = 0;
		}

		if(StyleType != FS_NORMAL && !Config->EmulationConfig->LockStyle)
			StyleType = FS_NORMAL;

		ConFontScaleUpdate();

		UpdatePens();

		CurrentFont = TextFont;

		SetFont(RPort,CurrentFont);

		ConOutputUpdate();

		CursorBackupValid = FALSE;

		VT52_Mode = FALSE;
	}

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* Reset():
	 *
	 *	Reset terminal to initial state.
	 */

VOID
Reset()
{
	LONG MaxColumns,MaxLines,Columns,Lines,i;

	ObtainSemaphore(&TerminalSemaphore);

	CursorEnabled = CursorInvisible = FALSE;

	if(Window->Flags & WFLG_WINDOWACTIVE)
		CursorGhosted = FALSE;
	else
		CursorGhosted = TRUE;

		/* Determine window inner dimensions and top/left edge offsets. */

	UpdateTerminalLimits();

	MaxColumns	= WindowWidth / TextFontWidth;
	MaxLines	= WindowHeight / TextFontHeight;

		/* Set up the new screen width. */

	if(Config->TerminalConfig->NumColumns < 20 || Config->TerminalConfig->AutoSize)
		Columns = MaxColumns;
	else
		Columns = Config->TerminalConfig->NumColumns;

		/* Set up the new screen height. */

	if(Config->TerminalConfig->NumLines < 20 || Config->TerminalConfig->AutoSize)
		Lines = MaxLines;
	else
		Lines = Config->TerminalConfig->NumLines;

		/* More columns than we will be able to display? */

	if(Columns > MaxColumns)
		Columns = MaxColumns;

		/* More lines than we will be able to display? */

	if(Lines > MaxLines)
		Lines = MaxLines;

		/* Set up the central data. */

	LastColumn	= Columns - 1;
	LastLine	= Lines - 1;
	LastPixel	= MUL_X(Columns) - 1;

	for(i = 0 ; i <= LastLine ; i++)
		RasterAttr[i] = SCALE_ATTR_NORMAL;

	memset(TabStops,FALSE,TabStopMax);

	for(i = 8 ; i < TabStopMax ; i += 8)
		TabStops[i] = TRUE;

	CharMode[0] = TABLE_ASCII;
	CharMode[1] = TABLE_GFX;

	Charset = 0;

	SetAPen(RPort,MappedPens[0][PenTable[0]]);

	SetMask(RPort,DepthMask);

	FillBox(RPort,WindowLeft,WindowTop,WindowWidth,WindowHeight);

	ScrollLineEraseScreen(2);

	RasterEraseScreen(2);

	if(!Config->EmulationConfig->LockColour)
	{
		if(Config->ScreenConfig->ColourMode == COLOUR_SIXTEEN)	/* Special case */
			ForegroundPen = 7;
		else
			ForegroundPen = GetPenIndex(SafeTextPen);

		BackgroundPen = 0;
	}

	if(StyleType != FS_NORMAL && !Config->EmulationConfig->LockStyle)
		StyleType = FS_NORMAL;

	UpdatePens();

	CurrentFont = TextFont;

	SetFont(RPort,CurrentFont);

	ConOutputUpdate();

	UseRegion = FALSE;
	RegionSet = FALSE;

	DB(kprintf("scroll region turned off\n"));

	if(!Config->EmulationConfig->CursorLocked)
		Config->EmulationConfig->CursorMode = KEYMODE_STANDARD;

	if(!Config->EmulationConfig->KeysLocked)
		Config->EmulationConfig->NumericMode = KEYMODE_STANDARD;

	Config->EmulationConfig->NewLineMode	= FALSE;
	Config->EmulationConfig->InsertMode		= FALSE;
	Config->EmulationConfig->ScrollMode		= SCROLL_JUMP;

	if(!Config->EmulationConfig->LockWrapping)
		Config->EmulationConfig->LineWrap = TRUE;

	if(!Config->EmulationConfig->FontLocked)
		CurrentCharWidth = SCALE_NORMAL;

	if(!Config->EmulationConfig->LockStyle)
		Attributes = 0;

	VT52_Mode = FALSE;

	Top			= 0;
	Bottom		= LastLine;
	CursorX		= 0;
	CursorY		= 0;

	CursorBackup.Charset		= Charset;
	CursorBackup.Attributes		= Attributes;
	CursorBackup.CursorX		= CursorX;
	CursorBackup.CursorY		= CursorY;
	CursorBackup.Style			= StyleType;
	CursorBackup.FgPen			= ForegroundPen;
	CursorBackup.BgPen			= BackgroundPen;
	CursorBackup.CurrentFont	= CurrentFont;
	CursorBackup.CharMode[0]	= CharMode[0];
	CursorBackup.CharMode[1]	= CharMode[1];

	CursorBackupValid = FALSE;

	ConFontScaleUpdate();

	RepositionCursor();

	ReleaseSemaphore(&TerminalSemaphore);
}

	/* PrinterController(STRPTR Buffer):
	 *
	 *	Controls various screen dump and capture functions.
	 */

VOID
PrinterController(STRPTR Buffer)
{
	if(Config->EmulationConfig->PrinterEnabled)
	{
		switch(Buffer[1])
		{
			case 'i':	/* Print screen */
			case '0':

				if(PrintFullScreen)
					PrintRegion(0,LastLine + 1,PrintFormFeed);
				else
					PrintRegion(Top,Bottom + 1,PrintFormFeed);

				break;

			case '5':	/* Turn on printer controller mode */

				OpenPrinterCapture(TRUE);
				break;

			case '4':	/* Turn off printer controller mode */

				ClosePrinterCapture(FALSE);
				break;

			case '?':

				if(Buffer[2] == '1')	/* Print current line */
					PrintRegion(CursorY,CursorY + 1,FALSE);

				if(Buffer[2] == '5')	/* Turn on auto print mode */
					OpenPrinterCapture(FALSE);

				if(Buffer[2] == '4')	/* Turn off auto print mode */
					ClosePrinterCapture(FALSE);

				break;
		}
	}
}

	/* RequestInformation(STRPTR Buffer):
	 *
	 *	Request miscellaneous information (state & cursor position).
	 */

VOID
RequestInformation(STRPTR Buffer)
{
	UBYTE LocalBuffer[40];
	WORD Value;

	ReadValue(Buffer,&Value);

	switch(Value)
	{
			/* Terminal status report, return code
			 * for `no malfunction'.
			 */

		case 5:

			EmulationSerWrite("\033[0n",-1);
			break;

			/* The origin is placed at 0/0 and the first
			 * cursor position is 1/1. We'll have to add
			 * 1 to our internal positions since our
			 * universe has been shifted one field to the
			 * left top corner.
			 */

		case 6:

			LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"\033[%ld;%ldR",CursorY + 1,CursorX + 1);

			EmulationSerWrite(LocalBuffer,-1);

			break;

			/* A VT200 command: request printer status.
			 * We will return `the printer is ready' in
			 * case the printer control commands are
			 * enabled, else return `no printer connected'.
			 */

		case 15:

			if(Config->EmulationConfig->PrinterEnabled)
				EmulationSerWrite("\033[?10n",-1);
			else
				EmulationSerWrite("\033[?11n",-1);

			break;

			/* VT200 command: request user defined
			 * key status. We will return `user
			 * defined keys are locked'.
			 */

		case 25:

			EmulationSerWrite("\033[?21n",-1);
			break;

			/* Another VT200 command: request
			 * keyboard language. We will return
			 * `keyboard language unknown'.
			 */

		case 26:

			EmulationSerWrite("\033[?27;0n",-1);
			break;
	}
}

	/* SetSomething(STRPTR Buffer):
	 *
	 *	Set a terminal option.
	 */

VOID
SetSomething(STRPTR Buffer)
{
	BOOL TurnOn;
	WORD Value;
	LONG Last;

	ReadValue(Buffer,&Value);
	Last = LastChar(Buffer);

	if(Buffer[1] == '?')
	{
		switch(Value)
		{
				/* Set cursor keys applications mode. */

			case 1:

					/* DECCKM */

				if(!Config->EmulationConfig->CursorLocked)
				{
					if(Last == 'h')
						Config->EmulationConfig->CursorMode = KEYMODE_APPLICATION;

					if(Last == 'l')
						Config->EmulationConfig->CursorMode = KEYMODE_STANDARD;
				}

				break;

				/* ANSI/VT52 mode */

			case 2:

					/* DECANM */

				if(Last != 'h')
					VT52_Mode = TRUE;

				break;

				/* Set line length (132 or 80). */

			case 3:

					/* DECCOLM */

				if(!Config->EmulationConfig->FontLocked)
				{
					if(CursorEnabled)
					{
						ClearCursor();

						TurnOn = TRUE;
					}
					else
						TurnOn = FALSE;

					if(Last == 'h')
					{
						if(CurrentCharWidth != SCALE_HALF)
						{
							CursorX = CursorY = 0;

							RepositionCursor();

							BackupRender();

							SetAPen(RPort,MappedPens[0][PenTable[0]]);

							ScrollLineRectFillNoTabChange(RPort,0,0,LastPixel,MUL_Y(LastLine + 1) - 1);

							ScrollLineEraseScreen(2);

							RasterEraseScreen(2);

							SaveConfig(Config,PrivateConfig);

							CurrentCharWidth = SCALE_HALF;

							BackupRender();

							ScreenSizeStuff();
						}
					}

					if(Last == 'l')
					{
						if(CurrentCharWidth != SCALE_NORMAL)
						{
							CursorX = CursorY = 0;

							RepositionCursor();

							BackupRender();

							SetAPen(RPort,MappedPens[0][PenTable[0]]);

							ScrollLineRectFillNoTabChange(RPort,0,0,LastPixel,MUL_Y(LastLine + 1) - 1);

							ScrollLineEraseScreen(2);

							RasterEraseScreen(2);

							SaveConfig(Config,PrivateConfig);

							CurrentCharWidth = SCALE_NORMAL;

							BackupRender();

							ScreenSizeStuff();
						}
					}

					if(TurnOn)
						DrawCursor();
					else
						ClearCursor();
				}

				break;

				/* Set scroll mode (jump or smooth). */

			case 4:

					/* DECSCLM */

				if(Last == 'h')
					Config->EmulationConfig->ScrollMode = SCROLL_SMOOTH;

				if(Last == 'l')
					Config->EmulationConfig->ScrollMode = SCROLL_JUMP;

				break;

				/* Reverse/normal screen. */

			case 5:

					/* DECSCNM */

				break;

				/* Turn region on or off. */

			case 6:

				if(Last == 'h')
					UseRegion = TRUE;

				if(Last == 'l')
					UseRegion = FALSE;

				DB(kprintf("scroll region = %ld\n",UseRegion));

				break;

				/* Turn character wrapping on or off. */

			case 7:

					/* DECAWM */

				if(!Config->EmulationConfig->LockWrapping)
				{
					if(Last == 'h')
						Config->EmulationConfig->LineWrap = TRUE;

					if(Last == 'l')
						Config->EmulationConfig->LineWrap = FALSE;
				}

				break;

				/* Turn auto repeat on or off. */

			case 8:

					/* DECARM */

				break;

				/* Set 240/480 line mode. */

			case 9:

				break;

				/* Print form feed after `print screen command'. */

			case 18:

					/* DECPFF */

				if(Last == 'h')
					PrintFormFeed = TRUE;

				if(Last == 'l')
					PrintFormFeed = FALSE;

				break;

				/* Print full screen or just region. */

			case 19:

					/* DECPEX */

				if(Last == 'h')
					PrintFullScreen = TRUE;

				if(Last == 'l')
					PrintFullScreen = FALSE;

				break;

				/* Text cursor enable. */

			case 25:

					/* DECTCEM */

				TurnOn = CursorEnabled;

				ClearCursor();

				if(Last == 'h')
					CursorInvisible = FALSE;

				if(Last == 'l')
					CursorInvisible = TRUE;

				if(TurnOn)
					DrawCursor();

				break;

				/* National/multinational character set. */

			case 42:

					/* DECNRCM */

				break;
		}
	}
	else
	{
		switch(Value)
		{
				/* Keyboard action unlocked/locked. */

			case 2:

					/* KAM */

				break;

				/* Insertion/replacement. */

			case 4:

					/* IRM */

				if(Last == 'h')
					Config->EmulationConfig->InsertMode = TRUE;

				if(Last == 'l')
					Config->EmulationConfig->InsertMode = FALSE;

				break;

				/* Echo on/off. */

			case 12:

					/* SRM */

				if(Last == 'h')
					Config->SerialConfig->Duplex = DUPLEX_FULL;

				if(Last == 'l')
					Config->SerialConfig->Duplex = DUPLEX_HALF;

				break;

				/* Line feed/new line. */

			case 20:

					/* LNM */

				if(Last == 'h')
					Config->EmulationConfig->NewLineMode = TRUE;

				if(Last == 'l')
					Config->EmulationConfig->NewLineMode = FALSE;

				break;
		}
	}
}

	/* NumericAppMode(STRPTR Buffer):
	 *
	 *	Set the numeric pad applications mode.
	 */

VOID
NumericAppMode(STRPTR Buffer)
{
	if(!Config->EmulationConfig->KeysLocked)
	{
		if(*Buffer == '=')
			Config->EmulationConfig->NumericMode = KEYMODE_APPLICATION;
		else
		{
			if(*Buffer == '>')
				Config->EmulationConfig->NumericMode = KEYMODE_STANDARD;
		}
	}
}

	/* MoveCursor(STRPTR Buffer):
	 *
	 *	Move the cursor in some direction and stop at
	 *	top/bottom/margin if necessary.
	 */

VOID
MoveCursor(STRPTR Buffer)
{
	BOOL InRegion;
	BOOL GoDown;
	WORD Value;
	LONG Hit;

	InRegion	= TRUE;
	GoDown		= FALSE;

	ReadValue(Buffer,&Value);

	if(Value < 1)
		Value = 1;

	switch(LastChar(Buffer))
	{
			/* Move cursor Up value lines */

		case 'A':

			Hit = 0;

			if(RegionSet)
			{
				if(CursorY >= Top)
					Hit = Top;
				else
					InRegion = FALSE;
			}

			CursorY -= Value;

			if(CursorY < Hit)
			{
				Value = CursorY - Hit;

				CursorY = Hit;

				if(Config->EmulationConfig->CursorWrap && InRegion)
					ScrollRegion(Value);
			}

			ConFontScaleUpdate();

			break;

			/* Move cursor Down value lines */

		case 'B':

			GoDown = TRUE;
			break;

			/* Move cursor Right value columns */

		case 'C':

			CursorX += Value;

			if(CursorX > LastPrintableColumn)
			{
				if(Config->EmulationConfig->CursorWrap)
				{
					Value = CursorX / (LastPrintableColumn + 1);

					CursorX	-= Value * (LastPrintableColumn + 1);

					GoDown = TRUE;
				}
				else
					CursorX = LastPrintableColumn;
			}

			break;

			/* Move cursor Left value columns */

		case 'D':

			CursorX -= Value;

			if(CursorX < 0)
			{
				if(Config->EmulationConfig->CursorWrap)
				{
					Value	 = CursorX / (LastPrintableColumn + 1);
					CursorX	-= Value * (LastPrintableColumn + 1);
					Value	 = -Value;

					GoDown = TRUE;
				}
				else
					CursorX = 0;
			}

			break;
	}

		/* Scroll down? */

	if(GoDown)
	{
		Hit = LastLine;

		if(RegionSet)
		{
			if(CursorY <= Bottom)
				Hit = Bottom;
			else
				InRegion = FALSE;
		}

		CursorY += Value;

		if(CursorY > Hit)
		{
			Value = CursorY - Hit;

			CursorY = Hit;

			if(Config->EmulationConfig->CursorWrap && InRegion)
				ScrollRegion(Value);
		}

		ConFontScaleUpdate();
	}

	if(CursorX > LastPrintableColumn)
		CursorX = LastPrintableColumn;

	RepositionCursor();
}

	/* MoveColumn(STRPTR Buffer):
	 *
	 *	Move the cursor to a certain column.
	 */

VOID
MoveColumn(STRPTR Buffer)
{
	WORD Value;

	ReadValue(Buffer,&Value);

	if(Value < 1)
		Value = 1;

	CursorX = Value - 1;

	if(CursorX < 0)
		CursorX = 0;

	if(CursorX > LastPrintableColumn)
		CursorX = LastPrintableColumn;

	RepositionCursor();
}

	/* EraseLine(STRPTR Buffer):
	 *
	 *	Erase a line on the display.
	 */

VOID
EraseLine(STRPTR Buffer)
{
	WORD Value;
	LONG Width;

	Width = GetFontWidth();

	if(*Buffer == '?')
		Buffer++;

	ReadValue(Buffer,&Value);

	BackupRender();

	switch(Value)
	{
		case 1:

			if(CursorX)
				ScrollLineRectFillNoTabChange(RPort,0,MUL_Y(CursorY),CursorX * Width - 1,MUL_Y(CursorY + 1) - 1);

			break;

		case 2:

			ScrollLineRectFillNoTabChange(RPort,0,MUL_Y(CursorY),LastPixel,MUL_Y(CursorY + 1) - 1);
			break;

		default:

			ScrollLineRectFillNoTabChange(RPort,CursorX * Width,MUL_Y(CursorY),LastPixel,MUL_Y(CursorY + 1) - 1);
			break;
	}

	ScrollLineEraseLine(Value);

	RasterEraseLine(Value);

	BackupRender();
}

	/* EraseScreen(STRPTR Buffer):
	 *
	 *	Erase parts of the screen.
	 */

VOID
EraseScreen(STRPTR Buffer)
{
	WORD Value;
	LONG Width;

	Width = GetFontWidth();

	if(*Buffer == '?')
		Buffer++;

	ReadValue(Buffer,&Value);

	BackupRender();

	switch(Value)
	{
		case 1:

			if(CursorY)
				ScrollLineRectFillNoTabChange(RPort,0,0,LastPixel,MUL_Y(CursorY) - 1);

			if(CursorX)
				ScrollLineRectFillNoTabChange(RPort,0,MUL_Y(CursorY),CursorX * Width - 1,MUL_Y(CursorY + 1) - 1);

			break;

		case 2:

			ScrollLineRectFillNoTabChange(RPort,0,0,LastPixel,MUL_Y(LastLine + 1) - 1);

			if(Config->EmulationConfig->CLSResetsCursor)
				CursorX = CursorY = 0;

			break;

		default:

			ScrollLineRectFillNoTabChange(RPort,CursorX * Width,MUL_Y(CursorY),LastPixel,MUL_Y(CursorY + 1) - 1);

			if(CursorY != LastLine)
				ScrollLineRectFillNoTabChange(RPort,0,MUL_Y(CursorY + 1),LastPixel,MUL_Y(LastLine + 1) - 1);

			break;
	}

	ScrollLineEraseScreen(Value);

	RasterEraseScreen(Value);

	BackupRender();
}

	/* EraseCharacters(STRPTR Buffer):
	 *
	 *	Erase a number of characters.
	 */

VOID
EraseCharacters(STRPTR Buffer)
{
	WORD Value;
	LONG Width;

	Width = GetFontWidth();

	if(*Buffer == '?')
		Buffer++;

	ReadValue(Buffer,&Value);

	BackupRender();

	if(Value < 1)
		Value = 1;

	if(Value > LastPrintableColumn)
		Value = LastPrintableColumn;

	RasterEraseCharacters(Value);

	ScrollLineRasterNoTabChange(RPort,Value * Width,0,CursorX * Width,MUL_Y(CursorY),LastPixel,MUL_Y(CursorY + 1) - 1,FALSE);

	ScrollLineEraseCharacters(Value);

	BackupRender();
}

	/* InsertCharacters(STRPTR Buffer):
	 *
	 *	Insert a number of characters.
	 */

VOID
InsertCharacters(STRPTR Buffer)
{
	WORD Value;
	LONG Width;

	Width = GetFontWidth();

	ReadValue(Buffer,&Value);

	BackupRender();

	if(Value < 1)
		Value = 1;

	if(CursorX + Value > LastPrintableColumn)
		Value = LastPrintableColumn - CursorX;

	if(Value > 0)
	{
		RasterShiftChar(Value);

		ScrollLineRasterNoTabChange(RPort,-Value * Width,0,CursorX * Width,MUL_Y(CursorY),LastPixel,MUL_Y(CursorY + 1) - 1,FALSE);

		ScrollLineShiftChar(Value);
	}

	BackupRender();
}

	/* InsertLine(STRPTR Buffer):
	 *
	 *	Insert a number of lines and scroll the rest of the
	 *	display down.
	 */

VOID
InsertLine(STRPTR Buffer)
{
	LONG RegionBottom,RegionTop,TheTop;
	WORD Value;

	TheTop = CursorY;

	ReadValue(Buffer,&Value);

	BackupRender();

	SetAPen(RPort,MappedPens[0][PenTable[0]]);

	if(Value < 1)
		Value = 1;

	if(RegionSet)
	{
		RegionTop		= Top;
		RegionBottom	= Bottom + 1;
	}
	else
	{
		RegionTop		= 0;
		RegionBottom	= LastLine + 1;
	}

	if(TheTop < RegionTop)
		TheTop = RegionTop;

	if(TheTop + Value > RegionBottom)
		Value = RegionBottom - TheTop;

	if(Value > 0)
	{
		RasterInsertLine(Value,TheTop);

		ScrollLineRaster(RPort,0,-MUL_Y(Value),0,MUL_Y(TheTop),LastPixel,MUL_Y(RegionBottom) - 1,FALSE);
	}

	BackupRender();
}

	/* ClearLine(STRPTR Buffer):
	 *
	 *	Clear a number of lines and scroll up the ones below it.
	 */

VOID
ClearLine(STRPTR Buffer)
{
	LONG RegionBottom,RegionTop,TheTop;
	WORD Value;

	TheTop = CursorY;

	ReadValue(Buffer,&Value);

	BackupRender();

	SetAPen(RPort,MappedPens[0][PenTable[0]]);

	if(Value < 1)
		Value = 1;

	if(RegionSet)
	{
		RegionTop		= Top;
		RegionBottom	= Bottom + 1;
	}
	else
	{
		RegionTop		= 0;
		RegionBottom	= LastLine + 1;
	}

	if(TheTop < RegionTop)
		TheTop = RegionTop;

	if(TheTop + Value > RegionBottom)
		Value = RegionBottom - TheTop;

	if(Value > 0)
	{
		RasterClearLine(Value,TheTop);

		ScrollLineRaster(RPort,0,MUL_Y(Value),0,MUL_Y(TheTop),LastPixel,MUL_Y(RegionBottom) - 1,FALSE);
	}

	BackupRender();
}

	/* SetTabs(STRPTR Buffer):
	 *
	 *	Set the current tab stops.
	 */

VOID
SetTabs(STRPTR Buffer)
{
	WORD Value;

	ReadValue(Buffer,&Value);

	if(Value < 1)
		Value = 0;

	switch(Value)
	{
		case 0:

			if(CursorX < TabStopMax)
				TabStops[CursorX] = FALSE;

			break;

		case 3:

			memset(TabStops,FALSE,TabStopMax);

			break;

		default:

			break;
	}
}

	/* SetAbsolutePosition(STRPTR Buffer):
	 *
	 *	Move the cursor to a given location on the display,
	 *	this routine ignores the current scroll region
	 *	settings.
	 */

VOID
SetAbsolutePosition(STRPTR Buffer)
{
	WORD Value;

	Buffer = ReadValue(Buffer,&Value);

	CursorY = 0;
	CursorX = 0;

	if(Value == -1)
		ConFontScaleUpdate();
	else
	{
			/* Our raster origin is 0/0 instead of 1/1. */

		if(Value)
			Value--;

		CursorY = Value;

		if(Buffer)
		{
			ReadValue(Buffer,&Value);

			if(Value > 0)
				CursorX = Value - 1;
			else
				CursorX = 0;
		}

			/* Truncate illegal positions. */

		if(CursorY > LastLine)
			CursorY = LastLine;

		ConFontScaleUpdate();

		if(CursorX > LastPrintableColumn)
			CursorX = LastPrintableColumn;
	}

	RepositionCursor();
}

	/* SetTopPosition(STRPTR Buffer):
	 *
	 *	Move the cursor to a given location on the display,
	 *	this routine respects the current scroll region
	 *	settings.
	 */

VOID
SetTopPosition(STRPTR Buffer)
{
	WORD Value;

	Buffer = ReadValue(Buffer,&Value);

	if(UseRegion && RegionSet)
		CursorY = Top;
	else
		CursorY = 0;

	CursorX = 0;

	if(Value == -1)
		ConFontScaleUpdate();
	else
	{
			/* Our raster origin is 0/0 instead of 1/1. */

		if(Value)
			Value--;

		if(UseRegion && RegionSet)
			CursorY = Top + Value;
		else
			CursorY = Value;

		if(Buffer)
		{
			ReadValue(Buffer,&Value);

			if(Value > 0)
				CursorX = Value - 1;
			else
				CursorX = 0;
		}

			/* Truncate illegal positions. */

		if(CursorY > LastLine)
			CursorY = LastLine;

		ConFontScaleUpdate();

		if(CursorX > LastPrintableColumn)
			CursorX = LastPrintableColumn;
	}

	RepositionCursor();
}

	/* SetAttributes(STRPTR Buffer):
	 *
	 *	Set the current display rendering attributes.
	 */

VOID
SetAttributes(STRPTR Buffer)
{
	WORD Value;

	do
	{
		Buffer = ReadValue(Buffer,&Value);

		if(Value == -1)
			Value = 0;

		switch(Value)
		{
			case 0:

				if(!Config->EmulationConfig->LockColour)
				{
					if(Config->ScreenConfig->ColourMode == COLOUR_SIXTEEN)	/* Special case */
						ForegroundPen = 7;
					else
						ForegroundPen = GetPenIndex(SafeTextPen);

					BackgroundPen = 0;
				}

				if(!Config->EmulationConfig->LockStyle)
					Attributes = 0;

				break;

			case 1:

				if(!Config->EmulationConfig->LockStyle)
					Attributes |= ATTR_HIGHLIGHT;

				break;

			case 4:

				if(!Config->EmulationConfig->LockStyle)
					Attributes |= ATTR_UNDERLINE;

				break;

			case 5:

				if(!Config->EmulationConfig->LockStyle)
					Attributes |= ATTR_BLINK;

				break;

			case 7:

				if(!Config->EmulationConfig->LockStyle)
					Attributes |= ATTR_INVERSE;

				break;

			case 22:

				if(!Config->EmulationConfig->LockStyle)
					Attributes &= ~ATTR_HIGHLIGHT;

				break;

			case 24:

				if(!Config->EmulationConfig->LockStyle)
					Attributes &= ~ATTR_UNDERLINE;

				break;

			case 25:

				if(!Config->EmulationConfig->LockStyle)
					Attributes &= ~ATTR_BLINK;

				break;

			case 27:

				if(!Config->EmulationConfig->LockStyle)
					Attributes &= ~ATTR_INVERSE;

				break;

			default:

				if(!Config->EmulationConfig->LockColour)
				{
					if(Value >= 30)
					{
						if(Value <= 37)
							ForegroundPen = Value - 30;
						else
						{
							if(Value >= 40 && Value <= 47)
								BackgroundPen = Value - 40;
						}
					}
				}

				break;
		}
	}
	while(Buffer);

	UpdatePens();

	RepositionCursor();
}

	/* SetRegion(STRPTR Buffer):
	 *
	 *	Set the current scroll region top and bottom.
	 */

VOID
SetRegion(STRPTR Buffer)
{
	LONG NewTop,NewBottom;
	WORD Value;

	NewBottom = LastLine;

	Buffer = ReadValue(Buffer,&Value);

	if(!Value)
		Value = 1;

	if(Value > 0)
	{
		if(Buffer)
		{
			NewTop = Value - 1;

			ReadValue(Buffer,&Value);

			if(Value > 0)
				NewBottom = Value - 1;

			if(NewBottom > LastLine)
				NewBottom = LastLine;

			if(NewTop > LastLine)
				NewTop = LastLine;
		}
		else
		{
			NewTop		= 0;
			NewBottom	= LastLine;
		}
	}
	else
	{
		NewTop		= 0;
		NewBottom	= LastLine;
	}

	if(NewTop < NewBottom)
	{
		if(NewTop != 0 || NewBottom != LastLine)
		{
			Top		= NewTop;
			Bottom	= NewBottom;

				/* FIXME: not sure about this one */

			UseRegion = TRUE;

			RegionSet = TRUE;
		}
		else
			UseRegion = RegionSet = FALSE;

		DB(kprintf("scroll region %ld->%ld\n",Top,Bottom));

		ResetCursor();
	}
	else
		RegionSet = FALSE;
}

	/* ResetCursor():
	 *
	 *	Reset cursor to top of screen.
	 */

VOID
ResetCursor()
{
	CursorX	= 0;

	if(UseRegion && RegionSet)
		CursorY = Top;
	else
		CursorY	= 0;

	ConFontScaleUpdate();

	RepositionCursor();
}

	/* MoveCursorUp(STRPTR Buffer):
	 *
	 *	Move the cursor up <n> lines, scroll the screen
	 *	contents if necessary.
	 */

VOID
MoveCursorUp(STRPTR Buffer)
{
	WORD Value;
	LONG i;

	ReadValue(Buffer,&Value);

	if(Value < 1)
		Value = 1;

	for(i = 0 ; i < Value ; i++)
		CursorScrollUp();

	CursorX = 0;

	RepositionCursor();
}

	/* MoveCursorDown(STRPTR Buffer):
	 *
	 *	Move the cursor down <n> lines, scroll the screen
	 *	contents if necessary.
	 */

VOID
MoveCursorDown(STRPTR Buffer)
{
	WORD Value;
	LONG i;

	ReadValue(Buffer,&Value);

	if(Value < 1)
		Value = 1;

	for(i = 0 ; i < Value ; i++)
		DownLine();

	CursorX = 0;

	RepositionCursor();
}

	/* VT52_CursorUp(STRPTR Buffer):
	 *
	 *	Move the cursor up one line.
	 */

VOID
VT52_CursorUp(STRPTR UnusedBuffer)
{
	if(CursorY > 0)
	{
		CursorY--;

		RepositionCursor();
	}
}

	/* VT52_CursorDown(STRPTR Buffer):
	 *
	 *	Move the cursor down one line.
	 */

VOID
VT52_CursorDown(STRPTR UnusedBuffer)
{
	if(CursorY < LastLine)
	{
		CursorY++;

		RepositionCursor();
	}
}

	/* VT52_CursorRight(STRPTR Buffer):
	 *
	 *	Move the cursor right one column.
	 */

VOID
VT52_CursorRight(STRPTR UnusedBuffer)
{
	if(CursorX < LastColumn)
	{
		CursorX++;

		RepositionCursor();
	}
}

	/* VT52_CursorLeft(STRPTR Buffer):
	 *
	 *	Move the cursor left one column.
	 */

VOID
VT52_CursorLeft(STRPTR UnusedBuffer)
{
		/* This command is shared by the VT220 emulation and */
		/* the VT52 mode. */

	if(VT52_Mode)
	{
		if(CursorX > 0)
		{
			CursorX--;

			RepositionCursor();
		}
	}
	else
		CursorScrollDown();	/* IND */
}

	/* VT52_CursorHome(STRPTR Buffer):
	 *
	 *	Move the cursor to the home position.
	 */

VOID
VT52_CursorHome(STRPTR UnusedBuffer)
{
		/* This command is shared by the VT220 emulation and */
		/* the VT52 mode. */

	if(VT52_Mode)
	{
		CursorX = CursorY = 0;

		RepositionCursor();
	}
	else
		SetTab();
}

	/* VT52_RevLF(STRPTR Buffer):
	 *
	 *	Perform reverse line feed, i.e. move cursor up one line.
	 */

VOID
VT52_RevLF(STRPTR UnusedBuffer)
{
	if(CursorY > 0)
	{
		CursorY--;

		RepositionCursor();
	}
}

	/* VT52_EraseEOS(STRPTR Buffer):
	 *
	 *	Erase till end of screen.
	 */

VOID
VT52_EraseEOS(STRPTR UnusedBuffer)
{
	EraseScreen("0");
}

	/* VT52_EraseEOL(STRPTR Buffer):
	 *
	 *	Erase till end of line.
	 */

VOID
VT52_EraseEOL(STRPTR UnusedBuffer)
{
	EraseLine("0");
}

	/* VT52_SetCursor(STRPTR Buffer):
	 *
	 *	Direct cursor address; move the cursor to a
	 *	specific position.
	 */

VOID
VT52_SetCursor(STRPTR Buffer)
{
	CursorX = Buffer[2] - ' ';
	CursorY = Buffer[1] - ' ';

	if(CursorX < 0)
		CursorX = 0;
	else
	{
		if(CursorX > LastColumn)
			CursorX = LastColumn;
	}

	if(CursorY < 0)
		CursorY = 0;
	else
	{
		if(CursorY > LastLine)
			CursorY = LastLine;
	}

	RepositionCursor();
}

	/* VT52_PrintOn(STRPTR Buffer):
	 *
	 *	Enter printer controller mode.
	 */

VOID
VT52_PrintOn(STRPTR UnusedBuffer)
{
	OpenPrinterCapture(FALSE);
}

	/* VT52_PrintOff(STRPTR Buffer):
	 *
	 *	Exit printer controller mode.
	 */

VOID
VT52_PrintOff(STRPTR UnusedBuffer)
{
	ClosePrinterCapture(FALSE);
}

	/* VT52_PrintScreen(STRPTR Buffer):
	 *
	 *	Print the entire screen.
	 */

VOID
VT52_PrintScreen(STRPTR UnusedBuffer)
{
	if(Config->EmulationConfig->PrinterEnabled)
		PrintRegion(0,LastLine + 1,PrintFormFeed);
}

	/* VT52_PrintLine(STRPTR Buffer):
	 *
	 *	Print the line the cursor sits in.
	 */

VOID
VT52_PrintLine(STRPTR UnusedBuffer)
{
	if(Config->EmulationConfig->PrinterEnabled)
		PrintRegion(CursorY,CursorY + 1,FALSE);
}

	/* VT52_EnterANSI(STRPTR Buffer):
	 *
	 *	Return to ANSI mode, or in other terms, change
	 *	the behaviour of the "<ESC>H" and "<ESC>D" commands.
	 */

VOID
VT52_EnterANSI(STRPTR UnusedBuffer)
{
	VT52_Mode = FALSE;
}
