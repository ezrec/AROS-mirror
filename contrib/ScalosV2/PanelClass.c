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
#include <clib/utility_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <intuition/classusr.h>
#include <graphics/gfx.h>
#include <stdlib.h>

#include "Scalos.h"
#include "PanelClass.h"
#include "SubRoutines.h"

#include "scalos_protos.h"
#include "debug.h"

LONG    oldright;

/****** Panel.scalos/--background *******************************************
*
*   Panel class provides an interface for Lists. It's just a plane where the
*   list elements can be displayed. Any scrolling and clipping will be
*   handled by this class.
*
*****************************************************************************
*/
// /

/** Init
*/
static ULONG Panel_Init(struct SC_Class *cl, Object *obj, struct opSet *msg, struct PanelInst *inst)
{
        if (SC_DoSuperMethodA(cl, obj, (Msg) msg))
        {
                inst->backbitmap = (Object *) GetTagData(SCCA_Panel_Background, 0,msg->ops_AttrList);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Setup
*/
static ULONG Panel_Setup(struct SC_Class *cl, Object *obj, Msg msg, struct PanelInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj,msg))
        {
                if (inst->backbitmap)
                        if (!(SC_DoMethod(inst->backbitmap, SCCM_Graphic_PreThinkScreen, scRenderInfo(obj)->screenobj)))
                                return(FALSE);
                if (inst->clipregion = NewRegion())
                        return(TRUE);

                inst->oldleftpos = 0x8000;
        }
        return(FALSE);
}
// /

/** Show
*/
static void Panel_Show(struct SC_Class *cl, Object *obj, struct SCCP_Area_Show *msg, struct PanelInst *inst)
{
        struct Rectangle rect;

        SC_DoSuperMethodA(cl,obj,(Msg) msg);

        ClearRegion(inst->clipregion);
        rect.MinX = _scleft(obj);
        rect.MinY = _sctop(obj);
        rect.MaxX = _scright(obj);
        rect.MaxY = _scbottom(obj);
        OrRectRegion(inst->clipregion,&rect);

        if (msg->action == SCCV_Area_Action_Resize)
        {
                if ((_scleft(obj) == inst->oldleftpos) && (_sctop(obj) == inst->oldtoppos))
                {
                                // if ((_scright(obj) > inst->oldright) || (_scbottom(obj) > inst->oldbottom))
                        {
                                if (inst->refreshregion = NewRegion())
                                {
                                        OrRectRegion(inst->refreshregion,&rect);
                                        rect.MaxX = inst->oldright;
                                        rect.MaxY = inst->oldbottom;
                                        ClearRectRegion(inst->refreshregion,&rect);
                                }
                        }
                }
        }
        else
        {
                if (inst->backbitmap)
                        if (!(SC_DoMethod(inst->backbitmap, SCCM_Graphic_PreThinkWindow, scRenderInfo(obj)->rport, msg->action)))
                        {
                                SC_DoMethod(inst->backbitmap, SCCM_Graphic_PostThinkScreen);
                                SC_DisposeObject(inst->backbitmap);
                                inst->backbitmap = NULL;
                        }

        }

}
// /

/** Hide
*/
static void Panel_Hide(struct SC_Class *cl, Object *obj, struct SCCP_Area_Hide *msg, struct PanelInst *inst)
{
        if (msg->action != SCCV_Area_Action_Resize)
        {
                if (inst->backbitmap)
                        if (!(SC_DoMethod(inst->backbitmap, SCCM_Graphic_PostThinkWindow, msg->action)))
                        {
                                SC_DoMethod(inst->backbitmap, SCCM_Graphic_PostThinkScreen);
                                SC_DisposeObject(inst->backbitmap);
                                inst->backbitmap = NULL;
                        }
        }
        SC_DoSuperMethodA(cl, obj, (Msg) msg);
}
// /

/** Cleanup
*/
static void Panel_Cleanup(struct SC_Class *cl, Object *obj, Msg msg, struct PanelInst *inst)
{
        if(inst->backbitmap)
                SC_DoMethod(inst->backbitmap, SCCM_Graphic_PostThinkScreen);
        DisposeRegion(inst->clipregion);

        SC_DoSuperMethodA(cl, obj, msg);
}
// /

/** Exit
*/
static void Panel_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct PanelInst *inst)
{
        if(inst->backbitmap)
                SC_DisposeObject(inst->backbitmap); 

        SC_DoSuperMethodA(cl, obj, msg);
}
// /

/** AskMinMax
*/
static void Panel_AskMinMax(struct SC_Class *cl, Object *obj, struct SCCP_Area_AskMinMax *msg, struct PanelInst *inst)
{
        SC_DoSuperMethodA(cl,obj, (Msg) msg);
        msg->sizes->minwidth += 1;
        msg->sizes->minheight += 1;
        msg->sizes->maxwidth = SCCV_Area_MaxSize;
        msg->sizes->maxheight = SCCV_Area_MaxSize;
        msg->sizes->defwidth += 100;
        msg->sizes->defheight += 100;
}
// /

/** SetIDCMP
*/
static void Panel_SetIDCMP(struct SC_Class *cl, Object *obj, struct SCCP_Area_SetIDCMP *msg, struct PanelInst *inst)
{
        msg->IDCMP |= inst->idcmp;
        SC_DoSuperMethodA(cl, obj, (Msg) msg);
}
// /

/** BeginDraw
*/
static ULONG Panel_BeginDraw(struct SC_Class *cl, Object *obj, struct SCCP_Area_BeginDraw *msg, struct PanelInst *inst)
{
        if (SC_DoSuperMethodA(cl, obj, (Msg) msg))
        {
                SC_DoMethod(_scwinobj(obj),SCCM_Window_SetClipRegion,inst->clipregion);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** EndDraw
*/
static void Panel_EndDraw(struct SC_Class *cl, Object *obj, struct SCCP_Area_EndDraw *msg, struct PanelInst *inst)
{
        SC_DoMethod(_scwinobj(obj),SCCM_Window_ClearClipRegion);

        SC_DoSuperMethodA(cl, obj, (Msg) msg);
}
// /


/** Draw
*/
static void Panel_Draw(struct SC_Class *cl, Object *obj, struct SCCP_Area_Draw *msg, struct PanelInst *inst)
{
        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        DEBUG3("Old Size %ld,%ld : %ld\n",inst->oldright,inst->oldbottom,oldright);
        DEBUG2("New Size %ld,%ld\n",_scright(obj),_scbottom(obj));

        oldright = _scright(obj);
        if (inst->refreshregion)
        {
                SC_DoMethod(_scwinobj(obj),SCCM_Window_SetClipRegion,inst->refreshregion);

                if (inst->backbitmap)
                        DoMethod(inst->backbitmap,SCCM_Bitmap_Fill, _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj), inst->left, inst->top,SCCV_Bitmap_Fill_BlitType_Clipped);
                else
                        EraseRect(_scrport(obj), _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj));

                SC_DoMethod(obj,SCCM_Panel_Redraw,SCCV_Area_Drawtype_Complete);

                SC_DoMethod(_scwinobj(obj),SCCM_Window_ClearClipRegion);

                DisposeRegion(inst->refreshregion);

                inst->refreshregion = NULL;
        }
        else
        {

                SC_DoMethod(_scwinobj(obj),SCCM_Window_SetClipRegion,inst->clipregion);

                if (inst->backbitmap)
                        DoMethod(inst->backbitmap,SCCM_Bitmap_Fill, _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj), inst->left, inst->top,SCCV_Bitmap_Fill_BlitType_Clipped);
                else
                        EraseRect(_scrport(obj), _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj));

                SC_DoMethod(obj,SCCM_Panel_Redraw,SCCV_Area_Drawtype_Complete);

                SC_DoMethod(_scwinobj(obj),SCCM_Window_ClearClipRegion);
        }
        inst->oldleft = inst->left;
        inst->oldtop = inst->top;

        inst->oldleftpos = _scleft(obj);
        inst->oldtoppos = _sctop(obj);
        inst->oldright = _scright(obj);
        inst->oldbottom = _scbottom(obj);
}

void drawScroll(struct SC_Class *cl, Object *obj,struct PanelInst *inst)
{
        if (SC_DoMethod(_scwinobj(obj), SCCM_Window_BeginDraw,obj))
        {
                if ((inst->oldleft != inst->left) || (inst->oldtop != inst->top))
                {
                        struct Region *region;
                        struct Rectangle rect;
                        LONG diffx = inst->left - inst->oldleft;
                        LONG diffy = inst->top - inst->oldtop;

                        if (((_scwidth(obj)/2) < abs(diffx)) || ((_scheight(obj)/2) < abs(diffy)))
                        {
                                if (inst->backbitmap)
                                        DoMethod(inst->backbitmap,SCCM_Bitmap_Fill, _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj), inst->left, inst->top,SCCV_Bitmap_Fill_BlitType_Clipped);
                                else
                                        EraseRect(_scrport(obj), _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj));

                                SC_DoMethod(obj,SCCM_Panel_Redraw,SCCV_Area_Drawtype_Complete);
                        }
                        else
                        {
                                if (region = NewRegion())
                                {
                                        rect.MinX = _scleft(obj);
                                        rect.MinY = _sctop(obj);
                                        rect.MaxX = _scright(obj);
                                        rect.MaxY = _scbottom(obj);
                                        OrRectRegion(region, &rect);

                                        rect.MinX -= diffx;
                                        rect.MinY -= diffy;
                                        rect.MaxX -= diffx + 1;
                                        rect.MaxY -= diffy + 1;
                                        ClearRectRegion(region, &rect);
                                        
                                        ScrollWindowRaster(scRenderInfo(obj)->window, diffx, diffy,
                                                                           _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj));

                                        if (inst->backbitmap)
                                        {
                                                if (diffx < 0)
                                                        DoMethod(inst->backbitmap,SCCM_Bitmap_Fill, _scleft(obj), _sctop(obj), _scleft(obj) - diffx - 1, _scbottom(obj), inst->left, inst->top,SCCV_Bitmap_Fill_BlitType_Clipped);
                                                else
                                                        DoMethod(inst->backbitmap,SCCM_Bitmap_Fill, _scright(obj) - diffx, _sctop(obj), _scright(obj), _scbottom(obj), inst->left + _scwidth(obj) - diffx, inst->top, SCCV_Bitmap_Fill_BlitType_Clipped);

                                                if (diffy < 0)
                                                        DoMethod(inst->backbitmap,SCCM_Bitmap_Fill, _scleft(obj), _sctop(obj), _scright(obj), _sctop(obj) - diffy - 1, inst->left, inst->top,SCCV_Bitmap_Fill_BlitType_Clipped);
                                                else
                                                        DoMethod(inst->backbitmap,SCCM_Bitmap_Fill, _scleft(obj), _scbottom(obj) - diffy, _scright(obj), _scbottom(obj), inst->left, inst->top + _scheight(obj) - diffy, SCCV_Bitmap_Fill_BlitType_Clipped);
                                        }


                                        SC_DoMethod(_scwinobj(obj),SCCM_Window_SetClipRegion,region);
                                        SC_DoMethod(obj,SCCM_Panel_Redraw,SCCV_Area_Drawtype_Complete);
                                        SC_DoMethod(_scwinobj(obj),SCCM_Window_ClearClipRegion);
                                        DisposeRegion(region);

                                }
                        }
                }
                else
                {
                        SC_DoMethod(obj,SCCM_Panel_Redraw,SCCV_Area_Drawtype_Update);
                }

        }
        inst->oldleft = inst->left;
        inst->oldtop = inst->top;

        inst->oldleftpos = _scleft(obj);
        inst->oldtoppos = _sctop(obj);
        inst->oldright = _scright(obj);
        inst->oldbottom = _scbottom(obj);

        SC_DoMethod(_scwinobj(obj), SCCM_Window_EndDraw,obj);
}       
// /

/** Set
*/
static void Panel_Set(struct SC_Class *cl, Object *obj, struct opSet *msg, struct PanelInst *inst)
{
        struct TagItem *taglist = msg->ops_AttrList;
        struct TagItem **tags = &taglist;
        struct TagItem *tag;
        BOOL redraw = FALSE;

        while(tag = NextTagItem(tags))
        {
                switch (tag->ti_Tag)
                {
                        case SCCA_Panel_Left :
                                inst->left = tag->ti_Data;
                                redraw = TRUE;
                                break;
                        case SCCA_Panel_Top :
                                inst->top = tag->ti_Data;
                                redraw = TRUE;
                                break;
                }
        }
        if (redraw)
                drawScroll(cl,obj,inst);

        SC_DoSuperMethodA(cl, obj, (Msg) msg);
}
// /

/** Redraw
*/
static void Panel_Redraw(struct SC_Class *cl, Object *obj, Msg msg, struct PanelInst *inst)
{
}
// /

/** Get
*/
static ULONG Panel_Get(struct SC_Class *cl, Object *obj, struct opGet *msg, struct PanelInst *inst)
{
        if (msg->opg_AttrID == SCCA_Panel_Left)
        {
                *(msg->opg_Storage) = (LONG) inst->left;
                return( TRUE );
        }
        if (msg->opg_AttrID == SCCA_Panel_Top)
        {
                *(msg->opg_Storage) = (LONG) inst->top;
                return( TRUE );
        }
        if (msg->opg_AttrID == SCCA_Area_StaticContents)
        {
                *(msg->opg_Storage) = FALSE;
                return( TRUE );
        }
        return(SC_DoSuperMethodA(cl,obj,(Msg) msg));
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData PanelMethods[] =
{
        { SCCM_Panel_Redraw,    (ULONG) Panel_Redraw,       sizeof(ULONG),  0, NULL},
        { SCCM_Init,            (ULONG) Panel_Init,         0,              0, NULL },
        { SCCM_Exit,            (ULONG) Panel_Exit,         0,              0, NULL },
        { SCCM_Area_Draw,       (ULONG) Panel_Draw,         0,              0, NULL },
        { SCCM_Area_BeginDraw,  (ULONG) Panel_BeginDraw,    0,              0, NULL },
        { SCCM_Area_EndDraw,    (ULONG) Panel_EndDraw,      0,              0, NULL },
        { SCCM_Area_Setup,      (ULONG) Panel_Setup,        0,              0, NULL},
        { SCCM_Area_Cleanup,    (ULONG) Panel_Cleanup,      0,              0, NULL },
        { SCCM_Area_Show,       (ULONG) Panel_Show,         0,              0, NULL },
        { SCCM_Area_Hide,       (ULONG) Panel_Hide,         0,              0, NULL },
        { SCCM_Area_AskMinMax,  (ULONG) Panel_AskMinMax,    0,              0, NULL },
        { SCCM_Area_SetIDCMP,   (ULONG) Panel_SetIDCMP,     0,              0, NULL },
        { OM_SET,               (ULONG) Panel_Set,          0,              0, NULL },
        { OM_GET,               (ULONG) Panel_Get,          0,              0, NULL },
        { SCMETHOD_DONE,        NULL,                       0,              0, NULL }
};
