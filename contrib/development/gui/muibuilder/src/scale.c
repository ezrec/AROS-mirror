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

#include "builder.h"

void InitScale(scale *scale_aux)
{
    InitArea(&scale_aux->Area);
    scale_aux->Area.Background = MUII_WindowBack;
    scale_aux->Area.Frame = MUIV_Frame_None;
    sprintf(scale_aux->label, "Scale_%d", nb_scale);
    scale_aux->horiz = TRUE;
    scale_aux->notify = create();
    scale_aux->notifysource = create();
}

BOOL NewScale(scale *scale_aux, BOOL new)
{
    APTR WI_scale, RegGroup;
    APTR bt_ok, bt_cancel;
    APTR CM_horiz, STR_label;
    ULONG signal;
    BOOL running = TRUE;
    BOOL result = FALSE;
    int active;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];
    CONST_STRPTR aux;

    if (new)
    {
        InitScale(scale_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&scale_aux->Area, TRUE, TRUE, TRUE, FALSE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_scale = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ScaleTitle),
        MUIA_Window_ID, MAKE_ID('S', 'C', 'A', 'L'),
        MUIA_HelpNode, "Scale",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, ColGroup(4),
                        GroupFrameT(GetMUIBuilderString(MSG_Orientation)),
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Horizontal)),
                        Child, CM_horiz = CheckMark(scale_aux->horiz),
                        Child, HVSpace,
                    End,
                    Child, HGroup,
                        Child, Label1(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, scale_aux->label,
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

    WI_current = WI_scale;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_scale, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);

    DoMethod(WI_scale, MUIM_Window_SetCycleChain, RegGroup, CM_horiz,
             GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, bt_ok, bt_cancel, NULL);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_scale);
    set(WI_scale, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(scale_aux->label, aux);
                    get(CM_horiz, MUIA_Selected, &active);
                    scale_aux->horiz = (active == 1);
                    ValidateArea(GR_GroupArea, &scale_aux->Area);
                    result = TRUE;
                    running = FALSE;
                    if (new)
                        nb_scale++;
                }
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
    set(WI_scale, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_scale);
    MUI_DisposeObject(WI_scale);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
