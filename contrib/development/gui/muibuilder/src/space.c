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

void InitSpace(space *space_aux)
{
    space_aux->notify = create();
    space_aux->notifysource = create();
    space_aux->type = 2;
    space_aux->spacing = 0;
    sprintf(space_aux->label, "Space_%d", nb_space);
}

BOOL NewSpace(space *space_aux, BOOL new)
{
    APTR WI_space;
    APTR bt_ok, bt_cancel;
    APTR RA_type, STR_label, STR_spacing;
    ULONG signal;
    BOOL running = TRUE;
    BOOL result = FALSE;
    CONST_STRPTR aux;
    CONST_STRPTR STR_RA_type[4];

    STR_RA_type[0] = GetMUIBuilderString(MSG_Horizontal);
    STR_RA_type[1] = GetMUIBuilderString(MSG_Vertical);
    STR_RA_type[2] = GetMUIBuilderString(MSG_HorizontalVertical);
    STR_RA_type[3] = NULL;

    if (new)
    {
        InitSpace(space_aux);
    }

    // *INDENT-OFF*
    WI_space = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_SpaceChoice),
        MUIA_Window_ID, MAKE_ID('S', 'P', 'A', 'C'),
        MUIA_HelpNode, "Space",
        WindowContents, VGroup,
            Child, VGroup,
                Child, ColGroup(3),
                    Child, HVSpace,
                    Child, RA_type = Radio(GetMUIBuilderString(MSG_Orientation), STR_RA_type),
                    Child, HVSpace,
                End,
                Child, ColGroup(2),
                Child, Label1(GetMUIBuilderString(MSG_Spacing)),
                Child, STR_spacing = StringObject,
                    StringFrame,
                    MUIA_String_Accept, "0123456879",
                    MUIA_String_Integer, space_aux->spacing,
                End,
                Child, Label1(GetMUIBuilderString(MSG_Label)),
                Child, STR_label = StringObject,
                    StringFrame,
                    MUIA_String_Contents, space_aux->label,
                    End,
                End,
            End,
            Child, HGroup,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_space;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_space, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(RA_type, MUIM_Notify, MUIA_Radio_Active, 0, STR_spacing, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(RA_type, MUIM_Notify, MUIA_Radio_Active, 1, STR_spacing, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(RA_type, MUIM_Notify, MUIA_Radio_Active, 2, STR_spacing, 3,
             MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(WI_space, MUIM_Window_SetCycleChain, RA_type, STR_spacing,
             STR_label, bt_ok, bt_cancel, NULL);

    set(RA_type, MUIA_Radio_Active, space_aux->type);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_space);
    set(WI_space, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(space_aux->label, aux);
                    aux = GetStr(STR_spacing);
                    space_aux->spacing = atoi(aux);
                    get(RA_type, MUIA_Radio_Active, &space_aux->type);
                    result = TRUE;
                    running = FALSE;
                    if (new)
                        nb_space++;
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
    set(WI_space, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_space);
    MUI_DisposeObject(WI_space);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);

}
