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

void InitPopObject(popobject *popobj_aux)
{
    InitArea(&popobj_aux->Area);
    popobj_aux->Area.Background = MUII_WindowBack;
    popobj_aux->Area.Frame = MUIV_Frame_None;
    sprintf(popobj_aux->label, "PO_label_%d", nb_popobject);
    popobj_aux->openhook = NULL;
    popobj_aux->closehook = NULL;
    popobj_aux->image = 0;
    popobj_aux->obj = NULL;
    popobj_aux->follow = TRUE;
    popobj_aux->light = TRUE;
    popobj_aux->Volatile = TRUE;
    popobj_aux->notify = create();
    popobj_aux->notifysource = create();
}

BOOL NewPopObject(popobject *popobj_aux, BOOL new)
{
    APTR WI_PopObject, RegGroup, LV_images, CH_follow, CH_light,
        CH_Volatile, BT_obj;
    APTR IM_obj, STR_openhook, STR_closehook, STR_label, BT_ok, BT_cancel;
    APTR IM_openhook, IM_closehook;
    BOOL running = TRUE;
    BOOL result = FALSE;
    ULONG signal, active;
    CONST_STRPTR aux;
    object *child_aux;
    int i;
    struct ObjGR_AreaGroup *GR_GroupArea;
    CONST_STRPTR RegisterTitles[3];
    CONST_STRPTR STR_images[3];

    STR_images[0] = GetMUIBuilderString(MSG_PopUpImage);
    STR_images[1] = GetMUIBuilderString(MSG_PopFileImage);
    STR_images[2] = GetMUIBuilderString(MSG_PopDrawerImage);

    if (new)
    {
        InitPopObject(popobj_aux);
    }

    if (!
        (GR_GroupArea =
         CreateGR_AreaGroup(&popobj_aux->Area, FALSE, TRUE, TRUE, TRUE,
                            FALSE)))
        return (FALSE);

    RegisterTitles[0] = GetMUIBuilderString(MSG_Attributes);
    RegisterTitles[1] = GetMUIBuilderString(MSG_Area);
    RegisterTitles[2] = NULL;

    // *INDENT-OFF*
    WI_PopObject = WindowObject,
        MUIA_Window_Title, GetMUIBuilderString(MSG_PopObjectChoices),
        MUIA_Window_ID, MAKE_ID('P', 'O', 'P', nb_popobject),
        MUIA_HelpNode, "PopObject",
        WindowContents, GroupObject,
            Child, RegGroup = RegisterGroup(RegisterTitles),
                Child, VGroup,
                    Child, GroupObject,
                        MUIA_Group_Horiz, TRUE,
                        Child, LV_images = ListviewObject,
                            MUIA_Listview_List, ListObject,
                                MUIA_List_Format, "COL=0",
                                InputListFrame,
                            End,
                        End,
                        Child, GroupObject,
                            GroupFrameT(GetMUIBuilderString(MSG_Attributes)),
                            MUIA_Group_Columns, 4,
                            Child, HVSpace,
                            Child, Label(GetMUIBuilderString(MSG_Follow)),
                            Child, CH_follow = CheckMark(popobj_aux->follow),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, Label(GetMUIBuilderString(MSG_Light)),
                            Child, CH_light = CheckMark(popobj_aux->light),
                            Child, HVSpace,
                            Child, HVSpace,
                            Child, Label(GetMUIBuilderString(MSG_Volatile)),
                            Child, CH_Volatile = CheckMark(popobj_aux->Volatile),
                            Child, HVSpace,
                        End,
                    End,
                    Child, GroupObject,
                        MUIA_Group_Horiz, TRUE,
                        Child, BT_obj = SimpleButton(GetMUIBuilderString(MSG_Object)),
                        Child, IM_obj = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                    End,
                    Child, GroupObject,
                        GroupFrameT(GetMUIBuilderString(MSG_CallHook)),
                        MUIA_Group_Columns, 3,
                        Child, Label(GetMUIBuilderString(MSG_CloseHook)),
                        Child, STR_openhook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, popobj_aux->openhook,
                        End,
                        Child, IM_openhook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                        Child, Label(GetMUIBuilderString(MSG_OpenHook)),
                        Child, STR_closehook = TextObject,
                            TextFrame,
                            MUIA_Text_Contents, popobj_aux->closehook,
                        End,
                        Child, IM_closehook = ImageObject,
                            MUIA_Image_Spec, 18,
                            MUIA_Image_FreeVert, TRUE,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
                    End,
                    Child, GroupObject,
                        GroupFrameT(GetMUIBuilderString(MSG_Indentification)),
                        MUIA_Group_Horiz, TRUE,
                        MUIA_Group_SameWidth, TRUE,
                        Child, Label(GetMUIBuilderString(MSG_Label)),
                        Child, STR_label = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen, 80,
                            MUIA_String_Format, 0,
                            MUIA_String_Contents, popobj_aux->label,
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

    WI_current = WI_PopObject;

    DoMethod(BT_ok, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(BT_cancel, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_END);
    DoMethod(WI_PopObject, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             app, 2, MUIM_Application_ReturnID, ID_OKWIN);
    DoMethod(IM_obj, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_NEWOBJ);
    DoMethod(BT_obj, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_INFO);
    DoMethod(IM_openhook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP1);
    DoMethod(IM_closehook, MUIM_Notify, MUIA_Pressed, FALSE, app, 2,
             MUIM_Application_ReturnID, ID_POPUP2);

    DoMethod(WI_PopObject, MUIM_Window_SetCycleChain, RegGroup, LV_images,
             CH_follow, CH_light, CH_Volatile, BT_obj, IM_obj,
             IM_openhook, IM_closehook, STR_label,
             GR_GroupArea->CH_Hide, GR_GroupArea->CH_Disable,
             GR_GroupArea->CH_InputMode, GR_GroupArea->CH_Phantom,
             GR_GroupArea->SL_Weight, GR_GroupArea->CY_Background,
             GR_GroupArea->CY_Frame, GR_GroupArea->STR_Char,
             GR_GroupArea->STR_TitleFrame, BT_ok, BT_cancel, NULL);

    for (i = 0; i < 3; i++)
    {
        aux = STR_images[i];
        DoMethod(LV_images, MUIM_List_Insert, &aux, 1,
                 MUIV_List_Insert_Bottom);
    }
    set(LV_images, MUIA_List_Active, popobj_aux->image);
    if (popobj_aux->obj)
        set(BT_obj, MUIA_Text_Contents, popobj_aux->obj->label);

    set(app, MUIA_Application_Sleep, TRUE);
    DoMethod(app, OM_ADDMEMBER, WI_PopObject);
    set(WI_PopObject, MUIA_Window_Open, TRUE);
    while (running)
    {
        switch (DoMethod(app, MUIM_Application_Input, &signal))
        {
            case ID_NEWOBJ:
                child_aux = NULL;
                nb_popobject++;
                if (child_aux = NewChild(child_aux, TRUE))
                {
                    if (popobj_aux->obj)
                    {
                        DeleteObject(popobj_aux->obj);
                    }
                    child_aux->father = popobj_aux;
                    popobj_aux->obj = child_aux;
                    set(BT_obj, MUIA_Text_Contents, child_aux->label);
                    if (child_aux->id != TY_POPOBJECT)
                        nb_popobject--;
                }
                break;
            case ID_POPUP1:
                if (new)
                    GetMUIarg((object *) popobj_aux,
                              (APTR *) & popobj_aux->openhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & popobj_aux->openhook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (popobj_aux->openhook)
                    set(STR_openhook, MUIA_Text_Contents,
                        popobj_aux->openhook);
                else
                    set(STR_openhook, MUIA_Text_Contents, "");
                WI_current = WI_PopObject;
                break;
            case ID_POPUP2:
                if (new)
                    GetMUIarg((object *) popobj_aux,
                              (APTR *) & popobj_aux->closehook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                else
                    GetMUIarg(NULL, (APTR *) & popobj_aux->closehook, NULL,
                              application.Functions, TRUE,
                              GetMUIBuilderString(MSG_ChoseFunction));
                if (popobj_aux->closehook)
                    set(STR_closehook, MUIA_Text_Contents,
                        popobj_aux->closehook);
                else
                    set(STR_closehook, MUIA_Text_Contents, "");
                WI_current = WI_PopObject;
                break;
            case ID_INFO:
                child_aux = popobj_aux->obj;
                if (child_aux)
                {
                    if (EditChild(child_aux))
                        set(BT_obj, MUIA_Text_Contents, child_aux->label);
                }
                WI_current = WI_PopObject;
                break;
            case ID_OKWIN:
                aux = GetStr(STR_label);
                if ((strlen(aux) > 0) && (popobj_aux->obj != NULL))
                {
                    strcpy(popobj_aux->label, aux);
                    get(LV_images, MUIA_List_Active, &popobj_aux->image);
                    get(CH_follow, MUIA_Selected, &active);
                    popobj_aux->follow = (active == 1);
                    get(CH_light, MUIA_Selected, &active);
                    popobj_aux->light = (active == 1);
                    get(CH_Volatile, MUIA_Selected, &active);
                    popobj_aux->Volatile = (active == 1);
                    ValidateArea(GR_GroupArea, &popobj_aux->Area);
                    if (new)
                    {
                        nb_popobject++;
                    }
                    result = TRUE;
                    running = FALSE;
                }
                else
                    ErrorMessage(GetMUIBuilderString
                                 (MSG_GiveObjectAndLabel));
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
    set(WI_PopObject, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, WI_PopObject);
    MUI_DisposeObject(WI_PopObject);
    DisposeGR_AreaGroup(GR_GroupArea);
    set(app, MUIA_Application_Sleep, FALSE);
    return (result);
}
