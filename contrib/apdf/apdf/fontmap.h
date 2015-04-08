/*************************************************************************\
 *                                                                       *
 * fontmap.h                                                             *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef FONTMAP_H
#define FONTMAP_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

//#define MYATTR_FontMap_Dir      (MYTAG_START+401) /* I.. */
//#define MYATTR_FontMap_Name     (MYTAG_START+402) /* I.. */
//#define MYATTR_FontMap_Scan     (MYTAG_START+403) /* I.. */
//#define MYATTR_FontMap_PDFDoc   (MYTAG_START+404) /* I.. */
//#define MYATTR_FontMap_CommInfo (MYTAG_START+405) /* I.. */
//#define MYATTR_FontMap_Changed  (MYTAG_START+406) /* ..G */
#define MYATTR_FontMap_Entries  MUIA_List_Entries

#define MYM_FontMap_Activate       (MYTAG_START+1401)
#define MYM_FontMap_Update         (MYTAG_START+1402)
#define MYM_FontMap_Add            (MYTAG_START+1403)
//#define MYM_FontMap_Apply          (MYTAG_START+1406)
#define MYM_FontMap_Clear          MUIM_List_Clear
#define MYM_FontMap_InsertSingle   MUIM_List_InsertSingle
#define MYM_FontMap_GetEntry       MUIM_List_GetEntry

#define MYATTR_FontInfoLV_CommInfo (MYTAG_START+450) /* I.. */

#define MYM_FontInfoLV_AddDevContents (MYTAG_START+1450)

struct MYP_FontMap_Activate {
    ULONG MethodID;
    LONG num;
};

struct MYP_FontMap_AddDevContents {
    LONG MethodID;
    struct FontOutputDev *out;
    int num;
};

struct fontmapentry {
    char pdffont[128];
    char font[128];
    int type;
    int flags;
};


extern struct MUI_CustomClass *FontMap_mcc;
extern struct MUI_CustomClass *fontinfolv_mcc;

#define FontMapObject NewObject(FontMap_mcc->mcc_Class,NULL
#define FontInfoLVObject NewObject(fontinfolv_mcc->mcc_Class,NULL


BOOL createFontMapClass(void);
BOOL deleteFontMapClass(void);

void add_fontmap(struct comm_info *ci,
		 struct PDFDoc *pdfdoc,const char* pdffont,
		 const char* font,int type);
void clear_fontmap(struct comm_info *ci,struct PDFDoc *pdfdoc);

#endif

