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

void InitLabel(label *label_aux)
{
    InitArea(&label_aux->Area);
    label_aux->Area.Weight = 0;
    label_aux->Area.Background = MUII_TextBack;
    label_aux->Area.Frame = MUIV_Frame_None;
    sprintf(label_aux->label, "LA_label_%d", nb_label);
    strcpy(label_aux->title, "label_title");
    label_aux->notify = create();
    label_aux->notifysource = create();
}

BOOL NewLabel(label *label_aux, BOOL new)
{
    APTR WI_label, RegGroup;
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
        InitLabel(label_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&label_aux->Area, FALSE, FALSE, FALSE, TRUE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_label = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_LabelChoice),
        MUIA_Window_ID, MAKE_ID('L', 'A', 'B', 'E'),
        MUIA_HelpNode, "Label",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, ColGroup(2),
                    GroupFrameT(GetMUIBuilderString(MSG_LabelIdent)),
                    Child, Label2(GetMUIBuilderString(MSG_Label)),
                    Child, STR_label = StringObject,
                        StringFrame,
                        MUIA_String_Contents, label_aux->label,
                        MUIA_ExportID, 1,
                    End,
                    Child, Label2(GetMUIBuilderString(MSG_Title)),
                    Child, STR_title = StringObject,
                        StringFrame,
                        MUIA_String_Contents, label_aux->title,
                        MUIA_ExportID, 1,
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

    WI_current = WI_label;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_label, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_label, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_title);
    DoMethod(STR_title, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_label, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);

    DoMethod(WI_label, MUIM_Window_SetCycleChain, RegGroup, STR_label,
             STR_title, GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, bt_ok, bt_cancel, NULL);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_label);
    set(WI_label, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if ((strlen(aux) > 0))
                {
                    strcpy(label_aux->label, aux);
                    aux = GetStr(STR_title);
                    strcpy(label_aux->title, aux);
                    ValidateArea(GR_GroupArea, &label_aux->Area);
                    if (new)
                    {
                        nb_label++;
                    }
                    result = TRUE;
                    running = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedLabelTitle));
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
    set(WI_label, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_label);
    MUI_DisposeObject(WI_label);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
