/*
**	FixPath.c
**
**	Fix the current Process search patch list by faking a CLI
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* This is how a linked list of directory search paths looks like. */

struct Path
{
	BPTR path_Next;	/* Pointer to next entry */
	BPTR path_Lock;	/* The drawer in question; may be NULL */
};

	/* ClonePath(BPTR StartPath):
	 *
	 *	Make a copy of the command search path attached to a
	 *	CLI process.
	 */

STATIC BPTR
ClonePath(BPTR StartPath)
{
	struct Path *First,*Last,*List,*New;

	for(List = BADDR(StartPath), First = Last = NULL ; List ; List = BADDR(List->path_Next))
	{
		if(List->path_Lock)
		{
			if(New = AllocVec(sizeof(struct Path),MEMF_ANY))
			{
				if(New->path_Lock = DupLock(List->path_Lock))
				{
					New->path_Next = BNULL;

					if(Last)
						Last->path_Next = MKBADDR(New);

					if(!First)
						First = New;

					Last = New;
				}
				else
				{
					FreeVec(New);
					break;
				}
			}
			else
				break;
		}
	}

	return(MKBADDR(First));
}

	/* AttachCLI(struct WBStartup *Startup):
	 *
	 *	Attach a valid CLI structure to the current process. Requires a
	 *	Workbench startup message whose command search path it will
	 *	duplicate.
	 */

VOID
AttachCLI(struct WBStartup *Startup)
{
	struct CommandLineInterface *DestCLI;

		/* Note: FreeDosObject can't free it, but the DOS */
		/*       process termination code can. */

	if(DestCLI = AllocDosObject(DOS_CLI,NULL))
	{
		struct MsgPort *ReplyPort;
		struct Process *Dest;

		DestCLI->cli_DefaultStack = 4096 / sizeof(ULONG);

		Dest = (struct Process *)FindTask(NULL);

		Dest->pr_CLI	 = MKBADDR(DestCLI);
		Dest->pr_Flags 	|= PRF_FREECLI;			/* Mark for cleanup */

		Forbid();

		ReplyPort = Startup->sm_Message.mn_ReplyPort;

			/* Does the reply port data point somewhere sensible? */

		if(ReplyPort && (ReplyPort->mp_Flags & PF_ACTION) == PA_SIGNAL && TypeOfMem(ReplyPort->mp_SigTask))
		{
			struct Process *Father;

				/* Get the address of the process that sent the startup message */

			Father = (struct Process *)ReplyPort->mp_SigTask;

				/* Just to be on the safe side... */

			if(Father->pr_Task.tc_Node.ln_Type == NT_PROCESS)
			{
				struct CommandLineInterface	*SourceCLI;

					/* Is there a CLI attached? */

				if(SourceCLI = BADDR(Father->pr_CLI))
				{
					STRPTR Prompt;

						/* Clone the other CLI data. */

					if(Prompt = (STRPTR)BADDR(SourceCLI->cli_Prompt))
						SetPrompt(&Prompt[1]);

					if(SourceCLI->cli_DefaultStack > DestCLI->cli_DefaultStack)
						DestCLI->cli_DefaultStack = SourceCLI->cli_DefaultStack;

					if(SourceCLI->cli_CommandDir)
						DestCLI->cli_CommandDir = ClonePath(SourceCLI->cli_CommandDir);
				}
			}
		}

		Permit();
	}
}
