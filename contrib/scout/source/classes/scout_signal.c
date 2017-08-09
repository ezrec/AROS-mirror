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

struct SignalWinData {
    TEXT swd_Title[WINDOW_TITLE_LENGTH];
    APTR swd_ParentWindow;
    APTR swd_SignalText[2];
    APTR swd_SignalString;
    APTR swd_SignalCheckmarks[32];
    APTR swd_OkButton;
};

struct MUIP_SignalWin_GetSignals { STACKED ULONG methodID; STACKED STRPTR objname; STACKED ULONG *signals; };

struct MUIP_SigBitChanged { STACKED ULONG whichbit; STACKED ULONG state; };

struct MUIP_SigStrChanged { STACKED UBYTE str; };

HOOKPROTONH(hex_editfunc, ULONG, struct SGWork *sgw, IPTR *msg)
{
    ULONG return_code;

    return_code = ~0L;

    if (*msg == SGH_KEY) {
        BOOL isHex;
        BOOL ok = FALSE;
        IPTR tmplong;

        isHex = IsUHex(sgw->WorkBuffer, &tmplong);

        if (isHex) {
            if (sgw->EditOp == EO_REPLACECHAR || sgw->EditOp == EO_INSERTCHAR || sgw->EditOp == EO_ENTER) {
                ULONG len;

                // the string represents a valid hexadecimal string
                len = strlen(sgw->WorkBuffer);
                if (sgw->WorkBuffer[0] == '$' && len <= 9) {
                    ok = TRUE;
                } else if (sgw->WorkBuffer[0] == '0' && (sgw->WorkBuffer[1] == 'x' || sgw->WorkBuffer[1] == 'X') && len <= 10) {
                    ok = TRUE;
                } else if (len <= 8) {
                    ok = TRUE;
                }
            } else {
                ok = TRUE;
            }
        }

        if (!ok) {
            SET_FLAG(sgw->Actions, SGA_BEEP);
            CLEAR_FLAG(sgw->Actions, SGA_USE);
        }
    } else {
        return_code = 0;
    }

    return return_code;
}
MakeStaticHook(hex_edithook, hex_editfunc);

HOOKPROTONH(sigbitchanged_func, void, Object *obj, struct MUIP_SigBitChanged *msg)
{
    struct SignalWinData *swd = INST_DATA(OCLASS(obj), obj);
    STRPTR sigstr;
    IPTR sigs;

    sigstr = (STRPTR)xget(swd->swd_SignalString, MUIA_String_Contents);
    IsUHex(sigstr, &sigs);
    if (msg->state) {
        SET_BIT(sigs, msg->whichbit);
    } else {
        CLEAR_BIT(sigs, msg->whichbit);
    }
    MySetStringContents(swd->swd_SignalString, "$%08lx", sigs);
}
MakeStaticHook(sigbitchanged_hook, sigbitchanged_func);

HOOKPROTONHNP(sigstrchanged_func, void, Object *obj)
{
    struct SignalWinData *swd = INST_DATA(OCLASS(obj), obj);
    STRPTR sigstr;
    IPTR sigs;
    ULONG bit;

    sigstr = (STRPTR)xget(swd->swd_SignalString, MUIA_String_Contents);
    IsUHex(sigstr, &sigs);
    for (bit = 0; bit < 32; bit++) {
        nnset(swd->swd_SignalCheckmarks[bit], MUIA_Selected, BIT_IS_SET(sigs, bit));
    }
}
MakeStaticHook(sigstrchanged_hook, sigstrchanged_func);

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR sigtext1, sigtext2, sigtext3, sigchecks[32], okButton, cancelButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_Window_ID, MakeID('S','E','N','D'),
        WindowContents, VGroup,

            Child, (IPTR)MyVSpace(2),
            Child, (IPTR)(sigtext1 = MyTextObject()),
            Child, (IPTR)MyVSpace(2),
            Child, (IPTR)HGroup, MUIA_Group_SameWidth, TRUE,
                Child, (IPTR)HGroup,
                    Child, (IPTR)KeyLabel(txtNewSignal, 's'),
                    Child, (IPTR)(sigtext2 = (Object *)StringObject,
                        MUIA_ControlChar, 's',
                        MUIA_CycleChain, TRUE,
                        MUIA_String_MaxLen, 10,
                        MUIA_String_EditHook, (IPTR)&hex_edithook,
                        MUIA_String_Format, MUIV_String_Format_Center,
                        StringFrame,
                    End),
                    Child, (IPTR)MyLabel(txtWaitSignal),
                    Child, (IPTR)(sigtext3 = MyTextObject()),
                End,
            End,

            Child, (IPTR)MyVSpace(2),
            Child, (IPTR)ColGroup(23),
                Child, (IPTR)(sigchecks[31] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("31"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[30] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("30"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[29] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("29"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[28] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("28"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[27] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("27"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[26] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("26"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[25] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("25"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[24] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("24"),
                Child, (IPTR)(sigchecks[23] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("23"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[22] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("22"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[21] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("21"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[20] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("20"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[19] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("19"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[18] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("18"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[17] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("17"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[16] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("16"),
                Child, (IPTR)(sigchecks[15] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("15"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[14] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("14"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[13] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("13"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[12] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("12"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[11] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("11"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[10] = MakeCheckmark(NULL)), Child, (IPTR)LLabel("10"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 9] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 9"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 8] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 8"),
                Child, (IPTR)(sigchecks[ 7] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 7"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 6] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 6"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 5] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 5"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 4] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 4"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 3] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 3"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 2] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 2"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 1] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 1"), Child, (IPTR)HSpace(0),
                Child, (IPTR)(sigchecks[ 0] = MakeCheckmark(NULL)), Child, (IPTR)LLabel(" 0"),
            End,

            Child, (IPTR)MyVSpace(2),
            Child, (IPTR)HGroup, MUIA_Group_SameWidth, TRUE,
                Child, (IPTR)(okButton = MakeButton(txtMUIOk)),
                Child, (IPTR)(cancelButton = MakeButton(txtMUICancel)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct SignalWinData *swd = INST_DATA(cl, obj);
        APTR parent;
        ULONG sig;

        swd->swd_SignalText[0] = sigtext1;
        swd->swd_SignalText[1] = sigtext3;
        swd->swd_SignalString = sigtext2;
        CopyMemQuick(sigchecks, swd->swd_SignalCheckmarks, sizeof(swd->swd_SignalCheckmarks));
        swd->swd_OkButton = okButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtSignalWinTitle, swd->swd_Title, sizeof(swd->swd_Title)));
        set(obj, MUIA_Window_ActiveObject, sigtext2);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 2, MUIM_Application_ReturnID, 1);
        DoMethod(sigtext2,     MUIM_Notify, MUIA_String_Contents,     MUIV_EveryTime, obj, 3, MUIM_CallHook, &sigstrchanged_hook, MUIV_TriggerValue);
        DoMethod(okButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          MUIV_Notify_Application, 2, MUIM_Application_ReturnID, 2);
        DoMethod(cancelButton, MUIM_Notify, MUIA_Pressed,             FALSE,          MUIV_Notify_Application, 2, MUIM_Application_ReturnID, 1);
        for (sig = 0; sig < 32; sig++) DoMethod(sigchecks[sig], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, obj, 4, MUIM_CallHook, &sigbitchanged_hook, sig, MUIV_TriggerValue);
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

STATIC IPTR mGetSignals( struct IClass *cl,
                          Object *obj,
                          struct MUIP_SignalWin_GetSignals *msg )
{
    struct SignalWinData *swd = INST_DATA(cl, obj);
    IPTR sig;
    APTR app;
    BOOL done = FALSE;
    IPTR result = 0;

    MySetContents(swd->swd_SignalText[0], msgSelectNewSignal, msg->objname);
    MySetContents(swd->swd_SignalText[1], MUIX_C "$%08lx", *msg->signals);
    MySetStringContents(swd->swd_SignalString, "$%08lx", *msg->signals);

    for (sig = 0; sig < 32; sig++) nnset(swd->swd_SignalCheckmarks[sig], MUIA_Selected, BIT_IS_SET(*msg->signals, sig));

    app = (APTR)xget(obj, MUIA_ApplicationObject);
    set(app, MUIA_Application_Sleep, TRUE);
    set(obj, MUIA_Window_Open, TRUE);

    while (!done) {
        ULONG signals;

        switch (DoMethod(app, MUIM_Application_NewInput, &signals)) {
            case MUIV_Application_ReturnID_Quit:
            case 1:
                done = TRUE;
                result = 0;
                break;

            case 2:
                done = TRUE;
                result = 1;

            default:
                break;
        }

        if (!done && signals != 0) {
            Wait(signals);
        }
    }

    set(obj, MUIA_Window_Open, FALSE);
    set(app, MUIA_Application_Sleep, FALSE);

    if (result) {
        STRPTR str;
        IPTR sig = 0;

        str = (STRPTR)xget(swd->swd_SignalString, MUIA_String_Contents);
        IsUHex(str, &sig);
        *msg->signals = sig;
    }

    return result;
}

DISPATCHER(SignalWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                    return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                return (mDispose(cl, obj, (APTR)msg));
        case MUIM_SignalWin_GetSignals: return (mGetSignals(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeSignalWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct SignalWinData), ENTRY(SignalWinDispatcher));
}

