#define Main main
#define __regargs
#define SPrintf sprintf
#include <exec/memory.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>

#include <string.h>
#include <stdio.h>

/*
 *	MeMeter.c - Copyright © 1992 by Devil's child.
 *
 *	Created:	07 Oct 1992  20:14:32
 *	Modified:	18 Mar 1993  18:22:27
 *
 *	Make>> sc <file>.c
 *	Make>> slink LIB:cs.o <file>.o SC SD ND BATCH NOICONS TO <file> LIB LIB:WBStartup.lib
 */


#define BASE_TIME	250000	/* Refresh mem 4 time per second */

#define GAD_TEXT	"   Free Mem   Max Used   Mem Used"
#define GAD_WIDTH	(33 * TF->tf_XSize + 10)
#define GAD_HEIGHT	(TF->tf_YSize + 4)
#define GAD_LEFT	(5 * TF->tf_XSize + 10)
#define V_SPACING	2
#define H_MARGIN	4


struct IntuitionBase *IntuitionBase;
struct Library *GadToolsBase;
struct GfxBase *GfxBase;
char *GadText[] = { "Chip", "Fast", "Total" };

long __regargs Main(char *argv[], struct WBStartup *WBenchMsg)
{
	struct TextFont *TF;
	struct RastPort *rp;
	struct IntuiMessage *IMsg;
	APTR VisualInfo = 0;
	struct NewGadget ng;
	struct TextAttr TextAttr;
	struct timerequest TimeReq;
	long InnerHeight, InnerWidth, i, TimerSignal, Signal, Avail[3];
	struct Gadget *g, *GadTab[3], *GList = NULL;
	struct Window *Win = NULL;
	struct MsgPort *TimerPort = NULL;
	long Snapshot[3] = { 0, 0, 0 }, MaxUsed[3] = { 0, 0, 0 };
	BOOL Quit = FALSE, Dev = 1;

	IntuitionBase = (APTR)OpenLibrary("intuition.library", 0);
	GfxBase = (APTR)OpenLibrary("graphics.library", 0);
	GadToolsBase = OpenLibrary("gadtools.library", 0);

	if (!(TimerPort = CreateMsgPort()))
		goto Exit;
	TimerSignal = 1L << TimerPort->mp_SigBit;
	if (Dev = OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)&TimeReq, 0L))
		goto Exit;
	TimeReq.tr_node.io_Message.mn_ReplyPort = TimerPort;
	TimeReq.tr_node.io_Command = TR_ADDREQUEST;
	TimeReq.tr_node.io_Flags = 0;
	TimeReq.tr_node.io_Error = 0;
	TimeReq.tr_time.tv_secs = 0;
	TimeReq.tr_time.tv_micro = BASE_TIME;

	TF = GfxBase->DefaultFont;
	InnerHeight = (V_SPACING + GAD_HEIGHT) * 4 + V_SPACING;
	InnerWidth = GAD_WIDTH + GAD_LEFT + 2 * H_MARGIN;

    if (!(g = CreateContext(&GList)))
		goto Exit;
	Win = OpenWindowTags(NULL,	WA_Left, 32767,
								WA_Top, 15,
								WA_InnerWidth, InnerWidth,
								WA_InnerHeight, InnerHeight,
								WA_IDCMP, IDCMP_GADGETUP|IDCMP_CLOSEWINDOW,
								WA_Flags, WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH|WFLG_ACTIVATE|WFLG_NOCAREREFRESH,
								WA_Gadgets, GList,
								WA_Title, "MeMeter V2.2",
								WA_AutoAdjust, TRUE,
								TAG_DONE);
	if (!Win)
		goto Exit;
	rp = Win->RPort;
	SetFont(rp, TF);
	AskFont(rp, &TextAttr);		/* Get font as a TextAttr for Gadget Creation */
	if (!(VisualInfo = GetVisualInfo(Win->WScreen, TAG_DONE)))
		goto Exit;
	ng.ng_LeftEdge = Win->BorderLeft + H_MARGIN + GAD_LEFT;
	ng.ng_TopEdge = Win->BorderTop + V_SPACING;
	ng.ng_Width = GAD_WIDTH;
	ng.ng_Height = GAD_HEIGHT;
	ng.ng_TextAttr = &TextAttr;
	ng.ng_GadgetText = GAD_TEXT;
	ng.ng_Flags = PLACETEXT_IN;
	ng.ng_VisualInfo = VisualInfo;
	if (!(g = CreateGadget(BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE)))
		goto Exit;
	ng.ng_Flags = PLACETEXT_LEFT;
	for( i=0 ; i<3 ; i++ ) {
		ng.ng_TopEdge += GAD_HEIGHT + V_SPACING;
		ng.ng_GadgetText = GadText[i];
		if (!(g = CreateGadget(TEXT_KIND, g, &ng, GTTX_Border, TRUE, TAG_DONE)))
			goto Exit;
		GadTab[i] = g;
	}
	RefreshGList(GList, Win, NULL, -1L);
    GT_RefreshWindow(Win, NULL);
	SetDrMd(rp, JAM2);
	SetAPen(rp, 1);
	SetBPen(rp, 0);
	SendIO((struct IORequest *)&TimeReq.tr_node);

	while(!Quit) {
		/* Refresh Memory Meter */
		Avail[0] = AvailMem(MEMF_CHIP);
		Avail[1] = AvailMem(MEMF_FAST);
		Avail[2] = Avail[0] + Avail[1];
		for( i=0 ; i<3 ; i++ ) {
			char Buf[50];
			long diff;

			diff = Snapshot[i]-Avail[i];
			if (MaxUsed[i] < diff)
				MaxUsed[i] = diff;
			SPrintf(Buf, " %10ld %10ld %10ld", Avail[i], MaxUsed[i], diff);
			Move(rp, ng.ng_LeftEdge + 5, Win->BorderTop + V_SPACING + (GAD_HEIGHT + V_SPACING) * (i+1) + TF->tf_Baseline + 2);
			Text(rp, Buf, 33);
//			GT_SetGadgetAttrs(GadTab[i], Win, NULL, GTTX_Text, Buf, TAG_DONE);	/* Blinks on refresh !! */
		}
		Signal = Wait((1L << Win->UserPort->mp_SigBit) | TimerSignal);
		while (IMsg = (struct IntuiMessage *)GetMsg(Win->UserPort)) {
			switch (IMsg->Class) {
			case IDCMP_CLOSEWINDOW:	
				Quit = TRUE;
				break;
			case IDCMP_GADGETUP:
				memcpy(Snapshot, Avail, 3 * sizeof(long));
				memset(MaxUsed, 0, 3 * sizeof(long));
				break;
			}
			ReplyMsg((struct Message *)IMsg);
		}
		if (Signal & TimerSignal) {
			TimeReq.tr_time.tv_secs = 0;
			TimeReq.tr_time.tv_micro = BASE_TIME;
			SendIO((struct IORequest *)&TimeReq.tr_node);
		}
	}
	AbortIO((struct IORequest *)&TimeReq);

Exit:
	if (Win)
		CloseWindow(Win);
	if (VisualInfo)
		FreeVisualInfo(VisualInfo);
	if (GList)
		FreeGadgets(GList);
	if (Dev == 0)
		CloseDevice((struct IORequest *)&TimeReq);
	if (TimerPort)
		DeleteMsgPort(TimerPort);
	CloseLibrary(GadToolsBase);
	CloseLibrary((struct Library *)GfxBase);
	CloseLibrary((struct Library *)IntuitionBase);
	return 0;
}


