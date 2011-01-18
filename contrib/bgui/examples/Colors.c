/*
 * @(#) $Header$
 *
 * Colors.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:19:35  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:25  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:45  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:01  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc Colors.c -mi -ms -mRR -proto -lbgui
quit
*/

#include "DemoCode.h"

/*
**      Object ID's.
**/
#define ID_ALT                  1
#define ID_QUIT                 2

VOID StartDemo( void )
{
        struct Window           *window;
        Object                  *WO_Window, *GO_Quit, *GO_B[ 2 ], *GO_Alt;
        ULONG                    signal, rc, tmp = 0, a;
        BOOL                     running = TRUE;

        /*
         *      Create the window object.
         */
        WO_Window = WindowObject,
                WINDOW_Title,           "Colors Demo",
                WINDOW_AutoAspect,      TRUE,
                WINDOW_SmartRefresh,    TRUE,
                WINDOW_RMBTrap,         TRUE,
                WINDOW_MasterGroup,
                        VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), GROUP_BackFill, SHINE_RASTER,
                                StartMember,
                                        InfoFixed( NULL, ISEQ_C "This small demo shows you how you can\n"
                                                         "change the background and label colors\n"
                                                         "of an object on the fly.\n\n"
                                                         "As you can see the colors of the below buttons\n"
                                                         "are normal but when the " ISEQ_B "Alternate" ISEQ_N " checkbox\n"
                                                         "is selected the colors are changed.",
                                                         NULL, 7 ),
                                EndMember,
                                StartMember,
                                        HGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), FRM_Type, FRTYPE_BUTTON, FRM_Recessed, TRUE,
                                                StartMember, GO_B[ 0 ] = Button( "Colors", 0 ), EndMember,
                                                StartMember, GO_B[ 1 ] = Button( "Demo",   0 ), EndMember,
                                        EndObject, FixMinHeight,
                                EndMember,
                                StartMember,
                                        HGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), FRM_Type, FRTYPE_BUTTON, FRM_Recessed, TRUE,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, GO_Alt = KeyCheckBox( "_Alternate", FALSE, ID_ALT ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                        EndObject, FixMinHeight,
                                EndMember,
                                StartMember,
                                        HGroupObject,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, GO_Quit = KeyButton( "_Quit", ID_QUIT ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                        EndObject, FixMinHeight,
                                EndMember,
                        EndObject,
        EndObject;

        /*
        **      Object created OK?
        **/
        if ( WO_Window ) {
                /*
                **      Assign the keys to the buttons.
                **/
                tmp += GadgetKey( WO_Window, GO_Quit,  "q" );
                tmp += GadgetKey( WO_Window, GO_Alt,   "a" );
                /*
                **      OK?
                **/
                if ( tmp == 2 ) {
                        /*
                        **      try to open the window.
                        **/
                        if ( window = WindowOpen( WO_Window )) {
                                /*
                                **      Obtain it's wait mask.
                                **/
                                GetAttr( WINDOW_SigMask, WO_Window, &signal );
                                /*
                                **      Event loop...
                                **/
                                do {
                                        Wait( signal );
                                        /*
                                        **      Handle events.
                                        **/
                                        while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
                                                /*
                                                **      Evaluate return code.
                                                **/
                                                switch ( rc ) {

                                                        case    WMHI_CLOSEWINDOW:
                                                        case    ID_QUIT:
                                                                running = FALSE;
                                                                break;

                                                        case    ID_ALT:
                                                                /*
                                                                 *      When the object is selected we use
                                                                 *      alternate coloring on the objects.
                                                                 *      If not we revert to default coloring.
                                                                 */
                                                                GetAttr( GA_Selected, GO_Alt, &tmp );
                                                                /*
                                                                 *      Setup the colors on the buttons.
                                                                 */
                                                                for ( a = 0; a < 2; a++ )
                                                                        SetGadgetAttrs(( struct Gadget * )GO_B[ a ], window, NULL,
                                                                                FRM_BackDriPen,                 tmp ? TEXTPEN  : (ULONG)~0,
                                                                                FRM_SelectedBackDriPen,         tmp ? SHINEPEN : (ULONG)~0,
                                                                                LAB_DriPen,                     tmp ? SHINEPEN : (ULONG)~0,
                                                                                LAB_SelectedDriPen,             tmp ? TEXTPEN  : (ULONG)~0,
                                                                                TAG_END );
                                                                break;
                                                }
                                        }
                                } while ( running );
                        } else
                                Tell( "Could not open the window\n" );
                } else
                        Tell( "Could not assign gadget keys\n" );
                /*
                **      Disposing of the window object will
                **      also close the window if it is
                **      already opened and it will dispose of
                **      all objects attached to it.
                **/
                DisposeObject( WO_Window );
        } else
                Tell( "Could not create the window object\n" );
}

