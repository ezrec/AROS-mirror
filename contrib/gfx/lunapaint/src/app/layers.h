/****************************************************************************
*                                                                           *
* layers.h -- Lunapaint,                                                    *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/

#ifndef _layers_h_
#define _layers_h_

#include <stdio.h>

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include <exec/types.h>
#include <libraries/mui.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <clib/alib_protos.h>
#include <string.h>

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include "canvas.h"

#include "config.h"

extern Object *WindowLayers;
extern Object *WidgetLayers;
extern Object *ScrollLayers;
extern Object *BtnAddLayer;
extern Object *BtnDelLayer;
extern Object *BtnSwapLayer;
extern Object *BtnMergeLayer;
extern Object *BtnCopyLayer;
extern Object *LayerOpacity;
extern Object *LayerName;
extern Object *LayerVisible;
extern Object *LayerControlGroup;
extern Object *LayerOpacityValue;

extern unsigned int *LayersWidgetTmpBuf;
extern unsigned int layersWidgetWidth;
extern unsigned int layersWidgetHeight;
extern int layersLastScrollPos;
extern int layersUpdateX, layersUpdateY, layersUpdateW, layersUpdateH;

extern struct Hook changeOpacityHook;
extern struct Hook acknowledgeOpacity;
extern struct Hook acknowledgeLayName;
extern struct Hook changeVisibilityHook;
extern struct oCanvas* lastDrawnCanvas;
extern BOOL forceLayerRedraw;

/*
    This function initializes the layer window and the widget
*/
void Init_LayersWindow ( );

/*
    This function returns the dimensions of the layer widget
*/
IPTR layerMinMax ( Class *CLASS, Object *self, struct MUIP_AskMinMax *message );

/*
    Blank box on the layer widget
*/
void layerRenderBlank ( );

/*
    This function renders the layer widget
*/
BOOL layerRender ( Class *CLASS, Object *self );

/*
    Render OS friendly text to the layer preview
*/
void RenderLayerNames ( int x, int y, int w, int h );

/*
    This function handles input on the layer widget
*/
IPTR layerHandleInput ( Class *CLASS, Object *self, struct MUIP_HandleInput *msg );

/*
    Repaint the actual widget
*/
void layersRepaintWindow ( Class *CLASS, Object *self );

/*
    Clean up
*/
void Exit_LayersWindow ( );


#endif

