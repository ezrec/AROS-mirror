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

char description[256];

APTR SearchLabel(char *objlabel, int depart)
{
    int i;
    object *obj_aux;
    APTR result = NULL;

    for (i = depart; (i < labels->nb_elements) && (!result); i++)
    {
        obj_aux = nth(labels, i);
        if (strcmp(objlabel, obj_aux->label) == 0)
        {
            result = obj_aux;
        }
    }
    return (result);
}

void LinkNotify(int depart)
{
    int i, j, k;
    object *obj_aux, *obj_aux2;
    event *evt;
    char *string_aux;

    for (i = depart; i < labels->nb_elements; i++)
    {
        obj_aux = nth(labels, i);
        for (j = 0; j < obj_aux->notify->nb_elements; j++)
        {
            evt = nth(obj_aux->notify, j);
            switch (TYActions[evt->id_cible][evt->dest_type])
            {
                case TY_WINOBJ:
                    if (!
                        (evt->destination =
                         SearchLabel(evt->label_cible, depart)))
                        evt->destination =
                            SearchLabel(evt->label_cible, 0);
                    evt->arguments = ParentWindow(evt->destination);
                    add(evt->destination->notifysource, obj_aux);
                    break;
                case TY_ID:
                    if (!
                        (evt->destination =
                         SearchLabel(evt->label_cible, depart)))
                        evt->destination =
                            SearchLabel(evt->label_cible, 0);
                    add(evt->destination->notifysource, obj_aux);
                    for (k = 0; k < application.Idents->nb_elements; k++)
                    {
                        string_aux = nth(application.Idents, k);
                        if (strcmp(evt->argstring, string_aux) == 0)
                        {
                            evt->arguments = string_aux;
                            break;
                        }
                    }
                    break;
                case TY_VARIABLE:
                    if (!
                        (evt->destination =
                         SearchLabel(evt->label_cible, depart)))
                        evt->destination =
                            SearchLabel(evt->label_cible, 0);
                    add(evt->destination->notifysource, obj_aux);
                    for (k = 0; k < application.Variables->nb_elements;
                         k++)
                    {
                        string_aux = nth(application.Variables, k);
                        if (strcmp(evt->argstring, string_aux) == 0)
                        {
                            evt->arguments = string_aux;
                            break;
                        }
                    }
                    break;
                case TY_FUNCTION:
                    if (!
                        (evt->destination =
                         SearchLabel(evt->label_cible, depart)))
                        evt->destination =
                            SearchLabel(evt->label_cible, 0);
                    add(evt->destination->notifysource, obj_aux);
                    for (k = 0; k < application.Functions->nb_elements;
                         k++)
                    {
                        string_aux = nth(application.Functions, k);
                        if (strcmp(evt->argstring, string_aux) == 0)
                        {
                            evt->arguments = string_aux;
                            break;
                        }
                    }
                    break;
                default:
                    if (!
                        (evt->destination =
                         SearchLabel(evt->label_cible, depart)))
                        evt->destination =
                            SearchLabel(evt->label_cible, 0);
                    add(evt->destination->notifysource, obj_aux);
                    break;
            }
            obj_aux2 = evt->destination;
            sprintf(description, "%s (%s) => %s (%s)",
                    Evenements[obj_aux->id][evt->src_type], obj_aux->label,
                    Actions[obj_aux2->id][evt->dest_type],
                    obj_aux2->label);
            strcpy(evt->description, description);
        }
    }
}

window *ParentWindow(APTR obj)
{
    if (obj == NULL) // FIX for a segfault
        return NULL;

    object *obj_aux;

    obj_aux = obj;

    if (obj_aux->id == TY_WINDOW)
        return ((window *) obj_aux);
    if (obj_aux->id == TY_APPLI)
        return (NULL);
    return (ParentWindow(obj_aux->father));
}

LONG Position(APTR obj, APTR list)
{
    int i, n;
    APTR aux;
    int result = -1;

    get(list, MUIA_List_Entries, &n);
    for (i = 0; i < n; i++)
    {
        DoMethod(list, MUIM_List_GetEntry, i, &aux);
        if (obj == aux)
        {
            result = i;
            break;
        }
    }
    return (result);
}

//__asm __saveds LONG DisplayFunc( register __a2 char **array, register __a1 object* obj )
//LONG DisplayFunc(char **array __asm("a2"), object *obj __asm("a1"))
HOOKPROTONH(DisplayFunc, LONG, char **array, object *obj)
{
    static char buffer[10], buffer2[82];
    window *win_aux;

    if (obj->id == TY_WINDOW)
    {
        win_aux = (window *) obj;
        if (!win_aux->deplie)
            strcpy(buffer, "\033I[6:30] ");
        else
            strcpy(buffer, "\033I[6:39] ");
        sprintf(buffer2, "\0338%s", obj->label);
    }
    else
    {
        buffer[0] = '\0';
        if (obj->id == TY_APPLI)
            sprintf(buffer2, "\0338%s", obj->label);
        else if (obj->id == TY_GROUP)
            sprintf(buffer2, "\0338%s", obj->label);
        else
            sprintf(buffer2, "%s", obj->label);
    }

    *array++ = buffer;
    *array = buffer2;

    return (0);
}

void InitLabels(APTR obj_aux, window *win, APTR list)
{
    object *obj;
    window *win_aux;
    menu *menu_aux;
    int i;

    obj = obj_aux;
    switch (obj->id)
    {
        case TY_APPLI:
            DoMethod(list, MUIM_List_Insert, &obj_aux, 1,
                     MUIV_List_Insert_Bottom);
            /* all windows EXCEPT win */
            if (application.appmenu->childs->nb_elements > 0)
                InitLabels(application.appmenu, NULL, list);
            for (i = 0; i < windows->nb_elements; i++)
            {
                win_aux = nth(windows, i);
                if (win_aux != win)
                    InitLabels(win_aux, win, list);
            }
            break;
        case TY_WINDOW:
            win_aux = obj_aux;
            win_aux->deplie = FALSE;
            DoMethod(list, MUIM_List_Insert, &obj_aux, 1,
                     MUIV_List_Insert_Bottom);
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = obj_aux;
            DoMethod(list, MUIM_List_Insert, &obj_aux, 1,
                     MUIV_List_Insert_Bottom);
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                InitLabels(nth(menu_aux->childs, i), NULL, list);
            }
            break;
        case TY_MENUITEM:
            DoMethod(list, MUIM_List_Insert, &obj_aux, 1,
                     MUIV_List_Insert_Bottom);
            break;
    }
}

void UpdateLabels(APTR obj, APTR list, ULONG pos)
{
    object *obj_aux;
    window *win_aux;
    group *group_aux;
    popobject *popobj_aux;
    menu *menu_aux;
    int i;

    obj_aux = obj;
    switch (obj_aux->id)
    {
        case TY_WINDOW:
            i = pos;
            set(list, MUIA_List_Active, MUIV_List_Active_Off);
            win_aux = obj;
            if (!win_aux->deplie)
            {
                DoMethod(list, MUIM_List_Remove, i);
                win_aux->deplie = TRUE;
                DoMethod(list, MUIM_List_Insert, &win_aux, 1, i);
                UpdateLabels(&win_aux->root, list, i + 1);
                if (win_aux->menu->childs->nb_elements > 0)
                    UpdateLabels(win_aux->menu, list, i + 1);
                set(list, MUIA_List_Active, i);
            }
            else
            {
                DoMethod(list, MUIM_List_Remove, i);
                win_aux->deplie = FALSE;
                DoMethod(list, MUIM_List_Insert, &win_aux, 1, i++);
                DoMethod(list, MUIM_List_GetEntry, i, &obj_aux);
                while ((obj_aux->id != TY_WINDOW)
                       && (obj_aux->id != TY_APPLI))
                {
                    DoMethod(list, MUIM_List_Remove, i);
                    DoMethod(list, MUIM_List_GetEntry, i, &obj_aux);
                    if (!obj)
                        break;
                }
                set(list, MUIA_List_Active, --i);
            }
            break;
        case TY_GROUP:
            group_aux = obj;
            for (i = group_aux->child->nb_elements - 1; i >= 0; i--)
            {
                obj_aux = nth(group_aux->child, i);
                UpdateLabels(obj_aux, list, pos);
            }
            DoMethod(list, MUIM_List_Insert, &group_aux, 1, pos);
            break;
        case TY_POPOBJECT:
            popobj_aux = obj;
            UpdateLabels(popobj_aux->obj, list, pos);
            DoMethod(list, MUIM_List_Insert, &obj_aux, 1, pos);
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = obj;
            for (i = menu_aux->childs->nb_elements; i >= 0; i--)
            {
                UpdateLabels(nth(menu_aux->childs, i), list, pos);
            }
            DoMethod(list, MUIM_List_Insert, &obj_aux, 1, pos);
            break;
        default:
            DoMethod(list, MUIM_List_Insert, &obj_aux, 1, pos);
            break;
    }
}

void WriteInList(APTR list, CONST_STRPTR chaines[])
{
    int i;

    set(list, MUIA_List_Quiet, TRUE);
    DoMethod(list, MUIM_List_Clear);

    for (i = 0; chaines[i] != NULL; i++)
    {
        DoMethod(list, MUIM_List_Insert, &chaines[i], 1,
                 MUIV_List_Insert_Bottom);
    }
    set(list, MUIA_List_Quiet, FALSE);
}

void ObjectEvents(object *obj, APTR list)
{
    if (obj->id <= Current_Max)
        WriteInList(list, Evenements[obj->id]);
    else
        ErrorMessage(GetMUIBuilderString(MSG_InternalError));
}

void ObjectActions(object *obj, APTR list)
{
    if (obj->id <= Current_Max)
        WriteInList(list, Actions[obj->id]);
    else
        ErrorMessage(GetMUIBuilderString(MSG_InternalError));
}

BOOL NotifyExist(object *obj, object *obj_dest, int src_type,
                 int dest_type)
{
    int i;
    BOOL result = FALSE;
    event *evenement;

    for (i = 0; (i < obj->notify->nb_elements) && (!result); i++)
    {
        evenement = nth(obj->notify, i);
        result = (evenement->destination == obj_dest) &&
            (evenement->src_type == src_type) &&
            (evenement->dest_type == dest_type);
    }
    return (result);
}

BOOL AddArgument(UNUSED APTR obj, event *evt, int type[])
{
    BOOL result = TRUE;
    object *obj_aux;

    switch (type[evt->dest_type])
    {
        case TY_NOTHING:
            evt->arguments = NULL;
            evt->argstring[0] = '\0';
            break;
        case TY_WINOBJ:
            evt->arguments = ParentWindow(evt->destination);
            obj_aux = evt->arguments;
            strcpy(evt->argstring, obj_aux->label);
            break;
        case TY_FUNCTION:
            result =
                GetMUIarg(NULL, &evt->arguments, evt->argstring,
                          application.Functions, FALSE,
                          GetMUIBuilderString(MSG_ChoseFunction));
            break;
        case TY_ID:
            result =
                GetMUIarg(NULL, &evt->arguments, evt->argstring,
                          application.Idents, FALSE,
                          GetMUIBuilderString(MSG_ChoseIdent));
            break;
        case TY_VARIABLE:
            result =
                GetMUIarg(NULL, &evt->arguments, evt->argstring,
                          application.Variables, FALSE,
                          GetMUIBuilderString(MSG_ChoseVariable));
            break;
        case TY_CONS_INT:
        case TY_CONS_STRING:
        case TY_CONS_CHAR:
        case TY_CONS_BOOL:
            result = GetConstant(evt, type[evt->dest_type], FALSE);
            break;
        default:
            ErrorMessage(GetMUIBuilderString(MSG_InternalError));
    }
    return (result);
}

BOOL EditEvent(event *evt)
{
    BOOL result = TRUE;

    switch (TYActions[evt->destination->id][evt->dest_type])
    {
        case TY_ID:
            result =
                GetMUIarg(NULL, &evt->arguments, evt->argstring,
                          application.Idents, TRUE,
                          GetMUIBuilderString(MSG_ChoseIdent));
            break;
        case TY_FUNCTION:
            result =
                GetMUIarg(NULL, &evt->arguments, evt->argstring,
                          application.Functions, TRUE,
                          GetMUIBuilderString(MSG_ChoseFunction));
            break;
        case TY_VARIABLE:
            result =
                GetMUIarg(NULL, &evt->arguments, evt->argstring,
                          application.Variables, TRUE,
                          GetMUIBuilderString(MSG_ChoseVariable));
            break;
        case TY_CONS_INT:
        case TY_CONS_STRING:
        case TY_CONS_BOOL:
        case TY_CONS_CHAR:
            GetConstant(evt,
                        TYActions[evt->destination->id][evt->dest_type],
                        TRUE);
            break;
    }
    return (result);
}

void DeleteNotification(APTR LV_notify, object *obj, int active)
{
    event *evenement;
    object *obj_aux;
    int i;

    evenement = nth(obj->notify, active);
    delete_nth(obj->notify, active);
    /* delete source notify element of destination object */
    obj_aux = evenement->destination;
    for (i = 0; i < obj_aux->notifysource->nb_elements; i++)
    {
        if (nth(obj_aux->notifysource, i) == obj)
        {
            delete_nth(obj_aux->notifysource, i);
            break;
        }
    }
    FreeVec(evenement);
    DoMethod(LV_notify, MUIM_List_Remove, MUIV_List_Remove_Active);
}

BOOL CreateNotify(object *obj, window *win)
{
    APTR WI_notify, LV_evt, LV_cible, LV_action, LV_notify;
    APTR BT_quit, BT_add, BT_remove;
    BOOL result = FALSE, running = TRUE, bool_aux;
    ULONG signal;
    int i, active, active2;
    object *obj_aux;
    char *chaine_aux;
    event *evenement;
    window *win_aux;
    char title[120];
    char description[240];

    MakeStaticHook(DisplayHook, DisplayFunc);

    sprintf(title, GetMUIBuilderString(MSG_Editing));
    strncat(title, obj->label, 120);

    // *INDENT-OFF*
    WI_notify = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_NotifyTitle),
        MUIA_Window_ID, MAKE_ID('N', 'O', 'T', 'I'),
        MUIA_HelpNode, "Notification",
        WindowContents, GroupObject,
            Child, TextObject,
                MUIA_Text_Contents, title,
                MUIA_Text_SetMin, TRUE,
                GroupFrame,
            End,
            Child, GroupObject,
                MUIA_Group_Horiz, TRUE,
                Child, LV_evt = ListviewObject,
                    GroupFrameT(GetMUIBuilderString(MSG_Events)),
                    MUIA_Listview_List, ListObject,
                        InputListFrame,
                    End,
                End,
                Child, LV_cible = ListviewObject,
                    GroupFrameT(GetMUIBuilderString(MSG_Objects)),
                    MUIA_Listview_List, ListObject,
                        InputListFrame,
                        MUIA_List_Format, "COL=0 P=\033C DELTA=8, COL=1 DELTA=8",
                        MUIA_List_DisplayHook, &DisplayHook,
                    End,
                End,
                Child, LV_action = ListviewObject,
                    GroupFrameT(GetMUIBuilderString(MSG_Actions)),
                    MUIA_Listview_DoubleClick, TRUE,
                    MUIA_Listview_List, ListObject,
                        InputListFrame,
                    End,
                End,
            End,
            Child, GroupObject,
                MUIA_Weight, 30,
                MUIA_Group_Horiz, TRUE,
                Child, LV_notify = ListviewObject,
                    MUIA_Listview_List, ListObject,
                        InputListFrame,
                    End,
                End,
                Child, GroupObject,
                    GroupFrameT(NULL),
                    MUIA_Weight, 0,
                    Child, BT_add = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_AddNotify)),
                    Child, BT_remove = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_RemoveNotify)),
                    Child, HVSpace,
                End,
            End,
            Child, GroupObject,
                MUIA_Group_Horiz, TRUE,
                MUIA_Group_SameWidth, TRUE,
                Child, BT_quit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Quit)),
            End,
        End,
    End;
    // *INDENT-ON*

    DoMethod(BT_quit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_add, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NOTIFY);
    DoMethod(BT_remove, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DELETE);
    DoMethod(LV_evt, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app, 2,
             MUIM_Application_ReturnID, ID_EVT);
    DoMethod(LV_cible, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app,
             2, MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(LV_cible, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DEPLIE);
    DoMethod(LV_action, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app,
             2, MUIM_Application_ReturnID, ID_ACTION);
    DoMethod(LV_action, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_NOTIFY);
    DoMethod(LV_notify, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app,
             2, MUIM_Application_ReturnID, ID_SEENOTIFY);
    DoMethod(LV_notify, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_EDIT);
    DoMethod(WI_notify, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);

    DoMethod(WI_notify, MUIM_Window_SetCycleChain, LV_evt, LV_cible,
             LV_action, LV_notify, BT_add, BT_remove, BT_quit, NULL);

    /* la liste de tous les labels existants */
    delete_all(labels);
    if (win)
        CreateLabels(win, NULL);
    for (i = 0; i < windows->nb_elements; i++)
    {
        CreateLabels(nth(windows, i), win);
    }

    /* liste des objets possibles */
    if (win)
    {
        InitLabels(win, NULL, LV_cible);
        UpdateLabels(win, LV_cible, 0);
    }
    InitLabels(&application, win, LV_cible);

    /* Initialise la liste des evenements */
    ObjectEvents(obj, LV_evt);

    /* Initialise la liste des notifications deja realisee */
    for (i = 0; i < obj->notify->nb_elements; i++)
    {
        evenement = nth(obj->notify, i);
        obj_aux = evenement->destination;
        sprintf(description, "%s (%s) => %s (%s)",
                Evenements[obj->id][evenement->src_type], obj->label,
                Actions[obj_aux->id][evenement->dest_type],
                obj_aux->label);
        strncpy(evenement->description, description, 120);
        chaine_aux = (char *) evenement->description;
        DoMethod(LV_notify, MUIM_List_Insert, &chaine_aux, 1,
                 MUIV_List_Insert_Bottom);
    }

    WI_current = WI_notify;

    set(LV_evt, MUIA_List_Active, 0);
    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_notify);
    set(WI_notify, MUIA_Window_Open, TRUE);

    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_DEPLIE:
                DoMethod(LV_cible, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &obj_aux);
                if (obj_aux)
                {
                    if (obj_aux->id == TY_WINDOW)
                    {
                        set(LV_cible, MUIA_List_Quiet, TRUE);
                        set(LV_action, MUIA_List_Quiet, TRUE);
                        get(LV_cible, MUIA_List_Active, &i);
                        UpdateLabels(obj_aux, LV_cible, i);
                        set(LV_cible, MUIA_List_Quiet, FALSE);
                        set(LV_action, MUIA_List_Quiet, FALSE);
                    }
                }
                break;
            case ID_SEENOTIFY:
                get(LV_notify, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    evenement = nth(obj->notify, active);
                    if ((evenement->destination->id != TY_WINDOW)
                        && (evenement->destination->id != TY_APPLI))
                    {
                        win_aux = ParentWindow(evenement->destination);
                        if (win_aux)
                            if (!win_aux->deplie)
                            {
                                set(LV_cible, MUIA_List_Quiet, TRUE);
                                set(LV_action, MUIA_List_Quiet, TRUE);
                                UpdateLabels(win_aux, LV_cible,
                                             Position(win_aux, LV_cible));
                                set(LV_cible, MUIA_List_Quiet, FALSE);
                                set(LV_action, MUIA_List_Quiet, FALSE);
                            }
                        i = Position(evenement->destination, LV_cible);
                    }
                    else
                        i = Position(evenement->destination, LV_cible);
                    if (i != -1)
                    {
                        set(LV_cible, MUIA_List_Active, i);
                        set(LV_evt, MUIA_List_Active, evenement->src_type);
                        ObjectActions(evenement->destination, LV_action);
                        set(LV_action, MUIA_List_Active,
                            evenement->dest_type);
                    }
                }
                break;
            case ID_EDIT:
                get(LV_notify, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    evenement = nth(obj->notify, active);
                    if (!EditEvent(evenement))
                        DeleteNotification(LV_notify, obj, active);
                }
                WI_current = WI_notify;
                break;
            case ID_DELETE:
                bool_aux = TRUE;
                get(LV_notify, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    if (config.request)
                        bool_aux =
                            RequestMessage(GetMUIBuilderString
                                           (MSG_Confirm));
                    if (bool_aux)
                        DeleteNotification(LV_notify, obj, active);
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_SelectNotifyLink));
                break;
            case ID_NOTIFY:
                get(LV_evt, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    get(LV_action, MUIA_List_Active, &active2);
                    if (active2 != MUIV_List_Active_Off)
                    {
                        DoMethod(LV_cible, MUIM_List_GetEntry,
                                 MUIV_List_GetEntry_Active, &obj_aux);
                        sprintf(description, "%s (%s) => %s (%s)",
                                Evenements[obj->id][active], obj->label,
                                Actions[obj_aux->id][active2],
                                obj_aux->label);
                        evenement =
                            AllocVec(sizeof(event),
                                     MEMF_PUBLIC | MEMF_CLEAR);
                        evenement->destination = obj_aux;
                        strcpy(evenement->label_cible, obj_aux->label);
                        strncpy(evenement->description, description, 120);
                        evenement->src_type = active;
                        evenement->dest_type = active2;
                        evenement->id_cible = obj_aux->id;
                        if (AddArgument
                            (obj_aux, evenement, TYActions[obj_aux->id]))
                        {
                            add(obj->notify, evenement);
                            chaine_aux = (char *) evenement->description;
                            set(LV_notify, MUIA_List_Quiet, TRUE);
                            DoMethod(LV_notify, MUIM_List_Insert,
                                     &chaine_aux, 1,
                                     MUIV_List_Insert_Bottom);
                            set(LV_notify, MUIA_List_Active,
                                obj->notify->nb_elements);
                            set(LV_notify, MUIA_List_Quiet, FALSE);
                            add(obj_aux->notifysource, obj);
                        }
                        else
                            FreeVec(evenement);
                    }
                    else
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_SelectAction));
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectEvent));
                WI_current = WI_notify;
                break;
            case ID_NEWOBJ:
                DoMethod(LV_cible, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &obj_aux);
                if (obj_aux)
                {
                    ObjectActions(obj_aux, LV_action);
                }
                break;
            case ID_OKWIN:
                result = TRUE;
            case ID_END:
                running = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    set(WI_notify, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_notify);
    MUI_DisposeObject(WI_notify);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
