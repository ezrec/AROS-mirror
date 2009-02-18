#ifndef _SEARCHWINDOW_H
#define _SEARCHWINDOW_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_SearchWindow                 (TAG_USER | 0x10000000)

/*** Public attributes ******************************************************/
#define MUIA_SearchWindow_SearchHook      (MUIB_SearchWindow | 0x00000000)

/*** Private methods * ******************************************************/
#define MUIM_SearchWindow_Find            (MUIB_SearchWindow | 0x00000000)
struct MUIP_SearchWindow_Find             {STACKED ULONG MethodID; STACKED BYTE forward;};

/*** Variables **************************************************************/
extern struct MUI_CustomClass *SearchWindow_CLASS;

/*** Macros *****************************************************************/
#define SearchWindowObject BOOPSIOBJMACRO_START(SearchWindow_CLASS->mcc_Class)

struct OWB_SearchMsg
{
    STRPTR criteria;
    LONG caseSensitive;
    LONG forward;
};

#endif /* _SEARCHWINDOW_H */
