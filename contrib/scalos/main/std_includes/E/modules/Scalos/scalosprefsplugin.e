OPT PREPROCESS
OPT MODULE
OPT EXPORT

/*
**	$VER: scalosprefsplugin.e 40.34 (07.08.06)
**
** $Date: 2006-01-06 15:38:53 +0200,,,,,,,,,,,,,,,,,,,, $
** $Revision: 1362 $
** $Id: $
**
**	Interface definitions for Scalos prefs plugins
**
**   (C) Copyright 2000-2006 The Scalos Team
**	All Rights Reserved
*/

-> ---------------------------------------------------------------------------

/*** Methods ***/

CONST	MUIM_ScalosPrefs_LoadConfig		= $84723201,
	MUIM_ScalosPrefs_UseConfig		= $84723202,
	MUIM_ScalosPrefs_SaveConfig		= $84723203,
	MUIM_ScalosPrefs_RestoreConfig		= $84723204,
	MUIM_ScalosPrefs_ResetToDefaults	= $84723205,
	MUIM_ScalosPrefs_LastSavedConfig	= $84723206,
	MUIM_ScalosPrefs_PageActive		= $84723207,
	MUIM_ScalosPrefs_OpenConfig		= $84723208,
	MUIM_ScalosPrefs_SaveConfigAs		= $84723209,
	MUIM_ScalosPrefs_About			= $8472320a,
	MUIM_ScalosPrefs_LoadNamedConfig	= $8472320b,
	MUIM_ScalosPrefs_CreateSubWindows	= $8472320c,
	MUIM_ScalosPrefs_SaveConfigIfChanged	= $8472320d,
	MUIM_ScalosPrefs_UseConfigIfChanged	= $8472320e,
	MUIM_ScalosPrefs_ParseToolTypes		= $8472320f,
	MUIM_ScalosPrefs_GetListOfMCCs		= $84723210

-> plugin-specific method IDs may start here
CONST	MUIM_ScalosPrefs_User			= $84723fff


/*** Method structs ***/

OBJECT muip_scalosprefs_pageactive
	methodid:LONG
	isactive:PTR TO LONG
ENDOBJECT

OBJECT muip_scalosprefs_loadnamedconfig
	methodid:LONG
	configfilename
ENDOBJECT

OBJECT muip_scalosprefs_parsetooltypes
	methodid:LONG
	tooltypes
ENDOBJECT

OBJECT muip_scalosprefs_mcclist
	mcc_name
	mcc_minversion:PTR TO LONG
	mcc_minrevision:PTR TO LONG
ENDOBJECT

OBJECT muip_scalosprefs_getlistofmccs
	methodid:LONG
ENDOBJECT

/*** Special method values ***/

/*** Attributes ***/

CONST	MUIA_ScalosPrefs_CreateIcons		= $84723301,	-> [isg] BOOL
	MUIA_ScalosPrefs_MainWindow		= $84723302,	-> [is.] Object *
	MUIA_ScalosPrefs_Application		= $84723303,	-> [is.] Object *
	MUIA_ScalosPrefs_ProgramName		= $84723304	-> [is.] CONST_STRPTR


/*** Special attribute values ***/



/*** Structures, Flags & Values ***/

-> "Which" values for SCAGetPrefsInfo() call
ENUM	SCAPREFSINFO_GetClass = 1,
	SCAPREFSINFO_GetTitle,
	SCAPREFSINFO_GetTitleImage

-> ---------------------------------------------------------------------------
