#include <dos/dos.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <utility/hooks.h>
#include <workbench/startup.h>
#include <diskfont/diskfont.h>
#include <workbench/startup.h>
#ifndef NO_POPUPMENU
#include <libraries/pm.h>
#endif
#include <exec/types.h>
#include <dos/dosextens.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/icon_lib.h>
#else
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/diskfont.h>
#ifndef NO_POPUPMENU
#include <proto/pm.h>
#endif
#include <proto/datatypes.h>
#include <proto/wb.h>
#endif

#include <clib/alib_protos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "misc.h"
#include "config.h"
#include "reqlist.h"
#include "myimageclass.h"
#include "patch.h"
#include "imageloader.h"
#include "anim.h"
#include "patternmatcher.h"
#include "cx.h"
#include "sound.h"
#include "randomlogo.h"
#include "application.h"

#include "myimage_protos.h"
#include "config_protos.h"
#include "reqlist_protos.h"

#define ARG_TEMPLATE "HOMEDIR=HOME/K,CYREQ=CR/S,NOREQTOOLS=NR/S,NOSOUND=NS/S,NOPOPUPMENU=NP/S,NOCGX/S,RAPREFSPATH=RAP/K"

#define ARG_HOMEDIR	0
#define ARG_CYREQ		1
#define ARG_REQTOOLS	2
#define ARG_NOSOUND  3
#define ARG_NOPOPUPMENU 4
#define ARG_NOCGX 5
#define ARG_RAPREFSPATH 6
#define NUM_ARGS		7

#define STDRAPREFS "Sys:Prefs/RAPrefsMUI"

static char *verstring = VERSTRING;
static char *authstring = "$AUTH: Jaca/Morgoth/Dreamolers-CAPS & Georg Steger";

static char *progname = "ReqAttack";
static char *portname = "ReqAttack Warp Core";

struct IntuitionBase *IntuitionBase=0;
struct GfxBase *GfxBase=0;
struct Library *DiskfontBase=0;
struct Library *LayersBase=0;
struct UtilityBase *UtilityBase=0;
struct Library *KeymapBase=0;
struct Library *CyberGfxBase=0;
struct Library *IconBase=0;
struct Library *CxBase=0;
struct Library *ReqToolsBase=0;
#ifndef NO_POPUPMENU
struct PopupMenuBase *PopupMenuBase=NULL;
#endif
struct Library *DataTypesBase=0;
struct Library *WorkbenchBase=0;
//struct Library *Picasso=0;

//struct Library *DOpusBase=0;

struct ProgPort progport;

BOOL fromwb,cyreq,reqtools=TRUE,nosound,popupmenu=TRUE,rapalloc=FALSE,cgx=TRUE;

struct SignalSemaphore scmdsem;
struct SignalSemaphore rapsem;

static struct WBStartup *wbmsg;
static BPTR old_currentdir;
static BOOL reset_currentdir;

static char s[256];

char *raprefspath=0;

static LONG Args[NUM_ARGS];

#ifndef __STORMGCC__
	#ifdef __GNUC__
		extern struct WBStartup *_WBenchMsg;
	#endif
#endif

void Cleanup(char *msg)
{
	struct EasyStruct es;
	WORD rc;

	CleanupConfig();
	CleanupAnim();
#ifndef NO_SOUND
	CleanupSound();
#endif
	CleanupRandomLogo();
	CleanupCX();
	CleanupImageLoader();
	CleanupMyImageClass();
	CleanupPatternMatcher();
	CleanupReqList();
	CleanupMyImage();
	CleanupMisc();

	if (rapalloc) FreeVec(raprefspath);

	if (msg)
	{
		if (fromwb && IntuitionBase)
		{
			es.es_StructSize = sizeof(es);
			es.es_Flags = 0;
			es.es_Title = progname;
			es.es_TextFormat = msg;
			es.es_GadgetFormat = "OK";
			EasyRequestArgs(0,&es,0,0);

		} else {

			Printf("ReqAttack: %s\n",msg);

		}

		rc = RETURN_WARN;

	} else {

		rc = RETURN_OK;

	}

	if (progport.port.mp_Node.ln_Name) RemPort(&progport.port);

	if (CyberGfxBase) CloseLibrary(CyberGfxBase);
	if (CxBase) CloseLibrary(CxBase);
	if (KeymapBase) CloseLibrary(KeymapBase);
	if (UtilityBase) CloseLibrary((struct Library *)UtilityBase);
	if (LayersBase) CloseLibrary(LayersBase);
	if (DiskfontBase) CloseLibrary(DiskfontBase);
	if (GfxBase) CloseLibrary((struct Library *)GfxBase);
	if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	if (ReqToolsBase) CloseLibrary(ReqToolsBase);
#ifndef NO_POPUPMENU
	if (PopupMenuBase) CloseLibrary((struct Library *)PopupMenuBase);
#endif
	if (DataTypesBase) CloseLibrary(DataTypesBase);
	if (WorkbenchBase) CloseLibrary(WorkbenchBase);
	if (IconBase) CloseLibrary(IconBase);
	//if (DOpusBase) CloseLibrary(DOpusBase);

	if (reset_currentdir) UnLock(CurrentDir(old_currentdir));

	exit(rc);
}

static void Init(void)
{
	struct MsgPort *mp;

	#if !defined(__STORMGCC__) && !defined(__AROS__)
		#ifdef __GNUC__

	if ((wbmsg = _WBenchMsg))
	{
		fromwb = TRUE;
	}

		#endif
	#endif

	InitSemaphore(&scmdsem);
	InitSemaphore(&rapsem);

	progport.port.mp_Node.ln_Type = NT_MSGPORT;
	progport.port.mp_Node.ln_Pri = -128;
	progport.port.mp_Flags = PA_IGNORE;
	progport.port.mp_SigTask = FindTask(0);
	NewList(&progport.port.mp_MsgList);

	Forbid();
	if ((mp = FindPort(RAPREFS)))
	{
		progport.prefsappactive = TRUE;
	} else {
		progport.prefsappactive = FALSE;
	}

	if ((mp = FindPort(portname)))
	{
		Signal(mp->mp_SigTask,SIGBREAKF_CTRL_C);
	} else {
		progport.port.mp_Node.ln_Name = portname;
		AddPort(&progport.port);
	}
	Permit();

	if (mp) Cleanup(0);

	((struct Process *)FindTask(0))->pr_WindowPtr = (struct Window *)-1;
	SetProgramName(progname);
}

static void OpenLibs(void)
{
	if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39)))
	{
		Cleanup("Can't open intuition.library V39!");
	}

	if (((struct Library *)(IntuitionBase))->lib_Version >= 50) cyreq = TRUE;

	if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0)))
	{
		Cleanup("Can't open graphics.library!");
	}

	if (!(DiskfontBase = OpenLibrary("diskfont.library",0)))
	{
		Cleanup("Can't open diskfont.library!");
	}

	if (!(LayersBase = OpenLibrary("layers.library",0)))
	{
		Cleanup("Can't open layers.library!");
	}

	if (!(UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library",0)))
	{
		Cleanup("Can't open utility.library!");
	}

	if (!(KeymapBase = OpenLibrary("keymap.library",0)))
	{
		Cleanup("Can't open keymap.library!");
	}

	if (!(WorkbenchBase = OpenLibrary("workbench.library",0)))
	{
		Cleanup("Can't open workbench.library!");
	}

	if (!(IconBase = OpenLibrary("icon.library",0)))
	{
		Cleanup("Can't open icon.library!");
	}

	CxBase = OpenLibrary("commodities.library",0);
	DataTypesBase = OpenLibrary("datatypes.library",39);
	//DOpusBase = OpenLibrary("dopus5:libs/dopus5.library",0);

}

static void GetArguments(void)
{
	struct RDArgs *MyArgs;
	struct DiskObject *icon;
	char *name;
	BPTR old_dir,lock;

	if (fromwb)
	{

			old_dir = CurrentDir(wbmsg->sm_ArgList[0].wa_Lock);
			icon = GetDiskObject(wbmsg->sm_ArgList[0].wa_Name);
			CurrentDir(old_dir);

			if (icon)
			{
				if ((name = FindToolType((STRPTR *) icon->do_ToolTypes,"RAPREFSPATH")))
				{
					raprefspath = AllocVec(strlen(name)+1,MEMF_CLEAR|MEMF_PUBLIC|MEMF_REVERSE);
					if (raprefspath) CopyMem(name,raprefspath,strlen(name)+1);
					rapalloc=TRUE;
				}

				if ((name = FindToolType((STRPTR *) icon->do_ToolTypes,"HOMEDIR")))
				{

					if ((lock = Lock(name,ACCESS_READ)))
					{
						old_dir = CurrentDir(lock);
						if (reset_currentdir)
						{
							UnLock(old_dir);
						} else {
							reset_currentdir = TRUE;
							old_currentdir = old_dir;
						}
					}
				}

				if (FindToolType((STRPTR *)icon->do_ToolTypes,"CYREQ"))
				{
					cyreq = TRUE;
				}

				if (FindToolType((STRPTR *)icon->do_ToolTypes,"NOREQTOOLS"))
				{
					reqtools = FALSE;
				}

				if (FindToolType((STRPTR *)icon->do_ToolTypes,"NOSOUND"))
				{
					nosound = TRUE;
				}

				if (FindToolType((STRPTR *)icon->do_ToolTypes,"NOPOPUPMENU"))
				{
					popupmenu = FALSE;
				}

				if (FindToolType((STRPTR *)icon->do_ToolTypes,"NOCGX"))
				{
					cgx = FALSE;
				}

				FreeDiskObject(icon);

			} /* if (icon) */

	} else {

		if (!(MyArgs = ReadArgs(ARG_TEMPLATE,Args,0)))
		{
			Fault(IoErr(),0,s,255);
			Cleanup(s);
		}

		if (Args[ARG_HOMEDIR])
		{
			if ((lock = Lock((char *)Args[ARG_HOMEDIR],ACCESS_READ)))
			{
				old_currentdir = CurrentDir(lock);
				reset_currentdir = TRUE;
			}
		}

		if (Args[ARG_CYREQ])		cyreq = TRUE;
		if (Args[ARG_REQTOOLS])	reqtools = FALSE;
		if (Args[ARG_NOSOUND])	nosound = TRUE;
		if (Args[ARG_NOPOPUPMENU]) popupmenu = FALSE;
		if (Args[ARG_NOCGX]) cgx = FALSE;

		if (Args[ARG_RAPREFSPATH])
		{
			raprefspath = AllocVec(strlen((char *)Args[ARG_RAPREFSPATH])+1,MEMF_CLEAR|MEMF_PUBLIC|MEMF_REVERSE);
			if (raprefspath) CopyMem((ULONG *)Args[ARG_RAPREFSPATH],raprefspath,strlen((char *)Args[ARG_RAPREFSPATH])+1);
			rapalloc=TRUE;
		}

		FreeArgs(MyArgs);

	} /* if (fromwb) else ... */

	if (rapalloc==FALSE) raprefspath = (char *)STDRAPREFS;

	Forbid();
	if (FindPort("CyReq")) cyreq = TRUE;
	Permit();
}

static void MainLoop(void)
{
	ULONG sigs;
	BOOL quitme = FALSE;

	D(bug("\n*** ENTERING MAINLOOP ***\n"));

	while (!quitme)
	{
		D(bug("Mainloop: Waiting for 0x%08lx\n",SIGBREAKF_CTRL_C |
															 imageloadermask |
															 patternmatchermask |
															 configmask | randomlogomask));

		sigs = Wait(SIGBREAKF_CTRL_C |
						imageloadermask |
						patternmatchermask |
						configmask |
						cxmask|randomlogomask);

		D(bug("Mainloop: Wait returned 0x%08lx\n",sigs));

		if (sigs & imageloadermask)
		{
			D(bug("Calling HandleImageLoader\n"));
			HandleImageLoader();
			D(bug("HandleImageLoader returned\n"));
		}

		if (sigs & patternmatchermask)
		{
			D(bug("Calling HandlePatternMatcher\n"));
			HandlePatternMatcher();
			D(bug("HandlePatternMatcher returned\n"));
		}

		if (sigs & configmask)
		{
			D(bug("Calling HandleConfig\n"));
			HandleConfig();
			D(bug("HandleConfig returned\n"));
		}

		if (sigs & cxmask)
		{
			D(bug("Calling HandleCX\n"));
			HandleCX();
			D(bug("HandleCX returned\n"));
		}

		if (sigs & randomlogomask)
		{
			HandleRandomLogo();
		}

		if (sigs & SIGBREAKF_CTRL_C)
		{
			D(bug("*** BREAK received\n"));
			if (RemovePatches())
			{
				quitme = TRUE;
			} else {
				DisplayBeep(0);
			}
		}
	}

}

#if 0
static void Test(void)
{
	struct EasyStruct es =
	{
		sizeof(struct EasyStruct),
		0,
		"SmartCrash 1.1 Copyright © 1995-1996 PitPlane Productions",
		"Exception %lx%s%s%s\n"
		"Task: %08lx  PC: %08lx  SR: %04lx  USP: %08lx  SSP: %08lx\n"
		"D: %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"
		"A: %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"
		"S: %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"
		"S: %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"
		"%.7s: %-.60s\n"
		"%s\n\n",
		"%sSuspend|Suspend&%sFree|E%sxit|%sDebug|Reboot|S%skip"
	};

	EasyRequest(0,&es,0,	1,"A","B","C",
								  1,2,3,4,5,
								  0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
								  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
								  0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
								  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
								  "Hund","Katze",
								  "Kiste",
								  "11 ","22 ","33 ","44 ","55 ","66 ","77 ","88 ","99 ","AA "
								  );
}
#endif

int main (void)
{
	//struct DosLibrary *DBase;

	D(bug("Init              \n"));Init();
	D(bug("OpenLibs          \n"));OpenLibs();
	D(bug("GetArguments      \n"));GetArguments();
	if (reqtools) ReqToolsBase = OpenLibrary("reqtools.library",0);
#ifndef NO_POPUPMENU
	if (popupmenu) PopupMenuBase = (struct PopupMenuBase *)OpenLibrary("popupmenu.library",9);
#endif
	//now enable the wildcard check... (same as MCP's Dos-Wildstar patch)
	if (DOSBase)
	{
		DOSBase->dl_Root->rn_Flags |= RNF_WILDSTAR;
	}

	if (cgx)
	{
		CyberGfxBase = OpenLibrary("cybergraphics.library",41);
	}

	D(bug("InitMisc          \n"));InitMisc();
	D(bug("InitMyImage       \n"));InitMyImage();
	D(bug("InitReqList       \n"));InitReqList();
	D(bug("InitPatternMatcher\n"));InitPatternMatcher();
	D(bug("InitMyImageClass  \n"));InitMyImageClass();
	D(bug("InitImageLoader   \n"));InitImageLoader();
	D(bug("InitCX            \n"));InitCX();
	InitRandomLogo();
	D(bug("Initanim          \n"));InitAnim();
	D(bug("Initconfig        \n"));InitConfig();
#ifndef NO_SOUND
	if (!(nosound))
	{D(bug("Initsound         \n"));InitSound();}
#endif
	D(bug("Installpatches    \n"));InstallPatches();
#ifndef __AROS__
	D(bug("InitApp           \n"));InitApplication();
#endif

/*	Test();*/

	D(bug("MainLoop          \n"));MainLoop();

	Cleanup(0);

	return 0;
}

/*#ifdef __MAXON__*/
void wbmain (struct WBStartup *msg)
{
	BPTR lock;

	wbmsg = msg;
	fromwb = TRUE;

	if ((lock = Lock("PROGDIR:",ACCESS_READ)))
	{
		old_currentdir = CurrentDir(lock);
		reset_currentdir = TRUE;
	}

	main();
}
/*#endif*/
