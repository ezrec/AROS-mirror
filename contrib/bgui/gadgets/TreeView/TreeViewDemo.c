/*
 * @(#) $Header$
 *
 * BGUI Tree List View class
 *
 * (C) Copyright 1999 Manuel Lemos.
 * (C) Copyright 1996-1999 Nick Christie.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.7  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.6  2003/01/18 19:10:20  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.5  2000/08/09 10:17:25  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.4  2000/08/08 14:02:07  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.3  2000/07/09 11:03:10  stegerg
 * bug fix.
 *
 * Revision 42.2  2000/07/09 03:05:09  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:22:17  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:36:05  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:59  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:19:23  mlemos
 * Made the program create TreeView object instances using the bgui.library to
 * setup the gadget library.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:19  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
*************************  TREEVIEW CLASS DEMO  *************************
************************************************************************/

#include <exec/exec.h>
#include <dos/dos.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>

#include <clib/alib_protos.h>
#include <clib/macros.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/bgui.h>
#include <proto/utility.h>

#include <bgui/bgui_treeview.h>

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
#endif

/************************************************************************
*****************************  DEFINITIONS  *****************************
************************************************************************/

/*
 * Object IDs
 */

#define ID_TV_TEST                      1
#define ID_BT_ADD                       2
#define ID_BT_REMOVE            3
#define ID_BT_QUIT                      4

/*
 * Some rawkey codes
 */

#define RAW_RETURN              68
#define RAW_UPARROW             76
#define RAW_DOWNARROW   77

#define SHIFTKEYS               (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)
#define CTRLKEY                 (IEQUALIFIER_CONTROL)
#define ALTKEYS                 (IEQUALIFIER_LALT|IEQUALIFIER_RALT)

/************************************************************************
*****************************  PROTOTYPES  ******************************
************************************************************************/

int main(int argc,char **argv);

//ASM SAVEDS ULONG WindowHandler(REG(a0) struct Hook *hook,
//      REG(a2) Object *obj, REG(a1) struct IntuiMessage *imsg);
ASM SAVEDS REGFUNCPROTO3(ULONG, WindowHandler,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A2, Object *, obj),
        REGPARAM(A1, struct IntuiMessage *, imsg));

#ifdef __AROS__
#else
extern void __stdargs KPrintF(char *fmt,...);
#endif

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/

/*
 *      Library base.
 */

struct Library  *BGUIBase = NULL;

struct IntuitionBase * IntuitionBase = NULL;
/************************************************************************
*******************************  MAIN()  ********************************
************************************************************************/

int main(int argc,char **argv)
{

IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

if ((BGUIBase = OpenLibrary("bgui.library",40)) && IntuitionBase)
        {
                struct Hook     idcmphook;
                Object          *WI_Main,*TV_Test,*BT_Add,*BT_Remove,*BT_Quit;

                TV_Test = BGUI_NewObject(BGUI_TREEVIEW_GADGET,
                                GA_ID,                          ID_TV_TEST,
                                TVA_Indentation,        12,
                                TVA_ImageStyle,         TVIS_BOX,
                                TVA_LineStyle,          TVLS_DOTS,
                                TVA_NoLeafImage,        TRUE,
                                LISTV_MinEntriesShown,5,
                                PGA_NewLook,            TRUE,
                                TAG_DONE);

                idcmphook.h_Entry = (HOOKFUNC) WindowHandler;
                idcmphook.h_Data= (APTR) TV_Test;

                WI_Main = WindowObject,
                        WINDOW_Title,                   "TreeView Class Demo",
                        WINDOW_ScreenTitle,             "TreeView Class for BGUI by Nick Christie (18.09.96)",
                        WINDOW_AutoAspect,              TRUE,
                        WINDOW_SmartRefresh,    TRUE,
                        WINDOW_RMBTrap,                 TRUE,
                        WINDOW_AutoKeyLabel,    TRUE,
                        WINDOW_ScaleHeight,             33,
                        WINDOW_IDCMPHook,               &idcmphook,
                        WINDOW_IDCMPHookBits,   IDCMP_RAWKEY,
                        WINDOW_MasterGroup,
                                VGroupObject, HOffset(4), VOffset(4), Spacing(4),
                                        StartMember,
                                                TV_Test,
                                        EndMember,
                                        StartMember,
                                                HGroupObject, HOffset(4), VOffset(4), Spacing(4),
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                BT_Add = ButtonObject,
                                                                        LAB_Label,                      "_Add",
                                                                        GA_ID,                          ID_BT_ADD,
                                                                EndObject,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                BT_Remove = ButtonObject,
                                                                        LAB_Label,                      "_Remove",
                                                                        GA_ID,                          ID_BT_REMOVE,
                                                                        GA_Disabled,            TRUE,
                                                                EndObject,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                BT_Quit = ButtonObject,
                                                                        LAB_Label,                      "_Quit",
                                                                        GA_ID,                          ID_BT_QUIT,
                                                                EndObject,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                EndObject, FixMinHeight,
                                        EndMember,
                                EndObject,
                        EndObject;

                if (WI_Main)
                        {
                        struct Window   *win;
                        BOOL                    ok;

                        static STRPTR   GrandParent = "Grandparent";
                        static STRPTR   Parent[] = {"Parent A","Parent B","Parent C"};
                        static STRPTR   ChildA[] = {"Child A0","Child A1","Child A2"};
                        static STRPTR   GrandA[] = {"Grandchild A0A","Grandchild A0B","Grandchild A2A"};
                        static STRPTR   ChildB[] = {"Child B0"};
                        static STRPTR   NewEntry = "New Entry";

                        ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                TVM_INSERT,NULL,GrandParent,TV_ROOT,TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Parent[0],GrandParent,TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Parent[1],GrandParent,TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Parent[2],GrandParent,TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,ChildA[0],Parent[0],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,ChildA[1],Parent[0],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,ChildA[2],Parent[0],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,GrandA[0],ChildA[0],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,GrandA[1],ChildA[0],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,GrandA[2],ChildA[2],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,ChildB[0],Parent[1],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                {
                                if (win = WindowOpen(WI_Main))
                                        {
                                        ULONG                   winsig,sigs,id;
                                        BOOL                    running;

                                        GetAttr(WINDOW_SigMask,WI_Main,&winsig);
                                        running = TRUE;

                                        while(running)
                                                {
                                                sigs = Wait(winsig|SIGBREAKF_CTRL_C);

                                                if (sigs & winsig)
                                                        {
                                                        while((id = HandleEvent(WI_Main)) != WMHI_NOMORE)
                                                                {
                                                                switch(id)
                                                                        {
                                                                        case WMHI_CLOSEWINDOW:
                                                                        case ID_BT_QUIT:
                                                                                running = FALSE;
                                                                                break;

                                                                        case ID_BT_ADD:
                                                                                if (BGUI_DoGadgetMethod(TV_Test,win,NULL,TVM_INSERT,
                                                                                        NULL,NewEntry,TV_SELECTED,TVW_CHILD_FIRST,0))
                                                                                        {
                                                                                        SetGadgetAttrs((struct Gadget *) BT_Add,
                                                                                                win,NULL,GA_Disabled,TRUE,TAG_DONE);
                                                                                        SetGadgetAttrs((struct Gadget *) BT_Remove,
                                                                                                win,NULL,GA_Disabled,FALSE,TAG_DONE);
                                                                                        }
                                                                                break;

                                                                        case ID_BT_REMOVE:
                                                                                if (BGUI_DoGadgetMethod(TV_Test,win,NULL,TVM_REMOVE,
                                                                                        NULL,NewEntry,TVW_ENTRY,0))
                                                                                        {
                                                                                        SetGadgetAttrs((struct Gadget *) BT_Add,
                                                                                                win,NULL,GA_Disabled,FALSE,TAG_DONE);
                                                                                        SetGadgetAttrs((struct Gadget *) BT_Remove,
                                                                                                win,NULL,GA_Disabled,TRUE,TAG_DONE);
                                                                                        }
                                                                                break;

                                                                        default:
                                                                                break;
                                                                        }
                                                                }
                                                        }

                                                if (sigs & SIGBREAKF_CTRL_C)
                                                        running = FALSE;        
                                                }
                                        }
                                }

                        DisposeObject(WI_Main);
                }
        CloseLibrary(BGUIBase);
        }

        if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
return(0);
}

/************************************************************************
***************************  WINDOWHANDLER()  ***************************
*************************************************************************
* Hook for main window IDCMP messages. Called by window object after its
* intuimsg processing has finished, before main() gets control. Used
* here to provide extra keyboard control not possible using normal
* gadget-key assignment, specifically: crsr up/down to select entry in
* treeview and ESC to close window.
*
*************************************************************************/

//ASM SAVEDS ULONG WindowHandler(REG(a0) struct Hook *hook,
//      REG(a2) Object *obj, REG(a1) struct IntuiMessage *imsg)
ASM SAVEDS REGFUNC3(ULONG, WindowHandler,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A2, Object *, obj),
        REGPARAM(A1, struct IntuiMessage *, imsg))
{
struct Window   *win;
ULONG                   cursel,method,entry,which,flags;
Object                  *view;

view = (Object *) hook->h_Data;
win = imsg->IDCMPWindow;

if (imsg->Class == IDCMP_RAWKEY)
        {
        method = TVM_SELECT;
        entry = (ULONG) TV_SELECTED;
        which = ~0;
        flags = TVF_MAKEVISIBLE;
        cursel = DoMethod(view,TVM_GETENTRY,TV_SELECTED,TVW_ENTRY,0);

        if (imsg->Code == RAW_UPARROW)
                {
                if (cursel)
                        {
                        which = TVW_TREE_PREV;

                        if (imsg->Qualifier & SHIFTKEYS)
                                which = TVW_TREE_PAGE_UP;

                        if (imsg->Qualifier & CTRLKEY)
                                {
                                entry = (ULONG) TV_ROOT;
                                which = TVW_CHILD_FIRST;
                                }
                        }
                else
                        {
                        which = TVW_CHILD_FIRST;
                        entry = (ULONG) TV_ROOT;
                        }
                }

        if (imsg->Code == RAW_DOWNARROW)
                {
                if (cursel)
                        {
                        which = TVW_TREE_NEXT;

                        if (imsg->Qualifier & SHIFTKEYS)
                                which = TVW_TREE_PAGE_DOWN;

                        if (imsg->Qualifier & CTRLKEY)
                                {
                                entry = (ULONG) TV_ROOT;
                                which = TVW_TREE_LAST;
                                }
                        }
                else
                        {
                        which = TVW_CHILD_FIRST;
                        entry = (ULONG) TV_ROOT;
                        }
                }

        if (imsg->Code == RAW_RETURN)
                {
                method = TVM_EXPAND;
                which = TVW_ENTRY;
                flags = TVF_TOGGLE;
                }

        if (which != ~0)
                BGUI_DoGadgetMethod(view,win,NULL,method,
                        NULL,entry,which,flags);
        }

return(0);
}
REGFUNC_END
