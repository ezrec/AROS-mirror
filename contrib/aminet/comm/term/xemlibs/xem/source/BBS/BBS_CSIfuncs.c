#include "ownincs/BBSconsole.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


                /* CSI Functions for the BBS-Ansi emulator.. */



VOID BBS_ignore(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	return;	/* ..(^: */
}


VOID BBS_insertchars(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	if(con->col < con->columns)
		myScrollRaster(con, -(cnt * XSIZE), 0, ((con->col - 1) * XSIZE), ((con->row-1) * YSIZE),con->border.Width - 1, ((con->row * YSIZE) - 1));
}


VOID BBS_cursorright(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	while(cnt--)
	{
		if(con->col >= con->columns)
		{
			if(con->stat & X_SCROLLCURSOR)
				BBS_cursornextline(con, "\001", 1);
			else
				break;
		}
		else
			con->col++;
	}
}


VOID BBS_cursorleft(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	while(cnt--)
	{
		if(con->col <= 1)
		{
			if(con->stat & X_SCROLLCURSOR)
			{
				con->col = con->columns;
				BBS_cursorup(con, "\001", 1);
			}
			else
				break;
		}
		else
			con->col--;
	}
}


VOID BBS_cursorup(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];

	if(cnt == 0)
		cnt = 1;

	if(con->stat & Y_SCROLLCURSOR)
	{
		if(con->row - cnt >= 1)
			con->row -= cnt;
		else
		{
			LONG ntimes;

			ntimes = (con->row > 1) ? (cnt - con->row) : cnt;
			con->row = 1;
			myScrollRaster(con, 0, -YSIZE * ntimes, 0, 0, con->border.Width - 1, (con->lines * YSIZE) - 1);
		}
	}

	else
	{
		if(con->row - cnt > 1)
			con->row -= cnt;
		else
			con->row = 1;
	}
}


VOID BBS_cursordown(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];

	if(cnt == 0)
		cnt = 1;

	if(con->stat & Y_SCROLLCURSOR)
	{
		if(con->row + cnt <= con->lines)
			con->row += cnt;
		else
		{
			LONG ntimes;

			ntimes = con->row + cnt - con->lines;
			con->row = con->lines;
			myScrollRaster(con, 0, +YSIZE * ntimes, 0, 0, con->border.Width - 1, (con->lines * YSIZE) - 1);
		}
	}
	else
	{
		if(con->row + cnt < con->lines)
			con->row += cnt;
		else
			con->row = con->lines;
	}
}


VOID BBS_cursornextline(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	con->col = 1;
	BBS_cursordown(con, args, argc);
}


VOID BBS_cursoraboveline(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	con->col = 1;
	BBS_cursorup(con, args, argc);
}


VOID BBS_cursorposition(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	con->row = args[0];
	if(con->row == 0)
		con->row = 1;
	else
	{
		if(con->row > con->lines)
			con->row = con->lines;
	}

	con->col = args[1];
	if(con->col == 0)
		con->col = 1;
	else
	{
		if(con->col > con->columns)
			con->col = con->columns;
	}
}


/*
 CSI Ps J	ED		Erase in display:
				0 = cursor to end of screen, inclusive
				1 = start of screen to cursor, inclusive
				2 = entire screen, cursor home
*/
VOID BBS_eraseindisplay(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	switch(args[0])
	{
		case 0:
			if(con->row < con->lines)
			{
				con->row++;
				BBS_eraselines(con, con->lines - con->row);
				con->row--;
			}
			BBS_erasechars(con, con->columns + 1 - con->col);
		break;

		case 1:
			BBS_erasescrbeg(con);
			BBS_eraselinebeg(con);
		break;

		case 2:
			BBS_erasescreen(con);
	}
}


/*
 CSI Ps K	EL		Erase in line:
				0 = cursor to end of line, inclusive
				1 = start of line to cursor, inclusive
				2 = entire line, cursor does not move
*/
VOID BBS_eraseinline(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	switch(args[0])
	{
		case 0:
			BBS_erasechars(con, con->columns + 1 - con->col);
		break;

		case 1:
			BBS_eraselinebeg(con);
		break;

		case 2:
			BBS_eraselinebeg(con);
			BBS_erasechars(con, con->columns + 1 - con->col);
		break;
	}
}


/*
 CSI Pn L	IL		Insert Pn lines preceding current line.
*/
VOID BBS_insertlines(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	if((con->row >= 1)  &&  (con->row <= con->lines))
	{
		if(cnt > (con->lines - con->row))
			cnt = (con->lines - con->row);

		myScrollRaster(con, 0, -(cnt * YSIZE), 0, (con->row - 1) * YSIZE, con->border.Width - 1, (con->lines * YSIZE) - 1);
	}
}


/*
 CSI Pn M	DL		Delete Pn lines from current downward, incl.
*/
VOID BBS_deletelines(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	if((con->row >= 1)  &&  (con->row <= con->lines))
	{
		if(cnt > (con->lines - con->row + 1))
			cnt = (con->lines - con->row + 1);

		myScrollRaster(con, 0, +(cnt * YSIZE), 0, (con->row - 1) * YSIZE, con->border.Width - 1, (con->lines * YSIZE) - 1);
	}
}


/*
 CSI Pn P	DCH		Delete Pn chars from cursor to left, incl.
*/
VOID BBS_deletechars(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	if(con->col < con->columns)
		myScrollRaster(con, +(cnt * XSIZE), 0, (con->col - 1) * XSIZE, (con->row - 1) * YSIZE, con->border.Width - 1, (con->row * YSIZE) - 1);
}


VOID BBS_scrollup(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	myScrollRaster(con, 0, -(cnt * YSIZE), 0, 0, con->border.Width - 1, (con->lines * YSIZE) - 1);
}


VOID BBS_scrolldown(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE cnt;

	cnt = args[0];
	if(cnt == 0)
		cnt = 1;

	myScrollRaster(con, 0, +(cnt * YSIZE), 0, 0, con->border.Width - 1, (con->lines * YSIZE) - 1);
}


VOID BBS_ANSIsetmodes(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UWORD i;

	for(i=0; i<(argc+1); i++)
	{
		switch(con->inter[i])
		{
			case NUL:
				if(args[i] == 20)
					con->stat |= NEWLINE;
			break;

			case '>':
				if(args[i] == 1)
					con->stat |= Y_SCROLLCURSOR;
			break;

			case '?':
				if(args[i] == 7)
					con->stat |= X_SCROLLCURSOR;
			break;

		}
	}
}


VOID BBS_ANSIresetmodes(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UWORD i;

	for(i=0; i<(argc+1); i++)
	{
		switch(con->inter[i])
		{
			case NUL:
				if(args[i] == 20)
					con->stat &= ~NEWLINE;
			break;

			case '>':
				if(args[i] == 1)
					con->stat &= ~Y_SCROLLCURSOR;
			break;

			case '?':
				if(args[i] == 7)
					con->stat &= ~X_SCROLLCURSOR;
			break;

		}
	}
}


STATIC ULONG GetTextStyle(struct BBSConsole *con)
{
	ULONG textStyles;

	textStyles = FS_NORMAL;

	if(con->attr & ATTR_FAT)
		textStyles |= FSF_BOLD;

	if(con->attr & ATTR_UNDERLINE)
		textStyles |= FSF_UNDERLINED;

	if(con->attr & ATTR_ITALIC)
		textStyles |= FSF_ITALIC;

	return(textStyles);
}


VOID BBS_textmodes(struct BBSConsole *con, UBYTE *args, ULONG argc)
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
				BBS_resetstyles(con, FALSE);
				oldTextStyle = GetTextStyle(con);
			break;

			case 1:
				con->attr |= (con->hiLite != FALSE) ? ATTR_HIGHLIGHT : ATTR_FAT;
			break;

			case 4:
				con->attr |= ATTR_UNDERLINE;
			break;

			case 5:
				con->attr |= ATTR_ITALIC;
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

			case 25:
				con->attr &= ~ATTR_ITALIC;
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


VOID BBS_devicestatreport(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	UBYTE com[16];
	STRPTR answer=NULL;

	if(args[0] == 6)
	{
		sprintf(com, "\233%ld;%ldR", con->row, con->col);
		answer = com;
	}

	if(answer != NULL)
		con->io->xem_swrite(answer, -1);
}


VOID BBS_savecursor(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	con->s_row	= con->row;
	con->s_col	= con->col;
	con->s_attr	= con->attr;
	con->s_apen	= con->apen;
	con->s_bpen	= con->bpen;
	con->s_shift= con->shift;
	con->s_stat	= con->stat;
}


VOID BBS_restorecursor(struct BBSConsole *con, UBYTE *args, ULONG argc)
{
	con->row	 	= con->s_row;
	con->col		= con->s_col;
	con->attr	= con->s_attr;
	con->apen	= con->s_apen;
	con->bpen	= con->s_bpen;
	con->shift	= con->s_shift;
	con->stat	= con->s_stat;

	SetAPen(con->rp, con->apen);
	SetBPen(con->rp, con->bpen);
	SetSoftStyle(con->rp, GetTextStyle(con), SYSTEM_TEXTSTYLES);
}



/* end of source-code */
