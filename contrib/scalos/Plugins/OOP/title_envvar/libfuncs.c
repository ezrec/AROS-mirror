/*
** libfuncs.c
**
// $Date$
// $Revision$
*/

#include <exec/types.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <scalos/scalos.h>

#include <utility/hooks.h>

#include <defs.h>
#include "plugin.h"

#ifdef __AROS__
#include "plugin-common.c"
#endif

//----------------------------------------------------------------

/* This is the UWORD which contains the ASCII value of
** the characters that this plugin should be activated by.
** in this case the ASCII characters are 0x65 followed by
** 0x76 which corresponds to "ev". This means that to
** activate this plugin you need to have "%ev" enetered
** somewhere in the title string in the Scalos main prefs.
*/
#define PARSE   0x6576

//----------------------------------------------------------------

#ifndef __AROS__
struct DosLibrary *DOSBase = NULL;
#endif

#ifdef __amigaos4__
/* On OS4 DoSuperMethod is part of intuition.library */
struct IntuitionBase *IntuitionBase = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct DOSIFace *IDOS = NULL;
struct Library *NewlibBase = NULL;
struct Interface *INewlib = NULL;
#endif /* __amigaos4__ */

STRPTR VersTag = LIB_VERSTRING;


BOOL initPlugin(struct PluginBase *base)
{
	DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 39);
	if (NULL == DOSBase)
		return FALSE;

#ifdef __AROS__
	base->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		return FALSE;

	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
	if (NULL == IntuitionBase)
		return FALSE;
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif /* __amigaos4__ */

	return TRUE;
}

void closePlugin(struct PluginBase *base)
{
#ifdef __amigaos4__
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

	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
}

//----------------------------------------------------------------

/* The actual (hook) function which does the work for this plugin

	NAME
                EnvVar

	SYNOPSIS
		success = EnvVar(cls, object, message)
		D0                 A0   A2      A1

		LONG = EnvVar(Class *, struct ScaRootList *, struct Title_Translate_Params *)

	FUNCTION
		To add the number of free pens to the title (bar) in Scalos

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
M68KFUNC_P3(ULONG, EnvVar,
	A0, Class *, cls,
	A2, Object *, object,
	A1, Msg, message)
{
	Class                   *keepclass = cls;   	/* a copy of the class just in case: register A0 is a scratch register remember */
	struct msg_Translate	*msg = (struct msg_Translate *)message;     	/* a copy of the message just in case: register A1 is a scratch register remember */

	/* Make sure that the method being called is the title translate method */
	if(msg->mxl_MethodID == SCCM_Title_Translate)
	{
		/* also check that the parse characters we have been passed are the ones that our
		** function will deal with */
		if(msg->mxl_ParseID == PARSE)
		{
			char VarName[128];
			CONST_STRPTR FmtPtr;
			STRPTR DestPtr;
			size_t Len;
			ULONG Success = FALSE;

			for (FmtPtr = msg->mxl_TitleFormat, DestPtr = VarName, Len = sizeof(VarName);
				*FmtPtr && '?' != *FmtPtr && Len > 1;
				*DestPtr++ = *FmtPtr++, Len--)
				;
			if ('?' == *FmtPtr)
				{
				// End of variable name found.
				LONG VarLen;

				*DestPtr = '\0';

				// Skip variable name
				msg->mxl_TitleFormat = FmtPtr + 1;

				// First, check for local variable
				VarLen = GetVar(VarName, msg->mxl_Buffer, msg->mxl_BuffLen, GVF_LOCAL_ONLY);

				// if no local variable found, check global variable
				if (-1 == VarLen)
					VarLen = GetVar(VarName, msg->mxl_Buffer, msg->mxl_BuffLen, GVF_GLOBAL_ONLY);

				if (VarLen > 0)
					{
					/* Advance the pointer to the title text to the new end
					** of the title Text (add on the number of characters we have added
					*/
					Success = TRUE;
					msg->mxl_BuffLen -= VarLen;
					msg->mxl_Buffer += VarLen;
					}
				}

			/* return boolean success value. Note that this may differ
			** depending on what method you are dealing with
			*/
			return Success;
		}
	}

	/* Call superclass method if we have failed to do something (this method does not get processed by
	** this function, the parse characters were not what we were looking for, etc) */
	return (ULONG) DoSuperMethodA(keepclass, object, (Msg)msg);
}
M68KFUNC_END

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------

