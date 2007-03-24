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

#ifndef _MACROS_H
#include "macros.h"
#endif

#include "Urltext.mcp_rev.h"

/***************************************************************************/

struct UrltextBase
{
    struct Library          libNode;
    ULONG                   segList;
    struct ExecBase         *sysBase;
    struct DosLibrary       *dosBase;
    struct Library          *utilityBase;
    struct IntuitionBase    *intuitionBase;
    struct Library          *localeBase;
    struct Library          *muiMasterBase;
    struct Library          *openURLBase;
    struct Library          *gfxBase;
    struct SignalSemaphore  libSem;
    struct MUI_CustomClass  *mcp;
    struct Catalog          *cat;
    ULONG                   flags;
};

#define BASEFLG_INIT    0x00000001

/***************************************************************************/

#endif /* _BASE_H */
