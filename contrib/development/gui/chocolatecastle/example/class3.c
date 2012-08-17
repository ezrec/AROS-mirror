/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/alib.h>

#include <zune/customclasses.h>

#include "class3.h"

/*** Class Data **************************************************************/
struct Class3_DATA
{
    // Insert object instance data here.
};


/*** Methods *****************************************************************/

/*** Class3__MUIM_Setup ***/

IPTR Class3__MUIM_Setup(Class *cl, Object *obj, struct MUIP_Setup *msg)
{
    struct Class3_DATA *data = INST_DATA(cl, obj);
    return 0;
}


/*** Class3__MUIM_Cleanup ***/

IPTR Class3__MUIM_Cleanup(Class *cl, Object *obj, struct MUIP_Cleanup *msg)
{
    struct Class3_DATA *data = INST_DATA(cl, obj);
    return 0;
}


/*** Class3__MUIM_HandleInput ***/

IPTR Class3__MUIM_HandleInput(Class *cl, Object *obj, struct MUIP_HandleInput *msg)
{
    struct Class3_DATA *data = INST_DATA(cl, obj);
    return 0;
}


/*** Class3__MUIM_Draw ***/

IPTR Class3__MUIM_Draw(Class *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct Class3_DATA *data = INST_DATA(cl, obj);
    return 0;
}


/*** Class3__MUIM_AskMinMax ***/

IPTR Class3__MUIM_AskMinMax(Class *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    struct Class3_DATA *data = INST_DATA(cl, obj);
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

