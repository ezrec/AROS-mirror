#ifndef AHI_Drivers_Device_DriverData_h
#define AHI_Drivers_Device_DriverData_h

#include <devices/ahi.h>
#include <exec/libraries.h>

#include "DriverBase.h"

struct DeviceBase
{
    struct DriverBase driverbase;
    struct Library*   dosbase;
};

#define DRIVERBASE_SIZEOF (sizeof (struct DeviceBase))
#define DOSBase  ((struct DosLibrary*) DeviceBase->dosbase)

struct DeviceData
{
    struct DriverData   driverdata;
    BYTE		mastersignal;
    BYTE		slavesignal;
    struct Process*	mastertask;
    struct Process*	slavetask;
    struct DeviceBase*	ahisubbase;
    APTR                mixbuffers[ 2 ];
    ULONG               unit;
};


#endif /* AHI_Drivers_Device_DriverData_h */
