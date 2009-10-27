#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "tblib.h"
#include "memory_internal.h"
#include "SDI_compiler.h"

APTR ASM tbAllocVecPooled( REG(a0, APTR poolHeader),
                           REG(d0, ULONG memSize) )
{
    ULONG *result;

    if ((result = AsmAllocPooled(poolHeader, memSize + sizeof(ULONG) * 2, (struct Library *)SysBase)) != NULL) {
        result[0] = TB_POOL_MAGIC;
        result[1] = memSize + sizeof(ULONG) * 2;
        return &result[2];
    } else {
        return NULL;
    }
}

