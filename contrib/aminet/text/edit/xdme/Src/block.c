/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	In this module are all functions that work with blocks.

    NOTES
	- A block has these attributes:
	    1- the editor it is in
	    2- the type (BT_NONE, BT_LINE, BT_NORMAL or BT_VERTICAL)
	       if it is BT_NONE, there is no block.
	    3- a start-position (line/column). If the type is not BT_NONE,
	       this field is guranteed to contain a valid position.
	    4- an end-position. If the line is -1, the end-position is not
	       yet defined.

******************************************************************************/


/**************************************
		Includes
**************************************/
#define BLOCK_C  1
#include <defs.h>
#include "clipboard.h"
#define MYDEBUG     0
#include "debug.h"


/**************************************
	    Globale Variable
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/
#define SWAP(a,b)   (void)((a)^=(b),(b)^=(a),(a)^=(b))
#define MIN(a,b)    ((a) <= (b) ? (a) : (b))
#define MAX(a,b)    ((a) >= (b) ? (a) : (b))

#define BF_LINE_SWAPPED     1L
#define BF_COLUMN_SWAPPED   2L


/**************************************
	    Interne Variable
**************************************/
/* ADA 8.8.94 static entfernt, weil win:format_string() block_type braucht */
UWORD block_type = BT_LINE;	/* default-type of block */


/******************************************

  NOTE: ActualBlock is const so that its fields cannot be
  changed from other files except through the interface
  functions provided.  This is so that certain consistancies
  about the block can be maintained.

*******************************************/
Prototype NOT_BLOCK_C_CONST Block     ActualBlock;

Block ActualBlock =
{
    NULL,	    /* ED * ep; 		       */
    BT_NONE,	    /* UWORD type;		       */
    0,		    /* UWORD flags;		       */
    -1, -1,	    /* Line start_line, end_line       */
    -1, -1,	    /* Column start_column, end_column */
    {		    /* col[3][2]		       */
	{-1, -1},
	{-1, -1},
	{-1, -1}
    }
};

Prototype Block * get_block (Block * buf);

Block * get_block (Block * buf)
{
    if (buf)
	memcpy (buf, &ActualBlock, sizeof(Block));

    return buf;
} /* get_block */


Prototype Block * set_block (Block *buf);

Block * set_block (Block * buf)
{
    if (buf)
    {
	if (block_ok ())
	    displayblock (0);

	memcpy (&ActualBlock, buf, sizeof (Block));
	block_normalize ();

	if (block_ok ())
	    displayblock (1);
    }

    return buf;
} /* set_block */


Prototype Block * set_block_nud (Block *buf);

Block * set_block_nud (Block * buf)
{
    if (buf)
    {
	if (block_ok ())
	    displayblock (0);

	memcpy (&ActualBlock, buf, sizeof (Block));
	block_normalize ();
    }

    return buf;
} /* set_block */


Prototype ED * set_block_ep (ED * new_ed);

ED * set_block_ep (ED * new_ep)
{
    ActualBlock.ep = new_ep;

    block_normalize ();

    return new_ep;
} /* set_block_ep */


Prototype UWORD set_block_type (UWORD type);

UWORD set_block_type (UWORD type)
{
    /* check if there is a block and convert the type */
    if (ActualBlock.type != type)
    {
	/* set new type */
	ActualBlock.type = type;
	block_normalize ();
    }

    if (type == BT_NONE)
    {
	ActualBlock.ep = NULL;
	ActualBlock.flags = 0;
    }

    return type;
} /* set_block_type */


Prototype UWORD set_block_flags (UWORD newflags, UWORD flagmask);

UWORD set_block_flags (UWORD newflags, UWORD flagmask)
{
    return ActualBlock.flags = (ActualBlock.flags & ~flagmask) |
			       (newflags          &  flagmask);
} /* set_block_flags */


Prototype void set_block_start (Line line, Column column);

void set_block_start (Line line, Column column)
{
    if (ActualBlock.flags & BF_LINE_SWAPPED)
	SWAP (ActualBlock.start_line, ActualBlock.end_line);

    if (ActualBlock.flags & BF_COLUMN_SWAPPED)
	SWAP (ActualBlock.start_column, ActualBlock.end_column);

    ActualBlock.flags &= ~(BF_LINE_SWAPPED | BF_COLUMN_SWAPPED);

    if (line != -2) /* -2 means leave  alone */
	ActualBlock.start_line	 = line;
    if (column != -2)
	ActualBlock.start_column = column;

    if (ActualBlock.start_line == -1 &&
	ActualBlock.end_line   != -1)
    {
	if (ActualBlock.ep && !GETF_BLOCKENDSFLOAT(ActualBlock.ep))
	    ActualBlock.flags ^= (BF_LINE_SWAPPED | BF_COLUMN_SWAPPED);

	SWAP (ActualBlock.start_line, ActualBlock.end_line);
	SWAP (ActualBlock.start_column, ActualBlock.end_column);
    }

    block_normalize ();
} /* set_block_start */


Prototype void set_block_end (Line line, Column column);

void set_block_end (Line line, Column column)
{
    if (ActualBlock.flags & BF_LINE_SWAPPED)
	SWAP (ActualBlock.start_line, ActualBlock.end_line);

    if (ActualBlock.flags & BF_COLUMN_SWAPPED)
	SWAP (ActualBlock.start_column, ActualBlock.end_column);

    ActualBlock.flags &= ~(BF_LINE_SWAPPED | BF_COLUMN_SWAPPED);

    if (ActualBlock.start_line >= 0)
    {
	if (line != -2)
	    ActualBlock.end_line   = line;
	if (column != -2)
	    ActualBlock.end_column = column;
    }
    else
    {
	if (line != -2)
	{
	    if (ActualBlock.ep && !GETF_BLOCKENDSFLOAT(ActualBlock.ep))
		ActualBlock.flags ^= BF_LINE_SWAPPED;

	    ActualBlock.start_line   = line;
	}

	if (column != -2)
	{
	    if (ActualBlock.ep && !GETF_BLOCKENDSFLOAT(ActualBlock.ep))
		ActualBlock.flags ^= BF_COLUMN_SWAPPED;

	    ActualBlock.start_column = column;
	}
    }

    block_normalize();
} /* set_block_end */


Prototype void unblock (void);

void unblock (void)
{
    ActualBlock.ep   = NULL;
    ActualBlock.type = BT_NONE;

    block_normalize();
} /* unblock */

/*****************************************************************************

    NAME
	block_leftcolumn
	block_rightcolumn

    PARAMETER
	Line line  The line number of the line in question.

    RETURN
	The column number for the left or right edge of the block
	on a given line.

    DESCRIPTION
	 These functions examine the ActualBlock structure and
	determine where the left or right edges of the block
	extend for a given line.  Note that the edge of the block
	may be past any actual text on that line.  Note also that
	the line number is understood to be a line in the
	ActualBlock.ep editor and not the current editor.

	The returned value is only valid after a call to
	normalize_block().

    EXAMPLE

******************************************************************************/
Prototype Column block_leftcolumn (Line);

Column block_leftcolumn (Line line)
{
    if (block_ok ())
    {
	if (line == ActualBlock.start_line)
	    return ActualBlock.col[0][0];

	if (    line >  ActualBlock.start_line &&
		line <	ActualBlock.end_line)
	    return ActualBlock.col[1][0];

	if (line == ActualBlock.end_line)
	    return ActualBlock.col[2][0];
    }

    return -1;
} /* block_leftcolumn */


Prototype Column block_rightcolumn (Line);

Column block_rightcolumn (Line line)
{
    if (block_ok ())
    {
	if (line == ActualBlock.start_line)
	    return ActualBlock.col[0][1];

	if (    line >  ActualBlock.start_line &&
		line <	ActualBlock.end_line)
	    return ActualBlock.col[1][1];

	if (line == ActualBlock.end_line)
	    return ActualBlock.col[2][1];
    }

    return -1;
} /* block_rightcolumn */


/*****************************************************************************

    NAME
	block_normalize

    PARAMETER
	none

    RETURN
	void

    DESCRIPTION
	This function initializes the col matrix of ActualBlock.
	The matrix holds the left and right column positions for
	the three cases of lines we have to consider for a block:
	the first line, the middle line(s), and the last line.

    EXAMPLE

******************************************************************************/
Prototype void block_normalize (void);

void block_normalize (void)
{
    Column left, right;

    if (ActualBlock.end_line >= 0)
    {
	if (ActualBlock.start_line > ActualBlock.end_line)
	{
	    if (ActualBlock.ep && !GETF_BLOCKENDSFLOAT(ActualBlock.ep))
		ActualBlock.flags ^= (BF_LINE_SWAPPED | BF_COLUMN_SWAPPED);

	    SWAP(ActualBlock.start_line,   ActualBlock.end_line);
	    SWAP(ActualBlock.start_column, ActualBlock.end_column);
	}

	if (ActualBlock.start_line  == ActualBlock.end_line &&
	    ActualBlock.start_column > ActualBlock.end_column)
	{
	    if (ActualBlock.ep && !GETF_BLOCKENDSFLOAT(ActualBlock.ep))
		ActualBlock.flags ^= BF_COLUMN_SWAPPED;

	    SWAP(ActualBlock.start_column, ActualBlock.end_column);
	}
    }

    if (ActualBlock.start_line   >= 0 &&
	ActualBlock.end_line	 >= 0 &&
	ActualBlock.start_column >= 0 &&
	ActualBlock.end_column	 >= 0)
    {
	switch (ActualBlock.type)
	{
	case BT_LINE:
	    ActualBlock.col[0][0] =
	    ActualBlock.col[1][0] =
	    ActualBlock.col[2][0] = 0;

	    ActualBlock.col[0][1] =
	    ActualBlock.col[1][1] =
	    ActualBlock.col[2][1] = MAXLINELEN;

	    break;

	case BT_NORMAL:
	    left = ActualBlock.start_column;
	    right = ActualBlock.end_column;

	    ActualBlock.col[0][0] = left;
	    ActualBlock.col[1][0] =
	    ActualBlock.col[2][0] = ActualBlock.start_line ==
				    ActualBlock.end_line ? left : 0;

	    ActualBlock.col[0][1] =
	    ActualBlock.col[1][1] = ActualBlock.start_line ==
				    ActualBlock.end_line ? right : MAXLINELEN;
	    ActualBlock.col[2][1] = right;

	    break;

	case BT_VERTICAL:
	case BT_HORIZONTAL:
	    left  = MIN (ActualBlock.start_column, ActualBlock.end_column);
	    right = MAX (ActualBlock.start_column, ActualBlock.end_column);

	    ActualBlock.col[0][0] =
	    ActualBlock.col[1][0] =
	    ActualBlock.col[2][0] = left;

	    ActualBlock.col[0][1] =
	    ActualBlock.col[1][1] =
	    ActualBlock.col[2][1] = right;

	    break;

	default:
	    ActualBlock.col[0][0] =
	    ActualBlock.col[1][0] =
	    ActualBlock.col[2][0] =
	    ActualBlock.col[0][1] =
	    ActualBlock.col[1][1] =
	    ActualBlock.col[2][1] = -1;

	    break;
	} /* switch (ActualBlock.type) */
    }
    else
    {
	 ActualBlock.col[0][0] =
	 ActualBlock.col[1][0] =
	 ActualBlock.col[2][0] =
	 ActualBlock.col[0][1] =
	 ActualBlock.col[1][1] =
	 ActualBlock.col[2][1] = -1;
    }

} /* block_normalize */


/*****************************************************************************

    NAME
	is_inblock

    PARAMETER
	Line   line;	    The line we want to check
	Column column;	    the column we are in. If column == -1,
			    we just check the line.

    RETURN
	UWORD blockpos;    Flags:

	BP_OUTSIDE	we are not inside any block.
	BP_START	we are in the line where the block starts.
	BP_END		we are in the line where the block ends.
	BP_BESIDE	the line contains some portion of the block.
			Could be left or right of the block body.
	BP_INSIDE	the char is inside the block.  BP_INSIDE
			always implies BP_BESIDE (can't be in a block
			unless there is some block on the line).

    DESCRIPTION
	This function checks whether a position is inside a block.
	If it isn't, we get BP_OUTSIDE (= 0).  If the indicated
	line contains some portion of a block, we get BP_BESIDE.
	If the indicated column is in the block, we get
	BP_BESIDE|BP_INSIDE.

	Note that the block type has no bearing on these results.
	We treat all blocks the same way regardless of type. Also
	note that some portion of a block may not actually have
	characters in it.  A HORIZONTAL block may extend over a
	blank line, for example.

	If line is the start-line of the block, BP_START is set.
	If line is the last line of the block, BP_END is set.

	If we don't care about the column, we may simply specify
	-1L for column. Else, we give the column we want to check
	and get an additional BP_INSIDE if we are really inside
	the block.

    EXAMPLE

	BP_OUTSIDE	there is no block in that line

	BP_BESIDE	there is some part of the block on this line, but
			this is not the start or end line.  Also we are
			either not within the highlighted area (or we
			specified -1 for column).

	BP_START|BP_BESIDE
			the line is the start-line of the block. Also the
			block has more than one line since BP_END is not
			set. The block is not a line-block (BP_INSIDE is
			always true for line-blocks !).

	BP_START|BP_END|BP_BESIDE
			The block has only one line and we are in it. This is
			also no line-block.

	BP_START|BP_INSIDE|BP_BESIDE
			We are inside the block and on it's start-line.

	BP_INSIDE|BP_BESIDE
			this position is inside the block, but not in the
			start- nor in the end-line.

******************************************************************************/

Prototype UWORD is_inblock (Line line, Column col);

UWORD is_inblock (Line line, Column col)
{
    UWORD blockpos = BP_OUTSIDE;

    /* Look if there is a block, the actual editor is the one that
       has it, we are not in a commandline and the line is inside it.
       NOTE: if there is no end-line yet, the last condition can never
       be TRUE ! */

    if (    ActualBlock.type != BT_NONE     &&
	    Ep == ActualBlock.ep	    &&
	    !GETF_COMLINEMODE(Ep)           &&
	    line >= ActualBlock.start_line  &&
	    line <= ActualBlock.end_line     )
	{

	blockpos |= BP_BESIDE;	/* some part of block is on this line */

	/* Check for start- and end-line */

	if (line == ActualBlock.start_line)
	    blockpos |= BP_START;

	if (line == ActualBlock.end_line)
	    blockpos |= BP_END;

	/* now check, if we are inside the block */

	if ( block_leftcolumn( line) <= col &&
	     block_rightcolumn(line) >= col   )
	    blockpos |= BP_INSIDE;

    } /* if validblock */

    /* return the flags */

    return (blockpos);
} /* is_inblock */


/*****************************************************************************

    NAME
	IsBlockVisible

    PARAMETER
	-

    RETURN
	BOOL yes;

    DESCRIPTION
	Returns TRUE if the currently marked block is visible at least
	partially.

******************************************************************************/

Prototype BOOL IsBlockVisible (void);

BOOL IsBlockVisible (void)
{
    return (BOOL) (block_ok () && ActualBlock.ep == Ep &&
	    !(ActualBlock.start_line >= Ep->topline + Lines ||
		ActualBlock.end_line < Ep->topline));
} /* IsBlockVisible */


/*****************************************************************************

    NAME
	displayblock

    PARAMETER
	BOOL on;	Shall we turn the block on ?

    RETURN
	void

    DESCRIPTION
	This function displays the block. If on is TRUE, the block
	is drawn (again), else we turn the block off. In this case, the
	block-selection is lost !

******************************************************************************/

Prototype void displayblock (BOOL on);

void displayblock (BOOL on)
{
    if (on)
    {
	/* draw block and write the text over it */
	redraw_block (TRUE,
		    Ep->topline, Ep->topcolumn,
		    Ep->topline + Lines - 1, Ep->topcolumn + Columns - 1);

    } else if (block_ok ()) /* turn block off AND there was a block */
    {
	ED  * ep    = Ep;
	RP  * rp    = ep->win->RPort;
	UWORD start = ActualBlock.start_line;
	UWORD end   = ActualBlock.end_line;

	/* no block anymore */
	unblock ();

	/* block is not visible */
	if (end < ep->topline)
	    return;

	/* start-line: If block starts above the 1st visible line,
	   we start in line 0 else in the line where the block starts. */

	if (start < ep->topline)
	    start = ep->topline;

	/* is end-line visible ? */
	if (ep->topline + Lines < end)
	    end = ep->topline + Lines -1;

	if (start <= end)
	{
	    /* color and mask */

	    SetAPen (rp, TEXT_BPEN(ep));
	    SetWrMsk (rp, BLOCK_MASK(ep));

	    /* clear area */
	    RectFill (rp, Xbase, ROW(start - Ep->topline), Xpixs,
		    ROW(end-Ep->topline + 1)-1);

	    /* redraw text */
	    redraw_block (TRUE, start, ep->topcolumn,
		end, ep->topcolumn + Columns -1);
	}
    } else /* just say "there is no block" */
	unblock ();

} /* displayblock */


/*****************************************************************************

    NAME
	do_blocktype

    PARAMETER
	av[0] : blocktype
	av[1] : LINE/CHARACTER/NORMAL/HORIZONTAL/VERTICAL

    RETURN
	void

    DESCRIPTION
	This sets the type of block you want to use. Allowed are 3 types
	of blocks:

	    LINE:	The line-block begins in a line and ends in a line.
			The columns are ignored and both lines are included
			in the block.
	    CHARCTER or NORMAL: The character-block begins at a specifiy line
			and columns and ends at another character-position.
	    HORIZONTAL: The horizontal block is a rectangular region of text
			like a cell of a table or a comment to the left or
			right of source code.
	    VERTICAL:	The vertical block is a rectangular region of text
			like a column of a table.

	If you have already marked a block, it is converted to the new type.

******************************************************************************/

/*DEFHELP #cmd prefs,block BLOCKTYPE type - @{B}type@{UB} is LINE (old), CHARACTER (like everywhere else) or VERTICAL (rectangular block). */

DEFUSERCMD("blocktype", 1, CF_VWM, void, do_blocktype, (void),)
{
    switch (av[1][0])
    {
    case 'l':
    case 'L':
	block_type = BT_LINE;
    break;

    case 'c':
    case 'C':
    case 'n':
    case 'N':
	block_type = BT_NORMAL;
    break;

    case 'v':
    case 'V':
	block_type = BT_VERTICAL;
    break;

    case 'h':
    case 'H':
	block_type = BT_HORIZONTAL;
    break;

    default:
	error ("block:\nUnknown block type.");
	return;
    break;
    }

    set_block_type( block_type );

    if (block_ok() )
      /* clean screen and force redraw */
      text_redisplay ();

} /* do_blocktype */


/*****************************************************************************

    NAME
	do_block

    PARAMETER
	av[0] : block unblock bstart bend lineblock

    RETURN
	void

    DESCRIPTION
	This routine allows to specify the limits of a block.

	The two commands BSTART and BEND can be used anywhere in the
	text (ie. you can user BEND above a BSTART). In this case,
	BEND and BSTART swap their meanings, ie. BEND now sets the start
	of the block and BSTART the end. This is for specifying a block
	with the mouse. If you drag the mouse above the BSTART, the BEND
	will begin to set the beginning of the block and if you drag it
	down over the end of the block, BEND will switch back.

******************************************************************************/

/*DEFHELP #cmd block BLOCK - Set start or end (if start is already set) of block */
/*DEFHELP #cmd block BSTART - Set start of block */
/*DEFHELP #cmd block BEND - Set end of block */
/*DEFHELP #cmd block UNBLOCK - clear the block markers for the current window */
/*DEFHELP #cmd block LINEBLOCK - mark the current line */

DEFUSERCMD("block",     0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("bstart",    0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("bend",      0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("unblock",   0, CF_VWM, void, do_block, (void),;)
DEFUSERCMD("lineblock", 0, CF_VWM, void, do_block, (void),)
{
    ED * ep = Ep;

    /* make sure we work on the most recent version of the text */

    text_sync ();

    /* look for the 1st character */

    switch(av[0][0])
    {
	case 'b': /* Block Bstart Bend */
	    switch (av[0][1]) /* b[lse] */
	    {
		case 'l': /* BLock */
		    /* if there is no block yet, we set the start-line */

		    if (ActualBlock.type == BT_NONE)
		    {
bstart:
			/* set the editor, the start-line and the type. To
			   make clear the end-line is not valid yet, we set
			   it to -1 */

			set_block_ep (ep);
			set_block_type (block_type);
			set_block_start (ep->line, ep->column);
			set_block_end (-1, -1);
			set_block_flags (0, (UWORD)~0);

			title ("Block begin");
		    } else
		    {
			/* if there is a block */

			if (block_ok ())
			{
			    /* shall we automagically unblock ? */

			    if (GETF_AUTOUNBLOCK(ep))
			    {
				/* trun block off */

				text_redrawblock (0);

				/* set start */

				goto bstart;
			    } else
			    {
				/* no auto-unblock ? ERROR ! */

				error ("block:\nBlock already marked");

				break;
			    } /* autounblock */
			} /* if there is already a block */

			/* The last BLOCK began in another editor: Restart ! */

			if (ActualBlock.ep != ep)
			    goto bstart;

			set_block_end (ep->line, ep->column);

			/* now, the block is ready ! */
			title ("Block end");

			text_redrawblock (1);       /* Now display */
		    }
		break;

		case 's': /* BStart */
		    /* no block yet or the old block was in another editor ? */
		    if (ActualBlock.type == BT_NONE || ActualBlock.ep != ep)
		    {
			/* is there an old block and the editor that contains
			   the block is not iconified */
			if (ActualBlock.type != BT_NONE &&
				!GETF_ICONMODE(ActualBlock.ep))
			{
			    /* change editor */
			    switch_ed (ActualBlock.ep);

			    /* switch block off */
			    text_redrawblock (0);

			    /* come back */
			    switch_ed (ep);
			}

			goto bstart;
		    }

		    if (ActualBlock.end_line != -1)
		    {
			redraw_block (FALSE,
				    ep->line,
				    ep->column,
				    -1, -1);
		    } else
		    {
			set_block_start (ep->line, ep->column);
			title ("Block begin");
		    }
		break;

		case 'e': /* bend */
		    /* It is not possible to use BEND if there is no
		       start yet or the block is in another editor */

		    if (ActualBlock.type == BT_NONE)
		    {
			error ("bend:\nYou must use BLOCK or\nBSTART first");
			break;
		    } else if (ActualBlock.ep != ep)
		    {
			error ("bend:\nYou cannot set the end for\n"
			       "a block in another editor !");
			break;
		    }

		    /* say what we do */
		    title ("Block end");

		    /* if there was no end yet, just set it and force
			a redisplay */

		    if (ActualBlock.end_line == -1)
		    {
			set_block_end (ep->line, ep->column);
			text_redrawblock (1);   /* Now display */
		    } else
		    {
			redraw_block (FALSE,
				    -1, -1,
				    ep->line, ep->column);
		    }
		break;
	    }
	break;

	case 'u':
	    /* force UNDRAW and say what we did */

	    text_redrawblock (0);

	    title ("Block unmarked");
	break;

	case 'l':
	    /* mark the whole line as a block */

	    set_block_ep (ep);
	    set_block_type (BT_LINE);
	    set_block_start (ep->line, 0);
	    set_block_end (ep->line, MAXLINELEN);
	    set_block_flags (0, (UWORD)~0);
	    title ("Line marked");

	    /* force redraw */

	    text_redrawblock (1);
	break;
    }
} /* do_block */


/*****************************************************************************

    NAME
	block_ok

    PARAMETER
	void

    RETURN
	BOOL

    DESCRIPTION
	Returns TRUE, if there is a valid block.

******************************************************************************/

Prototype BOOL block_ok (void);

BOOL block_ok (void)
{
    /* A block is valid IF it has a type AND a valid end-line */

    return ((BOOL)(ActualBlock.type != BT_NONE && ActualBlock.end_line >= 0));
} /* block_ok */

/*DEFHELP #cmd block,clip COPY - copy currently marked text into clipboard */

DEFUSERCMD("copy", 0, CF_VWM|CF_ICO|CF_COK, void, do_copy, (void),)
{
    struct IOClipReq * ior;
    char * str = block_to_string ();

    if (str)
    {
	D(bug("block_to_string:\n`%s'\n", str));

	if ((ior = CBOpen (0)))
	{
	    CBWriteFTXT (ior, str);

	    CBClose (ior);
	}

	free (str);
    }
    else
	D(bug("block_to_string: (NUL)\n"));
} /* do_copy */


/*****************************************************************************

    NAME
	block_to_string

    PARAMETER
	void

    RETURN
	char * string;

    DESCRIPTION
	Creates a string from the currently marked block. The lines
	are separated with '\n' and the string is terminated with
	'\0'.

    NOTES
	- The string will only contain '\n's, if the end of a line is
	  included in the block.
	- The block has to be freed with free() if you don't need it anymore,
	  but this may change.

    HISTORY
	13. Feb 1993	ada created

******************************************************************************/


Prototype char * block_to_string (void);

char * block_to_string (void)
{
    ED	 * ep;
    Line   line;
    int    length;
    int    linelen;
    char * string,
	 * ptr,
	 * text;

    if (block_ok ())
    {
	ep = ActualBlock.ep;

	/* to prevent us from writing the same code twice (once for
	   figuring out how long the string will be and once for actually
	   filling the string), we simply check if "string" is NULL.
	   If it is, we have to get the length; else we copy the contents. */

	ptr = string = NULL;

	/*
	   LINE: The length of the block is the sum of the lengths of all
		lines + CR + \0.

	   NORMAL: The length of this block is the sum of the lengths of all
		body lines plus the length of the start- and the end-line
		plus \0. The length of the start-line is

		    o 1, if length < start_column
		    o length-start_column+1, if length > start_column

		and the length of the end-line is

		    o end_column+1, if length > end_column
		    o length+1, if length < end_column

		If the block has only one line, the length is

		    o end_column-start_column if length > end_col
		    o length-start_column+1 if length < end_col

	   VERTICAL: The length of the block is the sum of the lengths of
		all lines + CR + \0. The length of a line is

		    o 0 if the line is < start-column
		    o length-start_column if the line is < end-column
		    o start_column-end_column if line is >= end-column

	   When we have the length, we allocate memory for it and copy
	   the block into that memory.
	 */

	length = 1;	/* ending '\0' */

	text_sync ();   /* use latest text */

	do {
	    for (line=ActualBlock.start_line; line <= ActualBlock.end_line;
								       line ++)
	    {
		Column left, right;

		text = GETTEXT (ep,line);
		linelen = strlen (text);

		if (linelen >= MAXLINELEN)
		{
		    D(bug("block_to_string: Aborting..."
			    "len (%ld) in line %ld too large !\n",
			    linelen, line));
		    break;
		}

		if (string && ptr-string > length)
		    break;

		left  = block_leftcolumn( line );
		right = block_rightcolumn( line );

		if ( string )
		    {
		      if ( left < linelen )
			{
			   strncpy( ptr, &text[left], right - left + 1 );
			   ptr += MIN( right - left + 1, linelen - left );
			}
		      *ptr ++ = '\n';
		    }
		  else
		    {
		      if ( left < linelen )
			  length += MIN( right - left + 1, linelen - left + 1 );
			else
			  length += 1;
		    }

	    }

	    if (string && ptr-string > length)
	    {
		D(bug("Aborting ... String exceeds "
		      "(%ld:%ld) )buffer in line %ld !\n",
			ptr-string, length, line));
	    }

	    if (string)
		break;	/* done ! */
	    else
	    {
		string = malloc (length+256);   /* allocate memory */
		ptr = string;		    /* points to EOS */
	    }

	/* after the loop, the string must be non-NULL, else we didn't get the
	   memory ! */
	} while (string != NULL);

	if (string) /* add '\0' */
	{
	    *ptr = 0;

	    if (length != strlen (string)+1)
	    {
		D(bug("Fehler: length: %ld  strlen: %ld\n",
			length, strlen(string)+1));
	    }
	}
    }
    else
	string = NULL;

    return (string);
} /* block_to_string */


/***************************
***  TML EDITS STOP HERE  **
****************************/

/*****************************************************************************

    NAME
	do_bdelete

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION
	Deletes the marked block from the text.

******************************************************************************/

/*DEFHELP #cmd block BDELETE - delete currently marked text */

DEFUSERCMD("bdelete", 0, 0, void, do_bdelete, (void),)
{
    long lines;
    ED * bep	 = ActualBlock.ep;
    ED * saveed  = Ep;

    /* is there a valid block ? */
    if (!block_ok ())
	return;

    /* TODO */
    if (ActualBlock.type != BT_LINE)
    {
	error ("bdelete:\nCannot use for this\ntype of block.");
	return;
    }

    /* don't allow it in VIEWMODE */

    if (GETF_VIEWMODE(bep))
    {
	error ("%s:\nCannot delete from a\nwrite-protected text", av[0]);
	return;
    } /* if */

    /* change editor */

    switch_ed (bep);

    /* length of block */

    lines = ActualBlock.end_line - ActualBlock.start_line + 1;

    /* if line is inside the block, move to start of block. If it is
	beyond the end-line, move up */

    if (bep->line >= ActualBlock.start_line && bep->line <= ActualBlock.end_line)
	bep->line = ActualBlock.start_line;
    else if (bep->line > ActualBlock.end_line)
	bep->line -= lines;

    /* same for the 1st line */

    if (bep->topline >= ActualBlock.start_line && bep->topline <= ActualBlock.end_line)
	bep->topline = ActualBlock.start_line;
    else if (bep->topline > ActualBlock.end_line)
	bep->topline -= lines;

    /* free memory and copy lines from beyond the block back */
    freelist (bep->list + ActualBlock.start_line, lines);
    bmovl (bep->list + ActualBlock.end_line + 1,
	    bep->list + ActualBlock.start_line,
	    bep->lines - ActualBlock.end_line - 1);

    /* adjust number of lines. Text has been modified */
    bep->lines	 -= lines;

    SETF_MODIFIED(bep, 1);

    /* move cursor if it's now outside the text */
    if (bep->line >= bep->lines)
	bep->line = bep->lines - 1;

    /* move topline if it's now outside the text */
    if (bep->topline >= bep->lines)
	bep->topline = bep->lines - 1;

    /* no lines in text ? */
    if (bep->line < 0)
	bep->topline = bep->line = 0;

    /* create at least one line */

    if (bep->lines == 0)
    {
	bep->lines ++;
	SETLINE(bep,0,allocline(1));
    }

    /* get current line */
    text_load ();

    /* no block anymore */
    unblock ();

    /* always redisplay */
    text_adjust (TRUE);

    /* adjust scroller */
    prop_adj ();

    /* old editor */
    switch_ed (saveed);
} /* do_bdelete */


/*****************************************************************************

    NAME
	do_bcopy

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION
	Copies a block to the line above the cursor. It is possible to
	copy the block into itself thus making it bigger.

******************************************************************************/

/*DEFHELP BCOPY - insert currently marked text before cursor */

DEFUSERCMD("bcopy", 0, 0, void, do_bcopy, (void),)
{
    LINE  * list;
    long    lines,
	    i;
    ED	  * ep = Ep;
    UBYTE * str;

    /* update line */

    text_sync ();

    /* No valid block ? */
    if (!block_ok ())
	return;

    /* TODO */
    if (ActualBlock.type != BT_LINE)
    {
	error ("bcopy:\nCannot use for this\ntype of block.");
	return;
    }

    /* get length of block */

    lines = ActualBlock.end_line - ActualBlock.start_line + 1;

    /* make sure we have enough space */

    if (extend (ep, lines))
    {
	/* get memory for lines */

	if ((list = (LINE *)alloclptr(lines)))
	{
	    /* copy lines to buffer */

	    bmovl (ActualBlock.ep->list+ActualBlock.start_line, list, lines);

	    /* make room for the lines */

	    bmovl (ep->list+ep->line, ep->list+ep->line+lines,
		    ep->lines-ep->line);

	    /* now copy each separate line */

	    for (i = 0; i < lines; i ++)
	    {
		/* get memory for one line */

		str = allocline (LENGTH ((char *)list[i]) + 1);

		/* no more memory ? */

		if (!str)
		{
		    /* set error */

		    nomemory ();

		    /* free memory */

		    FreeMem (list, lines * sizeof(LINE));
		    freelist (ep->list + ep->line, i);

		    /* make space for block disappear */

		    bmovl (ep->list+ep->line+lines, ep->list+ep->line,
			    ep->lines-ep->line);

		    return;
		}

		/* copy contents */

		strcpy ((char *)str, (char *)list[i]);

		/* add line to list */

		SETLINE(ep,ep->line+i,str);
	    }

	    /* free buffer */

	    FreeMem (list, lines * sizeof(LINE));

	    /* if we inserted the block before the start of the block,
	       we must adjust it */

	    if (ep == ActualBlock.ep)
	    {
		if (ep->line <= ActualBlock.start_line)
		{
		    ActualBlock.start_line += lines;
		    ActualBlock.end_line   += lines;
		} else if (ep->line <= ActualBlock.end_line)
		    ActualBlock.end_line   += lines;
	    }

	    /* Text has been modified. Also the number of lines has changed. */

	    SETF_MODIFIED(ep, 1);
	    ep->lines += lines;

#ifdef NOTDEF
	    /* update display */
	    if (!text_adjust (FALSE))
	    {
		scroll_display (0, -lines, ep->topcolumn, ep->line,
			ep->topcolumn+Columns, ep->topline+Lines);
	    }
#endif

	    /* Move cursor to the bottom */

	    ep->line += lines;

	    /* get new contents */

	    text_load ();

	    if (!text_adjust (FALSE)) /* display is adjusted, if needed */
	    {
		scroll_display (0, -lines, ep->topcolumn, ep->line-lines,
			ep->topcolumn+Columns, ep->topline+Lines-1);
	    }

	    /* adjust scroller */
	    prop_adj ();
	} else
	    nomemory ();
    } else
	nomemory ();
} /* do_bcopy */


/*****************************************************************************

    NAME
	do_bmove

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION
	Moves the marked block between the current line and the line
	above it. It's not possible to move the block into itself.

******************************************************************************/

/*DEFHELP #cmd block BMOVE - delete currently marked text and insert it before cursor line */

DEFUSERCMD("bmove", 0, 0, void, do_bmove, (void),)
{
    long   lines;
    LINE * list;
    ED	 * ep = Ep;

    /* update text */

    text_sync ();

    /* no block ?? */
    if (!block_ok ())
	return;

    /* TODO */
    if (ActualBlock.type != BT_LINE)
    {
	error ("bmove:\nCannot use for this\ntype of block.");
	return;
    }

    /* not allowed in viewmode */

    if (GETF_VIEWMODE(ActualBlock.ep))
    {
	error ("%s:\nCannot move from a\nwrite-protected text", av[0]);
	return;
    } /* if */

    /* it is not very usefull to move a block into itself */

    if (    ActualBlock.ep == ep                &&
	    ep->line >= ActualBlock.start_line	&&
	    ep->line <= ActualBlock.end_line	)
    {
	error ("bmove:\nCannot move a\nblock into itself");
	return;
    }

    /* length of block */

    lines = ActualBlock.end_line - ActualBlock.start_line + 1;

    /* get memory for a copy of the block */

    if (!(list = (LINE *)alloclptr (lines)))
    {
	nomemory ();
	return;
    }

    /* text has been modified */
    SETF_MODIFIED (ActualBlock.ep, 1);
    SETF_MODIFIED (ep, 1);

    /* copy block to tmp. memory */
    bmovl (ActualBlock.ep->list + ActualBlock.start_line, list, lines);

    /* are we in the editor that has the block or do we have to move the
       block to some other editor ? */
    if (ep == ActualBlock.ep)
    {
	/* behind the block: move the lines behind the block upto the current
	   line up to the line where the block started and copy the former
	   block into the new space */

	if (ep->line > ActualBlock.start_line)
	{
	    bmovl ( ep->list + ActualBlock.end_line + 1,
		    ep->list + ActualBlock.start_line,
		    ep->line - ActualBlock.end_line -1);
	    bmovl (list, ep->list + ep->line - lines, lines);
	} else
	{   /* before the block: copy all lines from the current upto
	       the line where the block began up to the block-end
	       and copy the former block into the new space */

	    bmovl (ep->list + ep->line, ep->list + ep->line + lines,
		    ActualBlock.start_line - ep->line);
	    bmovl (list, ep->list + ep->line, lines);

	    /* move down */
	    ep->line += lines;
	}

	/* load (new) current line */
	text_load ();
    } else
    {	/* move block between editors */

	/* make room in the editor that gets the block */
	if (extend (ep, lines))
	{
	    /* copy the lines after the block the beginning of the block
	       thus deleting him */

	    bmovl ( ActualBlock.ep->list + ActualBlock.end_line + 1,
		    ActualBlock.ep->list + ActualBlock.start_line,
		    ActualBlock.ep->lines - ActualBlock.end_line - 1);

	    /* make room in the list of lines for the block */
	    bmovl ( ep->list + ep->line,
		    ep->list + ep->line + lines,
		    ep->lines - ep->line);

	    /* copy the buffer to its new place */
	    bmovl (list, ep->list + ep->line, lines);

	    /* adjust the number of lines both editors have now */
	    ep->lines		  += lines; /* he gets the block */
	    ActualBlock.ep->lines -= lines; /* he lost it */

	    /* the current line in the editor that had the block was inside
	       the block */
	    if (    ActualBlock.ep->line >= ActualBlock.start_line  &&
		    ActualBlock.ep->line <= ActualBlock.end_line    )
	    {
		/* move cursor to the place where the block started */
		ActualBlock.ep->line = ActualBlock.start_line;

		/* if the block was the last thing in this text, move up
		   one more line */
		if (ActualBlock.ep->line == ActualBlock.ep->lines)
		    ActualBlock.ep->line --;
	    }

	    /* if the line was behind the last line of the block, move up
	       the length of the block in lines. */
	    if (ActualBlock.ep->line > ActualBlock.end_line)
		ActualBlock.ep->line -= lines;

	    /* ... but not too far */
	    if (ActualBlock.ep->line < 0)
		ActualBlock.ep->line = 0;

	    /* if we moved the whole text, make sure the old editor
	       al least contains one single line */
	    if (ActualBlock.ep->lines == 0)
	    {
		LINE ptr = allocline (1);

		SETLINE(ActualBlock.ep,0,ptr);
		ActualBlock.ep->lines ++;
	    }

	    ep->line += lines;	/* Move cursor down */

	    ActualBlock.type = BT_NONE; /* clear block */

	    text_load ();       /* text_switch() calls text_sync() ! */
	    switch_ed (ActualBlock.ep); /* make source to the active editor */

	    if (!GETF_ICONMODE(Ep))
	    {	     /* Not iconified */
		text_adjust (TRUE);

		/* adjust scroller */
		prop_adj ();
	    }

	    switch_ed (ep);             /* old Ed */
	}
    }

    unblock ();                         /* We don't have any block anymore */

    /* free tmp. buffer */
    FreeMem (list, lines * sizeof(LINE));

    /* force redisplay */
    text_adjust (TRUE);
} /* do_bmove */


/*****************************************************************************

    NAME
	do_bsource

    PARAMETER
	void

    RETURN
	void

    DESCRIPTION
	Executes all lines inside a block as if they were typed as commands.
	The block is removed before the execution starts. The text
	is updated before every line. Thus self-modifying code is possible.

    NOTE
	- since the start and end lines are loaded immediately and the
	  block unblock'd before execution starts, you can theoretically have
	  another BSOURCE as part of this BSOURCE (but be carefull!).
	- BSOURCE allows self-modifying code

******************************************************************************/

/*DEFHELP #cmd block,source BSOURCE - execute currently marked text block as if it were a script file */

DEFUSERCMD("bsource", 0, CF_VWM|CF_ICO, void, do_bsource, (void),)
{
    ED * ep;
    char buf[MAXLINELEN];
    int  i, start, end;

    if (!block_ok ())
	return;

    /* TODO */
    if (ActualBlock.type != BT_LINE)
    {
	error ("bsource:\nCannot use for this\ntype of block.");
	return;
    }

    /* find start and end */

    ep	  = ActualBlock.ep;
    start = ActualBlock.start_line;
    end   = ActualBlock.end_line + 1;

    /* unblock */

    text_redrawblock (0);

    /* execute every line */

    for (i = start; i < end; i ++)
    {
	text_sync (); /* make sure we are using the latest text */

	if (*(GETTEXT(ep,i)) != '#')
	{
	    strcpy (buf, GETTEXT(ep,i));

	    if (!do_command (buf))
		break;
	}
    }
} /* do_bsource */


/******************************************************************************
*****  ENDE block.c
******************************************************************************/
