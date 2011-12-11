#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "tblib.h"
#include "memory_internal.h"
#include <SDI/SDI_compiler.h>

#if defined(__GNUC__)
void FreeVecPooled( APTR poolHeader,
                    APTR memory )
{
    ULONG *tmp = memory;

    if (tmp && tmp[-2] == TB_POOL_MAGIC && tmp[-1] != 0) {
        FreePooled(poolHeader, &tmp[-2], tmp[-1]);
    }
}
#else
void ASM tbFreeVecPooled( REG(a0, APTR poolHeader),
                          REG(a1, APTR memory) )
{
    ULONG *tmp = memory;

    if (tmp && tmp[-2] == TB_POOL_MAGIC && tmp[-1] != 0) {
        AsmFreePooled(poolHeader, &tmp[-2], tmp[-1], (struct Library *)SysBase);
    }
}
#endif
