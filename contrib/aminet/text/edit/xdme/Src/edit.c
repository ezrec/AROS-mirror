/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Everything for editing like DEL, BS, INDENT, DELINE, INSLINE, ...

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include "clipboard.h"
#define MYDEBUG     0
#include <debug.h>
#include <string.h>


/**************************************
	    Globale Variable
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/


/**************************************
	   Interne Prototypes
**************************************/


/*DEFHELP #cmd textedit BACK - backspace, (delete char to left of cursor) */
/*DEFHELP #cmd textedit BS - backspace, (delete char to left of cursor) */
/*DEFHELP #cmd textedit DEL - delete, (deletes char under cursor) */

DEFUSERCMD("bs",   0, CF_COK, void, do_bs, (void),;)
DEFUSERCMD("del",  0, CF_COK, void, do_bs, (void),;)
DEFUSERCMD("back", 0, CF_COK, void, do_bs, (void),)
{
    ED	 * ep  = Ep;
    RP	 * rp  = ep->win->RPort;
    WORD   i,
	   col = ep->column;
    Column right;
    BOOL   is_bs = (av[0][0] == 'b');

    if ((Current[col] && !is_bs) || (col && is_bs))
    {
	if (is_bs)
	{
	    movmem (Current + col, Current + col - 1, Clen - col + 1);

	    col --;
	    ep->column --;

	} else
	    movmem (Current + col + 1, Current + col, Clen - col);

	Clen --;

	i = ep->line;
	right = ep->topcolumn + Columns;

	if (!Nsu) {                         /* PATCH_NULL [09-Dec-92] : line added */
	    if (col < ep->topcolumn)
	    {
		text_adjust (FALSE);
	    } else
	    {
		if (is_inblock (i, right) & BP_INSIDE)
		{
		    SetAPen (rp, BLOCK_FPEN(ep));
		    SetBPen (rp, BLOCK_BPEN(ep));
		} else
		{
		    SetAPen (rp, TEXT_FPEN(ep));
		    SetBPen (rp, TEXT_BPEN(ep));
		}

		SetWrMsk (rp, BLOCK_MASK(ep));

		i -= ep->topline;

		ScrollRaster(rp, Xsize, 0,
		    COL(col - ep->topcolumn), ROW(i),
		    Xpixs, ROW(i + 1) - 1
		);

		if (ep->win->WLayer->Flags & LAYERREFRESH)
		    OptimizedRefresh (ep);
		else if (Clen >= right)
		{
		    right --;

		    Move (rp, COLT(right - ep->topcolumn), ROWT(i));
		    Text (rp, Current + right, 1);
		}
	    }
	} /* if !Nsu */ 		    /* PATCH_NULL [09-Dec-92] : line added */

	if (!GETF_COMLINEMODE(Ep) && GETF_WORDWRAP(Ep))
	    do_reformat(0);
    } else
    {
	SETF_ABORTCOMMAND(Ep,1);
    }
} /* do_bs */


/*DEFHELP #cmd textedit REMEOL - Remove text under and beyond the cursor. */

DEFUSERCMD("remeol", 0, CF_VWM|CF_COK, void, do_remeol, (void),)
{
    Current[Clen = Ep->column] = 0;

    text_sync ();
    text_redisplaycurrline ();
} /* do_remeol */


/*DEFHELP #cmd textedit SPLIT - Split line at cursor */

DEFUSERCMD("split", 0, 0, void, do_split, (void),)
{
    UBYTE buf[MAXLINELEN];
    ED	* ep		   = Ep;
    RP	* rp		   = ep->win->RPort;

    strcpy ((char *)buf, (char *)Current+ep->column);
    Current[Clen = ep->column] = '\0';

    text_sync ();

    SetAPen (rp, TEXT_BPEN(ep));
    SetWrMsk (rp, BLOCK_MASK(ep));

    if (!Nsu)
	RectFill (rp, COL(0), ROW(ep->line-ep->topline),
	       Xpixs, ROW(ep->line-ep->topline+1)-1);

    SetAPen (rp, TEXT_FPEN(ep));

    text_displayseg (ep->line - ep->topline, 1);

    ep->line ++;

    if (ep->line != ep->lines)
	do_insline ();
    else
	do_downadd ();

    strcpy ((char *)Current, (char *)buf);
    Clen = strlen ((char *)Current);

    text_sync ();
    text_displayseg (ep->line - ep->topline, 1);
    do_up ();
} /* do_split */


/*DEFHELP #cmd textedit JOIN - join next line to line at cursor */

DEFUSERCMD("join", 0, 0, int, do_join, (void),)
{
    int   i = Clen;
    ED * ep = Ep;

    /* Check if there is a line below and the length of both lines */
    if (ep->line + 1 < ep->lines &&
		LINELEN(ep,ep->line+1)+i <= MAXLINELEN-2)
    {
	char * ptr;

	/* Find first nonspace in next line */
	for (ptr=GETTEXT(ep,ep->line+1); *ptr==' ' && *ptr; ptr ++);

	if (i && !Current[i]) /* ignore last char if it's \0 */
	    i --;

	/* Find last char in this line */
	for (; i >= 0 && Current[i] == ' '; i--);

	/* Add a space if neccessary */
	if (i >= 0)
	{
	    if (Current[i])
		i++;

	    Current[i ++] = ' ';
	}

	/* Copy the line */
	strcpy((char *)Current+i, ptr);

	/* new length */
	Clen = strlen ((char *)Current);

	/* redisplay */
	text_sync ();
	text_displayseg (ep->line - ep->topline, 1);

	i = text_lineno ();

	do_down ();
	do_deline ();

	if (i != text_lineno ())
	    do_up ();

	return(1);
    }

    return(0);
} /* do_join */


/*
 * n == -1  :	force reformat entire paragraph
 * n ==  0  :	only until line equalizes (from text_write())
 *
 * What is a paragraph?   A paragraph ends whenever the left justification
 * gets larger, or on a blank line.
 */

/*DEFHELP #cmd textedit,format REFORMAT - reformat paragraph using the margin. */

DEFUSERCMD("reformat", 0, 0, void, do_reformat, (int n),)
{
    char * str;
    ED	 * ep = Ep;
    RP	 * rp = ep->win->RPort;
    int    nlok;   /* Next Line ok ? */
    int    lnsc;   /* LastNS of Current */
    int    fnst;   /* FirstNS of next Line */
    int    fnsc;   /* FirstNS of Current */
    int    column  = ep->column;
    int    srow    = ep->line;
    int    crow    = srow;
    int    erow    = srow;
    WORD   dins    = 0; 	/* relative insert lines/delete lines	*/
    char   moded   = 0; 	/* any modifications done at all?	*/
    char   checked = 0; 	/* for cursor positioning.		*/
    Line   old_top;

    if (ep->config.margin == 0)
	ep->config.margin = 75;

    Nsu ++;

    old_top = ep->topline;

    for (;;)
    {
	str = GETTEXT(ep,ep->line+1);
	fnst = 0;
	fnsc = firstns((char *)Current);
	nlok = (ep->line + 1 < ep->lines && fnsc >= (fnst=firstns(str)));
	if (ep->config.wwcol >= 0)
	    fnst = fnsc = ep->config.wwcol;
	if (nlok && str[0] == 0)
	    nlok = 0;
	lnsc = lastns((char *)Current);
	if (lnsc < ep->config.margin)
	{    /* space at end of line for marg-lnsc-2 letter word   */
	    if (nlok == 0)        /* but no more data to joinup   */
		break;		  /* done */
	    if (ep->config.margin - lnsc - 2 >= wordlen(str+fnst))
	    {
		ep->column = 0;
		Clen = lastns((char *)Current);
		if (Current[Clen])
		    ++Clen;
		moded = 1;
		--dins;
		if (do_join())
		    continue;
		++dins;

		error ("reformat:\nMargin too big");
		break;
	    }
	    if (n == 0)        /* if couldn't mod line, and text_write, don't update any more */
		break;
	    do_down ();
	    erow = ep->line;
	    continue;
	}
				/* no space, need to split	*/
				/* find start of prev word	*/
	for (;;)
	{
	    int i = lnsc;
	    while (i && Current[i] != ' ')
		--i;
	    lnsc = i;
	    if (i >= ep->config.margin)
	    {
		while (i && Current[i] == ' ')
		    --i;
		if (i < ep->config.margin)
		    break;
		lnsc = i;
	    } else
		break;
	}

	/* Word too long */
	if (wordlen((char *)Current+lnsc) >= ep->config.margin)
	{
	    error ("reformat:\nWord too long");
	    break;
	}

	if (lnsc)
	{	      /* ok to split at word	      */
	    ++lnsc;
	    ++dins;
	    ep->column = lnsc;
	    do_split (); /* Split at point LNSC          */
	    do_down ();          /* must insert proper amount?   */
	    {
		int indent = (nlok == 0) ? fnsc : fnst;
		if (ep->config.wwcol >= 0)
		    indent = ep->config.wwcol;
		if (!checked)
		{
		    checked = 1;
		    if (lnsc <= column)
		    {	/* if split before cursor   */
			column = column - ep->column + indent;
			++crow;
		    }
		}
		if (Clen + indent < MAXLINELEN-3)
		{
		    movmem(Current, Current + indent,
			    strlen((char *)Current)+1);
		    memset(Current, ' ', indent);
		    Clen += indent;
		}
	    }
	    erow = ep->line;
	    continue;
	}

	if (n == 0)
	    break;

	do_down ();
    }

    if (column < 0 || column > 200)
	column = 0;

    if (srow >= ep->lines)
	goto ra;

    Nsu --;

    if (old_top != ep->topline)
    {
	scroll_display (0, ep->topline-old_top,
		ep->topcolumn, ep->topline,
		ep->topcolumn + Columns, ep->topline + Lines);
    }

    if (dins || srow < ep->topline || srow >= ep->topline + Lines)
    {
ra:
	text_sync ();

	ep->line = crow;
	ep->column = column;

	text_load ();

	text_adjust (TRUE);
    } else
    {
	text_sync ();

	ep->line = crow;
	ep->column = column;

	text_load ();

	if (erow != srow)
	{
	    if (!text_adjust (FALSE))
	    {
		if (erow - ep->topline > Lines)
		    erow = ep->topline + Lines;

		SetAPen(rp, TEXT_BPEN(ep));
		SetWrMsk (rp, BLOCK_MASK(ep));

		RectFill(rp, COL(0), ROW(srow - ep->topline),
		    Xpixs, ROW(erow - ep->topline + 1)-1);

		SetAPen(rp, TEXT_FPEN(ep));

		text_displayseg(srow - ep->topline, erow - srow + 1);
	    }
	} else
	{
	    if (moded)
		text_redisplaycurrline ();
	}
    }
    if (column > Clen)
    {
	setmem(Current+Clen, column - Clen, ' ');
	Current[column] = 0;
    }
    ep->column = column;
} /* do_reformat */


/*DEFHELP #cmd textedit INSLINE - insert line */

DEFUSERCMD("insline", 0, 0, void, do_insline, (void),)
{
    LINE ptr;
    ED *ep = Ep;

    SETF_MODIFIED(ep,1);

    text_sync ();

    if (makeroom (32))
    {
	ptr = allocline (1);

	bmovl(ep->list+ep->line, ep->list+ep->line+1,ep->lines-ep->line);
	SETLINE(ep,ep->line,ptr);

	ep->lines ++;

	if (ActualBlock.ep == ep)
	{
	    if (ep->line <= ActualBlock.start_line)
		set_block_start (ActualBlock.start_line+1, -2);
	    if (ep->line <= ActualBlock.end_line)
		set_block_end (ActualBlock.end_line+1, -2);
	}
    } else
    {
	nomemory ();
    }

    text_load ();

    if (Nsu == 0)
    {
	scroll_display (0, -1, 0, ep->line, MAXLINELEN, ep->topline+Lines);
	text_displayseg (ep->line - ep->topline, 1);
    }
} /* do_insline */


/*DEFHELP #cmd textedit DELINE - delete line */

DEFUSERCMD("deline", 0, 0, void, do_deline, (void),)
{
    int delline, eline;
    ED *ep = Ep;

    /* store line for later UNDO */
    strcpy ((char *)Deline, (char *)Current);

    if (ep->lines > 1)
    {
	SETF_MODIFIED(ep,1);

	text_sync ();

	/* remove line */
	freeline (GETLINE(ep,ep->line));
	bmovl (ep->list+ep->line+1, ep->list+ep->line, ep->lines-ep->line-1);

	/* adjust block */
	if (ActualBlock.ep == ep)
	{
	    if (ep->line < ActualBlock.start_line)
		set_block_start (ActualBlock.start_line-1, -2);

	    if (ep->line <= ActualBlock.end_line)
		set_block_end (ActualBlock.end_line-1, -2);

	    /* if we just deleted the last line of the block, clear it */
	    if (ActualBlock.end_line < ActualBlock.start_line)
		unblock ();
	}

	/* remember line we deleted */
	delline = ep->line;

	/* line was last one */
	if (ep->line >= --ep->lines)
	{
	    /* move up one line */
	    --ep->line;

	    /* get the line into current */
	    text_load ();

	    /* line now below topline ? */
	    if (ep->line < ep->topline)
	    {
		/* update screen if allowed */
		if (!Nsu)
		{
		    /* find new position */
		    ep->topline = ep->line - (Lines>>1);

		    if (ep->topline < 0)
			ep->topline = 0;

		    text_redisplay ();
		}

		return;
	    }
	}

	/* get line */
	text_load ();

	/* update display */
	if (!Nsu)
	{
	    eline = ep->topline + Lines - 1;

	    if (ep->lines < eline)
		eline = ep->lines +1;

	    scroll_display (0, 1, ep->topcolumn, delline,
				  ep->topcolumn+Columns-1, eline);
	}
    } else
    {
	/* no more lines in the text */
	do_firstcolumn ();
	do_remeol ();
	SETF_MODIFIED(ep,0);
    }
} /* do_deline */


/*DEFHELP #cmd textedit TLATE how - Modify character under cursor. */

DEFUSERCMD("tlate", 1, CF_COK, void, do_tlate, (void),)
{
    UBYTE * ptr = av[1];
    ED	  * ep	= Ep;
    UBYTE   c	= Current[ep->column];

    if (c == 0)
	c = ' ';

    switch (*ptr)
    {
	case '+':
	    c += strtol ((char *)ptr+1, NULL, 0);
	    break;

	case '-':
	    c -= strtol ((char *)ptr+1, NULL, 0);
	    break;

	case '"':
	    c = ptr[1];
	    break;

	case 'u':
	case 'U':
	    c = toupper (c);
	    break;

	case 'l':
	case 'L':
	    c = tolower (c);
	    break;

	default:
	    c = strtol ((char *)ptr, NULL, 0);
    }

    if (c)
    {
	if (Current[ep->column] == 0)
	{
	    Clen = ep->column + 1;
	    Current[Clen] = 0;
	}

	if (!Nsu && Current[ep->column] != c)
	{
	    LINE text_line;

	    SETF_MODIFIED(ep,TRUE);

	    Current[ep->column] = c;	/* Change char */

	    /* get the actual line */
	    text_line = GETLINE(ep,ep->line);

	    /* if the char fits into the line, just copy it (ie. avoid
	       text_sync(). Else call text_sync() */

	    if (ep->column < LENGTH(text_line))
	    {
		char * ptr;

		/* change it */
		ptr = CONTENTS(text_line);
		ptr[ep->column] = c;
	    }
	    else
		text_sync ();

	    /* Delete the character on screen. First, find the color
	       we need for this operation, then draw over it */
	    if (is_inblock (ep->line, ep->column) & BP_INSIDE)
		SetAPen (ep->win->RPort, BLOCK_BPEN(ep));
	    else
		SetAPen (ep->win->RPort, TEXT_BPEN(ep));

	    /* redraw that character */
	    {
		UWORD lin, col;

		col = ep->column - ep->topcolumn;
		lin = ep->line - ep->topline;

		SetWrMsk (ep->win->RPort, BLOCK_MASK(Ep));
		RectFill (ep->win->RPort, COL(col), ROW(lin),
				    COL(col+1)-1, ROW(lin+1)-1);
		SetWrMsk (ep->win->RPort, ALL_MASK);
	    }

	    redraw_textlineseg (ep->line, ep->column, ep->column+1);
	}
    }
} /* do_tlate */


/*DEFHELP #cmd textedit,format JUSTIFY how - simple text justification. */

DEFUSERCMD("justify", 1, 0, void, do_justify, (void),)
{
    ED *ep = Ep;
    WORD  firstnb, lastnb, i, n, fill, c, sp;
    WORD  changed = FALSE;

    firstnb = firstns((char *)Current);
    lastnb = lastns((char *)Current);

    if (ep->config.margin >= MAXLINELEN)
    {
	error ("%s:\nMargin (%ld) too large\n(max %ld)\n",
		av[0], ep->config.margin, MAXLINELEN-1);
	return;
    }

    if (firstnb >= lastnb) /* line empty ? */
	return;

    switch (tolower (av[1][0]))
    {
	case 'l':
	    /* remove all firstnb spaces */
	    if (firstnb)
	    {
		Clen -= firstnb;
		memmove (Current, Current+firstnb, Clen+1);
		changed = TRUE;
	    }
	    break;

	case 'r':
	    /* insert/remove spaces, so the last character is at margin */
	    if (n = (ep->config.margin - lastnb))
		goto insrem;

	    break;

	case 'c':
	    /* width of line */
	    n = (lastnb - firstnb + 1);

	    if (n < ep->config.margin)
	    {
		n = (ep->config.margin/2) - (n+1)/2 - firstnb;

		if (n) /* insert/remove n spaces */
		{
insrem:
		    changed = TRUE;

		    if (n > 0) /* insert */
		    {
			memmove (Current+n, Current, Clen+1);
			memset (Current, ' ', n);
		    }
		    else /* remove */
		    {
			/* n is bigger than the number of spaces at this
			   point, because otherwise the length of the
			   line would have been bigger than the margin.
			   And remember: n is NEGATIVE */

			memmove (Current, Current-n, Clen+n+1);
		    }

		    Clen += n;
		}
	    }

	    break;

	case 'f':
	    n = 0;
	    i = firstnb;

	    while (i <= lastnb)
	    {
		while ((c = Current[i]) && c != ' ')
		    i++;
		if (i <= lastnb)
		{
		    n++;
		    while (Current[i] == ' ')
			i++;
		}
	    }

	    fill = ep->config.margin - lastnb - 1;
	    i = firstnb;
	    Current[lastnb + 1] = 0;

	    if (n > 0 && fill > 0)
		changed = TRUE;

	    while (n > 0 && fill > 0 && Current[i])
	    {
		while ((c = Current[i]) && c != ' ')
		    i++;
		sp = fill / n;
		movmem (&Current[i], &Current[i + sp],
			strlen((char *)&Current[i]) + 1);
		memset ((char *)&Current[i], ' ', sp);
		while (Current[i] == ' ')
		    i++;
		fill -= sp;
		n--;
	    }

	    break;

	default:
	    error ("%s:\nunknown justification\n%s", av[0], av[1]);
	    break;
    }

    if (changed)
    {
	text_sync ();
	text_redisplaycurrline ();
    }
} /* do_justify */


/*DEFHELP #cmd textedit UNJUSTIFY - removes extra spaces in a line */

DEFUSERCMD("unjustify", 0, 0, void, do_unjustify, (void),)
{
    WORD  i, j, waswhite = FALSE;
    UBYTE c;

    for (i = 0; Current[i] == ' '; i++);

    for (j = i; Current[i]; i++)
    {
	c = Current[j] = Current[i];

	if (c != ' ' || !waswhite)
	    j++;

	waswhite = (c == ' ');
    }

    Current[j] = 0;

    if (i != j)
    {
	text_sync ();
	text_redisplaycurrline ();
    }
} /* do_unjustify */


/*DEFHELP #cmd textedit INDENT what how - indent text. @{B}what@{UB} specifies what to indent and @{B}how@{UB} how to indent it. */

DEFUSERCMD("indent", 2, 0, void, do_indent, (void),)
{
    Line   start,
	   end;
    Column column;
    int    indent,
	   dir,
	   align,
	   len,
	   curr_indent,
	   leading,
	   trailing,
	   t;
    char * ptr;
    RP	 * rp;
    LINE * linelist;

    /* decode range */
    ptr = av[1];

    start = end = -1;

    while (*ptr)
    {
	if (*ptr == '.')
	{
	    start = end = Ep->line;
	    break;
	} else if (isdigit(*ptr))
	{
	    if (start == -1)
		start = atoi (ptr);
	    else end = atoi (ptr);

	    while (isdigit(*ptr)) ptr ++;

	    ptr --;
	} else if (*ptr == '$')
	{
	    if (start == -1)
		start = get_pong (*ptr - '0');
	    else
		end = get_pong (*ptr - '0');
	} else if (*ptr == 'b')
	{
	    if (!block_ok())
	    {
		error ("indent block:\nNo block specified");
		return ;
	    }

	    if (!ptr[1])
	    {
		start = ActualBlock.start_line;
		end = ActualBlock.end_line;
	    } else
	    {
		ptr ++;

		if (*ptr == 's')
		    indent = ActualBlock.start_line;
		else
		    indent = ActualBlock.end_line;

		if (start == -1)
		    start = indent;
		else
		    end = indent;
	    }
	} else if (*ptr == 't')
	{
	    start = 1;
	    end = Ep->lines;
	} else if (*ptr == '_')
	{
	    if (start == -1)
		start = Ep->lines;
	    else
		end = Ep->lines;
	}

	ptr ++;
    } /* while (*ptr) */

    if (start == -1)
    {
	error ("indent:\ncannot evaluate start-line");
	return ;
    }

    if (end == -1) end = start;

    if (start > end)
    {
	start ^= end;	    /* Swap ! 8-O */
	end   ^= start;
	start ^= end;
    }

    /* Now find out how to indent */

    ptr = av[2];

    dir = 1;		/* indent, i.e. insert spaces */
    align = 0;		/* just insert `indent' spaces */
    indent = -1;
    column = 0;

    while (*ptr)
    {
	if (*ptr == '-')
	    dir = -1;		/* outdent line, i.e. delete spaces */
	else if (*ptr == '.')
	    align = 1;		/* align line to a multiple of `indent' */
	else if (*ptr == 't')
	    indent = Ep->config.tabstop;
	else if (isdigit(*ptr))
	{
	    indent = atoi (ptr);

	    while (isdigit(*ptr)) ptr++;
	    ptr --;
	} else if (*ptr == 'c')
	    column = Ep->column;

	ptr ++;
    } /* while (*ptr) */

    if (indent == -1)
    {
	error ("indent:\nCannot evaluate indent-amount");
	return ;
    }

    text_sync ();

    /* Note : If the line contains less spaces than indent specifies and
       the user wants to outdent, the line is left-aligned, i.e. no chars
       except spaces are deleted ! This is also true for indent, i.e. no
       chars are moved over the right border (MAXLINELEN chars). */

    linelist = Ep->list + start;

    for (t=start; t <= end; t ++, linelist ++)
    {
	strcpy (Current, linelist[0]);
	Clen = strlen (Current);

	if (Clen <= column) continue;

	ptr = Current + column;

	while (*ptr == ' ') ptr ++;

	if (!*ptr) continue;          /* Empty line */

	leading  = (int)ptr - (int)Current;
	trailing = MAXLINELEN-1 - Clen;

	/* Should I align the line to a multiple of indent or just
	    insert/delete indent spaces ? */
	if (align)
	{
	    /* use appropriate difference */
	    if (dir > 0)
		curr_indent = indent - (leading % indent);
	    else
	    {
		curr_indent = leading % indent;

		if (!curr_indent)
		    curr_indent = indent;
	    }
	} else curr_indent = indent;

	len = strlen (ptr) + 1;

	if (dir > 0)
	{
	    if (trailing < curr_indent)
		curr_indent = trailing;

	    if (!curr_indent) continue;

	    movmem (ptr, ptr+curr_indent, len);
	    setmem (ptr, curr_indent, ' ');
	} else
	{
	    if (leading < curr_indent)
		curr_indent = leading;

	    if (!curr_indent) continue;

	    movmem (ptr, ptr-curr_indent, len);
	}

	Clen = strlen (Current);

#ifdef DEBUG
printf ("Old len %d  New Len %d     BlenOld %d  BlenNew %d\n",
	len, Clen, ((len + 8) & ~7), ((Clen + 8) & ~7));
#endif

	/* Find out if we have to allocate new memory */
	/* Clen is total length in bytes while len is only strlen() */
	/* TODO: Ain't work yet */
	/*if ((((Clen + 8) & ~7) != ((len + 8) & ~7)))
	{*/
	    ptr = allocline (Clen + 1);

	    if (!ptr)
	    {
		nomemory ();
		break;
	    }

	    freeline (linelist[0]);
	    linelist[0] = ptr;
	/*}*/

	strcpy (ptr, Current);
    } /* for (t = start; t <= end; t ++, linelist ++) */

    SETF_MODIFIED(Ep,1);

    text_load ();

    /* Don't display if there is nothing to display */
    if (end < Ep->topline)
	return;

    if (start < Ep->topline)
    {
	start = 0;
	t = end - Ep->topline +1;
    }
    else
    {
	t = end - start +1;
	start -= Ep->topline;

	if (start >= Lines)
	    return;
    }

    if (start+t > Lines)
	t = Lines-start;

    if (!Nsu)
    {
	rp = Ep->win->RPort;

	end = start + t-1;

	/* only clear area, if there is a line outside the block */
	if (!is_inblock (start + Ep->topline, -1) ||
		!is_inblock (end + Ep->topline, -1))
	{
	    SetAPen (rp, TEXT_BPEN(Ep));
	    SetWrMsk (rp, BLOCK_MASK(Ep));
	    RectFill (rp, Xbase, ROW(start), Xpixs, ROW(start+t)-1);
	}

	if (!text_sync ())
	    text_displayseg (start, t);
    }
} /* do_indent */


/*DEFHELP #cmd textedit INSLINES n - insert @{B}n@{UB} lines at once */

DEFUSERCMD("inslines", 1, 0, void, do_inslines, (void),)
{
     int lines = atoi (av[1]);

     if (lines <= 0) return;

     while (lines)
     {
	  do_insline ();
	  lines --;
     }

} /* do_inslines */


/*DEFHELP DELETE - delete characters upto the next blank (if on a blank) or to the next non-blank (if on non-blank) */

DEFUSERCMD("delete", 0, CF_COK, void, do_delete, (void),)
{
    char * ptr, * hptr;

    hptr = ptr = Current + Ep->column;

    if (!*hptr)
	return;

    if (*hptr == ' ')
	while (*hptr == ' ') hptr ++;
    else
	while (*hptr != ' ') hptr ++;

    memmove (ptr, hptr, strlen (hptr)+1);

    text_sync ();
    text_redisplaycurrline ();
} /* do_delete */


/*DEFHELP #cmd textedit DELINES n - delete @{B}n@{UB} lines */

DEFUSERCMD("delines", 1, 0, void, do_delines, (void),)
{
     int t;

     t = atoi (av[1]);

     for ( ; t >= 0; t--)
	  do_deline ();
} /* do_delines */


/*
 *  Commands submitted by Markus Wenzel
 */

/*DEFHELP #cmd misc UNDELINE - insert most recently deleted line (only last line saved) */

DEFUSERCMD("undeline", 0, 0, void, do_undeline, (void),)
{
   do_insline ();
   text_load ();

   strcpy((char *)Current, (char *)Deline);

   text_sync ();
   text_displayseg (Ep->line - Ep->topline, 1);
} /* do_undeline */


/*DEFHELP #cmd block,clip CLIPINS - Insert current contents of clipboard in the text */

DEFUSERCMD("clipins", 0, CF_COK, void, do_clipins, (void),)
{
    struct IOClipReq * ior;
    struct cbbuf * buf;
    LONG    lines;
    BOOL    failed = 0;
    ED	  * ep	   = Ep;

    if (!(ior = CBOpen (0)) )
	return;

    if (CBQueryFTXT (ior))
    {
	buf = CBReadCHRS (ior);

	if (buf)
	{
	    UBYTE line[MAXLINELEN];
	    UBYTE * ptr1, * ptr2;
	    LINE ptr;
	    int len;

	    ptr2 = line;
	    ptr1 = buf->mem;

	    while (*ptr1 && *ptr1 != '\n')
		*ptr2 ++ = *ptr1 ++;

	    *ptr2 = 0;

	    text_write (line);

	    if (*ptr1)
	    {
		do_split ();
		do_down ();
		do_firstcolumn ();

		lines = ep->lines;

		for (ptr1 ++; *ptr1; )
		{
		    ptr2 = line;
		    len = 0;

		    while (*ptr1 && *ptr1 != '\n')
		    {
			*ptr2 ++ = *ptr1 ++;
			len ++;
		    }

		    *ptr2 = 0;
		    len ++;

		    if (*ptr1)
			ptr1 ++;
		    else
		    {
			text_write (line);
			break;
		    }

		    if (makeroom(256) && (ptr = allocline(len)))
		    {
			SETLINE(ep,ep->lines,ptr);
			ep->lines ++;

			movmem(line, ptr, len);
		    }
		    else
		    {
			set_window_params ();
			nomemory ();
			failed = 1;
			break;
		    }
		}

		if (!failed && lines <= ep->lines - 1)
		{
		    Block block;

		    /* INSFILE: move text to correct position */
		    block.ep	     = ep;
		    block.start_line = lines;
		    block.end_line   = ep->lines - 1;
		    block.type	     = BT_LINE;

		    set_block_nud (&block);
		    do_bmove ();
		}
	    } /* more than one line */

	    window_title ();

	    CBFreeBuf (buf);
	}

	CBReadDone (ior);
    }

    CBClose (ior);
} /* do_clipins */


/*DEFHELP SPACE - inserts a single space no matter what insertmode is set to */

DEFUSERCMD("space", 0, CF_COK, void, do_space, (void),)
{
    ED * ep = Ep;
    int  insmode = GETF_INSERTMODE(ep);

    SETF_INSERTMODE(ep,1);
    text_write (" ");
    SETF_INSERTMODE(ep,insmode);
} /* do_space */


/*DEFHELP #cmd textedit,movement RETURN - if AUTOINDENT is off: (FIRST DOWNADD) else insert line, split current line and indent like last line above. */

DEFUSERCMD("return", 0, CF_COK, void, do_return, (void),)
{
    char   buf[MAXLINELEN];
    char * partial;

    if (GETF_COMLINEMODE(Ep))
    {
	strcpy (buf, (char *)Current);

	partial = esc_partial;	/* PATCH_NULL: P -> esc_p */
	esc_partial = NULL;	/* PATCH_NULL: P -> esc_p */

	escapecomlinemode ();

	if (partial)
	{
	    if (do_command (buf))
		do_command (partial);

	    free(partial);
	} else
	    do_command (buf);
    } else
    {
	if (GETF_AUTOINDENT(Ep))
	{   /* If user want's autoindent */
	    WORD   indent;  /* how much */
	    WORD   line;    /* Line-no. */
	    char * ptr;     /* pointer to Ep->list[line] */

	    text_sync ();   /* store actual line */

	    /* Start with current line */
	    indent = 0;
	    ptr = (char *)Current;

	    /* Find 1. nonspace ... */
	    while (isspace (*ptr))
	    {
		ptr ++;
		indent ++;
	    }

	    /* ... but not '\0'. In this case the line is empty and
	       indent is therefore 0 */
	    if (!*ptr)
		indent = 0;

	    /* Try on, if indent is zero, line is empty and there are more
	       lines on top */
	    if (!indent && !*ptr && Ep->line)
	    {
		line = Ep->line - 1;

		/* Until 1. line */
		while (line)
		{
		    /* Try every line */
		    ptr = GETTEXT(Ep,line);
		    indent = 0;

		    while (isspace (*ptr))
		    {
			ptr ++;
			indent ++;
		    }

		    if (indent || *ptr)
			break;

		    line --;
		}
	    }

	    /* Not at last position in line ? then we have to split and
	       indent the rest */
	    if (Ep->column != Clen)
	    {
		/* Split line, move to next line but shouldn't show this */
		do_split ();
		do_down ();

		/* Indent new line */
		if (indent > Ep->column)
		    indent -= indent - Ep->column;

		if (indent > 0)
		{
		    movmem (Current, Current+indent, Clen+1);
		    setmem (Current, indent, ' ');
		    Clen += indent;
		    Ep->column = indent;
		} else
		    Ep->column = 0;

		Current[Clen] = 0;

		/* and position cursor */
		text_sync ();

		/* Show result */
		text_redisplaycurrline ();
	    } else
	    {
		if (Ep->line != Ep->lines-1)
		{
		    do_down ();
		    do_insline ();

		    Ep->column = indent;
		    text_sync ();
		} else
		{
		    do_split ();
		    Ep->column = indent;
		    text_sync ();
		    do_down ();
		}
	    }
	} else
	{
	    Ep->column = 0;
	    text_sync ();
	    do_downadd ();
	}
    } /* !Comlinemode */
} /* do_return */


/******************************************************************************
*****  ENDE edit.c
******************************************************************************/
