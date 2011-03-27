/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@libsdl.org
*/

#include "SDL_endian.h"
#include "SDL_ahiaudio.h"
#include "SDL_timer.h"
#ifndef __AROS__
#include "SDL_cgxasm.h"
#endif
#include <stdlib.h>
#include "../../main/aros/mydebug.h"

extern Uint32 has_obtained_spec;

/* Audio driver functions */
static int AHI_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void AHI_ThreadInit(_THIS);
static void AHI_WaitAudio(_THIS);
static void AHI_PlayAudio(_THIS);
static Uint8 *AHI_GetAudioBuf(_THIS);
static void AHI_CloseAudio(_THIS);

/* Internal functions */

static Fixed AHI_Volume = 0x10000;

void AHI_Mute(ULONG mute)
{
	AHI_Volume = mute ? 0 : 0x10000;
}

/* Audio driver bootstrap functions */

static int Audio_Available(void)
{
	/* AHI always available */

	return 1;
}

static void Audio_DeleteDevice(SDL_AudioDevice *device)
{
	D(bug("[SDL] Audio_DeleteDevice()\n"));
	free(device->hidden);
	free(device);
}

static SDL_AudioDevice *Audio_CreateDevice(int devindex)
{
	struct SDL_PrivateAudioData *hidden;
	SDL_AudioDevice *this;

	D(bug("[SDL] Audio_CreateDevice()\n"));

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)malloc(sizeof(SDL_AudioDevice));
	hidden = (struct SDL_PrivateAudioData *)malloc((sizeof *hidden));

	if (this == NULL || hidden == NULL)
	{
		SDL_OutOfMemory();

		if (this)
			free(this);

		if (hidden)
			free(hidden);

		return(0);
	}

	memset(this, 0, (sizeof *this));
	memset(hidden, 0, (sizeof *hidden));

	NEWLIST(&hidden->dummy_mp.mp_MsgList);
	hidden->dummy_mp.mp_Node.ln_Type = NT_MSGPORT;
	hidden->dummy_mp.mp_Flags = PA_SIGNAL;
	hidden->dummy_mp.mp_SigBit = SIGB_SINGLE;
	hidden->dummy_mp.mp_SigTask = FindTask(NULL);

	this->hidden = hidden;

	/* Set the function pointers */
	this->OpenAudio = AHI_OpenAudio;
	this->ThreadInit = AHI_ThreadInit;
	this->WaitAudio = AHI_WaitAudio;
	this->PlayAudio = AHI_PlayAudio;
	this->GetAudioBuf = AHI_GetAudioBuf;
	this->CloseAudio = AHI_CloseAudio;

	this->free = Audio_DeleteDevice;

	return this;
}

AudioBootStrap AHI_bootstrap = {
	"AHI",
	"MorphOS ahi.device SDL audio",
	Audio_Available,
	Audio_CreateDevice
};


void static AHI_WaitAudio(_THIS)
{
	struct AHIRequest *req;

	D(bug("[SDL] AHI_WaitAudio()\n"));

	req = &this->hidden->audio_req[this->hidden->current_buffer];

	//if (!CheckIO((struct IORequest *)req)
	if (req->ahir_Std.io_Data)
	{
		WaitIO((struct IORequest *)req);
		req->ahir_Std.io_Data = NULL;

		GetMsg(&this->hidden->thread_audio_mp);
		GetMsg(&this->hidden->thread_audio_mp);
	}
}

static void AHI_PlayAudio(_THIS)
{
	struct SDL_PrivateAudioData *hidden = this->hidden;
	struct AHIRequest *req;
	APTR   mixbuf;
	ULONG current, current2;
	LONG mixbufsize;

	D(bug("[SDL] AHI_PlayAudio()\n"));

	current  = hidden->current_buffer;
	current2 = current ^ 1;
	req = &hidden->audio_req[current];

	/* Write the audio data out */

	mixbufsize = hidden->size; /* Note: this is an audio buffer size in _bytes_ */
	mixbuf	  = hidden->mixbuf[current];

	req->ahir_Std.io_Data    = mixbuf;
	req->ahir_Std.io_Length  = mixbufsize;
	req->ahir_Std.io_Offset  = 0;
	req->ahir_Std.io_Command = CMD_WRITE;
	req->ahir_Frequency      = hidden->freq;
	req->ahir_Volume         = AHI_Volume;	//0x10000;
	req->ahir_Type           = hidden->type;
	req->ahir_Position       = 0x8000;
	req->ahir_Link           = (hidden->playing ? &hidden->audio_req[current2] : NULL);

	switch (hidden->convert)
	{
		case AHI_CONVERT_8:
		{
			BYTE *buf = mixbuf;

			do
			{
				*buf++ = *buf - 128;
				mixbufsize--;
			}
			while (mixbufsize);
		}
		break;

		case AHI_CONVERT_16:
		{
			WORD *buf = mixbuf;

			do
			{
				*buf++ = *buf - 32768;
				mixbufsize -= 2;
			}
			while (mixbufsize > 0);
		}
		break;

		case AHI_CONVERT_S16LSB:
#ifndef __AROS__
			copy_and_swap16(mixbuf, mixbuf, mixbufsize / 2);
			break;
#endif
		case AHI_CONVERT_U16LSB:
		{
			UWORD j, *buf = mixbuf;

			j = hidden->convert == AHI_CONVERT_U16LSB ? 32768 : 0;

			do
			{
				UWORD sample = *buf;
				*buf++ = ((sample >> 8) | (sample << 8)) - j;
				mixbufsize -= 2;
			}
			while (mixbufsize > 0);
		}
		break;
	}

	SendIO((struct IORequest *)req);
	hidden->current_buffer = current2;
	hidden->playing = 1;
}

static Uint8 *AHI_GetAudioBuf(_THIS)
{
	D(bug("[SDL] AHI_GetAudioBuf()\n"));
	return(this->hidden->mixbuf[this->hidden->current_buffer]);
}

static void AHI_CloseAudio(_THIS)
{
	struct SDL_PrivateAudioData *hidden = this->hidden;

	D(bug("[SDL] AHI_CloseAudio()\n"));

	if (hidden->have_ahi)
	{
		CloseDevice((struct IORequest *)&hidden->ctrl_req);
	}

	if (hidden->mixbuf[0])
		FreeMem(hidden->mixbuf[0], hidden->size);

	if (hidden->mixbuf[1])
		FreeMem(hidden->mixbuf[1], hidden->size);

	hidden->mixbuf[0]  = NULL;
	hidden->mixbuf[1]  = NULL;
	hidden->have_ahi   = 0;
	hidden->playing    = 0;
}

static int AHI_OpenAudio(_THIS, SDL_AudioSpec *spec)
{	
	struct SDL_PrivateAudioData *hidden = this->hidden;
#ifndef __AROS__
	int altivec_align = 0;
#endif

	D(bug("[SDL] AHI_OpenAudio()\n"));

	if (spec->channels > 2)
	{
		D(bug("More than 2 channels not currently supported, forcing conversion...\n"));
		spec->channels = 2;
	}

	D(bug("requested samples is %ld\n", spec->samples));

	/* Fiddle with this to find out working combination with games
	 *
	 * Recommended testing: E-UAE, Bolcatoid
	 */

	if (has_obtained_spec && spec->samples > 1024)
	{
		spec->samples = spec->freq / 20;

		if (spec->samples < 256)
			spec->samples = 256;

		D(bug("[SDL] Changed sample count. New count is %ld.\n", spec->samples));
	}

	hidden->convert = AHI_NO_CONVERSION;

	/* Determine the audio parameters from the AudioSpec */
	switch (spec->format)
	{
		case AUDIO_U8:
			hidden->convert = AHI_CONVERT_8;
		case AUDIO_S8:
			hidden->bytespersample = 1;
			hidden->type = spec->channels == 1 ? AHIST_M8S : AHIST_S8S;
			break;
#ifndef __AROS__
		case AUDIO_S16LSB:
			altivec_align = 1;
			hidden->convert = AHI_CONVERT_S16LSB;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_U16LSB:
			hidden->convert = AHI_CONVERT_U16LSB;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_U16MSB:
			hidden->convert = AHI_CONVERT_16;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_S16MSB:
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
#else
#if AROS_BIG_ENDIAN
		case AUDIO_S16LSB:
			// SWAP
			hidden->convert = AHI_CONVERT_S16LSB;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_U16LSB:
			// SWAP + SIGN
			hidden->convert = AHI_CONVERT_U16LSB;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_U16MSB:
			// SIGN
			hidden->convert = AHI_CONVERT_16;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_S16MSB:
			// NOTHING TO BE DONE
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
#else
		case AUDIO_S16LSB:
			// NOTHING TO BE DONE
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_U16LSB:
			// SIGN
			hidden->convert = AHI_CONVERT_16;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_U16MSB:
			// SWAP + SIGN
			hidden->convert = AHI_CONVERT_U16LSB;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
		case AUDIO_S16MSB:
			// SWAP
			hidden->convert = AHI_CONVERT_S16LSB;
			hidden->bytespersample = 2;
			hidden->type = spec->channels == 1 ? AHIST_M16S : AHIST_S16S;
			break;
#endif
#endif
			
		default:
			SDL_SetError("Unsupported audio format");
			return(-1);
	}

	/* Update the fragment size as size in bytes */
	SDL_CalculateAudioSpec(spec);

	hidden->ctrl_req.ahir_Std.io_Message.mn_ReplyPort = &hidden->dummy_mp;
	hidden->ctrl_req.ahir_Std.io_Message.mn_Length = sizeof(struct AHIRequest);
	hidden->ctrl_req.ahir_Version = 4;

	if (OpenDevice(AHINAME, 0, (struct IORequest *)&hidden->ctrl_req, NULL) != 0)
	{
		D(bug("could not open ahi\n"));
		SDL_SetError("Unable to open AHI device! Error code %d.\n", hidden->ctrl_req.ahir_Std.io_Error);
		//DeleteIORequest((struct IORequest *)hidden->ctrl_req);
		return -1;
	}

	hidden->have_ahi = 1;
#ifndef __AROS__
	/* Align size to multiples of 16 bytes */
	if (altivec_align)
		spec->size = (spec->size + 15) & ~15;
#endif
	/* Set output frequency and size */
	hidden->freq = spec->freq;
	hidden->size = spec->size;

	/* Allocate mixing buffer */
#ifndef __AROS__
	if (altivec_align)
	{
		hidden->mixbuf[0] = (Uint8 *)AllocMemAligned(spec->size, MEMF_ANY, 32, 0);
		hidden->mixbuf[1] = (Uint8 *)AllocMemAligned(spec->size, MEMF_ANY, 32, 0);
	}
	else
#endif
	{
		hidden->mixbuf[0] = (Uint8 *)AllocMem(spec->size, MEMF_ANY);
		hidden->mixbuf[1] = (Uint8 *)AllocMem(spec->size, MEMF_ANY);
	}

	if ((hidden->mixbuf[0] == NULL) || (hidden->mixbuf[1] == NULL))
	{
		D(bug("could not alloc audio mixing buffers\n"));
		SDL_SetError("Unable to allocate audio mixing buffers!\n");

		if (hidden->mixbuf[0])
			FreeMem(hidden->mixbuf[0], hidden->size);

		if (hidden->mixbuf[1])
			FreeMem(hidden->mixbuf[1], hidden->size);

		CloseDevice((struct IORequest *)&hidden->ctrl_req);
		hidden->mixbuf[0] = NULL;
		hidden->mixbuf[1] = NULL;
		hidden->have_ahi  = 0;
		return -1;
	}

	NEWLIST(&hidden->thread_audio_mp.mp_MsgList);
	hidden->thread_audio_mp.mp_Node.ln_Pri	= 60;
	hidden->thread_audio_mp.mp_Node.ln_Type  = NT_MSGPORT;
	hidden->thread_audio_mp.mp_Flags 		  = PA_SIGNAL;

	hidden->current_buffer =0;
	hidden->playing = 0;

	D(bug("OpenAudio...OK\n"));

	/* We're ready to rock and roll. :-) */

	return 0;
}

static void AHI_ThreadInit(_THIS)
{
	struct SDL_PrivateAudioData *hidden = this->hidden;
	struct MsgPort 	*mp;
	struct Task 	*task;

	CopyMem(&hidden->ctrl_req, &hidden->audio_req[0], sizeof(struct AHIRequest));
	CopyMem(&hidden->ctrl_req, &hidden->audio_req[1], sizeof(struct AHIRequest));

	mp = &hidden->thread_audio_mp;

	hidden->audio_req[0].ahir_Std.io_Message.mn_ReplyPort = mp;
	hidden->audio_req[1].ahir_Std.io_Message.mn_ReplyPort = mp;

	mp->mp_SigBit  = AllocSignal(-1);
	mp->mp_SigTask = task = FindTask(NULL);

	SetTaskPri(task, 1);
}
