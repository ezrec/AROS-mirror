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
    Object                      *pen;
    STRPTR                      title;
    char                        spec[sizeof(struct MUI_PenSpec)];
    struct MUIS_TheBar_Gradient grad;
    ULONG                       flags;
};

enum
{
    FLG_Closing = 1<<0,
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
        case MUIA_Pendisplay_Spec:
            *msg->opg_Storage = (IPTR)data->spec;
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
            case MUIA_Pendisplay_Spec:
            {
              strlcpy(data->spec, (STRPTR)tidata, sizeof(data->spec));
              redraw = TRUE;
            }
            break;
        }
    }

    if (redraw)
    {
        if (data->win && !(data->flags & FLG_Closing))
            set(data->pen,MUIA_Pendisplay_Spec,data->spec);

        MUI_Redraw(obj,MADF_DRAWOBJECT);
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

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
            MUIA_Window_Width,          MUIV_Window_Width_MinMax(16),
            MUIA_Window_Height,         MUIV_Window_Height_MinMax(16),
            MUIA_Window_MenuGadget,     FALSE,
            MUIA_Window_SnapshotGadget, FALSE,
            MUIA_Window_ConfigGadget,   FALSE,
            MUIA_Window_IconifyGadget,  FALSE,

            WindowContents, VGroup,
                Child, (IPTR)(data->pen = (Object *)penadjustObject,
                    MUIA_Popbackground_PopObj, (IPTR)obj,
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
        set(data->pen,MUIA_Pendisplay_Spec,data->spec);
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
        if (get(data->pen,MUIA_Pendisplay_Spec,&x)) set(obj,MUIA_Pendisplay_Spec,x);
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

    if ((get(msg->obj,MUIA_Pendisplay_Spec,&x) && x && *x)
        || get(msg->obj,MUIA_Popbackground_Grad,&x))
        return MUIV_DragQuery_Accept;

    if (get(msg->obj,MUIA_Imagedisplay_Spec,&x) && (*x=='2') && (*(x+1)==':'))
        return MUIV_DragQuery_Accept;

    return MUIV_DragQuery_Refuse;
}

/***********************************************************************/

static IPTR
mDragDrop(UNUSED struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
    STRPTR x;

    if (get(msg->obj,MUIA_Pendisplay_Spec,&x)) set(obj,MUIA_Pendisplay_Spec,x);
    else
        if (get(msg->obj,MUIA_Imagedisplay_Spec,&x)) set(obj,MUIA_Pendisplay_Spec,x+2);
        else
            if (get(msg->obj,MUIA_Popbackground_Grad,&x))
            {
                char  spec[sizeof(struct MUI_PenSpec)];
                ULONG c, r, g, b;

                c = (((struct MUIS_TheBar_Gradient *)x)->flags & MUIV_TheBar_Gradient_DragTo) ? ((struct MUIS_TheBar_Gradient *)x)->to : ((struct MUIS_TheBar_Gradient *)x)->from;
                r = c>>16;
                g = (c>>8) & 0xff;
                b = c & 0xff;

                snprintf(spec, sizeof(spec), (STRPTR)"r%08x,%08x,%08x",(r<<24)|(r<<16)|(r<<8)|r,(g<<24)|(g<<16)|(g<<8)|g,(b<<24)|(b<<16)|(b<<8)|b);
                set(obj,MUIA_Pendisplay_Spec,spec);
            }

    return 0;
}

/***********************************************************************/

DISPATCHER(PoppenDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:                   return mNew(cl,obj,(APTR)msg);
    case OM_GET:                   return mGet(cl,obj,(APTR)msg);
    case OM_SET:                   return mSets(cl,obj,(APTR)msg);
    case MUIM_DragQuery:           return mDragQuery(cl,obj,(APTR)msg);
    case MUIM_Cleanup:             return mCleanup(cl,obj,(APTR)msg);
    case MUIM_DragDrop:            return mDragDrop(cl,obj,(APTR)msg);
    case MUIM_Popbackground_Open:  return mOpen(cl,obj,(APTR)msg);
    case MUIM_Popbackground_Close: return mClose(cl,obj,(APTR)msg);
    default:                       return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

void
freePoppen(void)
{
    if (lib_poppen)
    {
        MUI_DeleteCustomClass(lib_poppen);
        lib_poppen = NULL;
    }
}

/***********************************************************************/

ULONG
initPoppen(void)
{
    if((lib_poppen = MUI_CreateCustomClass(NULL, MUIC_Pendisplay, NULL, sizeof(struct data), ENTRY(PoppenDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            lib_poppen->mcc_Class->cl_ID = (STRPTR)"Poppen";

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/
