#ifndef TABS_H
#define TABS_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_Tabs                   (TAG_USER | 0x10000000)

/*** Public attributes ******************************************************/
#define MUIA_Tabs_Location          (MUIB_Tabs | 0x00000000)
#define MUIA_Tabs_ActiveTab         (MUIB_Tabs | 0x00000001)
#define MUIA_Tabs_CloseButton       (MUIB_Tabs | 0x00000002)
#define MUIA_Tabs_ActiveTabObject   (MUIB_Tabs | 0x00000003)

/*** Public constants *******************************************************/
#define MUIV_Tabs_Top               (MUIB_Tabs | 0x00000000)
#define MUIV_Tabs_Left              (MUIB_Tabs | 0x00000001)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *Tabs_CLASS;

/*** Macros *****************************************************************/
#define TabsObject BOOPSIOBJMACRO_START(Tabs_CLASS->mcc_Class)

#endif /* ZUNE_TABS_H */
