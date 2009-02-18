#ifndef TABBEDGROUP_H
#define TABBEDGROUP_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_TabbedGroup                 (TAG_USER | 0x10000000)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *TabbedGroup_CLASS;

/*** Macros *****************************************************************/
#define TabbedGroupObject BOOPSIOBJMACRO_START(TabbedGroup_CLASS->mcc_Class)

#endif /* TABBEDGROUP_H */
