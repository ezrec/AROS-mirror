#ifndef AHI_Drivers_Common_library_h
#define AHI_Drivers_Common_library_h

#include <exec/types.h>
#include "DriverBase.h"

extern const char  LibName[];
extern const char  LibIDString[];
extern const UWORD LibVersion;
extern const UWORD LibRevision;

void
ReqA( const char*        text,
      APTR               args,
      struct DriverBase* AHIsubBase );

#define Req(a0, args...) \
        ({ULONG _args[] = { args }; ReqA((a0), (APTR)_args, AHIsubBase);})

#endif /* AHI_Drivers_Common_library_h */
