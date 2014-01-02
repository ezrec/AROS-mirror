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

#include "builder.h"
#include "codenotify.h"
#include "muib_file.h"

/********************* Application ********************/
int CACTAppli[] = {
    2, MB_MUIM_Application_ReturnID, 0, 0,
    2, MB_MUIM_Application_ReturnID, MB_MUIV_Application_ReturnID_Quit, 0,
    3, MB_MUIM_Set, MB_MUIA_Application_Sleep, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Application_Sleep, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTAppli[] = {
    MB_MUIA_Application_Iconified, TRUE,
    MB_MUIA_Application_Iconified, FALSE,
    MB_MUIA_Application_Active, TRUE,
    MB_MUIA_Application_Active, FALSE,
    0, 0,
};

int ArgEVTAppli[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/*********************** Window ***********************/
int CACTWindow[] = {
    3, MB_MUIM_Set, MB_MUIA_Window_Open, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Window_Open, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_Activate, TRUE,
    2, MB_MUIM_CallHook, 0, 0,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject,
    MB_MUIV_Window_ActiveObject_None,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject,
    MB_MUIV_Window_ActiveObject_Next,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject,
    MB_MUIV_Window_ActiveObject_Prev,
    0, 0, 0, 0
};

int CEVTWindow[] = {
    MB_MUIA_Window_CloseRequest, TRUE,
    MB_MUIA_Window_Activate, TRUE,
    MB_MUIA_Window_Activate, FALSE,
    0, 0
};

int ArgEVTWindow[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_MUI,
    0
};

/*********************** Button ***********************/
int CACTButton[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject, 0,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, 0,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTButton[] = {
    MB_MUIA_Pressed, TRUE,
    MB_MUIA_Pressed, FALSE,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    MB_MUIA_Timer, MB_MUIV_EveryTime,
    0, 0
};

int ArgEVTButton[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_MUI,
    0
};

/******************* Group *************************/
int CACTGroup[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject, 0,
    3, MB_MUIM_Set, MB_MUIA_Group_ActivePage, MB_MUIV_TriggerValue,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTGroup[] = {
    MB_MUIA_Group_ActivePage, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTGroup[] = {
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* String ************************/
int CACTString[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject, 0,
    3, MB_MUIM_Set, MB_MUIA_String_Contents, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_String_Contents, 0,
    3, MB_MUIM_Set, MB_MUIA_String_Contents, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTString[] = {
    MB_MUIA_String_Acknowledge, MB_MUIV_EveryTime,
    MB_MUIA_String_BufferPos, MB_MUIV_EveryTime,
    MB_MUIA_String_Contents, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTString[] = {
    ARG_MUI,
    ARG_MUI,
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/****************** Listview ***********************/
int CACTListview[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject, 0,
    1, MB_MUIM_List_Clear, 0, 0,
    2, MB_MUIM_List_Jump, 0, 0,
    2, MB_MUIM_List_Redraw, MB_MUIV_List_Redraw_All, 0,
    2, MB_MUIM_List_Redraw, MB_MUIV_List_Redraw_Active, 0,
    2, MB_MUIM_List_Remove, MB_MUIV_List_Remove_Active, 0,
    1, MB_MUIM_List_Sort, 0, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTListview[] = {
    MB_MUIA_Listview_DoubleClick, TRUE,
    MB_MUIA_List_Active, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTListview[] = {
    ARG_BOOL,
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Gauge *************************/
int CACTGauge[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Gauge_Current, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Gauge_Current, 0,
    3, MB_MUIM_Set, MB_MUIA_Gauge_Current, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTGauge[] = {
    MB_MUIA_Gauge_Current, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTGauge[] = {
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Cycle *************************/
int CACTCycle[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject, 0,
    3, MB_MUIM_Set, MB_MUIA_Cycle_Active, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Cycle_Active, 0,
    3, MB_MUIM_Set, MB_MUIA_Cycle_Active, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTCycle[] = {
    MB_MUIA_Cycle_Active, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTCycle[] = {
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Radio *************************/
int CACTRadio[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject, 0,
    3, MB_MUIM_Set, MB_MUIA_Radio_Active, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Radio_Active, 0,
    3, MB_MUIM_Set, MB_MUIA_Radio_Active, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTRadio[] = {
    MB_MUIA_Radio_Active, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTRadio[] = {
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Label *************************/
int CACTLabel[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, 0,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTLabel[] = {
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTLabel[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Space *************************/
int CACTSpace[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTSpace[] = {
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    0, 0
};

int ArgEVTSpace[] = {
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Check *************************/
int CACTCheck[] = {
    3, MB_MUIM_Set, MB_MUIA_Selected, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Selected, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Selected, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Selected, 0,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Window_ActiveObject, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTCheck[] = {
    MB_MUIA_Selected, TRUE,
    MB_MUIA_Selected, FALSE,
    MB_MUIA_Selected, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTCheck[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Scale *************************/
int CACTScale[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTScale[] = {
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    0, 0
};

int ArgEVTScale[] = {
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Image *************************/
int CACTImage[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTImage[] = {
    MB_MUIA_Pressed, TRUE,
    MB_MUIA_Pressed, FALSE,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTImage[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Slider ************************/
int CACTSlider[] = {
    3, MB_MUIM_Set, MB_MUIA_Slider_Level, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Slider_Level, 0,
    3, MB_MUIM_Set, MB_MUIA_Slider_Level, 0,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTSlider[] = {
    MB_MUIA_Slider_Level, MB_MUIV_EveryTime,
    MB_MUIA_Slider_Level, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTSlider[] = {
    ARG_MUI,
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Text *************************/
int CACTText[] = {
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, 0,
    3, MB_MUIM_Set, MB_MUIA_Text_Contents, 0,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTText[] = {
    MB_MUIA_Text_Contents, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTText[] = {
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* Prop *************************/
int CACTProp[] = {
    3, MB_MUIM_Set, MB_MUIA_Prop_First, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Prop_First, 0,
    3, MB_MUIM_Set, MB_MUIA_Prop_First, 0,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTProp[] = {
    MB_MUIA_Prop_First, MB_MUIV_EveryTime,
    MB_MUIA_Prop_Entries, MB_MUIV_EveryTime,
    MB_MUIA_Prop_Visible, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTProp[] = {
    ARG_MUI,
    ARG_MUI,
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* ColorField *************************/
int CACTColorField[] = {
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Red, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Red, 0,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Red, 0,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Blue, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Blue, 0,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Blue, 0,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Green, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Green, 0,
    3, MB_MUIM_Set, MB_MUIA_Colorfield_Green, 0,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTColorField[] = {
    MB_MUIA_Colorfield_Red, MB_MUIV_EveryTime,
    MB_MUIA_Colorfield_Blue, MB_MUIV_EveryTime,
    MB_MUIA_Colorfield_Green, MB_MUIV_EveryTime,
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTColorField[] = {
    ARG_MUI,
    ARG_MUI,
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* PopAsl *************************/
int CACTPopAsl[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    1, MB_MUIM_Popstring_Open, 0, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTPopAsl[] = {
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTPopAsl[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/******************* PopObject *************************/
int CACTPopObject[] = {
    3, MB_MUIM_Set, MB_MUIA_ShowMe, FALSE,
    3, MB_MUIM_Set, MB_MUIA_ShowMe, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Disabled, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTPopObject[] = {
    MB_MUIA_ShowMe, FALSE,
    MB_MUIA_ShowMe, TRUE,
    MB_MUIA_Disabled, TRUE,
    MB_MUIA_Disabled, FALSE,
    0, 0
};

int ArgEVTPopObject[] = {
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/***************** MenuStrip ***********************************/
int CACTMenuStrip[] = {
    3, MB_MUIM_Set, MB_MUIA_Menustrip_Enabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Menustrip_Enabled, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTMenuStrip[] = {
    MB_MUIA_Menustrip_Enabled, TRUE,
    MB_MUIA_Menustrip_Enabled, FALSE,
    0, 0,
};

int ArgEVTMenuStrip[] = {
    ARG_BOOL,
    ARG_BOOL,
    0
};

/***************** Menu ***********************************/
int CACTMenu[] = {
    3, MB_MUIM_Set, MB_MUIA_Menu_Enabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Menu_Enabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Menu_Title, 0,
    3, MB_MUIM_Set, MB_MUIA_Menu_Title, 0,
    3, MB_MUIM_Set, MB_MUIA_Menu_Title, 0,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTMenu[] = {
    MB_MUIA_Menu_Enabled, TRUE,
    MB_MUIA_Menu_Enabled, FALSE,
    0, 0,
};

int ArgEVTMenu[] = {
    ARG_BOOL,
    ARG_BOOL,
    0
};

/***************** MenuItem ***********************************/
int CACTMenuItem[] = {
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Enabled, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Enabled, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Checked, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Checked, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Checkit, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Checkit, FALSE,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Title, MB_MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Title, 0,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Title, 0,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Shortcut, MUIV_TriggerValue,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Shortcut, 0,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Shortcut, 0,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Toggle, TRUE,
    3, MB_MUIM_Set, MB_MUIA_Menuitem_Toggle, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0, 0, 0
};

int CEVTMenuItem[] = {
    MB_MUIA_Menuitem_Trigger, MB_MUIV_EveryTime,
    MB_MUIA_Menuitem_Enabled, TRUE,
    MB_MUIA_Menuitem_Enabled, FALSE,
    MB_MUIA_Menuitem_Checked, TRUE,
    MB_MUIA_Menuitem_Checked, FALSE,
    MB_MUIA_Menuitem_Checkit, TRUE,
    MB_MUIA_Menuitem_Checkit, FALSE,
    2, MB_MUIM_CallHook, 0, 0,
    0, 0,
};

int ArgEVTMenuItem[] = {
    ARG_MUI,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    ARG_BOOL,
    0
};

/*************************** complete *************************/

int *CEvenements[] = {
    CEVTAppli,
    CEVTWindow,
    CEVTGroup,
    CEVTButton,
    CEVTString,
    CEVTListview,
    CEVTGauge,
    CEVTCycle,
    CEVTRadio,
    CEVTLabel,
    CEVTSpace,
    CEVTCheck,
    CEVTScale,
    CEVTImage,
    CEVTSlider,
    CEVTText,
    CEVTProp,
    CEVTListview,               /* ListView = DirList */
    CEVTSpace,                  /* Rectangle = Space */
    CEVTColorField,
    CEVTPopAsl,
    CEVTPopObject,
    CEVTMenuStrip,
    CEVTMenu,
    CEVTMenuItem
};

int *CActions[] = {
    CACTAppli,
    CACTWindow,
    CACTGroup,
    CACTButton,
    CACTString,
    CACTListview,
    CACTGauge,
    CACTCycle,
    CACTRadio,
    CACTLabel,
    CACTSpace,
    CACTCheck,
    CACTScale,
    CACTImage,
    CACTSlider,
    CACTText,
    CACTProp,
    CACTListview,               /* ListView = DirList */
    CACTSpace,                  /* Rectangle = Space */
    CACTColorField,
    CACTPopAsl,
    CACTPopObject,
    CACTMenuStrip,
    CACTMenu,
    CACTMenuItem
};

int *ArgEVT[] = {
    ArgEVTAppli,
    ArgEVTWindow,
    ArgEVTGroup,
    ArgEVTButton,
    ArgEVTString,
    ArgEVTListview,
    ArgEVTGauge,
    ArgEVTCycle,
    ArgEVTRadio,
    ArgEVTLabel,
    ArgEVTSpace,
    ArgEVTCheck,
    ArgEVTScale,
    ArgEVTImage,
    ArgEVTSlider,
    ArgEVTText,
    ArgEVTProp,
    ArgEVTListview,             /* ListView = DirList */
    ArgEVTSpace,                /* Rectangle = Space */
    ArgEVTColorField,
    ArgEVTPopAsl,
    ArgEVTPopObject,
    ArgEVTMenuStrip,
    ArgEVTMenu,
    ArgEVTMenuItem
};
