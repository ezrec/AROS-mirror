/*
 * @(#) $Header$
 *
 * BGUI Prefs Editor
 * BGUIPrefs_gui.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Respect Software
 * (C) Copyright 1996-1997 Ian J. Einman.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:17:00  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:29:50  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:56:50  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.4  1999/08/30 05:02:21  mlemos
 * Updated the copyright and the version information.
 *
 * Revision 1.1.2.3  1999/08/30 00:25:38  mlemos
 * Removed window creation timing code.
 * Removed GRM_REPLACEMEMBER test code.
 * Made the relayout of the added pages be done after the preferences and
 * example pages are added.
 *
 * Revision 1.1.2.2  1999/08/29 17:11:10  mlemos
 * Made the preferences and example pages be created and added on demand.
 *
 * Revision 1.1.2.1  1998/09/20 17:28:45  mlemos
 * Peter Bornhall sources.
 *
 *
 *
 */

// GROUP DEFAULTS
extern	ULONG		 grp_frmtype,
					 grp_recessed,
					 grp_vspnarrow,
					 grp_hspnarrow,
					 grp_vspnormal,
					 grp_hspnormal,
					 grp_vspwide,
					 grp_hspwide;

// INFO DEFAULTS
extern	ULONG		 inf_frmtype,
					 inf_recessed;

// INDICATOR DEFAULTS
extern	ULONG		 ind_frmtype,
					 ind_recessed;

extern	struct	NewMenu		Menus[];

enum {
	BGP_MASTER,
	BGP_LISTVIEW1,
	BGP_MAIN_PAGES,
	BGP_GENERAL_PAGE, 
	BGP_GEN_TOOLTIPS,
	BGP_GEN_TICKS,
	BGP_GEN_REFRESH,
	BGP_GEN_BUFFER, 
	BGP_GEN_SIZING,
	BGP_SPACING_PAGE,
	BGP_SPC_HNAR_STR,
	BGP_SPC_HNAR_SLI,
	BGP_SPC_VNAR_STR,
	BGP_SPC_VNAR_SLI,
	BGP_SPC_HNOR_STR,
	BGP_SPC_HNOR_SLI, 
	BGP_SPC_VNOR_STR,
	BGP_SPC_VNOR_SLI,
	BGP_SPC_HWID_STR,
	BGP_SPC_HWID_SLI,
	BGP_SPC_VWID_STR,
	BGP_SPC_VWID_SLI,
	BGP_MISC_PAGE,
	BGP_SETTING_PAGES, 
	BGP_GROUP_PAGE1,
	BGP_GRP_FRMTYPE,
	BGP_GRP_RECESSED,
	BGP_BUTTON_PAGE1,
	BGP_BUT_FRMTYPE,
	BGP_BUT_RECESSED,
	BGP_STRING_PAGE1,
	BGP_STR_FRMTYPE,
	BGP_STR_RECESSED,
	BGP_CHECKBOX_PAGE1,
	BGP_CHK_FRMTYPE,
	BGP_CHK_RECESSED,
	BGP_CYCLE_PAGE1,
	BGP_CYC_FRMTYPE,
	BGP_CYC_RECESSED, 
	BGP_CYC_POPUP,
	BGP_CYC_POPACTIVE,
	BGP_INFO_PAGE1,
	BGP_INF_FRMTYPE,
	BGP_INF_RECESSED,
	BGP_PROGRESS_PAGE1,
	BGP_PRG_FRMTYPE,
	BGP_PRG_RECESSED, 
	BGP_INDICATOR_PAGE1,
	BGP_IND_FRMTYPE,
	BGP_IND_RECESSED,
	BGP_SLIDER_PAGE1, 
	BGP_TEMP1,
	BGP_PROPORTIONAL_PAGE1,
	BGP_TEMP2,
	BGP_MX_PAGE1,
	BGP_TEMP3,
	BGP_LISTVIEW_PAGE1,
	BGP_TEMP4,
	BGP_EXAMPLE_PAGES,
	BGP_GROUP_PAGE2,
	BGP_GRP_SAMPLE,
	BGP_BUTTON_PAGE2,
	BGP_BUT_SAMPLE,
	BGP_STRING_PAGE2,
	BGP_STR_SAMPLE,
	BGP_CHECKBOX_PAGE2,
	BGP_CHK_SAMPLE,
	BGP_CYCLE_PAGE2,
	BGP_CYC_SAMPLE,
	BGP_INFO_PAGE2,
	BGP_INF_SAMPLE,
	BGP_PROGRESS_PAGE2,
	BGP_PRG_SAMPLE, 
	BGP_INDICATOR_PAGE2,
	BGP_IND_SAMPLE,
	BGP_SLIDER_PAGE2,
	BGP_SLI_SAMPLE, 
	BGP_PROPORTIONAL_PAGE2,
	BGP_PRO_SAMPLE,
	BGP_MX_PAGE2,
	BGP_MX_SAMPLE, 
	BGP_LISTVIEW_PAGE2,
	BGP_LST_SAMPLE,
	BGP_SAVE, 
	BGP_USE,
	BGP_CANCEL, 
	BGP_NUMGADS
};

Object *BGUIPrefsObjs[BGP_NUMGADS];

UBYTE *BGP_LISTVIEW1Entrys[] = {
	"General",
	"Spacing",
	"Group",
	"Button",
	"String",
	"Checkbox",
	"Cycle",
	"Info",
	"Progress",
	"Indicator",
	"(Slider)",
	"(Proportional)",
	"(MX)",
	"(Listview)",
	NULL
};

UBYTE *BGP_FRAMELabels[] = {
	"Button",
	"Fuzz Button",
	"Xen Button",
	"Ridge",
	"Fuzz Ridge",
	"Drop Box",
	"NeXT",
	"None",
	NULL
};

UBYTE *BGP_CYC_SAMPLELabels[] = {
	"#1",
	"#2",
	"#3",
	"#4",
	"#5",
	NULL
};

UBYTE *BGP_MX_SAMPLELabels[] = {
	"Entry 1",
	"Entry 2",
	NULL
};

UBYTE *BGP_LST_SAMPLEEntrys[] = {
	"Entry 1",
	"Entry 2",
	"Entry 3",
	"Entry 4",
	"Entry 5",
	NULL
};

UBYTE *BGP_GEN_REFRESHLabels[] = {
	"Simple",
	"Smart",
	NULL
};

UBYTE *BGP_GEN_BUFFERLabels[] = {
	"Standard",
	"No Buffering",
	NULL
};

UBYTE *BGP_GEN_SIZINGLabels[] = {
	"Bottom",
	"Left",
	NULL
};

// DEFAULTS
struct	TagItem	*grp_defaults,			// BGUI_GROUP_GADGET
				*but_defaults,			// BGUI_BUTTON_GADGET
				*str_defaults,			// BGUI_STRING_GADGET
				*chk_defaults,			// BGUI_CHECKBOX_GADGET
				*cyc_defaults,			// BGUI_CYCLE_GADGET
				*inf_defaults,			// BGUI_INFO_GADGET
				*prg_defaults,			// BGUI_PROGRESS_GADGET
				*ind_defaults;			// BGUI_INDICATOR_GADGET

// GROUP DEFAULTS
ULONG			 grp_frmtype	= 4,	// FRM_DefaultType
				 grp_recessed	= 0,	// FRM_Recessed
				 grp_vspnarrow	= 2,	// GROUP_DefVSpaceNarrow
				 grp_hspnarrow	= 2,	// GROUP_DefHSpaceNarrow
				 grp_vspnormal	= 4,	// GROUP_DefVSpaceNormal
				 grp_hspnormal	= 4,	// GROUP_DefHSpaceNormal
				 grp_vspwide	= 8,	// GROUP_DefVSpaceWide
				 grp_hspwide	= 8;	// GROUP_DefHSpaceWide

// BUTTON DEFAULTS
ULONG			 but_frmtype	= 1,	// FRM_Type
				 but_recessed	= 0;	// FRM_Recessed

// STRING DEFAULTS
ULONG			 str_frmtype	= 2,	// FRM_Type
				 str_recessed	= 0;	// FRM_Recessed

// CHECKBOX DEFAULTS
ULONG			 chk_frmtype	= 1,	// FRM_Type
				 chk_recessed	= 0;	// FRM_Recessed

// CYCLE DEFAULTS
ULONG			 cyc_frmtype	= 1,	// FRM_Type
				 cyc_recessed	= 0,	// FRM_Recessed
				 cyc_popup		= 0,	// CYC_Popup
				 cyc_popactive	= 0;	// CYC_PopActive

// INFO DEFAULTS
ULONG			 inf_frmtype	= 1,	// FRM_Type
				 inf_recessed	= 1;	// FRM_Recessed

// PROGRESS DEFAULTS
ULONG			 prg_frmtype	= 1,	// FRM_Type
				 prg_recessed	= 1;	// FRM_Recessed

// INDICATOR DEFAULTS
ULONG			 ind_frmtype	= 1,	// FRM_Type
				 ind_recessed	= 1;	// FRM_Recessed

/*
**	GetDefaults( void )
**
**	Get the defaults at startup.  These might be the
**	real defaults, or defaults previously stored by
**	the user.
*/
void
GetDefaults( void )
{
	// group defaults
	grp_defaults	= BGUI_GetDefaultTags( BGUI_GROUP_GADGET );
	grp_frmtype		= GetTagData( FRM_DefaultType, grp_frmtype, grp_defaults );
	grp_recessed	= GetTagData( FRM_Recessed, grp_recessed, grp_defaults );
	grp_vspnarrow	= GetTagData( GROUP_DefVSpaceNarrow, grp_vspnarrow, grp_defaults );
	grp_hspnarrow	= GetTagData( GROUP_DefHSpaceNarrow, grp_hspnarrow, grp_defaults );
	grp_vspnormal	= GetTagData( GROUP_DefVSpaceNormal, grp_vspnormal, grp_defaults );
	grp_hspnormal	= GetTagData( GROUP_DefHSpaceNormal, grp_hspnormal, grp_defaults );
	grp_vspwide		= GetTagData( GROUP_DefVSpaceWide, grp_vspwide, grp_defaults );
	grp_hspwide		= GetTagData( GROUP_DefHSpaceWide, grp_hspwide, grp_defaults );

	// button defaults
	but_defaults	= BGUI_GetDefaultTags( BGUI_BUTTON_GADGET );
	but_frmtype		= GetTagData( FRM_Type, but_frmtype, but_defaults );
	but_recessed	= GetTagData( FRM_Recessed, but_recessed, but_defaults );

	// string defaults
	str_defaults	= BGUI_GetDefaultTags( BGUI_STRING_GADGET );
	str_frmtype		= GetTagData( FRM_Type, str_frmtype, str_defaults );
	str_recessed	= GetTagData( FRM_Recessed, str_recessed, str_defaults );

	// checkbox defaults
	chk_defaults	= BGUI_GetDefaultTags( BGUI_CHECKBOX_GADGET );
	chk_frmtype		= GetTagData( FRM_Type, chk_frmtype, chk_defaults );
	chk_recessed	= GetTagData( FRM_Recessed, chk_recessed, chk_defaults );

	// cycle defaults
	cyc_defaults	= BGUI_GetDefaultTags( BGUI_CYCLE_GADGET );
	cyc_frmtype		= GetTagData( FRM_Type, cyc_frmtype, cyc_defaults );
	cyc_recessed	= GetTagData( FRM_Recessed, cyc_recessed, cyc_defaults );
	cyc_popup		= GetTagData( CYC_Popup, cyc_popup, cyc_defaults );
	cyc_popactive	= GetTagData( CYC_PopActive, cyc_popactive, cyc_defaults );

	// info defaults
	inf_defaults	= BGUI_GetDefaultTags( BGUI_INFO_GADGET );
	inf_frmtype		= GetTagData( FRM_DefaultType, inf_frmtype, inf_defaults );
	inf_recessed	= GetTagData( FRM_Recessed, inf_recessed, inf_defaults );

	// progress defaults
	prg_defaults	= BGUI_GetDefaultTags( BGUI_PROGRESS_GADGET );
	prg_frmtype		= GetTagData( FRM_Type, prg_frmtype, prg_defaults );
	prg_recessed	= GetTagData( FRM_Recessed, prg_recessed, prg_defaults );

	// indicator defaults
	ind_defaults	= BGUI_GetDefaultTags( BGUI_INDICATOR_GADGET );
	ind_frmtype		= GetTagData( FRM_DefaultType, ind_frmtype, ind_defaults );
	ind_recessed	= GetTagData( FRM_Recessed, ind_recessed, ind_defaults );
}

static Object *PreferencesPage(ULONG page)
{
	Object **ar = BGUIPrefsObjs;

	switch(page)
	{
		case PAGE_GENERAL:
			if(ar[BGP_GENERAL_PAGE]==NULL)
			{

// GENERAL
				ar[BGP_GENERAL_PAGE] = VGroupObject,
							NormalSpacing,
							NormalHOffset,
							BOffset( GRSPACE_NORMAL ),
							FRM_Type, FRTYPE_DEFAULT,
							//NeXTFrame,
							FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
							FRM_Title, "General Settings",
							VarSpace (50),
							StartMember,
								VGroupObject,
									NormalSpacing,
									NormalHOffset,
									EqualHeight,
									StartMember,
										HGroupObject,
											NormalSpacing,
// GENERAL: Tooltips
											StartMember,
												ar[BGP_GEN_TOOLTIPS] = CheckBoxObject,
													LAB_Label, "_Tooltips",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_GEN_TOOLTIPS,
												EndObject,
												FixMinWidth,
											EndMember,
											VarSpace (50),
// GENERAL: Ticks
											StartMember,
												ar[BGP_GEN_TICKS] = StringObject,
													LAB_Label, "T_icks",
													LAB_Place, PLACE_LEFT,
													STRINGA_TextVal, "",
													STRINGA_MaxChars, 65,
													STRINGA_MinCharsVisible, 0,
													GA_ID, BGP_GEN_TICKS,
												EndObject,
												FixMinWidth,
											EndMember,
										EndObject,
									EndMember,
// GENERAL: Refreshmode
									StartMember,
										ar[BGP_GEN_REFRESH] = CycleObject,
											LAB_Label, "_Refreshmode",
											CYC_Labels, BGP_GEN_REFRESHLabels,
											GA_ID, BGP_GEN_REFRESH,
										EndObject,
									EndMember,
// GENERAL: Buffering
									StartMember,
										ar[BGP_GEN_BUFFER] = CycleObject,
											LAB_Label, "_Buffering",
											CYC_Labels, BGP_GEN_BUFFERLabels,
											GA_ID, BGP_GEN_BUFFER,
										EndObject,
									EndMember,
// GENERAL: Size gadget
									StartMember,
										ar[BGP_GEN_SIZING] = CycleObject,
											LAB_Label, "Size _Gadget",
											CYC_Labels, BGP_GEN_SIZINGLabels,
											GA_ID, BGP_GEN_SIZING,
										EndObject,
									EndMember,
								EndObject,
								FixMinHeight,
							EndMember,
							VarSpace (50),
						EndObject;

			}
			return(ar[BGP_GENERAL_PAGE]);

		case PAGE_SPACING:
			if(ar[BGP_SPACING_PAGE]==NULL)
			{

// SPACING
				ar[BGP_SPACING_PAGE] = VGroupObject,
							NormalSpacing,
							StartMember,
								VGroupObject,
									NormalSpacing,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Narrow Spacing",
									VarSpace (50),
// SPACING: Narrow H
									StartMember,
										HGroupObject,
											NormalHOffset,
											StartMember,
												ar[BGP_SPC_HNAR_STR] = StringObject,
													LAB_Label, "Horizontal",
													LAB_Place, PLACE_LEFT,
													STRINGA_LongVal, grp_hspnarrow,
													STRINGA_IntegerMin, 0,
													STRINGA_IntegerMax, 64,
													STRINGA_MaxChars, 2,
													STRINGA_MinCharsVisible, 3,
													STRINGA_Justification, GACT_STRINGRIGHT,
													GA_ID, BGP_SPC_HNAR_STR,
												EndObject,
												FixMinWidth,
											EndMember,
											StartMember,
												ar[BGP_SPC_HNAR_SLI] = SliderObject,
													SLIDER_Level, grp_hspnarrow,
													SLIDER_Max, 64,
													PGA_NewLook, TRUE,
													GA_ID, BGP_SPC_HNAR_SLI,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
// SPACING: Narrow V
									StartMember,
										HGroupObject,
											NormalHOffset,
											StartMember,
												ar[BGP_SPC_VNAR_STR] = StringObject,
													LAB_Label, "Vertical",
													LAB_Place, PLACE_LEFT,
													STRINGA_LongVal, grp_vspnarrow,
													STRINGA_IntegerMin, 0,
													STRINGA_IntegerMax, 64,
													STRINGA_MaxChars, 2,
													STRINGA_MinCharsVisible, 3,
													STRINGA_Justification, GACT_STRINGRIGHT,
													GA_ID, BGP_SPC_VNAR_STR,
												EndObject,
												FixMinWidth,
											EndMember,
											StartMember,
												ar[BGP_SPC_VNAR_SLI] = SliderObject,
													SLIDER_Level, grp_vspnarrow,
													SLIDER_Max, 64,
													PGA_NewLook, TRUE,
													GA_ID, BGP_SPC_VNAR_SLI,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject,
							EndMember,
							StartMember,
								VGroupObject,
									NormalSpacing,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Normal Spacing",
									VarSpace (50),
// SPACING: Normal H
									StartMember,
										HGroupObject,
											NormalHOffset,
											StartMember,
												ar[BGP_SPC_HNOR_STR] = StringObject,
													LAB_Label, "Horizontal",
													LAB_Place, PLACE_LEFT,
													STRINGA_LongVal, grp_hspnormal,
													STRINGA_IntegerMin, 0,
													STRINGA_IntegerMax, 64,
													STRINGA_MaxChars, 2,
													STRINGA_MinCharsVisible, 3,
													STRINGA_Justification, GACT_STRINGRIGHT,
													GA_ID, BGP_SPC_HNOR_STR,
												EndObject,
												FixMinWidth,
											EndMember,
											StartMember,
												ar[BGP_SPC_HNOR_SLI] = SliderObject,
													SLIDER_Level, grp_hspnormal,
													SLIDER_Max, 64,
													PGA_NewLook, TRUE,
													GA_ID, BGP_SPC_HNOR_SLI,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
// SPACING: Normal V
									StartMember,
										HGroupObject,
											NormalHOffset,
											StartMember,
												ar[BGP_SPC_VNOR_STR] = StringObject,
													LAB_Label, "Vertical",
													LAB_Place, PLACE_LEFT,
													STRINGA_LongVal, grp_vspnormal,
													STRINGA_IntegerMin, 0,
													STRINGA_IntegerMax, 64,
													STRINGA_MaxChars, 2,
													STRINGA_MinCharsVisible, 3,
													STRINGA_Justification, GACT_STRINGRIGHT,
													GA_ID, BGP_SPC_VNOR_STR,
												EndObject,
												FixMinWidth,
											EndMember,
											StartMember,
												ar[BGP_SPC_VNOR_SLI] = SliderObject,
													SLIDER_Level, grp_vspnormal,
													SLIDER_Max, 64,
													PGA_NewLook, TRUE,
													GA_ID, BGP_SPC_VNOR_SLI,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject,
							EndMember,
							StartMember,
								VGroupObject,
									NormalSpacing,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Wide Spacing",
									VarSpace (50),
// SPACING: Wide H
									StartMember,
										HGroupObject,
											NormalHOffset,
											StartMember,
												ar[BGP_SPC_HWID_STR] = StringObject,
													LAB_Label, "Horizontal",
													LAB_Place, PLACE_LEFT,
													STRINGA_LongVal, grp_hspwide,
													STRINGA_IntegerMin, 0,
													STRINGA_IntegerMax, 64,
													STRINGA_MaxChars, 2,
													STRINGA_MinCharsVisible, 3,
													STRINGA_Justification, GACT_STRINGRIGHT,
													GA_ID, BGP_SPC_HWID_STR,
												EndObject,
												FixMinWidth,
											EndMember,
											StartMember,
												ar[BGP_SPC_HWID_SLI] = SliderObject,
													SLIDER_Level, grp_hspwide,
													SLIDER_Max, 64,
													PGA_NewLook, TRUE,
													GA_ID, BGP_SPC_HWID_SLI,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
// SPACING: Wide V
									StartMember,
										HGroupObject,
											NormalHOffset,
											StartMember,
												ar[BGP_SPC_VWID_STR] = StringObject,
													LAB_Label, "Vertical",
													LAB_Place, PLACE_LEFT,
													STRINGA_LongVal, grp_vspwide,
													STRINGA_IntegerMin, 0,
													STRINGA_IntegerMax, 64,
													STRINGA_MaxChars, 2,
													STRINGA_MinCharsVisible, 3,
													STRINGA_Justification, GACT_STRINGRIGHT,
													GA_ID, BGP_SPC_VWID_STR,
												EndObject,
												FixMinWidth,
											EndMember,
											StartMember,
												ar[BGP_SPC_VWID_SLI] = SliderObject,
													SLIDER_Level, grp_vspwide,
													SLIDER_Max, 64,
													PGA_NewLook, TRUE,
													GA_ID, BGP_SPC_VWID_SLI,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject,
							EndMember,
						EndObject;

			}
			return(ar[BGP_SPACING_PAGE]);

		case PAGE_GROUP:
			if(ar[BGP_GROUP_PAGE1]==NULL)
			{

// MISC: GROUP PAGE
				ar[BGP_GROUP_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Group Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_GRP_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_GRP_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_GRP_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_GRP_RECESSED,
												EndObject,
												FixMinWidth,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_GROUP_PAGE1]);

		case PAGE_BUTTON:
			if(ar[BGP_BUTTON_PAGE1]==NULL)
			{

// MISC: BUTTON PAGE
				ar[BGP_BUTTON_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Button Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_BUT_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_BUT_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_BUT_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_BUT_RECESSED,
												EndObject,
												FixMinWidth,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_BUTTON_PAGE1]);

		case PAGE_STRING:
			if(ar[BGP_STRING_PAGE1]==NULL)
			{

// MISC: STRING PAGE
				ar[BGP_STRING_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "String Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_STR_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_STR_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_STR_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_STR_RECESSED,
												EndObject,
												FixMinWidth,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_STRING_PAGE1]);

		case PAGE_CHECKBOX:
			if(ar[BGP_CHECKBOX_PAGE1]==NULL)
			{

// MISC: CHECKBOX PAGE
				ar[BGP_CHECKBOX_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Checkbox Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_CHK_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_CHK_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_CHK_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_CHK_RECESSED,
												EndObject,
												FixMinWidth,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_CHECKBOX_PAGE1]);

		case PAGE_CYCLE:
			if(ar[BGP_CYCLE_PAGE1]==NULL)
			{

// MISC: CYCLE PAGE
				ar[BGP_CYCLE_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Cycle Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_CYC_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_CYC_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_CYC_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_CYC_RECESSED,
												EndObject,
												FixMinWidth,
											EndMember,
											StartMember,
												HGroupObject,
													NormalSpacing,
													StartMember,
														ar[BGP_CYC_POPUP] = CheckBoxObject,
															LAB_Label, "Popup",
															LAB_Place, PLACE_LEFT,
															GA_ID, BGP_CYC_POPUP,
														EndObject,
														FixMinWidth,
													EndMember,
													VarSpace (50),
													StartMember,
														ar[BGP_CYC_POPACTIVE] = CheckBoxObject,
															LAB_Label, "Pop Active",
															LAB_Place, PLACE_LEFT,
															GA_ID, BGP_CYC_POPACTIVE,
														EndObject,
														FixMinWidth,
													EndMember,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_CYCLE_PAGE1]);

		case PAGE_INFO:
			if(ar[BGP_INFO_PAGE1]==NULL)
			{

// MISC: INFO PAGE
				ar[BGP_INFO_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Info Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_INF_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_INF_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_INF_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_INF_RECESSED,
												EndObject,	
												FixMinWidth,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_INFO_PAGE1]);

		case PAGE_PROGRESS:
			if(ar[BGP_PROGRESS_PAGE1]==NULL)
			{

// MISC: PROGRESS PAGE
				ar[BGP_PROGRESS_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Progress Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_PRG_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_PRG_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_PRG_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_PRG_RECESSED,
												EndObject,
												FixMinWidth,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;
			}
			return(ar[BGP_PROGRESS_PAGE1]);

		case PAGE_INDICATOR:
			if(ar[BGP_INDICATOR_PAGE1]==NULL)
			{

// MISC: INDICATOR PAGE
				ar[BGP_INDICATOR_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Indicator Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											EqualHeight,
											StartMember,
												ar[BGP_IND_FRMTYPE] = CycleObject,
													LAB_Label, "_Frame Type",
													CYC_Labels, BGP_FRAMELabels,
													GA_ID, BGP_IND_FRMTYPE,
												EndObject,
											EndMember,
											StartMember,
												ar[BGP_IND_RECESSED] = CheckBoxObject,
													LAB_Label, "_Recessed",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_IND_RECESSED,
												EndObject,
												FixMinWidth,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;
			}
			return(ar[BGP_INDICATOR_PAGE1]);

		case PAGE_SLIDER:
			if(ar[BGP_SLIDER_PAGE1]==NULL)
			{

// MISC: SLIDER PAGE
				ar[BGP_SLIDER_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Slider Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											StartMember,
												ar[BGP_TEMP1] = ButtonObject,
													LAB_Label, "Button",
													GA_ID, BGP_TEMP1,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;
			}
			return(ar[BGP_SLIDER_PAGE1]);

		case PAGE_PROPORTIONAL:
			if(ar[BGP_PROPORTIONAL_PAGE1]==NULL)
			{

// MISC: PROPORTIONAL PAGE
				ar[BGP_PROPORTIONAL_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Proportional Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											StartMember,
												ar[BGP_TEMP2] = ButtonObject,
													LAB_Label, "Button",
													GA_ID, BGP_TEMP2,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_PROPORTIONAL_PAGE1]);

		case PAGE_MX:
			if(ar[BGP_MX_PAGE1]==NULL)
			{

// MISC: MX PAGE
				ar[BGP_MX_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "MX Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											StartMember,
												ar[BGP_TEMP3] = ButtonObject,
													LAB_Label, "Button",
													GA_ID, BGP_TEMP3,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_MX_PAGE1]);

		case PAGE_LISTVIEW:
			if(ar[BGP_LISTVIEW_PAGE1]==NULL)
			{

// MISC: LISTVIEW PAGE
				ar[BGP_LISTVIEW_PAGE1] = VGroupObject,
									NormalHOffset,
									BOffset( GRSPACE_NORMAL ),
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Listview Settings",
									VarSpace (50),
									StartMember,
										VGroupObject,
											NormalSpacing,
											NormalHOffset,
											StartMember,
												ar[BGP_TEMP4] = ButtonObject,
													LAB_Label, "Button",
													GA_ID, BGP_TEMP4,
												EndObject,
											EndMember,
										EndObject,
										FixMinHeight,
									EndMember,
									VarSpace (50),
								EndObject;

			}
			return(ar[BGP_LISTVIEW_PAGE1]);

	}
}

static Object *ExamplesPage(ULONG page)
{
	Object **ar = BGUIPrefsObjs;

	switch(page)
	{
		case PAGE_GROUP:
			if(ar[BGP_GROUP_PAGE2]==NULL)
			{

// EXAMPLES: GROUP SAMPLE
				ar[BGP_GROUP_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Group",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_GRP_SAMPLE] = InfoObject,
													FRM_Type, grp_frmtype,
													FRM_Recessed, grp_recessed,
													INFO_TextFormat, " ",
													INFO_HorizOffset, 4,
													INFO_VertOffset, 4,
													GA_ID, BGP_GRP_SAMPLE,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_GROUP_PAGE2]);

		case PAGE_BUTTON:
			if(ar[BGP_BUTTON_PAGE2]==NULL)
			{

// EXAMPLES: BUTTON SAMPLE
				ar[BGP_BUTTON_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Button",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_BUT_SAMPLE] = ButtonObject,
													LAB_Label, "Button",
													GA_ID, BGP_BUT_SAMPLE,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_BUTTON_PAGE2]);

		case PAGE_STRING:
			if(ar[BGP_STRING_PAGE2]==NULL)
			{

// EXAMPLES: STRING SAMPLE
				ar[BGP_STRING_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example String",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_STR_SAMPLE] = StringObject,
													STRINGA_TextVal, "String",
													STRINGA_MaxChars, 65,
													STRINGA_MinCharsVisible, 0,
													STRINGA_Justification, GACT_STRINGCENTER,
													GA_ID, BGP_STR_SAMPLE,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_STRING_PAGE2]);

		case PAGE_CHECKBOX:
			if(ar[BGP_CHECKBOX_PAGE2]==NULL)
			{

// EXAMPLES: CHECKBOX SAMPLE
				ar[BGP_CHECKBOX_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Checkbox",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_CHK_SAMPLE] = CheckBoxObject,
													LAB_Label, "_CheckBox",
													LAB_Place, PLACE_LEFT,
													GA_ID, BGP_CHK_SAMPLE,
												EndObject,
												FixMinWidth,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_CHECKBOX_PAGE2]);

		case PAGE_CYCLE:
			if(ar[BGP_CYCLE_PAGE2]==NULL)
			{
// EXAMPLES: CYCLE SAMPLE
				ar[BGP_CYCLE_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Cycle",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_CYC_SAMPLE] = CycleObject,
													CYC_Labels, BGP_CYC_SAMPLELabels,
													GA_ID, BGP_CYC_SAMPLE,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_CYCLE_PAGE2]);

		case PAGE_INFO:
			if(ar[BGP_INFO_PAGE2]==NULL)
			{
// EXAMPLES: INFO SAMPLE
				ar[BGP_INFO_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Info",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_INF_SAMPLE] = InfoObject,
													FRM_Type, inf_frmtype,
													FRM_Recessed, inf_recessed,
													INFO_TextFormat, "Info",
													INFO_HorizOffset, 6,
													INFO_VertOffset, 4,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_INFO_PAGE2]);

		case PAGE_PROGRESS:
			if(ar[BGP_PROGRESS_PAGE2]==NULL)
			{

// EXAMPLES: PROGRESS SAMPLE
				ar[BGP_PROGRESS_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Progress",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_PRG_SAMPLE] = ProgressObject,
													PROGRESS_FormatString, " ",
													PROGRESS_Done,	45,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_PROGRESS_PAGE2]);

		case PAGE_INDICATOR:
			if(ar[BGP_INDICATOR_PAGE2]==NULL)
			{
// EXAMPLES: INDICATOR SAMPLE
				ar[BGP_INDICATOR_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Indicator",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_IND_SAMPLE] = IndicatorObject,
													FRM_Type, ind_frmtype,
													FRM_Recessed, ind_recessed,
													INDIC_Level, 50,
													INDIC_FormatString, "%ld",
													INDIC_Justification, IDJ_CENTER,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_INDICATOR_PAGE2]);

		case PAGE_SLIDER:
			if(ar[BGP_SLIDER_PAGE2]==NULL)
			{

// EXAMPLES: SLIDER SAMPLE
				ar[BGP_SLIDER_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Slider",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_SLI_SAMPLE] = SliderObject,
													LAB_Label, "S_lider",
													LAB_Place, PLACE_LEFT,
													SLIDER_Max, 10,
													GA_ID, BGP_SLI_SAMPLE,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_SLIDER_PAGE2]);

		case PAGE_PROPORTIONAL:
			if(ar[BGP_PROPORTIONAL_PAGE2]==NULL)
			{

// EXAMPLES: PROPORTIONAL SAMPLE
				ar[BGP_PROPORTIONAL_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Proportional",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_PRO_SAMPLE] = PropObject,
													LAB_Label, "_Prop",
													PGA_Freedom, FREEHORIZ,
													PGA_Total, 25,
													PGA_Visible, 5,
													PGA_Top, 0,
													PGA_Total, 25,
													PGA_Arrows, TRUE,
													GA_ID, BGP_PRO_SAMPLE,
												EndObject,
												FixMinHeight,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_PROPORTIONAL_PAGE2]);

		case PAGE_MX:
			if(ar[BGP_MX_PAGE2]==NULL)
			{

// EXAMPLES: MX SAMPLE
				ar[BGP_MX_PAGE2] = HGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example MX",
									VarSpace (20),
									StartMember,
										VGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_MX_SAMPLE] = MxObject,
													GROUP_Style, GRSTYLE_VERTICAL,
													LAB_Label, "S_elect",
													LAB_Place, PLACE_LEFT,
													MX_LabelPlace, PLACE_RIGHT,
													MX_Labels, BGP_MX_SAMPLELabels,
													GA_ID, BGP_MX_SAMPLE,
												EndObject,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_MX_PAGE2]);

		case PAGE_LISTVIEW:
			if(ar[BGP_LISTVIEW_PAGE2]==NULL)
			{

// EXAMPLES: LISTVIEW SAMPLE
				ar[BGP_LISTVIEW_PAGE2] = VGroupObject,
									NormalOffset,
									FRM_Type, FRTYPE_DEFAULT,
									//NeXTFrame,
									FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
									FRM_Title, "Example Listview",
									VarSpace (20),
									StartMember,
										HGroupObject,
											VarSpace (20),
											StartMember,
												ar[BGP_LST_SAMPLE] = ListviewObject,
													LAB_Label, "S_elect",
													LAB_Place, PLACE_LEFT,
													LISTV_EntryArray, BGP_LST_SAMPLEEntrys,
													GA_ID, BGP_LST_SAMPLE,
												EndObject,
											EndMember,
											VarSpace (20),
										EndObject,
									EndMember,
									VarSpace (20),
								EndObject;

			}
			return(ar[BGP_LISTVIEW_PAGE2]);

	}
}

static void DisposePreferencesPage(ULONG page)
{
	Object **ar = BGUIPrefsObjs;

	switch(page)
	{
		case PAGE_GENERAL:
			DisposeObject(ar[BGP_GENERAL_PAGE]);
			ar[BGP_GENERAL_PAGE]=NULL;
			break;
		case PAGE_SPACING:
			DisposeObject(ar[BGP_SPACING_PAGE]);
			ar[BGP_SPACING_PAGE]=NULL;
			break;
		case PAGE_GROUP:
			DisposeObject(ar[BGP_GROUP_PAGE1]);
			ar[BGP_GROUP_PAGE1]=NULL;
			break;
		case PAGE_BUTTON:
			DisposeObject(ar[BGP_BUTTON_PAGE1]);
			ar[BGP_BUTTON_PAGE1]=NULL;
			break;
		case PAGE_STRING:
			DisposeObject(ar[BGP_STRING_PAGE1]);
			ar[BGP_STRING_PAGE1]=NULL;
			break;
		case PAGE_CHECKBOX:
			DisposeObject(ar[BGP_CHECKBOX_PAGE1]);
			ar[BGP_CHECKBOX_PAGE1]=NULL;
			break;
		case PAGE_CYCLE:
			DisposeObject(ar[BGP_CYCLE_PAGE1]);
			ar[BGP_CYCLE_PAGE1]=NULL;
			break;
		case PAGE_INFO:
			DisposeObject(ar[BGP_INFO_PAGE1]);
			ar[BGP_INFO_PAGE1]=NULL;
			break;
		case PAGE_PROGRESS:
			DisposeObject(ar[BGP_PROGRESS_PAGE1]);
			ar[BGP_PROGRESS_PAGE1]=NULL;
			break;
		case PAGE_INDICATOR:
			DisposeObject(ar[BGP_INDICATOR_PAGE1]);
			ar[BGP_INDICATOR_PAGE1]=NULL;
			break;
		case PAGE_SLIDER:
			DisposeObject(ar[BGP_SLIDER_PAGE1]);
			ar[BGP_SLIDER_PAGE1]=NULL;
			break;
		case PAGE_PROPORTIONAL:
			DisposeObject(ar[BGP_PROPORTIONAL_PAGE1]);
			ar[BGP_PROPORTIONAL_PAGE1]=NULL;
			break;
		case PAGE_MX:
			DisposeObject(ar[BGP_MX_PAGE1]);
			ar[BGP_MX_PAGE1]=NULL;
			break;
		case PAGE_LISTVIEW:
			DisposeObject(ar[BGP_LISTVIEW_PAGE1]);
			ar[BGP_LISTVIEW_PAGE1]=NULL;
			break;
	}
}

static void DisposeExamplesPage(ULONG page)
{
	Object **ar = BGUIPrefsObjs;

	switch(page)
	{
		case PAGE_GROUP:
			DisposeObject(ar[BGP_GROUP_PAGE2]);
			ar[BGP_GROUP_PAGE2]=NULL;
			break;
		case PAGE_BUTTON:
			DisposeObject(ar[BGP_BUTTON_PAGE2]);
			ar[BGP_BUTTON_PAGE2]=NULL;
			break;
		case PAGE_STRING:
			DisposeObject(ar[BGP_STRING_PAGE2]);
			ar[BGP_STRING_PAGE2]=NULL;
			break;
		case PAGE_CHECKBOX:
			DisposeObject(ar[BGP_CHECKBOX_PAGE2]);
			ar[BGP_CHECKBOX_PAGE2]=NULL;
			break;
		case PAGE_CYCLE:
			DisposeObject(ar[BGP_CYCLE_PAGE2]);
			ar[BGP_CYCLE_PAGE2]=NULL;
			break;
		case PAGE_INFO:
			DisposeObject(ar[BGP_INFO_PAGE2]);
			ar[BGP_INFO_PAGE2]=NULL;
			break;
		case PAGE_PROGRESS:
			DisposeObject(ar[BGP_PROGRESS_PAGE2]);
			ar[BGP_PROGRESS_PAGE2]=NULL;
			break;
		case PAGE_INDICATOR:
			DisposeObject(ar[BGP_INDICATOR_PAGE2]);
			ar[BGP_INDICATOR_PAGE2]=NULL;
			break;
		case PAGE_SLIDER:
			DisposeObject(ar[BGP_SLIDER_PAGE2]);
			ar[BGP_SLIDER_PAGE2]=NULL;
			break;
		case PAGE_PROPORTIONAL:
			DisposeObject(ar[BGP_PROPORTIONAL_PAGE2]);
			ar[BGP_PROPORTIONAL_PAGE2]=NULL;
			break;
		case PAGE_MX:
			DisposeObject(ar[BGP_MX_PAGE2]);
			ar[BGP_MX_PAGE2]=NULL;
			break;
		case PAGE_LISTVIEW:
			DisposeObject(ar[BGP_LISTVIEW_PAGE2]);
			ar[BGP_LISTVIEW_PAGE2]=NULL;
			break;
	}
}

/*
**	ULONG = Frame2Cycle( ULONG )
**
**	Translate a FRTYPE_xxx value into a value suitable
**	for use with a cyclegadget.  See also Cycle2Frame[].
*/
ULONG
Frame2Cycle( ULONG tmp_frame )
{
	ULONG	ret_frame;
	switch( tmp_frame ) {
		case	FRTYPE_BUTTON:		ret_frame = 0; break;
		case	FRTYPE_FUZZ_BUTTON:	ret_frame = 1; break;
		case	FRTYPE_XEN_BUTTON:	ret_frame = 2; break;
		case	FRTYPE_RIDGE:		ret_frame = 3; break;
		case	FRTYPE_FUZZ_RIDGE:	ret_frame = 4; break;
		case	FRTYPE_DROPBOX:		ret_frame = 5; break;
		case	FRTYPE_NEXT:		ret_frame = 6; break;
		case	FRTYPE_NONE:		ret_frame = 7; break;
	}
	return	ret_frame;
}

/*
**	SetGadgets( void )
**
**	Update the gadgets of the user interface to
**	reflect the current settings.
*/
void
SetGadgets(ULONG page)
{
	Object **ar = BGUIPrefsObjs;

	ULONG	frame = 0;

	switch(page)
	{
		case PAGE_GROUP:
			// GROUP PAGE [1997-02-17]
			frame = Frame2Cycle( grp_frmtype );
			SetAttrs( ar[ BGP_GRP_FRMTYPE ], CYC_Active, frame, TAG_DONE );
			SetAttrs( ar[ BGP_GRP_RECESSED ], GA_Selected, grp_recessed, TAG_DONE );
			break;

		case PAGE_BUTTON:
			// BUTTON PAGE [1997-02-16]
			frame = Frame2Cycle( but_frmtype );
			SetAttrs( ar[ BGP_BUT_FRMTYPE ], CYC_Active, frame, TAG_DONE );
			SetAttrs( ar[ BGP_BUT_RECESSED ], GA_Selected, but_recessed, TAG_DONE );
			break;

		case PAGE_STRING:
			// STRING PAGE [1997-02-16]
			frame = Frame2Cycle( str_frmtype );
			SetAttrs( ar[ BGP_STR_FRMTYPE ], CYC_Active, frame, TAG_DONE );
			SetAttrs( ar[ BGP_STR_RECESSED ], GA_Selected, str_recessed, TAG_DONE );
			break;

		case PAGE_CHECKBOX:
			// CHECKBOX PAGE [1997-02-16]
			frame = Frame2Cycle( chk_frmtype );
			SetAttrs( ar[ BGP_CHK_FRMTYPE ], CYC_Active, frame, TAG_DONE );
			SetAttrs( ar[ BGP_CHK_RECESSED ], GA_Selected, chk_recessed, TAG_DONE );
			break;

		case PAGE_CYCLE:
			// CYCLE PAGE [1997-02-16]
			frame = Frame2Cycle( cyc_frmtype );
			SetAttrs( ar[ BGP_CYC_FRMTYPE ], CYC_Active, frame , TAG_DONE) ;
			SetAttrs( ar[ BGP_CYC_RECESSED ], GA_Selected, cyc_recessed, TAG_DONE );
			SetAttrs( ar[ BGP_CYC_POPUP ], GA_Selected, cyc_popup, TAG_DONE );
			if( 0 == cyc_popup )
				SetAttrs( ar[ BGP_CYC_POPACTIVE ], GA_Disabled, TRUE, TAG_DONE );
			else
				SetAttrs( ar[ BGP_CYC_POPACTIVE ], GA_Selected, cyc_popactive, TAG_DONE );
			break;

		case PAGE_INFO:
			// INFO PAGE [1997-02-16]
			frame = Frame2Cycle( inf_frmtype );
			SetAttrs( ar[ BGP_INF_FRMTYPE ], CYC_Active, frame, TAG_DONE );
			SetAttrs( ar[ BGP_INF_RECESSED ], GA_Selected, inf_recessed, TAG_DONE );
			break;

		case PAGE_PROGRESS:
			// PROGRESS PAGE [1997-02-16]
			frame = Frame2Cycle( prg_frmtype );
			SetAttrs( ar[ BGP_PRG_FRMTYPE ], CYC_Active, frame, TAG_DONE );
			SetAttrs( ar[ BGP_PRG_RECESSED ], GA_Selected, prg_recessed, TAG_DONE );
			break;

		case PAGE_INDICATOR:
			// INDICATOR PAGE [1997-02-16]
			frame = Frame2Cycle( ind_frmtype );
			SetAttrs( ar[ BGP_IND_FRMTYPE ], CYC_Active, frame, TAG_DONE );
			SetAttrs( ar[ BGP_IND_RECESSED ], GA_Selected, ind_recessed, TAG_DONE );
			break;

		/*
		case PAGE_SLIDER:
			// SLIDER PAGE
			SetAttrs( ar[ BGP_SLIFRAME ], CYC_Active, sli_frame, TAG_DONE );
			SetAttrs( ar[ BGP_SLIRECES ], GA_Selected, sli_reces, TAG_DONE );
			SetAttrs( ar[ BGP_SLINLOOK ], GA_Selected, sli_nlook, TAG_DONE );
			SetAttrs( ar[ BGP_SAMPLE_SLIDER ], FRM_Type, Cycle2Frame[ sli_frame ], TAG_DONE );
			SetAttrs( ar[ BGP_SAMPLE_SLIDER ], FRM_Recessed, sli_reces, TAG_DONE );
			SetAttrs( ar[ BGP_SAMPLE_SLIDER ], PGA_NewLook, sli_nlook, TAG_DONE );
			AddCondit( ar[ BGP_SLIFRAME ], ar[ BGP_SLIRECES ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;
		*/
	}
}

/*
**	SetCondits( void )
**
**	Set the conditions for different settings.
*/
void
SetCondits( ULONG page)
{
	Object **ar = BGUIPrefsObjs;

	switch(page)
	{
		case PAGE_GROUP:
			// GROUP PAGE [1997-02-17]
			AddCondit( ar[ BGP_GRP_FRMTYPE ], ar[ BGP_GRP_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;

		case PAGE_BUTTON:
			// BUTTON PAGE [1997-02-16]
			AddCondit( ar[ BGP_BUT_FRMTYPE ], ar[ BGP_BUT_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;

		case PAGE_STRING:
			// STRING PAGE [1997-02-16]
			AddCondit( ar[ BGP_STR_FRMTYPE ], ar[ BGP_STR_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;

		case PAGE_CHECKBOX:
			// CHECKBOX PAGE [1997-02-16]
			AddCondit( ar[ BGP_CHK_FRMTYPE ], ar[ BGP_CHK_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;

		case PAGE_CYCLE:
			// CYCLE PAGE [1997-02-16]
			AddCondit( ar[ BGP_CYC_FRMTYPE ], ar[ BGP_CYC_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			AddCondit( ar[ BGP_CYC_POPUP ], ar[ BGP_CYC_POPACTIVE ], GA_Selected, 0, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;

		case PAGE_INFO:
			// INFO PAGE [1997-02-16]
			AddCondit( ar[ BGP_INF_FRMTYPE ], ar[ BGP_INF_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;

		case PAGE_PROGRESS:
			// PROGRESS PAGE [1997-02-16]
			AddCondit( ar[ BGP_PRG_FRMTYPE ], ar[ BGP_PRG_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;

		case PAGE_INDICATOR:
			// INDICATOR PAGE [1997-02-16]
			AddCondit( ar[ BGP_IND_FRMTYPE ], ar[ BGP_IND_RECESSED ], CYC_Active, 7, GA_Disabled, TRUE, GA_Disabled, FALSE );
			break;
	}
}

struct
{
	ULONG PreferencesPage;
	ULONG ExamplePage;
	BOOL Set;
	Object *Preferences;
	Object *Example;
}
pages_map[PAGE_COUNT];

ULONG total_pages=0,total_preferences_pages=0,total_example_pages=0;

Object *
InitBGUIPrefs( ULONG page)
{
	Object *win;
	Object **ar = BGUIPrefsObjs;
	ULONG gadget_page;

	memset(pages_map,'\0',sizeof(pages_map));
	gadget_page=(page<=PAGE_GROUP ? PAGE_GROUP : page);
	win = WindowObject,
		WINDOW_Title, "BGUI Preferences",
		WINDOW_ScreenTitle, "BGUI Preferences 41.10 - Copyright ©1996-1999, BGUI Developers Team",
		WINDOW_SmartRefresh, TRUE,
		WINDOW_AutoAspect, TRUE,
		WINDOW_AutoKeyLabel, TRUE,
		WINDOW_ScaleWidth, 50,
		WINDOW_CloseGadget, FALSE,
		WINDOW_ShowTitle, TRUE,
		WINDOW_MenuStrip, Menus,
		WINDOW_MasterGroup, ar[BGP_MASTER] = VGroupObject,
			NormalSpacing,
			NormalOffset,
			StartMember,
				HGroupObject,
					NormalSpacing,

// Mode listview.
					StartMember,
						VGroupObject,
							NormalHOffset,
							BOffset( GRSPACE_NORMAL ),
							FRM_Type, FRTYPE_DEFAULT,
							//NeXTFrame,
							FRM_Flags, FRF_CENTER_TITLE | FRF_HIGHLIGHT_TITLE,
							FRM_Title, "Mode",
							StartMember,
								ar[BGP_LISTVIEW1] = ListviewObject,
									PGA_NewLook, TRUE,
									LISTV_EntryArray, BGP_LISTVIEW1Entrys,
									GA_ID, BGP_LISTVIEW1,
								EndObject,
							EndMember,
						EndObject,
						Weight (20),
					EndMember,

					StartMember, ar[BGP_MAIN_PAGES] = PageObject,
						PAGE_Active,page>=PAGE_GROUP ? PAGE_GROUP : page,
	
						PageMember, PreferencesPage(PAGE_GENERAL),
						PageMember, PreferencesPage(PAGE_SPACING),
// GADGET SETTINGS
						PageMember, ar[BGP_MISC_PAGE] = VGroupObject,
							NormalSpacing,
// MISC
							StartMember, ar[BGP_SETTING_PAGES] = PageObject,
								PAGE_Active, 0,
								PageMember, pages_map[gadget_page].Preferences=PreferencesPage(gadget_page),
								EndObject,
							EndMember,
// EXAMPLES
							StartMember, ar[BGP_EXAMPLE_PAGES] = PageObject,
								PAGE_Active, 0,
								PageMember, pages_map[gadget_page].Example=ExamplesPage(gadget_page),
							EndObject,
							FixMinHeight,
						EndMember,
					EndObject,
				EndObject,
			EndMember,
		EndObject,
	EndMember,


// Horizontal separator.
	StartMember,
		SeparatorObject,
			SEP_Horiz, TRUE,
		EndObject,
		FixMinHeight,
	EndMember,


// Action buttons (Save/Use/Cancel).
	StartMember,
		HGroupObject,
			StartMember,
				ar[BGP_SAVE] = ButtonObject,
					LAB_Label, "_Save",
					GA_ID, BGP_SAVE,
				EndObject,
			EndMember,
			VarSpace (50),
			StartMember,
				ar[BGP_USE] = ButtonObject,
					LAB_Label, "_Use",
					GA_ID, BGP_USE,
				EndObject,
			EndMember,
			VarSpace (50),
			StartMember,
				ar[BGP_CANCEL] = ButtonObject,
					LAB_Label, "_Cancel",
					GA_ID, BGP_CANCEL,
				EndObject,
			EndMember,
		EndObject,
		FixMinHeight,
	EndMember,

	EndObject,

	EndObject;

	if(win)
	{
		// Add notification for the spacing.
/*
		AddMap( ar[ BGP_SPC_VNAR_STR ], ar[ BGP_SPC_VNAR_SLI ], VSpNar1 );
		AddMap( ar[ BGP_SPC_VNAR_SLI ], ar[ BGP_SPC_VNAR_STR ], VSpNar2 );
		AddMap( ar[ BGP_SPC_HNAR_STR ], ar[ BGP_SPC_HNAR_SLI ], HSpNar1 );
		AddMap( ar[ BGP_SPC_HNAR_SLI ], ar[ BGP_SPC_HNAR_STR ], HSpNar2 );
		AddMap( ar[ BGP_SPC_VNOR_STR ], ar[ BGP_SPC_VNOR_SLI ], VSpNor1 );
		AddMap( ar[ BGP_SPC_VNOR_SLI ], ar[ BGP_SPC_VNOR_STR ], VSpNor2 );
		AddMap( ar[ BGP_SPC_HNOR_STR ], ar[ BGP_SPC_HNOR_SLI ], HSpNor1 );
		AddMap( ar[ BGP_SPC_HNOR_SLI ], ar[ BGP_SPC_HNOR_STR ], HSpNor2 );
		AddMap( ar[ BGP_SPC_VWID_STR ], ar[ BGP_SPC_VWID_SLI ], VSpWid1 );
		AddMap( ar[ BGP_SPC_VWID_SLI ], ar[ BGP_SPC_VWID_STR ], VSpWid2 );
		AddMap( ar[ BGP_SPC_HWID_STR ], ar[ BGP_SPC_HWID_SLI ], HSpWid1 );
		AddMap( ar[ BGP_SPC_HWID_SLI ], ar[ BGP_SPC_HWID_STR ], HSpWid2 );
*/

		// Activate the very first entry.
		SetAttrs( ar[ BGP_LISTVIEW1 ], LISTV_Select, LISTV_Select_First, TAG_DONE );

		// Set the gadgets to the correct states.
		SetGadgets(page);

		// Set conditions.
		SetCondits(page);

		pages_map[PAGE_GENERAL].PreferencesPage=total_pages++;
		pages_map[PAGE_GENERAL].Set=TRUE;
		pages_map[PAGE_SPACING].PreferencesPage=total_pages++;
		pages_map[PAGE_SPACING].Set=TRUE;
		pages_map[gadget_page].PreferencesPage=total_preferences_pages++;
		pages_map[gadget_page].ExamplePage=total_example_pages++;
		pages_map[gadget_page].Set=TRUE;
	}
	return( win );
}

BOOL SetNewPage(struct Window *window,ULONG page)
{
	Object **ar = BGUIPrefsObjs;

	switch( page ) {
		case	PAGE_GENERAL:
		case	PAGE_SPACING:
			SetGadgetAttrs( ( struct Gadget * )ar[ BGP_MAIN_PAGES ], window, NULL, PAGE_Active, pages_map[page].PreferencesPage, TAG_DONE );
			break;
		default:
		{
			ULONG cur_page;
			BOOL add_preferences,add_example;

			if(!pages_map[page].Set)
			{
				ULONG previous_preferences_page, previous_example_page;

				GetAttr( PAGE_Active, ar[ BGP_SETTING_PAGES ], &previous_preferences_page);
				GetAttr( PAGE_Active, ar[ BGP_EXAMPLE_PAGES ], &previous_example_page);
				if((add_preferences=(pages_map[page].Preferences==NULL)))
				{
					if((pages_map[page].Preferences=PreferencesPage(page))==NULL)
						return(FALSE);
					pages_map[page].PreferencesPage=total_preferences_pages;
					total_preferences_pages++;
				}
				if((add_example=(pages_map[page].Example==NULL)))
				{
					if((pages_map[page].Example=ExamplesPage(page))==NULL)
					{
						DisposePreferencesPage(page);
						pages_map[page].Preferences=NULL;
						total_preferences_pages--;
						return(FALSE);
					}
					pages_map[page].ExamplePage=total_example_pages;
					total_example_pages++;
				}

				// Set the gadgets to the correct states.
				SetGadgets(page);

				if(add_preferences
				&& !DoMethod(ar[BGP_SETTING_PAGES],GRM_ADDMEMBER,pages_map[page].Preferences,LGO_Relayout,FALSE,TAG_END))
				{
					DisposeExamplesPage(page);
					pages_map[page].Example=NULL;
					total_example_pages--;
					DisposePreferencesPage(page);
					pages_map[page].Preferences=NULL;
					total_preferences_pages--;
					return(FALSE);
				}

				if(add_example
				&& !DoMethod(ar[BGP_EXAMPLE_PAGES],GRM_ADDMEMBER,pages_map[page].Example,LGO_Relayout,FALSE,TAG_END))
				{
					if(DoMethod(ar[BGP_SETTING_PAGES],GRM_REMMEMBER,pages_map[page].Preferences))
					{
						DisposePreferencesPage(page);
						pages_map[page].Preferences=NULL;
						total_preferences_pages--;
					}
					DisposeExamplesPage(page);
					pages_map[page].Example=NULL;
					total_example_pages--;
					return(FALSE);
				}

				SetAttrs( ar[ BGP_SETTING_PAGES ], PAGE_Active, pages_map[page].PreferencesPage, TAG_DONE);
				SetAttrs( ar[ BGP_EXAMPLE_PAGES ], PAGE_Active, pages_map[page].ExamplePage, TAG_DONE);

				{
					struct bmRelayout  bmr;

					bmr.MethodID   = BASE_RELAYOUT;
					bmr.bmr_GInfo = NULL;
					bmr.bmr_RPort = window->RPort;

					if(!BGUI_DoGadgetMethodA(ar[BGP_MISC_PAGE], window, NULL, (Msg)&bmr))
					{
						SetAttrs( ar[ BGP_SETTING_PAGES ], PAGE_Active, previous_preferences_page, TAG_DONE);
						SetAttrs( ar[ BGP_EXAMPLE_PAGES ], PAGE_Active, previous_example_page, TAG_DONE);

						if(DoMethod(ar[BGP_SETTING_PAGES],GRM_REMMEMBER,pages_map[page].Preferences))
						{
							DisposePreferencesPage(page);
							pages_map[page].Preferences=NULL;
							total_preferences_pages--;
						}
						if(DoMethod(ar[BGP_EXAMPLE_PAGES],GRM_REMMEMBER,pages_map[page].Example))
						{
							DisposeExamplesPage(page);
							pages_map[page].Example=NULL;
							total_example_pages--;
						}
						return(FALSE);
					}
				}

				// Set conditions.
				SetCondits(page);

				pages_map[page].Set=TRUE;

			}
			GetAttr( PAGE_Active, ar[ BGP_MAIN_PAGES ], &cur_page );
			if( cur_page < PAGE_GROUP ) {
				SetAttrs( ar[ BGP_SETTING_PAGES ], PAGE_Active, pages_map[page].PreferencesPage, TAG_DONE);
				SetAttrs( ar[ BGP_EXAMPLE_PAGES ], PAGE_Active, pages_map[page].ExamplePage, TAG_DONE);
				SetGadgetAttrs((struct Gadget *)ar[ BGP_MAIN_PAGES ], window, NULL, PAGE_Active, PAGE_GROUP, TAG_DONE);
			}
			else {
				SetGadgetAttrs((struct Gadget *)ar[ BGP_SETTING_PAGES ], window, NULL, PAGE_Active, pages_map[page].PreferencesPage, TAG_DONE);
				SetGadgetAttrs((struct Gadget *)ar[ BGP_EXAMPLE_PAGES ], window, NULL, PAGE_Active, pages_map[page].ExamplePage, TAG_DONE);
			}
			break;
		}
	}
	return(TRUE);
}
