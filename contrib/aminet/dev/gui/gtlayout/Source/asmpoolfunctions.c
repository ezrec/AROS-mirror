#include <exec/exec.h>
#include <exec/types.h>
#include <proto/exec.h>

#ifndef __AROS__
APTR ASM AsmCreatePool(REG(d0) ULONG MemFlags,REG(d1) ULONG PuddleSize,REG(d2) ULONG ThreshSize,REG(a6) struct ExecBase *SysBase)
{
	return CreatePool(MemFlags, PuddleSize, ThreshSize);
}
VOID ASM AsmDeletePool(REG(a0) APTR PoolHeader,REG(a6) struct ExecBase *SysBase)
{
	DeletePool(PoolHeader);
}
APTR ASM AsmAllocPooled(REG(a0) APTR PoolHeader,REG(d0) ULONG Size,REG(a6) struct ExecBase *SysBase)
{
	return AllocPooled(PoolHeader, Size);
}

VOID ASM AsmFreePooled(REG(a0) APTR PoolHeader,REG(a1) APTR Memory,REG(d0) ULONG MemSize,REG(a6) struct ExecBase *SysBase)
{
	FreePooled(PoolHeader, Memory, MemSize);
}
#endif
