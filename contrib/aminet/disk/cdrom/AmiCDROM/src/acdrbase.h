#ifndef ACDRBASE_H
#define ACDRBASE_H

#include <dos/bptr.h>
#include <exec/execbase.h>
#include <exec/devices.h>
#include <intuition/intuitionbase.h>

struct ACDRBase {
	struct Device device;
	BPTR seglist;
	struct ExecBase *SysBase;
	struct DOSBase *DOSBase;
	struct Library *UtilityBase;
	struct IntuitionBase *IntuitionBase;
	struct Library *IconBase;
	struct Library *WorkbenchBase;
	struct MsgPort port;
	struct MsgPort rport;
};

#define expunge() AROS_LC0(BPTR, expunge, struct ACDRBase *, acdrbase, 3, acdr)

#endif

