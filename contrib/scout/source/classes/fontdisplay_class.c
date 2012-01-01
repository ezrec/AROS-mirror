#include "system_headers.h"

struct FontDisplayData
{
    struct TextFont *fdd_Font;
    STRPTR fdd_Text;
    struct Library *fdd_LayersBase;
};

#define LayersBase      fdd->fdd_LayersBase

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct FontDisplayData *fdd;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg)))
        return 0;

    fdd = INST_DATA(cl, obj);

    /* parse initial taglist */
    fdd->fdd_Font = (struct TextFont *)GetTagData(MUIA_FontDisplay_Font, (IPTR)NULL, msg->ops_AttrList);
    fdd->fdd_Text = _strdup_pool((STRPTR)GetTagData(MUIA_FontDisplay_Text, (IPTR)NULL, msg->ops_AttrList), globalPool);

    if (!(fdd->fdd_LayersBase = OpenLibrary("layers.library", 37))) {
        CoerceMethod(cl, obj, OM_DISPOSE);
        obj = NULL;
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct FontDisplayData *fdd = INST_DATA(cl, obj);

    if (fdd->fdd_Text) tbFreeVecPooled(globalPool, fdd->fdd_Text);
    if (fdd->fdd_LayersBase) CloseLibrary(fdd->fdd_LayersBase);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct FontDisplayData *fdd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;
    BOOL redraw = FALSE;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_FontDisplay_Font:
                fdd->fdd_Font = (struct TextFont *)tag->ti_Data;
                redraw = TRUE;
                break;

            case MUIA_FontDisplay_Text:
                if (fdd->fdd_Text) {
                    tbFreeVecPooled(globalPool, fdd->fdd_Text);
                    fdd->fdd_Text = NULL;
                }
                fdd->fdd_Text = _strdup_pool((STRPTR)tag->ti_Data, globalPool);
                redraw = TRUE;
                break;
        }
    }

    if (redraw) {
        MUI_Redraw(obj, MADF_DRAWOBJECT); /* redraw ourselves completely */
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}


STATIC IPTR mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct FontDisplayData *fdd = INST_DATA(cl, obj);
    IPTR *store = msg->opg_Storage;

    switch (msg->opg_AttrID) {
        case MUIA_FontDisplay_Font:
            *store = (IPTR)fdd->fdd_Font; return (TRUE);
            break;

        case MUIA_FontDisplay_Text:
            *store = (IPTR)fdd->fdd_Text; return (TRUE);
            break;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

STATIC IPTR mAskMinMax( struct IClass *cl,
                         Object *obj,
                         struct MUIP_AskMinMax *msg )
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth  += 100;
    msg->MinMaxInfo->DefWidth  += 100;
    msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;

    msg->MinMaxInfo->MinHeight += 60;
    msg->MinMaxInfo->DefHeight += 60;
    msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;

    return 0;
}

STATIC IPTR mDraw( struct IClass *cl,
                    Object *obj,
                    struct MUIP_Draw *msg )
{
    struct FontDisplayData *fdd = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (fdd->fdd_Font != NULL && fdd->fdd_Text != NULL) {
        APTR handle;

        SetFont(_rp(obj), fdd->fdd_Font);
        SetAPen(_rp(obj), _pens(obj)[MPEN_TEXT]);

        handle = MUI_AddClipping(muiRenderInfo(obj), _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj));

        if (fdd->fdd_Font->tf_YSize > _mheight(obj)) {
            Move(_rp(obj), _mleft(obj), _mtop(obj) + fdd->fdd_Font->tf_Baseline);
        } else {
            Move(_rp(obj), _mleft(obj), _mtop(obj) + ((_mheight(obj) - fdd->fdd_Font->tf_YSize) / 2) + fdd->fdd_Font->tf_Baseline);
        }
        Text(_rp(obj), fdd->fdd_Text, strlen(fdd->fdd_Text));

        MUI_RemoveClipping(muiRenderInfo(obj), handle);
    }

    return 0;
}

DISPATCHER(FontDisplayDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW        : return (mNew      (cl, obj, (APTR)msg));
        case OM_DISPOSE    : return (mDispose  (cl, obj, (APTR)msg));
        case OM_SET        : return (mSet      (cl, obj, (APTR)msg));
        case OM_GET        : return (mGet      (cl, obj, (APTR)msg));
        case MUIM_AskMinMax: return (mAskMinMax(cl, obj, (APTR)msg));
        case MUIM_Draw     : return (mDraw     (cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeFontDisplayClass( void )
{
    return MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(struct FontDisplayData), ENTRY(FontDisplayDispatcher));
}

