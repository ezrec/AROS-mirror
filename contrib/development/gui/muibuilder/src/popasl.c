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

void InitPopAsl(popaslobj *popasl)
{
    InitArea(&popasl->Area);
    popasl->Area.Background = MUII_WindowBack;
    popasl->Area.Frame = MUIV_Frame_None;
    sprintf(popasl->label, "PA_label_%d", nb_popasl);
    popasl->starthook = NULL;
    popasl->stophook = NULL;
    popasl->type = 0;
    popasl->image = 0;
    popasl->notify = create();
    popasl->notifysource = create();
}

APTR LoadPopAsl(FILE *fichier, APTR father, int version)
{
    popaslobj *popasl;

    popasl = AllocVec(sizeof(popaslobj), MEMF_PUBLIC | MEMF_CLEAR);
    InitPopAsl(popasl);
    popasl->id = TY_POPASL;
    popasl->father = father;
    strcpy(popasl->label, LitChaine(fichier));
    popasl->generated = ReadInt(fichier);
    ReadHelp(fichier, popasl);
    ReadNotify(fichier, popasl);
    if (version >= 113)
        ReadArea(fichier, &popasl->Area);
    ReadFunctionHook(fichier, &popasl->starthook);
    ReadFunctionHook(fichier, &popasl->stophook);
    popasl->type = ReadInt(fichier);
    popasl->image = ReadInt(fichier);
    return (popasl);
}

void SavePopAsl(FILE *fichier, popaslobj *popasl)
{
    SaveArea(fichier, &popasl->Area);
    fprintf(fichier, "%s\n", popasl->starthook);
    fprintf(fichier, "%s\n", popasl->stophook);
    fprintf(fichier, "%d\n", popasl->type);
    fprintf(fichier, "%d\n", popasl->image);
}

BOOL NewPopAsl(popaslobj *popasl, BOOL new)
{

    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    CONST_STRPTR aux;
    APTR WI_PopAsl, RegGroup, RA_type, STR_starthook, STR_stophook;
    APTR IM_starthook, IM_stophook, STR_label, BT_ok;
    APTR BT_cancel, LV_images;
    CONST_STRPTR STR_RA_type[4];
    int i;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];
    CONST_STRPTR STR_images[3];

    STR_images[0] = GetMUIBuilderString(MSG_PopUpImage);
    STR_images[1] = GetMUIBuilderString(MSG_PopFileImage);
    STR_images[2] = GetMUIBuilderString(MSG_PopDrawerImage);

    STR_RA_type[0] = GetMUIBuilderString(MSG_FileRequester);
    STR_RA_type[1] = GetMUIBuilderString(MSG_FontRequester);
    STR_RA_type[2] = GetMUIBuilderString(MSG_ScreenModeRequester);
    STR_RA_type[3] = NULL;

    if (new)
    {
        InitPopAsl(popasl);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&popasl->Area, FALSE, TRUE, TRUE, TRUE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_PopAsl = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_PopAslChoices),
        MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
        MUIA_HelpNode, "PopAsl",
        WindowContents, GroupObject,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, GroupObject,
                        MUIA_Group_Horiz, TRUE,
                        Child, LV_images = ListviewObject,
                            MUIA_Listview_List, ListObject,
                                InputListFrame,
                            End,
                        End,
                        Child, RA_type = RadioObject,
                            GroupFrameT(GetMUIBuilderString(MSG_RequesterType)),
                            MUIA_Radio_Entries, STR_RA_type,
                        End,
                    End,
                    Child, GroupObject,
                        GroupFrameT(GetMUIBuilderString(MSG_CallHook)),
                        MUIA_Group_Columns, 3,
                        Child, Label(GetMUIBuilderString(MSG_StartHook)),
                        Child, STR_starthook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, popasl->starthook,
                        End,
                        Child, IM_starthook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                        Child, Label(GetMUIBuilderString(MSG_StopHook)),
                        Child, STR_stophook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, popasl->stophook,
                        End,
                        Child, IM_stophook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                    End,
                    Child, GroupObject,
                        GroupFrameT(GetMUIBuilderString(MSG_Indentification)),
                        MUIA_Group_Horiz, TRUE,
                        Child, Label(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen, 80,
                            MUIA_String_Format, 0,
                            MUIA_String_Contents, popasl->label,
                        End,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, GroupObject,
            MUIA_Group_Horiz, TRUE,
            MUIA_Group_SameWidth, TRUE,
                Child, BT_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, BT_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_PopAsl;

    DoMethod(BT_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_PopAsl, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_PopAsl, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_starthook);
    DoMethod(STR_starthook, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_PopAsl, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_stophook);
    DoMethod(STR_stophook, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_PopAsl, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(IM_starthook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP1);
    DoMethod(IM_stophook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP2);

    DoMethod(WI_PopAsl, MUIM_Window_SetCycleChain, RegGroup, LV_images,
             RA_type, IM_starthook, IM_stophook, STR_label,
             GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, BT_ok, BT_cancel, NULL);

    set(RA_type, MUIA_Radio_Active, popasl->type);

    for (i = 0; i < 3; i++)
    {
        aux = STR_images[i];
        DoMethod(LV_images, MUIM_List_Insert, &aux, 1,
                 MUIV_List_Insert_Bottom);
    }
    set(LV_images, MUIA_List_Active, popasl->image);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_PopAsl);
    set(WI_PopAsl, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_POPUP1:
                if (new)
                    GetMUIarg((object *) popasl,
                              (APTR *) & popasl->starthook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & popasl->starthook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (popasl->starthook)
                    set(STR_starthook, MUIA_Text_Contents,
                        popasl->starthook);
                else
                    set(STR_starthook, MUIA_Text_Contents, "");
                WI_current = WI_PopAsl;
                break;
            case ID_POPUP2:
                if (new)
                    GetMUIarg((object *) popasl,
                              (APTR *) & popasl->stophook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & popasl->stophook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (popasl->stophook)
                    set(STR_stophook, MUIA_Text_Contents,
                        popasl->stophook);
                else
                    set(STR_stophook, MUIA_Text_Contents, "");
                WI_current = WI_PopAsl;
                break;
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(popasl->label, aux);
                    get(RA_type, MUIA_Radio_Active, &popasl->type);
                    get(LV_images, MUIA_List_Active, &popasl->image);
                    ValidateArea(GR_GroupArea, &popasl->Area);
                    if (new)
                    {
                        nb_popasl++;
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
    set(WI_PopAsl, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_PopAsl);
    MUI_DisposeObject(WI_PopAsl);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
