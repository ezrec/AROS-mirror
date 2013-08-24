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

 $Id$

***************************************************************************/

#include "class.h"

/***********************************************************************/

struct data
{
    Object                      *win;
    Object                      *back;
    STRPTR                      title;
    char                        spec[POPBACKGROUNDSPECSIZE];
    struct MUIS_TheBar_Gradient grad;
    struct MUI_EventHandlerNode eh;
    ULONG                       flags;
};

enum
{
    FLG_Gradient     = 1<<0,
    FLG_GradientMode = 1<<1,
    FLG_CyberDeep    = 1<<2,
    FLG_Closing      = 1<<3,
};

/***********************************************************************/

static IPTR
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if((obj = (Object *)DoSuperNew(cl,obj,
            ButtonFrame,
            MUIA_InputMode,   MUIV_InputMode_RelVerify,
            MUIA_InnerLeft,   4,
            MUIA_InnerTop,    4,
            MUIA_InnerRight,  4,
            MUIA_InnerBottom, 4,
            TAG_MORE, msg->ops_AttrList)))
    {
        struct data *data = INST_DATA(cl,obj);

        data->title = (STRPTR)GetTagData(MUIA_Window_Title, (IPTR)NULL, msg->ops_AttrList);

        DoMethod(obj,MUIM_Notify,MUIA_Pressed,FALSE,obj,1,MUIM_Popbackground_Open);

        if (GetTagData(MUIA_Popbackground_Gradient,FALSE,msg->ops_AttrList))
            data->flags |= FLG_Gradient;
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_Imagedisplay_Spec:
            if (data->flags & FLG_GradientMode) return FALSE;
            *msg->opg_Storage = (IPTR)data->spec;
            return TRUE;

        case MUIA_Popbackground_Grad:
            if (!(data->flags & FLG_GradientMode)) return FALSE;
            *msg->opg_Storage = (IPTR)&data->grad;
            return TRUE;

        default:
            return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

static IPTR
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;
    BOOL redraw = FALSE;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_Imagedisplay_Spec:
            {
                strlcpy(data->spec, (STRPTR)tidata, sizeof(data->spec));
                data->flags &= ~FLG_GradientMode;
                redraw = TRUE;
            }
            break;

            case MUIA_Popbackground_Grad:
                memcpy(&data->grad,(struct MUIS_TheBar_Gradient *)tidata,sizeof(data->grad));
                SetSuperAttrs(cl,obj,MUIA_Imagedisplay_Spec,NULL,TAG_DONE);
                data->flags |= FLG_GradientMode;
                redraw = TRUE;
                break;
        }
    }

    if (redraw)
    {
        if(data->win && !(data->flags & FLG_Closing))
        {
            if (data->flags & FLG_GradientMode) set(data->back,MUIA_Popbackground_Grad,&data->grad);
            else set(data->back,MUIA_Imagedisplay_Spec,data->spec);
        }

        MUI_Redraw(obj,MADF_DRAWOBJECT);
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg)) return FALSE;

    if ((GetBitMapAttr(_screen(obj)->RastPort.BitMap,BMA_DEPTH)>8) &&
        CyberGfxBase && IsCyberModeID(GetVPModeID(&_screen(obj)->ViewPort)))
        data->flags |= FLG_CyberDeep;
    else data->flags &= ~FLG_CyberDeep;

    return TRUE;
}

/***********************************************************************/

static IPTR
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data    *data = INST_DATA(cl,obj);

    if (data->win)
    {
        set(data->win,MUIA_Window_Open,FALSE);
        DoMethod(_app(obj),OM_REMMEMBER,data->win);
        MUI_DisposeObject(data->win);
        data->win = NULL;
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    memset(&data->eh,0,sizeof(data->eh));
    data->eh.ehn_Class  = cl;
    data->eh.ehn_Object = obj;
    data->eh.ehn_Events = IDCMP_RAWKEY;
    DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->eh);

    return TRUE;
}

/***********************************************************************/

static IPTR
mHide(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->eh);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoSuperMethodA(cl,obj,(Msg)msg);

    if ((data->flags & FLG_GradientMode) && (data->flags & FLG_CyberDeep) && (msg->flags & (MADF_DRAWUPDATE|MADF_DRAWOBJECT)))
        drawGradient(obj,&data->grad);

    return 0;
}

/***********************************************************************/

static IPTR
mHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (msg->imsg && (msg->imsg->Class==IDCMP_RAWKEY))
    {
        //Printf("%lx %lx\n",msg->imsg->Qualifier,msg->imsg->Code);
        if (msg->imsg->Code==0x60 || msg->imsg->Code==0x61) data->grad.flags |= MUIV_TheBar_Gradient_DragTo;
        else data->grad.flags &= ~MUIV_TheBar_Gradient_DragTo;
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mDeleteDragImage(struct IClass *cl,Object *obj,struct MUIP_DeleteDragImage *msg)
{
    struct data *data = INST_DATA(cl,obj);

    data->grad.flags &= ~MUIV_TheBar_Gradient_DragTo;

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mOpen(struct IClass *cl,Object *obj, UNUSED Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!data->win)
    {
        Object *ok, *cancel;

        data->win = (Object *)WindowObject,
            MUIA_Window_Title,          (IPTR)data->title,
            MUIA_Window_LeftEdge,       _left(obj),
            MUIA_Window_TopEdge,        _bottom(obj)+1,
            MUIA_Window_RefWindow,      (IPTR)_win(obj),
            MUIA_Window_Height, MUIV_Window_Height_MinMax(16),
            MUIA_Window_MenuGadget,     FALSE,
            MUIA_Window_SnapshotGadget, FALSE,
            MUIA_Window_ConfigGadget,   FALSE,
            MUIA_Window_IconifyGadget,  FALSE,
            WindowContents, VGroup,
                Child, (IPTR)(data->back = (Object *)backgroundadjustObject,
                    MUIA_Popbackground_PopObj,   (IPTR)obj,
                    MUIA_Popbackground_Gradient, data->flags & FLG_Gradient,
                End),
                Child, HGroup,
                    Child, (IPTR)(ok = obutton(Msg_Pop_OK,Msg_Pop_OK_Help)),
                    Child, (IPTR)HSpace(0),
                    Child, (IPTR)(cancel = obutton(Msg_Pop_Cancel,Msg_Pop_Cancel_Help)),
                End,
            End,
        End;

        if (data->win)
        {
            DoSuperMethod(cl,obj,MUIM_MultiSet,MUIA_CycleChain,TRUE,ok,cancel,NULL);

            DoMethod(data->win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,5,MUIM_Application_PushMethod,obj,2,MUIM_Popbackground_Close,FALSE);
            DoMethod(ok,MUIM_Notify,MUIA_Pressed,FALSE,MUIV_Notify_Application,5,MUIM_Application_PushMethod,obj,2,MUIM_Popbackground_Close,TRUE);
            DoMethod(cancel,MUIM_Notify,MUIA_Pressed,FALSE,MUIV_Notify_Application,5,MUIM_Application_PushMethod,obj,2,MUIM_Popbackground_Close,FALSE);

            DoMethod(_app(obj),OM_ADDMEMBER,data->win);
        }
    }

    if (data->win)
    {
        if (data->flags & FLG_GradientMode) set(data->back,MUIA_Popbackground_Grad,&data->grad);
        else set(data->back,MUIA_Imagedisplay_Spec,data->spec);

        set(data->win,MUIA_Window_Open,TRUE);
    }

    return 0;
}

/***********************************************************************/

static IPTR
mClose(struct IClass *cl,Object *obj,struct MUIP_Popbackground_Close *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (msg->success)
    {
        IPTR x;

        data->flags |= FLG_Closing;
        if (get(data->back,MUIA_Imagedisplay_Spec,&x)) set(obj,MUIA_Imagedisplay_Spec,x);
        else if (get(data->back,MUIA_Popbackground_Grad,&x)) set(obj,MUIA_Popbackground_Grad,x);
        data->flags &= ~FLG_Closing;
    }

    set(data->win,MUIA_Window_Open,FALSE);
    DoMethod(_app(obj),OM_REMMEMBER,data->win);
    MUI_DisposeObject(data->win);
    data->win = NULL;

    return 0;
}

/***********************************************************************/

static IPTR
mDragQuery(UNUSED struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
    STRPTR x;

    if (obj==msg->obj) return MUIV_DragQuery_Refuse;

    if (((get(msg->obj,MUIA_Imagedisplay_Spec,&x) || get(msg->obj,MUIA_Pendisplay_Spec,&x)) && x && *x) ||
        get(msg->obj,MUIA_Popbackground_Grad,&x)) return MUIV_DragQuery_Accept;

    return MUIV_DragQuery_Refuse;
}

/***********************************************************************/

static IPTR
mDragDrop(struct IClass *cl, Object *obj,struct MUIP_DragDrop *msg)
{
  struct data *data = INST_DATA(cl,obj);
  IPTR                x;

  if((x = xget(msg->obj, MUIA_Imagedisplay_Spec)))
    set(obj,MUIA_Imagedisplay_Spec,x);
  else if((x = xget(msg->obj,MUIA_Pendisplay_Spec)))
  {
    char spec[sizeof(struct MUI_PenSpec)+2];

    snprintf(spec,sizeof(spec),(STRPTR)"2:%s",(char *)x);
    set(obj,MUIA_Imagedisplay_Spec,spec);
  }
  else if((x = xget(msg->obj,MUIA_Popbackground_Grad)))
  {
    if(data->flags & FLG_Gradient)
      set(obj,MUIA_Popbackground_Grad,&x);
    else
    {
      char  spec[sizeof(struct MUI_PenSpec)+2];
      ULONG c, r, g, b;

      c = (((struct MUIS_TheBar_Gradient *)x)->flags & MUIV_TheBar_Gradient_DragTo) ? ((struct MUIS_TheBar_Gradient *)x)->to : ((struct MUIS_TheBar_Gradient *)x)->from;
      r = c>>16;
      g = (c>>8) & 0xff;
      b = c & 0xff;

      snprintf(spec, sizeof(spec), (STRPTR)"2:r%08x,%08x,%08x",(r<<24)|(r<<16)|(r<<8)|r,(g<<24)|(g<<16)|(g<<8)|g,(b<<24)|(b<<16)|(b<<8)|b);
      set(obj,MUIA_Imagedisplay_Spec,spec);
    }
  }

  return 0;
}

/***********************************************************************/

DISPATCHER(PopbackgroundDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:                   return mNew(cl,obj,(APTR)msg);
    case OM_GET:                   return mGet(cl,obj,(APTR)msg);
    case OM_SET:                   return mSets(cl,obj,(APTR)msg);
    case MUIM_Setup:               return mSetup(cl,obj,(APTR)msg);
    case MUIM_Cleanup:             return mCleanup(cl,obj,(APTR)msg);
    case MUIM_Show:                return mShow(cl,obj,(APTR)msg);
    case MUIM_Hide:                return mHide(cl,obj,(APTR)msg);
    case MUIM_Draw:                return mDraw(cl,obj,(APTR)msg);
    case MUIM_HandleEvent:         return mHandleEvent(cl,obj,(APTR)msg);
    case MUIM_DeleteDragImage:     return mDeleteDragImage(cl,obj,(APTR)msg);
    case MUIM_DragQuery:           return mDragQuery(cl,obj,(APTR)msg);
    case MUIM_DragDrop:            return mDragDrop(cl,obj,(APTR)msg);
    case MUIM_Popbackground_Open:  return mOpen(cl,obj,(APTR)msg);
    case MUIM_Popbackground_Close: return mClose(cl,obj,(APTR)msg);
    default:                       return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

void
freePopbackground(void)
{
    if (lib_popbackground)
    {
        MUI_DeleteCustomClass(lib_popbackground);
        lib_popbackground = NULL;
    }
}

/***********************************************************************/

ULONG
initPopbackground(void)
{
    if((lib_popbackground = MUI_CreateCustomClass(NULL, MUIC_Imagedisplay, NULL, sizeof(struct data), ENTRY(PopbackgroundDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            lib_popbackground->mcc_Class->cl_ID = (STRPTR)"Popbackground";

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/
