#ifndef _PRIVATE_H
#define _PRIVATE_H

/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id: private.h 22 2007-01-02 21:30:56Z damato $

***************************************************************************/

#include <mui/TheBar_mcc.h>

#include "TheBar_mcp.h"

/***************************************************************************/

extern ULONG lib_flags;
extern struct Catalog *lib_cat;

extern struct MUI_CustomClass *ThisClass;

#if !defined(__MORPHOS__) && !defined(__amigaos4__)
extern struct MUI_CustomClass *lib_coloradjust;
extern struct MUI_CustomClass *lib_penadjust;
extern struct MUI_CustomClass *lib_backgroundadjust;
extern struct MUI_CustomClass *lib_poppen;
extern struct MUI_CustomClass *lib_popbackground;
#endif

enum
{
  BASEFLG_Init    = 1<<0,
  BASEFLG_MUI20   = 1<<1,
  BASEFLG_MUI4    = 1<<2,
  BASEFLG_MORPHOS = 1<<3,
};

/***************************************************************************/

struct InstData
{
  Object  *menu;

  Object  *groupBack;
  Object  *useGroupBack;

  Object  *buttonBack;
  Object  *useButtonBack;

  Object  *frameShinePen;
  Object  *frameShadowPen;
  Object  *frameStyle;

  Object  *disBodyPen;
  Object  *disShadowPen;

  Object  *barSpacerShinePen;
  Object  *barSpacerShadowPen;

  #if defined(__MORPHOS__) || defined(__amigaos4__)
  Object  *frame;
  #else
  Object  *barFrameShinePen;
  Object  *barFrameShadowPen;
  #endif

  Object  *dragBarShinePen;
  Object  *dragBarShadowPen;
  Object  *dragBarFillPen;
  Object  *useDragBarFillPen;

  Object  *textFont;
  Object  *textGfxFont;
  Object  *textOverUseShine;
  Object  *viewMode;
  Object  *labelPos;
  Object  *borderless;
  Object  *raised;
  Object  *sunny;
  Object  *scaled;
  Object  *barSpacer;
  Object  *enableKeys;

  Object  *horizSpacing;
  Object  *vertSpacing;
  Object  *barSpacerSpacing;

  Object  *horizInnerSpacing;
  Object  *topInnerSpacing;
  Object  *bottomInnerSpacing;

  #if !defined(__MORPHOS__) && !defined(__amigaos4__)
  Object  *leftBarFrameSpacing;
  Object  *rightBarFrameSpacing;
  Object  *topBarFrameSpacing;
  Object  *bottomBarFrameSpacing;
  #endif

  Object  *horizTexGfxSpacing;
  Object  *vertTexGfxSpacing;

  Object  *precision;
  Object  *scale;
  Object  *specialSelect;

  Object  *ignoreSel;
  Object  *ignoreDis;
  Object  *dontMove;
  Object  *ntRaiseActive;

  Object  *disMode;

  Object  *spacersSize;

  STRPTR  base;
  ULONG   flags;
};

/***************************************************************************/


#endif /* _PRIVATE_H */

