/*
**	Start.c
**
**	The program entry point
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _COMPILER_H
#include "Compiler.h"
#endif	/* _COMPILER_H */

#ifndef INTUITION_INTUITIONBASE_H
#include <intuition/intuitionbase.h>
#endif	/* INTUITION_INTUITIONBASE_H */

#ifndef WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif	/* WORKBENCH_WORKBENCH_H */

#ifndef EXEC_EXECBASE_H
#include <exec/execbase.h>
#endif	/* EXEC_EXECBASE_H */

#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif	/* EXEC_MEMORY_H */

#ifndef DOS_DOSEXTENS_H
#include <dos/dosextens.h>
#endif	/* DOS_DOSEXTENS_H */

#ifndef DOS_EXALL_H
#include <dos/exall.h>
#endif	/* DOS_EXALL_H */

#ifndef CLIB_INTUITION_PROTOS_H
#include <clib/intuition_protos.h>
#endif	/* CLIB_INTUITION_PROTOS_H */

#ifndef CLIB_EXEC_PROTOS_H
#include <clib/exec_protos.h>
#endif	/* CLIB_EXEC_PROTOS_H */

#ifndef CLIB_DOS_PROTOS_H
#include <clib/dos_protos.h>
#endif	/* CLIB_DOS_PROTOS_H */

#ifdef __GNUC__
#ifdef __AROS__
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#else
#include <inline/intuition.h>
#include <inline/exec.h>
#include <inline/dos.h>
#endif
#endif	/* __GNUC__ */

#ifndef NO_PRAGMAS
#include <pragmas/intuition_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#endif	/* NO_PRAGMAS */

#include <string.h>

	/* Standard topaz.font/8. */

extern struct TextAttr		 DefaultFont;

	/* Library base pointers. */

extern struct ExecBase		*SysBase;
extern struct DosLibrary	*DOSBase;
extern struct IntuitionBase	*IntuitionBase;

	/* The appropriate machine type check. */

#if defined(CPU_ANY)
#define MACHINE_OKAY (1)
#else
#define MACHINE_OKAY (SysBase->AttnFlags & AFF_68020)
#endif

	/* Start():
	 *
	 *	Program entry point, checks for the right CPU type and
	 *	runs the main program if possible.
	 */

LONG SAVE_DS STACKARGS
#ifndef __AROS__
Start(VOID)
#else
main(VOID)
#endif
{
	extern LONG Main(VOID);

	LONG Result = RETURN_FAIL;

		/* Get SysBase. */

#ifndef __AROS__
	SysBase = *(struct ExecBase **)4;
#endif
		/* Is the minimum required processor type and Kickstart 2.04 (or higher)
		 * available?
		 */

	if(MACHINE_OKAY && (SysBase->LibNode.lib_Version > 37 || (SysBase->LibNode.lib_Version == 37 && SysBase->SoftVer >= 175)))
		Result = Main();
	else
	{
		struct Process *ThisProcess = (struct Process *)FindTask(NULL);

			/* Is a Shell window available? */

		if(ThisProcess->pr_CLI)
		{
				/* Show the message. */

			if(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",0))
			{
				STRPTR String;

				if(MACHINE_OKAY)
					String = "Kickstart 2.04 or higher required.\a\n";
				else
					String = "MC68020 or higher required.\a\n";

				Write(Output(),String,strlen(String));

				CloseLibrary((struct Library *)DOSBase);
			}
		}
		else
		{
			struct WBStartup *WBenchMsg;

				/* Wait for startup message. */

			WaitPort(&ThisProcess->pr_MsgPort);

				/* Get the message. */

			WBenchMsg = (struct WBStartup *)GetMsg(&ThisProcess->pr_MsgPort);

				/* Show the message. */

			if(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0))
			{
				STATIC struct IntuiText SorryText = {0,1,JAM1,6,3,&DefaultFont,(STRPTR)"Sorry",NULL};
				struct IntuiText *Body;
				LONG Width;

				if(MACHINE_OKAY)
				{
					STATIC struct IntuiText BodyText = {0,1,JAM1,5,3,&DefaultFont,(STRPTR)"Kickstart 2.04 or higher required.",NULL};

					Body	= &BodyText;
					Width	= 309;
				}
				else
				{
					STATIC struct IntuiText BodyText = {0,1,JAM1,5,3,&DefaultFont,(STRPTR)"MC68020 or higher required.",NULL};

					Body	= &BodyText;
					Width	= 253;
				}

				AutoRequest(NULL,Body,NULL,&SorryText,0,0,Width,46);

				CloseLibrary((struct Library *)IntuitionBase);
			}

				/* Return the startup message and exit. */

			Forbid();

			ReplyMsg((struct Message *)WBenchMsg);
		}
	}

	return(Result);
}
