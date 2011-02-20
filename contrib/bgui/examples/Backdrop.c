/*
 * @(#) $Header$
 *
 * Backdrop.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:19:26  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:16  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:37  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:44:53  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc Backdrop.c -mi -ms -mRR -proto -lbgui
quit
*/

#include "DemoCode.h"
#include <graphics/displayinfo.h>

/*
 *      Quit object ID.
 */
#define ID_QUIT         1

/*
 *      A borderless window ;)
 */
VOID Say( struct Screen *screen )
{
        Object                  *WO_Window, *GO_Ok;
        ULONG                    signal, rc;
        BOOL                     running = TRUE;

        /*
         *      Create the window.
         */
        WO_Window = WindowObject,
                WINDOW_Borderless,      TRUE,
                WINDOW_SmartRefresh,    TRUE,
                WINDOW_RMBTrap,         TRUE,
                WINDOW_AutoAspect,      TRUE,
                WINDOW_Screen,          screen,
                WINDOW_MasterGroup,
                        VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 3 ),
                                FRM_Type,       FRTYPE_BUTTON,
                                FRM_BackFill,   SHINE_RASTER,
                                StartMember,
                                        InfoObject,
                                                FRM_Type,               FRTYPE_BUTTON,
                                                FRM_Recessed,           TRUE,
                                                INFO_TextFormat,        ISEQ_C "This demonstration shows you how to\n"
                                                                        "create a backdrop, borderless window with BGUI.\n\n"
                                                                        "You may recognize the GUI as the main window\n"
                                                                        "of SPOT but that's because I could not\n"
                                                                        "come up with something original.\n\n"
                                                                        "Just click on " ISEQ_B "Quit" ISEQ_N " to exit the demo.",
                                                INFO_FixTextWidth,      TRUE,
                                                INFO_MinLines,          8,
                                        EndObject,
                                EndMember,
                                StartMember,
                                        HGroupObject,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, GO_Ok = KeyButton( "_OK", ID_QUIT ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                        EndObject, FixMinHeight,
                                EndMember,
                        EndObject,
        EndObject;

        /*
         *      OK?
         */
        if ( WO_Window ) {
                /*
                 *      Add gadget key.
                 */
                if ( GadgetKey( WO_Window, GO_Ok, "o" )) {
                        /*
                         *      Open the window.
                         */
                        if ( WindowOpen( WO_Window )) {
                                GetAttr( WINDOW_SigMask, WO_Window, &signal );
                                /*
                                 *      Wait for somebody to click
                                 *      on the "OK" gadget.
                                 */
                                do {
                                        Wait( signal );
                                        while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
                                                if ( rc == ID_QUIT )
                                                        running = FALSE;
                                        }
                                } while ( running );
                        }
                }
                DisposeObject( WO_Window );
        }
}

/*
 *      Here we go...
 */
VOID StartDemo( void )
{
        struct Screen           *myscreen, *wblock;
        struct DrawInfo         *dri;
        struct Window           *window;
        Object                  *WO_Window;
        APTR                     wlock;
        ULONG                    mode, rc, signal;
        BOOL                     running = TRUE;

        /*
         *      Lock the workbench screen.
         */
        if (( wblock = LockPubScreen( "Workbench" ))) {
                /*
                 *      Obtain it's DrawInfo.
                 */
                if (( dri = GetScreenDrawInfo( wblock ))) {
                        /*
                         *      And mode ID.
                         */
                        if (( mode = GetVPModeID( &wblock->ViewPort )) != INVALID_ID ) {
                                /*
                                 *      Open a screen ala your workbench.
                                 */
                                if (( myscreen = OpenScreenTags( NULL, SA_Depth,         dri->dri_Depth,
                                                                      SA_Width,         wblock->Width,
                                                                      SA_Height,        wblock->Height,
                                                                      SA_DisplayID,     mode,
                                                                      SA_Pens,          dri->dri_Pens,
                                                                      SA_Title,         "Backdrop Demo.",
                                                                      TAG_END ))) {
                                        /*
                                         *      Create a simple backdrop window on
                                         *      the screen whilst keeping the screen
                                         *      title visible.
                                         */
                                        WO_Window = WindowObject,
                                                WINDOW_SmartRefresh,            TRUE,
                                                WINDOW_Backdrop,                TRUE,
                                                WINDOW_ShowTitle,               TRUE,
                                                WINDOW_CloseOnEsc,              TRUE,
                                                WINDOW_AutoAspect,              TRUE,
                                                WINDOW_NoBufferRP,              TRUE,
                                                WINDOW_Screen,                  myscreen,
                                                WINDOW_MasterGroup,
                                                        HGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
                                                                StartMember,
                                                                        VGroupObject, Spacing( 4 ),
                                                                                StartMember, Button( "Message list...", 0 ), EndMember,
                                                                                StartMember, Button( "Messages...",     0 ), EndMember,
                                                                                VarSpace( 40 ),
                                                                                StartMember, Button( "Import...",       0 ), EndMember,
                                                                                StartMember, Button( "Export...",       0 ), EndMember,
                                                                                StartMember, Button( "Maintenance...",  0 ), EndMember,
                                                                                StartMember, Button( "Optimize...",     0 ), EndMember,
                                                                                VarSpace( 40 ),
                                                                                StartMember, Button( "Poll...",         0 ), EndMember,
                                                                                StartMember, Button( "File request...", 0 ), EndMember,
                                                                                VarSpace( 40 ),
                                                                                StartMember, Button( "Iconify",         0 ), EndMember,
                                                                                StartMember, Button( "Quit",            ID_QUIT ), EndMember,
                                                                        EndObject, FixMinSize,
                                                                EndMember,
                                                                StartMember, ListviewObject, EndObject, EndMember,
                                                        EndObject,
                                        EndObject;

                                        /*
                                         *      Window OK?
                                         */
                                        if ( WO_Window ) {
                                                /*
                                                 *      Open the window.
                                                 */
                                                if (( window = WindowOpen( WO_Window ))) {
                                                        /*
                                                         *      Show an explanation window.
                                                         */
                                                        wlock = BGUI_LockWindow( window );
                                                        Say( myscreen );
                                                        BGUI_UnlockWindow( wlock );
                                                        /*
                                                         *      Pick up the window signal.
                                                         */
                                                        GetAttr( WINDOW_SigMask, WO_Window, &signal );
                                                        /*
                                                         *      Event loop...
                                                         */
                                                        do {
                                                                Wait( signal );
                                                                while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
                                                                        switch ( rc ) {

                                                                                case    WMHI_CLOSEWINDOW:
                                                                                case    ID_QUIT:
                                                                                        running = FALSE;
                                                                                        break;
                                                                        }
                                                                }
                                                        } while ( running );
                                                } else
                                                        Tell( "Unable to open the window\n" );
                                                DisposeObject( WO_Window );
                                        } else
                                                Tell( "Unable to create the window object\n" );
                                        CloseScreen( myscreen );
                                } else
                                        Tell( "Unable to open the screen\n" );
                        } else
                                Tell( "Unknown screen mode\n" );
                        FreeScreenDrawInfo( wblock, dri );
                } else
                        Tell( "Unable to get DrawInfo\n" );
                UnlockPubScreen( NULL, wblock );
        } else
                Tell( "Unable to lock the Workbench screen\n" );
}
