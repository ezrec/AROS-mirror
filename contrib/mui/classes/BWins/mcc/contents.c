
#define MUIMASTER_YES_INLINE_STDARG

#include "class.h"
#include <utility/pack.h>
#include <libraries/gadtools.h>

/***********************************************************************/

struct data
{
    struct IClass *cl;
    Object        *this;

    Object        *close;
    Object        *dragBar;
    Object        *root;
    Object        *size;

    APTR          menu;
    APTR          ubar;
    APTR          umenu;
    APTR          usub;
    Object        *mparent;
    ULONG         uitems;
    ULONG         subids;

    LONG          shine;
    LONG          shadow;

    LONG          ls;
    LONG          rs;
    LONG          ts;
    LONG          bs;

    LONG          uls;
    LONG          urs;
    LONG          uts;
    LONG          ubs;

    ULONG         ID;
    ULONG         flags;
};

enum
{
    FLG_Setup      = 1<<FLG_AvailableBit,
    FLG_Over       = 1<<(FLG_AvailableBit+1),
    FLG_PMenu      = 1<<(FLG_AvailableBit+2),

    FLG_DragBarRem = 1<<(FLG_AvailableBit+3),
    FLG_SizeRem    = 1<<(FLG_AvailableBit+4),
};

/***********************************************************************/

static IPTR ASM SAVEDS
layoutFun(REG(a0) struct Hook *h,REG(a2) Object *obj,REG(a1) struct MUI_LayoutMsg *lm)
{
    struct data *data = INST_DATA(lib_contents->mcc_Class,obj);

    switch (lm->lm_Type)
    {
        case MUILM_MINMAX:
        {
            ULONG ls, rs, ts, bs, dragBar, size, dbt;
            ULONG          root = 0;
            UWORD minWidth, defWidth, maxWidth, minHeight, defHeight, maxHeight, over;

            ls = (data->ls>=0) ? data->ls : data->uls;
            rs = (data->rs>=0) ? data->rs : data->urs;
            ts = (data->ts>=0) ? data->ts : data->uts;
            bs = (data->bs>=0) ? data->bs : data->ubs;

            dragBar = !(data->flags & FLG_NoDragBar) && (data->flags & FLG_ShowDragBar);
            dbt = data->flags & FLG_DragBarTop;

            if (data->root) get(data->root,MUIA_ShowMe,&root);
            else root = 0;

            size = !(data->flags & FLG_NoSize) && (data->flags & FLG_ShowSize);

            over = (data->flags & FLG_Over) ? 1 : 0;

            /* width */
            minWidth = ls+rs+2;
            if (dragBar && !dbt) minWidth += _minwidth(data->dragBar);
            if ((minWidth<MUI_MAXMAX) && root) minWidth += _minwidth(data->root);
            if ((minWidth<MUI_MAXMAX) && size) minWidth += _minwidth(data->size);

            defWidth = ls+rs+2;
            if (dragBar && !dbt) defWidth += _defwidth(data->dragBar);
            if ((defWidth<MUI_MAXMAX) && root) defWidth += _defwidth(data->root);
            if ((defWidth<MUI_MAXMAX) && size) defWidth += _defwidth(data->size);

            maxWidth = ls+rs+2;
            if (dragBar && !dbt) maxWidth += _maxwidth(data->dragBar);
            if ((maxWidth<MUI_MAXMAX) && root) maxWidth += _maxwidth(data->root);
            if ((maxWidth<MUI_MAXMAX) && size) maxWidth += _maxwidth(data->size);

            /* height */
            minHeight = 0;
            if (dragBar && !dbt && (minHeight<_minheight(data->dragBar))) minHeight = _minheight(data->dragBar);
            if (root && (minHeight<_minheight(data->root))) minHeight = _minheight(data->root);
            if (size && (minHeight<_minheight(data->size))) minHeight = _minheight(data->size);
            minHeight += ts+bs+((dragBar && dbt) ? _minheight(data->dragBar) : 0)+2;

            defHeight = 0;
            if (dragBar && !dbt && (defHeight<_defheight(data->dragBar))) defHeight = _defheight(data->dragBar);
            if (root && (defHeight<_defheight(data->root))) defHeight = _defheight(data->root);
            if (size && (defHeight<_defheight(data->size))) defHeight = _defheight(data->size);
            defHeight += ts+bs+((dragBar && dbt) ? _defheight(data->dragBar) : 0)+2;

            maxHeight = 0;
            if (root) maxHeight = _maxheight(data->root);
            if (size && (maxHeight<_maxheight(data->size))) maxHeight = _maxheight(data->size);
            if (maxHeight<MUI_MAXMAX) maxHeight += ts+bs+((dragBar && dbt) ? _maxheight(data->dragBar) : 0)+2;

            /*kprintf("mM - minWidth:%ld defWidth:%ld maxWidth:%ld minHeight:%ld defHeight:%ld maxHeight:%ld\n",
                minWidth,
                defWidth,
                maxWidth,
                minHeight,
                defHeight,
                maxHeight);*/

            lm->lm_MinMax.MinWidth  = minWidth;
            lm->lm_MinMax.DefWidth  = defWidth;
            lm->lm_MinMax.MaxWidth  = maxWidth;
            lm->lm_MinMax.MinHeight = minHeight;
            lm->lm_MinMax.DefHeight = defHeight;
            lm->lm_MinMax.MaxHeight = maxHeight;

            return 0;
        }

        case MUILM_LAYOUT:
        {
            Object *o;
            ULONG  ls, rs, ts, bs, x, y, width, height, dragBar, dbt, size, tw, th;
            ULONG           root = 0;
            UWORD  over;

            ls = (data->ls>=0) ? data->ls : data->uls;
            rs = (data->rs>=0) ? data->rs : data->urs;
            ts = (data->ts>=0) ? data->ts : data->uts;
            bs = (data->bs>=0) ? data->bs : data->ubs;

            dragBar = !(data->flags & FLG_NoDragBar) && (data->flags & FLG_ShowDragBar);
            dbt = data->flags & FLG_DragBarTop;

            if (data->root) get(data->root,MUIA_ShowMe,&root);
            else root = 0;

            size = !(data->flags & FLG_NoSize) && (data->flags & FLG_ShowSize);

            over = (data->flags & FLG_Over) ? 1 : 0;

            /*kprintf("Group - width:%ld minwidth:%ld defwidth:%ld maxwidth:%ld\nheight:%ld minheight:%ld defheight:%ld maxheight:%ld\n",
                _width(obj),_minwidth(obj),_defwidth(obj),_maxwidth(obj),
                _height(obj),_minheight(obj),_defheight(obj),_maxheight(obj));*/

            tw = _width(obj)-ls-rs-2;
            th = _height(obj)-ts-bs-2;

            x = 1;
            y = 1;

            if (dragBar)
            {
                o = data->dragBar;

                /*kprintf("DragBar - width:%ld minwidth:%ld defwidth:%ld maxwidth:%ld\nheight:%ld minheight:%ld defheight:%ld maxheight:%ld\n",
                    _width(o),_minwidth(o),_defwidth(o),_maxwidth(o),
                    _height(o),_minheight(o),_defheight(o),_maxheight(o));*/

                if (dbt)
                {
                    width = _width(obj)-(over ? 0 : 2);
                    height = _defheight(o);
                }
                else
                {
                    width = _defwidth(o);
                    height = _height(obj)-(over ? 0 : 2);
                }

                /*kprintf("DragBar: %ld %ld %ld %ld\n",x-over,y-over,width,height);*/
                if (!MUI_Layout(o,x-over,y-over,width,height,0)) return FALSE;

                if (dbt)
                {
                    y += height;
                    th -= height;
                }
                else
                {
                    x += width;
                    tw -= width;
                }
            }

            x += ls;
            y += ts;

            if (size)
            {
                ULONG x, y;

                o = data->size;

                /*kprintf("Size - width:%ld minwidth:%ld defwidth:%ld maxwidth:%ld\nheight:%ld minheight:%ld defheight:%ld maxheight:%ld\n",
                    _width(o),_minwidth(o),_defwidth(o),_maxwidth(o),
                    _height(o),_minheight(o),_defheight(o),_maxheight(o));*/

                x = _width(obj)-1-_defwidth(o);
                y = _height(obj)-1-_defheight(o);
                width = _defwidth(o);
                height = _defheight(o);

                /*kprintf("Size: %ld %ld %ld %ld\n",x,y,width,height);*/
                if (!MUI_Layout(o,x,y,width,height,0)) return FALSE;
                tw -= width;
            }

            if (root)
            {
                o = data->root;

                /*kprintf("Root - width:%ld minwidth:%ld defwidth:%ld maxwidth:%ld\nheight:%ld minheight:%ld defheight:%ld maxheight:%ld\n",
                    _width(o),_minwidth(o),_defwidth(o),_maxwidth(o),
                    _height(o),_minheight(o),_defheight(o),_maxheight(o));*/

                width = tw;
                height = th;

                /*kprintf("Root: %ld %ld %ld %ld\n",x,y,width,height);*/
                if (!MUI_Layout(o,x,y,width,height,0)) return FALSE;
            }

            return TRUE;
        }
    }

    return MUILM_UNKNOWN;
}

static struct Hook layoutHook = {{NULL,NULL},(HOOKFUNC)layoutFun,NULL,NULL};

/***********************************************************************/

#define MBAR1 (ULONG)(-1)
#define MBAR2 (ULONG)(-2)
#define MBAR3 (ULONG)(-3)
#define MBAR4 (ULONG)(-4)

static struct NewMenu nmenu[] =
{
    MTITLE(MCCMSG_BWin,MCCMSG_BWin),
        MITEM(MCCMSG_ToFront,MCCMSG_ToFront),
        MITEM(MCCMSG_ToBack,MCCMSG_ToBack),
        MBAR(MBAR1),
        MITEM(MCCMSG_HideWin,MCCMSG_HideWin),
        MITEM(MCCMSG_CloseWin,MCCMSG_CloseWin),
        MBAR(MBAR2),
        MTITEM(MCCMSG_DragBarTop,MCCMSG_DragBarTop),
        MTITEM(MCCMSG_AutoShow,MCCMSG_AutoShow),
        MTITEM(MCCMSG_DragBar,MCCMSG_DragBar),
        MTITEM(MCCMSG_Size,MCCMSG_Size),
        MBAR(MBAR3),
        MITEM(MCCMSG_Snapshot,MCCMSG_Snapshot),
        MITEM(MCCMSG_Unsnapshot,MCCMSG_Unsnapshot),
        MBAR(MBAR4),
        MITEM(MCCMSG_Settings,MCCMSG_Settings),
    MEND
};

struct pack
{
    Object *root;
    ULONG  ID;
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

    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_Background,pack,back,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_Flags,pack,flags,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerLeft,pack,ls,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerTop,pack,rs,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerRight,pack,ts,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(MUIA_BWin_Base,MUIA_BWin_InnerBottom,pack,bs,PKCTRL_LONG|PKCTRL_PACKONLY),

    PACK_NEWOFFSET(WindowContents),
    PACK_ENTRY(WindowContents,WindowContents,pack,root,PKCTRL_LONG|PKCTRL_PACKONLY),


    PACK_NEWOFFSET(MUIA_Window_ID),
    PACK_ENTRY(MUIA_Window_ID,MUIA_Window_ID,pack,ID,PKCTRL_LONG|PKCTRL_PACKONLY),


    PACK_ENDTABLE
};

static IPTR ASM
mNew(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    struct pack             pack;
    struct data             temp;
    struct TagItem *attrs = msg->ops_AttrList;
    ULONG          flags;

    memset(&pack,0,sizeof(pack));
    pack.ls = pack.rs = pack.ts = pack.bs = -1;
    PackStructureTags(&pack,packTable,attrs);

    memset(&temp,0,sizeof(temp));
    temp.root  = pack.root;
    temp.flags = flags = pack.flags;

    if ((obj = (Object *)DoSuperNew(cl,obj,
           MUIA_Group_LayoutHook, &layoutHook,
           MUIA_Background,       pack.back,
           MUIA_InnerLeft,        0,
           MUIA_InnerRight,       0,
           MUIA_InnerTop,         0,
           MUIA_InnerBottom,      0,
           MUIA_Group_Spacing,    0,

           (flags & FLG_NoDragBar) ? TAG_IGNORE : Child, temp.dragBar = SysGadObject,
               MUIA_BWin_Type,     GTYP_WDRAGGING,
               MUIA_BWin_Activate, flags & FLG_Activate,
           End,

           temp.root ? Child : TAG_IGNORE, temp.root,

           (flags & FLG_NoSize) ? TAG_IGNORE : Child, temp.size = SysGadObject,
               MUIA_BWin_Type,     GTYP_SIZING,
               MUIA_BWin_Activate, flags & FLG_Activate,
           End,

           TAG_MORE, attrs)))
    {
        struct data *data = INST_DATA(cl,obj);
        APTR        menu;

        *data = temp;

        data->cl     = cl;
        data->this   = obj;
        data->subids = 1024;
        data->ID     = pack.ID;

        data->ls     = pack.ls;
        data->rs     = pack.rs;
        data->ts     = pack.ts;
        data->bs     = pack.bs;

        if (flags & FLG_Activate) data->flags &= ~FLG_Activate;
        else data->flags |= FLG_Activate;

        nfset(obj,MUIA_BWin_Show,!(data->flags & FLG_Activate));

        if (PopupMenuBase &&
            ((menu = PMMenu(nmenu[0].nm_Label),
                PMItem(nmenu[1].nm_Label), PM_ID, nmenu[1].nm_UserData, PMEnd,
                PMItem(nmenu[2].nm_Label), PM_ID, nmenu[2].nm_UserData, PMEnd,
                PMDBar(MBAR1),
                PMItem(nmenu[4].nm_Label), PM_ID, nmenu[4].nm_UserData, PMEnd,
                PMItem(nmenu[5].nm_Label), PM_ID, nmenu[5].nm_UserData, PMEnd,
                PMDBar(MBAR2),
                (flags & FLG_NoDragBar) ? TAG_IGNORE : PMCheckItem(nmenu[7].nm_Label,nmenu[7].nm_UserData), PMEnd,
                ((flags & FLG_NoDragBar) && (flags & FLG_NoSize)) ? TAG_IGNORE : PMCheckItem(nmenu[8].nm_Label,nmenu[8].nm_UserData),   PMEnd,
                (flags & FLG_NoDragBar) ? TAG_IGNORE : PMCheckItem(nmenu[9].nm_Label,nmenu[9].nm_UserData),   PMEnd,
                (flags & FLG_NoSize) ? TAG_IGNORE : PMCheckItem(nmenu[10].nm_Label,nmenu[10].nm_UserData), PMEnd,
                ((flags & FLG_NoDragBar) && (flags & FLG_NoSize)) ? TAG_IGNORE : PMDBar(MBAR3),
                PMItem(nmenu[12].nm_Label), PM_ID, nmenu[12].nm_UserData, PMEnd,
                PMItem(nmenu[13].nm_Label), PM_ID, nmenu[13].nm_UserData, PMEnd,
                PMDBar(MBAR4),
                PMItem(nmenu[15].nm_Label), PM_ID, nmenu[15].nm_UserData, PMEnd,
                PMEnd
             )))
        {
            data->flags |= FLG_PMenu;
        }
        else
        {
            if ((menu = MUI_MakeObject(MUIO_MenustripNM,nmenu,0)))
            {
                Object *o;

                data->mparent = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_BWin);

                if (flags & (FLG_NoDragBar|FLG_NoSize))
                {
                    if ((flags & FLG_NoDragBar) && (flags & FLG_NoSize))
                    {
                        o = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBarTop);
                        DoMethod(menu,MUIM_Family_Remove,o);
                        MUI_DisposeObject(o);

                        o = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_AutoShow);
                        DoMethod(menu,MUIM_Family_Remove,o);
                        MUI_DisposeObject(o);

                        o = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBar);
                        DoMethod(menu,MUIM_Family_Remove,o);
                        MUI_DisposeObject(o);

                        o = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_Size);
                        DoMethod(menu,MUIM_Family_Remove,o);
                        MUI_DisposeObject(o);

                        o = (Object *)DoMethod(menu,MUIM_FindUData,MBAR3);
                        DoMethod(menu,MUIM_Family_Remove,o);
                        MUI_DisposeObject(o);
                    }
                    else
                        if (flags & FLG_NoDragBar)
                        {
                            o = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBarTop);
                            DoMethod(menu,MUIM_Family_Remove,o);
                            MUI_DisposeObject(o);

                            o = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBar);
                            DoMethod(menu,MUIM_Family_Remove,o);
                            MUI_DisposeObject(o);
                        }
                        else
                        {
                            o = (Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_Size);
                            DoMethod(menu,MUIM_Family_Remove,o);
                            MUI_DisposeObject(o);
                        }
                }
            }
        }

        if (menu)
        {
            data->menu = menu;
            SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_ContextMenu,TRUE,TAG_DONE);
        }
    }

    return (IPTR)obj;
}

/***************************************************************************/

static IPTR ASM
mDispose(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->menu) {
        if (data->flags & FLG_PMenu) PM_FreePopupMenu(data->menu);
        else MUI_DisposeObject(data->menu);
    }

    return DoSuperMethodA(cl,obj,(APTR)msg);
}

/***************************************************************************/

static IPTR ASM
mSets(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    const struct TagItem *tstate;
    ULONG          au, show, layout, res;

    for (au = layout = FALSE, tstate = msg->ops_AttrList; (tag = NextTagItem(&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch (tag->ti_Tag)
        {
            case MUIA_BWin_Show:
                if (BOOLSAME(data->flags & FLG_Activate,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    au = TRUE;
                    if (tidata) data->flags |= FLG_Activate;
                    else data->flags &= ~FLG_Activate;
                }
                break;

            case MUIA_BWin_ShowDragBar:
                if ((data->flags & FLG_NoDragBar) || BOOLSAME(data->flags & FLG_ShowDragBar,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_ShowDragBar;
                    else data->flags &= ~FLG_ShowDragBar;

                    layout = TRUE;
                }
                break;

            case MUIA_BWin_ShowSize:
                if ((data->flags & FLG_NoSize) || BOOLSAME(data->flags & FLG_ShowSize,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_ShowSize;
                    else data->flags &= ~FLG_ShowSize;

                    layout = TRUE;
                }
                break;

            case MUIA_BWin_ShowClose:
                if ((data->flags & FLG_NoClose) || BOOLSAME(data->flags & FLG_ShowClose,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_ShowClose;
                    else data->flags &= ~FLG_ShowClose;

                    layout = TRUE;
                }
                break;

            case MUIA_BWin_AutoShow:
                if (tidata)
                {
                    au = TRUE;
                    data->flags |= FLG_AutoShow;
                }
                else data->flags &= ~FLG_AutoShow;
                break;

            case MUIA_BWin_NoCloseItem:
                if (tidata) data->flags |= FLG_NoCloseItem;
                else data->flags &= ~FLG_NoCloseItem;
                break;

            case MUIA_BWin_NoHideItem:
                if (tidata) data->flags |= FLG_NoHideItem;
                else data->flags &= ~FLG_NoHideItem;
                break;

            case MUIA_BWin_DragBarTop:
                if (tidata) data->flags |= FLG_DragBarTop;
                else data->flags &= ~FLG_DragBarTop;
                set(data->dragBar,MUIA_BWin_Horiz,tidata);
                layout = TRUE;
                break;

            case MUIA_BWin_InnerLeft:
                data->ls = (LONG)tidata;
                layout = TRUE;
                break;

            case MUIA_BWin_InnerRight:
                data->rs = (LONG)tidata;
                layout = TRUE;
                break;

            case MUIA_BWin_InnerTop:
                data->ts = (LONG)tidata;
                layout = TRUE;
                break;

            case MUIA_BWin_InnerBottom:
                data->bs = (LONG)tidata;
                layout = TRUE;
                break;

            case MUIA_BWin_Background:
                SetSuperAttrs(cl,obj,MUIA_Group_Forward,FALSE,MUIA_Background,tidata,TAG_DONE);
                break;

            case WindowContents:
                if (data->flags & FLG_Setup)
                {
                    DoSuperMethod(cl,obj,MUIM_Group_InitChange);
                    DoSuperMethod(cl,obj,OM_REMMEMBER,data->root);
                    DoSuperMethod(cl,obj,OM_ADDMEMBER,tidata);
                }
                else DoSuperMethod(cl,obj,OM_REMMEMBER,data->root);

                data->root = (Object *)tidata;

                if (data->flags & FLG_Setup) DoSuperMethod(cl,obj,MUIM_Group_ExitChange);
                else DoSuperMethod(cl,obj,OM_ADDMEMBER,tidata);
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_Window_ID:
                data->ID = tidata;
                break;
        }
    }

    res = DoSuperMethodA(cl,obj,(APTR)msg);

    /*
    ** If you think that the following is crazy, beleave me MUI has a serious
    ** bug in Background optimization code (something about that is in MUIUndoc.guide)
    */

    show = data->flags & FLG_Activate;

    if (au && (data->flags & FLG_AutoShow) && (show ? (!(data->flags & FLG_ShowDragBar) || !(data->flags & FLG_ShowSize)) : TRUE))
    {
        DoSuperMethod(cl,obj,MUIM_Group_InitChange);
        SetAttrs(_win(obj),MUIA_BWin_ShowDragBar,show,MUIA_BWin_ShowSize,show,TAG_DONE);
        DoSuperMethod(cl,obj,MUIM_Group_ExitChange);
    }
    else
        if (layout)
        {
            DoSuperMethod(cl,obj,MUIM_Group_InitChange);

            /* DragBar uses backgrounds so it is crazy... */
            if (!(data->flags & FLG_NoDragBar))
            {
                if (data->flags & FLG_ShowDragBar)
                {
                    if (data->flags & FLG_DragBarRem)
                    {
                        set(data->dragBar,MUIA_BWin_Activate,show);
                        DoMethod(obj,OM_ADDMEMBER,data->dragBar);
                        data->flags &= ~FLG_DragBarRem;
                        set(data->dragBar,MUIA_BWin_Activate,show);
                    }
                }
                else
                {
                    if (!(data->flags & FLG_DragBarRem))
                    {
                        set(data->dragBar,MUIA_BWin_Activate,FALSE);
                        DoMethod(obj,OM_REMMEMBER,data->dragBar);
                        data->flags |= FLG_DragBarRem;
                    }
                }
            }

            /* Size doesn't use backgrounds so it is normal */
            if (!(data->flags & FLG_NoSize))
            {
                if (data->flags & FLG_ShowSize)
                {
                    if (data->flags & FLG_SizeRem)
                    {
                        DoMethod(obj,OM_ADDMEMBER,data->size);
                        data->flags &= ~FLG_SizeRem;
                        set(data->size,MUIA_BWin_Activate,show);
                    }
                }
                else
                {
                    if (!(data->flags & FLG_SizeRem))
                    {
                        DoMethod(obj,OM_REMMEMBER,data->size);
                        data->flags |= FLG_SizeRem;
                    }
                }
            }

            DoSuperMethod(cl,obj,MUIM_Group_ExitChange);
        }
        else
        {
            if (!(data->flags & FLG_NoDragBar)) set(data->dragBar,MUIA_BWin_Activate,show);
            if (!(data->flags & FLG_NoSize)) set(data->size,MUIA_BWin_Activate,show);
        }

    return res;
}

/***********************************************************************/

static IPTR ASM
mSetup(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR                 pen;
    ULONG       v, top;
    ULONG                *val;

    if (!data->root) return FALSE;

    v = DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_Spacing,&val) ? *val : MUIDEF_BWin_Spacing;
    data->uls = GETLEFTSP(v);
    data->urs = GETRIGHTSP(v);
    data->uts = GETTOPSP(v);
    data->ubs = GETBOTTOMSP(v);

    v = DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_Types,&val) ? *val : MUIDEF_BWin_Types;
    if (GETUSEOVER(v)) data->flags |= FLG_Over;
    else data->flags &= ~FLG_Over;

    top = data->flags & FLG_DragBarTop;
    set(data->dragBar,MUIA_BWin_Horiz,top);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_WinShinePen,&pen)) pen = MUIDEF_BWin_WinShinePen;
    data->shine = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

    if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_WinShadowPen,&pen)) pen = MUIDEF_BWin_WinShadowPen;
    data->shadow = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

    data->flags |= FLG_Setup;

    return TRUE;
}

/***********************************************************************/

static IPTR ASM
mCleanup(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    MUI_ReleasePen(muiRenderInfo(obj),data->shadow);
    MUI_ReleasePen(muiRenderInfo(obj),data->shine);

    data->flags &= ~FLG_Setup;

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG SAVEDS ASM
menuChoice(REG(a0) struct data *data,REG(d0) ULONG id)
{
    Object *obj = data->this;

    switch (id)
    {
        case MCCMSG_ToFront:
            DoMethod(_win(obj),MUIM_Window_ToFront);
            break;

        case MCCMSG_ToBack:
            DoMethod(_win(obj),MUIM_Window_ToBack);
            break;

        case MCCMSG_HideWin:
            DoMethod(_win(obj),MUIM_Window_ActionIconify);
            break;

        case MCCMSG_CloseWin:
            set(_win(obj),MUIA_Window_CloseRequest,TRUE);
            break;

        case MCCMSG_DragBarTop:
            set(_win(obj),MUIA_BWin_DragBarTop,(data->flags & FLG_DragBarTop) ? FALSE : TRUE);
            break;

        case MCCMSG_AutoShow:
            set(_win(obj),MUIA_BWin_AutoShow,(data->flags & FLG_AutoShow) ? FALSE : TRUE);
            if (!(data->flags & FLG_AutoShow)) nfset(obj,MUIA_BWin_Show,TRUE);
            break;

        case MCCMSG_DragBar:
            set(_win(obj),MUIA_BWin_ShowDragBar,(data->flags & FLG_ShowDragBar) ? FALSE : TRUE);
            break;

        case MCCMSG_Size:
            set(_win(obj),MUIA_BWin_ShowSize,(data->flags & FLG_ShowSize) ? FALSE : TRUE);
            break;

        case MCCMSG_Snapshot:
            DoMethod(_win(obj),MUIM_Window_Snapshot,1);
            break;

        case MCCMSG_Unsnapshot:
            DoMethod(_win(obj),MUIM_Window_Snapshot,0);
            break;

        case MCCMSG_Settings:
            set(_app(obj),MUIA_Application_Sleep,TRUE);
            DoMethod(_app(obj),MUIM_Application_OpenConfigWindow,0);
            set(_app(obj),MUIA_Application_Sleep,FALSE);
            break;

        default:
            set(_win(obj),MUIA_BWin_UserItemID,id);
            break;
    }

    return 0;
}

/***********************************************************************/

ULONG ASM SAVEDS
pmenuFunc(REG(a0) struct Hook *hook,REG(a2) struct PopupMenu *pm,REG(a1) APTR msg)
{
    return menuChoice(hook->h_Data,pm->ID);
}

/***********************************************************************/

static IPTR SAVEDS ASM
mContextMenuBuild(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_ContextMenuBuild *msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR        menu = data->menu;
    ULONG       flags = data->flags;

    if (!menu) return DoSuperMethodA(cl,obj,(Msg)msg);

    if (flags & FLG_PMenu)
    {
        struct Hook hook;
        struct Window *win;

        if ((flags & FLG_NoHideItem) && (flags & FLG_NoCloseItem))
        {
            pmset(PM_FindItem(menu,MCCMSG_HideWin),PM_Hidden,TRUE);
            pmset(PM_FindItem(menu,MCCMSG_CloseWin),PM_Hidden,TRUE);
            pmset(PM_FindItem(menu,MBAR1),PM_Hidden,TRUE);
        }
        else
        {
            pmset(PM_FindItem(menu,MCCMSG_HideWin),PM_Hidden,flags & FLG_NoHideItem);
            pmset(PM_FindItem(menu,MCCMSG_CloseWin),PM_Hidden,flags & FLG_NoCloseItem);
            pmset(PM_FindItem(menu,MBAR1),PM_Hidden,FALSE);
        }

        if (!((flags & FLG_NoDragBar) && (flags & FLG_NoSize)))
        {
            if (flags & FLG_AutoShow)
            {
                pmset(PM_FindItem(menu,MCCMSG_AutoShow),PM_Checked,TRUE);

                if (!(flags & FLG_NoDragBar))
                {
                    pmset(PM_FindItem(menu,MCCMSG_DragBarTop),PM_Hidden,FALSE);
                    pmset(PM_FindItem(menu,MCCMSG_DragBar),PM_Hidden,TRUE);
                }

                if (!(flags & FLG_NoSize)) pmset(PM_FindItem(menu,MCCMSG_Size),PM_Hidden,TRUE);
            }
            else
            {
                pmset(PM_FindItem(menu,MCCMSG_AutoShow),PM_Checked,FALSE);

                if (!(flags & FLG_NoDragBar))
                {
                    PM_SetItemAttrs(PM_FindItem(menu,MCCMSG_DragBarTop),PM_Hidden,!(flags & FLG_ShowDragBar),PM_Checked,flags & FLG_DragBarTop,TAG_DONE);
                    PM_SetItemAttrs(PM_FindItem(menu,MCCMSG_DragBar),PM_Hidden,FALSE,PM_Checked,flags & FLG_ShowDragBar,TAG_DONE);
                }

                if (!(flags & FLG_NoSize)) PM_SetItemAttrs(PM_FindItem(menu,MCCMSG_Size),PM_Hidden,FALSE,PM_Checked,flags & FLG_ShowSize,TAG_DONE);
            }
        }

        pmset(PM_FindItem(menu,MCCMSG_Snapshot),PM_Hidden,!data->ID);
        pmset(PM_FindItem(menu,MCCMSG_Unsnapshot),PM_Hidden,!data->ID);
        pmset(PM_FindItem(menu,MBAR4),PM_Hidden,!data->ID);

        get(_win(obj),MUIA_Window_Window,&win);

        hook.h_Entry = (HOOKFUNC)pmenuFunc;
        hook.h_Data  = data;
        PM_OpenPopupMenu(win,PM_Menu,        menu,
                             PM_MenuHandler, &hook,
                             TAG_DONE);
        return (IPTR)NULL;
    }

    if ((flags & FLG_NoHideItem) && (flags & FLG_NoCloseItem))
    {
        set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_HideWin),MUIA_Menuitem_Enabled,FALSE);
        set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_CloseWin),MUIA_Menuitem_Enabled,FALSE);
    }
    else
    {
        set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_HideWin),MUIA_Menuitem_Enabled,!(flags & FLG_NoHideItem));
        set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_CloseWin),MUIA_Menuitem_Enabled,!(flags & FLG_NoCloseItem));
    }

    if (!((flags & FLG_NoDragBar) && (flags & FLG_NoSize)))
    {
        if (flags & FLG_AutoShow)
        {
            set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_AutoShow),MUIA_Menuitem_Checked,TRUE);

            if (!(flags & FLG_NoDragBar))
            {
                set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBarTop),MUIA_Menuitem_Enabled,TRUE);
                set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBar),MUIA_Menuitem_Enabled,FALSE);
            }

            if (!(flags & FLG_NoSize)) set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_Size),MUIA_Menuitem_Enabled,FALSE);
        }
        else
        {
            set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_AutoShow),MUIA_Menuitem_Checked,FALSE);

            if (!(flags & FLG_NoDragBar))
            {
                SetAttrs((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBarTop),MUIA_Menuitem_Enabled,flags & FLG_ShowDragBar,MUIA_Menuitem_Checked,flags & FLG_DragBarTop,TAG_DONE);
                SetAttrs((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_DragBar),MUIA_Menuitem_Enabled,TRUE,MUIA_Menuitem_Checked,flags & FLG_ShowDragBar,TAG_DONE);
            }

            if (!(flags & FLG_NoSize)) SetAttrs((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_Size),MUIA_Menuitem_Enabled,TRUE,MUIA_Menuitem_Checked,flags & FLG_ShowSize,TAG_DONE);
        }
    }

    set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_Snapshot),MUIA_Menuitem_Enabled,data->ID);
    set((Object *)DoMethod(menu,MUIM_FindUData,MCCMSG_Unsnapshot),MUIA_Menuitem_Enabled,data->ID);

    return (IPTR)menu;
}

/***************************************************************************/

static IPTR SAVEDS ASM
mContextMenuChoice(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_ContextMenuChoice *msg)
{
    return menuChoice(INST_DATA(cl,obj),muiUserData(msg->item));
}

/***********************************************************************/

static IPTR ASM
mDraw(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Draw *msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoSuperMethodA(cl,obj,(APTR)msg);

    if (!(data->flags & FLG_Borders) && (msg->flags & MADF_DRAWOBJECT))
    {
        struct RastPort *rp = _rp(obj);
        UWORD           xofs, yofs;

        xofs = yofs = 0;
        if (!(data->flags & FLG_NoDragBar) && (data->flags & FLG_ShowDragBar) && (data->flags & FLG_Over)) {
            if (data->flags & FLG_DragBarTop) yofs = _height(data->dragBar);
            else xofs = _width(data->dragBar);
        }

        SetAPen(rp,MUIPEN(data->shine));
        if (xofs) Move(rp,_left(obj)+xofs,_top(obj));
        else
        {
            Move(rp,_left(obj),_bottom(obj));
            Draw(rp,_left(obj),_top(obj)+yofs);
        }

        if (yofs) Move(rp,_right(obj),_top(obj)+yofs);
        else Draw(rp,_right(obj),_top(obj));

        SetAPen(rp,MUIPEN(data->shadow));
        Draw(rp,_right(obj),_bottom(obj));
        Draw(rp,_left(obj)+(xofs ? xofs : 1),_bottom(obj));
    }

    return 0;
}

/***********************************************************************/

static IPTR ASM
mAddUserItem(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_BWin_AddUserItem *msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR        o;

    if (!data->menu) return 0;

    if (data->flags & FLG_PMenu)
    {
        if (!data->uitems)
        {
            if (!(data->ubar = PM_MakeItem(PM_TitleBar, TRUE, TAG_DONE)) ||
                !(data->umenu = PM_MakeItem(PM_Title, getString(MCCMSG_User),
                    PM_Sub, data->usub = PM_MakeMenu(PM_Dummy, 0,
                        PM_Item, PM_MakeItem(PM_Hidden, TRUE, TAG_DONE),
                    End,
                End))
            {
                if (data->ubar)
                {
                    PM_FreePopupMenu(data->ubar);
                    data->ubar= NULL;
                }

                return 0;
            }
        }

        if (!(o = (msg->title==MUIV_BWin_AddUserItem_Bar) ?
                    PM_MakeItem(PM_TitleBar, TRUE, PM_ID, data->subids, TAG_DONE) :
                    PM_MakeItem(PM_Title, msg->title, PM_ID, data->subids, TAG_DONE)))
        {
            if (!--data->uitems)
            {
                PM_FreePopupMenu(data->umenu);
                PM_FreePopupMenu(data->ubar);

                data->umenu = data->ubar = NULL;
            }

            return 0;
        }

        PM_InsertMenuItem(data->usub,
            PM_Insert_Last, TRUE,
            PM_Insert_Item, o,
            TAG_DONE);

        if (!data->uitems)
            PM_InsertMenuItem(data->menu,
                PM_Insert_Last, TRUE,
                PM_Insert_Item, data->ubar,
                PM_Insert_Item, data->umenu,
                TAG_DONE);
    }
    else
    {
        if (!data->uitems)
        {
            if (!(data->ubar = MenuitemObject, MUIA_Menuitem_Title, NM_BARLABEL, End) ||
                !(data->umenu = MenuitemObject, MUIA_Menuitem_Title, getString(MCCMSG_User), End))
            {
                if (data->ubar)
                {
                    MUI_DisposeObject(data->ubar);
                    data->ubar = NULL;
                }

                return 0;
            }
        }

        if (!(o = MenuitemObject, MUIA_Menuitem_Title, msg->title, MUIA_UserData, data->subids, End))
        {
            if (!data->uitems)
            {
                MUI_DisposeObject(data->umenu);
                MUI_DisposeObject(data->ubar);

                data->umenu = data->ubar = NULL;
            }

            return 0;
        }

        if (!data->uitems)
        {
            DoMethod(data->mparent,MUIM_Family_AddTail,data->ubar);
            DoMethod(data->mparent,MUIM_Family_AddTail,data->umenu);
        }

        DoMethod(data->umenu,MUIM_Family_AddTail,o);
    }

    if (o)
    {
        data->uitems++;

        return data->subids++;
    }

    return 0;
}

/***********************************************************************/

static IPTR ASM
mRemoveUserItem(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_BWin_RemoveUserItem *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       res = FALSE;

    if (!data->menu) return FALSE;

    if (data->flags & FLG_PMenu)
    {
        struct PopupMenu *item;

        if ((item = PM_FindItem(data->usub,msg->id)))
        {
            PM_RemoveMenuItem(data->usub,item);
            PM_FreePopupMenu(item);

            if (!--data->uitems)
            {
                PM_RemoveMenuItem(data->menu,data->umenu);
                PM_FreePopupMenu(data->umenu);

                PM_RemoveMenuItem(data->menu,data->ubar);
                PM_FreePopupMenu(data->ubar);
            }

            res = TRUE;
        }
    }
    else
    {
        Object *item;

        if ((item = (Object *)DoMethod(data->umenu,MUIM_FindUData,msg->id)))
        {
            DoMethod(data->umenu,MUIM_Family_Remove,item);
            MUI_DisposeObject(item);

            if (!--data->uitems)
            {
                DoMethod(data->mparent,MUIM_Family_Remove,data->umenu);
                MUI_DisposeObject(data->umenu);

                DoMethod(data->mparent,MUIM_Family_Remove,data->ubar);
                MUI_DisposeObject(data->ubar);
            }

            res = TRUE;
        }
    }

    return res;
}

/***********************************************************************/

#ifdef __AROS__
static BOOPSI_DISPATCHER(IPTR, dispatcher, cl, obj, msg)
#else
static IPTR ASM SAVEDS
dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
#endif
{
    switch(msg->MethodID)
    {
        case OM_NEW:                   return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:               return mDispose(cl,obj,(APTR)msg);
        case OM_SET:                   return mSets(cl,obj,(APTR)msg);

        case MUIM_Setup:               return mSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:             return mCleanup(cl,obj,(APTR)msg);
        case MUIM_Draw:                return mDraw(cl,obj,(APTR)msg);
        case MUIM_ContextMenuBuild:    return mContextMenuBuild(cl,obj,(APTR)msg);
        case MUIM_ContextMenuChoice:   return mContextMenuChoice(cl,obj,(APTR)msg);

        case MUIM_BWin_AddUserItem:    return mAddUserItem(cl,obj,(APTR)msg);
        case MUIM_BWin_RemoveUserItem: return mRemoveUserItem(cl,obj,(APTR)msg);

        default:                       return DoSuperMethodA(cl,obj,msg);
    }
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

/***************************************************************************/

BOOL ASM
initContents(void)
{
    if ((lib_contents = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct data),dispatcher)))
    {
        static ULONG done;

        if (MUIMasterBase->lib_Version>=20)
            lib_contents->mcc_Class->cl_ID = "BWinContentsGroup";

        if (!done)
        {
            localizeMenus(nmenu);

            done = TRUE;
        }

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

