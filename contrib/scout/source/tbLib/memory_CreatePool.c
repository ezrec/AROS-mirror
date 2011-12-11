#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "tblib.h"
#include "memory_internal.h"
#include <SDI/SDI_compiler.h>

void * ASM tbCreatePool( REG(d0, ULONG flags),
                         REG(d1, ULONG puddleSize),
                         REG(d2, ULONG threshSize) )
{
    return AsmCreatePool(flags, puddleSize, threshSize, (struct Library *)SysBase);
}

