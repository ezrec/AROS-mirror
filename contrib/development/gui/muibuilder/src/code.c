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

#include <dos/dostags.h>
#include "builder.h"
#include "muib_file.h"
#include "codenotify.h"

char genfile[512];
char nospace[80];
queue *genlabels;
char get_string[80];
char real_getstring[100];
int nb_identificateurs;
int nb_var_aux = 0;
BOOL code = TRUE;
BOOL env = TRUE;
BOOL declarations = TRUE;
BOOL local = FALSE;
BOOL notifications = TRUE;
BOOL generate_all = FALSE;
int numfirstlabel;
APTR TextInfo;
queue *parameters;

BOOL IsParentFrom(object *objsrc, object *objdest)
{
    if ((objdest == objsrc) || (objsrc->id == TY_APPLI))
        return (TRUE);
    else if (objdest->id == TY_APPLI)
        return (FALSE);
    else
        return (IsParentFrom(objsrc, objdest->father));
}

LONG PopObjectDepth(popobject *popobj)
{
    if (popobj->obj->id == TY_POPOBJECT)
        return (PopObjectDepth((popobject *) popobj->obj) + 1);
    else
        return (0);
}

void UpdateNodeIdent(queue *list, char *content)
{
    chainon *chainon_aux;
    int i;

    chainon_aux = list->head;
    for (i = 0; i < list->nb_elements; i++)
    {
        if (generate_all)
        {
            if (!chainon_aux->ident)
                nb_identificateurs++;
            chainon_aux->ident = 1;
        }
        else if ((chainon_aux->element == content))
        {
            if (!chainon_aux->ident)
                nb_identificateurs++;
            chainon_aux->ident = 1;
            break;
        }
        chainon_aux = chainon_aux->next;
    }
}

void CalculeNbIdent(APTR obj)
{
    object *obj_aux;
    window *win_aux;
    group *group_aux;
    listview *list_aux;
    popobject *popobj_aux;
    chaine *string_aux;
    slider *slider_aux;
    check *check_aux;
    menu *menu_aux;
    int i;

    obj_aux = obj;
    nb_identificateurs++;
    switch (obj_aux->id)
    {
        case TY_APPLI:
            if (application.appmenu->childs->nb_elements > 0)
                CalculeNbIdent(application.appmenu);
            for (i = 0; i < windows->nb_elements; i++)
            {
                CalculeNbIdent(nth(windows, i));
            }
            break;
        case TY_WINDOW:
            win_aux = obj;
            if (win_aux->menu->childs->nb_elements > 0)
                CalculeNbIdent(win_aux->menu);
            CalculeNbIdent(&win_aux->root);
            break;
        case TY_GROUP:
            group_aux = obj;
            if (group_aux->registermode)
                nb_identificateurs++;
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                CalculeNbIdent(nth(group_aux->child, i));
            }
            break;
        case TY_CYCLE:
            nb_identificateurs++;
            break;
        case TY_TEXT:
            nb_identificateurs++;
            break;
        case TY_RADIO:
            nb_identificateurs++;
            break;
        case TY_POPASL:
            nb_identificateurs++;
            break;
        case TY_POPOBJECT:
            popobj_aux = obj;
            nb_identificateurs++;
            CalculeNbIdent(popobj_aux->obj);
            break;
        case TY_LISTVIEW:
            list_aux = obj;
            if ((list_aux->type == 1) && (strlen(list_aux->content) > 0))
                nb_identificateurs++;
            break;
        case TY_STRING:
            string_aux = obj;
            if (string_aux->title_exist)
                nb_identificateurs = nb_identificateurs + 2;
            break;
        case TY_SLIDER:
            slider_aux = obj;
            if (slider_aux->title_exist)
                nb_identificateurs = nb_identificateurs + 2;
            break;
        case TY_CHECK:
            check_aux = obj;
            if (check_aux->title_exist)
                nb_identificateurs = nb_identificateurs + 2;
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = obj;
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                CalculeNbIdent(nth(menu_aux->childs, i));
            }
            break;
    }
}

void GenerateListLabels(FILE *fichier, queue *list, char type)
{
    int i;
    chainon *chainon_aux;

    chainon_aux = list->head;
    for (i = 0; i < list->nb_elements; i++)
    {
        if (chainon_aux->ident)
        {
            fprintf(fichier, "%c%s%c", type,
                    (char *) chainon_aux->element, 0);
        }
        chainon_aux = chainon_aux->next;
    }
}

void GenerateListInits(FILE *fichier, queue *list)
{
    int i;
    chainon *chainon_aux;

    chainon_aux = list->head;
    for (i = 0; i < list->nb_elements; i++)
    {
        if (chainon_aux->ident)
        {
            fprintf(fichier, "%c", 0);
            chainon_aux->ident = 0;
        }
        chainon_aux = chainon_aux->next;
    }
}

void GenerateExternalLabels(FILE *fichier)
{
    chainon *chainon_aux;
    int i;
    object *obj;

    chainon_aux = parameters->head;
    for (i = 0; i < parameters->nb_elements; i++)
    {
        obj = chainon_aux->element;
        fprintf(fichier, "%c%s%c", TYPEVAR_EXTERNAL_PTR, obj->label, 0);
        chainon_aux = chainon_aux->next;
    }
}

void GenerateLabels(APTR obj, FILE *fichier)
{
    object *obj_aux;
    window *win_aux;
    group *group_aux;
    listview *list_aux;
    popobject *popobj_aux;
    chaine *string_aux;
    slider *slider_aux;
    check *check_aux;
    menu *menu_aux;
    int i;

    obj_aux = obj;
    if (obj_aux->generated)
    {
        fprintf(fichier, "%c%s%c", TYPEVAR_PTR, obj_aux->label, 0);
    }
    else
    {
        fprintf(fichier, "%c%s%c", TYPEVAR_LOCAL_PTR, obj_aux->label, 0);
    }
    switch (obj_aux->id)
    {
        case TY_APPLI:
            if (application.appmenu->childs->nb_elements > 0)
                GenerateLabels(application.appmenu, fichier);
            for (i = 0; i < windows->nb_elements; i++)
            {
                GenerateLabels(nth(windows, i), fichier);
            }
            break;
        case TY_WINDOW:
            win_aux = obj;
            if (win_aux->menu->childs->nb_elements > 0)
                GenerateLabels(win_aux->menu, fichier);
            GenerateLabels(&win_aux->root, fichier);
            break;
        case TY_GROUP:
            group_aux = obj;
            if (group_aux->registermode)
                fprintf(fichier, "%cSTR_%s%c", TYPEVAR_TABSTRING,
                        obj_aux->label, 0);
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                GenerateLabels(nth(group_aux->child, i), fichier);
            }
            break;
        case TY_CYCLE:
            fprintf(fichier, "%c%sContent%c", TYPEVAR_TABSTRING,
                    obj_aux->label, 0);
            break;
        case TY_TEXT:
            fprintf(fichier, "%cSTR_%s%c", TYPEVAR_STRING, obj_aux->label,
                    0);
            break;
        case TY_RADIO:
            fprintf(fichier, "%c%sContent%c", TYPEVAR_TABSTRING,
                    obj_aux->label, 0);
            break;
        case TY_POPASL:
            fprintf(fichier, "%cSTR_%s%c", TYPEVAR_PTR, obj_aux->label, 0);
            break;
        case TY_LISTVIEW:
            list_aux = obj;
            if ((list_aux->type == 1) && (strlen(list_aux->content) > 0))
                fprintf(fichier, "%c%sContent%c", TYPEVAR_STRING,
                        obj_aux->label, 0);
            break;
        case TY_POPOBJECT:
            popobj_aux = obj;
            fprintf(fichier, "%cSTR_%s%c", TYPEVAR_PTR, obj_aux->label, 0);
            GenerateLabels(popobj_aux->obj, fichier);
            break;
        case TY_STRING:
            string_aux = obj;
            if (string_aux->title_exist)
            {
                fprintf(fichier, "%cobj_aux%d%c", TYPEVAR_LOCAL_PTR,
                        nb_var_aux++, 0);
                fprintf(fichier, "%cobj_aux%d%c", TYPEVAR_LOCAL_PTR,
                        nb_var_aux++, 0);
            }
            break;
        case TY_CHECK:
            check_aux = obj;
            if (check_aux->title_exist)
            {
                fprintf(fichier, "%cobj_aux%d%c", TYPEVAR_LOCAL_PTR,
                        nb_var_aux++, 0);
                fprintf(fichier, "%cobj_aux%d%c", TYPEVAR_LOCAL_PTR,
                        nb_var_aux++, 0);
            }
            break;
        case TY_SLIDER:
            slider_aux = obj;
            if (slider_aux->title_exist)
            {
                fprintf(fichier, "%cobj_aux%d%c", TYPEVAR_LOCAL_PTR,
                        nb_var_aux++, 0);
                fprintf(fichier, "%cobj_aux%d%c", TYPEVAR_LOCAL_PTR,
                        nb_var_aux++, 0);
            }
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = obj;
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                GenerateLabels(nth(menu_aux->childs, i), fichier);
            }
            break;
    }
}

void GenerateExternalInits(FILE *fichier)
{
    int i;

    for (i = 0; i < parameters->nb_elements; i++)
    {
        fprintf(fichier, "%c", 0);
    }
}

void GenerateInitLabels(APTR obj, FILE *fichier)
{
    object *obj_aux;
    window *win_aux;
    group *group_aux;
    listview *list_aux;
    cycle *cycle_aux;
    radio *radio_aux;
    texte *text_aux;
    chaine *string_aux;
    slider *slider_aux;
    check *check_aux;
    popobject *popobj_aux;
    menu *menu_aux;
    int i;

    obj_aux = obj;
    switch (obj_aux->id)
    {
        case TY_APPLI:
            if (application.appmenu->childs->nb_elements > 0)
                GenerateInitLabels(application.appmenu, fichier);
            fprintf(fichier, "%c", 0);
            for (i = 0; i < windows->nb_elements; i++)
            {
                GenerateInitLabels(nth(windows, i), fichier);
            }
            break;
        case TY_WINDOW:
            win_aux = obj;
            if (win_aux->menu->childs->nb_elements > 0)
                GenerateInitLabels(win_aux->menu, fichier);
            if (win_aux->generated)
                fprintf(fichier, "%c", 0);
            GenerateInitLabels(&win_aux->root, fichier);
            break;
        case TY_GROUP:
            group_aux = obj;
            fprintf(fichier, "%c", 0);
            if (group_aux->registermode)
            {
                if (group_aux->entries->nb_elements != 0)
                {
                    for (i = 0; i < group_aux->entries->nb_elements; i++)
                    {
                        if (!local)
                            fprintf(fichier, "%s%c",
                                    (char *) nth(group_aux->entries, i),
                                    0);
                        else
                            fprintf(fichier, "%s%s%d%c", catprepend,
                                    group_aux->label, i, 0);
                    }
                    fprintf(fichier, "%c", 0);
                }
                else
                {
                    if (!local)
                        fprintf(fichier, "Error%c", 0);
                    else
                        fprintf(fichier, "%s%s0%c", catprepend,
                                group_aux->label, 0);
                    fprintf(fichier, "%c", 0);
                }
            }
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                GenerateInitLabels(nth(group_aux->child, i), fichier);
            }
            break;
        case TY_CYCLE:
            cycle_aux = obj;
            fprintf(fichier, "%c", 0);
            for (i = 0; i < cycle_aux->entries->nb_elements; i++)
            {
                if (!local)
                    fprintf(fichier, "%s%c",
                            (char *) nth(cycle_aux->entries, i), 0);
                else
                    fprintf(fichier, "%s%s%d%c", catprepend,
                            cycle_aux->label, i, 0);
            }
            fprintf(fichier, "%c", 0);
            break;
        case TY_TEXT:
            text_aux = obj;
            fprintf(fichier, "%c", 0);
            if ((!local) || (strlen(text_aux->content) == 0))
                fprintf(fichier, "%s%c", text_aux->content, 0);
            else
                fprintf(fichier, "%s%s%c", catprepend, text_aux->label, 0);
            break;
        case TY_RADIO:
            radio_aux = obj;
            fprintf(fichier, "%c", 0);
            for (i = 0; i < radio_aux->entries->nb_elements; i++)
            {
                if (!local)
                    fprintf(fichier, "%s%c",
                            (char *) nth(radio_aux->entries, i), 0);
                else
                    fprintf(fichier, "%s%s%d%c", catprepend,
                            radio_aux->label, i, 0);
            }
            fprintf(fichier, "%c", 0);
            break;
        case TY_LISTVIEW:
            list_aux = obj;
            fprintf(fichier, "%c", 0);
            if ((list_aux->type == 1) && (strlen(list_aux->content) > 0))
            {
                if (!local)
                    fprintf(fichier, "%s%c", list_aux->content, 0);
                else
                    fprintf(fichier, "%s%sContent%c", catprepend,
                            list_aux->label, 0);
            }
            break;
        case TY_POPASL:
            fprintf(fichier, "%c", 0);
            fprintf(fichier, "%c", 0);
            break;
        case TY_POPOBJECT:
            popobj_aux = obj;
            fprintf(fichier, "%c", 0);
            fprintf(fichier, "%c", 0);
            GenerateInitLabels(popobj_aux->obj, fichier);
            break;
        case TY_STRING:
            string_aux = obj;
            fprintf(fichier, "%c", 0);
            if (string_aux->title_exist)
            {
                fprintf(fichier, "%c", 0);
                fprintf(fichier, "%c", 0);
            }
            break;
        case TY_CHECK:
            check_aux = obj;
            fprintf(fichier, "%c", 0);
            if (check_aux->title_exist)
            {
                fprintf(fichier, "%c", 0);
                fprintf(fichier, "%c", 0);
            }
            break;
        case TY_SLIDER:
            slider_aux = obj;
            fprintf(fichier, "%c", 0);
            if (slider_aux->title_exist)
            {
                fprintf(fichier, "%c", 0);
                fprintf(fichier, "%c", 0);
            }
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = obj;
            fprintf(fichier, "%c", 0);
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                GenerateInitLabels(nth(menu_aux->childs, i), fichier);
            }
            break;
        default:
            fprintf(fichier, "%c", 0);
    }
}

void VerifyLabels(void)
{
    APTR WI_multi, lv_multi;
    APTR bt_quit;
    int i, j;
    object *obj1, *obj2;
    BOOL bool_aux;
    BOOL multiple_label = FALSE;
    BOOL running = TRUE;
    ULONG signal;
    char *chaine_aux;
    CONST_STRPTR STR_TX_label_0;
    queue *multi;
    object *obj_aux;
    chainon *chainon1, *chainon2;
    char buffer[256];
    group *group_aux;

    STR_TX_label_0 = GetMUIBuilderString(MSG_MultiplyDefinedLabel);

    multi = create();

    // *INDENT-OFF*
    WI_multi = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_MultiDefinitions),
        MUIA_Window_ID, MAKE_ID('M', 'U', 'L', 'T'),
        MUIA_HelpNode, "Code",
        WindowContents, GroupObject,
            Child, TextObject,
                MUIA_Background, 131,
                MUIA_Text_Contents, STR_TX_label_0,
                MUIA_Text_SetMax, 0,
                MUIA_Text_SetMin, 1,
                MUIA_Frame, 5,
            End,
            Child, lv_multi = ListviewObject,
                MUIA_Listview_DoubleClick, TRUE,
                MUIA_Listview_List, ListObject,
                    InputListFrame,
                End,
            End,
            Child, bt_quit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Quit)),
        End,
    End;
    // *INDENT-ON*

    DoMethod(bt_quit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_multi, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(lv_multi, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK);
    DoMethod(WI_multi, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_multi, MUIM_Window_SetCycleChain, lv_multi, bt_quit, NULL);

    chainon1 = labels->head;
    for (i = 0; i < labels->nb_elements - 1; i++)
    {
        j = i + 1;
        obj1 = chainon1->element;
        chainon2 = chainon1->next;
        obj2 = chainon2->element;
        bool_aux = (strcmp(obj1->label, obj2->label) != 0);
        if (obj1->id == TY_GROUP)
        {
            group_aux = (group *) obj1;
            if (group_aux->registermode &&
                (group_aux->entries->nb_elements !=
                 group_aux->child->nb_elements))
            {
                sprintf(buffer, GetMUIBuilderString(MSG_GroupError),
                        group_aux->label, group_aux->entries->nb_elements,
                        group_aux->child->nb_elements);
                ErrorMessage(buffer);
            }
        }
        while ((bool_aux) && (j < labels->nb_elements - 1))
        {
            j++;
            chainon2 = chainon2->next;
            obj2 = chainon2->element;
            bool_aux = (strcmp(obj1->label, obj2->label) != 0);
        }
        if (!bool_aux)
        {
            add(multi, obj1);
            chaine_aux = obj1->label;
            DoMethod(lv_multi, MUIM_List_Insert, &chaine_aux, 1,
                     MUIV_List_Insert_Bottom);
        }
        multiple_label = multiple_label || (!bool_aux);
        chainon1 = chainon1->next;
    }

    if (multiple_label)
    {
        set(app, MUIA_Application_Sleep, TRUE);
        DoMethod(app, OM_ADDMEMBER, WI_multi);
        set(lv_multi, MUIA_List_Active, 0);
        set(WI_multi, MUIA_Window_ActiveObject, lv_multi);
        set(WI_multi, MUIA_Window_Open, TRUE);
        while (running)
        {
            switch (DoMethod(app, MUIM_Application_Input, &signal))
            {
                case ID_DBCLICK:
                    get(lv_multi, MUIA_List_Active, &i);
                    if (i != MUIV_List_Active_Off)
                    {
                        obj_aux = nth(multi, i);
                        if ((obj_aux->id == TY_MENU)
                            || (obj_aux->id == TY_MENU)
                            || (obj_aux->id == TY_SUBMENU))
                        {
                            NewMenu(ParentMenu((menu *) obj_aux), FALSE);
                        }
                        else
                            NewWindow(ParentWindow(obj_aux), FALSE,
                                      obj_aux);
                    }
                    break;
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
        set(WI_multi, MUIA_Window_Open, FALSE);
        DoMethod(app, OM_REMMEMBER, WI_multi);
        set(app, MUIA_Application_Sleep, FALSE);
    }
    MUI_DisposeObject(WI_multi);
    delete_all(multi);
    FreeVec(multi);
}

void CreateLabels(APTR obj_aux, window *win)
{
    object *obj;
    window *win_aux;
    group *group_aux;
    popobject *popobj_aux;
    menu *menu_aux;
    int i;

    obj = obj_aux;
    switch (obj->id)
    {
        case TY_APPLI:
            add(labels, obj_aux);
            if (application.appmenu->childs->nb_elements > 0)
                CreateLabels(application.appmenu, NULL);
            /* all windows EXCEPT win */
            for (i = 0; i < windows->nb_elements; i++)
            {
                win_aux = nth(windows, i);
                if (win_aux != win)
                    CreateLabels(win_aux, NULL);
            }
            break;
        case TY_WINDOW:
            win_aux = obj_aux;
            add(labels, win_aux);
            if (win_aux->menu->childs->nb_elements > 0)
                CreateLabels(win_aux->menu, NULL);
            CreateLabels(&win_aux->root, NULL);
            break;
        case TY_GROUP:
            group_aux = obj_aux;
            add(labels, group_aux);
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                CreateLabels(nth(group_aux->child, i), NULL);
            }
            break;
        case TY_POPOBJECT:
            popobj_aux = obj_aux;
            add(labels, obj);
            CreateLabels(popobj_aux->obj, NULL);
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = obj_aux;
            add(labels, obj);
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                CreateLabels(nth(menu_aux->childs, i), NULL);
            }
            break;
        default:
            add(labels, obj);
            break;
    }
}

int NumLabel(APTR obj)
{
    object *obj1;
    int nb;
    queue *file_aux;
    chainon *chainon_aux;
    listview *listview_aux;
    group *group_aux;
    chaine *string_aux;
    slider *slider_aux;
    check *check_aux;

    file_aux = labels;
    chainon_aux = file_aux->head;
    obj1 = chainon_aux->element;
    for (nb = 0; (obj != obj1) && chainon_aux->next;)
    {
        switch (obj1->id)
        {
            case TY_TEXT:
            case TY_POPASL:
            case TY_RADIO:
            case TY_CYCLE:
            case TY_POPOBJECT:
                nb++;
                break;
            case TY_LISTVIEW:
                listview_aux = (listview *) obj1;
                if ((listview_aux->type == 1)
                    && (strlen(listview_aux->content) > 0))
                    nb++;
                break;
            case TY_CHECK:
                check_aux = (check *) obj1;
                if (check_aux->title_exist)
                    nb = nb + 2;
                break;
            case TY_SLIDER:
                slider_aux = (slider *) obj1;
                if (slider_aux->title_exist)
                    nb = nb + 2;
                break;
            case TY_STRING:
                string_aux = (chaine *) obj1;
                if (string_aux->title_exist)
                    nb = nb + 2;
                break;
            case TY_GROUP:
                group_aux = (group *) obj1;
                if (group_aux->registermode)
                    nb++;
                break;
        }
        nb++;
        chainon_aux = chainon_aux->next;
        obj1 = chainon_aux->element;
    }
    nb = nb - numfirstlabel;
    return (nb);
}

void NoSpace(char *chaine)
{
    char *aux1;

    strncpy(nospace, chaine, 80);
    while (aux1 = strrchr(nospace, ' '))
    {
        *aux1 = '_';
    }
}

void WriteCreateObj(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_CREATEOBJ, chaine, 0);
}

void WriteAttribut(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_ATTRIBUT, chaine, 0);
}

void WriteEnd(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_END, chaine, 0);
}

void WriteFunction(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_FUNCTION, chaine, 0);
}

void WriteObjFunction(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_OBJFUNCTION, chaine, 0);
}

void WriteString(FILE *fichier, char *chaine)
{
    fprintf(fichier, "%c%s%c", TC_STRING, chaine, 0);
}

void WriteLocaleString(FILE *fichier, char *chaine)
{
    char aux[84];

    NoSpace(chaine);
    sprintf(aux, "%s%s", catprepend, nospace);
    fprintf(fichier, "%c%s%c", TC_LOCALESTRING, aux, 0);
}

/* For Title Frame strings */
void WriteLocaleString2(FILE *fichier, char *chaine)
{
    char aux[89];

    NoSpace(chaine);
    sprintf(aux, "%s%sTitle", catprepend, nospace);
    fprintf(fichier, "%c%s%c", TC_LOCALESTRING, aux, 0);
}

void WriteLocaleChar(FILE *fichier, char *chaine)
{
    char aux[84];

    NoSpace(chaine);
    sprintf(aux, "%s%s", catprepend, nospace);
    fprintf(fichier, "%c%s%c", TC_LOCALECHAR, aux, 0);
}

void WriteLocaleCharAsString(FILE *fichier, char *chaine)
{
    char aux[84];

    NoSpace(chaine);
    sprintf(aux, "%s%sChar", catprepend, nospace);
    fprintf(fichier, "%c%s%c", TC_LOCALESTRING, aux, 0);
}

void WriteExternalFunction(FILE *fichier, char *chaine)
{
    fprintf(fichier, "%c%s%c", TC_EXTERNAL_FUNCTION, chaine, 0);
}

void WriteExternalConstant(FILE *fichier, char *chaine)
{
    fprintf(fichier, "%c%s%c", TC_EXTERNAL_CONSTANT, chaine, 0);
}

void WriteVariable(FILE *fichier, char *chaine)
{
    fprintf(fichier, "%c%s%c", TC_EXTERNAL_VARIABLE, chaine, 0);
}

void WriteInteger(FILE *fichier, int entier)
{
    fprintf(fichier, "%c%d%c", TC_INTEGER, entier, 0);
}

void WriteChar(FILE *fichier, char caractere)
{
    fprintf(fichier, "%c%c%c", TC_CHAR, caractere, 0);
}

void WriteVarAffect(FILE *fichier, int entier)
{
    fprintf(fichier, "%c%d%c", TC_VAR_AFFECT, entier, 0);
}

void WriteVarArg(FILE *fichier, int entier)
{
    fprintf(fichier, "%c%d%c", TC_VAR_ARG, entier, 0);
}

void WriteObjArg(FILE *fichier, object *obj)
{
    int i = 0;
    chaine *string_aux;
    slider *slider_aux;
    check *check_aux;


    switch (obj->id)
    {
        case TY_STRING:
            string_aux = (chaine *) obj;
            if (string_aux->title_exist)
                i++;
            break;
        case TY_SLIDER:
            slider_aux = (slider *) obj;
            if (slider_aux->title_exist)
                i++;
            break;
        case TY_CHECK:
            check_aux = (check *) obj;
            if (check_aux->title_exist)
                i++;
            break;
    }
    fprintf(fichier, "%c%d%c", TC_OBJ_ARG, NumLabel(obj) + i, 0);
}

void WriteObjArg2(FILE *fichier, int entier)
{
    fprintf(fichier, "%c%d%c", TC_OBJ_ARG, entier, 0);
}

void WriteEndFunction(FILE *fichier)
{
    fprintf(fichier, "%c%c", TC_END_FUNCTION, 0);
}

void WriteBool(FILE *fichier, BOOL entier)
{
    fprintf(fichier, "%c%d%c", TC_BOOL, entier, 0);
}


void WriteMUIArg(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_MUIARG, chaine, 0);
}

void WriteMUIArgAttribute(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_MUIARG_ATTRIBUT, chaine, 0);
}

void WriteMUIArgFunction(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_MUIARG_FUNCTION, chaine, 0);
}

void WriteMUIArgObjFunction(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_MUIARG_OBJFUNCTION, chaine, 0);
}

void WriteMUIArgObj(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_MUIARG_OBJ, chaine, 0);
}

void WriteBeginNotification(FILE *fichier, int chaine)
{
    fprintf(fichier, "%c%d%c", TC_BEGIN_NOTIFICATION, chaine, 0);
}

void WriteEndNotification(FILE *fichier)
{
    fprintf(fichier, "%c%c", TC_END_NOTIFICATION, 0);
}

void WriteWindowID(FILE *fichier, window *win_aux)
{
    char ID[10];
    int i;

    WriteFunction(fichier, MB_MAKE_ID);
    i = 0;
    while ((i < windows->nb_elements) && (nth(windows, i) != win_aux))
    {
        i++;
    }
    sprintf(ID, "%d", i);
    strcat(ID, "WIN");
    WriteChar(fichier, ID[0]);
    WriteChar(fichier, ID[1]);
    WriteChar(fichier, ID[2]);
    WriteChar(fichier, ID[3]);
    WriteEndFunction(fichier);
}

void CodeArea(FILE *fichier, object *obj, area *Area,
              BOOL Hide, BOOL Disable, BOOL InputMode,
              BOOL Phantom, BOOL Weight, BOOL Background,
              BOOL Frame, BOOL key, BOOL TitleFrame)
{
    unsigned i;

    if ((Hide) && (Area->Hide))
    {
        WriteAttribut(fichier, MB_MUIA_ShowMe);
        WriteBool(fichier, !Area->Hide);
    }
    if ((Disable) && (Area->Disable))
    {
        WriteAttribut(fichier, MB_MUIA_Disabled);
        WriteBool(fichier, Area->Disable);
    }
    if ((InputMode) && (Area->InputMode))
    {
        WriteAttribut(fichier, MB_MUIA_InputMode);
        WriteMUIArgAttribute(fichier, MB_MUIV_InputMode_RelVerify);
    }
    if ((Phantom) && (Area->Phantom))
    {
        WriteAttribut(fichier, MB_MUIA_FramePhantomHoriz);
        WriteBool(fichier, Area->Phantom);
    }
    if ((Weight) && (Area->Weight != 100))
    {
        WriteAttribut(fichier, MB_MUIA_Weight);
        WriteInteger(fichier, Area->Weight);
    }
    if (Background)
    {
        i = Area->Background;
        if (i >= MUII_BACKGROUND)
            i = i - 120;
        if (i > MUII_SelectedBack)
            i = i + 48;
        else
            i = i + MB_MUII_WindowBack;
        WriteAttribut(fichier, MB_MUIA_Background);
        WriteMUIArgAttribute(fichier, i);
    }
    if ((Frame) && (Area->Frame != MUIV_Frame_None))
    {
        WriteAttribut(fichier, MB_MUIA_Frame);
        WriteMUIArgAttribute(fichier, MB_MUIV_Frame_None + Area->Frame);
    }
    if ((key) && (Area->key != '\0'))
    {
        WriteAttribut(fichier, MB_MUIA_ControlChar);
        if (local)
            WriteLocaleChar(fichier, obj->label);
        else
            WriteChar(fichier, Area->key);
    }
    if ((TitleFrame) && (strlen(Area->TitleFrame) > 0))
    {
        WriteAttribut(fichier, MB_MUIA_FrameTitle);
        if (local)
            WriteLocaleString2(fichier, obj->label);
        else
            WriteString(fichier, Area->TitleFrame);
    }
}

void CodeCreate(object *obj, FILE *fichier)
{
    window *win_aux;
    group *group_aux;
    chaine *string_aux;
    bouton *button_aux;
    listview *listview_aux;
    gauge *gauge_aux;
    cycle *cycle_aux;
    radio *radio_aux;
    label *label_aux;
    check *check_aux;
    scale *scale_aux;
    slider *slider_aux;
    image *image_aux;
    texte *text_aux;
    prop *prop_aux;
    dirlist *dirlist_aux;
    rectangle *rect_aux;
    colorfield *field_aux;
    popaslobj *popasl;
    popobject *popobj_aux;
    menu *menu_aux;
    space *space_aux;
    char buffer[256];
    int i;

    switch (obj->id)
    {
        case TY_APPLI:
            for (i = 0; i < windows->nb_elements; i++)
            {
                CodeCreate(nth(windows, i), fichier);
            }
            if (application.appmenu->childs->nb_elements > 0)
                CodeCreate((object *) application.appmenu, fichier);
            WriteVarAffect(fichier, 0);
            WriteCreateObj(fichier, MB_ApplicationObject);
            if (strlen(application.author) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_Author);
                WriteString(fichier, application.author);
            }
            if (application.appmenu->childs->nb_elements > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_Menustrip);
                WriteObjArg(fichier, (object *) application.appmenu);
            }
            if (strlen(application.base) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_Base);
                WriteString(fichier, application.base);
            }
            if (strlen(application.title) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_Title);
                WriteString(fichier, application.title);
            }
            if (strlen(application.version) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_Version);
                WriteString(fichier, application.version);
            }
            if (strlen(application.copyright) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_Copyright);
                if (local)
                    WriteLocaleString(fichier, "AppCopyright");
                else
                    WriteString(fichier, application.copyright);
            }
            if (strlen(application.description) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_Description);
                if (local)
                    WriteLocaleString(fichier, "AppDescription");
                else
                    WriteString(fichier, application.description);
            }
            if (strlen(application.helpfile) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Application_HelpFile);
                WriteString(fichier, FilePart(application.helpfile));
            }
            for (i = 0; i < windows->nb_elements; i++)
            {
                WriteAttribut(fichier, MB_SubWindow);
                WriteObjArg(fichier, nth(windows, i));
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_WINDOW:
            win_aux = (window *) obj;

            CodeCreate((object *) &win_aux->root, fichier);
            if (win_aux->menu->childs->nb_elements > 0)
                CodeCreate((object *) win_aux->menu, fichier);

            WriteVarAffect(fichier, NumLabel(win_aux));
            WriteCreateObj(fichier, MB_WindowObject);
            WriteAttribut(fichier, MB_MUIA_Window_Title);
            if (local)
                WriteLocaleString(fichier, win_aux->label);
            else
                WriteString(fichier, win_aux->title);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            if (win_aux->menu->childs->nb_elements > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Window_Menustrip);
                WriteObjArg(fichier, (object *) win_aux->menu);
            }
            WriteAttribut(fichier, MB_MUIA_Window_ID);
            WriteWindowID(fichier, win_aux);
            if (win_aux->appwindow)
            {
                WriteAttribut(fichier, MB_MUIA_Window_AppWindow);
                WriteBool(fichier, win_aux->appwindow);
            }
            if (win_aux->backdrop)
            {
                WriteAttribut(fichier, MB_MUIA_Window_Backdrop);
                WriteBool(fichier, win_aux->backdrop);
            }
            if (win_aux->borderless)
            {
                WriteAttribut(fichier, MB_MUIA_Window_Borderless);
                WriteBool(fichier, win_aux->borderless);
            }
            if (!win_aux->closegadget)
            {
                WriteAttribut(fichier, MB_MUIA_Window_CloseGadget);
                WriteBool(fichier, win_aux->closegadget);
            }
            if (!win_aux->depthgadget)
            {
                WriteAttribut(fichier, MB_MUIA_Window_DepthGadget);
                WriteBool(fichier, win_aux->depthgadget);
            }
            if (!win_aux->dragbar)
            {
                WriteAttribut(fichier, MB_MUIA_Window_DragBar);
                WriteBool(fichier, win_aux->dragbar);
            }
            if (!win_aux->sizegadget)
            {
                WriteAttribut(fichier, MB_MUIA_Window_SizeGadget);
                WriteBool(fichier, win_aux->sizegadget);
            }
            if (win_aux->nomenu)
            {
                WriteAttribut(fichier, MB_MUIA_Window_NoMenus);
                WriteBool(fichier, win_aux->nomenu);
            }
            if (win_aux->needmouse)
            {
                WriteAttribut(fichier, MB_MUIA_Window_NeedsMouseObject);
                WriteBool(fichier, win_aux->needmouse);
            }
            WriteAttribut(fichier, MB_WindowContents);
            WriteObjArg(fichier, (object *) &win_aux->root);
            WriteEnd(fichier, MB_End);
            break;
        case TY_GROUP:
            group_aux = (group *) obj;
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                CodeCreate(nth(group_aux->child, i), fichier);
            }
            if (group_aux->virtual)
            {
                WriteVarAffect(fichier, NumLabel(group_aux));
                WriteCreateObj(fichier, MB_VirtgroupObject);
                WriteMUIArg(fichier, MB_VirtualFrame);
                if (group_aux->registermode)
                {
                    WriteAttribut(fichier, MB_Child);
                    WriteCreateObj(fichier, MB_RegisterObject);
                    WriteAttribut(fichier, MB_MUIA_Register_Titles);
                    WriteVarArg(fichier, NumLabel(group_aux) + 1);
                }
            }
            else
            {
                WriteVarAffect(fichier, NumLabel(group_aux));
                if (group_aux->registermode)
                {
                    WriteCreateObj(fichier, MB_RegisterObject);
                    WriteAttribut(fichier, MB_MUIA_Register_Titles);
                    WriteVarArg(fichier, NumLabel(group_aux) + 1);
                }
                else
                {
                    WriteCreateObj(fichier, MB_GroupObject);
                }
            }
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &group_aux->Area,
                     TRUE, TRUE, FALSE, TRUE, TRUE,
                     (group_aux->Area.Background != MUII_WindowBack),
                     TRUE, FALSE, TRUE);
            if ((group_aux->rows) && (!group_aux->registermode))
            {
                WriteAttribut(fichier, MB_MUIA_Group_Rows);
                WriteInteger(fichier, group_aux->number);
            }
            if ((group_aux->columns) && (!group_aux->registermode))
            {
                WriteAttribut(fichier, MB_MUIA_Group_Columns);
                WriteInteger(fichier, group_aux->number);
            }
            if ((group_aux->horizontal) && (!group_aux->registermode))
            {
                WriteAttribut(fichier, MB_MUIA_Group_Horiz);
                WriteBool(fichier, group_aux->horizontal);
            }
            if (!group_aux->registermode)
            {
                if (!group_aux->samesize)
                {
                    if (group_aux->sameheight)
                    {
                        WriteAttribut(fichier, MB_MUIA_Group_SameHeight);
                        WriteBool(fichier, group_aux->sameheight);
                    }
                    if (group_aux->samewidth)
                    {
                        WriteAttribut(fichier, MB_MUIA_Group_SameWidth);
                        WriteBool(fichier, group_aux->samewidth);
                    }
                }
                else
                {
                    WriteAttribut(fichier, MB_MUIA_Group_SameSize);
                    WriteBool(fichier, group_aux->samesize);
                }
            }
            if ((group_aux->horizspacing) && (!group_aux->registermode))
            {
                WriteAttribut(fichier, MB_MUIA_Group_HorizSpacing);
                WriteInteger(fichier, group_aux->horizspace);
            }
            if ((group_aux->vertspacing) && (!group_aux->registermode))
            {
                WriteAttribut(fichier, MB_MUIA_Group_VertSpacing);
                WriteInteger(fichier, group_aux->vertspace);
            }
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                WriteAttribut(fichier, MB_Child);
                WriteObjArg(fichier, nth(group_aux->child, i));
            }
            WriteEnd(fichier, MB_End);
            if (group_aux->virtual)
            {
                WriteVarAffect(fichier, NumLabel(group_aux));
                WriteCreateObj(fichier, MB_ScrollgroupObject);
                if (group_aux->Area.Weight != 100)
                {
                    WriteAttribut(fichier, MB_MUIA_Weight);
                    WriteInteger(fichier, group_aux->Area.Weight);
                }
                WriteAttribut(fichier, MB_MUIA_Scrollgroup_Contents);
                WriteObjArg(fichier, (object *) group_aux);
                WriteEnd(fichier, MB_End);
                if (group_aux->registermode)
                    WriteEnd(fichier, MB_End);
            }
            break;
        case TY_STRING:
            string_aux = (chaine *) obj;
            WriteVarAffect(fichier, NumLabel(string_aux));
            WriteCreateObj(fichier, MB_StringObject);
            CodeArea(fichier, obj, &string_aux->Area,
                     TRUE,
                     TRUE, FALSE, TRUE,
                     (!string_aux->title_exist), FALSE, TRUE, TRUE, TRUE);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            if (strlen(string_aux->content) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_String_Contents);
                WriteString(fichier, string_aux->content);
            }
            if (strlen(string_aux->accept) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_String_Accept);
                WriteString(fichier, string_aux->accept);
            }
            if (strlen(string_aux->reject) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_String_Reject);
                WriteString(fichier, string_aux->reject);
            }
            if (string_aux->maxlen != 80)
            {
                WriteAttribut(fichier, MB_MUIA_String_MaxLen);
                WriteInteger(fichier, string_aux->maxlen);
            }
            if (string_aux->format)
            {
                WriteAttribut(fichier, MB_MUIA_String_Format);
                WriteMUIArgAttribute(fichier,
                                     string_aux->format +
                                     MB_MUIV_String_Format_Left);
            }
            if (string_aux->secret)
            {
                WriteAttribut(fichier, MB_MUIA_String_Secret);
                WriteBool(fichier, TRUE);
            }
            WriteEnd(fichier, MB_End);
            if (string_aux->title_exist)
            {
                WriteVarAffect(fichier, NumLabel(string_aux) + 2);
                if (string_aux->Area.key != '\0')
                    WriteObjFunction(fichier, MB_KeyLabel2);
                else
                    WriteObjFunction(fichier, MB_Label2);
                if (local)
                    WriteLocaleString(fichier, string_aux->label);
                else
                    WriteString(fichier, string_aux->title);
                if (string_aux->Area.key != '\0')
                {
                    if (local)
                    {
                        WriteLocaleChar(fichier, string_aux->label);
                    }
                    else
                    {
                        WriteChar(fichier, string_aux->Area.key);
                    }
                }
                WriteEndFunction(fichier);
                WriteVarAffect(fichier, NumLabel(string_aux) + 1);
                WriteCreateObj(fichier, MB_GroupObject);
                WriteAttribut(fichier, MB_MUIA_Group_Columns);
                WriteInteger(fichier, 2);
                CodeArea(fichier, obj, &string_aux->Area,
                         FALSE, FALSE, FALSE, FALSE,
                         TRUE, FALSE, FALSE, FALSE, FALSE);
                WriteAttribut(fichier, MB_Child);
                WriteObjArg2(fichier, NumLabel(string_aux) + 2);
                WriteAttribut(fichier, MB_Child);
                WriteObjArg2(fichier, NumLabel(string_aux));
                WriteEnd(fichier, MB_End);
            }
            break;
        case TY_LABEL:
            label_aux = (label *) obj;
            WriteVarAffect(fichier, NumLabel(obj));
            if ((label_aux->Area.Weight != 0) ||
                (label_aux->Area.Hide) ||
                (label_aux->Area.Disable) || (label_aux->Area.Phantom))
            {
                WriteCreateObj(fichier, MB_TextObject);
                WriteAttribut(fichier, MB_MUIA_Text_PreParse);
                WriteString(fichier, "\\033r");
                WriteAttribut(fichier, MB_MUIA_Text_Contents);
                if (local)
                    WriteLocaleString(fichier, label_aux->label);
                else
                    WriteString(fichier, label_aux->title);
                CodeArea(fichier, obj, &label_aux->Area,
                         TRUE, TRUE, FALSE, TRUE,
                         (label_aux->Area.Weight != 0),
                         FALSE, FALSE, TRUE, FALSE);
                WriteAttribut(fichier, MB_MUIA_InnerLeft);
                WriteInteger(fichier, 0);
                WriteAttribut(fichier, MB_MUIA_InnerRight);
                WriteInteger(fichier, 0);
                WriteEnd(fichier, MB_End);
            }
            else
            {
                WriteObjFunction(fichier, MB_Label);
                CreateLocaleString(label_aux->title, label_aux->Area.key);
                if (local)
                    WriteLocaleString(fichier, label_aux->label);
                else
                    WriteString(fichier, locale_string);
                WriteEndFunction(fichier);
            }
            break;
        case TY_SPACE:
            space_aux = (space *) obj;
            WriteVarAffect(fichier, NumLabel(obj));
            switch (space_aux->type)
            {
                case 0:
                    WriteObjFunction(fichier, MB_HSpace);
                    WriteInteger(fichier, space_aux->spacing);
                    WriteEndFunction(fichier);
                    break;
                case 1:
                    WriteObjFunction(fichier, MB_VSpace);
                    WriteInteger(fichier, space_aux->spacing);
                    WriteEndFunction(fichier);
                    break;
                case 2:
                    WriteMUIArgObj(fichier, MB_HVSpace);
                    break;
            }
            break;
        case TY_SCALE:
            scale_aux = (scale *) obj;
            WriteVarAffect(fichier, NumLabel(obj));
            WriteCreateObj(fichier, MB_ScaleObject);
            CodeArea(fichier, obj, &scale_aux->Area,
                     TRUE, TRUE, TRUE, TRUE, TRUE,
                     (scale_aux->Area.Background != MUII_WindowBack),
                     (scale_aux->Area.Frame != MUIV_Frame_None),
                     FALSE, FALSE);
            if (scale_aux->horiz)
            {
                WriteAttribut(fichier, MB_MUIA_Scale_Horiz);
                WriteBool(fichier, scale_aux->horiz);
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_SLIDER:
            slider_aux = (slider *) obj;
            WriteVarAffect(fichier, NumLabel(slider_aux));
            WriteCreateObj(fichier, MB_SliderObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &slider_aux->Area,
                     TRUE, TRUE, TRUE, TRUE,
                     (!slider_aux->title_exist),
                     (slider_aux->Area.Background != MUII_WindowBack),
                     (slider_aux->Area.Frame != MUIV_Frame_None),
                     TRUE, TRUE);
            WriteAttribut(fichier, MB_MUIA_Slider_Min);
            WriteInteger(fichier, slider_aux->min);
            WriteAttribut(fichier, MB_MUIA_Slider_Max);
            WriteInteger(fichier, slider_aux->max);
            if (!slider_aux->horizontal)
            {
                WriteAttribut(fichier, MB_MUIA_Group_Horiz);
                WriteBool(fichier, slider_aux->horizontal);
            }
            if (slider_aux->quiet)
            {
                WriteAttribut(fichier, MB_MUIA_Slider_Quiet);
                WriteBool(fichier, slider_aux->quiet);
            }
            WriteAttribut(fichier, MB_MUIA_Slider_Level);
            WriteInteger(fichier, slider_aux->init);
            if (slider_aux->reverse)
            {
                WriteAttribut(fichier, MB_MUIA_Slider_Reverse);
                WriteBool(fichier, slider_aux->reverse);
            }
            WriteEnd(fichier, MB_End);
            if (slider_aux->title_exist)
            {
                WriteVarAffect(fichier, NumLabel(slider_aux) + 2);
                if (slider_aux->Area.key != '\0')
                    WriteObjFunction(fichier, MB_KeyLabel2);
                else
                    WriteObjFunction(fichier, MB_Label2);
                if (local)
                    WriteLocaleString(fichier, slider_aux->label);
                else
                    WriteString(fichier, slider_aux->title);
                if (slider_aux->Area.key != '\0')
                {
                    if (local)
                    {
                        WriteLocaleChar(fichier, slider_aux->label);
                    }
                    else
                    {
                        WriteChar(fichier, slider_aux->Area.key);
                    }
                }
                WriteEndFunction(fichier);
                WriteVarAffect(fichier, NumLabel(slider_aux) + 1);
                WriteCreateObj(fichier, MB_GroupObject);
                WriteAttribut(fichier, MB_MUIA_Group_Columns);
                WriteInteger(fichier, 2);
                if (!slider_aux->horizontal)
                {
                    WriteAttribut(fichier, MB_MUIA_Group_Horiz);
                    WriteBool(fichier, slider_aux->horizontal);
                }
                CodeArea(fichier, obj, &slider_aux->Area,
                         FALSE, FALSE, FALSE, FALSE,
                         TRUE, FALSE, FALSE, FALSE, FALSE);
                WriteAttribut(fichier, MB_Child);
                WriteObjArg2(fichier, NumLabel(slider_aux) + 2);
                WriteAttribut(fichier, MB_Child);
                WriteObjArg2(fichier, NumLabel(slider_aux));
                WriteEnd(fichier, MB_End);
            }
            break;
        case TY_PROP:
            prop_aux = (prop *) obj;
            WriteVarAffect(fichier, NumLabel(prop_aux));
            WriteCreateObj(fichier, MB_PropObject);
            WriteMUIArg(fichier, MB_PropFrame);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &prop_aux->Area,
                     TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
                     (prop_aux->Area.Frame != MUIV_Frame_Prop),
                     TRUE, TRUE);
            WriteAttribut(fichier, MB_MUIA_Prop_Entries);
            WriteInteger(fichier, prop_aux->entries);
            WriteAttribut(fichier, MB_MUIA_Prop_First);
            WriteInteger(fichier, prop_aux->first);
            if (prop_aux->horizontal)
            {
                WriteAttribut(fichier, MB_MUIA_Prop_Horiz);
                WriteBool(fichier, prop_aux->horizontal);
            }
            WriteAttribut(fichier, MB_MUIA_Prop_Visible);
            WriteInteger(fichier, prop_aux->visible);
            if (prop_aux->fixheight)
            {
                WriteAttribut(fichier, MB_MUIA_FixHeight);
                WriteInteger(fichier, prop_aux->height);
            }
            if (prop_aux->fixwidth)
            {
                WriteAttribut(fichier, MB_MUIA_FixWidth);
                WriteInteger(fichier, prop_aux->width);
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_KEYBUTTON:
            button_aux = (bouton *) obj;
            WriteVarAffect(fichier, NumLabel(button_aux));
            if ((button_aux->Area.Weight == 100) &&
                (!button_aux->Area.Hide) &&
                (!button_aux->Area.Disable) &&
                (!button_aux->Area.Phantom) &&
                (strchr(button_aux->title, tolower(button_aux->Area.key))
                 || strchr(button_aux->title,
                           toupper(button_aux->Area.key))))
            {
                WriteObjFunction(fichier, MB_SimpleButton);
                CreateLocaleString(button_aux->title,
                                   button_aux->Area.key);
                if (local)
                    WriteLocaleString(fichier, button_aux->label);
                else
                    WriteString(fichier, locale_string);
                WriteEndFunction(fichier);
            }
            else
            {
                WriteCreateObj(fichier, MB_TextObject);
                WriteMUIArg(fichier, MB_ButtonFrame);
                CodeArea(fichier, obj, &button_aux->Area,
                         TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
                         FALSE, TRUE, FALSE);
                WriteAttribut(fichier, MB_MUIA_Text_Contents);
                if (local)
                    WriteLocaleString(fichier, button_aux->label);
                else
                    WriteString(fichier, button_aux->title);
                WriteAttribut(fichier, MB_MUIA_Text_PreParse);
                WriteString(fichier, "\\033c");
                if (button_aux->Area.key != '\0')
                {
                    WriteAttribut(fichier, MB_MUIA_Text_HiChar);
                    if (!local)
                        WriteChar(fichier, button_aux->Area.key);
                    else
                        WriteLocaleChar(fichier, button_aux->label);
                }
                if (obj->Help.generated)
                {
                    WriteAttribut(fichier, MB_MUIA_HelpNode);
                    WriteString(fichier, obj->label);
                }
                WriteAttribut(fichier, MB_MUIA_InputMode);
                WriteMUIArgAttribute(fichier, MB_MUIV_InputMode_RelVerify);
                WriteEnd(fichier, MB_End);
            }
            break;
        case TY_TEXT:
            text_aux = (texte *) obj;
            WriteVarAffect(fichier, NumLabel(text_aux));
            WriteCreateObj(fichier, MB_TextObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &text_aux->Area,
                     TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
            WriteAttribut(fichier, MB_MUIA_Text_Contents);
            WriteVarArg(fichier, NumLabel(text_aux) + 1);
            if (strlen(text_aux->preparse) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Text_PreParse);
                WriteString(fichier, text_aux->preparse);
            }
            if (text_aux->max)
            {
                WriteAttribut(fichier, MB_MUIA_Text_SetMax);
                WriteBool(fichier, text_aux->max);
            }
            if (text_aux->min)
            {
                WriteAttribut(fichier, MB_MUIA_Text_SetMin);
                WriteBool(fichier, text_aux->min);
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_IMAGE:
            image_aux = (image *) obj;
            WriteVarAffect(fichier, NumLabel(image_aux));
            WriteCreateObj(fichier, MB_ImageObject);
            WriteAttribut(fichier, MB_MUIA_Image_Spec);
            if (strlen(image_aux->spec) > 0)
            {
                sprintf(buffer, "5:%s", image_aux->spec);
                WriteString(fichier, buffer);
            }
            else
                WriteInteger(fichier, image_aux->type);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &image_aux->Area,
                     TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE,
                     TRUE, TRUE);
            if (image_aux->freevert)
            {
                WriteAttribut(fichier, MB_MUIA_Image_FreeVert);
                WriteBool(fichier, image_aux->freevert);
            }
            if (image_aux->freehoriz)
            {
                WriteAttribut(fichier, MB_MUIA_Image_FreeHoriz);
                WriteBool(fichier, image_aux->freehoriz);
            }
            if (image_aux->fixheight)
            {
                WriteAttribut(fichier, MB_MUIA_FixHeight);
                WriteInteger(fichier, image_aux->height);
            }
            if (image_aux->fixwidth)
            {
                WriteAttribut(fichier, MB_MUIA_FixWidth);
                WriteInteger(fichier, image_aux->width);
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_LISTVIEW:
            listview_aux = (listview *) obj;
            WriteVarAffect(fichier, NumLabel(listview_aux));
            switch (listview_aux->type)
            {
                case 0:
                    WriteCreateObj(fichier, MB_ListObject);
                    break;
                case 1:
                    WriteCreateObj(fichier, MB_FloattextObject);
                    if (strlen(listview_aux->content) > 0)
                    {
                        WriteAttribut(fichier, MB_MUIA_Floattext_Text);
                        WriteVarArg(fichier, NumLabel(listview_aux) + 1);
                    }
                    break;
                case 2:
                    WriteCreateObj(fichier, MB_VolumelistObject);
                    break;
            }
            CodeArea(fichier, obj, &listview_aux->Area,
                     FALSE, TRUE, FALSE, TRUE, FALSE,
                     (listview_aux->Area.Background != MUII_ListBack),
                     TRUE, TRUE, FALSE);
            if (strlen(listview_aux->title) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_List_Title);
                if (local)
                    WriteLocaleString(fichier, listview_aux->label);
                else
                    WriteString(fichier, listview_aux->title);
            }
            if (strlen(listview_aux->format) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_List_Format);
                WriteString(fichier, listview_aux->format);
            }
            if (listview_aux->select)
            {
                WriteAttribut(fichier, MB_MUIA_List_Active);
                WriteMUIArgAttribute(fichier,
                                     MB_MUIV_List_Active_Off +
                                     listview_aux->select);
            }
            if (listview_aux->adjustheight)
            {
                WriteAttribut(fichier, MB_MUIA_List_AdjustHeight);
                WriteBool(fichier, listview_aux->adjustheight);
            }
            if (listview_aux->adjustwidth)
            {
                WriteAttribut(fichier, MB_MUIA_List_AdjustWidth);
                WriteBool(fichier, listview_aux->adjustwidth);
            }
            if (listview_aux->comparehook)
            {
                WriteAttribut(fichier, MB_MUIA_List_CompareHook);
                WriteExternalFunction(fichier, listview_aux->comparehook);
                UpdateNodeIdent(application.Functions,
                                listview_aux->comparehook);
            }
            if (listview_aux->constructhook)
            {
                WriteAttribut(fichier, MB_MUIA_List_ConstructHook);
                WriteExternalFunction(fichier,
                                      listview_aux->constructhook);
                UpdateNodeIdent(application.Functions,
                                listview_aux->constructhook);
            }
            if (listview_aux->destructhook)
            {
                WriteAttribut(fichier, MB_MUIA_List_DestructHook);
                WriteExternalFunction(fichier, listview_aux->destructhook);
                UpdateNodeIdent(application.Functions,
                                listview_aux->destructhook);
            }
            if (listview_aux->multitesthook)
            {
                WriteAttribut(fichier, MB_MUIA_List_MultiTestHook);
                WriteExternalFunction(fichier,
                                      listview_aux->multitesthook);
                UpdateNodeIdent(application.Functions,
                                listview_aux->multitesthook);
            }
            if (listview_aux->displayhook)
            {
                WriteAttribut(fichier, MB_MUIA_List_DisplayHook);
                WriteExternalFunction(fichier, listview_aux->displayhook);
                UpdateNodeIdent(application.Functions,
                                listview_aux->displayhook);
            }
            WriteEnd(fichier, MB_End);

            WriteVarAffect(fichier, NumLabel(listview_aux));
            WriteCreateObj(fichier, MB_ListviewObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &listview_aux->Area,
                     TRUE, FALSE, FALSE, FALSE, TRUE, FALSE,
                     FALSE, FALSE, TRUE);
            if (listview_aux->multiselect)
            {
                WriteAttribut(fichier, MB_MUIA_Listview_MultiSelect);
                WriteMUIArgAttribute(fichier,
                                     MB_MUIV_Listview_MultiSelect_Default);
            }
            if (listview_aux->doubleclick)
            {
                WriteAttribut(fichier, MB_MUIA_Listview_DoubleClick);
                WriteBool(fichier, listview_aux->doubleclick);
            }
            if (!listview_aux->inputmode)
            {
                WriteAttribut(fichier, MB_MUIA_Listview_Input);
                WriteBool(fichier, listview_aux->inputmode);
            }
            WriteAttribut(fichier, MB_MUIA_Listview_List);
            WriteObjArg(fichier, (object *) listview_aux);
            WriteEnd(fichier, MB_End);
            break;
        case TY_CHECK:
            check_aux = (check *) obj;
            WriteVarAffect(fichier, NumLabel(check_aux));
            if ((check_aux->Area.Weight == 100) &&
                (!check_aux->Area.Hide) &&
                (!check_aux->Area.Disable) &&
                (!check_aux->Area.Phantom) &&
                (check_aux->Area.Background == MUII_ButtonBack))
            {
                if (check_aux->Area.key != '\0')
                    WriteObjFunction(fichier, MB_KeyCheckMark);
                else
                    WriteObjFunction(fichier, MB_CheckMark);
                WriteBool(fichier, check_aux->init_state);
                if (check_aux->Area.key != '\0')
                {
                    if (local)
                        WriteLocaleChar(fichier, check_aux->label);
                    else
                        WriteChar(fichier, check_aux->Area.key);
                }
                WriteEndFunction(fichier);
            }
            else
            {
                WriteCreateObj(fichier, MB_ImageObject);
                CodeArea(fichier, obj, &check_aux->Area,
                         TRUE, TRUE, FALSE, TRUE, TRUE,
                         (check_aux->Area.Background != MUII_ButtonBack),
                         FALSE, TRUE, FALSE);
                WriteAttribut(fichier, MB_MUIA_InputMode);
                WriteMUIArgAttribute(fichier, MB_MUIV_InputMode_Toggle);
                WriteAttribut(fichier, MB_MUIA_Image_Spec);
                WriteMUIArgAttribute(fichier, MB_MUII_CheckMark);
                WriteAttribut(fichier, MB_MUIA_Image_FreeVert);
                WriteBool(fichier, TRUE);
                WriteAttribut(fichier, MB_MUIA_Selected);
                WriteBool(fichier, check_aux->init_state);
                WriteAttribut(fichier, MB_MUIA_ShowSelState);
                WriteBool(fichier, FALSE);
                WriteEnd(fichier, MB_End);
            }
            if (check_aux->title_exist)
            {
                WriteVarAffect(fichier, NumLabel(check_aux) + 2);
                if (check_aux->Area.key != '\0')
                    WriteObjFunction(fichier, MB_KeyLabel2);
                else
                    WriteObjFunction(fichier, MB_Label2);
                if (local)
                    WriteLocaleString(fichier, check_aux->label);
                else
                    WriteString(fichier, check_aux->title);
                if (check_aux->Area.key != '\0')
                {
                    if (local)
                    {
                        WriteLocaleChar(fichier, check_aux->label);
                    }
                    else
                    {
                        WriteChar(fichier, check_aux->Area.key);
                    }
                }
                WriteEndFunction(fichier);
                WriteVarAffect(fichier, NumLabel(check_aux) + 1);
                WriteCreateObj(fichier, MB_GroupObject);
                WriteAttribut(fichier, MB_MUIA_Group_Columns);
                WriteInteger(fichier, 2);
                WriteAttribut(fichier, MB_Child);
                WriteObjArg2(fichier, NumLabel(check_aux) + 2);
                WriteAttribut(fichier, MB_Child);
                WriteObjArg2(fichier, NumLabel(check_aux));
                WriteEnd(fichier, MB_End);
            }
            break;
        case TY_DIRLIST:
            dirlist_aux = (dirlist *) obj;
            WriteVarAffect(fichier, NumLabel(dirlist_aux));
            WriteCreateObj(fichier, MB_DirlistObject);
            CodeArea(fichier, obj, &dirlist_aux->Area,
                     FALSE, TRUE, FALSE, TRUE, FALSE,
                     TRUE, TRUE, TRUE, TRUE);
            if (dirlist_aux->filterhook)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_FilterHook);
                WriteExternalFunction(fichier, dirlist_aux->filterhook);
                UpdateNodeIdent(application.Functions,
                                dirlist_aux->filterhook);
            }
            if (strlen(dirlist_aux->directory) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_Directory);
                WriteString(fichier, dirlist_aux->directory);
            }
            if (strlen(dirlist_aux->accept) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_AcceptPattern);
                WriteString(fichier, dirlist_aux->accept);
            }
            if (strlen(dirlist_aux->reject) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_RejectPattern);
                WriteString(fichier, dirlist_aux->reject);
            }
            if (dirlist_aux->drawers)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_DrawersOnly);
                WriteBool(fichier, dirlist_aux->drawers);
            }
            if (dirlist_aux->files)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_FilesOnly);
                WriteBool(fichier, dirlist_aux->files);
            }
            if (dirlist_aux->filter)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_FilterDrawers);
                WriteBool(fichier, dirlist_aux->filter);
            }
            if (dirlist_aux->multi)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_MultiSelDirs);
                WriteBool(fichier, dirlist_aux->multi);
            }
            if (dirlist_aux->icons)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_RejectIcons);
                WriteBool(fichier, dirlist_aux->icons);
            }
            if (dirlist_aux->sortdirs)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_SortDirs);
                WriteMUIArgAttribute(fichier,
                                     MB_MUIV_Dirlist_SortDirs_First +
                                     dirlist_aux->sortdirs);
            }
            if (dirlist_aux->highlow)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_SortHighLow);
                WriteBool(fichier, dirlist_aux->highlow);
            }
            if (dirlist_aux->sorttype)
            {
                WriteAttribut(fichier, MB_MUIA_Dirlist_SortType);
                WriteMUIArgAttribute(fichier,
                                     MB_MUIV_Dirlist_SortType_Name +
                                     dirlist_aux->sorttype);
            }
            WriteEnd(fichier, MB_End);
            WriteVarAffect(fichier, NumLabel(dirlist_aux));
            WriteCreateObj(fichier, MB_ListviewObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &dirlist_aux->Area,
                     TRUE, FALSE, FALSE, FALSE, TRUE,
                     FALSE, FALSE, FALSE, FALSE);
            if (dirlist_aux->multi)
            {
                WriteAttribut(fichier, MB_MUIA_Listview_MultiSelect);
                WriteMUIArgAttribute(fichier,
                                     MB_MUIV_Listview_MultiSelect_Default);
            }
            WriteAttribut(fichier, MB_MUIA_Listview_List);
            WriteObjArg(fichier, obj);
            WriteEnd(fichier, MB_End);
            break;
        case TY_GAUGE:
            gauge_aux = (gauge *) obj;
            WriteVarAffect(fichier, NumLabel(gauge_aux));
            WriteCreateObj(fichier, MB_GaugeObject);
            WriteMUIArg(fichier, MB_GaugeFrame);
            CodeArea(fichier, obj, &gauge_aux->Area,
                     TRUE, TRUE, FALSE, TRUE, TRUE,
                     FALSE, FALSE, FALSE, FALSE);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            if (gauge_aux->fixwidth)
            {
                WriteAttribut(fichier, MB_MUIA_FixWidth);
                WriteInteger(fichier, gauge_aux->width);
            }
            if (gauge_aux->fixheight)
            {
                WriteAttribut(fichier, MB_MUIA_FixHeight);
                WriteInteger(fichier, gauge_aux->height);
            }
            if (gauge_aux->divide > 1)
            {
                WriteAttribut(fichier, MB_MUIA_Gauge_Divide);
                WriteInteger(fichier, gauge_aux->divide);
            }
            if (gauge_aux->horizontal)
            {
                WriteAttribut(fichier, MB_MUIA_Gauge_Horiz);
                WriteBool(fichier, gauge_aux->horizontal);
            }
            if (strlen(gauge_aux->infotext) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Gauge_InfoText);
                if (local)
                    WriteLocaleString(fichier, gauge_aux->label);
                else
                    WriteString(fichier, gauge_aux->infotext);
            }
            WriteAttribut(fichier, MB_MUIA_Gauge_Max);
            WriteInteger(fichier, gauge_aux->max);
            WriteEnd(fichier, MB_End);
            break;
        case TY_CYCLE:
            cycle_aux = (cycle *) obj;
            WriteVarAffect(fichier, NumLabel(cycle_aux));
            WriteCreateObj(fichier, MB_CycleObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &cycle_aux->Area,
                     TRUE, TRUE, FALSE, TRUE, TRUE,
                     FALSE, TRUE, TRUE, FALSE);
            WriteAttribut(fichier, MB_MUIA_Cycle_Entries);
            WriteVarArg(fichier, NumLabel(cycle_aux) + 1);
            WriteEnd(fichier, MB_End);
            break;
        case TY_RADIO:
            radio_aux = (radio *) obj;
            WriteVarAffect(fichier, NumLabel(radio_aux));
            WriteCreateObj(fichier, MB_RadioObject);
            CodeArea(fichier, obj, &radio_aux->Area,
                     TRUE, TRUE, TRUE, TRUE, TRUE,
                     (radio_aux->Area.Background != MUII_WindowBack),
                     TRUE, TRUE, TRUE);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            WriteAttribut(fichier, MB_MUIA_Radio_Entries);
            WriteVarArg(fichier, NumLabel(radio_aux) + 1);
            WriteEnd(fichier, MB_End);
            break;
        case TY_RECTANGLE:
            rect_aux = (rectangle *) obj;
            WriteVarAffect(fichier, NumLabel(rect_aux));
            if ((rect_aux->type != 1)
                || (strlen(rect_aux->Area.TitleFrame) == 0))
            {
                WriteCreateObj(fichier, MB_RectangleObject);
                if (obj->Help.generated)
                {
                    WriteAttribut(fichier, MB_MUIA_HelpNode);
                    WriteString(fichier, obj->label);
                }
                CodeArea(fichier, obj, &rect_aux->Area,
                         TRUE, TRUE, FALSE, TRUE, TRUE,
                         (rect_aux->Area.Background != MUII_WindowBack),
                         TRUE, TRUE, TRUE);
                if (rect_aux->type == 2)
                {
                    WriteAttribut(fichier, MB_MUIA_Rectangle_VBar);
                    WriteBool(fichier, TRUE);
                }
                if (rect_aux->type == 1)
                {
                    WriteAttribut(fichier, MB_MUIA_Rectangle_HBar);
                    WriteBool(fichier, TRUE);
                }
                if (rect_aux->fixwidth)
                {
                    WriteAttribut(fichier, MB_MUIA_FixWidth);
                    WriteInteger(fichier, rect_aux->width);
                }
                if (rect_aux->fixheight)
                {
                    WriteAttribut(fichier, MB_MUIA_FixHeight);
                    WriteInteger(fichier, rect_aux->height);
                }
                WriteEnd(fichier, MB_End);
            }
            else
            {
                WriteObjFunction(fichier, MB_MUI_MakeObject);
                WriteMUIArg(fichier, MB_MUIO_BarTitle);
                if (local)
                    WriteLocaleString2(fichier, rect_aux->label);
                else
                    WriteString(fichier, rect_aux->Area.TitleFrame);
                WriteEndFunction(fichier);
            }
            break;
        case TY_COLORFIELD:
            field_aux = (colorfield *) obj;
            WriteVarAffect(fichier, NumLabel(field_aux));
            WriteCreateObj(fichier, MB_ColorfieldObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &field_aux->Area,
                     TRUE, TRUE, FALSE, TRUE, TRUE,
                     FALSE, TRUE, TRUE, TRUE);
            if (field_aux->fixheight)
            {
                WriteAttribut(fichier, MB_MUIA_FixHeight);
                WriteInteger(fichier, field_aux->height);
            }
            if (field_aux->fixwidth)
            {
                WriteAttribut(fichier, MB_MUIA_FixWidth);
                WriteInteger(fichier, field_aux->width);
            }
            if (field_aux->Area.InputMode)
            {
                WriteAttribut(fichier, MB_MUIA_InputMode);
                WriteInteger(fichier, field_aux->Area.InputMode);
            }
            WriteAttribut(fichier, MB_MUIA_Colorfield_Red);
            WriteInteger(fichier, field_aux->red);
            WriteAttribut(fichier, MB_MUIA_Colorfield_Green);
            WriteInteger(fichier, field_aux->green);
            WriteAttribut(fichier, MB_MUIA_Colorfield_Blue);
            WriteInteger(fichier, field_aux->blue);
            WriteEnd(fichier, MB_End);
            break;
        case TY_POPASL:
            popasl = (popaslobj *) obj;
            i = popasl->image +MB_MUII_PopUp;
            WriteVarAffect(fichier, NumLabel(popasl) + 1);
            if (popasl->Area.key == '\0')
            {
                WriteObjFunction(fichier, MB_String);
                WriteString(fichier, "");
                WriteInteger(fichier, 80);
                WriteEndFunction(fichier);
            }
            else
            {
                WriteObjFunction(fichier, MB_KeyString);
                WriteString(fichier, "");
                WriteInteger(fichier, 80);
                if (local)
                    WriteLocaleChar(fichier, popasl->label);
                else
                    WriteChar(fichier, popasl->Area.key);
                WriteEndFunction(fichier);
            }
            WriteVarAffect(fichier, NumLabel(popasl));
            WriteObjFunction(fichier, MB_PopButton);
            WriteMUIArgAttribute(fichier, i);
            WriteEndFunction(fichier);
            WriteVarAffect(fichier, NumLabel(popasl));
            WriteCreateObj(fichier, MB_PopaslObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &popasl->Area,
                     TRUE, TRUE, FALSE, TRUE, TRUE,
                     FALSE,
                     (popasl->Area.Background != MUII_WindowBack),
                     FALSE, FALSE);
            WriteAttribut(fichier, MB_MUIA_Popasl_Type);
            WriteInteger(fichier, popasl->type);
            WriteAttribut(fichier, MB_MUIA_Popstring_String);
            WriteObjArg2(fichier, NumLabel(popasl) + 1);
            WriteAttribut(fichier, MB_MUIA_Popstring_Button);
            WriteObjArg2(fichier, NumLabel(popasl));
            if (popasl->starthook)
            {
                WriteAttribut(fichier, MB_MUIA_Popasl_StartHook);
                WriteExternalFunction(fichier, popasl->starthook);
                UpdateNodeIdent(application.Functions, popasl->starthook);
            }
            if (popasl->stophook)
            {
                WriteAttribut(fichier, MB_MUIA_Popasl_StopHook);
                WriteExternalFunction(fichier, popasl->stophook);
                UpdateNodeIdent(application.Functions, popasl->stophook);
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_POPOBJECT:
            popobj_aux = (popobject *) obj;
            i = popobj_aux->image +MB_MUII_PopUp;
            CodeCreate(popobj_aux->obj, fichier);
            WriteVarAffect(fichier, NumLabel(popobj_aux) + 1);
            if (popobj_aux->Area.key == '\0')
            {
                WriteObjFunction(fichier, MB_String);
                WriteString(fichier, "");
                WriteInteger(fichier, 80);
                WriteEndFunction(fichier);
            }
            else
            {
                WriteObjFunction(fichier, MB_KeyString);
                WriteString(fichier, "");
                WriteInteger(fichier, 80);
                if (local)
                    WriteLocaleChar(fichier, popobj_aux->label);
                else
                    WriteChar(fichier, popobj_aux->Area.key);
                WriteEndFunction(fichier);
            }
            WriteVarAffect(fichier, NumLabel(popobj_aux));
            WriteCreateObj(fichier, MB_PopobjectObject);
            if (obj->Help.generated)
            {
                WriteAttribut(fichier, MB_MUIA_HelpNode);
                WriteString(fichier, obj->label);
            }
            CodeArea(fichier, obj, &popobj_aux->Area,
                     TRUE, TRUE, FALSE, TRUE, TRUE,
                     FALSE,
                     (popobj_aux->Area.Background != MUII_WindowBack),
                     FALSE, FALSE);
            if (popobj_aux->openhook)
            {
                WriteAttribut(fichier, MB_MUIA_Popobject_ObjStrHook);
                WriteExternalFunction(fichier, popobj_aux->openhook);
                UpdateNodeIdent(application.Functions,
                                popobj_aux->openhook);
            }
            if (popobj_aux->closehook)
            {
                WriteAttribut(fichier, MB_MUIA_Popobject_StrObjHook);
                WriteExternalFunction(fichier, popobj_aux->closehook);
                UpdateNodeIdent(application.Functions,
                                popobj_aux->closehook);
            }
            WriteAttribut(fichier, MB_MUIA_Popstring_String);
            WriteObjArg2(fichier, NumLabel(popobj_aux) + 1);
            WriteAttribut(fichier, MB_MUIA_Popstring_Button);
            WriteFunction(fichier, MB_PopButton);
            WriteMUIArg(fichier, i);
            WriteEndFunction(fichier);
            if (!popobj_aux->light)
            {
                WriteAttribut(fichier, MB_MUIA_Popobject_Light);
                WriteBool(fichier, popobj_aux->light);
            }
            if (!popobj_aux->follow)
            {
                WriteAttribut(fichier, MB_MUIA_Popobject_Follow);
                WriteBool(fichier, popobj_aux->follow);
            }
            if (!popobj_aux->Volatile)
            {
                WriteAttribut(fichier, MB_MUIA_Popobject_Volatile);
                WriteBool(fichier, popobj_aux->Volatile);
            }
            WriteAttribut(fichier, MB_MUIA_Popobject_Object);
            WriteObjArg(fichier, popobj_aux->obj);
            WriteEnd(fichier, MB_End);
            break;
        case TY_MENU:
            menu_aux = (menu *) obj;
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                CodeCreate(nth(menu_aux->childs, i), fichier);
            }
            WriteVarAffect(fichier, NumLabel(menu_aux));
            WriteCreateObj(fichier, MB_MenustripObject);
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                WriteAttribut(fichier, MB_MUIA_Family_Child);
                WriteObjArg(fichier, nth(menu_aux->childs, i));
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_SUBMENU:
            menu_aux = (menu *) obj;
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                CodeCreate(nth(menu_aux->childs, i), fichier);
            }
            WriteVarAffect(fichier, NumLabel(menu_aux));
            WriteCreateObj(fichier, MB_MenuitemObject);
            if (strlen(menu_aux->name) > 0)
            {
                WriteAttribut(fichier, MB_MUIA_Menuitem_Title);
                if (local)
                    WriteLocaleString(fichier, menu_aux->label);
                else
                    WriteString(fichier, menu_aux->name);
            }
            if (!menu_aux->menu_enable)
            {
                WriteAttribut(fichier, MB_MUIA_Menu_Enabled);
                WriteBool(fichier, menu_aux->menu_enable);
            }
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                WriteAttribut(fichier, MB_MUIA_Family_Child);
                WriteObjArg(fichier, nth(menu_aux->childs, i));
            }
            WriteEnd(fichier, MB_End);
            break;
        case TY_MENUITEM:
            menu_aux = (menu *) obj;
            WriteVarAffect(fichier, NumLabel(menu_aux));
            if (strncmp(menu_aux->name, "BarLabel", 8) == 0)
            {
                WriteObjFunction(fichier, MB_MUI_MakeObject);
                WriteMUIArg(fichier, MB_MUIO_Menuitem);
                WriteMUIArg(fichier, MB_MN_BARLABEL);
                WriteInteger(fichier, 0);
                WriteInteger(fichier, 0);
                WriteInteger(fichier, 0);
                WriteEndFunction(fichier);
            }
            else
            {
                WriteCreateObj(fichier, MB_MenuitemObject);
                if (strlen(menu_aux->name) > 0)
                {
                    WriteAttribut(fichier, MB_MUIA_Menuitem_Title);
                    if (local)
                        WriteLocaleString(fichier, menu_aux->label);
                    else
                        WriteString(fichier, menu_aux->name);
                }
                if (!menu_aux->menu_enable)
                {
                    WriteAttribut(fichier, MB_MUIA_Menuitem_Enabled);
                    WriteBool(fichier, menu_aux->menu_enable);
                }
                if (menu_aux->key)
                {
                    WriteAttribut(fichier, MB_MUIA_Menuitem_Shortcut);
                    nospace[0] = menu_aux->key;
                    nospace[1] = '\0';
                    if (local)
                        WriteLocaleCharAsString(fichier, menu_aux->label);
                    else
                        WriteString(fichier, nospace);
                }
                if (menu_aux->check_enable)
                {
                    WriteAttribut(fichier, MB_MUIA_Menuitem_Checkit);
                    WriteBool(fichier, menu_aux->check_enable);
                    if (menu_aux->check_state)
                    {
                        WriteAttribut(fichier, MB_MUIA_Menuitem_Checked);
                        WriteBool(fichier, menu_aux->check_state);
                    }
                }
                if (menu_aux->Toggle)
                {
                    WriteAttribut(fichier, MB_MUIA_Menuitem_Toggle);
                    WriteBool(fichier, menu_aux->Toggle);
                }
                WriteEnd(fichier, MB_End);
            }
            break;
    }
}

int PositionInParametersList(APTR obj)
{
    int result = 0;
    chainon *chainon_aux;
    int i;

    chainon_aux = parameters->head;
    for (i = 1; i <= parameters->nb_elements; i++)
    {
        if (chainon_aux->element == obj)
        {
            result = i;
            break;
        }
        chainon_aux = chainon_aux->next;
    }
    return (result);
}

void AddObjAsParameter(APTR obj)
{
    if (!PositionInParametersList(obj))
        add(parameters, obj);
}

/* Write a notification for the object parameter objpar  */
/* objpar is the object on which the notification occurs */
void WriteNotify(FILE *fichier, APTR objpar, APTR father)
{
    object *obj;
    int i, id_src, id_dest, aux, num_label;
    object *obj_aux, *obj_aux2;
    event *evt;
    BOOL bool_aux;
    char buffer[120];

    obj = objpar;
    id_src = obj->id;
    for (i = 0; i < obj->notify->nb_elements; i++)
    {
        evt = nth(obj->notify, i);
        obj_aux = evt->destination;
        if (!IsParentFrom(father, obj_aux))
        {
            AddObjAsParameter(obj_aux);
            num_label =
                nb_identificateurs + PositionInParametersList(obj_aux) - 1;
        }
        else
            num_label = NumLabel(obj_aux);
        id_dest = obj_aux->id;
        if (TYActions[id_dest][evt->dest_type] == TY_WINOBJ)
        {
            if (!(bool_aux = IsParentFrom(father, evt->arguments)))
                AddObjAsParameter(evt->arguments);
        }
        switch (id_src)
        {
            case TY_APPLI:
                WriteBeginNotification(fichier, 0);
                break;
            case TY_POPASL:
                WriteBeginNotification(fichier, NumLabel(obj) + 1);
                break;
            case TY_POPOBJECT:
                WriteBeginNotification(fichier, NumLabel(obj) + 1);
                break;
            default:
                WriteBeginNotification(fichier, NumLabel(obj));
                break;
        }
        WriteMUIArg(fichier, MB_MUIM_Notify);
        WriteMUIArg(fichier, CEvenements[id_src][2 * evt->src_type + 0]);
        switch (ArgEVT[id_src][evt->src_type])
        {
            case ARG_BOOL:
                WriteBool(fichier,
                          CEvenements[id_src][2 * evt->src_type + 1]);
                break;
            case ARG_INT:
                WriteInteger(fichier,
                             CEvenements[id_src][2 * evt->src_type + 1]);
                break;
            case ARG_MUI:
                WriteMUIArgAttribute(fichier,
                                     CEvenements[id_src][2 *
                                                         evt->src_type +
                                                         1]);
                break;
        }
        switch (TYActions[id_dest][evt->dest_type])
        {
            case TY_NOTHING:
                if (id_dest == 0)
                    WriteVarArg(fichier, 0);
                else
                    WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                if (CActions[id_dest][4 * evt->dest_type + 0] > 2)
                {
                    WriteMUIArg(fichier,
                                CActions[id_dest][4 * evt->dest_type + 2]);
                    aux = CActions[id_dest][4 * evt->dest_type + 3];
                    if ((aux != 0) && (aux != 1))
                    {
                        WriteMUIArgAttribute(fichier, aux);
                    }
                    else
                        WriteBool(fichier, aux);
                }
                else if (CActions[id_dest][4 * evt->dest_type + 0] > 1)
                    WriteMUIArgAttribute(fichier,
                                         CActions[id_dest][4 *
                                                           evt->dest_type +
                                                           2]);
                WriteEndNotification(fichier);
                break;
            case TY_WINOBJ:
                obj_aux2 = evt->arguments;
                if (!bool_aux)
                    WriteVarArg(fichier,
                                nb_identificateurs +
                                PositionInParametersList(obj_aux) - 1);
                else
                    WriteVarArg(fichier, NumLabel(obj_aux2));
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                if (CActions[id_dest][4 * evt->dest_type + 2])
                    WriteMUIArg(fichier,
                                CActions[id_dest][4 * evt->dest_type + 2]);
                WriteVarArg(fichier, num_label);
                WriteEndNotification(fichier);
                break;
            case TY_FUNCTION:
                WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                WriteExternalFunction(fichier, evt->arguments);
                UpdateNodeIdent(application.Functions, evt->arguments);
                WriteEndNotification(fichier);
                break;
            case TY_ID:
                WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                WriteExternalConstant(fichier, evt->arguments);
                UpdateNodeIdent(application.Idents, evt->arguments);
                WriteEndNotification(fichier);
                break;
            case TY_VARIABLE:
                WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 2]);
                WriteVariable(fichier, evt->arguments);
                UpdateNodeIdent(application.Variables, evt->arguments);
                WriteEndNotification(fichier);
                break;
            case TY_CONS_INT:
                WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                if (CActions[id_dest][4 * evt->dest_type + 0] > 2)
                    WriteMUIArg(fichier,
                                CActions[id_dest][4 * evt->dest_type + 2]);
                WriteInteger(fichier, atoi(evt->argstring));
                WriteEndNotification(fichier);
                break;
            case TY_CONS_CHAR:
                WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 2]);
                WriteChar(fichier, evt->argstring[0]);
                WriteEndNotification(fichier);
                break;
            case TY_CONS_STRING:
                WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 2]);
                if (local)
                {
                    sprintf(buffer, "%sNotify%d", obj->label, i);
                    WriteLocaleString(fichier, buffer);
                }
                else
                    WriteString(fichier, evt->argstring);
                WriteEndNotification(fichier);
                break;
            case TY_CONS_BOOL:
                WriteVarArg(fichier, num_label);
                WriteInteger(fichier,
                             CActions[id_dest][4 * evt->dest_type + 0]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 1]);
                WriteMUIArg(fichier,
                            CActions[id_dest][4 * evt->dest_type + 2]);
                WriteBool(fichier, (evt->argstring[0] == '1'));
                WriteEndNotification(fichier);
                break;
        }
    }
}

void CreateCodeChain(FILE *fichier, window *win_aux)
{
    int i;
    object *obj_aux;

    WriteBeginNotification(fichier, NumLabel(win_aux));
    WriteMUIArg(fichier, MB_MUIM_Window_SetCycleChain);
    for (i = 0; i < win_aux->chain->nb_elements; i++)
    {
        obj_aux = nth(win_aux->chain, i);
        WriteVarArg(fichier, NumLabel(obj_aux));
    }
    WriteInteger(fichier, 0);
    WriteEndNotification(fichier);
}

void CodeNotify(FILE *fichier, APTR objpar, object *father)
{
    object *obj;
    window *win_aux;
    group *group_aux;
    popobject *popobj_aux;
    menu *menu_aux;
    int i;

    obj = objpar;
    switch (obj->id)
    {
        case TY_APPLI:
            WriteNotify(fichier, &application, father);
            if (application.appmenu->childs->nb_elements > 0)
                CodeNotify(fichier, application.appmenu, father);
            for (i = 0; i < windows->nb_elements; i++)
            {
                win_aux = nth(windows, i);
                CodeNotify(fichier, win_aux, father);
            }
            for (i = 0; i < windows->nb_elements; i++)
            {
                win_aux = nth(windows, i);
                if (win_aux->initopen)
                {
                    WriteFunction(fichier, MB_set);
                    WriteVarArg(fichier, NumLabel(win_aux));
                    WriteMUIArg(fichier, MB_MUIA_Window_Open);
                    WriteBool(fichier, win_aux->initopen);
                    WriteEndFunction(fichier);
                }
            }
            break;
        case TY_WINDOW:
            win_aux = (window *) obj;
            if (win_aux->menu->childs->nb_elements > 0)
                CodeNotify(fichier, win_aux->menu, father);
            WriteNotify(fichier, obj, father);
            for (i = 0; i < win_aux->root.child->nb_elements; i++)
            {
                CodeNotify(fichier, nth(win_aux->root.child, i), father);
            }
            CreateCodeChain(fichier, win_aux);
            break;
        case TY_GROUP:
            group_aux = (group *) obj;
            WriteNotify(fichier, obj, father);
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                CodeNotify(fichier, nth(group_aux->child, i), father);
            }
            break;
        case TY_POPOBJECT:
            popobj_aux = (popobject *) obj;
            WriteNotify(fichier, obj, father);
            CodeNotify(fichier, popobj_aux->obj, father);
            break;
        case TY_MENU:
        case TY_SUBMENU:
            menu_aux = (menu *) obj;
            WriteNotify(fichier, obj, father);
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                CodeNotify(fichier, nth(menu_aux->childs, i), father);
            }
            break;
        default:
            WriteNotify(fichier, obj, father);
            break;
    }
}

void GenerateCodeObject(APTR obj_aux)
{
    FILE *fichier;
    char buffer[512];
    char dir[512], file[512];
    CONST_STRPTR title = GetMUIBuilderString(MSG_CodeFileName);

    if (!(parameters = create()))
        return;
    strcpy(dir, genfile);
    extract_catalog(dir);
    extract_file(genfile, file);
    if (LoadRequester(title, dir, file))
    {
        strcpy(buffer, dir);
        AddPart(buffer, file, 512);

        /* Options */
        if (fichier = fopen("T:MUIBuilder1.tmp", "w+"))
        {
            fprintf(fichier, "%s%c", buffer, 0);
            fprintf(fichier, "%s%c", real_getstring, 0);
            fprintf(fichier, "%s%c", catfile, 0);
            fprintf(fichier, "%d%d%d%d%d%d%d", env, declarations,
                    code, local, notifications, 0, 0);
            fclose(fichier);
        }

        /* Code */
        nb_identificateurs = 0;
        CalculeNbIdent(obj_aux);
        numfirstlabel = 0;
        numfirstlabel = NumLabel(obj_aux);
        if (fichier = fopen("T:MUIBuilder4.tmp", "w+"))
        {
            nb_var_aux = 0;
            CodeCreate(obj_aux, fichier);
            fclose(fichier);
        }

        /* Notifications */
        if (fichier = fopen("T:MUIBuilder5.tmp", "w+"))
        {
            CodeNotify(fichier, obj_aux, obj_aux);
            fclose(fichier);
        }

        /* Variables declarations */
        if (fichier = fopen("T:MUIBuilder2.tmp", "w+"))
        {
            fprintf(fichier, "%d%c",
                    nb_identificateurs + parameters->nb_elements, 0);
            GenerateLabels(obj_aux, fichier);
            GenerateExternalLabels(fichier);
            GenerateListLabels(fichier, application.Functions,
                               TYPEVAR_HOOK);
            GenerateListLabels(fichier, application.Idents, TYPEVAR_IDENT);
            GenerateListLabels(fichier, application.Variables,
                               TYPEVAR_EXTERNAL);
            fclose(fichier);
        }

        /* Variables initializations */
        if (fichier = fopen("T:MUIBuilder3.tmp", "w+"))
        {
            /*
                les GenerateListInits doivent etre positionnes
                obligatoirement apres les GenerateListLabels
                car ils repositionnent les elements 'non generes
            */
            GenerateInitLabels(obj_aux, fichier);
            GenerateExternalInits(fichier);
            GenerateListInits(fichier, application.Functions);
            GenerateListInits(fichier, application.Idents);
            GenerateListInits(fichier, application.Variables);
            fclose(fichier);
        }

        /* Call GenCodeX */
        sprintf(buffer, "\"%s", MBDir);
        AddPart(buffer, "Modules/GenCode", 512);
        strncat(buffer, config.langage, 512);
        strcat(buffer, "\"");
        set(TextInfo, MUIA_Text_Contents,
            GetMUIBuilderString(MSG_GenerateSource));

        // *INDENT-OFF*
        SystemTags(buffer,
                   SYS_Input, Open("CON:0/50//200/GenCode/AUTO/CLOSE/WAIT", MODE_NEWFILE),
                   SYS_Output, NULL,
                   SYS_Asynch, TRUE,
                   SYS_UserShell, TRUE,
                   NP_ConsoleTask, NULL,
                   TAG_DONE);
        // *INDENT-ON*
    }
    if (parameters)
    {
        delete_all(parameters);
        FreeVec(parameters);
    }
}

void GenerateCode(UNUSED queue *windows)
{
    FILE *fichier;
    char buffer[512];

    if (!(parameters = create()))
        return;

    /* Options */
    if (fichier = fopen("T:MUIBuilder1.tmp", "w+"))
    {
        fprintf(fichier, "%s%c", genfile, 0);
        NoSpace(application.title);
        if (strlen(real_getstring) == 0)
        {
            if (strlen(get_string) != 0)
                strcpy(real_getstring, get_string);
            else
                sprintf(real_getstring, "Get%sString", nospace);
        }
        fprintf(fichier, "%s%c", real_getstring, 0);
        fprintf(fichier, "%s%c", catfile, 0);
        fprintf(fichier, "%d%d%d%d%d%d%d", env, declarations, code, local,
                notifications, 1, 0);
        fclose(fichier);
    }
    nb_identificateurs = 0;
    numfirstlabel = 0;
    CalculeNbIdent(&application);

    /* Code */
    if (fichier = fopen("T:MUIBuilder4.tmp", "w+"))
    {
        nb_var_aux = 0;
        CodeCreate((object *) &application, fichier);
        fclose(fichier);
    }

    /* Notifications */
    if (fichier = fopen("T:MUIBuilder5.tmp", "w+"))
    {
        CodeNotify(fichier, &application, (object *) &application);
        fclose(fichier);
    }

    /* Variables declarations */
    if (fichier = fopen("T:MUIBuilder2.tmp", "w+"))
    {
        fprintf(fichier, "%d%c",
                nb_identificateurs + parameters->nb_elements, 0);
        GenerateLabels(&application, fichier);
        GenerateExternalLabels(fichier);
        GenerateListLabels(fichier, application.Functions, TYPEVAR_HOOK);
        GenerateListLabels(fichier, application.Idents, TYPEVAR_IDENT);
        GenerateListLabels(fichier, application.Variables,
                           TYPEVAR_EXTERNAL);
        fclose(fichier);
    }

    /* Variables initializations */
    if (fichier = fopen("T:MUIBuilder3.tmp", "w+"))
    {
        GenerateInitLabels(&application, fichier);
        GenerateExternalInits(fichier);
        GenerateListInits(fichier, application.Functions);
        GenerateListInits(fichier, application.Idents);
        GenerateListInits(fichier, application.Variables);
        fclose(fichier);
    }

    /* Call GenCodeX */
    sprintf(buffer, "\"%s", MBDir);
    AddPart(buffer, "Modules/GenCode", 512);
    strncat(buffer, config.langage, 512);
    strcat(buffer, "\"");
    set(TextInfo, MUIA_Text_Contents,
        GetMUIBuilderString(MSG_GenerateSource));

    // *INDENT-OFF*
    SystemTags(buffer,
               SYS_Input, Open("CON:0/50//200/GenCode/AUTO/CLOSE/WAIT", MODE_NEWFILE),
               SYS_Output, NULL,
               SYS_Asynch, TRUE,
               SYS_UserShell, TRUE,
               NP_ConsoleTask, NULL,
               TAG_DONE);
    // *INDENT-ON*

    if (parameters)
    {
        delete_all(parameters);
        FreeVec(parameters);
    }
}

void Code()
{
    APTR WI_code, lv_labels, GR_grp_0, bt_app, bt_object, bt_remove,
        lv_genlabels;
    APTR bt_add, bt_quit, bt_catalog, RegGroup, STR_CatalogPrepend;
    APTR CM_declarations, CM_code, CM_env, CM_local, CM_notifications,
        CM_genall;
    APTR STR_GetString, STR_filename, STR_catalog;
    int active;
    ULONG signal;
    BOOL running = TRUE;
    BOOL bool_aux;
    int i;
    object *obj_aux;
    char *chaine_aux;
    CONST_STRPTR RegisterTitle[3];

    RegisterTitle[0] = GetMUIBuilderString(MSG_CodeOption);
    RegisterTitle[1] = GetMUIBuilderString(MSG_Options);
    RegisterTitle[2] = NULL;

    NoSpace(application.title);
    if (strlen(real_getstring) == 0)
    {
        if (strlen(get_string) != 0)
            strcpy(real_getstring, get_string);
        else
            sprintf(real_getstring, "Get%sString", nospace);
    }
    if (strlen(genfile) == 0)
        sprintf(genfile, "%s", application.title);
    if (strlen(genfile) == 0)
        sprintf(genfile, "ObjectFile");
    if (strlen(catfile) == 0)
        sprintf(catfile, "%sStrings.cd", application.title);
    if (strlen(catprepend) == 0)
        strcpy(catprepend, "MSG_");

    // *INDENT-OFF*
    WI_code = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_CodeOption),
        MUIA_Window_ID, MAKE_ID('C', 'O', 'D', 'E'),
        MUIA_HelpNode, "Code",
        WindowContents, VGroup, /*bug MUI */
            Child, RegGroup = RegisterGroup(RegisterTitle),
                Child, GroupObject,
                    Child, GroupObject,
                        MUIA_Group_Horiz, 1,
                        MUIA_Group_SameHeight, 1,
                        Child, lv_labels = ListviewObject,
                            GroupFrameT(GetMUIBuilderString(MSG_ObjectsLabels)),
                            MUIA_Listview_List, ListObject,
                                InputListFrame,
                            End,
                        End,
                        Child, lv_genlabels = ListviewObject,
                            GroupFrameT(GetMUIBuilderString(MSG_GeneratedLabels)),
                            MUIA_Listview_List, ListObject,
                                InputListFrame,
                            End,
                        End,
                        Child, GR_grp_0 = GroupObject,
                            MUIA_Weight, 25,
                            Child, HVSpace,
                            Child, bt_app = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_AppCode)),
                            Child, HVSpace,
                            Child, bt_object = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_ObjectCode)),
                            Child, HVSpace,
                            Child, bt_remove = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_RemoveLabel)),
                            Child, HVSpace,
                            Child, bt_add = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_AddLabel)),
                            Child, HVSpace,
                            Child, bt_catalog = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_CatalogButton)),
                            Child, HVSpace,
                            Child, bt_quit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Quit)),
                            Child, HVSpace,
                        End,
                    End,
                    Child, TextInfo = TextObject,
                        MUIA_Text_PreParse, "\033c",
                        TextFrame,
                        MUIA_ShowMe, FALSE,
                    End,
                End,
                Child, VGroup,
                    Child, ColGroup(4),
                        GroupFrameT(GetMUIBuilderString(MSG_Options)),
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Declarations)),
                        Child, CM_declarations = CheckMark(declarations),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Environment)),
                        Child, CM_env = CheckMark(env),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_CodeOption)),
                        Child, CM_code = CheckMark(code),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_GenerateAllIDs)),
                        Child, CM_genall = CheckMark(generate_all),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_NotificationsOption)),
                        Child, CM_notifications = CheckMark(notifications),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Locale)),
                        Child, CM_local = CheckMark(local),
                        Child, HVSpace,
                    End,
                    Child, HVSpace,
                        Child, ColGroup(2),
                        Child, Label2(GetMUIBuilderString(MSG_GetString)),
                        Child, STR_GetString = StringObject,
                            StringFrame,
                            MUIA_String_Contents, real_getstring,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_CatalogPrepend)),
                        Child, STR_CatalogPrepend = StringObject,
                            StringFrame,
                            MUIA_String_Contents, catprepend,
                            MUIA_String_MaxLen, 5,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_CodeOption)),
                        Child, PopaslObject, MUIA_Popasl_Type, 0,
                            MUIA_Popstring_String, STR_filename = String(genfile, 256),
                            MUIA_Popstring_Button, PopButton(MUII_PopFile),
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Catalog)),
                        Child, PopaslObject,
                            MUIA_Popasl_Type, 0,
                            MUIA_Popstring_String, STR_catalog = String(catfile, 256),
                            MUIA_Popstring_Button, PopButton(MUII_PopFile),
                        End,
                    End,
                End,
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_code;

    DoMethod(bt_app, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GENERATE);
    DoMethod(bt_object, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GENERATEWIN);
    DoMethod(bt_remove, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DELETE);
    DoMethod(bt_add, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(bt_quit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_code, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(bt_catalog, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_CATALOG);
    DoMethod(STR_GetString, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_code, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_filename);
    DoMethod(STR_filename, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_code, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_catalog);
    DoMethod(STR_catalog, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_code, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_GetString);
    DoMethod(app, OM_ADDMEMBER, WI_code);

    DoMethod(WI_code, MUIM_Window_SetCycleChain, RegGroup, lv_labels,
             lv_genlabels, bt_app, bt_object, bt_remove, bt_add,
             bt_catalog, bt_quit, CM_declarations, CM_env, CM_code,
             CM_local, STR_GetString, STR_CatalogPrepend, STR_filename,
             STR_catalog, NULL);

    set(app, MUIA_Application_Sleep, TRUE);

    /* Create variables lists */
    genlabels = create();
    delete_all(labels);
    CreateLabels(&application, NULL);


    set(WI_code, MUIA_Window_Open, TRUE);

    VerifyLabels();

    set(lv_labels, MUIA_List_Quiet, TRUE);
    set(lv_genlabels, MUIA_List_Quiet, TRUE);
    for (i = 0; i < labels->nb_elements; i++)
    {
        obj_aux = nth(labels, i);
        chaine_aux = obj_aux->label;
        DoMethod(lv_labels, MUIM_List_Insert, &chaine_aux, 1,
                 MUIV_List_Insert_Bottom);
        if (obj_aux->generated)
        {
            add(genlabels, obj_aux);
            DoMethod(lv_genlabels, MUIM_List_Insert, &chaine_aux, 1,
                     MUIV_List_Insert_Bottom);
        }
    }
    set(lv_labels, MUIA_List_Quiet, FALSE);
    set(lv_genlabels, MUIA_List_Quiet, FALSE);

    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_GENERATE:
                if (windows->nb_elements > 0)
                {
                    set(TextInfo, MUIA_ShowMe, TRUE);
                    set(TextInfo, MUIA_Text_Contents,
                        GetMUIBuilderString(MSG_CodeGeneration));
                    get(CM_env, MUIA_Selected, &active);
                    env = (active == 1);
                    get(CM_code, MUIA_Selected, &active);
                    code = (active == 1);
                    get(CM_declarations, MUIA_Selected, &active);
                    declarations = (active == 1);
                    get(CM_local, MUIA_Selected, &active);
                    local = (active == 1);
                    get(CM_notifications, MUIA_Selected, &active);
                    notifications = (active == 1);
                    get(CM_genall, MUIA_Selected, &active);
                    generate_all = (active == 1);
                    strcpy(real_getstring, GetStr(STR_GetString));
                    strcpy(genfile, GetStr(STR_filename));
                    strcpy(catfile, GetStr(STR_catalog));
                    strcpy(catprepend, GetStr(STR_CatalogPrepend));
                    set(app, MUIA_Application_Sleep, TRUE);
                    GenerateCode(windows);
                    set(app, MUIA_Application_Sleep, FALSE);
                    set(TextInfo, MUIA_ShowMe, FALSE);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedWindows));
                break;
            case ID_GENERATEWIN:
                get(lv_genlabels, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    set(TextInfo, MUIA_ShowMe, TRUE);
                    set(TextInfo, MUIA_Text_Contents,
                        GetMUIBuilderString(MSG_CodeGeneration));
                    get(CM_env, MUIA_Selected, &active);
                    env = (active == 1);
                    get(CM_code, MUIA_Selected, &active);
                    code = (active == 1);
                    get(CM_declarations, MUIA_Selected, &active);
                    declarations = (active == 1);
                    get(CM_local, MUIA_Selected, &active);
                    local = (active == 1);
                    get(CM_notifications, MUIA_Selected, &active);
                    notifications = (active == 1);
                    get(CM_genall, MUIA_Selected, &active);
                    generate_all = (active == 1);
                    strcpy(real_getstring, GetStr(STR_GetString));
                    get(lv_genlabels, MUIA_List_Active, &active);
                    strcpy(genfile, GetStr(STR_filename));
                    strcpy(catfile, GetStr(STR_catalog));
                    strcpy(catprepend, GetStr(STR_CatalogPrepend));
                    set(app, MUIA_Application_Sleep, TRUE);
                    GenerateCodeObject(nth(genlabels, active));
                    set(app, MUIA_Application_Sleep, FALSE);
                    set(TextInfo, MUIA_ShowMe, FALSE);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectLabel));
                break;
            case ID_NEWOBJ:
                get(lv_labels, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    obj_aux = nth(labels, active);
                    bool_aux = FALSE;
                    i = 0;
                    while ((!bool_aux) && (i < genlabels->nb_elements))
                    {
                        bool_aux = (obj_aux == nth(genlabels, i));
                        i++;
                    }
                    if (bool_aux)
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_AlreadyGenerated));
                    else
                    {
                        add(genlabels, obj_aux);
                        chaine_aux = obj_aux->label;
                        set(lv_genlabels, MUIA_List_Quiet, TRUE);
                        DoMethod(lv_genlabels, MUIM_List_Insert,
                                 &chaine_aux, 1, MUIV_List_Insert_Bottom);
                        set(lv_genlabels, MUIA_List_Quiet, FALSE);
                        obj_aux->generated = TRUE;
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_DELETE:
                get(lv_genlabels, MUIA_List_Active, &active);
                bool_aux = TRUE;
                if (active != MUIV_List_Active_Off)
                {
                    if (config.request)
                        bool_aux =
                            RequestMessage(GetMUIBuilderString
                                           (MSG_Confirm));
                    if (bool_aux)
                    {
                        obj_aux = nth(genlabels, active);
                        obj_aux->generated = FALSE;
                        delete_nth(genlabels, active);
                        DoMethod(lv_genlabels, MUIM_List_Remove, active);
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectLabel));
                break;
            case ID_CATALOG:
                strcpy(catprepend, GetStr(STR_CatalogPrepend));
                strcpy(catfile, GetStr(STR_catalog));
                CreateCatalog();
                break;
            case ID_END:
                get(CM_env, MUIA_Selected, &active);
                env = (active == 1);
                get(CM_code, MUIA_Selected, &active);
                code = (active == 1);
                get(CM_declarations, MUIA_Selected, &active);
                declarations = (active == 1);
                strcpy(real_getstring, GetStr(STR_GetString));
                get(CM_local, MUIA_Selected, &active);
                local = (active == 1);
                get(CM_notifications, MUIA_Selected, &active);
                notifications = (active == 1);
                get(CM_genall, MUIA_Selected, &active);
                generate_all = (active == 1);
                strcpy(genfile, GetStr(STR_filename));
                strcpy(catfile, GetStr(STR_catalog));
                strcpy(catprepend, GetStr(STR_CatalogPrepend));
                running = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    delete_all(genlabels);
    FreeVec(genlabels);
    set(WI_code, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_code);
    MUI_DisposeObject(WI_code);
    set(app, MUIA_Application_Sleep, FALSE);
}
