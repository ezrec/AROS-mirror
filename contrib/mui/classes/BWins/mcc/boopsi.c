
#include "class.h"
#include <graphics/gfxmacros.h>

/***********************************************************************/

struct data
{
    LONG  shine;
    LONG  shadow;
    LONG  foreground;

    LONG  nashine;
    LONG  nashadow;
    LONG  naforeground;

    APTR  back;
    APTR  naback;

    ULONG type;
    ULONG flags;

    UBYTE shape;
    UBYTE nashape;

    BYTE  bheight;
    BYTE  bright;
};

enum
{
    BFLG_Setup           = 1<<0,
    BFLG_Horiz           = 1<<1,
    BFLG_UseShine        = 1<<2,
    BFLG_UseShadow       = 1<<3,
    BFLG_UseNAShine      = 1<<4,
    BFLG_UseNAShadow     = 1<<5,
    BFLG_UseForeground   = 1<<6,
    BFLG_UseNAForeground = 1<<7,
    BFLG_UseBack         = 1<<8,
    BFLG_UseNABack       = 1<<9,
    BFLG_Activate        = 1<<10,
    BFLG_UseNA           = 1<<11,
    BFLG_NTRedraw        = 1<<12,
};

/***********************************************************************/

static ULONG ASM
mNew(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    if ((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Boopsi_Class, lib_sgad,
            MUIA_Boopsi_Smart, TRUE,
            GA_Left,           0,
            GA_Top,            0,
            GA_Width,          0,
            GA_Height,         0,
            MUIA_FillArea,     TRUE,
            TAG_MORE,          msg->ops_AttrList)))
    {
        struct data *data = INST_DATA(cl,obj);

        data->type = GetTagData(MUIA_BWin_Type,MUIDEF_BWin_Type,msg->ops_AttrList);

        if (GetTagData(MUIA_BWin_Horiz,FALSE,msg->ops_AttrList))
            data->flags = BFLG_Horiz;

        if (GetTagData(MUIA_BWin_Activate,TRUE,msg->ops_AttrList))
            data->flags |= BFLG_Activate;
    }

    return (ULONG)obj;
}

/***************************************************************************/

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
            case MUIA_BWin_Horiz:
                if (BOOLSAME(data->flags & BFLG_Horiz,tidata)) tag->ti_Tag = TAG_IGNORE;
                else
                    if (tidata) data->flags |= BFLG_Horiz;
                    else data->flags &= ~BFLG_Horiz;
                break;

            case MUIA_BWin_Activate:
                if ((data->flags & BFLG_UseNA) && !BOOLSAME(data->flags & BFLG_Activate,tidata))
                {
                    APTR back = NULL;

                    if (tidata)
                    {
                        data->flags |= BFLG_Activate;
                        if (data->flags & BFLG_UseBack) back = data->back;
                    }
                    else
                    {
                        data->flags &= ~BFLG_Activate;
                        if (data->flags & BFLG_UseNABack) back = data->naback;
                    }

                    if (data->flags & (BFLG_UseBack|BFLG_UseNABack))
                    {
                        if (!back)
                        {
                            Object *parent;

                            get(obj,MUIA_Parent,&parent);
                            back = parent ? _backspec(parent) : (APTR)MUII_WindowBack;
                        }

                        data->flags |= BFLG_NTRedraw;
                        SetSuperAttrs(cl,obj,MUIA_Background,back,TAG_DONE);
                        data->flags &= ~BFLG_NTRedraw;
                    }

                    MUI_Redraw(obj,MADF_DRAWOBJECT);
                }
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(APTR)msg);
}

/***********************************************************************/

static ULONG ASM
mSetup(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR                 ptr;
    ULONG       types;
    ULONG                *v;

    types = DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_Types,&v) ? *v : MUIDEF_BWin_Types;

    switch (data->type)
    {
        case GTYP_WDRAGGING:
            if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_DragBarBack,&ptr)) ptr = MUIDEF_BWin_DragBarBack;
            data->back = ptr;

            if (GETUSEDRAGBARBACK(types)) data->flags |= BFLG_UseBack;
            else data->flags &= ~BFLG_UseBack;

            if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_NADragBarBack,&ptr)) ptr = MUIDEF_BWin_NADragBarBack;
            data->naback = ptr;

            if (GETUSENADRAGBARBACK(types)) data->flags |= BFLG_UseNABack;
            else data->flags &= ~BFLG_UseNABack;

            if (data->flags & ((data->flags & BFLG_Activate) ? BFLG_UseBack : BFLG_UseNABack))
                ptr = (data->flags & BFLG_Activate) ? data->back : data->naback;
            else
            {
                Object *parent;

                get(obj,MUIA_Parent,&parent);
                ptr = parent ? _backspec(parent) : (APTR)MUII_WindowBack;
            }

            SetSuperAttrs(cl,obj,MUIA_Background,ptr,TAG_DONE);

            break;

        default:
            break;
    }

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    data->shine = data->nashine = data->shadow = data->nashadow = 0;

    switch (data->type)
    {
        case GTYP_WDRAGGING:
            data->shape   = GETDRAGBARTYPE(types);
            data->nashape = GETNADRAGBARTYPE(types);

            if (data->shape!=MUIV_BWin_Shape_None)
            {
                if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_DragBarShinePen,&ptr)) ptr = MUIDEF_BWin_DragBarShinePen;
                data->shine = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                data->flags |= BFLG_UseShine;

                if (data->shape!=MUIV_BWin_Shape_Solid)
                {
                    if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_DragBarShadowPen,&ptr)) ptr = MUIDEF_BWin_DragBarShadowPen;
                    data->shadow = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                    data->flags |= BFLG_UseShadow;
                }
            }

            if (data->nashape!=MUIV_BWin_Shape_None)
            {
                if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_NADragBarShinePen,&ptr)) ptr = MUIDEF_BWin_NADragBarShinePen;
                data->nashine = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                data->flags |= BFLG_UseNAShine;

                if (data->nashape!=MUIV_BWin_Shape_Solid)
                {
                    if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_NADragBarShadowPen,&ptr)) ptr = MUIDEF_BWin_NADragBarShadowPen;
                    data->nashadow = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                    data->flags |= BFLG_UseNAShadow;
                }
            }

            if ((data->shape!=data->nashape) ||
                (data->shine!=data->nashine) ||
                (data->shadow!=data->nashadow) ||
                !((!(data->flags & BFLG_UseBack) && !(data->flags & BFLG_UseNABack)) || ((data->flags & BFLG_UseBack) && (data->flags & BFLG_UseNABack) && !strcmp(data->back,data->naback))))
                    data->flags |= BFLG_UseNA;
            else data->flags &= ~BFLG_UseNA;

            break;

        case GTYP_SIZING:
            data->shape   = GETSIZETYPE(types);
            data->nashape = GETNASIZETYPE(types);

            data->foreground = data->naforeground = 0;

            if (data->shape!=MUIV_BWin_Shape_None)
            {
                if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_SizeShinePen,&ptr)) ptr = MUIDEF_BWin_SizeShinePen;
                data->shine = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);

                data->flags |= BFLG_UseShine;

                if (data->shape!=MUIV_BWin_Shape_Solid)
                {
                    if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_SizeShadowPen,&ptr)) ptr = MUIDEF_BWin_SizeShadowPen;
                    data->shadow = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                    data->flags |= BFLG_UseShadow;

                    if (GETUSESIZEFOREGROUND(types))
                    {
                        if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_SizeForegroundPen,&ptr)) ptr = MUIDEF_BWin_SizeForegroundPen;
                        data->foreground = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                        data->flags |= BFLG_UseForeground;
                    }
                }
            }

            if (data->nashape!=MUIV_BWin_Shape_None)
            {
                if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_NASizeShinePen,&ptr)) ptr = MUIDEF_BWin_NASizeShinePen;
                data->nashine = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);

                data->flags |= BFLG_UseNAShine;

                if (data->nashape!=MUIV_BWin_Shape_Solid)
                {
                    if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_NASizeShadowPen,&ptr)) ptr = MUIDEF_BWin_NASizeShadowPen;
                    data->nashadow = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                    data->flags |= BFLG_UseNAShadow;

                    if (GETUSENASIZEFOREGROUND(types))
                    {
                        if (!DoSuperMethod(cl,obj,MUIM_GetConfigItem,MUICFG_BWin_NASizeForegroundPen,&ptr)) ptr = MUIDEF_BWin_NASizeForegroundPen;
                        data->naforeground = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)ptr,0);
                        data->flags |= BFLG_UseNAForeground;
                    }
                }
            }

            if ((data->shape!=data->nashape) ||
                (data->shine!=data->nashine) ||
                (data->shadow!=data->nashadow) ||
                !((!(data->flags & BFLG_UseBack) && !(data->flags & BFLG_UseNABack)) || ((data->flags & BFLG_UseBack) && (data->flags & BFLG_UseNABack) && (data->foreground==data->naforeground))))
                    data->flags |= BFLG_UseNA;
            else data->flags &= ~BFLG_UseNA;

            break;
    }

    data->bheight = _screen(obj)->BarHeight;
    data->bright  = _screen(obj)->WBorRight;

    data->flags |= BFLG_Setup;

    return TRUE;
}

/***********************************************************************/

static ULONG ASM
mCleanup(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->flags & BFLG_UseNAForeground) MUI_ReleasePen(muiRenderInfo(obj),data->naforeground);
    if (data->flags & BFLG_UseForeground) MUI_ReleasePen(muiRenderInfo(obj),data->foreground);
    if (data->flags & BFLG_UseNAShadow) MUI_ReleasePen(muiRenderInfo(obj),data->nashadow);
    if (data->flags & BFLG_UseShadow) MUI_ReleasePen(muiRenderInfo(obj),data->shadow);
    if (data->flags & BFLG_UseNAShine) MUI_ReleasePen(muiRenderInfo(obj),data->nashine);
    if (data->flags & BFLG_UseShine) MUI_ReleasePen(muiRenderInfo(obj),data->shine);

    data->flags &= ~(BFLG_Setup|BFLG_UseShine|BFLG_UseShadow|BFLG_UseNAShine|BFLG_UseNAShadow|BFLG_UseForeground|BFLG_UseNAForeground);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG ASM
mAskMinMax(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_AskMinMax *msg)
{
    struct data *data = INST_DATA(cl,obj);
    struct RastPort      rp;
    struct TextExtent    te;

    DoSuperMethodA(cl,obj,(APTR)msg);

    InitRastPort(&rp);
    SetFont(&rp,_font(obj));
    TextExtent(&rp,"X",1,&te);

    switch (data->type)
    {
        case GTYP_WDRAGGING:
            if (data->flags & BFLG_Horiz)
            {
                UWORD h, nah;

                switch(data->shape)
                {
                    case MUIV_BWin_Shape_Line:
                        h = 7;
                        break;

                    default:
                        h = MAX(te.te_Height,8);
                        break;
                }

                switch(data->nashape)
                {
                    case MUIV_BWin_Shape_Line:
                        nah = 7;
                        break;

                    default:
                        nah = MAX(te.te_Height,8);
                        break;
                }

                if (h<nah) h = nah;

                msg->MinMaxInfo->MinWidth = 0;
                msg->MinMaxInfo->DefWidth = 0;
                msg->MinMaxInfo->MaxWidth = MUI_MAXMAX;

                msg->MinMaxInfo->MinHeight = h;
                msg->MinMaxInfo->DefHeight = h;
                msg->MinMaxInfo->MaxHeight = h;
            }
            else
            {
                UWORD w, naw;

                switch(data->shape)
                {
                    case MUIV_BWin_Shape_Line:
                        w = 7;
                        break;

                    default:
                        w = MAX(te.te_Width,8);
                        break;
                }

                switch(data->nashape)
                {
                    case MUIV_BWin_Shape_Line:
                        naw = 7;
                        break;

                    default:
                        naw = MAX(te.te_Width,8);
                        break;
                }

                if (w<naw) w = naw;

                msg->MinMaxInfo->MinWidth = w;
                msg->MinMaxInfo->DefWidth = w;
                msg->MinMaxInfo->MaxWidth = w;

                msg->MinMaxInfo->MinHeight = 0;
                msg->MinMaxInfo->DefHeight = 0;
                msg->MinMaxInfo->MaxHeight = MUI_MAXMAX;
            }
            break;

        case GTYP_SIZING:
        {
            UWORD h = MAX(te.te_Width,8);

            h = 9;

            msg->MinMaxInfo->MinWidth = h;
            msg->MinMaxInfo->DefWidth = h;
            msg->MinMaxInfo->MaxWidth = h;

            msg->MinMaxInfo->MinHeight = h;
            msg->MinMaxInfo->DefHeight = h;
            msg->MinMaxInfo->MaxHeight = h;
            break;
        }
    }

    return 0;
}

/***********************************************************************/

static ULONG ASM
mDraw(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Draw *msg)
{
    struct data     *data = INST_DATA(cl,obj);
    struct RastPort          srp;
    struct RastPort *rp;
    ULONG           activate = data->flags & BFLG_Activate;
    WORD            l, t, r, b, w, h;
    UBYTE           shine = MUIPEN(activate ? data->shine : data->nashine),
                             shadow = MUIPEN(activate ? data->shadow : data->nashadow);

    if (data->flags & BFLG_NTRedraw) return 0;

    DoSuperMethodA(cl,obj,(APTR)msg);
    if (!(msg->flags & (MADF_DRAWUPDATE|MADF_DRAWOBJECT))) return 0;

    memcpy(rp = &srp,_rp(obj),sizeof(srp));

    l = _mleft(obj);
    t = _mtop(obj);
    r = _mright(obj);
    b = _mbottom(obj);
    w = _mwidth(obj);
    h = _mheight(obj);

    switch (data->type)
    {
        case GTYP_WDRAGGING:
        {
            switch (activate ? data->shape : data->nashape)
            {
                case MUIV_BWin_Shape_None:
                    break;

                case MUIV_BWin_Shape_Original:
                {
                    UWORD d[] = {0xAAAA, 0x0000};

                    SetDrMd(rp,JAM1);

                    SetAPen(rp,shine);
                    SetAfPt(rp,d,1);
                    RectFill(rp,l,t,r,b);

                    d[1] = 0xFFFF;
                    d[0] = 0x0000;
                    SetAPen(rp,shadow);
                    RectFill(rp,l,t,r,b);

                    break;
                }

                case MUIV_BWin_Shape_Line:
                {
                    if (data->flags & BFLG_Horiz)
                    {
                        WORD delta = r-l;

                        if (delta>16) delta = 4;
                        else if (delta>8) delta = 2;
                             else delta = 0;

                        l += delta;
                        r -= delta;

                        SetAPen(rp,shine);
                        Move(rp,l,t+2);
                        Draw(rp,r,t+2);

                        SetAPen(rp,shadow);
                        Move(rp,l,t+4);
                        Draw(rp,r,t+4);
                        if (delta)
                        {
                            WritePixel(rp,l-1,t+3);
                            WritePixel(rp,r+1,t+3);
                        }
                    }
                    else
                    {
                        WORD delta = b-t;

                        if (delta>16) delta = 4;
                        else if (delta>8) delta = 2;
                             else delta = 0;

                        t += delta;
                        b -= delta;

                        SetAPen(rp,shine);
                        Move(rp,l+2,t);
                        Draw(rp,l+2,b);

                        SetAPen(rp,shadow);
                        Move(rp,l+4,t);
                        Draw(rp,l+4,b);
                        if (delta)
                        {
                            WritePixel(rp,l+3,t-1);
                            WritePixel(rp,l+3,b+1);
                        }
                    }

                    break;
                }

                case MUIV_BWin_Shape_Solid:
                    SetAPen(rp,shine);
                    RectFill(rp,l,t,r,b);
                    break;
            }

            break;
        }

        case GTYP_SIZING:
        {
            UBYTE useForeground = data->flags & (activate ? BFLG_UseForeground : BFLG_UseNAForeground),
                           foreground = MUIPEN(activate ? data->foreground : data->naforeground);

            switch (activate ? data->shape : data->nashape)
            {
                case MUIV_BWin_Shape_None:
                    break;

                case MUIV_BWin_Shape_Original:
                {
                    WORD i, c;

                    if (useForeground)
                    {
                        SetBPen(rp,foreground);
                        SetDrMd(rp,JAM2);
                    }

                    for (c = 0, i = h-1; i>=0; i--, c = !c)
                    {
                        SetAPen(rp,c ? shine : shadow);
                        rp->LinePtrn = c ? 0xAAAA : 0xFFFF;
                        Move(rp,r-i,b);
                        Draw(rp,r,b-i);
                    }
                    break;
                }

                case MUIV_BWin_Shape_Line:
                {
                    WORD i, c;

                    for (c = 0, i = h-1; i>=0; i--, c++)
                    {
                        switch (c%4)
                        {
                            case 0:
                                SetAPen(rp,shine);
                                break;

                            case 1: case 3:
                                if (useForeground) SetAPen(rp,foreground);
                                else continue;
                                break;

                            case 2:
                                SetAPen(rp,shadow);
                                break;
                        }

                        Move(rp,r-i,b);
                        Draw(rp,r,b-i);
                    }

                    break;
                }

                case MUIV_BWin_Shape_Solid:
                {
                    WORD i;

                    SetAPen(rp,shine);

                    for (i = h-1; i>=0; i--)
                    {
                        Move(rp,r-i,b);
                        Draw(rp,r,b-i);
                    }

                    break;
                }
            }
        }
    }

    return 0;
}

/***********************************************************************/

static ULONG ASM SAVEDS
dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    switch(msg->MethodID)
    {
        case OM_NEW:         return mNew(cl,obj,(APTR)msg);
        case OM_SET:         return mSets(cl,obj,(APTR)msg);
        case MUIM_Draw:      return mDraw(cl,obj,(APTR)msg);
        case MUIM_Setup:     return mSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:   return mCleanup(cl,obj,(APTR)msg);
        case MUIM_AskMinMax: return mAskMinMax(cl,obj,(APTR)msg);
        default:             return DoSuperMethodA(cl,obj,msg);
    }
}

/***************************************************************************/

BOOL ASM
initBoopsi(void)
{
    if ((lib_boopsi = MUI_CreateCustomClass(NULL,MUIC_Boopsi,NULL,sizeof(struct data),dispatcher)))
    {
        if (MUIMasterBase->lib_Version>=20)
            lib_boopsi->mcc_Class->cl_ID = "BWinBoopsi";

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/
