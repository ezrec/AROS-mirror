#ifndef AHI_Drivers_Common_DriverBase_h
#define AHI_Drivers_Common_DriverBase_h

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>

struct DriverBase
{
    struct Library  library;
    UWORD           pad;
    BPTR            seglist;
    struct Library* intuitionbase;
    struct Library* utilitybase;
};

#define IntuitionBase ((struct IntuitionBase*) AHIsubBase->intuitionbase)
#define UtilityBase   ((struct UtilityBase*)   AHIsubBase->utilitybase)


struct DriverData
{
};


BOOL
DriverInit( struct DriverBase* AHIsubBase );

VOID
DriverCleanup( struct DriverBase* AHIsubBase );

#endif /* AHI_Drivers_Common_DriverBase_h */
