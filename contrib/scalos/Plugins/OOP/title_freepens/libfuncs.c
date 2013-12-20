/*
** libfuncsc
** 19th May 2000
** David McMinn
** The actual public functions that are in our plugin. Remember, there
** is only one public function in a Scalos plugin, and it simply returns
** a pointer to a ScaClassInfo. The h_Entry field of the Hook structure
** within the ScaClassInfo structure points to the actual function which
** performs the task required (adding text to the titles of screens,
** windows etc).
// $Date$
// $Revision$
*/

#include <exec/types.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
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

/* This is the UWORD which contains the ASCII value of
** the characters that this plugin should be activated by.
** in this case the ASCII characters are 0x77 followed by
** 0x70 which corresponds to w and p. This means that to
** activate this plugin you need to have "%wp" enetered
** somewhere in the title string in the Scalos main prefs.
*/
#define PARSE   0x7770


#if 0
/* Our own version of the strlen function, to save having to link
** the standard libraries
*/
ULONG strlen(STRPTR s0)
{
	ULONG n=0;
	while(*s0++)
	{
		n++;
	}
	return(n);
}
#endif

#ifdef __amigaos4__
/* On OS4 DoSuperMethod is part of intuition.library */
struct IntuitionBase *IntuitionBase = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif /* __amigaos4__ */

STRPTR VersTag = LIB_VERSTRING;

BOOL initPlugin(struct PluginBase *base)
{
#ifdef __amigaos4__
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

#ifdef __AROS__
	base->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

	return TRUE;
}

BOOL closePlugin(struct PluginBase *base)
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
#endif /* __amigaos4__ */
	return TRUE;
}

/* The actual (hook) function which does the work for this plugin

	NAME
		FreePens

	SYNOPSIS
		success = FreePens(cls, object, message)
		D0                 A0   A2      A1

		LONG = FreePens(Class *, struct ScaRootList *, struct Title_Translate_Params *)

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
M68KFUNC_P3(ULONG, FreePens,
	A0, Class *, cls,
	A2, Object *, obj,
	A1, Msg, message)
{
	Class                   *keepclass = cls;   	/* a copy of the class just in case: register A0 is a scratch register remember */
	struct msg_Translate	*msg = (struct msg_Translate *)message;     	/* a copy of the message just in case: register A1 is a scratch register remember */
	ULONG                   freepens = 0;   	/* a count of the number of free pens */
	struct ScaRootList	*object = (struct ScaRootList *)obj;

	/* Make sure that the method being called is the title translate method */
	if(msg->mxl_MethodID == SCCM_Title_Translate)
	{
		/* also check that the parse characters we have been passed are the ones that our
		** function will deal with */
		if(msg->mxl_ParseID == PARSE)
		{
			/* if the pointer to the Scalos screen is valid, then work our
			** way through the vast amounts of pointers :) to get the
			** number of free pens in the screen
			*/
			if(object->rl_internInfos->ii_Screen != NULL)
				freepens = (ULONG)object->rl_internInfos->ii_Screen->ViewPort.ColorMap->PalExtra->pe_NFree;

			if (msg->mxl_BuffLen >= 4)
				{
				size_t Length;

				/* Add our text to the end of the title text*/
				sprintf(msg->mxl_Buffer, "%lu", (unsigned long)freepens);

				/* Advance the pointer to the title text to the new end
				** of the title Text (add on the number of characters we have added
				*/
				Length = strlen(msg->mxl_Buffer);
				msg->mxl_BuffLen -= Length;
				msg->mxl_Buffer += Length;
				}

			/* return boolean success value. Note that this may differ
			** depending on what method you are dealing with
			*/
			return(1);
		}
	}

	/* Call superclass method if we have failed to do something (this method does not get processed by
	** this function, the parse characters were not what we were looking for, etc) */
	return (ULONG) DoSuperMethodA(keepclass, (Object *)object, (Msg) msg);
}
M68KFUNC_END

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
