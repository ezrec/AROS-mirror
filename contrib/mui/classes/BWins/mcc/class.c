#define MUIMASTER_YES_INLINE_STDARG

#include "class.h"
#include <utility/pack.h>

/****************************************************************************/

#include "bwin_private.h"

/***********************************************************************/

struct pack
{
    Object *root;
    ULONG  ID;
    ULONG  save;
    ULONG  back;
    ULONG  flags;

    LONG   ls;
    LONG   rs;
    LONG   ts;
    LONG   bs;
};

static ULONG packTable[] =
{
    PACK_STARTTABLE(MUIA_BWin_Base),

    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_Save,pack,save,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_Background,pack,back,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerLeft,pack,ls,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerTop,pack,rs,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerRight,pack,ts,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerBottom,pack,bs,PKCTRL_LONG|PKCTRL_PACKONLY),

    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_NoDragBar,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_NoDragBar),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_NoSize,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_NoSize),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_NoClose,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_NoClose),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_ShowDragBar,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_ShowDragBar),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_ShowSize,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_ShowSize),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_ShowClose,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_ShowClose),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_DragBarTop,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_DragBarTop),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_AutoShow,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_AutoShow),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_NoCloseItem,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_NoCloseItem),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_NoHideItem,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_NoHideItem),
    PACK_LONGBIT(MUIA_BWin_Base,MUIA_BWin_Borders,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Borders),


    PACK_NEWOFFSET(WindowContents),
    PACK_ENTRY(WindowContents,WindowContents,pack,root,PKCTRL_LONG|PKCTRL_PACKONLY),


    PACK_NEWOFFSET(MUIA_Window_ID),
    PACK_ENTRY(MUIA_Window_ID,MUIA_Window_ID,pack,ID,PKCTRL_LONG|PKCTRL_PACKONLY),


    PACK_NEWOFFSET(MUIA_Window_Activate),
    PACK_LONGBIT(MUIA_Window_Activate,MUIA_Window_Activate,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Activate),
};

static ULONG ASM
mNew(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    struct pack             pack;
    struct TagItem *attrs = msg->ops_AttrList, *contTag, *titleTag;
    Object         *contents;
    ULONG          flags, borders;

    pack.root  = NULL;
    pack.ID    = 0;
    pack.save  = MUIDEF_BWin_Save;
    pack.back  = MUIDEF_BWin_Background;
    pack.flags = MUIDEF_BWin_Flags;
    pack.ls    = pack.rs = pack.ts = pack.bs = -1;
    PackStructureTags(&pack,packTable,attrs);

    flags = pack.flags;

    if ((borders = flags & FLG_Borders))
    {
        flags |= FLG_NoDragBar;
        titleTag = NULL;
    }
    else if ((titleTag = FindTagItem(MUIA_Window_Title,attrs))) titleTag->ti_Tag = TAG_IGNORE;


    if ((contTag = FindTagItem(WindowContents,attrs))) contTag->ti_Tag = TAG_IGNORE;

    if (flags & FLG_NoDragBar) flags &= ~FLG_ShowDragBar;
    if (flags & FLG_NoSize) flags &= ~FLG_ShowSize;
    if (flags & FLG_NoClose) flags &= ~FLG_ShowClose;

    if ((obj = (Object *)DoSuperNew(cl,obj,
            borders ? TAG_IGNORE : MUIA_Window_Borderless,   TRUE,
            borders ? TAG_IGNORE : MUIA_Window_CloseGadget,  FALSE,
            borders ? TAG_IGNORE : MUIA_Window_DragBar,      FALSE,
            borders ? TAG_IGNORE : MUIA_Window_DepthGadget,  FALSE,
            MUIA_Window_SizeGadget,                          FALSE,

            MUIA_Window_RootObject, contents = ContentsObject,
                WindowContents,        pack.root,
                MUIA_Window_ID,        pack.ID,
                MUIA_BWin_Flags,       flags,
                MUIA_BWin_Background,  pack.back,
                MUIA_BWin_InnerLeft,   pack.ls,
                MUIA_BWin_InnerTop,    pack.ts,
                MUIA_BWin_InnerRight,  pack.rs,
                MUIA_BWin_InnerBottom, pack.bs,
            End,

            TAG_MORE, attrs)))
    {
        struct data *data = INST_DATA(cl,obj);

        data->root     = pack.root;
        data->contents = contents;
        data->save     = pack.save;
        data->ID       = pack.ID;
        data->flags    = flags;
        data->ls       = pack.ls;
        data->rs       = pack.rs;
        data->ts       = pack.ts;
        data->bs       = pack.bs;
    }

    if (titleTag) titleTag->ti_Tag = MUIA_Window_Title;
    if (contTag) contTag->ti_Tag = WindowContents;

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG ASM
mGet(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opGet *msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch(msg->opg_AttrID)
    {
        case MUIA_Version:          *msg->opg_Storage = VERSION; return TRUE;
        case MUIA_Revision:         *msg->opg_Storage = REVISION; return TRUE;

        case WindowContents:        *msg->opg_Storage = (ULONG)data->root; return TRUE;

        case MUIA_BWin_NoDragBar:   *msg->opg_Storage = (data->flags & FLG_NoDragBar) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_NoSize:      *msg->opg_Storage = (data->flags & FLG_NoSize) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_NoClose:     *msg->opg_Storage = (data->flags & FLG_NoClose) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_ShowDragBar: *msg->opg_Storage = (data->flags & FLG_ShowDragBar) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_ShowSize:    *msg->opg_Storage = (data->flags & FLG_ShowSize) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_ShowClose:   *msg->opg_Storage = (data->flags & FLG_ShowClose) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_NoCloseItem: *msg->opg_Storage = (data->flags & FLG_NoCloseItem) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_NoHideItem:  *msg->opg_Storage = (data->flags & FLG_NoHideItem) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_DragBarTop:  *msg->opg_Storage = (data->flags & FLG_DragBarTop) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_AutoShow:    *msg->opg_Storage = (data->flags & FLG_AutoShow) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_Borders:     *msg->opg_Storage = (data->flags & FLG_Borders) ? TRUE : FALSE; return TRUE;
        case MUIA_BWin_Save:        *msg->opg_Storage = data->save; return TRUE;
        case MUIA_BWin_UserItemID:  *msg->opg_Storage = data->lastid; return TRUE;

        case MUIA_BWin_InnerLeft:   *msg->opg_Storage = data->ls; return TRUE;
        case MUIA_BWin_InnerTop:    *msg->opg_Storage = data->ts; return TRUE;
        case MUIA_BWin_InnerRight:  *msg->opg_Storage = data->rs; return TRUE;
        case MUIA_BWin_InnerBottom: *msg->opg_Storage = data->bs; return TRUE;

        default: return DoSuperMethodA(cl,obj,(APTR)msg);
    }
}

/***********************************************************************/

static ULONG ASM
mSets(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem          *tstate;

    for (tstate = msg->ops_AttrList; (tag = NextTagItem(&tstate)); )
    {
        ULONG tidata = tag->ti_Data;

        switch (tag->ti_Tag)
        {
            case MUIA_Window_Activate:
                if (!BOOLSAME(data->flags & FLG_Activate,tidata))
                {
                    if (tidata) data->flags |= FLG_Activate;
                    else data->flags &= ~FLG_Activate;

                    nfset(data->contents,MUIA_BWin_Show,tidata);
                }
                break;

            case MUIA_Window_Title:
                if (!(data->flags & FLG_Borders)) tag->ti_Tag = TAG_IGNORE;
                break;

            case WindowContents:
                nfset(data->contents,WindowContents,data->root = (Object *)tidata);
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_BWin_ShowDragBar:
                if ((data->flags & FLG_NoDragBar) || BOOLSAME(data->flags & FLG_ShowDragBar,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_ShowDragBar;
                    else data->flags &= ~FLG_ShowDragBar;

                    nfset(data->contents,MUIA_BWin_ShowDragBar,tidata);
                }
                break;

            case MUIA_BWin_ShowSize:
                if ((data->flags & FLG_NoSize) || BOOLSAME(data->flags & FLG_ShowSize,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_ShowSize;
                    else data->flags &= ~FLG_ShowSize;

                    nfset(data->contents,MUIA_BWin_ShowSize,tidata);
                }
                break;

            case MUIA_BWin_ShowClose:
                if ((data->flags & FLG_NoClose) || BOOLSAME(data->flags & FLG_ShowClose,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_ShowClose;
                    else data->flags &= ~FLG_ShowClose;

                    nfset(data->contents,MUIA_BWin_ShowClose,tidata);
                }
                break;

            case MUIA_BWin_AutoShow:
                if (BOOLSAME(data->flags & FLG_AutoShow,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_AutoShow;
                    else data->flags &= ~FLG_AutoShow;

                    nfset(data->contents,MUIA_BWin_AutoShow,tidata);
                }
                break;

            case MUIA_BWin_NoCloseItem:
                if (BOOLSAME(data->flags & FLG_NoCloseItem,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_NoCloseItem;
                    else data->flags &= ~FLG_NoCloseItem;

                    nfset(data->contents,MUIA_BWin_NoCloseItem,tidata);
                }
                break;

            case MUIA_BWin_NoHideItem:
                if (BOOLSAME(data->flags & FLG_NoHideItem,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_NoHideItem;
                    else data->flags &= ~FLG_NoHideItem;

                    nfset(data->contents,MUIA_BWin_NoHideItem,tidata);
                }
                break;

            case MUIA_BWin_DragBarTop:
                if ((data->flags & FLG_NoDragBar) || BOOLSAME(data->flags & FLG_DragBarTop,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_DragBarTop;
                    else data->flags &= ~FLG_DragBarTop;

                    nfset(data->contents,MUIA_BWin_DragBarTop,tidata);
                }
                break;

            case MUIA_BWin_Save:
                if (data->save==tidata) tag->ti_Tag = TAG_IGNORE;
                else data->save = tidata;
                break;

            case MUIA_BWin_UserItemID:
                data->lastid = tidata;
                break;

            case MUIA_BWin_InnerLeft:
                if ((LONG)tidata==data->ls) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->ls = (LONG)tidata;
                    nfset(data->contents,MUIA_BWin_InnerLeft,tidata);
                }
                break;

            case MUIA_BWin_InnerRight:
                if ((LONG)tidata==data->rs) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->rs = (LONG)tidata;
                    nfset(data->contents,MUIA_BWin_InnerRight,tidata);
                }
                break;

            case MUIA_BWin_InnerTop:
                if ((LONG)tidata==data->ts) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->ts = (LONG)tidata;
                    nfset(data->contents,MUIA_BWin_InnerTop,tidata);
                }
                break;

            case MUIA_BWin_InnerBottom:
                if ((LONG)tidata==data->bs) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    data->bs = (LONG)tidata;
                    nfset(data->contents,MUIA_BWin_InnerBottom,tidata);
                }
                break;

            case MUIA_Window_ID:
            case MUIA_BWin_Background:
                nfset(data->contents,tag->ti_Tag,tidata);
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(APTR)msg);
}

/***********************************************************************/

static ULONG ASM
mDoContentsMethod(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    return DoMethodA(data->contents,msg);
}

/***********************************************************************/

enum
{
    EFLG_DragBarTop = 1<<0,
    EFLG_AutoShow   = 1<<1,
    EFLG_DragBar    = 1<<2,
    EFLG_Size       = 1<<3,
    EFLG_Close      = 1<<4,
};

static ULONG ASM
mExport(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Export *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       id;

    if (data->save && (id = (muiNotifyData(obj)->mnd_ObjectID)))
    {
        ULONG e = 0;

        if (!(data->flags & FLG_NoDragBar) || !(data->flags & FLG_NoSize))
        {
            if ((data->save & MUIV_BWin_Save_DragBarTop) && (data->flags & FLG_DragBarTop))
                e |= EFLG_DragBarTop;

            if ((data->save & MUIV_BWin_Save_AutoShow) && (data->flags & FLG_AutoShow))
                e |= EFLG_AutoShow;

            if ((data->save & MUIV_BWin_Save_ShowDragBar) && !(data->flags & FLG_NoDragBar) && (data->flags & FLG_ShowDragBar))
                e |= EFLG_DragBar;

            if ((data->save & MUIV_BWin_Save_ShowSize) && !(data->flags & FLG_NoSize) && (data->flags & FLG_ShowSize))
                e |= EFLG_Size;
        }

        DoMethod(msg->dataspace,MUIM_Dataspace_Add,&e,sizeof(e),id);
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG ASM
mImport(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Import *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       id;

    if (data->save && (id = (muiNotifyData(obj)->mnd_ObjectID)))
    {
        ULONG *e;

        if ((e = (ULONG *)DoMethod(msg->dataspace,MUIM_Dataspace_Find,id)))
        {
            ULONG v = *e;

            if (data->save & MUIV_BWin_Save_DragBarTop)
                set(obj,MUIA_BWin_DragBarTop,v & EFLG_DragBarTop);

            if (data->save & MUIV_BWin_Save_AutoShow)
                set(obj,MUIA_BWin_AutoShow, v & EFLG_AutoShow);

            if (!(data->save & MUIV_BWin_Save_AutoShow) || !(v & EFLG_AutoShow))
            {
                if ((data->save & MUIV_BWin_Save_ShowDragBar) && !(data->flags & FLG_NoDragBar))
                    set(obj,MUIA_BWin_ShowDragBar,v & EFLG_DragBar);

                if ((data->save & MUIV_BWin_Save_ShowSize) && !(data->flags & FLG_NoSize))
                    set(obj,MUIA_BWin_ShowSize,v & EFLG_Size);
            }
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR,dispatcher,cl,obj,msg)
#else
static ULONG ASM SAVEDS
dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
#endif
{
    switch(msg->MethodID)
    {
        case OM_NEW:                   return mNew(cl,obj,(APTR)msg);
        case OM_SET:                   return mSets(cl,obj,(APTR)msg);
        case OM_GET:                   return mGet(cl,obj,(APTR)msg);

        case MUIM_Import:              return mImport(cl,obj,(APTR)msg);
        case MUIM_Export:              return mExport(cl,obj,(APTR)msg);

        case MUIM_BWin_AddUserItem:    return mDoContentsMethod(cl,obj,(APTR)msg);
        case MUIM_BWin_RemoveUserItem: return mDoContentsMethod(cl,obj,(APTR)msg);

        default:                       return DoSuperMethodA(cl,obj,msg);
    }
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

/***********************************************************************/

BOOL ASM
initMCC(void)
{
    if ((lib_mcc = MUI_CreateCustomClass(lib_base,MUIC_Window,NULL,sizeof(struct data),dispatcher)))
    {
        if (MUIMasterBase->lib_Version>=20)
            lib_mcc->mcc_Class->cl_ID = PRG;

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/
