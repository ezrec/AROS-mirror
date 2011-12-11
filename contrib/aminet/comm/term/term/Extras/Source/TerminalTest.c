/*
**	TerminalTest.c
**
**	Terminal emulation test routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC BPTR					TestFile;
STATIC UBYTE				TestFileName[MAX_FILENAME_LENGTH];
STATIC struct MsgPort		*MicroPort;
STATIC struct timerequest	*MicroRequest;
STATIC BOOL					UseTestDelay;

STATIC VOID
MicroCleanup(VOID)
{
	if(MicroRequest)
	{
		if(MicroRequest->tr_node.io_Device)
			CloseDevice((struct IORequest *)MicroRequest);

		DeleteIORequest((struct IORequest *)MicroRequest);
		MicroRequest = NULL;
	}

	if(MicroPort)
	{
		DeleteMsgPort(MicroPort);
		MicroPort = NULL;
	}

}

STATIC BOOL
MicroSetup(VOID)
{
	if(MicroPort = CreateMsgPort())
	{
		if(MicroRequest = (struct timerequest *)CreateIORequest(MicroPort,sizeof(struct timerequest)))
		{
			if(!OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)MicroRequest,0))
				return(TRUE);
		}
	}

	MicroCleanup();

	return(FALSE);
}

VOID
StopTerminalTest()
{
	MicroCleanup();

	if(TestFile)
	{
		Close(TestFile);
		TestFile = BNULL;
	}

	if(MainJobQueue && TerminalTestJob)
		SuspendJob(MainJobQueue,TerminalTestJob);
}

VOID
StartTerminalTest(ULONG Qualifier)
{
	struct FileRequester *FileRequester;

	StopTerminalTest();

	if(FileRequester = OpenSingleFile(Window,"Open terminal test file",NULL,NULL,TestFileName,sizeof(TestFileName)))
	{
		if(TestFile = Open(TestFileName,MODE_OLDFILE))
		{
			if(MicroSetup())
			{
				if(Qualifier & IEQUALIFIER_RSHIFT)
					UseTestDelay = FALSE;
				else
					UseTestDelay = TRUE;

				ActivateJob(MainJobQueue,TerminalTestJob);
			}
			else
			{
				Close(TestFile);
				TestFile = BNULL;
			}
		}

		FreeAslRequest(FileRequester);
	}
}

BOOL
HandleTerminalTestJob(JobNode *UnusedJob)
{
	UBYTE LocalBuffer[40];
	LONG Len;

	Len = Read(TestFile,LocalBuffer,sizeof(LocalBuffer));

	if(Len > 0)
	{
		LONG i;

		if(UseTestDelay)
		{
			for(i = 0 ; i < Len ; i++)
			{
				MicroRequest->tr_node.io_Command	= TR_ADDREQUEST;
				MicroRequest->tr_time.tv_secs		= 0;
				MicroRequest->tr_time.tv_micro		= MILLION / 200;

				DoIO((struct IORequest *)MicroRequest);

				(*ConTransfer)(&LocalBuffer[i],1);
			}
		}
		else
		{
			for(i = 0 ; i < Len ; i++)
				(*ConTransfer)(&LocalBuffer[i],1);
		}
	}
	else
		StopTerminalTest();

	return(FALSE);
}
