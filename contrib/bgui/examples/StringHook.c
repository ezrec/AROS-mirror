/*
 * @(#) $Header$
 *
 * StringHook.c
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
 * Revision 42.0  2000/05/09 22:20:05  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:55  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:10  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:42  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc StringHook.c -proto -mi -ms -mRR -lbgui
quit
*/

#include "DemoCode.h"

#include <ctype.h>

/*
**      Object ID's.
**/
#define ID_QUIT                 1

/*
**      Info text.
**/
UBYTE  *WinInfo = ISEQ_C "This demo shows you how to include a\n"
                  "string object edit hook. The string object\n"
                  "has a edit hook installed which only allows\n"
                  "you to enter hexadecimal characters. It will\n"
                  "also convert the character you click on to 0.";

/*
**      String object edit hook. Copied from the
**      RKM-Manual Libraries. Page 162-166.
**/
//SAVEDS ASM ULONG HexHookFunc( REG(a0) struct Hook *hook, REG(a2) struct SGWork *sgw, REG(a1) ULONG *msg )
SAVEDS ASM REGFUNC3(ULONG, HexHookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, struct SGWork *, sgw),
	REGPARAM(A1, ULONG *, msg))
{
        ULONG           rc = ~0;

        switch ( *msg ) {

                case    SGH_KEY:
                        /*
                        **      Only allow for hexadecimal characters and convert
                        **      lowercase to uppercase.
                        **/
                        if ( sgw->EditOp == EO_REPLACECHAR || sgw->EditOp == EO_INSERTCHAR ) {
                                if ( ! isxdigit( sgw->Code )) {
                                        sgw->Actions |= SGA_BEEP;
                                        sgw->Actions &= ~SGA_USE;
                                } else {
                                        sgw->WorkBuffer[ sgw->BufferPos - 1 ] = toupper( sgw->Code );
                                }
                        }
                        break;

                case    SGH_CLICK:
                        /*
                        **      Convert the character under the
                        **      cursor to 0.
                        **/
                        if ( sgw->BufferPos < sgw->NumChars )
                                sgw->WorkBuffer[ sgw->BufferPos ] = '0';
                        break;

                default:
                        rc = 0L;
                        break;
        }
        return( rc );
}
REGFUNC_END

/*
**      Uncomment the below typedef if your compiler
**      complains about it.
**/

/* typedef ULONG (*HOOKFUNC)(); */

struct Hook HexHook = { NULL, NULL, (HOOKFUNC)HexHookFunc, NULL, NULL };

VOID StartDemo( void )
{
	struct Window           *window;
	Object                  *WO_Window, *GO_Quit;
	ULONG                    signal, rc;
	BOOL                     running = TRUE;

	/*
	**      Create the window object.
	**/
	WO_Window = WindowObject,
		WINDOW_Title,           "String Edit Hook Demo",
		WINDOW_AutoAspect,      TRUE,
		WINDOW_LockHeight,      TRUE,
		WINDOW_RMBTrap,         TRUE,
		WINDOW_AutoKeyLabel,		TRUE,
		WINDOW_MasterGroup,
			VGroupObject, NormalHOffset, NormalVOffset, NormalSpacing,
				GROUP_BackFill, FILL_RASTER,
				StartMember,
					InfoFixed( NULL, WinInfo, NULL, 5 ),
				EndMember,
				StartMember,
					HGroupObject, HOffset( 4 ), VOffset( 4 ), FRM_Type, FRTYPE_BUTTON, FRM_Recessed, TRUE,
						StartMember,
							StringObject,
								LAB_Label,              "Only HEX characters:",
								LAB_Style,              FSF_BOLD,
								FuzzRidgeFrame,
								STRINGA_MaxChars,       256,
								STRINGA_EditHook,       &HexHook,
							EndObject,
						EndMember,
					EndObject,
				EndMember,
				StartMember,
					HGroupObject,
						VarSpace( 50 ),
							StartMember, GO_Quit  = FuzzButton( "_Quit",  ID_QUIT  ), EndMember,
						VarSpace( 50 ),
					EndObject, FixMinHeight,
				EndMember,
			EndObject,
		EndObject;

	/*
	**      Object created OK?
	**/
	if ( WO_Window )
	{
		/*
		**      try to open the window.
		**/
		if ( window = WindowOpen( WO_Window ))
		{
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
				while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE )
				{
					/*
					**      Evaluate return code.
					**/
					switch ( rc )
					{
					case WMHI_CLOSEWINDOW:
					case ID_QUIT:
						running = FALSE;
						break;
					}
				}
			}	while ( running );
		}	else Tell( "Could not open the window\n" );
		/*
		**      Disposing of the window object will
		**      also close the window if it is
		**      already opened and it will dispose of
		**      all objects attached to it.
		**/
		DisposeObject( WO_Window );
	}	else Tell( "Could not create the window object\n" );
}
