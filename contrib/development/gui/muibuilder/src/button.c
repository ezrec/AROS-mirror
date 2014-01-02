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

void InitButton(bouton *keybutton)
{
    sprintf(keybutton->label, "BT_label_%d", nb_button);
    strcpy(keybutton->title, "button title");
    InitArea(&keybutton->Area);
    keybutton->Area.key = 0;
    keybutton->notify = create();
    keybutton->notifysource = create();
    keybutton->Area.Background = MUII_ButtonBack;
    keybutton->Area.Frame = MUIV_Frame_Button;
}

BOOL NewButton(bouton *keybutton, BOOL new)
{
    APTR WI_button, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR STR_label, STR_title;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    CONST_STRPTR aux;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitButton(keybutton);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&keybutton->Area, FALSE, FALSE, FALSE, TRUE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_button = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ButtonChoices),
        MUIA_Window_ID, MAKE_ID('B', 'U', 'T', 'T'),
        MUIA_HelpNode, "Button",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, ColGroup(2),
                    GroupFrameT(GetMUIBuilderString(MSG_ButtonIdent)),
                    MUIA_Group_SameWidth, TRUE,
                    Child, Label2(GetMUIBuilderString(MSG_Label)),
                    Child, STR_label = StringObject,
                        StringFrame,
                        MUIA_String_Contents, keybutton->label,
                        MUIA_ExportID, 1,
                    End,
                    Child, Label2(GetMUIBuilderString(MSG_Title)),
                    Child, STR_title = StringObject,
                        StringFrame,
                        MUIA_String_Contents, keybutton->title,
                        MUIA_ExportID, 2,
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

    WI_current = WI_button;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_button, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_button, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_title);
    DoMethod(STR_title, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_button, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);

    DoMethod(WI_button, MUIM_Window_SetCycleChain, RegGroup, STR_label,
             STR_title, GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, bt_ok, bt_cancel, NULL);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_button);
    set(WI_button, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(keybutton->label, aux);
                    aux = GetStr(STR_title);
                    strcpy(keybutton->title, aux);
                    ValidateArea(GR_GroupArea, &keybutton->Area);
                    if (new)
                    {
                        nb_button++;
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
    set(WI_button, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_button);
    MUI_DisposeObject(WI_button);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
