#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


              /* emulator independend page/display manager */


struct PageLine {
	struct Node pl_Link;

/* Raster */

	UBYTE	pl_TextRaster[MAXCOLUMNS];
	UBYTE	pl_EraseRaster[MAXCOLUMNS];
	UWORD	pl_AttributesRaster[MAXCOLUMNS];

/* Line Attributes */

	WORD	pl_FirstCol;
	WORD	pl_LastCol;
	UBYTE pl_LinePens;

	UBYTE	pl_LineMode;
};

STATIC BOOL InvalidPage(struct VT340Console *con);
STATIC VOID SetTopDisplayLine(struct VT340Console *con);
STATIC struct PageLine *GetCurrentLine(struct VT340Console *con);
STATIC struct PageLine *GetLineByRow(struct VT340Console *con, UWORD row);
STATIC VOID memset_word(UWORD *array, UWORD value, UWORD chars);
STATIC VOID EraseCharacters(struct PageLine *line, WORD col, UWORD chars);
STATIC VOID MoveCharacters(struct PageLine *line, WORD colTo, WORD colFrom, UWORD chars);
STATIC VOID ResetLine(struct PageLine *line);
STATIC VOID ClearPages(struct VT340Console *con);
STATIC VOID AdjustPages(struct VT340Console *con, UBYTE pages);
STATIC VOID PageEraseLineBeg(struct VT340Console *con);
STATIC VOID PageEraseLineEnd(struct VT340Console *con);
STATIC VOID PageEraseScrBeg(struct VT340Console *con);
STATIC VOID PageEraseActiveScreen(struct VT340Console *con);
STATIC VOID PagePutString(struct VT340Console *con, STRPTR string, UWORD chars, WORD col);
STATIC VOID PageSetTurboMask(struct VT340Console *con);
STATIC VOID PageSetDefaultMask(struct VT340Console *con);
STATIC BOOL PageGetTurboDims(struct VT340Console *con, WORD *top, WORD *bot, WORD *left, WORD *right, WORD direction);


UBYTE PageGetLineMode(struct VT340Console *con)
{
	struct PageLine *line;

	line = GetCurrentLine(con);
	return(line->pl_LineMode);
}


VOID RefreshPage(struct VT340Console *con)
{
	WORD s_row, s_col;

	s_row = con->row;
	s_col = con->col;

	mySetRast(con);

	if(con->firstLine <= con->lastLine  &&  con->firstLine > 0)
	{
		for(con->row=con->firstLine; con->row <= con->lastLine; con->row++)
			PageGetLine(con);
	}

	con->row = s_row;
	con->col = s_col;
}


VOID PageEraseCharacters(struct VT340Console *con, WORD col, UWORD chars)
{
	struct PageLine *line;

	line = GetCurrentLine(con);
	EraseCharacters(line, col, chars);
}


VOID DeletePage(struct VT340Console *con)
{
	if(con->pageBuffer != NULL)
	{
		FreeVec(con->pageBuffer);
		con->pageBuffer = NULL;
	}

	ClearPages(con);
}


BOOL CreatePage(struct VT340Console *con)
{
	if(con->displayInactive != FALSE)
		return(TRUE);

	ClearPages(con);

	con->pageBuffer = AllocVec(MAXROWS * sizeof(struct PageLine), MEMF_ANY | MEMF_CLEAR);
	if(con->pageBuffer == NULL)
		return(FALSE);

	AdjustPages(con, 4);

	return(TRUE);
}


VOID PageEraseInDisplay(struct VT340Console *con, UWORD mode)
{
	struct PageLine *pageLine;
	UWORD lines;

	if(InvalidPage(con) != FALSE)
		return;

	switch(mode)
	{
		case 2:		/* entire screen */
			PageEraseActiveScreen(con);

			pageLine = con->topDisplayLine;
			lines = con->lastLine;
		break;

		case 1:		/* start of screen to cursor, inclusive */
			PageEraseScrBeg(con);
			PageEraseLineBeg(con);

			PageEraseInLine(con, 1);
			if(con->row > con->firstLine)
			{
				lines = con->row - con->firstLine;
				pageLine = con->topDisplayLine;
			}
			else
				return;
		break;

		default:		/* cursor to end of screen, inclusive */
			PageEraseLineEnd(con);
			PageEraseLines(con, con->row, con->lastLine - con->row);

			PageEraseInLine(con, 0);
			if(con->row < con->lastLine)
			{
				lines = con->lastLine - con->row;
				pageLine = GetCurrentLine(con);
				pageLine = (struct PageLine *)pageLine->pl_Link.ln_Succ;
			}
			else
				return;
		break;

	}

	while(lines--)
	{
		if(pageLine == NULL)
			break;

		ResetLine(pageLine);
		pageLine = (struct PageLine *)pageLine->pl_Link.ln_Succ;
	}
}


VOID PageEraseInLine(struct VT340Console *con, UWORD mode)
{
	struct PageLine *line;
	WORD col;
	UWORD chars;

	if(InvalidPage(con) != FALSE)
		return;

	switch(mode)
	{
		case 2:		/* entire line */
			PageEraseLines(con, con->row-1, 1);

			chars = MAXCOLUMNS;
			col = 1;
		break;

		case 1:		/* start of line to cursor, inclusive */
			PageEraseLineBeg(con);

			chars = con->col;
			col = 1;
		break;

		default:		/* cursor to end of line, inclusive */
			PageEraseLineEnd(con);

			chars = MAXCOLUMNS - (con->col-1);
			col = con->col;
		break;

	}

	line = GetCurrentLine(con);
	EraseCharacters(line, col, chars);
}


VOID PageSelectiveEraseScreen(struct VT340Console *con, UWORD mode)
{
	WORD first, last, s_row;
	UWORD i;

	if(InvalidPage(con) != FALSE)
		return;

	switch(mode)
	{
		default:
		case 0:		/* cursor to end of screen, inclusive */
			PageSelectiveEraseLine(con, 0);
			if(con->row == con->lastLine)
				return;
			first	= con->row + 1;
			last	= con->lastLine + 1;
		break;

		case 1:		/* start of screen to cursor, inclusive */
			PageSelectiveEraseLine(con, 1);
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
		PageSelectiveEraseLine(con, 2);
	}
	con->row = s_row;
}


VOID PageSelectiveEraseLine(struct VT340Console *con, UWORD mode)
{
	struct PageLine *line;
	BOOL setPrevCol;
	UWORD cnt, chars, i;
	WORD col, prevCol;

	if(InvalidPage(con) != FALSE)
		return;

	switch(mode)
	{
		case 2:		/* entire line */
			chars = MAXCOLUMNS;
			col = 1;
		break;

		case 1:		/* start of line to cursor, inclusive */
			chars = con->col;
			col = 1;
		break;

		default:		/* cursor to end of line, inclusive */
			chars = MAXCOLUMNS - (con->col-1);
			col = con->col;
		break;

	}

	cnt	= 0; 
	line	= GetCurrentLine(con);
	prevCol	= col;
	setPrevCol = FALSE;

	for(i=col-1; i<chars; i++)
	{
		if(line->pl_EraseRaster[i] == ERASEABLE)
		{
			cnt++;
			if(setPrevCol != FALSE)
			{
				setPrevCol = FALSE;
				prevCol = i+1;
			}
		}
		else
		{
			if(cnt != 0)
			{
				EraseCharacters(line, prevCol, cnt);
				cnt = 0;
			}

			setPrevCol = TRUE;
		}
	}

	if(cnt != 0)
		EraseCharacters(line, prevCol, cnt);

	PageGetLine(con);
}


VOID PagePageRegion(struct VT340Console *con, WORD direction, WORD top, WORD bot)
{
}


VOID PageScrollRegion(struct VT340Console *con, WORD direction, WORD top, WORD bot)
{
	struct PageLine *topLine, *botLine, *tmpLine;
	UWORD diff;

	if(InvalidPage(con) != FALSE)
		return;

	topLine = GetLineByRow(con, top);
	botLine = GetLineByRow(con, bot);
	diff = abs(direction);

	top--;

	if(direction < 0)	/* scroll up */
	{
		if(top + diff >= bot)
		{
			for(; top<bot; top++)
			{
				ResetLine(topLine);
				topLine = (struct PageLine *)topLine->pl_Link.ln_Succ;
			}
		}
		else
		{
			UWORD i;

			for(i=0; i<diff; i++)
			{
				tmpLine = (struct PageLine *)botLine->pl_Link.ln_Pred;
				Remove(&botLine->pl_Link);
				ResetLine(botLine);
				Insert(&con->page[con->currentPage], &botLine->pl_Link, topLine->pl_Link.ln_Pred);
				topLine = botLine;
				botLine = tmpLine;
			}
		}
	}
	else	/* scroll down */
	{
		if(bot - diff <= top)
		{
			for(; top<bot; top++)
			{
/*				FPrintf(Output(), "%ld\n", top);*/
				ResetLine(topLine);
				topLine = (struct PageLine *)topLine->pl_Link.ln_Succ;
			}
		}
		else
		{
			UWORD i;

			for(i=0; i<diff; i++)
			{
				tmpLine = (struct PageLine *)topLine->pl_Link.ln_Succ;
				Remove(&topLine->pl_Link);
				ResetLine(topLine);
				Insert(&con->page[con->currentPage], &topLine->pl_Link, &botLine->pl_Link);
				botLine = topLine;
				topLine = tmpLine;
			}
		}
	}

	SetTopDisplayLine(con);
}


VOID PageScrollDisplay(struct VT340Console *con, WORD direction, WORD oldTop, WORD oldBot)
{
/* delta X == 0 */
	UWORD x1, y1, x2, y2;
	WORD top, bot, left, right;

	left	= 1;
	top	= oldTop;
	right	= con->columns;
	bot	= oldBot;

	if(PageGetTurboDims(con, &top, &bot, &left, &right, direction) == FALSE)
	{
/*		DisplayBeep(NULL);*/
		return;
	}
/*
	/* zum debuggen, legt ein Rechteck um den zu scrollenden Text */

	x1 = ((left - 1) << 3) + con->border.Left;
	y1 = ((top - 1) * con->ySize) + con->border.Top;
	x2 = ((right << 3) - 1) + con->border.Left;
	y2 = (bot * con->ySize - 1) + con->border.Top;
	Move(con->io->xem_window->RPort, x1, y1);
	Draw(con->io->xem_window->RPort, x1, y2);
	Draw(con->io->xem_window->RPort, x2, y2);
	Draw(con->io->xem_window->RPort, x2, y1);
	Draw(con->io->xem_window->RPort, x1, y1);
	return;
*/

	PageScrollRegion(con, direction, oldTop, oldBot);

	PageSetTurboMask(con);

	x1 = (left - 1) << 3;
	y1 = (top - 1) * con->ySize;
	x2 = (right << 3) - 1;
	y2 = (bot * con->ySize) - 1;

	if(con->pstat & P_SMOOTH_SCROLL)
	{
		UWORD i, cnt;

		cnt = abs(direction) * con->ySize;

		for(i=0; i<cnt; i++)
		{
			WaitTOF();
			myScrollRaster(con, 0, direction, x1, y1, x2, y2);
		}
	}
	else
		myScrollRaster(con, 0, direction * con->ySize, x1, y1, x2, y2);

	PageSetDefaultMask(con);
}


VOID PageShiftChars(struct VT340Console *con, WORD direction)
{
	struct PageLine *line;
	UWORD diff;

	if(InvalidPage(con) != FALSE)
		return;

	line = GetCurrentLine(con);
	diff = abs(direction);

	if(con->col + diff >= MAXCOLUMNS)
	{
		EraseCharacters(line, con->col, MAXCOLUMNS - (con->col-1));
		return;
	}

	if(direction < 0)	/* scroll right */
	{
		MoveCharacters(line, MAXCOLUMNS-1, (MAXCOLUMNS-1)-diff, MAXCOLUMNS - (con->col-1) - diff);
		EraseCharacters(line, con->col, diff);
	}
	else	/* scroll left */
	{
		MoveCharacters(line, con->col, con->col + diff, MAXCOLUMNS - (con->col-1) - diff);
		EraseCharacters(line, MAXCOLUMNS - diff, diff);
	}
}


STATIC VOID PagePutString(struct VT340Console *con, STRPTR string, UWORD chars, WORD col)
{
	struct PageLine *line;

	if(InvalidPage(con) != FALSE)
		return;


	col--;		/* first col == 1 */
	line = GetCurrentLine(con);

	if(chars == -1)
		chars = strlen(string);

	if(col + chars > MAXCOLUMNS)
		chars = MAXCOLUMNS - col;

	if(chars == 1)
	{
		line->pl_TextRaster[col] = *string;
		line->pl_EraseRaster[col] = con->DECSCA;
		line->pl_AttributesRaster[col] = ATTRIBUTES_WORD;
	}
	else
	{
		memcpy(&line->pl_TextRaster[col], string, chars);
		memset(&line->pl_EraseRaster[col], con->DECSCA, chars);
		memset_word(&line->pl_AttributesRaster[col], ATTRIBUTES_WORD, chars);
	}
}


VOID PageGetLine(struct VT340Console *con)
{
	struct PageLine *line;
	UWORD chars;
	UBYTE width;

	if(InvalidPage(con) != FALSE)
		return;

	line = GetCurrentLine(con);

	chars = con->border.Width >> 3;
	width	= PageGetLineMode(con);

	if(width != SCALE_ATTR_NORMAL)
	{
		chars >>= 1;
		PrintScaled(con, line->pl_TextRaster, chars, 1, con->row, width, TRUE);
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
		ptr	= line->pl_TextRaster;
		cnt	= 0;
		offset= 0;
		sapen	= ATTRIBUTES_APEN(line->pl_AttributesRaster[offset]);
		sbpen	= ATTRIBUTES_BPEN(line->pl_AttributesRaster[offset]);
		textStyle = ATTRIBUTES_STYLE(line->pl_AttributesRaster[offset]);


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

			if((textStyle & FSF_UNDERLINED)  &&  !(line->pl_AttributesRaster[offset] & ATTR_UNDERLINE))
			{
				flush = TRUE;
				textStyle &= ~FSF_UNDERLINED;
				newStyle = TRUE;
			}
			else
			{
				if(!(textStyle & FSF_UNDERLINED)  &&  (line->pl_AttributesRaster[offset] & ATTR_UNDERLINE))
				{
					flush = TRUE;
					textStyle |= FSF_UNDERLINED;
					newStyle = TRUE;
				}
			}

			if((textStyle & FSF_BOLD)  &&  !(line->pl_AttributesRaster[offset] & ATTR_FAT))
			{
				flush = TRUE;
				textStyle &= ~FSF_BOLD;
				newStyle = TRUE;
			}
			else
			{
				if(!(textStyle & FSF_BOLD)  &&  (line->pl_AttributesRaster[offset] & ATTR_FAT))
				{
					flush = TRUE;
					textStyle |= FSF_BOLD;
					newStyle = TRUE;
				}
			}

			pen = ATTRIBUTES_APEN(line->pl_AttributesRaster[offset]);
			if(sapen != pen)
			{
				flush = TRUE;
				sapen = pen;
				newAPen= TRUE;
			}

			pen = ATTRIBUTES_BPEN(line->pl_AttributesRaster[offset]);
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


               /** Private Functions (they're mine..!!) **/

VOID PagePrintDisplay(struct VT340Console *con, BPTR handle, BOOL wholeScreen)
{
	struct PageLine *line;
	STRPTR string;
	UWORD i;
	WORD top, bot, len;

	if(wholeScreen != FALSE)
	{
		top = (con->gstat & G_FULLSCREEN) ? con->firstLine : con->top;
		bot = (con->gstat & G_FULLSCREEN) ? con->lastLine  : con->bot;
	}
	else
	{
		top = con->row;
		bot = con->row;
	}

	top = 1;
	bot = con->linesPerPage;

	line = GetLineByRow(con, top);
	for(i=top-1; i<bot; i++)
	{
		string = line->pl_TextRaster;
		line = (struct PageLine *)line->pl_Link.ln_Succ;

		len = MAXCOLUMNS-1;

		while(len >= 0  &&  string[len] == ' ')
			len--;

		if(len >= 0)
		{
			if(!FWrite(handle, string, len+1, 1))
				goto bibi;	/* nicht immer, aber immer öfters..(-; */
		}

		if(!FWrite(handle, "\n", 1, 1))
			break;
	}

bibi:
	;
}

/*********************/
STATIC BOOL InvalidPage(struct VT340Console *con)
{
	return((BOOL)(con->displayInactive != FALSE  ||  con->pageBuffer == NULL));
}


STATIC VOID SetTopDisplayLine(struct VT340Console *con)
{
	con->topDisplayLine = GetHead(&con->page[con->currentPage]);
}


STATIC struct PageLine *GetCurrentLine(struct VT340Console *con)
{
	struct PageLine *line;
	WORD row;

	line	= con->topDisplayLine;
	row	= con->row;

	while(--row)
		line = (struct PageLine *)line->pl_Link.ln_Succ;

	return(line);
}


STATIC struct PageLine *GetLineByRow(struct VT340Console *con, UWORD row)
{
	struct PageLine *line;

	line	= con->topDisplayLine;

	while(--row)
		line = (struct PageLine *)line->pl_Link.ln_Succ;

	return(line);
}


STATIC VOID memset_word(UWORD *array, UWORD value, UWORD chars)
{
	while(chars--)
		*array++ = value;
}


STATIC VOID EraseCharacters(struct PageLine *line, WORD col, UWORD chars)
{
	col--;		/* first col == 1 */
	memset(&line->pl_TextRaster[col], ' ', chars);
	memset(&line->pl_EraseRaster[col], ERASEABLE, chars);
	memset_word(&line->pl_AttributesRaster[col], BACKGROUND_PEN, chars);
}


STATIC VOID MoveCharacters(struct PageLine *line, WORD colTo, WORD colFrom, UWORD chars)
{
	UWORD i;

	/* home == 1/1 */
	colTo--;
	colFrom--;

	if(colTo > colFrom)
	{
		for(i=0; i<chars; i++, colTo--, colFrom--)
		{
			line->pl_TextRaster[colTo]			= line->pl_TextRaster[colFrom];
			line->pl_EraseRaster[colTo]		= line->pl_EraseRaster[colFrom];
			line->pl_AttributesRaster[colTo]	= line->pl_AttributesRaster[colFrom];
		}
	}
	else
	{
		if(colTo < colFrom)
		{
			for(i=0; chars--; i++, colTo++, colFrom++)
			{
				line->pl_TextRaster[colTo]			= line->pl_TextRaster[colFrom];
				line->pl_EraseRaster[colTo]		= line->pl_EraseRaster[colFrom];
				line->pl_AttributesRaster[colTo]	= line->pl_AttributesRaster[colFrom];
			}
		}
/*		else
			colTo == colFrom
*/	}
}


STATIC VOID ResetLine(struct PageLine *line)
{
	/* don't touch ->pl_Link..!! */

	EraseCharacters(line, 1, MAXCOLUMNS);

	line->pl_FirstCol	= MAXCOLUMNS;
	line->pl_LastCol	= 1;
	line->pl_LinePens	= BACKGROUND_PEN;
	line->pl_LineMode	= SCALE_ATTR_NORMAL;
}


STATIC VOID ClearPages(struct VT340Console *con)
{
	NewList(&con->page[0]);
	NewList(&con->page[1]);
	NewList(&con->page[2]);
	NewList(&con->page[3]);
	NewList(&con->page[4]);
	NewList(&con->page[5]);
}


STATIC VOID AdjustPages(struct VT340Console *con, UBYTE pages)
{
	struct List *pageList;
	struct PageLine *pageBuffer;
	UBYTE i, j;

	ClearPages(con);

	if(pages > 6  ||  pages == 3  ||  pages == 5)
		pages = 4;

	con->pagesInUse	= pages;	
	con->linesPerPage	= MAXROWS / pages;
	con->currentPage	= 0;

	pageBuffer = con->pageBuffer;

	for(i=0; i<con->pagesInUse; i++)
	{
		pageList = &con->page[i];

		for(j=0; j<con->linesPerPage; j++)
		{
			ResetLine(pageBuffer);
			AddTail(pageList, &pageBuffer->pl_Link);
			pageBuffer++;
		}
	}

	SetTopDisplayLine(con);
}






/*
**	termScale.c
**
**	Single scaled character output routines
**
**	Copyright © 1990-1992 by Olaf `Olsen' Barthel & MXM
**		All Rights Reserved
*/


                      /* `Olsen' for president ;-) */


	/* DeleteScale():
	 *
	 *	Frees all the data associated with font scaling.
	 */

VOID DeleteScale(struct VT340Console *con)
{
	WORD i;

	if(con->ScaleArgs != NULL)
	{
		FreeVec(con->ScaleArgs);
		con->ScaleArgs = NULL;
	}

	if(con->ScaleDstBitMap != NULL)
	{
		for(i=0 ; i<con->ScaleDstBitMap->Depth; i++)
		{
			if(con->ScaleDstBitMap->Planes[i] != NULL)
				FreeRaster(con->ScaleDstBitMap->Planes[i], con->PlaneWidth << 1, con->PlaneHeight << 1);
		}

		FreeVec(con->ScaleDstBitMap);
		con->ScaleDstBitMap = NULL;
	}

	if(con->ScaleSrcBitMap != NULL)
	{
		for(i=0; i<con->ScaleSrcBitMap->Depth; i++)
		{
			if(con->ScaleSrcBitMap->Planes[i] != NULL)
				FreeRaster(con->ScaleSrcBitMap->Planes[i], con->PlaneWidth, con->PlaneHeight);
		}

		FreeVec(con->ScaleSrcBitMap);
		con->ScaleSrcBitMap = NULL;
	}

	if(con->ScaleRPort != NULL)
	{
		FreeVec(con->ScaleRPort);
		con->ScaleRPort = NULL;
	}
}


#include <graphics/scale.h>

	/* CreateScale():
	 *
	 *	Sets up the data required for real-time font scaling
	 *	(bitmaps, rastports, etc.).
	 */

BOOL CreateScale(struct VT340Console *con)
{
		/* Create a RastPort to render into. */

	if(con->ScaleRPort = (struct RastPort *)AllocVec(sizeof(struct RastPort),MEMF_ANY|MEMF_CLEAR))
	{
		WORD i;

		con->PlaneWidth	= con->dcFont->tf_XSize;
/*		con->PlaneHeight	= con->ySize;*/
		con->PlaneHeight	= MAX_YSIZE;

			/* Initialize it. */

		InitRastPort(con->ScaleRPort);

			/* Create the bitmap to render into. */

		if(con->ScaleSrcBitMap = (struct BitMap *)AllocVec(sizeof(struct BitMap),MEMF_ANY|MEMF_CLEAR))
		{
				/* Create the bitmap to place the scaled font data into. */

			if(con->ScaleDstBitMap = (struct BitMap *)AllocVec(sizeof(struct BitMap),MEMF_ANY|MEMF_CLEAR))
			{
				BOOL allFine=TRUE;

					/* Initialize the bitmap. */

				InitBitMap(con->ScaleSrcBitMap, con->io->xem_window->WScreen->RastPort.BitMap->Depth, con->PlaneWidth, con->PlaneHeight);

					/* Allocate the necessary memory space. */

				for(i=0; i<con->ScaleSrcBitMap->Depth; i++)
				{
					if(!(con->ScaleSrcBitMap->Planes[i] = AllocRaster(con->PlaneWidth, con->PlaneHeight)))
					{
						allFine = FALSE;
						break;
					}
				}

				if(allFine != FALSE)
				{
						/* Initialize destination bitmap, it must be
						 * able to hold four times the size of the
						 * source data.
						 */

					InitBitMap(con->ScaleDstBitMap, con->io->xem_window->WScreen->RastPort.BitMap->Depth, con->PlaneWidth << 1, con->PlaneHeight << 1);

						/* Allocate space for the destination area. */

					for(i=0; i<con->ScaleDstBitMap->Depth; i++)
					{
						if(!(con->ScaleDstBitMap->Planes[i] = AllocRaster(con->PlaneWidth << 1, con->PlaneHeight << 1)))
						{
							allFine = FALSE;
							break;
						}
					}

					if(allFine != FALSE)
					{
							/* Put the source bitmap into the source RastPort. */

						con->ScaleRPort->BitMap = con->ScaleSrcBitMap;

							/* Install the fonts. */

						SetFont(con->ScaleRPort, con->dcFont);

							/* Set the default rendering pens. */

						SetAPen(con->ScaleRPort, 1);
						SetBPen(con->ScaleRPort, 0);

							/* By default, overwrite data. */

						SetDrMd(con->ScaleRPort, JAM2);

						con->ScaleTextStyle = FS_NORMAL;
						SetSoftStyle(con->ScaleRPort, FS_NORMAL, SYSTEM_TEXTSTYLES);


							/* Allocate space for the bitmap scaling arguments. */

						if(con->ScaleArgs = (struct BitScaleArgs *)AllocVec(sizeof(struct BitScaleArgs), MEMF_ANY|MEMF_CLEAR))
						{
								/* Initialize the structure. */

							con->ScaleArgs->bsa_SrcHeight		= MAX_YSIZE;
/*							con->ScaleArgs->bsa_SrcHeight		= con->dcFont->tf_YSize;*/
							con->ScaleArgs->bsa_SrcWidth		= con->dcFont->tf_XSize;

							con->ScaleArgs->bsa_YSrcFactor	= 1;

							con->ScaleArgs->bsa_SrcBitMap		= con->ScaleSrcBitMap;
							con->ScaleArgs->bsa_DestBitMap	= con->ScaleDstBitMap;

							return(TRUE);
						}
					}
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
	 *	the approriate characters are scaled in real-time before
	 *	they are displayed.
	 */

VOID PrintScaled(struct VT340Console *con, UBYTE *buffer, LONG size, WORD col, WORD row, UBYTE scaleMode, BOOL reScale)
{
	struct PageLine *line;
	UWORD cursorX, cursorY, srcY, destX, destY, sizeX, baseline, penOffset;
	

	/* home = 1/1 */

	cursorX = col-1;
	cursorY = row-1;

	penOffset = col-1;

	line = (reScale != FALSE) ? GetLineByRow(con, row) : NULL;


		/* Determine scale to be used. */

	switch(scaleMode)
	{
		case SCALE_ATTR_TOP2X:			/* Double height (top bits). */
		{
			con->ScaleArgs->bsa_XDestFactor	= 2;
			con->ScaleArgs->bsa_YDestFactor	= 2;
			con->ScaleArgs->bsa_XSrcFactor	= 1;

			srcY	= 0;
			destX	= cursorX << 3;
			sizeX	= 16;

			con->ScaleCache = -1;
		}
		break;

		case SCALE_ATTR_BOT2X:			/* Double height (bottom bits). */
		{
			con->ScaleArgs->bsa_XDestFactor	= 2;
			con->ScaleArgs->bsa_YDestFactor	= 2;
			con->ScaleArgs->bsa_XSrcFactor	= 1;

			srcY	= con->ySize;
			destX	= cursorX << 3;
			sizeX	= 16;

			con->ScaleCache = -1;
		}
		break;

		case SCALE_ATTR_2X:			/* Double width. */
		{
			con->ScaleArgs->bsa_XDestFactor	= 2;
			con->ScaleArgs->bsa_YDestFactor	= 1;
			con->ScaleArgs->bsa_XSrcFactor	= 1;

			srcY	= 0;
			destX	= cursorX << 3;
			sizeX	= 16;

			con->ScaleCache = -1;
		}
		break;

		default:
			;
		return;

	}

		/* Look for the font type to scale. */

	if(con->ScaleRPort->Font != con->dcFont)
	{
		SetFont(con->ScaleRPort, con->dcFont);
		con->ScaleArgs->bsa_SrcWidth = 8;
		con->ScaleCache = -1;
	}

		/* Calculate topmost line to write to. */

	destY = cursorY * con->ySize;

		/* Remember the font baseline. */

	baseline = con->dcFont->tf_Baseline;

		/* Run down the buffer... */

	while(size--)
	{

			/* Set the approriate colours. */


		if(line != NULL)	/*  => (reScale != FALSE) */
		{
			UBYTE pen;

		/*
		** ATTR_HIGHLIGHT && ATTR_INVERSE are
		** handled via apen/bpen..(-:
		*/

			if((con->ScaleTextStyle & FSF_UNDERLINED)  &&  !(line->pl_AttributesRaster[penOffset] & ATTR_UNDERLINE))
			{
				con->ScaleTextStyle &= ~FSF_UNDERLINED;
				SetSoftStyle(con->ScaleRPort, FS_NORMAL, SYSTEM_TEXTSTYLES);
				con->ScaleCache = -1;
			}
			else
			{
				if(!(con->ScaleTextStyle & FSF_UNDERLINED)  &&  (line->pl_AttributesRaster[penOffset] & ATTR_UNDERLINE))
				{
					con->ScaleTextStyle |= FSF_UNDERLINED;
					SetSoftStyle(con->ScaleRPort, FSF_UNDERLINED, SYSTEM_TEXTSTYLES);
					con->ScaleCache = -1;
				}
			}

			if((con->ScaleTextStyle & FSF_BOLD)  &&  !(line->pl_AttributesRaster[penOffset] & ATTR_FAT))
			{
				con->ScaleTextStyle &= ~FSF_BOLD;
				SetSoftStyle(con->ScaleRPort, FS_NORMAL, SYSTEM_TEXTSTYLES);
				con->ScaleCache = -1;
			}
			else
			{
				if(!(con->ScaleTextStyle & FSF_BOLD)  &&  (line->pl_AttributesRaster[penOffset] & ATTR_FAT))
				{
					con->ScaleTextStyle |= FSF_BOLD;
					SetSoftStyle(con->ScaleRPort, FSF_BOLD, SYSTEM_TEXTSTYLES);
					con->ScaleCache = -1;
				}
			}

			pen = ATTRIBUTES_APEN(line->pl_AttributesRaster[penOffset]);
			if(con->ScaleRPort->FgPen != pen)
			{
				SetAPen(con->ScaleRPort, pen);
				con->ScaleCache = -1;
			}

			pen = ATTRIBUTES_BPEN(line->pl_AttributesRaster[penOffset]);
			if(con->ScaleRPort->BgPen != pen)
			{
				SetBPen(con->ScaleRPort, pen);
				con->ScaleCache = -1;
			}
		}
		else
		{
			if((con->ScaleTextStyle & FSF_UNDERLINED)  &&  !(con->attr & ATTR_UNDERLINE))
			{
				con->ScaleTextStyle &= ~FSF_UNDERLINED;
				SetSoftStyle(con->ScaleRPort, FS_NORMAL, FSF_UNDERLINED);
				con->ScaleCache = -1;
			}
			else
			{
				if(!(con->ScaleTextStyle & FSF_UNDERLINED)  &&  (con->attr & ATTR_UNDERLINE))
				{
					con->ScaleTextStyle |= FSF_UNDERLINED;
					SetSoftStyle(con->ScaleRPort, FSF_UNDERLINED, FSF_UNDERLINED);
					con->ScaleCache = -1;
				}
			}

			if((con->ScaleTextStyle & FSF_BOLD)  &&  !(con->attr & ATTR_FAT))
			{
				con->ScaleTextStyle &= ~FSF_BOLD;
				SetSoftStyle(con->ScaleRPort, FS_NORMAL, FSF_BOLD);
				con->ScaleCache = -1;
			}
			else
			{
				if(!(con->ScaleTextStyle & FSF_BOLD)  &&  (con->attr & ATTR_FAT))
				{
					con->ScaleTextStyle |= FSF_BOLD;
					SetSoftStyle(con->ScaleRPort, FSF_BOLD, SYSTEM_TEXTSTYLES);
					con->ScaleCache = -1;
				}
			}

			if(con->ScaleRPort->FgPen != con->apen)
			{
				SetAPen(con->ScaleRPort, con->apen);
				con->ScaleCache = -1;
			}

			if(con->ScaleRPort->BgPen != con->bpen)
			{
				SetBPen(con->ScaleRPort, con->bpen);
				con->ScaleCache = -1;
			}
		}

		penOffset++;


		if(*buffer != con->ScaleCache)	/* same char as before..?? */
		{
				/* Print the character to be scaled into the
				 * invisible drawing area.
				 */

			Move(con->ScaleRPort, 0, baseline);
			Text(con->ScaleRPort, buffer, 1);

				/* Scale the font. */

			BitMapScale(con->ScaleArgs);
		}

		con->ScaleCache = *buffer;
		buffer++;


			/* Render the character. */

		myBltBitMapRastPort(con, con->ScaleDstBitMap, 0, srcY, destX, destY, sizeX, con->ySize, 0xC0);

		destX += sizeX;
	}
}



/************************************/

/*
 ESC # 3	DECDHL		Double height and width line, top half
 ESC # 4	DECDHL		Double height and width line, bottom half
 ESC # 5	DECSWL		Single height and width line
 ESC # 6	DECDWL		Double width single height line
 ESC # 8	DECALN		Screen alignment test, fill screen with E's
*/
VOID PageTextScaleMode(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	struct PageLine *line;
	BOOL reset;
	UBYTE mode;

	if(con->inESC == '8')
	{
		VT340_screenaligntest(con);
		return;
	}


	reset = (PageGetLineMode(con) != SCALE_ATTR_NORMAL);

	switch(con->inESC)
	{
		case '3':
			mode = SCALE_ATTR_TOP2X;
		break;

		case '4':
			mode = SCALE_ATTR_BOT2X;
		break;

		case '5':
			mode = SCALE_ATTR_NORMAL;
		break;

		case '6':
			mode = SCALE_ATTR_2X;
		break;

	}


	line = GetCurrentLine(con);
	line->pl_LineMode = mode;

	if(reset != FALSE)
	{
		PageEraseLines(con, con->row-1, 1);
		PageGetLine(con);
	}
	else
	{
		VT340_adjustCursorColumnPosition(con);

		if(mode != SCALE_ATTR_NORMAL)
		{
			PageEraseCharacters(con, (con->columns>>1) + 1, (con->columns>>1));
			PageGetLine(con);
		}
	}
}


/************************************/


STATIC VOID PageSetTurboMask(struct VT340Console *con)
{
	struct PageLine *line;
	UWORD i;
	WORD top, bot;
	UBYTE mask;

	VT340_getTopBot(con, &top, &bot);

	line = GetLineByRow(con, top);
	for(i=top, mask=0; i<=bot; i++)
	{
		mask |= line->pl_LinePens;
		line = (struct PageLine *)line->pl_Link.ln_Succ;
	}

	SetWrMsk(con->rp, mask);
}

STATIC VOID PageSetDefaultMask(struct VT340Console *con)
{
	SetWrMsk(con->rp, con->writeMask);
}


VOID PageTextOut(struct VT340Console *con)
{
	if(con->gstat & G_PRINT_CONTROL  &&  con->printerHandle != NULL)
	{
		if(!FWrite(con->printerHandle, con->ordText, con->ordC, 1))
		{
			con->gstat &= ~G_PRINT_CONTROL;

			if(con->gstat & G_AUTO_PRINT_INACTIVE)
			{
				con->gstat &= ~G_AUTO_PRINT_INACTIVE;
				con->gstat |=  G_AUTO_PRINT;
			}

			CreateOwnStatusLine(con);
		}
	}
	else
	{
		struct PageLine *line;
		WORD col;
		UBYTE chars, width;

		if(con->gstat & G_AUTO_PRINT  &&  con->printerHandle != NULL)
		{
			if(!FWrite(con->printerHandle, con->ordText, con->ordC, 1))
			{
				con->gstat &= ~(G_AUTO_PRINT | G_AUTO_PRINT_INACTIVE);
				CreateOwnStatusLine(con);
			}
		}

		col	= con->ordCol;
		chars	= con->ordC;
		line	= GetCurrentLine(con);
		width	= line->pl_LineMode;

		if(width != SCALE_ATTR_NORMAL)
		{
			col <<= 1;
			col--;
			if(con->ustat & U_INSERT)
				PageInsertChars(con, col, chars << 1);
		}
		else
		{
			if(con->ustat & U_INSERT)
			{
				PageInsertChars(con, col, chars);
			}
		}

		line->pl_LinePens |= (con->apen | con->bpen);

		if(line->pl_FirstCol > col)
			line->pl_FirstCol = col;

		if(line->pl_LastCol < col + (chars-1))
			line->pl_LastCol = col + (chars-1);


		if(width != SCALE_ATTR_NORMAL)
			PrintScaled(con, con->ordText, chars, col, con->row, width, FALSE);
		else
			myMove_Text(con, col, con->row, con->ordText, chars);

		PagePutString(con, con->ordText, con->ordC, con->ordCol);
	}

	con->ordC = 0;
}


STATIC BOOL PageGetTurboDims(struct VT340Console *con, WORD *top, WORD *bot, WORD *left, WORD *right, WORD direction)
{
	struct PageLine *line;
	WORD i;
	WORD min, max, sTop, sBot;

/* Home == 1/1 */

	sTop = *top;
	sBot = *bot;

	*left  = 1;
	*right = con->columns;


/*	FPrintf(Output(), "Vorher: %ld, %ld, %ld, %ld\n", *top, *bot, *left, *right);*/

	line = GetLineByRow(con, *top);
	for(i=*top; i<*bot; i++)
	{
		if(line->pl_LinePens == BACKGROUND_PEN)
			*top += 1;
		else
			break;

		line = (struct PageLine *)line->pl_Link.ln_Succ;
	}

	line = GetLineByRow(con, *bot);
	for(i=*bot; i>*top; i--)
	{
		if(line->pl_LinePens == BACKGROUND_PEN)
			*bot -= 1;
		else
			break;

		line = (struct PageLine *)line->pl_Link.ln_Pred;
	}

	if(*top == *bot  &&  line->pl_LinePens == BACKGROUND_PEN)
		return(FALSE);			/* nothing to scroll..(-: */


	min	= con->columns;
	max	= 1;
	line	= GetLineByRow(con, *top);
	for(i=*top; i<=*bot; i++)
	{
		if(line->pl_LinePens != BACKGROUND_PEN)
		{
			if(min > line->pl_FirstCol)
				min = line->pl_FirstCol;

			if(max < line->pl_LastCol)
				max = line->pl_LastCol;
		}
		line = (struct PageLine *)line->pl_Link.ln_Succ;
	}

	if(min > max)
		min = max;



	if(direction < 0)	/* scroll down */
	{
		*bot += abs(direction);
		if(*bot > sBot)
			*bot = sBot;
	}
	else	/* scroll up */
	{
		*top -= direction;
		if(*top < sTop)
			*top = sTop;
	}



	*left = min;
	*right = max;

	if(*left < 1)
		*left = 1;

	if(*right > con->columns)
		*right = con->columns;

/*	FPrintf(Output(), "Nachher: %ld, %ld, %ld, %ld\n\n", *top, *bot, *left, *right);*/

	return(TRUE);
}


VOID PageDeleteChars(struct VT340Console *con, WORD col, UBYTE cnt)
{
	UWORD xsize;
	WORD y;

	xsize = XSIZE;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
		xsize <<= 1;

	y = con->row * con->ySize;

	if(col - 1 + cnt > con->columns)
		cnt = con->columns - col + 1;

	PageShiftChars(con, +cnt);
	myScrollRaster(con, xsize * cnt, 0, (col-1) * xsize, y - con->ySize, con->border.Width - 1, y - 1);
}


VOID PageInsertChars(struct VT340Console *con, WORD col, UBYTE cnt)
{
	UWORD xsize;
	WORD y;

	xsize = XSIZE;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
		xsize <<= 1;

	if(col < con->columns)
	{
		y = con->row * con->ySize;
		PageShiftChars(con, -cnt);
		myScrollRaster(con,(-xsize * cnt), 0,
		 (col - 1) * xsize, y-con->ySize, con->border.Width-1, y-1);
    }
}


STATIC VOID PageEraseLineBeg(struct VT340Console *con)
{
	UWORD xsize;
	UWORD y;

	xsize	= XSIZE;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
		xsize	<<= 1;

	y = (con->row * con->ySize);
	myRectFill(con, 0, y - con->ySize, (con->col * xsize)-1, y-1, BACKGROUND_PEN);
}


STATIC VOID PageEraseLineEnd(struct VT340Console *con)
{
	UWORD lim, xsize;
	WORD y;

	xsize	= XSIZE;
	lim	= con->columns;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
	{
		xsize	<<= 1;
		lim	<<= 1;
	}

	y = (con->row * con->ySize);
	myRectFill(con, (con->col-1) * xsize, y - con->ySize, con->border.Width-1, y-1, BACKGROUND_PEN);
}


VOID PageEraseLines(struct VT340Console *con, WORD row, UWORD cnt)
{
	if(cnt == 0)
		return;

	myRectFill(con, 0, row * con->ySize, con->border.Width-1, ((row + cnt) * con->ySize)-1, BACKGROUND_PEN);
}


STATIC VOID PageEraseScrBeg(struct VT340Console *con)
{
	UWORD cnt = con->row - 1;

	if(con->row > 1)
		myRectFill(con, 0, (con->firstLine-1) * con->ySize, con->border.Width-1, (cnt * con->ySize)-1, BACKGROUND_PEN);

	PageEraseLineBeg(con);
}


STATIC VOID PageEraseActiveScreen(struct VT340Console *con)
{
	myRectFill(con, 0, (con->firstLine-1) * con->ySize, con->border.Width-1, (con->lastLine * con->ySize)-1, BACKGROUND_PEN);
}


/************************************/



VOID CreateOwnStatusLine(struct VT340Console *con)
{
	if(con->pstat & P_OWN_STATUSLINE  &&  con->statusLine)
	{
		auto UBYTE line[132];
		auto UBYTE buf[80];
		UBYTE *opt1, *opt2, *opt3, i;

		setmem(line, sizeof(line), ' ');

/* */
		if(con->inTEK != FALSE)
			strcpy(line, " TEK 4010/4014  · ");
		else
		{
			if(con->pstat & P_ANSI_MODE)
				opt1 = (con->pstat & P_VT340_MODE) ? " VT-340" : " VT-102";
			else
				opt1 = " VT-52 ";

			strcpy(line, opt1);
/* */

/* */
			opt1 = (con->pstat & P_CONVERT) ? " · 7" : " · 8";
			strcat(line, opt1);
			strcat(line, " BIT · ");
/* */
		}

		strcat(line, "      ");

/* */

		sprintf(buf, " · %ld×%ld · PAGE:%ld/%ld", con->columns, con->lastLine, con->currentPage+1, con->pagesInUse);
		strcat(line, buf);

/* */

		sprintf(buf, " (%3ld/%3ld) · ", con->firstLine, con->lastLine);
		strcat(line, buf);

/* */

		opt1 = (con->ustat & U_INSERT)	? "I-"
													: "O-";
		opt2 = (con->tstat & T_WRAP)		? "W-"
													: "T-";
		opt3 = (con->ustat & U_NEWLINE)	? "N · "
													: "L · ";

		strcat(line, opt1);
		strcat(line, opt2);
		strcat(line, opt3);
/* */

/* */
		if(con->gstat & G_PRINT_CONTROL)
			opt1 = (con->gstat & G_AUTO_PRINT_INACTIVE) ? "PRINT CTL**"
																	  : "PRINT CNTRL";
		else
		{
			if(con->gstat & G_AUTO_PRINT)
				opt1 = "AUTO PRINT";
			else
				opt1 = "PRINTER OFF";
		}

		strcat(line, opt1);
		for(i=0; i<sizeof(line); i++)
		{
			if(line[i] == '\0')
				line[i] = ' ';
		}
/* */

		SetFont(con->rp, con->topazFont);
		SetDrMd(con->rp, JAM2|INVERSVID);
		SetSoftStyle(con->rp, FS_NORMAL, FSF_UNDERLINED);
		SetAPen(con->rp, con->foregroundPen);
		SetBPen(con->rp, BACKGROUND_PEN);

		myMove_Text(con, 1, con->statusLine, line, con->border.Width>>3);

		SetAPen(con->rp, con->apen);
		SetBPen(con->rp, con->bpen);
		SetSoftStyle(con->rp, GetTextStyle(con), SYSTEM_TEXTSTYLES);
		SetDrMd(con->rp, JAM2);
		SetFont(con->rp, con->dcFont);

		VT340_locked(con, (con->inDCS  ||  con->inPrivate));
	}
}


VOID DeleteStatusLine(struct VT340Console *con, BOOL ownOnly)
{
	if(con->statusLine != 0)
	{
		if(ownOnly == FALSE  ||  (ownOnly != FALSE  &&  con->pstat & P_OWN_STATUSLINE))
		{
			struct PageLine *line;

			myRectFill(con, 0, (con->statusLine-1) * FONT_YSIZE, con->border.Width-1, con->border.Height-1, BACKGROUND_PEN);
			line = GetLineByRow(con, con->statusLine);
			ResetLine(line);
		}
	}
}


/************************/

VOID PageCursorFlip(struct VT340Console *con)
{
	UWORD x1, x2, y1, y2, xsize=XSIZE;
	WORD row, col;
	UBYTE offset;

	con->cursorVisible = (con->cursorVisible != FALSE) ? FALSE : TRUE;

	if(!(con->ustat & U_CURSOR_VISIBLE))
		return;

	offset = (con->pstat & P_CURSOR_UNDERSCORE) ? BOTLINE : 0;

	row = con->row - 1;
	col = con->col - 1;
	if(col > (con->columns-1))
		col = con->columns-1;
	if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
	{
		xsize <<= 1;
		col <<= 1;
	}
	SetDrMd(con->rp, COMPLEMENT);

	x1 = col * XSIZE;
	y1 = (row+1) * con->ySize - FONT_YSIZE + offset;
	x2 = col * XSIZE + xsize - 1;
	y2 = (row+1) * con->ySize - 1;

	myRectFill(con, x1, y1, x2, y2, -1);

	SetDrMd(con->rp, JAM2);
}


VOID PageCursorVisible(struct VT340Console *con)
{
	if(con->cursorVisible == FALSE)
		PageCursorFlip(con);
}


VOID PageCursorInvisible(struct VT340Console *con)
{
	if(con->cursorVisible != FALSE)
		PageCursorFlip(con);
}


STATIC VOID DeleteTimer(struct MsgPort **timerPort, struct timerequest **doTime)
{
	if(*doTime != NULL  &&  (*doTime)->tr_node.io_Device != NULL)
	{
		CloseDevice((struct IORequest *)*doTime);
		*doTime = NULL;
	}

	if(*timerPort != NULL)
	{
		DeletePort(*timerPort);
		*timerPort = NULL;
	}

	if(*doTime != NULL)
	{
		DeleteIORequest((struct IORequest *)*doTime);
		*doTime = NULL;
	}
}


STATIC BOOL CreateTimer(struct MsgPort **timerPort, struct timerequest **doTime)
{
	if(*timerPort = CreatePort(NULL, 0))
	{
		if(*doTime = (struct timerequest *)CreateIORequest(*timerPort, sizeof(struct timerequest)))
		{
			if(!OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)*doTime, 0))
			{
				(*doTime)->tr_node.io_Command = TR_ADDREQUEST;
				(*doTime)->tr_node.io_Flags	= 0;
				(*doTime)->tr_node.io_Error	= 0;

				return(TRUE);
			}
		}
	}

	DeleteTimer(timerPort, doTime);

	return(FALSE);
}


VOID ASyncTimer(struct timerequest *doTime, ULONG ticks, BOOL *requestPending)
{
	doTime->tr_time.tv_secs		= ticks / 50;
	doTime->tr_time.tv_micro	= (ticks % 50) * 20000;
	doTime->tr_node.io_Command	= TR_ADDREQUEST;

	SendIO((struct IORequest *)doTime);
	*requestPending = TRUE;
}


BOOL CreateCursorServer(struct VT340Console *con)
{
	if(con == NULL  ||  !(con->pstat & P_CURSOR_BLINKING)  ||  con->isReady == FALSE)
		return(FALSE);

	if(con->timerPort == NULL  &&  con->doTime == NULL)
	{
		if(CreateTimer(&(con->timerPort), &(con->doTime)) != FALSE)
		{
			*con->io->xem_signal |= (1 << con->timerPort->mp_SigBit);

			con->cursorVisible = FALSE;
			con->requestPending= FALSE;

			ASyncTimer(con->doTime, CURSOR_OFF, &con->requestPending);
		}
	}

	return(FALSE);
}


BOOL DeleteCursorServer(struct VT340Console *con)
{
	if(con == NULL)
		return(FALSE);

	if(con->requestPending != FALSE)
	{
		AbortIO((struct IORequest *)con->doTime);
		WaitIO((struct IORequest *)con->doTime);
		con->requestPending = FALSE;
	}

	DeleteTimer(&(con->timerPort), &(con->doTime));

	return(TRUE);
}


/************************/

/* end of source-code */
