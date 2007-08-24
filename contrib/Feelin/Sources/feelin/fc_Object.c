/*
**    FC_Object
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

$VER: 04.00 (2005/08/04)
 
    Every FC_Object can be turned on-the-fly as a semaphore. FC_Semaphore is
    now deprecated.
    
    The new FM_SetAs method can be used instead of FM_SetAsString (which  is
    now considered deprecated) to set a formated string. It can also be used
    to set a decimal number, an hexadecimal  number  or  a  constant  number
    converted from a given string.
    
    FM_CallHook and FM_CallHookEntry no longer call code with FeelinBase  in
    a6. Using hooks should be avoided as much as possible.

$VER: 03.12 (2005/04/07)
 
    FM_CallHook and FM_CallHookEntry call code with FeelinBase in a6
 
$VER: 03.10 (2004/12/14)

    Notifications are  no  longer  semaphore  locked,  this  was  completely
    useless...  and  sometime lead to inter-semaphore locking when disposing
    objects.

    The attribute supplied with the FM_Notify method is  now  resolved  when
    the  notification  structure is created. The attribute name can be short
    e.g. "First" or standard e.g. "FA_Prop_First".

*/

#include "Private.h"

#define F_POOL(sem_member)                         (((FPool *)(FeelinBase -> NotifyPool)) -> Semaphore.ss_ ## sem_member)

//#define CS_LOCK_NOTIFYPOOL
//#define CS_WARN_NOTIFICATION_LOOP
//#define DB_NOTIFY
//#define DB_HANDLENOTIFY

///Header
struct FeelinNotifyHandler
{
    struct FeelinNotifyHandler      *Next;
    uint32                            Flags;
    uint32                            Attribute;
    uint32                            Value;
    FObject                          Target;
    uint32                            Method;
    uint32                            Count;
// uint32 Msg[]
// uint32 MsgCopy[]
};

#define FF_NOTIFY_PERFORMING                    (1 << 0)

struct LocalObjectData
{
     uint32                          ID;
     APTR                            UserData;
     struct FeelinNotifyHandler     *Handlers;
     struct SignalSemaphore         *Atomic;
};
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Object_New
F_METHOD_NEW(Object_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*item;

    while  ((item = NextTagItem(&Tags)) != NULL)
    switch (item -> ti_Tag)
    {
        case FA_ID:
        {
            LOD -> ID = item -> ti_Data;
        }
        break;
    }

    F_DoA(Obj,FM_Set,Msg);

    return Obj;
}
//+
///Object_Dispose
F_METHOD_DISPOSE(Object_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FObject parent = (FObject) F_Get(Obj,FA_Parent);

    if (parent)
    {
        F_Do(parent,FM_RemMember,Obj);
    }
 
    if (LOD -> Handlers)
    {
        F_Do(Obj,FM_UnNotify,ALL);
    }
    
    F_Dispose(LOD -> Atomic); LOD -> Atomic = NULL;
}
//+
///Object_Set
F_METHOD_SET(Object_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem         *Tags = Msg,item;

    BOOL notify = TRUE;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_UserData: LOD -> UserData = (APTR)(item.ti_Data); break;
        case FA_NoNotify: notify = (item.ti_Data) ? FALSE : TRUE; break;
    }

    if (!notify || !LOD -> Handlers)
    {
        return;
    }

//      F_Log(0,"NOTIFY >> BEGIN");
    #ifdef CS_LOCK_NOTIFYPOOL
    F_SPool(FeelinBase -> NotifyPool);
    #endif

    Tags = Msg;

    while (F_DynamicNTI(&Tags,&item,Class))
    {
        struct FeelinNotifyHandler *nh;

        for (nh = LOD -> Handlers ; nh ; nh = nh -> Next)
        {
            if (FF_NOTIFY_PERFORMING & nh -> Flags)
            {
                #ifdef CS_WARN_NOTIFICATION_LOOP
                F_Log(FV_LOG_DEV,"!!NOTIFY LOOP!! Attribute (0x%08lx.%s) - Target %s{%08lx}",nh -> Attribute,f_find_attribute_name(nh -> Attribute,FeelinBase),_classname(nh -> Target),nh -> Target);
                #endif

                continue;
            }

            if ((nh -> Attribute == item.ti_Tag) && (nh -> Value == item.ti_Data || nh -> Value == FV_Notify_Always))
            {
                uint32 *cpy = NULL;
                FObject target = nh -> Target;

                nh -> Flags |= FF_NOTIFY_PERFORMING;

                switch ((uint32)(target))
                {
                    case FV_Notify_Self:          target = Obj; break;
                    case FV_Notify_Parent:        target = (FObject) F_Get(Obj,FA_Parent);         break;
                    case FV_Notify_Window:        target = (FObject) F_Get(Obj,FA_WindowObject);   break;
                    case FV_Notify_Application:   target = (FObject) F_Get(Obj,FA_Application);    break;
                }
            
//            F_Log(0,"ATTRIBUTE (0x%08lx)(%s) - TARGET %s{%08lx}",nh -> Attribute,f_find_attribute_name(nh -> Attribute,FeelinBase),_classname(target),target);

                if (nh -> Count)
                {
                    uint32 *msg = (APTR)((uint32)(nh) + sizeof (struct FeelinNotifyHandler));
                    cpy = (APTR)((uint32)(msg) + (nh -> Count + 1) * sizeof (uint32));

                    if (nh -> Value == FV_Notify_Always)
                    {
                        uint32 i;
/*
///DB_HANDLENOTIFY
#ifdef DB_HANDLENOTIFY
                        F_Log(0,"NOTIFY_ALWAYS");
#endif
//+
*/
                        for (i = 0 ; i < nh -> Count ; i++)
                        {
//                     F_Log(0,"   >> (%ld)(0x%08lx)",i,msg[i]);
 
                            switch (msg[i])
                            {
                                case FV_Notify_Value:   cpy[i] = item.ti_Data; break;
                                case FV_Notify_Toggle:  cpy[i] = (item.ti_Data) ? FALSE : TRUE; break;
                                default:                cpy[i] = msg[i]; break;
                            }
                        }
                    }
                    else
                    {
///DB_HANDLENOTIFY
#ifdef DB_HANDLENOTIFY
                        F_Log(0,"NOTIFY_0x%08lx",nh -> Value);
#endif
//+

                        CopyMem(msg,cpy,nh -> Count * sizeof (uint32));
                    }
                }

///DB_HANDLENOTIFY
#ifdef DB_HANDLENOTIFY
                F_Log(0,"Attribute 0x%08lx - Target %s{%08lx} - Method 0x%08lx - Msg 0x%08lx",nh -> Attribute,_classname(target),target,nh -> Method,cpy);

                if (nh -> Count)
                {
                    F_Log(0,"     >> Org 0x%08lx %08lx %08lx %08lx %08lx",(APTR)((uint32)(nh) + sizeof (struct FeelinNotifyHandler)));
                    F_Log(0,"     >> Msg 0x%08lx %08lx %08lx %08lx %08lx",cpy);
                }
#endif
//+

                if (nh -> Method == FM_Draw)
                {
                    F_Draw(target,*cpy);
                }
                else
                {
                    F_DoA(target,nh -> Method,cpy);
                }

                nh -> Flags &= ~FF_NOTIFY_PERFORMING;
            }
        }
    }

    #ifdef CS_LOCK_NOTIFYPOOL
    F_RPool(FeelinBase -> NotifyPool);
    #endif
//      F_Log(0,"NOTIFY >> END");
}
//+
///Object_Get
F_METHOD_GET(Object_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem         *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Class:    F_STORE(_class(Obj)); break;
        case FA_Version:  F_STORE((((struct in_FeelinClass *)(_class(Obj))) -> Module) ? ((struct in_FeelinClass *)(_class(Obj))) -> Module -> lib_Version : 0); break;
        case FA_Revision: F_STORE((((struct in_FeelinClass *)(_class(Obj))) -> Module) ? ((struct in_FeelinClass *)(_class(Obj))) -> Module -> lib_Revision : 0); break;
        case FA_ID:       F_STORE(LOD -> ID); break;
        case FA_UserData: F_STORE(LOD -> UserData); break;
    }
}
//+

///Object_Notify
F_METHODM(APTR,Object_Notify,FS_Notify)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinNotifyHandler *nh;

    if (Msg -> Method < 255 || Msg -> Count > 20)
    {
        F_Log(FV_LOG_DEV,"Suspicious Notify: Attribute 0x%08lx - Method 0x%08lx - Count 0x%08lx",_classname(Obj),Obj,Msg -> Attribute,Msg -> Method,Msg -> Count);
        return NULL;
    }

//   F_Log(0,"OPool(0x%08lx.%ld)",F_POOL(Owner),F_POOL(NestCount));
    #ifdef CS_LOCK_NOTIFYPOOL
    F_OPool(FeelinBase -> NotifyPool);
    #endif

    if ((nh = F_NewP(FeelinBase -> NotifyPool,(Msg -> Count) ? sizeof (struct FeelinNotifyHandler) + (Msg -> Count + 1) * sizeof (uint32) * 2 : sizeof (struct FeelinNotifyHandler))) != NULL)
    {
        nh -> Attribute = Msg -> Attribute;
        nh -> Value     = Msg -> Value;
        nh -> Target    = Msg -> Target;
        nh -> Method    = F_DynamicFindID((STRPTR)(Msg -> Method));
        nh -> Count     = Msg -> Count;

/*** try to resolve the attribute and its value ****************************/

        if (!(TAG_USER & Msg -> Attribute))
        {
            FClassAttribute *attr = F_DynamicFindAttribute((STRPTR)(Msg -> Attribute),_class(Obj),NULL);

            if (attr)
            {
                nh -> Attribute = attr -> ID;

                //if (attr -> Values && ((int32)(nh -> Value) > 0xFFFF) && (uint32)(nh -> Value) != FV_Notify_Always)
                if (attr -> Values && TypeOfMem((APTR)(nh -> Value)))
                {
                    FClassAttributeValue *val;

                    for (val = attr -> Values ; val -> Name ; val++)
                    {
                        if (F_StrCmp((STRPTR)(nh -> Value),val -> Name,ALL) == 0)
                        {
                            nh -> Value = val -> Value; break;
                        }
                    }
                }
            }
        }

/*** try to resolve method *************************************************/
        
        CopyMem((APTR)((uint32)(Msg) + sizeof (struct FS_Notify)),(APTR)((uint32)(nh) + sizeof (struct FeelinNotifyHandler)),Msg -> Count * sizeof (uint32));

///DB_NOTIFY
        #ifdef DB_NOTIFY
        F_Log(0,"Obj 0x%08lx (%s) - Notify 0x%lx - Attribute 0x%lx (0x%08.lx) - Target 0x%lx (0x%lx) - Count %ld",Obj,_class(Obj) -> Name,nh,nh -> Attribute,nh -> Value,nh -> Target,nh -> Method,nh -> Count);
        F_LogA(0,"   >> Msg 0x%08lx %08lx %08lx %08lx %08lx",(APTR)((uint32)(nh) + sizeof (struct FeelinNotifyHandler)));
        #endif
//+

        if (LOD -> Handlers)
        {
            struct FeelinNotifyHandler *prev = LOD -> Handlers;

            while (prev -> Next) prev = prev -> Next;
            prev -> Next = nh;
        }
        else
        {
            LOD -> Handlers = nh;
        }
    }
    else
    {
        F_Log(FV_LOG_USER,"Unable to allocate handler for attribute 0x%08lx",_classname(Obj),Obj,Msg -> Attribute);
    }

    //F_Log(0,"RPool(0x%08lx.%ld)",F_POOL(Owner),F_POOL(NestCount));
    #ifdef CS_LOCK_NOTIFYPOOL
    F_RPool(FeelinBase -> NotifyPool);
    #endif

    return nh;
}
//+
///Object_UnNotify
F_METHODM(void,Object_UnNotify,FS_UnNotify)
{
    struct LocalObjectData      *LOD = F_LOD(Class,Obj);
    struct FeelinNotifyHandler  *nh;
/*
    if ((uint32)(Msg -> Handler) == ALL)
    {
        F_Log(0,"OPool(0x%08lx.%ld) - ALL HANDLERS",F_POOL(Owner),F_POOL(NestCount));
    }
    else
    {
        F_Log(0,"OPool(0x%08lx.%ld) - Handler 0x%08lx - Attribute 0x%08lx (0x%08lx) - Target %s{%08lx}",F_POOL(Owner),F_POOL(NestCount),((struct FeelinNotifyHandler *)(Msg -> Handler)) -> Attribute,((struct FeelinNotifyHandler *)(Msg -> Handler)) -> Value,_classname(((struct FeelinNotifyHandler *)(Msg -> Handler)) -> Target),((struct FeelinNotifyHandler *)(Msg -> Handler)) -> Target);
    }
*/
    #ifdef CS_LOCK_NOTIFYPOOL
    F_OPool(FeelinBase -> NotifyPool);
    #endif

    if ((uint32)(Msg -> Handler) == ALL)
    {
        while ((nh = LOD -> Handlers) != NULL)
        {
            LOD -> Handlers = nh -> Next;
            
            if (FF_NOTIFY_PERFORMING & nh -> Flags)
            {
                F_Log(FV_LOG_DEV,"Trying to remove a notify handler (0x%08lx) under process. The notify handler will not be disposed to ensure the end of the notify process. Check your source code !!\n   >> Attribute (0x%08lx)(%s) - Target %s{%08lx} - Method (0x%08lx)(%s)",nh,nh -> Attribute,f_find_attribute_name(nh -> Attribute,FeelinBase),_classname(nh -> Target),nh -> Target,nh -> Method,f_find_method_name(nh -> Method,FeelinBase));
 
                nh -> Next = NULL;
            }
            else F_Dispose(nh);
        }
    }
    else if (LOD -> Handlers)
    {
        struct FeelinNotifyHandler *prev=NULL;

        for (nh = LOD -> Handlers ; nh ; nh = nh -> Next)
        {
            if (nh == Msg -> Handler)
            {
                if (prev)   prev -> Next = nh -> Next;
                else        LOD -> Handlers = nh -> Next;

                if (FF_NOTIFY_PERFORMING & nh -> Flags)
                {
                    F_Log(FV_LOG_DEV,"Trying to remove a notify handler (0x%08lx) under process. The notify handler will not be disposed to ensure the end of the notify process. Check your source code !!\n   >> Attribute (0x%08lx)(%s) - Target %s{%08lx} - Method (0x%08lx)(%s)",nh,nh -> Attribute,f_find_attribute_name(nh -> Attribute,FeelinBase),_classname(nh -> Target),nh -> Target,nh -> Method,f_find_method_name(nh -> Method,FeelinBase));

                    nh -> Next = NULL;
                }
                else F_Dispose(nh);

                break;
            }
            prev = nh;
        }

        if (!nh)
        {
            F_Log(FV_LOG_DEV,"Unknown Handler 0x%08lx",_classname(Obj),Obj,Msg -> Handler);
        }
    }

//   F_Log(0,"RPool(0x%08lx.%ld)",F_POOL(Owner),F_POOL(NestCount));
    #ifdef CS_LOCK_NOTIFYPOOL
    F_RPool(FeelinBase -> NotifyPool);
    #endif
}
//+

///Object_MultiSet
F_METHODM(void,Object_MultiSet,FS_MultiSet)
{
    FObject *list;

    for (list = (FObject *)((uint32)(Msg) + sizeof (struct FS_MultiSet)) ; *list ; list++)
    {
        F_Set(*list,Msg -> Attribute,Msg -> Value);
    }
}
//+
///Object_SetAs
F_METHODM(void,Object_SetAs,FS_SetAs)
{
    int32 nonotify = (FF_SetAs_Notify & Msg -> Flags) ? FALSE : TRUE;

//    F_Log(0,"FLAGS 0x%08lx >> %ld",Msg -> Flags,Msg -> Flags & 0x0000FFFF);
 
    switch (Msg -> Flags & 0x0000FFFF)
    {
        case FV_SetAs_String:
        {
            STRPTR str = F_StrNewA(NULL,Msg -> Data,(APTR)((uint32)(Msg) + sizeof (struct FS_SetAs)));

//         F_Log(0,"NOTIFY (%ld) STRING (0x%08lx)(%s) >> (%s)",!nonotify,Msg -> Data,Msg -> Data,str);
 
            if (str)
            {
                F_Do(Obj,FM_Set, FA_NoNotify,nonotify, Msg -> Attribute,(uint32)(str), TAG_DONE);

                F_Dispose(str);
            }
        }
        break;
        
        case FV_SetAs_Decimal:
        {
            int32 val=0;
            
            if (Msg -> Data)
            {
                stcd_l(Msg -> Data,&val);
            }
            
//         F_Log(0,"NOTIFY (%ld) DECIMAL (0x%08lx)(%s) >> (%ld)",!nonotify,Msg -> Data,Msg -> Data,val);
            
            F_Do(Obj,FM_Set, FA_NoNotify,nonotify, Msg -> Attribute,val, TAG_DONE);
        }
        break;
    
        case FV_SetAs_Hexadecimal:
        {
            int32 val=0;
            
            if (Msg -> Data)
            {
                stch_l(Msg -> Data,&val);
            }
            
//         F_Log(0,"NOTIFY (%ld) HEX (0x%08lx)(%s) >> (%ld)",!nonotify,Msg -> Data,Msg -> Data,val);

            F_Do(Obj,FM_Set, FA_NoNotify,nonotify, Msg -> Attribute,val, TAG_DONE);
        }
        break;
    
        case FV_SetAs_Constant:
        {
            int32 val=0;
            
            if (Msg -> Data)
            {
                val = *((uint32 *)(Msg -> Data));
            }
            
//         F_Log(0,"NOTIFY (%ld) CONSTANT (0x%08lx)(%s) >> (%08lx)",Msg -> Notify,Msg -> Data,Msg -> Data,*((uint32 *)(Msg -> Data)));
 
            F_Do(Obj,FM_Set, FA_NoNotify,nonotify, Msg -> Attribute,val, TAG_DONE);
        }
        break;
/*
        default:
        {
            F_Log(0,"INVALID TYPE (%ld)",Msg -> Type);
        }
        break;
*/
    }
}
//+
///Object_WriteLong
F_METHODM(void,Object_WriteLong,FS_WriteLong)
{
    *Msg -> Memory = Msg -> Value;
}
//+
///Object_WriteString
F_METHODM(uint32,Object_WriteString,FS_WriteString)
{
    uint32 len = F_StrLen(Msg -> Str);

    CopyMem(Msg -> Str,Msg -> Memory,len + 1);

    return len;
}
//+
///Object_Null
F_METHOD(uint32,Object_Null)
{
    return 0;
}
//+
///Object_CallHook
F_METHODM(uint32,Object_CallHook,FS_CallHook)
{
    if (Msg -> Hook && Msg -> Hook -> h_Entry)
    {
        return CallHookPkt(Msg -> Hook,Obj,++Msg);
    }
    return 0;
}
//+
///Object_CallHookEntry
F_METHODM(uint32,Object_CallHookEntry,FS_CallHookEntry)
{
    if (Msg -> Entry)
    {
        struct Hook hook;

        hook.h_MinNode.mln_Succ = NULL;
        hook.h_MinNode.mln_Pred = NULL;
        hook.h_Entry            = (HOOKFUNC) Msg -> Entry;
        hook.h_SubEntry         = NULL;
        hook.h_Data             = NULL;
 
        return CallHookPkt(&hook,Obj,++Msg);
    }
    return 0;
}
//+

///Object_Lock
F_METHODM(uint32,Object_Lock,FS_Lock)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    Forbid();
 
    if (!LOD -> Atomic)
    {
        LOD -> Atomic = F_New(sizeof (struct SignalSemaphore));

//      F_Log(0,"NEW ATOMIC 0x%08lx",LOD -> Atomic);
 
        if (LOD -> Atomic)
        {
            InitSemaphore(LOD -> Atomic);
        }
    }

    Permit();
    
    if (LOD -> Atomic)
    {
        if (FF_Lock_Attempt & Msg -> Flags)
        {
            if (FF_Lock_Shared & Msg -> Flags)
            {
//            F_Log(0,"AttemptSemaphoreShared 0x%08lx",LOD -> Atomic);
                return AttemptSemaphoreShared(LOD -> Atomic);
            }
            else
            {
//            F_Log(0,"AttemptSemaphore 0x%08lx",LOD -> Atomic);
                return AttemptSemaphore(LOD -> Atomic);
            }
        }
        else if (FF_Lock_Shared & Msg -> Flags)
        {
//         F_Log(0,"ObtainSemaphoreShared 0x%08lx",LOD -> Atomic);
            ObtainSemaphoreShared(LOD -> Atomic);
        }
        else
        {
//         F_Log(0,"ObtainSemaphore 0x%08lx",LOD -> Atomic);
            ObtainSemaphore(LOD -> Atomic);
        }
    }
    return 0;
}
//+
///Object_Unlock
F_METHOD(void,Object_Unlock)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);


    if (LOD -> Atomic)
    {
//      F_Log(0,"Owner (%s)(0x%08lx)",LOD -> Atomic -> ss_Owner -> tc_Node.ln_Name,LOD -> Atomic -> ss_Owner);
        
        ReleaseSemaphore(LOD -> Atomic);
    }
    else
    {
        F_Log(FV_LOG_DEV,"No Semaphore created yet !");
    }
}
//+

/****************************************************************************
*** Constructor *************************************************************
****************************************************************************/

STATIC F_ATTRIBUTES_ARRAY =
{
    F_ATTRIBUTES_ADD_BOTH("Class",        FV_TYPE_POINTER, FA_Class),
    F_ATTRIBUTES_ADD_BOTH("Revision",     FV_TYPE_INTEGER, FA_Revision),
    F_ATTRIBUTES_ADD_BOTH("Version",      FV_TYPE_INTEGER, FA_Version),
    F_ATTRIBUTES_ADD_BOTH("ID",           FV_TYPE_INTEGER, FA_ID),
    F_ATTRIBUTES_ADD_BOTH("UserData",     FV_TYPE_POINTER, FA_UserData),
    F_ATTRIBUTES_ADD_BOTH("NoNotify",     FV_TYPE_BOOLEAN, FA_NoNotify),
    F_ATTRIBUTES_ADD_BOTH("Parent",       FV_TYPE_OBJECT,  FA_Parent),
    F_ATTRIBUTES_ADD_BOTH("Child",        FV_TYPE_OBJECT,  FA_Child),
    F_ATTRIBUTES_ADD_BOTH("ContextHelp",  FV_TYPE_STRING,  FA_ContextHelp),
    F_ATTRIBUTES_ADD_BOTH("ContextMenu",  FV_TYPE_OBJECT,  FA_ContextMenu),
    
    F_ARRAY_END
};

STATIC F_METHODS_ARRAY =
{
    F_METHODS_ADD_BOTH(Object_Set,            "Set",            FM_Set),
    F_METHODS_ADD_BOTH(Object_Get,            "Get",            FM_Get),
    F_METHODS_ADD_BOTH(Object_Lock,           "Lock",           FM_Lock),
    F_METHODS_ADD_BOTH(Object_Unlock,         "Unlock",         FM_Unlock),
    F_METHODS_ADD_BOTH(Object_New,            "New",            FM_New),
    F_METHODS_ADD_BOTH(Object_Dispose,        "Dispose",        FM_Dispose),
    F_METHODS_ADD_BOTH(Object_Notify,         "Notify",         FM_Notify),
    F_METHODS_ADD_BOTH(Object_UnNotify,       "UnNotify",       FM_UnNotify),
    F_METHODS_ADD_BOTH(Object_SetAs,          "SetAs",          FM_SetAs),
    F_METHODS_ADD_BOTH(Object_CallHook,       "CallHook",       FM_CallHook),
    F_METHODS_ADD_BOTH(Object_CallHookEntry,  "CallHookEntry",  FM_CallHookEntry),
    F_METHODS_ADD_BOTH(Object_WriteLong,      "WriteLong",      FM_WriteLong),
    F_METHODS_ADD_BOTH(Object_WriteString,    "WriteString",    FM_WriteString),
    F_METHODS_ADD_BOTH(Object_MultiSet,       "MultiSet",       FM_MultiSet),
    F_METHODS_ADD_BOTH(Object_Null,           "Export",         FM_Export),
    F_METHODS_ADD_BOTH(Object_Null,           "Import",         FM_Import),
    F_METHODS_ADD_BOTH(Object_Null,           "Connect",        FM_Connect),
    F_METHODS_ADD_BOTH(Object_Null,           "Disconnect",     FM_Disconnect),
    F_METHODS_ADD_BOTH(Object_Null,           "AddMember",      FM_AddMember),
    F_METHODS_ADD_BOTH(Object_Null,           "RemMember",      FM_RemMember),
    
    F_ARRAY_END
};

STATIC F_TAGS_ARRAY =
{
    F_TAGS_ADD_LOD,
    F_TAGS_ADD_METHODS,
    F_TAGS_ADD_ATTRIBUTES,
    
    F_ARRAY_END
};

///fc_object_create
FClass * fc_object_create(struct in_FeelinBase *FeelinBase)
{
    return F_CreateClassA(FC_Object,F_TAGS);
}
//+

