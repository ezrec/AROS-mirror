/*
 * @(#) $Header$
 *
 * Useless.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995-1996 Jaba Development.
 * (C) Copyright 1995-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * This is DragNDrop in it's most basic form. It is totally
 * useless unless you create subclasses which actually make
 * use of the dragged and dropped information.
 *
 * This example does show you a way to handle input events
 * in a class which supports dragging.
 *
 * The program will open a window with two buttons, one draggable
 * and one droppable. You can drag the draggable object on
 * the dropable and when you release it the screen will beep.
 *
 * $Log$
 * Revision 42.2  2004/06/17 07:38:47  chodorowski
 * Added missing REGFUNC_END.
 *
 * Revision 42.1  2000/05/15 19:29:50  stegerg
 * replacements for REG macro.
 *
 * Revision 42.0  2000/05/09 22:20:11  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:00  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:15  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:45  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc Useless.c -proto -mi -ms -mRR -lbgui
quit
*/

#include "DemoCode.h"

/*
 *	Simple type-cast.
 */
#define INPUT(x)		(( struct gpInput * )x )

/*
 *	We need a simple subclass of the BGUI button class to
 *	make it draggable.
 */
//SAVEDS ASM ULONG DispatchSB( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
SAVEDS ASM REGFUNC3(ULONG, DispatchSB,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
	struct RastPort         *rp;
	struct gpInput		 copy;
	ULONG			 rc = 0L, dr;

	switch ( msg->MethodID ) {

		case	GM_HANDLEINPUT:
			/*
			 *	Handle the input. To implement a draggable object we
			 *	need to call the BASE_DRAGGING method as part of our
			 *	input-processing code. This method will tell us what
			 *	to do with the input event.
			 */

			/*
			 *	Copy the input message. The BASE_DRAGGING method uses
			 *	the same message structure.
			 */
			copy = *INPUT( msg );
			copy.MethodID = BASE_DRAGGING;

			/*
			 *	Send this method to ourself. Eventually it will end
			 *	up at the baseclass.
			 */
			if (( dr = DoMethodA( obj, ( Msg )&copy )) != BDR_DRAGGING ) {
				switch ( dr ) {

					case	BDR_DRAGPREPARE:
						/*
						 *	The baseclass is about to start
						 *	dragging the object. We de-select
						 *	the object.
						 */
						if ( rp = ObtainGIRPort( INPUT( msg )->gpi_GInfo )) {
							SetAttrs( obj, GA_Selected, FALSE, TAG_END );
							DoMethod( obj, GM_RENDER, INPUT( msg )->gpi_GInfo, rp, GREDRAW_REDRAW );
							ReleaseGIRPort( rp );
						}
						break;

					case	BDR_NONE:
						/*
						 *	The baseclass did not respond to the message
						 *	so we will handle this event by calling the
						 *	superclass (buttonclass).
						 */
						rc = DoSuperMethodA( cl, obj, msg );
						break;

					case	BDR_DROP:
						/*
						 *	Object dropped on the other object.
						 */
					case	BDR_CANCEL:
						/*
						 *	Dropped on a non-droppable object or
						 *	operation cancelled. We simply return
						 *	GMR_NOREUSE in these cases.
						 */
						rc = GMR_NOREUSE;
						break;
				}
			} else
				rc = GMR_MEACTIVE;
			break;

		default:
			/*
			 *	All the rest goes to the superclass...
			 */
			rc = DoSuperMethodA( cl, obj, msg );
			break;
	}
	return( rc );
}
REGFUNC_END

/*
 *	Initialize the useless class.
 */
Class *InitUselessClass( void )
{
	Class			*cl = NULL, *super;

	/*
	 *	Get a pointer to the ButtonClass which
	 *	is our superclass.
	 */
	if ( super = BGUI_GetClassPtr( BGUI_BUTTON_GADGET )) {
		if ( cl = MakeClass( NULL, NULL, super, 0L, 0L ))
			cl->cl_Dispatcher.h_Entry = ( HOOKFUNC )DispatchSB;
	}
	return( cl );
}

/*
 *	Here we go.
 */
VOID StartDemo( void )
{
	struct Window		*window;
	Class			*class;
	Object			*WO_Window;
	ULONG			 signal = 0, rc;
	BOOL			 running = TRUE;

	/*
	 *	Setup the class.
	 */
	if ( class = InitUselessClass()) {
		/*
		 *	Create the window object tree.
		 */
		WO_Window = WindowObject,
			WINDOW_Title,		"Useless ;)",
			WINDOW_RMBTrap,         TRUE,
			WINDOW_AutoAspect,	TRUE,
			WINDOW_SizeGadget,	FALSE,
			WINDOW_MasterGroup,
				VGroupObject, NormalVOffset, NormalHOffset, NormalSpacing,
					StartMember,
						InfoFixed( NULL, ISEQ_C "Useless demo. Just drag and drop the\nleft button on the right one", NULL, 2 ), FixMinHeight,
					EndMember,
					StartMember,
						HGroupObject, WideSpacing,
							StartMember,
								/*
								 *	Create an object from the above
								 *	defined class.
								 */
								NewObject( class, NULL, FuzzButtonFrame,
									Label( "Drag me!" ), BT_DragObject, TRUE, TAG_END ),
							EndMember,
							StartMember,
								ButtonObject,
									FuzzButtonFrame,
									Label( "Drop me!" ),
									BT_DropObject, TRUE,
								EndObject,
							EndMember,
						EndObject,
					EndMember,
				EndObject,
		EndObject;

		/*
		 *	Object OK?
		 */
		if ( WO_Window ) {
			/*
			 *	Open it.
			 */
			if ( window = WindowOpen( WO_Window )) {
				/*
				 *	Get signal mask.
				 */
				GetAttr( WINDOW_SigMask, WO_Window, &signal );
				do {
					Wait( signal );

					/*
					 *	Handle messages.
					 */
					while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
						switch ( rc ) {
							case	WMHI_CLOSEWINDOW:
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
		Tell( "Unable to initialize the class.\n" );
}
