/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

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

/* Stub functions for system-level CD-ROM audio control */

#include	<stdio.h>

#include "SDL_cdrom.h"
#include "SDL_syscdrom.h"

#include	<dos/dosextens.h>
#include	<dos/filehandler.h>
#include	<devices/scsidisk.h>

#include	<proto/dos.h>
#include	<proto/exec.h>
#include	<aros/macros.h>
#include 	"../../main/aros/mydebug.h"

/* Just large enought to hold TOC */
#define	TOC_SIZE				804
#define	SCSI_INBUF_SIZE	TOC_SIZE
#define	SCSI_SENSE_SIZE	252

#ifdef __AROS__
#define AllocTaskPooled(x) AllocVec(x,MEMF_ANY)
#define FreeTaskPooled(x,y) FreeVec(x)
#endif

struct MyCDROM
{
	char	dosname[20];
	char	devname[108];
	ULONG	unit;
	ULONG	unitflags;

	/* Filled in when this unit is opened	*/

	struct IOStdReq	*req;
	struct SCSICmd		scsicmd;
	UWORD	*inbuf;
	UBYTE	*sensebuf;
};

struct TrackInfo
{
	UBYTE	Pad1, Flags, TrackNumber, Pad2;
	ULONG	Address;
};

struct CD_TOC
{
	UWORD	Length;
	UBYTE	FirstTrack;
	UBYTE	LastTrack;
	struct TrackInfo	TOCData[100];
};

UBYTE                *CDList[MAX_DRIVES];

/**********************************************************************
	FindDup

	Return 1 if duplicate
**********************************************************************/

static ULONG FindDup(UBYTE **cdlist, CONST_STRPTR devname, ULONG unit)
{
	struct MyCDROM	*entry;
	ULONG	i;

	D(bug("CDRom : FindDup\n"));
	
	for (i = 0; i < MAX_DRIVES; i++)
	{
		entry	= (struct MyCDROM *)cdlist[i];

		if (entry == NULL)
			break;

		if (entry->unit == unit && strcmp(entry->devname, devname) == 0)
			return 1;
	}

	return 0;
}

/**********************************************************************
	CopyStringBSTRToC

	Convert BCPL string to STRPTR
**********************************************************************/

LONG CopyStringBSTRToC (BSTR src, STRPTR dst, ULONG dst_size)
{
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

/**********************************************************************
	SendCMD
**********************************************************************/

static LONG SendCMD(SDL_CD *cdrom, const UBYTE *cmd, int cmdlen)
{
	struct MyCDROM	*entry	= (struct MyCDROM *)CDList[cdrom->id];

	D(bug("CDRom : SendCMD..."));
	
	entry->scsicmd.scsi_Data			= entry->inbuf;
	entry->scsicmd.scsi_Length			= SCSI_INBUF_SIZE;
	entry->scsicmd.scsi_SenseActual		= 0;
	entry->scsicmd.scsi_SenseData		= (UBYTE *)entry->sensebuf;
	entry->scsicmd.scsi_SenseLength		= SCSI_SENSE_SIZE;
	entry->scsicmd.scsi_CmdActual		= 0;
	entry->scsicmd.scsi_Command			= (UBYTE *)cmd;
	entry->scsicmd.scsi_CmdLength		= cmdlen;
	entry->scsicmd.scsi_Flags			= SCSIF_READ | SCSIF_AUTOSENSE;

	DoIO((struct IORequest *)entry->req);
	
	D(bug("%s\n",entry->scsicmd.scsi_Status ? "NOK" : "OK"));

	return (entry->scsicmd.scsi_Status ? -1 : 0);
}

static const char *SDL_SYS_CDName(int drive)
{
	D(bug("CDRom : SDL_SYS_CDName\n"));

	return(CDList[drive]);
}

static void SDL_DisposeCD(struct MyCDROM *entry)
{
	D(bug("CDRom : SDL_DisposeCD\n"));

	DeleteIORequest((struct IORequest *)entry->req);

	if (entry->inbuf)
		FreeTaskPooled(entry->inbuf, SCSI_INBUF_SIZE);

	if (entry->sensebuf)
		FreeTaskPooled(entry->sensebuf, SCSI_SENSE_SIZE);

	entry->req			= NULL;
	entry->inbuf		= NULL;
	entry->sensebuf	= NULL;
}

static int SDL_SYS_CDOpen(int drive)
{
	struct MyCDROM	*entry;
	struct MsgPort	*port;

	D(bug("CDRom : SDL_SYS_CDOpen\n"));

	/* Yay! Use DOS message port :-) */

	entry	= (struct MyCDROM *)CDList[drive];
	port	= &((struct Process *)FindTask(NULL))->pr_MsgPort;

	if ((entry->req = CreateIORequest(port, sizeof(struct IOStdReq))) != NULL)
	{
		if ((entry->inbuf		= AllocTaskPooled(SCSI_INBUF_SIZE)) != NULL)
		if ((entry->sensebuf	= AllocTaskPooled(SCSI_SENSE_SIZE)) != NULL)
		if (OpenDevice(entry->devname, entry->unit, (struct IORequest *)entry->req, entry->unitflags) == 0)
		{
			entry->req->io_Command	= HD_SCSICMD;
			entry->req->io_Data		= (APTR)&entry->scsicmd;
			entry->req->io_Length	= sizeof(struct SCSICmd);

			return drive;
		}

		SDL_DisposeCD(entry);
	}

	return -1;
}

static void SDL_SYS_CDClose(SDL_CD *cdrom)
{
	struct MyCDROM	*entry	= (struct MyCDROM *)CDList[cdrom->id];

	D(bug("CDRom : SDL_SYS_CDClose\n"));

	
	if (entry && entry->req)
	{
		CloseDevice((struct IORequest *)entry->req);
		SDL_DisposeCD(entry);
	}
}

static int SDL_SYS_CDGetTOC(SDL_CD *cdrom)
{
	static const UBYTE	Cmd[10] =
		{ 0x43, 0, 0, 0, 0, 0, 0, TOC_SIZE >> 8, TOC_SIZE & 0xff, 0 };
	int	okay;

	D(bug("CDRom : SDL_SYS_CDGetTOC\n"));
	
	okay	= 0;

	if (SendCMD(cdrom, Cmd, sizeof(Cmd)) == 0)
	{
		struct MyCDROM	*entry	= (struct MyCDROM *)CDList[cdrom->id];
		struct CD_TOC	*toc;
		int	i;

		toc	= (struct CD_TOC *)entry->inbuf;
		
		D(bug("TOC : length = %d, First Track = %d, Last Track = %d\n",AROS_BE2WORD(toc->Length), toc->FirstTrack, toc->LastTrack));

		cdrom->numtracks = toc->LastTrack - toc->FirstTrack + 1;

		if ( cdrom->numtracks > SDL_MAX_TRACKS ) {
			cdrom->numtracks = SDL_MAX_TRACKS;
		}
		
		D(bug("Found %d tracks\n",cdrom->numtracks));

		/* Read all the track TOC entries */

		okay	= 1;

		for ( i=0; i<cdrom->numtracks; ++i )
		{
			cdrom->track[i].id = i+1;

			D(bug("Track %d :",i));
			D(bug("%02X, %02X, %02X, %02X\t",toc->TOCData[i].Pad1, toc->TOCData[i].Flags, toc->TOCData[i].TrackNumber, toc->TOCData[i].Pad2));

			if (toc->TOCData[i].Flags & 0x04)
			{
				cdrom->track[i].type	= SDL_DATA_TRACK;
				D(bug("\tDATA\t"));
			}
			else
			{
				cdrom->track[i].type	= SDL_AUDIO_TRACK;
				D(bug("\tAUDIO\t"));
			}

			cdrom->track[i].offset		= AROS_BE2LONG(toc->TOCData[i].Address);

			if (i == cdrom->numtracks)
			{
				cdrom->track[i].length	= 0;
				D(bug("length : %d\n",0));
			}
			else
			{
				cdrom->track[i].length		= AROS_BE2LONG(toc->TOCData[i+1].Address) - cdrom->track[i].offset;
				D(bug("length : %d\n",cdrom->track[i].length));
			}
		}
	}

	return(okay ? 0 : -1);
}

static CDstatus SDL_SYS_CDStatus(SDL_CD *cdrom, int *position)
{
	static const UBYTE	Cmd[10]	=
		{ 0x42, 0, 0x40, 0x01, 0, 0, 0, SCSI_INBUF_SIZE >> 8, SCSI_INBUF_SIZE & 0xff, 0 };
	CDstatus	status;

	D(bug("CDRom : SDL_SYS_CDStatus\n"));

	status	= CD_ERROR;

	if (SendCMD(cdrom, Cmd, sizeof(Cmd)) == 0)
	{
		struct MyCDROM	*entry	= (struct MyCDROM *)CDList[cdrom->id];
		UBYTE	*buf;
		
		buf	= (UBYTE *)entry->inbuf;

		D(bug("CDRom : Status : %02X,%02X\n", buf[0],buf[1]));
		
		switch (buf[1])
		{
			case 0x11: 	status	= CD_PLAYING;	break;
			case 0x12: 	status	= CD_PAUSED;	break;
			case 0x13: 	status	= CD_STOPPED;	break;
			case 0x14: 	status	= CD_ERROR;	break;
			default:	status	= CD_STOPPED;	break;
		}

	}

	return status;
}

static int SDL_SYS_CDPlay(SDL_CD *cdrom, int start, int length)
{
	UBYTE	Cmd[12]	= { 0xa5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	UWORD	*tmp		= (UWORD *)Cmd;

	D(bug("CDRom : SDL_SYS_CDPlay\n"));

	tmp[1]	= AROS_BE2WORD(start >> 16);
	tmp[2]	= AROS_BE2WORD(start);
	tmp[3]	= AROS_BE2WORD(length >> 16);
	tmp[4]	= AROS_BE2WORD(length);

	SendCMD(cdrom, Cmd, sizeof(Cmd));
	return 0;
}

static int SDL_SYS_CDPause(SDL_CD *cdrom)
{
	static const UBYTE	Cmd[10] =
		{ 0x4b, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	D(bug("CDRom : SDL_SYS_CDPause\n"));

	SendCMD(cdrom, Cmd, sizeof(Cmd));
	return 0;
}

static int SDL_SYS_CDResume(SDL_CD *cdrom)
{
	static const UBYTE Cmd[10] = { 0x4b, 0, 0, 0, 0, 0, 0, 0, 1, 0 };

	D(bug("CDRom : SDL_SYS_CDResume\n"));

	SendCMD(cdrom, Cmd, sizeof(Cmd));
	return 0;
}

static int SDL_SYS_CDStop(SDL_CD *cdrom)
{
	static const UBYTE Cmd[10] = { 0x4e, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	D(bug("CDRom : SDL_SYS_CDStop\n"));

	SendCMD(cdrom, Cmd, sizeof(Cmd));
	return 0;
}

static int SDL_SYS_CDEject(SDL_CD *cdrom)
{
	static const UBYTE	Cmd[6] =
		{ 0x1b, 0, 0, 0, 0x02, 0 };

	D(bug("CDRom : SDL_SYS_CDEject\n"));
   
	SendCMD(cdrom, (UBYTE *)Cmd, sizeof(Cmd));
	return 0;
}

int  SDL_SYS_CDInit(void)
{
	struct DosList	*dlist;
	ULONG	devices;
	UBYTE	**cdlist;

	D(bug("CDRom : SDL_SYS_CDInit\n"));

	cdlist	= CDList;
	devices	= 0;
	SDL_numcds = 0;

	SDL_CDcaps.Name 	= SDL_SYS_CDName;
	SDL_CDcaps.Open 	= SDL_SYS_CDOpen;
	SDL_CDcaps.GetTOC 	= SDL_SYS_CDGetTOC;
	SDL_CDcaps.Status 	= SDL_SYS_CDStatus;
	SDL_CDcaps.Play 	= SDL_SYS_CDPlay;
	SDL_CDcaps.Pause 	= SDL_SYS_CDPause;
	SDL_CDcaps.Resume 	= SDL_SYS_CDResume;
	SDL_CDcaps.Stop 	= SDL_SYS_CDStop;
	SDL_CDcaps.Eject 	= SDL_SYS_CDEject;
	SDL_CDcaps.Close 	= SDL_SYS_CDClose;

	dlist	= LockDosList(LDF_DEVICES|LDF_READ);

	while ((dlist = NextDosEntry(dlist, LDF_DEVICES|LDF_READ)) != NULL && devices < MAX_DRIVES)
	{
		struct FileSysStartupMsg	*startup;
		CONST_STRPTR	devname;
		ULONG	devnamelen;
		
		startup = BADDR(dlist->dol_misc.dol_handler.dol_Startup);
		if ((ULONG) startup > 0x400 &&
		    (startup->fssm_Unit & 0xff000000) == 0 &&
		     startup->fssm_Device && startup->fssm_Environ)
		{

			if ((devname = BADDR(startup->fssm_Device)) != NULL)
			{
				if ((devnamelen	= CopyStringBSTRToC((BSTR)devname, (STRPTR)devname, 108)) != 0)
				{
					if (FindDup(cdlist, devname, startup->fssm_Unit) == 0)
					{
						struct MyCDROM	*entry;

						if ((entry =  AllocTaskPooled(sizeof(*entry))) != NULL)
						{
							CONST_STRPTR	dosname;
							ULONG	dosnamelen;

							dosname		= (CONST_STRPTR)BADDR(dlist->dol_Name);
							dosnamelen	= CopyStringBSTRToC((BSTR)dosname, (STRPTR)dosname, 20);

							entry->unit			= startup->fssm_Unit;
							entry->unitflags	= startup->fssm_Flags;

							memcpy(entry->dosname, dosname, dosnamelen);
							memcpy(entry->devname, devname, devnamelen);

							entry->dosname[ dosnamelen ]	= '\0';
							entry->devname[ devnamelen ]	= '\0';

							D(bug("CDRom : devname : %s, dosname %s\n",entry->devname,entry->dosname));

							entry->req			= NULL;
							entry->inbuf		= NULL;
							entry->sensebuf		= NULL;

							cdlist[devices] = (char *)entry;
							devices++;
						}
					}
				}
			}
		}
	}

	UnLockDosList(LDF_DEVICES|LDF_READ);

	if (devices)
	{
		struct IOStdReq	*req;
		struct SCSICmd		*scsicmd;
		struct MsgPort	*port;
		ULONG	i;
		UBYTE	*inbuf, *sensebuf;

		port	= &((struct Process *)FindTask(NULL))->pr_MsgPort;

		if ((req = CreateIORequest(port, sizeof(struct IOStdReq))) != NULL)
		{
			if ((inbuf = AllocTaskPooled(SCSI_INBUF_SIZE)) != NULL)
			{
				if ((sensebuf = AllocTaskPooled(SCSI_SENSE_SIZE)) != NULL)
				{
					if ((scsicmd = (struct SCSICmd *)AllocTaskPooled(sizeof(*scsicmd))) != NULL)
					{
						req->io_Command	= HD_SCSICMD;
						req->io_Data		= (APTR)scsicmd;
						req->io_Length	= sizeof(struct SCSICmd);

						for (i = 0; i < MAX_DRIVES; i++)
						{
							struct MyCDROM	*entry;
							ULONG	is_cdrom;

							entry	= (struct MyCDROM *)cdlist[i];

							if (entry == NULL)
								break;

							is_cdrom	= 0;

							if (OpenDevice(entry->devname, entry->unit, (struct IORequest *)req, entry->unitflags) == 0)
							{
								static const UBYTE	Cmd[]	= { 0x12, 0, 0, 0, 252, 0 };

								scsicmd->scsi_Data			= (UWORD *)inbuf;
								scsicmd->scsi_Length			= SCSI_INBUF_SIZE;
								scsicmd->scsi_SenseActual	= 0;
								scsicmd->scsi_SenseData		= sensebuf;
								scsicmd->scsi_SenseLength	= SCSI_SENSE_SIZE;
								scsicmd->scsi_CmdActual		= 0;
								scsicmd->scsi_Command		= (UBYTE *)Cmd;
								scsicmd->scsi_CmdLength		= sizeof(Cmd);
								scsicmd->scsi_Flags			= SCSIF_READ | SCSIF_AUTOSENSE;

								DoIO((struct IORequest *)req);

								if (scsicmd->scsi_Status == 0 && req->io_Error == 0)
								{
									/* 5 is for CDROM (maybe not proper checking) */
									/* Must be removable as well*/

									D(
									{
										int i;
										for (i=0;i<2;i++) bug("inbuf[%d] : %04X\n", i, inbuf[i]);
									}
									)
									
									if (((inbuf[0] & 0x05) == 5) && (inbuf[1] & 0x80))
										is_cdrom	= 1;
								}
								D(else bug("CDRom : DoIO error\n"));

								CloseDevice((struct IORequest *)req);
							}

							if (is_cdrom == 0)
							{
								devices--;
								cdlist[i]	= cdlist[devices];
							}
							else 
							{
								SDL_numcds++;
								D(bug("CDRom detected\n"));
							}
						}

						FreeTaskPooled(scsicmd, sizeof(*scsicmd));
					}

					FreeTaskPooled(sensebuf, SCSI_SENSE_SIZE);
				}

				FreeTaskPooled(inbuf, SCSI_INBUF_SIZE);
			}

			DeleteIORequest((struct IORequest *)req);
		}
	}

	return(0);
}

void SDL_SYS_CDQuit(void)
{
	D(bug("CDRom : SDL_SYS_CDQuit\n"));
}
