/*
**	Flow.c
**
**	Data flow scanner routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Hints for the data flow scanner. */

STATIC WORD	ScanStart,ScanEnd;

STATIC WORD	AttentionCount[SCAN_COUNT],AttentionLength[SCAN_COUNT],FlowCount;

	/* FlowFilter(STRPTR Data,LONG Size,UBYTE Mask):
	 *
	 *	Data flow filter.
	 */

LONG
FlowFilter(UBYTE *Data,LONG Size,UBYTE Mask)
{
	LONG OriginalSize;
	UBYTE c;

		/* We will return the number of bytes in the buffer
		 * after processing it.
		 */

	OriginalSize = Size;

		/* Run until done. */

	do
	{
		if(c = (*Data++ & Mask))
		{

				/* We already got a `CONNECT';
				 * continue scanning the serial output
				 * data for the actual baud rate.
				 */

			if(BaudPending)
			{
				BOOL GotIt;

					/* Check if we found a line terminator. */

				if(c < ' ')
					GotIt = TRUE;
				else
				{
					GotIt = FALSE;

						/* Check if can add another character to the
						 * buffer.
						 */

					if(BaudCount > 0 || c != ' ')
					{
						BaudBuffer[BaudCount++] = c;

							/* Stop before the buffer can overflow. */

						if(BaudCount == sizeof(BaudBuffer) - 1)
							GotIt = TRUE;
					}
				}

					/* Did we get the baud rate we were looking for? */

				if(GotIt)
				{
						/* We made a connection. */

					FlowInfo.Connect = TRUE;
					FlowInfo.Changed = TRUE;

						/* We are no longer looking for a baud rate. */

					BaudPending = FALSE;

						/* Strip trailing spaces. */

					while(BaudCount > 0 && BaudBuffer[BaudCount - 1] == ' ')
						BaudCount--;

					BaudBuffer[BaudCount] = 0;

						/* Turn the modem response string into a number
						 * and store it as the connection speed.
						 */

					DTERate = GetBaudRate(BaudBuffer);

						/* If no output should show up, stop
						 * the data from flowing until it gets
						 * enabled again. This will usually happen
						 * after the dial panel has closed and
						 * reopened the display.
						 */

					if(ConsoleQuiet)
					{
							/* Keep the current data. */

						DataHold = Data;

							/* If there is any data left,
							 * remember how much there is.
							 * If nothing is left, don't
							 * bother. The cache read routines
							 * will check if DataHold is not NULL
							 * and if DataSize is greater than
							 * zero before accessing the cache.
							 */

						if(Size > 0)
							DataSize = Size - 1;
						else
							DataSize = 0;

						UpdateSerialJob();

						OriginalSize -= DataSize;
					}
				}
			}
			else
			{
				BOOL MatchMade;
				LONG i;

				do
				{
					MatchMade = FALSE;

						/* Scan all ID strings for matches. */

					for(i = ScanStart ; i <= ScanEnd ; i++)
					{
							/* This sequence is a likely
							 * match.
							 */

						if(AttentionCount[i] == FlowCount)
						{
								/* Does the character
								 * fit into the sequence?
								 */

							if(c == AttentionBuffers[i][FlowCount] & Mask)
							{
								MatchMade = TRUE;

									/* Did we hit the
									 * last character
									 * in the sequence?
									 */

								if(++AttentionCount[i] == AttentionLength[i])
								{
										/* We've got a valid
										 * sequence, now look
										 * which flags to change.
										 */

									switch(i)
									{
											/* We got a `no carrier' message. */

										case SCAN_NOCARRIER:

											if(!FlowInfo.NoCarrier)
											{
												FlowInfo.NoCarrier	= TRUE;
												FlowInfo.Changed	= TRUE;
											}

											break;

											/* Got another call. */

										case SCAN_RING:

											if(!FlowInfo.Ring)
											{
												FlowInfo.Ring		= TRUE;
												FlowInfo.Changed	= TRUE;
											}

											break;

											/* Got a voice call. */

										case SCAN_VOICE:

											if(!FlowInfo.Voice)
											{
												FlowInfo.Voice		= TRUE;
												FlowInfo.Changed	= TRUE;
											}

											break;

											/* Modem reported an error. */

										case SCAN_ERROR:

											if(!FlowInfo.Error)
											{
												FlowInfo.Error		= TRUE;
												FlowInfo.Changed	= TRUE;
											}

											break;

											/* Got a connect message. */

										case SCAN_CONNECT:

											if(!Online)
											{
												BaudBuffer[0]	= 0;

												BaudPending	= TRUE;
												BaudCount	= 0;
											}

											break;

											/* Line is busy. */

										case SCAN_BUSY:

											if(!FlowInfo.Busy)
											{
												FlowInfo.Busy		= TRUE;
												FlowInfo.Changed	= TRUE;
											}

											break;

										case SCAN_NODIALTONE:

											if(!FlowInfo.NoDialTone)
											{
												FlowInfo.NoDialTone	= TRUE;
												FlowInfo.Changed	= TRUE;
											}

											break;

											/* Modem accepted a command. */

										case SCAN_OK:

											if(!FlowInfo.Ok)
											{
												FlowInfo.Ok			= TRUE;
												FlowInfo.Changed	= TRUE;
											}

											break;

										default:

											if(!FlowInfo.Signature)
											{
												FlowInfo.Signature	= i;
												FlowInfo.Changed	= TRUE;
											}

											break;
									}
								}
							}
						}
					}

					if(MatchMade)
						FlowCount++;
					else
					{
						if(FlowCount)
						{
							FlowCount = 0;

							memset(AttentionCount,0,sizeof(AttentionCount));
						}
						else
							break;
					}
				}
				while(!FlowCount);
			}
		}
	}
	while(--Size);

	return(OriginalSize);
}

	/* ResetDataFlowFilter():
	 *
	 *	Set up the data flow parser. The parser scans the serial
	 *	output data for more or less interesting modem output
	 *	(carrier lost, connect, etc.).
	 */

VOID
ResetDataFlowFilter()
{
	LONG i;

	for(i = 0 ; i < SCAN_COUNT ; i++)
		AttentionBuffers[i][0] = 0;

		/* Set up `NO CARRIER' message. */

	if(Config->ModemConfig->NoCarrier[0])
		LimitedSPrintf(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_NOCARRIER],"%s\r",Config->ModemConfig->NoCarrier);

		/* Set up `CONNECT' message. */

	LimitedStrcpy(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_CONNECT],Config->ModemConfig->Connect);

		/* Set up `VOICE' message. */

	if(Config->ModemConfig->Voice[0])
		LimitedSPrintf(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_VOICE],"%s\r",Config->ModemConfig->Voice);

		/* Set up `RING' message. */

	if(Config->ModemConfig->Ring[0])
		LimitedSPrintf(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_RING],"%s\r",Config->ModemConfig->Ring);

		/* Set up `BUSY' message. */

	if(Config->ModemConfig->Busy[0])
		LimitedSPrintf(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_BUSY],"%s\r",Config->ModemConfig->Busy);

		/* Set up `NO DIALTONE' message. */

	if(Config->ModemConfig->NoDialTone[0])
		LimitedSPrintf(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_NODIALTONE],"%s\r",Config->ModemConfig->NoDialTone);

		/* Set up `OK' message. */

	if(Config->ModemConfig->Ok[0])
		LimitedSPrintf(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_OK],"%s\r",Config->ModemConfig->Ok);

		/* Set up `ERROR' message. */

	if(Config->ModemConfig->Error[0])
		LimitedSPrintf(ATTENTION_BUFFER_SIZE,AttentionBuffers[SCAN_ERROR],"%s\r",Config->ModemConfig->Error);

		/* Reset match counter. */

	FlowCount = 0;

		/* Reset indices. */

	memset(AttentionCount,0,sizeof(AttentionCount));

		/* Determine lengths. */

	for(i = 0 ; i < SCAN_COUNT ; i++)
		AttentionLength[i] = strlen(AttentionBuffers[i]);

	for(i = SCAN_SIGDEFAULTUPLOAD ; i <= SCAN_SIGBINARYDOWNLOAD ; i++)
	{
		if(AttentionLength[i] = Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD + i - SCAN_SIGDEFAULTUPLOAD].Length)
			CopyMem(Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD + i - SCAN_SIGDEFAULTUPLOAD].Signature,AttentionBuffers[i],Config->TransferConfig->Signatures[TRANSFERSIG_DEFAULTUPLOAD + i - SCAN_SIGDEFAULTUPLOAD].Length);
	}

	if(Config->TransferConfig->ASCIIUploadType != XFER_XPR && Config->TransferConfig->ASCIIUploadType != XFER_EXTERNALPROGRAM)
		AttentionLength[SCAN_SIGASCIIUPLOAD] = 0;

	if(Config->TransferConfig->ASCIIDownloadType != XFER_XPR && Config->TransferConfig->ASCIIUploadType != XFER_EXTERNALPROGRAM)
		AttentionLength[SCAN_SIGASCIIDOWNLOAD] = 0;

	if(Config->TransferConfig->TextUploadType != XFER_XPR && Config->TransferConfig->TextUploadType != XFER_EXTERNALPROGRAM)
		AttentionLength[SCAN_SIGTEXTUPLOAD] = 0;

	if(Config->TransferConfig->TextDownloadType != XFER_XPR && Config->TransferConfig->TextUploadType != XFER_EXTERNALPROGRAM)
		AttentionLength[SCAN_SIGTEXTDOWNLOAD] = 0;

	if(Config->TransferConfig->BinaryUploadType != XFER_XPR && Config->TransferConfig->BinaryUploadType != XFER_EXTERNALPROGRAM)
		AttentionLength[SCAN_SIGBINARYUPLOAD] = 0;

	if(Config->TransferConfig->BinaryDownloadType != XFER_XPR && Config->TransferConfig->BinaryUploadType != XFER_EXTERNALPROGRAM)
		AttentionLength[SCAN_SIGBINARYDOWNLOAD] = 0;

		/* No, we are not yet looking for a baud rate. */

	BaudPending = FALSE;

		/* Reset the flags. */

	memset(&FlowInfo,FALSE,sizeof(struct FlowInfo));

		/* Full data check is slower than looking for
		 * just a single sequence (such as the `CONNECT'
		 * below). This mode is reserved for the dial panel.
		 */

	ScanStart	= 0;
	ScanEnd		= SCAN_COUNT - 1;
	UseFlow		= TRUE;
}
