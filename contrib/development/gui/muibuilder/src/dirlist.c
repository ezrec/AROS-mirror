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

void InitDirList(dirlist *dirlist_aux)
{
    InitArea(&dirlist_aux->Area);
    dirlist_aux->Area.Background = MUII_ListBack;
    dirlist_aux->Area.Frame = MUIV_Frame_InputList;
    dirlist_aux->drawers = FALSE;
    dirlist_aux->files = FALSE;
    dirlist_aux->filter = FALSE;
    dirlist_aux->multi = FALSE;
    dirlist_aux->icons = FALSE;
    dirlist_aux->highlow = FALSE;
    dirlist_aux->weight = 100;
    dirlist_aux->sorttype = 0;
    strcpy(dirlist_aux->accept, "");
    strcpy(dirlist_aux->directory, "progdir:");
    strcpy(dirlist_aux->reject, "");
    sprintf(dirlist_aux->label, "LV_label_%d", nb_listview);
    dirlist_aux->notify = create();
    dirlist_aux->notifysource = create();
    dirlist_aux->filterhook = NULL;
    dirlist_aux->sortdirs = 0;
}

APTR LoadDirList(FILE *fichier, APTR father, int version)
{
    dirlist *dirlist_aux;

    dirlist_aux = AllocVec(sizeof(dirlist), MEMF_PUBLIC | MEMF_CLEAR);
    InitDirList(dirlist_aux);
    dirlist_aux->id = TY_DIRLIST;
    dirlist_aux->father = father;
    strcpy(dirlist_aux->label, LitChaine(fichier));
    dirlist_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, dirlist_aux);
    if (version > 104)
        ReadNotify(fichier, dirlist_aux);
    if (version < 113)
        dirlist_aux->Area.Weight = ReadInt(fichier);
    else
        ReadArea(fichier, &dirlist_aux->Area);
    dirlist_aux->drawers = ReadInt(fichier);
    dirlist_aux->files = ReadInt(fichier);
    dirlist_aux->filter = ReadInt(fichier);
    dirlist_aux->multi = ReadInt(fichier);
    dirlist_aux->icons = ReadInt(fichier);
    dirlist_aux->highlow = ReadInt(fichier);
    dirlist_aux->sorttype = ReadInt(fichier);
    strcpy(dirlist_aux->directory, LitChaine(fichier));
    if (version >= 113)
    {
        strcpy(dirlist_aux->accept, LitChaine(fichier));
        strcpy(dirlist_aux->reject, LitChaine(fichier));
        dirlist_aux->sortdirs = ReadInt(fichier);
        ReadFunctionHook(fichier, &dirlist_aux->filterhook);
    }
    return (dirlist_aux);
}

void SaveDirList(FILE *fichier, dirlist *dirlist_aux)
{
    SaveArea(fichier, &dirlist_aux->Area);
    fprintf(fichier, "%d\n", dirlist_aux->drawers);
    fprintf(fichier, "%d\n", dirlist_aux->files);
    fprintf(fichier, "%d\n", dirlist_aux->filter);
    fprintf(fichier, "%d\n", dirlist_aux->multi);
    fprintf(fichier, "%d\n", dirlist_aux->icons);
    fprintf(fichier, "%d\n", dirlist_aux->highlow);
    fprintf(fichier, "%d\n", dirlist_aux->sorttype);
    fprintf(fichier, "%s\n", dirlist_aux->directory);
    fprintf(fichier, "%s\n", dirlist_aux->accept);
    fprintf(fichier, "%s\n", dirlist_aux->reject);
    fprintf(fichier, "%d\n", dirlist_aux->sortdirs);
    fprintf(fichier, "%s\n", dirlist_aux->filterhook);
}

BOOL NewDirList(dirlist *dirlist_aux, BOOL new)
{
    ULONG signal;
    BOOL running = TRUE;
    BOOL result = FALSE;
    int active;
    APTR CH_drawers, CH_files;
    APTR CH_filter, CH_multi, CH_icons, CH_highlow;
    APTR RA_sorttype, RA_sortdirs, STR_directory;
    APTR bt_ok, bt_cancel, WI_listdir, RegGroup;
    APTR STR_label, STR_accept, STR_reject;
    CONST_STRPTR aux;
    CONST_STRPTR STR_RA_sorttype[4];
    CONST_STRPTR STR_RA_sortdirs[4];
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    STR_RA_sorttype[0] = GetMUIBuilderString(MSG_SortbyName);
    STR_RA_sorttype[1] = GetMUIBuilderString(MSG_SortbyDate);
    STR_RA_sorttype[2] = GetMUIBuilderString(MSG_SortbySize);
    STR_RA_sorttype[3] = NULL;

    STR_RA_sortdirs[0] = GetMUIBuilderString(MSG_DirsFirst);
    STR_RA_sortdirs[1] = GetMUIBuilderString(MSG_DirsLast);
    STR_RA_sortdirs[2] = GetMUIBuilderString(MSG_DirsMixed);
    STR_RA_sortdirs[3] = NULL;

    if (new)
    {
        InitDirList(dirlist_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&dirlist_aux->Area, FALSE, TRUE, TRUE, TRUE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_listdir = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ListDirChoices),
        MUIA_Window_ID, MAKE_ID('D', 'I', 'R', 'L'),
        MUIA_HelpNode, "Dirlist",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, HGroup,
                        Child, GroupObject,
                            GroupFrameT(GetMUIBuilderString(MSG_Attributes)),
                            MUIA_Group_Columns, 4,
                            Child, HVSpace,
                            Child, TextObject,
                                MUIA_Text_PreParse, "\33r",
                                MUIA_Text_Contents, GetMUIBuilderString(MSG_DrawersOnly),
                                MUIA_Weight, 0,
                                MUIA_InnerLeft, 0,
                                MUIA_InnerRight, 0,
                            End,
                            Child, CH_drawers = CheckMark(FALSE),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, TextObject,
                                MUIA_Text_PreParse, "\33r",
                                MUIA_Text_Contents, GetMUIBuilderString(MSG_FilesOnly),
                                MUIA_Weight, 0,
                                MUIA_InnerLeft, 0,
                                MUIA_InnerRight, 0,
                            End,
                            Child, CH_files = CheckMark(FALSE),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, TextObject,
                                MUIA_Text_PreParse, "\33r",
                                MUIA_Text_Contents, GetMUIBuilderString(MSG_FilterDrawers),
                                MUIA_Weight, 0,
                                MUIA_InnerLeft, 0,
                                MUIA_InnerRight, 0,
                            End,
                            Child, CH_filter = CheckMark(FALSE),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, TextObject,
                                MUIA_Text_PreParse, "\33r",
                                MUIA_Text_Contents, GetMUIBuilderString(MSG_MultiSelection),
                                MUIA_Weight, 0,
                                MUIA_InnerLeft, 0,
                                MUIA_InnerRight, 0,
                            End,
                            Child, CH_multi = CheckMark(FALSE),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, TextObject,
                                MUIA_Text_PreParse, "\33r",
                                MUIA_Text_Contents, GetMUIBuilderString(MSG_RejectIcons),
                                MUIA_Weight, 0,
                                MUIA_InnerLeft, 0,
                                MUIA_InnerRight, 0,
                            End,
                            Child, CH_icons = CheckMark(FALSE),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, TextObject,
                                MUIA_Text_PreParse, "\33r",
                                MUIA_Text_Contents, GetMUIBuilderString(MSG_SortHighLow),
                                MUIA_Weight, 0,
                                MUIA_InnerLeft, 0,
                                MUIA_InnerRight, 0,
                            End,
                            Child, CH_highlow = CheckMark(FALSE),
                            Child, HVSpace,
                        End,
                        Child, VGroup,
                            Child, RA_sorttype = RadioObject,
                                GroupFrameT(GetMUIBuilderString(MSG_SortType)),
                                MUIA_Radio_Entries, STR_RA_sorttype,
                            End,
                            Child, RA_sortdirs = RadioObject,
                                GroupFrameT(GetMUIBuilderString(MSG_SortDirs)),
                                MUIA_Radio_Entries, STR_RA_sortdirs,
                            End,
                        End,
                    End,
                    Child, GroupObject,
                        MUIA_Group_Columns, 2,
                        Child, TextObject,
                            MUIA_Text_PreParse, "\33r",
                            MUIA_Text_Contents, GetMUIBuilderString(MSG_Dir),
                            MUIA_Weight, 0,
                            MUIA_InnerLeft, 0,
                            MUIA_InnerRight, 0,
                        End,
                        Child, STR_directory = StringObject,
                            StringFrame,
                            MUIA_String_Contents, dirlist_aux->directory,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Accept)),
                        Child, STR_accept = StringObject,
                            StringFrame,
                            MUIA_String_Contents, dirlist_aux->accept,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Reject)),
                        Child, STR_reject = StringObject,
                            StringFrame,
                            MUIA_String_Contents, dirlist_aux->reject,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, dirlist_aux->label,
                        End,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, GroupObject,
                MUIA_Group_Horiz, 1,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_listdir;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_listdir, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_listdir, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_directory);
    DoMethod(STR_directory, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_listdir, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_accept);
    DoMethod(STR_accept, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_listdir, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_reject);
    DoMethod(STR_reject, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_listdir, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);

    DoMethod(WI_listdir, MUIM_Window_SetCycleChain, RegGroup, CH_drawers,
             CH_files, CH_filter, CH_multi, CH_icons, CH_highlow,
             RA_sorttype, RA_sortdirs, STR_directory, STR_accept,
             STR_reject, STR_label, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, bt_ok,
             bt_cancel, NULL);

    set(RA_sorttype, MUIA_Radio_Active, dirlist_aux->sorttype);
    set(RA_sortdirs, MUIA_Radio_Active, dirlist_aux->sortdirs);
    set(CH_drawers, MUIA_Selected, dirlist_aux->drawers);
    set(CH_files, MUIA_Selected, dirlist_aux->files);
    set(CH_filter, MUIA_Selected, dirlist_aux->filter);
    set(CH_multi, MUIA_Selected, dirlist_aux->multi);
    set(CH_icons, MUIA_Selected, dirlist_aux->icons);
    set(CH_highlow, MUIA_Selected, dirlist_aux->highlow);
    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_listdir);
    set(WI_listdir, MUIA_Window_Open, TRUE);

    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(dirlist_aux->label, aux);
                    aux = GetStr(STR_directory);
                    strcpy(dirlist_aux->directory, aux);
                    aux = GetStr(STR_accept);
                    strcpy(dirlist_aux->accept, aux);
                    aux = GetStr(STR_reject);
                    strcpy(dirlist_aux->reject, aux);
                    get(CH_drawers, MUIA_Selected, &active);
                    dirlist_aux->drawers = (active == 1);
                    get(CH_files, MUIA_Selected, &active);
                    dirlist_aux->files = (active == 1);
                    get(CH_filter, MUIA_Selected, &active);
                    dirlist_aux->filter = (active == 1);
                    get(CH_multi, MUIA_Selected, &active);
                    dirlist_aux->multi = (active == 1);
                    get(CH_icons, MUIA_Selected, &active);
                    dirlist_aux->icons = (active == 1);
                    get(CH_highlow, MUIA_Selected, &active);
                    dirlist_aux->highlow = (active == 1);
                    get(RA_sorttype, MUIA_Radio_Active,
                        &dirlist_aux->sorttype);
                    get(RA_sortdirs, MUIA_Radio_Active,
                        &dirlist_aux->sortdirs);
                    ValidateArea(GR_GroupArea, &dirlist_aux->Area);
                    if (new)
                    {
                        nb_listview++;
                    }
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
                running = FALSE;
                break;
        }
        if (signal)
            Wait(signal);
    }
    set(WI_listdir, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_listdir);
    MUI_DisposeObject(WI_listdir);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
