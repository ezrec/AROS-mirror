/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	[v]ref [string]
	Bringup a cross reference editor window.  The file [refpath]dme.refs
	and dme.refs in the current directory are searched for the reference.
	If found, the file specified is searched and the segment specified
	loaded as a new file in a new (read-only) window.

	[v]ctags [string]
	Bringup a cross definition editor window. The file [refpath]tags
	and tags in the current directory are searched for the reference.
	If found, the file specified is shown in a window, and the line
	specified is visited.

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include <unistd.h>
#include <proto/exec.h>
#include <proto/intuition.h>

/**************************************
	    Globale Variable
**************************************/
Prototype LIST PBase;

LIST	PBase;			/*  special XDME paths	 */


/**************************************
      Interne Defines & Strukturen
**************************************/
typedef struct _PEN
{
    NODE    Node;
    char    path[1];
} PEN;


/**************************************
	    Interne Variable
**************************************/


/**************************************
	   Interne Prototypes
**************************************/
static int strnspacecmp (const char * str1, const char * str2, int max);
static int searchref (char * file, char * find, char ** psstr, char ** pfile, long * plines, char **pestr);
static int dirpart (char * str);


Prototype void path_init (void);

void path_init (void)
{
    PEN * pen;

    NewList ((struct List *)&PBase);

    if ((pen = malloc (sizeof(PEN))))
    {
	*pen->path = 0;
	pen->Node.ln_Name = pen->path;

	AddTail ((struct List *)&PBase, (struct Node *)pen);
    }
} /* path_init */


static int strnspacecmp (const char * str1, const char * str2, int max)
{
    while (*str1 && max)
    {
	if (isspace (*str1))
	{
	    if (!isspace (*str2))
		break;

	    while (isspace (*str1) && max) { str1 ++; max --; }

	    if (!max)
		break;

	    while (isspace (*str2)) str2 ++;
	}
	else
	{
	    if (*str1 != *str2)
		break;

	    str1 ++;
	    str2 ++;
	    max --;
	}
    }

    return (max ? *str1 - *str2 : 0);
}/* strnspacecmp */


/*
 *  Special XDME paths for REF and CTAGS
 */

/*DEFHELP #cmd prefs,refs ADDPATH path - Add the specified symbolic directory to XDME's special search path (see REF and CTAGS). */

DEFUSERCMD("addpath", 1, CF_VWM|CF_ICO, void, do_addpath, (void),)
{
    PEN * pen;
    WORD  len = strlen (av[1]);

    for (pen = (PEN *)GetHead(&PBase); pen; pen=(PEN *)GetSucc((struct Node *)pen))
    {
	if (stricmp (av[1], pen->path) == 0)
	    return;
    }

    if ((pen = malloc (sizeof (PEN) + len)))
	strcpy (pen->path, av[1]);

    AddTail ((LIST *) & PBase, (NODE *) pen);

    pen->Node.ln_Name = pen->path;
} /* do_addpath */


/*DEFHELP #cmd prefs,refs REMPATH wild - Remove a directorys from XDME's special path. */

DEFUSERCMD("rempath", 1, CF_VWM|CF_COK|CF_ICO, void, do_rempath, (void),)
{
    PEN * pen;

    for (pen = (PEN *)GetHead(&PBase); pen; pen = (PEN *)GetSucc((struct Node *)pen))
    {
	if (!stricmp (av[1], pen->path))
	{
	    Remove ((NODE *) pen);
	    free (pen);
	}
    }
} /* do_rempath */


/*DEFHELP #cmd refs REFCTAGS - The utimate command for hopping through source. First, we try CTAGS and if we don't find anything, we check REF. */
/*DEFHELP #cmd refs VREFCTAGS name - like REFCTAGS, but looks for @{B}name@{UB} */

DEFUSERCMD("vrefctags", 1, CF_VWM|CF_ICO, void, do_refctags, (void),;)
DEFUSERCMD("refctags",  0, CF_VWM|CF_ICO, void, do_refctags, (void),)
{
    char *av_bak[2];
    BOOL requester;
    BOOL title;

    av_bak[0] = av[0];
    av_bak[1] = av[1];

    requester = GETF_NOREQUEST(Ep);
    title     = GETF_SHOWTITLE(Ep);

    SETF_NOREQUEST(Ep,TRUE);
    SETF_SHOWTITLE(Ep,FALSE);

    do_ctags ();

    if (GETF_ABORTCOMMAND(Ep)) {
	SETF_ABORTCOMMAND(Ep,0);
	do_refs ();
    } /* if */

    SETF_NOREQUEST(Ep,requester);
    SETF_SHOWTITLE(Ep,title);

    if (GETF_ABORTCOMMAND(Ep)) {
	char *str = ((av_bak[1] != NULL) ? (char *)av_bak[1] : current_word ());
	if (!str[0])
	    error ("%s: No searchstring defined!\n", av_bak[0]);
	else
	{
	    char buffer[256];

	    strcpy (buffer, str);

	    error ("%s:\nCannot find anything about\n`%s'.", av_bak[0], buffer);
	}
    } /* if */
} /* do_refctags */


/*
 *  Implement ctags
 */

/*DEFHELP #cmd refs CTAGS - search for the tag under the cursor (see below) */
/*DEFHELP #cmd refs VCTAGS name - search for tag @{B}name@{UB} */

DEFUSERCMD("vctags", 1, CF_VWM, void, do_ctags, (void),;)
DEFUSERCMD("ctags",  0, CF_VWM, void, do_ctags, (void),)
{
    char    str[MAXLINELEN];
    char    path[128];
    char    buf[300];
    char    sbuf[128];
    WORD    xlen;
    WORD    slen;
    /* WORD    dbaselen; unused var */
    BPTR    oldlock = CurrentDir (Ep->dirlock);
    ED	  * ed;
    LONG    lineno = -1;

    if (av[1] != NULL)
    {
	strcpy (str, av[1]);
    }
    else
    {
	strcpy (str, current_word ());
    }

    xlen = strlen (str);
    if (!str[0]) {
	error ("%s: No searchstring defined!\n", av[0]);
	return;
    } /* if */

    if (!GETF_ICONMODE(Ep))
	title ("search tags");

    {
	FILE * fi;
	PEN  * pen;
	WORD   j,
	       len;

	/*
	 *  Note: pen not used first pass and set to list head, so next
	 *  pass it will be the first element.
	 *
	 *  Note2:  The file path depends on several factors.  (1) tags in
	 *	    'current' directory, use path to name of current window.
	 *	    (2) tags in directory in XDME special path, use special
	 *	    path.  (3) tag entry is a full path name, override
	 *	    previous directories.
	 */

	mountrequest (0);

	for (pen=(PEN *)GetHead(&PBase); pen;
				    pen=(PEN *)GetSucc((struct Node *)pen))
	{
	    strcpy (path, pen->path);
	    AddPart (path, "tags", sizeof(path));

	    if ((fi = fopen (path, "r")))
	    {
		mountrequest (1);

		while ((len = xefgets (fi, buf, 128)) >= 0)
		{
		    for (j = 0; buf[j] && buf[j] != ' '; ++j);

		    if (j == 0 || buf[0] == '#')
			continue;

		    if (j == xlen && strncmp (str, buf, j) == 0)
		    {
			while (buf[j] == ' ')
			    ++j;

			/*
			 *  Extract the file name into str.  If the
			 *  filename does not contain an absolute path,
			 *  prepend it with such.
			 */
			{
			    char prep = 1;
			    int i;

			    for (i = 0; buf[j] && buf[j] != ' '; ++i, ++j)
			    {
				str[i] = buf[j];
				if (str[i] == ':')
				    prep = 0;
			    }

			    str[i] = 0;

			    if (prep)
			    {
				char buf[256];

				strcpy (buf, pen->path);
				AddPart (buf, str, sizeof(buf));

				strcpy (str, buf);
			    }
			}

			fclose (fi);                        /* PATCH_NULL [10 Mar 1993] : moved 2 lines up */

			/* BERKELEY FIX: I have found a format in CTags */
			/*	<tag> <file> <linenumber>		*/
			/* which is supported now			*/
			while (isspace(buf[j])) ++j;          /* PATCH_NULL [10 Mar 1993] : added */

			if (is_number (buf+j))              /* PATCH_NULL [10 Mar 1993] : added */
			{				    /* PATCH_NULL [10 Mar 1993] : added */
			    lineno = atol (buf+j)-1;        /* PATCH_NULL [10 Mar 1993] : added */
			}				    /* PATCH_NULL [10 Mar 1993] : added */
			else				    /* PATCH_NULL [10 Mar 1993] : added */
			{				    /* PATCH_NULL [10 Mar 1993] : added */
			    while (buf[j] && buf[j] != '^') /*  SEARCH ARG */
				++j;


			    if (buf[j] != '^')
			    {
				error ("ctags:\nError in tags-file");
				goto done;
			    }

			    ++j;    /* Skip ^ */

			    /* UN*X FIX: Dme works for Aztec ctags, which has the format:   */
			    /*	    <tag>   <file>  /^<pattern> 			    */
			    /* However, ctags under UN*X has the following format:	    */
			    /*	    <tag>   <file>  /^<pattern>[$]/			    */
			    /* We just ignore the '$' and '/' so that both foramts work     */
			    /* Thomas Rolfs 27/3/91					    */

			    {
				int i = 0;

				while (buf[j] && buf[j] != '^' && buf[j] != '$'
					    && buf[j] != '/' && buf[j] != '?')
				{
				    sbuf[i++] = buf[j++];
				}

				sbuf[i] = 0;
			    }

			    /* End of fix.  */

			    slen = strlen (sbuf);
			} /* if */			    /* PATCH_NULL [10 Mar 1993] : added */

			if ((ed = finded (FilePart(str), 0)) == NULL) /* PATCH_NULL 20-07-94 added "FilePart" */
			{
			    strcpy (buf, "newwindow newfile `");
			    strcat (buf, str);
			    strcat (buf, "' viewmode on");
			    do_command (buf);

			    /* remove path */
			    strcpy (str, FilePart (str));

			    ed = finded (str, 0);
			} else
			{
			    WindowToFront (ed->win);
			    ActivateWindow (ed->win);
			}

			if (ed == NULL)
			{
			    error ("ctags:\nunable to load file\n`%s'", str);
			    goto done;
			}

			switch_ed (ed);
			text_cursor (0);

			if (lineno < 0)                                    /* PATCH_NULL [10 Mar 1993] : added */
			{
			    for (lineno = 0; lineno < ed->lines; ++lineno) /* PATCH_NULL [10 Mar 1993] : i -> lineno */
			    {
				if (strnspacecmp (sbuf, ed->list[lineno], slen) == 0)
				    break;
			    }

			    if (lineno == ed->lines)
			    {
				int i;

				printf ("searching:\n");
				for (i=0; sbuf[i] && i<slen; i++)
				{
				    printf ("%02x", sbuf[i]);
				    if ((i & 3) == 3)
					printf (" ");
				}
				printf ("\n");
			    }
			}

			sprintf (buf, "first goto %ld", lineno + 1);
			do_command (buf);
			text_cursor (1);    /* turn cursor on */

			goto done;
		    }
		}

		fclose (fi);

		mountrequest (0);
	    }
	}

	error ("ctags:\ntag `%s'\nnot found", str);

	mountrequest (1);
    }
done:
    CurrentDir (oldlock);
} /* do_ctags */


/*
 *  Implement references
 */

/*DEFHELP #cmd refs REF - reference string under cursor (see below) */
/*DEFHELP #cmd refs VREF name - reference @{B}name@{UB} */

DEFUSERCMD("vref", 1, CF_VWM|CF_ICO, void, do_refs, (void),;)
DEFUSERCMD("ref",  0, CF_VWM|CF_ICO, void, do_refs, (void),)
{
    static char str[MAXLINELEN];
    static char tmpfile[128];
    static char path[128];
    char      * srch;
    char      * file;
    char      * estr;
    long	len;
    int 	bcnt = 10;
    WORD	slen,
		elen;
    FILE      * fi,
	      * fj;
    WORD	tmph,
		tmpw;
    BPTR	oldlock = CurrentDir (Ep->dirlock);

    if (av[1] != NULL)
    {
	strcpy (str, av[1]);
    }
    else
	strcpy (str, current_word ());

    if (!str[0]) {
	error ("%s: No searchstring defined!\n", av[0]);
	return;
    } /* if */


    strcpy (tmpfile, "t:ref_");
    strncat (tmpfile, str, sizeof (tmpfile) - 32);

    title ("search .refs");

    {
	PEN * pen;

	mountrequest (0);

	/* search all paths */
	for (pen = (PEN *)GetHead(&PBase); pen; pen=(PEN *)GetSucc(pen))
	{
	    strcpy (path, pen->path);
	    AddPart (path, "dme.refs", sizeof (path));

	    if (searchref (path, str, &srch, &file, &len, &estr))
	    {
		mountrequest (1);
		goto found;
	    }
	}

	error ("refs:\nReference `%s'\nnot found", str);
	mountrequest (1);
	goto done;
    }

found:
    title ("search file");
    slen = strlen (srch);

    if (estr)
	elen = strlen (estr);

    fi = fopen (file, "r");

    if (fi == NULL)
    {		/*  try using path prefix   */
	strcpy (str, path);
	strcpy (str + strlen (str) - 8, file);
	fi = fopen (str, "r");
    }

    if (fi)
    {
	WORD  lenstr;

	if (srch[0] == '@' && srch[1] == '@')
	{
	    fseek (fi, atoi (srch + 2), 0);
	    if ((lenstr = xefgets (fi, str, MAXLINELEN)) >= 0)
		goto autoseek;
	}

	while ((lenstr = xefgets (fi, str, MAXLINELEN)) >= 0)
	{
	    if (strncmp (str, srch, slen) == 0)
	    {
autoseek:
		title ("load..");
		if ((fj = fopen (tmpfile, "w")))
		{
		    tmph = 0;
		    tmpw = 0;

		    do
		    {
			if (lenstr > tmpw)
			    tmpw = strlen (str);

			tmph ++;

			fputs (str, fj);
			fputc ('\n', fj);

			if (estr && strncmp (str, estr, elen) == 0)
			    break;

			len --;
		    }

		    while ((lenstr = xefgets (fi, str, MAXLINELEN)) >= 0 && len);

		    fclose (fj);

		    if (tmph > 10)
			tmph = 10;
		    if (tmpw > 80)
			tmpw = 80;

		    sprintf (str, "openwindow +0+0+%d+%d newfile %s viewmode on", (tmpw << 3) + 24, (tmph << 3) + 24, tmpfile);
		    do_command (str);
		    text_cursor (1);    /* turn cursor on */
		    unlink (tmpfile);
		} else
		{
		    error ("refs:\nUnable to open\n`%s'\nfor write", tmpfile);
		}

		fclose (fi);
		free (srch);
		free (file);

		if (estr)
		    free (estr);

		goto done;
	    }
	    if (--bcnt == 0)
	    {  /* check break every so so  */
		bcnt = 50;

		if (breakcheck ())
		    break;
	    }
	}
	fclose (fi);

	error ("refs:\nSearch for `%s'\nfailed", str);
    } else
    {
	error ("refs:\nUnable to open\nsub document");
    }

    free (srch);
    free (file);

    if (estr)
	free (estr);
done:
    CurrentDir (oldlock);
} /* do_refs */


/*
 *  Reference file format:
 *
 *  `key' `lines' `file' `searchstring'
 *
 *  where `lines' can be a string instead ... like a read-until, otherwise
 *  the number of lines to read from the reference.
 */

static int searchref (char * file, char * find, char ** psstr, char ** pfile, long * plines, char **pestr)
{
    FILE  * fi;
    char    buf[MAXLINELEN];
    char  * ptr,
	  * base;
    char  * b1,
	  * b2,
	  * b3,
	  * b4;
    char    quoted;

    /* WORD  findlen = strlen(find); TODO */

    if ((fi = fopen (file, "r")))
    {
	while (xefgets (fi, (base = buf), MAXLINELEN) >= 0)
	{
	    if (buf[0] == '#')
		continue;

	    ptr = breakout (&base, &quoted, &b1);

	    /* TODO if (ptr && *ptr && strncmp(ptr, find, findlen) == 0) */
	    if (ptr && *ptr && strcmp (ptr, find) == 0)
	    {
		if ((ptr = breakout (&base, &quoted, &b2)))
		{
		    *pestr = NULL;
		    *plines = atoi (ptr);

		    if (*plines == 0)
		    {
			*pestr = (char *) malloc (strlen (ptr) + 1);
			strcpy (*pestr, ptr);
		    }

		    if ((ptr = breakout (&base, &quoted, &b3)))
		    {
			*pfile = (char *) malloc (strlen (ptr) + 1);
			strcpy (*pfile, ptr);

			if ((ptr = breakout (&base, &quoted, &b4)))
			{
			    *psstr = (char *) malloc (strlen (ptr) + 1);

			    strcpy (*psstr, ptr);
			    fclose (fi);

			    if (b1)
				free (b1);
			    if (b2)
				free (b2);
			    if (b3)
				free (b3);
			    if (b4)
				free (b4);

			    return (1);
			}

			free (*pfile);

			if (b4)
			    free (b4);
		    }

		    if (pestr)
			free (*pestr);
		    if (b3)
			free (b3);
		}

		if (b2)
		    free (b2);
	    }

	    if (b1)
		free (b1);
	}

	fclose (fi);
    }

    return (0);
} /* searchref */


/*
static int dirpart (char * str)
{
    WORD    i;

    for (i = strlen (str) - 1; i >= 0; --i)
    {
	if (str[i] == '/' || str[i] == ':')
	    break;
    }

    return (i + 1);
} / * dirpart */


/******************************************************************************
*****  ENDE refs.c
******************************************************************************/
