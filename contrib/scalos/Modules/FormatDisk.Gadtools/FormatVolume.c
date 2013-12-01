// FormatVolume.c
// $Date$
// $Revision$
// $Id$


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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

#include <defs.h>

#include "Format.h"
#define	FormatDisk_NUMBERS
#include STR(SCALOSLOCALE)

//----
extern ULONG FFS;
extern BOOL QuickFmt;
extern BOOL Verify;
extern BOOL Icon;

extern char temp[];

char TheDeviceName[64];
BOOL NsdSupport = FALSE;
char deviceName[64];

UBYTE MakePrintable(UBYTE ch);

BOOL checkfor64bitcommandset(struct IOStdReq *io);

//-----------------------------------------------------------------------------

// This functions handles the low-level format, the high-level format, the
// creation of icons, etc.
void formatVolume(BPTR *volumeLock, CONST_STRPTR volumeName, 		
	const struct FormatSettings *Parms, char *statString)
{
	struct IOExtTD *io1;
	BOOL fmtResult = TRUE;
	int result = 0;
	char temp2[4];
	DriveLayout layout;
	struct MsgPort *devPort;
	BOOL writeProtCont = TRUE;
	char typeDOS[8];

	// If the volume lock is NULL, assume that the volumeName string holds
	// a valid device pointer.
	if (*volumeLock == (BPTR)NULL)
		{
		strcpy(deviceName,volumeName);
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: deviceName=<%s>\n", __LINE__, deviceName));
		}

	// Get the drive name (if possible)
	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: * GET *** volumeToDevName(%08lx,%s,%lx) * BUT WHY ??\n", __LINE__, *volumeLock,deviceName,&layout));

        d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: ** CHECK ** NameOfDevice=<%s> Len=<%ld>\n", __LINE__, NameOfDevice, strlen(NameOfDevice)));


	if (!volumeToDevName(*volumeLock,deviceName,&layout))
		{
		printError((STRPTR) GetLocString(MSGID_DRIVE_NOFOUND),NULL,NULL);
		return;
		}

	strcpy(TheDeviceName, deviceName);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: * CHECK DEVICENAME: deviceName=<%s> TheDeviceName=<%s>\n", __LINE__, deviceName, TheDeviceName));

	// This port will be used to communicate with the filesystem
	devPort = DeviceProc(deviceName);
	if (devPort == NULL)
		{
		printError((STRPTR) GetLocString(MSGID_DRIVE_NOFOUND),NULL,NULL);
		return;
		}

	// Inhibit the drive
	DoPkt1(devPort,ACTION_INHIBIT,DOSTRUE);

	// If we got a lock to the volume that we're going to format it, destroy
	// it, since the volume that it points to is about to be erased anyway.
	if (*volumeLock != (BPTR)NULL)
		{
		UnLock(*volumeLock);
		*volumeLock = (BPTR)NULL;
		}

	// Open the disk device.
	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: * CALL *** OpenDrive(%s,%ld,%lx)\n", \
		__LINE__, layout.devName, layout.unit, layout.flags));

	if ((io1 = OpenDrive(layout.devName, layout.unit, layout.flags)) != NULL)
		{
		// Determine the write protect status.
		io1->iotd_Req.io_Data = NULL;
		io1->iotd_Req.io_Length = 0;
		io1->iotd_Req.io_Command = TD_PROTSTATUS;
		DoIO((struct IORequest *)io1);

		// Loop while the disk stays protected and user keeps pressing Retry.
		while (io1->iotd_Req.io_Actual != 0 && (writeProtCont = alertIsWriteProtected(volumeName)))
			DoIO((struct IORequest *)io1);

		// If the disk is not write-protected.
		if (writeProtCont)
			{
			// Do a full format if the user didnt select the Quick option.
			if (checkfor64bitcommandset((struct IOStdReq *)io1) == TRUE)
				{
				NsdSupport = TRUE;
				if (fromcli)
					printf((STRPTR) GetLocString(MSGID_CLI_SUPPORT_64CMD), deviceName);
				}
			else
				{
				if (fromcli)
					printf((STRPTR) GetLocString(MSGID_CLI_NO_SUPPORT_64CMD), deviceName);
				}

			if (fromcli)
				{
				Write(Output(),(STRPTR) GetLocString(MSGID_CLI_INSERT_DISK), strlen((STRPTR) GetLocString(MSGID_CLI_INSERT_DISK))); // 41
				Write(Output(),temp,strlen(temp));
				Write(Output(),(STRPTR) GetLocString(MSGID_CLI_PRESS_RETURN), strlen((STRPTR) GetLocString(MSGID_CLI_PRESS_RETURN))); // 18
				Read(Input(),temp2,1);
				}

			if ( (SetSignal(0,0) & SIGBREAKF_CTRL_C) != SIGBREAKF_CTRL_C)
				{
				if (!Parms->quick)
					fmtResult = doFullFormat(&layout, statString, io1);
				if (fmtResult == TRUE)
					{
					// Write an extra carriage return, if run from the CLI.
					if (fromcli)
						Write(Output(),"\n",1);

					// Tell the user that were doing the high-level format.
				   	fmtResult = !updateStatWindow((STRPTR) GetLocString(MSGID_INITIALIZING_DISK),1000);

					// If the user hasnt clicked on Stop, do the high-level format.
					if (fmtResult)
						{
						// Volume name buffer to use with MKBADDR() below.
						char mybuf[128];

						strcpy(mybuf, " ");
						strcat(mybuf, Parms->newName);

						d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: mybuf=<%s>\n", __LINE__, mybuf));

						// result = Format(deviceName,newName, ffs ? ID_FFS_DISK : ID_DOS_DISK);

						// First disk initializing test. If trouble DO: "ACTION_FORMAT" via Dopkt2() below ??.
						result = Format(deviceName, Parms->newName, Parms->ffs);
						
						if (result)
							updateStatWindow((STRPTR) GetLocString(MSGID_INITIALIZING_DISK_SUCCESS),1000);
						else
							{
							updateStatWindow((STRPTR) GetLocString(MSGID_INITIALIZING_DISK_FAILED),1000);


							// Something failed try "ACTION_FORMAT" via Dopkt2() ??
							result = DoPkt2(devPort, ACTION_FORMAT, (IPTR)MKBADDR(mybuf), Parms->ffs);
							
							updateStatWindow((STRPTR) GetLocString(MSGID_SECOND_INITIALIZING_DISK),1000);
							if (result)
								updateStatWindow((STRPTR) GetLocString(MSGID_INITIALIZING_DISK_SUCCESS),1000); // Comment "on PASS 2" will be removed after severals tests.
							else
								updateStatWindow((STRPTR) GetLocString(MSGID_INITIALIZING_DISK_FAILED),1000);
							}

						sprintf(typeDOS, "%c%c%c%c",
									MakePrintable((Parms->ffs >> 24) & 0xff),
									MakePrintable((Parms->ffs >> 16) & 0xff),
									MakePrintable((Parms->ffs >>  8) & 0xff),
									MakePrintable(Parms->ffs & 0xff));


						d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: FORMATTED: DRIVE deviceName<%s> AS newName=<%s> DOSTYPE: ffs=«$%lx» [%s]\n", \
							__LINE__, deviceName, newName, ffs, typeDOS));
						}
					}
				}
			else
				{
				d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: [SIGBREAKF_CTRL_C]\n", __LINE__));
				Write(Output(),(STRPTR) GetLocString(MSGID_CLI_BREAK), strlen((STRPTR) GetLocString(MSGID_CLI_BREAK))); // 9
				}
			}
		else
			printError("Cannot format volume", volumeName, "because the disk is write protected");

		// Close the disk device.
		CloseDrive(io1);
		}
	else
		{
		printError((STRPTR) GetLocString(MSGID_DEVICE_NOT_ACCESS),NULL,NULL);

		if (devPort)
			DoPkt1(devPort,ACTION_INHIBIT,DOSFALSE);  // ADDED: If error occurred, drives were always inhibited.
		return;
		}

	// Uninhibit the drive
	DoPkt1(devPort,ACTION_INHIBIT,DOSFALSE);
 

	// Wait for the drive to come on line, but a better way could be apply here!
	Delay(50);

	// If the disk was never unprotected, return.
	if (!writeProtCont)
		{
		return;
		}

	// If the format was successful and the user wants icons created.
	if (Parms->icon && result == DOSTRUE)
		{
		struct DiskObject *trashIcon;
		BPTR trashLock;
		size_t TrashLen;


		TrashLen = strlen(Parms->TrashName);

		// User can choose its own Trashcan name.
		if (TrashLen > 0)
			sprintf(statString, (STRPTR) GetLocString(MSGID_CREATING_TRASHCAN_USER), Parms->TrashName);
		else
			strcpy(statString, (STRPTR) GetLocString(MSGID_CREATING_TRASHCAN_DEFAULT));

		// Update the user
		fmtResult = !updateStatWindow(statString ,1000);

		// If she didnt press Stop.
		if (fmtResult)
			{
			// Create the trashcan name (<volume>:Trashcan).
			Delay(120);	// Wait a little, drive can be yet inhibited.

			strcpy(deviceName, Parms->newName);
			strcat(deviceName,":");

			if (TrashLen > 0)
				strcat(deviceName, Parms->TrashName);
			else
				strcat(deviceName, "Trashcan");

			// Create the trashcan directory.
			trashLock = CreateDir(deviceName);

			// If it was successfully created.
			if (trashLock != (BPTR)NULL)
				{
				UnLock(trashLock);
				// Get the trashcan icon.
				trashIcon = GetDefDiskObject(WBGARBAGE);

				// If there wasnt an error.
				if (trashIcon != NULL)
					{
					// Write the icon to disk.
					if (!PutDiskObject(deviceName,trashIcon))
						printError((STRPTR) GetLocString(MSGID_CREATING_TRASHCAN_ERROR), NULL,NULL);

					// and free it.
					FreeDiskObject(trashIcon);
					}
				else
					{
					// printf("ERROR NUMBER = %ld\n", IoErr());
					printError((STRPTR) GetLocString(MSGID_CREATING_TRASHCAN_ERROR), NULL,NULL);
					}
				}
			else
				{
				// printf("ERROR NUMBER = %ld\n", IoErr());
				printError((STRPTR) GetLocString(MSGID_CREATING_TRASHCAN_DIR_ERROR), NULL,NULL);
				}
			}
		}

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: NsdSupport=%ld deviceName=<%s>\n", __LINE__, NsdSupport, deviceName));

	return;
}

//----------------------------------------------

UBYTE MakePrintable(UBYTE ch)
{
	if (ch < ' ')
		ch += '0';

	return ch;
}

