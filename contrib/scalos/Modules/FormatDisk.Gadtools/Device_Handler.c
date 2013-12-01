// Device_Handler.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <dos/dos.h>
#include <dos/filehandler.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <workbench/icon.h>
#include <devices/trackdisk.h>
#include <dos/rdargs.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/utility.h>

#include <defs.h>
#include "Format.h"

//-----------------------------------------------------------------------------

STRPTR NameOfDevice = "";		// +jmc+ : driver name copied here, problems found after twice analyzes for a same drive.

LONG TextHighCyl=0;
LONG TextLayoutBPT=0;

//-----------------------------------------------------------------------------

// Convert a volume name to a device name, and get information
// on the layout of the disk
//
BOOL volumeToDevName(BPTR volumeLock, char *dev, DriveLayout *layout)
	{
	struct DosList *dosList;
	char name[36];
	struct Process *process;
	APTR oldWdw;
	BOOL Found = FALSE;


	d1(KPrintF("%s/%s/%ld: START dev=<%s>\n", __FILE__, __FUNC__, __LINE__, dev));
	debugLock_d1(volumeLock);

	// Disable requestors during the execution of this function
	process = (struct Process *) FindTask(NULL);
	oldWdw = process->pr_WindowPtr;
	process->pr_WindowPtr = (APTR)(-1);

	// Get the DOS device list
	dosList = LockDosList(LDF_DEVICES|LDF_READ);

	// Go through each entry
	while (!Found && dosList != NULL)
		{
		dosList = NextDosEntry(dosList, LDF_DEVICES|LDF_READ);
		if (dosList == NULL) 
			break;

		d1(KPrintF("%s/%s/%ld: dol_Startup=%08lx  dol_Handler=%08lx  Type=%ld  Task=%08lx  Lock=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, dosList->dol_misc.dol_handler.dol_Startup,
			dosList->dol_misc.dol_handler.dol_Handler, dosList->dol_Type, dosList->dol_Task, dosList->dol_Lock));

		// If the node in the list is a volume
		if (dosList->dol_misc.dol_handler.dol_Startup > (BPTR) 1000 &&
			(dosList->dol_Task /* || dosList->dol_misc.dol_handler.dol_Handler */ || dosList->dol_misc.dol_handler.dol_SegList))
			{
			BOOL stat = FALSE;

			// Get the name of the device
			BtoCString(dosList->dol_Name, name, sizeof(name) - 1);
			strcat(name, ":");

			/*Get the information on the device*/
			/*		driveEnv = (struct DosEnvec *)
			BADDR(((struct FileSysStartupMsg *)
			BADDR(dosList->dol_misc.dol_handler.dol_Startup))->fssm_Environ);*/

			d1(KPrintF("%s/%s/%ld: dol_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

			// If the volume lock is NULL, the 'dev' is assumed to be the
			// name of a device holding an unformatted disk, so compare the
			// name to the name of the current node

			if (volumeLock == (BPTR)NULL)
				{
				stat = (Stricmp(dev, name) == 0);
				d1(KPrintF("%s/%s/%ld: stat=%ld dev=%s name=%s\n", __FILE__, __FUNC__, __LINE__, stat, dev,name));
				}
			else
				{
				// Otherwise, since 'dev' could hold a volume rather than
				// device name, get a lock on it and compare it to the lock
				// on the device that was given;  if they're the same, we've
				// found the drive
				BPTR tempLock;

				tempLock = Lock(name, ACCESS_READ);
				debugLock_d1(tempLock);
				if (tempLock)
					{
					ULONG same;

					same = SameLock(tempLock,volumeLock);
					d1(KPrintF("%s/%s/%ld: same=%ld\n", __FILE__, __FUNC__, __LINE__, same));
					stat = LOCK_SAME == same || LOCK_SAME_VOLUME == same;
					UnLock(tempLock);
					}
				}

			/*If weve found the drive, get the information on it*/
			if (stat)
				{
				// Get a pointer to the structure that holds the needed info
				struct FileSysStartupMsg *startup;
				struct DosEnvec *driveEnv;

				startup = (struct FileSysStartupMsg *) BADDR(dosList->dol_misc.dol_handler.dol_Startup);
				driveEnv = (struct DosEnvec *) BADDR(startup->fssm_Environ);

				// Get the information
				layout->unit = startup->fssm_Unit;

				// Copy the device name to layout->devName
				BtoCString(startup->fssm_Device, layout->devName, sizeof(layout->devName) - 1);
				d1(KPrintF("%s/%s/%ld: layout->devName=%s\n", __FILE__, __FUNC__, __LINE__, layout->devName));

				if (strlen(NameOfDevice) > 0)
					{
					d1(KPrintF("%s/%s/%ld: ** OK ** NameOfDevice lenght > 0 Len=[%ld]\n", __FILE__, __FUNC__, __LINE__, strlen(NameOfDevice)));

					strcpy(layout->devName, NameOfDevice);

					d1(KPrintF("%s/%s/%ld: ** SO ** layout.devName = NameofDevice=<%s>\n", __FILE__, __FUNC__, __LINE__, layout->devName));
					}
				else
					NameOfDevice = layout->devName;

				d1(KPrintF("%s/%s/%ld: NameOfDevice=<%s> layout->devName=<%s>\n", __FILE__, __FUNC__, __LINE__, NameOfDevice, layout->devName));


				layout->flags = startup->fssm_Flags;
				layout->memType = driveEnv->de_BufMemType;
				layout->lowCyl = driveEnv->de_LowCyl;
				layout->highCyl = driveEnv->de_HighCyl;
				layout->surfaces = driveEnv->de_Surfaces;
				layout->BPT = driveEnv->de_BlocksPerTrack;
				layout->blockSize = driveEnv->de_SizeBlock;

//---------------------------------------------------------------------------------------------------------------------------------
				TextHighCyl = layout->highCyl;
				TextLayoutBPT = layout->BPT;


				// Copy the device name back to 'dev'
				strcpy(dev,name);

				d1(KPrintF("%s/%s/%ld: FOUBND dev=<%s> name=<%s> flags=%lx memType=%lx lowCyl=%ld highCyl=%ld surfaces=%ld BlocksPerTrack=%ld blockSize=%ld TAILLE: [ %ld ]\n",
					__FILE__, __FUNC__, __LINE__, dev, name, layout->flags, layout->memType, layout->lowCyl, layout->highCyl,
					layout->surfaces, layout->BPT,	layout->blockSize, ((layout->highCyl + 1) * layout->BPT) ));

//--------------------------------------------------------------------------------------------------------------------------------------

				Found = TRUE;
				}
			}
		}

	d1(KPrintF("%s/%s/%ld: Found=%ld\n", __FILE__, __FUNC__, __LINE__, Found));

	// We didnt find the drive in the list, so unlock the list
	UnLockDosList(LDF_DEVICES|LDF_READ);

	// Restore the requester pointer
	process->pr_WindowPtr = oldWdw;

	// And return
	return Found;
	}

//-----------------------------------------------------------------------------

// Create a communications port linking this process to the drive
struct IOExtTD *OpenDrive(char *driveDevName,ULONG unit,ULONG flags)
	{
	struct MsgPort *diskPort;
	struct IOExtTD *diskRequest;

	// Create the message port
	if ((diskPort = CreateMsgPort()) != NULL)
		{
		// Create the IORequest
		diskRequest = (struct IOExtTD *)
			CreateIORequest(diskPort,sizeof(struct IOExtTD));
		if(diskRequest != NULL)
			{
			// Open the device, and return the IORequest if the device
			// opened successfully

			d1(KPrintF("%s/%s/%ld: driveDevName=<%s> unit=[%ld]\n", __FILE__, __FUNC__, __LINE__, driveDevName, unit));

			if(!OpenDevice(driveDevName,unit,(struct IORequest *)diskRequest,flags))
				return(diskRequest);

			// The device didnt open, so clean up
			DeleteIORequest((struct IORequest *)diskRequest);
			}

		DeleteMsgPort(diskPort);
		}

	// Return NULL to indicate that an error occurred
	return(NULL);
	}

//-----------------------------------------------------------------------------

// Close an open device and delete the accompanying port, etc.
void CloseDrive(struct IOExtTD *diskRequest)
	{
	CloseDevice((struct IORequest *)diskRequest);
	DeleteMsgPort(diskRequest->iotd_Req.io_Message.mn_ReplyPort);
	DeleteIORequest((struct IORequest *)diskRequest);
	}

//-----------------------------------------------------------------------------

// Convert a CSTR to a BSTR
BSTR makeBSTR(CONST_STRPTR in, char *out)
{
	int c;

	out[0] = strlen(in);
	for(c = 0; c < out[0]; c++) 
		out[c+1] = in[c];

	return MKBADDR(out);
}


//-----------------------------------------------------------------------------

// Convert a BSTR to a CSTR

STRPTR BtoCString(BPTR bString, STRPTR Buffer, size_t MaxLen)
{
#ifdef __AROS__
	// AROS needs special handling because it uses NULL-terminated
	// strings on some platforms.
	size_t Len = AROS_BSTR_strlen(bString);
	if (Len >= MaxLen)
		Len = MaxLen - 1;
	strncpy(Buffer, AROS_BSTR_ADDR(bString), Len);
	Buffer[Len] = '\0';

	return Buffer;
#else
	UBYTE *bStringAddr = BADDR(bString);
	size_t Length = bStringAddr[0];

	if (Length >= MaxLen)
		Length = MaxLen - 1;

	memcpy(Buffer, bStringAddr + 1, Length);
	Buffer[Length] = '\0';

	return Buffer;
#endif
}

//-----------------------------------------------------------------------------

