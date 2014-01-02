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

#define Image(nr) ImageObject, \
    MUIA_Image_Spec, nr, \
    MUIA_Image_FreeHoriz, TRUE, \
    MUIA_Image_FreeVert, TRUE, \
    MUIA_FixHeight, 10, \
    MUIA_FixWidth, 15, \
    MUIA_InputMode, MUIV_InputMode_RelVerify, \
End

void InitImage(image *image_aux)
{
    InitArea(&image_aux->Area);
    image_aux->Area.Frame = MUIV_Frame_ImageButton;
    sprintf(image_aux->label, "IM_label_%d", nb_image);
    image_aux->type = -1;
    image_aux->freehoriz = TRUE;
    image_aux->freevert = TRUE;
    image_aux->fixheight = TRUE;
    image_aux->fixwidth = TRUE;
    image_aux->height = 10;
    image_aux->width = 8;
    image_aux->notify = create();
    image_aux->notifysource = create();
    image_aux->spec[0] = '\0';
}

APTR LoadImage(FILE *fichier, APTR father, int version)
{
    image *image_aux;

    image_aux = AllocVec(sizeof(image), MEMF_PUBLIC | MEMF_CLEAR);
    InitImage(image_aux);
    image_aux->id = TY_IMAGE;
    image_aux->father = father;
    strcpy(image_aux->label, LitChaine(fichier));
    image_aux->generated = ReadInt(fichier);
    ReadHelp(fichier, image_aux);
    if (version > 104)
        ReadNotify(fichier, image_aux);
    if (version >= 113)
        ReadArea(fichier, &image_aux->Area);
    image_aux->type = ReadInt(fichier);
    if (version < 113)
        image_aux->Area.InputMode = ReadInt(fichier);
    image_aux->freevert = ReadInt(fichier);
    image_aux->freehoriz = ReadInt(fichier);
    image_aux->fixheight = ReadInt(fichier);
    image_aux->fixwidth = ReadInt(fichier);
    image_aux->height = ReadInt(fichier);
    image_aux->width = ReadInt(fichier);
    if (version < 113)
        image_aux->Area.Frame = ReadInt(fichier) * MUIV_Frame_ImageButton;
    if (version > 122)
        strcpy(image_aux->spec, LitChaine(fichier));
    return (image_aux);
}

void SaveImage(FILE *fichier, image *image_aux)
{
    SaveArea(fichier, &image_aux->Area);
    fprintf(fichier, "%d\n", image_aux->type);
    fprintf(fichier, "%d\n", image_aux->freevert);
    fprintf(fichier, "%d\n", image_aux->freehoriz);
    fprintf(fichier, "%d\n", image_aux->fixheight);
    fprintf(fichier, "%d\n", image_aux->fixwidth);
    fprintf(fichier, "%d\n", image_aux->height);
    fprintf(fichier, "%d\n", image_aux->width);
    fprintf(fichier, "%s\n", image_aux->spec);
}

BOOL NewImage(image *image_aux, BOOL new)
{
    APTR WI_image, RegGroup;
    APTR bt_cancel, bt_ok;
    APTR STR_label, STR_height, STR_width, PA_spec;
    APTR CM_height, CM_width, CM_horiz, CM_vert, STR_spec;
    APTR images[22];
    BOOL running = TRUE, bool_aux;
    BOOL result = FALSE;
    ULONG signal;
    ULONG active;
    CONST_STRPTR aux;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];

    if (new)
    {
        InitImage(image_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&image_aux->Area, TRUE, FALSE, TRUE, TRUE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_image = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_ImageChoice),
        MUIA_Window_ID, MAKE_ID('I', 'M', 'A', 'G'),
        MUIA_HelpNode, "Image",
        WindowContents, VGroup,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, HGroup,
                        Child, HVSpace,
                        Child, ColGroup(5),
                            GroupFrameT(GetMUIBuilderString(MSG_Images)),
                            Child, images[1] = Image(MUII_ArrowUp),
                            Child, images[2] = Image(MUII_ArrowDown),
                            Child, images[3] = Image(MUII_ArrowLeft),
                            Child, images[4] = Image(MUII_ArrowRight),
                            Child, images[5] = Image(MUII_RadioButton),
                            Child, images[6] = Image(MUII_Cycle),
                            Child, images[7] = Image(MUII_PopUp),
                            Child, images[8] = Image(MUII_PopFile),
                            Child, images[9] = Image(MUII_PopDrawer),
                            Child, images[10] = Image(MUII_Drawer),
                            Child, images[11] = Image(MUII_HardDisk),
                            Child, images[12] = Image(MUII_Disk),
                            Child, images[13] = Image(MUII_Chip),
                            Child, images[14] = Image(MUII_Volume),
                            Child, images[15] = Image(MUII_Network),
                            Child, images[16] = Image(MUII_Assign),
                            Child, images[17] = Image(MUII_TapePlay),
                            Child, images[18] = Image(MUII_TapePlayBack),
                            Child, images[19] = Image(MUII_TapePause),
                            Child, images[20] = Image(MUII_TapeStop),
                            Child, images[21] = Image(MUII_TapeRecord),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, HVSpace,
                        End,
                        Child, HVSpace,
                        Child, ColGroup(2),
                            GroupFrameT(GetMUIBuilderString(MSG_Choices)),
                            Child, Label2(GetMUIBuilderString(MSG_FreeVertical)),
                            Child, CM_vert = CheckMark(image_aux->freevert),
                            Child, Label2(GetMUIBuilderString(MSG_FreeHorizontal)),
                            Child, CM_horiz = CheckMark(image_aux->freehoriz),
                            Child, Label2(GetMUIBuilderString(MSG_FixHeight)),
                            Child, CM_height = CheckMark(image_aux->fixheight),
                            Child, Label2(GetMUIBuilderString(MSG_FixWidth)),
                            Child, CM_width = CheckMark(image_aux->fixwidth),
                            Child, HVSpace,
                            Child, HVSpace,
                        End,
                        Child, HVSpace,
                    End,
                    Child, PA_spec = PopaslObject,
                        MUIA_Popasl_Type, ASL_FileRequest,
                        MUIA_Popstring_String, STR_spec = String(image_aux->spec, 128),
                        MUIA_Popstring_Button, PopButton(MUII_PopFile),
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_Size)),
                        Child, Label1(GetMUIBuilderString(MSG_Height)),
                        Child, STR_height = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, image_aux->height,
                        End,
                        Child, Label1(GetMUIBuilderString(MSG_Width)),
                        Child, STR_width = StringObject,
                            StringFrame,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, image_aux->width,
                        End,
                    End,
                    Child, ColGroup(2),
                        GroupFrameT(GetMUIBuilderString(MSG_ImageIdent)),
                        Child, Label2(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_Contents, image_aux->label,
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

    WI_current = WI_image;

    DoMethod(images[1], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 11);
    DoMethod(images[2], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 12);
    DoMethod(images[3], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 13);
    DoMethod(images[4], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 14);
    DoMethod(images[5], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 15);
    DoMethod(images[6], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 16);
    DoMethod(images[7], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 17);
    DoMethod(images[8], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 18);
    DoMethod(images[9], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 19);
    DoMethod(images[10], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 20);
    DoMethod(images[11], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 21);
    DoMethod(images[12], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 22);
    DoMethod(images[13], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 23);
    DoMethod(images[14], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 24);
    DoMethod(images[15], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 25);
    DoMethod(images[16], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 26);
    DoMethod(images[17], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 27);
    DoMethod(images[18], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 28);
    DoMethod(images[19], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 29);
    DoMethod(images[20], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 30);
    DoMethod(images[21], MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, 31);
    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_image, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);

    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, TRUE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_height, MUIM_Notify, MUIA_Selected, FALSE, STR_height, 3,
             MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, TRUE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, FALSE);
    DoMethod(CM_width, MUIM_Notify, MUIA_Selected, FALSE, STR_width, 3,
             MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(STR_label, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_image, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_height);
    DoMethod(STR_height, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_image, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_width);
    DoMethod(STR_width, MUIM_Notify, MUIA_String_Acknowledge,
             MUIV_EveryTime, WI_image, 3, MUIM_Set,
             MUIA_Window_ActiveObject, STR_label);

    DoMethod(WI_image, MUIM_Window_SetCycleChain, RegGroup,
             images[1], images[2], images[3], images[4], images[5],
             images[6], images[7], images[8], images[9], images[10],
             images[11], images[12], images[13], images[14], images[15],
             images[16], images[17], images[18], images[19], images[20],
             images[21], CM_vert, CM_horiz, CM_height, CM_width, PA_spec,
             STR_height, STR_width, STR_label, GR_GroupArea->CH_Hide,
             GR_GroupArea->CH_Disable, GR_GroupArea->CH_InputMode,
             GR_GroupArea->CH_Phantom, GR_GroupArea->SL_Weight,
             GR_GroupArea->CY_Background, GR_GroupArea->CY_Frame,
             GR_GroupArea->STR_Char, GR_GroupArea->STR_TitleFrame, bt_ok,
             bt_cancel, NULL);
    if (!image_aux->fixheight)
        set(STR_height, MUIA_Disabled, TRUE);
    if (!image_aux->fixwidth)
        set(STR_width, MUIA_Disabled, TRUE);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_image);
    set(WI_image, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case 11:
                image_aux->type = MUII_ArrowUp;
                break;
            case 12:
                image_aux->type = MUII_ArrowDown;
                break;
            case 13:
                image_aux->type = MUII_ArrowLeft;
                break;
            case 14:
                image_aux->type = MUII_ArrowRight;
                break;
            case 15:
                image_aux->type = MUII_RadioButton;
                break;
            case 16:
                image_aux->type = MUII_Cycle;
                break;
            case 17:
                image_aux->type = MUII_PopUp;
                break;
            case 18:
                image_aux->type = MUII_PopFile;
                break;
            case 19:
                image_aux->type = MUII_PopDrawer;
                break;
            case 20:
                image_aux->type = MUII_Drawer;
                break;
            case 21:
                image_aux->type = MUII_HardDisk;
                break;
            case 22:
                image_aux->type = MUII_Disk;
                break;
            case 23:
                image_aux->type = MUII_Chip;
                break;
            case 24:
                image_aux->type = MUII_Volume;
                break;
            case 25:
                image_aux->type = MUII_Network;
                break;
            case 26:
                image_aux->type = MUII_Assign;
                break;
            case 27:
                image_aux->type = MUII_TapePlay;
                break;
            case 28:
                image_aux->type = MUII_TapePlayBack;
                break;
            case 29:
                image_aux->type = MUII_TapePause;
                break;
            case 30:
                image_aux->type = MUII_TapeStop;
                break;
            case 31:
                image_aux->type = MUII_TapeRecord;
                break;
            case ID_OKWIN:
                aux = GetStr(STR_spec);
                if (strlen(aux) > 0)
                    image_aux->type = -1;
                get(PA_spec, MUIA_Popasl_Active, &bool_aux);
                bool_aux = !bool_aux;
                if ((strlen(aux) == 0) && (image_aux->type == -1))
                    bool_aux = FALSE;
                aux = GetStr(STR_label);
                if ((strlen(image_aux->label) > 0) && bool_aux)
                {
                    strcpy(image_aux->label, aux);
                    aux = GetStr(STR_spec);
                    strncpy(image_aux->spec, aux, 128);
                    get(CM_height, MUIA_Selected, &active);
                    image_aux->fixheight = (active == 1);
                    get(CM_width, MUIA_Selected, &active);
                    image_aux->fixwidth = (active == 1);
                    get(CM_vert, MUIA_Selected, &active);
                    image_aux->freevert = (active == 1);
                    get(CM_horiz, MUIA_Selected, &active);
                    image_aux->freehoriz = (active == 1);
                    aux = GetStr(STR_height);
                    image_aux->height = atoi(aux);
                    aux = GetStr(STR_width);
                    image_aux->width = atoi(aux);
                    ValidateArea(GR_GroupArea, &image_aux->Area);
                    if (new)
                    {
                        nb_image++;
                    }
                    result = TRUE;
                    running = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString(MSG_NeedImageLabel));
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
    set(WI_image, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_image);
    MUI_DisposeObject(WI_image);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
