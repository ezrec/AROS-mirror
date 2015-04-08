/*************************************************************************\
 *                                                                       *
 * docscroll.h                                                           *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef DOCSCROLL_H
#define DOCSCROLL_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_DocScroll_Contents       (MYTAG_START+350) /* I.. */
#define MYATTR_DocScroll_Left           (MYTAG_START+351) /* ISG */
#define MYATTR_DocScroll_Top            (MYTAG_START+352) /* ISG */
#define MYATTR_DocScroll_Width          (MYTAG_START+353) /* ..G */
#define MYATTR_DocScroll_Height         (MYTAG_START+354) /* ..G */
#define MYATTR_DocScroll_Document       (MYTAG_START+355) /* ISG */
#define MYATTR_DocScroll_Page           (MYTAG_START+356) /* IS. */
#define MYATTR_DocScroll_ViewMode       (MYTAG_START+357) /* ISG */
#define MYATTR_DocScroll_Quiet          (MYTAG_START+358) /* IS. */
#define MYATTR_DocScroll_Zoom           (MYTAG_START+359) /* ISG */
#define MYATTR_DocScroll_NoHistory      (MYTAG_START+360) /* .S. */
#define MYATTR_DocScroll_DocChanged     (MYTAG_START+361) /* ..G */
#define MYATTR_DocScroll_CommInfo       (MYTAG_START+362) /* I.. */
#define MYATTR_DocScroll_FullScreen     (MYTAG_START+363) /* I.. */

#define MYM_DocScroll_PageDown          (MYTAG_START+1350)
#define MYM_DocScroll_PageUp            (MYTAG_START+1351)
#define MYM_DocScroll_PosChanged        (MYTAG_START+1352)
#define MYM_DocScroll_Layout            (MYTAG_START+1353)
#define MYM_DocScroll_ResetData         (MYTAG_START+1354)
#define MYM_DocScroll_UpdateScroll      (MYTAG_START+1355)
#define MYM_DocScroll_UpdateState       (MYTAG_START+1356)
#define MYM_DocScroll_Backward          (MYTAG_START+1357)
#define MYM_DocScroll_Forward           (MYTAG_START+1358)
#define MYM_DocScroll_HistoryPush       (MYTAG_START+1359)

#define MYATTR_Virtgroup_GeometryChange (MYTAG_START+450) /* ..G */

enum ViewMode {
    vmFix, vmContinuous, vmFitPage, vmSpecial
};

struct MYP_DocScroll_PageUpDown {
    LONG MethodID;
    int percent;
};

struct MYP_DocScroll_HistoryPush {
    LONG MethodID;
    int page;
    BPTR dir;
    char *name;
};


extern struct MUI_CustomClass *docscroll_mcc;
extern struct MUI_CustomClass *vgroup_mcc;

#define DocScrollObject NewObject(docscroll_mcc->mcc_Class,NULL
#define myVirtgroupObject NewObject(vgroup_mcc->mcc_Class,NULL

BOOL createDocScrollClass(void);
BOOL deleteDocScrollClass(void);

#endif

