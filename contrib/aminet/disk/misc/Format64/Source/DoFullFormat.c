#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <dos/dos.h>
#include <dos/filehandler.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <workbench/icon.h>
#include <devices/newstyle.h>
#include <devices/trackdisk.h>
#include <dos/rdargs.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/graphics.h>

#include "Format.h"
#include "string.h"
#include "stdio.h"

/* External symbols */

extern BOOL FFS;
extern BOOL QuickFmt;
extern BOOL Verify;
extern BOOL Icon;
extern struct Library *GadToolsBase;
extern struct Library *IconBase;
extern BPTR StdErr;
extern LONG args[7];
extern struct WBStartup *WBenchMsg;

/* Internal prototypes for this specific C file.*/

BOOL checkfor64bitcommandset(struct IOStdReq *io);

void nsd_add(ULONG *bf, ULONG value1, ULONG value2) {
#warning TODO: maybe use QUAD for this

	value2 += value1;
	if (value1>value2)
		bf[0] += 1;
	bf[1] = value2;
}

/*This function does a full (low-level) format of a disk*/
BOOL doFullFormat(DriveLayout *layout,char *statString,char *devName,
		     struct IOExtTD *io1)
	{
   ULONG nsdbuffer[2], nsdbuffer2[2];
   ULONG trackSize, numTracks, i, track;
	UWORD formatcommand = TD_FORMAT, verifycommand = CMD_READ;
   int c;
   UBYTE error=2, *write = NULL;
   BOOL errorB = FALSE, command64 = FALSE;

   /*Get the size of a track (#surfaces*#blocks*#longwordsPerTrack*4*/
   trackSize=layout->surfaces*layout->blockSize*4*layout->BPT;

	command64 = checkfor64bitcommandset((struct IOStdReq *)io1);
	if (command64 == TRUE)
		{
		formatcommand = NSCMD_TD_FORMAT64;
		verifycommand = NSCMD_TD_READ64;
		}

	nsdbuffer[0] = 0; nsdbuffer[1] = 0;

   /*Allocate enough memory for one track*/
   write = (UBYTE*)AllocMem(trackSize, layout->memType|MEMF_CLEAR);
   if (write != NULL)
		{
      /*Initialize the IORequest*/
//      io1->iotd_Req.io_Data=write;
 //     io1->iotd_Req.io_Length=trackSize;


      /*Get the starting byte position in the volume*/
		for (i=0; i<layout->lowCyl; i++)
			nsd_add((ULONG*)&nsdbuffer, nsdbuffer[1], trackSize);

		nsdbuffer2[0] = nsdbuffer[0];
		nsdbuffer2[1] = nsdbuffer[1];

		for (i=layout->lowCyl; i<layout->highCyl; i++)
			nsd_add((ULONG*)&nsdbuffer2, nsdbuffer2[1], trackSize);

//	printf("start: %u %u end: %u %u\n",
//		nsdbuffer[0], nsdbuffer[1],
//		nsdbuffer2[0], nsdbuffer2[1]);

		if (nsdbuffer2[0] != 0)
			if (command64 == FALSE)
				{
				printError("You are trying to format beyond 4GB border although\nyour device does not support 64 bit commands. Due to this\nfact, format operation is aborted in order to save your drive.", NULL, NULL);
				FreeMem(write, trackSize);
				return(FALSE);
				}
			else
				{
				formatcommand = NSCMD_TD_FORMAT64;
				verifycommand = NSCMD_TD_READ64;
				}

      numTracks=layout->highCyl-layout->lowCyl+1;

      /*Clear the status window*/
      error=(updateStatWindow(" ",0)) ? 0 : error;

      /*For each track*/
      for(track=0;track<numTracks && error!=0;track++)
			{
			/*Setup to format the disk*/
			io1->iotd_Req.io_Data    = write;
			io1->iotd_Req.io_Length  = trackSize;
			io1->iotd_Req.io_Command = formatcommand;
			io1->iotd_Req.io_Actual  = (ULONG)nsdbuffer[0];
			io1->iotd_Req.io_Offset  = (ULONG)nsdbuffer[1];
/*
			printf("%u %u %u %u command $%lx\n",
				nsdbuffer[0],
				io1->iotd_Req.io_Actual,
				nsdbuffer[1],
				io1->iotd_Req.io_Offset,
				io1->iotd_Req.io_Command);
*/
			/*Update the status window*/
			sprintf(statString, "Formatting cylinder %ld, %ld to go  ",
				track+layout->lowCyl, numTracks-track-1);

			error = (updateStatWindow(statString,track*1000/numTracks)) ?0:error;

			/*If the user didnt press Stop*/
			if(error != 0)
				{
				error = (DoIO((struct IORequest *)io1)) ? 0 : error;
				if(error == 0)
					{
					printError("\nA formatting error occured",NULL,NULL);
					errorB = TRUE;
					}
				}

			/*If we're suppossed to verify, and the user didn't press 'Stop'*/
			if(Verify == TRUE && error != 0)
				{
				/*Setup the same IORequest to do a read*/
				io1->iotd_Req.io_Data    = write;
				io1->iotd_Req.io_Length  = trackSize;
				io1->iotd_Req.io_Command = verifycommand;
				io1->iotd_Req.io_Actual  = (ULONG)nsdbuffer[0];
				io1->iotd_Req.io_Offset  = (ULONG)nsdbuffer[1];

				/*Update the status*/
				sprintf(statString, "Verifying  cylinder %ld, %ld to go  ",
					track+layout->lowCyl, numTracks-track-1);

				error = (updateStatWindow(statString,
					( (1+(track*2))*1000)/(2*numTracks) )) ? 0 : error;


				/*If the user hasnt pressed Stop*/
				if(error != 0)
					{
					error = (DoIO((struct IORequest *)io1)) ? 0 : error;
					if (error == 0)
						{
						printError("\nA verify error occurred",NULL,NULL);
						errorB=TRUE;
						}
					}

				/*Check the data that was read back.*/
				if (error != 0)
					if (errorB == FALSE)
						for(c=0; c<trackSize; c++)
							{
							if (write[c] != 0)
								{
								errorB = TRUE;
								printError("\nA verify error occurred",NULL,NULL);
								break;
								}
							write[c]=0;
							}
				} /* End of Verify */

			if (error == 0 || errorB == TRUE) break;
			else { error = 2; errorB = FALSE;
			       nsd_add((ULONG*)&nsdbuffer, nsdbuffer[1], trackSize); }

			} /* End format loop */

		/*Were done, so free the track memory*/
		FreeMem(write,trackSize);
		if (error == 0 || errorB == TRUE) return(FALSE);
		}
   else printError("Couldn't allocate write buffer",NULL,NULL);

   return(TRUE);
	}
