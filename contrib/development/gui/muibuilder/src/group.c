/***************************************************************************

  MUIBuilder - MUI interface builder
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$$

***************************************************************************/

#ifdef __MORPHOS__
#undef USE_INLINE_STDARG
#endif

#include "builder.h"

void InitGroup(group *group_aux)
{
    InitArea(&group_aux->Area);
    group_aux->Area.Background = MUII_WindowBack;
    group_aux->Area.Frame = MUIV_Frame_None;
    group_aux->child = create();
    group_aux->horizontal = FALSE;
    group_aux->registermode = FALSE;
    group_aux->sameheight = FALSE;
    group_aux->samewidth = FALSE;
    group_aux->samesize = FALSE;
    group_aux->virtual = FALSE;
    group_aux->columns = FALSE;
    group_aux->rows = FALSE;
    group_aux->horizspacing = FALSE;
    group_aux->vertspacing = FALSE;
    group_aux->number = 2;
    group_aux->horizspace = 0;
    group_aux->vertspace = 0;
    sprintf(group_aux->label, "GR_grp_%d", nb_group);
    strcpy(group_aux->Area.TitleFrame, "");
    group_aux->entries = create();
    group_aux->notify = create();
    group_aux->notifysource = create();
    group_aux->deplie = FALSE;
}

APTR LoadGroup(FILE *fichier, APTR father, int version)
{
    group *group_aux;
    char *chaine_aux;
    object *obj_aux;
    int i;
    BOOL title_exist;

    group_aux = AllocVec(sizeof(group), MEMF_PUBLIC | MEMF_CLEAR);
    InitGroup(group_aux);
    group_aux->id = TY_GROUP;
    group_aux->father = father;
    group_aux->child = create();
    group_aux->deplie = FALSE;
    strcpy(group_aux->label, LitChaine(fichier));
    group_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, group_aux);
    if (version > 104)
        ReadNotify(fichier, group_aux);
    if (version >= 113)
        ReadArea(fichier, &group_aux->Area);
    if (version < 113)
        strcpy(group_aux->Area.TitleFrame, LitChaine(fichier));
    group_aux->root = ReadInt(fichier);
    if (!group_aux->root)
        add(current_window->groups, group_aux);
    if (version < 113)
    {
        title_exist = ReadInt(fichier); /* obsolete title_exist */
        if (title_exist)
            group_aux->Area.Frame = MUIV_Frame_Group;
        else
            group_aux->Area.TitleFrame[0] = '\0';
    }
    group_aux->horizontal = ReadInt(fichier);
    group_aux->registermode = ReadInt(fichier);
    group_aux->sameheight = ReadInt(fichier);
    group_aux->samesize = ReadInt(fichier);
    group_aux->samewidth = ReadInt(fichier);
    group_aux->virtual = ReadInt(fichier);
    group_aux->rows = ReadInt(fichier);
    group_aux->horizspacing = ReadInt(fichier);
    group_aux->vertspacing = ReadInt(fichier);
    group_aux->columns = ReadInt(fichier);
    group_aux->number = ReadInt(fichier);
    if (version < 113)
        group_aux->Area.Weight = ReadInt(fichier);
    group_aux->horizspace = ReadInt(fichier);
    group_aux->vertspace = ReadInt(fichier);
    if (version > 100)
    {
        do
        {
            chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
            sprintf(chaine_aux, "%s", (char *) LitChaine(fichier));
            if (strcmp(chaine_aux, "//END_ENTRIES//") != 0)
            {
                add(group_aux->entries, chaine_aux);
            }
        }
        while (strcmp(chaine_aux, "//END_ENTRIES//") != 0);
        FreeVec(chaine_aux);
        for (i = 0; i < group_aux->entries->nb_elements; i++)
        {
            group_aux->registertitles[i] = nth(group_aux->entries, i);
        }
        group_aux->registertitles[i] = NULL;
    }
    do
    {
        i = ReadInt(fichier);
        if (i != -1)
        {
            if (!group_aux->root)
                obj_aux = LoadObject(fichier, group_aux, i);
            else
                obj_aux = LoadObject(fichier, &current_window->root, i);
            add(group_aux->child, obj_aux);
        }
    }
    while (i != -1);
    if (group_aux->samesize)
    {
        group_aux->sameheight = TRUE;
        group_aux->samewidth = TRUE;
    }
    group_aux->horizontal = (group_aux->horizontal && (!group_aux->rows)
                             && (!group_aux->columns));
    return (group_aux);
}

void SaveGroup(FILE *fichier, group *group_aux)
{
    int i;

    SaveArea(fichier, &group_aux->Area);
    fprintf(fichier, "%d\n", group_aux->root);
    fprintf(fichier, "%d\n", group_aux->horizontal);
    fprintf(fichier, "%d\n", group_aux->registermode);
    fprintf(fichier, "%d\n", group_aux->sameheight);
    fprintf(fichier, "%d\n", group_aux->samesize);
    fprintf(fichier, "%d\n", group_aux->samewidth);
    fprintf(fichier, "%d\n", group_aux->virtual);
    fprintf(fichier, "%d\n", group_aux->rows);
    fprintf(fichier, "%d\n", group_aux->horizspacing);
    fprintf(fichier, "%d\n", group_aux->vertspacing);
    fprintf(fichier, "%d\n", group_aux->columns);
    fprintf(fichier, "%d\n", group_aux->number);
    fprintf(fichier, "%d\n", group_aux->horizspace);
    fprintf(fichier, "%d\n", group_aux->vertspace);
    for (i = 0; i < group_aux->entries->nb_elements; i++)
    {
        fprintf(fichier, "%s\n", (char *) nth(group_aux->entries, i));
    }
    fprintf(fichier, "%s\n", "//END_ENTRIES//");
    for (i = 0; i < group_aux->child->nb_elements; i++)
    {
        SaveObject(nth(group_aux->child, i), fichier);
    }
    fprintf(fichier, "%d\n", -1);
}

BOOL NewGroup(group *group_aux, int root, BOOL new)
{
    APTR WI_group, RegGroup1, RegGroup2;
    APTR bt_okgroup, bt_cancelgroup;
    APTR CM_sens, CM_registermode, CM_sameheight, CM_samesize;
    APTR CM_samewidth, CM_virtual, CM_rows, CM_columns;
    APTR CM_horizspacing, CM_vertspacing, STR_horizspace, STR_vertspace;
    APTR STR_label, STR_rownumber, gr_buttons;
    APTR GR_register, lv_entries, bt_new, bt_remove, bt_up, bt_down,
        STR_entry;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    LONG active;
    int i, n;
    CONST_STRPTR aux;
    char *label_aux;
    LONG cy_active;
    BOOL dbclick;
    CONST_STRPTR titles[3];
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];
    queue *undo;

    titles[0] = GetMUIBuilderString(MSG_Attributes);
    titles[1] = GetMUIBuilderString(MSG_Register);
    titles[2] = NULL;

    if (new)
    {
        InitGroup(group_aux);
    }

    undo = CopyStringsList(group_aux->entries);

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&group_aux->Area, FALSE, TRUE, TRUE, FALSE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    GR_register = GroupObject,
        GroupFrameT(GetMUIBuilderString(MSG_RegisterEntries)),
        Child, HGroup,
            Child, lv_entries = ListviewObject,
                MUIA_Listview_List, ListObject,
                    InputListFrame,
                End,
            End,
            Child, gr_buttons = GroupObject,
                MUIA_Weight, 25,
                Child, bt_new = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_New)),
                Child, bt_remove = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Remove)),
                Child, bt_up = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Up)),
                Child, bt_down = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Down)),
                Child, HVSpace,
            End, End, Child, ColGroup(2), Child,
            Label2(GetMUIBuilderString(MSG_Entry)),
            Child, STR_entry = StringObject,
                StringFrame,
                MUIA_String_MaxLen, 80,
                MUIA_String_Format, 0,
            End,
        End,
    End;

    WI_group = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_GroupWindow),
        MUIA_Window_ID, MAKE_ID('G', 'R', 'O', 'U'),
        MUIA_HelpNode, "Group",
        WindowContents, VGroup,
            Child, RegGroup1 = RegisterGroup(RegisterTitles),
                Child, ColGroup(2),
                    MUIA_Group_SameHeight, TRUE,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_GroupAttributes)),
                        MUIA_HorizWeight, 25,
                        Child, Label1(GetMUIBuilderString(MSG_HorizGroup)),
                        Child, CM_sens = CheckMark(group_aux->horizontal),
                        Child, Label1(GetMUIBuilderString(MSG_PageMode)),
                        Child, CM_registermode = CheckMark(group_aux->registermode),
                        Child, Label1(GetMUIBuilderString(MSG_SameHeight)),
                        Child, CM_sameheight = CheckMark(group_aux->sameheight),
                        Child, Label1(GetMUIBuilderString(MSG_SameWidth)),
                        Child, CM_samewidth = CheckMark(group_aux->samewidth),
                        Child, Label1(GetMUIBuilderString(MSG_SameSize)),
                        Child, CM_samesize = CheckMark(group_aux->samesize),
                        Child, Label1(GetMUIBuilderString(MSG_VirtualGroup)),
                        Child, CM_virtual = CheckMark(group_aux->virtual),
                        Child, Label1(GetMUIBuilderString(MSG_Columns)),
                        Child, CM_columns = CheckMark(group_aux->columns),
                        Child, Label1(GetMUIBuilderString(MSG_Rows)),
                        Child, CM_rows = CheckMark(group_aux->rows),
                        Child, Label1(GetMUIBuilderString(MSG_HorizSpacing)),
                        Child, CM_horizspacing = CheckMark(group_aux->horizspacing),
                        Child, Label1(GetMUIBuilderString(MSG_VertSpacing)),
                        Child, CM_vertspacing = CheckMark(group_aux->vertspacing),
                        Child, HVSpace,
                        Child, HVSpace,
                    End,
                    Child, RegGroup2 = RegisterGroup(titles),
                        Child, VGroup,
                            Child, HVSpace,
                            Child, ColGroup(2),
                                GroupFrameT(GetMUIBuilderString(MSG_RowsColumns)),
                                Child, Label1(GetMUIBuilderString(MSG_Number)),
                                Child, STR_rownumber = StringObject,
                                    StringFrame,
                                    MUIA_String_Accept, "0123456789",
                                    MUIA_String_Integer, group_aux->number,
                                End,
                            End,
                            Child, HVSpace,
                            Child, ColGroup(2),
                                GroupFrameT(GetMUIBuilderString(MSG_Spacing)),
                                Child, TextObject,
                                    MUIA_Text_PreParse, "\033r",
                                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Horizontal),
                                    MUIA_Weight, 0,
                                    MUIA_InnerLeft, 0,
                                    MUIA_InnerRight, 0,
                                End,
                                Child, STR_horizspace = StringObject,
                                    StringFrame,
                                    MUIA_String_Accept, "0123456789",
                                    MUIA_String_Format, 0,
                                    MUIA_String_Integer, group_aux->horizspace,
                                End,
                                Child, TextObject,
                                    MUIA_Text_PreParse, "\033r",
                                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Vertical),
                                    MUIA_Weight, 0,
                                    MUIA_InnerLeft, 0,
                                    MUIA_InnerRight, 0,
                                End,
                                Child, STR_vertspace = StringObject,
                                    StringFrame,
                                    MUIA_String_Accept, "0123456789",
                                    MUIA_String_Format, 0,
                                    MUIA_String_Integer, group_aux->vertspace,
                                End,
                            End,
                            Child, HVSpace,
                            Child, ColGroup(2),
                                GroupFrameT(GetMUIBuilderString(MSG_GroupIdent)),
                                Child, Label1(GetMUIBuilderString(MSG_Label)),
                                Child, STR_label = StringObject,
                                    StringFrame,
                                    MUIA_String_Contents, group_aux->label,
                                    MUIA_ExportID, 1,
                                End,
                            End,
                        End,
                        Child, GR_register,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, HGroup,
                MUIA_Group_SameSize, TRUE,
                Child, bt_okgroup = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancelgroup = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_group;

    DoMethod(bt_okgroup, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancelgroup, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_group, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, TRUE, CM_columns, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, FALSE, CM_columns, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, TRUE, CM_rows, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, FALSE, CM_rows, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, TRUE, STR_rownumber, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, FALSE, STR_rownumber, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, TRUE, STR_rownumber,
             3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, FALSE, STR_rownumber,
             3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_horizspacing, MUIM_Notify, MUIA_Selected, TRUE,
             STR_horizspace, 3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_horizspacing, MUIM_Notify, MUIA_Selected, FALSE,
             STR_horizspace, 3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_vertspacing, MUIM_Notify, MUIA_Selected, TRUE,
             STR_vertspace, 3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_vertspacing, MUIM_Notify, MUIA_Selected, FALSE,
             STR_vertspace, 3, MUIM_Set, MUIA_Disabled, TRUE);
    /* Sens notifications */
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, TRUE, CM_rows, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, FALSE, CM_rows, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, TRUE, CM_rows, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, TRUE, CM_columns, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, FALSE, CM_columns, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, TRUE, CM_columns, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, TRUE, CM_registermode, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, FALSE, CM_registermode,
             3, MUIM_Set, MUIA_Disabled, group_aux->root);
    DoMethod(CM_sens, MUIM_Notify, MUIA_Selected, TRUE, CM_registermode, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    /* columns & rows notifications */
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, TRUE, CM_sens, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, TRUE, CM_sens, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, TRUE, CM_sens, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, TRUE, CM_sens, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, FALSE, CM_sens, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, FALSE, CM_sens, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, TRUE, CM_registermode,
             3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, TRUE, CM_registermode,
             3, MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, TRUE, CM_registermode, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, TRUE, CM_registermode, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_columns, MUIM_Notify, MUIA_Selected, FALSE,
             CM_registermode, 3, MUIM_Set, MUIA_Disabled, group_aux->root);
    DoMethod(CM_rows, MUIM_Notify, MUIA_Selected, FALSE, CM_registermode,
             3, MUIM_Set, MUIA_Disabled, group_aux->root);
    /* register notifications */
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, gr_buttons,
             3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, FALSE,
             gr_buttons, 3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, lv_entries,
             3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, FALSE,
             lv_entries, 3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, CM_rows, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, CM_rows, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, FALSE, CM_rows,
             3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, CM_columns,
             3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, CM_columns,
             3, MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, FALSE,
             CM_columns, 3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, CM_sens, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, TRUE, CM_sens, 3,
             MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(CM_registermode, MUIM_Notify, MUIA_Selected, FALSE, CM_sens,
             3, MUIM_Set, MUIA_Disabled, FALSE);
    /* buttons notifications */
    DoMethod(bt_remove, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DELETE);
    DoMethod(bt_down, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DOWN);
    DoMethod(bt_up, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_UP);
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, STR_entry, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DBCLICK2);
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, WI_group, 3,
             MUIM_Set, MUIA_Window_ActiveObject, STR_entry);
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, STR_entry, 3,
             MUIM_Set, MUIA_String_Contents, "");
    DoMethod(STR_entry, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(STR_entry, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, STR_entry, 3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
             STR_entry, 3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
             WI_group, 3, MUIM_Set, MUIA_Window_ActiveObject, STR_entry);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK);


    DoMethod(WI_group, MUIM_Window_SetCycleChain, RegGroup1,
             CM_sens, CM_registermode, CM_sameheight, CM_samewidth,
             CM_samesize, CM_virtual, CM_columns, CM_rows, CM_horizspacing,
             CM_vertspacing, RegGroup2, STR_rownumber, STR_horizspace,
             STR_vertspace, STR_label, lv_entries, bt_new, bt_remove,
             bt_up, bt_down, STR_entry, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame,
             bt_okgroup, bt_cancelgroup, NULL);

    set(CM_rows, MUIA_Selected, group_aux->rows);
    if (!group_aux->registermode)
    {
        set(gr_buttons, MUIA_Disabled, TRUE);
        set(lv_entries, MUIA_Disabled, TRUE);
    }
    set(CM_columns, MUIA_Selected, group_aux->columns);
    if (group_aux->rows)
    {
        set(CM_columns, MUIA_Disabled, TRUE);
        set(CM_sens, MUIA_Disabled, TRUE);
        set(CM_registermode, MUIA_Disabled, TRUE);
    }
    if (group_aux->columns)
    {
        set(CM_rows, MUIA_Disabled, TRUE);
        set(CM_sens, MUIA_Disabled, TRUE);
        set(CM_registermode, MUIA_Disabled, TRUE);
    }
    if (group_aux->registermode)
    {
        set(CM_rows, MUIA_Disabled, TRUE);
        set(CM_columns, MUIA_Disabled, TRUE);
        set(CM_sens, MUIA_Disabled, TRUE);
    }
    if (group_aux->horizontal)
    {
        set(CM_registermode, MUIA_Disabled, TRUE);
        set(CM_rows, MUIA_Disabled, TRUE);
        set(CM_columns, MUIA_Disabled, TRUE);
    }
    if (group_aux->root)
    {
        set(CM_registermode, MUIA_Disabled, TRUE);
        set(CM_virtual, MUIA_Disabled, TRUE);
    }
    set(STR_rownumber, MUIA_Disabled, TRUE);
    if (!group_aux->horizspacing)
        set(STR_horizspace, MUIA_Disabled, TRUE);
    if (!group_aux->vertspacing)
        set(STR_vertspace, MUIA_Disabled, TRUE);
    if (group_aux->rows || group_aux->columns)
        set(STR_rownumber, MUIA_Disabled, FALSE);

    if (!new)
    {
        for (i = 0; i < group_aux->entries->nb_elements; i++)
        {
            label_aux = nth(group_aux->entries, i);
            DoMethod(lv_entries, MUIM_List_Insert, &label_aux, 1,
                     MUIV_List_Insert_Bottom);
        }
    }

    set(STR_entry, MUIA_Disabled, TRUE);
    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_group);
    set(WI_group, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(group_aux->label, aux);
                    aux = GetStr(STR_rownumber);
                    group_aux->number = atoi(aux);
                    aux = GetStr(STR_horizspace);
                    group_aux->horizspace = atoi(aux);
                    aux = GetStr(STR_vertspace);
                    group_aux->vertspace = atoi(aux);
                    get(CM_sens, MUIA_Selected, &cy_active);
                    group_aux->horizontal = (cy_active == 1);
                    get(CM_registermode, MUIA_Selected, &cy_active);
                    group_aux->registermode = (cy_active == 1);
                    get(CM_sameheight, MUIA_Selected, &cy_active);
                    group_aux->sameheight = (cy_active == 1);
                    get(CM_samewidth, MUIA_Selected, &cy_active);
                    group_aux->samewidth = (cy_active == 1);
                    get(CM_samesize, MUIA_Selected, &cy_active);
                    group_aux->samesize = (cy_active == 1);
                    get(CM_virtual, MUIA_Selected, &cy_active);
                    group_aux->virtual = (cy_active == 1);
                    get(CM_rows, MUIA_Selected, &cy_active);
                    group_aux->rows = (cy_active == 1);
                    get(CM_columns, MUIA_Selected, &cy_active);
                    group_aux->columns = (cy_active == 1);
                    get(CM_horizspacing, MUIA_Selected, &cy_active);
                    group_aux->horizspacing = (cy_active == 1);
                    get(CM_vertspacing, MUIA_Selected, &cy_active);
                    group_aux->vertspacing = (cy_active == 1);
                    group_aux->root = (root == GR_ROOT);
                    if (group_aux->number < 1)
                        group_aux->number = 1;
                    ValidateArea(GR_GroupArea, &group_aux->Area);
                    if (undo)
                    {
                        for (i = 0; i < undo->nb_elements; i++)
                        {
                            FreeVec(nth(undo, 0));
                            delete_nth(undo, 0);
                        }
                        FreeVec(undo);
                    }
                    if (new)
                    {
                        nb_group++;
                    }
                    result = TRUE;
                    running = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedLabel));
                break;
            case ID_DBCLICK:
                get(lv_entries, MUIA_List_Active, &active);
                set(STR_entry, MUIA_String_Contents,
                    nth(group_aux->entries, active));
                dbclick = TRUE;
                break;
            case ID_DBCLICK2:
                dbclick = FALSE;
                break;
            case ID_NEWOBJ:
                aux = GetStr(STR_entry);
                if (strlen(aux) > 0)
                {
                    if (!dbclick)
                    {
                        label_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
                        strcpy(label_aux, aux);
                        set(lv_entries, MUIA_List_Quiet, TRUE);
                        DoMethod(lv_entries, MUIM_List_Insert, &label_aux,
                                 1, MUIV_List_Insert_Bottom);
                        add(group_aux->entries, label_aux);
                        set(lv_entries, MUIA_List_Quiet, FALSE);
                    }
                    else
                    {
                        get(lv_entries, MUIA_List_Active, &active);
                        strcpy(nth(group_aux->entries, active), aux);
                        DoMethod(lv_entries, MUIM_List_Redraw,
                                 MUIV_List_Redraw_Active);
                        dbclick = FALSE;
                    }
                }
                break;
            case ID_DOWN:
                get(lv_entries, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(lv_entries, MUIM_List_Exchange, active,
                             active + 1);
                    down(group_aux->entries, active);
                    if (active < group_aux->entries->nb_elements)
                        set(lv_entries, MUIA_List_Active, active + 1);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectEntry));
                break;
            case ID_UP:
                get(lv_entries, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(lv_entries, MUIM_List_Exchange, active - 1,
                             active);
                    up(group_aux->entries, active);
                    if (active > 0)
                        set(lv_entries, MUIA_List_Active, active - 1);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectEntry));
                break;
            case ID_DELETE:
                get(lv_entries, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    FreeVec(nth(group_aux->entries, active));
                    delete_nth(group_aux->entries, active);
                    DoMethod(lv_entries, MUIM_List_Remove,
                             MUIV_List_Remove_Active);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectEntry));
                break;
            case ID_END:
                if (undo)
                {
                    n = group_aux->entries->nb_elements;
                    for (i = 0; i < n; i++)
                    {
                        FreeVec(nth(group_aux->entries, 0));
                        delete_nth(group_aux->entries, 0);
                    }
                    FreeVec(group_aux->entries);
                    group_aux->entries = undo;
                }
                running = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    for (i = 0; i < group_aux->entries->nb_elements; i++)
    {
        group_aux->registertitles[i] = nth(group_aux->entries, i);
    }
    group_aux->registertitles[i] = NULL;
    set(WI_group, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_group);
    MUI_DisposeObject(WI_group);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
