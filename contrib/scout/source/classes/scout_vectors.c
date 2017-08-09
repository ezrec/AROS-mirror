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

struct VectorsWinData {
    TEXT vwd_Title[WINDOW_TITLE_LENGTH];
    APTR vwd_ResetVectButton[6];
    APTR vwd_AutoVectButton[7];
    APTR vwd_IntVectButton[16];
};

struct VectorsCallbackUserData {
    APTR ud_ResetVectButton[6];
    APTR ud_AutoVectButton[7];
    APTR ud_IntVectButton[16];
};

struct MUIP_VectorsWin_Disassemble {
    STACKED ULONG methodID;
    STACKED APTR button;
};

void ClearResetVectors( void )
{
    SysBase->ColdCapture = NULL;
    SysBase->CoolCapture = NULL;
    SysBase->WarmCapture = NULL;
    SysBase->KickMemPtr = NULL;
    SysBase->KickTagPtr = NULL;
    SysBase->KickCheckSum = NULL;
}

STATIC void ReceiveList( void (* callback)( struct VectorEntry *ve, void *userData ),
                         void *userData )
{
    struct VectorEntry *ve;

    if ((ve = tbAllocVecPooled(globalPool, sizeof(struct VectorEntry))) != NULL) {
        if (SendDaemon("Vectors")) {
            while (ReceiveDecodedEntry(ve, sizeof(struct VectorEntry))) {
                callback(ve, userData);
            }
        }

        tbFreeVecPooled(globalPool, ve);
    }
}

STATIC void IterateList( void (* callback)( struct VectorEntry *ve, void *userData ),
                         void *userData )
{
    struct VectorEntry *ve;

    if ((ve = tbAllocVecPooled(globalPool, sizeof(struct VectorEntry))) != NULL) {
        UBYTE *vbr = NULL;
	ULONG vec;
#if !defined(__AROS__)
        vbr = (UBYTE *)GetVBR();
#endif

        Forbid();

        _snprintf(ve->ve_ResetVectors[0], sizeof(ve->ve_ResetVectors[0]), "$%08lx", SysBase->ColdCapture);
        _snprintf(ve->ve_ResetVectors[1], sizeof(ve->ve_ResetVectors[1]), "$%08lx", SysBase->CoolCapture);
        _snprintf(ve->ve_ResetVectors[2], sizeof(ve->ve_ResetVectors[2]), "$%08lx", SysBase->WarmCapture);
        _snprintf(ve->ve_ResetVectors[3], sizeof(ve->ve_ResetVectors[3]), "$%08lx", SysBase->KickMemPtr);
        _snprintf(ve->ve_ResetVectors[4], sizeof(ve->ve_ResetVectors[4]), "$%08lx", SysBase->KickTagPtr);
        _snprintf(ve->ve_ResetVectors[5], sizeof(ve->ve_ResetVectors[5]), "$%08lx", SysBase->KickCheckSum);

        for (vec = 0; vec < 7; vec++) {
#if !defined(__mc68000)
            if (amigaOS4 || arOS) {
                // the 68k interrupt vectors don't exist anymore in AmigaOS4/AmigaOne
                _snprintf(ve->ve_AutoVectors[vec], sizeof(ve->ve_AutoVectors[vec]), "$%08lx", 0);
            } else
#endif
            {
                // classic Amigas and MorphOS still got them
                _snprintf(ve->ve_AutoVectors[vec], sizeof(ve->ve_AutoVectors[vec]), "$%08lx", *((ULONG *)(vbr + 0x0064 + vec * sizeof(ULONG))));
            }
        }

        for (vec = 0; vec < 16; vec++) {
            _snprintf(ve->ve_IntVectors[vec], sizeof(ve->ve_IntVectors[vec]), "$%08lx", SysBase->IntVects[vec].iv_Code);
        }

        Permit();

        callback(ve, userData);

        tbFreeVecPooled(globalPool, ve);
    }
}

STATIC void UpdateCallback( struct VectorEntry *ve,
                            void *userData )
{
    struct VectorsCallbackUserData *ud = (struct VectorsCallbackUserData *)userData;
    ULONG vec;
    IPTR code;

    for (vec = 0; vec < 3; vec++) {
        IsUHex(ve->ve_ResetVectors[vec], &code);
        set(ud->ud_ResetVectButton[vec], MUIA_DisassemblerButton_Address, code);
    }
    for (vec = 3; vec < 6; vec++) {
        IsUHex(ve->ve_ResetVectors[vec], &code);
        MySetContents(ud->ud_ResetVectButton[vec], MUIX_C "$%08lx", code);
    }
    for (vec = 0; vec < 7; vec++) {
        IsUHex(ve->ve_AutoVectors[vec], &code);
        set(ud->ud_AutoVectButton[vec], MUIA_DisassemblerButton_Address, code);
    }
    for (vec = 0; vec < 16; vec++) {
        IsUHex(ve->ve_IntVectors[vec], &code);
        set(ud->ud_IntVectButton[vec], MUIA_DisassemblerButton_Address, code);
    }
}

STATIC void PrintCallback( struct VectorEntry *ve,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtVectorsPrintHeader, txtResetVectors);
    PrintFOneLine((BPTR)userData, txtVectorsPrintLineReset, ve->ve_ResetVectors[0], ve->ve_ResetVectors[3],
                                                            ve->ve_ResetVectors[1], ve->ve_ResetVectors[4],
                                                            ve->ve_ResetVectors[2], ve->ve_ResetVectors[5]);

    PrintFOneLine((BPTR)userData, txtVectorsPrintHeader, txtAutoVectorInterrupts);
    PrintFOneLine((BPTR)userData, txtVectorsPrintLineAuto, ve->ve_AutoVectors[0], ve->ve_AutoVectors[4],
                                                           ve->ve_AutoVectors[1], ve->ve_AutoVectors[5],
                                                           ve->ve_AutoVectors[2], ve->ve_AutoVectors[6],
                                                           ve->ve_AutoVectors[3]);

    PrintFOneLine((BPTR)userData, txtVectorsPrintHeader, txtInterruptVectors);
    PrintFOneLine((BPTR)userData, txtVectorsPrintLineVectors, ve->ve_IntVectors[ 0], ve->ve_IntVectors[ 8],
                                                              ve->ve_IntVectors[ 1], ve->ve_IntVectors[ 9],
                                                              ve->ve_IntVectors[ 2], ve->ve_IntVectors[10],
                                                              ve->ve_IntVectors[ 3], ve->ve_IntVectors[11],
                                                              ve->ve_IntVectors[ 4], ve->ve_IntVectors[12],
                                                              ve->ve_IntVectors[ 5], ve->ve_IntVectors[13],
                                                              ve->ve_IntVectors[ 6], ve->ve_IntVectors[14],
                                                              ve->ve_IntVectors[ 7], ve->ve_IntVectors[15]);
}

STATIC void SendCallback( struct VectorEntry *ve,
                          UNUSED void *userData )
{
    SendEncodedEntry(ve, sizeof(struct VectorEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    STATIC STRPTR vectorPages[4] = { NULL };
    APTR pages, resetVectButton[6], autoVectButton[7], intVectButton[16], updateButton, printButton, exitButton;

    vectorPages[0] = txtResetVectors;
    vectorPages[1] = txtAutoVectorInterrupts;
    vectorPages[2] = txtInterruptVectors;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Vectors",
        MUIA_Window_ID, MakeID('V','E','C','T'),
        WindowContents, VGroup,

            Child, (IPTR)(pages = (Object *)RegisterGroup(vectorPages),
                MUIA_CycleChain, TRUE,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtVectorColdCapture),
                            Child, (IPTR)(resetVectButton[0] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorCoolCapture),
                            Child, (IPTR)(resetVectButton[1] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorWarmCapture),
                            Child, (IPTR)(resetVectButton[2] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorKickMemPtr),
                            Child, (IPTR)(resetVectButton[3] = MyTextObject()),
                            Child, (IPTR)MyLabel2(txtVectorKickTagPtr),
                            Child, (IPTR)(resetVectButton[4] = MyTextObject()),
                            Child, (IPTR)MyLabel2(txtVectorKickChecksum),
                            Child, (IPTR)(resetVectButton[5] = MyTextObject()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtVectorLevel1),
                            Child, (IPTR)(autoVectButton[0] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorLevel2),
                            Child, (IPTR)(autoVectButton[1] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorLevel3),
                            Child, (IPTR)(autoVectButton[2] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorLevel4),
                            Child, (IPTR)(autoVectButton[3] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorLevel5),
                            Child, (IPTR)(autoVectButton[4] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorLevel6),
                            Child, (IPTR)(autoVectButton[5] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorLevel7),
                            Child, (IPTR)(autoVectButton[6] = (Object *)DisassemblerButtonObject, End),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtVectorInt00),
                            Child, (IPTR)(intVectButton[ 0] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt01),
                            Child, (IPTR)(intVectButton[ 1] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt02),
                            Child, (IPTR)(intVectButton[ 2] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt03),
                            Child, (IPTR)(intVectButton[ 3] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt04),
                            Child, (IPTR)(intVectButton[ 4] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt05),
                            Child, (IPTR)(intVectButton[ 5] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt06),
                            Child, (IPTR)(intVectButton[ 6] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt07),
                            Child, (IPTR)(intVectButton[ 7] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt08),
                            Child, (IPTR)(intVectButton[ 8] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt09),
                            Child, (IPTR)(intVectButton[ 9] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt10),
                            Child, (IPTR)(intVectButton[10] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt11),
                            Child, (IPTR)(intVectButton[11] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt12),
                            Child, (IPTR)(intVectButton[12] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt13),
                            Child, (IPTR)(intVectButton[13] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt14),
                            Child, (IPTR)(intVectButton[14] = (Object *)DisassemblerButtonObject, End),
                            Child, (IPTR)MyLabel2(txtVectorInt15),
                            Child, (IPTR)(intVectButton[15] = (Object *)DisassemblerButtonObject, End),
                        End,
                    End,
                End,
            End),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct VectorsWinData *vwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(resetVectButton, vwd->vwd_ResetVectButton, sizeof(vwd->vwd_ResetVectButton));
        CopyMemQuick(autoVectButton, vwd->vwd_AutoVectButton, sizeof(vwd->vwd_AutoVectButton));
        CopyMemQuick(intVectButton, vwd->vwd_IntVectButton, sizeof(vwd->vwd_IntVectButton));

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtVectorsTitle, vwd->vwd_Title, sizeof(vwd->vwd_Title)));
        set(obj, MUIA_Window_ActiveObject, pages);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     1, MUIM_VectorsWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     1, MUIM_VectorsWin_Print);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct VectorsWinData *vwd = INST_DATA(cl, obj);
    struct VectorsCallbackUserData ud;

    CopyMemQuick(vwd->vwd_ResetVectButton, ud.ud_ResetVectButton, sizeof(vwd->vwd_ResetVectButton));
    CopyMemQuick(vwd->vwd_AutoVectButton, ud.ud_AutoVectButton, sizeof(vwd->vwd_AutoVectButton));
    CopyMemQuick(vwd->vwd_IntVectButton, ud.ud_IntVectButton, sizeof(vwd->vwd_IntVectButton));

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintVectors(NULL);

    return 0;
}

DISPATCHER(VectorsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                 return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:             return (mDispose(cl, obj, (APTR)msg));
        case MUIM_VectorsWin_Update: return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_VectorsWin_Print:  return (mPrint(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintVectors( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendVectorList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeVectorsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct VectorsWinData), ENTRY(VectorsWinDispatcher));
}

