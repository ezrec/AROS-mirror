
#include <proto/exec.h>
#include <string.h>
#include "base.h"

/****************************************************************************/

char author[] = "\0$Author Alfonso Ranieri <alforan@tin.it>";
char libName[] = PRG;
struct UrltextBase *UrltextBase;

/****************************************************************************/

BOOL ASM initBase(REG(a0) struct UrltextBase *base);
void ASM freeBase(REG(a0) struct UrltextBase *base);

/****************************************************************************/

struct UrltextBase *SAVEDS ASM
initLib (REG(a0) ULONG segList,REG(a6) APTR SysBase,REG(d0) struct UrltextBase *base)
{
    base->sysBase       = SysBase;
    base->segList       = segList;
    base->dosBase       = NULL;
    base->utilityBase   = NULL;
    base->intuitionBase = NULL;
    base->gfxBase       = NULL;
    base->localeBase    = NULL;
    base->muiMasterBase = NULL;
    base->openURLBase   = NULL;
    base->mcp           = NULL;
    base->cat           = NULL;
    base->flags         = 0;

    memset(&base->libSem,0,sizeof(struct SignalSemaphore));
    InitSemaphore(&base->libSem);

    return UrltextBase = base;
}

/****************************************************************************/

struct UrltextBase * SAVEDS ASM
openLib(REG(a6) struct UrltextBase *base)
{
    register struct ExecBase    *SysBase = base->sysBase;
    register struct UrltextBase *res = base;

    base->libNode.lib_OpenCnt++;
    base->libNode.lib_Flags &= ~LIBF_DELEXP;

    ObtainSemaphore(&base->libSem);
    if (!(base->flags & BASEFLG_INIT))
    {
        if (!initBase(base))
        {
            base->libNode.lib_OpenCnt--;
            res = NULL;
        }
    }
    ReleaseSemaphore(&base->libSem);

    return res;
}

/****************************************************************************/

ULONG SAVEDS ASM
expungeLib(REG(a6) struct UrltextBase *base)
{
    if (base->libNode.lib_OpenCnt==0)
    {
        register struct ExecBase    *SysBase = base->sysBase;
        register ULONG              segList = base->segList;

        Remove((struct Node *)base);
        FreeMem((UBYTE *)base-base->libNode.lib_NegSize,base->libNode.lib_NegSize + base->libNode.lib_PosSize);

        return segList;
    }

    base->libNode.lib_Flags |= LIBF_DELEXP;

    return NULL;
}

/****************************************************************************/

ULONG SAVEDS ASM
closeLib(REG(a6) struct UrltextBase *base)
{
    if (--base->libNode.lib_OpenCnt==0)
    {
        register struct ExecBase *SysBase = base->sysBase;

        ObtainSemaphore(&base->libSem);
        freeBase(base);
        ReleaseSemaphore(&base->libSem);

        if (base->libNode.lib_Flags & LIBF_DELEXP) return expungeLib(base);
    }

    return NULL;
}

/****************************************************************************/
