/*
 *  prefs_file.c
 *
 * $Date$
 * $Revision$
 *
 *  20010716 DM Created. Will be used by prefs and plugin programs
 *
 */

#ifdef __NOLIBBASE__
#undef __NOLIBBASE__
#endif

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/debug_protos.h>

#include <dos/dosextens.h>
#include <dos/notify.h>
#include <string.h>

#include "defs.h"
#include "prefs_file.h"

#define	d1(x)		;
#define	d2(x)		x;


/* Tries to find the volume or assign (confusingly ;) that is passed - without the
 * colon!, e.g. FindDevice("ENV"); Returns true if found. Means you don't need to
 * disable requesters when searching for volumes (e.g. ENV: :)
 */
static BOOL FindDevice(CONST_STRPTR devname)
{
	struct DosList  *dl;        /* For locking/finding volume or assign in the doslist */
	BOOL            found = 0;  /* flag to return showing if we found the device */
	struct DosList  *de;        /* entry in the doslist */

	dl = LockDosList(LDF_VOLUMES|LDF_ASSIGNS|LDF_DEVICES|LDF_READ);

	d1(kprintf("FindDevice: finding '%s'\n", devname));
	if(NULL != (de = FindDosEntry(dl, devname, LDF_VOLUMES|LDF_ASSIGNS|LDF_DEVICES)) )
	{
		d1(kprintf("FindDevice: found!\n"));
		found = 1;
	}
	UnLockDosList(LDF_VOLUMES|LDF_ASSIGNS|LDF_DEVICES|LDF_READ);
	return(found);
}


LONG ReadPrefsFile(CONST_STRPTR file, STRPTR datefmt, ULONG datelen, STRPTR timefmt, ULONG timelen)
{
	BPTR    fh;         /* File handle for the saved file */
	char    *newline;   /* pointer to the first newline character in each string */
	LONG    result = 0;

	fh = Open(file, MODE_OLDFILE);
	if(fh)
	{
		/* Fix for pre-V39 FGets() */
		if(((struct Library *)DOSBase)->lib_Version < 39) { datelen--; timelen--; }

		/* Read line for dat format into string and replace any newline with a NULL */
		if(FGets(fh, datefmt, datelen))
		{
			newline = strchr(datefmt, '\n');
			if(newline) *newline = '\0';
			result++;
		}

		/* Same for the time format */
		if(FGets(fh, timefmt, timelen))
		{
			newline = strchr(timefmt, '\n');
			if(newline) *newline = '\0';
			result++;
		}

		Close(fh);
	}
	return(result);
}


LONG WritePrefsFile(CONST_STRPTR file, CONST_STRPTR datefmt, CONST_STRPTR timefmt)
{
	BPTR    fh;     /* File handle for the saved file */
	LONG    result = 0;

	fh = Open(file, MODE_NEWFILE);
	if(fh)
	{
		FPuts(fh, datefmt); FPutC(fh, '\n');
		FPuts(fh, timefmt); FPutC(fh, '\n');
		Close(fh);
		result = 1;
	}
	return(result);
}


LONG LoadPrefs(STRPTR datefmt, ULONG datelen, STRPTR timefmt, ULONG timelen)
{
	LONG    result = 0;

	d1(kprintf("LoadPrefs: enter"));
	if(FindDevice("ENV"))
	{
		result = ReadPrefsFile(USENAME, datefmt, datelen, timefmt, timelen);
		d1(kprintf("LoadPrefs: ENV found, result=%ld\n", result));
	}

	if(!result && FindDevice("ENVARC"))
	{
		result = ReadPrefsFile(SAVENAME, datefmt, datelen, timefmt, timelen);
		d1(kprintf("LoadPrefs: ENV found, result=%ld\n", result));
	}

	if(!result)
	{
		ReadPrefsFile("PROGDIR:title_clock.prefs", datefmt, datelen, timefmt, timelen);
		d1(kprintf("LoadPrefs: Reading from PROGDIR:"));
	}
	return(result);
}


/* Saves prefs file to ENV: and ENVARC: */
LONG SavePrefs(CONST_STRPTR datefmt, CONST_STRPTR timefmt)
{
	LONG    result = 0;

	d1(kprintf("SavePrefs: enter"));
	if(FindDevice("ENV"))
	{
		result += WritePrefsFile(USENAME, datefmt, timefmt);
		d1(kprintf("SavePrefs: Env found, result of write=%ld\n", result));
	}
	if(FindDevice("ENVARC"))
	{
		result += WritePrefsFile(SAVENAME, datefmt, timefmt);
		d1(kprintf("SavePrefs: Env found, result of write=%ld\n", result));
	}
	return(result);
}


/* Saves prefs only to ENV: */
LONG UsePrefs(CONST_STRPTR datefmt, CONST_STRPTR timefmt)
{
	LONG    result = 0;

	d1(kprintf("UsePrefs: enter\n"));
	if(FindDevice("ENV"))
	{
		result = WritePrefsFile(USENAME, datefmt, timefmt);
		d1(kprintf("UsePrefs: Env found, result of write ('%s','%s')=%ld\n", datefmt, timefmt, result));
	}
	return(result);
}

BOOL StartPrefsNotify(struct NotifyRequest *notify)
{
	BOOL Success;

	notify->nr_Name = (STRPTR) USENAME;

	Success = StartNotify(notify);

	if (!Success)
		notify->nr_Name = NULL;

	return Success;
}


void EndPrefsNotify(struct NotifyRequest *notify)
{
	if (notify->nr_Name)
		{
		struct Message *msg;

		EndNotify(notify);

		while (msg = GetMsg(notify->nr_stuff.nr_Msg.nr_Port))
			{
			ReplyMsg(msg);
			}
		}
}

