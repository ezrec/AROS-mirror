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

/*** Identifier base ********************************************************/
#define MUIB_DriveListEditor             (TAG_USER | 0x16000000)

/*** Methods ****************************************************************/
#define MUIM_DriveListEditor_AddDrive    (MUIB_DriveListEditor | 0x00000001)
#define MUIM_DriveListEditor_RemoveAll   (MUIB_DriveListEditor | 0x00000002)

struct  MUIP_DriveListEditor_AddDrive    {STACKED ULONG MethodID; STACKED struct Drive *drive;};
struct  MUIP_DriveListEditor_RemoveAll   {STACKED ULONG MethodID;};

/*** Macros *****************************************************************/
#define DriveListEditorObject BOOPSIOBJMACRO_START(DriveListEditor_CLASS->mcc_Class)

#endif /* _DRIVELISTEDITOR_H_ */
