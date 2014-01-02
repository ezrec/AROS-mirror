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

void InitString(chaine *chaine_aux)
{
    InitArea(&chaine_aux->Area);
    chaine_aux->Area.Background = MUII_WindowBack;
    chaine_aux->Area.Frame = MUIV_Frame_String;
    chaine_aux->title_exist = FALSE;
    sprintf(chaine_aux->label, "STR_label_%d", nb_string);
    strcpy(chaine_aux->title, "string title");
    strcpy(chaine_aux->content, "");
    chaine_aux->accept[0] = '\0';
    chaine_aux->reject[0] = '\0';
    chaine_aux->maxlen = 80;
    chaine_aux->secret = FALSE;
    chaine_aux->format = 0;
    chaine_aux->integer = FALSE;
    chaine_aux->notify = create();
    chaine_aux->notifysource = create();
}

BOOL NewString(chaine *chaine_aux, BOOL new)
{
    APTR WI_string, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR STR_label, STR_title, STR_content;
    APTR STR_accept, STR_reject, STR_maxlen;
    APTR CM_title, CM_secret, CM_integer;
    APTR CY_format;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal;
    CONST_STRPTR aux;
    int active;
    CONST_STRPTR STR_CY_format[4];
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    STR_CY_format[0] = GetMUIBuilderString(MSG_Left);
    STR_CY_format[1] = GetMUIBuilderString(MSG_Center);
    STR_CY_format[2] = GetMUIBuilderString(MSG_Right);
    STR_CY_format[3] = NULL;

    if (new)
    {
        InitString(chaine_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&chaine_aux->Area, FALSE, FALSE, TRUE, TRUE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_string = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_StringChoice),
        MUIA_Window_ID, MAKE_ID('L', 'I', 'S', 'T'),
        MUIA_HelpNode, "Label",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, ColGroup(4),
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_UseTitle)),
                        Child, CM_title = CheckMark(chaine_aux->title_exist),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_SecretString)),
                        Child, CM_secret = CheckMark(chaine_aux->secret),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, Label1(GetMUIBuilderString(MSG_IntegerString)),
                        Child, CM_integer = CheckMark(chaine_aux->integer),
                        Child, HVSpace,
                    End,
                    Child, CY_format = Cycle(STR_CY_format),
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_StringIdent)),
                        MUIA_Group_SameWidth, TRUE,
                        Child, Label2(GetMUIBuilderString(MSG_Title)),
                        Child, STR_title = StringObject,
                            StringFrame,
                            MUIA_String_Contents, chaine_aux->title,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, chaine_aux->label,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Content)),
                        Child, STR_content = StringObject,
                            StringFrame,
                            MUIA_String_Contents, chaine_aux->content,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Accept)),
                        Child, STR_accept = StringObject,
                            StringFrame,
                            MUIA_String_Contents, chaine_aux->accept,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Reject)),
                        Child, STR_reject = StringObject,
                            StringFrame,
                            MUIA_String_Contents, chaine_aux->reject,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_MaxLen)),
                        Child, STR_maxlen = StringObject,
                            StringFrame,
                            MUIA_String_Integer, chaine_aux->maxlen,
                            MUIA_String_Accept, "0123456789",
                        End,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, HGroup,
                MUIA_Group_SameSize, TRUE,
                Child, bt_ok = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
            End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_string;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_string, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
             2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(CM_title, MUIM_Notify, MUIA_Selected, TRUE, STR_title, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_title, MUIM_Notify, MUIA_Selected, FALSE, STR_title, 3,
             MUIM_Set, MUIA_Disabled, TRUE);
    DoMethod(STR_title, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_string, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_string, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_content);
    DoMethod(STR_content, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_string, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_accept);
    DoMethod(STR_accept, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_string, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_reject);
    DoMethod(STR_reject, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_string, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_maxlen);
    DoMethod(STR_maxlen, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_string, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_title);

    DoMethod(WI_string, MUIM_Window_SetCycleChain, RegGroup, CM_title,
             CM_secret, CM_integer, CY_format, STR_title, STR_label,
             STR_content, STR_accept, STR_reject, STR_maxlen, bt_ok,
             bt_cancel, GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, NULL);

    set(CY_format, MUIA_Cycle_Active, chaine_aux->format);
    set(STR_title, MUIA_Disabled, !chaine_aux->title_exist);
    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_string);
    set(WI_string, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(chaine_aux->label, aux);
                    aux = GetStr(STR_title);
                    strcpy(chaine_aux->title, aux);
                    aux = GetStr(STR_content);
                    strcpy(chaine_aux->content, aux);
                    aux = GetStr(STR_accept);
                    strcpy(chaine_aux->accept, aux);
                    aux = GetStr(STR_reject);
                    strcpy(chaine_aux->reject, aux);
                    aux = GetStr(STR_maxlen);
                    chaine_aux->maxlen = atoi(aux);
                    get(CM_title, MUIA_Selected, &active);
                    chaine_aux->title_exist = (active == 1);
                    get(CM_integer, MUIA_Selected, &active);
                    chaine_aux->integer = (active == 1);
                    get(CM_secret, MUIA_Selected, &active);
                    chaine_aux->secret = (active == 1);
                    get(CY_format, MUIA_Cycle_Active, &chaine_aux->format);
                    if ((chaine_aux->integer) && new)
                        strcpy(chaine_aux->accept, "-0123456789");
                    ValidateArea(GR_GroupArea, &chaine_aux->Area);
                    if (new)
                    {
                        nb_string++;
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
    set(WI_string, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_string);
    MUI_DisposeObject(WI_string);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
