#ifndef CAD_H
#define CAD_H
/*
 * @(#) $Header$
 *
 * Change AutoDoc program
 * cad.h
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2000/05/15 19:28:19  stegerg
 * REG() macro replacementes
 *
 * Revision 42.0  2000/05/09 22:10:49  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:23:11  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:55:34  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.4  1999/08/21 19:26:17  mlemos
 * Integrated Anton Rolls changes to fix the command line argument template
 * handling and XML format support.
 *
 * Revision 1.1.2.3  1999/05/31 02:52:45  mlemos
 * Integrated Anton Rolls changes to generate documentation files in
 * AmigaGuide and HTML formats.
 *
 * Revision 1.1.2.2  1998/10/01 04:40:01  mlemos
 * Updated the prototypes of the functions changed in cad_file.c.
 *
 * Revision 1.1.2.1  1998/09/19 01:50:14  mlemos
 * Ian sources.
 *
 *
 *
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/iffparse.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/bgui.h>
#include <proto/utility.h>
#include <proto/locale.h>

#include <string.h>
#include <ctype.h>

#define NAME      "CAD"

#ifdef __VBCC__
  #define __AMIGADATE__ " 17 March 1999 "
#endif

#define VERSTAG   "$VER: CAD 1.2 " __AMIGADATE__ " (C) Copyright 1996 Ian J. Einman & Jaba Development.\r\n"
#define VERSION   "1.2"

#ifdef _DCC
#define SAVEDS __geta4
#define ASM
#define REG(x) __ ## x
#else
#define SAVEDS __saveds
#define ASM __asm
#define REG(x) register __ ## x
#endif

#ifdef __VBCC__
#undef SAVEDS
#define SAVEDS
#undef ASM
#define ASM
#undef REG
#define REG(x) __reg(#x)
#endif

#ifdef _AROS

  #ifndef AROS_ASMCALL_H
  #include <aros/asmcall.h>
  #endif
  
  #undef ASM
  #define ASM
  #undef SAVEDS
  #define SAVEDS

  #define REGPARAM(reg,type,name) AROS_UFHA(type, name, reg)
  
  #define REGFUNC1(r,n,a1) AROS_UFH1(r,n,a1)
  #define REGFUNC2(r,n,a1,a2) AROS_UFH2(r,n,a1,a2)
  #define REGFUNC3(r,n,a1,a2,a3) AROS_UFH3(r,n,a1,a2,a3)
  #define REGFUNC4(r,n,a1,a2,a3,a4) AROS_UFH4(r,n,a1,a2,a3,a4)
  #define REGFUNC5(r,n,a1,a2,a3,a4,a5) AROS_UFH5(r,n,a1,a2,a3,a4,a5)
  #define REGFUNC6(r,n,a1,a2,a3,a4,a5,a6) AROS_UFH6(r,n,a1,a2,a3,a4,a5,a6)
  #define REGFUNC7(r,n,a1,a2,a3,a4,a5,a6,a7) AROS_UFH7(r,n,a1,a2,a3,a4,a5,a6,a7)
  #define REGFUNC8(r,n,a1,a2,a3,a4,a5,a6,a7,a8) AROS_UFH8(r,n,a1,a2,a3,a4,a5,a6,a7,a8)
  #define REGFUNC9(r,n,a1,a2,a3,a4,a5,a6,a7,a8,a9) AROS_UFH9(r,n,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  
#else
  #define REGPARAM(reg,type,name) REG(reg) type name
  
  #define REGFUNC1(r,n,a1) r n(a1)
  #define REGFUNC2(r,n,a1,a2) r n(a1,a2)
  #define REGFUNC3(r,n,a1,a2,a3) r n(a1,a2,a3)
  #define REGFUNC4(r,n,a1,a2,a3,a4) r n(a1,a2,a3,a4)
  #define REGFUNC5(r,n,a1,a2,a3,a4,a5) r n(a1,a2,a3,a4,a5)
  #define REGFUNC6(r,n,a1,a2,a3,a4,a5,a6) r n(a1,a2,a3,a4,a5,a6)
  #define REGFUNC7(r,n,a1,a2,a3,a4,a5,a6,a7) r n(a1,a2,a3,a4,a5,a6,a7)
  #define REGFUNC8(r,n,a1,a2,a3,a4,a5,a6,a7,a8) r n(a1,a2,a3,a4,a5,a6,a7,a8)
  #define REGFUNC9(r,n,a1,a2,a3,a4,a5,a6,a7,a8,a9) r n(a1,a2,a3,a4,a5,a6,a7,a8,a9)
  
#endif

typedef struct CadNode {
   UBYTE        cn_Name[ 32 ];
   UBYTE       *cn_Data;
   ULONG        cn_DataLength;
} CADNODE;

/*
 * CADHEADER [ sizeof CADHEADER ]
 *    CADDESC [ sizeof CADDESC ]
 *       cd_Length bytes of data.
 *    CADDESC [ sizeof CADDESC ]
 *       cd_Length bytes of data.
 *    ...
 */
#define ID_CADF         MAKE_ID('C','A','D','F')
#define CADF_VERSION 1

typedef struct CadHeader {
   ULONG    ch_Ident;
   ULONG    ch_Version;
   UBYTE    ch_BaseName[ 64 ];
   ULONG    ch_NumDesc;
} CADHEADER;

typedef struct CadDesc {
   UBYTE    cd_Name[ 64 ];
   ULONG    cd_Length;
} CADDESC;


/* used by Match() and main() in cad_main.c
   and Generate() in cad_file.c
*/               

#define MATCH_FROMLIST 1
#define MATCH_TO       2
#define MATCH_DOC      3
#define MATCH_GUIDE    4
#define MATCH_HTML     5
#define MATCH_XML      6
#define MATCH_MAX      7

/* Prototypes. */
ULONG DosRequest( UBYTE *, UBYTE *, ... );
ULONG MyRequest( UBYTE *, UBYTE *, ... );
VOID MyFPrintf( BPTR, UBYTE *, ... );
VOID SetAttr( Object *, struct GadgetInfo *, Tag, ... );
VOID MultiSetAttr( struct GadgetInfo *, Tag, ULONG, Object *, ... );

/* ascii character 34 is the quote symbol  "   used mostly in GenerateGuide() etc. */
#define QUOTE "\""

BOOL SaveCadFile(BOOL window_opened);
BOOL LoadCadFile(BOOL window_opened);
BOOL Generate(BOOL window_opened, ULONG outputFormat);  /* This function calls one of these: */
BOOL GenerateDoc(BOOL window_opened);   /* generate text document */
BOOL GenerateHTML(BOOL window_opened);  /* generate HTML document - Anton */
BOOL GenerateXML(BOOL window_opened);   /* generate  XML document - Anton */
BOOL GenerateGuide(BOOL window_opened); /* generate AmigaGuide document - Anton */
UBYTE *MapSlash(UBYTE *source, UBYTE *destination); /* support function for GenerateGuide() */

#endif
