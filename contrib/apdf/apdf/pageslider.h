/*************************************************************************\
 *                                                                       *
 * pageslider.h                                                          *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef PAGESLIDER_H
#define PAGESLIDER_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_PageSlider_Value       (MYTAG_START+901) /* .SG */
#define MYATTR_PageSlider_Document    (MYTAG_START+902) /* IS. */
#define MYATTR_PageSlider_CommInfo    (MYTAG_START+903) /* I.. */
#define MYATTR_PageSlider_Label       (MYTAG_START+904) /* .S. */
#define MYATTR_PageSlider_StringObj   (MYTAG_START+905) /* I.. */
#define MYATTR_PageSlider_PDFDoc      (MYTAG_START+906) /* IS. private */
#define MYATTR_PageSlider_Value1      (MYTAG_START+907) /* .SG private */

extern struct MUI_CustomClass *pageslider_mcc;

#define PageSliderObject NewObject(pageslider_mcc->mcc_Class,NULL

BOOL createPageSliderClass(void);
BOOL deletePageSliderClass(void);

#endif

