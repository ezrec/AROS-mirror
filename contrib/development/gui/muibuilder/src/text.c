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

void InitText(texte * text_aux)
{
    InitArea(&text_aux->Area);
    text_aux->Area.Background = MUII_TextBack;
    text_aux->Area.Frame = MUIV_Frame_Text;
    sprintf(text_aux->label, "TX_label_%d", nb_text);
    text_aux->preparse[0] = '\0';
    text_aux->content[0] = '\0';
    text_aux->max = FALSE;
    text_aux->min = TRUE;
    text_aux->notify = create();
    text_aux->notifysource = create();
}

APTR LoadText(FILE *fichier, APTR father, int version)
{
    texte *text_aux;

    text_aux = AllocVec(sizeof(texte), MEMF_PUBLIC | MEMF_CLEAR);
    InitText(text_aux);
    text_aux->id = TY_TEXT;
    text_aux->father = father;
    strcpy(text_aux->label, LitChaine(fichier));
    text_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, text_aux);
    if (version > 104)
        ReadNotify(fichier, text_aux);
    if (version < 113)
    {
        text_aux->Area.Background = ReadInt(fichier);
        text_aux->Area.Frame = ReadInt(fichier);
    }
    if (version >= 113)
    {
        ReadArea(fichier, &text_aux->Area);
        strcpy(text_aux->preparse, LitChaine(fichier));
    }
    text_aux->max = ReadInt(fichier);
    text_aux->min = ReadInt(fichier);
    strcpy(text_aux->content, LitChaine(fichier));
    return (text_aux);
}

void SaveText(FILE *fichier, texte * text_aux)
{
    SaveArea(fichier, &text_aux->Area); /* 113 */
    fprintf(fichier, "%s\n", text_aux->preparse);       /* 113 */
    fprintf(fichier, "%d\n", text_aux->max);
    fprintf(fichier, "%d\n", text_aux->min);
    fprintf(fichier, "%s\n", text_aux->content);
}

BOOL NewText(texte * text_aux, BOOL new)
{
    ULONG signal, active;
    BOOL running = TRUE;
    BOOL result = FALSE;
    APTR WI_text, CH_max, CH_min, RegGroup;
    APTR STR_content, STR_label, STR_preparse, bt_ok, bt_cancel;
    CONST_STRPTR aux;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitText(text_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&text_aux->Area, TRUE, TRUE, TRUE, TRUE,
                            TRUE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_text = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_TextAttributes),
        MUIA_Window_ID, MAKE_ID('B', 'U', 'T', 'T'),
        MUIA_HelpNode, "Text",
        WindowContents, GroupObject,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, GroupObject,
                    Child, GroupObject,
                        GroupFrameT(GetMUIBuilderString(MSG_Attributes)),
                        MUIA_Group_Columns, 4,
                        Child, HVSpace,
                        Child, TextObject,
                            MUIA_Text_PreParse, "\33r",
                            MUIA_Text_Contents, GetMUIBuilderString(MSG_TextSetMax),
                            MUIA_Weight, 0,
                            MUIA_InnerLeft, 0,
                            MUIA_InnerRight, 0,
                        End,
                        Child, CH_max = CheckMark(FALSE),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, TextObject,
                            MUIA_Text_PreParse, "\33r",
                            MUIA_Text_Contents, GetMUIBuilderString(MSG_TextSetMin),
                            MUIA_Weight, 0,
                            MUIA_InnerLeft, 0,
                            MUIA_InnerRight, 0,
                        End,
                        Child, CH_min = CheckMark(FALSE),
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, HVSpace,
                        Child, HVSpace,
                    End,
                    Child, ColGroup(2),
                        Child, Label2(GetMUIBuilderString(MSG_Preparse)),
                        Child, STR_preparse = StringObject,
                            StringFrame,
                            MUIA_String_Contents, text_aux->preparse,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_TextContent)),
                        Child, STR_content = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen, 255,
                            MUIA_String_Contents,
                            text_aux->content,
                        End,
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, text_aux->label,
                        End,
                    End,
                End,
                Child, GR_GroupArea->GR_AreaGroup,
            End,
            Child, GroupObject,
                MUIA_Group_Horiz, 1,
                MUIA_Group_SameWidth, TRUE,
                Child, bt_ok =  MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Ok)),
                Child, bt_cancel = MUI_MakeObject(MUIO_Button, GetMUIBuilderString(MSG_Cancel)),
                End,
        End,
    End;
    // *INDENT-ON*

    WI_current = WI_text;

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_text, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(STR_content, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_text, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);
    DoMethod(STR_preparse, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_text, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_content);
    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_text, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_preparse);

    DoMethod(WI_text, MUIM_Window_SetCycleChain, RegGroup,
             CH_max, CH_min, STR_preparse, STR_content, STR_label,
             bt_ok, bt_cancel,
             GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, NULL);

    set(CH_max, MUIA_Selected, text_aux->max);
    set(CH_min, MUIA_Selected, text_aux->min);
    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_text);
    set(WI_text, MUIA_Window_Open, TRUE);

    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if (strlen(aux) > 0)
                {
                    strcpy(text_aux->label, aux);
                    aux = GetStr(STR_content);
                    strcpy(text_aux->content, aux);
                    aux = GetStr(STR_preparse);
                    strcpy(text_aux->preparse, aux);
                    get(CH_max, MUIA_Selected, &active);
                    text_aux->max = (active == 1);
                    get(CH_min, MUIA_Selected, &active);
                    text_aux->min = (active == 1);
                    ValidateArea(GR_GroupArea, &text_aux->Area);
                    if (new)
                    {
                        nb_text++;
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
    set(WI_text, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_text);
    MUI_DisposeObject(WI_text);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
