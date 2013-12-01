// Splash.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <devices/timer.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <graphics/gfxmacros.h>
#include <intuition/gadgetclass.h>
#include <hardware/blit.h>
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
#include <scalos/GadgetBar.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"


//----------------------------------------------------------------------------

// select relatively high priority for splash process to ensure
// fast reply to messages
#define SPLASH_PROCESS_TASK_PRI	3

#define	SPLASH_MSG_HOLD_TICKS	3		// minimum time for a message to stay visible (in ticks, 50 per seconds)

struct SM_UpdateSplash
	{
	struct ScalosMessage    ScalosMessage;
	ULONG	smu_Command;
	char	smu_String[1];		// String to display - variable length !!!
	};

struct SplashInstance
	{
	char spli_VersionText[256];
	BOOL spli_fInitialized;
	SCALOSSEMAPHORE spli_SplashSema;
	struct MsgPort *spli_SplashPort;
	struct Window *spli_SplashWindow;
	ULONG spli_SplashWindowMask;
	STRPTR spli_LastSplashText;
	struct DatatypesImage *spli_SplashLogo;
	struct DatatypesImage *spli_SplashBackground;
	struct Hook spli_BackFillHook;
	struct Gadget *spli_VersionGadget;
	struct Gadget *spli_CompilerGadget;
	struct Gadget *spli_TextGadget;
	};

struct SplashTimer
	{
	struct MsgPort *spt_ioPort;
	T_TIMEREQUEST *spt_TimeReq;
	BOOL spt_timerOpen;
	BOOL spt_timerPending;		// TRUE when spt_TimeReq is in use
	};

enum SplashCommands { SPLCMD_Update, SPLCMD_Close, SPLCMD_AddUser, SPLCMD_RemUser };

//----------------------------------------------------------------------------

// local functions

static BOOL SendSplashMsg(ULONG Command, CONST_STRPTR Text);
static ULONG SplashProcess(APTR Dummy, struct SM_RunProcess *msg);
static void UpdateSplash(struct SplashInstance *inst, CONST_STRPTR text, BOOL ForceRedraw);
static BOOL OpenSplash(struct SplashInstance *inst, WORD iWidth, WORD iHeight);
static void CloseSplash(struct SplashInstance *inst);
static SAVEDS(ULONG) BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg);
static BOOL InitSplashTimer(struct SplashTimer *Timer);
static void CleanupSplashTimer(struct SplashTimer *Timer);
static BOOL ReadSplashLogo(struct DatatypesImage **Logo);
static void RenderSplashLogo(struct SplashInstance *inst);
static struct Gadget *SplashCreateGadgets(struct SplashInstance *inst, WORD iWidth, WORD *iHeight);
static void SplashUpdateTextGadget(struct Window *win, struct Gadget *gad, CONST_STRPTR NewText);
static void SplashChangeGadgetWidth(struct Window *win, struct Gadget *gad, ULONG NewWidth);

//----------------------------------------------------------------------------

// local Data

struct SplashInstance splashInst;

//----------------------------------------------------------------------------


// returns : 
//	RETURN_OK if no error or dos error code

ULONG InitSplash(struct MsgPort *ReplyPort)
{
///
	struct SplashInstance *inst = &splashInst;

	ScalosInitSemaphore(&inst->spli_SplashSema);

	inst->spli_BackFillHook.h_Data = inst;
	SETHOOKFUNC(inst->spli_BackFillHook, BackFillFunc);

	d1(kprintf("%s/%s/%ld: ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, ReplyPort));

	if (CurrentPrefs.pref_EnableSplash)
		{
		d1(KPrintF("%s/%s/%ld: RunProcess(SplashProcess)\n", __FILE__, __FUNC__, __LINE__));
		if (!RunProcess(&MainWindowTask->mwt.iwt_WindowTask, SplashProcess, 0, NULL, ReplyPort))
			return ERROR_NO_FREE_STORE;

		inst->spli_fInitialized = TRUE;
		}

	return RETURN_OK;
///
}


void SplashAddUser(void)
{
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SendSplashMsg(SPLCMD_AddUser, NULL);
}


void SplashRemoveUser(void)
{
	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SendSplashMsg(SPLCMD_RemUser, NULL);
}


void SplashDisplayProgress(const char *fmt, ULONG NumArgs, ...)
{
///
	struct SplashInstance *inst = &splashInst;
	char text[512];
	va_list args;
	ULONG *ArgList = NULL;
	BOOL SemaLocked = FALSE;

	d1(kprintf("%s/%s/%ld: fmt=<%s>\n", __FILE__, __FUNC__, __LINE__, fmt));

	do	{
		ULONG *pArg;

		if (!inst->spli_fInitialized)
			break;

		if (!CurrentPrefs.pref_EnableSplash)
			break;

		ScalosObtainSemaphore(&inst->spli_SplashSema);
		SemaLocked = TRUE;

		if (NULL == inst->spli_SplashPort || NULL == iInfos.xii_iinfos.ii_Screen)
			break;

		va_start(args, NumArgs);

		ArgList = ScalosAlloc(1 + NumArgs * sizeof(ULONG));
		if (NULL == ArgList)
			break;

		pArg = ArgList;
		while (NumArgs--)
			*pArg++ = va_arg(args, ULONG);

		if (strlen(fmt) > 0)
			ScaFormatStringArgs(text, sizeof(text), fmt, ArgList);
		else
			strcpy(text, "");

		va_end(args);

		d1(KPrintF("%s/%s/%ld: text=<%s>\n", __FILE__, __FUNC__, __LINE__, text));

		if (strlen(text) > 0)
			SendSplashMsg(SPLCMD_Update, text);
		else
			SendSplashMsg(SPLCMD_Close, NULL);
		} while (0);

	if (ArgList)
		ScalosFree(ArgList);
	if (SemaLocked)
		ScalosReleaseSemaphore(&inst->spli_SplashSema);
///
}


static BOOL SendSplashMsg(ULONG Command, CONST_STRPTR Text)
{
///
	struct SplashInstance *inst = &splashInst;
	struct SM_UpdateSplash *msg;
	BOOL Result = FALSE;
	size_t Len = sizeof(struct SM_UpdateSplash) - sizeof(struct SM_CloseWindow);

	d1(KPrintF("%s/%s/%ld: Task=%08lx  inst->spli_SplashPort=%08lx  Cmd=%ld\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL), inst->spli_SplashPort, Command));

	if (!inst->spli_fInitialized || NULL == inst->spli_SplashPort)
		return FALSE;

	if (Text)
		Len += strlen(Text);

	msg = (struct SM_UpdateSplash *) SCA_AllocMessage(MTYP_CloseWindow, Len);

	if (msg)
		{
		struct MsgPort *SplashReplyPort;
		struct SM_UpdateSplash *rMsg;

		SplashReplyPort = CreateMsgPort();
		d1(KPrintF("%s/%s/%ld: SplashReplyPort=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, SplashReplyPort, msg));
		if (SplashReplyPort)
			{
			ULONG WaitMask = 1 << SplashReplyPort->mp_SigBit;
			ULONG SigsReceived;

			msg->smu_Command = Command;

			if (Text)
				strcpy(msg->smu_String, Text);

			msg->ScalosMessage.sm_Message.mn_ReplyPort = SplashReplyPort;
			PutMsg(inst->spli_SplashPort, (struct Message *) msg);

			d1(KPrintF("%s/%s/%ld: Task=%08lx  SplashReplyPort=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL), SplashReplyPort, msg));

			// Wait for reply to make sure update is complete
			d1(kprintf("%s/%s/%ld: before Wait()\n", __FILE__, __FUNC__, __LINE__));
			SigsReceived = Wait(WaitMask);
			d1(kprintf("%s/%s/%ld: after Wait()\n", __FILE__, __FUNC__, __LINE__));

			if (SigsReceived & WaitMask)
				{
				while ((rMsg = (struct SM_UpdateSplash *) GetMsg(SplashReplyPort)))
					{
					d1(KPrintF("%s/%s/%ld: rMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, rMsg));
					SCA_FreeMessage((struct ScalosMessage *) rMsg);
					}
				}

			d1(KPrintF("%s/%s/%ld: Task=%08lx\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)));

			DeleteMsgPort(SplashReplyPort);
			}
		else
			{
			SCA_FreeMessage((struct ScalosMessage *) msg);
			}

		Result = TRUE;
		}

	d1(KPrintF("%s/%s/%ld: Task=%08lx\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)));

	return Result;
///
}


static ULONG SplashProcess(APTR Dummy, struct SM_RunProcess *msg)
{
///
	struct SplashInstance *inst = &splashInst;
	struct SplashTimer CloseTimer;

	memset(&CloseTimer, 0, sizeof(CloseTimer));

	do	{
		struct MsgPort *port;
		BOOL Running = TRUE;
		BOOL Closed = FALSE;
		LONG UserCount = 0;
		ULONG SplashMask, ioMask;
		struct Message *smsg;

		SetTaskPri(FindTask(NULL), SPLASH_PROCESS_TASK_PRI);

		inst->spli_SplashPort = CreateMsgPort();
		if (NULL == inst->spli_SplashPort)
			break;

		d1(kprintf("%s/%s/%ld: inst->spli_SplashPort=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->spli_SplashPort));

		if (!InitSplashTimer(&CloseTimer))
			break;

		d1(kprintf("%s/%s/%ld: inst->spli_SplashPort=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->spli_SplashPort));

		SplashMask = 1 << inst->spli_SplashPort->mp_SigBit;
		ioMask = 1 << CloseTimer.spt_ioPort->mp_SigBit;

		d1(kprintf("%s/%s/%ld: SplashMask=%08lx  ioMask=%08lx\n", __FILE__, __FUNC__, __LINE__, SplashMask, ioMask));

		do	{
			struct SM_UpdateSplash *msg;
			ULONG RcvdMask;

			d1(kprintf("%s/%s/%ld: Mask=%08lx\n", __FILE__, __FUNC__, __LINE__, SplashMask | ioMask));

			RcvdMask = Wait(SplashMask | ioMask | inst->spli_SplashWindowMask);

			d1(kprintf("%s/%s/%ld: RcvdMask=%08lx\n", __FILE__, __FUNC__, __LINE__, RcvdMask));

			if (RcvdMask & SplashMask)
				{
				while ((msg = (struct SM_UpdateSplash *) GetMsg(inst->spli_SplashPort)))
					{
					d1(kprintf("%s/%s/%ld: msg=%08lx  Cmd=%ld  timerPending=%ld\n", __FILE__, __FUNC__, __LINE__, msg, msg->smu_Command, CloseTimer.spt_timerPending));

					if (CloseTimer.spt_timerPending)
						{
						d1(kprintf("%s/%s/%ld: RcvdMask=%08lx\n", __FILE__, __FUNC__, __LINE__, RcvdMask));

						AbortIO((struct IORequest *) CloseTimer.spt_TimeReq);
						WaitIO((struct IORequest *) CloseTimer.spt_TimeReq);
						CloseTimer.spt_timerPending = FALSE;
						RcvdMask &= ~ioMask;
						}

					switch (msg->smu_Command)
						{
					case SPLCMD_AddUser:
						d1(kprintf("%s/%s/%ld: SPLCMD_AddUser UserCount=%ld  timerPending=%ld\n", __FILE__, __FUNC__, __LINE__, UserCount, CloseTimer.spt_timerPending));
						UserCount++;
						break;

					case SPLCMD_RemUser:
						d1(kprintf("%s/%s/%ld: SPLCMD_RemUser UserCount=%ld  timerPending=%ld\n", __FILE__, __FUNC__, __LINE__, UserCount, CloseTimer.spt_timerPending));
						UserCount--;
						if (UserCount <= 0)
							{
							CloseTimer.spt_TimeReq->tr_node.io_Command = TR_ADDREQUEST;
							CloseTimer.spt_TimeReq->tr_time.tv_micro = 0;
							CloseTimer.spt_TimeReq->tr_time.tv_secs = CurrentPrefs.pref_SplashHoldTime;

							BeginIO((struct IORequest *) CloseTimer.spt_TimeReq);
							CloseTimer.spt_timerPending = TRUE;
							}
						break;

					case SPLCMD_Update:
						d1(kprintf("%s/%s/%ld: SPLCMD_Update UserCount=%ld  timerPending=%ld\n", __FILE__, __FUNC__, __LINE__, UserCount, CloseTimer.spt_timerPending));
						if (UserCount <= 0)
							{
							CloseTimer.spt_TimeReq->tr_time.tv_micro = 0;
							CloseTimer.spt_TimeReq->tr_time.tv_secs = CurrentPrefs.pref_SplashHoldTime;

							BeginIO((struct IORequest *) CloseTimer.spt_TimeReq);
							CloseTimer.spt_timerPending = TRUE;
							}

						if (!Closed)
							UpdateSplash(inst, msg->smu_String, FALSE);

						Delay(SPLASH_MSG_HOLD_TICKS);
						break;

					case SPLCMD_Close:
						d1(kprintf("%s/%s/%ld: SPLCMD_Close UserCount=%ld  timerPending=%ld\n", __FILE__, __FUNC__, __LINE__, UserCount, CloseTimer.spt_timerPending));
						Running = FALSE;
						break;
						}

					d1(kprintf("%s/%s/%ld: replying msg=%08lx  ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, \
						msg, msg->ScalosMessage.sm_Message.mn_ReplyPort));

					ReplyMsg((struct Message *) msg);
					}

				d1(kprintf("%s/%s/%ld: Running=%ld\n", __FILE__, __FUNC__, __LINE__, Running));
				}
			if (RcvdMask & ioMask)
				{
				d1(kprintf("%s/%s/%ld: UserCount=%ld  timerPending=%ld\n", __FILE__, __FUNC__, __LINE__, UserCount, CloseTimer.spt_timerPending));
				if (CloseTimer.spt_timerPending)
					{
					WaitIO((struct IORequest *) CloseTimer.spt_TimeReq);
					CloseTimer.spt_timerPending = FALSE;

					Running = FALSE;
					}
				}
			if (RcvdMask & inst->spli_SplashWindowMask)
				{
				struct IntuiMessage *iMsg;

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				while (inst->spli_SplashWindow && (iMsg = (struct IntuiMessage *) GetMsg(inst->spli_SplashWindow->UserPort)))
					{
					ULONG mClass = iMsg->Class;
//					UWORD mCode = iMsg->Code;

					ReplyMsg((struct Message *) iMsg);

					d1(KPrintF("%s/%s/%ld: mClass=%08lx\n", __FILE__, __FUNC__, __LINE__, mClass));

					switch (mClass)
						{
					case IDCMP_MOUSEBUTTONS:
						Closed = TRUE;
						CloseSplash(inst);
						break;

					case IDCMP_CHANGEWINDOW:
						d1(KPrintF("%s/%s/%ld: IDCMP_CHANGEWINDOW\n", __FILE__, __FUNC__, __LINE__));
						EraseRect(inst->spli_SplashWindow->RPort,
							inst->spli_SplashWindow->BorderLeft,
							inst->spli_SplashWindow->BorderTop,
							inst->spli_SplashWindow->Width - inst->spli_SplashWindow->BorderRight - 1,
							inst->spli_SplashWindow->Height - inst->spli_SplashWindow->BorderBottom - 1
							);

						RenderSplashLogo(inst);
						if (inst->spli_LastSplashText)
							UpdateSplash(inst, inst->spli_LastSplashText, TRUE);
						else
							UpdateSplash(inst, "", TRUE);
						break;
					default:
						break;
						}

					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					}
				}

			} while (Running);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ScalosObtainSemaphore(&inst->spli_SplashSema);

		port = inst->spli_SplashPort;
		inst->spli_SplashPort = NULL;

		while ((smsg = GetMsg(port)))
			{
			ReplyMsg(smsg);
			}

		d1(KPrintF("%s/%s/%ld: after ScalosObtainSemaphore\n", __FILE__, __FUNC__, __LINE__));

		CloseSplash(inst);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		DeleteMsgPort(port);

		ScalosReleaseSemaphore(&inst->spli_SplashSema);
		} while (0);

	d1(kprintf("%s/%s/%ld: inst->spli_SplashPort=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->spli_SplashPort));

	CleanupSplashTimer(&CloseTimer);

	if (inst->spli_SplashPort)
		{
		DeleteMsgPort(inst->spli_SplashPort);
		inst->spli_SplashPort = NULL;
		}
		
	return 0;
///
}


static void UpdateSplash(struct SplashInstance *inst, CONST_STRPTR text, BOOL ForceRedraw)
{
///
	d1(kprintf("%s/%s/%ld: text=<%s>\n", __FILE__, __FUNC__, __LINE__, text));

	if (iInfos.xii_iinfos.ii_Screen)
		{
		struct RastPort rp;
		struct TextExtent tExtT, tExtV, tExtS;
		short iWidth, iHeight;
		BOOL NeedDrawText = ForceRedraw;

		d1(kprintf("%s/%s/%ld: text=<%s>\n", __FILE__, __FUNC__, __LINE__, text));

		ScaFormatStringMaxLength(inst->spli_VersionText, sizeof(inst->spli_VersionText),
			GetLocString(MSGID_PROGRESS_SCALOSVERSION), 
			ScalosBase->scb_LibNode.lib_Version,
			ScalosBase->scb_LibNode.lib_Revision,
			(ULONG) ScalosBuildNr);

		rp = iInfos.xii_iinfos.ii_Screen->RastPort;

		Scalos_SetFont(&rp, iInfos.xii_iinfos.ii_Screen->RastPort.Font, &ScreenTTFont);

		Scalos_SetSoftStyle(&rp, FSF_BOLD, FSF_BOLD, &ScreenTTFont);
		Scalos_TextExtent(&rp, inst->spli_VersionText, strlen(inst->spli_VersionText), &tExtV);

		Scalos_SetSoftStyle(&rp, FS_NORMAL, FSF_BOLD, &ScreenTTFont);
		Scalos_TextExtent(&rp, COMPILER_STRING, strlen(COMPILER_STRING), &tExtS);

		Scalos_SetSoftStyle(&rp, FS_NORMAL, FSF_BOLD, &ScreenTTFont);
		Scalos_TextExtent(&rp, text, strlen(text), &tExtT);

		iWidth = max(tExtT.te_Width, max(tExtS.te_Width, tExtV.te_Width)) + 2 * tExtT.te_Height;
		if (iWidth < iInfos.xii_iinfos.ii_Screen->Width/10)
			iWidth = iInfos.xii_iinfos.ii_Screen->Width/10;
		iHeight = tExtT.te_Height * 5;

		d1(kprintf("%s/%s/%ld: iWidth=%ld  iHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iWidth, iHeight));

		if (inst->spli_SplashLogo)
			{
			if (iWidth < inst->spli_SplashLogo->dti_BitMapHeader->bmh_Width + 2*5)
				iWidth = inst->spli_SplashLogo->dti_BitMapHeader->bmh_Width + 2*5;
			iHeight += inst->spli_SplashLogo->dti_BitMapHeader->bmh_Height + 2*5;

			d1(kprintf("%s/%s/%ld: iWidth=%ld  iHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iWidth, iHeight));
			}

		if (inst->spli_SplashWindow)
			{
			WORD jWidth = inst->spli_SplashWindow->Width - inst->spli_SplashWindow->BorderLeft - inst->spli_SplashWindow->BorderRight;
			WORD jHeight = inst->spli_SplashWindow->Height - inst->spli_SplashWindow->BorderTop - inst->spli_SplashWindow->BorderBottom;

			if (jWidth < iWidth || jHeight < iHeight)
				{
				WORD NewWidth = inst->spli_SplashWindow->BorderLeft + inst->spli_SplashWindow->BorderRight + iWidth;
				WORD NewHeight = inst->spli_SplashWindow->BorderTop + inst->spli_SplashWindow->BorderBottom + iHeight;

				WindowToFront(inst->spli_SplashWindow);

				if (NewWidth > inst->spli_SplashWindow->Width || NewHeight > inst->spli_SplashWindow->Height)
					{
					WORD NewLeftEdge = (iInfos.xii_iinfos.ii_Screen->Width - NewWidth) / 2;
					WORD NewTopEdge = (iInfos.xii_iinfos.ii_Screen->Height - NewHeight) / 2;

					ChangeWindowBox(inst->spli_SplashWindow,
						NewLeftEdge, NewTopEdge,
						NewWidth, NewHeight
						);

					SplashChangeGadgetWidth(inst->spli_SplashWindow, inst->spli_VersionGadget, NewWidth - 2 * 10);
					SplashChangeGadgetWidth(inst->spli_SplashWindow, inst->spli_CompilerGadget, NewWidth - 2 * 10);
					SplashChangeGadgetWidth(inst->spli_SplashWindow, inst->spli_TextGadget, NewWidth - 2 * 20);

					NeedDrawText = TRUE;
					}
				}

			d1(kprintf("%s/%s/%ld: iWidth=%ld  iHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iWidth, iHeight));
			}
		else
			{
			if (!OpenSplash(inst, iWidth, iHeight))
				return;

			d1(kprintf("%s/%s/%ld: iWidth=%ld  iHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iWidth, iHeight));

			NeedDrawText = TRUE;
			}

		d1(kprintf("%s/%s/%ld: MinY=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, tExtT.te_Extent.MinY, tExtT.te_Extent.MaxY));

		if (NeedDrawText)
			{
			SplashUpdateTextGadget(inst->spli_SplashWindow, inst->spli_VersionGadget, inst->spli_VersionText);
			SplashUpdateTextGadget(inst->spli_SplashWindow, inst->spli_CompilerGadget, COMPILER_STRING);
			}

		SplashUpdateTextGadget(inst->spli_SplashWindow, inst->spli_TextGadget, text);

		if (inst->spli_LastSplashText)
			FreeCopyString(inst->spli_LastSplashText);

		inst->spli_LastSplashText = AllocCopyString(text);
		}
///
}


static BOOL OpenSplash(struct SplashInstance *inst, WORD iWidth, WORD iHeight)
{
///
	struct Gadget *gList;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, iWidth, iHeight));
	d1(KPrintF("%s/%s/%ld: PubScreen=%08lx\n", __FILE__, __FUNC__, __LINE__, iInfos.xii_iinfos.ii_Screen));

	do	{
		if (ReadSplashLogo(&inst->spli_SplashLogo))
			{
			if (iWidth < inst->spli_SplashLogo->dti_BitMapHeader->bmh_Width + 2*5)
				iWidth = inst->spli_SplashLogo->dti_BitMapHeader->bmh_Width + 2*5;
			iHeight += inst->spli_SplashLogo->dti_BitMapHeader->bmh_Height + 2*5;
			}
		inst->spli_SplashBackground = CreateDatatypesImage("THEME:SplashBackground", 0);

		gList = SplashCreateGadgets(inst, iWidth, &iHeight);
		if (NULL == gList)
			break;

		inst->spli_SplashWindow = LockedOpenWindowTags(NULL,
			WA_Left, (iInfos.xii_iinfos.ii_Screen->Width - iWidth) / 2,
			WA_Top, (iInfos.xii_iinfos.ii_Screen->Height - iHeight) / 2,
			WA_Activate, FALSE,
			WA_InnerWidth, iWidth,
			WA_InnerHeight, iHeight,
			WA_SizeGadget, FALSE,
			WA_DragBar, FALSE,
			WA_DepthGadget, FALSE,
			WA_Borderless, TRUE,
			WA_SmartRefresh, TRUE,
			WA_NewLookMenus, TRUE,
			WA_Gadgets, gList,
			WA_BackFill, &inst->spli_BackFillHook,
#if defined(WA_FrontWindow)
			WA_FrontWindow, TRUE,
#elif defined(WA_StayTop)
			WA_StayTop, TRUE,
#endif /* WA_FrontWindow */
			WA_SCA_Opaqueness, SCALOS_OPAQUENESS(0),
			WA_IDCMP, IDCMP_CHANGEWINDOW | IDCMP_MOUSEBUTTONS,
			WA_PubScreen, iInfos.xii_iinfos.ii_Screen,
			TAG_END);

		d1(kprintf("%s/%s/%ld: inst->spli_SplashWindow=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->spli_SplashWindow));

		if (NULL == inst->spli_SplashWindow)
			break;

		inst->spli_SplashWindowMask = 1 << inst->spli_SplashWindow->UserPort->mp_SigBit;

		Scalos_SetFont(inst->spli_SplashWindow->RPort, iInfos.xii_iinfos.ii_Screen->RastPort.Font, &ScreenTTFont);
		SetAPen(inst->spli_SplashWindow->RPort, 1);
		SetDrMd(inst->spli_SplashWindow->RPort, JAM1);

		RenderSplashLogo(inst);

		WindowFadeIn(inst->spli_SplashWindow);

		Success = TRUE;
		} while (0);

	return Success;
///
}


static void CloseSplash(struct SplashInstance *inst)
{
///
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (inst->spli_SplashWindow)
		{
		struct Window *win = inst->spli_SplashWindow;

		inst->spli_SplashWindow = NULL;
		WindowFadeOut(win);
		inst->spli_SplashWindowMask = 0L;
		LockedCloseWindow(win);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	DisposeDatatypesImage(&inst->spli_SplashLogo);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	DisposeDatatypesImage(&inst->spli_SplashBackground);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (inst->spli_LastSplashText)
		{
		FreeCopyString(inst->spli_LastSplashText);
		inst->spli_LastSplashText = NULL;
		}
	if (inst->spli_VersionGadget)
		{
		SCA_DisposeScalosObject((Object *) inst->spli_VersionGadget);
		inst->spli_VersionGadget = NULL;
		}
	if (inst->spli_TextGadget)
		{
		SCA_DisposeScalosObject((Object *) inst->spli_TextGadget);
		inst->spli_TextGadget = NULL;
		}
	if (inst->spli_CompilerGadget)
		{
		SCA_DisposeScalosObject((Object *) inst->spli_CompilerGadget);
		inst->spli_CompilerGadget = NULL;
		}
///
}

// object == (struct RastPort *) result->RastPort
// message == [ (Layer *) layer, (struct Rectangle) bounds,
//              (LONG) offsetx, (LONG) offsety ]

static SAVEDS(ULONG) BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg)
{
///
	struct SplashInstance *inst = (struct SplashInstance *) bfHook->h_Data;
	struct RastPort rpCopy;

	d1(kprintf("%s/%s/%ld: RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));
	d1(kprintf("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
	d1(kprintf("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY));

	rpCopy = *rp;
	rpCopy.Layer = NULL;

	if (inst->spli_SplashBackground)
		{
		WindowBackFill(&rpCopy, msg, inst->spli_SplashBackground->dti_BitMap,
			inst->spli_SplashBackground->dti_BitMapHeader->bmh_Width,
			inst->spli_SplashBackground->dti_BitMapHeader->bmh_Height,
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
///
}


static BOOL InitSplashTimer(struct SplashTimer *Timer)
{
///
	BYTE odError;

	memset(Timer, 0, sizeof(struct SplashTimer));

	Timer->spt_ioPort = CreateMsgPort();
	if (NULL == Timer->spt_ioPort)
		return FALSE;

	d1(kprintf("%s/%s/%ld: ioPort=%08lx\n", __FILE__, __FUNC__, __LINE__, Timer->spt_ioPort));

	Timer->spt_TimeReq = (T_TIMEREQUEST *)CreateIORequest(Timer->spt_ioPort, sizeof(T_TIMEREQUEST));
	if (NULL == Timer->spt_TimeReq)
		return FALSE;

	d1(kprintf("%s/%s/%ld: TimeReq=%08lx\n", __FILE__, __FUNC__, __LINE__, Timer->spt_TimeReq));

	odError = OpenDevice(TIMERNAME, UNIT_VBLANK, &Timer->spt_TimeReq->tr_node, 0);
	d1(kprintf("%s/%s/%ld: OpenDevice returned %ld\n", __FILE__, __FUNC__, __LINE__, odError));
	if (0 != odError)
		return FALSE;

	Timer->spt_timerOpen = TRUE;

	return TRUE;
///
}


static void CleanupSplashTimer(struct SplashTimer *Timer)
{
///
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (Timer->spt_TimeReq)
		{
		if (Timer->spt_timerPending)
			{
			AbortIO((struct IORequest *) Timer->spt_TimeReq);
			WaitIO((struct IORequest *) Timer->spt_TimeReq);

			Timer->spt_timerPending = FALSE;
			}
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		if (Timer->spt_timerOpen)
			{
			CloseDevice(&Timer->spt_TimeReq->tr_node);
			Timer->spt_timerOpen = FALSE;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		DeleteIORequest((struct IORequest *) Timer->spt_TimeReq);
		Timer->spt_TimeReq = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (Timer->spt_ioPort)
		{
		DeleteMsgPort(Timer->spt_ioPort);
		Timer->spt_ioPort = NULL;
		}
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
///
}


static BOOL ReadSplashLogo(struct DatatypesImage **Logo)
{
///
	*Logo  = CreateDatatypesImage("THEME:ScalosSplashLogo", 0);
	if (NULL == *Logo)
		*Logo = CreateDatatypesImage("THEME:ScalosLogo", 0);

	return (BOOL) (NULL != *Logo);
///
}



static void RenderSplashLogo(struct SplashInstance *inst)
{
///
	if (inst->spli_SplashLogo)
		{
		WORD x0, y0;
		struct RastPort rp;

		Scalos_InitRastPort(&rp);

		rp.BitMap = inst->spli_SplashLogo->dti_BitMap;

		y0 = inst->spli_SplashWindow->BorderTop + 5;
		x0 = (inst->spli_SplashWindow->Width - inst->spli_SplashLogo->dti_BitMapHeader->bmh_Width) / 2;

		DtImageDraw(inst->spli_SplashLogo,
			inst->spli_SplashWindow->RPort,
			x0,
			y0,
			inst->spli_SplashLogo->dti_BitMapHeader->bmh_Width,
			inst->spli_SplashLogo->dti_BitMapHeader->bmh_Height
			);

		Scalos_DoneRastPort(&rp);
		}
///
}


static struct Gadget *SplashCreateGadgets(struct SplashInstance *inst, WORD iWidth, WORD *iHeight)
{
///
	struct Gadget *gList;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct TextAttr tAttr;
		struct Gadget *gad;
		struct NewGadget ng;
		LONG iTop;

		AskFont(&iInfos.xii_iinfos.ii_Screen->RastPort, &tAttr);

		iTop = iInfos.xii_iinfos.ii_Screen->WBorTop;

		if (inst->spli_SplashLogo)
			{
			iTop += inst->spli_SplashLogo->dti_BitMapHeader->bmh_Height + 2*5;
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ng.ng_Height = tAttr.ta_YSize + 4;
		ng.ng_LeftEdge = 10;
		ng.ng_TopEdge = iTop;
		ng.ng_Width = iWidth - 10 - 10;

		gad = gList = inst->spli_VersionGadget = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GBTDTA_Text, (ULONG) inst->spli_VersionText,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			GBTDTA_SoftStyle, FSF_BOLD,
			TAG_END);
		d1(kprintf("%s/%s/%ld: gad=%08lx\n", __FILE__, __FUNC__, __LINE__, gad));
		if (NULL == gad)
			break;

		ng.ng_TopEdge += (3 * tAttr.ta_YSize) / 2;

		d1(kprintf("%s/%s/%ld: gad=%08lx\n", __FILE__, __FUNC__, __LINE__, gad));

		gad = inst->spli_CompilerGadget = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GA_Previous, (ULONG) gad,
			GBTDTA_Text, (ULONG) COMPILER_STRING,
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			TAG_END);
		d1(kprintf("%s/%s/%ld: gad=%08lx\n", __FILE__, __FUNC__, __LINE__, gad));
		if (NULL == gad)
			break;

		ng.ng_TopEdge += (3 * tAttr.ta_YSize) / 2;

		gad = inst->spli_TextGadget = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarText.sca",
			GA_Left, ng.ng_LeftEdge,
			GA_Top, ng.ng_TopEdge,
			GA_Width, ng.ng_Width,
			GA_Height, ng.ng_Height,
			GA_Previous, (ULONG) gad,
			GBTDTA_Text, (ULONG) "",
			GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
			GBTDTA_TTFont, (ULONG) &ScreenTTFont,
			GBTDTA_DrawMode, JAM1,
			GBTDTA_Justification, GACT_STRINGCENTER,
			TAG_END);
		d1(kprintf("%s/%s/%ld: gad=%08lx\n", __FILE__, __FUNC__, __LINE__, gad));
		if (NULL == gad)
			break;

		*iHeight = ng.ng_TopEdge + (2 * tAttr.ta_YSize);

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: END  gList=%08lx  Success=%ld\n", __FILE__, __FUNC__, __LINE__, gList, Success));

	return Success ? gList : NULL;
///
}


static void SplashUpdateTextGadget(struct Window *win, struct Gadget *gad, CONST_STRPTR NewText)
{
///
	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	SetGadgetAttrs(gad, win, NULL,
		GBTDTA_Text, (ULONG) NewText,
		TAG_END);
	EraseRect(win->RPort,
		gad->LeftEdge, gad->TopEdge,
		gad->LeftEdge + gad->Width - 1,
		gad->TopEdge + gad->Height -1);
	RefreshGList(gad, win, NULL, 1);
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


static void SplashChangeGadgetWidth(struct Window *win, struct Gadget *gad, ULONG NewWidth)
{
///
	SetGadgetAttrs(gad, win, NULL,
		GA_Width, NewWidth,
		TAG_END);
	EraseRect(win->RPort,
		gad->LeftEdge, gad->TopEdge,
		gad->LeftEdge + gad->Width - 1,
		gad->TopEdge + gad->Height -1);
	RefreshGList(gad, win, NULL, 1);
///
}


