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
#include <proto/alib.h>
#include <proto/graphics.h>
#include <intuition/classusr.h>
#include <intuition/screens.h>
#include <intuition/intuition.h>

#include "Scalos.h"
#include "RectFillClass.h"
#include "ScalosIntern.h"
#include "SubRoutines.h"
#include "scalos_protos.h"

#include "Debug.h"

static ULONG RectFill_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct RectFillInst *inst)
{
        ULONG ret = FALSE;

        DEBUG("Got Init\n");

        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                inst->o = SC_NewObject(NULL,SCC_DOSFILEICON_NAME,SCCA_Icon_Name,"tools:utilities/mouse-xy",TAG_DONE);
                SC_DoMethod(inst->o,SCCM_Icon_GetIcon,SCCV_Icon_GetIcon_Disk);
                ret = TRUE;
        }
        DEBUG1("Got Init: %ld\n",ret);
        return(ret);
}

static ULONG RectFill_Setup(struct SC_Class *cl, Object *obj, Msg msg, struct RectFillInst *inst)
{
        ULONG ret = FALSE;

        DEBUG("Got Setup\n");

        if (SC_DoSuperMethodA(cl, obj, msg))
        {
                SC_DoMethod(inst->o, SCCM_Graphic_PreThinkScreen, scRenderInfo(obj)->screenobj);
                SC_DoMethod(obj,SCCM_Area_SetIDCMP,0);
                ret = TRUE;
        }
        DEBUG1("Got Setup: %ld\n",ret);
        return(ret);
}

static void RectFill_Show(struct SC_Class *cl, Object *obj, Msg msg, struct RectFillInst *inst)
{
        DEBUG("Got Show\n");
        SC_DoSuperMethodA(cl, obj, msg);

        SC_DoMethod(inst->o, SCCM_Graphic_PreThinkWindow, scRenderInfo(obj)->rport, 0);

        DEBUG4("BoxSize: %ld,%ld / %ld,%ld\n",(LONG) _scleft(obj),(LONG) _sctop(obj),(LONG) _scwidth(obj),(LONG) _scheight(obj));

        DEBUG("Show finished\n");
}

static void RectFill_Draw(struct SC_Class *cl, Object *obj, Msg msg, struct RectFillInst *inst)
{
        struct DrawInfo *drinfo = (struct DrawInfo *) get(scRenderInfo(obj)->screenobj, SCCA_Screen_DrawInfo);
                // struct Region *region = NewRegion();
        struct Rectangle rect;

        rect.MinX = _scleft(obj);
        rect.MinY = _sctop(obj);
        rect.MaxX = _scright(obj) - 10;
        rect.MaxY = _scbottom(obj);

        DEBUG("Got Draw\n");
        DEBUG4("BoxSize: %ld,%ld / %ld,%ld\n",(LONG) _scleft(obj),(LONG) _sctop(obj),(LONG) _scwidth(obj),(LONG) _scheight(obj));

        SC_DoSuperMethodA(cl, obj, msg);

        SC_DoMethod(inst->o, SCCM_Graphic_Draw, _scleft(obj), _sctop(obj),SCCV_Graphic_DrawFlags_AbsPos);

                /*
        OrRectRegion(region,&rect);
        SC_DoMethod(_scwinobj(obj),SCCM_Window_SetClipRegion,region);

        if (inst->state)
                SetAPen(_scrport(obj),(drinfo->dri_Pens)[SHINEPEN]);
        else
                SetAPen(_scrport(obj),(drinfo->dri_Pens)[FILLPEN]);

        RectFill(_scrport(obj), _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj));

        SC_DoMethod(_scwinobj(obj),SCCM_Window_ClearClipRegion);
        DisposeRegion(region);
                */

        DEBUG("Draw finished\n");
}

static void RectFill_Hide(struct SC_Class *cl, Object *obj, Msg msg, struct RectFillInst *inst)
{
        DEBUG("Got Hide\n");
        SC_DoMethod(inst->o, SCCM_Graphic_PostThinkWindow, 0);

        SC_DoSuperMethodA(cl, (Object *) obj, msg);
        DEBUG("Hide finished\n");
}

static void RectFill_Cleanup(struct SC_Class *cl, Object *obj, Msg msg, struct RectFillInst *inst)
{
        SC_DoMethod(inst->o, SCCM_Graphic_PostThinkScreen);

        SC_DoSuperMethodA(cl, (Object *) obj, msg);
        DEBUG("Got Cleanup\n");
}

static void RectFill_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct RectFillInst *inst)
{
        DEBUG("Got Exit\n");
        SC_DisposeObject(inst->o);

        SC_DoSuperMethodA(cl, (Object *) obj, msg);
        DEBUG("Exit finished\n");
}

static void RectFill_AskMinMax(struct SC_Class *cl, Object *obj, struct SCCP_Area_AskMinMax *msg, struct RectFillInst *inst)
{
        DEBUG("Got AskMinMax\n");

        SC_DoSuperMethodA(cl,obj, (Msg) msg);
        msg->sizes->minwidth += 10;
        msg->sizes->minheight += 10;
        msg->sizes->maxwidth = SCCV_Area_MaxSize;
        msg->sizes->maxheight = SCCV_Area_MaxSize;
        msg->sizes->defwidth += 100;
        msg->sizes->defheight += 100;

        DEBUG("AskMinMax finished\n");
}

static void RectFill_SetIDCMP(struct SC_Class *cl, Object *obj, struct SCCP_Area_SetIDCMP *msg, struct RectFillInst *inst)
{
        msg->IDCMP |= IDCMP_MOUSEBUTTONS;
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
}

static void RectFill_HandleInput(struct SC_Class *cl, Object *obj, struct SCCP_Area_HandleInput *msg, struct RectFillInst *inst)
{
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
        if (msg->imsg->Class == IDCMP_MOUSEBUTTONS)
        {
                DEBUG("HandleInput !!\n");
                if (msg->imsg->Code == SELECTDOWN)
                {
                        inst->state = 1;
                        SC_SetAttrs(inst->o,SCCA_Icon_Selected,TRUE,TAG_DONE);
                                // SC_DoMethod(_scwinobj(obj), SCCM_Window_Redraw, obj, SCCV_Area_Drawtype_Complete);
                }
                if (msg->imsg->Code == SELECTUP)
                {
                        inst->state = 0;
                        SC_SetAttrs(inst->o,SCCA_Icon_Selected,FALSE,TAG_DONE);
                                //SC_DoMethod(_scwinobj(obj), SCCM_Window_Redraw, obj, SCCV_Area_Drawtype_Complete);
                }
        }
}

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData RectFillMethods[] =
{
        { SCCM_Init,(ULONG) RectFill_Init, 0, 0, NULL },
        { SCCM_Area_Setup,(ULONG) RectFill_Setup, 0, 0, NULL },
        { SCCM_Area_AskMinMax,(ULONG) RectFill_AskMinMax, 0, 0, NULL },
        { SCCM_Area_Show,(ULONG) RectFill_Show, 0, 0, NULL },
        { SCCM_Area_Draw,(ULONG) RectFill_Draw, 0, 0, NULL },
        { SCCM_Area_Hide,(ULONG) RectFill_Hide, 0, 0, NULL },
        { SCCM_Area_Cleanup,(ULONG) RectFill_Cleanup, 0, 0, NULL },
        { SCCM_Exit,(ULONG) RectFill_Exit, 0, 0, NULL },
        { SCCM_Area_SetIDCMP,(ULONG) RectFill_SetIDCMP, 0, 0, NULL },
        { SCCM_Area_HandleInput,(ULONG) RectFill_HandleInput, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0,0, NULL }
};

