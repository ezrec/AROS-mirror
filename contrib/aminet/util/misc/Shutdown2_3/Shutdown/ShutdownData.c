#include "ShutdownGlobal.h"

struct ShutdownBase		*GlobalBase;

struct ExecBase			*SysBase;
struct DosLibrary		*DOSBase;
struct RxsLib			*RexxSysBase;
struct Library			*UtilityBase;
struct IntuitionBase		*IntuitionBase;
struct GfxBase			*GfxBase;
struct Library			*GadToolsBase;

struct SignalSemaphore		 DeviceWatchSemaphore;
BYTE				 DeviceInUse = FALSE;
BYTE				 DeviceProbablyInUse;

struct Process			*MainProcess	= NULL;
BYTE				 NotifySignal	= -1;

struct Process			*HandlerProcess;
