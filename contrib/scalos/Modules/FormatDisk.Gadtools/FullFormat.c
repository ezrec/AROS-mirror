// DoFullFormat.c
// $Date$
// $Revision$


#include <string.h>
#include <stdio.h>

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

#include <devices/newstyle.h>

#include <defs.h>

#include "Format.h"
#define	FormatDisk_NUMBERS
#include STR(SCALOSLOCALE)

//-----------------------------------------------------------------------------

#ifdef __GNUC__

typedef	unsigned long long ULONG64;

// Macros to acccess one of the longs of an ULONG64
#define	ULONG64_LOW(long64)	((ULONG) ((long64) & 0xffffffff))
#define	ULONG64_HIGH(long64)	((ULONG) (((long64) >> 32) & 0xffffffff))

#else /* __GNUC__ */

typedef	struct { ULONG High, Low; } ULONG64;


// Macros to acccess one of the longs of an ULONG64
#define	ULONG64_LOW(long64)	((long64).Low)
#define	ULONG64_HIGH(long64)	((long64).High)

#endif /* __GNUC__ */

//-----------------------------------------------------------------------------

//--- External symbols
extern BOOL FFS;
extern BOOL QuickFm;
extern BOOL Verify;
extern BOOL Icon;

static ULONG64 Make64( ULONG n );
static ULONG64 Incr64(ULONG64 x, ULONG n );

//--- NSD Functions
BOOL checkfor64bitcommandset(struct IOStdReq *io);

//-----------------------------------------------------------------------------

// This function does a full (low-level) format of a disk
BOOL doFullFormat(DriveLayout *layout, char *statString, struct IOExtTD *io1)
{
	ULONG64 nsdbuffer, nsdbuffer2;
	ULONG trackSize, numTracks, i, track;
	UWORD formatcommand = TD_FORMAT;
	UWORD verifycommand = CMD_READ;
	int c;
	UBYTE error = 2, *write; 			// +jmc+ not = NULL
	BOOL errorB = FALSE, command64; 	// +jmc+ not = FALSE;


	// Get the size of a track (#surfaces*#blocks*#longwordsPerTrack*4)
	trackSize = layout->surfaces*layout->blockSize*4*layout->BPT;

	command64 = checkfor64bitcommandset((struct IOStdReq *)io1);
	if (command64 == TRUE)
		{
		formatcommand = NSCMD_TD_FORMAT64;
		verifycommand = NSCMD_TD_READ64;
		}

	nsdbuffer = Make64(0);

	// Allocate enough memory for one track
	write = (UBYTE*)AllocMem(trackSize, layout->memType|MEMF_CLEAR);
	if (write != NULL)
		{
		// Initialize the IORequest*/
//      	io1->iotd_Req.io_Data = write;
//      	io1->iotd_Req.io_Length = trackSize;


		// Get the starting byte position in the volume
		for (i = 0; i<layout->lowCyl; i++)
			nsdbuffer = Incr64(nsdbuffer, trackSize);

		nsdbuffer2 = nsdbuffer;

		for (i = layout->lowCyl; i<layout->highCyl; i++)
			nsdbuffer2 = Incr64(nsdbuffer2, trackSize);

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: start: nsdbuffer[0]=%u nsdbuffer[1]=%u end: nsdbuffer2[0]=%u nsdbuffer2[1]=%u\n", \
			__LINE__, ULONG64_HIGH(nsdbuffer), ULONG64_LOW(nsdbuffer), ULONG64_HIGH(nsdbuffer2), ULONG64_LOW(nsdbuffer2)));

		if (ULONG64_HIGH(nsdbuffer2) != 0)
			{
			if (command64 == FALSE)
				{
				printError((STRPTR) GetLocString(MSGID_WARNING_FORMAT_DEVICE_NO64BIT), NULL, NULL);
				FreeMem(write, trackSize);
				return(FALSE);
				}
			else
				{
				formatcommand = NSCMD_TD_FORMAT64;
				verifycommand = NSCMD_TD_READ64;
				}
			}

		numTracks = layout->highCyl-layout->lowCyl+1;

		// Clear the status window
		error = (updateStatWindow(" ",0)) ? 0 : error;

		//  For each track
		for (track = 0;track < numTracks && error != 0;track++)
			{
			// Setup to format the disk
			io1->iotd_Req.io_Data    = write;
			io1->iotd_Req.io_Length  = trackSize;
			io1->iotd_Req.io_Command = formatcommand;
			io1->iotd_Req.io_Actual  = ULONG64_HIGH(nsdbuffer);
			io1->iotd_Req.io_Offset  = ULONG64_LOW(nsdbuffer);

			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: iotd_Req.io_Data=<%u> iotd_Req.io_Length=<%u> command=$%lx iotd_Req.io_Actual=<%u> iotd_Req.io_Offset=<%u> write=%08lx\n", \
				__LINE__, io1->iotd_Req.io_Data, io1->iotd_Req.io_Length, io1->iotd_Req.io_Command, io1->iotd_Req.io_Actual, io1->iotd_Req.io_Offset, write));

			// Update the status window
			sprintf(statString, (STRPTR) GetLocString(MSGID_GUI_FORMATTING_CYLINDER),
				track+layout->lowCyl, numTracks, numTracks-track-1);

			error = (updateStatWindow(statString,track*1000/numTracks)) ? 0 :error;

			// If the user did not press Stop
			if (error != 0)
				{
				error = (DoIO((struct IORequest *)io1)) ? 0 : error;
				if (error == 0)
					{
					printError((STRPTR) GetLocString(MSGID_FORMATTING_ERROR),NULL,NULL);
					errorB = TRUE;
					}
				}
			else
				{
				updateStatWindow((STRPTR) GetLocString(MSGID_GUI_FORMATTING_ABORTED),1000);
				}

			// If we're suppossed to verify, and the user didn't press 'Stop'
			if (Verify == TRUE && error != 0)
				{
				// Setup the same IORequest to do a read
				io1->iotd_Req.io_Data    = write;
				io1->iotd_Req.io_Length  = trackSize;
				io1->iotd_Req.io_Command = verifycommand;
				io1->iotd_Req.io_Actual  = ULONG64_HIGH(nsdbuffer);
				io1->iotd_Req.io_Offset  = ULONG64_LOW(nsdbuffer);

				d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: iotd_Req.io_Data=<%u> iotd_Req.io_Length=<%u> command=$%lx iotd_Req.io_Actual=<%u> iotd_Req.io_Offset=<%u>\n", \
					__LINE__, io1->iotd_Req.io_Data, io1->iotd_Req.io_Length, io1->iotd_Req.io_Command, io1->iotd_Req.io_Actual, io1->iotd_Req.io_Offset));

				// Update the status
				sprintf(statString, (STRPTR) GetLocString(MSGID_GUI_VERIFYING_CYLINDER),
					track+layout->lowCyl, numTracks, numTracks-track-1);

				error = (updateStatWindow(statString, ( (1+(track*2))*1000)/(2*numTracks) )) ? 0 : error;

				// If the user hasnt pressed Stop
				if (error != 0)
					{
					error = (DoIO((struct IORequest *)io1)) ? 0 : error;
					if (error == 0)
						{
						printError((STRPTR) GetLocString(MSGID_VERIFYING_ERROR),NULL,NULL);
						errorB = TRUE;
						}
					}
				else
					{
					updateStatWindow((STRPTR) GetLocString(MSGID_GUI_VERIFYING_ABORTED),1000);
					}

				// Check the data that was read back.
				if (error != 0)
					if (errorB == FALSE)
						for (c=0; c<trackSize; c++)
							{
							if (write[c] != 0)
								{
								errorB = TRUE;
								printError((STRPTR) GetLocString(MSGID_VERIFYING_ERROR),NULL,NULL);
								break;
								}
							write[c] = 0;
							}
				} // End of Verify

				if (error == 0 || errorB == TRUE)
					break;
				else
					{
					error = 2; 
					errorB = FALSE;
					nsdbuffer = Incr64(nsdbuffer, trackSize);
					}
			} // End format loop

		// Were done, so free the track memory
		FreeMem(write,trackSize);
		if (error == 0 || errorB == TRUE)
			return(FALSE);
		}
	else
		printError("Couldn't allocate write buffer",NULL,NULL);

	return(TRUE);
}

/*----------------------------------------------------------------------
* Name:    Make64()
*
* Action:  Construct a 64-bit integer from a 32-bit integer.
*
* Input:   n - 32 bit integer to be used to construct 64-bit integer.
*
* Return
* Value:   Constructed 64-bit integer.
*
* Side
* Effects: None.
*
*---------------------------------------------------------------------*/
static ULONG64 Make64( ULONG n )
{
#ifdef __GNUC__
	return (ULONG64) n;
#else /* __GNUC__ */
	ULONG64 result;

	result.High = 0;
	result.Low = n;

	return ( result );
#endif /* __GNUC__ */
} /* Make64() */


/*----------------------------------------------------------------------
* Name:    Incr64()
*
* Action:  Increment a 64-bit integer by some 32-bit integer.
*
* Input:   x - 64-bit integer to be incremented.
*          n - 32-bit increment.
*
* Return
* Value:   Result of increment operation.
*
* Side
* Effects: None.
*
* Notes:   Assume no overflow of sum.
*          Rewritten for efficiency
*---------------------------------------------------------------------*/
static ULONG64 Incr64(ULONG64 x, ULONG n )
{
#ifdef __GNUC__
	return x + n;
#else /* __GNUC__ */
	x.Low += n;			/* add lo order word */

	if ( x.Low < n )			/* did the add cause an overflow? */
		x.High++;			/* yes, adjust hi word for carry */

	return ( x );
#endif /* __GNUC__ */
} /* Incr64() */

