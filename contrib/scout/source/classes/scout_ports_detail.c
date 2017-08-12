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

struct PortsDetailWinData {
    char pdwd_Title[WINDOW_TITLE_LENGTH];
    APTR pdwd_Texts[11];
    struct PortEntry *pdwd_Port;
};

STATIC CONST struct MaskedLongFlag portFlags[] = {
    { PA_SIGNAL,  PF_ACTION, "PA_SIGNAL"  },
    { PA_SOFTINT, PF_ACTION, "PA_SOFTINT" },
    { PA_IGNORE,  PF_ACTION, "PA_IGNORE"  },
    { 0,          0,         NULL }
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct PortsDetailWinData *pdwd = INST_DATA(cl, obj);
    struct PortEntry *pe = pdwd->pdwd_Port;
    struct MsgPort *mp = pe->pe_Addr;

    MySetContentsHealed(pdwd->pdwd_Texts[ 0], "%s", nonetest(mp->mp_Node.ln_Name));
    MySetContents(pdwd->pdwd_Texts[ 1], ADDRESS_FORMAT, mp);
    MySetContents(pdwd->pdwd_Texts[ 2], "%s", GetNodeType(mp->mp_Node.ln_Type));
    MySetContents(pdwd->pdwd_Texts[ 3], "%ld", mp->mp_Node.ln_Pri);
    set(pdwd->pdwd_Texts[ 4], MUIA_FlagsButton_Flags, mp->mp_Flags);
    MySetContents(pdwd->pdwd_Texts[ 5], "%s", GetSigBitName(mp->mp_SigBit));
    set(pdwd->pdwd_Texts[ 6], MUIA_TaskButton_Task, mp->mp_SigTask);
    MySetContents(pdwd->pdwd_Texts[ 7], ADDRESS_FORMAT, &mp->mp_MsgList);
    MySetContents(pdwd->pdwd_Texts[ 8], ADDRESS_FORMAT, mp->mp_MsgList.lh_Head);
    MySetContents(pdwd->pdwd_Texts[ 9], ADDRESS_FORMAT, mp->mp_MsgList.lh_Tail);
    MySetContents(pdwd->pdwd_Texts[10], ADDRESS_FORMAT, mp->mp_MsgList.lh_TailPred);

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtPortsDetailTitle, pe->pe_Name, pdwd->pdwd_Title, sizeof(pdwd->pdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[11], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Ports",
        MUIA_Window_ID, MakeID('.','P','O','R'),
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
                            Child, (IPTR)MyLabel2(txtPortFlags2),
                            Child, (IPTR)(texts[ 4] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtPortFlags,
                                MUIA_FlagsButton_MaskArray, (IPTR)portFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtPortFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtPortSigBit2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtPortSigTask2),
                            Child, (IPTR)(texts[ 6] = (Object *)TaskButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtPortMsgList),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtPortMsgListHead),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtPortMsgListTail),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtPortMsgListTailPred),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct PortsDetailWinData *pdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, pdwd->pdwd_Texts, sizeof(pdwd->pdwd_Texts));

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
    struct PortsDetailWinData *pdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_PortsDetailWin_Port:
                pdwd->pdwd_Port = (struct PortEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(PortsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakePortsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct PortsDetailWinData), ENTRY(PortsDetailWinDispatcher));
}

