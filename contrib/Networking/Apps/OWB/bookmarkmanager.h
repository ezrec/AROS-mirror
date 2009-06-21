#ifndef _BOOKMARKMANAGER_H
#define _BOOKMARKMANAGER_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_BookmarkManager                 (TAG_USER | 0x10000200)

/*** Public attributes ******************************************************/
#define MUIA_BookmarkManager_BookmarkMenu    (MUIB_BookmarkManager|0x00000000)
#define MUIA_BookmarkManager_SelectedURL     (MUIB_BookmarkManager|0x00000001)

/*** Private methods * ******************************************************/
#define MUIM_BookmarkManager_Insert          (MUIB_BookmarkManager|0x00000000)
#define MUIM_BookmarkManager_Change          (MUIB_BookmarkManager|0x00000001)
#define MUIM_BookmarkManager_Remove          (MUIB_BookmarkManager|0x00000002)
#define MUIM_BookmarkManager_Display         (MUIB_BookmarkManager|0x00000003)
#define MUIM_BookmarkManager_Move            (MUIB_BookmarkManager|0x00000004)
struct MUIP_BookmarkManager_Move             {STACKED ULONG MethodID; STACKED ULONG position;};
#define MUIM_BookmarkManager_Bookmark        (MUIB_BookmarkManager|0x00000005)
struct MUIP_BookmarkManager_Bookmark         {STACKED ULONG MethodID; STACKED STRPTR label; STACKED STRPTR url;};

/*** Constants **************************************************************/
#define MUIV_BookmarkManager_MoveUp          -1
#define MUIV_BookmarkManager_MoveDown        -2

/*** Variables **************************************************************/
extern struct MUI_CustomClass *BookmarkManager_CLASS;

/*** Macros *****************************************************************/
#define BookmarkManagerObject BOOPSIOBJMACRO_START(BookmarkManager_CLASS->mcc_Class)

#endif /* _BOOKMARKMANAGER_H */
