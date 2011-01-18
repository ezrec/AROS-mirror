/*
 * $Id$
 *
 */

#include "rexx.h"
#include "defs.h"
#include <proto/dos.h>
#include <proto/icon.h>

typedef struct WBStartup    WBS;
typedef struct DiskObject   DISKOBJ;


/*DEFHELP #cmd io NEWFILE name - replace current text with new file */
/*DEFHELP #cmd textedit,io INSFILE name - insert a file into the current text. */

DEFUSERCMD("newfile", 1, CF_VWM, int, do_edit, (void),;)
DEFUSERCMD("insfile", 1,      0, int, do_edit, (void),)
{
    FILE *  fi;
    BPTR    oldlock;
    BPTR    new_lock;
    LONG    lines;
    UBYTE   buf[MAXLINELEN];
    LINE    ptr;
    BOOL    failed = 1;
    BOOL    newfile = (av[0][0] == 'n');
    ED	  * ep	   = Ep;

    text_sync ();

    if (newfile)
    {	/* newfile or insfile */
	if (GETF_MODIFIED(ep) && getyn("XDME Warning", "Delete modified text ?", "Yes|No") == 0)
	    return (-1);

	ep = uninit_init (ep);

	strncpy ((char *)ep->name, (char *)av[1], 63);
	ep->name[63] = 0;
	SETF_MODIFIED(ep,0);
    }
    else
    {
	SETF_MODIFIED(ep,1);
    }

    lines = ep->lines;
    oldlock = CurrentDir (ep->dirlock);

    if ( (fi = fopen (av[1], "r")) )
    {
	int  len;
	char oktitle  = 1;

	/* Get lock for file */
	if (newfile)
	{
	    if ( (new_lock = Lock (av[1], SHARED_LOCK)) )
	    {
		BOOL success;

		/* get full path */
		success = NameFromLock (new_lock, tmp_buffer,
							  sizeof (tmp_buffer));

		/* always unlock lock */
		UnLock (new_lock);

		if (success)
		{
		    strncpy (ep->name, FilePart (tmp_buffer),
							  sizeof (ep->name)-1);
		    ep->name[sizeof(ep->name)-1] = 0;

		    *(PathPart(tmp_buffer)) = 0;

		    new_lock = Lock (tmp_buffer, SHARED_LOCK);

		    UnLock (ep->dirlock);

		    ep->dirlock = new_lock;
		}
	    }
	}

	title ("Loading %s ...", ep->name);

	while ((len = xefgets(fi, (char *)buf, MAXLINELEN-1)) >= 0)
	{
	    failed = 0;
	    len ++;

	    if (makeroom(256) && (ptr = allocline(len)))
	    {
		SETLINE(ep,ep->lines,ptr);
		ep->lines ++;

		movmem(buf, ptr, len);
	    } else
	    {
		set_window_params ();
		nomemory ();
		oktitle = 0;
		break;
	    }
	}

	set_window_params ();

	if (oktitle)
	{
	    title ("Loading OK");
	    SETF_MFORCETITLE(ep,TRUE);
	}

	fclose (fi);
    } else /* couldn't open file */
    {
	if (!newfile) /* insertfile ? */
	{
	    warn ("File `%s' not found", av[1]);

	    cmderr = CMD_FAILED;
	} else
	{
	    /* copy argument */
	    strcpy (tmp_buffer, av[1]);

	    /* copy name of file */
	    strncpy (ep->name, FilePart (tmp_buffer), sizeof (ep->name)-1);
	    ep->name[sizeof(ep->name)-1] = 0;

	    /* clip name off */
	    *(PathPart (tmp_buffer)) = 0;

	    /* get new lock */
	    new_lock = Lock (tmp_buffer, SHARED_LOCK);

	    /* unlock old lock */
	    UnLock (ep->dirlock);

	    /* remember new lock */
	    ep->dirlock = new_lock;
	}
    }

    CurrentDir (oldlock);

    if (ep->lines != 1 && lines == 1 && *GETTEXT(ep,0) == 0)
    {
	ep->line     = 0;
	freeline (GETLINE(ep,0));
	bmovl (ep->list+1, ep->list, --ep->lines);
    } else
    {
	if (!failed && lines <= ep->lines - 1)
	{
	    Block block;

	    /* INSFILE: move text to correct position */
	    block.ep	     = ep;
	    block.start_line = lines;
	    block.end_line   = ep->lines - 1;
	    block.type	     = BT_LINE;
	    set_block_nud (&block);

	    do_bmove ();
	}
    }

    set_window_params ();
    window_title ();
    text_load ();
    text_redisplay ();

    return (!failed);
} /* do_edit */


static BOOL blockmode;

/*DEFHELP #cmd io APPENDSAVE file - Append current text to @{B}file@{UB} */

DEFUSERCMD("appendsave", 1, CF_VWM|CF_COK|CF_ICO, int, do_appendsave, (void),)
{
    return (saveit ("a"));
} /* do_appendsave */


/*DEFHELP #cmd io,block BAPPENDSAVE file - Append currently marked text to @{B}file@{UB} */

DEFUSERCMD("bappendsave", 1, CF_VWM|CF_COK|CF_ICO, int, do_bappendsave, (void),)
{
    int result = FALSE;

    if (block_ok ())
    {
	blockmode = 1;

	result = saveit ("a");

	blockmode = 0;
    } else
	error ("bappendsave:\nNo block specified");

    return (result);
} /* do_bappendsave */


/*DEFHELP #cmd block,io BSAVE file - save the block to @{B}file@{UB} */

DEFUSERCMD("bsave", 1, CF_VWM|CF_ICO, int, do_bsave, (void),)
{
    int result = FALSE;

    if (block_ok ())
    {
	blockmode = 1;

	result = saveit ("w");

	blockmode = 0;
    } else
	error ("bsave:\nNo block specified");

    return (result);
} /* do_bsave() */


/*DEFHELP #cmd block,requester,io BARPSAVE - save the block to a file with filerequester */

DEFUSERCMD("barpsave", 0, CF_VWM|CF_ICO, int, do_barpsave, (void),)
{
    int result = FALSE;

    if (block_ok ())
    {
	blockmode = 1;

	result = aslsave ();

	blockmode = 0;
    } else
	error ("barpsave:\nNo block specified");

    return (result);
} /* do_barpsave */


/*DEFHELP #cmd io SAVEOLD - save current text under current name */

DEFUSERCMD("saveold", 0, CF_VWM|CF_COK|CF_ICO, int, do_save, (void),)
{
    av[1] = Ep->name;

    return (do_writeto ());
} /* do_save() */


/*DEFHELP #cmd io SAVEAS file - save current text under a different name (title line name does change) */

DEFUSERCMD("saveas", 1, CF_VWM|CF_COK|CF_ICO, int, do_saveas, (void),)
{
    int ret = saveit ("w");

    do_chfilename ();

    return (ret);
} /* do_saveas */


/*DEFHELP #cmd io WRITETO file - write text to this file. The current name of the text is not changed. */

DEFUSERCMD("writeto", 1, CF_VWM|CF_COK|CF_ICO, int, do_writeto, (void),)
{
    return (saveit ("w"));
} /* do_writeto */


/*DEFHELP #cmd requester,io ARPSAVE - SAVEAS with filerequester */

DEFUSERCMD("arpsave", 0, CF_VWM|CF_ICO, int, do_aslsave, (void),)
{
    return (aslsave ());
} /* do_aslsave */


/*DEFHELP requester,io REQSAVE - SAVEAS with ReqTools filerequester */

DEFUSERCMD("reqsave", 0, CF_VWM|CF_ICO, int, do_reqsave, (void),)
{
    return (reqsave ());
} /* do_reqsave */


Prototype int getappname (char * buffer, int bsize);

int getappname (char * buffer, int bsize)
{
    char   buff2[108];
    char * pn ;
    BPTR   pd_lock;

    pd_lock = GetProgramDir();

    if (!Wbs) {
	/* pd_lock = GetProgramDir(); */

	if (!GetProgramName (buff2, sizeof (buff2))) {
	    // ERROR (size) or WB-Start or s.o. did set it name NULL
	    return FALSE;
	} /* if */
	pn = buff2;
     } else {
	/* pd_lock = ((struct WBStartup *)Wbs)->sm_ArgList[0].wa_Lock; */
	pn = ((struct WBStartup *)Wbs)->sm_ArgList[0].wa_Name;
    } /* if (!)wbs */

    /* printf ("PName: %s\n", pn); */

    if (pd_lock) {
       if (!NameFromLock (pd_lock, buffer, bsize)) {
	   // ERROR (size or path not mounted)
	   return FALSE;
	} /* if */

	/* printf ("PDir:  %s ($%08lx)\n", buffer, pd_lock); */

	if (!AddPart (buffer, FilePart(pn), bsize)) {
	    // ERROR (size)
	    return FALSE;
	} /* if */
    } else
	strncpy (buffer, pn, bsize);

    /* printf ("PPath: %s\n", buffer); */
    return TRUE;
} /* getappname */


Prototype int saveit (const char * om);

int saveit (const char * om)
{
    BPTR    oldlock;
    FILE  * fi;
    LONG    i;
    WORD    j,
	    k;
    UBYTE * ptr,
	  * bp,
	    buffer[MAXLINELEN];
    Line    xs,
	    xe;
    ED	  * ep;

    if (!stricmp (av[1], "unnamed"))
    {
	return (do_aslsave ());
    } else if (!stricmp (Ep->name, "unnamed"))  /* get new name */
    {
	strncpy (Ep->name, FilePart (av[1]), sizeof (Ep->name)-1);
	Ep->name[sizeof(Ep->name)-1] = 0;
    }

    if (blockmode) /* block is ok */
    {
	xs = ActualBlock.start_line;
	xe = ActualBlock.end_line + 1;
	ep = ActualBlock.ep;
    } else
    {
	xs = 0;
	xe = Ep->lines;
	ep = Ep;
    }

    text_sync ();
    oldlock = CurrentDir (Ep->dirlock);

    if (GETF_DOBACK(Ep))
    {
	BPTR lock;

	lock = Lock (av[1], ACCESS_READ);

	if (lock)
	{
	    UnLock (lock);
	    sprintf (buffer, "%s.bak", av[1]);
	    Rename (av[1], buffer);
	}
    }

    if ( (fi = fopen (av[1], om)) )
    {
	title ("Saving ...");

	for (i = xs; i < xe; ++i)
	{
	    ptr = GETTEXT(ep,i);

	    if (GETF_SAVETABS(ep))
	    {
		for (bp = buffer, j = 0; *ptr; ++ptr, ++bp, j = (j+1)&7)
		{
		    *bp = *ptr;
		    if (j == 7 && *bp == ' ' && *(bp-1) == ' ')
		    {
			k = j;
			while (k-- >= 0 && *bp == ' ')
			    --bp;
			*++bp = 9;
		    } else
		    {
			if (GETF_SIMPLETABS(ep))        /* PATCH_NULL [25 Jan 1993] : line added */
			    break;			/* PATCH_NULL [25 Jan 1993] : line added */

			if (*bp < 32 || *bp == '\"' || *bp == '\'' || *bp == '`' || *bp == '(')
			    break;
		    }
		}
		strcpy ((char *)bp, (char *)ptr);

		ptr = buffer;
	    }

	    fputs (ptr, fi);
	    fputc ('\n', fi);
	}

	if (fclose (fi))
	{
	    error ("save:\nWrite failed !");
	    goto undo;
	} else
	{
	    title ("Saving OK");
	    SETF_MFORCETITLE(ep,TRUE);

	    /* Only remove modify if blockmode is OFF */
	    SETF_MODIFIED(ep, (GETF_MODIFIED(ep) && blockmode));
	}

	/* PATCH_NULL 12-08-94 moved here */
	if (GETF_SAVEICONS(ep))       /* PATCH_NULL 12-08-94 replaced Wbs && Wdisable */
	{     /* Write out .info file */
	    DISKOBJ sdo, * d;

	    memset (&sdo, 0, sizeof(sdo));

	    if ((d = GetDiskObject ((char *)av[1])) == NULL)
	    {
		/* PATCH_NULL 15-08-94 ADDED - use any icon */
		char *iconname;
		if ((iconname = getvar("iconname"))) { /* that is a very slow method, but we need not care since we have Disk access following */
		    if ((d = GetDiskObject (iconname))) {
			PutDiskObject((char *)av[1], d);
			FreeDiskObject (d);
		    } /* if */
		    free (iconname);
		} /* if */
		if (d != NULL); /* begin an (empty) "if"-statement to allow the following "else"; we cannot use the previous "if" since that does not make sure, we have an icon found */
		/* PATCH_NULL 15-08-94 ADDED - use any icon */

		/* Get path to XDME with name of the editor appended */
		/* WAS: if (Wbs && getpathto (Wbs->sm_ArgList[0].wa_Lock, Wbs->sm_ArgList[0].wa_Name, tmp_buffer)) */
		else if (getappname(tmp_buffer, sizeof(tmp_buffer)))  /* PATCH_NULL 12-08-94 : added */
		{

		    sdo.do_Magic   = WB_DISKMAGIC;
		    sdo.do_Version = WB_DISKVERSION;

		    makemygadget (&sdo.do_Gadget);

		    sdo.do_Type        = WBPROJECT;
		    sdo.do_DefaultTool = (char *)tmp_buffer;
		    sdo.do_ToolTypes   = NULL;
		    sdo.do_CurrentX    = NO_ICON_POSITION;
		    sdo.do_CurrentY    = NO_ICON_POSITION;
		    sdo.do_DrawerData  = NULL;
		    sdo.do_ToolWindow  = NULL;
		    sdo.do_StackSize   = 8192;

		    PutDiskObject ((char *)av[1], &sdo);
		}
	    } else
	    {
		FreeDiskObject (d);
	    }
	}
    } else
    {
	error ("save:\nUnable to open write file\n`%s'", av[1]);
undo:
	if (GETF_DOBACK(Ep))
	{
	    sprintf (buffer, "%s.bak", av[1]);
	    Rename (buffer, av[1]);
	}
    }

    blockmode = 0;
    CurrentDir (oldlock);

    window_title ();

    return (!GETF_ABORTCOMMAND(ep));
} /* do_saveit */


/*DEFHELP #cmd io CD dir - set directory of current window to @{B}dir@{UB} */

DEFUSERCMD("cd", 1, CF_VWM|CF_ICO, void, do_cd, (void),)
{
    BPTR oldlock;
    BPTR lock;

    oldlock = CurrentDir (Ep->dirlock);

    if ( (lock = Lock (av[1], SHARED_LOCK)) )
    {
	UnLock (CurrentDir (oldlock));
	Ep->dirlock = (long)lock;
    }
    else
    {
	CurrentDir (oldlock);

	error ("cd:\nUnable to cd to\n`%s'", av[1]);
    }
} /* do_cd */


/*DEFHELP #cmd misc,io PRINT text - Print @{B}text@{UB} to the shell XDME was started in */

DEFUSERCMD("print", 1, CF_VWM|CF_ICO,void,do_print,(void),)
{
    puts (av[1]);
} /* do_print */


/******************************************************************************
***** END io.c
******************************************************************************/
