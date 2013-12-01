#ifndef SCALOS_MENUPREFSPLUGIN_H
#define SCALOS_MENUPREFSPLUGIN_H
/*
**	$VER: scalosmenuprefsplugin.h 40.33 (14 Apr 2005 21:52:37)
**
** $Date$
** $Revision$
**
**	Special methods and tags for Scalos menu preferences plugin
**
**   (C) Copyright 2000-2005 The Scalos Team
**	All Rights Reserved
*/

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

// ---------------------------------------------------------------------------

/*** Methods ***/

#define	MUIM_ScalosPrefs_MenuPrefs_ImportTD	(MUIM_ScalosPrefs_User + 1)
#define	MUIM_ScalosPrefs_MenuPrefs_ImportP	(MUIM_ScalosPrefs_User + 2)
#define	MUIM_ScalosPrefs_MenuPrefs_Merge	(MUIM_ScalosPrefs_User + 3)
#define	MUIM_ScalosPrefs_MenuPrefs_CollapseAll	(MUIM_ScalosPrefs_User + 4)
#define	MUIM_ScalosPrefs_MenuPrefs_ExpandAll	(MUIM_ScalosPrefs_User + 5)


/*** Method structs ***/

/*** Special method values ***/

/*** Attributes ***/
#define MUIM_ScalosPrefs_MenuPrefs_HideObsolete (MUIM_ScalosPrefs_User + 1001)

/*** Special attribute values ***/



/*** Structures, Flags & Values ***/

// ---------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* SCALOS_MENUPREFSPLUGIN_H */
