// about.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dostags.h>
#include <graphics/gfx.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <libraries/mcpgfx.h>
#include <libraries/gadtools.h>
#include <libraries/ttengine.h>
#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <datatypes/pictureclass.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define __USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/layers.h>
#include <proto/diskfont.h>
#include "debug.h"
#include <proto/scalos.h>
#include <proto/scalosplugin.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <Year.h> // +jmc+
#include <ScalosMcpGfx.h>

#include <scalos/scalos.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"
#include "locale.h"
#include "about.h"
#include "TTLayout.h"
#include "DtImageClass.h"

//----------------------------------------------------------------------------

#define ABOUT_BITMAP_HEIGHT	150

/*  text: first bytes:
 *  0 = Font: n = normal, b =bold, i = italic, j = bold+italic
 *  1 = DRIPen: 0..9 (ascii)
 *  2 = Justify: l = left, r = right, c = centered
 *  3-5 = SkipLines: 3 digits (decimal) = height in pixels to skip after this textline
 */
#define FmtHeader_Length	6

#define	ABOUT_GADGET_IMAGE_BASENAME	"THEME:About"

#define	ABOUT_NUMBER_OF_GADGETS	5

//----------------------------------------------------------------------------

/* Note: the only good thing I can think of about the underscores being there
 * is that we won't have any name conflicts even if we give them the same name
 * in the C files, cos they'll all have underscores added :)
 */

static CONST_STRPTR bSysinfomodule = "SystemInfo.module";

static STRPTR  bAboutname = "Scalos_About";		/* Name of the task & command which gets run for the about window */
static const STRPTR  bCopyright = "© Copyright 2000" CURRENTYEAR "The Scalos Team";

enum AboutGadgetID
	{
	ABOUT_GADGETID_OK = 100,
	ABOUT_GADGETID_STOP,
	ABOUT_GADGETID_SYSINFO,
	ABOUT_GADGETID_FLUSH,
	ABOUT_GADGETID_REBOOT
	};

//----------------------------------------------------------------------------

static LONG RunAbout(struct internalScaWindowTask *iwt, LONG lOkenabled, struct MsgPort *mp_Replyport);
static ULONG NewAbout(APTR dummy, struct SM_RunProcess *msg);
static BOOL ReadAboutBackground(struct DatatypesImage **Logo);
static void GenerateLogo(struct AboutInfo *abi);
static STRPTR ExpandText(CONST_STRPTR Text);
static void FreeExpandedText(STRPTR Text);
static struct ttDef *CreateDiskPluginList(struct AboutInfo *abi);
static struct ttDef *CreateAboutText(struct AboutInfo *abi_About, ULONG MsgId);
static BOOL InitAboutBitMap(struct AboutBitMap *abm, LONG Width, LONG Height, struct BitMap *windowBM);
static void CleanupAboutBitMap(struct AboutBitMap *abm);
static struct AboutGadgetInfo *AboutCreateButton(const struct AboutGadgetDef *agd, struct Gadget *PrevGadget);
static void AboutFreeButton(struct AboutGadgetInfo *agi);
static struct IntuiMessage *ScrollUp(struct AboutInfo *abi_Scroll, LONG lLines, LONG lXoffset, LONG lWidth, LONG ttHeight);
static void FillRegInfo(STRPTR deststring, size_t MaxLen);
static void ScaFlush(void);
static void ScaReboot(struct Window *win_Parent);
static void SystemInfo(void);
static void InitialShowAbout(struct AboutInfo *abi, LONG lXoffset, LONG lWidth);
static SAVEDS(CONST_STRPTR) AboutGadgetTextHookFunc(struct Hook *hook, Object *o, LONG GadgetID);
static void AboutBackfill(struct AboutInfo *abi);
static void AboutInfoCleanup(struct AboutInfo *ab);

//----------------------------------------------------------------------------

static struct Hook AboutGadgetTextHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(AboutGadgetTextHookFunc), 	// h_Entry + h_SubEntry
	NULL,					// h_Data
	};

//----------------------------------------------------------------------------

/* Simply a stub function for things like the about menu item
 * to call the run the code for the about requester with no
 * reply port
 */
void AboutProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	RunAbout(iwt, 0, NULL);
}



/* Name		runabout()
 * Parameters	LONG ok_enabled = flag to show whether the OK button in the about requester is enabled or not
 *		struct MsgPort *replyport = the message port to send any replies to when finished
 * Returns	LONG = success
 */
static LONG RunAbout(struct internalScaWindowTask *iwt, LONG lOkenabled, struct MsgPort *mp_Replyport)
{
	struct SM_RunProcess	*smrp_Message;	/* Message allocated for running procedure */

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (NULL != (smrp_Message = (struct SM_RunProcess *) SCA_AllocMessage(MTYP_RunProcess, 4)) )
		{
		smrp_Message->ScalosMessage.sm_Message.mn_ReplyPort = mp_Replyport;
		smrp_Message->EntryPoint = NewAbout;
		smrp_Message->Flags = lOkenabled;
		smrp_Message->WindowTask = &iwt->iwt_WindowTask;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ChildProcessRun(iwt,
			&smrp_Message->ScalosMessage,
			NP_Priority, -5,
			NP_CommandName, (ULONG) bAboutname,
			NP_Name, (ULONG) bAboutname,
			TAG_DONE);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}
	return(0);
}



/* Name		NewAbout()
 * Parameters	None
 * Returns	always 0
 * Description	The code for the embedded task which opens the About window
 *		and scrolls the text
 */
static ULONG NewAbout(APTR dummy, struct SM_RunProcess *msg)
{
	struct AboutInfo	*abi;			/* information for about requester */
	LONG			lWidest;		/* Pixel width of widest string for gadgets */
	LONG			lScrollwidth;		/* Width of the scrolling area of the window */
	LONG			lScrollheight;		/* Height of the amount of scrolling that will take place */
	UWORD 			lastGadgetID = 0;
	ULONG 			TickCount = 0;
	struct Gadget		*StopGadget = NULL;
	struct Region		*rg_Region;		/* new region that we want to install for clipping the drawing commands */
	struct Region		*rg_Oldregion;		/* region which was previously set in the window */
	struct Rectangle	ra_Regionsize;		/* size of the above regions */
	struct AboutGadgetInfo	*agi;
	static const ULONG IDCMPFlags =  IDCMP_GADGETUP | IDCMP_GADGETHELP | IDCMP_MOUSEBUTTONS;


	do	{
		static const struct AboutGadgetDef AboutGadgets[] =
			{
			{ "ButtonOkNormal", 	"ButtonOkSelected",	"ButtonOkDisabled", 	MSGID_OKNAME, 			ABOUT_GADGETID_OK, 	LEFTIMAGE },
			{ "ButtonStopNormal", 	"ButtonStopSelected",	"ButtonStopDisabled", 	MSGID_ABOUTHOLDSCROLLNAME, 	ABOUT_GADGETID_STOP, 	LEFTIMAGE },
			{ "ButtonInfoNormal", 	"ButtonInfoSelected",	"ButtonInfoDisabled", 	MSGID_SYSINFONAME, 		ABOUT_GADGETID_SYSINFO, LEFTIMAGE },
			{ "ButtonFlushNormal", 	"ButtonFlushSelected",	"ButtonFlushDisabled", 	MSGID_FLUSHNAME, 		ABOUT_GADGETID_FLUSH, 	LEFTIMAGE },
			{ "ButtonRebootNormal", "ButtonRebootSelected",	"ButtonRebootDisabled", MSGID_REBOOTNAME, 		ABOUT_GADGETID_REBOOT, 	LEFTIMAGE },
			};
		int iQuit;		    /* Flag to show whether to quit secondary event loop or not */
		struct RastPort rp;
		struct IBox ttBox;
		ULONG n;
		LONG innerWidth;
		struct Gadget *PrevGadget = NULL;
		LONG MaxButtonWidth;
		LONG MaxButtonHeight;
		LONG GadgetLeft, GadgetTop;
		LONG WindowHeight;
		BOOL DisableSysInfo = TRUE;   // only enable SysInfo Gadget if sysinfo modules available
		BPTR ModsDirLock = LockScaModsDir();

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		/* Try to allocate memory to hold ABoutInfo structure (which is where everything to
		 * do with the about window is stored.
		 */
		abi = ScalosAlloc(sizeof(struct AboutInfo));
		if (NULL == abi)
			break;

		memset(abi, 0, sizeof(struct AboutInfo));

		NewList(&abi->abi_GadgetList);

		d1(KPrintF("%s/%s/%ld: pref_UseScreenTTFont=%ld  FontDesc=<%s>\n", \
			__FILE__, __FUNC__, __LINE__, CurrentPrefs.pref_UseScreenTTFont, \
                        CurrentPrefs.pref_ScreenTTFontDescriptor));

		if (CurrentPrefs.pref_UseScreenTTFont)
			abi->abi_ttDesc = CurrentPrefs.pref_ScreenTTFontDescriptor;
		else
			abi->abi_ttDesc = NULL;

		/* Create a BitMap to hold the Scalos logo. Decode logo from executable
		 * into this bitmap.
		 */
		GenerateLogo(abi);

		ReadAboutBackground(&abi->abi_Background);

		abi->abi_ttd = CreateAboutText(abi, MSGID_ABOUT_TESTTEXT);
		if (NULL == abi->abi_ttd)
			break;

		abi->abi_Font = OpenDiskFont(iInfos.xii_iinfos.ii_Screen->Font);

		for (n = 0; n < Sizeof(AboutGadgets); n++)
			{
			agi = AboutCreateButton(&AboutGadgets[n], PrevGadget);
			if (agi)
				{
				AddTail(&abi->abi_GadgetList, &agi->agi_Node);
				PrevGadget = agi->agi_Gadget;
				}
			else
				break;
			}

		/* Collect width of the generated gadgets */
		lWidest	= 0;
		MaxButtonWidth = MaxButtonHeight = 0;
		for (agi = (struct AboutGadgetInfo *) abi->abi_GadgetList.lh_Head;
			agi != (struct AboutGadgetInfo *) &abi->abi_GadgetList.lh_Tail;
			agi = (struct AboutGadgetInfo *) agi->agi_Node.ln_Succ)
			{
			lWidest	+= 20 + agi->agi_Width;
			MaxButtonHeight = max(MaxButtonHeight, agi->agi_Height);
			MaxButtonWidth = max(MaxButtonWidth, 5 + agi->agi_Width);
			d1(KPrintF("%s/%s/%ld: agi_Width=%lu  agi_Height=%lu\n", __FILE__, __FUNC__, __LINE__, agi->agi_Width, agi->agi_Height));
			}

		Scalos_InitRastPort(&rp);
		SetDrMd(&rp, JAM1);
		TTLayout(&rp, abi->abi_ttd, &ttBox, TTL_Vertical, NULL);

		d1(KPrintF("%s/%s/%ld: Width=%lu  Height=%lu\n", __FILE__, __FUNC__, __LINE__, ttBox.Width, ttBox.Height));

		lWidest = max(lWidest, ttBox.Width);

		/* Get widest width of formatted scroller strings and add some extra space to it */
		lScrollwidth = lWidest;
		lWidest += (iInfos.xii_iinfos.ii_Screen->WBorLeft + iInfos.xii_iinfos.ii_Screen->WBorRight + 14);
		lScrollheight = abi->abi_Font->tf_YSize * 20;
		lScrollheight += (lScrollheight/10 + iInfos.xii_iinfos.ii_Screen->WBorBottom + iInfos.xii_iinfos.ii_Screen->WBorTop);

		WindowHeight = lScrollheight + (2 * MaxButtonHeight);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		/* Try to open about window */
		abi->abi_Window = LockedOpenWindowTags(NULL,
				WA_Title, (ULONG) GetLocString(MSGID_ABOUTNAME),
				WA_CustomScreen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				WA_IDCMP, IDCMPFlags,
				WA_Activate, TRUE,
				WA_RMBTrap, TRUE,
				WA_DepthGadget, TRUE,
				WA_DragBar, TRUE,
				WA_HelpGroup, HelpGroupID,
				WA_Height, WindowHeight,
				WA_Width, lWidest,
				WA_Top, (iInfos.xii_iinfos.ii_Screen->Height - WindowHeight + 1)/2,
				WA_Left, (iInfos.xii_iinfos.ii_Screen->Width - lWidest + 1)/2,
				WA_SCA_Opaqueness, SCALOS_OPAQUENESS(0),
				TAG_DONE);

		if (NULL == abi->abi_Window)
			break;

		Scalos_SetFont(abi->abi_Window->RPort, abi->abi_Font, NULL);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		/* Store various dimensions about the window */
		abi->abi_wInnerleft = abi->abi_Window->BorderLeft;
		abi->abi_wInnerright = abi->abi_Window->BorderRight;
		abi->abi_wInnertop = abi->abi_Window->BorderTop;
		abi->abi_wInnerbottom = abi->abi_Window->BorderBottom;
		abi->abi_wInnerwidth = abi->abi_Window->Width - (abi->abi_wInnerleft + abi->abi_wInnerright);
		abi->abi_wInnerheight = abi->abi_Window->Height - (abi->abi_wInnertop + abi->abi_wInnerbottom);

		// Check if SysInfo module is available
		if (ModsDirLock)
			{
			BPTR siLock;
			BPTR oldDir = CurrentDir(ModsDirLock);

			siLock = Lock((STRPTR) bSysinfomodule, ACCESS_READ);
			if (siLock)
				{
				DisableSysInfo = FALSE;
				UnLock(siLock);
				}

			CurrentDir(oldDir);
			UnLock(ModsDirLock);
			}

		GadgetLeft = abi->abi_Window->Width / (2 * Sizeof(AboutGadgets));
		GadgetTop = abi->abi_wInnertop + abi->abi_wInnerheight - (5 + MaxButtonHeight);

		for (agi = (struct AboutGadgetInfo *) abi->abi_GadgetList.lh_Head;
			agi != (struct AboutGadgetInfo *) &abi->abi_GadgetList.lh_Tail;
			agi = (struct AboutGadgetInfo *) agi->agi_Node.ln_Succ)
			{
			SetAttrs(agi->agi_Gadget,
				GA_Left, GadgetLeft - (agi->agi_Width / 2),
				GA_Top, GadgetTop,
				TAG_END);

			if (ABOUT_GADGETID_STOP == agi->agi_Gadget->GadgetID)
				{
				StopGadget = agi->agi_Gadget;
				SetAttrs(agi->agi_Gadget,
					GA_ToggleSelect, TRUE,
					GA_Selected, FALSE,
					TAG_END);
				}
			else if (ABOUT_GADGETID_SYSINFO == agi->agi_Gadget->GadgetID)
				{
				SetAttrs(agi->agi_Gadget,
					GA_Disabled, DisableSysInfo,
					TAG_END);
				}

			GadgetLeft += abi->abi_Window->Width / Sizeof(AboutGadgets);
			}

		d1(KPrintF("%s/%s/%ld: StopGadget=%08lx\n", __FILE__, __FUNC__, __LINE__, StopGadget));

		/* Try to allocate a new region, which will be used to clip the
		 * drawing of the backfill pattern on the window.
		 */

		rg_Region = NewRegion();
		if (NULL == rg_Region)
			break;

		/* Set size of region to be used for clipping */
		ra_Regionsize.MinX = 0; ra_Regionsize.MinY = 0;
		ra_Regionsize.MaxX = abi->abi_Window->Width;
		ra_Regionsize.MaxY = abi->abi_Window->Height;

		/* Draw a rectangle on the region which drawing will be enabled through it */
		if (!OrRectRegion(rg_Region, &ra_Regionsize))
			break;

		/* Set up another rectangular area for clipping */
		ra_Regionsize.MinX = abi->abi_wInnerleft + 5;
		ra_Regionsize.MinY = abi->abi_wInnertop + 5;
		ra_Regionsize.MaxX = abi->abi_wInnerleft + abi->abi_wInnerwidth - 6;
		ra_Regionsize.MaxY = abi->abi_wInnertop + abi->abi_wInnerheight - (6 + 14) - Scalos_GetFontHeight(abi->abi_Window->RPort);

		/* Clear the rectangle so that all drawing will be clipped inside it */
		if (!ClearRectRegion(rg_Region, &ra_Regionsize))
			break;

		/* Put the new region into the window and get pointer to the current one */
		rg_Oldregion = InstallClipRegion(abi->abi_Window->WLayer, rg_Region);

		AboutBackfill(abi);

		/* restore the original clipping region. we don't check the return value
		 * since it should just be a pointer to the region which we created
		 * (rg_Region) and are now going to free.
		 */
		InstallClipRegion(abi->abi_Window->WLayer, rg_Oldregion);
		DisposeRegion(rg_Region);

		/* Re-use co-ordinates that were used for the last clipping mask (the one
		 * through which all drawing was clipped)
		 */
		ra_Regionsize.MaxX++; ra_Regionsize.MaxY++;
		McpGfxDrawFrame(abi->abi_Window->RPort, ra_Regionsize.MinX, ra_Regionsize.MinY,
			ra_Regionsize.MaxX, ra_Regionsize.MaxY, 
			IA_ShadowPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN],
			IA_HighlightPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
			IA_Recessed, 1,
			IA_FrameType, MF_FRAME_BUTTON,
			TAG_END);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		/* Attach gadgets to window and refresh them */
		AddGList(abi->abi_Window,
			((struct AboutGadgetInfo *) abi->abi_GadgetList.lh_Head)->agi_Gadget,
			-1,
			-1,
			NULL);
		RefreshGList(((struct AboutGadgetInfo *) abi->abi_GadgetList.lh_Head)->agi_Gadget,
			abi->abi_Window,
			NULL,
			Sizeof(AboutGadgets));

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));


		if (!InitAboutBitMap(&abi->abi_Bitmap, 
				lScrollwidth, min(ttBox.Height, ABOUT_BITMAP_HEIGHT), 
				abi->abi_Window->RPort->BitMap))
			break;

		abi->abi_Initial = TRUE;
		abi->abi_DoScroll = TRUE;

		/* Set the dimensions of the scroll area in the about info */
		abi->abi_lScrollsize.MinX = abi->abi_wInnerleft + 7;
		abi->abi_lScrollsize.MinY = abi->abi_wInnertop + 7;
		abi->abi_lScrollsize.MaxX = abi->abi_wInnerleft + abi->abi_wInnerwidth - 8;
		abi->abi_lScrollsize.MaxY = abi->abi_wInnertop + abi->abi_wInnerheight - (8 + 14) - Scalos_GetFontHeight(abi->abi_Window->RPort);

		innerWidth = 1 + abi->abi_lScrollsize.MaxX - abi->abi_lScrollsize.MinX;

		abi->abi_XOffset = (innerWidth - ttBox.Width) / 2;
		abi->abi_bmOffset = 0;
		abi->abi_lCurrentline = INT_MAX;

		d1(kprintf("%s/%s/%ld: w=%ld  ttBox.Width=%ld  abi_XOffset=%ld\n", \
			__FILE__, __FUNC__, __LINE__, (1 + abi->abi_lScrollsize.MaxX - abi->abi_lScrollsize.MinX),\
			ttBox.Width, abi->abi_XOffset));

		FillBackground(abi->abi_Window->RPort, abi->abi_Background,
				abi->abi_lScrollsize.MinX, abi->abi_lScrollsize.MinY, 
				abi->abi_lScrollsize.MaxX, abi->abi_lScrollsize.MaxY,
				0, 0);

		HelpControl(abi->abi_Window, HC_GADGETHELP);	    // Turn on gadget help

		WindowFadeIn(abi->abi_Window);

		do	{
			LONG ScrollWidth = ttBox.Width;
			LONG ScrollXOffset;

			if (ScrollWidth > innerWidth)
				ScrollWidth = innerWidth;

			ScrollXOffset = abi->abi_lScrollsize.MinX + (innerWidth - ScrollWidth)/2;

			iQuit = 0;

			do	{
				ULONG	lClass;			/* type of intuimessage */
				UWORD   iCode;
				APTR	pIaddress;		/* a copy of the IntuiMessage.IAddress field */
				struct IntuiMessage *im_Msg;	/* Message from window IDCMP port */

				im_Msg = ScrollUp(abi, ttBox.Height, ScrollXOffset, ScrollWidth, ttBox.Height);

				if (! im_Msg)
					continue;

				/* This is an IDCMP message, so we find out the information we want
				 * from it, reply to it and then call a function depending on what
				 * gadget had been pressed.
				 */
				lClass = im_Msg->Class;
				iCode = im_Msg->Code;
				pIaddress = im_Msg->IAddress;
				ReplyMsg(&im_Msg->ExecMessage);

				switch (lClass)
					{
				case IDCMP_MOUSEBUTTONS:
					d1(KPrintF("%s/%s/%ld: IDCMP_MOUSEBUTTONS abi=%08lx\n", __FILE__, __FUNC__, __LINE__, abi));
					if (SELECTDOWN == iCode)
						{
						abi->abi_DoScroll = !abi->abi_DoScroll;

						SetGadgetAttrs(StopGadget, abi->abi_Window, NULL,
							GA_Selected, !abi->abi_DoScroll,
							TAG_END);

						RefreshGList(StopGadget, abi->abi_Window, NULL, 1);
						}
					break;

				case IDCMP_INTUITICKS:
					d1(KPrintF("%s/%s/%ld: IDCMP_INTUITICKS abi=%08lx\n", __FILE__, __FUNC__, __LINE__, abi));
					if (++TickCount > 10)
						{
						d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
						ModifyIDCMP(abi->abi_Window, IDCMPFlags);
						TickCount = 0;
						IconWinShowGadgetToolTip((struct internalScaWindowTask *) msg->WindowTask,
							lastGadgetID, &AboutGadgetTextHook);
						}
					break;

				case IDCMP_GADGETHELP:
					d1(KPrintF("%s/%s/%ld: IDCMP_GADGETHELP abi=%08lx\n", __FILE__, __FUNC__, __LINE__, abi));
					if (pIaddress)
						{
						struct Gadget *gad = (struct Gadget *) pIaddress;
						d1(KPrintF("%s/%s/%ld: IDCMP_GADGETHELP GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, gad->GadgetID));

						if (lastGadgetID != gad->GadgetID)
							{
							d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
							lastGadgetID = gad->GadgetID;
							TickCount = 0;
							ModifyIDCMP(abi->abi_Window, IDCMPFlags | IDCMP_INTUITICKS);
							}
						}
					else
						{
						d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
						lastGadgetID = 0;
						TickCount = 0;
						}
					break;

				case IDCMP_GADGETUP:
					{
					struct Gadget *gad = (struct Gadget *) pIaddress;

					d1(KPrintF("%s/%s/%ld: IDCMP_GADGETUP GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, gad->GadgetID));

					switch(gad->GadgetID)
						{
					case ABOUT_GADGETID_OK:		/* OK gadget */
						iQuit = 1; 		/* Simply quit the about window */
						break;

					case ABOUT_GADGETID_STOP:	// Scroll Gadget
						abi->abi_DoScroll = !abi->abi_DoScroll;
						break;

					case ABOUT_GADGETID_SYSINFO:	/* System Info gadget */
						iQuit = 1;
						SystemInfo();
						break;

					case ABOUT_GADGETID_FLUSH:	/* Flush gadget */
						iQuit = 1;
						ScaFlush();
						break;

					case ABOUT_GADGETID_REBOOT:	/* Reboot gadget */
						iQuit = 1;
						ScaReboot(abi->abi_Window);
						break;

					default:	/* Any other(??!?!?!) gadget */
						break;
						}
					} /* if IDCMP_GADGETUP */
					break;

				default:
					d1(KPrintF("%s/%s/%ld: lClass=%08lx\n", __FILE__, __FUNC__, __LINE__, lClass));
					break;
					}

				} while (!iQuit);
			} while (!iQuit);

		Scalos_DoneRastPort(&rp);
		} while (0);

	ResetToolTips((struct internalScaWindowTask *) msg->WindowTask);

	d1(KPrintF("%s/%s/%ld: abi=%08lx\n", __FILE__, __FUNC__, __LINE__, abi));

	AboutInfoCleanup(abi);

	return 0;
}



/* Name		ScaFlush()
 * Parameters	None
 * Returns	Nothing
 * Description	Causes memory to be flushed of all unused items
 */
static void ScaFlush(void)
{
	APTR	pPtr;		/* Pointer to some allocated memory. Don't care, as we free it immediately */

	if (NULL != (pPtr = AllocVec(0x7FFFFFF0L, MEMF_PUBLIC)) )
	{
		FreeVec(pPtr);
	}
}



/* Name		ScaReboot()
 * Parameters	Pointer to the window which the requester should use as a parent
 * Returns	Nothing
 * Description	Asks if the user wants to reboot then reboots if the user selects OK
 */
static void ScaReboot(struct Window *win_Parent)
{
#if (defined(__MORPHOS__) && defined(ShutdownA)) || defined(__AROS__)
#if defined(__MORPHOS__)
	if (DOSBase->dl_lib.lib_Version >= 51)
#elif defined(__AROS__)
	if (1)
#endif
		{
		// On MorphOS 2.x, there is an API to shutdown the machine, so we support both shutdown and reboot
		switch (UseRequest(win_Parent, MSGID_ABOUT_SHUTDOWN_REBOOT_CONFIRM,
				MSGID_ABOUT_CONFIRM_GADGETS, NULL))
			{
		case 1:		// Reboot
			ColdReboot();
			break;
		case 2:		// Shutdown
			ShutdownA(0);
			break;
		default:	// Cancel
			return;
			break;
			}
		}
#endif /* __MORPHOS__ || __AROS__*/
	if (UseRequest(win_Parent, MSGID_SUREREBOOTNAME, MSGID_OKCANCELNAME, NULL))
		{
		ColdReboot();
		}
}



/* Name		SystemInfo()
 * Parameters	None
 * Returns	Nothing
 * Description	Tries to run the systeminfo.module
 */
static void SystemInfo(void)
{
	struct WBArg	wa_Module;	/* A WBArg to run the (systeminfo) module from */

	if ((BPTR)NULL != (wa_Module.wa_Lock = LockScaModsDir()) )
	{
		wa_Module.wa_Name = (STRPTR) bSysinfomodule;

		SCA_WBStartTags(&wa_Module, 1,
			SCA_Stacksize, 16384,
			TAG_END);
		/* Should probably check the return code for this and unlock it if the
		 * call fails (depending on whether LockScaModsDir actually keeps the
		 * lock in the first place)
		 */
	}
}



/* Name		ScrollUp
 * Parameters	struct AboutInfo *abi_Scroll - the window to scroll
 *		LONG lLines - number of lines to scroll up
 *		LONG lXoffset - x position offset to be scrolling from
 *		LONG lWidth - width of area to scroll
 * Returns	Pointer to an IntuiMessage if any gadgets pressed
 * Description	Scrolls an area of the window up while blitting new data to the window
 */
static struct IntuiMessage *ScrollUp(struct AboutInfo *abi, LONG lLines, LONG lXoffset, LONG lWidth, LONG ttHeight)
{
	struct IntuiMessage *im_Msg;	/* IntuiMessage we receive when gadgets pressed */
	LONG bmWidth = GetBitMapAttr(abi->abi_Bitmap.abm_RastPort.BitMap, BMA_WIDTH);
	LONG bmHeight = GetBitMapAttr(abi->abi_Bitmap.abm_RastPort.BitMap, BMA_HEIGHT);

	abi->abi_LineCount = 0;

	d1(kprintf("%s/%s/%ld: bmWidth=%ld  bmHeight=%ld\n", __FILE__, __FUNC__, __LINE__, bmWidth, bmHeight));

	if (abi->abi_Initial)
		InitialShowAbout(abi, lXoffset, lWidth);

	do      {
		im_Msg = (struct IntuiMessage *) GetMsg(abi->abi_Window->UserPort);
		if (NULL == im_Msg)
			{
			if (abi->abi_lCurrentline >= bmHeight)
				{
				d1(kprintf("%s/%s/%ld: abi->abi_bmOffset=%ld\n", __FILE__, __FUNC__, __LINE__, abi->abi_bmOffset));

				SetDrMd(&abi->abi_Bitmap.abm_RastPort, JAM1);
				FillBackground(&abi->abi_Bitmap.abm_RastPort, abi->abi_Background,
						0, 0,
						bmWidth - 1, bmHeight - 1,
						abi->abi_XOffset, 1 - abi->abi_bmOffset);
				TTRender(&abi->abi_Bitmap.abm_RastPort, abi->abi_ttd, 0, -abi->abi_bmOffset);

				abi->abi_lCurrentline = 0;
				}

			Delay(1);

			if (abi->abi_DoScroll)
				{
				WaitBOVP(&iInfos.xii_iinfos.ii_Screen->ViewPort);
				ScrollRaster(abi->abi_Window->RPort, 0, SCROLLSPEED,
						abi->abi_lScrollsize.MinX, 
						abi->abi_lScrollsize.MinY, 
						abi->abi_lScrollsize.MaxX, 
						abi->abi_lScrollsize.MaxY);

				abi->abi_LineCount += SCROLLSPEED;

				FillBackground(abi->abi_Window->RPort, abi->abi_Background,
						abi->abi_lScrollsize.MinX, abi->abi_lScrollsize.MaxY - SCROLLSPEED + 1,
						abi->abi_lScrollsize.MaxX, abi->abi_lScrollsize.MaxY,
						0, abi->abi_LineCount);

				ClipBlit(&abi->abi_Bitmap.abm_RastPort, 
					0, abi->abi_lCurrentline,
					abi->abi_Window->RPort,
					lXoffset, abi->abi_lScrollsize.MaxY - SCROLLSPEED + 1, 
					lWidth, SCROLLSPEED, 
					ABC | ABNC);

				abi->abi_lCurrentline += SCROLLSPEED;
				abi->abi_bmOffset += SCROLLSPEED;
				}
			}
		else
			{
			/* If the message we receive is the iorequest, we enable gadgets and carry on */
			switch (im_Msg->Class)
				{
			case IDCMP_GADGETUP:
			case IDCMP_MOUSEBUTTONS:
			case IDCMP_GADGETHELP:
			case IDCMP_INTUITICKS:
				return(im_Msg);
				break;
			default:
				ReplyMsg(&im_Msg->ExecMessage);
				break;
				}
			}

		// Check if we have reached the bottom of the scrolling contents
		if (abi->abi_bmOffset > ttHeight)
			{
			// Restart from top
			abi->abi_bmOffset = 0;
			abi->abi_lCurrentline = INT_MAX;
			}
		} while (abi->abi_LineCount < lLines);

	return im_Msg;
}

//----------------------------------------------------------------------------

static struct AboutGadgetInfo *AboutCreateButton(const struct AboutGadgetDef *agd, struct Gadget *PrevGadget)
{
	STRPTR ImageNameNrm;
	STRPTR ImageNameSel = NULL;
	STRPTR ImageNameDisabled = NULL;
	struct AboutGadgetInfo *agi = NULL;
	BOOL Success = FALSE;

	do	{
		ImageNameNrm = AllocPathBuffer();
		if (NULL == ImageNameNrm)
			break;

		ImageNameSel= AllocPathBuffer();
		if (NULL == ImageNameSel)
			break;

		ImageNameDisabled = AllocPathBuffer();
		if (NULL == ImageNameDisabled)
			break;

		agi = ScalosAlloc(sizeof(struct AboutGadgetInfo));
		if (NULL == agi)
			break;

		d1(KPrintF("%s/%s/%ld: NormalImgName=<%s>\n", __FILE__, __FUNC__, __LINE__, NormalImgName));

		memset(agi, 0, sizeof(struct AboutGadgetInfo));

		stccpy(ImageNameNrm, ABOUT_GADGET_IMAGE_BASENAME, Max_PathLen);
		AddPart(ImageNameNrm, agd->agd_ImageNameNormal, Max_PathLen);

		stccpy(ImageNameSel, ABOUT_GADGET_IMAGE_BASENAME, Max_PathLen);
		AddPart(ImageNameSel, agd->agd_ImageNameSelected, Max_PathLen);

		stccpy(ImageNameDisabled, ABOUT_GADGET_IMAGE_BASENAME, Max_PathLen);
		AddPart(ImageNameDisabled, agd->agd_ImageNameDisabled, Max_PathLen);

		// imgNormal is always required
		agi->agi_Image = NewObject(DtImageClass, NULL,
			DTIMG_ImageName, (ULONG) ImageNameNrm,
			DTIMG_SelImageName, (ULONG) ImageNameSel,
			DTIMG_DisabledImageName, (ULONG) ImageNameDisabled,
			TAG_END);
		d1(KPrintF("%s/%s/%ld: agi_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, agi->agi_Image));
		if (NULL == agi->agi_Image)
			{
			struct RastPort rp;
			struct TextExtent Extent;
			struct TextFont *font = NULL;
			CONST_STRPTR GadgetText = GetLocString(agd->agd_GadgetTextID);

			memset(&Extent, 0, sizeof(Extent));

			InitRastPort(&rp);
			font = OpenDiskFont(iInfos.xii_iinfos.ii_Screen->Font);

			Scalos_SetFont(&rp, font, NULL);

			Scalos_TextExtent(&rp, GadgetText, strlen(GadgetText), &Extent);

			Scalos_CloseFont(&font, NULL);

			d1(KPrintF("%s/%s/%ld: te_Width=%ld  te_height=%ld\n", __FILE__, __FUNC__, __LINE__, Extent.te_Width, Extent.te_Height));

			agi->agi_Width = 10 + Extent.te_Width;
			agi->agi_Height = 6 + Extent.te_Height;

			agi->agi_Themed = FALSE;

			// no Theme image could be found - fall back to sys image
			agi->agi_Image = NewObject(NULL, FRAMEICLASS,
					IA_Width, agi->agi_Width,
					IA_Height, agi->agi_Height,
					SYSIA_DrawInfo, (ULONG) iInfos.xii_iinfos.ii_DrawInfo,
					IA_FrameType, FRAME_BUTTON,
					TAG_END);
			d1(KPrintF("%s/%s/%ld: agi_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, agi->agi_Image));
			if (NULL == agi->agi_Image)
				break;

			agi->agi_Gadget = (struct Gadget *) NewObject(NULL, FRBUTTONCLASS,
				GA_Width, agi->agi_Width,
				GA_Height, agi->agi_Height,
				GA_ID, agd->agd_GadgetID,
				GA_Text, GadgetText,
				GA_RelVerify, TRUE,
				GA_GadgetHelp, TRUE,
				GA_Image, (ULONG) agi->agi_Image,
				PrevGadget ? GA_Previous : TAG_IGNORE, (ULONG) PrevGadget,
#if defined(GA_LabelPlace)
				GA_LabelPlace, GV_LabelPlace_In,
#endif //defined(GA_LabelPlace)
				TAG_END);
			d1(KPrintF("%s/%s/%ld: agi_Gadget=%08lx\n", __FILE__, __FUNC__, __LINE__, agi->agi_Gadget));
			}
		else
			{
			agi->agi_Themed = FALSE;

			GetAttr(IA_Width, agi->agi_Image, &agi->agi_Width);
			GetAttr(IA_Height, agi->agi_Image, &agi->agi_Height);

			agi->agi_Gadget = (struct Gadget *) SCA_NewScalosObjectTags("ButtonGadget.sca",
				GA_ID, agd->agd_GadgetID,
				GA_RelVerify, TRUE,
				GA_GadgetHelp, TRUE,
				GA_Image, (ULONG) agi->agi_Image,
				PrevGadget ? GA_Previous : TAG_IGNORE, (ULONG) PrevGadget,
				TAG_END);
			}
		d1(KPrintF("%s/%s/%ld: agi_Gadget=%08lx\n", __FILE__, __FUNC__, __LINE__, agi->agi_Gadget));
		if (NULL == agi->agi_Gadget)
			break;

		Success = TRUE;
		} while (0);

	if (ImageNameNrm)
		FreePathBuffer(ImageNameNrm);
	if (ImageNameSel)
		FreePathBuffer(ImageNameSel);
	if (ImageNameDisabled)
		FreePathBuffer(ImageNameDisabled);
	if (!Success)
		{
		AboutFreeButton(agi);
		agi = NULL;
		}

	return agi;
}


static void AboutFreeButton(struct AboutGadgetInfo *agi)
{
	if (agi)
		{
		if (agi->agi_Image)
			{
			DisposeObject(agi->agi_Image);
			agi->agi_Image = NULL;
			}
		if (agi->agi_Gadget)
			{
			if (agi->agi_Themed)
				SCA_DisposeScalosObject((Object *) agi->agi_Gadget);
			else
				DisposeObject((Object *) agi->agi_Gadget);
			agi->agi_Gadget	= NULL;
			}
		ScalosFree(agi);
		}
}


/* Name		GenerateLogo()
 * Parameters	struct AboutInfo *abi - pointer to about info structure for this window
 * Description	Generates a properly remapped bitmap with the scalos logo on it
 */
static void GenerateLogo(struct AboutInfo *abi)
{
	ReadScalosLogo(&abi->abi_Logo);
}



/* Name		FillRegInfo()
 * Parameters	Pointer to area of memory to store the registration string in
 *	maximum length of memory area
 * Returns	Nothing
 * Description	Extracts the user information from the keyfile and stores it in
 *		the string to be displayed.
 */
static void FillRegInfo(STRPTR bRegString, size_t MaxLen)
{
	strcpy(bRegString, GetLocString(MSGID_ABOUT_UNREGTEXT));
	strcat(bRegString, "\n");

	#ifdef COMMERCIAL
	if (pKeyfileaddr != NULL)
	#endif
		{
		STRPTR	bStartstring;	    /* Pointer to the start of the current line */
		CONST_STRPTR  bReguser;	    /* Pointer to start of name string for registered user */
		BYTE	bRegchar;	    /* Current character from the registered owner name */

		strcpy(bRegString, "\n");
		MaxLen -= strlen(bRegString);
		bRegString += strlen(bRegString);

		#ifdef COMMERCIAL
		bReguser = pKeyfileaddr;
		bReguser += 28;
		#else
		bReguser = GetLocString(MSGID_ABOUT_COMREGINFO);
		#endif

		d1(kprintf("%s/%s/%ld: bRegUser=<%s>\n", __FILE__, __FUNC__, __LINE__, bReguser));

		strcpy(bRegString, GetLocString(MSGID_ABOUT_REGTOTEXT));
		strcat(bRegString, "\n");
		MaxLen -= strlen(bRegString);
		bRegString += strlen(bRegString);

		do
		{
			bStartstring = bRegString;

			strcpy(bRegString, "n2c000");
			MaxLen -= strlen(bRegString);
			bRegString += strlen(bRegString);

			do
			{
				bRegchar = *bReguser++;
				if ((UBYTE)bRegchar > '\n')
				{
					*bRegString++ = bRegchar;
					MaxLen--;
				}
				else if (bRegchar == 1)
				{
					*bRegString++ = ' ';
					MaxLen--;
				}
			} while (((UBYTE)bRegchar > '\n' || bRegchar == 1) && MaxLen > 1);

			*bRegString++ = '\n';
			MaxLen--;

			d1(kprintf("%s/%s/%ld: bStartstring=<%s>\n", __FILE__, __FUNC__, __LINE__, bStartstring));

		} while (bRegchar != '\n' && bRegchar && MaxLen > 1);

		strncpy(bStartstring, "n2c004", 6);

		strcpy(bRegString, GetLocString(MSGID_ABOUT_REGTEXT));

		d1(kprintf("%s/%s/%ld: remaining MaxLen=%ld\n", __FILE__, __FUNC__, __LINE__, MaxLen));
		}
}


BOOL ReadScalosLogo(struct DatatypesImage **Logo)
{
	*Logo = CreateDatatypesImage("THEME:ScalosAboutLogo", 0);
	if (NULL == *Logo)
		*Logo = CreateDatatypesImage("THEME:ScalosLogo", 0);

	return (BOOL) (NULL != *Logo);
}



static BOOL ReadAboutBackground(struct DatatypesImage **Logo)
{
	*Logo = CreateDatatypesImage("THEME:AboutBackground", 0);

	return (BOOL) (NULL != *Logo);
}


static STRPTR ExpandText(CONST_STRPTR Text)
{
	size_t Length = strlen(Text);
	CONST_STRPTR lp;
	STRPTR TextCopy, dlp;

	for (lp=Text; *lp; )
		{
		if ('%' == *lp)
			{
			lp++;

			switch (*lp)
				{
			case 'b':		// Build nr
				Length += strlen(ScalosBuildNr);
				lp++;
				break;
			case 'r':		// Revision
				Length += strlen(ScalosRevision);
				lp++;
				break;
			case 'v':		// Version
				Length += 8;	// room for "999.9999"
				lp++;
				break;
			case 'c':		// COMPILER_STRING
				Length += strlen(COMPILER_STRING);
				lp++;
				break;
			case 'p':		// Plugin list
				lp++;
				break;
			case 'l':		// Scalos Logo
				lp++;
				break;
			case 'R':		// Registration info
				Length += 1024;
				lp++;
				break;
			case 'C':		// Copyright +jmc+
				Length += strlen(bCopyright);
				lp++;
				break;
			default:
				break;
				}
			}
		else
			lp++;
		}

	TextCopy = ScalosAlloc(Length);
	if (NULL == TextCopy)
		return NULL;

	for (lp=Text, dlp=TextCopy; *lp; )
		{
		if ('%' == *lp)
			{
			lp++;

			switch (*lp)
				{
			case '%':
				*dlp++ = '%';
				lp++;
				break;
			case 'b':		// Build Nr.
				strcpy(dlp, ScalosBuildNr);
				dlp += strlen(dlp);
				lp++;
				break;
			case 'r':		// Revision
				strcpy(dlp, ScalosBase->scb_Revision);
				dlp += strlen(dlp);
				lp++;
				break;
			case 'v':		// Version
				d1(KPrintF("%s/%s/%ld: Version: %ld\n", __FILE__, __FUNC__, __LINE__, ScalosBase->scb_LibNode.lib_Version));
				ScaFormatString(dlp, "%ld.%ld",
					(LONG) ScalosBase->scb_LibNode.lib_Version, (LONG) ScalosBase->scb_LibNode.lib_Revision);
				dlp += strlen(dlp);
				lp++;
				break;
			case 'c':		// COMPILER_STRING
				strcpy(dlp, COMPILER_STRING);
				dlp += strlen(dlp);
				lp++;
				break;
			case 'p':		// Plugin list
				*dlp++ = '%';
				*dlp++ = 'p';
				lp++;
				break;
			case 'l':		// Scalos Logo
				*dlp++ = '%';
				*dlp++ = 'l';
				lp++;
				break;
			case 'R':		// Registration info
				{
					STRPTR lpc;

					lpc = ScalosAlloc(1024);
					if (lpc)
						{
						FillRegInfo(lpc, 1024);
						strcpy(dlp, lpc);
						dlp += strlen(dlp);
						ScalosFree(lpc);
						}
					lp++;
				}
				break;
			case 'C':		// Copyright +jmc+
				strcpy(dlp, bCopyright);
				dlp += strlen(dlp);
				lp++;
				break;
			case '\0':
				break;
			default:
				*dlp++ = *lp++;
				break;
				}
			}
		else
			*dlp++ = *lp++;
		}
	*dlp = '\0';

	return TextCopy;
}


static void FreeExpandedText(STRPTR Text)
{
	if (Text)
		ScalosFree(Text);
}


static struct ttDef *CreateDiskPluginList(struct AboutInfo *abi)
{
	struct ttDef *ttd = NULL;
	struct ttDef *ttdLeft = NULL, *ttdCenter=NULL, *ttdRight = NULL;
	struct ScalosTagList stl;
	struct PluginClass *plug;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	do	{
		if (RETURN_OK != ScalosTagListInit(&stl))
			break;

		d1(kprintf("%s/%s/%ld: TagList=%08lx\n", __FILE__, __FUNC__, __LINE__, stl.stl_TagList));

		ScalosTagListNewEntry(&stl, TTSpace(2));
		for (plug=ScalosPluginList; plug; plug = (struct PluginClass *) plug->plug_Node.mln_Succ)
			{
			if (plug->plug_base && plug->plug_filename)
				{
				ScalosTagListNewEntry(&stl, 
					TT_Item,
					(ULONG) TT_CreateItem(TT_Title, FilePart(plug->plug_filename),
							TT_HAlign, GTJ_LEFT,
							TT_TextPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[TEXTPEN],
							TT_Font, (ULONG) iInfos.xii_iinfos.ii_Screen->Font,
							TT_TTFont, abi->abi_ttDesc,
							TT_SpaceLeft, 5,
							End
					);
				}
			}
		ScalosTagListNewEntry(&stl, TTSpace(2));
		ScalosTagListEnd(&stl);

		ttdLeft = TT_CreateItem(TTVert,
					 TT_Members, TT_CreateItemA(stl.stl_TagList),
					 End,
				 End;

		if (NULL == ttdLeft)
			break;


		stl.stl_Index = 0;
		ScalosTagListNewEntry(&stl, TTSpace(2));
		for (plug=ScalosPluginList; plug; plug = (struct PluginClass *) plug->plug_Node.mln_Succ)
			{
			if (plug->plug_base)
				{
				struct ScaOOPPluginBase *ScalosPluginBase;
#ifdef __amigaos4__
				struct ScalosPluginIFace *IScalosPlugin;
#endif
				const struct ScaClassInfo *ci;

				ScalosPluginBase = (struct ScaOOPPluginBase *) plug->plug_base;
#ifdef __amigaos4__
				IScalosPlugin = (struct ScalosPluginIFace *) plug->plug_iface;
#endif
				ci = SCAGetClassInfo();
				if (ci)
					{
					ScalosTagListNewEntry(&stl,
						TT_Item,
						(ULONG) TT_CreateItem(TT_Title, ci->ci_name,
								TT_HAlign, GTJ_LEFT,
								TT_TextPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[TEXTPEN],
								TT_Font, (ULONG) iInfos.xii_iinfos.ii_Screen->Font,
								TT_TTFont, abi->abi_ttDesc,
								End
						);
					}
				}
			}
		ScalosTagListNewEntry(&stl, TTSpace(2));
		ScalosTagListEnd(&stl);

		ttdCenter = TT_CreateItem(TTVert,
					 TT_Members, TT_CreateItemA(stl.stl_TagList),
					 End,
				 End;

		if (NULL == ttdCenter)
			break;



		stl.stl_Index = 0;
		ScalosTagListNewEntry(&stl, TTSpace(2));
		for (plug=ScalosPluginList; plug; plug = (struct PluginClass *) plug->plug_Node.mln_Succ)
			{
			if (plug->plug_base)
				{
				struct ScaOOPPluginBase *ScalosPluginBase;
#ifdef __amigaos4__
				struct ScalosPluginIFace *IScalosPlugin;
#endif
				const struct ScaClassInfo *ci;

				ScalosPluginBase = (struct ScaOOPPluginBase *) plug->plug_base;
#ifdef __amigaos4__
				IScalosPlugin = (struct ScalosPluginIFace *) plug->plug_iface;
#endif
				ci = SCAGetClassInfo();
				if (ci)
					{
					char text[20];

					ScaFormatStringMaxLength(text, sizeof(text), 
						"%3lu.%-3lu", 
						(ULONG) ScalosPluginBase->soob_Library.lib_Version, 
						(ULONG) ScalosPluginBase->soob_Library.lib_Revision);

					d1(kprintf("%s/%s/%ld: text=<%s>  TTDesc=<%s>\n", \
						__FILE__, __FUNC__, __LINE__, text, abi->abi_ttDesc));

					ScalosTagListNewEntry(&stl,
						TT_Item,
						(ULONG) TT_CreateItem(TT_Title, text,
							TT_HAlign, GTJ_LEFT,
							TT_SpaceRight, 5,
							TT_Font, (ULONG) iInfos.xii_iinfos.ii_Screen->Font,
							TT_TTFont, abi->abi_ttDesc,
							TT_TextPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[TEXTPEN],
							End
						);
					}
				}
			}
		ScalosTagListNewEntry(&stl, TTSpace(2));
		ScalosTagListEnd(&stl);

		ttdRight = TT_CreateItem(TTVert,
					 TT_Members, TT_CreateItemA(stl.stl_TagList),
					 End,
				 End;

		if (NULL == ttdRight)
			break;
		} while (0);

	d1(kprintf("%s/%s/%ld: TagList=%08lx\n", __FILE__, __FUNC__, __LINE__, stl.stl_TagList));

	if (stl.stl_TagList)
		{
		ttd = TT_CreateItem(TTHoriz,
					TT_Members, TT_CreateItem(
						TT_Item, ttdLeft,
						TT_Item, ttdCenter,
						TT_Item, ttdRight,
						End,
					TT_DrawFrame, MF_FRAME_XEN,
					End,
				End;

		d1(kprintf("%s/%s/%ld: ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd));
		}

	ScalosTagListCleanup(&stl);

	if (NULL == ttd)
		{
		if (ttdLeft)
			TTDisposeItem(ttdLeft);
		if (ttdCenter)
			TTDisposeItem(ttdCenter);
		if (ttdRight)
			TTDisposeItem(ttdRight);
		}

	d1(kprintf("%s/%s/%ld: ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd));

	return ttd;
}


static struct ttDef *CreateAboutText(struct AboutInfo *abi, ULONG MsgId)
{
	struct ttDef *ttd = NULL;
	struct ScalosTagList stl;
	STRPTR MsgStart, lp;

	memset(&stl, 0, sizeof(stl));

	do	{
		MsgStart = lp = ExpandText(GetLocString(MsgId));
		if (NULL == MsgStart)
			break;

		if (RETURN_OK != ScalosTagListInit(&stl))
			break;

		lp = MsgStart;
		while (*lp)
			{
			CONST_STRPTR lp2;
			size_t Length;
			char cFontstyle;	/* Font style of the text string */
			ULONG textStyle;	/* Font style as passed parameter for TT_TextStyle */
			ULONG Justification;	// Text justification for TT_HAlign
			BYTE bPennum;		/* Pen number to render text in */
			char cJustify;		/* Justify location of the string */
			LONG lSkipline;

			cFontstyle = *lp++;

			switch (cFontstyle)
				{
			case 'n':
				textStyle = FS_NORMAL;
				break;
				
			case 'b':
				textStyle = FSF_BOLD;
				break;
				
			case 'i':
				textStyle = FSF_ITALIC;
				break;
				
			default:
				textStyle = FSF_ITALIC | FSF_BOLD;
				break;
				}

			/* Read pen number from format section (second byte) and set it as the colour to use */
			bPennum = *lp++ - 48;

			/* Read justification and extra lines to skip (3rd and 4th-7th bytes) from the format specifier */
			cJustify = *lp++;
			lSkipline = (lp[0] - '0') * 100 + (lp[1] - '0') * 10 + (lp[2] - '0');
			lp += 3;

			switch (cJustify)
				{
			case 'l':
				Justification = GTJ_LEFT;
				break;
			case 'r':
				Justification = GTJ_RIGHT;
				break;
			default:
				Justification = GTJ_CENTER;
				break;
				}

			d1(kprintf("%s/%s/%ld: Pen=%ld  Justification=%ld  skipLine=%ld\n", \
				__FILE__, __FUNC__, __LINE__, bPennum, Justification, lSkipline));

			d1(kprintf("%s/%s/%ld: lp=<%s>\n", __FILE__, __FUNC__, __LINE__, lp));

			if ('%' == *lp)
				{
				d1(kprintf("%s/%s/%ld: lp=<%s>\n", __FILE__, __FUNC__, __LINE__, lp));

				switch (lp[1])
					{
				case 'p':	// Plugin List
					ScalosTagListNewEntry(&stl, TT_Item, (ULONG) CreateDiskPluginList(abi));
					break;
				case 'l':	// Scalos Logo
					ScalosTagListNewEntry(&stl, 
						TT_Item, 
						(ULONG) TT_CreateItem(TT_Image, abi->abi_Logo,
								TT_HAlign, Justification,
								TAG_END)
						);
					break;
					}

				while (*lp && '\n' != *lp)
					lp++;

				if ('\n' == *lp)
					lp++;
				}
			else
				{
				STRPTR lp3Start, lp3;

				for (lp2 = lp, Length = 0; *lp2 && '\n' != *lp2; lp2++, Length++)
					;

				lp3Start = lp3 = ScalosAlloc(1 + Length);
				if (NULL == lp3)
					break;

				while (*lp && '\n' != *lp)
					*lp3++ = *lp++;
				*lp3 = '\0';

				if ('\n' == *lp)
					lp++;

				d1(kprintf("%s/%s/%ld: lp=<%s>  TTDesc=<%s>\n", __FILE__, __FUNC__, __LINE__, lp3Start, abi->abi_ttDesc));

				ScalosTagListNewEntry(&stl, 
					TT_Item,
					(ULONG) TT_CreateItem(TT_LayoutMode, TTL_Horizontal,
					TT_Members, TT_CreateItem(TT_Title, lp3Start,
							TT_HAlign, Justification,
							TT_TextStyle, textStyle,
							TT_Font, (ULONG) iInfos.xii_iinfos.ii_Screen->Font,
							TT_TTFont, abi->abi_ttDesc,
							TT_TextPen, (ULONG) iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[bPennum],
							TAG_END),
					TAG_END)
					);

				ScalosFree(lp3Start);
				}

			if (lSkipline)
				{
				d1(kprintf("%s/%s/%ld: Skipline=%lu\n", __FILE__, __FUNC__, __LINE__, lSkipline));

				ScalosTagListNewEntry(&stl, 
					TT_Item,
					(ULONG) TT_CreateItem(TT_Space, lSkipline,
						TAG_END)
					);
				}
			}

		ScalosTagListEnd(&stl);
		} while (0);

	if (MsgStart)
		FreeExpandedText(MsgStart);

	if (stl.stl_TagList)
		{
		ttd = TT_CreateItemA(stl.stl_TagList);

		ScalosTagListCleanup(&stl);
		}

	return ttd;
}


static BOOL InitAboutBitMap(struct AboutBitMap *abm, LONG Width, LONG Height, struct BitMap *windowBM)
{
	d1(kprintf("%s/%s/%ld: About RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, &abm->abm_RastPort));

	/* Set up rastport structure in the AboutInfo structure */
	Scalos_InitRastPort(&abm->abm_RastPort);

	/* +dm+ friend bitmap set to NULL */
	abm->abm_BitMap = abm->abm_RastPort.BitMap = AllocBitMap(Width, Height,
			windowBM->Depth,
			BMF_MINPLANES|BMF_CLEAR,
			(GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH) <= 8 ? NULL : windowBM));

	if (NULL == abm->abm_BitMap)
		return FALSE;

	abm->abm_LayerInfo = NewLayerInfo();
	d1(kprintf("%s/%s/%ld: LayerInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, abm->abm_LayerInfo));
	if (NULL == abm->abm_LayerInfo)
		return FALSE;

	abm->abm_Layer = abm->abm_RastPort.Layer = CreateUpfrontLayer(abm->abm_LayerInfo, 
		abm->abm_RastPort.BitMap,
		0, 0, Width - 1, Height - 1,
		LAYERSIMPLE,
		NULL);
	d1(kprintf("%s/%s/%ld: layer=%08lx\n", __FILE__, __FUNC__, __LINE__, abm->abm_Layer));
	if (NULL == abm->abm_Layer)
		return FALSE;

	d1(kprintf("%s/%s/%ld: Text bitmap bytes per row=%ld\n", __FILE__, __FUNC__, __LINE__, (LONG)abm->abm_BitMap->BytesPerRow));

	return TRUE;
}


static void CleanupAboutBitMap(struct AboutBitMap *abm)
{
	if (abm)
		{
		WaitBlit();

		if (abm->abm_Layer)
			{
			DeleteLayer(0, abm->abm_Layer);
			abm->abm_Layer = NULL;
			}
		if (abm->abm_LayerInfo)
			{
			DisposeLayerInfo(abm->abm_LayerInfo);
			abm->abm_LayerInfo = NULL;
			}
		if (abm->abm_BitMap)
			{
			FreeBitMap(abm->abm_BitMap);
			abm->abm_BitMap = NULL;
			}

		Scalos_DoneRastPort(&abm->abm_RastPort);
		}
}

//----------------------------------------------------------------------------

static void InitialShowAbout(struct AboutInfo *abi, LONG lXoffset, LONG lWidth)
{
	LONG bmWidth = GetBitMapAttr(abi->abi_Bitmap.abm_RastPort.BitMap, BMA_WIDTH);
	LONG bmHeight = GetBitMapAttr(abi->abi_Bitmap.abm_RastPort.BitMap, BMA_HEIGHT);
	LONG Height = abi->abi_lScrollsize.MaxY - abi->abi_lScrollsize.MinY + 1;
	LONG y = abi->abi_lScrollsize.MinY;

	SetDrMd(&abi->abi_Bitmap.abm_RastPort, JAM1);

	while (Height)
		{
		LONG UseHeight = min(Height, bmHeight);

		d1(kprintf("%s/%s/%ld: abi->abi_bmOffset=%ld\n", __FILE__, __FUNC__, __LINE__, abi->abi_bmOffset));

		FillBackground(&abi->abi_Bitmap.abm_RastPort, abi->abi_Background,
			0, 0,
			bmWidth - 1, bmHeight - 1,
			abi->abi_XOffset, 1 - abi->abi_bmOffset);
		TTRender(&abi->abi_Bitmap.abm_RastPort, abi->abi_ttd, 0, -abi->abi_bmOffset);

		abi->abi_lCurrentline = 0;

		ClipBlit(&abi->abi_Bitmap.abm_RastPort,
			0, abi->abi_lCurrentline,
			abi->abi_Window->RPort,
			lXoffset, y,
			lWidth,
			UseHeight,
			ABC | ABNC);

		abi->abi_lCurrentline += UseHeight;
		abi->abi_bmOffset += UseHeight;
		y += UseHeight;

		Height -= UseHeight;
		}

	abi->abi_Initial = FALSE;
}

//----------------------------------------------------------------------------

static SAVEDS(CONST_STRPTR) AboutGadgetTextHookFunc(struct Hook *hook, Object *o, LONG GadgetID)
{
	CONST_STRPTR Result = NULL;

	d1(KPrintF("%s/%s/%ld: hook=%08lx  o=%08lx  GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, hook, o, GadgetID));

	switch (GadgetID)
		{
	case ABOUT_GADGETID_OK:
		d1(KPrintF("%s/%s/%ld: ABOUT_GADGETID_OK\n", __FILE__, __FUNC__, __LINE__));
		Result = GetLocString(MSGID_ABOUT_OK_SHORTHELP);
		break;
	case ABOUT_GADGETID_STOP:
		d1(KPrintF("%s/%s/%ld: ABOUT_GADGETID_STOP\n", __FILE__, __FUNC__, __LINE__));
		Result = GetLocString(MSGID_ABOUT_STOP_SHORTHELP);
		break;
	case ABOUT_GADGETID_SYSINFO:
		d1(KPrintF("%s/%s/%ld: ABOUT_GADGETID_SYSINFO\n", __FILE__, __FUNC__, __LINE__));
		Result = GetLocString(MSGID_ABOUT_SYSINFO_SHORTHELP);
		break;
	case ABOUT_GADGETID_FLUSH:
		d1(KPrintF("%s/%s/%ld: ABOUT_GADGETID_FLUSH\n", __FILE__, __FUNC__, __LINE__));
		Result = GetLocString(MSGID_ABOUT_FLUSH_SHORTHELP);
		break;
	case ABOUT_GADGETID_REBOOT:
		d1(KPrintF("%s/%s/%ld: ABOUT_GADGETID_REBOOT\n", __FILE__, __FUNC__, __LINE__));
		Result = GetLocString(MSGID_ABOUT_REBOOT_SHORTHELP);
		break;
	default:
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

// Draw something to the window through the clipping mask we created in our region
static void AboutBackfill(struct AboutInfo *abi)
{
	abi->abi_BackfillImage = CreateDatatypesImage(ABOUT_GADGET_IMAGE_BASENAME "/" "Backfill", 0);

	if (abi->abi_BackfillImage)
		{
		FillBackground(abi->abi_Window->RPort,
			abi->abi_BackfillImage,
                        abi->abi_wInnerleft, abi->abi_wInnertop,
			abi->abi_wInnerleft + abi->abi_wInnerwidth - 1,
			abi->abi_wInnertop + abi->abi_wInnerheight - 1,
			abi->abi_wInnerleft, abi->abi_wInnertop);
		}
	else
		{
		static UWORD wAboutpattern[] = { 0xAAAA, 0x5555 };    /* Pattern to fill the background of the about window */

		McpGfxRectFill(abi->abi_Window->RPort,
			abi->abi_wInnerleft, abi->abi_wInnertop,
			abi->abi_wInnerleft + abi->abi_wInnerwidth - 1,
			abi->abi_wInnertop + abi->abi_wInnerheight - 1,
			IA_APatSize, 1,
			IA_APattern, (ULONG) wAboutpattern,
			IA_FGPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
			TAG_END);
		}
}

//----------------------------------------------------------------------------

static void AboutInfoCleanup(struct AboutInfo *abi)
{
	if (abi)
		{
		struct AboutGadgetInfo *agi;

		CleanupAboutBitMap(&abi->abi_Bitmap);

		if (abi->abi_Window)
			{
			WindowFadeOut(abi->abi_Window);
			LockedCloseWindow(abi->abi_Window);
			}

		while ((agi = (struct AboutGadgetInfo *) RemHead(&abi->abi_GadgetList)))
			{
			AboutFreeButton(agi);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (abi->abi_ttd)
			TTDisposeItem(abi->abi_ttd);

		/* Free logo datatypes object */
		DisposeDatatypesImage(&abi->abi_Logo);

		// free Window backfill
		DisposeDatatypesImage(&abi->abi_BackfillImage);

		// free about scrolling area background
		DisposeDatatypesImage(&abi->abi_Background);

		if (abi->abi_Font)
			Scalos_CloseFont(&abi->abi_Font, NULL);

		/* Free memory allocated for AboutInfo structure */
		ScalosFree((APTR)abi);
		}
}

//----------------------------------------------------------------------------

