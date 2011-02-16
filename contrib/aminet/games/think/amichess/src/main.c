#ifdef __AROS__
#include <libraries/mui.h>
#include <workbench/workbench.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/muimaster.h>
#include <proto/timer.h>
#include <proto/intuition.h>
#else
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/icon_protos.h>
#include <clib/intuition_protos.h>
#include <clib/muimaster_protos.h>
#include <clib/timer_protos.h>
#endif

#include <datatypes/pictureclass.h>
#include <datatypes/soundclass.h>
#include <dos/dostags.h>
#include <graphics/scale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "book.h"
#include "common.h"

extern struct MUI_CustomClass *MUI_Chess_Class;

Object *mui_app;

struct DiskObject *PrgIcon;
struct Screen *wbscreen;
ULONG pix_x;
ULONG col_white,col_black;
Object *piecesgfx;
struct BitMap *pieces_bm;
APTR pieces_mask;
char *pieces_folder;
Object *board_light;
struct BitMap *board_light_bm;
Object *board_dark;
struct BitMap *board_dark_bm;

ULONG snd_signal,snd_sigmask;
Object *snd_move;
Object *snd_mate;
Object *snd_smate;
Object *snd_draw;
Object *snd_takes,*snd_hit;
Object *snd_check;
Object *snd_00,*snd_000,*snd_castle;
Object *snd_P;
Object *snd_N;
Object *snd_B;
Object *snd_R;
Object *snd_Q;
Object *snd_K;
Object *snd_fielda[8],*snd_field1[8];
BOOL snd_playing;

ULONG voice,promotepiece;

void CleanupModules();
void InitModules();

struct
{
char *pubscreen;
} args;

#ifdef __GNUC__
struct Library *MUIMasterBase;

static void InitLibs()
{
MUIMasterBase=OpenLibrary((UBYTE *)"muimaster.library",19);
}

static void CloseLibs()
{
CloseLibrary(MUIMasterBase);
}

#endif

#ifdef __AROS__
Object *DoSuperNew(struct IClass *cl,Object *obj,Tag tag1,...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR)DoSuperMethod(cl,obj,OM_NEW,AROS_SLOWSTACKTAGS_ARG(tag1),0);
    AROS_SLOWSTACKTAGS_POST
}
#else
Object *DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...)
{
return(Object *)DoSuperMethod(cl,obj,OM_NEW,&tag1,0);
}
#endif

static void FreePieces()
{
if(piecesgfx)
	{
	DisposeDTObject(piecesgfx);
	piecesgfx=0;
	pieces_bm=0;
	pieces_mask=0;
	}
}

void LoadPieces(char *name)
{
BPTR lock;
FreePieces();
if(lock=Lock(pieces_folder,SHARED_LOCK))
	{
	BPTR oldlock=CurrentDir(lock);

	if(piecesgfx=NewDTObject((void *)name,DTA_GroupID,GID_PICTURE,PDTA_DestMode,PMODE_V43,PDTA_Remap,1,PDTA_Screen,wbscreen,PDTA_FreeSourceBitMap,1,PDTA_UseFriendBitMap,1,TAG_END))
		{
		if(DoDTMethod(piecesgfx,0,0,DTM_PROCLAYOUT,0,1))
			{
			GetDTAttrs(piecesgfx,PDTA_DestBitMap,&pieces_bm,PDTA_MaskPlane,&pieces_mask,TAG_END);

			}
		}

	CurrentDir(oldlock);
	UnLock(lock);
	}
}

static void ScaleBitMaps(struct BitMap *src,UWORD srcw,UWORD srch,struct BitMap *dest,UWORD destw,UWORD desth)
{
struct BitScaleArgs bsa;
bsa.bsa_SrcX=0;
bsa.bsa_SrcY=0;
bsa.bsa_SrcWidth=srcw;
bsa.bsa_SrcHeight=srch;
bsa.bsa_XSrcFactor=srcw;
bsa.bsa_YSrcFactor=srch;
bsa.bsa_DestX=0;
bsa.bsa_DestY=0;
bsa.bsa_DestWidth=destw;
bsa.bsa_DestHeight=desth;
bsa.bsa_XDestFactor=destw;
bsa.bsa_YDestFactor=desth;
bsa.bsa_SrcBitMap=src;
bsa.bsa_DestBitMap=dest;
bsa.bsa_Flags=0;
bsa.bsa_XDDA=0;
bsa.bsa_YDDA=0;
bsa.bsa_Reserved1=0;
bsa.bsa_Reserved2=0;
BitMapScale(&bsa);
}

static void FreeBoard()
{
if(board_light)
	{
	DisposeDTObject(board_light);
	board_light=0;
	}
if(board_light_bm)
	{
	FreeBitMap(board_light_bm);
	board_light_bm=0;
	}
if(board_dark)
	{
	DisposeDTObject(board_dark);
	board_dark=0;
	}
if(board_dark)
	{
	FreeBitMap(board_dark_bm);
	board_dark_bm=0;
	}
}

void LoadBoard(char *name)
{
BPTR lock1;
FreeBoard();
if(lock1=Lock("PROGDIR:Boards",SHARED_LOCK))
	{
	BPTR oldlock=CurrentDir(lock1);
	BPTR lock2;
	if(lock2=Lock(name,SHARED_LOCK))
		{
		struct BitMap *bm,*wbm=wbscreen->RastPort.BitMap;
		struct BitMapHeader *bh;
		CurrentDir(lock2);
		if(board_light=NewDTObject("Light",DTA_GroupID,GID_PICTURE,PDTA_DestMode,PMODE_V43,PDTA_Remap,1,PDTA_Screen,wbscreen,PDTA_FreeSourceBitMap,1,PDTA_UseFriendBitMap,1,TAG_END))
			{
			if(DoDTMethod(board_light,0,0,DTM_PROCLAYOUT,0,1))
				{
				GetDTAttrs(board_light,PDTA_BitMapHeader,&bh,PDTA_DestBitMap,&bm,TAG_END);
				if(board_light_bm=AllocBitMap(pix_x,pix_x,GetBitMapAttr(wbm,BMA_DEPTH),BMF_MINPLANES,wbm)) ScaleBitMaps(bm,bh->bmh_Width,bh->bmh_Height,board_light_bm,pix_x,pix_x);
				}
			}
		if(board_dark=NewDTObject("Dark",DTA_GroupID,GID_PICTURE,PDTA_DestMode,PMODE_V43,PDTA_Remap,1,PDTA_Screen,wbscreen,PDTA_FreeSourceBitMap,1,PDTA_UseFriendBitMap,1,TAG_END))
			{
			if(DoDTMethod(board_dark,0,0,DTM_PROCLAYOUT,0,1))
				{
				GetDTAttrs(board_dark,PDTA_BitMapHeader,&bh,PDTA_DestBitMap,&bm,TAG_END);
				if(board_dark_bm=AllocBitMap(pix_x,pix_x,GetBitMapAttr(wbm,BMA_DEPTH),BMF_MINPLANES,wbm)) ScaleBitMaps(bm,bh->bmh_Width,bh->bmh_Height,board_dark_bm,pix_x,pix_x);
				}
			}
		UnLock(lock2);
		}
	CurrentDir(oldlock);
	UnLock(lock1);
	}
}

static void FreeSound()
{
ULONG i;
DisposeDTObject(snd_move);
DisposeDTObject(snd_mate);
DisposeDTObject(snd_smate);
DisposeDTObject(snd_draw);
DisposeDTObject(snd_takes);
DisposeDTObject(snd_hit);
DisposeDTObject(snd_check);
DisposeDTObject(snd_00);
DisposeDTObject(snd_000);
DisposeDTObject(snd_castle);
DisposeDTObject(snd_P);
DisposeDTObject(snd_N);
DisposeDTObject(snd_B);
DisposeDTObject(snd_R);
DisposeDTObject(snd_Q);
DisposeDTObject(snd_K);
for(i=0;i<8;i++)
	{
	DisposeObject(snd_fielda[i]);
	DisposeObject(snd_field1[i]);
	}
if(snd_signal!=~0) FreeSignal(snd_signal);
}

static void LoadSound()
{
BPTR lock;
if(lock=Lock("PROGDIR:Sounds",SHARED_LOCK))
	{
	ULONG i;
	BPTR oldlock=CurrentDir(lock);
	struct TagItem tags[5];
	char text[10];
	tags[0].ti_Tag=DTA_GroupID;
	tags[0].ti_Data=GID_SOUND;
	tags[1].ti_Tag=SDTA_SignalTask;
	tags[1].ti_Data=(IPTR)FindTask(0);
	tags[2].ti_Tag=SDTA_SignalBit;
	tags[2].ti_Data=snd_sigmask;
	tags[3].ti_Tag=SDTA_Volume;
	tags[3].ti_Data=64;
	tags[4].ti_Tag=TAG_END;
	snd_move=NewDTObjectA("Tap",tags);
	snd_mate=NewDTObjectA("Mate",tags);
	snd_smate=NewDTObjectA("Stalemate",tags);
	snd_draw=NewDTObjectA("Draw",tags);
	snd_takes=NewDTObjectA("Takes",tags);
	snd_hit=NewDTObjectA("Hit",tags);
	snd_check=NewDTObjectA("Check",tags);
	snd_00=NewDTObjectA("O-O",tags);
	snd_000=NewDTObjectA("O-O-O",tags);
	snd_castle=NewDTObjectA("Castling",tags);
	snd_P=NewDTObjectA("Pawn",tags);
	snd_N=NewDTObjectA("Knight",tags);
	snd_B=NewDTObjectA("Bishop",tags);
	snd_R=NewDTObjectA("Rook",tags);
	snd_Q=NewDTObjectA("Queen",tags);
	snd_K=NewDTObjectA("King",tags);
	for(i=0;i<8;i++)
		{
		sprintf(text,"%c",'a'+i);
		snd_fielda[i]=NewDTObjectA(text,tags);
		sprintf(text,"%c",'1'+i);
		snd_field1[i]=NewDTObjectA(text,tags);
		}
	CurrentDir(oldlock);
	UnLock(lock);
	}
}

void PlaySound(Object *snd,BOOL wait)
{
if(snd)
	{
	DoDTMethod(snd,0,0,DTM_TRIGGER,0,STM_PLAY,0);
	snd_playing = TRUE;
	if(wait) Wait(snd_sigmask);
	}
}

void MoveSound(char *move)
{
if(voice)
	{
	PlaySound(snd_move,0);
	Delay(5);
	if(!strcmp(move,"O-O")) PlaySound(snd_00,1);
	else if(!strcmp(move,"O-O-O")) PlaySound(snd_000,1);
	else
		{
		ULONG i=0;
		char c;
		while(c=*move)
			{
			if(c=='N') PlaySound(snd_N,1);
			else if(c=='B') PlaySound(snd_B,1);
			else if(c=='R') PlaySound(snd_R,1);
			else if(c=='Q') PlaySound(snd_Q,1);
			else if(c=='K') PlaySound(snd_K,1);
			else if(c>='a'&&c<='h')
				{
				if(!i) PlaySound(snd_P,1);
				PlaySound(snd_fielda[c-'a'],1);
				}
			else if(c>='1'&&c<='8') PlaySound(snd_field1[c-'1'],1);
			else if(c=='x') PlaySound(snd_takes,1);
			else if(c=='+') PlaySound(snd_check,1);
			else if(c=='#') PlaySound(snd_mate,1);
			move++;
			i++;
			}
		}
	}
else
	{
	if(strstr(move,"O-O")) PlaySound(snd_castle,1);
	else if(strstr(move,"x")) PlaySound(snd_hit,1);
	else
		{
		PlaySound(snd_move,0);
		Delay(5);
		}
	if(strstr(move,"+")) PlaySound(snd_check,1);
	}
}

void Ende(void)
{
Object *snd;
if(snd=NewDTObject("PROGDIR:Sounds/End",DTA_GroupID,GID_SOUND,SDTA_SignalTask,FindTask(0),SDTA_SignalBit,snd_sigmask,TAG_END)) PlaySound(snd,1);
if(mui_app) MUI_DisposeObject(mui_app);
ReleasePen(wbscreen->ViewPort.ColorMap,col_white);
ReleasePen(wbscreen->ViewPort.ColorMap,col_black);
if(wbscreen) UnlockPubScreen(0,wbscreen);
FreeBoard();
FreePieces();
FreeSound();
if(PrgIcon) FreeDiskObject(PrgIcon);
#ifdef __GNUC__
CleanupModules();
CloseLibs();
#endif
DisposeDTObject(snd);
}

static void InitGUI()
{
mui_app=NewObject(MUI_Chess_Class->mcc_Class,0,TAG_END);
if(!mui_app) exit(20);
}

int main(int argc,char *argv[])
{
int compilebook=0;
time_t now;
Object *snd;
ULONG signals = 0;

atexit(Ende);
#ifdef __GNUC__
InitLibs();
InitModules();
#endif

if(argc>0)
	{
	struct RDArgs *rd;
	if(rd=ReadArgs("PUBSCREEN/K",(long *)&args,0))
		{
		FreeArgs(rd);
		}
	}
else
	{
	if(PrgIcon=GetDiskObject("PROGDIR:AmiChess"))
		{
		char *val;
		if(val=FindToolType(PrgIcon->do_ToolTypes,"PUBSCREEN")) args.pubscreen=val;
		}
	}
if(!(wbscreen=LockPubScreen(args.pubscreen)))
	{
	if(!(wbscreen=LockPubScreen(0))) exit(20);
	}
if(wbscreen->Width>=1024)
	{
	pieces_folder="PROGDIR:Pieces/1024";
	pix_x=72;
	}
else
	{
	pieces_folder="PROGDIR:Pieces/800";
	pix_x=56;
	}
snd_signal=AllocSignal(~0);
snd_sigmask=1<<snd_signal;
if(snd=NewDTObject("PROGDIR:Sounds/Chess",DTA_GroupID,GID_SOUND,SDTA_SignalTask,FindTask(0),SDTA_SignalBit,snd_sigmask,TAG_END)) PlaySound(snd,0);
col_white=ObtainBestPen(wbscreen->ViewPort.ColorMap,0xFFFFFFFF,0xFFFFFFFF,0xB0B0B0B0,0);
col_black=ObtainBestPen(wbscreen->ViewPort.ColorMap,0x90909090,0x48484848,0x00000000,0);
LoadBoard("Default");
LoadPieces("Default");
LoadSound();
InitGUI();

time(&now);
srand((unsigned) now);

flags=0;
SETFLAG(flags,POST);
Initialize();

bookmode=BOOKPREFER;
bookfirstlast=3;
SETFLAG(flags,USENULL);
SearchTime=5;

DoMethod(mui_app,MUIM_Chess_WinOpen);
DoMethod(mui_app,MUIM_Chess_ShowBoard);

if (snd_playing) Wait(snd_sigmask);

DisposeDTObject(snd);
for(;;)
{
	if(DoMethod(mui_app,MUIM_Application_NewInput,&signals)==MUIV_Application_ReturnID_Quit)
	{
	    break;
	}

	if(flags&AUTOPLAY)
	{
	    DoMethod(mui_app,MUIM_Chess_SwapSides);
	    signals = CheckSignal(signals);
	}
	else if(signals) signals=Wait(signals);
}
return 0;
}

#ifdef __AROS__
struct Device *TimerBase;
#else
APTR TimerBase;
#endif

#ifdef __cplusplus

void gettimeofday(struct timeval *t)
{
struct timerequest io;
OpenDevice(TIMERNAME,UNIT_MICROHZ,&io,0);
TimerBase=io.io_Device;
GetSysTime(t);
CloseDevice(&io);
}

#else

void gettimeofday(struct timeval *t)
{
struct timerequest io;
#ifdef __AROS__
io.tr_node.io_Message.mn_Length = sizeof(io);
OpenDevice(TIMERNAME,UNIT_VBLANK,&io.tr_node,0);
#else
OpenDevice(TIMERNAME,UNIT_MICROHZ,&io.tr_node,0);
#endif
TimerBase=io.tr_node.io_Device;
GetSysTime(t);
CloseDevice(&io.tr_node);
}

#endif

#ifdef __MIXEDBINARY__

ULONG FromPPCDoMethod(ULONG MethodID,...)
{
return DoMethodA(mui_app,(Msg)&MethodID); 
}

void FromPPCexit(int val)
{
exit(val);
}

#endif
