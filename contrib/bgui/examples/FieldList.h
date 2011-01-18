/*
 * @(#) $Header$
 *
 * FieldList.h
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995-1996 Jaba Development.
 * (C) Copyright 1995-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * This is a simple subclass of the Listview class which
 * will allow drops from another Listview object, positioned
 * drops and sortable drops.
 *
 * It is included in the demonstration programs that make
 * use of it.
 *
 * Tags:
 *  FL_DropAccept -- A pointer to the listview object from
 *    which drops will be accepted. Ofcourse
 *    you are responsible to make sure that
 *    the entries of this object are of the
 *    same format as the entries of the
 *    target object.
 *
 *  FL_SortDrops -- When TRUE the class will sort the drops
 *    automatically. When FALSE the drops may
 *    be positioned by the user.
 *
 * $Log$
 * Revision 42.2  2004/06/17 07:38:47  chodorowski
 * Added missing REGFUNC_END.
 *
 * Revision 42.1  2000/05/15 19:28:19  stegerg
 * REG() macro replacementes
 *
 * Revision 42.0  2000/05/09 22:19:43  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:33  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:52  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/02/19 05:03:52  mlemos
 * Added support to build with Storm C.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:52  mlemos
 * Ian sources
 *
 *
 */

#include <proto/utility.h>

/*
 *	Tags for this subclass.
 */
#define FL_AcceptDrop		TAG_USER+0x2000         /* IS--- */
#define FL_SortDrops		TAG_USER+0x2001         /* IS--- */

/*
 *	Object instance data.
 */
typedef struct {
	Object			*fld_Accept;		/* Accept drops from this object. */
	BOOL			 fld_SortDrops;         /* Auto-sort drops. */
} FLD;

/*
 *	Some easy type-casts.
 */
#define SET(x)		(( struct opSet * )x )
#define QUERY(x)	(( struct bmDragPoint * )x )
#define DROP(x)         (( struct bmDropped * )x )

/*
 *	Set attributes.
 */
//STATIC ASM VOID SetFLAttr( REG(a0) FLD *fld, REG(a1) struct TagItem *attr )
STATIC ASM REGFUNC2(VOID, SetFLAttr,
	REGPARAM(A0, FLD *, fld),
	REGPARAM(A1, struct TagItem *, attr))
{
	struct TagItem		*tag, *tstate = attr;

	/*
	 *	Scan attribute list.
	 */
	while ( tag = NextTagItem( &tstate )) {
		switch ( tag->ti_Tag ) {

			case	FL_AcceptDrop:
				fld->fld_Accept = ( Object * )tag->ti_Data;
				break;

			case	FL_SortDrops:
				fld->fld_SortDrops = tag->ti_Data;
				break;
		}
	}
}
REGFUNC_END

/*
 *	Class dispatcher. Remember! NOSTACKCHECK or __interrupt for
 *	SAS users!
 */
#ifdef __STORM__
STATIC ULONG SAVEDS ASM
#else
STATIC SAVEDS ASM ULONG
#endif
//DispatchFL( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
REGFUNC3(,DispatchFL,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
	FLD		       *fld;
	APTR			entry;
	struct IBox	       *ib;
	ULONG			rc, spot;

	/*
	 *	What do they want...
	 */
	switch ( msg->MethodID ) {

		case	OM_NEW:
			/*
			 *	Let the superclass make the object.
			 */
			if ( rc = DoSuperMethodA( cl, obj, msg )) {
				/*
				 *	Get instance data.
				 */
				fld = ( FLD * )INST_DATA( cl, rc );

				/*
				 *	Set defaults.
				 */
				fld->fld_Accept    = NULL;
				fld->fld_SortDrops = FALSE;

				/*
				 *	Get attributes.
				 */
				SetFLAttr( fld, SET( msg )->ops_AttrList );
			}
			break;

		case	OM_SET:
			/*
			 *	First the superclass.
			 */
			rc = DoSuperMethodA( cl, obj, msg );

			/*
			 *	Then we have a go.
			 */
			fld = ( FLD * )INST_DATA( cl, obj );
			SetFLAttr( fld, SET( msg )->ops_AttrList );
			break;

		case	BASE_DRAGQUERY:
			/*
			 *	We only allow drops from ourselves and from
			 *	the object specified with FL_AcceptDrop.
			 */

			/*
			 *	We let the superclass worry about it when
			 *	the requesting object request is us.
			 */
			if ( QUERY( msg )->bmdp_Source == obj )
				return( DoSuperMethodA( cl, obj, msg ));

			/*
			 *	Get instance data.
			 */
			fld = ( FLD * )INST_DATA( cl, obj );

			/*
			 *	Is it the object specified with FL_AcceptDrop?
			 */
			if ( QUERY( msg )->bmdp_Source == fld->fld_Accept ) {
				/*
				 *	Get the listview class list bounds.
				 */
				GetAttr( LISTV_ViewBounds, obj, ( ULONG * )&ib );

				/*
				 *	Mouse inside view bounds? Since the superclass
				 *	starts sending this message when the mouse goes
				 *	inside the hitbox we only need to check if it
				 *	is not located right of the view area.
				 */
				if ( QUERY( msg )->bmdp_Mouse.X < ib->Width )
					return( BQR_ACCEPT );
			}

			/*
			 *	Screw the rest...
			 */
			rc = BQR_REJECT;

			break;

		case	BASE_DROPPED:
			/*
			 *	If the drop comes from ourself we let the
			 *	superclass handle it.
			 */
			if ( DROP( msg )->bmd_Source == obj )
				return( DoSuperMethodA( cl, obj, msg ));

			/*
			 *	Get instance data.
			 */
			fld = ( FLD * )INST_DATA( cl, obj );

			/*
			 *	Find out where the drop was made.
			 */
			GetAttr( LISTV_DropSpot, obj, &spot );

			/*
			 *	Simply pick up all selected entries
			 *	from the dragged object.
			 */
			while ( entry = ( APTR )FirstSelected( DROP( msg )->bmd_Source )) {
				/*
				 *	Set it on ourselves. We insert it when we are
				 *	not sortable. We add them sorted when we are
				 *	sortable.
				 */
				if ( fld->fld_SortDrops == FALSE ) DoMethod( obj, LVM_INSERTSINGLE, NULL, spot, entry, LVASF_SELECT );
				else				   DoMethod( obj, LVM_ADDSINGLE, NULL, entry, LVAP_SORTED, LVASF_SELECT );

				/*
				 *	Remove it from the dropped object.
				 */
				DoMethod( DROP( msg )->bmd_Source, LVM_REMENTRY, NULL, entry );
			}

			/*
			 *	Refresh the dragged object. We do not have to
			 *	refresh ourselves since the base class will
			 *	do this for us when we are deactivated.
			 */
			BGUI_DoGadgetMethod( DROP( msg )->bmd_Source,
					     DROP( msg )->bmd_SourceWin,
					     DROP( msg )->bmd_SourceReq,
					     LVM_REFRESH, NULL );
			rc = 1;
			break;

		default:
			/*
			 *	Let's the superclass handle the rest.
			 */
			rc = DoSuperMethodA( cl, obj, msg );
			break;
	}
	return( rc );

}
REGFUNC_END

/*
 *	Simple class initialization.
 */
Class *InitFLClass( void )
{
	Class			*super, *cl = NULL;

	/*
	 *	Obtain the ListviewClass pointer which
	 *	will be our superclass.
	 */
	if ( super = BGUI_GetClassPtr( BGUI_LISTVIEW_GADGET )) {
		/*
		 *	Create the class.
		 */
		if ( cl = MakeClass( NULL, NULL, super, sizeof( FLD ), 0L ))
			/*
			 *	Setup dispatcher.
			 */
			cl->cl_Dispatcher.h_Entry = ( HOOKFUNC )DispatchFL;
	}
	return( cl );
}
