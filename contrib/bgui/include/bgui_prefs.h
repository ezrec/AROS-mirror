/*
 * @(#) $Header$
 *
 * $VER: libraries/bgui_prefs.h 41.10 (25.2.98)
 * bgui.library preferences constants.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:23:25  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:01:59  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:14:01  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:16:09  mlemos
 * Ian sources
 *
 *
 */

/*
 * IFF ID tags.
 */
#define ID_BGUI   MAKE_ID('B','G','U','I')
#define ID_DTAG   MAKE_ID('D','T','A','G')

/*
 * Preferences specific tags.
 */
#define GROUP_DefHSpaceNarrow           (BGUI_TB+9001)  /* V41.8 */
#define GROUP_DefHSpaceNormal           (BGUI_TB+9002)
#define GROUP_DefHSpaceWide             (BGUI_TB+9003)
#define GROUP_DefVSpaceNarrow           (BGUI_TB+9004)
#define GROUP_DefVSpaceNormal           (BGUI_TB+9005)
#define GROUP_DefVSpaceWide             (BGUI_TB+9006)

#define FRM_DefaultType                 (BGUI_TB+9101)  /* V41.8 */

#ifndef CLASSLIB_H
/*
 * Private library functions.
 */
#pragma libcall BGUIBase BGUI_GetDefaultTags a8 001
#pragma libcall BGUIBase BGUI_DefaultPrefs ae 0
#pragma libcall BGUIBase BGUI_LoadPrefs b4 801

struct TagItem *BGUI_GetDefaultTags(ULONG id);
/*
 * If you pass the id number of a BGUI class to this function, you will get the current
 * default tags for the class.  This taglist is not a copy, and should not be freed.
 * After any BGUI_LoadPrefs or BGUI_DefaultPrefs, this pointer will become invalid,
 * and should be retrieved again.
 *
 * Example:  group_defaults = BGUI_GetDefaultTags(BGUI_GROUP_GADGET);
 *           spvnormal = GetTagData(GROUP_DefVSpaceNormal, 4, group_defaults);
 *
 * This will get the default normal vertical spacing (or 4 if the taglist wasn't found).
 */


VOID BGUI_DefaultPrefs(VOID);
/*
 * Resets all class defaults to internal defaults.  No files are loaded.
 */


VOID BGUI_LoadPrefs(UBYTE *name);
/*
 * Loads the specified BGUI prefs file into BGUI.  Effective for current task only.
 */
#endif
