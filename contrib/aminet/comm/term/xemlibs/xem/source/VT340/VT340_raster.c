#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;



STATIC BOOL InvalidRaster(struct VT340Console *con)
{
	return((BOOL) (con->displayInactive != FALSE  ||  con->textRaster == NULL  ||
						con->textRaster == NULL        ||  con->penRaster == NULL));
}


VOID RefreshRaster(struct VT340Console *con)
{
	WORD s_row, s_col;

	s_row = con->row;
	s_col = con->col;

	VT340_erasewholescreen(con);

	if(con->firstLine <= con->lastLine  &&  con->firstLine > 0)
	{
		for(con->row=con->firstLine; con->row <= con->lastLine; con->row++)
			RasterGetLine(con);
	}

	con->row = s_row;
	con->col = s_col;
}


STATIC VOID FillRaster(struct VT340Console *con, WORD first, UWORD cnt)
{
	while(cnt)
	{
		con->textRaster[first]	= ' ';
		con->eraseRaster[first]	= ERASEABLE;
		con->penRaster[first]	= BACKGROUND_PEN;
		first++;
		cnt--;
	}
}


VOID DeleteRaster(struct VT340Console *con)
{
	if(con->textRaster != NULL)
	{
		FreeVec(con->textRaster);
		con->textRaster = NULL;
	}

	if(con->eraseRaster != NULL)
	{
		FreeVec(con->eraseRaster);
		con->eraseRaster = NULL;
	}
}


BOOL CreateRaster(struct VT340Console *con)
{
	if(con->displayInactive != FALSE)
		return(TRUE);


	con->textRaster	= NULL;
	con->eraseRaster	= NULL;
	con->penRaster		= NULL;

	con->rasterWidth	 = con->io->xem_window->WScreen->Width >> 3;
	con->rasterHeight = con->io->xem_window->WScreen->Height >> 3;

	con->textRaster = AllocVec((con->rasterWidth * con->rasterHeight) * sizeof(UBYTE), MEMF_ANY | MEMF_CLEAR);
	if(con->textRaster == NULL)
		return(FALSE);

	con->eraseRaster = AllocVec((con->rasterWidth * con->rasterHeight) * sizeof(UBYTE), MEMF_ANY | MEMF_CLEAR);
	if(con->eraseRaster == NULL)
		return(FALSE);

	con->penRaster = AllocVec((con->rasterWidth * con->rasterHeight) * sizeof(UWORD), MEMF_ANY | MEMF_CLEAR);
	if(con->penRaster == NULL)
		return(FALSE);

	return(TRUE);
}


VOID RasterEraseScreen(struct VT340Console *con, UWORD mode)
{
	WORD first, last;

	if(InvalidRaster(con) != FALSE)
		return;

	switch(mode)
	{
		case 2:		/* entire screen */
			first	= (con->firstLine - 1) * con->rasterWidth;
			last	= con->lastLine * con->rasterWidth;
		break;

		
		case 1:		/* start of screen to cursor, inclusive */
			first = (con->firstLine - 1) * con->rasterWidth;
			last	= (con->row - 1) * con->rasterWidth + (con->col - 1); 
		break;

		default:		/* cursor to end of screen, inclusive */
			first	= (con->row - 1) * con->rasterWidth + (con->col-1);
			last	= con->lastLine * con->rasterWidth;
		break;

	}

	if(last > first)
		FillRaster(con, first, last - first);
}


VOID RasterEraseLine(struct VT340Console *con, UWORD mode)
{
	WORD first, last;

	if(InvalidRaster(con) != FALSE)
		return;

	switch(mode)
	{
		case 2:		/* entire line */
			first	= (con->row-1) * con->rasterWidth;
			last	= first + con->rasterWidth;
		break;

		case 1:		/* start of line to cursor, inclusive */
			first	= (con->row-1) * con->rasterWidth;
			last	= first + con->col;
		break;

		default:		/* cursor to end of line, inclusive */
			first	= (con->row-1) * con->rasterWidth + (con->col-1);
			last	= con->row * con->rasterWidth;
		break;

	}

	if(last > first)
		FillRaster(con, first, last - first);
}


VOID RasterSelectiveEraseScreen(struct VT340Console *con, UWORD mode)
{
	WORD first, last, s_row;
	UWORD i;

	if(InvalidRaster(con) != FALSE)
		return;

	switch(mode)
	{
		default:
		case 0:		/* cursor to end of screen, inclusive */
			RasterSelectiveEraseLine(con, 0);
			if(con->row == con->lastLine)
				return;
			first	= con->row + 1;
			last	= con->lastLine + 1;
		break;

		case 1:		/* start of screen to cursor, inclusive */
			RasterSelectiveEraseLine(con, 1);
			if(con->row == 1)
				return;
			first = con->firstLine;
			last	= con->row;
		break;

		case 2:		/* entire screen */
			first = con->firstLine;
			last	= con->lastLine + 1;
		break;

	}

	s_row = con->row;
	for(i=first; i<last; i++)
	{
		con->row = i;
		RasterSelectiveEraseLine(con, 2);
	}
	con->row = s_row;
}


VOID RasterSelectiveEraseLine(struct VT340Console *con, UWORD mode)
{
	UWORD cnt, chars, i;
	WORD first, prevpos, s_col;
	BOOL erase;
	UBYTE *rasterptr;

	if(InvalidRaster(con) != FALSE)
		return;

	switch(mode)
	{
		default:
		case 0:		/* cursor to end of line, inclusive */
			first	= con->col - 1;
			cnt	= con->rasterWidth;
		break;

		case 1:		/* start of line to cursor, inclusive */
			first	= 0;
			cnt	= con->col;
		break;

		case 2:		/* entire line */
			first	= 0;
			cnt	= con->rasterWidth;
		break;

	}

	chars = 0; 
	prevpos = first + 1;
	erase = FALSE;
	rasterptr = &con->eraseRaster[(con->row-1) * con->rasterWidth];

	for(i=first; i<cnt; i++)
	{
		if(rasterptr[i] == ERASEABLE)
		{
			chars++;
			if(erase == FALSE)
			{
				erase = TRUE;
				prevpos = i + 1;
			}
		}
		else
		{
			erase = FALSE;
			if(chars)
			{
				s_col = con->col;
				con->col = prevpos;
				RasterEraseCharacters(con, chars);
				con->col = s_col;
				chars = 0;
			}
		}
	}

	if(chars)
	{
		s_col = con->col;
		con->col = prevpos;
		RasterEraseCharacters(con, chars);
		con->col = s_col;
	}

	s_col = con->col;
	con->col = first + 1;
	RasterGetLine(con);
	con->col = s_col;
}


VOID RasterEraseCharacters(struct VT340Console *con, UWORD chars)
{
	WORD first;

	if(InvalidRaster(con) != FALSE)
		return;

/* Error-Checking MUSS im Caller geschehen, da diese Routine nicht weiss,
   wie breit der Screen ist!  Bsp: siehe VT340_erasechars() */

	first	= (con->row-1) * con->rasterWidth + (con->col-1);

	FillRaster(con, first, chars);
/*
	memset(&con->textRaster[first], ' ', chars);
	memset(&con->eraseRaster[first], ERASEABLE, chars);
	memset(&con->penRaster[first], BACKGROUND_PEN, chars<<1);
*/
}


VOID RasterScrollRegion(struct VT340Console *con, WORD direction, WORD top, WORD bot)
{
	UWORD lines, diff, max;

	if(InvalidRaster(con) != FALSE)
		return;

/* top & bot zählen von null an */

	diff	= abs(direction);

	if(top + diff > bot)
		diff = bot - top;


	lines = bot - top - diff;
	max	= lines * con->rasterWidth;

	if(direction < 0)	/* scroll up */
	{
		UWORD i;
		WORD first, last;
		UBYTE *e_firstptr, *e_lastptr;
		UBYTE *t_firstptr, *t_lastptr;
		UWORD *p_firstptr, *p_lastptr;

		first	= (top + lines) * con->rasterWidth - 1;
		last	= (top + lines + diff) * con->rasterWidth - 1;

		e_firstptr	= &con->eraseRaster[first];
		e_lastptr	= &con->eraseRaster[last];
		t_firstptr	= &con->textRaster[first];
		t_lastptr	= &con->textRaster[last];
		p_firstptr	= &con->penRaster[first];
		p_lastptr	= &con->penRaster[last];

		i = 0;
		while(i++ < max)
		{
			*e_lastptr-- = *e_firstptr--;
			*t_lastptr-- = *t_firstptr--;
			*p_lastptr-- = *p_firstptr--;
		}

		FillRaster(con, top * con->rasterWidth, diff * con->rasterWidth);
/*
		memset(&con->textRaster[top * con->rasterWidth], ' ', diff * con->rasterWidth);
		memset(&con->eraseRaster[top * con->rasterWidth], ERASEABLE, diff * con->rasterWidth);
		memset(&con->penRaster[top * con->rasterWidth], BACKGROUND_PEN, (diff * con->rasterWidth)<<1);
*/
	}
	else	/* scroll down */
	{
		UWORD i;
		WORD first, last;
		UBYTE *e_firstptr, *e_lastptr;
		UBYTE *t_firstptr, *t_lastptr;
		UWORD *p_firstptr, *p_lastptr;

		first	= top * con->rasterWidth + (con->rasterWidth * diff);
		last	= top * con->rasterWidth;

		e_firstptr	= &con->eraseRaster[first];
		e_lastptr	= &con->eraseRaster[last];
		t_firstptr	= &con->textRaster[first];
		t_lastptr	= &con->textRaster[last];
		p_firstptr	= &con->penRaster[first];
		p_lastptr	= &con->penRaster[last];
		i = 0;
		while(i++ < max)
		{
			*e_lastptr++ = *e_firstptr++;
			*t_lastptr++ = *t_firstptr++;
			*p_lastptr++ = *p_firstptr++;
		}

		FillRaster(con, (bot-diff) * con->rasterWidth, diff * con->rasterWidth);
/*
		memset(&con->textRaster[(bot - diff) * con->rasterWidth], ' ', diff * con->rasterWidth);
		memset(&con->eraseRaster[(bot - diff) * con->rasterWidth], ERASEABLE, diff * con->rasterWidth);
		memset(&con->penRaster[bot - diff], BACKGROUND_PEN, (diff * con->rasterWidth)<<1);
*/
	}
}


VOID RasterShiftChars(struct VT340Console *con, WORD direction, WORD row, WORD col)
{
	WORD diff, max, top;

	if(InvalidRaster(con) != FALSE)
		return;

	diff = abs(direction);
	if(col + diff > con->rasterWidth)
		diff = con->rasterWidth - col;

	top	= row * con->rasterWidth; 
	max	= con->rasterWidth - col - diff;

	if(direction < 0)	/* scroll right */
	{
		UWORD i;
		WORD first, last;
		UBYTE *e_firstptr, *e_lastptr;
		UBYTE *t_firstptr, *t_lastptr;
		UWORD *p_firstptr, *p_lastptr;

		first	= con->rasterWidth - diff - 1;
		last	= con->rasterWidth - 1;

		e_firstptr	= &con->eraseRaster[top + first];
		e_lastptr	= &con->eraseRaster[top + last];
		t_firstptr	= &con->textRaster[top + first];
		t_lastptr	= &con->textRaster[top + last];
		p_firstptr	= &con->penRaster[top + first];
		p_lastptr	= &con->penRaster[top + last];

		i = 0;
		while(i++ < max)
		{
			*e_lastptr-- = *e_firstptr--;
			*t_lastptr-- = *t_firstptr--;
			*p_lastptr-- = *p_firstptr--;
		}
		FillRaster(con, top + col, diff);
/*
		memset(&con->textRaster[top + col], ' ', diff);
		memset(&con->eraseRaster[top + col], ERASEABLE, diff);
		memset(&con->penRaster[top + col], BACKGROUND_PEN, diff<<1);
*/
	}
	else	/* scroll left */
	{
		UWORD i;
		WORD first, last;
		UBYTE *e_firstptr, *e_lastptr;
		UBYTE *t_firstptr, *t_lastptr;
		UWORD *p_firstptr, *p_lastptr;

		first	= col + diff;
		last	= col;

		e_firstptr	= &con->eraseRaster[top + first];
		e_lastptr	= &con->eraseRaster[top + last];
		t_firstptr	= &con->textRaster[top + first];
		t_lastptr	= &con->textRaster[top + last];
		p_firstptr	= &con->penRaster[top + first];
		p_lastptr	= &con->penRaster[top + last];

		i = 0;
		while(i++ < max)
		{
			*e_lastptr++ = *e_firstptr++;
			*t_lastptr++ = *t_firstptr++;
			*p_lastptr++ = *p_firstptr++;
		}
		FillRaster(con, top + con->rasterWidth - diff, diff);
/*
		memset(&con->textRaster[top + con->rasterWidth - diff], ' ', diff);
		memset(&con->eraseRaster[top + con->rasterWidth - diff], ERASEABLE, diff);
		memset(&con->penRaster[top + con->rasterWidth - diff], BACKGROUND_PEN, diff<<1);
*/
	}
}


VOID RasterPutString(struct VT340Console *con, STRPTR string, UWORD lenght, UWORD xPos, UWORD yPos)
{
	ULONG linePos;

	if(InvalidRaster(con) != FALSE)
		return;

/* xPos & yPos count from 0 */

	linePos = yPos * con->rasterWidth + xPos;

	if(lenght == -1)
		lenght = strlen(string);

	if(lenght == 1)
	{
		if(xPos <= con->rasterWidth - 1)
		{
			con->textRaster[linePos]	= *string;
			con->eraseRaster[linePos]	= con->DECSCA;
		}
	}
	else
	{
		if(xPos + lenght >= con->rasterWidth - 1)
			lenght = con->rasterWidth - 1 - xPos;

		CopyMem(string, &con->textRaster[linePos], lenght);
		memset(&con->eraseRaster[linePos], con->DECSCA, lenght);
	}

	while(lenght)
	{
		con->penRaster[linePos] = ATTRIBUTES_WORD;
		linePos++;
		lenght--;
	}
/*	KPrintF("%ld\r\n", con->penRaster[linePos]);*/
}


VOID RasterGetLine(struct VT340Console *con)
{
	UWORD chars;
	UBYTE width;

	if(InvalidRaster(con) != FALSE)
		return;

	chars = con->border.Width >> 3;
	width	= *(con->rows + con->row);

	if(width)
	{
		chars >>= 1;
		PrintScaled(con, &con->textRaster[(con->row-1) * con->rasterWidth], chars, 1, con->row, width, TRUE);
	}
	else
	{
		STRPTR ptr;
		BOOL flush, newAPen, newBPen, newStyle;
		UWORD cnt, offset;
		WORD left, top;
		UBYTE sapen, sbpen, pen, textStyle;

		left	= 1;
		top	= con->row;
		ptr	= &con->textRaster[(con->row-1) * con->rasterWidth];
		cnt	= 0;
		offset= (con->row-1) * con->rasterWidth;
		sapen	= ATTRIBUTES_APEN(con->penRaster[offset]);
		sbpen	= ATTRIBUTES_BPEN(con->penRaster[offset]);
		textStyle = ATTRIBUTES_STYLE(con->penRaster[offset]);


		SetAPen(con->rp, sapen);
		SetBPen(con->rp, sbpen);
		SetSoftStyle(con->rp, textStyle, SYSTEM_TEXTSTYLES);

		flush		= FALSE;
		newAPen	= FALSE;
		newBPen	= FALSE;
		newStyle	= FALSE;
		while(chars)
		{

		/*
		** ATTR_HIGHLIGHT && ATTR_INVERSE are
		** handled via apen/bpen..(-:
		*/

			if((textStyle & FSF_UNDERLINED)  &&  !(con->penRaster[offset] & ATTR_UNDERLINE))
			{
				flush = TRUE;
				textStyle &= ~FSF_UNDERLINED;
				newStyle = TRUE;
			}
			else
			{
				if(!(textStyle & FSF_UNDERLINED)  &&  (con->penRaster[offset] & ATTR_UNDERLINE))
				{
					flush = TRUE;
					textStyle |= FSF_UNDERLINED;
					newStyle = TRUE;
				}
			}

			if((textStyle & FSF_BOLD)  &&  !(con->penRaster[offset] & ATTR_FAT))
			{
				flush = TRUE;
				textStyle &= ~FSF_BOLD;
				newStyle = TRUE;
			}
			else
			{
				if(!(textStyle & FSF_BOLD)  &&  (con->penRaster[offset] & ATTR_FAT))
				{
					flush = TRUE;
					textStyle |= FSF_BOLD;
					newStyle = TRUE;
				}
			}

			pen = ATTRIBUTES_APEN(con->penRaster[offset]);
			if(sapen != pen)
			{
				flush = TRUE;
				sapen = pen;
				newAPen= TRUE;
			}

			pen = ATTRIBUTES_BPEN(con->penRaster[offset]);
			if(sbpen != pen)
			{
				flush = TRUE;
				sbpen = pen;
				newBPen= TRUE;
			}


			if(flush != FALSE)
			{
				flush = FALSE;

				myMove_Text(con, left, top, ptr, cnt);

				left	+= cnt;
				ptr	+= cnt;
				cnt = 0;
			}

			cnt++;
			chars--;
			offset++;


			if(newStyle != FALSE)
			{
				newStyle = FALSE;
				SetSoftStyle(con->rp, textStyle, SYSTEM_TEXTSTYLES);
			}

			if(newAPen != FALSE)
			{
				newAPen = FALSE;
				SetAPen(con->rp, sapen);
			}

			if(newBPen != FALSE)
			{
				newBPen = FALSE;
				SetBPen(con->rp, sbpen);
			}
		}

		if(cnt)
			myMove_Text(con, left, top, ptr, cnt);


		SetAPen(con->rp, con->apen);
		SetBPen(con->rp, con->bpen);
		SetSoftStyle(con->rp, GetTextStyle(con), SYSTEM_TEXTSTYLES);
	}
}



/* end of source-code */
