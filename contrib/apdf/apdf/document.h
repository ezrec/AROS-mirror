/*************************************************************************\
 *                                                                       *
 * document.h                                                            *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef DOCUMENT_H
#define DOCUMENT_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_Document_Dir            (MYTAG_START+201) /* I.. */
#define MYATTR_Document_Name           (MYTAG_START+202) /* I.. */
#define MYATTR_Document_NumPages       (MYTAG_START+203) /* ..G */
#define MYATTR_Document_PDFDoc         (MYTAG_START+204) /* ..G */
#define MYATTR_Document_Outlines       (MYTAG_START+205) /* ..G */
#define MYATTR_Document_HasOutlines    (MYTAG_START+206) /* ..G */
#define MYATTR_Document_Application    (MYTAG_START+207) /* I.G */
#define MYATTR_Document_OpenPage       (MYTAG_START+208) /* .SG */
#define MYATTR_Document_OpenZoom       (MYTAG_START+209) /* .SG */
#define MYATTR_Document_OpenRotate     (MYTAG_START+210) /* .SG */
#define MYATTR_Document_OpenCropBox    (MYTAG_START+211) /* .SG */
#define MYATTR_Document_CommInfo       (MYTAG_START+212) /* I.. */
#define MYATTR_Document_OpenPageLabel  (MYTAG_START+213) /* .S. */
#define MYATTR_Document_OpenTextAA     (MYTAG_START+214) /* ..G */
#define MYATTR_Document_OpenStrokeAA   (MYTAG_START+215) /* ..G */
#define MYATTR_Document_OpenFillAA     (MYTAG_START+216) /* ..G */
#define MYATTR_Document_NeedPassword   (MYTAG_START+218) /* I.. */
#define MYATTR_Document_OpenColumns    (MYTAG_START+219) /* ..G */
#define MYATTR_Document_OpenRows       (MYTAG_START+220) /* ..G */
#define MYATTR_Document_OpenContinuous (MYTAG_START+221) /* ..G */
#define MYATTR_Document_OpenDispMode   (MYTAG_START+222) /* ..G */
#define MYATTR_Document_OpenToolBar    (MYTAG_START+223) /* ..G */
#define MYATTR_Document_OpenNavBar     (MYTAG_START+224) /* ..G */
#define MYATTR_Document_OpenFullScreen (MYTAG_START+225) /* ..G */
#define MYATTR_Document_OwnerPW        (MYTAG_START+226) /* I.. */
#define MYATTR_Document_UserPW         (MYTAG_START+227) /* I.. */
#define MYATTR_Document_OpenOutlines   (MYTAG_START+228) /* ..G */

#define MYM_Document_IncRef            (MYTAG_START+1201)
#define MYM_Document_DecRef            (MYTAG_START+1202)
#define MYM_Document_OpenSaveWindow    (MYTAG_START+1203)
#define MYM_Document_OpenPrintWindow   (MYTAG_START+1204)
#define MYM_Document_OpenInfoWindow    (MYTAG_START+1205)
#define MYM_Document_OpenPrefsWindow   (MYTAG_START+1206)
#define MYM_Document_OpenScanWindow    (MYTAG_START+1207)
#define MYM_Document_Save              (MYTAG_START+1208)
#define MYM_Document_Scan              (MYTAG_START+1209)
#define MYM_Document_RefreshWindows    (MYTAG_START+1210)
#define MYM_Document_SavePrefs         (MYTAG_START+1211)
#define MYM_Document_LoadPrefs         (MYTAG_START+1212)
#define MYM_Document_UsePrefs          (MYTAG_START+1213)

extern struct MUI_CustomClass *document_mcc;
extern struct MUI_CustomClass *simpledoc_mcc;
extern struct MUI_CustomClass *rotate_slider_mcc;

#define DocumentObject NewObject(document_mcc->mcc_Class,NULL
#define SimpleDocObject NewObject(simpledoc_mcc->mcc_Class,NULL
#define MyRotateSliderObject NewObject(rotate_slider_mcc->mcc_Class,NULL


BOOL createDocumentClass(void);
BOOL deleteDocumentClass(void);
BOOL createSimpleDocClass(void);
BOOL deleteSimpleDocClass(void);

struct comm_info;

Object *getDocument(struct comm_info *,Object *app,Object* win,BPTR lock,const char* name);
void refresh_doc_fontmaps(void);

#endif

