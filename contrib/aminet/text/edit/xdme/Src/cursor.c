/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	All routines that only move the cursor.

******************************************************************************/

/**************************************
		Includes
**************************************/
#include <defs.h>
#include <editor.h>
#include <proto/intuition.h>
#include <proto/graphics.h>


/**************************************
	    Globale Variable
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/
static struct TextMarker marker[PINGDEPTH];

/* TODO: replace define by function for infinite history */

#define MaxHistoLines	    40

/* HistoBuff is filled [0..NumHistoLines-1], NumHistoLines is in
   [0..MaxHistoLines], HistoLine is in [0..NumHistoLines]. If NumHistoLines
   is 0, then the HistoryBuffer is empty. */

Prototype char	* HistoBuff[];
Prototype WORD	  NumHistoLines;
Prototype WORD	  HistoLine;

char * HistoBuff[MaxHistoLines];
WORD   NumHistoLines;
WORD   HistoLine;


/**************************************
	   Interne Prototypes
**************************************/


/*DEFHELP #cmd movement UP -cursor up. If in commandline, move to previous line of commandline-history */

DEFUSERCMD("up", 0, CF_VWM|CF_COK, void, do_up, (void),)
{
    ED * ep = Ep;

    if (!GETF_COMLINEMODE(ep))
    {
	if (GetLine(ep))
	{
	    text_sync ();
	    ep->line --;
	    text_load ();

	    if (Ep->line < Ep->topline)
		ScrollAndUpdate (0, -1);
	} else
	    SETF_ABORTCOMMAND(Ep,1);
    } else
    {
	if (HistoLine)
	{
	    HistoLine --;

	    strcpy (Current, HistoBuff[HistoLine]);

	    ep->column = Clen = strlen (Current);
	    ep->topcolumn = GetColumn(ep) - Columns + 1;

	    if (GetTopcolumn(ep) < 0)
		ep->topcolumn = 0;

	    text_redisplaycurrline ();
	}
    }
} /* do_up */


/*DEFHELP #cmd movement DOWN - cursor down. If in commandline move to next line of commandline-history */

DEFUSERCMD("down", 0, CF_VWM|CF_COK, void, do_down, (void),)
{
    ED * ep = Ep;

    if (!GETF_COMLINEMODE(Ep))
    {
	if (GetLine(ep) + 1 < GetLines(ep))
	{
	    text_sync ();
	    ep->line ++;
	    text_load ();

	    if (GetLine(ep) - GetTopline(ep) >= Lines)
		ScrollAndUpdate (0, 1);
	} else
	    SETF_ABORTCOMMAND(Ep,1);
    } else
    { /* Comlinemode */
	if (HistoLine < NumHistoLines)
	{
	    HistoLine ++;

	    if (HistoLine != NumHistoLines)
		strcpy (Current, HistoBuff[HistoLine]);
	    else
		*Current = 0;

	    ep->column = Clen = strlen (Current);
	    ep->topcolumn = GetColumn(ep) - Columns + 1;

	    if (GetTopcolumn(ep) < 0)
		ep->topcolumn = 0;

	    text_redisplaycurrline ();
	}
    } /* if (!Comlinemode) */
} /* do_down */


/*DEFHELP #cmd movement SCROLLDOWN - Scroll down without moving cursor */

DEFUSERCMD("scrolldown", 0, CF_VWM, void, do_scrolldown, (void),)
{
    ED * ep = Ep;

    if (GetTopline(ep) + Lines < GetLines(ep))
    {
	text_sync ();
	ep->line ++;
	text_load ();

	ScrollAndUpdate (0, 1);
    } else
	SETF_ABORTCOMMAND(Ep,1);
} /* do_scrolldown */


/*DEFHELP #cmd movement SCROLLUP - Scroll up without moving cursor */

DEFUSERCMD("scrollup", 0, CF_VWM, void, do_scrollup, (void),)
{
    ED * ep = Ep;

    if (GetTopline(ep))
    {
	text_sync ();
	ep->line --;
	text_load ();

	ScrollAndUpdate (0, -1);
    } else
	SETF_ABORTCOMMAND(Ep,1);
} /* do_scrollup */


/*DEFHELP #cmd movement PAGEDOWN - pagedown a partial page (see PAGESET) */
/*DEFHELP #cmd movement PAGELEFT - page to the left as requested by StyleGuide. */
/*DEFHELP #cmd movement PAGERIGHT - dito to the right */
/*DEFHELP #cmd movement PAGEUP - pageup a partial page (see PAGESET) */
/*DEFHELP #cmd prefs PAGESET n -@{B}n@{UB} PERCENT (0 to 100). page step size relative to the current number of rows in the window. */

DEFUSERCMD("pagedown",  0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageleft",  0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageright", 0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageup",    0, CF_VWM, void, do_page, (void),;)
DEFUSERCMD("pageset",   1, CF_VWM, void, do_page, (void),)
{
    ED	 * ep = Ep;
    Column nx = 0, t;
    Line   ny = 0;

    switch(av[0][4])
    {
	case 'u':
	    if (!GetTopline(ep) && !GetLine(ep)) return;

	    if (!GETF_NICEPAGING(ep) && GetLine(ep) != GetTopline(ep))
	    {
		text_sync ();
		ep->line = ep->topline;
		text_load ();
		return;
	    }

	    ny = Lines * PageJump / 100;

	    text_sync ();

	    ep->line -= ny;

	    if (GetLine(ep) < 0)
		ep-> line = 0;

	    text_load ();

	    if (GetTopline(ep) < ny)
		ny = -GetTopline(ep);
	    else
		ny = -ny;

	    break;

	case 'd':
	    if (GetTopline(ep) + Lines >= GetLines(ep) && GetLine(ep) == GetLines(ep) -1)
		return;

	    text_sync ();

	    if (!GETF_NICEPAGING(ep) && GetLine(ep) != GetTopline(ep) + Lines -1)
		ep->line = GetTopline(ep) + Lines -1;
	    else
	    {
		ny = Lines * PageJump / 100;

		ep->line += ny;

		/* don't advance further if the last line is already visible */
		if (GetTopline(ep)+Lines >= GetLines(ep))
		    ny = 0;
	    }

	    if (GetLine(ep) >= GetLines(ep))
		ep->line = GetLines(ep) - 1;

	    text_load ();

	    break;

	case 'l':
	    if (!GetTopcolumn(ep) && !GetColumn(ep)) return;

	    t = firstns (Current);

	    if (!GETF_NICEPAGING(ep))
	    {
		if (GetColumn(ep) != GetTopcolumn(ep))
		{
		    if (t > GetTopcolumn(ep) && GetColumn(ep) != t)
			ep->column = t;
		    else
			ep->column = GetTopcolumn(ep);

		    text_sync ();
		    return;
		}
	    }
	    else
	    {
		if (t > GetTopcolumn(ep) && GetColumn(ep) != t)
		{
		    ep->column = t;

		    text_sync ();
		    return;
		}
		else if (!t && !GetTopcolumn(ep))
		{
		    if (GetColumn(ep))
		    {
			ep->column = 0;
			text_sync ();
		    }

		    return;
		}
	    }

	    nx = Columns * PageJump / 100;

	    if (GetTopcolumn(ep) < nx)
		nx = GetTopcolumn(ep);

	    if (t < GetColumn(ep) && t > GetColumn(ep) - nx)
		ep->column = t;
	    else
	    {
		if (t == GetColumn(ep) && !GetTopcolumn(ep))
		    ep->column = 0;
		else
		    ep->column -= nx;
	    }

	    nx = -nx;

	    break;

	case 'r':
	    if (GetTopcolumn(ep) + Columns >= MAXLINELEN &&
			GetColumn(ep) == MAXLINELEN-1)
		return;

	    t = LINELEN(ep,GetLine(ep));

	    if (!GETF_NICEPAGING(ep))
	    {
		if (GetColumn(ep) != GetTopcolumn(ep) + Columns -1)
		{
		    if (t < GetTopcolumn(ep) + Columns && t > GetColumn(ep))
			ep->column = t;
		    else
			ep->column = GetTopcolumn(ep) + Columns -1;

		    if (GetColumn(ep) >= MAXLINELEN)
			ep->column = MAXLINELEN - 1;

		    text_sync ();
		    return;
		}
	    }
	    else /* nicepaging */
	    {
		if (t > GetColumn(ep) && t < GetTopcolumn(ep) + Columns)
		{
		    ep->column = t;

		    text_sync ();
		    return;
		}
	    }

	    nx = Columns * PageJump / 100;

	    if (t > GetColumn(ep) && t < GetColumn(ep) + nx)
		ep->column = t;
	    else
		ep->column += nx;

	    if (GetColumn(ep) >= MAXLINELEN)
		ep->column = MAXLINELEN-1;

	    if (GetTopcolumn(ep)+nx > GetColumn(ep))
		nx = (GetColumn(ep) - Columns/2) - GetTopcolumn(ep);

	    if (GetColumn(ep) > Clen)
	    {
		memset (Current+Clen, ' ', GetColumn(ep)-Clen);
		Current[Clen = GetColumn(ep)] = 0;
	    }

	    break;

	case 's':
	    PageJump = atoi ((char *)av[1]);

	    if (PageJump > 100)
		PageJump = 100;
	    else if (PageJump < 1)
		PageJump = 1;

	    break;
    }

    if (!Nsu && (nx || ny))
	ScrollAndUpdate (nx, ny);

} /* do_page */


/*DEFHELP #cmd textedit,movement DOWNADD - cursor down. If at bottom of text, add a line. */

DEFUSERCMD("downadd", 0, 0, void, do_downadd, (void),)
{
    ED	  * ep = Ep;
    UBYTE * ptr;

    if (GetLine(ep) + 1 >= GetLines(ep))
    {
	SETF_MODIFIED (ep, 1);

	if (makeroom (32))
	{
	    ptr = allocline (1);

	    SETLINE(ep,GetLines(ep),ptr);
	    ep->lines ++;

	    if (GETF_SLINE (ep))
		ep->column = 0;
	} else
	{
	    nomemory ();
	}
    }

    do_down ();
} /* do_downadd */


/*DEFHELP #cmd movement LEFT - cursor left */

DEFUSERCMD("left", 0, CF_VWM|CF_COK, void, do_left, (void),)
{
    ED * ep = Ep;

    if (GetColumn(ep))
    {
	ep->column --;

	if (GetColumn(ep) < GetTopcolumn(ep))
	    text_adjust (FALSE);
    }
    else if (GETF_SLINE (ep)) /* do wrap */
    {
	do_up ();
	ep->column = lastns (Current);
    }
    else
	SETF_ABORTCOMMAND(ep,1);
} /* do_left */


/*DEFHELP #cmd movement RIGHT - cursor right */

DEFUSERCMD("right", 0, CF_VWM|CF_COK, void, do_right, (void),)
{
    ED * ep = Ep;

    if (GetColumn(ep) != MAXLINELEN-1)
    {
	if (Current[GetColumn(ep)] == 0)
	{
	    if (GETF_SLINE (ep)) /* do wrap */
	    {
		ep->column = 0;
		do_down ();
		return;
	    }

	    Current[GetColumn(ep)] = ' ';
	    Current[GetColumn(ep)+1]= '\0';

	    Clen ++;
	}

	ep->column ++;

	if (GetColumn(ep) - GetTopcolumn(ep) >= Columns)
	    text_adjust (FALSE);
    }
    else if (GETF_SLINE (ep)) /* do wrap */
    {
	ep->column = 0;
	do_down ();
    }
    else
	SETF_ABORTCOMMAND(Ep,1);
} /* do_right */


/*DEFHELP #cmd movement TAB - forward tab */

DEFUSERCMD("tab", 0, CF_VWM|CF_COK, void, do_tab, (void),)
{
    WORD  n;
    ED	* ep = Ep;

    /* TODO */
    for (n=ep->config.tabstop-(GetColumn(ep) % ep->config.tabstop); n>0; n--)
	do_right ();
} /* do_tab */


/*DEFHELP #cmd movement BACKTAB - backward tab */

DEFUSERCMD("backtab", 0, CF_VWM|CF_COK, void, do_backtab, (void),)
{
    WORD  n;
    ED	* ep = Ep;

    n = GetColumn(ep) % ep->config.tabstop;
    if (!n)
	n = ep->config.tabstop;

    /* TODO */
    for (; n > 0; --n)
	do_left ();
} /* do_backtab */


/*DEFHELP #cmd movement TOP - Move to Top of File */

DEFUSERCMD("top", 0, CF_VWM, void, do_top, (void),)
{
    text_sync ();
    Ep->line = 0;
    text_load ();
    text_adjust (FALSE);
} /* do_top */


/*DEFHELP #cmd movement BOTTOM - Move to Bottom of File */

DEFUSERCMD("bottom", 0, CF_VWM, void, do_bottom, (void),)
{
    text_sync ();
    Ep->line = Ep->lines - 1;
    text_load ();
    text_adjust (FALSE);
} /* do_bottom */


/*DEFHELP #cmd movement FIRST - move to column 1 */

DEFUSERCMD("first", 0, CF_VWM|CF_COK, void, do_firstcolumn, (void),)
{
    if (Ep->column)
    {
	Ep->column = 0;
	text_adjust (FALSE);
    }
} /* do_firstcolumn */


/*DEFHELP #cmd movement FIRSTNB - Move to first non-blank in line. */

DEFUSERCMD("firstnb", 0, CF_VWM|CF_COK, void, do_firstnb, (void),)
{
    for (Ep->column = 0; Current[Ep->column] == ' '; Ep->column ++);

    if (Current[Ep->column] == 0)
	Ep->column = 0;

    text_adjust (FALSE);
} /* do_firstnb */


/*DEFHELP #cmd movement LAST - move one beyond the last non-space in a line. */

DEFUSERCMD("last", 0, CF_VWM|CF_COK, void, do_lastcolumn, (void),)
{
    WORD  i;

    text_sync ();

    i = (GETF_COMLINEMODE(Ep)) ? Clen : LINELEN(Ep,Ep->line);

    if (i != Ep->column)
    {
	Ep->column = i;
	text_adjust (FALSE);
    }
} /* do_lastcolumn */


/*DEFHELP #cmd movement GOTO dest - Goto to a position in the text. */

DEFUSERCMD("goto", 1, CF_VWM, void, do_goto, (void),)
{
    Line   line;
    Column col;
    long   i;
    char * ptr = (char *)av[1];

    i	 = 0;
    line = -1;
    col  = -1;

    switch (*ptr)
    {
	case 'b':
	case 's':
	   if (ActualBlock.type != BT_NONE && Ep == ActualBlock.ep)
	   {
	       line = ActualBlock.start_line;

	       if (ActualBlock.type != BT_LINE)
		   col = ActualBlock.start_column;
	   }
	break;

	case 'e':
	    if (ActualBlock.type != BT_NONE && Ep == ActualBlock.ep)
	    {
		line = ActualBlock.end_line;

		if (ActualBlock.type != BT_LINE)
		    col = ActualBlock.end_column;
	    }
	break;

	case '+':
	    i = 1;

	case '-':
	    line = Ep->line;

	default:
	    line += atoi(ptr+i);
    } /* switch (*ptr) */

    if (line >= Ep->lines)
	line = Ep->lines - 1;

    if (line < 0)
	line = 0;

    text_sync ();

    if (col != -1)
	Ep->column = col;

    Ep->line = line;

    text_load ();
    text_adjust (FALSE);
} /* do_goto */


/*DEFHELP #cmd movement SCREENTOP - Move cursor to the top of the screen */

DEFUSERCMD("screentop", 0, CF_VWM, void, do_screentop, (void),)
{
    text_sync ();
    Ep->line = Ep->topline;
    text_load ();
    text_adjust (FALSE);
} /* do_screentop */


/*DEFHELP #cmd movement SCREENBOTTOM - Move cursor to the bottom of the screen. */

DEFUSERCMD("screenbottom", 0, CF_VWM, void, do_screenbottom, (void),)
{
    text_sync ();

    Ep->line = Ep->topline + Lines - 1;

    if (Ep->line < 0 || Ep->line >= Ep->lines)
	Ep->line = Ep->lines - 1;

    text_load ();
    text_adjust (FALSE);
} /* do_screenbottom */


/*DEFHELP #cmd movement WLEFT - move to beginning of previous word. If in the middle of a word, move to beginning of current word. */

DEFUSERCMD("wleft", 0, CF_VWM|CF_COK, void, do_wleft, (void),)
{
    ED * ep = Ep;
    int  i;

    for (;;)
    {
	i = GetColumn(ep);

	if (i == 0)
	    goto prevline;

	i --;

	while (i && Current[i] == ' ')
	    i --;

	if (i == 0 && Current[0] == ' ')
	{
prevline:
	    if (GETF_COMLINEMODE(Ep) || GetLine(ep) == 0)
	    {
		i = GetColumn(ep);
		break;
	    }

	    text_sync ();

	    ep->line --;

	    text_load ();

	    ep->column = Clen;
	    continue;
	} /* while !SOL && isspace(Current[i]) */

	while (i && Current[i] != ' ')
	    i --;

	if (Current[i] == ' ')
	    i ++;

	break;
    } /* forever */

    ep->column = i;

    text_adjust (FALSE);
} /* do_wleft */


/*DEFHELP #cmd movement WRIGHT - move to beginning of next word */

DEFUSERCMD("wright", 0, CF_VWM|CF_COK, void, do_wright, (void),)
{
    ED * ep = Ep;
    int  i;

    for (;;)
    {
	i = GetColumn(ep);

	if (i == Clen)
	    goto nextline;

	while (i != Clen && Current[i] != ' ')  /* skip past current word */
	    i ++;

	while (i != Clen && Current[i] == ' ')  /* to beg. of next word   */
	    i ++;

	if (i == Clen)
	{
nextline:
	    if (GETF_COMLINEMODE(Ep) || GetLine(ep) == GetLines(ep) - 1)
	    {
		i = GetColumn(ep);
		break;
	    }

	    text_sync ();

	    ep->line ++;

	    text_load ();

	    ep->column = i = 0;

	    if (Current[0] != ' ')
		break;

	    continue;
	}

	break;
    } /* forever */

    ep->column = i;

    text_adjust (FALSE);
} /* do_wright() */


/*DEFHELP #cmd movement COL n - Move cursor to column @{B}n@{UB} or @{B}n@{UB} characters left (@{B}-n@{UB}) or right (@{B}+n@{UB}) */

DEFUSERCMD("col", 1, CF_VWM|CF_COK, void, do_col, (void),)
{
    int    col;
    char * ptr = (char *)av[1];

    switch(*ptr)
    {
	case '+':
	    col = text_colno() + atoi(ptr + 1);
	    if (col >= MAXLINELEN)
		col = MAXLINELEN-1;
	break;

	case '-':
	    col = text_colno() + atoi(ptr);
	    if (col < 0)
		col = 0;
	break;

	default:
	    col = atoi(ptr) - 1;
	break;
    }

    if (col > MAXLINELEN-1 || col < 0)
    {
	SETF_ABORTCOMMAND(Ep,1);
	return;
    }

    while (Clen < col)
	Current[Clen ++] = ' ';

    Current[Clen] = 0;

    Ep->column = col;

    text_adjust (FALSE);
} /* do_col */


/*DEFHELP #cmd movement SCROLLLEFT - Scroll left without moving cursor */
/*DEFHELP #cmd movement SCROLLRIGHT - Scroll right without moving cursor */

DEFUSERCMD("scrollleft",  0, CF_VWM, void, do_scroll, (int dir),;)
DEFUSERCMD("scrollright", 0, CF_VWM, void, do_scroll, (int dir),)
{
    ED	 * ep = Ep;
    UWORD step	  = 4;

    if (Nsu)
	return;

    if (dir == -1)
    {
	if (av[0][6] == 'l')
	    dir = SCROLL_LEFT;
	else
	    dir = SCROLL_RIGHT;
    }

    if (dir == SCROLL_LEFT)
    {
	if (!GetTopcolumn(ep))
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    return;
	}

	if (GetTopcolumn(ep) < step)
	    step = GetTopcolumn(ep);
    } else if (dir == SCROLL_RIGHT)
    {
	if ((GetTopcolumn(ep) + Columns) >= MAXLINELEN)
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    return;
	}

	if ((GetTopcolumn(ep) + Columns + step) >= MAXLINELEN)
	    step = MAXLINELEN - (GetTopcolumn(ep) + Columns);
    }

    if (dir == SCROLL_LEFT)
    {
	/* GetTopcolumn(ep) -= step; */
	ep->column    -= step;
    } else
    {
	UWORD end;

	/* fill Current with spaces */
	end = GetColumn(ep) + step;

	for ( ; Clen<end; Clen++)
	    Current[Clen] = ' ';

	Current[Clen] = 0;

	/* GetTopcolumn(ep) += step; */
	ep->column    += step;
    }

    ScrollAndUpdate (step, 0);
    /* scroll_display ((dir == SCROLL_LEFT ? -step : step), 0,
	    GetTopcolumn(ep), GetTopline(ep),
	    GetTopcolumn(ep) + Columns, GetTopline(ep) + Lines); */
} /* do_scroll */



Prototype int get_char (ED * ep, struct pos * curr, int dir);

int get_char (ED * ep, struct pos * curr, int dir)
{
    int c;

    if (dir == 0)
    {
	curr->ptr = CONTENTS(Current);
	curr->len = strlen (curr->ptr);
	curr->lin = Ep->line;
	curr->col = Ep->column;

	if (curr->col > curr->len)
	    curr->col = curr->len - 1;

	c = curr->ptr[curr->col];
    }
    else if (dir < 0)
    {
	if (curr->col < 0)
	{
	    do
	    {
		curr->lin --;

		if (curr->lin < 0)
		    return (-1);

		curr->ptr = GETTEXT(ep,curr->lin);
		curr->len = strlen (curr->ptr);
	    } while (!curr->len);

	    curr->col = curr->len-1;
	} /* col out of range */

	c = curr->ptr[curr->col];

	curr->col --;
    }
    else
    {
	if (curr->col > curr->len)
	{
	    do
	    {
		curr->lin ++;

		if (curr->lin >= GetLines(ep))
		    return (-1);

		curr->ptr = GETTEXT(ep,curr->lin);
		curr->len = strlen (curr->ptr);
	    } while (!curr->len);

	    curr->col = 0;
	} /* col out of range */

	c = curr->ptr[curr->col];

	curr->col ++;
    }

    return (c);
}


/*DEFHELP #cmd movement MATCH - find matching paren. Works with (), [], {}, `' and C-comments */

DEFUSERCMD("match", 0, CF_VWM|CF_COK, void, do_match, (void),)
{
    char   dec, 	/* char that indicates a "level_down"   */
	   inc; 	/*	    -""-         "level_up"     */
    WORD   dir; 	/* direction of search			*/
    WORD   level;	/* "level"                              */
    int    c;
    struct pos curr;	/* current position */

    /* set start values */
    inc = get_char (Ep, &curr, 0);

    /* Check for a known paren and select the search-direction accordingly. */
    switch (inc)
    {
	case '{':
	    dir = 1;
	    dec = '}';
	break;

	case '}':
	    dir = -1;
	    dec = '{';
	break;

	case '[':
	    dir = 1;
	    dec = ']';
	break;

	case ']':
	    dir = -1;
	    dec = '[';
	break;

	case '(':
	    dir = 1;
	    dec = ')';
	break;

	case ')':
	    dir = -1;
	    dec = '(';
	break;

	case '`':
	    dir = 1;
	    dec = '\'';
	break;

	case '\'':
	    dir = -1;
	    dec = '`';
	break;

	case '*':
	case '/': {
	    int ln, nested;

	    {
		int pre, next;

		pre = next = 0;

		if (curr.col)
		    pre = curr.ptr[curr.col-1];

		if (curr.col != curr.len-1)
		    next = curr.ptr[curr.col+1];

		if (inc == '*')
		{
		    if (pre == '/')
		    {
			if (next == '/')
			{
			    warn ("match: ambigous comment");
			    return;
			}
			else
			    dir = 1;	/* / * x */
		    }
		    else
		    {
			if (next == '/')
			    dir = -1;	/* x * / */
			else
			    return;	/* x * x */
		    }

		    /* skip '*' */
		    get_char (Ep, &curr, dir);
		}
		else /* inc == '/' */
		{
		    if (pre == '*')
		    {
			if (next == '*')
			{
			    warn ("match: ambigous comment");
			    return;
			}
			else
			    dir = -1;	/* * / x */
		    }
		    else
		    {
			if (next == '*')
			    dir = 1;	/* x / * */
			else
			    return;	/* x / x */
		    }

		    /* skip '/*' */
		    get_char (Ep, &curr, dir);
		    get_char (Ep, &curr, dir);
		}
	    } /* block */

	    nested = FALSE;
	    level = 1;		/* we _are_ inside the comment */

	    /* now look for the first * /-sequence (get_char returns
	       the next char in the specified direction and from within
	       the comment, * / always terminates it */
	    for (;;)
	    {
		ln = curr.lin;
		c = get_char (Ep, &curr, dir);

		if (c == -1)
		    goto not_found;

		/* Zeilenwechsel ? -> Trenner */
		if (ln != curr.lin)
		    continue;

		while (c == '/')
		{
		    c = get_char (Ep, &curr, dir);

		    if (c == -1)
			goto not_found;

		    if (c == '*')
		    {
			level ++;
			nested = TRUE;
		    }
		}

		while (c == '*')
		{
		    c = get_char (Ep, &curr, dir);

		    if (c == -1)
			goto not_found;

		    if (c == '/')
		    {
			level --;

			if (!level)
			    goto found;
		    }
		}
	    }

	    if (nested)
		warn ("match: nested commend");
	break; }

	default:
	    dir = 0;
	break;
    }

    /* if there is no known paren, just return */
    if (!dir)
	return;

    /* we start with level 0. Since we ARE on a paren, this is increased soon */
    level = 0;

    for (;;)
    {
	c = get_char (Ep, &curr, dir);

	if (c == -1)
	{
not_found:
	    warn ("match: No matching paren");
	    return;
	}

	if (c == inc)
	{ /* check for "inc-char" */
	    level ++;
	} else
	{
	    if (c == dec)
	    { /* dito */
		level --;

		/* if we have reached the start-level, we are through */
		if (!level)
		    break;
	    }
	}
    }

found:
    text_sync ();

    if (curr.lin >= Ep->lines)
	curr.lin = Ep->lines - 1;

    Ep->line   = curr.lin;
    Ep->column = curr.col - dir;    /* undo last oper. in get_char() */

    text_load ();
    text_adjust (FALSE);
} /* do_match */


/*DEFHELP #cmd movement PING n - set a text marker (@{B}n@{UB} = 0-9). */

DEFUSERCMD("ping", 1, CF_VWM|CF_ICO, void, do_ping, (void),)
{
    UWORD num = atoi (av[1]);

    if (num >= PINGDEPTH)
    {
	error ("ping:\n%ld out of range\n(max %ld)", num, PINGDEPTH);
	return;
    }

    marker[num].ep     = Ep;
    marker[num].column = Ep->column;
    marker[num].line   = Ep->line;

    title ("Position marked");
} /* do_ping */


/*DEFHELP #cmd movement PONG n - move to a previously set text marker (@{B}n@{UB} = 0-9) */

DEFUSERCMD("pong", 1, CF_VWM, void, do_pong, (void),)
{
    WORD  num = atoi (av[1]);

    text_sync ();

    if (num < 0 || num >= PINGDEPTH || !marker[num].ep)
    {
	error ("pong:\nrange error or\nyet nothing marked");
	return;
    }

    text_cursor (1);
    switch_ed (marker[num].ep);
    text_cursor (0);

    if (IntuitionBase->ActiveWindow != Ep->win)
    {
	WindowToFront (Ep->win);
	ActivateWindow (Ep->win);
    }

    if ((Ep->line = marker[num].line) >= Ep->lines)
    {
	marker[num].line = Ep->line = Ep->lines - 1;
    }

    Ep->column = marker[num].column;

    text_load ();
    text_adjust (FALSE);
} /* do_pong */


Prototype int get_pong (int num);

int get_pong (int num)
{
     if (num < 0 || num >= PINGDEPTH || !marker[num].ep)
     {
	  return (-1);
     }

     if (marker[num].line >= Ep->lines)
	  return (marker[num].line = Ep->lines-1);

     return (marker[num].line);
} /* get_pong */


static int Savetopline, Savecolumn, Savetopcolumn;

/*DEFHELP #cmd misc RECALL - recall most recently entered command. Must be used from a keymap (c-esc). */

DEFUSERCMD("recall", 0, CF_VWM, void, do_recall, (void),)
{
    av[0] = "escimm";

    /* Copy last Histoline if possible */
    if (NumHistoLines)
	av[1] = HistoBuff[NumHistoLines - 1];
    else
	av[1] = "";

    do_esc ();

    if (NumHistoLines)
	HistoLine --;
} /* do_recall */


/*DEFHELP #cmd misc ESC - toggle manual command entry mode */
/*DEFHELP #cmd misc ESCIMM arg - go into command entry mode prompting with @{B}arg@{UB} */

DEFUSERCMD("escimm", 1, CF_VWM|CF_PAR, void, do_esc, (void),;)
DEFUSERCMD("esc",    0, CF_VWM|CF_COK, void, do_esc, (void),)
{
    ED	* ep = Ep;
    RP	* rp = ep->win->RPort;
    WORD  y;

    if (GETF_COMLINEMODE(ep))
    {
	escapecomlinemode ();
	return;
    } else
    {			 /* PATCH_NULL added */
	esc_partial = Partial;	/* PATCH_NULL added */
	Partial = NULL; 	/* PATCH_NULL added */
    }

    text_sync ();

    if (av[0][3] == 'i')
	strcpy ((char *)Current, (char *)av[1]);
    else
	Current[0] = 0;

    Clen = strlen ((char *)Current);
    SETF_COMLINEMODE(ep,1);

    /* returnoveride (1); */            /* PATCH_NULL [25 Jan 1993] : commented out */

    Savetopline   = GetTopline(ep);
    Savecolumn	  = GetColumn(ep);
    Savetopcolumn = GetTopcolumn(ep);

    ep->column = Clen;
    ep->topcolumn = GetColumn(ep) - Columns + 1;

    if (GetColumn(ep) < Columns)
	ep->topcolumn = 0;

    ep->topline = GetLine(ep) - Lines + 1;

    y = ROW (Lines-1);
    SetAPen (rp, TEXT_BPEN(ep));
    SetWrMsk (rp, ALL_MASK);
    RectFill (rp, COL(0), y, Xpixs, Ypixs);

    y --;
    SetAPen (rp, TEXT_FPEN(ep));
    RectFill (rp, COL(0), y, Xpixs, y);

    text_displayseg (Lines - 1, 1);

    text_cursor (1);

    HistoLine = NumHistoLines;
} /* do_esc */


Prototype void escapecomlinemode (void);

void escapecomlinemode (void)
{
    ED	 * ep = Ep;
    RP	 * rp = ep->win->RPort;
    char * ptr;

    if (esc_partial)
    {	   /* PATCH_NULL P -> esc_p */
	free(esc_partial);  /* PATCH_NULL P -> esc_p */
	esc_partial = NULL; /* PATCH_NULL P -> esc_p */
    }

    if (GETF_COMLINEMODE(ep))
    {
	/* Don't add empty lines */
	if (*Current)
	{
	    ptr = Current + strlen (Current) -1;

	    while (*ptr == ' ' && ptr != (char *)Current)
		ptr --;

	    /* Set string-end after last char. Note that this code
	       removes histo-lines with length 1 (single chars) */
	    if (ptr != (char *)Current)
		ptr ++;

	    *ptr = 0;
	}

	/* If we are somewhere above and we didn't change anything,
	   we won't add a new history-line */
	/* If there is something to add */
	if (!(HistoLine != NumHistoLines && !strcmp (Current,
		HistoBuff[HistoLine])) && *Current)
		{
	    /* Check if buffer is full */
	    if (NumHistoLines == MaxHistoLines)
	    {
		/* Free oldest line and shift buffer */
		free (HistoBuff[0]);

		/*movmem (HistoBuff + 1, HistoBuff,
			sizeof (char *) * (MaxHistoLines - 1));*/
		memmove (HistoBuff, HistoBuff + 1,
			sizeof (char *) * (MaxHistoLines - 1));

		/* We have now space for one line */
		NumHistoLines --;
	    }

	    /* Allocate new line */
	    ptr = malloc (strlen (Current) + 1);

	    /* Copy new line if possible. Note that even if malloc()
	       failed we don't loose information. */
	    if (ptr)
	    {
		strcpy (ptr, (char *)Current);

		/* Add line. At this point, NumHistoLines is ALWAYS
		   less than MaxHistoLines ! */
		HistoBuff[NumHistoLines ++] = ptr;
	    }
	}

	SETF_COMLINEMODE(ep,0);

	/* returnoveride (0); */        /* PATCH_NULL [25 Jan 1993] : commented out */

	ep->topline   = Savetopline;
	ep->column    = Savecolumn;
	ep->topcolumn = Savetopcolumn;

	text_load ();

	SetAPen (rp, TEXT_BPEN(ep));
	SetWrMsk (rp, ALL_MASK);
	RectFill (rp, COL(0), ROW(Lines-1)-1, Xpixs, Ypixs);

	SetAPen (rp, TEXT_FPEN(ep));
	text_displayseg (Lines - 2, 2);
    }
} /* escapecomlinemode */


Prototype void markerkill (ED * ep);

void markerkill (ED * ep)
{
    WORD  i;

    for (i = 0; i < PINGDEPTH; ++i)
    {	    /*	remove ping-pong marks	*/
	if (marker[i].ep == ep)
	    marker[i].ep = NULL;
    }
} /* markerkill */


/******************************************************************************
*****  ENDE cursor.c
******************************************************************************/
