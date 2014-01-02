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

  $Id$

***************************************************************************/

#ifdef __MORPHOS__
#undef USE_INLINE_STDARG
#endif

#include <dos/dostags.h>
#include "builder.h"

char guidenamedir[256];
char filename[512];
queue *objects;
FILE *ficguide;

void Modify(APTR obj)
{
    ULONG signal;
    object *obj_aux;
    CONST_STRPTR aux;
    BOOL running = TRUE;
    APTR WI_modify, STR_title, TX_length, BT_edit, BT_ok, BT_cancel;
    CONST_STRPTR STR_TX_length;
    char buffer[128];
    APTR LV_text, CH_generated;
    char *STR_LV_text = NULL;
    int i;

    STR_TX_length = GetMUIBuilderString(MSG_TextLength);

    obj_aux = obj;

    // *INDENT-OFF*
    WI_modify = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_NodeTitle),
        MUIA_Window_ID, MAKE_ID('M', 'O', 'D', 'I'),
        MUIA_HelpNode, "Amigaguide",
        WindowContents, GroupObject,
            Child, ColGroup(2),
                Child, Label2(GetMUIBuilderString(MSG_NodeTitle)),
                Child, STR_title = StringObject,
                    StringFrame,
                    MUIA_String_Format, 0,
                    MUIA_String_Contents, obj_aux->Help.title,
                End,
            End,
            Child, HGroup,
                Child, HVSpace,
                Child, Label2(GetMUIBuilderString(MSG_HelpGenerated)),
                Child, CH_generated = CheckMark(obj_aux->Help.generated),
                Child, HVSpace,
            End,
            Child, TX_length = TextObject,
                MUIA_Background, 131,
                MUIA_Text_Contents, STR_TX_length,
                MUIA_Text_SetMax, 0,
                MUIA_Text_SetMin, 1,
                MUIA_Frame, 4,
            End,
            Child, LV_text = ListviewObject,
                MUIA_Listview_Input, FALSE,
                MUIA_Listview_List, FloattextObject,
                    MUIA_Floattext_Text, STR_LV_text,
                    ReadListFrame,
                End,
            End,
            Child, BT_edit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_EditNodeText)),
            Child, GroupObject,
                MUIA_Group_Horiz, 1,
                MUIA_Group_SameWidth, 1,
                Child, BT_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, BT_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    DoMethod(BT_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(BT_edit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWGRP);
    DoMethod(WI_modify, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_END);

    DoMethod(WI_modify, MUIM_Window_SetCycleChain, STR_title, CH_generated,
             LV_text, BT_edit, BT_ok, BT_cancel, NULL);

    sprintf(buffer, "%s : %d", GetMUIBuilderString(MSG_TextLength),
            obj_aux->Help.nb_char);
    set(TX_length, MUIA_Text_Contents, buffer);
    if (obj_aux->Help.nb_char > 0)
        set(LV_text, MUIA_Floattext_Text, obj_aux->Help.content);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_modify);
    set(WI_modify, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_title);
                strcpy(obj_aux->Help.title, aux);
                get(CH_generated, MUIA_Selected, &i);
                obj_aux->Help.generated = (i == 1);
                running = FALSE;
                break;
            case ID_NEWGRP:
                EditNode(obj_aux);
                sprintf(buffer, "%s : %d",
                        GetMUIBuilderString(MSG_TextLength),
                        obj_aux->Help.nb_char);
                set(TX_length, MUIA_Text_Contents, buffer);
                if (obj_aux->Help.nb_char > 0)
                    set(LV_text, MUIA_Floattext_Text,
                        obj_aux->Help.content);
                else
                    set(LV_text, MUIA_Floattext_Text, NULL);
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
    set(WI_modify, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_modify);
    MUI_DisposeObject(WI_modify);
    set(app, MUIA_Application_Sleep, FALSE);
}

void WriteHelp(APTR obj, FILE *fichier)
{
    object *obj_aux;
    int i;
    char *aux;

    obj_aux = obj;
    aux = obj_aux->Help.content;
    for (i = 0; i < obj_aux->Help.nb_char; i++)
    {
        fprintf(fichier, "%c", *aux);
        aux++;
    }
}

void GuideReference(APTR obj, FILE *fichier)
{
    object *obj_aux;
    group *group_aux;
    int i;

    obj_aux = obj;
    switch (obj_aux->id)
    {
        case TY_GROUP:
            group_aux = obj;
            if ((group_aux->Help.generated)
                && (group_aux->Help.nb_char > 0))
                fprintf(ficguide, "\t\t@{\" %s \" link %s }\n",
                        obj_aux->Help.title, obj_aux->label);
            else
                for (i = 0; i < group_aux->child->nb_elements; i++)
                {
                    GuideReference(nth(group_aux->child, i), fichier);
                }
            break;
        default:
            if ((obj_aux->Help.nb_char > 0) && (obj_aux->Help.generated))
            {
                fprintf(ficguide, "\t\t@{\" %s \" link %s }\n",
                        obj_aux->Help.title, obj_aux->label);
            }
    }
}

void GenerateGuide(APTR obj)
{
    object *obj_aux;
    window *win_aux;
    group *group_aux;
    popobject *popobj_aux;
    int i;

    obj_aux = obj;
    printf("Object = %s\n", obj_aux->label);

    switch (obj_aux->id)
    {
        case TY_WINDOW:
            win_aux = obj;
            fprintf(ficguide, "@NODE %s \"%s\"\n", win_aux->label,
                    win_aux->title);
            WriteHelp(win_aux, ficguide);
            for (i = 0; i < win_aux->root.child->nb_elements; i++)
            {
                GuideReference(nth(win_aux->root.child, i), ficguide);
            }
            fprintf(ficguide, "@ENDNODE\n\n");
            for (i = 0; i < win_aux->root.child->nb_elements; i++)
            {
                GenerateGuide(nth(win_aux->root.child, i));
            }
            break;
        case TY_GROUP:
            group_aux = obj;
            if ((group_aux->Help.generated) && (group_aux->Help.nb_char))
            {
                fprintf(ficguide, "@NODE %s \"%s\"\n", group_aux->label,
                        group_aux->Help.title);
                WriteHelp(group_aux, ficguide);
                for (i = 0; i < group_aux->child->nb_elements; i++)
                {
                    GuideReference(nth(group_aux->child, i), ficguide);
                }
                fprintf(ficguide, "@ENDNODE\n\n");
            }
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                GenerateGuide(nth(group_aux->child, i));
            }
            break;
        case TY_POPOBJECT:
            popobj_aux = obj;
            if ((obj_aux->Help.nb_char > 0) && (obj_aux->Help.generated))
            {
                fprintf(ficguide, "@NODE %s \"%s\"\n", obj_aux->label,
                        obj_aux->Help.title);
                WriteHelp(obj_aux, ficguide);
                obj_aux = popobj_aux->obj;
                GuideReference(obj_aux, ficguide);
                fprintf(ficguide, "@ENDNODE\n\n");
                GenerateGuide(obj_aux);
            }
            break;
        default:
            if ((obj_aux->Help.nb_char > 0) && (obj_aux->generated))
            {
                fprintf(ficguide, "@NODE %s \"%s\"\n", obj_aux->label,
                        obj_aux->Help.title);
                WriteHelp(obj_aux, ficguide);
                fprintf(ficguide, "@ENDNODE\n\n");
            }
            break;
    }
}

void EditNode(APTR obj)
{
    object *obj_aux;
    FILE *fichier;
    int i;
    char buffer[50];
    char *aux;
    BPTR alloc;
    struct FileInfoBlock Info;

    obj_aux = obj;
    if (obj_aux->Help.nb_char > 0)
    {
        if (fichier = fopen("T:MUIBuilderGuide.tmp", "w"))
        {
            aux = obj_aux->Help.content;
            for (i = 0; i < obj_aux->Help.nb_char; i++)
            {
                fprintf(fichier, "%c", *aux);
                aux++;
            }
        }
        else
            ErrorMessage(GetMUIBuilderString(MSG_CantOpen));
        fclose(fichier);
    }
    set(app, MUIA_Application_Sleep, TRUE);
    sprintf(buffer, "%s T:MUIBuilderGuide.tmp", config.editor);
    SystemTags(buffer,
               SYS_Input, Open("NIL:", MODE_NEWFILE),
               SYS_Output, Open("NIL:", MODE_NEWFILE),
               SYS_Asynch, FALSE,
               SYS_UserShell, TRUE, NP_ConsoleTask, NULL, TAG_DONE);
    set(app, MUIA_Application_Sleep, FALSE);
    if (obj_aux->Help.nb_char > 0)
        FreeVec(obj_aux->Help.content);
    if (alloc = Open("T:MUIBuilderGuide.tmp", MODE_OLDFILE))
    {
        ExamineFH(alloc, &Info);
        obj_aux->Help.nb_char = Info.fib_Size;
        obj_aux->Help.content =
            AllocVec(Info.fib_Size + 1, MEMF_PUBLIC | MEMF_CLEAR);
        Read(alloc, obj_aux->Help.content, obj_aux->Help.nb_char);
        aux =
            (char *) ((IPTR) obj_aux->Help.content +
                      obj_aux->Help.nb_char);
        *aux = '\0';
        Close(alloc);
    }
    DeleteFile("T:MUIBuilderGuide.tmp");

}

void EditAppNode(void)
{
    FILE *fichier;
    int i;
    char buffer[50];
    char *aux;
    BPTR alloc;
    struct FileInfoBlock Info;

    if (application.Help.nb_char > 0)
    {
        if (fichier = fopen("T:MUIBuilderGuide.tmp", "w"))
        {
            aux = application.Help.content;
            for (i = 0; i < application.Help.nb_char; i++)
            {
                fprintf(fichier, "%c", *aux);
                aux++;
            }
        }
        else
            ErrorMessage(GetMUIBuilderString(MSG_CantOpen));
        fclose(fichier);
    }
    set(app, MUIA_Application_Sleep, TRUE);
    sprintf(buffer, "%s T:MUIBuilderGuide.tmp", config.editor);
    Execute(buffer, 0, 0);
    set(app, MUIA_Application_Sleep, FALSE);
    if (application.Help.nb_char > 0)
        FreeVec(application.Help.content);
    if (alloc = Open("T:MUIBuilderGuide.tmp", MODE_OLDFILE))
    {
        ExamineFH(alloc, &Info);
        application.Help.nb_char = Info.fib_Size;
        application.Help.content =
            AllocVec(Info.fib_Size + 1, MEMF_PUBLIC | MEMF_CLEAR);
        Read(alloc, application.Help.content, application.Help.nb_char);
        aux =
            (char *) ((IPTR) application.Help.content +
                      application.Help.nb_char);
        *aux = '\0';
        Close(alloc);
    }
    DeleteFile("T:MUIBuilderGuide.tmp");
}

void AddObject(APTR obj, APTR list)
{
    object *obj_aux;
    window *win_aux;
    group *group_aux;
    char *chaine_aux;
    int i;

    obj_aux = obj;
    switch (obj_aux->id)
    {
        case TY_WINDOW:
            win_aux = obj;
            AddObject(&win_aux->root, list);
            break;
        case TY_GROUP:
            group_aux = obj;
            if (!group_aux->root)
            {
                add(objects, group_aux);
                chaine_aux = obj_aux->label;
                DoMethod(list, MUIM_List_Insert, &chaine_aux, 1,
                         MUIV_List_Insert_Bottom);
            }
            for (i = 0; i < group_aux->child->nb_elements; i++)
            {
                AddObject(nth(group_aux->child, i), list);
            }
            break;
        default:
            chaine_aux = obj_aux->label;
            DoMethod(list, MUIM_List_Insert, &chaine_aux, 1,
                     MUIV_List_Insert_Bottom);
            add(objects, obj);
            break;
    }
}

void Guide()
{
    ULONG signal;
    BOOL running = TRUE;
    APTR WI_guide, LV_windows, LV_childs, BT_objnode, BT_appnode,
        BT_winnode, BT_view;
    APTR BT_gen, BT_genobj, BT_genwin, BT_quit;
    APTR bt_edit, bt_edit2;
    char *chaine_aux;
    char buffer[128];
    window *win_aux;
    object *obj_aux;
    LONG active;
    int i;
    char name[255];

    objects = create();

    // *INDENT-OFF*
    WI_guide = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_AmigaGuide),
        MUIA_Window_ID, MAKE_ID('G', 'U', 'I', 'D'),
        MUIA_HelpNode, "Amigaguide",
        WindowContents, GroupObject,
            MUIA_Group_Horiz, 1,
            Child, GroupObject,
                GroupFrameT(GetMUIBuilderString(MSG_Objects)),
                MUIA_Group_Horiz, 1,
                Child, VGroup,
                    Child, LV_windows = ListviewObject,
                        MUIA_Listview_List, ListObject,
                            InputListFrame,
                        End,
                    End,
                    Child, bt_edit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Edit)),
                End,
                Child, VGroup,
                    Child, LV_childs = ListviewObject,
                        MUIA_Listview_List, ListObject,
                            InputListFrame,
                        End,
                    End,
                    Child, bt_edit2 =
                    MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Edit2)),
                End,
            End,
            Child, GroupObject,
                GroupFrameT(GetMUIBuilderString(MSG_Operations)),
                MUIA_Weight, 0,
                Child, BT_appnode = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_AppNode)),
                Child, BT_winnode = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_WindowNode)),
                Child, BT_objnode = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_ObjectNode)),
                Child, BT_gen = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_GenerateWholeDoc)),
                Child, BT_genwin = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_GenerateWinDoc)),
                Child, BT_genobj = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_GenerateObjDoc)),
                Child, BT_view = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_ViewDoc)),
                Child, BT_quit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Quit)),
                Child, HVSpace,
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_guide;

    DoMethod(BT_quit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(BT_objnode, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(BT_appnode, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWGRP);
    DoMethod(BT_winnode, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWWIN);
    DoMethod(BT_gen, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GENERATE);
    DoMethod(BT_genwin, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GENERATEWIN);
    DoMethod(BT_genobj, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_FINTEST);
    DoMethod(BT_view, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_INFO);
    DoMethod(LV_windows, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
             app, 2, MUIM_Application_ReturnID, ID_GROUP);
    DoMethod(LV_windows, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK);
    DoMethod(bt_edit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DBCLICK);
    DoMethod(LV_childs, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK2);
    DoMethod(bt_edit2, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DBCLICK2);
    DoMethod(WI_guide, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_END);

    DoMethod(WI_guide, MUIM_Window_SetCycleChain, LV_windows, LV_childs,
             BT_appnode, BT_winnode, BT_objnode, BT_gen, BT_genwin,
             BT_genobj, BT_view, BT_quit, bt_edit, bt_edit2, NULL);

    for (i = 0; i < windows->nb_elements; i++)
    {
        win_aux = nth(windows, i);
        chaine_aux = win_aux->label;
        DoMethod(LV_windows, MUIM_List_Insert, &chaine_aux, 1,
                 MUIV_List_Insert_Bottom);
    }

    if (strlen(application.helpfile) == 0)
    {
        strcpy(guidenamedir, savedir);
        strcpy(application.helpfile, savedir);
        AddPart(application.helpfile, savefile, 256);
        change_extend(application.helpfile, ".guide");
    }

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_guide);
    set(WI_guide, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_DBCLICK:
                get(LV_windows, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    Modify(nth(windows, active));
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectWindow));
                break;
            case ID_DBCLICK2:
                get(LV_childs, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    Modify(nth(objects, active));
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_GROUP:
                get(LV_windows, MUIA_List_Active, &active);
                win_aux = nth(windows, active);
                DoMethod(LV_childs, MUIM_List_Clear);
                delete_all(objects);
                set(LV_childs, MUIA_List_Quiet, TRUE);
                AddObject(win_aux, LV_childs);
                set(LV_childs, MUIA_List_Quiet, FALSE);
                break;
            case ID_NEWOBJ:
                get(LV_childs, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    obj_aux = nth(objects, active);
                    EditNode(obj_aux);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_NEWWIN:
                get(LV_windows, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    obj_aux = nth(windows, active);
                    EditNode(obj_aux);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectWindow));
                break;
            case ID_NEWGRP:
                EditAppNode();
                break;
            case ID_INFO:
                strcpy(name, FilePart(application.helpfile));
                if (LoadRequester
                    (GetMUIBuilderString(MSG_AmigaguideDocName),
                     guidenamedir, name))
                {
                    strncpy(filename, guidenamedir, 255);
                    AddPart(filename, name, 255);
                    sprintf(buffer, "SYS:Utilities/Multiview \"%s\"",
                            filename);
                    SystemTags(buffer, SYS_Input,
                               Open("NIL:", MODE_NEWFILE), SYS_Output,
                               Open("NIL:", MODE_NEWFILE), SYS_Asynch,
                               TRUE, SYS_UserShell, TRUE, NP_ConsoleTask,
                               NULL, TAG_DONE);
                    strncpy(application.helpfile, filename, 255);
                }
                break;
            case ID_GENERATE:
                strcpy(name, FilePart(application.helpfile));
                if (LoadRequester
                    (GetMUIBuilderString(MSG_AmigaguideDocName),
                     guidenamedir, name))
                {
                    strncpy(application.helpfile, guidenamedir, 255);
                    AddPart(application.helpfile, name, 255);
                    if (ficguide = fopen(application.helpfile, "w"))
                    {
                        fprintf(ficguide, "@database \"%s\"\n",
                                application.title);
                        fprintf(ficguide, "@NODE Main \"%s\"\n",
                                application.title);
                        chaine_aux = application.Help.content;
                        for (i = 0; i < application.Help.nb_char; i++)
                        {
                            fprintf(ficguide, "%c", *chaine_aux);
                            chaine_aux++;
                        }
                        for (i = 0; i < windows->nb_elements; i++)
                        {
                            obj_aux = nth(windows, i);
                            fprintf(ficguide, "\t\t@{\" %s \" link %s }\n",
                                    obj_aux->Help.title, obj_aux->label);
                        }
                        fprintf(ficguide, "@ENDNODE\n\n");
                        for (i = 0; i < windows->nb_elements; i++)
                        {
                            GenerateGuide(nth(windows, i));
                        }
                        fclose(ficguide);
                    }
                    else
                        ErrorMessage(GetMUIBuilderString
                                     (MSG_CantOpenAmigaGuide));
                }
                break;
            case ID_FINTEST:
                get(LV_childs, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    strcpy(name, FilePart(application.helpfile));
                    if (LoadRequester
                        (GetMUIBuilderString(MSG_AmigaguideDocName),
                         guidenamedir, name))
                    {
                        strncpy(application.helpfile, guidenamedir, 255);
                        AddPart(application.helpfile, name, 255);
                        if (ficguide = fopen(application.helpfile, "w"))
                        {
                            get(LV_childs, MUIA_List_Active, &i);
                            obj_aux = nth(objects, i);
                            GenerateGuide(obj_aux);
                            fclose(ficguide);
                        }
                        else
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_CantOpenAmigaGuide));
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
                break;
            case ID_GENERATEWIN:
                get(LV_windows, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    strcpy(name, FilePart(application.helpfile));
                    if (LoadRequester
                        (GetMUIBuilderString(MSG_AmigaguideDocName),
                         guidenamedir, name))
                    {
                        strncpy(application.helpfile, guidenamedir, 255);
                        AddPart(application.helpfile, name, 255);
                        if (ficguide = fopen(application.helpfile, "w"))
                        {
                            get(LV_windows, MUIA_List_Active, &i);
                            obj_aux = nth(windows, i);
                            GenerateGuide(obj_aux);
                            fclose(ficguide);
                        }
                        else
                            ErrorMessage(GetMUIBuilderString
                                         (MSG_CantOpenAmigaGuide));
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectObject));
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
    delete_all(objects);
    FreeVec(objects);
    set(WI_guide, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_guide);
    MUI_DisposeObject(WI_guide);
    set(app, MUIA_Application_Sleep, FALSE);
}
