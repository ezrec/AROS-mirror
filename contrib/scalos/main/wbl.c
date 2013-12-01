// wbl.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <datatypes/pictureclass.h>
#include <libraries/asl.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include <proto/asl.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

enum WBL_REQresult	{ WBLRR_Cancel=0, WBLRR_Retry=1, WBLRR_SelectTool, WBLRR_UseDefaultTool };

//----------------------------------------------------------------------------

// local functions

static BOOL WBLRequestFile(struct WBArg *arg);
static void AddWBProgram(struct WBStartup *wbStart);

//----------------------------------------------------------------------------

// local data


SAVEDS(ULONG) INTERRUPT WblTask(void)
{
	ULONG Result = 0;
	struct WblMessage *msg;
	struct WBStartup *wbStart = NULL;
	struct DevProc *devProc = NULL;
	BPTR segList = (BPTR)NULL;
	BPTR oldDir = NOT_A_LOCK;
	BPTR newLock = (BPTR)NULL;
	struct Process *myTask = (struct Process *) FindTask(NULL);

#ifdef __amigaos4__
	IDOS->Obtain();
#endif

	do	{
		msg = (struct WblMessage *) GetMsg(&myTask->pr_MsgPort);
		if (NULL == msg)
			WaitPort(&myTask->pr_MsgPort);
		} while (NULL == msg);

	d1(kprintf("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	do	{
		BOOL Retry = FALSE;
		struct WBArg *firstArg;
		size_t Length = sizeof(struct WBStartup) + (2 + msg->wbl_IO.wbl_Input.wbli_NumArgs) * sizeof(struct WBArg);

		d1(kprintf("%s/%s/%ld: Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));

		if (ID_WBLM != msg->wbl_Magic)		// just to be on the safe side...
			break;

		wbStart = AllocVec(Length, MEMF_PUBLIC | MEMF_CLEAR);
		if (NULL == wbStart)
			break;

		d1(kprintf("%s/%s/%ld: wbStart=%08lx\n", __FILE__, __FUNC__, __LINE__, wbStart));

		wbStart->sm_Message.mn_ReplyPort = wbPort;
		wbStart->sm_Message.mn_Length = Length;
		wbStart->sm_NumArgs = msg->wbl_IO.wbl_Input.wbli_NumArgs;

		// copy ArgList
		firstArg = (struct WBArg *) ((char *) (&wbStart->sm_ArgList) + 3 * sizeof(APTR));

		CopyMem(msg->wbl_IO.wbl_Input.wbli_ArgList, firstArg, 
			msg->wbl_IO.wbl_Input.wbli_NumArgs * sizeof(struct WBArg));

		wbStart->sm_ArgList = firstArg;

		d1(kprintf("%s/%s/%ld: Name = <%s>\n", __FILE__, __FUNC__, __LINE__, wbStart->sm_ArgList[0].wa_Name));
		debugLock_d1(wbStart->sm_ArgList[0].wa_Lock);

		SCA_ScreenTitleMsg(GetLocString(MSGID_WBLOADNAME), &wbStart->sm_ArgList[0].wa_Name);

		do	{
			d1(kprintf("%s/%s/%ld: wbl_IO.wbl_Input.wbli_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->wbl_IO.wbl_Input.wbli_Flags));

			if (msg->wbl_IO.wbl_Input.wbli_Flags & SCAF_WBStart_PathSearch)
				{
				// search through path
				struct AssignList *aList = NULL;
				BPTR dirLock = wbStart->sm_ArgList[0].wa_Lock;

				if (MainTask->pr_CLI)
					{
					struct CommandLineInterface *cli = (struct CommandLineInterface *) BADDR(MainTask->pr_CLI);

					aList = BADDR(cli->cli_CommandDir);
					}

				d1(kprintf("%s/%s/%ld: aList=%08lx\n", __FILE__, __FUNC__, __LINE__, aList));

				do	{
					newLock = DupLock(dirLock);
					oldDir = CurrentDir(newLock);

					segList = LoadSeg(wbStart->sm_ArgList[0].wa_Name);
					d1(kprintf("%s/%s/%ld: segList=%08lx\n", __FILE__, __FUNC__, __LINE__, segList));

					if (segList)
						{
						UnLock(wbStart->sm_ArgList[0].wa_Lock);
						wbStart->sm_ArgList[0].wa_Lock = newLock;
						newLock = (BPTR)NULL;
						CurrentDir(oldDir);
						oldDir = NOT_A_LOCK;
						}
					else
						{
						if (aList)
							{
							CurrentDir(oldDir);
							UnLock(newLock);
							newLock = (BPTR)NULL;
							oldDir = NOT_A_LOCK;

							dirLock = aList->al_Lock;
							aList = (struct AssignList *) BADDR(aList->al_Next);

							d1(kprintf("%s/%s/%ld: aList=%08lx\n", __FILE__, __FUNC__, __LINE__, aList));
							}
						else
							{
							CurrentDir(oldDir);
							UnLock(newLock);
							newLock = (BPTR)NULL;
							oldDir = NOT_A_LOCK;

							do	{
								devProc = GetDeviceProc("c:", devProc);
								d1(kprintf("%s/%s/%ld: devProc=%08lx\n", __FILE__, __FUNC__, __LINE__, devProc));
								if (devProc)
									{
									newLock = DupLock(devProc->dvp_Lock);
									oldDir = CurrentDir(newLock);
		
									segList = LoadSeg(wbStart->sm_ArgList[0].wa_Name);
									d1(kprintf("%s/%s/%ld: segList=%08lx\n", __FILE__, __FUNC__, __LINE__, segList));
									if ((BPTR)NULL == segList)
										{
										CurrentDir(oldDir);
										UnLock(newLock);
										newLock = (BPTR)NULL;
										oldDir = NOT_A_LOCK;
										}
									}
								} while (devProc && (BPTR)NULL == segList);

							if (devProc)
								{
								FreeDeviceProc(devProc);
								devProc = NULL;
								}
							}
						}
					} while ((BPTR)NULL == segList && aList);
				}
			else
				{
				oldDir = CurrentDir(wbStart->sm_ArgList[0].wa_Lock);

				segList = LoadSeg(wbStart->sm_ArgList[0].wa_Name);
				d1(kprintf("%s/%s/%ld: segList=%08lx\n", __FILE__, __FUNC__, __LINE__, segList));
				}

			if (IS_VALID_LOCK(oldDir))
				{
				CurrentDir(oldDir);
				oldDir = NOT_A_LOCK;
				}

			if (segList)
				{
				ULONG Priority, StackSize;
				BPTR progDirLock;
				struct Process *NewProc;

				wbStart->sm_Segment = segList;

				Priority = msg->wbl_IO.wbl_Input.wbli_Priority;
				StackSize = msg->wbl_IO.wbl_Input.wbli_StackSize;

				d1(kprintf("%s/%s/%ld: Priority=%ld StackSize=%lu\n", __FILE__, __FUNC__, __LINE__, Priority, StackSize));

				msg->wbl_IO.wbl_Return.wblr_WBStartup = wbStart;

				ReplyMsg((struct Message *) msg);
				msg = NULL;

#if 0
				{
				short n;

				d1(kprintf("%s/%s/%ld: NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, wbStart->sm_NumArgs));
				for (n=0; n<wbStart->sm_NumArgs; n++)
					{
					d1(kprintf("%s/%s/%ld: n=%ld Name=<%s>  Lock=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, n, wbStart->sm_ArgList[n].wa_Name, wbStart->sm_ArgList[n].wa_Lock));
					debugLock_d1(wbStart->sm_ArgList[n].wa_Lock);
					}
				}
#endif

				CurrentDir(wbStart->sm_ArgList[0].wa_Lock);
				progDirLock = Lock(wbStart->sm_ArgList[0].wa_Name, ACCESS_READ);

				d1(kprintf("%s/%s/%ld: progDirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, progDirLock));
				oldDir = NOT_A_LOCK;

				if (progDirLock)
					{
					debugLock_d1(progDirLock);

					oldDir = ParentDir(progDirLock);
					UnLock(progDirLock);
					}

				d1(kprintf("%s/%s/%ld: oldDir=%08lx\n", __FILE__, __FUNC__, __LINE__, oldDir));

				if (!IS_VALID_LOCK(oldDir))
					oldDir = DupLock(wbStart->sm_ArgList[0].wa_Lock);

				debugLock_d1(oldDir);

				CurrentDir((BPTR)NULL);

				// CreateNewProc()
				NewProc = CreateNewProcTags(
					NP_HomeDir, oldDir,
					NP_WindowPtr, NULL,
					NP_Cli, FALSE,
					NP_CurrentDir, NULL,
					NP_CloseOutput, FALSE,
					NP_CloseInput, FALSE,
					NP_Output, NULL,
					NP_Input, NULL,
					NP_ConsoleTask, NULL,
					NP_FreeSeglist, TRUE,
					NP_Priority, Priority,
					NP_StackSize, StackSize,
					NP_Seglist, wbStart->sm_Segment,
					NP_Name, (ULONG) wbStart->sm_ArgList[0].wa_Name,
					NP_Path, DupWBPathList(),
					TAG_END);

				d1(kprintf("%s/%s/%ld: Process=%08lx\n", __FILE__, __FUNC__, __LINE__, NewProc));
				if (NULL == NewProc)
					break;

				wbStart->sm_Process = &NewProc->pr_MsgPort;

				oldDir = NOT_A_LOCK;

				AddWBProgram(wbStart);

#if 0
				{
				short n;

				d1(kprintf("%s/%s/%ld: NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, wbStart->sm_NumArgs));
				for (n=0; n<wbStart->sm_NumArgs; n++)
					{
					d1(kprintf("%s/%s/%ld: n=%ld Name=<%s>  Lock=%08lx\n", \
						__LINE__, n, wbStart->sm_ArgList[n].wa_Name, wbStart->sm_ArgList[n].wa_Lock));
					debugLock_d1(wbStart->sm_ArgList[n].wa_Lock);
					}
				}
#endif

				d1(kprintf("%s/%s/%ld: MsgPort=%08lx\n", __FILE__, __FUNC__, __LINE__, &NewProc->pr_MsgPort));
				d1(kprintf("%s/%s/%ld: LaunchCount=%ld\n", __FILE__, __FUNC__, __LINE__, WBLaunchCount));

				d1(kprintf("%s/%s/%ld: Process=%08lx  Segment=%08lx  NumArgs=%ld  ToolWindow=<%s>\n", __FILE__, __FUNC__, __LINE__, \
					wbStart->sm_Process, wbStart->sm_Segment, wbStart->sm_NumArgs, wbStart->sm_ToolWindow));

				PutMsg(&NewProc->pr_MsgPort, (struct Message *) wbStart);

				wbStart = NULL;
				segList = (BPTR)NULL;
				SCA_ScreenTitleMsg(NULL, NULL);
				}
			else
				{
				ULONG ReqResult;
				ULONG MsgIdGadgets = MSGID_OPENERRORGADGETS;
				Object *DefaultIconObject = NULL;
				STRPTR DefaultTool = NULL;

				if (devProc)
					{
					FreeDeviceProc(devProc);
					devProc = NULL;
					}
				if (newLock)
					{
					UnLock(newLock);
					newLock = (BPTR)NULL;
					}

				d1(kprintf("%s/%s/%ld: sm_NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, wbStart->sm_NumArgs));
				if (wbStart->sm_NumArgs > 1)
					{
					Object *IconObject;
					ULONG IconType = 0;
					BPTR OldDir = CurrentDir(wbStart->sm_ArgList[1].wa_Lock);

					IconObject = (Object *) NewIconObjectTags((STRPTR) wbStart->sm_ArgList[1].wa_Name,
						TAG_END);

					d1(kprintf("%s/%s/%ld: IconObject=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObject));

					if (IconObject)
						{
						GetAttr(IDTA_Type, IconObject, &IconType);

						DisposeIconObject(IconObject);

						DefaultIconObject = ReturnDefIconObjTags(wbStart->sm_ArgList[1].wa_Lock,
							wbStart->sm_ArgList[1].wa_Name,
							TAG_END);

						d1(kprintf("%s/%s/%ld: IconType=%ld  DefaultIconObject=%08lx\n", __FILE__, __FUNC__, __LINE__, IconType, DefaultIconObject));

						if (DefaultIconObject)
							{
							GetAttr(IDTA_DefaultTool, DefaultIconObject, (APTR) &DefaultTool);

							d1(kprintf("%s/%s/%ld: DefaultTool=<%s>\n", __FILE__, __FUNC__, __LINE__, DefaultTool ? DefaultTool : (STRPTR) ""));

							if ((WBPROJECT == IconType) && DefaultTool && (strlen(DefaultTool) > 0))
								MsgIdGadgets = MSGID_OPENERRORGADGETS2;
							}

						}

					CurrentDir(OldDir);
					}

				ReqResult = UseRequestArgs(NULL, MSGID_OPENERRORNAME, MsgIdGadgets,
						1,
						wbStart->sm_ArgList[0].wa_Name);

				d1(kprintf("%s/%s/%ld: ReqResult=%ld\n", __FILE__, __FUNC__, __LINE__, ReqResult));

				switch (ReqResult)
					{
				default:
				case WBLRR_Cancel:
					Retry = FALSE;
					break;

				case WBLRR_Retry:
					Retry = TRUE;
					break;

				case WBLRR_SelectTool:
					Retry = WBLRequestFile(&wbStart->sm_ArgList[0]);
					d1(kprintf("%s/%s/%ld: AslBase=%08lx\n", __FILE__, __FUNC__, __LINE__, AslBase));
					break;

				case WBLRR_UseDefaultTool:
					d1(kprintf("%s/%s/%ld: WBLRR_UseDefaultTool DefaultTool=<%s>\n", __FILE__, __FUNC__, __LINE__, DefaultTool ? DefaultTool : (STRPTR) ""));
					if (DefaultTool && strlen(DefaultTool) > 0)
						{
						STRPTR pp;
						char ch;
						BPTR ToolLock;
						BPTR oldDir = CurrentDir(wbStart->sm_ArgList[1].wa_Lock);

						pp = PathPart(DefaultTool);
						ch = *pp;
						*pp = '\0';

						d1(KPrintF("%s/%s/%ld: PathPart=<%s>\n", __FILE__, __FUNC__, __LINE__, DefaultTool));

						ToolLock = Lock(DefaultTool, ACCESS_READ);
						debugLock_d1(ToolLock);
						if (ToolLock)
							{
							Retry = TRUE;

							UnLock(wbStart->sm_ArgList[0].wa_Lock);
							wbStart->sm_ArgList[0].wa_Lock = ToolLock;

							*pp = ch;

							FreeCopyString(wbStart->sm_ArgList[0].wa_Name);
							wbStart->sm_ArgList[0].wa_Name = AllocCopyString(FilePart(DefaultTool));
							}
						else
							{
							Retry = FALSE;
							*pp = ch;
							}

						CurrentDir(oldDir);
						}
					break;
					}

				if (DefaultIconObject)
					DisposeIconObject(DefaultIconObject);
				}
			} while (msg && Retry);
		} while (0);

	d1(kprintf("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

	if (msg)
		{
		msg->wbl_IO.wbl_Return.wblr_WBStartup = wbStart;
		ReplyMsg((struct Message *) msg);
		}

	if (devProc)
		FreeDeviceProc(devProc);
	if (segList)
		UnLoadSeg(segList);
	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (newLock)
		UnLock(newLock);
	if (wbStart)
		{
		ULONG n;

		d1(KPrintF("%s/%s/%ld: wbStart=%08lx  sm_NumArgs=%lu\n", __FILE__, __FUNC__, __LINE__, wbStart, wbStart->sm_NumArgs));

		for (n=0; n<wbStart->sm_NumArgs; n++)
			{
			d1(KPrintF("%s/%s/%ld: wa_Lock=%08lx  wa_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, wbStart->sm_ArgList[n].wa_Lock, wbStart->sm_ArgList[n].wa_Name));

			if (wbStart->sm_ArgList[n].wa_Lock)
				{
				UnLock(wbStart->sm_ArgList[n].wa_Lock);
				wbStart->sm_ArgList[n].wa_Lock = (BPTR)NULL;
				}
			if (wbStart->sm_ArgList[n].wa_Name)
				{
				FreeCopyString(wbStart->sm_ArgList[n].wa_Name);
				wbStart->sm_ArgList[n].wa_Name = NULL;
				}
			}

		wbStart->sm_NumArgs = 0;

		if (msg)
			ReplyMsg(&wbStart->sm_Message);
		else
			FreeVec(wbStart);
		}

	d1(kprintf("%s/%s/%ld: WblTask finished  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

#ifdef __amigaos4__
	IDOS->Release();
#endif

	return Result;
}


static BOOL WBLRequestFile(struct WBArg *arg)
{
	STRPTR Path;
	struct FileRequester *fileReq = NULL;
	BOOL Success = FALSE;

	do	{
		Path = AllocPathBuffer();
		if (NULL == Path)
			break;

		if (!NameFromLock(arg->wa_Lock, Path, Max_PathLen))
			break;

		// AllocAslRequest()
		fileReq = AllocAslRequestTags(ASL_FileRequest,
				ASLFR_PrivateIDCMP, TRUE,
				ASLFR_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				ASLFR_TitleText, (ULONG) GetLocString(MSGID_WBLOAD_ASLTITLE),
				ASLFR_DoSaveMode, FALSE,
				ASLFR_RejectIcons, TRUE,
				TAG_END);

		if (NULL == fileReq)
			break;

		// AslRequest()
		Success = AslRequestTags(fileReq,
				ASLFR_InitialFile, (ULONG) FilePart(arg->wa_Name),
				ASLFR_InitialDrawer, (ULONG) Path,
				TAG_END);

		if (Success)
			{
			BPTR dirLock;

			dirLock = Lock(fileReq->fr_Drawer, ACCESS_READ);
			if ((BPTR)NULL == dirLock)
				{
				Success = FALSE;
				break;
				}
			UnLock(arg->wa_Lock);
			arg->wa_Lock = dirLock;

			FreeCopyString(arg->wa_Name);
			arg->wa_Name = AllocCopyString(fileReq->fr_File);
			if (NULL == arg->wa_Name)
				{
				Success = FALSE;
				break;
				}
			}
		} while (0);

	if (Path)
		FreePathBuffer(Path);
	if (fileReq)
		FreeAslRequest(fileReq);

	return Success;
}


// add entry to list of wb-launched programs
static void AddWBProgram(struct WBStartup *wbStart)
{
	STRPTR ProgramPath = NULL;
	struct WBNode *newNode;

	ScalosObtainSemaphore(&WBStartListSema);

	do	{
		newNode = ScalosAlloc(sizeof(struct WBNode));
		if (NULL == newNode)
			break;

		ProgramPath = AllocPathBuffer();
		if (NULL == ProgramPath)
			break;

		if (!NameFromLock(wbStart->sm_ArgList[0].wa_Lock, ProgramPath, Max_PathLen))
			break;
		if (!AddPart(ProgramPath, wbStart->sm_ArgList[0].wa_Name, Max_PathLen))
			break;

		newNode->wbn_Node.ln_Type = NT_USER;
		newNode->wbn_Node.ln_Name = ProgramPath;
		newNode->wbn_wbStart = wbStart;

		AddTail(&WBStartList, &newNode->wbn_Node);

		newNode = NULL;
		ProgramPath = NULL;
		} while (0);

	if (newNode)
		ScalosFree(newNode);
	if (ProgramPath)
		FreePathBuffer(ProgramPath);

	WBLaunchCount++;
	ScalosReleaseSemaphore(&WBStartListSema);
}


// remove entry from list of wb-launched programs
void RemWBProgram(struct WBStartup *wbStart)
{
	struct WBNode *wbNode;

	ScalosObtainSemaphore(&WBStartListSema);
	WBLaunchCount--;

	d1(kprintf("%s/%s/%ld: wbStart=%08lx\n", __FILE__, __FUNC__, __LINE__, wbStart));

	for (wbNode = (struct WBNode *) WBStartList.lh_Head;
		wbNode != (struct WBNode *) WBStartList.lh_Tail; )
		{
		struct WBNode *wbNext = (struct WBNode *) wbNode->wbn_Node.ln_Succ;

		d1(kprintf("%s/%s/%ld: wbn_wbStart=%08lx\n", __FILE__, __FUNC__, __LINE__, wbNode->wbn_wbStart));

		if (wbNode->wbn_wbStart == wbStart)
			{
			Remove(&wbNode->wbn_Node);

			if (wbNode->wbn_Node.ln_Name)
				{
				FreePathBuffer(wbNode->wbn_Node.ln_Name);
				wbNode->wbn_Node.ln_Name = NULL;
				}

			ScalosFree(wbNode);

			break;
			}

		wbNode = wbNext;
		}

	ScalosReleaseSemaphore(&WBStartListSema);

	FreeVec(wbStart);
}
