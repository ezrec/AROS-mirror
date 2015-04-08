/*************************************************************************\
 *                                                                       *
 * docbitmap.h                                                           *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef DOCBITMAP_H
#define DOCBITMAP_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_DocBitmap_Page        (MYTAG_START+501) /* ISG */
#define MYATTR_DocBitmap_Zoom        (MYTAG_START+502) /* ISG */
#define MYATTR_DocBitmap_Selected    (MYTAG_START+503) /* ISG */
#define MYATTR_DocBitmap_Selection   (MYTAG_START+504) /* ISG */
#define MYATTR_DocBitmap_Document    (MYTAG_START+505) /* ISG */
#define MYATTR_DocBitmap_DocChanged  (MYTAG_START+506) /* ..G */
#define MYATTR_DocBitmap_FullScreen  (MYTAG_START+507) /* IS. */
#define MYATTR_DocBitmap_Message     (MYTAG_START+508) /* ..G */
#define MYATTR_DocBitmap_Rotate      (MYTAG_START+509) /* ISG */
#define MYATTR_DocBitmap_CropBox     (MYTAG_START+510) /* ISG */
#define MYATTR_DocBitmap_CommInfo    (MYTAG_START+511) /* I.. */
#define MYATTR_DocBitmap_TextAA      (MYTAG_START+512) /* ISG */
#define MYATTR_DocBitmap_StrokeAA    (MYTAG_START+513) /* ISG */
#define MYATTR_DocBitmap_FillAA      (MYTAG_START+514) /* ISG */
#define MYATTR_DocBitmap_Back        (MYTAG_START+515) /* ..G */
#define MYATTR_DocBitmap_Run         (MYTAG_START+516) /* ..G */
#define MYATTR_DocBitmap_URL         (MYTAG_START+517) /* ..G */
#define MYATTR_DocBitmap_Open        (MYTAG_START+518) /* ..G */
#define MYATTR_DocBitmap_OpenNew     (MYTAG_START+519) /* ..G */
#define MYATTR_DocBitmap_PageNew     (MYTAG_START+520) /* ..G */
#define MYATTR_DocBitmap_DPI         (MYTAG_START+521) /* ISG */
#define MYATTR_DocBitmap_BoxGrabber  (MYTAG_START+522) /* ..G */
#define MYATTR_DocBitmap_DrawMode    (MYTAG_START+523) /* IS. */
#define MYATTR_DocBitmap_DrawBg      (MYTAG_START+524) /* IS. */
#define MYATTR_DocBitmap_Quiet       (MYTAG_START+525) /* IS. */
#define MYATTR_DocBitmap_LensMode    (MYTAG_START+526) /* I.. */
#define MYATTR_DocBitmap_VisLeft     (MYTAG_START+527) /* ISG */
#define MYATTR_DocBitmap_VisTop      (MYTAG_START+528) /* ISG */
#define MYATTR_DocBitmap_VisWidth    (MYTAG_START+529) /* IS. */
#define MYATTR_DocBitmap_VisHeight   (MYTAG_START+530) /* IS. */
#define MYATTR_DocBitmap_Ready       (MYTAG_START+531) /* ..G */
/*#define MYATTR_DocBitmap_Up          (MYTAG_START+532) /* ..G */
/*#define MYATTR_DocBitmap_Down        (MYTAG_START+533) /* ..G */
/*#define MYATTR_DocBitmap_Left        (MYTAG_START+534) /* ..G */
/*#define MYATTR_DocBitmap_Right       (MYTAG_START+535) /* ..G */
#define MYATTR_DocBitmap_Width       (MYTAG_START+536) /* ..G */
#define MYATTR_DocBitmap_Height      (MYTAG_START+537) /* ..G */
#define MYATTR_DocBitmap_Active      (MYTAG_START+538) /* ISG */
#define MYATTR_DocBitmap_XOffset     (MYTAG_START+539) /* IS. */
#define MYATTR_DocBitmap_YOffset     (MYTAG_START+540) /* IS. */
#define MYATTR_DocBitmap_ShowActive  (MYTAG_START+541) /* IS. */
#define MYATTR_DocBitmap_XSpacing    (MYTAG_START+542) /* IS. */
#define MYATTR_DocBitmap_YSpacing    (MYTAG_START+543) /* IS. */

#define MYM_DocBitmap_UpdateAnnot       (MYTAG_START+1501)
#define MYM_DocBitmap_ToClip            (MYTAG_START+1502)
#define MYM_DocBitmap_Search            (MYTAG_START+1503)
#define MYM_DocBitmap_RLeft             (MYTAG_START+1504)
#define MYM_DocBitmap_RRight            (MYTAG_START+1505)
#define MYM_DocBitmap_Crop              (MYTAG_START+1506)
#define MYM_DocBitmap_UnCrop            (MYTAG_START+1507)
#define MYM_DocBitmap_Refresh           (MYTAG_START+1508)
#define MYM_DocBitmap_Action            (MYTAG_START+1509)
#define MYM_DocBitmap_DestroyLens       (MYTAG_START+1510)
#define MYM_DocBitmap_Redraw            (MYTAG_START+1511)
#define MYM_DocBitmap_UpdatePage        (MYTAG_START+1512)
#define MYM_DocBitmap_UpdateTextField   (MYTAG_START+1513)
#define MYM_DocBitmap_UpdateChoiceField (MYTAG_START+1514)
#define MYM_DocBitmap_DrawPartial       (MYTAG_START+1515)
#define MYM_DocBitmap_DeleteObj         (MYTAG_START+1516) /* private */
#define MYM_DocBitmap_ChoiceDblClick    (MYTAG_START+1517)
#define MYM_DocBitmap_ChoiceActiveChg   (MYTAG_START+1518)

struct MYS_DocBitmap_CropBox {
    int left;
    int top;
    int right;
    int bottom;
};

#define borderpixels    1
#define shadowpixels    2

/*#define btop    borderpixels
#define bbottom (borderpixels+shadowpixels)
#define bleft   borderpixels
#define bright  (borderpixels+shadowpixels)*/

#define minZoom 0
#define maxStdZoom 10
#define maxZoom 13
#define defZoom 6
#define zoomPage 11
#define zoomWidth 12
#define zoomCustom 13
extern int zoomDPI[];

struct DocBitmapData;
typedef struct DocBitmapData DocBitmapData;

extern struct MUI_CustomClass *docbitmap_mcc;

#define DocBitmapObject NewObject(docbitmap_mcc->mcc_Class,NULL

BOOL createDocBitmapClass(void);
BOOL deleteDocBitmapClass(void);

#endif

