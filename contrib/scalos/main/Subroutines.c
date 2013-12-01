// Subroutines.c
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

//----------------------------------------------------------------------------

// local functions

//----------------------------------------------------------------------------

// local data

//----------------------------------------------------------------------------

void StripTrailingColon(STRPTR Line)
{
	size_t Len = strlen(Line);

	Line += Len - 1;

	if (':' == *Line)
		*Line = '\0';
}


void StripTrailingLF(STRPTR Line)
{
	size_t Len = strlen(Line);

	Line += Len - 1;

	if ('\n' == *Line)
		*Line = '\0';
}


void StripIconExtension(STRPTR Line)
{
	ULONG Pos = IsIconName(Line);

	if (Pos && ~0 != Pos)
		{
		// strip ".info" from name
		*((char *) Pos) = '\0';
		}
}


STRPTR SafeStrCat(STRPTR dest, CONST_STRPTR src, size_t DestLen)
{
	STRPTR dst;
	size_t Len = strlen(dest);

	if (DestLen < Len)
		return dest;

	DestLen -= Len;
	dst = dest + Len;

	while (DestLen > 1 && *src)
		{
		*dst++ = *src++;
		DestLen--;
		}
	*dst = '\0';

	return dest;
}


//Output: BOOL (FALSE = no overlap)
//	when TRUE, <in> has been moved to wt_LateIconList

BOOL CheckOverlap(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	WORD xMin, xMax, yMin, yMax;
	struct ScaIconNode *inTest;
	BOOL Overlaps = FALSE;

	d1(kprintf("\n" "%s/%s/%ld: START in=%08lx  <%s>\n", \
		__FILE__, __FUNC__, __LINE__, in, in->in_Name ? in->in_Name : (STRPTR) ""));
	d1(kprintf("%s/%s/%ld: bLeft=%ld  bTop=%ld  bWidth=%ld  bHeight=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gg->BoundsLeftEdge, gg->BoundsTopEdge, gg->BoundsWidth, gg->BoundsHeight));

	ScalosLockIconListExclusive(iwt);

	xMin = xMax = gg->BoundsLeftEdge;
	yMin = yMax = gg->BoundsTopEdge;
	xMax += gg->BoundsWidth - 1;
	yMax += gg->BoundsHeight - 1;

	for (inTest=iwt->iwt_WindowTask.wt_IconList; inTest && !Overlaps; inTest = (struct ScaIconNode *) inTest->in_Node.mln_Succ)
		{
		struct ExtGadget *ggTest = (struct ExtGadget *) inTest->in_Icon;

		if (gg != ggTest &&
				xMax > ggTest->BoundsLeftEdge &&
				yMax > ggTest->BoundsTopEdge &&
				xMin < (ggTest->BoundsLeftEdge + ggTest->BoundsWidth - 1) &&
				yMin < (ggTest->BoundsTopEdge + ggTest->BoundsHeight - 1))
			{
			d1(kprintf("%s/%s/%ld: Overlaps with in=%08lx  <%s>\n", \
				__FILE__, __FUNC__, __LINE__, inTest, inTest->in_Name ? inTest->in_Name : (STRPTR) ""));
			d1(kprintf("%s/%s/%ld: bLeft=%ld  bTop=%ld  bWidth=%ld  bHeight=%ld\n", \
				__FILE__, __FUNC__, __LINE__, ggTest->BoundsLeftEdge, ggTest->BoundsTopEdge, ggTest->BoundsWidth, ggTest->BoundsHeight));

			Overlaps = TRUE;
			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &iwt->iwt_WindowTask.wt_LateIconList, in);
			}
		}

	ScalosUnLockIconList(iwt);

	d1(kprintf("%s/%s/%ld: Overlaps=%ld\n", __FILE__, __FUNC__, __LINE__, Overlaps));

	return Overlaps;
}

// ----------------------------------------------------------

BOOL ScaRectInRegion(const struct Region *TestRegion, const struct Rectangle *TestRect)
{
	d1(kprintf("%s/%s/%ld: TestRect: MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, TestRect->MinX, TestRect->MaxX, TestRect->MinY, TestRect->MaxY));
	d1(kprintf("%s/%s/%ld: TestRegion->bounds: MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, TestRegion->bounds.MinX, TestRegion->bounds.MaxX, TestRegion->bounds.MinY, TestRegion->bounds.MaxY));

	// First do quick check if <TestRect> overlaps TestRegion->bounds
	if (TestRect->MaxX > TestRegion->bounds.MinX &&
		TestRect->MaxY > TestRegion->bounds.MinY &&
		TestRect->MinX < TestRegion->bounds.MaxX &&
		TestRect->MinY < TestRegion->bounds.MaxY)
		{
		// if <TestRect> overlaps TestRegion->bounds,
		// check if we really overlap one of TestRegion->RegionRectangle's
		const struct RegionRectangle *RegionRect;

		for (RegionRect=TestRegion->RegionRectangle; RegionRect; RegionRect=RegionRect->Next)
			{
			d1(kprintf("%s/%s/%ld: RegionRect->bounds: MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", \
				__FILE__, __FUNC__, __LINE__, RegionRect->bounds.MinX, RegionRect->bounds.MaxX, RegionRect->bounds.MinY, RegionRect->bounds.MaxY));

			if (TestRect->MaxX > TestRegion->bounds.MinX + RegionRect->bounds.MinX &&
				TestRect->MaxY > TestRegion->bounds.MinY + RegionRect->bounds.MinY &&
				TestRect->MinX < TestRegion->bounds.MinX + RegionRect->bounds.MaxX &&
				TestRect->MinY < TestRegion->bounds.MinY + RegionRect->bounds.MaxY)
				{
				d1(kprintf("%s/%s/%ld: Rect in region\n", __FILE__, __FUNC__, __LINE__));
				return TRUE;
				}
			}
		}

	d1(kprintf("%s/%s/%ld: Rect NOT in region\n", __FILE__, __FUNC__, __LINE__));

	return FALSE;
}

// ----------------------------------------------------------

ULONG CheckPosition(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	WORD x, y, x0, y0;
	BOOL f = FALSE;

	d1(kprintf("%s/%s/%ld: START in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name ? in->in_Name : (STRPTR) ""));

	x = iwt->iwt_InnerWidth - gg->BoundsWidth - 1;
	y = iwt->iwt_InnerHeight - gg->BoundsHeight - 1;

	if (x < gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset)
		{
		x += iwt->iwt_WindowTask.wt_XOffset;
		x0 = gg->LeftEdge - gg->BoundsLeftEdge;
		gg->BoundsLeftEdge = x;
		x += x0;
		gg->LeftEdge = x;

		f = TRUE;
		}
	if (y < gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset)
		{
		y += iwt->iwt_WindowTask.wt_YOffset;
		y0 = gg->TopEdge - gg->BoundsTopEdge;
		gg->BoundsTopEdge = y;
		y += y0;
		gg->TopEdge = y;

		f = TRUE;
		}

	x = 1;
	y = 1;

	if (x > gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset)
		{
		x += iwt->iwt_WindowTask.wt_XOffset;
		x0 = gg->LeftEdge - gg->BoundsLeftEdge;
		gg->BoundsLeftEdge = x;
		x += x0;
		gg->LeftEdge = x;

		f = TRUE;
		}
	if (y > gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset)
		{
		y += iwt->iwt_WindowTask.wt_YOffset;
		y0 = gg->TopEdge - gg->BoundsTopEdge;
		gg->BoundsTopEdge = y;
		y += y0;
		gg->TopEdge = y;

		f = TRUE;
		}

	d1(kprintf("%s/%s/%ld: f=%ld\n", __FILE__, __FUNC__, __LINE__, f));

	if (f)
		return CheckOverlap(iwt, in);
	else
		return FALSE;
}


// check if closing Workbench is allowed
// return TRUE if allowed to close
BOOL CheckAllowCloseWB(void)
{
	short n;
	struct Hook *hook;
	struct Process *myProc;
	BOOL MayClose = TRUE;
	struct Task *BackFillTask;

	// avoid closing Scalos while async backfill processes are active
	for (n=0; n<5; n++)
		{
		Forbid();
		BackFillTask = FindTask("Scalos_Async_Backfill");
		Permit();

		if (NULL == BackFillTask)
			break;

		Delay(50);	// wait 1s
		}

	Forbid();
	BackFillTask = FindTask("Scalos_Async_Backfill");
	Permit();
	if (BackFillTask)
		return FALSE;

	ScalosObtainSemaphoreShared(&CloseWBHookListSemaphore);

	for (hook = (struct Hook *) globalCloseWBHookList.lh_Head;
		MayClose && hook != (struct Hook *) &globalCloseWBHookList.lh_Tail;
		hook = (struct Hook *) hook->h_MinNode.mln_Succ)
		{
		struct SetupCleanupHookMsg schm;

		d1(kprintf("%s/%s/%ld: hook=%08lx  h_Entry=%08lx\n", __FILE__, __FUNC__, __LINE__, hook, hook->h_Entry));

		schm.schm_Length = sizeof(schm);
		schm.schm_State = SCHMSTATE_TryCleanup;

		if (CallHookPkt(hook, NULL, &schm))
			MayClose = FALSE;
		}

	ScalosReleaseSemaphore(&CloseWBHookListSemaphore);

	if (!MayClose)
		return FALSE;

	if (!CurrentPrefs.pref_DisableCloseWorkbench)
		return TRUE;

	myProc = (struct Process *) FindTask(NULL);

	d1(kprintf("%s/%s/%ld: Process=%08lx  ln_Type=%ld\n", __FILE__, __FUNC__, __LINE__, myProc, myProc->pr_Task.tc_Node.ln_Type));

	if (myProc->pr_Task.tc_Node.ln_Name)
		{
		d1(kprintf("%s/%s/%ld: TaskName=<%s>\n", __FILE__, __FUNC__, __LINE__, myProc->pr_Task.tc_Node.ln_Name));

		if (0 == strcmp(myProc->pr_Task.tc_Node.ln_Name, "« IPrefs »"))
			return TRUE;
		}

	if (NT_PROCESS == myProc->pr_Task.tc_Node.ln_Type)
		{
		STRPTR Buffer = AllocPathBuffer();

		if (Buffer)
			{
			if (GetProgramName(Buffer, Max_PathLen))
				{
				d1(kprintf("%s/%s/%ld: ProgramName=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));

				if (0 == stricmp(FilePart(Buffer), "VisualPrefs"))
					{
					FreePathBuffer(Buffer);
					return TRUE;
					}
				}
			FreePathBuffer(Buffer);
			}
		}

	return FALSE;
}


ULONG WaitReply(struct MsgPort *port, struct internalScaWindowTask *iwt, ULONG MsgType)
{
	struct Message *msg;

	d1(kprintf("%s/%s/%ld: START iwt=%08lx  port=%08lx  MsgType=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, port, MsgType));

	do	{
		do	{
			WaitPort(port);

			msg = GetMsg(port);
			} while (NULL == msg);

		d1(Forbid(); KPrintF("%s/%s/%ld: iwt=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, msg); Permit());

		if (iwt && port == iwt->iwt_WindowTask.wt_IconPort)
			{
			ULONG Result;
			ULONG rcvdMsgType;

			Result = HandleWindowTaskIconPortMessages(iwt, msg, &rcvdMsgType);

			if (MsgType == rcvdMsgType)
				{
				d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
				return Result;
				}
			}
		else if (iInfos.xii_iinfos.ii_MainMsgPort == port)
			{
			struct MainTask *mt = (struct MainTask *) iwt;

			if (iwt && msg == (struct Message *) mt->iorequest)
				{
				// .timermsg
				d1(kprintf("%s/%s/%ld: Timer Message\n", __FILE__, __FUNC__, __LINE__));

				StartMainTaskTimer(mt);
				}
			else
				{
				if (NT_REPLYMSG != msg->mn_Node.ln_Type)
					{
					d1(kprintf("%s/%s/%ld: Reply Message  ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->mn_ReplyPort));
					ReplyMsg(msg);
					}
				else
					{
					struct SM_Requester *sMsg = (struct SM_Requester *) msg;

					if (ID_IMSG == sMsg->ScalosMessage.sm_Signature)
						{
						ULONG rcvdMsgType = sMsg->ScalosMessage.sm_MessageType;
						ULONG Result = sMsg->smrq_MultiPurpose.smrq_ReqResult;

						d1(kprintf("%s/%s/%ld: ScalosMessage  MsgType=%ld\n", __FILE__, __FUNC__, __LINE__, rcvdMsgType));

						SCA_FreeMessage(&sMsg->ScalosMessage);

						if (MsgType == rcvdMsgType)
							{
							d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
							return Result;
							}
						}
					}
				}
			}
		else
			{
			if (NT_REPLYMSG != msg->mn_Node.ln_Type)
				{
				struct IntuiMessage *iMsg = (struct IntuiMessage *) msg;

				d1(kprintf("%s/%s/%ld: Reply Message\n", __FILE__, __FUNC__, __LINE__));

				if (iwt && IDCMP_REFRESHWINDOW == iMsg->Class)
					IDCMPRefreshWindow(iwt, iMsg);

				ReplyMsg(msg);
				}
			else
				{
				struct SM_Requester *sMsg = (struct SM_Requester *) msg;

				if (ID_IMSG == sMsg->ScalosMessage.sm_Signature)
					{
					ULONG rcvdMsgType = sMsg->ScalosMessage.sm_MessageType;
					ULONG Result = sMsg->smrq_MultiPurpose.smrq_ReqResult;

					d1(kprintf("%s/%s/%ld: ScalosMessage  MsgType=%ld\n", __FILE__, __FUNC__, __LINE__, rcvdMsgType));

					if ((MTYP_AsyncBackFill == rcvdMsgType) && iwt)
						AsyncReplyMsg(iwt, &sMsg->ScalosMessage.sm_Message, NULL);

					SCA_FreeMessage(&sMsg->ScalosMessage);

					if (MsgType == rcvdMsgType)
						{
						d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));
						return Result;
						}
					}
				}

			}
		} while (1);
}


ULONG HandleWindowTaskIconPortMessages(struct internalScaWindowTask *iwt, 
	struct Message *Msg, ULONG *MessageType)
{
	struct ScalosMessage *sMsg = (struct ScalosMessage *) Msg;
	ULONG Result = 0;

	d1(Forbid(); kprintf("%s/%s/%ld: START iwt=%08lx Msg=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, Msg); Permit());

	*MessageType = 0;

	if (ID_IMSG == sMsg->sm_Signature)
		{
		APTR ArgPtr = (APTR) (((UBYTE *) &sMsg->sm_MessageType) + sizeof(sMsg->sm_MessageType));
		short mIndex = sMsg->sm_MessageType - 1;

		d1(Forbid(); KPrintF("%s/%s/%ld: iwt=%08lx  Msg=%08lx  MsgType=%ld, ln_Type=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwt, sMsg, sMsg->sm_MessageType, Msg->mn_Node.ln_Type); Permit());

		*MessageType = sMsg->sm_MessageType;

		if (NT_REPLYMSG == Msg->mn_Node.ln_Type)
			{
			if (mIndex > 0 && mIndex < MTYP_MAX)
				{
				d1(KPrintF("%s/%s/%ld: mIndex=%ld ReplyFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, mIndex, iMsgTable[mIndex].ReplyFunc));

				if (iMsgTable[mIndex].ReplyFunc)
					(*iMsgTable[mIndex].ReplyFunc)(iwt, Msg, ArgPtr);

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				}

			Result = ((struct SM_Requester *) sMsg)->smrq_MultiPurpose.smrq_ReqResult;

			SCA_FreeMessage(sMsg);

			if (iwt->iwt_IconPortOutstanding > 0)
				iwt->iwt_IconPortOutstanding--;
			}
		else
			{
			d1(Forbid(); KPrintF("%s/%s/%ld: mIndex=%ld  MsgFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, mIndex, iMsgTable[mIndex].MsgFunc); Permit());

			if (mIndex > 0 && mIndex < MTYP_MAX)
				{
				d1(KPrintF("%s/%s/%ld: mIndex=%ld MsgFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, mIndex, iMsgTable[mIndex].MsgFunc));

				if (iMsgTable[mIndex].MsgFunc)
					(*iMsgTable[mIndex].MsgFunc)(iwt, Msg, ArgPtr);

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				}

			d1(Forbid(); KPrintF("%s/%s/%ld: Reply/Free Msg=%08lx\n", __FILE__, __FUNC__, __LINE__, Msg); Permit());

			if (Msg->mn_ReplyPort)
				ReplyMsg(Msg);
			else
				SCA_FreeMessage(sMsg);
			}
		}
	else
		{
		// handle IntuiMessage
		d1(KPrintF("%s/%s/%ld: iwt=%08lx  IntuiMessage Msg=%08lx  Class=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, iwt, Msg, ((struct IntuiMessage *) Msg)->Class));

		Result = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_Message, Msg);
		if (Result)
			{
			iwt->iwt_CloseWindow = TRUE;
			d1(kprintf("%s/%s/%ld: Return IDCMP_CLOSEWINDOW\n", __FILE__, __FUNC__, __LINE__));
			Result = IDCMP_CLOSEWINDOW;
			}
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}

	d1(Forbid(); KPrintF("%s/%s/%ld: END iwt=%08lx Result=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, Result); Permit());

	return Result;
}


void ClearMsgPort(struct MsgPort *msgPort)
{
	if (msgPort)
		{
		struct ScalosMessage *msg;

		do	{
			msg = (struct ScalosMessage *) GetMsg(msgPort);
			d1(Forbid(); KPrintF("%s/%s/%ld:  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg); Permit());
			if (msg)
				{
				if (NT_REPLYMSG == msg->sm_Message.mn_Node.ln_Type)
					{
					ReplyMsg(&msg->sm_Message);
					}
				else
					{
					if (ID_IMSG == msg->sm_Signature)
						SCA_FreeMessage(msg);
					}
				}
			} while (msg);
		}
}

