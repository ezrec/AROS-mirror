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
#include <exec/memory.h>

void InitArea(area *Area)
{
    Area->Hide = FALSE;
    Area->Disable = FALSE;
    Area->InputMode = FALSE;
    Area->Phantom = FALSE;
    Area->Weight = 100;
    Area->Background = MUII_WindowBack;
    Area->Frame = MUIV_Frame_None;
    Area->key = '\0';
    Area->TitleFrame[0] = '\0';
}

void ValidateArea(struct ObjGR_AreaGroup *Obj, area *Area)
{
    int active;

    sprintf(Area->TitleFrame, "%s", GetStr(Obj->STR_TitleFrame));
    get(Obj->SL_Weight, MUIA_Slider_Level, &Area->Weight);
    get(Obj->CY_Background, MUIA_Cycle_Active, &Area->Background);
    get(Obj->CY_Frame, MUIA_Cycle_Active, &Area->Frame);
    Area->key = ((char *) GetStr(Obj->STR_Char))[0];
    get(Obj->CH_Hide, MUIA_Selected, &active);
    Area->Hide = (active == 1);
    get(Obj->CH_Disable, MUIA_Selected, &active);
    Area->Disable = (active == 1);
    get(Obj->CH_InputMode, MUIA_Selected, &active);
    Area->InputMode = (active == 1);
    get(Obj->CH_Phantom, MUIA_Selected, &active);
    Area->Phantom = (active == 1);
    if (Area->Background > MUII_SelectedBack)
        Area->Background += 120;
}

struct ObjGR_AreaGroup *CreateGR_AreaGroup(area *Area, BOOL InputMode,
                                           BOOL Background, BOOL Frame,
                                           BOOL Char, BOOL TitleFrame)
{
    struct ObjGR_AreaGroup *Object;

    APTR obj_aux0;
    APTR obj_aux1;
    APTR obj_aux2;
    APTR obj_aux3;
    APTR obj_aux4;
    APTR obj_aux5;
    APTR obj_aux6;
    APTR obj_aux7;
    APTR obj_aux8;
    APTR obj_aux9;
    APTR obj_aux10;

    if (!
        (Object =
         AllocVec(sizeof(struct ObjGR_AreaGroup),
                  MEMF_PUBLIC | MEMF_CLEAR)))
        return (NULL);

    Object->STR_CY_Background[0] = "Window";
    Object->STR_CY_Background[1] = "Requester";
    Object->STR_CY_Background[2] = "Button";
    Object->STR_CY_Background[3] = "List";
    Object->STR_CY_Background[4] = "Text";
    Object->STR_CY_Background[5] = "Proportional";
    Object->STR_CY_Background[6] = "Selected";
    Object->STR_CY_Background[7] = "Popup";
    Object->STR_CY_Background[8] = "Background";
    Object->STR_CY_Background[9] = "Shadow";
    Object->STR_CY_Background[10] = "Shine";
    Object->STR_CY_Background[11] = "Fill";
    Object->STR_CY_Background[12] = "ShadowBack";
    Object->STR_CY_Background[13] = "ShadowFill";
    Object->STR_CY_Background[14] = "ShadowShine";
    Object->STR_CY_Background[15] = "FillBack";
    Object->STR_CY_Background[16] = "ShineBack";
    Object->STR_CY_Background[17] = "FillBack2";
    Object->STR_CY_Background[18] = "HShineBack";
    Object->STR_CY_Background[19] = "HShadowBack";
    Object->STR_CY_Background[20] = "HShineShine";
    Object->STR_CY_Background[21] = "HShadowShadow";
    Object->STR_CY_Background[22] = "LastPat";
    Object->STR_CY_Background[23] = NULL;
    Object->STR_CY_Frame[0] = "None";
    Object->STR_CY_Frame[1] = "Button";
    Object->STR_CY_Frame[2] = "ImageButton";
    Object->STR_CY_Frame[3] = "Text";
    Object->STR_CY_Frame[4] = "String";
    Object->STR_CY_Frame[5] = "ReadList";
    Object->STR_CY_Frame[6] = "InputList";
    Object->STR_CY_Frame[7] = "Prop";
    Object->STR_CY_Frame[8] = "Gauge";
    Object->STR_CY_Frame[9] = "Group";
    Object->STR_CY_Frame[10] = "PopUp";
    Object->STR_CY_Frame[11] = "Virtual";
    Object->STR_CY_Frame[12] = "Slider";
    Object->STR_CY_Frame[13] = "Count";
    Object->STR_CY_Frame[14] = NULL;

    obj_aux1 = Label(GetMUIBuilderString(MSG_Hide));

    Object->CH_Hide = CheckMark(Area->Hide);

    obj_aux2 = Label(GetMUIBuilderString(MSG_Disable));

    Object->CH_Disable = CheckMark(Area->Disable);

    obj_aux3 = Label(GetMUIBuilderString(MSG_InputMode));

    Object->CH_InputMode = CheckMark(Area->InputMode);

    obj_aux4 = Label(GetMUIBuilderString(MSG_PhantomFrame));

    Object->CH_Phantom = CheckMark(Area->Phantom);

    // *INDENT-OFF*
    obj_aux0 = GroupObject,
        GroupFrameT(0),
        MUIA_Group_Columns, 4,
        Child, obj_aux1,
        Child, Object->CH_Hide,
        Child, obj_aux2,
        Child, Object->CH_Disable,
        Child, obj_aux3,
        Child, Object->CH_InputMode,
        Child, obj_aux4,
        Child, Object->CH_Phantom,
    End;

    obj_aux6 = Label(GetMUIBuilderString(MSG_Weight));

    Object->SL_Weight = SliderObject,
        MUIA_Slider_Min, 0,
        MUIA_Slider_Max, 100,
        MUIA_Slider_Quiet, FALSE,
        MUIA_Slider_Level, Area->Weight,
        MUIA_Slider_Reverse, FALSE,
    End;

    obj_aux7 = Label(GetMUIBuilderString(MSG_Background));

    Object->CY_Background = CycleObject,
        MUIA_Cycle_Entries, Object->STR_CY_Background,
        MUIA_Disabled, !Background,
    End;

    obj_aux8 = Label(GetMUIBuilderString(MSG_Frame));

    Object->CY_Frame = CycleObject,
        MUIA_Cycle_Entries, Object->STR_CY_Frame,
        MUIA_Cycle_Active, Area->Frame,
        MUIA_Disabled, !Frame,
    End;

    obj_aux9 = Label(GetMUIBuilderString(MSG_ControlChar));

    Object->STR_Char = StringObject,
        StringFrame,
        MUIA_String_MaxLen, 2,
        MUIA_String_Format, 0,
        MUIA_String_Contents, &Area->key,
        MUIA_Disabled, !Char,
    End;

    obj_aux10 = Label(GetMUIBuilderString(MSG_TitleFrame));

    Object->STR_TitleFrame = StringObject,
        StringFrame,
        MUIA_String_MaxLen, 80,
        MUIA_String_Format, 0,
        MUIA_String_Contents, Area->TitleFrame,
        MUIA_Disabled, !TitleFrame,
    End;

    obj_aux5 = GroupObject,
        GroupFrameT(0),
        MUIA_Group_Columns, 2,
        Child, obj_aux6,
        Child, Object->SL_Weight,
        Child, obj_aux7,
        Child, Object->CY_Background,
        Child, obj_aux8,
        Child, Object->CY_Frame,
        Child, obj_aux9,
        Child, Object->STR_Char,
        Child, obj_aux10,
        Child, Object->STR_TitleFrame,
    End;

    Object->GR_AreaGroup = GroupObject,
        Child, obj_aux0,
        Child, obj_aux5,
    End;
    // *INDENT-ON*


    if (!Object->GR_AreaGroup)
    {
        FreeVec(Object);
        return (NULL);
    }

    if (Area->Background > MUII_SelectedBack)
        set(Object->CY_Background, MUIA_Cycle_Active,
            Area->Background - 120);
    else
        set(Object->CY_Background, MUIA_Cycle_Active, Area->Background);
    set(Object->CH_InputMode, MUIA_Disabled, !InputMode);

    return (Object);
}

void DisposeGR_AreaGroup(struct ObjGR_AreaGroup *Object)
{
    FreeVec(Object);
}
