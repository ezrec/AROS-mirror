/*
 * libfuncs.c
 *
 * $Date$
 * $Revision$
 *
 */

#include <exec/types.h>
#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/locale.h>
#include <proto/intuition.h>

#include <dos/dos.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <libraries/locale.h>
#include <scalos/scalos.h>

#include <utility/date.h>
#include <utility/hooks.h>

#include <stdio.h>
#include <string.h>

#include <defs.h>
#include "prefs/prefs_file.h"
#include "libfuncs.h"

#ifdef __AROS__
#include "plugin-common.c"
#endif

/*
**  The TITLETIME identifier (%ti in Scalos title prefs) adds the current time to the title bar.
**  TITLEDATE adds %da so that the current date. Both are in their default locale.
*/
#define TITLETIME   0x7469
#define TITLEDATE   0x6461


struct FormatDateHookData
	{
	STRPTR fdhd_Buffer;		// buffer to write characters to
	size_t fdhd_Length;		// length of buffer
	};


#ifndef __AROS__
struct DosLibrary   *DOSBase;
T_LOCALEBASE   	LocaleBase;
#endif

#ifdef __amigaos4__
struct IntuitionBase *IntuitionBase;
struct Library *NewlibBase;
#endif

#ifdef __amigaos4__
struct LocaleIFace *ILocale;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
struct Interface *INewlib;
#endif

struct myLibBase *globalLibBase;

STRPTR VersTag = LIB_VERSTRING;

/* Hook function for FormatDate() which takes a character and pushes into
 * the string specified in the hook.h_Data field
 */
static M68KFUNC_P3(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch)
{
	struct FormatDateHookData *fd = (struct FormatDateHookData *) theHook->h_Data;

	(void) locale;

	if (fd->fdhd_Length >= 1)
		{
		*(fd->fdhd_Buffer)++ = ch;
		fd->fdhd_Length--;
		}
}
M68KFUNC_END


/* The actual (hook) function which does the work for this plugin

	NAME
		TimeDate

	SYNOPSIS
		success = TimeDate(cls, object, message)
		D0                 A0   A2      A1

		LONG = TimeDate(Class *, struct ScaRootList *, struct Title_Translate_Params *)

	FUNCTION
		To add the current time and date to the title (bar) in Scalos

	INPUTS
		cls     - Pointer to the class from which this function was called
		object  - the ScaRootList pointer from Scalos
		message - the message being passed to our function that we must act on
		NB: These will always be the same except for the type of message that is
		passed. Check scalos/scalos.h for what will be passed in the message for
		a particular method.

	RESULT
		success - boolean to show success or failure.
		NB: This is different for different methods, so check what the return
		value should be for a particular method in scalos/scalos.h

	SEE ALSO
		scalos/scalos.h
*/
M68KFUNC_P3(ULONG, TimeDate,
	A0, Class *, cls,
	A2, Object *, object,
	A1, Msg, message)
{
	Class                   *keepclass = cls;   	/* a copy of the class just in case: register A0 is a scratch register remember */
	struct msg_Translate	*msg = (struct msg_Translate *)message;     	/* a copy of the message just in case: register A1 is a scratch register remember */
	struct Locale           *loc = NULL;        	/* So we can get the correct date/time format for the display */
	ULONG                   retval = 0;
	BOOL dosup = TRUE;

	if (globalLibBase->lb_NotifyPort)
	{
		struct Message *msg;
		BOOL ReloadPrefs = FALSE;

		d1(KPrintF(__FILE__ "/%s/%ld: check notification messages\n", __FUNC__, __LINE__));

		while (msg = GetMsg(globalLibBase->lb_NotifyPort))
			{
			d1(KPrintF(__FILE__ "/%s/%ld: msg=%08lx\n", __FUNC__, __LINE__, msg));
			ReloadPrefs = TRUE;
			ReplyMsg(msg);
			}

		if (ReloadPrefs)
			{
			LoadPrefs(globalLibBase->lb_datestr, FORMAT_SIZE,
				globalLibBase->lb_timestr, FORMAT_SIZE);
			d1(KPrintF(__FILE__ "/%s/%ld: date='%s' time='%s'\n", \
				__FUNC__, __LINE__, globalLibBase->lb_datestr, globalLibBase->lb_timestr));
			}
	}

	/* Make sure that the method being called is the title translate method */
	if (msg->mxl_MethodID == SCCM_Title_Translate)
	{
		struct DateStamp        ds;
		struct FormatDateHookData fd;
		struct Hook fmtHook;
		STATIC_PATCHFUNC(FormatDateHookFunc)

		DateStamp(&ds);

		fmtHook.h_Entry = (HOOKFUNC) PATCH_NEWFUNC(FormatDateHookFunc);
		fmtHook.h_Data = &fd;

		fd.fdhd_Buffer = msg->mxl_Buffer;
		fd.fdhd_Length = msg->mxl_BuffLen;

		loc = OpenLocale(NULL);             /* Guaranteed to be successful */

		switch(msg->mxl_ParseID)
		{
		case TITLETIME:
			if(*globalLibBase->lb_timestr)
				{
				FormatDate(loc, globalLibBase->lb_timestr, &ds, &fmtHook);
				}
			else
				{
				FormatDate(loc, loc->loc_TimeFormat, &ds, &fmtHook);
				}
			msg->mxl_BuffLen -= strlen(msg->mxl_Buffer);
			msg->mxl_Buffer += strlen(msg->mxl_Buffer);
			dosup = FALSE;
			retval = 1;
			break;

		case TITLEDATE:
			if(*globalLibBase->lb_datestr)
				{
				FormatDate(loc, globalLibBase->lb_datestr, &ds, &fmtHook);
				}
			else
				{
				FormatDate(loc, loc->loc_DateFormat, &ds, &fmtHook);
				}
			msg->mxl_BuffLen -= strlen(msg->mxl_Buffer);
			msg->mxl_Buffer += strlen(msg->mxl_Buffer);
			dosup = FALSE;
			retval = 1;
			break;
		}
	}

	if (dosup)
		{
		/* Call superclass method if we have failed to do something (this method does not get processed by
		** this function, the parse characters were not what we were looking for, etc) */
		retval = DoSuperMethodA(keepclass, (Object *)object, (Msg) msg);
		}
	if (loc)
		CloseLocale(loc);

	return(retval);
}
M68KFUNC_END

BOOL initPlugin(struct PluginBase *pluginBase)
{
	struct myLibBase *myLibBase = (struct myLibBase *)pluginBase;

#ifdef __AROS__
	pluginBase->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

	BOOL Success = FALSE;

	do	{
		globalLibBase = myLibBase;

		DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 36);
		if (NULL == DOSBase)
			break;
#ifdef __amigaos4__
		IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
		if (NULL == IDOS)
			break;
#endif
		LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 0);
		d1(KPrintF(__FILE__ "/%s/%ld: LocaleBase=0x%08X\n", __FUNC__, __LINE__, LocaleBase));
		if (NULL == LocaleBase)
			break;
#ifdef __amigaos4__
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		if (NULL == ILocale)
			break;
		IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
		if (NULL == IntuitionBase)
			break;
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			break;
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			break;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			break;
#endif

		d1(KPrintF(__FILE__ "/%s/%ld: All libs OK\n", __FUNC__, __LINE__));

		LoadPrefs(myLibBase->lb_datestr, FORMAT_SIZE,
			myLibBase->lb_timestr, FORMAT_SIZE);

		d1(KPrintF(__FILE__ "/%s/%ld: LoadPrefs OK  datestr=<%s>  timestr=<%s>\n", \
			__FUNC__, __LINE__, myLibBase->lb_datestr, myLibBase->lb_timestr));

		myLibBase->lb_NotifyPort = CreateMsgPort();
		d1(KPrintF(__FILE__ "/%s/%ld: NotifyPort=%08lx\n", __FUNC__, __LINE__, myLibBase->lb_NotifyPort));
		if (NULL == myLibBase->lb_NotifyPort)
			break;

		myLibBase->lb_NotifyRequest.nr_Name = NULL;
		myLibBase->lb_NotifyRequest.nr_UserData = (ULONG)NULL;
		myLibBase->lb_NotifyRequest.nr_Flags = NRF_SEND_MESSAGE;
		myLibBase->lb_NotifyRequest.nr_stuff.nr_Msg.nr_Port = myLibBase->lb_NotifyPort;

		(void) StartPrefsNotify(&myLibBase->lb_NotifyRequest);

		d1(KPrintF(__FILE__ "/%s/%ld: StartPrefsNotify OK\n", __FUNC__, __LINE__));

		Success = TRUE;

	} while (0);

	return Success;
}


BOOL closePlugin(struct PluginBase *pluginBase)
{
	struct myLibBase *myLibBase = (struct myLibBase *)pluginBase;

	EndPrefsNotify(&myLibBase->lb_NotifyRequest);

	if (myLibBase->lb_NotifyPort)
		{
		DeleteMsgPort(myLibBase->lb_NotifyPort);
		myLibBase->lb_NotifyPort = NULL;
		}
#ifdef __amigaos4_
	if (INewlib)
		{
		DropInterface(INewlib);
		INewlib = NULL;
		}
	if (NewlibBase)
		{
		CloseLibrary(NewlibBase);
		NewlibBase = NULL;
		}
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
	if(IntuitionBase)
		{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
		}
	if(IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif
	if(DOSBase)
		{
		CloseLibrary((struct Library *)DOSBase);
		DOSBase = NULL;
		}
#ifdef __amigaos4_
	if(ILocale)
		{
		DropInterface((struct Interface *)ILocale);
		ILocale = NULL;
		}
#endif
	if(LocaleBase)
		{
		CloseLibrary((struct Library *)LocaleBase);
		LocaleBase = NULL;
		}
	return TRUE;
}

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
