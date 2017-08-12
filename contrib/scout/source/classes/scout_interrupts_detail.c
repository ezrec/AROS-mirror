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

struct InterruptsDetailWinData {
    TEXT idwd_Title[WINDOW_TITLE_LENGTH];
    APTR idwd_Texts[8];
    struct InterruptEntry *idwd_Interrupt;
};

STATIC STRPTR GetIntType( ULONG num )
{
    switch (num) {
        case  0: return txtInterrupt00Name;
        case  1: return txtInterrupt01Name;
        case  2: return txtInterrupt02Name;
        case  3: return txtInterrupt03Name;
        case  4: return txtInterrupt04Name;
        case  5: return txtInterrupt05Name;
        case  6: return txtInterrupt06Name;
        case  7: return txtInterrupt07Name;
        case  8: return txtInterrupt08Name;
        case  9: return txtInterrupt09Name;
        case 10: return txtInterrupt10Name;
        case 11: return txtInterrupt11Name;
        case 12: return txtInterrupt12Name;
        case 13: return txtInterrupt13Name;
        case 14: return txtInterrupt14Name;
        case 15: return txtInterrupt15Name;
    }

    return txtNone;
}

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct InterruptsDetailWinData *idwd = INST_DATA(cl, obj);
    struct InterruptEntry *ie = idwd->idwd_Interrupt;
    struct Interrupt *is = ie->ie_Addr;

    MySetContentsHealed(idwd->idwd_Texts[0], "%s", ie->ie_Name);
    MySetContents(idwd->idwd_Texts[1], ADDRESS_FORMAT, is);
    MySetContents(idwd->idwd_Texts[2], "%s", GetNodeType(is->is_Node.ln_Type));
    MySetContents(idwd->idwd_Texts[3], "%ld", is->is_Node.ln_Pri);
    set(idwd->idwd_Texts[4], MUIA_DisassemblerButton_Address, is->is_Data);
    set(idwd->idwd_Texts[5], MUIA_DisassemblerButton_Address, is->is_Code);
    MySetContents(idwd->idwd_Texts[6], "%s", ie->ie_Kind);
    MySetContents(idwd->idwd_Texts[7], "%s", GetIntType(ie->ie_IntNumber));

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtInterruptsDetailTitle, ie->ie_Name, idwd->idwd_Title, sizeof(idwd->idwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[8], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Interrupts",
        MUIA_Window_ID, MakeID('.','I','N','T'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtNodeName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodeType2),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodePri2),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtInterruptData2),
                            Child, (IPTR)(texts[ 4] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtInterruptCode2),
                            Child, (IPTR)(texts[ 5] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtInterruptSysType),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtInterruptSysName),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct InterruptsDetailWinData *idwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, idwd->idwd_Texts, sizeof(idwd->idwd_Texts));

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
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

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct InterruptsDetailWinData *idwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_InterruptsDetailWin_Interrupt:
                idwd->idwd_Interrupt = (struct InterruptEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(InterruptsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeInterruptsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct InterruptsDetailWinData), ENTRY(InterruptsDetailWinDispatcher));
}

