/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

***************************************************************************/

#include "class.h"

/***********************************************************************/

#undef _riflags
#define _riflags(obj) (MUIMRI_THINFRAMES)

/***********************************************************************/

struct data
{
    Object *bar;
    long   pshine;
    long   pshadow;
    UWORD  sp;
    ULONG  flags;
};

enum
{
    FLG_Horiz                = 1<<0,
    FLG_Bar                  = 1<<1,
    FLG_UserBarSpacerSpacing = 1<<2,
    FLG_ShowMe               = 1<<3,
};

/***********************************************************************/

static IPTR
mNew(struct IClass *cl, Object *obj, struct opSet *msg)
{
    ENTER();

    if ((obj = (Object *)DoSuperNew(cl,obj,
            isFlagSet(lib_flags,BASEFLG_MUI20) ? TAG_IGNORE : MUIA_CustomBackfill, TRUE,
            TAG_MORE, (IPTR)msg->ops_AttrList)))
    {
        struct data    *data = INST_DATA(cl,obj);
        struct TagItem *tag;

        data->bar = (Object *)GetTagData(MUIA_TheButton_TheBar,FALSE,msg->ops_AttrList);

        data->flags = (GetTagData(MUIA_Group_Horiz,FALSE,msg->ops_AttrList) ? FLG_Horiz : 0) |
                      ((GetTagData(MUIA_TheButton_Spacer,MUIV_TheButton_Spacer_Bar,msg->ops_AttrList)==MUIV_TheButton_Spacer_Bar) ? FLG_Bar : 0) |
                      (GetTagData(MUIA_ShowMe,TRUE,msg->ops_AttrList) ? FLG_ShowMe : 0);

        if ((tag = FindTagItem(MUIA_TheBar_BarSpacerSpacing,msg->ops_AttrList)) &&
            ((int)tag->ti_Data>=0 && (int)tag->ti_Data<=16))
        {
            data->sp = tag->ti_Data;
            data->flags |= FLG_UserBarSpacerSpacing;
        }
    }

    RETURN((IPTR)obj);
    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct data *data = INST_DATA(cl,obj);
    IPTR        result = FALSE;

    ENTER();

    switch (msg->opg_AttrID)
    {
        case MUIA_TheButton_Spacer: *msg->opg_Storage = isFlagSet(data->flags,FLG_Bar) ? MUIV_TheButton_Spacer_Bar : MUIV_TheButton_Spacer_Button; result = TRUE; break;
        case MUIA_ShowMe:           *msg->opg_Storage = isFlagSet(data->flags,FLG_ShowMe) ? TRUE : FALSE; result = TRUE; break;
        default:                    result = DoSuperMethodA(cl,obj,(Msg)msg);
    }

    RETURN(result);
    return result;
}

/***********************************************************************/

static IPTR
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;
    IPTR result = 0;

    ENTER();

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        ULONG tidata = tag->ti_Data;

        switch (tag->ti_Tag)
        {
            case MUIA_Group_Horiz:
                if (tidata) setFlag(data->flags,FLG_Horiz);
                else clearFlag(data->flags,FLG_Horiz);
                break;

            case MUIA_TheButton_Spacer:
                if (tidata==MUIV_TheButton_Spacer_Bar) setFlag(data->flags,FLG_Bar);
                else clearFlag(data->flags,FLG_Bar);
                break;

            case MUIA_ShowMe:
                if (BOOLSAME(isFlagSet(data->flags,FLG_ShowMe),tidata))
                    tag->ti_Data = TAG_IGNORE;
                else
                {
                    if (tidata) setFlag(data->flags,FLG_ShowMe);
                    else clearFlag(data->flags,FLG_ShowMe);
                }
                break;
        }
    }

    result = DoSuperMethodA(cl,obj,(Msg)msg);

    RETURN(result);
    return result;
}

/***********************************************************************/

static IPTR
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR        pen;
    ULONG      *val;

    ENTER();

    if (!DoSuperMethodA(cl,obj,(Msg)msg))
    {
        RETURN(FALSE);
        return FALSE;
    }

    if (isFlagSet(data->flags,FLG_Bar))
    {
        if (!getconfigitem(cl,obj,MUICFG_TheBar_BarSpacerShinePen,&pen))
            pen = MUIDEF_TheBar_BarSpacerShinePen;
        data->pshine = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

        if (!getconfigitem(cl,obj,MUICFG_TheBar_BarSpacerShadowPen,&pen))
            pen = MUIDEF_TheBar_BarSpacerShadowPen;
        data->pshadow = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

        if (isFlagClear(data->flags, FLG_UserBarSpacerSpacing))
        {
            data->sp = getconfigitem(cl,obj,MUICFG_TheBar_BarSpacerSpacing,&val) ?
                *val : MUIDEF_TheBar_BarSpacerSpacing;
        }
    }

    RETURN(TRUE);
    return TRUE;
}

/***********************************************************************/

static IPTR
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    IPTR        result = 0;

    ENTER();

    if (isFlagSet(data->flags,FLG_Bar))
    {
        MUI_ReleasePen(muiRenderInfo(obj),data->pshine);
        MUI_ReleasePen(muiRenderInfo(obj),data->pshadow);
    }

    result = DoSuperMethodA(cl,obj,msg);

    RETURN(result);
    return result;
}

/***********************************************************************/

static IPTR
mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       delta = 2*data->sp+2;

    ENTER();

    DoSuperMethodA(cl,obj,(Msg)msg);

    if (isFlagSet(data->flags,FLG_Bar))
    {
        if (isFlagSet(data->flags,FLG_Horiz))
        {
            if (isFlagSet(_riflags(obj),MUIMRI_THINFRAMES))
                delta += 2;

            msg->MinMaxInfo->MinWidth  += delta;
            msg->MinMaxInfo->DefWidth  += delta;
            msg->MinMaxInfo->MaxWidth  += delta;
            msg->MinMaxInfo->MaxHeight  = MUI_MAXMAX;
        }
        else
        {
            msg->MinMaxInfo->MinHeight += delta;
            msg->MinMaxInfo->DefHeight += delta;
            msg->MinMaxInfo->MaxWidth   = MUI_MAXMAX;
            msg->MinMaxInfo->MaxHeight += delta;
        }
    }
    else
    {
        msg->MinMaxInfo->MaxWidth  = MUI_MAXMAX;
        msg->MinMaxInfo->MaxHeight = MUI_MAXMAX;
    }

    RETURN(0);
    return 0;
}

/***********************************************************************/

static IPTR
mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct data *data = INST_DATA(cl,obj);

    ENTER();

    DoSuperMethodA(cl,obj,(Msg)msg);

    if (isFlagSet(data->flags,FLG_Bar) && (isFlagSet(msg->flags,MADF_DRAWOBJECT) || isFlagSet(msg->flags,MADF_DRAWOBJECT)))
    {
        struct RastPort *rp = _rp(obj);

        if (isFlagSet(data->flags,FLG_Horiz))
        {
            UWORD l, t, b, fw = isFlagSet(_riflags(obj),MUIMRI_THINFRAMES) ? 1 : 2;

            l = _mleft(obj)+(_mwidth(obj)>>1)-fw;
            t = _mtop(obj)+1;
            b = _mbottom(obj)-1;

            SetAPen(rp,MUIPEN(data->pshadow));
            RectFill(rp,l,t,l+fw-1,b);

            SetAPen(rp,MUIPEN(data->pshine));
            RectFill(rp,l+fw,t,l+fw+fw-1,b);
        }
        else
        {
            UWORD l, t, r;

            l = _mleft(obj)+1;
            t = _mtop(obj)+(_mheight(obj)>>1)-1;
            r = _mright(obj)-1;

            SetAPen(rp,MUIPEN(data->pshadow));
            RectFill(rp,l,t,r,t);

            SetAPen(rp,MUIPEN(data->pshine));
            RectFill(rp,l,t+1,r,t+1);
        }
    }

    RETURN(0);
    return 0;
}

/***********************************************************************/

static IPTR
mBackfill(struct IClass *cl,Object *obj,struct MUIP_Backfill *msg)
{
    struct data *data = INST_DATA(cl,obj);
    IPTR        result = 0;

    ENTER();

    if (data->bar)
    {
        result = DoMethod(data->bar,MUIM_Backfill,msg->left,
                                                  msg->top,
                                                  msg->right,
                                                  msg->bottom,
                                                  msg->left+msg->xoffset,
                                                  msg->top+msg->yoffset,
                                                  0);
    }
    else
    {
        result = DoSuperMethod(cl,obj,MUIM_DrawBackground,msg->left,
                                                          msg->top,
                                                          msg->right-msg->left+1,
                                                          msg->bottom-msg->top+1,
                                                          msg->xoffset,
                                                          msg->yoffset,
                                                          0);
    }

    RETURN(result);
    return result;
}

/***********************************************************************/

DISPATCHER(SpacerDispatcher)
{
    switch(msg->MethodID)
    {
        case OM_NEW:              return mNew(cl,obj,(APTR)msg);
        case OM_GET:              return mGet(cl,obj,(APTR)msg);
        case OM_SET:              return mSets(cl,obj,(APTR)msg);
        case MUIM_AskMinMax:      return mAskMinMax(cl,obj,(APTR)msg);
        case MUIM_Draw:           return mDraw(cl,obj,(APTR)msg);
        case MUIM_Setup:          return mSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:        return mCleanup(cl,obj,(APTR)msg);
        case MUIM_Backfill:       return mBackfill(cl,obj,(APTR)msg);
        default:                  return DoSuperMethodA(cl,obj,msg);
    }
}

/***********************************************************************/

BOOL
initSpacerClass(void)
{
    BOOL result = FALSE;

    ENTER();

    if ((lib_spacerClass = MUI_CreateCustomClass(NULL,(STRPTR)MUIC_Area,NULL,sizeof(struct data),ENTRY(SpacerDispatcher))))
    {
        if(isFlagSet(lib_flags, BASEFLG_MUI20))
            lib_spacerClass->mcc_Class->cl_ID = (STRPTR)"TheBar_Spacer";

        result = TRUE;
    }

    RETURN(result);
    return result;
}

/***********************************************************************/

