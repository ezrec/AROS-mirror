/*
 * @(#) $Header$
 *
 * BGUI Prefs Editor
 * BGUIPrefs_iff.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Respect Software
 * (C) Copyright 1996-1997 Ian J. Einman.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:17:05  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:29:54  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:56:54  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/09/20 17:28:48  mlemos
 * Peter Bornhall sources.
 *
 *
 *
 */

Object	**ar = BGUIPrefsObjs;
Object	*BGUIPrefsObjs[ BGP_NUMGADS ];

// GROUP DEFAULTS
extern	IPTR		 grp_frmtype,
					 grp_recessed,
					 grp_vspnarrow,
					 grp_hspnarrow,
					 grp_vspnormal,
					 grp_hspnormal,
					 grp_vspwide,
					 grp_hspwide,

// BUTTON DEFAULTS
					 but_frmtype,
					 but_recessed,

// STRING DEFAULTS
					 str_frmtype,
					 str_recessed,

// CHECKBOX DEFAULTS
					 chk_frmtype,
					 chk_recessed,

// CYCLE DEFAULTS
					 cyc_frmtype,
					 cyc_recessed,
					 cyc_popup,
					 cyc_popactive,

// INFO DEFAULTS
					 inf_frmtype,
					 inf_recessed,

// PROGRESS DEFAULTS
					 prg_frmtype,
					 prg_recessed,

// INDICATOR DEFAULTS
					 ind_frmtype,
					 ind_recessed;


struct TagItem *
CloneTagList( Tag t, ... )
{
	AROS_SLOWSTACKTAGS_PRE_AS(t, struct TagItem *)
	retval = CloneTagItems(AROS_SLOWSTACKTAGS_ARG(t));
	AROS_SLOWSTACKTAGS_POST
}


void
SaveTagList( struct IFFHandle *iff, ULONG type, struct TagItem *tags )
{
	struct	TagItem	*tag;
	const struct TagItem *tstate = tags;
	ULONG	zero = TAG_DONE;

	PushChunk( iff, ID_BGUI, ID_DTAG, IFFSIZE_UNKNOWN );
	WriteChunkBytes( iff, ( UBYTE * )&type, sizeof( ULONG ) );

	while(( tag = NextTagItem( &tstate ) )) {

		WriteChunkBytes( iff, ( UBYTE * )tag, sizeof( struct TagItem ) );

	};

	WriteChunkBytes( iff, ( UBYTE * )&zero, sizeof( ULONG ) );
	PopChunk( iff );

}


void
SaveBGUIPrefs( char *dirname, char *name )
{
	BPTR				 dir,
						 olddir;
	struct	IFFHandle	*iff;
	struct	TagItem		*objecttags = NULL;

	dir = Lock( dirname, ACCESS_READ );

	if( !dir ) dir = CreateDir( dirname );

	if( dir ) {

		olddir = CurrentDir( dir );

		if(( iff = AllocIFF() )) {

			if(( iff->iff_Stream = (IPTR)Open( name, MODE_NEWFILE ) )) {

				InitIFFasDOS( iff );
				if( OpenIFF( iff, IFFF_WRITE ) == 0 ) {

					PushChunk( iff, ID_BGUI, ID_FORM, IFFSIZE_UNKNOWN );

// GROUP
					objecttags = CloneTagList(
						FRM_DefaultType, grp_frmtype,
						FRM_Recessed, grp_recessed,
						GROUP_DefVSpaceNarrow, grp_vspnarrow,
						GROUP_DefHSpaceNarrow, grp_hspnarrow,
						GROUP_DefVSpaceNormal, grp_vspnormal,
						GROUP_DefHSpaceNormal, grp_hspnormal,
						GROUP_DefVSpaceWide, grp_vspwide,
						GROUP_DefHSpaceWide, grp_hspwide,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_GROUP_GADGET, objecttags );

// BUTTON
					objecttags = CloneTagList(
						FRM_Type, but_frmtype,
						FRM_Recessed, but_recessed,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_BUTTON_GADGET, objecttags );

// STRING
					objecttags = CloneTagList(
						FRM_Type, str_frmtype,
						FRM_Recessed, str_recessed,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_STRING_GADGET, objecttags );

// CHECKBOX
					objecttags = CloneTagList(
						FRM_Type, chk_frmtype,
						FRM_Recessed, chk_recessed,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_CHECKBOX_GADGET, objecttags );

// CYCLE
					objecttags = CloneTagList(
						FRM_Type, cyc_frmtype,
						FRM_Recessed, cyc_recessed,
						CYC_PopActive, cyc_popactive,
						CYC_Popup, cyc_popup,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_CYCLE_GADGET, objecttags );

// INFO
					objecttags = CloneTagList(
						FRM_DefaultType, inf_frmtype,
						FRM_Recessed, inf_recessed,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_INFO_GADGET, objecttags );

// PROGRESS
					objecttags = CloneTagList(
						FRM_Type, prg_frmtype,
						FRM_Recessed, prg_recessed,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_PROGRESS_GADGET, objecttags );

// INDICATOR
					objecttags = CloneTagList(
						FRM_DefaultType, ind_frmtype,
						FRM_Recessed, ind_recessed,
						TAG_DONE
					);
					SaveTagList( iff, BGUI_INDICATOR_GADGET, objecttags );

					PopChunk( iff );
					CloseIFF( iff );
				};
				Close( (BPTR)iff->iff_Stream );
			};
			FreeIFF( iff );
		};
		CurrentDir( olddir );
		UnLock( dir );
	};
}
