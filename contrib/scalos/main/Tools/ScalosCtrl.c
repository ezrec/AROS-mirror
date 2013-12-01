// ScalosCtrl.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/exec.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <intuition/classusr.h>
#include <dos/dos.h>
#include <libraries/dos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/scalos.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/utility_protos.h>
#include <clib/scalos_protos.h>

#include <scalos/scalos.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <defs.h> // +jmc+
#include <Year.h> // +jmc+

struct ScalosBase *ScalosBase;
extern struct DosLibrary *DOSBase;

#ifdef __amigaos4__
struct ScalosIFace *IScalos;
#endif

CONST_STRPTR Ver = "\0$VER:ScalosCtrl 40.16 (2.1.2004) "
	COMPILER_STRING
	" ©2004" CURRENTYEAR " The Scalos Team";



int main(int argc, char *argv[])
{
	CONST_STRPTR ArgTemplate = 
		"SS=DEFSTACKSIZE/N/K,"
		"CWB=CLOSEWBDISABLED/N/K,"
		"SPL=SPLASHENABLED/N/K,"
		"TTE=TOOLTIPENABLED/N/K,"
		"TTD=TOOLTIPDELAY/N/K,"
		"OD=OLDDRAGICONMODE/N/K,"
		"SB=STATUSBAR/N/K,"
		"STW=STRIPEDTEXTWINDOWS/N/K,"
		"DDC=DRAGDROPCOUNTDISPLAY/N/K,"
		"WDM=WINDOWDROPMARKMODE/N/K,"
		"CBS=COPYBUFFSIZE/N/K,"
		"QUIET/S,"
		"Q=QUERY/S";
	CONST_STRPTR WdmNames[] = { "never", "on non-backdrop windows", "always" };
	IPTR ArgOpts[13];
	struct RDArgs *Args;
	ULONG Result;
	BOOL fQuiet = FALSE;

	memset(ArgOpts, 0, sizeof(ArgOpts));

	ScalosBase = (struct ScalosBase *) OpenLibrary("scalos.library", 40);
	if (NULL == ScalosBase)
		{
		fprintf(stderr, "Failed to open scalos.library V40+\n");
		return RETURN_ERROR;
		}
#ifdef __amigaos4__
	IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
	if (NULL == IScalos)
		{
		fprintf(stderr, "Failed to get OS4 interface of scalos.library V40+\n");
		CloseLibrary((struct Library *)ScalosBase);
		return RETURN_ERROR;
		}
#endif

	if (((struct Library *)DOSBase)->lib_Version < 39)
		{
		printf("ScalosCtrl needs DOS.library v39+\n");
#ifdef __amigaos4__
		DropInterface((struct Interface *)IScalos);
#endif
		CloseLibrary((struct Library *)ScalosBase);
		return RETURN_ERROR;
		}

	Args = ReadArgs(ArgTemplate, ArgOpts, NULL);
	if (NULL == Args)
		{
		PrintFault(IoErr(), NULL);	/* prints the appropriate err message */

		return RETURN_ERROR;
		}

	if (ArgOpts[11])
		{
		fQuiet = TRUE;
		}

	if (ArgOpts[0])
		{
		ULONG defStack;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetDefaultStackSize, *((ULONG *) ArgOpts[0]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error setting default stack size");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetDefaultStackSize, (ULONG) &defStack,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying default stack size");

		if (!fQuiet)
			printf("Default stack size is now %ld bytes.\n", (long) defStack);
		}

	if (ArgOpts[1])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetCloseWBDisabled, *((ULONG *) ArgOpts[1]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error disabling CloseWorkbench()");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetCloseWBDisabled, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying CloseWorkbench() disable state");

		if (!fQuiet)
			printf("CloseWorkbench() is now %s.\n", Enabled ? "LOCKED" : "UNLOCKED");
		}

	if (ArgOpts[2])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetSplashEnable, *((ULONG *) ArgOpts[2]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error enabling splash window");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetSplashEnable, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying Splash window state");

		if (!fQuiet)
			printf("Splash window is now %s.\n", Enabled ? "ON" : "OFF");
		}

	if (ArgOpts[3])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetToolTipEnable, *((ULONG *) ArgOpts[3]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error enabling tooltips");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetToolTipEnable, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying ToolTip state");

		if (!fQuiet)
			printf("ToolTips are now %s.\n", Enabled ? "ON" : "OFF");
		}

	if (ArgOpts[4])
		{
		ULONG ttDelay;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetToolTipDelay, *((ULONG *) ArgOpts[4]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error setting ToolTip Delay");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetToolTipDelay, (ULONG) &ttDelay,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying ToolTip Delay");

		if (!fQuiet)
			printf("ToolTip delay is now %ld s.\n", (long) ttDelay);
		}

	if (ArgOpts[5])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetOldDragIconMode, *((ULONG *) ArgOpts[5]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error setting Icon Dragging Mode");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetOldDragIconMode, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying Icon Dragging Mode");

		if (!fQuiet)
			printf("Icon Dragging mode is now %s.\n", Enabled ? "OLD" : "NEW");
		}
	if (ArgOpts[6])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetStatusBarEnable, *((ULONG *) ArgOpts[6]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error enabling Status Bar");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetStatusBarEnable, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying Status Bar state");

		if (!fQuiet)
			printf("Status bar is now %s.\n", Enabled ? "ON" : "OFF");
		}
	if (ArgOpts[7])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetStripedTextWindows, *((ULONG *) ArgOpts[7]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error enabling Striped Text Windows");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetStripedTextWindows, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying Striped Text Windows state");

		if (!fQuiet)
			printf("Striped Text Windows are now %s.\n", Enabled ? "ON" : "OFF");
		}
	if (ArgOpts[8])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetDisplayDragCount, *((ULONG *) ArgOpts[8]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error enabling DragDrop Object count display");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetDisplayDragCount, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying DragDrop Object count display state");

		if (!fQuiet)
			printf("DragDrop Object count display is now %s.\n", Enabled ? "ON" : "OFF");
		}
	if (ArgOpts[9])
		{
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetWindowDropMarkMode, *((ULONG *) ArgOpts[9]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error setting window drop mark mode");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetWindowDropMarkMode, (ULONG) &Enabled,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying window drop mark mode");

		if (!fQuiet)
			printf("Window drop mark mode is now <%s>.\n", WdmNames[Enabled & 0x03]);
		}
	if (ArgOpts[10])
		{
		ULONG BuffSize;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetCopyBuffSize, *((ULONG *) ArgOpts[10]),
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error setting copy buffer size");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetCopyBuffSize, (ULONG) &BuffSize,
			TAG_END);

		if (Result < 1)
			PrintFault(IoErr(), "Error querying copy buffer size");

		if (!fQuiet)
			printf("Copy buffer size is now %ld Bytes.\n", (long) BuffSize);
		}
	if (ArgOpts[12])
		{
		// Query
		ULONG Enabled;

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetCopyBuffSize, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("Copy Buffer size      : %lu bytes\n", (unsigned long) Enabled);

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetDefaultStackSize, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("Default stack size    : %lu bytes\n", (unsigned long) Enabled);

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetCloseWBDisabled, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("CloseWorkbench()      : %s\n", Enabled ? "LOCKED" : "UNLOCKED");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetSplashEnable, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("Splash Window         : %s\n", Enabled ? "ENABLED" : "DISABLED");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetToolTipEnable, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("ToolTips              : %s\n", Enabled ? "ENABLED" : "DISABLED");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetToolTipDelay, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("ToolTip delay         : %lu s\n", (unsigned long) Enabled);

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetOldDragIconMode, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("Icon Dragging mode    : %s\n", Enabled ? "OLD" : "NEW");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetStatusBarEnable, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("StatusBar             : %s\n", Enabled ? "ENABLED" : "DISABLED");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetStripedTextWindows, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("Striped Text Windows  : %s\n", Enabled ? "ENABLED" : "DISABLED");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetDisplayDragCount, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("Display Drag Count    : %s\n", Enabled ? "ENABLED" : "DISABLED");

		Result = SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetWindowDropMarkMode, (ULONG) &Enabled,
			TAG_END);
		if (Result >= 1)
			printf("Window drop mark mode : %s\n", WdmNames[Enabled & 0x03]);
		}

	FreeArgs(Args);

#ifdef __amigaos4__
	DropInterface((struct Interface *)IScalos);
#endif
	CloseLibrary((struct Library *) ScalosBase);

	return RETURN_OK;
}
