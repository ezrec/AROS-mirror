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
#include <proto/utility.h>
#include <proto/alib.h>
#include <proto/graphics.h>

#include "Scalos.h"
#include "GraphicClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"

/****** Graphic.scalos/--background *****************************************
*
*   Graphic class is the super class of all simple graphic elements, such as
*   bitmaps or texts
*
*   A valid calling sequence for the methods is for example:
*
*   PreThinkWindow with SCCV_Area_Action_OpenClose
*       Draw
*       Draw
*       ...
*       PreThinkWindow with SCCV_Area_Action_Resize
*           Draw
*           ...
*           PreThinkWindow with SCCV_Area_Action_Resize
*               Draw
*               ...
*           PostThinkWindow with SCCV_Area_Action_Resize
*       PostThinkWindow with SCCV_Area_Action_Resize
*   PostThinkWindow with SCCV_Area_Action_OpenClose
*
*****************************************************************************
*/
// /
/****** Graphic.scalos/SCCA_Graphic_Height **********************************
*
*  NAME
*   SCCA_Graphic_Height -- (V40) ISG (ULONG)
*
*  FUNCTION
*   This attribute specifies the height of the object. The use differs from
*   the class which uses the attribute. For a bitmap class it has to be set
*   to tell the class the size of your data. In some classes it may be
*   impossible to set this attribute. It can not be set within a
*   PreThinkWindow PostThinkWindow pair.
*
*****************************************************************************
*/                                                              
// /

/****** Graphic.scalos/SCCA_Graphic_Width ***********************************
*
*  NAME
*   SCCA_Graphic_Width -- (V40) ISG ULONG
*
*  FUNCTION
*   This attribute specifies the width of the object. The use differs from
*   the class which uses the attribute. For a bitmap class it has to be set
*   to tell the class the size of your data. In some classes it may be
*   impossible to set this attribute. It can not be set within a
*   PreThinkWindow PostThinkWindow pair.
*
*****************************************************************************
*/
// /
/****** Graphic.scalos/SCCA_Graphic_Left ************************************
*
*  NAME
*   SCCA_Graphic_Left -- (V40) ISG (ULONG)
*
*  FUNCTION
*   This attribute sets the left position of the graphic object. It may only
*   affect the SCCM_Graphic_Draw method.
*
*****************************************************************************
*/
// /
/****** Graphic.scalos/SCCA_Graphic_Top *************************************
*
*  NAME
*   SCCA_Graphic_Top -- (V40) ISG (ULONG)
*
*  FUNCTION
*   This attribute sets the top position of the graphic object. It may only
*   affect the SCCM_Graphic_Draw method.
*
*****************************************************************************
*/
// /

/****** Graphic.scalos/SCCM_Graphic_PreThinkScreen **************************
*
*  NAME
*   SCCM_Graphic_PreThinkScreen (V40)
*
*  SYNOPSIS
*   ULONG DoMethod(obj,SCCM_Graphic_PreThinkScreen,Object *screenobj);
*
*  FUNCTION
*   This method ties a Graphic object to a screen.
*   You have to call this method everytime the screen associated with
*   the graphic object changes.
*   This is necessary at least once before you call
*   SCCM_Graphic_PreThinkWindow.
*
*  INPUTS
*   screenobj - the Screen.scalos object that should be associated with the
*               graphic object
*
*  RESULT
*   TRUE if SCCM_Graphic_PreThinkScreen was successfull,
*   FALSE otherwise.
*
*  NOTES
*
*  SEE ALSO
*   SCCM_Graphic_PostThinkScreen
*
*****************************************************************************
*/                                                              

static ULONG Graphic_PreThinkScreen(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PreThinkScreen *msg, struct GraphicInst *inst)
{
        inst->screenobj = msg->Screenobj;
        return TRUE;
}
// /

/****** Graphic.scalos/SCCM_Graphic_PreThinkWindow **************************
*
*  NAME
*   SCCM_Graphic_PreThinkWindow (V40)
*
*  SYNOPSIS
*   ULBOOL DoMethod(obj, SCCM_Graphic_PreThinkWindow,struct RastPort *rastport,ULONG action);
*
*  FUNCTION
*   This method ties a graphic object to a window.
*   You have to call this method everytime the window associated with
*   the Graphic object changes.
*   This is necessary at least once before you call SCCM_Graphic_Draw.
*
*
*  INPUTS
*   rastport - the window`s rastport that should be associated with the
*              Graphic object
*
*   action - SCCV_Window_Resize if the window has been resized
*            SCCV_Window_OpenClose if the window has been opened
*
*  RESULT
*   TRUE if SCCM_Graphic_PreThinkWindow was successfull,
*   FALSE otherwise.
*
*  SEE ALSO
*   SCCM_Graphic_PostThinkWindow, SCCA_Graphic_Height, SCCA_Graphic_Width
*
*****************************************************************************
*/                                                              

static ULONG Graphic_PreThinkWindow(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PreThinkWindow *msg, struct GraphicInst *inst)
{
        inst->rastport = msg->rastport;
        return TRUE;
}
// /

/****** Graphic.scalos/SCCM_Graphic_Draw ************************************
*
*  NAME
*   SCCM_Graphic_Draw (V40)
*
*  SYNOPSIS
*   void DoMethod(obj,SCCM_Graphic_Draw,LONG posX,LONG posY,ULONG drawgflags);
*
*  FUNCTION
*   This method draws the graphic object to the window`s rastport as fast as
*   possible. All the time consuming things have been performed before.
*
*  INPUTS
*   posX,posY - the destination position of the upper left corner of the
*               graphics data relative to the window`s rastport
*   drawflags - SCCV_Graphic_DrawFlags_RelPos - posX and posY will be added
*                                               to the objects position, the
*                                               object will be drawn to the
*                                               resulting position
*               SCCV_Graphic_DrawFlags_AbsPos - posX and posY are relativ to
*                                               the rastport
*
*  RESULT
*   not defined
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/                                                              

static void Graphic_Draw(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_Draw *msg, struct GraphicInst *inst)
{
}
// /

/****** Graphic.scalos/SCCM_Graphic_PostThinkWindow *************************
*
*  NAME
*   SCCM_Graphic_PostThinkWindow (V40)
*
*  SYNOPSIS
*   void DoMethod(obj,SCCM_Graphic_PostThinkWindow,ULONG action);
*
*  FUNCTION
*   This method unties a graphic object from a window.
*   You have to call this method for every successfull call to
*   SCCM_Graphic_PreThinkWindow.
*   This method is called automatically if SCCM_Graphic_PreThinkWindow has
*   returned FALSE, so it`s wrong if you call this method for an
*   unsuccessfull call to SCCM_Graphic_PreThinkWindow too.
*
*  INPUTS
*   action - SCCV_Window_Resize if the window has been resized
*            SCCV_Window_OpenClose if the window has been closed
*
*  RESULT
*   not defined
*
*  NOTES
*
*  SEE ALSO
*   SCCM_Graphic_PreThinkWindow
*
*****************************************************************************
*/

static void Graphic_PostThinkWindow(struct SC_Class *cl, Object *obj, struct SCCP_Graphic_PostThinkWindow *msg, struct GraphicInst *inst)
{
        inst->rastport = NULL;
}
// /

/****** Graphic.scalos/SCCM_Graphic_PostThinkScreen *************************
*
*  NAME
*   SCCM_Graphic_PostThinkScreen (V40)
*
*  SYNOPSIS
*   void DoMethod(obj,SCCM_Graphic_PostThinkScreen);
*
*  FUNCTION
*   This method unties a graphic object from a screen.
*   You have to call this method for every successfull call to
*   SCCM_Graphic_PreThinkScreen.
*   This method is called automatically if SCCM_Graphic_PreThinkScreen has
*   returned FALSE, so it`s wrong if you call this method for an
*   unsuccessfull call to SCCM_Graphic_PreThinkScreen too.
*
*  INPUTS
*
*  RESULT
*   not defined
*
*  NOTES
*
*  SEE ALSO
*   SCCM_Graphic_PreThinkScreen
*
*****************************************************************************
*/

static void Graphic_PostThinkScreen(struct SC_Class *cl, Object *obj, Msg *msg, struct GraphicInst *inst)
{
        inst->screenobj = NULL;
}   
// /

/****** Graphic.scalos/SCCM_Graphic_Erase ***********************************
*
*  NAME
*   SCCM_Graphic_Erase (V40)
*
*  SYNOPSIS
*   void DoMethod(obj,SCCM_Graphic_Erase,LONG posX,LONG posY,ULONG drawgflags);
*
*  FUNCTION
*   This method erases the graphic object from rastport and filles the area
*   with the standard background. It's similar to the graphics.library
*   function EraseRect(), but it restores backgounds from objects too.
*
*  INPUTS
*   posX,posY - the destination position of the upper left corner of the
*               graphics data relative to the window`s rastport
*   drawflags - SCCV_Graphic_DrawFlags_RelPos - posX and posY will be added
*                                               to the objects position, the
*                                               object will be erased from the
*                                               resulting position
*               SCCV_Graphic_DrawFlags_AbsPos - posX and posY are relativ to
*                                               the rastport
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/

static void Graphic_Erase(struct SC_Class *cl, struct SC_GraphicObject *obj, struct SCCP_Graphic_Draw *msg, struct GraphicInst *inst)
{
        int x = msg->posX;
        int y = msg->posY;

        if (msg->drawflags == SCCV_Graphic_DrawFlags_RelPos)
        {
                x += obj->bounds.Left;         // direct access allowed, because we are access our own
                y += obj->bounds.Top;
        }

        EraseRect(obj->rastport,x,y,x + obj->bounds.Width - 1,y + obj->bounds.Height - 1);

}
// /

/****** Graphic.scalos/SCCM_Graphic_HitTest *********************************
*
*  NAME
*   SCCM_Graphic_HitTest (V40)
*
*  SYNOPSIS
*   ULONG DoMethod(obj,SCCM_Graphic_HitTest,LONG X,LONG Y);
*
*  FUNCTION
*   This method should be used to find out if the given coordinates are
*   inside of the graphic object.
*
*  INPUTS
*   X,Y - the position to test
*
*  RESULT
*   TRUE if the coordinates are inside of the object, else FALSE.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG Graphic_HitTest(struct SC_Class *cl, struct SC_GraphicObject *obj, struct SCCP_Graphic_HitTest *msg, struct GraphicInst *inst)
{
        if ((msg->X >= obj->bounds.Left) && (msg->X < (obj->bounds.Left + obj->bounds.Width))
                && (msg->Y >= obj->bounds.Top) && (msg->Y < (obj->bounds.Top + obj->bounds.Height)))
                return(TRUE);
        else
                return(FALSE);
}
// /

/** Init a graphic object
*/

static ULONG Graphic_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct GraphicInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                inst->bounds.Left = GetTagData(SCCA_Graphic_Left,0,msg->ops_AttrList);
                inst->bounds.Top = GetTagData(SCCA_Graphic_Top,0,msg->ops_AttrList);
                inst->bounds.Width = GetTagData(SCCA_Graphic_Width,0,msg->ops_AttrList);
                inst->bounds.Height = GetTagData(SCCA_Graphic_Height,0,msg->ops_AttrList);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Set one or more setable attribute(s)
 */
static void Graphic_Set( struct SC_Class *cl, Object *obj, struct opSet *msg, struct GraphicInst *inst )
{
        inst->bounds.Left = GetTagData(SCCA_Graphic_Left,inst->bounds.Left,msg->ops_AttrList);
        inst->bounds.Top = GetTagData(SCCA_Graphic_Top,inst->bounds.Top,msg->ops_AttrList);

        if (!(inst->rastport))
        {
                inst->bounds.Width = GetTagData(SCCA_Graphic_Width,inst->bounds.Width,msg->ops_AttrList);
                inst->bounds.Height = GetTagData(SCCA_Graphic_Height,inst->bounds.Height,msg->ops_AttrList);
        }

        SC_DoSuperMethodA(cl,obj,(Msg) msg); // analog to MUI devguide
}
// /

/** Get one attribute of all of the getable attributes
 */
static ULONG Graphic_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct GraphicInst *inst )
{
        if (msg->opg_AttrID == SCCA_Graphic_Height)
        {
                *(msg->opg_Storage) = (ULONG) inst->bounds.Height;
                return( TRUE );
        }

        if (msg->opg_AttrID == SCCA_Graphic_Width)
        {
                *(msg->opg_Storage) = (ULONG) inst->bounds.Width;
                return( TRUE );
        }

        if (msg->opg_AttrID == SCCA_Graphic_Left)
        {
                *(msg->opg_Storage) = (LONG) inst->bounds.Left;
                return( TRUE );
        }

        if (msg->opg_AttrID == SCCA_Graphic_Top)
        {
                *(msg->opg_Storage) = (LONG) inst->bounds.Top;
                return( TRUE );
        }

        return( SC_DoSuperMethodA(cl, obj, (Msg) msg) );
}
// /

/** Clone a graphic object
*/

static Object *Graphic_Clone(struct SC_Class *cl, Object *obj, Msg msg, struct GraphicInst *inst)
{
        struct GraphicInst *newinst;

        if ((newinst = (struct GraphicInst *) SC_DoSuperMethodA(cl,obj, (Msg) msg)))
        {
                newinst->bounds = inst->bounds;
                return((Object *) newinst);
        }
        return(NULL);
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData GraphicMethods[] =
{
        { OM_SET,(ULONG)                        Graphic_Set, 0, 0, NULL },               // derived from RootClass
        { OM_GET,(ULONG)                        Graphic_Get, 0, 0, NULL },               // derived from RootClass
        { SCCM_Init,(ULONG)                     Graphic_Init, 0, 0, NULL },              // derived from RootClass
        { SCCM_Graphic_PreThinkScreen,(ULONG)   Graphic_PreThinkScreen, sizeof(struct SCCP_Graphic_PreThinkScreen), SCMDF_FULLFLUSH, NULL },  // a new method
        { SCCM_Graphic_PreThinkWindow,(ULONG)   Graphic_PreThinkWindow, sizeof(struct SCCP_Graphic_PreThinkWindow), SCMDF_FULLFLUSH, NULL },  // a new method
        { SCCM_Graphic_Draw,(ULONG)             Graphic_Draw, sizeof(struct SCCP_Graphic_Draw), SCMDF_DIRECTMETHOD, NULL },
        { SCCM_Graphic_Erase,(ULONG)            Graphic_Erase, sizeof(struct SCCP_Graphic_Draw), SCMDF_DIRECTMETHOD, NULL },
        { SCCM_Graphic_HitTest,(ULONG)          Graphic_HitTest, sizeof(struct SCCP_Graphic_HitTest), 0, NULL },
        { SCCM_Graphic_PostThinkWindow,(ULONG)  Graphic_PostThinkWindow, sizeof(struct SCCP_Graphic_PostThinkWindow), SCMDF_FULLFLUSH, NULL },    // a new method
        { SCCM_Graphic_PostThinkScreen,(ULONG)  Graphic_PostThinkScreen, sizeof(ULONG), 0, NULL },   // a new method
//  { SCCM_Exit,(ULONG)                     Graphic_Exit, 0, 0, NULL },              // derived from RootClass
        { SCCM_Clone,(ULONG)                    Graphic_Clone, 0, 0, NULL },              // derived from RootClass
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

