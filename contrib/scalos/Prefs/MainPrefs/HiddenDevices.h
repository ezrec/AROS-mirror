// HiddenDevices.h
// $Date$
// $Revision$


#ifndef HIDDENDEVICES_H
#define HIDDENDEVICES_H

//-----------------------------------------------------------------

#include "ScalosPrefs.h"

//-----------------------------------------------------------------

BOOL HiddenDevicesInit(void);
void HiddenDevicesCleanup(void);
void FillHiddenDevicesList(struct SCAModule *app);
BOOL ReadWorkbenchPrefs(CONST_STRPTR filename);
LONG WriteWorkbenchPrefs(CONST_STRPTR filename);
BOOL AddHiddenDevice(CONST_STRPTR Name);
BOOL RemoveHiddenDevice(CONST_STRPTR Name);

//-----------------------------------------------------------------

#define getv(o,a) mui_getv(o,a)

//-----------------------------------------------------------------

#endif /* HIDDENDEVICES_H */
