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
 * Revision 42.6  2003/01/18 19:10:05  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.5  2000/08/09 10:17:24  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.4  2000/08/08 19:29:08  chodorowski
 * Added #include <proto/alib.h> to fix some warning.
 *
 * Revision 42.3  2000/08/08 13:47:32  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros; now includes
 * compilerspecific.h where they are defined.
 *
 * Revision 42.2  2000/08/07 21:48:39  stegerg
 * fixed and activated REGFUNC/REGPARAM macros.
 *
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

#ifdef __AROS__
#include <proto/alib.h>
#else
#include <clib/alib_protos.h>
#endif

#include <string.h>
#include <ctype.h>

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
#endif

#define NAME      "CAD"

#ifdef __VBCC__
  #define __AMIGADATE__ " 17 March 1999 "
#endif

#define VERSTAG   "$VER: CAD 1.2 " __AMIGADATE__ " (C) Copyright 1996 Ian J. Einman & Jaba Development.\r\n"
#define VERSION   "1.2"

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

#endif /* CAD_H */
