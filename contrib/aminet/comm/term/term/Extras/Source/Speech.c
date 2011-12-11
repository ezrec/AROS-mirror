/*
**	Speech.c
**
**	Speech support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Local symbols. */

#ifndef __AROS__
STATIC UBYTE SpeechString[512];
STATIC UBYTE TranslatedString[512];
#endif
STATIC struct narrator_rb *NarratorRequest;
STATIC struct MsgPort *NarratorPort;
STATIC BOOL DidSpeak;

	/* DeleteSpeech():
	 *
	 *	Free the data associated with the speech function.
	 */

VOID
DeleteSpeech()
{
	if(DidSpeak)
	{
		if(!CheckIO((struct IORequest *)NarratorRequest))
			WaitIO((struct IORequest *)NarratorRequest);
	}

#ifndef __AROS__
	if(NarratorRequest)
	{
		if(NarratorRequest->message.io_Device)
			CloseDevice((struct IORequest *)NarratorRequest);

		DeleteIORequest((struct IORequest *)NarratorRequest);
		NarratorRequest = NULL;
	}
#endif

	if(NarratorPort)
	{
		DeleteMsgPort(NarratorPort);
		NarratorPort = NULL;
	}

	if(TranslatorBase)
	{
		CloseLibrary(TranslatorBase);
		TranslatorBase = NULL;
	}

	DidSpeak = FALSE;
}

	/* CreateSpeech():
	 *
	 *	Open translator.library and narrator.device, perform
	 *	the necessary setups for the speech function.
	 */

BOOL
CreateSpeech()
{
#ifndef __AROS__
	if(TranslatorBase = OpenLibrary("translator.library",0))
	{
		if(NarratorPort = CreateMsgPort())
		{
			if(NarratorRequest = (struct narrator_rb *)CreateIORequest(NarratorPort,sizeof(struct narrator_rb)))
			{
				STATIC UBYTE AnyChannel[] =
				{
					LEFT0F,
					LEFT1F,
					RIGHT0F,
					RIGHT1F
				};

					/* Any channel will do. */

				NarratorRequest->ch_masks			= AnyChannel;
				NarratorRequest->nm_masks			= sizeof(AnyChannel);

					/* This is a write request. */

				NarratorRequest->message.io_Command	= CMD_WRITE;
				NarratorRequest->message.io_Data	= (APTR)SpeechString;

				if(!OpenDevice("narrator.device",0,(struct IORequest *)NarratorRequest,0))
				{
					SpeechSetup();

					return(TRUE);
				}
			}
		}
	}

	DeleteSpeech();
#endif
	return(FALSE);
}

	/* Say(STRPTR String,...):
	 *
	 *	Translate a string into phonemes and speak it.
	 */

VOID
Say(CONST_STRPTR String,...)
{
#ifndef __AROS__
	if(SpeechConfig.Enabled && English)
	{
		if(!TranslatorBase)
			CreateSpeech();

		if(TranslatorBase)
		{
			va_list VarArgs;

			if(DidSpeak)
			{
				if(!CheckIO((struct IORequest *)NarratorRequest))
					WaitIO((struct IORequest *)NarratorRequest);
			}

			va_start(VarArgs,String);
			LimitedVSPrintf(sizeof(TranslatedString),TranslatedString,String,VarArgs);
			va_end(VarArgs);

			if(!Translate(TranslatedString,strlen(TranslatedString),SpeechString,511))
			{
				NarratorRequest->message.io_Length = strlen(SpeechString);

				ClrSignal(PORTMASK(NarratorRequest->message.io_Message.mn_ReplyPort));

				SendIO((struct IORequest *)NarratorRequest);

				DidSpeak = TRUE;
			}
			else
				DidSpeak = FALSE;
		}
	}
#endif
}

	/* SpeechSetup():
	 *
	 *	Transfer the configuration data into the setup.
	 */

VOID
SpeechSetup()
{
#ifndef __AROS__
	if(NarratorRequest)
	{
		NarratorRequest->rate		= SpeechConfig.Rate;
		NarratorRequest->pitch		= SpeechConfig.Pitch;
		NarratorRequest->sex		= SpeechConfig.Sex;
		NarratorRequest->volume		= SpeechConfig.Volume;
		NarratorRequest->sampfreq	= SpeechConfig.Frequency;
	}
#endif
}
