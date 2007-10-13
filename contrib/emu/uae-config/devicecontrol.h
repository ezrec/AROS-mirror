#ifndef _DEVICECONTROL_H_
#define _DEVICECONTROL_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_DeviceControl                  (TAG_USER | 0x10000000)

/*** Attributes *************************************************************/
#define MUIA_DeviceControl_Type             (MUIB_DeviceControl | 0x00000000)
#define MUIA_DeviceControl_Activity         (MUIB_DeviceControl | 0x00000001)
#define MUIA_DeviceControl_Path             (MUIB_DeviceControl | 0x00000002)
#define MUIA_DeviceControl_Name             (MUIB_DeviceControl | 0x00000003)

#define MUIV_DeviceControl_Type_None        (0)
#define MUIV_DeviceControl_Type_FloppyDrive (1)
#define MUIV_DeviceControl_Type_HardDrive   (2)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *DeviceControl_CLASS;

/*** Macros *****************************************************************/
#define DeviceControlObject BOOPSIOBJMACRO_START(DeviceControl_CLASS->mcc_Class)

#endif /* _DEVICECONTROL_H_ */
