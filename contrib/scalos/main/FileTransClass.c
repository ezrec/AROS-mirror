// FileTransClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <datatypes/pictureclass.h>
#include <utility/hooks.h>
#include <devices/timer.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/timer.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"

#include "int64.h"
#include "FileTransClass.h"
#include "GaugeGadgetClass.h"
#include "DtImageClass.h"

//----------------------------------------------------------------------------

// local data definitions

//#define	SHOW_EVERY_OBJECT
#define	SHOW_REMAINING_TIME

struct FileTransOp
	{
	struct Node fto_Node;
	ULONG fto_OpCode;	// enum ftOpCodes

	BPTR fto_SrcDirLock;
	CONST_STRPTR fto_SrcName;

	BPTR fto_DestDirLock;
	CONST_STRPTR fto_DestName;

	LONG fto_MouseX;		// position for destination icon
	LONG fto_MouseY;
	};

#define	FILETRANS_GADGET_IMAGE_BASENAME		"THEME:FileTrans"

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) FileTransClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_New(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_Dispose(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_Copy(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_Move(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_CreateLink(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_Delete(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_CheckAbort(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_OpenWindow(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_DelayedOpenWindow(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_UpdateWindow(Class *cl, Object *o, Msg msg);

static void AddFTOp(struct FileTransClassInstance *inst,
	enum ftOpCodes opType,
	BPTR srcDir, CONST_STRPTR srcName,
	BPTR destDir, CONST_STRPTR destName,
	LONG x, LONG y);
static void ProcessFTOps(Class *cl, Object *o);
static LONG DoFileTransCopy(Class *cl, Object *o, struct FileTransOp *fto);
static LONG DoFileTransMove(Class *cl, Object *o, struct FileTransOp *fto);
static LONG DoFileTransCreateLink(Class *cl, Object *o, struct FileTransOp *fto);
static LONG DoFileTransDelete(Class *cl, Object *o, struct FileTransOp *fto);
static void PutIcon2(CONST_STRPTR SrcName, BPTR srcLock, 
	CONST_STRPTR DestName, BPTR destLock, LONG x, LONG y);
static void FreeFTO(struct FileTransOp *fto);
static LONG CountFTOps(Class *cl, Object *o, struct FileTransOp *fto);
static SAVEDS(ULONG) FileTrans_BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg);
static LONG GetElapsedTime(T_TIMEVAL *tv);
static ULONG FileTransClass_CheckCountTimeout(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_OverwriteRequest(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_WriteProtectedRequest(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_ErrorRequest(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_LinksNotSupportedRequest(Class *cl, Object *o, Msg msg);
static ULONG FileTransClass_InsufficientSpaceRequest(Class *cl, Object *o, Msg msg);
static void FileTransClass_UpdateTextGadget(struct Window *win, struct Gadget *gad, CONST_STRPTR NewText);
static void FileTransClass_ChangeGadgetWidth(struct Window *win, struct Gadget *gad, ULONG NewWidth);
static void FileTransClass_ResizeWindow(struct FileTransClassInstance *inst);
static BOOL FileTransClass_CreateGadgets(struct FileTransClassInstance *inst);
static BOOL FileTransClass_CheckSufficientSpace(Class *cl, Object *o,
	struct FileTransOp *fto, ULONG BodyTextId, ULONG GadgetTextId);
static BOOL FileTransClass_IsRamDisk(BPTR dirLock, const struct InfoData *info);
static BOOL FileTransClass_ExistsIcon(BPTR DirLock, CONST_STRPTR Name);

//----------------------------------------------------------------------------

struct ScalosClass *initFileTransClass(const struct PluginClass *plug)
{
	struct ScalosClass *FileTransClass;

	FileTransClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct FileTransClassInstance),
			NULL);

	if (FileTransClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(FileTransClass->sccl_class->cl_Dispatcher, FileTransClass_Dispatcher);
		}

	return FileTransClass;
}


static SAVEDS(ULONG) FileTransClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = FileTransClass_New(cl, o, msg);
		break;

	case OM_DISPOSE:
		Result = FileTransClass_Dispose(cl, o, msg);
		break;

	case SCCM_FileTrans_Copy:
		Result = FileTransClass_Copy(cl, o, msg);
		break;

	case SCCM_FileTrans_Move:
		Result = FileTransClass_Move(cl, o, msg);
		break;

	case SCCM_FileTrans_CreateLink:
		Result = FileTransClass_CreateLink(cl, o, msg);
		break;

	case SCCM_FileTrans_Delete:
		Result = FileTransClass_Delete(cl, o, msg);
		break;

	case SCCM_FileTrans_CheckAbort:
		Result = FileTransClass_CheckAbort(cl, o, msg);
		break;

	case SCCM_FileTrans_OpenWindow:
		Result = FileTransClass_OpenWindow(cl, o, msg);
		break;

	case SCCM_FileTrans_OpenWindowDelayed:
		Result = FileTransClass_DelayedOpenWindow(cl, o, msg);
		break;

	case SCCM_FileTrans_UpdateWindow:
		Result = FileTransClass_UpdateWindow(cl, o, msg);
		break;

	case SCCM_FileTrans_CountTimeout:
		Result = FileTransClass_CheckCountTimeout(cl, o, msg);
		break;

	case SCCM_FileTrans_OverwriteRequest:
		Result = FileTransClass_OverwriteRequest(cl, o, msg);
		break;

	case SCCM_FileTrans_WriteProtectedRequest:
		Result = FileTransClass_WriteProtectedRequest(cl, o, msg);
		break;

	case SCCM_FileTrans_ErrorRequest:
		Result = FileTransClass_ErrorRequest(cl, o, msg);
		break;

	case SCCM_FileTrans_LinksNotSupportedRequest:
		Result = FileTransClass_LinksNotSupportedRequest(cl, o, msg);
		break;

	case SCCM_FileTrans_InsufficientSpaceRequest:
		Result = FileTransClass_InsufficientSpaceRequest(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG FileTransClass_New(Class *cl, Object *o, Msg msg)
{
	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (o)	
		{
		struct opSet *ops = (struct opSet *) msg;
		struct FileTransClassInstance *inst = INST_DATA(cl, o);

		ScalosInitSemaphore(&inst->ftci_OpListSemaphore);
		NewList(&inst->ftci_OpList);

		inst->ftci_TotalBytes = MakeU64(0);
		inst->ftci_TotalItems = 0;
		inst->ftci_TotalFiles = 0;
		inst->ftci_TotalDirs = 0;
		inst->ftci_TotalLinks = 0;

		SETHOOKFUNC(inst->ftci_BackFillHook, FileTrans_BackFillFunc);
		inst->ftci_BackFillHook.h_Data = inst;

		inst->ftci_Background = CreateDatatypesImage("THEME:FileTrans/Backfill", 0);
		if (NULL == inst->ftci_Background)
			inst->ftci_Background = CreateDatatypesImage("THEME:FileTransBackground", 0);

		ScalosObtainSemaphoreShared(&CopyHookSemaphore);

		inst->ftci_Number = GetTagData(SCCA_FileTrans_Number, 0, ops->ops_AttrList);
		inst->ftci_Screen = (struct Screen *) GetTagData(SCCA_FileTrans_Screen, (ULONG)NULL, ops->ops_AttrList);
		inst->ftci_ReplaceMode = GetTagData(SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask, ops->ops_AttrList);
		inst->ftci_OverwriteMode = GetTagData(SCCA_FileTrans_WriteProtectedMode, SCCV_WriteProtectedMode_Ask, ops->ops_AttrList);
		inst->ftci_LinksMode = GetTagData(SCCA_FileTrans_LinksNotSupportedMode, SCCV_LinksNotSupportedMode_Ask, ops->ops_AttrList);

		inst->ftci_Line1Buffer = AllocPathBuffer();
		inst->ftci_Line2Buffer = AllocPathBuffer();

		if (NULL == inst->ftci_Line1Buffer || NULL == inst->ftci_Line2Buffer)
			{
			DoMethod(o, OM_DISPOSE);
			o = NULL;
			}

		if (globalCopyHook)
			{
			struct CopyBeginMsg cbm;

			cbm.cbm_Length = sizeof(cbm);
			cbm.cbm_Action = CPACTION_Begin;
			cbm.cbm_SourceDrawer = (BPTR)NULL;
			cbm.cbm_DestinationDrawer = (BPTR)NULL;

			inst->ftci_HookResult = CallHookPkt(globalCopyHook, NULL, &cbm);
			}
		}

	return (ULONG) o;
}


static ULONG FileTransClass_Dispose(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	ULONG n;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (globalCopyHook && RETURN_OK == inst->ftci_HookResult)
		{
		struct CopyEndMsg cem;

		cem.cem_Length = sizeof(cem);
		cem.cem_Action = CPACTION_End;

		inst->ftci_HookResult = CallHookPkt(globalCopyHook, NULL, &cem);
		}

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	ProcessFTOps(cl, o);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	ScalosReleaseSemaphore(&CopyHookSemaphore);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (inst->ftci_Window)
		{
		LockedCloseWindow(inst->ftci_Window);
		inst->ftci_Window = NULL;
		}

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	for (n = 0; n < sizeof(inst->ftci_TextLineGadgets) / sizeof(inst->ftci_TextLineGadgets[0]); n++)
		{
		if (inst->ftci_TextLineGadgets[n])
			{
			SCA_DisposeScalosObject((Object *) inst->ftci_TextLineGadgets[n]);
			inst->ftci_TextLineGadgets[n] = NULL;
			}
		}
	if (inst->ftci_GaugeGadget)
		{
		SCA_DisposeScalosObject((Object *) inst->ftci_GaugeGadget);
		inst->ftci_GaugeGadget = NULL;
		}
	if (inst->ftci_CancelButtonFrame)
		{
		if (inst->ftci_CancelButtonGadget)
			{
			DisposeObject((Object *) inst->ftci_CancelButtonGadget);
			inst->ftci_CancelButtonGadget = NULL;
			}
		DisposeObject(inst->ftci_CancelButtonFrame);
		inst->ftci_CancelButtonFrame = NULL;
		}
	else
		{
		if (inst->ftci_CancelButtonGadget)
			{
			SCA_DisposeScalosObject((Object *) inst->ftci_CancelButtonGadget);
			inst->ftci_CancelButtonGadget = NULL;
			}
		}
	if (inst->ftci_CancelButtonImage)
		{
		DisposeObject(inst->ftci_CancelButtonImage);
		inst->ftci_CancelButtonImage = NULL;
		}

	if (inst->ftci_Line1Buffer)
		{
		FreePathBuffer(inst->ftci_Line1Buffer);
		inst->ftci_Line1Buffer = NULL;
		}
	if (inst->ftci_Line2Buffer)
		{
		FreePathBuffer(inst->ftci_Line2Buffer);
		inst->ftci_Line2Buffer = NULL;
		}
	if (inst->ftci_LastErrorFileName)
		{
		FreeCopyString(inst->ftci_LastErrorFileName);
		inst->ftci_LastErrorFileName = NULL;
		}

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	DisposeDatatypesImage(&inst->ftci_Background);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return DoSuperMethodA(cl, o, msg);
}


static ULONG FileTransClass_Copy(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	struct msg_Copy *mcp = (struct msg_Copy *) msg;
	ULONG Result;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (globalCopyHook && RETURN_OK == inst->ftci_HookResult)
		{
		struct CopyDataMsg cdm;

		cdm.cdm_Length = sizeof(cdm);
		cdm.cdm_Action = CPACTION_Copy;
		cdm.cdm_SourceLock = mcp->mcp_SrcDirLock;
		cdm.cdm_SourceName = (STRPTR) mcp->mcp_SrcName;
		cdm.cdm_DestinationLock = mcp->mcp_DestDirLock;
		cdm.cdm_DestinationName = (STRPTR) mcp->mcp_DestName;
		cdm.cdm_DestinationX = mcp->mcp_MouseX;
		cdm.cdm_DestinationY = mcp->mcp_MouseY;

		if (NULL == cdm.cdm_SourceName)
			{
			// SrcFileName may be NULL, 
			// then SourceDirLock is an absolute filename, i.e. "Volume:"
			cdm.cdm_SourceName = (STRPTR) "";
			}

		if (NULL == cdm.cdm_DestinationName)
			cdm.cdm_DestinationName = cdm.cdm_SourceName;

		d1(kprintf("%s/%s/%ld: Src name=<%s>\n", __FILE__, __FUNC__, __LINE__, cdm.cdm_SourceName));
		debugLock_d1(cdm.cdm_SourceLock);
		d1(kprintf("%s/%s/%ld: Dest name=<%s>\n", __FILE__, __FUNC__, __LINE__, cdm.cdm_DestinationName));
		debugLock_d1(cdm.cdm_DestinationLock);

		Result = CallHookPkt(globalCopyHook, NULL, &cdm);

		if (RETURN_OK == Result)
			{
			// Copying of object enqueued successfully
			// AsyncWB (which is the premium user of the copy hook)
			// seems to check whether an icon is present which is recognized by icon.library
			// this check fails for PNG icons unless a patch like PowerIcons is loaded.

			if (!FileTransClass_ExistsIcon(mcp->mcp_SrcDirLock, mcp->mcp_SrcName))
				{
				// No icon present which is recongized by icon.library
				BPTR oldDir = CurrentDir(mcp->mcp_DestDirLock);
				STRPTR IconName = AllocPathBuffer();

				d1(kprintf("%s/%s/%ld: SrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, mcp->mcp_SrcName));

				if (IconName)
					{
					stccpy(IconName, mcp->mcp_SrcName, Max_PathLen);
					SafeStrCat(IconName, ".info", Max_PathLen);

					if (ExistsObject(mcp->mcp_SrcDirLock, IconName))
						{
						cdm.cdm_Length = sizeof(cdm);
						cdm.cdm_Action = CPACTION_Copy;
						cdm.cdm_SourceLock = mcp->mcp_SrcDirLock;
						cdm.cdm_SourceName = IconName;
						cdm.cdm_DestinationLock = mcp->mcp_DestDirLock;
						cdm.cdm_DestinationName = IconName;
						cdm.cdm_DestinationX = mcp->mcp_MouseX;
						cdm.cdm_DestinationY = mcp->mcp_MouseY;

						d1(kprintf("%s/%s/%ld: Src name=<%s>\n", __FILE__, __FUNC__, __LINE__, cdm.cdm_SourceName));
						debugLock_d1(cdm.cdm_SourceLock);
						d1(kprintf("%s/%s/%ld: Dest name=<%s>\n", __FILE__, __FUNC__, __LINE__, cdm.cdm_DestinationName));
						debugLock_d1(cdm.cdm_DestinationLock);

						Result = CallHookPkt(globalCopyHook, NULL, &cdm);	
						}

					FreePathBuffer(IconName);
					}

				CurrentDir(oldDir);	
				}
			}
		return Result;
		}
	else
		{
		AddFTOp(inst, FTOPCODE_Copy,
			mcp->mcp_SrcDirLock, mcp->mcp_SrcName,
			mcp->mcp_DestDirLock, mcp->mcp_DestName,
			mcp->mcp_MouseX, mcp->mcp_MouseY);
		}

	return RETURN_OK;
}


static ULONG FileTransClass_Move(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	struct msg_Move *mmv = (struct msg_Move *) msg;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	d1(kprintf("%s/%s/%ld: Name=<%s>  x=%ld  y=%ld\n", \
		__LINE__, mmv->mmv_Name, mmv->mmv_MouseX, mmv->mmv_MouseY));

	AddFTOp(inst, FTOPCODE_Move,
		mmv->mmv_SrcDirLock, mmv->mmv_Name,
		mmv->mmv_DestDirLock, mmv->mmv_Name,
		mmv->mmv_MouseX, mmv->mmv_MouseY);

	return RETURN_OK;
}


static ULONG FileTransClass_CreateLink(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	struct msg_Copy *mcp = (struct msg_Copy *) msg;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	d1(kprintf("%s/%s/%ld: SrcName=<%s>  DestName=<%s>  x=%ld  y=%ld\n", \
		__LINE__, mcp->mcp_SrcName, mcp->mcp_DestName, mcp->mcp_MouseX, mcp->mcp_MouseY));

	AddFTOp(inst, FTOPCODE_CreateLink,
		mcp->mcp_SrcDirLock, mcp->mcp_SrcName,
		mcp->mcp_DestDirLock, mcp->mcp_DestName,
		mcp->mcp_MouseX, mcp->mcp_MouseY);

	return RETURN_OK;
}


static ULONG FileTransClass_Delete(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	struct msg_Delete *mmd = (struct msg_Delete *) msg;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	d1(kprintf("%s/%s/%ld: Name=<%s> \n", __FILE__, __FUNC__, __LINE__, mmd->mmd_Name));

	AddFTOp(inst, FTOPCODE_Delete,
		mmd->mmd_DirLock, mmd->mmd_Name,
		mmd->mmd_DirLock, mmd->mmd_Name,
		0, 0);

	return RETURN_OK;
}


static ULONG FileTransClass_OpenWindow(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);

	AskFont(&inst->ftci_Screen->RastPort, &inst->ftci_TextAttr);

	do	{
		CONST_STRPTR WinTitle;

		if (!FileTransClass_CreateGadgets(inst))
			break;

		switch (inst->ftci_CurrentOperation)
			{
		case FTOPCODE_Copy:
			WinTitle = GetLocString(MSGID_FILETRANSTITLE_COPY);
			break;
		case FTOPCODE_Move:
			WinTitle = GetLocString(MSGID_FILETRANSTITLE_MOVE);
			break;
		case FTOPCODE_Delete:
			WinTitle = GetLocString(MSGID_FILETRANSTITLE_DELETE);
			break;
		default:
			WinTitle = "";
			break;
			}

		inst->ftci_Window = LockedOpenWindowTags(NULL,
			WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_GADGETUP | IDCMP_NEWSIZE,
			WA_Gadgets, inst->ftci_GadgetList,
			WA_CustomScreen, inst->ftci_Screen,
			WA_Left, (inst->ftci_Screen->Width - inst->ftci_WindowWidth) / 2,
			WA_Top, (inst->ftci_Screen->Height - inst->ftci_WindowHeight) / 2,
			WA_Height, inst->ftci_WindowHeight,
			WA_Width, inst->ftci_WindowWidth,
			WA_MinHeight, inst->ftci_WindowHeight,
			WA_MinWidth, inst->ftci_WindowWidth,
			WA_MaxWidth, ~0,
			WA_MaxHeight, inst->ftci_WindowHeight,
			WA_BackFill, &inst->ftci_BackFillHook,
			WA_ScreenTitle, ScreenTitleBuffer,
			WA_DepthGadget, TRUE,
			WA_SizeGadget, TRUE,
			WA_SizeBBottom, TRUE,
			WA_DragBar, TRUE,
			WA_CloseGadget, TRUE,
			WA_SmartRefresh, TRUE,
			WA_RMBTrap, TRUE,
			WA_Title, WinTitle,
			TAG_END);
		if (NULL == inst->ftci_Window)
			break;

		// ...seems like GTTX_Text isn't honored during CreateGadget(),
		// so I set the Gadget texts here again
#ifdef SHOW_EVERY_OBJECT
		FileTransClass_UpdateTextGadget(inst->ftci_Window,
			inst->ftci_TextLineGadgets[0], inst->ftci_Line1Buffer);
#endif /* SHOW_EVERY_OBJECT */
//		  RefreshGList(inst->ftci_CancelButtonGadget, inst->ftci_Window, NULL, 1);
		} while (0);

	d1(Delay(5 * 50));

	return (ULONG) inst->ftci_Window;
}


static ULONG FileTransClass_CheckAbort(Class *cl, Object *o, Msg msg)
{
	ULONG Result = RETURN_OK;
	struct FileTransClassInstance *inst = INST_DATA(cl, o);

	if (inst->ftci_Window)
		{
		struct IntuiMessage *iMsg;

		d1(kprintf("%s/%s/%ld: Handling intuition messages for copy window\n", __FILE__, __FUNC__, __LINE__));

		while ((iMsg = (struct IntuiMessage *) GetMsg(inst->ftci_Window->UserPort)))
			{
			ULONG msgClass = iMsg->Class;
			struct Gadget *g = (struct Gadget *) iMsg->IAddress;

			ReplyMsg(&iMsg->ExecMessage);

			d1(kprintf("%s/%s/%ld: Class=%08lx\n", __FILE__, __FUNC__, __LINE__, msgClass));

			switch (msgClass)
				{
			case IDCMP_REFRESHWINDOW:
				BeginRefresh(inst->ftci_Window);
				EndRefresh(inst->ftci_Window, TRUE);
				break;

			case IDCMP_GADGETUP:
				switch (g->GadgetID)
					{
				case GID_CancelButton:
					Result = RESULT_UserAborted;
					break;
					}
				break;

			case IDCMP_CLOSEWINDOW:
				Result = RESULT_UserAborted;
				break;

			case IDCMP_NEWSIZE:
				FileTransClass_ResizeWindow(inst);
				break;
				}
			}
		}

	return Result;
}


static void AddFTOp(struct FileTransClassInstance *inst,
	enum ftOpCodes opType,
	BPTR srcDir, CONST_STRPTR srcName,
	BPTR destDir, CONST_STRPTR destName,
	LONG x, LONG y)
{
	struct FileTransOp *fto;

	inst->ftci_CurrentOperation = opType;

	fto = ScalosAlloc(sizeof(struct FileTransOp));
	d1(kprintf("%s/%s/%ld: fto=%08lx\n", __FILE__, __FUNC__, __LINE__, fto));
	if (fto)
		{
		debugLock_d1(srcDir);
		debugLock_d1(destDir);
		d1(kprintf("%s/%s/%ld: Op=%ld  Src=<%s>  Dest=<%s>\n", __FILE__, __FUNC__, __LINE__, opType, srcName, destName));
		d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

		fto->fto_OpCode = opType;
		fto->fto_SrcDirLock = DupLock(srcDir);
		fto->fto_DestDirLock = DupLock(destDir);

		if (NULL == srcName)
			{
			// SrcFileName may be NULL, 
			// then SourceDirLock is an absolute filename, i.e. "Volume:"
			fto->fto_SrcName = AllocCopyString("");
			}
		else
			fto->fto_SrcName = AllocCopyString(srcName);

		if (NULL == destName)
			fto->fto_DestName = AllocCopyString(srcName);
		else
			fto->fto_DestName = AllocCopyString(destName);

		fto->fto_MouseX = x;
		fto->fto_MouseY = y;

		ScalosObtainSemaphore(&inst->ftci_OpListSemaphore);
		AddTail(&inst->ftci_OpList, &fto->fto_Node);
		ScalosReleaseSemaphore(&inst->ftci_OpListSemaphore);
		}
}


static void ProcessFTOps(Class *cl, Object *o)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

	inst->ftci_CurrentBytes = MakeU64(0);
	inst->ftci_CurrentItems = 0;

	GetSysTime(&inst->ftci_ProcessStartTime);

	inst->ftci_CopyStartTime = inst->ftci_ProcessStartTime;
	inst->ftci_LastRemainTimeDisplay = inst->ftci_ProcessStartTime;

	ScalosObtainSemaphore(&inst->ftci_OpListSemaphore);

	if (!IsListEmpty(&inst->ftci_OpList))
		{
		struct FileTransOp *fto;
		LONG Result = RETURN_OK;
		CONST_STRPTR OpString;

		DoMethod(o, SCCM_FileTrans_OpenWindowDelayed);

		fto = (struct FileTransOp *) inst->ftci_OpList.lh_Head;

		switch (fto->fto_OpCode)
			{
		case FTOPCODE_Copy:
			OpString = GetLocString(MSGID_PREPARING_COPY);
			break;
		case FTOPCODE_Move:
			OpString = GetLocString(MSGID_PREPARING_MOVE);
			break;
		case FTOPCODE_Delete:
			OpString = GetLocString(MSGID_PREPARING_DELETE);
			break;
		default:
			OpString = "";
			break;
			}
#ifdef SHOW_EVERY_OBJECT
		stccpy(inst->ftci_Line1Buffer, OpString, Max_PathLen);
		strcpy(inst->ftci_Line2Buffer, "");
#else
		stccpy(inst->ftci_Line2Buffer, OpString, Max_PathLen);
#endif /* SHOW_EVERY_OBJECT */

		DoMethod(o, SCCM_FileTrans_UpdateWindow,
			FTUPDATE_Initial,
			NULL, NULL, "");

		for (fto = (struct FileTransOp *) inst->ftci_OpList.lh_Head;
			(RETURN_OK == Result) && (fto != (struct FileTransOp *) &inst->ftci_OpList.lh_Tail);
			fto = (struct FileTransOp *) fto->fto_Node.ln_Succ)
			{
			d1(KPrintF("%s/%s/%ld: fto=%08lx\n", __FILE__, __FUNC__, __LINE__, fto));
			Result = CountFTOps(cl, o, fto);
			}

		inst->ftci_CountValid = RETURN_OK == Result;
		d1(kprintf("%s/%s/%ld: ftci_CountValid=%ld  Result=%ld\n", __FILE__, __FUNC__, __LINE__, inst->ftci_CountValid, Result));

		if (inst->ftci_CountValid)
			{
			inst->ftci_TotalItems = inst->ftci_CurrentItems;
			inst->ftci_TotalBytes = inst->ftci_CurrentBytes;
			}

		inst->ftci_CurrentItems = 0;
		inst->ftci_CurrentBytes = MakeU64(0);

		GetSysTime(&inst->ftci_CopyStartTime);

		if (inst->ftci_LastErrorFileName)
			{
			FreeCopyString(inst->ftci_LastErrorFileName);
			inst->ftci_LastErrorFileName = NULL;
			}

		Result = inst->ftci_LastErrorCode = RETURN_OK;

		while ((fto = (struct FileTransOp *) RemHead(&inst->ftci_OpList)))
			{
			d1(kprintf("%s/%s/%ld: fto=%08lx\n", __FILE__, __FUNC__, __LINE__, fto));
			debugLock_d1(fto->fto_SrcDirLock);
			debugLock_d1(fto->fto_DestDirLock);
			d1(kprintf("%s/%s/%ld: Op=%ld  Src=<%s>  Dest=<%s>\n", \
				__LINE__, fto->fto_OpCode, fto->fto_SrcName, fto->fto_DestName));

			if (RETURN_OK == Result)
				{
				switch (fto->fto_OpCode)
					{
				case FTOPCODE_Copy:
					Result = DoFileTransCopy(cl, o, fto);
					break;

				case FTOPCODE_Move:
					Result = DoFileTransMove(cl, o, fto);
					break;

				case FTOPCODE_CreateLink:
					Result = DoFileTransCreateLink(cl, o, fto);
					break;

				case FTOPCODE_Delete:
					Result = DoFileTransDelete(cl, o, fto);
					break;
					}

				d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				}

			FreeFTO(fto);
			}
		d1(kprintf("%s/%s/%ld: fto=%08lx\n", __FILE__, __FUNC__, __LINE__, fto));
		d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		}

	ScalosReleaseSemaphore(&inst->ftci_OpListSemaphore);

	DoMethod(iwtMain->iwt_WindowTask.mt_MainObject, SCCM_ClearClipboard);
}


static LONG DoFileTransCopy(Class *cl, Object *o, struct FileTransOp *fto)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;
	BOOL VolumeHasIcon = FALSE;
	BPTR VoldDir;
	Object* OldIconObj = NULL;

	if (!FileTransClass_CheckSufficientSpace(cl, o, fto,
		MSGID_FILETRANSFER_INSUFFICIENTSPACE_BODY, MSGID_FILETRANSFER_INSUFFICIENTSPACE_GADGETS))
		{
		return RETURN_WARN;
		}

	// Check if "fto->fto_SrcName" length = 0 from current dir "fto->fto_SrcDirLock",
	// then try to allocate a icon Object for disk icon.
	// If ok and if icon type = "WBDISK", disk icon image will be used instead to use
	// the default def_drawer icon image.
	//
	VoldDir = CurrentDir(fto->fto_SrcDirLock);
	if (0 == strlen(fto->fto_SrcName))
		{
		OldIconObj = NewIconObject("disk", NULL);
		if (OldIconObj)
			{
			ULONG IconType;

			GetAttr(IDTA_Type, OldIconObj, &IconType);

			if (WBDISK == IconType)
				VolumeHasIcon = TRUE;
			}
		}
	CurrentDir(VoldDir);

	Result = CopyCommand(cl, o, fto->fto_SrcDirLock, fto->fto_DestDirLock,
		fto->fto_SrcName, fto->fto_DestName);

	d1(kprintf("%s/%s/%ld: Result=%08lx  ftci_MostCurrentReplaceMode=%ld\n", __FILE__, __FUNC__, __LINE__, Result, ftci->ftci_MostCurrentReplaceMode));

	if (RETURN_OK == Result)
		{
		// Copying of object finished successfully
		// Now try to copy the acompanying icon

		BPTR oldDir = CurrentDir(fto->fto_DestDirLock);
		Object *IconObj;
		LONG existsResult = EXISTREQ_Replace;

		d1(kprintf("%s/%s/%ld: SrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, fto->fto_SrcName));

		IconObj = NewIconObjectTags(fto->fto_DestName, TAG_END);
		if (IconObj)
			{
			STRPTR IconName;

			switch (ftci->ftci_MostCurrentReplaceMode)
				{
			case SCCV_ReplaceMode_Ask:
				IconName = AllocPathBuffer();
				if (IconName)
					{
					CONST_STRPTR Extension = "";

					GetAttr(IDTA_Extention, IconObj, (APTR) &Extension);

					stccpy(IconName, fto->fto_DestName, Max_PathLen);
					SafeStrCat(IconName, Extension, Max_PathLen);

					if (ExistsObject(fto->fto_DestDirLock, IconName))
						{
						existsResult = ftci->ftci_MostCurrentReplaceMode = DoMethod(o, SCCM_FileTrans_OverwriteRequest,
							OVERWRITEREQ_CopyIcon,
							fto->fto_SrcDirLock, IconName,
							fto->fto_DestDirLock, IconName,
							ftci->ftci_Window, 
							MSGID_EXISTSICON_COPY, MSGID_EXISTS_GNAME_NEW);
						}

					d1(kprintf("%s/%s/%ld: Result=%08lx  ftci_MostCurrentReplaceMode=%ld\n", \
						__LINE__, Result, ftci->ftci_MostCurrentReplaceMode));

					FreePathBuffer(IconName);
					}
				break;
			case SCCV_ReplaceMode_Never:
				existsResult = EXISTREQ_Skip;
				break;
			case SCCV_ReplaceMode_Always:
				existsResult = EXISTREQ_Replace;
				break;
				}

			d1(kprintf("%s/%s/%ld: existsResult=%ld\n", __FILE__, __FUNC__, __LINE__, existsResult));

			switch (existsResult)
				{
			case EXISTREQ_ReplaceAll:
				existsResult = EXISTREQ_Replace;
				break;
			case EXISTREQ_SkipAll:
				existsResult = EXISTREQ_Skip;
				break;
			default:
				break;
				}

			DisposeIconObject(IconObj);
			}

		if (EXISTREQ_Replace == existsResult)
			{
			if (0 == strlen(fto->fto_SrcName))
				{
				// Source is an absolute filename, i.e. "Volume:"
				// Source is to be copied into a directory

				if (VolumeHasIcon)
					{
					// Disk icon found from source.
					// Set correct "DTA_Name" and "IDTA_Type" for OldIconObj.
					//
					SetAttrs(OldIconObj,
						DTA_Name, fto->fto_DestName,
						IDTA_Type, (ULONG) WBDRAWER,
						TAG_END);

					IconObj = OldIconObj;
					}
				else
					{
					IconObj = GetDefIconObject(WBDRAWER, NULL);
					}

				d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

				if (IconObj)
					{
					struct ExtGadget *gg = (struct ExtGadget *) IconObj;

					d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", \
							__LINE__, fto->fto_MouseX, fto->fto_MouseY));

					gg->LeftEdge = fto->fto_MouseX;
					gg->TopEdge = fto->fto_MouseY;

					PutIconObjectTags(IconObj, fto->fto_DestName,
						ICONA_NoNewImage, TRUE,
						TAG_END);

					DisposeIconObject(IconObj);

					}
				}
			else
				{
				PutIcon2(fto->fto_SrcName, fto->fto_SrcDirLock, 
					fto->fto_DestName, fto->fto_DestDirLock,
					fto->fto_MouseX, fto->fto_MouseY);
				}

			if (!VolumeHasIcon)
				{
				if (OldIconObj)
					DisposeIconObject(OldIconObj);
				}

			// update destination window
			ScalosDropAddIcon(fto->fto_DestDirLock, fto->fto_DestName,
				fto->fto_MouseX, fto->fto_MouseY);
			}

		CurrentDir(oldDir);
		}

	return Result;
}


static LONG DoFileTransMove(Class *cl, Object *o, struct FileTransOp *fto)
{
	LONG Result;
	BPTR IconLock;

	debugLock_d1(fto->fto_SrcDirLock);
	debugLock_d1(fto->fto_DestDirLock);
	d1(kprintf("%s/%s/%ld: fto_SrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, fto->fto_SrcName));

	if (!FileTransClass_CheckSufficientSpace(cl, o, fto,
		MSGID_FILETRANSFER_INSUFFICIENTSPACE_BODY, MSGID_FILETRANSFER_INSUFFICIENTSPACE_GADGETS))
		{
		return RETURN_WARN;
		}

	Result = MoveCommand(cl, o, fto->fto_SrcDirLock, fto->fto_DestDirLock, fto->fto_SrcName);

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	if (RETURN_OK == Result)
		{
		BPTR oldDir;
		CONST_STRPTR ext = ".info";
		char Name[MAX_FileName];

		d1(kprintf("%s/%s/%ld: fto->fto_SrcName without 'ext' = <%s>\n", __FILE__, __FUNC__, __LINE__, fto->fto_SrcName));

		stccpy(Name, fto->fto_SrcName, sizeof(Name) - strlen(ext) - 1 );
		strcat(Name, ext);

		oldDir = CurrentDir(fto->fto_SrcDirLock);

		if ((IconLock = Lock(Name, SHARED_LOCK)))
			{
			UnLock(IconLock);

			d1(kprintf("%s/%s/%ld: Found ICON: <%s>\n", __FILE__, __FUNC__, __LINE__, Name));

			// move associated icon
			Result = MoveCommand(cl, o, fto->fto_SrcDirLock, fto->fto_DestDirLock, Name);

			d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK == Result)
				{
				PutIcon2(fto->fto_SrcName, fto->fto_DestDirLock, 
					fto->fto_DestName, fto->fto_DestDirLock, 
					fto->fto_MouseX, fto->fto_MouseY);
				}
			}

		CurrentDir(oldDir);
		}

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	// always try to handle icons apprioately if move was successfull
	// even if no .info icon could be moved since windows might be "show all files"
	if (RETURN_OK == Result)
		{
		// update source window
		ScalosDropRemoveIcon(fto->fto_SrcDirLock, fto->fto_SrcName);

		d1(kprintf("%s/%s/%ld: after ScalosDropRemoveIcon, Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

		// update destination window
		ScalosDropAddIcon(fto->fto_DestDirLock, fto->fto_DestName,
			fto->fto_MouseX, fto->fto_MouseY);
		}

	d1(kprintf("%s/%s/%ld: END Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG DoFileTransCreateLink(Class *cl, Object *o, struct FileTransOp *fto)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;

	Result = CreateLinkCommand(cl, o, fto->fto_SrcDirLock, fto->fto_DestDirLock, fto->fto_SrcName,  fto->fto_DestName);

	d1(KPrintF("%s/%s/%ld: Result=%ld  ftci_MostCurrentReplaceMode=%ld\n", __FILE__, __FUNC__, __LINE__, Result, ftci->ftci_MostCurrentReplaceMode));

	if (RETURN_OK == Result)
		{
		// Link to object created successfully
		// Now try to copy the object's icon

		BPTR oldDir = CurrentDir(fto->fto_SrcDirLock);
		Object *IconObj;
		LONG existsResult = EXISTREQ_Replace;

		d1(KPrintF("%s/%s/%ld: fto_SrcName=<%s>  fto_DestName=<%s>\n", __FILE__, __FUNC__, __LINE__, fto->fto_SrcName, fto->fto_DestName));

		IconObj = NewIconObjectTags(fto->fto_SrcName, TAG_END);
		d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		if (IconObj)
			{
			STRPTR SrcIconName;

			switch (ftci->ftci_MostCurrentReplaceMode)
				{
			case SCCV_ReplaceMode_Ask:
				SrcIconName = AllocPathBuffer();
				if (SrcIconName)
					{
					STRPTR DestIconName;

					DestIconName = AllocPathBuffer();
					if (DestIconName)
						{
						CONST_STRPTR Extension = "";

						GetAttr(IDTA_Extention, IconObj, (APTR) &Extension);

						stccpy(DestIconName, fto->fto_DestName, Max_PathLen);
						SafeStrCat(DestIconName, Extension, Max_PathLen);

						stccpy(SrcIconName, fto->fto_SrcName, Max_PathLen);
						SafeStrCat(SrcIconName, Extension, Max_PathLen);

						if (ExistsObject(fto->fto_DestDirLock, DestIconName))
							{
							existsResult = ftci->ftci_MostCurrentReplaceMode = DoMethod(o, SCCM_FileTrans_OverwriteRequest,
								OVERWRITEREQ_CopyIcon,
								fto->fto_SrcDirLock, SrcIconName,
								fto->fto_DestDirLock, DestIconName,
								ftci->ftci_Window,
								MSGID_EXISTSICON_COPY, MSGID_EXISTS_GNAME_NEW);
							}

						d1(KPrintF("%s/%s/%ld: Result=%08lx  ftci_MostCurrentReplaceMode=%ld\n", \
							__LINE__, Result, ftci->ftci_MostCurrentReplaceMode));

						FreePathBuffer(DestIconName);
						}

					FreePathBuffer(SrcIconName);
					}
				break;
			case SCCV_ReplaceMode_Never:
				existsResult = EXISTREQ_Skip;
				break;
			case SCCV_ReplaceMode_Always:
				existsResult = EXISTREQ_Replace;
				break;
				}

			d1(KPrintF("%s/%s/%ld: existsResult=%ld\n", __FILE__, __FUNC__, __LINE__, existsResult));

			switch (existsResult)
				{
			case EXISTREQ_ReplaceAll:
				existsResult = EXISTREQ_Replace;
				break;
			case EXISTREQ_SkipAll:
				existsResult = EXISTREQ_Skip;
				break;
			default:
				break;
				}

			DisposeIconObject(IconObj);
			}

		d1(KPrintF("%s/%s/%ld: existsResult=%ld\n", __FILE__, __FUNC__, __LINE__, existsResult));
		if (EXISTREQ_Replace == existsResult)
			{
			if (0 == strlen(fto->fto_SrcName))
				{
				// Source is an absolute filename, i.e. "Volume:"
				// Source is to be copied into a directory

				IconObj = GetDefIconObject(WBDRAWER, NULL);

				d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
				if (IconObj)
					{
					struct ExtGadget *gg = (struct ExtGadget *) IconObj;

					d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
						__FILE__, __FUNC__, __LINE__, fto->fto_MouseX, fto->fto_MouseY));

					gg->LeftEdge = fto->fto_MouseX;
					gg->TopEdge = fto->fto_MouseY;

					CurrentDir(fto->fto_DestDirLock);

					PutIconObjectTags(IconObj, fto->fto_DestName,
						TAG_END);

					DisposeIconObject(IconObj);
					}
				}
			else
				{
				PutIcon2(fto->fto_SrcName, fto->fto_SrcDirLock, 
					fto->fto_DestName, fto->fto_DestDirLock,
					fto->fto_MouseX, fto->fto_MouseY);
				}

			debugLock_d1(fto->fto_DestDirLock);
			d1(KPrintF("%s/%s/%ld: fto_DestName=<%s>\n", __FILE__, __FUNC__, __LINE__, fto->fto_DestName));

			// update destination window
			ScalosDropAddIcon(fto->fto_DestDirLock, fto->fto_DestName,
				fto->fto_MouseX, fto->fto_MouseY);
			}

		CurrentDir(oldDir);
		}

	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static LONG DoFileTransDelete(Class *cl, Object *o, struct FileTransOp *fto)
{
	//struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	LONG Result;

	Result = DeleteCommand(cl, o, fto->fto_SrcDirLock, fto->fto_SrcName);

	d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	if (RETURN_OK == Result)
		{
		// object deleted successfully
		// Now try to delete the acompanying icon

		BPTR oldDir = CurrentDir(fto->fto_SrcDirLock);

		d1(kprintf("%s/%s/%ld: SrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, fto->fto_SrcName));

		// update destination window
		ScalosDropRemoveIcon(fto->fto_SrcDirLock, fto->fto_SrcName);

		CurrentDir(oldDir);
		}

	return Result;
}


static void PutIcon2(CONST_STRPTR SrcName, BPTR srcLock, 
	CONST_STRPTR DestName, BPTR destLock, LONG x, LONG y)
{
	BPTR oldDir;
	Object *IconObj;

	oldDir = CurrentDir(srcLock);

	IconObj = NewIconObject(SrcName, NULL);
	if (IconObj)
		{
		struct ExtGadget *gg = (struct ExtGadget *) IconObj;

		d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

		gg->LeftEdge = x;
		gg->TopEdge = y;

		CurrentDir(destLock);

		// PutIconObject()
		PutIconObjectTags(IconObj, DestName, 
			ICONA_NoNewImage, TRUE,
			TAG_END);

		DisposeIconObject(IconObj);
		}

	CurrentDir(oldDir);
}


static void FreeFTO(struct FileTransOp *fto)
{
	if (fto)
		{
		if (fto->fto_SrcDirLock)
			{
			UnLock(fto->fto_SrcDirLock);
			fto->fto_SrcDirLock = (BPTR)NULL;
			}
		if (fto->fto_DestDirLock)
			{
			UnLock(fto->fto_DestDirLock);
			fto->fto_DestDirLock = (BPTR)NULL;
			}
		if (fto->fto_SrcName)
			{
			FreeCopyString((STRPTR) fto->fto_SrcName);
			fto->fto_SrcName = NULL;
			}
		if (fto->fto_DestName)
			{
			FreeCopyString((STRPTR) fto->fto_DestName);
			fto->fto_DestName = NULL;
			}

		ScalosFree(fto);
		}
}


// Count number of bytes and items to process
static LONG CountFTOps(Class *cl, Object *o, struct FileTransOp *fto)
{
//	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	LONG Result = RETURN_OK;

	d1(KPrintF("%s/%s/%ld: START  TotalBytes=%lu  TotalItems=%ld\n", \
		__LINE__, ULONG64_LOW(inst->ftci_TotalBytes), inst->ftci_TotalItems));

	switch (fto->fto_OpCode)
		{
	case FTOPCODE_CreateLink:
		d1(KPrintF("%s/%s/%ld: FTOPCODE_CreateLink\n", __FILE__, __FUNC__, __LINE__));
		Result = CountCommand(cl, o, fto->fto_SrcDirLock, fto->fto_SrcName, FALSE);
		break;

	case FTOPCODE_Copy:
		d1(KPrintF("%s/%s/%ld: FTOPCODE_Copy\n", __FILE__, __FUNC__, __LINE__));
		Result = CountCommand(cl, o, fto->fto_SrcDirLock, fto->fto_SrcName, TRUE);
		break;

	case FTOPCODE_Move:
		d1(KPrintF("%s/%s/%ld: FTOPCODE_Move\n", __FILE__, __FUNC__, __LINE__));
		switch (ScaSameLock(fto->fto_SrcDirLock, fto->fto_DestDirLock))
			{
		case LOCK_SAME:
			break;
		case LOCK_SAME_VOLUME:
			Result = CountCommand(cl, o, fto->fto_SrcDirLock, fto->fto_SrcName, FALSE);
			break;
		case LOCK_DIFFERENT:
			Result = CountCommand(cl, o, fto->fto_SrcDirLock, fto->fto_SrcName, TRUE);
			break;
			}
		break;

	case FTOPCODE_Delete:
		Result = CountCommand(cl, o, fto->fto_SrcDirLock, fto->fto_SrcName, TRUE);
		break;
		}

	d1(KPrintF("%s/%s/%ld: END Result=%ld  TotalBytes=%lu  TotalItems=%ld\n", \
		__LINE__, Result, ULONG64_LOW(inst->ftci_TotalBytes), inst->ftci_TotalItems));

	return Result;
}


static ULONG FileTransClass_DelayedOpenWindow(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);

	if (NULL == inst->ftci_Window)
		{
		LONG Diff = GetElapsedTime(&inst->ftci_ProcessStartTime);

		if (Diff >= CurrentPrefs.pref_FileTransWindowDelay)
			DoMethod(o, SCCM_FileTrans_OpenWindow);
		}

	return (ULONG) inst->ftci_Window;
}


// object == (struct RastPort *) result->RastPort
// message == [ (Layer *) layer, (struct Rectangle) bounds,
//              (LONG) offsetx, (LONG) offsety ]

static SAVEDS(ULONG) FileTrans_BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg)
{
	struct FileTransClassInstance *inst = (struct FileTransClassInstance *) bfHook->h_Data;
	struct RastPort rpCopy;

	d1(kprintf("%s/%s/%ld: RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));
	d1(kprintf("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
	d1(kprintf("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY));

	rpCopy = *rp;
	rpCopy.Layer = NULL;

	if (inst->ftci_Background)
		{
		WindowBackFill(&rpCopy, msg, inst->ftci_Background->dti_BitMap,
			inst->ftci_Background->dti_BitMapHeader->bmh_Width,
			inst->ftci_Background->dti_BitMapHeader->bmh_Height,
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN],
			0, 0,
			NULL);
		}
	else
		{
		WindowBackFill(&rpCopy, msg, NULL,
			0, 0,
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN],
			0, 0,
			NULL);
		}

	return 0;
}


static ULONG FileTransClass_UpdateWindow(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	struct msg_UpdateWindow *muw = (struct msg_UpdateWindow *) msg;
	STRPTR DestDirName;

	switch (muw->muw_UpdateMode)
		{
	case FTUPDATE_Initial:
		DoMethod(o, SCCM_FileTrans_OpenWindowDelayed);
		break;

	case FTUPDATE_EnterNewDir:
		DoMethod(o, SCCM_FileTrans_OpenWindowDelayed);

		DestDirName = AllocPathBuffer();

		if (DestDirName)
			{
			NameFromLock(muw->muw_DestDirLock, DestDirName, Max_PathLen);

			// show <from "fromdir" to "todir"> in lower text gadget
			ScaFormatStringMaxLength(inst->ftci_Line2Buffer, Max_PathLen,
				 "%s \"%s\" %s \"%s\"", 
				(ULONG) GetLocString(MSGID_FROMNAME),
				(ULONG) muw->muw_SrcName,
				(ULONG) GetLocString(MSGID_TONAME),
				(ULONG) DestDirName);

			d1(kprintf("%s/%s/%ld: Line2Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->ftci_Line2Buffer));

			if (inst->ftci_Window)
				{
				FileTransClass_UpdateTextGadget(inst->ftci_Window,
					inst->ftci_TextLineGadgets[1], inst->ftci_Line2Buffer);

				FreePathBuffer(DestDirName);
				}
			}
		break;

	case FTUPDATE_Entry:
		DoMethod(o, SCCM_FileTrans_OpenWindowDelayed);
#ifdef SHOW_EVERY_OBJECT
		// show <Copying "filename"> in upper text gadget
		ScaFormatStringMaxLength(inst->ftci_Line1Buffer, Max_PathLen,
			 "%s \"%s\"", 
			GetLocString(FTOPCODE_Copy == inst->ftci_CurrentOperation ? MSGID_COPYINGNAME : MSGID_MOVINGNAME), 
			muw->muw_SrcName);

		if (inst->ftci_Window)
			{
			FileTransClass_UpdateTextGadget(inst->ftci_Window,
				inst->ftci_TextLineGadgets[0], inst->ftci_Line1Buffer);
			}
#endif /* SHOW_EVERY_OBJECT */
		break;

	case FTUPDATE_SameFile:
		DoMethod(o, SCCM_FileTrans_OpenWindowDelayed);
		break;

	case FTUPDATE_Final:
		// if no more operations are outstanding, force current to total bytecount.
		if (IsListEmpty(&inst->ftci_OpList))
			inst->ftci_CurrentBytes = inst->ftci_TotalBytes;
		break;
		}

	if (inst->ftci_Window)
		{
		ULONG64 PercentFinishedBytes;
		ULONG	PercentFinishedItems;
		ULONG	PercentFinished;

		if (inst->ftci_CountValid)
			{
			PercentFinishedBytes = Div64(Mul64(inst->ftci_CurrentBytes, MakeU64(100), NULL), inst->ftci_TotalBytes, NULL);
			PercentFinishedItems = (inst->ftci_CurrentItems * 100) / inst->ftci_TotalItems;
			}
		else
			{
			PercentFinishedBytes = MakeU64(100);
			PercentFinishedItems = 100;
			}

		PercentFinished = ((75 * ULONG64_LOW(PercentFinishedBytes)) + (25 * PercentFinishedItems)) / 100;

		if (PercentFinished > 100)
			PercentFinished = 100;

#ifdef SHOW_REMAINING_TIME
		d1(KPrintF("%s/%s/%ld: cItem=%lu  tItem=%lu  Current=%lu  Total=%lu  Percent=%ld\n", \
			__FILE__, __FUNC__, __LINE__, inst->ftci_CurrentItems, inst->ftci_TotalItems, \
			ULONG64_LOW(inst->ftci_CurrentBytes), ULONG64_LOW(inst->ftci_TotalBytes), PercentFinished));

		if (GetElapsedTime(&inst->ftci_LastRemainTimeDisplay) >= 200)
			{
			char CurrentBytesBuffer[40];
			char TotalBytesBuffer[40];
			char SpeedBuffer[40];
			ULONG64 Speed;
			LONG RemainingTime;
			ULONG elapsedTime = GetElapsedTime(&inst->ftci_CopyStartTime);

			d1(kprintf("%s/%s/%ld: PercentFinishedBytes=%ld  PercentFinishedItems=%ld\n", \
				__FILE__, __FUNC__, __LINE__, PercentFinishedBytes, PercentFinishedItems));
			d1(kprintf("%s/%s/%ld: ftci_CurrentItems=%ld  ftci_TotalItems=%ld\n", \
				__FILE__, __FUNC__, __LINE__, inst->ftci_CurrentItems, inst->ftci_TotalItems));

			// Calculate speed in Bytes/s
			if (elapsedTime > 1000)
				Speed = Div64(inst->ftci_CurrentBytes, MakeU64(elapsedTime / 1000), NULL);
			else
				Speed = MakeU64(0);

			if (PercentFinished && elapsedTime)
				{
				ULONG TotalTime = (100 * elapsedTime) / PercentFinished;

				RemainingTime = TotalTime - elapsedTime;
				}
			else
				{
				RemainingTime = 0;
				}

			d1(kprintf("%s/%s/%ld: ElapsedTime=%lu cItem=%lu  tItem=%lu  Current=%lu  Total=%lu  Percent=%ld  remTime=%lu\n", \
				__FILE__, __FUNC__, __LINE__, GetElapsedTime(&inst->ftci_CopyStartTime), \
				inst->ftci_CurrentItems, inst->ftci_TotalItems, \
				ULONG64_LOW(inst->ftci_CurrentBytes), ULONG64_LOW(inst->ftci_TotalBytes), \
				PercentFinished, RemainingTime/1000));

			d1(kprintf("%s/%s/%ld: Percent=%ld  RemainingTime=%ld s\n", \
				__FILE__, __FUNC__, __LINE__, PercentFinished, RemainingTime/1000));

			if (inst->ftci_CountValid && PercentFinished > 3)
				{
				LONG RemainingSeconds = RemainingTime / 1000;
				LONG RemainingMinutes = RemainingSeconds / 60;
				LONG RemainingHours   = RemainingMinutes / 60;

				if (RemainingHours > 1)
					{
					RemainingMinutes -= RemainingHours * 60;

					ScaFormatStringMaxLength(inst->ftci_Line3Buffer, sizeof(inst->ftci_Line3Buffer),
						"%s %ld %s %ld %s",
						(ULONG) GetLocString(MSGID_FILETRANSFER_REMAININGTIME),
						RemainingHours,
						(ULONG) GetLocString(RemainingHours > 1 ?
							MSGID_FILETRANSFER_HOURS : MSGID_FILETRANSFER_HOUR),
						RemainingMinutes,
						(ULONG) GetLocString(RemainingMinutes > 1 ?
							MSGID_FILETRANSFER_MINUTES : MSGID_FILETRANSFER_MINUTE));
					}
				else if (RemainingMinutes > 1)
					{
					ScaFormatStringMaxLength(inst->ftci_Line3Buffer, sizeof(inst->ftci_Line3Buffer),
						"%s %ld %s", 
						(ULONG) GetLocString(MSGID_FILETRANSFER_REMAININGTIME),
						RemainingMinutes,
						(ULONG) GetLocString(MSGID_FILETRANSFER_MINUTES));
					}
				else
					{
					RemainingSeconds++;

					ScaFormatStringMaxLength(inst->ftci_Line3Buffer, sizeof(inst->ftci_Line3Buffer),
						"%s %ld %s", 
						(ULONG) GetLocString(MSGID_FILETRANSFER_REMAININGTIME),
						RemainingSeconds,
						(ULONG) GetLocString(RemainingSeconds > 1 ?
							MSGID_FILETRANSFER_SECONDS : MSGID_FILETRANSFER_SECOND));
					}
				}
			else
				{
				strcpy(inst->ftci_Line3Buffer, "");
				}

			TitleClass_Convert64KMG(inst->ftci_CurrentBytes, CurrentBytesBuffer, sizeof(CurrentBytesBuffer));
			TitleClass_Convert64KMG(inst->ftci_TotalBytes, TotalBytesBuffer, sizeof(TotalBytesBuffer));
			TitleClass_Convert64KMGRounded(Speed, SpeedBuffer, sizeof(SpeedBuffer), 99);

			if (inst->ftci_CountValid)
				{
				ScaFormatStringMaxLength(inst->ftci_Line4Buffer, sizeof(inst->ftci_Line4Buffer),
					GetLocString(MSGID_FILETRANSFER_DETAILS_OBJECTCOUNT),
					inst->ftci_CurrentItems, inst->ftci_TotalItems);

				ScaFormatStringMaxLength(inst->ftci_Line5Buffer, sizeof(inst->ftci_Line5Buffer),
					GetLocString(MSGID_FILETRANSFER_DETAILS_SIZE),
					(ULONG) CurrentBytesBuffer,
					(ULONG) TotalBytesBuffer);
				}
			else
				{
				ScaFormatStringMaxLength(inst->ftci_Line4Buffer, sizeof(inst->ftci_Line4Buffer),
					GetLocString(MSGID_FILETRANSFER_DETAILS_OBJECTCOUNT_NOTOTAL),
					inst->ftci_CurrentItems);

				ScaFormatStringMaxLength(inst->ftci_Line5Buffer, sizeof(inst->ftci_Line5Buffer),
					GetLocString(MSGID_FILETRANSFER_DETAILS_SIZE_NOTOTAL),
					(ULONG) CurrentBytesBuffer);
				}

			ScaFormatStringMaxLength(inst->ftci_Line6Buffer, sizeof(inst->ftci_Line6Buffer),
				GetLocString(MSGID_FILETRANSFER_DETAILS_SPEED),
				(ULONG) SpeedBuffer);

			FileTransClass_UpdateTextGadget(inst->ftci_Window,
				inst->ftci_TextLineGadgets[2], inst->ftci_Line3Buffer);

			FileTransClass_UpdateTextGadget(inst->ftci_Window,
				inst->ftci_TextLineGadgets[3], inst->ftci_Line4Buffer);

			FileTransClass_UpdateTextGadget(inst->ftci_Window,
				inst->ftci_TextLineGadgets[4], inst->ftci_Line5Buffer);

			FileTransClass_UpdateTextGadget(inst->ftci_Window,
				inst->ftci_TextLineGadgets[5], inst->ftci_Line6Buffer);

			GetSysTime(&inst->ftci_LastRemainTimeDisplay);
			}
#endif

		if (inst->ftci_CountValid)
			{
			SetGadgetAttrs(inst->ftci_GaugeGadget, inst->ftci_Window, NULL,
				SCAGAUGE_Level, PercentFinished,
				TAG_END);
			}
		}

	return 0;
}


// Get elapsed time since time <tv> in milliseconds.
static LONG GetElapsedTime(T_TIMEVAL *tv)
{
	T_TIMEVAL Now;
	LONG Diff;

	GetSysTime(&Now);
	SubTime(&Now, tv);

	Diff = 1000 * Now.tv_secs + Now.tv_micro / 1000;

	d1(kprintf("%s/%s/%ld: Now s=%ld  ms=%ld   Start s=%ld  ms=%ld  Diff=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
			Now.tv_secs, Now.tv_micro, \
			tv->tv_secs, tv->tv_micro, Diff));

	return Diff;
}


static ULONG FileTransClass_CheckCountTimeout(Class *cl, Object *o, Msg msg)
{
	struct FileTransClassInstance *inst = INST_DATA(cl, o);
	LONG Diff = GetElapsedTime(&inst->ftci_ProcessStartTime);

	return (ULONG) (Diff > CurrentPrefs.pref_FileCountTimeout);
}


static ULONG FileTransClass_OverwriteRequest(Class *cl, Object *o, Msg msg)
{
	struct msg_OverwriteRequest *mov = (struct msg_OverwriteRequest *) msg;
	enum ExistsReqResult result;

	result = UseRequestArgs(mov->mov_ParentWindow,
		mov->mov_SuggestedBodyTextId, 
		mov->mov_SuggestedGadgetTextId, 
		1,
		mov->mov_DestName);

	return (ULONG) result;
}


static ULONG FileTransClass_WriteProtectedRequest(Class *cl, Object *o, Msg msg)
{
	struct msg_WriteProtectedRequest *mwp = (struct msg_WriteProtectedRequest *) msg;
	enum ExistsReqResult result;
	char ErrorText[100];

	Fault(mwp->mwp_ErrorCode, (STRPTR) "", ErrorText, sizeof(ErrorText));

	result = UseRequestArgs(mwp->mwp_ParentWindow,
		mwp->mwp_SuggestedBodyTextId,
		mwp->mwp_SuggestedGadgetTextId,
		2,
		mwp->mwp_DestName,
                ErrorText
                );

	return (ULONG) result;
}


static ULONG FileTransClass_ErrorRequest(Class *cl, Object *o, Msg msg)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	struct msg_ErrorRequest *mer = (struct msg_ErrorRequest *) msg;
	enum ErrorReqResult result;
	char ErrorText[100];

	Fault(mer->mer_ErrorCode, (STRPTR) "", ErrorText, sizeof(ErrorText)-1);

	result = UseRequestArgs(mer->mer_ParentWindow,
		mer->mer_SuggestedBodyTextId,
		mer->mer_SuggestedGadgetTextId,
		2,
		mer->mer_ErrorFileName,
                ErrorText
                );

	return (ULONG) result;
}


static ULONG FileTransClass_LinksNotSupportedRequest(Class *cl, Object *o, Msg msg)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	struct msg_LinksNotSupportedRequest *mlns = (struct msg_LinksNotSupportedRequest *) msg;
	enum LinksNotSupportedReqResult result = LINKSNOTSUPPORTEDREQ_Abort;
	STRPTR DestPath;

	do	{
		DestPath = AllocPathBuffer();
		if (NULL == DestPath)
			break;

		if (!NameFromLock(mlns->mlns_DestDirLock, DestPath, Max_PathLen))
			break;

		result = UseRequestArgs(mlns->mlns_ParentWindow,
			mlns->mlns_SuggestedBodyTextId,
			mlns->mlns_SuggestedGadgetTextId,
			2,
			mlns->mlns_SrcName,
                        DestPath
			);
		} while (0);

	if (DestPath)
		FreePathBuffer(DestPath);

	return (ULONG) result;
}


static ULONG FileTransClass_InsufficientSpaceRequest(Class *cl, Object *o, Msg msg)
{
//	struct FileTransClassInstance *ftci = INST_DATA(cl, o);
	struct msg_InsufficientSpaceRequest *miss = (struct msg_InsufficientSpaceRequest *) msg;
	enum InsufficientSpaceReqResult result = INSUFFICIENTSPACE_Abort;
	STRPTR DestPath;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		char RequiredSpaceString[30];
		char AvailableSpaceString[30];

		DestPath = AllocPathBuffer();
		d1(KPrintF("%s/%s/%ld: DestPath=%08lx\n", __FILE__, __FUNC__, __LINE__, DestPath));
		if (NULL == DestPath)
			break;

		if (!NameFromLock(miss->miss_DestDirLock, DestPath, Max_PathLen))
			break;

		d1(KPrintF("%s/%s/%ld: DestPath=<%s>\n", __FILE__, __FUNC__, __LINE__, DestPath));

		TitleClass_Convert64KMG(*miss->miss_RequiredSpace, RequiredSpaceString, sizeof(RequiredSpaceString));
		TitleClass_Convert64KMG(*miss->miss_AvailableSpace, AvailableSpaceString, sizeof(AvailableSpaceString));

		d1(KPrintF("%s/%s/%ld: RequiredSpaceString=<%s>  AvailableSpaceString=<%s>\n", __FILE__, __FUNC__, __LINE__, RequiredSpaceString, AvailableSpaceString));

		result = UseRequestArgs(miss->mlns_ParentWindow,
			miss->miss_SuggestedBodyTextId,
			miss->miss_SuggestedGadgetTextId,
			3,
			DestPath,
			RequiredSpaceString,
			AvailableSpaceString
			);
		} while (0);

	if (DestPath)
		FreePathBuffer(DestPath);

	d1(KPrintF("%s/%s/%ld: result=%ld\n", __FILE__, __FUNC__, __LINE__, result));

	return (ULONG) result;
}


static void FileTransClass_UpdateTextGadget(struct Window *win, struct Gadget *gad, CONST_STRPTR NewText)
{
	SetGadgetAttrs(gad, win, NULL,
		GBTDTA_Text, (ULONG) NewText,
		TAG_END);
	EraseRect(win->RPort,
		gad->LeftEdge, gad->TopEdge,
		gad->LeftEdge + gad->Width - 1,
		gad->TopEdge + gad->Height -1);
	RefreshGList(gad, win, NULL, 1);
}


static void FileTransClass_ChangeGadgetWidth(struct Window *win, struct Gadget *gad, ULONG NewWidth)
{
	SetGadgetAttrs(gad, win, NULL,
		GA_Width, NewWidth,
		TAG_END);
	EraseRect(win->RPort,
		gad->LeftEdge, gad->TopEdge,
		gad->LeftEdge + gad->Width - 1,
		gad->TopEdge + gad->Height -1);
	RefreshGList(gad, win, NULL, 1);
}


static void FileTransClass_ResizeWindow(struct FileTransClassInstance *inst)
{
	FileTransClass_ChangeGadgetWidth(inst->ftci_Window, inst->ftci_GaugeGadget,
		inst->ftci_Window->Width - 10 - 10);
#ifdef SHOW_EVERY_OBJECT
	FileTransClass_ChangeGadgetWidth(inst->ftci_Window, inst->ftci_TextLineGadgets[0],
		inst->ftci_Window->Width - 10 - 10);
#endif /* SHOW_EVERY_OBJECT */
	FileTransClass_ChangeGadgetWidth(inst->ftci_Window, inst->ftci_TextLineGadgets[1],
		inst->ftci_Window->Width - 10 - 10);
#ifdef SHOW_REMAINING_TIME
	FileTransClass_ChangeGadgetWidth(inst->ftci_Window, inst->ftci_TextLineGadgets[2],
		inst->ftci_Window->Width - 10 - 10);
#endif /* SHOW_REMAINING_TIME */
	FileTransClass_ChangeGadgetWidth(inst->ftci_Window, inst->ftci_TextLineGadgets[3],
		inst->ftci_Window->Width - 10 - 10);
	FileTransClass_ChangeGadgetWidth(inst->ftci_Window, inst->ftci_TextLineGadgets[4],
		inst->ftci_Window->Width - 10 - 10);
	FileTransClass_ChangeGadgetWidth(inst->ftci_Window, inst->ftci_TextLineGadgets[5],
		inst->ftci_Window->Width - 10 - 10);

	EraseRect(inst->ftci_Window->RPort,
		inst->ftci_CancelButtonGadget->LeftEdge, inst->ftci_CancelButtonGadget->TopEdge,
		inst->ftci_CancelButtonGadget->LeftEdge + inst->ftci_CancelButtonGadget->Width - 1,
		inst->ftci_CancelButtonGadget->TopEdge + inst->ftci_CancelButtonGadget->Height -1);
	SetGadgetAttrs(inst->ftci_CancelButtonGadget, inst->ftci_Window, NULL,
		GA_Left, (inst->ftci_Window->Width - inst->ftci_CancelButtonGadget->Width) / 2,
		TAG_END);
}


static BOOL FileTransClass_CreateGadgets(struct FileTransClassInstance *inst)
{
	BOOL Success = FALSE;
	STRPTR ImageNameNrm;
	STRPTR ImageNameSel = NULL;

	AskFont(&inst->ftci_Screen->RastPort, &inst->ftci_TextAttr);

	do	{
		struct Gadget *gad = NULL;
		struct NewGadget ng;

		ImageNameNrm = AllocPathBuffer();
		if (NULL == ImageNameNrm)
			break;

		ImageNameSel= AllocPathBuffer();
		if (NULL == ImageNameSel)
			break;

		inst->ftci_WindowWidth = 300;

		ng.ng_LeftEdge = 10;
		ng.ng_TopEdge = inst->ftci_Screen->BarHeight + inst->ftci_TextAttr.ta_YSize;
		ng.ng_Width = inst->ftci_WindowWidth - 10 - 10;
		ng.ng_Height = inst->ftci_TextAttr.ta_YSize + 4;

#ifdef SHOW_EVERY_OBJECT
		d1(kprintf("%s/%s/%ld: Line1Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->ftci_Line1Buffer));

		gad = inst->ftci_GadgetList = inst->ftci_TextLineGadgets[0] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			(gad ? GA_Previous : TAG_IGNORE), (ULONG) gad,
			GBTDTA_Text, (ULONG) inst->ftci_Line1Buffer,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			TAG_END);
		if (NULL == gad)
			break;

		ng.ng_TopEdge += ng.ng_Height + 5;
#endif /* SHOW_EVERY_OBJECT */

		d1(kprintf("%s/%s/%ld: Line2Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->ftci_Line2Buffer));

		gad = inst->ftci_TextLineGadgets[1] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			(gad ? GA_Previous : TAG_IGNORE), (ULONG) gad,
			GBTDTA_Text, (ULONG) inst->ftci_Line2Buffer,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_Justification, GACT_STRINGLEFT,
			TAG_END);
		if (NULL == gad)
			break;

#ifndef SHOW_EVERY_OBJECT
		inst->ftci_GadgetList = gad;
#endif /* SHOW_EVERY_OBJECT */

		ng.ng_TopEdge += ng.ng_Height + 5;

		gad = inst->ftci_GaugeGadget = (struct Gadget *) SCA_NewScalosObjectTags("Gauge.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GA_Previous, (ULONG) gad,
			SCAGAUGE_MinValue, 0,
			SCAGAUGE_MaxValue, 100,
			SCAGAUGE_Level, 0,
			SCAGAUGE_Vertical, FALSE,
			SCAGAUGE_WantNumber, FALSE,
			SCAGAUGE_BarPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[FILLPEN],
			TAG_END);

		if (NULL == gad)
			break;
#ifdef SHOW_REMAINING_TIME
		ng.ng_TopEdge += ng.ng_Height + 5;

		gad = inst->ftci_TextLineGadgets[2] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GA_Previous, (ULONG) gad,
			GBTDTA_Text, (ULONG) inst->ftci_Line3Buffer,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			TAG_END);

		if (NULL == gad)
			break;
#endif // SHOW_REMAINING_TIME
		ng.ng_TopEdge += ng.ng_Height + 5;

		gad = inst->ftci_TextLineGadgets[3] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GA_Previous, (ULONG) gad,
			GBTDTA_Text, (ULONG) inst->ftci_Line4Buffer,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			TAG_END);

		if (NULL == gad)
			break;

		ng.ng_TopEdge += ng.ng_Height + 5;

		gad = inst->ftci_TextLineGadgets[4] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GA_Previous, (ULONG) gad,
			GBTDTA_Text, (ULONG) inst->ftci_Line5Buffer,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			TAG_END);

		if (NULL == gad)
			break;

		ng.ng_TopEdge += ng.ng_Height + 5;

		gad = inst->ftci_TextLineGadgets[5] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GA_Previous, (ULONG) gad,
			GBTDTA_Text, (ULONG) inst->ftci_Line6Buffer,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			TAG_END);

		if (NULL == gad)
			break;

		ng.ng_TopEdge += ng.ng_Height + 5;

		ng.ng_GadgetText = (STRPTR) GetLocString(MSGID_CANCELBUTTON);
		ng.ng_Height = inst->ftci_TextAttr.ta_YSize + 4 + 2;
		ng.ng_Width = 5 + 5 + Scalos_TextLength(&inst->ftci_Screen->RastPort, ng.ng_GadgetText, strlen(ng.ng_GadgetText));
		ng.ng_LeftEdge = (inst->ftci_WindowWidth - ng.ng_Width) / 2;

		stccpy(ImageNameNrm, FILETRANS_GADGET_IMAGE_BASENAME, Max_PathLen);
		AddPart(ImageNameNrm, "ButtonCancelNormal", Max_PathLen);

		stccpy(ImageNameSel, FILETRANS_GADGET_IMAGE_BASENAME, Max_PathLen);
		AddPart(ImageNameSel, "ButtonCancelSelected", Max_PathLen);

		// imgNormal is always required
		inst->ftci_CancelButtonImage = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) ImageNameNrm,
			DTIMG_SelImageName, (ULONG) ImageNameSel,
			TAG_END);
		d1(KPrintF("%s/%s/%ld: agi_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, agi->agi_Image));
		if (NULL == inst->ftci_CancelButtonImage)
			{
			// no Theme image could be found - fall back to sys image
			inst->ftci_CancelButtonFrame = NewObject(NULL, FRAMEICLASS,
				IA_Width, ng.ng_LeftEdge,
				IA_Height, ng.ng_TopEdge,
				SYSIA_DrawInfo, (ULONG) iInfos.xii_iinfos.ii_DrawInfo,
				IA_FrameType, FRAME_BUTTON,
				TAG_END);
			d1(KPrintF("%s/%s/%ld: ftci_CancelButtonFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->ftci_CancelButtonFrame));
			if (NULL == inst->ftci_CancelButtonFrame)
				break;

			gad = inst->ftci_CancelButtonGadget = (struct Gadget *) NewObject(NULL, FRBUTTONCLASS,
				GA_Left, ng.ng_LeftEdge,
				GA_Top, ng.ng_TopEdge,
				GA_Width, ng.ng_Width,
				GA_Height, ng.ng_Height,
				GA_Previous, (ULONG) gad,
				GA_ID, GID_CancelButton,
				GA_RelVerify, TRUE,
				GA_Underscore, '_',
				GA_Image, (ULONG) inst->ftci_CancelButtonFrame,
				GA_Text, (ULONG) ng.ng_GadgetText,
				TAG_END);
			d1(KPrintF("%s/%s/%ld: agi_Gadget=%08lx\n", __FILE__, __FUNC__, __LINE__, gad));
			}
		else
			{
			ULONG ImgWidth;

			GetAttr(IA_Width, inst->ftci_CancelButtonImage, &ImgWidth);
			//GetAttr(IA_Height, inst->ftci_CancelButtonImage, &agi->agi_Height);

			inst->ftci_CancelButtonGadget = (struct Gadget *) SCA_NewScalosObjectTags("ButtonGadget.sca",
				GA_Left, (inst->ftci_WindowWidth - ImgWidth) / 2,
				GA_Top, ng.ng_TopEdge,
				GA_ID, GID_CancelButton,
				GA_RelVerify, TRUE,
				GA_Image, (ULONG) inst->ftci_CancelButtonImage,
				GA_Previous, (ULONG) gad,
				TAG_END);
			}
		d1(KPrintF("%s/%s/%ld: gad=%08lx\n", __FILE__, __FUNC__, __LINE__, gad));
		if (NULL == gad)
			break;

		inst->ftci_WindowHeight	= ng.ng_TopEdge + ng.ng_Height + 10 + inst->ftci_Screen->BarHeight;

		Success = TRUE;
		} while (0);

	if (ImageNameNrm)
		FreePathBuffer(ImageNameNrm);
	if (ImageNameSel)
		FreePathBuffer(ImageNameSel);

	return Success;
}

//----------------------------------------------------------------------------

static BOOL FileTransClass_CheckSufficientSpace(Class *cl, Object *o,
	struct FileTransOp *fto, ULONG BodyTextId, ULONG GadgetTextId)
{
	struct FileTransClassInstance *ftci = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld: START  ftci_CountValid=%ld\n", __FILE__, __FUNC__, __LINE__, ftci->ftci_CountValid));

	if (ftci->ftci_CountValid)
		{
		enum InsufficientSpaceReqResult rc;
		struct InfoData *info = ScalosAllocInfoData();
		ULONG64 AvailableSpace;

		d1(KPrintF("%s/%s/%ld: info=%08lx\n", __FILE__, __FUNC__, __LINE__, info));

		if (info)
			{
			// Check available space on destination device
			BOOL isRamDisk;

			Info(fto->fto_DestDirLock, info);

			d1(KPrintF("%s/%s/%ld: id_NumBlocks=%lu  id_NumBlocksUsed=%lu\n", __FILE__, __FUNC__, __LINE__, info->id_NumBlocks, info->id_NumBlocksUsed));

			isRamDisk = FileTransClass_IsRamDisk(fto->fto_DestDirLock, info);

			AvailableSpace = Mul64(MakeU64(info->id_NumBlocks - info->id_NumBlocksUsed), MakeU64(info->id_BytesPerBlock), NULL);
			ScalosFreeInfoData(&info);

			if (!isRamDisk && Cmp64(AvailableSpace, ftci->ftci_TotalBytes) < 0)
				{
				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				rc = DoMethod(o, SCCM_FileTrans_InsufficientSpaceRequest,
					ftci->ftci_Window,
					fto->fto_SrcDirLock,
					fto->fto_SrcName,
					fto->fto_DestDirLock,
					fto->fto_DestName,
					&ftci->ftci_TotalBytes,
					&AvailableSpace,
					BodyTextId,
					GadgetTextId);

				d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));

				if (INSUFFICIENTSPACE_Ignore != rc)
					return FALSE;
				}
			}
		}

	return TRUE;
}

//----------------------------------------------------------------------------

// AmigaOS3.x RAM disk always return 0 free blocks
// we need to skip check for sufficient space here!
static BOOL FileTransClass_IsRamDisk(BPTR dirLock, const struct InfoData *info)
{
	BPTR ramDiskLock;
	static BOOL isRamDisk = FALSE;

	do	{
		LONG sameLock;

		ramDiskLock = Lock("RAM:", ACCESS_READ);
		d1(KPrintF("%s/%s/%ld: ramDiskLock=%08lx\n", __FILE__, __FUNC__, __LINE__, ramDiskLock));
		if (BNULL == ramDiskLock)
			break;

		sameLock = SameLock(dirLock, ramDiskLock);
		d1(KPrintF("%s/%s/%ld: sameLock=%ld\n", __FILE__, __FUNC__, __LINE__, sameLock));

		if (LOCK_DIFFERENT == sameLock)
			break;

		if (info->id_NumBlocksUsed != info->id_NumBlocks)
			break;

		d1(KPrintF("%s/%s/%ld: isRamDisk = TRUE\n", __FILE__, __FUNC__, __LINE__));

		isRamDisk = TRUE;
		} while (0);

	if (ramDiskLock)
		UnLock(ramDiskLock);

	return isRamDisk;
}

//----------------------------------------------------------------------------

// check if named object (file/directory) exists
static BOOL FileTransClass_ExistsIcon(BPTR DirLock, CONST_STRPTR Name)
{
	BOOL Exists = FALSE;
	BPTR oldDir = CurrentDir(DirLock);
	struct DiskObject *icon;

	icon = GetDiskObject((STRPTR) Name);
	if (icon)
		{
		Exists = TRUE;
		FreeDiskObject(icon);
		}

	CurrentDir(oldDir);

	return Exists;
}

//----------------------------------------------------------------------------
