#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

struct Library        *MUIMasterBase = NULL;
struct ExecBase       *SysBase       = NULL;
struct UtilityBase    *UtilityBase   = NULL;
struct DosLibrary     *DOSBase       = NULL;
struct GfxBase        *GfxBase       = NULL;
struct IntuitionBase  *IntuitionBase = NULL;

#define GETINTERFACE(iface, type, base)	TRUE
#define DROPINTERFACE(iface)

// in case the user didn't specify an own minimum
// muimaster version we increase it here.
#ifndef MASTERVERSION
#define MASTERVERSION MUIMASTER_VMIN
#endif
