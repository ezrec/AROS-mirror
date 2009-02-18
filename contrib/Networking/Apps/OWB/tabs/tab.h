#ifndef TAB_H
#define TAB_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_Tab                 (TAG_USER | 0x10000000)

/*** Public attributes ******************************************************/
#define MUIA_Tab_Active          (MUIB_Tab  | 0x00000000)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *Tab_CLASS;

/*** Macros *****************************************************************/
#define TabObject BOOPSIOBJMACRO_START(Tab_CLASS->mcc_Class)

#endif /* TAB_H */
