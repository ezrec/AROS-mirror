#include <aros/oldprograms.h>
/*
 *      ClickTest.c
 *
 *      compile & link with GoodDouble.o
 *
 *      ( or with GoodDouble2.o if you are NOT INTERESTED in MIDDLE
 *      button, but you MUST also #undefine ALL_BUTTONS. FORGET IT! )
 */

#include <exec/types.h>
#include <intuition/intuition.h>

#include "good_double.h"            /* macros & functions' prototypes */

#include <stdio.h>

/*
 *  they usually put something like that it ROM Kernel manuals...
 */
#ifdef LATTICE
    int CXBRK(void)     { return(0); }
    void chkabort(void) { return; }
#endif

/* something to brighten-up the output... */
#define ANSI_WHITE  "›2m"   /* sometimes it's white, sometimes not... */
#define ANSI_NORMAL "›0m"


struct IntuitionBase *IntuitionBase;

main()
{
    LONG ret_val = 5L;

    struct NewWindow TestWindow = {
        50, 50,                         /* left, top */
        450, 80,                        /* width, height */
        0, 1,                           /* detailpen, blockpen */
        ( CLOSEWINDOW | MOUSEBUTTONS | VANILLAKEY ),
        ( WINDOWCLOSE | WINDOWDRAG | NOCAREREFRESH | RMBTRAP ),
        NULL,                           /* no custom gadgets */
        NULL,                           /* no custom menu images */
        (UBYTE *)"Let's play -- HIT ME ME WITH EVERYTHING YOU GOT!",
        NULL,                           /* no custom screen structure */
        NULL,                           /* no superbitmap */
        0, 0, 0, 0,
        WBENCHSCREEN
    };

    struct Window       *testWnd;
    struct IntuiMessage *msg;

    ULONG  class;
    USHORT code;
    ULONG  secs, micros;

    BOOL fDone = FALSE;         /* the Hungarian naming convention! */

    /*
     *  this, as well as CloseLibrary(), wouldn't be needed for DICE
     */
    IntuitionBase =
            (struct IntuitionBase *)OpenLibrary( "intuition.library", 0L );

    if ( ! IntuitionBase )
        return( ret_val );

    /*
     * open our window
     */
    testWnd = (struct Window *)OpenWindow( &TestWindow );
    if ( ! testWnd )
        goto BAD_RETURN;

    while ( ! fDone ) {
        WaitPort( testWnd->UserPort );
        while ( msg = (struct IntuiMessage *)GetMsg( testWnd->UserPort ) ) {

            class       =   msg->Class;
            code        =   msg->Code;
            secs        =   msg->Seconds;
            micros      =   msg->Micros;

            ReplyMsg( (struct Message *)msg );

            switch ( class ) {
                case MOUSEBUTTONS:
                    switch ( code ) {
                        case MENUDOWN:
    /* ---> */              if ( RightDouble( secs, micros ) )
                                puts( "Second, quick one... It was "
                                    ANSI_WHITE "DOUBLE "
                                    ANSI_NORMAL "RIGHT!" );
                            else
                                puts( "Just a simple RIGHT..." );
                            break;
                        case MIDDLEDOWN:
    /* ---> */              if ( MiddleDouble( secs, micros ) )
                                puts( "Next! It was quick... So it was "
                                    ANSI_WHITE "DOUBLE "
                                    ANSI_NORMAL "MIDDLE!" );
                            else
                                puts( "Just a MIDDLE..." );
                            break;
                        case SELECTDOWN:
    /* ---> */              if ( LeftDouble( secs, micros ) )
                                puts( "And now a second... "
                                    ANSI_WHITE "DOUBLE "
                                    ANSI_NORMAL "LEFT then!" );
                            else
                                puts( "Just a straight LEFT..." );
                            break;
                    }
                    break;
                case VANILLAKEY:
                    puts( ANSI_WHITE "Ouch! " ANSI_NORMAL
                            "You wacked me with " ANSI_WHITE "VANILLA"
                            ANSI_NORMAL "...!" );
                    break;
                case CLOSEWINDOW:
                    fDone = TRUE;
                    break;
                default:
                    break;
            }
        }
    }

    while ( msg = (struct IntuiMessage *)GetMsg( testWnd->UserPort ) )
        ReplyMsg( (struct Message *)msg );

    CloseWindow( testWnd );
    puts( "Well, it " ANSI_WHITE "WAS fun" ANSI_NORMAL ". Wasn't it?" );
    ret_val = 0L;

BAD_RETURN:
    CloseLibrary( (struct Library *)IntuitionBase );
    return( ret_val );
}
