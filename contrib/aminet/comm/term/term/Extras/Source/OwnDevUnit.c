/*
**	OwnDevUnit.c
**
**	Device locking routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Local copy of what we are locking */

STATIC UBYTE	DeviceName[MAX_FILENAME_LENGTH];
STATIC LONG		UnitNumber = -1;
STATIC BOOL		DeviceLocked = FALSE;

VOID
UnlockDevice()
{
	if(DeviceLocked)
	{
#ifndef __AROS__
		FreeDevUnit(DeviceName,UnitNumber);
#endif
		DeviceLocked = FALSE;
	}
}

BOOL
LockDevice(STRPTR Device,LONG Unit,STRPTR ErrorString,LONG ErrorStringSize)
{
	UnlockDevice();

	if(OwnDevUnitBase)
	{
#ifndef __AROS__
		CONST_STRPTR Error;

		if(Error = AttemptDevUnit(Device,Unit,TermIDString,OwnDevBit))
		{
				/* Check for the type of error, if any */

			if(ErrorString)
			{
				if(!Strnicmp(Error,ODUERR_LEADCHAR,1))
					LimitedSPrintf(ErrorStringSize,ErrorString,LocaleString(MSG_SERIAL_ERROR_ACCESSING_TXT),Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber,&Error[1]);
				else
					LimitedSPrintf(ErrorStringSize,ErrorString,LocaleString(MSG_SERIAL_DEVICE_IN_USE_TXT),Config->SerialConfig->SerialDevice,Config->SerialConfig->UnitNumber,Error);
			}

			return(FALSE);
		}
		else
#endif
		{
			LimitedStrcpy(sizeof(DeviceName),DeviceName,Device);

			UnitNumber = Unit;

			DeviceLocked = TRUE;
		}
	}

	return(TRUE);
}
