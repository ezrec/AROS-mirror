/*
**  HTMail.c
*/

#include <proto/dos.h>
#include <proto/exec.h>

#include <dos/dostags.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "HTAccess.h"
#include "HTAnchor.h"
#include "HTUtils.h"

LONG StatFile (const UBYTE *, struct FileInfoBlock *);
BPTR OpenTemp (UBYTE *);
LONG Compose (const char *, const char *);

extern void application_user_feedback (char *);
extern char *amiga_editor;
extern char *amiga_sendmail;

static char
	buf [256];

LONG
StatFile (const UBYTE *filename, struct FileInfoBlock *fib)
{
	BPTR
		lock = Lock ((UBYTE *) filename, ACCESS_READ);
	LONG
		i = 0;

	if (!lock)
	{
		i = -1;
	}
	else
	{
		if (!Examine (lock, fib))
			i = -1;
	}

	if (lock)
	{
		UnLock (lock);
	}

	if (i < 0)
	{
		sprintf (buf, "Can't open %s", filename);
		application_user_feedback (buf);
	}

	return i;
}

static LONG
Editor (const UBYTE *filename)
{
	/*
	**  Editor
	**
	**	Call the specified editor to edit the specified filename.
	**
	**	If the Editor failed (non-zero return value), or the user
	**	did not modify the input file, then let the user know, and
	**	return an appropriate value to the caller.
	*/
	LONG
		erslt,
		rslt = 1;
	UBYTE
		*buf = malloc (256);
	struct FileInfoBlock
		*fib1 = NULL,
		*fib2 = NULL;

	if (!buf)
	{
		goto leave;
	}

	fib1 = AllocDosObject (DOS_FIB, NULL);
	if (!fib1)
	{
		goto leave;
	}

	fib2 = AllocDosObject (DOS_FIB, NULL);
	if (!fib2)
	{
		goto leave;
	}

	if (StatFile (filename, fib1) < 0)
	{
		goto leave;
	}

	sprintf (buf, "%s %s", amiga_editor, filename);

	erslt = SystemTags (buf,
			SYS_UserShell, 1,
			NP_StackSize,  32000,
			TAG_DONE);

	if (erslt == -1)
	{
		sprintf (buf, "Couldn't start editor %s", amiga_editor);
		application_user_feedback (buf);
		goto leave;
	}

	if (erslt)
	{
		sprintf (buf, "Editor returned error %ld\nMessage discarded", (long)erslt);
		application_user_feedback (buf);
		goto leave;
	}

	StatFile (filename, fib2);

	if (CompareDates (&fib1->fib_Date, &fib2->fib_Date) == 0)
	{
		application_user_feedback ("No changes to input file.\nMessage discarded");
		goto leave;
	}

	/*
	** everything worked OK
	*/
	rslt = 0;

leave:
	if (fib2)
	{
		FreeDosObject (DOS_FIB, fib2);
	}

	if (fib1)
	{
		FreeDosObject (DOS_FIB, fib1);
	}

	if (buf)
	{
		free (buf);
	}

	return rslt;
}

static LONG
SendMail (const int edit, const UBYTE *filename)
{
	/*
	**  SendMail
	**
	**	Take a completely built article in 'filename' and go through
	**	the required steps to inject the message into the mail system.
	**
	**	Returns zero if the mail was sent correctly.
	**
	**	This routine will ensure that filename is deleted.
	*/
	LONG
		rslt = 1,   /* assume failure */
		len = strlen (filename) + 1;
	UBYTE
		*buf   = malloc (256),
		*fname = malloc (len);
	BPTR
		inp = BNULL;

	if (!buf)
	{
		goto leave;
	}
	if (!fname)
	{
		goto leave;
	}

	/* save filename -- it may be from TempName() */
	strcpy (fname, filename);

	if (edit && Editor (fname))
	{
		goto leave;
	}

	inp = Open ("NIL:", MODE_NEWFILE);
	if (!inp)
	{
		application_user_feedback ("Can't open NIL:");
		goto leave;
	}

	sprintf (buf, "%s <%s\ndelete >nil: quiet %s\n", amiga_sendmail, fname, fname);

	rslt = SystemTags (buf,
			SYS_Input,     inp,
			SYS_Output,    0,
			SYS_Asynch,    1,
			SYS_UserShell, 1,
			NP_StackSize,  32000,
			TAG_DONE);

	if (rslt != -1)
	{
		inp = 0;
		rslt = 0;
	}

	if (rslt != 0)
	{
		if (rslt < 0)
			sprintf (buf, "Could not initiate %s", amiga_sendmail);
		else
			sprintf (buf, "Error from %s\nReturn value %ld", amiga_sendmail, (long)rslt);
		application_user_feedback (buf);
	}

leave:
	if (inp)
	{
		Close (inp);
	}

	if (fname)
	{
		free (fname);
	}

	if (buf)
	{
		free (buf);
	}

	return rslt;
}

static void
AppendHeader (BPTR file)
{
	BPTR
		header;
	APTR
		winptr;
	struct Process
		*me = (struct Process *) FindTask (NULL);
	UBYTE
		*buf = malloc (256);

	const UBYTE
		H_Name [] = "uulib:.header";

	extern char
		*amosaic_version;

	if (!buf)
	{
		return;
	}

	winptr = me->pr_WindowPtr;
	me->pr_WindowPtr = (APTR) -1;

	if ((header = Open ((UBYTE *) H_Name, MODE_OLDFILE)))
	{
		while (FGets (header, buf, 254))
		{
			Write (file, buf, strlen (buf));
		}
		Close (header);
	}

	sprintf (buf, "X-Mailer: %s\n\n", &amosaic_version [6]);
	Write (file, buf, strlen (buf));

	free (buf);
	me->pr_WindowPtr = winptr;

	return;
}

static void
AppendSignature (BPTR file)
{
	BPTR
		sig;
	UBYTE
		*buf = malloc (256);
	APTR
		winptr;
	struct Process
		*me = (struct Process *) FindTask (NULL);

	const UBYTE
		S_Name [] = "uulib:.signature";

	if (!buf)
	{
		return;
	}

	winptr = me->pr_WindowPtr;
	me->pr_WindowPtr = (APTR) -1;

	if ((sig = Open ((UBYTE *) S_Name, MODE_OLDFILE)))
	{
		Write (file, "-- \n", 4);
		while (FGets (sig, buf, 254))
		{
			Write (file, buf, strlen (buf));
		}
		Close (sig);
	}

	free (buf);
	me->pr_WindowPtr = winptr;

	return;
}

static const char
	HEADER_Start [] = "To: %s\nSubject: %s%s\n";
static long
	temp_inx = 1;

BPTR
OpenTemp (UBYTE *ptr)
{
	BPTR
		t;

	sprintf (ptr, "T:T.%p.%ld.mosaic", FindTask (NULL), (long)(temp_inx++));
	t = Open (ptr, MODE_NEWFILE);
	if (!t)
	{
		sprintf (buf, "Can't open %s", ptr);
		application_user_feedback (buf);
	}

	return t;
}

LONG
Compose (const char *where, const char *orig_url)
{
	UBYTE
		*ptr = malloc (256);
	BPTR
		mail;
	LONG
		rslt = 0;

	if (!ptr)
	{
		return HT_NOT_LOADED;
	}

	mail = OpenTemp (ptr);
	if (!mail)
	{
		goto leave;
	}

	sprintf (buf, HEADER_Start, where ? where : "", orig_url ? orig_url : "", "");
	Write (mail, buf, strlen (buf));
	AppendHeader (mail);
	AppendSignature (mail);
	Close (mail);

	rslt = SendMail (1, ptr);

	if (rslt)
		rslt = HT_NOT_LOADED;
	else
		rslt = HT_LOADED;  /* success, but no document */

leave:
	if (ptr)
	{
		free (ptr);
	}

	return rslt;
}

static int mailinitialized;

static int mailinitialize (void)
{
	if (!amiga_editor || *amiga_editor == '\0')
	{
		application_user_feedback ("Mosaic/Editor is not defined");
		return 0;
	}

	if (!amiga_sendmail || *amiga_sendmail == '\0')
	{
		application_user_feedback ("Mosaic/Sendmail is not defined");
		return 0;
	}

	mailinitialized = 1;
	return 1;
}

int
HTLoadMail (const char * arg, HTParentAnchor * anchor, HTFormat format_out, HTStream * stream)
{
	const char
		*ptr;
	int
		rslt;

	CTRACE (stderr, "HTLoadMail: arg '%s', address '%s' physical '%s' title '%s'\n", arg, anchor->address, anchor->physical, anchor->title);
	CTRACE (stderr, "parent is self == %s\n", anchor->parent == anchor ? "true" : "false");

	if (!mailinitialized)
	{
		if (!mailinitialize ())
		{
			CTRACE (stderr, "HTLoadMail: mailinitialize() failed\n");
			return HT_NOT_LOADED;
		}
	}

	ptr = arg;
	if (strnicmp (ptr, "mailto:", 7) == 0)
		ptr += 7;

	rslt = (int) Compose (ptr, anchor->address);
	CTRACE (stderr, "Compose() returned %ld\n", (long)rslt);

	return rslt;
}

HTProtocol HTMail = { "mailto", HTLoadMail, NULL };
