/*************************************************************************\
 *                                                                       *
 * pageset.h                                                             *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef PAGESET_H
#define PAGESET_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif


#define MYATTR_PageSet_Columns          (MYTAG_START+650) /* ISG */
#define MYATTR_PageSet_Rows             (MYTAG_START+651) /* ISG */
#define MYATTR_PageSet_Document         (MYTAG_START+652) /* IS. */
#define MYATTR_PageSet_Page             (MYTAG_START+653) /* ISG */
#define MYATTR_PageSet_CommInfo         (MYTAG_START+654) /* I.. */
#define MYATTR_PageSet_Zoom             (MYTAG_START+655) /* ISG */
#define MYATTR_PageSet_DocChanged       (MYTAG_START+656) /* ..G */
#define MYATTR_PageSet_Message          (MYTAG_START+657) /* ..G */
#define MYATTR_PageSet_Rotate           (MYTAG_START+658) /* ISG */
#define MYATTR_PageSet_CropBox          (MYTAG_START+659) /* ISG */
#define MYATTR_PageSet_TextAA           (MYTAG_START+660) /* ISG */
#define MYATTR_PageSet_StrokeAA         (MYTAG_START+661) /* ISG */
#define MYATTR_PageSet_FillAA           (MYTAG_START+662) /* ISG */
#define MYATTR_PageSet_Run              (MYTAG_START+663) /* ..G */
#define MYATTR_PageSet_URL              (MYTAG_START+664) /* ..G */
#define MYATTR_PageSet_Open             (MYTAG_START+665) /* ..G */
#define MYATTR_PageSet_OpenNew          (MYTAG_START+666) /* ..G */
#define MYATTR_PageSet_PageNew          (MYTAG_START+667) /* ..G */
#define MYATTR_PageSet_DPI              (MYTAG_START+668) /* IS. */
#define MYATTR_PageSet_Quiet            (MYTAG_START+669) /* IS. */
#define MYATTR_PageSet_Selection        (MYTAG_START+670) /* ISG */
#define MYATTR_PageSet_VGroup           (MYTAG_START+671) /* IS. */
#define MYATTR_PageSet_Ready            (MYTAG_START+672) /* ..G */
#define MYATTR_PageSet_NumPages         (MYTAG_START+673) /* ..G */
#define MYATTR_PageSet_ActivePage       (MYTAG_START+674) /* ISG */
#define MYATTR_PageSet_VisWidth         (MYTAG_START+675) /* IS. */
#define MYATTR_PageSet_VisHeight        (MYTAG_START+676) /* IS. */
#define MYATTR_PageSet_XOffset          (MYTAG_START+677) /* IS. */
#define MYATTR_PageSet_YOffset          (MYTAG_START+678) /* IS. */
#define MYATTR_PageSet_StripMode        (MYTAG_START+679) /* IS. */
#define MYATTR_PageSet_Back             (MYTAG_START+680) /* ..G */
#define MYATTR_PageSet_DrawMode         (MYTAG_START+681) /* IS. */
#define MYATTR_PageSet_FullScreen       (MYTAG_START+682) /* IS. */

#define MYM_PageSet_ToClip              (MYTAG_START+1651)
#define MYM_PageSet_Search              (MYTAG_START+1652)
#define MYM_PageSet_RLeft               (MYTAG_START+1653)
#define MYM_PageSet_RRight              (MYTAG_START+1654)
#define MYM_PageSet_Crop                (MYTAG_START+1655)
#define MYM_PageSet_UnCrop              (MYTAG_START+1657)
#define MYM_PageSet_Refresh             (MYTAG_START+1658)
#define MYM_PageSet_Action              (MYTAG_START+1659)
/*#define MYM_PageSet_Resize              (MYTAG_START+1660)*/
#define MYM_PageSet_AddRow              (MYTAG_START+1661)
#define MYM_PageSet_RemRow              (MYTAG_START+1662)
#define MYM_PageSet_AddColumn           (MYTAG_START+1663)
#define MYM_PageSet_RemColumn           (MYTAG_START+1664)
#define MYM_PageSet_Select              (MYTAG_START+1665) /* private */
#define MYM_PageSet_VStrip              (MYTAG_START+1666)

struct MYS_PageSet_CropBox {
    int left;
    int top;
    int right;
    int bottom;
};


extern struct MUI_CustomClass *pageset_mcc;

#define PageSetObject NewObject(pageset_mcc->mcc_Class,NULL

BOOL createPageSetClass(void);
BOOL deletePageSetClass(void);

#endif

