/*
**	Beep.c
**
**	Simple beep sound support routine
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Beep():
	 *
	 *	Produce a simple decent beep sound.
	 */

VOID
Beep()
{
	if(SoundConfig.Volume)
	{
		struct MsgPort *AudioPort;

			/* Create the IO reply port. */

		if(AudioPort = (struct MsgPort *)CreateMsgPort())
		{
			struct IOAudio *AudioRequest;

				/* Create the audio IO info. */

			if(AudioRequest = (struct IOAudio *)CreateIORequest(AudioPort,sizeof(struct IOAudio)))
			{
				STATIC UBYTE AnyChannel[] =
				{
					LEFT0F,LEFT1F,	RIGHT0F,RIGHT1F
				};

				AudioRequest->ioa_Request.io_Command				= ADCMD_ALLOCATE;
				AudioRequest->ioa_Request.io_Flags					= ADIOF_NOWAIT | ADIOF_PERVOL;
				AudioRequest->ioa_Request.io_Message.mn_Node.ln_Pri	= 127;
				AudioRequest->ioa_Data								= AnyChannel;
				AudioRequest->ioa_Length							= sizeof(AnyChannel);

					/* Open audio.device */

				if(!OpenDevice(AUDIONAME,0,(struct IORequest *)AudioRequest,0))
				{
						/* Minimal sine wave. */

					STATIC BYTE SineWave[8] =
					{
						 90,
						 127,
						 90,
						 0,
						-90,
						-127,
						-90,
						 0
					};

					BYTE *Wave;

					if(Wave = AllocMem(sizeof(SineWave),MEMF_CHIP))
					{
						CopyMem(SineWave,Wave,sizeof(SineWave));

							/* Prepare to play a beep sound. */

						AudioRequest->ioa_Request.io_Command	= CMD_WRITE;
						AudioRequest->ioa_Request.io_Flags		= ADIOF_PERVOL;
						AudioRequest->ioa_Period				= SysBase->ex_EClockFrequency / 3200;
						AudioRequest->ioa_Volume				= (SoundConfig.Volume * (64 / 2)) / 100;
						AudioRequest->ioa_Cycles				= 150;
						AudioRequest->ioa_Data					= Wave;
						AudioRequest->ioa_Length				= sizeof(SineWave);

							/* Play it. */

						BeginIO((struct IORequest *)AudioRequest);
						WaitIO((struct IORequest *)AudioRequest);

						FreeMem(Wave,sizeof(SineWave));
					}

						/* Close audio.device */

					CloseDevice((struct IORequest *)AudioRequest);
				}

					/* Free the audio request. */

				DeleteIORequest((struct IORequest *)AudioRequest);
			}

				/* Free the IO reply port. */

			DeleteMsgPort(AudioPort);
		}
	}
}

	/* BellSignal():
	 *
	 *	Your whole bell signal routine (visible + audible).
	 */

VOID
BellSignal()
{
		/* Handle the visual part. */

	if(Config->TerminalConfig->BellMode != BELL_AUDIBLE)
	{
		if(StatusProcess)
			Signal((struct Task *)StatusProcess,SIG_BELL);
	}

		/* Let it beep. */

	if(Config->TerminalConfig->BellMode == BELL_AUDIBLE || Config->TerminalConfig->BellMode == BELL_BOTH)
		SoundPlay(SOUND_BELL);
}
