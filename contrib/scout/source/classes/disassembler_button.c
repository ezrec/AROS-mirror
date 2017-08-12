#include "system_headers.h"

struct DisassemblerButtonData
{
    APTR dbd_Address;
    BOOL dbd_ForceHexDump;
};

static void UpdateButton( struct IClass *cl,
                          Object *obj )
{
    struct DisassemblerButtonData *dbd = INST_DATA(cl, obj);
    TEXT tmp[ADDRESS_LENGTH];

    _snprintf(tmp, sizeof(tmp), ADDRESS_FORMAT, dbd->dbd_Address);
    SetAttrs(obj, MUIA_Disabled, !isValidPointer(dbd->dbd_Address),
                  MUIA_Text_Contents, tmp,
                  TAG_DONE);
}

static IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct DisassemblerButtonData *dbd;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg))) {
        return 0;
    }

    dbd = INST_DATA(cl, obj);
    dbd->dbd_Address = (APTR)GetTagData(MUIA_DisassemblerButton_Address, 0, msg->ops_AttrList);
    dbd->dbd_ForceHexDump = GetTagData(MUIA_DisassemblerButton_ForceHexDump, FALSE, msg->ops_AttrList);

    UpdateButton(cl, obj);

    DoMethod(obj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Self, 1, MUIM_DisassemblerButton_Disassemble);

    return (IPTR)obj;
}

static IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct DisassemblerButtonData *dbd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_DisassemblerButton_Address:
                dbd->dbd_Address = (APTR)tag->ti_Data;
                UpdateButton(cl, obj);
                break;

            case MUIA_DisassemblerButton_ForceHexDump:
                dbd->dbd_ForceHexDump = tag->ti_Data;
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}


static IPTR mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct DisassemblerButtonData *dbd = INST_DATA(cl, obj);
    IPTR *store = msg->opg_Storage;

    switch (msg->opg_AttrID) {
        case MUIA_DisassemblerButton_Address:
            *store = (IPTR)dbd->dbd_Address;
            return TRUE;
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

static IPTR mDisassemble( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    struct DisassemblerButtonData *dbd = INST_DATA(cl, obj);
    APTR parentWin;
    APTR disasmWin;

    parentWin = (APTR)xget(obj, MUIA_WindowObject);

    if ((disasmWin = (Object *)DisassemblerWindowObject,
            MUIA_Window_ParentWindow, (IPTR)parentWin,
            MUIA_DisassemblerWin_Address, (IPTR)dbd->dbd_Address,
            MUIA_DisassemblerWin_ForceHexDump, dbd->dbd_ForceHexDump,
        End) != NULL) {
        COLLECT_RETURNIDS;
        DoMethod(disasmWin, MUIM_DisassemblerWin_Disassemble);
        set(disasmWin, MUIA_Window_Open, TRUE);
        REISSUE_RETURNIDS;
    }

    return 0;
}

DISPATCHER(DisassemblerButtonDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                              return (mNew(cl, obj, (APTR)msg));
        case OM_SET:                              return (mSet(cl, obj, (APTR)msg));
        case OM_GET:                              return (mGet(cl, obj, (APTR)msg));
        case MUIM_AskMinMax:                      return (mAskMinMax(cl, obj, (APTR)msg));
        case MUIM_DisassemblerButton_Disassemble: return (mDisassemble(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeDisassemblerButtonClass( void )
{
    return MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct DisassemblerButtonData), ENTRY(DisassemblerButtonDispatcher));
}

