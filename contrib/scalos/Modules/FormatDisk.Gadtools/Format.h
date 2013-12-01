/*
 * Format.h
 *
 * $Date$
 * $Revision$
 *
 */

#ifndef  FORMAT_H
#define  FORMAT_H

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif

#include <devices/trackdisk.h>
#include <workbench/startup.h>

typedef struct DriveLayout
{
	ULONG lowCyl,highCyl;
	ULONG surfaces;
	ULONG BPT;
	ULONG blockSize;
	ULONG unit;
	ULONG flags;
	ULONG memType;
	char  devName[256];
} DriveLayout;

struct FormatSettings
	{
	CONST_STRPTR newName;
	CONST_STRPTR TrashName;
	ULONG ffs;
	BOOL quick;
	BOOL verify;
	BOOL icon;
	};

//-----------------------------------------------------------------------------

typedef enum prepResult {eOK,eCancel,eQuit, eQuick} prepResult;

// int main(void);

//-----------------------------------------------------------------------------

void formatVolume(BPTR *volumeLock,
		CONST_STRPTR volumeName,
		const struct FormatSettings *Parms,
		char *statString);

BOOL doFullFormat(DriveLayout *layout,
	char *statString,
	struct IOExtTD *io1);

// BOOL askAreYouSure(char *volumeName, BOOL truncColon);
BOOL askAreYouSure(char *volumeName, BOOL truncColon, char *DeviceSize);
// BOOL askAreYouSure(char *volumeName, BOOL truncColon, char *BodyText);

BOOL alertIsWriteProtected(CONST_STRPTR devName);

void printError(CONST_STRPTR first, CONST_STRPTR second, CONST_STRPTR third);

void getVolumeName(char *name, struct WBArg *argList, UWORD disk);

void cleanup(ULONG err);

prepResult getPrepInput(void);

BOOL volumeToDevName(BPTR volumeLock, char *dev, DriveLayout *layout);

struct IOExtTD *OpenDrive(char *driveDevName, ULONG unit, ULONG flags);

void CloseDrive(struct IOExtTD *diskRequest);

BSTR makeBSTR(CONST_STRPTR in, char *out);
STRPTR BtoCString(BPTR bString, STRPTR Buffer, size_t MaxLen);

void parseArgs(char *drive,
	char *newName,
	BOOL *ffs,
	BOOL *intl,
	BOOL *icons,
	char *TrashName,
	BOOL *quick,
	BOOL *verify);

//-----------------------------------------------------------------------------

BOOL updateStatWindow(char *string, UWORD percent);
CONST_STRPTR GetLocString(ULONG MsgId);

//-----------------------------------------------------------------------------

extern STRPTR NameOfDevice;
extern STRPTR ShortDostype;

extern BOOL fromcli;

// Debug swithes 
#define d1(x)		;
#define d2(x)		x;

#define	debugLock_d1(LockName) ;
#define	debugLock_d2(LockName) \
	{\
	char xxName[200];\
	strcpy(xxName, "");\
	NameFromLock((LockName), xxName, sizeof(xxName));\
	kprintf(__FILE__ "/%s/%ld: " #LockName "=%08lx <%s>\n", __FUNC__, __LINE__, LockName, xxName);\
	}


// From debug lib 
extern int kprintf(CONST_STRPTR, ...);
extern int KPrintF(CONST_STRPTR, ...);

//----------------------------------------------------------------------------

struct FormatDisk_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

//-----------------------------------------------------------------------------

#endif /* FORMAT_H */


