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

BOOL MakeTest;

/* Interpreter of C-like strings */
void Interprete(char *src, char *dest)
{
    char *end;
    char *p, *q;
    int num;

    end = src + strlen(src) + 1;
    q = dest;
    for (p = src; p < end; p++, q++)
    {
        switch (*p)
        {
            case '\\':
                switch (*(++p))
                {
                    case '"':
                        *q = '"';
                        break;
                    case '\n':
                        q--;
                        break;
                    case 'n':
                        *q = '\n';
                        break;
                    case 'r':
                        *q = '\r';
                        break;
                    case 't':
                        *q = '\t';
                        break;
                    case 'e':
                        *q = '\033';
                        break;
                    case 'x':
                        sscanf((char *) ++p, "%2x", &num);
                        break;
                    default:
                        if ((*p <= '9') && (*p >= '0'))
                        {
                            sscanf((char *) p, "%3o", &num);
                            *q = num;
                            p += 2;
                        }
                        else
                        {
                            *q = *p;
                        }
                        break;
                }
                break;
            default:
                *q = *p;
        }
    }
}

void TestChain(window *win)
{
    int i;
    object *obj_aux;
    struct MUIP_Window_SetCycleChain *chain;
    chainon *chainon_aux;

    if (!win)
        return;

    chain =
        AllocVec(win->chain->nb_elements * sizeof(Object *) + sizeof(*chain),
                 MEMF_PUBLIC | MEMF_CLEAR);
    if (chain)
    {
        chain->MethodID = MUIM_Window_SetCycleChain;
        chainon_aux = win->chain->head;
        for (i = 0; i < win->chain->nb_elements; i++)
        {
            obj_aux = chainon_aux->element;
            chain->obj[i] = obj_aux->muiobj;
            chainon_aux = chainon_aux->next;
        }
        chain->obj[i] = NULL;
        win->muichain = chain;
        DoMethodA(win->muiobj, (Msg) chain);
        FreeVec(chain);
    }
}

void TestWindow(window *win)
{
    window *win_aux;
    APTR okwin;
    APTR bt_ok;
    BOOL running = TRUE;
    ULONG signal;
    int i;
    BOOL opening;
    int windows_opened = 0;
    chainon *chainon_aux;
    APTR WI_SaveCurrent;
    menu *CurrentMenu;

    set(app, MUIA_Application_Sleep, TRUE);

    if (win)
    {
        opening = win->initopen;
        win->initopen = TRUE;
    }
    chainon_aux = windows->head;
    for (i = 0; i < windows->nb_elements; i++)
    {
        win_aux = chainon_aux->element;
        if (win_aux != win)
        {
            CreateWindow(win_aux);
            TestChain(win_aux);
            DoMethod(win_aux->muiobj,
                     MUIM_Notify, MUIA_Window_Open, TRUE,
                     app, 2, MUIM_Application_ReturnID, ID_OPEN);
            DoMethod(win_aux->muiobj,
                     MUIM_Notify, MUIA_Window_Open, FALSE,
                     app, 2, MUIM_Application_ReturnID, ID_CLOSE);
            DoMethod(win_aux->muiobj,
                     MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                     win_aux->muiobj,
                     3, MUIM_Set, MUIA_Window_Open, FALSE);
            DoMethod(app, OM_ADDMEMBER, win_aux->muiobj);
        }
        chainon_aux = chainon_aux->next;
    }

    if (win)
    {
        CreateWindow(win);
        TestChain(win);
        DoMethod(win->muiobj,
                 MUIM_Notify, MUIA_Window_Open, TRUE,
                 app, 2, MUIM_Application_ReturnID, ID_OPEN);
        DoMethod(win->muiobj,
                 MUIM_Notify, MUIA_Window_Open, FALSE,
                 app, 2, MUIM_Application_ReturnID, ID_CLOSE);
        DoMethod(win->muiobj,
                 MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                 win->muiobj, 3, MUIM_Set, MUIA_Window_Open, FALSE);
        DoMethod(app, OM_ADDMEMBER, win->muiobj);
    }
    /* Application Menu creation */
    application.appmenu->muiobj = AppMenu;
    chainon_aux = application.appmenu->childs->head;
    for (i = 0; i < application.appmenu->childs->nb_elements; i++)
    {
        CurrentMenu = (menu *) chainon_aux->element;
        CreateWindow(CurrentMenu);
        TestNotify((object *) CurrentMenu);
        DoMethod(AppMenu, MUIM_Family_AddTail, CurrentMenu->muiobj);
        chainon_aux = chainon_aux->next;
    }
    chainon_aux = windows->head;
    for (i = 0; i < windows->nb_elements; i++)
    {
        win_aux = chainon_aux->element;
        if (win_aux != win)
        {
            TestNotify((object *) win_aux);
            set(win_aux->muiobj, MUIA_Window_Open, win_aux->initopen);
        }
        chainon_aux = chainon_aux->next;
    }
    if (win)
    {
        TestNotify((object *) win);
        set(win->muiobj, MUIA_Window_Open, win->initopen);
    }

    // *INDENT-OFF*
    okwin = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_TestWindow),
        MUIA_Window_ID, MAKE_ID('O', 'K', 'O', 'K'),
        WindowContents, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_TestClick)),
    End;
    // *INDENT-ON*

    DoMethod(okwin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_FINTEST);
    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_FINTEST);

    WI_SaveCurrent = WI_current;
    WI_current = okwin;

    DoMethod(app, OM_ADDMEMBER, okwin);
    set(okwin, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_CLOSE:
                windows_opened--;
                if (windows_opened == 0)
                    running = FALSE;
                break;
            case ID_OPEN:
                windows_opened++;
                break;
            case ID_FINTEST:
                running = FALSE;
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    DoMethod(app, OM_REMMEMBER, okwin);
    MUI_DisposeObject(okwin);
    /* Dispose all windows from application */
    chainon_aux = windows->head;
    for (i = 0; i < windows->nb_elements; i++)
    {
        win_aux = chainon_aux->element;
        if (win_aux != win)
        {
            DoMethod(app, OM_REMMEMBER, win_aux->muiobj);
            MUI_DisposeObject(win_aux->muiobj);
        }
        chainon_aux = chainon_aux->next;
    }
    if (win)
    {
        DoMethod(app, OM_REMMEMBER, win->muiobj);
        MUI_DisposeObject(win->muiobj);
        win->initopen = opening;
    }

    /* Dispose Application Menu */
    chainon_aux = application.appmenu->childs->head;
    for (i = 0; i < application.appmenu->childs->nb_elements; i++)
    {
        CurrentMenu = (menu *) chainon_aux->element;
        DoMethod(AppMenu, MUIM_Family_Remove, CurrentMenu->muiobj);
        chainon_aux = chainon_aux->next;
    }

    set(app, MUIA_Application_Sleep, FALSE);
    WI_current = WI_SaveCurrent;
}

APTR CreateWindow(APTR objet)
{
    APTR aux;
    window *win;
    bouton *button;
    group *grp;
    listview *listeview;
    chaine *string_aux;
    gauge *gauge_aux;
    cycle *cycle_aux;
    radio *radio_aux;
    label *label_aux;
    check *check_aux;
    scale *scale_aux;
    image *image_aux;
    slider *slider_aux;
    texte *text_aux;
    prop *prop_aux;
    dirlist *dirlist_aux;
    rectangle *rect_aux;
    colorfield *field_aux;
    popaslobj *popasl;
    popobject *popobj_aux;
    space *space_aux;
    menu *menu_aux;
    object *obj;
    char buffer[256];
    char buffer2[80];
    APTR result;
    APTR virt;
    int i, n;
    BOOL bool_aux;
    char *chaine, *chaine2, *chaine3;
    chainon *chainon_aux;

    CONST_STRPTR TestList[4];

    TestList[0] = GetMUIBuilderString(MSG_FirstLine);
    TestList[1] = GetMUIBuilderString(MSG_SecondLine);
    TestList[2] = GetMUIBuilderString(MSG_ThirdLine);
    TestList[3] = NULL;

    if (!objet)
        return (NULL);

    obj = objet;

    // *INDENT-OFF*
    switch (obj->id)
    {
        case TY_WINDOW:
            win = objet;
            sprintf(buffer, win->title);
            if (strlen(buffer) < 4)
                strcat(buffer, "TEST");
            result = CreateWindow(win->menu);
            if (win->nomenu)
            {
                result = WindowObject,
                    MUIA_Window_Title, win->title,
                    MUIA_Window_ID, MAKE_ID(buffer[0], buffer[1],
                                            buffer[2], buffer[3]),
                    MUIA_Window_Activate, FALSE,
                    MUIA_Window_AppWindow, win->appwindow,
                    MUIA_Window_Backdrop, win->backdrop,
                    MUIA_Window_Borderless, win->borderless,
                    MUIA_Window_CloseGadget, win->closegadget,
                    MUIA_Window_DepthGadget, win->depthgadget,
                    MUIA_Window_DragBar, win->dragbar,
                    MUIA_Window_SizeGadget, win->sizegadget,
                    MUIA_Window_NoMenus, win->nomenu,
                    MUIA_Window_Menustrip, result,
                    MUIA_Window_RootObject, CreateWindow(&win->root),
                End;
            }
            else
            {
                result = WindowObject,
                    MUIA_Window_Title, win->title,
                    MUIA_Window_ID, MAKE_ID(buffer[0], buffer[1],
                                            buffer[2], buffer[3]),
                    MUIA_Window_Activate, FALSE,
                    MUIA_Window_AppWindow, win->appwindow,
                    MUIA_Window_Backdrop, win->backdrop,
                    MUIA_Window_Borderless, win->borderless,
                    MUIA_Window_CloseGadget, win->closegadget,
                    MUIA_Window_DepthGadget, win->depthgadget,
                    MUIA_Window_DragBar, win->dragbar,
                    MUIA_Window_SizeGadget, win->sizegadget,
                    MUIA_Window_Menustrip, result,
                    MUIA_Window_RootObject, CreateWindow(&win->root),
                End;
            }
            if (!result)
                ErrorMessage("Can't create Window");
            obj->muiobj = result;
            break;
        case TY_GROUP:
            grp = objet;
            if (strlen(grp->Area.TitleFrame) > 0)
                chaine = grp->Area.TitleFrame;
            else
                chaine = NULL;
            if (grp->virtual)
            {
                if (!grp->registermode)
                {
                    result = ScrollgroupObject,
                        MUIA_Weight, grp->Area.Weight,
                        MUIA_Scrollgroup_Contents, virt = VirtgroupObject,
                            MUIA_Background, grp->Area.Background,
                            MUIA_ControlChar, grp->Area.key,
                            MUIA_ShowMe, !grp->Area.Hide,
                            MUIA_FramePhantomHoriz, grp->Area.Phantom,
                            MUIA_Disabled, grp->Area.Disable,
                            MUIA_FrameTitle, chaine,
                            MUIA_Frame, grp->Area.Frame,
                            MUIA_Group_SameHeight, grp->sameheight || grp->samesize,
                            MUIA_Group_SameWidth, grp->samewidth || grp->samesize,
                        End,
                    End;
                }
                else
                {
                    result = ScrollgroupObject,
                        MUIA_Weight, grp->Area.Weight,
                        MUIA_Scrollgroup_Contents, VirtgroupObject,
                            MUIA_Background, grp->Area.Background,
                            MUIA_ControlChar, grp->Area.key,
                            MUIA_ShowMe, !grp->Area.Hide,
                            MUIA_FramePhantomHoriz, grp->Area.Phantom,
                            MUIA_Disabled, grp->Area.Disable,
                            MUIA_FrameTitle, chaine,
                            MUIA_Frame, grp->Area.Frame,
                            MUIA_Group_SameHeight, grp->sameheight || grp->samesize,
                            MUIA_Group_SameWidth, grp->samewidth || grp->samesize,
                            Child, virt = RegisterGroup(grp->registertitles),
                            End,
                        End,
                    End;
                }
            }
            else
            {
                if (grp->registermode)
                {
                    result = RegisterGroup(grp->registertitles),
                        MUIA_Weight, grp->Area.Weight,
                        MUIA_Background, grp->Area.Background,
                        MUIA_ControlChar, grp->Area.key,
                        MUIA_ShowMe, !grp->Area.Hide,
                        MUIA_FramePhantomHoriz, grp->Area.Phantom,
                        MUIA_Disabled, grp->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, grp->Area.Frame,
                        MUIA_Group_SameHeight, grp->sameheight || grp->samesize,
                        MUIA_Group_SameWidth, grp->samewidth || grp->samesize,
                    End;
                }
                else
                {
                    result = GroupObject,
                        MUIA_Background, grp->Area.Background,
                        MUIA_ControlChar, grp->Area.key,
                        MUIA_ShowMe, !grp->Area.Hide,
                        MUIA_FramePhantomHoriz, grp->Area.Phantom,
                        MUIA_Disabled, grp->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, grp->Area.Frame,
                        MUIA_Group_SameHeight, grp->sameheight || grp->samesize,
                        MUIA_Group_SameWidth, grp->samewidth || grp->samesize,
                        MUIA_Weight, grp->Area.Weight,
                    End;
                }
            }
            if (grp->virtual)
            {
                set(virt, MUIA_Group_Horiz, grp->horizontal);
                if (grp->horizspacing)
                    set(virt, MUIA_Group_HorizSpacing, grp->horizspace);
                if (grp->vertspacing)
                    set(virt, MUIA_Group_VertSpacing, grp->vertspace);
                if (grp->rows)
                    set(virt, MUIA_Group_Rows, grp->number);
                if (grp->columns)
                    set(virt, MUIA_Group_Columns, grp->number);
            }
            else
            {
                set(result, MUIA_Group_Horiz, grp->horizontal);
                if (grp->horizspacing)
                    set(result, MUIA_Group_HorizSpacing, grp->horizspace);
                if (grp->vertspacing)
                    set(result, MUIA_Group_VertSpacing, grp->vertspace);
                if (grp->rows)
                    set(result, MUIA_Group_Rows, grp->number);
                if (grp->columns)
                    set(result, MUIA_Group_Columns, grp->number);
            }
            n = 0;
            if ((grp->rows) || (grp->columns))
            {
                n = grp->child->nb_elements % grp->number;
                if (n > 0)
                    n = grp->number - n;
                sprintf(buffer, GetMUIBuilderString(MSG_GroupError),
                        grp->label, grp->child->nb_elements + n,
                        grp->child->nb_elements);
                if ((n > 0) && (config.request))
                    ErrorMessage(buffer);
            }
            if ((grp->registermode)
                && (grp->entries->nb_elements != grp->child->nb_elements))
            {
                sprintf(buffer, GetMUIBuilderString(MSG_GroupError),
                        grp->label, grp->entries->nb_elements,
                        grp->child->nb_elements);
                if (config.request)
                    ErrorMessage(buffer);
                if (grp->entries->nb_elements > grp->child->nb_elements)
                    n = grp->entries->nb_elements -
                        grp->child->nb_elements;
            }
            chainon_aux = grp->child->head;
            for (i = 0; i < grp->child->nb_elements; i++)
            {
                if (grp->virtual)
                    DoMethod(virt, OM_ADDMEMBER,
                             CreateWindow(chainon_aux->element));
                else
                    DoMethod(result, OM_ADDMEMBER,
                             CreateWindow(chainon_aux->element));
                chainon_aux = chainon_aux->next;
            }
            if (grp->child->nb_elements == 0)
            {
                if (grp->virtual)
                {
                    DoMethod(virt, OM_ADDMEMBER, HVSpace);
                }
                else
                {
                    DoMethod(result, OM_ADDMEMBER, HVSpace);
                }
            }
            for (i = 0; i < n; i++)
            {
                if (grp->virtual)
                    DoMethod(virt, OM_ADDMEMBER, HVSpace);
                else
                    DoMethod(result, OM_ADDMEMBER, HVSpace);
            }
            obj->muiobj = result;
            break;
        case TY_KEYBUTTON:
            button = objet;
            Interprete(button->title, buffer);
            result = TextObject,
                ButtonFrame,
                MUIA_Text_Contents, buffer,
                MUIA_Text_PreParse, "\33c",
                MUIA_Text_HiChar, button->Area.key,
                MUIA_ControlChar, button->Area.key,
                MUIA_InputMode, MUIV_InputMode_RelVerify,
                MUIA_Background, MUII_ButtonBack,
                MUIA_Weight, button->Area.Weight,
                MUIA_Disabled, button->Area.Disable,
                MUIA_FramePhantomHoriz, button->Area.Phantom,
                MUIA_ShowMe, !button->Area.Hide,
            End;
            obj->muiobj = result;
            break;
        case TY_LISTVIEW:
            listeview = objet;
            Interprete(listeview->content, buffer);
            chaine = listeview->Area.TitleFrame;
            if (strlen(chaine) == 0)
                chaine = NULL;
            if (listeview->multiselect)
                i = MUIV_Listview_MultiSelect_Default;
            else
                i = MUIV_Listview_MultiSelect_None;
            if (strlen(listeview->title) > 0)
                chaine2 = listeview->title;
            else
                chaine2 = NULL;
            n = -1 - listeview->select;
            switch (listeview->type)
            {
                case 0:
                    result = ListviewObject,
                        MUIA_Weight, listeview->Area.Weight,
                        MUIA_Listview_MultiSelect, i,
                        MUIA_Listview_Input, listeview->inputmode,
                        MUIA_ControlChar, listeview->Area.key,
                        MUIA_InputMode, listeview->Area.InputMode,
                        MUIA_ShowMe, !listeview->Area.Hide,
                        MUIA_FramePhantomHoriz, listeview->Area.Phantom,
                        MUIA_FrameTitle, chaine,
                        MUIA_Listview_List, ListObject,
                            MUIA_Frame, listeview->Area.Frame,
                            MUIA_Background, listeview->Area.Background,
                            MUIA_Disabled, listeview->Area.Disable,
                            MUIA_List_SourceArray, TestList,
                            MUIA_List_AdjustHeight, listeview->adjustheight,
                            MUIA_List_AdjustWidth, listeview->adjustwidth,
                            MUIA_List_Active, n,
                            MUIA_List_Title, chaine2,
                        End,
                    End;
                    break;
                case 1:
                    result = ListviewObject,
                        MUIA_Weight, listeview->Area.Weight,
                        MUIA_Listview_MultiSelect, i,
                        MUIA_Listview_Input, listeview->inputmode,
                        MUIA_ControlChar, listeview->Area.key,
                        MUIA_InputMode, listeview->Area.InputMode,
                        MUIA_ShowMe, !listeview->Area.Hide,
                        MUIA_FramePhantomHoriz, listeview->Area.Phantom,
                        MUIA_FrameTitle, chaine,
                        MUIA_Listview_List, FloattextObject,
                            MUIA_Frame, listeview->Area.Frame,
                            MUIA_Background, listeview->Area.Background,
                            MUIA_Disabled, listeview->Area.Disable,
                            MUIA_Floattext_Text, buffer,
                            MUIA_List_AdjustHeight, listeview->adjustheight,
                            MUIA_List_AdjustWidth, listeview->adjustwidth,
                            MUIA_List_Active, n,
                            MUIA_List_Title, chaine2,
                        End,
                    End;
                    break;
                case 2:
                    result = ListviewObject,
                        MUIA_Weight, listeview->Area.Weight,
                        MUIA_Listview_MultiSelect, i,
                        MUIA_Listview_Input, listeview->inputmode,
                        MUIA_ControlChar, listeview->Area.key,
                        MUIA_InputMode, listeview->Area.InputMode,
                        MUIA_ShowMe, !listeview->Area.Hide,
                        MUIA_FramePhantomHoriz, listeview->Area.Phantom,
                        MUIA_FrameTitle, chaine,
                        MUIA_Listview_List, VolumelistObject,
                            MUIA_Frame, listeview->Area.Frame,
                            MUIA_Background, listeview->Area.Background,
                            MUIA_Disabled, listeview->Area.Disable,
                            MUIA_List_AdjustHeight, listeview->adjustheight,
                            MUIA_List_AdjustWidth, listeview->adjustwidth,
                            MUIA_List_Active, n,
                            MUIA_List_Title, chaine2,
                        End,
                    End;
                    break;
            }
            obj->muiobj = result;
            break;
        case TY_DIRLIST:
            dirlist_aux = objet;
            if (dirlist_aux->multi)
                i = MUIV_Listview_MultiSelect_Default;
            else
                i = MUIV_Listview_MultiSelect_None;
            if (strlen(dirlist_aux->Area.TitleFrame) > 0)
                chaine = dirlist_aux->Area.TitleFrame;
            else
                chaine = NULL;
            ParsePatternNoCase(dirlist_aux->accept, buffer, 256);
            if (strlen(dirlist_aux->accept) > 0)
                chaine2 = buffer;
            else
                chaine2 = NULL;
            ParsePatternNoCase(dirlist_aux->reject, buffer2, 80);
            if (strlen(dirlist_aux->reject) > 0)
                chaine3 = buffer2;
            else
                chaine3 = NULL;
            result = ListviewObject,
                MUIA_Weight, dirlist_aux->Area.Weight,
                MUIA_Listview_MultiSelect, i,
                MUIA_ControlChar, dirlist_aux->Area.key,
                MUIA_InputMode, dirlist_aux->Area.InputMode,
                MUIA_ShowMe, !dirlist_aux->Area.Hide,
                MUIA_FramePhantomHoriz, dirlist_aux->Area.Phantom,
                MUIA_FrameTitle, chaine,
                MUIA_Listview_Input, TRUE,
                MUIA_Listview_List, DirlistObject,
                    MUIA_Frame, dirlist_aux->Area.Frame,
                    MUIA_Background, dirlist_aux->Area.Background,
                    MUIA_Disabled, dirlist_aux->Area.Disable,
                    MUIA_Dirlist_Directory, dirlist_aux->directory,
                    MUIA_Dirlist_DrawersOnly, dirlist_aux->drawers,
                    MUIA_Dirlist_FilesOnly, dirlist_aux->files,
                    MUIA_Dirlist_FilterDrawers, dirlist_aux->filter,
                    MUIA_Dirlist_MultiSelDirs, dirlist_aux->multi,
                    MUIA_Dirlist_RejectIcons, dirlist_aux->icons,
                    MUIA_Dirlist_SortDirs, dirlist_aux->sortdirs,
                    MUIA_Dirlist_SortHighLow, dirlist_aux->highlow,
                    MUIA_Dirlist_SortType, dirlist_aux->sorttype,
                    MUIA_Dirlist_RejectPattern, chaine3,
                    MUIA_Dirlist_AcceptPattern, chaine2,
                End,
            End;
            obj->muiobj = result;
            break;
        case TY_TEXT:
            text_aux = objet;
            chaine = text_aux->Area.TitleFrame;
            if (strlen(chaine) == 0)
                chaine = NULL;
            Interprete(text_aux->content, buffer);
            Interprete(text_aux->preparse, text_aux->testpreparse);
            if (strlen(text_aux->testpreparse) > 0)
            {
                result = TextObject,
                    MUIA_Background, text_aux->Area.Background,
                    MUIA_ControlChar, text_aux->Area.key,
                    MUIA_InputMode, text_aux->Area.InputMode,
                    MUIA_ShowMe, !text_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, text_aux->Area.Phantom,
                    MUIA_Weight, text_aux->Area.Weight,
                    MUIA_Disabled, text_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_Frame, text_aux->Area.Frame,
                    MUIA_Text_HiChar, text_aux->Area.key,
                    MUIA_Text_PreParse, text_aux->testpreparse,
                    MUIA_Text_Contents, buffer,
                    MUIA_Text_SetMax, text_aux->max,
                    MUIA_Text_SetMin, text_aux->min,
                End;
            }
            else
            {
                result = TextObject,
                    MUIA_Background, text_aux->Area.Background,
                    MUIA_ControlChar, text_aux->Area.key,
                    MUIA_InputMode, text_aux->Area.InputMode,
                    MUIA_ShowMe, !text_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, text_aux->Area.Phantom,
                    MUIA_Weight, text_aux->Area.Weight,
                    MUIA_Disabled, text_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_Frame, text_aux->Area.Frame,
                    MUIA_Text_HiChar, text_aux->Area.key,
                    MUIA_Text_Contents, buffer,
                    MUIA_Text_SetMax, text_aux->max,
                    MUIA_Text_SetMin, text_aux->min,
                End;
            }
            obj->muiobj = result;
            break;
        case TY_CHECK:
            check_aux = objet;
            if (check_aux->title_exist)
            {
                result = ColGroup(2),
                    MUIA_ShowMe, !check_aux->Area.Hide,
                    Child, KeyLabel1(check_aux->title,
                                     check_aux->Area.key),
                    Child, virt = ImageObject,
                        ImageButtonFrame,
                        MUIA_InputMode, MUIV_InputMode_Toggle,
                        MUIA_Image_Spec, MUII_CheckMark,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_Selected, check_aux->init_state,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_ShowSelState, FALSE,
                        MUIA_ControlChar, check_aux->Area.key,
                        MUIA_Disabled, check_aux->Area.Disable,
                        MUIA_FramePhantomHoriz, check_aux->Area.Phantom,
                    End,
                End;
                obj->muiobj = virt;
            }
            else
            {
                result = ImageObject,
                    ImageButtonFrame,
                    MUIA_InputMode, MUIV_InputMode_Toggle,
                    MUIA_Image_Spec, MUII_CheckMark,
                    MUIA_Image_FreeVert, TRUE,
                    MUIA_Selected, check_aux->init_state,
                    MUIA_Background, MUII_ButtonBack,
                    MUIA_ShowSelState, FALSE,
                    MUIA_ControlChar, check_aux->Area.key,
                    MUIA_ShowMe, !check_aux->Area.Hide,
                    MUIA_Disabled, check_aux->Area.Disable,
                    MUIA_FramePhantomHoriz, check_aux->Area.Phantom,
                End;
                obj->muiobj = result;
            }
            break;
        case TY_SCALE:
            scale_aux = objet;
            result = ScaleObject,
                MUIA_Scale_Horiz, scale_aux->horiz,
                MUIA_Background, scale_aux->Area.Background,
                MUIA_InputMode, scale_aux->Area.InputMode,
                MUIA_ShowMe, !scale_aux->Area.Hide,
                MUIA_FramePhantomHoriz, scale_aux->Area.Phantom,
                MUIA_Weight, scale_aux->Area.Weight,
                MUIA_Disabled, scale_aux->Area.Disable,
                MUIA_Frame, scale_aux->Area.Frame,
            End;
            obj->muiobj = result;
            break;
        case TY_SLIDER:
            slider_aux = objet;
            chaine = slider_aux->Area.TitleFrame;
            if (strlen(chaine) == 0)
                chaine = NULL;
            if (slider_aux->title_exist)
            {
                result = GroupObject,
                    MUIA_Group_Horiz, slider_aux->horizontal,
                    Child, KeyLabel(slider_aux->title,
                                    slider_aux->Area.key),
                    Child, virt = SliderObject,
                        MUIA_Group_Horiz, slider_aux->horizontal,
                        MUIA_Slider_Min, slider_aux->min,
                        MUIA_Slider_Max, slider_aux->max,
                        MUIA_Slider_Quiet, slider_aux->quiet,
                        MUIA_Slider_Reverse, slider_aux->reverse,
                        MUIA_Slider_Level, slider_aux->init,
                        MUIA_Background, slider_aux->Area.Background,
                        MUIA_ControlChar, slider_aux->Area.key,
                        MUIA_InputMode, slider_aux->Area.InputMode,
                        MUIA_ShowMe, !slider_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, slider_aux->Area.Phantom,
                        MUIA_Weight, slider_aux->Area.Weight,
                        MUIA_Disabled, slider_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, slider_aux->Area.Frame,
                    End,
                End;
                obj->muiobj = virt;
            }
            else
            {
                result = SliderObject,
                    MUIA_Group_Horiz, slider_aux->horizontal,
                    MUIA_Slider_Min, slider_aux->min,
                    MUIA_Slider_Max, slider_aux->max,
                    MUIA_Slider_Quiet, slider_aux->quiet,
                    MUIA_Slider_Level, slider_aux->init,
                    MUIA_Slider_Reverse, slider_aux->reverse,
                    MUIA_Background, slider_aux->Area.Background,
                    MUIA_ControlChar, slider_aux->Area.key,
                    MUIA_InputMode, slider_aux->Area.InputMode,
                    MUIA_ShowMe, !slider_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, slider_aux->Area.Phantom,
                    MUIA_Weight, slider_aux->Area.Weight,
                    MUIA_Disabled, slider_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_Frame, slider_aux->Area.Frame,
                End;
                obj->muiobj = result;
            }
            break;
        case TY_STRING:
            string_aux = objet;
            if (strlen(string_aux->Area.TitleFrame) > 0)
                chaine = string_aux->Area.TitleFrame;
            else
                chaine = NULL;
            if (string_aux->title_exist)
            {
                result = ColGroup(2),
                    MUIA_Weight, string_aux->Area.Weight,
                    MUIA_ShowMe, !string_aux->Area.Hide,
                    Child, KeyLabel2(string_aux->title,
                                     string_aux->Area.key),
                    Child, aux = StringObject,
                        StringFrame,
                        MUIA_String_Contents, string_aux->content,
                        MUIA_String_Format, string_aux->format,
                        MUIA_String_MaxLen, string_aux->maxlen,
                        MUIA_String_Secret, string_aux->secret,
                        MUIA_Frame, string_aux->Area.Frame,
                        MUIA_InputMode, string_aux->Area.InputMode,
                        MUIA_Disabled, string_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_FramePhantomHoriz, string_aux->Area.Phantom,
                        MUIA_ControlChar, string_aux->Area.key,
                    End,
                End;
                if (strlen(string_aux->accept) > 0)
                    set(aux, MUIA_String_Accept, string_aux->accept);
                if (strlen(string_aux->reject) > 0)
                    set(aux, MUIA_String_Reject, string_aux->reject);
                obj->muiobj = aux;
            }
            else
            {
                result = StringObject, StringFrame,
                    MUIA_String_Contents, string_aux->content,
                    MUIA_String_Contents, string_aux->content,
                    MUIA_String_Accept, string_aux->accept,
                    MUIA_String_Reject, string_aux->reject,
                    MUIA_String_Format, string_aux->format,
                    MUIA_String_MaxLen, string_aux->maxlen,
                    MUIA_String_Secret, string_aux->secret,
                    MUIA_Weight, string_aux->Area.Weight,
                    MUIA_Frame, string_aux->Area.Frame,
                    MUIA_InputMode, string_aux->Area.InputMode,
                    MUIA_Disabled, string_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_FramePhantomHoriz, string_aux->Area.Phantom,
                    MUIA_ShowMe, !string_aux->Area.Hide,
                    MUIA_ControlChar, string_aux->Area.key,
                End;
                if (strlen(string_aux->accept) > 0)
                    set(result, MUIA_String_Accept, string_aux->accept);
                if (strlen(string_aux->reject) > 0)
                    set(result, MUIA_String_Reject, string_aux->reject);
                obj->muiobj = result;
            }
            break;
        case TY_LABEL:
            label_aux = objet;
            Interprete(label_aux->title, buffer);
            result = TextObject,
                MUIA_Text_PreParse, "\33r",
                MUIA_Text_Contents, buffer,
                MUIA_InnerLeft, 0,
                MUIA_InnerRight, 0,
                MUIA_Weight, label_aux->Area.Weight,
                MUIA_Disabled, label_aux->Area.Disable,
                MUIA_FramePhantomHoriz, label_aux->Area.Phantom,
                MUIA_ShowMe, !label_aux->Area.Hide,
                MUIA_ControlChar, label_aux->Area.key,
                MUIA_Text_HiChar, label_aux->Area.key,
            End;
            obj->muiobj = result;
            break;
        case TY_SPACE:
            space_aux = objet;
            switch (space_aux->type)
            {
                case 0:
                    result = HSpace(space_aux->spacing);
                    break;
                case 1:
                    result = VSpace(space_aux->spacing);
                    break;
                case 2:
                    result = HVSpace;
                    break;
            }
            obj->muiobj = result;
            break;
        case TY_GAUGE:
            gauge_aux = objet;
            chaine = gauge_aux->Area.TitleFrame;
            if (strlen(chaine) == 0)
                chaine = NULL;
            if (gauge_aux->fixheight)
            {
                if (gauge_aux->fixwidth)
                {
                    result = GaugeObject,
                        GaugeFrame,
                        MUIA_FixHeight, gauge_aux->height,
                        MUIA_FixWidth, gauge_aux->width,
                        MUIA_Gauge_Divide, gauge_aux->divide,
                        MUIA_Gauge_Horiz, gauge_aux->horizontal,
                        MUIA_Gauge_Max, gauge_aux->max,
                        MUIA_Gauge_InfoText, gauge_aux->infotext,
                        MUIA_ControlChar, gauge_aux->Area.key,
                        MUIA_InputMode, gauge_aux->Area.InputMode,
                        MUIA_ShowMe, !gauge_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, gauge_aux->Area.Phantom,
                        MUIA_Weight, gauge_aux->Area.Weight,
                        MUIA_Disabled, gauge_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, gauge_aux->Area.Frame,
                    End;
                }
                else
                {
                    result = GaugeObject,
                        GaugeFrame,
                        MUIA_FixHeight, gauge_aux->height,
                        MUIA_Gauge_Divide, gauge_aux->divide,
                        MUIA_Gauge_Horiz, gauge_aux->horizontal,
                        MUIA_Gauge_Max, gauge_aux->max,
                        MUIA_Gauge_InfoText, gauge_aux->infotext,
                        MUIA_ControlChar, gauge_aux->Area.key,
                        MUIA_InputMode, gauge_aux->Area.InputMode,
                        MUIA_ShowMe, !gauge_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, gauge_aux->Area.Phantom,
                        MUIA_Weight, gauge_aux->Area.Weight,
                        MUIA_Disabled, gauge_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, gauge_aux->Area.Frame,
                    End;
                }
            }
            else
            {
                if (gauge_aux->fixwidth)
                {
                    result = GaugeObject,
                        GaugeFrame,
                        MUIA_FixWidth, gauge_aux->width,
                        MUIA_Gauge_Divide, gauge_aux->divide,
                        MUIA_Gauge_Horiz, gauge_aux->horizontal,
                        MUIA_Gauge_Max, gauge_aux->max,
                        MUIA_Gauge_InfoText, gauge_aux->infotext,
                        MUIA_ControlChar, gauge_aux->Area.key,
                        MUIA_InputMode, gauge_aux->Area.InputMode,
                        MUIA_ShowMe, !gauge_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, gauge_aux->Area.Phantom,
                        MUIA_Weight, gauge_aux->Area.Weight,
                        MUIA_Disabled, gauge_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, gauge_aux->Area.Frame,
                    End;
                }
                else
                {
                    result = GaugeObject,
                        GaugeFrame,
                        MUIA_Gauge_Divide, gauge_aux->divide,
                        MUIA_Gauge_Horiz, gauge_aux->horizontal,
                        MUIA_Gauge_Max, gauge_aux->max,
                        MUIA_Gauge_InfoText, gauge_aux->infotext,
                        MUIA_ControlChar, gauge_aux->Area.key,
                        MUIA_InputMode, gauge_aux->Area.InputMode,
                        MUIA_ShowMe, !gauge_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, gauge_aux->Area.Phantom,
                        MUIA_Weight, gauge_aux->Area.Weight,
                        MUIA_Disabled, gauge_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, gauge_aux->Area.Frame,
                    End;
                }
            }
            obj->muiobj = result;
            break;
        case TY_CYCLE:
            cycle_aux = objet;
            result = CycleObject,
                MUIA_Cycle_Entries, cycle_aux->STRA_cycle,
                MUIA_Weight, cycle_aux->Area.Weight,
                MUIA_InputMode, cycle_aux->Area.InputMode,
                MUIA_Disabled, cycle_aux->Area.Disable,
                MUIA_FramePhantomHoriz, cycle_aux->Area.Phantom,
                MUIA_Frame, cycle_aux->Area.Frame,
                MUIA_ShowMe, !cycle_aux->Area.Hide,
                MUIA_ControlChar, cycle_aux->Area.key,
            End;
            obj->muiobj = result;
            break;
        case TY_PROP:
            prop_aux = objet;
            chaine = prop_aux->Area.TitleFrame;
            if (strlen(chaine) == 0)
                chaine = NULL;
            if (prop_aux->fixheight)
            {
                if (prop_aux->fixwidth)
                {
                    result = PropObject,
                        MUIA_Prop_Entries, prop_aux->entries,
                        MUIA_Prop_First, prop_aux->first,
                        MUIA_Prop_Horiz, prop_aux->horizontal,
                        MUIA_Prop_Visible, prop_aux->visible,
                        MUIA_FixHeight, prop_aux->height,
                        MUIA_FixWidth, prop_aux->width,
                        MUIA_Background, prop_aux->Area.Background,
                        MUIA_ControlChar, prop_aux->Area.key,
                        MUIA_InputMode, prop_aux->Area.InputMode,
                        MUIA_ShowMe, !prop_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, prop_aux->Area.Phantom,
                        MUIA_Weight, prop_aux->Area.Weight,
                        MUIA_Disabled, prop_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, prop_aux->Area.Frame,
                    End;
                }
                else
                {
                    result = PropObject,
                        MUIA_Prop_Entries, prop_aux->entries,
                        MUIA_Prop_First, prop_aux->first,
                        MUIA_Prop_Horiz, prop_aux->horizontal,
                        MUIA_Prop_Visible, prop_aux->visible,
                        MUIA_FixHeight, prop_aux->height,
                        MUIA_Background, prop_aux->Area.Background,
                        MUIA_ControlChar, prop_aux->Area.key,
                        MUIA_InputMode, prop_aux->Area.InputMode,
                        MUIA_ShowMe, !prop_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, prop_aux->Area.Phantom,
                        MUIA_Weight, prop_aux->Area.Weight,
                        MUIA_Disabled, prop_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, prop_aux->Area.Frame,
                    End;
                }
            }
            else
            {
                if (prop_aux->fixwidth)
                {
                    result = PropObject,
                        PropFrame,
                        MUIA_Prop_Entries, prop_aux->entries,
                        MUIA_Prop_First, prop_aux->first,
                        MUIA_Prop_Horiz, prop_aux->horizontal,
                        MUIA_Prop_Visible, prop_aux->visible,
                        MUIA_FixWidth, prop_aux->width,
                        MUIA_Background, prop_aux->Area.Background,
                        MUIA_ControlChar, prop_aux->Area.key,
                        MUIA_InputMode, prop_aux->Area.InputMode,
                        MUIA_ShowMe, !prop_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, prop_aux->Area.Phantom,
                        MUIA_Weight, prop_aux->Area.Weight,
                        MUIA_Disabled, prop_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, prop_aux->Area.Frame,
                    End;
                }
                else
                {
                    result = PropObject,
                        PropFrame,
                        MUIA_Prop_Entries, prop_aux->entries,
                        MUIA_Prop_First, prop_aux->first,
                        MUIA_Prop_Horiz, prop_aux->horizontal,
                        MUIA_Prop_Visible, prop_aux->visible,
                        MUIA_Background, prop_aux->Area.Background,
                        MUIA_ControlChar, prop_aux->Area.key,
                        MUIA_InputMode, prop_aux->Area.InputMode,
                        MUIA_ShowMe, !prop_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, prop_aux->Area.Phantom,
                        MUIA_Weight, prop_aux->Area.Weight,
                        MUIA_Disabled, prop_aux->Area.Disable,
                        MUIA_FrameTitle, chaine,
                        MUIA_Frame, prop_aux->Area.Frame,
                    End;
                }
            }
            obj->muiobj = result;
            break;
        case TY_IMAGE:
            image_aux = objet;
            if (strlen(image_aux->spec) > 0)
            {
                sprintf(buffer, "5:%s", image_aux->spec);
                result = ImageObject,
                    MUIA_ControlChar, image_aux->Area.key,
                    MUIA_Image_Spec, buffer,
                    MUIA_InputMode, image_aux->Area.InputMode,
                    MUIA_FixHeight, image_aux->height,
                    MUIA_FixWidth, image_aux->width,
                    MUIA_Frame, image_aux->Area.Frame,
                    MUIA_ShowMe, !image_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, image_aux->Area.Phantom,
                    MUIA_Image_FreeVert, image_aux->freevert,
                    MUIA_Image_FreeHoriz, image_aux->freehoriz,
                End;
            }
            else if (image_aux->fixheight)
            {
                if (image_aux->fixwidth)
                {
                    result = ImageObject,
                        MUIA_ControlChar, image_aux->Area.key,
                        MUIA_InputMode, image_aux->Area.InputMode,
                        MUIA_ShowMe, !image_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, image_aux->Area.Phantom,
                        MUIA_Weight, image_aux->Area.Weight,
                        MUIA_Disabled, image_aux->Area.Disable,
                        MUIA_Frame, image_aux->Area.Frame,
                        MUIA_Image_Spec, image_aux->type,
                        MUIA_Image_FreeVert, image_aux->freevert,
                        MUIA_Image_FreeHoriz, image_aux->freehoriz,
                        MUIA_FixHeight, image_aux->height,
                        MUIA_FixWidth, image_aux->width,
                    End;
                }
                else
                {
                    result = ImageObject,
                        MUIA_ControlChar, image_aux->Area.key,
                        MUIA_InputMode, image_aux->Area.InputMode,
                        MUIA_ShowMe, !image_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, image_aux->Area.Phantom,
                        MUIA_Weight, image_aux->Area.Weight,
                        MUIA_Disabled, image_aux->Area.Disable,
                        MUIA_Frame, image_aux->Area.Frame,
                        MUIA_Image_Spec, image_aux->type,
                        MUIA_Image_FreeVert, image_aux->freevert,
                        MUIA_Image_FreeHoriz, image_aux->freehoriz,
                        MUIA_FixHeight, image_aux->height,
                        MUIA_Image_FreeHoriz, image_aux->freehoriz,
                    End;
                }
            }
            else
            {
                if (image_aux->fixwidth)
                {
                    result = ImageObject,
                        MUIA_ControlChar, image_aux->Area.key,
                        MUIA_InputMode, image_aux->Area.InputMode,
                        MUIA_ShowMe, !image_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, image_aux->Area.Phantom,
                        MUIA_Weight, image_aux->Area.Weight,
                        MUIA_Disabled, image_aux->Area.Disable,
                        MUIA_Frame, image_aux->Area.Frame,
                        MUIA_Image_Spec, image_aux->type,
                        MUIA_Image_FreeVert, image_aux->freevert,
                        MUIA_Image_FreeHoriz, image_aux->freehoriz,
                        MUIA_FixWidth, image_aux->width,
                        MUIA_Image_FreeVert, image_aux->freevert,
                    End;
                }
                else
                {
                    result = ImageObject,
                        MUIA_ControlChar, image_aux->Area.key,
                        MUIA_InputMode, image_aux->Area.InputMode,
                        MUIA_ShowMe, !image_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, image_aux->Area.Phantom,
                        MUIA_Weight, image_aux->Area.Weight,
                        MUIA_Disabled, image_aux->Area.Disable,
                        MUIA_Frame, image_aux->Area.Frame,
                        MUIA_Image_Spec, image_aux->type,
                        MUIA_Image_FreeVert, image_aux->freevert,
                        MUIA_Image_FreeHoriz, image_aux->freehoriz,
                    End;
                }
            }
            obj->muiobj = result;
            break;
        case TY_RADIO:
            radio_aux = objet;
            if (strlen(radio_aux->Area.TitleFrame))
                chaine = radio_aux->Area.TitleFrame;
            else
                chaine = NULL;
            result = RadioObject,
                MUIA_Weight, radio_aux->Area.Weight,
                MUIA_Frame, radio_aux->Area.Frame,
                MUIA_Background, radio_aux->Area.Background,
                MUIA_InputMode, radio_aux->Area.InputMode,
                MUIA_Disabled, radio_aux->Area.Disable,
                MUIA_FrameTitle, chaine,
                MUIA_FramePhantomHoriz, radio_aux->Area.Phantom,
                MUIA_ShowMe, !radio_aux->Area.Hide,
                MUIA_ControlChar, radio_aux->Area.key,
                MUIA_Radio_Entries, radio_aux->STRA_radio,
            End;
            obj->muiobj = result;
            break;
        case TY_RECTANGLE:
            rect_aux = objet;
            chaine = rect_aux->Area.TitleFrame;
            if (strlen(chaine) == 0)
                chaine = NULL;
            bool_aux = (rect_aux->type == 1)
                && (strlen(rect_aux->Area.TitleFrame) > 0);
            if ((rect_aux->fixwidth) && (!rect_aux->fixheight)
                && (!bool_aux))
            {
                result = RectangleObject,
                    MUIA_Rectangle_VBar, (rect_aux->type == 2),
                    MUIA_Rectangle_HBar, (rect_aux->type == 1),
                    MUIA_FixWidth, rect_aux->width,
                    MUIA_Background, rect_aux->Area.Background,
                    MUIA_ControlChar, rect_aux->Area.key,
                    MUIA_InputMode, rect_aux->Area.InputMode,
                    MUIA_ShowMe, !rect_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, rect_aux->Area.Phantom,
                    MUIA_Weight, rect_aux->Area.Weight,
                    MUIA_Disabled, rect_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_Frame, rect_aux->Area.Frame,
                End;
            }
            if ((rect_aux->fixheight) && (!rect_aux->fixwidth)
                && (!bool_aux))
            {
                result = RectangleObject,
                    MUIA_Rectangle_VBar, (rect_aux->type == 2),
                    MUIA_Rectangle_HBar, (rect_aux->type == 1),
                    MUIA_FixHeight, rect_aux->height,
                    MUIA_Background, rect_aux->Area.Background,
                    MUIA_ControlChar, rect_aux->Area.key,
                    MUIA_InputMode, rect_aux->Area.InputMode,
                    MUIA_ShowMe, !rect_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, rect_aux->Area.Phantom,
                    MUIA_Weight, rect_aux->Area.Weight,
                    MUIA_Disabled, rect_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_Frame, rect_aux->Area.Frame,
                End;
            }
            if (rect_aux->fixwidth && rect_aux->fixheight && (!bool_aux))
            {
                result = RectangleObject,
                    MUIA_Rectangle_VBar, (rect_aux->type == 2),
                    MUIA_Rectangle_HBar, (rect_aux->type == 1),
                    MUIA_FixWidth, rect_aux->width,
                    MUIA_FixHeight, rect_aux->height,
                    MUIA_Background, rect_aux->Area.Background,
                    MUIA_ControlChar, rect_aux->Area.key,
                    MUIA_InputMode, rect_aux->Area.InputMode,
                    MUIA_ShowMe, !rect_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, rect_aux->Area.Phantom,
                    MUIA_Weight, rect_aux->Area.Weight,
                    MUIA_Disabled, rect_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_Frame, rect_aux->Area.Frame,
                End;
            }
            if ((!rect_aux->fixheight) && (!rect_aux->fixwidth)
                && (!bool_aux))
            {
                result = RectangleObject,
                    MUIA_Rectangle_VBar, (rect_aux->type == 2),
                    MUIA_Rectangle_HBar, (rect_aux->type == 1),
                    MUIA_Background, rect_aux->Area.Background,
                    MUIA_ControlChar, rect_aux->Area.key,
                    MUIA_InputMode, rect_aux->Area.InputMode,
                    MUIA_ShowMe, !rect_aux->Area.Hide,
                    MUIA_FramePhantomHoriz, rect_aux->Area.Phantom,
                    MUIA_Weight, rect_aux->Area.Weight,
                    MUIA_Disabled, rect_aux->Area.Disable,
                    MUIA_FrameTitle, chaine,
                    MUIA_Frame, rect_aux->Area.Frame,
                End;
            }
            if (bool_aux)
                result =
                    MUI_MakeObject(MUIO_BarTitle,
                                   rect_aux->Area.TitleFrame);
            obj->muiobj = result;
            break;
        case TY_COLORFIELD:
            field_aux = objet;
            if (field_aux->fixheight)
            {
                if (field_aux->fixwidth)
                {
                    result = ColorfieldObject,
                        MUIA_FixHeight, field_aux->height,
                        MUIA_FixWidth, field_aux->width,
                        MUIA_ControlChar, field_aux->Area.key,
                        MUIA_InputMode, field_aux->Area.InputMode,
                        MUIA_ShowMe, !field_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, field_aux->Area.Phantom,
                        MUIA_Weight, field_aux->Area.Weight,
                        MUIA_Disabled, field_aux->Area.Disable,
                        MUIA_Frame, field_aux->Area.Frame,
                        MUIA_Colorfield_Red, field_aux->red,
                        MUIA_Colorfield_Green, field_aux->green,
                        MUIA_Colorfield_Blue, field_aux->blue,
                    End;
                }
                else
                {
                    result = ColorfieldObject,
                        MUIA_FixHeight, field_aux->height,
                        MUIA_ControlChar, field_aux->Area.key,
                        MUIA_InputMode, field_aux->Area.InputMode,
                        MUIA_ShowMe, !field_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, field_aux->Area.Phantom,
                        MUIA_Weight, field_aux->Area.Weight,
                        MUIA_Disabled, field_aux->Area.Disable,
                        MUIA_Frame, field_aux->Area.Frame,
                        MUIA_Colorfield_Red, field_aux->red,
                        MUIA_Colorfield_Green, field_aux->green,
                        MUIA_Colorfield_Blue, field_aux->blue,
                    End;
                }
            }
            else
            {
                if (field_aux->fixwidth)
                {
                    result = ColorfieldObject,
                        MUIA_FixWidth, field_aux->width,
                        MUIA_ControlChar, field_aux->Area.key,
                        MUIA_InputMode, field_aux->Area.InputMode,
                        MUIA_ShowMe, !field_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, field_aux->Area.Phantom,
                        MUIA_Weight, field_aux->Area.Weight,
                        MUIA_Disabled, field_aux->Area.Disable,
                        MUIA_Frame, field_aux->Area.Frame,
                        MUIA_Colorfield_Red, field_aux->red,
                        MUIA_Colorfield_Green, field_aux->green,
                        MUIA_Colorfield_Blue, field_aux->blue,
                    End;
                }
                else
                {
                    result = ColorfieldObject,
                        MUIA_ControlChar, field_aux->Area.key,
                        MUIA_InputMode, field_aux->Area.InputMode,
                        MUIA_ShowMe, !field_aux->Area.Hide,
                        MUIA_FramePhantomHoriz, field_aux->Area.Phantom,
                        MUIA_Weight, field_aux->Area.Weight,
                        MUIA_Disabled, field_aux->Area.Disable,
                        MUIA_Frame, field_aux->Area.Frame,
                        MUIA_Colorfield_Red, field_aux->red,
                        MUIA_Colorfield_Green, field_aux->green,
                        MUIA_Colorfield_Blue, field_aux->blue,
                    End;
                }
            }
            obj->muiobj = result;
            break;
        case TY_POPASL:
            popasl = objet;
            result = PopaslObject,
                MUIA_Popasl_Type, popasl->type,
                MUIA_Popstring_String, String("", 80),
                MUIA_Popstring_Button, PopButton(popasl->image + MUII_PopUp),
                MUIA_ControlChar, popasl->Area.key,
                MUIA_ShowMe, !popasl->Area.Hide,
                MUIA_FramePhantomHoriz, popasl->Area.Phantom,
                MUIA_Weight, popasl->Area.Weight,
                MUIA_Disabled, popasl->Area.Disable,
                MUIA_Frame, popasl->Area.Frame,
            End;
            obj->muiobj = result;
            break;
        case TY_POPOBJECT:
            popobj_aux = objet;
            result = PopobjectObject,
                MUIA_Popstring_String, String("", 80),
                MUIA_Popstring_Button,
                PopButton(popobj_aux->image + MUII_PopUp),
                MUIA_Popobject_Light, popobj_aux->light,
                MUIA_Popobject_Follow, popobj_aux->follow,
                MUIA_Popobject_Volatile, popobj_aux->Volatile,
                MUIA_Popobject_Object, CreateWindow(popobj_aux->obj),
                MUIA_ControlChar, popobj_aux->Area.key,
                MUIA_ShowMe, !popobj_aux->Area.Hide,
                MUIA_FramePhantomHoriz, popobj_aux->Area.Phantom,
                MUIA_Weight, popobj_aux->Area.Weight,
                MUIA_Disabled, popobj_aux->Area.Disable,
                MUIA_Frame, popobj_aux->Area.Frame,
            End;
            obj->muiobj = result;
            break;
        case TY_MENU:
            menu_aux = objet;
            result = MenustripObject, End;
            chainon_aux = menu_aux->childs->head;
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                DoMethod(result, MUIM_Family_AddTail,
                         CreateWindow(chainon_aux->element));
                chainon_aux = chainon_aux->next;
            }
            if (!menu_aux->childs->nb_elements)
            {
                MUI_DisposeObject(result);
                result = NULL;
            }
            obj->muiobj = result;
            break;
        case TY_SUBMENU:
            menu_aux = objet;
            result = MenuitemObject,
                MUIA_Menuitem_Title, menu_aux->name,
                MUIA_Menu_Enabled, menu_aux->menu_enable, End;
            chainon_aux = menu_aux->childs->head;
            for (i = 0; i < menu_aux->childs->nb_elements; i++)
            {
                DoMethod(result, MUIM_Family_AddTail,
                         CreateWindow(chainon_aux->element));
                chainon_aux = chainon_aux->next;
            }
            obj->muiobj = result;
            break;
        case TY_MENUITEM:
            menu_aux = objet;
            if (strncmp(menu_aux->name, "BarLabel", 8) != 0)
                if (menu_aux->key)
                    result = MenuitemObject,
                        MUIA_Menuitem_Shortcut, &menu_aux->key,
                        MUIA_Menuitem_Title, menu_aux->name,
                        MUIA_Menuitem_Enabled, menu_aux->menu_enable,
                        MUIA_Menuitem_Checked, menu_aux->check_state,
                        MUIA_Menuitem_Checkit, menu_aux->check_enable,
                        MUIA_Menuitem_Toggle, menu_aux->Toggle,
                    End;
                else
                    result = MenuitemObject,
                        MUIA_Menuitem_Title, menu_aux->name,
                        MUIA_Menuitem_Enabled, menu_aux->menu_enable,
                        MUIA_Menuitem_Checked, menu_aux->check_state,
                        MUIA_Menuitem_Checkit, menu_aux->check_enable,
                        MUIA_Menuitem_Toggle, menu_aux->Toggle,
                    End;
            else
                result =
                    MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);
            obj->muiobj = result;
            break;
    }
    // *INDENT-ON*
    return (result);
}
