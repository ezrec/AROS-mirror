// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <intuition/classusr.h>
#include <intuition/screens.h>
#include <intuition/intuition.h>

#include "Scalos.h"
#include "GFXTestClass.h"
#include "ScalosIntern.h"
#include "SubRoutines.h"
#include "scalos_protos.h"

#include "debug.h"

static ULONG GFXTest_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct GFXTestInst *inst)
{
        ULONG ret = FALSE;

        DEBUG("Got Init\n");

        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                if (inst->obj = SC_NewObject(NULL,SCC_TEXT_NAME,SCCA_Text_String,"Test 1 2 3 test",SCCA_Text_TextPen,2,SCCA_Text_Type,SCCV_Text_Type_Shadow,TAG_DONE))
                {
                        ret = TRUE;
                }
        }
        DEBUG1("Got Init: %ld\n",ret);
        return(ret);
}

static ULONG GFXTest_Setup(struct SC_Class *cl, Object *obj, Msg msg, struct GFXTestInst *inst)
{
        ULONG ret = FALSE;

        DEBUG("Got Setup\n");

        if (SC_DoSuperMethodA(cl, obj, msg))
        {
                SC_DoMethod(obj,SCCM_Area_SetIDCMP,0);
                SC_DoMethod(inst->obj,SCCM_Graphic_PreThinkScreen,((struct SC_AreaObject *) obj)->rinfo->screenobj);
                ret = TRUE;
        }
        DEBUG1("Got Setup: %ld\n",ret);
        return(ret);
}

static void GFXTest_Show(struct SC_Class *cl, Object *obj, Msg msg, struct GFXTestInst *inst)
{
        DEBUG("Got Show\n");
        SC_DoSuperMethodA(cl, obj, msg);

        SC_DoMethod(inst->obj,SCCM_Graphic_PreThinkWindow,((struct SC_AreaObject *) obj)->rinfo->rport,SCCV_Area_Action_OpenClose);

        DEBUG("Show finished\n");
}

static void GFXTest_Draw(struct SC_Class *cl, struct SC_AreaObject *obj, Msg msg, struct GFXTestInst *inst)
{
        DEBUG("Got Draw\n");

        SC_DoSuperMethodA(cl, (Object *) obj, msg);

        SC_DoMethod(inst->obj,SCCM_Graphic_Draw,obj->bounds.Left,obj->bounds.Top,SCCV_Graphic_DrawFlags_RelPos);

        DEBUG("Draw finished\n");
}

static void GFXTest_Hide(struct SC_Class *cl, Object *obj, Msg msg, struct GFXTestInst *inst)
{
        DEBUG("Got Hide\n");
        SC_DoMethod(inst->obj,SCCM_Graphic_PostThinkWindow);
        SC_DoSuperMethodA(cl, (Object *) obj, msg);
        DEBUG("Hide finished\n");
}

static void GFXTest_Cleanup(struct SC_Class *cl, Object *obj, Msg msg, struct GFXTestInst *inst)
{
        SC_DoMethod(inst->obj,SCCM_Graphic_PostThinkScreen);
        SC_DoSuperMethodA(cl, (Object *) obj, msg);
        DEBUG("Got Cleanup\n");
}

static void GFXTest_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct GFXTestInst *inst)
{
        DEBUG("Got Exit\n");
        SC_DisposeObject(inst->obj);
        SC_DoSuperMethodA(cl, (Object *) obj, msg);
        DEBUG("Exit finished\n");
}

static void GFXTest_AskMinMax(struct SC_Class *cl, Object *obj, struct SCCP_Area_AskMinMax *msg, struct GFXTestInst *inst)
{
        DEBUG("Got AskMinMax\n");

        SC_DoSuperMethodA(cl,obj, (Msg) msg);
        msg->sizes->minwidth += get(inst->obj,SCCA_Graphic_Width);
        msg->sizes->minheight += get(inst->obj,SCCA_Graphic_Height);
        msg->sizes->maxwidth = SCCV_Area_MaxSize;
        msg->sizes->maxheight = SCCV_Area_MaxSize;
        msg->sizes->defwidth += get(inst->obj,SCCA_Graphic_Width);
        msg->sizes->defheight += get(inst->obj,SCCA_Graphic_Height);

        DEBUG("AskMinMax finished\n");
}

static void GFXTest_SetIDCMP(struct SC_Class *cl, Object *obj, struct SCCP_Area_SetIDCMP *msg, struct GFXTestInst *inst)
{
        msg->IDCMP |= IDCMP_MOUSEBUTTONS;
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
}

static void GFXTest_HandleInput(struct SC_Class *cl, Object *obj, struct SCCP_Area_HandleInput *msg, struct GFXTestInst *inst)
{
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
                /*
        if (msg->imsg->Class == IDCMP_MOUSEBUTTONS)
        {
                DEBUG("HandleInput !!\n");
                if (msg->imsg->Code == SELECTDOWN)
                {
                        SC_DoMethod(_scwinobj(obj), SCCM_Window_Redraw, obj, SCCV_Area_Drawtype_Complete);
                }
                if (msg->imsg->Code == SELECTUP)
                {
                        SC_DoMethod(_scwinobj(obj), SCCM_Window_Redraw, obj, SCCV_Area_Drawtype_Complete);
                }
        }
                */
}

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData GFXTestMethods[] =
{
        { SCCM_Init,(ULONG) GFXTest_Init, 0, 0, NULL },
        { SCCM_Area_Setup,(ULONG) GFXTest_Setup, 0, 0, NULL },
        { SCCM_Area_AskMinMax,(ULONG) GFXTest_AskMinMax, 0, 0, NULL },
        { SCCM_Area_Show,(ULONG) GFXTest_Show, 0, 0, NULL },
        { SCCM_Area_Draw,(ULONG) GFXTest_Draw, 0, 0, NULL },
        { SCCM_Area_Hide,(ULONG) GFXTest_Hide, 0, 0, NULL },
        { SCCM_Area_Cleanup,(ULONG) GFXTest_Cleanup, 0, 0, NULL },
        { SCCM_Exit,(ULONG) GFXTest_Exit, 0, 0, NULL },
        { SCCM_Area_SetIDCMP,(ULONG) GFXTest_SetIDCMP, 0, 0, NULL },
        { SCCM_Area_HandleInput,(ULONG) GFXTest_HandleInput, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0,0, NULL }
};

