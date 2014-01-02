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

void InitSlider(slider *slider_aux)
{
    InitArea(&slider_aux->Area);
    slider_aux->Area.Background = MUII_WindowBack;
    slider_aux->Area.Frame = MUIV_Frame_Slider;
    slider_aux->title_exist = TRUE;
    strcpy(slider_aux->title, "Slider Title");
    sprintf(slider_aux->label, "SL_label_%d", nb_slider);
    slider_aux->quiet = FALSE;
    slider_aux->max = 100;
    slider_aux->min = 0;
    slider_aux->init = 0;
    slider_aux->reverse = FALSE;
    slider_aux->notify = create();
    slider_aux->notifysource = create();
    slider_aux->horizontal = TRUE;
}

BOOL NewSlider(slider *slider_aux, BOOL new)
{
    APTR WI_slider, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR STR_label, STR_title, STR_max, STR_min, STR_init;
    APTR CM_title, CM_quiet, CM_reverse, CM_horizontal;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    CONST_STRPTR aux;
    int active;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitSlider(slider_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&slider_aux->Area, TRUE, TRUE, TRUE, TRUE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_slider = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_SliderChoice),
        MUIA_Window_ID, MAKE_ID('S', 'L', 'I', 'D'),
        MUIA_HelpNode, "Slider",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, ColGroup(4),
                        Child, HVSpace,
                        Child, Label2(GetMUIBuilderString(MSG_SliderQuiet)),
                        Child, CM_quiet = CheckMark(slider_aux->quiet),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_UseTitleSlider)),
                        Child, CM_title = CheckMark(slider_aux->title_exist),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Reverse)),
                        Child, CM_reverse = CheckMark(slider_aux->reverse),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_Horizontal)),
                        Child, CM_horizontal = CheckMark(slider_aux->horizontal),
                        Child, HVSpace,
                    End,
                    Child, ColGroup(2),
                        Child, Label1(GetMUIBuilderString(MSG_Maximum)),
                        Child, STR_max = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "-0123456789",
                            MUIA_String_Integer, slider_aux->max,
                        End,
                        Child, Label1(GetMUIBuilderString(MSG_Minimum)),
                        Child, STR_min = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "-0123456789",
                            MUIA_String_Integer, slider_aux->min,
                        End,
                        Child, Label1(GetMUIBuilderString(MSG_Init)),
                        Child, STR_init = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, slider_aux->init,
                        End,
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_SliderIdent)),
                        MUIA_Group_SameWidth, TRUE,
                        Child, Label2(GetMUIBuilderString(MSG_Title)),
                        Child, STR_title = StringObject,
                            StringFrame,
                            MUIA_String_Contents, slider_aux->title,
                            MUIA_ExportID, 1,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, slider_aux->label,
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

    WI_current = WI_slider;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_slider, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(CM_title, MUIM_Notify, MUIA_Selected, TRUE, STR_title, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_title, MUIM_Notify, MUIA_Selected, FALSE, STR_title, 3,
             MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(STR_title, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_slider, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_slider, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_title);
    DoMethod(STR_max, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             WI_slider, 3, MUIM_Set, MUIA_Window_ActiveObject, STR_min);
    DoMethod(STR_min, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             WI_slider, 3, MUIM_Set, MUIA_Window_ActiveObject, STR_init);
    DoMethod(STR_init, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_slider, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_max);

    DoMethod(WI_slider, MUIM_Window_SetCycleChain, RegGroup, CM_quiet,
             CM_title, CM_reverse, CM_horizontal, STR_max, STR_min,
             STR_init, STR_title, STR_label, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, bt_ok,
             bt_cancel, NULL);

    set(STR_title, MUIA_Disabled, !slider_aux->title_exist);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_slider);
    set(WI_slider, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(slider_aux->label, aux);
                    aux = GetStr(STR_title);
                    strcpy(slider_aux->title, aux);
                    aux = GetStr(STR_max);
                    slider_aux->max = atoi(aux);
                    aux = GetStr(STR_min);
                    slider_aux->min = atoi(aux);
                    aux = GetStr(STR_init);
                    slider_aux->init = atoi(aux);
                    get(CM_title, MUIA_Selected, &active);
                    slider_aux->title_exist = (active == 1);
                    get(CM_quiet, MUIA_Selected, &active);
                    slider_aux->quiet = (active == 1);
                    get(CM_reverse, MUIA_Selected, &active);
                    slider_aux->reverse = (active == 1);
                    get(CM_horizontal, MUIA_Selected, &active);
                    slider_aux->horizontal = (active == 1);
                    ValidateArea(GR_GroupArea, &slider_aux->Area);
                    if (new)
                    {
                        nb_slider++;
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
    set(WI_slider, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_slider);
    MUI_DisposeObject(WI_slider);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
