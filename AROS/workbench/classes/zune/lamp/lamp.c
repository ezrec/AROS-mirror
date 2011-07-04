/*
    Copyright © 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <utility/tagitem.h>
#include <libraries/mui.h>
#include <dos/dos.h>
#include <zune/iconimage.h>
#include <exec/memory.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>

#include <aros/inquire.h>
#include <proto/aros.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/dos.h>
#include <proto/alib.h>
#include <proto/graphics.h>

#include <string.h>

#include "lamp.h"
#include "lamp_private.h"


#define DEBUG 1
#include <aros/debug.h>

// Lamp sizes from MUIV_Lamp_Type_Tiny to MUIV_Lamp_Type_Huge
const static ULONG lampsizes[] = {5, 6, 7, 8, 9};

// FIXME: find proper colors
const static ULONG defaultcolors[][3] =
{
    {0x00000000, 0x00000000, 0x00000000}, // MUIV_Lamp_Color_Off           
    {0x00000000, 0xffffffff, 0x00000000}, // MUIV_Lamp_Color_Ok           
    {0x00000000, 0x00000000, 0x00000000}, // MUIV_Lamp_Color_Warning      
    {0x00ffffff, 0x00000000, 0x00000000}, // MUIV_Lamp_Color_Error        
    {0xffffffff, 0x00000000, 0x00000000}, // MUIV_Lamp_Color_FatalError    
    {0x00000000, 0x00ffffff, 0x00000000}, // MUIV_Lamp_Color_Processing    
    {0x00000000, 0x00ffffff, 0x00000000}, // MUIV_Lamp_Color_LookingUp  
    {0x00000000, 0x00ffffff, 0x00000000}, // MUIV_Lamp_Color_Connecting   
    {0x00000000, 0x00ffffff, 0x00000000}, // MUIV_Lamp_Color_SendingData  
    {0x00000000, 0x00ffffff, 0x00000000}, // MUIV_Lamp_Color_ReceivingData 
    {0x00000000, 0x00ffffff, 0x00000000}, // MUIV_Lamp_Color_LoadingData 
    {0x00000000, 0x00ffffff, 0x00000000}, // MUIV_Lamp_Color_SavingData
};

/*** Methods ****************************************************************/
Object *Lamp__OM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct Lamp_DATA           *data;
    const struct TagItem       *tstate = msg->ops_AttrList;
    struct TagItem             *tag;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
    if (!obj) return FALSE;

    data = INST_DATA(cl, obj);

    // Defaults
    data->lmp_Type = MUIV_Lamp_Type_Medium;
    data->lmp_ColorSpec = MUIV_Lamp_Color_Off;
    data->lmp_Color[0] = defaultcolors[0][0];
    data->lmp_Color[1] = defaultcolors[0][1];
    data->lmp_Color[2] = defaultcolors[0][2];
    data->lmp_ColorType = MUIV_Lamp_ColorType_UserDefined;

    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Lamp_Type:
                data->lmp_Type = tag->ti_Data;
                break;

            case MUIA_Lamp_Color:
                if (tag->ti_Data < 12)
                {
                    data->lmp_ColorSpec = tag->ti_Data;
                    data->lmp_Color[0] = defaultcolors[data->lmp_ColorSpec][0];
                    data->lmp_Color[1] = defaultcolors[data->lmp_ColorSpec][1];
                    data->lmp_Color[2] = defaultcolors[data->lmp_ColorSpec][2];
                    data->lmp_ColorType = MUIV_Lamp_ColorType_UserDefined;
                }
                else
                {
                    ULONG *cols = (ULONG *)tag->ti_Data;
                    data->lmp_Color[0] = cols[0];
                    data->lmp_Color[1] = cols[1];
                    data->lmp_Color[2] = cols[2];
                    data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                }
                break;

            case MUIA_Lamp_Red:
                data->lmp_Color[0] = tag->ti_Data;
                data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                break; 

            case MUIA_Lamp_Green:
                data->lmp_Color[1] = tag->ti_Data;
                data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                break;

            case MUIA_Lamp_Blue:
                data->lmp_Color[2] = tag->ti_Data;
                data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                break;

            case MUIA_Lamp_PenSpec:
                data->lmp_PenSpec = *((struct MUI_PenSpec *)tag->ti_Data);
                data->lmp_ColorType = MUIV_Lamp_ColorType_PenSpec;
                break;
        }
    }

    return obj;
}


IPTR Lamp__OM_SET(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct Lamp_DATA           *data   = INST_DATA(cl, obj);
    const struct TagItem       *tstate = msg->ops_AttrList;
    struct TagItem             *tag;

    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Lamp_Type:
                data->lmp_Type = tag->ti_Data;
                break;

            case MUIA_Lamp_Color:
                if (tag->ti_Data < 12)
                {
                    data->lmp_ColorSpec = tag->ti_Data;
                    data->lmp_Color[0] = defaultcolors[data->lmp_ColorSpec][0];
                    data->lmp_Color[1] = defaultcolors[data->lmp_ColorSpec][1];
                    data->lmp_Color[2] = defaultcolors[data->lmp_ColorSpec][2];
                    data->lmp_ColorType = MUIV_Lamp_ColorType_UserDefined;
                }
                else
                {
                    ULONG *cols = (ULONG *)tag->ti_Data;
                    data->lmp_Color[0] = cols[0];
                    data->lmp_Color[1] = cols[1];
                    data->lmp_Color[2] = cols[2];
                    data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                }
                break;

            case MUIA_Lamp_Red:
                data->lmp_Color[0] = tag->ti_Data;
                data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                break; 

            case MUIA_Lamp_Green:
                data->lmp_Color[1] = tag->ti_Data;
                data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                break;

            case MUIA_Lamp_Blue:
                data->lmp_Color[2] = tag->ti_Data;
                data->lmp_ColorType = MUIV_Lamp_ColorType_Color;
                break;

            case MUIA_Lamp_PenSpec:
                data->lmp_PenSpec = *((struct MUI_PenSpec *)tag->ti_Data);
                data->lmp_ColorType = MUIV_Lamp_ColorType_PenSpec;
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg) msg);
}


IPTR Lamp__OM_GET(struct IClass *cl, Object *obj, struct opGet *msg)
{
    struct Lamp_DATA   *data  = INST_DATA(cl, obj);
    IPTR               *store = msg->opg_Storage;

    switch (msg->opg_AttrID)
    {
        case MUIA_Lamp_Type:
            *store = data->lmp_Type;
            return TRUE;

        case MUIA_Lamp_Color:
            *store = (IPTR)data->lmp_Color;
            return TRUE;

        case MUIA_Lamp_ColorType:
            *store = data->lmp_ColorType;
            return TRUE;

        case MUIA_Lamp_Red:
            *store = data->lmp_Color[0];
            return TRUE;

        case MUIA_Lamp_Green:
            *store = data->lmp_Color[1];
            return TRUE;

        case MUIA_Lamp_Blue:
            *store = data->lmp_Color[2];
            return TRUE;

        case MUIA_Lamp_PenSpec:
            *store = (IPTR)&data->lmp_PenSpec;
            return TRUE;
    }
    
    return DoSuperMethodA(cl, obj, (Msg) msg);
}


IPTR Lamp__MUIM_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    // struct Lamp_DATA *data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (!(msg->flags & MADF_DRAWOBJECT))
            return 0;

    // FIXME: Draw something

    return 0;
}


IPTR Lamp__MUIM_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth  += 10;
    msg->MinMaxInfo->DefWidth  += 10;
    msg->MinMaxInfo->MaxWidth  += 10;

    msg->MinMaxInfo->MinHeight += 10;
    msg->MinMaxInfo->DefHeight += 10;
    msg->MinMaxInfo->MaxHeight += 10;

    return 0;
}


IPTR Lamp__MUIM_Lamp_SetRGB(struct IClass *cl, Object *obj, struct MUIP_Lamp_SetRGB *msg)
{
    // struct Lamp_DATA   *data  = INST_DATA(cl, obj);

    ULONG rgb[3];

    rgb[0] = msg->red;
    rgb[1] = msg->green;
    rgb[2] = msg->blue;

    SET(obj, MUIA_Lamp_Color, rgb);

    return 0;
}
