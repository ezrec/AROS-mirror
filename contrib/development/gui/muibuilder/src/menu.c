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
#include <ctype.h>

void InitMenu(menu *menu_aux)
{
    sprintf(menu_aux->label, "MN_label_%d", nb_menu);
    menu_aux->name[0] = '\0';
    menu_aux->menu_enable = TRUE;
    menu_aux->check_enable = FALSE;
    menu_aux->check_state = FALSE;
    menu_aux->Toggle = FALSE;
    menu_aux->key = '\0';
    menu_aux->fold = FALSE;
    menu_aux->childs = create();
    menu_aux->notify = create();
    menu_aux->notifysource = create();
}

APTR LoadMenu(FILE *fichier, APTR father, UNUSED int version, int id)
{
    int i;
    menu *menu_aux, *tmp_menu;

    menu_aux = AllocVec(sizeof(menu), MEMF_PUBLIC | MEMF_CLEAR);
    if (!menu_aux)
        return (NULL);

    InitMenu(menu_aux);
    menu_aux->id = id;
    menu_aux->father = father;
    strcpy(menu_aux->label, LitChaine(fichier));
    menu_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, menu_aux);
    ReadNotify(fichier, menu_aux);
    menu_aux->menu_enable = ReadInt(fichier);
    menu_aux->check_enable = ReadInt(fichier);
    menu_aux->check_state = ReadInt(fichier);
    menu_aux->Toggle = ReadInt(fichier);
    menu_aux->key = ((char *) LitChaine(fichier))[0];
    strcpy(menu_aux->name, LitChaine(fichier));
    if ((id == TY_MENU) || (id == TY_SUBMENU))
    {
        menu_aux->fold = TRUE;
        do
        {
            i = ReadInt(fichier);
            if (i != -1)
            {
                tmp_menu = LoadObject(fichier, menu_aux, i);
                add(menu_aux->childs, tmp_menu);
            }
        }
        while (i != -1);
    }
    return (menu_aux);
}

void SaveMenu(FILE *fichier, menu *menu_aux)
{
    int i;

    fprintf(fichier, "%d\n", menu_aux->menu_enable);
    fprintf(fichier, "%d\n", menu_aux->check_enable);
    fprintf(fichier, "%d\n", menu_aux->check_state);
    fprintf(fichier, "%d\n", menu_aux->Toggle);
    fprintf(fichier, "%c\n", menu_aux->key);
    fprintf(fichier, "%s\n", menu_aux->name);
    switch (menu_aux->id)
    {
        case TY_MENU:
        case TY_SUBMENU:
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                SaveObject(nth(menu_aux->childs, i), fichier);
            }
            fprintf(fichier, "%d\n", -1);
            break;
        case TY_MENUITEM:
            break;
    }
}

void StringToLabel(char *src, char *dest)
{
    while (*src)
    {
        if (isalnum(*src))
        {
            *dest++ = *src;
        }
        src++;
    }
    *dest = *src;
}


menu *CreateMenu(BOOL generated)
{
    menu *menu_aux;

    menu_aux = AllocVec(sizeof(menu), MEMF_PUBLIC | MEMF_CLEAR);
    if (!menu_aux)
        return (NULL);

    InitMenu(menu_aux);
    menu_aux->id = TY_MENU;
    menu_aux->generated = generated;
    menu_aux->Help.nb_char = 0;
    menu_aux->fold = TRUE;
    strcpy(menu_aux->Help.title, "Menu Gadget");
    nb_menu++;
    return (menu_aux);
}

//LONG MenuDisplayFunc(char **array __asm("a2"), menu* menu_aux __asm("a1"))
HOOKPROTONH(MenuDisplayFunc, LONG, char **array, menu* menu_aux)
{
    static char buffer[10], buffer2[150];
    menu *aux;
    char concat[2];

    concat[0] = config.tree_char;
    concat[1] = '\0';
    buffer[0] = '\0';
    buffer2[0] = '\0';
    if (menu_aux->id == TY_SUBMENU)
    {
        if (!menu_aux->fold)
            strcpy(buffer, "\033I[6:30] ");
        else
            strcpy(buffer, "\033I[6:39] ");
        aux = menu_aux->father;
        while (aux->id == TY_SUBMENU)
        {
            strcat(buffer2, concat);
            aux = aux->father;
        }
        strcat(buffer2, "\0338 ");
        strcat(buffer2, menu_aux->name);
    }
    else
    {
        buffer[0] = '\0';
        aux = menu_aux->father;
        while (aux->id == TY_SUBMENU)
        {
            strcat(buffer2, concat);
            aux = aux->father;
        }
        strcat(buffer2, " ");
        strcat(buffer2, menu_aux->name);
    }

    *array++ = buffer;
    *array = buffer2;

    return (0);
}

void InitMenuList(APTR list, menu *menu_aux)
{
    int i;

    switch (menu_aux->id)
    {
        case TY_SUBMENU:
            DoMethod(list, MUIM_List_Insert, &menu_aux, 1,
                     MUIV_List_Insert_Bottom);
            // FIXME: intentionally falltrough?
        case TY_MENU:
            if (menu_aux->fold)
                for (i = 0; i < menu_aux->childs->nb_elements; i++)
                {
                    InitMenuList(list, nth(menu_aux->childs, i));
                }
            break;
        case TY_MENUITEM:
            DoMethod(list, MUIM_List_Insert, &menu_aux, 1,
                     MUIV_List_Insert_Bottom);
            break;
    }
}

BOOL IsMenuParent(menu *father, menu *menu_aux)
{
    if (menu_aux->father == father)
        return (TRUE);
    if (menu_aux->id == TY_MENU)
        return (FALSE);
    return (IsMenuParent(father, menu_aux->father));
}

menu *ParentMenu(menu *menu_aux)
{
    if (menu_aux->id == TY_MENU)
        return (menu_aux);
    else
        return (ParentMenu(menu_aux->father));
}

void AddToSubMenu(APTR list, menu *father, menu *menu_aux)
{
    int i, j, n;
    menu *tmp_menu;

    set(list, MUIA_List_Quiet, TRUE);
    get(list, MUIA_List_Entries, &n);
    for (i = 0; i < n; i++)
    {
        DoMethod(list, MUIM_List_GetEntry, i, &tmp_menu);
        if (tmp_menu == father)
            break;
    }
    father->fold = TRUE;
    DoMethod(list, MUIM_List_Redraw, i);
    for (j = i + 1; j < n; j++)
    {
        DoMethod(list, MUIM_List_GetEntry, j, &tmp_menu);
        if (!IsMenuParent(father, tmp_menu))
            break;
    }
    DoMethod(list, MUIM_List_Insert, &menu_aux, 1, j);
    set(list, MUIA_List_Active, j);
    set(list, MUIA_List_Quiet, FALSE);
}

void DeleteMenuEntry(APTR list, menu *menu_aux, int position)
{
    menu *tmp_menu;
    BOOL bool_aux;

    DoMethod(list, MUIM_List_Remove, position);
    if (menu_aux->id != TY_MENUITEM)
    {
        do
        {
            DoMethod(list, MUIM_List_GetEntry, position, &tmp_menu);
            if (tmp_menu)
            {
                if (bool_aux = IsMenuParent(menu_aux, tmp_menu))
                    DoMethod(list, MUIM_List_Remove, position);
            }
            else
                bool_aux = FALSE;
        }
        while (bool_aux);
    }
}

int DisplayMenuEntry(APTR list, menu *menu_aux, int position)
{
    int pos, i;

    DoMethod(list, MUIM_List_Insert, &menu_aux, 1, position);
    pos = position + 1;
    if ((menu_aux->id == TY_SUBMENU) && (menu_aux->fold))
    {
        for (i = 0; i < menu_aux->childs->nb_elements; i++)
        {
            pos = DisplayMenuEntry(list, nth(menu_aux->childs, i), pos);
        }
    }
    return (pos);
}

BOOL NewMenu(menu *menu_aux, UNUSED BOOL new)
{
    APTR WI_Menu, TX_label_1, CH_Check, CH_enable;
    APTR CH_CheckState, CH_Toggle, STR_ShortCut;
    APTR LV_Menu, BT_NewMenu, BT_SubMenu, BT_MenuItem, BT_Notify;
    APTR BT_Delete, BT_Up, BT_Down, STR_Entry, STR_label2;
    APTR STR_Label, BT_Ok, BT_Barlabel;
    APTR obj_aux0, obj_aux1, obj_aux2;
    APTR obj_aux3, obj_aux4, obj_aux5;
    APTR obj_aux6, obj_aux7, obj_aux8;
    APTR obj_aux9, obj_aux10, obj_aux11;
    APTR obj_aux12, obj_aux13, obj_aux14;
    APTR obj_aux15, obj_aux16, obj_aux17;
    APTR obj_aux18, obj_aux19, obj_aux20, obj_aux21;
    BOOL result = FALSE;
    BOOL running = TRUE;
    ULONG signal;
    LONG active;
    CONST_STRPTR aux;
    menu *tmp_menu, *father, *tmp_menu2;
    int action = 0;
    int i, j, n;

    MakeStaticHook(MenuDisplayHook, MenuDisplayFunc);

    obj_aux1 =
        MUI_MakeObject(MUIO_BarTitle,
                       GetMUIBuilderString(MSG_MenuCreation));

    TX_label_1 = HVSpace;
    obj_aux6 = Label(GetMUIBuilderString(MSG_EnableCheck));
    CH_Check = CheckMark(FALSE);
    obj_aux7 = Label(GetMUIBuilderString(MSG_CheckState));
    CH_CheckState = CheckMark(FALSE);
    obj_aux8 = Label(GetMUIBuilderString(MSG_Toggle));
    CH_Toggle = CheckMark(FALSE);
    obj_aux9 = Label(GetMUIBuilderString(MSG_ShortCut));
    STR_ShortCut = StringObject, StringFrame, MUIA_String_MaxLen, 2, End;

    // *INDENT-OFF*
    obj_aux5 = GroupObject,
        MUIA_Disabled, TRUE,
        Child, GroupObject,
            MUIA_Group_Columns, 2,
            Child, obj_aux6,
            Child, CH_Check,
            Child, obj_aux7,
            Child, CH_CheckState,
            Child, obj_aux8,
            Child, CH_Toggle,
        End,
        Child, ColGroup(2),
            Child, obj_aux9,
            Child, STR_ShortCut,
        End,
    End;

    obj_aux10 = HVSpace;

    LV_Menu = ListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_List_Format, "COL=0 P=\033C DELTA=8, COL=1 DELTA=8",
        MUIA_List_DisplayHook, &MenuDisplayHook,
    End;

    obj_aux4 = GroupObject,
        Child, TX_label_1,
        Child, ColGroup(2),
            Child, Label(GetMUIBuilderString(MSG_Enable)),
            Child, CH_enable = CheckMark(menu_aux->menu_enable),
        End,
        Child, ColGroup(2),
            Child, Label(GetMUIBuilderString(MSG_Label)),
            Child, STR_label2 = StringObject,
                StringFrame,
            End,
        End,
        Child, MUI_MakeObject(MUIO_HBar),
        Child, obj_aux5,
        Child, obj_aux10,
    End;

    obj_aux11 = RectangleObject,
        MUIA_Rectangle_VBar, TRUE,
        MUIA_FixWidth, 5,
    End;

    LV_Menu = ListviewObject,
        MUIA_Listview_Input, TRUE,
        MUIA_Listview_List, LV_Menu,
    End;

    BT_NewMenu = SimpleButton(GetMUIBuilderString(MSG_NewTitle));
    BT_SubMenu = SimpleButton(GetMUIBuilderString(MSG_SubMenu));
    BT_MenuItem = SimpleButton(GetMUIBuilderString(MSG_MenuItem));
    BT_Barlabel = SimpleButton("_Barlabel");
    obj_aux15 = HVSpace;
    BT_Delete = SimpleButton(GetMUIBuilderString(MSG_DeleteMenu));
    BT_Up = SimpleButton(GetMUIBuilderString(MSG_Up));
    BT_Down = SimpleButton(GetMUIBuilderString(MSG_Down));

    obj_aux14 = GroupObject,
        MUIA_Weight, 20,
        Child, BT_NewMenu,
        Child, BT_SubMenu,
        Child, BT_MenuItem,
        Child, BT_Barlabel,
        Child, BT_Notify = SimpleButton(GetMUIBuilderString(MSG_Notify)),
        Child, obj_aux15,
        Child, BT_Delete,
        Child, BT_Up,
        Child, BT_Down,
    End;

    obj_aux13 = GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, LV_Menu,
        Child, obj_aux14,
    End;

    obj_aux17 = Label(GetMUIBuilderString(MSG_Entry));

    STR_Entry = StringObject,
        StringFrame,
        MUIA_Disabled, TRUE,
    End;

    obj_aux16 = GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, obj_aux17,
        Child, STR_Entry,
    End;

    obj_aux12 = GroupObject, Child, obj_aux13, Child, obj_aux16, End;

    obj_aux3 = GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, obj_aux4, Child, obj_aux11, Child, obj_aux12, End;

    obj_aux2 = GroupObject,
        Child, obj_aux3,
    End;

    obj_aux18 = RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    obj_aux20 = Label(GetMUIBuilderString(MSG_Label));

    STR_Label = StringObject,
        StringFrame,
        MUIA_String_Contents, menu_aux->label,
    End;

    obj_aux19 = GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, obj_aux20,
        Child, STR_Label,
    End;

    BT_Ok = SimpleButton(GetMUIBuilderString(MSG_Quit));

    obj_aux21 = GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_SameWidth, TRUE,
        Child, BT_Ok,
    End;

    obj_aux0 = GroupObject,
        Child, obj_aux1,
        Child, obj_aux2,
        Child, obj_aux18,
        Child, obj_aux19,
        Child, obj_aux21,
    End;

    WI_Menu = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_MenuCreation),
        MUIA_Window_ID, MAKE_ID('M', 'E', 'N', 'U'),
        MUIA_HelpNode, "Menus",
        WindowContents, obj_aux0,
    End;
    // *INDENT-ON*

    WI_current = WI_Menu;
    DoMethod(WI_Menu, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_Ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_Up, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_UP);
    DoMethod(BT_Down, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DOWN);
    DoMethod(BT_Delete, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DELETE);
    DoMethod(BT_Notify, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NOTIFY);
    DoMethod(BT_NewMenu, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEW_MENUTITLE);
    DoMethod(BT_MenuItem, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEW_MENUITEM);
    DoMethod(BT_SubMenu, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEW_SUBMENU);
    DoMethod(BT_Barlabel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWBARLABEL);
    DoMethod(LV_Menu, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_DBCLICK);
    DoMethod(STR_Entry, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_ACTION);

    DoMethod(CH_Check, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, app, 2,
             MUIM_Application_ReturnID, ID_VALIDATE);
    DoMethod(CH_CheckState, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
             app, 2, MUIM_Application_ReturnID, ID_VALIDATE);
    DoMethod(CH_Toggle, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, app, 2,
             MUIM_Application_ReturnID, ID_VALIDATE);
    DoMethod(STR_ShortCut, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, app, 2, MUIM_Application_ReturnID,
             ID_VALIDATE);
    DoMethod(STR_label2, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, app, 2, MUIM_Application_ReturnID,
             ID_VALIDATE);
    DoMethod(STR_label2, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_Menu, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_ShortCut);
    DoMethod(STR_ShortCut, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_Menu, 3, MUIM_Set,
             MUIA_Window_ActiveObject, LV_Menu);

    DoMethod(LV_Menu, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app,
             2, MUIM_Application_ReturnID, ID_UPDATE);

    DoMethod(WI_Menu,
             MUIM_Window_SetCycleChain, CH_enable, STR_label2,
             CH_Check, CH_CheckState, CH_Toggle,
             STR_ShortCut, LV_Menu, STR_Label, STR_Entry,
             BT_NewMenu, BT_SubMenu, BT_MenuItem, BT_Notify, BT_Delete,
             BT_Up, BT_Down, BT_Ok, NULL);

    InitMenuList(LV_Menu, menu_aux);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_Menu);
    set(WI_Menu, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_UPDATE:
                DoMethod(LV_Menu, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &tmp_menu);
                if (tmp_menu)
                {
                    if ((tmp_menu->id == TY_SUBMENU)
                        || ((tmp_menu->id == TY_MENUITEM)
                            && (strncmp(tmp_menu->name, "BarLabel", 8) ==
                                0)))
                    {
                        set(obj_aux5, MUIA_Disabled, TRUE);
                        set(STR_label2, MUIA_String_Contents,
                            tmp_menu->label);
                        break;
                    }
                    else
                        set(obj_aux5, MUIA_Disabled, FALSE);
                    set(CH_enable, MUIA_Selected, tmp_menu->menu_enable);
                    set(CH_Check, MUIA_Selected, tmp_menu->check_enable);
                    set(CH_Toggle, MUIA_Selected, tmp_menu->Toggle);
                    set(CH_CheckState, MUIA_Selected,
                        tmp_menu->check_state);
                    set(STR_ShortCut, MUIA_String_Contents,
                        &tmp_menu->key);
                    set(STR_label2, MUIA_String_Contents, tmp_menu->label);
                }
                break;
            case ID_VALIDATE:
                DoMethod(LV_Menu, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &tmp_menu);
                if (tmp_menu)
                {
                    get(CH_Check, MUIA_Selected, &active);
                    tmp_menu->check_enable = (active == 1);
                    get(CH_CheckState, MUIA_Selected, &active);
                    tmp_menu->check_state = (active == 1);
                    get(CH_Toggle, MUIA_Selected, &active);
                    tmp_menu->Toggle = (active == 1);
                    get(CH_enable, MUIA_Selected, &active);
                    tmp_menu->menu_enable = (active == 1);
                    aux = GetStr(STR_ShortCut);
                    tmp_menu->key = *aux;
                    aux = GetStr(STR_label2);
                    strcpy(tmp_menu->label, aux);
                }
                break;
            case ID_DBCLICK:
                get(LV_Menu, MUIA_Listview_ClickColumn, &active);
                if (active == MUIV_List_Active_Off)
                    break;
                DoMethod(LV_Menu, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &tmp_menu);
                if ((active == 0) && (tmp_menu->id == TY_SUBMENU))
                {
                    get(LV_Menu, MUIA_List_Active, &active);
                    DoMethod(LV_Menu, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &tmp_menu);
                    set(LV_Menu, MUIA_List_Quiet, TRUE);
                    DeleteMenuEntry(LV_Menu, tmp_menu, active);
                    tmp_menu->fold = !(tmp_menu->fold);
                    DisplayMenuEntry(LV_Menu, tmp_menu, active);
                    set(LV_Menu, MUIA_List_Active, active);
                    set(LV_Menu, MUIA_List_Quiet, FALSE);
                }
                if (active == 1)
                {
                    action = ID_EDIT;
                    DoMethod(LV_Menu, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &tmp_menu);
                    set(STR_Entry, MUIA_String_Contents, tmp_menu->name);
                    set(STR_Entry, MUIA_Disabled, FALSE);
                    set(WI_Menu, MUIA_Window_ActiveObject, STR_Entry);
                }
                break;
            case ID_ACTION:
                switch (action)
                {
                    case ID_EDIT:
                        get(LV_Menu, MUIA_List_Active, &active);
                        if (active != MUIV_List_Active_Off)
                        {
                            DoMethod(LV_Menu, MUIM_List_GetEntry,
                                     MUIV_List_GetEntry_Active, &tmp_menu);
                            strcpy(tmp_menu->name,
                                   (char *) GetStr(STR_Entry));
                            DoMethod(LV_Menu, MUIM_List_Redraw, active);
                        }
                        break;
                    case ID_NEW_MENUTITLE:
                        if (tmp_menu =
                            AllocVec(sizeof(menu),
                                     MEMF_PUBLIC | MEMF_CLEAR))
                        {
                            InitMenu(tmp_menu);
                            tmp_menu->id = TY_SUBMENU;
                            tmp_menu->father = menu_aux;
                            strcpy(tmp_menu->name, GetStr(STR_Entry));
                            strcpy(tmp_menu->label, menu_aux->label);
                            strncat(tmp_menu->label, tmp_menu->name, 80);
                            StringToLabel(tmp_menu->label,
                                          tmp_menu->label);
                            DoMethod(LV_Menu, MUIM_List_Insert, &tmp_menu,
                                     1, MUIV_List_Insert_Bottom);
                            add(menu_aux->childs, tmp_menu);
                            set(LV_Menu, MUIA_List_Active,
                                menu_aux->childs->nb_elements);
                            set(WI_Menu, MUIA_Window_ActiveObject,
                                LV_Menu);
                        }
                        break;
                    case ID_NEW_SUBMENU:
                        get(LV_Menu, MUIA_List_Active, &active);
                        if (active != MUIV_List_Active_Off)
                        {
                            DoMethod(LV_Menu, MUIM_List_GetEntry,
                                     MUIV_List_GetEntry_Active, &father);
                            tmp_menu2 = father->father;
                            while (tmp_menu2->id != TY_MENU)
                            {
                                father = tmp_menu2;
                                tmp_menu2 = tmp_menu2->father;

                            }
                            if (tmp_menu =
                                AllocVec(sizeof(menu),
                                         MEMF_PUBLIC | MEMF_CLEAR))
                            {
                                InitMenu(tmp_menu);
                                tmp_menu->id = TY_SUBMENU;
                                tmp_menu->father = father;
                                aux = GetStr(STR_Entry);
                                strcpy(tmp_menu->name, aux);
                                strcpy(tmp_menu->label, menu_aux->label);
                                strncat(tmp_menu->label, tmp_menu->name,
                                        80);
                                StringToLabel(tmp_menu->label,
                                              tmp_menu->label);
                                AddToSubMenu(LV_Menu, father, tmp_menu);
                                add(father->childs, tmp_menu);
                                set(WI_Menu, MUIA_Window_ActiveObject,
                                    LV_Menu);
                            }
                        }
                        else
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_SelectMenuTitle));
                        break;
                    case ID_NEW_MENUITEM:
                        get(LV_Menu, MUIA_List_Active, &active);
                        if (active != MUIV_List_Active_Off)
                        {
                            DoMethod(LV_Menu, MUIM_List_GetEntry,
                                     MUIV_List_GetEntry_Active, &father);
                            if (father->id != TY_SUBMENU)
                            {
                                father = father->father;
                            }
                            if (tmp_menu =
                                AllocVec(sizeof(menu),
                                         MEMF_PUBLIC | MEMF_CLEAR))
                            {
                                InitMenu(tmp_menu);
                                tmp_menu->id = TY_MENUITEM;
                                tmp_menu->father = father;
                                strcpy(tmp_menu->name, GetStr(STR_Entry));
                                strcpy(tmp_menu->label, menu_aux->label);
                                strncat(tmp_menu->label, tmp_menu->name,
                                        80);
                                StringToLabel(tmp_menu->label,
                                              tmp_menu->label);
                                AddToSubMenu(LV_Menu, father, tmp_menu);
                                add(father->childs, tmp_menu);
                                set(WI_Menu, MUIA_Window_ActiveObject,
                                    STR_label2);
                            }
                        }
                        else
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_SelectSubMenu));
                        break;
                }
                set(STR_Entry, MUIA_Disabled, TRUE);
                break;
            case ID_NEW_MENUTITLE:
                action = ID_NEW_MENUTITLE;
                set(STR_Entry, MUIA_String_Contents, "");
                set(STR_Entry, MUIA_Disabled, FALSE);
                set(WI_Menu, MUIA_Window_ActiveObject, STR_Entry);
                break;
            case ID_NEW_SUBMENU:
                get(LV_Menu, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    action = ID_NEW_SUBMENU;
                    set(STR_Entry, MUIA_String_Contents, "");
                    set(STR_Entry, MUIA_Disabled, FALSE);
                    set(WI_Menu, MUIA_Window_ActiveObject, STR_Entry);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectSubMenu));
                break;
            case ID_NEW_MENUITEM:
                get(LV_Menu, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    action = ID_NEW_MENUITEM;
                    set(STR_Entry, MUIA_String_Contents, "");
                    set(STR_Entry, MUIA_Disabled, FALSE);
                    set(WI_Menu, MUIA_Window_ActiveObject, STR_Entry);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectSubMenu));
                break;
            case ID_NEWBARLABEL:
                get(LV_Menu, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    action = ID_NEW_MENUITEM;
                    sprintf(nospace, "BarLabel%d", nb_barlabel++);
                    set(STR_Entry, MUIA_String_Contents, nospace);
                    set(STR_Entry, MUIA_Disabled, TRUE);
                    DoMethod(app, MUIM_Application_ReturnID, ID_ACTION);
                    set(WI_Menu, MUIA_Window_ActiveObject, STR_label2);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectSubMenu));
                break;
            case ID_DELETE:
                get(LV_Menu, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(LV_Menu, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &tmp_menu);
                    set(LV_Menu, MUIA_List_Quiet, TRUE);
                    DeleteMenuEntry(LV_Menu, tmp_menu, active);
                    set(LV_Menu, MUIA_List_Quiet, FALSE);
                    father = tmp_menu->father;
                    for (i = 0; i < father->childs->nb_elements; i++)
                    {
                        if (tmp_menu == nth(father->childs, i))
                            break;
                    }
                    delete_nth(father->childs, i);
                    DeleteObject(tmp_menu);
                }
                break;
            case ID_UP:
                get(LV_Menu, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(LV_Menu, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &tmp_menu);
                    father = tmp_menu->father;
                    for (i = 0; i < father->childs->nb_elements; i++)
                    {
                        if (tmp_menu == nth(father->childs, i))
                            break;
                    }
                    if (i == 0)
                        break;
                    down(father->childs, i - 1);
                    set(LV_Menu, MUIA_List_Quiet, TRUE);
                    DeleteMenuEntry(LV_Menu, tmp_menu, active);
                    get(LV_Menu, MUIA_List_Entries, &n);
                    father = nth(father->childs, i);
                    for (i = 0; i < n; i++)
                    {
                        DoMethod(LV_Menu, MUIM_List_GetEntry, i,
                                 &tmp_menu2);
                        if (father == tmp_menu2)
                            break;
                    }
                    DisplayMenuEntry(LV_Menu, tmp_menu, i);
                    set(LV_Menu, MUIA_List_Active, i);
                    set(LV_Menu, MUIA_List_Quiet, FALSE);
                }
                break;
            case ID_DOWN:
                get(LV_Menu, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(LV_Menu, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &tmp_menu);
                    father = tmp_menu->father;
                    for (i = 0; i < father->childs->nb_elements; i++)
                    {
                        if (tmp_menu == nth(father->childs, i))
                            break;
                    }
                    if (i == father->childs->nb_elements - 1)
                        break;
                    up(father->childs, i + 1);
                    set(LV_Menu, MUIA_List_Quiet, TRUE);
                    DeleteMenuEntry(LV_Menu, tmp_menu, active);
                    get(LV_Menu, MUIA_List_Entries, &n);
                    father = nth(father->childs, i);
                    for (i = 0; i < n; i++)
                    {
                        DoMethod(LV_Menu, MUIM_List_GetEntry, i,
                                 &tmp_menu2);
                        if (father == tmp_menu2)
                            break;
                    }
                    for (j = i + 1; j < n; j++)
                    {
                        DoMethod(LV_Menu, MUIM_List_GetEntry, j,
                                 &tmp_menu2);
                        if (tmp_menu2)
                        {
                            if (!IsMenuParent(father, tmp_menu2))
                                break;
                        }
                        else
                            break;
                    }
                    DisplayMenuEntry(LV_Menu, tmp_menu, j);
                    set(LV_Menu, MUIA_List_Active, j);
                    set(LV_Menu, MUIA_List_Quiet, FALSE);
                }
                break;
            case ID_NOTIFY:
                get(LV_Menu, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(LV_Menu, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &tmp_menu);
                    CreateNotify((object *) tmp_menu, NULL);
                    WI_current = WI_Menu;
                }
                break;
            case ID_OKWIN:
                aux = GetStr(STR_Label);
                if (strlen(aux) > 0)
                {
                    strcpy(menu_aux->label, aux);
                    result = TRUE;
                    running = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedLabel));
                break;
            case ID_END:
                running = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if ((signal) && (running))
            Wait(signal);
    }
    set(WI_Menu, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_Menu);
    MUI_DisposeObject(WI_Menu);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
