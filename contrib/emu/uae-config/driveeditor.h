#ifndef _DRIVEEDITOR_H_
#define _DRIVEEDITOR_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_DriveEditor                 (TAG_USER | 0x15000000)

/*** Attributes *************************************************************/
#define MUIA_DriveEditor_Drive           (MUIB_DriveEditor | 0x00000000)

#define MUIV_DriveEditor_ReturnID_Cancel (1)
#define MUIV_DriveEditor_ReturnID_OK     (2)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *DriveEditor_CLASS;

/*** Macros *****************************************************************/
#define DriveEditorObject BOOPSIOBJMACRO_START(DriveEditor_CLASS->mcc_Class)

#endif /* _DRIVEEDITOR_H_ */
