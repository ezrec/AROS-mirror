/*
**	SendText.c
**
**	Text sending support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* MatchPrompt():
	 *
	 *	Search incoming data stream for a match.
	 */

BOOL
MatchPrompt(STRPTR Data,LONG Size,STRPTR Prompt,LONG PromptLen)
{
	STATIC LONG WaitCount,PromptCount;

	if(!Data)
		WaitCount = PromptCount = 0;
	else
	{
		UBYTE c,Mask;

		if(Config->SerialConfig->StripBit8)
			Mask = 0x7F;
		else
			Mask = 0xFF;

		do
		{
			if(c = ((*Data++) & Mask))
			{
				BOOL MatchMade;

				do
				{
					MatchMade = FALSE;

					if(PromptCount == WaitCount)
					{
						if(c == Prompt[WaitCount] & Mask)
						{
							MatchMade = TRUE;

							if(PromptLen == ++PromptCount)
								return(TRUE);
						}
					}

					if(MatchMade)
						WaitCount++;
					else
					{
						if(WaitCount)
						{
							WaitCount = 0;

							PromptCount = 0;
						}
						else
							break;
					}
				}
				while(!WaitCount);
			}
		}
		while(--Size);
	}

	return(FALSE);
}

	/* LocalWaitForPrompt(STRPTR Prompt,LONG PromptLen):
	 *
	 *	Scan the incoming data flow for a certain string.
	 */

STATIC BOOL
LocalWaitForPrompt(STRPTR Prompt,LONG PromptLen)
{
	ULONG Signals;
	BOOL GotIt;

	MatchPrompt(NULL,0,NULL,0);

		/* Start the timer. */

	StartTime(Config->ClipConfig->SendTimeout / 100,(Config->ClipConfig->SendTimeout % 100) * 10000);

		/* Lock the current xOFF state and clear the xOFF flag. */

	Lock_xOFF();
	Clear_xOFF();

	GotIt = FALSE;

		/* Loop until the prompt is found or the timer elapses. */

	do
	{
		Signals = (*SerialWaitForData)(SIG_TIMER);

		if(Signals & SIG_SERIAL)
		{
			ULONG Length;

				/* Check how much data is available. */

			if(Length = (*SerialGetWaiting)())
			{
					/* Don't read more than the buffer will hold. */

				if(Length > SerialBufferSize / 2)
					Length = SerialBufferSize / 2;

					/* Read the data. */

				if(Length = (*SerialRead)(ReadBuffer,Length))
				{
						/* Got some more data. */

					BytesIn += Length;

						/* Translate the buffer contents if necessary. */

					if(Translate_CR_LF)
						Length = (*Translate_CR_LF)(ReadBuffer,Length);
					else
						Length = 1;

					if(Length)
					{
						ConProcess(ReadBuffer,Length);

							/* Check if this is it. */

						if(MatchPrompt(ReadBuffer,Length,Prompt,PromptLen))
							GotIt = TRUE;
					}
				}
			}
		}

			/* Stop when the bell rings. */

		if(Signals & SIG_TIMER)
			break;
	}
	while(!GotIt);

		/* Stop the timer. */

	StopTime();

	Unlock_xOFF();

	return(GotIt);
}

	/* SendLinePrompt(STRPTR Line,LONG Len):
	 *
	 *	Send text line, wait for prompt.
	 */

BOOL
SendLinePrompt(STRPTR Line,LONG Len)
{
	LONG i;

	if(Len == -1)
		Len = strlen(Line);

	while(Len)
	{
		i = 0;

		while(i < Len && Line[i] != '\r')
			i++;

		if(Line[i] == '\r')
		{
			i++;

			SerWrite(Line,i);

			if(!LocalWaitForPrompt(SendPrompt,SendPromptLen))
				return(FALSE);
		}
		else
		{
			if(i)
				SerWrite(Line,i);
		}

		Len		-= i;
		Line	+= i;
	}

	return(TRUE);
}

	/* SendLineSimple(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, no fancy features.
	 */

BOOL
SendLineSimple(STRPTR Line,LONG Len)
{
	if(Len == -1)
		Len = strlen(Line);

	SerWrite(Line,Len);

	return(TRUE);
}

	/* SendLineDial(STRPTR Line,LONG Len):
	 *
	 *	The SendLineSimple for the dialer and init commands.
	 */

BOOL
SendLineDial(STRPTR Line,LONG Len)
{
	if(Len == -1)
		Len = strlen(Line);

	if(Config->ModemConfig->CharSendDelay > 0)
	{
		ULONG	Seconds	= Config->ModemConfig->CharSendDelay / MILLION,
				Micros	= Config->ModemConfig->CharSendDelay % MILLION;

		while(Len--)
		{
			SerWrite(Line++,1);

			DelayTime(Seconds,Micros);
		}
	}
	else
		SerWrite(Line,Len);

	return(TRUE);
}

	/* SendLineDelay(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, include a delay where necessary.
	 */

BOOL
SendLineDelay(STRPTR Line,LONG Len)
{
	if(Len == -1)
		Len = strlen(Line);

	while(Len--)
	{
		SerWrite(Line,1);

		if(*Line == '\r')
			DelayTime(Config->ClipConfig->LineDelay / 100,(Config->ClipConfig->LineDelay % 100) * 10000);
		else
			DelayTime(Config->ClipConfig->CharDelay / 100,(Config->ClipConfig->CharDelay % 100) * 10000);

		Line++;
	}

	return(TRUE);
}

	/* SendLineEcho(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, wait for single characters to be echoed.
	 */

BOOL
SendLineEcho(STRPTR Line,LONG Len)
{
	ULONG Signals;
	BOOL GotIt;

	if(Len == -1)
		Len = strlen(Line);

		/* Lock the current xOFF state and clear the xOFF flag. */

	Lock_xOFF();
	Clear_xOFF();

	while(Len--)
	{
			/* Send the next character. */

		SerWrite(Line,1);

			/* Start the timer. */

		StartTime(Config->ClipConfig->SendTimeout / 100,(Config->ClipConfig->SendTimeout % 100) * 10000);

			/* Loop until we got the character we wanted or the time elapses. */

		GotIt = FALSE;

		do
		{
			Signals = (*SerialWaitForData)(SIG_TIMER);

				/* Did we receive new data? */

			if(Signals & SIG_SERIAL)
			{
					/* Is there any character waiting? */

				if((*SerialGetWaiting)())
				{
						/* Read that character. */

					if((*SerialRead)(ReadBuffer,1))
					{
						ULONG Length;

						BytesIn++;

							/* Check if the character needs translation. */

						if(Translate_CR_LF)
							Length = (*Translate_CR_LF)(ReadBuffer,1);
						else
							Length = 1;

						if(Length)
							ConProcess(ReadBuffer,Length);

							/* Is that it? */

						if(Length == 1 && ReadBuffer[0] == Line[0])
							GotIt = TRUE;
					}
				}
			}

				/* Did the bell ring? */

			if(Signals & SIG_TIMER)
				break;
		}
		while(!GotIt);

			/* Stop the timer. */

		StopTime();

			/* Check if we got the echo. If not, return an error. */

		if(!GotIt)
		{
			Unlock_xOFF();
			return(FALSE);
		}

		Line++;
	}

	Unlock_xOFF();

	return(TRUE);
}

	/* SendLineAnyEcho(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, wait for characters to be echoed.
	 */

BOOL
SendLineAnyEcho(STRPTR Line,LONG Len)
{
	ULONG Signals;
	BOOL GotIt;

	if(Len == -1)
		Len = strlen(Line);

		/* Lock the current xOFF state and clear the xOFF flag. */

	Lock_xOFF();
	Clear_xOFF();

	while(Len--)
	{
			/* Send the next character. */

		SerWrite(Line,1);

			/* Start the timer. */

		StartTime(Config->ClipConfig->SendTimeout / 100,(Config->ClipConfig->SendTimeout % 100) * 10000);

			/* Loop until we got the character we wanted or the time elapses. */

		GotIt = FALSE;

		do
		{
			Signals = (*SerialWaitForData)(SIG_TIMER);

				/* Did we receive new data? */

			if(Signals & SIG_SERIAL)
			{
					/* Is there any character waiting? */

				if((*SerialGetWaiting)())
				{
						/* Read that character. */

					if((*SerialRead)(ReadBuffer,1))
					{
						LONG Length;

						BytesIn++;

						if(Translate_CR_LF)
							Length = (*Translate_CR_LF)(ReadBuffer,1);
						else
							Length = 1;

						if(Length)
							ConProcess(ReadBuffer,Length);

						GotIt = TRUE;
					}
				}
			}

				/* Did the bell ring? */

			if(Signals & SIG_TIMER)
				break;
		}
		while(!GotIt);

			/* Stop the timer. */

		StopTime();

			/* Check if we got the echo. If not, return an error. */

		if(!GotIt)
		{
			Unlock_xOFF();
			return(FALSE);
		}

		Line++;
	}

	Unlock_xOFF();

	return(TRUE);
}

	/* SendLineKeyDelay(STRPTR Line,LONG Len):
	 *
	 *	Send a text line, include keyboard delay pauses between characters.
	 */

BOOL
SendLineKeyDelay(STRPTR Line,LONG Len)
{
	struct Preferences Prefs;

	if(Len == -1)
		Len = strlen(Line);

		/* Get current key repeat delay. */

	GetPrefs(&Prefs,offsetof(struct Preferences,KeyRptDelay));

		/* Any delay specified at all? */

	if(Prefs.KeyRptSpeed.tv_secs || Prefs.KeyRptSpeed.tv_micro)
	{
		while(Len--)
		{
			SerWrite(Line++,1);

			if(Len)
				DelayTime(Prefs.KeyRptSpeed.tv_secs,Prefs.KeyRptSpeed.tv_micro);
		}
	}
	else
		SerWrite(Line,Len);

	return(TRUE);
}

	/* ChangeSendLine(SENDLINE NewSendLine):
	 *
	 *	Change the routine that sends full lines of text.
	 */

SENDLINE
ChangeSendLine(SENDLINE NewSendLine)
{
	SENDLINE OldSendLine;

	OldSendLine	= SendLine;
	SendLine	= NewSendLine;

	return(OldSendLine);
}

	/* RestoreSendLine():
	 *
	 *	Restore the SendLine pointer to its original routine.
	 */

VOID
RestoreSendLine(SENDLINE ChangedSendLine,SENDLINE OriginalSendLine)
{
	if(SendLine == ChangedSendLine)
		SendLine = OriginalSendLine;
}

	/* SendSetup():
	 *
	 *	Choose the right routine for the text line output job.
	 */

VOID
SendSetup()
{
		/* Prepare the prompt string. */

	if(Config->ClipConfig->LinePrompt[0])
		SendPromptLen = TranslateString(Config->ClipConfig->LinePrompt,SendPrompt);
	else
	{
		SendPrompt[0] = 0;
		SendPromptLen = 0;
	}

		/* Pick the line send routine. */

	switch(Config->ClipConfig->PacingMode)
	{
		case PACE_DIRECT:

			SendLine = (SENDLINE)SendLineSimple;
			break;

		case PACE_ECHO:

			if(Config->ClipConfig->SendTimeout)
				SendLine = (SENDLINE)SendLineEcho;
			else
				SendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_ANYECHO:

			if(Config->ClipConfig->SendTimeout)
				SendLine = (SENDLINE)SendLineAnyEcho;
			else
				SendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_PROMPT:

			if(Config->ClipConfig->SendTimeout)
				SendLine = (SENDLINE)SendLinePrompt;
			else
				SendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_DELAY:

			if(Config->ClipConfig->LineDelay || Config->ClipConfig->CharDelay)
				SendLine = (SENDLINE)SendLineDelay;
			else
				SendLine = (SENDLINE)SendLineSimple;

			break;

		case PACE_KEYBOARD:

			SendLine = (SENDLINE)SendLineKeyDelay;
			break;
	}
}
