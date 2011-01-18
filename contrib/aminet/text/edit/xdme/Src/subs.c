/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	General subroutines.

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include <clib/macros.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/diskfont.h>
#define MYDEBUG     0
#include "debug.h"


/**************************************
	    Globale Variable
**************************************/

/* Assembler */
Prototype void swapmem (void *, void *, ULONG);


/**************************************
      Interne Defines & Strukturen
**************************************/
typedef struct FileInfoBlock FIB;


/**************************************
	    Interne Variable
**************************************/


/**************************************
	   Interne Prototypes
**************************************/


/*
 *  Create XDME's text icon.
 */

Prototype void makemygadget (struct Gadget *gad);

#if 1 /* alternate icon */
void makemygadget (struct Gadget *gad)
{
    static UWORD IconI1Data[] =
    {
    /* Plane 0 */
      0x0000,0x0800,0x1542,0x4800,0x1282,0x4800,0x7FFF,0xF800,
      0x4000,0x0800,0x5C80,0x0800,0x4000,0x0800,0x5BDB,0x8800,
      0x4000,0x0800,0x57CE,0x8800,0x4000,0x0800,0x5FDF,0x8800,
      0x4000,0x3800,0x5B75,0x8800,0x4000,0x3800,0x0000,0x0800,
      0xFFFF,0xF800,
    /* Plane 1 */
      0xFFFF,0xF000,0x8000,0x0000,0x8000,0x0000,0x8000,0x0000,
      0x8000,0x4000,0x8000,0x4000,0x8000,0x4000,0x8000,0x4000,
      0x8000,0x7000,0x8000,0x4000,0x8000,0x4000,0x8000,0x4000,
      0x8000,0x4000,0x8000,0x4000,0x8000,0x4000,0xFFFF,0xC000,
      0x0000,0x0000,
    };

    static struct Image IconI1 =
    {
      0, 0, 21, 17, 2, IconI1Data, 0x0003, 0x0000, NULL
    };

    static UWORD IconI2Data[] =
    {
    /* Plane 0 */
      0x0000,0x0800,0x755F,0xF800,0x729F,0xF800,0x7FFF,0xF800,
      0x4000,0x0800,0x5C80,0x0800,0x4000,0x0800,0x5BDB,0x8800,
      0x4000,0x3800,0x57CE,0xB800,0x4000,0x3800,0x5FDF,0xB800,
      0x4000,0x3800,0x5B75,0xB800,0x4000,0x3800,0x0000,0x3800,
      0xFFFF,0xF800,
    /* Plane 1 */
      0xFFFF,0xF000,0xE01D,0xB000,0xE01D,0xB000,0x8000,0x0000,
      0x8000,0x5000,0x8000,0x5000,0x8000,0x5000,0x8000,0x5000,
      0x8000,0x7000,0x8030,0x7000,0x8030,0x7000,0x8000,0x7000,
      0x8000,0x4000,0x8000,0x7000,0x8000,0x4000,0xFFFF,0xF000,
      0x0000,0x0000,
    };

    static struct Image IconI2 =
    {
      0, 0, 21, 17, 2, IconI2Data, 0x0003, 0x0000, NULL
    };

    memset (gad, 0, sizeof(struct Gadget));

    gad->Width	      = 21;
    gad->Height       = 18;
    gad->Flags	      = GFLG_GADGIMAGE | GFLG_GADGHIMAGE;
    gad->GadgetType   = GTYP_BOOLGADGET;
    gad->Activation   = GACT_RELVERIFY | GACT_IMMEDIATE;
    gad->GadgetRender = (APTR)&IconI1;
    gad->SelectRender = (APTR)&IconI2;
} /* makemygadget */
#else
void makemygadget (struct Gadget *gad)
{
    static const UWORD ga[] =
    {
     /* Plane 0 */
     0x0000,0x0000,0x0000,0x0400,0x0000,0x0000,0x0000,0x0C00,
     0x0000,0x0000,0x0000,0x0C00,0x0FFF,0xFFFF,0xFFFF,0x8C00,
     0x0C00,0x0000,0x0000,0x0C00,0x0C1F,0xFFFF,0xFFE0,0x0C00,
     0x0C17,0xFFFF,0xFFA0,0x0C00,0x0C1E,0x08BF,0xFFE0,0x0C00,
     0x0C17,0xFFFF,0xFFA0,0x0C00,0x0C1F,0x25FF,0xFFE0,0x0C00,
     0x0C17,0xFFFF,0xFFA0,0x0C00,0x0C1F,0x0BFF,0xFFE0,0x0C00,
     0x0C17,0xFFFF,0xFFA0,0x0C00,0x0C1E,0x1FFF,0xFFE0,0x0C00,
     0x0C16,0xFFFF,0xFFA0,0x0C00,0x0C1E,0x7FFF,0xFFE0,0x0C00,
     0x0C17,0xFFFF,0xFFA0,0x0C00,0x0800,0x0000,0x0000,0x0C00,
     0x0000,0x0000,0x0000,0x0C00,0x007B,0xFF9E,0x7FF8,0x0C00,
     0x0031,0x98CE,0x7318,0x0C00,0x001B,0x186F,0xF318,0x0C00,
     0x001B,0x186F,0xF360,0x0C00,0x000E,0x186D,0xB3E0,0x0C00,
     0x001B,0x186D,0xB360,0x0C00,0x001B,0x186C,0x3318,0x0C00,
     0x0031,0x98CC,0x3318,0x0C00,0x007B,0xFF9E,0x7FF8,0x0C00,
     0x0000,0x0000,0x0000,0x0C00,0x7FFF,0xFFFF,0xFFFF,0xFC00,
     /* Plane 1 */
     0xFFFF,0xFFFF,0xFFFF,0xF800,0xD555,0x5555,0x5555,0x5000,
     0xD555,0x5555,0x5555,0x5000,0xD000,0x0000,0x0000,0x5000,
     0xD000,0x0000,0x0000,0xD000,0xD000,0x0000,0x0000,0xD000,
     0xD003,0xFFFF,0xFF00,0xD000,0xD003,0xFFFF,0xFF00,0xD000,
     0xD003,0xFFFF,0xFF00,0xD000,0xD003,0xFFFF,0xFF00,0xD000,
     0xD003,0xFFFF,0xFF00,0xD000,0xD003,0xFFFF,0xFF00,0xD000,
     0xD003,0xFFFF,0xFF00,0xD000,0xD003,0xFFFF,0xFF00,0xD000,
     0xD003,0xFFFF,0xFF00,0xD000,0xD003,0xFFFF,0xFF00,0xD000,
     0xD003,0xFFFF,0xFF00,0xD000,0xD7FF,0xFFFF,0xFFFF,0xD000,
     0xD555,0x5555,0x5555,0x5000,0xD504,0x0041,0x0005,0x5000,
     0xD544,0x4511,0x0445,0x5000,0xD544,0x4510,0x0445,0x5000,
     0xD544,0x4510,0x0415,0x5000,0xD551,0x4510,0x4415,0x5000,
     0xD544,0x4510,0x4415,0x5000,0xD544,0x4511,0x4445,0x5000,
     0xD544,0x4511,0x4445,0x5000,0xD504,0x0041,0x0005,0x5000,
     0xD555,0x5555,0x5555,0x5000,0x8000,0x0000,0x0000,0x0000
    };
    static const struct Image image =
    {
     0, 0, 54, 30, 2, (UWORD *)ga, 3, 0, NULL
    };

    memset (gad, 0, sizeof(struct Gadget));

    gad->Width	      = 54;
    gad->Height       = 31; /* Blank line between Image and Text ! */
    gad->Flags	      = GADGIMAGE|GADGHCOMP;
    gad->GadgetType   = BOOLGADGET;
    gad->Activation   = RELVERIFY|GADGIMMEDIATE;
    gad->GadgetRender = (APTR)&image;
} /* makemygadget */
#endif


/*
 * return index of first non space.  Returns 0 if no spaces found.
 */

Prototype int firstns (char * str);

int firstns (char * str)
{
    WORD  i;

    for (i = 0; str[i] && str[i] == ' '; i ++);

    if (str[i] == 0)
	i = 0;

    return (i);
} /* firstns */


/*
 *  Return index of last non-space, 0 if no spaces.
 */

Prototype int lastns (char * str);

int lastns (char * str)
{
    WORD  i;

    /* get last char of string (i might be -1 after that !).
       now walk left until a char != ' ' is found or i == 0. */
    for (i = strlen(str) - 1; i > 0 && str[i] == ' '; i --);

    if (i < 0)  /* string empty */
	i = 0;

    return (i);
} /* lastns */


/*
 *  Return length of word under cursor
 */

Prototype int wordlen (char * str);

int wordlen (char * str)
{
    WORD  i;

    for (i = 0; *str && *str != ' '; ++i, ++str);

    return(i);
} /* wordlen */


/*
 *  Backtracks the program lock, 0 on failure, 1 on success.
 *  The filename is appended to the lock, if it exists.
 */

Prototype BOOL getpathto (BPTR lock, char * filename, char * buf);

BOOL getpathto (BPTR lock, char * filename, char * buf)
{
    if (!NameFromLock (lock, buf, PATHSIZE))
	return (FALSE);

    if (filename)
	AddPart (buf, filename, PATHSIZE);

    return (TRUE);
}


/*
 *  Allocation routines and other shortcuts
 */

static LINE empty_line = "";

Prototype LINE allocline (long size);

LINE allocline (long size)
{
     if (size <= 1) return (empty_line);     /* no more tiny empty blocks */

     /* Make sure we always get a padded block */
     size += 7;
     size &= ~7;

     return (AllocMem (size, 0));
} /* allocline */


Prototype void freeline (LINE line);

void freeline (LINE line)
{
    WORD  size;

    if (line && line != empty_line)
    {
	size = strlen (line) + 8;
	size &= ~7;

	FreeMem (line, size);
    }
} /* freeline */


/*
 *  Remove tabs in a buffer
 */

Prototype int detab (char * ibuf, char * obuf, int maxlen);

int detab (char * ibuf, char * obuf, int maxlen)
{
    WORD  i, j;

    maxlen -= 1;

    for (i = j = 0; ibuf[i] && j < maxlen; ++i)
    {
	if (ibuf[i] == 9)
	{
	    do
	    {
		obuf[j++] = ' ';
	    } while ((j & 7) && j < maxlen);
	} else
	{
	    obuf[j++] = ibuf[i];
	}
    }

    if (j && obuf[j-1] == '\n')
	--j;

    while (j && obuf[j-1] == ' ')
	--j;

    obuf[j] = 0;

    return (j);
} /* detab */


Prototype int xefgets (FILE * fi, char * buf, int max);

int xefgets (FILE * fi, char * buf, int max)
{
    char ebuf[MAXLINELEN];

    if (breakcheck())   /* PATCH_NULL  20-07-94 added */
	return -1;	/* PATCH_NULL  20-07-94 added */

    if (fgets (ebuf, max, fi))
	return (detab (ebuf, buf, max));

    return (-1);
} /* xefgets */


Prototype ED * finded (char * str, int doff);

ED * finded (char * str, int doff)
{
    ED *ed;

    for (ed = (ED *)GetHead(&DBase); ed; ed=(ED *)GetSucc((struct Node *)ed))
    {
	if (strlen (ed->name) >= doff && stricmp (str, ed->name+doff) == 0)
	    return (ed);
    }

    return (NULL);
} /* finded */


Prototype void mountrequest (int bool);

void mountrequest (int bool)
{
    static APTR     original_pr_WindowPtr = NULL;
    register PROC * proc;

    proc = (PROC *)FindTask (0);

    if (!bool && proc->pr_WindowPtr != (APTR)-1)
    {
	original_pr_WindowPtr = proc->pr_WindowPtr;
	proc->pr_WindowPtr    = (APTR)-1;
    }

    if (bool && proc->pr_WindowPtr == (APTR)-1)
	proc->pr_WindowPtr = original_pr_WindowPtr;
} /* mountrequest */


/*
 *  GETFONT()
 *
 *  This function properly searches resident and disk fonts for the
 *  font.
 */

struct Library * DiskfontBase;

Prototype FONT * GetFont (char * name, WORD  size);

FONT * GetFont (char * name, WORD  size)
{
    FONT * font1;
    TA	   Ta;
    UWORD pos;

    strcpy (tmp_buffer, name);
    pos = strlen (tmp_buffer);

    if (pos < 6 || (pos > 5 && stricmp (tmp_buffer + pos - 5, ".font")))
	strcpy (tmp_buffer + pos, ".font");

    Ta.ta_Name	= (UBYTE *)tmp_buffer;
    Ta.ta_YSize = size;
    Ta.ta_Style = 0;
    Ta.ta_Flags = 0;

    font1 = OpenFont (&Ta);

    if (!font1 || font1->tf_YSize != Ta.ta_YSize)
    {
	FONT * font2;

	if (DiskfontBase = OpenLibrary ("diskfont.library", 0L))
	{
	    if (font2 = OpenDiskFont (&Ta))
	    {
		if (font1)
		    CloseFont (font1);

		font1 = font2;

		CloseLibrary (DiskfontBase);
	    }
	}
    } /* if !font1 || Wrong_Font_Size */

    return (font1);
} /* GetFont */


Prototype void movetocursor (void);

void movetocursor (void)
{
    Move (Ep->win->RPort, COLT(Ep->column - Ep->topcolumn),
	  ROWT(Ep->line - Ep->topline));
} /* movetocursor */


Prototype int extend (ED * ep, int lines);

int extend (ED * ep, int lines)
{
    long   extra = ep->maxlines - ep->lines;
    LINE * list;

    if (lines > extra)
    {
	lines += ep->lines;

	if (list = (LINE *)alloclptr(lines))
	{
	    bmovl (ep->list, list, ep->lines);
	    FreeMem (ep->list, sizeof(LINE) * ep->maxlines);

	    ep->maxlines = lines;
	    ep->list	 = list;

	    return (1);
	}

	nomemory ();
	return (0);
    }

    return (1);
} /* extend */


Prototype int makeroom (int n);

int makeroom (int n)
{
    ED * ep = Ep;

    if (ep->lines >= ep->maxlines)
	return (extend (ep, n));

    return (1);
} /* makeroom */


Prototype void freelist (LINE * list, int n);

void freelist (LINE * list, int n)
{
    while (n)
    {
	freeline (list[0]);
	list ++;
	n --;
    }
} /* freelist */


/*DEFHELP #cmd misc UNDO - undo current line (must be mapped to a key to work) */

DEFUSERCMD("undo", 0, 0, void, do_undo, (void),)
{
    text_load ();
    text_redisplaycurrline ();
} /* do_undo */


Prototype long lineflags (int line);

long lineflags (int line)
{
    long flags;

    if (line < Ep->topline)
	flags = LINE_ABOVE;
    else if (line < Ep->topline + Lines)
	flags = LINE_VISIBLE;
    else
	flags = LINE_BELOW;

    return (flags);
} /* lineflags */


Prototype void scroll_display (WORD dx, WORD dy, Column lc, Line tl, Column rc, Line bl);

void scroll_display (WORD dx, WORD dy, Column lc, Line tl, Column rc, Line bl)
{
    UWORD width, height;
    Line lines, rest;
    UWORD x1, y1, x2, y2, t;
    Line bottom;
    Column right;
    RP * rp;

    /* don't do anything if we are not allowed */
    if (GETF_ICONMODE(Ep) || Nsu || (!dx && !dy)) return;

    /* clip coords */
    if (tl < Ep->topline)
	tl = Ep->topline;

    if (bl >= Ep->topline+Lines)
	bl = Ep->topline + Lines - 1;

    if (lc < Ep->topcolumn)
	lc = Ep->topcolumn;

    if (rc >= Ep->topcolumn+Columns)
	rc = Ep->topcolumn + Columns - 1;

    /* check if area is visible */
    if (bl < tl || rc < lc) return;

    /* is there something below the text */
    if (bl > Ep->lines)
    {
	bottom = bl - Ep->lines + 1;
    } else
    {
	bottom = 0;
    }

    /* is there something to the right */
    if (rc > MAXLINELEN)
    {
	right = rc - MAXLINELEN + 1;
    } else
    {
	right = 0;
    }

    /* get size of real text */
    width  = rc - lc + 1 - right;
    height = bl - tl + 1 - bottom;

    if (dx)
    {
	lines = ABS(dx);    /* number of columns to draw */

	if (lines >= width)
	     rest = 0;
	else rest = width - lines;

	dx *= Xsize;
    } else
    {
	lines = ABS(dy);    /* number of lines to draw */

	/* if the number of new lines is bigger than the height of the
	   remaining text, we have to erase the empty lines. Otherwise
	   we store the first line of the area that is to update in rest. */

	if (lines >= height)
	     rest = 0;
	else rest = height - lines;

	dy *= Ysize;
    }

    x1 = lc - Ep->topcolumn;
    x2 = rc - Ep->topcolumn;
    y1 = tl - Ep->topline;
    y2 = bl - Ep->topline;

    t = y1;

    x1 = COL(x1);
    x2 = COL(x2+1)-1;
    y1 = ROW(y1);
    y2 = ROW(y2+1)-1;

    rp = Ep->win->RPort;

#ifdef MYDEBUG
    D(bug("scroll_display: rest %ld  height %ld  lines %ld  t %ld (%ld,%ld)-(%ld,%ld)\n",
	  rest, height, lines, t, x1, y1, x2, y2));
#endif

    SetWrMsk (rp, BLOCK_MASK(Ep));
    SetBPen (rp, TEXT_BPEN(Ep));

    if (rest)
    {
	ScrollRaster (rp, dx, dy, x1, y1, x2, y2);

	if (dx)
	{
	    Line   line;
	    Column col;

	    line = tl;
	    col  = lc;

	    if (dx > 0)
		col += rest;

	    redraw_block (TRUE, line, col, line + height, col + lines - 1);
	} else
	{
	    if (dy < 0)
	    {
		text_displayseg (t, lines);
	    } else
	    {
		text_displayseg (t+rest, lines);
	    }
	}

	if (Ep->win->WLayer->Flags & LAYERREFRESH)
	    OptimizedRefresh (Ep);
    }
    else
    {
	SetAPen (rp, TEXT_BPEN(Ep));
	RectFill (rp, x1, y1, x2, y2);

	text_displayseg (t, height);
    }

    SetWrMsk (rp, ALL_MASK);
} /* scroll_display */


Prototype void ScrollAndUpdate (int dx, int dy);

void ScrollAndUpdate (int dx, int dy)
{
    RP * rp = Ep->win->RPort;
    int sx, sy;
    Line line;

    SetWrMsk (rp, IsBlockVisible () ? BLOCK_MASK(Ep) : TEXT_MASK(Ep));

    Ep->topcolumn += dx;
    line = (Ep->topline += dy);

    /* if block WAS visible or it is NOW, use blockmask */
    if (IsBlockVisible ())
	SetWrMsk (rp, BLOCK_MASK(Ep));

    SetBPen (rp, TEXT_BPEN(Ep));

    sx = dx < 0 ? -dx : dx;
    sy = dy < 0 ? -dy : dy;

    if (sx < Columns && sy < Lines)
    {
	ScrollRaster (rp, dx*Xsize, dy*Ysize, Xbase, Ybase, Xpixs, Ypixs);

	if (dx)
	{
	    Column col;

	    col  = Ep->topcolumn;

	    if (dx > 0)
	    {
		col += Columns - sx;
		sx = Columns;
	    }

	    redraw_block (TRUE, line, col, line + Lines-1, col + sx-1);
	} else
	{
	    line = 0;

	    if (dy > 0)
		line += Lines-sy;

	    text_displayseg (line, sy);
	}

	if (Ep->win->WLayer->Flags & LAYERREFRESH)
	    OptimizedRefresh (Ep);
    }
    else
    {
	SetAPen (rp, TEXT_BPEN(Ep));
	RectFill (rp, Xbase, Ybase, Xpixs, Ypixs);

	text_displayseg (line, Lines);
    }

    SetWrMsk (rp, ALL_MASK);
} /* ScrollAndUpdate */


Prototype char * skip_whitespace (char * ptr);

char * skip_whitespace (char * ptr)
{
    while (isspace (*ptr)) ptr ++;

    return (ptr);
} /* skip_whitespace */


/*
 *  IS_NUMBER
 *  tests if a string represents a valid number, i.e.
 *  if there are only numbers in it ( what about spaces around,
 *  and + before a number, do they disturb atol??? )
 */

Prototype char is_number (char * str);

char is_number (char * str)
{
    while (*str==' ')           /* ignore leading spaces */
    {
	str++;
    } /* while */

    if ((*str=='-')||(*str=='+'))  /* dont worry about +/- */
    {
	str++;
    } /* if */

    while (isdigit(*str))     /* if there are only numbers in str: ok */
    {
	str++;
    } /* while */

    while (*str==' ')          /* ignore ending spaces */
    {
	str++;
    } /* while */

    return ((char)(!*str));
} /* is_number */


Prototype char * getnextcomline (FILE* fi, int * lineno);

char * getnextcomline (FILE* fi, int * lineno)
{
    /* skipps comments and emptylines */
    char* buf;
    char* ptr;
    do
    {
	if (lineno)
	{
	    (*lineno)++; /* Welches hat Priortaet?? */
	} /* if */

	buf = tmp_buffer;
	if ((xefgets(fi, tmp_buffer, sizeof(tmp_buffer)))<0)
	/* buf = fgets(tmp_buffer, sizeof(tmp_buffer), fi); */
	if (!buf)
	{
	    error ("%s:\nUnexpected End Of File Encountered", av[0]);
	    return(NULL);
	} /* if */
	buf[sizeof (tmp_buffer)-1] = 0; /* marker for eol */
	while (*buf<33 && *buf)
	{
	    buf++;
	} /* while */
    } while (buf[0] == 0 || buf[0] == '#');
    for (ptr = buf; *ptr;ptr++)
    {
	if (*ptr == 10)
	{
	    *ptr = 0;
	} /* if */
    } /* for */
    return(buf);
} /* getnextcomline */


/*
 *  fname
 *
 *  instead of a macro: sets a ptr to the filename of a full path&filename
 *  (returniert einen Zeiger auf den namensteil eines pfades)
 *
 * SHOULD BE PUT INTO SUBS.C
 */

Prototype char * fname (char * fullpath);

char * fname (char * fullpath)
{
    return ((char *)FilePart (fullpath));
} /* fname */


Prototype BOOL switch_ed (ED * newed);

BOOL switch_ed (ED * newed)
{
    BOOL success;

    if (newed)
    {
	if (Ep)
	    text_sync ();

	Ep = newed;

	text_load ();

	if (!GETF_ICONMODE(Ep))
	{
	    set_window_params ();
	    window_title ();
	}

	text_cursor (1);

	success = TRUE;
    } else
	success = FALSE;

    return (success);
} /* switch_ed */


/*DEFHELP #cmd movement MAKECURSORVISIBLE - Scrolls an oversized screen so the cursor will become visible. */

DEFUSERCMD("makecursorvisible", 0, CF_VWM|CF_COK, void, do_makecursorvisible, (void),)
{
    if ((Ep->win->Flags & WFLG_WINDOWACTIVE) && !GETF_ICONMODE(Ep))
    {
	UWORD x1 = COL(Ep->column - Ep->topcolumn) + Ep->win->LeftEdge;
	UWORD y1 = ROW(Ep->line   - Ep->topline)   + Ep->win->TopEdge;
	UWORD x2, y2;

	x2 = x1 + (Xsize << 2) + Xsize;
	y2 = y1 + (Ysize << 2) + Ysize;

	if (x1 < (Xsize << 2))
	    x1 = 0;
	else
	    x1 -= Xsize << 2;

	if (y1 < (Ysize << 2))
	    y1 = 0;
	else
	    y1 -= Ysize << 2;

	MakeRectVisible (Ep->win, x1, y1, x2, y2);
    }
} /* do_makecursorvisible */


Prototype void MakeRectVisible (WIN * win, UWORD minx, UWORD miny, UWORD maxx, UWORD maxy);

void MakeRectVisible (WIN * win, UWORD minx, UWORD miny, UWORD maxx, UWORD maxy)
{
    struct DimensionInfo diminfo;
    struct Screen      * screen        = win->WScreen;
    ULONG		 Width, Height;
    WORD		 dx, dy;

    GetDisplayInfoData (NULL,
			(UBYTE *)&diminfo,
			sizeof(diminfo),
			DTAG_DIMS,
			GetVPModeID (&screen->ViewPort)
    );

    Width  = diminfo.TxtOScan.MaxX - diminfo.TxtOScan.MinX;
    Height = diminfo.TxtOScan.MaxY - diminfo.TxtOScan.MinY;

    if (Width > screen->Width)
	Width = screen->Width;

    if (Height > screen->Height)
	Height = screen->Height;

    if (minx < win->LeftEdge)
	minx = win->LeftEdge;

    if (miny < win->TopEdge)
	miny = win->TopEdge;

    if (maxx > win->LeftEdge + win->Width)
	maxx = win->LeftEdge + win->Width;

    if (maxy > win->TopEdge + win->Height)
	maxy = win->TopEdge + win->Height;

    dx = (Width - screen->LeftEdge) - maxx + 1;

    if (dx > 0)
    {
	dx = minx + screen->LeftEdge;

	if (dx > 0)
	    dx = 0;
	else
	    dx = -dx;
    }

    dy = (Height - screen->TopEdge) - maxy + 1;

    if (dy > 0)
    {
	dy = miny + screen->TopEdge;

	if (dy >= 0)
	    dy = 0;
	else
	    dy = -dy;
    }

    MoveScreen (screen, dx, dy);
} /* MakeRectVisible */


Prototype int LINELEN (ED * ep, Line nr);

int LINELEN (ED * ep, Line nr)
{
    int len;

    if (nr == ep->line)
    {
	/* line is in Current */

	if ((len = lastns ((char *)Current)) || (*Current && *Current != ' '))
	    len ++;
    }
    else
	len = strlen (GETTEXT(ep,nr));

    return (len);
} /* LINELEN */


/******************************************************************************
*****  ENDE subs.c
******************************************************************************/
