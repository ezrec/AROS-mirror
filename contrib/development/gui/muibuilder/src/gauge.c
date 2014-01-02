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

void InitGauge(gauge *gauge_aux)
{
    InitArea(&gauge_aux->Area);
    gauge_aux->Area.Background = MUII_WindowBack;
    gauge_aux->Area.Frame = MUIV_Frame_Gauge;
    gauge_aux->divide = 0;
    gauge_aux->horizontal = TRUE;
    gauge_aux->max = 100;
    sprintf(gauge_aux->label, "GA_label_%d", nb_gauge);
    gauge_aux->infotext[0] = '\0';
    gauge_aux->height = 10;
    gauge_aux->width = 15;
    gauge_aux->fixheight = TRUE;
    gauge_aux->fixwidth = FALSE;
    gauge_aux->notify = create();
    gauge_aux->notifysource = create();
}

BOOL NewGauge(gauge *gauge_aux, BOOL new)
{
    APTR WI_gauge, RegGroup;
    APTR bt_cancel, bt_ok;
    ULONG signal;
    BOOL running = TRUE;
    BOOL result = FALSE;
    APTR STR_max, STR_height, STR_width, STR_label, STR_divide,
        STR_InfoText;
    APTR CM_horiz, CM_height, CM_width;
    CONST_STRPTR aux;
    ULONG active;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitGauge(gauge_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&gauge_aux->Area, FALSE, FALSE, FALSE, FALSE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_gauge = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_GaugeChoices),
        MUIA_Window_ID, MAKE_ID('G', 'A', 'U', 'G'),
        MUIA_HelpNode, "Gauge",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, ColGroup(4),
                        GroupFrameT(GetMUIBuilderString(MSG_Choices)),
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_HorizGauge)),
                        Child, CM_horiz = CheckMark(gauge_aux->horizontal),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_FixHeight)),
                        Child, CM_height = CheckMark(gauge_aux->fixheight),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_FixWidth)),
                        Child, CM_width = CheckMark(gauge_aux->fixwidth),
                        Child, HVSpace,
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_Attributes)),
                        MUIA_Group_SameWidth, TRUE,
                        Child, Label2(GetMUIBuilderString(MSG_Maximum)),
                        Child, STR_max = StringObject,
                            StringFrame, MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, gauge_aux->max,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Divide)),
                        Child, STR_divide = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, gauge_aux->divide,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Height)),
                        Child, STR_height = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, gauge_aux->height,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Width)),
                        Child, STR_width = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, gauge_aux->width,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Infotext)),
                        Child, STR_InfoText = StringObject,
                            StringFrame,
                            MUIA_String_Contents, gauge_aux->infotext,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, gauge_aux->label,
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

    WI_current = WI_gauge;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_gauge, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, TRUE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, FALSE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, TRUE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, FALSE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(STR_max, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             WI_gauge, 3, MUIM_Set, MUIA_Window_ActiveObject, STR_divide);
    DoMethod(STR_divide, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_gauge, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_height);
    DoMethod(STR_height, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_gauge, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_width);
    DoMethod(STR_width, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_gauge, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_InfoText);
    DoMethod(STR_InfoText, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_gauge, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_gauge, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_max);


    if (!gauge_aux->fixheight)
        set(STR_height, MUIA_Disabled, TRUE);
    if (!gauge_aux->fixwidth)
        set(STR_width, MUIA_Disabled, TRUE);

    DoMethod(WI_gauge, MUIM_Window_SetCycleChain, RegGroup, CM_horiz,
             CM_height, CM_width, STR_max, STR_divide, STR_height,
             STR_width, STR_InfoText, STR_label, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, bt_ok,
             bt_cancel, NULL);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_gauge);
    set(WI_gauge, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if ((strlen(aux) > 0))
                {
                    strcpy(gauge_aux->label, aux);
                    aux = GetStr(STR_max);
                    gauge_aux->max = atoi(aux);
                    get(CM_horiz, MUIA_Selected, &active);
                    gauge_aux->horizontal = (active == 1);
                    get(CM_height, MUIA_Selected, &active);
                    gauge_aux->fixheight = (active == 1);
                    get(CM_width, MUIA_Selected, &active);
                    gauge_aux->fixwidth = (active == 1);
                    aux = GetStr(STR_height);
                    gauge_aux->height = atoi(aux);
                    aux = GetStr(STR_divide);
                    gauge_aux->divide = atoi(aux);
                    aux = GetStr(STR_width);
                    gauge_aux->width = atoi(aux);
                    strcpy(gauge_aux->infotext, GetStr(STR_InfoText));
                    ValidateArea(GR_GroupArea, &gauge_aux->Area);
                    if (new)
                    {
                        nb_gauge++;
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
    set(WI_gauge, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_gauge);
    MUI_DisposeObject(WI_gauge);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
