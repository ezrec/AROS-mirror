#include <intuition/intuitionbase.h>
#include <workbench/workbench.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>

#ifdef USE_PROTO_INCLUDES
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#else
#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

#include <pragmas/intuition_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#endif

#include "filex.h"

	/* exec.library base pointer. */

extern struct ExecBase	*SysBase;

	/* The approriate machine type check. */

#ifdef MACHINE020
#define MACHINE_OKAY (SysBase -> AttnFlags & AFF_68020)
#else
#define MACHINE_OKAY (1)
#endif

extern LONG start(void);

/* 
 * LONG __saveds Start(void)
 *
 * Prüft Prozessor und Kickstartversion. Entweder wird main() aufgerufen
 * oder eine Fehlermeldung ausgegeben.
 */

#include <aros/debug.h>

#ifdef __AROS__
__startup static AROS_PROCH(Start, argstr, argsize, AbsExecBase)
#else
#define AbsExecBase (*(struct ExecBase **)4)
#define AROS_PROCFUNC_INIT
#define AROS_PROCFUNC_EXIT

LONG __saveds Start(void)
#endif
{
	AROS_PROCFUNC_INIT

	static const unsigned char versionstring[] =
		"\0$VER:FileX "VSTRING" ("DATE") © 1993-1994 by Klaas Hermanns.";
		/* Get SysBase. */

#ifdef __AROS__
    	(void)versionstring;
#endif
	SysBase = AbsExecBase;
	bug("SysBase = %p\n", SysBase);

		/* Is the minimum required processor type and Kickstart 2.04 (or higher)
		 * available?
		 */
	if(MACHINE_OKAY && (SysBase -> LibNode . lib_Version > 37 || (SysBase -> LibNode . lib_Version == 37 && SysBase -> SoftVer >= 175)))
		return(start());
	else
	{
		register struct Process *ThisProcess = (struct Process *)FindTask(NULL);

			/* Is a Shell window available? */

		if(ThisProcess -> pr_CLI)
		{
			register struct DosLibrary *DOSBase;

				/* Show the message. */

			if((DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",0)))
			{
				if(MACHINE_OKAY)
					Write(ThisProcess -> pr_COS,"Kickstart 2.04 or higher required.\n",36);
				else
					Write(ThisProcess -> pr_COS,"MC68020 or higher required.\n",29);

				CloseLibrary((struct Library *)DOSBase);
			}

			return(RETURN_FAIL);
		}
		else
		{
			register struct IntuitionBase	*IntuitionBase;
			register struct WBStartup	*WBenchMsg;

				/* Wait for startup message. */

			WaitPort(&ThisProcess -> pr_MsgPort);

				/* Get the message. */

			WBenchMsg = (struct WBStartup *)GetMsg(&ThisProcess -> pr_MsgPort);

				/* Show the message. */

			if((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
			{
				STATIC struct IntuiText SorryText = {0,1,JAM1,6,3,0,"Continue",NULL};

				if(MACHINE_OKAY)
				{
					STATIC struct IntuiText BodyText = {0,1,JAM1,5,3,0,(STRPTR)"Kickstart 2.04 or higher required.",NULL};

					AutoRequest(NULL,&BodyText,NULL,&SorryText,0,0,309,46);
				}
				else
				{
					STATIC struct IntuiText BodyText = {0,1,JAM1,5,3,0,(UBYTE *)"MC68020 or higher required.",NULL};

					AutoRequest(NULL,&BodyText,NULL,&SorryText,0,0,253,46);
				}

				CloseLibrary((struct Library *)IntuitionBase);
			}

				/* Return the startup message and exit. */

			Forbid();

			ReplyMsg((struct Message *)WBenchMsg);
		}
	}
	
	return 0; /* stegerg: there was no return at all */

	AROS_PROCFUNC_EXIT
}
