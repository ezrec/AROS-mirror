#ifndef SOUND_H
#define SOUND_H

#ifndef NO_SOUND

#include <devices/ahi.h>

#define SOUND_VERSION 2

#define SOUNDCMD_PLAY 1
#define SOUNDCMD_STOP 2
#define SOUNDCMD_MOVE 3
#define SOUNDCMD_ACTIVE 4
#define SOUNDCMD_INACTIVE 5

struct SoundMsg
{
	struct Message msg;
	char *filename;
	WORD version;
	WORD command;
	ULONG handle;
	ULONG rc;
	WORD windowpan;
	WORD screensize;
};

struct RASoundPrefs
{
	ULONG	AHIModeID;
	ULONG	MixFreq;
};

struct SampleUserData
{
	struct Task 		*task;
	BYTE					sampleplay;
	BYTE					sampleloop;
	struct SoundNode	*samplenode;
};

void InitSound(void);
void CleanupSound(void);
ULONG __saveds SoundFunc(REG(a0,struct Hook,*hook),REG(a2,struct AHIAudioCtrl,*actrl),
		REG(a1,struct AHISoundMessage,*chan));
ULONG SendSoundMsg(char *portname,WORD command,char *filename,ULONG handle,struct ReqNode *reqnode);
BOOL RALoadSample(void);
void RAUnloadSample(void);
void RAAllocateSound(void);
void RAFreeSound(void);
BOOL OpenAHI(void);
void CloseAHI(void);
void SoundTask(void);

#endif

#endif

