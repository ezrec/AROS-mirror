#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#include "macros.h"
#include "BWin.mcp_rev.h"

/***************************************************************************/

extern struct ExecBase        *SysBase;
extern struct DosLibrary      *DOSBase;
#ifdef __AROS__
extern struct UtilityBase     *UtilityBase;
#else
extern struct Library         *UtilityBase;
#endif
extern struct IntuitionBase   *IntuitionBase;
extern struct LocaleBase      *LocaleBase;
extern struct Library         *MUIMasterBase;

extern struct Library         *lib_base;
extern ULONG                  lib_segList;
extern struct SignalSemaphore lib_libSem;
extern struct Catalog         *lib_cat;
extern ULONG                  lib_flags;
extern struct MUI_CustomClass *lib_shape;
extern struct MUI_CustomClass *lib_class;

/***************************************************************************/

enum
{
    BASEFLG_Init  = 1<<0,
    //BASEFLG_Avoid = 1<<1,
};

/***************************************************************************/

BOOL ASM initBase(void);
void ASM freeBase(void);

/***************************************************************************/
