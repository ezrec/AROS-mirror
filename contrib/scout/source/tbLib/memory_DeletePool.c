#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "tblib.h"
#include "memory_internal.h"
#include <SDI/SDI_compiler.h>

void ASM tbDeletePool( REG(a0, void *poolHeader) )
{
    AsmDeletePool(poolHeader, (struct Library *)SysBase);
}

