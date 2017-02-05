/* Copyright 2010-2011 Fredrik Wikstrom. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/filehandler.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <string.h>
#include "scsicmd.h"
#include "endian.h"

static inline APTR CheckBPTR (BPTR bptr) {
	APTR cptr;
#ifndef AROS_FAST_BPTR
	if (bptr & 0xc0000000) return NULL;
#endif
	cptr = BADDR(bptr);
	if (!TypeOfMem(cptr)) return NULL;
	return cptr;
}

static inline LONG CopyStringBSTRToC (BSTR src, char *dst, ULONG dst_size) {
#ifdef __AROS__
	STRPTR ptr = AROS_BSTR_ADDR(src);
	ULONG ln = AROS_BSTR_strlen(src);
#else
	UBYTE *ptr = BADDR(src);
	ULONG ln = *ptr++;
#endif
	if (ln > (dst_size-1)) ln = dst_size-1;
	memcpy(dst, ptr, ln);
	dst[ln] = 0;
	return ln;
}

struct IOStdReq *GetCDDevice (const char *drive) {
	char drive_name[16];
	struct DosList *dl;
	struct FileSysStartupMsg *fssm = NULL;
	const ULONG flags = LDF_DEVICES|LDF_READ;
	char device_name[64];
	ULONG device_unit;
	struct MsgPort *cd_mp;
	struct IOStdReq *cd_io;
	BYTE error;
	
	if (drive && drive[0]) {
		LONG len;
		strlcpy(drive_name, drive, sizeof(drive_name));
		len = strlen(drive_name);
		if (len && drive_name[len-1] == ':') {
			drive_name[len-1] = 0;
		}
	} else {
		strcpy(drive_name, "CD0");
	}
	
	dl = LockDosList(flags);
	if ((dl = FindDosEntry(dl, drive_name, flags))) {
		fssm = CheckBPTR(dl->dol_misc.dol_handler.dol_Startup);
		if (fssm && CheckBPTR(fssm->fssm_Device)) {
			CopyStringBSTRToC(fssm->fssm_Device, device_name, sizeof(device_name));
			device_unit = fssm->fssm_Unit;
		} else {
			fssm = NULL;
		}
	}
	UnLockDosList(flags);
	if (!fssm) {
		return NULL;
	}

	cd_mp = CreateMsgPort();
	cd_io = (struct IOStdReq *)CreateIORequest(cd_mp, sizeof(struct IOExtTD));
	if (!cd_io) {
		DeleteMsgPort(cd_mp);
		return NULL;
	}
	
	error = OpenDevice(device_name, device_unit, (struct IORequest *)cd_io, 0);
	if (error) {
		DeleteIORequest(cd_io);
		DeleteMsgPort(cd_mp);
		return NULL;
	}
	
	return cd_io;
}

void FreeCDDevice (struct IOStdReq *cd_io) {
	if (cd_io) {
		struct MsgPort *cd_mp = cd_io->io_Message.mn_ReplyPort;
		CloseDevice((struct IORequest *)cd_io);
		DeleteIORequest((struct IORequest *)cd_io);
		DeleteMsgPort(cd_mp);
	}
}

BYTE ReadCDTOC (struct IOStdReq *cd_io, struct CDTOC *cd_toc) {
	UBYTE command[10];
	UBYTE tocbuf[804];
	BYTE error;
	ULONG tocsize;
	ULONG num_tracks;
	ULONG track;
	
	memset(command, 0, sizeof(command));
	command[0] = SCSI_CMD_READTOC;
	wbe16(&command[7], sizeof(tocbuf));
	error = DoSCSICmd(cd_io, command, sizeof(command),
		tocbuf, sizeof(tocbuf), SCSIF_READ|SCSIF_AUTOSENSE);
	if (error) {
		return error;
	}
	
	tocsize = rbe16(&tocbuf[0]);
	if (tocsize >= 2) tocsize -= 2;
	num_tracks = (tocsize >> 3) - 1;
	
	cd_toc->FirstTrack = 1;
	cd_toc->LastTrack = num_tracks;
	for (track = 0; track < num_tracks; track++) {
		cd_toc->Tracks[track].IsAudio = (tocbuf[4 + (track << 3) + 1] & 4) ? FALSE : TRUE;
		cd_toc->Tracks[track].StartAddr = rbe32(&tocbuf[4 + (track << 3) + 4]);
		cd_toc->Tracks[track].EndAddr = rbe32(&tocbuf[4 + (track << 3) + 12]);
		if (!cd_toc->CurrentTrack && cd_toc->Tracks[track].IsAudio) {
			cd_toc->CurrentTrack = track + 1;
		}
	}
	for (track = num_tracks; track < MAX_TRACKS; track++) {
		cd_toc->Tracks[track].IsAudio = FALSE;
		cd_toc->Tracks[track].StartAddr = 0;
		cd_toc->Tracks[track].EndAddr = 0;
	}
	cd_toc->FirstTrack = cd_toc->CurrentTrack;
	cd_toc->CurrentAddr = cd_toc->Tracks[cd_toc->CurrentTrack-1].StartAddr;
	
	return 0;
}

BYTE DoSCSICmd (struct IOStdReq *io, APTR cmd, ULONG cmd_len,
	APTR buf, ULONG buf_len, ULONG flags)
{
	struct SCSICmd scsicmd;
	UBYTE scsisense[252];
	
	if (!io->io_Device) {
		return IOERR_OPENFAIL;
	}
	memset(&scsicmd, 0, sizeof(scsicmd));
	scsicmd.scsi_Data = buf;
	scsicmd.scsi_Length = buf_len;
	scsicmd.scsi_SenseData = scsisense;
	scsicmd.scsi_SenseLength = sizeof(scsisense);
	scsicmd.scsi_Command = cmd;
	scsicmd.scsi_CmdLength = cmd_len;
	scsicmd.scsi_Flags = flags;
	io->io_Command = HD_SCSICMD;
	io->io_Data = &scsicmd;
	io->io_Length = sizeof(scsicmd);
	return DoIO((struct IORequest *)io);
}
