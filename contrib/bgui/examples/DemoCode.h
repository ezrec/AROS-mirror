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
 * Revision 42.8  2003/01/18 19:10:11  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.7  2000/08/10 17:53:19  stegerg
 * removed a debug output.
 *
 * Revision 42.6  2000/08/08 20:36:51  stegerg
 * UtilityBase on AROS is not of type struct Library *, but
 * struct UtilityBase *. BTW: Why????????? I think we should
 * fix this. Also locale.library seems to have this "problem".
 *
 * Revision 42.5  2000/08/08 19:29:54  chodorowski
 * Minor changes.
 *
 * Revision 42.4  2000/08/08 13:51:34  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.3  2000/08/07 21:49:06  stegerg
 * fixed/activated REGFUNC/REGPARAM macros.
 *
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
#include <stdio.h>

/*
 * The entry point of all demo programs.
 */
extern VOID StartDemo( void );

/*
 * Output file handle and BGUI
 * library base.
 */

BPTR   StdOut;

struct Library        *BGUIBase;

#ifdef __AROS__
struct IntuitionBase  *IntuitionBase;
struct GfxBase        *GfxBase;
struct UtilityBase    *UtilityBase;
#endif

/*
 * Output text to the CLI or Workbench console.
 */

VOID Tell( UBYTE *fstr, ... )
{
   if ( StdOut ) VFPrintf( StdOut, fstr, ( ULONG * )&fstr + 1 );
}

BOOL openlibs(void)
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary( "intuition.library", 0 );
  GfxBase       = (struct GfxBase *) OpenLibrary( "graphics.library", 0 );
  UtilityBase   = OpenLibrary( "utility.library", 0 );
  
  if (!IntuitionBase || !GfxBase || ! UtilityBase)
    return FALSE;
  
  return TRUE;
}

void closelibs(void)
{ 
  if (IntuitionBase) CloseLibrary((struct Library*)IntuitionBase);
  if (GfxBase) CloseLibrary((struct Library*)GfxBase);
  if (UtilityBase) CloseLibrary( UtilityBase );
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
