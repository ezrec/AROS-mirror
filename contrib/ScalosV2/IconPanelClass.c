
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
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>

#include "Scalos.h"
#include "IconPanelClass.h"
#include "SubRoutines.h"

#include "scalos_protos.h"
#include "Debug.h"

/****** IconPanel.scalos/--background ***************************************
*
*   IconPanel class is the list of icon objects.
*
*****************************************************************************
*/
// /

/** settextpos subroutine
*/
void settextpos(Object *icon, Object *text)
{
        int x,y;

        if (get(icon,SCCA_Icon_NoPosition))
        {
                        // render new position
                SC_SetAttrs(icon,SCCA_Graphic_Left,20,SCCA_Graphic_Top,20,TAG_DONE);
        }

        y = get(icon,SCCA_Graphic_Height) + 1 + get(icon,SCCA_Graphic_Top);
        x = (get(icon,SCCA_Graphic_Width) - get(text,SCCA_Graphic_Width))/2 + get(icon,SCCA_Graphic_Left);

        SC_SetAttrs(text,SCCA_Graphic_Left,x,SCCA_Graphic_Top,y,TAG_DONE);
}
// /

/** Init
*/
static ULONG IconPanel_Init(struct SC_Class *cl, Object *obj, struct opSet *msg, struct IconPanelInst *inst)
{
        if (SC_DoSuperMethodA(cl, obj, (Msg) msg))
        {
                NewList((struct List *) &inst->iconlist);
                NewList((struct List *) &inst->newiconlist);
                InitSemaphore(&inst->iconsem);
                InitSemaphore(&inst->newiconsem);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Setup
*/
static ULONG IconPanel_Setup(struct SC_Class *cl, Object *obj, Msg msg, struct IconPanelInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj,msg))
        {
                struct IconNode *node;

                SC_SetAttrs(obj,SCCA_Panel_MinX,0,SCCA_Panel_MaxX,300,SCCA_Panel_MinY,0,SCCA_Panel_MaxY,200,TAG_DONE);

                ObtainSemaphore(&inst->iconsem);

                for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
                {
                        SC_DoMethod(node->icon,SCCM_Graphic_PreThinkScreen,scRenderInfo(obj)->screenobj);
                        SC_DoMethod(node->text,SCCM_Graphic_PreThinkScreen,scRenderInfo(obj)->screenobj);
                }

                ReleaseSemaphore(&inst->iconsem);


                ObtainSemaphore(&inst->newiconsem);

                for (node = (struct IconNode *) inst->newiconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
                {
                        SC_DoMethod(node->icon,SCCM_Graphic_PreThinkScreen,scRenderInfo(obj)->screenobj);
                        SC_DoMethod(node->text,SCCM_Graphic_PreThinkScreen,scRenderInfo(obj)->screenobj);
                }

                ReleaseSemaphore(&inst->newiconsem);

                inst->idcmp = IDCMP_MOUSEBUTTONS | IDCMP_MENUVERIFY;
                SC_DoMethod(obj,SCCM_Area_SetIDCMP,0);

                return(TRUE);
        }
        return(FALSE);
}
// /

/** Show
*/
static void IconPanel_Show(struct SC_Class *cl, Object *obj, struct SCCP_Area_Show *msg, struct IconPanelInst *inst)
{
        struct IconNode *node;

        SC_DoSuperMethodA(cl,obj,(Msg) msg);

        ObtainSemaphore(&inst->iconsem);

        for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                SC_DoMethod(node->icon,SCCM_Graphic_PreThinkWindow,_scrport(obj),SCCV_Area_Action_OpenClose);
                SC_DoMethod(node->text,SCCM_Graphic_PreThinkWindow,_scrport(obj),SCCV_Area_Action_OpenClose);
                settextpos(node->icon,node->text);
        }

        ReleaseSemaphore(&inst->iconsem);


        ObtainSemaphore(&inst->newiconsem);

        for (node = (struct IconNode *) inst->newiconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                SC_DoMethod(node->icon,SCCM_Graphic_PreThinkWindow,_scrport(obj),SCCV_Area_Action_OpenClose);
                SC_DoMethod(node->text,SCCM_Graphic_PreThinkWindow,_scrport(obj),SCCV_Area_Action_OpenClose);
                settextpos(node->icon,node->text);
        }

        ReleaseSemaphore(&inst->newiconsem);
}
// /

/** Hide
*/
static void IconPanel_Hide(struct SC_Class *cl, Object *obj, struct SCCP_Area_Hide *msg, struct IconPanelInst *inst)
{
        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        if (msg->action == SCCV_Area_Action_OpenClose)
        {
                struct IconNode *node;

                ObtainSemaphore(&inst->iconsem);

                for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
                {
                        SC_DoMethod(node->icon,SCCM_Graphic_PostThinkWindow,SCCV_Area_Action_OpenClose);
                        SC_DoMethod(node->text,SCCM_Graphic_PostThinkWindow,SCCV_Area_Action_OpenClose);
                }

                ReleaseSemaphore(&inst->iconsem);


                ObtainSemaphore(&inst->newiconsem);

                for (node = (struct IconNode *) inst->newiconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
                {
                        SC_DoMethod(node->icon,SCCM_Graphic_PostThinkWindow,SCCV_Area_Action_OpenClose);
                        SC_DoMethod(node->text,SCCM_Graphic_PostThinkWindow,SCCV_Area_Action_OpenClose);
                }

                ReleaseSemaphore(&inst->newiconsem);
        }
}
// /

/** Cleanup
*/
static void IconPanel_Cleanup(struct SC_Class *cl, Object *obj, Msg msg, struct IconPanelInst *inst)
{
        struct IconNode *node;

        SC_DoSuperMethodA(cl, obj, msg);

        ObtainSemaphore(&inst->iconsem);

        for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                SC_DoMethod(node->icon,SCCM_Graphic_PostThinkScreen);
                SC_DoMethod(node->text,SCCM_Graphic_PostThinkScreen);
        }

        ReleaseSemaphore(&inst->iconsem);


        ObtainSemaphore(&inst->newiconsem);

        for (node = (struct IconNode *) inst->newiconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                SC_DoMethod(node->icon,SCCM_Graphic_PostThinkScreen);
                SC_DoMethod(node->text,SCCM_Graphic_PostThinkScreen);
        }

        ReleaseSemaphore(&inst->newiconsem);
}
// /

/** Exit
*/
static void IconPanel_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct IconPanelInst *inst)
{
        struct IconNode *node;

        ObtainSemaphore(&inst->iconsem);

        for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                SC_DisposeObject(node->icon);
                SC_DisposeObject(node->text);
        }
        FreeAllNodes(&inst->iconlist);

        ReleaseSemaphore(&inst->iconsem);


        ObtainSemaphore(&inst->newiconsem);

        for (node = (struct IconNode *) inst->newiconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                SC_DisposeObject(node->icon);
                SC_DisposeObject(node->text);
        }
        FreeAllNodes(&inst->newiconlist);

        ReleaseSemaphore(&inst->newiconsem);


        SC_DoSuperMethodA(cl, obj, msg);
}
// /

/*
static void Panel_Set(struct SC_Class *cl, Object *obj, struct opSet *msg, struct PanelInst *inst)
{
        struct TagItem **tags;
        struct TagItem *tag;
        BOOL redraw = FALSE;

        tags = &msg->ops_AttrList;
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
                SC_DoMethod(_scwinobj(obj), SCCM_Window_Redraw, obj, SCCV_Area_Drawtype_Update);

        SC_DoSuperMethodA(cl, obj, (Msg) msg);
}
*/

/** Redraw
*/
static void IconPanel_Redraw(struct SC_Class *cl, Object *obj, struct SCCP_Panel_Redraw *msg, struct IconPanelInst *inst)
{
        LONG x = _scleft(obj) - get(obj,SCCA_Panel_Left);
        LONG y = _sctop(obj) - get(obj,SCCA_Panel_Top);
        struct IconNode *node;

        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        ObtainSemaphoreShared(&inst->iconsem);

        for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                SC_DoMethod(node->icon,SCCM_Graphic_Draw,x,y,SCCV_Graphic_DrawFlags_RelPos);
                SC_DoMethod(node->text,SCCM_Graphic_Draw,x,y,SCCV_Graphic_DrawFlags_RelPos);
        }

        ReleaseSemaphore(&inst->iconsem);
}
// /

/****** IconPanel.scalos/SCCM_IconPanel_Add *********************************
*
*  NAME
*   SCCM_IconPanel_Add
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_IconPanel_Add, Object *iconobj);
*
*  FUNCTION
*   This method will add an icon to the panel. Any information to show the
*   icon on screen will be taken from the icon object itself.
*
*  INPUTS
*   iconobj - an Icon.scalos object with the image which will be displayed
*
*  RESULT
*   The itemid which was added or NULL if it fails.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG IconPanel_Add(struct SC_Class *cl, Object *obj, struct SCCP_IconPanel_Add *msg, struct IconPanelInst *inst)
{
        struct IconNode *node;
        ULONG ret = 0;

        ObtainSemaphore(&inst->iconsem);

        if ((node = AllocNode(&inst->iconlist, sizeof(struct IconNode))))
        {
                        // give the icon a unique id
                if ((node->itemid = ++inst->itemid))
                        node->itemid =   inst->itemid;

                node->icon = msg->iconobj;
                node->text = SC_NewObject(NULL,SCC_TEXT_NAME,SCCA_Text_String,get(node->icon,SCCA_Icon_Name),
                                                                  TAG_DONE);

                ret = node->itemid;

                if (scRenderInfo(obj) && scRenderInfo(obj)->screenobj)
                {
                                // Setup was done
                        SC_DoMethod(node->icon,SCCM_Graphic_PreThinkScreen,scRenderInfo(obj)->screenobj);
                        SC_DoMethod(node->text,SCCM_Graphic_PreThinkScreen,scRenderInfo(obj)->screenobj);

                        if (_scrport(obj))
                                        // Show was done
                        {
                                SC_DoMethod(node->icon,SCCM_Graphic_PreThinkWindow,_scrport(obj),SCCV_Area_Action_OpenClose);
                                SC_DoMethod(node->text,SCCM_Graphic_PreThinkWindow,_scrport(obj),SCCV_Area_Action_OpenClose);

                                settextpos(node->icon,node->text);

                                if (SC_DoMethod(_scwinobj(obj), SCCM_Window_BeginDraw, obj))
                                {
                                        LONG x = _scleft(obj) - get(obj,SCCA_Panel_Left);
                                        LONG y = _sctop(obj) - get(obj,SCCA_Panel_Top);

                                        SC_DoMethod(node->icon,SCCM_Graphic_Draw,x,y,SCCV_Graphic_DrawFlags_RelPos);
                                        SC_DoMethod(node->text,SCCM_Graphic_Draw,x,y,SCCV_Graphic_DrawFlags_RelPos);
                                        SC_DoMethod(_scwinobj(obj), SCCM_Window_EndDraw, obj);
                                }
                        }
                }
        }
        ReleaseSemaphore(&inst->iconsem);
        return(ret);
}
// /

/** find item (if a id was given this call must be protected by a semaphore of the specific list
*/
static struct IconNode *finditem(struct MinList *list, ULONG itemid)
{
        struct IconNode *node;

        for (node = (struct IconNode *) list->mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                if (node->itemid == itemid)
                        return(node);
        }
        return(NULL);
}
// /

/****** IconPanel.scalos/SCCM_IconPanel_Remove ******************************
*
*  NAME
*   SCCM_IconPanel_Remove
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_IconPanel_Remove, ULONG itemid);
*
*  FUNCTION
*   This method removes a entry from the list. To get the item
*   information use SCCM_IconPanel_GetItem. The icon will removed from screen
*   too.
*
*  INPUTS
*   itemid - which stands for a specific item on the list.
*
*  RESULT
*   TRUE if the icon was successfully removed.
*   FALSE if the item wasn't found on the list or any other error happens.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG IconPanel_Remove(struct SC_Class *cl, Object *obj, struct SCCP_IconPanel_Remove *msg, struct IconPanelInst *inst)
{
        struct IconNode *node;

        ObtainSemaphore(&inst->iconsem);
        if ((node = finditem(&inst->iconlist,msg->itemid)))
        {
                Remove((struct Node *) node);

                if (scRenderInfo(obj) && scRenderInfo(obj)->screenobj)
                {
                        if (_scrport(obj))
                        {
                                LONG x = _scleft(obj) - get(obj,SCCA_Panel_Left);
                                LONG y = _sctop(obj) - get(obj,SCCA_Panel_Top);

                                if (SC_DoMethod(_scwinobj(obj),SCCM_Window_BeginDraw,obj))
                                {
                                        SC_DoMethod(node->icon,SCCM_Graphic_Erase,x,y,SCCV_Graphic_DrawFlags_RelPos);
                                        SC_DoMethod(node->text,SCCM_Graphic_Erase,x,y,SCCV_Graphic_DrawFlags_RelPos);
                                        SC_DoMethod(_scwinobj(obj),SCCM_Window_EndDraw,obj);
                                }
                                SC_DoMethod(node->icon,SCCM_Graphic_PostThinkWindow);
                                SC_DoMethod(node->text,SCCM_Graphic_PostThinkWindow);
                        }

                        SC_DoMethod(node->icon,SCCM_Graphic_PostThinkScreen);
                        SC_DoMethod(node->text,SCCM_Graphic_PostThinkScreen);
                }
                SC_DisposeObject(node->icon);
                SC_DisposeObject(node->text);
                FreeNode((struct MinNode *) node);
                ReleaseSemaphore(&inst->iconsem);
                return(TRUE);
        }
        ReleaseSemaphore(&inst->iconsem);

        ObtainSemaphore(&inst->newiconsem);
        if ((node = finditem(&inst->newiconlist,msg->itemid)))
        {
                SC_DisposeObject(node->icon);
                SC_DisposeObject(node->text);
                FreeNode((struct MinNode *) node);
                ReleaseSemaphore(&inst->newiconsem);
                return(TRUE);
        }
        ReleaseSemaphore(&inst->newiconsem);

        return(FALSE);
}
// /

/****** IconPanel.scalos/SCCM_IconPanel_GetEntry ****************************
*
*  NAME
*   SCCM_IconPanel_GetEntry
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_IconPanel_GetEntry, ULONG which, APTR itemid );
*
*  FUNCTION
*   This method will return an item from the list. Which one can be selected
*   by the which argument.
*
*  INPUTS
*   which - one of these values:
*               SCCV_IconPanel_First - first entry
*               SCCV_IconPanel_FirstSelected - first selected entry
*               SCCV_IconPanel_Next - next entry relative to item
*               SCCV_IconPanel_NextSelected - next selected entry relative
*                                             to item
*               SCCV_IconPanel_Previous - previous entry relative to item
*               SCCV_IconPanel_PreviousSelected - next selected entry
*                                                 relative to item
*               SCCV_IconPanel_Active - active entry
*               SCCV_IconPanel_Last - last entry
*               SCCV_IconPanel_LastSelected - last selected entry
*   itemid - an item where next or previous item is relative to or NULL
*
*  RESULT
*   The result will the item or NULL if an error occured.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG IconPanel_GetEntry(struct SC_Class *cl, Object *obj, struct SCCP_IconPanel_GetEntry *msg, struct IconPanelInst *inst)
{
        struct IconNode *node;
        ULONG ret = 0;

        ObtainSemaphoreShared(&inst->iconsem);

        if (IsListEmpty((struct List *) &inst->iconlist))
                return(0);

        if (!(msg->itemid))
                node = (struct IconNode *) inst->iconlist.mlh_Head;
        else
        {
                if (!(node = finditem(&inst->iconlist,msg->itemid)))
                {
                        ReleaseSemaphore(&inst->iconsem);
                        return(0);
                }
        }

        switch (msg->which)
        {

          case SCCV_IconPanel_First :
                ret = ((struct IconNode *) &inst->iconlist.mlh_Head)->itemid;
                break;
 
          case SCCV_IconPanel_Last :
                ret = ((struct IconNode *) &inst->iconlist.mlh_Tail)->itemid;
                break;

          case SCCV_IconPanel_Next :
                if (node && ((node = (struct IconNode *) node->node.mln_Succ)->node.mln_Succ))
                        ret = node->itemid;
                else
                        ret = 0;
                break;

          case SCCV_IconPanel_Previous :
                if (node && ((node = (struct IconNode *) node->node.mln_Pred)->node.mln_Pred))
                        ret = node->itemid;
                else
                        ret = 0;
                break;

          case SCCV_IconPanel_FirstSelected :
                node = (struct IconNode *) inst->iconlist.mlh_Head;

          case SCCV_IconPanel_NextSelected :
                for (;node->node.mln_Succ; node = (struct IconNode *) (node->node.mln_Succ))
                {
                        if (get(node->icon,SCCA_Icon_Selected) == TRUE)
                        {
                                ret = node->itemid;
                                break;
                        }
                }
                break;

          case SCCV_IconPanel_LastSelected :
                node = (struct IconNode *) inst->iconlist.mlh_Tail;

          case SCCV_IconPanel_PreviousSelected :
                for (;node->node.mln_Pred; node = (struct IconNode *) (node->node.mln_Pred))
                {
                        if (get(node->icon,SCCA_Icon_Selected) == TRUE)
                        {
                                ret = node->itemid;
                                break;
                        }
                }
                break;

          case SCCV_IconPanel_Active :
                if (inst->activenode)
                        ret = inst->activenode->itemid;
                break;

        }

        ReleaseSemaphore(&inst->iconsem);
        return(ret);
}
// /

/****** IconPanel.scalos/SCCM_IconPanel_GetEntryPos *************************
*
*  NAME
*   SCCM_IconPanel_GetEntryPos
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_IconPanel_GetEntryPos, LONG x, LONG y);
*
*  FUNCTION
*   This method can be used to get an entry which is under a specific
*   position.
*
*  INPUTS
*   x,y - coordinates which are relative to the windows position. e.g.
*         mouse coords from IDCMP_MouseMove values.
*
*  RESULT
*   The result will the item or NULL if an error occured.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG IconPanel_GetEntryPos(struct SC_Class *cl, Object *obj, struct SCCP_IconPanel_GetEntryPos *msg, struct IconPanelInst *inst)
{
        LONG x = _scleft(obj) - get(obj,SCCA_Panel_Left);
        LONG y = _sctop(obj) - get(obj,SCCA_Panel_Top);
        LONG iconx,icony;
        struct IconNode *node;
        ULONG ret = 0;

        if ((_scleft(obj) <= msg->x) && (_sctop(obj) <= msg->y) && (_scright(obj) >= msg->x) && (_scbottom(obj) >= msg->y))
        {
                ObtainSemaphoreShared(&inst->iconsem);
                for (node = (struct IconNode *) inst->iconlist.mlh_Tail;node->node.mln_Pred; node = (struct IconNode *) (node->node.mln_Pred))
                {
                        iconx = x + get(node->icon,SCCA_Graphic_Left);
                        icony = y + get(node->icon,SCCA_Graphic_Top);

                        if ((iconx <= msg->x) && (icony <= msg->y))
                        {
                                iconx += get(node->icon,SCCA_Graphic_Width);
                                icony += get(node->icon,SCCA_Graphic_Height);
                                if ((iconx > msg->x) && (icony > msg->y))
                                {
                                        ret = node->itemid;
                                        break;
                                }
                        }
                }
                ReleaseSemaphore(&inst->iconsem);
        }
        return(ret);
}
// /

/****** IconPanel.scalos/SCCM_IconPanel_Activate ****************************
*
*  NAME
*   SCCM_IconPanel_Activate
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_IconPanel_Activate, ULONG itemid, ULONG flags);
*
*  FUNCTION
*   This method can be used to get an entry which is under a specific
*   position.
*
*  INPUTS
*   x,y - coordinates which are relative to the windows position. e.g.
*         mouse coords from IDCMP_MouseMove values.
*
*  RESULT
*   The result will the item or NULL if an error occured.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG IconPanel_Activate(struct SC_Class *cl, Object *obj, struct SCCP_IconPanel_Activate *msg, struct IconPanelInst *inst)
{
        return(0);
}
// /

/** SetIDCMP
*/
static void IconPanel_SetIDCMP(struct SC_Class *cl, Object *obj, struct SCCP_Area_SetIDCMP *msg, struct IconPanelInst *inst)
{
        msg->IDCMP |= inst->idcmp;
        SC_DoSuperMethodA(cl,obj,(Msg) msg);
}
// /

/** HandleInput
*/
static void IconPanel_HandleInput(struct SC_Class *cl, Object *obj, struct SCCP_Area_HandleInput *msg, struct IconPanelInst *inst)
{
        LONG offsetx = get(obj,SCCA_Panel_Left);
        LONG offsety = get(obj,SCCA_Panel_Top);
        int mousex, mousey;
        struct IconNode *node;

        SC_DoSuperMethodA(cl,obj,(Msg) msg);

        switch (msg->imsg->Class)
        {
          case IDCMP_MOUSEBUTTONS :
                mousex = msg->imsg->MouseX - _scleft(obj);
                mousey = msg->imsg->MouseY - _sctop(obj);

                if ((mousex >= 0) && (mousex < _scwidth(obj)) && (mousey >= 0) && (mousey < _scheight(obj)))
                {
                        mousex += offsetx;
                        mousey += offsety;
                        if (msg->imsg->Code == SELECTDOWN)
                        {
                                BOOL found = FALSE;

                                ObtainSemaphoreShared(&inst->iconsem);
                                for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
                                {
                                        if (SC_DoMethod(node->icon, SCCM_Graphic_HitTest, mousex, mousey))
                                        {
                                                found = TRUE;

                                                if (inst->lastclicked == node)
                                                {
                                                        if (DoubleClick(inst->lastseconds,inst->lastmicros,msg->imsg->Seconds,msg->imsg->Micros))
                                                        {
                                                                DEBUG("Double Click !!\n");
                                                        }
                                                }

                                                if (!(get(node->icon,SCCA_Icon_Selected)))
                                                {
                                                        SC_SetAttrs(node->icon,SCCA_Icon_Selected,TRUE,TAG_DONE);
                                                        if (SC_DoMethod(_scwinobj(obj),SCCM_Window_BeginDraw,obj))
                                                        {
                                                                SC_DoMethod(node->icon,SCCM_Graphic_Draw,_scleft(obj)-offsetx,_sctop(obj)-offsety,SCCV_Graphic_DrawFlags_RelPos);
                                                                SC_DoMethod(_scwinobj(obj),SCCM_Window_EndDraw,obj);
                                                        }
                                                }
                                                break;
                                        }
                                }
                                if (!(found))
                                {
                                        for (node = (struct IconNode *) inst->iconlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct IconNode *) ((struct MinNode *) node)->mln_Succ)
                                        {
                                           if (get(node->icon,SCCA_Icon_Selected))
                                           {
                                                   SC_SetAttrs(node->icon,SCCA_Icon_Selected,FALSE,TAG_DONE);
                                                   if (SC_DoMethod(_scwinobj(obj),SCCM_Window_BeginDraw,obj))
                                                   {
                                                           SC_DoMethod(node->icon,SCCM_Graphic_Draw,_scleft(obj)-offsetx,_sctop(obj)-offsety,SCCV_Graphic_DrawFlags_RelPos);
                                                           SC_DoMethod(_scwinobj(obj),SCCM_Window_EndDraw,obj);
                                                   }
                                           }
                                        }
                                        node = NULL;
                                }
                                ReleaseSemaphore(&inst->iconsem);

                                inst->lastclicked = node;
                                inst->lastseconds = msg->imsg->Seconds;
                                inst->lastmicros = msg->imsg->Micros;
                        }
                }
                break;

          case IDCMP_MENUVERIFY :

                break;
        }
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData IconPanelMethods[] =
{
        { SCCM_IconPanel_Add, (ULONG) IconPanel_Add, sizeof(struct SCCP_IconPanel_Add), 0, NULL},
        { SCCM_IconPanel_Remove, (ULONG) IconPanel_Remove, sizeof(ULONG), 0, NULL},
        { SCCM_IconPanel_GetEntry, (ULONG) IconPanel_GetEntry, sizeof(struct SCCP_IconPanel_GetEntry), 0, NULL},
        { SCCM_IconPanel_GetEntryPos, (ULONG) IconPanel_GetEntryPos, sizeof(struct SCCP_IconPanel_GetEntryPos), 0, NULL},
        { SCCM_Panel_Redraw, (ULONG) IconPanel_Redraw, 0, 0, NULL},
        { SCCM_Init, (ULONG) IconPanel_Init, 0, 0, NULL },
        { SCCM_Exit, (ULONG) IconPanel_Exit, 0, 0, NULL },
        { SCCM_Area_Show,(ULONG) IconPanel_Show, 0, 0, NULL },
        { SCCM_Area_Hide, (ULONG) IconPanel_Hide, 0, 0, NULL },
        { SCCM_Area_Setup,(ULONG) IconPanel_Setup, 0, 0, NULL },
        { SCCM_Area_Cleanup, (ULONG) IconPanel_Cleanup, 0, 0, NULL },
        { SCCM_Area_HandleInput, (ULONG) IconPanel_HandleInput, 0, 0, NULL },
        { SCCM_Area_SetIDCMP, (ULONG) IconPanel_SetIDCMP, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

