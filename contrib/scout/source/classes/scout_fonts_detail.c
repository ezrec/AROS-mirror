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
 * You must not use this source code to gain profit of any kind!
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"

struct FontsDetailWinData {
    char fdwd_Title[WINDOW_TITLE_LENGTH];
    APTR fdwd_Texts[13];
    APTR fdwd_MainGroup;
    APTR fdwd_ExampleGroup;
    APTR fdwd_Example;
    struct FontEntry *fdwd_Font;
};

STATIC CONST struct LongFlag fontFlags[] = {
    { FPF_ROMFONT,      "FPF_ROMFONT" },
    { FPF_DISKFONT,     "FPF_DISKFONT" },
    { FPF_REVPATH,      "FPF_REVPATH" },
    { FPF_TALLDOT,      "FPF_TALLDOT" },
    { FPF_WIDEDOT,      "FPF_WIDEDOT" },
    { FPF_PROPORTIONAL, "FPF_PROPORTIONAL" },
    { FPF_DESIGNED,     "FPF_DESIGNED" },
    { FPF_REMOVED,      "FPF_REMOVED" },
    { 0,                NULL },
};

STATIC CONST struct LongFlag fontStyles[] = {
    { FSF_UNDERLINED,  "FSF_UNDERLINED" },
    { FSF_BOLD,        "FSF_BOLD" },
    { FSF_ITALIC,      "FSF_ITALIC" },
    { FSF_EXTENDED,    "FSF_EXTENDED" },
#if defined(__amigaos4__)
    { FSF_ANTIALIASED, "FSF_ANTIALIASED" },
#else
    { 0x10,            "< ??? >" },
#endif
    { 0x20,            "< ??? >" },
    { FSF_COLORFONT,   "FSF_COLORFONT" },
    { FSF_TAGGED,      "FSF_TAGGED" },
    { 0,               NULL },
};

STATIC CONST struct LongFlag fontExtensionFlags0[] = {
    { TE0F_NOREMFONT,  "TE0F_NOREMFONT" },
    { 0x0002,          "< ??? >" },
    { 0x0004,          "< ??? >" },
    { 0x0008,          "< ??? >" },
    { 0x0010,          "< ??? >" },
    { 0x0020,          "< ??? >" },
    { 0x0040,          "< ??? >" },
    { 0x0080,          "< ??? >" },
    { 0x0100,          "< ??? >" },
    { 0x0200,          "< ??? >" },
    { 0x0400,          "< ??? >" },
    { 0x0800,          "< ??? >" },
    { 0x1000,          "< ??? >" },
    { 0x2000,          "< ??? >" },
    { 0x4000,          "< ??? >" },
    { 0x8000,          "< ??? >" },
    { 0,               NULL },
};

STATIC CONST struct LongFlag fontExtensionFlags1[] = {
#if defined(__amigaos4__)
    { TE1F_NOUNDERLINING,   "TE1F_NOUNDERLINING" },
    { TE1F_NOEMBOLDENING,   "TE1F_NOEMBOLDENING" },
    { TE1F_NOSHEARING,      "TE1F_NOSHEARING" },
    { TE1F_NODEUNDERLINING, "TE1F_NODEUNDERLINING" },
    { TE1F_NODEEMBOLDENING, "TE1F_NODEEMBOLDENING" },
    { TE1F_NODESHEARING,    "TE1F_NODESHEARING" },
    { TE1F_NOANTIALIASING,  "TE1F_NOANTIALIASING" },
    { TE1F_GENFONT,         "TE1F_GENFONT" },
#else
    { 0x0001,               "< ??? >" },
    { 0x0002,               "< ??? >" },
    { 0x0004,               "< ??? >" },
    { 0x0008,               "< ??? >" },
    { 0x0010,               "< ??? >" },
    { 0x0020,               "< ??? >" },
    { 0x0040,               "< ??? >" },
    { 0x0080,               "< ??? >" },
#endif
    { 0x0100,               "< ??? >" },
    { 0x0200,               "< ??? >" },
    { 0x0400,               "< ??? >" },
    { 0x0800,               "< ??? >" },
    { 0x1000,               "< ??? >" },
    { 0x2000,               "< ??? >" },
    { 0x4000,               "< ??? >" },
    { 0x8000,               "< ??? >" },
    { 0,                    NULL },
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct FontsDetailWinData *fdwd = INST_DATA(cl, obj);
    struct FontEntry *fe = fdwd->fdwd_Font;
    struct TextFont *tf = fe->fe_Addr;
    APTR examplegroup, example;

    MySetContentsHealed(fdwd->fdwd_Texts[ 0], "%s", tf->tf_Message.mn_Node.ln_Name);
    MySetContents(fdwd->fdwd_Texts[ 1], "$%08lx", tf);
    MySetContents(fdwd->fdwd_Texts[ 2], "$%08lx", tf->tf_Extension);
    MySetContents(fdwd->fdwd_Texts[ 3], "%ld", tf->tf_YSize);
    set(fdwd->fdwd_Texts[ 4], MUIA_FlagsButton_Flags, tf->tf_Style);
    set(fdwd->fdwd_Texts[ 5], MUIA_FlagsButton_Flags, tf->tf_Flags);
    MySetContents(fdwd->fdwd_Texts[ 6], "%ld", tf->tf_XSize);
    MySetContents(fdwd->fdwd_Texts[ 7], "%ld", tf->tf_Baseline);
    MySetContents(fdwd->fdwd_Texts[ 8], "%ld", tf->tf_BoldSmear);
    MySetContents(fdwd->fdwd_Texts[ 9], "%ld", tf->tf_Accessors);
    MySetContents(fdwd->fdwd_Texts[10], "%ld", tf->tf_LoChar);
    MySetContents(fdwd->fdwd_Texts[11], "%ld", tf->tf_HiChar);
    MySetContents(fdwd->fdwd_Texts[12], "%ld", tf->tf_Modulo);

    if (tf->tf_Extension != NULL) {
        struct TextFontExtension *tfe = (struct TextFontExtension *)tf->tf_Extension;
        APTR subgroup, texts[5];

        subgroup = VGroup,
            GroupFrameT(txtFontsExtension),
            Child, ColGroup(2),
                Child, MyLabel2(txtFontsExtensionFlags02),
                Child, texts[ 0] = FlagsButtonObject,
                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                    MUIA_FlagsButton_Title, txtFontsExtensionFlags0,
                    MUIA_FlagsButton_BitArray, fontExtensionFlags0,
                    MUIA_FlagsButton_WindowTitle, txtFontsExtensionFlags0Title,
                End,
                Child, MyLabel2(txtFontsExtensionFlags12),
                Child, texts[ 1] = FlagsButtonObject,
                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                    MUIA_FlagsButton_Title, txtFontsExtensionFlags1,
                    MUIA_FlagsButton_BitArray, fontExtensionFlags1,
                    MUIA_FlagsButton_WindowTitle, txtFontsExtensionFlags1Title,
                End,
                Child, MyLabel2(txtFontsExtensionTags),
                Child, texts[ 2] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, MyLabel2(txtFontsExtensionOFontPatchS),
                Child, texts[ 3] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, MyLabel2(txtFontsExtensionOFontPatchK),
                Child, texts[ 4] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
            End,
        End;

        DoMethod(fdwd->fdwd_MainGroup, MUIM_Group_InitChange);
        DoMethod(fdwd->fdwd_MainGroup, OM_ADDMEMBER, subgroup);
        DoMethod(fdwd->fdwd_MainGroup, MUIM_Group_ExitChange);

        set(texts[ 0], MUIA_FlagsButton_Flags, tfe->tfe_Flags0);
        set(texts[ 1], MUIA_FlagsButton_Flags, tfe->tfe_Flags1);
        set(texts[ 2], MUIA_DisassemblerButton_Address, tfe->tfe_Tags);
        set(texts[ 3], MUIA_DisassemblerButton_Address, tfe->tfe_OFontPatchS);
        set(texts[ 4], MUIA_DisassemblerButton_Address, tfe->tfe_OFontPatchK);
    }

    examplegroup = VGroup,
        GroupFrameT(txtFontsExample),
        Child, example = FontDisplayObject,
            MUIA_FontDisplay_Text, txtFontsExampleText,
            MUIA_Weight, 20,
        End,
    End;

    DoMethod(fdwd->fdwd_MainGroup, MUIM_Group_InitChange);
    DoMethod(fdwd->fdwd_MainGroup, OM_ADDMEMBER, examplegroup);
    DoMethod(fdwd->fdwd_MainGroup, MUIM_Group_ExitChange);

    set(example, MUIA_FontDisplay_Font, tf);

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtFontsDetailTitle, fe->fe_Name, fdwd->fdwd_Title, sizeof(fdwd->fdwd_Title)));
}

STATIC ULONG mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, maingroup, texts[13], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Fonts",
        MUIA_Window_ID, MakeID('.','F','O','N'),
        WindowContents, VGroup,

            Child, group = ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, maingroup = VGroup,
                        GroupFrame,
                        Child, ColGroup(2),
                            Child, MyLabel2(txtNodeName2),
                            Child, texts[ 0] = MyTextObject6(),
                            Child, MyLabel2(txtAddress2),
                            Child, texts[ 1] = MyTextObject6(),
                            Child, MyLabel2(txtFontsExtension2),
                            Child, texts[ 2] = MyTextObject6(),
                            Child, MyLabel2(txtFontsYSize2),
                            Child, texts[ 3] = MyTextObject6(),
                            Child, MyLabel2(txtFontsStyle2),
                            Child, texts[ 4] = FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, txtFontsStyle,
                                MUIA_FlagsButton_BitArray, fontStyles,
                                MUIA_FlagsButton_WindowTitle, txtFontsStyleTitle,
                            End,
                            Child, MyLabel2(txtFontsFlags2),
                            Child, texts[ 5] = FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, txtFontsFlags,
                                MUIA_FlagsButton_BitArray, fontFlags,
                                MUIA_FlagsButton_WindowTitle, txtFontsFlagsTitle,
                            End,
                            Child, MyLabel2(txtFontsXSize2),
                            Child, texts[ 6] = MyTextObject6(),
                            Child, MyLabel2(txtFontsBaseline),
                            Child, texts[ 7] = MyTextObject6(),
                            Child, MyLabel2(txtFontsBoldSmear),
                            Child, texts[ 8] = MyTextObject6(),
                            Child, MyLabel2(txtFontsAccessors2),
                            Child, texts[ 9] = MyTextObject6(),
                            Child, MyLabel2(txtFontsLoChar2),
                            Child, texts[10] = MyTextObject6(),
                            Child, MyLabel2(txtFontsHiChar2),
                            Child, texts[11] = MyTextObject6(),
                            Child, MyLabel2(txtFontsModulo),
                            Child, texts[12] = MyTextObject6(),
                        End,
                    End,
                End,
            End,
            Child, MyVSpace(4),
            Child, exitButton = MakeButton(txtExit),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct FontsDetailWinData *fdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, fdwd->fdwd_Texts, sizeof(fdwd->fdwd_Texts));
        fdwd->fdwd_MainGroup = maingroup;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (ULONG)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (ULONG)obj;
}

STATIC ULONG mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC ULONG mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct FontsDetailWinData *fdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem(&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_FontsDetailWin_Font:
                fdwd->fdwd_Font = (struct FontEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(FontsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}
DISPATCHER_END

APTR MakeFontsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct FontsDetailWinData), DISPATCHER_REF(FontsDetailWinDispatcher));
}

