/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

struct InstData
{
  Object  *menu;

  Object  *groupBack;
  Object  *useGroupBack;
  Object  *frame;
  Object  *barFrameShinePen;
  Object  *barFrameShadowPen;

  Object  *buttonBack;
  Object  *useButtonBack;
  Object  *frameShinePen;
  Object  *frameShadowPen;
  Object  *frameStyle;

  Object  *disBodyPen;
  Object  *disShadowPen;

  Object  *barSpacerShinePen;
  Object  *barSpacerShadowPen;

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

  Object  *leftBarFrameSpacing;
  Object  *rightBarFrameSpacing;
  Object  *topBarFrameSpacing;
  Object  *bottomBarFrameSpacing;

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

#define VERSION_IS_AT_LEAST(ver, rev, minver, minrev) (((ver) > (minver)) || ((ver) == (minver) && (rev) == (minrev)) || ((ver) == (minver) && (rev) > (minrev)))
#define LIB_VERSION_IS_AT_LEAST(lib, minver, minrev)  VERSION_IS_AT_LEAST(((struct Library *)(lib))->lib_Version, ((struct Library *)(lib))->lib_Revision, minver, minrev)

/***************************************************************************/

#ifndef MUIA_Scrollgroup_AutoBars
#define MUIA_Scrollgroup_AutoBars           0x8042f50e /* V20 isg BOOL              */
#endif

/***************************************************************************/
