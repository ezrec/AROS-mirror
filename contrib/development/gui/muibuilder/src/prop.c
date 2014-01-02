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

void InitProp(prop *prop_aux)
{
    InitArea(&prop_aux->Area);
    prop_aux->Area.Background = MUII_PropBack;
    prop_aux->Area.Frame = MUIV_Frame_Prop;
    prop_aux->horizontal = TRUE;
    prop_aux->entries = 100;
    sprintf(prop_aux->label, "PR_label_%d", nb_prop);
    prop_aux->first = 0;
    prop_aux->visible = 10;
    prop_aux->fixheight = TRUE;
    prop_aux->fixwidth = FALSE;
    prop_aux->height = 8;
    prop_aux->width = 8;
    prop_aux->notify = create();
    prop_aux->notifysource = create();
}

BOOL NewProp(prop *prop_aux, BOOL new)
{
    APTR WI_prop, RegGroup;
    APTR bt_cancel, bt_ok;
    ULONG signal;
    BOOL running = TRUE;
    BOOL result = FALSE;
    APTR STR_entries, STR_first, STR_visible, STR_label;
    APTR STR_height, STR_width;
    APTR CM_horiz, CM_width, CM_height;
    CONST_STRPTR aux;
    ULONG active;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitProp(prop_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&prop_aux->Area, TRUE, FALSE, TRUE, TRUE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_prop = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_PropChoices),
        MUIA_Window_ID, MAKE_ID('P', 'R', 'O', 'P'),
        MUIA_HelpNode, "Prop",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, ColGroup(4),
                        GroupFrameT(GetMUIBuilderString(MSG_Choices)),
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_HorizontalProp)),
                        Child, CM_horiz = CheckMark(prop_aux->horizontal),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_FixWidth)),
                        Child, CM_width = CheckMark(prop_aux->fixwidth),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_FixHeight)),
                        Child, CM_height = CheckMark(prop_aux->fixheight),
                        Child, HVSpace,
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_Attributes)),
                        MUIA_Group_SameWidth, TRUE,
                        Child, Label1(GetMUIBuilderString(MSG_Width)),
                        Child, STR_width = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, prop_aux->width,
                        End,
                        Child, Label1(GetMUIBuilderString(MSG_Height)),
                        Child, STR_height = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, prop_aux->height,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_EntriesNumber)),
                        Child, STR_entries = StringObject,
                            StringFrame, MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, prop_aux->entries,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_FirstNumber)),
                        Child, STR_first = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, prop_aux->first,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_VisibleNumber)),
                        Child, STR_visible = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, prop_aux->visible,
                        End,
                        Child, Label1(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, prop_aux->label,
                        End,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, HGroup,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_prop;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_prop, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);

    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, TRUE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, FALSE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, TRUE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, FALSE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(STR_entries, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_prop, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_first);
    DoMethod(STR_first, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_prop, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_visible);
    DoMethod(STR_visible, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_prop, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_prop, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_entries);

    DoMethod(WI_prop, MUIM_Window_SetCycleChain, RegGroup, CM_horiz,
             CM_width, CM_height, STR_width, STR_height, STR_entries,
             STR_first, STR_visible, STR_label, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, bt_ok,
             bt_cancel, NULL);

    if (!prop_aux->fixheight)
        set(STR_height, MUIA_Disabled, TRUE);
    if (!prop_aux->fixwidth)
        set(STR_width, MUIA_Disabled, TRUE);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_prop);
    set(WI_prop, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(prop_aux->label, aux);
                    aux = GetStr(STR_entries);
                    prop_aux->entries = atoi(aux);
                    get(CM_horiz, MUIA_Selected, &active);
                    prop_aux->horizontal = (active == 1);
                    get(CM_height, MUIA_Selected, &active);
                    prop_aux->fixheight = (active == 1);
                    get(CM_width, MUIA_Selected, &active);
                    prop_aux->fixwidth = (active == 1);
                    aux = GetStr(STR_first);
                    prop_aux->first = atoi(aux);
                    aux = GetStr(STR_visible);
                    prop_aux->visible = atoi(aux);
                    aux = GetStr(STR_height);
                    prop_aux->height = atoi(aux);
                    aux = GetStr(STR_width);
                    prop_aux->width = atoi(aux);
                    ValidateArea(GR_GroupArea, &prop_aux->Area);
                    if (new)
                    {
                        nb_prop++;
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
    set(WI_prop, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_prop);
    MUI_DisposeObject(WI_prop);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
