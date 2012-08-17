/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/graphics.h>
#include <proto/alib.h>

#include <zune/customclasses.h>

#include "class3.h"

/*** Class Data **************************************************************/
struct Class3_DATA
{
    int x, y, sx, sy;
};


/*** Methods *****************************************************************/

/*** Class3__MUIM_Setup ***/

IPTR Class3__MUIM_Setup(Class *cl, Object *obj, struct MUIP_Setup *msg)
{
    // struct Class3_DATA *data = INST_DATA(cl, obj);

    if (!(DoSuperMethodA(cl, obj, msg)))
        return FALSE;

    MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY);

    return TRUE;
}


/*** Class3__MUIM_Cleanup ***/

IPTR Class3__MUIM_Cleanup(Class *cl, Object *obj, struct MUIP_Cleanup *msg)
{
    // struct Class3_DATA *data = INST_DATA(cl, obj);

    MUI_RejectIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY);

    return DoSuperMethodA(cl, obj, msg);
}


/*** Class3__MUIM_HandleInput ***/

IPTR Class3__MUIM_HandleInput(Class *cl, Object *obj, struct MUIP_HandleInput *msg)
{
    struct Class3_DATA *data = INST_DATA(cl, obj);

    #define _between(a,x,b) ((x)>=(a) && (x)<=(b))
    #define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))

    if (msg->muikey!=MUIKEY_NONE)
    {
        switch (msg->muikey)
        {
            case MUIKEY_LEFT : data->sx=-1; MUI_Redraw(obj, MADF_DRAWUPDATE); break;
            case MUIKEY_RIGHT: data->sx= 1; MUI_Redraw(obj, MADF_DRAWUPDATE); break;
            case MUIKEY_UP   : data->sy=-1; MUI_Redraw(obj, MADF_DRAWUPDATE); break;
            case MUIKEY_DOWN : data->sy= 1; MUI_Redraw(obj, MADF_DRAWUPDATE); break;
        }
    }

    if (msg->imsg)
    {
        switch (msg->imsg->Class)
        {
            case IDCMP_MOUSEBUTTONS:
            {
                if (msg->imsg->Code==SELECTDOWN)
                {
                    if (_isinobject(msg->imsg->MouseX, msg->imsg->MouseY))
                    {
                        data->x = msg->imsg->MouseX;
                        data->y = msg->imsg->MouseY;
                        MUI_Redraw(obj, MADF_DRAWUPDATE);
                        MUI_RequestIDCMP(obj, IDCMP_MOUSEMOVE);
                    }
                }
                else
                    MUI_RejectIDCMP(obj, IDCMP_MOUSEMOVE);
            }
            break;

            case IDCMP_MOUSEMOVE:
            {
                if (_isinobject(msg->imsg->MouseX, msg->imsg->MouseY))
                {
                    data->x = msg->imsg->MouseX;
                    data->y = msg->imsg->MouseY;
                    MUI_Redraw(obj, MADF_DRAWUPDATE);
                }
            }
            break;
        }
    }

    return DoSuperMethodA(cl, obj, msg);
}


/*** Class3__MUIM_Draw ***/

IPTR Class3__MUIM_Draw(Class *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct Class3_DATA *data = INST_DATA(cl, obj);

    DoSuperMethodA(cl,obj,msg);

    if (msg->flags & MADF_DRAWUPDATE)
    {
        if (data->sx || data->sy)
        {
            SetBPen(_rp(obj), _dri(obj)->dri_Pens[SHINEPEN]);
            ScrollRaster(_rp(obj), data->sx, data->sy, _mleft(obj),
                _mtop(obj), _mright(obj), _mbottom(obj));
            SetBPen(_rp(obj), 0);
            data->sx = 0;
            data->sy = 0;
        }
        else
        {
            SetAPen(_rp(obj), _dri(obj)->dri_Pens[SHADOWPEN]);
            WritePixel(_rp(obj), data->x, data->y);
        }
    }
    else if (msg->flags & MADF_DRAWOBJECT)
    {
        SetAPen(_rp(obj), _dri(obj)->dri_Pens[SHINEPEN]);
        RectFill(_rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));
    }

    return 0;
}


/*** Class3__MUIM_AskMinMax ***/

IPTR Class3__MUIM_AskMinMax(Class *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    // struct Class3_DATA *data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, msg);

    msg->MinMaxInfo->MinWidth  += 100;
    msg->MinMaxInfo->DefWidth  += 120;
    msg->MinMaxInfo->MaxWidth  += 500;

    msg->MinMaxInfo->MinHeight += 40;
    msg->MinMaxInfo->DefHeight += 90;
    msg->MinMaxInfo->MaxHeight += 300;

    return 0;
}


/*** Setup *******************************************************************/
ZUNE_CUSTOMCLASS_5
(
    Class3, NULL, MUIC_Area, NULL,
    MUIM_Setup,     struct MUIP_Setup *,
    MUIM_Cleanup,     struct MUIP_Cleanup *,
    MUIM_HandleInput,     struct MUIP_HandleInput *,
    MUIM_Draw,     struct MUIP_Draw *,
    MUIM_AskMinMax,     struct MUIP_AskMinMax *
);

