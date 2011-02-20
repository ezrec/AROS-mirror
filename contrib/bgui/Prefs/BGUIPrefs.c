/*
 * @(#) $Header$
 *
 * BGUI Prefs Editor
 * BGUIPrefs.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Respect Software
 * (C) Copyright 1996-1997 Ian J. Einman.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2003/01/18 19:10:07  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.1  2000/07/09 18:06:21  bergers
 * Made the Preferences tool compilable for AROS. This one has an MMakefile!
 *
 * Revision 42.0  2000/05/09 22:16:54  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:29:45  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:56:46  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.4  1999/08/30 05:02:19  mlemos
 * Updated the copyright and the version information.
 *
 * Revision 1.1.2.3  1999/08/29 20:42:43  mlemos
 * Removed window opening and disposing timing code.
 *
 * Revision 1.1.2.2  1999/08/29 17:11:07  mlemos
 * Made the preferences and example pages be created and added on demand.
 *
 * Revision 1.1.2.1  1998/09/20 17:28:35  mlemos
 * Peter Bornhall sources.
 *
 *
 *
 */

#ifdef __AROS__
extern struct Library * BGUIBase;
#endif

//
//	INCLUDES
//
#include "BGUIPrefs_rev.h"
#include "BGUIPrefs.h"
#include "BGUIPrefs_gui.c"
#include "BGUIPrefs_iff.c"


//
//	CONSTANTS
//
const static char	VersTag[]		= VERSTAG,
					PrefDir[]		= "SYS:Prefs/Presets";

const static UBYTE	Cycle2Frame[]	= {
	FRTYPE_BUTTON,
	FRTYPE_FUZZ_BUTTON,
	FRTYPE_XEN_BUTTON,
	FRTYPE_RIDGE,
	FRTYPE_FUZZ_RIDGE,
	FRTYPE_DROPBOX,
	FRTYPE_NEXT,
	FRTYPE_NONE,
};


//
//	EXTERNAL
//
extern	Object	**ar;
extern	Object	*BGUIPrefsObjs[ BGP_NUMGADS ];

extern	Object	*InitBGUIPrefs( ULONG page);
extern	void	 SaveBGUIPrefs( char *dirname, char *name );
//extern	Object	*FrameFrom( Object *o );


//
//	LOCAL GLOBAL
//
struct	Library		*BGUIBase;
struct	Window		*window = NULL;

struct	NewMenu	 Menus[] =	{
	{ NM_TITLE,	"Project",				NULL,	0,	0,	NULL },
	{ NM_ITEM,	"Open...",				"O",	0,	0,	(APTR)MNU_OPEN },
	{ NM_ITEM,	"Save As...",			"A",	0,	0,	(APTR)MNU_SAVEAS },
	{ NM_ITEM,	NM_BARLABEL,			NULL,	0,	0,	NULL },
	{ NM_ITEM,	"About...",				"?",	0,	0,	(APTR)MNU_ABOUT },
	{ NM_ITEM,	NM_BARLABEL,			NULL,	0,	0,	NULL },
	{ NM_ITEM,	"Quit",					"Q",	0,	0,	(APTR)MNU_QUIT },
	{ NM_TITLE,	"Edit",					NULL,	0,	0,	NULL },
	{ NM_ITEM,	"Reset To Defaults",	"D",	0,	0,	(APTR)MNU_DEFAULTS },
	{ NM_ITEM,	"Last Saved",			"L",	0,	0,	(APTR)MNU_LASTSAVED },
	{ NM_ITEM,	"Restore",				"R",	0,	0,	(APTR)MNU_RESTORE },
	{ NM_TITLE,	"Presets",				NULL,	0,	0,	NULL },
	{ NM_ITEM,	"Normal",				"N",	0,	0,	(APTR)MNU_PRE_NORM },
	{ NM_ITEM,	"Fuzzy",				"F",	0,	0,	(APTR)MNU_PRE_FUZZ },
	{ NM_ITEM,	"XEN",					"X",	0,	0,	(APTR)MNU_PRE_XEN },
	{ NM_END,	NULL,					NULL,	0,	0,	NULL }
};


IPTR 	 prf_drawer,	//	= NULL,
		 prf_file;		//	= NULL;

// Spacing control
/*
ULONG	VSpNar1[] = { STRINGA_LongVal, SLIDER_Level, TAG_END },
		VSpNar2[] = { SLIDER_Level, STRINGA_LongVal, TAG_END },
		HSpNar1[] = { STRINGA_LongVal, SLIDER_Level, TAG_END },
		HSpNar2[] = { SLIDER_Level, STRINGA_LongVal, TAG_END },
		VSpNor1[] = { STRINGA_LongVal, SLIDER_Level, TAG_END },
		VSpNor2[] = { SLIDER_Level, STRINGA_LongVal, TAG_END },
		HSpNor1[] = { STRINGA_LongVal, SLIDER_Level, TAG_END },
		HSpNor2[] = { SLIDER_Level, STRINGA_LongVal, TAG_END },
		VSpWid1[] = { STRINGA_LongVal, SLIDER_Level, TAG_END },
		VSpWid2[] = { SLIDER_Level, STRINGA_LongVal, TAG_END },
		HSpWid1[] = { STRINGA_LongVal, SLIDER_Level, TAG_END },
		HSpWid2[] = { SLIDER_Level, STRINGA_LongVal, TAG_END };
*/

/*
**	SetGadgets2( void )
**
**	Update the gadgets of the user interface to
**	reflect the current settings.
*/
void
SetGadgets2( void )
{
	ULONG	frame = 0;

	SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VNOR_STR ], window, NULL, STRINGA_LongVal, grp_vspnormal, TAG_DONE);
	SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HNOR_STR ], window, NULL, STRINGA_LongVal, grp_hspnormal, TAG_DONE);

	// GROUP PAGE [1997-02-17]
	frame = Frame2Cycle( grp_frmtype );
	SetGadgetAttrs( ( struct Gadget * )ar[ BGP_GRP_FRMTYPE ], window, NULL, CYC_Active, frame, TAG_DONE);
	SetGadgetAttrs( ( struct Gadget * )ar[ BGP_GRP_SAMPLE ], window, NULL, FRM_Recessed, grp_recessed, TAG_DONE);
	SetGadgetAttrs( ( struct Gadget * )ar[ BGP_GRP_SAMPLE ], window, NULL, FRM_Type, grp_frmtype, TAG_DONE);
	if( grp_frmtype == FRTYPE_NONE )
		SetGadgetAttrs( ( struct Gadget * )ar[ BGP_GRP_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
	else
		SetGadgetAttrs( ( struct Gadget * )ar[ BGP_GRP_RECESSED ], window, NULL, GA_Selected, grp_recessed, TAG_DONE);

}

ULONG
GetFilename( int mode )
{
	Object				*filereq;
	IPTR 				 pf = (IPTR)NULL;

	if( !prf_drawer )
		prf_drawer = ( IPTR )&PrefDir;

	if( !mode ) {
		filereq = FileReqObject,
			ASLFR_TitleText, "Load BGUI Preferences",
			FRQ_Drawer, prf_drawer,
			FRQ_File, prf_file,
		EndObject;
	} else {
		filereq = FileReqObject,
			ASLFR_DoSaveMode, TRUE,
			ASLFR_TitleText, "Save BGUI Preferences",
			FRQ_Drawer, prf_drawer,
			FRQ_File, prf_file,
		EndObject;
	}

	if( filereq ) {

		if( !DoRequest( filereq ) ) {

			GetAttr( FRQ_Path, filereq, &pf );
			GetAttr( FRQ_File, filereq, &prf_file );
			GetAttr( FRQ_Drawer, filereq, &prf_drawer );

		}
	}
	return pf;

}

#ifdef __AROS__

struct IntuitionBase * IntuitionBase;
struct UtilityBase * UtilityBase;
struct Library * IFFParseBase;

BOOL openlibs(void)
{
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
  UtilityBase   = (struct UtilityBase   *)OpenLibrary("utility.library",0);
  IFFParseBase  =                         OpenLibrary("iffparse.library",0);
  
  if(IntuitionBase && UtilityBase && IFFParseBase)
    return TRUE;
  
  return FALSE;
}

void closelibs(void)
{
  if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
  if (UtilityBase) CloseLibrary((struct Library *)UtilityBase);
  if (IFFParseBase) CloseLibrary((struct Library *)IFFParseBase);
}

#endif

/*
**	int = main( int, char ** )
**
**	Main program starts and ends here.
*/
int main( int count, char **args )
{

	struct	bguiRequest	 AboutRequest = { };
	Object				*WO_Window;
	ULONG				 rc,
						 pf,
						 signal = 0,
						 frame = 0;
	BOOL				 running = TRUE;

#ifdef __AROS__
	if (FALSE == openlibs())
	{
	  closelibs();
	  printf("Failed to open lib!\n");
	  return -1;
	}
#endif

	// Open the library, at least version 41 required.
	if(( BGUIBase = OpenLibrary( "bgui.library", 41 ))) {

		// Get the default settings.
		GetDefaults();

		// Create the window object.
		if(( WO_Window = InitBGUIPrefs(INITIAL_PAGE) )) {

			// Open the window.
			if(( window = WindowOpen( WO_Window ) )) {

				// Get a signal for our window.
				GetAttr( WINDOW_SigMask, WO_Window, &signal );

				// Main event loop.
				do {

					// Wait for a signal.
					Wait( signal );

					// Process all events.
					while( ( rc = HandleEvent( WO_Window ) ) != WMHI_NOMORE ) {

						// What happened?
						switch( rc ) {

							// Save and use settings and exit.
							case	BGP_SAVE:

							// Use settings and exit.
							case	BGP_USE:
									if( BGP_SAVE == rc )
										SaveBGUIPrefs( "ENVARC:BGUI", "Default.prefs" );
									SaveBGUIPrefs( "ENV:BGUI", "Default.prefs" );
									running = FALSE;
									break;

							case	MNU_DEFAULTS:
									BGUI_DefaultPrefs();
									GetDefaults();
									SetGadgets2();
									break;

							// Save settings as
							case	MNU_OPEN:
									pf = GetFilename( 0 );
									if( pf ) {
										BGUI_LoadPrefs( ( UBYTE * )pf );
										GetDefaults();
										//grp_defaults	= BGUI_GetDefaultTags( BGUI_GROUP_GADGET );
										//grp_vspnormal	= GetTagData( GROUP_DefVSpaceNormal, grp_vspnormal, grp_defaults );
										//grp_hspnormal	= GetTagData( GROUP_DefHSpaceNormal, grp_hspnormal, grp_defaults );
										//SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VNOR_STR ], window, NULL, STRINGA_LongVal, grp_vspnormal, TAG_DONE);
										//SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HNOR_STR ], window, NULL, STRINGA_LongVal, grp_hspnormal, TAG_DONE);
										SetGadgets2();
									}
									break;

							// Save settings as
							case	MNU_SAVEAS:
									pf = GetFilename( 1 );
									if( pf )
										SaveBGUIPrefs( ( char * )prf_drawer, ( char * )prf_file );
									break;

							// We need to quit, right now.
							case	MNU_QUIT:
							case	BGP_CANCEL:
									running = FALSE;
									break;

							// User is searching for knowledge.
							case	MNU_ABOUT:
									if( !AboutRequest.br_Title ) {
										AboutRequest.br_Title         = "About " PRGNAME;
										AboutRequest.br_GadgetFormat  = "*_OK";
										AboutRequest.br_Underscore    = '_';
										AboutRequest.br_TextFormat    = ISEQ_C ISEQ_B VSTR ISEQ_N "\n----\nCopyright ©" _COPYRIGHTYEARS_ ", BGUI Developers Team\nAll rights reserved.";
										AboutRequest.br_Flags         = BREQF_AUTO_ASPECT|BREQF_FAST_KEYS|BREQF_LOCKWINDOW;
										AboutRequest.br_ReqPos        = POS_CENTERSCREEN;
									}
									BGUI_RequestA( window, &AboutRequest, NULL );
									break;


							// Handle page switching.
							case	BGP_LISTVIEW1:
									GetAttr( LISTV_LastClickedNum, ar[ BGP_LISTVIEW1 ], &frame );
									SetNewPage(window,frame);
									break;

							// SPACING PAGE
							case	BGP_SPC_HNAR_STR:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_HNAR_STR ], &grp_hspnarrow );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HNAR_SLI ], window, NULL, SLIDER_Level, grp_hspnarrow, TAG_DONE);
									break;
							case	BGP_SPC_HNAR_SLI:
									GetAttr( SLIDER_Level, ar[ BGP_SPC_HNAR_SLI ], &grp_hspnarrow );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HNAR_STR ], window, NULL, STRINGA_LongVal, grp_hspnarrow, TAG_DONE);
									break;
							case	BGP_SPC_VNAR_STR:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_VNAR_STR ], &grp_vspnarrow );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VNAR_SLI ], window, NULL, SLIDER_Level, grp_vspnarrow, TAG_DONE);
									break;
							case	BGP_SPC_VNAR_SLI:
									GetAttr( SLIDER_Level, ar[ BGP_SPC_VNAR_SLI ], &grp_vspnarrow );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VNAR_STR ], window, NULL, STRINGA_LongVal, grp_vspnarrow, TAG_DONE);
									break;
							case	BGP_SPC_HNOR_STR:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_HNOR_STR ], &grp_hspnormal );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HNOR_SLI ], window, NULL, SLIDER_Level, grp_hspnormal, TAG_DONE);
									break;
							case	BGP_SPC_HNOR_SLI:
									GetAttr( SLIDER_Level, ar[ BGP_SPC_HNOR_SLI ], &grp_hspnormal );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HNOR_STR ], window, NULL, STRINGA_LongVal, grp_hspnormal, TAG_DONE);
									break;
							case	BGP_SPC_VNOR_STR:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_VNOR_STR ], &grp_vspnormal );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VNOR_SLI ], window, NULL, SLIDER_Level, grp_vspnormal, TAG_DONE);
									break;
							case	BGP_SPC_VNOR_SLI:
									GetAttr( SLIDER_Level, ar[ BGP_SPC_VNOR_SLI ], &grp_vspnormal );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VNOR_STR ], window, NULL, STRINGA_LongVal, grp_vspnormal, TAG_DONE);
									break;
							case	BGP_SPC_HWID_STR:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_HWID_STR ], &grp_hspwide );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HWID_SLI ], window, NULL, SLIDER_Level, grp_hspwide, TAG_DONE);
									break;
							case	BGP_SPC_HWID_SLI:
									GetAttr( SLIDER_Level, ar[ BGP_SPC_HWID_SLI ], &grp_hspwide );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_HWID_STR ], window, NULL, STRINGA_LongVal, grp_hspwide, TAG_DONE);
									break;
							case	BGP_SPC_VWID_STR:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_VWID_STR ], &grp_vspwide );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VWID_SLI ], window, NULL, SLIDER_Level, grp_vspwide, TAG_DONE);
									break;
							case	BGP_SPC_VWID_SLI:
									GetAttr( SLIDER_Level, ar[ BGP_SPC_VWID_SLI ], &grp_vspwide );
									SetGadgetAttrs( ( struct Gadget * )ar[ BGP_SPC_VWID_STR ], window, NULL, STRINGA_LongVal, grp_vspwide, TAG_DONE);
									break;
/*
							case	BGP_SPC_VNAR_STR:
							case	BGP_SPC_VNAR_SLI:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_VNAR_STR ], &grp_vspnarrow );
									break;
							case	BGP_SPC_HNOR_STR:
							case	BGP_SPC_HNOR_SLI:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_HNOR_STR ], &grp_hspnormal );
									break;
							case	BGP_SPC_VNOR_STR:
							case	BGP_SPC_VNOR_SLI:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_VNOR_STR ], &grp_vspnormal );
									break;
							case	BGP_SPC_HWID_STR:
							case	BGP_SPC_HWID_SLI:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_HWID_STR ], &grp_hspwide );
									break;
							case	BGP_SPC_VWID_STR:
							case	BGP_SPC_VWID_SLI:
									GetAttr( STRINGA_LongVal, ar[ BGP_SPC_VWID_STR ], &grp_vspwide );
									break;
*/

							// GROUP PAGE
							case	BGP_GRP_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_GRP_RECESSED ], &grp_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_GRP_SAMPLE ], window, NULL, FRM_Recessed, grp_recessed, TAG_DONE);
									break;
							case	BGP_GRP_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_GRP_FRMTYPE ], &frame );
									grp_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_GRP_SAMPLE ], window, NULL, FRM_Type, grp_frmtype, TAG_DONE);
									if( grp_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_GRP_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_GRP_RECESSED ], window, NULL, GA_Selected, grp_recessed, TAG_DONE);
									break;

							// BUTTON PAGE
							case	BGP_BUT_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_BUT_RECESSED ], &but_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_BUT_SAMPLE ], window, NULL, FRM_Recessed, but_recessed, TAG_DONE);
									break;
							case	BGP_BUT_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_BUT_FRMTYPE ], &frame );
									but_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_BUT_SAMPLE ], window, NULL, FRM_Type, but_frmtype, TAG_DONE);
									if( but_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_BUT_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_BUT_RECESSED ], window, NULL, GA_Selected, but_recessed, TAG_DONE);
									break;

							// STRING PAGE
							case	BGP_STR_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_STR_RECESSED ], &str_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_STR_SAMPLE ], window, NULL, FRM_Recessed, str_recessed, TAG_DONE);
									break;
							case	BGP_STR_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_STR_FRMTYPE ], &frame );
									str_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_STR_SAMPLE ], window, NULL, FRM_Type, str_frmtype, TAG_DONE);
									if( str_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_STR_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_STR_RECESSED ], window, NULL, GA_Selected, str_recessed, TAG_DONE);
									break;

							// CHECKBOX PAGE
							case	BGP_CHK_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_CHK_RECESSED ], &chk_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_CHK_SAMPLE ], window, NULL, FRM_Recessed, chk_recessed, TAG_DONE);
									break;
							case	BGP_CHK_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_CHK_FRMTYPE ], &frame );
									chk_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_CHK_SAMPLE ], window, NULL, FRM_Type, chk_frmtype, TAG_DONE);
									if( chk_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_CHK_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_CHK_RECESSED ], window, NULL, GA_Selected, chk_recessed, TAG_DONE);
									break;

							// CYCLE PAGE
							case	BGP_CYC_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_CYC_RECESSED ], &cyc_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_SAMPLE ], window, NULL, FRM_Recessed, cyc_recessed, TAG_DONE);
									break;
							case	BGP_CYC_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_CYC_FRMTYPE ], &frame );
									cyc_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_SAMPLE ], window, NULL, FRM_Type, cyc_frmtype, TAG_DONE);
									if( cyc_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_RECESSED ], window, NULL, GA_Selected, cyc_recessed, TAG_DONE);
									break;
							case	BGP_CYC_POPUP:
									GetAttr( GA_Selected, ar[ BGP_CYC_POPUP ], &cyc_popup );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_SAMPLE ], window, NULL, CYC_Popup, cyc_popup, TAG_DONE);
									if( 0 == cyc_popup )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_POPACTIVE ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_POPACTIVE ], window, NULL, GA_Selected, cyc_popactive, TAG_DONE);
									break;
							case	BGP_CYC_POPACTIVE:
									GetAttr( GA_Selected, ar[ BGP_CYC_POPACTIVE ], &cyc_popactive );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_CYC_SAMPLE ], window, NULL, CYC_PopActive, cyc_popactive, TAG_DONE);
									break;

							// INFO PAGE
							case	BGP_INF_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_INF_RECESSED ], &inf_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_INF_SAMPLE ], window, NULL, FRM_Recessed, inf_recessed, TAG_DONE);
									break;
							case	BGP_INF_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_INF_FRMTYPE ], &frame );
									inf_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_INF_SAMPLE ], window, NULL, FRM_Type, inf_frmtype, TAG_DONE);
									if( inf_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_INF_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_INF_RECESSED ], window, NULL, GA_Selected, inf_recessed, TAG_DONE);
									break;

							// PROGRESS PAGE
							case	BGP_PRG_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_PRG_RECESSED ], &prg_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_PRG_SAMPLE ], window, NULL, FRM_Recessed, prg_recessed, TAG_DONE);
									break;
							case	BGP_PRG_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_PRG_FRMTYPE ], &frame );
									prg_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_PRG_SAMPLE ], window, NULL, FRM_Type, prg_frmtype, TAG_DONE);
									if( prg_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_PRG_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_PRG_RECESSED ], window, NULL, GA_Selected, prg_recessed, TAG_DONE);
									break;

							// INDICATOR PAGE
							case	BGP_IND_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_IND_RECESSED ], &ind_recessed );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_IND_SAMPLE ], window, NULL, FRM_Recessed, ind_recessed, TAG_DONE);
									break;
							case	BGP_IND_FRMTYPE:
									GetAttr( CYC_Active, ar[ BGP_IND_FRMTYPE ], &frame );
									ind_frmtype = Cycle2Frame[ frame ];
									SetGadgetAttrs((struct Gadget *)ar[ BGP_IND_SAMPLE ], window, NULL, FRM_Type, ind_frmtype, TAG_DONE);
									if( ind_frmtype == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_IND_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_IND_RECESSED ], window, NULL, GA_Selected, ind_recessed, TAG_DONE);
									break;

/*
							// SLIDER PAGE
							case	BGP_SLI_RECESSED:
									GetAttr( GA_Selected, ar[ BGP_SLI_RECESSED ], &frame );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_SLI_SAMPLE ], window, NULL, FRM_Recessed, frame, TAG_DONE);
									break;
							case	BGP_SLIFRAME:
									GetAttr( CYC_Active, ar[ BGP_SLIFRAME ], &frame );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_SLI_SAMPLE ], window, NULL, FRM_Type, Cycle2Frame[ frame ], TAG_DONE);
									if( Cycle2Frame[ frame ] == FRTYPE_NONE )
										SetGadgetAttrs((struct Gadget *)ar[ BGP_SLI_RECESSED ], window, NULL, GA_Selected, 0, TAG_DONE);
									else
										SetGadgetAttrs((struct Gadget *)ar[ BGP_SLI_RECESSED ], window, NULL, GA_Selected, frame, TAG_DONE);
									break;
							case	BGP_SLINLOOK:
									GetAttr( GA_Selected, ar[ BGP_SLINLOOK ], &frame );
									SetGadgetAttrs((struct Gadget *)ar[ BGP_SLI_SAMPLE ], window, NULL, PGA_NewLook, frame, TAG_DONE);
									break;
*/

						}

					}

				} while( running );

			}

			// Dispose (automatically close) our window object.
			DisposeObject( WO_Window );

		}

		// Close the library when we're done with it.
		CloseLibrary( BGUIBase );
	}

#ifdef __AROS__
	closelibs();
#endif

	// Return error code.
	return( 0 );

}
