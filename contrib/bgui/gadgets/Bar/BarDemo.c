/*
 * @(#) $Header$
 *
 * BGUI library
 *
 * (C) Copyright 2000 BGUI Developers Team.
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2000/07/09 03:05:08  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.0  2000/05/09 22:20:33  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:22  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:27  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:02  mlemos
 * Initial revision.
 *
 *
 */

#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/bgui.h>
#include <clib/alib_protos.h>

#include <bgui/bgui_bar.h>

/*
 * Output file handle and BGUI
 * library base.
 */
BPTR     StdOut;
struct Library *BGUIBase;

struct IntuitionBase * IntuitionBase;

/*
** Notification map-lists.
**/
struct TagItem pga2slh[] = { { BAR_HorizontalPosition, BAR_HorizontalPosition, }, { TAG_END }  };
struct TagItem pga2slv[] = { { BAR_VerticalPosition,   BAR_VerticalPosition,   }, { TAG_END }  };
struct TagItem sl2prg1[] = { { BAR_HorizontalPosition, BAR_HorizontalSize,     }, { TAG_END }  };
struct TagItem prg12in[] = { { BAR_HorizontalSize,     INDIC_Level,            }, { TAG_END }  };
struct TagItem sl2prg2[] = { { BAR_VerticalPosition,   BAR_VerticalSize,       }, { TAG_END }  };
struct TagItem prg22in[] = { { BAR_VerticalSize,       INDIC_Level,            }, { TAG_END }  };

#define Tell(fstr, args...) do { \
	if ( StdOut ) FPrintf( StdOut, fstr ,##args); \
} while (0)

static Object *OpenMainWindow(struct MsgPort *SharedPort,struct Window **win)
{
   Object            *object=NULL, *p1, *p2, *s1, *s2, *p, *i1, *i2;

   /*
   ** Not created yet? Create it now!
   **/
   if ( ! object ) {
      object = WindowObject,
         WINDOW_Title,     "BGUI Bar demo",
         WINDOW_RMBTrap,         TRUE,
         WINDOW_SmartRefresh, TRUE,
         WINDOW_AutoAspect,   TRUE,
         WINDOW_SharedPort,   SharedPort,
         WINDOW_MasterGroup,
            VGroupObject, NormalOffset, NormalSpacing,
               StartMember,
                  HGroupObject, NormalSpacing,
                     StartMember,
                        VGroupObject, NormalSpacing,
                           StartMember, i1 = IndicatorFormat( 0, 100, 0, IDJ_CENTER, "%ld%%" ), FixMinHeight, EndMember,
                           StartMember,
                             p1=BGUI_NewObject(BGUI_BAR_GADGET,
                             	BAR_Horizontal,FALSE,
                             	BAR_Vertical,FALSE,
                             	BAR_HorizontalPosition,0,
                             	BAR_HorizontalSize,0,
                             	BAR_HorizontalTotal,100,
                             	TAG_END),
                           EndMember,
                           StartMember,
                              s1=BGUI_NewObject(BGUI_BAR_GADGET,
                              	BAR_Horizontal,TRUE,
                              	BAR_Vertical,FALSE,
                              	BAR_HorizontalPosition,0,
                              	BAR_HorizontalSize,1,
                              	BAR_HorizontalTotal,101,
                              	TAG_END),
                              FixHeight( 16 ),
                           EndMember,
                        EndObject,
                     EndMember,
                     StartMember,
                        p=BGUI_NewObject(BGUI_BAR_GADGET,
                        	BAR_Horizontal,TRUE,
                        	BAR_Vertical,TRUE,
                        	BAR_HorizontalPosition,0,
                        	BAR_HorizontalSize,1,
                        	BAR_HorizontalTotal,101,
                        	BAR_VerticalPosition,0,
                        	BAR_VerticalSize,1,
                        	BAR_VerticalTotal,101,
                        	TAG_END),
                     EndMember,
                     StartMember,
                        s2=BGUI_NewObject(BGUI_BAR_GADGET,
                        	BAR_Horizontal,FALSE,
                        	BAR_Vertical,TRUE,
                        	BAR_VerticalPosition,0,
                        	BAR_VerticalSize,1,
                        	BAR_VerticalTotal,101,
                        	TAG_END),
                        FixWidth( 16 ),
                     EndMember,
                     StartMember,
                        VGroupObject, NormalSpacing,
                           StartMember, i2 = IndicatorFormat( 0, 100, 0, IDJ_CENTER, "%ld%%" ), FixMinHeight, EndMember,
                           StartMember,
                             p2=BGUI_NewObject(BGUI_BAR_GADGET,
                             	BAR_Horizontal,FALSE,
                             	BAR_Vertical,FALSE,
                             	BAR_VerticalPosition,0,
                             	BAR_VerticalSize,0,
                             	BAR_VerticalTotal,100,
                             	TAG_END),
                           EndMember,
                        EndObject,
                     EndMember,
                  EndObject,
               EndMember,
            EndObject,
      EndObject;

      if ( object ) {
         /*
         ** Connect sliders, prop, progression and indicators.
         **/
         AddMap( s1, p1, sl2prg1 );
         AddMap( s2, p2, sl2prg2 );
         AddMap( p,  s1, pga2slh );
         AddMap( p,  s2, pga2slv );
         AddMap( p1, i1, prg12in );
         AddMap( p2, i2, prg22in );
      }
   }


   /*
   ** Object OK?
   **/
   if ( object ) {
      /*
      ** Open window.
      **/
      if((*win=WindowOpen( object ))==NULL)
      {
      	DisposeObject(object);
      	object=NULL;
      }
   }

   return( object );
}

static VOID StartDemo(void)
{
   Object         *WA_Main;
   struct MsgPort *SharedPort;
   ULONG       sigmask = 0L, sigrec,rc;
   struct Window         *win=NULL, *sigwin;
   BOOL        running = TRUE;

   /*
   ** Create the shared message port.
   **/
   if (( SharedPort = CreateMsgPort())) {
      /*
      ** Open the main window.
      **/
      if (( WA_Main = OpenMainWindow(SharedPort,&win))) {
         /*
         ** OR signal masks.
         **/
         sigmask |= ( 1L << SharedPort->mp_SigBit );
         /*
         ** Loop...
         **/
         do {
            /*
            ** Wait for the signals to come.
            **/
            sigrec = Wait( sigmask );

            /*
            ** Find out the which window signalled us.
            **/
            if ( sigrec & ( 1 << SharedPort->mp_SigBit )) {
               while (( sigwin = GetSignalWindow( WA_Main ))) {

                  /*
                  ** Main window signal?
                  **/
                  if ( sigwin == win ) {
                     /*
                     ** Call the main-window event handler.
                     **/
                     while (( rc = HandleEvent( WA_Main )) != WMHI_NOMORE ) {
                        switch ( rc ) {

                           case  WMHI_CLOSEWINDOW:
                              running = FALSE;
                              break;

                        }
                     }
                  }
               }
            }
         } while ( running );
      }
      /*
      ** Dispose of all window objects.
      **/
      if (WA_Main)   DisposeObject(WA_Main);
      /*
      ** Delete the shared message port.
      **/
      DeleteMsgPort( SharedPort );
   } else
      Tell( "Unable to create a message port.\n" );
}

/*
 * Main entry point.
 */
int main( int argc, char **argv )
{
   struct Process       *this_task = ( struct Process * )FindTask( NULL );
   BOOL            is_wb = FALSE;

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

   if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
     return -1;
   
   /*
    * Open BGUI.
    */
   if (( BGUIBase = OpenLibrary( BGUINAME, BGUIVERSION ))) {
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

   CloseLibrary((struct Library *)IntuitionBase);

   return( 0 );
}

