/* Copyright © 1996 Kai Hofmann. All rights reserved. */

#ifndef _PHONELOGGENERATOR_H
#define _PHONELOGGENERATOR_H 1

#ifndef _PHONELOG_H
#include "PhoneLog.h"
#endif	/* _PHONELOG_H */

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#ifndef DOS_EXALL_H
#include <dos/exall.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

typedef struct PhoneLogHandle
{
	BPTR	file;
	LONG	error;
} PhoneLogHandle;

PhoneLogHandle *OpenPhoneLog(char *name);
void ClosePhoneLog(PhoneLogHandle *handle);
void WritePhoneLogStartEntry(PhoneLogHandle *handle, struct PhoneLogEntry *item);
void WritePhoneLogEndEntry(PhoneLogHandle *handle, struct PhoneLogEntry *item);

#endif	/* _PHONELOGGENERATOR_H */
