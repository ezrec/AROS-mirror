/*************************************************************************\
 *                                                                       *
 * doclens.h                                                             *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef DOCLENS_H
#define DOCLENS_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_DocLens_Document    (MYTAG_START+701) /* ISG */
#define MYATTR_DocLens_Page        (MYTAG_START+702) /* ISG */
#define MYATTR_DocLens_DPI         (MYTAG_START+703) /* ISG */
#define MYATTR_DocLens_Rotate      (MYTAG_START+704) /* ISG */
#define MYATTR_DocLens_CropBox     (MYTAG_START+705) /* ISG */
#define MYATTR_DocLens_CommInfo    (MYTAG_START+706) /* I.. */
#define MYATTR_DocLens_TextAA      (MYTAG_START+707) /* ISG */
#define MYATTR_DocLens_StrokeAA    (MYTAG_START+708) /* ISG */
#define MYATTR_DocLens_FillAA      (MYTAG_START+709) /* ISG */

extern struct MUI_CustomClass *doclens_mcc;

#define DocLensObject NewObject(doclens_mcc->mcc_Class,NULL

BOOL createDocLensClass(void);
BOOL deleteDocLensClass(void);

#endif

