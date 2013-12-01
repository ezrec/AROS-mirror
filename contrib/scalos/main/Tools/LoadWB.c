// LoadWB.c
// $Date$
// $Revision$

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <string.h>

#include <proto/intuition.h>

// --- Scalos Specific Includes
#include <proto/preferences.h>
#include <scalos/preferences.h>
#include <scalos/scalos.h>

#include <defs.h> // +jmc+
#include <Year.h> // +jmc+


#define DEFAULT_STACKSIZE       32768

#define TEMPLATE		"SP=SCALOSPATH/K,ORIG=ORIGINAL/S,DEBUG=-DEBUG/S,DELAY/S,CLEANUP/S,"\
				"NEWPATH/S,HIDE/S,SIMPLEGELS/S,NOWAIT/S,FORCEPALETTE/S"
#define OPT_SCALOSPATH		0
#define OPT_ORIG		1
#define OPT_DEBUG		2
#define OPT_NOWAIT		8
#define OPT_FORCEPALETTE	9
#define OPT_COUNT		10
#define DEFPATH			"Scalos:"
#define PREFSFILE		"ENV:Scalos/Scalos.prefs"
#define LOADWB			"LoadWB.orig DELAY"
#define SCAARGS			" EMU"
#define FORCEPALETTE		" FORCEPALETTE"


// --- Version String Stuff
const char VERString[] = "\0$VER: LoadWB.Scalos 1.7 (06.10.2008) "
	COMPILER_STRING
	" Copyright 2000" CURRENTYEAR " The Scalos Team [http://scalos.noname.fr]";

#if defined(__GNUC__)
struct DosLibrary *DOSBase;
#else
extern struct DosLibrary *DOSBase;
#endif
struct IntuitionBase *IntuitionBase;
struct Library *PreferencesBase;
struct Library *ScalosBase;

#ifdef __amigaos4__
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
struct PreferencesIFace *IPreferences;
#endif

// --- Function Prototypes
static ULONG NumberOfWindowsOpen(void);


int main(int argc, char *argv[])
{
	struct RDArgs *rdargs = NULL;
	IPTR result[OPT_COUNT];
	char scalospath[300] = DEFPATH;
	static const char *scalosexename = "Scalos";
	int Result = RETURN_OK;

	do	{
		DOSBase = (APTR) OpenLibrary(DOSNAME, 39);
		if (NULL == DOSBase)
			{
			Result = RETURN_FAIL;
			break;
			}
#ifdef __amigaos4__
		IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
		if (NULL == IDOS)
			{
			Result = RETURN_FAIL;
			break;
			}
#endif

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
		if (NULL == IntuitionBase)
			{
			Result = RETURN_FAIL;
			break;
			}
#ifdef __amigaos4__
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			{
			Result = RETURN_FAIL;
			break;
			}
#endif

		// --- Clear The Results Array
		memset(result, 0, sizeof(result));


		// --- Parse The Command Line Arguments
		if(!(rdargs = ReadArgs(TEMPLATE, result, NULL)))
			{
			// --- Display The Error To The User
			PrintFault(IoErr(), NULL);
			Result = RETURN_FAIL;
			break;
			}

		// --- By-pass Scalos And Run Old Workbench
		if(result[OPT_ORIG])
			{
			SystemTags(LOADWB,
				NP_StackSize, DEFAULT_STACKSIZE,
				TAG_END);
			break;
			}

		// --- Force A Scalos Home Path (ignores prefs setting and default path)
		if(result[OPT_SCALOSPATH])
			{
			strcpy(scalospath, (char *)result[OPT_SCALOSPATH]);
			}
		else
			{
			PreferencesBase = OpenLibrary("preferences.library", 0);
#ifdef __amigaos4__
			if (PreferencesBase)
				{
				IPreferences = (struct PreferencesIFace *)GetInterface(
					PreferencesBase, "main", 1, NULL
				);
				if (!IPreferences)
					{
					CloseLibrary(PreferencesBase);
					PreferencesBase = NULL;
					}
				}
#endif
			// --- Attempt To Open Preferences.Library To Load User Set Scalos Location
			if(PreferencesBase)
				{
				struct PrefsHandle *MyPrefs;

				// --- Allocate Memory To Hold Prefs Data In
				MyPrefs = AllocPrefsHandle((STRPTR) scalosexename);

				// --- Load The Main Scalos Prefs
				ReadPrefsHandle(MyPrefs, (STRPTR) PREFSFILE);

				// --- Get The Path To The Scalos Installation
				GetPreferences(MyPrefs, MAKE_ID('M','A','I','N'),
					SCP_PathsHome, scalospath, sizeof(scalospath));

				// --- DeAllocate Memory
				FreePrefsHandle(MyPrefs);

				// --- Clean Up
#ifdef __amigaos4__
				DropInterface((struct Interface *)IPreferences);
#endif
				CloseLibrary(PreferencesBase);
				}
			}

		// --- Shows Debugging Information
		if(result[OPT_DEBUG])
			{
			Printf((STRPTR) "%s\n", (ULONG) VERString);
			}


		// --- Check If Scalos Is Already Running & Exit if it Is
		if((ScalosBase = OpenLibrary(SCALOSNAME, 0)))
			{
			CloseLibrary(ScalosBase);

			Printf("Error: Scalos is already running on your system\n");
			Result = RETURN_WARN;
			break;
			}

		// --- Add Any Additional Command Arguments For Scalos To The Executable Path
		AddPart(scalospath, (STRPTR) scalosexename, sizeof(scalospath) - strlen(SCAARGS));
		strcat(scalospath, SCAARGS);

		if (result[OPT_FORCEPALETTE])
			{
			strcat(scalospath, FORCEPALETTE);
			}

		// --- Start Scalos From Its Home Directory
		// SystemTagList()
		if (RETURN_OK == SystemTags(scalospath, 
			NP_CommandName, (ULONG) "Scalos",
			SYS_Asynch, TRUE,
			SYS_Output, NULL,
			SYS_Input, Open("CON:////Scalos/AUTO/CLOSE/WAIT", MODE_OLDFILE),
			NP_Error, NULL,
			SYS_UserShell, TRUE,
			NP_ConsoleTask, NULL,
			NP_WindowPtr, NULL,
			NP_CurrentDir, NULL,
			NP_StackSize, DEFAULT_STACKSIZE,
			NP_Cli, TRUE,
			TAG_END))
			{
			if (0 == result[OPT_NOWAIT])
				{
				while (NumberOfWindowsOpen() < 2)
					Delay(10);
				}
			}
		else
			{
			// --- Drop Back And Load Original Workbench
			SystemTagList(LOADWB, NULL);
			}
		} while (0);

	// --- Clean-Up
	if (rdargs)
		FreeArgs(rdargs);
#ifdef __amigaos4__
	if (IIntuition)
		DropInterface((struct Interface *)IIntuition);
#endif
	if (IntuitionBase)
		CloseLibrary((struct Library *) IntuitionBase);
#ifdef __amigaos4__
	if (IDOS)
		DropInterface((struct Interface *)IDOS);
#endif
	if (DOSBase)
		CloseLibrary((struct Library *) DOSBase);

	return Result;
}


static ULONG NumberOfWindowsOpen(void)
{
	ULONG winCount = 0;
	struct Screen *defPubScreen;

	defPubScreen = LockPubScreen(NULL);

	if (defPubScreen)
		{
		struct Window *win;

		Forbid();
		for (win = defPubScreen->FirstWindow; win; win = win->NextWindow)
			winCount++;
		Permit();

		UnlockPubScreen(NULL, defPubScreen);
		}

	return winCount;
}

