// CLIStart.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <datatypes/iconobject.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include <defs.h>
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

//----------------------------------------------------------------------------


BOOL CLIStart(BPTR dirLock, CONST_STRPTR PrgName, Object *iconObject, ULONG def_StackSize)
{
	BPTR oldCurrentDir;
	STRPTR defTool;
	STRPTR cmd;
	ULONG StackSize = def_StackSize;
	size_t CmdLen;
	ULONG IconType;
	BOOL Success = FALSE;
	const struct DiskObject *icon = NULL;

	oldCurrentDir = CurrentDir(dirLock);

	GetAttr(IDTA_DefaultTool, iconObject, (APTR) &defTool);
	GetAttr(IDTA_Stacksize, iconObject, &StackSize);
	GetAttr(AIDTA_Icon, iconObject, (APTR) &icon);

	if (NULL == defTool)
		defTool = (STRPTR) "";

	if (StackSize < def_StackSize)
		StackSize = def_StackSize;

	d1(kprintf("%s/%s/%ld: PrgName=<%s>  deftool=<%s>\n", __FILE__, __FUNC__, __LINE__, PrgName, defTool));

	GetAttr(IDTA_Type, iconObject, (ULONG *) &IconType);
	switch ((ULONG) IconType)
		{
	case WBTOOL:
	case WB_TEXTICON_TOOL:
		CmdLen = strlen(PrgName) + 10;
		break;

	case WBPROJECT:
		GetAttr(DTA_Name, iconObject, (APTR) &PrgName);
		if (NULL == PrgName)
			PrgName = (STRPTR) "";

		CmdLen = strlen(PrgName) + strlen(defTool) + 10;
		break;
	default:
		return FALSE;
		break;
		}

	d1(kprintf("%s/%s/%ld: PrgName=<%s>  deftool=<%s>\n", __FILE__, __FUNC__, __LINE__, PrgName, defTool));

	cmd = AllocVec(CmdLen, MEMF_PUBLIC);
	if (cmd)
		{
		BPTR inputFH, outputFH;
		LONG Error;
		STRPTR CliName;

		switch ((ULONG) IconType)
			{
		case WBTOOL:
		case WB_TEXTICON_TOOL:
			ScaFormatStringMaxLength(cmd, CmdLen, "\"%s\"", (ULONG) PrgName);
			break;
		case WBPROJECT:
			ScaFormatStringMaxLength(cmd, CmdLen, "\"%s\" \"%s\"", (ULONG) defTool, (ULONG) PrgName);
			break;
		default:
			return FALSE;
			break;
			}

		if (icon && icon->do_ToolWindow && strlen(icon->do_ToolWindow))
			CliName = (STRPTR) icon->do_ToolWindow;
		else
			CliName = (STRPTR) CurrentPrefs.pref_ConsoleName;      // was: "CON:////Scalos CLI/AUTO"

		outputFH = (BPTR)NULL;	      // ... see SystemTagList() AutoDoc
		inputFH = Open(CliName, MODE_NEWFILE);

		d1(kprintf("%s/%s/%ld: inputFH=%08lx  outputFH=%08lx\n", __FILE__, __FUNC__, __LINE__, inputFH, outputFH));

		// SystemTagList()
		Error = SystemTags(cmd,
			SYS_Input, inputFH,
			SYS_Output, outputFH,
			SYS_Asynch, TRUE,
			SYS_UserShell, TRUE,
			NP_StackSize, StackSize,
			NP_Path, DupWBPathList(),
			TAG_END);

		d1(kprintf("%s/%s/%ld: SystemTags returned %ld\n", __FILE__, __FUNC__, __LINE__, Error));

		if (RETURN_OK == Error)
			Success = TRUE;

		FreeVec(cmd);
		}

	CurrentDir(oldCurrentDir);

	return Success;
}


BPTR DupWBPathList(void)
{
	struct Process *wbProc = ScalosMainTask;
	struct CommandLineInterface *cli;
	struct AssignList *aList, *StartList = NULL, **nList = NULL;

	d1(KPrintF("%s/%s/%ld: wbProc=%08lx\n", __FILE__, __FUNC__, __LINE__, wbProc));
	if (NULL == wbProc)
		return (BPTR)NULL;
	if (NT_PROCESS != wbProc->pr_Task.tc_Node.ln_Type)
		return (BPTR)NULL;

	cli = BADDR(wbProc->pr_CLI);
	if (NULL == cli)
		return (BPTR)NULL;

	aList = BADDR(cli->cli_CommandDir);
	if (NULL == aList)
		return (BPTR)NULL;

	while (aList)
		{
		struct AssignList *nNode = AllocVec(sizeof(struct AssignList), MEMF_PUBLIC);

		if (NULL == nNode)
			break;
		if (NULL == nList)
			StartList = nNode;
		else
			*nList = (struct AssignList *) MKBADDR(nNode);

		nNode->al_Next = NULL;
		nNode->al_Lock = DupLock(aList->al_Lock);
		nList = &nNode->al_Next;

		d1(kprintf("%s/%s/%ld: aList=%08lx Next=%08lx Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			aList, aList->al_Next, aList->al_Lock));

		aList = BADDR(aList->al_Next);
		}

	return MKBADDR(StartList);
}


