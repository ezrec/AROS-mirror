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

static struct MUI_CustomClass *penslist = NULL;
#define penslistObject NewObject(penslist->mcc_Class,NULL

/***********************************************************************/

struct penslistData
{
    Object *list;
    Object *pens[8];
    APTR   pimages[8];
    LONG   active;
};

/***********************************************************************/

static IPTR
mPenslistNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *list;

    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_CycleChain,    TRUE,
            MUIA_Listview_List, list = (Object *)ListObject,
                InputListFrame,
                MUIA_Background,         MUII_ListBack,
                MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
                MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
            End,
            TAG_MORE,msg->ops_AttrList)))
    {
        struct penslistData *data = INST_DATA(cl,obj);

        data->list   = list;
        data->active = -1;
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mPenslistSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct penslistData  *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_List_Active:
                data->active = (LONG)tidata;
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mPenslistSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct penslistData *data = INST_DATA(cl,obj);
    int                 i;
    struct RastPort              rp;
    struct TextExtent            te;
    UWORD                        w, h;

    static const char *pens[9];

    pens[0] = tr(Msg_Poppen_Shine);
    pens[1] = tr(Msg_Poppen_Halfshine);
    pens[2] = tr(Msg_Poppen_Background);
    pens[3] = tr(Msg_Poppen_Halfshadow);
    pens[4] = tr(Msg_Poppen_Shadow);
    pens[5] = tr(Msg_Poppen_Text);
    pens[6] = tr(Msg_Poppen_Fill);
    pens[7] = tr(Msg_Poppen_Mark);
    pens[8] = NULL;

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    memcpy(&rp,&_screen(obj)->RastPort,sizeof(rp));
    SetFont(&rp,_font(obj));
    TextExtent(&rp,"X",1,&te);
    w = te.te_Height;
    h = te.te_Height;
    if (h>4) h -= 2;

    for (i = 0; i<8; i++)
    {
        char buf[64];

        snprintf(buf,sizeof(buf),(STRPTR)"2:m%d",i);

        data->pens[i] = (Object *)ImageObject,
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert,  TRUE,
            MUIA_FixWidth,        w,
            MUIA_FixHeight,       h,
            MUIA_Image_Spec,      (IPTR)buf,
        End;

        if (data->pens[i])
        {
            data->pimages[i] = (APTR)DoMethod(data->list,MUIM_List_CreateImage,data->pens[i],0);
            #if defined(__LP64__)
            snprintf(buf,sizeof(buf),(STRPTR)"\33O[%016lx] %s",(long unsigned int)data->pimages[i],pens[i]);
            #else
            snprintf(buf,sizeof(buf),(STRPTR)"\33O[%08lx] %s",data->pimages[i],pens[i]);
            #endif
            DoMethod(data->list,MUIM_List_InsertSingle,buf,MUIV_List_Insert_Bottom);
        }
    }

    set(data->list,MUIA_List_Active,data->active);

    return TRUE;
}

/***********************************************************************/

static IPTR
mPenslistCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct penslistData *data = INST_DATA(cl,obj);
    int                 i;

    for (i = 0; i<7; i++)
    {
        if (data->pens[i])
        {
            if (data->pimages[i])
            {
                DoMethod(data->list,MUIM_List_DeleteImage,data->pimages[i]);
                data->pimages[i] = NULL;
            }

            MUI_DisposeObject(data->pens[i]);
            data->pens[i] = NULL;
        }
    }

    DoMethod(data->list,MUIM_List_Clear);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mPenslistShow(struct IClass *cl,Object *obj,Msg msg)
{
    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;
    set(_win(obj),MUIA_Window_DefaultObject,obj);

    return TRUE;
}

/***********************************************************************/

DISPATCHER(penslistDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:       return mPenslistNew(cl,obj,(APTR)msg);
    case OM_SET:       return mPenslistSets(cl,obj,(APTR)msg);
    case MUIM_Setup:   return mPenslistSetup(cl,obj,(APTR)msg);
    case MUIM_Cleanup: return mPenslistCleanup(cl,obj,(APTR)msg);
    case MUIM_Show:    return mPenslistShow(cl,obj,(APTR)msg);
    default:           return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

static ULONG
initPenslist(void)
{
    if((penslist = MUI_CreateCustomClass(NULL, MUIC_Listview, NULL, sizeof(struct penslistData), ENTRY(penslistDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            penslist->mcc_Class->cl_ID = (STRPTR)"Penslist";

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

enum
{
    PAGE_MUI,
    PAGE_Colormap,
    PAGE_RGB,
};

/***********************************************************************/

struct penadjustData
{
    Object *mui;
    Object *colormap;
    Object *rgb;
    char   spec[sizeof(struct MUI_PenSpec)];
    ULONG  flags;
};

/***********************************************************************/

static IPTR
mPenadjustNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *mui, *colormap, *rgb;

    static const char *titles[4];

    titles[0] = tr(Msg_Poppen_MUI);
    titles[1] = tr(Msg_Poppen_Colormap);
    titles[2] = tr(Msg_Poppen_RGB);
    titles[3] = NULL;

    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Register_Titles, titles,
            MUIA_CycleChain,      TRUE,

            /* MUI */
            Child, mui = (Object *)penslistObject, End,

            /* Colormap */
            Child, VGroup,
                Child, owspace(1),
                Child, (IPTR)(colormap = oslider(0,Msg_Poppen_ColorMapHelp,-128,127)),
                Child, owspace(1),
            End,

            /* RGB */
            Child, rgb = (Object *)coloradjustObject, End,

            TAG_MORE, msg->ops_AttrList)))
    {
        struct penadjustData *data = INST_DATA(cl,obj);
        Object               *pop;

        data->mui      = mui;
        data->colormap = colormap;
        data->rgb      = rgb;

        pop = (Object *)GetTagData(MUIA_Popbackground_PopObj, (ULONG)NULL, msg->ops_AttrList);
        DoMethod(mui,MUIM_Notify,MUIA_Listview_DoubleClick,MUIV_EveryTime,MUIV_Notify_Application,5,MUIM_Application_PushMethod,pop,2,MUIM_Popbackground_Close,TRUE);
    }

    return (IPTR)obj;
}

/***********************************************************************/

static ULONG
mPenadjustGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct penadjustData *data = INST_DATA(cl,obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_Pendisplay_Spec:
            if (DoMethod(obj,MUIM_Popbackground_GetSpec,data->spec,NULL,0)==MUIV_Popbackground_GetSpec_Fail)
                return FALSE;
            *msg->opg_Storage = (ULONG)data->spec;
            return TRUE;

        default:
            return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

static ULONG
mPenadjustSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct penadjustData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_Pendisplay_Spec:
            {
              if(DoMethod(obj,MUIM_Popbackground_SetSpec,tidata,NULL,0)!=MUIV_Popbackground_SetSpec_Fail)
                strlcpy(data->spec, (STRPTR)tidata, sizeof(data->spec));
            }
            break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mPenadjustDragQuery(UNUSED struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
    STRPTR x;

    if (obj==msg->obj) return MUIV_DragQuery_Refuse;

    if ((get(msg->obj,MUIA_Pendisplay_Spec,&x) && x && *x) ||
        get(msg->obj,MUIA_Popbackground_Grad,&x))
        return MUIV_DragQuery_Accept;

    if (get(msg->obj,MUIA_Imagedisplay_Spec,&x) && x && (*x=='2') && (*(x+1)==':'))
        return MUIV_DragQuery_Accept;

    return MUIV_DragQuery_Refuse;
}

/***********************************************************************/

static IPTR
mPenadjustDragDrop(UNUSED struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
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

static IPTR
mPenadjustSetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_SetSpec *msg)
{
    struct penadjustData *data = INST_DATA(cl,obj);
    char                 *spec = msg->spec, c;

    if (msg->flags & MUIV_Popbackground_SetSpec_Image)
    {
        if (!spec || (*spec++!='2') || (!*spec++==':') || !(c = *spec++) || !*spec)
            return MUIV_Popbackground_SetSpec_Fail;
    }
    else
        if (!spec || !(c = *spec++) || !*spec) return MUIV_Popbackground_SetSpec_Fail;

    switch (c)
    {
        /* MUI */
        case 'm':
        {
            LONG v;

            v = atoi(spec);
            if (v<0 || v>7) return MUIV_Popbackground_SetSpec_Fail;

            set(data->mui,MUIA_List_Active,v);
            superset(cl,obj,MUIA_Group_ActivePage,PAGE_MUI);
            return MUIV_Popbackground_SetSpec_Spec;
        }

        /* Colormap */
        case 'p':
        {
            LONG v;

            v = atoi(spec);
            if (v<-128 || v>127) return MUIV_Popbackground_SetSpec_Fail;

            set(data->colormap,MUIA_Numeric_Value,v);
            superset(cl,obj,MUIA_Group_ActivePage,PAGE_Colormap);
            return MUIV_Popbackground_SetSpec_Spec;
        }

        /* RGB */
        case 'r':
        {
            struct MUI_RGBcolor rgb;
            char       *p, *s;

            p = strchr(spec,',');
            if (!p) return MUIV_Popbackground_SetSpec_Fail;
            *p = 0;
            if (stch_l(spec,(long *)&rgb.red)!=8) return MUIV_Popbackground_SetSpec_Fail;
            *p++ = ',';

            s = p;
            p = strchr(s,',');
            if (!p) return MUIV_Popbackground_SetSpec_Fail;
            *p = 0;
            if (stch_l(s,(long *)&rgb.green)!=8) return MUIV_Popbackground_SetSpec_Fail;
            *p++ = ',';

            s = p;
            if (stch_l(s,(long *)&rgb.blue)!=8) return MUIV_Popbackground_SetSpec_Fail;

            set(data->rgb,MUIA_Coloradjust_RGB,&rgb);
            superset(cl,obj,MUIA_Group_ActivePage,PAGE_RGB);
            return MUIV_Popbackground_SetSpec_Spec;
         }

        default:
            return MUIV_Popbackground_SetSpec_Fail;
    }
}

/***********************************************************************/

static IPTR
mPenadjustGetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_GetSpec *msg)
{
    struct penadjustData *data = INST_DATA(cl,obj);
    char spec[32];
    IPTR res = MUIV_Popbackground_GetSpec_Spec;
    SIPTR x;

    superget(cl,obj,MUIA_Group_ActivePage,&x);
    switch(x)
    {
        /* MUI */
        case PAGE_MUI:
        {
          x = xget(data->mui, MUIA_List_Active);
          if(x>=0)
            snprintf(spec, sizeof(spec), (STRPTR)"m%d",x);
          else
            res = MUIV_Popbackground_GetSpec_Fail;

        }
        break;

        /* Colormap */
        case PAGE_Colormap:
        {
          x = xget(data->colormap, MUIA_Numeric_Value);
          snprintf(spec, sizeof(spec), (STRPTR)"p%d",x);
        }
        break;

        /* RGB */
        case PAGE_RGB:
        {
          struct MUI_RGBcolor *rgb = (struct MUI_RGBcolor *)xget(data->rgb, MUIA_Coloradjust_RGB);

          snprintf(spec, sizeof(spec), (STRPTR)"r%08x,%08x,%08x", rgb->red, rgb->green, rgb->blue);
        }
        break;
    }

    if (res==MUIV_Popbackground_GetSpec_Fail) *msg->spec = 0;
    else
    {
        if (msg->flags & MUIV_Popbackground_GetSpec_Image)
          sprintf(msg->spec, (STRPTR)"2:%s", spec);
        else
          strcpy(msg->spec, spec);
    }

    return res;
}

/***********************************************************************/

DISPATCHER(penadjustDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:                     return mPenadjustNew(cl,obj,(APTR)msg);
    case OM_GET:                     return mPenadjustGet(cl,obj,(APTR)msg);
    case OM_SET:                     return mPenadjustSets(cl,obj,(APTR)msg);
    case MUIM_DragQuery:             return mPenadjustDragQuery(cl,obj,(APTR)msg);
    case MUIM_DragDrop:              return mPenadjustDragDrop(cl,obj,(APTR)msg);
    case MUIM_Popbackground_SetSpec: return mPenadjustSetSpec(cl,obj,(APTR)msg);
    case MUIM_Popbackground_GetSpec: return mPenadjustGetSpec(cl,obj,(APTR)msg);
    default:                         return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

void
freePenadjust(void)
{
    if (penslist)
    {
        MUI_DeleteCustomClass(penslist);
        penslist = NULL;
    }

    if (lib_penadjust)
    {
        MUI_DeleteCustomClass(lib_penadjust);
        lib_penadjust = NULL;
    }
}

/***********************************************************************/

ULONG
initPenadjust(void)
{
    if (initPenslist() &&
        (lib_penadjust = MUI_CreateCustomClass(NULL, MUIC_Register, NULL, sizeof(struct penadjustData), ENTRY(penadjustDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            lib_penadjust->mcc_Class->cl_ID = (STRPTR)"Penadjust";

        return TRUE;
    }

    freePenadjust();

    return FALSE;
}

/***********************************************************************/
