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
#include "AL/al.h"
#include "AL/alc.h"

#include <devices/ahi.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>

static char *ahi_device;

typedef struct {
	struct MsgPort *mp;
	struct AHIRequest *io, *io2;
	APTR buf, buf2;
	unsigned int bsize;
	unsigned int ahifmt;
	int nch;
	int bps;
	int freq;
	struct Process *proc;
    struct Task * parent;
} ahi_data;

static int AHIProc(void)
{
	ALCdevice *pDevice = (ALCdevice*)(FindTask(NULL))->tc_UserData;
	ahi_data *data = (ahi_data*)pDevice->ExtraData;
	struct AHIRequest *io, *io2, *join;
	APTR buf, buf2, tmp;

	data->mp->mp_SigTask = FindTask(NULL);
	data->mp->mp_SigBit = AllocSignal(-1);
	data->mp->mp_Flags = PA_SIGNAL;
	io = data->io;
	io2 = data->io2;
	join = NULL;
	buf = data->buf;
	buf2 = data->buf2;

	while (!(SetSignal(0,0) & SIGF_CHILD))
	{
		SuspendContext(NULL);
		aluMixData(pDevice->Context, buf, data->bsize, pDevice->Format);
		ProcessContext(NULL);

		if (data->bps == 1) {
			int i = data->bsize;
			unsigned char *ptr = (unsigned char*)buf;
			while (i--) *ptr++ ^= 0x80;
		}

		io->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
		io->ahir_Std.io_Command = CMD_WRITE;
		io->ahir_Std.io_Data = buf;
		io->ahir_Std.io_Length = data->bsize;
		io->ahir_Std.io_Offset = 0;
		io->ahir_Frequency = data->freq;
		io->ahir_Type = data->ahifmt;
		io->ahir_Volume = 0x10000;
		io->ahir_Position = 0x8000;
		io->ahir_Link = join;
        SendIO((struct IORequest*)io);

		if (join) {
			WaitIO((struct IORequest*)io);
		}
		join = io;
		io = io2; io2 = join;
 		tmp = buf;
		buf = buf2; buf2 = tmp;
	}
	
	if (join) {
		WaitIO((struct IORequest*)io);
	}
	
	data->mp->mp_Flags = PA_IGNORE;
	FreeSignal(data->mp->mp_SigBit);
	data->mp->mp_SigTask = NULL;
	data->mp->mp_SigBit = -1;
	
    /* Simulate NP_NotifyOnDeath */
    Signal(data->parent, SIGF_CHILD);
	return 0;
}

static ALCboolean ahi_open_playback(ALCdevice *device, const ALCchar *deviceName)
{
	ahi_data *data;
	
	if(deviceName)
	{
		if(strcmp(deviceName, ahi_device) != 0)
			return ALC_FALSE;
		device->szDeviceName = ahi_device;
	} else
		device->szDeviceName = ahi_device;
	
	data = (ahi_data*)calloc(1, sizeof(ahi_data));
	device->ExtraData = data;
	if (data == NULL) {
		return ALC_FALSE;
	}
	
	data->mp = CreateMsgPort();
	data->io = (struct AHIRequest*)CreateIORequest(data->mp, sizeof(struct AHIRequest));
	data->io2 = (struct AHIRequest*)CreateIORequest(data->mp, sizeof(struct AHIRequest));
	if (data->io == NULL || data->io2 == NULL) {
		return ALC_FALSE;
	}
	
	data->io->ahir_Version = 4;
	if (OpenDevice("ahi.device", 0, (struct IORequest*)data->io, 0)) {
		data->io->ahir_Std.io_Device = NULL;
		AL_PRINT("Could not open ahi.device\n");
		return ALC_FALSE;
	}
	CopyMem(data->io, data->io2, sizeof(struct AHIRequest));
	
	data->nch = aluChannelsFromFormat(device->Format);
	data->bps = aluBytesFromFormat(device->Format);
	if (data->nch == 1) {
		switch (data->bps) {
			case 1:
				data->ahifmt = AHIST_M8S;
				break;
			case 2:
				data->ahifmt = AHIST_M16S;
				break;
			case 4:
				data->ahifmt = AHIST_M32S;
				break;
			default:
				AL_PRINT("Unknown format?! %x\n", device->Format);
				return ALC_FALSE;
		}
	} else if (data->nch == 2) {
		switch (data->bps) {
			case 1:
				data->ahifmt = AHIST_S8S;
				break;
			case 2:
				data->ahifmt = AHIST_S16S;
				break;
			case 4:
				data->ahifmt = AHIST_S32S;
				break;
			default:
				AL_PRINT("Unknown format?! %x\n", device->Format);
				return ALC_FALSE;
		}
	} else {
		AL_PRINT("Unknown format?! %x\n", device->Format);
		return ALC_FALSE;
	}
	
	data->freq = device->Frequency;
	data->bsize = (data->freq / 10) * data->nch * data->bps;
	data->buf = AllocVec(data->bsize, MEMF_PUBLIC);
	data->buf2 = AllocVec(data->bsize, MEMF_PUBLIC);
	if (data->buf == NULL || data->buf2 == NULL) {
		return ALC_FALSE;
	}
	
    data->parent = FindTask(NULL);
	data->proc = CreateNewProcTags(
		NP_UserData,				device,
		NP_Entry,					AHIProc,
//		NP_Child,					TRUE,
		NP_Name,					"AHIProc (OpenAL)",
		NP_Priority,				10,
//		NP_NotifyOnDeathSigTask,	NULL,
		TAG_END);
	if (data->proc == NULL) {
		return ALC_FALSE;
	}
	
	return ALC_TRUE;
}

static void ahi_close_playback(ALCdevice *device)
{
	ahi_data *data = (ahi_data*)device->ExtraData;
	if (data) {
		if (data->proc) {
			Signal(&data->proc->pr_Task, SIGF_CHILD);
			Wait(SIGF_CHILD);
			data->proc = NULL;
		}
		FreeVec(data->buf);
		FreeVec(data->buf2);
		data->buf = data->buf2 = NULL;
		if (data->io && data->io->ahir_Std.io_Device) {
			CloseDevice((struct IORequest*)data->io);
			data->io->ahir_Std.io_Device = NULL;
		}
		DeleteIORequest(data->io);
		DeleteIORequest(data->io2);
		data->io = data->io2 = NULL;
		DeleteMsgPort(data->mp);
		data->mp = NULL;
		free(data);
		device->ExtraData = NULL;
	}
}

static ALCboolean ahi_open_capture(ALCdevice *device, const ALCchar *deviceName, ALCuint frequency, ALCenum format, ALCsizei SampleSize)
{
	return ALC_FALSE;
}

static const BackendFuncs ahi_funcs = {
    ahi_open_playback,
    ahi_close_playback,
    ahi_open_capture,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

void alc_ahi_init(BackendFuncs *func_list)
{
	*func_list = ahi_funcs;
	
	ahi_device = AppendDeviceList("AHI Software");
	AppendAllDeviceList(ahi_device);
}
