#include <exec/ports.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/notify.h>
#include <devices/ahi.h>
#include <utility/hooks.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <datatypes/datatypes.h>
#include <datatypes/soundclass.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#else
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/ahi.h>
#include <proto/datatypes.h>
#endif

#include <clib/alib_protos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "global.h"
#include "reqlist.h"
#include "sound.h"
#include "sample.h"
#include "misc.h"
/*J*/
#define SIGF_ROGER SIGBREAKF_CTRL_D
#define RA_DEFAULT_SOUNDPORT "RAAHISound"
#define APREFS_FILE 	"ENV:RAAHISound.prefs"
#define APREFS_FILE2	"ENVARC:RAAHISound.prefs"

static char *TASKNAME = "ReqAttack AHISound HQ";

static struct MsgPort soundport;
static struct SoundNode soundnode;
static struct Process *soundtask;
static struct Task *maintask;
static struct RASoundPrefs soundprefs;
static struct Hook soundfunchook;
static ULONG soundmask,samplefuncmask;
static BOOL soundtaskok,portadded;
static char act_filename[1024];
static struct Task *ourtask;
BYTE samplefuncsig;
D(BYTE callcount = 0;);

struct Library		*AHIBase=FALSE;
struct MsgPort		*AHImp=NULL;
struct AHIRequest	*AHIio=NULL;
BYTE					AHIDevice=-1;

static ULONG	handle=1,loopcount=0;
struct AHIAudioCtrl *audioctrl=0;
static struct AHISampleInfo sampleinfo;

ULONG command,mrc,mhan;

extern ULONG HookEntry();

static struct NotifyRequest notify;

void InitSound(void)
{
	ULONG	file;

	if (file = Open(APREFS_FILE2,MODE_OLDFILE))
	{
		Read(file,&soundprefs,sizeof(struct RASoundPrefs));
		Close(file);
	} else {
		soundprefs.AHIModeID = AHI_DEFAULT_ID;
		soundprefs.MixFreq = AHI_DEFAULT_FREQ;
	}

	maintask = FindTask(0);

	CacheClearU();

	if (!(soundtask = CreateNewProcTags(NP_Entry,(LONG)SoundTask,
												  NP_StackSize,16384,
												  NP_Priority,0,
												  NP_WindowPtr,-1,
												  NP_Name,(LONG)TASKNAME,
												  TAG_DONE)))
	{
		Cleanup("Can't create Sound Process!");
	}

	Wait(SIGF_ROGER);

	if (!soundtaskok) Cleanup("Initialization of Sound Process failed!");

}

void CleanupSound(void)
{
	if (soundtask)
	{
		Signal(&soundtask->pr_Task,SIGBREAKF_CTRL_C);
		Wait(SIGF_ROGER);
	}

}

void SoundTask(void)
{
	ULONG sigs,mpan,screensize,file;
	struct Process *pr;
	struct Message *smsg;
	struct MsgPort *port;
	struct SoundMsg *msg;

	pr = (struct Process *)FindTask(0);
	pr->pr_WindowPtr = (APTR)-1;	/*don't open requesters!*/

	ourtask = FindTask(0);

	Forbid();
	if ((port = FindPort(RA_DEFAULT_SOUNDPORT)))
	{
		Permit();
		soundtaskok = FALSE;
	}
	else
	{
		Permit();
		soundport.mp_Node.ln_Type = NT_MSGPORT;
		soundport.mp_Node.ln_Pri = 0;
		soundport.mp_Node.ln_Name = RA_DEFAULT_SOUNDPORT;
		soundport.mp_Flags = PA_SIGNAL;
		soundport.mp_SigBit = AllocSignal(-1);
		soundport.mp_SigTask = FindTask(0);
		NewList(&soundport.mp_MsgList);

		AddPort(&soundport);
		portadded = TRUE;

		soundmask = 1L << soundport.mp_SigBit;

		soundtaskok = TRUE;
	}

	samplefuncsig = AllocSignal(-1);
	samplefuncmask = 1L << samplefuncsig;

	soundfunchook.h_Entry = (HOOKFUNC)HookEntry;
	soundfunchook.h_SubEntry = (HOOKFUNC)SoundFunc;

	notify.nr_Name = APREFS_FILE;
	notify.nr_Flags = NRF_SEND_MESSAGE | NRF_NOTIFY_INITIAL;
	notify.nr_stuff.nr_Msg.nr_Port = &soundport;
	StartNotify(&notify);

	Signal(maintask,SIGF_ROGER);

	while(1)
	{
		sigs = Wait(SIGBREAKF_CTRL_C | soundmask | samplefuncmask);

		D(bug("RAHI: got signals %lx\n",sigs));

		if (sigs & SIGBREAKF_CTRL_C) break;

		if (sigs & samplefuncmask)
		{
			RAFreeSound();
		}

		if (sigs & soundmask)
		{
			while (msg = (struct SoundMsg *)GetMsg(&soundport))
			{
				if (msg->version == NOTIFY_CODE)
				{
					if (file = Open(APREFS_FILE,MODE_OLDFILE))
					{
						Read(file,&soundprefs,sizeof(struct RASoundPrefs));
						Close(file);
					}
					ReplyMsg(&msg->msg);

				} else {

					if(msg->filename) strcpy(act_filename,msg->filename);

					mrc = msg->rc; command = msg->command;
					mpan = msg->windowpan; screensize = msg->screensize;
					mhan = msg->handle;

					if (msg->command == SOUNDCMD_PLAY) msg->rc=handle+1;

					switch(command)
					{
					case SOUNDCMD_PLAY:
								D(bug("RAHI: processing PLAY command...\n"));
								RAFreeSound();
								D(bug("RAHI: removing old PLAY requests...\n"));
								if (RALoadSample())
								{
									D(bug("RAHI: sample loaded\n"));
									RAAllocateSound();
									D(bug("RAHI: audio allocated\n"));
									if (audioctrl)
									{
										soundnode.pan = (((0x8000L * mpan)/screensize)+0x4000L);
										if (soundnode.pan > 0xFFFFL) soundnode.pan = 0x4000L;
										soundnode.vol = 0xFFFFL;

										sampleinfo.ahisi_Type = soundnode.sampletype;
										sampleinfo.ahisi_Address = soundnode.samples;
										sampleinfo.ahisi_Length = soundnode.samplesize;
										loopcount = 0;
										soundnode.samplehandle = handle;

										if (AHI_LoadSound(0,AHIST_SAMPLE,&sampleinfo,audioctrl) == AHIE_OK)
										{
											D(bug("RAHI: AHI_LoadSound OK\n"));
											if (soundnode.samplelooplen)
											{
												AHI_Play(audioctrl,AHIP_BeginChannel,0,AHIP_Freq,soundnode.samplefreq,AHIP_Vol,soundnode.vol,AHIP_Pan,soundnode.pan,AHIP_Sound,0,AHIP_EndChannel,0,TAG_DONE);
											}else{
												AHI_Play(audioctrl,AHIP_BeginChannel,0,AHIP_Freq,soundnode.samplefreq,AHIP_Vol,soundnode.vol,AHIP_Pan,soundnode.pan,AHIP_Sound,0,AHIP_LoopSound,AHI_NOSOUND,AHIP_LoopVol,0,AHIP_EndChannel,0,TAG_DONE);
											}
											AHI_ControlAudio(audioctrl,AHIC_Play,TRUE,TAG_DONE);
											D(bug("RAHI: AHIC_PLAY set to TRUE\n"));
										}
									} else {	/*couldn't allocate audio to play!*/
										RAFreeSound();
									}
								}
								D(bug("RAHI: PLAY processing done\n"));
							break;
						case SOUNDCMD_STOP:
								D(bug("RAHI: STOP command\n"));
								if (mhan == handle)
								{
									if (soundnode.samplelooplen) RAFreeSound();
								}
							break;
						case SOUNDCMD_MOVE:
								if (mhan == handle)
								{
										soundnode.pan = (((0x8000L * mpan)/screensize)+0x4000L);
										if (soundnode.pan > 0xFFFFL) soundnode.pan = 0x4000L;
										if (audioctrl) AHI_SetVol(0,soundnode.vol,soundnode.pan,audioctrl,AHISF_IMM);
								}
							break;
						case SOUNDCMD_ACTIVE:
								D(bug("RAHI: ACTIVE command\n"));
								if (mhan == handle)
								{
										soundnode.vol = 0xFFFFL;
										if (audioctrl) AHI_SetVol(0,soundnode.vol,soundnode.pan,audioctrl,AHISF_IMM);
								}
							break;
						case SOUNDCMD_INACTIVE:
								if (mhan == handle)
								{
										soundnode.vol = 0x8000L;
										if (audioctrl) AHI_SetVol(0,soundnode.vol,soundnode.pan,audioctrl,AHISF_IMM);
								}
							break;
					}
				ReplyMsg(&msg->msg);
				D(bug("RAHI: message replied to sender\n"));
				}
				D(bug("RAHI: hunting for queued messages\n"));
			}
		}
		D(bug("RAHI: looping again...\n"));
	}
/*exit*/

	EndNotify(&notify);

	RAFreeSound();

	if (samplefuncsig != -1) FreeSignal(samplefuncsig);

	if (portadded)
	{
		Forbid();
		while ((smsg = GetMsg(&soundport)))
		{
			ReplyMsg(smsg);
		}
		RemPort(&soundport);
		Permit();

		FreeSignal(soundport.mp_SigBit);
	}

	Signal(maintask,SIGF_ROGER);
}

void RAAllocateSound(void)
{
	if (OpenAHI())
	{
	//Delay(5);
	if (audioctrl==0) audioctrl = AHI_AllocAudio(AHIA_AudioID,soundprefs.AHIModeID,
											AHIA_MixFreq,soundprefs.MixFreq,AHIA_Channels,1,
											AHIA_Sounds,1,AHIA_SoundFunc,&soundfunchook,TAG_DONE);
	}else{
		RAFreeSound();
	}
};

void RAFreeSound(void)
{
	handle++; /*JUST IN CASE ;)*/

	if (audioctrl)
	{
		D(bug("RAHI:RAFREE: stoping and freeing AHI resources...\n"));
		AHI_ControlAudio(audioctrl,AHIC_Play,FALSE,TAG_DONE);
		//Delay(5);
		AHI_FreeAudio(audioctrl);
		audioctrl = 0;
	}
	CloseAHI();
	RAUnloadSample();

};

/*THE SOUND FUNCTION!*/

ULONG __saveds SoundFunc(REG(a0,struct Hook,*hook),
		REG(a2,struct AHIAudioCtrl,*actrl),
		REG(a1,struct AHISoundMessage,*chan))
{

if (soundnode.samplehandle == handle);
{
	if (loopcount > 0)
	{
		if (!(soundnode.samplelooplen)) /*no looping!*/
		{
			Signal(ourtask,samplefuncmask);//free the sound resources...
		}
	} else {	/*notify start sample!*/
		loopcount++;
	}
}

	return 0;

}

BOOL RALoadSample(void)
{
	ULONG *dto,*sampledata,samplesize;
	char *filename;
	BOOL rc,loop;
	static struct DTMethod dtm;
	static struct VoiceHeader *vhdr;

	RAUnloadSample();
	rc = FALSE;

	soundnode.vol = 0x10000L;

	if (act_filename[0] != '!')
	{
		filename = &act_filename[0]; loop = FALSE;
	} else {
		filename = &act_filename[1];
		loop = TRUE;
	}
	D(bug("RAHI:LOAD: creating DT object from %s\n",filename));
	if (dto = (ULONG *)NewDTObject(filename,DTA_GroupID,GID_SOUND,TAG_DONE,NULL))
	{
		D(bug("RAHI:LOAD: loaded %lx\n",dto));
		dtm.dtm_Label=0;
		dtm.dtm_Command=0;
		dtm.dtm_Method = GM_LAYOUT;
		if (DoMethodA((Object *)dto,(Msg)&dtm))
		{
			D(bug("RAHI:LOAD: layouting...\n"));
			GetDTAttrs((Object *)dto,SDTA_VoiceHeader,&vhdr,SDTA_Sample,&sampledata,SDTA_SampleLength,&samplesize,TAG_DONE);
			soundnode.sampledata = dto;
			D(bug("RAHI:LOAD: sample data is: %lx\n",sampledata));
			if (!(sampledata))
			{
				GetDTAttrs((Object *)dto,SDTA_LeftSample,&sampledata,TAG_DONE);
				if (!(sampledata))
				{
					GetDTAttrs((Object *)dto,SDTA_RightSample,&sampledata,TAG_DONE);
					if (!(sampledata))
					{
						DisposeDTObject((Object *)dto);
						return FALSE;
					}
				}
			}
			soundnode.samples = sampledata;
			soundnode.samplesize = vhdr->vh_OneShotHiSamples;
			soundnode.sampletype = AHIST_M8S;
			soundnode.samplefreq = vhdr->vh_SamplesPerSec;
			if (loop)
			{
				soundnode.samplelooplen = soundnode.samplesize;
			} else {
				soundnode.samplelooplen = 0;
			}
			soundnode.sampleloopstart = 0;
			rc = TRUE;
			D(bug("RAHI:LOAD: sample set up\n"));
		}
	}

	return rc;
}

void RAUnloadSample(void)
{
	if (soundnode.sampledata) DisposeDTObject((Object *)soundnode.sampledata);
	soundnode.sampledata=0;
}

BOOL OpenAHI(void)
{
	if (AHImp) CloseAHI();
	if (AHIio) CloseAHI();
	if (AHIDevice == 0) CloseAHI();
// now were protected against opening AHI without closing previously opened...

	if(AHImp = CreateMsgPort())
	{
		if(AHIio = (struct AHIRequest *) CreateIORequest(
				AHImp, sizeof(struct AHIRequest)))
		{
			AHIio->ahir_Version = 4;
			AHIDevice = OpenDevice(AHINAME,AHI_NO_UNIT,
				(struct IORequest *) AHIio, NULL);
			if (AHIDevice == 0)
			{
				AHIBase = (struct Library *) AHIio->ahir_Std.io_Device;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CloseAHI(void)
{
	if(AHIDevice == 0)
	{
		CloseDevice((struct IORequest *) AHIio);
		AHIDevice = -1;
		AHIBase = 0;
	}

	if(AHIio)
	{
		DeleteIORequest((struct IORequest *) AHIio);
		AHIio = NULL;
	}

	if(AHImp)
	{
		DeleteMsgPort(AHImp);
		AHImp = NULL;
	}
}

/*jend*/

ULONG SendSoundMsg(char *portname,WORD command,char *filename,ULONG handle,struct ReqNode *reqnode)
{
	struct SoundMsg msg;
	struct MsgPort replyport;
	struct MsgPort *soundport;

	ObtainSemaphore(&scmdsem);

	D(callcount++;);
	D(bug("Sound:%d: semaphore obtained\n",callcount));

	replyport.mp_Node.ln_Type = NT_MSGPORT;
	replyport.mp_Flags = PA_SIGNAL;
	replyport.mp_SigBit = SIGB_SINGLE;
	replyport.mp_SigTask = FindTask(0);
	NewList(&replyport.mp_MsgList);

	SetSignal(0,SIGF_SINGLE);

	msg.msg.mn_ReplyPort = &replyport;
	msg.msg.mn_Length = sizeof(msg);

	msg.version = SOUND_VERSION;
	msg.command = command;
	msg.filename = filename;
	msg.handle = handle;
	msg.rc = 0;
	msg.windowpan = (reqnode->win->LeftEdge + (reqnode->win->Width / 2) + 1);
	msg.screensize = reqnode->scr->Width;

	D(bug("Sound:%d: message filled\n",callcount));

	Forbid();
	if ((soundport = FindPort(portname)))
	{
		D(bug("Sound:%d: port found - putting message\n",callcount));
		PutMsg(soundport,&msg.msg);
		D(bug("Sound:%d: waiting for reply\n",callcount));
		WaitPort(&replyport);
	}
	Permit();

	D(bug("Sound:%d: server replied\n",callcount));

	ReleaseSemaphore(&scmdsem);

	return msg.rc;

}

