#ifndef _EMULATOR_H_
#define _EMULATOR_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_Emulator                   (TAG_USER | 0x12000000)

/*** Attributes *************************************************************/
#define MUIA_Emulator_State             (MUIB_Emulator | 0x00000000)
#define MUIV_Emulator_State_Stopped     (0)
#define MUIV_Emulator_State_Paused      (1)
#define MUIV_Emulator_State_Running     (2)

#define MUIA_Emulator_Fullscreen        (MUIB_Emulator | 0x00000001)

#define MUIA_Emulator_Window            (MUIB_Emulator | 0x00000002)

/*** Methods ****************************************************************/
#define MUIM_Emulator_Restart           (MUIB_Emulator | 0x00000000)
#define MUIM_Emulator_LoadState         (MUIB_Emulator | 0x00000001)
#define MUIM_Emulator_SaveState         (MUIB_Emulator | 0x00000002)

struct  MUIP_Emulator_LoadState         {ULONG MethodID; CONST_STRPTR filename;};
#define MUIP_Emulator_SaveState         MUIP_Emulator_LoadState

/*** Variables **************************************************************/
extern struct MUI_CustomClass *Emulator_CLASS;

/*** Macros *****************************************************************/
#define EmulatorObject BOOPSIOBJMACRO_START(Emulator_CLASS->mcc_Class)

#endif /* _EMULATOR_H_ */
