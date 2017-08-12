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

struct ScreenModesWinData {
    TEXT smwd_Title[WINDOW_TITLE_LENGTH];
    APTR smwd_ScreenModeList;
    APTR smwd_ScreenModeText;
    APTR smwd_ScreenModeCount;
    APTR smwd_MoreButton;
};

struct ScreenModesCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(smodelist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct ScreenModeEntry));
}
MakeStaticHook(smodelist_con2hook, smodelist_con2func);

HOOKPROTONHNO(smodelist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(smodelist_des2hook, smodelist_des2func);

HOOKPROTONHNO(smodelist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct ScreenModeEntry *sme = (struct ScreenModeEntry *)msg->entry;

    if (sme) {
        msg->strings[0] = sme->sme_Id;
        msg->strings[1] = sme->sme_Name;
        msg->strings[2] = sme->sme_Width;
        msg->strings[3] = sme->sme_Height;
        msg->strings[4] = sme->sme_Depth;
    } else {
        msg->strings[0] = txtScreenModeID;
        msg->strings[1] = txtName;
        msg->strings[2] = txtScreenModeWidth;
        msg->strings[3] = txtScreenModeHeight;
        msg->strings[4] = txtScreenModeDepth;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(smodelist_dsp2hook, smodelist_dsp2func);

STATIC LONG smodelist_cmp2colfunc( struct ScreenModeEntry *sme1,
                                struct ScreenModeEntry *sme2,
                                ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(sme1->sme_Id, sme2->sme_Id);
        case 1: return stricmp(sme1->sme_Name, sme2->sme_Name);
        case 2: return IntegerCompare(sme1->sme_Width, sme2->sme_Width);
        case 3: return IntegerCompare(sme1->sme_Height, sme2->sme_Height);
        case 4: return IntegerCompare(sme1->sme_Depth, sme2->sme_Depth);
    }
}

STATIC LONG smodelist_cmpfunc( const struct Node *n1,
                               const struct Node *n2 )
{
    return stricmp(((struct ScreenModeEntry *)n1)->sme_Name, ((struct ScreenModeEntry *)n2)->sme_Name);
}

HOOKPROTONHNO(smodelist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct ScreenModeEntry *sme1, *sme2;
    ULONG col1, col2;

    sme1 = (struct ScreenModeEntry *)msg->entry1;
    sme2 = (struct ScreenModeEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = smodelist_cmp2colfunc(sme2, sme1, col1);
    } else {
        cmp = smodelist_cmp2colfunc(sme1, sme2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = smodelist_cmp2colfunc(sme2, sme1, col2);
    } else {
        cmp = smodelist_cmp2colfunc(sme1, sme2, col2);
    }

    return cmp;
}
MakeStaticHook(smodelist_cmp2hook, smodelist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct ScreenModeEntry *sme, void *userData ),
                         void *userData )
{
    struct ScreenModeEntry *sme;

    if ((sme = tbAllocVecPooled(globalPool, sizeof(struct ScreenModeEntry))) != NULL) {
        if (SendDaemon("GetSModeList")) {
            while (ReceiveDecodedEntry(sme, sizeof(struct ScreenModeEntry))) {
                callback(sme, userData);
            }
        }

        tbFreeVecPooled(globalPool, sme);
    }
}

STATIC void IterateList( void (* callback)( struct ScreenModeEntry *sme, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MinList tmplist;
    struct ScreenModeData *smd;
    struct ScreenModeEntry *sme;

    NewList((struct List *)&tmplist);

    if ((smd = tbAllocVecPooled(globalPool, sizeof(*smd))) != NULL) {
        ULONG id = INVALID_ID;

        while ((id = NextDisplayInfo(id)) != INVALID_ID) {
            if ((sme = AllocVec(sizeof(struct ScreenModeEntry), MEMF_CLEAR)) != NULL) {
                sme->sme_ModeID = id;
                _snprintf(sme->sme_Id, sizeof(sme->sme_Id), ADDRESS_FORMAT, id);

                GetDisplayInfoData(NULL, (UBYTE *)&smd->dimension, sizeof(smd->dimension), DTAG_DIMS, id);
                _snprintf(sme->sme_Width, sizeof(sme->sme_Width), "%5ld", smd->dimension.Nominal.MaxX - smd->dimension.Nominal.MinX + 1);
                _snprintf(sme->sme_Height, sizeof(sme->sme_Height), "%5ld", smd->dimension.Nominal.MaxY - smd->dimension.Nominal.MinY + 1);
                _snprintf(sme->sme_Depth, sizeof(sme->sme_Depth), "%5ld", (smd->dimension.MaxDepth == 32) ? 24 : smd->dimension.MaxDepth);

                if (GetDisplayInfoData(NULL, (UBYTE *)&smd->name, sizeof(smd->name), DTAG_NAME, id)) {
                    stccpy(sme->sme_Name, smd->name.Name, sizeof(sme->sme_Name));
                } else {
                    if (GetDisplayInfoData(NULL, (UBYTE *)&smd->monitor, sizeof(smd->monitor), DTAG_MNTR, id)) {
                        if (GetDisplayInfoData(NULL, (UBYTE *)&smd->display, sizeof(smd->display), DTAG_DISP, id)) {
                            ULONG prop = smd->display.PropertyFlags;
                            WORD  xres = smd->dimension.Nominal.MaxX - smd->dimension.Nominal.MinX;

                            NameCopy(sme->sme_Name, smd->monitor.Mspc->ms_Node.xln_Name);
                            if (sme->sme_Name[0] == 'E') {
                                if (!strcmp(sme->sme_Name, "EURO72:")) stccpy(sme->sme_Name, "EURO:72Hz ", sizeof(sme->sme_Name));
                                if (!strcmp(sme->sme_Name, "EURO36:")) stccpy(sme->sme_Name, "EURO:36Hz ", sizeof(sme->sme_Name));
                            }

                            if (xres > 1200)     _strcatn(sme->sme_Name, txtScreenModeSuperHighRes, sizeof(sme->sme_Name));
                            else if (xres > 600) _strcatn(sme->sme_Name, txtScreenModeHighRes, sizeof(sme->sme_Name));
                            else if (xres > 300) _strcatn(sme->sme_Name, txtScreenModeLowRes, sizeof(sme->sme_Name));
                            else                 _strcatn(sme->sme_Name, txtScreenModeExtraLowRes, sizeof(sme->sme_Name));

                            if (FLAG_IS_SET(prop, DIPF_IS_DUALPF)) {
                                _strcatn(sme->sme_Name, txtScreenModeDualPF, sizeof(sme->sme_Name));
                                if (FLAG_IS_SET(prop, DIPF_IS_PF2PRI)) _strcatn(sme->sme_Name, txtScreenModeDualPF2, sizeof(sme->sme_Name));
                            }
                            if (FLAG_IS_SET(prop, DIPF_IS_HAM           )) _strcatn(sme->sme_Name, txtScreenModeHAM, sizeof(sme->sme_Name));
                            if (FLAG_IS_SET(prop, DIPF_IS_EXTRAHALFBRITE)) _strcatn(sme->sme_Name, txtScreenModeEHB, sizeof(sme->sme_Name));
                            if (FLAG_IS_SET(prop, DIPF_IS_LACE          )) _strcatn(sme->sme_Name, txtScreenModeInterlace, sizeof(sme->sme_Name));
                            if (FLAG_IS_SET(prop, DIPF_IS_SCANDBL       )) _strcatn(sme->sme_Name, txtScreenModeDblScan, sizeof(sme->sme_Name));
                        } else {
                            stccpy(sme->sme_Name, smd->monitor.Mspc->ms_Node.xln_Name, sizeof(sme->sme_Name));
                        }
                    } else {
                        stccpy(sme->sme_Name, txtScreenModeUnknown, sizeof(sme->sme_Name));
                    }
                }

                AddTail((struct List *)&tmplist, (struct Node *)sme);
            }
        }

        tbFreeVecPooled(globalPool, smd);
    }

    if (sort) SortLinkedList((struct List *)&tmplist, smodelist_cmpfunc);

    ITERATE_LIST(&tmplist, struct ScreenModeEntry *, sme) {
        callback(sme, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct ScreenModeEntry *sme,
                            void *userData )
{
    struct ScreenModesCallbackUserData *ud = (struct ScreenModesCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, sme);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct ScreenModeEntry *sme,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtScreenModesPrintLine, sme->sme_Id, sme->sme_Width, sme->sme_Height, sme->sme_Depth, sme->sme_Name);
}

STATIC void SendCallback( struct ScreenModeEntry *sme,
                          UNUSED void *userData )
{
    SendEncodedEntry(sme, sizeof(struct ScreenModeEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR smodelist, smodetext, smodecount, updateButton, printButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "ScreenMode",
        MUIA_Window_ID, MakeID('S','M','D','E'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&smodelist, MakeID('S','M','L','V'), "BAR,BAR,BAR,BAR,BAR", &smodelist_con2hook, &smodelist_des2hook, &smodelist_dsp2hook, &smodelist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&smodetext, &smodecount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(moreButton   = MakeButton(txtMore)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ScreenModesWinData *smwd = INST_DATA(cl, obj);
        APTR parent;

        smwd->smwd_ScreenModeList = smodelist;
        smwd->smwd_ScreenModeText = smodetext;
        smwd->smwd_ScreenModeCount = smodecount;
        smwd->smwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtScreenModesTitle, smwd->smwd_Title, sizeof(smwd->smwd_Title)));
        set(obj, MUIA_Window_DefaultObject, smodelist);
        set(moreButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(smodelist,    MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_ScreenModesWin_ListChange);
        DoMethod(smodelist,    MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_ScreenModesWin_More);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ScreenModesWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ScreenModesWin_Print);
        DoMethod(moreButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ScreenModesWin_More);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(smodelist,    MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct ScreenModesWinData *smwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(smwd->smwd_ScreenModeList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ScreenModesWinData *smwd = INST_DATA(cl, obj);
    struct ScreenModesCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(smwd->smwd_ScreenModeList, MUIA_NList_Quiet, TRUE);
    DoMethod(smwd->smwd_ScreenModeList, MUIM_NList_Clear);

    ud.ud_List = smwd->smwd_ScreenModeList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(smwd->smwd_ScreenModeCount, ud.ud_Count);
    MySetContents(smwd->smwd_ScreenModeText, "");

    set(smwd->smwd_ScreenModeList, MUIA_NList_Quiet, FALSE);
    set(smwd->smwd_MoreButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintSMode(NULL);

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct ScreenModesWinData *smwd = INST_DATA(cl, obj);
        struct ScreenModeEntry *sme;

        if ((sme = (struct ScreenModeEntry *)GetActiveEntry(smwd->smwd_ScreenModeList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)ScreenModesDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_ScreenModesDetailWin_ScreenMode, sme);
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
    struct ScreenModesWinData *smwd = INST_DATA(cl, obj);
    struct ScreenModeEntry *sme;

    if ((sme = (struct ScreenModeEntry *)GetActiveEntry(smwd->smwd_ScreenModeList)) != NULL) {
        MySetContents(smwd->smwd_ScreenModeText, "%s \"%s\"", sme->sme_Id, sme->sme_Name);
        if (!clientstate) set(smwd->smwd_MoreButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(ScreenModesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                         return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                     return (mDispose(cl, obj, (APTR)msg));
        case MUIM_ScreenModesWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_ScreenModesWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_ScreenModesWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_ScreenModesWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintSMode( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtScreenModesPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendSModeList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeScreenModesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ScreenModesWinData), ENTRY(ScreenModesWinDispatcher));
}

