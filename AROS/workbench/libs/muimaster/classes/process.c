/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <clib/alib_protos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/utility.h>

#include "mui.h"
#include "muimaster_intern.h"
#include "support.h"
#include "support_classes.h"
#include "process_private.h"

/*  #define MYDEBUG 1 */
#include "debug.h"

extern struct Library *MUIMasterBase;


IPTR Process__OM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct Process_DATA *data;
    struct TagItem *tags;
    struct TagItem *tag;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
    if (!obj)
        return FALSE;

    data = INST_DATA(cl, obj);

    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags)); )
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Process_AutoLaunch:
                data->autolaunch = tag->ti_Data;
                break;

            case MUIA_Process_Name:
                data->name = tag->ti_Data;
                break;

            case MUIA_Process_Priority:
                data->priority = tag->ti_Data;
                break;

            case MUIA_Process_SourceClass:
                data->sourceclass = tag->ti_Data;
                break;

            case MUIA_Process_SourceObject:
                data->sourceobject = tag->ti_Data;
                break;

            case MUIA_Process_StackSize:
                data->stacksize = tag->ti_Data;
                break;
        }
    }

    D(bug("muimaster.library/process.c: Process Object created at 0x%lx\n",obj));

    return (IPTR)obj;
}


IPTR Process__OM_GET(struct IClass *cl, Object *obj, struct opGet *msg)
{
#define STORE *(msg->opg_Storage)

    struct Process_DATA *data = INST_DATA(cl, obj);

    STORE = (IPTR)0;

    switch(msg->opg_AttrID)
    {
        case MUIA_Process_Task:
            STORE = data->task;
            return TRUE;
    }

    return DoSuperMethodA(cl, obj, (Msg) msg);
#undef STORE
}


IPTR Process__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct Process_DATA *data = INST_DATA(cl, obj);

    return DoSuperMethodA(cl, obj, msg);
}


IPTR Process__MUIM_Process_Kill(struct IClass *cl, Object *obj, struct MUIP_Process_Kill *msg)
{
    struct Process_DATA *data = INST_DATA(cl, obj);

    return 0;
}


IPTR Process__MUIM_Process_Launch(struct IClass *cl, Object *obj, struct MUIP_Process_Launch *msg)
{
    struct Process_DATA *data = INST_DATA(cl, obj);

    return 0;
}


IPTR Process__MUIM_Process_Process(struct IClass *cl, Object *obj, struct MUIP_Process_Process *msg)
{
    struct Process_DATA *data = INST_DATA(cl, obj);

    return 0;
}


IPTR Process__MUIM_Process_Signal(struct IClass *cl, Object *obj, struct MUIP_Process_Signal *msg)
{
    struct Process_DATA *data = INST_DATA(cl, obj);

    return 0;
}


#if ZUNE_BUILTIN_PROCESS
BOOPSI_DISPATCHER(IPTR, Process_Dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
        case OM_NEW:                    return Process__OM_NEW(cl, obj, (struct opSet *)msg);
        case OM_GET:                    return Process__OM_SET(cl, obj, (struct opSet *)msg);
        case OM_DISPOSE:                return Process__OM_DISPOSE(cl, obj, msg);
        case MUIM_Process_Kill:         return Process__MUIM_Process_Kill(cl, obj, (struct MUIP_Process_Kill *)msg);
        case MUIM_Process_Launch:       return Process__MUIM_Process_Launch(cl, obj, (struct MUIP_Process_Launch *)msg);
        case MUIM_Process_Process:      return Process__MUIM_Process_Process(cl, obj, (struct MUIP_Process_Process *)msg);
        case MUIM_Process_Signal:       return Process__MUIM_Process_Signal(cl, obj, (struct MUIP_Process_Signal *)msg);
        default:                        return DoSuperMethodA(cl, obj, msg);
    }
}
BOOPSI_DISPATCHER_END

const struct __MUIBuiltinClass _MUI_Process_desc =
{
    MUIC_Process,
    MUIC_Semaphore,
    sizeof(struct Process_DATA),
    (void*)Process_Dispatcher
};
#endif /* ZUNE_BUILTIN_PROCESS */
