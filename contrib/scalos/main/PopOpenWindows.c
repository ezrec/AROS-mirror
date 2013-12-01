// PopOpenWindows.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/intuition.h>
#include "debug.h"
#include <proto/scalos.h>
#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

struct PopOpenData
	{
	struct internalScaWindowTask *pod_ParentWindowTask;
	struct DragHandle *pod_DragHandle;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) PopOpenProcess(struct PopOpenData *sMsg, struct SM_RunProcess *msg);

//----------------------------------------------------------------------------

void PopOpenWindow(struct internalScaWindowTask *iwt, struct DragHandle *dh)
{
	struct ScaPopChildWindow *spcw = NULL;

	do	{
		struct PopOpenData pod;

		pod.pod_ParentWindowTask = iwt;
		pod.pod_DragHandle = dh;

		if (iwt->iwt_CloseWindow)
			break;

		spcw = ScalosAlloc(sizeof(struct ScaPopChildWindow));
		if (NULL == spcw)
			break;

		d1(KPrintF("%s/%s/%ld:  before SuspendDrag\n", __FILE__, __FUNC__, __LINE__));
		d1(KPrintF("%s/%s/%ld:  Task=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__,  FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

		spcw->spcw_DragHandle = dh;
		spcw->spcw_WasLocked = SuspendDrag(dh, iwt);
		spcw->spcw_DestWindowTask = dh->drgh_PopOpenDestWindow;

		spcw->spcw_EventHandle = (APTR) DoMethod(spcw->spcw_DestWindowTask->iwt_WindowTask.mt_MainObject,
			SCCM_AddListener,
			SCCM_WindowStartComplete,
			iwt->iwt_WindowTask.wt_IconPort,
			1);
		d1(KPrintF("%s/%s/%ld:  spcw_EventHandle=%08lx\n", __FILE__, __FUNC__, __LINE__, spcw->spcw_EventHandle));
		if (NULL == spcw->spcw_EventHandle)
			break;

		ScalosObtainSemaphore(&iwt->iwt_PopChildListSemaphore);
		AddTail(&iwt->iwt_PopChildList, &spcw->spcw_Node);
		spcw = NULL;
		ScalosReleaseSemaphore(&iwt->iwt_PopChildListSemaphore);

		DoMethod(dh->drgh_PopOpenDestWindow->iwt_WindowTask.mt_MainObject,
			SCCM_RunProcess,
			PopOpenProcess,
			&pod,
			sizeof(pod),
			dh->drgh_PopOpenDestWindow->iwt_WindowTask.wt_IconPort);
		} while (0);

	if (spcw)
		PopChildWindowDispose(iwt, spcw);
}

//----------------------------------------------------------------------------

void PopChildWindowDispose(struct internalScaWindowTask *iwt, struct ScaPopChildWindow *spcw)
{
	d1(KPrintF("%s/%s/%ld:  START  iwt=%08lx  spcw=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, spcw));

	if (spcw)
		{
		d1(KPrintF("%s/%s/%ld:  spcw_EventHandle=%08lx\n", __FILE__, __FUNC__, __LINE__, spcw->spcw_EventHandle));
		if (spcw->spcw_EventHandle)
			{
			DoMethod(spcw->spcw_DestWindowTask->iwt_WindowTask.mt_MainObject,
				SCCM_RemListener,
				spcw->spcw_EventHandle);
			}
		d1(KPrintF("%s/%s/%ld:  spcw_WasLocked=%ld\n", __FILE__, __FUNC__, __LINE__, spcw->spcw_WasLocked));
		if (spcw->spcw_WasLocked)
			{
			d1(KPrintF("%s/%s/%ld:  xii_GlobalDragHandle=%08lx  spcw_DragHandle=%08lx\n", __FILE__, __FUNC__, __LINE__, iInfos.xii_GlobalDragHandle, spcw->spcw_DragHandle));

			if (iInfos.xii_GlobalDragHandle != spcw->spcw_DragHandle)
				spcw->spcw_DragHandle = NULL;

			ResumeDrag(spcw->spcw_DragHandle, iwt, spcw->spcw_WasLocked);
			}
		ScalosFree(spcw);
		}

	d1(KPrintF("%s/%s/%ld:  END  iwt=%08lx  spcw=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, spcw));
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) PopOpenProcess(struct PopOpenData *pod, struct SM_RunProcess *msg)
{

	d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		ULONG Success;

		d1(KPrintF("%s/%s/%ld: before SCCM_AddListener\n", __FILE__, __FUNC__, __LINE__));

		Success = DoMethod(pod->pod_DragHandle->drgh_PopOpenDestWindow->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_Open,
			pod->pod_DragHandle->drgh_PopOpenIcon,
			ICONWINOPENF_DoNotActivateWindow | ICONWINOPENF_NewWindow
				| ICONWINOPENF_IgnoreFileTypes | ICONWINOPENF_DdPopupWindow);

		d1(KPrintF("%s/%s/%ld:  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
		if (!Success)
			break;
		} while (0);

	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------

// If CloseAll is FALSE, window under mouse pointer will not be closed
// If CloseAll is TRUE, all popup windows are closed

void ClosePopupWindows(BOOL CloseAll)
{
	struct ScaWindowStruct *ws, *wsNext;
	struct internalScaWindowTask *iwtUnderMouse;
	struct ScaIconNode *in;
	struct ScaIconNode *inOuterBounds;
	struct Window *foreignWin;

	d1(kprintf("%s/%s/%ld: START  CloseAll=%ld\n", __FILE__, __FUNC__, __LINE__, CloseAll));

	QueryObjectUnderPointer(&iwtUnderMouse, &in, &inOuterBounds, &foreignWin);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// Close all windows that have popped up during D&D
	if (NULL == iwtUnderMouse)
		{
		SCA_LockWindowList(SCA_LockWindowList_Exclusiv);
		}
	else
		{
		if (in)
			ScalosUnLockIconList(iwtUnderMouse);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (ws = winlist.wl_WindowStruct; ws; ws = wsNext)
		{
		wsNext = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if ((CloseAll || (NULL == iwtUnderMouse) || (ws != iwtUnderMouse->iwt_WindowTask.mt_WindowStruct)) && (ws->ws_Flags & WSV_FlagF_DdPopupWindow))
			{
			struct SM_CloseWindow *msg;

			msg = (struct SM_CloseWindow *) SCA_AllocMessage(MTYP_CloseWindow, 0);
			if (msg)
				{
				msg->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
				PutMsg(ws->ws_MessagePort, &msg->ScalosMessage.sm_Message);
				}
			}
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}

	SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------


