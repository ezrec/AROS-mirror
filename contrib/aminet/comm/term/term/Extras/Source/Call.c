/*
**	Call.c
**
**	Call log file maintenance routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Some local variables. */

STATIC BPTR				CallFile;
STATIC struct timeval	CallTime;
STATIC PhoneLogHandle *	LogHandle;
STATIC BOOL				CallActive;

	/* InitPhoneLogEntry(struct PhoneLogEntry *Item,STRPTR Number,STRPTR Name):
	 *
	 *	Initialize a log entry for the PhoneLog support routines.
	 */

STATIC VOID
InitPhoneLogEntry(struct PhoneLogEntry *Item,CONST_STRPTR Number,CONST_STRPTR Name)
{
	struct ClockData ClockData;
	struct timeval Now;

	GetSysTime(&Now);
	Amiga2Date(Now.tv_secs,&ClockData);

	memset(Item,0,sizeof(struct PhoneLogEntry));

	if(Number)
	{
		LimitedStrcpy(sizeof(Item->Number),Item->Number,Number);

		if(Number == Name)
			Name = "???";
	}

	if(Name)
		LimitedStrcpy(sizeof(Item->Name),Item->Name,Name);

	strcpy(Item->Reason,"-");

	strcpy(Item->ProgramName,"term");

	Item->ProgramVersion	= TermVersion;
	Item->ProgramRevision	= TermRevision;

	Item->StartDay		= ClockData.mday;
	Item->StartMonth	= ClockData.month;
	Item->StartYear		= ClockData.year;
	Item->StartHour		= ClockData.hour;
	Item->StartMin		= ClockData.min;
	Item->StartSec		= ClockData.sec;

	Item->EndDay		= ClockData.mday;
	Item->EndMonth		= ClockData.month;
	Item->EndYear		= ClockData.year;
	Item->EndHour		= ClockData.hour;
	Item->EndMin		= ClockData.min;
	Item->EndSec		= ClockData.sec;
}

	/* CallDate():
	 *
	 *	Add the current date and time to the logfile.
	 */

STATIC VOID
CallDate(LONG Seconds,BPTR FileHandle)
{
		/* Days of the week. */

	STATIC STRPTR CallDays[7] =
	{
		"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
	};

		/* Months of the year. */

	STATIC STRPTR CallMonths[12] =
	{
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
	};

	struct ClockData ClockData;

		/* Convert time and date. */

	Amiga2Date(Seconds,&ClockData);

		/* Add the date line. */

	FPrintf(FileHandle,"%s %s %02ld %02ld:%02ld:%02ld %ld\n",CallDays[ClockData.wday],CallMonths[ClockData.month - 1],ClockData.mday,ClockData.hour,ClockData.min,ClockData.sec,ClockData.year);
}

	/* MakeCall(struct PhoneEntry *Entry):
	 *
	 *	Register a new phone call.
	 */

VOID
MakeCall(CONST_STRPTR Name,CONST_STRPTR Number)
{
		/* End previous entry. */

	if(CallActive)
		StopCall(FALSE);

		/* Get current system time. */

	GetSysTime(&CallTime);

		/* Call logging enabled? */

	if(Config->CaptureConfig->LogCall && Config->CaptureConfig->CallLogFileName[0])
	{
		if(Config->CaptureConfig->LogFileFormat == LOGFILEFORMAT_CallInfo)
		{
				/* Open logfile for writing. */

			if(CallFile = OpenToAppend(Config->CaptureConfig->CallLogFileName,NULL))
			{
					/* Add the title line. */

				FPrintf(CallFile,"%s (%s)\n--------------------------------\nLogin:  ",Name,Number);

					/* Make the line complete. */

				CallDate(CallTime.tv_secs,CallFile);

				CallActive = TRUE;
			}
		}
		else
		{
				/* Open the log file */

			if(LogHandle = OpenPhoneLog(Config->CaptureConfig->CallLogFileName))
			{
				struct PhoneLogEntry Item;

					/* Set up a log entry */

				InitPhoneLogEntry(&Item,Name,Number);

					/* And write it */

				WritePhoneLogStartEntry(LogHandle,&Item);

				CallActive = TRUE;
			}
		}
	}
}

	/* StopCall(BOOLEAN Finish):
	 *
	 *	End the current phone call.
	 */

VOID
StopCall(BOOL Finish)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	BOOL GotName = FALSE;

		/* Is a call currently being made? */

	if(CallFile)
	{
		struct timeval StopTime;
		LONG Seconds;

			/* Get current system time. */

		GetSysTime(&StopTime);
		Seconds = StopTime.tv_secs;

			/* Subtract the starting time from it. */

		SubTime(&StopTime,&CallTime);

			/* Add the info line. */

		if(Finish)
			FPrintf(CallFile,"*** term exited before logout: ");
		else
			FPrintf(CallFile,"Logout: ");

			/* Make the line complete. */

		CallDate(Seconds,CallFile);

			/* Get the file name. */

		GotName = NameFromFH(CallFile,LocalBuffer,sizeof(LocalBuffer));

			/* Add the online time. */

		FPrintf(CallFile,"Time online: %02ld:%02ld:%02ld\n\n",(StopTime.tv_secs % 86400) / 3600,(StopTime.tv_secs % 3600) / 60,StopTime.tv_secs % 60);

			/* Finis... */

		Close(CallFile);
		CallFile = BNULL;
	}

		/* Now what about the PhoneLog file? */

	if(LogHandle)
	{
		struct PhoneLogEntry Item;

			/* Get the file name. */

		GotName = NameFromFH(LogHandle->file,LocalBuffer,sizeof(LocalBuffer));

		InitPhoneLogEntry(&Item,NULL,NULL);

		WritePhoneLogEndEntry(LogHandle,&Item);
		ClosePhoneLog(LogHandle);

		LogHandle = NULL;
	}


		/* Clear the `executable' bit. */

	if(GotName)
	{
		AddProtection(LocalBuffer,FIBF_EXECUTE);

		if(Config->MiscConfig->CreateIcons)
			AddIcon(LocalBuffer,FILETYPE_TEXT,TRUE);
	}

		/* No call is active now */

	CallActive = FALSE;
}
