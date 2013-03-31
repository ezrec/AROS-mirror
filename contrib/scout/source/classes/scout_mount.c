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

struct MountsWinData {
    TEXT mwd_Title[WINDOW_TITLE_LENGTH];
    APTR mwd_MountList;
    APTR mwd_MountText;
    APTR mwd_MountCount;
    APTR mwd_MoreButton;
};

struct MountCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(mountlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct MountEntry));
}
MakeStaticHook(mountlist_con2hook, mountlist_con2func);

HOOKPROTONHNO(mountlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(mountlist_des2hook, mountlist_des2func);

HOOKPROTONHNO(mountlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct MountEntry *me = (struct MountEntry *)msg->entry;

    if (me) {
        msg->strings[0] = me->me_Address;
        msg->strings[1] = me->me_Name;
        msg->strings[2] = me->me_Heads;
        msg->strings[3] = me->me_Cylinders;
        msg->strings[4] = me->me_DiskState;
        msg->strings[5] = me->me_Filesystem;
        msg->strings[6] = me->me_DeviceName;
        msg->strings[7] = me->me_Unit;
        msg->strings[8] = me->me_Handler;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtName;
        msg->strings[2] = txtMountHeads;
        msg->strings[3] = txtMountCylinders;
        msg->strings[4] = txtMountState;
        msg->strings[5] = txtMountFilesystem;
        msg->strings[6] = txtMountDevice;
        msg->strings[7] = txtMountUnit;
        msg->strings[8] = txtMountHandler;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
        msg->preparses[6] = (STRPTR)MUIX_B;
        msg->preparses[7] = (STRPTR)MUIX_B;
        msg->preparses[8] = (STRPTR)MUIX_B;
    }
}

MakeStaticHook(mountlist_dsp2hook, mountlist_dsp2func);

STATIC LONG mountlist_cmp2colfunc( struct MountEntry *me1,
                                   struct MountEntry *me2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(me1->me_Address, me2->me_Address);
        case 1: return stricmp(me1->me_Name, me2->me_Name);
        case 2: return IntegerCompare(me1->me_Heads, me2->me_Heads);
        case 3: return IntegerCompare(me1->me_Cylinders, me2->me_Cylinders);
        case 4: return stricmp(me1->me_DiskState, me2->me_DiskState);
        case 5: return stricmp(me1->me_Filesystem, me2->me_Filesystem);
        case 6: return stricmp(me1->me_DeviceName, me2->me_DeviceName);
        case 7: return IntegerCompare(me1->me_Unit, me2->me_Unit);
        case 8: return stricmp(me1->me_Handler, me2->me_Handler);
    }
}

STATIC LONG mountlist_cmpfunc( const struct Node *n1,
                               const struct Node *n2 )
{
    return stricmp(((struct MountEntry *)n1)->me_Name, ((struct MountEntry *)n2)->me_Name);
}

HOOKPROTONHNO(mountlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct MountEntry *me1, *me2;
    ULONG col1, col2;

    me1 = (struct MountEntry *)msg->entry1;
    me2 = (struct MountEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = mountlist_cmp2colfunc(me2, me1, col1);
    } else {
        cmp = mountlist_cmp2colfunc(me1, me2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = mountlist_cmp2colfunc(me2, me1, col2);
    } else {
        cmp = mountlist_cmp2colfunc(me1, me2, col2);
    }

    return cmp;
}
MakeStaticHook(mountlist_cmp2hook, mountlist_cmp2func);

STRPTR GetDiskState( LONG state )
{
    ULONG i = 0;
    STATIC LONG diskstates[]= { ID_WRITE_PROTECTED, ID_VALIDATING, ID_VALIDATED, 0 };
    STATIC STRPTR diskstatestext[] = { NULL, NULL, NULL, NULL };

    diskstatestext[0] = txtReadOnly;
    diskstatestext[1] = txtValidating;
    diskstatestext[2] = txtReadWrite;

    while (diskstates[i]) {
        if (diskstates[i] == state) {
            return diskstatestext[i];
        }
        i++;
    }

    return NULL;
}

STRPTR GetDiskType( LONG type )
{
    ULONG i;
    union {
        LONG typeInt;
        TEXT typeChars[4];
    } _type;
    STATIC TEXT tmp[16];

#if defined(__AROS__)
    _type.typeInt = AROS_BE2LONG(type);
#else
    _type.typeInt = type;
#endif
    for (i = 0; i > 2; i++) {
        if (!isprint(_type.typeChars[i])) {
            break;
        }
    }

    if (i > 2) {
        _snprintf(tmp, sizeof(tmp), "$%08lx", type);
    } else {
        if (isprint(_type.typeChars[3])) {
            _snprintf(tmp, sizeof(tmp), "%lc%lc%lc%lc", _type.typeChars[0], _type.typeChars[1], _type.typeChars[2], _type.typeChars[3]);
        } else if (_type.typeChars[3] == 0x00) {
            _snprintf(tmp, sizeof(tmp), "%lc%lc%lc", _type.typeChars[0], _type.typeChars[1], _type.typeChars[2]);
        } else {
            _snprintf(tmp, sizeof(tmp), "%lc%lc%lc\\%ld", _type.typeChars[0], _type.typeChars[1], _type.typeChars[2], _type.typeChars[3]);
        }
    }

    return tmp;
}

BOOL IsValidFSSM( struct FileSysStartupMsg *fssm )
{
    // lots of tests to filter out faked FSSMs, thanks to Harry Sintonen for the sample source
    if ((IPTR)fssm > 0x400 &&
        points2ram(fssm) &&
        fssm->fssm_Unit <= 0x00ffffff) {
            UBYTE *p = BADDR(fssm->fssm_Device);

            if (p != NULL &&
                points2ram(p) &&
                p[0] != 0x00 &&
                p[1] != 0x00) {
                    struct DosEnvec *de = (struct DosEnvec *)BADDR(fssm->fssm_Environ);

                    if (de != NULL && points2ram(de)) {
                        return TRUE;
                    }
            }
    }

    return FALSE;
}

STATIC void ReceiveList( void (* callback)( struct MountEntry *me, void *userData ),
                         void *userData )
{
    struct MountEntry *me;

    if ((me = tbAllocVecPooled(globalPool, sizeof(struct MountEntry))) != NULL) {
        if (SendDaemon("GetMountList")) {
            while (ReceiveDecodedEntry(me, sizeof(struct MountEntry))) {
                callback(me, userData);
            }
        }

        tbFreeVecPooled(globalPool, me);
    }
}

STATIC void IterateList( void (* callback)( struct MountEntry *me, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct DosList *dol;
    struct InfoData *id;
    struct MinList tmplist;
    struct MountEntry *me;

    NewList((struct List *)&tmplist);

    NoReqOn();

    dol = LockDosList(LDF_READ | LDF_DEVICES);

    while ((dol = NextDosEntry(dol, LDF_DEVICES)) != NULL) {
        struct FileSysStartupMsg *fssm;
        BOOL validFSSM;

        fssm = (struct FileSysStartupMsg *)BADDR(dol->dol_misc.dol_handler.dol_Startup);
        validFSSM = IsValidFSSM(fssm);

        if ((me = AllocVec(sizeof(struct MountEntry), MEMF_CLEAR)) != NULL) {
            me->me_Device = dol;
            CopyMem(dol, &me->me_dol, sizeof(me->me_dol));

            _snprintf(me->me_Address, sizeof(me->me_Address), "$%08lx", dol);
            _snprintf(me->me_Name, sizeof(me->me_Name), "%b:", dol->dol_Name);
            stccpy(me->me_Unit, "---", sizeof(me->me_Unit));
            stccpy(me->me_Heads, "---", sizeof(me->me_Heads));
            stccpy(me->me_Cylinders, "---", sizeof(me->me_Cylinders));
            stccpy(me->me_DiskState, "---", sizeof(me->me_DiskState));
            stccpy(me->me_Filesystem, "---", sizeof(me->me_Filesystem));
            stccpy(me->me_Handler, nonetest(NULL), sizeof(me->me_Handler));
            stccpy(me->me_DeviceName, "---", sizeof(me->me_DeviceName));
            me->me_ValidFSSM = FALSE;

            if (validFSSM) {
                struct DosEnvec *env = (struct DosEnvec *)BADDR(fssm->fssm_Environ);
                ULONG size;

                size = MIN((env->de_TableSize + 1) * sizeof(ULONG), sizeof(me->me_fssmEnviron));
                CopyMem(env, &me->me_fssmEnviron, size);
                // better copy the device name, for the case it is not NUL-terminated
                _snprintf(me->me_Handler, sizeof(me->me_Handler), "%b", fssm->fssm_Device);
                _snprintf(me->me_DeviceName, sizeof(me->me_DeviceName), "%b", fssm->fssm_Device);
                _snprintf(me->me_Unit, sizeof(me->me_Unit), "%3ld", fssm->fssm_Unit);
                me->me_fssmUnit = fssm->fssm_Unit;
                me->me_fssmFlags = fssm->fssm_Flags;
                me->me_ValidFSSM = TRUE;
            }

            if (dol->dol_misc.dol_handler.dol_Handler != ZERO) {
                _snprintf(me->me_Handler, sizeof(me->me_Handler), "%b", dol->dol_misc.dol_handler.dol_Handler);
            }

            AddTail((struct List *)&tmplist, (struct Node *)me);
        }
    }

    UnLockDosList(LDF_READ | LDF_DEVICES);

    if ((id = tbAllocVecPooled(globalPool, sizeof(*id))) != NULL) {
        ITERATE_LIST(&tmplist, struct MountEntry *, me) {
            BPTR lock;
            BOOL infoSuccessful = FALSE;

	    if (IsFileSystem(me->me_Name)) {
	        if ((lock = Lock(me->me_Name, SHARED_LOCK)) != ZERO) {
                    if (Info(lock, id)) {
                        stccpy(me->me_DiskState, GetDiskState(id->id_DiskState), sizeof(me->me_DiskState));
                        stccpy(me->me_Filesystem, GetDiskType(id->id_DiskType), sizeof(me->me_DiskState));
                        infoSuccessful = TRUE;
                    }
                    UnLock(lock);
                }
	    }
            if (me->me_ValidFSSM) {
                struct DosEnvec *env = &me->me_fssmEnviron;

                if (env->de_Surfaces < 42 * 42) {
                    _snprintf(me->me_Heads, sizeof(me->me_Heads), "%6lD", env->de_Surfaces);
                }

                _snprintf(me->me_Cylinders, sizeof(me->me_Cylinders), "%12lD", env->de_HighCyl - env->de_LowCyl + 1);

                if (!infoSuccessful) {
                    struct MsgPort *mp;

                    if ((mp = CreateMsgPort()) != NULL) {
                        struct IOStdReq *io;

                        if ((io = (struct IOStdReq *)CreateIORequest(mp, sizeof(struct IOStdReq))) != NULL) {
                            if (OpenDevice(me->me_DeviceName, me->me_fssmUnit, (struct IORequest *)io, me->me_fssmFlags) == 0) {
                                io->io_Command = TD_CHANGESTATE;
                                DoIO((struct IORequest *)io);
                                if (io->io_Error == 0) {
                                    if (io->io_Actual) {
                                        stccpy(me->me_DiskState, txtNoDisk, sizeof(me->me_DiskState));
                                    } else {
                                        stccpy(me->me_DiskState, txtUnreadable, sizeof(me->me_DiskState));
                                    }
                                } else {
                                    stccpy(me->me_DiskState, txtNoDisk, sizeof(me->me_DiskState));
                                }

                                CloseDevice((struct IORequest *)io);
                            } else {
                               stccpy(me->me_DiskState, txtNoDisk, sizeof(me->me_DiskState));
                            }

                            DeleteIORequest((struct IORequest *)io);
                        }

                        DeleteMsgPort(mp);
                    }
                }

                if (env->de_TableSize >= DE_DOSTYPE) {
                   stccpy(me->me_Filesystem, GetDiskType(env->de_DosType), sizeof(me->me_Filesystem));
                }
            }
        }

        tbFreeVecPooled(globalPool, id);
    }

    NoReqOff();

    if (sort) SortLinkedList((struct List *)&tmplist, mountlist_cmpfunc);

    ITERATE_LIST(&tmplist, struct MountEntry *, me) {
        callback(me, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct MountEntry *me,
                            void *userData )
{
    struct MountCallbackUserData *ud = (struct MountCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, me);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct MountEntry *me,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtMountsPrintLine, me->me_Address, me->me_Name, me->me_Unit, me->me_Heads, me->me_Cylinders, me->me_DiskState, me->me_Filesystem, me->me_Handler);
}

STATIC void SendCallback( struct MountEntry *me,
                          UNUSED void *userData )
{
    SendEncodedEntry(me, sizeof(struct MountEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR mountlist, mounttext, mountcount, updateButton, printButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Mount",
        MUIA_Window_ID, MakeID('M','O','D','E'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&mountlist, MakeID('M','D','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR,BAR P=" MUIX_R ",BAR", &mountlist_con2hook, &mountlist_des2hook, &mountlist_dsp2hook, &mountlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview (&mounttext, &mountcount),

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
        struct MountsWinData *mwd = INST_DATA(cl, obj);
        APTR parent;

        mwd->mwd_MountList = mountlist;
        mwd->mwd_MountText = mounttext;
        mwd->mwd_MountCount = mountcount;
        mwd->mwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtMountsTitle, mwd->mwd_Title, sizeof(mwd->mwd_Title)));
        set(obj, MUIA_Window_DefaultObject, mountlist);
        set(moreButton, MUIA_Disabled, TRUE);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(mountlist,      MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_MountsWin_ListChange);
        DoMethod(mountlist,      MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_MountsWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MountsWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MountsWin_Print);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MountsWin_More);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(mountlist,      MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct MountsWinData *mwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(mwd->mwd_MountList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct MountsWinData *mwd = INST_DATA(cl, obj);
    struct MountCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(mwd->mwd_MountList, MUIA_NList_Quiet, TRUE);
    DoMethod(mwd->mwd_MountList, MUIM_NList_Clear);

    ud.ud_List = mwd->mwd_MountList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(mwd->mwd_MountCount, ud.ud_Count);
    MySetContents(mwd->mwd_MountText, "");

    set(mwd->mwd_MountList, MUIA_NList_Quiet, FALSE);
    set(mwd->mwd_MoreButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintMounts(NULL);

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct MountsWinData *mwd = INST_DATA(cl, obj);
        struct MountEntry *me;

        if ((me = (struct MountEntry *)GetActiveEntry(mwd->mwd_MountList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)MountsDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                End) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_MountsDetailWin_Mount, me);
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
    struct MountsWinData *mwd = INST_DATA(cl, obj);
    struct MountEntry *me;

    if ((me = (struct MountEntry *)GetActiveEntry(mwd->mwd_MountList)) != NULL) {
        MySetContents(mwd->mwd_MountText, "%s \"%s\"", me->me_Address, me->me_Name);
        if (!clientstate) set(mwd->mwd_MoreButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(MountsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                    return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                return (mDispose(cl, obj, (APTR)msg));
        case MUIM_MountsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_MountsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_MountsWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_MountsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintMounts( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtMountsPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendMountList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeMountsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MountsWinData), ENTRY(MountsWinDispatcher));
}

