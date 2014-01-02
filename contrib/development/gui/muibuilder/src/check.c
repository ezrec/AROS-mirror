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

void InitCheckMark(check *check_aux)
{
    InitArea(&check_aux->Area);
    check_aux->Area.Background = MUII_ButtonBack;
    check_aux->Area.Frame = MUIV_Frame_ImageButton;
    check_aux->title_exist = FALSE;
    strcpy(check_aux->title, "CheckMark Title");
    sprintf(check_aux->label, "CH_label_%d", nb_check);
    check_aux->notify = create();
    check_aux->notifysource = create();
    check_aux->init_state = FALSE;
}

BOOL NewCheckMark(check *check_aux, BOOL new)
{
    APTR WI_check, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR STR_label, STR_title;
    APTR CM_title, CM_state;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    CONST_STRPTR aux;
    int active;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitCheckMark(check_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&check_aux->Area, FALSE, TRUE, FALSE, TRUE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

   // *INDENT-OFF*

    WI_check = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_CheckMarkChoice),
        MUIA_Window_ID, MAKE_ID('C', 'H', 'E', 'C'),
        MUIA_HelpNode, "CheckMark",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, ColGroup(4),
                    Child, HVSpace,
                    Child, Label1(GetMUIBuilderString(MSG_UseTitle)),
                    Child, CM_title = CheckMark(check_aux->title_exist),
                    Child, HVSpace,
                    Child, HVSpace,
                    Child, Label1(GetMUIBuilderString(MSG_InitialState)),
                    Child, CM_state = CheckMark(check_aux->init_state),
                    Child, HVSpace,
                End,
                Child, ColGroup(2),
                    GroupFrameT(GetMUIBuilderString(MSG_CheckMarkIdent)),
                    Child, Label2(GetMUIBuilderString(MSG_Title)),
                    Child, STR_title = StringObject,
                        StringFrame,
                        MUIA_String_Contents, check_aux->title,
                        MUIA_ExportID, 1,
                    End,
                    Child, Label2(GetMUIBuilderString(MSG_Label)),
                    Child, STR_label = StringObject,
                        StringFrame,
                        MUIA_String_Contents, check_aux->label,
                        MUIA_ExportID, 1,
                    End,
                End,
            End,
            Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, HGroup,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_check;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_check, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(CM_title, MUIM_Notify, MUIA_Selected, TRUE, STR_title, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_title, MUIM_Notify, MUIA_Selected, FALSE, STR_title, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(STR_title, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_check, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_check, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_title);

    DoMethod(WI_check, MUIM_Window_SetCycleChain, RegGroup, CM_title,
             CM_state, STR_title, STR_label, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, bt_ok,
             bt_cancel, NULL);

    set(STR_title, MUIA_Disabled, !check_aux->title_exist);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_check);
    set(WI_check, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(check_aux->label, aux);
                    aux = GetStr(STR_title);
                    strcpy(check_aux->title, aux);
                    get(CM_title, MUIA_Selected, &active);
                    check_aux->title_exist = (active == 1);
                    get(CM_state, MUIA_Selected, &active);
                    check_aux->init_state = (active == 1);
                    ValidateArea(GR_GroupArea, &check_aux->Area);
                    if (new)
                    {
                        nb_check++;
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
    set(WI_check, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_check);
    MUI_DisposeObject(WI_check);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
