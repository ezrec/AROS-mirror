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

struct FontsWinData {
    TEXT fwd_Title[WINDOW_TITLE_LENGTH];
    APTR fwd_FontList;
    APTR fwd_FontText;
    APTR fwd_FontCount;
    APTR fwd_CloseButton;
    APTR fwd_RemoveButton;
    APTR fwd_MoreButton;
};

struct FontCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(fontslist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct FontEntry));
}
MakeStaticHook(fontslist_con2hook, fontslist_con2func);

HOOKPROTONHNO(fontslist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(fontslist_des2hook, fontslist_des2func);

HOOKPROTONHNO(fontslist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct FontEntry *fe = (struct FontEntry *)msg->entry;

    if (fe) {
        msg->strings[0] = fe->fe_Address;
        msg->strings[1] = fe->fe_Name;
        msg->strings[2] = fe->fe_YSize;
        msg->strings[3] = fe->fe_XSize;
        msg->strings[4] = fe->fe_Style;
        msg->strings[5] = fe->fe_Flags;
        msg->strings[6] = fe->fe_Accessors;
        msg->strings[7] = fe->fe_LoChar;
        msg->strings[8] = fe->fe_HiChar;
        msg->strings[9] = fe->fe_Place;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtFontsYSize;
        msg->strings[3] = txtFontsXSize;
        msg->strings[4] = txtFontsStyle;
        msg->strings[5] = txtFontsFlags;
        msg->strings[6] = txtFontsAccessors;
        msg->strings[7] = txtFontsLoChar;
        msg->strings[8] = txtFontsHiChar;
        msg->strings[9] = txtFontsType;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
        msg->preparses[6] = (STRPTR)MUIX_B;
        msg->preparses[7] = (STRPTR)MUIX_B;
        msg->preparses[8] = (STRPTR)MUIX_B;
        msg->preparses[9] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(fontslist_dsp2hook, fontslist_dsp2func);

STATIC LONG fontslist_cmp2colfunc( struct FontEntry *fe1,
                                   struct FontEntry *fe2,
                                   ULONG column )
{
    LONG cmp;

    switch (column) {
        default:
        case 0: cmp = HexCompare(fe1->fe_Address, fe2->fe_Address); break;
        case 1: cmp = stricmp(fe1->fe_Name, fe2->fe_Name); if (cmp == 0) cmp = IntegerCompare(fe1->fe_YSize, fe2->fe_YSize); break;
        case 2: cmp = IntegerCompare(fe1->fe_YSize, fe2->fe_YSize); break;
        case 3: cmp = IntegerCompare(fe1->fe_XSize, fe2->fe_XSize); break;
        case 4: cmp = HexCompare(fe1->fe_Style, fe2->fe_Style); break;
        case 5: cmp = HexCompare(fe1->fe_Flags, fe2->fe_Flags); break;
        case 6: cmp = IntegerCompare(fe1->fe_Accessors, fe2->fe_Accessors); break;
        case 7: cmp = IntegerCompare(fe1->fe_LoChar, fe2->fe_LoChar); break;
        case 8: cmp = IntegerCompare(fe1->fe_HiChar, fe2->fe_HiChar); break;
        case 9: cmp = stricmp(fe1->fe_Place, fe2->fe_Place); break;
    }

    return cmp;
}

STATIC LONG fontslist_cmpfunc( const struct Node *n1,
                              const struct Node *n2 )
{
    return stricmp(((struct FontEntry *)n1)->fe_Name, ((struct FontEntry *)n2)->fe_Name);
}

HOOKPROTONHNO(fontslist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct FontEntry *fe1, *fe2;
    ULONG col1, col2;

    fe1 = (struct FontEntry *)msg->entry1;
    fe2 = (struct FontEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = fontslist_cmp2colfunc(fe2, fe1, col1);
    } else {
        cmp = fontslist_cmp2colfunc(fe1, fe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = fontslist_cmp2colfunc(fe2, fe1, col2);
    } else {
        cmp = fontslist_cmp2colfunc(fe1, fe2, col2);
    }

    return cmp;
}
MakeStaticHook(fontslist_cmp2hook, fontslist_cmp2func);

void FlushFonts(void)
{
    struct TextFont *tf;

    Forbid();

    tf = (struct TextFont *)GfxBase->TextFonts.lh_Head;
    while (tf) {
        struct TextFont *_tf;

        _tf = (struct TextFont *)tf->tf_Message.mn_Node.ln_Succ;

        if (tf->tf_Accessors == 0 && FLAG_IS_SET(tf->tf_Flags, FPF_DISKFONT)) {
            RemFont(tf);
            Remove((struct Node *)tf);
        }

        tf = _tf;
    }

    Permit();
}

STATIC void ReceiveList( void (* callback)( struct FontEntry *fe, void *userData ),
                         void *userData )
{
    struct FontEntry *fe;

    if ((fe = tbAllocVecPooled(globalPool, sizeof(struct FontEntry))) != NULL) {
        if (SendDaemon("GetFontList")) {
            while (ReceiveDecodedEntry(fe, sizeof(struct FontEntry))) {
                callback(fe, userData);
            }
        }

        tbFreeVecPooled(globalPool, fe);
    }
}

STATIC void IterateList( void (* callback)( struct FontEntry *fe, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct TextFont *tf;
    struct MinList tmplist;
    struct FontEntry *fe;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&GfxBase->TextFonts, struct TextFont *, tf) {
        if ((fe = AllocVec(sizeof(struct FontEntry), MEMF_CLEAR)) != NULL) {
            fe->fe_Addr = tf;

            _snprintf(fe->fe_Address, sizeof(fe->fe_Address), ADDRESS_FORMAT, tf);
            _snprintf(fe->fe_YSize, sizeof(fe->fe_YSize), "%3ld", tf->tf_YSize);
            _snprintf(fe->fe_XSize, sizeof(fe->fe_XSize), "%3ld", tf->tf_XSize);
            _snprintf(fe->fe_Style, sizeof(fe->fe_Style), "$%02lx", tf->tf_Style);
            _snprintf(fe->fe_Flags, sizeof(fe->fe_Flags), "$%02lx", tf->tf_Flags);
            _snprintf(fe->fe_LoChar, sizeof(fe->fe_LoChar), "%3ld", tf->tf_LoChar);
            _snprintf(fe->fe_HiChar, sizeof(fe->fe_HiChar), "%3ld", tf->tf_HiChar);
            _snprintf(fe->fe_Accessors, sizeof(fe->fe_Accessors), "%6lD", tf->tf_Accessors);
            if (FLAG_IS_SET(tf->tf_Flags, FPF_ROMFONT)) {
                stccpy(fe->fe_Place, txtFontsROMFont, sizeof(fe->fe_Place));
            } else {
                stccpy(fe->fe_Place, txtFontsDiskFont, sizeof(fe->fe_Place));
            }
            stccpy(fe->fe_Name, tf->tf_Message.mn_Node.ln_Name, sizeof(fe->fe_Name));

            AddTail((struct List *)&tmplist, (struct Node *)fe);
        }
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, fontslist_cmpfunc);

    ITERATE_LIST(&tmplist, struct FontEntry *, fe) {
        callback(fe, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct FontEntry *fe,
                            void *userData )
{
    struct FontCallbackUserData *ud = (struct FontCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, fe);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct FontEntry *fe,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtFontsPrintLine, fe->fe_Address, fe->fe_YSize, fe->fe_XSize, fe->fe_Style, fe->fe_Flags, fe->fe_Accessors, fe->fe_LoChar, fe->fe_HiChar, fe->fe_Place, fe->fe_Name);
}

STATIC void SendCallback( struct FontEntry *fe,
                          UNUSED void *userData )
{
    SendEncodedEntry(fe, sizeof(struct FontEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR fontslist, fontstext, fontscount, updateButton, printButton, closeButton, removeButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Fonts",
        MUIA_Window_ID, MakeID('F','O','N','T'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&fontslist, MakeID('F','O','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR", &fontslist_con2hook, &fontslist_des2hook, &fontslist_dsp2hook, &fontslist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&fontstext, &fontscount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(closeButton    = MakeButton(txtClose)),
                Child, (IPTR)(removeButton   = MakeButton(txtRemove)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct FontsWinData *fwd = INST_DATA(cl, obj);
        APTR parent;

        fwd->fwd_FontList = fontslist;
        fwd->fwd_FontText = fontstext;
        fwd->fwd_FontCount = fontscount;
        fwd->fwd_CloseButton = closeButton;
        fwd->fwd_RemoveButton = removeButton;
        fwd->fwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtFontsTitle, fwd->fwd_Title, sizeof(fwd->fwd_Title)));
        set(obj, MUIA_Window_DefaultObject, fontslist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, fwd->fwd_CloseButton,
                                                          fwd->fwd_RemoveButton,
                                                          fwd->fwd_MoreButton,
                                                          NULL);

        DoMethod(parent,          MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,             MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(fontslist,       MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_FontsWin_ListChange);
        DoMethod(fontslist,       MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_FontsWin_More);
        DoMethod(updateButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_FontsWin_Update);
        DoMethod(printButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_FontsWin_Print);
        DoMethod(closeButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_FontsWin_Close);
        DoMethod(removeButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_FontsWin_Remove);
        DoMethod(moreButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_FontsWin_More);
        DoMethod(exitButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(fontslist,       MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct FontsWinData *fwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(fwd->fwd_FontList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct FontsWinData *fwd = INST_DATA(cl, obj);
    struct FontCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(fwd->fwd_FontList, MUIA_NList_Quiet, TRUE);
    DoMethod(fwd->fwd_FontList, MUIM_NList_Clear);

    ud.ud_List = fwd->fwd_FontList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(fwd->fwd_FontCount, ud.ud_Count);
    MySetContents(fwd->fwd_FontText, "");

    set(fwd->fwd_FontList, MUIA_NList_Quiet, FALSE);
    set(fwd->fwd_FontList, MUIA_NList_Active, MUIV_NList_Active_Off);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, fwd->fwd_CloseButton,
                                                      fwd->fwd_RemoveButton,
                                                      fwd->fwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintFonts(NULL);

    return 0;
}

STATIC IPTR mClose( struct IClass *cl,
                     Object *obj,
                     UNUSED Msg msg )
{
    struct FontsWinData *fwd = INST_DATA(cl, obj);
    struct FontEntry *fe;

    if ((fe = (struct FontEntry *)GetActiveEntry(fwd->fwd_FontList)) != NULL) {
        SIPTR cnt;

        if (IsDec(fe->fe_Accessors, &cnt) && cnt > 0) {
            ULONG answer;

            if ((answer = MyRequest(msgOnceAllCancel, msgWantToCloseFont, fe->fe_Name)) != 0) {
                if (answer == 1) {
                    if (MyDoCommand("CloseFont %s", fe->fe_Address) == RETURN_OK) {
                        cnt--;
                    }
                } else if (answer == 2) {
                    while (cnt > 0) {
                        if (MyDoCommand("CloseFont " ADDRESS_FORMAT, fe->fe_Addr) != RETURN_OK) break;
                        cnt--;
                    }
                }

                _snprintf(fe->fe_Accessors, sizeof(fe->fe_Accessors), "%6lD", cnt);
                RedrawActiveEntry(fwd->fwd_FontList);
            }
        } else {
            MyRequest(msgErrorContinue, msgAccessorCountIsZero);
        }
    }

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct FontsWinData *fwd = INST_DATA(cl, obj);
    struct FontEntry *fe;

    if ((fe = (struct FontEntry *)GetActiveEntry(fwd->fwd_FontList)) != NULL) {
        if (fe->fe_Addr->tf_Accessors > 0) {
            if (MyRequest(msgYesNo, msgWantToRemoveFont, fe->fe_Name)) {
                if (MyDoCommand("RemoveFont %s", fe->fe_Address) == RETURN_OK) {
                    DoMethod(obj, MUIM_FontsWin_Update);
                }
            }
        } else {
            MyRequest(msgErrorContinue, msgAccessorCountIsNotZero);
        }
    }

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct FontsWinData *fwd = INST_DATA(cl, obj);
        struct FontEntry *fe;

        if ((fe = (struct FontEntry *)GetActiveEntry(fwd->fwd_FontList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)FontsDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                End) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_FontsDetailWin_Font, fe);
                set(detailWin, MUIA_Window_Open, TRUE);
                REISSUE_RETURNIDS;
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct FontsWinData *fwd = INST_DATA(cl, obj);
    struct FontEntry *fe;

    if ((fe = (struct FontEntry *)GetActiveEntry(fwd->fwd_FontList)) != NULL) {
        MySetContents(fwd->fwd_FontText, "%s \"%s\"", fe->fe_Address, fe->fe_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, fwd->fwd_CloseButton,
                                                           fwd->fwd_RemoveButton,
                                                           (clientstate) ? NULL : fwd->fwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(FontsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                   return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:               return (mDispose(cl, obj, (APTR)msg));
        case MUIM_FontsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_FontsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_FontsWin_Close:      return (mClose(cl, obj, (APTR)msg));
        case MUIM_FontsWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_FontsWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_FontsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintFonts( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtFontsPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendFontList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeFontsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct FontsWinData), ENTRY(FontsWinDispatcher));
}

