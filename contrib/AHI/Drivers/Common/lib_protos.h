#ifndef AHI_Drivers_Common_lib_protos_h
#define AHI_Drivers_Common_lib_protos_h

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

struct Library*
LibInit( struct Library*  library,
	 BPTR             seglist,
	 struct ExecBase* SysBase );

struct Library*
LibOpen( ULONG           version,
	 struct Library* LibBase );

struct Library*
LibClose( struct Library* LibBase );

BPTR LibExpunge( struct Library* LibBase );

ULONG LibNull( struct Library* LibBase );

#endif /* AHI_Drivers_Common_lib_protos_h */
