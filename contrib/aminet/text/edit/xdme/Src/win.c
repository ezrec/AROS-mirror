/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Everything for windows and GUI

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include <graphics/text.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/classes.h>
#include <intuition/icclass.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <string.h>
#define MYDEBUG 0
#include "debug.h"


/**************************************
	    Globale Variable
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/
#define IDCMPFLAGS   (IDCMP_CLOSEWINDOW |\
		      IDCMP_NEWSIZE |\
		      IDCMP_RAWKEY |\
		      IDCMP_MOUSEBUTTONS |\
		      IDCMP_ACTIVEWINDOW |\
		      IDCMP_INACTIVEWINDOW |\
		      IDCMP_MOUSEMOVE |\
		      IDCMP_MENUPICK |\
		      IDCMP_GADGETUP |\
		      IDCMP_GADGETDOWN |\
		      IDCMP_REFRESHWINDOW)

#define WINDOWFLAGS  (WFLG_ACTIVATE |\
		      WFLG_SIZEGADGET |\
		      WFLG_DRAGBAR |\
		      WFLG_DEPTHGADGET |\
		      WFLG_CLOSEGADGET |\
		      WFLG_SIMPLE_REFRESH)

#define ICONIFYFLAGS (WFLG_DRAGBAR |\
		      WFLG_RMBTRAP |\
		      WFLG_DEPTHGADGET |\
		      WFLG_SIMPLE_REFRESH)



struct NewWindow Nw =
{
   0, 1, 0  , 0  , 0, 0,  /*  width, height filled in by program */
   IDCMPFLAGS, WINDOWFLAGS,
   NULL, NULL, (UBYTE *)"",
   NULL, NULL,
   100, 50, (UWORD)-1, (UWORD)-1,
   WBENCHSCREEN
};

#define NUM_GADS	4

struct PropGadget
{
    struct Gadget   scroller;
    struct Gadget   up;
    struct Gadget   down;
#ifdef NOT_DEF
    struct Gadget   depth;
#endif
    struct Gadget   iconify;
    struct PropInfo pinfo;	    /* PropInfo for scroller */
    struct Image    simage;	    /* image for scroller */
    struct Image  * upimage;
    struct Image  * downimage;
#ifdef NOT_DEF
    struct Image  * depthimage;
#endif
    struct Image  * iconifyimage;
};


const struct PropGadget gadgetdefaults =
{
    {	/* PropGadget */
	NULL,
	0,0, 0,0,
	GFLG_RELRIGHT|GFLG_RELHEIGHT,
	GACT_RIGHTBORDER|GACT_RELVERIFY|GACT_IMMEDIATE|GACT_FOLLOWMOUSE,
	GTYP_PROPGADGET,
	NULL, NULL, NULL, NULL, NULL,
	0, NULL
    },
    {	/* Up-Image */
	NULL,
	0,0, 0,0,
	GFLG_RELRIGHT|GFLG_RELBOTTOM|GFLG_GADGHIMAGE|GFLG_GADGIMAGE,
	GACT_RIGHTBORDER|GACT_RELVERIFY|GACT_IMMEDIATE,
	GTYP_BOOLGADGET,
	NULL, NULL, NULL, NULL, NULL,
	1, NULL
    },
    {	/* Down-Gadget */
	NULL,
	0,0, 0,0,
	GFLG_RELRIGHT|GFLG_RELBOTTOM|GFLG_GADGHIMAGE|GFLG_GADGIMAGE,
	GACT_RIGHTBORDER|GACT_RELVERIFY|GACT_IMMEDIATE,
	GTYP_BOOLGADGET,
	NULL, NULL, NULL, NULL, NULL,
	2, NULL
    },
#ifdef NOT_DEF
    {	/* Depth-Gadget */
	NULL,
	0,0, 0,0,
	GFLG_RELRIGHT|GFLG_GADGHIMAGE|GFLG_GADGIMAGE,
	GACT_TOPBORDER|GACT_RELVERIFY,
	GTYP_BOOLGADGET,
	NULL, NULL, NULL, NULL, NULL,
	3, NULL
    },
#endif
    {	/* Zoom-Gadget */
	NULL,
	0,0, 0,0,
	GFLG_RELRIGHT|GFLG_GADGHIMAGE|GFLG_GADGIMAGE,
	GACT_TOPBORDER|GACT_RELVERIFY,
	GTYP_BOOLGADGET,
	NULL, NULL, NULL, NULL, NULL,
	4, NULL
    },
    {	/* PropInfo */
	AUTOKNOB|FREEVERT|PROPNEWLOOK|PROPBORDERLESS,
	MAXPOT, MAXPOT,
	MAXBODY, MAXBODY,
    },
};


/**************************************
	    Interne Variable
**************************************/
static UWORD	WIN_MINWIDTH,		/* min. sizes for a window */
		WIN_MINHEIGHT;

#define MAX_LOG     10
static char * log_messages[MAX_LOG];
static UWORD numlogs = 0;


/**************************************
	   Interne Prototypes
**************************************/
static void log (char *);
static void StripIntuiMessages (struct MsgPort *,struct Window *);


/*DEFHELP #cmd win ICONIFY - iconify the window */

DEFUSERCMD("iconify", 0, CF_VWM|CF_ICO, void, do_iconify, (void),)
{
    text_sync ();

    if (!GETF_COMLINEMODE(Ep))
	iconify ();
} /* do_iconify */


/*DEFHELP #cmd prefs,win SETDEFICONTITLE string - Sets the pattern for the window-title when iconifed */

static char * default_icontitle = "%f";
static char icontitle_pattern[80] = "%f";

DEFUSERCMD("setdeficontitle", 1, CF_VWM|CF_ICO|CF_COK, void, do_setdeficontitle, (void),)
{
    if (!*av[1])
	strcpy (icontitle_pattern, default_icontitle);
    else
    {
	strncpy (icontitle_pattern, av[1], sizeof (icontitle_pattern));
	icontitle_pattern[sizeof (icontitle_pattern)-1] = 0;
    }
} /* do_setdeftitle */


/* New iconify() routine by fgk. */
Prototype void iconify (void);

void iconify (void)
{
    WIN 	   * newwin;
    ED		   * ep      = Ep;
    WIN 	   * win     = ep->win;
    struct IntuiText itxt;		/* To find width of prop fonts */

    itxt.ITextFont = Ep->win->WScreen->Font;	/* Init */
    itxt.NextText  = NULL;

    if (!GETF_ICONMODE(ep))
    {
	ep->config.winx      = win->LeftEdge;
	ep->config.winy      = win->TopEdge;
	ep->config.winwidth  = win->Width;
	ep->config.winheight = win->Height;

	if(Ep->win->WScreen->Font != NULL)
	    Nw.Height = Ep->win->WScreen->Font->ta_YSize + 3;	/* height */
	else
	    Nw.Height = GfxBase->DefaultFont->tf_YSize + 3;

	format_string (ep->wtitle, icontitle_pattern);
	itxt.IText = ep->wtitle;

	/* pretending spaces are always 8 */
	Nw.Width  = 60 + IntuiTextLength(&itxt);          /* width */

	Nw.LeftEdge = ep->config.iwinx;
	Nw.TopEdge  = ep->config.iwiny;

	if (Nw.LeftEdge + Nw.Width > win->WScreen->Width)   /* keep in bounds */
	    Nw.LeftEdge = win->WScreen->Width - Nw.Width;

	if (Nw.TopEdge + Nw.Height > win->WScreen->Height)
	    Nw.TopEdge = win->WScreen->Height - Nw.Height;

	Nw.Title = ep->wtitle;
	Nw.Flags = ICONIFYFLAGS;

	if (!GETF_MODIFIED(ep))
	{     /* no CLOSE */
	    Nw.Flags |= WFLG_CLOSEGADGET;
	}

	Nw.DetailPen = TEXT_BPEN(ep);
	Nw.BlockPen  = TEXT_FPEN(ep);

	if (GETF_ICONACTIVE(ep) && win->Flags & WFLG_WINDOWACTIVE)      /*  KTS */
	    Nw.Flags |= WFLG_ACTIVATE;

	if (newwin = opensharedwindow (&Nw))
	{
	    Nw.BlockPen  = (unsigned char)-1;

	    rem_prop (ep);
	    closesharedwindow (win);
	    SETF_ICONMODE(ep,1);

	    ep->win	 = newwin;
	}
	else
	    window_title (); /* undo effects of format_string() */
    }
} /* iconify */


/*DEFHELP #cmd win UNICONIFY - uniconify the window */

DEFUSERCMD("uniconify", 0, CF_VWM|CF_ICO, void, uniconify, (void),)
{
    ED	* ep = Ep;
    WIN * win = ep->win;
    WIN * newwin;
    RP	* rp;

    if (GETF_ICONMODE(ep))
    {
	ep->config.iwinx = win->LeftEdge;
	ep->config.iwiny = win->TopEdge;
	Nw.LeftEdge	 = ep->config.winx;
	Nw.TopEdge	 = ep->config.winy;
	Nw.Width	 = ep->config.winwidth;
	Nw.Height	 = ep->config.winheight;
	Nw.Title	 = ep->wtitle;
	Nw.Flags	 = WINDOWFLAGS;
	Nw.DetailPen	 = TEXT_BPEN(ep);
	Nw.BlockPen	 = TEXT_FPEN(ep);

	if (newwin = opensharedwindow (&Nw))
	{
	    closesharedwindow (win);

	    win = ep->win = newwin;
	    rp	= win->RPort;

	    if (ep->font)
		SetFont (rp, ep->font);

	    set_window_params ();
	    rest_prop (ep);

	    MShowTitle	 = 0;
	    SETF_ICONMODE (ep, FALSE);

	    text_adjust (TRUE);

	    menu_strip (currentmenu(),win);   /* PATCH_NULL [13 Jan 1993] : added currentmenu(), */
	    window_title ();
	}
    }
} /* uniconify */


/*DEFHELP #cmd win NEWWINDOW - open new window using default window parameters */

DEFUSERCMD("newwindow", 0, CF_VWM|CF_ICO, void, do_newwindow, (void),)
{
    WIN * win;

    if (Ep)
    {
	text_sync ();
    }
    
    if (text_init (Ep, NULL, &Nw))
    {
	Nw.Title = Ep->wtitle;
	Nw.Flags = WINDOWFLAGS;

	if (win = opensharedwindow (&Nw))
	{
	    menu_strip (currentmenu(),win);   /* PATCH_NULL [13 Jan 1993] : added currentmenu(), */

	    Ep->win = win;
	    set_window_params ();
	    Ep->propgad = (struct Gadget *)add_prop (win);

	    text_load ();
	}
	else
	{
	    text_uninit ();
	} /* if opensharedwindow */
    } /* if text_init */
} /* do_newwindow */


/*
 *  openwindow with geometry specification.  Negative number specify
 *  relative-right / relative-left (leftedge & topedge), or relative-width /
 *  relative height (width & height).
 *
 *	<leftedge><topedge><width><height>
 *
 *  Example:	+10+10-20-20	Open window centered on screen 10 pixels
 *				from the border on all sides.
 */

/*DEFHELP #cmd win OPENWINDOW geo -open new window using specified geometry. */

DEFUSERCMD("openwindow", 1, CF_VWM|CF_ICO, void, do_openwindow, (void),)
{
    WIN *win;

    if (Ep)
	text_sync ();

    if (text_init (Ep, NULL, &Nw))
    {
	GeometryToNW (av[1], &Nw);

	Nw.Title = Ep->wtitle;
	Nw.Flags = WINDOWFLAGS;

	if (win = opensharedwindow (&Nw))
	{
	    menu_strip (currentmenu(),win);   /* PATCH_NULL [13 Jan 1993] : added currentmenu(), */

	    Ep->win = win;
	    set_window_params ();
	    Ep->propgad = (struct Gadget *)add_prop (win);

	    text_load ();
	}
	else
	{
	    text_uninit ();
	} /* if opensharedwindow */
    } /* if text_init */
} /* do_openwindow */


Prototype WIN * TOpenWindow (struct NewWindow * nw);

WIN * TOpenWindow (struct NewWindow * nw)
{
    WIN * win;
    struct Screen * PubScreen;

    /* get WB or any screen */
    PubScreen = LockPubScreen (XDMEArgs.publicscreenname);

    if (PubScreen)
    {
	nw->MinWidth = PubScreen->WBorLeft + PubScreen->WBorRight +
		4 * GfxBase->DefaultFont->tf_XSize;
	nw->MinHeight = PubScreen->WBorTop + PubScreen->Font->ta_YSize * 5 +
		1 + PubScreen->WBorBottom;
    }
    else
    {
	nw->MinWidth  = 100;
	nw->MinHeight = 50;
    }

    win = OpenWindowTags (nw,
		WA_RptQueue, 1,
		WA_PubScreen, PubScreen,
		WA_AutoAdjust, TRUE,
		TAG_DONE);

    if (PubScreen)
	UnlockPubScreen (XDMEArgs.publicscreenname, PubScreen);

    return (win);
} /* TOpenWindow */


Prototype WIN * opensharedwindow (struct NewWindow *nw);

WIN * opensharedwindow (struct NewWindow *nw)
{
    WIN * win;

    if (Sharedport)
	nw->IDCMPFlags = 0L;
    else
	nw->IDCMPFlags = IDCMPFLAGS;

    win = TOpenWindow (nw);

    if (win)
    {
	long xend = win->Width - win->BorderRight - 1;
	long yend = win->Height- win->BorderBottom - 1;

	if (Sharedport)
	{
	    win->UserPort = Sharedport;
	    ModifyIDCMP (win, IDCMPFLAGS);
	} else
	{
	    Sharedport = win->UserPort;
	}

	if (xend > win->BorderLeft && yend > win->BorderTop)
	{
	    SetAPen (win->RPort, nw->DetailPen);
	    RectFill (win->RPort, win->BorderLeft, win->BorderTop, xend, yend);
	    SetAPen (win->RPort, nw->BlockPen);
	}
    }

    return (win);
} /* opensharedwindow */


/* the following function straight from RKM by TJM */

static void StripIntuiMessages (struct MsgPort *mp, struct Window *win)
{
    IMESS *msg, *succ;

    msg = (IMESS *)mp->mp_MsgList.lh_Head;

    while (succ = (IMESS *)msg->ExecMessage.mn_Node.ln_Succ)
    {
	if (msg->IDCMPWindow == win)
	{
	    Remove ((struct Node *)msg);
	    ReplyMsg ((struct Message *)msg);
	}

	msg = succ;
    }
} /* StripIntuiMessages */


/* modifed TJM to close win's immediately using CloseWindowSafely from RKM */
Prototype void closesharedwindow (WIN * win);

void closesharedwindow (WIN * win)
{
    if (win)
    {
	SetWindowTitles (win, "", (char *)-1);
	ClearMenuStrip (win);

	Forbid ();

	StripIntuiMessages (win->UserPort,win);

	win->UserPort = NULL;
	ModifyIDCMP (win,0);

	Permit ();

	CloseWindow (win);
    }
} /* closesharedwindow */


Prototype int getyn (char * title, char * text, char * gads, ...);

int getyn (char * title, char * text, char * gads, ...)
{
    va_list va;
    int     result;
    static struct EasyStruct es =
    {
	sizeof (struct EasyStruct),
	0L, 0, 0, 0
    };

    va_start (va, gads);

    es.es_Title        = title;
    es.es_TextFormat   = text;
    es.es_GadgetFormat = gads;

    result = EasyRequestArgs (Ep->win, &es, NULL, va);

    va_end (va);

    return (result);
} /* getyn */


Prototype void show_title (char *);

void show_title (char * title)
{
    if (GETF_SHOWTITLE(Ep) && !GETF_ICONMODE(Ep))
    {
	SetWindowTitles (Ep->win, title, (char *)-1);
	MShowTitle = 3;
    } /* if showtitle */
} /* show_title */


Prototype void title (char * fmt, ...);

void title (char * fmt, ...)
{
    va_list va;

    va_start (va, fmt);

    vsprintf (tmp_buffer, fmt, va);

    show_title (tmp_buffer);
} /* title */


/*DEFHELP #cmd prefs,win SETDEFTITLE string - Sets the pattern for the window-title. */

#define DEFAULT_TITLE	    "%l/%L %c %C %m %f %i"

static char * default_title = DEFAULT_TITLE;
static char title_pattern[30] = DEFAULT_TITLE;

DEFUSERCMD("setdeftitle", 1, CF_VWM|CF_ICO|CF_COK, void, do_setdeftitle, (void),)
{
    if (!*av[1])
	strcpy (title_pattern, default_title);
    else
    {
	strncpy (title_pattern, av[1], sizeof (title_pattern));
	title_pattern[sizeof (title_pattern)-1] = 0;
    }

    window_title ();
} /* do_setdeftitle */


/*DEFLONG #long SETDEFTITLE,SETDEFICONTITLE

This command allows to specify a pattern from which XDME will built a
string and display it at the appropriate position. The string can contain
any characters (like in @{B}printf()@{UB}). The following characters are replaced by
a special string, however:

	Sequence    Replacement

	   @{B}%%@{UB}	     A single @{B}%@{UB} in the resulting string
	   @{B}%l@{UB}	     the current line
	   @{B}%L@{UB}	     the number of lines
	   @{B}%c@{UB}	     the current column
	   @{B}%C@{UB}	     the code of the character under the cursor in hex
	   @{B}%m@{UB}	     modified flag (either @{B}@{UB}- or @{B}*@{UB})
	   @{B}%f@{UB}	     the current filename
	   @{B}%p@{UB}	     the last 20 characters of the current path
	   @{B}%b@{UB}	     the actual blocktype (@{B}L@{UB} for line, @{B}N@{UB} for character oriented
		    and @{B}V@{UB} for vertical

The defaults for XDME's title are: @{B}%l/%L %C %c %m %f %i@{UB}
The defaults for XDME's icon are: @{B}%f@{UB}
*/

Prototype void format_string (char * dest, const char * fmt);

void format_string (char * dest, const char * fmt)
{
    static BPTR lock;
    static char path[32];

    while (*fmt)
    {
	if (*fmt == '%')
	{
	    fmt ++;

	    switch (*fmt)
	    {
	    case '%':
		*dest ++ = '%';
		break;

	    case 'l': /* current line */
		sprintf (dest, "%4ld", Ep->line+1);
		dest += strlen (dest);
		break;

	    case 'L': /* number of lines */
		sprintf (dest, "%4ld", Ep->lines);
		dest += strlen (dest);
		break;

	    case 'c': /* current column */
		sprintf (dest, "%3ld", Ep->column+1);
		dest += strlen (dest);
		break;

	    case 'C': /* current character */
		sprintf (dest, "%02x", Ep->column >= Clen ? ' ' : Current[Ep->column]);
		dest += strlen (dest);
		break;

	    case 'm': /* modified ? */
		*dest ++ = GETF_MODIFIED(Ep) ? '*' : '-';
		break;

	    case 'f': /* filename */
		strcpy (dest, Ep->name);
		dest += strlen (dest);
		break;

	    case 'b': { /* block-type */
		extern UWORD block_type;
		*dest ++ = *(" LNV" + block_type);
		break; }

	    case 'i': /* insertmode */
		if (!GETF_INSERTMODE(Ep))
		{
		    strcpy (dest, "Ovr");
		    dest += 3;
		}

		break;

	    case 'p': /* path */
		/* just do this, if we don't have the path already */
		if (Ep->dirlock != lock)
		{
		    int len;
		    char buffer[256];

		    /* put path into a separate buffer so we can cut it
		       later */
		    getpathto (Ep->dirlock, NULL, buffer);

		    /* cut path to max. 24 chars */
		    len = strlen (buffer);
		    if (len > 20)
		    {
			len -= 20;
			strcpy (path, "...");
		    }
		    else
		    {
			*path = 0;
			len = 0;
		    }

		    strcat (path, buffer+len);
		    lock = Ep->dirlock;
		}

		strcpy (dest, path);

		dest += strlen (dest);
		break;
	    }

	    fmt ++;
	}
	else
	    *dest ++ = *fmt ++;
    }

    *dest = 0;
} /* format_string */


Prototype void window_title (void);

void window_title (void)
{
    if (GETF_MEMORYFAIL(Ep))
    {
	title(" -- NO MEMORY -- ");
	SETF_MEMORYFAIL(Ep,0);
	text_redisplay ();
    }

    if (GETF_MFORCETITLE(Ep))
    {
	MShowTitle = 0;
	SETF_MFORCETITLE(Ep,0);
    }

    if (GETF_ICONMODE(Ep))
	return;

    if (MShowTitle)
    {
	MShowTitle --;
    }
    else
    {
	int    len;
	int    maxlen;
	WORD   width;
	FONT * oldfont;
	ED   * ep    = Ep;
	WIN  * win   = ep->win;
	RP   * rp    = win->RPort;
	struct TextExtent bounds;

	format_string (ep->wtitle, title_pattern);

	len = strlen (ep->wtitle);

	if (len < Columns && Columns < sizeof (ep->wtitle))
	{
	    setmem (ep->wtitle+len, Columns - len + 1, ' ');
	    ep->wtitle[Columns + 1] = 0;
	}

	/*
	 *  Update title
	 */

	if (GETF_WINDOWTITLES(Ep))                            /* PATCH_NULL 16-08-94 */
	    SetWindowTitles(ep->win, ep->wtitle, (char *)-1); /* PATCH_NULL 16-08-94 */
	else						      /* PATCH_NULL 16-08-94 */
	{
	    oldfont = win->RPort->Font;
	    SetFont (rp, win->WScreen->RastPort.Font);

	    win->Title = ep->wtitle;

	    SetAPen (rp, TITLE_FPEN(ep));
	    SetBPen (rp, TITLE_BPEN(ep));
	    SetDrMd (rp, JAM2);
	    SetWrMsk (rp, ALL_MASK);

	    width = win->Width - 96;

	    maxlen = TextFit (rp, ep->wtitle, len, &bounds, NULL,
			1L, width, rp->Font->tf_YSize);

	    /* write new text */
	    /* SetWindowTitles(ep->win, ep->wtitle, (char *)-1); / * PATCH_NULL each time the window was deactivated, parts of the title went away */
	    Move (rp, 30, rp->Font->tf_Baseline+1);
	    Text (rp, ep->wtitle, maxlen);      /* No flash */

	    /* clear to eol */
	    width = win->Width - 66;

	    if (rp->cp_x < width) {

    /* >>> PATCH_NULL 28-07-94 - Patch to clear the Title in a right manner */
		if (win->Flags & WFLG_WINDOWACTIVE) {
		    SetAPen(rp, 3);
		} else {
		    SetAPen(rp, 0);
		}
		RectFill (rp, rp->cp_x, 1, width, rp->Font->tf_YSize);
    /* <<< PATCH_NULL 28-07-94 - this patch works only w/ 4-color WB */
	    } /* if */

	    SetAPen (rp, TEXT_FPEN(ep));
	    SetBPen (rp, TEXT_BPEN(ep));
	    SetFont (rp, oldfont);
	} /* if */
    }
} /* window_title */


Prototype void set_window_params (void);

void set_window_params (void)
{
    ED	* ep = Ep;
    WIN * win = ep->win;
    RP	* rp = win->RPort;
    WORD  t;
    WORD  x;

    /* Set Character-Size */
    Xsize = rp->Font->tf_XSize;
    Ysize = rp->Font->tf_YSize + LineDistance;

    /* Set Borders */
    Xbase = win->BorderLeft;
    Ybase = win->BorderTop;

    /* Find Width/Height */
    Xpixs   = win->Width - win->BorderRight - Xbase;
    Ypixs   = win->Height- win->BorderBottom- Ybase;

    /* Find Width/Height in Characters */
    Columns = Xpixs / Xsize;
    Lines    = Ypixs / Ysize;

    /* Now Calculate Xpixs/Ypixs */
    Xpixs = Xbase + Columns * Xsize - 1;
    Ypixs = Ybase + Lines    * Ysize - 1;

    /* Set Base for Text() */
    XTbase  =  Xbase;
    YTbase  =  Ybase + rp->Font->tf_Baseline + (LineDistance + 1)/2;

    /* Set Pens */
    SetAPen(rp, TEXT_FPEN(Ep));
    SetBPen(rp, TEXT_BPEN(Ep));

    /* Initialize Arrays of X/Y-Coords for faster rendering */
    if (ColumnPos[1] != Xsize)
	for (t=0, x=0; t<MAXLINELEN; t++, x += Xsize)
	    ColumnPos[t] = x;

    if (RowPos[1] != Ysize)
	for (t=0, x=0; t<MAXROWS; t++, x += Ysize)
	    RowPos[t] = x;
} /* set_window_params */


/*DEFHELP #cmd prefs,win RESIZE cols rows -Resize current window. E.G: (@{B}resize 70 23@{UB}) */

DEFUSERCMD("resize", 2, CF_VWM, void, do_resize, (void),)
{
    WIN * win	 = Ep->win;
    int   cols	 = atoi (av[1]);
    int   rows	 = atoi (av[2]);
    WORD  width  = (cols * Xsize) + win->BorderLeft +
		   win->BorderRight;
    WORD  height = (rows * Ysize) + win->BorderTop +
		   win->BorderBottom;

    if (cols < 2 || rows < 1)
    {
	error ("resize:\nCannot make window this small.\n"
	       "Window must have at least 2 columns and 1 row !");
    } else if (width > win->WScreen->Width - win->LeftEdge ||
	    height > win->WScreen->Height - win->TopEdge)
	    {
	error ("resize:\nwindow too big (move it to\n"
	       "upper left corner and retry)");
    } else
    {
	SizeWindow (win, width - win->Width, height - win->Height);
	Delay (5);    /* wait 0.1 seconds for OS to resize */
    }
} /* do_resize */


/* Convert geometry to nw params. */

Prototype char * geoskip (char * ptr, int * pval, int * psgn);

char * geoskip (char * ptr, int * pval, int * psgn)
{
    ptr = skip_whitespace (ptr);

    if (*ptr == '-')
	*psgn = -1;
    else
	*psgn = 1;

    if (*ptr == '-' || *ptr == '+')
	ptr ++;

    *pval = strtol (ptr, &ptr, 0);

    return (ptr);
} /* geoskip */


/* Convert GEO-String to NewWindow-structure */

Prototype void GeometryToNW (char * geo, struct NewWindow *nw);

void GeometryToNW (char * geo, struct NewWindow *nw)
{
    int 	  n;
    int 	  sign;
    struct Screen scr;

    GetScreenData (&scr, sizeof (scr), WBENCHSCREEN, NULL);

    if (*geo)
    {
	geo = geoskip (geo, &n, &sign);
	if (sign > 0)
	    nw->LeftEdge = n;
	else
	    nw->LeftEdge = scr.Width - n;
    }

    if (*geo)
    {
	geo = geoskip (geo, &n, &sign);
	if (sign > 0)
	    nw->TopEdge = n;
	else
	    nw->TopEdge = scr.Height - n;
    }

    if (*geo)
    {
	geo = geoskip (geo, &n, &sign);
	if (sign > 0)
	    nw->Width = n;
	else
	    nw->Width = scr.Width - nw->LeftEdge - n;
    }

    if (*geo)
    {
	geoskip (geo, &n, &sign);

	if (sign > 0)
	    nw->Height = n;
	else
	    nw->Height = scr.Height - nw->TopEdge - n;
    }
} /* GeometryToNW */


/* prop gadget stuff (TJM) */

Prototype void rest_prop (ED * ep);

void rest_prop (ED * ep)
{
    if (ep->propgad)
    {
	struct PropGadget * pg = (struct PropGadget *)ep->propgad;

	AddGList (ep->win, &pg->scroller, 0, NUM_GADS, NULL);
	RefreshGList (&pg->scroller, ep->win, NULL, NUM_GADS);
    }
} /* rest_prop */


Prototype void rem_prop (ED * ep);

void rem_prop (ED * ep)
{
    if (ep->propgad)
    {
	struct PropGadget * pg = (struct PropGadget *)ep->propgad;

	RemoveGList (ep->win, &pg->scroller, NUM_GADS);
    }
} /* rest_prop */


Prototype struct PropGadget * add_prop (struct Window * win);

struct PropGadget * add_prop (struct Window * win)
{
    struct PropGadget * pg;
    struct DrawInfo   * mydrawinfo;
    struct Image      * dummy;
    struct TagItem	taglist[5] =
    {
	SYSIA_Which, NULL,
	SYSIA_DrawInfo, NULL,
	SYSIA_Size, 0,
    };
    UWORD  height, size,
	   size_width, size_height,
	   depth_width, depth_height;

    /* Get memory */
    if (!(pg = AllocMem (sizeof(struct PropGadget), 0)))
	return NULL;

    /* copy default flags/modes/etc. */
    memmove (pg, &gadgetdefaults, sizeof(struct PropGadget));

    /* find out sizes */
    mydrawinfo = GetScreenDrawInfo (win->WScreen);

    if (win->WScreen->ViewPort.Modes & HIRES)
    {
	size = SYSISIZE_MEDRES;

	/* Don't use HIRES, since that will produce too large images */
	/* if (win->WScreen->ViewPort.Modes & LACE)
	    size = SYSISIZE_HIRES;
	else */
    }
    else
	size = SYSISIZE_LOWRES;

    taglist[0].ti_Data = DEPTHIMAGE;
    taglist[1].ti_Data = (ULONG)mydrawinfo;
    taglist[2].ti_Data = size;
    taglist[3].ti_Tag  = IA_Height;	/* the depth-gadget needs a height */
    taglist[3].ti_Data = win->BorderTop;
    taglist[4].ti_Tag  = TAG_END;

    /* get size of depth-gadget */
    if (!(dummy = (struct Image *)NewObjectA (NULL, "sysiclass", taglist)) )
    {
oom:
	FreeMem (pg, sizeof (struct PropGadget));
	FreeScreenDrawInfo (win->WScreen, mydrawinfo);
	return (NULL);
    }

    depth_width  = dummy->Width;
    depth_height = dummy->Height;

#ifdef NOT_DEF
    pg->depth.GadgetRender = pg->depth.SelectRender = (APTR)pg->depthimage;
#else
    DisposeObject (dummy);
#endif

    /* Get the ZOOMIMAGE here because we need the height */
    taglist[0].ti_Data = ZOOMIMAGE;

    if (!(pg->iconifyimage = (struct Image *)NewObjectA (NULL, "sysiclass", taglist)) )
	goto oom;

    pg->iconify.GadgetRender = pg->iconify.SelectRender = (APTR)pg->iconifyimage;

    taglist[0].ti_Data = SIZEIMAGE;
    taglist[3].ti_Tag  = TAG_END;

    /* get size of size-gadget */
    if (!(dummy = (struct Image *)NewObjectA (NULL, "sysiclass", taglist)) )
    {
	DisposeObject (pg->iconifyimage);
	goto oom;
    }

    size_width	= dummy->Width; 	/* width of up/down-gadgets */
    size_height = dummy->Height;	/* bottom offset */

    /* we don't need the image anymore */
    DisposeObject (dummy);

    taglist[0].ti_Data = UPIMAGE;

    if (!(pg->upimage = (struct Image *)NewObjectA (NULL, "sysiclass", taglist)) )
    {
	DisposeObject (pg->iconifyimage);
	goto oom;
    }

    pg->up.GadgetRender = pg->up.SelectRender = (APTR)pg->upimage;

    height = pg->upimage->Height;

    taglist[0].ti_Data = DOWNIMAGE;

    if (!(pg->downimage = (struct Image *)NewObjectA (NULL, "sysiclass", taglist)) )
    {
	DisposeObject (pg->iconifyimage);
	DisposeObject (pg->upimage);
	goto oom;
    }

    pg->down.GadgetRender = pg->down.SelectRender = (APTR)pg->downimage;

    /* Release drawinfo */
    FreeScreenDrawInfo (win->WScreen, mydrawinfo);

    /* Now init all sizes/positions */
    pg->scroller.TopEdge    = depth_height + 1;
    pg->scroller.Height     = -(depth_height + size_height + 2*height + 2);
    pg->up.LeftEdge =
	pg->down.LeftEdge   = -(size_width - 1);
    pg->scroller.LeftEdge   = -(size_width - 4);
    pg->down.TopEdge	    = -(size_height + height - 1);
    pg->up.TopEdge	    = pg->down.TopEdge - height;
    pg->up.Width =
	pg->down.Width	    = size_width;
    pg->scroller.Width	    = size_width - 6;
    pg->up.Height =
	pg->down.Height     = height;
#ifdef NOT_DEF
    pg->depth.Width	    = pg->depthimage->Width;
    pg->depth.Height	    = pg->depthimage->Height;
    pg->depth.LeftEdge	    = -(pg->depth.Width - 1);
#endif
    pg->iconify.Width	    = pg->iconifyimage->Width;
    pg->iconify.Height	    = pg->iconifyimage->Height;
    pg->iconify.LeftEdge    = -(depth_width + pg->iconify.Width - 3);

    /* Other fields */
    pg->scroller.GadgetRender = (APTR)&pg->simage;
    pg->scroller.SpecialInfo  = (APTR)&pg->pinfo;

    /* Link gadgets */
    pg->scroller.NextGadget = &pg->up;
    pg->up.NextGadget	    = &pg->down;
    pg->down.NextGadget     = &pg->iconify;
    /* pg->depth.NextGadget    = &pg->iconify; */

    /* and add them to the window */
    AddGList (win, &pg->scroller, 0, NUM_GADS, NULL);
    RefreshGList (&pg->scroller, win, NULL, NUM_GADS);

    noadj = 0;	    /* allow scroller refreshing */

    /* return field */
    return (pg);
} /* add_prop */


Prototype void free_prop (struct PropGadget * pg);

void free_prop (struct PropGadget * pg)
{
    if (pg)
    {
	/* Free elements */
	DisposeObject (pg->upimage);
	DisposeObject (pg->downimage);
	/* DisposeObject (pg->depthimage); */
	DisposeObject (pg->iconifyimage);

	/* Free struct */
	FreeMem (pg, sizeof (struct PropGadget));
    }
} /* free_prop */


Prototype void prop_adj (void);

void prop_adj (void)
{
    ULONG VertBody, VertPot;

     /* block adjustment when already set by prop gad */
    if (!Ep->propgad || noadj || GETF_ICONMODE(Ep))
	return;

    /* If there are less lines than the window has, the scroller has
       full size */
    if (Ep->lines <= Lines)
    {
	VertPot  = 0;
	VertBody = MAXBODY;
    } else
    {
	ULONG overlap = Lines - (Lines * PageJump) / 100;
	ULONG total;

	/* If we have more lines visible than the text actually has (ie.
	   there are empty lines visible) the total number of lines is
	   (topline + Lines) and the position is at its maximum. Else, the
	   number of lines is the length of the text and the position is
	   (toppos / invisible lines) */

	if (Ep->topline + Lines > Ep->lines)
	{
	    total = Ep->topline + Lines;

	    VertPot  = MAXPOT;
	} else
	{
	    total = Ep->lines;

	    VertPot  = (Ep->topline * MAXPOT) / (total - Lines);
	}

	/* The body-size is (number of lines for jump-scroll / all other
	   lines */
	VertBody = ((Lines - overlap) * MAXBODY) / (total - overlap);
    }

    /* set it */
    NewModifyProp (Ep->propgad, Ep->win, NULL,
	((struct PropInfo *)Ep->propgad->SpecialInfo)->Flags,
	MAXPOT, VertPot,
	MAXBODY, VertBody,
	1
    );
} /* prop_adj */


Prototype ULONG new_top (void);

ULONG new_top (void)
{
    ULONG top;

    if (Ep->lines <= Lines)
    {
	top = 0;
    } else
    {
	ULONG total;

	if (Ep->topline + Lines > Ep->lines)
	    total = Ep->topline;
	else
	    total = Ep->lines - Lines;

	top = ((struct PropInfo *)Ep->propgad->SpecialInfo)->VertPot *
		total / MAXPOT;

	/* we may have to adjust the body-size (the user already adjust the
	   position) */
	if (Ep->topline + Lines > Ep->lines)
	{
	    ULONG VertBody;
	    ULONG overlap = Lines - (Lines * PageJump) / 100;

	    VertBody = ((Lines - overlap) * MAXBODY) /
		       (Ep->topline + Lines - overlap);

	    NewModifyProp (Ep->propgad, Ep->win, NULL,
		((struct PropInfo *)Ep->propgad->SpecialInfo)->Flags,
		MAXPOT, ((struct PropInfo *)Ep->propgad->SpecialInfo)->VertPot,
		MAXBODY, VertBody,
		1
	    );
	}
    }

    return (top);
} /* new_top */


/*DEFHELP #cmd prefs,win TITLE title - set window title manually */

DEFUSERCMD("title", 1, CF_VWM|CF_ICO, void, do_title, (void),)
{
    static char buffer[256];

    strncpy ((char *)buffer, (char *)av[1], sizeof (buffer)-1);
    buffer[sizeof (buffer)-1] = 0;

    show_title ((char *)buffer);
} /* do_title */


/*DEFHELP #cmd win SHOWLOG - XDME collects all warnings internally. These can now be showed again with this command. */

DEFUSERCMD("showlog", 0, CF_VWM|CF_ICO|CF_COK, void, do_showlog, (void),)
{
    ED * ep = Ep;
    int t;
    UBYTE * ptr;

    /* Neues Fenster auf */
    do_newwindow ();

    /* Nur wenn das ging ... */
    if (ep != Ep)
    {
	for (t=0; t<numlogs; Ep->lines = ++t)
	{
	    if ( !makeroom(8) || !(ptr = allocline (strlen (log_messages[t]) + 1)) )
		break;

	    strcpy (ptr, log_messages[t]);

	    SETLINE(Ep,t,ptr);
	}

	if (numlogs)
	    strcpy (Current, log_messages[0]);

	Clen = strlen (Current);

	text_redisplay ();
	SETF_VIEWMODE(ep,1);
    }
} /* do_showlog */


static void log (char * text)
{
    /* Wenn schon alle Slots voll sind, Platz schaffen */
    if (numlogs == MAX_LOG)
    {
	/* Erste message freigeben */
	free (log_messages[0]);

	/* Rest nach vorne schieben */
	movmem (&log_messages[1], &log_messages[0],
					    sizeof(char *) * (MAX_LOG - 1));

	/* Anzahl anpassen */
	numlogs --;
    }

    /* Wenn neuer log angefügt werden konnte, anzahl erhöhen */
    if (log_messages[numlogs] = strdup (text))
	numlogs ++;

} /* log */


Prototype void error (char * fmt, ...);

void error (char * fmt, ...)
{
    va_list va;
    static struct EasyStruct es =
    {
	sizeof (struct EasyStruct),
	NULL,
	"XDME Error",
	NULL,
	"Ok"
    };

    va_start (va, fmt);

    SETF_ABORTCOMMAND(Ep,1);

    if (GETF_NOREQUEST(Ep))
    {
	char * ptr;			    /* PATCH_NULL [25 Jan 1993] : line added */

	vsprintf (tmp_buffer, fmt, va);     /* PATCH_NULL [25 Jan 1993] : line added */
	ptr = tmp_buffer;		    /* PATCH_NULL [25 Jan 1993] : line added */

	while (*ptr) {                      /* PATCH_NULL [25 Jan 1993] : line added */
	    if (*ptr == '\n') *ptr = ' ';   /* PATCH_NULL [25 Jan 1993] : line added */
	    ptr ++;			    /* PATCH_NULL [25 Jan 1993] : line added */
	} /* while */			    /* PATCH_NULL [25 Jan 1993] : line added */

	show_title (tmp_buffer);            /* PATCH_NULL [25 Jan 1993] : line added */
	log (tmp_buffer);
    }
    else
    {
	es.es_TextFormat = fmt;

	EasyRequestArgs ((Ep ? Ep->win : NULL), &es, NULL, va);

    } /* if (not) noRequest */              /* PATCH_NULL [25 Jan 1993] : line added */

    va_end (va);

    if (CMDSH_Active && CMDSH_ErrorsOut) { /* PATCH_NULL 21-09-94 : added */
	va_start (va, fmt);
	CMDSH_Print(fmt, (ULONG *)va);
	va_end (va);
    } /* if */
} /* error */


Prototype void warn (char * fmt, ...);

void warn (char * fmt, ...)
{
    va_list va;

    va_start (va, fmt);

    vsprintf (tmp_buffer, fmt, va);

    show_title (tmp_buffer);
    log (tmp_buffer);

    va_end (va);

    if (CMDSH_Active && CMDSH_WarningsOut) { /* PATCH_NULL 21-09-94 : added */
	va_start (va, fmt);
	CMDSH_Print(fmt, (ULONG *)va);
	va_end (va);
    } /* if */
} /* warn */


/*
	Changes the window-size and position. If the position is negative,
	it's calculated as offset from the right/bottom border. If
	width/height are negative, they are relative to the screen's
	width/height. If they are 0, they are not changed.
*/

/*DEFHELP #cmd win SETGEOMETRY x y width height - Set x/y position and width/height of XDME's window. */

DEFUSERCMD("setgeometry", 4, CF_VWM, void, do_setgeometry, (void),)
{
    static const char error_text[] =
	":\n"
	"Cannot set window to\n";
    WORD top, left, width, height;
    SCREEN * screen;
    char * error_ptr = NULL;
    long   minsize = 0;

    left   = strtol (av[1], NULL, 0);
    top    = strtol (av[2], NULL, 0);
    width  = strtol (av[3], NULL, 0);
    height = strtol (av[4], NULL, 0);

    screen = Ep->win->WScreen;

    if (left < 0)
	left += screen->Width;
    if (top < 0)
	top += screen->Height;
    if (width < 0)
	width += screen->Width + 1;
    if (height < 0)
	height += screen->Height + 1;

    if (!width || GETF_ICONMODE(Ep))
	width = Ep->win->Width;
    if (!height || GETF_ICONMODE(Ep))
	height = Ep->win->Height;

    /* if the left/top is not ok, or we are not in iconmode AND the
       width/height is wrong ... */
    if (left < 0)
	error_ptr = "left edge negative";
    else if (top < 0)
	error_ptr = "top edge negative";
    else if (left > screen->Width)
	error_ptr = "left edge too big";
    else if (top > screen->Height)
	error_ptr = "top edge too big";
    else if (!GETF_ICONMODE(Ep))
    {
	if (width < Nw.MinWidth)
	{
	    error_ptr = "width is too narrow\n(The minimal width is ";
	    minsize = Nw.MinWidth;
	} else if (height < Nw.MinHeight)
	{
	    error_ptr = "height is too low\n(The minimal height is ";
	    minsize = Nw.MinHeight;
	} else if (left + width > screen->Width)
	    error_ptr = "right edge is too big";
	else if (top + height > screen->Height)
	    error_ptr = "bottom edge is too big";
    }

    if (error_ptr)
    {
	if (minsize != 0)
	{
	    error ("%s%s(%ld/%ld), W:%ld, H:%ld\nbecause the %s%ld).",
		    av[0], error_text, left, top, width, height, error_ptr,
		    minsize);
	} else
	{
	    error ("%s%s(%ld/%ld), W:%ld, H:%ld\nbecause the %s.",
		    error_text, av[0], left, top, width, height, error_ptr);
	}
    } else
    {
	ChangeWindowBox (Ep->win, left, top, width, height);
	Delay (5);
    }
} /* do_setgeometry */


/*DEFHELP #cmd win TOBACK - Move active window to back */

DEFUSERCMD("toback", 0, CF_VWM|CF_COK|CF_ICO, void, do_toback, (void),)
{
    WindowToBack (Ep->win);
} /* do_toback */


/*DEFHELP #cmd win TOFRONT - Move active window to front */

DEFUSERCMD("tofront", 0, CF_VWM|CF_COK|CF_ICO, void, do_tofront, (void),)
{
    WindowToFront (Ep->win);
} /* do_tofront */


/******************************************************************************
*****  ENDE win.c
******************************************************************************/
