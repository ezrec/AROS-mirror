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
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/alib.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include "Scalos.h"
#include "AreaClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"

/****** Area.scalos/--background ********************************************
*
*   Area class is the superclass of all GUI-classes (e.g. Gadgets). It
*   provides a standard set of methods for Init/Exit and input handling.
*
*****************************************************************************
*/
// /

/****** Area.scalos/SCCM_Area_Setup *****************************************
*
*  NAME
*   SCCM_Area_Setup
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Area_Setup,struct SC_RenderInfo *rinfo);
*
*  FUNCTION
*   This method will be called if some lowlevel infos of the SC_RenderInfo
*   structure are filled. Currently this is only the screenobj field. Here
*   you should prepare anything which depends on specific screen. (e.g. Pens).
*   The windowobject calles this method right before the window will be
*   opened.
*
*  INPUTS
*   rinfo - pointer to struct SC_RenderInfo. Normaly you should ignore it
*           and use the _scRenderInfo macro instead.
*
*  RESULT
*   TRUE if SCCM_Area_Setup was successfull.
*
*  NOTES
*   If SCCM_Area_Setup was successfull, you will receive a matching
*   SCCM_Area_Cleanup sometime.
*
*  SEE ALSO
*   SCCM_Area_Cleanup
*
*****************************************************************************
*/                                                              

static ULONG Area_Setup(struct SC_Class *cl, struct SC_AreaObject *obj, struct SCCP_Area_Setup *msg, struct AreaInst *inst)
{
        // Set the global RenderInfo, so that every object has a pointer to the struct SC_RenderInfo
        inst->rinfo = msg->rinfo;
        
        return(TRUE);
}

// /

/****** Area.scalos/SCCM_Area_Cleanup ***************************************
*
*  NAME
*   SCCM_Area_Cleanup
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Area_Cleanup);
*
*  FUNCTION
*   Your object will receive this method if the window was closed. You should
*   now free all resources depending on the screen. Receiving this method
*   does not mean SCCM_Area_Setup will never be called again.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*   This method will only be sent by the window object if SCCM_Area_Setup
*   was successfull.
*   But the method could also be called automatically if SCCM_Area_Setup
*   does a CoerceMethod() to this method in order to clean up on failure.
*
*   Note that CoerceMethod() has a new name ( SC_DoClassMethod ) and a new
*   caling sheme with scalos.
*
*  SEE ALSO
*   SCCM_Area_Setup
*
*****************************************************************************
*/                                                              
// /

/****** Area.scalos/SCCM_Area_Draw ******************************************
*
*  NAME
*   SCCM_Area_Draw
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Area_Draw);
*
*  FUNCTION
*   Now you have to draw your object. You should avoid any renderings in 
*   this method. You should render in SCCM_Show.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*   SCCM_Area_Setup, SCCM_Area_Show, SCCM_Area_Hide, SCCM_Area_Cleanup
*
*****************************************************************************
*/                                                              
// /

/****** Area.scalos/SCCM_Area_Show ******************************************
*
*  NAME
*   SCCM_Area_Show
*
*  SYNOPSIS
*   SC_DoMethod(obj, SCCM_Area_Show, ULONG action, struct IBox *bounds);
*
*  FUNCTION
*   After the window was opened your object will receive this method. The
*   size information and the whole SC_RenderInfo structure are filled out.
*   Size renderings and anything else to prepare drawing should be done here.
*
*  INPUTS
*   action - SCCV_Area_Action_OpenClose - SCCM_Area_Show was sent because of window-
*                                  open.
*            SCCV_Area_Action_Resize - SCCM_Area_Show was sent because of window-
*                               resize.
*   bounds - size and position of this object. Don't touch this, use the
*            bounds from struct ScalosObject instead.
*
*  RESULT
*
*  NOTES
*   You should only check the action parameter if you want to use it to
*   speedup your class.
*
*  SEE ALSO
*   SCCM_Area_Setup, SCCM_Area_Draw, SCCM_Area_Hide, SCCM_Area_Cleanup
*
*****************************************************************************
*/                                                              

static void Area_Show(struct SC_Class *cl, struct SC_AreaObject *obj, struct SCCP_Area_Show *msg, struct AreaInst *inst)
{
        inst->bounds = *msg->bounds; // copy complete struct IBox
}
// /

/****** Area.scalos/SCCM_Area_Hide ******************************************
*
*  NAME
*   SCCM_Area_Hide
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Area_Hide,ULONG action);
*
*  FUNCTION
*   This method will be sent after window close or resize. Any allocations
*   done in SCCM_Area_Show should be freed here.
*
*  INPUTS
*   action - SCCV_Area_Action_OpenClose - SCCM_Area_Show was sent because of window-
*                                  close.
*            SCCV_Area_Action_Resize - SCCM_Area_Show was sent because of window-
*                               resize.
*
*  RESULT
*
*  NOTES
*   You should only check the action parameter if you need it to speedup your
*   class.
*
*  SEE ALSO
*   SCCM_Area_Setup, SCCM_Area_Show, SCCM_Area_Draw, SCCM_Area_Cleanup
*
*****************************************************************************
*/                                                              
// /

/****** Area.scalos/SCCM_Area_AskMinMax *************************************
*
*  NAME
*   SCCM_Area_AskMinMax
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Area_AskMinMax,struct SC_MinMax *sizes);
*
*  FUNCTION
*   This method is used to find out minimum, maximum and default sizes of an
*   object.
*   Call your superclass first and then *ADD* your sizes to the MinMax struct.
*   If you want no maximum limit, just use SCCV_Area_MaxSize.
*
*  INPUTS
*   sizes - sizes to fill out
*
*  RESULT
*
*  NOTES
*   This method will be sent after SCCM_Area_Setup, but before
*   SCCM_Area_Show.
*
*  SEE ALSO
*
*****************************************************************************
*/                                                              

static void Area_AskMinMax(struct SC_Class *cl, struct SC_AreaObject *obj, struct SCCP_Area_AskMinMax *msg, struct AreaInst *inst)
{
        msg->sizes->minwidth = 0;
        msg->sizes->minheight = 0;
        msg->sizes->maxwidth = 0;
        msg->sizes->maxheight = 0;
        msg->sizes->defwidth = 0;
        msg->sizes->defheight = 0;
}
// /

/****** Area.scalos/SCCM_Area_SetIDCMP **************************************
*
*  NAME
*   SCCM_Area_SetIDCMP
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Area_SetDCMP, ULONG idcmp);
*
*  FUNCTION
*   Using this method the object can request or reject IDCMPs. Normally you
*   send this method to your object. If you receive this method you should
*   bitwise OR your IDCMPs to the idcmp-value in the Msg. This method will
*   only be successfull after calling the superclass in SCCM_Area_Setup.
*
*  INPUTS
*   idcmp - IDCMP to set or change
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/                                                              

static void Area_SetIDCMP(struct SC_Class *cl, struct SC_AreaObject *obj, struct SCCP_Area_SetIDCMP *msg, struct AreaInst *inst)
{
        if (scRenderInfo(obj) && scRenderInfo(obj)->windowobj)
                SC_DoMethod(scRenderInfo(obj)->windowobj, SCCM_Window_SetIDCMP, msg->IDCMP, obj);
}
// /

/****** Area.scalos/SCCM_Area_BeginDraw *************************************
*
*  NAME
*   SCCM_Area_BeginDraw
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Area_BeginDraw, Object *object);
*
*  FUNCTION
*   There are to ways to draw into the window. One within the SCCM_Area_Draw
*   and one between SCCM_Area_BeginDraw and SCCM_Area_EndDraw. SCCM_Area_Draw
*   is sent to all object to refresh it. SCCM_AreaBeginDraw can be used to
*   draw into the window from another method.
*   This method will initializes any for drawing into the window, which means
*   that ClipRegions and coordinate transformations will be done. After the
*   drawing is done SCCM_Area_EndDraw must be called.
*   Only do anything after receiving this method if the object arg points to
*   same object as yours, if not return FALSE immediately. If your object
*   has some child object, send this method to all your childs. To optimize
*   that a bid you could parse your list of childs first to find the object
*   first and send to method only to it. If you could find it, you need to
*   send it to all childs because the childs could have childs too.
*   
*  INPUTS
*   object - the object where the draw should be initializes
*
*  RESULT
*   TRUE if the begin was successfully, else FALSE. Any drawing is only
*   allowed if TRUE was returned from your super class.
*
*  NOTES
*
*  SEE ALSO
*   Window.scalos/SCCM_Window_BeginDraw, SCCM_Area_EndDraw
*
*****************************************************************************
*/                                                              

static ULONG Area_BeginDraw(struct SC_Class *cl, struct SC_AreaObject *obj, struct SCCP_Area_BeginDraw *msg, struct AreaInst *inst)
{
        if (msg->object == (Object *) obj)
        {
                return(TRUE);
        }
        return(FALSE);
}
// /

/****** Area.scalos/SCCM_Area_EndDraw ***************************************
*
*  NAME
*   SCCM_Area_EndDraw
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Area_EndDraw, Object *object);
*
*  FUNCTION
*   This method finishes all drawing. For more info check
*   SCCM_Area_BeginDraw.
*
*  INPUTS
*   object - the object where the draw should be finished
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*   Window.scalos/SCCM_Window_EndDraw, SCCM_Area_BeginDraw
*
*****************************************************************************
*/
// /

/****** Area.scalos/SCCM_Area_DelayedShow ***********************************
*
*  NAME
*   SCCM_Area_DelayedShow
*
*  SYNOPSIS
*   SC_DoMethod(obj, SCCM_Area_DelayedShow);
*
*  FUNCTION
*   This is a second show method during that you may do things that can`t be
*   done when LockLayers() and related functions are active. You can request
*   this method if you need it from window class using
*   SCCM_Window_DelayedShowObject. Normally you don`t need to.
*   If you requested this method the normal show method will be sent too.
*   For a window resize it will be sent after the draw method.
*
*   Program flow:
*
*   Hide
*   Show
*   Draw
*   UnlockLayer()
*   DelayedShow
*   
*  INPUTS
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*   
*
*****************************************************************************
*/                                                              
// /

/** Area Get
*/
static ULONG Area_Get(struct SC_Class *cl, struct SC_AreaObject *obj, struct opGet *msg, struct AreaInst *inst)
{
        if (msg->opg_AttrID == SCCA_Area_InnerWidth)
        {
                *(msg->opg_Storage) = (ULONG) inst->bounds.Width;
                return( TRUE );
        }
        if (msg->opg_AttrID == SCCA_Area_InnerHeight)
        {
                *(msg->opg_Storage) = (ULONG) inst->bounds.Height;
                return( TRUE );
        }
        return(SC_DoSuperMethodA(cl,(Object *) obj,(Msg) msg));
}
// /

/** ------------------------- MethodList ------------------------------ */

struct SC_MethodData AreaMethods[] =
{
        { SCCM_Area_Draw,           NULL,                       sizeof(ULONG),                          SCMDF_DIRECTMETHOD, NULL },
        { SCCM_Area_Setup,          (ULONG) Area_Setup,         sizeof(struct SCCP_Area_Setup),         0,                  NULL },
        { SCCM_Area_Cleanup,        NULL,                       sizeof(ULONG),                          0,                  NULL },
        { SCCM_Area_Show,           (ULONG) Area_Show,          sizeof(struct SCCP_Area_Show),          0,                  NULL },
        { SCCM_Area_Hide,           NULL,                       sizeof(struct SCCP_Area_Hide),          0,                  NULL },
        { SCCM_Area_AskMinMax,      (ULONG) Area_AskMinMax,     sizeof(struct SCCP_Area_AskMinMax),     0,                  NULL },
        { SCCM_Area_SetIDCMP,       (ULONG) Area_SetIDCMP,      sizeof(struct SCCP_Area_SetIDCMP),      0,                  NULL },
        { SCCM_Area_BeginDraw,      (ULONG) Area_BeginDraw,     sizeof(struct SCCP_Area_BeginDraw),     SCMDF_DIRECTMETHOD, NULL },
        { SCCM_Area_EndDraw,        NULL,                       sizeof(struct SCCP_Area_EndDraw),       SCMDF_DIRECTMETHOD, NULL },
        { OM_GET,                   (ULONG) Area_Get,           0,                                      0,                  NULL },
        { SCCM_Area_DelayedShow,    NULL,                       sizeof(ULONG),                          0,                  NULL },
        { SCMETHOD_DONE,            NULL,                       0,                                      0,                  NULL }
};
// /

