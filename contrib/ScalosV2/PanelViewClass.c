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
#include <clib/utility_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <graphics/gfx.h>

#include "Scalos.h"
#include "PanelViewClass.h"
#include "SubRoutines.h"
#include "WindowClass.h"

#include "scalos_protos.h"
#include "debug.h"

/****** PanelView.scalos/--background ***************************************
*
*   PanelView class is the group class for panel objects.
*   It handles the input events from intuition and calls the apropriate
*   methods of the assigned panel object.
*
*****************************************************************************
*/
// /

/** Init
*/
static ULONG PanelView_Init(struct SC_Class *cl, Object *obj, struct opSet *msg, struct PanelViewInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj,(Msg) msg))
        {
                if (!(inst->panel = (Object *) GetTagData(SCCA_PanelView_Panel,0,msg->ops_AttrList)))
                {
                        SC_DoClassMethod(cl,NULL,(ULONG) obj,SCCM_Exit);
                        return(FALSE);
                }
                SC_DoSuperMethod(cl,obj,OM_ADDMEMBER,inst->panel);

                SC_DoMethod(inst->panel,SCCM_Notify,SCCA_Panel_MinX,SCCV_EveryTime,obj,
                                        3,SCCM_Set,SCCA_PanelView_MinX,SCCV_TriggerValue);
                SC_DoMethod(inst->panel,SCCM_Notify,SCCA_Panel_MinY,SCCV_EveryTime,obj,
                                        3,SCCM_Set,SCCA_PanelView_MinY,SCCV_TriggerValue);
                SC_DoMethod(inst->panel,SCCM_Notify,SCCA_Panel_MaxX,SCCV_EveryTime,obj,
                                        3,SCCM_Set,SCCA_PanelView_MaxX,SCCV_TriggerValue);
                SC_DoMethod(inst->panel,SCCM_Notify,SCCA_Panel_MaxY,SCCV_EveryTime,obj,
                                        3,SCCM_Set,SCCA_PanelView_MaxY,SCCV_TriggerValue);

                SC_DoMethod(inst->panel,SCCM_Notify,SCCA_Panel_Left,SCCV_EveryTime,obj,
                                        3,SCCM_Set,SCCA_PanelView_Left,SCCV_TriggerValue);
                SC_DoMethod(inst->panel,SCCM_Notify,SCCA_Panel_Top,SCCV_EveryTime,obj,
                                        3,SCCM_Set,SCCA_PanelView_Top,SCCV_TriggerValue);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Setup
*/
static ULONG PanelView_Setup(struct SC_Class *cl, Object *obj, struct SCCP_Area_Setup *msg, struct PanelViewInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj,(Msg) msg))
        {
                SC_SetAttrs(_scwinobj(obj),SCCA_Window_SliderRight,TRUE,SCCA_Window_SliderBottom,TRUE,TAG_DONE);
                inst->idcmp = IDCMP_GADGETUP | IDCMP_GADGETDOWN;
                SC_DoMethod(obj,SCCM_Area_SetIDCMP,0);
                SC_DoMethod(_scwinobj(obj),SCCM_Window_DelayedShowObject,obj);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Show
*/
static void PanelView_Show(struct SC_Class *cl, Object *obj, struct SCCP_Area_Show *msg, struct PanelViewInst *inst)
{
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
        inst->sliderright = (struct Gadget *) get(_scwinobj(obj),SCCA_Window_SliderRight);
        inst->sliderbottom = (struct Gadget *) get(_scwinobj(obj),SCCA_Window_SliderBottom);
}
// /

/** DelayedShow
*/
static void PanelView_DelayedShow(struct SC_Class *cl, Object *obj, Msg msg, struct PanelViewInst *inst)
{
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
        SetGadgetAttrs(inst->sliderbottom,_scwindow(obj),NULL,PGA_Total,inst->maxx - inst->minx + 1,
                                   PGA_Visible,get(inst->panel,SCCA_Area_InnerWidth),PGA_Top,inst->left - inst->minx,
                                   TAG_DONE);
        SetGadgetAttrs(inst->sliderright,_scwindow(obj),NULL,PGA_Total,inst->maxy - inst->miny + 1,
                                   PGA_Visible,get(inst->panel,SCCA_Area_InnerHeight),PGA_Top,inst->top - inst->miny,
                                   TAG_DONE);
}
// /

/** Hide
*/
static void PanelView_Hide(struct SC_Class *cl, Object *obj, struct SCCP_Area_Hide *msg, struct PanelViewInst *inst)
{
        SC_DoSuperMethodA(cl, obj, (Msg) msg);
}
// /

/** Set
*/
static void PanelView_Set(struct SC_Class *cl, Object *obj, struct opSet *msg, struct PanelViewInst *inst)
{
        struct TagItem *taglist = msg->ops_AttrList;
        struct TagItem **tags = &taglist;
        struct TagItem *tag;
        BOOL newx = FALSE;
        BOOL newy = FALSE;

        while(tag = NextTagItem(tags))
        {
                switch (tag->ti_Tag)
                {
                        case SCCA_PanelView_MinX :
                                inst->minx = tag->ti_Data;
                                newx = TRUE;
                                break;
                        case SCCA_PanelView_MaxX :
                                inst->maxx = tag->ti_Data;
                                DEBUG1("New MaxX: %ld\n",inst->maxx);
                                newx = TRUE;
                                break;
                        case SCCA_PanelView_Left :
                                inst->left = tag->ti_Data;
                                newx = TRUE;
                                break;

                        case SCCA_PanelView_MinY :
                                inst->miny = tag->ti_Data;
                                newy = TRUE;
                                break;
                        case SCCA_PanelView_MaxY :
                                inst->maxy = tag->ti_Data;
                                newy = TRUE;
                                break;
                        case SCCA_PanelView_Top :
                                inst->left = tag->ti_Data;
                                newy = TRUE;
                                break;
                }
        }

        if (inst->minx > inst->left)
        {
                inst->left = inst->minx;
                SC_SetAttrs(inst->panel,SCCA_Panel_Left,inst->left,TAG_DONE);
        }
        if (inst->miny > inst->top)
        {
                inst->top = inst->miny;
                SC_SetAttrs(inst->panel,SCCA_Panel_Top,inst->top,TAG_DONE);
        }
                

        if (newx && inst->sliderbottom)
                SetGadgetAttrs(inst->sliderbottom,_scwindow(obj),NULL,PGA_Total,inst->maxx - inst->minx + 1,
                                           PGA_Visible,get(inst->panel,SCCA_Area_InnerWidth),PGA_Top,inst->left - inst->minx,
                                           TAG_DONE);
        if (newy && inst->sliderright)
                SetGadgetAttrs(inst->sliderright,_scwindow(obj),NULL,PGA_Total,inst->maxy - inst->miny + 1,
                                           PGA_Visible,get(inst->panel,SCCA_Area_InnerHeight),PGA_Top,inst->top - inst->miny,
                                           TAG_DONE);

        SC_DoSuperMethodA(cl, obj, (Msg) msg);
}
// /

/** SetIDCMP
*/
static void PanelView_SetIDCMP(struct SC_Class *cl, Object *obj, struct SCCP_Area_SetIDCMP *msg, struct PanelViewInst *inst)
{
        msg->IDCMP |= inst->idcmp;
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
}
// /

/** Get
*/
static ULONG PanelView_Get(struct SC_Class *cl, Object *obj, struct opGet *msg, struct PanelViewInst *inst)
{
        if (msg->opg_AttrID == SCCA_PanelView_Left)
        {
                *(msg->opg_Storage) = (ULONG) inst->left;
                return( TRUE );
        }
        if (msg->opg_AttrID == SCCA_PanelView_Top)
        {
                *(msg->opg_Storage) = (ULONG) inst->top;
                return( TRUE );
        }
        return(SC_DoSuperMethodA(cl,obj,(Msg) msg));
}
// /

/** HandleInput
*/
static void PanelView_HandleInput(struct SC_Class *cl, Object *obj, struct SCCP_Area_HandleInput *msg, struct PanelViewInst *inst)
{
        ULONG gadid = ((struct Gadget *) msg->imsg->IAddress)->GadgetID;

        SC_DoSuperMethodA(cl,obj,(Msg) msg);

        switch (msg->imsg->Class)
        {
          case IDCMP_MOUSEMOVE :
                switch (inst->movegadid)
                {
                  case SLIDERGADID_BOTTOM :
                        inst->left = get((Object *) inst->sliderbottom,PGA_Top) + inst->minx;
                        SC_SetAttrs(inst->panel,SCCA_Panel_Left,inst->left,TAG_DONE);
                        break;
                  case SLIDERGADID_RIGHT :
                        inst->top = get((Object *) inst->sliderright,PGA_Top) + inst->miny;
                        SC_SetAttrs(inst->panel,SCCA_Panel_Top,inst->top,TAG_DONE);
                        break;
                }

          case IDCMP_GADGETDOWN :
                if ((gadid == SLIDERGADID_BOTTOM) || (gadid == SLIDERGADID_RIGHT))
                {
                        inst->movegadid = gadid;
                        inst->idcmp |= IDCMP_MOUSEMOVE;
                        SC_DoMethod(obj,SCCM_Area_SetIDCMP,0);
                }
                break;

          case IDCMP_GADGETUP :
                if ((gadid == SLIDERGADID_BOTTOM) || (gadid == SLIDERGADID_RIGHT))
                {
                        switch (inst->movegadid)
                        {
                          case SLIDERGADID_BOTTOM :
                                inst->left = get((Object *) inst->sliderbottom,PGA_Top) + inst->minx;
                                SC_SetAttrs(inst->panel,SCCA_Panel_Left,inst->left,TAG_DONE);
                                break;
                          case SLIDERGADID_RIGHT :
                                inst->top = get((Object *) inst->sliderright,PGA_Top) + inst->miny;
                                SC_SetAttrs(inst->panel,SCCA_Panel_Top,inst->top,TAG_DONE);
                                break;
                        }
                        inst->movegadid = 0;
                        inst->idcmp &= ~IDCMP_MOUSEMOVE;
                        SC_DoMethod(obj,SCCM_Area_SetIDCMP,0);
                }
                break;

        }
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData PanelViewMethods[] =
{
        { SCCM_Init,(ULONG) PanelView_Init, 0, 0, NULL },
        { SCCM_Area_Setup,(ULONG) PanelView_Setup, 0, 0, NULL },
        { SCCM_Area_Show,(ULONG) PanelView_Show, 0, 0, NULL },
        { SCCM_Area_Hide, (ULONG) PanelView_Hide, 0, 0, NULL },
        { OM_SET, (ULONG) PanelView_Set, 0, 0, NULL },
        { OM_GET, (ULONG) PanelView_Get, 0, 0, NULL },
        { SCCM_Area_SetIDCMP,(ULONG) PanelView_SetIDCMP, 0, 0, NULL },
        { SCCM_Area_HandleInput,(ULONG) PanelView_HandleInput, 0, 0, NULL },
        { SCCM_Area_DelayedShow,(ULONG) PanelView_DelayedShow, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};
