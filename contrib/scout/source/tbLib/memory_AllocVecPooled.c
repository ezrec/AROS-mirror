#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "tblib.h"
#include "memory_internal.h"
#include <SDI/SDI_compiler.h>

#if defined(__GNUC__)
APTR AllocVecPooled( APTR poolHeader,
                     ULONG memSize )
{
    ULONG *result;

    if ((result = (ULONG *)AllocPooled(poolHeader, memSize + sizeof(ULONG) * 2)) != NULL) {
        result[0] = TB_POOL_MAGIC;
        result[1] = memSize + sizeof(ULONG) * 2;
        return &result[2];
    } else {
        return NULL;
    }
}
#else
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
#endif
