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
    MySetContents(fdwd->fdwd_Texts[ 1], ADDRESS_FORMAT, tf);
    MySetContents(fdwd->fdwd_Texts[ 2], ADDRESS_FORMAT, tf->tf_Extension);
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

        subgroup = (Object *)VGroup,
            GroupFrameT(txtFontsExtension),
            Child, (IPTR)ColGroup(2),
                Child, (IPTR)MyLabel2(txtFontsExtensionFlags02),
                Child, (IPTR)(texts[ 0] = (Object *)FlagsButtonObject,
                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                    MUIA_FlagsButton_Title, (IPTR)txtFontsExtensionFlags0,
                    MUIA_FlagsButton_BitArray, (IPTR)fontExtensionFlags0,
                    MUIA_FlagsButton_WindowTitle, (IPTR)txtFontsExtensionFlags0Title,
                End),
                Child, (IPTR)MyLabel2(txtFontsExtensionFlags12),
                Child, (IPTR)(texts[ 1] = (Object *)FlagsButtonObject,
                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                    MUIA_FlagsButton_Title, (IPTR)txtFontsExtensionFlags1,
                    MUIA_FlagsButton_BitArray, (IPTR)fontExtensionFlags1,
                    MUIA_FlagsButton_WindowTitle, (IPTR)txtFontsExtensionFlags1Title,
                End),
                Child, (IPTR)MyLabel2(txtFontsExtensionTags),
                Child, (IPTR)(texts[ 2] = (Object *)DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End),
                Child, (IPTR)MyLabel2(txtFontsExtensionOFontPatchS),
                Child, (IPTR)(texts[ 3] = (Object *)DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End),
                Child, (IPTR)MyLabel2(txtFontsExtensionOFontPatchK),
                Child, (IPTR)(texts[ 4] = (Object *)DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End),
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

    examplegroup = (Object *)VGroup,
        GroupFrameT(txtFontsExample),
        Child, (IPTR)(example = (Object *)FontDisplayObject,
            MUIA_FontDisplay_Text, (IPTR)txtFontsExampleText,
            MUIA_Weight, 20,
        End),
    End;

    DoMethod(fdwd->fdwd_MainGroup, MUIM_Group_InitChange);
    DoMethod(fdwd->fdwd_MainGroup, OM_ADDMEMBER, examplegroup);
    DoMethod(fdwd->fdwd_MainGroup, MUIM_Group_ExitChange);

    set(example, MUIA_FontDisplay_Font, tf);

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtFontsDetailTitle, fe->fe_Name, fdwd->fdwd_Title, sizeof(fdwd->fdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, maingroup, texts[13], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Fonts",
        MUIA_Window_ID, MakeID('.','F','O','N'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(maingroup = (Object *)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtNodeName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsExtension2),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsYSize2),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsStyle2),
                            Child, (IPTR)(texts[ 4] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtFontsStyle,
                                MUIA_FlagsButton_BitArray, (IPTR)fontStyles,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtFontsStyleTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtFontsFlags2),
                            Child, (IPTR)(texts[ 5] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtFontsFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)fontFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtFontsFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtFontsXSize2),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsBaseline),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsBoldSmear),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsAccessors2),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsLoChar2),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsHiChar2),
                            Child, (IPTR)(texts[11] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtFontsModulo),
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
        struct FontsDetailWinData *fdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, fdwd->fdwd_Texts, sizeof(fdwd->fdwd_Texts));
        fdwd->fdwd_MainGroup = maingroup;

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
    struct FontsDetailWinData *fdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
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

APTR MakeFontsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct FontsDetailWinData), ENTRY(FontsDetailWinDispatcher));
}

