#ifndef BGUI_IMAGE_H
#define BGUI_IMAGE_H
/*
 * @(#) $Header$
 *
 * $VER: bgui/bgui_image.h 41.10 (25.4.96)
 *
 * Image classes structures and constants.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.11  2000/05/09 20:01:48  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:13:47  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:15:50  mlemos
 * Ian sources
 *
 *
 */

#ifndef INTUITION_IMAGECLASS_H
#include <intuition/imageclass.h>
#endif /* INTUITION_IMAGECLASS_H */

/*****************************************************************************
 *
 *      "frameclass" - BOOPSI framing image.
 *
 *      Tags: 1 - 80    Methods: 21 - 40
 */
#define FRM_TAGSTART                    (BGUI_TB+1)
#define FRM_Type                        (BGUI_TB+1)     /* ISG-- */
#define FRM_CustomHook                  (BGUI_TB+2)     /* ISG-- */
#define FRM_BackFillHook                (BGUI_TB+3)     /* ISG-- */
#define FRM_Title                       (BGUI_TB+4)     /* ISG-- */
#define FRM_TextAttr                    (BGUI_TB+5)     /* ISG-- */
#define FRM_Flags                       (BGUI_TB+6)     /* ISG-- */
#define FRM_FrameWidth                  (BGUI_TB+7)     /* ISG-- */
#define FRM_FrameHeight                 (BGUI_TB+8)     /* ISG-- */
#define FRM_BackFill                    (BGUI_TB+9)     /* ISG-- */
#define FRM_EdgesOnly                   (BGUI_TB+10)    /* ISG-- */
#define FRM_Recessed                    (BGUI_TB+11)    /* ISG-- */
#define FRM_CenterTitle                 (BGUI_TB+12)    /* ISG-- */
#define FRM_HighlightTitle              (BGUI_TB+13)    /* ISG-- */
#define FRM_ThinFrame                   (BGUI_TB+14)    /* ISG-- */
#define FRM_BackPen                     (BGUI_TB+15)    /* ISG-- */  /* V39 */
#define FRM_SelectedBackPen             (BGUI_TB+16)    /* ISG-- */  /* V39 */
#define FRM_BackDriPen                  (BGUI_TB+17)    /* ISG-- */  /* V39 */
#define FRM_SelectedBackDriPen          (BGUI_TB+18)    /* ISG-- */  /* V39 */
#define FRM_TitleLeft                   (BGUI_TB+19)    /* ISG-- */  /* V40 */
#define FRM_TitleRight                  (BGUI_TB+20)    /* ISG-- */  /* V40 */
#define FRM_BackRasterPen               (BGUI_TB+21)    /* ISG-- */  /* V41 */
#define FRM_BackRasterDriPen            (BGUI_TB+22)    /* ISG-- */  /* V41 */
#define FRM_SelectedBackRasterPen       (BGUI_TB+23)    /* ISG-- */  /* V41 */
#define FRM_SelectedBackRasterDriPen    (BGUI_TB+24)    /* ISG-- */  /* V41 */
#define FRM_Template                    (BGUI_TB+25)    /* IS--- */  /* V41 */
#define FRM_TitleID                     (BGUI_TB+26)    /* ISG-- */  /* V41 */
#define FRM_FillPattern                 (BGUI_TB+27)    /* ISG-- */  /* V41 */
#define FRM_SelectedFillPattern         (BGUI_TB+28)    /* ISG-- */  /* V41 */
#define FRM_OuterOffsetLeft             (BGUI_TB+31)    /* ISG-- */  /* V41 */
#define FRM_OuterOffsetRight            (BGUI_TB+32)    /* ISG-- */  /* V41 */
#define FRM_OuterOffsetTop              (BGUI_TB+33)    /* ISG-- */  /* V41 */
#define FRM_OuterOffsetBottom           (BGUI_TB+34)    /* ISG-- */  /* V41 */
#define FRM_InnerOffsetLeft             (BGUI_TB+35)    /* ISG-- */  /* V41 */
#define FRM_InnerOffsetRight            (BGUI_TB+36)    /* ISG-- */  /* V41 */
#define FRM_InnerOffsetTop              (BGUI_TB+37)    /* ISG-- */  /* V41 */
#define FRM_InnerOffsetBottom           (BGUI_TB+38)    /* ISG-- */  /* V41 */
#define FRM_TAGDONE                     (BGUI_TB+80)

/* Back fill types */
#define STANDARD_FILL           0
#define SHINE_RASTER            1
#define SHADOW_RASTER           2
#define SHINE_SHADOW_RASTER     3
#define FILL_RASTER             4
#define SHINE_FILL_RASTER       5
#define SHADOW_FILL_RASTER      6
#define SHINE_BLOCK             7
#define SHADOW_BLOCK            8
#define FILL_BLOCK              9

/* Flags */
#define FRF_EDGES_ONLY          (1<<0)
#define FRF_RECESSED            (1<<1)
#define FRF_CENTER_TITLE        (1<<2)
#define FRF_HIGHLIGHT_TITLE     (1<<3)
#define FRF_THIN_FRAME          (1<<4)
#define FRF_TITLE_LEFT          (1<<5)  /* V40 */
#define FRF_TITLE_RIGHT         (1<<6)  /* V40 */

#define FRB_EDGES_ONLY          0
#define FRB_RECESSED            1
#define FRB_CENTER_TITLE        2
#define FRB_HIGHLIGHT_TITLE     3
#define FRB_THIN_FRAME          4
#define FRB_TITLE_LEFT          5       /* V40 */
#define FRB_TITLE_RIGHT         6       /* V40 */

/* Frame types */
#define FRTYPE_CUSTOM           0
#define FRTYPE_BUTTON           1
#define FRTYPE_RIDGE            2
#define FRTYPE_DROPBOX          3
#define FRTYPE_NEXT             4
#define FRTYPE_RADIOBUTTON      5
#define FRTYPE_XEN_BUTTON       6
#define FRTYPE_TAB_ABOVE        7       /* V40 */
#define FRTYPE_TAB_BELOW        8       /* V40 */
#define FRTYPE_BORDER           9       /* V40 */
#define FRTYPE_NONE             10      /* V40 */
#define FRTYPE_FUZZ_BUTTON      11      /* V41.1 */
#define FRTYPE_FUZZ_RIDGE       12      /* V41.2 */
#define FRTYPE_TAB_TOP          13      /* V41.8 */
#define FRTYPE_TAB_BOTTOM       14
#define FRTYPE_TAB_LEFT         15
#define FRTYPE_TAB_RIGHT        16

#define FRTYPE_DEFAULT          (~0)    /* V41.8 */

#define FRAMEM_BACKFILL                 (BGUI_MB+21)

/* Backfill a specific rectangle with the backfill hook. */
struct fmBackfill {
        ULONG             MethodID;             /* FRM_RENDER                     */
        struct BaseInfo  *fmb_BInfo;            /* BaseInfo ready for rendering   */
        struct Rectangle *fmb_Bounds;           /* Rendering bounds.              */
        ULONG             fmb_State;            /* See "intuition/imageclass.h"   */
};

/*
 *      FRM_RENDER:
 *
 *      The message packet sent to both the FRM_CustomHook
 *      and FRM_BackFillHook routines. Note that this
 *      structure is READ-ONLY!
 *
 *      The hook is called as follows:
 *
 *              rc = hookFunc( REG(A0) struct Hook         *hook,
 *                             REG(A2) Object              *image_object,
 *                             REG(A1) struct FrameDrawMsg *fdraw );
 */
#define FRM_RENDER              (1L) /* Render yourself           */

struct FrameDrawMsg
{
        ULONG             fdm_MethodID;         /* FRM_RENDER                     */
        struct RastPort  *fdm_RPort;            /* RastPort ready for rendering   */
        struct DrawInfo  *fdm_DrawInfo;         /* All you need to render         */
        struct Rectangle *fdm_Bounds;           /* Rendering bounds.              */
        UWORD             fdm_State;            /* See "intuition/imageclass.h"   */
        /*
         * The following fields are only defined under V41.
         */
        UBYTE             fdm_Horizontal;       /* Horizontal thickness           */
        UBYTE             fdm_Vertical;         /* Vertical thickness             */
};

/*
 *      FRM_THICKNESS:
 *
 *      The message packet sent to the FRM_Custom hook.
 *
 *      The hook is called as follows:
 *
 *      rc = hookFunc( REG(A0) struct Hook              *hook,
 *                     REG(A2) Object                   *image_object,
 *                     REG(A1) struct ThicknessMsg      *thick );
 */
#define FRM_THICKNESS           (2L) /* Give the default frame thickness. */

struct ThicknessMsg {
        ULONG            tm_MethodID;           /* FRM_THICKNESS                  */
        struct {
                UBYTE   *Horizontal;            /* Storage for horizontal         */
                UBYTE   *Vertical;              /* Storage for vertical   */
        }                tm_Thickness;
        BOOL             tm_Thin;               /* Added in V38!                  */
};

/* Possible hook return codes. */
#define FRC_OK                  0 /* OK       */
#define FRC_UNKNOWN             1 /* Unknown method */


/*****************************************************************************
 *
 *      "labelclass" - BOOPSI labeling image.
 *
 *      Tags: 81 - 160          Methods: 1 - 20
 */

#define LAB_TAGSTART                    (BGUI_TB+81)
#define LAB_TextAttr                    (BGUI_TB+81)    /* ISG-- */
#define LAB_Style                       (BGUI_TB+82)    /* ISG-- */
#define LAB_Underscore                  (BGUI_TB+83)    /* ISG-- */
#define LAB_Place                       (BGUI_TB+84)    /* ISG-- */
#define LAB_Label                       (BGUI_TB+85)    /* ISG-- */
#define LAB_Flags                       (BGUI_TB+86)    /* ISG-- */
#define LAB_Highlight                   (BGUI_TB+87)    /* ISG-- */
#define LAB_HighUScore                  (BGUI_TB+88)    /* ISG-- */
#define LAB_Pen                         (BGUI_TB+89)    /* ISG-- */
#define LAB_SelectedPen                 (BGUI_TB+90)    /* ISG-- */
#define LAB_DriPen                      (BGUI_TB+91)    /* ISG-- */
#define LAB_SelectedDriPen              (BGUI_TB+92)    /* ISG-- */
#define LAB_LabelID                     (BGUI_TB+93)    /* ISG-- */     /* V41 */
#define LAB_Template                    (BGUI_TB+94)    /* IS--- */     /* V41 */
#define LAB_NoPlaceIn                   (BGUI_TB+95)    /* ISG-- */     /* V41.7 */
#define LAB_SelectedStyle               (BGUI_TB+96)    /* ISG-- */     /* V41.7 */
#define LAB_FlipX                       (BGUI_TB+97)    /* ISG-- */     /* V41.7 */
#define LAB_FlipY                       (BGUI_TB+98)    /* ISG-- */     /* V41.7 */
#define LAB_FlipXY                      (BGUI_TB+99)    /* ISG-- */     /* V41.7 */
#define LAB_TAGDONE                     (BGUI_TB+160)

/* Flags */
#define LABF_HIGHLIGHT          (1<<0)  /* Highlight label        */
#define LABF_HIGH_USCORE        (1<<1)  /* Highlight underscoring */
#define LABF_FLIP_X             (1<<2)  /* Flip across x axis     */
#define LABF_FLIP_Y             (1<<3)  /* Flip across y axis     */
#define LABF_FLIP_XY            (1<<4)  /* Flip across x = y      */

#define LABB_HIGHLIGHT          0       /* Highlight label        */
#define LABB_HIGH_USCORE        1       /* Highlight underscoring */
#define LABB_FLIP_X             2       /* Flip across x axis     */
#define LABB_FLIP_Y             3       /* Flip across y axis     */
#define LABB_FLIP_XY            4       /* Flip across x = y      */

/* Label placement */
#define PLACE_IN                0
#define PLACE_LEFT              1
#define PLACE_RIGHT             2
#define PLACE_ABOVE             3
#define PLACE_BELOW             4

/* New methods */
/*
 *      The IM_EXTENT method is used to find out how many
 *      pixels the label extents the relative hitbox in
 *      either direction. Normally this method is called
 *      by the baseclass.
 */
#define IM_EXTENT                       (BGUI_MB+1)

struct impExtent {
        ULONG                   MethodID;       /* IM_EXTENT                */
        struct RastPort        *impe_RPort;     /* RastPort                 */
        struct IBox            *impe_Extent;    /* Storage for extentions.  */
        struct {
                UWORD          *Width;          /* Storage width in pixels  */
                UWORD          *Height;         /* Storage height in pixels */
        }                       impe_LabelSize;
        UWORD                   impe_Flags;     /* See below.               */
};

#define EXTF_MAXIMUM            (1<<0)          /* Request maximum extensions. */


/*****************************************************************************
 *
 *      "vectorclass" - BOOPSI scalable vector image.
 *
 *      Tags: 161 - 240
 *
 *      Based on an idea found in the ObjectiveGadTools.library
 *      by Davide Massarenti.
 */
#define VIT_TAGSTART                    (BGUI_TB+161)
#define VIT_VectorArray                 (BGUI_TB+161)   /* ISG-- */
#define VIT_BuiltIn                     (BGUI_TB+162)   /* ISG-- */
#define VIT_Pen                         (BGUI_TB+163)   /* ISG-- */
#define VIT_DriPen                      (BGUI_TB+164)   /* ISG-- */
#define VIT_MinWidth                    (BGUI_TB+165)   /* --G-- */  /* V41.8 */
#define VIT_MinHeight                   (BGUI_TB+166)   /* --G-- */
#define VIT_TAGDONE                     (BGUI_TB+200)

/*
 *      Command structure which can contain
 *      coordinates, data and command flags.
 */
struct VectorItem {
        WORD                    vi_x;           /* X coordinate or data */
        WORD                    vi_y;           /* Y coordinate         */
        ULONG                   vi_Flags;       /* See below            */
};

/* Flags */
#define VIF_MOVE                (1<<0)  /* Move to vc_x, vc_y                */
#define VIF_DRAW                (1<<1)  /* Draw to vc_x, vc_y                */
#define VIF_AREASTART           (1<<2)  /* Start AreaFill at vc_x, vc_y      */
#define VIF_AREAEND             (1<<3)  /* End AreaFill at vc_x, vc_y        */
#define VIF_XRELRIGHT           (1<<4)  /* vc_x relative to right edge       */
#define VIF_YRELBOTTOM          (1<<5)  /* vc_y relative to bottom edge      */
#define VIF_SHADOWPEN           (1<<6)  /* switch to SHADOWPEN, Move/Draw    */
#define VIF_SHINEPEN            (1<<7)  /* switch to SHINEPEN, Move/Draw     */
#define VIF_FILLPEN             (1<<8)  /* switch to FILLPEN, Move/Draw      */
#define VIF_TEXTPEN             (1<<9)  /* switch to TEXTPEN, Move/Draw      */
#define VIF_COLOR               (1<<10) /* switch to color(s) in vc_x (+y)   */
#define VIF_LASTITEM            (1<<11) /* last element of the element list  */
#define VIF_SCALE               (1<<12) /* X & Y are design width & height   */
#define VIF_DRIPEN              (1<<13) /* switch to dripen(s) in vc_x (+y)  */
#define VIF_AOLPEN              (1<<14) /* set area outline pen vc_x         */
#define VIF_AOLDRIPEN           (1<<15) /* set area outline dripen vc_x      */
#define VIF_ENDOPEN             (1<<16) /* end area outline pen              */
#define VIF_MINSIZE             (1<<17) /* X & Y are minimum size            */  /* V41.8 */
#define VIF_LINEPATTERN         (1<<18) /* Use line pattern in vc_x          */
#define VIF_BPEN                (1<<19) /* Interpret vc_y as bpen            */
#define VIF_DRAWMODE            (1<<20) /* Draw mode                         */

/* Built-in images. */
#define BUILTIN_GETPATH         1
#define BUILTIN_GETFILE         2
#define BUILTIN_CHECKMARK       3
#define BUILTIN_POPUP           4
#define BUILTIN_ARROW_UP        5
#define BUILTIN_ARROW_DOWN      6
#define BUILTIN_ARROW_LEFT      7
#define BUILTIN_ARROW_RIGHT     8
#define BUILTIN_CYCLE           9       /* V41 */
#define BUILTIN_CYCLE2          10      /* V41 */
#define BUILTIN_RADIOBUTTON     11      /* V41 */
#define BUILTIN_GETFONT         12      /* V41.9 */
#define BUILTIN_GETSCREEN       13      /* V41.9 */

/* Design width and heights of the built-in images. */
#define GETPATH_WIDTH           20
#define GETPATH_HEIGHT          14
#define GETFILE_WIDTH           20
#define GETFILE_HEIGHT          14
#define CHECKMARK_WIDTH         24
#define CHECKMARK_HEIGHT        11
#define POPUP_WIDTH             14
#define POPUP_HEIGHT            12
#define ARROW_UP_WIDTH          16
#define ARROW_UP_HEIGHT         9
#define ARROW_DOWN_WIDTH        16
#define ARROW_DOWN_HEIGHT       9
#define ARROW_LEFT_WIDTH        10
#define ARROW_LEFT_HEIGHT       12
#define ARROW_RIGHT_WIDTH       10
#define ARROW_RIGHT_HEIGHT      12

#endif /* BGUI_IMAGE_H */
