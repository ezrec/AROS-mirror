/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Inherits all the search/replace stuff for DME

******************************************************************************/

/* Includes */
#include "defs.h"
#define MYDEBUG     0
#include <debug.h>
#include <proto/intuition.h>

/* Globale Variable */

/* Interne Defines & Strukturen */

/* Interne Variable */
/*static*/ UBYTE Fstr[MAXLINELEN]; /* PATCH_NULL - commented out for spcvars */
/*static*/ UBYTE Rstr[MAXLINELEN]; /* PATCH_NULL - commented out for spcvars */
static char Srch_sign;
static char Doreplace;

/* Interne Prototypes */
static void search_operation (void);


/*****************************************************************************

    NAME
	do_findstr

    PARAMETER
	av[0]	    "find" or "replace"
	av[1]	    string to find or replace-string

    DESCRIPTION
	Copies the search/replace-string to the internal buffer.

******************************************************************************/

/*DEFHELP #cmd searchreplace FINDSTR string - Set the search string pattern */
/*DEFHELP #cmd searchreplace REPSTR string - SET the replace string pattern */

DEFUSERCMD("repstr",  1, CF_VWM, void, do_findstr, (void),;)
DEFUSERCMD("findstr", 1, CF_VWM, void, do_findstr, (void),)
{
    if (av[0][0] == 'f')    /* Check command name */
	strcpy (Fstr, av[1]);    /* "find" */
    else
	strcpy (Rstr, av[1]);    /* "replace" */
} /* do_findstr */


/*****************************************************************************

    NAME
	do_findr

    PARAMETER
	av[0]	"findr", "nextr" or "prevr"
	av[1]	find-string
	av[2]	replace-string

    DESCRIPTION
	Starts one find/replace-operation with both search- and replace-string
	as arguments.

******************************************************************************/

/*DEFHELP #cmd searchreplace FINDR s1 s2 - Set find and replace patterns and do one find&replace. */
/*DEFHELP #cmd searchreplace NEXTR - find next occurance and replace */
/*DEFHELP #cmd searchreplace PREVR - find previous occurance and replace */

DEFUSERCMD("findr", 2, 0, void, do_findr, (void),;)
DEFUSERCMD("nextr", 0, 0, void, do_findr, (void),;)
DEFUSERCMD("prevr", 0, 0, void, do_findr, (void),)
{
    Doreplace = 1;  /* Really replace */
    Srch_sign = 1;  /* Search to EOF */

    switch (*av[0])
    {
	case 'f':
	    strcpy (Fstr, av[1]);    /* Set strings for "findr" */
	    strcpy (Rstr, av[2]);
	    break;

	case 'p':
	    Srch_sign = -1;	    /* Search reverse */
	    break;
    }

    search_operation (); /* Start search */
} /* do_findr */


/*****************************************************************************

    NAME
	do_find

    PARAMETER
	av[0]	"find", "next" or "prev"
	av[1]	find-string if av[0] == find

    DESCRIPTION
	Looks for a new string or searches forwards/backwars for the old one.

******************************************************************************/

/*DEFHELP #cmd searchreplace FIND string - Set the search pattern to @{B}string@{UB} and do a NEXT */
/*DEFHELP #cmd searchreplace NEXT - find next occurance of search pattern */
/*DEFHELP #cmd searchreplace PREV - find previous occurance of search pattern */

DEFUSERCMD("find", 1, CF_VWM, void, do_find, (void),;)
DEFUSERCMD("next", 0, CF_VWM, void, do_find, (void),;)
DEFUSERCMD("prev", 0, CF_VWM, void, do_find, (void),)
{
    Doreplace = 0;  /* Don't replace */
    Srch_sign = 1;

    switch (av[0][0])
    {
	case 'f':   /* Find: new string */
	    strcpy (Fstr, av[1]);
	break;

	case 'p':   /* Search reverse */
	    Srch_sign = -1;
	break;
    }

    search_operation ();
} /* do_find */


/*****************************************************************************

    NAME
	do_replace

    PARAMETER
	av[0]	"replace"

    DESCRIPTION
	Substitutes the next strlen(search-string) chars with the rep-string.
	This command does no checking ! It always removes strlen(search-
	string) chars and the rep-string is always inserted.

******************************************************************************/

/*DEFHELP #cmd searchreplace REPLACE - replaces the next strlen(findstr) chars with repstr */

DEFUSERCMD("replace", 0, 0, void, do_replace, (void),)
{
    ULONG  rlen;
    ULONG  flen;
    Column col = Ep->column;

    rlen = strlen (Rstr);
    flen = strlen (Fstr);

    text_sync ();

    /* Check whether replace-string does fit into the line */
    if (rlen > flen && (Clen+rlen-flen) > MAXLINELEN-1)
    {
	error ("replace:\nLine Too Long");
    } else
    /* This checks whether the search-pattern does fit into
	the current line at the current position !?!?! */
	   if (Clen >= col+flen)
    {
	/* Move rest of line. Make enough place for replace-string */
	movmem (Current+col+flen, Current+col+rlen, Clen-col-flen+1);

	/* copy replace-string */
	if (rlen)
	    movmem (Rstr, Current+col, rlen);

	/* correct line-length and position after symbol */
	Clen	   += rlen-flen;

	/* only if we search forward else stay on first char */
	if (Srch_sign > 0)
	    Ep->column += rlen -1; /* one position left since we start
				      at Ep->column+1 with searching */

	if (Clen < 0)
	    Clen = 0;

	if (Ep->column < 0)
	    Ep->column = 0;
    }

    /* Check display, since we are not sure, whether the actual
       position is visible on the screen */
    if (!text_adjust (FALSE))
    {
	text_sync ();
	text_redisplaycurrline ();
    }

} /* do_replace */


/******************************************************************************

    NAME
	search_operation

    PARAMETER
	Fstr	    String to look for
	Rstr	    String to replace Fsrt with (if Doreplace != 0)
	Doreplace   Replace Fstr with Rstr ?
	Ep	    Different fields in this structure

    DESCRIPTION
	Searches in the text for Fstr and replaces it with Rstr if Doreplace is
	true. If Ep->ignorecase is true, the string-search is case-insensitive
	else not.

	For speed, I did replace the old function with this new one. The
	routine works like Boyer-Moore search but I didn't find a good
	explanation for it in books. You will find some for searching from the
	beginning of a string but none if you try reverse. Sigh.

******************************************************************************/

//#define DEBUG
static UBYTE skip[256]; 	    /* Distance for Boyer-Moore */

static void search_operation (void)
{
    UBYTE * ptr;
    WORD    j;	    /* Counter in Search-Pattern */
    Column  col;    /* actual column */
    Line    lin;    /* actual line */
    UWORD   linlen; /* length of actual line */
    UWORD   test;   /* result of case(in)sensitive compare */
    ED	  * ep;     /* actual editor */
    int flen = strlen (Fstr);        /* Length of Search/Replace-String */

    if (!flen)
    {			 /* Error if nothing to look for. */
	error ("search:\nNo find pattern");
	return;
    }

    /* Note that we do not check for nothing to replace, since the user may
       want to erase the full pattern */

    text_sync ();                    /* Save Current */

    ep = Ep;

    col = ep->column;		    /* Actual column ... */
    lin = ep->line;		    /* ... and line */

    /* initskip() */
    memset (skip, flen, sizeof (skip));  /* default: skip full length */

    if (Srch_sign > 0)
    {	 /* Search foreward */
	for (j=0; j<flen; j ++)
	{
	    if (GETF_IGNORECASE(ep))
	    {
		skip[tolower(Fstr[j])] =
		    skip[toupper(Fstr[j])] = flen-j-1;
	    } else
		skip[Fstr[j]] = flen-j-1;
	}

	ptr = GETTEXT(ep,lin);          /* Get line contents ... */
	linlen = strlen (ptr);          /* ... and its length */
	col ++;

	for (; ep; )
	{
	    for (;;)
	    {
		/* This is the search-loop. Always remeber Boyer-Moore
		begins comparing at the end and scans forward ! */
		j = flen - 1;
		col += j;

		/* Until reached beginning of search-string or EOL */
		while (col < linlen)
		{
		    /* if both are equal (case(in)sensitive) */
		    if (GETF_IGNORECASE(ep))
			test = (tolower (ptr[col]) == tolower (Fstr[j]));
		    else
			test = (ptr[col] == Fstr[j]);

		    if (test)
		    {
			if (!j) /* Full Match */
			    goto found;

			col --; /* Next position */
			j --;
		    } else
		    {	 /* Determine skip-length */
			if (flen-j >= skip[ptr[col]])
			    j = flen - j;    /* length of match */
			else
			    j = skip[ptr[col]]; /* Adjust to char. */

			if (!j)
			{
			    printf ("search: infinite loop\n");
			    return;
			}

			col += j;

			j = flen - 1;	/* Restart j */
		    } /* Compare */
		} /* while in line */

		lin ++;

		if (lin >= ep->lines)
		    break;

		ptr = GETTEXT(ep,lin);      /* Contents and Length */
		linlen = strlen (ptr);
		col = 0;
	    } /* while in text */

	    if (!GETF_GLOBALSEARCH(ep))
		break;

	    if (ep = (ED *)GetSucc((struct Node *)ep))
	    {
		lin = 0;
		ptr = GETTEXT(ep,0);      /* Contents and Length */
		linlen = strlen (ptr);
	    }
	} /* for all editors */
    } else
    { /* Search backward */
	for (j=flen-1; j>=0; j--)
	{
	    if (GETF_IGNORECASE(ep))
	    {
		skip[tolower(Fstr[j])] = j;
		skip[toupper(Fstr[j])] = j;
	    } else
	    {
		skip[Fstr[j]] = j;
	    }
	}

	ptr = GETTEXT(ep,lin);          /* Get line contents ... */
	linlen = strlen (ptr);          /* ... and its length */

	if (col >= linlen)  /* Beyound End-Of-Line ? */
	    col = linlen-flen;
	else
	    col --;	/* don't find it right here if we are already on it */

D(bug("new search\n"));

	for (; ep; )
	{
	    for (;;)
	    {
		/* Same as above but reverse */
		j = 0;

		while (col >= 0)
		{ /* Until BOL */
		    /* if both are equal (case(in)sensitive) */
		    if (GETF_IGNORECASE(ep))
			test = (tolower (ptr[col]) == tolower (Fstr[j]));
		    else
			test = (ptr[col] == Fstr[j]);

D(bug("[%s]\n[%s]\ncol %2ld j %2ld skip %2ld\n", ptr+col, Fstr+j, col, j,
		skip[ptr[col]]));

		    if (test)
		    {
			j ++;
			col ++; /* Next position */

			if (j == flen)
			{ /* Full Match */
			    col -= flen;
			    goto found;
			}
		    } else
		    {	 /* Determine skip-length */
			/* j += j; */

			if (j < skip[ptr[col]])
			{
			    j = skip[ptr[col]]; /* Adjust to char. */
			}
			else
			    j += flen;

			if (!j)
			{
			    printf ("search: infinite loop\n");
			    return;
			}

			col -= j;

			j = 0;	 /* Restart j */
		    } /* Compare */

#if DEBUG
{
    char buf[41];

    strncpy (buf, &ptr[col], 40);
    buf[35] = 0;

    printf ("Str '%6s'  Text '%s'  col=%2d j=%+d l=%3d\n", Fstr+j, buf, col, j, lin);
}
#endif
		} /* while in line */

		/* Prev. line */
		if (!lin)   /* Stop on 1. line */
		    break;

		lin --;
D(bug("new line\n"));

		ptr = GETTEXT(ep,lin);      /* Contents and Length */
		linlen = strlen (ptr);

		/* Last char. */
		col = linlen-flen;
	    } /* while in text */

	    if (!GETF_GLOBALSEARCH(ep))
		break;

	    if (ep = (ED *)GetPred((struct Node *)ep))
	    {
		lin = ep->lines - 1;
		ptr = GETTEXT(ep,lin);
		col = linlen = strlen (ptr);
	    }
	} /* for all editors */
    } /* if foreward/backward */

    warn ("Pattern `%s' Not Found", Fstr);
    SETF_ABORTCOMMAND(ep,1);
    return;

found:
    if (ep != Ep)
    {
	switch_ed (ep);

	if (GETF_ICONMODE(ep))
	{
	    uniconify ();
	} else
	{
	    /* Make window active */
	    WindowToFront (ep->win);
	    ActivateWindow (ep->win);

	    set_window_params ();
	    window_title ();
	}
    }

    ep->line = lin;	  /* Set Position */
    ep->column = col;

    text_load ();        /* Copy Line into Current */

    if (Doreplace)
	do_replace ();
    else
	text_adjust (FALSE);
} /* search_operation */


/******************************************************************************
*****  ENDE search.c
******************************************************************************/
