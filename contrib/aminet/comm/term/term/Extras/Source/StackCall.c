#include <exec/types.h>
#include <exec/tasks.h>
#include <exec/memory.h>
#include <exec/execbase.h>

#include <dos/dos.h>
#include <dos/dosextens.h>

#include <proto/exec.h>
#include <proto/dos.h>

LONG StackCall(LONG * Success, 
               LONG StackSize, 
               LONG ArgCount, 
               LONG (* func)(unsigned long arg1, ...),
               unsigned long arg1, ...)
{
	LONG rc = FALSE;

#if 0
	ULONG * stack = AllocMem(StackSize, MEMF_PUBLIC|MEMF_CLEAR);

	if (NULL != stack) {
		struct StackSwapStruct SSS;
		ULONG * cur_stackptr = stack;
		SSS.stk_Lower = stack;
		SSS.stk_Upper = stack + StackSize;
		
		while (ArgCount > 0) {
			ArgCount--;
		}
		SSS.stk_Pointer = cur_stackptr;
		StackSwap(&SSS);
		
		StackSwap(&SSS);
		
		rc = TRUE;
	}
#endif
	rc = TRUE;
	/*
	 * Assumption is that there is enough memory!
	 */
	if (ArgCount <= 1) {
		func(arg1);
	}
	return rc;	
}


LONG StackSize(struct Task * Task)
{
	if (NULL == Task) {
		Task = FindTask(NULL);
	}
	
	/*
	 * Should cause a preemption here to have
	 * tc_SPReg set properly?!
	 */
	return (Task->tc_SPReg - Task->tc_SPLower);
}
