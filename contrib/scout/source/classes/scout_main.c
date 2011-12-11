/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#include "system_headers.h"

struct MainWinData {
    TEXT mwd_Title[WINDOW_TITLE_LENGTH];
    APTR mwd_Windows[30];
    ULONG mwd_UpdateCommands[30];
};

#if !defined(MUIA_Dtpic_Name)
    #define MUIA_Dtpic_Name 0x80423d72
#endif

HOOKPROTONH(close_callfunc, void, Object *obj, ULONG *index)
{
    struct MainWinData *mwd = INST_DATA(OCLASS(obj), obj);

    mwd->mwd_Windows[*index] = NULL;
}
MakeStaticHook(close_callhook, close_callfunc);

STATIC APTR LogoObject( void )
{
    APTR logo = NULL;
    BPTR lock;

    if ((lock = Lock("PROGDIR:scout.logo", SHARED_LOCK)) != ZERO) {
        UnLock(lock);

        if (CheckMCCVersion(MUIC_Dtpic, 19, 0, FALSE)) {
            logo = MUI_NewObject(MUIC_Dtpic, MUIA_Dtpic_Name, "PROGDIR:scout.logo", TAG_DONE);
        }
    }

    if (logo == NULL) {
        logo = (Object *)BodychunkObject,
                    MUIA_FixWidth, SCOUT_LOGO_WIDTH,
                    MUIA_FixHeight, SCOUT_LOGO_HEIGHT,
                    MUIA_Bitmap_Width, SCOUT_LOGO_WIDTH,
                    MUIA_Bitmap_Height, SCOUT_LOGO_HEIGHT,
                    MUIA_Bodychunk_Depth, SCOUT_LOGO_DEPTH ,
                    MUIA_Bodychunk_Body, (IPTR)scout_logo_body,
                    MUIA_Bodychunk_Compression, SCOUT_LOGO_COMPRESSION,
                    MUIA_Bodychunk_Masking, SCOUT_LOGO_MASKING,
                    MUIA_Bitmap_SourceColors, (IPTR)scout_logo_colors,
                    MUIA_Bitmap_Transparent, FALSE,
               End;
    }

    return logo;
}

STATIC void CreateSubWindow( struct IClass *cl,
                             Object *obj,
                             ULONG idx,
                             struct MUI_CustomClass *mcc,
                             ULONG cmd )
{
    struct MainWinData *mwd = INST_DATA(cl, obj);

    if (mwd->mwd_Windows[idx]) {
        DoMethod(mwd->mwd_Windows[idx], MUIM_Window_ToFront);
        DoMethod(mwd->mwd_Windows[idx], cmd);
        set(mwd->mwd_Windows[idx], MUIA_Window_Activate, TRUE);
    } else {
        APTR win;

        if ((win = NewObject(mcc->mcc_Class, NULL, MUIA_Window_ParentWindow, obj,
                                                   MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                                                   TAG_DONE)) != NULL) {
            COLLECT_RETURNIDS;
            mwd->mwd_Windows[idx] = win;
            mwd->mwd_UpdateCommands[idx] = cmd;
            DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_CallHook, &close_callhook, idx);
            set(win, MUIA_Window_Open, TRUE);
            DoMethod(win, cmd);
            REISSUE_RETURNIDS;
        }
    }
}

STATIC ULONG mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR button[32];

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Main",
        MUIA_Window_ID, MakeID('M','A','I','N'),
        WindowContents, HGroup,
            MUIA_Group_SameHeight, TRUE,
            Child, (IPTR)VGroup,                  /* This is the SCOUT logo */
                MUIA_Weight    , 0,
                MUIA_Frame     , MUIV_Frame_Text,
                MUIA_Background, MUII_SHADOW,
                Child, (IPTR)VSpace(0),
                Child, (IPTR)LogoObject(),
                Child, (IPTR)VSpace(0),
            End,

            Child, (IPTR)ColGroup(5), MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(button[ 0] = MakeButton(txtMainAllocations)),
                Child, (IPTR)(button[ 1] = MakeButton(txtMainAssigns)),
                Child, (IPTR)(button[ 2] = MakeButton(txtMainBoopsiClasses)),
                Child, (IPTR)(button[ 3] = MakeButton(txtMainCommodities)),
                Child, (IPTR)(button[ 4] = MakeButton(txtMainDevices)),

                Child, (IPTR)(button[ 5] = MakeButton(txtMainExpansions)),
                Child, (IPTR)(button[ 6] = MakeButton(txtMainFonts)),
                Child, (IPTR)(button[ 7] = MakeButton(txtMainInputHandlers)),
                Child, (IPTR)(button[ 8] = MakeButton(txtMainInterrupts)),
                Child, (IPTR)(button[ 9] = MakeButton(txtMainLibraries)),

                Child, (IPTR)(button[10] = MakeButton(txtMainLocks)),
                Child, (IPTR)(button[11] = MakeButton(txtMainLowMemory)),
                Child, (IPTR)(button[12] = MakeButton(txtMainMemory)),
                Child, (IPTR)(button[13] = MakeButton(txtMainMountedDevs)),
                Child, (IPTR)(button[14] = MakeButton(txtMainPorts)),

                Child, (IPTR)(button[15] = MakeButton(txtMainResidents)),
                Child, (IPTR)(button[16] = MakeButton(txtMainResCmds)),
                Child, (IPTR)(button[17] = MakeButton(txtMainResources)),
                Child, (IPTR)(button[18] = MakeButton(txtMainScreenModes)),
                Child, (IPTR)(button[19] = MakeButton(txtMainSemaphores)),

                Child, (IPTR)(button[20] = MakeButton(txtMainSystem)),
                Child, (IPTR)(button[21] = MakeButton(txtMainTasks)),
                Child, (IPTR)(button[22] = MakeButton(txtMainTimer)),
                Child, (IPTR)(button[23] = MakeButton(txtMainVectors)),
                Child, (IPTR)(button[24] = MakeButton(txtMainWindows)),

                Child, (IPTR)(button[25] = MakeButton(txtMainPatches)),
                Child, (IPTR)(button[26] = MakeButton(txtMainCatalogs)),
                Child, (IPTR)(button[27] = MakeButton(txtMainAudioModes)),
                Child, (IPTR)(button[28] = MakeButton(txtMainResetHandlers)),
                Child, (IPTR)(button[29] = MakeButton(txtMainMonitors)),
		
		Child, (IPTR)(button[30] = MakeButton(txtMainOOPClasses)),
		Child, (IPTR)(button[31] = MakeButton(txtMainMonitorClass)),
		Child, (IPTR)HSpace(0),
		Child, (IPTR)HSpace(0),
		Child, (IPTR)HSpace(0),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct MainWinData *mwd = INST_DATA(cl, obj);
    #if !defined(__amigaos4__)
        struct PatchPort *pp;
        struct SetManPort *sp;
        struct Library *pc = NULL;
    #endif

        set(obj, MUIA_Window_Title, MyGetWindowTitle("\0", mwd->mwd_Title, sizeof(mwd->mwd_Title)));

        DoMethod(obj,        MUIM_Notify,  MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(button[ 0], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowAllocations);
        DoMethod(button[ 1], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowAssigns);
        DoMethod(button[ 2], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowClasses);
        DoMethod(button[ 3], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowCommodities);
        DoMethod(button[ 4], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowDevices);
        DoMethod(button[ 5], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowExpansions);
        DoMethod(button[ 6], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowFonts);
        DoMethod(button[ 7], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowInputHandlers);
        DoMethod(button[ 8], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowInterrupts);
        DoMethod(button[ 9], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowLibraries);
        DoMethod(button[10], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowLocks);
        DoMethod(button[11], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowLowMemory);
        DoMethod(button[12], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowMemory);
        DoMethod(button[13], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowMounts);
        DoMethod(button[14], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowPorts);
        DoMethod(button[15], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowResidents);
        DoMethod(button[16], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowCommands);
        DoMethod(button[17], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowResources);
        DoMethod(button[18], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowScreenModes);
        DoMethod(button[19], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowSemaphores);
        DoMethod(button[20], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowSystem);
        DoMethod(button[21], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowTasks);
        DoMethod(button[22], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowTimers);
        DoMethod(button[23], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowVectors);
        DoMethod(button[24], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowWindows);
        DoMethod(button[25], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowPatches);
        DoMethod(button[26], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowCatalogs);
        DoMethod(button[27], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowAudioModes);
        DoMethod(button[28], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowResetHandlers);
        DoMethod(button[29], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowMonitors);
	DoMethod(button[30], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowOOPClasses);
	DoMethod(button[31], MUIM_Notify,  MUIA_Pressed, FALSE, obj, 1, MUIM_MainWin_ShowMonitorClass);

/* TODO: Scout is able to run on a network, and there can be a situation where server
         runs for example AROS and client runs AmigaOS. So we should be able to make
	 decisions about supported extensions by querying server's OS, not by #ifdef's here */
    #if defined(__amigaos4__)
        set(button[25], MUIA_Disabled, TRUE);
    #else
        Forbid();
        pp = (struct PatchPort *)FindPort(PATCHPORT_NAME);
        sp = (struct SetManPort *)FindPort(SETMANPORT_NAME);
        #if !defined(__AROS__)
        pc = (struct Library *)OpenResource(PATCHRES_NAME);
        #endif
        Permit();
        set(button[25], MUIA_Disabled, (pp == NULL && sp == NULL && pc == NULL));
    #endif

    set(button[30], MUIA_Disabled, !arOS);
    set(button[31], MUIA_Disabled, !(morphOS || arOS));
    }

    return (ULONG)obj;
}

STATIC ULONG mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC ULONG mAbout( UNUSED struct IClass *cl,
                     Object *obj,
                     UNUSED Msg msg )
{
    APTR aboutWin;

    if ((aboutWin = (Object *)AboutWindowObject,
            MUIA_Window_ParentWindow, (IPTR)obj,
            MUIA_Window_NoMenus, TRUE,
        End) != NULL) {
        APTR app;

        app = (APTR)xget(obj, MUIA_ApplicationObject);
        DoMethod(app, OM_ADDMEMBER, aboutWin);

        DoMethod(aboutWin, MUIM_AboutWin_About);

        DoMethod(app, OM_REMMEMBER, aboutWin);

        MUI_DisposeObject(aboutWin);
    }

    return 0;
}

STATIC ULONG mShowAllocations( struct IClass *cl,
                               Object *obj,
                               UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 0, AllocationsWinClass, MUIM_AllocationsWin_Update);

    return 0;
}

STATIC ULONG mShowAssigns( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 1, AssignsWinClass, MUIM_AssignsWin_Update);

    return 0;
}

STATIC ULONG mShowClasses( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 2, ClassesWinClass, MUIM_ClassesWin_Update);

    return 0;
}

STATIC ULONG mShowOOPClasses( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 2, OOPWinClass, MUIM_OOPWin_Update);

    return 0;
}

STATIC ULONG mShowCommodities( struct IClass *cl,
                               Object *obj,
                               UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 3, CommoditiesWinClass, MUIM_CommoditiesWin_Update);

    return 0;
}

STATIC ULONG mShowDevices( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 4, DevicesWinClass, MUIM_DevicesWin_Update);

    return 0;
}

STATIC ULONG mShowExpansions( struct IClass *cl,
                              Object *obj,
                              UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 5, ExpansionsWinClass, MUIM_ExpansionsWin_Update);

    return 0;
}

STATIC ULONG mShowFonts( struct IClass *cl,
                         Object *obj,
                         UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 6, FontsWinClass, MUIM_FontsWin_Update);

    return 0;
}

STATIC ULONG mShowInputHandlers( struct IClass *cl,
                                 Object *obj,
                                 UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 7, InputHandlersWinClass, MUIM_InputHandlersWin_Update);

    return 0;
}

STATIC ULONG mShowInterrupts( struct IClass *cl,
                              Object *obj,
                              UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 8, InterruptsWinClass, MUIM_InterruptsWin_Update);

    return 0;
}

STATIC ULONG mShowLibraries( struct IClass *cl,
                             Object *obj,
                             UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 9, LibrariesWinClass, MUIM_LibrariesWin_Update);

    return 0;
}

STATIC ULONG mShowLocks( struct IClass *cl,
                         Object *obj,
                         UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 10, LocksWinClass, MUIM_LocksWin_Update);

    return 0;
}

STATIC ULONG mShowLowMemory( struct IClass *cl,
                             Object *obj,
                             UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 11, LowMemoryWinClass, MUIM_LowMemoryWin_Update);

    return 0;
}

STATIC ULONG mShowMemory( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 12, MemoryWinClass, MUIM_MemoryWin_Update);

    return 0;
}

STATIC ULONG mShowMounts( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 13, MountsWinClass, MUIM_MountsWin_Update);

    return 0;
}

STATIC ULONG mShowPorts( struct IClass *cl,
                         Object *obj,
                         UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 14, PortsWinClass, MUIM_PortsWin_Update);

    return 0;
}

STATIC ULONG mShowResidents( struct IClass *cl,
                             Object *obj,
                             UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 15, ResidentsWinClass, MUIM_ResidentsWin_Update);

    return 0;
}

STATIC ULONG mShowCommands( struct IClass *cl,
                            Object *obj,
                            UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 16, CommandsWinClass, MUIM_CommandsWin_Update);

    return 0;
}

STATIC ULONG mShowResources( struct IClass *cl,
                             Object *obj,
                             UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 17, ResourcesWinClass, MUIM_ResourcesWin_Update);

    return 0;
}

STATIC ULONG mShowScreenModes( struct IClass *cl,
                               Object *obj,
                               UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 18, ScreenModesWinClass, MUIM_ScreenModesWin_Update);

    return 0;
}

STATIC ULONG mShowSemaphores( struct IClass *cl,
                              Object *obj,
                              UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 19, SemaphoresWinClass, MUIM_SemaphoresWin_Update);

    return 0;
}

STATIC ULONG mShowSystem( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 20, SystemWinClass, MUIM_SystemWin_Update);

    return 0;
}

STATIC ULONG mShowTasks( struct IClass *cl,
                         Object *obj,
                         UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 21, TasksWinClass, MUIM_TasksWin_Update);

    return 0;
}

STATIC ULONG mShowTimers( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 22, TimersWinClass, MUIM_TimersWin_Update);

    return 0;
}

STATIC ULONG mShowVectors( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 23, VectorsWinClass, MUIM_VectorsWin_Update);

    return 0;
}

STATIC ULONG mShowWindows( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 24, WindowsWinClass, MUIM_WindowsWin_Update);

    return 0;
}

STATIC ULONG mShowPatches( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 25, PatchesWinClass, MUIM_PatchesWin_Update);

    return 0;
}

STATIC ULONG mShowCatalogs( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 26, CatalogsWinClass, MUIM_CatalogsWin_Update);

    return 0;
}

STATIC ULONG mShowAudioModes( struct IClass *cl,
                              Object *obj,
                              UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 27, AudioModesWinClass, MUIM_AudioModesWin_Update);

    return 0;
}

STATIC ULONG mShowResetHandlers( struct IClass *cl,
                                 Object *obj,
                                 UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 28, ResetHandlersWinClass, MUIM_ResetHandlersWin_Update);

    return 0;
}

STATIC ULONG mShowMonitors( struct IClass *cl,
                                 Object *obj,
                                 UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 29, MonitorsWinClass, MUIM_MonitorsWin_Update);

    return 0;
}

STATIC ULONG mShowMonitorClass( struct IClass *cl,
                                 Object *obj,
                                 UNUSED Msg msg )
{
    CreateSubWindow(cl, obj, 29, MonitorClassWinClass, MUIM_MonitorClassWin_Update);

    return 0;
}

STATIC ULONG mFlushDevices( UNUSED struct IClass *cl,
                            UNUSED Object *obj,
                            UNUSED Msg msg )
{
    MyDoCommand("FLUSHDEVS");

    return 0;
}

STATIC ULONG mFlushFonts( UNUSED struct IClass *cl,
                          UNUSED Object *obj,
                          UNUSED Msg msg )
{
    MyDoCommand("FLUSHFONTS");

    return 0;
}

STATIC ULONG mFlushLibraries( UNUSED struct IClass *cl,
                              UNUSED Object *obj,
                              UNUSED Msg msg )
{
    MyDoCommand("FLUSHLIBS");

    return 0;
}

STATIC ULONG mFlushAll( UNUSED struct IClass *cl,
                        UNUSED Object *obj,
                        UNUSED Msg msg )
{
    MyDoCommand("FLUSHALL");

    return 0;
}

STATIC ULONG mUpdateAll( struct IClass *cl,
                         Object *obj,
                         UNUSED Msg msg )
{
    struct MainWinData *mwd = INST_DATA(cl, obj);
    ULONG i;

    for (i = 0; i < ARRAY_SIZE(mwd->mwd_Windows); i++) {
        if (mwd->mwd_Windows[i]) {
            DoMethod(mwd->mwd_Windows[i], mwd->mwd_UpdateCommands[i]);
        }
    }

    return 0;
}

DISPATCHER(MainWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                         return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                     return (mDispose(cl, obj, (APTR)msg));
        case MUIM_MainWin_About:             return (mAbout(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowAllocations:   return (mShowAllocations(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowAssigns:       return (mShowAssigns(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowClasses:       return (mShowClasses(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowCommodities:   return (mShowCommodities(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowDevices:       return (mShowDevices(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowExpansions:    return (mShowExpansions(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowFonts:         return (mShowFonts(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowInputHandlers: return (mShowInputHandlers(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowInterrupts:    return (mShowInterrupts(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowLibraries:     return (mShowLibraries(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowLocks:         return (mShowLocks(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowLowMemory:     return (mShowLowMemory(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowMemory:        return (mShowMemory(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowMounts:        return (mShowMounts(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowPorts:         return (mShowPorts(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowResidents:     return (mShowResidents(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowCommands:      return (mShowCommands(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowResources:     return (mShowResources(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowScreenModes:   return (mShowScreenModes(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowSemaphores:    return (mShowSemaphores(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowSystem:        return (mShowSystem(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowTasks:         return (mShowTasks(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowTimers:        return (mShowTimers(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowVectors:       return (mShowVectors(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowWindows:       return (mShowWindows(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowPatches:       return (mShowPatches(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowCatalogs:      return (mShowCatalogs(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowAudioModes:    return (mShowAudioModes(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowResetHandlers: return (mShowResetHandlers(cl, obj, (APTR)msg));
        case MUIM_MainWin_ShowMonitors:      return (mShowMonitors(cl, obj, (APTR)msg));
	case MUIM_MainWin_ShowOOPClasses:    return (mShowOOPClasses(cl, obj, (APTR)msg));
	case MUIM_MainWin_ShowMonitorClass:  return (mShowMonitorClass(cl, obj, (APTR)msg));
        case MUIM_MainWin_FlushDevices:      return (mFlushDevices(cl, obj, (APTR)msg));
        case MUIM_MainWin_FlushFonts:        return (mFlushFonts(cl, obj, (APTR)msg));
        case MUIM_MainWin_FlushLibraries:    return (mFlushLibraries(cl, obj, (APTR)msg));
        case MUIM_MainWin_FlushAll:          return (mFlushAll(cl, obj, (APTR)msg));
        case MUIM_MainWin_UpdateAll:         return (mUpdateAll(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeMainWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MainWinData), ENTRY(MainWinDispatcher));
}

