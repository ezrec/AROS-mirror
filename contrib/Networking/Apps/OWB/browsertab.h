#ifndef _BROWSERTAB_H
#define _BROWSERTAB_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_BrowserTab                 (MUIB_AROS | 0x00000f00)

/*** Public attributes ******************************************************/
#define MUIA_BrowserTab_Title           (MUIB_BrowserTab  | 0x00000000)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *BrowserTab_CLASS;

/*** Macros *****************************************************************/
#define BrowserTabObject BOOPSIOBJMACRO_START(BrowserTab_CLASS->mcc_Class)

#endif /* _BROWSERTAB_H */
