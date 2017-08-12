/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"

struct CommandsWinData {
    TEXT cwd_Title[WINDOW_TITLE_LENGTH];
    APTR cwd_CommandTree;
    APTR cwd_CommandText;
    APTR cwd_CommandCount;
    APTR cwd_RemoveButton;
};

struct CommandsCallbackUserData {
    APTR ud_Tree;
    ULONG ud_Count;
};

HOOKPROTONHNO(comtree_confunc, IPTR, struct MUIP_NListtree_ConstructMessage *msg)
{
    return AllocListEntry(msg->MemPool, msg->UserData, sizeof(struct CommandEntry));
}
MakeStaticHook(comtree_conhook, comtree_confunc);

HOOKPROTONHNO(comtree_desfunc, void, struct MUIP_NListtree_DestructMessage *msg)
{
    FreeListEntry(msg->MemPool, &msg->UserData);
}
MakeStaticHook(comtree_deshook, comtree_desfunc);

HOOKPROTONHNO(comtree_dspfunc, void, struct MUIP_NListtree_DisplayMessage *msg)
{
    STATIC CONST_STRPTR empty = "\0";

    if (msg->TreeNode) {
        struct CommandEntry *ce = msg->TreeNode->tn_User;

        if (stricmp(ce->ce_Type, "SEG_SINGLE") == 0 || stricmp(ce->ce_Type, "SEG_LIST") == 0) {
            msg->Array[0] = ce->ce_Address;
            msg->Array[1] = ce->ce_Name;
            msg->Array[2] = ce->ce_UseCount;
            msg->Array[3] = ce->ce_SegmentNumStr;
            msg->Array[4] = ce->ce_Lower;
            msg->Array[5] = ce->ce_Upper;
            msg->Array[6] = ce->ce_Size;
        } else {
            msg->Array[0] = (STRPTR)empty;
            msg->Array[1] = (STRPTR)empty;
            msg->Array[2] = (STRPTR)empty;
            msg->Array[3] = ce->ce_SegmentNumStr;
            msg->Array[4] = ce->ce_Lower;
            msg->Array[5] = ce->ce_Upper;
            msg->Array[6] = ce->ce_Size;
        }
        msg->Preparse[4] = (STRPTR)MUIX_C;
        msg->Preparse[5] = (STRPTR)MUIX_C;
        msg->Preparse[6] = (STRPTR)MUIX_R;
    } else {
        msg->Array[0] = txtAddress;
        msg->Array[1] = txtName;
        msg->Array[2] = txtCommandUseCount;
        msg->Array[3] = txtCommandSegment;
        msg->Array[4] = txtCommandLower;
        msg->Array[5] = txtCommandUpper;
        msg->Array[6] = txtCommandSize;
        msg->Preparse[0] = (STRPTR)MUIX_B;
        msg->Preparse[1] = (STRPTR)MUIX_B;
        msg->Preparse[2] = (STRPTR)MUIX_B;
        msg->Preparse[3] = (STRPTR)MUIX_B;
        msg->Preparse[4] = (STRPTR)MUIX_B MUIX_C;
        msg->Preparse[5] = (STRPTR)MUIX_B MUIX_C;
        msg->Preparse[6] = (STRPTR)MUIX_B MUIX_R;
    }
}
MakeStaticHook(comtree_dsphook, comtree_dspfunc);

HOOKPROTONHNO(comtree_cmpfunc, LONG, struct MUIP_NListtree_CompareMessage *msg)
{
    LONG cmp;

    cmp = stricmp(msg->TreeNode1->tn_Name, msg->TreeNode2->tn_Name);
    if (cmp == 0) {
        struct CommandEntry *ce1, *ce2;

        ce1 = (struct CommandEntry *)msg->TreeNode1->tn_User;
        ce2 = (struct CommandEntry *)msg->TreeNode2->tn_User;
        cmp = IntegerCompare(ce1->ce_SegmentNumStr, ce2->ce_SegmentNumStr);
    }

    return cmp;
}
MakeStaticHook(comtree_cmphook, comtree_cmpfunc);

HOOKPROTONHNO(comtree_findfunc, LONG, struct MUIP_NListtree_FindUserDataMessage *msg)
{
    struct CommandEntry *ce;

    ce = (struct CommandEntry *)msg->UserData;

    if (ce) {
        return strcmp(msg->User, ce->ce_Name);
    } else {
        return ~0;
    }
}
MakeStaticHook(comtree_findhook, comtree_findfunc);

STRPTR GetUCType( LONG uc )
{
    STATIC CONST LONG uctype[] = { CMD_SYSTEM, CMD_INTERNAL, CMD_DISABLED, CMD_NOTLOADED, 0 };
    STATIC STRPTR uctypetext[5] = { NULL };
    LONG i;

    uctypetext[0] = txtCommandSystem;
    uctypetext[1] = txtCommandInternal;
    uctypetext[2] = txtCommandDisabled;
    uctypetext[3] = txtCommandNotLoaded;

    i = 0;
    while (uctype[i]) {
        if (uc == uctype[i]) {
            return (uctypetext[i]);
        }
        i++;
     }

     return NULL;
}

STATIC void ReceiveList( void (* callback)( struct CommandEntry *ce, void *userData ),
                         void *userData )
{
    struct CommandEntry *ce;

    if ((ce = tbAllocVecPooled(globalPool, sizeof(struct CommandEntry))) != NULL) {
        if (SendDaemon("GetComList")) {
            while (ReceiveDecodedEntry(ce, sizeof(struct CommandEntry))) {
                callback(ce, userData);
            }
        }

        tbFreeVecPooled(globalPool, ce);
    }
}

STATIC void ScanSegments( struct List *list,
                          UNUSED LONG system )
{
#if defined(__amigaos4__)
    struct DosResidentSeg *seg;

    seg = FindSegment(NULL, NULL, system);
#elif defined(__AROS__)
    struct Segment *seg = NULL; // FIXME
#else
    struct Segment *seg;

    seg = (struct Segment *)BADDR(((struct PrivateDosInfo *)(BADDR(((struct PrivateDosLibrary *)DOSBase)->dl_Root->rn_Info)))->di_NetHand);
#endif

    while (seg) {
        struct CommandEntry *ce;

        if ((ce = AllocVec(sizeof(struct CommandEntry), MEMF_CLEAR)) != NULL) {
            STRPTR tmp;

            ce->ce_Addr = seg;
            _snprintf(ce->ce_Address, sizeof(ce->ce_Address), ADDRESS_FORMAT, seg);
            _snprintf(ce->ce_Name, sizeof(ce->ce_Name), "%b", MKBADDR(seg->seg_Name));

            if ((tmp = GetUCType(seg->seg_UC)) != NULL) {
                stccpy(ce->ce_UseCount, tmp, sizeof(ce->ce_UseCount));

                stccpy(ce->ce_Lower, "---", sizeof(ce->ce_Lower));
                stccpy(ce->ce_Upper, "---", sizeof(ce->ce_Upper));
                stccpy(ce->ce_Size, "---", sizeof(ce->ce_Size));

                ce->ce_SegmentNum = -1;
                stccpy(ce->ce_SegmentNumStr, "---", sizeof(ce->ce_SegmentNumStr));
                stccpy(ce->ce_Type, "SEG_SINGLE", sizeof(ce->ce_Type));

                AddTail(list, (struct Node *)ce);
             } else {
                IPTR *_seg;
                BOOL first = TRUE;
                ULONG segNum;

                _snprintf(ce->ce_UseCount, sizeof(ce->ce_UseCount), "%lD", seg->seg_UC - 1);

                segNum = 1;
                ce->ce_SegmentNum = segNum;
                _snprintf(ce->ce_SegmentNumStr, sizeof(ce->ce_SegmentNumStr), "%lD", ce->ce_SegmentNum);

                _seg = (IPTR *)BADDR(seg->seg_Seg);
                do {
                    struct CommandEntry *_ce;

                    if ((_ce = AllocVec(sizeof(struct CommandEntry), MEMF_CLEAR)) != NULL) {
                        LONG size;

                        CopyMemQuick(ce, _ce, sizeof(struct CommandEntry));

                        size = *(_seg - 1);
                        _snprintf(_ce->ce_Lower, sizeof(_ce->ce_Lower), ADDRESS_FORMAT, ((UBYTE *)_seg) + 4);
                        _snprintf(_ce->ce_Size, sizeof(_ce->ce_Size), "%lD", size);
                        _snprintf(_ce->ce_Upper, sizeof(_ce->ce_Upper), ADDRESS_FORMAT, ((UBYTE *)_seg) - 4 + size);

                        if (first) {
                            first = FALSE;
                            stccpy(_ce->ce_Type, "SEG_LIST", sizeof(_ce->ce_Type));
                        } else {
                            segNum++;
                            _ce->ce_SegmentNum = segNum;
                            _snprintf(_ce->ce_SegmentNumStr, sizeof(_ce->ce_SegmentNumStr), "%lD", _ce->ce_SegmentNum);
                            stccpy(_ce->ce_Type, "SEG_NODE", sizeof(_ce->ce_Type));
                        }

                        AddTail(list, (struct Node *)_ce);
                    }

                    _seg = BADDR(*_seg);
                } while (_seg);
             }
        }

    #if defined(__amigaos4__)
        seg = FindSegment(NULL, seg, system);
    #else
        seg = (struct Segment *)BADDR(seg->seg_Next);
    #endif
    }
}

STATIC void IterateList( void (* callback)( struct CommandEntry *ce, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct CommandEntry *ce;
#if defined(__amigaos4__)
    struct SignalSemaphore *sema;
#endif

    NewList((struct List *)&tmplist);

#if defined(__amigaos4__)
    Forbid();
    sema = FindSemaphore("DosResident");
    Permit();

    if (sema) {
        ObtainSemaphoreShared(sema);
        ScanSegments((struct List *)&tmplist, DOSTRUE);
        ScanSegments((struct List *)&tmplist, DOSFALSE);
        ReleaseSemaphore(sema);
    }
#else
    Forbid();
    ScanSegments((struct List *)&tmplist, DOSTRUE);
    Permit();
#endif

    ITERATE_LIST(&tmplist, struct CommandEntry *, ce) {
        callback(ce, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct CommandEntry *ce,
                            void *userData )
{
    struct CommandsCallbackUserData *ud = (struct CommandsCallbackUserData *)userData;
    struct MUI_NListtree_TreeNode *parent;
    ULONG flags;

    flags = 0;
    if (stricmp(ce->ce_Type, "SEG_SINGLE") == 0) {
        ud->ud_Count++;
    } else if (stricmp(ce->ce_Type, "SEG_LIST") == 0) {
        flags = TNF_LIST | TNF_OPEN | TNF_FROZEN;
        ud->ud_Count++;
    } else if (stricmp(ce->ce_Type, "SEG_NODE") == 0) {
        // don't count up, this belongs to a SEG_LIST node
    }

    parent = (struct MUI_NListtree_TreeNode *)DoMethod(ud->ud_Tree, MUIM_NListtree_FindUserData, MUIV_NListtree_FindUserData_ListNode_Root, ce->ce_Name, MUIV_NListtree_FindUserData_Flag_StartNode);
    DoMethod(ud->ud_Tree, MUIM_NListtree_Insert, ce->ce_Name, ce, parent, MUIV_NListtree_Insert_PrevNode_Sorted, flags);
}

STATIC void PrintCallback( struct CommandEntry *ce,
                           void *userData )
{
    if (ce->ce_SegmentNum == -1 || ce->ce_SegmentNum == 1) {
        PrintFOneLine((BPTR)userData, txtCommandPrintLine, ce->ce_Address, ce->ce_Name, ce->ce_UseCount, ce->ce_Lower, ce->ce_Upper, ce->ce_Size);
    } else {
        PrintFOneLine((BPTR)userData, txtCommandPrintLine, "", "", "", ce->ce_Lower, ce->ce_Upper, ce->ce_Size);
    }
}

STATIC void SendCallback( struct CommandEntry *ce,
                          UNUSED void *userData )
{
    SendEncodedEntry(ce, sizeof(struct CommandEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR comtree, comtext, comcount, updateButton, printButton, removeButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Resident Cmds",
        MUIA_Window_ID, MakeID('R','C','O','M'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListtreeObject(&comtree, "BAR,BAR,BAR,BAR,BAR,BAR,", &comtree_conhook, &comtree_deshook, &comtree_dsphook, &comtree_cmphook, &comtree_findhook, 3, FALSE),
            Child, (IPTR)MyBelowListview(&comtext, &comcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(removeButton = MakeButton(txtRemove)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct CommandsWinData *cwd = INST_DATA(cl, obj);
        APTR parent;

        cwd->cwd_CommandTree = comtree;
        cwd->cwd_CommandText = comtext;
        cwd->cwd_CommandCount = comcount;
        cwd->cwd_RemoveButton = removeButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtCommandsTitle, cwd->cwd_Title, sizeof(cwd->cwd_Title)));
        set(obj, MUIA_Window_DefaultObject, comtree);
        set(removeButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(comtree,      MUIM_Notify, MUIA_NListtree_Active,    MUIV_EveryTime, obj,                     1, MUIM_CommandsWin_ListChange);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommandsWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommandsWin_Print);
        DoMethod(removeButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommandsWin_Remove);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct CommandsWinData *cwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(cwd->cwd_CommandTree, MUIM_NListtree_Clear, NULL, 0);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct CommandsWinData *cwd = INST_DATA(cl, obj);
    struct CommandsCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(cwd->cwd_CommandTree, MUIA_NListtree_Quiet, TRUE);
    DoMethod(cwd->cwd_CommandTree, MUIM_NListtree_Clear, NULL, 0);

    ud.ud_Tree = cwd->cwd_CommandTree;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    SetCountText(cwd->cwd_CommandCount, ud.ud_Count);
    MySetContents(cwd->cwd_CommandText, "");

    set(cwd->cwd_CommandTree, MUIA_NListtree_Quiet, FALSE);
    set(cwd->cwd_RemoveButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintCommands(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct CommandsWinData *cwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(cwd->cwd_CommandTree)) != NULL) {
        struct CommandEntry *ce = (struct CommandEntry *)tn->tn_User;

        if (strcmp(ce->ce_UseCount, txtCommandInternal) == 0) {
            MyRequest(msgErrorContinue, msgCantRemoveInternal);
        } else if (strcmp(ce->ce_UseCount, txtCommandSystem) == 0) {
            MyRequest(msgErrorContinue, msgCantRemoveSystem);
        } else if (strcmp(ce->ce_UseCount, txtCommandDisabled) == 0) {
            MyRequest(msgErrorContinue, msgCantRemoveDisabled);
        } else {
            if (MyRequest(msgYesNo, msgWantToRemoveCommand, ce->ce_Name)) {
                if (MyDoCommand("RemoveCommand " ADDRESS_FORMAT, ce->ce_Addr) == RETURN_OK) {
                    DoMethod(obj, MUIM_CommandsWin_Update);
                }
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct CommandsWinData *cwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(cwd->cwd_CommandTree)) != NULL) {
        struct CommandEntry *ce = (struct CommandEntry *)tn->tn_User;

        MySetContents(cwd->cwd_CommandText, "%s \"%s\"", ce->ce_Address, ce->ce_Name);
        set(cwd->cwd_RemoveButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(CommandsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                      return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                  return (mDispose(cl, obj, (APTR)msg));
        case MUIM_CommandsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_CommandsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_CommandsWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_CommandsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintCommands( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtCommandPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendComList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeCommandsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct CommandsWinData), ENTRY(CommandsWinDispatcher));
}

