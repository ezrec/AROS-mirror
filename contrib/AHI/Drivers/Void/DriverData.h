#ifndef AHI_Drivers_Void_DriverData_h
#define AHI_Drivers_Void_DriverData_h

#include <exec/libraries.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include "DriverBase.h"

struct VoidBase
{
    struct DriverBase driverbase;
    struct Library*   dosbase;
};

#define DRIVERBASE_SIZEOF (sizeof (struct VoidBase))


#define DOSBase           ((struct DosLibrary*)    VoidBase->dosbase)

struct VoidData
{
    struct DriverData   driverdata;
    UBYTE		flags;
    UBYTE		pad1;
    BYTE		mastersignal;
    BYTE		slavesignal;
    struct Process*	mastertask;
    struct Process*	slavetask;
    struct VoidBase*	ahisubbase;
    APTR		mixbuffer;
};


#endif /* AHI_Drivers_Void_DriverData_h */
