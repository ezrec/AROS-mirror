#ifndef _PREFERENCES_H
#define _PREFERENCES_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_PreferencesManager                 (TAG_USER | 0x10000100)

/*** Private methods ********************************************************/
#define MUIM_PreferencesManager_Use             (MUIB_PreferencesManager | 0x00000000)
#define MUIM_PreferencesManager_Load            (MUIB_PreferencesManager | 0x00000001)
#define MUIM_PreferencesManager_Save            (MUIB_PreferencesManager | 0x00000002)

/*** Attributes *************************************************************/
#define MUIA_PreferencesManager_Preferences     (MUIB_PreferencesManager | 0x00000000)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *PreferencesManager_CLASS;

/*** Macros *****************************************************************/
#define PreferencesManagerObject BOOPSIOBJMACRO_START(PreferencesManager_CLASS->mcc_Class)

#endif /* _PREFERENCES_H */
