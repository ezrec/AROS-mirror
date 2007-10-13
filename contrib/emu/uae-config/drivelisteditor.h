#ifndef _DRIVELISTEDITOR_H_
#define _DRIVELISTEDITOR_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Variables **************************************************************/
extern struct MUI_CustomClass *DriveListEditor_CLASS;

/*** Macros *****************************************************************/
#define DriveListEditorObject BOOPSIOBJMACRO_START(DriveListEditor_CLASS->mcc_Class)

#endif /* _DRIVELISTEDITOR_H_ */
