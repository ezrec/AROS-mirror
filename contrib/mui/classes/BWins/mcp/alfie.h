/***********************************************************************/
/*
** alfie's default
*/

#define ALFIE_BWin_LeftSpacing          ((ULONG)4)
#define ALFIE_BWin_TopSpacing           ((ULONG)4)
#define ALFIE_BWin_RightSpacing         ((ULONG)4)
#define ALFIE_BWin_BottomSpacing        ((ULONG)4)
#define ALFIE_BWin_Spacing              MAKE_ID(ALFIE_BWin_LeftSpacing,ALFIE_BWin_TopSpacing,ALFIE_BWin_RightSpacing,ALFIE_BWin_BottomSpacing)

/* Pens and backgrounds */
#define ALFIE_BWin_WinShinePen          ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define ALFIE_BWin_WinShadowPen         ((APTR)"r00000000,00000000,00000000")
#define ALFIE_BWin_WinForegroundPen     ((APTR)"r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define ALFIE_BWin_DragBarShinePen      ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define ALFIE_BWin_DragBarShadowPen     ((APTR)"r23232323,26262626,72727272")
#define ALFIE_BWin_DragBarBack          ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define ALFIE_BWin_NADragBarShinePen    ((APTR)"rD0D0D0D0,D0D0D0D0,D0D0D0D0")
#define ALFIE_BWin_NADragBarShadowPen   ((APTR)"r78787878,78787878,78787878")
#define ALFIE_BWin_NADragBarBack        ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define ALFIE_BWin_SizeShinePen         ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define ALFIE_BWin_SizeShadowPen        ((APTR)"r23232323,26262626,72727272")
#define ALFIE_BWin_SizeForegroundPen    ((APTR)"r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define ALFIE_BWin_NASizeShinePen       ((APTR)"rD0D0D0D0,D0D0D0D0,D0D0D0D0")
#define ALFIE_BWin_NASizeShadowPen      ((APTR)"r78787878,78787878,78787878")
#define ALFIE_BWin_NASizeForegroundPen  ((APTR)"r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define ALFIE_BWin_CloseShinePen        ((APTR)"rFFFFFFFF,FFFFFFFF,FFFFFFFF")
#define ALFIE_BWin_CloseShadowPen       ((APTR)"r00000000,00000000,00000000")
#define ALFIE_BWin_CloseBack            ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")
#define ALFIE_BWin_NACloseShinePen      ((APTR)"rD0D0D0D0,D0D0D0D0,D0D0D0D0")
#define ALFIE_BWin_NACloseShadowPen     ((APTR)"r78787878,78787878,78787878")
#define ALFIE_BWin_NACloseBack          ((APTR)"2:r9C9C9C9C,9C9C9C9C,9C9C9C9C")

/* Types */
#define ALFIE_BWin_UseDragBarBack       0
#define ALFIE_BWin_UseNADragBarBack     0
#define ALFIE_BWin_UseSizeForeground    0
#define ALFIE_BWin_UseNASizeForeground  0
#define ALFIE_BWin_UseCloseBack         0
#define ALFIE_BWin_UseNACloseBack       0
#define ALFIE_BWin_Use                  ((ALFIE_BWin_UseDragBarBack ? MUIV_BWin_UseDragBarBack : 0) |\
                                         (ALFIE_BWin_UseNADragBarBack ? MUIV_BWin_UseNADragBarBack : 0) |\
                                         (ALFIE_BWin_UseSizeForeground ? MUIV_BWin_UseSizeForeground : 0) |\
                                         (ALFIE_BWin_UseNASizeForeground ? MUIV_BWin_UseNASizeForeground : 0) |\
                                         (ALFIE_BWin_UseCloseBack ? MUIV_BWin_UseCloseBack  : 0) |\
                                         (ALFIE_BWin_UseNACloseBack ? MUIV_BWin_UseNACloseBack  : 0))

#define ALFIE_BWin_DragBarType          MUIV_BWin_Shape_Line
#define ALFIE_BWin_NADragBarType        MUIV_BWin_Shape_Line
#define ALFIE_BWin_SizeType             MUIV_BWin_Shape_Line
#define ALFIE_BWin_NASizeType           MUIV_BWin_Shape_Line
#define ALFIE_BWin_CloseType            MUIV_BWin_Shape_Line
#define ALFIE_BWin_NACloseType          MUIV_BWin_Shape_Line
#define ALFIE_BWin_Types                ((ALFIE_BWin_UseBacks<<24) |\
                                         (ALFIE_BWin_DragBarType) |\
                                         (ALFIE_BWin_NADragBarType<<4) |\
                                         (ALFIE_BWin_SizeType<<8) |\
                                         (ALFIE_BWin_NASizeType<<12) |\
                                         (ALFIE_BWin_CloseType<<16) |\
                                         (ALFIE_BWin_NACloseType<<20))

/***********************************************************************/
