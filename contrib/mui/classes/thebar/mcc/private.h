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

struct pen
{
    UBYTE pen;
    UBYTE done;
};

#define STATICSORTSIZE 31

struct InstData
{
    struct MinList                 buttons;

    struct Screen                  *screen;
    ULONG                          screenDepth;

    struct MUIS_TheBar_Brush       **brushes;
    struct MUIS_TheBar_Brush       **sbrushes;
    struct MUIS_TheBar_Brush       **dbrushes;
    struct MUIS_TheBar_Strip       strip;
    ULONG                          nbr;

    struct MUIS_TheBar_Brush       image;
    struct pen                     pens[256];
    struct pen                     gpens[256];

    struct MUIS_TheBar_Brush       simage;
    struct pen                     spens[256];
    struct pen                     sgpens[256];

    struct MUIS_TheBar_Brush       dimage;
    struct pen                     dpens[256];
    struct pen                     dgpens[256];

    Object                         *db;

    ULONG                          spacer;
    ULONG                          barPos;
    ULONG                          viewMode;
    ULONG                          labelPos;

    ULONG                          buttonWidth;
    ULONG                          buttonHeight;
    ULONG                          width;
    ULONG                          height;

    ULONG                          precision;
    ULONG                          scale;
    ULONG                          disMode;
    ULONG                          active;
    ULONG                          changes;
    ULONG                          remove;

    LONG                           cols;
    LONG                           rows;
    LONG                           lcols;
    LONG                           lrows;

    ULONG                          horizSpacing;
    ULONG                          vertSpacing;
    ULONG                          leftBarFrameSpacing;
    ULONG                          rightBarFrameSpacing;
    ULONG                          topBarFrameSpacing;
    ULONG                          bottomBarFrameSpacing;
    ULONG                          barSpacerSpacing;
    ULONG                          horizInnerSpacing;
    ULONG                          topInnerSpacing;
    ULONG                          bottomInnerSpacing;
    ULONG                          horizTextGfxSpacing;
    ULONG                          vertTextGfxSpacing;
    LONG                           barFrameShinePen;
    LONG                           barFrameShadowPen;

    ULONG                          sortMsgID;
    ULONG                          sort[STATICSORTSIZE+1];

    ULONG                          stripsRows;
    ULONG                          stripsCols;
    ULONG                          stripHorizSpace;
    ULONG                          stripVertSpace;

    struct MUI_DragImage           *dm;
    struct MUIS_TheBar_DragImage   di;

    struct BitMap                  *gradbm;
    struct MUIS_TheBar_Gradient    grad;

    struct MUIS_TheBar_Appearance  ap;

    ULONG                          id;

    ULONG                          spacersSize;

    #if defined(VIRTUAL)
    ULONG                          objWidth;
    ULONG                          objMWidth;
    ULONG                          objHeight;
    ULONG                          objMHeight;
    #endif

    char                           frameSpec[256];
    ULONG                          userFrame;

    #if defined(__amigaos3__)
    BOOL                           allowAlphaChannel;
    #endif

    ULONG                          flags;
    ULONG                          userFlags;
    ULONG                          flags2;
    ULONG                          userFlags2;

    struct MUI_EventHandlerNode    eh;
};

/* flags */
enum
{
    FLG_Horiz              = 1<<0,
    FLG_Borderless         = 1<<1,
    FLG_Raised             = 1<<2,
    FLG_Scaled             = 1<<3,
    FLG_Sunny              = 1<<4,
    FLG_EnableKeys         = 1<<5,
    FLG_Setup              = 1<<6,
    FLG_Limbo              = 1<<7,
    FLG_TextOnly           = 1<<8,
    FLG_FreeBrushes        = 1<<9,
    FLG_FreeStrip          = 1<<10,
    FLG_Background         = 1<<11,
    FLG_IsInVirtgroup      = 1<<12,
    FLG_DragBar            = 1<<13,
    FLG_Framed             = 1<<14,
    FLG_Table              = 1<<15,
    FLG_FreeHoriz          = 1<<16,
    FLG_FreeVert           = 1<<17,
    FLG_BarSpacer          = 1<<18,
    FLG_Strip              = 1<<20,
    FLG_CyberMap           = 1<<21,
    FLG_CyberDeep          = 1<<22,
    FLG_RebuildbitMaps     = 1<<23,
};

/* flags 2 */
enum
{
    FLG2_Gradient            = 1<<0,
    FLG2_IgnoreAppearance    = 1<<1,
    FLG2_ForceWindowActivity = 1<<2,
    FLG2_EventHandler        = 1<<3,
};

/* userFlags */
enum
{
    UFLG_UserHorizSpacing          = 1<<0,
    UFLG_UserVertSpacing           = 1<<1,
    UFLG_UserBarSpacerSpacing      = 1<<2,
    UFLG_UserHorizInnerSpacing     = 1<<3,
    UFLG_UserTopInnerSpacing       = 1<<4,
    UFLG_UserBottomInnerSpacing    = 1<<5,
    UFLG_UserLeftBarFrameSpacing   = 1<<6,
    UFLG_UserRightBarFrameSpacing  = 1<<7,
    UFLG_UserTopBarFrameSpacing    = 1<<8,
    UFLG_UserBottomBarFrameSpacing = 1<<9,
    UFLG_UserHorizTextGfxSpacing   = 1<<10,
    UFLG_UserVertTextGfxSpacing    = 1<<11,
    UFLG_UserPrecision             = 1<<12,
    UFLG_UserScale                 = 1<<13,
    UFLG_UserDisMode               = 1<<14,
    UFLG_UserSpecialSelect         = 1<<15,
    UFLG_SpecialSelect             = 1<<16,
    UFLG_UserTextOverUseShine      = 1<<17,
    UFLG_TextOverUseShine          = 1<<18,
    UFLG_UserIgnoreSelImages       = 1<<19,
    UFLG_IgnoreSelImages           = 1<<20,
    UFLG_UserIgnoreDisImages       = 1<<21,
    UFLG_IgnoreDisImages           = 1<<22,
};

/* userFlags2 */
enum
{
    UFLG2_UserDontMove             = 1<<0,
    UFLG2_DontMove                 = 1<<1,
    UFLG2_UserNtRaiseActive        = 1<<2,
    UFLG2_NtRaiseActive            = 1<<3,
    UFLG2_UserSpacersSize          = 1<<4,
    UFLG2_UserFrame                = 1<<5,
};

/***********************************************************************/

// Button
struct Button
{
  struct MinNode node;
  ULONG          ID;
  Object         *obj;
  ULONG          img;
  const char     *text;
  const char     *help;
  struct IClass  *class;
  ULONG          exclude;
  struct MinList notifyListClone;

  ULONG          flags;
};

// Notify
struct ButtonNotify
{
  struct MinNode     node;  // to link it in the notifies list of a button
  struct MUIP_Notify msg;   // the cached MUI Notify message
};

enum
{
    BFLG_NoClick   = 1<<0,
    BFLG_Immediate = 1<<1,
    BFLG_Toggle    = 1<<2,
    BFLG_Disabled  = 1<<3,
    BFLG_Selected  = 1<<4,
    BFLG_Sleep     = 1<<5,
    BFLG_Hide      = 1<<6,

    BFLG_TableHide = 1<<23,
};

/***********************************************************************/

/* build.c */
void build(struct InstData *data);
void freeBitMaps(struct InstData *data);

/***********************************************************************/

