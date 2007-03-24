
#include <proto/exec.h>
#include <string.h>
#include "base.h"

/****************************************************************************/

struct SignalSemaphore lib_libSem;

struct ExecBase        *SysBase = NULL;
struct DosLibrary      *DOSBase = NULL;
struct Library         *UtilityBase = NULL;
struct IntuitionBase   *IntuitionBase = NULL;
struct LocaleBase      *LocaleBase = NULL;
struct Library         *MUIMasterBase = NULL;
struct GfxBase         *GfxBase = NULL;
struct PopupMenuBase   *PopupMenuBase = NULL;

struct Library         *lib_base;
ULONG                  lib_segList;

struct Catalog         *lib_cat = NULL;
ULONG                  lib_flags = 0;

Class                  *lib_sgad = NULL;
struct MUI_CustomClass *lib_boopsi = NULL;
struct MUI_CustomClass *lib_contents = NULL;
struct MUI_CustomClass *lib_mcc = NULL;

char author[] = "\0$Author Alfonso Ranieri <alforan@tin.it>";
char libName[] = PRG;

/****************************************************************************/

BOOL ASM initBase(void);
void ASM freeBase(void);

/****************************************************************************/

struct Library * ASM SAVEDS
initLib (REG(a0) ULONG segList,REG(a6) APTR sys,REG(d0) struct Library *base)
{

#define SysBase sys

    memset(&lib_libSem,0,sizeof(struct SignalSemaphore));
    InitSemaphore(&lib_libSem);

#undef SysBase

    SysBase     = sys;
    lib_segList = segList;

    return lib_base = base;
}

/****************************************************************************/

struct Library * ASM SAVEDS
openLib(REG(a6) struct Library *base)
{
    register struct Library *res;

    ObtainSemaphore(&lib_libSem);

    base->lib_OpenCnt++;
    base->lib_Flags &= ~LIBF_DELEXP;

    if (!(lib_flags & BASEFLG_Init) && !initBase())
    {
        base->lib_OpenCnt--;
        res = NULL;
    }
    else res = base;

    ReleaseSemaphore(&lib_libSem);

    return res;
}

/****************************************************************************/

ULONG ASM SAVEDS
expungeLib(REG(a6) struct Library *base)
{
    register ULONG res;

    ObtainSemaphore(&lib_libSem);

    if (!base->lib_OpenCnt)
    {
        Remove((struct Node *)base);
        FreeMem((UBYTE *)base-base->lib_NegSize,base->lib_NegSize+base->lib_PosSize);

        res = lib_segList;
    }
    else
    {
        base->lib_Flags |= LIBF_DELEXP;
        res = NULL;
    }

    ReleaseSemaphore(&lib_libSem);

    return res;
}

/****************************************************************************/

ULONG ASM SAVEDS
closeLib(REG(a6) struct Library *base)
{
    register ULONG res = NULL;

    ObtainSemaphore(&lib_libSem);

    if (!--base->lib_OpenCnt)
    {
        freeBase();
        if (base->lib_Flags & LIBF_DELEXP) res = expungeLib(base);
    }

    ReleaseSemaphore(&lib_libSem);

    return res;
}

/****************************************************************************/
