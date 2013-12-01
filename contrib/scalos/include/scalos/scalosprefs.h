#ifndef SCALOS_PREFS_H
#define SCALOS_PREFS_H
/*
**	$VER: scalosprefs.h 40.33 (14 Apr 2005 17:32:41)
**
** $Date$
** $Revision$
**
**	File format for Scalos main preferences
**
**   (C) Copyright 2000-2005 The Scalos Team
**	All Rights Reserved
*/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// ---------------------------------------------------------------------------

// values in DragType
enum ScalosPrefsDragType 
	{ 
	DRAGTYPE_ImageOnly = 0,
	DRAGTYPE_ImageAndText
	};

// Values in _prefDragMethod
enum ScalosPrefsDragMethodValues
	{
	DRAGMETHOD_GELS	= 0,
	DRAGMETHOD_Custom
	};

// Values in dragTranspType
enum ScalosPrefsTranspType
	{
	TRANSPTYPE_Ghosted,
	TRANSPTYPE_Transparent,
	};

// Values in dragtranspmode
enum ScalosPrefsDragTranspModeValues
	{
	DRAGTRANSPMODE_SolidAndTransp =	0,
	DRAGTRANSPMODE_Solid,
	DRAGTRANSPMODE_Transp,
	DRAGTRANSPMODE_TranspAndSolid,
	};

// Values in dragtransp
#define	DRAGTRANSPB_DiskIcons		0
#define	DRAGTRANSPB_DrawerIcons		1
#define	DRAGTRANSPB_ToolIcons		2
#define	DRAGTRANSPB_ProjectIcons	3
#define	DRAGTRANSPB_TrashcanIcons	4
#define	DRAGTRANSPB_KickIcons		5
#define	DRAGTRANSPB_AppIcons		6
#define	DRAGTRANSPB_AppWindows		7
#define	DRAGTRANSPB_ScalosWindows	8
#define DRAGTRANSPB_IconifiedWinIcons	9

#define	DRAGTRANSPF_DiskIcons		(1 << DRAGTRANSPB_DiskIcons)
#define	DRAGTRANSPF_DrawerIcons		(1 << DRAGTRANSPB_DrawerIcons)
#define	DRAGTRANSPF_ToolIcons		(1 << DRAGTRANSPB_ToolIcons)
#define	DRAGTRANSPF_ProjectIcons	(1 << DRAGTRANSPB_ProjectIcons)
#define	DRAGTRANSPF_TrashcanIcons	(1 << DRAGTRANSPB_TrashcanIcons)
#define	DRAGTRANSPF_KickIcons		(1 << DRAGTRANSPB_KickIcons)
#define	DRAGTRANSPF_AppIcons		(1 << DRAGTRANSPB_AppIcons)
#define	DRAGTRANSPF_AppWindows		(1 << DRAGTRANSPB_AppWindows)
#define DRAGTRANSPF_ScalosWindows	(1 << DRAGTRANSPB_ScalosWindows)
#define DRAGTRANSPF_IconifiedWinIcons	(1 << DRAGTRANSPB_IconifiedWinIcons)

struct SCP_PluginEntry
	{
	WORD pey_Priority;
	UWORD pey_InstSize;
	UWORD pey_NeededVersion;
	UWORD pey_unused;
	char pey_Path[1];		// variable-length string, terminated by '\0'
//	char pey_ClassName[1];		// variable-length string, terminated by '\0'
//	char pey_SuperclassName[1];	// variable-length string, terminated by '\0'
	};

struct SCP_GadgetStringEntry
	{
	ULONG gse_Length;		// size of gse_Strings
	char gse_Strings[1];		// variable length
	};

struct SCP_GadgetEntry
	{
	UWORD sgy_GadgetType;
	char  sgy_Action[40];		// Name of Menu Command -
	ULONG sgy_NormalImageIndex;	// file name of normal state image - index into SCP_ControlBarGadgetStrings array
	ULONG sgy_SelectedImageIndex;	// file name of selected state image - index into SCP_ControlBarGadgetStrings array
	ULONG sgy_DisabledImageIndex;	// file name of disabled state image - index into SCP_ControlBarGadgetStrings array
	ULONG sgy_HelpTextIndex;	// Bubble help text - index into SCP_ControlBarGadgetStrings array
	};

enum SCPGadgetTypes
	{
	SCPGadgetType_BackButton,
	SCPGadgetType_ForwardButton,
	SCPGadgetType_UpButton,
	SCPGadgetType_History,
	SCPGadgetType_BrowseButton,
	SCPGadgetType_ViewBy,		// cycle gadget for "View By"
	SCPGadgetType_ShowMode,		// cycle gadget for "Show All" vs. "Show Only Icons"
	SCPGadgetType_Separator,	// empty space as separator
	SCPGadgetType_ActionButton,	// user-defined action button with arbitrary menu command
	};

// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* SCALOS_PREFS_H */
