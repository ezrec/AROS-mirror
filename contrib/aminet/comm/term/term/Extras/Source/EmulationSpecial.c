/*
**	EmulationSpecial.c
**
**	Special terminal emulation character handling
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* DoxON_xOFF():
	 *
	 *	This routine handles both xON and xOFF, it makes the emulation
	 *	swallow the character rather than printing it.
	 */

BOOL
DoxON_xOFF()
{
	return(FALSE);
}

	/* DoBackspace():
	 *
	 *	Special function: perform backspace.
	 */

BOOL
DoBackspace()
{
	if(CursorX)
	{
		LONG DeltaX,MinX;

		CursorX--;

			/* What backspace mode are we in? */

		switch(Config->EmulationConfig->DestructiveBackspace)
		{
				/* Do nothing */

			case 0:

				RepositionCursor();
				break;

				/* Shift the line to the left */

			case 2:

				BackupRender();

				RasterEraseCharacters(1);

				if(FontScalingRequired)
				{
					if(CurrentCharWidth == SCALE_NORMAL)
					{
						DeltaX	= TextFontWidth * 2;
						MinX	= MUL_X(CursorX) * 2;
					}
					else
					{
						DeltaX	= TextFontWidth / 2;
						MinX	= MUL_X(CursorX) / 2;
					}
				}
				else
				{
					DeltaX	= TextFontWidth;
					MinX	= MUL_X(CursorX);
				}

				ScrollLineEraseCharacters(1);

				ScrollLineRaster(RPort,DeltaX,0,MinX,MUL_Y(CursorY),LastPixel,MUL_Y(CursorY + 1) - 1,FALSE);

				BackupRender();

				RepositionCursor();

				break;

				/* Clear the character below the cursor */

			default:

				RepositionCursor();

				ObtainSemaphore(&RasterSemaphore);

				Raster[CursorY * RasterWidth + CursorX] = ' ';

				ReleaseSemaphore(&RasterSemaphore);

				BackupRender();

				if(FontScalingRequired)
					PrintScaled(" ",1,CurrentFontScale);
				else
					Text(RPort," ",1);

				BackupRender();

				break;
		}
	}

	return(FALSE);
}

	/* DoBeep():
	 *
	 *	The real interface to the beep routine.
	 */

BOOL
DoBeep()
{
	BellSignal();

	return(FALSE);
}

	/* DoLF():
	 *
	 *	Special function: perform line feed.
	 */

BOOL
DoLF()
{
		/* This takes care of regular jump scrolling */

	if(CursorY == Bottom && Bottom > 0 && Config->EmulationConfig->MaxJump > 1)
	{
		LONG Scroll,TotalLines,OldBack = BackgroundPen;

		Scroll = Config->EmulationConfig->MaxJump;

			/* How tall is the current scroll region? */

		TotalLines = Bottom - Top + 1;

			/* Don't scroll more than the entire screenful */

		if(Scroll > TotalLines)
			Scroll = TotalLines;

		if(OldBack)
		{
			BackgroundPen = 0;

			UpdatePens();
		}

			/* Scroll the region... */

		ScrollRegion(Scroll);

			/* Reposition the cursor */

		if(CursorY > Scroll)
			CursorY -= Scroll;
		else
			CursorY = 0;

		if(OldBack)
		{
			BackgroundPen = OldBack;

			UpdatePens();
		}
	}

	DownLine();

	RepositionCursor();

	return(FALSE);
}

	/* DoShiftIn():
	 *
	 *	Special function: Shift into graphics mode
	 */

BOOL
DoShiftIn()
{
	if(CharMode[1] == TABLE_GFX && GFX)
		CurrentFont = GFX;

	if(CharMode[1] == TABLE_ASCII)
		CurrentFont = TextFont;

	SetFont(RPort,CurrentFont);

	ConOutputUpdate();

	Charset = 1;

	return(FALSE);
}

	/* DoShiftOut():
	 *
	 *	Special function: Shift out of graphics mode
	 */

BOOL
DoShiftOut()
{
	if(CharMode[0] == TABLE_GFX && GFX)
		CurrentFont = GFX;

	if(CharMode[0] == TABLE_ASCII)
		CurrentFont = TextFont;

	SetFont(RPort,CurrentFont);

	ConOutputUpdate();

	Charset = 0;

	return(FALSE);
}

	/* DoCR_LF():
	 *
	 *	Special function: perform carriage return and line feed.
	 */

BOOL
DoCR_LF()
{
	CursorX = 0;

	DownLine();

	RepositionCursor();

	return(FALSE);
}

	/* DoFF():
	 *
	 *	Special function: perform form feed.
	 */

BOOL
DoFF()
{
	if(Config->EmulationConfig->NewLineMode)
	{
		CursorX = 0;

		DoCR_LF();
	}
	else
	{
		EraseScreen("2");

		CursorX = CursorY = 0;

		RepositionCursor();

		ConFontScaleUpdate();
	}

	return(FALSE);
}

	/* DoLF_FF_VT():
	 *
	 *	Special function: handle line feed, form feed and vertical
	 *	tab.
	 */

BOOL
DoLF_FF_VT()
{
	if(Config->EmulationConfig->NewLineMode)
		DoCR_LF();
	else
		DoLF();

	return(FALSE);
}

	/* DoCR():
	 *
	 *	Special function: handle carriage return.
	 */

BOOL
DoCR()
{
	if(Config->EmulationConfig->NewLineMode)
		DoCR_LF();
	else
	{
		CursorX = 0;

		RepositionCursor();
	}

	return(FALSE);
}

	/* DoTab():
	 *
	 *	Special function: handle tab, move cursor to next
	 *	tab stop.
	 */

BOOL
DoTab()
{
	LONG Column;

	if(RasterAttr[CursorY] == SCALE_ATTR_NORMAL)
		Column = LastColumn;
	else
		Column = ((LastColumn + 1) / 2) - 1;

	if(Config->EmulationConfig->LineWrap)
	{
		if(CursorX >= LastColumn)
		{
			CursorX = 0;

			DownLine();
		}
		else
		{
			while(CursorX < Column)
			{
				CursorX++;

				if(TabStops[CursorX])
					break;
			}
		}
	}
	else
	{
		while(CursorX < Column)
		{
			CursorX++;

			if(TabStops[CursorX])
				break;
		}
	}

	RepositionCursor();

	return(FALSE);
}

	/* DoEnq():
	 *
	 *	Special function: send answerback message.
	 */

BOOL
DoEnq()
{
	if(Config->EmulationConfig->AnswerBack[0])
		SerialCommand(Config->EmulationConfig->AnswerBack);

	return(FALSE);
}

	/* DoEsc():
	 *
	 *	Start new control sequence.
	 */

BOOL
DoEsc()
{
	if(Config->TerminalConfig->EmulationMode == EMULATION_TTY)
		(* ConDump)("^",1);

	return(TRUE);
}

	/* DoCsi():
	 *
	 *	Start new control sequence.
	 */

BOOL
DoCsi()
{
	if(Config->TerminalConfig->EmulationMode == EMULATION_TTY)
		(* ConDump)("^[",2);

	return(ParseCode('['));
}

	/* DoNewEsc(LONG Char):
	 *
	 *	Start new control sequence.
	 */

BOOL
DoNewEsc(LONG UnusedChar)
{
	if(Config->TerminalConfig->EmulationMode == EMULATION_TTY)
		(* ConDump)("^",1);

	DoCancel();

	return(TRUE);
}

	/* DoNewCsi(LONG Char):
	 *
	 *	Start new control sequence.
	 */

BOOL
DoNewCsi(LONG UnusedChar)
{
	if(Config->TerminalConfig->EmulationMode == EMULATION_TTY)
		(* ConDump)("^[",2);

	DoCancel();

	return(ParseCode('['));
}
