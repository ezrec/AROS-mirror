#ifndef UNARCGROUP_CLASS_H
#define UNARCGROUP_CLASS_H

/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_UnarcGroup                 (TAG_USER | 0x10000000)

/*** Attributes *************************************************************/
#define MUIA_UnarcGroup_Archive         (MUIB_UnarcGroup | 0)
#define MUIA_UnarcGroup_Destination     (MUIB_UnarcGroup | 1)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *UnarcGroup_CLASS;

/*** Macros *****************************************************************/
#define UnarcGroupObject BOOPSIOBJMACRO_START(UnarcGroup_CLASS->mcc_Class)


#endif /* UNARCGROUP_CLASS_H */
