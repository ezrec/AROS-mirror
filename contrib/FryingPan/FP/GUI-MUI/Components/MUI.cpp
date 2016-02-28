/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "MUI.h"
#include <Generic/LibrarySpool.h>
#include <libclass/intuition.h>
#include <libraries/mui.h>

MUI::MUI()
{
   cbButtonHook = 0;
   gateBtns.Initialize(this, &MUI::gateButtons);
}

MUI::~MUI()
{
   muiElements.ForEach(&MUI::freeItem);
}
   
bool MUI::freeItem(elemAssoc* const &item)
{
   delete item;
   return true;
}

uint32 *MUI::muiLabel(const char* contents, char key, int id, Alignment align)
{
   Object *all;
   
   all = TextObject,
      MUIA_Text_Contents,     contents,
      MUIA_Weight,            20,
      MUIA_Text_HiChar,       key,
      MUIA_ShowSelState,      false,
      MUIA_Text_PreParse,     align == Align_Left     ? "\033l" :
                              align == Align_Right    ? "\033r" :
                                                        "\033c",
   End;
   
   muiElements << new elemAssoc(T_Label, id, (uint32 *)all);

   return (uint32 *)all;
}

uint32 *MUI::muiButton(const char* contents, char key, int id)
{
   Object *btn;
   
   btn = TextObject,
      ButtonFrame,
      MUIA_Text_Contents,     contents,
      MUIA_Text_HiChar,       key,
      MUIA_ControlChar,       key,
      MUIA_Text_PreParse,     "\033c",
      MUIA_Font,              MUIV_Font_Button,
      MUIA_Background,        MUII_ButtonBack,
      MUIA_InputMode,         MUIV_InputMode_RelVerify,
   End;

   DoMtd(btn, ARRAY(MUIM_Notify, MUIA_Pressed, false, (IPTR)btn, 4, MUIM_CallHook, (IPTR)gateBtns.GetHook(), id, 0));

   muiElements << new elemAssoc(T_Button, id, (uint32 *)btn);

   return (uint32 *)btn;
}

uint32 *MUI::muiCycle(const char** contents, char key, int id, int active)
{
   Object *btn;
   
   btn = CycleObject,
      MUIA_Cycle_Entries,     contents,
      MUIA_ControlChar,       key,
   End;

   DoMtd(btn, ARRAY(MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, (IPTR)btn, 4, MUIM_CallHook, (IPTR)gateBtns.GetHook(), id, MUIV_TriggerValue));

   Intuition->SetAttrsA(btn, (TagItem*)ARRAY(
      MUIA_Cycle_Active,      active,
      TAG_DONE,               0
   ));

   muiElements << new elemAssoc(T_Cycle, id, (uint32 *)btn);
   return (uint32 *)btn;
}

unsigned long MUI::gateButtons(iptr btn, iptr idptr)
{
   return cbButtonHook(idptr[0], (iptr)idptr[1]);
}

void MUI::setButtonCallBack(const Hook *hook)
{
   cbButtonHook = hook;
}

uint32 *MUI::muiCheckBox(const char* name, char key, int id, Alignment align, bool state)
{
   unsigned long *all;
   unsigned long *checkbox;
   unsigned long *label = 0;

   if (0 != name)
   {
      label    = muiLabel(name, key, ID_Default, align);
   } 
   else
   {
      label    = muiLabel("", 0, ID_Default, align);
   }

   checkbox = (unsigned long *)ImageObject,
      ImageButtonFrame,
      MUIA_Image_Spec,        MUII_CheckMark,
      MUIA_Background,        MUII_ButtonBack,
      MUIA_Image_FreeVert,    true,
      MUIA_ShowSelState,      false,
      MUIA_Weight,            0,
   End;

   all = (unsigned long *)HGroup,
      MUIA_InputMode,         MUIV_InputMode_Toggle,
      MUIA_ShowSelState,      false,
      Child,                  (align == Align_Left) ? checkbox : label,
      Child,                  (align == Align_Left) ? label : checkbox,
      MUIA_ControlChar,       key,
   End;

   DoMtd((Object *)all, ARRAY(MUIM_Notify, MUIA_Selected, MUIV_EveryTime, (IPTR)all, 4, MUIM_CallHook, (IPTR)gateBtns.GetHook(), id, MUIV_TriggerValue));

   muiElements << new elemAssoc(T_CheckBox, id, all);

   muiSetSelected(id, state);

   return all;
}

uint32 *MUI::muiSlider(int32 min, int32 max, int32 level, char key, int id)
{
   unsigned long *slider;

   slider = (unsigned long *)SliderObject,
      MUIA_Numeric_Min,       min,
      MUIA_Numeric_Max,       max,
      MUIA_Numeric_Value,     level,
      MUIA_ControlChar,       key,
   End;

   DoMtd((Object *)slider, ARRAY(MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, (IPTR)slider, 4, MUIM_CallHook, (IPTR)gateBtns.GetHook(), id, MUIV_TriggerValue));

   muiElements << new elemAssoc(T_Slider, id, slider);
   return slider;
}

uint32 *MUI::muiBar(const char *name, bool vertical)
{
   return (uint32 *)RectangleObject,
      MUIA_Rectangle_HBar,       !vertical,
      MUIA_Rectangle_VBar,       vertical,
      MUIA_Rectangle_BarTitle,   name,
      vertical ? 
         MUIA_FixWidth : MUIA_FixHeight,  10,
   End;
}

void MUI::muiSetEnabled(int id, bool enabled)
{
   for (int i=0; i<muiElements.Count(); i++)
   {
      if (muiElements[i]->id == id)
      {
         Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
            MUIA_Disabled,    !enabled,
            TAG_DONE,         0
         ));
      }
   }
}

void MUI::muiSetSelected(int id, int selected)
{
   for (int i=0; i<muiElements.Count(); i++)
   {
      if (muiElements[i]->id == id)
      {
         switch (muiElements[i]->type)
         {
            case T_CheckBox:
               Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
                  MUIA_Selected,    selected,
                  TAG_DONE,         0
               ));
               break;
         
            case T_Cycle:
               Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
                  MUIA_Cycle_Active,selected,
                  TAG_DONE,         0
               ));
               break;

            default:
               break;
         }
      }
   }
}

void MUI::muiSetVisible(int id, bool visible)
{
   for (int i=0; i<muiElements.Count(); i++)
   {
      if (muiElements[i]->id == id)
      {
         Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
            MUIA_ShowMe,      visible,
            TAG_DONE,         0
         ));
      }
   }
}

void MUI::muiSetValue(int id, int value)
{
   for (int i=0; i<muiElements.Count(); i++)
   {
      if (muiElements[i]->id == id)
      {
         switch (muiElements[i]->type)
         {
            case T_Gauge:
               Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
                  MUIA_Gauge_Current,     (int)value,
                  TAG_DONE,               0
               ));
               break;

            case T_Slider:
               Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
                  MUIA_Numeric_Value,     (int)value,
                  TAG_DONE,               0
               ));
               break;

            default:
               break;
         }
      }
   }
}

void MUI::muiSetText(int id, const char *text)
{
   for (int i=0; i<muiElements.Count(); i++)
   {
      if (muiElements[i]->id == id)
      {
         switch (muiElements[i]->type)
         {
            case T_Label:
               Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
                  MUIA_Text_Contents,     (IPTR)text,
                  TAG_DONE,               0
               ));
               break;
         
            case T_String:
               Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
                  MUIA_String_Contents,   (IPTR)text,
                  TAG_DONE,               0
               ));
               break;

            case T_Gauge:
               Intuition->SetAttrsA(muiElements[i]->elem, (TagItem*)ARRAY(
                  MUIA_Gauge_InfoText,    (IPTR)text,
                  TAG_DONE,               0
               ));
               break;

            default:
               break;
         }
      }
   }
}

uint32 *MUI::muiString(const char* contents, char key, int id, Alignment align)
{
   unsigned long *all;

   all = (unsigned long *)StringObject,
      StringFrame,
      MUIA_String_AdvanceOnCR,   true,
      MUIA_ControlChar,          key,
      MUIA_String_Format,        align == Align_Left  ?  MUIV_String_Format_Left:
                                 align == Align_Right ?  MUIV_String_Format_Right:
                                                         MUIV_String_Format_Center,
   End;

   muiElements << new elemAssoc(T_String, id, all);

   DoMtd((Object *)all, ARRAY(MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, (IPTR)all, 4, MUIM_CallHook, (IPTR)gateBtns.GetHook(), id, MUIV_TriggerValue));
   muiSetText(id, contents);
   return all;
}

uint32 *MUI::muiGauge(const char* contents, int id)
{
   unsigned long *all;

   all = (unsigned long *)GaugeObject,
      GaugeFrame,
      MUIA_Gauge_Horiz,          true,
      MUIA_Gauge_InfoText,       contents,
      MUIA_Gauge_Max,            65535,
      MUIA_Gauge_Current,        0,
   End;

   muiElements << new elemAssoc(T_Gauge, id, all);

   return all;
}

uint32 *MUI::muiSpace(int weight)
{
   return (uint32 *)RectangleObject,
      MUIA_Weight,      weight,
   End;
}

