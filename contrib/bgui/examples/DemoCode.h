/*
 * @(#) $Header$
 *
 * DemoCode.h
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2000/07/04 05:02:22  bergers
 * Made examples compilable.
 *
 * Revision 42.1  2000/05/15 19:28:19  stegerg
 * REG() macro replacementes
 *
 * Revision 42.0  2000/05/09 22:19:38  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:28  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:47  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/02/19 05:03:50  mlemos
 * Added support to build with Storm C.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:49  mlemos
 * Ian sources
 *
 *
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <libraries/gadtools.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>
#include <intuition/sghooks.h>
#ifndef __STORM__
#include <graphics/gfxmacros.h>
#endif
#include <workbench/workbench.h>
#include <workbench/startup.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/bgui.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/utility.h>

#ifdef __STORM__
#include <graphics/gfxmacros.h>
#endif

#include <stdlib.h>

#ifdef _DCC
#define SAVEDS __geta4
#define ASM
#define REG(x) __ ## x
#define CHIP(t) __chip t
#elif __STORM__
#define CHIP(t) __chip t
#ifndef SAVEDS
#define SAVEDS  __saveds
#endif
#ifndef ASM
#define ASM
#endif
#ifndef REG
#define REG(x) register __ ## x
#endif
#else
#define SAVEDS __saveds
#define ASM __asm
#define REG(x) register __ ## x
#define CHIP(t) t chip
#endif

#ifdef _AROS

  #ifndef AROS_ASMCALL_H
  #include <aros/asmcall.h>
  #endif
  
  #undef ASM
  #define ASM
  #undef SAVEDS
  #define SAVEDS

#if 0
  #ifndef REGPARAM
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
  #endif
#else
  #ifndef REGPARAM
  #define REGPARAM(reg,type,name) type name
  
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
#endif

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

/*
 * The entry point of all demo programs.
 */
extern VOID StartDemo( void );

/*
 * Output file handle and BGUI
 * library base.
 */
BPTR     StdOut;
struct Library *BGUIBase;
struct IntuitionBase * IntuitionBase;
struct GfxBase * GfxBase;
struct UtilityBase * UtilityBase;
/*
 * Output text to the CLI or Workbench console.
 */
VOID Tell( UBYTE *fstr, ... )
{
   if ( StdOut ) VFPrintf( StdOut, fstr, ( ULONG * )&fstr + 1 );
}

BOOL openlibs(void)
{
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library",0);  
  
  if (!IntuitionBase || !GfxBase || ! UtilityBase)
    return FALSE;
  
  return TRUE;
}

void closelibs(void)
{ 
  if (IntuitionBase) CloseLibrary((struct Library*)IntuitionBase);
  if (GfxBase) CloseLibrary((struct Library*)GfxBase);
  if (UtilityBase) CloseLibrary((struct Library*)UtilityBase);
}

/*
 * Main entry point.
 */
int main( int argc, char **argv )
{
   struct Process       *this_task = ( struct Process * )FindTask( NULL );
   BOOL            is_wb = FALSE;

   if (FALSE == openlibs())
   {
     closelibs();
     return -1;
   }

   if ( this_task->pr_CLI )
      /*
       * Started from the CLI. Simply pickup
       * the CLI output handle.
       */
      StdOut = Output();
   else {
      /*
       * Workbench startup. Open a console
       * for output.
       */
      StdOut = Open( "CON:10/10/500/100/BGUI Demo Output/WAIT/AUTO", MODE_NEWFILE );
      is_wb = TRUE;
   }

   /*
    * Open BGUI.
    */
   if ( BGUIBase = OpenLibrary( BGUINAME, BGUIVERSION )) {
      /*
       * Run the demo.
       */
      StartDemo();
      CloseLibrary( BGUIBase );
   } else
      Tell( "Unable to open %s version %ld\n", BGUINAME, BGUIVERSION );

   /*
    * Close console if ran from
    * the workbench.
    */
   if ( is_wb ) {
      if ( StdOut ) Close( StdOut );
   }

   return( 0 );
}

/*
 * DICE stub which simply calls
 * main() when run from the
 * workbench.
 */
#if _DCC || __STORM__
int wbmain( struct WBStartup *wbs )
{
   return( main( NULL, 0 ));
}
#endif

#define FuzzButton(label,id) ButtonObject, LAB_Label, label, LAB_Underscore, '_',\
   GA_ID, id, FuzzButtonFrame, EndObject
