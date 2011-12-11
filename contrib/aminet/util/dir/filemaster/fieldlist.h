/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
 *	FIELDLIST.H
 *
 *	(C) Copyright 1995-1996 Jaba Development.
 *	(C) Copyright 1995-1996 Jan van den Baard.
 *	    All Rights Reserved.
 *
 *	This is a simple subclass of the Listview class which
 *	will allow drops from another Listview object, positioned
 *	drops and sortable drops.
 *
 *	It is included in the demonstration programs that make
 *	use of it.
 *
 *	Tags:
 *		FL_DropAccept -- A pointer to the listview object from
 *				 which drops will be accepted. Ofcourse
 *				 you are responsible to make sure that
 *				 the entries of this object are of the
 *				 same format as the entries of the
 *				 target object.
 *
 *		FL_SortDrops -- When TRUE the class will sort the drops
 *				automatically. When FALSE the drops may
 *				be positioned by the user.
 */

#include <proto/utility.h>
#include <libraries/bgui.h>
#include <proto/bgui.h>
#include <libraries/bgui_macros.h>

#ifdef AROS
/* ruct Library          *BGUIBase=NULL; */
#include <aros/asmcall.h>
#endif

/*
 *	Tags for this subclass.
 */
#define FL_AcceptDrop0		TAG_USER+0x2000         /* IS--- */
#define FL_AcceptDrop1		TAG_USER+0x2001         /* IS--- */
#define FL_AcceptDrop2		TAG_USER+0x2002         /* IS--- */
#define FL_AcceptDrop3		TAG_USER+0x2003         /* IS--- */
#define FL_AcceptDrop4		TAG_USER+0x2004         /* IS--- */
#define FL_AcceptDrop5		TAG_USER+0x2005         /* IS--- */
#define FL_SortDrops		TAG_USER+0x2006         /* IS--- */
#define FL_RemoveOld		TAG_USER+0x2007

/*
 *	Object instance data.
 */
typedef struct {
	Object	*fld_Accept[6];		/* Accept drops from this object. */
	BOOL	 fld_SortDrops;         /* Auto-sort drops. */
	BOOL	 fld_RemoveOld;
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
#ifdef AROS
void SetFLAttr(FLD *fld, struct TagItem *attr)
#else
STATIC void __saveds __asm SetFLAttr(register __a0 FLD *fld, register __a1 struct TagItem *attr )
#endif
{
	struct TagItem		*tag, *tstate = attr;

	/*
	 *	Scan attribute list.
	 */
	while ( (tag = NextTagItem( (const struct TagItem **)&tstate ))) {
		switch ( tag->ti_Tag ) {

			case	FL_AcceptDrop0:
				fld->fld_Accept[0] = ( Object * )tag->ti_Data;
				break;
			case	FL_AcceptDrop1:
				fld->fld_Accept[1] = ( Object * )tag->ti_Data;
				break;
			case	FL_AcceptDrop2:
				fld->fld_Accept[2] = ( Object * )tag->ti_Data;
				break;
			case	FL_AcceptDrop3:
				fld->fld_Accept[3] = ( Object * )tag->ti_Data;
				break;
			case	FL_AcceptDrop4:
				fld->fld_Accept[4] = ( Object * )tag->ti_Data;
				break;
			case	FL_AcceptDrop5:
				fld->fld_Accept[5] = ( Object * )tag->ti_Data;
				break;

			case	FL_SortDrops:
				fld->fld_SortDrops = tag->ti_Data;
				break;

			case	FL_RemoveOld:
				fld->fld_RemoveOld = tag->ti_Data;
				break;
		}
	}
}
/*
 *	Class dispatcher. Remember! NOSTACKCHECK or __interrupt for
 *	SAS users!
 */

#ifdef __AROS__
AROS_UFH3S(ULONG, DispatchFL,
    AROS_UFHA(Class *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
#else
STATIC ULONG __saveds __asm DispatchFL(register __a0 Class *cl,register __a2 Object *obj, register __a1 Msg msg )
#endif
{
#ifdef __AROS__
    	AROS_USERFUNC_INIT
#endif

	FLD		       *fld,*fld2;
	APTR			entry;
	struct IBox	       *ib;
	ULONG			rc, spot;
ULONG *acceptapu;
WORD cnt1;

	/*
	 *	What do they want...
	 */
	switch ( msg->MethodID ) {

		case	OM_NEW:
			/*
			 *	Let the superclass make the object.
			 */
			if (( rc = DoSuperMethodA( cl, obj, msg ))) {
				/*
				 *	Get instance data.
				 */
				fld = ( FLD * )INST_DATA( cl, rc );

				/*
				 *	Set defaults.
				 */
				fld->fld_Accept[0]    = NULL;
				fld->fld_Accept[1]    = NULL;
				fld->fld_Accept[2]    = NULL;
				fld->fld_Accept[3]    = NULL;
				fld->fld_Accept[4]    = NULL;
				fld->fld_Accept[5]    = NULL;
				fld->fld_SortDrops = FALSE;
				fld->fld_RemoveOld = FALSE;

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
			acceptapu=QUERY(msg)->bmdp_Source;
			for(cnt1=0;cnt1<6;cnt1++) {
				if(acceptapu&&acceptapu==fld->fld_Accept[cnt1]) break;
			}
			if ( cnt1<6) {
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
			while (( entry = ( APTR )FirstSelected( DROP( msg )->bmd_Source ))) {
				/*
				 *	Set it on ourselves. We insert it when we are
				 *	not sortable. We add them sorted when we are
				 *	sortable.
				 */
				if ( fld->fld_SortDrops == FALSE ) DoMethod( obj, LVM_INSERTSINGLE, NULL, spot, (IPTR)entry, LVASF_SELECT );
				else				   DoMethod( obj, LVM_ADDSINGLE, NULL, (IPTR)entry, LVAP_SORTED, LVASF_SELECT );

				/*
				 *	Remove it from the dropped object.
				 */
				fld2=(FLD*)INST_DATA(cl,DROP(msg)->bmd_Source);
				if(fld2->fld_RemoveOld) DoMethod( DROP( msg )->bmd_Source, LVM_REMENTRY, NULL, (IPTR)entry );
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
#ifdef __AROS__
    	AROS_USERFUNC_EXIT
#endif
}
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
	if ( (super = BGUI_GetClassPtr( BGUI_LISTVIEW_GADGET ))) {
		/*
		 *	Create the class.
		 */
		if (( cl = MakeClass( NULL, NULL, super, sizeof( FLD ), 0L )))
			/*
			 *	Setup dispatcher.
			 */ 
			cl->cl_Dispatcher.h_Entry = ( HOOKFUNC  )DispatchFL;
		}
	return( cl );
}
