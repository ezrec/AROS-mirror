/*
 * @(#) $Header$
 *
 * Images.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2000/07/04 05:02:22  bergers
 * Made examples compilable.
 *
 * Revision 42.0  2000/05/09 22:19:51  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:41  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:58  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:23  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc Images.c -proto -mi -ms -mRR -lbgui
quit
*/

#include "DemoCode.h"


/*
**	Generated with IconEdit.
**/
#ifdef _AROS
UWORD HelpI1Data[] =
#else
CHIP( UWORD ) HelpI1Data[] =
#endif
{
/* Plane 0 */
    0x0000,0x0020,0x0000,0x00A0,0x0020,0x0000,0x0143,0xE020,
    0x0000,0x0A8F,0xF820,0x0000,0x051C,0x7C3F,0xE000,0x0038,
    0x3E00,0x0000,0x0038,0x3E00,0x0800,0x00FF,0xBFFF,0x0800,
    0x003C,0x3E00,0x0800,0x07FF,0xFF3F,0x0800,0x0000,0x7C00,
    0x0800,0x06FF,0x7FFD,0x0800,0x0000,0x7000,0x0800,0x07F7,
    0xFAE7,0x0800,0x0000,0xE000,0x0800,0x07DE,0xFBFD,0x0800,
    0x0000,0xC000,0x0800,0x06FD,0xF7EF,0x0800,0x0001,0xC000,
    0x0800,0x0003,0xE000,0x0800,0x0003,0xE1B7,0x0800,0x0003,
    0xE000,0x0800,0x0001,0xC000,0x0800,0x0000,0x0000,0x0800,
    0x0000,0x0000,0x0800,0x7FFF,0xFFFF,0xF800,
/* Plane 1 */
    0xFFFF,0xFFD0,0x0000,0xFEAF,0xFFDC,0x0000,0xFD5C,0x1FDF,
    0x0000,0xE0B0,0x27DF,0xC000,0xF06B,0x9BC0,0x0000,0xFFD7,
    0xDDFF,0xF000,0xFFD7,0xDDFF,0xF000,0xFF18,0x5C00,0xF000,
    0xFFDB,0xDDFF,0xF000,0xF800,0x18C0,0xF000,0xFFFF,0xBBFF,
    0xF000,0xF900,0xB002,0xF000,0xFFFF,0xAFFF,0xF000,0xF808,
    0x2518,0xF000,0xFFFF,0x5FFF,0xF000,0xF821,0x4402,0xF000,
    0xFFFF,0x3FFF,0xF000,0xF902,0x8810,0xF000,0xFFFE,0xBFFF,
    0xF000,0xFFFD,0xDFFF,0xF000,0xFFFD,0xDE48,0xF000,0xFFFD,
    0xDFFF,0xF000,0xFFFE,0x3FFF,0xF000,0xFFFF,0xFFFF,0xF000,
    0xFFFF,0xFFFF,0xF000,0x0000,0x0000,0x0000,
};

struct Image HelpI1 =
{
    0, 0,			/* Upper left corner */
    37, 26, 2,			/* Width, Height, Depth */
    HelpI1Data,                 /* Image data */
    0x0003, 0x0000,		/* PlanePick, PlaneOnOff */
    NULL			/* Next image */
};

#ifdef _AROS
UWORD HelpI2Data[] =
#else
CHIP( UWORD ) HelpI2Data[] =
#endif
{
/* Plane 0 */
    0x0000,0x0020,0x0000,0x00A0,0x0020,0x0000,0x0140,0xE020,
    0x0000,0x0A81,0xF020,0x0000,0x0501,0xF03F,0xE000,0x0001,
    0xF000,0x0000,0x0001,0xF000,0x0800,0x00FF,0xF7FF,0x0800,
    0x0000,0xE000,0x0800,0x07FB,0xFF3F,0x0800,0x0000,0xE000,
    0x0800,0x06FF,0xFFFD,0x0800,0x0000,0xE000,0x0800,0x07F7,
    0xFAE7,0x0800,0x0000,0x4000,0x0800,0x07DE,0xFBFD,0x0800,
    0x0000,0x4000,0x0800,0x06FD,0xF7EF,0x0800,0x0000,0xE000,
    0x0800,0x0001,0xF000,0x0800,0x0001,0xF1B7,0x0800,0x0001,
    0xF000,0x0800,0x0000,0xE000,0x0800,0x0000,0x0000,0x0800,
    0x0000,0x0000,0x0800,0x7FFF,0xFFFF,0xF800,
/* Plane 1 */
    0xFFFF,0xFFD0,0x0000,0xFEAF,0xFFDC,0x0000,0xFD5F,0x1FDF,
    0x0000,0xE0BE,0xEFDF,0xC000,0xF07E,0xEFC0,0x0000,0xFFFE,
    0xEFFF,0xF000,0xFFFE,0xEFFF,0xF000,0xFF00,0x4800,0xF000,
    0xFFFF,0x5FFF,0xF000,0xF804,0x40C0,0xF000,0xFFFF,0x5FFF,
    0xF000,0xF900,0x4002,0xF000,0xFFFF,0x5FFF,0xF000,0xF808,
    0x0518,0xF000,0xFFFF,0xBFFF,0xF000,0xF821,0x0402,0xF000,
    0xFFFF,0xBFFF,0xF000,0xF902,0x0810,0xF000,0xFFFF,0x5FFF,
    0xF000,0xFFFE,0xEFFF,0xF000,0xFFFE,0xEE48,0xF000,0xFFFE,
    0xEFFF,0xF000,0xFFFF,0x1FFF,0xF000,0xFFFF,0xFFFF,0xF000,
    0xFFFF,0xFFFF,0xF000,0x0000,0x0000,0x0000,
};

struct Image HelpI2 =
{
    0, 0,			/* Upper left corner */
    37, 26, 2,			/* Width, Height, Depth */
    HelpI2Data,                 /* Image data */
    0x0003, 0x0000,		/* PlanePick, PlaneOnOff */
    NULL			/* Next image */
};

/*
**	Put up a simple requester.
**/
ULONG Req( struct Window *win, UBYTE *gadgets, UBYTE *body, ... )
{
	struct bguiRequest	req = { NULL };

	req.br_GadgetFormat	= gadgets;
	req.br_TextFormat	= body;
	req.br_Flags		= BREQF_CENTERWINDOW | BREQF_LOCKWINDOW;
	req.br_Underscore	= '_';

	return( BGUI_RequestA( win, &req, ( ULONG * )( &body + 1 )));
}

/*
**	Object ID's.
**/
#define ID_QUIT                 1
#define ID_HELP                 2

VOID StartDemo( void )
{
	struct Window		*window;
	Object			*WO_Window, *GO_Quit, *GO_Help;
	ULONG			 signal = 0, rc, tmp = 0;
	BOOL			 running = TRUE;

	/*
	**	Create the window object.
	**/
	WO_Window = WindowObject,
		WINDOW_Title,		"Image Demo",
		WINDOW_AutoAspect,	TRUE,
		WINDOW_MasterGroup,
			VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
				StartMember,
					GO_Help = ButtonObject,
						BUTTON_Image,		&HelpI1,
						BUTTON_SelectedImage,	&HelpI2,
						LAB_Label,		"_Help",
						LAB_Underscore,         '_',
						LAB_Place,		PLACE_LEFT,
						FRM_Type,		FRTYPE_BUTTON,
						FRM_EdgesOnly,		TRUE,
						GA_ID,			ID_HELP,
					EndObject,
				EndMember,
				StartMember,
					HGroupObject,
						VarSpace( 50 ),
						StartMember, GO_Quit  = KeyButton( "_Quit",  ID_QUIT  ), EndMember,
						VarSpace( 50 ),
					EndObject, FixMinHeight,
				EndMember,
			EndObject,
	EndObject;

	/*
	**	Object created OK?
	**/
	if ( WO_Window ) {
		/*
		**	Assign the keys to the buttons.
		**/
		tmp  = GadgetKey( WO_Window, GO_Quit,  "q" );
		tmp += GadgetKey( WO_Window, GO_Help,  "h" );
		/*
		**	OK?
		**/
		if ( tmp == 2 ) {
			/*
			**	try to open the window.
			**/
			if ( window = WindowOpen( WO_Window )) {
				/*
				**	Obtain it's wait mask.
				**/
				GetAttr( WINDOW_SigMask, WO_Window, &signal );
				/*
				**	Event loop...
				**/
				do {
					Wait( signal );
					/*
					**	Handle events.
					**/
					while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
						/*
						**	Evaluate return code.
						**/
						switch ( rc ) {

							case	WMHI_CLOSEWINDOW:
							case	ID_QUIT:
								running = FALSE;
								break;

							case	ID_HELP:
								Req( window, "_Continue", ISEQ_C "This small demo shows you how to use\n"
											  "standard intuition images in button objects." );
								break;

						}
					}
				} while ( running );
			} else
				Tell( "Could not open the window\n" );
		} else
			Tell( "Could not assign gadget keys\n" );
		/*
		**	Disposing of the window object will
		**	also close the window if it is
		**	already opened and it will dispose of
		**	all objects attached to it.
		**/
		DisposeObject( WO_Window );
	} else
		Tell( "Could not create the window object\n" );
}
