#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


                /* CSI Funktionen zu VT340 Emulator.. */


STATIC VOID VT340_placeCursorAt(struct VT340Console *con, WORD row, WORD col);
STATIC VOID VT340_moveCursorForward(struct VT340Console *con, UWORD cnt);
STATIC VOID VT340_moveCursorDown(struct VT340Console *con, UWORD cnt);



VOID VT340_ignore(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	return;	/* ..(^: */
}


/*
 CSI Pn @	ICH		Insert Pn spaces at and after cursor
*/
VOID VT340_insertchars(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	PageInsertChars(con, con->col, cnt);
}


/*
 CSI Pn A	CUU		Cursor up Pn lines, does not scroll
*/
VOID VT340_simplecursorup(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	WORD top;
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	top = (con->top > con->row) ? con->firstLine : con->top;

	if(con->row - cnt > top)
		con->row -= cnt;
	else
		con->row = top;


	VT340_adjustCursorColumnPosition(con);
}


/*
 CSI Pn B	CUD		Cursor down Pn lines, does not scroll
*/
VOID VT340_simplecursordown(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	VT340_moveCursorDown(con, cnt);
}


STATIC VOID VT340_moveCursorDown(struct VT340Console *con, UWORD cnt)
{
	WORD bot;

	bot = (con->row > con->bot) ? con->lastLine : con->bot;

	if(con->row + cnt < bot)
		con->row += cnt;
	else
		con->row = bot;

	VT340_adjustCursorColumnPosition(con);
}


/*
 CSI Pn C	CUF		Cursor forward, stays on same line
*/
VOID VT340_simplecursorright(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	VT340_moveCursorForward(con, cnt);
}


STATIC VOID VT340_moveCursorForward(struct VT340Console *con, UWORD cnt)
{
	UWORD columns;

	columns = con->columns;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
		columns >>= 1;

	if(con->col + cnt < columns)
		con->col += cnt;
	else
		con->col = columns;
}


/*
 CSI Pn D	CUB		Cursor backward, stays on same line
*/
VOID VT340_simplecursorleft(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	if(con->col - cnt > 1)
		con->col -= cnt;
	else
		con->col = 1;
}


/*
 ESC M          RI 		Reverse Index, cursor up one line, can scroll
 RI	8/13  	8dh   ESC M		Reverse index, move cursor up one line,scrolls
*/
VOID VT340_cursorup(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	BYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	while(cnt--)
	{
		if(con->row == con->top)
		{
			if(con->pstat & P_QUICK_REND  &&  !(con->pstat & P_SMOOTH_SCROLL))
			{
				PageScrollDisplay(con, -2, con->top, con->bot);
				con->row++;
			}
			else
				PageScrollDisplay(con, -1, con->top, con->bot);
		}
		else
		{
			if(con->row > 1)
				con->row--;
		}
	}


	VT340_adjustCursorColumnPosition(con);
}


VOID VT340_adjustCursorColumnPosition(struct VT340Console *con)
{
	UWORD columns;

	columns = con->columns;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
		columns >>= 1;

	if(con->col > columns)
		con->col = columns;
}


/*
 IND	8/4   	84h   ESC D	Index, move cursor down one line, scrolls
 ESC D          IND			Index, moves cursor down one line, can scroll
*/
VOID VT340_cursordown(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	while(cnt--)
	{
		if(con->row == con->bot)
		{
			if(con->pstat & P_QUICK_REND  &&  !(con->pstat & P_SMOOTH_SCROLL))
			{
				PageScrollDisplay(con, +2, con->top, con->bot);
				con->row--;
			}
			else
				PageScrollDisplay(con, +1, con->top, con->bot);
		}
		else
		{
			if(con->row < con->lastLine)
				con->row++;
		}
	}

	VT340_adjustCursorColumnPosition(con);
}


/*
 ESC E      NEL				Move cursor to start of line below, can scroll
 NEL	8/5  	85h   ESC E		Next line, like CR/LF, scrolls
 CSI Pn E	CNL				Next-line (same as cr/lf), do Pn times
*/
VOID VT340_cursornextline(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->col = 1;	/* do a fake CR */
	VT340_cursordown(con, args, argc);
}


/*
 ESC M          RI 				Reverse Index, cursor up one line, can scroll
 CSI Pn F		 CPL				Previous-line (reverse index), do Pn times
*/
VOID VT340_cursoraboveline(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->col = 1;	/* do a fake CR */
	VT340_cursorup(con, args, argc);
}


/*
                               => ABSOLUT <=
 CSI Pr; Pc H	CUP		Set cursor to row, column (same as HVP)
 CSI Pr; Pc f	HVP		Set cursor to row, column (same as CUP)
*/

VOID VT340_cursorposition(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_placeCursorAt(con, args[0], args[1]);
}


STATIC VOID VT340_placeCursorAt(struct VT340Console *con, WORD row, WORD col)
{
	WORD top, bot;

	VT340_getTopBot(con, &top, &bot);

	con->row = row;

	if(con->row == 0)
		con->row = top;
	else
	{
		con->row += top-1;

		if(con->row > bot)
			con->row = bot;
	}


	con->col = col;

	if(con->col < 1)
		con->col = 1;
	else
		VT340_adjustCursorColumnPosition(con);
}


/*
 CSI Ps J	ED		Erase in display:
				0 = cursor to end of screen, inclusive
				1 = start of screen to cursor, inclusive
				2 = entire screen, reset lines to single
*/
VOID VT340_eraseindisplay(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(con->firstLine == con->statusLine)	/* Aetsch..<^:< */
	{
		VT340_eraseinline(con, args, argc);		 /* help prevent guru's..(-; */
		return;
	}

	switch(args[0])
	{
		case 2:		/* clears the row-register */
			PageEraseInDisplay(con, 2);
		break;

		case 1:
			PageEraseInDisplay(con, 1);
		break;

		default:
			PageEraseInDisplay(con, 0);
		break;

	}
}


/*
 CSI Ps K	EL		Erase in line:
				0 = cursor to end of line, inclusive
				1 = start of line to cursor, inclusive
				2 = entire line, cursor does not move
*/
VOID VT340_eraseinline(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	switch(args[0])
	{
		case 2:
			PageEraseInLine(con, 2);
		break;

		case 1:
			PageEraseInLine(con, 1);
		break;

		default:
			PageEraseInLine(con, 0);
		break;

	}
}


VOID VT340_sel_eraseindisplay(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	switch(args[0])
	{
		case 2:
			PageSelectiveEraseScreen(con, 2);
		break;

		case 1:
			PageSelectiveEraseScreen(con, 1);
		break;

		default:
			PageSelectiveEraseScreen(con, 0);
		break;

	}
}


VOID VT340_sel_eraseinline(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	switch(args[0])
	{
		case 2:
			PageSelectiveEraseLine(con, 2);
		break;

		case 1:
			PageSelectiveEraseLine(con, 1);
		break;

		default:
			PageSelectiveEraseLine(con, 0);
		break;

	}
}


/*
 CSI Pn L	IL		Insert Pn lines preceding current line.
*/
VOID VT340_insertlines(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD lines;

	lines = args[0];
	if(!lines)
		lines = 1;

	if(con->row >= con->top  &&  con->row <= con->bot)
	{
		UBYTE cnt;

		cnt = con->bot - con->row + 1;
		if(lines > cnt)
			lines = cnt;

		PageScrollRegion(con, -lines, con->row, con->bot);
		myScrollRaster(con, 0, -con->ySize * lines, 0, (con->row - 1) * con->ySize, con->border.Width-1, (con->bot*con->ySize)-1);

		con->col = 1;
	}
}


/*
 CSI Pn M	DL		Delete Pn lines from current downward, incl.
*/
VOID VT340_deletelines(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD lines=0;

	lines = args[0];
	if(lines == 0)
		lines = 1;

	if(con->row >= con->top  &&  con->row <= con->bot)
	{
		UBYTE cnt;

		cnt = con->bot - con->row + 1;
		if(lines > cnt)
			lines = cnt;

		PageScrollRegion(con, +lines, con->row, con->bot);
		myScrollRaster(con, 0, +con->ySize * lines, 0, (con->row-1)*con->ySize, con->border.Width-1, (con->bot*con->ySize)-1);

		con->col = 1;
	}
}


/*
 CSI Pn P	DCH		Delete Pn chars from cursor to left, incl.
*/
VOID VT340_deletechars(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	PageDeleteChars(con, con->col, cnt);
}


/*
 CSI Pn S	SU
 CSI Pn T	SD
*/
VOID VT340_scrollcon(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD lines;

	lines = (args[0] == 0) ? 1 : args[0];

	switch(con->inCSI)
	{
		case 'S':	/* Console Scroll Down */
			if((con->firstLine-1) + lines > con->lastLine)
				lines = con->lastLine - (con->firstLine-1);
			PageScrollDisplay(con, +(lines), con->firstLine, con->lastLine);
			VT340_adjustCursorColumnPosition(con);
		break;

		case 'T':	/* Console Scroll Up */
			if(con->lastLine - lines < (con->firstLine-1))
				lines = con->lastLine - (con->firstLine-1);
			PageScrollDisplay(con, -(lines), con->firstLine, con->lastLine);
			VT340_adjustCursorColumnPosition(con);
		break;

	}
}


/*
 CSI Pn X	ECH		Erase Pn chars at and to right of cursor
*/
VOID VT340_erasechars(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD xsize, lim;
	WORD col, y;
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	xsize	= XSIZE;
	lim	= con->columns;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
	{
		xsize	<<= 1;
		lim	<<= 1;
	}

	if(con->col - 1 + cnt > lim)
		cnt = lim - con->col + 1;

	PageEraseCharacters(con, con->col, cnt);

	col	= con->col - 1 + cnt;
	y		= con->row * con->ySize;
	myRectFill(con, (con->col-1) * xsize, y-con->ySize, (col*xsize)-1, y-1, BACKGROUND_PEN);
}


VOID VT340_tabControl(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD i;
	UBYTE cnt;

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		switch(cnt)
		{
			case 0:
				VT340_setActualTabPos(con);
			break;

			case 2:
				VT340_clearActualTabPos(con);
			break;

			case 5:
				VT340_clearAllTabs(con);
			break;
		}
	}
}


VOID VT340_tabClear(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD i;
	UBYTE cnt;

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		switch(cnt)
		{
			case 0:
				VT340_clearActualTabPos(con);
			break;

			case 3:
				VT340_clearAllTabs(con);
			break;
		}
	}
}


/*
 CSI Pn I          CHI		Cursor forward Pn tabstops
*/
VOID VT340_tabForward(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	while(cnt--)
	{
		for(;;)
		{
			if(con->col >= con->columns)
				return;

			con->col++;
			if(VT340_isTab(con))
				break;
		}
	}
}


/*
 CSI Pn I          CBT		Cursor backward Pn tabstops
*/
VOID VT340_tabBackward(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	while(cnt--)
	{
		for(;;)
		{
			if(con->col <= 1)
				return;

			con->col--;
			if(VT340_isTab(con))
				break;
		}
	}
}


/*
 CSI Pa;...Pa h	SM		Set ANSI mode, see table below
        Pa	Mnemonic	Mode		Set (h)
			2	KAM	   		   *Keyboard	locked
			3	CRM					Control rep	act upon
			4	IRM					Insert		insert
		  10	HEM					Horz editing	n/a
		  12	SRM					Local echo	off
		  20	LNM					New Line		cr=>cr/lf
*/
VOID VT340_ANSIsetmodes(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	BOOL update=FALSE;
	UWORD i;
	UBYTE cnt;

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		switch(cnt)
		{
			case 4:
				con->ustat |= U_INSERT;
				update = TRUE;
			break;

			case 20:
				con->ustat |= U_NEWLINE;
				update = TRUE;
			break;
		}
	}

	if(update != FALSE)
		CreateOwnStatusLine(con);
}


/*
 CSI ? Ps;...;Ps h  SM		Set DEC mode, see table below
 CSI ? Ps;...;Ps l  RM		Reset DEC mode, see table below
         Ps     Mnemonic       Mode           Set (h)
         0                     error (ignored)
         1      DECCKM         cursor keys    application
         2      DECANM         ANSI	     		 VT340/VT102
         3      DECCOLM        Columns        +132 col
         4      DECSCLM       *Scrolling      smooth
         5      DECSCNM        Screen, whole  reverse video
         6      DECOM          Origin         stay in margins
         7      DECAWM         Autowrap       on
         8      DECARM        *Autorepeat     on
         9      DECINLM       *Interlace      on
         18     DECPFF         Printer term   Form Feed
         19     DECPEX         Printer extent screen
			25		 DECTCEM	       Cursor	       visible
         34     n/a            Invoke macro:  TERMINALS
         38     n/a            Graphics (Tek) ++graphics
			42		 DECNRCM	       Nat Repl Char  enable
			66		 DECNKM	       Numeric keypad application
	 		68		 DECKBUM	      *Typewriter     data process
                             + See comments on EGA boards.
                            ++ Ignored if DISABLE TEK has been given.
	(SET TERMINAL CHARACTER-SET <country> must be given to employ DEC
	National Replacement Characters. Enabling NRCs replaces	G0..G3 chars
	sets with the NRC set and disables 8-bit controls; disabling only
	re-enables 8-bit controls.)
*/
VOID VT340_DECsetmodes(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	BOOL update=FALSE;
	UWORD i;
	UBYTE cnt;

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		switch(cnt)
		{
			case 1:
				con->ustat |= U_CURAPP;
			break;

			case 4:
				con->pstat |= P_SMOOTH_SCROLL;
			break;

			case 6:
				con->tstat |= T_USE_REGION;
				con->row = con->top;
				con->col = 1;
			break;

			case 7:
				con->tstat |= T_WRAP;
				update = TRUE;
			break;

			case 18:
				con->gstat |= G_FORMFEED;
			break;

			case 19:
				con->gstat |= G_FULLSCREEN;
			break;

			case 25:
				con->ustat |= U_CURSOR_VISIBLE;
			break;

			case 38:
				con->inTEK = TRUE;
				update = TRUE;
			break;

			case 66:
				con->ustat |= U_KEYAPP;
			break;

			case 67:
				con->ustat |= U_BACKSPACE_BS;
			break;

		}
	}

	if(update != FALSE)
		CreateOwnStatusLine(con);
}


/*
 CSI Pa;...Pa l SM		Reset ANSI mode, see table below
        Pa	Mnemonic	Mode		Reset (l)
			2	KAM	      		*Keyboard		unlocked
			3	CRM					Control rep		debug display
			4	IRM					Insert			replace
			10	HEM					Horz editing	always reset
			12	SRM					Local echo		on
			20	LNM					New Line			cr=>cr
*/
VOID VT340_ANSIresetmodes(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	BOOL update=FALSE;
	UWORD i;
	UBYTE cnt;

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		switch(cnt)
		{
			case 4:
				con->ustat &= ~U_INSERT;
				update = TRUE;
			break;

			case 20:
				con->ustat &= ~U_NEWLINE;
				update = TRUE;
			break;
		}
	}

	if(update != FALSE)
		CreateOwnStatusLine(con);
}


/*
 CSI ? Ps;...;Ps l  RM		Reset DEC mode, see table below
         Ps     Mnemonic       Mode           Reset (l)
         0                     error (ignored)
         1      DECCKM         cursor keys    cursor/numeric
         2      DECANM         ANSI	          VT52
         3      DECCOLM        Columns        80 col
         4      DECSCLM       *Scrolling      jump
         5      DECSCNM        Screen, whole  normal
         6      DECOM          Origin         ignore margins
         7      DECAWM         Autowrap       off
         8      DECARM        *Autorepeat     off
         9      DECINLM       *Interlace      off
         18     DECPFF         Printer term   none
         19     DECPEX         Printer extent scrolling region
	 25	DECTCEM	       		 Cursor	       invisible
         34     n/a            Invoke macro:  TERMINALR
         38     n/a            Graphics (Tek) text
	 42	DECNRCM	      		 Nat Repl Char  disable
	 66	DECNKM	     			 Numeric keypad numeric
	 68	DECKBUM	      		*Typewriter     typewriter
                             + See comments on EGA boards.
                            ++ Ignored if DISABLE TEK has been given.
	(SET TERMINAL CHARACTER-SET <country> must be given to employ DEC
	National Replacement Characters. Enabling NRCs replaces	G0..G3 chars
	sets with the NRC set and disables 8-bit controls; disabling only
	re-enables 8-bit controls.)
*/
VOID VT340_DECresetmodes(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	BOOL update=FALSE;
	UWORD i;
	UBYTE cnt;

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		switch(cnt)
		{
			case 1:
				con->ustat &= ~U_CURAPP;
			break;

			case 2:
				VT340_invokeVT52(con);
			break;

			case 4:
				con->pstat &= ~P_SMOOTH_SCROLL;
			break;

			case 6:
				con->tstat &= ~T_USE_REGION;
				con->row = con->firstLine;
				con->col = 1;
			break;

			case 7:
				con->tstat &= ~T_WRAP;
				update = TRUE;
			break;

			case 18:
				con->gstat &= ~G_FORMFEED;
			break;

			case 19:
				con->gstat &= ~G_FULLSCREEN;
			break;

			case 25:
				con->ustat &= ~U_CURSOR_VISIBLE;
			break;

			case 38:
				con->inTEK = FALSE;
				update = TRUE;
			break;

			case 66:	/* keypad application */
				con->ustat &= ~U_KEYAPP;
			break;

			case 67:
				con->ustat &= ~U_BACKSPACE_BS;
			break;

		}
	}

	if(update != FALSE)
		CreateOwnStatusLine(con);
}


/*
 CSI Ps;...Ps m  SGR		Select graphic rendition
		                  Ps
								0	all attributes off (#'s 1, 4, 5, 7)
                        1	bold, intensify foreground
                        4	underscore (reverse video on IBM CGA/EGA)
                        5	blink
                        7	reverse video, per character
								22	bold off, VT340
			               24	underline off, VT340
			               25	blinking off, VT340
			               27	reverse video off, VT340
         non-DEC extensions: 30-37 = foreground color = 30 + colors
                             40-47 = background color = 40 + colors
                             colors: 1 = red, 2 = green, 4 = blue
*/
VOID VT340_textmodes(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	ULONG mask, oldTextStyle;
	UWORD i;
	UBYTE cnt;

	oldTextStyle = GetTextStyle(con);

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		switch(cnt)
		{
			case 0:
				VT340_resetstyles(con, FALSE);
				oldTextStyle = GetTextStyle(con);
			break;

			case 1:
				con->attr |= (con->hiLite != FALSE) ? ATTR_HIGHLIGHT : ATTR_FAT;
			break;

			case 4:
				con->attr |= ATTR_UNDERLINE;
			break;

			case 7:
				if(!(con->attr & ATTR_INVERSE))
				{
					UBYTE help;

					help		 = con->apen;
					con->apen = con->bpen;
					con->bpen = help;
				}

				con->attr |= ATTR_INVERSE;
			break;

			case 22:
				con->attr &= (con->hiLite != FALSE) ? ~ATTR_HIGHLIGHT : ~ATTR_FAT;
			break;

			case 24:
				con->attr &= ~ATTR_UNDERLINE;
			break;

			case 27:
				if(con->attr & ATTR_INVERSE)
				{
					UBYTE help;

					help		 = con->apen;
					con->apen = con->bpen;
					con->bpen = help;
				}

				con->attr &= ~ATTR_INVERSE;
			break;

			default:
				if(con->mono == FALSE)
				{
					UBYTE *pen;

					if(cnt >= 30  &&  cnt <= 37)
					{
						pen = (con->attr & ATTR_INVERSE) ? &con->bpen : &con->apen;
						*pen = cnt - 30;
					}
					else
					{
						if(cnt >= 40  &&  cnt <= 47)
						{
							pen = (con->attr & ATTR_INVERSE) ? &con->apen : &con->bpen;
							*pen = cnt - 40;
						}
					}
				}
			break;

		}
	}

	mask = (1 << con->io->xem_window->WScreen->RastPort.BitMap->Depth) - 1;

	if(con->apen > mask  &&  !(con->apen & mask))
		con->apen = con->foregroundPen;

	if(con->bpen > mask  &&  !(con->bpen & mask))
		con->bpen = BACKGROUND_PEN;


	if(con->attr & ATTR_HIGHLIGHT)
	{
		UBYTE *pen;

		pen = (con->attr & ATTR_INVERSE) ? &con->bpen : &con->apen;
		if(con->mono != FALSE)
			*pen = 3;
		else
			*pen |= 8;
	}
	else
	{
		UBYTE *pen;

		pen = (con->attr & ATTR_INVERSE) ? &con->bpen : &con->apen;

		if(con->mono != FALSE)
			*pen = con->foregroundPen;
		else
			*pen &= ~8;
	}

	if(con->apen == con->bpen)
	{
		con->bpen = BACKGROUND_PEN;

		if(con->apen == con->bpen)
			con->apen = con->foregroundPen;
	}

	if(oldTextStyle != GetTextStyle(con))
		SetSoftStyle(con->rp, GetTextStyle(con), SYSTEM_TEXTSTYLES);

	if(con->apen != con->rp->FgPen)
		SetAPen(con->rp, con->apen);

	if(con->bpen != con->rp->BgPen)
		SetBPen(con->rp, con->bpen);
}


/* Request  CSI 5 n		operating status
 Response CSI 0 n			no malfunction
 
 Request  CSI 6 n		cursor position report
 Response CSI Pr; Pc R			Pr = row, Pc = column. Origin is 1,1
 
 Request  CSI ? 15 n		printer status
 Response CIS ? 10 n			printer ready
      or  CSI ? 11 n			printer is not ready
 
 Request  CSI ? 25 n		User Definable Key status
 Response CSI ? 20 n			UDKs are unlocked (21 if locked)
 
 Request  CSI ? 26 n		keyboard dialect
 Response CSI ? 27; Ps n	in MS Kermit this is controlled by
									command SET TERMINAL CHARACTER-SET <country>
		Ps	Country					Ps	Country
		1	North American/ASCII	 9	Italian
		2	British					13	Norwegian/Danish
		8	Dutch						16	Portugese
		6	Finnish					15	Spanish
		14	French					12	Swedish
		4	French Canadian		11	Swiss (German)
		7	German
*/
VOID VT340_devicestatreport(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE com[16], *answer=NULL;

	switch(args[0])
	{
		case 5:
			answer = "\033[0n";
		break;

		case 6:
		{
			WORD row;

			row = con->row;
			if(con->tstat & T_USE_REGION)
				row -= con->top - 1;

			sprintf(com, "\033[%ld;%ldR", row, con->col);
			answer = com;
		}
		break;

		case 15:
			if(VT340_openprinterFH(con) != FALSE)
				answer = "\033[?10n";
			else
				answer = "\033[?11n";
		break;

		case 25:
			answer = "\033[?21n";
		break;

		case 26:
			answer = "\033[?27;1n";
	}

	if(answer)
		con->io->xem_swrite(answer, -1);
}


VOID VT340_reportUPSS(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE *answer;

	answer = (con->pstat & P_ISO_LATIN1) ?	"\033P1!uA\033/" :	/* ISO */
														"\033P0!u%5\033/";	/* DEC */
	con->io->xem_swrite(answer, -1);
}


/* CSI Pt; Pb r	DECSTBM		Set top and bottom scrolling margins, resp.
										CSI r resets margin to full screen.
*/
VOID VT340_setregion(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->top = args[0];
	if(!con->top)
		con->top = con->firstLine;

	con->bot = args[1];
	if(!con->bot)
		con->bot = con->lastLine;
	else
	{
		if(con->bot > con->lastLine)
			con->bot = con->lastLine;
	}

	if(con->top > con->bot)
	{
		con->top = con->firstLine;
		con->bot = con->lastLine;
	}

	VT340_getTopBot(con, &con->row, NULL);
	con->col = 1;
}


/*
 CSI Pr d         CVA         ANSI Cursor to row Pr, absolute
*/
VOID VT340_setrow(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_placeCursorAt(con, args[0], con->col);
}


/*
 CSI Pc G         CHA         ANSI Cursor to absolute column Pc
*/
VOID VT340_setcol(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_placeCursorAt(con, con->row, args[0]);
}


/*
 CSI Pn a         CUF         ANSI Cursor forward Pn columns
*/
VOID VT340_moveCursForward(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	VT340_moveCursorForward(con, cnt);
}


/*
 CSI Pn e         CUD         ANSI Cursor down Pn rows
*/
VOID VT340_moveCursDown(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(!cnt)
		cnt = 1;

	VT340_moveCursorDown(con, cnt);
}


/*
 CSI Pl; Pc " p	DECSCL		Set conformance level (set terminal type)
		Pl	Pc			level selected   (also does a soft reset, not hard)
		61	0			VT102, 7-bit controls (61 is otherwise VT340 level)
		62	0 or 2	VT340, 8-bit controls (62 is otherwise VT200 level)
		62	1			VT340, 7-bit controls
		63	0 or 2 	VT340, 8-bit controls (63 is otherwise VT300 level)
		63	1			VT340, 7-bit controls
*/
VOID VT340_setserviceclass(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	/* in _resetcon() wird _cancel() ausgefuehrt, das die args auf null
		setzt.. */
	UBYTE s_args = con->args[1];


	switch(con->args[0])
	{
		case 61:
			VT340_resetcon(con, FALSE);
			con->pstat &= ~P_VT340_MODE;		/* VT-102 7 bit */
			con->pstat |= P_CONVERT;
		break;

		case 62:
		case 63:
			if(s_args == 1)		/* VT-340 7 bit */
			{
				VT340_resetcon(con, FALSE);
				con->pstat |= P_VT340_MODE;
				con->pstat |= P_CONVERT;
			}
			else
				if(s_args == 0  ||  s_args == 2)		/* VT-340 8 bit */
				{
					VT340_resetcon(con, FALSE);
					con->pstat |= P_VT340_MODE;
					con->pstat &= ~P_CONVERT;
				}
		break;

	}

	CreateOwnStatusLine(con);
}


VOID VT340_reportANSIsettings(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	ULONG mode;
	UBYTE answer[30];

	switch(args[0])
	{
		case 4:
			mode = (con->ustat & U_INSERT) ? 1 : 2;
		break;

		case 20:
			mode = (con->ustat & U_NEWLINE) ? 1 : 2;
		break;

		default:
			mode = 0;
		break;
	}
	
	sprintf(answer, "\033[%ld;%ld$y", args[0], mode);
	con->io->xem_swrite(answer, -1);
}


VOID VT340_reportDECsettings(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	ULONG mode;
	UBYTE answer[30];

	switch(args[0])
	{
		case 1:
			mode = (con->ustat & U_CURAPP) ? 1 : 2;
		break;

		case 4:
			mode = (con->pstat & P_SMOOTH_SCROLL) ? 1 : 2;
		break;

		case 6:
			mode = (con->tstat & T_USE_REGION) ? 1 : 2;
		break;

		case 7:
			mode = (con->tstat & T_WRAP) ? 1 : 2;
		break;

		case 18:
			mode = (con->gstat & G_FORMFEED) ? 1 : 2;
		break;

		case 19:
			mode = (con->gstat & G_FULLSCREEN) ? 1 : 2;
		break;

		case 25:
			mode = (con->ustat & U_CURSOR_VISIBLE) ? 1 : 2;
		break;

		case 66:
			mode = (con->ustat & U_KEYAPP) ? 1 : 2;
		break;

		default:
			mode = 0;
		break;
	}
	
	sprintf(answer, "\033[?%ld;%ld$y", args[0], mode);
	con->io->xem_swrite(answer, -1);
}


VOID VT340_setselective(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->DECSCA = (args[0] == 1) ? DONT_ERASE : ERASEABLE;
}


VOID VT340_printcontrolmode(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	switch(args[0])
	{
		case 0:
			if(VT340_openprinterFH(con) != FALSE)
			{
				PagePrintDisplay(con, con->printerHandle, TRUE);
				VT340_printterminator(con, con->printerHandle);
			}
		break;

		case 4:
			con->gstat &= ~G_PRINT_CONTROL;

			if(con->gstat & G_AUTO_PRINT_INACTIVE)
			{
				con->gstat &= ~G_AUTO_PRINT_INACTIVE;
				con->gstat |=  G_AUTO_PRINT;
			}

			VT340_printterminator(con, con->printerHandle);
			CreateOwnStatusLine(con);
		break;

		case 5:
			if(VT340_openprinterFH(con) != FALSE)
			{
				con->gstat |= G_PRINT_CONTROL;

				if(con->gstat & G_AUTO_PRINT)
				{
					con->gstat |=  G_AUTO_PRINT_INACTIVE;
					con->gstat &= ~G_AUTO_PRINT;
				}

				CreateOwnStatusLine(con);
			}
		break;

	}
}


VOID VT340_autoprintmode(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	switch(args[0])
	{
		case 1:
			if(VT340_openprinterFH(con) != FALSE)
			{
				PagePrintDisplay(con, con->printerHandle, FALSE);
				VT340_printterminator(con, con->printerHandle);
			}
		break;

		case 4:
			con->gstat &= ~(G_AUTO_PRINT | G_AUTO_PRINT_INACTIVE);
			VT340_printterminator(con, con->printerHandle);
			CreateOwnStatusLine(con);
		break;

		case 5:
			if(VT340_openprinterFH(con) != FALSE)
			{
				if(con->gstat & G_PRINT_CONTROL)
					con->gstat |= G_AUTO_PRINT_INACTIVE;
				else
				{
					con->gstat |= G_AUTO_PRINT;
					CreateOwnStatusLine(con);
				}
			}
		break;

	}
}


VOID VT340_softreset(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_resetcon(con, FALSE);
}

/*
VOID VT340_loadleds(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	BOOL update=FALSE;
	UWORD i;
	UBYTE cnt;

	for(i=0; i<(argc+1); i++)
	{
		cnt = args[i];
		if(cnt <= 4)
		{
			update = TRUE;

			if(cnt == 0)
				con->leds = 0;
			else
				con->leds |= (1 << cnt);
		}
	}

	if(update != FALSE)
		CreateOwnStatusLine(con);
}
*/
VOID VT340_reportcursorstate(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(args[0] == 1)	/* report entire terminal state */
	{
		con->io->xem_swrite("\033P1$s\033/", -1);	/* currently ignored */
	}
}


VOID VT340_reportprestate(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(args[0] == 1)	/* report entire terminal state */
	{
		con->io->xem_swrite("\033P1$u\033/", -1);	/* currently ignored */
	}
	else
	{
		if(args[0] == 2)	/* report tab stops */
		{
			BOOL gotcha;
			UWORD cnt=0, col;
			UBYTE *tabs;

			tabs = con->DCS_buffer;

			if(con->pstat & P_CONVERT)
			{
				tabs[cnt++] = ESC;
				tabs[cnt++] = 'P';
			}
			else
				tabs[cnt++] = DCS;

			for(col = 1, gotcha = FALSE; col < con->columns; col++)
			{
				if(VT340_isTab(con))
				{
					UBYTE val;

					gotcha = TRUE;
 
					if(val = col / 10)
						tabs[cnt++] = val + '0';
					
					val = col % 10;
					tabs[cnt++] = val + '0';

					tabs[cnt++] = '/';	/* Note the separator "/" occurs in
												a real VT320 but should have been ";". */
				}
			}

			if(gotcha)	/* kill last separator.. */
				cnt--;

			if(con->pstat & P_CONVERT)
			{
				tabs[cnt++] = ESC;
				tabs[cnt++] = '/';
			}
			else
				tabs[cnt++] = ST;
			
			con->io->xem_swrite(con->DCS_buffer, cnt);
		}
	}
}


VOID VT340_selectactivedisplay(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(con->ustat & U_HOST_STATUSLINE)
	{
		if(args[0] == 0)
		{
			con->ustat &= ~U_HOST_STATUSLINE_ACTIVE;
			VT340_resetscreen(con, FALSE);
			con->row = con->m_row;
			con->col = con->m_col;

			VT340_checkdims(con);
		}
		else
		{
			if(args[0] == 1)
			{
				con->m_row = con->row;
				con->m_col = con->col;
				con->ustat |= U_HOST_STATUSLINE_ACTIVE;
				VT340_resetscreen(con, FALSE);
			}
		}
	}
}


VOID VT340_setstatusline(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD y;

	y = con->statusLine * con->ySize;

	switch(args[0])
	{
		case 0:	/* no status-line */
			con->pstat &= ~P_OWN_STATUSLINE;
			con->ustat &= ~U_HOST_STATUSLINE;
			DeleteStatusLine(con, FALSE);
		break;

		case 1:	/* own status-line */
			con->pstat |= P_OWN_STATUSLINE;
			con->ustat &= ~U_HOST_STATUSLINE;
			CreateOwnStatusLine(con);
		break;

		case 2:	/* host status-line */
			con->pstat &= ~P_OWN_STATUSLINE;
			con->ustat |= U_HOST_STATUSLINE;
			DeleteStatusLine(con, FALSE);
		break;

	}
}



/* end of source-code */
