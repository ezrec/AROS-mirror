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

void InitRectangle(rectangle *rect)
{
    InitArea(&rect->Area);
    rect->Area.Background = MUII_WindowBack;
    rect->Area.Frame = MUIV_Frame_None;
    sprintf(rect->label, "REC_label_%d", nb_rectangle);
    rect->height = 8;
    rect->width = 10;
    rect->fixheight = TRUE;
    rect->fixwidth = FALSE;
    rect->type = 1;
    rect->notify = create();
    rect->notifysource = create();
}

BOOL NewRectangle(rectangle *rect, BOOL new)
{
    APTR WI_rect, RegGroup, CH_fixheight, CH_fixwidth, STR_height;
    APTR STR_width, STR_label, BT_ok, BT_cancel, CY_type;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal, active;
    CONST_STRPTR aux;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];
    CONST_STRPTR STR_CY_type[4];

    STR_CY_type[0] = GetMUIBuilderString(MSG_Rectangle);
    STR_CY_type[1] = GetMUIBuilderString(MSG_HorizontalBar);
    STR_CY_type[2] = GetMUIBuilderString(MSG_VerticalBar);
    STR_CY_type[3] = NULL;

    if (new)
    {
        InitRectangle(rect);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&rect->Area, TRUE, TRUE, TRUE, TRUE, TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_rect = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_RectangleChoices),
        MUIA_Window_ID, MAKE_ID('R', 'E', 'C', 'T'),
        MUIA_HelpNode, "Rectangle",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, CY_type = CycleObject,
                        MUIA_Cycle_Entries, STR_CY_type,
                        MUIA_Cycle_Active, rect->type,
                    End,
                    Child, GroupObject,
                        MUIA_Group_Columns, 4,
                        Child, HVSpace,
                        Child, Label(GetMUIBuilderString(MSG_FixHeight)),
                        Child, CH_fixheight = CheckMark(rect->fixheight),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label(GetMUIBuilderString(MSG_FixWidth)),
                        Child, CH_fixwidth = CheckMark(rect->fixwidth),
                        Child, HVSpace,
                    End,
                    Child, GroupObject,
                        GroupFrameT(GetMUIBuilderString(MSG_Size)),
                        MUIA_Group_Columns, 2,
                        Child, Label(GetMUIBuilderString(MSG_Height)),
                        Child, STR_height = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "-0123456789",
                            MUIA_String_MaxLen, 80,
                            MUIA_String_Format, 0,
                            MUIA_String_Integer, rect->height,
                        End,
                        Child, Label(GetMUIBuilderString(MSG_Width)),
                        Child, STR_width = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "-0123456789",
                            MUIA_String_MaxLen, 80,
                            MUIA_String_Format, 0,
                            MUIA_String_Integer, rect->width,
                        End,
                    End,
                    Child, GroupObject,
                        GroupFrameT(GetMUIBuilderString(MSG_Indentification)),
                        MUIA_Group_Columns, 2,
                        Child, Label(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen, 80,
                            MUIA_String_Format, 0,
                            MUIA_String_Contents, rect->label,
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

    WI_current = WI_rect;

    DoMethod(BT_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_rect, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_rect, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_height);
    DoMethod(STR_height, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_rect, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_width);
    DoMethod(STR_width, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_rect, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);

    DoMethod(WI_rect, MUIM_Window_SetCycleChain, RegGroup, CY_type,
             CH_fixheight, CH_fixwidth, STR_height, STR_width, STR_label,
             GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, BT_ok, BT_cancel, NULL);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_rect);
    set(WI_rect, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if ((strlen(aux) > 0))
                {
                    strcpy(rect->label, aux);
                    aux = GetStr(STR_height);
                    rect->height = atoi(aux);
                    aux = GetStr(STR_width);
                    rect->width = atoi(aux);
                    get(CH_fixheight, MUIA_Selected, &active);
                    rect->fixheight = (active == 1);
                    get(CH_fixwidth, MUIA_Selected, &active);
                    rect->fixwidth = (active == 1);
                    get(CY_type, MUIA_Cycle_Active, &rect->type);
                    ValidateArea(GR_GroupArea, &rect->Area);
                    if (new)
                    {
                        nb_rectangle++;
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
    set(WI_rect, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_rect);
    MUI_DisposeObject(WI_rect);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
