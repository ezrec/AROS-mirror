#ifndef _BWIN_PRIVATE_MCC_H
#define _BWIN_PRIVATE_MCC_H

/*
**  $VER: BWin_private_mcc.h 20.5 (7.5.2003)
**  Includes Release 20.5
**
**  BWin.mcc
**  Borderless windows class
**
**  (C) Copyright 2002-2003 Alfonso [alfie] Ranieri <alforan@tin.it>
**      Destributed under the terms of the GNU Public Licence 2
*/

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#ifndef __AROS__
#define STACKED
#endif

/***********************************************************************/

#define MUIC_BWin  "BWin.mcc"
#define BWinObject MUI_NewObject(MUIC_BWin

/***********************************************************************/
/*
** Attributes - Scheme is [ISGN]
*/

/* Base */
#define MUIA_BWin_Base        0xFEC9012C

/* Private */
#define MUIA_BWin_Type        0xFEC9012D  /* [I...] ULONG */
#define MUIA_BWin_Flags       0xFEC9012E  /* [I...] ULONG */
#define MUIA_BWin_Show        0xFEC9012F  /* [I...] ULONG */
#define MUIA_BWin_Horiz       0xFEC90130  /* [IS..] BOOL  */
#define MUIA_BWin_Activate    0xFEC90131  /* [IS..] BOOL  */

/* Public */
#define MUIA_BWin_NoDragBar   0xFEC90135  /* [I.G.] BOOL  */
#define MUIA_BWin_NoSize      0xFEC90136  /* [I.G.] BOOL  */
#define MUIA_BWin_NoClose     0xFEC90137  /* [I.G.] BOOL  */
#define MUIA_BWin_ShowDragBar 0xFEC90138  /* [ISG.] BOOL  */
#define MUIA_BWin_ShowSize    0xFEC90139  /* [ISG.] BOOL  */
#define MUIA_BWin_ShowClose   0xFEC9013A  /* [ISG.] BOOL  */
#define MUIA_BWin_AutoShow    0xFEC9013B  /* [ISG.] BOOL  */
#define MUIA_BWin_Save        0xFEC9013C  /* [ISG.] ULONG */
#define MUIA_BWin_NoCloseItem 0xFEC9013D  /* [ISG.] BOOL  */
#define MUIA_BWin_NoHideItem  0xFEC9013E  /* [ISG.] BOOL  */
#define MUIA_BWin_DragBarTop  0xFEC9013F  /* [ISG.] BOOL  */
#define MUIA_BWin_Background  0xFEC90140  /* [ISGN] ULONG */
#define MUIA_BWin_Borders     0xFEC90141  /* [I.G.] ULONG */
#define MUIA_BWin_UserItemID  0xFEC90142  /* [..GN] ULONG */

#define MUIA_BWin_InnerLeft   0xFEC90143  /* [ISGN] ULONG */
#define MUIA_BWin_InnerTop    0xFEC90144  /* [ISGN] ULONG */
#define MUIA_BWin_InnerRight  0xFEC90145  /* [ISGN] ULONG */
#define MUIA_BWin_InnerBottom 0xFEC90146  /* [ISGN] ULONG */

/***********************************************************************/
/*
** Special values
*/

/* MUIA_BWin_Flags values - Private - */
enum
{
    FLG_NoDragBar   = 1<<0,
    FLG_NoSize      = 1<<1,
    FLG_NoClose     = 1<<2,
    FLG_ShowDragBar = 1<<3,
    FLG_ShowSize    = 1<<4,
    FLG_ShowClose   = 1<<5,
    FLG_AutoShow    = 1<<6,
    FLG_NoCloseItem = 1<<7,
    FLG_NoHideItem  = 1<<8,
    FLG_DragBarTop  = 1<<9,
    FLG_Activate    = 1<<10,
    FLG_Borders     = 1<<11,
};

enum
{
    FLG_AvailableBit = 16,
};

/* Shapes values - Private - */
enum
{
    MUIV_BWin_Shape_None,
    MUIV_BWin_Shape_Original,
    MUIV_BWin_Shape_Line,
    MUIV_BWin_Shape_Solid,
};

/* MUIA_BWin_Save values */
enum
{
    MUIV_BWin_Save_DragBarTop  = 1<<0,
    MUIV_BWin_Save_AutoShow    = 1<<1,
    MUIV_BWin_Save_ShowDragBar = 1<<2,
    MUIV_BWin_Save_ShowSize    = 1<<3,
    MUIV_BWin_Save_ShowClose   = 1<<4,

    MUIV_BWin_Save_All         = MUIV_BWin_Save_DragBarTop|MUIV_BWin_Save_AutoShow|MUIV_BWin_Save_ShowDragBar|MUIV_BWin_Save_ShowSize|MUIV_BWin_Save_ShowClose,
};

/***********************************************************************/
/*
** Methods
*/

#define MUIM_BWin_AddUserItem    0xFEC9012D /* struct MUIP_BWin_AddUserItem    */
#define MUIM_BWin_RemoveUserItem 0xFEC9012E /* struct MUIP_BWin_RemoveUserItem */

/***********************************************************************/
/*
** Methods structures
*/

struct MUIP_BWin_AddUserItem
{
    STACKED ULONG  MethodID;
    STACKED STRPTR title;
};

#define MUIV_BWin_AddUserItem_Bar ((STRPTR)-1)

struct MUIP_BWin_RemoveUserItem
{
    STACKED ULONG  MethodID;
    STACKED ULONG  id;
};

/***********************************************************************/
/*
** BWin.mcp
*/

/* Base */
#define MUICFG_BWin_Base                0xFEC90150

/* Attributes */
#define MUICFG_BWin_Spacing             0xFEC90151
#define MUICFG_BWin_WinShinePen         0xFEC90152
#define MUICFG_BWin_WinShadowPen        0xFEC90153
#define MUICFG_BWin_WinForegroundPen    0xFEC90154
#define MUICFG_BWin_DragBarShinePen     0xFEC90155
#define MUICFG_BWin_DragBarShadowPen    0xFEC90156
#define MUICFG_BWin_DragBarBack         0xFEC90157
#define MUICFG_BWin_NADragBarShinePen   0xFEC90158
#define MUICFG_BWin_NADragBarShadowPen  0xFEC90159
#define MUICFG_BWin_NADragBarBack       0xFEC9015A
#define MUICFG_BWin_SizeShinePen        0xFEC9015B
#define MUICFG_BWin_SizeShadowPen       0xFEC9015C
#define MUICFG_BWin_SizeForegroundPen   0xFEC9015D
#define MUICFG_BWin_NASizeShinePen      0xFEC9015E
#define MUICFG_BWin_NASizeShadowPen     0xFEC9015F
#define MUICFG_BWin_NASizeForegroundPen 0xFEC90160
#define MUICFG_BWin_CloseShinePen       0xFEC90161
#define MUICFG_BWin_CloseShadowPen      0xFEC90162
#define MUICFG_BWin_CloseBack           0xFEC90163
#define MUICFG_BWin_NACloseShinePen     0xFEC90164
#define MUICFG_BWin_NACloseShadowPen    0xFEC90165
#define MUICFG_BWin_NACloseBack         0xFEC90166
#define MUICFG_BWin_Types               0xFEC90167

/***********************************************************************/
/*
** Defaults
*/

/* Sparse */
#define MUIDEF_BWin_Type                GTYP_WDRAGGING
#define MUIDEF_BWin_Flags               (FLG_ShowDragBar|FLG_ShowSize|FLG_Activate)
#define MUIDEF_BWin_Horiz               FALSE
#define MUIDEF_BWin_NoDragBar           FALSE
#define MUIDEF_BWin_NoSize              FALSE
#define MUIDEF_BWin_NoClose             FALSE
#define MUIDEF_BWin_ShowDragBar         TRUE
#define MUIDEF_BWin_ShowSize            TRUE
#define MUIDEF_BWin_ShowClose           TRUE
#define MUIDEF_BWin_AutoShow            FALSE
#define MUIDEF_BWin_Save                0
#define MUIDEF_BWin_NoCloseItem         FALSE
#define MUIDEF_BWin_NoHideItem          FALSE
#define MUIDEF_BWin_DragBarTop          FALSE
#define MUIDEF_BWin_Background          MUII_WindowBack
#define MUIDEF_BWin_Activate            TRUE

/* Spacing */
#define MUIDEF_BWin_LeftSpacing         ((ULONG)4)
#define MUIDEF_BWin_TopSpacing          ((ULONG)4)
#define MUIDEF_BWin_RightSpacing        ((ULONG)4)
#define MUIDEF_BWin_BottomSpacing       ((ULONG)4)
#define MUIDEF_BWin_Spacing             MAKE_ID(MUIDEF_BWin_LeftSpacing,MUIDEF_BWin_TopSpacing,MUIDEF_BWin_RightSpacing,MUIDEF_BWin_BottomSpacing)

/* Pens and backgrounds */
#define MUIDEF_BWin_WinShinePen         ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_BWin_WinShadowPen        ((APTR)"r00000000,00000000,00000000")
#define MUIDEF_BWin_WinForegroundPen    ((APTR)"r9C9C9C9C,9C9C9C9C,9C9C9C9C")

#define MUIDEF_BWin_DragBarShinePen     ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_BWin_DragBarShadowPen    ((APTR)"r00000000,00000000,00000000")
#define MUIDEF_BWin_DragBarBack         ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define MUIDEF_BWin_NADragBarShinePen   ((APTR)"rD0D0D0D0,D0D0D0D0,D0D0D0D0")
#define MUIDEF_BWin_NADragBarShadowPen  ((APTR)"r78787878,78787878,78787878")
#define MUIDEF_BWin_NADragBarBack       ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")

#define MUIDEF_BWin_SizeShinePen        ((APTR)"r00000000,00000000,00000000")
#define MUIDEF_BWin_SizeShadowPen       ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_BWin_SizeForegroundPen   ((APTR)"r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define MUIDEF_BWin_NASizeShinePen      ((APTR)"r78787878,78787878,78787878")
#define MUIDEF_BWin_NASizeShadowPen     ((APTR)"rD0D0D0D0,D0D0D0D0,D0D0D0D0")
#define MUIDEF_BWin_NASizeForegroundPen ((APTR)"r9C9C9C9C,9C9C9C9C,9C9C9C9C")

#define MUIDEF_BWin_CloseShinePen       ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define MUIDEF_BWin_CloseShadowPen      ((APTR)"r00000000,00000000,00000000")
#define MUIDEF_BWin_CloseBack           ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define MUIDEF_BWin_NACloseShinePen     ((APTR)"rD0D0D0D0,D0D0D0D0,D0D0D0D0")
#define MUIDEF_BWin_NACloseShadowPen    ((APTR)"r78787878,78787878,78787878")
#define MUIDEF_BWin_NACloseBack         ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")

/* Use bits (0xFF000000 of Types) */
#define MUIV_BWin_UseDragBarBack        (1<<0)
#define MUIV_BWin_UseNADragBarBack      (1<<1)
#define MUIV_BWin_UseSizeForeground     (1<<2)
#define MUIV_BWin_UseNASizeForeground   (1<<3)
#define MUIV_BWin_UseCloseBack          (1<<4)
#define MUIV_BWin_UseNACloseBack        (1<<5)
#define MUIV_BWin_UseOver               (1<<6)

/* Types */
#define MUIDEF_BWin_UseDragBarBack      1
#define MUIDEF_BWin_UseNADragBarBack    1
#define MUIDEF_BWin_UseSizeForeground   1
#define MUIDEF_BWin_UseNASizeForeground 1
#define MUIDEF_BWin_UseCloseBack        1
#define MUIDEF_BWin_UseNACloseBack      1
#define MUIDEF_BWin_Use                 ((MUIDEF_BWin_UseDragBarBack ? MUIV_BWin_UseDragBarBack : 0) |\
                                         (MUIDEF_BWin_UseNADragBarBack ? MUIV_BWin_UseNADragBarBack : 0) |\
                                         (MUIDEF_BWin_UseSizeForeground ? MUIV_BWin_UseSizeForeground : 0) |\
                                         (MUIDEF_BWin_UseNASizeForeground ? MUIV_BWin_UseNASizeForeground : 0) |\
                                         (MUIDEF_BWin_UseNACloseBack ? MUIV_BWin_UseNACloseBack  : 0) |\
                                         (MUIDEF_BWin_UseCloseBack ? MUIV_BWin_UseCloseBack  : 0))

#define MUIDEF_BWin_DragBarType         MUIV_BWin_Shape_Original
#define MUIDEF_BWin_NADragBarType       MUIV_BWin_Shape_Original
#define MUIDEF_BWin_SizeType            MUIV_BWin_Shape_Original
#define MUIDEF_BWin_NASizeType          MUIV_BWin_Shape_Original
#define MUIDEF_BWin_CloseType           MUIV_BWin_Shape_Original
#define MUIDEF_BWin_NACloseType         MUIV_BWin_Shape_Original

#define MUIDEF_BWin_Types               ((MUIDEF_BWin_Use<<24) |\
                                         (MUIDEF_BWin_DragBarType) |\
                                         (MUIDEF_BWin_NADragBarType<<4) |\
                                         (MUIDEF_BWin_SizeType<<8) |\
                                         (MUIDEF_BWin_NASizeType<<12) |\
                                         (MUIDEF_BWin_CloseType<<16) |\
                                         (MUIDEF_BWin_NACloseType<<20))

/***********************************************************************/
/*
** Macros
*/

/* Spacing */
#define GETLEFTSP(v)                    (((v) & 0xFF000000)>>24)
#define GETTOPSP(v)                     (((v) & 0x00FF0000)>>16)
#define GETRIGHTSP(v)                   (((v) & 0x0000FF00)>>8)
#define GETBOTTOMSP(v)                  ((v) & 0x000000FF)

/* Use */
#define GETUSE(v)                       (((v) & 0xFF000000)>>24)
#define GETUSEDRAGBARBACK(v)            (GETUSE(v) & MUIV_BWin_UseDragBarBack)
#define GETUSENADRAGBARBACK(v)          (GETUSE(v) & MUIV_BWin_UseNADragBarBack)
#define GETUSESIZEFOREGROUND(v)         (GETUSE(v) & MUIV_BWin_UseSizeForeground)
#define GETUSENASIZEFOREGROUND(v)       (GETUSE(v) & MUIV_BWin_UseNASizeForeground)
#define GETUSECLOSEBACK(v)              (GETUSE(v) & MUIV_BWin_UseCloseBack)
#define GETUSENACLOSEBACK(v)            (GETUSE(v) & MUIV_BWin_UseNACloseBack)
#define GETUSEOVER(v)                   (GETUSE(v) & MUIV_BWin_UseOver)

/* Shapes type */
#define GETDRAGBARTYPE(v)               (((v) & 0xF))
#define GETNADRAGBARTYPE(v)             (((v) & 0xF0)>>4)
#define GETSIZETYPE(v)                  (((v) & 0xF00)>>8)
#define GETNASIZETYPE(v)                (((v) & 0xF000)>>12)
#define GETCLOSETYPE(v)                 (((v) & 0xF0000)>>16)
#define GETNACLOSETYPE(v)               (((v) & 0xF00000)>>20)

/***********************************************************************/
/*
** Query() values
*/

enum
{
    MUIV_Query_MCC,
    MUIV_Query_MCP,
    MUIV_Query_MCPImage,
    MUIV_Query_OnlyGlobal,
    MUIV_Query_InfoClass,
};

/***********************************************************************/

#endif /* _BWIN_PRIVATE_MCC_H */
