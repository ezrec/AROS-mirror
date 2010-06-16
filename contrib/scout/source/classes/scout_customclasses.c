#include "system_headers.h"

struct MUI_CustomClass *FontDisplayClass = NULL;
struct MUI_CustomClass *AboutWinClass = NULL;
struct MUI_CustomClass *AllocationsWinClass = NULL;
struct MUI_CustomClass *AssignsWinClass = NULL;
struct MUI_CustomClass *AudioModesWinClass = NULL;
struct MUI_CustomClass *AudioModesDetailWinClass = NULL;
struct MUI_CustomClass *CatalogsWinClass = NULL;
struct MUI_CustomClass *ClassesWinClass = NULL;
struct MUI_CustomClass *ClassesDetailWinClass = NULL;
struct MUI_CustomClass *CommandsWinClass = NULL;
struct MUI_CustomClass *CommoditiesWinClass = NULL;
struct MUI_CustomClass *CommoditiesDetailWinClass = NULL;
struct MUI_CustomClass *DevicesWinClass = NULL;
struct MUI_CustomClass *DevicesDetailWinClass = NULL;
struct MUI_CustomClass *DisassemblerWinClass = NULL;
struct MUI_CustomClass *ExpansionsWinClass = NULL;
struct MUI_CustomClass *ExpansionsDetailWinClass = NULL;
struct MUI_CustomClass *FlagsWinClass = NULL;
struct MUI_CustomClass *FontsWinClass = NULL;
struct MUI_CustomClass *FontsDetailWinClass = NULL;
struct MUI_CustomClass *FunctionsWinClass = NULL;
struct MUI_CustomClass *InputHandlersWinClass = NULL;
struct MUI_CustomClass *InputHandlersDetailWinClass = NULL;
struct MUI_CustomClass *InterruptsWinClass = NULL;
struct MUI_CustomClass *InterruptsDetailWinClass = NULL;
struct MUI_CustomClass *LibrariesWinClass = NULL;
struct MUI_CustomClass *LibrariesDetailWinClass = NULL;
struct MUI_CustomClass *LocksWinClass = NULL;
struct MUI_CustomClass *LowMemoryWinClass = NULL;
struct MUI_CustomClass *LowMemoryDetailWinClass = NULL;
struct MUI_CustomClass *MainWinClass = NULL;
struct MUI_CustomClass *MemoryWinClass = NULL;
struct MUI_CustomClass *MemoryDetailWinClass = NULL;
struct MUI_CustomClass *MonitorsWinClass = NULL;
struct MUI_CustomClass *MonitorsDetailWinClass = NULL;
struct MUI_CustomClass *MountsWinClass = NULL;
struct MUI_CustomClass *MountsDetailWinClass = NULL;
struct MUI_CustomClass *OOPWinClass = NULL;
struct MUI_CustomClass *ParentWinClass = NULL;
struct MUI_CustomClass *PatchesWinClass = NULL;
struct MUI_CustomClass *PortsWinClass = NULL;
struct MUI_CustomClass *PortsDetailWinClass = NULL;
struct MUI_CustomClass *PriorityWinClass = NULL;
struct MUI_CustomClass *ResetHandlersWinClass = NULL;
struct MUI_CustomClass *ResetHandlersDetailWinClass = NULL;
struct MUI_CustomClass *ResidentsWinClass = NULL;
struct MUI_CustomClass *ResidentsDetailWinClass = NULL;
struct MUI_CustomClass *ResourcesWinClass = NULL;
struct MUI_CustomClass *ResourcesDetailWinClass = NULL;
struct MUI_CustomClass *ScreenModesWinClass = NULL;
struct MUI_CustomClass *ScreenModesDetailWinClass = NULL;
struct MUI_CustomClass *SemaphoresWinClass = NULL;
struct MUI_CustomClass *SignalWinClass = NULL;
struct MUI_CustomClass *SystemWinClass = NULL;
struct MUI_CustomClass *TasksWinClass = NULL;
struct MUI_CustomClass *TasksDetailWinClass = NULL;
struct MUI_CustomClass *TimersWinClass = NULL;
struct MUI_CustomClass *VectorsWinClass = NULL;
struct MUI_CustomClass *WindowsWinClass = NULL;
struct MUI_CustomClass *WindowsDetailWinClass = NULL;

struct MUI_CustomClass *DisassemblerButtonClass = NULL;
struct MUI_CustomClass *FlagsButtonClass = NULL;
struct MUI_CustomClass *MonitorButtonClass = NULL;
struct MUI_CustomClass *PortButtonClass = NULL;
struct MUI_CustomClass *TaskButtonClass = NULL;

BOOL CreateCustomClasses( void )
{
    if ((FontDisplayClass            = MakeFontDisplayClass()))
    if ((ParentWinClass              = MakeParentWinClass()))
    if ((DisassemblerButtonClass     = MakeDisassemblerButtonClass()))
    if ((FlagsButtonClass            = MakeFlagsButtonClass()))
    if ((TaskButtonClass             = MakeTaskButtonClass()))
    if ((MonitorButtonClass	         = MakeMonitorButtonClass()))
    if ((PortButtonClass             = MakePortButtonClass()))
    if ((AboutWinClass               = MakeAboutWinClass()))
    if ((AllocationsWinClass         = MakeAllocationsWinClass()))
    if ((AssignsWinClass             = MakeAssignsWinClass()))
    if ((AudioModesWinClass          = MakeAudioModesWinClass()))
    if ((AudioModesDetailWinClass    = MakeAudioModesDetailWinClass()))
    if ((CatalogsWinClass            = MakeCatalogsWinClass()))
    if ((ClassesWinClass             = MakeClassesWinClass()))
    if ((ClassesDetailWinClass       = MakeClassesDetailWinClass()))
    if ((CommandsWinClass            = MakeCommandsWinClass()))
    if ((CommoditiesWinClass         = MakeCommoditiesWinClass()))
    if ((CommoditiesDetailWinClass   = MakeCommoditiesDetailWinClass()))
    if ((DevicesWinClass             = MakeDevicesWinClass()))
    if ((DevicesDetailWinClass       = MakeDevicesDetailWinClass()))
    if ((DisassemblerWinClass        = MakeDisassemblerWinClass()))
    if ((ExpansionsWinClass          = MakeExpansionsWinClass()))
    if ((ExpansionsDetailWinClass    = MakeExpansionsDetailWinClass()))
    if ((FlagsWinClass               = MakeFlagsWinClass()))
    if ((FontsWinClass               = MakeFontsWinClass()))
    if ((FontsDetailWinClass         = MakeFontsDetailWinClass()))
    if ((FunctionsWinClass           = MakeFunctionsWinClass()))
    if ((InputHandlersWinClass       = MakeInputHandlersWinClass()))
    if ((InputHandlersDetailWinClass = MakeInputHandlersDetailWinClass()))
    if ((InterruptsWinClass          = MakeInterruptsWinClass()))
    if ((InterruptsDetailWinClass    = MakeInterruptsDetailWinClass()))
    if ((LibrariesWinClass           = MakeLibrariesWinClass()))
    if ((LibrariesDetailWinClass     = MakeLibrariesDetailWinClass()))
    if ((LocksWinClass               = MakeLocksWinClass()))
    if ((LowMemoryWinClass           = MakeLowMemoryWinClass()))
    if ((LowMemoryDetailWinClass     = MakeLowMemoryDetailWinClass()))
    if ((MainWinClass                = MakeMainWinClass()))
    if ((MemoryWinClass              = MakeMemoryWinClass()))
    if ((MemoryDetailWinClass        = MakeMemoryDetailWinClass()))
    if ((MonitorsWinClass            = MakeMonitorsWinClass()))
    if ((MonitorsDetailWinClass	     = MakeMonitorsDetailWinClass()))
    if ((MountsWinClass              = MakeMountsWinClass()))
    if ((MountsDetailWinClass        = MakeMountsDetailWinClass()))
    if ((OOPWinClass                 = MakeOOPWinClass()))
    if ((PatchesWinClass             = MakePatchesWinClass()))
    if ((PortsWinClass               = MakePortsWinClass()))
    if ((PortsDetailWinClass         = MakePortsDetailWinClass()))
    if ((PriorityWinClass            = MakePriorityWinClass()))
    if ((ResetHandlersWinClass       = MakeResetHandlersWinClass()))
    if ((ResetHandlersDetailWinClass = MakeResetHandlersDetailWinClass()))
    if ((ResidentsWinClass           = MakeResidentsWinClass()))
    if ((ResidentsDetailWinClass     = MakeResidentsDetailWinClass()))
    if ((ResourcesWinClass           = MakeResourcesWinClass()))
    if ((ResourcesDetailWinClass     = MakeResourcesDetailWinClass()))
    if ((ScreenModesWinClass         = MakeScreenModesWinClass()))
    if ((ScreenModesDetailWinClass   = MakeScreenModesDetailWinClass()))
    if ((SemaphoresWinClass          = MakeSemaphoresWinClass()))
    if ((SignalWinClass              = MakeSignalWinClass()))
    if ((SystemWinClass              = MakeSystemWinClass()))
    if ((TasksWinClass               = MakeTasksWinClass()))
    if ((TasksDetailWinClass         = MakeTasksDetailWinClass()))
    if ((TimersWinClass              = MakeTimersWinClass()))
    if ((VectorsWinClass             = MakeVectorsWinClass()))
    if ((WindowsWinClass             = MakeWindowsWinClass()))
    if ((WindowsDetailWinClass       = MakeWindowsDetailWinClass()))
    {
        return TRUE;
    }

    return FALSE;
}

void DeleteCustomClasses( void )
{
    if (AboutWinClass)                MUI_DeleteCustomClass(AboutWinClass);
    if (AllocationsWinClass)          MUI_DeleteCustomClass(AllocationsWinClass);
    if (AssignsWinClass)              MUI_DeleteCustomClass(AssignsWinClass);
    if (AudioModesWinClass)           MUI_DeleteCustomClass(AudioModesWinClass);
    if (AudioModesDetailWinClass)     MUI_DeleteCustomClass(AudioModesDetailWinClass);
    if (CatalogsWinClass)             MUI_DeleteCustomClass(CatalogsWinClass);
    if (ClassesWinClass)              MUI_DeleteCustomClass(ClassesWinClass);
    if (ClassesDetailWinClass)        MUI_DeleteCustomClass(ClassesDetailWinClass);
    if (CommandsWinClass)             MUI_DeleteCustomClass(CommandsWinClass);
    if (CommoditiesWinClass)          MUI_DeleteCustomClass(CommoditiesWinClass);
    if (CommoditiesDetailWinClass)    MUI_DeleteCustomClass(CommoditiesDetailWinClass);
    if (DevicesWinClass)              MUI_DeleteCustomClass(DevicesWinClass);
    if (DevicesDetailWinClass)        MUI_DeleteCustomClass(DevicesDetailWinClass);
    if (DisassemblerWinClass)         MUI_DeleteCustomClass(DisassemblerWinClass);
    if (ExpansionsWinClass)           MUI_DeleteCustomClass(ExpansionsWinClass);
    if (ExpansionsDetailWinClass)     MUI_DeleteCustomClass(ExpansionsDetailWinClass);
    if (FlagsWinClass)                MUI_DeleteCustomClass(FlagsWinClass);
    if (FontsWinClass)                MUI_DeleteCustomClass(FontsWinClass);
    if (FontsDetailWinClass)          MUI_DeleteCustomClass(FontsDetailWinClass);
    if (FunctionsWinClass)            MUI_DeleteCustomClass(FunctionsWinClass);
    if (InputHandlersWinClass)        MUI_DeleteCustomClass(InputHandlersWinClass);
    if (InputHandlersDetailWinClass)  MUI_DeleteCustomClass(InputHandlersDetailWinClass);
    if (InterruptsWinClass)           MUI_DeleteCustomClass(InterruptsWinClass);
    if (InterruptsDetailWinClass)     MUI_DeleteCustomClass(InterruptsDetailWinClass);
    if (LibrariesWinClass)            MUI_DeleteCustomClass(LibrariesWinClass);
    if (LibrariesDetailWinClass)      MUI_DeleteCustomClass(LibrariesDetailWinClass);
    if (LocksWinClass)                MUI_DeleteCustomClass(LocksWinClass);
    if (LowMemoryWinClass)            MUI_DeleteCustomClass(LowMemoryWinClass);
    if (LowMemoryDetailWinClass)      MUI_DeleteCustomClass(LowMemoryDetailWinClass);
    if (MainWinClass)                 MUI_DeleteCustomClass(MainWinClass);
    if (MemoryWinClass)               MUI_DeleteCustomClass(MemoryWinClass);
    if (MemoryDetailWinClass)         MUI_DeleteCustomClass(MemoryDetailWinClass);
    if (MonitorsWinClass)             MUI_DeleteCustomClass(MonitorsWinClass);
    if (MonitorsDetailWinClass)       MUI_DeleteCustomClass(MonitorsDetailWinClass);
    if (MountsWinClass)               MUI_DeleteCustomClass(MountsWinClass);
    if (MountsDetailWinClass)         MUI_DeleteCustomClass(MountsDetailWinClass);
    if (OOPWinClass)                  MUI_DeleteCustomClass(OOPWinClass);
    if (PatchesWinClass)              MUI_DeleteCustomClass(PatchesWinClass);
    if (PortsWinClass)                MUI_DeleteCustomClass(PortsWinClass);
    if (PortsDetailWinClass)          MUI_DeleteCustomClass(PortsDetailWinClass);
    if (PriorityWinClass)             MUI_DeleteCustomClass(PriorityWinClass);
    if (ResetHandlersWinClass)        MUI_DeleteCustomClass(ResetHandlersWinClass);
    if (ResetHandlersDetailWinClass)  MUI_DeleteCustomClass(ResetHandlersDetailWinClass);
    if (ResidentsWinClass)            MUI_DeleteCustomClass(ResidentsWinClass);
    if (ResidentsDetailWinClass)      MUI_DeleteCustomClass(ResidentsDetailWinClass);
    if (ResourcesWinClass)            MUI_DeleteCustomClass(ResourcesWinClass);
    if (ResourcesDetailWinClass)      MUI_DeleteCustomClass(ResourcesDetailWinClass);
    if (ScreenModesWinClass)          MUI_DeleteCustomClass(ScreenModesWinClass);
    if (ScreenModesDetailWinClass)    MUI_DeleteCustomClass(ScreenModesDetailWinClass);
    if (SemaphoresWinClass)           MUI_DeleteCustomClass(SemaphoresWinClass);
    if (SignalWinClass)               MUI_DeleteCustomClass(SignalWinClass);
    if (SystemWinClass)               MUI_DeleteCustomClass(SystemWinClass);
    if (TasksWinClass)                MUI_DeleteCustomClass(TasksWinClass);
    if (TasksDetailWinClass)          MUI_DeleteCustomClass(TasksDetailWinClass);
    if (TimersWinClass)               MUI_DeleteCustomClass(TimersWinClass);
    if (VectorsWinClass)              MUI_DeleteCustomClass(VectorsWinClass);
    if (WindowsWinClass)              MUI_DeleteCustomClass(WindowsWinClass);
    if (WindowsDetailWinClass)        MUI_DeleteCustomClass(WindowsDetailWinClass);

    if (PortButtonClass)              MUI_DeleteCustomClass(PortButtonClass);
    if (TaskButtonClass)              MUI_DeleteCustomClass(TaskButtonClass);
    if (FlagsButtonClass)             MUI_DeleteCustomClass(FlagsButtonClass);
    if (DisassemblerButtonClass)      MUI_DeleteCustomClass(DisassemblerButtonClass);
    if (ParentWinClass)               MUI_DeleteCustomClass(ParentWinClass);
    if (FontDisplayClass)             MUI_DeleteCustomClass(FontDisplayClass);
    if (MonitorButtonClass)           MUI_DeleteCustomClass(MonitorButtonClass);
}
