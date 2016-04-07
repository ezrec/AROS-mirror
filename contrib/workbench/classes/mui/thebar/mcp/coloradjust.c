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
#include <proto/colorwheel.h>
#include <gadgets/gradientslider.h>
#include <gadgets/colorwheel.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>

/***********************************************************************/

struct Library *ColorWheelBase = NULL;
struct Library *GradientSliderBase = NULL;

static struct MUI_CustomClass *gradientslider = NULL;
#define gradientsliderObject NewObject(gradientslider->mcc_Class, NULL

static struct MUI_CustomClass *colorwheel = NULL;
#define colorwheelObject NewObject(colorwheel->mcc_Class, NULL

/***********************************************************************/

enum
{
    FLG_Setup = 1<<0,
    FLG_Show  = 1<<1,
};


/***********************************************************************/

struct colorWheelData
{
    struct ColorWheelHSB        hsb;
    struct MUI_EventHandlerNode eh;
    ULONG                       flags;
};

/***********************************************************************/

static IPTR
mColorWheelNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_FillArea,         FALSE,
            MUIA_ShortHelp,        tr(MSG_Coloradjust_WheelHelp),
            MUIA_Boopsi_ClassID,   "colorwheel.gadget",
            MUIA_Boopsi_MinWidth,  30,
            MUIA_Boopsi_MinHeight, 30,
            MUIA_Boopsi_TagScreen, WHEEL_Screen,
            WHEEL_Screen,          NULL,
            WHEEL_Abbrv,           tr(MSG_Coloradjust_WheelAbbr),
            GA_Left,               0,
            GA_Top,                0,
            GA_Width,              0,
            GA_Height,             0,
            TAG_MORE, msg->ops_AttrList)))
    {
        struct colorWheelData *data = INST_DATA(cl,obj);

        data->hsb.cw_Hue        = 0;
        data->hsb.cw_Saturation = 0;
        data->hsb.cw_Brightness = 0xffffffff;

        msg->MethodID = OM_SET;
        DoMethodA(obj,(Msg)msg);
        msg->MethodID = OM_NEW;
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mColorWheelSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct colorWheelData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case WHEEL_HSB:
                if(data->hsb.cw_Hue != ((struct ColorWheelHSB *)tidata)->cw_Hue ||
                   data->hsb.cw_Saturation != ((struct ColorWheelHSB *)tidata)->cw_Saturation ||
                   data->hsb.cw_Brightness != ((struct ColorWheelHSB *)tidata)->cw_Brightness)
                {
                  memcpy(&data->hsb,(struct ColorWheelHSB *)tidata,sizeof(data->hsb));
                }
                else
                {
                  tag->ti_Tag = TAG_IGNORE;
                }
                break;

            case WHEEL_Hue:
                if(data->hsb.cw_Hue !=tidata)
                {
                  data->hsb.cw_Hue = tidata;
                }
                else
                {
                  tag->ti_Tag = TAG_IGNORE;
                }
                break;

            case WHEEL_Saturation:
                if(data->hsb.cw_Saturation != tidata)
                {
                  data->hsb.cw_Saturation = tidata;
                }
                else
                {
                  tag->ti_Tag = TAG_IGNORE;
                }
                break;

            case WHEEL_Brightness:
                if(data->hsb.cw_Brightness != tidata)
                {
                  data->hsb.cw_Brightness = tidata;
                }
                else
                {
                  tag->ti_Tag = TAG_IGNORE;
                }
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mColorWheelShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct colorWheelData *data = INST_DATA(cl,obj);
    struct ColorWheelHSB           hsb;

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    memcpy(&hsb,&data->hsb,sizeof(hsb));
    if (data->hsb.cw_Hue) data->hsb.cw_Hue = 0;
    else data->hsb.cw_Hue = 1;
    SetSuperAttrs(cl, obj, WHEEL_HSB, &hsb, TAG_DONE);

    memset(&data->eh,0,sizeof(data->eh));
    data->eh.ehn_Class  = cl;
    data->eh.ehn_Object = obj;
    data->eh.ehn_Events = IDCMP_MOUSEMOVE|IDCMP_INTUITICKS;
    DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->eh);

    data->flags |= FLG_Show;

    return TRUE;
}

/***********************************************************************/

static IPTR
mColorWheelHide(struct IClass *cl,Object *obj,Msg msg)
{
    struct colorWheelData *data = INST_DATA(cl,obj);

    #ifdef __AROS__
    // AROS's get() doesn't like a structure as storage pointer
    GetAttr(WHEEL_HSB, obj, (IPTR*)&data->hsb);
    #else
    get(obj,WHEEL_HSB, (ULONG)&data->hsb);
    #endif

    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->eh);

    data->flags &= ~FLG_Show;

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mColorWheelHandleEvent(UNUSED struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
    if ((msg->imsg->Class==IDCMP_MOUSEMOVE) || (msg->imsg->Class==IDCMP_INTUITICKS))
    {
        struct ColorWheelHSB hsb;

        #ifdef __AROS__
        // AROS's get() doesn't like a structure as storage pointer
        GetAttr(WHEEL_HSB,obj,(IPTR*)&hsb);
        #else
        get(obj,WHEEL_HSB,(ULONG)&hsb);
        #endif
        SetAttrs(obj,WHEEL_Hue,hsb.cw_Hue,WHEEL_Saturation,hsb.cw_Saturation,WHEEL_Brightness,hsb.cw_Brightness,TAG_DONE);
    }

    return 0;
}

/***********************************************************************/

DISPATCHER(colorWheelDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:           return mColorWheelNew(cl,obj,(APTR)msg);
    case OM_SET:           return mColorWheelSets(cl,obj,(APTR)msg);
    case MUIM_Show:        return mColorWheelShow(cl,obj,(APTR)msg);
    case MUIM_Hide:        return mColorWheelHide(cl,obj,(APTR)msg);
    case MUIM_HandleEvent: return mColorWheelHandleEvent(cl,obj,(APTR)msg);
    default:               return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

static ULONG
initColorwheel(void)
{
    if((colorwheel = MUI_CreateCustomClass(NULL, MUIC_Boopsi, NULL, sizeof(struct colorWheelData), ENTRY(colorWheelDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            colorwheel->mcc_Class->cl_ID = (STRPTR)"Coloradjust_Colorwheel";

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

struct gradientSliderData
{
    ULONG                       cur;
    UWORD                       *pens;
    struct ColorWheelRGB        rgb;
    struct MUI_EventHandlerNode eh;
    ULONG                       flags;
};

/***********************************************************************/

static IPTR
mGradientSliderNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    UWORD *pens;

    if (!(pens = AllocMem(sizeof(UWORD)*4,MEMF_PUBLIC|MEMF_CLEAR)))
        return 0;

    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_FillArea,         FALSE,
            MUIA_ShortHelp,        tr(MSG_Coloradjust_GradientHelp),
            MUIA_Boopsi_ClassID,   "gradientslider.gadget",
            MUIA_Boopsi_MaxWidth,  18,
            GRAD_CurVal,           0xffff,
            GRAD_PenArray,         pens,
            PGA_Freedom,           LORIENT_VERT,
            GRAD_KnobPixels,       6,
            GA_Left,               0,
            GA_Top,                0,
            GA_Width,              0,
            GA_Height,             0,
            TAG_MORE, msg->ops_AttrList)))
    {
        struct gradientSliderData *data = INST_DATA(cl,obj);

        data->cur          = 0;
        data->rgb.cw_Red   = 0xffffffff;
        data->rgb.cw_Green = 0xffffffff;
        data->rgb.cw_Blue  = 0xffffffff;

        if((data->pens = pens))
          pens[2] = (UWORD)~0;

        msg->MethodID = OM_SET;
        DoMethodA(obj,(Msg)msg);
        msg->MethodID = OM_NEW;
    }
    else if (pens) FreeMem(pens,sizeof(UWORD)*4);

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mGradientSliderDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct gradientSliderData *data = INST_DATA(cl,obj);

    if (data->pens) FreeMem(data->pens,sizeof(UWORD)*4);

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mGradientSliderSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct gradientSliderData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;
    BOOL rgb = FALSE;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case WHEEL_HSB:
            {
                struct ColorWheelHSB h;
                struct ColorWheelRGB r;

                h.cw_Hue        = ((struct ColorWheelHSB *)tidata)->cw_Hue;
                h.cw_Saturation = ((struct ColorWheelHSB *)tidata)->cw_Saturation;
                h.cw_Brightness = 0xffffffff;

                ConvertHSBToRGB(&h,&r);

                if (data->rgb.cw_Red!=r.cw_Red || data->rgb.cw_Green!=r.cw_Green || data->rgb.cw_Blue!=r.cw_Blue)
                {
                    data->rgb.cw_Red   = r.cw_Red;
                    data->rgb.cw_Green = r.cw_Green;
                    data->rgb.cw_Blue  = r.cw_Blue;

                    rgb = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;
            }

            case GRAD_CurVal:
                if (data->cur!=tidata) data->cur = tidata;
                else tag->ti_Tag = TAG_IGNORE;
                break;
        }
    }

    if (rgb && (data->flags & FLG_Setup) && data->pens)
    {
        UWORD p = data->pens[0];

        data->pens[0] = ObtainBestPenA(_screen(obj)->ViewPort.ColorMap,data->rgb.cw_Red,data->rgb.cw_Green,data->rgb.cw_Blue,NULL);
        MUI_Redraw(obj,MADF_DRAWOBJECT);
        ReleasePen(_screen(obj)->ViewPort.ColorMap,p);
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mGradientSliderSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct gradientSliderData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (data->pens)
    {
        data->pens[0]  = ObtainBestPenA(_screen(obj)->ViewPort.ColorMap,data->rgb.cw_Red,data->rgb.cw_Green,data->rgb.cw_Blue,NULL);
        data->pens[1]  = ObtainBestPenA(_screen(obj)->ViewPort.ColorMap,0,0,0,NULL);
    }

    data->flags |= FLG_Setup;

    return TRUE;
}

/***********************************************************************/

static IPTR
mGradientSliderCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct gradientSliderData *data = INST_DATA(cl,obj);

    if (data->pens)
    {
        ReleasePen(_screen(obj)->ViewPort.ColorMap,data->pens[0]);
        ReleasePen(_screen(obj)->ViewPort.ColorMap,data->pens[1]);
    }

    data->flags &= ~FLG_Setup;

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mGradientSliderShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct gradientSliderData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    SetSuperAttrs(cl,obj,GRAD_CurVal,data->cur,MUIA_NoNotify,TRUE,TAG_DONE);

    memset(&data->eh,0,sizeof(data->eh));
    data->eh.ehn_Class  = cl;
    data->eh.ehn_Object = obj;
    data->eh.ehn_Events = IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS;
    DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->eh);

    data->flags |= FLG_Show;

    return TRUE;
}

/***********************************************************************/

static IPTR
mGradientSliderHide(struct IClass *cl,Object *obj,Msg msg)
{
    struct gradientSliderData *data = INST_DATA(cl,obj);

    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->eh);

    data->flags &= ~FLG_Show;

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mGradientSliderHandleEvent(UNUSED struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
    if ((msg->imsg->Class==IDCMP_MOUSEMOVE) || (msg->imsg->Class==IDCMP_MOUSEBUTTONS))
    {
        IPTR x;

        get(obj,GRAD_CurVal,&x);
        set(obj,GRAD_CurVal,x);
    }

    return 0;
}

/***********************************************************************/

DISPATCHER(gradientSliderDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:           return mGradientSliderNew(cl,obj,(APTR)msg);
    case OM_DISPOSE:       return mGradientSliderDispose(cl,obj,(APTR)msg);
    case OM_SET:           return mGradientSliderSets(cl,obj,(APTR)msg);
    case MUIM_Setup:       return mGradientSliderSetup(cl,obj,(APTR)msg);
    case MUIM_Cleanup:     return mGradientSliderCleanup(cl,obj,(APTR)msg);
    case MUIM_Show:        return mGradientSliderShow(cl,obj,(APTR)msg);
    case MUIM_Hide:        return mGradientSliderHide(cl,obj,(APTR)msg);
    case MUIM_HandleEvent: return mGradientSliderHandleEvent(cl,obj,(APTR)msg);
    default:               return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

static ULONG
initGradientslider(void)
{
    if((gradientslider = MUI_CreateCustomClass(NULL, MUIC_Boopsi, NULL, sizeof(struct gradientSliderData), ENTRY(gradientSliderDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            gradientslider->mcc_Class->cl_ID = (STRPTR)"Coloradjust_Gradientslider";

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

struct coloradjustData
{
    Object               *color;
    Object               *red;
    Object               *green;
    Object               *blue;
    Object               *colorwheel;
    Object               *slider;
    struct ColorWheelHSB hsb;
    struct ColorWheelRGB rgb;
    ULONG                flags;
};

/***********************************************************************/

static IPTR
mColoradjustNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if((obj = (Object *)DoSuperNew(cl,obj,MUIA_Group_Horiz,TRUE,TAG_MORE,msg->ops_AttrList)))
    {
        struct coloradjustData *data = INST_DATA(cl,obj);
        Object                 *o;
        struct TagItem         *tag;

        if((tag = FindTagItem(MUIA_Coloradj_Colorfield,msg->ops_AttrList)))
        {
            data->color = (Object *)tag->ti_Data;
        }
        else
        {
            data->color = (Object *)ColorfieldObject,
                TextFrame,
                MUIA_Background,  MUII_ButtonBack,
                MUIA_InnerLeft,   2,
                MUIA_InnerTop,    2,
                MUIA_InnerRight,  2,
                MUIA_InnerBottom, 2,
                MUIA_FixWidth,    24,
            End;
            if (!data->color)
            {
                 CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
                 return 0;
            }
            DoMethod(obj,OM_ADDMEMBER,data->color);
        }

        o = (Object *)VGroup,
            Child, (IPTR)(data->red   = oslider(0,MSG_Coloradjust_RedHelp,0,255)),
            Child, (IPTR)(data->green = oslider(0,MSG_Coloradjust_GreenHelp,0,255)),
            Child, (IPTR)(data->blue  = oslider(0,MSG_Coloradjust_BlueHelp,0,255)),
            Child, HGroup,
                MUIA_Group_HorizSpacing, 1,
                Child, (IPTR)(data->colorwheel = (Object *)colorwheelObject, End),
                Child, (IPTR)(data->slider = (Object *)gradientsliderObject, End),
            End,
        End;
        if (!o)
        {
             CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
             return 0;
        }
        DoMethod(obj,OM_ADDMEMBER,o);

        data->hsb.cw_Hue        = 0;
        data->hsb.cw_Saturation = 0;
        data->hsb.cw_Brightness = 0xffffffff;

        data->rgb.cw_Red   = 0xffffffff;
        data->rgb.cw_Green = 0xffffffff;
        data->rgb.cw_Blue  = 0xffffffff;

        DoSuperMethod(cl,obj,MUIM_MultiSet,MUIA_Numeric_Value,255,data->red,data->green,data->blue,NULL);
        if (data->color) set(data->color,MUIA_Colorfield_RGB,&data->rgb);

        msg->MethodID = OM_SET;
        DoMethodA(obj,(Msg)msg);
        msg->MethodID = OM_NEW;

        DoMethod(data->colorwheel,MUIM_Notify,WHEEL_Hue,MUIV_EveryTime,obj,3,MUIM_Set,WHEEL_Hue,MUIV_TriggerValue);
        DoMethod(data->colorwheel,MUIM_Notify,WHEEL_Saturation,MUIV_EveryTime,obj,3,MUIM_Set,WHEEL_Saturation,MUIV_TriggerValue);
        DoMethod(data->colorwheel,MUIM_Notify,WHEEL_Brightness,MUIV_EveryTime,obj,3,MUIM_Set,WHEEL_Brightness,MUIV_TriggerValue);

        DoMethod(data->slider,MUIM_Notify,GRAD_CurVal,MUIV_EveryTime,obj,3,MUIM_Set,GRAD_CurVal,MUIV_TriggerValue);

        DoMethod(data->red,MUIM_Notify,MUIA_Numeric_Value,MUIV_EveryTime,obj,3,MUIM_Set,MUIA_Coloradj_RedComp,MUIV_TriggerValue);
        DoMethod(data->green,MUIM_Notify,MUIA_Numeric_Value,MUIV_EveryTime,obj,3,MUIM_Set,MUIA_Coloradj_GreenComp,MUIV_TriggerValue);
        DoMethod(data->blue,MUIM_Notify,MUIA_Numeric_Value,MUIV_EveryTime,obj,3,MUIM_Set,MUIA_Coloradj_BlueComp,MUIV_TriggerValue);
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mColoradjustGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct coloradjustData *data = INST_DATA(cl,obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_Coloradjust_RGB:
            *msg->opg_Storage = (IPTR)&data->rgb;
            return TRUE;

        case MUIA_Coloradjust_Red:
            *msg->opg_Storage = data->rgb.cw_Red;
            return TRUE;

        case MUIA_Coloradjust_Green:
            *msg->opg_Storage = data->rgb.cw_Green;
            return TRUE;

        case MUIA_Coloradjust_Blue:
            *msg->opg_Storage = data->rgb.cw_Blue;
            return TRUE;

        default:
            return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

static IPTR
mColoradjustSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct coloradjustData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;
    BOOL nonotify = FALSE, wheel = FALSE, hsb = FALSE, rgb = FALSE, grad = FALSE, comp = FALSE;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case WHEEL_Hue:
                if (data->hsb.cw_Hue!=tidata)
                {
                    data->hsb.cw_Hue = tidata;
                    hsb = wheel = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case WHEEL_Saturation:
                if (data->hsb.cw_Saturation!=tidata)
                {
                    data->hsb.cw_Saturation = tidata;
                    hsb = wheel = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case WHEEL_Brightness:
                if (data->hsb.cw_Brightness!=tidata)
                {
                    data->hsb.cw_Brightness = tidata;
                    hsb = wheel = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case GRAD_CurVal:
            {
                ULONG b = 0xffff-tidata;

                if ((data->hsb.cw_Brightness>>16)!=b)
                {
                    data->hsb.cw_Brightness = (b<<16)|b;
                    hsb = grad = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;
            }

            case MUIA_Coloradj_RedComp:
                if ((data->rgb.cw_Red>>24)!=tidata)
                {
                    data->rgb.cw_Red = (tidata<<24)|(tidata<<16)|(tidata<<8)|tidata;
                    rgb = comp = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_Coloradj_GreenComp:
                if ((data->rgb.cw_Green>>24)!=tidata)
                {
                    data->rgb.cw_Green = (tidata<<24)|(tidata<<16)|(tidata<<8)|tidata;
                    rgb = comp = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_Coloradj_BlueComp:
                if ((data->rgb.cw_Blue>>24)!=tidata)
                {
                    data->rgb.cw_Blue = (tidata<<24)|(tidata<<16)|(tidata<<8)|tidata;
                    rgb = comp = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_Coloradjust_RGB:
                if (data->rgb.cw_Red!=((struct ColorWheelRGB *)tidata)->cw_Red ||
                    data->rgb.cw_Green!=((struct ColorWheelRGB *)tidata)->cw_Green ||
                    data->rgb.cw_Blue!=((struct ColorWheelRGB *)tidata)->cw_Blue)
                {
                    data->rgb.cw_Red   = ((struct ColorWheelRGB *)tidata)->cw_Red;
                    data->rgb.cw_Green = ((struct ColorWheelRGB *)tidata)->cw_Green;
                    data->rgb.cw_Blue  = ((struct ColorWheelRGB *)tidata)->cw_Blue;

                    rgb = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_Coloradjust_Red:
                if (data->rgb.cw_Red!=tidata)
                {
                    data->rgb.cw_Red = tidata;
                    rgb = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_Coloradjust_Green:
                if (data->rgb.cw_Green!=tidata)
                {
                    data->rgb.cw_Green = tidata;
                    rgb = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_Coloradjust_Blue:
                if (data->rgb.cw_Blue!=tidata)
                {
                    data->rgb.cw_Blue = tidata;
                    rgb = TRUE;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_NoNotify:
                nonotify = tidata;
                break;
        }
    }

    if (hsb) ConvertHSBToRGB(&data->hsb,&data->rgb);
    else
        if (rgb)
        {
            ConvertRGBToHSB(&data->rgb,&data->hsb);
            ConvertHSBToRGB(&data->hsb,&data->rgb);
        }

    if (hsb || rgb)
    {
        if (!grad)
        {
            SetAttrs(data->slider,GRAD_CurVal,   0xffff-(data->hsb.cw_Brightness>>16),
                                  WHEEL_HSB,     &data->hsb,
                                  MUIA_NoNotify, TRUE,
                                  TAG_DONE);
        }

        if (!comp)
        {
            nnset(data->red,MUIA_Numeric_Value,data->rgb.cw_Red>>24);
            nnset(data->green,MUIA_Numeric_Value,data->rgb.cw_Green>>24);
            nnset(data->blue,MUIA_Numeric_Value,data->rgb.cw_Blue>>24);
        }

        if (!wheel) nnset(data->colorwheel,WHEEL_HSB,&data->hsb);

        if (data->color) set(data->color,MUIA_Colorfield_RGB,&data->rgb);

        if (!nonotify)
            SetSuperAttrs(cl,obj,MUIA_Coloradjust_RGB,&data->rgb,
                                 MUIA_Coloradjust_Red,data->rgb.cw_Red,
                                 MUIA_Coloradjust_Green,data->rgb.cw_Green,
                                 MUIA_Coloradjust_Blue,data->rgb.cw_Blue,
                                 TAG_DONE);
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mColoradjustSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct coloradjustData *data = INST_DATA(cl,obj);

    if ((GetBitMapAttr(_screen(obj)->RastPort.BitMap,BMA_DEPTH)>8) &&
        CyberGfxBase && IsCyberModeID(GetVPModeID(&_screen(obj)->ViewPort)))
    {
        set(data->red,MUIA_Background,"2:rffffffff,00000000,00000000");
        set(data->green,MUIA_Background,"2:r00000000,ffffffff,00000000");
        set(data->blue,MUIA_Background,"2:r00000000,00000000,ffffffff,");
    }
    else
    {
        set(data->red,MUIA_Background,MUII_SliderBack);
        set(data->green,MUIA_Background,MUII_SliderBack);
        set(data->blue,MUIA_Background,MUII_SliderBack);
    }

    /*
    set(data->red,MUIA_FillArea,FALSE);
    set(data->green,MUIA_FillArea,FALSE);
    set(data->blue,MUIA_FillArea,FALSE);
    */

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mColoradjustShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct coloradjustData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (data->color) set(data->color,MUIA_Colorfield_RGB,&data->rgb);

    return TRUE;
}

/***********************************************************************/

static IPTR
mColoradjustDragQuery(UNUSED struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
    STRPTR x;

    if (obj==msg->obj) return MUIV_DragQuery_Refuse;

    if (get(msg->obj,MUIA_Coloradjust_RGB,&x) || get(msg->obj,MUIA_Colorfield_RGB,&x) ||
        get(msg->obj,MUIA_Popbackground_Grad,&x))
        return MUIV_DragQuery_Accept;

    if (get(msg->obj,MUIA_Pendisplay_Spec,&x) && x && (*x=='r'))
        return MUIV_DragQuery_Accept;

    if (get(msg->obj,MUIA_Imagedisplay_Spec,&x) && x && (*x=='2') && (*(x+1)==':') && (*(x+2)=='r'))
        return MUIV_DragQuery_Accept;

    return MUIV_DragQuery_Refuse;
}

/***********************************************************************/

static IPTR
mColoradjustDragDrop(UNUSED struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
    STRPTR x;

    if (get(msg->obj,MUIA_Coloradjust_RGB,&x) || get(msg->obj,MUIA_Colorfield_RGB,&x))
    {
        set(obj,MUIA_Coloradjust_RGB,x);
    }
    else
    {
        ULONG im;

        im = get(msg->obj,MUIA_Imagedisplay_Spec,&x);

        if (im || get(msg->obj,MUIA_Pendisplay_Spec,&x))
        {
            char        spec[sizeof(struct MUI_PenSpec)];
            struct ColorWheelRGB rgb;
            char        *p;

            if (im) x +=2;
            else x++;

            strlcpy(spec, x, sizeof(spec));

            if((p = strchr(spec,',')))
            {
                *p = 0;
                if (stch_l(spec,(long *)&rgb.cw_Red)==8)
                {
                    char *s;

                    s = ++p;
                    if((p = strchr(s,',')))
                    {
                        *p = 0;
                        if (stch_l(s,(long *)&rgb.cw_Green)==8)
                        {
                            s = ++p;
                            if (stch_l(s,(long *)&rgb.cw_Blue)==8) set(obj,MUIA_Coloradjust_RGB,&rgb);
                        }
                    }
                }
            }
        }
        else
            if (get(msg->obj,MUIA_Popbackground_Grad,&x))
            {
                struct ColorWheelRGB rgb;
                ULONG       c, r, g, b;

                c = (((struct MUIS_TheBar_Gradient *)x)->flags & MUIV_TheBar_Gradient_DragTo) ? ((struct MUIS_TheBar_Gradient *)x)->to : ((struct MUIS_TheBar_Gradient *)x)->from;
                r = c>>16;
                g = (c>>8) & 0xff;
                b = c & 0xff;

                rgb.cw_Red   = (r<<24)|(r<<16)|(r<<8)|r;
                rgb.cw_Green = (g<<24)|(g<<16)|(g<<8)|g;
                rgb.cw_Blue  = (b<<24)|(b<<16)|(b<<8)|b;

                set(obj,MUIA_Coloradjust_RGB,&rgb);
            }
    }

    return 0;
}

/***********************************************************************/

DISPATCHER(coloradjustDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:         return mColoradjustNew(cl,obj,(APTR)msg);
    case OM_GET:         return mColoradjustGet(cl,obj,(APTR)msg);
    case OM_SET:         return mColoradjustSets(cl,obj,(APTR)msg);
    case MUIM_Setup:     return mColoradjustSetup(cl,obj,(APTR)msg);
    case MUIM_Show:      return mColoradjustShow(cl,obj,(APTR)msg);
    case MUIM_DragQuery: return mColoradjustDragQuery(cl,obj,(APTR)msg);
    case MUIM_DragDrop:  return mColoradjustDragDrop(cl,obj,(APTR)msg);
    default:             return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

void
freeColoradjust(void)
{
    if (ColorWheelBase)
    {
        CloseLibrary(ColorWheelBase);
        ColorWheelBase = NULL;
    }

    if (GradientSliderBase)
    {
        CloseLibrary(GradientSliderBase);
        GradientSliderBase = NULL;
    }

    if (gradientslider)
    {
        MUI_DeleteCustomClass(gradientslider);
        gradientslider = NULL;
    }

    if (colorwheel)
    {
        MUI_DeleteCustomClass(colorwheel);
        colorwheel = NULL;
    }

    if (lib_coloradjust)
    {
        MUI_DeleteCustomClass(lib_coloradjust);
        lib_coloradjust = NULL;
    }
}

/***********************************************************************/

ULONG
initColoradjust(void)
{
    if ((ColorWheelBase = OpenLibrary("gadgets/colorwheel.gadget",0)) &&
        (GradientSliderBase = OpenLibrary("gadgets/gradientslider.gadget",0)) &&
        initColorwheel() &&
        initGradientslider() &&
        (lib_coloradjust = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct coloradjustData), ENTRY(coloradjustDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            lib_coloradjust->mcc_Class->cl_ID = (STRPTR)"Coloradjust";

        return TRUE;
    }

    freeColoradjust();

    return FALSE;
}

/***********************************************************************/
