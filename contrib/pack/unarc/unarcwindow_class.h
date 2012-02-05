#ifndef UNARCWINDOW_CLASS_H
#define UNARCWINDOW_CLASS_H

/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_UnarcWindow                (TAG_USER | 0x10000000)

/*** Attributes *************************************************************/
#define MUIA_UnarcWindow_Archive        (MUIB_UnarcWindow | 0)
#define MUIA_UnarcWindow_Destination    (MUIB_UnarcWindow | 1)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *UnarcWindow_CLASS;

/*** Macros *****************************************************************/
#define UnarcWindowObject BOOPSIOBJMACRO_START(UnarcWindow_CLASS->mcc_Class)


#endif /* UNARCWINDOW_CLASS_H */
