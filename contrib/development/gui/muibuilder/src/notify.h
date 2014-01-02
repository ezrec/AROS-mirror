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

#ifndef NOTIFY_H
#define NOTIFY_H

#include "builder.h"
#include "notifyconstants.h"

extern BOOL GetConstant(event *, int, BOOL);

/* constantes */
#define ID_evt
#define ID_obj
#define ID_action

/********************* Application ********************/
extern CONST_STRPTR ACTAppli[];
extern int TYAppli[];
extern CONST_STRPTR EVTAppli[];

/*********************** Window ***********************/
extern CONST_STRPTR ACTWindow[];
extern int TYWindow[];
extern CONST_STRPTR EVTWindow[];

/*********************** Button ***********************/
extern CONST_STRPTR ACTButton[];
extern int TYButton[];
extern CONST_STRPTR EVTButton[];

/******************* Group *************************/
extern CONST_STRPTR ACTGroup[];
extern int TYGroup[];
extern CONST_STRPTR EVTGroup[];

/******************* String ************************/
extern CONST_STRPTR ACTString[];
extern int TYString[];
extern CONST_STRPTR EVTString[];

/****************** ListView ***********************/
extern CONST_STRPTR ACTListview[];
extern int TYListview[];
extern CONST_STRPTR EVTListview[];

/******************* Gauge *************************/
extern CONST_STRPTR ACTGauge[];
extern int TYGauge[];
extern CONST_STRPTR EVTGauge[];

/******************* Cycle *************************/
extern CONST_STRPTR ACTCycle[];
extern int TYCycle[];
extern CONST_STRPTR EVTCycle[];

/******************* Radio *************************/
extern CONST_STRPTR ACTRadio[];
extern int TYRadio[];
extern CONST_STRPTR EVTRadio[];

/******************* Label *************************/
extern CONST_STRPTR ACTLabel[];
extern int TYLabel[];
extern CONST_STRPTR EVTLabel[];

/******************* Space *************************/
extern CONST_STRPTR ACTSpace[];
extern int TYSpace[];
extern CONST_STRPTR EVTSpace[];

/******************* Check *************************/
extern CONST_STRPTR ACTCheck[];
extern int TYCheck[];
extern CONST_STRPTR EVTCheck[];

/******************* Scale *************************/
extern CONST_STRPTR ACTScale[];
extern int TYScale[];
extern CONST_STRPTR EVTScale[];

/******************* Image *************************/
extern CONST_STRPTR ACTImage[];
extern int TYImage[];
extern CONST_STRPTR EVTImage[];

/******************* Slider ************************/
extern CONST_STRPTR ACTSlider[];
extern int TYSlider[];
extern CONST_STRPTR EVTSlider[];

/******************* Text *************************/
extern CONST_STRPTR ACTText[];
extern int TYText[];
extern CONST_STRPTR EVTText[];

/******************* Prop *************************/
extern CONST_STRPTR ACTProp[];
extern int TYProp[];
extern CONST_STRPTR EVTProp[];

/******************* ColorField *************************/
extern CONST_STRPTR ACTColorField[];
extern int TYColorField[];
extern CONST_STRPTR EVTColorField[];

/******************* PopAsl *************************/
extern CONST_STRPTR ACTPopAsl[];
extern int TYPopAsl[];
extern CONST_STRPTR EVTPopAsl[];

/******************* PopObject *************************/
extern CONST_STRPTR ACTPopObject[];
extern int TYPopObject[];
extern CONST_STRPTR EVTPopObject[];

/******************* MenuStrip *****************************/
extern CONST_STRPTR ACTMenuStrip[];
extern int TYMenuStrip[];
extern CONST_STRPTR EVTMenuStrip[];

/******************* Menu *****************************/
extern CONST_STRPTR ACTMenu[];
extern int TYMenu[];
extern CONST_STRPTR EVTMenu[];

/******************* MenuItem *************************/
extern CONST_STRPTR ACTMenuItem[];
extern int TYMenuItem[];
extern CONST_STRPTR EVTMenuItem[];

/*************************** la totale *************************/

extern CONST_STRPTR *Evenements[];
extern CONST_STRPTR *Actions[];
extern int *TYActions[];

#endif
