#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "tblib.h"
#include "memory_internal.h"
#include <SDI/SDI_compiler.h>

APTR ASM tbAllocPooled( REG(a0, APTR poolHeader),
                        REG(d0, ULONG memSize) )
{
    return tbAllocVecPooled(poolHeader, memSize);
}

