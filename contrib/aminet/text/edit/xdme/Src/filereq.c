/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	All routines that handle file-requesters. We try to open ASL and
	if that's not possible, REQTOOLS.

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include <proto/asl.h>
#include <proto/intuition.h>
#include <proto/reqtools.h>
#include <proto/dos.h>

/**************************************
	    Globale Variable
**************************************/
Prototype struct FileRequester	 * FReq;
Prototype struct ReqToolsBase	 * ReqToolsBase;
Prototype struct rtFileRequester * RFReq;

struct FileRequester   * FReq;	  /* File-Req. structure */
struct rtFileRequester * RFReq;
struct ReqToolsBase    * ReqToolsBase;


/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/
static char pattern[64] = "";
static char file[108];
static char dir[PATHSIZE];


/**************************************
	   Interne Prototypes
**************************************/


/*DEFHELP #cmd prefs,io,requester PATTERN pat - sets the pattern for the filerequesters. */

DEFUSERCMD("pattern", 1, CF_VWM|CF_COK|CF_ICO, void, do_pattern, (void),)
{
    strncpy (pattern, av[1], 63);
    pattern[63] = 0;
}


/*DEFHELP #cmd requester,io ARPLOAD - NEWFILE with filerequester */

DEFUSERCMD("arpload", 0, CF_VWM, void, do_aslload, (void),)
{
    BPTR oldlock;
    BOOL ret;

    oldlock = CurrentDir (Ep->dirlock);

    splitpath (Ep->name, file, dir);

    if (!stricmp (file, "unnamed"))
	*file = 0;

    fixdirname (dir);

    if (AslBase)
    {		       /* If we have asl.library & requester */
	if (!FReq)
	    FReq = AllocAslRequest (ASL_FileRequest, NULL);

	if (!FReq)
	{
	    nomemory ();
	    return;
	}

	ret = AslRequestTags ((APTR)FReq,
		ASL_Hail,	"XDME Load New File",
		ASL_OKText,	"Load",
		ASL_Window,	Ep->win,
		ASL_FuncFlags,	FILF_NEWIDCMP | FILF_PATGAD,
		ASL_Dir,	dir,
		ASL_File,	file,
		ASL_Pattern,	pattern,
		ASL_LeftEdge,	Ep->config.aslleft + Ep->win->LeftEdge,     /* use config coords */
		ASL_TopEdge,	Ep->config.asltop + Ep->win->TopEdge,
		ASL_Width,	Ep->config.aslwidth,
		ASL_Height,	Ep->config.aslheight,
		TAG_DONE
	);

	Ep->config.asltop    = FReq->rf_TopEdge - Ep->win->TopEdge;	 /* save coords */
	Ep->config.aslleft   = FReq->rf_LeftEdge - Ep->win->LeftEdge;
	Ep->config.aslwidth  = FReq->rf_Width;
	Ep->config.aslheight = FReq->rf_Height;

	if (ret)
	{
	    BPTR newlock;

	    if ((newlock = Lock (FReq->rf_Dir, SHARED_LOCK)))
	    {
		BPTR cd = CurrentDir (oldlock);

		Ep->dirlock = (long)newlock;

		strcpy (pattern, FReq->rf_Pat);

		av[0] = (UBYTE *)"n";
		av[1] = (UBYTE *) FReq->rf_File;

		if (do_edit () == -1)
		{
		    Ep->dirlock = (long)cd;
		    UnLock (newlock);
		}
		else
		    UnLock (cd);
	    }
	} else
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    CurrentDir (oldlock);
	}
    } else
    {		   /* no asl.library? try reqtools */
	do_reqload ();
    }
} /* do_aslload */


Prototype int aslsave (void);

int aslsave (void)
{
    BPTR oldlock = CurrentDir (Ep->dirlock);
    BOOL ret;

    splitpath (Ep->name, file, dir);

    if (!stricmp (file, "unnamed"))
	*file = 0;

    fixdirname (dir);

    if (AslBase)
    {
	if (!FReq)
	    FReq = AllocAslRequest (ASL_FileRequest, NULL);

	if (!FReq)
	{
	    nomemory ();
	    return (FALSE);
	}

loop:
	ret = AslRequestTags( (APTR)FReq,
		ASL_Hail,	"XDME Save File",
		ASL_OKText,	"Save",
		ASL_Window,	Ep->win,
		ASL_FuncFlags,	FILF_NEWIDCMP | FILF_SAVE,
		ASL_Dir,	dir,
		ASL_File,	file,
		ASL_LeftEdge,	Ep->config.aslleft + Ep->win->LeftEdge,     /* use config coords */
		ASL_TopEdge,	Ep->config.asltop + Ep->win->TopEdge,
		ASL_Width,	Ep->config.aslwidth,
		ASL_Height,	Ep->config.aslheight,
		TAG_DONE
	);

	Ep->config.asltop    = FReq->rf_TopEdge - Ep->win->TopEdge;	 /* save coords */
	Ep->config.aslleft   = FReq->rf_LeftEdge - Ep->win->LeftEdge;
	Ep->config.aslwidth  = FReq->rf_Width;
	Ep->config.aslheight = FReq->rf_Height;

	if (ret)
	{
	    BPTR tmp_lock;

	    if (!stricmp (FReq->rf_File, "unnamed"))
	    {
		error ("arpsave:\nCannot save file as\n`unnamed'");
		*file = 0;
		goto loop;
	    }

	    CurrentDir (oldlock);

	    strcpy (file, FReq->rf_File);
	    strcpy (dir, FReq->rf_Dir);
	    strcpy (pattern, FReq->rf_Pat);

	    if (!*file)
	    {
		error ("%s:\nYou must specify a name", av[0]);
		goto loop;
	    }

	    AddPart (dir, file, PATHSIZE);

	    av[1] = (UBYTE *)dir;

	    if ((tmp_lock = Lock (dir, ACCESS_READ)))
	    {
		UnLock (tmp_lock);

		if (!getyn ("XDME Warning",
			"The file\n`%s'\ndoes already exist."
			"Do you want to overwrite it ?",
			"Yes|No", dir) )
		    return (FALSE);
	    }

	    return (do_saveas ());
	} else
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    CurrentDir (oldlock);
	}
    } else
    {
	return (do_reqsave ());
    }

    return (FALSE);
} /* aslsave */


/*DEFHELP #cmd requester,io ARPINSFILE - INSFILE with filerequester */

DEFUSERCMD("arpinsfile", 0, 0, void, do_aslinsfile, (void),)
{
    BPTR oldlock = CurrentDir (Ep->dirlock);
    BOOL ret;

    splitpath (Ep->name, file, dir);

    if (!stricmp (file, "unnamed"))
	*file = 0;

    fixdirname (dir);

    if (AslBase)
    {
	if (!FReq)
	    FReq = AllocAslRequest (ASL_FileRequest, NULL);

	if (!FReq)
	{
	    nomemory ();
	    return;
	}

	ret = AslRequestTags ((APTR)FReq,
		ASL_Hail,	"XDME Insert File",
		ASL_OKText,	"Insert",
		ASL_Window,	Ep->win,
		ASL_FuncFlags,	FILF_NEWIDCMP | FILF_PATGAD,
		ASL_Dir,	dir,
		ASL_File,	file,
		ASL_Pattern,	pattern,
		ASL_LeftEdge,	Ep->config.aslleft + Ep->win->LeftEdge,     /* use config coords */
		ASL_TopEdge,	Ep->config.asltop + Ep->win->TopEdge,
		ASL_Width,	Ep->config.aslwidth,
		ASL_Height,	Ep->config.aslheight,
		TAG_DONE );

	Ep->config.asltop    = FReq->rf_TopEdge - Ep->win->TopEdge;	 /* save coords */
	Ep->config.aslleft   = FReq->rf_LeftEdge - Ep->win->LeftEdge;
	Ep->config.aslwidth  = FReq->rf_Width;
	Ep->config.aslheight = FReq->rf_Height;

	if (ret)
	{
	    CurrentDir (oldlock);

	    /* Fields are READ-ONLY */
	    strcpy (file, FReq->rf_File);
	    strcpy (dir, FReq->rf_Dir);
	    strcpy (pattern, FReq->rf_Pat);

	    AddPart (dir, file, PATHSIZE);

	    av[0] = (UBYTE *)"i";
	    av[1] = (UBYTE *)dir;

	    do_edit ();
	} else
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    CurrentDir (oldlock);
	}
    } else
    {
	do_reqinsfile ();
    }
} /* do_aslinsfile */


/*DEFHELP #cmd requester,font ARPFONT - SETFONT with fontrequester */

DEFUSERCMD("arpfont", 0, CF_VWM, void, do_aslfont, (void),)
{
    if (AslBase)
    {
	struct FontRequester *fontreq;

#ifdef _DCC
	ULONG fwin = (ULONG)Ep->win;  /* put ASL on correct screen */
#endif

	struct TagItem FontTags[] =
	{   /* DICE 2.06.37 allows us to initialize this here... */
#ifdef _DCC
	    ASL_Window,     fwin,
	    ASL_LeftEdge,   Ep->config.aslleft + Ep->win->LeftEdge,	/* use config coords */
	    ASL_TopEdge,    Ep->config.asltop + Ep->win->TopEdge,
#else
	    ASL_Window,     0L,
	    ASL_LeftEdge,   0L,
	    ASL_TopEdge,    0L,
#endif
	    ASL_Hail,	    (ULONG)"XDME fixed width font",
	    ASL_FuncFlags,  FILF_NEWIDCMP | FONF_FIXEDWIDTH,
	    /* Removed Min & MaxHeight since I cannot figure out what the
	       user may want */
	    TAG_DONE
	};

#ifndef _DCC
	FontTags[0].ti_Data = (ULONG)Ep->win;
	FontTags[1].ti_Data = Ep->config.aslleft + Ep->win->LeftEdge;
	FontTags[2].ti_Data = Ep->config.asltop + Ep->win->TopEdge;
#endif

	if ((fontreq = (struct FontRequester *)
				AllocAslRequest (ASL_FontRequest,FontTags) ))
	{

	    if (AslRequest (fontreq, NULL))
	    {

		/*  Since we have GetFont() already, just pass
		 *   the name and size from fontreq's TextAttr to it.
		 *
		 *  What follows mimics SETFONT (in cmnd3.c)
		 */

		FONT * font = (FONT *)GetFont( fontreq->fo_Attr.ta_Name,
					     (short) fontreq->fo_Attr.ta_YSize );
		ED * ep = Ep;

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
		} else
		{
		    error ("aslfont:\nUnable to find font\n%s/%ld", fontreq->fo_Attr.ta_Name,
			   fontreq->fo_Attr.ta_YSize);
		}
	    }
	    else
		SETF_ABORTCOMMAND(Ep,1);

	    FreeAslRequest (fontreq);
	}
	else
	{
	    nomemory ();
	}
    } else
    {
	do_reqfont ();
    }
} /* do_aslfont */


/*  this little kludge gets a DIR: string for the ASL requester
 */

Prototype void fixdirname (char * dir);

void fixdirname (char * dir)
{
    if (!strlen (dir))
    {		      /* if no DIR; KLUDGE ALERT! */
	if (NameFromLock (Ep->dirlock, dir, PATHSIZE)!=DOSTRUE)
	{
	    dir[0] = 0; /* assumes NameFromLock doesn't clean up on err(??) */
	}
    }
} /* fixdirname */


/*
 *  Search backwards for first ':' or '/' and split path there.
 *  This subroutine may appear to be coded incorrectly to a novice
 *  programmer.  It isn't [now].
 */

Prototype void splitpath (char * name, char * file, char * dir);

void splitpath (char * name, char * file, char * dir)
{
    int i;

    strcpy (file, FilePart (name));

    i = (long)PathPart (name) - (long)name;

    movmem (name, dir, i);
    dir[i] = 0;
} /* splitpath */


/*
 *  The REQ interface uses the reqtools.library if it exists, else tries to
 *  use the ARP interfafce.
 */

/*DEFHELP requester,io REQLOAD - NEWFILE with ReqTools filerequester */

DEFUSERCMD("reqload", 0, CF_VWM, void, do_reqload, (void),)
{
    BPTR oldlock = CurrentDir (Ep->dirlock);

    splitpath (Ep->name, file, dir);

    if (!stricmp (file, "unnamed"))
	*file = 0;

    fixdirname (dir);

    if (ReqToolsBase)
    {		     /* If we have req.library & requester */
	if (!RFReq)
	    RFReq = rtAllocRequestA (RT_FILEREQ, NULL);

	if (!RFReq)
	{
	    nomemory ();
	    return;
	}

	rtChangeReqAttr (RFReq, RTFI_Dir, dir, TAG_DONE);

	if (rtFileRequest (RFReq, file, "XDME Load New File",
		RTFI_OkText,	"Load",
		RT_Window,	Ep->win,
		RT_ReqPos,	REQPOS_TOPLEFTSCR,
		RT_LeftOffset,	Ep->config.aslleft,
		RT_TopOffset,	Ep->config.asltop,
		RTFI_Height,	Ep->config.aslheight,
		TAG_DONE ))
		{
	    BPTR newlock;

	    if ((newlock = Lock (RFReq->Dir, SHARED_LOCK)))
	    {
		BPTR cd = CurrentDir (oldlock);

		Ep->dirlock = (long)newlock;

		strcpy (dir, RFReq->Dir);
		AddPart (dir, file, PATHSIZE);

		av[0] = (UBYTE *)"n";
		av[1] = (UBYTE *) file;

		if (do_edit () == -1)
		{
		    Ep->dirlock = (long)cd;
		    UnLock (newlock);
		}
		else
		    UnLock (cd);
	    }
	} else
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    CurrentDir (oldlock);
	}
    } else
    {		   /* no req.library? Gulp */
	if (AslBase)
	    error ("reqload:\nNo ReqTools.Library\nTry ARPLOAD");
	else
	    error ("For filerequester,\nI need REQ- or ASL.LIBRARY !");
    }
} /* do_reqload */


Prototype int reqsave (void);

int reqsave (void)
{
    BPTR oldlock = CurrentDir(Ep->dirlock);

    splitpath (Ep->name, file, dir);

    if (!stricmp (file, "unnamed"))
	*file = 0;

    fixdirname (dir);

    if (ReqToolsBase)
    {		     /* If we have req.library & requester */
	if (!RFReq)
	    RFReq = rtAllocRequestA (RT_FILEREQ, NULL);

	if (!RFReq)
	{
	    nomemory ();
	    return (FALSE);
	}

	rtChangeReqAttr (RFReq, RTFI_Dir, dir, TAG_DONE);

loop:
	if (rtFileRequest (RFReq, file, "XDME Save File",
		RTFI_OkText,	"Save",
		RT_Window,	Ep->win,
		RTFI_Flags,	FREQF_SAVE,
		RT_ReqPos,	REQPOS_TOPLEFTSCR,
		RT_LeftOffset,	Ep->config.aslleft,
		RT_TopOffset,	Ep->config.asltop,
		RTFI_Height,	Ep->config.aslheight,
		TAG_DONE ))
	{
	    BPTR tmp_lock;

	    if (!stricmp (file, "unnamed"))
	    {
		error ("arpsave:\nCannot save file as\n`unnamed'");
		goto loop;
	    }

	    CurrentDir (oldlock);

	    strcpy (dir, RFReq->Dir);
	    AddPart (dir, file, PATHSIZE);

	    if (!*file)
	    {
		error ("%s:\nYou must specify a name", av[0]);
		goto loop;
	    }

	    av[1] = (UBYTE *)dir;

	    if ((tmp_lock = Lock (dir, ACCESS_READ)))
	    {
		UnLock (tmp_lock);

		if (!getyn ("XDME Warning",
			"The file\n`%s'\ndoes already exist."
			"Do you want to overwrite it ?",
			"Yes|No", dir) )
		    return (FALSE);
	    }

	    return (do_saveas ());
	} else
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    CurrentDir (oldlock);
	}
    } else
    {
	if (AslBase)
	    error ("reqsave:\nNo ReqTools.Library\nTry ARPSAVE");
	else
	    error ("For filerequester,\nI need REQ- or ASL.LIBRARY !");
    }

    return (FALSE);
} /* reqsave */


/*DEFHELP requester,io REQINSFILE - INSFILE with ReqTools filerequester */

DEFUSERCMD("reqinsfile", 0, 0, void, do_reqinsfile, (void),)
{
    BPTR oldlock = CurrentDir (Ep->dirlock);

    splitpath (Ep->name, file, dir);

    if (!stricmp (file, "unnamed"))
	*file = 0;

    fixdirname (dir);

    if (ReqToolsBase)
    {		     /* If we have req.library & requester */
	if (!RFReq)
	    RFReq = rtAllocRequestA (RT_FILEREQ, NULL);

	if (!RFReq)
	{
	    nomemory ();
	    return;
	}

	rtChangeReqAttr (RFReq, RTFI_Dir, dir, TAG_DONE);

	if (rtFileRequest (RFReq, file, "XDME Insert File",
		RTFI_OkText,	"Load",
		RT_ReqPos,	REQPOS_TOPLEFTSCR,
		RT_Window,	Ep->win,
		RT_LeftOffset,	Ep->config.aslleft,
		RT_TopOffset,	Ep->config.asltop,
		RTFI_Height,	Ep->config.aslheight,
		TAG_DONE ))
	{

	    CurrentDir (oldlock);

	    strcpy (dir, RFReq->Dir);
	    AddPart (dir, file, PATHSIZE);

	    av[0] = (UBYTE *)"i";
	    av[1] = (UBYTE *)file;

	    do_edit ();
	} else
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    CurrentDir (oldlock);
	}
    } else
    {
	if (AslBase)
	    error ("reqinsfile:\nNo ReqTools.Library\nTry ARPINSFILE");
	else
	    error ("For filerequester,\nI need REQ- or ASL.LIBRARY !");
    }
} /* do_reqinsfile */


/*DEFHELP requester,io REQFONT - SETFONT with ReqTools fontrequester */

DEFUSERCMD("reqfont", 0, CF_VWM, void, do_reqfont, (void),)
{
    if (ReqToolsBase)
    {
	struct rtFontRequester *fontreq;

#ifdef _DCC
	struct Window *fwin = Ep->win;	/* put ASL on correct screen */
#endif

	struct TagItem FontTags[] =
	{   /* DICE 2.06.37 allows us to initialize this here... */
#ifdef _DCC
	    RT_Window,	    fwin,
#else
	    RT_Window,	    NULL,
#endif
	    RTFO_MinHeight, 6,
	    RTFO_MaxHeight, 24,
	    RTFO_Flags,     FREQF_FIXEDWIDTH, /* | FILF_NEWIDCMP */
	    TAG_DONE };

#ifndef _DCC
	    FontTags[0].ti_Data = (ULONG)Ep->win;
#endif

	if ((fontreq = rtAllocRequestA (RT_FONTREQ, NULL)))
	{
	    if (rtFontRequestA (fontreq, "XDME fixed width font", FontTags))
	    {

		/*  Since we have GetFont() already, just pass
		 *   the name and size from fontreq's TextAttr to it.
		 *
		 *  What follows mimics SETFONT (in cmnd3.c)
		 */

		FONT * font = (FONT *)GetFont (fontreq->Attr.ta_Name,
					     (short) fontreq->Attr.ta_YSize );
		ED   * ep   = Ep;

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
		} else
		{
		    error ("aslfont:\nUnable to find font\n%s/%ld", fontreq->Attr.ta_Name,
			   fontreq->Attr.ta_YSize);
		}
	    }
	    else
		SETF_ABORTCOMMAND(Ep,1);

	    rtFreeRequest (fontreq);
	} else
	{
	    nomemory ();
	}
    } else
    {
	if (AslBase)
	    error ("reqfont:\nNo ReqTools.Library\nTry ARPFONT");
	else
	    error ("Need ASL.LIBRARY or\nREQTOOLS.LIBRARY for\nfont requester !");
    }
} /* do_reqfont */


/******************************************************************************
*****  ENDE filereq.c
******************************************************************************/
