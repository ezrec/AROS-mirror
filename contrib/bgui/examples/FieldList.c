/*
 * @(#) $Header$
 *
 * FieldList.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995-1996 Jaba Development.
 * (C) Copyright 1995-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * This example is a bit more complicated. It will
 * open a window with two listviews. The left-one
 * is a sortable listview which will automatically
 * sort the entries which are dropped on it.
 *
 * The right listview allows you to position the
 * dropped entries.
 *
 * This demonstration uses the listview subclass
 * as defined in the "fieldlist.h" file.
 *
 * $Log$
 * Revision 42.1  2000/08/10 11:50:54  chodorowski
 * Cleaned up and prettyfied the GUIs a bit.
 *
 * Revision 42.0  2000/05/09 22:19:41  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:30  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:49  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:12  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0.
dcc FieldList.c -proto -mi -ms -mRR -3.0 -lbgui
quit
*/

#include "DemoCode.h"
#include "FieldList.h"

#include <graphics/gfxbase.h>
#include <string.h>

/*
 *      Just a bunch of entries like the
 *      ones found in the SnoopDos 3.0
 *      Format Editor.
 *
 *      This does not have to be sorted since the
 *      class will do this for us.
 */
STATIC UBYTE *entries[] = {
	"CallAddr\t%c",
	"Date\t%d",
	"Hunk:Offset\t%h",
	"Task ID\t%i",
	"Segment Name\t%s",
	"Time\t%t",
	"Count\t%u",
	"Process Name\t%p",
	"Action\t%a",
	"Target Name\t%n",
	"Options\t%o",
	"Res.\t%r",
	NULL
};

/*
 *      Object ID's
 */
#define ID_QUIT         1

extern struct GfxBase           *GfxBase;

/*
 *      Here we go...
 */
VOID StartDemo( void )
{
	struct Window                   *window;
	struct TextAttr                  fixed;
	UBYTE                            fname[ 32 ];
	Object                          *WO_Window, *GO_ListSorted, *GO_ListPlace;
	IPTR                             signal;
	ULONG                            rc;
	Class                           *class;
	BOOL                             running = TRUE;
	
	ULONG                            weights[] = { 50, 5 };

	/*
	 *      Close your eyes! This is very ugly code and should
	 *      not be regarded as good programming practice!
	 *
	 *      Do not use this kind of code in serious programs. I
	 *      just did this to keep it simple.
	 */
	Forbid();
	strcpy( fname, GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name );
	fixed.ta_Name   = fname;
	fixed.ta_YSize  = GfxBase->DefaultFont->tf_YSize;
	fixed.ta_Style  = GfxBase->DefaultFont->tf_Style;
	fixed.ta_Flags  = GfxBase->DefaultFont->tf_Flags;
	Permit();

	/*
	 *      Initialize the class.
	 */
	if (( class = InitFLClass())) {
		/*
		 *      Build the window object tree.
		 */
		WO_Window = WindowObject,
			WINDOW_Title,                   "Listview Drag-n-Drop",
			WINDOW_ScaleWidth,      25,
			WINDOW_ScaleHeight,     15,
			WINDOW_RMBTrap,         TRUE,
			WINDOW_AutoAspect,      TRUE,
			WINDOW_AutoKeyLabel,    TRUE,
			WINDOW_MasterGroup,
				VGroupObject, HOffset(6), VOffset(6), Spacing(6),
					StartMember,
						InfoFixed( NULL, ISEQ_C "Field selection using\nListview Drag-n-Drop.", NULL, 2 ), FixMinHeight,
					EndMember,
					StartMember,
						HGroupObject, Spacing(6),
							StartMember,
								/*
								 *      Create a auto-sort draggable and
								 *      droppable FL class object.
								 */
								GO_ListSorted = NewObject( class, NULL, LAB_Label,              "Available Fields",
													LAB_Place,                                      PLACE_ABOVE,
													LISTV_EntryArray,                       entries,
													LISTV_SortEntryArray,   TRUE,
													LISTV_Columns,                          2,
													LISTV_ColumnWeights,            weights,
												//      LISTV_ListFont,         &fixed,
													FL_SortDrops,                           TRUE,
													BT_DragObject,                          TRUE,
													BT_DropObject,                          TRUE,
													TAG_END ),
							EndMember,
							StartMember,
								/*
								 *      Create a draggable and dropable
								 *      FL class object which allows positioning
								 *      the drops.
								 */
								GO_ListPlace = NewObject( class, NULL, LAB_Label,               "Current Format",
												       LAB_Place,               PLACE_ABOVE,
												       LISTV_ShowDropSpot,      TRUE,
												  //     LISTV_Columns,         2,
												//              LISTV_ColumnWeights,            weights,
												   //  LISTV_ListFont,         &fixed,
												       BT_DragObject,           TRUE,
												       BT_DropObject,           TRUE,
												       TAG_END ),
							EndMember,
						EndObject,
					EndMember,
					StartMember,
						HGroupObject,
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
			 *      Tell the FL class objects to accept
			 *      drops from eachother.
			 */
			SetAttrs( GO_ListSorted, FL_AcceptDrop, GO_ListPlace,  TAG_END );
			SetAttrs( GO_ListPlace,  FL_AcceptDrop, GO_ListSorted, TAG_END );

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
		FreeClass( class );
	} else
		Tell( "Unable to initialize the FL class.\n" );
}
