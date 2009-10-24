#ifndef _SEARCHBAR_H
#define _SEARCHBAR_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_SearchBar                 (TAG_USER | 0x10000000)

/*** Public attributes ******************************************************/

/*** Private methods * ******************************************************/
#define MUIM_SearchBar_Find            (MUIB_SearchBar | 0x00000000)
struct MUIP_SearchBar_Find             {STACKED ULONG MethodID; STACKED BYTE forward;};

/*** Variables **************************************************************/
extern struct MUI_CustomClass *SearchBar_CLASS;

/*** Macros *****************************************************************/
#define SearchBarObject BOOPSIOBJMACRO_START(SearchBar_CLASS->mcc_Class)

struct OWB_SearchMsg
{
    STRPTR criteria;
    LONG caseSensitive;
    LONG forward;
};

#endif /* _SEARCHBAR_H */
