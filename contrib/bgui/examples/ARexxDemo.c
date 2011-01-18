/*
 * @(#) $Header$
 *
 * ARexxDemo.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1994-1995 Jan van den Baard.
 * (C) Copyright 1994-1995 Jaba Development.
 * All Rights Reserved.
 *
 * Description: Simple demonstration of the arexx class.
 *
 * $Log$
 * Revision 42.1  2000/05/15 19:29:50  stegerg
 * replacements for REG macro.
 *
 * Revision 42.0  2000/05/09 22:19:14  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:04  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:27  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/02/19 05:03:45  mlemos
 * Added support to build with Storm C.
 *
 * Revision 1.1.2.1  1998/10/12 01:50:43  mlemos
 * Initial revision integrated from Ian sources.
 *
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc arexxdemo.c -proto -mi -ms -lbgui -larexxclass.o
quit
*/

#include "DemoCode.h"

/*
**      Protos for the arexx command functions.
**/
//ASM VOID rx_Name( REG(a0) REXXARGS *, REG(a1) struct RexxMsg * );
ASM REGFUNC2(VOID, rx_Name,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rm));

//ASM VOID rx_Version( REG(a0) REXXARGS *, REG(a1) struct RexxMsg * );
ASM REGFUNC2(VOID, rx_Version,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rm));

//ASM VOID rx_Author( REG(a0) REXXARGS *, REG(a1) struct RexxMsg * );
ASM REGFUNC2(VOID, rx_Author,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rm));

//ASM VOID rx_Date( REG(a0) REXXARGS *, REG(a1) struct RexxMsg * );
ASM REGFUNC2(VOID, rx_Date,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rm));

struct Library *BGUIBase = NULL;

/*
**      The following commands are
**      valid for this demo.
**/
REXXCOMMAND Commands[] = {
        "NAME",                 NULL,                   rx_Name,
        "VERSION",              NULL,                   rx_Version,
        "AUTHOR",               NULL,                   rx_Author,
        "DATE",                 "SYSTEM/S",             rx_Date,
};

/*
**      NAME
**/
#ifdef __STORM__
VOID SAVEDS ASM
#else
SAVEDS ASM VOID
#endif
//rx_Name( REG(a0) REXXARGS *ra, REG(a1) struct RexxMsg *rxm )
REGFUNC2(,rx_Name,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rxm))
{
        /*
        **      Simply return the program name.
        **/
        ra->ra_Result = "ARexxDemo";
}

/*
**      VERSION
**/
#ifdef __STORM__
VOID SAVEDS ASM 
#else
SAVEDS ASM VOID
#endif
//rx_Version( REG(a0) REXXARGS *ra, REG(a1) struct RexxMsg *rxm )
REGFUNC2(,rx_Version,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rxm))
{
        /*
        **      Simply return the program version.
        **/
        ra->ra_Result = "1.0";
}

/*
**      AUTHOR
**/
#ifdef __STORM__
VOID SAVEDS ASM
#else
SAVEDS ASM VOID
#endif
//rx_Author( REG(a0) REXXARGS *ra, REG(a1) struct RexxMsg *rxm )
REGFUNC2(,rx_Author,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rxm))
{
        /*
        **      Simply return the authors name.
        **/
        ra->ra_Result = "Jan van den Baard";
}

/*
**      Buffer for the system date.
**/
UBYTE                   systemDate[ 10 ];

/*
**      DATE
**/
#ifdef __STORM__
VOID SAVEDS ASM
#else
SAVEDS ASM VOID
#endif
//rx_Date( REG(a0) REXXARGS *ra, REG(a1) struct RexxMsg *rxm )
REGFUNC2(,rx_Date,
	REGPARAM(A0, REXXARGS *, ra),
	REGPARAM(A1, struct RexxMsg *, rxm))
{
        struct DateTime                 dt;

        /*
        **      SYSTEM switch specified?
        **/
        if ( ! ra->ra_ArgList[ 0 ] )
                /*
                **      No. Simply return the compilation date.
                **/
                ra->ra_Result = "25-09-94";
        else {
                /*
                **      Compute system date.
                **/
                DateStamp(( struct DateStamp * )&dt );
                dt.dat_Format  = FORMAT_CDN;
                dt.dat_StrDate = systemDate;
                dt.dat_Flags   = 0;
                dt.dat_StrDay  = NULL;
                dt.dat_StrTime = NULL;
                DateToStr(&dt);
                /*
                **      And return it.
                **/
                ra->ra_Result = systemDate;
        }
}

/*
**      Object ID's.
**/
#define ID_QUIT                 1

VOID StartDemo()
{
   struct Window           *window;
   Object                  *WO_Window, *GO_Quit, *AO_Rexx;
   ULONG                    signal = 0, rxsig = 0L, rc, sigrec;
   BOOL                     running = TRUE;

   /*
    *      Create host object.
    */
   if ( AO_Rexx = BGUI_NewObject(BGUI_AREXX_OBJECT, AC_HostName, "RXDEMO", AC_CommandList, Commands, TAG_END )) {
        /*
        **      Create the window object.
        **/
        WO_Window = WindowObject,
                WINDOW_Title,           "ARexx Demo",
                WINDOW_SizeGadget,      FALSE,
                WINDOW_RMBTrap,         TRUE,
                WINDOW_AutoAspect,      TRUE,
                WINDOW_MasterGroup,
                        VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), GROUP_BackFill, SHINE_RASTER,
                                StartMember,
                                        InfoFixed( NULL,
                                                   ISEQ_C "This is a small demonstration of\n"
                                                   "the ARexx BOOPSI class. Please run the\n"
                                                   ISEQ_B "Demo.rexx" ISEQ_N " script and see\n"
                                                   "what happens",
                                                   NULL,
                                                   4 ),
                                EndMember,
                                StartMember,
                                        HGroupObject,
                                                VarSpace( 50 ),
                                                StartMember, GO_Quit  = KeyButton( "_Quit",  ID_QUIT  ), EndMember,
                                                VarSpace( 50 ),
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
                                **      Obtain wait masks.
                                **/
                                GetAttr( WINDOW_SigMask, WO_Window, &signal );
                                GetAttr( AC_RexxPortMask, AO_Rexx, &rxsig );
                                /*
                                **      Event loop...
                                **/
                                do {
                                        sigrec = Wait( signal | rxsig );

                                        /*
                                        **      ARexx event?
                                        **/
                                        if ( sigrec & rxsig )
                                                DoMethod( AO_Rexx, ACM_HANDLE_EVENT );

                                        /*
                                        **      Window event?
                                        **/
                                        if ( sigrec & signal ) {
                                                /*
                                                **      Handle events.
                                                **/
                                                while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
                                                        /*
                                                        **      Evaluate return code.
                                                        **/
                                                        switch (rc)
                                                        {
                                                        case WMHI_CLOSEWINDOW:
                                                        case ID_QUIT:
                                                           running = FALSE;
                                                           break;
                                                        }
                                                }
                                        }
                                } while ( running );
                        } else PutStr ( "Could not open the window.\n" );
                } else PutStr( "Could not assign gadget keys.\n" );
                DisposeObject( WO_Window );
        } else PutStr( "Could not create the window object.\n" );
        DisposeObject( AO_Rexx );
   } else PutStr( "Could not create the ARexx host.\n" );
}
