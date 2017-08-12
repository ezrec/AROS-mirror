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

struct WindowsWinData {
    TEXT wwd_Title[WINDOW_TITLE_LENGTH];
    APTR wwd_WindowTree;
    APTR wwd_WindowText;
    APTR wwd_CloseButton;
    APTR wwd_FrontButton;
    APTR wwd_VisibleButton;
    APTR wwd_MoreButton;
};

struct WindowsCallbackUserData {
    APTR ud_Tree;
    ULONG ud_Count;
};

HOOKPROTONHNO(wintree_confunc, IPTR, struct MUIP_NListtree_ConstructMessage *msg)
{
    return AllocListEntry(msg->MemPool, msg->UserData, sizeof(struct WindowEntry));
}
MakeStaticHook(wintree_conhook, wintree_confunc);

HOOKPROTONHNO(wintree_desfunc, void, struct MUIP_NListtree_DestructMessage *msg)
{
    FreeListEntry(msg->MemPool, &msg->UserData);
}
MakeStaticHook(wintree_deshook, wintree_desfunc);

HOOKPROTONHNO(wintree_dspfunc, void, struct MUIP_NListtree_DisplayMessage *msg)
{
    STATIC CONST_STRPTR empty = "\0";

    if (msg->TreeNode != NULL) {
        struct WindowEntry *we = msg->TreeNode->tn_User;

        if (stricmp(we->we_Type, "SCREEN") == 0) {
            msg->Array[0] = we->we_Address;
            msg->Array[1] = we->we_Position;
            msg->Array[2] = we->we_Size;
            msg->Array[3] = we->we_Flags;
            msg->Array[4] = (STRPTR)empty;
            msg->Array[5] = we->we_Title;
            msg->Preparse[0] = (STRPTR)MUIX_PH;
            msg->Preparse[1] = (STRPTR)MUIX_PH;
            msg->Preparse[2] = (STRPTR)MUIX_PH;
            msg->Preparse[3] = (STRPTR)MUIX_PH;
            msg->Preparse[4] = (STRPTR)MUIX_PH;
            msg->Preparse[5] = (STRPTR)MUIX_PH;
        } else if (stricmp(we->we_Type, "WINDOW") == 0) {
            msg->Array[0] = we->we_Address;
            msg->Array[1] = we->we_Position;
            msg->Array[2] = we->we_Size;
            msg->Array[3] = we->we_Flags;
            msg->Array[4] = we->we_IDCMP;
            msg->Array[5] = we->we_Title;
        }
    } else {
        msg->Array[0] = txtAddress;
        msg->Array[1] = txtWindowPos;
        msg->Array[2] = txtWindowSize;
        msg->Array[3] = txtWindowFlags;
        msg->Array[4] = txtWindowIDCMP;
        msg->Array[5] = txtWindowTitle;
        msg->Preparse[0] = (STRPTR)MUIX_B;
        msg->Preparse[1] = (STRPTR)MUIX_B;
        msg->Preparse[2] = (STRPTR)MUIX_B;
        msg->Preparse[3] = (STRPTR)MUIX_B;
        msg->Preparse[4] = (STRPTR)MUIX_B;
        msg->Preparse[5] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(wintree_dsphook, wintree_dspfunc);

HOOKPROTONHNO(wintree_cmpfunc, LONG, struct MUIP_NListtree_CompareMessage *msg)
{
    struct WindowEntry *we1, *we2;

    we1 = (struct WindowEntry *)msg->TreeNode1->tn_User;
    we2 = (struct WindowEntry *)msg->TreeNode2->tn_User;

    if (we1 != NULL && we2 != NULL) {
        return stricmp(we1->we_Title, we2->we_Title);
    } else {
        return 0;
    }
}
MakeStaticHook(wintree_cmphook, wintree_cmpfunc);

HOOKPROTONHNO(wintree_findfunc, LONG, struct MUIP_NListtree_FindUserDataMessage *msg)
{
    struct WindowEntry *we;

    we = (struct WindowEntry *)msg->UserData;

    if (we) {
        if (stricmp(we->we_Type, "SCREEN") == 0) {
            return stricmp(msg->User, we->we_ScreenAddress);
        } else {
            return ~0;
        }
    } else {
        return ~0;
    }
}
MakeStaticHook(wintree_findhook, wintree_findfunc);

BOOL CloseScreenAll( struct Screen *screen )
{
    struct Window *win;
    struct List *publist;

    win = screen->FirstWindow;
    while (win) {
        struct Window *nextwin;

        nextwin = win->NextWindow;
        CloseWindow(win);
        win = nextwin;
    }

    if ((publist = LockPubScreenList()) != NULL) {
        struct PubScreenNode *psn;
        BOOL isPublic = FALSE;
        ULONG pubVisitors = 0;

        ITERATE_LIST(publist, struct PubScreenNode *, psn) {
            if (psn->psn_Screen == screen) {
                isPublic = TRUE;
                pubVisitors = psn->psn_VisitorCount;
                break;
            }
        }

        UnlockPubScreenList();

        if (isPublic && pubVisitors > 0) {
            ULONG i;

            for (i = 0; i < pubVisitors; i++) {
                UnlockPubScreen(NULL, screen);
            }
        }
    }

    return CloseScreen(screen);
}

STATIC BOOL ObjectExists( struct WindowEntry *we )
{
    if (stricmp(we->we_Type, "SCREEN") == 0) {
        return (BOOL)((MyFindScreen(we->we_Address) != NULL) ? TRUE : FALSE);
    } else if (stricmp(we->we_Type, "WINDOW") == 0) {
        return (BOOL)((MyFindWindow(we->we_Address) != NULL) ? TRUE : FALSE);
    } else {
        return FALSE;
    }
}

STATIC void ReceiveList( void (* callback)( struct WindowEntry *we, void *userData ),
                         void *userData )
{
    struct WindowEntry *we;

    if ((we = tbAllocVecPooled(globalPool, sizeof(struct WindowEntry))) != NULL) {
        if (SendDaemon("GetWinList")) {
            while (ReceiveDecodedEntry(we, sizeof(struct WindowEntry))) {
                callback(we, userData);
            }
        }

        tbFreeVecPooled(globalPool, we);
    }
}

STATIC void IterateList( void (* callback)( struct WindowEntry *we, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct WindowEntry *we;
    ULONG lock;
    struct Screen *screen;

    NewList((struct List *)&tmplist);

    lock = LockIBase(0);

    screen = IntuitionBase->FirstScreen;

    while (screen) {
        if ((we = AllocVec(sizeof(struct WindowEntry), MEMF_CLEAR)) != NULL) {
            struct Window *window;

            we->we_Addr = (APTR)screen;

            _snprintf(we->we_Address, sizeof(we->we_Address), ADDRESS_FORMAT, screen);
            _snprintf(we->we_Position, sizeof(we->we_Position), "%ld,%ld", screen->LeftEdge, screen->TopEdge);
            _snprintf(we->we_Size, sizeof(we->we_Size), "%ld,%ld", screen->Width, screen->Height);
            _snprintf(we->we_Flags, sizeof(we->we_Flags), "$%04lx", screen->Flags);
            stccpy(we->we_IDCMP, "---------", sizeof(we->we_IDCMP));
            _snprintf(we->we_Title, sizeof(we->we_Title), "%-."TEXT_LENGTH_CHAR"s", (TEXT *)nonetest((STRPTR)screen->Title));
            _snprintf(we->we_ScreenAddress, sizeof(we->we_ScreenAddress), ADDRESS_FORMAT, screen);
            stccpy(we->we_Type, "SCREEN", sizeof(we->we_Type));

            AddTail((struct List *)&tmplist, (struct Node *)we);

            if ((window = screen->FirstWindow) != NULL) {
                while (window != NULL) {
                    if ((we = AllocVec(sizeof(struct WindowEntry), MEMF_CLEAR)) != NULL) {
                        we->we_Addr = (APTR)window;
                        _snprintf(we->we_Address, sizeof(we->we_Address), ADDRESS_FORMAT, window);
                        _snprintf(we->we_Position, sizeof(we->we_Position), "%ld,%ld", window->LeftEdge, window->TopEdge);
                        _snprintf(we->we_Size, sizeof(we->we_Size), "%ld,%ld", window->Width, window->Height);
                        _snprintf(we->we_Flags, sizeof(we->we_Flags), "$%08lx", window->Flags);
                        _snprintf(we->we_IDCMP, sizeof(we->we_IDCMP), "$%08lx", window->IDCMPFlags);
                        _snprintf(we->we_Title, sizeof(we->we_Title), "%-."TEXT_LENGTH_CHAR"s", (TEXT *)nonetest((STRPTR)window->Title));
                        _snprintf(we->we_ScreenAddress, sizeof(we->we_ScreenAddress), ADDRESS_FORMAT, screen);
                        stccpy(we->we_Type, "WINDOW", sizeof(we->we_Type));

                        AddTail((struct List *)&tmplist, (struct Node *)we);
                    }

                    window = window->NextWindow;
                }
            }
        }

        screen = screen->NextScreen;
    }

    UnlockIBase(lock);

    ITERATE_LIST(&tmplist, struct WindowEntry *, we) {
        callback(we, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct WindowEntry *we,
                            void *userData )
{
    struct WindowsCallbackUserData *ud = (struct WindowsCallbackUserData *)userData;
    struct MUI_NListtree_TreeNode *parent;
    ULONG flags;

    flags = 0;
    if (stricmp(we->we_Type, "SCREEN") == 0) flags = TNF_LIST | TNF_OPEN;
    if (stricmp(we->we_Type, "WINDOW") == 0) flags = 0;

    parent = (struct MUI_NListtree_TreeNode *)DoMethod(ud->ud_Tree, MUIM_NListtree_FindUserData, MUIV_NListtree_FindUserData_ListNode_Root, we->we_ScreenAddress, MUIV_NListtree_FindUserData_Flag_StartNode);
    DoMethod(ud->ud_Tree, MUIM_NListtree_Insert, we->we_Title, we, parent, MUIV_NListtree_Insert_PrevNode_Sorted, flags);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct WindowEntry *we,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtWindowsPrintLine, we->we_Address, we->we_Position, we->we_Size, we->we_Flags, we->we_IDCMP, we->we_Title);
}

STATIC void SendCallback( struct WindowEntry *we,
                          UNUSED void *userData )
{
    SendEncodedEntry(we, sizeof(struct WindowEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR wintree, wintext, updateButton, printButton, closeButton, frontButton, visibleButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Windows",
        MUIA_Window_ID, MakeID('W','I','N','D'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListtreeObject(&wintree, "BAR,BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR,BAR,BAR", &wintree_conhook, &wintree_deshook, &wintree_dsphook, &wintree_cmphook, &wintree_findhook, 5, FALSE),
            Child, (IPTR)(wintext = MyTextObject()),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton  = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton   = MakeButton(txtPrint)),
                Child, (IPTR)(closeButton   = MakeButton(txtClose)),
                Child, (IPTR)(frontButton   = MakeButton(txtToFront)),
                Child, (IPTR)(visibleButton = MakeButton(txtMakeVisible)),
                Child, (IPTR)(moreButton    = MakeButton(txtMore)),
                Child, (IPTR)(exitButton    = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct WindowsWinData *wwd = INST_DATA(cl, obj);
        APTR parent;

        wwd->wwd_WindowTree = wintree;
        wwd->wwd_WindowText = wintext;
        wwd->wwd_CloseButton = closeButton;
        wwd->wwd_FrontButton = frontButton;
        wwd->wwd_VisibleButton = visibleButton;
        wwd->wwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtWindowsTitle, wwd->wwd_Title, sizeof(wwd->wwd_Title)));
        set(obj, MUIA_Window_DefaultObject, wintree);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, wwd->wwd_CloseButton,
                                                          wwd->wwd_FrontButton,
                                                          wwd->wwd_VisibleButton,
                                                          wwd->wwd_MoreButton,
                                                          NULL);

        DoMethod(parent,        MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,           MUIM_Notify, MUIA_Window_CloseRequest,   TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(wintree,       MUIM_Notify, MUIA_NListtree_Active,      MUIV_EveryTime, obj,                     1, MUIM_WindowsWin_ListChange);
        DoMethod(wintree,       MUIM_Notify, MUIA_NListtree_DoubleClick, MUIV_EveryTime, obj,                     1, MUIM_WindowsWin_More);
        DoMethod(updateButton,  MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_WindowsWin_Update);
        DoMethod(printButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_WindowsWin_Print);
        DoMethod(closeButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_WindowsWin_Close);
        DoMethod(frontButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_WindowsWin_ToFront);
        DoMethod(visibleButton, MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_WindowsWin_MakeVisible);
        DoMethod(moreButton,    MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_WindowsWin_More);
        DoMethod(exitButton,    MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct WindowsWinData *wwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(wwd->wwd_WindowTree, MUIM_NListtree_Clear, NULL, 0);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct WindowsWinData *wwd = INST_DATA(cl, obj);
    struct WindowsCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(wwd->wwd_WindowTree, MUIA_NListtree_Quiet, TRUE);
    DoMethod(wwd->wwd_WindowTree, MUIM_NListtree_Clear, NULL, 0);

    ud.ud_Tree = wwd->wwd_WindowTree;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    MySetContents(wwd->wwd_WindowText, "");

    set(wwd->wwd_WindowTree, MUIA_NListtree_Quiet, FALSE);
    set(wwd->wwd_WindowTree, MUIA_NListtree_Active, MUIV_NListtree_Active_Off);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, wwd->wwd_CloseButton,
                                                      wwd->wwd_FrontButton,
                                                      wwd->wwd_VisibleButton,
                                                      wwd->wwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintWindows(NULL);

    return 0;
}

STATIC IPTR mClose( struct IClass *cl,
                     Object *obj,
                     UNUSED Msg msg )
{
    struct WindowsWinData *wwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(wwd->wwd_WindowTree)) != NULL) {
        struct WindowEntry *we = (struct WindowEntry *)tn->tn_User;

        if (stricmp(we->we_Type, "SCREEN") == 0) {
            if (MyRequest(msgYesNo, msgWantToCloseScreen, we->we_Title)) {
                if (MyDoCommand("CloseScreen %s", we->we_Address) == RETURN_OK) {
                    DoMethod(obj, MUIM_WindowsWin_Update);
                }
            }
        } else if (stricmp(we->we_Type, "WINDOW") == 0) {
            if (MyRequest(msgYesNo, msgWantToCloseWindow, we->we_Title)) {
                if (MyDoCommand("CloseWindow %s", we->we_Address) == RETURN_OK) {
                    DoMethod(obj, MUIM_WindowsWin_Update);
                }
            }
        }
    }

    return 0;
}

STATIC IPTR mToFront( struct IClass *cl,
                       Object *obj,
                       UNUSED Msg msg )
{
    struct WindowsWinData *wwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(wwd->wwd_WindowTree)) != NULL) {
        struct WindowEntry *we = (struct WindowEntry *)tn->tn_User;

        MyDoCommand("PopToFront %s", we->we_Address);
    }

    return 0;
}

STATIC IPTR mMakeVisible( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    struct WindowsWinData *wwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(wwd->wwd_WindowTree)) != NULL) {
        struct WindowEntry *we = (struct WindowEntry *)tn->tn_User;

        MyDoCommand("MakeVisible %s", we->we_Address);
    }

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct WindowsWinData *wwd = INST_DATA(cl, obj);
        struct MUI_NListtree_TreeNode *tn;

        if ((tn = GetActiveTreeNode(wwd->wwd_WindowTree)) != NULL) {
            struct WindowEntry *we = (struct WindowEntry *)tn->tn_User;

            if (ObjectExists(we)) {
                APTR detailWin;

                if ((detailWin = (Object *)WindowsDetailWindowObject,
                        MUIA_Window_Title, (IPTR)we->we_Title,
                        MUIA_Window_ParentWindow, (IPTR)obj,
                        MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                    End) != NULL) {
                    COLLECT_RETURNIDS;
                    set(detailWin, MUIA_WindowsDetailWin_Object, we);
                    set(detailWin, MUIA_Window_Open, TRUE);
                    REISSUE_RETURNIDS;
                }
            } else {
                if (strcmp(we->we_Type, "SCREEN") == 0) {
                    MyRequest(msgErrorContinue, msgCantFindScreen, we->we_Addr);
                } else {
                    MyRequest(msgErrorContinue, msgCantFindWindow, we->we_Addr);
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
    struct WindowsWinData *wwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(wwd->wwd_WindowTree)) != NULL) {
        struct WindowEntry *we = (struct WindowEntry *)tn->tn_User;

        MySetContents(wwd->wwd_WindowText, "%s \"%s\"", we->we_Address, we->we_Title);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, wwd->wwd_CloseButton,
                                                           wwd->wwd_FrontButton,
                                                           (clientstate) ? NULL : wwd->wwd_MoreButton,
                                                           NULL);
        set(wwd->wwd_VisibleButton, MUIA_Disabled, strcmp(we->we_Type, "SCREEN") == 0);
    }

    return 0;
}

DISPATCHER(WindowsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                      return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                  return (mDispose(cl, obj, (APTR)msg));
        case MUIM_WindowsWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_WindowsWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_WindowsWin_Close:       return (mClose(cl, obj, (APTR)msg));
        case MUIM_WindowsWin_ToFront:     return (mToFront(cl, obj, (APTR)msg));
        case MUIM_WindowsWin_MakeVisible: return (mMakeVisible(cl, obj, (APTR)msg));
        case MUIM_WindowsWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_WindowsWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintWindows( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtWindowsPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendWinList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeWindowsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct WindowsWinData), ENTRY(WindowsWinDispatcher));
}

