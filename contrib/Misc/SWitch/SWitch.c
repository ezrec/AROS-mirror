/*
 * SWitch      (C) 1996  John M Haubrich Jr
 *                       All Rights Under Copyright Reserved
 *
 * Version 1.0
 *
 * Written using SAS/C 6.56
 *
 * PROGRAM AND SOURCE CODE ARE FREELY (RE)DISTRIBUTABLE.
 *
 * By John M. Haubrich, Jr.
 */

#include <aros/oldprograms.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include <exec/libraries.h>
#include <libraries/commodities.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <dos/dos.h>
#include <dos/rdargs.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <proto/commodities.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef LATTICE
int CXBRK(void) { return(0); }
int chkabort(void) { return(0); }
#endif

/*
 * VERSION STRING ===================================
 */
char *vers="$VER: SWitch 1.0 (15.05.96)";


/*
 * DEFINES ------------------------------------------
 */
#define BASENAME              "SWitch"
#define EVT_HOTKEY            1L
#define GAD_LISTVIEW          101L
#define ID_MENU_HIDE          201L
#define ID_MENU_QUIT          202L
#define ID_MENU_SAVESETTINGS  203L

#define MAXINFOCHARS          128   // 'Information...' allows 128 chars max
                                    // in string gadget.


/*
 * STRUCTURE DEFINITIONS ----------------------------
 */
struct NewBroker newbroker = {
   NB_VERSION,
   "SWitch",
   "Pops up a screen/window list.",
   "User selects which to activate.",
   NBU_UNIQUE | NBU_NOTIFY,
   0, 0, NULL, 0
};

struct MyGlobals {
   struct MsgPort *broker_mp;
   CxObj          *broker;
   ULONG           cxsigflag;  // our combined message port signal FLAG

   BOOL            fRunFromWB;
   BOOL            fBeQuiet;

   struct Gadget  *listview;
   struct List     list;
   UWORD           uwNumItems;

   char            szHotkeyDesc[MAXINFOCHARS];
   WORD            wLeftEdge;
   WORD            wTopEdge;
   WORD            wWidth;
   WORD            wHeight;
   BOOL            fPointerRel;
   SHORT           sJumpClicks;
};

struct SWNode
{
   struct Node    node;
   struct Screen *scr;
   struct Window *wdw;
};


/*
 * GLOBAL VARIABLES ---------------------------------
 */
struct MyGlobals   MG;
struct Gadget     *glist = NULL;
struct Window     *mywin = NULL;
struct Menu       *menu  = NULL;
void              *vi    = NULL;

struct NewMenu mynewmenu[] =
{
   { NM_TITLE, "Project",       0 , 0, 0, NULL },
   {  NM_ITEM, "Hide",         "H", 0, 0, (APTR)ID_MENU_HIDE },
   {  NM_ITEM, "Quit",         "Q", 0, 0, (APTR)ID_MENU_QUIT },

//   { NM_TITLE, "Prefs",         0 , 0, 0, NULL },
//   {  NM_ITEM, "Save Settings", 0 , 0, 0, (APTR)ID_MENU_SAVESETTINGS },
//
   {   NM_END, NULL,            0 , 0, 0, NULL }
};


/*
 * FUNCTION PROTOTYPES ------------------------------
 */
void main( int, char ** );
void ProcessMsg(void);
int  JMH_EasyReq( char *, char *, char *);
void JMH_Complain(char *);
void JMH_Info(char *);
int  JMH_Query(char *, char *);
void JMH_Quit(char *);
VOID JMH_InitVars(void);
void JumpToIt( UWORD, struct Screen * );
BOOL HandleEvents( void );
BOOL fnPopupJumpList( void );
struct Gadget *CreateAllGadgets( struct Gadget **, void *, WORD, WORD, WORD, WORD );


/*
 * LIBRARY VARIABLES --------------------------------
 */
struct Library       *CxBase        = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct IconBase      *IconBase      = NULL;
struct DosBase       *DosBase       = NULL;
struct GfxBase       *GfxBase       = NULL;
struct Library       *GadToolsBase  = NULL;


/*
 * FUNCTIONS ----------------------------------------
 */

/*
 * JMH_EasyReq(): pop up JMH_EasyRequest() with a custom title bar, message and
 *            response buttons.
 -------------------------------------------------------------------------*/
int JMH_EasyReq( char *title, char *text, char *resp ) {
   static struct EasyStruct es = {
      sizeof(struct EasyStruct),
      NULL,
      NULL,
      NULL,
      NULL
   };
   int stat;


   if ( MG.fBeQuiet == FALSE )
   {
      if ( MG.fRunFromWB == TRUE )
      {
         es.es_Title = title;
         es.es_TextFormat = text;
         es.es_GadgetFormat = resp;

         stat = EasyRequest(NULL, &es, NULL, TAG_DONE);
      }
      else
      {
         printf( "%s\n", text );
         stat = -1;
      }
   }
   return( stat );
}

/*
 * JMH_Complain(): pop up JMH_EasyRequest() with a JMH_Complaint message
 -------------------------------------------------------------------------*/
void JMH_Complain(char *t)
{
   char buf[100];


   if (t) {
      sprintf( buf, "%s Complaint...", BASENAME );
      JMH_EasyReq( buf, t, "OK" );
   }
}

/*
 * JMH_Info(): pop up JMH_EasyRequest() with a (nice) informative message
 *             if WB flag set else send output to stderr
 -------------------------------------------------------------------------*/
void JMH_Info(char *t)
{
   char buf[100];


   if (t) {
      sprintf( buf, "%s Information...", BASENAME );
      JMH_EasyReq( buf, t, "OK" );
   }
}

/*
 * JMH_Query(): are you sure... ? (1=yes, 0=no)
 -------------------------------------------------------------------------*/
int JMH_Query(char *t, char *response) {
   char  buf[100];
   char *resp;
   int   stat;


   if (t) {
      sprintf( buf, "%s Request...", BASENAME );

      if (response)
         resp = response;
      else
         resp = "Yes|No";
      stat = JMH_EasyReq( buf, t, resp );
      return(stat);
   }
}

/*
 * JMH_Quit(): close down everything we've opened and JMH_Quit
 -------------------------------------------------------------------------*/
void JMH_Quit( char *t )
{
   char  szQuitBuf[256+20];


   if ( mywin )
   {
      ClearMenuStrip( mywin );
      CloseWindow( mywin );
      mywin = NULL;
   }

   if ( t )
   {
      sprintf( szQuitBuf, "%s\nExiting...", t );
      JMH_Complain( szQuitBuf );
   }

   if ( MG.broker )     DeleteCxObjAll( MG.broker );
   if ( MG.broker_mp )  DeletePort( MG.broker_mp );

   if ( GfxBase )       CloseLibrary( (struct Library *)GfxBase );
   if ( GadToolsBase )
   {
      if ( menu )    FreeMenus( menu );

      //
      // Free list nodes, if any
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if ( MG.list.lh_Head->ln_Succ )
      {
         struct SWNode *swworknode, *swnode;


         swworknode = (struct SWNode *)MG.list.lh_Head;
         while ( swnode = (struct SWNode *)( swworknode->node.ln_Succ ) )
         {
            FreeVec( swworknode );
            swworknode = swnode;
         }
         NewList( &MG.list );
      }

      if ( vi )
      {
         FreeVisualInfo( vi );
         vi = NULL;
      }
      if ( glist )
      {
         FreeGadgets( glist );
         glist = NULL;
      }
      CloseLibrary(GadToolsBase);
   }
   if ( DosBase )         CloseLibrary( (struct Library *)DosBase );
   if ( CxBase )          CloseLibrary( CxBase );
   if ( IconBase )        CloseLibrary( (struct Library *)IconBase );
   if ( IntuitionBase )   CloseLibrary( (struct Library *)IntuitionBase );

   exit( ( t ? TRUE : FALSE ) );
}


/*
 * JMH_InitVars(): initialize variables to default values
 -------------------------------------------------------------------------*/
VOID JMH_InitVars()
{
   //
   // initialize variables...
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   memset( &MG, 0, sizeof( MG ) );

   MG.broker     = NULL;
   MG.broker_mp  = NULL;
   MG.fRunFromWB = TRUE;
   MG.fBeQuiet   = FALSE;

   NewList( &MG.list );


   menu = CreateMenus( mynewmenu, TAG_DONE );
}


/**
 ** M A I N   P R O G R A M   -----------------------
 **/
void main( int argc, char *argv[] )
{
   UBYTE            **ttypes,
                     *temp;
   CxMsg             *msg;
   CxObj             *filter;
   LONG               errorcode;


   //
   // open libraries
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   IntuitionBase = (struct IntuitionBase *)
      OpenLibrary("intuition.library",37L);
   if (!IntuitionBase) JMH_Quit("Cannot open intuition.library 37+");

   IconBase = (struct IconBase *)OpenLibrary("icon.library",37L);
   if (!IconBase) JMH_Quit("Cannot open icon.library 37+");

   CxBase = OpenLibrary("commodities.library",37L);
   if (!CxBase)   JMH_Quit("Cannot open commodities.library 37+");

   DosBase = (struct DosBase *)OpenLibrary("dos.library",37L);
   if (!DosBase)   JMH_Quit("Cannot open dos.library 37+");

   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",37L);
   if (!GfxBase)  JMH_Quit("Cannot open graphics.library 37+");

   GadToolsBase = OpenLibrary("gadtools.library",37L);
   if (!GadToolsBase)   JMH_Quit("Cannot open gadtools.library 37+");

   //
   // initialize variables
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   JMH_InitVars();

   if (!(MG.broker_mp = CreateMsgPort()))
      JMH_Quit( "Cannot create broker message port." );

   //
   // user may only run from Workbench
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   MG.fRunFromWB = TRUE;

   // get opts from TOOLTYPES in icon
   ttypes = ArgArrayInit( argc, argv );

   // user-specified priority for the commodity
   newbroker.nb_Pri = (BYTE)ArgInt( ttypes, "CX_PRIORITY", 0 );

   // report errors?
   temp = ArgString( ttypes, "QUIET", "FALSE" );
   if ( stricmp( temp, "TRUE" ) == 0 )
   {
      MG.fBeQuiet = TRUE;
   }
   else
   {
      MG.fBeQuiet = FALSE;
   }

   // read hotkey
   temp = ArgString( ttypes, "HOTKEY", "rawkey lalt esc" );
   strcpy( MG.szHotkeyDesc, temp );

   // read left/top/width/height
   temp = ArgString( ttypes, "LEFTEDGE", "0"   );   MG.wLeftEdge = atoi( temp );
   temp = ArgString( ttypes, "TOPEDGE" , "20"  );   MG.wTopEdge  = atoi( temp );
   temp = ArgString( ttypes, "WIDTH"   , "300" );   MG.wWidth    = atoi( temp );
      MG.wWidth = ( MG.wWidth < 100 ? 100 : MG.wWidth );
   temp = ArgString( ttypes, "HEIGHT"  , "150" );   MG.wHeight   = atoi( temp );
      MG.wHeight = ( MG.wHeight < 80 ? 80 : MG.wHeight );

   // popup jump window relative to mouse pointer?
   temp = ArgString( ttypes, "POINTERRELATIVE", NULL );
   if ( temp )
      MG.fPointerRel = TRUE;
   else
      MG.fPointerRel = FALSE;

   // read number of clicks to jump
   temp = ArgString( ttypes, "JUMPCLICKS", "1"  );
   MG.sJumpClicks = atoi( temp );
   if      ( MG.sJumpClicks < 1 )   MG.sJumpClicks = 1;
   else if ( MG.sJumpClicks > 2 )   MG.sJumpClicks = 2;

   ArgArrayDone();

   newbroker.nb_Port = MG.broker_mp;
   MG.cxsigflag = 1L << MG.broker_mp->mp_SigBit;

   if ( MG.broker = CxBroker(&newbroker, &errorcode) )
   {
      //
      // Establish hotkey filter
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if ( filter = HotKey( MG.szHotkeyDesc, MG.broker_mp, EVT_HOTKEY ) )
      {
         AttachCxObj( MG.broker, filter );
      }
      if ( CxObjError( filter ) )
      {
         JMH_Quit( "Error creating filter for hotkey." );
      }
      else
      {
         ActivateCxObj( MG.broker, TRUE );
         ProcessMsg();  // process msgs until user quits...
      }
   }
   else
   {
      //
      // an error has occured.. show user!
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      switch ( errorcode )
      {
         case CBERR_SYSERR:
            JMH_Quit( "Broker SYSTEM error.\nNo memory?" );
            break;
         case CBERR_DUP:
            //
            // this message is shown in ProcessMsg(): 'already running...'
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            break;
         case CBERR_VERSION:
            JMH_Quit( "Broker VERSION error." );
            break;
      }
   }

   //
   // Empty message port of all remaining messages
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   while ( msg = (CxMsg *)GetMsg( MG.broker_mp ) )
      ReplyMsg( (struct Message *)msg );

   JMH_Quit( NULL );
}

void ProcessMsg()
{
   CxMsg  *msg;
   ULONG   sigrcvd;
   ULONG   msgid;
   ULONG   msgtype;
   BOOL    fKeepGoing = TRUE;


   while ( fKeepGoing == TRUE )
   {
      sigrcvd = Wait( SIGBREAKF_CTRL_C | MG.cxsigflag );

      while ( msg = (CxMsg *)GetMsg( MG.broker_mp ) )
      {
         msgid = CxMsgID( msg );
         msgtype = CxMsgType( msg );
         ReplyMsg( (struct Message *)msg );

         switch ( msgtype )
         {
            case CXM_IEVENT:
               if ( msgid == EVT_HOTKEY )
               {
                  ActivateCxObj( MG.broker, FALSE );
                  fKeepGoing = ( fnPopupJumpList() ? FALSE : TRUE );
                  ActivateCxObj( MG.broker, TRUE );
               }
               break;

            case CXM_COMMAND:
               switch ( msgid )
               {
                  case CXCMD_DISABLE:
                     ActivateCxObj( MG.broker, FALSE );
                     break;

                  case CXCMD_ENABLE:
                     ActivateCxObj( MG.broker, TRUE );
                     break;

                  case CXCMD_KILL:
                     fKeepGoing = FALSE;
                     break;

                  case CXCMD_UNIQUE:
                     JMH_Complain( "SWitch is already running!" );
                     break;
               }
               break;
         }
         if ( fKeepGoing == FALSE ) break;
      }
      if ( sigrcvd & SIGBREAKF_CTRL_C )
      {
         fKeepGoing = FALSE;
      }
   }
}

// -----------------------------------------------------------------------
// fnPopupJumpList(): prepare jump list and launch window
// -----------------------------------------------------------------------
BOOL fnPopupJumpList( void )
{
   BOOL               fQuitCX = FALSE;    // FALSE = keep going; TRUE = quit program
   struct Screen     *scr;
   struct Window     *wdw;
   char              *pszTitle;
   UBYTE              szTitle[80];
   struct SWNode     *swnode;
   struct SWNode     *swworknode;
   LONG               lIBLock;

   //
   // Free list nodes, if any
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   if ( MG.list.lh_Head->ln_Succ )
   {
      swworknode = (struct SWNode *)MG.list.lh_Head;
      while ( swnode = (struct SWNode *)( swworknode->node.ln_Succ ) )
      {
         FreeVec( swworknode );
         swworknode = swnode;
      }
      NewList( &MG.list );
   }

   //
   // Fill the screen/window list
   // scan screenlist...
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   lIBLock = LockIBase(0);
   MG.uwNumItems = 0;
   for ( scr = IntuitionBase->FirstScreen; scr; scr = scr->NextScreen )
   {
      // use Default Title if there's no on-screen title
      if ( scr->Title == NULL )  pszTitle = scr->DefaultTitle;
      else                       pszTitle = scr->Title;

      swnode = (struct SWNode *)AllocVec( sizeof( struct SWNode ) + strlen( pszTitle ) + 1, MEMF_PUBLIC | MEMF_CLEAR );
      if ( swnode )
      {
         swnode->scr = scr;
         swnode->wdw = NULL;              // no window yet...
         pszTitle = ( pszTitle ? pszTitle : "Unnamed Screen" );
         strcpy( (char *)( swnode + 1 ), pszTitle );  // copy screen name to ln_Name
         swnode->node.ln_Name = (char *)( swnode + 1 );
         AddTail( &MG.list, (struct Node *)swnode );
         MG.uwNumItems++;
      }
 
      //
      // ...then windowlist
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      for ( wdw = scr->FirstWindow; wdw; wdw = wdw->NextWindow )
      {
         if ( wdw->Title )
         {
            pszTitle = wdw->Title;
         }
         else
         {
            sprintf( szTitle, "Unnamed Window at (%d,%d)", wdw->LeftEdge, wdw->TopEdge );
            pszTitle = szTitle;
         }
         swnode = (struct SWNode *)AllocVec( sizeof( struct SWNode ) + 3 + strlen( pszTitle ) + 1, MEMF_PUBLIC | MEMF_CLEAR );
         if ( swnode )
         {
            swnode->scr = scr;
            swnode->wdw = wdw;
            sprintf( (char *)( swnode + 1 ), "   %s", pszTitle );  // copy window name to ln_Name (indented)
            swnode->node.ln_Name = (char *)( swnode + 1 );
            AddTail( &MG.list, (struct Node *)swnode );
            MG.uwNumItems++;
         }
      }
   }
   UnlockIBase( lIBLock );

   //
   // Open the window and drop into the window event loop
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   fQuitCX = HandleEvents();

   return( fQuitCX );
}


void JumpToIt( UWORD imsgCode, struct Screen *currscr )
{
   struct Window *wdw;
   struct SWNode *swnode;
   int            i;


   //
   // imsgCode contains index of selection
   // walk thru LL for nth item
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   for ( swnode = (struct SWNode *)MG.list.lh_Head, i = 0;
         i < imsgCode;
         i++, swnode = (struct SWNode *)swnode->node.ln_Succ );

   // jump to screen/window
   ScreenToFront( swnode->scr );
   wdw = ( swnode->wdw ? swnode->wdw : swnode->scr->FirstWindow );
   WindowToFront( wdw );
   ActivateWindow( wdw );
   if ( IntuitionBase->LibNode.lib_Version >= 39 )
   {
      ScreenPosition( currscr,
                      SPOS_MAKEVISIBLE | SPOS_FORCEDRAG,
                      wdw->LeftEdge,
                      wdw->TopEdge,
                      wdw->LeftEdge + wdw->Width,
                      wdw->TopEdge  + wdw->Height );
   }
}


BOOL HandleEvents( void )
{
   BOOL                 fQuitCX = FALSE;   // FALSE = keep going; TRUE = quit program
   struct IntuiMessage *imsg;
   struct Gadget       *gad;
   struct Screen       *currscr = NULL;
   ULONG                imsgClass;
   UWORD                imsgCode;
   UWORD                imsgQual;
   UWORD                uwCode = 0xFFFF;
   UWORD                uwListItem = 0;
   ULONG                imsgSeconds = 0;
   ULONG                imsgMicros = 0;
   ULONG                ulSeconds = 0;
   ULONG                ulMicros = 0;
   WORD                 topborder;
   struct MenuItem     *mitem;
   WORD                 wLeftEdge;
   WORD                 wTopEdge;
   WORD                 wWidth;
   WORD                 wHeight;
   BOOL                 fTerminated = FALSE;
   BOOL                 fShift = FALSE;
   BOOL                 fAlt = FALSE;
   BOOL                 fFound;
   struct SWNode       *swnode;
   UWORD                i;


   currscr = IntuitionBase->ActiveScreen;

   if ( !( vi = GetVisualInfo( currscr, TAG_DONE ) ) )
   {
      // can't get visual info... bail out!
      DisplayBeep( NULL );
      goto endrtn;
   }

   //
   // How tall is window's title bar?
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   topborder = currscr->WBorTop + (currscr->Font->ta_YSize + 1);

   wWidth    = ( MG.wWidth    > currscr->Width  ? currscr->Width                        : MG.wWidth    );
   wHeight   = ( MG.wHeight   > currscr->Height ? currscr->Height                       : MG.wHeight   );
   wLeftEdge = ( MG.wLeftEdge > currscr->Width  ? currscr->Width  - wWidth              : MG.wLeftEdge );
   wTopEdge  = ( MG.wTopEdge  > currscr->Height ? currscr->Height - wHeight - topborder : MG.wTopEdge  );

   //
   // override left/top if POINTERRELATIVE mode is being used
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   if ( MG.fPointerRel )
   {
      wLeftEdge = currscr->MouseX;
      wTopEdge  = currscr->MouseY;
   }

   if ( !( mywin = OpenWindowTags( NULL,
                                   WA_CustomScreen , currscr,
                                   WA_Left         , wLeftEdge,
                                   WA_Top          , wTopEdge,
                                   WA_InnerWidth   , wWidth,
                                   WA_InnerHeight  , wHeight,
                                   WA_MinWidth     , 100,
                                   WA_MinHeight    , 80,
                                   WA_MaxWidth     , 32767,
                                   WA_MaxHeight    , 32767,
                                   WA_AutoAdjust   , TRUE,
                                   WA_Activate     , TRUE,
                                   WA_DragBar      , TRUE,
                                   WA_DepthGadget  , TRUE,
                                   WA_CloseGadget  , TRUE,
                                   WA_SizeGadget   , TRUE,
                                   WA_SizeBBottom  , TRUE,
                                   WA_SimpleRefresh, TRUE,
                                   WA_IDCMP        , CLOSEWINDOW | REFRESHWINDOW | MENUPICK | IDCMP_NEWSIZE |
                                                     IDCMP_RAWKEY | IDCMP_INACTIVEWINDOW | LISTVIEWIDCMP,
                                   WA_Title        , "SWitch List",
                                   TAG_DONE ) ) )
   {
      DisplayBeep( NULL );
      goto endrtn;
   }

   if ( !CreateAllGadgets( &glist, vi, mywin->BorderLeft, topborder,
                           mywin->Width  - mywin->BorderLeft - mywin->BorderRight,
                           mywin->Height - mywin->BorderTop  - mywin->BorderBottom ) )
   {
      DisplayBeep( NULL );
      goto endrtn;
   }
   AddGList( mywin, glist, -1, -1, NULL );
   RefreshGList( glist, mywin, NULL, -1 );
   GT_RefreshWindow( mywin, NULL );
   LayoutMenus( menu, vi,
                GTMN_NewLookMenus, TRUE,
                TAG_DONE );
   SetMenuStrip( mywin, menu );

   while ( !fTerminated )
   {
      Wait ( 1 << mywin->UserPort->mp_SigBit );
      while ( ( !fTerminated ) && ( imsg = GT_GetIMsg( mywin->UserPort ) ) )
      {
         imsgClass   = imsg->Class;
         imsgCode    = imsg->Code;
         imsgQual    = imsg->Qualifier;
         imsgSeconds = imsg->Seconds;
         imsgMicros  = imsg->Micros;
         gad = (struct Gadget *)imsg->IAddress;
         GT_ReplyIMsg( imsg );
         switch ( imsgClass )
         {
            case IDCMP_NEWSIZE:
               RemoveGList( mywin, glist, -1 );
               FreeGadgets( glist );
               glist = NULL;
               if ( !CreateAllGadgets( &glist, vi, mywin->BorderLeft, topborder,
                                       mywin->Width  - mywin->BorderLeft - mywin->BorderRight,
                                       mywin->Height - mywin->BorderTop  - mywin->BorderBottom ) )
               {
                  DisplayBeep( NULL );
               }
               else
               {
                  AddGList( mywin, glist, -1, -1, NULL );
                  SetRast( mywin->RPort, 0 );
                  RefreshWindowFrame( mywin );
                  RefreshGList( glist, mywin, NULL, -1 );
                  GT_RefreshWindow( mywin, NULL );
               }
               break;
            case IDCMP_RAWKEY:
               switch( imsgCode )
               {
                  case 0x60:
                  case 0x61:
                     // LEFT or RIGHT SHIFT pressed
                     fShift = TRUE;
                     break;
                  case 0xE0:
                  case 0xE1:
                     // LEFT or RIGHT SHIFT released
                     fShift = FALSE;
                     break;
                  case 0x64:
                  case 0x65:
                     // LEFT or RIGHT ALT pressed
                     fAlt = TRUE;
                     break;
                  case 0xE4:
                  case 0xE5:
                     // LEFT or RIGHT ALT released
                     fAlt = FALSE;
                     break;
                  case 0x43:
                  case 0x44:
                     // ENTER/RETURN
                     JumpToIt( uwListItem, currscr );

                     // close jump window and exit...
                     fTerminated = TRUE;
                     break;
                  case 0x45:
                     // ESC
                     if ( fAlt == FALSE  &&  fShift == FALSE )
                     {
                        fTerminated = TRUE;
                     }
                     break;
                  case 0x4C:
                     // Up cursor
                     if ( uwListItem <= 0 )     break;

                     if      ( fShift == TRUE )
                     {
                        for ( swnode = (struct SWNode *)MG.list.lh_Head, i = 0;
                              ( ( i < uwListItem-1 ) && ( swnode->node.ln_Succ ) );
                              i++, swnode = (struct SWNode *)swnode->node.ln_Succ );
                        for ( fFound = FALSE; swnode->node.ln_Pred; i--, swnode = (struct SWNode *)swnode->node.ln_Pred )
                        {
                           if ( swnode->wdw == NULL )
                           {
                              fFound = TRUE;
                              break;
                           }
                        }
                        if ( fFound )
                        {
                           uwListItem = i;
                           GT_SetGadgetAttrs( MG.listview, mywin, NULL, GTLV_Selected   , uwListItem,
                                                                        GTLV_MakeVisible, uwListItem,
                                                                        ( GadToolsBase->lib_Version < 39 ? GTLV_Top : TAG_SKIP ), uwListItem,
                                                                        TAG_DONE, TAG_DONE );
                        }
                     }
                     else if ( fAlt == TRUE )
                     {
                        uwListItem = 0;
                        GT_SetGadgetAttrs( MG.listview, mywin, NULL, GTLV_Selected   , uwListItem,
                                                                     GTLV_MakeVisible, uwListItem,
                                                                     ( GadToolsBase->lib_Version < 39 ? GTLV_Top : TAG_SKIP ), uwListItem,
                                                                     TAG_DONE, TAG_DONE );
                     }
                     else
                     {
                        uwListItem = ( uwListItem-1 >= 0 ? uwListItem-1 : 0 );
                        GT_SetGadgetAttrs( MG.listview, mywin, NULL, GTLV_Selected   , uwListItem,
                                                                     GTLV_MakeVisible, uwListItem,
                                                                     ( GadToolsBase->lib_Version < 39 ? GTLV_Top : TAG_SKIP ), uwListItem,
                                                                     TAG_DONE, TAG_DONE );
                     }
                     break;
                  case 0x4D:
                     // Down cursor
                     if ( uwListItem >= MG.uwNumItems-1 )     break;

                     if      ( fShift == TRUE )
                     {
                        for ( swnode = (struct SWNode *)MG.list.lh_Head, i = 0;
                              ( ( i < uwListItem+1 ) && ( swnode->node.ln_Succ ) );
                              i++, swnode = (struct SWNode *)swnode->node.ln_Succ );
                        for ( fFound = FALSE; swnode->node.ln_Succ; i++, swnode = (struct SWNode *)swnode->node.ln_Succ )
                        {
                           if ( swnode->wdw == NULL )
                           {
                              fFound = TRUE;
                              break;
                           }
                        }
                        if ( fFound )
                        {
                           uwListItem = i;
                           GT_SetGadgetAttrs( MG.listview, mywin, NULL, GTLV_Selected   , uwListItem,
                                                                        GTLV_MakeVisible, uwListItem,
                                                                        ( GadToolsBase->lib_Version < 39 ? GTLV_Top : TAG_SKIP ), uwListItem,
                                                                        TAG_DONE, TAG_DONE );
                        }
                     }
                     else if ( fAlt == TRUE )
                     {
                        uwListItem = MG.uwNumItems - 1;
                        GT_SetGadgetAttrs( MG.listview, mywin, NULL, GTLV_Selected   , uwListItem,
                                                                     GTLV_MakeVisible, uwListItem,
                                                                     ( GadToolsBase->lib_Version < 39 ? GTLV_Top : TAG_SKIP ), uwListItem,
                                                                     TAG_DONE, TAG_DONE );
                     }
                     else
                     {
                        uwListItem = ( uwListItem+1 < MG.uwNumItems ? uwListItem+1 : MG.uwNumItems-1 );
                        GT_SetGadgetAttrs( MG.listview, mywin, NULL, GTLV_Selected   , uwListItem,
                                                                     GTLV_MakeVisible, uwListItem,
                                                                     ( GadToolsBase->lib_Version < 39 ? GTLV_Top : TAG_SKIP ), uwListItem,
                                                                     TAG_DONE, TAG_DONE );
                     }
                     break;
               }
               break;

            case MENUPICK:
               while ( ( imsgCode != MENUNULL ) && ( !fTerminated ) )
               {
                  mitem = ItemAddress( menu, imsgCode );
                  switch( (ULONG)GTMENUITEM_USERDATA( mitem ) )
                  {
                     case ID_MENU_HIDE:
                        fTerminated = TRUE;
                        break;
                     case ID_MENU_QUIT:
                        fTerminated = TRUE;
                        fQuitCX     = TRUE;
                        break;
                     case ID_MENU_SAVESETTINGS:
                        // save settings here -- how replace existing tool types
                        break;
                  }
                  imsgCode = mitem->NextSelect;
               }
               break;

            case GADGETUP:
               if ( gad->GadgetID == GAD_LISTVIEW )
               {
                  uwListItem = imsgCode;
                  if ( ( MG.sJumpClicks == 1 ) ||
                       ( ( MG.sJumpClicks == 2 ) &&
                         DoubleClick( ulSeconds, ulMicros, imsgSeconds, imsgMicros ) &&
                         ( imsgCode == uwCode ) ) )
                  {
                     JumpToIt( imsgCode, currscr );

                     //
                     // close jump window and exit...
                     // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                     fTerminated = TRUE;
                  }
                  ulSeconds = imsgSeconds;
                  ulMicros  = imsgMicros;
                  uwCode    = imsgCode;
               }
               break;
            case CLOSEWINDOW:
            case IDCMP_INACTIVEWINDOW:
               fTerminated = TRUE;
               break;
            case REFRESHWINDOW:
               GT_BeginRefresh( mywin );
               GT_EndRefresh( mywin, TRUE );
               break;
         }
      }
   }

   //
   // Close the window and cleanup
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
endrtn:
   if ( mywin )
   {
      ClearMenuStrip( mywin );
      CloseWindow( mywin );
      mywin = NULL;
   }
   if ( vi )
   {
      FreeVisualInfo( vi );
      vi = NULL;
   }
   if ( glist )
   {
      FreeGadgets( glist );
      glist = NULL;
   }

   //
   // Send back return code
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   return( fQuitCX );
}

struct Gadget *CreateAllGadgets( struct Gadget **glistptr, void *vi,
                                 WORD wLeft, WORD topborder, WORD wWidth, WORD wHeight )
{
   struct NewGadget  ng;
   struct Gadget    *gad;


   memset( &ng, 0, sizeof( struct NewGadget ) );

   gad = CreateContext(glistptr);

   ng.ng_LeftEdge   = wLeft + 1;
   ng.ng_TopEdge    = topborder + 1;
   ng.ng_Width      = wWidth - 2;
   ng.ng_Height     = wHeight;
   ng.ng_TextAttr   = NULL;
   ng.ng_VisualInfo = vi;
   ng.ng_GadgetID   = GAD_LISTVIEW;
   ng.ng_Flags      = NULL;
   MG.listview = gad = CreateGadget( LISTVIEW_KIND, gad, &ng,
                                     GTLV_Labels      , &MG.list,
                                     GTLV_Selected    , 0,
                                     GTLV_ShowSelected, NULL,
                                     LAYOUTA_Spacing  , 2,
                                     TAG_DONE );

   return( gad );
}
