/******************************************************************************

    MODUL
	$Id$

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#define MYDEBUG 1
#include "debug.h"

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


/*****************************************************************************

    NAME
	uninit_init

    PARAMETER
	ED * ep;

    RETURN
	ED * new_ep;

    DESCRIPTION
	Resets an editor-struct.

******************************************************************************/

Prototype ED * uninit_init (ED * ep);

ED * uninit_init (ED * ep)
{
    FONT	  * font;
    BPTR	    lock   = DupLock (ep->dirlock);
    WIN 	  * win;
    struct Gadget * gadg;
    int 	    bufsiz = CONFIG_SIZE;
    char	  * buf    = malloc (bufsiz);

    /* get buffer for config */
    if (!buf)
    {
	nomemory ();

	warn ("Out of memory !");

	return (NULL);
    }

    movmem (&ep->beginconfig, buf, bufsiz);

    /* remember window, font and gadget */
    win 	= ep->win;
    font	= ep->font;
    gadg	= ep->propgad;
    ep->font	= NULL;

    /* throw anything away */
    text_uninit ();

    /* init editor */
    text_init (Ep, NULL, NULL);

    /* switch to new one */
    ep = Ep;

    /* close font if there is one */
    if (ep->font)
	CloseFont (ep->font);

    /* reset window */
    ep->win  = win;

    /* set new font */
    if ( (ep->font = font) )
    {
	SetFont (ep->win->RPort, font);
	set_window_params ();
    }

    /* set gadget */
    ep->propgad = gadg;

    /* copy config back */
    movmem (buf, &ep->beginconfig, bufsiz);

    /* no modifications. Set cursor to beginning */
    SETF_MODIFIED (ep, 0);
    ep->line	 = ep->topline = 0;

    /* create new lock */
    UnLock (ep->dirlock);
    ep->dirlock = (BPTR)lock;

    /* free buffer */
    free (buf);

    return (ep);
} /* uninit_init */


/*****************************************************************************

    NAME
	setpen

    PARAMETER
	Line line, Column column

    RETURN
	same as is_inblock()

    DESCRIPTION
	Chooses the correct pen (color) for the given position.

******************************************************************************/

/* Return: Same as is_inblock() ! */
Prototype int setpen (Line line, Column column);

int setpen (Line line, Column column)
{
    ED	* ep   = Ep;
    RP	* rp   = ep->win->RPort;
    WORD  test;

    test = is_inblock (line, column);

    if (test & BP_INSIDE)
    {
	SetAPen (rp,  BLOCK_FPEN(ep));
	SetBPen (rp,  BLOCK_BPEN(ep));
	SetWrMsk (rp, BLOCK_MASK(ep));
    } else
    {
	SetAPen (rp,  TEXT_FPEN(ep));
	SetBPen (rp,  TEXT_BPEN(ep));
	SetWrMsk (rp, TEXT_MASK(ep));
    }

    return (test);
} /* setpen */


/*****************************************************************************

    NAME

    PARAMETER

    RETURN

    DESCRIPTION

******************************************************************************/

Prototype WORD inversemode (int n);

WORD inversemode (int n)
{
    RP	* rp = Ep->win->RPort;
    WORD  test;
    UBYTE fgpen;
    UBYTE bgpen;

    test = setpen (Ep->line, Ep->column);

    if (n)
    {
	fgpen = ~rp->FgPen;
	bgpen = ~rp->BgPen;

	SetAPen (rp, bgpen);
	SetBPen (rp, fgpen);
	SetDrMd (rp, JAM2|INVERSVID);
    } else
	SetDrMd (rp, JAM2);

    SetWrMsk (rp, ALL_MASK);

    return ((WORD)(test & BP_INSIDE));
} /* inversemode */


/*****************************************************************************

    NAME

    PARAMETER

    RETURN

    DESCRIPTION

******************************************************************************/

Prototype int text_init (ED * oldep, WIN * win, struct NewWindow * nw);

int text_init (ED * oldep, WIN * win, struct NewWindow * nw)
{
    ED * ep;

    if (!(ep = (ED *)allocb (sizeof(ED))) )
	return (0);

    memset (ep, 0, sizeof(ED));
    ep->win = win;

    if (oldep)
    {
	ep->dirlock = (long)DupLock (oldep->dirlock);

	movmem (&oldep->beginconfig, &ep->beginconfig, CONFIG_SIZE);

	if (oldep->font)
	{
	    ep->font = oldep->font;
	    ep->font->tf_Accessors ++;

	    if (win)
		SetFont (win->RPort, ep->font);
	}

	/* change oldep to "last editor" to have the iconified window
	   positioned right */
	oldep = GetTail ((LIST *)&DBase);

	if (GETF_ICONMODE(oldep))
	{
	    ep->config.iwinx = oldep->win->LeftEdge;
	    ep->config.iwiny = oldep->win->TopEdge;
	} else
	{
	    ep->config.iwinx = oldep->config.iwinx;
	    ep->config.iwiny = oldep->config.iwiny;
	}

	ep->config.iwinx += TextLength (oldep->win->RPort, oldep->name,
			    strlen (oldep->name)) + 60;

	if (ep->config.iwinx > oldep->win->WScreen->Width)
	{
	    ep->config.iwinx = 0;

	    if(Ep->win->WScreen->Font != NULL)
		/* height */
		ep->config.iwiny += oldep->win->WScreen->Font->ta_YSize + 3;
	    else
		ep->config.iwiny += GfxBase->DefaultFont->tf_YSize + 3;
	}
    }
    else
    {
	PROC * proc = (PROC *)FindTask (NULL);

	ep->dirlock = (long)DupLock (proc->pr_CurrentDir);

	loadconfig (ep);
    }

    ep->lines	  = 1;
    ep->maxlines  = 32;
    ep->list	  = (UBYTE **)alloclptr (ep->maxlines);
    SETLINE(ep,0,allocline (1));

    Current[0] = Clen = 0;

    AddTail ((LIST *)&DBase, (NODE *)ep);
    ep->node.ln_Name = ep->name;
    ep->keytable  = currenthash ();      /* PATCH_NULL [01 Feb 1993] : added */
    ep->menustrip = currentmenu ();      /* PATCH_NULL [01 Feb 1993] : added */

    strcpy ((char *)ep->name, "unnamed");
    Ep = ep;	/* no switch_ed() here ! */

    strcpy (ep->wtitle, "  OK  ");

    if (win)
	text_cursor (1);

    if (nw)
    {
	if (!GETF_ICONMODE(ep) && ep->win)
	{
	    nw->LeftEdge = ep->win->LeftEdge;
	    nw->TopEdge  = ep->win->TopEdge;
	    nw->Width	 = ep->win->Width;
	    nw->Height	 = ep->win->Height;
	} else
	{
	    if (ep->config.winwidth && ep->config.winheight)
	    {
		nw->LeftEdge = ep->config.winx;
		nw->TopEdge  = ep->config.winy;
		nw->Width    = ep->config.winwidth;
		nw->Height   = ep->config.winheight;
	    } else
	    {
		nw->LeftEdge = 0;
		nw->TopEdge  = 0;
		nw->Width    = 640;
		nw->Height   = 200;
	    }
	}

	nw->DetailPen = TEXT_BPEN(ep);
	nw->BlockPen  = TEXT_FPEN(ep);
    }

    return (1);
} /* text_init */


/*****************************************************************************

    NAME
	text_switch

    PARAMETER
	WIN * win;	    The window to switch to.

    RETURN
	BOOL found;	    Was the change successful ?

    DESCRIPTION
	Searches for the specified window and makes the editor with that
	window active.

******************************************************************************/

Prototype BOOL text_switch (WIN * win);

BOOL text_switch (WIN * win)
{
    ED * ep;

    /* look for that window */

    for (ep=(ED *)GetHead(&DBase); ep; ep=(ED *)GetSucc((struct Node *)ep))
    {
	if (ep->win == win)
	    break;
    }

    return (switch_ed (ep));
} /* text_switch */


/*****************************************************************************

    NAME
	text_sync

    PARAMETER
	void

    RETURN
	BOOL redraw;		Did text_sync() a screen-refresh ?

    DESCRIPTION
	copy current line to list of lines and say, if screen-update
	is neccessary. Furthermore, Current is filled with spaces upto
	the current column.

******************************************************************************/

Prototype BOOL text_sync (void);

BOOL text_sync (void)
{
    ED	  * ep	   = Ep;
//    char    redraw = 0;
    WORD    len;
    UBYTE * ptr;
    UBYTE * line;

    /* remove trailing spaces */
    len = strlen ((char *)Current)-1;
    ptr = Current + len;

    if (!GETF_SLINE (ep)) /* preserve line length in shortline mode */
    {
	while (len && *ptr == ' ')
	{
	    len --;
	    ptr --;
	}

	if (*ptr == ' ')
	    *ptr = 0;
	else
	{
	    ptr[1] = 0;
	    len ++;
	}
    }
    else
	len ++;

    /* Line length */
    Clen = len;

    if (!GETF_COMLINEMODE(Ep))
    {
	WORD  len_in_blocks, clen_in_blocks;

	line = GETTEXT(ep,ep->line);
	len = strlen ((char *)line);

	/* Find out how many blocks (*8) we use and how many we need */
	len_in_blocks  =  (len + 8) & ~7;
	clen_in_blocks = (Clen + 8) & ~7;

	/* This was highly wasting memory. OS is always padding Alloc's
	   to 8 Bytes. If the length of the line is still within one
	   block but has changed we freed and allocated new memory.
	   This guranteed to fragment memory.

	   Just use the old version and edit some large text. Use avail,
	   save & reload text and "avail" again. The you'll see what I mean.
	   This is fixed now. */

	/* I must not use strcpy() if the line was empty since empty
	   lines are handled differently, i.e. they use static memory.
	   Only allocate new, if the line was empty or if the size in
	   blocks has changed, i.e. we need one more or less block of
	   memory (see AllocMem() in Amiga-ROM-Kernel-Ref.-Manual about
	   blocks). */
	if ((!len && Clen) || len_in_blocks != clen_in_blocks)
	{
	    if ( (ptr = allocline(Clen+1)) )
	    {

#ifdef DEBUG_MEM_H
    printf ("Allocated at %08x\n", ptr);
#endif

		SETF_MODIFIED(ep,1);
		SETF_OVERIDE(ep,0);

		freeline (line);
		SETLINE(ep,ep->line,(LINE)ptr);

		strcpy ((char *)ptr, (char *)Current);
	    } else
	    {
		nomemory ();
		strcpy ((char *)Current, (char *)line);
	    }
	} else
	{ /* Within one block ... */
	    UBYTE * ptr1, * ptr2;

	    ptr1 = line;
	    ptr2 = Current;

	    /* We need not to copy stuff that's equal ! */
	    while (*ptr1 == *ptr2)
	    {
		if (!*ptr1)
		    break;

		ptr1 ++;
		ptr2 ++;
	    }

	    if (*ptr1 != *ptr2)
	    {
		SETF_MODIFIED(ep,1);
		SETF_OVERIDE(ep,0);

/*printf ("Lines \nOld `%s'\nNew `%s'\ndiffer at\n%s\n%s", line, Current, ptr1, ptr2); */

		strcpy ((char *)ptr1, (char *)ptr2);
/*printf ("New Line `%s'\n", line);*/
	    } /* Any changes ? */
	} /* Old block length != new block length */
    } /* if (!Comlinemode) */

error:
    return ((BOOL)text_adjust (FALSE));
} /* text_sync */


/*****************************************************************************

    NAME
	text_adjust

    PARAMETER
	BOOL force;		TRUE: Redraw screen even if the cursor
				      is visible.
				FALSE: Draw only, if cursor is not visible.

    RETURN
	BOOL did_redraw;

    DESCRIPTION
	Make sure the current cursor position is visible.

******************************************************************************/

Prototype BOOL text_adjust (BOOL force);

BOOL text_adjust (BOOL force)
{
    if (!Nsu)
    {
	if (Ep->column - Ep->topcolumn >= Columns || Ep->column < Ep->topcolumn)
	{
	    force = 1;

	    Ep->topcolumn = Ep->column - (Columns>>1);

	    if (Ep->topcolumn < 0)
		Ep->topcolumn = 0;
	}

	if (Ep->line - Ep->topline >= Lines || Ep->line < Ep->topline)
	{
	    force = 1;

	    Ep->topline = Ep->line - (Lines>>1);

	    if (Ep->topline < 0)
		Ep->topline = 0;
	}

	/* need I redraw ?? */
	if (force)
	    text_redisplay ();
    }

    /* pad line with spaces upto cursor */
    while (Ep->column > Clen)
	Current[Clen++] = ' ';

    Current[Clen] = '\0';

    return (force);
} /* text_adjust */


/*****************************************************************************

    NAME
	text_load

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION
	Load current line into Current. Current is padded with spaces
	if the cursor is to the left.

******************************************************************************/

Prototype void text_load (void);

void text_load (void)
{
    if (GETF_COMLINEMODE(Ep))
	return;

    strcpy ((char *)Current, GETTEXT(Ep,Ep->line));

    Clen = strlen ((char *)Current);

    if (GETF_SLINE(Ep) && (Ep->column > Clen))
	Ep->column = Clen; /* PATCH_NULL [14 Jun 1993] : added */

    while (Ep->column > Clen)
	Current[Clen++] = ' ';

    Current[Clen] = '\0';
} /* text_load */


Prototype int text_colno (void);

int text_colno (void)
{
    return((int)Ep->column);
} /* text_colno */


Prototype int text_lineno (void);

int text_lineno (void)
{
    return((int)Ep->line+1);
} /* text_lineno */


Prototype int text_lines (void);

int text_lines (void)
{
    return((int)Ep->lines);
} /* text_lines */


Prototype int text_cols (void);

int text_cols (void)
{
    return((int)Clen);
} /* text_cols */


Prototype int text_imode (void);

int text_imode (void)
{
    return ((int)GETF_INSERTMODE(Ep));
} /* text_imode */


Prototype int text_tabsize (void);

int text_tabsize (void)
{
    return((int)Ep->config.tabstop);
} /* text_tabsize */


Prototype UBYTE * text_name (void);

UBYTE * text_name (void)
{
    return(Ep->name);
} /* text_name */


/*****************************************************************************

    NAME

    PARAMETER

    RETURN

    DESCRIPTION

******************************************************************************/

Prototype void text_uninit (void);

void text_uninit (void)
{
    ED * ep = Ep;

    if (ep)
    {
	markerkill (ep);
	check_stack (ep);

	freelist (ep->list, ep->lines);
	FreeMem (ep->list, ep->maxlines * sizeof(LINE));

	DelAllVarsFromTree((void **)&ep->textvars); /* PATCH_NULL: local vars have to be freed */

	if (ActualBlock.ep == ep)
	    unblock ();

	Remove ((NODE *)ep);

	if (ep->font)
	{
	    SetFont (ep->win->RPort, ep->win->WScreen->RastPort.Font);
	    CloseFont (ep->font);
	}

	UnLock (ep->dirlock);
	FreeMem (ep, sizeof(ED));

	if ( (ep = (ED *)GetHead (&DBase)) )
	{
	    Ep = NULL;

	    switch_ed (ep);

	    text_load ();
	} else
	    Ep = NULL;
    }
} /* text_uninit */


/*****************************************************************************

    NAME

    PARAMETER

    RETURN

    DESCRIPTION

******************************************************************************/

Prototype void text_cursor (int n);

void text_cursor (int n)
{
    ED	* ep   = Ep;
    RP	* rp   = ep->win->RPort;

    if (GETF_ICONMODE(Ep) || Nsu)
	return;

    movetocursor ();
    inversemode (n);

    if (Current[ep->column])
    {
	Text (rp, Current + ep->column, 1);
    } else
    {
	Text (rp, (STRPTR)" ", 1);
    }

    if (n && GETF_FOLLOWCURSOR(Ep))
	do_makecursorvisible ();

    inversemode (0);

} /* text_cursor */


/*****************************************************************************

    NAME

    PARAMETER

    RETURN

    DESCRIPTION

******************************************************************************/

Prototype void text_position (int col, int row);

void text_position (int col, int row)
{
    ED	* ep	 = Ep;
    WORD  column,
	  line;

    text_sync ();

    column = ep->topcolumn + col;

    if (column > MAXLINELEN-1)
	column = MAXLINELEN-1;

    if (column < 0)
	column = 0;

    line = ep->topline + row;

    if (line >= ep->lines)
	line = ep->lines - 1;

    if (line < 0)
	line = 0;

    ep->column = column;
    ep->line = line;

    text_load ();
    text_adjust (FALSE);
} /* text_position */


/*****************************************************************************

    NAME
	text_displayseg

    PARAMETER
	int start;
	int lines;

    RETURN
	void

    DESCRIPTION

******************************************************************************/

Prototype void text_displayseg (int start, int lines);

void text_displayseg (int start, int lines)
{
    ED * ep = Ep;

    if (Nsu || GETF_ICONMODE(ep)) return;

    if (GETF_COMLINEMODE(Ep))
    {
	text_redraw_cmdline ();
    } else
    {
	Line   start_line;

	start_line = start + ep->topline;

	redraw_block (TRUE, start_line, ep->topcolumn,
			start_line + lines-1, ep->topcolumn + Columns-1);

	prop_adj ();
    } /* !Comlinemode */
} /* text_displayseg */


/*****************************************************************************

    NAME
	text_redraw_cmdline

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION

******************************************************************************/

Prototype void text_redraw_cmdline (void);

void text_redraw_cmdline (void)
{
    RP * rp = Ep->win->RPort;
    int  len;
    int  topline = Ep->line - Ep->topline;

    /* Clear back */
    SetAPen  (rp, TEXT_BPEN(Ep));
    SetWrMsk (rp, BLOCK_MASK(Ep));
    RectFill (rp, Xbase, ROW(topline), Xpixs, Ypixs);

    /* Write text */
    SetAPen (rp, TEXT_FPEN(Ep));
    SetBPen (rp, TEXT_BPEN(Ep));

    len = Clen - Ep->topcolumn;

    if (len > 0)
    {
	Move (rp, XTbase, ROWT(topline));
	Text (rp, Current + Ep->topcolumn, (len > Columns) ? Columns : len);
    }

    SetWrMsk (rp, ALL_MASK);
} /* text_redraw_cmdline */


/*****************************************************************************

    NAME
	do_redisplay

    PARAMETER
	void

    RETURN
       void

    DESCRIPTION

******************************************************************************/

/*DEFHELP #cmd win,misc REDISPLAY - force XDME to redraw the text */

DEFUSERCMD("redisplay", 0, CF_VWM, void, do_redisplay, (void),)
{
    text_adjust (TRUE);
} /* do_redisplay */


/*****************************************************************************

    NAME
	text_redisplay

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION

******************************************************************************/

Prototype void text_redisplay (void);

void text_redisplay (void)
{
    ED * ep = Ep;
    RP * rp = ep->win->RPort;

    if (!Nsu)
    {
	if (!GETF_COMLINEMODE(Ep))
	{
	    SetAPen (rp, TEXT_BPEN(ep));
	    SetWrMsk (rp, ALL_MASK);

	    RectFill (rp, Xbase, Ybase, Xpixs, Ypixs);
	}

	text_displayseg (0, Lines);
    }
} /* text_redisplay */


/*****************************************************************************

    NAME
	text_redisplaycurrline

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION
	Redrenders the current line.

******************************************************************************/

Prototype void text_redisplaycurrline (void);

void text_redisplaycurrline (void)
{
    ED * ep  = Ep;
    RP * rp  = ep->win->RPort;
    int  row = ep->line - ep->topline;

    if (!Nsu)
    {
	if (GETF_COMLINEMODE(Ep))
	{
	    text_displayseg (0, Lines);
	} else {
	    SetAPen (rp, TEXT_BPEN(ep));
	    SetWrMsk (rp, ALL_MASK);
	    RectFill (rp, Xbase, ROW(row), Xpixs, ROW(row+1)-1);

	    redraw_block (TRUE, ep->line, ep->topcolumn, ep->line,
		    ep->topcolumn + Columns - 1);
	}
    }
} /* text_redisplaycurrline */


/*****************************************************************************

    NAME
	text_write

    PARAMETER
	UBYTE * str;

    RETURN
	void

    DESCRIPTION
	Inserts the string str into the text.

******************************************************************************/

Prototype void text_write (UBYTE * str);

void text_write (UBYTE * str)
{
    WORD  len = strlen ((char *)str);
    WORD  i;
    ED	* ep  = Ep;
    RP	* rp  = ep->win->RPort;

    if (Clen + len >= MAXLINELEN-1)
    {
	text_sync ();
	text_load ();
    }

    if (!GETF_INSERTMODE(ep))
    {
	UWORD line, column_start, column_end;

	if (ep->column + len >= MAXLINELEN-1)
	    goto fail;

	movmem (str, Current + ep->column, len);

	/* clear area */
	line = ep->line - ep->topline;
	column_start = ep->column - ep->topcolumn;
	column_end = column_start + len;

	/* only draw, if we need to (otherwise the screen is refreshed !) */
	if (column_end < Columns)
	{
	    SetAPen (rp, TEXT_BPEN(ep));
	    SetWrMsk (rp, TEXT_MASK(ep));       /* don't write over block */

	    /* clear area */
	    RectFill (rp, COL(column_start), ROW(line),
		    COL(column_end)-1, ROW(line+1)-1);

	    /* refresh block */
	    redraw_block (TRUE, ep->line, ep->column, ep->line,
			    ep->column + len - 1);

	    setpen (ep->line, ep->column);
	}

	if (ep->column + len >= Clen)
	    Clen = ep->column + len;

	Current[Clen] = 0;
    } else
    {
	if (Clen + len >= MAXLINELEN-1)
	    goto fail;

	movmem (Current + ep->column, Current + ep->column + len,
		Clen+1-ep->column);
	movmem (str, Current + ep->column, len);
	Clen += len;

	if (len < Columns - (ep->column - ep->topcolumn))
	{
	    i = ep->line - ep->topline;

	    setpen (ep->line, ep->column);

	    ScrollRaster (rp, -len * Xsize, 0 ,
		COL(ep->column - ep->topcolumn),
		ROW(i),
		Xpixs,
		ROW(i + 1) - 1);

	    if (Ep->win->WLayer->Flags & LAYERREFRESH)
		OptimizedRefresh (Ep);
	}

	/* update block */
	if (ActualBlock.type == BT_NORMAL &&
		(is_inblock (ep->line, ep->column) & BP_INSIDE))
	    set_block_end (-2, ActualBlock.end_column + len);
    }

    i = ep->column;
    redraw_lineseg (Current, ROWT(ep->line - ep->topline), i, i+len);

    ep->column += len;

    if (ep->column - ep->topcolumn >= Columns)
    {
	text_sync ();
	text_adjust (FALSE);
    }

fail:
    if (!GETF_COMLINEMODE(ep))
    {
	if (GETF_WORDWRAP(ep))
	    do_reformat(0);
	else if (GETF_AUTOSPLIT(ep))
	{
	    WORD  thislen;	    /* Length of this line */
//	      WORD  nextlen;	      /* Length of next line */
	    WORD  wordlen;	    /* Length of word */
	    char * ptr;

	    /* Check if we have to break */
	    thislen = strlen ((char *)Current);

	    if (thislen > ep->config.margin && ep->column == thislen)
	    {
		/* Well, lets start.
		   This algorithm is very simple for now. It does work only,
		   when the cursor is the end of the line and copies the last
		   word in the next line.
		*/
		wordlen = thislen - 1;
		ptr = (char *)Current + wordlen;

		while (*ptr != ' ' && wordlen)
		{
		    ptr --;
		    wordlen --;
		}

		/* If we found a space, we are 1 char wrong */
		if (wordlen)
		{
		    /* ptr ++ dead assign */
		    wordlen ++;
		}

		/* Calc. length of last word */
		wordlen = thislen - wordlen;

		/* Do nothing if this word won't fit in the next line */
		if (wordlen < ep->config.margin)
		{
		    /* Set cursor at the beginning of the word,
		       split line there, go down and go to last
		       char in this line. */
		    ep->column -= wordlen;
		    do_split ();
		    do_down ();
		    do_lastcolumn ();
		} /* if does word fit */
	    } /* if "Do we have to split ?" */
	} /* if Autosplit */
    } /* if !Comlinemode */

    SetWrMsk (rp, ALL_MASK);
} /* text_write */


/*****************************************************************************

    NAME
	text_redrawblock

    PARAMETER
	BOOL on;	Turn block on (TRUE) or off (FALSE)

    RETURN
	void

    DESCRIPTION
	Redraw the block.

******************************************************************************/

Prototype void text_redrawblock (BOOL on);

void text_redrawblock (BOOL on)
{
    ED * saveed = NULL;

    /* if there is a block that belongs to an editor */

    if (ActualBlock.ep)
    {
	/* change the actual editor, if the editor the block is in and the
	   actual one are not the same */

	if (ActualBlock.ep != Ep)
	{
	    saveed = Ep;
	    switch_ed (ActualBlock.ep);
	}

	/* redraw the block */

	displayblock (on);

	/* if we changed the editor, undo it */

	if (saveed)
	    switch_ed (saveed);
    } else if (!on)
    {
	/* if we want to turn the block off, do it here */

	unblock ();
    }
} /* text_redrawblock */


/*****************************************************************************

    NAME
	redraw_text

    PARAMETER
	Line start_line;    Line to start with
	Line end_line;	    Line to end in (not included)

    RETURN
	void

    DESCRIPTION
	Redraws some lines of text. Only the start-line is included in the
	update.

******************************************************************************/

Prototype void redraw_text (Line start_line, Line end_line);

void redraw_text (Line start_line, Line end_line)
{
    if (start_line < Ep->topline)
	start_line = Ep->topline;

    if (end_line > Ep->topline + Lines)
	end_line = Ep->topline + Lines;
    if (end_line > Ep->lines)
	end_line = Ep->lines;

    for ( ; start_line < end_line; start_line ++)
	redraw_textline (Ep, start_line);
} /* redraw_text */


/*****************************************************************************

    NAME
	redraw_textline

    PARAMETER
	Line line;

    RETURN
	void

    DESCRIPTION
	Redraws one complete lines of text.

    NOTES
	- line must be a valid line from the text.
	- THIS IS A MACRO !

******************************************************************************/


/*****************************************************************************

    NAME
	redraw_textlineseg

    PARAMETER
	Line   line;		The line
	Column start_column;	The column to start in
	Column end_column;	The column to end with (not included)

    RETURN
	void

    DESCRIPTION
	Redraws part of a lines in the text. We redraw all text between
	start_column and end_column including only the former.

    NOTE
	If used to update text that traverses a block-border, the colors
	of the drawn text will be wrong.

******************************************************************************/

Prototype void redraw_textlineseg (Line line, Column start_column, Column end_column);

void redraw_textlineseg (Line line, Column start_column, Column end_column)
{
    setpen (line, start_column);

    if (line == Ep->line)
	redraw_lineseg (Current, ROWT(line - Ep->topline),
		start_column, end_column);
    else
	redraw_lineseg (GETTEXT(Ep,line), ROWT(line - Ep->topline),
		start_column, end_column);
} /* redraw_textlineseg */


/*****************************************************************************

    NAME
	redraw_lineseg

    PARAMETER
	UBYTE * text;		The contents of the line
	UWORD  y;	       y-position to draw at
	Column	start_column;	The column to start in
	Column	end_column;	The column to end with (not included)

    RETURN
	void

    DESCRIPTION
	Redraws part of a lines in the text. We redraw all text between
	start_column and end_column including only the former.

******************************************************************************/

Prototype void redraw_lineseg (UBYTE * text, UWORD y, Column start_column, Column end_column);

void redraw_lineseg (UBYTE * text, UWORD y, Column start_column, Column end_column)
{
    RP *   rp	     = Ep->win->RPort;
    int    len;

    if (Nsu) return;

    /* start-column visible ? */

    if (start_column < Ep->topcolumn)
	start_column = Ep->topcolumn;

    /* end-column visible ? */

    if (end_column > Ep->topcolumn + Columns)
	end_column = Ep->topcolumn + Columns;

    /* outside screen ? */

    if (start_column > end_column)
	return;

    /* get length of line beyond the start-column */

    len = strlen (text);

    if (len > end_column)
	len = end_column;

    len -= start_column;

    /* if there is still text ... */

    if (len > 0)
    {
	/* get contents of line at offset */
	text += start_column;

	while (*text == ' ' && len)
	{
	    text ++;
	    len --;
	    start_column ++;
	}

	/* draw any text that is still to draw */

	if (len)
	{
	    /* set gfxcursor */

	    Move (rp, COLT(start_column - Ep->topcolumn), y);

	    /* draw text */

	    Text (rp, text, len);
	}
    }
} /* redraw_lineseg */


/******************************************************************************
*****  ENDE text.c
******************************************************************************/
