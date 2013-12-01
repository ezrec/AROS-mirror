// test.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/intuitionbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <workbench/workbench.h>
#include <exec/nodes.h>

#include <clib/alib_protos.h>

//#include <proto/wbstart.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/wb.h>

#if 0
#include <scalos/scalos.h>
#include "wb39.h"
#endif

#include <defs.h>

static void PrintPathList(BPTR bList);
static SAVEDS(ULONG) cwbselHookFunc(struct Hook *hook, APTR reserved, struct IconSelectMsg *ism);


struct Library *WorkbenchBase;
extern struct ExecBase *SysBase;

#ifdef __amigaos4__
struct WorkbenchIFace *IWorkbench;
#endif

struct Hook cwbselHook;

int main()
{
	BOOL Result;
	LONG Found = FALSE;
	ULONG stackSize = 0;
	struct List *dList = NULL;
	char *fName = "HD1-6GB:Bilder/Normal";
	char *kName;
	int ch;


	WorkbenchBase = OpenLibrary(WORKBENCH_NAME, 44);
	if (NULL == WorkbenchBase)
		return 10;
#ifdef __amigaos4__
	IWorkbench = (struct WorkbenchIFace *)GetInterface(WorkbenchBase, "main", 1, NULL);
	if (NULL == IWorkbench)
		return 10;
#endif

//	printf("wt_fDragging = %08lx\n", offsetof(struct extScaWindowTask, wt_fDragging));

#if 1
	printf("======================================================\n");

	Result = WorkbenchControl(NULL, 
		WBCTRLA_GetDefaultStackSize, (ULONG) &stackSize,
		TAG_END);
	printf("WorkbenchControl(WBCTRLA_GetDefaultStackSize) returned %d  DefaultStackSize=%lu\n", Result, (unsigned long) stackSize);

	printf("======================================================\n");

	Result = WorkbenchControl(NULL, 
		WBCTRLA_GetProgramList, (ULONG) &dList,
		TAG_END);
	printf("WorkbenchControl(WBCTRLA_GetProgramList) returned %d  List=%08lx\n", Result, (unsigned long) dList);

	if (dList)
		{
		struct Node *xNode;

		for (xNode=dList->lh_Head; xNode != (struct Node *) &dList->lh_Tail; xNode=xNode->ln_Succ)
			{
			printf("	Node=%08lx Type=%02x Name=<%s>\n", (unsigned long) xNode, xNode->ln_Type, xNode->ln_Name);
			}

		Result = WorkbenchControl(NULL, 
			WBCTRLA_FreeProgramList, (ULONG) dList,
			TAG_END);

		printf("WorkbenchControl(WBCTRLA_FreeProgramList) returned %d\n", Result);
		}

	printf("======================================================\n");

	Result = WorkbenchControl(NULL, 
		WBCTRLA_GetOpenDrawerList, (ULONG) &dList,
		TAG_END);
	printf("WorkbenchControl(WBCTRLA_GetOpenDrawerList) returned %d  List=%08lx\n", Result, (unsigned long) dList);

	if (dList)
		{
		struct Node *xNode;

		for (xNode=dList->lh_Head; xNode != (struct Node *) &dList->lh_Tail; xNode=xNode->ln_Succ)
			{
			printf("	Node=%08lx Type=%02x Name=<%s>\n", (unsigned long) xNode, xNode->ln_Type, xNode->ln_Name);
			}

		Result = WorkbenchControl(NULL, 
			WBCTRLA_FreeOpenDrawerList, (ULONG) dList,
			TAG_END);

		printf("WorkbenchControl(WBCTRLA_FreeOpenDrawerList) returned %d\n", Result);
		}

	printf("======================================================\n");

	Result = WorkbenchControl(NULL, 
		WBCTRLA_GetHiddenDeviceList, (ULONG) &dList,
		TAG_END);
	printf("WorkbenchControl(WBCTRLA_GetHiddenDeviceList) returned %d  List=%08lx\n", Result, (unsigned long) dList);

	if (dList)
		{
		struct Node *xNode;

		for (xNode=dList->lh_Head; xNode != (struct Node *) &dList->lh_Tail; xNode=xNode->ln_Succ)
			{
			printf("	Node=%08lx Type=%02x Name=<%s>\n", (unsigned long) xNode, xNode->ln_Type, xNode->ln_Name);
			}

		Result = WorkbenchControl(NULL, 
			WBCTRLA_FreeHiddenDeviceList, (ULONG) dList,
			TAG_END);

		printf("WorkbenchControl(WBCTRLA_FreeHiddenDeviceList) returned %d\n", Result);
		}

	printf("======================================================\n");

	Result = WorkbenchControl(NULL, 
		WBCTRLA_GetSelectedIconList, (ULONG) &dList,
		TAG_END);
	printf("WorkbenchControl(WBCTRLA_GetSelectedIconList) returned %d  List=%08lx\n", Result, (unsigned long) dList);

	if (dList)
		{
		struct Node *xNode;

		for (xNode=dList->lh_Head; xNode != (struct Node *) &dList->lh_Tail; xNode=xNode->ln_Succ)
			{
			printf("	Node=%08lx Type=%02x Name=<%s>\n", (unsigned long) xNode, xNode->ln_Type, xNode->ln_Name);
			}

		// WorkbenchControlA()
		Result = WorkbenchControl(NULL, 
			WBCTRLA_FreeSelectedIconList, (ULONG) dList,
			TAG_END);

		printf("WorkbenchControl(WBCTRLA_FreeSelectedIconList) returned %d\n", Result);
		}

	printf("======================================================\n");

	Result = WorkbenchControl(fName,
		WBCTRLA_IsOpen, (ULONG) &Found,
		TAG_END);
	printf("WorkbenchControl(<%s>, WBCTRLA_IsOpen) returned %d, Open=%ld\n", fName, Result, (long) Found);

	printf("======================================================\n");

	dList = NULL;
	Result = WorkbenchControl(fName,
		WBCTRLA_DuplicateSearchPath, (ULONG) &dList,
		TAG_END);
	printf("WorkbenchControl(WBCTRLA_DuplicateSearchPath) returned %d, List=%08lx\n", Result, (unsigned long) dList);

	if (dList)
		PrintPathList((BPTR) dList);

	Result = WorkbenchControl(fName,
		WBCTRLA_FreeSearchPath, (ULONG) dList,
		TAG_END);
	printf("WorkbenchControl(WBCTRLA_FreeSearchPath) returned %d\n", Result);

	printf("======================================================\n");

	kName = "SYS:";
	printf("Press \"O\"+Enter to open %s Drawer...\n", kName);
	ch = getchar();

	if ('O' == toupper(ch))
		{
		// OpenWorkbenchObjectA()
		Result = OpenWorkbenchObject(kName, TAG_END);
		printf("OpenWorkbenchObject(%s) returned %d\n", kName, Result);

		if (Result)
			{
			printf("======================================================\n");

			Delay(50 * 3);	// Wait until drawer opened

			SETHOOKFUNC(cwbselHook, cwbselHookFunc);
			cwbselHook.h_Data = NULL;

			kName = "SYS:";
			// ChangeWorkbenchSelectionA()
			Result = ChangeWorkbenchSelection(kName, &cwbselHook, TAG_END);
			printf("ChangeWorkbenchSelection(%s) returned %d\n", kName, Result);

			printf("======================================================\n");

			printf("Press Enter to close %s Drawer...\n", kName);
			getchar();

			kName = "SYS:";
			Result = CloseWorkbenchObject(kName, TAG_END);
			printf("CloseWorkbenchObject(%s) returned %d\n", kName, Result);
			}
		}

	printf("======================================================\n");

	kName = "HD1-6GB:PD/BasiliskII/src/sony.cpp";

	// MakeWorkbenchObjectVisibleA()
	Result = MakeWorkbenchObjectVisible(kName,
			TAG_END);
	printf("MakeWorkbenchObjectVisible(%s) returned %d\n", kName, Result);

	printf("======================================================\n");
#endif
#ifdef __amigaos4__
	if (IWorkbench)
		{
		DropInterface((struct Interface *)IWorkbench);
		IWorkbench = NULL;
		}
#endif
	if (WorkbenchBase)
		{
		CloseLibrary(WorkbenchBase);
		WorkbenchBase = NULL;
		}

	return 0;
}


static void PrintPathList(BPTR bList)
{
	struct AssignList *aList;

	aList = BADDR(bList);
	while (aList)
		{
		char Buffer[250];

		NameFromLock(aList->al_Lock, Buffer, sizeof(Buffer));
		printf("Path <%s>\n", Buffer);

		aList = BADDR(aList->al_Next);
		}
}


static SAVEDS(ULONG) cwbselHookFunc(struct Hook *hook, APTR reserved, struct IconSelectMsg *ism)
{
#if 0
	printf("Length	= %ld\n", ism->ism_Length);
	printf("Drawer	= %08lx\n", ism->ism_Drawer);
	printf("Name	= <%s>\n", ism->ism_Name);
	printf("Type	= %ld\n", ism->ism_Type);
	printf("Selected= %ld\n", ism->ism_Selected);
	printf("Tags	= %08lx\n", ism->ism_Tags);
	printf("DrwWind	= %08lx\n", ism->ism_DrawerWindow);
	printf("ParWind	= %08lx\n", ism->ism_ParentWindow);
	printf("Left	= %ld\n", ism->ism_Left);
	printf("Top	= %ld\n", ism->ism_Top);
	printf("Width	= %ld\n", ism->ism_Width);
	printf("Height	= %ld\n", ism->ism_Height);
#endif
	if (0 == stricmp(ism->ism_Name, "devs"))
		ism->ism_Top -= 20;

//	if (0 == stricmp(ism->ism_Name, "Trashcan"))
//		return ISMACTION_Stop;

	// Select "Prefs" drawer
	if (0 == stricmp("Prefs", ism->ism_Name))
		return ISMACTION_Select;

	return ISMACTION_Ignore;
}

