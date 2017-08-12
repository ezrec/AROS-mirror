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

struct PatchesWinData {
    TEXT pwd_Title[WINDOW_TITLE_LENGTH];
    APTR pwd_PatchList;
    APTR pwd_PatchText;
    APTR pwd_PatchCount;
    APTR pwd_EnableButton;
    APTR pwd_DisableButton;
    struct SetManPort *pwd_SetManPort;
};

struct PatchesCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(patchlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct PatchEntry));
}
MakeStaticHook(patchlist_con2hook, patchlist_con2func);

HOOKPROTONHNO(patchlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(patchlist_des2hook, patchlist_des2func);

HOOKPROTONHNO(patchlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct PatchEntry *pe = (struct PatchEntry *)msg->entry;

    if (pe) {
        msg->strings[0] = pe->pe_Address;
        msg->strings[1] = pe->pe_Library;
        msg->strings[2] = pe->pe_OffsetDec;
        msg->strings[3] = pe->pe_OffsetHex;
        msg->strings[4] = pe->pe_Function;
        msg->strings[5] = pe->pe_State;
        msg->strings[6] = pe->pe_Patcher;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtLibrary;
        msg->strings[2] = txtOffset;
        msg->strings[3] = txtHexadecimal;
        msg->strings[4] = txtFunction;
        msg->strings[5] = txtState;
        msg->strings[6] = txtPatcher;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
        msg->preparses[6] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(patchlist_dsp2hook, patchlist_dsp2func);

STATIC LONG patchlist_cmp2colfunc( struct PatchEntry *pe1,
                                   struct PatchEntry *pe2,
                                   ULONG column )
{
    LONG cmp;

    switch (column) {
        default:
        case 0: cmp = HexCompare(pe1->pe_Address, pe2->pe_Address); break;
        case 1: cmp = stricmp(pe1->pe_Library, pe2->pe_Library); break;
        case 2: cmp = IntegerCompare(pe1->pe_OffsetDec, pe2->pe_OffsetDec); break;
        case 3: cmp = HexCompare(pe1->pe_OffsetHex, pe2->pe_OffsetHex); break;
        case 4: cmp = stricmp(pe1->pe_Function, pe2->pe_Function); if (cmp == 0) cmp = stricmp(pe1->pe_Patcher, pe2->pe_Patcher); break;
        case 5: cmp = stricmp(pe1->pe_State, pe2->pe_State); break;
        case 6: cmp = stricmp(pe1->pe_Patcher, pe2->pe_Patcher); break;
    }

    return cmp;
}

#if !defined(__amigaos4__) && !defined(__AROS__)
STATIC LONG patchlist_cmpfunc( const struct Node *n1,
                               const struct Node *n2 )
{
    LONG cmp;

    cmp = stricmp(((struct PatchEntry *)n1)->pe_Library, ((struct PatchEntry *)n2)->pe_Library);
    if (cmp == 0) cmp = stricmp(((struct PatchEntry *)n1)->pe_Function, ((struct PatchEntry *)n2)->pe_Function);

    return cmp;
}
#endif

HOOKPROTONHNO(patchlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct PatchEntry *pe1, *pe2;
    ULONG col1, col2;

    pe1 = (struct PatchEntry *)msg->entry1;
    pe2 = (struct PatchEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = patchlist_cmp2colfunc(pe2, pe1, col1);
    } else {
        cmp = patchlist_cmp2colfunc(pe1, pe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = patchlist_cmp2colfunc(pe2, pe1, col2);
    } else {
        cmp = patchlist_cmp2colfunc(pe1, pe2, col2);
    }

    return cmp;
}
MakeStaticHook(patchlist_cmp2hook, patchlist_cmp2func);

#if !defined(__amigaos4__) && !defined(__AROS__)
STATIC void GetFunctionName( struct Library *lib,
                             LONG offset,
                             struct PatchEntry *pe )
{
    ULONG absOffset = abs(offset);

    if ((absOffset <= LIB_VECTSIZE * 6 && lib->lib_Node.ln_Type == NT_DEVICE) ||
        (absOffset <= LIB_VECTSIZE * 4 && lib->lib_Node.ln_Type == NT_LIBRARY)) {
        CONST_STRPTR help;

        switch (absOffset) {
            case LIB_VECTSIZE * 1: help = "Open";    break;
            case LIB_VECTSIZE * 2: help = "Close";   break;
            case LIB_VECTSIZE * 3: help = "Expunge"; break;
            case LIB_VECTSIZE * 4: help = "ExtFunc"; break;
            case LIB_VECTSIZE * 5: help = "BeginIO"; break;
            case LIB_VECTSIZE * 6: help = "AbortIO"; break;
            default:               help = ""; break;
        }
        stccpy(pe->pe_Function, help, sizeof(pe->pe_Function));
    } else {
        // better use pe_Library as name instead of lib->lib_Node.ln_Name, because the first one is already checked
        if (pe->pe_Library) {
            if (IdentifyBase) {
                switch (IdFunctionTags(pe->pe_Library, offset, IDTAG_FuncNameStr, pe->pe_Function,
                                                               IDTAG_StrLength, sizeof(pe->pe_Function),
                                                               TAG_DONE)) {
                    case IDERR_NOFD:
                        stccpy(pe->pe_Function, txtNoFDFile, sizeof(pe->pe_Function));
                        break;

                    case IDERR_NOMEM:
                        pe->pe_Function[0] = 0x00;
                        break;

                    case IDERR_OFFSET:
                        stccpy(pe->pe_Function, txtNoFunctionEntry, sizeof(pe->pe_Function));
                        break;
                }
            } else {
                stccpy(pe->pe_Function, txtNoIdentifyLibrary, sizeof(pe->pe_Function));
            }
        } else {
            stccpy(pe->pe_Function, txtNoFDFile, sizeof(pe->pe_Function));
        }
    }
}
#endif

#if !defined(__amigaos4__) && !defined(__AROS__)
STATIC void IterateSaferPatchesList( struct MinList *tmplist,
                                     struct PatchPort *pp )
{
    struct LibPatchList *ll;

    NoReqOn();

    ITERATE_LIST(&pp->pp_PatchList, struct LibPatchList *, ll) {
        struct LibPatchNode *ln;

        ITERATE_LIST(&ll->ll_PatchList, struct LibPatchNode *, ln) {
            struct LibPatchEntry *le;

            ITERATE_LIST(&ln->ln_PatchList, struct LibPatchEntry *, le) {
                struct PatchEntry *pe;

                if ((pe = AllocVec(sizeof(struct PatchEntry), MEMF_CLEAR)) != NULL) {
                    ULONG absOffset;

                    absOffset = abs(ln->ln_Offset);

                    pe->pe_Addr = le;
                    _snprintf(pe->pe_Address, sizeof(pe->pe_Address), ADDRESS_FORMAT, le->le_NewEntry);
                    pe->pe_LibraryAddr = ll->ll_LibBase;
                    stccpy(pe->pe_Library, nonetest(ll->ll_LibBase->lib_Node.ln_Name), sizeof(pe->pe_Library));
                    pe->pe_Offset = ln->ln_Offset;
                    _snprintf(pe->pe_OffsetDec, sizeof(pe->pe_OffsetDec), "-%ld", absOffset);
                    _snprintf(pe->pe_OffsetHex, sizeof(pe->pe_OffsetHex), "-$%04lx", absOffset);

                    GetFunctionName(ll->ll_LibBase, ln->ln_Offset, pe);

                    if (FLAG_IS_SET(le->le_Flags, LEF_DISABLED)) {
                        stccpy(pe->pe_State, txtPatchDisabled, sizeof(pe->pe_State));
                    } else if (FLAG_IS_SET(le->le_Flags, LEF_REMOVED)) {
                        stccpy(pe->pe_State, txtPatchRemoved, sizeof(pe->pe_State));
                    } else {
                        stccpy(pe->pe_State, txtPatchActive, sizeof(pe->pe_State));
                    }

                    stccpy(pe->pe_Type, "SAFERPATCHES", sizeof(pe->pe_Type));

                    stccpy(pe->pe_Patcher, nonetest(le->le_Patcher), sizeof(pe->pe_Patcher));

                    AddTail((struct List *)tmplist, (struct Node *)pe);
                }
            }
        }
    }

    NoReqOff();
}

STATIC void IteratePatchControlList( struct MinList *tmplist,
                                     struct Library *PatchBase )
{
    struct PatchControlNode *list;

    if ((list = GetPatchList()) != NULL) {
        struct PatchControlNode *pcn = list;

        NoReqOn();

        while (pcn) {
            struct PatchEntry *pe;

            if ((pe = AllocVec(sizeof(struct PatchEntry), MEMF_CLEAR)) != NULL) {
                ULONG absOffset;

                absOffset = abs(pcn->pcn_Offset);

                pe->pe_Addr = pcn;
                _snprintf(pe->pe_Address, sizeof(pe->pe_Address), ADDRESS_FORMAT, pcn->pcn_FunctionAddress);
                pe->pe_LibraryAddr = pcn->pcn_Library;
                stccpy(pe->pe_Library, nonetest(pcn->pcn_Library->lib_Node.ln_Name), sizeof(pe->pe_Library));
                pe->pe_Offset = pcn->pcn_Offset;
                _snprintf(pe->pe_OffsetDec, sizeof(pe->pe_OffsetDec), "-%ld", absOffset);
                _snprintf(pe->pe_OffsetHex, sizeof(pe->pe_OffsetHex), "-$%04lx", absOffset);

                GetFunctionName(pcn->pcn_Library, pcn->pcn_Offset, pe);

                stccpy(pe->pe_State, txtPatchActive, sizeof(pe->pe_State));
                stccpy(pe->pe_Patcher, nonetest((STRPTR)pcn->pcn_TaskName), sizeof(pe->pe_Patcher));
                stccpy(pe->pe_Type, "PATCHCONTROL", sizeof(pe->pe_Type));

                AddTail((struct List *)tmplist, (struct Node *)pe);
            }

            pcn = pcn->pcn_Next;
        }

        NoReqOff();
    }

    FreePatchList(list);
}

STATIC void IterateSetManList( struct MinList *tmplist,
                               struct SetManPort *sp )
{
    if (stricmp((STRPTR)sp->sp_Version, SETMAN_VERSION) == 0) {
        struct SetManNode *sn;

        ObtainSemaphore(&sp->sp_Semaphore);

        NoReqOn();

        ITERATE_LIST(&sp->sp_PatchList, struct SetManNode *, sn) {
            struct PatchEntry *pe;

            if ((pe = AllocVec(sizeof(struct PatchEntry), MEMF_CLEAR)) != NULL) {
                ULONG absOffset;

                absOffset = abs(sn->sn_Offset);

                pe->pe_Addr = sn;
                _snprintf(pe->pe_Address, sizeof(pe->pe_Address), ADDRESS_FORMAT, sn->sn_SetFunc);
                pe->pe_LibraryAddr = sn->sn_Library;
                stccpy(pe->pe_Library, nonetest(sn->sn_Library->lib_Node.ln_Name), sizeof(pe->pe_Library));
                pe->pe_Offset = sn->sn_Offset;
                _snprintf(pe->pe_OffsetDec, sizeof(pe->pe_OffsetDec), "-%ld", absOffset);
                _snprintf(pe->pe_OffsetHex, sizeof(pe->pe_OffsetHex), "-$%04lx", absOffset);

                GetFunctionName(sn->sn_Library, sn->sn_Offset, pe);

                if (sn->sn_SetFunc == NULL) {
                    stccpy(pe->pe_State, txtPatchRemoved, sizeof(pe->pe_State));
                } else if (sn->sn_Jmp1 == JMPCODE) {
                    stccpy(pe->pe_State, txtPatchActive, sizeof(pe->pe_State));
                } else {
                    stccpy(pe->pe_State, txtPatchDisabled, sizeof(pe->pe_State));
                }

                stccpy(pe->pe_Patcher, nonetest((STRPTR)sn->sn_TaskName), sizeof(pe->pe_Patcher));

                stccpy(pe->pe_Type, "SETMAN", sizeof(pe->pe_Type));

                AddTail((struct List *)tmplist, (struct Node *)pe);
            }
        }

        NoReqOff();

        ReleaseSemaphore(&sp->sp_Semaphore);
    } else {
        TEXT wantversion[4];

        stccpy(wantversion, SETMAN_VERSION, sizeof(wantversion));

        MyRequest(msgErrorContinue, msgWrongSetManVersion, *((ULONG *)wantversion), *((ULONG *)sp->sp_Version));
    }
}
#endif

STATIC void ReceiveList( void (* callback)( struct PatchEntry *pe, void *userData ),
                         void *userData )
{
    struct PatchEntry *pe;

    if ((pe = tbAllocVecPooled(globalPool, sizeof(struct PatchEntry))) != NULL) {
        if (SendDaemon("GetPatchList")) {
            while (ReceiveDecodedEntry(pe, sizeof(struct PatchEntry))) {
                callback(pe, userData);
            }
        }

        tbFreeVecPooled(globalPool, pe);
    }
}

STATIC void IterateList( UNUSED void (* callback)( struct PatchEntry *pe, void *userData ),
                         UNUSED void *userData,
                         UNUSED BOOL sort )
{
#if !defined(__amigaos4__) && !defined(__AROS__)
    struct MinList tmplist;
    struct PatchEntry *pe;
    struct PatchPort *pp;
    struct SetManPort *sp;
    struct Library *pc;

    NewList((struct List *)&tmplist);

    Forbid();
    pp = (struct PatchPort *)FindPort(PATCHPORT_NAME);
    sp = (struct SetManPort *)FindPort(SETMANPORT_NAME);
    pc = (struct Library *)OpenResource(PATCHRES_NAME);
    Permit();

    if (pp) {
        IterateSaferPatchesList(&tmplist, pp);
    } else if (pc) {
        IteratePatchControlList(&tmplist, pc);
    } else if (sp) {
        IterateSetManList(&tmplist, sp);
    } else {
        MyRequest(msgErrorContinue, msgControlProgramNotInstalled);
    }

    if (sort) SortLinkedList((struct List *)&tmplist, patchlist_cmpfunc);

    ITERATE_LIST(&tmplist, struct PatchEntry *, pe) {
        callback(pe, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
#endif
}

STATIC void UpdateCallback( struct PatchEntry *pe,
                            void *userData )
{
    struct PatchesCallbackUserData *ud = (struct PatchesCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, pe);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct PatchEntry *pe,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtPatchesPrintLine, pe->pe_Address, pe->pe_Library, pe->pe_OffsetHex, pe->pe_Function, pe->pe_State, pe->pe_Patcher);
}

STATIC void SendCallback( struct PatchEntry *pe,
                          UNUSED void *userData )
{
    SendEncodedEntry(pe, sizeof(struct PatchEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR patchlist, patchtext, patchcount, updateButton, printButton, enableButton, disableButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Patches",
        MUIA_Window_ID, MakeID('P','T','C','H'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&patchlist, MakeID('P','T','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR,BAR P=" MUIX_C ",BAR", &patchlist_con2hook, &patchlist_des2hook, &patchlist_dsp2hook, &patchlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&patchtext, &patchcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(enableButton   = MakeButton(txtEnable)),
                Child, (IPTR)(disableButton  = MakeButton(txtDisable)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct PatchesWinData *pwd = INST_DATA(cl, obj);
        APTR parent;

        pwd->pwd_PatchList = patchlist;
        pwd->pwd_PatchText = patchtext;
        pwd->pwd_PatchCount = patchcount;
        pwd->pwd_EnableButton = enableButton;
        pwd->pwd_DisableButton = disableButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtPatchesTitle, pwd->pwd_Title, sizeof(pwd->pwd_Title)));
        set(obj, MUIA_Window_DefaultObject, patchlist);
        set(patchlist, MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, pwd->pwd_EnableButton,
                                                          pwd->pwd_DisableButton,
                                                          NULL);

        DoMethod(parent,        MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,           MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(patchlist,     MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_PatchesWin_ListChange);
        DoMethod(updateButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PatchesWin_Update);
        DoMethod(printButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PatchesWin_Print);
        DoMethod(enableButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PatchesWin_Enable);
        DoMethod(disableButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PatchesWin_Disable);
        DoMethod(exitButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(patchlist,     MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);

    #if !defined(__amigaos4__) && !defined(__AROS__)
        Forbid();
        pwd->pwd_SetManPort = (struct SetManPort *)FindPort(SETMANPORT_NAME);
        Permit();
    #else
        pwd->pwd_SetManPort = NULL;
    #endif
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct PatchesWinData *pwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(pwd->pwd_PatchList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct PatchesWinData *pwd = INST_DATA(cl, obj);
    struct PatchesCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(pwd->pwd_PatchList, MUIA_NList_Quiet, TRUE);
    DoMethod(pwd->pwd_PatchList, MUIM_NList_Clear);

    ud.ud_List = pwd->pwd_PatchList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(pwd->pwd_PatchCount, ud.ud_Count);
    MySetContents(pwd->pwd_PatchText, "");

    set(pwd->pwd_PatchList, MUIA_NList_Quiet, FALSE);
    set(pwd->pwd_PatchList, MUIA_NList_Active, MUIV_NList_Active_Off);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, pwd->pwd_EnableButton,
                                                      pwd->pwd_DisableButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintPatches(NULL);

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct PatchesWinData *pwd = INST_DATA(cl, obj);
    struct PatchEntry *pe;

    if ((pe = (struct PatchEntry *)GetActiveEntry(pwd->pwd_PatchList)) != NULL) {
        MySetContents(pwd->pwd_PatchText, "%s %s/%s()", pe->pe_Address, pe->pe_Library, pe->pe_Function);
        if (!clientstate && stricmp(pe->pe_Type, "SAFERPATCHES") == 0) {
            DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, pwd->pwd_EnableButton,
                                                               pwd->pwd_DisableButton,
                                                               NULL);
        }
    }

    return 0;
}

STATIC IPTR mEnable( UNUSED struct IClass *cl,
                      UNUSED Object *obj,
                      UNUSED Msg msg )
{
#if !defined(__amigaos4__) && !defined(__AROS__)
    struct PatchesWinData *pwd = INST_DATA(cl, obj);
    ULONG id = MUIV_NList_NextSelected_Start;

    while (TRUE) {
        struct PatchEntry *pe;

        DoMethod(pwd->pwd_PatchList, MUIM_NList_NextSelected, &id);
        if (id == MUIV_NList_NextSelected_End) {
            set(pwd->pwd_PatchList, MUIA_NList_Active, MUIV_NList_Active_Off);
            break;
        }

        DoMethod(pwd->pwd_PatchList, MUIM_NList_GetEntry, id, &pe);
        if (pe) {
            struct LibPatchEntry *le = (struct LibPatchEntry *)pe->pe_Addr;

            if (stricmp(pe->pe_Type, "SAFERPATCHES") == 0) {
                if (FLAG_IS_SET(le->le_Flags, LEF_DISABLED) && FLAG_IS_SET(le->le_Flags, LEF_REMOVED)) {
                    SetFunction(pe->pe_LibraryAddr, pe->pe_Offset, (APTR)le->le_NewEntry);
                    CLEAR_FLAG(le->le_Flags, LEF_DISABLED);
                    stccpy(pe->pe_State, txtPatchActive, sizeof(pe->pe_State));
                    RedrawActiveEntry(pwd->pwd_PatchList);
                }
            } else if (stricmp(pe->pe_Type, "SETMAN") == 0) {
                struct SetManNode *sn = (struct SetManNode *)pe->pe_Addr;

                ObtainSemaphore(&pwd->pwd_SetManPort->sp_Semaphore);

                if (sn->sn_Jmp1 == BRACODE && sn->sn_SetFunc != NULL) {
                    sn->sn_Jmp1 = JMPCODE;
                    CacheClearU();

                    stccpy(pe->pe_State, txtPatchActive, sizeof(pe->pe_State));
                    RedrawActiveEntry(pwd->pwd_PatchList);
                }

                ReleaseSemaphore(&pwd->pwd_SetManPort->sp_Semaphore);
            }

            DoMethod(pwd->pwd_PatchList, MUIM_NList_Select, id, MUIV_NList_Select_Off, NULL);
            id = MUIV_NList_NextSelected_Start;
        }
    }
#endif

    return 0;
}

STATIC IPTR mDisable( UNUSED struct IClass *cl,
                       UNUSED Object *obj,
                       UNUSED Msg msg )
{
#if !defined(__amigaos4__) && !defined(__AROS__)
    struct PatchesWinData *pwd = INST_DATA(cl, obj);
    ULONG id = MUIV_NList_NextSelected_Start;
    ULONG disableMode = 1;

    while (TRUE) {
        struct PatchEntry *pe;

        DoMethod(pwd->pwd_PatchList, MUIM_NList_NextSelected, &id);
        if (id == MUIV_NList_NextSelected_End) {
            set(pwd->pwd_PatchList, MUIA_NList_Active, MUIV_NList_Active_Off);
            break;
        }
        if (disableMode == 0) break;

        DoMethod(pwd->pwd_PatchList, MUIM_NList_GetEntry, id, &pe);
        if (pe) {
            struct LibPatchEntry *le = (struct LibPatchEntry *)pe->pe_Addr;

            if (stricmp(pe->pe_Type, "SAFERPATCHES") == 0) {
                if (BIT_IS_CLEAR(le->le_Flags, LEF_DISABLED) && BIT_IS_CLEAR(le->le_Flags, LEF_REMOVED)) {
                    if (disableMode != 2) disableMode = MyRequest(msgYesAllNoAbort, msgWantToDisablePatches);

                    if (disableMode == 1 || disableMode == 2) {
                        SetFunction(pe->pe_LibraryAddr, pe->pe_Offset, (APTR)&le->le_Jmp);
                        SET_FLAG(le->le_Flags, LEF_DISABLED);
                        stccpy(pe->pe_State, txtPatchDisabled, sizeof(pe->pe_State));
                        RedrawActiveEntry(pwd->pwd_PatchList);
                    }
                }
            } else if (stricmp(pe->pe_Type, "SETMAN") == 0) {
                struct SetManNode *sn = (struct SetManNode *)pe->pe_Addr;

                ObtainSemaphore(&pwd->pwd_SetManPort->sp_Semaphore);

                if (sn->sn_Jmp1 == JMPCODE && sn->sn_SetFunc != NULL) {
                    if (disableMode != 2) disableMode = MyRequest(msgYesAllNoAbort, msgWantToDisablePatches);

                    if (disableMode == 1 || disableMode == 2) {

                        sn->sn_Jmp1 = BRACODE;
                        CacheClearU();

                        stccpy(pe->pe_State, txtPatchActive, sizeof(pe->pe_State));
                        RedrawActiveEntry(pwd->pwd_PatchList);
                    }
                }

                ReleaseSemaphore(&pwd->pwd_SetManPort->sp_Semaphore);
            }

            DoMethod(pwd->pwd_PatchList, MUIM_NList_Select, id, MUIV_NList_Select_Off, NULL);
            id = MUIV_NList_NextSelected_Start;
        }
    }
#endif

    return 0;
}

DISPATCHER(PatchesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                 return (mDispose(cl, obj, (APTR)msg));
        case MUIM_PatchesWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_PatchesWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_PatchesWin_Enable:     return (mEnable(cl, obj, (APTR)msg));
        case MUIM_PatchesWin_Disable:    return (mDisable(cl, obj, (APTR)msg));
        case MUIM_PatchesWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintPatches( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtPatchesPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendPatchList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakePatchesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct PatchesWinData), ENTRY(PatchesWinDispatcher));
}

