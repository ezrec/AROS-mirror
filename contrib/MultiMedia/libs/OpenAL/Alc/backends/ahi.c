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

#include <devices/ahi.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>

#undef Lock

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

static const ALCchar AHIDevice[] = "AHI default";

#define PLAYERTASK_NAME       "OpenAL Audio Task"
#define PLAYERTASK_PRIORITY   10

typedef struct {
	struct MsgPort *mp;
	struct AHIRequest *AHIReqs[2];
	APTR PlayerBuffer[2];
	APTR tmpBuf;
	unsigned int bsize;
	unsigned int ahifmt;
	int nch;
	int bps;
	int freq;
	struct Process *proc;
	struct Task * parent;
	int signal;
	int num_frags; // Since AHI doesn't report the fragment size, emulate it
} ahi_data;

static ALuint AHIProc(ALvoid *ptr)
{
    ALCdevice *pDevice = (ALCdevice*)(FindTask(NULL))->tc_UserData;
    ahi_data *data = (ahi_data*)pDevice->ExtraData;
    struct AHIRequest *AHIReqs[2];
    APTR PlayerBuffer[2];
    ULONG AHICurBuf = 0;
    UWORD AHIOtherBuf = AHICurBuf^1;
    UBYTE *BufferPointer;
    BOOL AHIReqSent[2] = {FALSE, FALSE};

    data->mp->mp_SigTask = FindTask(NULL);
    data->mp->mp_SigBit = AllocSignal(-1);
    data->mp->mp_Flags = PA_SIGNAL;
    
    AHIReqs[0] = data->AHIReqs[0];
    AHIReqs[1] = data->AHIReqs[1];
    PlayerBuffer[0] = data->PlayerBuffer[0];
    PlayerBuffer[1] = data->PlayerBuffer[1];
    AHICurBuf=0;
    BufferPointer = PlayerBuffer[0];

    while (!(SetSignal(0,0) & SIGF_CHILD))
    {
            struct AHIRequest *req = AHIReqs[AHICurBuf];
            AHIOtherBuf = AHICurBuf^1;

//		SuspendContext(NULL);
            aluMixData(pDevice, data->tmpBuf, pDevice->UpdateSize);
//		ProcessContext(NULL);

            memcpy(BufferPointer, data->tmpBuf, pDevice->UpdateSize);

            if (AHIReqSent[AHICurBuf])
            {
                    if (req->ahir_Std.io_Data)
                    {
                            WaitIO((struct IORequest *)req);
                            req->ahir_Std.io_Data = NULL;

                            GetMsg(data->mp);
                            GetMsg(data->mp);
                    }
            }

            if (pDevice->FmtType == 1) {
                    int i = data->bsize;
                    unsigned char *ptr = (unsigned char*)PlayerBuffer[AHICurBuf];
                    while (i--) *ptr++ ^= 0x80;
            }

            req->ahir_Std.io_Message.mn_Node.ln_Pri = 127;
            req->ahir_Std.io_Command = CMD_WRITE;
            req->ahir_Std.io_Data = PlayerBuffer[AHIOtherBuf];
            req->ahir_Std.io_Length = data->bsize;
            req->ahir_Std.io_Offset = 0;
            req->ahir_Frequency = data->freq;
            req->ahir_Type = data->ahifmt;
            req->ahir_Volume = 0x10000;
            req->ahir_Position = 0x8000;
            req->ahir_Link = (AHIReqSent[AHIOtherBuf] && !CheckIO((struct IORequest *) AHIReqs[AHIOtherBuf])) ? AHIReqs[AHIOtherBuf] : NULL; //join;
    SendIO((struct IORequest*)req);

            AHIReqSent[AHICurBuf] = TRUE;
            AHICurBuf = AHIOtherBuf;

            BufferPointer = PlayerBuffer[AHICurBuf];
    }

    if (AHIReqs[AHIOtherBuf]) {
            WaitIO((struct IORequest*)AHIReqs[AHIOtherBuf]);
    }

    data->mp->mp_Flags = PA_IGNORE;
    FreeSignal(data->mp->mp_SigBit);
    data->mp->mp_SigTask = NULL;
    data->mp->mp_SigBit = -1;

    /* Simulate NP_NotifyOnDeath */
    Signal(data->parent, 1 << data->signal);

    return 0;
}

static ALCenum ahi_open_playback(ALCdevice *device, const ALCchar *deviceName)
{
	ahi_data *data;

	if(deviceName)
	{
            if(strcmp(deviceName, AHIDevice) != 0)
                    return ALC_INVALID_VALUE;
	} else
		deviceName = AHIDevice;

	data = (ahi_data*)calloc(1, sizeof(ahi_data));
	if (data == NULL) {
		return ALC_FALSE;
	}

	device->ExtraData = data;
        device->DeviceName = (ALCchar *)deviceName;

	data->mp = CreateMsgPort();
	data->AHIReqs[0] = (struct AHIRequest*)CreateIORequest(data->mp, sizeof(struct AHIRequest));
	data->AHIReqs[1] = (struct AHIRequest*)CreateIORequest(data->mp, sizeof(struct AHIRequest));
	if (data->AHIReqs[0] == NULL || data->AHIReqs[1] == NULL) {
		return ALC_INVALID_VALUE;
	}

	data->AHIReqs[0]->ahir_Version = 4;
	if (OpenDevice("ahi.device", 0, (struct IORequest*)data->AHIReqs[0], 0)) {
		data->AHIReqs[0]->ahir_Std.io_Device = NULL;
		ERR("Failed to open ahi.device\n");
		return ALC_INVALID_VALUE;
	}
	CopyMem(data->AHIReqs[0], data->AHIReqs[1], sizeof(struct AHIRequest));

	data->nch = ChannelsFromDevFmt(device->FmtChans);

	if (data->nch == 1) {
		switch (device->FmtType) {
			case DevFmtByte:
				data->ahifmt = AHIST_M8S;
				break;
			case DevFmtUShort:
				data->ahifmt = AHIST_M16S;
				break;
			case DevFmtUInt:
				data->ahifmt = AHIST_M32S;
				break;
                        case DevFmtUByte:
                        case DevFmtShort:
                        case DevFmtInt:
                        case DevFmtFloat:
			default:
				ERR("Unknown format?! %d\n", device->FmtType);
				return ALC_INVALID_VALUE;
		}
	} else if (data->nch == 2) {
		switch (device->FmtType) {
			case DevFmtByte:
				data->ahifmt = AHIST_S8S;
				break;
			case DevFmtUShort:
				data->ahifmt = AHIST_S16S;
				break;
			case DevFmtUInt:
				data->ahifmt = AHIST_S32S;
				break;
                        case DevFmtUByte:
                        case DevFmtShort:
                        case DevFmtInt:
                        case DevFmtFloat:
			default:
				ERR("Unknown format?! %d\n", device->FmtType);
				return ALC_INVALID_VALUE;
		}
	} else {
		ERR("Unknown format?! %d\n", device->FmtType);
		return ALC_INVALID_VALUE;
	}

	data->freq = device->Frequency;
	
	data->bsize = (data->freq / 10) * data->nch * device->FmtType;
	data->PlayerBuffer[0] = AllocVec(data->bsize, MEMF_PUBLIC);
	data->PlayerBuffer[1] = AllocVec(data->bsize, MEMF_PUBLIC);
	data->tmpBuf          = AllocVec(data->bsize, MEMF_PUBLIC);  
	if (data->PlayerBuffer[0] == NULL || data->PlayerBuffer[1] == NULL || data->tmpBuf == NULL) {
		return ALC_INVALID_VALUE;
	}

	data->signal = AllocSignal(-1);
	data->parent = FindTask(NULL);
	data->proc = CreateNewProcTags(
		NP_UserData,				device,
		NP_Entry,					AHIProc,
//ORIG CODE		NP_Child,					TRUE,
		NP_Name,					PLAYERTASK_NAME,
		NP_Priority,				PLAYERTASK_PRIORITY,
		NP_StackSize,				40960,
//ORIG CODE		NP_NotifyOnDeathSigTask,	IExec->FindTask(NULL),
//ORIG CODE		NP_NotifyOnDeathSignalBit,	data->signal,
		TAG_END);
	if (data->proc == NULL) {
		return ALC_INVALID_VALUE;
	}

	return ALC_NO_ERROR;
}

static void ahi_close_playback(ALCdevice *device)
{
	ahi_data *data = (ahi_data*)device->ExtraData;
	if (data) 
	{
		if (data->proc) {
			Signal(&data->proc->pr_Task, SIGF_CHILD);
			Wait(1 << data->signal);
//ORIG CODE			Delay(100); //Wait to be sure that audio task has finished its work..
			data->proc = NULL;
			FreeSignal(data->signal);
			data->signal = -1;
		}
		FreeVec(data->PlayerBuffer[0]);
		FreeVec(data->PlayerBuffer[1]);
		FreeVec(data->tmpBuf);
		data->PlayerBuffer[0] = data->PlayerBuffer[1] = data->tmpBuf = NULL;
		
		if (data->AHIReqs[0])
		{
			if (!CheckIO((struct IORequest *) data->AHIReqs[0]))
			{
				AbortIO((struct IORequest *) data->AHIReqs[0]);
				WaitIO((struct IORequest *) data->AHIReqs[0]);
			}
		}

		if(data->AHIReqs[1])
		{ // Only if the second request was started
			if (!CheckIO((struct IORequest *) data->AHIReqs[1]))
			{
    			AbortIO((struct IORequest *) data->AHIReqs[1]);
    			WaitIO((struct IORequest *) data->AHIReqs[1]);
			}
		}
		
		if (data->AHIReqs[0] && data->AHIReqs[0]->ahir_Std.io_Device) {
			CloseDevice((struct IORequest*)data->AHIReqs[0]);
			data->AHIReqs[0]->ahir_Std.io_Device = NULL;
		}
		DeleteIORequest(data->AHIReqs[0]);
		DeleteIORequest(data->AHIReqs[1]);
		data->AHIReqs[0] = data->AHIReqs[1] = NULL;
		DeleteMsgPort(data->mp);
		data->mp = NULL;
		free(data);
		device->ExtraData = NULL;
	}
}

static ALCenum ahi_open_capture(ALCdevice *Device, const ALCchar *deviceName)
{
	return ALC_INVALID_VALUE;
}

static const BackendFuncs ahi_funcs = {
    ahi_open_playback,
    ahi_close_playback,
    NULL, //ahi_reset_playback
    NULL, //ahi_start_playback
    NULL, //ahi_stop_playback
    ahi_open_capture,
    NULL, //ahi_close_capture,
    NULL, //ahi_start_capture,
    NULL, //ahi_stop_capture,
    NULL, //ahi_capture_samples,
    NULL, //ahi_available_samples,
    ALCdevice_LockDefault,
    ALCdevice_UnlockDefault,
    ALCdevice_GetLatencyDefault
};

ALCboolean alc_ahi_init(BackendFuncs *func_list)
{
    *func_list = ahi_funcs;
    return ALC_TRUE;
}

void alc_ahi_deinit(void)
{
}

void alc_ahi_probe(enum DevProbe type)
{
    switch(type)
    {
        case ALL_DEVICE_PROBE:
            AppendAllDevicesList(AHIDevice);
            break;
        case CAPTURE_DEVICE_PROBE:
            break;
    }
}

