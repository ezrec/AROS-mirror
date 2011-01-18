#ifndef _BASE_H
#define _BASE_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

#ifndef _MACROS_H
#include "macros.h"
#endif

#include "BWin.mcc_rev.h"

/***************************************************************************/

extern struct ExecBase         *SysBase;
extern struct DosLibrary       *DOSBase;
#ifdef __AROS__
extern struct UtilityBase      *UtilityBase;
#else
extern struct Library          *UtilityBase;
#endif
extern struct IntuitionBase    *IntuitionBase;
extern struct LocaleBase       *LocaleBase;
extern struct Library          *MUIMasterBase;
extern struct GfxBase          *GfxBase;
extern struct PopupMenuBase    *PopupMenuBase;

extern struct Library          *lib_base;
extern ULONG                   lib_segList;
extern struct SignalSemaphore  lib_libSem;
extern struct Catalog          *lib_cat;
extern ULONG                   lib_flags;

extern Class                   *lib_sgad;
extern struct MUI_CustomClass  *lib_boopsi;
extern struct MUI_CustomClass  *lib_contents;
extern struct MUI_CustomClass  *lib_mcc;

enum
{
    BASEFLG_Init = 1<<0,
};

/***************************************************************************/

#endif /* _BASE_H */
