#include "system_headers.h"

struct MonitorButtonData
{
    struct MonitorSpec *mbd_Monitor;
};

static void UpdateButton( struct IClass *cl,
                          Object *obj )
{
    struct MonitorButtonData *mbd = INST_DATA(cl, obj);
    BOOL disabled;

    if (mbd->mbd_Monitor) {
        disabled = FALSE;
        if (!mbd->mbd_Monitor->ms_Node.xln_Name) {
            MySetContents(obj, ADDRESS_FORMAT"$%08lx", mbd->mbd_Monitor);
        } else {
            MySetContentsHealed(obj, ADDRESS_FORMAT ": " MUIX_B "%s", mbd->mbd_Monitor, mbd->mbd_Monitor->ms_Node.xln_Name);
        }
    } else
        disabled = TRUE;
    set(obj, MUIA_Disabled, disabled);
}

static IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct MonitorButtonData *mbd;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg))) {
        return 0;
    }

    mbd = INST_DATA(cl, obj);
    mbd->mbd_Monitor = (struct MonitorSpec *)GetTagData(MUIA_MonitorButton_MonitorSpec, (IPTR)NULL, msg->ops_AttrList);

    UpdateButton(cl, obj);

    DoMethod(obj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Self, 1, MUIM_MonitorButton_ShowMonitorSpec);

    return (IPTR)obj;
}

static IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct MonitorButtonData *mbd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;
    BOOL update = FALSE;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_MonitorButton_MonitorSpec:
                mbd->mbd_Monitor = (struct MonitorSpec *)tag->ti_Data;
                update = TRUE;
                break;
        }
    }

    if (update) UpdateButton(cl, obj);

    return DoSuperMethodA(cl, obj, (Msg)msg);
}


static IPTR mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct MonitorButtonData *mbd = INST_DATA(cl, obj);
    IPTR *store = msg->opg_Storage;

    switch (msg->opg_AttrID) {
        case MUIA_MonitorButton_MonitorSpec:
            *store = (IPTR)mbd->mbd_Monitor; return (TRUE);
            break;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static IPTR mAskMinMax( struct IClass *cl,
                         Object *obj,
                         struct MUIP_AskMinMax *msg )
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth = _font(obj)->tf_XSize * 11;
    msg->MinMaxInfo->DefWidth = _font(obj)->tf_XSize * 11;

    return 0;
}

static IPTR mShowMonitorSpec( struct IClass *cl,
                               Object *obj,
                               UNUSED Msg msg )
{
    struct MonitorButtonData *mbd = INST_DATA(cl, obj);
    Object *monitorWin;
    Object *parentWin;

    parentWin = (APTR)xget(obj, MUIA_WindowObject);

    if ((monitorWin = (Object *)MonitorsDetailWindowObject,
            MUIA_Window_ParentWindow, (IPTR)parentWin,
            MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
        End) != NULL) {
        COLLECT_RETURNIDS;
        SetAttrs(monitorWin, MUIA_MonitorsDetailWin_MonitorSpec, mbd->mbd_Monitor,
                          MUIA_Window_Open, TRUE,
                          TAG_DONE);
        REISSUE_RETURNIDS;
    }

    return 0;
}

DISPATCHER(MonitorButtonDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                   return (mNew(cl, obj, (APTR)msg));
        case OM_SET:                   return (mSet(cl, obj, (APTR)msg));
        case OM_GET:                   return (mGet(cl, obj, (APTR)msg));
        case MUIM_AskMinMax:           return (mAskMinMax(cl, obj, (APTR)msg));
        case MUIM_MonitorButton_ShowMonitorSpec:
				       return (mShowMonitorSpec(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeMonitorButtonClass( void )
{
    return MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct MonitorButtonData), ENTRY(MonitorButtonDispatcher));
}

