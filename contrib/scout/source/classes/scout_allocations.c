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

struct AllocationsWinData {
    TEXT awd_Title[WINDOW_TITLE_LENGTH];
    APTR awd_CIAAText[5];
    APTR awd_CIABText[5];
    APTR awd_PortsText[4];
};

struct AllocationsCallbackUserData {
    APTR ud_CIAAUsers[5];
    APTR ud_CIABUsers[5];
    APTR ud_PortsUsers[4];
};

STATIC ULONG ASM SAVEDS INTERRUPT DummyIntFunc( void )
{
    return 0;
}

STATIC STRPTR AlcMiscUser( ULONG unit )
{
    struct Library *MiscBase;
    STRPTR result;

    if ((MiscBase = OpenResource(MISCNAME)) != NULL) {
    #if defined(__amigaos4__)
        struct MiscIFace *IMisc;
    #endif

        if (GETINTERFACE(IMisc, MiscBase)) {
            STRPTR name;

            if ((name = (STRPTR)AllocMiscResource(unit, "Scout")) != NULL) {
                result = name;
            } else {
                FreeMiscResource(unit);
                result = txtAllocationFree;
            }

            DROPINTERFACE(IMisc);
        } else {
            result = txtAllocationNotAvailable;
        }
    } else {
        result = txtAllocationNotAvailable;
    }

    return result;
}

STATIC STRPTR AlcCIAUser( CONST_STRPTR res,
                          WORD unit )
{
    struct Library *CiaBase;

    if ((CiaBase = OpenResource(res)) != NULL) {
        struct Interrupt dummyInt, *intr;

        memset(&dummyInt, 0x00, sizeof(struct Interrupt));
        dummyInt.is_Node.ln_Type = NT_INTERRUPT;
        dummyInt.is_Node.ln_Pri = -127;
        dummyInt.is_Node.ln_Name = (STRPTR)"« Scout's Dummy Int »";
        dummyInt.is_Code = (void (*)())DummyIntFunc;

        Disable();
        if ((intr = AddICRVector(CiaBase, unit, &dummyInt)) == NULL) RemICRVector(CiaBase, unit, &dummyInt);
        Enable();

        if (intr) {
            return (intr->is_Node.ln_Name) ? (STRPTR)intr->is_Node.ln_Name : txtAllocationAllocated;
        } else {
            return txtAllocationFree;
        }
    } else {
        return txtAllocationNotAvailable;
    }
}

STATIC void ReceiveList( void (* callback)( struct AllocationEntry *ae, void *userData ),
                         void *userData )
{
    struct AllocationEntry *ae;

    if ((ae = tbAllocVecPooled(globalPool, sizeof(struct AllocationEntry))) != NULL) {
        if (SendDaemon("GetAlcList")) {
            while (ReceiveDecodedEntry(ae, sizeof(struct AllocationEntry))) {
                callback(ae, userData);
            }
        }

        tbFreeVecPooled(globalPool, ae);
    }
}

STATIC void IterateList( void (* callback)( struct AllocationEntry *ae, void *userData ),
                         void *userData )
{
    struct AllocationEntry *ae;

    if ((ae = tbAllocVecPooled(globalPool, sizeof(struct AllocationEntry))) != NULL) {
        ULONG i;

        Forbid();

        for (i = 0; i < 5; i++) stccpy(ae->ae_CIAAUsers[i], AlcCIAUser(CIAANAME, i), sizeof(ae->ae_CIAAUsers[i]));
        for (i = 0; i < 5; i++) stccpy(ae->ae_CIABUsers[i], AlcCIAUser(CIABNAME, i), sizeof(ae->ae_CIABUsers[i]));
        stccpy(ae->ae_PortsUsers[0], AlcMiscUser(MR_SERIALPORT), sizeof(ae->ae_PortsUsers[0]));
        stccpy(ae->ae_PortsUsers[1], AlcMiscUser(MR_SERIALBITS), sizeof(ae->ae_PortsUsers[1]));
        stccpy(ae->ae_PortsUsers[2], AlcMiscUser(MR_PARALLELPORT), sizeof(ae->ae_PortsUsers[2]));
        stccpy(ae->ae_PortsUsers[3], AlcMiscUser(MR_PARALLELBITS), sizeof(ae->ae_PortsUsers[3]));

        Permit();

        callback(ae, userData);

        tbFreeVecPooled(globalPool, ae);
    }
}

STATIC void UpdateCallback( struct AllocationEntry *ae,
                            void *userData )
{
    struct AllocationsCallbackUserData *ud = (struct AllocationsCallbackUserData *)userData;
    ULONG i;

    for (i = 0; i < 5; i++) MySetContents(ud->ud_CIAAUsers[i], ae->ae_CIAAUsers[i]);
    for (i = 0; i < 5; i++) MySetContents(ud->ud_CIABUsers[i], ae->ae_CIABUsers[i]);
    for (i = 0; i < 4; i++) MySetContents(ud->ud_PortsUsers[i], ae->ae_PortsUsers[i]);
}

STATIC void PrintCallback( struct AllocationEntry *ae,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtAllocationsPrintHeaderCIA, "A");
    PrintFOneLine((BPTR)userData, txtAllocationsPrintLineCIA, ae->ae_CIAAUsers[0], ae->ae_CIAAUsers[1], ae->ae_CIAAUsers[2], ae->ae_CIAAUsers[3], ae->ae_CIAAUsers[4]);

    PrintFOneLine((BPTR)userData, txtAllocationsPrintHeaderCIA, "B");
    PrintFOneLine((BPTR)userData, txtAllocationsPrintLineCIA, ae->ae_CIABUsers[0], ae->ae_CIABUsers[1], ae->ae_CIABUsers[2], ae->ae_CIABUsers[3], ae->ae_CIABUsers[4]);

    PrintFOneLine((BPTR)userData, txtAllocationsPrintHeaderPorts);
    PrintFOneLine((BPTR)userData, txtAllocationsPrintLinePorts, ae->ae_PortsUsers[0], ae->ae_PortsUsers[1], ae->ae_PortsUsers[2], ae->ae_PortsUsers[3]);
}

STATIC void SendCallback( struct AllocationEntry *ae,
                          UNUSED void *userData )
{
    SendEncodedEntry(ae, sizeof(struct AllocationEntry));
}

STATIC ULONG mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    STATIC STRPTR allocationPages[4] = { NULL };
    APTR pages, ciaaText[5], ciabText[5], portsText[4], updateButton, printButton, exitButton;

    allocationPages[0] = txtAllocationCIAA;
    allocationPages[1] = txtAllocationCIAB;
    allocationPages[2] = txtAllocationPorts;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Allocation",
        MUIA_Window_ID, MakeID('A','L','L','C'),
        WindowContents, VGroup,

            Child, (IPTR)(pages = (Object *)RegisterGroup(allocationPages),
                MUIA_CycleChain, TRUE,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)Label1(txtAllocationTimerA),
                            Child, (IPTR)(ciaaText[0] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationTimerB),
                            Child, (IPTR)(ciaaText[1] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationAlarm),
                            Child, (IPTR)(ciaaText[2] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationSerial),
                            Child, (IPTR)(ciaaText[3] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationFlag),
                            Child, (IPTR)(ciaaText[4] = MyTextObject()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)Label1(txtAllocationTimerA),
                            Child, (IPTR)(ciabText[0] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationTimerB),
                            Child, (IPTR)(ciabText[1] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationAlarm),
                            Child, (IPTR)(ciabText[2] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationSerial),
                            Child, (IPTR)(ciabText[3] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationFlag),
                            Child, (IPTR)(ciabText[4] = MyTextObject()),
                        End,
                    End,
                End,

                Child, (IPTR)ScrollgroupObject,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)Label1(txtAllocationSerialPort),
                            Child, (IPTR)(portsText[0] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationSerialControl),
                            Child, (IPTR)(portsText[1] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationParallelPort),
                            Child, (IPTR)(portsText[2] = MyTextObject()),
                            Child, (IPTR)Label1(txtAllocationParallelControl),
                            Child, (IPTR)(portsText[3] = MyTextObject()),
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
        struct AllocationsWinData *awd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(ciaaText, awd->awd_CIAAText, sizeof(awd->awd_CIAAText));
        CopyMemQuick(ciabText, awd->awd_CIABText, sizeof(awd->awd_CIABText));
        CopyMemQuick(portsText, awd->awd_PortsText, sizeof(awd->awd_PortsText));

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (ULONG)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtAllocationsTitle, awd->awd_Title, sizeof(awd->awd_Title)));
        set(obj, MUIA_Window_ActiveObject, pages);
        set(pages, MUIA_CycleChain, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     1, MUIM_AllocationsWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     1, MUIM_AllocationsWin_Print);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (ULONG)obj;
}

STATIC ULONG mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC ULONG mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct AllocationsWinData *awd = INST_DATA(cl, obj);
    struct AllocationsCallbackUserData ud;

    CopyMemQuick(awd->awd_CIAAText, ud.ud_CIAAUsers, sizeof(ud.ud_CIAAUsers));
    CopyMemQuick(awd->awd_CIABText, ud.ud_CIABUsers, sizeof(ud.ud_CIABUsers));
    CopyMemQuick(awd->awd_PortsText, ud.ud_PortsUsers, sizeof(ud.ud_PortsUsers));

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    return 0;
}

STATIC ULONG mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintAllocations(NULL);

    return 0;
}

DISPATCHER(AllocationsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                 return (mDispose(cl, obj, (APTR)msg));
        case MUIM_AllocationsWin_Update: return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_AllocationsWin_Print:  return (mPrint(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintAllocations( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendAlcList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeAllocationsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct AllocationsWinData), ENTRY(AllocationsWinDispatcher));
}

