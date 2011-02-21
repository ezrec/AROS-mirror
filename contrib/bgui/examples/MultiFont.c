/*
 * @(#) $Header$
 *
 * MultiFont.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:20:00  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:49  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:06  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:34  mlemos
 * Ian sources
 *
 *
 */

/*
dcc MultiFont.c -mi -ms -mRR -proto -lbgui
quit
*/

#include "DemoCode.h"

/*
 *	Fonts used in the code.
 */
struct TextAttr ButtonFont	= { "diamond.font", 12, FS_NORMAL, FPF_DISKFONT };
struct TextAttr Info1Font	= { "emerald.font", 17, FS_NORMAL, FPF_DISKFONT };
struct TextAttr Info2Font	= { "opal.font",    9,  FS_NORMAL, FPF_DISKFONT };

struct TextFont *Button, *Info1, *Info2;
struct Library	*DiskfontBase;

/*
 *	Object ID's
 */
#define ID_QUIT                 1

/*
 *	Info texts.
 */
UBYTE  *IText1 = ISEQ_C ISEQ_HIGHLIGHT "MultiFont";
UBYTE  *IText2 = ISEQ_C "This demo shows you how you\n"
		 "can use different fonts inside a\n"
		 "single window.";

VOID StartDemo( void )
{
	struct Window		*window;
	Object			*WO_Window, *GO_Quit;
	IPTR			 signal;
	ULONG			 rc;
	BOOL			 running = TRUE;

	/*
	 *	We need this one to open the fonts.
	 */
	if (( DiskfontBase = OpenLibrary( "diskfont.library", 36 ))) {
		/*
		 *	We open the fonts ourselves since BGUI
		 *	opens all fonts with OpenFont() which
		 *	means that they have to be resident
		 *	in memory.
		 */
		if (( Button = OpenDiskFont( &ButtonFont ))) {
			if (( Info1 = OpenDiskFont( &Info1Font ))) {
				if (( Info2 = OpenDiskFont( &Info2Font ))) {
					/*
					 *	Create the window object.
					 */
					WO_Window = WindowObject,
						WINDOW_Title,		"Multi-Font Demo",
						WINDOW_AutoAspect,	TRUE,
						WINDOW_LockHeight,	TRUE,
						WINDOW_RMBTrap,         TRUE,
						WINDOW_MasterGroup,
							VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), GROUP_BackFill, SHINE_RASTER,
								StartMember,
									VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 2 ),
										FRM_Type,		FRTYPE_BUTTON,
										FRM_Recessed,		TRUE,
										StartMember,
											InfoObject,
												INFO_TextFormat,	IText1,
												INFO_HorizOffset,	0,
												INFO_VertOffset,	0,
												INFO_FixTextWidth,	TRUE,
												INFO_MinLines,		1,
												BT_TextAttr,		&Info1Font,
											EndObject,
										EndMember,
										StartMember,
											HorizSeparator,
										EndMember,
										StartMember,
											InfoObject,
												INFO_TextFormat,	IText2,
												INFO_HorizOffset,	0,
												INFO_VertOffset,	0,
												INFO_FixTextWidth,	TRUE,
												INFO_MinLines,		3,
												BT_TextAttr,		&Info2Font,
											EndObject,
										EndMember,
									EndObject,
								EndMember,
								StartMember,
									HGroupObject,
										VarSpace( 50 ),
										StartMember,
											GO_Quit = ButtonObject,
												LAB_Label,	"_Quit",
												LAB_Underscore, '_',
												ButtonFrame,
												GA_ID,		ID_QUIT,
												BT_TextAttr,	&ButtonFont,
											EndObject,
										EndMember,
										VarSpace( 50 ),
									EndObject, FixMinHeight,
								EndMember,
							EndObject,
					EndObject;

					/*
					 *	Object created OK?
					 */
					if ( WO_Window ) {
						/*
						 *	Assign the key to the button.
						 */
						if ( GadgetKey( WO_Window, GO_Quit,  "q" )) {
							/*
							 *	try to open the window.
							 */
							if (( window = WindowOpen( WO_Window ))) {
								/*
								 *	Obtain it's wait mask.
								 */
								GetAttr( WINDOW_SigMask, WO_Window, &signal );
								/*
								 *	Event loop...
								 */
								do {
									Wait( signal );
									/*
									 *	Handle events.
									 */
									while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
										/*
										 *	Evaluate return code.
										 */
										switch ( rc ) {

											case	WMHI_CLOSEWINDOW:
											case	ID_QUIT:
												running = FALSE;
												break;
										}
									}
								} while ( running );
							} else
								Tell( "Could not open the window\n" );
						} else
							Tell( "Could not assign gadget keys\n" );
						/*
						 *	Disposing of the window object will
						 *	also close the window if it is
						 *	already opened and it will dispose of
						 *	all objects attached to it.
						 */
						DisposeObject( WO_Window );
					} else
						Tell( "Could not create the window object\n" );
					CloseFont( Info2 );
				} else
					Tell( "Could not open opal.font\n" );
				CloseFont( Info1 );
			} else
				Tell( "Could not open emerald.font\n" );
			CloseFont( Button );
		} else
			Tell( "Could not open diamond.font\n" );
		CloseLibrary( DiskfontBase );
	} else
		Tell( "Could not open diskfont.library\n" );
}
