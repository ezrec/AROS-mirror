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

#include "notify.h"
#include "MUIBuilder_cat.h"

/********************* Application ********************/
CONST_STRPTR ACTAppli[6];
int TYAppli[6];
CONST_STRPTR EVTAppli[5];

/*********************** Window ***********************/
CONST_STRPTR ACTWindow[8];
int TYWindow[8];
CONST_STRPTR EVTWindow[4];

/*********************** Button ***********************/
CONST_STRPTR ACTButton[10];
int TYButton[10];
CONST_STRPTR EVTButton[8];

/******************* Group *************************/
CONST_STRPTR ACTGroup[8];
int TYGroup[8];
CONST_STRPTR EVTGroup[6];

/******************* String ************************/
CONST_STRPTR ACTString[10];
int TYString[10];
CONST_STRPTR EVTString[8];

/****************** ListView ***********************/
CONST_STRPTR ACTListview[13];
int TYListview[13];
CONST_STRPTR EVTListview[7];

/******************* Gauge *************************/
CONST_STRPTR ACTGauge[9];
int TYGauge[9];
CONST_STRPTR EVTGauge[6];

/******************* Cycle *************************/
CONST_STRPTR ACTCycle[10];
int TYCycle[10];
CONST_STRPTR EVTCycle[6];

/******************* Radio *************************/
CONST_STRPTR ACTRadio[10];
int TYRadio[10];
CONST_STRPTR EVTRadio[6];

/******************* Label *************************/
CONST_STRPTR ACTLabel[9];
int TYLabel[9];
CONST_STRPTR EVTLabel[5];

/******************* Space *************************/
CONST_STRPTR ACTSpace[4];
int TYSpace[4];
CONST_STRPTR EVTSpace[3];

/******************* Check *************************/
CONST_STRPTR ACTCheck[11];
int TYCheck[11];
CONST_STRPTR EVTCheck[8];

/******************* Scale *************************/
CONST_STRPTR ACTScale[4];
int TYScale[4];
CONST_STRPTR EVTScale[3];

/******************* Image *************************/
CONST_STRPTR ACTImage[7];
int TYImage[7];
CONST_STRPTR EVTImage[7];

/******************* Slider ************************/
CONST_STRPTR ACTSlider[10];
int TYSlider[10];
CONST_STRPTR EVTSlider[7];

/******************* Text *************************/
CONST_STRPTR ACTText[9];
int TYText[9];
CONST_STRPTR EVTText[6];

/******************* Prop *************************/
CONST_STRPTR ACTProp[10];
int TYProp[10];
CONST_STRPTR EVTProp[8];

/******************* ColorField *************************/
CONST_STRPTR ACTColorField[16];
int TYColorField[16];
CONST_STRPTR EVTColorField[8];

/******************* PopAsl *************************/
CONST_STRPTR ACTPopAsl[7];
int TYPopAsl[7];
CONST_STRPTR EVTPopAsl[5];

/******************* PopObject *************************/
CONST_STRPTR ACTPopObject[7];
int TYPopObject[7];
CONST_STRPTR EVTPopObject[5];

/******************* MenuStrip *****************************/
CONST_STRPTR ACTMenuStrip[4];
int TYMenuStrip[4];
CONST_STRPTR EVTMenuStrip[3];

/******************* Menu *****************************/
CONST_STRPTR ACTMenu[7];
int TYMenu[7];
CONST_STRPTR EVTMenu[3];

/******************* MenuItem *************************/
CONST_STRPTR ACTMenuItem[16];
int TYMenuItem[16];
CONST_STRPTR EVTMenuItem[8];

/*************************** la totale *************************/
CONST_STRPTR *Evenements[] = {
    EVTAppli,
    EVTWindow,
    EVTGroup,
    EVTButton,
    EVTString,
    EVTListview,
    EVTGauge,
    EVTCycle,
    EVTRadio,
    EVTLabel,
    EVTSpace,
    EVTCheck,
    EVTScale,
    EVTImage,
    EVTSlider,
    EVTText,
    EVTProp,
    EVTListview,
    EVTSpace,
    EVTColorField,
    EVTPopAsl,
    EVTPopObject,
    EVTMenuStrip,
    EVTMenu,
    EVTMenuItem
};

CONST_STRPTR *Actions[] = {
    ACTAppli,
    ACTWindow,
    ACTGroup,
    ACTButton,
    ACTString,
    ACTListview,
    ACTGauge,
    ACTCycle,
    ACTRadio,
    ACTLabel,
    ACTSpace,
    ACTCheck,
    ACTScale,
    ACTImage,
    ACTSlider,
    ACTText,
    ACTProp,
    ACTListview,
    ACTSpace,
    ACTColorField,
    ACTPopAsl,
    ACTPopObject,
    ACTMenuStrip,
    ACTMenu,
    ACTMenuItem
};

int *TYActions[] = {
    TYAppli,
    TYWindow,
    TYGroup,
    TYButton,
    TYString,
    TYListview,
    TYGauge,
    TYCycle,
    TYRadio,
    TYLabel,
    TYSpace,
    TYCheck,
    TYScale,
    TYImage,
    TYSlider,
    TYText,
    TYProp,
    TYListview,
    TYSpace,
    TYColorField,
    TYPopAsl,
    TYPopObject,
    TYMenuStrip,
    TYMenu,
    TYMenuItem
};

void InitNotifyArrays(void)
{
/********************* Application ********************/
    ACTAppli[0] = GetMUIBuilderString(MSG_ReturnID);
    ACTAppli[1] = GetMUIBuilderString(MSG_ReturnQuit);
    ACTAppli[2] = GetMUIBuilderString(MSG_ApplicationSleep);
    ACTAppli[3] = GetMUIBuilderString(MSG_ApplicationWake);
    ACTAppli[4] = GetMUIBuilderString(MSG_CallFunction);
    ACTAppli[5] = NULL;

    TYAppli[0] = TY_ID;
    TYAppli[1] = TY_NOTHING;
    TYAppli[2] = TY_NOTHING;
    TYAppli[3] = TY_NOTHING;
    TYAppli[4] = TY_FUNCTION;
    TYAppli[5] = 0;

    EVTAppli[0] = GetMUIBuilderString(MSG_Iconify);
    EVTAppli[1] = GetMUIBuilderString(MSG_UnIconify);
    EVTAppli[2] = GetMUIBuilderString(MSG_NotifyActivate);
    EVTAppli[3] = GetMUIBuilderString(MSG_Unactivate);
    EVTAppli[4] = NULL;

/*********************** Window ***********************/
    ACTWindow[0] = GetMUIBuilderString(MSG_OpenWindow);
    ACTWindow[1] = GetMUIBuilderString(MSG_CloseWindow);
    ACTWindow[2] = GetMUIBuilderString(MSG_Active);
    ACTWindow[3] = GetMUIBuilderString(MSG_CallFunction);
    ACTWindow[4] = GetMUIBuilderString(MSG_ActiveNoObj);
    ACTWindow[5] = GetMUIBuilderString(MSG_ActiveNextObj);
    ACTWindow[6] = GetMUIBuilderString(MSG_ActivePrevObj);
    ACTWindow[7] = NULL;

    TYWindow[0] = TY_NOTHING;
    TYWindow[1] = TY_NOTHING;
    TYWindow[2] = TY_NOTHING;
    TYWindow[3] = TY_FUNCTION;
    TYWindow[4] = 0;

    EVTWindow[0] = GetMUIBuilderString(MSG_CloseRequest);
    EVTWindow[1] = GetMUIBuilderString(MSG_NotifyActivate);
    EVTWindow[2] = GetMUIBuilderString(MSG_Deactivate);
    EVTWindow[3] = NULL;

/*********************** Button ***********************/
    ACTButton[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTButton[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTButton[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTButton[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTButton[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTButton[5] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTButton[6] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTButton[7] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTButton[8] = GetMUIBuilderString(MSG_CallFunction);
    ACTButton[9] = NULL;

    TYButton[0] = TY_NOTHING;
    TYButton[1] = TY_NOTHING;
    TYButton[2] = TY_NOTHING;
    TYButton[3] = TY_NOTHING;
    TYButton[4] = TY_WINOBJ;
    TYButton[5] = TY_NOTHING;
    TYButton[6] = TY_CONS_STRING;
    TYButton[7] = TY_VARIABLE;
    TYButton[8] = TY_FUNCTION;
    TYButton[9] = 0;

    EVTButton[0] = GetMUIBuilderString(MSG_PressButton);
    EVTButton[1] = GetMUIBuilderString(MSG_ReleaseButton);
    EVTButton[2] = GetMUIBuilderString(MSG_NotifyHide);
    EVTButton[3] = GetMUIBuilderString(MSG_NotifyShow);
    EVTButton[4] = GetMUIBuilderString(MSG_NotifyDisabled);
    EVTButton[5] = GetMUIBuilderString(MSG_NotifyEnabled);
    EVTButton[6] = GetMUIBuilderString(MSG_TimerAction);
    EVTButton[7] = NULL;

/******************* Group *************************/
    ACTGroup[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTGroup[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTGroup[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTGroup[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTGroup[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTGroup[5] = GetMUIBuilderString(MSG_ChangePage);
    ACTGroup[6] = GetMUIBuilderString(MSG_CallFunction);
    ACTGroup[7] = NULL;

    TYGroup[0] = TY_NOTHING;
    TYGroup[1] = TY_NOTHING;
    TYGroup[2] = TY_NOTHING;
    TYGroup[3] = TY_NOTHING;
    TYGroup[4] = TY_WINOBJ;
    TYGroup[5] = TY_NOTHING;
    TYGroup[6] = TY_FUNCTION;
    TYGroup[7] = 0;

    EVTGroup[0] = GetMUIBuilderString(MSG_ChangePage);
    EVTGroup[1] = GetMUIBuilderString(MSG_NotifyHide);
    EVTGroup[2] = GetMUIBuilderString(MSG_NotifyShow);
    EVTGroup[3] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTGroup[4] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTGroup[5] = NULL;

/******************* String ************************/
    ACTString[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTString[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTString[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTString[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTString[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTString[5] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTString[6] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTString[7] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTString[8] = GetMUIBuilderString(MSG_CallFunction);
    ACTString[9] = NULL;

    TYString[0] = TY_NOTHING;
    TYString[1] = TY_NOTHING;
    TYString[2] = TY_NOTHING;
    TYString[3] = TY_NOTHING;
    TYString[4] = TY_WINOBJ;
    TYString[5] = TY_NOTHING;
    TYString[6] = TY_CONS_STRING;
    TYString[7] = TY_VARIABLE;
    TYString[8] = TY_FUNCTION;
    TYString[9] = 0;

    EVTString[0] = GetMUIBuilderString(MSG_Acknowlede);
    EVTString[1] = GetMUIBuilderString(MSG_ChangePosition);
    EVTString[2] = GetMUIBuilderString(MSG_ChangeContent);
    EVTString[3] = GetMUIBuilderString(MSG_NotifyHide);
    EVTString[4] = GetMUIBuilderString(MSG_NotifyShow);
    EVTString[5] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTString[6] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTString[7] = NULL;

/****************** ListView ***********************/
    ACTListview[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTListview[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTListview[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTListview[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTListview[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTListview[5] = GetMUIBuilderString(MSG_ClearList);
    ACTListview[6] = GetMUIBuilderString(MSG_JumpToPosition);
    ACTListview[7] = GetMUIBuilderString(MSG_RedrawAll);
    ACTListview[8] = GetMUIBuilderString(MSG_RedrawActive);
    ACTListview[9] = GetMUIBuilderString(MSG_RemoveActive);
    ACTListview[10] = GetMUIBuilderString(MSG_SortList);
    ACTListview[11] = GetMUIBuilderString(MSG_CallFunction);
    ACTListview[12] = NULL;

    TYListview[0] = TY_NOTHING;
    TYListview[1] = TY_NOTHING;
    TYListview[2] = TY_NOTHING;
    TYListview[3] = TY_NOTHING;
    TYListview[4] = TY_WINOBJ;
    TYListview[5] = TY_NOTHING;
    TYListview[6] = TY_CONS_INT;
    TYListview[7] = TY_NOTHING;
    TYListview[8] = TY_NOTHING;
    TYListview[9] = TY_NOTHING;
    TYListview[10] = TY_NOTHING;
    TYListview[11] = TY_FUNCTION;
    TYListview[12] = 0;

    EVTListview[0] = GetMUIBuilderString(MSG_NotifyDoubleClick);
    EVTListview[1] = GetMUIBuilderString(MSG_ChangeActive);
    EVTListview[2] = GetMUIBuilderString(MSG_NotifyHide);
    EVTListview[3] = GetMUIBuilderString(MSG_NotifyShow);
    EVTListview[4] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTListview[5] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTListview[6] = NULL;

/******************* Gauge *************************/
    ACTGauge[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTGauge[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTGauge[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTGauge[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTGauge[4] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTGauge[5] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTGauge[6] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTGauge[7] = GetMUIBuilderString(MSG_CallFunction);
    ACTGauge[8] = NULL;

    TYGauge[0] = TY_NOTHING;
    TYGauge[1] = TY_NOTHING;
    TYGauge[2] = TY_NOTHING;
    TYGauge[3] = TY_NOTHING;
    TYGauge[4] = TY_NOTHING;
    TYGauge[5] = TY_CONS_INT;
    TYGauge[6] = TY_VARIABLE;
    TYGauge[7] = TY_FUNCTION;
    TYGauge[8] = 0;

    EVTGauge[0] = GetMUIBuilderString(MSG_ChangeValue);
    EVTGauge[1] = GetMUIBuilderString(MSG_NotifyHide);
    EVTGauge[2] = GetMUIBuilderString(MSG_NotifyShow);
    EVTGauge[3] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTGauge[4] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTGauge[5] = NULL;

/******************* Cycle *************************/
    ACTCycle[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTCycle[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTCycle[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTCycle[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTCycle[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTCycle[5] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTCycle[6] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTCycle[7] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTCycle[8] = GetMUIBuilderString(MSG_CallFunction);
    ACTCycle[9] = NULL;

    TYCycle[0] = TY_NOTHING;
    TYCycle[1] = TY_NOTHING;
    TYCycle[2] = TY_NOTHING;
    TYCycle[3] = TY_NOTHING;
    TYCycle[4] = TY_WINOBJ;
    TYCycle[5] = TY_NOTHING;
    TYCycle[6] = TY_CONS_INT;
    TYCycle[7] = TY_VARIABLE;
    TYCycle[8] = TY_FUNCTION;
    TYCycle[9] = 0;

    EVTCycle[0] = GetMUIBuilderString(MSG_ActiveModified);
    EVTCycle[1] = GetMUIBuilderString(MSG_NotifyHide);
    EVTCycle[2] = GetMUIBuilderString(MSG_NotifyShow);
    EVTCycle[3] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTCycle[4] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTCycle[5] = NULL;

/******************* Radio *************************/
    ACTRadio[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTRadio[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTRadio[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTRadio[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTRadio[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTRadio[5] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTRadio[6] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTRadio[7] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTRadio[8] = GetMUIBuilderString(MSG_CallFunction);
    ACTRadio[9] = NULL;

    TYRadio[0] = TY_NOTHING;
    TYRadio[1] = TY_NOTHING;
    TYRadio[2] = TY_NOTHING;
    TYRadio[3] = TY_NOTHING;
    TYRadio[4] = TY_WINOBJ;
    TYRadio[5] = TY_NOTHING;
    TYRadio[6] = TY_CONS_INT;
    TYRadio[7] = TY_VARIABLE;
    TYRadio[8] = TY_FUNCTION;
    TYRadio[9] = 0;

    EVTRadio[0] = GetMUIBuilderString(MSG_ActiveModified);
    EVTRadio[1] = GetMUIBuilderString(MSG_NotifyHide);
    EVTRadio[2] = GetMUIBuilderString(MSG_NotifyShow);
    EVTRadio[3] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTRadio[4] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTRadio[5] = NULL;

/******************* Label *************************/
    ACTLabel[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTLabel[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTLabel[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTLabel[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTLabel[4] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTLabel[5] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTLabel[6] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTLabel[7] = GetMUIBuilderString(MSG_CallFunction);
    ACTLabel[8] = NULL;

    TYLabel[0] = TY_NOTHING;
    TYLabel[1] = TY_NOTHING;
    TYLabel[2] = TY_NOTHING;
    TYLabel[3] = TY_NOTHING;
    TYLabel[4] = TY_NOTHING;
    TYLabel[5] = TY_CONS_STRING;
    TYLabel[6] = TY_VARIABLE;
    TYLabel[7] = TY_FUNCTION;
    TYLabel[8] = 0;

    EVTLabel[0] = GetMUIBuilderString(MSG_NotifyHide);
    EVTLabel[1] = GetMUIBuilderString(MSG_NotifyShow);
    EVTLabel[2] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTLabel[3] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTLabel[4] = NULL;

/******************* Space *************************/
    ACTSpace[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTSpace[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTSpace[2] = GetMUIBuilderString(MSG_CallFunction);
    ACTSpace[3] = NULL;

    TYSpace[0] = TY_NOTHING;
    TYSpace[1] = TY_NOTHING;
    TYSpace[2] = TY_FUNCTION;
    TYSpace[3] = 0;

    EVTSpace[0] = GetMUIBuilderString(MSG_NotifyHide);
    EVTSpace[1] = GetMUIBuilderString(MSG_NotifyShow);
    EVTSpace[2] = NULL;

/******************* Check *************************/
    ACTCheck[0] = GetMUIBuilderString(MSG_Select);
    ACTCheck[1] = GetMUIBuilderString(MSG_Unselect);
    ACTCheck[2] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTCheck[3] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTCheck[4] = GetMUIBuilderString(MSG_NotifyHide);
    ACTCheck[5] = GetMUIBuilderString(MSG_NotifyShow);
    ACTCheck[6] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTCheck[7] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTCheck[8] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTCheck[9] = GetMUIBuilderString(MSG_CallFunction);
    ACTCheck[10] = NULL;

    TYCheck[0] = TY_NOTHING;
    TYCheck[1] = TY_NOTHING;
    TYCheck[2] = TY_NOTHING;
    TYCheck[3] = TY_VARIABLE;
    TYCheck[4] = TY_NOTHING;
    TYCheck[5] = TY_NOTHING;
    TYCheck[6] = TY_NOTHING;
    TYCheck[7] = TY_NOTHING;
    TYCheck[8] = TY_WINOBJ;
    TYCheck[9] = TY_FUNCTION;
    TYCheck[10] = 0;

    EVTCheck[0] = GetMUIBuilderString(MSG_SelectOn);
    EVTCheck[1] = GetMUIBuilderString(MSG_SelectOff);
    EVTCheck[2] = GetMUIBuilderString(MSG_SelectChange);
    EVTCheck[3] = GetMUIBuilderString(MSG_NotifyHide);
    EVTCheck[4] = GetMUIBuilderString(MSG_NotifyShow);
    EVTCheck[5] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTCheck[6] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTCheck[7] = NULL;

/******************* Scale *************************/
    ACTScale[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTScale[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTScale[2] = GetMUIBuilderString(MSG_CallFunction);
    ACTScale[3] = NULL;

    TYScale[0] = TY_NOTHING;
    TYScale[1] = TY_NOTHING;
    TYScale[2] = TY_FUNCTION;
    TYScale[3] = 0;

    EVTScale[0] = GetMUIBuilderString(MSG_NotifyHide);
    EVTScale[1] = GetMUIBuilderString(MSG_NotifyShow);
    EVTScale[2] = NULL;

/******************* Image *************************/
    ACTImage[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTImage[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTImage[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTImage[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTImage[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTImage[5] = GetMUIBuilderString(MSG_CallFunction);
    ACTImage[6] = NULL;

    TYImage[0] = TY_NOTHING;
    TYImage[1] = TY_NOTHING;
    TYImage[2] = TY_NOTHING;
    TYImage[3] = TY_NOTHING;
    TYImage[4] = TY_WINOBJ;
    TYImage[5] = TY_FUNCTION;
    TYImage[6] = 0;

    EVTImage[0] = GetMUIBuilderString(MSG_PressImage);
    EVTImage[1] = GetMUIBuilderString(MSG_ReleaseImage);
    EVTImage[2] = GetMUIBuilderString(MSG_NotifyHide);
    EVTImage[3] = GetMUIBuilderString(MSG_NotifyShow);
    EVTImage[4] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTImage[5] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTImage[6] = NULL;

/******************* Slider ************************/
    ACTSlider[0] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTSlider[1] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTSlider[2] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTSlider[3] = GetMUIBuilderString(MSG_NotifyHide);
    ACTSlider[4] = GetMUIBuilderString(MSG_NotifyShow);
    ACTSlider[5] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTSlider[6] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTSlider[7] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTSlider[8] = GetMUIBuilderString(MSG_CallFunction);
    ACTSlider[9] = NULL;

    TYSlider[0] = TY_NOTHING;
    TYSlider[1] = TY_CONS_INT;
    TYSlider[2] = TY_VARIABLE;
    TYSlider[3] = TY_NOTHING;
    TYSlider[4] = TY_NOTHING;
    TYSlider[5] = TY_NOTHING;
    TYSlider[6] = TY_NOTHING;
    TYSlider[7] = TY_WINOBJ;
    TYSlider[8] = TY_FUNCTION;
    TYSlider[9] = 0;

    EVTSlider[0] = GetMUIBuilderString(MSG_ChangedPosition);
    EVTSlider[1] = GetMUIBuilderString(MSG_Reverse);
    EVTSlider[2] = GetMUIBuilderString(MSG_NotifyHide);
    EVTSlider[3] = GetMUIBuilderString(MSG_NotifyShow);
    EVTSlider[4] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTSlider[5] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTSlider[6] = NULL;

/******************* Text *************************/
    ACTText[0] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTText[1] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTText[2] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTText[3] = GetMUIBuilderString(MSG_NotifyHide);
    ACTText[4] = GetMUIBuilderString(MSG_NotifyShow);
    ACTText[5] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTText[6] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTText[7] = GetMUIBuilderString(MSG_CallFunction);
    ACTText[8] = NULL;

    TYText[0] = TY_NOTHING;
    TYText[1] = TY_CONS_STRING;
    TYText[2] = TY_VARIABLE;
    TYText[3] = TY_NOTHING;
    TYText[4] = TY_NOTHING;
    TYText[5] = TY_NOTHING;
    TYText[6] = TY_NOTHING;
    TYText[7] = TY_FUNCTION;
    TYText[8] = 0;

    EVTText[0] = GetMUIBuilderString(MSG_ChangedText);
    EVTText[1] = GetMUIBuilderString(MSG_NotifyHide);
    EVTText[2] = GetMUIBuilderString(MSG_NotifyShow);
    EVTText[3] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTText[4] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTText[5] = NULL;

/******************* Prop *************************/
    ACTProp[0] = GetMUIBuilderString(MSG_PutTriggerValue);
    ACTProp[1] = GetMUIBuilderString(MSG_PutConstantValue);
    ACTProp[2] = GetMUIBuilderString(MSG_PutVariableValue);
    ACTProp[3] = GetMUIBuilderString(MSG_NotifyHide);
    ACTProp[4] = GetMUIBuilderString(MSG_NotifyShow);
    ACTProp[5] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTProp[6] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTProp[7] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTProp[8] = GetMUIBuilderString(MSG_CallFunction);
    ACTProp[9] = NULL;

    TYProp[0] = TY_NOTHING;
    TYProp[1] = TY_CONS_INT;
    TYProp[2] = TY_VARIABLE;
    TYProp[3] = TY_NOTHING;
    TYProp[4] = TY_NOTHING;
    TYProp[5] = TY_NOTHING;
    TYProp[6] = TY_NOTHING;
    TYProp[7] = TY_WINOBJ;
    TYProp[8] = TY_FUNCTION;
    TYProp[9] = 0;

    EVTProp[0] = GetMUIBuilderString(MSG_ChangedPosition);
    EVTProp[1] = GetMUIBuilderString(MSG_ChangedEntriesNumber);
    EVTProp[2] = GetMUIBuilderString(MSG_ChangedVisibleEntries);
    EVTProp[3] = GetMUIBuilderString(MSG_NotifyHide);
    EVTProp[4] = GetMUIBuilderString(MSG_NotifyShow);
    EVTProp[5] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTProp[6] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTProp[7] = NULL;

/******************* ColorField *************************/
    ACTColorField[0] = GetMUIBuilderString(MSG_PutTriggerRed);
    ACTColorField[1] = GetMUIBuilderString(MSG_PutConstantRed);
    ACTColorField[2] = GetMUIBuilderString(MSG_PutVariableRed);
    ACTColorField[3] = GetMUIBuilderString(MSG_PutTriggerGreen);
    ACTColorField[4] = GetMUIBuilderString(MSG_PutConstantGreen);
    ACTColorField[5] = GetMUIBuilderString(MSG_PutVariableGreen);
    ACTColorField[6] = GetMUIBuilderString(MSG_PutTriggerBlue);
    ACTColorField[7] = GetMUIBuilderString(MSG_PutConstantBlue);
    ACTColorField[8] = GetMUIBuilderString(MSG_PutVariableBlue);
    ACTColorField[9] = GetMUIBuilderString(MSG_NotifyHide);
    ACTColorField[10] = GetMUIBuilderString(MSG_NotifyShow);
    ACTColorField[11] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTColorField[12] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTColorField[13] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTColorField[14] = GetMUIBuilderString(MSG_CallFunction);
    ACTColorField[15] = NULL;

    TYColorField[0] = TY_NOTHING;
    TYColorField[1] = TY_CONS_INT;
    TYColorField[2] = TY_VARIABLE;
    TYColorField[3] = TY_NOTHING;
    TYColorField[4] = TY_CONS_INT;
    TYColorField[5] = TY_VARIABLE;
    TYColorField[6] = TY_NOTHING;
    TYColorField[7] = TY_CONS_INT;
    TYColorField[8] = TY_VARIABLE;
    TYColorField[9] = TY_NOTHING;
    TYColorField[10] = TY_NOTHING;
    TYColorField[11] = TY_NOTHING;
    TYColorField[12] = TY_NOTHING;
    TYColorField[13] = TY_WINOBJ;
    TYColorField[14] = TY_FUNCTION;
    TYColorField[15] = 0;

    EVTColorField[0] = GetMUIBuilderString(MSG_ChangedRed);
    EVTColorField[1] = GetMUIBuilderString(MSG_ChangedGreen);
    EVTColorField[2] = GetMUIBuilderString(MSG_ChangedBlue);
    EVTColorField[3] = GetMUIBuilderString(MSG_NotifyHide);
    EVTColorField[4] = GetMUIBuilderString(MSG_NotifyShow);
    EVTColorField[5] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTColorField[6] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTColorField[7] = NULL;

/******************* PopAsl *************************/
    ACTPopAsl[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTPopAsl[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTPopAsl[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTPopAsl[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTPopAsl[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTPopAsl[5] = GetMUIBuilderString(MSG_CallFunction);
    ACTPopAsl[6] = NULL;

    TYPopAsl[0] = TY_NOTHING;
    TYPopAsl[1] = TY_NOTHING;
    TYPopAsl[2] = TY_NOTHING;
    TYPopAsl[3] = TY_NOTHING;
    TYPopAsl[4] = TY_NOTHING;
    TYPopAsl[5] = TY_FUNCTION;
    TYPopAsl[6] = 0;

    EVTPopAsl[0] = GetMUIBuilderString(MSG_NotifyHide);
    EVTPopAsl[1] = GetMUIBuilderString(MSG_NotifyShow);
    EVTPopAsl[2] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTPopAsl[3] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTPopAsl[4] = NULL;

/******************* PopObject *************************/
    ACTPopObject[0] = GetMUIBuilderString(MSG_NotifyHide);
    ACTPopObject[1] = GetMUIBuilderString(MSG_NotifyShow);
    ACTPopObject[2] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTPopObject[3] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTPopObject[4] = GetMUIBuilderString(MSG_NotifyActivate);
    ACTPopObject[5] = GetMUIBuilderString(MSG_CallFunction);
    ACTPopObject[6] = NULL;

    TYPopObject[0] = TY_NOTHING;
    TYPopObject[1] = TY_NOTHING;
    TYPopObject[2] = TY_NOTHING;
    TYPopObject[3] = TY_NOTHING;
    TYPopObject[4] = TY_WINOBJ;
    TYPopObject[5] = TY_FUNCTION;
    TYPopObject[6] = 0;

    EVTPopObject[0] = GetMUIBuilderString(MSG_NotifyHide);
    EVTPopObject[1] = GetMUIBuilderString(MSG_NotifyShow);
    EVTPopObject[2] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTPopObject[3] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTPopObject[4] = NULL;

/***************** MenuStrip **********************************/
    ACTMenuStrip[0] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTMenuStrip[1] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTMenuStrip[2] = GetMUIBuilderString(MSG_CallFunction);
    ACTMenuStrip[3] = NULL;

    TYMenuStrip[0] = TY_NOTHING;
    TYMenuStrip[1] = TY_NOTHING;
    TYMenuStrip[2] = TY_FUNCTION;
    TYMenuStrip[3] = 0;

    EVTMenuStrip[0] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTMenuStrip[1] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTMenuStrip[2] = NULL;

/***************** Menu **********************************/
    ACTMenu[0] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTMenu[1] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTMenu[2] = GetMUIBuilderString(MSG_ChangeTitleTrigger);
    ACTMenu[3] = GetMUIBuilderString(MSG_ChangeTitleVariable);
    ACTMenu[4] = GetMUIBuilderString(MSG_ChangeTitleConstant);
    ACTMenu[5] = GetMUIBuilderString(MSG_CallFunction);
    ACTMenu[6] = NULL;

    TYMenu[0] = TY_NOTHING;
    TYMenu[1] = TY_NOTHING;
    TYMenu[2] = TY_NOTHING;
    TYMenu[3] = TY_VARIABLE;
    TYMenu[4] = TY_CONS_STRING;
    TYMenu[5] = TY_FUNCTION;
    TYMenu[6] = 0;

    EVTMenu[0] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTMenu[1] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTMenu[2] = NULL;

/***************** MenuItem **********************************/
    ACTMenuItem[0] = GetMUIBuilderString(MSG_NotifyEnable);
    ACTMenuItem[1] = GetMUIBuilderString(MSG_NotifyDisable);
    ACTMenuItem[2] = GetMUIBuilderString(MSG_CheckMenuON);
    ACTMenuItem[3] = GetMUIBuilderString(MSG_CheckMenuOFF);
    ACTMenuItem[4] = GetMUIBuilderString(MSG_CheckableON);
    ACTMenuItem[5] = GetMUIBuilderString(MSG_CheckableOFF);
    ACTMenuItem[6] = GetMUIBuilderString(MSG_ChangeTitleTrigger);
    ACTMenuItem[7] = GetMUIBuilderString(MSG_ChangeTitleVariable);
    ACTMenuItem[8] = GetMUIBuilderString(MSG_ChangeTitleConstant);
    ACTMenuItem[9] = GetMUIBuilderString(MSG_ChangeShortTrigger);
    ACTMenuItem[10] = GetMUIBuilderString(MSG_ChangeShortVariable);
    ACTMenuItem[11] = GetMUIBuilderString(MSG_ChangeShortConstant);
    ACTMenuItem[12] = GetMUIBuilderString(MSG_ToggleFlagON);
    ACTMenuItem[13] = GetMUIBuilderString(MSG_ToggleFlagOFF);
    ACTMenuItem[14] = GetMUIBuilderString(MSG_CallFunction);
    ACTMenuItem[15] = NULL;

    TYMenuItem[0] = TY_NOTHING;
    TYMenuItem[1] = TY_NOTHING;
    TYMenuItem[2] = TY_NOTHING;
    TYMenuItem[3] = TY_NOTHING;
    TYMenuItem[4] = TY_NOTHING;
    TYMenuItem[5] = TY_NOTHING;
    TYMenuItem[6] = TY_NOTHING;
    TYMenuItem[7] = TY_VARIABLE;
    TYMenuItem[8] = TY_CONS_STRING;
    TYMenuItem[9] = TY_NOTHING;
    TYMenuItem[10] = TY_VARIABLE;
    TYMenuItem[11] = TY_CONS_STRING;
    TYMenuItem[12] = TY_NOTHING;
    TYMenuItem[13] = TY_NOTHING;
    TYMenuItem[14] = TY_FUNCTION;
    TYMenuItem[15] = 0;

    EVTMenuItem[0] = GetMUIBuilderString(MSG_MenuTriggered);
    EVTMenuItem[1] = GetMUIBuilderString(MSG_NotifyEnable);
    EVTMenuItem[2] = GetMUIBuilderString(MSG_NotifyDisable);
    EVTMenuItem[3] = GetMUIBuilderString(MSG_CheckMenuON);
    EVTMenuItem[4] = GetMUIBuilderString(MSG_CheckMenuOFF);
    EVTMenuItem[5] = GetMUIBuilderString(MSG_CheckableON);
    EVTMenuItem[6] = GetMUIBuilderString(MSG_CheckableOFF);
    EVTMenuItem[7] = NULL;

}
