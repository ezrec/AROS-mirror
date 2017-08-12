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
 * @author Pavel Fedin <pavel.fedin@mail.ru>
 */

#include "system_headers.h"

#ifdef __AROS__

#include <oop/oop.h>

#endif

struct OOPDetailWinData {
    TEXT cdwd_Title[WINDOW_TITLE_LENGTH];
    APTR cdwd_Texts[13];
    struct OOPClassEntry *cdwd_Class;
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct OOPDetailWinData *cdwd = INST_DATA(cl, obj);
    struct OOPClassEntry *ce = cdwd->cdwd_Class;
#ifdef __AROS__
    OOP_Class *ic = ce->ce_Addr;

    MySetContentsHealed(cdwd->cdwd_Texts[0], "%s", ce->ce_ClassName);
    MySetContents(cdwd->cdwd_Texts[1], ADDRESS_FORMAT, ic);
    MySetContents(cdwd->cdwd_Texts[2], ADDRESS_FORMAT, ic->OOPBasePtr);
    MySetContents(cdwd->cdwd_Texts[3], "%lD", ic->InstOffset);
    set(cdwd->cdwd_Texts[4], MUIA_DisassemblerButton_Address, ic->UserData);
    set(cdwd->cdwd_Texts[5], MUIA_DisassemblerButton_Address, ic->cl_DoMethod);
    set(cdwd->cdwd_Texts[6], MUIA_DisassemblerButton_Address, ic->cl_CoerceMethod);
    set(cdwd->cdwd_Texts[7], MUIA_DisassemblerButton_Address, ic->cl_DoSuperMethod);
    MySetContentsHealed(cdwd->cdwd_Texts[8], "%s", ce->ce_SuperClassName);
#endif

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtOOPDetailTitle, ce->ce_ClassName, cdwd->cdwd_Title, sizeof(cdwd->cdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[9], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "OOP",
        MUIA_Window_ID, MakeID('O','C','L','S'),
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
			    Child, (IPTR)MyLabel2(txtOOPBasePtr),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtOOPInstOffset),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtOOPUserData),
                            Child, (IPTR)(texts[ 4] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtOOPDoMethod2),
                            Child, (IPTR)(texts[ 5] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtOOPCoerceMethod2),
                            Child, (IPTR)(texts[ 6] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtOOPDoSuperMethod2),
                            Child, (IPTR)(texts[ 7] = (Object *)DisassemblerButtonObject,
                            End),
			    Child, (IPTR)MyLabel2(txtOOPSuper),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct OOPDetailWinData *cdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, cdwd->cdwd_Texts, sizeof(cdwd->cdwd_Texts));

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
    struct OOPDetailWinData *cdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_OOPDetailWin_Class:
                cdwd->cdwd_Class = (struct OOPClassEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(OOPDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

APTR MakeOOPDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct OOPDetailWinData), ENTRY(OOPDetailWinDispatcher));
}


