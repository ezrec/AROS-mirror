/*
 * @(#) $Header$
 *
 * BGUI library
 * vectorclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.7  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.6  2003/01/18 19:10:03  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.5  2000/08/08 20:57:26  chodorowski
 * Minor fixes to build on Amiga.
 *
 * Revision 42.4  2000/05/29 15:42:49  stegerg
 * fixed some "comparison is always 1 due to limited range of data type"
 * errors
 *
 * Revision 42.3  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:48  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:10:34  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:23  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/11/16 20:01:56  mlemos
 * Replaced AllocVec and FreeVec calls by BGUI_AllocPoolMem and
 * BGUI_FreePoolMem calls respectively.
 * Restored prior RastPort AreaInfo and TmpRas after rendering.
 *
 * Revision 41.10  1998/02/25 21:13:26  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:10:00  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.

#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct {
   struct TmpRas      vd_TmpRas;        /* TmpRas for areafill's.     */
   struct AreaInfo    vd_AreaInfo;      /* AreaInfo for areafill's.   */
   UWORD              vd_SizeX;         /* Design width.              */
   UWORD              vd_SizeY;         /* Design height.             */
   UWORD              vd_MinWidth;      /* Minimum width.             */
   UWORD              vd_MinHeight;     /* Minimum height.            */
   UWORD              vd_BuiltInType;   /* Built-in type.             */
   UWORD              vd_Flags;         /* Flags (see below).         */
   UWORD              vd_Pen;           /* Pen to use for drawing.    */
   UWORD              vd_DriPen;        /* DriPen to use for drawing. */
   struct VectorItem *vd_VectorArray;   /* The vector elements.       */
   PLANEPTR           vd_AreaPlane;     /* Bit-plane for areafills.   */
   WORD              *vd_VertexBuffer;  /* Buffer for areafills.      */
   UWORD              vd_PlaneWidth;    /* Width of area-plane.       */
   UWORD              vd_PlaneHeight;   /* Height of area-plane.      */
   UWORD              vd_ErasePen;      /* Pen to erase with.         */
   struct TmpRas     *vd_OldTmpRas;     /* TmpRas before rendering.   */
   struct AreaInfo   *vd_OldAreaInfo;   /* AreaInfo before rendering. */
}  VD;

#define VDATF_AREA       (1<<0)         /* We are area filling.       */

///
/// Built-in images.

/*
 * Built-in GETPATH image.
 */
STATIC struct VectorItem BuiltinGetPath[] = {
   { 20, 14,  VIF_SCALE                                 },
   {  4,  4,  VIF_MOVE | VIF_AREASTART                  },
   {  8,  4,  VIF_DRAW                                  },
   {  9,  5,  VIF_DRAW                                  },
   {  5,  5,  VIF_DRAW                                  },
   {  5,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  4,  VIF_AREAEND                               },
   {  8,  4,  VIF_MOVE                                  },
   {  9,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  6,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  4,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  4,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  9,  5,  VIF_MOVE                                  },
   {  8,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT | VIF_LASTITEM   },
};

/*
 * Built-in GETFILE image.
 */
STATIC struct VectorItem BuiltinGetFile[] = {
   { 20, 14,  VIF_SCALE                                 },
   {  4,  2,  VIF_MOVE | VIF_AREASTART                  },
   {  5,  2,  VIF_DRAW                                  },
   {  5,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  2,  VIF_AREAEND                               },
   {  4,  2,  VIF_MOVE                                  },
   {  8,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  8,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  8,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  2,  VIF_MOVE | VIF_YRELBOTTOM                 },
   {  4,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  6,  VIF_MOVE | VIF_XRELRIGHT | VIF_AREASTART  },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  6,  VIF_LASTITEM | VIF_XRELRIGHT              },
};

/*
 * Built-in CheckMark image.
 */
STATIC struct VectorItem BuiltinCheckMark[] = {
   { 24, 11,  VIF_SCALE                                 },
   {  6,  5,  VIF_MOVE | VIF_AREASTART                  },
   {  9,  8,  VIF_DRAW                                  },
   { 11,  8,  VIF_DRAW                                  },
   {  6,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  5,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  7,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   { 11,  7,  VIF_DRAW                                  },
   { 10,  7,  VIF_DRAW                                  },
   {  8,  5,  VIF_DRAW                                  },
   {  6,  5,  VIF_LASTITEM                              },
};

/*
 * Built-in RadioButton image.
 */
STATIC struct VectorItem BuiltinRadioButton[] = {
   { 16,  8,  VIF_SCALE                                 },
   {  4,  3,  VIF_MOVE | VIF_AREASTART                  },
   {  6,  2,  VIF_DRAW                                  },
   {  6,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  3,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  3,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  6,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  6,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  3,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  3,  VIF_LASTITEM                              },
};

#ifdef __AROS__
#warning A comment within a comment makes gcc puke...
#if 0
/*******************************************************************************
/*
 * Built-in PopUp image.
 */
STATIC struct VectorItem BuiltInPopUp[] = {
   {  44,  20,  VIF_MOVE | VIF_AREASTART },
   {  56,  20,  VIF_DRAW                 },
   {  56,  35,  VIF_DRAW                 },
   {  73,  35,  VIF_DRAW                 },
   {  50,  55,  VIF_DRAW                 },
   {  27,  35,  VIF_DRAW                 },
   {  44,  35,  VIF_DRAW                 },
   {  44,  20,  VIF_DRAW | VIF_AREAEND   },
   {  27,  70,  VIF_MOVE | VIF_AREASTART },
   {  73,  70,  VIF_DRAW                 },
   {  73,  77,  VIF_DRAW                 },
   {  27,  77,  VIF_DRAW | VIF_AREAEND   },
   {  18,  14,  VIF_MINSIZE              },
   { 100, 100,  VIF_SCALE | VIF_LASTITEM },
};

/*
 * Built-in Cycle image.
 */
STATIC struct VectorItem BuiltInCycle[] = {
   { 18, 14,  VIF_SCALE                                 },
   {  5,  3,  VIF_MOVE | VIF_AREASTART | VIF_YRELBOTTOM },
   {  6,  3,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  4,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  4,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  5,  4,  VIF_DRAW                                  },
   {  6,  4,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  6,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  8,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  6,  8,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  5,  8,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  3,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  5,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  5,  4,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  6,  3,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  5,  3,  VIF_DRAW                                  },
   {  4,  4,  VIF_DRAW                                  },
   {  4,  4,  VIF_DRAW | VIF_AREAEND   | VIF_YRELBOTTOM | VIF_LASTITEM },
};

/*
 * Built-in Cycle image - selected.
 */
STATIC struct VectorItem BuiltInCycle2[] = {
   { 18, 14,  VIF_SCALE                                 },
   {  5,  3,  VIF_MOVE | VIF_XRELRIGHT | VIF_AREASTART  },
   {  6,  3,  VIF_DRAW                                  },
   {  5,  4,  VIF_DRAW                                  },
   {  5,  4,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  5,  4,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  6,  4,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  6,  6,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  8,  6,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  6,  8,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  5,  8,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  3,  6,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  5,  6,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  5,  4,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  6,  3,  VIF_DRAW                 | VIF_YRELBOTTOM },
   {  5,  3,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  4,  4,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  4,  4,  VIF_DRAW | VIF_XRELRIGHT | VIF_AREAEND    | VIF_LASTITEM },
};
*****************************************************************************************/
#endif
#endif

/*
 * Built-in PopUp image.
 */
STATIC struct VectorItem BuiltInPopUp[] = {
   { 15, 13, VIF_SCALE  },
   {  6,  2, VIF_MOVE | VIF_AREASTART  },
   {  8,  2, VIF_DRAW   },
   {  8,  4, VIF_DRAW   },
   { 10,  4, VIF_DRAW   },
   {  7,  7, VIF_DRAW   },
   {  4,  4, VIF_DRAW   },
   {  6,  4, VIF_DRAW   },
   {  6,  2, VIF_DRAW | VIF_AREAEND    },
   {  4,  9, VIF_MOVE | VIF_AREASTART  },
   { 10,  9, VIF_DRAW   },
   { 10, 10, VIF_DRAW   },
   {  4, 10, VIF_DRAW | VIF_AREAEND | VIF_LASTITEM },
};

/*
 * Built-in Cycle image.
 */
STATIC struct VectorItem BuiltInCycle[] = {
   { 17, 13, VIF_SCALE },
   {  4, 10, VIF_MOVE | VIF_AREASTART},
   { 10, 10, VIF_DRAW },
   { 11,  9, VIF_DRAW },
   {  4,  9, VIF_DRAW },
   {  4,  3, VIF_DRAW },
   { 10,  3, VIF_DRAW },
   { 10,  5, VIF_DRAW },
   {  8,  5, VIF_DRAW },
   { 10,  7, VIF_DRAW },
   { 11,  7, VIF_DRAW },
   { 13,  5, VIF_DRAW },
   { 11,  5, VIF_DRAW },
   { 11,  3, VIF_DRAW },
   { 10,  2, VIF_DRAW },
   {  4,  2, VIF_DRAW },
   {  3,  3, VIF_DRAW },
   {  3,  9, VIF_DRAW | VIF_AREAEND | VIF_LASTITEM },
};

/*
 * Built-in Cycle image - selected.
 */
STATIC struct VectorItem BuiltInCycle2[] = {
   { 17, 13, VIF_SCALE },
   { 11,  2, VIF_MOVE | VIF_AREASTART},
   {  5,  2, VIF_DRAW },
   {  4,  3, VIF_DRAW },
   { 11,  3, VIF_DRAW },
   { 11,  9, VIF_DRAW },
   {  5,  9, VIF_DRAW },
   {  5,  7, VIF_DRAW },
   {  7,  7, VIF_DRAW },
   {  5,  5, VIF_DRAW },
   {  4,  5, VIF_DRAW },
   {  2,  7, VIF_DRAW },
   {  4,  7, VIF_DRAW },
   {  4,  9, VIF_DRAW },
   {  5, 10, VIF_DRAW },
   { 11, 10, VIF_DRAW },
   { 12,  9, VIF_DRAW },
   { 12,  3, VIF_DRAW | VIF_AREAEND | VIF_LASTITEM },
};

/*
 * Built-in arrow up image.
 */
STATIC struct VectorItem BuiltInArrowUp[] = {
   { 16,  9,  VIF_SCALE                                 },
   {  3,  6,  VIF_MOVE | VIF_AREASTART                  },
   {  8,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  8,  2,  VIF_DRAW                                  },
   {  3,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  8,  3,  VIF_DRAW                                  },
   {  8,  3,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  6,  VIF_DRAW | VIF_LASTITEM                   },
};

/*
 * Built-in arrow DOWN vector image.
 */
STATIC struct VectorItem BuiltInArrowDown[] = {
   { 16,  9,  VIF_SCALE                                 },
   {  3,  6,  VIF_MOVE | VIF_AREASTART | VIF_YRELBOTTOM },
   {  8,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  8,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  3,  6,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  8,  3,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  8,  3,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  4,  6,  VIF_DRAW | VIF_LASTITEM  | VIF_YRELBOTTOM },
};

/*
 * Built-in arrow LEFT vector image.
 */
STATIC struct VectorItem BuiltInArrowLeft[] = {
   { 10, 12,  VIF_SCALE                                 },
   {  6,  2,  VIF_MOVE | VIF_AREASTART },
   {  3,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  3,  6,  VIF_DRAW                                  },
   {  6,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  6,  3,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  6,  VIF_DRAW                                  },
   {  4,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  6,  3,  VIF_DRAW | VIF_LASTITEM                   },
};

/*
 * Built-in arrow RIGHT vector image.
 */
STATIC struct VectorItem BuiltInArrowRight[] = {
   { 10, 12,  VIF_SCALE                                 },
   {  6,  2,  VIF_MOVE | VIF_AREASTART | VIF_XRELRIGHT  },
   {  3,  6,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  3,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  6,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  6,  3,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  6,  3,  VIF_DRAW | VIF_LASTITEM | VIF_XRELRIGHT   },
};

/*
 * Built-in GETFONT image.
 */
STATIC struct VectorItem BuiltInGetFont[] = {
   {  8,  4,  VIF_MOVE | VIF_AREASTART                  },
   { 10,  4,  VIF_DRAW                                  },
   { 10,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  8,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  8,  4,  VIF_AREAEND                               },
   {  8,  4,  VIF_MOVE | VIF_AREASTART                  },
   { 18,  4,  VIF_DRAW                                  },
   { 20,  6,  VIF_DRAW                                  },
   { 20, 12,  VIF_DRAW                                  },
   { 18, 14,  VIF_DRAW                                  },
   { 20, 12,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 20,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 18,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  9,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  9,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 17,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 18,  7,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 18, 12,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 17, 13,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  9, 13,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  9, 13,  VIF_DRAW                                  },
   { 17, 13,  VIF_DRAW                                  },
   { 18, 12,  VIF_DRAW                                  },
   { 18,  7,  VIF_DRAW                                  },
   { 17,  6,  VIF_DRAW                                  },
   {  9,  6,  VIF_DRAW | VIF_AREAEND                    },

   { 24,  4,  VIF_MOVE | VIF_AREASTART                  },
   { 26,  4,  VIF_DRAW                                  },
   { 26,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 24,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 24,  4,  VIF_DRAW | VIF_AREAEND                    },
   { 25, 13,  VIF_MOVE | VIF_AREASTART                  },
   { 30, 13,  VIF_DRAW                                  },
   { 32, 13,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 32,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 30,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 25,  4,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 25,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 29,  6,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 30,  7,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 30, 12,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 29, 13,  VIF_DRAW | VIF_YRELBOTTOM                 },
   { 25, 13,  VIF_DRAW | VIF_YRELBOTTOM | VIF_AREAEND   },

   { 20, 14,  VIF_MINSIZE                               },
   { 40, 28,  VIF_SCALE | VIF_LASTITEM                  },
};

/*
 * Built-in GETSCREEN image.
 */
STATIC struct VectorItem BuiltInGetScreen[] = {
   { 20, 14,  VIF_SCALE                                 },
   {  4,  2,  VIF_MOVE | VIF_AREASTART                  },
   {  5,  2,  VIF_DRAW                                  },
   {  5,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  2,  VIF_DRAW | VIF_YRELBOTTOM                 },
   {  4,  2,  VIF_AREAEND                               },
   {  4,  2,  VIF_MOVE                                  },
   {  8,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  8,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  8,  2,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  2,  VIF_MOVE | VIF_YRELBOTTOM                 },
   {  4,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  6,  VIF_MOVE | VIF_XRELRIGHT | VIF_AREASTART  },
   {  4,  6,  VIF_DRAW | VIF_XRELRIGHT                  },
   {  4,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  2,  VIF_DRAW | VIF_XRELRIGHT | VIF_YRELBOTTOM },
   {  5,  6,  VIF_LASTITEM | VIF_XRELRIGHT              },
};

static struct VectorItem *BuiltIn[] =
{
   NULL,
   BuiltinGetPath,
   BuiltinGetFile,
   BuiltinCheckMark,
   BuiltInPopUp,
   BuiltInArrowUp,
   BuiltInArrowDown,
   BuiltInArrowLeft,
   BuiltInArrowRight,
   BuiltInCycle,
   BuiltInCycle2,
   BuiltinRadioButton,
   BuiltInGetFont,
   BuiltInGetScreen,
};
///

/*
 * Deallocate the AreaFill() stuff.
 */
//STATIC ASM VOID CleanUpArea( REG(a0) struct RastPort *rp, REG(a1) VD *vd )
STATIC ASM REGFUNC2(VOID, CleanUpArea,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(A1, VD *, vd))
{
   if (vd->vd_Flags & VDATF_AREA)
   {
      /*
       * Don't deallocate to soon.
       */
      WaitBlit();

      rp->TmpRas=vd->vd_OldTmpRas;
      rp->AreaInfo=vd->vd_OldAreaInfo;

      /*
       * Deallocate the buffers.
       */
      if (vd->vd_AreaPlane)
      {
         FreeRaster( vd->vd_AreaPlane, vd->vd_PlaneWidth, vd->vd_PlaneHeight );
         vd->vd_AreaPlane = NULL;
      }

      if (vd->vd_VertexBuffer)
      {
         BGUI_FreePoolMem( vd->vd_VertexBuffer );
         vd->vd_VertexBuffer = NULL;
      }

      vd->vd_PlaneWidth = vd->vd_PlaneHeight = 0;

      /*
       * Mark us as not area filling.
       */
      vd->vd_Flags &= ~VDATF_AREA;
   };
}
REGFUNC_END

/*
 * Allocate the necessary stuff
 * for area filling. If this fails
 * the image areas are not filled.
 */
STATIC ULONG SetupArea( struct RastPort *rp, VD *vd, struct VectorItem *vi, UWORD w, UWORD h )
{
   UWORD              numelem = 0;
   struct VectorItem *vi2;

   /*
    * Count the number of vector elements
   ** in the area fill operation.
   **/
   for (vi2 = vi; !(vi2->vi_Flags & (VIF_AREAEND|VIF_LASTITEM)); vi2++);

   /*
    * Safety margins.
    */
   numelem = vi2 - vi + 2;

   w <<= 1;
   h <<= 1;

   /*
    * Allocate and setup the buffers etc.
    */
   if (vd->vd_VertexBuffer = (UWORD *)BGUI_AllocPoolMem(numelem * 5))
   {
      memset(vd->vd_VertexBuffer,'\0',numelem * 5);
      if (vd->vd_AreaPlane = AllocRaster(w, h)) {
         /*
          * Initialize structures.
          */
         InitTmpRas( &vd->vd_TmpRas, vd->vd_AreaPlane, RASSIZE( w, h ));
         InitArea( &vd->vd_AreaInfo, vd->vd_VertexBuffer, numelem );
         /*
          * Set them up.
          */
         vd->vd_OldTmpRas   = rp->TmpRas;
         rp->TmpRas         = &vd->vd_TmpRas;
         vd->vd_OldAreaInfo = rp->AreaInfo;
         rp->AreaInfo       = &vd->vd_AreaInfo;
         /*
          * Mark raster size.
          */
         vd->vd_PlaneWidth  = w;
         vd->vd_PlaneHeight = h;
         /*
          * Mark us as area filling.
          */
         vd->vd_Flags |= VDATF_AREA;

         return 1;
      }
   }
   /*
    * Oops. Screw up somewhere.
    */
   CleanUpArea(rp, vd);

   return 0;
}

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(VectorClassNew, struct opSet *, ops)
{
   VD       *vd;
   ULONG     rc;

   /*
    * First we let the superclass
    * create a new object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Get us the instance data.
       */
      vd = INST_DATA(cl, rc);

      /*
       * Setup defaults.
       */
      vd->vd_Pen = vd->vd_DriPen = (UWORD)~0;

      /*
       * Setup the object.
       */
      AsmCoerceMethod(cl, (Object *)rc, OM_SET, ops->ops_AttrList, NULL);
   }
   return rc;
}
METHOD_END
///
/// OM_SET
/*
 * Change one or more attrubutes.
 */
METHOD(VectorClassSet, struct opSet *, ops)
{
   VD                *vd = INST_DATA(cl, obj);
   struct TagItem    *tag, *tstate = ops->ops_AttrList;
   ULONG              data;
   BOOL               resize = FALSE;
   struct VectorItem *vi;
   ULONG              rc;

   /*
    * First we let the superclass
    * change the attributes it
    * knows.
    */
   rc = AsmDoSuperMethodA(cl, obj, (Msg)ops);

   /*
    * Get the attributes.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case VIT_VectorArray:
         vd->vd_VectorArray = (struct VectorItem *)data;
         resize = TRUE;
         break;

      case VIT_BuiltIn:
         vd->vd_VectorArray = BuiltIn[data];
         vd->vd_BuiltInType = data;
         resize = TRUE;
         break;

      case VIT_Pen:
         vd->vd_Pen = data;
         break;

      case VIT_DriPen:
         vd->vd_DriPen = data;
         break;

      case IMAGE_ErasePen:
         vd->vd_ErasePen = data;
         break;
      };
   };

   if (resize)
   {
      /*
       * Setup the default minimums.
       */
      vd->vd_MinWidth  = 0;
      vd->vd_MinHeight = 0;

      for (vi = vd->vd_VectorArray; resize; vi++)
      {
         if (vi->vi_Flags & VIF_SCALE)
         {
            /*
             * Setup the design size.
             */
            vd->vd_SizeX = vi->vi_x;
            vd->vd_SizeY = vi->vi_y;
         };
         if (vi->vi_Flags & VIF_MINSIZE)
         {
            /*
             * Setup the design size.
             */
            vd->vd_MinWidth  = vi->vi_x;
            vd->vd_MinHeight = vi->vi_y;
         };
         if (vi->vi_Flags & VIF_LASTITEM) resize = FALSE;
      };
      if (!vd->vd_MinWidth)  vd->vd_MinWidth  = vd->vd_SizeX;
      if (!vd->vd_MinHeight) vd->vd_MinHeight = vd->vd_SizeY;
   };
   return rc;
}
METHOD_END
///
/// OM_GET
/*
 * Give an attribute value.
 */
METHOD(VectorClassGet, struct opGet *, opg)
{
   VD          *vd = INST_DATA( cl, obj );
   ULONG        rc = 1, *store = opg->opg_Storage;

   /*
    * Firts we see if the attribute
    * they want is know to us. If not
    * we pass it onto the superclass.
    */
   switch (opg->opg_AttrID)
   {
   case VIT_VectorArray:   STORE vd->vd_VectorArray;  break;
   case VIT_BuiltIn:       STORE vd->vd_BuiltInType;  break;
   case VIT_Pen:           STORE vd->vd_Pen;          break;
   case VIT_DriPen:        STORE vd->vd_DriPen;       break;
   case VIT_MinWidth:      STORE vd->vd_MinWidth;     break;
   case VIT_MinHeight:     STORE vd->vd_MinHeight;    break;
   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   }

   return rc;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the vector image.
 */
METHOD(VectorClassRender, struct bmRender *, bmr)
{
   VD                *vd = INST_DATA(cl, obj);
   struct VectorItem *vi = vd->vd_VectorArray;
   struct BaseInfo   *bi = bmr->bmr_BInfo;
   struct RastPort   *rp = bi->bi_RPort;
   ULONG              rc = 1, f;
   int                l, t, r, b, w, h, x, y;
   BOOL               go = TRUE;

   /*
    * Only render if there are vector
    * elements to render.
    */
   if (!vi) return 1;

   /*
    * Get image position and dimensions.
    */
   l = IMAGE(obj)->LeftEdge;
   t = IMAGE(obj)->TopEdge;
   w = IMAGE(obj)->Width;
   h = IMAGE(obj)->Height;

   /*
    * Get image right and bottom edge coordinates.
    */
   r = l + w - 1;
   b = t + h - 1;

   /*
    * Setup the correct startup-pen.
    */
   if (vd->vd_DriPen != (UWORD)~0)
      BSetDPenA(bi, vd->vd_DriPen);
   else if (vd->vd_Pen != (UWORD)~0)
      BSetPenA(bi, vd->vd_Pen);
   else
   {
      switch (bmr->bmr_Flags)
      {
      case IDS_SELECTED:
         BSetDPenA(bi, FILLTEXTPEN);
         break;

      case IDS_INACTIVESELECTED:
      default:
         BSetDPenA(bi, TEXTPEN);
         break;
      };
   };

   /*
    * Jam only only color.
    */
   BSetDrMd(bi, JAM1);

   /*
    * Clear rp pattern.
    */
   BClearAfPt(bi);
   BNDRYOFF(rp);

   /*
    * Loop through the vector elements.
    */
   for (; go; vi++)
   {
      f = vi->vi_Flags;
      x = vi->vi_x;
      y = vi->vi_y;

      go = !(f & VIF_LASTITEM);

      if (f & (VIF_SCALE|VIF_MINSIZE))
         continue;

      if (f & VIF_COLOR)
      {
         BSetPenA(bi, x);
         if (f & VIF_BPEN) BSetPenB(bi, y);
         continue;
      };
      if (f & VIF_DRIPEN)
      {
         BSetDPenA(bi, x);
         if (f & VIF_BPEN) BSetDPenB(bi, y);
         continue;
      };
      if (f & VIF_DRAWMODE)
      {
         BSetDrMd(bi, x);
         continue;
      };

      if (f & VIF_LINEPATTERN)
      {
         SetDrPt(rp, x);
         BSetDrMd(bi, JAM2);
         continue;
      };

      if      (f & VIF_AOLPEN)  SetOPen(rp, x);
      else if (f & VIF_ENDOPEN) BNDRYOFF(rp);

      if      (f & VIF_SHADOWPEN) BSetDPenA(bi, SHADOWPEN);
      else if (f & VIF_SHINEPEN)  BSetDPenA(bi, SHINEPEN);
      else if (f & VIF_FILLPEN)   BSetDPenA(bi, FILLPEN);
      else if (f & VIF_TEXTPEN)   BSetDPenA(bi, TEXTPEN);

      /*
       * Do we need to setup an areafill?
       */
      if (f & VIF_AREASTART) SetupArea(rp, vd, vi, w, h);

      /*
       * Calculate the X and Y coordinates.
       */
      if (vd->vd_SizeX) x = (x * w) / vd->vd_SizeX;
      if (vd->vd_SizeY) y = (y * h) / vd->vd_SizeY;

      /*
       * Calculate X from the correct edge.
       */
      if (f & VIF_XRELRIGHT)  x = r - x;
      else x += l;

      /*
       * Calculate Y from the correct edge.
       */
      if (f & VIF_YRELBOTTOM) y = b - y;
      else y += t;

      /*
       * Check that the coordinates do
       * not go outside the bounding box.
       */
      if      (x < l) x = l;
      else if (x > r) x = r;

      if      (y < t) y = t;
      else if (y > b) y = b;

      /*
       * (Area)Move or (Area)Draw to the
       * coordinates.
       */
      if (f & VIF_MOVE)
      {
         if (vd->vd_Flags & VDATF_AREA) AreaMove(rp, x, y);
         else                           Move(rp, x, y);
      }
      else if (f & VIF_DRAW)
      {
         if (vd->vd_Flags & VDATF_AREA) AreaDraw(rp, x, y);
         else                           Draw(rp, x, y);
      }

      /*
       * Are we done area filling?
       */
      if (f & (VIF_AREAEND|VIF_LASTITEM))
      {
         if (vd->vd_Flags & VDATF_AREA)
         {
            AreaEnd(rp);
            CleanUpArea(rp, vd);
         }
      }
   }
   /*
    * In case a color change or something unexpected gave us a VIF_AREAEND.
    */
   CleanUpArea(rp, vd);

   return rc;
}
METHOD_END
///

/// Class initialization.
/*
 * Class function array.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,            (FUNCPTR)VectorClassRender,
   OM_NEW,                 (FUNCPTR)VectorClassNew,
   OM_SET,                 (FUNCPTR)VectorClassSet,
   OM_GET,                 (FUNCPTR)VectorClassGet,
   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitVectorClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_IMAGE_OBJECT,
                         CLASS_ObjectSize,     sizeof(VD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///

