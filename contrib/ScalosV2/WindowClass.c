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
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include "debug.h"

#include "Scalos.h"
#include "WindowClass.h"
#include "ScalosIntern.h"
#include "SubRoutines.h"

#include "scalos_protos.h"

/****** Window.scalos/--background ******************************************
*
*   Windowclass provides an interface to intuition windows. Any windowclass
*   object will have its own thread.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_Left **************************************
*
*  NAME
*   SCCA_Window_Left -- (V40) ISG (WORD)
*
*  FUNCTION
*   You can set the left offset of the window using this attribute.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_Top ***************************************
*
*  NAME
*   SCCA_Window_Top -- (V40) ISG (WORD)
*
*  FUNCTION
*   This attributes sets the top offset of the window.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_Width *************************************
*
*  NAME
*   SCCA_Window_Width -- (V40) ISG (WORD)
*
*  FUNCTION
*   Sets the width of the window. Default is the default width of the root
*   object.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_Height ************************************
*
*  NAME
*   SCCA_Window_Height -- (V40) ISG (WORD)
*
*  FUNCTION
*   Sets the height of the window. Default is the default height of the root
*   object.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_Title *************************************
*
*  NAME
*   SCCA_Window_Title -- (V40) ISG (char *)
*
*  FUNCTION
*   This attribute is used to set the title of the window. The string will
*   be copied.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_SliderRight *******************************
*
*  NAME
*   SCCA_Window_SliderRight -- (V40) I.. (ULONG) [BOOL]
*
*  FUNCTION
*   Insert a slider into the right border of the window.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_SliderBottom ******************************
*
*  NAME
*   SCCA_Window_SliderBottom -- (V40) I.. (ULONG) [BOOL]
*
*  FUNCTION
*   Insert a slider into the bottom border of the window.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_ScreenObj *********************************
*
*  NAME
*   SCCA_Window_ScreenObj -- (V40) I.. (Object *)
*
*  FUNCTION
*   A Scalos-ScreenObject to specify the screen where the window should be
*   opened.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_Root **************************************
*
*  NAME
*   SCCA_Window_Root -- (V40) I.. (Object *)
*
*  FUNCTION
*   This attribute set the main object of the window. It must be a area-sub-
*   class object.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_CloseRequest ******************************
*
*  NAME
*   SCCA_Window_CloseRequest -- (V40) ..G (ULONG) [BOOL]
*
*  FUNCTION
*   The attribute is set to TRUE if the user hits the closebutton. It's
*   normally used to initiate a notify.
*
*****************************************************************************
*/
// /
/****** Window.scalos/SCCA_Window_Window ************************************
*
*  NAME
*   SCCA_Window_Window -- (V40) ..G (struct Window *)
*
*  FUNCTION
*   Geting this attribute will return a pointer to the intuition window
*   struct or NULL if the window isn't open.
*
*****************************************************************************
*/
// /

/*----------------------------------------------------------------------*/

struct TagItem boolspflagslist[] = 
{
        {SCCA_Window_SliderRight,SWSF_SliderRight},
        {SCCA_Window_SliderBottom,SWSF_SliderBottom},
        {TAG_DONE,NULL}
};

/*--------------------------- Functions --------------------------------*/

/** SetTags - help function for some methods
 * set instance data from taglist using previous instance data values as defaults
 */

static BOOL Window_SetTags(struct TagItem *taglist,struct WindowInst *inst)
{
        struct TagItem *tag;

        inst->left = (WORD)                 GetTagData(SCCA_Window_Left,(ULONG) inst->left,taglist);
        inst->top = (WORD)                  GetTagData(SCCA_Window_Top,(ULONG) inst->top,taglist);
        inst->width = (WORD)                GetTagData(SCCA_Window_Width,(ULONG) inst->width,taglist);
        inst->height = (WORD)               GetTagData(SCCA_Window_Height,(ULONG) inst->height,taglist);
        inst->rinfo.screenobj = (Object *)  GetTagData(SCCA_Window_ScreenObj,(ULONG) inst->rinfo.screenobj,taglist);
        if (tag = FindTagItem(SCCA_Window_Title,taglist))
        {
                FreeString(inst->title);
                if(!(inst->title = (char *)AllocCopyString((char *) tag->ti_Data)))
                        return FALSE;
        }
        inst->specialflags = PackBoolTags(inst->specialflags,taglist,boolspflagslist);
        inst->rootobj = (Object *) GetTagData(SCCA_Window_Root,(ULONG) inst->rootobj,taglist);
        return TRUE;
}
// /

/** Init - initialize instance data to fixed values
 */

static ULONG Window_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct WindowInst *inst)
{
        ULONG ret;

        if (SC_DoSuperMethodA(cl,obj,(Msg )msg))
        {
                SetTaskPri(FindTask(NULL),2);   // Taskpri to 2 to avoid refresh problems

                inst->title = (char *)AllocCopyString( "Scalos Window" );
                inst->flags = WFLG_SIZEGADGET|
                                          WFLG_DRAGBAR|
                                          WFLG_DEPTHGADGET|
                                          WFLG_CLOSEGADGET|
                                          WFLG_SIZEBBOTTOM|
                                          WFLG_SIMPLE_REFRESH|
                                          WFLG_REPORTMOUSE|
                                          WFLG_ACTIVATE|
                                          WFLG_NEWLOOKMENUS|
                                          WFLG_HASZOOM;

                InitSemaphore(&inst->inputsem);
                NewList((struct List *) &inst->inputlist);
                NewList((struct List *) &inst->regionlist);
                InitSemaphore(&inst->dshowsem);
                NewList((struct List *) &inst->dshowlist);

                ret = Window_SetTags( msg->ops_AttrList,inst );

                if ( !ret || !(inst->rootobj) || !(inst->title) )
                {
                        SC_DoClassMethod(cl, NULL,(ULONG) obj, SCCM_Exit); // if something went wrong we have to call SCCM_Exit of our superclass
                        return(FALSE);
                }
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Exit - deallocate resources from Window_Init()
 */

static void Window_Exit( struct SC_Class *cl, Object *obj, Msg msg, struct WindowInst *inst )
{
        if (inst->rinfo.window)
                DoMethod(obj,SCCM_Window_Open,FALSE);
        if (inst->rootobj)
                SC_DisposeObject(inst->rootobj);
        if (inst->title)
                FreeString( inst->title );

        ObtainSemaphore(&inst->inputsem);
        FreeAllNodes(&inst->inputlist);
        ReleaseSemaphore(&inst->inputsem);

        ObtainSemaphore(&inst->dshowsem);
        FreeAllNodes(&inst->dshowlist);
        ReleaseSemaphore(&inst->dshowsem);

        SC_DoSuperMethodA(cl,obj,msg);
}
// /

/** Set - set the given attribute(s)
 * no return value
 */

static void Window_Set( struct SC_Class *cl, Object *obj, struct opSet *msg, struct WindowInst *inst )
{
        SC_DoSuperMethodA( cl,obj,(Msg )msg );
        if (!(inst->rinfo.window))
                Window_SetTags( msg->ops_AttrList,inst );
}
// /

/** Get - return the requested attribute in the method attribute structure (struct opGet in this case)
 */

static ULONG Window_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct WindowInst *inst )
{
        if (msg->opg_AttrID == SCCA_Window_Left)
        {
                *(msg->opg_Storage) = (LONG) inst->left;
                return( TRUE );
        }
        if (msg->opg_AttrID == SCCA_Window_Window)
        {
                *(msg->opg_Storage) = (LONG) inst->rinfo.window;
                return( TRUE );
        }
        if (msg->opg_AttrID == SCCA_Window_SliderRight)
        {
                *(msg->opg_Storage) = (LONG) inst->sliderright;
                return( TRUE );
        }
        if (msg->opg_AttrID == SCCA_Window_SliderBottom)
        {
                *(msg->opg_Storage) = (LONG) inst->sliderbottom;
                return( TRUE );
        }
        return(SC_DoSuperMethodA(cl,obj,(Msg) msg));
}
// /

/****** Window.scalos/SCCM_Window_Open **************************************
*
*  NAME
*   SCCM_Window_Open (V40)
*
*  SYNOPSIS
*   ULONG DoMethod(obj,SCCM_Window_Open,ULONG bool);
*
*  FUNCTION
*   Using this method you can open or close a window.
*
*  INPUTS
*   bool - TRUE if you like to open the window and FALSE to close it.
*
*  RESULT
*   TRUE if the state change was successful. Which means if you like to open
*        the window and it wasn't open and could successfully opened.
*   FALSE if the state change fails.
*
*****************************************************************************
*/

static ULONG Window_Open( struct SC_Class *cl, Object *obj, struct SCCP_Window_Open *msg, struct WindowInst *inst )
{
        struct Screen *screen = (struct Screen *) get(inst->rinfo.screenobj,SCCA_Screen_Screen);
        struct DrawInfo *drawinfo = (struct DrawInfo *) get(inst->rinfo.screenobj,SCCA_Screen_DrawInfo);
        struct Gadget *gadgets = NULL;
        ULONG offset;
        struct Window *win;

        inst->rinfo.windowobj = obj;

        if (!(inst->rinfo.window) && msg->open)
        {
                if (!(SC_DoMethod(inst->rootobj, SCCM_Area_Setup, &inst->rinfo)))
                        return(FALSE);

                if (inst->specialflags & SWSF_SliderRight)
                        inst->flags |= WFLG_SIZEBRIGHT;
                if (inst->specialflags & SWSF_SliderBottom)
                        inst->flags |= WFLG_SIZEBBOTTOM;

                inst->arrowupimg = NewObject(NULL,"sysiclass",
                                                                         SYSIA_DrawInfo,drawinfo,
                                                                         SYSIA_Which,UPIMAGE,
                                                                         TAG_DONE);
                inst->arrowleftimg = NewObject(NULL,"sysiclass",
                                                                           SYSIA_DrawInfo,drawinfo,
                                                                           SYSIA_Which,LEFTIMAGE,
                                                                           TAG_DONE);

                inst->borderleft = screen->WBorLeft;
                inst->bordertop = screen->WBorTop + screen->Font->ta_YSize + 1;
                inst->borderright = screen->WBorRight;

                if (inst->flags & WFLG_SIZEBRIGHT)
                        inst->borderright = inst->arrowupimg->Width;

                inst->borderbottom = screen->WBorBottom;

                if (inst->flags & WFLG_SIZEBBOTTOM)
                        inst->borderbottom = inst->arrowleftimg->Height;

                if ((inst->specialflags & SWSF_SliderRight) || (inst->specialflags & SWSF_SliderBottom))
                {
                        inst->arrowdownimg = NewObject(NULL,"sysiclass",
                                                                                   SYSIA_DrawInfo,drawinfo,
                                                                                   SYSIA_Which,DOWNIMAGE,
                                                                                   TAG_DONE);
                        inst->arrowrightimg = NewObject(NULL,"sysiclass",
                                                                                        SYSIA_DrawInfo,drawinfo,
                                                                                        SYSIA_Which,RIGHTIMAGE,
                                                                                        TAG_DONE);

                        if (inst->specialflags & SWSF_SliderRight)
                        {
                                inst->arrowup = NewObject(NULL,"buttongclass",
                                                                                  GA_Image,inst->arrowupimg,
                                                                                  GA_RelRight,-(inst->arrowupimg->Width-1),
                                                                                  GA_RelBottom,-(2*(inst->arrowupimg->Height)+inst->arrowleftimg->Height-1),
                                                                                  GA_RightBorder,TRUE,
                                                                                  GA_ID,ARROWGADID_UP,
                                                                                  GA_Immediate,TRUE,
                                                                                  GA_Next,gadgets,
                                                                                  TAG_DONE);
                                inst->arrowdown = NewObject(NULL,"buttongclass",
                                                                                        GA_Image,inst->arrowdownimg,
                                                                                        GA_RelRight,-(inst->arrowdownimg->Width-1),
                                                                                        GA_RelBottom,-(inst->arrowdownimg->Height+inst->arrowleftimg->Height-1),
                                                                                        GA_RightBorder,TRUE,
                                                                                        GA_ID,ARROWGADID_DOWN,
                                                                                        GA_Immediate,TRUE,
                                                                                        GA_NEXT,inst->arrowup,
                                                                                        TAG_DONE);
                                offset = screen->WBorTop+screen->Font->ta_YSize+4;
                                inst->sliderright = NewObject(NULL,"propgclass",
                                                                                          GA_RelVerify,TRUE,
                                                                                          GA_Immediate,TRUE,
                                                                                          GA_RightBorder,TRUE,
                                                                                          PGA_Freedom,FREEVERT,
                                                                                          PGA_NewLook,TRUE,
                                                                                          PGA_Borderless,TRUE,
                                                                                          GA_FollowMouse,TRUE,
                                                                                          GA_ID,SLIDERGADID_RIGHT,
                                                                                          GA_RelRight,-(inst->arrowupimg->Width-5),
                                                                                          GA_RelHeight,-(offset+2*(inst->arrowupimg->Height)+inst->arrowleftimg->Height-1),
                                                                                          GA_Top,offset-2,
                                                                                          GA_Width,inst->arrowupimg->Width-8,
                                                                                          PGA_Total,1,PGA_Visible,1,
                                                                                          GA_Next,inst->arrowdown,
                                                                                          TAG_DONE);
                                gadgets = inst->sliderright;
                        }
                        if (inst->specialflags & SWSF_SliderBottom)
                        {

                                inst->arrowleft = NewObject(NULL,"buttongclass",
                                                                                        GA_Image,inst->arrowleftimg,
                                                                                        GA_RelBottom,-(inst->arrowleftimg->Height-1),
                                                                                        GA_RelRight,-(2*(inst->arrowleftimg->Width)+inst->arrowupimg->Width-1),
                                                                                        GA_BottomBorder,TRUE,
                                                                                        GA_ID,ARROWGADID_LEFT,
                                                                                        GA_Immediate,TRUE,
                                                                                        GA_Next,gadgets,
                                                                                        TAG_DONE);
                                inst->arrowright = NewObject(NULL,"buttongclass",
                                                                                         GA_Image,inst->arrowrightimg,
                                                                                         GA_RelBottom,-(inst->arrowrightimg->Height-1),
                                                                                         GA_RelRight,-(inst->arrowrightimg->Width+inst->arrowupimg->Width-1),
                                                                                         GA_BottomBorder,TRUE,
                                                                                         GA_ID,ARROWGADID_RIGHT,
                                                                                         GA_Immediate,TRUE,
                                                                                         GA_NEXT,inst->arrowleft,
                                                                                         TAG_DONE);
                                inst->sliderbottom = NewObject(NULL,"propgclass",
                                                                                           GA_RelVerify,TRUE,
                                                                                           GA_Immediate,TRUE,
                                                                                           GA_BottomBorder,TRUE,
                                                                                           PGA_Freedom,FREEHORIZ,
                                                                                           PGA_NewLook,TRUE,
                                                                                           PGA_Borderless,TRUE,
                                                                                           GA_FollowMouse,TRUE,
                                                                                           GA_ID,SLIDERGADID_BOTTOM,
                                                                                           GA_RelBottom,-(inst->arrowleftimg->Height-4),
                                                                                           GA_RelWidth,-(screen->WBorLeft+2*(inst->arrowleftimg->Width)+inst->arrowupimg->Width+1),
                                                                                           GA_Left,screen->WBorLeft-1,
                                                                                           GA_Height,inst->arrowleftimg->Height-6,
                                                                                           PGA_Total,1,
                                                                                           PGA_Visible,1,
                                                                                           GA_Next,inst->arrowright,
                                                                                           TAG_DONE);
                                gadgets = inst->sliderbottom;
                        }
                }
                else
                {
                        DisposeObject(inst->arrowupimg);
                        DisposeObject(inst->arrowleftimg);
                }

                inst->minmax.minwidth = 0;
                inst->minmax.minheight = 0;
                inst->minmax.maxwidth = 0;
                inst->minmax.maxheight = 0;
                inst->minmax.defwidth = 0;
                inst->minmax.defheight = 0;
                SC_DoMethod(inst->rootobj,SCCM_Area_AskMinMax,&inst->minmax);

                // if min and max sizes are the same, the window isn't sizeable.
                if ((inst->minmax.minwidth == inst->minmax.maxwidth) &&
                        (inst->minmax.minheight == inst->minmax.maxheight))
                {
                        inst->flags &= ~WFLG_SIZEGADGET;
                        inst->borderright = screen->WBorRight;
                        inst->borderbottom = screen->WBorBottom;
                }

                // set default sizes if there wasn't a value specified before
                if (!(inst->width))
                        inst->width = inst->minmax.defwidth + inst->borderright+inst->borderleft;
                if (!(inst->height))
                        inst->height = inst->minmax.defheight + inst->borderbottom+inst->bordertop;
                DEBUG4("WinBorder sizes: %ld,%ld,%ld,%ld\n",inst->borderleft,inst->bordertop,inst->borderright,inst->borderbottom);

                if ( win = OpenWindowTags(NULL,
                                                                  WA_Left,(ULONG) inst->left, 
                                                                  WA_Top,(ULONG) inst->top,
                                                                  WA_Width,(ULONG) inst->width, 
                                                                  WA_Height,(ULONG) inst->height,
                                                                  WA_Flags,inst->flags, 
                                                                  WA_PubScreenFallBack,TRUE,
                                                                  WA_Title,inst->title, 
                                                                  WA_Gadgets,gadgets, WA_CustomScreen,screen,
                                                                  WA_MinWidth,inst->minmax.minwidth+inst->borderright+inst->borderleft,
                                                                  WA_MinHeight,inst->minmax.minheight+inst->borderbottom+inst->bordertop,
                                                                  WA_MaxWidth,inst->minmax.maxwidth+inst->borderright+inst->borderleft,
                                                                  WA_MaxHeight,inst->minmax.maxheight+inst->borderbottom+inst->bordertop,
                                                                  TAG_DONE))
                {
                        struct  IBox newbounds;
                        struct  MinNode *node;
                        ULONG   idcmp = WinIDCMP;

                        inst->rinfo.rport = win->RPort;
                        inst->left = win->LeftEdge;
                        inst->top = win->TopEdge;
                        inst->width = win->Width;
                        inst->height = win->Height;
                        newbounds.Left = win->BorderLeft;
                        newbounds.Top = win->BorderTop;
                        newbounds.Width = win->Width-(win->BorderLeft + win->BorderRight);
                        newbounds.Height = win->Height-(win->BorderTop + win->BorderBottom);

                        win->UserPort = (struct MsgPort *) get(obj,SCCA_MsgPort);
                        ObtainSemaphore(&inst->inputsem);
                        for (node = inst->inputlist.mlh_Head; node->mln_Succ; node = node->mln_Succ)
                                idcmp |= ((struct InputNode *) node)->IDCMP;
                        ModifyIDCMP(win, idcmp);
                        inst->rinfo.window = win;
                        ReleaseSemaphore(&inst->inputsem);

                        SC_DoMethod(inst->rootobj,SCCM_Area_Show,SCCV_Area_Action_OpenClose,&newbounds);
                        ObtainSemaphoreShared(&inst->dshowsem);
                        for (node = inst->dshowlist.mlh_Head; node->mln_Succ; node = node->mln_Succ)
                                SC_DoMethod(((struct DShowNode *) node)->obj,SCCM_Area_DelayedShow);
                        ReleaseSemaphore(&inst->dshowsem);

                        SC_DoMethod(inst->rootobj,SCCM_Area_Draw,SCCV_Area_Drawtype_Complete);
                        return(TRUE);
                }
        }
        if ( inst->rinfo.window && !(msg->open) )
        {
                SC_DoMethod(inst->rootobj, SCCM_Area_Hide, SCCV_Area_Action_OpenClose);

                inst->rinfo.window->UserPort = NULL;
                ModifyIDCMP( inst->rinfo.window,NULL );

                CloseWindow( inst->rinfo.window );
                inst->rinfo.window = NULL;
                SC_DoMethod(inst->rootobj, SCCM_Area_Cleanup);
                if (inst->sliderright)
                        DisposeObject(inst->sliderright);
                if (inst->sliderbottom)
                        DisposeObject(inst->sliderbottom);
                if (inst->arrowup)
                {
                        DisposeObject(inst->arrowup);
                        DisposeObject(inst->arrowupimg);
                }
                if (inst->arrowdown)
                {
                        DisposeObject(inst->arrowdown);
                        DisposeObject(inst->arrowdownimg);
                }
                if (inst->arrowleft)
                {
                        DisposeObject(inst->arrowleft);
                        DisposeObject(inst->arrowleftimg);
                }
                if (inst->arrowright)
                {
                        DisposeObject(inst->arrowright);
                        DisposeObject(inst->arrowrightimg);
                }
                return( TRUE );
        }
        return( FALSE );
}
// /

/** handle IDCMP msg for this window
*/
static ULONG Window_MessageReceived(struct SC_Class *cl, Object *obj, struct SCCP_MessageReceived *msg, struct WindowInst *inst)
{
        struct  Window *win = inst->rinfo.window;
        struct  IntuiMessage *imsg = (struct IntuiMessage *) msg->message;
        struct  MinNode *node;
        ULONG   msgtype;

                //DEBUG1("HandleIDCMPMessage: %lx\n",imsg->Class);
        if (msgtype = SC_IsScalosMsg(msg->message))
        {
                ULONG   newidcmp = WinIDCMP;
                struct  InputNode *inode = 0;
                struct  MinNode *node;

                DEBUG1("InternalMessage: %ld\n",msgtype);
                switch (msgtype)
                {
                        case SCMSG_SETIDCMP :
                                ObtainSemaphore(&inst->inputsem);
                                for (node = inst->inputlist.mlh_Head; node->mln_Succ; node = node->mln_Succ)
                                        if (((struct InputNode *) node)->obj == ((struct SCMSGP_SetIDCMP *) msg->message)->obj)
                                        {
                                                inode = (struct InputNode *) node;
                                                break;
                                        }
        
                                if (((struct SCMSGP_SetIDCMP *) msg->message)->IDCMP && !(inode))
                                        inode = (struct InputNode *) AllocNode(&inst->inputlist,sizeof(struct InputNode));

                                if (inode)
                                        if (!((struct SCMSGP_SetIDCMP *) msg->message)->IDCMP)
                                                FreeNode(inode);
                                        else
                                        {
                                                inode->IDCMP = ((struct SCMSGP_SetIDCMP *) msg->message)->IDCMP;
                                                inode->obj = ((struct SCMSGP_SetIDCMP *) msg->message)->obj;
                                        }
                                for (node = inst->inputlist.mlh_Head; node->mln_Succ; node = node->mln_Succ)
                                        newidcmp |= ((struct InputNode *) node)->IDCMP;

                                ModifyIDCMP(inst->rinfo.window,newidcmp);
                                ReleaseSemaphore(&inst->inputsem);
                                break;
                }
        }
        else
        {
                switch (imsg->Class)
                {

                        case IDCMP_REFRESHWINDOW :
                                LockLayerInfo(inst->rinfo.window->WLayer->LayerInfo);
                                LockLayerRom(inst->rinfo.window->WLayer);
                                BeginUpdate(inst->rinfo.window->WLayer);
                                inst->specialflags |= SWSF_IsRefreshing;
                                if ((inst->width == win->Width) && (inst->height == win->Height))
                                {
                                        SC_DoMethod(inst->rootobj,SCCM_Area_Draw,SCCV_Area_Drawtype_Complete);
                                }
                                else
                                {
                                        DEBUG2("Refresh Error size: %ld,%ld\n",win->Width,win->Height);
                                }
                                inst->specialflags &= ~SWSF_IsRefreshing;
                                inst->rinfo.window->WLayer->Flags &= ~LAYERREFRESH;
                                EndUpdate(inst->rinfo.window->WLayer,TRUE);
                                UnlockLayerRom(inst->rinfo.window->WLayer);
                                UnlockLayerInfo(inst->rinfo.window->WLayer->LayerInfo);
                                break;

                        case IDCMP_CLOSEWINDOW :
                                DEBUG("CloseWindow Request\n");
                                SC_SetAttrs(obj, SCCA_Window_CloseRequest,TRUE,TAG_DONE);
                                break;
                        case IDCMP_NEWSIZE :

                                DEBUG2("NewSize %ld,%ld\n",win->Width,win->Height);
#ifndef NOLOCKLAYER
                                LockLayer(NULL,inst->rinfo.window->WLayer);
#endif
                                if ((inst->width != win->Width) || (inst->height != win->Height))
                                {
                                        struct IBox newbounds;

                                        inst->width = win->Width;
                                        inst->height = win->Height;
                                        SC_DoMethod(inst->rootobj,SCCM_Area_Hide,SCCV_Area_Action_Resize);
                                        newbounds.Left = win->BorderLeft;
                                        newbounds.Top = win->BorderTop;
                                        newbounds.Width = win->Width-(win->BorderLeft + win->BorderRight);
                                        newbounds.Height = win->Height-(win->BorderTop + win->BorderBottom);
                                        SC_DoMethod(inst->rootobj,SCCM_Area_Show,SCCV_Area_Action_Resize,&newbounds);
                                        SC_DoMethod(inst->rootobj,SCCM_Area_Draw,SCCV_Area_Drawtype_Complete);
                                }

                                if ((inst->width != win->Width) || (inst->height != win->Height))
                                {
#ifndef NOLOCKLAYER
                                        UnlockLayer(inst->rinfo.window->WLayer);
#endif
                                        ObtainSemaphoreShared(&inst->dshowsem);
                                        for (node = inst->dshowlist.mlh_Head; node->mln_Succ; node = node->mln_Succ)
                                                SC_DoMethod(((struct DShowNode *) node)->obj,SCCM_Area_DelayedShow);
                                        ReleaseSemaphore(&inst->dshowsem);
                                }
                                else
#ifndef NOLOCKLAYER
                                        UnlockLayer(inst->rinfo.window->WLayer);
#endif
                                break;
                }

                ObtainSemaphoreShared(&inst->inputsem);
                for (node = inst->inputlist.mlh_Head; node->mln_Succ; node = node->mln_Succ)
                {
                        if (((struct InputNode *) node)->IDCMP & imsg->Class)
                                SC_DoMethod(((struct InputNode *) node)->obj,SCCM_Area_HandleInput,imsg);
                }
                ReleaseSemaphore(&inst->inputsem);

        }

        return TRUE;
}
// /

/** SetIDCMP
*/
static void Window_SetIDCMP(struct SC_Class *cl, Object *obj, struct SCCP_Window_SetIDCMP *msg, struct WindowInst *inst)
{
        struct SCMSGP_SetIDCMP *smsg;

        if (smsg = (struct SCMSGP_SetIDCMP *) SC_AllocMsg(SCMSG_SETIDCMP,sizeof(struct SCMSGP_SetIDCMP)))
        {
                smsg->IDCMP = msg->IDCMP;
                smsg->obj = msg->obj;
                PutMsg((struct MsgPort *) get(obj,SCCA_MsgPort), (struct Message *) smsg);      // this message will be freed from the destobject
        }
}
// /

/****** Window.scalos/SCCM_Window_SetClipRegion *****************************
*
*  NAME
*   SCCM_Window_SetClipRegion (V40)
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Window_SetClipRegion,struct Region *clipregion);
*
*  FUNCTION
*   This method sets a clipping region for the window. This region must only
*   be used for short time. It's only valid to use this method within a
*   SCCM_Area_Draw method. The region will not changed. It can be used
*   multiple times.
*
*  INPUTS
*   clipregion - a  region with the clipping infos
*
*  RESULT
*   TRUE - if the clipregion was set
*   FALSE - if something went wrong, you must not use
*           SCCM_Window_ClearClipRegion
*
*  SEE ALSO
*   graphics.library/NewRegion, SCCM_Window_ClearClipRegion
*
*****************************************************************************
*/

static ULONG Window_SetClipRegion(struct SC_Class *cl, Object *obj, struct SCCP_Window_SetClipRegion *msg, struct WindowInst *inst)
{
        struct Region *newregion;
        struct Region *oldregion;
        struct RegionNode *regionnode;

        if (inst->specialflags & SWSF_IsRefreshing)
                oldregion = inst->rinfo.window->WLayer->DamageList;
        else
                oldregion = inst->rinfo.window->WLayer->ClipRegion;

        if (regionnode = (struct RegionNode *) AllocNode(&inst->regionlist, sizeof(struct RegionNode)))
                regionnode->region = oldregion;
        else
                return(FALSE);

        if (newregion = NewRegion())
        {
                OrRegionRegion(msg->clipregion, newregion);
                if (oldregion)
                        AndRegionRegion(oldregion, newregion);

                if (inst->specialflags & SWSF_IsRefreshing)
                {
                        EndUpdate(inst->rinfo.window->WLayer,FALSE);
                        inst->rinfo.window->WLayer->DamageList = newregion;
                        BeginUpdate(inst->rinfo.window->WLayer);
                }
                else
                        InstallClipRegion(inst->rinfo.window->WLayer,newregion);
                return(TRUE);
        }
        else
                FreeNode(inst->regionlist.mlh_TailPred);
                return(FALSE);
}
// /

/****** Window.scalos/SCCM_Window_ClearClipRegion *****************************
*
*  NAME
*   SCCM_Window_ClearClipRegion (V40)
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Window_ClearClipRegion);
*
*  FUNCTION
*   Resets the windows clipping region. This method must be used after your
*   draw operations finish.
*
*  INPUTS
*
*  RESULT
*
*  SEE ALSO
*   SCCM_Window_SetClipRegion
*
*****************************************************************************
*/

static void Window_ClearClipRegion(struct SC_Class *cl, Object *obj, Msg msg, struct WindowInst *inst)
{
        if (inst->specialflags & SWSF_IsRefreshing)
        {
                struct Region *oldregion;

                EndUpdate(inst->rinfo.window->WLayer,FALSE);
                oldregion = inst->rinfo.window->WLayer->DamageList;
                inst->rinfo.window->WLayer->DamageList = ((struct RegionNode *) inst->regionlist.mlh_TailPred)->region;
                BeginUpdate(inst->rinfo.window->WLayer);
                DisposeRegion(oldregion);
        }
        else
                DisposeRegion(InstallClipRegion(inst->rinfo.window->WLayer, ((struct RegionNode *) inst->regionlist.mlh_TailPred)->region));

        FreeNode(inst->regionlist.mlh_TailPred);
}
// /

/****** Window.scalos/SCCM_Window_BeginDraw *********************************
*
*  NAME
*   SCCM_Window_BeginDraw (V40)
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Window_BeginDraw, Object *object);
*
*  FUNCTION
*   This method will initialize the drawing for a specific object. For more
*   info check out SCCM_Area_BeginDraw.
*
*  INPUTS
*   object - the object which should begin draw
*
*  RESULT
*   TRUE if the init was successfull, else FALSE. Drawing is only allowed if
*   this function returnes TRUE.
*
*  SEE ALSO
*   Area.scalos/SCCM_Area_BeginDraw, SCCM_Window_EndDraw
*
*****************************************************************************
*/

static ULONG Window_BeginDraw(struct SC_Class *cl, Object *obj, struct SCCP_Window_BeginDraw *msg, struct WindowInst *inst)
{
        if (inst->rinfo.window || inst->rootobj)
        {
                return(SC_DoMethod(inst->rootobj, SCCM_Area_BeginDraw, msg->object));
        }
        return(FALSE);
}
// /

/****** Window.scalos/SCCM_Window_EndDraw ***********************************
*
*  NAME
*   SCCM_Window_EndDraw (V40)
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Window_EndDraw, Object *object);
*
*  FUNCTION
*   This method will finish the drawing for a specific object. For more
*   info check out SCCM_Area_EndDraw.
*
*  INPUTS
*   object - the object which should end draw
*
*  RESULT
*
*  SEE ALSO
*   Area.scalos/SCCM_Area_BeginDraw, SCCM_Window_BeginDraw
*
*****************************************************************************
*/

static void Window_EndDraw(struct SC_Class *cl, Object *obj, struct SCCP_Window_EndDraw *msg, struct WindowInst *inst)
{
        if (inst->rinfo.window || inst->rootobj)
        {
                SC_DoMethod(inst->rootobj, SCCM_Area_EndDraw, msg->object);
        }
}
// /

/****** Window.scalos/SCCM_Window_DelayedShowObject *************************
*
*  NAME
*   SCCM_Window_DelayedShowObject (V40)
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Window_DelayedShowObject, Object *object);
*
*  FUNCTION
*   This method adds a object to be in the DelayedShow list, which means it
*   will receive SCCM_Area_DelayedShow methods.
*   The problem with the standard SCCM_Area_Show method is, that it will be
*   called with a locked window layer. Sometimes it's impossible to use
*   functions within SCCM_Area_Show (e.g. Gadget refreshing), then it's
*   possible to use the delayed show, but the window size may not been the
*   right, because the window size could change between Show and DelayShow.
*
*  INPUTS
*   object - the object which should receive SCCM_Area_DelayedShow methods
*
*  RESULT
*
*  NOTE
*   Use this function only when you really need it!
*
*  SEE ALSO
*
*****************************************************************************
*/

static void Window_DelayedShowObject(struct SC_Class *cl, Object *obj, struct SCCP_Window_DelayedShowObject *msg, struct WindowInst *inst)
{
        struct DShowNode *dnode;
        struct MinNode  *node;

        ObtainSemaphore(&inst->dshowsem);

        for (node = inst->dshowlist.mlh_Head; node->mln_Succ; node = node->mln_Succ)
                if (((struct DShowNode *) node)->obj == msg->object)
                {
                        ReleaseSemaphore(&inst->dshowsem);
                        return;
                }

        if (dnode = (struct DShowNode *) AllocNode(&inst->dshowlist,sizeof(struct DShowNode)))
                dnode->obj = msg->object;

        ReleaseSemaphore(&inst->dshowsem);
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData WindowMethods[] =
{
        { SCCM_Init,(ULONG) Window_Init, 0, 0, NULL },
        { SCCM_Exit,(ULONG) Window_Exit, 0, 0, NULL },
        { OM_SET,(ULONG) Window_Set, 0, 0, NULL },
        { OM_GET,(ULONG) Window_Get, 0, 0, NULL },
        { SCCM_Window_Open,(ULONG) Window_Open, sizeof(struct SCCP_Window_Open), 0, NULL },
        { SCCM_MessageReceived,(ULONG) Window_MessageReceived, 0, 0, NULL },
        { SCCM_Window_SetIDCMP,(ULONG) Window_SetIDCMP, sizeof(struct SCCP_Window_SetIDCMP), 0, NULL },
        { SCCM_Window_SetClipRegion,(ULONG) Window_SetClipRegion, sizeof(struct SCCP_Window_SetClipRegion), 0, NULL },
        { SCCM_Window_ClearClipRegion,(ULONG) Window_ClearClipRegion, sizeof(ULONG), 0, NULL },
        { SCCM_Window_BeginDraw,(ULONG) Window_BeginDraw, sizeof(struct SCCP_Window_BeginDraw), 0, NULL },
        { SCCM_Window_EndDraw,(ULONG) Window_EndDraw, sizeof(struct SCCP_Window_EndDraw), 0, NULL },
        { SCCM_Window_DelayedShowObject,(ULONG) Window_DelayedShowObject, sizeof(struct SCCP_Window_DelayedShowObject), 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

