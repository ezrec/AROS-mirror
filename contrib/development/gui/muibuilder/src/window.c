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
#include "muibuilder_rev.h"
#include <utility/hooks.h>

APTR testwin = NULL;

void Toggle(object *obj, APTR list, int position);
int AfficheObjet(APTR list, object *obj, int position);

void TestQuick(window *win)
{
    if (MakeTest)
    {
        if (testwin)
        {
            set(testwin, MUIA_Window_Open, FALSE);
            DoMethod(app, OM_REMMEMBER, testwin);
            MUI_DisposeObject(testwin);
            testwin = NULL;
        }
        if (win->root.child->nb_elements > 0)
        {
            testwin = CreateWindow(win);
            DoMethod(testwin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                     testwin, 3, MUIM_Set, MUIA_Window_Open, FALSE);
            DoMethod(app, OM_ADDMEMBER, testwin);
            set(testwin, MUIA_Window_Open, TRUE);
        }
    }
}

void InitWindow(window *win_aux)
{
    strcpy(win_aux->title, "window_title");
    sprintf(win_aux->label, "WI_label_%d", nb_window);
    win_aux->appwindow = FALSE;
    win_aux->backdrop = FALSE;
    win_aux->borderless = FALSE;
    win_aux->closegadget = TRUE;
    win_aux->depthgadget = TRUE;
    win_aux->dragbar = TRUE;
    win_aux->sizegadget = TRUE;
    win_aux->initopen = TRUE;
    win_aux->nomenu = FALSE;
    win_aux->needmouse = FALSE;
    win_aux->notify = create();
    win_aux->notifysource = create();
    win_aux->chain = create();
    win_aux->menu = CreateMenu(TRUE);;
    win_aux->Help.nb_char = 0;
    win_aux->Help.title[0] = '\0';
    win_aux->Help.content = NULL;
    win_aux->father = &application;
}

APTR LoadWindow(FILE *fichier, int version)
{
    window *win_aux;
    group *group_aux;
    int i;

    win_aux = AllocVec(sizeof(window), MEMF_PUBLIC | MEMF_CLEAR);
    InitWindow(win_aux);
    win_aux->root.child = create();
    win_aux->groups = create();
    win_aux->root.entries = create();
    win_aux->root.notify = create();
    win_aux->root.notifysource = create();
    win_aux->id = TY_WINDOW;
    win_aux->root.deplie = TRUE;
    current_window = win_aux;
    strcpy(win_aux->label, LitChaine(fichier));
    win_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, win_aux);
    if (version > 104)
        ReadNotify(fichier, win_aux);
    strcpy(win_aux->title, LitChaine(fichier));
    if (version > 101)
    {
        win_aux->appwindow = ReadInt(fichier);
        win_aux->backdrop = ReadInt(fichier);
        win_aux->borderless = ReadInt(fichier);
        win_aux->closegadget = ReadInt(fichier);
        win_aux->depthgadget = ReadInt(fichier);
        win_aux->dragbar = ReadInt(fichier);
        win_aux->sizegadget = ReadInt(fichier);
    }
    if (version > 104)
        win_aux->initopen = ReadInt(fichier);
    if (version > 123)
        win_aux->nomenu = ReadInt(fichier);
    if (version > 125)
        win_aux->needmouse = ReadInt(fichier);
    if (version >= 114)
    {
        i = ReadInt(fichier);
        win_aux->menu = LoadObject(fichier, win_aux, i);
    }

    add(current_window->groups, &win_aux->root);
    group_aux = LoadObject(fichier, win_aux, ReadInt(fichier));
    CopyObject((object *) group_aux, (object *) &win_aux->root,
               sizeof(group), group_aux->father);
    if (strlen(group_aux->label) > 0)
         win_aux->root.label[strlen(group_aux->label)] = '\0';
    FreeVec(win_aux->root.notify);
    FreeVec(win_aux->root.notifysource);
    win_aux->root.notify = group_aux->notify;
    win_aux->root.notifysource = group_aux->notifysource;
    win_aux->root.entries = group_aux->entries;
    win_aux->root.child = group_aux->child;
    win_aux->root.father = group_aux->father;

    for (i = 0; i <= win_aux->root.entries->nb_elements; i++)
    {
        win_aux->root.registertitles[i] = group_aux->registertitles[i];
    }
    FreeVec(group_aux);
    CreateChain((object *) win_aux, win_aux);
    return (win_aux);
}

void SaveWindow(FILE *fichier, window *win_aux)
{
    fprintf(fichier, "%s\n", win_aux->title);
    fprintf(fichier, "%d\n", win_aux->appwindow);
    fprintf(fichier, "%d\n", win_aux->backdrop);
    fprintf(fichier, "%d\n", win_aux->borderless);
    fprintf(fichier, "%d\n", win_aux->closegadget);
    fprintf(fichier, "%d\n", win_aux->depthgadget);
    fprintf(fichier, "%d\n", win_aux->dragbar);
    fprintf(fichier, "%d\n", win_aux->sizegadget);
    fprintf(fichier, "%d\n", win_aux->initopen);
    fprintf(fichier, "%d\n", win_aux->nomenu);
    fprintf(fichier, "%d\n", win_aux->needmouse);
    SaveObject(win_aux->menu, fichier);
    SaveObject(&win_aux->root, fichier);
}

void CreateChain(object *obj, window *win)
{
    int i, n;
    group *group_aux;

    switch (obj->id)
    {
        case TY_WINDOW:
            n = win->chain->nb_elements;
            for (i = 0; i < n; i++)
            {
                delete_nth(win->chain, 0);
            }
            CreateChain((object *) &win->root, win);
            break;
        case TY_GROUP:
            group_aux = (group *) obj;
            if (group_aux->generated)
                add(win->chain, obj);
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                CreateChain(nth(group_aux->child, i), win);
            }
            break;
        case TY_TEXT:
        case TY_LABEL:
        case TY_GAUGE:
        case TY_SPACE:
        case TY_SCALE:
            break;
        default:
            if (obj->generated)
                add(win->chain, obj);
            break;
    }
}

void AddToGroup(group *group_aux, APTR list, int position, object *obj)
{
    int pos = position + 1;
    object *obj_aux;

    if (!group_aux->deplie)
    {
        Toggle((object *) group_aux, list, pos);
        DoMethod(list, MUIM_List_Redraw, pos);
    }
    else
    {
        DoMethod(list, MUIM_List_GetEntry, pos, &obj_aux);
        if (obj_aux)
            while (IsParent(obj_aux, group_aux))
            {
                pos++;
                DoMethod(list, MUIM_List_GetEntry, pos, &obj_aux);
                if (!obj_aux)
                    break;
            }
        AfficheObjet(list, obj, pos);
    }
}

void InitList(APTR list, APTR obj, int Depth, int MaxDepth)
{
    object *obj_aux;
    group *group_aux;
    int i;

    obj_aux = obj;
    switch (obj_aux->id)
    {
        case TY_GROUP:
            group_aux = obj;
            if ((Depth < MaxDepth) || (group_aux->deplie))
            {
                group_aux->deplie = TRUE;
                DoMethod(list, MUIM_List_Insert, &group_aux, 1,
                         MUIV_List_Insert_Bottom);
                for (i = 0; i < group_aux->child->nb_elements; i++)
                {
                    InitList(list, nth(group_aux->child, i), Depth + 1,
                             MaxDepth);
                }
            }
            else
                DoMethod(list, MUIM_List_Insert, &group_aux, 1,
                         MUIV_List_Insert_Bottom);
            break;
        default:
            get(list, MUIA_List_Entries, &i);
            AfficheObjet(list, obj_aux, i);
            break;
    }
}

void ForceFold(APTR obj, BOOL force)
{
    object *obj_aux;
    group *group_aux;
    int i;

    obj_aux = obj;
    switch (obj_aux->id)
    {
        case TY_GROUP:
            group_aux = obj;
            group_aux->deplie = force;
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                ForceFold(nth(group_aux->child, i), force);
            }
            break;
        default:
            break;
    }
}

BOOL IsParent(object *obj, group *parent)
{
    if (obj->id == TY_WINDOW)
        return (FALSE);
    if (obj->father == parent)
        return (TRUE);
    else
        return (IsParent(obj->father, parent));
}

/* display object in list */
int AfficheObjet(APTR list, object *obj, int position)
{
    group *group_aux;
    popobject *popobj_aux;
    int i;
    int pos = position;

    DoMethod(list, MUIM_List_Insert, &obj, 1, pos++);
    if (obj->id == TY_POPOBJECT)
    {
        popobj_aux = (popobject *) obj;
        pos = AfficheObjet(list, popobj_aux->obj, pos);
    }
    if (obj->id == TY_GROUP)
    {
        group_aux = (group *) obj;
        if (group_aux->deplie)
        {
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                pos = AfficheObjet(list, nth(group_aux->child, i), pos);
            }
        }
    }
    return (pos);
}

void EffaceObjet(APTR list, object *obj, int position)
{
    group *group_aux;
    popobject *popobj_aux;
    BOOL bool_aux;

    switch (obj->id)
    {
        case TY_GROUP:
            group_aux = (group *) obj;
            bool_aux = group_aux->deplie;
            if (group_aux->deplie)
                Toggle(obj, list, position + 1);
            group_aux->deplie = bool_aux;
            DoMethod(list, MUIM_List_Remove, position);
            break;
        case TY_POPOBJECT:
            popobj_aux = (popobject *) obj;
            DoMethod(list, MUIM_List_Remove, position);
            EffaceObjet(list, popobj_aux->obj, position);
            break;
        default:
            DoMethod(list, MUIM_List_Remove, position);
    }
}

void Toggle(object *obj, APTR list, int position)
{
    object *obj_aux;
    group *group_aux;
    int i;
    int pos = position;

    if (obj->id != TY_GROUP)
        return;

    group_aux = (group *) obj;
    group_aux->deplie = !group_aux->deplie;
    if (group_aux->deplie)
        for (i = 0; i < group_aux->child->nb_elements; i++)
        {
            obj_aux = nth(group_aux->child, i);
            pos = AfficheObjet(list, obj_aux, pos);
        }
    else
    {
        DoMethod(list, MUIM_List_GetEntry, position, &obj_aux);
        if (obj_aux)
            while (IsParent(obj_aux, group_aux))
            {
                DoMethod(list, MUIM_List_Remove, position);
                DoMethod(list, MUIM_List_GetEntry, position, &obj_aux);
                if (!obj_aux)
                    break;
            }
    }
}

void EditObject(APTR WI_window, APTR lv_group, window *win_aux)
{
    BOOL bool_aux;
    object *child_aux;

    DoMethod(lv_group, MUIM_List_GetEntry, MUIV_List_GetEntry_Active,
             &child_aux);
    if (child_aux)
    {
        if (config.phantom)
            set(WI_window, MUIA_Window_Open, FALSE);
        bool_aux = EditChild(child_aux);
        if (config.phantom)
            set(WI_window, MUIA_Window_Open, TRUE);
        WI_current = WI_window;
        if (bool_aux)
        {
            TestQuick(win_aux);
            ShowObject(lv_group);
        }
    }
    else
        ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
}

int SearchObject(APTR list, object *obj)
{
    int i;
    int nb;
    object *obj_aux;

    get(list, MUIA_List_Entries, &nb);
    for (i = 0; i < nb; i++)
    {
        DoMethod(list, MUIM_List_GetEntry, i, &obj_aux);
        if (obj == obj_aux)
            break;
    }
    if (i != nb)
        return (i);
    else
        return (-1);
}

void ShowObject(APTR lv_group)
{
    object *child_aux;

    if (MakeTest)
    {
        DoMethod(lv_group, MUIM_List_GetEntry, MUIV_List_GetEntry_Active,
                 &child_aux);
        if ((testwin) && (child_aux))
        {
            DoMethod(testwin, MUIM_Set, MUIA_Window_ActiveObject,
                     child_aux->muiobj);
        }
    }
}

//__asm __saveds LONG DisplayTreeFunc( register __a2 char **array, register __a1 object* obj )
//DisplayTreeFunc(char **array __asm("a2"), object *obj __asm("a1"))
HOOKPROTONH(DisplayTreeFunc, LONG, char **array, object *obj)
{
    static char buffer[10], buffer2[82];
    static char buffer3[2], buffer4[2];
    static char aux[100];
    group *group_aux;
    object *obj_aux;
    char concat[3];

    concat[0] = config.tree_char;
    concat[1] = ' ';
    concat[2] = '\0';
    buffer2[0] = '\0';
    if (obj->generated)
        strcpy(buffer3, "G");
    else
        strcpy(buffer3, " ");
    if (obj->Help.generated)
        strcpy(buffer4, "H");
    else
        strcpy(buffer4, " ");
    obj_aux = obj;
    group_aux = (group *) obj_aux->father;
    while ((obj_aux->id != TY_GROUP) || (group_aux->id == TY_POPOBJECT))
    {
        obj_aux = obj_aux->father;
        group_aux = (group *) obj_aux->father;
        strcat(buffer2, concat);
    }
    group_aux = (group *) obj_aux;
    while (!group_aux->root)
    {
        strcat(buffer2, concat);
        group_aux = group_aux->father;
    }
    if (obj->id == TY_GROUP)
    {
        group_aux = (group *) obj;
        if (!group_aux->deplie)
            strcpy(buffer, "\033I[6:30]");
        else
            strcpy(buffer, "\033I[6:39]");
        sprintf(aux, "\0338%s", obj->label);
        strncat(buffer2, aux, 82);
    }
    else
    {
        buffer[0] = '\0';
        sprintf(aux, "%s", obj->label);
        strncat(buffer2, aux, 82);
    }

    *array++ = buffer3;
    *array++ = buffer4;
    *array++ = buffer;
    *array = buffer2;

    return (0);
}

BOOL NewWindow(window *win_aux, BOOL new, object *obj2search)
{
    APTR WI_window, RegGroup;
    APTR bt_cancelwin, bt_okwin, bt_test, bt_notify, bt_notify2;
    APTR bt_add, bt_rm, bt_up, bt_down, bt_copy, bt_edit;
    APTR bt_totmp, bt_tochild, bt_tmprm;
    APTR bt_copytmp, bt_menu;
    APTR lv_group, lv_tmp;
    APTR STR_label, STR_title;
    CONST_STRPTR aux;
    APTR CH_appwindow, CH_backdrop, CH_borderless, CH_closegadget;
    APTR CH_depthgadget, CH_dragbar, CH_sizegadget, CH_initopen, CH_nomenu;
    APTR CH_needmouse;

    int active, i, n;
    char *chaine_aux;
    object *child_aux, *child2_aux, *obj_aux;
    group *group_aux;
    queue *undo;
    char LVT_Info[512];

    ULONG signal;
    BOOL running = TRUE;
    BOOL result = FALSE;
    BOOL bool_aux;
    CONST_STRPTR registertitles[3];

    MakeStaticHook(DisplayHook, DisplayTreeFunc);

    struct NewMenu Menu[] = {
        {NM_TITLE, "", "", 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_CONFIG},
        {NM_ITEM, NM_BARLABEL, 0, 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_SAVE},
        {NM_ITEM, NM_BARLABEL, 0, 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_INFO},
        {NM_TITLE, "", "", 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_UNDO},
        {NM_TITLE, "", "", 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_FOLD},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_UNFOLD},
        {NM_END, NULL, 0, 0, 0, (APTR) 0},
    };

    registertitles[0] = GetMUIBuilderString(MSG_Creation);
    registertitles[1] = GetMUIBuilderString(MSG_Attributes);
    registertitles[2] = NULL;

    Menu[0].nm_Label = GetMUIBuilderString(MSG_Project);
    Menu[1].nm_Label = GetMenuString(MSG_Config);
    Menu[1].nm_CommKey = GetMenuChar(MSG_Config);
    Menu[3].nm_Label = GetMenuString(MSG_SaveMenu);
    Menu[3].nm_CommKey = GetMenuChar(MSG_SaveMenu);
    Menu[5].nm_Label = GetMenuString(MSG_About);
    Menu[5].nm_CommKey = GetMenuChar(MSG_About);
    Menu[6].nm_Label = GetMUIBuilderString(MSG_EditMenu);
    Menu[7].nm_Label = GetMenuString(MSG_Undo);
    Menu[7].nm_CommKey = GetMenuChar(MSG_Undo);
    Menu[8].nm_Label = GetMUIBuilderString(MSG_Tree);
    Menu[9].nm_Label = GetMenuString(MSG_Fold);
    Menu[9].nm_CommKey = GetMenuChar(MSG_Fold);
    Menu[10].nm_Label = GetMenuString(MSG_Unfold);
    Menu[10].nm_CommKey = GetMenuChar(MSG_Unfold);

    sprintf(LVT_Info, GetMUIBuilderString(MSG_AboutText), VERS, DATE);

    if (new)
    {
        InitWindow(win_aux);
        add(windows, win_aux);
    }

    undo = create();

    // *INDENT-OFF*
    WI_window = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_WindowAttributes),
        MUIA_HelpNode, "Window",
        MUIA_Window_ID, MAKE_ID('W', 'I', 'N', 'D'),
        MUIA_Window_Menustrip, MUI_MakeObject(MUIO_MenustripNM, Menu),
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(registertitles),
                Child, VGroup,
                    Child, HGroup,
                        Child, ColGroup(2),
                            GroupFrameT(GetMUIBuilderString(MSG_Objects)),
                            Child, VGroup,
                                Child, lv_group = ListviewObject,
                                    MUIA_Listview_DefClickColumn, 2,
                                    MUIA_Listview_List, ListObject,
                                        InputListFrame,
                                        MUIA_List_Format, "COL=0 P=\033C DELTA=1, COL=1 P=\033C DELTA=8, COL=2 P=\033C DELTA=1, COL=3 DELTA=8",
                                        MUIA_List_DisplayHook, &DisplayHook,
                                    End,
                                End,
                                Child, bt_notify = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Notify)),
                                End,
                            Child, VGroup,
                            MUIA_Weight, 0,
                            Child, bt_add = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_AddChild)),
                            Child, bt_up = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Up)),
                            Child, bt_down = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Down)),
                            Child, bt_rm = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_RemoveChild)),
                            Child, HVSpace,
                            Child, bt_edit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_EditObject)),
                            Child, HVSpace,
                            Child, bt_copy = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Copy)),
                            Child, bt_copytmp = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_CopyTmp)),
                            Child, bt_totmp = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_ToTmp)),
                            Child, bt_tochild = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_ToChild)),
                        End,
                    End,
                    Child, VGroup,
                        GroupFrameT(GetMUIBuilderString(MSG_Tmp)),
                        MUIA_HorizWeight, 50,
                        Child, lv_tmp = ListviewObject,
                            MUIA_Listview_List, ListObject,
                                InputListFrame,
                            End,
                        End,
                        Child, bt_tmprm = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_RemoveTmp)),
                    End,
                End,
            End,
            Child, VGroup,
                Child, ColGroup(2),
                    GroupFrameT(GetMUIBuilderString(MSG_WindowIdentification)),
                    Child, Label2(GetMUIBuilderString(MSG_Label)),
                    Child, STR_label = StringObject,
                        StringFrame,
                        MUIA_String_Contents, win_aux->label,
                        MUIA_ExportID, 1,
                    End,
                    Child, Label2(GetMUIBuilderString(MSG_Title)),
                        Child, STR_title = StringObject,
                            StringFrame,
                            MUIA_String_Contents, win_aux->title,
                            MUIA_ExportID, 2,
                        End,
                    End,
                    Child, HGroup,
                        Child, HSpace(0),
                        Child, ColGroup(4),
                            GroupFrameT(GetMUIBuilderString(MSG_Attributes)),
                            Child, Label1(GetMUIBuilderString(MSG_AppWindow)),
                            Child, CH_appwindow = CheckMark(win_aux->appwindow),
                            Child, Label1(GetMUIBuilderString(MSG_Backdrop)),
                            Child, CH_backdrop = CheckMark(win_aux->backdrop),
                            Child, Label1(GetMUIBuilderString(MSG_Borderless)),
                            Child, CH_borderless = CheckMark(win_aux->borderless),
                            Child, Label1(GetMUIBuilderString(MSG_CloseGadget)),
                            Child, CH_closegadget = CheckMark(win_aux->closegadget),
                            Child, Label1(GetMUIBuilderString(MSG_DepthGadget)),
                            Child, CH_depthgadget = CheckMark(win_aux->depthgadget),
                            Child, Label1(GetMUIBuilderString(MSG_DragBar)),
                            Child, CH_dragbar = CheckMark(win_aux->dragbar),
                            Child, Label1(GetMUIBuilderString(MSG_SizeGadget)),
                            Child, CH_sizegadget = CheckMark(win_aux->sizegadget),
                            Child, Label1(GetMUIBuilderString(MSG_OpenAtInit)),
                            Child, CH_initopen = CheckMark(win_aux->initopen),
                            Child, Label1(GetMUIBuilderString(MSG_NoMenu)),
                            Child, CH_nomenu = CheckMark(win_aux->nomenu),
                            Child, Label1(GetMUIBuilderString(MSG_NeedMouse)),
                            Child, CH_needmouse = CheckMark(win_aux->needmouse),
                        End,
                        Child, HSpace(0),
                    End,
                    Child, HGroup,
                        MUIA_Group_SameWidth, TRUE,
                        Child, bt_notify2 = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Notify)),
                        Child, bt_menu = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Menu)),
                    End,
                End,
            End,
            Child, HGroup,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_okwin = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_test = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Test)),
                Child, bt_cancelwin = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_window;

    DoMethod(bt_okwin, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_notify, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NOTIFY);
    DoMethod(bt_notify2, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NOTIFY);
    DoMethod(bt_menu, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEW_SUBMENU);
    DoMethod(bt_cancelwin, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(bt_test, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_TEST);
    DoMethod(bt_rm, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DELETE);
    DoMethod(bt_add, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(bt_up, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_UP);
    DoMethod(bt_down, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DOWN);
    DoMethod(bt_totmp, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_TOTMP);
    DoMethod(bt_tochild, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_TOCHILD);
    DoMethod(bt_tmprm, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_TMPRM);
    DoMethod(bt_copy, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_COPY);
    DoMethod(bt_copytmp, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_COPYTMP);
    DoMethod(WI_window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(STR_title, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_window, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_window, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_title);
    DoMethod(lv_group, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK);
    DoMethod(bt_edit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_EDIT);
    DoMethod(lv_group, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, app,
             2, MUIM_Application_ReturnID, ID_UPDATE);
    DoMethod(CH_nomenu, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
             bt_menu, 3, MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

    DoMethod(WI_window, MUIM_Window_SetCycleChain, RegGroup, lv_group,
             lv_tmp, bt_notify, bt_add, bt_up, bt_down, bt_rm, bt_edit,
             bt_copy, bt_copytmp, bt_totmp, bt_tochild, bt_tmprm, bt_okwin,
             bt_test, bt_cancelwin, STR_label, STR_title, CH_appwindow,
             CH_backdrop, CH_borderless, CH_closegadget, CH_depthgadget,
             CH_dragbar, CH_sizegadget, CH_initopen, CH_nomenu, bt_notify2,
             bt_menu, NULL);

    if (!new)
    {
        set(bt_cancelwin, MUIA_Disabled, TRUE);
    }

    /* initialisation des listes */
    for (i = 0; i < tmp->nb_elements; i++)
    {
        child_aux = nth(tmp, i);
        chaine_aux = (char *) (child_aux->label);
        DoMethod(lv_tmp, MUIM_List_Insert, &chaine_aux, 1,
                 MUIV_List_Insert_Bottom);
    }

    if (obj2search)
        i = 1000;
    else
        i = config.depth;
    InitList(lv_group, &win_aux->root, 0, i);
    if (obj2search)
    {
        get(lv_group, MUIA_List_Entries, &n);
        for (i = 0; i < n; i++)
        {
            DoMethod(lv_group, MUIM_List_GetEntry, i, &child_aux);
            if (obj2search == child_aux)
            {
                set(lv_group, MUIA_List_Active, i);
                break;
            }
        }
    }
    else
        set(lv_group, MUIA_List_Active, MUIV_List_Active_Top);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_window);
    if (!new)
        TestQuick(win_aux);
    set(WI_window, MUIA_Window_Open, TRUE);
    set(WI_window, MUIA_Window_ActiveObject, lv_group);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_UPDATE:
                ShowObject(lv_group);
                break;
            case ID_COPY:
                DoMethod(lv_group, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &child_aux);
                if (child_aux)
                {
                    obj_aux = Duplicate(child_aux, NULL);
                    if (obj_aux)
                    {
                        if (obj_aux->id == TY_GROUP)
                        {
                            group_aux = (group *) obj_aux;
                            group_aux->root = FALSE;
                        }
                        add(tmp, obj_aux);
                        chaine_aux = (char *) (obj_aux->label);
                        set(lv_tmp, MUIA_List_Quiet, TRUE);
                        DoMethod(lv_tmp, MUIM_List_Insert, &chaine_aux, 1,
                                 MUIV_List_Insert_Bottom);
                        set(lv_tmp, MUIA_List_Quiet, FALSE);
                    }
                    else
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_NotEnoughMemory));
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_COPYTMP:
                get(lv_tmp, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(lv_group, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &obj_aux);
                    if (obj_aux)
                    {
                        child_aux = nth(tmp, active);
                        get(lv_group, MUIA_List_Active, &active);
                        if (obj_aux->id != TY_GROUP)
                        {
                            obj_aux = obj_aux->father;
                            while (obj_aux->id != TY_GROUP)
                            {
                                obj_aux = obj_aux->father;
                            }
                            active = SearchObject(lv_group, obj_aux);

                        }
                        group_aux = (group *) obj_aux;
                        obj_aux = Duplicate(child_aux, group_aux);
                        if (obj_aux)
                        {
                            add(group_aux->child, obj_aux);
                            set(lv_group, MUIA_List_Quiet, TRUE);
                            AddToGroup(group_aux, lv_group, active,
                                       obj_aux);
                            set(lv_group, MUIA_List_Quiet, FALSE);
                            WI_current = WI_window;
                            TestQuick(win_aux);
                            ShowObject(lv_group);
                        }
                        else
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_NotEnoughMemory));
                    }
                    else
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_SelectObject));

                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectTmpObject));
                break;
            case ID_NOTIFY:
                get(RegGroup, MUIA_Group_ActivePage, &active);
                if (active == 0)
                {
                    get(lv_group, MUIA_List_Active, &active);
                    if (active != MUIV_List_Active_Off)
                    {
                        DoMethod(lv_group, MUIM_List_GetEntry,
                                 MUIV_List_GetEntry_Active, &child_aux);
                        CreateNotify(child_aux, ParentWindow(child_aux));
                        WI_current = WI_window;
                    }
                    else
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_SelectObject));
                }
                else
                {
                    CreateNotify((object *) win_aux, win_aux);
                    WI_current = WI_window;
                }
                break;
            case ID_TOTMP:
                get(lv_group, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(lv_group, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &child_aux);
                    obj_aux = child_aux->father;
                    if (obj_aux->id == TY_POPOBJECT)
                    {
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_CantMovePopObjectChild));
                        break;
                    }
                    if (child_aux->id == TY_GROUP)
                    {
                        group_aux = (group *) child_aux;
                        if (group_aux->root)
                        {
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_CantMoveRoot));
                            break;
                        }
                    }
                    add(tmp, child_aux);
                    chaine_aux = child_aux->label;
                    DoMethod(lv_tmp, MUIM_List_Insert, &chaine_aux, 1,
                             MUIV_List_Insert_Bottom);
                    /* on recherche l'objet a effacer chez son pere */
                    group_aux = child_aux->father;
                    for (i = 0; i < group_aux->child->nb_elements; i++)
                    {
                        if (child_aux == nth(group_aux->child, i))
                            break;
                    }
                    delete_nth(group_aux->child, i);
                    set(lv_group, MUIA_List_Quiet, TRUE);
                    EffaceObjet(lv_group, child_aux, active);
                    set(lv_group, MUIA_List_Quiet, FALSE);
                    WI_current = WI_window;
                    TestQuick(win_aux);
                    ShowObject(lv_group);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_TOCHILD:
                get(lv_tmp, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(lv_group, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &obj_aux);
                    if (obj_aux)
                    {
                        DoMethod(lv_tmp, MUIM_List_Remove, active);
                        child_aux = nth(tmp, active);
                        delete_nth(tmp, active);
                        get(lv_group, MUIA_List_Active, &active);
                        if (obj_aux->id != TY_GROUP)
                        {
                            do
                                obj_aux = obj_aux->father;
                            while (obj_aux->id != TY_GROUP);
                            active = SearchObject(lv_group, obj_aux);

                        }
                        group_aux = (group *) obj_aux;
                        add(group_aux->child, child_aux);
                        child_aux->father = group_aux;
                        set(lv_group, MUIA_List_Quiet, TRUE);
                        AddToGroup(group_aux, lv_group, active, child_aux);
                        set(lv_group, MUIA_List_Quiet, FALSE);
                        WI_current = WI_window;
                        TestQuick(win_aux);
                        ShowObject(lv_group);
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectTmpObject));
                break;
            case ID_UP:
                bool_aux = FALSE;
                DoMethod(lv_group, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &obj_aux);
                if (obj_aux)
                {
                    get(lv_group, MUIA_List_Active, &active);
                    if (obj_aux->id == TY_GROUP)
                    {
                        group_aux = (group *) obj_aux;
                        if (group_aux->root)
                            break;
                    }
                    group_aux = obj_aux->father;
                    if (group_aux->id == TY_POPOBJECT)
                    {
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_CantMovePopObjectChild));
                        break;
                    }
                    /* recherche de la position i de l'objet dans le groupe pere */
                    for (i = 0; i < group_aux->child->nb_elements; i++)
                    {
                        if (obj_aux == nth(group_aux->child, i))
                            break;
                    }
                    if (i == 0)
                        break;  /* on ne fait rien s'il est en tete de groupe */
                    set(lv_group, MUIA_List_Quiet, TRUE);
                    EffaceObjet(lv_group, obj_aux, active);
                    up(group_aux->child, i);
                    child_aux = nth(group_aux->child, i);
                    /* recherche du frere precedent */
                    for (i = active - 1; i > 0; i--)
                    {
                        DoMethod(lv_group, MUIM_List_GetEntry, i,
                                 &child2_aux);
                        if (child_aux == child2_aux)
                            break;
                    }
                    AfficheObjet(lv_group, obj_aux, i);
                    if (bool_aux)
                        Toggle(obj_aux, lv_group, i + 1);
                    set(lv_group, MUIA_List_Active, i);
                    set(lv_group, MUIA_List_Quiet, FALSE);
                    WI_current = WI_window;
                    TestQuick(win_aux);
                    ShowObject(lv_group);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_DOWN:
                bool_aux = FALSE;
                DoMethod(lv_group, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &obj_aux);
                if (obj_aux)
                {
                    get(lv_group, MUIA_List_Active, &active);
                    if (obj_aux->id == TY_GROUP)
                    {
                        group_aux = (group *) obj_aux;
                        if (group_aux->root)
                            break;
                    }
                    group_aux = obj_aux->father;
                    if (group_aux->id == TY_POPOBJECT)
                    {
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_CantMovePopObjectChild));
                        break;
                    }
                    /* recherche de la position i de l'objet dans le groupe pere */
                    for (i = 0; i < group_aux->child->nb_elements; i++)
                    {
                        if (obj_aux == nth(group_aux->child, i))
                            break;
                    }
                    /* on ne fait rien s'il est en queue de groupe */
                    if (i == group_aux->child->nb_elements - 1)
                        break;
                    set(lv_group, MUIA_List_Quiet, TRUE);
                    EffaceObjet(lv_group, obj_aux, active);
                    down(group_aux->child, i);
                    child_aux = nth(group_aux->child, i);
                    get(lv_group, MUIA_List_Entries, &n);
                    for (i = active + 1; i < n; i++)
                    {
                        DoMethod(lv_group, MUIM_List_GetEntry, i,
                                 &child2_aux);
                        if (!IsParent(child2_aux, (group *) child_aux))
                            break;
                    }
                    AfficheObjet(lv_group, obj_aux, i);
                    if (bool_aux)
                        Toggle(obj_aux, lv_group, i + 1);
                    set(lv_group, MUIA_List_Active, i);
                    set(lv_group, MUIA_List_Quiet, FALSE);
                    WI_current = WI_window;
                    TestQuick(win_aux);
                    ShowObject(lv_group);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_DELETE:
                bool_aux = TRUE;
                DoMethod(lv_group, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &obj_aux);
                if (config.request)
                    bool_aux =
                        RequestMessage(GetMUIBuilderString(MSG_Confirm));
                if ((obj_aux) && bool_aux)
                {
                    get(lv_group, MUIA_List_Active, &active);
                    child_aux = obj_aux->father;
                    if (child_aux->id == TY_POPOBJECT)
                    {
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_CantDeletePopObjectChild));
                        break;
                    }
                    if (obj_aux->id == TY_GROUP)
                    {
                        group_aux = (group *) obj_aux;
                        if (group_aux->root)
                        {
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_RemoveRootGroup));
                            break;
                        }
                    }
                    set(lv_group, MUIA_List_Quiet, TRUE);
                    EffaceObjet(lv_group, obj_aux, active);
                    set(lv_group, MUIA_List_Quiet, FALSE);
                    group_aux = obj_aux->father;
                    for (i = 0; i < group_aux->child->nb_elements; i++)
                    {
                        if (obj_aux == nth(group_aux->child, i))
                            break;
                    }
                    add(undo, obj_aux);
                    delete_nth(group_aux->child, i);
                    WI_current = WI_window;
                    TestQuick(win_aux);
                    ShowObject(lv_group);
                }
                else if (bool_aux)
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_TMPRM:
                get(lv_tmp, MUIA_List_Active, &active);
                bool_aux = TRUE;
                if (config.request)
                    bool_aux =
                        RequestMessage(GetMUIBuilderString(MSG_Confirm));
                if (bool_aux)
                {
                    if ((active != MUIV_List_Active_Off))
                    {
                        child_aux = nth(tmp, active);
                        add(undo, child_aux);
                        delete_nth(tmp, active);
                        DoMethod(lv_tmp, MUIM_List_Remove, active);
                    }
                    else
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_SelectTmpObject));
                }
                break;
            case ID_NEWOBJ:
                child_aux = NULL;
                get(lv_group, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    DoMethod(lv_group, MUIM_List_GetEntry,
                             MUIV_List_GetEntry_Active, &obj_aux);
                    if (obj_aux->id != TY_GROUP)
                    {
                        obj_aux = obj_aux->father;
                        while (obj_aux->id != TY_GROUP)
                            obj_aux = obj_aux->father;
                        active = SearchObject(lv_group, obj_aux);
                    }
                    if (config.phantom)
                        set(WI_window, MUIA_Window_Open, FALSE);
                    if (child_aux = NewChild(child_aux, TRUE))
                    {
                        group_aux = (group *) obj_aux;
                        child_aux->father = group_aux;
                        add(group_aux->child, child_aux);
                        set(lv_group, MUIA_List_Quiet, TRUE);
                        AddToGroup(group_aux, lv_group, active, child_aux);
                        set(lv_group, MUIA_List_Quiet, FALSE);
                        WI_current = WI_window;
                        TestQuick(win_aux);
                        ShowObject(lv_group);
                    }
                    if (config.phantom)
                        set(WI_window, MUIA_Window_Open, TRUE);
                    WI_current = WI_window;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_DBCLICK:
                get(lv_group, MUIA_Listview_ClickColumn, &active);
                DoMethod(lv_group, MUIM_List_GetEntry,
                         MUIV_List_GetEntry_Active, &child_aux);
                if (!child_aux)
                    break;
                if (active == 2)
                {
                    if (child_aux->id == TY_GROUP)
                    {
                        set(lv_group, MUIA_List_Quiet, TRUE);
                        get(lv_group, MUIA_List_Active, &active);
                        Toggle(child_aux, lv_group, active + 1);
                        set(lv_group, MUIA_List_Quiet, FALSE);
                        break;
                    }
                    else
                    {
                        EditObject(WI_window, lv_group, win_aux);
                        DoMethod(lv_group, MUIM_List_Redraw,
                                 MUIV_List_Redraw_Active);
                        break;
                    }
                }
                if (active == 0)
                {
                    child_aux->generated = !child_aux->generated;
                    DoMethod(lv_group, MUIM_List_Redraw,
                             MUIV_List_Redraw_Active);
                    break;
                }
                if (active == 1)
                {
                    child_aux->Help.generated = !child_aux->Help.generated;
                    DoMethod(lv_group, MUIM_List_Redraw,
                             MUIV_List_Redraw_Active);
                    break;
                }
                EditObject(WI_window, lv_group, win_aux);
                DoMethod(lv_group, MUIM_List_Redraw,
                         MUIV_List_Redraw_Active);
                break;
            case ID_EDIT:
                EditObject(WI_window, lv_group, win_aux);
                DoMethod(lv_group, MUIM_List_Redraw,
                         MUIV_List_Redraw_Active);
                break;
            case ID_OKWIN:
                if (win_aux->root.child->nb_elements > 0)
                {
                    aux = GetStr(STR_label);
                    strcpy(win_aux->label, aux);
                    aux = GetStr(STR_title);
                    strcpy(win_aux->title, aux);
                    get(CH_appwindow, MUIA_Selected, &active);
                    win_aux->appwindow = (active == 1);
                    get(CH_backdrop, MUIA_Selected, &active);
                    win_aux->backdrop = (active == 1);
                    get(CH_borderless, MUIA_Selected, &active);
                    win_aux->borderless = (active == 1);
                    get(CH_closegadget, MUIA_Selected, &active);
                    win_aux->closegadget = (active == 1);
                    get(CH_depthgadget, MUIA_Selected, &active);
                    win_aux->depthgadget = (active == 1);
                    get(CH_dragbar, MUIA_Selected, &active);
                    win_aux->dragbar = (active == 1);
                    get(CH_sizegadget, MUIA_Selected, &active);
                    win_aux->sizegadget = (active == 1);
                    get(CH_initopen, MUIA_Selected, &active);
                    win_aux->initopen = (active == 1);
                    get(CH_nomenu, MUIA_Selected, &active);
                    win_aux->nomenu = (active == 1);
                    get(CH_needmouse, MUIA_Selected, &active);
                    win_aux->needmouse = (active == 1);
                    delete_all(win_aux->chain);
                    CreateChain((object *) win_aux, win_aux);
                    if (new)
                    {
                        nb_window++;
                    }
                    result = TRUE;
                    running = FALSE;
                    set(testwin, MUIA_Window_Open, FALSE);
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_QuitWindowAttributes));
                break;
            case ID_TEST:
                aux = GetStr(STR_title);
                strcpy(win_aux->title, aux);
                get(CH_appwindow, MUIA_Selected, &active);
                win_aux->appwindow = (active == 1);
                get(CH_backdrop, MUIA_Selected, &active);
                win_aux->backdrop = (active == 1);
                get(CH_borderless, MUIA_Selected, &active);
                win_aux->borderless = (active == 1);
                get(CH_closegadget, MUIA_Selected, &active);
                win_aux->closegadget = (active == 1);
                get(CH_depthgadget, MUIA_Selected, &active);
                win_aux->depthgadget = (active == 1);
                get(CH_dragbar, MUIA_Selected, &active);
                win_aux->dragbar = (active == 1);
                get(CH_sizegadget, MUIA_Selected, &active);
                win_aux->sizegadget = (active == 1);
                get(CH_initopen, MUIA_Selected, &active);
                win_aux->initopen = (active == 1);
                get(CH_nomenu, MUIA_Selected, &active);
                win_aux->nomenu = (active == 1);

                if (win_aux->root.child->nb_elements > 0)
                {
                    if (testwin)
                    {
                        set(testwin, MUIA_Window_Open, FALSE);
                        DoMethod(app, OM_REMMEMBER, testwin);
                        MUI_DisposeObject(testwin);
                        testwin = NULL;
                    }
                    CreateChain((object *) win_aux, win_aux);
                    TestWindow(win_aux);
                    TestQuick(win_aux);
                    ShowObject(lv_group);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NoChild));
                break;
            case ID_INFO:
                ErrorMessage(LVT_Info);
                break;
            case ID_CONFIG:
                if (config.phantom)
                    set(WI_window, MUIA_Window_Open, FALSE);
                Config();
                DoMethod(lv_group, MUIM_List_Redraw, MUIV_List_Redraw_All);
                set(WI_window, MUIA_Window_Open, TRUE);
                WI_current = WI_window;
                break;
            case ID_SAVE:
                if (windows->nb_elements > 0)
                {
                    SaveApplication(FALSE);
                    modify = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedWindows));
                break;
            case ID_FOLD:
                set(lv_group, MUIA_List_Quiet, TRUE);
                DoMethod(lv_group, MUIM_List_Clear);
                ForceFold(&win_aux->root, FALSE);
                InitList(lv_group, &win_aux->root, 0, 1);
                set(lv_group, MUIA_List_Quiet, FALSE);
                break;
            case ID_UNFOLD:
                set(lv_group, MUIA_List_Quiet, TRUE);
                DoMethod(lv_group, MUIM_List_Clear);
                ForceFold(&win_aux->root, TRUE);
                InitList(lv_group, &win_aux->root, 0, 10000);
                set(lv_group, MUIA_List_Quiet, FALSE);
                break;
            case ID_UNDO:
                if (undo->nb_elements > 0)
                {
                    obj_aux = nth(undo, undo->nb_elements - 1);
                    delete_nth(undo, undo->nb_elements - 1);
                    chaine_aux = obj_aux->label;
                    add(tmp, obj_aux);
                    DoMethod(lv_tmp, MUIM_List_Insert, &chaine_aux, 1,
                             MUIV_List_Insert_Bottom);
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_NothingToUndelete));
                break;
            case ID_NEW_SUBMENU:
                if (config.phantom)
                    set(WI_window, MUIA_Window_Open, FALSE);
                if (NewMenu(win_aux->menu, FALSE))
                {
                    TestQuick(win_aux);
                    ShowObject(lv_group);
                }
                WI_current = WI_window;
                if (config.phantom)
                    set(WI_window, MUIA_Window_Open, TRUE);
                break;
            case ID_END:
                set(testwin, MUIA_Window_Open, FALSE);
                if (new)
                    delete_nth(windows, windows->nb_elements - 1);
                // FIXME: intentionally falltrough?
            case MUIV_Application_ReturnID_Quit:
                running = FALSE;
                break;
        }
        if (signal)
            Wait(signal);
    }
    set(WI_window, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_window);
    MUI_DisposeObject(WI_window);
    set(app, MUIA_Application_Sleep, FALSE);

    /* undelete the undo queue */
    n = undo->nb_elements;
    for (i = 0; i < n; i++)
    {
        DeleteObject(nth(undo, 0));
        delete_nth(undo, 0);
    }
    FreeVec(undo);

    return (result);
}
