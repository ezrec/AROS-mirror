#ifndef PREFS_SCA_MENU_H
#define PREFS_SCA_MENU_H
/*
**	$VER: scalos_pattern.h 40.33 (14 Apr 2005 19:41:04)
**
** $Date$
** $Revision$
**
**	File format for scalos_menu preferences
**
**   (C) Copyright 2000-2006 The Scalos Team
**	All Rights Reserved
*/

// ---------------------------------------------------------------------------

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// This is the exact structure that stores menu information in menu preferences files
// !!! BE VERY CAREFUL TO KEEP PREFS COMPATIBLE IF YOU EVER CHANGE THIS STRUCTURE !!!
struct ScalosMenuTree
	{
	struct	ScalosMenuTree *mtre_Next;
	struct	ScalosMenuTree *mtre_tree;
	UBYTE	mtre_type;
	UBYTE	mtre_flags;
	union	{
		struct	{
			char	mtre_hotkey[2];
			STRPTR	mtre_name;
			STRPTR	mtre_iconnames;	// only valid if MTREFLGF_IconNames
						// points to 2 strings with menu icon path names
						// for empty path names, it points to two '\0'
			} MenuTree;
		struct	{
			UBYTE	mcom_flags;
			UBYTE	mcom_type;	// enum ScalosMenuCommandType
			ULONG	mcom_stack;
			STRPTR	mcom_name;
			BYTE	mcom_pri;	// priority for command process
						// only valid if MCOMFLGF_Priority set
			} MenuCommand;
		} MenuCombo;
	};

enum ScalosMenuType
	{
	SCAMENUTYPE_MainMenu = 0,
	SCAMENUTYPE_Menu,
	SCAMENUTYPE_MenuItem,
	SCAMENUTYPE_Command,
	SCAMENUTYPE_ToolsMenu
	};

enum ScalosMenuCommandType
	{
	SCAMENUCMDTYPE_Command = 0,		// 0 = Command (internal)
	SCAMENUCMDTYPE_Workbench,		// 1 = Workbench
	SCAMENUCMDTYPE_AmigaDOS,		// 2 = AmigaDOS
	SCAMENUCMDTYPE_IconWindow,		// 3 = Iconwindow
	SCAMENUCMDTYPE_ARexx,			// 4 = Arexx
	SCAMENUCMDTYPE_Plugin			// 5 = Menu plugin
	};

// values in mtre_flags
// bits 0..2 are RESERVED
#define MTREFLGB_IconNames	7		//  mtre_iconnames are valid
#define MTREFLGF_IconNames	(1 << MTREFLGB_IconNames)

// values in mcom_flags
#define	MCOMFLGB_Args		0		// this command uses arguments
#define	MCOMFLGF_Args		(1 << MCOMFLGB_Args)
#define	MCOMFLGB_Priority	1		// this command uses priority information in mcom_pri
#define	MCOMFLGF_Priority	(1 << MCOMFLGB_Priority)

// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* PREFS_SCA_MENU_H */
