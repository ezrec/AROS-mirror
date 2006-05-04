
/* playmf.c */

/*=======================================================================

This file is a prototype score player which reads standard MIDI files,
format type 0 or 1.  It conforms to the 1.0 specification for standard 
MIDI files by Dave Oppenheim of Opcode Systems.  It uses both the CAMD and 
RealTime libraries by Roger Dannenberg, Bill Barton, David Joiner, et al.

Original Design and coding by Dan Baker, Commodore Business Machines.

                   Modified in 1998 by Christian Buchner


Some fixes here and there to make it compile under AROS/gcc.
30-1-2001 Kjetil S. Matheussen.

=======================================================================*/

//#define NO_INLINE_STDARG

#include "playmf_includes.h"
#include <aros/64bit.h>

/* AmigaOS/Sasc -> AROS/gcc fixes. */
#include <libcore/compiler.h>
#ifndef __SASC
#  define __stdargs
#  define XPKERRMSGSIZE 100
#endif

/* Version string */
static UBYTE *Version = "$VER: PlayMF 41.0 ("__DATE__")";

#define NORMPRI 25

#define ERRORSTRING_SIZE (2*XPKERRMSGSIZE)


/* Library bases */

struct DosLibrary		*DOSBase;
struct IntuitionBase	*IntuitionBase;
struct GfxBase			*GfxBase;
struct UtilityBase		*UtilityBase;
struct ExecBase			*SysBase;
struct Library			*CamdBase;
struct Library			*RealTimeBase;
struct Library			*XpkBase;


/* Workbench mode? */

BOOL WBMode = FALSE;



/**********    debug macros     ***********/
#define MYDEBUG  1
void kprintf(UBYTE *fmt,...);
void dprintf(UBYTE *fmt,...);
#define DEBTIME 0
#define bug Printf
#if MYDEBUG
#define D(x) (x); if(DEBTIME>0) Delay(DEBTIME);
#else
#define D(x) ;
#endif /* MYDEBUG */
/********** end of debug macros **********/



/*---------------------*/
/* S M F  Header  File */
/*---------------------*/
/* Four-character IDentifier builder*/
#if AROS_BIG_ENDIAN
#  define MakeID(a,b,c,d)  ( (LONG)(a)<<24L | (LONG)(b)<<16L | (c)<<8 | (d) )
#  define LGET(a) a
#  define WGET(a) a
#  define UWGET(a) a
#else
#  define MakeID(a,b,c,d)  ( (LONG)(d)<<24L | (LONG)(c)<<16L | (b)<<8 | (a) )
LONG LGET(LONG a){
  typedef union{
    ULONG a;
    UBYTE b[4];
  }conv;

  conv temp;
  LONG ret;

  temp.a=a;

  ret=MakeID(temp.b[3],temp.b[2],temp.b[1],temp.b[0]);
  return ret;
}
WORD WGET(WORD a){
  typedef union{
    WORD a;
    UBYTE b[2];
  }conv;
  conv temp;

  temp.a=a;

  return (temp.b[0]<<8 | (temp.b[1]));
  
}
UWORD UWGET(UWORD a){
  typedef union{
    UWORD a;
    UBYTE b[2];
  }conv;
  conv temp;

  temp.a=a;

  return (temp.b[0]<<8 | (temp.b[1]));
  
}

#endif

#define MThd MakeID('M','T','h','d')
#define MTrk MakeID('M','T','r','k')

struct SMFHeader
{
	LONG     ChunkID;  /* 4 ASCII characters */
	LONG     VarLeng;
	WORD     Format;
	UWORD    Ntrks;
	WORD     Division;
};


typedef enum
{
	Event_ignore,
	Event_playable,
	Event_sysex,
	Event_tempo,
	Event_trackend
} Eventtype;


struct DecTrack
{
	UWORD  tracknum;   /* number of this track */
	BOOL   trackend;   /* end of track flag */
	ULONG  absdelta;   /* 32-bit delta */
	ULONG  nexclock;   /* storage */
	UBYTE  status;     /* status from file */
	UBYTE  rstatus;    /* running status from track */
	UBYTE  comsize;    /* size of current command */
	UBYTE  d1;         /* data byte 1 */
	UBYTE  d2;         /* data byte 2 */
	UBYTE *endmarker;
	Eventtype eventtype;
	UBYTE *eventptr;
};



struct SysEx
{
	struct MinNode se_node;	/* node for linking */
	ULONG se_size;			/* sysex size */
	UBYTE se_data[0]	;	/* sysex data */
};


/*------------------*/
/* MIDI Header File */
/*------------------*/
#define MAXTRAX 200L
#define MIDIBUFSIZE 512L


/*--------------*/
/*   Globals    */
/*--------------*/

struct Global
{
	UBYTE  trackct;
	UBYTE  donecount;
	
	UBYTE  masterswitch;
	
	ULONG  division;
	ULONG  tempo;
	
	ULONG  lastclock;
	ULONG  abstimeHI, abstimeLO;
	
	struct DecTrack *pDTrack[MAXTRAX];
	UBYTE  *ptrack[MAXTRAX];
	UBYTE  *ptrackend[MAXTRAX];
	
	ULONG  fillclock[2];
	ULONG  fillstate[2];
	ULONG  buftempo[2];
	
	UBYTE  lastRSchan;

	UBYTE *pfillbuf[2];
	
	/* These buffers hold the notes translated */
	/* from the midifile file for playback */
	UBYTE  fillbuf1[MIDIBUFSIZE];
	UBYTE  fillbuf2[MIDIBUFSIZE];
	
	struct MinList SysExList[2];
};


/*------------------------------*/
/* Flags used by LoadAndPlaySMF */
/*------------------------------*/

#define MIDIF_REPLACE ( 1L)
#define MIDIF_GM      ( 2L)
#define MIDIF_GS      ( 4L)
#define MIDIF_XG      ( 8L)
#define MIDIF_MT32    (16L)


UBYTE CommandSize[7]=
{
	2,		/* 80-8f */
	2,		/* 90-9f */
	2,		/* a0-af */
	2,		/* b0-bf */
	1,		/* c0-cf */
	1,		/* d0-df */
	2,		/* e0-ef */
};

UBYTE CommonSize[8]=
{
	0,		/* f0 */
	1,		/* f1 */
	2,		/* f2 */
	1,		/* f3 */
	0,		/* f4 */
	0,		/* f5 */
	0,		/* f6 */
	0,		/* f7 */
};


/*--------------------------------*/
/* Initialisation/Reset sequences */
/*--------------------------------*/

UBYTE GMInit[]=
{
	0xF0,
	0x7E,
	0x7F,
	0x09,
	0x01,
	0xF7,
};


UBYTE GSInit[]=
{
	0xF0,
	0x41,
	0x10,
	0x42,
	0x12,
	0x40,
	0x00,
	0x7F,
	0x00,
	0x41,
	0xF7
};


UBYTE XGInit[]=
{
	0xF0,
	0x7E,
	0x7F,
	0x09,
	0x01,
	0xF7,
	
	0xF0,
	0x43,
	0x10,
	0x4C,
	0x00,
	0x00,
	0x7E,
	0x00,
	0xF7
};


UBYTE MT32Init[]=
{
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41, 0xF7,
	0xB9, 0x00, 0x00,
	0xC9, 0x7F,
	0xB9, 0x0A, 0x40,
	0xB9, 0x5B, 0x40,
	0xB0, 0x00, 0x7F,
	0xC0, 0x00,
	0xB0, 0x0A, 0x40,
	0xB0, 0x5B, 0x40,
	0xB1, 0x00, 0x7F,
	0xC1, 0x44,
	0xB1, 0x0A, 0x36,
	0xB1, 0x5B, 0x40,
	0xB2, 0x00, 0x7F,
	0xC2, 0x30,
	0xB2, 0x0A, 0x36,
	0xB2, 0x5B, 0x40,
	0xB3, 0x00, 0x7F,
	0xC3, 0x5F,
	0xB3, 0x0A, 0x36,
	0xB3, 0x5B, 0x40,
	0xB4, 0x00, 0x7F,
	0xC4, 0x4E,
	0xB4, 0x0A, 0x36,
	0xB4, 0x5B, 0x40,
	0xB5, 0x00, 0x7F,
	0xC5, 0x29,
	0xB5, 0x0A, 0x12,
	0xB5, 0x5B, 0x40,
	0xB6, 0x00, 0x7F,
	0xC6, 0x03,
	0xB6, 0x0A, 0x5B,
	0xB6, 0x5B, 0x40,
	0xB7, 0x00, 0x7F,
	0xC7, 0x6E,
	0xB7, 0x0A, 0x01,
	0xB7, 0x5B, 0x40,
	0xB8, 0x00, 0x7F,
	0xC8, 0x7A,
	0xB8, 0x0A, 0x7F,
	0xB8, 0x5B, 0x40,
	0xB9, 0x64, 0x00,
	0xB9, 0x65, 0x00,
	0xB9, 0x06, 0x0C,
	0xB9, 0x26, 0x00,
	0xB9, 0x64, 0x7F,
	0xB9, 0x65, 0x7F,
	0xB0, 0x64, 0x00,
	0xB0, 0x65, 0x00,
	0xB0, 0x06, 0x0C,
	0xB0, 0x26, 0x00,
	0xB0, 0x64, 0x7F,
	0xB0, 0x65, 0x7F,
	0xB1, 0x64, 0x00,
	0xB1, 0x65, 0x00,
	0xB1, 0x06, 0x0C,
	0xB1, 0x26, 0x00,
	0xB1, 0x64, 0x7F,
	0xB1, 0x65, 0x7F,
	0xB2, 0x64, 0x00,
	0xB2, 0x65, 0x00,
	0xB2, 0x06, 0x0C,
	0xB2, 0x26, 0x00,
	0xB2, 0x64, 0x7F,
	0xB2, 0x65, 0x7F,
	0xB3, 0x64, 0x00,
	0xB3, 0x65, 0x00,
	0xB3, 0x06, 0x0C,
	0xB3, 0x26, 0x00,
	0xB3, 0x64, 0x7F,
	0xB3, 0x65, 0x7F,
	0xB4, 0x64, 0x00,
	0xB4, 0x65, 0x00,
	0xB4, 0x06, 0x0C,
	0xB4, 0x26, 0x00,
	0xB4, 0x64, 0x7F,
	0xB4, 0x65, 0x7F,
	0xB5, 0x64, 0x00,
	0xB5, 0x65, 0x00,
	0xB5, 0x06, 0x0C,
	0xB5, 0x26, 0x00,
	0xB5, 0x64, 0x7F,
	0xB5, 0x65, 0x7F,
	0xB6, 0x64, 0x00,
	0xB6, 0x65, 0x00,
	0xB6, 0x06, 0x0C,
	0xB6, 0x26, 0x00,
	0xB6, 0x64, 0x7F,
	0xB6, 0x65, 0x7F,
	0xB7, 0x64, 0x00,
	0xB7, 0x65, 0x00,
	0xB7, 0x06, 0x0C,
	0xB7, 0x26, 0x00,
	0xB7, 0x64, 0x7F,
	0xB7, 0x65, 0x7F,
	0xB8, 0x64, 0x00,
	0xB8, 0x65, 0x00,
	0xB8, 0x06, 0x0C,
	0xB8, 0x26, 0x00,
	0xB8, 0x64, 0x7F,
	0xB8, 0x65, 0x7F,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x20, 0x04, 0x04, 0x18, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x21, 0x04, 0x04, 0x17, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x22, 0x04, 0x04, 0x16, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x23, 0x04, 0x04, 0x15, 0xF7, 
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x24, 0x04, 0x04, 0x14, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x25, 0x04, 0x04, 0x13, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x26, 0x04, 0x04, 0x12, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x27, 0x04, 0x04, 0x11, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x28, 0x04, 0x04, 0x10, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x29, 0x04, 0x04, 0x0F, 0xF7,
	0xB9, 0x0B, 0x50,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x10, 0x0E, 0x00, 0x22, 0xF7,
	0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x01, 0x31, 0x00, 0x04, 0x35, 0x6A, 0x6B, 0xF7
};

UBYTE ProgramReset[] =
{
	MS_Prog |  0, 0,
	MS_Prog |  1, 0,
	MS_Prog |  2, 0,
	MS_Prog |  3, 0,
	MS_Prog |  4, 0,
	MS_Prog |  5, 0,
	MS_Prog |  6, 0,
	MS_Prog |  7, 0,
	MS_Prog |  8, 0,
	MS_Prog |  9, 0,
	MS_Prog | 10, 0,
	MS_Prog | 11, 0,
	MS_Prog | 12, 0,
	MS_Prog | 13, 0,
	MS_Prog | 14, 0,
	MS_Prog | 15, 0
};

UBYTE ResetAllControllers[] =
{
	MS_Ctrl | 0,  MM_ResetCtrl, 0,
	MS_Ctrl | 1,  MM_ResetCtrl, 0,
	MS_Ctrl | 2,  MM_ResetCtrl, 0,
	MS_Ctrl | 3,  MM_ResetCtrl, 0,
	MS_Ctrl | 4,  MM_ResetCtrl, 0,
	MS_Ctrl | 5,  MM_ResetCtrl, 0,
	MS_Ctrl | 6,  MM_ResetCtrl, 0,
	MS_Ctrl | 7,  MM_ResetCtrl, 0,
	MS_Ctrl | 8,  MM_ResetCtrl, 0,
	MS_Ctrl | 9,  MM_ResetCtrl, 0,
	MS_Ctrl | 10, MM_ResetCtrl, 0,
	MS_Ctrl | 11, MM_ResetCtrl, 0,
	MS_Ctrl | 12, MM_ResetCtrl, 0,
	MS_Ctrl | 13, MM_ResetCtrl, 0,
	MS_Ctrl | 14, MM_ResetCtrl, 0,
	MS_Ctrl | 15, MM_ResetCtrl, 0
};

UBYTE AllNotesOff[] =
{
	MS_Ctrl | 0,  MM_AllOff, 0,
	MS_Ctrl | 1,  MM_AllOff, 0,
	MS_Ctrl | 2,  MM_AllOff, 0,
	MS_Ctrl | 3,  MM_AllOff, 0,
	MS_Ctrl | 4,  MM_AllOff, 0,
	MS_Ctrl | 5,  MM_AllOff, 0,
	MS_Ctrl | 6,  MM_AllOff, 0,
	MS_Ctrl | 7,  MM_AllOff, 0,
	MS_Ctrl | 8,  MM_AllOff, 0,
	MS_Ctrl | 9,  MM_AllOff, 0,
	MS_Ctrl | 10, MM_AllOff, 0,
	MS_Ctrl | 11, MM_AllOff, 0,
	MS_Ctrl | 12, MM_AllOff, 0,
	MS_Ctrl | 13, MM_AllOff, 0,
	MS_Ctrl | 14, MM_AllOff, 0,
	MS_Ctrl | 15, MM_AllOff, 0
};


UBYTE AllSoundsOff[] =
{
	MS_Ctrl | 0,  0x78, 0,
	MS_Ctrl | 1,  0x78, 0,
	MS_Ctrl | 2,  0x78, 0,
	MS_Ctrl | 3,  0x78, 0,
	MS_Ctrl | 4,  0x78, 0,
	MS_Ctrl | 5,  0x78, 0,
	MS_Ctrl | 6,  0x78, 0,
	MS_Ctrl | 7,  0x78, 0,
	MS_Ctrl | 8,  0x78, 0,
	MS_Ctrl | 9,  0x78, 0,
	MS_Ctrl | 10, 0x78, 0,
	MS_Ctrl | 11, 0x78, 0,
	MS_Ctrl | 12, 0x78, 0,
	MS_Ctrl | 13, 0x78, 0,
	MS_Ctrl | 14, 0x78, 0,
	MS_Ctrl | 15, 0x78, 0
};


/*-------------------*/
/*     Prototypes    */
/*-------------------*/
LONG WBInterface(struct Process *MyProc);
ULONG LoadAndPlaySMF(UBYTE *smfname, UBYTE *linkname, ULONG Flags);
ULONG DecodeAndPlaySMF(ULONG smfdatasize, UBYTE *smfdata, UBYTE *linkname, ULONG Flags);
BOOL OpenLibs(void);
void CloseLibs(void);
BOOL LoadFile(char *name, APTR *ptr, ULONG *len, UBYTE *ErrorString, BOOL *Break);

void CollectEvents(	struct Global *glob, struct MidiLink *pMidiLink );
ULONG GetDelta(UBYTE **value);
UBYTE *DecodeEvent(struct Global *glob, UBYTE *ptdata, struct DecTrack *pDT, struct MidiLink *pMidiLink);

LONG __stdargs Message(UBYTE *Msg,UBYTE *Options,...);
LONG __stdargs PrintDOSFault(LONG code, STRPTR header, UBYTE *options);

                            /* AsmSupport.a */
extern ASM void AddAbsTime( REG(a0) ULONG *abstimeLO,
		       REG(a1) ULONG *abstimeHI,
		       REG(d0) ULONG tempo,
		       REG(d1) ULONG lowclock,
		       REG(d2) ULONG division );

extern ASM ULONG CalcFillClock(REG(d0) ULONG abstimeLO,
			       REG(d1) ULONG abstimeHI,
			       REG(d2) ULONG divisor);

#if defined(NO_INLINE_STDARG)
/* Compiler glue: stub functions for camd.library */
struct MidiNode *CreateMidi(Tag tag, ...);
BOOL SetMidiAttrs(struct MidiNode *mi, Tag tag, ...);
struct MidiLink *AddMidiLink(struct MidiNode *mi, LONG type, Tag tag, ...);
BOOL SetPlayerAttrs(struct Player *pi, Tag tag, ...);
struct Player *CreatePlayer(Tag tag, ...);
#endif
                              /* SPrintf.a */
APTR __stdargs SPrintf(char *, const char *, ...);
APTR __stdargs VSprintf(char *, const char *, va_list);

                              /* Filter.c */
extern LONG FileRequester(UBYTE *defaultdir, UBYTE *linkname, ULONG Flags);



/*-----------------*/
/* Shell Interface */
/*-----------------*/

//LONG SAVEDS mymain(void)
int main()
{
	LONG ReturnCode = RETURN_FAIL;
	
	struct	Process *MyProc;
	
	UBYTE *linkname = "debugdriver.out.0";
	UWORD opts;
	ULONG Flags;
	
	BPTR ObjectLock;
	struct FileInfoBlock *fib;
	BOOL AslReq = FALSE;
	
	MyProc = (struct Process*) FindTask(NULL);
	
	if (!MyProc->pr_CLI)
	{
		WBMode = TRUE;
		ReturnCode = WBInterface(MyProc);
	}
	else
	{
		if (OpenLibs())
		{
			/* CLI argument parsing */
			
			struct	RDArgs *RDArgs;
			
			struct	ArgArray
			{
				UBYTE *aa_File;
				UBYTE *aa_Link;
				ULONG  aa_Replace;
				ULONG  aa_GM;
				ULONG  aa_GS;
				ULONG  aa_XG;
				ULONG  aa_MT32;
			} AA = {NULL, NULL, FALSE, FALSE, FALSE, FALSE, FALSE};
			
			static UBYTE	*Template = "FILE,LINK/K,REPLACE/S,GM/S,GS/S,XG/S,MT32/S";
			

			if ((RDArgs=ReadArgs(Template, (LONG *)&AA, 0)))
			{
				ReturnCode = RETURN_ERROR;
				
				Flags = 0;
				
				if (AA.aa_Replace) Flags |= MIDIF_REPLACE;
				
				opts = 0;
				if (AA.aa_GM  ) {opts++; Flags |= MIDIF_GM  ;};
				if (AA.aa_GS  ) {opts++; Flags |= MIDIF_GS  ;};
				if (AA.aa_XG  ) {opts++; Flags |= MIDIF_XG  ;};
				if (AA.aa_MT32) {opts++; Flags |= MIDIF_MT32;};
				if (opts>1)
				{
					Message("Please specify only one of GM, GS, XG and MT32.", NULL);
				}
				else
				{
					if (AA.aa_Link) linkname = AA.aa_Link;

					if (AA.aa_File==NULL) 
					{

						ReturnCode = FileRequester("", linkname, Flags);

					}
					else
					{
						if (!(ObjectLock = Lock(AA.aa_File, SHARED_LOCK)))
						{
							Message("Cannot locate file or directory '%s'.", NULL, AA.aa_File);
						}
						else
						{
							if (!(fib=(struct FileInfoBlock*)AllocDosObject(DOS_FIB, TAG_DONE)))
							{
								Message("No memory for FileInfoBlock.", NULL);
							}
							else
							{
								if (Examine(ObjectLock, fib))
								{
									if ((fib->fib_DirEntryType == ST_USERDIR) ||
										(fib->fib_DirEntryType == ST_LINKDIR) )
									{
										AslReq = TRUE;
									}
								}
								
								FreeDosObject(DOS_FIB,fib);
							}
							UnLock(ObjectLock);
						}
						
						if (AslReq)
						{
							ReturnCode = FileRequester(AA.aa_File, linkname, Flags);
						}
						else
						{
							ReturnCode = LoadAndPlaySMF(AA.aa_File,linkname, Flags);
						}
					}
				}
				
				FreeArgs(RDArgs);
			}
			else
			{

				PrintDOSFault(IoErr(),"PlayMF",NULL);
			}

			CloseLibs();
		}


	}



	return(ReturnCode);
}


/*---------------------*/
/* Workbench Interface */
/*---------------------*/

LONG WBInterface(struct Process *MyProc)
{
	struct WBStartup *wbmsg;
	
	LONG ReturnCode = RETURN_FAIL;
	
	UBYTE *linkname = "debugdriver.out.0";
	ULONG Flags = MIDIF_REPLACE;
	
	WaitPort(&MyProc->pr_MsgPort);
	
	if ((wbmsg = (struct WBStartup*)GetMsg(&MyProc->pr_MsgPort)))
	{
		if (OpenLibs())
		{
			ReturnCode = FileRequester("", linkname, Flags);
			
			CloseLibs();
		}
		Forbid();
		ReplyMsg((struct Message*)wbmsg);
	}
	return(ReturnCode);
}


/*----------------------------*/
/* Load and play the SMF file */
/*----------------------------*/

ULONG LoadAndPlaySMF(UBYTE *smfname, UBYTE *linkname, ULONG Flags)
{
	ULONG ReturnCode = RETURN_FAIL;
	
	struct SMFHeader  *pSMFHeader;
	WORD               w;
	
	ULONG              smfdatasize;
	UBYTE             *smfdata;
	UBYTE ErrorString[XPKERRMSGSIZE];
	
	BOOL Break=FALSE;
	
	BOOL Success=FALSE;
	
	/*-------------------*/
	/* Read the SMF file */
	/*-------------------*/
	
	if (!LoadFile(smfname, (APTR *)&smfdata, &smfdatasize, ErrorString, &Break))
	{
		if (Break)
		{
			if (!WBMode) PrintDOSFault(ERROR_BREAK, NULL, NULL);
		}
		else
		{
			Message("Error loading SMF file: '%s'", NULL, ErrorString);
		}
	}
	else
	{
		pSMFHeader=(struct SMFHeader *)smfdata;
		
		if (pSMFHeader->ChunkID != MThd ) 
		{
			Message("Midifile has unknown header ID.", NULL);
		}
		else
		{
			if (LGET(pSMFHeader->VarLeng) != 6 ) 
			{
				Message("Midifile has unknown header.", NULL);
			}
			else
			{
				Success=TRUE;
				
				if (WGET(pSMFHeader->Format) == 0 ) 
				{
					if (!WBMode) Message("Parsing midifile format type 0.", NULL);
				}
				else
				{
					if (WGET(pSMFHeader->Format) == 1 ) 
					{
						if (!WBMode) Message("Parsing midifile format type 1.", NULL);
					}
					else
					{
						Message("Can't parse this midifile type %ld.", NULL, (ULONG)WGET(pSMFHeader->Format));
						Success=FALSE;
					}
				}
				
				if (Success)
				{
					if(UWGET(pSMFHeader->Ntrks) >MAXTRAX )
					{
						Message("Midifile has more than MAXTRAX (=%ld) tracks.", NULL, (ULONG)MAXTRAX);
					}
					else
					{
						if (!WBMode) Message("Midifile has %ld tracks.", NULL, UWGET(pSMFHeader->Ntrks));
						
						/*--------------------*/
						/* Evaluate time base */
						/*--------------------*/
						
						if (LGET(pSMFHeader->Division) < 0)
						{
							/* Real time: find frame rate */
							w = (LGET(pSMFHeader->Division) >> 8) & 0x007f;
							
							if(w!=24 && w!=25 && w!=29 && w!=30)
							{
								Message("Non-metrical time specified; not MIDI/SMPTE frame rate.", NULL);
							}
							else 
							{
								Message("Non-metrical time; MIDI/SMPTE frame rate.", NULL);
							}
							
							/* Real-time: find seconds resolution */
							w=LGET(pSMFHeader->Division) & 0x007f;
							
							if(w==4)
							{
								Message("Non-metrical time in quarter seconds (MTC resolution).", NULL);
							}
							else
							{
								if(w==8 || w==10 || w==80)
								{
									Message("Non-metrical time in 1/nths of a second (bit resolution).", NULL);
								}
								else
								{
									Message("Non-metrical time in 1/nths of a second.", NULL);
								}
							}
						}
						else
						{
							if (!WBMode) Message("Midifile has 1/%ldth quarter notes per delta tick.", NULL, LGET(pSMFHeader->Division));
							
							/*--------------------------------*/
							/* Now decode and play that beast */
							/*--------------------------------*/
							
							ReturnCode = DecodeAndPlaySMF(smfdatasize, smfdata, linkname, Flags);
						}
					}
				}
			}
		}
		FreeMem(smfdata,smfdatasize);
	}
	return(ReturnCode);
}


/*-------------------------------*/
/* Decode and play the MIDI file */
/*-------------------------------*/

ULONG DecodeAndPlaySMF(ULONG smfdatasize, UBYTE *smfdata, UBYTE *linkname, ULONG Flags)
{
	ULONG ReturnCode = RETURN_FAIL;
	
	struct Global     *glob;
	
	struct SMFHeader  *hdr = (struct SMFHeader *)smfdata;
	
	UBYTE             *pbyte,x;
	LONG               res;
	
	UBYTE             *pData;
	ULONG              sizeDTrack;
	
	ULONG              midiSignal = -1;
	
	struct MidiNode   *pMidiNode;
	struct MidiLink   *pMidiLink;
	
	BOOL               timerr;
	
	ULONG              wakeup;
	
	WORD               oldpri=128;/* Priority to restore */
	struct Task       *mt;        /* Pointer to this task */
	
	UBYTE              ConductorName[50];
	BOOL               Success;
	
	UWORD              tries;
	APTR               RTLock;
	struct Conductor  *ExistingConductor;
	struct Player     *pPlayer;
	
	struct SysEx *se, *nse;
	//ULONG  printedtempo = 0;
	
	BOOL Running, Finished;
	
	if (!(glob = AllocVec(sizeof(struct Global), MEMF_ANY|MEMF_CLEAR)))
	{
		Message("No memory for global variables!", NULL);
	}
	else
	{
		glob->division    = (ULONG)WGET(hdr->Division);
		
		/* Status of $F1 is undefined in Standard MIDI file Spec */
		glob->lastRSchan  = 0xf1;
		glob->tempo       = 500000L;
		
		glob->pfillbuf[0] = glob->fillbuf1;
		glob->pfillbuf[1] = glob->fillbuf2;
		
		NewList((struct List*)&glob->SysExList[0]);
		NewList((struct List*)&glob->SysExList[1]);
		
		/*----------------------*/
		/* Find the MIDI tracks */
		/*----------------------*/
		glob->trackct=0;
		pbyte=smfdata;
		
		while(	(pbyte-smfdata < smfdatasize) &&
				(glob->trackct < MAXTRAX) )
		{
			if	((*pbyte=='M')&&(*(pbyte+1)=='T')&&
				(*(pbyte+2)=='r')&&(*(pbyte+3)=='k'))
			{
				/* End of (previous) track marker */
				if (glob->trackct > 0)
					glob->ptrackend[glob->trackct-1]=pbyte;
				
				if (glob->trackct == UWGET(hdr->Ntrks)) break;
				
				/* Beginning of track */
				glob->ptrack[glob->trackct]=pbyte+8;
				glob->trackct++;
				pbyte+=4;
			}
			else pbyte++;
		}
		
		/* End of track marker */   
		if (glob->trackct > 0)
			glob->ptrackend[glob->trackct-1]=pbyte;
		
		//{
		//	UWORD track;
		//	for ( track=0 ; track<UWGET(hdr->Ntrks)+1 ; track++ )
		//	{
		//		Message("Track %2ld, $%08lx-$%08lx", NULL, (ULONG)track, glob->ptrack[track], glob->ptrackend[track]);
		//	}
		//}
		
		if(glob->trackct != UWGET(hdr->Ntrks))
		{
			Message("Missing tracks! Only %ld tracks found (%ld expected).", NULL, (ULONG)glob->trackct, (ULONG)UWGET(hdr->Ntrks));
		}
		else
		{
			
			/*----------------------------------------------*/
			/* Set up a MidiNode and a MidiLink.  Link the  */
			/* node to the default "out.0" MidiCluster .    */
			/*----------------------------------------------*/
			if (!(pMidiNode=CreateMidi( MIDI_Name,    "PlayMF Player",
										MIDI_MsgQueue, 0L,      /* This is a send-only   */
										MIDI_SysExSize,4096L,   /* MIDI node so no input */
										TAG_END)))              /* buffers are needed.   */
			{
				Message("No memory for MIDI Node!", NULL);
			}
			else
			{
				if (!(pMidiLink=AddMidiLink( pMidiNode, MLTYPE_Sender, 
														MLINK_Comment,  "PlayMF Player Link",
														MLINK_Parse,    TRUE,
														MLINK_Location, linkname,
														TAG_END)))
				{
					Message("No memory for MIDI Link!", NULL);
				}
				else
				{
					/*----------------------------------------*/
					/* Set up DTrack structure for each track */
					/*----------------------------------------*/
					sizeDTrack=glob->trackct*sizeof(struct DecTrack);
					
					if (!(pData=AllocMem( sizeDTrack, MEMF_PUBLIC | MEMF_CLEAR )))
					{
						Message("No memory for work area!", NULL);
					}
					else
					{
						for(x=0;x<glob->trackct;x++)
						{
							glob->pDTrack[x]=(struct DecTrack *)
								(x * sizeof(struct DecTrack) + pData);
							/* add end marker */
							
							glob->pDTrack[x]->tracknum = x+1;
						 	glob->pDTrack[x]->endmarker = glob->ptrackend[x];
						}
						
						
						/* Initialize DecTrack structures */
						for(x=0;x<glob->trackct;x++)
						{  
							/* Takes a pointer to the delta of a raw <MTrk event>, a pointer   */
							/* to a DecTrack decoding structure to store the decoded event     */ 
							/* Returns a pointer to the next raw <MTrk event> in the track     */
							/* or 0 if the track is exhausted.                                 */
							glob->ptrack[x] = DecodeEvent(glob, glob->ptrack[x] , glob->pDTrack[x] , pMidiLink );
						}
						
						/*---------------------------------------------------------------*/ 
						/* Set up a Player and a Conductor to get timing information */
						/*---------------------------------------------------------------*/ 
						if ((midiSignal = AllocSignal(-1L)) == -1)
						{
							Message("Couldn't allocate a signal bit!", NULL);
						}
						else
						{
							strcpy(ConductorName, "PlayMF to ");
							strcat(ConductorName, linkname);
							
							ExistingConductor = NULL;
							
							Success = FALSE;
							
							RTLock = NULL;
							
							for (tries=0 ; tries<10 ; tries++)
							{
								if (!RTLock)
									RTLock = LockRealTime(RT_CONDUCTORS);
								
								if (!RTLock)
								{
									Message("Failed to lock Realtime conductors!", NULL);
									break;
								}
								else
								{
									if ((ExistingConductor = FindConductor(ConductorName)))
									{
										if (!(Flags & MIDIF_REPLACE))
										{
											Message("There is already a PlayMF task\nsending to MIDI link '%s'.", NULL, linkname);
											break;
										}
										else
										{
											struct Player *pl;
											
											for (pl = (struct Player *) ExistingConductor->cdt_Players.mlh_Head ;
												 pl->pl_Link.ln_Succ ;
												 pl = (struct Player *)pl->pl_Link.ln_Succ )
											{
												if (!strcmp("PlayMF Player", pl->pl_Link.ln_Name))
												{
													Forbid();
														if (RTLock) {UnlockRealTime(RTLock); RTLock = NULL;}
														Signal(pl->pl_Task, SIGBREAKF_CTRL_C);
													Permit();
													Delay(10);
													break;
												}
											}
										}
									}
									else
									{
										Success = TRUE;
										break;
									}
								}
							}
							if (RTLock) {UnlockRealTime(RTLock); RTLock = NULL;}
							
							if (tries == 10)
							{
								Message("The other PlayMF task won't go away!", NULL);
							}
							
							if (Success)
							{
								mt=FindTask(NULL);
								
								if (!(pPlayer=CreatePlayer( PLAYER_Name,	"PlayMF Player",
															PLAYER_Conductor,  ConductorName,
															PLAYER_AlarmSigTask, mt,
															PLAYER_AlarmSigBit,midiSignal,
															TAG_END)))
								{
									Message("Can't create a Player!", NULL);
								}
								else
								{
									/*---------------------------------*/
									/* Make sure the clock is stopped. */
									/*---------------------------------*/
									if ((res = SetConductorState( pPlayer, CONDSTATE_STOPPED, 0L )) != 0)
									{
										Message("Couldn't stop conductor!", NULL);
									}
									else
									{
										/*----------------------------------------*/
										/* send GM/GS/XG/MT32 MIDI initialization */
										/*----------------------------------------*/
										
										UBYTE *init = NULL;
										LONG size  = 0;
										UBYTE data;
										
										if (Flags & MIDIF_GM)   {init = GMInit;   size=sizeof(GMInit);  }
										if (Flags & MIDIF_GS)   {init = GSInit;   size=sizeof(GSInit);  }
										if (Flags & MIDIF_XG)   {init = XGInit;   size=sizeof(XGInit);  }
										if (Flags & MIDIF_MT32) {init = MT32Init; size=sizeof(MT32Init);}
										
										if (!init)
										{
											if (!WBMode) Message("Resetting programs and controllers...", NULL);
											
											/* Reset programs */
											ParseMidi(pMidiLink,ProgramReset,sizeof(ProgramReset));
											Delay(5);
											
											/* Reset all controllers */
											ParseMidi(pMidiLink,ResetAllControllers,sizeof(ResetAllControllers));
											Delay(5);
										}
										else
										{
											if (!WBMode) Message("Sending specified MIDI init...", NULL);
											
											while(size > 0)
											{
												data = *init;
												if (data == 0xf0)
												{
													UBYTE *ptr=init;
													ULONG len=1;
													while(*ptr++ != 0xf7) len++;
													
													PutSysEx(pMidiLink, init);
													init+=len; size-=len;
													
													Delay(15);
												}
												else
												{
													if ((data & 0xf0) == 0xb0)
													{
														ParseMidi(pMidiLink, init, 3);
														init+=3; size-=3;
														Delay(3);
													}
													else
													{
														if ((data & 0xf0) == 0xc0)
														{
															ParseMidi(pMidiLink, init, 2);
															init+=2; size-=2;
															Delay(3);
														}
														else
														{
															Message("Something's wrong in init sequence!\nCode: $%02lx", NULL, (ULONG)data);
															break;
														}
													}
												}
											}
											Delay(25);
										}
										
										/*-----------------------------------*/
										/* Transfer first events to A buffer */
										/*-----------------------------------*/
										
										glob->masterswitch=0L;
										CollectEvents(glob, pMidiLink);
										
										/*-----------------------------------------------------*/
										/* Priority Must Be Above Intuition and Graphics Stuff */
										/*-----------------------------------------------------*/ 
										oldpri=SetTaskPri(mt,NORMPRI);
										
										/*------------------------------------*/
										/* and start the RealTime alarm clock */
										/*------------------------------------*/
										if ((res = SetConductorState( pPlayer, CONDSTATE_RUNNING, 0L )) != 0)
										{
										
											Message("Couldn't start conductor!", NULL);
										}
										else
										{
											Running = TRUE;
											Finished = FALSE;
											ReturnCode = RETURN_WARN;
										
											/*-----------------*/
											/* MAIN EVENT LOOP */
											/*-----------------*/   
											while( Running )
											{
												/*---------------*/
												/* Set the alarm */
												/*---------------*/
												timerr = SetPlayerAttrs( pPlayer, 
																		PLAYER_AlarmTime, glob->fillclock[glob->masterswitch],
																		PLAYER_Ready, TRUE,
																		TAG_END );

												
												if(!timerr)
												{
													Message("Couldn't set player attrs!", NULL);
													Running=FALSE;
												}
												
												/*--------------------------*/
												/* Fill the other buffer... */
												/*--------------------------*/
												
												glob->masterswitch ^= 1L;
												CollectEvents(glob, pMidiLink);
												
												/*--------------------------------*/
												/* ...while waiting for the alarm */
												/*--------------------------------*/
												if(timerr)
												{
													wakeup=Wait((1L<< midiSignal) | SIGBREAKF_CTRL_C);
													
													if(wakeup & (1L << midiSignal))
														timerr = FALSE;
													
													if(wakeup & SIGBREAKF_CTRL_C)
													{
														if (!WBMode) PrintDOSFault(ERROR_BREAK, NULL, NULL);
														Running=FALSE;
													}
												}
												
												if (Running)
												{
													/*------------------------*/
													/* Send off one buffer... */
													/*------------------------*/
													
													for ( se = (struct SysEx*) glob->SysExList[glob->masterswitch^1].mlh_Head ;
														  (nse = (struct SysEx*) se->se_node.mln_Succ) ;
														  se = nse )
													{
														//Message("Sending off SysEx msg length: %ld", NULL, se->se_size);
														PutSysEx(pMidiLink, se->se_data);
														Remove((struct Node *)se);
														FreeVec(se);
													}
													
													if(glob->fillstate[glob->masterswitch^1]!=0)
													{
														ParseMidi(pMidiLink,glob->pfillbuf[glob->masterswitch^1],
																	glob->fillstate[glob->masterswitch^1]);
													}
													
													//if (printedtempo != glob->buftempo[glob->masterswitch^1])
													//{
													//	Message("Tempo: %ld BPM", NULL, 60*1000000/glob->buftempo[glob->masterswitch^1]);
													//	printedtempo = glob->buftempo[glob->masterswitch^1];
													//}
													
													/*---------------------*/
													/* So are we finished? */
													/*---------------------*/
													
													if (Finished)
													{
														ReturnCode = RETURN_OK;
														Running=FALSE;
													}
													
													if (glob->donecount >= glob->trackct)
														Finished = TRUE;
												}
											}
											
											
											/* Flush outstanding SysEx queue */
											for ( se = (struct SysEx*) glob->SysExList[0].mlh_Head ;
												  (nse = (struct SysEx*) se->se_node.mln_Succ) ;
												  se = nse )
											{
												Remove((struct Node *)se);
												FreeVec(se);
											}
											
											for ( se = (struct SysEx*) glob->SysExList[1].mlh_Head ;
												  (nse = (struct SysEx*) se->se_node.mln_Succ) ;
												  se = nse )
											{
												Remove((struct Node *)se);
												FreeVec(se);
											}
											
											/* Stop all notes */
											ParseMidi(pMidiLink,AllNotesOff,sizeof(AllNotesOff));
											
											/* Stop all sounds (e.g. pedal-held notes) */
											ParseMidi(pMidiLink,AllSoundsOff,sizeof(AllSoundsOff));
											
											/* Wait until AllNotesOff/AllSoundsOff has been */
											/* truly sent */
											Delay(10);
										}
										
										/* Restore Priority */
										if(oldpri != 128) SetTaskPri(mt,oldpri);
									}
									DeletePlayer(pPlayer);
								}
							}
							FreeSignal(midiSignal);
						}
						FreeMem(pData,sizeDTrack);
					}
					RemoveMidiLink(pMidiLink);
				}
				DeleteMidi(pMidiNode);
			}
		}
		FreeVec(glob);
	}
	return(ReturnCode);
}



/*----------------*/
/* Open Libraries */
/*----------------*/

BOOL OpenLibs(void)
{
	BOOL Success=FALSE;
	
	if ((DOSBase=(struct DosLibrary*)OpenLibrary("dos.library",37L)))
	{
		if ((GfxBase=(struct GfxBase*)OpenLibrary("graphics.library",37L)))
		{
			if ((IntuitionBase=(struct IntuitionBase*)OpenLibrary("intuition.library",37L)))
			{
				if ((UtilityBase=(struct UtilityBase*)OpenLibrary("utility.library",37L)))
				{
					if (!((CamdBase=OpenLibrary("camd.library",0L))))
					{
						Message("This program requires camd.library!", NULL);
					}
					else
					{
						if ((!(RealTimeBase=OpenLibrary("realtime.library",0L))))
						{
							Message("This program requires realtime.library!", NULL);
						}
						else
						{
							Success=TRUE;
						}
					}
				}
			}
		}
	}
	if (!Success) CloseLibs();
	
	return(Success);
}


/*-----------------*/
/* Close Libraries */
/*-----------------*/

void CloseLibs(void)
{
	if (XpkBase)
	{
		CloseLibrary(XpkBase);
		XpkBase=NULL;
	}
	
	if (RealTimeBase)
	{
		CloseLibrary(RealTimeBase);
		RealTimeBase=NULL;
	}
	
	if (CamdBase)
	{
		CloseLibrary(CamdBase);
		CamdBase=NULL;
	}
	
	if (UtilityBase)
	{
		CloseLibrary((struct Library *)UtilityBase);
		UtilityBase=NULL;
	}
	
	if (IntuitionBase)
	{
		CloseLibrary((struct Library*)IntuitionBase);
		IntuitionBase=NULL;
	}
	
	if (GfxBase)
	{
		CloseLibrary((struct Library*)GfxBase);
		GfxBase=NULL;
	}
	
	if (DOSBase)
	{
		CloseLibrary((struct Library*)DOSBase);
		DOSBase=NULL;
	}
}


/*---------------------------------------*/
/* Collect the next bunch of MIDI events */
/*---------------------------------------*/

void CollectEvents(	struct Global *glob, struct MidiLink *pMidiLink )
{
	UWORD track;
	ULONG lowclock;
	UBYTE mswitch = glob->masterswitch;
	ULONG nexttempo = glob->tempo;
	ULONG pos = 0;
	ULONG delta = 0;
	
	if (glob->donecount < glob->trackct)
	{
		lowclock=0xffffffff;
		
		for(track=0;track<glob->trackct;track++)
		{
			if(glob->pDTrack[track]->nexclock < lowclock && glob->ptrack[track])
				lowclock=glob->pDTrack[track]->nexclock;
		}
		
		delta = lowclock - glob->lastclock;
		glob->lastclock = lowclock;
		
		for(track=0;track<glob->trackct;track++)
		{
			struct DecTrack *pDT = glob->pDTrack[track];
			
			if( (pDT->nexclock == lowclock) && glob->ptrack[track])
			{
				do
				{
					/* Transfer event to parse buffer and handle successor */
					
					switch(pDT->eventtype)
					{
						case Event_playable:
						{
							if(pDT->rstatus == glob->lastRSchan)
							{ 
								/* Running status */
								if (pDT->comsize>0) *(glob->pfillbuf[mswitch] + pos++)=pDT->d1;
								if (pDT->comsize>1) *(glob->pfillbuf[mswitch] + pos++)=pDT->d2;
							}
							else 
							{
								/* New status so store status and data bytes */
								*(glob->pfillbuf[mswitch] + pos++)=pDT->status;
								glob->lastRSchan=pDT->status;
								
								if (pDT->comsize>0) *(glob->pfillbuf[mswitch] + pos++)=pDT->d1;
								if (pDT->comsize>1) *(glob->pfillbuf[mswitch] + pos++)=pDT->d2;
	 						}
						}
						break;
						
						case Event_sysex:
						{
							/* Link SysEx into Queue */
							UBYTE *src = pDT->eventptr;
							UBYTE  hdr = *src++;
							struct SysEx *se = NULL;
							
							if (hdr == 0xf0)
							{
								ULONG length = GetDelta(&src);
								
								if ((se = AllocVec(sizeof(struct SysEx)+length+1, MEMF_ANY|MEMF_CLEAR)))
								{
									UBYTE *dst = se->se_data;
									se->se_size = length+1;
									*dst++ = 0xf0;
									while(length--) *dst++ = *src++;
								}
							}
							
							if (hdr == 0xf7)
							{
								ULONG length = GetDelta(&src);
								
								if ((se = AllocVec(sizeof(struct SysEx)+length+1, MEMF_ANY|MEMF_CLEAR)))
								{
									UBYTE *dst = se->se_data;
									se->se_size = length;
									while(length--) *dst++ = *src++;
								}
							}
							
							if (se)
							{
								AddTail( (struct List*)&glob->SysExList[mswitch], (struct Node*)se );
							}
						}
						break;
						
						case Event_tempo:
						{
							UBYTE *meta = pDT->eventptr;
							LONG metalen = GetDelta(&meta);
							
							nexttempo = 0;
							while (metalen-- > 0)
								nexttempo = (nexttempo << 8)|*(meta++);
							
							//Message("Tempo meta-event (%ld BPM) at nexclock %ld!", NULL, 60*1000000/nexttempo, pDT->nexclock);
						}
						break;
						
						case Event_trackend:
						{
							glob->donecount++;
						}
						break;
						
						default:
						{
							Message("Unknown event?! $%02lx", NULL, (ULONG)pDT->eventtype);
						}
						break;
					}
					
					glob->ptrack[track]=DecodeEvent(glob, glob->ptrack[track], pDT, pMidiLink);
					
				} while ( (pDT->absdelta == 0) && glob->ptrack[track]);
		 	}
		}
	}
	
	glob->fillstate[mswitch] = pos;
	glob->buftempo [mswitch] = nexttempo;
	
	AddAbsTime(&glob->abstimeLO,&glob->abstimeHI,glob->tempo,delta,glob->division);
	glob->fillclock[mswitch] = CalcFillClock(glob->abstimeLO,glob->abstimeHI,833);
	
	glob->tempo = nexttempo;
}



/*--------------------------*/  
/* Determine command length */
/*--------------------------*/  

ULONG GetDelta(UBYTE **value)
{
	register ULONG newval = 0;
	register UWORD x;
	register UBYTE dat;
	
	for(x=0 ; x<4 ; x++)
	{
		dat = *((*value)++);
		newval = newval<<7;
		newval |= dat & 0x7f;
		if (dat < 0x80) break;
	}
	return(newval);
}


/*--------------------------------------------------*/  
/* Translate from raw track data to a decoded event */
/*--------------------------------------------------*/  

UBYTE *DecodeEvent(struct Global *glob, UBYTE *ptdata, struct DecTrack *pDT, struct MidiLink *pMidiLink)
{
	ULONG length;
	BOOL skipit;
	
	UBYTE status;
	UBYTE data;
	UBYTE comsize;
	
	pDT->absdelta = 0L;
	
	/* is this track all used up? */
	if ( pDT->trackend ) return(NULL);
	
	if ( ptdata >= pDT->endmarker )
	{
		Message("Warning: missing proper track end marker in track %ld.", NULL, (ULONG)pDT->tracknum);
		
		pDT->eventptr = ptdata;
		pDT->eventtype = Event_trackend;
		pDT->trackend = TRUE;
		return(ptdata);
	}
	
	skipit=FALSE;
	do
	{
		/* Decode delta */
		ULONG delta = GetDelta(&ptdata);
		pDT->absdelta += delta;
		pDT->nexclock += delta;
		
		pDT->eventtype = Event_ignore;
		
		data = *ptdata;
		
		if(data & 0x80) /* Event with status ($80-$FF): decode new status */  
		{
			ptdata++;
			
			if (data < 0xf0)	/* "Normal" events? */
			{
				status  = data;
				comsize = CommandSize[(data&0x7f)>>4];
				
				pDT->status  = status;
				pDT->comsize = comsize;
				pDT->rstatus = 0;	 /* No running status was used */
				
				pDT->eventtype = Event_playable;
				skipit=FALSE;
			}
			else
			{
				if (data < 0xf8)	/* System Common Event? */
				{
					status  = data;
					comsize = CommonSize[status-0xf0];
					
					skipit=TRUE;
					
					if (status==0xf0 ||
						status==0xf7 )   /* It's a sysex event */
					{
						pDT->eventptr = ptdata-1;
						pDT->eventtype = Event_sysex;
						skipit = FALSE;
						
						length=GetDelta(&ptdata);
						ptdata+=length;
					}
				}
				else
				{
					status  = data;
					comsize = 0;
					
					skipit=TRUE;
					
					if (data == 0xff)	/* It's a meta event ($ff) */
					{
						UBYTE metatype;
						
						metatype=*(ptdata++);
						
						if (metatype==0x2F)		/* track end marker */
						{
							pDT->eventptr = ptdata;
							pDT->eventtype = Event_trackend;
							pDT->trackend = TRUE;
							skipit=FALSE;
						}
						else
						{
							if (metatype==0x51)	/* Tempo change */
							{
								pDT->eventptr = ptdata;
								pDT->eventtype = Event_tempo;
								skipit=FALSE;
							}
							
							length = GetDelta(&ptdata);
							ptdata+=length;
						}
					}
				}
			}
		}
		else /* Event without status ($00-$7F): use running status */
		{
			status  = pDT->status;
			
			if ( status==0 )
			{
				Message("Warning: Data bytes without initial status in track %ld.", NULL, (ULONG)pDT->tracknum);
				comsize = 1;
				skipit = TRUE;
			}
			else
			{
				skipit=FALSE;
				comsize = pDT->comsize;
				pDT->rstatus = status;
				pDT->eventtype = Event_playable;
			}
		}
		
		if (comsize > 0) pDT->d1 = *ptdata++;
			else pDT->d1 = 0;
		if (comsize > 1) pDT->d2 = *ptdata++;
			else pDT->d2 = 0;
	}
	while (skipit);
	
	return(ptdata);
}


/*------------------------------------------------------------*/
/* Load a file to memory, XPK support - taken from GMPlay 1.3 */
/*------------------------------------------------------------*/

BOOL LoadFile(char *name, APTR *ptr, ULONG *len, UBYTE *ErrorString, BOOL *Break)
{
  BOOL Success=FALSE;
  
  BPTR lock;
  struct FileInfoBlock *fib;
  BPTR fh;

  LONG Err=0;

  UBYTE ErrString[XPKERRMSGSIZE];
  
  SetIoErr(0);
  
  if ((lock=Lock(name,SHARED_LOCK)))
    {
      
      if (!(fib=(struct FileInfoBlock*)AllocDosObject(DOS_FIB, TAG_DONE)))
	{
	  SetIoErr(ERROR_NO_FREE_STORE);
	}
      else
	{
	  if (Examine(lock, fib))
	    {
	      *len=fib->fib_Size;
		  
	      if (!(*ptr=AllocMem(*len, MEMF_ANY|MEMF_CLEAR)))
		{
		  SetIoErr(ERROR_NO_FREE_STORE);
		}
	      else
		{
		  if ((fh=Open(name,MODE_OLDFILE)))
		    {
		      if (Read(fh,*ptr,*len) == *len)
			{
			  Success=TRUE;
			}
		      Close(fh);
		      
		      if (CheckSignal(SIGBREAKF_CTRL_C)) {*Break=TRUE; Success=FALSE;}
		    }
		  
		  if (!Success)
		    {
		      FreeMem(*ptr, *len); *ptr=NULL; *len=0;
		    }
		}
	    }
	  FreeDosObject(DOS_FIB,fib);
	}
    }
  UnLock(lock);

  
  if (!Success)
    {
      *ptr=NULL;
      *len=0;
      
      if (!(*Break))
	{
	  if ((Err = IoErr()))
	    {
	      if (Err > 0)
		{
		  Fault(Err, "DOS error: ", ErrorString, ERRORSTRING_SIZE);
		}
	      if (Err < 0)
		{
		  sprintf(ErrorString, "XPK error: %s",ErrString);
		}
	    }
	  else
	    {
	      sprintf(ErrorString, "no error set");
	    }
	}
    }
  
  return(Success);
}



/* Show a message to the user */

LONG __stdargs Message(UBYTE *Msg,UBYTE *Options,...)
{
	LONG retval;
	
	BOOL req = FALSE;
	
	va_list Arg;
	va_start(Arg,Options);
	
	// if (Options) if (strchr(Options,'|')) req = TRUE;
	if (Options) req = TRUE;
	
	if (IntuitionBase && (WBMode || req))
	{
		struct EasyStruct Req={sizeof(struct EasyStruct),0,"Piano Meter",0, NULL};
		
		if (!Options) Options = "I see";
		
		Req.es_TextFormat=Msg;
		Req.es_GadgetFormat=Options;
		
		retval=EasyRequestArgs(NULL,&Req,0,Arg);
	}
	else
	{
		if (DOSBase)
		{
			VPrintf(Msg,Arg);
			Printf("\n");
			
			retval=0;
		}
	}
	va_end(Arg);
	
	return(retval);
}


/* Show a DOS Fault string to the user */

LONG __stdargs PrintDOSFault(LONG code, STRPTR header, UBYTE *Options)
{
	LONG retval;
	
	BOOL req = FALSE;
	
	// if (Options) if (strchr(Options,'|')) req = TRUE;
	if (Options) req = TRUE;
	
	if (IntuitionBase && (WBMode || req))
	{
		struct EasyStruct Req={sizeof(struct EasyStruct),0,"Piano Meter",0, NULL};
		struct TagItem emptytags[1] = {{TAG_DONE}};
		
		UBYTE buffer[128];
		Fault(code, header, buffer, sizeof(buffer));
		
		if (!Options) Options = "I see";
		
		Req.es_TextFormat=buffer;
		Req.es_GadgetFormat=Options;
		
		retval=EasyRequestArgs(NULL,&Req,0,emptytags);
	}
	else
	{
		PrintFault(code, header);
		
		retval=0;
	}
	
	return (retval);
}



/*----------------*/
/* Stub funcitons */
/*----------------*/

#if defined(NO_INLINE_STDARG)
/* Compiler glue: stub functions for camd.library */
struct MidiNode *CreateMidi(Tag tag, ...)
{
	return CreateMidiA((struct TagItem *)&tag );
}

BOOL SetMidiAttrs(struct MidiNode *mi, Tag tag, ...)
{
	return SetMidiAttrsA(mi, (struct TagItem *)&tag );
}

struct MidiLink *AddMidiLink(struct MidiNode *mi, LONG type, Tag tag, ...)
{
	return AddMidiLinkA(mi, type, (struct TagItem *)&tag );
}

/* Compiler glue: stub functions for realtime.library */
BOOL SetPlayerAttrs(struct Player *pi, Tag tag, ...)
{
	return SetPlayerAttrsA(pi, (struct TagItem *)&tag );
}

struct Player *CreatePlayer(Tag tag, ...)
{
	return CreatePlayerA( (struct TagItem *)&tag );
}
#endif


                            /* AsmSupport.a */

ASM void AddAbsTime( REG(a0) ULONG *abstimeLO,
		       REG(a1) ULONG *abstimeHI,
		       REG(d0) ULONG tempo,
		       REG(d1) ULONG lowclock,
		       REG(d2) ULONG division )
{
  QUAD temp0,temp1,temp2,temp3,temp4=1;
  temp1=tempo;
  temp2=lowclock;
  temp3=division;

  temp0=SMult64(tempo,lowclock);

  temp4=temp0/temp3;

  *abstimeLO+=LOW32OF64(temp4);
  *abstimeHI+=HIGH32OF64(temp4);
}

ASM ULONG CalcFillClock(REG(d0) ULONG abstimeLO,
			       REG(d1) ULONG abstimeHI,
			       REG(d2) ULONG divisor)
{
  QUAD temp1=abstimeHI;
  QUAD temp2=divisor;

  temp1=temp1<<32;
  temp1+=abstimeLO;

  return (ULONG)temp1/temp2;
}



#ifdef __SASC
void __regargs __chkabort(void) {}      /* Disable SAS CTRL-C checking. */
#else
#ifdef LATTICE
void chkabort(void) {}                  /* Disable LATTICE CTRL-C checking */
#endif
#endif

