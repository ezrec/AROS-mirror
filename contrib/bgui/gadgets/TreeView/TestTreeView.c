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
 * Revision 42.4  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.3  2000/08/09 10:17:25  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.2  2000/08/08 14:02:07  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:22:11  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:59  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:54  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:19:20  mlemos
 * Made the program create TreeView object instances using the bgui.library to
 * setup the gadget library.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:13  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
*************************  TEST TREEVIEW CLASS  *************************
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
#define ID_BT_QUIT                      2
#define ID_BT_TEST1                     3
#define ID_BT_TEST2                     4
#define ID_CYC_METHOD           5
#define ID_CYC_ENTRY            6
#define ID_CYC_WHICH            7
#define ID_CH_SELECTED          8
#define ID_CH_VISIBLE           9
#define ID_CH_MAKEVISIBLE       10
#define ID_CH_SELECT            11
#define ID_CH_DESELECT          12
#define ID_CH_MULTISELECT       13
#define ID_CH_EXPAND            14
#define ID_CH_CONTRACT          15
#define ID_CH_TOGGLE            16

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

//ASM SAVEDS ULONG TVNotifyHandler(REG(a0) struct Hook *hook,
//      REG(a2) Object *obj, REG(a1) struct opUpdate *opu);
ASM SAVEDS REGFUNCPROTO3(ULONG, TVNotifyHandler,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A2, Object *, obj),
        REGPARAM(A1, struct opUpdate *, opu));

struct List     *MakeList(void);
void FreeList(struct List *list);

extern void __stdargs KPrintF(char *fmt,...);

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/

/*
 *      Library base.
 */

struct Library  *BGUIBase = NULL;

/************************************************************************
*******************************  MAIN()  ********************************
************************************************************************/

int main(int argc,char **argv)
{

static STRPTR CycEntryLabels[] = {
        "Root", "Selected", NULL
};

static STRPTR CycWhichLabels[] = {
        "Entry", "Parent", "Child First", "Child Last", "Child Sorted",
        "Child All", "Child Tree", "Sib. First", "Sib. Last",
        "Sib. Next", "Sibl. Prev", "Sib. Sorted", "Sib. All",
        "Sib. Tree", "Tree First", "Tree Last", "Tree Next",
        "Tree Prev", "Tree PgUp", "Tree PgDn", NULL
};

static ULONG WhichValues[] = {
        TVW_ENTRY, TVW_PARENT, TVW_CHILD_FIRST, TVW_CHILD_LAST, TVW_CHILD_SORTED,
        TVW_CHILD_ALL, TVW_CHILD_TREE, TVW_SIBLING_FIRST, TVW_SIBLING_LAST,
        TVW_SIBLING_NEXT, TVW_SIBLING_PREV, TVW_SIBLING_SORTED, TVW_SIBLING_ALL,
        TVW_SIBLING_TREE, TVW_TREE_FIRST, TVW_TREE_LAST, TVW_TREE_NEXT,
        TVW_TREE_PREV, TVW_TREE_PAGE_UP, TVW_TREE_PAGE_DOWN
};

static STRPTR CycMethodLabels[] = {
        "TVM_INSERT", "TVM_REMOVE", "TVM_REPLACE", "TVM_MOVE", "TVM_GETENTRY",
        "TVM_SELECT", "TVM_VISIBLE", "TVM_EXPAND", "TVM_CLEAR", "TVM_LOCK",
        "TVM_UNLOCK", "TVM_SORT", "TVM_REDRAW", "TVM_REFRESH", "TVM_REBUILD",
        NULL
};

static ULONG MethodValues[] = {
        TVM_INSERT, TVM_REMOVE, TVM_REPLACE, TVM_MOVE, TVM_GETENTRY, TVM_SELECT,
        TVM_VISIBLE, TVM_EXPAND, TVM_CLEAR, TVM_LOCK, TVM_UNLOCK, TVM_SORT,
        TVM_REDRAW, TVM_REFRESH, TVM_REBUILD
};

static STRPTR NewEntry = "New Entry";

static STRPTR RepEntry = "Replacement Entry";


if (BGUIBase = OpenLibrary("bgui.library",40))
        {
                struct Hook     idcmphook,tvnotifyhook;
                Object          *WI_Main,*TV_Test,*BT_Test1,*BT_Test2,*BT_Quit;
                Object          *CYC_Method,*CYC_Which, *CYC_Entry;
                Object          *CH_Selected,*CH_Visible,*CH_MakeVisible;
                Object          *CH_Select,*CH_Deselect,*CH_MultiSelect;
                Object          *CH_Expand,*CH_Contract,*CH_Toggle;
                Object          *IMG_Expand,*IMG_Contract;

                IMG_Expand = VectorObject,VIT_BuiltIn,BUILTIN_ARROW_DOWN,
                                        IA_Width,9,IA_Height,9,EndObject;
                IMG_Contract = VectorObject,VIT_BuiltIn,BUILTIN_ARROW_RIGHT,
                                        IA_Width,9,IA_Height,9,EndObject;

                TV_Test = BGUI_NewObject(BGUI_TREEVIEW_GADGET,
                                GA_ID,                          ID_TV_TEST,
                                TVA_Indentation,        12,
                                TVA_ImageStyle,         TVIS_BOX,
                                TVA_LineStyle,          TVLS_DOTS,
                                TVA_LeftAlignImage,     FALSE,
                                TVA_NoLeafImage,        TRUE,
                                /*TVA_ExpandedImage,    IMG_Expand,*/
                                /*TVA_ContractedImage, IMG_Contract,*/
                                LISTV_MinEntriesShown,5,
                                LISTV_MultiSelect,      TRUE,
                                PGA_NewLook,            TRUE,
                                TAG_DONE);

                idcmphook.h_Entry = (HOOKFUNC) WindowHandler;
                idcmphook.h_Data= (APTR) TV_Test;
                tvnotifyhook.h_Entry = (HOOKFUNC) TVNotifyHandler;
                tvnotifyhook.h_Data= (APTR) TV_Test;

                WI_Main = WindowObject,
                        WINDOW_Title,                   "TreeView Class Testbed",
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
                                                HGroupObject,
                                                        StartMember,
                                                                VGroupObject, HOffset(4), VOffset(4), Spacing(4),
                                                                                XenFrame,
                                                                                FRM_Recessed,   TRUE,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CYC_Method = CycleObject,
                                                                                        LAB_Label,              "_Method",
                                                                                        GA_ID,                  ID_CYC_METHOD,
                                                                                        CYC_Labels,             CycMethodLabels,
                                                                                        CYC_Popup,              TRUE,
                                                                                        CYC_PopActive,  TRUE,
                                                                                EndObject,
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CYC_Entry = CycleObject,
                                                                                        LAB_Label,              "_Entry",
                                                                                        GA_ID,                  ID_CYC_ENTRY,
                                                                                        CYC_Labels,             CycEntryLabels,
                                                                                        CYC_Popup,              TRUE,
                                                                                        CYC_PopActive,  TRUE,
                                                                                EndObject,
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CYC_Which = CycleObject,
                                                                                        LAB_Label,              "_Which",
                                                                                        GA_ID,                  ID_CYC_WHICH,
                                                                                        CYC_Labels,             CycWhichLabels,
                                                                                        CYC_Popup,              TRUE,
                                                                                        CYC_PopActive,  TRUE,
                                                                                EndObject,
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                EndObject, FixMinHeight,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                VGroupObject, HOffset(4), VOffset(4), Spacing(2),
                                                                                XenFrame,
                                                                                FRM_Recessed,   TRUE,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_Selected = CheckBox("_Selected",FALSE,ID_CH_SELECTED),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_Visible = CheckBox("_Visible",FALSE,ID_CH_VISIBLE),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_MakeVisible = CheckBox("Ma_keVisible",FALSE,ID_CH_MAKEVISIBLE),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                EndObject,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                VGroupObject, HOffset(4), VOffset(4), Spacing(2),
                                                                                XenFrame,
                                                                                FRM_Recessed,   TRUE,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_Select = CheckBox("Se_lect",FALSE,ID_CH_SELECT),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_Deselect = CheckBox("_Deselect",FALSE,ID_CH_DESELECT),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_MultiSelect = CheckBox("M_ultiSelect",FALSE,ID_CH_MULTISELECT),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                EndObject,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                VGroupObject, HOffset(4), VOffset(4), Spacing(2),
                                                                                XenFrame,
                                                                                FRM_Recessed,   TRUE,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_Expand = CheckBox("E_xpand",FALSE,ID_CH_EXPAND),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_Contract = CheckBox("_Contract",FALSE,ID_CH_CONTRACT),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                        StartMember,
                                                                                CH_Toggle = CheckBox("To_ggle",FALSE,ID_CH_TOGGLE),
                                                                        EndMember,
                                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                                EndObject,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                EndObject, FixMinHeight,
                                        EndMember,
                                        StartMember,
                                                HGroupObject, HOffset(4), VOffset(4), Spacing(4),
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                BT_Test1 = ButtonObject,
                                                                        LAB_Label,                      "Test _1",
                                                                        GA_ID,                          ID_BT_TEST1,
                                                                EndObject,
                                                        EndMember,
                                                        VarSpace(DEFAULT_WEIGHT/2),
                                                        StartMember,
                                                                BT_Test2 = ButtonObject,
                                                                        LAB_Label,                      "Test _2",
                                                                        GA_ID,                          ID_BT_TEST2,
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

                        static STRPTR   Parent[] = {"Parent 0A","Parent 1A","Parent 2A"};
                        static STRPTR   Child[] = {"Child 0A","Child 1A","Child 2A"};
                        static STRPTR   Grand[] = {"Grandchild 0A","Grandchild 1A","Grandchild 2A"};

                        /* ok = AddHook(TV_Test,&tvnotifyhook); */

                        ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                TVM_INSERT,NULL,Parent[0],TV_ROOT,TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Child[0],Parent[0],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Grand[0],Child[0],TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Grand[1],Child[0],TVW_CHILD_LAST,0);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Grand[2],Child[0],TVW_CHILD_LAST,0);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Parent[1],TV_ROOT,TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Child[1],Parent[1],TVW_CHILD_LAST,0);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Parent[2],TV_ROOT,TVW_CHILD_LAST,TVF_EXPAND);

                        if (ok)
                                ok = (BOOL) BGUI_DoGadgetMethod(TV_Test,NULL,NULL,
                                        TVM_INSERT,NULL,Child[2],Parent[2],TVW_CHILD_LAST,0);

                        if (ok)
                                {
                                if (win = WindowOpen(WI_Main))
                                        {
                                        ULONG                   winsig,sigs,id,rc;
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
                                                                ULONG   method,methodid,entryid,tmp;
                                                                ULONG   whichid,entry,which,flags;
                                                                STRPTR  rcstr;

                                                                switch(id)
                                                                        {
                                                                        case WMHI_CLOSEWINDOW:
                                                                        case ID_BT_QUIT:
                                                                                running = FALSE;
                                                                                break;

                                                                        case ID_BT_TEST1:
                                                                                GetAttr(CYC_Active,CYC_Method,&methodid);
                                                                                GetAttr(CYC_Active,CYC_Entry,&entryid);
                                                                                GetAttr(CYC_Active,CYC_Which,&whichid);

                                                                                method = MethodValues[methodid];
                                                                                entry = entryid ? (ULONG) TV_SELECTED : (ULONG) TV_ROOT;
                                                                                which = WhichValues[whichid];
                                                                                rcstr = "N/A";
                                                                                flags = 0;

                                                                                GetAttr(GA_Selected,CH_Selected,&tmp);
                                                                                if (tmp) flags |= TVF_SELECTED;
                                                                                GetAttr(GA_Selected,CH_Visible,&tmp);
                                                                                if (tmp) flags |= TVF_VISIBLE;
                                                                                GetAttr(GA_Selected,CH_Select,&tmp);
                                                                                if (tmp) flags |= TVF_SELECT;
                                                                                GetAttr(GA_Selected,CH_Deselect,&tmp);
                                                                                if (tmp) flags |= TVF_DESELECT;
                                                                                GetAttr(GA_Selected,CH_MultiSelect,&tmp);
                                                                                if (tmp) flags |= TVF_MULTISELECT;
                                                                                GetAttr(GA_Selected,CH_MakeVisible,&tmp);
                                                                                if (tmp) flags |= TVF_MAKEVISIBLE;
                                                                                GetAttr(GA_Selected,CH_Expand,&tmp);
                                                                                if (tmp) flags |= TVF_EXPAND;
                                                                                GetAttr(GA_Selected,CH_Contract,&tmp);
                                                                                if (tmp) flags |= TVF_CONTRACT;
                                                                                GetAttr(GA_Selected,CH_Toggle,&tmp);
                                                                                if (tmp) flags |= TVF_TOGGLE;

                                                                                switch(method)
                                                                                        {
                                                                                        case TVM_INSERT:
                                                                                        case TVM_MOVE:
                                                                                                rc = BGUI_DoGadgetMethod(TV_Test,win,NULL,method,
                                                                                                        NULL,NewEntry,entry,which,flags);
                                                                                                break;

                                                                                        case TVM_REPLACE:
                                                                                                rc = BGUI_DoGadgetMethod(TV_Test,win,NULL,method,
                                                                                                        NULL,entry,RepEntry,which,flags);
                                                                                                break;

                                                                                        case TVM_GETENTRY:
                                                                                                rc = DoMethod(TV_Test,method,
                                                                                                        entry,which,flags);
                                                                                                rcstr = rc ? (STRPTR) rc : "NULL";
                                                                                                break;

                                                                                        case TVM_REMOVE:
                                                                                        case TVM_SELECT:
                                                                                        case TVM_VISIBLE:
                                                                                        case TVM_EXPAND:
                                                                                                rc = BGUI_DoGadgetMethod(TV_Test,win,NULL,method,
                                                                                                        NULL,entry,which,flags);
                                                                                                break;

                                                                                        default:
                                                                                                rc = BGUI_DoGadgetMethod(TV_Test,win,NULL,method,NULL);
                                                                                                break;
                                                                                        }

                                                                                /* debug
                                                                                KPrintF("ID_BT_TEST1: rc=%08lx (%ld) [%s]\n",rc,rc,rcstr);
                                                                                */
                                                                                break;

                                                                        case ID_BT_TEST2:
                                                                                if (entry = DoMethod(TV_Test,TVM_GETENTRY,
                                                                                                TV_SELECTED,TVW_ENTRY,0))
                                                                                        {
                                                                                        /* debug
                                                                                        KPrintF("ID_BT_TEST2: entry=%08lx\n",entry);
                                                                                        */
                                                                                        SetGadgetAttrs((struct Gadget *) TV_Test,
                                                                                                win,NULL,TVA_Top,entry,TAG_DONE);
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
                        DisposeObject(IMG_Contract);
                        DisposeObject(IMG_Expand);
                        }
                CloseLibrary(BGUIBase);
                }
return(0);
}

/*************************************************************************/

//ASM SAVEDS ULONG TVNotifyHandler(REG(a0) struct Hook *hook,
//      REG(a2) Object *obj, REG(a1) struct opUpdate *opu)
ASM SAVEDS REGFUNC3(ULONG, TVNotifyHandler,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A2, Object *, obj),
        REGPARAM(A1, struct opUpdate *, opu))
{
/* debug
struct TagItem  *tag;
*/
struct TagItem  *tstate;
STRPTR                  entry;

tstate = opu->opu_AttrList;

entry = (STRPTR) GetTagData(TVA_Entry,(ULONG) "None",tstate);

/* debug
KPrintF("Test/TVNotify: Entry=%s  Flags=%lu\n",entry,opu->opu_Flags);

while(tag = NextTagItem(&tstate))
        KPrintF("  Tag: %08lx  Value: %08lx\n",tag->ti_Tag,tag->ti_Data);
*/

return(0);
}
REGFUNC_END

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

/*************************************************************************/

struct List     *MakeList(void)
{
struct List     *list;

static STRPTR TestStrs[] = {
        "Hello", "Cruel", "World",
        NULL
};

if (list = AllocVec(sizeof(struct List),MEMF_ANY|MEMF_CLEAR))
        {
        struct Node     *n;
        STRPTR          *p;

        NewList(list);

        p = TestStrs;
        n = (struct Node *) 1;

        while(*p && n)
                {
                if (n = (struct Node *) AllocVec(sizeof(struct Node),MEMF_ANY|MEMF_CLEAR))
                        {
                        n->ln_Name = *p++;
                        AddTail(list,n);
                        }
                }

        if (!n)
                {
                FreeList(list);
                list = NULL;
                }
        }

return(list);
}

/*************************************************************************/

void FreeList(struct List *list)
{
struct Node     *n;

if (list)
        {
        while(n = RemTail(list))
                FreeVec(n);

        FreeVec(list);
        }
}

