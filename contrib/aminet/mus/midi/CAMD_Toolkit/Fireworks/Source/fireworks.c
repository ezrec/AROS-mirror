
/*-------------------------------------------------*/
/* fireworks.c - graphical MIDI note visualisation */
/*          ® 1998 by Christian Buchner            */
/*-------------------------------------------------*/

/* Very hackish port to AROS by Kjetil S. Mathuessen 2001. */



#include "fireworks.h"
#include "fireworks_protos.h"
#include "fwmodes.h"

#define PREF_VERSION 1


struct Task *MyTask;	/* this task */
BOOL WBMode = FALSE;	/* Workbench or not? */

                        /* for MIDI timestamping */
extern struct EClockVal stampeclock;

						/* Globals for Watchdog timer */
						/* (from timer.c) */

extern struct Interrupt VertBlank;
extern BOOL Watch;
extern ULONG IdleCount;



/* Function Prototypes */

LONG ShellInterface(void);
LONG WBInterface(struct Process *MyProc);

BOOL WindowLayout(struct Globals *glob, WORD *minw, WORD *minh, WORD *maxw, WORD *maxh);

LONG Fireworks(struct Prefs *pref);
void MainLoop(struct Globals *glob, struct Prefs *pref);

BOOL OpenGUI(struct Globals *glob, struct Prefs *pref);
void TitleWindow(struct Globals *glob, UBYTE *Text, ...);
BOOL SelectScreenMode(struct Globals *glob, struct Prefs *pref, struct ScreenModeRequester **srq);
void CloseGUI(struct Globals *glob, struct Prefs *pref);

void InitPrefs(struct Prefs *pref);
BOOL LoadPrefs(struct Prefs *pref);
BOOL SavePrefs(struct Globals *glob, struct Prefs *pref, UBYTE *EnvName);

void InitGlobals(struct Globals *glob);

void AsyncSelectMIDILink(struct Globals *glob, struct Prefs *pref);
void AsyncSelectScreenMode(struct Globals *glob, struct Prefs *pref);
void AsyncSelectImage(struct Globals *glob, struct Prefs *pref);
void AsyncSelectAndPlay(struct Globals *glob, struct Prefs *pref);

BOOL InitOrUpdateGraphics(struct Globals *glob, struct Prefs *pref);
void FreeGraphics(struct Globals *glob);
struct Screen *GetScreen(struct Globals *glob);

void LoadImage(struct Globals *glob, struct Prefs *pref);
BOOL SelectImage(struct Globals *glob, struct Prefs *pref, struct FileRequester **fr, UBYTE *filebuffer, ULONG MaxSize);

BOOL SelectMIDI(struct Globals *glob, struct Prefs *pref, struct FileRequester **fr, UBYTE *filebuffer, ULONG MaxSize);
void PlayMIDI(struct Globals *glob, struct Prefs *pref);
void StopMIDI(struct Globals *glob, struct Prefs *pref);



/* Palette and Sprite data structures */

ULONG ScreenPalette[] =
{
	((2L<<16) | 0),
	
	0x00000000, 0x00000000, 0x00000000,
	0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA,
	
	((0L<<16) | 0)
};


UWORD PenArray[] =
{
	1, // DETAILPEN
	0, // BLOCKPEN
	1, // TEXTPEN
	1, // SHINEPEN
	1, // SHADOWPEN
	1, // FILLPEN
	0, // FILLTEXTPEN
	0, // BACKGROUNDPEN
	1, // HIGHLIGHTTEXTPEN
	1, // BARDETAILPEN
	0, // BARBLOCKPEN
	0, // BARTRIMPEN
};


UBYTE PenColors[NUMPENS][3] =
{
	{0x00, 0x00, 0x00},			/* Black background */
	{0xff, 0xff, 0xff},			/* white sparks/glitter */
	
	{255,  64,  64},			/* Chan  1 */
	{255, 137,  64},			/* Chan  2 */
	{255, 207,  64},			/* Chan  3 */
	{230, 255,  64},			/* Chan  4 */
	{159, 255,  64},			/* Chan  5 */
	 {86, 255,  64},			/* Chan  6 */
	 {64, 255, 112},			/* Chan  7 */
	 {64, 255, 185},			/* Chan  8 */
	 {64, 255, 255},			/* Chan  9 */
	 {64, 185, 255},			/* Chan 10 */
	 {64, 112, 255},			/* Chan 11 */
	 {86,  64, 255},			/* Chan 12 */
	{159,  64, 255},			/* Chan 13 */
	{230,  64, 255},			/* Chan 14 */
	{255,  64, 207},			/* Chan 15 */
	{255,  64, 137},			/* Chan 16 */
};


CHIP UWORD WaitPointer[] =
{
	0x0000, 0x0000,

	0x0400, 0x07C0,
	0x0000, 0x07C0,
	0x0100, 0x0380,
	0x0000, 0x07E0,
	0x07C0, 0x1FF8,
	0x1FF0, 0x3FEC,
	0x3FF8, 0x7FDE,
	0x3FF8, 0x7FBE,
	0x7FFC, 0xFF7F,
	0x7EFC, 0xFFFF,
	0x7FFC, 0xFFFF,
	0x3FF8, 0x7FFE,
	0x3FF8, 0x7FFE,
	0x1FF0, 0x3FFC,
	0x07C0, 0x1FF8,
	0x0000, 0x07E0,

	0x0000, 0x0000,		/* reserved, must be NULL */
};

CHIP UWORD NoPointer[] =
{
	0x0000, 0x0000,

	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,

	0x0000, 0x0000,		/* reserved, must be NULL */
};


/* Array of random numbers (0 or 1) */

UBYTE randomarray33[256];


/* Some defines */

/* the opposite of noteon() */
#define noteoff(m) ( voicemsg(m,MS_NoteOff) || (voicemsg(m,MS_NoteOn) && (!(m)->mm_Data2)) )

#define offsetof(s, m)  (size_t)(&(((s *)0)->m))
#define MIN(a,b)    ((a) <= (b) ? (a) : (b))
#define elementsof(a)  ((sizeof(a) / sizeof(a[0])))


/* Intuition Menu structures */

enum
{
	Menu_Ignore1,
	Menu_Link,
	Menu_Hooks,
	Menu_Save,
	Menu_Ignore2,
	Menu_About,
	Menu_Ignore3,
	Menu_Quit,
	Menu_Ignore4,
	Menu_Kill,
	Menu_Load,
	Menu_Tile,
	Menu_Ignore5,
	Menu_Screenmode,
	Menu_Fullscreen,
	Menu_Titlebar,
	Menu_Mouse,
	Menu_Ignore6,
	Menu_Mode1,
	Menu_Mode2,
	Menu_Mode3,
	Menu_Ignore7,
	Menu_Sens1,
	Menu_Sens2,
	Menu_Sens3,
	Menu_Ignore8,
	Menu_Double,
	Menu_Sparks,
	Menu_Ignore9,
	Menu_Release,
	Menu_Ignore10,
	Menu_Play,
	Menu_Stop,
	Menu_Ignore11,
	Menu_Init1,
	Menu_Init2,
	Menu_Init3,
	Menu_Init4,
	Menu_Init5,
	Menu_Ignore12,
	Menu_Timestamp,
	Menu_Ignore13,
};


#define M1 (1<<0)
#define M2 (1<<1)
#define M3 (1<<2)

#define S1 (1<<4)
#define S2 (1<<5)
#define S3 (1<<6)

#define I1 (1<<5)
#define I2 (1<<6)
#define I3 (1<<7)
#define I4 (1<<8)
#define I5 (1<<9)

struct NewMenu FWNewMenu[]=
{
	/* nm_Type	nm_Label			nm_CommKey	nm_Flags			nm_MutualExclude	nm_UserData */
	{NM_TITLE,	"Project",			NULL,		0,					0,					(APTR)Menu_Ignore1},
	{NM_ITEM,	"MIDI Link...",		"L",		0,					0,					(APTR)Menu_Link},
	{NM_ITEM,	"ASL Hooks",		"A",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Hooks},
	{NM_ITEM,	"Save Config",		"C",		0,					0,					(APTR)Menu_Save},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore2},
	{NM_ITEM,	"About",			"?",		0,					0,					(APTR)Menu_About},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore3},
	{NM_ITEM,	"Quit",				"Q",		0,					0,					(APTR)Menu_Quit},
	{NM_TITLE,	"Window",			NULL,		0,					0,					(APTR)Menu_Ignore4},
	{NM_ITEM,	"Remove Backdrop",	"R",		0,					0,					(APTR)Menu_Kill},
	{NM_ITEM,	"Load Backdrop...",	"B",		0,					0,					(APTR)Menu_Load},
	{NM_ITEM,	"Tile Backdrop",	"T",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Tile},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore5},
	{NM_ITEM,	"Set Screenmode...","M",		0,					0,					(APTR)Menu_Screenmode},
	{NM_ITEM,	"Full Screen",		"F",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Fullscreen},
	{NM_ITEM,	"Hide Titlebar",	"H",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Titlebar},
	{NM_ITEM,	"Hide Mouseptr",	"I",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Mouse},
	{NM_TITLE,	"Rendering",		NULL,		0,					0,					(APTR)Menu_Ignore6},
	{NM_ITEM,	"Laser Mode",		"1",		CHECKIT,			(   M2|M3),			(APTR)Menu_Mode1},
	{NM_ITEM,	"Pixel Mode",		"2",		CHECKIT,			(M1|   M3),			(APTR)Menu_Mode2},
	{NM_ITEM,	"Fountain Mode",	"3",		CHECKIT,			(M1|M2   ),			(APTR)Menu_Mode3},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore7},
	{NM_ITEM,	"Sensitivity Max",	"X",		CHECKIT,			(   S2|S3),			(APTR)Menu_Sens1},
	{NM_ITEM,	"Sensitivity Med",	"Y",		CHECKIT,			(S1|   S3),			(APTR)Menu_Sens2},
	{NM_ITEM,	"Sensitivity Low",	"Z",		CHECKIT,			(S1|S2   ),			(APTR)Menu_Sens3},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore8},
	{NM_ITEM,	"Double Strength",	"D",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Double},
	{NM_ITEM,	"Sparks",			"+",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Sparks},
	{NM_TITLE,	"MIDI",				NULL,		0,					0,					(APTR)Menu_Ignore9},
	{NM_ITEM,	"Release Notes",	"N",		0,					0,					(APTR)Menu_Release},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore10},
	{NM_ITEM,	"Play MIDI file...","P",		0,					0,					(APTR)Menu_Play},
	{NM_ITEM,	"Stop playing",		"S",		0,					0,					(APTR)Menu_Stop},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore11},
	{NM_ITEM,	"Normal Init",		NULL,		CHECKIT,			(   I2|I3|I4|I5),	(APTR)Menu_Init1},
	{NM_ITEM,	"GM Reset",			NULL,		CHECKIT,			(I1|   I3|I4|I5),	(APTR)Menu_Init2},
	{NM_ITEM,	"GS Reset",			NULL,		CHECKIT,			(I1|I2|   I4|I5),	(APTR)Menu_Init3},
	{NM_ITEM,	"XG Reset",			NULL,		CHECKIT,			(I1|I2|I3   |I5),	(APTR)Menu_Init4},
	{NM_ITEM,	"MT32 Emu",			NULL,		CHECKIT,			(I1|I2|I3|I4   ),	(APTR)Menu_Init5},
	{NM_ITEM,	NM_BARLABEL,		NULL,		0,					0,					(APTR)Menu_Ignore12},
	{NM_ITEM,	"Timestamping",		"E",		CHECKIT|MENUTOGGLE,	0,					(APTR)Menu_Timestamp},
	{NM_END,		NULL,				NULL,		0,					0,					(APTR)Menu_Ignore13},
};




/*--------------*/
/* Startup code */
/*--------------*/

SAVEDS int main(void)
{
	LONG ReturnCode;
	
	struct	Process *MyProc;
	
	MyProc = (struct Process*) FindTask(NULL);
	
	if (!MyProc->pr_CLI)
	{
		WBMode = TRUE;
		ReturnCode = WBInterface(MyProc);
	}
	else
	{
		ReturnCode = ShellInterface();
	}
	
	return(ReturnCode);
}



/*---------------------*/
/* Workbench Interface */
/*---------------------*/

LONG WBInterface(struct Process *MyProc)
{
	struct WBStartup *wbmsg;
	
	LONG ReturnCode = RETURN_ERROR;
	
	WaitPort(&MyProc->pr_MsgPort);
	
	if ((wbmsg = (struct WBStartup*)GetMsg(&MyProc->pr_MsgPort)))
	{
		if (OpenLibs())
		{
			BPTR OldDir;
			struct Prefs *pref;
			
			OldDir = CurrentDir(wbmsg->sm_ArgList->wa_Lock);
			
			if (!(pref = AllocVec(sizeof(struct Prefs),MEMF_ANY|MEMF_CLEAR)))
			{
				Message("No memory for prefs!",NULL);
			}
			else
			{
				InitPrefs(pref);
				LoadPrefs(pref);
				
				ReturnCode = Fireworks(pref);
				
				FreeVec(pref);
			}
			
			CurrentDir(OldDir);
			
			CloseLibs();
		}
		Forbid();
		ReplyMsg((struct Message*)wbmsg);
	}
	return(ReturnCode);
}



/*-----------------*/
/* Shell Interface */
/*-----------------*/

LONG  ShellInterface(void)
{
	LONG ReturnCode = RETURN_ERROR;
	BPTR lock;
	BPTR OldDir;
	
	if (OpenLibs())
	{
//		if (!(lock = Lock("PROGDIR:", SHARED_LOCK)))
		if (!(lock = Lock("", SHARED_LOCK)))
		{
			Message("Unable to locate program directory.",NULL);
		}
		else
		{
			/* CLI argument parsing */
			
			struct	ArgArray
			{
				UBYTE *aa_Link;
				ULONG *aa_Mode;
				ULONG *aa_Sensitivity;
				ULONG *aa_WinX;
				ULONG *aa_WinY;
				ULONG *aa_WinW;
				ULONG *aa_WinH;
				UBYTE *aa_Image;
				ULONG  aa_Tile;
				ULONG  aa_Fullscreen;
				ULONG  aa_Double;
				ULONG  aa_Sparks;
				ULONG  aa_GM;
				ULONG  aa_GS;
				ULONG  aa_XG;
				ULONG  aa_MT32;
				ULONG  aa_NoHooks;
				ULONG  aa_NoTS;
			} AA;
			
			static UBYTE	*Template = "L=LINK/K,M=MODE/K/N,S=SENSITIVITY/K/N,WX=WINX/K/N,WY=WINY/K/N,WW=WINW/K/N,WH=WINH/K/N,I=IMAGE/K,T=TILE/S,FS=FULLSCREEN/S,D=DOUBLE/S,SP=SPARKS/S,GM/S,GS/S,XG/S,MT32/S,NH=NOHOOKS/S,NTS=NOTIMESTAMPING/S";
			struct RDArgs *RDArgs;
			
			struct Prefs *pref;
			
			ReturnCode = RETURN_FAIL;
			
			OldDir = CurrentDir(lock);
			
			if (!(pref = AllocVec(sizeof(struct Prefs),MEMF_ANY|MEMF_CLEAR)))
			{
				Message("No memory for prefs!",NULL);
			}
			else
			{
				InitPrefs(pref);
				LoadPrefs(pref);
				
				memset(&AA, 0, sizeof(struct ArgArray));
				if ((RDArgs=ReadArgs(Template, (LONG *)&AA, 0)))
				{
					BOOL fault = FALSE;
					UWORD opts;
					
					if (AA.aa_Link)
					{
						strncpy(pref->Link, AA.aa_Link, sizeof(pref->Link));
						pref->Link[sizeof(pref->Link)-1] = 0;
					}
					if (AA.aa_Mode)
					{
						LONG mode = (*AA.aa_Mode) - 1;
						if (mode >=0 && mode < NUM_FWMODES) pref->FWMode = mode;
						else
							Message("Mode out of range! There are currently modes 1-%ld available.", NULL, NUM_FWMODES);
							fault = TRUE;
					}
					if (AA.aa_Sensitivity)
					{
						LONG sens = (*AA.aa_Sensitivity);
						switch(sens)
						{
							case 1: pref->Sensitivity=100; break;
							case 2: pref->Sensitivity= 75; break;
							case 3: pref->Sensitivity= 50; break;
							default:
								Message("Sensitivity out of range! Allowed range is 1-3.", NULL);
								fault = TRUE;
								break;
						}
					}
					if (AA.aa_WinX) pref->WinX= *AA.aa_WinX;
					if (AA.aa_WinY) pref->WinY= *AA.aa_WinY;
					if (AA.aa_WinW) pref->WinW= *AA.aa_WinW;
					if (AA.aa_WinH) pref->WinH= *AA.aa_WinH;
					if (AA.aa_Image)
					{
						strncpy(pref->Image, AA.aa_Image, sizeof(pref->Image));
						pref->Image[sizeof(pref->Image)-1] = 0;
					}
					if (AA.aa_Tile) pref->Flags |= PREFF_TILE;
					if (AA.aa_Fullscreen) pref->Flags |= PREFF_FULLSCREEN;
					if (AA.aa_Double) pref->Flags |= PREFF_DOUBLE;
					if (AA.aa_Sparks) pref->Flags |= PREFF_SPARKS;
					if (AA.aa_NoHooks) pref->Flags &= ~(PREFF_ASLHOOKS);
					if (AA.aa_NoTS) pref->Flags &= ~(PREFF_TIMESTAMP);
					
					opts = 0;
					if (AA.aa_GM  ) {opts++; pref->Flags |= PREFF_GM  ;};
					if (AA.aa_GS  ) {opts++; pref->Flags |= PREFF_GS  ;};
					if (AA.aa_XG  ) {opts++; pref->Flags |= PREFF_XG  ;};
					if (AA.aa_MT32) {opts++; pref->Flags |= PREFF_MT32;};
					if (opts>1)
					{
						Message("Please specify only one of GM, GS, XG and MT32.", NULL);
						fault = TRUE;
					}
					
					if (!fault)
					{
						ReturnCode = Fireworks(pref);
					}
					
					FreeArgs(RDArgs);
				}
				else
				{
					PrintFault(IoErr(),"Fireworks");
				}
				
				FreeVec(pref);
			}
			
			CurrentDir(OldDir);
			
			UnLock(lock);
		}
		CloseLibs();
	}

	return(ReturnCode);
}



/*-----------------------------*/
/* GUI and MIDI initialisation */
/*-----------------------------*/

LONG Fireworks(struct Prefs *pref)
{
	LONG ReturnCode = RETURN_FAIL;
	
	struct Globals *glob;
	
	BYTE OldPri;
	
	MyTask = FindTask(NULL);
	
	if (!(glob = AllocVec(sizeof(struct Globals), MEMF_ANY|MEMF_CLEAR)))
	{
		Message("No memory for globals!",NULL);
	}
	else
	{
		InitGlobals(glob);
		
		if (OpenGUI(glob,pref))
		{
			if (!((glob->NotePool = CreatePool(MEMF_ANY,8192,4096))))
			{
				Message("No memory for NotePool!",NULL);
			}
			else
			{
				if (!(glob->midi = CreateMidi(
					MIDI_Name, "Fireworks",
					MIDI_TimeStamp, &stampeclock.ev_lo,
					MIDI_RecvSignal, SIGBREAKB_CTRL_E,
					MIDI_MsgQueue,   2000,
					MIDI_ErrFilter, CMEF_All,
					TAG_DONE)))
				{
					Message("Cannot create MIDI port!",NULL);
				}
				else
				{
					if (!(glob->link = AddMidiLink(glob->midi, MLTYPE_Receiver,
						MLINK_Name, "Fireworks Link",
						MLINK_Location, pref->Link,
						MLINK_EventMask, CMF_Note|CMF_Mode,
						MLINK_Comment,  "Fireworks [Input]",
						TAG_DONE)))
					{
						Message("Cannot create link to MIDI interface '%s'",NULL,pref->Link);
					}
					else
					{
						TitleWindow(glob, "Fireworks [%s] [idle]", pref->Link);
						
						if (!(glob->treq = OpenTimer()))
						{
							
						}
						else
						{
							OldPri = MyTask->tc_Node.ln_Pri;
							
							MainLoop(glob, pref);
							
							WaitAsync(glob);
							
							ReturnCode = RETURN_OK;
							
							SetTaskPri(MyTask, OldPri);
							
							CloseTimer(glob->treq);
							glob->treq=NULL;
						}
						RemoveMidiLink(glob->link);
						glob->link = NULL;
					}

					DeleteMidi(glob->midi);
					glob->midi = NULL;
				}

				DeletePool(glob->NotePool);
				glob->NotePool = NULL;
			}
			CloseGUI(glob, pref);
		}
		if (glob->ImageFR)
		{
			FreeAslRequest(glob->ImageFR);
			glob->ImageFR = NULL;
		}
		if (glob->MIDIFR)
		{
			FreeAslRequest(glob->MIDIFR);
			glob->MIDIFR = NULL;
		}
		if (glob->ScreenModeRQ)
		{
			FreeAslRequest(glob->ScreenModeRQ);
			glob->ScreenModeRQ = NULL;
		}
		FreeVec(glob);
	}
	return(ReturnCode);
}



/*--------------------*/
/* Set Prefs defaults */
/*--------------------*/

void InitPrefs(struct Prefs *pref)
{
	memset(pref, 0, sizeof(struct Prefs));
	strcpy(pref->Header, "Fireworks");
	pref->Version = PREF_VERSION;
	strcpy(pref->Link,"debugdriver.out.0");
	pref->FWMode = ParabolicMode2;
	pref->Sensitivity = 100;
	pref->Flags = PREFF_BACKDROP | PREFF_ASLHOOKS | PREFF_TIMESTAMP;
	pref->WinX = 100;
	pref->WinY = 200;
	pref->WinW = 256;
	pref->WinH = 256;
	strcpy(pref->Image,"PROGDIR:Backdrop.Pic");
	pref->ScreenMode = -1L;
	pref->AutoScroll = FALSE;
}


/*------------*/
/* Load Prefs */
/*------------*/

BOOL LoadPrefs(struct Prefs *pref)
{
	BOOL Success = FALSE;
	UBYTE *name = "ENV:MIDI/Fireworks.prefs";
	BPTR file;
	
	if ((file = Open(name, MODE_OLDFILE)))
	{
		if (Read(file, pref, sizeof(struct Prefs)) != -1)
		{
			if (stricmp(pref->Header, "Fireworks"))
			{
				Message("'%s' is not a Fireworks preferences file. Using defaults.", NULL, name);
			}
			else
			{
				if (pref->Version != PREF_VERSION)
				{
					Message("'%s' preferences version is not supported. Using defaults.", NULL, name);
				}
				else
				{
					Success = TRUE;
				}
			}
		}
		if (!Success) InitPrefs(pref);
		
		Close(file);
	}
	return(Success);
}


/*------------*/
/* Save Prefs */
/*------------*/

BOOL SavePrefs(struct Globals *glob, struct Prefs *pref, UBYTE *EnvName)
{
	BOOL Success = FALSE;
	BPTR OldDir=NULL;
	BPTR EnvLock=NULL;
	BPTR MIDILock=NULL;
	BPTR FH;
	UBYTE *FileName = "Fireworks.prefs";
	UBYTE *MIDIName = "MIDI";
	
	pref->WinX = glob->Window->LeftEdge;
	pref->WinY = glob->Window->TopEdge;
	pref->WinW = glob->ww;
	pref->WinH = glob->wh;
	
	if ((EnvLock=Lock(EnvName,ACCESS_READ)))
	{
		OldDir=CurrentDir(EnvLock);
		
		if (!(MIDILock=Lock(MIDIName,ACCESS_READ)))
		{
			MIDILock=CreateDir(MIDIName);
		}
		if (MIDILock)
		{
			CurrentDir(MIDILock);
			
			Success=TRUE;
		}
	}
	
	if (Success)
	{
		Success=FALSE;
		
		if ((FH=Open(FileName,MODE_NEWFILE)))
		{
			if (Write(FH,pref,sizeof(struct Prefs))==sizeof(struct Prefs))
			{
				Success=TRUE;
			}
			Close(FH);
		}
	}
	
	if (OldDir)  CurrentDir(OldDir);
	if (MIDILock) UnLock(MIDILock);
	if (EnvLock) UnLock(EnvLock);
	
	return(Success);
}




/*--------------------*/
/* Initialize globals */
/*--------------------*/

void InitGlobals(struct Globals *glob)
{
	UWORD i;
	
	for (i=0; i<NUMPENS; i++) glob->PenArray[i] = -1L;
}



/*-------------------------------*/
/* Open the GUI (Screen, Window) */
/*-------------------------------*/

BOOL OpenGUI(struct Globals *glob, struct Prefs *pref)
{
	BOOL Success = FALSE;
	
	struct Screen *wbscr;
	
	WORD minw, minh;
	WORD maxw, maxh;
	
	UWORD mode, sens, init;
	
	glob->Screen = NULL;
	
	if (!(glob->AppPort = CreateMsgPort()))
	{
		Message("Failed to create AppMsg port.", NULL);
	}
	else
	{
		if (pref->Flags & PREFF_FULLSCREEN)
		{
			if (pref->ScreenMode == -1L)
			{
				if (!SelectScreenMode(glob, pref, &glob->ScreenModeRQ))
				{
					pref->Flags &= ~(PREFF_FULLSCREEN);
				}
			}
		}
		
		if (pref->Flags & PREFF_FULLSCREEN)
		{
			if (pref->ScreenMode != -1L)
			{
				struct Screen *scr;
				
				/* maybe screen couldn't be closed before (visitor window) */
				if (glob->OpenedScreen) CloseScreen(glob->OpenedScreen);
				
				if (!(scr = OpenScreenTags(NULL,
					SA_DisplayID, pref->ScreenMode,
					SA_Width, (ULONG)pref->ScreenWidth,
					SA_Height, (ULONG)pref->ScreenHeight,
					SA_Depth, (ULONG)pref->ScreenDepth,
					SA_Overscan, (ULONG)pref->OverscanType,
					SA_AutoScroll, (ULONG)pref->AutoScroll,
					SA_SharePens, (ULONG)TRUE,
					SA_Interleaved, (ULONG)TRUE,
					SA_PubName, "Fireworks",
					SA_Colors32, ScreenPalette,
					SA_Pens, PenArray,
					TAG_DONE )))
				{
					Message("Unable to open selected screen!",NULL);
				}
				else
				{
					glob->Screen = glob->OpenedScreen = scr;
					
					if (glob->LockedScreen)
					{
						UnlockPubScreen(NULL, glob->LockedScreen);
						glob->LockedScreen = NULL;
					}
				}
			}
		}
		else
		{
			if (!glob->LockedScreen)
			{
				glob->LockedScreen = GetScreen(glob);
			}
			glob->Screen = glob->LockedScreen;
		}
		
		if (glob->Screen)
		{
			if (!(glob->VisualInfo = GetVisualInfo(glob->Screen, TAG_DONE)))
			{
				Message("No visual info!",NULL);
			}
			else
			{
				glob->ww = pref->WinW;
				glob->wh = pref->WinH;
				
				if (!(WindowLayout(glob, &minw, &minh, &maxw, &maxh )))
				{
					Message("Couldn't layout window!",NULL);
				}
				else
				{
					if (pref->Flags & PREFF_FULLSCREEN)
					{
						ULONG height;
						ULONG BDrop;
						
						if (pref->Flags & PREFF_HDTITLEBAR)
						{
							height = pref->ScreenHeight;
							BDrop = FALSE;
						}
						else
						{
							height = pref->ScreenHeight-(glob->Screen->BarHeight+1);
							BDrop = FALSE;
						}
						
						glob->Window = OpenWindowTags( NULL,
								WA_PubScreen, glob->Screen,
								WA_Left, 0,
								WA_Top, glob->Screen->BarHeight+1,
								WA_Width, pref->ScreenWidth,
								WA_Height, height,
								WA_Backdrop, BDrop,
								WA_Borderless, TRUE,
								WA_Activate, TRUE,
								WA_IDCMP, IDCMP_MENUPICK|IDCMP_REFRESHWINDOW,
								WA_SimpleRefresh, TRUE,
								WA_NewLookMenus, TRUE,
								TAG_DONE );
					}
					else
					{
						glob->Window = OpenWindowTags( NULL,
								WA_PubScreen, glob->Screen,
								WA_Title, "Fireworks",
								WA_Left, pref->WinX,
								WA_Top, pref->WinY,
								WA_InnerWidth, (ULONG)glob->ww,
								WA_InnerHeight, (ULONG)glob->wh,
								WA_GimmeZeroZero, TRUE,
								WA_DepthGadget, TRUE,
								WA_SizeGadget, TRUE,
								WA_SizeBBottom, TRUE,
								WA_CloseGadget, TRUE,
								WA_DragBar, TRUE,
								WA_Activate, TRUE,
								WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE|IDCMP_MENUPICK|IDCMP_REFRESHWINDOW,
								WA_SimpleRefresh, TRUE,
								WA_NewLookMenus, TRUE,
								TAG_DONE );
					}
					
					if (!glob->Window)
					{
						Message("Couldn't open window!",NULL);
					}
					else
					{
						glob->ProcWindow = ((struct Process*)MyTask)->pr_WindowPtr;
						((struct Process*)MyTask)->pr_WindowPtr = glob->Window;
						
						if ( (wbscr = LockPubScreen("Workbench") ))
						{
							if (glob->Screen == wbscr)
							{
								glob->AppWindow = AddAppWindow( 1, NULL, glob->Window, glob->AppPort, TAG_DONE);
							}
							
							UnlockPubScreen( NULL, wbscr );
						}
						
						glob->ww = glob->Window->Width  - glob->Window->BorderLeft-glob->Window->BorderRight ;
						glob->wh = glob->Window->Height - glob->Window->BorderTop -glob->Window->BorderBottom;
						
						glob->Window->MinWidth  = minw + glob->Window->BorderLeft+glob->Window->BorderRight;
						glob->Window->MinHeight = minh + glob->Window->BorderTop +glob->Window->BorderBottom;
						glob->Window->MaxWidth  = maxw + glob->Window->BorderLeft+glob->Window->BorderRight;
						glob->Window->MaxHeight = maxh + glob->Window->BorderTop +glob->Window->BorderBottom;
						
						if (!(InitOrUpdateGraphics(glob,pref)))
						{
							Message("Failed to initialize graphics!",NULL);
						}
						else
						{
							BltBitMapRastPort(glob->PaintBitMap, 0, 0, glob->Window->RPort, 0, 0, glob->ww, glob->wh, 0xc0);
							
							if (pref->Flags & PREFF_BACKDROP)
								if (pref->Image) LoadImage(glob,pref);
							
							if (pref->Flags & PREFF_ASLHOOKS)
								FWNewMenu[Menu_Hooks].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Hooks].nm_Flags &= (~CHECKED);
							
							if (pref->Flags & PREFF_TILE)
								FWNewMenu[Menu_Tile].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Tile].nm_Flags &= (~CHECKED);
							
							if (pref->Flags & PREFF_FULLSCREEN)
								FWNewMenu[Menu_Fullscreen].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Fullscreen].nm_Flags &= (~CHECKED);
							
							if (pref->Flags & PREFF_HDTITLEBAR)
								FWNewMenu[Menu_Titlebar].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Titlebar].nm_Flags &= (~CHECKED);
							
							if (pref->Flags & PREFF_HDMOUSEPTR)
								FWNewMenu[Menu_Mouse].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Mouse].nm_Flags &= (~CHECKED);
							
							if (pref->Flags & PREFF_DOUBLE)
								FWNewMenu[Menu_Double].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Double].nm_Flags &= (~CHECKED);
							
							if (pref->Flags & PREFF_SPARKS)
								FWNewMenu[Menu_Sparks].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Sparks].nm_Flags &= (~CHECKED);
							
							for (mode = 0; mode < NUM_FWMODES ; mode++)
							{
								if (pref->FWMode == mode)
									FWNewMenu[Menu_Mode1+mode].nm_Flags |= CHECKED;
								else
									FWNewMenu[Menu_Mode1+mode].nm_Flags &= (~CHECKED);
							}
							
							for (sens = 1; sens <= 3 ; sens++)
							{
								if (((sens == 1) && (pref->Sensitivity == 100)) ||
								    ((sens == 2) && (pref->Sensitivity ==  75)) ||
								    ((sens == 3) && (pref->Sensitivity ==  50)))
									FWNewMenu[Menu_Sens1+sens-1].nm_Flags |= CHECKED;
								else
									FWNewMenu[Menu_Sens1+sens-1].nm_Flags &= (~CHECKED);
							}
							
							for (init = 0; init <= 4 ; init++)
							{
								BOOL set = FALSE;
								
								switch(init)
								{
									case 0:
										if (!(pref->Flags&(PREFF_GM|PREFF_GS|PREFF_XG|PREFF_MT32)))
											set = TRUE;
										break;
									case 1:
										if (pref->Flags & PREFF_GM)
											set = TRUE;
										break;
									case 2:
										if (pref->Flags & PREFF_GS)
											set = TRUE;
										break;
									case 3:
										if (pref->Flags & PREFF_XG)
											set = TRUE;
										break;
									case 4:
										if (pref->Flags & PREFF_MT32)
											set = TRUE;
										break;
								}
								
								if (set)
									FWNewMenu[Menu_Init1+init].nm_Flags |= CHECKED;
								else
									FWNewMenu[Menu_Init1+init].nm_Flags &= ~(CHECKED);
							}
							
							if (pref->Flags & PREFF_TIMESTAMP)
								FWNewMenu[Menu_Timestamp].nm_Flags |= CHECKED;
							else
								FWNewMenu[Menu_Timestamp].nm_Flags &= (~CHECKED);
							
							if (!(glob->Menu=(struct Menu *)CreateMenus(FWNewMenu, TAG_DONE)))
							{
								Message("Failed to create intuition menu.",NULL);
							}
							else
							{
								LayoutMenus(glob->Menu, glob->VisualInfo, GTMN_NewLookMenus, TRUE, TAG_DONE);
								SetMenuStrip(glob->Window, glob->Menu);
								
								Success = TRUE;
							}
							
						}
					}
				}
			}
		}
	}

	if (!Success) CloseGUI(glob, pref);

	return(Success);
}



/*----------------------*/
/* Set the window title */
/*----------------------*/

void TitleWindow(struct Globals *glob, UBYTE *Text, ...)
{
	va_list Arg;
	va_start(Arg,Text);
	
	vsprintf(glob->WTitle, Text, Arg);
	
	if (glob->OpenedScreen)
	{
		SetWindowTitles(glob->Window, (UBYTE*) ~0, glob->WTitle);
	}
	else
	{
		SetWindowTitles(glob->Window, glob->WTitle, (UBYTE*) ~0);
	}
	
	va_end(Arg);
}



/*---------------------------*/
/* Choose desired screenmode */
/*---------------------------*/

BOOL SelectScreenMode(struct Globals *glob, struct Prefs *pref, struct ScreenModeRequester **srq)
{
	BOOL Result = FALSE;
	
	ULONG InitialDisplayID = pref->ScreenMode;
	UWORD InitialDisplayDepth = pref->ScreenDepth;
	UWORD InitialDisplayWidth = pref->ScreenWidth;
	UWORD InitialDisplayHeight = pref->ScreenHeight;
	ULONG InitialOverscanType = pref->OverscanType;
	BOOL InitialAutoScroll = pref->AutoScroll;
	
	if (InitialDisplayID == -1L)
	{
		struct DimensionInfo dims;
		
		if ((InitialDisplayID = BestModeID(
			BIDTAG_NominalWidth, 320,
			BIDTAG_NominalHeight, 240,
			BIDTAG_Depth, 8,
			TAG_DONE )) == INVALID_ID)
		{
			InitialDisplayID = LORES_KEY;
		}
		
		InitialOverscanType = OSCAN_TEXT;
		InitialAutoScroll = TRUE;
		
		if (GetDisplayInfoData(NULL, (UBYTE*)&dims, sizeof(dims), DTAG_DIMS, InitialDisplayID) < sizeof(dims))
		{
			InitialDisplayDepth = 8;
			InitialDisplayWidth = 320;
			InitialDisplayHeight = 240;
		}
		else
		{
			InitialDisplayDepth  = dims.MaxDepth;
			InitialDisplayWidth  = (dims.TxtOScan.MaxX-dims.TxtOScan.MinX) + 1;
			InitialDisplayHeight = (dims.TxtOScan.MaxY-dims.TxtOScan.MinY) + 1;
		}
	}
	
	if (!(*srq))
	{
		if (!((*srq) = AllocAslRequestTags(ASL_ScreenModeRequest,
			ASLSM_TitleText, "Select screen mode",
			TAG_DONE)))
		{
			Message("Unable to allocate ASL screenmode requester.",NULL);
		}
	}
	
	if (*srq)
	{
		Result = AslRequestTags(*srq,
			ASLSM_Screen, GetScreen(glob),
			ASLSM_DoWidth, TRUE,
			ASLSM_DoHeight, TRUE,
			ASLSM_DoDepth, TRUE,
			ASLSM_DoOverscanType, TRUE,
			ASLSM_DoAutoScroll, TRUE,
			ASLSM_InitialDisplayID, InitialDisplayID,
			ASLSM_InitialDisplayWidth, InitialDisplayWidth,
			ASLSM_InitialDisplayHeight, InitialDisplayHeight,
			ASLSM_InitialDisplayDepth, InitialDisplayDepth,
			ASLSM_InitialOverscanType, InitialOverscanType,
			ASLSM_InitialAutoScroll, InitialAutoScroll,
			TAG_DONE);
		
		if (Result)
		{
			pref->ScreenMode=(*srq)->sm_DisplayID;
			pref->ScreenWidth=(*srq)->sm_DisplayWidth;
			pref->ScreenHeight=(*srq)->sm_DisplayHeight;
			pref->ScreenDepth=(*srq)->sm_DisplayDepth;
			pref->OverscanType=(*srq)->sm_OverscanType;
			pref->AutoScroll=(*srq)->sm_AutoScroll;
		}
	}
	return(Result);
}



/*--------------------------------*/
/* Close the GUI (Screen, Window) */
/*--------------------------------*/

void CloseGUI(struct Globals *glob, struct Prefs *pref)
{
	if (glob->Window)
	{
		if (glob->Menu)
		{
			ClearMenuStrip(glob->Window);
			
			FreeMenus(glob->Menu);
			glob->Menu = NULL;
		}
		
		FreeGraphics(glob);
		
		((struct Process*)MyTask)->pr_WindowPtr=glob->ProcWindow;
		glob->ProcWindow = NULL;
		
		if (glob->LockedScreen)
		{
			pref->WinX = glob->Window->LeftEdge;
			pref->WinY = glob->Window->TopEdge;
			pref->WinW = glob->ww;
			pref->WinH = glob->wh;
		}
		
		if (glob->AppWindow)
		{
			RemoveAppWindow(glob->AppWindow);
			glob->AppWindow = NULL;
		}
		
		CloseWindow(glob->Window);
		glob->Window = NULL;
	}
	
	if (glob->Screen)
	{
		if (glob->VisualInfo)
		{
			FreeVisualInfo(glob->VisualInfo);
			glob->VisualInfo = NULL;
		}
		
		if (glob->LockedScreen)
		{
			UnlockPubScreen(NULL, glob->LockedScreen);
			glob->LockedScreen = NULL;
		}
		
		if (glob->OpenedScreen)
		{
			if (CloseScreen(glob->OpenedScreen))
				glob->OpenedScreen = NULL;
		}
		
		glob->Screen = NULL;
	}
	
	if (glob->AppPort)
	{
		struct Message *msg;
		
		while((msg=GetMsg(glob->AppPort))) ReplyMsg(msg);
		
		DeleteMsgPort(glob->AppPort);
		glob->AppPort = NULL;
	}
}



/*-------------------------*/
/* Set Busy/Normal Pointer */
/*-------------------------*/

void BusyPointer(struct Globals *glob, struct Prefs *pref)
{
	SetPointer(glob->Window, WaitPointer, 16, 16, -6, 0);
}


void NormalPointer(struct Globals *glob, struct Prefs *pref)
{
	if ((pref->Flags & PREFF_FULLSCREEN) &&
	    (pref->Flags & PREFF_HDMOUSEPTR))
		SetPointer(glob->Window, NoPointer, 16, 16, -6, 0);
	else
		ClearPointer(glob->Window);
}



/*-----------------------------*/
/* Calculate window dimensions */
/*-----------------------------*/

BOOL WindowLayout(struct Globals *glob, WORD *minw, WORD *minh, WORD *maxw, WORD *maxh)
{
	BOOL Success = TRUE;
	
	*minw=64;
	*minh=64;
	
	*maxw=1024;
	*maxh=1024;
	
	if (glob->ww < *minw) glob->ww = *minw;
	if (glob->wh < *minh) glob->wh = *minh;
	
	if (glob->ww > *maxw) glob->ww = *maxw;
	if (glob->wh > *maxh) glob->wh = *maxh;
	
	return(Success);
}



/*-----------------*/
/* Main event loop */
/*-----------------*/

UWORD NoteArray[128];
UBYTE ChanUse[16];

void MainLoop(struct Globals *glob, struct Prefs *pref)
{
	UWORD Mask = 0xffff;
	
	UWORD i;
	
	BOOL Active;
	APTR fwdata;
	
	ULONG signals;
	ULONG gotsignals;
	struct IntuiMessage *imsg;
	ULONG Cl;
	UWORD Co;
	APTR IA;
	
	ULONG timersig = (1L << glob->treq->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
	ULONG appsig   = (1L << glob->AppPort->mp_SigBit);
	ULONG winsig   = (1L << glob->Window->UserPort->mp_SigBit);
	BOOL TimerActive;
	
	UBYTE Err;
	
	srand(GetTimeDelta());
	for (i=0; i<256; i++)
	{
		randomarray33[i] = ((rand()%100)<33) ? 1 : 0;
	}


	Active=TRUE;
	glob->GUIRefresh=FALSE;
	glob->LinkRefresh=FALSE;
	
	memset(NoteArray,0,sizeof(NoteArray));
	memset(ChanUse  ,0,sizeof(ChanUse  ));

	Watch = FALSE;
//	AddIntServer(INTB_VERTB, &VertBlank);

	glob->ww = glob->Window->Width  - glob->Window->BorderLeft-glob->Window->BorderRight ;
	glob->wh = glob->Window->Height - glob->Window->BorderTop -glob->Window->BorderBottom;
	
	if (!(fwdata = (FWDefinitions[pref->FWMode].InitFireworks)(glob, pref)))
	{
	  Message("Unable to initialize fireworks.", "Oh no");
	}
	else
	{
		signals = winsig | appsig | timersig | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_E;
		
		TimerActive = FALSE;
		SetTaskPri(MyTask, HIGHPRI);
		while(Active)
		{
			if (glob->GUIRefresh)
			{
				BOOL SaveWatch = Watch;
				Watch = FALSE;
				
				CloseGUI(glob, pref);
				
				if (!OpenGUI(glob, pref))
				{
					pref->Flags &= (~PREFF_FULLSCREEN);
					
					if (!OpenGUI(glob, pref))
					{
						Active = FALSE;
						break;
					}
				}
				
				TitleWindow(glob, "Fireworks [%s]%s", pref->Link, TimerActive ? "" : " [idle]");
				
				winsig = (1L << glob->Window->UserPort->mp_SigBit);
				signals = winsig | appsig | timersig | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_E;
				
				if (fwdata) (FWDefinitions[pref->FWMode].RethinkWindow)(fwdata);
				
				glob->GUIRefresh = FALSE;
				
				Watch=SaveWatch;
			}

			if (glob->LinkRefresh)
			{
				struct MidiLink *newlink;
				
				if (!(newlink = AddMidiLink(glob->midi, MLTYPE_Receiver,
					MLINK_Name, "VU Meter Link",
					MLINK_Location, pref->Link,
					MLINK_EventMask, CMF_Note|CMF_Mode,
					MLINK_Comment,  "Fireworks [Input]",
					TAG_DONE)))
				{
					Message("Cannot create link to MIDI interface '%s'",NULL,pref->Link);
				}
				else
				{
					RemoveMidiLink( glob->link );
					FlushMidi ( glob->midi );
					
					glob->link = newlink;
					
					TitleWindow(glob, "Fireworks [%s]%s", pref->Link, TimerActive ? "" : " [idle]");
					
					memset(NoteArray,0,sizeof(NoteArray));
					memset(ChanUse  ,0,sizeof(ChanUse  ));
					
					if (fwdata) (FWDefinitions[pref->FWMode].FreeNoteData)(fwdata);
				}
				
				glob->LinkRefresh = FALSE;
			}

			gotsignals = Wait(signals);

			if (gotsignals & winsig)
			{
				while(Active && (imsg=(struct IntuiMessage*)GetMsg(glob->Window->UserPort)))
				{
					Cl=imsg->Class;
					Co=imsg->Code;
					IA=imsg->IAddress;
					ReplyMsg((struct Message*)imsg);
					
					if (Cl==IDCMP_CLOSEWINDOW)
					{
						if (AskAsync(glob))
						{
							Active = FALSE;
							break;
						}
						else
							AsyncMessage(glob,CloseTask, "Cannot quit yet. Please close all open requesters.","I will");
					}
					
					if (Cl==IDCMP_NEWSIZE)
					{
						BOOL SaveWatch = Watch;
						Watch = FALSE;
						
						glob->ww = pref->WinW = glob->Window->Width  - glob->Window->BorderLeft-glob->Window->BorderRight ;
						glob->wh = pref->WinH = glob->Window->Height - glob->Window->BorderTop -glob->Window->BorderBottom;
						
						if (!(InitOrUpdateGraphics(glob, pref)))
						{
							Message("Failed to update graphics!",NULL);
						}
						
						if (glob->PaintBitMap)
						{
							BltBitMapRastPort(glob->PaintBitMap, 0, 0, glob->Window->RPort, 0, 0, glob->ww, glob->wh, 0xc0);
						}
						else EraseRect(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
						
						if (fwdata) (FWDefinitions[pref->FWMode].RethinkWindow)(fwdata);
						
						Watch = SaveWatch;
					}
					
					if (Cl==IDCMP_REFRESHWINDOW)
					{
						BeginRefresh( glob->Window );
						
						if (glob->PaintBitMap)
						{
							BltBitMapRastPort(glob->PaintBitMap, 0, 0, glob->Window->RPort, 0, 0, glob->ww, glob->wh, 0xc0);
						}
						else EraseRect(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
						
					    EndRefresh( glob->Window, TRUE );
					}
					
					if (Cl==IDCMP_MENUPICK)
					{
						BOOL SaveWatch = Watch;
						
						struct MenuItem *n;
						ULONG pick;
						
						Watch = FALSE;
						
						while( (Co != MENUNULL) && Active)
						{
							n = ItemAddress( glob->Window->MenuStrip, (ULONG)Co );
							pick = (ULONG) GTMENUITEM_USERDATA( n );
							
							switch(pick)
							{
								case Menu_Link:
								{
									AsyncSelectMIDILink(glob, pref);
								}
								break;
								
								case Menu_Hooks:
								{
									if (n->Flags & CHECKED)
										pref->Flags |= PREFF_ASLHOOKS;
									else
									{
										pref->Flags &= (~PREFF_ASLHOOKS);
										
										/* Damn, there is no (legal) way */
										/* to clear the filter function */
										/* of an existing ASL requester */
										
										if (glob->ImageFR)
										{
											FreeAslRequest(glob->ImageFR);
											glob->ImageFR = NULL;
										}
										
										if (glob->MIDIFR)
										{
											FreeAslRequest(glob->MIDIFR);
											glob->MIDIFR = NULL;
										}
									}
								}
								break;
								
								case Menu_Save:
								{
									if ((!SavePrefs(glob, pref,"ENVARC:")) ||
										(!SavePrefs(glob, pref,"ENV:")))
										AsyncMessage(glob,SaveTask, "Failed to save configuration!", NULL);
								}
								break;
								
								case Menu_About:
								{
									AsyncMessage(glob,AboutTask, "Fireworks\n© 1998 by Christian Buchner\nflowerp@eikon.e-technik.tu-muenchen.de","Cool");
								}
								break;
								
								case Menu_Quit:
								{
									if (AskAsync(glob))
										Active = FALSE;
									else
										AsyncMessage(glob,CloseTask, "Cannot quit yet. Please close all open requesters.","I will");
								}
								break;
								
								case Menu_Kill:
								{
									pref->Flags &= (~PREFF_BACKDROP);
									
									if (glob->dto)
									{
										SetAPen(glob->Window->RPort, glob->PenArray[Backgroundpen]);
										RectFill(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
										
										DisposeDTObject(glob->dto);
										glob->dto = NULL;
										
										if (!(InitOrUpdateGraphics(glob,pref)))
										{
											Message("Failed to update graphics!",NULL);
										}
										
										if (glob->PaintBitMap)
										{
											BltBitMapRastPort(glob->PaintBitMap, 0, 0, glob->Window->RPort, 0, 0, glob->ww, glob->wh, 0xc0);
										}
										else EraseRect(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
									}
								}
								break;
								
								case Menu_Load:
								{
									AsyncSelectImage(glob, pref);
								}
								break;
								
								case Menu_Tile:
								{
									if (n->Flags & CHECKED)
										pref->Flags |= PREFF_TILE;
									else
										pref->Flags &= (~PREFF_TILE);
									
									if (!(InitOrUpdateGraphics(glob,pref)))
									{
										Message("Failed to update graphics!",NULL);
									}
									
									if (glob->PaintBitMap)
									{
										BltBitMapRastPort(glob->PaintBitMap, 0, 0, glob->Window->RPort, 0, 0, glob->ww, glob->wh, 0xc0);
									}
									else EraseRect(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
								}
								break;
								
								case Menu_Screenmode:
								{
									AsyncSelectScreenMode(glob, pref);
								}
								break;
								
								case Menu_Fullscreen:
								{
									if (n->Flags & CHECKED)
									{
										if (pref->ScreenMode == -1L)
										{
											n->Flags &= (~CHECKED);
											AsyncSelectScreenMode(glob, pref);
										}
										else
										{
											pref->Flags |= PREFF_FULLSCREEN;
											glob->GUIRefresh = TRUE;
										}
									}
									else
									{
										glob->GUIRefresh = TRUE;
										pref->Flags &= (~PREFF_FULLSCREEN);
									}
								}
								break;
								
								case Menu_Titlebar:
								{
									if (n->Flags & CHECKED)
									{
										if (!(pref->Flags & PREFF_HDTITLEBAR))
											glob->GUIRefresh = TRUE;
										
										pref->Flags |= PREFF_HDTITLEBAR;
									}
									else
									{
										if (pref->Flags & PREFF_HDTITLEBAR)
											glob->GUIRefresh = TRUE;
										
										pref->Flags &= ~(PREFF_HDTITLEBAR);
									}
								}
								break;
								
								case Menu_Mouse:
								{
									if (n->Flags & CHECKED)
									{
										pref->Flags |= PREFF_HDMOUSEPTR;
										NormalPointer(glob, pref);
									}
									else
									{
										pref->Flags &= ~(PREFF_HDMOUSEPTR);
										NormalPointer(glob, pref);
									}
								}
								break;
								
								case Menu_Mode1:
								case Menu_Mode2:
								case Menu_Mode3:
								{
									if (n->Flags & CHECKED)
									{
										UWORD NewMode = pick - Menu_Mode1;
										if (NewMode != pref->FWMode)
										{
											if (fwdata)
											{
												(FWDefinitions[pref->FWMode].ExitFireworks)(fwdata);
												fwdata = NULL;
											}
											
											pref->FWMode = NewMode;
											
											if (!(fwdata = (FWDefinitions[pref->FWMode].InitFireworks)(glob, pref)))
											{
												Message("Unable to initialize fireworks.", "Oh no");
												n->Flags &= (~CHECKED);
											}
										}
									}
								}
								break;
								
								case Menu_Sens1:
								case Menu_Sens2:
								case Menu_Sens3:
								{
									if (n->Flags & CHECKED)
									{
										UWORD sens = pick - Menu_Sens1 + 1;
										switch(sens)
										{
											case 1: pref->Sensitivity=100; break;
											case 2: pref->Sensitivity= 75; break;
											case 3: pref->Sensitivity= 50; break;
										}
									}
								}
								break;
								
								case Menu_Double:
								{
									if (n->Flags & CHECKED)
										pref->Flags |= PREFF_DOUBLE;
									else
										pref->Flags &= (~PREFF_DOUBLE);
								}
								break;
								
								case Menu_Sparks:
								{
									if (n->Flags & CHECKED)
										pref->Flags |= PREFF_SPARKS;
									else
										pref->Flags &= (~PREFF_SPARKS);
								}
								break;
								
								case Menu_Release:
								{
									memset(NoteArray,0,sizeof(NoteArray));
									memset(ChanUse  ,0,sizeof(ChanUse  ));
									
									if (fwdata) (FWDefinitions[pref->FWMode].TimePassed)(fwdata);
									if (fwdata) (FWDefinitions[pref->FWMode].ReleaseNotes)(fwdata, 0);
								}
								break;
								
								case Menu_Play:
								{
									AsyncSelectAndPlay(glob, pref);
								};
								break;
								
								case Menu_Stop:
								{
									StopMIDI(glob,pref);
								}
								break;
								
								case Menu_Init1:
								case Menu_Init2:
								case Menu_Init3:
								case Menu_Init4:
								case Menu_Init5:
								{
									if (n->Flags & CHECKED)
									{
										UWORD init = pick - Menu_Init1;
										pref->Flags &= (~(PREFF_GM|PREFF_GS|PREFF_XG|PREFF_MT32));
										
										switch(init)
										{
											case 1: pref->Flags |= PREFF_GM;
												break;
											case 2: pref->Flags |= PREFF_GS;
												break;
											case 3: pref->Flags |= PREFF_XG;
												break;
											case 4: pref->Flags |= PREFF_MT32;
												break;
										}
									}
								}
								break;
								
								case Menu_Timestamp:
								{
									if (n->Flags & CHECKED)
										pref->Flags |= PREFF_TIMESTAMP;
									else
										pref->Flags &= (~PREFF_TIMESTAMP);
								}
								break;
								
							}
							
							Co = n->NextSelect;
						}
						
						Watch = SaveWatch;
					}
				}
			}

			if (Active && (gotsignals & SIGBREAKF_CTRL_E))
			{
				MidiMsg msg;
				
				if (fwdata) (FWDefinitions[pref->FWMode].TimePassed)(fwdata);
				
				while (GetMidi(glob->midi,&msg))
				{
					LONG reltime = 0;
					
					if (pref->Flags & PREFF_TIMESTAMP)
					{
						reltime = HowOldIsTimestamp(msg.mm_Time);
						if (reltime < 0) reltime = 0;
					}
					
					if (noteon(&msg))
					{
						UBYTE chn  = msg.mm_Status & MS_ChanBits;
						UWORD note = msg.mm_Data1;
						UWORD cmsk = 1<<chn;

						// if ((Mask & cmsk) && !((NoteArray[note] & Mask)))
						// {
						// }
						
						if (!(NoteArray[note] & cmsk))
						{
							/* Channel wird benutzt */
							ChanUse[chn]++;
							
							NoteArray[note] |= cmsk;
							
							/* Note angeschlagen */
							if (fwdata){
							  (FWDefinitions[pref->FWMode].NoteOn)(fwdata, chn, note, msg.mm_Data2, reltime);
							}
						}      
						
					}
					else
					{
						if (noteoff(&msg))
						{
							UBYTE chn  = msg.mm_Status & MS_ChanBits;
							UWORD note = msg.mm_Data1;
							UWORD cmsk = 1<<chn;
							if (NoteArray[note] & cmsk)
							{
								/* Channel wird nicht mehr benutzt */
								ChanUse[chn]--;
								
								NoteArray[note] &= (~cmsk);
								
								/* Note losgelassen */
								
								if (fwdata) (FWDefinitions[pref->FWMode].NoteOff)(fwdata, chn, note, reltime);
							}
							
							// if ((Mask & cmsk) && (!(NoteArray[note] & Mask)))
							// {
							// }
						}
						else
						{
							if (modemsg(&msg))
							{
								UBYTE chn  = msg.mm_Status & MS_ChanBits;
								UBYTE mode = msg.mm_Data1;
								UWORD note;
								UWORD cmsk = 1<<chn;
								
								if (mode == MM_AllOff)
								{
									for (note=0;note<128;note++)
									{
										if (NoteArray[note] & cmsk)
										{
											/* Channel wird nicht mehr benuzt */
											ChanUse[chn]--;
											
											if (fwdata) (FWDefinitions[pref->FWMode].NoteOff)(fwdata, chn, note, reltime);
											
											NoteArray[note] &= (~cmsk);
										}
										
										// if ((Mask & cmsk) && (!(NoteArray[note] & Mask)))
										// {
										// }
									}
								}
							}
						}
					}
				}
				
				if ((Err = GetMidiErr(glob->midi)))
				{
					if (Err & CMEF_MsgErr)			AsyncMessage(glob,ErrTask, "MIDI Error: MsgErr!",NULL);
					if (Err & CMEF_BufferFull)		AsyncMessage(glob,ErrTask, "MIDI Error: BufferFull!",NULL);
					if (Err & CMEF_SysExFull)		AsyncMessage(glob,ErrTask, "MIDI Error: SysExFull!",NULL);
					if (Err & CMEF_ParseMem)		AsyncMessage(glob,ErrTask, "MIDI Error: ParseMem!",NULL);
					if (Err & CMEF_RecvErr)			AsyncMessage(glob,ErrTask, "MIDI Error: RecvErr!",NULL);
					if (Err & CMEF_RecvOverflow)	AsyncMessage(glob,ErrTask, "MIDI Error: RecvOverflow!",NULL);
					if (Err & CMEF_SysExTooBig)		AsyncMessage(glob,ErrTask, "MIDI Error: SysExTooBig!",NULL);
				}
				
				/* (re-)start timer if it isn't running */
				if (!TimerActive)
				{
					/* but only if there is something to display */
					if (fwdata) if (!(FWDefinitions[pref->FWMode].IsIdle)(fwdata))
					{
						glob->treq->tr_node.io_Command = TR_ADDREQUEST;
						glob->treq->tr_time.tv_secs  = 0;
						glob->treq->tr_time.tv_micro = 1000000 / FPS;
						SendIO((struct IORequest*)glob->treq);
						TimerActive = TRUE;
						Watch = TRUE;
						
						TitleWindow(glob, "Fireworks [%s]", pref->Link);
					}
				}
			}

			if (Active && (gotsignals & timersig))
			{
				BOOL idle = TRUE;
				IdleCount = 0;
				
				if (fwdata) (FWDefinitions[pref->FWMode].TimePassed)(fwdata);
				if (fwdata) idle = (FWDefinitions[pref->FWMode].IsIdle)(fwdata);
				
				if (!idle)
				{
					glob->treq->tr_node.io_Command = TR_ADDREQUEST;
					glob->treq->tr_time.tv_secs  = 0;
					glob->treq->tr_time.tv_micro = 1000000 / FPS;
					SendIO((struct IORequest*)glob->treq);
				}
				
				if ((glob->BGBitMap) && (glob->PaintBitMap))
				{
//					BltBitMapRastPort(glob->BGBitMap, 0, 0, &glob->PaintRP, 0, 0, glob->ww, glob->wh, 0xc0);


					if (fwdata) (FWDefinitions[pref->FWMode].DrawFireworks)(fwdata, Mask);
					
	//				BltBitMapRastPort(glob->PaintBitMap, 0, 0, glob->Window->RPort, 0, 0, glob->ww, glob->wh, 0xc0);
				}
				
				if (idle)
				{
					TimerActive = FALSE;
					Watch = FALSE;
					SetTaskPri(MyTask, HIGHPRI);
					
					TitleWindow(glob, "Fireworks [%s] [idle]", pref->Link);
				}
				else
				{
					/* reset task priority if it has
					   been modified by the watchdog */
					if (MyTask->tc_Node.ln_Pri != NORMPRI)
					{
						SetTaskPri(MyTask, NORMPRI);
					}
				}
			}

			if (Active && (gotsignals & appsig))
			{
				struct AppMessage *appm;
				
				while((appm = (struct AppMessage*)GetMsg(glob->AppPort)))
				{
					if (appm->am_Type == AMTYPE_APPWINDOW)
					{
						BOOL SaveWatch = Watch;
						struct WBArg *arg = appm->am_ArgList;
						ULONG i;
						
						Watch = FALSE;
						
						for (i=0 ; i < appm->am_NumArgs ; i++, arg++)
						{
							BOOL IsPicture = FALSE;
							BOOL IsMIDI = FALSE;
							
							BPTR OldDir;
							BPTR lock;
							
							OldDir = CurrentDir(arg->wa_Lock);
							
							if ((lock = Lock(arg->wa_Name, SHARED_LOCK)))
							{
								struct DataType *dtn;
								
								if ((dtn = ObtainDataTypeA (DTST_FILE, (APTR) lock, NULL)))
								{
									if (dtn->dtn_Header->dth_GroupID == GID_PICTURE)
									{
										if (NameFromLock(lock, pref->Image, sizeof(pref->Image)))
										{
											IsPicture = TRUE;
										}
									}
									ReleaseDataType (dtn);
								}
								
								if (!IsPicture)
								{
									BPTR dupedlock;
									BPTR file;
									
									if ((dupedlock = DupLock(lock)))
									{
										if ((file = OpenFromLock(dupedlock)))
										{
											UBYTE header[20];
											
											if (Read(file, header, sizeof(header)) == sizeof(header))
											{
												if ( ((header[0] == 'M') &&
													  (header[1] == 'T') &&
													  (header[2] == 'h') &&
													  (header[3] == 'd'))
													
													||
													
													 ((header[0] == 'X') &&
													  (header[1] == 'P') &&
													  (header[2] == 'K') &&
													  (header[3] == 'F') &&
													  (header[16]== 'M') &&
													  (header[17]== 'T') &&
													  (header[18]== 'h') &&
													  (header[19]== 'd')) )
												{
													if (NameFromLock(lock, pref->MIDIFile, sizeof(pref->MIDIFile)))
													{
														IsMIDI = TRUE;
													}
												}
											}
											Close(file);
										} else UnLock(dupedlock);
									}
								}
								UnLock(lock);
							}
							CurrentDir(OldDir);
							
							if (IsPicture) LoadImage(glob, pref);
							else
								if (IsMIDI) PlayMIDI(glob, pref);
								else
									AsyncMessage(glob,DropTask, "Please drop only picture objects or MIDI files\ninto the Fireworks window.","Sorry");
						}
						Watch = SaveWatch;
					}
					ReplyMsg((struct Message*)appm);
				}
			}

			if (gotsignals & SIGBREAKF_CTRL_C)
			{
				if (AskAsync(glob))
				{
					Active = FALSE;
					Watch = FALSE;
				}
				else
					AsyncMessage(glob,CloseTask, "Cannot quit yet. Please close all open requesters.","I will");
			}
		}

//		RemIntServer(INTB_VERTB, &VertBlank);

		if (TimerActive)
		{
			AbortIO((struct IORequest*)glob->treq);
			WaitIO((struct IORequest*)glob->treq);
		}

		if (fwdata)
		{
			(FWDefinitions[pref->FWMode].ExitFireworks)(fwdata);
			fwdata = NULL;
		}

	}
}



/*-----------------------------------*/
/* Asynchronously select a MIDI link */
/*-----------------------------------*/

void SyncSelectMIDILink(struct Globals *glob, struct Prefs *pref, APTR UserData)
{
  /*
	APTR listreq;
	char *linkname;
	

	if (!(listreq = AllocListRequest(
		LISTREQ_Screen, GetScreen(glob),
		LISTREQ_TitleText, "Select input link",
		TAG_DONE )))
	{
		Message("Couldn't allocate a list requester.",NULL);
	}
	else
	*/

	/*
	{
		if (SelectCluster( listreq, pref->Link, sizeof(pref->Link), TAG_DONE ))
		{
			if (strcmp((glob->link)->ml_Location->mcl_Node.ln_Name,pref->Link))
			{
				glob->LinkRefresh = TRUE;
				Signal(MyTask, SIGBREAKF_CTRL_E);	// wake up main task
			}
		}
		FreeListRequest(listreq);
	}
*/
}

void AsyncSelectMIDILink(struct Globals *glob, struct Prefs *pref)
{
	if (!StartAsyncTask(glob, pref, "Fireworks MIDI link selection task", LinkTask, &SyncSelectMIDILink, NULL, 0))
	{
		SyncSelectMIDILink(glob, pref, NULL);
	}
}



/*------------------------------------*/
/* Asynchronously select a screenmode */
/*------------------------------------*/

void SyncSelectScreenMode(struct Globals *glob, struct Prefs *pref, APTR UserData)
{
	if (SelectScreenMode(glob, pref, &glob->ScreenModeRQ))
	{
		pref->Flags |= PREFF_FULLSCREEN;
		glob->GUIRefresh = TRUE;
		Signal(MyTask, SIGBREAKF_CTRL_E);	/* wake up main task */
	}
}

void AsyncSelectScreenMode(struct Globals *glob, struct Prefs *pref)
{
	if (!StartAsyncTask(glob, pref, "Fireworks ScreenMode selection task", ScreenTask, &SyncSelectScreenMode, NULL, 0))
	{
		SyncSelectScreenMode(glob, pref, NULL);
	}
}



/*---------------------------------------------------*/
/* Asynchronously select an image and notify program */
/*---------------------------------------------------*/

void AsyncSelectImageFunction(struct Globals *glob, struct Prefs *pref, APTR UserData)
{
	BOOL Success = FALSE;
	UBYTE *filename;
	UBYTE *ptr, save;
	BPTR dirlock;
	ULONG msglen;
	struct AppMessage *appm,*got;
	struct WBArg *argument;
	UBYTE *name;
	
	struct Process *MyProc = (struct Process*)FindTask(NULL);
	
	if (SelectImage(glob, pref, &glob->ImageFR, pref->Image, sizeof(pref->Image)))
	{
		pref->Flags |= PREFF_BACKDROP;
		
		/* now fake an AppMessage to notify program of image change */
		
		filename = FilePart(pref->Image);
		
		ptr = PathPart(pref->Image);
		save = *ptr;
		*ptr = 0;
		
		if ((dirlock = Lock(pref->Image, SHARED_LOCK)))
		{
			*ptr = save;
			msglen = sizeof(struct AppMessage)+sizeof(struct WBArg)+strlen(filename)+1;
			
			if ((appm = AllocVec(msglen,MEMF_ANY|MEMF_CLEAR)))
			{
				argument = (struct WBArg*)(appm+1);
				name = (UBYTE*)(argument+1);
				
				appm->am_Message.mn_Length = msglen;
				appm->am_Message.mn_ReplyPort = &MyProc->pr_MsgPort;
				
				appm->am_Type = AMTYPE_APPWINDOW;
				appm->am_NumArgs = 1;
				appm->am_ArgList = argument;
				
				argument->wa_Lock = dirlock;
				argument->wa_Name = name;
				
				strcpy(name, filename);
				
				PutMsg(glob->AppPort, (struct Message*)appm);
				
				got = NULL;
				while (got != appm)
				{
					WaitPort(&MyProc->pr_MsgPort);
					got = (struct AppMessage*)GetMsg(&MyProc->pr_MsgPort);
				}
				
				FreeVec(appm);
				
				Success = TRUE;
			}
			UnLock(dirlock);
		}
		else *ptr = save;
	}
}

void AsyncSelectImage(struct Globals *glob, struct Prefs *pref)
{
	if (!(StartAsyncTask(glob, pref, "Fireworks Backdrop selection task", ImageTask, &AsyncSelectImageFunction, NULL, 0)))
	{
		if (SelectImage(glob, pref, &glob->ImageFR, pref->Image, sizeof(pref->Image)))
		{
			pref->Flags |= PREFF_BACKDROP;
			LoadImage(glob,pref);
		}
	}
}



/*------------------------------------------*/
/* Asynchronously select & play a MIDI file */
/*------------------------------------------*/

void SelectAndPlay(struct Globals *glob, struct Prefs *pref, APTR UserData)
{
	if (SelectMIDI(glob, pref, &glob->MIDIFR, pref->MIDIFile,sizeof(pref->MIDIFile)))
	{
		PlayMIDI(glob,pref);
	}
}

void AsyncSelectAndPlay(struct Globals *glob, struct Prefs *pref)
{
	if (!StartAsyncTask(glob, pref, "Fireworks MIDI file selection task", PlayTask, &SelectAndPlay, NULL, 0))
	{
		SelectAndPlay(glob, pref, NULL);
	}
}



/*----------------------------*/
/* Initialize/Update Graphics */
/*----------------------------*/

BOOL InitOrUpdateGraphics(struct Globals *glob, struct Prefs *pref)
{
	BOOL Success = FALSE;
	
	if (glob->PaintLayer)
	{
		DeleteLayer(0, glob->PaintLayer);
		glob->PaintLayer = NULL;
	}
	
	if (glob->BGBitMap)
	{
		FreeBitMap(glob->BGBitMap);
		glob->BGBitMap = NULL;
	}
	
	if (glob->PaintBitMap)
	{
		FreeBitMap(glob->PaintBitMap);
		glob->PaintBitMap = NULL;
	}
	
	if (glob->Window)
	{
		UWORD i;
		struct ColorMap *cmap = glob->Window->WScreen->ViewPort.ColorMap;
		
		struct BitMap *friend = glob->Window->RPort->BitMap;
		ULONG depth = GetBitMapAttr(friend, BMA_DEPTH);
		ULONG flags = GetBitMapAttr(friend, BMA_FLAGS);
		
		struct RastPort BGRP;
		
		BusyPointer(glob, pref);
		
		for (i = 0 ; i < NUMPENS ; i++)
		{
			if (glob->PenArray[i] == -1L)
			{
				glob->PenArray[i] = ObtainBestPen(
					cmap,
					(ULONG)PenColors[i][0]<<24, 
					(ULONG)PenColors[i][1]<<24, 
					(ULONG)PenColors[i][2]<<24,
					OBP_Precision, PRECISION_IMAGE,
					TAG_DONE );
			}
		}
		
		/* Cybergraphics kludge */
		if ( !(flags & BMF_STANDARD) ) flags |= BMF_MINPLANES;
		
		if ((glob->BGBitMap = AllocBitMap(glob->ww, glob->wh, depth, flags | BMF_CLEAR, friend)))
		{
			struct BitMap *ImageBM   = NULL;
			struct BitMapHeader *bmh = NULL;
			
			InitRastPort(&BGRP);
			BGRP.BitMap = glob->BGBitMap;
			
			SetAPen(&BGRP, glob->PenArray[Backgroundpen]);
			
			if (glob->dto)
			{
				GetDTAttrs(glob->dto, PDTA_DestBitMap, &ImageBM,
								 PDTA_BitMapHeader, &bmh,
							TAG_DONE);
			}
			
			if ((!ImageBM) || (!bmh))
			{
				RectFill(&BGRP, 0, 0, (glob->ww)-1, (glob->wh)-1);
			}
			else
			{
				UWORD width = bmh->bmh_Width;
				UWORD height= bmh->bmh_Height;
				
				if (pref->Flags & PREFF_TILE)
				{
					UWORD x,y;
					UWORD dow, doh;
					
					for (y = 0 ; y < glob->wh ; y += doh)
					{
						doh = height;
						
						if (y + doh > glob->wh)
							doh = glob->wh - y;
						
						for (x = 0 ; x < glob->ww ; x += dow)
						{
							dow = width; 
							
							if (x + dow > glob->ww)
								dow = glob->ww - x;
							
							BltBitMap(ImageBM, 0, 0, glob->BGBitMap, x, y, dow, doh, 0xc0, 0xff,NULL);
						}
					}
				}
				else
				{
					struct BitScaleArgs bsa;
					
					bsa.bsa_SrcX		= 0;
					bsa.bsa_SrcY		= 0;
					bsa.bsa_SrcWidth	= width;
					bsa.bsa_SrcHeight	= height;
					bsa.bsa_XSrcFactor	= width;
					bsa.bsa_YSrcFactor	= height;
					bsa.bsa_DestX		= 0;
					bsa.bsa_DestY		= 0;
					bsa.bsa_DestWidth	= glob->ww;
					bsa.bsa_DestHeight	= glob->wh;
					bsa.bsa_XDestFactor	= glob->ww;
					bsa.bsa_YDestFactor	= glob->wh;
					bsa.bsa_SrcBitMap	= ImageBM;
					bsa.bsa_DestBitMap	= glob->BGBitMap;
					bsa.bsa_Flags		= 0;
					bsa.bsa_XDDA		= 0;
					bsa.bsa_YDDA		= 0;
					bsa.bsa_Reserved1	= 0;
					bsa.bsa_Reserved2	= 0;
					
					BitMapScale(&bsa);
				}
			}
			
			if ( (glob->PaintBitMap = AllocBitMap(glob->ww, glob->wh, depth, flags | BMF_CLEAR, friend)))
			{
				InitRastPort(&glob->PaintRP);
				glob->PaintRP.BitMap = glob->PaintBitMap;
				
				if (!(glob->LInfo)) glob->LInfo = NewLayerInfo();
				
				if (glob->LInfo)
				{
					if ((glob->PaintLayer = CreateUpfrontLayer(glob->LInfo, glob->PaintBitMap, 0, 0, (glob->ww)-1, (glob->wh)-1, LAYERSMART, NULL)))
					{
						glob->PaintRP.Layer = glob->PaintLayer;
						
						BltBitMapRastPort(glob->BGBitMap, 0, 0, &glob->PaintRP, 0, 0, glob->ww, glob->wh, 0xc0);
						
						Success = TRUE;
					}
				}
			}
		}
		NormalPointer(glob,pref);
	}
	
	if (!Success)
	{
		if (glob->PaintLayer)
		{
			DeleteLayer(0, glob->PaintLayer);
			glob->PaintLayer = NULL;
		}
		
		if (glob->BGBitMap)
		{
			FreeBitMap(glob->BGBitMap);
			glob->BGBitMap = NULL;
		}
		
		if (glob->PaintBitMap)
		{
			FreeBitMap(glob->PaintBitMap);
			glob->PaintBitMap = NULL;
		}
	}
	
	return(Success);
}



/*-------------------------*/
/* Free allocated Graphics */
/*-------------------------*/

void FreeGraphics(struct Globals *glob)
{
	UWORD i;
	
	if (glob->dto)
	{
		SetAPen(glob->Window->RPort, glob->PenArray[Backgroundpen]);
		RectFill(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
		
		DisposeDTObject(glob->dto);
		glob->dto = NULL;
	}
	
	if (glob->PaintLayer)
	{
		DeleteLayer(0, glob->PaintLayer);
		glob->PaintLayer = NULL;
	}
	
	if (glob->LInfo)
	{
		DisposeLayerInfo(glob->LInfo);
		glob->LInfo = NULL;
	}
	
	if (glob->BGBitMap)
	{
		FreeBitMap(glob->BGBitMap);
		glob->BGBitMap = NULL;
	}
	
	if (glob->PaintBitMap)
	{
		FreeBitMap(glob->PaintBitMap);
		glob->PaintBitMap = NULL;
	}
	
	if (glob->Window)
	{
		struct ColorMap *cmap = glob->Window->WScreen->ViewPort.ColorMap;
		
		for (i = 0 ; i < NUMPENS ; i++)
		{
			if (glob->PenArray[i] != -1L)
			{
				ReleasePen( cmap, glob->PenArray[i] );
				glob->PenArray[i] = -1L;
			}
		}
	}
}



/*--------------------*/
/* Get current screen */
/*--------------------*/

struct Screen *GetScreen(struct Globals *glob)
{
	struct Window *win;
	struct Screen *scr = NULL;
	
	if (MyTask)
	{
		win = ((struct Window*)((struct Process*)MyTask)->pr_WindowPtr);
		if (win) scr = win->WScreen;
	}
	
	if (!scr)
	{
		if (glob->LockedScreen)
			scr = glob->LockedScreen;
		else
			scr = glob->LockedScreen = LockPubScreen(NULL);
	}
	
	return(scr);
}



/*----------------------------------*/
/* Load image using datatypes (V43) */
/*----------------------------------*/

void LoadImage(struct Globals *glob, struct Prefs *pref)
{
	if (glob->dto)
	{
		SetAPen(glob->Window->RPort, glob->PenArray[Backgroundpen]);
		RectFill(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
		
		DisposeDTObject(glob->dto);
		glob->dto = NULL;
	}
	
	BusyPointer(glob, pref);
	
	if ((glob->dto = (Object *) NewDTObject(pref->Image,
		DTA_GroupID, GID_PICTURE, 
//		PDTA_DestMode, MODE_V43,
		PDTA_Remap, TRUE,
		PDTA_Screen, GetScreen(glob),
		PDTA_FreeSourceBitMap, TRUE,
		TAG_DONE) ))
	{
		SetDTAttrs(glob->dto, NULL, NULL,
					PDTA_Remap, TRUE,
					PDTA_Screen, GetScreen(glob),
					PDTA_FreeSourceBitMap, TRUE,
//					PDTA_UseFriendBitMap, TRUE,
					TAG_DONE );
		
		DoMethod( glob->dto, DTM_PROCLAYOUT, NULL, TRUE );
	}
	else
	{
		LONG errnum=IoErr();
		UBYTE errstring[80];
		
		if (errnum>=DTERROR_UNKNOWN_DATATYPE)
		{
			sprintf(errstring,GetDTString(errnum),pref->Image);
		}
		else
		{
			Fault(errnum,NULL,errstring,sizeof(errstring));
		}
		Message("%s: %s",NULL,pref->Image,errstring);
	}
	
	if (!(InitOrUpdateGraphics(glob,pref)))
	{
		Message("Failed to update graphics!",NULL);
	}
	
	if (glob->PaintBitMap)
	{
		BltBitMapRastPort(glob->PaintBitMap, 0, 0, glob->Window->RPort, 0, 0, glob->ww, glob->wh, 0xc0);
	}
	else EraseRect(glob->Window->RPort, 0, 0, (glob->ww)-1, (glob->wh)-1);
	
	NormalPointer(glob, pref);
}



/*-------------------------------------------*/
/* Datatypes hook function for ASL requester */
/*-------------------------------------------*/

ASM SAVEDS ULONG Filter (REG(a0) struct Hook *h, REG(a2) struct FileRequester *fr, REG(a1) struct AnchorPath *ap)
{
	struct DataType *dtn;
	ULONG use = FALSE;
	UBYTE buffer[300];
	BPTR lock;

	if (ap->ap_Info.fib_DirEntryType > 0)
	{
		use = TRUE;
	}
	else
	{
		strncpy (buffer, fr->fr_Drawer, sizeof (buffer));
		AddPart (buffer, ap->ap_Info.fib_FileName, sizeof (buffer));
		if ((lock = Lock (buffer, ACCESS_READ)))
		{
			if ((dtn = ObtainDataTypeA (DTST_FILE, (APTR) lock, NULL)))
			{
				if (dtn->dtn_Header->dth_GroupID == GID_PICTURE)
					use = TRUE;
				
				ReleaseDataType (dtn);
			}
			UnLock (lock);
		}
	}
	return (use);
}



/*--------------------------------------*/
/* Select image to load (ASL requester) */
/*--------------------------------------*/

BOOL SelectImage(struct Globals *glob, struct Prefs *pref, struct FileRequester **fr, UBYTE *filebuffer, ULONG MaxSize)
{
	BOOL Selected=FALSE;
	
	UBYTE initialdrawer[200];
	UBYTE initialfile  [40];
	
	struct Hook filter = {{0}};
	UBYTE save, *ptr;
	
	filter.h_Entry = (HOOKFUNC)Filter;
	
	ptr = PathPart(filebuffer);
	save = *ptr; *ptr = 0;
	strncpy (initialdrawer,filebuffer,sizeof(initialdrawer));
	initialdrawer[sizeof(initialdrawer)-1]=0;
	*ptr = save;
	strncpy (initialfile, FilePart(filebuffer), sizeof(initialfile));
	initialfile[sizeof(initialfile)-1]=0;
	
	if (!(*fr))
	{
		if(!((*fr) = AllocAslRequestTags (ASL_FileRequest,
					ASLFR_TitleText,	"Select Picture to load",
					ASLFR_PositiveText,	"Load",
					ASLFR_RejectIcons,	TRUE,
					ASLFR_DoPatterns, TRUE,
					TAG_DONE)))
		{
			Message("Couldn't allocate asl requester",NULL);
		}
	}
	
	if (*fr)
	{
		Selected = AslRequestTags(*fr,
			ASLFR_Screen, GetScreen(glob),
			ASLFR_InitialDrawer, initialdrawer,
			ASLFR_InitialFile, initialfile,
			(pref->Flags & PREFF_ASLHOOKS) ? ASLFR_FilterFunc : TAG_IGNORE,	&filter,
			TAG_DONE);
		
		if (Selected)
		{
			strncpy (filebuffer, (*fr)->fr_Drawer, MaxSize);
			AddPart (filebuffer, (*fr)->fr_File,   MaxSize);
		}
	}
	
	return(Selected);
}



/*--------------------------------------*/
/* MIDI hook function for ASL requester */
/*--------------------------------------*/

ASM SAVEDS ULONG MIDIFilter (REG(a0) struct Hook *h, REG(a2) struct FileRequester *fr, REG(a1) struct AnchorPath *ap)
{
	ULONG use = FALSE;
	
	UBYTE buffer[300];
	UBYTE header[20];
	BPTR file;
	
	if (ap->ap_Info.fib_DirEntryType >0)
		use = TRUE;
	else
	{
		strncpy (buffer, fr->fr_Drawer, sizeof (buffer));
		AddPart (buffer, ap->ap_Info.fib_FileName, sizeof (buffer));
		
		if ((file = Open (buffer, MODE_OLDFILE)))
		{
			if (Read(file,header,sizeof(header)) == sizeof(header))
			{
				if ( ((header[0] == 'M') &&
					  (header[1] == 'T') &&
					  (header[2] == 'h') &&
					  (header[3] == 'd'))
					
					||
					
					 ((header[0] == 'X') &&
					  (header[1] == 'P') &&
					  (header[2] == 'K') &&
					  (header[3] == 'F') &&
					  (header[16]== 'M') &&
					  (header[17]== 'T') &&
					  (header[18]== 'h') &&
					  (header[19]== 'd')) )
				{
					use = TRUE;
				}
			}
			Close (file);
		}
	}
	return(use);
}



/*----------------------------*/
/* Select a MIDI file to load */
/*----------------------------*/

BOOL SelectMIDI(struct Globals *glob, struct Prefs *pref, struct FileRequester **fr, UBYTE *filebuffer, ULONG MaxSize)
{
	BOOL Selected=FALSE;
	
	UBYTE initialdrawer[200];
	UBYTE initialfile  [40];
	
	struct Hook filter = {{0}};
	UBYTE save, *ptr;
	
	filter.h_Entry = (HOOKFUNC)MIDIFilter;
	
	ptr = PathPart(filebuffer);
	save = *ptr; *ptr = 0;
	strncpy (initialdrawer,filebuffer,sizeof(initialdrawer));
	initialdrawer[sizeof(initialdrawer)-1]=0;
	*ptr = save;
	strncpy (initialfile, FilePart(filebuffer), sizeof(initialfile));
	initialfile[sizeof(initialfile)-1]=0;

	if (!(*fr))
	{
		if (!((*fr) = AllocAslRequestTags (ASL_FileRequest,
					ASLFR_TitleText,	"Select MIDI file to play",
					ASLFR_PositiveText,	"Play",
					ASLFR_RejectIcons,	TRUE,
					ASLFR_DoPatterns, TRUE,
					TAG_DONE)))
		{
			Message("Couldn't allocate asl requester",NULL);
		}
	}
	if (*fr)
	{
		Selected = AslRequestTags(*fr,
					ASLFR_Screen, GetScreen(glob),
					ASLFR_InitialDrawer, initialdrawer,
					ASLFR_InitialFile, initialfile,
//					(pref->Flags & PREFF_ASLHOOKS) ? ASLFR_FilterFunc : TAG_IGNORE,	&filter,
					TAG_DONE);
		
		if (Selected)
		{
			strncpy (filebuffer, (*fr)->fr_Drawer, MaxSize);
			AddPart (filebuffer, (*fr)->fr_File,   MaxSize);
		}
	}
	
	return(Selected);
}



/*-----------------------------------*/
/* Invoke PlayMF to play a MIDI file */
/*-----------------------------------*/

void PlayMIDI(struct Globals *glob, struct Prefs *pref)
{
	UBYTE CommandString[300];
	UBYTE *init = "";
	BPTR lock;
	BPTR out;
	BOOL CloseOut = FALSE;
	
	kprintf("Calling the dos-command Execute makes AROS crash here. So no automatic start of PlayMF.\n");
	return;

	if (pref->Flags & PREFF_GM)   init = "GM";
	if (pref->Flags & PREFF_GS)   init = "GS";
	if (pref->Flags & PREFF_XG)   init = "XG";
	if (pref->Flags & PREFF_MT32) init = "MT32";
	
	if ((lock = Lock("PlayMF", SHARED_LOCK )))
	{
		UnLock(lock);
		
		sprintf(CommandString,"Run PlayMF \42%s\42 LINK=\42%s\42 %s REPLACE", pref->MIDIFile, pref->Link, init);
	}
	else
	{
		sprintf(CommandString,"Run PlayMF \42%s\42 LINK=\42%s\42 %s REPLACE", pref->MIDIFile, pref->Link, init);
	}

	if ((out = Output()) == NULL)
	{
		if ((out = Open("NIL:", MODE_OLDFILE))) CloseOut = TRUE;
	}

//	if (!Execute( CommandString, NULL, out))
	if (!Execute( "Run PlayMF felicida.mid", NULL, NULL))
	{
		AsyncMessage(glob,LaunchTask, "Unable to launch PlayMF.\nPlease make sure it is located somewhere in your search path.","Damn");
	}

	if (CloseOut) Close(out);
}



/*--------------------------*/
/* Stop PlayMF (if running) */
/*--------------------------*/

void StopMIDI(struct Globals *glob, struct Prefs *pref)
{
	APTR lock;
	struct MidiCluster *clust;
	struct MidiLink *ml;
	
	if ((lock = LockCAMD(CD_Linkages)))
	{
		if ((clust = FindCluster(pref->Link)))
		{
			for (ml = (struct MidiLink*)clust->mcl_Senders.lh_Head ;
				 ml->ml_Node.ln_Succ ;
				 ml = (struct MidiLink*)ml->ml_Node.ln_Succ )
			{
				if (!stricmp("PlayMF Player", ml->ml_MidiNode->mi_Node.ln_Name))
				{
					Signal(ml->ml_MidiNode->mi_SigTask, SIGBREAKF_CTRL_C);
					break;
				}
			}
		}
		UnlockCAMD(lock);
	}
}
