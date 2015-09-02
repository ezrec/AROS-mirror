/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "config.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

#include "alMain.h"
#include "alu.h"
#include "threads.h"
#include "compat.h"

#include "backends/base.h"

#include <devices/ahi.h>
#include <proto/exec.h>

static const ALCchar ahi_device[] = "AHI Default";

static const char *ahi_driver = "ahi.device";

typedef struct ALCplaybackAHI {
	DERIVE_FROM_TYPE(ALCbackend);

	int ahi_fmt;

	struct MsgPort *ahip;
	struct AHIRequest *ahir[2];
	struct AHIRequest *link;

	ALubyte *mix_data[2];
	int data_size;

	volatile int killNow;
	althrd_t thread;
} ALCplaybackAHI;

static int ALCplaybackAHI_mixerProc(void *ptr);

static void ALCplaybackAHI_Construct(ALCplaybackAHI *self, ALCdevice *device);
static DECLARE_FORWARD(ALCplaybackAHI, ALCbackend, void, Destruct)
static ALCenum ALCplaybackAHI_open(ALCplaybackAHI *self, const ALCchar *name);
static void ALCplaybackAHI_close(ALCplaybackAHI *self);
static ALCboolean ALCplaybackAHI_reset(ALCplaybackAHI *self);
static ALCboolean ALCplaybackAHI_start(ALCplaybackAHI *self);
static void ALCplaybackAHI_stop(ALCplaybackAHI *self);
static DECLARE_FORWARD2(ALCplaybackAHI, ALCbackend, ALCenum, captureSamples, ALCvoid*, ALCuint)
static DECLARE_FORWARD(ALCplaybackAHI, ALCbackend, ALCuint, availableSamples)
static DECLARE_FORWARD(ALCplaybackAHI, ALCbackend, ALint64, getLatency)
static DECLARE_FORWARD(ALCplaybackAHI, ALCbackend, void, lock)
static DECLARE_FORWARD(ALCplaybackAHI, ALCbackend, void, unlock)
DECLARE_DEFAULT_ALLOCATORS(ALCplaybackAHI)
DEFINE_ALCBACKEND_VTABLE(ALCplaybackAHI);

static int ALCplaybackAHI_mixerProc(void *ptr) {
	ALCplaybackAHI *self = (ALCplaybackAHI*)ptr;
	ALCdevice *device = STATIC_CAST(ALCbackend, self)->mDevice;
	struct MsgPort *ahip = self->ahip;
	ALint frameSize;
	ALint cb = 0;

    SetRTPriority();
    althrd_setname(althrd_current(), MIXER_THREAD_NAME);

	ahip->mp_SigTask = FindTask(NULL);
	ahip->mp_SigBit = AllocSignal(-1);
	ahip->mp_Flags = PA_SIGNAL;

	frameSize = FrameSizeFromDevFmt(device->FmtChans, device->FmtType);

	while(!self->killNow && device->Connected) {
		ALint len = self->data_size;
		ALubyte *WritePtr = self->mix_data[cb];
		struct AHIRequest *ahir = self->ahir[cb];

		aluMixData(device, WritePtr, len/frameSize);

		ahir->ahir_Std.io_Message.mn_Node.ln_Pri = 127;
		ahir->ahir_Std.io_Command = CMD_WRITE;
		ahir->ahir_Std.io_Data    = WritePtr;
		ahir->ahir_Std.io_Length  = len;
		ahir->ahir_Frequency = device->Frequency;
		ahir->ahir_Type      = self->ahi_fmt;
		ahir->ahir_Volume    = 0x10000;
		ahir->ahir_Position  = 0x8000;
		ahir->ahir_Link      = self->link;
	   	SendIO((struct IORequest*)ahir);

		if(self->link != NULL) {
			WaitIO((struct IORequest*)self->link);
		}

		self->link = ahir;
		cb ^= 1;
	}

	if(self->link != NULL) {
		WaitIO((struct IORequest*)self->link);
	}

	ahip->mp_Flags = PA_IGNORE;
	FreeSignal(ahip->mp_SigBit);
	ahip->mp_SigTask = NULL;
	ahip->mp_SigBit = -1;

	return 0;
}

static void ALCplaybackAHI_Construct(ALCplaybackAHI *self, ALCdevice *device) {
	ALCbackend_Construct(STATIC_CAST(ALCbackend, self), device);
	SET_VTABLE2(ALCplaybackAHI, ALCbackend, self);
}

static int get_ahi_format(ALCdevice *device) {
	switch(ChannelsFromDevFmt(device->FmtChans)) {
	case 1:
		switch(device->FmtType) {
		case DevFmtUByte:
			device->FmtType = DevFmtByte;
		case DevFmtByte:
			return AHIST_M8S;
		case DevFmtUShort:
			device->FmtType = DevFmtShort;
		case DevFmtShort:
			return AHIST_M16S;
		case DevFmtUInt:
		case DevFmtFloat:
			device->FmtType = DevFmtInt;
		case DevFmtInt:
			return AHIST_M32S;
		default:
			break;
		}
		break;
	case 2:
		switch(device->FmtType) {
		case DevFmtUByte:
			device->FmtType = DevFmtByte;
		case DevFmtByte:
			return AHIST_S8S;
		case DevFmtUShort:
			device->FmtType = DevFmtShort;
		case DevFmtShort:
			return AHIST_S16S;
		case DevFmtUInt:
		case DevFmtFloat:
			device->FmtType = DevFmtInt;
		case DevFmtInt:
			return AHIST_S32S;
		default:
			break;
		}
		break;
	default:
		break;
	}
	ERR("Unknown format?! chans: %d type: %d\n", device->FmtChans, device->FmtType);
	return -1;
}

static ALCenum ALCplaybackAHI_open(ALCplaybackAHI *self, const ALCchar *name) {
	ALCdevice *device = STATIC_CAST(ALCbackend, self)->mDevice;

	if(!name)
		name = ahi_device;
	else if(strcmp(name, ahi_device) != 0)
		return ALC_INVALID_VALUE;

	self->killNow = 0;

	self->ahi_fmt = get_ahi_format(device);
	if(self->ahi_fmt == -1)
		return ALC_INVALID_VALUE;

	self->ahip = CreateMsgPort();

	self->ahir[0] = (struct AHIRequest*)CreateIORequest(self->ahip, sizeof(struct AHIRequest));
	if(self->ahir[0] == NULL) {
		return ALC_OUT_OF_MEMORY;
	}

	self->ahir[0]->ahir_Version = 4;

	if(OpenDevice("ahi.device", AHI_DEFAULT_UNIT, (struct IORequest *)self->ahir[0], 0)
		!= 0)
	{
		return ALC_OUT_OF_MEMORY;
	}

	self->ahir[1] = (struct AHIRequest*)CreateIORequest(self->ahip, sizeof(struct AHIRequest));
	if(self->ahir[1] == NULL) {
		return ALC_OUT_OF_MEMORY;
	}

        CopyMem(self->ahir[0], self->ahir[1], sizeof(struct AHIRequest));

	al_string_copy_cstr(&device->DeviceName, name);

	return ALC_NO_ERROR;
}

static void ALCplaybackAHI_close(ALCplaybackAHI *self) {
	CloseDevice((struct IORequest*)self->ahir[0]);
	DeleteIORequest(self->ahir[1]);
	DeleteIORequest(self->ahir[0]);
	self->ahir[0] = self->ahir[1] = NULL;

	DeleteMsgPort(self->ahip);
	self->ahip = NULL;

	self->ahi_fmt = -1;
}

static ALCboolean ALCplaybackAHI_reset(ALCplaybackAHI *self) {
	ALCdevice *device = STATIC_CAST(ALCbackend, self)->mDevice;

	self->ahi_fmt = get_ahi_format(device);
	if(self->ahi_fmt == -1)
		return ALC_FALSE;

	SetDefaultChannelOrder(device);

	return ALC_TRUE;
}

static ALCboolean ALCplaybackAHI_start(ALCplaybackAHI *self) {
	ALCdevice *device = STATIC_CAST(ALCbackend, self)->mDevice;

	self->data_size = device->UpdateSize * FrameSizeFromDevFmt(device->FmtChans, device->FmtType);

	self->mix_data[0] = AllocVec(self->data_size, MEMF_ANY);
	self->mix_data[1] = AllocVec(self->data_size, MEMF_ANY);

	if(self->mix_data[0] != NULL && self->mix_data[1] != NULL) {
		self->killNow = 0;
		if(althrd_create(&self->thread, ALCplaybackAHI_mixerProc, self) == althrd_success)
			return ALC_TRUE;
	}

	FreeVec(self->mix_data[1]);
	FreeVec(self->mix_data[0]);
	self->mix_data[0] = self->mix_data[1] = NULL;

	return ALC_FALSE;
}

static void ALCplaybackAHI_stop(ALCplaybackAHI *self) {
	int res;

	if(self->killNow)
		return;

	self->killNow = 1;
	althrd_join(self->thread, &res);

	FreeVec(self->mix_data[1]);
	FreeVec(self->mix_data[0]);
	self->mix_data[0] = self->mix_data[1] = NULL;
}

typedef struct ALCahiBackendFactory {
	DERIVE_FROM_TYPE(ALCbackendFactory);
} ALCahiBackendFactory;
#define ALCAHIBACKENDFACTORY_INITIALIZER { { GET_VTABLE2(ALCahiBackendFactory, ALCbackendFactory) } }

ALCbackendFactory *ALCahiBackendFactory_getFactory(void);

static ALCboolean ALCahiBackendFactory_init(ALCahiBackendFactory *self);
static DECLARE_FORWARD(ALCahiBackendFactory, ALCbackendFactory, void, deinit)
static ALCboolean ALCahiBackendFactory_querySupport(ALCahiBackendFactory *self, ALCbackend_Type type);
static void ALCahiBackendFactory_probe(ALCahiBackendFactory *self, enum DevProbe type);
static ALCbackend* ALCahiBackendFactory_createBackend(ALCahiBackendFactory *self, ALCdevice *device, ALCbackend_Type type);
DEFINE_ALCBACKENDFACTORY_VTABLE(ALCahiBackendFactory);

ALCbackendFactory *ALCahiBackendFactory_getFactory(void) {
	static ALCahiBackendFactory factory = ALCAHIBACKENDFACTORY_INITIALIZER;
	return STATIC_CAST(ALCbackendFactory, &factory);
}

ALCboolean ALCahiBackendFactory_init(ALCahiBackendFactory* UNUSED(self)) {
	ConfigValueStr("ahi", "device", &ahi_driver);

	return ALC_TRUE;
}

ALCboolean ALCahiBackendFactory_querySupport(ALCahiBackendFactory* UNUSED(self), ALCbackend_Type type) {
	if(type == ALCbackend_Playback)
		return ALC_TRUE;
	return ALC_FALSE;
}

void ALCahiBackendFactory_probe(ALCahiBackendFactory* UNUSED(self), enum DevProbe type) {
	switch(type) {
		case ALL_DEVICE_PROBE:
			AppendAllDevicesList(ahi_device);
			break;
		case CAPTURE_DEVICE_PROBE:
			break;
	}
}

ALCbackend* ALCahiBackendFactory_createBackend(ALCahiBackendFactory* UNUSED(self), ALCdevice *device, ALCbackend_Type type) {
	if(type == ALCbackend_Playback) {
		ALCplaybackAHI *backend;

		backend = ALCplaybackAHI_New(sizeof(*backend));
		if(!backend) return NULL;
		memset(backend, 0, sizeof(*backend));

		ALCplaybackAHI_Construct(backend, device);

		return STATIC_CAST(ALCbackend, backend);
    }

    return NULL;
}

