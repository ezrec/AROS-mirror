/*************************************************************************\
 *                                                                       *
 * docgroup.h                                                            *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef DOCGROUP_H
#define DOCGROUP_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_DocGroup_Document    (MYTAG_START+601) /* ISG */
#define MYATTR_DocGroup_DocChanged  (MYTAG_START+602) /* ..G */
#define MYATTR_DocGroup_Page        (MYTAG_START+603) /* ISG */
#define MYATTR_DocGroup_Zoom        (MYTAG_START+604) /* ISG */
#define MYATTR_DocGroup_Rotate      (MYTAG_START+605) /* ISG */
#define MYATTR_DocGroup_CropBox     (MYTAG_START+606) /* ISG */
#define MYATTR_DocGroup_CommInfo    (MYTAG_START+607) /* I.. */
#define MYATTR_DocGroup_TextAA      (MYTAG_START+608) /* ISG */
#define MYATTR_DocGroup_StrokeAA    (MYTAG_START+609) /* ISG */
#define MYATTR_DocGroup_FillAA      (MYTAG_START+610) /* ISG */
#define MYATTR_DocGroup_Run         (MYTAG_START+611) /* ..G */
#define MYATTR_DocGroup_URL         (MYTAG_START+612) /* ..G */
#define MYATTR_DocGroup_Open        (MYTAG_START+613) /* ..G */
#define MYATTR_DocGroup_OpenNew     (MYTAG_START+614) /* ..G */
#define MYATTR_DocGroup_PageNew     (MYTAG_START+615) /* ..G */
#define MYATTR_DocGroup_PageLabel   (MYTAG_START+616) /* .S. */
#define MYATTR_DocGroup_MainObject  (MYTAG_START+617) /* ..G */
#define MYATTR_DocGroup_ShowToolBar (MYTAG_START+618) /* IS. */
#define MYATTR_DocGroup_ShowNavBar  (MYTAG_START+619) /* IS. */
#define MYATTR_DocGroup_FullScreen  (MYTAG_START+620) /* I.. */

#define MYM_DocGroup_Search         (MYTAG_START+1601)
#define MYM_DocGroup_OpenOutlines   (MYTAG_START+1602)
#define MYM_DocGroup_GoToOutline    (MYTAG_START+1603)
#define MYM_DocGroup_CloseOutlines  (MYTAG_START+1604)
#define MYM_DocGroup_Ready          (MYTAG_START+1605) /* private */
#define MYM_DocGroup_Refresh        (MYTAG_START+1606)

extern struct MUI_CustomClass *docgroup_mcc;
extern struct MUI_CustomClass *vdocgroup_mcc;

#define DocGroupObject NewObject(docgroup_mcc->mcc_Class,NULL
#define VDocGroupObject NewObject(vdocgroup_mcc->mcc_Class,NULL

BOOL createDocGroupClass(void);
BOOL deleteDocGroupClass(void);
BOOL createVDocGroupClass(void);
BOOL deleteVDocGroupClass(void);

#endif

