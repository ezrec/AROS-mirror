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

void InitCycle(cycle *cycle_aux)
{
    cycle_aux->entries = create();
    sprintf(cycle_aux->label, "CY_label_%d", nb_cycle);
    InitArea(&cycle_aux->Area);
    cycle_aux->Area.Background = MUII_ButtonBack;
    cycle_aux->Area.Frame = MUIV_Frame_Button;
    cycle_aux->notify = create();
    cycle_aux->notifysource = create();
}

APTR LoadCycle(FILE *fichier, APTR father, int version)
{
    cycle *cycle_aux;
    int i;
    char *chaine_aux;

    cycle_aux = AllocVec(sizeof(cycle), MEMF_PUBLIC | MEMF_CLEAR);
    InitCycle(cycle_aux);
    cycle_aux->id = TY_CYCLE;
    cycle_aux->entries = create();
    cycle_aux->father = father;
    strcpy(cycle_aux->label, LitChaine(fichier));
    cycle_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, cycle_aux);
    if (version > 104)
        ReadNotify(fichier, cycle_aux);
    if (version < 113)
    {
        cycle_aux->Area.Weight = ReadInt(fichier);
    }
    if (version >= 113)
        ReadArea(fichier, &cycle_aux->Area);
    do
    {
        chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
        sprintf(chaine_aux, "%s", (char *) LitChaine(fichier));
        if (strcmp(chaine_aux, "//END_ENTRIES//") != 0)
        {
            add(cycle_aux->entries, chaine_aux);
        }
    }
    while (strcmp(chaine_aux, "//END_ENTRIES//") != 0);
    FreeVec(chaine_aux);
    for (i = 0; i < cycle_aux->entries->nb_elements; i++)
    {
        cycle_aux->STRA_cycle[i] = nth(cycle_aux->entries, i);
    }
    cycle_aux->STRA_cycle[i] = NULL;
    return (cycle_aux);
}

void SaveCycle(FILE *fichier, cycle *cycle_aux)
{
    int i;

    SaveArea(fichier, &cycle_aux->Area);
    for (i = 0; i < cycle_aux->entries->nb_elements; i++)
    {
        fprintf(fichier, "%s\n", (char *) nth(cycle_aux->entries, i));
    }
    fprintf(fichier, "%s\n", "//END_ENTRIES//");
}

BOOL NewCycle(cycle *cycle_aux, BOOL new)
{
    APTR WI_cycle, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR bt_new, bt_remove, bt_up, bt_down;
    APTR STR_entry, STR_label;
    APTR lv_entries;
    APTR label_aux;
    ULONG signal;
    LONG active;
    BOOL running = TRUE;
    BOOL result = FALSE;
    BOOL dbclick = FALSE;
    CONST_STRPTR aux;
    int i, n;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];
    queue *undo;

    if (new)
    {
        InitCycle(cycle_aux);
    }

    undo = CopyStringsList(cycle_aux->entries);

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&cycle_aux->Area, FALSE, FALSE, FALSE, TRUE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_cycle = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_CycleChoice),
        MUIA_Window_ID, MAKE_ID('C', 'Y', 'C', 'L'),
        MUIA_HelpNode, "Cycle",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, VGroup, GroupFrameT(GetMUIBuilderString(MSG_CycleEntries)),
                        Child, HGroup,
                            Child, lv_entries = ListviewObject,
                                MUIA_Listview_List, ListObject,
                                    InputListFrame,
                                    MUIA_Listview_DoubleClick, TRUE,
                                End,
                            End,
                            Child, VGroup,
                                MUIA_Weight, 20,
                                Child, bt_new = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_New)),
                                Child, bt_remove = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Remove)),
                                Child, bt_up = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Up)),
                                Child, bt_down = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Down)),
                                Child, HVSpace,
                            End,
                        End,
                        Child, HGroup,
                            Child, Label1(GetMUIBuilderString(MSG_Entry)),
                            Child, STR_entry = StringObject,
                                StringFrame,
                            End,
                        End,
                    End,
                    Child, ColGroup(2),
                        Child, Label1(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, cycle_aux->label,
                        End,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, HGroup,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_cycle;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
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
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, STR_entry, 3,
             MUIM_Set, MUIA_String_Contents, "");
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, WI_cycle, 3,
             MUIM_Set, MUIA_Window_ActiveObject, STR_entry);
    DoMethod(STR_entry, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(STR_entry, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, STR_entry, 3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(WI_cycle, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
             STR_entry, 3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
             WI_cycle, 3, MUIM_Set, MUIA_Window_ActiveObject, STR_entry);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK);

    DoMethod(WI_cycle, MUIM_Window_SetCycleChain, RegGroup, lv_entries,
             bt_new, bt_remove, bt_up, bt_down, STR_entry, STR_label,
             bt_ok, bt_cancel, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, NULL);

    set(STR_entry, MUIA_Disabled, TRUE);
    if (!new)
    {
        for (i = 0; i < cycle_aux->entries->nb_elements; i++)
        {
            label_aux = nth(cycle_aux->entries, i);
            DoMethod(lv_entries, MUIM_List_Insert, &label_aux, 1,
                     MUIV_List_Insert_Bottom);
        }
    }

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_cycle);
    set(WI_cycle, MUIA_Window_Open, TRUE);

    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_DBCLICK:
                get(lv_entries, MUIA_List_Active, &active);
                set(STR_entry, MUIA_String_Contents,
                    nth(cycle_aux->entries, active));
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
                        set(lv_entries, MUIA_List_Quiet, FALSE);
                        add(cycle_aux->entries, label_aux);
                    }
                    else
                    {
                        get(lv_entries, MUIA_List_Active, &active);
                        strcpy(nth(cycle_aux->entries, active), aux);
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
                    down(cycle_aux->entries, active);
                    if (active < cycle_aux->entries->nb_elements)
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
                    up(cycle_aux->entries, active);
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
                    FreeVec(nth(cycle_aux->entries, active));
                    delete_nth(cycle_aux->entries, active);
                    DoMethod(lv_entries, MUIM_List_Remove,
                             MUIV_List_Remove_Active);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectEntry));
                break;
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if ((strlen(cycle_aux->label) > 0)
                    && (cycle_aux->entries->nb_elements > 0))
                {
                    strcpy(cycle_aux->label, aux);
                    ValidateArea(GR_GroupArea, &cycle_aux->Area);
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
                        nb_cycle++;
                    }
                    result = TRUE;
                    running = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_NeedLabelEntries));
                break;
            case ID_END:
                if (undo)
                {
                    n = cycle_aux->entries->nb_elements;
                    for (i = 0; i < n; i++)
                    {
                        FreeVec(nth(cycle_aux->entries, 0));
                        delete_nth(cycle_aux->entries, 0);
                    }
                    FreeVec(cycle_aux->entries);
                    cycle_aux->entries = undo;
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
    for (i = 0; i < cycle_aux->entries->nb_elements; i++)
    {
        cycle_aux->STRA_cycle[i] = nth(cycle_aux->entries, i);
    }
    cycle_aux->STRA_cycle[i] = NULL;
    set(WI_cycle, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_cycle);
    MUI_DisposeObject(WI_cycle);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
