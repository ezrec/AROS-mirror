/*
**	Memory.c
**
**	Memory management routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Main memory pool puddle size */

#define PUDDLE_SIZE 32768

	/* Memory allocation data */

STATIC struct SignalSemaphore	MemorySemaphore;
STATIC APTR			MemoryPool;

	/* MemorySetup():
	 *
	 *	Set up the main memory pool.
	 */

BOOL
MemorySetup()
{
	InitSemaphore(&MemorySemaphore);

	if(MemoryPool = AsmCreatePool(MEMF_PUBLIC | MEMF_ANY,PUDDLE_SIZE,PUDDLE_SIZE,SysBase))
		return(TRUE);
	else
		return(FALSE);
}

	/* MemoryCleanup():
	 *
	 *	Clean up the main memory pool.
	 */

VOID
MemoryCleanup()
{
	if(MemoryPool)
	{
		AsmDeletePool(MemoryPool,SysBase);

		MemoryPool = NULL;
	}
}

	/* FreeVecPooled(APTR Memory):
	 *
	 *	Free a memory chunk.
	 */

VOID
FreeVecPooled(APTR Memory)
{
	if(Memory && MemoryPool)
	{
		ULONG *Data = (ULONG *)Memory;

		ObtainSemaphore(&MemorySemaphore);

		AsmFreePooled(MemoryPool,&Data[-1],Data[-1],SysBase);

		ReleaseSemaphore(&MemorySemaphore);
	}
}

	/* AllocVecPooled(ULONG Size,ULONG Flags):
	 *
	 *	Allocate a memory chunk from the main memory pool and
	 *	remember its size.
	 */

APTR
AllocVecPooled(ULONG Size,ULONG Flags)
{
	if(MemoryPool && Size > 0)
	{
		ULONG *Data;

		ObtainSemaphore(&MemorySemaphore);
		Data = (ULONG *)AsmAllocPooled(MemoryPool,Size + sizeof(ULONG),SysBase);
		ReleaseSemaphore(&MemorySemaphore);

		if(Data)
		{
			*Data++ = Size + sizeof(ULONG);

				/* Zero the chunk if necessary */

			if(Flags & MEMF_CLEAR)
				memset(Data,0,Size);
		}

		return((APTR)Data);
	}
	else
	{
		struct Process *CurrentProcess;

			/* If a process is making the call,
			 * set the secondary error code to
			 * something meaningful.
			 */

		CurrentProcess = (struct Process *)FindTask(NULL);

		if(CurrentProcess->pr_Task.tc_Node.ln_Type == NT_PROCESS)
			SetIoErr(ERROR_NO_FREE_STORE);

		return(NULL);
	}
}
