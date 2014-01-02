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

#include <proto/muimaster.h>
#include <libraries/mui.h>
#include "builder.h"
#include "muibuilder_rev.h"

#if defined(__amigaos4__)
#define SYS_EDITOR "SYS:Tools/NotePad"
#elif defined(__AROS__)
#define SYS_EDITOR "SYS:Tools/Editor"
#else
#define SYS_EDITOR "C:Ed"
#endif

APTR WI_current;
APTR app, AppMenu;
queue *windows;
queue *labels;
queue *tmp;
struct DiskObject *dobj;
int nb_window, nb_listview, nb_group, nb_button, nb_string;
int nb_gauge, nb_cycle, nb_radio, nb_label, nb_check, nb_image, nb_slider;
int nb_text, nb_prop, nb_rectangle, nb_colorfield, nb_popasl;
int nb_popobject, nb_menu, nb_space, nb_scale, nb_barlabel;

// gpearman, 23/09/04
//int nb_modules = 0;

conf config;
appli application;
static APTR WI_build;
char MBDir[512], DocDir[512], Icon[512], ImageDir[256];
char *PopModules[50];
BOOL modify = FALSE;
struct Library *MUIMasterBase;
//LONG __stack = 20000;

/* Init & Fail Functions*/

static VOID fail(APTR app, CONST_STRPTR str)
{
    if (MUIMasterBase)
    {
        MUI_DisposeObject(app);
        CloseLibrary(MUIMasterBase);
    }

    if (str)
    {
        ErrorMessageEasy(str);
        exit(20);
    }
    exit(0);
}

static VOID init(VOID)
{
    char buffer[40];

    if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN)))
    {
        sprintf(buffer, "Failed to open %s V%d\n.", MUIMASTER_NAME,
                MUIMASTER_VMIN);
        fail(NULL, buffer);
    }
}

CONST_STRPTR GetStr(APTR obj)
{
    CONST_STRPTR aux;

    get(obj, MUIA_String_Contents, &aux);
    return (aux);
}


void InitAppli(void)
{
    application.id = TY_APPLI;
    strcpy(application.label, "App");
    application.generated = TRUE;
    strcpy(application.author, "NONE");
    strcpy(application.base, "NONE");
    strcpy(application.title, "NONE");
    strcpy(application.version, "$");
    strcat(application.version, "VER: NONE XX.XX (XX.XX.XX)");
    strcpy(application.copyright, "NOBODY");
    strcpy(application.description, "NONE");
    application.Help.nb_char = 0;
    application.appmenu = CreateMenu(TRUE);
    application.notify = create();
    application.Idents = create();
    application.notifysource = create();
    application.Functions = create();
    application.Constants = create();
    application.Idents = create();
    application.Variables = create();
    application.muiobj = app;
}

void DeleteApplication(void)
{
    int i;
    int n;

    DeleteObject(application.appmenu);
    DeleteObjNotify((object *) &application);
    DeleteSourceNotify((object *) &application);
    if (application.Help.nb_char > 0)
        FreeVec(application.Help.content);
    n = windows->nb_elements;
    for (i = 0; i < n; i++)
    {
        DeleteObject(nth(windows, 0));
        delete_nth(windows, 0);
    }
    n = application.Functions->nb_elements;
    for (i = 0; i < n; i++)
    {
        FreeVec(nth(application.Functions, 0));
        delete_nth(application.Functions, 0);
    }
    FreeVec(application.Functions);
    n = application.Idents->nb_elements;
    for (i = 0; i < n; i++)
    {
        FreeVec(nth(application.Idents, 0));
        delete_nth(application.Idents, 0);
    }
    FreeVec(application.Idents);
    n = application.Variables->nb_elements;
    for (i = 0; i < n; i++)
    {
        FreeVec(nth(application.Variables, 0));
        delete_nth(application.Variables, 0);
    }
    FreeVec(application.Variables);
    delete_all(labels);
    nb_window = 0;
    nb_listview = 0;
    nb_group = 0;
    nb_button = 0;
    nb_string = 0;
    nb_gauge = 0;
    nb_cycle = 0;
    nb_radio = 0;
    nb_label = 0;
    nb_check = 0;
    nb_image = 0;
    nb_slider = 0;
    nb_text = 0;
    nb_prop = 0;
    nb_rectangle = 0;
    nb_colorfield = 0;
    nb_popasl = 0;
    nb_popobject = 0;
    nb_menu = 0;
    nb_space = 0;
    nb_scale = 0;
    nb_barlabel = 0;
    modify = FALSE;
}

void EXIT_PRG(void)
{
    int i, n;

    DeleteApplication();
    FreeVec(windows);
    FreeVec(labels);
    for (i = 0; i < nb_modules; i++)
    {
        FreeVec(PopModules[i]);
    }
    n = tmp->nb_elements;
    for (i = 0; i < n; i++)
    {
        DeleteObject(nth(tmp, 0));
        delete_nth(tmp, 0);
    }
    FreeVec(tmp);
    if (dobj)
        FreeDiskObject(dobj);
    MUI_DisposeObject(app);
    DeleteFile("T:MUIBuilder1.tmp");
    DeleteFile("T:MUIBuilder2.tmp");
    DeleteFile("T:MUIBuilder3.tmp");
    DeleteFile("T:MUIBuilder4.tmp");
    DeleteFile("T:MUIBuilder5.tmp");
    if (MUIMasterBase)
        CloseLibrary(MUIMasterBase);
    CloseLocale();
    exit(0);
}

void NewAppli(void)
{
    APTR WI_app;
    APTR STR_base, STR_author, STR_copyright, STR_title;
    APTR STR_version, STR_description;
    APTR bt_ok, bt_cancel, bt_notify, bt_menu;
    ULONG signal;
    BOOL running = TRUE;
    CONST_STRPTR aux;

    // *INDENT-OFF*
    WI_app = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_Application),
        MUIA_Window_ID, MAKE_ID('A', 'P', 'P', 'L'),
        MUIA_HelpNode, "Application",
        WindowContents, VGroup,
            Child, GroupObject,
                MUIA_Group_Columns, 2,
                    Child, TextObject,
                    MUIA_Text_PreParse, "\33r",
                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Base),
                    MUIA_Weight, 0,
                    MUIA_InnerLeft, 0,
                    MUIA_InnerRight, 0,
                End,
                Child, STR_base = StringObject,
                    StringFrame,
                    MUIA_String_Contents, application.base,
                End,
                Child, TextObject,
                    MUIA_Text_PreParse, "\33r",
                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Author),
                    MUIA_Weight, 0,
                    MUIA_InnerLeft, 0,
                    MUIA_InnerRight, 0,
                End,
                Child, STR_author = StringObject,
                    StringFrame,
                    MUIA_String_Contents, application.author,
                End,
                Child, TextObject,
                    MUIA_Text_PreParse, "\33r",
                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Title),
                    MUIA_Weight, 0,
                    MUIA_InnerLeft, 0,
                    MUIA_InnerRight, 0,
                End,
                Child, STR_title = StringObject,
                    StringFrame,
                    MUIA_String_Contents, application.title,
                End,
                Child, TextObject,
                    MUIA_Text_PreParse, "\33r",
                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Version),
                    MUIA_Weight, 0,
                    MUIA_InnerLeft, 0,
                    MUIA_InnerRight, 0,
                End,
                Child, STR_version = StringObject,
                    StringFrame,
                    MUIA_String_Contents, application.version,
                End,
                Child, TextObject,
                    MUIA_Text_PreParse, "\33r",
                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Copyright),
                    MUIA_Weight, 0,
                    MUIA_InnerLeft, 0,
                    MUIA_InnerRight, 0,
                End,
                Child, STR_copyright = StringObject,
                    StringFrame,
                    MUIA_String_Contents, application.copyright,
                End,
                Child, TextObject,
                    MUIA_Text_PreParse, "\33r",
                    MUIA_Text_Contents, GetMUIBuilderString(MSG_Description),
                    MUIA_Weight, 0,
                    MUIA_InnerLeft, 0,
                    MUIA_InnerRight, 0,
                End,
                Child, STR_description = StringObject,
                    StringFrame,
                    MUIA_String_Contents, application.description,
                End,
            End,
            Child, ColGroup(2),
                MUIA_Group_SameWidth, TRUE,
                Child, bt_notify = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Notify)),
                Child, bt_menu = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Menu)),
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
             End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_app;

    DoMethod(WI_app, MUIM_Window_SetCycleChain, STR_base, STR_author,
             STR_title, STR_version, STR_copyright, STR_description,
             bt_notify, bt_menu, bt_ok, bt_cancel, NULL);
    DoMethod(bt_notify, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NOTIFY);
    DoMethod(bt_menu, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEW_SUBMENU);
    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(STR_base, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_app, 3, MUIM_Set, MUIA_Window_ActiveObject,
             STR_author);
    DoMethod(STR_author, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_app, 3, MUIM_Set, MUIA_Window_ActiveObject,
             STR_title);
    DoMethod(STR_title, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_app, 3, MUIM_Set, MUIA_Window_ActiveObject,
             STR_version);
    DoMethod(STR_version, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_app, 3, MUIM_Set, MUIA_Window_ActiveObject,
             STR_copyright);
    DoMethod(STR_copyright, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_app, 3, MUIM_Set, MUIA_Window_ActiveObject,
             STR_description);
    DoMethod(STR_description, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_app, 3, MUIM_Set, MUIA_Window_ActiveObject,
             STR_base);
    DoMethod(WI_app, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_END);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_app);
    set(WI_app, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_author);
                strcpy(application.author, aux);
                aux = GetStr(STR_base);
                strcpy(application.base, aux);
                aux = GetStr(STR_title);
                strcpy(application.title, aux);
                aux = GetStr(STR_version);
                strcpy(application.version, aux);
                aux = GetStr(STR_copyright);
                strcpy(application.copyright, aux);
                aux = GetStr(STR_description);
                strcpy(application.description, aux);
                running = FALSE;
                break;
            case ID_NOTIFY:
                CreateNotify((object *) &application, NULL);
                WI_current = WI_app;
                break;
            case ID_NEW_SUBMENU:
                if (config.phantom)
                    set(WI_app, MUIA_Window_Open, FALSE);
                NewMenu(application.appmenu, FALSE);
                WI_current = WI_app;
                if (config.phantom)
                    set(WI_app, MUIA_Window_Open, TRUE);
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
    set(WI_app, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_app);
    MUI_DisposeObject(WI_app);
    set(app, MUIA_Application_Sleep, FALSE);
}

//SAVEDS ASM LONG StrObjFunc(REG(a2) Object *pop,REG(a1) Object *str)
//BOOL StrObjFunc(Object *pop __asm("a2"), Object *str __asm("a1"))
HOOKPROTONH(StrObjFunc, BOOL, Object *pop, Object *str)
{
    char *x, *s;
    int i;

    get(str, MUIA_String_Contents, &s);

    for (i = 0;; i++)
    {
        DoMethod(pop, MUIM_List_GetEntry, i, &x);
        if (!x)
        {
            set(pop, MUIA_List_Active, MUIV_List_Active_Off);
            break;
        }
        else if (!stricmp(x, s))
        {
            set(pop, MUIA_List_Active, i);
            break;
        }
    }
    return (TRUE);
}

//SAVEDS ASM VOID ObjStrFunc(REG(a2) Object *pop,REG(a1) Object *str)
//void ObjStrFunc(Object *pop __asm("a2"), Object *str __asm("a1"))
HOOKPROTONH(ObjStrFunc, void, Object *pop, Object *str)
{
    char *x;
    DoMethod(pop, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &x);
    set(str, MUIA_String_Contents, x);
}

void SaveConfig(void)
{
    FILE *fichier;
    BPTR lock;

    lock = Lock(savedir, ACCESS_READ);
    if (lock)
    {
        NameFromLock(lock, savedir, 256);
    }
    if (fichier = fopen("ENV:MUIBuilder.env", "w"))
    {
        fprintf(fichier, "%d\n", config.phantom);
        fprintf(fichier, "%d\n", config.request);
        fprintf(fichier, "%s\n", config.langage);
        fprintf(fichier, "%d\n", config.icons);
        fprintf(fichier, "%d\n", MakeTest);
        fprintf(fichier, "%s\n", config.editor);
        fprintf(fichier, "%s\n", get_string);
        fprintf(fichier, "%d\n", config.depth);
        fprintf(fichier, "%c\n", config.tree_char);
        fprintf(fichier, "%s\n", savedir);
        fprintf(fichier, "%s\n", ImageDir);
        fprintf(fichier, "%d\n", config.columns);
        fprintf(fichier, "%d\n", config.labels);
        fclose(fichier);
    }
}

void Config(void)
{
    APTR WI_config;
    APTR CH_phantom, CH_request, CH_icons, CH_test;
    APTR bt_ok, bt_cancel, bt_save;
    APTR STR_editor, IM_geteditor;
    APTR STR_getstring, SL_depth, SL_columns;
    APTR STR_code, STR_TreeChar, PA_imagedir, CH_Labels;
    ULONG signal;
    BOOL running = TRUE;
    CONST_STRPTR aux;
    int active;
    char dir[526];
    char filename[256];
    CONST_STRPTR registertitle[4];

    MakeStaticHook(StrObjHook, StrObjFunc);
    MakeStaticHook(ObjStrHook, ObjStrFunc);

    APTR popup, plist;

    registertitle[0] = GetMUIBuilderString(MSG_Misc);
    registertitle[1] = GetMUIBuilderString(MSG_Tree);
    registertitle[2] = GetMUIBuilderString(MSG_Objects);;
    registertitle[3] = NULL;

    // *INDENT-OFF*
    WI_config = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_Configuration),
        MUIA_Window_ID, MAKE_ID('C', 'O', 'N', 'F'),
        MUIA_ExportID, MAKE_ID('C', 'O', 'N', 'F'),
        MUIA_HelpNode, "Prefs",
        WindowContents, VGroup,
            Child, RegisterGroup(registertitle),
                Child, VGroup,
                    Child, ColGroup(4),
                        GroupFrameT(GetMUIBuilderString(MSG_ConfigChoices)), Child,
                        HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_GhostWindows)),
                        Child, CH_phantom = CheckMark(config.phantom),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Icons)),
                        Child, CH_icons = CheckMark(config.icons),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Requests)),
                        Child, CH_request = CheckMark(config.request),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_TestLabel)),
                        Child, CH_test = CheckMark(MakeTest),
                        Child, HVSpace,
                    End,
                    Child, ColGroup(2),
                        Child, Label2(GetMUIBuilderString(MSG_CodeOption)),
                        Child, popup = PopobjectObject,
                            MUIA_Popstring_String, String(0, 60),
                            MUIA_Popstring_Button, PopButton(MUII_PopUp),
                            MUIA_Popobject_StrObjHook, &StrObjHook,
                            MUIA_Popobject_ObjStrHook, &ObjStrHook,
                            MUIA_Popobject_Object, plist = ListviewObject,
                                MUIA_Listview_List, ListObject,
                                    InputListFrame,
                                    MUIA_List_SourceArray, PopModules,
                                End,
                            End,
                        End,
                        Child, Label1(GetMUIBuilderString(MSG_GetString)),
                        Child, STR_getstring = StringObject,
                            StringFrame,
                            MUIA_String_Format, 0,
                            MUIA_String_Contents, get_string,
                        End,
                        Child, TextObject,
                            MUIA_Text_PreParse, "\033r",
                            MUIA_Text_Contents, GetMUIBuilderString(MSG_Editor),
                            MUIA_Weight, 0,
                            MUIA_InnerLeft, 0,
                            MUIA_InnerRight, 0,
                        End,
                        Child, HGroup,
                            MUIA_Group_HorizSpacing, 0,
                            Child, STR_editor = StringObject,
                                StringFrame,
                                MUIA_String_Format, 0,
                                MUIA_String_Contents, config.editor,
                            End,
                            Child, IM_geteditor = ImageObject,
                                MUIA_Image_Spec, 18,
                                MUIA_Image_FreeVert, 1,
                                MUIA_InputMode, MUIV_InputMode_RelVerify,
                                MUIA_Frame, MUIV_Frame_ImageButton,
                            End,
                        End,
                    End,
                End,
                Child, ColGroup(2),
                    Child, Label(GetMUIBuilderString(MSG_Depth)),
                    Child, SL_depth = Slider(1, 5, config.depth),
                    Child, Label(GetMUIBuilderString(MSG_TreeChar)),
                    Child, STR_TreeChar = StringObject,
                        StringFrame,
                        MUIA_String_MaxLen, 2,
                        MUIA_String_Contents, &config.tree_char,
                    End,
                End,
                Child, ColGroup(2),
                    Child, Label2(GetMUIBuilderString(MSG_Label)),
                    Child, HGroup,
                        Child, CH_Labels = CheckMark(config.labels),
                        Child, HVSpace,
                    End,
                    Child, Label(GetMUIBuilderString(MSG_Columns)),
                    Child, SL_columns = Slider(1, 5, config.columns),
                    Child, Label1(GetMUIBuilderString(MSG_ImageDrawer)),
                    Child, PA_imagedir = PopaslObject,
                        MUIA_Popasl_Type, 0,
                        MUIA_Popstring_String, String(ImageDir, 256),
                        MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
                        ASLFR_DrawersOnly, TRUE,
                    End,
                End,
            End,
            Child, HGroup,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_save = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Save)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(bt_save, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_SAVE);
    DoMethod(WI_config, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(IM_geteditor, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_EDIT);
    DoMethod(plist, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, popup, 2,
             MUIM_Popstring_Close, TRUE);

    DoMethod(WI_config, MUIM_Window_SetCycleChain, CH_phantom, CH_icons,
             CH_request, CH_test, SL_depth, popup, STR_editor,
             IM_geteditor, STR_getstring, bt_ok, bt_save, bt_cancel, NULL);

    get(popup, MUIA_Popstring_String, &STR_code);
    set(STR_code, MUIA_String_Contents, config.langage);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_config);

    set(WI_config, MUIA_Window_Open, TRUE);

    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_SAVE:
                get(CH_phantom, MUIA_Selected, &active);
                config.phantom = (active == 1);
                get(CH_request, MUIA_Selected, &active);
                config.request = (active == 1);
                get(CH_icons, MUIA_Selected, &active);
                config.icons = (active == 1);
                get(CH_test, MUIA_Selected, &active);
                MakeTest = (active == 1);
                get(CH_Labels, MUIA_Selected, &active);
                config.labels = active;
                aux = GetStr(STR_code);
                strcpy(config.langage, aux);
                aux = GetStr(STR_editor);
                strcpy(config.editor, aux);
                aux = GetStr(STR_getstring);
                strcpy(get_string, aux);
                get(SL_depth, MUIA_Slider_Level, &active);
                config.depth = active;
                get(SL_columns, MUIA_Slider_Level, &active);
                config.columns = active;
                aux = GetStr(STR_TreeChar);
                config.tree_char = *aux;
                strcpy(ImageDir, GetStr(PA_imagedir));
                SaveConfig();
                DoMethod(app, MUIM_Application_Save,
                         MUIV_Application_Save_ENVARC);
                Execute("copy ENV:MUIBuilder.env ENVARC:MUIBuilder.env", 0,
                        0);
                running = FALSE;
                break;
            case ID_EDIT:
                strcpy(dir, "SYS:Utilities");
                filename[0] = '\0';
                if (LoadRequester
                    (GetMUIBuilderString(MSG_EditorChoice), dir, filename))
                {
                    AddPart(dir, filename, 128);
                    set(STR_editor, MUIA_String_Contents, dir);
                }
                break;
            case ID_LOAD:
                strcpy(dir, MBDir);
                AddPart(dir, "modules", 512);
                filename[0] = '\0';
                if (LoadRequester
                    (GetMUIBuilderString(MSG_FileName), dir, filename))
                {
                    strncpy(config.langage, dir, 128);
                    AddPart(config.langage, filename, 128);
                    set(STR_code, MUIA_String_Contents, config.langage);
                }
                break;
            case ID_OKWIN:
                get(CH_phantom, MUIA_Selected, &active);
                config.phantom = (active == 1);
                get(CH_request, MUIA_Selected, &active);
                config.request = (active == 1);
                get(CH_icons, MUIA_Selected, &active);
                config.icons = (active == 1);
                get(CH_test, MUIA_Selected, &active);
                MakeTest = (active == 1);
                get(CH_Labels, MUIA_Selected, &active);
                config.labels = active;
                aux = GetStr(STR_code);
                strcpy(config.langage, aux);
                aux = GetStr(STR_editor);
                strcpy(config.editor, aux);
                aux = GetStr(STR_getstring);
                strcpy(get_string, aux);
                get(SL_depth, MUIA_Slider_Level, &active);
                config.depth = active;
                get(SL_columns, MUIA_Slider_Level, &active);
                config.columns = active;
                aux = GetStr(STR_TreeChar);
                config.tree_char = *aux;
                strcpy(ImageDir, GetStr(PA_imagedir));
                SaveConfig();
                running = FALSE;
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
    set(WI_config, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_config);
    MUI_DisposeObject(WI_config);
    set(app, MUIA_Application_Sleep, FALSE);
}

//SAVEDS ASM LONG AppMsgFunc(REG(a2) APTR obj, REG(a1) struct AppMessage **x)
//int AppMsgFunc(APTR obj __asm("a2"), struct AppMessage **x __asm("a1"))
HOOKPROTONH(AppMsgFunc, LONG, APTR obj, struct AppMessage **x)
{
    struct WBArg *ap;
    struct AppMessage *amsg = *x;
    static char buf[256];
    BOOL bool_aux;

    ap = amsg->am_ArgList;

    NameFromLock(ap->wa_Lock, buf, sizeof(buf));
    strcpy(savedir, buf);
    strcpy(savefile, ap->wa_Name);
    AddPart(buf, ap->wa_Name, sizeof(buf));
    if (WI_current == WI_build)
    {
        if ((windows->nb_elements > 0) && (config.request))
            bool_aux =
                RequestMessage(GetMUIBuilderString(MSG_ApplicationLost));
        if (bool_aux)
        {
            Load(obj, buf, FALSE);
        }
    }
    else
    {
        ErrorMessage(GetMUIBuilderString(MSG_ComeBack));
    }
    return (0);
}

void Load(APTR lv_window, char *file, BOOL request)
{
    object *child_aux;
    char *aux;
    int i;

    DoMethod(lv_window, MUIM_List_Clear);
    if (request)
        LoadApplication();
    else
    {
        LoadFile(file);
    }
    set(lv_window, MUIA_List_Quiet, TRUE);
    for (i = 0; i < windows->nb_elements; i++)
    {
        child_aux = (object *) nth(windows, i);
        aux = (char *) (child_aux->label);
        DoMethod(lv_window, MUIM_List_Insert, &aux, 1,
                 MUIV_List_Insert_Bottom);
    }
    if (windows->nb_elements > 0)
    {
        set(lv_window, MUIA_List_Active, 0);
        set(WI_current, MUIA_Window_ActiveObject, lv_window);
    }
    set(lv_window, MUIA_List_Quiet, FALSE);
}

void Merge(APTR lv_window)
{
    object *child_aux;
    char *aux;
    int i;

    DoMethod(lv_window, MUIM_List_Clear);
    MergeFile();
    set(lv_window, MUIA_List_Quiet, TRUE);
    for (i = 0; i < windows->nb_elements; i++)
    {
        child_aux = (object *) nth(windows, i);
        aux = (char *) (child_aux->label);
        DoMethod(lv_window, MUIM_List_Insert, &aux, 1,
                 MUIV_List_Insert_Bottom);
    }
    set(lv_window, MUIA_List_Quiet, FALSE);
}

int main(int argc, char *argv[])
{
    static APTR bt_new, bt_delete, bt_view, bt_code;
    static APTR bt_appli;
    static APTR bt_edit;
    static APTR bt_guide;
    static APTR lv_window;
    BOOL running;
    BOOL bool_aux;
    ULONG signal;
    ULONG val;
    FILE *fichier;
    window *win_aux;
    char *aux;
    int active;
    char LVT_Info[512];
    char buffer[512];
    struct WBStartup *WBMsg;
    BPTR lock;

    MakeStaticHook(AppMsgHook, AppMsgFunc);

    struct NewMenu Menu[] = {
        {NM_TITLE, "", "", 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_CONFIG},
        {NM_ITEM, NM_BARLABEL, 0, 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_NEWAPPLI},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_LOAD},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_SAVE},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_SAVEAS},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_MERGE},
        {NM_ITEM, NM_BARLABEL, 0, 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_INFO},
        {NM_ITEM, NM_BARLABEL, 0, 0, 0, (APTR) 0},
        {NM_ITEM, "", "", 0, 0, (APTR) ID_END},
        {NM_END, NULL, 0, 0, 0, (APTR) 0},
    };

    InitLocale();
    init();
    sprintf(LVT_Info, GetMUIBuilderString(MSG_AboutText), VERS, DATE);
    Menu[0].nm_Label = GetMUIBuilderString(MSG_Project);
    Menu[1].nm_Label = GetMenuString(MSG_Config);
    Menu[1].nm_CommKey = GetMenuChar(MSG_Config);
    Menu[3].nm_Label = GetMenuString(MSG_NewMenu);
    Menu[3].nm_CommKey = GetMenuChar(MSG_NewMenu);
    Menu[4].nm_Label = GetMenuString(MSG_Load);
    Menu[4].nm_CommKey = GetMenuChar(MSG_Load);
    Menu[5].nm_Label = GetMenuString(MSG_SaveMenu);
    Menu[5].nm_CommKey = GetMenuChar(MSG_SaveMenu);
    Menu[6].nm_Label = GetMenuString(MSG_SaveAs);
    Menu[6].nm_CommKey = GetMenuChar(MSG_SaveAs);
    Menu[7].nm_Label = GetMenuString(MSG_Merge);
    Menu[7].nm_CommKey = GetMenuChar(MSG_Merge);
    Menu[9].nm_Label = GetMenuString(MSG_About);
    Menu[9].nm_CommKey = GetMenuChar(MSG_About);
    Menu[11].nm_Label = GetMenuString(MSG_QuitMenu);
    Menu[11].nm_CommKey = GetMenuChar(MSG_QuitMenu);

    windows = create();
    labels = create();
    tmp = create();
    application.Help.nb_char = 0;

    real_getstring[0] = '\0';
    lock = Lock("PROGDIR:", ACCESS_READ);
    NameFromLock(lock, MBDir, 512);
    UnLock(lock);

    if (fichier = fopen("ENV:MUIBuilder.env", "r"))
    {
        config.phantom = ReadInt(fichier);
        config.request = ReadInt(fichier);
        strcpy(config.langage, LitChaine(fichier));
        config.icons = ReadInt(fichier);
        MakeTest = ReadInt(fichier);
        strcpy(config.editor, LitChaine(fichier));
        strcpy(get_string, LitChaine(fichier));
        config.depth = ReadInt(fichier);
        aux = LitChaine(fichier);
        config.tree_char = *aux;
        strcpy(savedir, LitChaine(fichier));
        strcpy(ImageDir, LitChaine(fichier));
        config.columns = ReadInt(fichier);
        if ((config.columns < 1) || (config.columns > 5))
            config.columns = 3;
        config.labels = ReadInt(fichier);
        if (config.labels)
            config.labels = TRUE;
        fclose(fichier);
    }
    else
    {
        config.phantom = FALSE;
        config.request = TRUE;
        MakeTest = FALSE;
        strcpy(config.langage, "C");
        config.icons = TRUE;
        strcpy(config.editor, SYS_EDITOR);
        get_string[0] = '\0';
        config.depth = 1;
        config.tree_char = ' ';
        savedir[0] = '\0';
        strncpy(ImageDir, MBDir, 128);
        AddPart(ImageDir, "Images/Chesnot_Hires_8", 128);
        config.labels = TRUE;
        config.columns = 1;
    }

    if (strlen(ImageDir) == 0)
    {
        strncpy(ImageDir, MBDir, 128);
        AddPart(ImageDir, "Images/Chesnot_Hires_8", 128);
    }

    ScanAvailableModules();
    InitNotifyArrays();

    strcpy(DocDir, MBDir);
    AddPart(DocDir, "Documentations/MUIBuilder.guide", 512);
    strcpy(Icon, MBDir);
    AddPart(Icon, "MUIBuilder", 512);

    // *INDENT-OFF*
    app = ApplicationObject,
        MUIA_Application_Title, "MUIBuilder",
        MUIA_Application_Version, "$VER: " VERS " (" DATE ")",
        MUIA_Application_Copyright, "Copyright ©1994-2011",
        MUIA_Application_Author, "Eric Totel, MUIBuilder Open Source Team",
        MUIA_Application_Description, GetMUIBuilderString(MSG_WrittenBy),
        MUIA_Application_Base, "BUILDER",
        MUIA_Application_HelpFile, DocDir,
        MUIA_Application_DiskObject, dobj = GetDiskObject(Icon),
        MUIA_Application_Menustrip, AppMenu = MenustripObject, End,
        SubWindow, WI_build = WindowObject,
            MUIA_Window_Title, VERS,
            MUIA_Window_ID, MAKE_ID('B', 'U', 'I', 'L'),
            MUIA_Window_AppWindow, TRUE,
            MUIA_Window_Menustrip, MUI_MakeObject(MUIO_MenustripNM, Menu),
            WindowContents, VGroup,
                Child, HGroup,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_Windows)),
                        MUIA_Weight, 300,
                        Child, lv_window = ListviewObject,
                            MUIA_Listview_DoubleClick, TRUE,
                            MUIA_Listview_List, ListObject,
                                InputListFrame,
                            End,
                        End,
                    End,
                    Child, VGroup,
                        GroupFrameT(GetMUIBuilderString(MSG_Creation)),
                        Child, HVSpace,
                        Child, bt_appli = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Appli)),
                        Child, HVSpace,
                        Child, bt_new = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_NewWindow)),
                        Child, HVSpace,
                        Child, bt_edit = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_EditWindow)),
                        Child, HVSpace,
                        Child, bt_delete = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Delete)),
                        Child, HVSpace,
                        Child, bt_view = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_View)),
                        Child, HVSpace,
                        Child, bt_code = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Code)),
                        Child, HVSpace,
                        Child, bt_guide = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Guide)),
                        Child, HVSpace,
                    End,
                End,
            End,
        End,
    End;
    // *INDENT-ON*

    if (!app)
    {
        fail(app, GetMUIBuilderString(MSG_Failed));
        exit(0);
    }

    InitAppli();

    WI_current = WI_build;

    if (argc)
    {
        if (argc > 1)
        {
            if (argc > 2)
                ErrorMessage(GetMUIBuilderString(MSG_Arguments));
            {
                lock = Lock(argv[1], ACCESS_READ);
                if (lock)
                {
                    NameFromLock(lock, savedir, 256);
                }
                else
                {
                    strncpy(savedir, argv[1], 256);
                }
                UnLock(lock);
                extract_file(savedir, savefile);
                extract_catalog(savedir);
                Load(lv_window, argv[1], FALSE);
            }
        }
    }
    else
    {
        WBMsg = (struct WBStartup *) argv;
        if (WBMsg->sm_NumArgs < 2)
        {
            savefile[0] = '\0';
        }
        else
        {
            strcpy(savefile, (*(WBMsg->sm_ArgList + 1)).wa_Name);
            NameFromLock((*(WBMsg->sm_ArgList + 1)).wa_Lock, savedir, 256);
            strcpy(buffer, savedir);
            AddPart(buffer, savefile, 512);
            Load(lv_window, buffer, FALSE);
        }
    }

    /* Button notifications */

    DoMethod(bt_appli, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_APPLI);
    DoMethod(bt_new, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWWIN);
    DoMethod(bt_view, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_TEST);
    DoMethod(bt_delete, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_DELETE);
    DoMethod(bt_code, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GENERATE);
    DoMethod(bt_edit, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_EDIT);
    DoMethod(bt_guide, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_GUIDE);

    /* Double-click notifications */

    DoMethod(lv_window, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, app,
             2, MUIM_Application_ReturnID, ID_DBCLICK);

    /* Close window notification */

    DoMethod(WI_build, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_END);


    /* Cycle chain */

    DoMethod(WI_build, MUIM_Window_SetCycleChain, bt_appli, bt_new,
             bt_edit, bt_delete, bt_view, bt_code, bt_guide, lv_window,
             NULL);

    DoMethod(lv_window, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
             lv_window, 3, MUIM_CallHook, &AppMsgHook, MUIV_TriggerValue);
    DoMethod(app, MUIM_Application_Load, MUIV_Application_Load_ENV);

    /* Open main window */

    set(WI_build, MUIA_Window_Open, TRUE);

    /* Main event handling */

    running = TRUE;
    while (running)
    {
        switch (val = DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_APPLI:
                if (config.phantom)
                    set(WI_build, MUIA_Window_Open, FALSE);
                NewAppli();
                WI_current = WI_build;
                if (config.phantom)
                    set(WI_build, MUIA_Window_Open, TRUE);
                break;
            case ID_INFO:
                ErrorMessage(LVT_Info);
                break;
            case ID_CONFIG:
                if (config.phantom)
                    set(WI_build, MUIA_Window_Open, FALSE);
                Config();
                WI_current = WI_build;
                set(WI_build, MUIA_Window_Open, TRUE);
                break;
            case ID_GUIDE:
                if (windows->nb_elements > 0)
                {
                    if (config.phantom)
                        set(WI_build, MUIA_Window_Open, FALSE);
                    Guide();
                    WI_current = WI_build;
                    modify = TRUE;
                    if (config.phantom)
                        set(WI_build, MUIA_Window_Open, TRUE);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedWindows));
                break;
            case ID_NEWWIN:
                win_aux =
                    AllocVec(sizeof(window), MEMF_PUBLIC | MEMF_CLEAR);
                if (win_aux == NULL)
                    break;

                win_aux->groups = create();
                win_aux->root.child = create();
                win_aux->generated = TRUE;
                win_aux->Help.nb_char = 0;

                InitGroup(&win_aux->root);
                win_aux->root.id = TY_GROUP;
                win_aux->root.father = win_aux;
                win_aux->root.root = TRUE;
                win_aux->root.horizontal = FALSE;
                win_aux->root.registermode = FALSE;
                win_aux->root.sameheight = FALSE;
                win_aux->root.samesize = FALSE;
                win_aux->root.samewidth = FALSE;
                win_aux->root.virtual = FALSE;
                win_aux->root.rows = FALSE;
                win_aux->root.columns = FALSE;
                win_aux->root.number = 2;
                win_aux->root.generated = FALSE;
                win_aux->root.Help.nb_char = 0;
                win_aux->root.horizspacing = FALSE;
                win_aux->root.vertspacing = FALSE;
                win_aux->root.horizspace = 0;
                win_aux->root.vertspace = 0;
                win_aux->root.entries = create();
                win_aux->root.notify = create();
                win_aux->root.notifysource = create();
                win_aux->root.deplie = TRUE;
                strcpy(win_aux->root.Area.TitleFrame, "");
                strcpy(win_aux->root.Help.title, "ROOT_TITLE");
                sprintf(win_aux->root.label, "GROUP_ROOT_%d", nb_window);
                add(win_aux->groups, &win_aux->root);

                win_aux->id = TY_WINDOW;
                if (config.phantom)
                    set(WI_build, MUIA_Window_Open, FALSE);
                if (NewWindow(win_aux, TRUE, NULL))
                {
                    aux = (char *) (win_aux->label);
                    set(lv_window, MUIA_List_Quiet, TRUE);
                    DoMethod(lv_window, MUIM_List_Insert, &aux, 1,
                             MUIV_List_Insert_Bottom);
                    strcpy(win_aux->Help.title, win_aux->title);
                    set(lv_window, MUIA_List_Quiet, FALSE);
                    modify = TRUE;
                }
                else
                    DeleteObject(win_aux);
                WI_current = WI_build;
                if (config.phantom)
                    set(WI_build, MUIA_Window_Open, TRUE);
                break;
            case ID_DBCLICK:
                get(lv_window, MUIA_List_Active, &active);
                win_aux = nth(windows, active);
                if (win_aux != NULL)
                {
                    if (config.phantom)
                        set(WI_build, MUIA_Window_Open, FALSE);
                    NewWindow(win_aux, FALSE, NULL);
                    WI_current = WI_build;
                    if (config.phantom)
                        set(WI_build, MUIA_Window_Open, TRUE);
                    DoMethod(lv_window, MUIM_List_Redraw,
                             MUIV_List_Redraw_Active);
                    modify = TRUE;
                }
                break;
            case ID_EDIT:
                get(lv_window, MUIA_List_Active, &active);
                if (active != MUIV_List_Active_Off)
                {
                    win_aux = nth(windows, active);
                    if (config.phantom)
                        set(WI_build, MUIA_Window_Open, FALSE);
                    NewWindow(win_aux, FALSE, NULL);
                    WI_current = WI_build;
                    modify = TRUE;
                    if (config.phantom)
                        set(WI_build, MUIA_Window_Open, TRUE);
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectWindow));
                break;
            case ID_TEST:
                if (windows->nb_elements > 0)
                {
                    TestWindow(NULL);
                    WI_current = WI_build;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedWindows));
                break;
            case ID_DELETE:
                get(lv_window, MUIA_List_Active, &active);
                bool_aux = TRUE;
                if (active != MUIV_List_Active_Off)
                {
                    if (config.request)
                        bool_aux =
                            RequestMessage(GetMUIBuilderString
                                           (MSG_Confirm));
                    if (bool_aux)
                    {
                        DeleteObject(nth(windows, active));
                        delete_nth(windows, active);
                        DoMethod(lv_window, MUIM_List_Remove,
                                 MUIV_List_Remove_Active);
                        modify = TRUE;
                    }
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_SelectWindow));
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
            case ID_SAVEAS:
                if (windows->nb_elements > 0)
                {
                    SaveApplication(TRUE);
                    modify = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedWindows));
                break;
            case ID_LOAD:
                bool_aux = TRUE;
                if ((windows->nb_elements > 0) && (config.request)
                    && (modify))
                    bool_aux =
                        RequestMessage(GetMUIBuilderString
                                       (MSG_ApplicationLost));
                if (bool_aux)
                {
                    set(app, MUIA_Application_Sleep, TRUE);
                    set(lv_window, MUIA_List_Quiet, TRUE);
                    Load(lv_window, "", TRUE);
                    set(lv_window, MUIA_List_Quiet, FALSE);
                    set(app, MUIA_Application_Sleep, FALSE);
                    modify = FALSE;
                }
                break;
            case ID_MERGE:
                set(lv_window, MUIA_List_Quiet, TRUE);
                Merge(lv_window);
                set(lv_window, MUIA_List_Quiet, FALSE);
                modify = TRUE;
                break;
            case ID_GENERATE:
                if (strlen(application.base) > 0)
                {
                    if (windows->nb_elements > 0)
                    {
                        if (config.phantom)
                            set(WI_build, MUIA_Window_Open, FALSE);
                        Code();
                        WI_current = WI_build;
                        modify = TRUE;
                        if (config.phantom)
                            set(WI_build, MUIA_Window_Open, TRUE);
                    }
                    else
                        ErrorMessage(GetMUIBuilderString(MSG_NeedWindows));
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedBase));
                break;
            case ID_NEWAPPLI:
                if (RequestMessage(GetMUIBuilderString(MSG_Confirm)))
                {
                    DeleteApplication();
                    savefile[0] = '\0';
                    InitAppli();
                    DoMethod(lv_window, MUIM_List_Clear);
                }
                break;
            case ID_END:
                bool_aux = TRUE;
                if (modify)
                    bool_aux =
                        RequestMessage(GetMUIBuilderString
                                       (MSG_ConfirmExit));
                if (bool_aux)
                {
                    SaveConfig();
                    EXIT_PRG();
                }
                break;
            case MUIV_Application_ReturnID_Quit:
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    return 0;
}
