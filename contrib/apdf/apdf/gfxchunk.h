/*************************************************************************\
 *                                                                       *
 * gfxchunk.h                                                            *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef GFXCHUNK_H
#define GFXCHUNK_H

#ifndef MYTAG_START
#   define MYTAG_START       TAG_USER
#endif

#define MYATTR_GfxChunk_Params          (MYTAG_START+101) /* IS. */
#define MYATTR_GfxChunk_Data            (MYTAG_START+102) /* ..G */
#define MYATTR_GfxChunk_ColorAllocator  (MYTAG_START+103) /* I.. */
#define MYATTR_GfxChunk_Done            (MYTAG_START+104) /* ..G */
#define MYATTR_GfxChunk_Ready           (MYTAG_START+105) /* ..G */
#define MYATTR_GfxChunk_PartialData     (MYTAG_START+106) /* ..G */
#define MYATTR_GfxChunk_Drawing         (MYTAG_START+107) /* ..G */
#define MYATTR_GfxChunk_BitMap          (MYTAG_START+108) /* ..G */

struct MYS_GfxChunk_Params {
    struct AOutputDev *aout;
    /* buffer properties */
    int width;
    int height;
    int modulo;
    int bpp;
    int pixfmt;
    struct BitMap *friendbm;
    /* buffer contents parameters */
    struct PDFDoc *pdfdoc;
    int page;
    int xoffset;
    int yoffset;
    int dpi;
    int rotate;
    int level;
};

#define MYM_GfxChunk_Clear          (MYTAG_START+1101)
#define MYM_GfxChunk_StartDrawing   (MYTAG_START+1102)
#define MYM_GfxChunk_RestartDrawing (MYTAG_START+1103)
#define MYM_GfxChunk_Replied        (MYTAG_START+1104) /* private */
#define MYM_GfxChunk_SendInit       (MYTAG_START+1105) /* private */

extern struct MUI_CustomClass *gfxchunk_mcc;

#define GfxChunkObject NewObject(gfxchunk_mcc->mcc_Class,NULL

BOOL createGfxChunkClass(void);
BOOL deleteGfxChunkClass(void);

#endif

