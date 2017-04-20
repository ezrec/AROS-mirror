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
#include <libraries/mui.h>
#include <devices/ahi.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/icon.h>
#include <clib/alib_protos.h>
#include <SDI_hook.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "scsicmd.h"
#include "diskchange.h"
#include "seekbar.h"
#include "endian.h"
#include "PlayCDDA_rev.h"

#define FRAME_SIZE 2352
#define CDDA_FRAMES 150
#define CDDA_BUFFER_SIZE (FRAME_SIZE*CDDA_FRAMES)
#define AHI_FRAMES 15
#define AHI_BUFFER_SIZE (FRAME_SIZE*AHI_FRAMES)

#ifndef MIN
#define MIN(a,b) ((a)<=(b)?(a):(b))
#endif
#define SWAP_PTR(a,b) { APTR tmp_ptr = a; a = b; b = tmp_ptr; }

#ifndef IOERR_SUCCESS
#define IOERR_SUCCESS 0
#endif

#define PROGNAME "PlayCDDA"
const char USED verstag[] = VERSTAG;

#ifndef DtpicObject
#define DtpicObject MUI_NewObject(MUIC_Dtpic
#endif

enum {
	GID_POSITION = 0,
	GID_EJECT,
	GID_STOP,
	GID_PAUSE,
	GID_PREV,
	GID_PLAY,
	GID_NEXT,
	GID_VOLUME,
	GID_TRACK01,
	GID_TRACK32 = GID_TRACK01 + 31,
	GID_STATUSDISP,
	NUM_GADGETS
};

void UpdateGUI (Object *wnd, Object **gad, struct CDTOC *cd_toc);
Object *CreateTable (LONG rows, LONG cols, Object **buttons);
Object *CreateColumn (LONG rows, LONG cols, LONG i, Object **buttons);

HOOKPROTO(SeekFunc, IPTR, APTR unused, IPTR *params);
MakeHook(SeekHook, SeekFunc);
HOOKPROTO(EjectFunc, IPTR, APTR unused, IPTR *params);
MakeHook(EjectHook, EjectFunc);
HOOKPROTO(StopFunc, IPTR, APTR unused, IPTR *params);
MakeHook(StopHook, StopFunc);
HOOKPROTO(PauseFunc, IPTR, APTR unused, IPTR *params);
MakeHook(PauseHook, PauseFunc);
HOOKPROTO(PrevFunc, IPTR, APTR unused, IPTR *params);
MakeHook(PrevHook, PrevFunc);
HOOKPROTO(PlayFunc, IPTR, APTR unused, IPTR *params);
MakeHook(PlayHook, PlayFunc);
HOOKPROTO(NextFunc, IPTR, APTR unused, IPTR *params);
MakeHook(NextHook, NextFunc);
HOOKPROTO(TrackFunc, IPTR, APTR unused, IPTR *params);
MakeHook(TrackHook, TrackFunc);

struct MUI_CustomClass *SeekBarClass;
struct IOStdReq *cd_io;
LONG load_eject = 1;
struct IOStdReq *cdda_io;
BOOL cdda_io_busy;
ULONG cdda_addr = -1, cdda_pos, cdda_frames;
TEXT status_disp[32];
LONG cd_in_drive = FALSE;

int main (void) {
	struct DiskObject *icon = NULL;
	const char *dosdev;
	APTR cd_change = NULL;
	LONG cd_signal = -1;
	ULONG gui_sig, cd_sig;
	struct CDTOC cd_toc;
	Object *app = NULL, *wnd;
	Object *gad[NUM_GADGETS];
	ULONG signals = 0;
	volatile ULONG volume = 64;
	struct MsgPort *ahi_mp = NULL;
	struct AHIRequest *ahi_io = NULL, *ahi_io2 = NULL, *join = NULL;
	APTR ahi_buf = NULL, ahi_buf2 = NULL;
	APTR cdda_buf = NULL, cdda_buf2 = NULL;
	struct SCSICmd scsicmd = { 0 };
	UBYTE scsisense[20];
	UBYTE readcmd[12] = { SCSI_CMD_READ_CD, 0x04, 0, 0, 0, 0, 0, 0, 0, 0x10, 0, 0 };
	
	if (!(ahi_buf = AllocVec(AHI_BUFFER_SIZE, MEMF_ANY)) ||
		!(ahi_buf2 = AllocVec(AHI_BUFFER_SIZE, MEMF_ANY)) ||
		!(cdda_buf = AllocVec(CDDA_BUFFER_SIZE, MEMF_ANY)) ||
		!(cdda_buf2 = AllocVec(CDDA_BUFFER_SIZE, MEMF_ANY)))
	{
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "Can't allocate memory");
		goto error;
	}

	ahi_mp = CreateMsgPort();
	if (!(ahi_io = CreateIORequest(ahi_mp, sizeof(*ahi_io))) ||
		!(ahi_io2 = CreateIORequest(ahi_mp, sizeof(*ahi_io))))
	{
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "CreateIORequest() failed");
		goto error;
	}
	if (OpenDevice(AHINAME, AHI_DEFAULT_UNIT, (struct IORequest *)ahi_io, 0) != IOERR_SUCCESS) {
		ahi_io->ahir_Std.io_Device = NULL;
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "OpenDevice() failed");
		goto error;
	}
	CopyMem(ahi_io, ahi_io2, sizeof(*ahi_io));

	icon = GetDiskObjectNew("PROGDIR:"PROGNAME);
	if (!icon) {
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "GetDiskObjectNew() failed");
		goto error;
	}
	
	SeekBarClass = SeekBar_CreateClass();
	if (!SeekBarClass) {
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "SeekBar_CreateClass()");
		goto error;
	}
	
	app = ApplicationObject,
		MUIA_Application_Title,					PROGNAME,
		MUIA_Application_Version,				&verstag[1],
		MUIA_Application_DiskObject,			icon,
		SubWindow,								wnd = WindowObject,
			MUIA_Window_Title,					VERS,
			WindowContents,						HGroup,
				Child,							VGroup,
					Child,						gad[GID_STATUSDISP] = TextObject,
						MUIA_Frame,				MUIV_Frame_Text,
						MUIA_Text_Contents,		"No disc in drive",
					End,
					Child,						gad[GID_POSITION] = NewObject(SeekBarClass->mcc_Class, NULL,
						MUIA_InputMode,			MUIV_InputMode_RelVerify,
						MUIA_Disabled,			TRUE,
					TAG_END),
					Child,						HGroup,
						Child,					gad[GID_EJECT] = DtpicObject,
							ImageButtonFrame,
							MUIA_InputMode,		MUIV_InputMode_RelVerify,
							MUIA_Dtpic_Name,	"tbimages:tapeeject",
							MUIA_ShortHelp,		"Eject",
						End,
						Child,					gad[GID_STOP] = DtpicObject,
							ImageButtonFrame,
							MUIA_InputMode,		MUIV_InputMode_RelVerify,
							MUIA_Dtpic_Name,	"tbimages:tapestop",
							MUIA_ShortHelp,		"Stop",
							MUIA_Disabled,		TRUE,
						End,
						Child,					gad[GID_PAUSE] = DtpicObject,
							ImageButtonFrame,
							MUIA_InputMode,		MUIV_InputMode_RelVerify,
							MUIA_Dtpic_Name,	"tbimages:tapepause",
							MUIA_ShortHelp,		"Pause",
							MUIA_Disabled,		TRUE,
						End,
						Child,					gad[GID_PREV] = DtpicObject,
							ImageButtonFrame,
							MUIA_InputMode,		MUIV_InputMode_RelVerify,
							MUIA_Dtpic_Name,	"tbimages:tapelast",
							MUIA_ShortHelp,		"Previous",
							MUIA_Disabled,		TRUE,
						End,
						Child,					gad[GID_PLAY] = DtpicObject,
							ImageButtonFrame,
							MUIA_InputMode,		MUIV_InputMode_RelVerify,
							MUIA_Dtpic_Name,	"tbimages:tapeplay",
							MUIA_ShortHelp,		"Play",
							MUIA_Disabled,		TRUE,
						End,
						Child,					gad[GID_NEXT] = DtpicObject,
							ImageButtonFrame,
							MUIA_InputMode,		MUIV_InputMode_RelVerify,
							MUIA_Dtpic_Name,	"tbimages:tapenext",
							MUIA_ShortHelp,		"Next",
							MUIA_Disabled,		TRUE,
						End,
					End,
				End,
				Child,							CreateTable(4, 8, &gad[GID_TRACK01]),
				Child,							VGroup,
					Child,						gad[GID_VOLUME] = SliderObject,
						MUIA_Slider_Horiz,		FALSE,
						MUIA_Slider_Min,		0,
						MUIA_Slider_Max,		64,
						MUIA_Slider_Level,		64,
					End,
					Child,						TextObject,
						MUIA_Text_Contents,		"Volume",
					End,
				End,
			End,
		End,
	End;
	if (!app) {
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "Can't create application object");
		goto error;
	}
	
	DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
		app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(gad[GID_POSITION], MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
		app, 5, MUIM_CallHook, &SeekHook, wnd, gad, &cd_toc);
	DoMethod(gad[GID_EJECT], MUIM_Notify, MUIA_Pressed, FALSE,
		app, 5, MUIM_CallHook, &EjectHook, wnd, gad, &cd_toc);
	DoMethod(gad[GID_STOP], MUIM_Notify, MUIA_Pressed, FALSE,
		app, 5, MUIM_CallHook, &StopHook, wnd, gad, &cd_toc);
	DoMethod(gad[GID_PAUSE], MUIM_Notify, MUIA_Pressed, FALSE,
		app, 5, MUIM_CallHook, &PauseHook, wnd, gad, &cd_toc);
	DoMethod(gad[GID_PREV], MUIM_Notify, MUIA_Pressed, FALSE,
		app, 5, MUIM_CallHook, &PrevHook, wnd, gad, &cd_toc);
	DoMethod(gad[GID_PLAY], MUIM_Notify, MUIA_Pressed, FALSE,
		app, 5, MUIM_CallHook, &PlayHook, wnd, gad, &cd_toc);
	DoMethod(gad[GID_NEXT], MUIM_Notify, MUIA_Pressed, FALSE,
		app, 5, MUIM_CallHook, &NextHook, wnd, gad, &cd_toc);
	{
		ULONG track;
		for (track = 0; track < 32; track++) {
			DoMethod(gad[GID_TRACK01 + track], MUIM_Notify, MUIA_Pressed, FALSE,
				app, 6, MUIM_CallHook, &TrackHook, wnd, gad, &cd_toc, track);
		}
	}
	DoMethod(gad[GID_VOLUME], MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
		app, 3, MUIM_WriteLong, MUIV_TriggerValue, &volume);
	
	set(wnd, MUIA_Window_Open, TRUE);
	if (!XGET(wnd, MUIA_Window_Open)) {
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "Can't open window");
		goto error;
	}
	
	dosdev = FindToolType(icon->do_ToolTypes, "DOSDEV");
	if (!dosdev) {
		dosdev = "CD0";
	}
	cd_io = GetCDDevice(dosdev);
	if (!cd_io) {
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "GetCDDevice() failed");
		goto error;
	}
	
	cdda_io = CreateIORequest(cd_io->io_Message.mn_ReplyPort, sizeof(struct IOStdReq));
	if (!cdda_io) {
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "CreateIORequest() failed");
		goto error;
	}
	CopyMem(cd_io, cdda_io, sizeof(struct IOStdReq));
	
	scsicmd.scsi_SenseData = scsisense;
	scsicmd.scsi_SenseLength = sizeof(scsisense);
	scsicmd.scsi_Command = readcmd;
	scsicmd.scsi_CmdLength = sizeof(readcmd);
	scsicmd.scsi_Flags = SCSIF_READ|SCSIF_AUTOSENSE;
	cdda_io->io_Command = HD_SCSICMD;
	cdda_io->io_Data = &scsicmd;
	cdda_io->io_Length = sizeof(scsicmd);

	cd_signal = AllocSignal(-1);
	cd_sig = 1UL << cd_signal;
	cd_change = AddDiskChangeHandler(cd_io, cd_sig);
	if (!cd_change) {
		MUI_Request(app, NULL, 0, "PlayCDDA error", "OK", "AddDiskChangeHandler() failed");
		goto error;
	}
	
	cd_io->io_Command = TD_CHANGESTATE;
	DoIO((struct IORequest *)cd_io);
	cd_in_drive = (cd_io->io_Error || cd_io->io_Actual) ? FALSE : TRUE;
	memset(&cd_toc, 0, sizeof(cd_toc));
	if (cd_in_drive) {
		load_eject = 1;
		ReadCDTOC(cd_io, &cd_toc);
		cdda_addr = cd_toc.CurrentAddr;
	}
	UpdateGUI(wnd, gad, &cd_toc);
	
	while ((LONG)DoMethod(app, MUIM_Application_NewInput, (IPTR)&gui_sig) !=
		MUIV_Application_ReturnID_Quit)
	{
		if (!cd_toc.IsPlaying || cd_toc.IsPaused) {
			signals = Wait(gui_sig|cd_sig|SIGBREAKF_CTRL_C);
		} else {
			signals = CheckSignal(gui_sig|cd_sig|SIGBREAKF_CTRL_C);
		}
		
		if (signals & SIGBREAKF_CTRL_C) {
			break;
		}
		
		if (signals & cd_sig) {
			if (cdda_io_busy) {
				AbortIO((struct IORequest *)cdda_io);
				WaitIO((struct IORequest *)cdda_io);
				cdda_io_busy = FALSE;
			}
			cdda_frames = 0;
			cd_io->io_Command = TD_CHANGESTATE;
			DoIO((struct IORequest *)cd_io);
			cd_in_drive = (cd_io->io_Error || cd_io->io_Actual) ? FALSE : TRUE;
			memset(&cd_toc, 0, sizeof(cd_toc));
			if (cd_in_drive) {
				load_eject = 1;
				ReadCDTOC(cd_io, &cd_toc);
			}
			UpdateGUI(wnd, gad, &cd_toc);
			cdda_addr = cd_toc.CurrentAddr;
		}
		
		if (cd_toc.IsPlaying && !cd_toc.IsPaused) {
			ULONG track = cd_toc.CurrentTrack;;
			ULONG addr = cd_toc.CurrentAddr;
			ULONG endaddr = cd_toc.Tracks[cd_toc.LastTrack-1].EndAddr;
			ULONG trackaddr = addr - cd_toc.Tracks[track-1].StartAddr;
			if (!cdda_frames) {
				ULONG frames, bytes;
				cdda_pos = 0;
				if (cdda_io_busy) {
					WaitIO((struct IORequest *)cdda_io);
					cdda_io_busy = FALSE;
					SWAP_PTR(cdda_buf, cdda_buf2);
				} else {
					frames = MIN(CDDA_FRAMES, endaddr - cdda_addr);
					bytes = frames * FRAME_SIZE;
					scsicmd.scsi_Data = cdda_buf;
					scsicmd.scsi_Length = bytes;
					wbe32(&readcmd[2], cdda_addr);
					readcmd[8] = frames;
					DoIO((struct IORequest *)cdda_io);
					cdda_addr += frames;
				}
				if (cdda_io->io_Error == IOERR_SUCCESS) {
					cdda_frames = CDDA_FRAMES;
					frames = MIN(CDDA_FRAMES, endaddr - cdda_addr);
					bytes = frames * FRAME_SIZE;
					scsicmd.scsi_Data = cdda_buf2;
					scsicmd.scsi_Length = bytes;
					wbe32(&readcmd[2], cdda_addr);
					readcmd[8] = frames;
					SendIO((struct IORequest *)cdda_io);
					cdda_addr += frames;
					cdda_io_busy = TRUE;
				}
			}
			if (!XGET(gad[GID_POSITION], MUIA_Pressed)) {
				struct Screen *screen;
				screen = (struct Screen *)XGET(wnd, MUIA_Window_Screen);
				if (screen && AttemptLockLayerRom(screen->BarLayer)) {
					UnlockLayerRom(screen->BarLayer);
					snprintf(status_disp, sizeof(status_disp), "Track %02d - %02d:%02d",
						track, (trackaddr / 75) / 60, (trackaddr / 75) % 60);
					set(gad[GID_STATUSDISP], MUIA_Text_Contents, status_disp);
					set(gad[GID_POSITION], MUIA_Slider_Level, addr);
				}
			}
			if (cdda_frames) {
				ULONG frames = MIN(AHI_FRAMES, endaddr - addr);
				ULONG bytes = frames * FRAME_SIZE;
#if WORDS_BIGENDIAN
				swab((UBYTE *)cdda_buf + (cdda_pos * FRAME_SIZE), ahi_buf, bytes);
#else
				CopyMem((UBYTE *)cdda_buf + (cdda_pos * FRAME_SIZE), ahi_buf, bytes);
#endif
				ahi_io->ahir_Std.io_Command = CMD_WRITE;
				ahi_io->ahir_Std.io_Offset = 0;
				ahi_io->ahir_Std.io_Data = ahi_buf;
				ahi_io->ahir_Std.io_Length = bytes;
				ahi_io->ahir_Type = AHIST_S16S;
				ahi_io->ahir_Frequency = 44100;
				ahi_io->ahir_Volume = volume << 10;
				ahi_io->ahir_Position = 0x8000;
				ahi_io->ahir_Link = join;
				SendIO((struct IORequest *)ahi_io);
				if (join) {
					WaitIO((struct IORequest *)join);
				}
				join = ahi_io;
				SWAP_PTR(ahi_io, ahi_io2);
				SWAP_PTR(ahi_buf, ahi_buf2);
				addr += frames;
				cdda_pos += frames;
				cdda_frames -= frames;
				cd_toc.CurrentAddr = addr;
				if (addr == endaddr) {
					cd_toc.IsPlaying = FALSE;
				} else if (addr >= cd_toc.Tracks[track-1].EndAddr) {
					while (addr >= cd_toc.Tracks[track-1].EndAddr) {
						cd_toc.CurrentTrack = ++track;
					}
					UpdateGUI(wnd, gad, &cd_toc);
				}
			} else {
				cd_toc.IsPlaying = FALSE;
			}
			if (!cd_toc.IsPlaying) {
				if (cdda_io_busy) {
					AbortIO((struct IORequest *)cdda_io);
					WaitIO((struct IORequest *)cdda_io);
					cdda_io_busy = FALSE;
				}
				cd_toc.IsPaused = FALSE;
				cd_toc.CurrentTrack = track = cd_toc.FirstTrack;
				cdda_addr = cd_toc.CurrentAddr = cd_toc.Tracks[track-1].StartAddr;
				cdda_frames = 0;
				UpdateGUI(wnd, gad, &cd_toc);
			}
		}
	}
	
error:
	if (ahi_io && ahi_io->ahir_Std.io_Device) {
		CloseDevice((struct IORequest *)ahi_io);
	}
	if (cdda_io && cdda_io->io_Device && cdda_io_busy) {
		AbortIO((struct IORequest *)cdda_io);
		WaitIO((struct IORequest *)cdda_io);
	}
	DeleteIORequest(cdda_io);
	FreeVec(ahi_buf);
	FreeVec(ahi_buf2);
	FreeVec(cdda_buf);
	FreeVec(cdda_buf2);
	RemDiskChangeHandler(cd_change);
	FreeSignal(cd_signal);
	FreeCDDevice(cd_io);
	MUI_DisposeObject(app);
	SeekBar_DeleteClass(SeekBarClass);
	FreeDiskObject(icon);
	DeleteIORequest(ahi_io);
	DeleteIORequest(ahi_io2);
	DeleteMsgPort(ahi_mp);

	return 0;
}

void UpdateGUI (Object *wnd, Object **gad, struct CDTOC *cd_toc) {
	ULONG track, trackaddr;
	
	track = cd_toc->CurrentTrack;
	if (track) {
		trackaddr = cd_toc->CurrentAddr - cd_toc->Tracks[track-1].StartAddr;
	} else {
		trackaddr = 0;
	}
	if (cd_toc->IsPlaying && track && cd_toc->Tracks[track-1].IsAudio) {
		snprintf(status_disp, sizeof(status_disp), "Track %02d - %02d:%02d",
			track, (trackaddr / 75) / 60, (trackaddr / 75) % 60);
		set(gad[GID_STATUSDISP], MUIA_Text_Contents, status_disp);
		SetAttrs(gad[GID_POSITION],
			MUIA_Disabled,		FALSE,
			MUIA_Slider_Min,	cd_toc->Tracks[track-1].StartAddr,
			MUIA_Slider_Max,	cd_toc->Tracks[track-1].EndAddr,
			MUIA_Slider_Level,	cd_toc->CurrentAddr,
			TAG_END);
	} else {
		snprintf(status_disp, sizeof(status_disp), track ? "Track %02d - STOPPED" :
			cd_in_drive ? "No track selected" : "No disc in drive", track);
		set(gad[GID_STATUSDISP], MUIA_Text_Contents, status_disp);
		SetAttrs(gad[GID_POSITION],
			MUIA_Disabled,		TRUE,
			MUIA_Slider_Level,	0,
			TAG_END);
	}
	
	for (track = 0; track < MAX_TRACKS; track++) {
		SetAttrs(gad[GID_TRACK01 + track],
			MUIA_Disabled, 		cd_toc->Tracks[track].IsAudio ? FALSE : TRUE,
			MUIA_Text_PreParse,	cd_toc->CurrentTrack == (track + 1) ? "\33c\33b" : "\33c",
			TAG_END);
	}

	set(gad[GID_EJECT], MUIA_Disabled, FALSE);
	set(gad[GID_STOP], MUIA_Disabled, !cd_toc->IsPlaying && !cd_toc->IsPaused);
	set(gad[GID_PAUSE], MUIA_Disabled, !cd_toc->IsPlaying || cd_toc->IsPaused);
	set(gad[GID_PREV], MUIA_Disabled, !cd_toc->CurrentTrack || (cd_toc->CurrentTrack <= cd_toc->FirstTrack));
	set(gad[GID_PLAY], MUIA_Disabled, !cd_toc->CurrentTrack || (cd_toc->IsPlaying && !cd_toc->IsPaused));
	set(gad[GID_NEXT], MUIA_Disabled, !cd_toc->CurrentTrack || (cd_toc->CurrentTrack >= cd_toc->LastTrack));
}

Object *CreateTable (LONG rows, LONG cols, Object **buttons) {
	struct TagItem *tags, *tag;
	Object *table, *column;
	LONG i;
	
	tags = AllocVec(sizeof(struct TagItem) * (cols + 2), MEMF_CLEAR);
	if (!tags) {
		return NULL;
	}
	
	tag = tags;
	tag->ti_Tag = MUIA_Group_Horiz;
	tag->ti_Data = TRUE;
	tag++;
	
	for (i = 0; i < cols; i++) {
		column = CreateColumn(rows, cols, i, buttons);
		tag->ti_Tag = MUIA_Group_Child;
		tag->ti_Data = (IPTR)column;
		tag++;
	}
	
	tag->ti_Tag = TAG_END;
	table = MUI_NewObjectA(MUIC_Group, tags);
	FreeVec(tags);
	
	return table;
}

Object *CreateColumn (LONG rows, LONG cols, LONG i, Object **buttons) {
	static char text[32][4];
	struct TagItem *tags, *tag;
	Object *column, *button;
	LONG j;
	
	tags = AllocVec(sizeof(struct TagItem) * (cols + 2), MEMF_CLEAR);
	if (!tags) {
		return NULL;
	}
	
	tag = tags;
	tag->ti_Tag = MUIA_Group_Horiz;
	tag->ti_Data = FALSE;
	tag++;
	
	for (j = 0; j < rows; j++, i += cols) {
		snprintf(text[i], 4, "%d", i+1);
		button = TextObject,
			ButtonFrame,
			MUIA_InputMode,		MUIV_InputMode_RelVerify,
			MUIA_Text_Contents,	text[i],
			MUIA_Text_PreParse,	"\33c",
		End;
		if (buttons) buttons[i] = button;
		if (button) set(button, MUIA_Disabled, TRUE);
		tag->ti_Tag = MUIA_Group_Child;
		tag->ti_Data = (IPTR)button;
		tag++;
	}
	
	tag->ti_Tag = TAG_END;
	column = MUI_NewObjectA(MUIC_Group, tags);
	FreeVec(tags);
	
	return column;
}

HOOKPROTO(SeekFunc, IPTR, APTR unused, IPTR *params) {
	Object **gad = (Object **)params[1];
	struct CDTOC *cd_toc = (struct CDTOC *)params[2];
	if (cd_toc->IsPlaying) {
		ULONG new_addr;
		new_addr = (ULONG)XGET(gad[GID_POSITION], MUIA_Slider_Level);
		if (new_addr != cd_toc->CurrentAddr) {
			if (cdda_io_busy) {
				AbortIO((struct IORequest *)cdda_io);
				WaitIO((struct IORequest *)cdda_io);
				cdda_io_busy = FALSE;
			}
			cdda_frames = 0;
			cdda_addr = cd_toc->CurrentAddr = new_addr;
		}
	}
	return 0;
}

HOOKPROTO(EjectFunc, IPTR, APTR unused, IPTR *params) {
	cd_io->io_Command = TD_EJECT;
	cd_io->io_Length = load_eject;
	DoIO((struct IORequest *)cd_io);
	load_eject ^= 1;
	return 0;
}

HOOKPROTO(StopFunc, IPTR, APTR unused, IPTR *params) {
	Object *wnd = (Object *)params[0];
	Object **gad = (Object **)params[1];
	struct CDTOC *cd_toc = (struct CDTOC *)params[2];
	if (cd_toc->IsPlaying) {
		if (cdda_io_busy) {
			AbortIO((struct IORequest *)cdda_io);
			WaitIO((struct IORequest *)cdda_io);
			cdda_io_busy = FALSE;
		}
		cdda_frames = 0;
		cd_toc->IsPlaying = FALSE;
		cd_toc->IsPaused = FALSE;
		cdda_addr = cd_toc->CurrentAddr = cd_toc->Tracks[cd_toc->CurrentTrack-1].StartAddr;
		UpdateGUI(wnd, gad, cd_toc);
	}
	return 0;
}

HOOKPROTO(PauseFunc, IPTR, APTR unused, IPTR *params) {
	Object *wnd = (Object *)params[0];
	Object **gad = (Object **)params[1];
	struct CDTOC *cd_toc = (struct CDTOC *)params[2];
	if (cd_toc->IsPlaying && !cd_toc->IsPaused) {
		cd_toc->IsPaused = TRUE;
		UpdateGUI(wnd, gad, cd_toc);
	}
	return 0;
}

HOOKPROTO(PrevFunc, IPTR, APTR unused, IPTR *params) {
	Object *wnd = (Object *)params[0];
	Object **gad = (Object **)params[1];
	struct CDTOC *cd_toc = (struct CDTOC *)params[2];
	if (cd_toc->CurrentTrack > cd_toc->FirstTrack) {
		if (cdda_io_busy) {
			AbortIO((struct IORequest *)cdda_io);
			WaitIO((struct IORequest *)cdda_io);
			cdda_io_busy = FALSE;
		}
		cdda_frames = 0;
		cd_toc->CurrentTrack--;
		cdda_addr = cd_toc->CurrentAddr = cd_toc->Tracks[cd_toc->CurrentTrack-1].StartAddr;
		cd_toc->IsPaused = FALSE;
		UpdateGUI(wnd, gad, cd_toc);
	}
	return 0;
}

HOOKPROTO(PlayFunc, IPTR, APTR unused, IPTR *params) {
	Object *wnd = (Object *)params[0];
	Object **gad = (Object **)params[1];
	struct CDTOC *cd_toc = (struct CDTOC *)params[2];
	if (!cd_toc->IsPlaying || cd_toc->IsPaused) {
		cd_toc->IsPlaying = TRUE;
		cd_toc->IsPaused = FALSE;
		UpdateGUI(wnd, gad, cd_toc);
	}
	return 0;
}

HOOKPROTO(NextFunc, IPTR, APTR unused, IPTR *params) {
	Object *wnd = (Object *)params[0];
	Object **gad = (Object **)params[1];
	struct CDTOC *cd_toc = (struct CDTOC *)params[2];
	if (cd_toc->CurrentTrack < cd_toc->LastTrack) {
		if (cdda_io_busy) {
			AbortIO((struct IORequest *)cdda_io);
			WaitIO((struct IORequest *)cdda_io);
			cdda_io_busy = FALSE;
		}
		cdda_frames = 0;
		cd_toc->CurrentTrack++;
		cdda_addr = cd_toc->CurrentAddr = cd_toc->Tracks[cd_toc->CurrentTrack-1].StartAddr;
		cd_toc->IsPaused = FALSE;
		UpdateGUI(wnd, gad, cd_toc);
	}
	return 0;
}

HOOKPROTO(TrackFunc, IPTR, APTR unused, IPTR *params) {
	Object *wnd = (Object *)params[0];
	Object **gad = (Object **)params[1];
	struct CDTOC *cd_toc = (struct CDTOC *)params[2];
	ULONG track = (ULONG)params[3];
	if (cd_toc->Tracks[track].IsAudio) {
		if (cdda_io_busy) {
			AbortIO((struct IORequest *)cdda_io);
			WaitIO((struct IORequest *)cdda_io);
			cdda_io_busy = FALSE;
		}
		cdda_frames = 0;
		cd_toc->CurrentTrack = track + 1;
		cdda_addr = cd_toc->CurrentAddr = cd_toc->Tracks[cd_toc->CurrentTrack-1].StartAddr;
		cd_toc->IsPlaying = TRUE;
		cd_toc->IsPaused = FALSE;
		UpdateGUI(wnd, gad, cd_toc);
	}
	return 0;
}
