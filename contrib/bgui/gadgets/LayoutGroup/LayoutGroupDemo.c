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
 * Revision 42.0  2000/05/09 22:20:56  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:44  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:47  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  2000/05/05 18:08:02  mlemos
 * Changed the positions, spans and weights of tree nodes and arrows to make
 * them layout cleaner.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:39  mlemos
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

#include <bgui/bgui_layoutgroup.h>

/*
 * Output file handle and BGUI
 * library base.
 */
BPTR     StdOut;
struct Library *BGUIBase;

struct IntuitionBase * IntuitionBase;

VOID Tell( UBYTE *fstr, ... )
{
   if ( StdOut ) VFPrintf( StdOut, fstr, ( ULONG * )&fstr + 1 );
}

STATIC struct VectorItem left_down_arrow[] = {
   { 20, 20, VIF_SCALE                  },
   { 19,  0, VIF_MOVE | VIF_AREASTART   },
   {  1, 18, VIF_DRAW                   },
   {  0, 16, VIF_DRAW                   },
   {  0, 20, VIF_DRAW                   },
   {  4, 20, VIF_DRAW                   },
   {  2, 19, VIF_DRAW                   },
   { 20,  1, VIF_DRAW                   },
   { 20,  0, VIF_DRAW                   },
   { 19,  0, VIF_AREAEND | VIF_LASTITEM },
};

STATIC struct VectorItem right_down_arrow[] = {
   {  40, 40,  VIF_SCALE                  },
   {   2,  0,  VIF_MOVE | VIF_AREASTART   },
   {  38, 36,  VIF_DRAW                   },
   {  40, 32,  VIF_DRAW                   },
   {  40, 40,  VIF_DRAW                   },
   {  32, 40,  VIF_DRAW                   },
   {  36, 38,  VIF_DRAW                   },
   {   0,  2,  VIF_DRAW                   },
   {   0,  0,  VIF_DRAW                   },
   {   2,  0,  VIF_AREAEND | VIF_LASTITEM },
};

#define TreeNode(name,row,column) \
	ButtonObject, \
		LAB_Label, name, \
	EndObject, \
	LGNA_Row,row, \
	LGNA_Column,column, \
	LGNA_ColumnSpan,2, \
	LGNA_VerticalWeight,0, \
	LGNA_LeftWeight,0, \
	LGNA_WidthWeight,1, \
	LGNA_RightWeight,0, \
	LGNA_TopOffset,1, \
	LGNA_BottomOffset,1, \
	LGNA_TopSpacing,1, \
	LGNA_BottomSpacing,1

#define LeftArrow(row,column) \
	ButtonObject, \
		VIT_VectorArray, left_down_arrow, \
		NoFrame,  \
	EndObject, \
	LGNA_Row,row, \
	LGNA_Column,column, \
	LGNA_VerticalWeight,1, \
	LGNA_LeftWeight,0, \
	LGNA_WidthWeight,1, \
	LGNA_RightWeight,0, \
	LGNA_TopWeight,0, \
	LGNA_HeightWeight,1, \
	LGNA_BottomWeight,0, \
	LGNA_TopOffset,0, \
	LGNA_RightOffset,0, \
	LGNA_BottomOffset,0, \
	LGNA_TopSpacing,0, \
	LGNA_RightSpacing,0, \
	LGNA_BottomSpacing,0

#define RightArrow(row,column) \
	ButtonObject, \
		VIT_VectorArray, right_down_arrow, \
		NoFrame,  \
	EndObject, \
	LGNA_Row,row, \
	LGNA_Column,column, \
	LGNA_VerticalWeight,1, \
	LGNA_LeftWeight,0, \
	LGNA_WidthWeight,1, \
	LGNA_RightWeight,0, \
	LGNA_TopWeight,0, \
	LGNA_HeightWeight,1, \
	LGNA_BottomWeight,0, \
	LGNA_LeftOffset,0, \
	LGNA_TopOffset,0, \
	LGNA_BottomOffset,0, \
	LGNA_LeftSpacing,0, \
	LGNA_TopSpacing,0, \
	LGNA_BottomSpacing,0

static Object *OpenMainWindow(struct MsgPort *SharedPort,struct Window **main)
{
   Object            *object=NULL;

   /*
   ** Not created yet? Create it now!
   **/
   if ( ! object ) {
      object = WindowObject,
         WINDOW_Title,     "BGUI Layout Group demo",
         WINDOW_RMBTrap,         TRUE,
         WINDOW_SmartRefresh, TRUE,
         WINDOW_AutoAspect,   TRUE,
         WINDOW_SharedPort,   SharedPort,
         WINDOW_MasterGroup,
            BGUI_NewObject(BGUI_LAYOUTGROUP_GADGET,
               LGA_LayoutType,LGT_TABLE,
               FRM_Title,"Laying gadgets in a tree using the table layout",
               NeXTFrame,
               StartMember,
                  TreeNode("Root node",0,2),
               EndMember,
               StartMember,
                  LeftArrow(1,2),
               EndMember,
               StartMember,
                  RightArrow(1,3),
               EndMember,
               StartMember,
                  TreeNode("Left branch",2,1),
               EndMember,
               StartMember,
                  TreeNode("Right branch",2,3),
               EndMember,
               StartMember,
                  LeftArrow(3,1),
               EndMember,
               StartMember,
                  RightArrow(3,2),
               EndMember,
               StartMember,
                  LeftArrow(3,3),
               EndMember,
               StartMember,
                  RightArrow(3,4),
               EndMember,
               StartMember,
                  TreeNode("Left leaf",4,0),
               EndMember,
               StartMember,
                  TreeNode("Middle leaf",4,2),
               EndMember,
               StartMember,
                  TreeNode("Right leaf",4,4),
               EndMember,
            TAG_END),
      EndObject;

   }

   /*
   ** Object OK?
   **/
   if ( object ) {
      /*
      ** Open window.
      **/
      if((*main=WindowOpen( object ))==NULL)
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
   struct Window         *main=NULL, *sigwin;
   BOOL        running = TRUE;

   /*
   ** Create the shared message port.
   **/
   if ( SharedPort = CreateMsgPort()) {
      /*
      ** Open the main window.
      **/
      if ( WA_Main = OpenMainWindow(SharedPort,&main)) {
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
               while ( sigwin = GetSignalWindow( WA_Main )) {

                  /*
                  ** Main window signal?
                  **/
                  if ( sigwin == main ) {
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

   if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
     return -1;

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

   CloseLibrary((struct Library *)IntuitionBase);

   return( 0 );
}

