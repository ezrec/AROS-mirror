#include "system_headers.h"

struct FlagsButtonData
{
    STRPTR fbd_Title;
    ULONG fbd_Flags;
    ULONG fbd_Type;
    APTR fbd_BitArray;
    APTR fbd_MaskArray;
    STRPTR fbd_WindowTitle;
};

static void UpdateButton( struct IClass *cl,
                          Object *obj )
{
    struct FlagsButtonData *fbd = INST_DATA(cl, obj);
    char tmp[ADDRESS_LENGTH];

    switch (fbd->fbd_Type) {
        case MUIV_FlagsButton_Type_Byte:
            Flags2BinStr(fbd->fbd_Flags & 0xff, tmp, sizeof(tmp));
            break;

        case MUIV_FlagsButton_Type_Word:
            _snprintf(tmp, sizeof(tmp), "$%04lx", fbd->fbd_Flags & 0xffff);
            break;

        case MUIV_FlagsButton_Type_Long:
            _snprintf(tmp, sizeof(tmp), "$%08lx", fbd->fbd_Flags & 0xffffffff);
            break;
    }

    set(obj, MUIA_Text_Contents, tmp);
}

static IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct FlagsButtonData *fbd;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg))) {
        return 0;
    }

    fbd = INST_DATA(cl, obj);
    fbd->fbd_Title = (STRPTR)GetTagData(MUIA_FlagsButton_Title, (IPTR)NULL, msg->ops_AttrList);
    fbd->fbd_Flags = GetTagData(MUIA_FlagsButton_Flags, 0, msg->ops_AttrList);
    fbd->fbd_BitArray = (APTR)GetTagData(MUIA_FlagsButton_BitArray, (IPTR)NULL, msg->ops_AttrList);
    fbd->fbd_MaskArray = (APTR)GetTagData(MUIA_FlagsButton_MaskArray, (IPTR)NULL, msg->ops_AttrList);
    fbd->fbd_Type = GetTagData(MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte, msg->ops_AttrList);
    fbd->fbd_WindowTitle = (STRPTR)GetTagData(MUIA_FlagsButton_WindowTitle, (IPTR)NULL, msg->ops_AttrList);

    UpdateButton(cl, obj);

    DoMethod(obj, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Self, 1, MUIM_FlagsButton_ShowFlags);

    return (IPTR)obj;
}

static IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct FlagsButtonData *fbd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;
    BOOL update = FALSE;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_FlagsButton_Title:
                fbd->fbd_Title = (STRPTR)tag->ti_Data;
                break;

            case MUIA_FlagsButton_Flags:
                fbd->fbd_Flags = tag->ti_Data;
                update = TRUE;
                break;

            case MUIA_FlagsButton_BitArray:
                fbd->fbd_BitArray = (APTR)tag->ti_Data;
                break;

            case MUIA_FlagsButton_MaskArray:
                fbd->fbd_MaskArray = (APTR)tag->ti_Data;
                break;

            case MUIA_FlagsButton_Type:
                fbd->fbd_Type = tag->ti_Data;
                update = TRUE;
                break;

            case MUIA_FlagsButton_WindowTitle:
                fbd->fbd_WindowTitle = (STRPTR)tag->ti_Data;
                break;
        }
    }

    if (update) {
        UpdateButton(cl, obj);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}


static IPTR mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct FlagsButtonData *fbd = INST_DATA(cl, obj);
    IPTR *store = msg->opg_Storage;

    switch (msg->opg_AttrID) {
        case MUIA_FlagsButton_Flags:
            *store = fbd->fbd_Flags; return (TRUE);
            break;

        case MUIA_FlagsButton_Type:
            *store = fbd->fbd_Type; return (TRUE);
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

static IPTR mShowFlags( struct IClass *cl,
                         Object *obj,
                         UNUSED Msg msg )
{
    struct FlagsButtonData *fbd = INST_DATA(cl, obj);
    APTR parentWin;
    APTR flagsWin;

    parentWin = (APTR)xget(obj, MUIA_WindowObject);

    if ((flagsWin = (Object *)FlagsWindowObject,
            MUIA_Window_Title, (IPTR)fbd->fbd_WindowTitle,
            MUIA_Window_ParentWindow, (IPTR)parentWin,
        End) != NULL) {
        COLLECT_RETURNIDS;
        DoMethod(flagsWin, MUIM_FlagsWin_ShowFlags, fbd->fbd_Flags, fbd->fbd_Type, fbd->fbd_BitArray, fbd->fbd_MaskArray, fbd->fbd_Title);
        set(flagsWin, MUIA_Window_Open, TRUE);
        REISSUE_RETURNIDS;
    }

    return 0;
}

DISPATCHER(FlagsButtonDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                     return (mNew(cl, obj, (APTR)msg));
        case OM_SET:                     return (mSet(cl, obj, (APTR)msg));
        case OM_GET:                     return (mGet(cl, obj, (APTR)msg));
        case MUIM_AskMinMax:             return (mAskMinMax(cl, obj, (APTR)msg));
        case MUIM_FlagsButton_ShowFlags: return (mShowFlags(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeFlagsButtonClass( void )
{
    return MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct FlagsButtonData), ENTRY(FlagsButtonDispatcher));
}

