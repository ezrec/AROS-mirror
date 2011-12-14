#ifndef  __MUIMINE_H_
#define  __MUIMINE_H_


/*************************************************************/
/* Includes and other common stuff for the MUI demo programs */
/*************************************************************/

#ifdef USE_ZUNE
#ifndef __AROS__
#define USE_ZUNE_ON_AMIGA
#endif
#endif

#define MUI_OBSOLETE

/* MUI */
#ifndef USE_ZUNE_ON_AMIGA
#include <libraries/mui.h>
#else
#include <mui.h>
#endif

/* System */
#include <dos/dos.h>
#include <graphics/gfxmacros.h>
#include <workbench/workbench.h>
#include <exec/memory.h>

/* Prototypes */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/asl_protos.h>

#include <proto/muimaster.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __AROS__
#include <aros/oldprograms.h>
#include <datatypes/pictureclass.h>
#include <workbench/startup.h>
#include <libraries/asl.h>
#define strcmpi strcmp
#endif

/* Compiler specific stuff */
#include <SDI/SDI_compiler.h>
#include <SDI/SDI_hook.h>

#ifndef _DCC
#ifdef __SASC
#include <pragmas/exec_sysbase_pragmas.h>
#else
#ifndef __GNUC__
#include <pragmas/exec_pragmas.h>
#endif /* ifndef __GNUC__ */
#endif /* ifdef SASC      */

#ifndef __GNUC__

#include <pragmas/datatypes_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/asl_pragmas.h>

#ifndef DATATYPES_PICTURECLASS_H
#include <datatypes/pictureclass.h>
#endif

#endif /* ifndef __GNUC__ */

#ifndef __AROS__
extern struct ExecBase *SysBase;
extern struct Library *IntuitionBase, *UtilityBase, *GfxBase, *DOSBase, *IconBase,
                      *DataTypesBase, *MUIMasterBase;
#endif
#endif /* ifdef _DCC */


#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


/*
    base tag value for programs custom classes
*/
#define MUIMINE_TAG_BASE        (TAG_USER | (123 << 16))

#define MFWINDOW_TAG_BASE       (MUIMINE_TAG_BASE | 0x1000)
#define BTWINDOW_TAG_BASE       (MUIMINE_TAG_BASE | 0x2000)
#define LEWINDOW_TAG_BASE       (MUIMINE_TAG_BASE | 0x3000)
#define ISWINDOW_TAG_BASE       (MUIMINE_TAG_BASE | 0x4000)
#define MINEFIELD_TAG_BASE      (MUIMINE_TAG_BASE | 0x5000)
#define FACEBUTTON_TAG_BASE     (MUIMINE_TAG_BASE | 0x6000)
#define DIGITS_TAG_BASE         (MUIMINE_TAG_BASE | 0x7000)



/****************************************************************************

    support functions and structures

****************************************************************************/

#ifdef __AROS__
#define __stdargs
#endif

IPTR __stdargs DoSuperNew(struct IClass *cl,Object *obj,IPTR tag1,...);

//
//  parameter structure for loading a bitmap from a file using datatypes
//
struct LoadBitMapData
{
    STRPTR                FileName;     // file spec. of picture file
    Object *              DTObject;     // datatype from picture file
    struct BitMapHeader * BitMapHeader; // bit map header from datatype
    struct BitMap *       BitMap;       // bit map from datatype
    struct Screen *       Screen;       // screen for datatype remapping
};

//
//  prototypes
//
int LoadBitMap(struct LoadBitMapData * data);

//
//  LoadBitMap() error codes
//
#define LBMERR_NONE         0   // no error
#define LBMERR_PARAM        1   // a parameter was NULL
#define LBMERR_ALLOC        2   // memory allocation failed
#define LBMERR_NOFILE       3   // could not find specified file
#define LBMERR_DTLOAD       4   // could not load file as picture datatype
#define LBMERR_LAYOUT       5   // datatype could not layout (remap) picture
#define LBMERR_BITMAPHEADER 6   // could not get bit map header from datatype
#define LBMERR_BITMAP       7   // could not get bit map from the datatype



#endif /* __MUIMINE_H_ */
