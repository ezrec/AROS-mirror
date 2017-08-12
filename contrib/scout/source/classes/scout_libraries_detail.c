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

struct LibrariesDetailWinData {
    TEXT ldwd_Title[WINDOW_TITLE_LENGTH];
    APTR ldwd_Texts[13];
    struct LibraryEntry *ldwd_Library;
    APTR ldwd_MainGroup;
};

STATIC CONST struct LongFlag libFlags[] = {
    { LIBF_SUMMING,   "LIBF_SUMMING" },
    { LIBF_CHANGED,   "LIBF_CHANGED" },
    { LIBF_SUMUSED,   "LIBF_SUMUSED" },
    { LIBF_DELEXP,    "LIBF_DELEXP" },

#if defined(__MORPHOS__)
    { LIBF_RAMLIB,    "LIBF_RAMLIB" },
    { LIBF_QUERYINFO, "LIBF_QUERYINFO" },
#elif defined(__amigaos4__)
    { LIBF_EXP0CNT,   "LIBF_EXP0CNT" },
    { 0x20,           "< ??? >" },
#else
    { 0x10,           "< ??? >" },
    { 0x20,           "< ??? >" },
#endif
    { 0x40,           "< ??? >" },
    { 0x80,           "< ??? >" },
    { 0,              NULL },
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct LibrariesDetailWinData *ldwd = INST_DATA(cl, obj);
    struct LibraryEntry *le = ldwd->ldwd_Library;
    struct Library *lib = le->le_Addr;
    LONG cnt, total;

    MySetContentsHealed(ldwd->ldwd_Texts[ 0], "%s", lib->lib_Node.ln_Name);
    MySetContents(ldwd->ldwd_Texts[ 1], ADDRESS_FORMAT, lib);
    MySetContentsHealed(ldwd->ldwd_Texts[ 2], "%s", nonetest(lib->lib_IdString));
    MySetContents(ldwd->ldwd_Texts[ 3], "%s", GetNodeType(lib->lib_Node.ln_Type));
    MySetContents(ldwd->ldwd_Texts[ 4], "%ld", lib->lib_Node.ln_Pri);
    MySetContents(ldwd->ldwd_Texts[ 5], "%ld.%ld", lib->lib_Version, lib->lib_Revision);
    MySetContents(ldwd->ldwd_Texts[ 6], "%lD", lib->lib_OpenCnt);
    set(ldwd->ldwd_Texts[ 7], MUIA_FlagsButton_Flags, lib->lib_Flags);
    MySetContents(ldwd->ldwd_Texts[ 8], "%lD", lib->lib_NegSize);
    MySetContents(ldwd->ldwd_Texts[ 9], "%lD", lib->lib_PosSize);
    MySetContents(ldwd->ldwd_Texts[10], "$%08lx", lib->lib_Sum); // ULONG

    GetRamPointerCount(lib, &cnt, &total);
    MySetContents(ldwd->ldwd_Texts[11], txtLibraryRPCFormat, cnt, total);
    MySetContents(ldwd->ldwd_Texts[12], "%s", le->le_CodeType);

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtLibrariesDetailTitle, le->le_Name, ldwd->ldwd_Title, sizeof(ldwd->ldwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[13], exitButton, mainGroup;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Libraries",
        MUIA_Window_ID, MakeID('.','L','I','B'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(mainGroup = (Object *)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtNodeName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryIdString),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodeType2),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodePri2),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryVersion2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryOpenCount2),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryFlags2),
                            Child, (IPTR)(texts[ 7] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtLibraryFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)libFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtLibraryFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtLibraryNegSize),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryPosSize),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibrarySum),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryRPC2),
                            Child, (IPTR)(texts[11] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryCodeType2),
                            Child, (IPTR)(texts[12] = MyTextObject6()),
                        End,
                    End),
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct LibrariesDetailWinData *ldwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, ldwd->ldwd_Texts, sizeof(ldwd->ldwd_Texts));
        ldwd->ldwd_MainGroup = mainGroup;

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
    struct LibrariesDetailWinData *ldwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_LibrariesDetailWin_Library:
                ldwd->ldwd_Library = (struct LibraryEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(LibrariesDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeLibrariesDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct LibrariesDetailWinData), ENTRY(LibrariesDetailWinDispatcher));
}

