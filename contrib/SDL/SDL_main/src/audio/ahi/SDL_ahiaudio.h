/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002  Sam Lantinga

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
    slouken@libsdl.org
*/

#ifndef _SDL_ahiaudio_h
#define _SDL_ahiaudio_h

#include <exec/exec.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <stdlib.h>
#include <string.h>

#include <devices/ahi.h>
#include "../../main/aros/mydebug.h"

#include "SDL.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiomem.h"
#include "../SDL_sysaudio.h"

/* Hidden "this" pointer for the audio functions */
#define _THIS	SDL_AudioDevice *this

struct SDL_PrivateAudioData {
	/* The handle for the audio device */
	struct AHIRequest ctrl_req, audio_req[2];
	struct MsgPort dummy_mp, thread_audio_mp;
	Sint32 freq,type,bytespersample,size;
	Uint8 *mixbuf[2];           /* The app mixing buffer */
	Uint32 current_buffer;
	Uint8  have_ahi;
	Uint8  playing;
	Uint16 convert;
};

enum
{
	AHI_NO_CONVERSION,
	AHI_CONVERT_8,
	AHI_CONVERT_16,
	AHI_CONVERT_U16LSB,
	AHI_CONVERT_S16LSB
};

#endif /* _SDL_ahiaudio_h */
