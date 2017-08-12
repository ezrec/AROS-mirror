#include "system_headers.h"

struct PortButtonData
{
    struct MsgPort *pbd_Port;
    struct PortEntry pbd_Entry;
};

STATIC void UpdateButton( struct IClass *cl,
                          Object *obj )
{
    struct PortButtonData *pbd = INST_DATA(cl, obj);
    TEXT tmp[FILENAME_LENGTH];

    if (pbd->pbd_Port) {
        stccpy(tmp, nonetest(pbd->pbd_Port->mp_Node.ln_Name), sizeof(tmp));
    } else {
        stccpy(tmp, nonetest(NULL), sizeof(tmp));
    }

    MySetContentsHealed(obj, ADDRESS_FORMAT ": " MUIX_B "%s", pbd->pbd_Port, tmp);
    set(obj, MUIA_Disabled, (pbd->pbd_Port == NULL) ? TRUE : FALSE);
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct PortButtonData *pbd;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg))) {
        return 0;
    }

    pbd = INST_DATA(cl, obj);
    pbd->pbd_Port = (struct MsgPort *)GetTagData(MUIA_PortButton_Port, (IPTR)NULL, msg->ops_AttrList);

    UpdateButton(cl, obj);

    DoMethod(obj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Self, 1, MUIM_PortButton_ShowPort);

    return (IPTR)obj;
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct PortButtonData *pbd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;
    BOOL update = FALSE;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_PortButton_Port:
                pbd->pbd_Port = (struct MsgPort *)tag->ti_Data;
                update = TRUE;
                break;
        }
    }

    if (update) UpdateButton(cl, obj);

    return DoSuperMethodA(cl, obj, (Msg)msg);
}


STATIC IPTR mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct PortButtonData *pbd = INST_DATA(cl, obj);
    IPTR *store = msg->opg_Storage;

    switch (msg->opg_AttrID) {
        case MUIA_PortButton_Port:
            *store = (IPTR)pbd->pbd_Port; return (TRUE);
            break;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

STATIC IPTR mAskMinMax( struct IClass *cl,
                         Object *obj,
                         struct MUIP_AskMinMax *msg )
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth = _font(obj)->tf_XSize * 11;
    msg->MinMaxInfo->DefWidth = _font(obj)->tf_XSize * 11;

    return 0;
}

STATIC IPTR mShowPort( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct PortButtonData *pbd = INST_DATA(cl, obj);
    APTR parentWin;
    APTR portWin;

    parentWin = (APTR)xget(obj, MUIA_WindowObject);

    pbd->pbd_Entry.pe_Addr = pbd->pbd_Port;
    stccpy(pbd->pbd_Entry.pe_Name, nonetest(pbd->pbd_Port->mp_Node.ln_Name), sizeof(pbd->pbd_Entry.pe_Name));

    if ((portWin = (Object *)(PortsDetailWindowObject,
            MUIA_Window_ParentWindow, (IPTR)parentWin,
            MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
        End)) != NULL) {
        COLLECT_RETURNIDS;
        SetAttrs(portWin, MUIA_PortsDetailWin_Port, &pbd->pbd_Entry,
                          MUIA_Window_Open, TRUE,
                          TAG_DONE);
        REISSUE_RETURNIDS;
    }

    return 0;
}

DISPATCHER(PortButtonDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                   return (mNew(cl, obj, (APTR)msg));
        case OM_SET:                   return (mSet(cl, obj, (APTR)msg));
        case OM_GET:                   return (mGet(cl, obj, (APTR)msg));
        case MUIM_AskMinMax:           return (mAskMinMax(cl, obj, (APTR)msg));
        case MUIM_PortButton_ShowPort: return (mShowPort(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakePortButtonClass( void )
{
    return MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct PortButtonData), ENTRY(PortButtonDispatcher));
}

