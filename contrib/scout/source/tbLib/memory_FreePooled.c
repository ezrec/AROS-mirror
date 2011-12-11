#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "tblib.h"
#include "memory_internal.h"
#include <SDI/SDI_compiler.h>

void ASM tbFreePooled( REG(a0, APTR poolHeader),
                       REG(a1, APTR memory),
                       REG(d0, ULONG memSize) )
{
    tbFreeVecPooled(poolHeader, memory);
}


