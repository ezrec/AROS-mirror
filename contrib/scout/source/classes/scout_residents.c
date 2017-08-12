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

struct ResidentsWinData {
    TEXT rwd_Title[WINDOW_TITLE_LENGTH];
    APTR rwd_ResidentList;
    APTR rwd_ResidentText;
    APTR rwd_ResidentCount;
    APTR rwd_MoreButton;
};

struct ResidentsCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(resilist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct ResidentEntry));
}
MakeStaticHook(resilist_con2hook, resilist_con2func);

HOOKPROTONHNO(resilist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(resilist_des2hook, resilist_des2func);

HOOKPROTONHNO(resilist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct ResidentEntry *re = (struct ResidentEntry *)msg->entry;

    if (re) {
        msg->strings[0] = re->re_Address;
        msg->strings[1] = re->re_Name;
        msg->strings[2] = re->re_Pri;
        msg->strings[3] = re->re_IdString;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtResidentName;
        msg->strings[2] = txtResidentPri;
        msg->strings[3] = txtResidentIdString;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(resilist_dsp2hook, resilist_dsp2func);

STATIC LONG resilist_cmp2colfunc( struct ResidentEntry *re1,
                                  struct ResidentEntry *re2,
                                  ULONG column )
{
    LONG cmp;

    switch (column) {
        default:
        case 0: cmp = HexCompare(re1->re_Address, re2->re_Address); break;
        case 2: cmp = PriorityCompare(re1->re_Pri, re2->re_Pri); if (cmp != 0) break;
        case 1: cmp = stricmp(re1->re_Name, re2->re_Name); break;
        case 3: cmp = stricmp(re1->re_IdString, re2->re_IdString); break;
    }

    return cmp;
}

STATIC LONG resilist_cmpfunc( const struct Node *n1,
                              const struct Node *n2 )
{
    struct ResidentEntry *re1, *re2;
    LONG cmp;

    re1 = (struct ResidentEntry *)n1;
    re2 = (struct ResidentEntry *)n2;

    cmp = PriorityCompare(re1->re_Pri, re2->re_Pri);
    if (cmp == 0) cmp = stricmp(re1->re_Name, re2->re_Name);

    return cmp;
}

HOOKPROTONHNO(resilist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct ResidentEntry *re1, *re2;
    ULONG col1, col2;

    re1 = (struct ResidentEntry *)msg->entry1;
    re2 = (struct ResidentEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = resilist_cmp2colfunc(re2, re1, col1);
    } else {
        cmp = resilist_cmp2colfunc(re1, re2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = resilist_cmp2colfunc(re2, re1, col2);
    } else {
        cmp = resilist_cmp2colfunc(re1, re2, col2);
    }

    return cmp;
}
MakeStaticHook(resilist_cmp2hook, resilist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct ResidentEntry *re, void *userData ),
                         void *userData )
{
    struct ResidentEntry *re;

    if ((re = tbAllocVecPooled(globalPool, sizeof(struct ResidentEntry))) != NULL) {
        if (SendDaemon("GetResiList")) {
            while (ReceiveDecodedEntry(re, sizeof(struct ResidentEntry))) {
                callback(re, userData);
            }
        }

        tbFreeVecPooled(globalPool, re);
    }
}

STATIC void IterateList( void (* callback)( struct ResidentEntry *re, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MinList tmplist;
    struct ResidentEntry *re;
    IPTR *resip, resi;

    NewList((struct List *)&tmplist);

    Forbid();

    if ((resip = (IPTR *)SysBase->KickTagPtr) != NULL) {
        while ((resi = *resip) != 0) {
            // HSMOD: must handle the case that has directly jump after each other
#if !defined(__AROS__) || (AROS_FLAVOUR & AROS_FLAVOUR_BINCOMPAT)
            while (resi & 0x80000000) {
                resip = (IPTR *)(resi & 0x7fffffff);
                resi = *resip;
            }
#endif
            if ((re = AllocVec(sizeof(struct ResidentEntry), MEMF_CLEAR)) != NULL) {
                struct Resident *rt;

                rt = (struct Resident *)resi;

                re->re_Addr = rt;
                _snprintf(re->re_Address, sizeof(re->re_Address), MUIX_PH ADDRESS_FORMAT MUIX_PT, rt);
                stccpy(re->re_Name, nonetest(rt->rt_Name), sizeof(re->re_Name));
                _snprintf(re->re_Pri, sizeof(re->re_Pri), "%4ld", rt->rt_Pri);
                stccpy(re->re_IdString, nonetest(rt->rt_IdString), sizeof(re->re_IdString));

                healstring(re->re_Name);
                healstring(re->re_IdString);

                AddTail((struct List *)&tmplist, (struct Node *)re);
            }

            resip++;
        }
    }

    if ((resip = (IPTR *)SysBase->ResModules) != NULL) {
        while ((resi = *resip) != 0) {
            // HSMOD: must handle the case that has directly jump after each other
#if !defined(__AROS__) || (AROS_FLAVOUR & AROS_FLAVOUR_BINCOMPAT)
            while (resi & 0x80000000) {
                resip = (IPTR *)(resi & 0x7fffffff);
                resi = *resip;
            }
#endif
            if ((re = AllocVec(sizeof(struct ResidentEntry), MEMF_CLEAR)) != NULL) {
                struct Resident *rt;

                rt = (struct Resident *)resi;

                re->re_Addr = rt;
                _snprintf(re->re_Address, sizeof(re->re_Address), ADDRESS_FORMAT, rt);
                stccpy(re->re_Name, nonetest(rt->rt_Name), sizeof(re->re_Name));
                _snprintf(re->re_Pri, sizeof(re->re_Pri), "%4ld", rt->rt_Pri);
                stccpy(re->re_IdString, nonetest(rt->rt_IdString), sizeof(re->re_IdString));

                healstring(re->re_Name);
                healstring(re->re_IdString);

                AddTail((struct List *)&tmplist, (struct Node *)re);
            }

            resip++;
        }
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, resilist_cmpfunc);

    ITERATE_LIST(&tmplist, struct ResidentEntry *, re) {
        callback(re, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct ResidentEntry *re,
                            void *userData )
{
    struct ResidentsCallbackUserData *ud = (struct ResidentsCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, re);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct ResidentEntry *re,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtResidentsPrintLine, StripMUIFormatting(re->re_Address), re->re_Name, re->re_Pri, re->re_IdString);
}

STATIC void SendCallback( struct ResidentEntry *re,
                          UNUSED void *userData )
{
    SendEncodedEntry(re, sizeof(struct ResidentEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR resilist, resitext, resicount, updateButton, printButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Residents",
        MUIA_Window_ID, MakeID('R','E','S','I'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&resilist, MakeID('R','E','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR", &resilist_con2hook, &resilist_des2hook, &resilist_dsp2hook, &resilist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&resitext, &resicount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ResidentsWinData *rwd = INST_DATA(cl, obj);
        APTR parent;

        rwd->rwd_ResidentList = resilist;
        rwd->rwd_ResidentText = resitext;
        rwd->rwd_ResidentCount = resicount;
        rwd->rwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtResidentsTitle, rwd->rwd_Title, sizeof(rwd->rwd_Title)));
        set(obj, MUIA_Window_DefaultObject, resilist);
        set(moreButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(resilist,     MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_ResidentsWin_ListChange);
        DoMethod(resilist,     MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_ResidentsWin_More);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResidentsWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResidentsWin_Print);
        DoMethod(moreButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResidentsWin_More);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(resilist,     MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_2, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct ResidentsWinData *rwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(rwd->rwd_ResidentList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ResidentsWinData *rwd = INST_DATA(cl, obj);
    struct ResidentsCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(rwd->rwd_ResidentList, MUIA_NList_Quiet, TRUE);
    DoMethod(rwd->rwd_ResidentList, MUIM_NList_Clear);

    ud.ud_List = rwd->rwd_ResidentList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(rwd->rwd_ResidentCount, ud.ud_Count);
    MySetContents(rwd->rwd_ResidentText, "");

    set(rwd->rwd_ResidentList, MUIA_NList_Quiet, FALSE);
    set(rwd->rwd_MoreButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintResidents(NULL);

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct ResidentsWinData *rwd = INST_DATA(cl, obj);
        struct ResidentEntry *re;

        if ((re = (struct ResidentEntry *)GetActiveEntry(rwd->rwd_ResidentList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)ResidentsDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_ResidentsDetailWin_Resident, re);
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
    struct ResidentsWinData *rwd = INST_DATA(cl, obj);
    struct ResidentEntry *re;

    if ((re = (struct ResidentEntry *)GetActiveEntry(rwd->rwd_ResidentList)) != NULL) {
        MySetContents(rwd->rwd_ResidentText, "%s \"%s\"", re->re_Address, re->re_Name);
        if (!clientstate) set(rwd->rwd_MoreButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(ResidentsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                       return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                   return (mDispose(cl, obj, (APTR)msg));
        case MUIM_ResidentsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_ResidentsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_ResidentsWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_ResidentsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintResidents( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtResidentsPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendResiList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeResidentsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ResidentsWinData), ENTRY(ResidentsWinDispatcher));
}

