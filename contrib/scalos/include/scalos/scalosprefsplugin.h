#ifndef SCALOS_PREFSPLUGIN_H
#define SCALOS_PREFSPLUGIN_H
/*
**	$VER: scalosprefsplugin.h 40.33 (14 Apr 2005 21:15:40)
**
** $Date$
** $Revision$
**
**	Interface definitions for Scalos prefs plugins
**
**   (C) Copyright 2000-2005 The Scalos Team
**	All Rights Reserved
*/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// ---------------------------------------------------------------------------

/*** Methods ***/

#define	MUIM_ScalosPrefs_LoadConfig		0x84723201
#define	MUIM_ScalosPrefs_UseConfig		0x84723202
#define	MUIM_ScalosPrefs_SaveConfig		0x84723203
#define	MUIM_ScalosPrefs_RestoreConfig		0x84723204
#define	MUIM_ScalosPrefs_ResetToDefaults	0x84723205
#define	MUIM_ScalosPrefs_LastSavedConfig	0x84723206
#define	MUIM_ScalosPrefs_PageActive		0x84723207
#define	MUIM_ScalosPrefs_OpenConfig		0x84723208
#define	MUIM_ScalosPrefs_SaveConfigAs		0x84723209
#define	MUIM_ScalosPrefs_About			0x8472320a
#define	MUIM_ScalosPrefs_LoadNamedConfig	0x8472320b
#define	MUIM_ScalosPrefs_CreateSubWindows	0x8472320c
#define	MUIM_ScalosPrefs_SaveConfigIfChanged	0x8472320d
#define	MUIM_ScalosPrefs_UseConfigIfChanged	0x8472320e
#define	MUIM_ScalosPrefs_ParseToolTypes		0x8472320f
#define MUIM_ScalosPrefs_GetListOfMCCs		0x84723210


// plugin-specific method IDs may start here
#define	MUIM_ScalosPrefs_User			0x84723fff


/*** Method structs ***/

struct MUIP_ScalosPrefs_PageActive
	{
	ULONG MethodID;
	ULONG isActive;
	};

struct MUIP_ScalosPrefs_LoadNamedConfig
	{
	ULONG MethodID;
	STRPTR ConfigFileName;
	};

struct MUIP_ScalosPrefs_ParseToolTypes
	{
	ULONG MethodID;
	STRPTR *ToolTypes;
	};

struct MUIP_ScalosPrefs_MCCList
	{
	CONST_STRPTR MccName;
	ULONG MccMinVersion;
	ULONG MccMinRevision;
	};

struct MUIP_ScalosPrefs_GetListOfMCCs
	{
	ULONG MethodID;
	};

/*** Special method values ***/

/*** Attributes ***/

#define	MUIA_ScalosPrefs_CreateIcons		0x84723301	// [isg] BOOL
#define	MUIA_ScalosPrefs_MainWindow		0x84723302	// [is.] Object *
#define	MUIA_ScalosPrefs_Application		0x84723303	// [is.] Object *
#define	MUIA_ScalosPrefs_ProgramName		0x84723304	// [is.] CONST_STRPTR


/*** Special attribute values ***/



/*** Structures, Flags & Values ***/

// "Which" values for SCAGetPrefsInfo() call
#define	SCAPREFSINFO_GetClass			1
#define	SCAPREFSINFO_GetTitle			2
#define	SCAPREFSINFO_GetTitleImage		3


// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* SCALOS_PREFSPLUGIN_H */
