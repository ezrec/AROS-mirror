
#include "class.h"
#include "alfie.h"

/***********************************************************************/

#include "bwin_private.h"

/***********************************************************************/

static Object * ASM
oclabel(REG(d0) ULONG text)
{
    return CLabel(getString(text));
}

/****************************************************************************/

static Object * ASM
oflabel(REG(d0) ULONG text)
{
    return FreeLabel(getString(text));
}

/****************************************************************************/

static Object * ASM
ocheckmark(REG(d0) ULONG key,REG(d1) ULONG help)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Checkmark,getString(key)))
        SetAttrs(obj,MUIA_CycleChain,TRUE,help ? MUIA_ShortHelp : TAG_IGNORE,help ? getString(help) : NULL,TAG_DONE);

    return obj;
}

/***********************************************************************/

static Object * ASM
ocycle(REG(a0) STRPTR *array,REG(a1) STRPTR key,REG(d0) ULONG help)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Cycle,key,array))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_ShortHelp,getString(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

static Object * ASM
oslider(REG(d0) ULONG key,REG(d1) ULONG help,REG(d2) LONG min,REG(d3) LONG max)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Slider,getString(key),min,max))
        SetAttrs(obj,MUIA_CycleChain,TRUE,help ? MUIA_ShortHelp : TAG_IGNORE,help ? getString(help) : NULL,TAG_DONE);

    return obj;
}

/***********************************************************************/

static Object * ASM
opopPen(REG(d0) ULONG key,REG(d1) ULONG title,REG(d2) ULONG help)
{
    return PoppenObject,
        MUIA_Window_Title, getString(title),
        MUIA_ControlChar,  key ? getKeyChar(getString(key)) : 0,
        MUIA_Draggable,    TRUE,
        MUIA_CycleChain,   TRUE,
        MUIA_ShortHelp,    getString(help),
    End;
}

/***********************************************************************/

static Object * ASM
opopImage(REG(d0) ULONG type,REG(d1) ULONG key,REG(d2) ULONG title,REG(d3) ULONG help)
{
    return MUI_NewObject(MUIC_Popimage,
        MUIA_Imageadjust_Type, type,
        MUIA_Window_Title,     getString(title),
        MUIA_ControlChar,      key ? getKeyChar(getString(key)) : 0,
        MUIA_Draggable,        TRUE,
        MUIA_CycleChain,       TRUE,
        MUIA_ShortHelp,        getString(help),
    End;
}

/***********************************************************************/

static ULONG ASM
mNew(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    if (obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg))
    {
        struct data *data = INST_DATA(cl,obj);
        char        buf[128], *t;
        Object      *prefs, *trans;

        snprintf(buf,sizeof(buf),getString(MCPMSG_Info_First),"\33bBWin " VRSTRING "\33n (" DATE ")\33n");

        if ((t = getString(MCPMSG_Info_Translator)) && *t)
            trans = VGroup,
                MUIA_InnerLeft,0,
                MUIA_InnerBottom,0,
                MUIA_InnerTop,0,
                MUIA_InnerRight,0,
                Child, TextObject,
                    MUIA_Text_PreParse, "\033c",
                    MUIA_Text_Contents, t,
                End,
                Child, RectangleObject,
                    MUIA_FixHeightTxt, "a",
                End,
            End;
        else trans = NULL;

        if (prefs = VGroup,

                Child, data->sg = ScrollgroupObject, // Scroller
                    MUIA_CycleChain, TRUE,
                    //MUIA_Scrollgroup_FreeHoriz, FALSE,

                    MUIA_Scrollgroup_Contents, VirtgroupObject, // Virt
                        VirtualFrame,

                        Child, ColGroup(2), // Cols2

                            Child, VGroup, // Window
                                GroupFrameT(getString(MCPMSG_Title_Window)),

                                Child, RectangleObject, MUIA_Weight, 20, End,

                                Child, HGroup,
                                    Child, RectangleObject, MUIA_Weight, 5, End,
                                    Child, oflabel(MCPMSG_WinShinePen),
                                    Child, data->winShinePen = opopPen(MCPMSG_WinShinePen,MCPMSG_WinShinePen_Title,MCPMSG_WinShinePen_Help),
                                    Child, RectangleObject, MUIA_Weight, 5, End,
                                    Child, oflabel(MCPMSG_WinShadowPen),
                                    Child, data->winShadowPen = opopPen(MCPMSG_WinShadowPen,MCPMSG_WinShadowPen_Title,MCPMSG_WinShadowPen_Help),
                                    Child, RectangleObject, MUIA_Weight, 5, End,
                                End,

                                Child, RectangleObject, MUIA_Weight, 20, End,

                                Child, HGroup,
                                    Child, HSpace(0),
                                    Child, oflabel(MCPMSG_DragBarOver),
                                    Child, data->over = ocheckmark(MCPMSG_DragBarOver,MCPMSG_DragBarOver_Help),
                                    Child, HSpace(0),
                                End,

                                Child, RectangleObject, MUIA_Weight, 20, End,
                            End, // Window

                            Child, ColGroup(3), // dragbar
                                GroupFrameT(getString(MCPMSG_Title_DragBar)),

                                Child, RectangleObject, MUIA_Weight, 1, End,
                                Child, oclabel(MCPMSG_DragBarActive),
                                Child, oclabel(MCPMSG_DragBarNonActive),

                                Child, oflabel(MCPMSG_DragBarShinePen),
                                Child, data->dragBarShinePen = opopPen(MCPMSG_DragBarShinePen,MCPMSG_DragBarShinePen_Title,MCPMSG_DragBarShinePen_Help),
                                Child, data->nadragBarShinePen = opopPen(0,MCPMSG_DragBarShinePen_Title,MCPMSG_DragBarShinePen_Help),

                                Child, oflabel(MCPMSG_DragBarShadowPen),
                                Child, data->dragBarShadowPen = opopPen(MCPMSG_DragBarShadowPen,MCPMSG_DragBarShadowPen_Title,MCPMSG_DragBarShadowPen_Help),
                                Child, data->nadragBarShadowPen = opopPen(0,MCPMSG_DragBarShadowPen_Title,MCPMSG_DragBarShadowPen_Help),

                                Child, oflabel(MCPMSG_DragBarBack),
                                Child, HGroup,
                                    Child, data->dragBarBack = opopImage(MUIV_Imageadjust_Type_Background,MCPMSG_DragBarBack,MCPMSG_DragBarBack_Title,MCPMSG_DragBarBack_Help),
                                    Child, oflabel(MCPMSG_UseDragBarBack),
                                    Child, VGroup,
                                        Child, VSpace(0),
                                        Child, data->useDragBarBack = ocheckmark(MCPMSG_UseDragBarBack,MCPMSG_UseDragBarBack_Help),
                                        Child, VSpace(0),
                                    End,
                                End,
                                Child, HGroup,
                                    Child, data->nadragBarBack = opopImage(MUIV_Imageadjust_Type_Background,0,MCPMSG_DragBarBack_Title,MCPMSG_DragBarBack_Help),
                                    Child, oflabel(MCPMSG_UseDragBarBack),
                                    Child, VGroup,
                                        Child, VSpace(0),
                                        Child, data->nauseDragBarBack = ocheckmark(0,MCPMSG_UseDragBarBack_Help),
                                        Child, VSpace(0),
                                    End,
                                End,

                                Child, FreeLabel(t = getString(MCPMSG_DragBarType)),
                                Child, data->dragBarType = ocycle(shapeStrings,t,MCPMSG_DragBarType_Help),
                                Child, data->nadragBarType = ocycle(shapeStrings,NULL,MCPMSG_DragBarType_Help),
                            End,  // dragbar

                            Child, ColGroup(3), // size
                                GroupFrameT(getString(MCPMSG_Title_Size)),

                                Child, RectangleObject, MUIA_Weight, 1, End,
                                Child, oclabel(MCPMSG_DragBarActive),
                                Child, oclabel(MCPMSG_DragBarNonActive),

                                Child, oflabel(MCPMSG_SizeShinePen),
                                Child, data->sizeShinePen = opopPen(MCPMSG_SizeShinePen,MCPMSG_SizeShinePen_Title,MCPMSG_SizeShinePen_Help),
                                Child, data->nasizeShinePen = opopPen(0,MCPMSG_SizeShinePen_Title,MCPMSG_SizeShinePen_Help),

                                Child, oflabel(MCPMSG_SizeShadowPen),
                                Child, data->sizeShadowPen = opopPen(MCPMSG_SizeShadowPen,MCPMSG_SizeShadowPen_Title,MCPMSG_SizeShadowPen_Help),
                                Child, data->nasizeShadowPen = opopPen(0,MCPMSG_SizeShadowPen_Title,MCPMSG_SizeShadowPen_Help),

                                Child, oflabel(MCPMSG_SizeForegroundPen),
                                Child, HGroup,
                                    Child, data->sizeForegroundPen = opopPen(MCPMSG_SizeForegroundPen,MCPMSG_SizeForegroundPen_Title,MCPMSG_SizeForegroundPen_Help),
                                    Child, oflabel(MCPMSG_UseSizeForegroundPen),
                                    Child, VGroup,
                                        Child, VSpace(0),
                                        Child, data->useSizeForeground = ocheckmark(MCPMSG_UseSizeForegroundPen,MCPMSG_UseSizeForegroundPen_Help),
                                        Child, VSpace(0),
                                    End,
                                End,
                                Child, HGroup,
                                    Child, data->nasizeForegroundPen = opopPen(0,MCPMSG_SizeForegroundPen_Title,MCPMSG_SizeForegroundPen_Help),
                                    Child, oflabel(MCPMSG_UseSizeForegroundPen),
                                    Child, VGroup,
                                        Child, VSpace(0),
                                        Child, data->nauseSizeForeground = ocheckmark(MCPMSG_UseSizeForegroundPen,MCPMSG_UseSizeForegroundPen_Help),
                                        Child, VSpace(0),
                                    End,
                                End,

                                Child, FreeLabel(t = getString(MCPMSG_SizeType)),
                                Child, data->sizeType = ocycle(shapeStrings,t,MCPMSG_SizeType_Help),
                                Child, data->nasizeType = ocycle(shapeStrings,NULL,MCPMSG_SizeType_Help),

                            End, // size

                            Child, VGroup, // close
                                GroupFrameT(getString(MCPMSG_Title_Close)),

                                Child, ColGroup(3), // close
                                    MUIA_Disabled, TRUE,

                                    Child, RectangleObject, MUIA_Weight, 1, End,
                                    Child, oclabel(MCPMSG_DragBarActive),
                                    Child, oclabel(MCPMSG_DragBarNonActive),

                                    Child, oflabel(MCPMSG_CloseShinePen),
                                    Child, opopPen(MCPMSG_CloseShinePen,MCPMSG_CloseShinePen_Title,MCPMSG_CloseShinePen_Help),
                                    Child, opopPen(0,MCPMSG_CloseShinePen_Title,MCPMSG_CloseShinePen_Help),

                                    Child, oflabel(MCPMSG_CloseShadowPen),
                                    Child, opopPen(MCPMSG_CloseShadowPen,MCPMSG_CloseShadowPen_Title,MCPMSG_CloseShadowPen_Help),
                                    Child, opopPen(0,MCPMSG_CloseShadowPen_Title,MCPMSG_CloseShadowPen_Help),

                                    Child, oflabel(MCPMSG_CloseBack),
                                    Child, HGroup,
                                        Child, opopImage(MUIV_Imageadjust_Type_Background,MCPMSG_CloseBack,MCPMSG_CloseBack_Title,MCPMSG_CloseBack_Help),
                                        Child, oflabel(MCPMSG_UseCloseBack),
                                        Child, VGroup,
                                            Child, VSpace(0),
                                            Child, ocheckmark(MCPMSG_UseCloseBack,MCPMSG_UseCloseBack_Help),
                                            Child, VSpace(0),
                                        End,
                                    End,
                                    Child, HGroup,
                                        Child, opopImage(MUIV_Imageadjust_Type_Background,MCPMSG_CloseBack,MCPMSG_CloseBack_Title,MCPMSG_CloseBack_Help),
                                        Child, oflabel(MCPMSG_UseCloseBack),
                                        Child, VGroup,
                                            Child, VSpace(0),
                                            Child, ocheckmark(0,MCPMSG_UseCloseBack_Help),
                                            Child, VSpace(0),
                                        End,
                                    End,

                                    Child, FreeLabel(t = getString(MCPMSG_CloseType)),
                                    Child, ocycle(shapeStrings,t,MCPMSG_CloseType_Help),
                                    Child, ocycle(shapeStrings,NULL,MCPMSG_CloseType_Help),
                                End,
                            End, // close

                        End, // Cols2

                        Child, ColGroup(5), // Spacing
                            GroupFrameT(getString(MCPMSG_Title_Spacing)),
                            Child, oflabel(MCPMSG_LeftSpacing),
                            Child, data->leftSp = oslider(MCPMSG_LeftSpacing,MCPMSG_LeftSpacing_Help,0,15),
                            Child, RectangleObject, MUIA_Weight, 10, End,
                            Child, oflabel(MCPMSG_TopSpacing),
                            Child, data->topSp = oslider(MCPMSG_TopSpacing,MCPMSG_TopSpacing_Help,0,15),
                            Child, oflabel(MCPMSG_RightSpacing),
                            Child, data->rightSp = oslider(MCPMSG_RightSpacing,MCPMSG_RightSpacing_Help,0,15),
                            Child, RectangleObject, MUIA_Weight, 10, End,
                            Child, oflabel(MCPMSG_BottomSpacing),
                            Child, data->bottomSp = oslider(MCPMSG_BottomSpacing,MCPMSG_BottomSpacing_Help,0,15),
                        End, // Spacing

                    End, // Virt
                End, // Scroller

                Child, CrawlingObject, // Crawling

                    MUIA_FixHeightTxt, "\n",
                    TextFrame,
                    MUIA_Background, MUII_TextBack,

                    Child, TextObject,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_Contents, buf,
                    End,

                    Child, TextObject,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_Contents, getString(MCPMSG_Info_Reserved),
                    End,

                    Child, RectangleObject,
                        MUIA_FixHeightTxt, "a",
                    End,

                    Child, TextObject,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_Contents, getString(MCPMSG_Info_Rest),
                    End,

                    Child, RectangleObject,
                        MUIA_FixHeightTxt, "a",
                    End,

                    trans ? Child : TAG_IGNORE, trans,

                    Child, TextObject,
                        MUIA_Text_PreParse, "\033c",
                        MUIA_Text_Contents, buf,
                    End,

                End,  // Crawling

        End) // Main group
        {
            DoSuperMethod(cl,obj,OM_ADDMEMBER,prefs);

            if (data->menu = MenustripObject,
                MUIA_Family_Child, MenuObjectT(getString(MCPMSG_MenuTitle_BWin)),
                    MUIA_Family_Child, MenuitemObject,
                        MUIA_Menuitem_Title, getString(MCPMSG_Item_Defaults),
                        MUIA_UserData, MCPMSG_Item_Defaults,
                    End,
                    MUIA_Family_Child, MenuitemObject,
                        MUIA_Menuitem_Title, getString(MCPMSG_Item_Alfie_Defaults),
                        MUIA_UserData, MCPMSG_Item_Alfie_Defaults,
                    End,
                End,
            End) SetSuperAttrs(cl,obj,MUIA_ContextMenu,data->menu,TAG_DONE);
        }
        else
        {
            CoerceMethod(cl,obj,OM_DISPOSE);
            obj = 0;
        }
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG ASM
mGet(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opGet *msg)
{
    switch (msg->opg_AttrID)
    {
        case MUIA_Version:  *msg->opg_Storage = VERSION; return TRUE;
        case MUIA_Revision: *msg->opg_Storage = REVISION; return TRUE;
        default: return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

static ULONG ASM
mDispose(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->menu) MUI_DisposeObject(data->menu);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG ASM
mConfigToGadgets(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Settingsgroup_ConfigToGadgets *msg)
{
    struct data *data = INST_DATA(cl, obj );
    Object      *cfg = msg->configdata;
    APTR                 ptr;
    ULONG       *val, v;

    /*if (lib_flags & BASEFLG_Avoid)
    {
        return 0;
    }*/

    /* winShinePen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_WinShinePen)))
        ptr = MUIDEF_BWin_WinShinePen;
    set(data->winShinePen,MUIA_Pendisplay_Spec,ptr);

    /* winShadowPen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_WinShadowPen)))
        ptr = MUIDEF_BWin_WinShadowPen;
    set(data->winShadowPen,MUIA_Pendisplay_Spec,ptr);

    /* dragBarShinePen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_DragBarShinePen)))
        ptr = MUIDEF_BWin_DragBarShinePen;
    set(data->dragBarShinePen,MUIA_Pendisplay_Spec,ptr);

    /* dragBarShadowPen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_DragBarShadowPen)))
        ptr = MUIDEF_BWin_DragBarShadowPen;
    set(data->dragBarShadowPen,MUIA_Pendisplay_Spec,ptr);

    /* dragBarBack */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_DragBarBack)))
        ptr = MUIDEF_BWin_DragBarBack;
    set(data->dragBarBack,MUIA_Imagedisplay_Spec,ptr);

    /* nadragBarShinePen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_NADragBarShinePen)))
        ptr = MUIDEF_BWin_NADragBarShinePen;
    set(data->nadragBarShinePen,MUIA_Pendisplay_Spec,ptr);

    /* nadragBarShadowPen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_NADragBarShadowPen)))
        ptr = MUIDEF_BWin_NADragBarShadowPen;
    set(data->nadragBarShadowPen,MUIA_Pendisplay_Spec,ptr);

    /* nadragBarBack */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_NADragBarBack)))
        ptr = MUIDEF_BWin_NADragBarBack;
    set(data->nadragBarBack,MUIA_Imagedisplay_Spec,ptr);

    /* sizeShinePen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_SizeShinePen)))
        ptr = MUIDEF_BWin_SizeShinePen;
    set(data->sizeShinePen,MUIA_Pendisplay_Spec,ptr);

    /* sizeShadowPen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_SizeShadowPen)))
        ptr = MUIDEF_BWin_SizeShadowPen;
    set(data->sizeShadowPen,MUIA_Pendisplay_Spec,ptr);

    /* sizeForegroundPen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_SizeForegroundPen)))
        ptr = MUIDEF_BWin_SizeForegroundPen;
    set(data->sizeForegroundPen,MUIA_Pendisplay_Spec,ptr);

    /* nasizeShinePen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_NASizeShinePen)))
        ptr = MUIDEF_BWin_NASizeShinePen;
    set(data->nasizeShinePen,MUIA_Pendisplay_Spec,ptr);

    /* nasizeShadowPen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_NASizeShadowPen)))
        ptr = MUIDEF_BWin_NASizeShadowPen;
    set(data->nasizeShadowPen,MUIA_Pendisplay_Spec,ptr);

    /* nasizeForegroundPen */
    if (!(ptr = (APTR)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_NASizeForegroundPen)))
        ptr = MUIDEF_BWin_NASizeForegroundPen;
    set(data->nasizeForegroundPen,MUIA_Pendisplay_Spec,ptr);

    /* types */
    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_Types)) ? *val : MUIDEF_BWin_Types;
    set(data->over,MUIA_Selected,GETUSEOVER(v));
    set(data->useDragBarBack,MUIA_Selected,GETUSEDRAGBARBACK(v));
    set(data->nauseDragBarBack,MUIA_Selected,GETUSENADRAGBARBACK(v));
    set(data->dragBarType,MUIA_Cycle_Active,GETDRAGBARTYPE(v));
    set(data->nadragBarType,MUIA_Cycle_Active,GETNADRAGBARTYPE(v));
    set(data->useSizeForeground,MUIA_Selected,GETUSESIZEFOREGROUND(v));
    set(data->nauseSizeForeground,MUIA_Selected,GETUSENASIZEFOREGROUND(v));
    set(data->sizeType,MUIA_Cycle_Active,GETSIZETYPE(v));
    set(data->nasizeType,MUIA_Cycle_Active,GETNASIZETYPE(v));

    /* spacing */
    v = (val = (ULONG *)DoMethod(cfg,MUIM_Dataspace_Find,MUICFG_BWin_Spacing)) ? *val : MUIDEF_BWin_Spacing;
    set(data->leftSp,MUIA_Numeric_Value,GETLEFTSP(v));
    set(data->topSp,MUIA_Numeric_Value,GETTOPSP(v));
    set(data->rightSp,MUIA_Numeric_Value,GETRIGHTSP(v));
    set(data->bottomSp,MUIA_Numeric_Value,GETBOTTOMSP(v));

    return 0;
}

/***********************************************************************/

static ULONG ASM
mGadgetsToConfig(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Settingsgroup_GadgetsToConfig *msg)
{
    struct data *data = INST_DATA(cl,obj);
    Object      *cfg = msg->configdata;
    APTR                 ptr;
    ULONG                val, v;

    /*if (lib_flags & BASEFLG_Avoid)
    {
        register LONG i;

        for (i = 0; i<256; i++)
            DoMethod(cfg,MUIM_Dataspace_Remove,MUIA_BWin_Base+i);

        return 0;
    }*/

    /* winShinePen */
    get(data->winShinePen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_WinShinePen);

    /* winShadowPen */
    get(data->winShadowPen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_WinShadowPen);

    /* dragBarShinePen */
    get(data->dragBarShinePen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_DragBarShinePen);

    /* dragBarShadowPen */
    get(data->dragBarShadowPen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_DragBarShadowPen);

#ifndef __AROS__
    // FIXME: AROS?
    /* dragBarBack */
    get(data->dragBarBack,MUIA_Imagedisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_ImageSpec),MUICFG_BWin_DragBarBack);
#endif

    /* nadragBarShinePen */
    get(data->nadragBarShinePen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_NADragBarShinePen);

    /* nadragBarShadowPen */
    get(data->nadragBarShadowPen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_NADragBarShadowPen);

#ifndef __AROS__
    // FIXME: AROS?
    /* nadragBarBack */
    get(data->nadragBarBack,MUIA_Imagedisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_ImageSpec),MUICFG_BWin_NADragBarBack);
#endif

    /* sizeShinePen */
    get(data->sizeShinePen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_SizeShinePen);

    /* sizeShadowPen */
    get(data->sizeShadowPen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_SizeShadowPen);

    /* sizeForegroundPen */
    get(data->sizeForegroundPen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_SizeForegroundPen);

    /* nasizeShinePen */
    get(data->nasizeShinePen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_NASizeShinePen);

    /* nasizeShadowPen */
    get(data->nasizeShadowPen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_NASizeShadowPen);

    /* nasizeForegroundPen */
    get(data->nasizeForegroundPen,MUIA_Pendisplay_Spec,&ptr);
    if (ptr) DoMethod(cfg,MUIM_Dataspace_Add,ptr,sizeof(struct MUI_PenSpec),MUICFG_BWin_NASizeForegroundPen);

    /* types */
    get(data->useDragBarBack,MUIA_Selected,&val);
    if (val) v = MUIV_BWin_UseDragBarBack;
    else v = 0;
    get(data->nauseDragBarBack,MUIA_Selected,&val);
    if (val) v |= MUIV_BWin_UseNADragBarBack;
    get(data->useSizeForeground,MUIA_Selected,&val);
    if (val) v |= MUIV_BWin_UseSizeForeground;
    get(data->nauseSizeForeground,MUIA_Selected,&val);
    if (val) v |= MUIV_BWin_UseNASizeForeground;
    get(data->over,MUIA_Selected,&val);
    if (val) v |= MUIV_BWin_UseOver;
    v = v<<24;
    get(data->dragBarType,MUIA_Cycle_Active,&val);
    v |= val;
    get(data->nadragBarType,MUIA_Cycle_Active,&val);
    v |= val<<4;
    get(data->sizeType,MUIA_Cycle_Active,&val);
    v |= val<<8;
    get(data->nasizeType,MUIA_Cycle_Active,&val);
    v |= val<<12;
    DoMethod(cfg,MUIM_Dataspace_Add,&v,sizeof(v),MUICFG_BWin_Types);

    /* spacing */
    get(data->leftSp,MUIA_Numeric_Value,&val);
    v = val<<24;
    get(data->topSp,MUIA_Numeric_Value,&val);
    v |= val<<16;
    get(data->rightSp,MUIA_Numeric_Value,&val);
    v |= val<<8;
    get(data->bottomSp,MUIA_Numeric_Value,&val);
    v |= val;
    DoMethod(cfg,MUIM_Dataspace_Add,&v,sizeof(v),MUICFG_BWin_Spacing);

    return 0;
}

/***********************************************************************/

static ULONG ASM
mContextMenuChoice(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_ContextMenuChoice *msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch (muiUserData(msg->item))
    {
        case MCPMSG_Item_Defaults:
            set(data->winShinePen,MUIA_Pendisplay_Spec,MUIDEF_BWin_WinShinePen);
            set(data->winShadowPen,MUIA_Pendisplay_Spec,MUIDEF_BWin_WinShadowPen);
            set(data->dragBarShinePen,MUIA_Pendisplay_Spec,MUIDEF_BWin_DragBarShinePen);
            set(data->dragBarShadowPen,MUIA_Pendisplay_Spec,MUIDEF_BWin_DragBarShadowPen);
            set(data->dragBarBack,MUIA_Imagedisplay_Spec,MUIDEF_BWin_DragBarBack);
            set(data->useDragBarBack,MUIA_Selected,MUIDEF_BWin_UseDragBarBack);
            set(data->dragBarType,MUIA_Cycle_Active,MUIDEF_BWin_DragBarType);
            set(data->nadragBarShinePen,MUIA_Pendisplay_Spec,MUIDEF_BWin_NADragBarShinePen);
            set(data->nadragBarShadowPen,MUIA_Pendisplay_Spec,MUIDEF_BWin_NADragBarShadowPen);
            set(data->nadragBarBack,MUIA_Imagedisplay_Spec,MUIDEF_BWin_NADragBarBack);
            set(data->nauseDragBarBack,MUIA_Selected,MUIDEF_BWin_UseNADragBarBack);
            set(data->nadragBarType,MUIA_Cycle_Active,MUIDEF_BWin_NADragBarType);
            set(data->sizeShinePen,MUIA_Pendisplay_Spec,MUIDEF_BWin_SizeShinePen);
            set(data->sizeShadowPen,MUIA_Pendisplay_Spec,MUIDEF_BWin_SizeShadowPen);
            set(data->sizeForegroundPen,MUIA_Pendisplay_Spec,MUIDEF_BWin_SizeForegroundPen);
            set(data->useSizeForeground,MUIA_Selected,MUIDEF_BWin_UseSizeForeground);
            set(data->sizeType,MUIA_Cycle_Active,MUIDEF_BWin_SizeType);
            set(data->nasizeShinePen,MUIA_Pendisplay_Spec,MUIDEF_BWin_NASizeShinePen);
            set(data->nasizeShadowPen,MUIA_Pendisplay_Spec,MUIDEF_BWin_NASizeShadowPen);
            set(data->nasizeForegroundPen,MUIA_Pendisplay_Spec,MUIDEF_BWin_NASizeForegroundPen);
            set(data->nauseSizeForeground,MUIA_Selected,MUIDEF_BWin_UseNASizeForeground);
            set(data->nasizeType,MUIA_Cycle_Active,MUIDEF_BWin_NASizeType);
            set(data->leftSp,MUIA_Numeric_Value,MUIDEF_BWin_LeftSpacing);
            set(data->topSp,MUIA_Numeric_Value,MUIDEF_BWin_TopSpacing);
            set(data->rightSp,MUIA_Numeric_Value,MUIDEF_BWin_RightSpacing);
            set(data->bottomSp,MUIA_Numeric_Value,MUIDEF_BWin_BottomSpacing);
            break;

        case MCPMSG_Item_Alfie_Defaults:
            set(data->winShinePen,MUIA_Pendisplay_Spec,ALFIE_BWin_WinShinePen);
            set(data->winShadowPen,MUIA_Pendisplay_Spec,ALFIE_BWin_WinShadowPen);
            set(data->dragBarShinePen,MUIA_Pendisplay_Spec,ALFIE_BWin_DragBarShinePen);
            set(data->dragBarShadowPen,MUIA_Pendisplay_Spec,ALFIE_BWin_DragBarShadowPen);
            set(data->dragBarBack,MUIA_Imagedisplay_Spec,ALFIE_BWin_DragBarBack);
            set(data->useDragBarBack,MUIA_Selected,ALFIE_BWin_UseDragBarBack);
            set(data->dragBarType,MUIA_Cycle_Active,ALFIE_BWin_DragBarType);
            set(data->nadragBarShinePen,MUIA_Pendisplay_Spec,ALFIE_BWin_NADragBarShinePen);
            set(data->nadragBarShadowPen,MUIA_Pendisplay_Spec,ALFIE_BWin_NADragBarShadowPen);
            set(data->nadragBarBack,MUIA_Imagedisplay_Spec,ALFIE_BWin_NADragBarBack);
            set(data->nauseDragBarBack,MUIA_Selected,ALFIE_BWin_UseNADragBarBack);
            set(data->nadragBarType,MUIA_Cycle_Active,ALFIE_BWin_NADragBarType);
            set(data->sizeShinePen,MUIA_Pendisplay_Spec,ALFIE_BWin_SizeShinePen);
            set(data->sizeShadowPen,MUIA_Pendisplay_Spec,ALFIE_BWin_SizeShadowPen);
            set(data->sizeForegroundPen,MUIA_Pendisplay_Spec,ALFIE_BWin_SizeForegroundPen);
            set(data->sizeType,MUIA_Cycle_Active,ALFIE_BWin_SizeType);
            set(data->useSizeForeground,MUIA_Selected,ALFIE_BWin_UseSizeForeground);
            set(data->nasizeShinePen,MUIA_Pendisplay_Spec,ALFIE_BWin_NASizeShinePen);
            set(data->nasizeShadowPen,MUIA_Pendisplay_Spec,ALFIE_BWin_NASizeShadowPen);
            set(data->nasizeForegroundPen,MUIA_Pendisplay_Spec,ALFIE_BWin_NASizeForegroundPen);
            set(data->nauseSizeForeground,MUIA_Selected,ALFIE_BWin_UseNASizeForeground);
            set(data->nasizeType,MUIA_Cycle_Active,ALFIE_BWin_NASizeType);
            set(data->leftSp,MUIA_Numeric_Value,ALFIE_BWin_LeftSpacing);
            set(data->topSp,MUIA_Numeric_Value,ALFIE_BWin_TopSpacing);
            set(data->rightSp,MUIA_Numeric_Value,ALFIE_BWin_RightSpacing);
            set(data->bottomSp,MUIA_Numeric_Value,ALFIE_BWin_BottomSpacing);
            break;
    }

    return 0;
}

/***********************************************************************/

static ULONG ASM
mShow(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    set(_win(obj),MUIA_Window_DefaultObject,data->sg);

    return TRUE;
}

/***********************************************************************/

static ULONG ASM
mHide(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    set(_win(obj),MUIA_Window_DefaultObject,NULL);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR,dispatcher,cl,obj,msg)
#else
static ULONG ASM SAVEDS
dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
#endif
{
    switch (msg->MethodID)
    {
        case OM_NEW:                             return mNew(cl,obj,(APTR)msg);
        case OM_GET:                             return mGet(cl,obj,(APTR)msg);
        case MUIM_Show:                          return mShow(cl,obj,(APTR)msg);
        case MUIM_Hide:                          return mHide(cl,obj,(APTR)msg);
        case MUIM_Settingsgroup_ConfigToGadgets: return mConfigToGadgets(cl,obj,(APTR)msg);
        case MUIM_Settingsgroup_GadgetsToConfig: return mGadgetsToConfig(cl,obj,(APTR)msg);
        case MUIM_ContextMenuChoice:             return mContextMenuChoice(cl,obj,(APTR)msg);
        case OM_DISPOSE:                         return mDispose(cl,obj,(APTR)msg);
        default:                                 return DoSuperMethodA(cl,obj,msg);
    }
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

/***********************************************************************/

BOOL ASM
initMCP(void)
{
    if (lib_class = MUI_CreateCustomClass(lib_base,MUIC_Mccprefs,NULL,sizeof(struct data),dispatcher))
    {
        if (MUIMasterBase->lib_Version>=20)
            lib_class->mcc_Class->cl_ID = PRG;

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/
