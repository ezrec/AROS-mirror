/*
 *              HOTKEY.C                                                                                                vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *      Updated by Thomas Richter, THOR, 5.3.2000
 *
 *              Controls the management of the commodities hotkey for SnoopDos,
 *              along with other related stuff (Workbench AppIcon/Tools Menu support).
 */

#include "system.h"
#include "snoopdos.h"
#include "snooptext.h"

#include "icon.h"

extern char CommodityTitle[];   /* From SNOOPDOS.C */

struct NewBroker MyBroker = {
        NB_VERSION,
        "SnoopDos",
        CommodityTitle,
        NULL,                                           /* Description, filled in at run time */
        NBU_UNIQUE | NBU_NOTIFY,
        COF_SHOW_HIDE,
        0, 0, 0
};

struct MsgPort          *BrokerMP;              /* Message port used for msgs from CX   */
CxObj                           *Broker;                /* CX handle for the broker we created  */
CxObj                           *Filter;                /* Filter to filter CX messages                 */
struct MsgPort          *WorkbenchPort; /* Port for AppIcon/AppMenu messages    */
struct DiskObject       *ProgramIcon;   /* Handle on our program's icon data    */
struct AppMenuItem      *MyAppMenu;             /* Current AppMenuItem, if any                  */
struct AppIcon          *MyAppIcon;             /* Current AppMenuItem, if any                  */
struct DiskObject       *AppIconObj;    /* Current object for AppMenuItem               */      
struct DiskObject       *DefIconObj;    /* A temporary default icon we use              */      

/*
 *              InstallHotKey(char *hotkey)
 *
 *              Initialises commodity support and sets things up so that the
 *              key sequence specified by "hotkey" will send a CTRL-F activation
 *              signal to SnoopDos. We can also get various messages from the
 *              commodities exchange to show/hide the interface etc -- these
 *              will be indicated using CommodityMask.
 *
 *              To see whether a hotkey is currently available or not, simply
 *              check the HotKeyActive flag. (This is used for disabling window
 *              gadgets etc.)
 *
 *              Returns true for success, false for failure.
 */
int InstallHotKey(char *hotkey)
{
        CxObj *newobj;

        HotKeyActive = 0;               /* Assume commodity is not currently active */

        if (CurSettings.Setup.HideMethod == HIDE_NONE || !CxBase) {
                /*
                 *              Don't install a commodity at all if we have no hide
                 *              method.
                 */
                return (0);
        }
        if (!BrokerMP) {
                BrokerMP = CreateMsgPort();
                if (!BrokerMP)
                        return (0);
                CommodityMask     = (1L << BrokerMP->mp_SigBit);
        }

        if (Broker && MyBroker.nb_Pri != CommodityPriority) {
                /*
                 *              The commodity priority has changed, so 
                 *              remove the current broker to force a new
                 *              one to be created at the correct priority.
                 */
                DeleteCxObjAll(Broker);
                Broker = NULL;
                Filter = NULL;
        }
        if (!Broker) {
                /*
                 *              Attempt to create a new broker
                 */
                MyBroker.nb_Descr = MSG(MSG_COMMODITY_DESC);
                MyBroker.nb_Port  = BrokerMP;
                MyBroker.nb_Pri   = CommodityPriority;
                Broker = CxBroker(&MyBroker, NULL);
                if (!Broker)
                        return (0);
                ActivateCxObj(Broker, 1);
        }
        /*
         *              Okay, now we create a new filter object to handle the
         *              commodity string we were passed
         */
        if (Filter)
                DeleteCxObjAll(Filter);
        
        Filter = CxFilter(hotkey);
        if (!Filter) {
                /*
                 *              Couldn't create the filter (probably out of memory --
                 *              specifying an illegal hotkey doesn't cause this to
                 *              fail, but instead produces an accumulated error later).
                 *
                 *              We still leave the broker itself installed, since it can
                 *              be used to allow the CX to send us Enable/Disable/Show/
                 *              Hide/Quit messages.
                 */
                return (0);
        }

        /*
         *              Now add our various translation bits and pieces to the filter
         */
        newobj = CxSignal(FindTask(NULL), SIGBREAKB_CTRL_F);
        if (!newobj)
                goto filter_error;
        AttachCxObj(Filter, newobj);

        newobj = CxTranslate(NULL);
        if (!newobj)
                goto filter_error;
        AttachCxObj(Filter, newobj);

        if (CxObjError(Filter))
                goto filter_error;

        AttachCxObj(Broker, Filter);
        HotKeyActive = 1;       /* Everything went okay so show we're active */
        return (1);

filter_error:
        DeleteCxObjAll(Filter);
        Filter = NULL;
        return 0;
}

/*
 *              HandleHotKey()
 *
 *              Handles any incoming messages on the commodities exchange port.
 *              Should be called whenever a CommodityMask signal is received.
 */
void HandleHotKeyMsgs(void)
{
        CxMsg *msg;

        if (!BrokerMP)
                return;

        while ((msg = (CxMsg *)GetMsg(BrokerMP)) != NULL) {
                ULONG msgid             = CxMsgID(msg);
                ULONG msgtype   = CxMsgType(msg);

                ReplyMsg((void *)msg);
                if (msgtype == CXM_COMMAND) {
                        switch (msgid) {
                                
                                case CXCMD_DISABLE:
                                        SetMonitorMode(MONITOR_DISABLED);
                                        break;

                                case CXCMD_ENABLE:
                                        if (Disabled)   /* Don't want to enable if Paused */
                                                SetMonitorMode(MONITOR_NORMAL);
                                        break;

                                case CXCMD_KILL:
                                        QuitFlag = 1;
                                        break;

                                case CXCMD_APPEAR:
                                case CXCMD_UNIQUE:
                                        ShowSnoopDos();
                                        break;

                                case CXCMD_DISAPPEAR:
                                        HideSnoopDos();
                                        break;
                        }
                }
        }
}

/*
 *              CleanupHotKey()
 *
 *              Frees any resources used by the commodities module
 */
void CleanupHotKey(void)
{
        if (Broker) {
                DeleteCxObjAll(Broker);
                Broker = NULL;
                Filter = NULL;
        }
        if (BrokerMP) {
                struct Message *msg;
                while ((msg = GetMsg(BrokerMP)) != NULL)
                        ReplyMsg(msg);
                DeleteMsgPort(BrokerMP);
                BrokerMP          = NULL;
                CommodityMask = 0;
        }
        HotKeyActive = 0;
}

/*
 *              GetCommandName()
 *
 *              Returns a pointer to a static string containing the name of
 *              this program. The pointer remains valid until the next call
 *              to this function.
 */
char *GetCommandName(void)
{
        static char name[200];
        struct Process *pr = (struct Process *)FindTask(NULL);
        struct CommandLineInterface *cli = BTOC(pr->pr_CLI);
        char *cmdname = (char *)BTOC(cli->cli_CommandName);

        if (WBenchMsg)
                return (WBenchMsg->sm_ArgList->wa_Name);
        
        memcpy(name, cmdname+1, *cmdname);
        name[*cmdname] = '\0';
        return (name);
}
 
/*
 *              GetProgramIcon()
 *
 *              Returns a pointer to an icon structure containing a program
 *              icon for SnoopDos, as follows:
 *
 *                      If SnoopDos was run from Workbench, we have a lock on it
 *                      If SnoopDos was run from CLI, we look in the directory
 *                      the program was run from. If we can't find it there, then
 *                      we give up and use the default SnoopDos icon image.
 *
 *              Regardless, you should call CleanupIcons() before exiting,
 *              to free up any loose icon images that may have been allocated.
 *
 *              Warning: icon.library must be open when you call this function.
 */
struct DiskObject *GetProgramIcon(void)
{
        if (ProgramIcon)
                return (ProgramIcon);
        
        if (WBenchMsg) {
                /*
                 *              Running from Workbench, so try and get the program icon
                 *              indicated in the lock
                 */
                struct WBArg *wbarg = WBenchMsg->sm_ArgList;
                BPTR lk;

                lk          = CurrentDir(wbarg->wa_Lock);
                ProgramIcon = GetDiskObject(wbarg->wa_Name);
                CurrentDir(lk);
        } else {
                /*
                 *              Running from CLI so try and get icon associated with executable
                 */
                struct Process *pr = (struct Process *)FindTask(NULL);
                BPTR lk;

                if (pr->pr_HomeDir)     
                        lk = CurrentDir(pr->pr_HomeDir);
                ProgramIcon = GetDiskObject(GetCommandName());
                if (pr->pr_HomeDir)
                        CurrentDir(lk);
        }
        if (ProgramIcon)
                return (ProgramIcon);
        
        /*
         *              Okay, unfortunately we couldn't locate the program icon, so
         *              instead, let's just fall back to the default icon.
         */
        DefSnoopDosIcon.do_StackSize = MINSTACKSIZE;    /* Fix this up here */
        return (&DefSnoopDosIcon);
}

/*
 *              CleanupIcons()
 *
 *              Frees any icon-related resources that were allocated
 */
void CleanupIcons(void)
{
        RemoveProgramFromWorkbench();
        if (ProgramIcon) {
                FreeDiskObject(ProgramIcon);
                ProgramIcon = NULL;
        }
        if (DefIconObj) {
                FreeDiskObject(DefIconObj);
                DefIconObj = NULL;
                AppIconObj = NULL;
        }
        if (WorkbenchPort) {
                struct Message *msg;

                while ((msg = GetMsg(WorkbenchPort)) != NULL)
                        ReplyMsg(msg);

                DeleteMsgPort(WorkbenchPort),
                WorkbenchPort = NULL;
                WorkbenchMask = 0;
        }
        if (WorkbenchBase) {
        #ifdef __amigaos4__
	        	if(IWorkbench) {
    	    		DropInterface((struct Interface*)IWorkbench);
        			IWorkbench = NULL;
        		}
        #endif
                CloseLibrary(WorkbenchBase);
                WorkbenchBase = NULL;
        }
}

/*
 *              WriteIcon(filename)
 *
 *              Writes a SnoopDos project icon to the associated filename. If an
 *              icon already exists for the filename, then that icon is left alone.
 *
 *              If no icon exists, then we create one using a copy of what we think
 *              is the icon SnoopDos was started from (or the default).
 */
void WriteIcon(char *filename)
{
        struct DiskObject newobj;
        struct DiskObject *dobj;

        if (!IconBase)
                return;
        
        dobj = GetDiskObject(filename);
        if (dobj) {
                FreeDiskObject(dobj);
                return;
        }
        dobj = GetProgramIcon();

        /*
         *              We can create a copy of our tool icon and change
         *              it to a project icon.
         */
        newobj = *dobj;
        newobj.do_CurrentX      = NO_ICON_POSITION;
        newobj.do_CurrentY      = NO_ICON_POSITION;
        newobj.do_ToolTypes     = DefToolTypes;
        newobj.do_Type                  = WBPROJECT;
        if (WBenchMsg)
                newobj.do_DefaultTool = WBenchMsg->sm_ArgList->wa_Name;
        else
                newobj.do_DefaultTool = GetCommandName();
        PutDiskObject(filename, &newobj);
}

/*
 *              AddProgramToWorkbench(hidetype)
 *
 *              Creates an AppIcon or AppMenuItem which can be used to re-active
 *              SnoopDos, and attaches it to Workbench. Normally used when
 *              SnoopDos goes into a HIDE state.
 *
 *              Hidetype is HIDE_ICON or HIDE_TOOLS, depending on which type of
 *              hide method is required.
 *
 *              Initialises WorkbenchMask accordingly.
 *
 *              Call RemoveProgramFromWorkbench() to remove the item later on.
 *
 *              Returns TRUE for success, FALSE for failure.
 */
int AddProgramToWorkbench(int hidetype)
{
        if (!IconBase)
                return (FALSE);

        if (!WorkbenchBase) {
#ifdef __amigaos4__
                WorkbenchBase = OpenLib("workbench.library", 37,(struct Interface **)&IWorkbench);
#else
                WorkbenchBase = OpenLibrary("workbench.library", 37);
#endif
                if (!WorkbenchBase)
                        return (FALSE);
        }
        if (!WorkbenchPort) {
                WorkbenchPort = CreateMsgPort();
                if (!WorkbenchPort)
                        return (FALSE);
                WorkbenchMask = (1 << WorkbenchPort->mp_SigBit);
        }
        if (hidetype == HIDE_TOOLS && !MyAppMenu) {
                /*
                 *              Adding an item to the Workbench tools menu
                 */
                MyAppMenu = AddAppMenuItem(0, 0, MSG(MSG_APPMENU_NAME),
                                                                   WorkbenchPort, NULL);
                if (!MyAppMenu)
                        return (FALSE);
        }
        if (hidetype == HIDE_ICON && !MyAppIcon) {
                /*
                 *              Adding an AppIcon to the Workbench. We make a copy
                 *              of the current program icon to use as our appicon.
                 */
                struct DiskObject *dobj;

                if (!AppIconObj) {
                        /*
                         *              We need a temporary icon to play around with.
                         *              We allocate a new default disk object, copy
                         *              it, and modify our copy to give us an icon
                         *              we can use as an AppIcon. We need to remember
                         *              to free the original icon before we exit.
                         */

                         /*             The following is not really required. We operate
                          *             directly on the program icon instead, and release
                          *             this icon.
                          *             THOR, 5.4.2000
                        static struct DiskObject defobj;
                                
                        DefIconObj = GetDefDiskObject(WBTOOL);
                        if (!DefIconObj)
                                return (FALSE);

                        defobj     = *DefIconObj;
                        AppIconObj = &defobj;
                          *
                          */

                        dobj       = GetProgramIcon();
                          /* Note that this call can't fail. In worst case, we
                           * get the default icon.
                           *
                           */

                          /*
                           * Removed again. THOR, 5.3.2000
                           *
                        AppIconObj->do_Gadget.Width                     = dobj->do_Gadget.Width;
                        AppIconObj->do_Gadget.Height            = dobj->do_Gadget.Height;
                        AppIconObj->do_Gadget.Flags                     = dobj->do_Gadget.Flags;
                        AppIconObj->do_Gadget.GadgetRender      = dobj->do_Gadget.GadgetRender;
                        AppIconObj->do_Gadget.SelectRender      = dobj->do_Gadget.SelectRender;
                        AppIconObj->do_Type                             = 0;
                           *
                           */

                        AppIconObj = dobj;
                }
                AppIconObj->do_CurrentX = CurSettings.IconPosLeft != -1 ?
                                                                  CurSettings.IconPosLeft :
                                                                  NO_ICON_POSITION;
                AppIconObj->do_CurrentY = CurSettings.IconPosTop != -1 ?
                                                                  CurSettings.IconPosTop :
                                                                  NO_ICON_POSITION;
                MyAppIcon = AddAppIcon(0, 0, APPICON_NAME, WorkbenchPort,
                                                           0L, AppIconObj, TAG_DONE);
                if (!MyAppIcon)
                        return (FALSE);
        }
        return (TRUE);
}

/*
 *              RemoveProgramFromWorkbench()
 *
 *              Removes any AppIcon or AppMenuItem item that is currently installed
 *              on Workbench. Usually called when the SnoopDos window is about
 *              about to reappear.
 */
void RemoveProgramFromWorkbench(void)
{
        if (MyAppMenu)          RemoveAppMenuItem(MyAppMenu),   MyAppMenu = NULL;
        if (MyAppIcon)          RemoveAppIcon(MyAppIcon),       MyAppIcon = NULL;
}

/*
 *              HandleWorkbenchMsgs()
 *
 *              Handles any outstanding messages at the Workbench message port
 */
void HandleWorkbenchMsgs(void)
{
        struct AppMessage *msg;
        int show = 0;

        if (!WorkbenchPort)
                return;
        
        while ((msg = (struct AppMessage *)GetMsg(WorkbenchPort)) != NULL) {
                show = 1;
                ReplyMsg(&msg->am_Message);
        }
        if (show)
                ShowSnoopDos();
}

