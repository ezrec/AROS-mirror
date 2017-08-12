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

struct AudioModesWinData {
    TEXT amwd_Title[WINDOW_TITLE_LENGTH];
    APTR amwd_AudioModeList;
    APTR amwd_AudioModeText;
    APTR amwd_AudioModeCount;
    APTR amwd_MoreButton;
};

struct AudioModesCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

#if defined(__amigaos4__)
	#undef NEED_GLOBAL_AHIBASE
#elif defined(__MORPHOS__)
	#define NEED_GLOBAL_AHIBASE
#elif defined(__AROS__)
	#undef NEED_GLOBAL_AHIBASE
#else
	#if defined(__SASC)
		#undef NEED_GLOBAL_AHIBASE
	#else
		#define NEED_GLOBAL_AHIBASE
	#endif
#endif

#if defined(NEED_GLOBAL_AHIBASE)
struct Library *AHIBase;
#endif

HOOKPROTONHNO(amodelist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct AudioModeEntry));
}
MakeStaticHook(amodelist_con2hook, amodelist_con2func);

HOOKPROTONHNO(amodelist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(amodelist_des2hook, amodelist_des2func);

HOOKPROTONHNO(amodelist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct AudioModeEntry *ame = (struct AudioModeEntry *)msg->entry;

    if (ame) {
        msg->strings[0] = ame->ame_Id;
        msg->strings[1] = ame->ame_Name;
        msg->strings[2] = ame->ame_Bits;
        msg->strings[3] = ame->ame_MinFrequency;
        msg->strings[4] = ame->ame_MaxFrequency;
    } else {
        msg->strings[0] = txtAudioModeID;
        msg->strings[1] = txtName;
        msg->strings[2] = txtAudioModeBits;
        msg->strings[3] = txtAudioModeMinFreq;
        msg->strings[4] = txtAudioModeMaxFreq;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(amodelist_dsp2hook, amodelist_dsp2func);

STATIC LONG amodelist_cmp2colfunc( struct AudioModeEntry *ame1,
                                   struct AudioModeEntry *ame2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(ame1->ame_Id, ame2->ame_Id);
        case 1: return stricmp(ame1->ame_Name, ame2->ame_Name);
        case 2: return IntegerCompare(ame1->ame_Bits, ame2->ame_Bits);
        case 3: return IntegerCompare(ame1->ame_MinFrequency, ame2->ame_MinFrequency);
        case 4: return IntegerCompare(ame1->ame_MaxFrequency, ame2->ame_MaxFrequency);
    }
}

STATIC LONG amodelist_cmpfunc( const struct Node *n1,
                               const struct Node *n2 )
{
    return stricmp(((struct AudioModeEntry *)n1)->ame_Name, ((struct AudioModeEntry *)n2)->ame_Name);
}

HOOKPROTONHNO(amodelist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct AudioModeEntry *ame1, *ame2;
    ULONG col1, col2;

    ame1 = (struct AudioModeEntry *)msg->entry1;
    ame2 = (struct AudioModeEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = amodelist_cmp2colfunc(ame2, ame1, col1);
    } else {
        cmp = amodelist_cmp2colfunc(ame1, ame2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = amodelist_cmp2colfunc(ame2, ame1, col2);
    } else {
        cmp = amodelist_cmp2colfunc(ame1, ame2, col2);
    }

    return cmp;
}
MakeStaticHook(amodelist_cmp2hook, amodelist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct AudioModeEntry *ame, void *userData ),
                         void *userData )
{
    struct AudioModeEntry *ame;

    if ((ame = tbAllocVecPooled(globalPool, sizeof(struct AudioModeEntry))) != NULL) {
        if (SendDaemon("GetAudioModelist")) {
            while (ReceiveDecodedEntry(ame, sizeof(struct AudioModeEntry))) {
                callback(ame, userData);
            }
        }

        tbFreeVecPooled(globalPool, ame);
    }
}

STATIC void IterateList( void (* callback)( struct AudioModeEntry *ame, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MinList tmplist;
    struct AudioModeEntry *ame;
    struct MsgPort *mp;

    NewList((struct List *)&tmplist);

    if ((mp = CreateMsgPort()) != NULL) {
        struct AHIRequest *ahir;

        if ((ahir = (struct AHIRequest *)CreateIORequest(mp, sizeof(struct AHIRequest))) != NULL) {
            ahir->ahir_Version = 4;

            if (OpenDevice(AHINAME, AHI_NO_UNIT, (struct IORequest *)ahir, 0) == 0) {
            #if !defined(NEED_GLOBAL_AHIBASE)
                struct Library *AHIBase;
            #endif
            #if defined(__amigaos4__)
                struct AHIIFace *IAHI;
            #endif

                AHIBase = (struct Library *)ahir->ahir_Std.io_Device;

                if (GETINTERFACE(IAHI, AHIBase)) {
	                ULONG id = AHI_INVALID_ID;

                    while ((id = AHI_NextAudioID(id)) != AHI_INVALID_ID) {
                        if ((ame = AllocVec(sizeof(struct AudioModeEntry), MEMF_CLEAR)) != NULL) {
                            ULONG bits, minfreq, maxfreq, numfreqs;

                            ame->ame_ModeID = id;
                            _snprintf(ame->ame_Id, sizeof(ame->ame_Id), ADDRESS_FORMAT, id);

                            AHI_GetAudioAttrs(id, NULL, AHIDB_Name, (IPTR)ame->ame_Name,
                                                        AHIDB_BufferLen, sizeof(ame->ame_Name),
                                                        TAG_DONE);
                            AHI_GetAudioAttrs(id, NULL, AHIDB_Bits, (IPTR)&bits,
                                                        AHIDB_MinMixFreq, (IPTR)&minfreq,
                                                        AHIDB_MaxMixFreq, (IPTR)&maxfreq,
                                                        AHIDB_Frequencies, (IPTR)&numfreqs,
                                                        TAG_DONE);
                            _snprintf(ame->ame_Bits, sizeof(ame->ame_Bits), "%3lD", bits);
                            if (numfreqs > 1) {
                                _snprintf(ame->ame_MinFrequency, sizeof(ame->ame_MinFrequency), "%6lD Hz", minfreq);
                                _snprintf(ame->ame_MaxFrequency, sizeof(ame->ame_MaxFrequency), "%6lD Hz", maxfreq);
                            } else {
                                stccpy(ame->ame_MinFrequency, "---", sizeof(ame->ame_MinFrequency));
                                stccpy(ame->ame_MaxFrequency, "---", sizeof(ame->ame_MaxFrequency));
                            }

                            AddTail((struct List *)&tmplist, (struct Node *)ame);
                        }
                    }

                    DROPINTERFACE(IAHI);
                }

                CloseDevice((struct IORequest *)ahir);

            #if defined(NEED_GLOBAL_AHIBASE)
                AHIBase = NULL;
            #endif
            } else {
                MyRequest(msgErrorContinue, msgCantOpenAHIDevice, AHINAME, ahir->ahir_Version);
            }

            DeleteIORequest((struct IORequest *)ahir);
        }

        DeleteMsgPort(mp);
    }

    if (sort) SortLinkedList((struct List *)&tmplist, amodelist_cmpfunc);

    ITERATE_LIST(&tmplist, struct AudioModeEntry *, ame) {
        callback(ame, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct AudioModeEntry *ame,
                            void *userData )
{
    struct AudioModesCallbackUserData *ud = (struct AudioModesCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, ame);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct AudioModeEntry *ame,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtAudioModesPrintLine, ame->ame_Id, ame->ame_Bits, ame->ame_MinFrequency, ame->ame_MaxFrequency, ame->ame_Name);
}

STATIC void SendCallback( struct AudioModeEntry *ame,
                          UNUSED void *userData )
{
    SendEncodedEntry(ame, sizeof(struct AudioModeEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR amodelist, amodetext, amodecount, updateButton, printButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "AudioModes",
        MUIA_Window_ID, MakeID('A','M','D','E'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&amodelist, MakeID('A','M','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR P=" MUIX_R, &amodelist_con2hook, &amodelist_des2hook, &amodelist_dsp2hook, &amodelist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&amodetext, &amodecount),

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
        struct AudioModesWinData *amwd = INST_DATA(cl, obj);
        APTR parent;

        amwd->amwd_AudioModeList = amodelist;
        amwd->amwd_AudioModeText = amodetext;
        amwd->amwd_AudioModeCount = amodecount;
        amwd->amwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtAudioModesTitle, amwd->amwd_Title, sizeof(amwd->amwd_Title)));
        set(obj, MUIA_Window_DefaultObject, amodelist);
        set(moreButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(amodelist,    MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_AudioModesWin_ListChange);
        DoMethod(amodelist,    MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_AudioModesWin_More);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_AudioModesWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_AudioModesWin_Print);
        DoMethod(moreButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_AudioModesWin_More);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(amodelist,    MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct AudioModesWinData *amwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(amwd->amwd_AudioModeList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct AudioModesWinData *amwd = INST_DATA(cl, obj);
    struct AudioModesCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(amwd->amwd_AudioModeList, MUIA_NList_Quiet, TRUE);
    DoMethod(amwd->amwd_AudioModeList, MUIM_NList_Clear);

    ud.ud_List = amwd->amwd_AudioModeList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(amwd->amwd_AudioModeCount, ud.ud_Count);
    MySetContents(amwd->amwd_AudioModeText, "");

    set(amwd->amwd_AudioModeList, MUIA_NList_Quiet, FALSE);
    set(amwd->amwd_MoreButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintAudioModes(NULL);

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct AudioModesWinData *amwd = INST_DATA(cl, obj);
        struct AudioModeEntry *ame;

        if ((ame = (struct AudioModeEntry *)GetActiveEntry(amwd->amwd_AudioModeList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)(AudioModesDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                End)) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_AudioModesDetailWin_AudioMode, ame);
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
    struct AudioModesWinData *amwd = INST_DATA(cl, obj);
    struct AudioModeEntry *ame;

    if ((ame = (struct AudioModeEntry *)GetActiveEntry(amwd->amwd_AudioModeList)) != NULL) {
        MySetContents(amwd->amwd_AudioModeText, "%s \"%s\"", ame->ame_Id, ame->ame_Name);
        if (!clientstate) set(amwd->amwd_MoreButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(AudioModesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                        return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                    return (mDispose(cl, obj, (APTR)msg));
        case MUIM_AudioModesWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_AudioModesWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_AudioModesWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_AudioModesWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintAudioModes( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtAudioModesPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendAudioModeList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeAudioModesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct AudioModesWinData), ENTRY(AudioModesWinDispatcher));
}

