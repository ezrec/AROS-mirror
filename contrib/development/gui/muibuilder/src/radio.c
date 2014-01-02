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

void InitRadio(radio *radio_aux)
{
    radio_aux->entries = create();
    sprintf(radio_aux->label, "RA_label_%d", nb_radio);
    InitArea(&radio_aux->Area);
    sprintf(radio_aux->Area.TitleFrame, "radio%d_title", nb_radio);
    radio_aux->Area.Background = MUII_WindowBack;
    radio_aux->Area.Frame = MUIV_Frame_Group;
    radio_aux->notify = create();
    radio_aux->notifysource = create();
}

queue *CopyStringsList(queue *list)
{
    int i;
    queue *result;
    APTR element;
    char *string;

    if (result = create())
        for (i = 0; i < list->nb_elements; i++)
        {
            string = nth(list, i);
            if (element =
                AllocVec(strlen(string) + 1, MEMF_PUBLIC | MEMF_CLEAR))
            {
                strcpy(element, string);
                add(result, element);
            }
        }
    return (result);
}

APTR LoadRadio(FILE *fichier, APTR father, int version)
{
    radio *radio_aux;
    int i;
    char *chaine_aux;

    radio_aux = AllocVec(sizeof(radio), MEMF_PUBLIC | MEMF_CLEAR);
    InitRadio(radio_aux);
    radio_aux->id = TY_RADIO;
    radio_aux->entries = create();
    radio_aux->father = father;
    strcpy(radio_aux->label, LitChaine(fichier));
    radio_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, radio_aux);
    if (version > 104)
        ReadNotify(fichier, radio_aux);
    if (version < 113)
    {
        strcpy(radio_aux->Area.TitleFrame, LitChaine(fichier));
        radio_aux->Area.Weight = ReadInt(fichier);
    }
    if (version >= 113)
        ReadArea(fichier, &radio_aux->Area);
    do
    {
        chaine_aux = AllocVec(80, MEMF_PUBLIC | MEMF_CLEAR);
        sprintf(chaine_aux, "%s", (char *) LitChaine(fichier));
        if (strcmp(chaine_aux, "//END_ENTRIES//") != 0)
        {
            add(radio_aux->entries, chaine_aux);
        }
    }
    while (strcmp(chaine_aux, "//END_ENTRIES//") != 0);
    FreeVec(chaine_aux);
    for (i = 0; i < radio_aux->entries->nb_elements; i++)
    {
        radio_aux->STRA_radio[i] = nth(radio_aux->entries, i);
    }
    radio_aux->STRA_radio[i] = NULL;
    return (radio_aux);
}

void SaveRadio(FILE *fichier, radio *radio_aux)
{
    int i;

    SaveArea(fichier, &radio_aux->Area);
    for (i = 0; i < radio_aux->entries->nb_elements; i++)
    {
        fprintf(fichier, "%s\n", (char *) nth(radio_aux->entries, i));
    }
    fprintf(fichier, "%s\n", "//END_ENTRIES//");
}

BOOL NewRadio(radio *radio_aux, BOOL new)
{
    APTR WI_radio, RegGroup;
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
        InitRadio(radio_aux);
    }

    undo = CopyStringsList(radio_aux->entries);

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&radio_aux->Area, TRUE, TRUE, TRUE, TRUE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_radio = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_RadioChoices),
        MUIA_Window_ID, MAKE_ID('R', 'A', 'D', 'I'),
        MUIA_HelpNode, "Radio",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, VGroup,
                        GroupFrameT(GetMUIBuilderString(MSG_RadioEntries)),
                        Child, HGroup,
                            Child, lv_entries = ListviewObject,
                                MUIA_Listview_List, ListObject,
                                    InputListFrame,
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
                            MUIA_String_Contents, radio_aux->label,
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

    WI_current = WI_radio;

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
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, WI_radio, 3,
             MUIM_Set, MUIA_Window_ActiveObject, STR_entry);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK);
    DoMethod(STR_entry, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(STR_entry, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, STR_entry, 3, MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
             STR_entry, 3, MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(lv_entries, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
             WI_radio, 3, MUIM_Set, MUIA_Window_ActiveObject, STR_entry);
    DoMethod(WI_radio, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);

    DoMethod(WI_radio, MUIM_Window_SetCycleChain, RegGroup, lv_entries,
             bt_new, bt_remove, bt_up, bt_down, STR_entry, STR_label,
             GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, bt_ok, bt_cancel, NULL);

    if (!new)
    {
        for (i = 0; i < radio_aux->entries->nb_elements; i++)
        {
            label_aux = nth(radio_aux->entries, i);
            DoMethod(lv_entries, MUIM_List_Insert, &label_aux, 1,
                     MUIV_List_Insert_Bottom);
        }
    }
    set(STR_entry, MUIA_Disabled, TRUE);
    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_radio);
    set(WI_radio, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_DBCLICK:
                get(lv_entries, MUIA_List_Active, &active);
                set(STR_entry, MUIA_String_Contents,
                    nth(radio_aux->entries, active));
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
                        add(radio_aux->entries, label_aux);
                    }
                    else
                    {
                        get(lv_entries, MUIA_List_Active, &active);
                        strcpy(nth(radio_aux->entries, active), aux);
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
                    down(radio_aux->entries, active);
                    if (active < radio_aux->entries->nb_elements)
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
                    up(radio_aux->entries, active);
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
                    FreeVec(nth(radio_aux->entries, active));
                    delete_nth(radio_aux->entries, active);
                    DoMethod(lv_entries, MUIM_List_Remove,
                             MUIV_List_Remove_Active);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectEntry));
                break;
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if ((strlen(aux) > 0)
                    && (radio_aux->entries->nb_elements > 0))
                {
                    strcpy(radio_aux->label, aux);
                    ValidateArea(GR_GroupArea, &radio_aux->Area);
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
                        nb_radio++;
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
                    n = radio_aux->entries->nb_elements;
                    for (i = 0; i < n; i++)
                    {
                        FreeVec(nth(radio_aux->entries, 0));
                        delete_nth(radio_aux->entries, 0);
                    }
                    FreeVec(radio_aux->entries);
                    radio_aux->entries = undo;
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
    for (i = 0; i < radio_aux->entries->nb_elements; i++)
    {
        radio_aux->STRA_radio[i] = nth(radio_aux->entries, i);
    }
    radio_aux->STRA_radio[i] = NULL;
    set(WI_radio, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_radio);
    MUI_DisposeObject(WI_radio);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
