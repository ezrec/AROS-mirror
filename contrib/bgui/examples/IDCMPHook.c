/*
 * @(#) $Header$
 *
 * IDCMPHook.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2004/06/17 07:38:47  chodorowski
 * Added missing REGFUNC_END.
 *
 * Revision 42.1  2000/05/15 19:29:50  stegerg
 * replacements for REG macro.
 *
 * Revision 42.0  2000/05/09 22:19:48  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:38  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:56  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/02/19 05:03:54  mlemos
 * Added support to build with Storm C.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:20  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc IDCMPHook.c -proto -mi -ms -mRR -lbgui
quit
*/

#include "DemoCode.h"

/*
**      Object ID.
**/
#define ID_QUIT                 1

/*
**      "tick" counter for the hook.
**/
UBYTE   Ticks = 0;

/*
**      Simple example of the hook code.
**/
#ifdef __STORM__
VOID SAVEDS ASM
#else
SAVEDS ASM VOID
#endif
//hookFunc( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct IntuiMessage *imsg )
REGFUNC3(,hookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct IntuiMessage *, imsg))
{
        struct Window                   *wptr = NULL;

        /*
        **      This hook is a simple IDCMP_INTUITICKS hook.
        **      More complex hooks can receive several message
        **      types depending on the setting of the
        **      WINDOW_IDCMPHookBits attribute.
        **
        **      Simply beep the screen
        **      every two seconds or so
        **      while the window is active.
        **/
        if ( Ticks == 20 ) {
                Ticks = 0;
                /*
                **      Only flash the screen on which
                **      the window is located.
                **/
                GetAttr( WINDOW_Window, obj, ( ULONG * )&wptr );
                if ( wptr )
                        DisplayBeep( wptr->WScreen );
        }
        Ticks++;
}
REGFUNC_END


/*
**      The hook structure.
**
** typedef unsigned long (*HOOKFUNC)();
**/
struct Hook idcmpHook = { NULL, NULL, (HOOKFUNC)hookFunc, NULL, NULL };

VOID StartDemo( void )
{
        struct Window           *window;
        Object                  *WO_Window, *GO_Quit;
        ULONG                    signal = 0, rc;
        BOOL                     running = TRUE;

        /*
        **      Create the window object.
        **/
        WO_Window = WindowObject,
                WINDOW_Title,           "IDCMPHook Demo",
                WINDOW_SizeGadget,      FALSE,
                WINDOW_RMBTrap,         TRUE,
                WINDOW_IDCMP,           IDCMP_INTUITICKS,
                WINDOW_IDCMPHookBits,   IDCMP_INTUITICKS,
                WINDOW_IDCMPHook,       &idcmpHook,
                WINDOW_AutoAspect,      TRUE,
                WINDOW_MasterGroup,
                        /*
                        **      A simple vertical group
                        **      containing a descriptive text
                        **      and a "Quit" button.
                        **/
                        VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), GROUP_BackFill, SHINE_RASTER,
                                StartMember, InfoFixed( NULL, "\33cThis small demo has a IDCMP-hook\n"
                                                              "installed which will flash the\n"
                                                              "display every two seconds while the\n"
                                                              "window is active.", NULL, 4 ), EndMember,
                                StartMember,
                                        HGroupObject,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, GO_Quit  = KeyButton( "_Quit",  ID_QUIT  ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                        EndObject,
                                EndMember,
                        EndObject,
        EndObject;

        /*
        **      Object created OK?
        **/
        if ( WO_Window ) {
                /*
                **      Assign a key to the button.
                **/
                if ( GadgetKey( WO_Window, GO_Quit,  "q" )) {
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
                                                }
                                        }
                                } while ( running );
                        } else
                                Tell( "Could not open the window\n" );
                } else
                        Tell( "Could not assign gadget key\n" );
                /*
                **      Disposing of the window object will
                **      also close the window if it is
                **      already opened and it will dispose of
                **      all objects attached to it.
                **/
                DisposeObject( WO_Window );
        } else
                Tell( "Unable to create the window object\n" );
}
