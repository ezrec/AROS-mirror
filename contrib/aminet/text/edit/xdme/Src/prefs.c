/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Everything to set, edit, get, load and save settings !

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include <proto/dos.h>
#include <proto/graphics.h>

#define MYDEBUG 0
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
Prototype int tas_flag (char * what, int old, char * prefix);
static void SetXDMEPen (UBYTE other, UBYTE * set);
static void CheckExt (char * str, char * ext);


int tas_flag (char * what, int old, char * prefix)
{
    int ret = test_arg (what, old);

    title ("%s is now %s", prefix, ret ? "ON" : "OFF");

    return (ret);
} /* tas_flag */


static void SetXDMEPen (UBYTE other, UBYTE * set)
{
    int pen;

    pen = atoi((char *)av[1]);

    if (pen != other)
    {
	*set = pen;
	text_redisplay ();
    } else
	error ("%s:\n"
	       "Cannot choose same color\n"
	       "for both back- and fore-\n"
	       "ground", av[0]);
} /* SetXDMEPen */


static void CheckExt (char * str, char * ext)
{
    int len, extlen;

    len = strlen (str);
    extlen = strlen (ext);

    if (len < extlen || strcmp (str+len-extlen, ext))
	strcat (str, ext);

} /* CheckExt */


/*DEFHELP #cmd prefs MARGIN n - set WordWrap and paragraph formatting margin (related to WORDWRAP and REFORMAT) */

DEFUSERCMD("margin", 1, CF_VWM|CF_ICO|CF_COK, void, do_margin, (void),)
{
    Ep->config.margin = atoi((char *)av[1]);
} /* do_margin */


/*DEFHELP #cmd prefs WORDWRAP what - set word wrap mode (related to MARGIN) */

DEFUSERCMD("wordwrap", 1, CF_VWM|CF_COK|CF_ICO, void, do_wordwrap, (void),)
{
    SETF_WORDWRAP(Ep, tas_flag (av[1], GETF_WORDWRAP(Ep), "WordWrap"));
} /* do_wordwrap() */


/*DEFHELP #cmd searchreplace,prefs GLOBAL what - turn global search on/off. If XDME cannot find a string in one window, it will continue with the next one. */

DEFUSERCMD("global", 1, CF_VWM|CF_COK|CF_ICO, void, do_global, (void),)
{
    SETF_GLOBALSEARCH(Ep,tas_flag (av[1], GETF_GLOBALSEARCH(Ep), "Global search"));
} /* global_search */


/*DEFHELP #cmd prefs SETPARCOL col - Set the LEFT margin for word wrap mode paragraphing & reformat. MUST be less than MARGIN. */

DEFUSERCMD("setparcol", 1, CF_VWM|CF_COK|CF_ICO, void, do_setparcol, (void),)
{
    Ep->config.wwcol = atoi ((char *)av[1]);
} /* do_setparcol() */


/*DEFHELP #cmd prefs TABSTOP n - Set tab stops every @{B}n@{UB}. does not effect text load. */

DEFUSERCMD("tabstop", 1, CF_VWM|CF_COK|CF_ICO, void, do_tabstop, (void),)
{
    Ep->config.tabstop = atoi ((char *)av[1]);
} /* do_tabstop() */


/*DEFHELP #cmd prefs INSERTMODE what - set INSERTMODE. */

DEFUSERCMD("insertmode", 1, CF_VWM|CF_COK|CF_ICO, void, do_insertmode, (void),)
{
    SETF_INSERTMODE(Ep, test_arg (av[1], GETF_INSERTMODE(Ep)));

    window_title ();
} /* do_insertmode() */


/*DEFHELP #cmd prefs,io CHFILENAME name - change the name of the working file */

DEFUSERCMD("chfilename", 1, CF_VWM|CF_ICO, void, do_chfilename, (void),)
{
    BPTR    new_lock;
    UBYTE * ptr;

    text_sync ();

    getpathto (Ep->dirlock, av[1], tmp_buffer);

    ptr = FilePart (tmp_buffer);
    if (ptr != tmp_buffer)
    {
	char c = *ptr;
	*ptr = 0;

	if ( (new_lock = Lock (tmp_buffer, SHARED_LOCK)) )
	{
	    UnLock (Ep->dirlock);

	    Ep->dirlock = new_lock;
	} else
	{
	    error ("%s:\nCannot get lock for\n`%s'.", av[0], tmp_buffer);

	    return;
	}

	*ptr = c;
    }

    strncpy ((char *)Ep->name, ptr, 63);
    Ep->name[63] = 0;

    SETF_MFORCETITLE(Ep,1);
} /* do_chfilename */


/*DEFHELP #cmd prefs SAVETABS what - Optimize file saves by crunching spaces to tabs. The default is OFF. */

DEFUSERCMD("savetabs", 1, CF_VWM|CF_COK|CF_ICO, void, do_savetabs, (void),)
{
    SETF_SAVETABS(Ep,test_arg (av[1], GETF_SAVETABS(Ep)));
} /* do_savetabs() */


/*DEFHELP #cmd prefs SETFONT font sz - Set the window's font. @{B}setfont topaz 11@{UB} */

DEFUSERCMD("setfont", 2, CF_VWM, void, do_setfont, (void),)
{
    FONT * font;
    ED	 * ep	= Ep;
    ULONG  size;

    size = atoi ((char *)av[2]);

    if (size > MAX_FONT_SIZE)
    {
	error ("setfont:\nSize %ld too large !", size);
	return;
    }

    strcpy (tmp_buffer, av[1]);
    CheckExt (tmp_buffer, ".font");

    font = (FONT *)GetFont (tmp_buffer, size);

    if (font)
    {
	if (ep->font)
	    CloseFont (ep->font);

	ep->font = font;

	SetFont (ep->win->RPort, font);
	SetRast (ep->win->RPort, 0);
	RefreshWindowFrame (ep->win);
	set_window_params ();
	text_adjust (TRUE);
    }
    else
    {
	error ("%s:\nUnable to find font\n%s/%ld", av[0], tmp_buffer, size);
    }
} /* do_setfont */


/*DEFHELP #cmd prefs IGNORECASE what -set case ignore for seaches. */

DEFUSERCMD("ignorecase", 1, CF_VWM|CF_COK|CF_ICO, void, do_ignorecase, (void),)
{
    SETF_IGNORECASE(Ep, tas_flag (av[1], GETF_IGNORECASE(Ep), "Ignore case"));
} /* do_ignorecase */


/*DEFHELP #cmd prefs,io SAVECONFIG -save current editor configuration to @{I}s:XDME.prefs@{UI} */

DEFUSERCMD("saveconfig", 0, CF_VWM|CF_COK|CF_ICO, void, do_saveconfig, (void),)
{
    ED	 * ep = Ep;
    FILE * fi;
    WIN  * win = ep->win;

   if (GETF_ICONMODE(ep))
   {
	ep->config.iwinx = win->LeftEdge;
	ep->config.iwiny = win->TopEdge;
   } else
   {
	ep->config.winx      = win->LeftEdge;
	ep->config.winy      = win->TopEdge;
	ep->config.winwidth  = win->Width;
	ep->config.winheight = win->Height;
   }

    if ( (fi = fopen (XDME_CONFIG, "w")) )
    {
	fwrite (CONFIG_VERSION, sizeof(CONFIG_VERSION)-1, 1, fi);
	fwrite (&ep->beginconfig, CONFIG_SIZE, 1, fi);
	fclose (fi);
    }
} /* do_saveconfig */


Prototype void loadconfig (ED * ep);

void loadconfig (ED * ep)
{
    static int showed_error = 0;
    FILE * fi;

    /* Always init the fields */
    /*movmem (&default_config, &ep->beginconfig, CONFIG_SIZE);*/
    memmove (&ep->beginconfig, &default_config, CONFIG_SIZE);

    if (fi = fopen (XDME_CONFIG, "r"))
    {
	fread (tmp_buffer, sizeof(CONFIG_VERSION)-1, 1, fi);

	if (strncmp (CONFIG_VERSION, tmp_buffer, sizeof(CONFIG_VERSION)-1) )
	{
	    if (!showed_error)
	    {
		error ("loadconfig:\n"
			"Wrong version for\n"
			"configuration file !\n"
			"Using defaults");
		showed_error = 1;
	    }
	}
	else
	{
	    fread (&ep->beginconfig, 1, CONFIG_SIZE, fi);
	}
	fclose (fi);
    }
} /* loadconfig */


/*DEFHELP #cmd prefs FGPEN pen - Set pen for text */

DEFUSERCMD("fgpen", 1, CF_VWM, void, do_fgpen, (void),)
{
    SetXDMEPen (TEXT_BPEN(Ep), &TEXT_FPEN(Ep));
} /* do_fgpen */


/*DEFHELP #cmd prefs TFPEN pen - set pen for title bar text */

DEFUSERCMD("tfpen", 1, CF_VWM, void, do_tfpen, (void),)
{
    SetXDMEPen (TITLE_BPEN(Ep), &TITLE_FPEN(Ep));
} /* do_tfpen */


/*DEFHELP #cmd prefs TBPEN pen - set pen for title bar background */

DEFUSERCMD("tbpen", 1, CF_VWM, void, do_tbpen, (void),)
{
    SetXDMEPen (TITLE_FPEN(Ep), &TITLE_BPEN(Ep));
} /* do_tfpen */


/*DEFHELP #cmd prefs BGPEN pen - set background pen for text */

DEFUSERCMD("bgpen", 1, CF_VWM, void, do_bgpen, (void),)
{
    SetXDMEPen (TEXT_FPEN(Ep), &TEXT_BPEN(Ep));
} /* do_bgpen */


/*DEFHELP #cmd prefs HGPEN pen - set highlight (block) pen */

DEFUSERCMD("hgpen", 1, CF_VWM, void, do_hgpen, (void),)
{
    SetXDMEPen (BLOCK_BPEN(Ep), &BLOCK_FPEN(Ep));
} /* do_hgpen */


/*DEFHELP #cmd prefs BBPEN pen - selects @{B}pen@{UB} as the block-background-pen */

DEFUSERCMD("bbpen", 1, CF_VWM, void, do_bbpen, (void),)
{
    SetXDMEPen (BLOCK_FPEN(Ep), &BLOCK_BPEN(Ep));
} /* do_hgpen */


/*DEFHELP #cmd prefs MODIFIED what - set modified flag manually (what={on,off,toggle}) */

DEFUSERCMD("modified", 1, CF_ICO, void, do_modified, (void),)
{
    SETF_MODIFIED(Ep, test_arg (av[1], GETF_MODIFIED(Ep)));
} /* do_modified */


/*DEFHELP #cmd prefs AUTOINDENT what - (De)Activate autoindent with RETURN */

DEFUSERCMD("autoindent", 1, CF_VWM|CF_ICO, void, do_autoindent, (void),)
{
    SETF_AUTOINDENT(Ep, test_arg ((char *)av[1], GETF_AUTOINDENT(Ep)));
} /* do_autoindent */


/*DEFHELP #cmd prefs,win SIZEWINDOW geo - change size and position of the current window to @{B}geo@{UB} */

DEFUSERCMD("sizewindow", 1, CF_VWM|CF_COK, void, do_sizewindow, (void),)
{
    WIN * win = Ep->win;
    struct NewWindow nw;
    int mdx, mdy;

    GeometryToNW (av[1], &nw);

    if (nw.LeftEdge + nw.Width <= win->WScreen->Width &&
	    nw.TopEdge + nw.Height <= win->WScreen->Height &&
	    nw.Width >= win->MinWidth &&
	    nw.Height >= win->MinHeight)
    {
	mdx = nw.LeftEdge - win->LeftEdge;
	mdy = nw.TopEdge - win->TopEdge;

	if (mdx > 0)
	    mdx = 0;

	if (mdy > 0)
	    mdy = 0;

	MoveWindow (win, mdx, mdy);
	SizeWindow (win, nw.Width - win->Width, nw.Height - win->Height);
	MoveWindow (win, nw.LeftEdge - win->LeftEdge, nw.TopEdge - win->TopEdge);
    }
} /* do_sizewindow */


/*DEFHELP #cmd prefs AUTOSPLIT what - (De)Activate autosplit. This is an alternative to WORDWRAP. AUTOSPLIT only breaks the line if it gets too long and doesn't touch the rest of the text. */

DEFUSERCMD("autosplit", 1, CF_VWM|CF_ICO, void, do_autosplit, (void),)
{
    SETF_AUTOSPLIT(Ep, test_arg (av[1], GETF_AUTOSPLIT(Ep)));
} /* do_autosplit */


/*DEFHELP #cmd prefs TASKPRI n - Set the priority of XDME to @{B}n@{UB} (-5..5) */

DEFUSERCMD("taskpri", 1, CF_VWM|CF_COK|CF_ICO, void, do_taskpri, (void),)
{
    LONG pri;

    pri = atoi (av[1]);

    if (pri > 5 || pri < -5)
	error ("%s:\n"
		"Wrong TaskPri %ld. Taskpri must\n"
		"be between -5 and 5 !", av[0], pri);
    else
	SetTaskPri (FindTask (NULL), pri);

} /* do_taskpri */


/*DEFHELP #cmd prefs DEBUG what - @{U}For programmers only@{UB} Allows to set a flag for testing code */

DEFUSERCMD("debug", 1, CF_VWM|CF_ICO, void, do_debug, (void),)
{
    SETF_DEBUG(Ep, tas_flag (av[1], GETF_DEBUG(Ep), "DEBUG"));
} /* do_debug */


/*DEFHELP #cmd prefs DOBACKUP what - specifies if XDME creates a .bak file before actually saving the text */

DEFUSERCMD("dobackup", 1, CF_VWM|CF_ICO, void, do_dobackup, (void),)
{
    SETF_DOBACK(Ep, tas_flag (av[1], GETF_DOBACK(Ep), "DoBackup"));
} /* do_debug */


/*DEFHELP #cmd prefs,block AUTOUNBLOCK what - clear old selection when a new block is defined (on) or give an error */

DEFUSERCMD("autounblock", 1, CF_VWM|CF_ICO, void, do_autounblock, (void),)
{
    SETF_AUTOUNBLOCK(Ep, test_arg (av[1], GETF_AUTOUNBLOCK(Ep)));
} /* do_autounblock */


/*DEFHELP #cmd prefs FOLLOWCURSOR what - XDME will make sure the cursor is visible if you switch it on with this command. Usefull on screens that extend over the visual area. */

DEFUSERCMD("followcursor", 1, CF_VWM|CF_COK|CF_ICO, void, do_followcursor, (void),)
{
    SETF_FOLLOWCURSOR(Ep,tas_flag (av[1], GETF_FOLLOWCURSOR(Ep), "Follow cursor"));
} /* do_followcursor */


/*DEFHELP #cmd prefs NICEPAGING what - Should @{B}PAGEUP@{UB} and @{B}PAGEDOWN@{UB} scroll the page immediately (@{B}on@{UB}) or jump to the border first */

DEFUSERCMD("nicepaging", 1, CF_VWM|CF_COK|CF_ICO, void, do_nicepaging, (void),)
{
    SETF_NICEPAGING(Ep, tas_flag (av[1], GETF_NICEPAGING(Ep), "DME-like paging"));
} /* do_nicepaging */


/*DEFHELP #cmd prefs ICONACTIVE what - Should XDME activate the iconified window */

DEFUSERCMD("iconactive", 1, CF_VWM|CF_COK|CF_ICO, void, do_iconactive, (void),)
{
    SETF_ICONACTIVE(Ep, tas_flag (av[1], GETF_ICONACTIVE(Ep), "Activate icon"));
} /* do_iconactive */


/*DEFHELP #cmd prefs BLOCKENDSFLOAT what - Should @{B}BSTART@{UB} always appear above @{B}BEND@{UB} (@{B}on@{UB}) or remain static */

DEFUSERCMD("blockendsfloat", 1, CF_VWM|CF_COK|CF_ICO, void, do_blockendsfloat, (void),)
{
    SETF_BLOCKENDSFLOAT(Ep, tas_flag (av[1], GETF_BLOCKENDSFLOAT(Ep), "Block ends float"));
    set_block_start (-2,-2);
} /* do_blockendsfloat */


/*DEFHELP #cmd prefs SLINE what - Should XDME not allow to go beyond the end of line and preserve the length of lines (default: no) */

DEFUSERCMD("sline", 1, CF_VWM|CF_COK|CF_ICO, void, do_sline, (void),)
{
    SETF_SLINE(Ep, tas_flag (av[1], GETF_SLINE(Ep), "Preserve linelength"));
} /* do_blockendsfloat */


/*****************************************************************************

    NAME
	do_setlinedistance

    PARAMETER
	av[1] : integer offset (>= 0)

    DESCRIPTION
	Sets the LineDistance. The offset must be positive.

******************************************************************************/

/*DEFHELP #cmd prefs SPACING n - Insert a gap of @{B}n@{UB} pixels between lines */

DEFUSERCMD("spacing", 1, CF_VWM, void, do_setlinedistance, (void),)
{
    int offset = strtol (av[1], NULL, 0);

    if (offset < 0)
	error ("%s:\nYou cannot set a negative\n"
		"space between the lines", av[0]);
    else if (offset > Xsize)
	error ("%s:\n"
		"What you want to do ? Add space\n"
		"for handwritten comments ?", av[0]);
    else
    {
	LineDistance = offset;

	SetAPen (Ep->win->RPort, TEXT_BPEN(Ep));
	SetWrMsk (Ep->win->RPort, BLOCK_MASK(Ep));

	/* Clear window */
	RectFill (Ep->win->RPort, Xbase, Ybase, Xpixs, Ypixs);

	set_window_params ();       /* recalc new offsets */
	text_adjust (TRUE);         /* redraw text */
    }
} /* do_setlinedistance */


/******************************************************************************
*****  ENDE prefs.c
******************************************************************************/
