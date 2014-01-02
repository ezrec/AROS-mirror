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

#include "notify.h"
#include "notifyconstants.h"

/* does a string already exist in a list ? */
BOOL ExistInList(CONST_STRPTR chaine, queue *list)
{
    BOOL result = FALSE;
    int i;
    chainon *chainon_aux;

    chainon_aux = list->head;
    for (i = 0; i < list->nb_elements; i++)
    {
        result = (strcmp(chaine, chainon_aux->element) == 0);
        if (result)
            break;
        chainon_aux = chainon_aux->next;
    }
    return (result);
}

/* Add an element in list if it doesn't exist */
BOOL AddInList(char *chaine, queue *list)
{
    BOOL result;

    result = (!ExistInList(chaine, list));
    if (result)
        add(list, chaine);
    return (result);
}

/* Get the pointer in the list for the string 'chaine' */
APTR PointerOnString(char *chaine, queue *list)
{
    int i;
    APTR result;
    chainon *chainon_aux;

    chainon_aux = list->head;
    for (i = 0; i < list->nb_elements; i++)
    {
        result = chainon_aux->element;
        if (strcmp(chaine, result) == 0)
            break;
        chainon_aux = chainon_aux->next;
    }
    if (i != list->nb_elements)
        return (result);
    else
        return (NULL);
}

/* returns a pointer on an object which uses the same argument */
APTR FindArgument(char *chaine)
{
    int i, j;
    object *obj;
    event *evt;
    listview *list_aux;
    popaslobj *popasl;
    popobject *popobj;
    APTR result = NULL;
    chainon *chainon_aux;

    chainon_aux = labels->head;
    for (i = 0; i < labels->nb_elements; i++)
    {
        obj = chainon_aux->element;
        switch (obj->id)
        {
            case TY_LISTVIEW:
                list_aux = (listview *) obj;
                if ((list_aux->comparehook == chaine) ||
                    (list_aux->constructhook == chaine) ||
                    (list_aux->displayhook == chaine) ||
                    (list_aux->destructhook == chaine) ||
                    (list_aux->multitesthook == chaine))
                    result = obj;
                break;
            case TY_POPOBJECT:
                popobj = (popobject *) obj;
                if ((popobj->openhook == chaine) ||
                    (popobj->closehook == chaine))
                    result = obj;
                break;
            case TY_POPASL:
                popasl = (popaslobj *) obj;
                if ((popasl->starthook == chaine) ||
                    (popasl->stophook == chaine))
                    result = obj;
                break;
        }
        if (!result)
            for (j = 0; j < obj->notify->nb_elements; j++)
            {
                evt = nth(obj->notify, j);
                switch (TYActions[evt->destination->id][evt->dest_type])
                {
                    case TY_FUNCTION:
                    case TY_ID:
                    case TY_VARIABLE:
                        if (chaine == evt->arguments)
                            result = obj;
                        break;
                }
                if (result)
                    break;
            }
        if (result)
            break;
        chainon_aux = chainon_aux->next;
    }
    return (result);
}

/* get an argument : could be a function, a constant ID, depending on the given list */
BOOL GetMUIarg(object *obj_aux, APTR * stringptr, char *stringname,
               queue *list, BOOL old, CONST_STRPTR title)
{
    ULONG signal;
    LONG active;
    BOOL running = TRUE;
    APTR WI_MUIarg, LV_labels, BT_ok, BT_cancel;
    APTR BT_remove, STR_add, TX_Text;
    CONST_STRPTR chaine_aux;
    char *label_aux;
    BOOL result = TRUE;
    int i;

    // *INDENT-OFF*
    WI_MUIarg = WindowObject,
        MUIA_Window_Title, title,
        MUIA_Window_ID, MAKE_ID('M', 'A', 'R', 'G'),
        MUIA_HelpNode, "Notification",
        WindowContents, GroupObject,
            Child, TX_Text = TextObject,
                MUIA_Text_PreParse, "\033c",
                TextFrame,
            End,
            Child, LV_labels = ListviewObject,
                MUIA_Listview_List, ListObject,
                    InputListFrame,
                End,
            End,
            Child, HGroup,
                Child, Label(GetMUIBuilderString(MSG_AddToList)),
                Child, STR_add = String("", 80),
            End,
            Child, GroupObject,
                MUIA_Group_Horiz, TRUE,
                MUIA_Group_SameWidth, TRUE,
                Child, BT_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, BT_remove = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_RemoveFromList)),
            End,
            Child, BT_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Quit)),
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_MUIarg;

    DoMethod(WI_MUIarg, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_END);
    DoMethod(LV_labels, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(LV_labels, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app,
             2, MUIM_Application_ReturnID, ID_INFO);
    DoMethod(STR_add, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             app, 2, MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(BT_remove, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DELETE);
    DoMethod(BT_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);

    DoMethod(WI_MUIarg, MUIM_Window_SetCycleChain, LV_labels, STR_add,
             BT_remove, BT_ok, BT_cancel, NULL);

    delete_all(labels);
    CreateLabels(&application, NULL);
    if (obj_aux)
        CreateLabels(obj_aux, NULL);
    for (i = 0; i < list->nb_elements; i++)
    {
        chaine_aux = nth(list, i);
        DoMethod(LV_labels, MUIM_List_Insert, &chaine_aux, 1,
                 MUIV_List_Insert_Bottom);
    }

    if ((old) && (*stringptr))
    {
        set(TX_Text, MUIA_Text_Contents, *stringptr);
        for (i = 0; i < list->nb_elements; i++)
        {
            chaine_aux = nth(list, i);
            if (strcmp(chaine_aux, *stringptr) == 0)
            {
                set(LV_labels, MUIA_List_Active, i);
                break;
            }
        }
    }

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_MUIarg);
    set(WI_MUIarg, MUIA_Window_Open, TRUE);

    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_INFO:
                DoMethod(LV_labels, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &chaine_aux);
                set(STR_add, MUIA_String_Contents, chaine_aux);
                break;
            case ID_OKWIN:
                get(LV_labels, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    if (stringptr)
                        *stringptr = nth(list, active);
                    if (stringname)
                        strcpy(stringname, nth(list, active));
                    set(TX_Text, MUIA_Text_Contents, *stringptr);
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_SelectIdentifier));
                break;
            case ID_NEWOBJ:
                chaine_aux = GetStr(STR_add);
                if (strlen(chaine_aux) > 0)
                {
                    if (!ExistInList(chaine_aux, list))
                    {
                        label_aux =
                            AllocVec(strlen(chaine_aux) + 1,
                                     MEMF_PUBLIC | MEMF_CLEAR);
                        if (!label_aux)
                        {
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_NotEnoughMemory));
                            break;
                        }
                        strcpy(label_aux, chaine_aux);
                        add(list, label_aux);
                        DoMethod(LV_labels, MUIM_List_Insert, &label_aux,
                                 1, MUIV_List_Insert_Bottom);
                        set(LV_labels, MUIA_List_Active,
                            MUIV_List_Active_Bottom);
                    }
                    else
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_IdentifierExists));
                }
                break;
            case ID_DELETE:
                get(LV_labels, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    if (stringptr)
                        if (*stringptr == nth(list, active))
                        {
                            *stringptr = NULL;
                            if (stringname)
                                stringname[0] = '\0';
                            set(TX_Text, MUIA_Text_Contents, stringname);
                            result = FALSE;
                        }
                    if (!FindArgument(nth(list, active)))
                    {
                        FreeVec(nth(list, active));
                        delete_nth(list, active);
                        DoMethod(LV_labels, MUIM_List_Remove,
                                 MUIV_List_Remove_Active);
                        set(LV_labels, MUIA_List_Active,
                            MUIV_List_Active_Off);
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_SelectIdentifier));
                break;
            case ID_END:
                result = (*stringptr != NULL);
                running = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    set(WI_MUIarg, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_MUIarg);
    MUI_DisposeObject(WI_MUIarg);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}

/* get a constant of a given type (bool, integer, string, ... ) */
BOOL GetConstant(event *evt, int Type, BOOL old)
{
    ULONG signal;
    BOOL running = TRUE;
    APTR WI_Constant, STR_Integer, STR_String, STR_Char, CY_Bool;
    APTR bt_ok, bt_cancel;
    char *STR_CY_Bool[3];
    int int_aux;
    char *string_aux;
    BOOL result = FALSE;
    APTR ActiveObject;

    STR_CY_Bool[0] = "FALSE";
    STR_CY_Bool[1] = "TRUE";
    STR_CY_Bool[2] = NULL;

    // *INDENT-OFF*
    WI_Constant = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_GetConstant),
        MUIA_Window_ID, MAKE_ID('C', 'O', 'N', 'S'),
        MUIA_HelpNode, "Constants",
        WindowContents, GroupObject,
            Child, GroupObject,
                MUIA_Group_Columns, 2,
                Child, Label(GetMUIBuilderString(MSG_IntegerConstant)),
                Child, STR_Integer = StringObject,
                    StringFrame,
                    MUIA_String_Accept, "-0123456789",
                    MUIA_String_MaxLen, 80,
                    MUIA_String_Format, 0,
                    MUIA_Disabled, (Type != TY_CONS_INT),
                End,
                Child, Label(GetMUIBuilderString(MSG_StringConstant)),
                Child, STR_String = StringObject,
                    StringFrame,
                    MUIA_String_MaxLen, 80,
                    MUIA_String_Format, 0,
                    MUIA_Disabled, (Type != TY_CONS_STRING),
                End,
                Child, Label(GetMUIBuilderString(MSG_CharConstant)),
                Child, STR_Char = StringObject,
                    StringFrame,
                    MUIA_String_MaxLen, 2,
                    MUIA_String_Format, 0,
                    MUIA_Disabled, (Type != TY_CONS_CHAR),
                End,
                Child, Label(GetMUIBuilderString(MSG_BooleanConstant)),
                Child, CY_Bool = CycleObject,
                    MUIA_Cycle_Entries, STR_CY_Bool,
                    MUIA_Disabled, (Type != TY_CONS_BOOL),
                End,
            End,
            Child, HGroup,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_Constant;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_Constant, MUIM_Window_SetCycleChain, STR_Integer,
             STR_String, STR_Char, CY_Bool, bt_ok, bt_cancel, NULL);

    switch (Type)
    {
        case TY_CONS_INT:
            if (old)
                set(STR_Integer, MUIA_String_Contents, evt->argstring);
            ActiveObject = STR_Integer;
            break;
        case TY_CONS_BOOL:
            if (old)
                set(CY_Bool, MUIA_Cycle_Active, atoi(evt->argstring));
            ActiveObject = CY_Bool;
            break;
        case TY_CONS_STRING:
            if (old)
                set(STR_String, MUIA_String_Contents, evt->argstring);
            ActiveObject = STR_String;
            break;
        case TY_CONS_CHAR:
            if (old)
                set(STR_Char, MUIA_String_Contents, evt->argstring);
            ActiveObject = STR_Char;
            break;
        default:
            ErrorMessage(GetMUIBuilderString(MSG_InternalError));
    }
    set(WI_Constant, MUIA_Window_ActiveObject, ActiveObject);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_Constant);
    set(WI_Constant, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                switch (Type)
                {
                    case TY_CONS_BOOL:
                        get(ActiveObject, MUIA_Cycle_Active, &int_aux);
                        sprintf(evt->argstring, "%d", int_aux);
                        break;
                    default:
                        get(ActiveObject, MUIA_String_Contents,
                            &string_aux);
                        sprintf(evt->argstring, "%s", string_aux);
                        break;
                }
                running = FALSE;
                result = TRUE;
                break;
            case ID_END:
                running = FALSE;
                result = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    set(WI_Constant, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_Constant);
    MUI_DisposeObject(WI_Constant);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
