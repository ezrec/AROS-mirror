/******************************************************************************

    MODUL
	$Id$

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include <dos/dostags.h>
#include <proto/layers.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/reqtools.h>
#include <proto/asl.h>
#include <proto/icon.h>

#define MYDEBUG     1
#include "debug.h"
#define DEBUG
//#define DETACH


/**************************************
	    Globale Variable
**************************************/
Prototype struct MsgPort  * Sharedport;
Prototype char		    MShowTitle;
Prototype char		    noadj;
Prototype struct GfxBase  * GfxBase;
Prototype struct __XDMEArgs XDMEArgs;

struct MsgPort * Sharedport;	    /* Port for all Windows */
char		 MShowTitle;	    /* Should DME refresh the title ? */
char		 noadj; 	    /* for blocking software adj. of prop gad
				       when intuition (user) already did it */

extern struct IntuitionBase * IntuitionBase;
extern struct GfxBase	    * GfxBase;
extern struct Library	    * IconBase;
extern struct Library	    * AslBase;
extern struct Library	    * LayersBase;

#ifndef __AROS__
extern long __stack = 16000;
#endif

WORD	  Mx, My;	     /* Mouse-Coords */


/**************************************
	       Typedefs
**************************************/
typedef struct WBStartup    WBS;
typedef struct DiskObject   DISKOBJ;


/**************************************
	    Interne Variable
**************************************/
static WORD	 oMx, oMy;	    /* starting coords */
static ULONG	 newxdme;	    /* force new xdme */

static const char default_startupfile[] = ".XDME.edrc";
static const char default_projectfile[] = "XDME_Project_File";

static const char tmplate[] = "Files/M,"
			      "Startup=-S/K,"
			      "New=-N/S,"
			      "Iconify=-I/S,"
			      "Autoload=-A/S,"
			      "PubScreen=-PS/K,"
			      "ProjectFile=-PF/K";

struct __XDMEArgs XDMEArgs =
{
    NULL,	    /* Files */
    NULL,	    /* Startup-Filename */
    FALSE,	    /* NEW */
    FALSE,	    /* Iconify */
    FALSE,	    /* Autoload */
    NULL,	    /* PublicScreen */
    NULL,	    /* ProjectFile */
    /* ... */
};


static const struct Config startup_config =
{
    -1, 	/* wwcol */

    0, 0,	/* winx, winy	       */
    640, 200,	/* winwidth, winheight */

    0, 11,	/* iwinx, iwiny */

    0, 0,	/* aslleft/top	   */
    400, 200,	/* aslwidth/height */

    4,		/* tabstop */
    75, 	/* margin  */

    1,		/* fgpen */
    0,		/* bgpen */
    2,		/* hgpen */
    1,		/* bbpen */
    1,		/* tfpen */
    0,		/* tbpen */

    1,		/* insertmode	    */
    0,		/* ignorecase	    */
    0,		/* wordwrap	    */
    0,		/* autosplit	    */
    1,		/* autoindent	    */
    1,		/* autounblock	    */
    0,		/* doback	    */
    0,		/* nicepaging	    */
    0,		/* iconactive	    */
    0,		/* blockendsfloat   */
};


/**************************************
	   Interne Prototypes
**************************************/
#ifndef __AROS__
extern void __regargs __chkabort (void);

/* disable ^C checking */
void __regargs __chkabort (void) { /* empty */ }
#endif


#ifdef UNUSED
Prototype void PrintCWD (void);

void PrintCWD (void)
{
    static char puffer[256];
    PROC * proc = (PROC *)FindTask (NULL);

    NameFromLock (proc->pr_CurrentDir, puffer, sizeof(puffer));

    kprintf ("CWD: `%s'\n", puffer);
}
#endif


extern int wbmain (WBS * wbs); /* !!! No Prototype !!! */

int wbmain (WBS * wbs)
{
    return (main (0, (char **)wbs));
} /* wbmain */



Prototype int main (int mac, char ** mav);

int main (int mac, char ** mav)
{
    ULONG  nf;		    /* # files on command line	   */
    BOOL   iawm        = 0; /* overide mouse buttons	   */
    BOOL   dontwait    = 0; /* don't wait for a message    */
    BOOL   LeftArea    = 0; /* has mouse after buttonpress moved a certain
			       distance */
    WORD   i;		    /* temp. counter		   */
    WORD   Code;	    /* IDCMP->Code		   */
    ULONG  Qualifier;	    /* IDCMP->Qualifier 	   */
    PROC * proc        = (PROC *)FindTask(NULL);   /* our Task    */
    BPTR   origlock;	    /* Start-Lock		   */
    ULONG  wait_ret;	    /* HD Result of Wait	   */
    ULONG  Mask        = 0; /* Mask for Wait()             */
    BOOL   gad_active  = 0; /* Is any gadget active ?	   */
    UWORD  gad_id;	    /* Which one ?		   */
    WORD   mqual;	    /* Qualifiers for mmove	   */
    BOOL   mmove       = 0; /* Mousemove		   */
    int    show_cursor;     /* shall I redraw the cursor ? */

#if MYDEBUG
    D(bug("------------- Hallo, hier ist XDME ----------------\n"));
#endif

#if 0
#ifdef DETACH
    /* Detach from CLI */
    fclose (stdin);
    fclose (stdout);        /*  debugging output needs kprintf ! */
    fclose (stderr);        /*  close stderr & console ref. */
#endif
#endif

    /* cd to dir of shell */
    origlock = CurrentDir (DupLock (proc->pr_CurrentDir));

    /* Init lists */
    NewList ((LIST *)&DBase);
    NewList ((LIST *)&PBase);
    init_macros ();
    init_variables ();
    path_init ();
    init_kb ();

    /* IntuitionBase = (struct IntuitionBase *)OpenLibrary ("intuition.library", 37L);
    GfxBase	  = (struct GfxBase       *)OpenLibrary ("graphics.library",  37L);
    if (!IntuitionBase || !GfxBase)
	exiterr ("cannot open intuition or graphics library");

    AslBase = OpenLibrary ("asl.library", 37L); */
    ReqToolsBase = (struct ReqToolsBase *)OpenLibrary ("reqtools.library", 37);

    PageJump = 80;	    /* 80% of page for Propgad & pageup/down */

    String = NULL;	    /*	initialize scanf variable   */

    unblock ();             /* No block */

    /* making a default-ed-config */
    movmem (&startup_config, &default_config, CONFIG_SIZE);

    /* setting default flags */
    SETF_SAVEICONS(Ep,0);
    SETF_ACTIVATETOFRONT(Ep,1);
    SETF_WINDOWCYCLING(Ep,1);
    SETF_SHOWTITLE(Ep,1);

    if (mac == 0)
    {		  /*  WORKBENCH STARTUP   */
	SETF_SAVEICONS(Ep,1);           /*  allow icon save     */
	Wbs = (WBS *)mav;

	/* if (!(IconBase = OpenLibrary ("icon.library", 0)) )
	    exiterr ("unable to open icon library"); */
    }

    /* Init AREXX */
    mountrequest (0);
    openrexx ();     /*   do this after the last possible call to exiterr() */
    mountrequest (1);

    /* Init Keyboard */
    new_menustrip ("default",1);        /* PATCH_NULL [13 Sep 1994] : instead of init_structures(); */
    new_keytable  ("default",1);        /* PATCH_NULL [13 Sep 1994] : instead of init_structures(); */

    /* Init CommandShell */ /* PATCH_NULL [13 Sep 1994] : added */
    CMDSH_Initialize ();
    Mask |= CMDSH_SigMask;

    /* Init AppIcon */ /* PATCH_NULL [21 Sep 1994] : added */
    APIC_Initialize ();
    Mask |= APIC_SigMask;

    {
	struct MsgPort * mp;

	Forbid ();
	mp = FindPort ("XDME.1");
	Permit ();

	newxdme = (!strcmp (RexxPortName, "XDME.1") || !mp);
    }

    nf = 0;
    /* Here we have to set the names BEFORE the parsing since none of the
       Arg-Parsers does set them (we make copies later) */
    XDMEArgs.projectfilename = default_projectfile; /* no strdup() here ! */
    XDMEArgs.startupfile     = default_startupfile; /* no strdup() here ! */

    /* WB-Startup or CLI ? */
    if (Wbs)
    {
	/* Work on TOOLTypes */
	if (Wbs->sm_ArgList[0].wa_Lock)
	{
	    DISKOBJ	 * dobj;	/* For WB-Startup */
	    struct WBArg * wbarg;
	    BPTR	   current;

	    for (i=0; i<Wbs->sm_NumArgs; i++)
	    {
		wbarg = &Wbs->sm_ArgList[i];

		UnLock (CurrentDir (current = DupLock (wbarg->wa_Lock)));

		if (dobj = GetDiskObjectNew (wbarg->wa_Name)) /* PATCH_NULL 21-08-94 added "New" */
		{
		    /* Only process names for ToolTypes, not for the the
		       program itself ! */

		    ops (dobj->do_ToolTypes);

		    if (i)
		    {
			if (loadfile (wbarg->wa_Lock, wbarg->wa_Name))
			    nf ++;
			else
			{
			    FreeDiskObject (dobj);
			    break;
			}
		    }

		    FreeDiskObject (dobj);
		}
	    }

	    /* CD to new base-dir */
	    UnLock (CurrentDir (DupLock (Wbs->sm_ArgList[0].wa_Lock)));
	}
    }
    else
    {
	XDMEArgs.newxdme = newxdme;

	/* Parse CLI-args */
	if ((XDMEArgs.ra = ReadArgs (tmplate, (LONG *)&XDMEArgs, NULL))
		&& XDMEArgs.files)
	{
#ifdef DEBUG
	    UWORD count = 0;
#endif
	    char ** nameptr;

	    for (nameptr=XDMEArgs.files; *nameptr; nameptr ++)
	    {
#ifdef DEBUG
		D(bug("Loading %ld %s ...\n", count ++, *nameptr));
#endif

		if (loadfile (origlock, *nameptr))
		    nf ++;
		else
		    break;
	    }
	}
    }

#ifdef DEBUG
    D(bug("Parsed ARGS\n"));
#endif

    if (XDMEArgs.startupfile)
	XDMEArgs.startupfile = strdup (XDMEArgs.startupfile);
    else
    {
#ifdef DEBUG
	D(bug("ERROR: XDMEArgs.startupfile == NULL <<<------------\n"));
#endif
	XDMEArgs.startupfile = strdup (default_startupfile);
    }

#ifdef DEBUG
    D(bug("Startup-file : \"%s\"\n", XDMEArgs.startupfile));
#endif

    if (XDMEArgs.projectfilename)
	XDMEArgs.projectfilename = strdup (XDMEArgs.projectfilename);
    else
    {
#ifdef DEBUG
	D(bug("ERROR: XDMEArgs.projectfilename == NULL <<<------------\n"));
#endif
	XDMEArgs.projectfilename = strdup (default_projectfile);
    }

#ifdef DEBUG
    D(bug("Projectfile : \"%s\"\n", XDMEArgs.projectfilename));
#endif

    if (XDMEArgs.publicscreenname)
	XDMEArgs.publicscreenname = strdup (XDMEArgs.publicscreenname);

#ifdef DEBUG
    D(bug("Screen : \"%s\"\n", XDMEArgs.publicscreenname ?
		XDMEArgs.publicscreenname : "WBench"));
#endif

    /* Free args */
    if (XDMEArgs.ra)
	FreeArgs (XDMEArgs.ra);

    if (!nf)
    {  /* no files to edit: Open simple empty window */
	if (XDMEArgs.newxdme || newxdme)
	{
	    do_newwindow ();
	    if (!Ep)
		goto quit_dme;

	    if (XDMEArgs.iconify)
	    {
		do_iconify ();
	    }
	}
	else
	{
	    if (XDMEArgs.iconify)
	    {
		do_rexx ("XDME.1", "newwindow iconify");
	    } else
	    {
		do_rexx ("XDME.1", "newwindow");
	    }
	}
    } /* No files */

    if (!XDMEArgs.newxdme) /* All done, quit */
	goto quit_dme;

    /* Read main .edrc file */
DL;
    mountrequest (0);
    av[0] = "source";
    av[1] = (UBYTE *)"S:XDME.edrc";
DL;
    do_source (TRUE);   /* Display error */
DL;

    /* Read other file (if possible) */
    if (NameFromLock (origlock, tmp_buffer, sizeof(tmp_buffer)))
    {
DL;
	AddPart (tmp_buffer, XDMEArgs.startupfile, sizeof(tmp_buffer));
	av[1] = tmp_buffer;
DL;
	do_source (FALSE); /* Not */
    }
DL;
    mountrequest (1);
DL;

    /* Initialize certain fields in structure */
    {
	ED * ep;
	ED * eb = GetHead ((LIST *)&DBase);
	UWORD ix, iy;

	for (ep = eb; ep = GetSucc(&ep->node); )
	{
	    ix = ep->beginconfig.iwinx;
	    iy = ep->beginconfig.iwiny;

	    movmem (&eb->beginconfig, &ep->beginconfig, CONFIG_SIZE);

	    ep->beginconfig.iwinx = ix;
	    ep->beginconfig.iwiny = iy;

	    if (eb->font)
	    {
		ep->font = eb->font;
		eb->font->tf_Accessors ++;
	    } /* if font */
	} /* for ed */
    }

    /* Get Wait()-Mask */
    Mask |= 1 << Sharedport->mp_SigBit;
    wait_ret = 0;
DL;

loop:
    if (!GETF_ICONMODE(Ep))  /* Display cursor ? */
	text_cursor (1);
DL;

    for ( ; !GETF_QUITFLAG(Ep); )
    {
	if (!GETF_ICONMODE(Ep))
	    window_title ();

	if (dontwait)
	{
	    dontwait --;
	} else if (!wait_ret)
	{
	    /* Wait for Window, AREXX or ^C */
	    wait_ret = Wait (Mask | RexxMask | SIGBREAKF_CTRL_C);
	}

	show_cursor = 0;    /* don't update cursor */

	/*
	 *  NOTE: due to operation of breakcheck(), the userport signal
	 *  may not be set even if there are messages pending.
	 */
	if (wait_ret & CMDSH_SigMask)   /* Message from CommandShell */ /* PATCH_NULL [13 Sep 1994] : added */
	{
	   text_cursor (0);
	   show_cursor ++;

	   CMDSH_Control();

	   wait_ret &= ~CMDSH_SigMask;
	}
	/* else */if (wait_ret & APIC_SigMask)   /* Message from AppIcon */ /* PATCH_NULL [21 Sep 1994] : added */
	{
	   text_cursor (0);
	   show_cursor ++;

	   APIC_Control();

	   wait_ret &= ~APIC_SigMask;
	}
	if (wait_ret & RexxMask)          /* HD RexxMessage */
	{
	    text_cursor (0);

	    extern_rexx_command ();       /* process HD rexxcommand */

	    wait_ret &= ~RexxMask;
	    show_cursor ++;
	}
	else if (wait_ret & SIGBREAKF_CTRL_C)
	{
	    SETF_QUITALL(Ep,1);
	    SETF_QUITFLAG(Ep,1);

	    wait_ret &= ~SIGBREAKF_CTRL_C;

	    goto boom;
	}
	else if (wait_ret & Mask)              /* HD IntuiMessage */
	{
	    IMESS * im;

	    wait_ret &= ~Mask;

	    while (im = (IMESS *)GetMsg(Ep->win->UserPort))
	    {
		SETF_ABORTCOMMAND(Ep,0);
		SETF_MSGCHECK(Ep,1);

		Code	     = im->Code;
		Qualifier    = im->Qualifier;

		clearbreaks ();

		if (im->IDCMPWindow != Ep->win)
		{
		    SETF_OVERIDE(Ep,0);

		    if (GETF_COMLINEMODE(Ep))
			escapecomlinemode ();

		    text_sync ();

		    MShowTitle = 0;

		    if (!GETF_ICONMODE(Ep))
			window_title ();

		    if (text_switch (im->IDCMPWindow) == 0)
		    {
			ReplyMsg ((MSG *)im);
			continue;
		    }
		}

		Mx = im->MouseX;
		My = im->MouseY;

		switch(im->Class)
		{
		    case IDCMP_NEWSIZE: {
			struct Window * win = Ep->win;
			WORD		wid;
			WORD		hei;

			/* Only works if uniconified */
			if (!GETF_ICONMODE(Ep))
			{
			    /* clear damage list */
			    BeginRefresh (Ep->win);
			    EndRefresh (Ep->win, TRUE);

			    /* Leave Comlinemode if neccessary */
			    if (GETF_COMLINEMODE(Ep))
				escapecomlinemode ();

			    /* Set new window-parameters */
			    set_window_params ();

			    /* Erase if neccessary */
			    wid = win->Width - win->BorderRight - 1;
			    hei = win->Height - win->BorderBottom - 1;

			    SetAPen (win->RPort, TEXT_BPEN(Ep));
			    SetWrMsk (win->RPort, BLOCK_MASK(Ep));

			    if (wid > Xpixs)
			    {
				RectFill (win->RPort, Xpixs+1, Ybase,
				    wid, hei);
			    }

			    if (hei > Ypixs)
			    {
				RectFill (win->RPort, Xbase, Ypixs+1,
				    Xpixs, hei);
			    }

			    SetWrMsk (win->RPort, ALL_MASK);

			    /* Update Text */
			    text_adjust (TRUE);
			    show_cursor ++;
			}
		    break; } /* NEWSIZE */

		    case IDCMP_REFRESHWINDOW:
			if (!GETF_ICONMODE(Ep))
			    OptimizedRefresh (Ep);

		    break; /* REFRESHWINDOW */

		    case IDCMP_MOUSEBUTTONS: {
			static WORD   lastCode = 0;
			static ULONG  lastSeconds;
			static ULONG  lastMicros;
			ULONG	      Seconds;
			ULONG	      Micros;
			WORD	      movement;

			/* get time of message */
			Seconds = im->Seconds;
			Micros = im->Micros;

			/* Avoid Crashing if Window is closed during keyctl() */
			ReplyMsg ((struct Message *)im);
			im = NULL;

			switch(Code)
			{
			    case MENUDOWN:
				if (GETF_ICONMODE(Ep))
				    break;

			    case MIDDLEDOWN:
			    case SELECTDOWN:
				/* if we only activated the window with a
				   buttonpress, ignore it */
				if (iawm)
				{
				    iawm = FALSE;
				    break;
				}

				/* If User moved the mouse "too much", this
				   cannot be called "dblclick" ;-) */
				{	/* PATCH_NULL 22-08-94 added */
				    WORD mx = Mx - oMx;
				    WORD my = My - oMy;

				    mx = mx >= 0 ? mx: -mx;
				    my = my >= 0 ? my: -my;
				    movement = mx > my? mx :my;
				}	/* PATCH_NULL 22-08-94 added */

				/* if there was no click yet or
				   the button changed or this is no
				   double-click anymore, then clear
				   the number of clicks and use the new code */
				if (!NumClicks          ||
				    lastCode != Code	||
				    movement > 4	|| /* PATCH_NULL 22-08-94 added */
				    !DoubleClick (lastSeconds, lastMicros,
						Seconds, Micros))
				{
				    lastCode	= Code;   /* new code */
				    NumClicks	= 0;	  /* button changed */
				}

				/* remember the time of this click */
				lastSeconds = Seconds;
				lastMicros  = Micros;
				/* increase the number of clicks */
				NumClicks ++;

				ReportMouse (TRUE, Ep->win);
				ModifyIDCMP (Ep->win, Ep->win->IDCMPFlags |
					IDCMP_INTUITICKS);

				oMx = Mx;
				oMy = My;
				LeftArea = FALSE;
			    break;

			    case SELECTUP:
			    case MENUUP:
			    case MIDDLEUP:
				ReportMouse (FALSE, Ep->win);
				ModifyIDCMP (Ep->win, Ep->win->IDCMPFlags &
					~IDCMP_INTUITICKS);
			    break;

			}

			if (Code == MENUDOWN && GETF_ICONMODE(Ep))
			{
			    uniconify ();
			    show_cursor ++;
			} else if (!iawm)
			{
#ifdef NOT_DEF /* PATCH_NULL [15 Feb 1993] : added */
			    if (MacroRecord)
			    {
				sprintf (tmp_buffer, "setptr %d %d", Mx, My);
				add_record (tmp_buffer);
			    } /* if */
#endif
			    text_cursor (0);
			    show_cursor ++;
#if 1 /* PATCH_NULL 03-07-94 */
    /* WARNING - THESE VALUES MIGHT INTERFERE WITH OTHER KEYS */
			    /* COMMENT: 							 */
			    /* Wir sollten noch eine Ueberpruefung der Mausposition einbauen und */
			    /* (unter Beruecksichtigung eines Schwellwertes) bei Mausbewegungen  */
			    /* NumClicks auf 0 resetten, das sollte jedoch jemand machen, der	 */
			    /* sich in main auskennt, nicht ich ;-)				 */
			    /* dasselbe ist zu den Qualifiern zu sagen, wenn die sich aendern,	 */
			    /* waere es vielleicht geschickter, NumClicks wieder auf 0 zurueck-  */
			    /* zusetzten							 */
			    /* noch besser ware es natuerlich, dieses Verhalten ueber FLAGS zu	 */
			    /* regeln, damit alle Seiten zufrieden sind 8-)			 */
			    /* oder soll diese Funktionalitaet nach keyctl verlagert werden? ;-) */
			    /* (just my $0.02)                                                   */
			    if ((NumClicks > 1) && (NumClicks < 5))
			    {
				switch (Code)
				{
				case SELECTDOWN: Code = (2 - NumClicks) + 0x67; break;
				case MIDDLEDOWN: Code = (2 - NumClicks) + 0x64; break;
				case MENUDOWN:	 Code = (2 - NumClicks) + 0x61; break;
	/* WARNING - THESE VALUES MIGHT INTERFERE WITH OTHER KEYS */
				default: ;
				} /* switch */
			    }
			    else if ((NumClicks > 1) && !(Code & 0x80))
				NumClicks = 0;
#endif /* PATCH_NULL 03-07-94 */

			    keyctl (NULL, Code, Qualifier);
			} else
			    iawm = FALSE;
		    break; } /* IDCMP_MOUSEBUTTONS */

		    case IDCMP_RAWKEY: {
			IMESS	copy;
			ULONG	prevkeys;

			/* Keybuttons stop Click-sequences */
			NumClicks = 0;
			/* Should we add that "NoMoreIntuiticks" here, too? */

			/* Prevent crashing in do_iconify()->
			   closesharedwindow */

			copy = *im;
			if (im->IAddress)
			    prevkeys = *(ULONG*)im->IAddress;
			else
			    prevkeys = 0;
			copy.IAddress = &prevkeys;

			ReplyMsg ((struct Message *)im);
			im = NULL;

			text_cursor (0);
			show_cursor ++;
			keyctl (&copy, Code, Qualifier);
		    break; } /* IDCMP_RAWKEY */

		    case IDCMP_MENUPICK: {
			IMESS copy;

			/* Menues stop Click-sequences */
			NumClicks = 0;
			/* Should we add that "NoMoreIntuiticks" here, too? */

			/* Prevent crashing in do_iconify()-> closesharedwindow */
			copy = *im;
			ReplyMsg ((struct Message *)im);
			im = NULL;

			text_cursor (0);
			show_cursor ++;
			menu_cmd (&copy);
			/* PATCH_NULL [02 Apr 1993] : menucontrol like keycontrol <<< */
		    break; } /* IDCMP_MENUPICK */

		    case IDCMP_CLOSEWINDOW:
			if (GETF_COMLINEMODE(Ep))
			    escapecomlinemode ();

			text_sync ();
			SETF_QUITFLAG(Ep,TRUE);
			show_cursor = 0;
		    break; /* IDCMP_CLOSEWINDOW */

		    case IDCMP_ACTIVEWINDOW:
			if (!GETF_ICONMODE(Ep))
			    iawm = TRUE;
		    break; /* IDCMP_ACTIVEWINDOW */

		    case IDCMP_INACTIVEWINDOW:		/* PATCH_NULL 15-08-94 ADDED */
			SetWindowTitles (Ep->win, Ep->wtitle, (UBYTE *)-1);

			/* Another Window starts w/ 0 Clicks */
			NumClicks = 0;					/* PATCH_NULL 22-08-94 added */

			/* "NoMoreIntuiticks" */
			/* I think that it the better way to make sure no other
			   window catches any input ... and it is called less
			   frequently  */
			if (Ep->win->IDCMPFlags & IDCMP_INTUITICKS) {   /* PATCH_NULL 22-08-94 added */
			    ReportMouse (FALSE, Ep->win);               /* PATCH_NULL 22-08-94 added */
			    ModifyIDCMP (Ep->win, Ep->win->IDCMPFlags & /* PATCH_NULL 22-08-94 added */
						    ~IDCMP_INTUITICKS); /* PATCH_NULL 22-08-94 added */
			} /* if */					/* PATCH_NULL 22-08-94 added */
		    break; /* IDCMP_INACTIVEWINDOW */	/* PATCH_NULL 15-08-94 ADDED */

		    case IDCMP_MOUSEMOVE:
			if (gad_active)
			{
			    int newtop = new_top ();

			    if (newtop != Ep->topline)
			    {
				noadj = 1;  /* don't touch the damn prop-gadget ! */
				text_sync ();

				newtop -= Ep->topline;

				text_cursor (0); /* Cursor off */
				show_cursor ++;

				Ep->line    += newtop;
				Ep->topline += newtop;

				text_load ();

				scroll_display (0, newtop, 0, Ep->topline,
					MAXLINELEN, Ep->topline + Lines);
			    }
			}
			else
			{
			    /* Lost ButtonUP ? */
			    if (!(Qualifier & ( IEQUALIFIER_MIDBUTTON |
						IEQUALIFIER_RBUTTON |
						IEQUALIFIER_LEFTBUTTON )) )
			    {
				ReportMouse (FALSE, Ep->win);
				ModifyIDCMP (Ep->win, Ep->win->IDCMPFlags &
					~IDCMP_INTUITICKS);
				break;
			    }

			    /* Process these after all messages have been
			       processed */
			    if (!LeftArea)
			    {
				if (((Mx > oMx) ? Mx-oMx>4 : oMx-Mx>4) ||
					((My > oMy) ? My-oMy>4 : oMy-My>4) )
				    LeftArea = TRUE;
			    }

			    if (LeftArea)
			    {
				mmove = 1;
				mqual = Qualifier;
			    }
			}
		    break; /* IDCMP_MOUSEMOVE */

		    case IDCMP_GADGETDOWN:
			if (GETF_COMLINEMODE(Ep))
			    escapecomlinemode ();

			gad_id = ((struct Gadget *)im->IAddress)->GadgetID;

			if (!gad_id)
			{
			    noadj = 1;	  /* block software prop refresh */
			} else
			{
			    text_cursor (0);
			    show_cursor ++;

			    if (gad_id == 1)
				do_scrollup ();
			    else
				do_scrolldown ();
			}

			/* up&down */
			if (gad_id == 1 || gad_id == 2)
			    ModifyIDCMP (Ep->win, Ep->win->IDCMPFlags |
					IDCMP_INTUITICKS);
			else if (gad_id == 0)  /* scroller */
			    text_adjust (FALSE);

			gad_active = TRUE;
		    break;

		    case IDCMP_GADGETUP: {
			gad_id = ((struct Gadget *)im->IAddress)->GadgetID;

			switch (gad_id)
			{
			case 0: { /* scroller */
			    int newtop = new_top ();

			    if (newtop != Ep->topline)
			    { /* Make sure we don't miss a jump */
				noadj = 1;  /* don't touch the damn prop-gadget ! */

				text_sync ();

				newtop -= Ep->topline;

				text_cursor (0); /* Cursor off */
				show_cursor ++;

				Ep->line    += newtop;
				Ep->topline += newtop;

				text_load ();

				ScrollAndUpdate (0, newtop);
			    }
			    break; }

			case 4: /* zoom */
			    if (!GETF_ICONMODE(Ep))
			    {
				ReplyMsg ((struct Message *)im);
				im = NULL;

				iconify ();
			    }

			    break;

			default:
			    ModifyIDCMP (Ep->win, Ep->win->IDCMPFlags &
					~IDCMP_INTUITICKS);
			}

			gad_active = FALSE;

			noadj = 0;		/* reallow software prop refresh */
		    break; }

		    case IDCMP_INTUITICKS:
			if (gad_active && gad_id)
			{
			    text_cursor (0);
			    show_cursor ++;

			    if (gad_id == 1)
				do_scrollup ();
			    else
				do_scrolldown ();
			} else if (LeftArea)
			{
			    mmove = 1;
			    mqual = Qualifier;
			}
		    break; /* IDCMP_INTUITICKS */
		} /* switch (class) */

		if (im)
		    ReplyMsg ((MSG *)im);

		if (GETF_QUITFLAG(Ep))
		{
		    dontwait = 2;
		    goto boom;
		} /* if (Quitflag) */
	    } /* While IntuiMessage */

	} /* if (Wait & Mask) */

	/* no more iawm */
	iawm = FALSE;

	if (mmove && !GETF_ICONMODE(Ep))
	{
	    mmove = 0;

#ifdef NOT_DEF		/* PATCH_NULL [15 Feb 1993] : added */
	    if (MacroRecord) {
		sprintf (tmp_buffer, "setptr %d %d", Mx, My);
		add_record (tmp_buffer);
	    } /* if */
#endif

	    text_cursor (0);
	    show_cursor ++;
	    keyctl (NULL, QMOVE, mqual);
	} /* if (mmove) */

	if (show_cursor && !GETF_ICONMODE(Ep))
	    text_cursor (1);

	UpdateEd (Ep);
    } /* for ( ; !Quitflag; ) */

boom:
    text_sync ();

    /* Close window(s) */
    {
	WIN	      * win;
	struct Gadget * gadget;
	int		result;

	do
	{
	    if (GETF_MODIFIED(Ep))
	    {
		uniconify ();
		text_cursor (1);

		result = getyn ("XDME Warning",
			"The file `%s' has been modified.\n"
			"Do you want to quit anyway (and loose all\n"
			"your work) ?",
			"Quit|Save & Quit|Cancel", Ep->name);

		switch (result)
		{
		    case 0:
			SETF_QUITFLAG(Ep,0);
			goto loop;
		    break;

		    case 2:
			if (!do_save ())
			{
			    SETF_QUITFLAG(Ep,0);
			    goto loop;
			}
		    break;
		}
	    }

	    win    = Ep->win;
	    gadget = Ep->propgad;

	    rem_prop (Ep);
	    text_uninit ();
	    closesharedwindow (win);

	    if (gadget)
		free_prop ((struct PropGadget *)gadget);
	} while (GETF_QUITALL(Ep) && Ep);
    }

    /* If we have another window left */
    if (Ep)
    {
	SETF_QUITFLAG(Ep,0);

	if (!GETF_ICONMODE(Ep))
	    set_window_params ();

	text_load ();

	MShowTitle = 0;
	goto loop;
    }

quit_dme:
    /* Close AREXX */
    closerexx ();

    /* Close Port after ARexx */
    if (Sharedport)
    {
	DeletePort (Sharedport);
	Sharedport = NULL;
    }

    /* Free Lock */
    UnLock (CurrentDir (origlock));

    /* Free memory */
    // dealloc_hash (currenthash());        /* PATCH_NULL [25 Jan 1993] : added currenthash */
    check_stack (NULL);

    if (XDMEArgs.startupfile)
	free (XDMEArgs.startupfile);

    if (XDMEArgs.projectfilename)
	free (XDMEArgs.projectfilename);

    if (XDMEArgs.publicscreenname)
	free (XDMEArgs.publicscreenname);

#ifdef DEBUG_MEM_H
    _debug_FreeAllMem ();
#endif /* DEBUG_MEM_H */

    clear_record ();
    exit_menustrips ();
    exit_keytables ();
    CMDSH_Terminate ();
    APIC_Terminate ();

    /* All OK. */
    exiterr (NULL);
} /* main */


Prototype void OptimizedRefresh (ED *);

void OptimizedRefresh (ED * ep)
{
    BOOL old;

    noadj = 1;	/* don't touch the damn prop-gadget ! */

    BeginRefresh (ep->win);
    LockLayer (NULL, ep->win->WLayer);
    /* text_redisplay ();   Must not be used because it calls RefreshGList() */

    set_window_params (); /* query current window parameters */

    /* leave Comlinemode */
    if (old = GETF_COMLINEMODE(Ep))
	escapecomlinemode ();

    /* clear background */
    SetAPen (ep->win->RPort, TEXT_BPEN(ep));
    SetWrMsk (ep->win->RPort, ALL_MASK);
    RectFill (ep->win->RPort, Xbase,Ybase, Xpixs, Ypixs);

    redraw_block (TRUE, ep->topline, ep->topcolumn,
	    ep->topline + Lines - 1, ep->topcolumn + Columns - 1);

    if (old)
	do_recall ();

    text_cursor (1);

    UnlockLayer (ep->win->WLayer);
    EndRefresh (ep->win, TRUE);

    noadj = 0;
} /* OptimizedRefresh */


/*DEFHELP #cmd movement TOMOUSE - moves cursor to mouse position */

DEFUSERCMD("tomouse", 0, CF_VWM, void, do_tomouse, (void),)
{
    WORD cx, cy;
    WORD minx, miny, maxx, maxy;

    if (Columns < 2 || Lines < 2)
	return;

    cx = (Mx-Xbase);
    if (cx >= 0)
    {
	if (cx < Xpixs)
	    cx /= Xsize;
	else
	    cx = Columns;
    } else
	cx = -1;

    cy = (My-Ybase);
    if (cy >= 0)
    {
	if (cy < Ypixs)
	    cy /= Ysize;
	else
	    cy = Lines;
    } else
	cy = -1;

    minx = 0;		    /* Scroll when MOUSE left window */
    miny = 0;
    maxx = Columns;
    maxy = Lines;

    if (cx < minx)
    { /* to the left */
	do_scroll (SCROLL_LEFT);

	if (cy < miny)
	{ /* Also up */
	    do_scrollup ();
	} else if (cy >= maxy)
	{ /* also down */
	    do_scrolldown ();
	}
    } else if (cx >= maxx)
    { /* dito to the right */
	do_scroll (SCROLL_RIGHT);

	if (cy < miny)
	{
	    do_scrollup ();
	} else if (cy >= maxy)
	{
	    do_scrolldown ();
	}
    } else
    { /* x is ok, check y */
	if (cy < miny)
	{ /* up */
	    do_scrollup ();
	} else if (cy >= maxy)
	{ /* down */
	    do_scrolldown ();
	} else
	    text_position (cx, cy); /* Just reposition cursor */
    }
} /* do_tomouse */


Prototype void exiterr (char * str);

void exiterr (char * str)
{
    if (str)
    {
	if (Output ())
	{
	    Write (Output (), str, strlen (str));
	    Write (Output (), "\n", 1);
	} else
	{
	    if (IntuitionBase)
		error (str);
	}
    }

    /* if (IconBase)
    {
	CloseLibrary (IconBase);
	IconBase = NULL;
    } */

    if (ReqToolsBase)
    {
	if (RFReq)
	{
	    rtFreeRequest (RFReq);

	    RFReq = NULL;
	}

	CloseLibrary ((struct Library *)ReqToolsBase);
	ReqToolsBase = NULL;
    }

    if (AslBase)
    {
	if (FReq)
	{
	    FreeAslRequest (FReq);

	    FReq = NULL;
	}

    /*	  CloseLibrary (AslBase);
	AslBase = NULL; */
    }

    /* if (GfxBase)
    {
	CloseLibrary ((struct Library *)GfxBase);
	GfxBase = NULL;
    }

    if (IntuitionBase)
    {
	CloseLibrary ((struct Library *)IntuitionBase);
	IntuitionBase = NULL;
    } */

    if (str)
	exit (1);

    exit (0);
} /* exiterr */


/*
 *  Check break by scanning pending messages in the Inputstream for a ^C.
 *  Msgchk forces a check, else the check is only made if the signal is
 *  set in the Inputstream (the signal is reset).
 */

Prototype int breakcheck (void);

int breakcheck (void)
{
    IMESS	* im;
    WIN 	* win  = Ep->win;
    struct List * list = &win->UserPort->mp_MsgList;

    ULONG sigs = SetSignal(0,SIGBREAKF_CTRL_C);                     /* PATCH_NULL [14 Feb 1993] : that mode slows looping not markably down ; PATCH_NULL 20-07-94 clearing ctl-c */

    if ((sigs & SIGBREAKF_CTRL_C) != 0) {                           /* PATCH_NULL [14 Feb 1993] : These lines are   */
	SETF_ABORTCOMMAND(Ep,1); /* PATCH_NULL 20-07-94 added */
	return(1);                                                  /* PATCH_NULL [14 Feb 1993] : intended to allow */
    }
    else if (GETF_MSGCHECK(Ep) || (sigs & (1<<win->UserPort->mp_SigBit)))    /* PATCH_NULL [14 Feb 1993] : external breaking */
    {
	SETF_MSGCHECK(Ep,0);
	SetSignal (0,1<<win->UserPort->mp_SigBit);

	im = (IMESS *)list->lh_Head;
	Forbid();

	for ( ; im != (IMESS *)&list->lh_Tail;
		im = (IMESS *)im->ExecMessage.mn_Node.ln_Succ)
		{
	    if (im->Class == IDCMP_RAWKEY && (im->Qualifier & 0xFB) == 0x08 &&
		im->Code == CtlC)
		{

		Permit ();
//		  SetSignal (SIGBREAKF_CTRL_C,SIGBREAKF_CTRL_C); /* PATCH_NULL 20-07-94 commented out */
		SETF_ABORTCOMMAND(Ep,1); /* PATCH_NULL 20-07-94 added */
		return (1);
	    }
	} /* for IMESS */

	Permit ();
    }

    return (0);
} /* breakcheck */


Prototype void breakreset (void);

void breakreset (void)
{
    SetSignal(0, SIGBREAKF_CTRL_C);
}


/* Parse WB-ToolTypes */

Prototype void ops (char ** toolarray);

void ops (char ** toolarray)
{
    char * tooltype;

    if (tooltype = FindToolType (toolarray, "ICONIFY"))
	XDMEArgs.iconify = MatchToolValue (tooltype, "YES");
    else
	XDMEArgs.iconify = FALSE;

    if (tooltype = FindToolType (toolarray, "NEW"))
	XDMEArgs.newxdme = MatchToolValue (tooltype, "YES") | newxdme;
    else
	XDMEArgs.newxdme = newxdme;

    if (tooltype = FindToolType (toolarray, "AUTOLOAD"))
	XDMEArgs.autoload = MatchToolValue (tooltype, "YES");
    else
	XDMEArgs.autoload = FALSE;

    if (tooltype = FindToolType (toolarray, "PUBSCREEN"))
	XDMEArgs.publicscreenname = tooltype;	/* no strdup() here ! */

    if (tooltype = FindToolType (toolarray, "PROJECTFILE"))
	XDMEArgs.projectfilename = tooltype;   /* no strdup() here ! */

    if (tooltype = FindToolType (toolarray, "STARTUP"))
	XDMEArgs.startupfile = tooltype;   /* no strdup() here ! */

#ifdef DEBUG
    D(bug("sfile: %s\nnew %ld\niconify %ld\nauto %ld\n"
	    "PSName: `%s'\npf: `%s'\n", XDMEArgs.startupfile,
	    XDMEArgs.newxdme, XDMEArgs.iconify, XDMEArgs.autoload,
	    XDMEArgs.publicscreenname ? XDMEArgs.publicscreenname : "(nul)",
	    XDMEArgs.projectfilename ? XDMEArgs.projectfilename : "(nul)"));
#endif
} /* ops */


Prototype BOOL loadfile (BPTR lock, char * name);

BOOL loadfile (BPTR lock, char * name)
{
    char buf[512];

    /* Load project ? */
    if (!stricmp (name, XDMEArgs.projectfilename))
    {
	if (XDMEArgs.newxdme)
	{
	    return ((BOOL) load_project ());
	} else
	{
	    strcpy (buf, "cd `");

	    if (NameFromLock (lock, buf + 4, sizeof (buf) - 4))
	    {
		strcat (buf, "' projectload");

		do_rexx ("XDME.1", buf);
	    }
	}
    } else
    {
	if (XDMEArgs.newxdme)
	{
	    /* Normal file. Open new window */
	    do_newwindow ();
	    if (!Ep)
		return (FALSE);

	    /* Load file for editing */
	    av[0] = (UBYTE *)"newfile";
	    av[1] = (UBYTE *)name;

	    UnLock (Ep->dirlock);
	    Ep->dirlock = DupLock (lock);

	    do_edit ();

	    /* Show title */
	    SETF_MFORCETITLE(Ep,1);
	    window_title ();

	    if (XDMEArgs.iconify)
		do_iconify ();
	} else
	{ /* Use AREXX */
	    strcpy (buf, "newwindow cd `");

	    if (NameFromLock (lock, buf + 14, sizeof (buf) - 14))
	    {
		sprintf (buf + strlen (buf), "' newfile `%s'%s",
		    name, (XDMEArgs.iconify ? " iconify" : ""));
		do_rexx ("XDME.1", buf);
	    }
	}
    } /* No PROJECTFILE */

    return (TRUE);
} /* loadfile */


/******************************************************************************
*****  ENDE main.c
******************************************************************************/
