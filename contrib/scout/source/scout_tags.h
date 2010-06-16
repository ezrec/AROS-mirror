#ifndef SCOUT_TAGS_H
#define SCOUT_TAGS_H 1

#define TAGBASE_TBOECKEL                            (TAG_USER | (0x2677 << 16))

#define MUIM_AllocationsWin_Update                  (TAGBASE_TBOECKEL | 0x1000)
#define MUIM_AllocationsWin_Print                   (TAGBASE_TBOECKEL | 0x1001)

#define MUIM_AssignsWin_Update                      (TAGBASE_TBOECKEL | 0x1010)
#define MUIM_AssignsWin_Print                       (TAGBASE_TBOECKEL | 0x1011)
#define MUIM_AssignsWin_Remove                      (TAGBASE_TBOECKEL | 0x1012)
#define MUIM_AssignsWin_ListChange                  (TAGBASE_TBOECKEL | 0x1013)

#define MUIM_ClassesWin_Update                      (TAGBASE_TBOECKEL | 0x1020)
#define MUIM_ClassesWin_Print                       (TAGBASE_TBOECKEL | 0x1021)
#define MUIM_ClassesWin_Remove                      (TAGBASE_TBOECKEL | 0x1022)
#define MUIM_ClassesWin_More                        (TAGBASE_TBOECKEL | 0x1023)
#define MUIM_ClassesWin_ListChange                  (TAGBASE_TBOECKEL | 0x1024)

#define MUIM_CommoditiesWin_Update                  (TAGBASE_TBOECKEL | 0x1030)
#define MUIM_CommoditiesWin_Print                   (TAGBASE_TBOECKEL | 0x1031)
#define MUIM_CommoditiesWin_Remove                  (TAGBASE_TBOECKEL | 0x1033)
#define MUIM_CommoditiesWin_Priority                (TAGBASE_TBOECKEL | 0x1032)
#define MUIM_CommoditiesWin_More                    (TAGBASE_TBOECKEL | 0x1034)
#define MUIM_CommoditiesWin_ListChange              (TAGBASE_TBOECKEL | 0x1035)
#define MUIM_CommoditiesWin_CxCommand               (TAGBASE_TBOECKEL | 0x1036)

#define MUIM_DevicesWin_Update                      (TAGBASE_TBOECKEL | 0x1040)
#define MUIM_DevicesWin_Print                       (TAGBASE_TBOECKEL | 0x1041)
#define MUIM_DevicesWin_Remove                      (TAGBASE_TBOECKEL | 0x1042)
#define MUIM_DevicesWin_Priority                    (TAGBASE_TBOECKEL | 0x1043)
#define MUIM_DevicesWin_More                        (TAGBASE_TBOECKEL | 0x1044)
#define MUIM_DevicesWin_Functions                   (TAGBASE_TBOECKEL | 0x1045)
#define MUIM_DevicesWin_ListChange                  (TAGBASE_TBOECKEL | 0x1046)

#define MUIM_ExpansionsWin_Update                   (TAGBASE_TBOECKEL | 0x1050)
#define MUIM_ExpansionsWin_Print                    (TAGBASE_TBOECKEL | 0x1051)
#define MUIM_ExpansionsWin_More                     (TAGBASE_TBOECKEL | 0x1052)
#define MUIM_ExpansionsWin_ListChange               (TAGBASE_TBOECKEL | 0x1053)

#define MUIM_FontsWin_Update                        (TAGBASE_TBOECKEL | 0x1060)
#define MUIM_FontsWin_Print                         (TAGBASE_TBOECKEL | 0x1061)
#define MUIM_FontsWin_Close                         (TAGBASE_TBOECKEL | 0x1062)
#define MUIM_FontsWin_Remove                        (TAGBASE_TBOECKEL | 0x1063)
#define MUIM_FontsWin_More                          (TAGBASE_TBOECKEL | 0x1064)
#define MUIM_FontsWin_ListChange                    (TAGBASE_TBOECKEL | 0x1065)

#define MUIM_InputHandlersWin_Update                (TAGBASE_TBOECKEL | 0x1070)
#define MUIM_InputHandlersWin_Print                 (TAGBASE_TBOECKEL | 0x1071)
#define MUIM_InputHandlersWin_Remove                (TAGBASE_TBOECKEL | 0x1072)
#define MUIM_InputHandlersWin_Priority              (TAGBASE_TBOECKEL | 0x1073)
#define MUIM_InputHandlersWin_More                  (TAGBASE_TBOECKEL | 0x1074)
#define MUIM_InputHandlersWin_ListChange            (TAGBASE_TBOECKEL | 0x1075)

#define MUIM_InterruptsWin_Update                   (TAGBASE_TBOECKEL | 0x1080)
#define MUIM_InterruptsWin_Print                    (TAGBASE_TBOECKEL | 0x1081)
#define MUIM_InterruptsWin_Remove                   (TAGBASE_TBOECKEL | 0x1082)
#define MUIM_InterruptsWin_More                     (TAGBASE_TBOECKEL | 0x1083)
#define MUIM_InterruptsWin_ListChange               (TAGBASE_TBOECKEL | 0x1084)

#define MUIM_LibrariesWin_Update                    (TAGBASE_TBOECKEL | 0x1090)
#define MUIM_LibrariesWin_Print                     (TAGBASE_TBOECKEL | 0x1091)
#define MUIM_LibrariesWin_Remove                    (TAGBASE_TBOECKEL | 0x1092)
#define MUIM_LibrariesWin_Priority                  (TAGBASE_TBOECKEL | 0x1093)
#define MUIM_LibrariesWin_Close                     (TAGBASE_TBOECKEL | 0x1094)
#define MUIM_LibrariesWin_More                      (TAGBASE_TBOECKEL | 0x1095)
#define MUIM_LibrariesWin_Functions                 (TAGBASE_TBOECKEL | 0x1096)
#define MUIM_LibrariesWin_ListChange                (TAGBASE_TBOECKEL | 0x1097)

#define MUIM_LocksWin_Update                        (TAGBASE_TBOECKEL | 0x10a0)
#define MUIM_LocksWin_Print                         (TAGBASE_TBOECKEL | 0x10a1)
#define MUIM_LocksWin_Remove                        (TAGBASE_TBOECKEL | 0x10a2)
#define MUIM_LocksWin_ListChange                    (TAGBASE_TBOECKEL | 0x10a3)

#define MUIM_LowMemoryWin_Update                    (TAGBASE_TBOECKEL | 0x10b0)
#define MUIM_LowMemoryWin_Print                     (TAGBASE_TBOECKEL | 0x10b1)
#define MUIM_LowMemoryWin_Cause                     (TAGBASE_TBOECKEL | 0x10b2)
#define MUIM_LowMemoryWin_Remove                    (TAGBASE_TBOECKEL | 0x10b3)
#define MUIM_LowMemoryWin_Priority                  (TAGBASE_TBOECKEL | 0x10b4)
#define MUIM_LowMemoryWin_More                      (TAGBASE_TBOECKEL | 0x10b5)
#define MUIM_LowMemoryWin_ListChange                (TAGBASE_TBOECKEL | 0x10b6)

#define MUIM_MemoryWin_Update                       (TAGBASE_TBOECKEL | 0x10c0)
#define MUIM_MemoryWin_Print                        (TAGBASE_TBOECKEL | 0x10c1)
#define MUIM_MemoryWin_Priority                     (TAGBASE_TBOECKEL | 0x10c2)
#define MUIM_MemoryWin_More                         (TAGBASE_TBOECKEL | 0x10c3)
#define MUIM_MemoryWin_ListChange                   (TAGBASE_TBOECKEL | 0x10c4)

#define MUIM_MountsWin_Update                       (TAGBASE_TBOECKEL | 0x10d0)
#define MUIM_MountsWin_Print                        (TAGBASE_TBOECKEL | 0x10d1)
#define MUIM_MountsWin_More                         (TAGBASE_TBOECKEL | 0x10d2)
#define MUIM_MountsWin_ListChange                   (TAGBASE_TBOECKEL | 0x10d3)

#define MUIM_PortsWin_Update                        (TAGBASE_TBOECKEL | 0x10e0)
#define MUIM_PortsWin_Print                         (TAGBASE_TBOECKEL | 0x10e1)
#define MUIM_PortsWin_Remove                        (TAGBASE_TBOECKEL | 0x10e2)
#define MUIM_PortsWin_Priority                      (TAGBASE_TBOECKEL | 0x10e3)
#define MUIM_PortsWin_More                          (TAGBASE_TBOECKEL | 0x10e4)
#define MUIM_PortsWin_ListChange                    (TAGBASE_TBOECKEL | 0x10e5)

#define MUIM_ResidentsWin_Update                    (TAGBASE_TBOECKEL | 0x10f0)
#define MUIM_ResidentsWin_Print                     (TAGBASE_TBOECKEL | 0x10f1)
#define MUIM_ResidentsWin_More                      (TAGBASE_TBOECKEL | 0x10f2)
#define MUIM_ResidentsWin_ListChange                (TAGBASE_TBOECKEL | 0x10f3)

#define MUIM_CommandsWin_Update                     (TAGBASE_TBOECKEL | 0x1100)
#define MUIM_CommandsWin_Print                      (TAGBASE_TBOECKEL | 0x1101)
#define MUIM_CommandsWin_Remove                     (TAGBASE_TBOECKEL | 0x1102)
#define MUIM_CommandsWin_ListChange                 (TAGBASE_TBOECKEL | 0x1103)

#define MUIM_ResourcesWin_Update                    (TAGBASE_TBOECKEL | 0x1110)
#define MUIM_ResourcesWin_Print                     (TAGBASE_TBOECKEL | 0x1111)
#define MUIM_ResourcesWin_Remove                    (TAGBASE_TBOECKEL | 0x1112)
#define MUIM_ResourcesWin_Priority                  (TAGBASE_TBOECKEL | 0x1113)
#define MUIM_ResourcesWin_More                      (TAGBASE_TBOECKEL | 0x1114)
#define MUIM_ResourcesWin_Functions                 (TAGBASE_TBOECKEL | 0x1115)
#define MUIM_ResourcesWin_ListChange                (TAGBASE_TBOECKEL | 0x1116)

#define MUIM_ScreenModesWin_Update                  (TAGBASE_TBOECKEL | 0x1120)
#define MUIM_ScreenModesWin_Print                   (TAGBASE_TBOECKEL | 0x1121)
#define MUIM_ScreenModesWin_More                    (TAGBASE_TBOECKEL | 0x1122)
#define MUIM_ScreenModesWin_ListChange              (TAGBASE_TBOECKEL | 0x1123)

#define MUIM_SemaphoresWin_Update                   (TAGBASE_TBOECKEL | 0x1130)
#define MUIM_SemaphoresWin_Print                    (TAGBASE_TBOECKEL | 0x1131)
#define MUIM_SemaphoresWin_Obtain                   (TAGBASE_TBOECKEL | 0x1132)
#define MUIM_SemaphoresWin_Release                  (TAGBASE_TBOECKEL | 0x1133)
#define MUIM_SemaphoresWin_Remove                   (TAGBASE_TBOECKEL | 0x1134)
#define MUIM_SemaphoresWin_Priority                 (TAGBASE_TBOECKEL | 0x1135)
#define MUIM_SemaphoresWin_ListChange               (TAGBASE_TBOECKEL | 0x1136)

#define MUIM_SystemWin_Update                       (TAGBASE_TBOECKEL | 0x1140)
#define MUIM_SystemWin_Print                        (TAGBASE_TBOECKEL | 0x1141)

#define MUIM_TasksWin_Update                        (TAGBASE_TBOECKEL | 0x1150)
#define MUIM_TasksWin_Print                         (TAGBASE_TBOECKEL | 0x1151)
#define MUIM_TasksWin_Remove                        (TAGBASE_TBOECKEL | 0x1152)
#define MUIM_TasksWin_Priority                      (TAGBASE_TBOECKEL | 0x1153)
#define MUIM_TasksWin_More                          (TAGBASE_TBOECKEL | 0x1154)
#define MUIM_TasksWin_ListChange                    (TAGBASE_TBOECKEL | 0x1155)
#define MUIM_TasksWin_Signal                        (TAGBASE_TBOECKEL | 0x1156)
#define MUIM_TasksWin_Break                         (TAGBASE_TBOECKEL | 0x1157)
#define MUIM_TasksWin_Freeze                        (TAGBASE_TBOECKEL | 0x1158)
#define MUIM_TasksWin_Activate                      (TAGBASE_TBOECKEL | 0x1159)
#define MUIM_TasksWin_CPUCheckChange                (TAGBASE_TBOECKEL | 0x115a)
#define MUIM_TasksWin_CPUCheck                      (TAGBASE_TBOECKEL | 0x115b)
#define MUIM_TasksWin_CPUCheckQuick                 (TAGBASE_TBOECKEL | 0x115c)

#define MUIM_TimersWin_Update                       (TAGBASE_TBOECKEL | 0x1160)
#define MUIM_TimersWin_Print                        (TAGBASE_TBOECKEL | 0x1161)
#define MUIM_TimersWin_Abort                        (TAGBASE_TBOECKEL | 0x1162)
#define MUIM_TimersWin_ListChange                   (TAGBASE_TBOECKEL | 0x1163)

#define MUIM_VectorsWin_Update                      (TAGBASE_TBOECKEL | 0x1170)
#define MUIM_VectorsWin_Print                       (TAGBASE_TBOECKEL | 0x1171)

#define MUIM_WindowsWin_Update                      (TAGBASE_TBOECKEL | 0x1180)
#define MUIM_WindowsWin_Print                       (TAGBASE_TBOECKEL | 0x1181)
#define MUIM_WindowsWin_Close                       (TAGBASE_TBOECKEL | 0x1182)
#define MUIM_WindowsWin_ToFront                     (TAGBASE_TBOECKEL | 0x1183)
#define MUIM_WindowsWin_MakeVisible                 (TAGBASE_TBOECKEL | 0x1184)
#define MUIM_WindowsWin_More                        (TAGBASE_TBOECKEL | 0x1185)
#define MUIM_WindowsWin_ListChange                  (TAGBASE_TBOECKEL | 0x1186)

#define MUIM_PatchesWin_Update                      (TAGBASE_TBOECKEL | 0x1190)
#define MUIM_PatchesWin_Print                       (TAGBASE_TBOECKEL | 0x1191)
#define MUIM_PatchesWin_Enable                      (TAGBASE_TBOECKEL | 0x1192)
#define MUIM_PatchesWin_Disable                     (TAGBASE_TBOECKEL | 0x1193)
#define MUIM_PatchesWin_ListChange                  (TAGBASE_TBOECKEL | 0x1194)

#define MUIM_CatalogsWin_Update                     (TAGBASE_TBOECKEL | 0x11a0)
#define MUIM_CatalogsWin_Print                      (TAGBASE_TBOECKEL | 0x11a1)
#define MUIM_CatalogsWin_ListChange                 (TAGBASE_TBOECKEL | 0x11a2)

#define MUIM_AudioModesWin_Update                   (TAGBASE_TBOECKEL | 0x11b0)
#define MUIM_AudioModesWin_Print                    (TAGBASE_TBOECKEL | 0x11b1)
#define MUIM_AudioModesWin_More                     (TAGBASE_TBOECKEL | 0x11b2)
#define MUIM_AudioModesWin_ListChange               (TAGBASE_TBOECKEL | 0x11b3)

#define MUIM_ResetHandlersWin_Update                (TAGBASE_TBOECKEL | 0x11c0)
#define MUIM_ResetHandlersWin_Print                 (TAGBASE_TBOECKEL | 0x11c1)
#define MUIM_ResetHandlersWin_Remove                (TAGBASE_TBOECKEL | 0x11c2)
#define MUIM_ResetHandlersWin_Priority              (TAGBASE_TBOECKEL | 0x11c3)
#define MUIM_ResetHandlersWin_More                  (TAGBASE_TBOECKEL | 0x11c4)
#define MUIM_ResetHandlersWin_ListChange            (TAGBASE_TBOECKEL | 0x11c5)

#define MUIM_MonitorsWin_Update			    (TAGBASE_TBOECKEL | 0x11d0)
#define MUIM_MonitorsWin_Print                      (TAGBASE_TBOECKEL | 0x11d1)
#define MUIM_MonitorsWin_Remove                     (TAGBASE_TBOECKEL | 0x11d2)
#define MUIM_MonitorsWin_Priority                   (TAGBASE_TBOECKEL | 0x11d3)
#define MUIM_MonitorsWin_More                       (TAGBASE_TBOECKEL | 0x11d4)
#define MUIM_MonitorsWin_ListChange                 (TAGBASE_TBOECKEL | 0x11d5)

#define MUIM_OOPWin_Update                          (TAGBASE_TBOECKEL | 0x11e0)
#define MUIM_OOPWin_Print                           (TAGBASE_TBOECKEL | 0x11e1)
#define MUIM_OOPWin_Remove                          (TAGBASE_TBOECKEL | 0x11e2)
#define MUIM_OOPWin_More                            (TAGBASE_TBOECKEL | 0x11e3)
#define MUIM_OOPWin_ListChange                      (TAGBASE_TBOECKEL | 0x11e4)

#define MUIM_MainWin_About                          (TAGBASE_TBOECKEL | 0x1200)
#define MUIM_MainWin_ShowAllocations                (TAGBASE_TBOECKEL | 0x1201)
#define MUIM_MainWin_ShowAssigns                    (TAGBASE_TBOECKEL | 0x1202)
#define MUIM_MainWin_ShowClasses                    (TAGBASE_TBOECKEL | 0x1203)
#define MUIM_MainWin_ShowCommodities                (TAGBASE_TBOECKEL | 0x1204)
#define MUIM_MainWin_ShowDevices                    (TAGBASE_TBOECKEL | 0x1205)
#define MUIM_MainWin_ShowExpansions                 (TAGBASE_TBOECKEL | 0x1206)
#define MUIM_MainWin_ShowFonts                      (TAGBASE_TBOECKEL | 0x1207)
#define MUIM_MainWin_ShowInputHandlers              (TAGBASE_TBOECKEL | 0x1208)
#define MUIM_MainWin_ShowInterrupts                 (TAGBASE_TBOECKEL | 0x1209)
#define MUIM_MainWin_ShowLibraries                  (TAGBASE_TBOECKEL | 0x120a)
#define MUIM_MainWin_ShowLocks                      (TAGBASE_TBOECKEL | 0x120b)
#define MUIM_MainWin_ShowLowMemory                  (TAGBASE_TBOECKEL | 0x120c)
#define MUIM_MainWin_ShowMemory                     (TAGBASE_TBOECKEL | 0x120d)
#define MUIM_MainWin_ShowMounts                     (TAGBASE_TBOECKEL | 0x120e)
#define MUIM_MainWin_ShowPorts                      (TAGBASE_TBOECKEL | 0x120f)
#define MUIM_MainWin_ShowResidents                  (TAGBASE_TBOECKEL | 0x1210)
#define MUIM_MainWin_ShowCommands                   (TAGBASE_TBOECKEL | 0x1211)
#define MUIM_MainWin_ShowResources                  (TAGBASE_TBOECKEL | 0x1212)
#define MUIM_MainWin_ShowScreenModes                (TAGBASE_TBOECKEL | 0x1213)
#define MUIM_MainWin_ShowSemaphores                 (TAGBASE_TBOECKEL | 0x1214)
#define MUIM_MainWin_ShowSystem                     (TAGBASE_TBOECKEL | 0x1215)
#define MUIM_MainWin_ShowTasks                      (TAGBASE_TBOECKEL | 0x1216)
#define MUIM_MainWin_ShowTimers                     (TAGBASE_TBOECKEL | 0x1217)
#define MUIM_MainWin_ShowVectors                    (TAGBASE_TBOECKEL | 0x1218)
#define MUIM_MainWin_ShowWindows                    (TAGBASE_TBOECKEL | 0x1219)
#define MUIM_MainWin_ShowPatches                    (TAGBASE_TBOECKEL | 0x121a)
#define MUIM_MainWin_ShowCatalogs                   (TAGBASE_TBOECKEL | 0x121b)
#define MUIM_MainWin_ShowAudioModes                 (TAGBASE_TBOECKEL | 0x121c)
#define MUIM_MainWin_ShowResetHandlers              (TAGBASE_TBOECKEL | 0x121d)
#define MUIM_MainWin_ShowMonitors	            (TAGBASE_TBOECKEL | 0x121e)
#define MUIM_MainWin_ShowOOPClasses		    (TAGBASE_TBOECKEL | 0x121f)
#define MUIM_MainWin_FlushDevices                   (TAGBASE_TBOECKEL | 0x1250)
#define MUIM_MainWin_FlushFonts                     (TAGBASE_TBOECKEL | 0x1251)
#define MUIM_MainWin_FlushLibraries                 (TAGBASE_TBOECKEL | 0x1252)
#define MUIM_MainWin_FlushAll                       (TAGBASE_TBOECKEL | 0x1253)
#define MUIM_MainWin_UpdateAll                      (TAGBASE_TBOECKEL | 0x1260)






#define MUIA_ScreenModesDetailWin_ScreenMode        (TAGBASE_TBOECKEL | 0x2000)

#define MUIA_ClassesDetailWin_Class                 (TAGBASE_TBOECKEL | 0x2010)

#define MUIA_CommoditiesDetailWin_Commodity         (TAGBASE_TBOECKEL | 0x2020)

#define MUIA_ExpansionsDetailWin_Expansion          (TAGBASE_TBOECKEL | 0x2030)

#define MUIA_InterruptsDetailWin_Interrupt          (TAGBASE_TBOECKEL | 0x2040)

#define MUIA_MemoryDetailWin_Memory                 (TAGBASE_TBOECKEL | 0x2050)

#define MUIA_MountsDetailWin_Mount                  (TAGBASE_TBOECKEL | 0x2060)

#define MUIA_PortsDetailWin_Port                    (TAGBASE_TBOECKEL | 0x2070)

#define MUIA_ResidentsDetailWin_Resident            (TAGBASE_TBOECKEL | 0x2080)

#define MUIA_FontsDetailWin_Font                    (TAGBASE_TBOECKEL | 0x2090)

#define MUIA_WindowsDetailWin_Object                (TAGBASE_TBOECKEL | 0x20a0)

#define MUIA_TasksDetailWin_Task                    (TAGBASE_TBOECKEL | 0x20b0)

#define MUIA_DevicesDetailWin_Device                (TAGBASE_TBOECKEL | 0x20c0)

#define MUIA_ResourcesDetailWin_Resource            (TAGBASE_TBOECKEL | 0x20d0)

#define MUIA_LibrariesDetailWin_Library             (TAGBASE_TBOECKEL | 0x20e0)

#define MUIA_AudioModesDetailWin_AudioMode          (TAGBASE_TBOECKEL | 0x20f0)

#define MUIA_InputHandlersDetailWin_InputHandler    (TAGBASE_TBOECKEL | 0x2100)

#define MUIA_ResetHandlersDetailWin_ResetHandler    (TAGBASE_TBOECKEL | 0x2110)

#define MUIA_LowMemoryDetailWin_LowMemoryHandler    (TAGBASE_TBOECKEL | 0x2120)

#define MUIA_MonitorsDetailWin_MonitorSpec	    (TAGBASE_TBOECKEL | 0x2130)





#define MUIM_PriorityWin_GetPriority                (TAGBASE_TBOECKEL | 0x3000)
#define MUIA_PriorityWin_ObjectName                 (TAGBASE_TBOECKEL | 0x3001)
#define MUIA_PriorityWin_Priority                   (TAGBASE_TBOECKEL | 0x3002)

#define MUIM_SignalWin_GetSignals                   (TAGBASE_TBOECKEL | 0x3010)

#define MUIM_FlagsWin_ShowFlags                     (TAGBASE_TBOECKEL | 0x3020)
#define MUIA_FlagsWin_Flags                         (TAGBASE_TBOECKEL | 0x3021)
#define MUIV_FlagsWin_Type_Byte                     MUIV_FlagsButton_Type_Byte
#define MUIV_FlagsWin_Type_Word                     MUIV_FlagsButton_Type_Word
#define MUIV_FlagsWin_Type_Long                     MUIV_FlagsButton_Type_Long

#define MUIM_FunctionsWin_ShowFunctions             (TAGBASE_TBOECKEL | 0x3040)
#define MUIM_FunctionsWin_ListChange                (TAGBASE_TBOECKEL | 0x3041)
#define MUIM_FunctionsWin_Disassemble               (TAGBASE_TBOECKEL | 0x3042)
#define MUIV_FunctionsWin_NodeType_Library          (TAGBASE_TBOECKEL | 0x3050)
#define MUIV_FunctionsWin_NodeType_Device           (TAGBASE_TBOECKEL | 0x3051)
#define MUIV_FunctionsWin_NodeType_Resource         (TAGBASE_TBOECKEL | 0x3052)

#define MUIM_AboutWin_About                         (TAGBASE_TBOECKEL | 0x3060)

#define MUIM_DisassemblerWin_Disassemble            (TAGBASE_TBOECKEL | 0x3070)
#define MUIA_DisassemblerWin_Address                (TAGBASE_TBOECKEL | 0x3071)
#define MUIA_DisassemblerWin_Range                  (TAGBASE_TBOECKEL | 0x3072)
#define MUIA_DisassemblerWin_ForceHexDump           (TAGBASE_TBOECKEL | 0x3073)

#define MUIA_FontDisplay_Font                       (TAGBASE_TBOECKEL | 0x8000)
#define MUIA_FontDisplay_Text                       (TAGBASE_TBOECKEL | 0x8001)

#define MUIM_DisassemblerButton_Disassemble         (TAGBASE_TBOECKEL | 0x8100)
#define MUIA_DisassemblerButton_Address             (TAGBASE_TBOECKEL | 0x8101)
#define MUIA_DisassemblerButton_ForceHexDump        (TAGBASE_TBOECKEL | 0x8102)

#define MUIM_FlagsButton_ShowFlags                  (TAGBASE_TBOECKEL | 0x8200)
#define MUIA_FlagsButton_Title                      (TAGBASE_TBOECKEL | 0x8201)
#define MUIA_FlagsButton_Flags                      (TAGBASE_TBOECKEL | 0x8202)
#define MUIA_FlagsButton_BitArray                   (TAGBASE_TBOECKEL | 0x8203)
#define MUIA_FlagsButton_MaskArray                  (TAGBASE_TBOECKEL | 0x8204)
#define MUIA_FlagsButton_Type                       (TAGBASE_TBOECKEL | 0x8205)
#define MUIV_FlagsButton_Type_Byte                  1
#define MUIV_FlagsButton_Type_Word                  2
#define MUIV_FlagsButton_Type_Long                  3
#define MUIA_FlagsButton_WindowTitle                (TAGBASE_TBOECKEL | 0x8206)

#define MUIM_TaskButton_ShowTask                    (TAGBASE_TBOECKEL | 0x8300)
#define MUIA_TaskButton_Task                        (TAGBASE_TBOECKEL | 0x8301)
#define MUIA_TaskButton_IsProcessPointer            (TAGBASE_TBOECKEL | 0x8302)

#define MUIM_PortButton_ShowPort                    (TAGBASE_TBOECKEL | 0x8400)
#define MUIA_PortButton_Port                        (TAGBASE_TBOECKEL | 0x8401)

#define MUIM_MonitorButton_ShowMonitorSpec	    (TAGBASE_TBOECKEL | 0x8500)
#define MUIA_MonitorButton_MonitorSpec	    	    (TAGBASE_TBOECKEL | 0x8501)

#define MUIA_Window_ParentWindow                    (TAGBASE_TBOECKEL | 0xe000)
#define MUIA_Window_MaxChildWindowCount             (TAGBASE_TBOECKEL | 0xe001)
#define MUIA_Window_LayoutString                    (TAGBASE_TBOECKEL | 0xe002)
#define MUIM_Window_AddChildWindow                  (TAGBASE_TBOECKEL | 0xe010)
#define MUIM_Window_RemChildWindow                  (TAGBASE_TBOECKEL | 0xe011)
#define MUIM_Window_CloseChildWindows               (TAGBASE_TBOECKEL | 0xe012)

#endif /* SCOUT_TAGS_H */

