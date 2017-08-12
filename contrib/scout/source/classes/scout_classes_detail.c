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

struct ClassesDetailWinData {
    TEXT cdwd_Title[WINDOW_TITLE_LENGTH];
    APTR cdwd_Texts[13];
    struct ClassEntry *cdwd_Class;
};

STATIC CONST struct LongFlag classFlags[] = {
    { CLF_INLIST, "CLF_INLIST" },
    { 0x00000002, "< ??? >" },
    { 0x00000004, "< ??? >" },
    { 0x00000008, "< ??? >" },
    { 0x00000010, "< ??? >" },
    { 0x00000020, "< ??? >" },
    { 0x00000040, "< ??? >" },
    { 0x00000080, "< ??? >" },
    { 0x00000100, "< ??? >" },
    { 0x00000200, "< ??? >" },
    { 0x00000400, "< ??? >" },
    { 0x00000800, "< ??? >" },
    { 0x00001000, "< ??? >" },
    { 0x00002000, "< ??? >" },
    { 0x00004000, "< ??? >" },
    { 0x00008000, "< ??? >" },
    { 0x00010000, "< ??? >" },
    { 0x00020000, "< ??? >" },
    { 0x00040000, "< ??? >" },
    { 0x00080000, "< ??? >" },
    { 0x00100000, "< ??? >" },
    { 0x00200000, "< ??? >" },
    { 0x00400000, "< ??? >" },
    { 0x00800000, "< ??? >" },
    { 0x01000000, "< ??? >" },
    { 0x02000000, "< ??? >" },
    { 0x04000000, "< ??? >" },
    { 0x08000000, "< ??? >" },
    { 0x10000000, "< ??? >" },
    { 0x20000000, "< ??? >" },
    { 0x40000000, "< ??? >" },
    { 0x80000000, "< ??? >" },
    { 0,          NULL },
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct ClassesDetailWinData *cdwd = INST_DATA(cl, obj);
    struct ClassEntry *ce = cdwd->cdwd_Class;
    struct IClass *ic = ce->ce_Addr;

    MySetContentsHealed(cdwd->cdwd_Texts[ 0], "%s", ic->cl_ID);
    MySetContents(cdwd->cdwd_Texts[ 1], ADDRESS_FORMAT, ic);
    MySetContentsHealed(cdwd->cdwd_Texts[ 2], "%s", (ic->cl_Super) ? (STRPTR)ic->cl_Super->cl_ID : nonetest(NULL));
    MySetContents(cdwd->cdwd_Texts[ 3], "%lD", ic->cl_InstOffset);
    MySetContents(cdwd->cdwd_Texts[ 4], "%lD", ic->cl_InstSize);
    set(cdwd->cdwd_Texts[ 5], MUIA_DisassemblerButton_Address, ic->cl_Dispatcher.h_Entry);
    set(cdwd->cdwd_Texts[ 6], MUIA_DisassemblerButton_Address, ic->cl_Dispatcher.h_SubEntry);
    set(cdwd->cdwd_Texts[ 7], MUIA_DisassemblerButton_Address, ic->cl_Dispatcher.h_Data);
    set(cdwd->cdwd_Texts[ 8], MUIA_DisassemblerButton_Address, ic->cl_UserData);
    MySetContents(cdwd->cdwd_Texts[ 9], "%lD", ic->cl_SubclassCount);
    MySetContents(cdwd->cdwd_Texts[10], "%lD", ic->cl_ObjectCount);
    set(cdwd->cdwd_Texts[11], MUIA_FlagsButton_Flags, ic->cl_Flags);
    MySetContents(cdwd->cdwd_Texts[12], "$%08lx", ic->cl_Reserved); // ULONG

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtClassesDetailTitle, ce->ce_ClassName, cdwd->cdwd_Title, sizeof(cdwd->cdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[13], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Classes",
        MUIA_Window_ID, MakeID('.','C','L','S'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtClassID2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtClassSuper),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtClassInstOffset),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtClassInstSize),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtClassEntry),
                            Child, (IPTR)(texts[ 5] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtClassSubEntry),
                            Child, (IPTR)(texts[ 6] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtClassData),
                            Child, (IPTR)(texts[ 7] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtClassUserData),
                            Child, (IPTR)(texts[ 8] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtClassSubclassCount),
                            Child, (IPTR)(texts[ 9] = (Object *)MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtClassObjectCount),
                            Child, (IPTR)(texts[10] = (Object *)MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtClassFlags2),
                            Child, (IPTR)(texts[11] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
                                MUIA_FlagsButton_Title, (IPTR)txtClassFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)classFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtClassFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtClassReserved),
                            Child, (IPTR)(texts[12] = MyTextObject6()),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ClassesDetailWinData *cdwd = INST_DATA(cl, obj);
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
    struct ClassesDetailWinData *cdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_ClassesDetailWin_Class:
                cdwd->cdwd_Class = (struct ClassEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(ClassesDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

APTR MakeClassesDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ClassesDetailWinData), ENTRY(ClassesDetailWinDispatcher));
}


