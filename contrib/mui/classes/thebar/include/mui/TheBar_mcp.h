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

***************************************************************************/

#ifndef _THEBAR_MCP_H_
#define _THEBAR_MCP_H_

/***********************************************************************/

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#if !defined(__AROS__) && defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

/***********************************************************************/

#define TBPTAGBASE 0xF76B0164

/***********************************************************************/
/*
** Settings
**/

#define MUICFG_TheBar_GroupBack             (TBPTAGBASE+0)   /* v11 ImageSpec         */
#define MUICFG_TheBar_UseGroupBack          (TBPTAGBASE+1)   /* v11 ULONG             */
#define MUICFG_TheBar_ButtonBack            (TBPTAGBASE+2)   /* v11 ImageSpec         */
#define MUICFG_TheBar_UseButtonBack         (TBPTAGBASE+3)   /* v11 ULONG             */
#define MUICFG_TheBar_FrameShinePen         (TBPTAGBASE+4)   /* v11 PenSpec           */
#define MUICFG_TheBar_FrameShadowPen        (TBPTAGBASE+5)   /* v11 PenSpec           */
#define MUICFG_TheBar_FrameStyle            (TBPTAGBASE+6)   /* v11 ULONG             */
#define MUICFG_TheBar_DisBodyPen            (TBPTAGBASE+7)   /* v11 PenSpec           */
#define MUICFG_TheBar_DisShadowPen          (TBPTAGBASE+8)   /* v11 PenSpec           */
#define MUICFG_TheBar_BarSpacerShinePen     (TBPTAGBASE+9)   /* v11 PenSpec           */
#define MUICFG_TheBar_BarSpacerShadowPen    (TBPTAGBASE+10)  /* v11 PenSpec           */
#define MUICFG_TheBar_BarFrameShinePen      (TBPTAGBASE+11)  /* v11 PenSpec           */
#define MUICFG_TheBar_BarFrameShadowPen     (TBPTAGBASE+12)  /* v11 PenSpec           */
#define MUICFG_TheBar_DragBarShinePen       (TBPTAGBASE+13)  /* v11 PenSpec           */
#define MUICFG_TheBar_DragBarShadowPen      (TBPTAGBASE+14)  /* v11 PenSpec           */
#define MUICFG_TheBar_DragBarFillPen        (TBPTAGBASE+15)  /* v17 PenSpec           */
#define MUICFG_TheBar_UseDragBarFillPen     (TBPTAGBASE+16)  /* v17 BOOL              */

#define MUICFG_TheBar_TextFont              (TBPTAGBASE+20)  /* v11 STRPTR            */
#define MUICFG_TheBar_TextGfxFont           (TBPTAGBASE+21)  /* v11 STRPTR            */

#define MUICFG_TheBar_HorizSpacing          (TBPTAGBASE+30)  /* v11 ULONG             */
#define MUICFG_TheBar_VertSpacing           (TBPTAGBASE+31)  /* v11 ULONG             */
#define MUICFG_TheBar_BarSpacerSpacing      (TBPTAGBASE+32)  /* v11 ULONG             */
#define MUICFG_TheBar_HorizInnerSpacing     (TBPTAGBASE+33)  /* v11 ULONG             */
#define MUICFG_TheBar_TopInnerSpacing       (TBPTAGBASE+34)  /* v11 ULONG             */
#define MUICFG_TheBar_BottomInnerSpacing    (TBPTAGBASE+35)  /* v11 ULONG             */
#define MUICFG_TheBar_LeftBarFrameSpacing   (TBPTAGBASE+36)  /* v11 ULONG             */
#define MUICFG_TheBar_RightBarFrameSpacing  (TBPTAGBASE+37)  /* v11 ULONG             */
#define MUICFG_TheBar_TopBarFrameSpacing    (TBPTAGBASE+38)  /* v11 ULONG             */
#define MUICFG_TheBar_BottomBarFrameSpacing (TBPTAGBASE+39)  /* v11 ULONG             */
#define MUICFG_TheBar_HorizTextGfxSpacing   (TBPTAGBASE+40)  /* v11 ULONG             */
#define MUICFG_TheBar_VertTextGfxSpacing    (TBPTAGBASE+41)  /* v11 ULONG             */

#define MUICFG_TheBar_Precision             (TBPTAGBASE+60)  /* v11 ULONG             */
#define MUICFG_TheBar_Event                 (TBPTAGBASE+61)  /* v11 ULONG             */
#define MUICFG_TheBar_Scale                 (TBPTAGBASE+62)  /* v11 ULONG             */
#define MUICFG_TheBar_SpecialSelect         (TBPTAGBASE+63)  /* v11 ULONG             */
#define MUICFG_TheBar_TextOverUseShine      (TBPTAGBASE+64)  /* v11 ULONG             */
#define MUICFG_TheBar_IgnoreSelImages       (TBPTAGBASE+65)  /* v12 ULONG             */
#define MUICFG_TheBar_IgnoreDisImages       (TBPTAGBASE+66)  /* v12 ULONG             */
#define MUICFG_TheBar_DisMode               (TBPTAGBASE+67)  /* v12 ULONG             */
#define MUICFG_TheBar_DontMove              (TBPTAGBASE+68)  /* v15 ULONG             */
#define MUICFG_TheBar_Gradient              (TBPTAGBASE+80)  /* v17 ULONG             */
#define MUICFG_TheBar_NtRaiseActive         (TBPTAGBASE+81)  /* v18 BOOL              */
#define MUICFG_TheBar_SpacersSize           (TBPTAGBASE+82)  /* v18 ULONG             */
#define MUICFG_TheBar_Appearance            (TBPTAGBASE+83)  /* v19 struct Appearance */

#define MUICFG_TheBar_Frame                 (TBPTAGBASE+84)  /* v19 struct Framespec  */
#define MUICFG_TheBar_ButtonFrame           (TBPTAGBASE+85)  /* v19 struct Framespec  */

struct MUIS_TheBar_Gradient
{
    ULONG from;
    ULONG to;
    ULONG flags;
};

enum
{
    MUIV_TheBar_Gradient_Horiz  = 1<<0,
    MUIV_TheBar_Gradient_DragTo = 1<<1, /* Private */
};

/***********************************************************************/
/*
** Defaults
*/

#define MUIDEF_TheBar_GroupBack             ((APTR)"2:m1")
#define MUIDEF_TheBar_UseGroupBack          FALSE
#define MUIDEF_TheBar_Frame                 ((APTR)"000000")
#define MUIDEF_TheBar_ButtonBack            ((APTR)"2:m1")
#define MUIDEF_TheBar_UseButtonBack         FALSE
#define MUIDEF_TheBar_ButtonFrame           ((APTR)"602222")

#define MUIDEF_TheBar_DisBodyPen            ((APTR)"r44444444,44444444,44444444")
#define MUIDEF_TheBar_DisShadowPen          ((APTR)"rdddddddd,dddddddd,dddddddd")

#define MUIDEF_TheBar_BarSpacerShinePen     ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_TheBar_BarSpacerShadowPen    ((APTR)"r00000000,00000000,00000000")
#define MUIDEF_TheBar_BarFrameShinePen      ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_TheBar_BarFrameShadowPen     ((APTR)"r00000000,00000000,00000000")

#define MUIDEF_TheBar_DragBarShinePen       ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_TheBar_DragBarShadowPen      ((APTR)"r00000000,00000000,00000000")
#define MUIDEF_TheBar_DragBarFillPen        ((APTR)"rCCCCCCCC,CCCCCCCC,CCCCCCCC")
#define MUIDEF_TheBar_UseDragBarFillPen     FALSE

#define MUIDEF_TheBar_FrameShinePen         ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_TheBar_FrameShadowPen        ((APTR)"r00000000,00000000,00000000")
#define MUIDEF_TheBar_FrameStyle            MUIV_TheButton_FrameStyle_Normal

#define MUIDEF_TheBar_TextFont              NULL
#define MUIDEF_TheBar_TextGfxFont           NULL

#define MUIDEF_TheBar_HorizSpacing          1
#define MUIDEF_TheBar_VertSpacing           1
#define MUIDEF_TheBar_BarSpacerSpacing      4
#define MUIDEF_TheBar_HorizInnerSpacing     1
#define MUIDEF_TheBar_TopInnerSpacing       0
#define MUIDEF_TheBar_BottomInnerSpacing    0
#define MUIDEF_TheBar_LeftBarFrameSpacing   2
#define MUIDEF_TheBar_RightBarFrameSpacing  2
#define MUIDEF_TheBar_TopBarFrameSpacing    2
#define MUIDEF_TheBar_BottomBarFrameSpacing 2
#define MUIDEF_TheBar_HorizTextGfxSpacing   1
#define MUIDEF_TheBar_VertTextGfxSpacing    1

#define MUIDEF_TheBar_Precision             MUIV_TheBar_Precision_Image
#define MUIDEF_TheBar_NotMUI20Event         MUIV_TheBar_Event_MouseMove
#define MUIDEF_TheBar_Event                 MUIV_TheBar_Event_MouseObject
#define MUIDEF_TheBar_Scale                 66
#define MUIDEF_TheBar_SpecialSelect         FALSE
#define MUIDEF_TheBar_TextOverUseShine      FALSE
#define MUIDEF_TheBar_IgnoreSelImages       FALSE
#define MUIDEF_TheBar_IgnoreDisImages       FALSE
#define MUIDEF_TheBar_DisMode               MUIV_TheBar_DisMode_Shape
#define MUIDEF_TheBar_DontMove              FALSE
#define MUIDEF_TheBar_NtRaiseActive         FALSE

#define MUIDEF_TheBar_SpacersSize           MUIV_TheBar_SpacersSize_Quarter

#define MUIDEF_TheBar_Appearance_ViewMode   MUIV_TheBar_ViewMode_TextGfx
#define MUIDEF_TheBar_Appearance_LabelPos   MUIV_TheBar_LabelPos_Bottom
#define MUIDEF_TheBar_Appearance_Flags      (MUIV_TheBar_Appearance_EnableKeys|MUIV_TheBar_Appearance_Borderless)

/***********************************************************************/

#ifndef _backspec
#ifdef __AROS__
#define _backspec(obj) ({IPTR tmp = 0; GetAttr(MUIA_Background, (Object *)(obj), &tmp); (APTR)tmp; })
#else
#define _backspec(obj) ((APTR)(*((ULONG *)(((char *)(obj))+80))))
#endif
#endif

/***********************************************************************/

#if !defined(__AROS__) && defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack()
  #elif defined(__VBCC__)
    #pragma default-align
  #endif
#endif

#endif /* _THEBAR_MCP_H_ */
