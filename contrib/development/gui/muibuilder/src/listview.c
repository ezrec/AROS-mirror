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

void InitListview(listview *list)
{
    InitArea(&list->Area);
    list->Area.Background = MUII_ListBack;
    list->Area.Frame = MUIV_Frame_InputList;
    sprintf(list->label, "LV_label_%d", nb_listview);
    list->title[0] = '\0';
    list->select = 0;
    list->multiselect = TRUE;
    list->doubleclick = FALSE;
    list->adjustheight = FALSE;
    list->adjustwidth = FALSE;
    list->inputmode = TRUE;
    list->displayhook = NULL;
    list->comparehook = NULL;
    list->constructhook = NULL;
    list->destructhook = NULL;
    list->multitesthook = NULL;
    list->format[0] = '\0';
    list->content[0] = '\0';
    list->notify = create();
    list->notifysource = create();
}

APTR LoadListview(FILE *fichier, APTR father, int version)
{
    listview *listview_aux;

    listview_aux = AllocVec(sizeof(listview), MEMF_PUBLIC | MEMF_CLEAR);
    InitListview(listview_aux);
    listview_aux->id = TY_LISTVIEW;
    listview_aux->father = father;
    strcpy(listview_aux->label, LitChaine(fichier));
    listview_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, listview_aux);
    if (version > 104)
        ReadNotify(fichier, listview_aux);
    if (version < 113)
    {
        listview_aux->Area.Weight = ReadInt(fichier);
    }
    if (version >= 113)
        ReadArea(fichier, &listview_aux->Area);
    listview_aux->type = ReadInt(fichier);
    if ((version < 113) && (listview_aux->type == 1))
        listview_aux->Area.Frame = MUIV_Frame_ReadList;
    if (version > 101)
    {
        listview_aux->multiselect = ReadInt(fichier);
        listview_aux->doubleclick = ReadInt(fichier);
    }
    if (version >= 113)
    {
        listview_aux->select = ReadInt(fichier);
        listview_aux->adjustheight = ReadInt(fichier);
        listview_aux->adjustwidth = ReadInt(fichier);
        listview_aux->inputmode = ReadInt(fichier);
        ReadFunctionHook(fichier, &listview_aux->comparehook);
        ReadFunctionHook(fichier, &listview_aux->constructhook);
        ReadFunctionHook(fichier, &listview_aux->displayhook);
        ReadFunctionHook(fichier, &listview_aux->multitesthook);
        if (version >= 120)
        {
            ReadFunctionHook(fichier, &listview_aux->destructhook);
        }
        strcpy(listview_aux->format, LitChaine(fichier));
        strcpy(listview_aux->title, LitChaine(fichier));
        strcpy(listview_aux->content, LitChaine(fichier));
    }
    return (listview_aux);
}

void SaveListview(FILE *fichier, listview *listview_aux)
{
    SaveArea(fichier, &listview_aux->Area);
    fprintf(fichier, "%d\n", listview_aux->type);
    fprintf(fichier, "%d\n", listview_aux->multiselect);
    fprintf(fichier, "%d\n", listview_aux->doubleclick);
    fprintf(fichier, "%d\n", listview_aux->select);
    fprintf(fichier, "%d\n", listview_aux->adjustheight);
    fprintf(fichier, "%d\n", listview_aux->adjustwidth);
    fprintf(fichier, "%d\n", listview_aux->inputmode);
    fprintf(fichier, "%s\n", listview_aux->comparehook);
    fprintf(fichier, "%s\n", listview_aux->constructhook);
    fprintf(fichier, "%s\n", listview_aux->displayhook);
    fprintf(fichier, "%s\n", listview_aux->multitesthook);
    fprintf(fichier, "%s\n", listview_aux->destructhook);
    fprintf(fichier, "%s\n", listview_aux->format);
    fprintf(fichier, "%s\n", listview_aux->title);
    fprintf(fichier, "%s\n", listview_aux->content);
}

BOOL NewListView(listview *list, BOOL new)
{
    APTR WI_list, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR STR_label, STR_format;
    APTR RA_type, RA_select;
    APTR CH_multiselect, CH_doubleclick, CH_adjustheight;
    APTR CH_inputmode, CH_adjustwidth;
    APTR STR_comparehook, STR_constructhook, STR_displayhook;
    APTR STR_destructhook, STR_multitesthook, STR_title;
    APTR STR_content;
    APTR IM_comparehook, IM_constructhook, IM_displayhook;
    APTR IM_destructhook, IM_multitesthook;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    CONST_STRPTR aux;
    int active;
    CONST_STRPTR lv_type[4];
    CONST_STRPTR lv_select[4];
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[4];

    lv_type[0] = GetMUIBuilderString(MSG_StandardList);
    lv_type[1] = GetMUIBuilderString(MSG_FloattextList);
    lv_type[2] = GetMUIBuilderString(MSG_VolumeList);
    lv_type[3] = NULL;

    lv_select[0] = GetMUIBuilderString(MSG_NoEntry);
    lv_select[1] = GetMUIBuilderString(MSG_FirstEntry);
    lv_select[2] = GetMUIBuilderString(MSG_LastEntry);
    lv_select[3] = NULL;

    if (new)
    {
        InitListview(list);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&list->Area, FALSE, TRUE, TRUE, TRUE, TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Strings);
    RegisterTitles[2] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[3] = NULL;

    // *INDENT-OFF*
    WI_list = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ListChoice),
        MUIA_Window_ID, MAKE_ID('L', 'I', 'S', 'T'),
        MUIA_HelpNode, "List",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, ColGroup(4),
                        GroupFrame,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_DoubleClick)),
                        Child, CH_doubleclick = CheckMark(list->doubleclick),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_MultiSelection)),
                        Child, CH_multiselect = CheckMark(list->multiselect),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_InputMode)),
                        Child, CH_inputmode = CheckMark(list->inputmode),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_AdjustHeight)),
                        Child, CH_adjustheight = CheckMark(list->adjustheight),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_AdjustWidth)),
                        Child, CH_adjustwidth = CheckMark(list->adjustwidth),
                        Child, HVSpace,
                    End,
                    Child, HGroup,
                        Child, RA_type = Radio(GetMUIBuilderString(MSG_ListType), lv_type),
                        Child, RA_select = Radio(GetMUIBuilderString(MSG_Activate), lv_select),
                    End,
                End,
                Child, VGroup,
                    Child, ColGroup(3),
                        Child, Label(GetMUIBuilderString(MSG_ConstructHook)),
                        Child, STR_constructhook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, list->constructhook,
                        End,
                        Child, IM_constructhook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                        Child, Label(GetMUIBuilderString(MSG_DestructHook)),
                        Child, STR_destructhook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, list->destructhook,
                        End,
                        Child, IM_destructhook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                        Child, Label(GetMUIBuilderString(MSG_CompareHook)),
                        Child, STR_comparehook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, list->comparehook,
                        End,
                        Child, IM_comparehook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                        Child, Label(GetMUIBuilderString(MSG_DisplayHook)),
                        Child, STR_displayhook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, list->displayhook,
                        End,
                        Child, IM_displayhook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                        Child, Label(GetMUIBuilderString(MSG_MultiTestHook)),
                        Child, STR_multitesthook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, list->multitesthook,
                        End,
                        Child, IM_multitesthook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_ListIdent)),
                        Child, Label2(GetMUIBuilderString(MSG_Format)),
                        Child, STR_format = StringObject,
                            StringFrame,
                            MUIA_String_Contents, list->format,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_FloatText)),
                        Child, STR_content = StringObject,
                            StringFrame,
                            MUIA_Disabled, !(list->type == 1),
                            MUIA_String_MaxLen, 255,
                            MUIA_String_Contents, list->content,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, list->label,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Title)),
                        Child, STR_title = StringObject,
                            StringFrame,
                            MUIA_String_Contents,
                            list->title,
                        End,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, HGroup,
                MUIA_Group_SameSize, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_list;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_list, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(RA_type, MUIM_Notify, MUIA_Radio_Active, 1, STR_content, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(RA_type, MUIM_Notify, MUIA_Radio_Active, 0, STR_content, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(RA_type, MUIM_Notify, MUIA_Radio_Active, 2, STR_content, 3,
             MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(IM_constructhook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP1);
    DoMethod(IM_destructhook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP2);
    DoMethod(IM_comparehook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP3);
    DoMethod(IM_displayhook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP4);
    DoMethod(IM_multitesthook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP5);

    DoMethod(WI_list, MUIM_Window_SetCycleChain, RegGroup,
             CH_doubleclick, CH_multiselect, CH_inputmode, CH_adjustheight,
             CH_adjustwidth, RA_type, RA_select,
             IM_constructhook, IM_destructhook, IM_comparehook,
             IM_displayhook, IM_multitesthook, STR_format, STR_content,
             STR_label, STR_title, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, bt_ok,
             bt_cancel, NULL);

    set(RA_type, MUIA_Radio_Active, list->type);
    set(RA_select, MUIA_Radio_Active, list->select);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_list);
    set(WI_list, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_POPUP1:
                if (new)
                    GetMUIarg((object *) list,
                              (APTR *) & list->constructhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & list->constructhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (list->constructhook)
                    set(STR_constructhook, MUIA_Text_Contents,
                        list->constructhook);
                else
                    set(STR_constructhook, MUIA_Text_Contents, "");
                WI_current = WI_list;
                break;
            case ID_POPUP2:
                if (new)
                    GetMUIarg((object *) list,
                              (APTR *) & list->destructhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & list->destructhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (list->destructhook)
                    set(STR_destructhook, MUIA_Text_Contents,
                        list->destructhook);
                else
                    set(STR_destructhook, MUIA_Text_Contents, "");
                WI_current = WI_list;
                break;
            case ID_POPUP3:
                if (new)
                    GetMUIarg((object *) list,
                              (APTR *) & list->comparehook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & list->comparehook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (list->comparehook)
                    set(STR_comparehook, MUIA_Text_Contents,
                        list->comparehook);
                else
                    set(STR_comparehook, MUIA_Text_Contents, "");
                WI_current = WI_list;
                break;
            case ID_POPUP4:
                if (new)
                    GetMUIarg((object *) list,
                              (APTR *) & list->displayhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & list->displayhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (list->displayhook)
                    set(STR_displayhook, MUIA_Text_Contents,
                        list->displayhook);
                else
                    set(STR_displayhook, MUIA_Text_Contents, "");
                WI_current = WI_list;
                break;
            case ID_POPUP5:
                if (new)
                    GetMUIarg((object *) list,
                              (APTR *) & list->multitesthook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & list->multitesthook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (list->multitesthook)
                    set(STR_multitesthook, MUIA_Text_Contents,
                        list->multitesthook);
                else
                    set(STR_multitesthook, MUIA_Text_Contents, "");
                WI_current = WI_list;
                break;
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(list->label, aux);
                    aux = GetStr(STR_title);
                    strcpy(list->title, aux);
                    aux = GetStr(STR_format);
                    strcpy(list->format, aux);
                    aux = GetStr(STR_content);
                    strcpy(list->content, aux);
                    get(RA_type, MUIA_Radio_Active, &list->type);
                    get(RA_select, MUIA_Radio_Active, &list->select);
                    get(CH_doubleclick, MUIA_Selected, &active);
                    list->doubleclick = (active == 1);
                    get(CH_multiselect, MUIA_Selected, &active);
                    list->multiselect = (active == 1);
                    get(CH_adjustheight, MUIA_Selected, &active);
                    list->adjustheight = (active == 1);
                    get(CH_adjustwidth, MUIA_Selected, &active);
                    list->adjustwidth = (active == 1);
                    get(CH_inputmode, MUIA_Selected, &active);
                    list->inputmode = (active == 1);
                    ValidateArea(GR_GroupArea, &list->Area);
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
                EXIT_PRG();
                break;
        }
        if (signal)
            Wait(signal);
    }
    set(WI_list, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_list);
    MUI_DisposeObject(WI_list);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
