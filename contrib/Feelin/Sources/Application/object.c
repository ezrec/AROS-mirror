#include "Private.h"

/*** Private ***************************************************************/

///app_create_ports
STATIC int32 app_create_ports(struct LocalObjectData *LOD)
{
    LOD->AppPort = CreateMsgPort();

    if (LOD->AppPort == NULL)
    {
        F_Log(FV_LOG_USER,"Unable to create Application's message port");

        return FALSE;
    }

    LOD -> WindowPort = CreateMsgPort();

    if (LOD->WindowPort == NULL)
    {
        F_Log(FV_LOG_USER,"Unable to create Windows' message port");

        return FALSE;
    }

    LOD -> TimersPort = CreateMsgPort();

    if (LOD->TimersPort == NULL)
    {
        F_Log(FV_LOG_USER,"Unable to create Timers' message port");

        return FALSE;
    }

    LOD -> TimeRequest = CreateIORequest(LOD -> TimersPort,sizeof (struct timerequest));

    if (LOD->TimeRequest == NULL)
    {
        F_Log(FV_LOG_USER,"Unable to create IORequest for timer.device");

        return FALSE;
    }

    if (OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest *) LOD -> TimeRequest,0))
    {
        F_Log(FV_LOG_USER,"Unable to open timer.device");
    }

    if (LOD->Base)
    {
        LOD -> AppPort -> mp_Node.ln_Pri = 2;
        LOD -> AppPort -> mp_Node.ln_Name = LOD -> Base;

        AddPort(LOD -> AppPort);
    }

    return TRUE;
}
//+
///app_delete_ports
STATIC void app_delete_ports(struct LocalObjectData *LOD)
{
    Forbid();

    if (LOD->TimeRequest)
    {
        if (LOD->AppPort && LOD->Base)
        {
            if (FindPort(LOD -> Base))
            {
                RemPort(LOD -> AppPort);
            }
        }

        CloseDevice((struct IORequest *) LOD -> TimeRequest);

        DeleteIORequest(LOD -> TimeRequest);

        LOD -> TimeRequest = NULL;
    }

    if (LOD->TimersPort)
    {
        struct Message *msg;

        while ((msg = GetMsg(LOD -> TimersPort)) != NULL)
        {
            ReplyMsg(msg);
        }
        DeleteMsgPort(LOD -> TimersPort);

        LOD -> TimersPort = NULL;
    }


    if (LOD->WindowPort)
    {
        struct Message *msg;

        while ((msg = GetMsg(LOD -> WindowPort)) != NULL)
        {
           ReplyMsg(msg);
        }

        DeleteMsgPort(LOD -> WindowPort);

        LOD -> WindowPort = NULL;
    }


    if (LOD->AppPort)
    {
        struct Message *msg;

        while ((msg = GetMsg(LOD -> AppPort)) != NULL)
        {
            if (IsRexxMsg((struct RexxMsg *)(msg)))
            {
                ReplyMsg(msg);
            }
            else
            {
                F_Dispose(msg);
            }
        }

        DeleteMsgPort(LOD -> AppPort); LOD -> AppPort = NULL;
    }

    Permit();
}
//+
///app_create_broker
STATIC int32 app_create_broker(struct LocalObjectData *LOD,struct TagItem *Tags)
{
    struct MsgPort *xp;

    if (!LOD -> Title ||
        !LOD -> Version ||
        !LOD -> Description)
    {
        return FALSE;
    }

    xp = CreateMsgPort();

    if (xp)
    {
        struct NewBroker *nb = F_New(sizeof (struct NewBroker));

        if (nb)
        {
            APTR bk;
            int32 er;

            nb -> nb_Version = NB_VERSION;
            nb -> nb_Name    = LOD -> Title;
            nb -> nb_Title   = LOD -> Version + 6;
            nb -> nb_Descr   = LOD -> Description;
            nb -> nb_Unique  = GetTagData(FA_Application_Unique,FALSE,Tags);
            nb -> nb_Pri     = GetTagData(FA_Application_BrokerPri,0,Tags);
            nb -> nb_Port    = xp;
            nb -> nb_Flags   = COF_SHOW_HIDE;

            bk = CxBroker(nb,(LONG *)&er);

            if (bk)
            {
                LOD -> Broker = bk;
                LOD -> BrokerPort = xp;
                LOD -> BrokerInit = nb;

                ActivateCxObj(bk,TRUE);

                return er;
            }
            F_Dispose(nb);
        }
        DeleteMsgPort(xp);
    }

    F_Log(FV_LOG_USER,"Unable to Create Commodity");

    return 0;
}
//+
///app_delete_broker
STATIC void app_delete_broker(struct LocalObjectData *LOD)
{
    if (LOD -> BrokerInit)
    {
        if (LOD -> BrokerPort)
        {
            struct Message *msg;

            Forbid();
            DeleteCxObj(LOD -> Broker); LOD -> Broker = NULL;

            while ((msg = GetMsg(LOD -> BrokerPort)))
            {
               ReplyMsg(msg);
            }

            DeleteMsgPort(LOD -> BrokerPort); LOD -> BrokerPort = NULL;
            Permit();
        }

        F_Dispose(LOD -> BrokerInit); LOD -> BrokerInit = NULL;
    }
}
//+

/*** Methods ***************************************************************/

///App_New
F_METHOD(ULONG,App_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    /* default values */

    LOD -> SleepCount       = 1;
    LOD -> p_Scheme         = "$application-scheme";
    LOD -> p_DisabledScheme = "$application-scheme-disable";

    /* parse initial values */

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_ColorScheme:             LOD -> p_Scheme         = (STRPTR)(item.ti_Data); break;
        case FA_DisabledColorScheme:     LOD -> p_DisabledScheme = (STRPTR)(item.ti_Data); break;

        case FA_Application_Title:       LOD -> Title       = (STRPTR)(item.ti_Data); break;
        case FA_Application_Version:     LOD -> Version     = (STRPTR)(item.ti_Data); break;
        case FA_Application_Copyright:   LOD -> Copyright   = (STRPTR)(item.ti_Data); break;
        case FA_Application_Author:      LOD -> Author      = (STRPTR)(item.ti_Data); break;
        case FA_Application_Description: LOD -> Description = (STRPTR)(item.ti_Data); break;
        case FA_Application_Base:        LOD -> Base        = (STRPTR)(item.ti_Data); break;

        case FA_Application_ResolveMapping: LOD -> ResolveMap = (FAppResolveMap *)(item.ti_Data); break;
    }

    /* creation time ! First, let's create all necessary message ports */

    if (app_create_ports(LOD))
    {
        /* Try to create a broker. Currently, a failure is not fatal */

        switch (app_create_broker(LOD,(struct TagItem *)(Msg)))
        {
            case CBERR_SYSERR:   F_Log(FV_LOG_USER,"System unable to create Broker");
            case CBERR_DUP:      F_Log(FV_LOG_USER,"Application already exists");
            case CBERR_VERSION:  F_Log(FV_LOG_USER,"Unknown Broker version");
        }

        LOD -> AppServer = F_SharedOpen("AppServer");

        if (LOD -> AppServer)
        {
            /* The FC_Dataspace object is used by objects to store data */

            LOD -> Dataspace = DataspaceObject, End;

            if (LOD -> Dataspace)
            {
                /* A new FC_Preference object should only be created if the
                application  has the FA_Application_Base attribute defined.
                Only  applications  with  a  base  name  can   have   local
                preferences. If base name is not defined, the FC_Preference
                object should be inherited from the AppServer's one.

                AppServer's FC_Preference object is used  as  a  reference.
                Preference  items  not defined in the application personnal
                FC_Preference  object  are  obtained  from  the   reference
                object.  More over, changes applied to the reference object
                are mirrored to FC_Preference objects using the  reference.
                */

                if (LOD -> Base)
                {
                    LOD -> Preferences = PreferenceObject,

                        "FA_Preference_Name",         LOD -> Base,
                        "FA_Preference_Reference",    F_Get(LOD -> AppServer,(ULONG) "FA_AppServer_Preference"),

                    End;
                }

                /* If the FC_Preference object fails to create, or  if  the
                application  has  no  basename, the AppServer FC_Preference
                object must be used instead. */

                if (!LOD -> Preferences)
                {
                    LOD -> Preferences = (FObject) F_Get(LOD -> AppServer,(ULONG) "FA_AppServer_Preference");

                    LOD -> Flags |= FF_APPLICATION_INHERITED_PREFS;
                }

                if (LOD -> Preferences)
                {
                    /*  A  notification  is  set  upon   the   FA_Preference_Update
                    attribute of the FC_Preference object, allowing the application
                    to be aware of preferences modifications, and update itself  to
                    mach these modifications */

                    LOD -> PreferencesNH = (FNotifyHandler *) F_Do(LOD -> Preferences,FM_Notify,"FA_Preference_Update",TRUE,Obj,FM_Application_Update,0);

                    if (LOD -> PreferencesNH)
                    {
                        /* Now that everything is  ready,  the  application
                        can be added to the application server. Calling our
                        super class (FC_Family) add all children defined in
                        the TAGS */

                        if (F_Do(LOD -> AppServer, FM_AddMember, Obj, FV_AddMember_Tail))
                        {
                            return F_SUPERDO();
                        }
                    }
                }
            }
        }
    }

    /* Something went wrong.  Children  defined  in  the  taglist  must  be
    disposed. */

    Tags = Msg;

    {
        struct TagItem *rtag;

        while ((rtag = F_DynamicNTI(&Tags,&item,Class)) != NULL)
        {
            if (item.ti_Tag == FA_Child)
            {
                F_DisposeObj((FObject)(item.ti_Data));
                rtag -> ti_Tag = TAG_IGNORE;
                rtag -> ti_Data = 0;
            }
        }
    }

    return 0;
}
//+
///App_Dispose
F_METHOD(void,App_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(LOD -> AppServer,FM_RemMember,Obj);

    /* Puting the application  to  sleep  close  all  windows  and  dispose
    graphic resources */

    F_Do(Obj,FM_Application_Sleep);

    F_Do(Obj,FM_Application_Save,FV_Application_BOTH);

    F_DisposeObj(LOD -> Dataspace); LOD -> Dataspace = NULL;

    F_Do(LOD -> Preferences,FM_UnNotify,LOD -> PreferencesNH);

    if (!(LOD -> Flags & FF_APPLICATION_INHERITED_PREFS))
    {
        F_DisposeObj(LOD -> Preferences);
    }
    LOD -> Preferences = NULL;

    if (LOD -> AppServer)
    {
        F_SharedClose(LOD -> AppServer);

        LOD -> AppServer = NULL;
    }

    app_delete_broker(LOD);
    app_delete_ports(LOD);

//   while (LOD -> Timers.Head)
//   F_Do(Obj,FM_Application_RemSignalHandler,(struct FeelinTimeHeader *)(LOD -> Timers.Head) -> Handler);

    F_SUPERDO();
}
//+
///App_Set
F_METHOD(void,App_Set)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Application_UserSignals:
        {
            LOD -> UserSignals = item.ti_Data;

            app_collect_signals(Class,Obj);
        }
        break;

        case FA_Application_Sleep:
        {
            if (item.ti_Data)
            {
                LOD -> SleepCount++; // 1 & more the application is put to sleep
            }
            else
            {
                LOD -> SleepCount--; // 0 & less the application is awakened
            }

            if (LOD -> SleepCount == 1)
            {
                F_Do(Obj,FM_Application_PushMethod,Obj,FM_Application_Sleep,0);
            }
            else if (LOD -> SleepCount == 0)
            {
                F_Do(Obj,FM_Application_PushMethod,Obj,FM_Application_Awake,0);
            }
        }
        break;

        case FA_Application_Menu:
        {
            F_Do(LOD->menu, FM_Disconnect);

            LOD->menu = (FObject) item.ti_Data;

            F_Do(LOD->menu, FM_Connect, LOD->menu);
        }
        break;
    }
    F_SUPERDO();
}
//+
///App_Get
F_METHOD(void,App_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Parent:                  F_STORE(LOD -> AppServer);       break;
        case FA_ColorScheme:             F_STORE(LOD -> Scheme);          break;
        case FA_DisabledColorScheme:     F_STORE(LOD -> DisabledScheme);  break;

        case FA_Application_Title:       F_STORE(LOD -> Title);        break;
        case FA_Application_Version:     F_STORE(LOD -> Version);      break;
        case FA_Application_Copyright:   F_STORE(LOD -> Copyright);    break;
        case FA_Application_Author:      F_STORE(LOD -> Author);       break;
        case FA_Application_Description: F_STORE(LOD -> Description);  break;
        case FA_Application_Base:        F_STORE(LOD -> Base);         break;

        case FA_Application_UserSignals: F_STORE(LOD -> UserSignals);  break;
        case FA_Application_WindowPort:  F_STORE(LOD -> WindowPort);   break;
        case FA_Application_BrokerPort:  F_STORE(LOD -> BrokerPort);   break;
        case FA_Application_Broker:      F_STORE(LOD -> Broker);       break;

        case FA_Application_Display:     F_STORE(LOD -> Display);      break;
        case FA_Application_Dataspace:   F_STORE(LOD -> Dataspace);    break;
        case FA_Application_Preference:  F_STORE(LOD -> Preferences);  break;

        case FA_Application_Sleep:       F_STORE(LOD -> SleepCount > 0); break;
        case FA_Application_Menu:        F_STORE(LOD->menu); break;
    }

    F_SUPERDO();
}
//+
///App_Connect
F_METHODM(uint32,App_Connect,FS_Connect)
{
    return TRUE;
}
//+
///App_Disconnect
F_METHOD(uint32,App_Disconnect)
{
    return TRUE;
}
//+
///App_AddMember
F_METHODM(uint32,App_AddMember,FS_AddMember)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        if (FF_Application_Setup & LOD -> Flags)
        {
            if (F_Get(Msg -> Object,FA_Window_Open))
            {
                F_Do(Msg -> Object,FM_Window_Open);
            }
        }
        return TRUE;
    }
    return FALSE;
}
//+
///App_RemMember
F_METHODM(void, App_RemMember, FS_RemMember)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg->Member)
    {
        if (Msg->Member == LOD->menu)
        {
            F_Do(Msg->Member, FM_Disconnect);

            LOD->menu = NULL;
        }
        else
        {
            if ((FF_Application_Setup & LOD -> Flags) && F_Get(Msg -> Member, FA_Window))
            {
                F_Do(Msg -> Member, FM_Window_Close);
            }

            F_SUPERDO();
        }
    }
}
//+

