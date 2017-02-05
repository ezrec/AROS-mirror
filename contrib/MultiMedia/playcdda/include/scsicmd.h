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

#ifndef SCSICMD_H
#define SCSICMD_H

#include <devices/trackdisk.h>
#include <devices/scsidisk.h>

#define MAX_TRACKS 32
#define SCSI_CMD_READTOC 0x43
#define SCSI_CMD_READ_CD 0xbe
#define SCSI_CMD_READ_CD_MSF 0xb9

struct CDTOC {
	UWORD FirstTrack;
	UWORD LastTrack;
	UBYTE IsPlaying, IsPaused;
	UWORD CurrentTrack;
	ULONG CurrentAddr;
	struct {
		ULONG IsAudio;
		ULONG StartAddr;
		ULONG EndAddr;
	} Tracks[MAX_TRACKS];
};

#define ADDR2MSF(x,m,s,f) do { \
	m = ((x) / 75UL) / 60UL; \
	s = ((x) / 75UL) % 60UL; \
	f = (x) % 75UL; \
	} while (0)

struct IOStdReq *GetCDDevice (const char *drive);
void FreeCDDevice (struct IOStdReq *cd_io);
BYTE ReadCDTOC (struct IOStdReq *io, struct CDTOC *cd_toc);
BYTE DoSCSICmd (struct IOStdReq *io, APTR cmd, ULONG cmd_len,
	APTR buf, ULONG buf_len, ULONG flags);

#endif
