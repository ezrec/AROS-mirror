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

void InitColorField(colorfield *field_aux)
{
    InitArea(&field_aux->Area);
    sprintf(field_aux->label, "CF_label_%d", nb_colorfield);
    field_aux->fixheight = TRUE;
    field_aux->fixwidth = TRUE;
    field_aux->height = 16;
    field_aux->width = 20;
    field_aux->red = 0xffffffff;
    field_aux->green = 0xffffffff;
    field_aux->blue = 0xffffffff;
    field_aux->notify = create();
    field_aux->notifysource = create();
}

BOOL NewColorField(colorfield *field_aux, BOOL new)
{
    APTR WI_ColorField, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR STR_label, STR_height, STR_width;
    APTR CM_height, CM_width, colorinit;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    ULONG active;
    CONST_STRPTR aux;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitColorField(field_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&field_aux->Area, TRUE, FALSE, TRUE, TRUE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_ColorField = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ColorfieldChoices),
        MUIA_Window_ID, MAKE_ID('C', 'O', 'L', 'F'),
        MUIA_HelpNode, "Colorfield",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, HGroup,
                        Child, ColGroup(2),
                            GroupFrameT(GetMUIBuilderString(MSG_ColorfieldChoices)),
                            MUIA_Weight, 0,
                            Child, Label2(GetMUIBuilderString(MSG_FixHeight)),
                            Child, CM_height = CheckMark(field_aux->fixheight),
                            Child, Label2(GetMUIBuilderString(MSG_FixWidth)),
                            Child, CM_width = CheckMark(field_aux->fixwidth),
                            Child, HVSpace,
                            Child, HVSpace,
                        End,
                        Child, colorinit = ColoradjustObject,
                            MUIA_Coloradjust_Red, field_aux->red,
                            MUIA_Coloradjust_Green, field_aux->green,
                            MUIA_Coloradjust_Blue, field_aux->blue,
                        End,
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_Size)),
                        Child, Label1(GetMUIBuilderString(MSG_Height)),
                        Child, STR_height = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, field_aux->height,
                            End,
                        Child, Label1(GetMUIBuilderString(MSG_Width)),
                        Child, STR_width = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, field_aux->width,
                        End,
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_Indentification)),
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, field_aux->label,
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

    WI_current = WI_ColorField;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_ColorField, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             app, 2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, TRUE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, FALSE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, TRUE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, FALSE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_ColorField, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_height);
    DoMethod(STR_height, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_ColorField, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_width);
    DoMethod(STR_width, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_ColorField, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);

    DoMethod(WI_ColorField, MUIM_Window_SetCycleChain, RegGroup, CM_height,
             CM_width, STR_height, STR_width, STR_label,
             GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, bt_ok, bt_cancel, NULL);

    if (!field_aux->fixheight)
        set(STR_height, MUIA_Disabled, TRUE);
    if (!field_aux->fixwidth)
        set(STR_width, MUIA_Disabled, TRUE);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_ColorField);
    set(WI_ColorField, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if ((strlen(field_aux->label) > 0))
                {
                    strcpy(field_aux->label, aux);
                    get(CM_height, MUIA_Selected, &active);
                    field_aux->fixheight = (active == 1);
                    get(CM_width, MUIA_Selected, &active);
                    field_aux->fixwidth = (active == 1);
                    aux = GetStr(STR_height);
                    field_aux->height = atoi(aux);
                    aux = GetStr(STR_width);
                    field_aux->width = atoi(aux);
                    get(colorinit, MUIA_Coloradjust_Red, &field_aux->red);
                    get(colorinit, MUIA_Coloradjust_Green,
                        &field_aux->green);
                    get(colorinit, MUIA_Coloradjust_Blue,
                        &field_aux->blue);
                    ValidateArea(GR_GroupArea, &field_aux->Area);
                    if (new)
                    {
                        nb_colorfield++;
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
    set(WI_ColorField, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_ColorField);
    MUI_DisposeObject(WI_ColorField);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
