/*
**	Sound.c
**
**	Sound support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Double buffering chunk size. */

#define BUFFER_SIZE 32768

	/* Maximum replay volume. */

#define MAX_VOLUME 64

	/* Stereo/channel sample type. */
#if 0
/* This should be in datatypes/soundclass.h */
typedef LONG SampleType;
#endif
	/* Channel definitions. */

#define SAMPLE_LEFT		2
#define SAMPLE_RIGHT	4
#define SAMPLE_STEREO	6

	/* A fixed-point value, 16 bits to the left of
	 * the point and 16 to the right. A Fixed is a
	 * number of 2**16ths, i.e. 65536ths.
	 */

typedef LONG Fixed;

	/* Unity = Fixed 1.0 = maximum volume */

#undef Unity
#define Unity 0x10000L

	/* Double-buffering information. */

struct BufferInfo
{
	LONG	Size;
	UBYTE	Buffer[BUFFER_SIZE];
};

	/* Local data. */

STATIC struct SignalSemaphore	 SoundSemaphore;
STATIC struct SoundInfo			*SoundSlot[SOUND_COUNT];
STATIC BOOL						 SoundInitialized;

	/* DeltaUnpack(UBYTE *Src,ULONG Size,BYTE *Dst):
	 *
	 *	Unpack Fibonacci-delta-encoded data.
	 */

STATIC VOID
DeltaUnpack(UBYTE *Src,ULONG Size,BYTE *Dst)
{
	STATIC BYTE CodeToDelta[16] = { -34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21 };

	BYTE	Value = (BYTE)Src[1];
	UBYTE	Code;

		/* Skip the header information. */

	Src		+= 2;
	Size	-= 2;

		/* Run down the chunk... */

	while(Size--)
	{
		Code = *Src++;

			/* Add the top nibble delta. */

		Value += CodeToDelta[Code >> 4];

		*Dst++ = Value;

			/* Add the bottom nibble delta. */

		Value += CodeToDelta[Code & 0xF];

		*Dst++ = Value;
	}
}

	/* FreeSound(struct SoundInfo *SoundInfo):
	 *
	 *	Free sound handle and associated data.
	 */

VOID
FreeSound(struct SoundInfo *SoundInfo)
{
	if(SoundInfo->SoundObject)
		DisposeDTObject(SoundInfo->SoundObject);
	else
		FreeVecPooled(SoundInfo->SoundData);

	FreeVecPooled(SoundInfo);
}

	/* LoadSound(STRPTR Name):
	 *
	 *	Load a sound file from disk.
	 */

struct SoundInfo *
LoadSound(STRPTR Name,BOOL Warn)
{
	struct SoundInfo	*SoundInfo = NULL;
	struct IFFHandle	*Handle;

		/* Allocate IFF handle for reading. */

	if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
	{
		LONG SoundStops[3 * 2] =
		{
			ID_8SVX,ID_VHDR,
			ID_8SVX,ID_CHAN,
			ID_8SVX,ID_BODY
		};

			/* Mark the chunks to stop at. */

		if(!StopChunks(Handle,SoundStops,3))
		{
			struct ContextNode *Chunk;
			struct VoiceHeader Header;
			SampleType Channel;
			BOOL SingleChannel;
			UBYTE Compression = CMP_NONE;

			Channel			= SAMPLE_STEREO;
			SingleChannel	= TRUE;

				/* Clear the voice header. */

			memset(&Header,0,sizeof(Header));

				/* Scan for data... */

			while(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				Chunk = CurrentChunk(Handle);

					/* What did we find? */

				switch(Chunk->cn_ID)
				{
						/* Looks like the basic voice header. */

					case ID_VHDR:

							/* Read the header. */

						if(ReadChunkRecords(Handle,&Header,MIN(Chunk->cn_Size,sizeof(Header)),1) == 1)
						{
								/* Compression type supported? */

							if(Header.vh_Compression == CMP_NONE || Header.vh_Compression == CMP_FIBDELTA)
							{
									/* Allocate the sound handle. */

								if(SoundInfo = (struct SoundInfo *)AllocVecPooled(sizeof(struct SoundInfo),MEMF_ANY | MEMF_CLEAR))
								{
										/* Install the rate, volume and length. */

									SoundInfo->Rate		= SysBase->ex_EClockFrequency * 5 / Header.vh_SamplesPerSec;
									SoundInfo->Length	= Header.vh_OneShotHiSamples ? Header.vh_OneShotHiSamples : (Header.vh_RepeatHiSamples ? Header.vh_RepeatHiSamples : Header.vh_SamplesPerHiCycle);
									SoundInfo->Volume	= (SoundConfig.Volume * ((Header.vh_Volume * MAX_VOLUME) / Unity)) / 100;

										/* Remember compression mode. */

									Compression		= Header.vh_Compression;
								}
							}
						}

						break;

						/* Looks like sound channel information. */

					case ID_CHAN:

							/* Do we have a handle to manage it? */

						if(SoundInfo)
						{
								/* Read the channel information. */

							if(ReadChunkRecords(Handle,&Channel,MIN(Chunk->cn_Size,sizeof(SampleType)),1) == 1)
							{
									/* Stereo sound file? */

								if(Channel == SAMPLE_STEREO)
									SingleChannel = FALSE;
							}
							else
							{
								FreeVecPooled(SoundInfo);

								SoundInfo = NULL;
							}
						}

						break;

						/* Looks like the sound body data. */

					case ID_BODY:

							/* Do we have sound handle to manage it? */

						if(SoundInfo)
						{
							BOOL Success = FALSE;

							if(!SoundInfo->Length)
								SoundInfo->Length = Chunk->cn_Size;

								/* Uncompressed raw data? */

							if(Compression == CMP_NONE)
							{
								ULONG Wanted;

								if(Channel == SAMPLE_STEREO && !SingleChannel)
								{
									Wanted = SoundInfo->Length * 2;

									if(Wanted > Chunk->cn_Size)
									{
										SoundInfo->Length /= 2;

										Wanted = SoundInfo->Length * 2;
									}
								}
								else
									Wanted = SoundInfo->Length;

								if(Chunk->cn_Size >= Wanted)
								{
										/* Allocate a buffer. */

									if(SoundInfo->SoundData = AllocVecPooled(Wanted,MEMF_ANY))
									{
											/* Read the data. */

										if(ReadChunkRecords(Handle,SoundInfo->SoundData,Wanted,1) == 1)
											Success = TRUE;
										else
											FreeVecPooled(SoundInfo->SoundData);
									}
								}
							}
							else
							{
								ULONG Wanted;

								if(Channel == SAMPLE_STEREO && !SingleChannel)
								{
									Wanted = SoundInfo->Length + 4;

									if(Wanted > Chunk->cn_Size)
									{
										SoundInfo->Length /= 2;

										Wanted = SoundInfo->Length * 2 + 4;

										SingleChannel = TRUE;
									}
								}
								else
									Wanted = SoundInfo->Length / 2 + 2;

								if(Chunk->cn_Size >= Wanted)
								{
									UBYTE *TempBuffer;

										/* Allocate a temporary decompression buffer. */

									if(TempBuffer = (UBYTE *)AllocVecPooled(Chunk->cn_Size,MEMF_ANY))
									{
											/* Read the compressed data. */

										if(ReadChunkRecords(Handle,TempBuffer,Chunk->cn_Size,1) == 1)
										{
											ULONG Length = SoundInfo->Length;

												/* Allocate space for the uncompressed data. */

											if(SoundInfo->SoundData = AllocVecPooled(Length * 2,MEMF_ANY))
											{
													/* Stereo sound file? */

												if(!SingleChannel && Channel == SAMPLE_STEREO)
												{
													UBYTE *Data = SoundInfo->SoundData;

														/* Unpack the stereo sound. */

													DeltaUnpack(TempBuffer,                 Length / 2 + 2,Data);
													DeltaUnpack(TempBuffer + Length / 2 + 2,Length / 2 + 2,Data + Length);
												}
												else
												{
														/* Unpack the mono sound. */

													DeltaUnpack(TempBuffer,Length / 2 + 2,SoundInfo->SoundData);
												}

												Success = TRUE;
											}
										}

										FreeVecPooled(TempBuffer);
									}
								}
							}

							if(!Success)
							{
								FreeVecPooled(SoundInfo);

								SoundInfo = NULL;
							}
						}

						break;
				}
			}

				/* Did we get what we wanted? */

			if(SoundInfo)
			{
					/* Any sound data allocated? */

				if(SoundInfo->SoundData)
				{
					UBYTE *Data = SoundInfo->SoundData;

						/* Which kind of sound file did
						 * we read?
						 */

					switch(Channel)
					{
							/* Left channel only. */

						case SAMPLE_LEFT:

							SoundInfo->LeftData = Data;
							break;

							/* Right channel only. */

						case SAMPLE_RIGHT:

							SoundInfo->RightData = Data;
							break;

							/* Two stereo channels. */

						case SAMPLE_STEREO:

								/* One sound mapped to two voices. */

							if(SingleChannel)
								SoundInfo->LeftData = SoundInfo->RightData = Data;
							else
							{
									/* Split the voice data. */

								SoundInfo->LeftData		= Data;
								SoundInfo->RightData	= Data + SoundInfo->Length;
							}

							break;
					}
				}
				else
				{
					FreeVecPooled(SoundInfo);

					SoundInfo = NULL;
				}
			}
		}

		CloseIFFStream(Handle);
	}

		/* Successful? */

	if(SoundInfo)
		strcpy(SoundInfo->Name,Name);
	else
	{
		if(DataTypesBase)
		{
			Object *Sound;

			if(Sound = NewDTObject(Name,
				DTA_SourceType,	DTST_FILE,
				DTA_GroupID,	GID_SOUND,
				SDTA_Volume,	(SoundConfig.Volume * MAX_VOLUME) / 100,
				SDTA_Cycles,	1,
			TAG_DONE))
			{
				struct VoiceHeader *Header;
				ULONG SampleLength;

				SampleLength	= 0;
				Header			= NULL;

				if(GetDTAttrs(Sound,
					SDTA_SampleLength,	&SampleLength,
					SDTA_VoiceHeader,	&Header,
				TAG_DONE) == 2)
				{
					if(SampleLength && Header)
					{
						ULONG Secs,Micro,Period;

						Period	= Header->vh_SamplesPerSec;

						Secs	= SampleLength / Period;
						Micro	= (1000000 / Period) * (SampleLength % Period);

						if(SoundInfo = (struct SoundInfo *)AllocVecPooled(sizeof(struct SoundInfo),MEMF_ANY | MEMF_CLEAR))
						{
							strcpy(SoundInfo->Name,Name);

							SoundInfo->SoundObject			= Sound;
							SoundInfo->SoundTime.tv_secs	= Secs;
							SoundInfo->SoundTime.tv_micro	= Micro;

							return(SoundInfo);
						}
					}
				}

				DisposeDTObject(Sound);
			}
		}

		if(Warn)
			ShowRequest(Window,LocaleString(MSG_TERMSOUND_COULD_NOT_LOAD_SOUND_FILE_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Name);
	}

	return(SoundInfo);
}

	/* ReplaySound():
	 *
	 *	Replay sound with given information (doubly-buffered).
	 */

STATIC VOID
ReplaySound(struct SoundInfo *SoundInfo,struct IOAudio *SoundControlRequest,struct IOAudio *SoundRequestLeft,struct IOAudio *SoundRequestRight,struct BufferInfo *BufferInfo)
{
	UBYTE	*Left	= SoundInfo->LeftData,
			*Right	= SoundInfo->RightData;
	ULONG	 Size	= SoundInfo->Length,Length;
	LONG	 Base	= 0;

	Length = MIN(BUFFER_SIZE,Size);

	Size -= Length;

		/* Left channel available? */

	if(!(((IPTR)SoundRequestLeft->ioa_Request.io_Unit) & (LEFT0F | LEFT1F)))
		Left = NULL;

		/* Right channel available? */

	if(!(((IPTR)SoundRequestRight->ioa_Request.io_Unit) & (RIGHT0F | RIGHT1F)))
		Right = NULL;

		/* Fill up left buffer. */

	if(Left)
	{
		CopyMem(Left,BufferInfo[Base].Buffer,Length);

		BufferInfo[Base].Size = Length;

		Left += Length;
	}

		/* Fill up right buffer. */

	if(Right)
	{
		CopyMem(Right,BufferInfo[Base + 2].Buffer,Length);

		BufferInfo[Base + 2].Size = Length;

		Right += Length;
	}

		/* Process sound data. */

	do
	{
			/* Block both channels. */

		SoundControlRequest->ioa_Request.io_Command = CMD_STOP;

		SendIO((struct IORequest *)SoundControlRequest);
		WaitIO((struct IORequest *)SoundControlRequest);

			/* Any data for the left channel? */

		if(Left)
		{
			SoundRequestLeft->ioa_Request.io_Command	= CMD_WRITE;
			SoundRequestLeft->ioa_Request.io_Flags		= ADIOF_PERVOL;
			SoundRequestLeft->ioa_Period				= SoundInfo->Rate;
			SoundRequestLeft->ioa_Volume				= SoundInfo->Volume;
			SoundRequestLeft->ioa_Cycles				= 1;
			SoundRequestLeft->ioa_Data					= BufferInfo[Base].Buffer;
			SoundRequestLeft->ioa_Length				= BufferInfo[Base].Size;

				/* Get the left channel going. */

			BeginIO((struct IORequest *)SoundRequestLeft);
		}

			/* Any data for the right channel? */

		if(Right)
		{
			SoundRequestRight->ioa_Request.io_Command	= CMD_WRITE;
			SoundRequestRight->ioa_Request.io_Flags		= ADIOF_PERVOL;
			SoundRequestRight->ioa_Period				= SoundInfo->Rate;
			SoundRequestRight->ioa_Volume				= SoundInfo->Volume;
			SoundRequestRight->ioa_Cycles				= 1;
			SoundRequestRight->ioa_Data					= BufferInfo[Base + 2].Buffer;
			SoundRequestRight->ioa_Length				= BufferInfo[Base + 2].Size;

				/* Get the right channel going. */

			BeginIO((struct IORequest *)SoundRequestRight);
		}

			/* Start up both channels synchronously... */

		SoundControlRequest->ioa_Request.io_Command = CMD_START;

		SendIO((struct IORequest *)SoundControlRequest);
		WaitIO((struct IORequest *)SoundControlRequest);

			/* Grab the other buffers. */

		Base ^= 1;

			/* Still any data left? */

		if(Size)
		{
				/* Cut off the next slice. */

			Length = MIN(BUFFER_SIZE,Size);

			Size -= Length;

				/* Left channel available? */

			if(Left)
			{
				CopyMem(Left,BufferInfo[Base].Buffer,Length);

				BufferInfo[Base].Size = Length;

				Left += Length;
			}

				/* Right channel available? */

			if(Right)
			{
				CopyMem(Right,BufferInfo[Base + 2].Buffer,Length);

				BufferInfo[Base + 2].Size = Length;

				Right += Length;
			}

				/* Last slice eaten? */

			if(!Size)
			{
					/* Wait for sounds to terminate. */

				if(Left)
					WaitIO((struct IORequest *)SoundRequestLeft);

				if(Right)
					WaitIO((struct IORequest *)SoundRequestRight);

					/* Block both channels. */

				SoundControlRequest->ioa_Request.io_Command = CMD_STOP;

				SendIO((struct IORequest *)SoundControlRequest);
				WaitIO((struct IORequest *)SoundControlRequest);

					/* Any data for the left channel? */

				if(Left)
				{
					SoundRequestLeft->ioa_Request.io_Command	= CMD_WRITE;
					SoundRequestLeft->ioa_Request.io_Flags		= ADIOF_PERVOL;
					SoundRequestLeft->ioa_Period				= SoundInfo->Rate;
					SoundRequestLeft->ioa_Volume				= SoundInfo->Volume;
					SoundRequestLeft->ioa_Cycles				= 1;
					SoundRequestLeft->ioa_Data					= BufferInfo[Base].Buffer;
					SoundRequestLeft->ioa_Length				= BufferInfo[Base].Size;

						/* Get the left channel going. */

					BeginIO((struct IORequest *)SoundRequestLeft);
				}

					/* Any data for the right channel? */

				if(Right)
				{
					SoundRequestRight->ioa_Request.io_Command	= CMD_WRITE;
					SoundRequestRight->ioa_Request.io_Flags		= ADIOF_PERVOL;
					SoundRequestRight->ioa_Period				= SoundInfo->Rate;
					SoundRequestRight->ioa_Volume				= SoundInfo->Volume;
					SoundRequestRight->ioa_Cycles				= 1;
					SoundRequestRight->ioa_Data					= BufferInfo[Base + 2].Buffer;
					SoundRequestRight->ioa_Length				= BufferInfo[Base + 2].Size;

						/* Get the right channel going. */

					BeginIO((struct IORequest *)SoundRequestRight);
				}

					/* Start up both channels synchronously... */

				SoundControlRequest->ioa_Request.io_Command = CMD_START;

				SendIO((struct IORequest *)SoundControlRequest);
				WaitIO((struct IORequest *)SoundControlRequest);
			}
		}

			/* Wait for sounds to terminate. */

		if(Left)
			WaitIO((struct IORequest *)SoundRequestLeft);

		if(Right)
			WaitIO((struct IORequest *)SoundRequestRight);
	}
	while(Size);
}

	/* PlaySound(struct SoundInfo *SoundInfo):
	 *
	 *	Replay a sound.
	 */

VOID
PlaySound(struct SoundInfo *SoundInfo)
{
	if(SoundInfo->SoundObject)
	{
		struct MsgPort *SoundTimePort;

		if(SoundTimePort = CreateMsgPort())
		{
			struct timerequest *SoundTimeRequest;

			if(SoundTimeRequest = (struct timerequest *)CreateIORequest(SoundTimePort,sizeof(struct timerequest)))
			{
				if(!OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)SoundTimeRequest,0))
				{
					SoundTimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
					SoundTimeRequest->tr_time				= SoundInfo->SoundTime;

					SetSignal(0,PORTMASK(SoundTimePort));

					SendIO((struct IORequest *)SoundTimeRequest);

					DoMethod(SoundInfo->SoundObject,DTM_TRIGGER,NULL,STM_PLAY,NULL);

					WaitIO((struct IORequest *)SoundTimeRequest);

					CloseDevice((struct IORequest *)SoundTimeRequest);
				}

				DeleteIORequest(SoundTimeRequest);
			}

			DeleteMsgPort(SoundTimePort);
		}
	}
	else
	{
		struct BufferInfo *BufferInfo;

			/* Allocate sound buffers. */

		if(BufferInfo = (struct BufferInfo *)AllocVec(4 * sizeof(struct BufferInfo),MEMF_CHIP))
		{
			struct MsgPort *SoundPort;

				/* Allocate an io replyport. */

			if(SoundPort = CreateMsgPort())
			{
				struct IOAudio *SoundControlRequest;

					/* Allocate the big sound control request. */

				if(SoundControlRequest = (struct IOAudio *)CreateIORequest(SoundPort,sizeof(struct IOAudio)))
				{
					struct IOAudio *SoundRequestLeft;

						/* Allocate the left channel sound request. */

					if(SoundRequestLeft = (struct IOAudio *)CreateIORequest(SoundPort,sizeof(struct IOAudio)))
					{
						struct IOAudio *SoundRequestRight;

							/* Allocate the right channel sound request. */

						if(SoundRequestRight = (struct IOAudio *)CreateIORequest(SoundPort,sizeof(struct IOAudio)))
						{
							STATIC UBYTE TwoChannels[] =
							{
								LEFT0F | RIGHT0F,
								LEFT0F | RIGHT1F,
								LEFT1F | RIGHT0F,
								LEFT1F | RIGHT1F
							};

							STATIC UBYTE LeftChannel[] =
							{
								LEFT0F,
								LEFT1F
							};

							STATIC UBYTE RightChannel[] =
							{
								RIGHT0F,
								RIGHT1F
							};

							UBYTE *AllocationMap;
							LONG AllocationSize;

								/* Determine the correct channel
								 * allocation map.
								 */

							if(SoundInfo->LeftData)
							{
								if(SoundInfo->RightData)
								{
									AllocationMap	= TwoChannels;
									AllocationSize	= sizeof(TwoChannels);
								}
								else
								{
									AllocationMap	= LeftChannel;
									AllocationSize	= sizeof(LeftChannel);
								}
							}
							else
							{
								AllocationMap	= RightChannel;
								AllocationSize	= sizeof(RightChannel);
							}

								/* Set up for sound channel allocation. */

							SoundControlRequest->ioa_Request.io_Message.mn_Node.ln_Pri	= 127;
							SoundControlRequest->ioa_Request.io_Command					= ADCMD_ALLOCATE;
							SoundControlRequest->ioa_Request.io_Flags					= ADIOF_NOWAIT | ADIOF_PERVOL;
							SoundControlRequest->ioa_Data								= AllocationMap;
							SoundControlRequest->ioa_Length								= AllocationSize;

								/* Open audio.device, allocating the sound
								 * channels on the fly.
								 */

							if(!OpenDevice(AUDIONAME,0,(struct IORequest *)SoundControlRequest,0))
							{
									/* Clone the sound control request. */

								CopyMem(SoundControlRequest,SoundRequestLeft,	sizeof(struct IOAudio));
								CopyMem(SoundControlRequest,SoundRequestRight,	sizeof(struct IOAudio));

									/* Separate the channels. */

								SoundRequestLeft ->ioa_Request.io_Unit = (struct Unit *)((IPTR)SoundRequestLeft ->ioa_Request.io_Unit & (LEFT0F  | LEFT1F));
								SoundRequestRight->ioa_Request.io_Unit = (struct Unit *)((IPTR)SoundRequestRight->ioa_Request.io_Unit & (RIGHT0F | RIGHT1F));

									/* Replay the sound... */

								ReplaySound(SoundInfo,SoundControlRequest,SoundRequestLeft,SoundRequestRight,BufferInfo);

									/* Do the big cleanup. */

								CloseDevice((struct IORequest *)SoundControlRequest);
							}

							DeleteIORequest(SoundRequestRight);
						}

						DeleteIORequest(SoundRequestLeft);
					}

					DeleteIORequest(SoundControlRequest);
				}

				DeleteMsgPort(SoundPort);
			}

			FreeVec(BufferInfo);
		}
	}
}

	/* SoundLoad(LONG Sound,BOOL Warn):
	 *
	 *	Loads or frees a sound slot.
	 */

STATIC BOOL
SoundLoad(LONG Sound,BOOL Warn)
{
	struct SoundInfo **Info;
	BOOL Success;
	STRPTR Name = "";

	Info	= &SoundSlot[Sound];
	Success	= FALSE;

		/* Which sound file name are we to pick? */

	switch(Sound)
	{
		case SOUND_BELL:

			Name = SoundConfig.BellFile;
			break;

		case SOUND_CONNECT:

			Name = SoundConfig.ConnectFile;
			break;

		case SOUND_DISCONNECT:

			Name = SoundConfig.DisconnectFile;
			break;

		case SOUND_GOODTRANSFER:

			Name = SoundConfig.GoodTransferFile;
			break;

		case SOUND_BADTRANSFER:

			Name = SoundConfig.BadTransferFile;
			break;

		case SOUND_RING:

			Name = SoundConfig.RingFile;
			break;

		case SOUND_VOICE:

			Name = SoundConfig.VoiceFile;
			break;

		case SOUND_ERROR:

			Name = SoundConfig.ErrorNotifyFile;
			break;
	}

		/* Is a file name available? */

	if(Name[0])
	{
			/* Sound slot already initialized? */

		if(*Info)
		{
				/* Did the file name change? */

			if(Stricmp((*Info)->Name,Name))
			{
					/* Free the sound slot. */

				FreeSound(*Info);

					/* Try to load the sound slot. */

				if(*Info = LoadSound(Name,Warn))
				{
						/* Remember the file name. */

					strcpy((*Info)->Name,Name);

					Success = TRUE;
				}
			}
			else
				Success = TRUE;
		}
		else
		{
				/* Try to load the sound slot. */

			if(*Info = LoadSound(Name,Warn))
			{
					/* Remember the file name. */

				strcpy((*Info)->Name,Name);

				Success = TRUE;
			}
		}
	}
	else
	{
			/* Does the slot still contain any sound? */

		if(*Info)
		{
				/* Free the slot. */

			FreeSound(*Info);

			*Info = NULL;
		}

		Success = TRUE;
	}

	return(Success);
}

	/* SoundServer(VOID):
	 *
	 *	Replay a sound.
	 */

STATIC VOID SAVE_DS STACKARGS
SoundServer(LONG Index)
{
		/* Get access to the sound slots. */

	SafeObtainSemaphoreShared(&SoundSemaphore);

		/* Replay the sound. */

	PlaySound(SoundSlot[Index]);

	Forbid();

		/* Release access to the sound slots. */

	ReleaseSemaphore(&SoundSemaphore);
}

	/* SoundExit():
	 *
	 *	Free allocated sound resources.
	 */

VOID
SoundExit()
{
	LONG i;

	if(SoundInitialized)
		ObtainSemaphore(&SoundSemaphore);

		/* Free the slots. */

	for(i = 0 ; i < SOUND_COUNT ; i++)
	{
		if(SoundSlot[i])
		{
			FreeSound(SoundSlot[i]);

			SoundSlot[i] = NULL;
		}
	}

	if(SoundInitialized)
		ReleaseSemaphore(&SoundSemaphore);
}

	/* SoundInit():
	 *
	 *	Allocate resources required for sound replaying.
	 */

VOID
SoundInit()
{
		/* Sound access semaphore available? */

	if(!SoundInitialized)
	{
		InitSemaphore(&SoundSemaphore);
		SoundInitialized = TRUE;
	}

		/* Preload sounds if necessary. */

	if(SoundConfig.Preload)
	{
		LONG i;

		for(i = 0 ; i < SOUND_COUNT ; i++)
			SoundLoad(i,TRUE);
	}
}

	/* SoundPlay(LONG Sound):
	 *
	 *	Play a certain sound slot.
	 */

VOID
SoundPlay(LONG Sound)
{
	if(SoundConfig.Volume > 0)
	{
		BOOL DoBeep = FALSE;

			/* Sound access semaphore available? */

		if(SoundInitialized)
		{
				/* Check to see if we are currently playing a
				 * sound.
				 */

			if(AttemptSemaphore(&SoundSemaphore))
			{
				ReleaseSemaphore(&SoundSemaphore);

					/* Release any other sound if necessary. */

				if(!SoundConfig.Preload)
				{
					LONG i;

					for(i = 0 ; i < SOUND_COUNT ; i++)
					{
						if(i != Sound && SoundSlot[i])
						{
							FreeSound(SoundSlot[i]);

							SoundSlot[i] = NULL;
						}
					}
				}

					/* Load the sound slot. */

				SoundLoad(Sound,FALSE);

					/* Slot filled? */

				if(!SoundSlot[Sound])
					DoBeep = TRUE;
				else
				{
						/* Fire off the sound server task. */

					if(!LocalCreateTask("term Sound Task",ThisProcess->pr_Task.tc_Node.ln_Pri + 10,(TASKENTRY)SoundServer,4000,1,Sound))
						DoBeep = TRUE;
				}
			}
		}

			/* Check if the default sound should be played. */

		if(DoBeep)
			Beep();
	}
}
