/*
 * @(#) $Header$
 *
 * List.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995-1996 Jaba Development.
 * (C) Copyright 1995-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * This example will show the most simple form of
 * a DragNDrop Listview. It opens a window with a
 * single-select Listview object.
 *
 * The entries in the Listview can be moved in the
 * list.
 *
 * $Log$
 * Revision 42.1  2000/08/10 11:50:54  chodorowski
 * Cleaned up and prettyfied the GUIs a bit.
 *
 * Revision 42.0  2000/05/09 22:19:53  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:43  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:00  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:26  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc List.c -proto -mi -ms -mRR -lbgui
quit
*/


#include "DemoCode.h"

/*
 *      Just a bunch of entries...
 */
STATIC UBYTE *entries[] = {
	"Entry 1",
	"Entry 2",
	"Entry 3",
	"Entry 4",
	"Entry 5",
	"Entry 6",
	"Entry 7",
	"Entry 8",
	"Entry 9",
	"Entry 10",
	"Entry 11",
	"Entry 12",
	"Entry 13",
	"Entry 14",
	"Entry 15",
	"Entry 16",
	"Entry 17",
	"Entry 18",
	"Entry 19",
	"Entry 20",
	NULL
};

/*
 *      Object ID's
 */
#define ID_QUIT         1

static char *TabLabels[] =
{
	"Single-Select",
	"Multi-Select",
	NULL
};
/*
**      Cycle to Page map-list.
**/
static ULONG Cyc2Page[] = { MX_Active, PAGE_Active, TAG_END };

/*
 *      Here we go...
 */
VOID StartDemo( void )
{
	struct Window                   *window;
	Object                  *WO_Window, *tabs, *page;
	ULONG                            signal, rc;
	BOOL                             running = TRUE;

	/*
	 *      Build the window object tree.
	 */
	WO_Window = WindowObject,
		WINDOW_Title,           "Listview DragNDrop",
		WINDOW_ScaleWidth,      25,
		WINDOW_ScaleHeight,     20,
		WINDOW_RMBTrap,         TRUE,
		WINDOW_AutoAspect,      TRUE,
		WINDOW_AutoKeyLabel,    TRUE,
		WINDOW_CloseOnEsc,      TRUE,
		WINDOW_MasterGroup,
			VGroupObject, NormalOffset, Spacing( 0 ),
				StartMember,
					tabs = Tabs(NULL, TabLabels, 0, 0),
				EndMember,
				StartMember,
					page = PageObject, FRM_Type, FRTYPE_TAB_ABOVE,
					PageMember,
						VGroupObject, WideVOffset, WideHOffset, NormalSpacing,
							StartMember,
								InfoFixed(NULL, ISEQ_C "Single-Select Drag-n-Drop\nListview object.", NULL, 2 ), FixMinHeight,
							EndMember,
							StartMember,
							/*
							 *      Create a draggable and droppable listview
							 *      and make it show the drop-spot.
							 */
								ListviewObject,
									LISTV_EntryArray,               entries,
									LISTV_ShowDropSpot,             TRUE,
									BT_DragObject,                  TRUE,
									BT_DropObject,                  TRUE,
								EndObject,
							EndMember,
						EndObject,
					PageMember,
						VGroupObject, WideVOffset, WideHOffset, NormalSpacing,
							StartMember,
								InfoFixed( NULL, ISEQ_C "Multi-Select Drag-n-Drop\nListview object.", NULL, 2 ), FixMinHeight,
							EndMember,
							StartMember,
							/*
							 *      Create a multi-select, draggable and
							 *      droppable listview and make it show
							 *      the drop-spot.
							 */
								ListviewObject,
									LISTV_MultiSelect,      TRUE,
									LISTV_EntryArray,               entries,
									LISTV_ShowDropSpot,     TRUE,
									BT_DragObject,                  TRUE,
									BT_DropObject,                  TRUE,
								EndObject,
							EndMember,
						EndObject,
					EndObject,
				EndMember,
				StartMember,
					HGroupObject, NormalOffset, NormalSpacing,
						VarSpace( DEFAULT_WEIGHT ),
						StartMember, PrefButton( "_Quit", ID_QUIT ), EndMember,
						VarSpace( DEFAULT_WEIGHT ),
					EndObject, FixMinHeight,
				EndMember,
			EndObject,
	EndObject;

	/*
	 *      Window object tree OK?
	 */
	if ( WO_Window ) {
		/*
		**      Connect the cycle to the page.
		**/
		AddMap(tabs, page, Cyc2Page );
		/*
		 *      Open the window.
		 */
		if (( window = WindowOpen( WO_Window ))) {
			/*
			 *      Get signal wait mask.
			 */
			GetAttr( WINDOW_SigMask, WO_Window, &signal );

			do {
				Wait( signal );
					/*
				 *      Handle messages.
				 */
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
			Tell( "Unable to open the window.\n" );
		DisposeObject( WO_Window );
	} else
		Tell( "Unable to create the window object.\n" );
}
