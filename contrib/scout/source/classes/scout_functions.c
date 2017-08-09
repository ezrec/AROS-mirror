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
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#include "system_headers.h"

struct FunctionsWinData {
    TEXT fwd_Title[WINDOW_TITLE_LENGTH];
    APTR fwd_FunctionsList;
    APTR fwd_FunctionsText;
    APTR fwd_FunctionsCount;
    APTR fwd_DisassembleButton;
};

struct MUIP_FunctionsWin_ShowFunctionsMessage {
    STACKED ULONG MethodID;
    STACKED ULONG nodeType;
    STACKED APTR base;
    STACKED STRPTR name;
};

struct FunctionsEntry {
    APTR fe_Address;
    TEXT fe_AddressStr[ADDRESS_LENGTH];
    ULONG fe_Offset;
    TEXT fe_OffsetDec[ADDRESS_LENGTH];
    TEXT fe_OffsetHex[ADDRESS_LENGTH];
    TEXT fe_FuncName[FILENAME_LENGTH];
};

HOOKPROTONHNO(funclist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct FunctionsEntry));
}
MakeStaticHook(funclist_con2hook, funclist_con2func);

HOOKPROTONHNO(funclist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(funclist_des2hook, funclist_des2func);

HOOKPROTONHNO(funclist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct FunctionsEntry *fe = (struct FunctionsEntry *)msg->entry;

    if (fe) {
        msg->strings[0] = fe->fe_OffsetDec;
        msg->strings[1] = fe->fe_OffsetHex;
        msg->strings[2] = fe->fe_AddressStr;
        msg->strings[3] = fe->fe_FuncName;
    } else {
        msg->strings[0] = txtOffset;
        msg->strings[1] = txtHexadecimal;
        msg->strings[2] = txtAddress;
        msg->strings[3] = txtName;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(funclist_dsp2hook, funclist_dsp2func);

STATIC LONG funclist_cmp2colfunc( struct FunctionsEntry *fe1,
                                  struct FunctionsEntry *fe2,
                                  ULONG column )
{
    switch (column) {
        default:
        case 0:
        case 1: return (IPTR)fe1->fe_Offset - (IPTR)fe2->fe_Offset;
        case 2: return (IPTR)fe1->fe_Address - (IPTR)fe2->fe_Address;
        case 3: return stricmp(fe1->fe_FuncName, fe2->fe_FuncName);
    }
}

HOOKPROTONHNO(funclist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct FunctionsEntry *fe1, *fe2;
    ULONG col1, col2;

    fe1 = (struct FunctionsEntry *)msg->entry1;
    fe2 = (struct FunctionsEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = funclist_cmp2colfunc(fe2, fe1, col1);
    } else {
        cmp = funclist_cmp2colfunc(fe1, fe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = funclist_cmp2colfunc(fe2, fe1, col2);
    } else {
        cmp = funclist_cmp2colfunc(fe1, fe2, col2);
    }

    return cmp;
}
MakeStaticHook(funclist_cmp2hook, funclist_cmp2func);

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR funclist, functext, funccount, disasmButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_Window_ID, MakeID('.','L','F','T'),
        WindowContents, VGroup,
            Child, (IPTR)MyNListviewObject(&funclist, MakeID('F','T','L','V'), "BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR,BAR", &funclist_con2hook, &funclist_des2hook, &funclist_dsp2hook, &funclist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&functext, &funccount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(disasmButton = MakeButton(txtDisassemble)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct FunctionsWinData *fwd = INST_DATA(cl, obj);
        APTR parent;

        fwd->fwd_FunctionsList = funclist;
        fwd->fwd_FunctionsText = functext;
        fwd->fwd_FunctionsCount = funccount;
        fwd->fwd_DisassembleButton = disasmButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, funclist);
        set(disasmButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(funclist,     MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_FunctionsWin_Disassemble);
        DoMethod(funclist,     MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_FunctionsWin_ListChange);
        DoMethod(disasmButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_FunctionsWin_Disassemble);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(funclist,     MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct FunctionsWinData *fwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(fwd->fwd_FunctionsList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mShowFunctions( struct IClass *cl,
                             Object *obj,
                             Msg msg )
{
    struct FunctionsWinData *fwd = INST_DATA(cl, obj);
    struct MUIP_FunctionsWin_ShowFunctionsMessage *sfm = (struct MUIP_FunctionsWin_ShowFunctionsMessage *)msg;

    if (sfm->nodeType != MUIV_FunctionsWin_NodeType_Resource ||
        (stricmp(sfm->name, FSRNAME) != 0 &&
         //stricmp(sfm->name, CARDRESNAME) != 0 &&
         stricmp(sfm->name, KEYMAPRESNAME) != 0)) {

        struct FunctionsEntry *fe;

        if ((fe = tbAllocVecPooled(globalPool, sizeof(struct FunctionsEntry))) != NULL) {
            struct Library *lib = (struct Library *)sfm->base;
            ULONG max, offset;
            BOOL useIdLib = (IdentifyBase != NULL);

            max = lib->lib_NegSize;

            set(fwd->fwd_FunctionsList, MUIA_NList_Quiet, TRUE);

            NoReqOn();

            for (offset = LIB_VECTSIZE; offset <= max; offset += LIB_VECTSIZE) {
#ifdef __AROS__
                fe->fe_Address = __AROS_GETVECADDR (lib, offset / LIB_VECTSIZE);
                
                if (points2ram(fe->fe_Address)) {
                    _snprintf(fe->fe_AddressStr, sizeof(fe->fe_AddressStr), MUIX_PH "$%08lx" MUIX_PT, fe->fe_Address);
                } else {
                    _snprintf(fe->fe_AddressStr, sizeof(fe->fe_AddressStr), "$%08lx", fe->fe_Address);
                }
#else
                struct JumpEntry *je;

                je = (struct JumpEntry *)((UBYTE *)lib - offset);

                #ifdef __MORPHOS__
                if (je->je_JMPInstruction == 0x4ef9 || je->je_JMPInstruction == 0x4afc) {
                #else
                if (je->je_JMPInstruction == 0x4ef9) {
                #endif
                    fe->fe_Address = je->je_JumpAddress;
                    if (points2ram(je->je_JumpAddress)) {
                        _snprintf(fe->fe_AddressStr, sizeof(fe->fe_AddressStr), MUIX_PH "$%08lx" MUIX_PT, je->je_JumpAddress);
                    } else {
                        _snprintf(fe->fe_AddressStr, sizeof(fe->fe_AddressStr), "$%08lx", je->je_JumpAddress);
                    }
                } else {
                    fe->fe_Address = (APTR)-1;
                    stccpy(fe->fe_AddressStr, txtNoJump, sizeof(fe->fe_AddressStr));
                }
#endif

                fe->fe_Offset = offset;
                _snprintf(fe->fe_OffsetDec, sizeof(fe->fe_OffsetDec), "-%ld", offset);
                _snprintf(fe->fe_OffsetHex, sizeof(fe->fe_OffsetHex), "-$%04lx", offset);

                if ((offset <= LIB_VECTSIZE * 6 && sfm->nodeType == MUIV_FunctionsWin_NodeType_Device) ||
                    (offset <= LIB_VECTSIZE * 4 && sfm->nodeType == MUIV_FunctionsWin_NodeType_Library)) {
                    CONST_STRPTR help;

                    switch (offset) {
                        case LIB_VECTSIZE * 1: help = "Open";    break;
                        case LIB_VECTSIZE * 2: help = "Close";   break;
                        case LIB_VECTSIZE * 3: help = "Expunge"; break;
                        case LIB_VECTSIZE * 4: help = "ExtFunc"; break;
                        case LIB_VECTSIZE * 5: help = "BeginIO"; break;
                        case LIB_VECTSIZE * 6: help = "AbortIO"; break;
                        default:               help = ""; break;
                    }
                    stccpy(fe->fe_FuncName, (TEXT *)help, sizeof(fe->fe_FuncName));
                } else if (useIdLib) {
                    ULONG error;
                    ULONG id_offset;

                    if (sfm->name) {
#ifdef __AROS__
                        /* identify.library on AROS assumes LIB_VECTSIZE of 6 */
                        id_offset = (offset / LIB_VECTSIZE) * 6;
#else
                        id_offset = offset;
#endif
                        if (stricmp(sfm->name, CIAANAME) == 0 || stricmp(sfm->name, CIABNAME) == 0) {
                            error = IdFunctionTags((STRPTR)"cia.resource", id_offset,
                                                                                      IDTAG_FuncNameStr, (IPTR)fe->fe_FuncName,
                                                                                      IDTAG_StrLength, sizeof(fe->fe_FuncName),
                                                                                      TAG_DONE);
                        } else if (stricmp(sfm->name, CARDRESNAME) == 0) {
                            error = IdFunctionTags((STRPTR)"cardres.resource", id_offset,
                                                                                      IDTAG_FuncNameStr, (IPTR)fe->fe_FuncName,
                                                                                      IDTAG_StrLength, sizeof(fe->fe_FuncName),
                                                                                      TAG_DONE);
                        } else {
                            error = IdFunctionTags(sfm->name, id_offset,
                                                                                      IDTAG_FuncNameStr, (IPTR)fe->fe_FuncName,
                                                                                      IDTAG_StrLength, sizeof(fe->fe_FuncName),
                                                                                      TAG_DONE);
                        }
                        switch (error) {
                            case IDERR_NOFD:
                                stccpy(fe->fe_FuncName, txtNoFDFile, sizeof(fe->fe_FuncName));
                                break;

                            case IDERR_NOMEM:
                                fe->fe_FuncName[0] = 0x00;
                                useIdLib = FALSE;
                                break;

                            case IDERR_OFFSET:
                                stccpy(fe->fe_FuncName, txtNoFunctionEntry, sizeof(fe->fe_FuncName));
                                break;
                        }
                    } else {
                        stccpy(fe->fe_FuncName, txtNoFDFile, sizeof(fe->fe_FuncName));
                    }
                } else {
                    fe->fe_FuncName[0] = 0x00;
                }

                InsertBottomEntry(fwd->fwd_FunctionsList, fe);
            }

            NoReqOff();

            set(fwd->fwd_FunctionsList, MUIA_NList_Quiet, FALSE);
            SetCountText(fwd->fwd_FunctionsCount, max / LIB_VECTSIZE);

            set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtFunctionsTitle, sfm->name, fwd->fwd_Title, sizeof(fwd->fwd_Title)));
        }

        tbFreeVecPooled(globalPool, fe);
    }

    set(obj, MUIA_Window_Open, TRUE);

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct FunctionsWinData *fwd = INST_DATA(cl, obj);
    struct FunctionsEntry *fe;

    if ((fe = (struct FunctionsEntry *)GetActiveEntry(fwd->fwd_FunctionsList)) != NULL) {
        if (fe->fe_FuncName[0] == '<') {
            MySetContents(fwd->fwd_FunctionsText, "%s %s", fe->fe_OffsetDec, fe->fe_FuncName);
        } else {
            MySetContents(fwd->fwd_FunctionsText, "%s %s()", fe->fe_OffsetDec, fe->fe_FuncName);
        }
        set(fwd->fwd_DisassembleButton, MUIA_Disabled, (clientstate || fe->fe_Address == (APTR)-1 || fe->fe_Address == NULL));
    }

    return 0;
}

STATIC IPTR mDisassemble( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    struct FunctionsWinData *fwd = INST_DATA(cl, obj);
    struct FunctionsEntry *fe;

    if ((fe = (struct FunctionsEntry *)GetActiveEntry(fwd->fwd_FunctionsList)) != NULL) {
        APTR disasmWin;

        if ((disasmWin = (Object *)DisassemblerWindowObject,
                MUIA_Window_ParentWindow, (IPTR)obj,
                MUIA_DisassemblerWin_Address, (IPTR)fe->fe_Address,
            End) != NULL) {
            COLLECT_RETURNIDS;
            DoMethod(disasmWin, MUIM_DisassemblerWin_Disassemble);
            set(disasmWin, MUIA_Window_Open, TRUE);
            REISSUE_RETURNIDS;
        }
    }

    return 0;
}

DISPATCHER(FunctionsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                          return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                      return (mDispose(cl, obj, (APTR)msg));
        case MUIM_FunctionsWin_ShowFunctions: return (mShowFunctions(cl, obj, (APTR)msg));
        case MUIM_FunctionsWin_ListChange:    return (mListChange(cl, obj, (APTR)msg));
        case MUIM_FunctionsWin_Disassemble:   return (mDisassemble(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

APTR MakeFunctionsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct FunctionsWinData), ENTRY(FunctionsWinDispatcher));
}

