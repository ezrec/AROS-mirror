/*********************************************************************
----------------------------------------------------------------------

	MysticView
	keyfile

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_KEYZ_H
#define	MYSTIC_KEYZ_H 1

#include "Mystic_Global.h"

struct keyfile
{
	struct SignalSemaphore semaphore;
	int len;
	char *key;
	unsigned long crctable[256];
};

extern BOOL CheckKey(void);
extern struct keyfile *InstallKey(void);
extern void RemoveKey(struct keyfile *key);
extern char *GetKeyLicensee(void);

#endif
