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

struct InstData
{
    APTR                        pool;                   // private memory pool for allocations
    Object                      *tb;                    // Parent TheBar object, if any
    ULONG                       id;                     // As it says

    struct Screen               *screen;                // _screen(obj) local copy
    ULONG                       screenDepth;            // screen depth
    struct RastPort             rp;                     // _rp(obj)

    struct MUIS_TheBar_Strip    *strip;

    ULONG                       imgWidth;               // Image width
    ULONG                       imgHeight;              // Image height

    struct MUIS_TheBar_Brush    *image;                 // Brush
    struct BitMap               *normalBM;              // Normal BitMap
    struct BitMap               *greyBM;                // Sunny BitMap
    APTR                        mask;                   // Normal mask
    struct pen                  pens[256];              // Allocated pens
    struct pen                  gpens[256];             // Grey allocated pens

    struct MUIS_TheBar_Brush    *simage;                // Selected brush
    struct BitMap               *snormalBM;             // Selected BitMap
    struct BitMap               *sgreyBM;               // Selected sunny BitMap
    APTR                        smask;                  // Selected mask
    struct pen                  spens[256];             // Allocated pens
    struct pen                  sgpens[256];            // Grey allocated pens

    struct MUIS_TheBar_Brush    *dimage;                // Disabled brush
    struct BitMap               *dnormalBM;             // Disabled BitMap
    struct BitMap               *dgreyBM;               // Disabled sunny BitMap
    APTR                        dmask;                  // Disabled mask
    struct pen                  dpens[256];             // Allocated pens
    struct pen                  dgpens[256];            // Grey allocated pens

    APTR                        activeBack;             // Button background
    APTR                        parentBack;             // Parent background

    LONG                        shine;                  // Frame shine pen
    LONG                        shadow;                 // Frame shadow pen
    LONG                        disBodyPen;             // Disabled body pen
    LONG                        disShadowPen;           // Disabled shadow pen

    ULONG                       vMode;                  // ViewMode
    ULONG                       fStyle;                 // Frame style
    ULONG                       fSize;                  // Frame size
    ULONG                       scale;                  // Scaled % ratio
    ULONG                       disMode;                // Disable mode
    ULONG                       precision;              // Remapping precision

    UBYTE                       blabel[TB_MAXLABELLEN]; // Button label
    UBYTE                       *end;                   // Point to text followin _?
    UBYTE                       cchar;                  // Char after _
    ULONG                       labelLen;               // blabel strlen
    ULONG                       lPos;                   // Label position
    struct TextExtent           lInfo;                  // blabel TextExtent
    struct TextExtent           eInfo;                  // end TextExtent
    struct TextExtent           ccInfo;                 // cchar TextExtent

    ULONG                       stripsRows;
    ULONG                       stripsCols;
    ULONG                       stripHorizSpace;
    ULONG                       stripVertSpace;

    struct MUI_EventHandlerNode eh;                     // Event handler

    struct TextFont             *tf;                    // Text font
    struct TextFont             *tgf;                   // TextGfx font

    ULONG                       horizInnerSpacing;      // Horiz Inner spacing
    ULONG                       topInnerSpacing;        // Top Inner spacing
    ULONG                       bottomInnerSpacing;     // Bottom Inner spacing

    ULONG                       horizTextGfxSpacing;    // Text/Gfx horiz spacing
    ULONG                       vertTextGfxSpacing;     // Text/Gfx vert spacing

    ULONG                       flags;                  // As it says
    ULONG                       flags2;                 // As it says
    ULONG                       userFlags;              // As it says

    UBYTE                         *nchunky;               // Normal chunky
    UBYTE                         *gchunky;               // Sunny chunky
    UBYTE                         *snchunky;              // Selected chunky
    UBYTE                         *sgchunky;              // Selected sunny chunky
    UBYTE                         *dnchunky;              // Disabled chunky
    UBYTE                         *dgchunky;              // Disabled sunny chunky

    struct MinList              notifyList;             // list of set notifies on the button

    ULONG                       qualifier;              // cureently active RAWKEY-Qualifiers

    #if defined(__amigaos3__)
    BOOL                        allowAlphaChannel;
    #endif
};

/* flags */
enum
{
    FLG_Setup              = 1<<0,  // MUIM_Setup
    FLG_Visible            = 1<<1,  // MUIM_Show
    FLG_MouseOver          = 1<<2,  // Mouse is currently over the button
    FLG_Handler            = 1<<3,  // Event added
    FLG_RedrawBack         = 1<<4,  // MUIM_Draw will be called to redraw background

    FLG_Raised             = 1<<5,  // Rasing frame
    FLG_Borderless         = 1<<6,  // Borderless
    FLG_Sunny              = 1<<7,  // Sunny
    FLG_Scaled             = 1<<8,  // Scaled

    FLG_Horiz              = 1<<9,  // Horiz
    FLG_ShowMe             = 1<<10, // MUIA_ShowMe
    FLG_Disabled           = 1<<11, // MUIA_Disabled
    FLG_Selected           = 1<<12, // MUIA_Selected
    FLG_IsInVirtgroup      = 1<<13, // Some parent is a Virtgroup

    FLG_IsSpacer           = 1<<14, // It is a spacer
    FLG_NoNotify           = 1<<15, // No notification
    FLG_NoClick            = 1<<16, // No input

    FLG_CyberMap           = 1<<17, // Cyber screen
    FLG_CyberDeep          = 1<<18, // Cyber screen and depth>256

    FLG_CloseTF            = 1<<19, // tf opened
    FLG_CloseTGF           = 1<<20, // tgf opened

    FLG_EnableKey          = 1<<21, // Key char shortcut in use

    FLG_Strip              = 1<<22,
};

/* flags2 */
enum
{
    FLG2_Limbo             = 1<<0,  // Limbo mode ON
    FLG2_Special           = 1<<1,  // Special mode ON
    FLG2_InVirtGroup       = 1<<2,  // In a Virtgroup for sure
};

/* userFlags */
enum
{
    UFLG_UserHorizTextGfxSpacing = 1<<0,  // Forced HorizTextGfxSpacing
    UFLG_UserVertTextGfxSpacing  = 1<<1,  // Forced VertTextGfxSpacing
    UFLG_UserHorizInnerSpacing   = 1<<2,  // Forced HorizInnerSpacing
    UFLG_UserTopInnerSpacing     = 1<<3,  // Forced TopInnerSpacing
    UFLG_UserBottomInnerSpacing  = 1<<4,  // Forced BottomInnerSpacing
    UFLG_UserPrecision           = 1<<5,  // Forced Precision
    UFLG_UserScale               = 1<<7,  // Forced Scale
    UFLG_UserDisMode             = 1<<6,  // Forced DisMode
    UFLG_UserSpecialSelect       = 1<<9,  // Forced SpecialSelect
    UFLG_SpecialSelect           = 1<<10, // SpecialSelect?
    UFLG_UserTextOverUseShine    = 1<<11, // Forced TextOverUseShine
    UFLG_TextOverUseShine        = 1<<12, // TextOverUseShine?
    UFLG_UserIgnoreSelImages     = 1<<13, // Forced IgnoreSelImages
    UFLG_IgnoreSelImages         = 1<<14, // IgnoreSelImages?
    UFLG_UserIgnoreDisImages     = 1<<15, // Forced IgnoreDisImages
    UFLG_IgnoreDisImages         = 1<<16, // IgnoreDisImages?
    UFLG_UserDontMove            = 1<<17,
    UFLG_DontMove                = 1<<18,
    UFLG_UserNtRaiseActive       = 1<<19,
    UFLG_NtRaiseActive           = 1<<20,
};

// Notify
struct ButtonNotify
{
  struct MinNode     node;  // to link it in the notifies list of a button
  struct MUIP_Notify msg;   // the cached MUI Notify message
};

/***********************************************************************/

/* build.c */
void scaleStripBitMaps(struct InstData *data);
void freeBitMaps(struct InstData *data);
void build(struct InstData *data);

/***********************************************************************/

// new attribute of MUI 4.0 to indicate that a class knows a custom way to indicate disabled objects
#ifndef MUIA_KnowsDisabled
#define MUIA_KnowsDisabled                 0x8042deef /* V20 isg BOOL              */
#endif

