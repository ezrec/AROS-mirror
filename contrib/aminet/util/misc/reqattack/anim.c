#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <devices/timer.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/layers_lib.h>
#else
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#endif

#include <clib/alib_protos.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <functions.h>

#include "global.h"
#include "myimage.h"
#include "anim.h"

#include "myimage_protos.h"

#define KILLCLIPREGION


static char *TASKNAME = "ReqAttack Animation HQ";

static struct MsgPort animport;
static struct Process *animtask;
static struct List animlist;
static struct Task *maintask;
static struct RastPort animrp;
static struct MsgPort timerport;
static struct timerequest timerio;
static APTR mempool;
static ULONG timermask,animmask;
static BOOL animtaskok;


void InitAnim(void)
{
	NewList(&animlist);
	maintask = FindTask(0);

	if (!(mempool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR,
										sizeof(struct AnimNode) * 10,
										sizeof(struct AnimNode) * 10)))
	{
		Cleanup("InitAnim: Out of memory!");
	}

	CacheClearU();

	if (!(animtask = CreateNewProcTags(NP_Entry,(LONG)AnimTask,
												  NP_StackSize,4096,
												  NP_Priority,-1,
												  NP_WindowPtr,-1,
												  NP_Name,(LONG)TASKNAME,
												  TAG_DONE)))
	{
		Cleanup("Can't create Animation Process!");
	}

	Wait(SIGF_ROGER);

	if (!animtaskok)
	{
		Cleanup("Initialization of Animation Process failed!");
	}

}

void CleanupAnim(void)
{
	if (animtask)
	{
		Signal(&animtask->pr_Task,SIGBREAKF_CTRL_C);
		Wait(SIGF_ROGER);
	}

	if (mempool) DeletePool(mempool);
}

WORD SendAnimMsg(WORD code,struct RastPort *rp,struct MyImage *im,WORD x,WORD y)
{
	struct AnimMsg msg;
	struct MsgPort replyport;

	replyport.mp_Node.ln_Type = NT_MSGPORT;
	replyport.mp_Flags = PA_SIGNAL;
	replyport.mp_SigBit = SIGB_SINGLE;
	replyport.mp_SigTask = FindTask(0);
	NewList(&replyport.mp_MsgList);

	SetSignal(0,SIGF_SINGLE);

	msg.msg.mn_ReplyPort = &replyport;
	msg.msg.mn_Length = sizeof(msg);

	msg.code = code;
	msg.rp = rp;
	msg.im = im;
	msg.x = x;
	msg.y = y;

	PutMsg(&animport,&msg.msg);
	WaitPort(&replyport);

	return msg.code;
}

void AnimTask(void)
{
	#ifdef KILLCLIPREGION

	struct Region *oldregion;

	#endif

	struct AnimMsg *msg;
	struct AnimNode *node,*succ;
	ULONG sigs;
	WORD waitingtics = 0, ticstowait = 0;
	BOOL timersent = FALSE;

	animport.mp_Node.ln_Type = NT_MSGPORT;
	animport.mp_Flags = PA_SIGNAL;
	animport.mp_SigBit = AllocSignal(-1);
	animport.mp_SigTask = FindTask(0);
	NewList(&animport.mp_MsgList);

	animmask = 1L << animport.mp_SigBit;

	timerport.mp_Node.ln_Type = NT_MSGPORT;
	timerport.mp_Flags = PA_SIGNAL;
	timerport.mp_SigBit = AllocSignal(-1);
	timerport.mp_SigTask = FindTask(0);
	NewList(&timerport.mp_MsgList);

	timermask = 1L << timerport.mp_SigBit;

	timerio.tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	timerio.tr_node.io_Message.mn_Length = sizeof(struct timerequest);
	timerio.tr_node.io_Message.mn_ReplyPort = &timerport;

	if (!OpenDevice("timer.device",UNIT_VBLANK,&timerio.tr_node,0))
	{
		animtaskok = TRUE;

		Signal(maintask,SIGF_ROGER);

		while(1)
		{
			sigs = Wait(SIGBREAKF_CTRL_C | timermask | animmask);

			if (sigs & SIGBREAKF_CTRL_C) break;

			if (sigs & animmask)
			{
				while ((msg = (struct AnimMsg *)GetMsg(&animport)))
				{
					switch(msg->code)
					{
						case ANIMM_START:
							msg->code = ANIMMR_FAIL;

							if ((node = AllocPooled(mempool,sizeof(struct AnimNode))))
							{
								node->rp = msg->rp;
								node->im = msg->im;
								node->destx = msg->x;
								node->desty = msg->y;
								node->tick = node->im->animspeed;
								node->frameinc = 1;
								AddTail(&animlist,&node->node);

								msg->code = ANIMMR_OK;

								if (!timersent)
								{
									waitingtics = node->tick > 1 ? node->tick : 1;

									timerio.tr_node.io_Command = TR_ADDREQUEST;
									timerio.tr_time.tv_secs = TIC_SEC_DELAY(waitingtics);
									timerio.tr_time.tv_micro = TIC_MICRO_DELAY(waitingtics);

									SendIO(&timerio.tr_node);

									timersent = TRUE;
								}

							}
							break;

						case ANIMM_STOP:
							node = (struct AnimNode *)animlist.lh_Head;
							while ((succ = (struct AnimNode *)node->node.ln_Succ))
							{
								if (node->im == msg->im)
								{
									Remove(&node->node);
									FreePooled(mempool,node,sizeof(struct AnimNode));
									break;
								}

								node = succ;
							}

							if (IsListEmpty(&animlist))
							{
								if (timersent)
								{
									if (!CheckIO(&timerio.tr_node)) AbortIO(&timerio.tr_node);
									WaitIO(&timerio.tr_node);

									GetMsg(&timerport);
									SetSignal(0,timermask);
									sigs &= ~timermask;

									timersent = FALSE;
								}
							}
							break;

					} /* switch(msg->code) */

					ReplyMsg(&msg->msg);

				} /* while ((msg = (struct AnimMsg *)GetMsg(&animport))) */

			} /* if (sigs & animmask) */

			if (sigs & timermask)
			{
				GetMsg(&timerport);
				SetSignal(0,timermask);

				ticstowait = TICS_PER_SEC * 2;

				node = (struct AnimNode *)animlist.lh_Head;
				while ((succ = (struct AnimNode *)node->node.ln_Succ))
				{
					node->tick -= waitingtics;

					if (node->tick <= 0)
					{
						node->tick = node->im->animspeed;

						node->activeframe += node->frameinc;

						if ((node->activeframe >= node->im->frames) ||
							 (node->activeframe < 0))
						{
							if (node->im->flags & MYIMF_PINGPONG)
							{
								node->frameinc = -node->frameinc;
								node->activeframe += node->frameinc * 2;
							} else {
								node->activeframe = 0;
							}
						};

						animrp = *node->rp;

						#ifdef KILLCLIPREGION

						LockLayer(0,animrp.Layer);
						oldregion = InstallClipRegion(animrp.Layer,0);

						#endif

						DrawMyImage(&animrp,node->im,node->destx,node->desty,node->activeframe);

						#ifdef KILLCLIPREGION

						InstallClipRegion(animrp.Layer,oldregion);
						UnlockLayer(animrp.Layer);

						#endif
					}

					if (node->tick < ticstowait)
					{
						ticstowait = node->tick;
					}

					node = succ;
				}

				waitingtics = ticstowait >= 1 ? ticstowait : 1;

				timerio.tr_node.io_Command = TR_ADDREQUEST;
				timerio.tr_time.tv_secs = TIC_SEC_DELAY(waitingtics);
				timerio.tr_time.tv_micro = TIC_MICRO_DELAY(waitingtics);
				SendIO(&timerio.tr_node);
				timersent = TRUE;

			} /* if (sigs & timermask) */

		} /* while(1) */

		if (timersent)
		{
			if (!CheckIO(&timerio.tr_node)) AbortIO(&timerio.tr_node);
			WaitIO(&timerio.tr_node);
		}

		CloseDevice(&timerio.tr_node);

	} /* if (!OpenDevice("timer.device",UNIT_VBLANK,&timerio.tr_node,0)) */

	Forbid();
	Signal(maintask,SIGF_ROGER);
}
