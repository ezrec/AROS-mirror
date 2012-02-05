#ifndef UNARCWINDOW_CLASS_H
#define UNARCWINDOW_CLASS_H

/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_UnarcWindow              (TAG_USER | 0x10000000)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *UnarcWindow_CLASS;

/*** Macros *****************************************************************/
#define UnarcWindowObject BOOPSIOBJMACRO_START(UnarcWindow_CLASS->mcc_Class)


#endif /* UNARCWINDOW_CLASS_H */
