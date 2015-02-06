// Window.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/intuitionbase.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <rexx/rxslib.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/rexxsyslib.h>
#include "debug.h"
#include <proto/scalos.h>
#include <proto/scalosgfx.h>
#include <proto/scalosmenuplugin.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/menu.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

#include "IconImageClass.h"

//----------------------------------------------------------------------------

// local data structures

struct AsyncArexxMenuCmdArg
	{
	struct internalScaWindowTask *asm_iwt;
	struct ScalosMenuTree *asm_MenuTree;
	struct ScaIconNode *asm_IconNode;
	};

#undef NO_ICON_POSITION_SHORT
#define	NO_ICON_POSITION_SHORT	((UWORD) 0x8000)

//----------------------------------------------------------------------------

// local functions

static void RunMenuCmd_Internal(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in, ULONG Flags);
static void RunMenuCmd_WBStart(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in);
static void RunMenuCmd_CLI(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in);
static void RunMenuCmd_ARexx(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in);
static SAVEDS(void) AsyncARexxMenuCmdStart(APTR xarg, struct SM_RunProcess *msg);
static void RunMenuCmd_Plugin(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in);
static STRPTR InsertCliArgDeviceName(struct internalScaWindowTask *iwt, STRPTR Buffer,
	size_t *BuffLen, struct ScaIconNode *in);
static STRPTR InsertCliArgs(struct internalScaWindowTask *iwt, STRPTR Buffer, 
	size_t *BuffLen, struct ScaIconNode *in);
static STRPTR InsertARexxArgs(struct internalScaWindowTask *iwt, STRPTR Buffer, 
	size_t *BuffLen, ULONG *ArgCount, struct ScaIconNode *in,
	struct RexxMsg *RxMsg);
static STRPTR InsertName(STRPTR Buffer, size_t *BuffLen, struct WBArg *arg);
static STRPTR InsertDeviceName(STRPTR BuffPtr, size_t *BuffLen, BPTR dirLock);
static STRPTR CliCopy(STRPTR BuffPtr, CONST_STRPTR *pName, size_t *BuffLen, BOOL Quotes);
static void DisposeScalosWindow_IconFound(struct internalScaWindowTask *iwt, 
	struct internalScaWindowTask *iwtIcon, struct ScaIconNode *in);
static void WindowBlitTransparentShadow(struct Window *PopupWindow,
	LONG Left, LONG Top, ULONG Width, ULONG Height,
	LONG ShadowXOffset, LONG ShadowYOffset, ULONG ShadowFlags);

//----------------------------------------------------------------------------

// local data items

// AppIcon message classes for internal menu commands
struct IconMenuSupports AppIconMenuCodes[] =
	{
	{ "open", 		AMCLASSICON_Open	},
	{ "clone",		AMCLASSICON_Copy	},
	{ "rename",		AMCLASSICON_Rename	},
	{ "iconinfo",		AMCLASSICON_Information	},
	{ "snapshot",		AMCLASSICON_Snapshot	},
	{ "unsnapshot",		AMCLASSICON_UnSnapshot	},
	{ "leaveout",		AMCLASSICON_LeaveOut	},
	{ "putaway",		AMCLASSICON_PutAway	},
	{ "delete",		AMCLASSICON_Delete	},
	{ "formatdisk",		AMCLASSICON_FormatDisk	},
	{ "emptytrashcan",	AMCLASSICON_EmptyTrash	},
	{ NULL,			0			},
	};

// Precalculated alpha values for smooth dropshadows
static const UBYTE ShadowTable[] =
	{
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 254, 254, 254, 253, 253, 253,
	252, 252, 251, 250, 250, 249, 249, 248,
	247, 246, 246, 245, 244, 243, 242, 241,
	240, 240, 239, 238, 237, 235, 234, 233,
	232, 231, 230, 229, 227, 226, 225, 224,
	222, 221, 220, 218, 217, 216, 214, 213,
	211, 210, 208, 207, 206, 204, 202, 201,
	199, 198, 196, 195, 193, 191, 190, 188,
	187, 185, 183, 182, 180, 178, 177, 175,
	173, 172, 170, 168, 166, 165, 163, 161,
	160, 158, 156, 154, 153, 151, 149, 148,
	146, 144, 142, 141, 139, 137, 136, 134,
	132, 131, 129, 127, 126, 124, 122, 121,
	119, 117, 116, 114, 113, 111, 109, 108,
	106, 105, 103, 102, 100,  99,  97,  96,
	94,  93,  91,  90,  88,  87,  86,  84,
	83,  81,  80,  79,  77,  76,  75,  73,
	72,  71,  70,  68,  67,  66,  65,  64,
	62,  61,  60,  59,  58,  57,  56,  55,
	54,  53,  52,  51,  50,  49,  48,  47,
	46,  45,  44,  43,  42,  41,  40,  39,
	39,  38,  37,  36,  35,  35,  34,  33,
	32,  32,  31,  30,  30,  29,  28,  28,
	27,  26,  26,  25,  25,  24,  23,  23,
	22,  22,  21,  21,  20,  20,  19,  19,
	18,  18,  17,  17,  16,  16,  16,  15,
	15,  14,  14,  14,  13,  13,  13,  12,
	12,  12,  11,  11,  11,  10,  10,  10,
	10,   9,   9,   9,   9,   8,   8,   8,
	8,   7,   7,   7,   7,   7,   6,   6,
	6,   6,   6,   5,   5,   5,   5,   5,
	};


static ULONG LayersLockCount;		// system-wide count of locks hold on layers
static ULONG IBaseLockNest;		// nesting count for LockIBase()

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------

// ATTENTION: 
// <WinListSemaphore> will stay locked if <*iWinUnderPtr> is not NULL
// (*iWinUnderPtr)->iwt_WindowTask.wt_IconSemaphore will stay locked if <*IconUnderPtr> is not NULL

BOOL QueryObjectUnderPointer(struct internalScaWindowTask **iWinUnderPtr,
	struct ScaIconNode **IconUnderPtr,
	struct ScaIconNode **OuterBoundsIconUnderPtr,
	struct Window **foreignWindow)
{
	struct Layer *PtrLayer;
	struct Screen *scr;
	WORD MouseX, MouseY;
	BOOL ScreenOk;
	ULONG ILock;
	BOOL Success = TRUE;

	*iWinUnderPtr = NULL;
	*IconUnderPtr = NULL;
	*foreignWindow = NULL;

	if (OuterBoundsIconUnderPtr)
		*OuterBoundsIconUnderPtr = NULL;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	Forbid();
	ILock = ScaLockIBase(0);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (scr = IntuitionBase->FirstScreen; scr; scr = scr->NextScreen)
		{
		d1( kprintf("screen=%lx  Offsetx=%ld ", scr, scr->ViewPort.DyOffset) );
		d1( kprintf("x=%ld  y=%ld\n", scr->MouseX, scr->MouseY) );

		if (scr->MouseY >= 0 && scr->MouseY < scr->Height)
			break;
		}

	ScreenOk = NULL != scr && scr == iInfos.xii_iinfos.ii_Screen;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScaUnlockIBase(ILock);
	Permit();

	d1(kprintf("%s/%s/%ld: scr=%08lx\n", __FILE__, __FUNC__, __LINE__, scr));
	if (!ScreenOk)
		return Success;

	PtrLayer = WhichLayer(&scr->LayerInfo, scr->MouseX, scr->MouseY);

	d1(kprintf("%s/%s/%ld: PtrLayer=%08lx\n", __FILE__, __FUNC__, __LINE__, PtrLayer));
	if (NULL == PtrLayer)
		return Success;

	MouseX = scr->MouseX;
	MouseY = scr->MouseY;

	if (PtrLayer && PtrLayer->Window)
		{
		d1(kprintf("%s/%s/%ld: PtrLayer=%08lx  Window=%08lx\n", __FILE__, __FUNC__, __LINE__, PtrLayer, PtrLayer->Window));

		if (SCA_LockWindowList(SCA_LockWindowList_AttemptShared))
			{
			struct ScaWindowStruct *swi = winlist.wl_WindowStruct;
			struct ScaWindowStruct *swif = NULL;	// Window under Mouse

			while (swi && NULL == swif)
				{
				if (swi->ws_Window == PtrLayer->Window)
					{
					swif = swi;
					}
				swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ;
				}

			d1(kprintf("%s/%s/%ld: swif=%08lx\n", __FILE__, __FUNC__, __LINE__, swif));

			if (swif)
				{
				// pointer is over Scalos window
				struct internalScaWindowTask *swit = (struct internalScaWindowTask *) swif->ws_WindowTask;

				*iWinUnderPtr = swit;

				d1(kprintf("%s/%s/%ld: WinUnderPtr=%08lx  swit=%08lx\n", __FILE__, __FUNC__, __LINE__, swit->iwt_WinUnderPtr, swit));

				// Pointer is inside a Scalos Window
				// now check icons

				if (swit->iwt_WindowTask.wt_Window)
					{
					if (ScalosAttemptLockIconListShared(swit))
						{
						struct ScaIconNode *in;
						LONG x, y;

						x = MouseX - swit->iwt_WindowTask.wt_Window->LeftEdge - swit->iwt_InnerLeft;
						y = MouseY - swit->iwt_WindowTask.wt_Window->TopEdge - swit->iwt_InnerTop;

						d1(kprintf("%s/%s/%ld: x=%ld  y=%ld \n", __FILE__, __FUNC__, __LINE__, x, y ));

						if (x >= 0 && x < swit->iwt_InnerWidth
							&& y >= 0 && y < swit->iwt_InnerHeight)
							{
							x += swit->iwt_WindowTask.wt_XOffset;
							y += swit->iwt_WindowTask.wt_YOffset;

							// check if pointer is over icon
							in = CheckMouseIcon(&swit->iwt_WindowTask.wt_IconList, swit, x, y);

							*IconUnderPtr = in;

							if (NULL == in && OuterBoundsIconUnderPtr)
								{
								*OuterBoundsIconUnderPtr = CheckMouseIconOuterBounds(&swit->iwt_WindowTask.wt_IconList, x, y);
								}
							}

						// Release icon semaphore if no icon found.
						if (NULL == *IconUnderPtr)
							ScalosUnLockIconList(swit);
						}
					else
						{
						// ScalosAttemptSemaphoreShared() failed
						Success = FALSE;
						}
					}

				d1(kprintf("%s/%s/%ld:  in=%08lx\n", __FILE__, __FUNC__, __LINE__, *IconUnderPtr));
				}
			else
				{
				// Pointer is over foreign (non-Scalos) window
				*foreignWindow = PtrLayer->Window;
				}

			// relase window semaphore if no ScalosWindow found
			if (NULL == *iWinUnderPtr)
				SCA_UnLockWindowList();
			}
		else
			{
			// AttemptLockWindowListShared() failed
			Success = FALSE;
			}
		}

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


void AdjustIconActive(struct internalScaWindowTask *iwt)
{
	struct ScaIconNode *in;
	ULONG SelectCount = 0;

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		if (gg->Flags & GFLG_SELECTED)
			SelectCount++;
		}

	IconActive = SelectCount != 0;
}


BOOL isCopyQualifier(ULONG Qualifier)
{
	return (BOOL) ((Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT | IEQUALIFIER_CAPSLOCK | 
		IEQUALIFIER_CONTROL | IEQUALIFIER_LALT  | IEQUALIFIER_RALT |
		IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND | 
		IEQUALIFIER_NUMERICPAD)) == CurrentPrefs.pref_CopyQualifier);
}


BOOL isMakeLinkQualifier(ULONG Qualifier)
{
	return (BOOL) ((Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT | IEQUALIFIER_CAPSLOCK | 
		IEQUALIFIER_CONTROL | IEQUALIFIER_LALT  | IEQUALIFIER_RALT |
		IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND | 
		IEQUALIFIER_NUMERICPAD)) == CurrentPrefs.pref_MakeLinkQualifier);
}


BOOL isMoveQualifier(ULONG Qualifier)
{
	return (BOOL) ((Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT | IEQUALIFIER_CAPSLOCK | 
		IEQUALIFIER_CONTROL | IEQUALIFIER_LALT  | IEQUALIFIER_RALT |
		IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND | 
		IEQUALIFIER_NUMERICPAD)) == CurrentPrefs.pref_ForceMoveQualifier);
}


BOOL isAlternateLssoQualifier(ULONG Qualifier)
{
	return (BOOL) ((Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT | IEQUALIFIER_CAPSLOCK |
		IEQUALIFIER_CONTROL | IEQUALIFIER_LALT  | IEQUALIFIER_RALT |
		IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND |
		IEQUALIFIER_NUMERICPAD)) == CurrentPrefs.pref_SingleWindowLassoQualifier);
}


void RunMenuCommand(struct internalScaWindowTask *iwt, 
	struct ScalosMenuTree *mtr, struct ScaIconNode *in, ULONG Flags)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  mtr=%08lx Next=%08lx  in=%08lx  Flags=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, mtr, mtr->mtre_Next, in, Flags));

	if (ScalosAttemptSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore))
		{
		d1(KPrintF("%s/%s/%ld: mtre_Next=%08lx\n", __FILE__, __FUNC__, __LINE__, mtr->mtre_Next));

		for (; mtr; mtr=mtr->mtre_Next)
			{
			d1(KPrintF("%s/%s/%ld: mtr=%08lx  mtre_Next=%08lx  type=%ld\n", __FILE__, __FUNC__, __LINE__, mtr, mtr->mtre_Next, mtr->mtre_type));

			if (SCAMENUTYPE_Command == mtr->mtre_type)
				{
				d1(kprintf("%s/%s/%ld: mcom_type=%ld  name=<%s>\n", \
					__FILE__, __FUNC__, __LINE__, mtr->MenuCombo.MenuCommand.mcom_type,\
					mtr->MenuCombo.MenuCommand.mcom_name));

				switch (mtr->MenuCombo.MenuCommand.mcom_type)
					{
				case SCAMENUCMDTYPE_Command:	// internal command
					RunMenuCmd_Internal(iwt, mtr, in, Flags);
					break;

				case SCAMENUCMDTYPE_Workbench:
					RunMenuCmd_WBStart(iwt, mtr, in);
					break;

				case SCAMENUCMDTYPE_AmigaDOS:
					RunMenuCmd_CLI(iwt, mtr, in);
					break;

				case SCAMENUCMDTYPE_IconWindow:
					if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_BrowserMode)
						{
						DoMethod(iwt->iwt_WindowTask.mt_MainObject,
							SCCM_IconWin_NewPath,
							mtr->MenuCombo.MenuCommand.mcom_name,
							TAG_END);
						}
					else
						{
						SCA_OpenIconWindowTags(SCA_Flags, SCAF_OpenWindow_ScalosPort,
							SCA_Path, (ULONG) mtr->MenuCombo.MenuCommand.mcom_name,
							SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
							TAG_END);
						}
					break;

				case SCAMENUCMDTYPE_ARexx:
					RunMenuCmd_ARexx(iwt, mtr, in);
					break;

				case SCAMENUCMDTYPE_Plugin:
					RunMenuCmd_Plugin(iwt, mtr, in);
					break;
					}
				}
			}

		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
		}
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void RunMenuCommandExt(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtDest,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in, ULONG Flags)
{
	d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in ? GetIconName(in) : (STRPTR) ""));

	if (iwt == iwtDest)
		{
		RunMenuCommand(iwtDest, mtr, in, Flags);
		}
	else
		{
		// Run menu command in other window context
		struct SM_RunMenuCmd *msg = (struct SM_RunMenuCmd *) SCA_AllocMessage(MTYP_RunMenuCmd, 0);

		if (msg)
			{
			msg->ScalosMessage.sm_Message.mn_ReplyPort = iwt->iwt_WindowTask.wt_IconPort;
			msg->smrm_MenuItem = CloneMenuTree(mtr);
			msg->smrm_IconNode = in;
			msg->smrm_Flags = Flags;

			PutMsg(iwtDest->iwt_WindowTask.wt_IconPort, (struct Message *) msg);
			}
		}
}


static void RunMenuCmd_Internal(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in, ULONG Flags)
{
	ULONG IconType = WBTOOL;

	d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in ? GetIconName(in) : (STRPTR) ""));

	if (NULL == mtr->MenuCombo.MenuCommand.mcom_name)
		return;

	if (in)
		{
		GetAttr(IDTA_Type, in->in_Icon, &IconType);
		}

	d1(KPrintF("%s/%s/%ld: in=%08lx IconType=%ld\n", __FILE__, __FUNC__, __LINE__, in, IconType));

	if (WBAPPICON == IconType)
		{
		ULONG n;
	
		d1(kprintf("%s/%s/%ld: iwt=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, in));

		for (n=0; AppIconMenuCodes[n].ims_CommandName; n++)
			{
			if (0 == Stricmp((STRPTR) AppIconMenuCodes[n].ims_CommandName, mtr->MenuCombo.MenuCommand.mcom_name))
				{
				WindowSendAppIconMsg(iwt, AppIconMenuCodes[n].ims_FlagMask, in);
				break;
				}
			}
		}
	else
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MenuCommand,
			mtr->MenuCombo.MenuCommand.mcom_name, in, Flags);
		}
}


static void RunMenuCmd_WBStart(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in)
{
	struct WBArg *args;
	ULONG ArgCount = 1;
	BOOL Success;
	STRPTR lp;
	LONG Priority = 0;
	BYTE ch;
	ULONG NumberOfWbArgs;

	d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in ? GetIconName(in) : (STRPTR) ""));

	if (NULL == mtr->MenuCombo.MenuCommand.mcom_name)
		return;

	if (in)
		NumberOfWbArgs = 2;
	else
		NumberOfWbArgs = 1 + SCA_CountWBArgs(NULL);

	d1(KPrintF("%s/%s/%ld: NumberOfWbArgs=%ld\n", __FILE__, __FUNC__, __LINE__, NumberOfWbArgs));

	args = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
	if (NULL == args)
		return;

	lp = PathPart(mtr->MenuCombo.MenuCommand.mcom_name);
	ch = *lp;
	*lp = '\0';

	args[0].wa_Lock = Lock(mtr->MenuCombo.MenuCommand.mcom_name, ACCESS_READ);
	*lp = ch;
	args[0].wa_Name = FilePart(mtr->MenuCombo.MenuCommand.mcom_name);

	if ((NumberOfWbArgs > 1) && (mtr->MenuCombo.MenuCommand.mcom_flags & MCOMFLGF_Args))
		{
		if (in)
			{
			ArgCount += DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_MakeWBArg, in, &args[1]);
			}
		else
			{
			ArgCount += SCA_MakeWBArgs(&args[1], NULL,
				NumberOfWbArgs - 1);
			}
		}

	if (mtr->MenuCombo.MenuCommand.mcom_flags & MCOMFLGF_Priority)
		Priority = mtr->MenuCombo.MenuCommand.mcom_pri;

	debugLock_d1(args[0].wa_Lock);
	d1(KPrintF("%s/%s/%ld: arg[0].wa_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, args[0].wa_Name));
	d1(KPrintF("%s/%s/%ld: ArgCount=%ld\n", __FILE__, __FUNC__, __LINE__, ArgCount));
	d1(KPrintF("%s/%s/%ld: Priority=%ld\n", __FILE__, __FUNC__, __LINE__, Priority));

	// SCA_WBStart()
	Success = SCA_WBStartTags(args, ArgCount,
		SCA_Stacksize, mtr->MenuCombo.MenuCommand.mcom_stack,
		SCA_Priority, Priority,
		TAG_END);

	if (NumberOfWbArgs > 1)
		{
		SCA_FreeWBArgs(&args[1], ArgCount - 1,
			Success ? (SCAF_FreeNames) : (SCAF_FreeNames | SCAF_FreeLocks));
		}

	if (!Success)
		UnLock(args[0].wa_Lock);

	ScalosFree(args);
}


static void RunMenuCmd_CLI(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in)
{
	STRPTR Buffer;
	BPTR oldDir = NOT_A_LOCK;
	BPTR dirLock = (BPTR)NULL;
	BPTR fdConsole;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  mtr=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, mtr, in));

	do	{
		CONST_STRPTR pName = mtr->MenuCombo.MenuCommand.mcom_name;
		size_t BuffLen = CLIpathSize - 1;
		STRPTR BuffPtr;
		LONG Priority = 0;

		Buffer = ScalosAlloc(CLIpathSize);
		if (NULL == Buffer)
			return;

		BuffPtr = Buffer;
		d1(kprintf("%s/%s/%ld: Buffer=%08lx  Len=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, BuffLen));

		if ((BPTR)NULL == iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock || CurrentPrefs.pref_MenuCurrentDir)
			{
			// .newcurdir:
			char ch;
			STRPTR pp;

			if ('\0' == *pName)
				break;

			d1(kprintf("%s/%s/%ld: pName=<%s>\n", __FILE__, __FUNC__, __LINE__, pName));

			CliCopy(BuffPtr, &pName, &BuffLen, FALSE);
			d1(kprintf("%s/%s/%ld: Buffer=<%s>  Len=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, BuffLen));

			pp = PathPart(Buffer);
			ch = *pp;
			if (pp != Buffer)
				{
				*pp = '\0';

				d1(kprintf("%s/%s/%ld: Buffer=<%s>  Len=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, BuffLen));
				dirLock = Lock(Buffer, ACCESS_READ);
				if ((BPTR)NULL == dirLock)
					break;

				*pp = ch;
				oldDir = CurrentDir(dirLock);
				}

			pp = FilePart(Buffer);

			if (BuffLen < 2)
				break;

			BuffLen = CLIpathSize - 1;
			BuffPtr = Buffer;

			if (strchr(pp, ' '))
				{
				*BuffPtr++ = '"';
				BuffLen--;

				while (*pp && BuffLen > 1)
					{
					*BuffPtr++ = *pp++;
					BuffLen--;
					}

				if (BuffLen < 2)
					break;

				*BuffPtr++ = '"';
				BuffLen--;
				}
			else
				{
				while (*pp && BuffLen > 1)
					{
					*BuffPtr++ = *pp++;
					BuffLen--;
					}
				}
			}
		else
			{
			BuffPtr = CliCopy(BuffPtr, &pName, &BuffLen, TRUE);
			d1(kprintf("%s/%s/%ld: Buffer=<%s>  Len=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, BuffLen));
			}

		d1(kprintf("%s/%s/%ld: BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, BuffLen));

		while (isspace(*pName))
			pName++;

		// copy optional argument(s)
		if (*pName)
			{
			if (BuffLen < 2)
				break;

			*BuffPtr++ = ' ';
			BuffLen--;

			d1(kprintf("%s/%s/%ld: BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, BuffLen));

			while (*pName && BuffLen > 1)
				{
				if (mtr->MenuCombo.MenuCommand.mcom_flags & MCOMFLGF_Args &&
					'%' == *pName)
					{
					static struct WBArg arg;

					switch (pName[1])
						{
					case 'p':	// insert all parameters
					case 'P':
						BuffPtr = InsertCliArgs(iwt, BuffPtr, &BuffLen, in);
						pName += 2;	// skip "%p"
						break;
					case 'w':
					case 'W':	// insert directory path of current window
						arg.wa_Lock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;
						arg.wa_Name = (STRPTR) "";
						BuffPtr = InsertName(BuffPtr, &BuffLen, &arg);
						pName += 2;	// skip "%w"
						break;
					case 'd':
					case 'D':	// insert device name for every argument
						BuffPtr = InsertCliArgDeviceName(iwt, BuffPtr, &BuffLen, in);
						pName += 2;	// skip "%d"
						break;
					case '%':	// insert "%"
						*BuffPtr++ = '%';
						pName += 2;	// skip "%%"
						BuffLen--;
						break;
					default:
						*BuffPtr++ = *pName++;
						BuffLen--;
						break;
						}
					}
				else
					{
					*BuffPtr++ = *pName++;
					BuffLen--;
					}
				}
			}
		*BuffPtr = '\0';

		d1(kprintf("%s/%s/%ld: Buffer=<%s>  Len=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, BuffLen));

		fdConsole = Open((STRPTR) CurrentPrefs.pref_ConsoleName, MODE_NEWFILE);

		d1(kprintf("%s/%s/%ld: console=%08lx\n", __FILE__, __FUNC__, __LINE__, fdConsole));

		if (mtr->MenuCombo.MenuCommand.mcom_flags & MCOMFLGF_Priority)
			Priority = mtr->MenuCombo.MenuCommand.mcom_pri;

		d1(kprintf("%s/%s/%ld: Priority=%ld\n", __FILE__, __FUNC__, __LINE__, Priority));

		// SystemTagList()
		SystemTags(Buffer,
			NP_StackSize, mtr->MenuCombo.MenuCommand.mcom_stack,
			NP_Path, DupWBPathList(),
			NP_WindowPtr, NULL,
			NP_Priority, Priority,
			SYS_Asynch, TRUE,
			SYS_Output, NULL,
			SYS_Input, fdConsole,
			TAG_END);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (dirLock)
		UnLock(dirLock);
	if (Buffer)
		ScalosFree(Buffer);
}


static void RunMenuCmd_ARexx(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in)
{
	struct AsyncArexxMenuCmdArg Args;

	Args.asm_iwt = iwt;
	Args.asm_MenuTree = mtr;
	Args.asm_IconNode = in;

	d1(kprintf("%s/%s/%ld: mcom_type=%ld  name=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, mtr->MenuCombo.MenuCommand.mcom_type,\
		mtr->MenuCombo.MenuCommand.mcom_name));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_RunProcess, AsyncARexxMenuCmdStart,
		&Args, sizeof(Args), SCCV_RunProcess_NoReply);
}


static SAVEDS(void) AsyncARexxMenuCmdStart(APTR xarg, struct SM_RunProcess *msg)
{
	struct AsyncArexxMenuCmdArg *arg = (struct AsyncArexxMenuCmdArg *) xarg;
	struct MsgPort *RexxPort;
	struct RexxMsg *RxMsg = NULL;
	BOOL RxMsgFilled = FALSE;
	ULONG ArgCount = 1;
	STRPTR CmdString = NULL;
	size_t BuffLen = CLIpathSize - 1;
	struct MsgPort *ReplyPort;

	(void) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  mtr=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, arg->asm_iwt, arg->asm_MenuTree, arg->asm_IconNode));

	do	{
		CONST_STRPTR pName = arg->asm_MenuTree->MenuCombo.MenuCommand.mcom_name;
		STRPTR BuffPtr;
		char chStop;

		struct Process *myProc = (struct Process *) FindTask(NULL);

		myProc->pr_WindowPtr = NULL;

		ReplyPort = CreateMsgPort();
		if (NULL == ReplyPort)
			break;

		RexxPort = FindPort(RXSDIR);
		d1(kprintf("%s/%s/%ld: RexxPort=%08lx\n", __FILE__, __FUNC__, __LINE__, RexxPort));
		if (NULL == RexxPort)
			break;

		RxMsg = CreateRexxMsg(ReplyPort, "Rexx", "WORKBENCH");
		d1(kprintf("%s/%s/%ld: RxMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, RxMsg));
		if (NULL == RxMsg)
			break;

		CmdString = BuffPtr = ScalosAlloc(CLIpathSize);
		if (NULL == CmdString)
			return;

		d1(kprintf("%s/%s/%ld: mcom_name=<%s>  mcom_flags=%04lx\n", \
			__FILE__, __FUNC__, __LINE__, arg->asm_MenuTree->MenuCombo.MenuCommand.mcom_name, \
			arg->asm_MenuTree->MenuCombo.MenuCommand.mcom_flags));

		switch (*pName)
			{
		case '"':
		case '\'':
			chStop = *pName;
			pName++;		// skip '"'
			break;
		default:
			chStop = ' ';
			break;
			}

		RxMsg->rm_Args[0] = (IPTR)BuffPtr;

		// copy command name (up to first chStop)
		while (*pName && chStop != *pName && BuffLen > 1)
			{
			*BuffPtr++ = *pName++;
			BuffLen--;
			}
		*BuffPtr++ = '\0';
		BuffLen--;

		d1(kprintf("%s/%s/%ld: rm_Args[0] = <%s>\n", __FILE__, __FUNC__, __LINE__, RxMsg->rm_Args[0]));

		if (chStop == *pName)
			pName++;	// skip chStop

		pName = stpblk(pName);	// skip spaces
		if (*pName)
			{
			// rm_Arg[1] gets remainder of command line
			RxMsg->rm_Args[1] = (IPTR) pName;
			ArgCount++;
			d1(kprintf("%s/%s/%ld: rm_Args[1] = <%s>\n", __FILE__, __FUNC__, __LINE__, RxMsg->rm_Args[1]));
			}

		// optionally append selected icons as additional arguments
		if (arg->asm_MenuTree->MenuCombo.MenuCommand.mcom_flags & MCOMFLGF_Args)
			InsertARexxArgs(arg->asm_iwt, BuffPtr, &BuffLen, &ArgCount, arg->asm_IconNode, RxMsg);

		RxMsg->rm_Action = RXFUNC | ((ArgCount - 1) & 0xff);

		d1(kprintf("%s/%s/%ld: ArgCount=%ld\n", __FILE__, __FUNC__, __LINE__, ArgCount));

		RxMsg->rm_Stdin = Open("NIL:", MODE_OLDFILE);
		d1(kprintf("%s/%s/%ld: stdin=%08lx\n", __FILE__, __FUNC__, __LINE__, RxMsg->rm_Stdin));
		if ((BPTR)NULL == RxMsg->rm_Stdin)
			break;

		RxMsg->rm_Stdout = Open((STRPTR) CurrentPrefs.pref_ConsoleName, MODE_NEWFILE);
		d1(kprintf("%s/%s/%ld: stdout=%08lx\n", __FILE__, __FUNC__, __LINE__, RxMsg->rm_Stdout));
		if ((BPTR)NULL == RxMsg->rm_Stdout)
			break;

		if (!FillRexxMsg(RxMsg, ArgCount, 0))
			break;

		d1(kprintf("%s/%s/%ld: FillRexxMsg OK\n", __FILE__, __FUNC__, __LINE__));

		RxMsgFilled = TRUE;

		PutMsg(RexxPort, (struct Message *) RxMsg);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		WaitPort(ReplyPort);
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		(void) GetMsg(ReplyPort);
		} while (0);

	if (RxMsg)
		{
		if (RxMsg->rm_Stdin)
			Close(RxMsg->rm_Stdin);
		if (RxMsg->rm_Stdout && RETURN_OK == RxMsg->rm_Result1)
			{
			// only close Stdout if no error !
			Close(RxMsg->rm_Stdout);
			}

		if (RxMsgFilled)
			ClearRexxMsg(RxMsg, ArgCount);

		DeleteRexxMsg(RxMsg);
		}

	if (CmdString)
		ScalosFree(CmdString);
	if (ReplyPort)
		DeleteMsgPort(ReplyPort);
}


static void RunMenuCmd_Plugin(struct internalScaWindowTask *iwt,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in)
{
	struct Library *ScalosMenuPluginBase;
#ifdef __amigaos4__
	struct ScalosMenuPluginIFace *IScalosMenuPlugin;
#endif

	d1(kprintf("%s/%s/%ld: iwt=%08lx  mtr=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, mtr, in));

	ScalosMenuPluginBase = OpenLibrary(mtr->MenuCombo.MenuCommand.mcom_name, 0);

	d1(kprintf("%s/%s/%ld: Library=<%s> Base=%08lx\n", __FILE__, __FUNC__, __LINE__, mtr->MenuCombo.MenuCommand.mcom_name, ScalosMenuPluginBase));

#ifdef __amigaos4__
	if (ScalosMenuPluginBase)
		{
		IScalosMenuPlugin = (struct ScalosMenuPluginIFace *)GetInterface(ScalosMenuPluginBase, "main", 1, NULL);
		if (NULL == IScalosMenuPlugin)
			{
			CloseLibrary(ScalosMenuPluginBase);
			ScalosMenuPluginBase = NULL;
			}
		}
#endif

	if (ScalosMenuPluginBase)
		{
		SCAMenuFunction((struct ScaWindowTask *) iwt, in);

#ifdef __amigaos4__
		DropInterface((struct Interface *)IScalosMenuPlugin);
#endif
		CloseLibrary(ScalosMenuPluginBase);
		}
}


static STRPTR InsertCliArgDeviceName(struct internalScaWindowTask *iwt, STRPTR Buffer,
	size_t *BuffLen, struct ScaIconNode *in)
{
	struct WBArg *args;
	ULONG NumberOfWbArgs;

	NumberOfWbArgs = 1 + SCA_CountWBArgs(in);

	args = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
	if (args)
		{
		d1(kprintf("%s/%s/%ld: Buffer=%08lx  BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, *BuffLen));

		if (in)
			{
			//.menuitem:
			d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg, in, args);
			Buffer = InsertDeviceName(Buffer, BuffLen, args[0].wa_Lock);

			SCA_FreeWBArgs(args, 1, SCAF_FreeLocks | SCAF_FreeNames);
			}
		else
			{
			ULONG n;
			ULONG iArgCount;

			iArgCount = SCA_MakeWBArgs(args, NULL, NumberOfWbArgs);
			if (iArgCount < 1)
				return Buffer;

			for (n=0; *BuffLen > 2 && n < iArgCount; n++)
				{
				Buffer = InsertDeviceName(Buffer, BuffLen, args[n].wa_Lock);
				d1(kprintf("%s/%s/%ld: Buffer=%08lx  BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, *BuffLen));
				}

			d1(kprintf("%s/%s/%ld: iArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, iArgCount));

			SCA_FreeWBArgs(args, iArgCount, SCAF_FreeLocks | SCAF_FreeNames);
			}
		ScalosFree(args);
		}

	return Buffer;
}

static STRPTR InsertCliArgs(struct internalScaWindowTask *iwt, STRPTR Buffer, 
	size_t *BuffLen, struct ScaIconNode *in)
{
	struct WBArg *args;
	ULONG NumberOfWbArgs;

	NumberOfWbArgs = 1 + SCA_CountWBArgs(in);

	args = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
	if (args)
		{
		d1(kprintf("%s/%s/%ld: Buffer=%08lx  BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, *BuffLen));

		if (in)
			{
			//.menuitem:
			d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg, in, args);
			Buffer = InsertName(Buffer, BuffLen, &args[0]);

			SCA_FreeWBArgs(args, 1, SCAF_FreeLocks | SCAF_FreeNames);
			}
		else
			{
			ULONG n;
			ULONG iArgCount;

			iArgCount = SCA_MakeWBArgs(args, NULL, NumberOfWbArgs);
			if (iArgCount < 1)
				return Buffer;

			for (n=0; *BuffLen > 2 && n < iArgCount; n++)
				{
				Buffer = InsertName(Buffer, BuffLen, &args[n]);
				d1(kprintf("%s/%s/%ld: Buffer=%08lx  BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, *BuffLen));
				}

			d1(kprintf("%s/%s/%ld: iArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, iArgCount));

			SCA_FreeWBArgs(args, iArgCount, SCAF_FreeLocks | SCAF_FreeNames);
			}
		ScalosFree(args);
		}

	return Buffer;
}


static STRPTR InsertARexxArgs(struct internalScaWindowTask *iwt, STRPTR Buffer, 
	size_t *BuffLen, ULONG *ArgCount, struct ScaIconNode *in,
	struct RexxMsg *RxMsg)
{
	ULONG ArgIndex = *ArgCount;
	struct WBArg *args;
	ULONG NumberOfWbArgs;

	if (*ArgCount > MAXRMARG)
		return Buffer;

	NumberOfWbArgs = 1 + SCA_CountWBArgs(in);

	args = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
	if (args)
		{
		d1(kprintf("%s/%s/%ld: Buffer=%08lx  BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, *BuffLen));

		if (in)
			{
			//.menuitem:
			d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg, in, args);

			RxMsg->rm_Args[ArgIndex] = (IPTR)Buffer;
			Buffer = InsertName(Buffer, BuffLen, &args[0]);

			RxMsg->rm_Args[ArgIndex] = (IPTR)stpblk((char *)RxMsg->rm_Args[ArgIndex]);	// skip blanks at argument start

			(*ArgCount)++;

			d1(kprintf("%s/%s/%ld: rm_Args[%ld] = <%s>\n", __FILE__, __FUNC__, __LINE__, ArgIndex, RxMsg->rm_Args[ArgIndex]));

			SCA_FreeWBArgs(args, 1, SCAF_FreeLocks | SCAF_FreeNames);
			}
		else
			{
			ULONG n;
			ULONG iArgCount;

			iArgCount = SCA_MakeWBArgs(args, NULL, NumberOfWbArgs);
			if (iArgCount < 1)
				return Buffer;

			for (n=0; *BuffLen > 2 && n < iArgCount && n <= MAXRMARG; n++, ArgIndex++, (*ArgCount)++)
				{
				RxMsg->rm_Args[ArgIndex] = (IPTR)Buffer;

				Buffer = InsertName(Buffer, BuffLen, &args[n]);

				RxMsg->rm_Args[ArgIndex] = (IPTR)stpblk((char *)RxMsg->rm_Args[ArgIndex]);	// skip blanks at argument start

				if (*BuffLen > 2)
					{
					// skip "\0" for next argument
					Buffer++;
					(*BuffLen)--;
					}

				d1(kprintf("%s/%s/%ld: rm_Args[%ld] = <%s>\n", __FILE__, __FUNC__, __LINE__, ArgIndex, RxMsg->rm_Args[ArgIndex]));
				}

			d1(kprintf("%s/%s/%ld: iArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, iArgCount));

			SCA_FreeWBArgs(args, iArgCount, SCAF_FreeLocks | SCAF_FreeNames);
			}
		ScalosFree(args);
		}

	d1(kprintf("%s/%s/%ld: ArgCount=%lu\n", __FILE__, __FUNC__, __LINE__, *ArgCount));

	return Buffer;
}


static STRPTR InsertName(STRPTR Buffer, size_t *BuffLen, struct WBArg *arg)
{
	BPTR oldDir;
	BPTR fLock;

	d1(kprintf("%s/%s/%ld: Buffer=%08lx  BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, *BuffLen));

	do	{
		size_t Len;

		oldDir = CurrentDir(arg->wa_Lock);

		fLock = Lock(arg->wa_Name, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			break;

		if (*BuffLen < 3)
			break;

		*Buffer++ = ' ';
		(*BuffLen)--;

		*Buffer++ = '"';
		(*BuffLen)--;

		if (!NameFromLock(fLock, Buffer, *BuffLen))
			break;

		Len = strlen(Buffer);
		Buffer += Len;
		*BuffLen -= Len;

		if (*BuffLen < 3)
			break;

		*Buffer++ = '"';
		(*BuffLen)--;

		*Buffer = '\0';
		} while (0);

	d1(kprintf("%s/%s/%ld: Buffer=%08lx  BuffLen=%lu\n", __FILE__, __FUNC__, __LINE__, Buffer, *BuffLen));

	CurrentDir(oldDir);
	if (fLock)
		UnLock(fLock);

	return Buffer;
}


static STRPTR InsertDeviceName(STRPTR BuffPtr, size_t *BuffLen, BPTR dirLock)
{
	struct InfoData *di;

	do	{
		const struct DosList *dlist;
		struct DosList *dl;

		di = ScalosAllocInfoData();
		if (NULL == di)
			break;

		if (!Info(dirLock, di))
			break;

		dlist = BADDR(di->id_VolumeNode);
		if (NULL == dlist)
			break;

		dl = LockDosList(LDF_DEVICES | LDF_READ);
		if (NULL == dl)
			break;

		while (dl)
			{
			if (dl->dol_Task == dlist->dol_Task)
				{
				CONST_STRPTR Src = BADDR(dl->dol_Name);
				size_t Len = *Src++;

				while (Len && *BuffLen > 1)
					{
					*BuffPtr++ = *Src++;
					(*BuffLen)--;
					Len--;
					}
				break;
				}
			dl = NextDosEntry(dl, LDF_DEVICES);
			}
		UnLockDosList(LDF_DEVICES | LDF_READ);
		} while (0);

	ScalosFreeInfoData(&di);

	return BuffPtr;
}


static STRPTR CliCopy(STRPTR BuffPtr, CONST_STRPTR *pName, size_t *BuffLen, BOOL Quotes)
{
	char cStop;

	if (Quotes)
		{
		if (*BuffLen < 2)
			return BuffPtr;

		*BuffPtr++ = '"';
		(*BuffLen)--;
		}

	switch (**pName)
		{
	case '"':
	case '\'':
		cStop = **pName;
		(*pName)++;
		break;
	default:
		cStop = ' ';
		break;
		}

	while (**pName && cStop != **pName && *BuffLen > 1)
		{
		*BuffPtr++ = **pName;
		(*pName)++;
		(*BuffLen)--;
		}

	if (**pName && ' ' != cStop)
		{
		// skip trailing quote
		(*pName)++;
		}

	if (Quotes)
		{
		if (*BuffLen < 2)
			return BuffPtr;

		*BuffPtr++ = '"';
		(*BuffLen)--;
		}

	*BuffPtr = '\0';

	return BuffPtr;
}


LONG ScalosPutIcon(struct ScaIconNode *in, BPTR destDirLock, BOOL NeedUpdateIcon)
{
	CONST_STRPTR IconName;
	LONG Result;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	if (!CurrentPrefs.pref_SaveDefIcons && in->in_Flags & INF_DefaultIcon)
		return ERROR_OBJECT_WRONG_TYPE;	// don't save default icons

	if (NULL == in->in_Name)
		return ERROR_INVALID_COMPONENT_NAME;
	if (NULL == in->in_Icon)
		return ERROR_OBJECT_NOT_FOUND;

	if (in->in_DeviceIcon)
		IconName = "disk";
	else
		IconName = GetIconName(in);

	Result = SaveIconObject(in->in_Icon, destDirLock, IconName, NeedUpdateIcon, NULL);

	if (!NeedUpdateIcon && (RETURN_OK == Result))
		DateStamp(&in->in_DateStamp);
	
	return Result;
}


Object *LoadIconObject(BPTR DirLock, CONST_STRPTR IconName, struct TagItem *TagList)
{
	Object *IconObj;
	BPTR oldDir;

	d1(KPrintF("%s/%s/%ld:  START IconObj=%08lx  NeedUpdateIcon=%ld\n", __FILE__, __FUNC__, __LINE__, IconObj, NeedUpdateIcon));
	debugLock_d1(DirLock);

	oldDir = CurrentDir(DirLock);

	IconObj = NewIconObjectTags(IconName,
		TAG_MORE, (ULONG) TagList,
		TAG_END);

	CurrentDir(oldDir);

	d1(KPrintF("%s/%s/%ld:  END IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	return IconObj;
}


LONG SaveIconObject(Object *IconObj, BPTR DirLock,
	CONST_STRPTR IconName, BOOL NeedUpdateIcon, 
	struct TagItem *TagList)
{
	BPTR oldDir;
	LONG Result;

	d1(KPrintF("%s/%s/%ld:  START IconObj=%08lx  NeedUpdateIcon=%ld\n", __FILE__, __FUNC__, __LINE__, IconObj, NeedUpdateIcon));
	debugLock_d1(DirLock);

	oldDir = CurrentDir(DirLock);

	// PutIconObject()
	Result = PutIconObjectTags(IconObj, IconName,
		ICONA_NoNewImage, TRUE,
		ICONA_NotifyWorkbench, NeedUpdateIcon,
		TAG_MORE, (ULONG) TagList,
		TAG_END);

	d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

	if (NeedUpdateIcon && (RETURN_OK == Result))
		{
		struct ScaUpdateIcon_IW upd;

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		upd.ui_iw_Lock = DirLock;
		upd.ui_iw_Name = (STRPTR) IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		SCA_UpdateIcon(SIV_IconWin, &upd, sizeof(upd));
		}

	CurrentDir(oldDir);

	d1(KPrintF("%s/%s/%ld:  END Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


void LockedCloseWindow(struct Window *win)
{
	ScalosObtainSemaphore(&LayersSema);
	d1(kprintf("%s/%s/%ld: ScalosObtainSemaphore(&LayersSema)\n", __FILE__, __FUNC__, __LINE__));

	if (win)
		{
		Scalos_DoneRastPort(win->RPort);
		d1(kprintf("%s/%s/%ld: Scalos_DoneRastPort(%08lx) win=%08lx Title=<%s>\n", __FILE__, __FUNC__, __LINE__, win->RPort, win, win->Title));
		}

	CloseWindow(win);
	d1(kprintf("%s/%s/%ld: CloseWindow(%08lx) - title=<%s>\n", __FILE__, __FUNC__, __LINE__, win ,win->Title));

	ScalosReleaseSemaphore(&LayersSema);
	d1(kprintf("%s/%s/%ld: ScalosReleaseSemaphore(&LayersSema)\n", __FILE__, __FUNC__, __LINE__));

}


struct Window *LockedOpenWindowTagList(struct NewWindow *nw, struct TagItem *TagList)
{
	struct Window *win;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	ScalosObtainSemaphore(&LayersSema);
	d1(kprintf("%s/%s/%ld: ScalosObtainSemaphore(&LayersSema) \n", __FILE__, __FUNC__, __LINE__));
	win = OpenWindowTagList(nw, TagList);
	d1(kprintf("%s/%s/%ld: win = OpenWindowTagList(nw, TagList) title=<%s> \n", __FILE__, __FUNC__, __LINE__, win->Title));
	ScalosReleaseSemaphore(&LayersSema);
	d1(kprintf("%s/%s/%ld: ScalosReleaseSemaphore(&LayersSema) \n", __FILE__, __FUNC__, __LINE__));
	return win;
}


VOID LockedSetWindowTitles(struct Window *win,
	CONST_STRPTR WindowTitle, CONST_STRPTR ScreenTitle)
{
	struct Window *iwin;

	ScalosObtainSemaphore(&LayersSema);
	d1(kprintf("%s/%s/%ld: ScalosObtainSemaphore(&LayersSema) \n", __FILE__, __FUNC__, __LINE__));

	// Make sure that <win> refers to a valid intuition window on the Scalos screen!
	for (iwin = iInfos.xii_iinfos.ii_Screen->FirstWindow; iwin; iwin = iwin->NextWindow)
		{
		if (iwin == win)
			{
			SetWindowTitles(win, (STRPTR) WindowTitle, (STRPTR) ScreenTitle);
			d1(kprintf("%s/%s/%ld: SetWindowTitles(win=%08lx, %s, %s) \n", __FILE__, __FUNC__, __LINE__, win, (STRPTR) WindowTitle, (STRPTR) ScreenTitle));
			break;
			}
		}

	ScalosReleaseSemaphore(&LayersSema);
	d1(kprintf("%s/%s/%ld: ScalosReleaseSemaphore(&LayersSema) \n", __FILE__, __FUNC__, __LINE__));

}


struct Window *LockedOpenWindowTags(struct NewWindow *nw, ULONG FirstTag, ...)
{
	struct Window *win = NULL;
	struct TagItem *TagList;
	va_list args;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);

	if (TagList)
		{
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		win = LockedOpenWindowTagList(nw, TagList);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		FreeTagItems(TagList);
		}

	va_end(args);

	d1(KPrintF("%s/%s/%ld: END  win=%08lx\n", __FILE__, __FUNC__, __LINE__, win));

	return win;
}


void SafeSetMenuStrip(struct Window *win)
{
	d1(kprintf("%s/%s/%ld: START win=%08lx  MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, win, MainMenu));

	if (win)
		{
		d1(kprintf("%s/%s/%ld: win=%08lx Title=<%s>  MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, win, win->Title, MainMenu));
		ScalosObtainSemaphore(&MenuSema);
		d1(kprintf("%s/%s/%ld: after ScalosObtainSemaphore(&MenuSema) \n", __FILE__, __FUNC__, __LINE__));

		if (win->MenuStrip)
			{
			ClearMenuStrip(win);
			d1(kprintf("%s/%s/%ld: after ClearMenuStrip(&MenuSema) \n", __FILE__, __FUNC__, __LINE__));
			}

		SetMenuStrip(win, MainMenu);
		d1(kprintf("%s/%s/%ld: SetMenuStrip(win, MainMenu)\n", __FILE__, __FUNC__, __LINE__));
		ScalosReleaseSemaphore(&MenuSema);
		d1(kprintf("%s/%s/%ld: after ScalosReleaseSemaphore(&MenuSema) \n", __FILE__, __FUNC__, __LINE__));
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void SafeClearMenuStrip(struct Window *win)
{
	d1(kprintf("%s/%s/%ld: START win=%08lx  MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, win, MainMenu));

	if (win)
		{
		d1(kprintf("%s/%s/%ld: win=%08lx Title=<%s>  MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, win, win->Title, MainMenu));
		ScalosObtainSemaphore(&MenuSema);
		d1(kprintf("%s/%s/%ld: after ScalosObtainSemaphore(&MenuSema) \n", __FILE__, __FUNC__, __LINE__));
		ClearMenuStrip(win);
		d1(kprintf("%s/%s/%ld: SetMenuStrip(win, MainMenu)\n", __FILE__, __FUNC__, __LINE__));
		ScalosReleaseSemaphore(&MenuSema);
		d1(kprintf("%s/%s/%ld: after ScalosReleaseSemaphore(&MenuSema) \n", __FILE__, __FUNC__, __LINE__));
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


// returns correct name for all icon types.
// guaranteed to NEVER return NULL.
CONST_STRPTR GetIconName(const struct ScaIconNode *in)
{
	CONST_STRPTR IconName = NULL;

	if (in)
		{
		ULONG IconType;

		GetAttr(IDTA_Type, in->in_Icon, &IconType);

		if (WBAPPICON == IconType)
			{
			GetAttr(IDTA_Text, in->in_Icon, (APTR) &IconName);
			}
		else
			{
			if (in->in_DeviceIcon)
				{
				if (in->in_DeviceIcon->di_Volume)
					IconName = in->in_DeviceIcon->di_Volume;
				else
					IconName = in->in_DeviceIcon->di_Device;
				}
			else
				{
				IconName = in->in_Name;
				}
			}
		}

	if (NULL == IconName)
		IconName = "";

	return IconName;
}


ULONG DisposeScalosWindow(struct internalScaWindowTask *iwt, struct ScaWindowStruct **WindowList)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	BOOL wasMainWindow = FALSE;
	BOOL Found = FALSE;
	BPTR xLock = (BPTR)NULL;

	SCA_LockWindowList(SCA_LockWindowList_Exclusiv);

	if (iInfos.xii_iinfos.ii_MainWindowStruct == ws)
		{
		iInfos.xii_iinfos.ii_MainWindowStruct = NULL;
		wasMainWindow = TRUE;
		}
	if (iInfos.xii_iinfos.ii_AppWindowStruct== ws)
		{
		iInfos.xii_iinfos.ii_AppWindowStruct= NULL;
		}
	
	if (WSV_Type_DeviceWindow != ws->ws_WindowType)
		{
		if (ws->ws_Flags & WSV_FlagF_RootWindow)
			{
			if (iInfos.xii_iinfos.ii_MainWindowStruct)
				{
				struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
				struct ScaIconNode *in;

				ScalosLockIconListShared(iwtx);

				for (in=iwtx->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					if (in->in_DeviceIcon)
						{
						xLock = DiskInfoLock(in);

						if (xLock)
							{
							if (LOCK_SAME == ScaSameLock(ws->ws_Lock, xLock))
								{
								DisposeScalosWindow_IconFound(iwt, iwtx, in);
								break;
								}
							}
						}
					}

				ScalosUnLockIconList(iwtx);
				}
			}
		else
			{
			xLock = ParentDir(ws->ws_Lock);

			if (xLock)
				{
				struct ScaWindowStruct *wsx;

				for (wsx=winlist.wl_WindowStruct; !Found && wsx; wsx = (struct ScaWindowStruct *) wsx->ws_Node.mln_Succ)
					{
					if (LOCK_SAME == ScaSameLock(xLock, wsx->ws_Lock))
						{
						struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) wsx->ws_WindowTask;
						struct ScaIconNode *in;

						ScalosLockIconListShared(iwtx);

						for (in=iwtx->iwt_WindowTask.wt_IconList; !Found && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
							{
							if (0 == strcmp(ws->ws_Name, GetIconName(in)))
								{
								DisposeScalosWindow_IconFound(iwt, iwtx, in);
								Found = TRUE;
								break;
								}
							}

						ScalosUnLockIconList(iwtx);
						}
					}

				if (iInfos.xii_iinfos.ii_MainWindowStruct && !Found)
					{
					struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
					struct ScaIconNode *in;

					ScalosLockIconListShared(iwtx);

					for (in=iwtx->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
						{
						if (in->in_Lock && LOCK_SAME == ScaSameLock(in->in_Lock, xLock)
							&& 0 == strcmp(ws->ws_Name, GetIconName(in)))
							{
							DisposeScalosWindow_IconFound(iwt, iwtx, in);
							break;
							}
						}

					ScalosUnLockIconList(iwtx);
					}
				}
			}
		}

	if (xLock)
		{
		UnLock(xLock);
		d1(kprintf("%s/%s/%ld: UnLock(xLock) / xLock=%08lx\n", __FILE__, __FUNC__, __LINE__, xLock));
		}

	if (ws->ws_WindowTaskName)
		{
		ScalosFree(ws->ws_WindowTaskName);
		ws->ws_WindowTaskName = NULL;
		}
	if (ws->ws_Name)
		{
		FreeCopyString(ws->ws_Name);
		ws->ws_Name = NULL;
		}
	if (ws->ws_Title)
		{
		FreeCopyString(ws->ws_Title);
		ws->ws_Title = NULL;
		}

	d1(kprintf("%s/%s/%ld: ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));

	SCA_FreeNode((struct ScalosNodeList *) WindowList, &ws->ws_Node);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SCA_UnLockWindowList();

	return wasMainWindow;
}


static void DisposeScalosWindow_IconFound(struct internalScaWindowTask *iwt, 
	struct internalScaWindowTask *iwtIcon, struct ScaIconNode *in)
{
	ULONG ddFlags;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	ULONG IconViewMode = TranslateViewModesToIcon(ws->ws_Viewmodes);

	d1(kprintf("%s/%s/%ld: iwt=%08lx  iwtIcon=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwtIcon, in));
	d1(kprintf("%s/%s/%ld: ViewModes=%ld\n", __FILE__, __FUNC__, __LINE__, ws->ws_Viewmodes));

	ddFlags = ws->ws_ViewAll;
	if (SortOrder_Ascending == ws->ws_SortOrder)
		ddFlags |= DDFLAGS_SORTASC;
	else if (SortOrder_Descending == ws->ws_SortOrder)
		ddFlags |= DDFLAGS_SORTDESC;

	SetAttrs(in->in_Icon,
		IDTA_ViewModes, IconViewMode,
		IDTA_Flags, ddFlags,
		IDTA_WindowRect, (ULONG) &ws->ws_Left,
		IDTA_WinCurrentX, ws->ws_xoffset,
		IDTA_WinCurrentY, ws->ws_yoffset,
		TAG_END);

	ClassSelectIcon(iwtIcon->iwt_WindowTask.mt_WindowStruct, in, FALSE);

	if (iwtIcon->iwt_LastIcon == in->in_Icon)
		iwtIcon->iwt_LastIcon = NULL;
}


// adjust window title if parent drawer has been renamed
void AdjustRenamedWindowName(BPTR objLock)
{
	struct ScaWindowStruct *ws;

	debugLock_d1(objLock);

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: ws=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, ws, ws->ws_Name));
		debugLock_d1(ws->ws_Lock);

		if (LOCK_SAME == ScaSameLock(ws->ws_Lock, objLock))
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
			STRPTR objName = AllocPathBuffer();

			d1(kprintf("%s/%s/%ld: iwt=%08lx  objName=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, objName));

			if (objName && NameFromLock(objLock, objName, Max_PathLen-1))
				{
				FreeCopyString(ws->ws_Name);
				ws->ws_Name = AllocCopyString(FilePart(objName));

				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				iwt->iwt_WinTitle = (STRPTR) DoMethod(iwt->iwt_TitleObject, SCCM_Title_Generate);
				DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetTitle, iwt->iwt_WinTitle);

				FreePathBuffer(objName);
				}
			}
		}

	SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
}


void LockWindow(struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_WindowTask.wt_Window)
		{
		if (iwt->iwt_WinLockCount++ < 1)
			{
			ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, 
				IDCMP_REFRESHWINDOW | IDCMP_CHANGEWINDOW | IDCMP_CLOSEWINDOW);
			SetWindowPointer(iwt->iwt_WindowTask.wt_Window, 
				WA_PointerDelay, 1,
				WA_BusyPointer, TRUE,
				TAG_END);
			}
		}

	d1(kprintf("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}


void UnLockWindow(struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_WinLockCount > 0
		&& 0 == --iwt->iwt_WinLockCount)
		{
		if (iwt->iwt_WindowTask.wt_Window)
			{
			SetWindowPointer(iwt->iwt_WindowTask.wt_Window,
				WA_Pointer, NULL,
				TAG_END);
			ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);
			}
		}

	d1(kprintf("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}


struct ScalosMenuTree *CloneMenuTree(const struct ScalosMenuTree *mtr)
{
	struct ScalosMenuTree *newNodeList = NULL;

	while (mtr)
		{
		struct ScalosMenuTree *mtrNew;

		d1(KPrintF("%s/%s/%ld:  mtr=%08lx  Next=%08lx\n", __FILE__, __FUNC__, __LINE__, mtr, mtr->mtre_Next));

		mtrNew = ScalosAlloc(sizeof(struct ScalosMenuTree));
		if (NULL == mtrNew)
			{
			DisposeMenuTree(newNodeList);
			return NULL;
			}

		mtrNew->mtre_Next = mtrNew->mtre_tree = NULL;

		AppendToMenuTree(&newNodeList, mtrNew);
		d1(KPrintF("%s/%s/%ld:  mtrNew=%08lx\n", __FILE__, __FUNC__, __LINE__, mtrNew));

		*mtrNew = *mtr;
		mtrNew->mtre_Next = NULL;
		if (mtr->mtre_tree)
			mtrNew->mtre_tree = CloneMenuTree(mtr->mtre_tree);

		if (SCAMENUTYPE_Command == mtr->mtre_type)
			mtrNew->MenuCombo.MenuCommand.mcom_name = AllocCopyString(mtr->MenuCombo.MenuCommand.mcom_name);
		else
			mtrNew->MenuCombo.MenuTree.mtre_name = AllocCopyString(mtr->MenuCombo.MenuTree.mtre_name);

		mtr = mtr->mtre_Next;
		}

	return newNodeList;
}


void DisposeMenuTree(struct ScalosMenuTree *mtr)
{
	while (mtr)
		{
		struct ScalosMenuTree *mtrNext = mtr->mtre_Next;

		DisposeMenuTree(mtr->mtre_tree);

		if (SCAMENUTYPE_Command == mtr->mtre_type)
			{
			if (mtr->MenuCombo.MenuCommand.mcom_name)
				{
				FreeCopyString(mtr->MenuCombo.MenuCommand.mcom_name);
				mtr->MenuCombo.MenuCommand.mcom_name = NULL;
				}
			}
		else
			{
			if (mtr->MenuCombo.MenuTree.mtre_name)
				{
				FreeCopyString(mtr->MenuCombo.MenuTree.mtre_name);
				mtr->MenuCombo.MenuTree.mtre_name = NULL;
				}
			}

		ScalosFree(mtr);

		mtr = mtrNext;
		}
}


void AppendToMenuTree(struct ScalosMenuTree **mtreList, struct ScalosMenuTree *mTree)
{
	while (*mtreList)
		{
		mtreList = &(*mtreList)->mtre_Next;
		}

	mTree->mtre_Next = NULL;
	*mtreList = mTree;
}


void SetIconWindowReadOnly(struct internalScaWindowTask *iwt, BOOL IsReadOnly)
{
	if (IsReadOnly)
		{
		if (NULL == iwt->iwt_PadLockGadget)
			{
			iwt->iwt_PadLockGadget = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarImage.sca",
				DTA_Name, (ULONG) "THEME:Window/StatusBar/PadLock",
				GBIDTA_WindowTask,(ULONG)  iwt,
				GA_ID, SBAR_GadgetID_PadLock,
				TAG_END);
			}

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
			SCCM_IconWin_AddToStatusBar, iwt->iwt_PadLockGadget,
			TAG_END);
		}
	else
		{
		if (iwt->iwt_PadLockGadget)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
				SCCM_IconWin_RemFromStatusBar, iwt->iwt_PadLockGadget,
				TAG_END);
			iwt->iwt_PadLockGadget = NULL;
			}
		}
}


void RedrawResizedWindow(struct internalScaWindowTask *iwt,
	WORD OldInnerWidth, WORD OldInnerHeight)
{
	struct Region *RedrawRegion;

	d1(kprintf("%s/%s/%ld: START iwt=%08lx  <%s>  wt_Window=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iwt->iwt_WindowTask.wt_Window));

	do	{
		struct Rectangle Rect;

		RedrawRegion = NewRegion();
		if (NULL == RedrawRegion)
			break;

		if (OldInnerWidth < iwt->iwt_InnerWidth)
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			Rect.MinX = iwt->iwt_InnerLeft + OldInnerWidth;
			Rect.MaxX = iwt->iwt_InnerLeft + iwt->iwt_InnerWidth - 1;
			Rect.MinY = iwt->iwt_InnerTop;
			Rect.MaxY = iwt->iwt_InnerTop + iwt->iwt_InnerHeight - 1;

			if (!OrRectRegion(RedrawRegion, &Rect))
				break;

			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
				Rect.MinX, Rect.MinY,
				Rect.MaxX, Rect.MaxY);
				}
		if (OldInnerHeight < iwt->iwt_InnerHeight)
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			Rect.MinX = iwt->iwt_InnerLeft;
			Rect.MaxX = iwt->iwt_InnerLeft + iwt->iwt_InnerWidth - 1;
			Rect.MinY = iwt->iwt_InnerTop + OldInnerHeight;
			Rect.MaxY = iwt->iwt_InnerTop + iwt->iwt_InnerHeight - 1;

			if (!OrRectRegion(RedrawRegion, &Rect))
				break;

			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
				Rect.MinX, Rect.MinY,
				Rect.MaxX, Rect.MaxY);
			}

		RefreshIconList(iwt, iwt->iwt_WindowTask.wt_IconList, RedrawRegion);
		} while (0);

	if (RedrawRegion)
		DisposeRegion(RedrawRegion);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void ScaLockLayers(struct Layer_Info *li)
{
	LayersLockCount++;
	LockLayers(li);
}


void ScaUnlockLayers(struct Layer_Info *li)
{
	UnlockLayers(li);
	LayersLockCount--;
}


void ScaLockScreenLayers(void)
{
	ScaLockLayers(&iInfos.xii_iinfos.ii_Screen->LayerInfo);
}


void ScaUnlockScreenLayers(void)
{
	ScaUnlockLayers(&iInfos.xii_iinfos.ii_Screen->LayerInfo);
}


ULONG ScaLockIBase(ULONG LockNumber)
{
	if (LayersLockCount)
		return 0;

	IBaseLockNest++;

	return LockIBase(LockNumber);
}


void ScaUnlockIBase(ULONG Lock)
{
	if (LayersLockCount)
		return;

	UnlockIBase(Lock);
	IBaseLockNest--;
}


void UpdateIconOverlays(struct internalScaWindowTask *iwt)
{
	ULONG n;

	// Dispose icon overlays
	for (n = 0; n < Sizeof(iwt->iwt_IconOverlays); n++)
		{
		DisposeDatatypesImage(&iwt->iwt_IconOverlays[n]);
		}

	// Attempt to read icon overlay images
	iwt->iwt_IconOverlays[ICONOVERLAYB_LeftOut] = CreateDatatypesImage("THEME:Icons/Overlay/LeftOut", 0);
	iwt->iwt_IconOverlays[ICONOVERLAYB_ReadOnly] = CreateDatatypesImage("THEME:Icons/Overlay/ReadOnly", 0);
	iwt->iwt_IconOverlays[ICONOVERLAYB_Thumbnail] = CreateDatatypesImage("THEME:Icons/Overlay/Thumbnail", 0);

	d1(KPrintF("%s/%s/%ld: ICONOVERLAYB_LeftOut=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconOverlays[ICONOVERLAYB_LeftOut]));
	d1(KPrintF("%s/%s/%ld: ICONOVERLAYB_ReadOnly=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconOverlays[ICONOVERLAYB_ReadOnly]));
	d1(KPrintF("%s/%s/%ld: ICONOVERLAYB_Thumbnail=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconOverlays[ICONOVERLAYB_Thumbnail]));

}


void AddIconOverlay(Object *IconObj, ULONG NewOverlay)
{
	ULONG IconOverlayType = ICONOVERLAY_None;

	GetAttr(IDTA_OverlayType, IconObj, &IconOverlayType);
	IconOverlayType |= NewOverlay;
	SetAttrs(IconObj,
		IDTA_OverlayType, IconOverlayType,
		TAG_END);
}


void RemoveIconOverlay(Object *IconObj, ULONG OldOverlay)
{
	ULONG IconOverlayType = ICONOVERLAY_None;

	GetAttr(IDTA_OverlayType, IconObj, &IconOverlayType);
	IconOverlayType &= ~OldOverlay;
	SetAttrs(IconObj,
		IDTA_OverlayType, IconOverlayType,
		TAG_END);
}


BOOL ScalosAttemptLockIconListShared(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (ScalosAttemptSemaphoreShared(iwt->iwt_WindowTask.wt_IconSemaphore))
		{
		if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
			{
			iwt->iwt_IconListLockedShared++;
			}
		d1(KPrintF("%s/%s/%ld: END(TRUE) iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
		return TRUE;
		}

	d1(KPrintF("%s/%s/%ld: END(FALSE) iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	return FALSE;
}


BOOL ScalosAttemptLockIconListExclusive(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (ScalosAttemptSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore))
		{
		if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
			{
			iwt->iwt_IconListLockedExclusive++;
			}
		d1(kprintf("%s/%s/%ld: END(TRUE)\n", __FILE__, __FUNC__, __LINE__));
		return TRUE;
		}

	d1(kprintf("%s/%s/%ld: END(FALSE)\n", __FILE__, __FUNC__, __LINE__));
	return FALSE;
}

#ifndef	DEBUG_SEMAPHORES

void ScalosLockIconListShared(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	// Problem addressed: if the current process already holds an exclusive
	// lock on the semaphore, ObtainSemaphoreShared() is guaranteed to
	// create a deadlock.
	// Solution: is we already own this semaphore exclusively,
	// ScalosAttemptSemaphore() will succeed.
	if (ScalosAttemptSemaphoreShared(iwt->iwt_WindowTask.wt_IconSemaphore))
		{
		if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
			{
			iwt->iwt_IconListLockedShared++;
			}
		d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
		return;
		}
	if (ScalosAttemptSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore))
		{
		if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
			{
			iwt->iwt_IconListLockedExclusive++;
			}
		d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
		return;
		}

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_IconSemaphore);

	if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
		{
		iwt->iwt_IconListLockedShared++;
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}


void ScalosLockIconListExclusive(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL)
		&& iwt->iwt_IconListLockedShared)
		{
		// the window process already holds a shared lock on the icon list.
		// calling ScalosObtainSemaphore now guarantees a deadlock.
		ULONG Count = 0;

		d1(kprintf("%s/%s/%ld: iwt_IconListLockedShared=%lu  iwt_IconListLockedExclusive=%lu\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_IconListLockedShared, iwt->iwt_IconListLockedExclusive));

		Forbid();

		// release all shared locks, and establish exclusive lock(s)
		while (iwt->iwt_IconListLockedShared)
			{
			iwt->iwt_IconListLockedShared--;
			Count++;
			ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore);
			}
		while (Count--)
			{
			iwt->iwt_IconListLockedExclusive++;
			ScalosObtainSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore);
			}

		Permit();

		d1(kprintf("%s/%s/%ld: iwt_IconListLockedShared=%lu  iwt_IconListLockedExclusive=%lu\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_IconListLockedShared, iwt->iwt_IconListLockedExclusive));
		}

	ScalosObtainSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore);

	if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
		{
		iwt->iwt_IconListLockedExclusive++;
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}

#endif /* DEBUG_SEMAPHORES */

void ScalosUnLockIconList(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_WindowProcess == (struct Process *) FindTask(NULL))
		{
		if (iwt->iwt_IconListLockedExclusive)
			iwt->iwt_IconListLockedExclusive--;
		else if (iwt->iwt_IconListLockedShared)
			iwt->iwt_IconListLockedShared--;
		}

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore);

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}



struct WindowHistoryEntry *WindowAddHistoryEntry(struct internalScaWindowTask *iwt, BPTR wsLock)
{
	struct WindowHistoryEntry *whe = NULL;
	STRPTR FullPath;
	BOOL Success = FALSE;

	do	{
		FullPath = AllocPathBuffer();
		d1(KPrintF("%s/%s/%ld: FullPath=%08lx\n", __FILE__, __FUNC__, __LINE__, FullPath));
		if (NULL == FullPath)
			break;

		if (!NameFromLock(wsLock, FullPath, Max_PathLen - 1))
			break;

		whe = ScalosAlloc(sizeof(struct WindowHistoryEntry) + strlen(FullPath));
		d1(KPrintF("%s/%s/%ld: whe=%08lx\n", __FILE__, __FUNC__, __LINE__, whe));
		if (NULL == whe)
			break;

		whe->whe_IconList = NULL;
		whe->whe_Node.ln_Name = whe->whe_Path;
		strcpy(whe->whe_Node.ln_Name, FullPath);

		AddTail(&iwt->iwt_HistoryList, &whe->whe_Node);

		Success = TRUE;
		} while (0);

	if (FullPath)
		FreePathBuffer(FullPath);
	if (!Success)
		{
		WindowHistoryEntryDispose(iwt, whe);
		whe = NULL;
		}

	d1(KPrintF("%s/%s/%ld: Success=%ld  whe=%08lx\n", __FILE__, __FUNC__, __LINE__, Success, whe));

	return whe;
}


void WindowHistoryEntryDispose(struct internalScaWindowTask *iwt, struct WindowHistoryEntry *whe)
{
	if (whe)
		{
		FreeIconList(iwt, &whe->whe_IconList);
		ScalosFree(whe);
		}
}


struct WindowHistoryEntry *WindowFindHistoryEntry(struct internalScaWindowTask *iwt, BPTR wsLock)
{
	struct WindowHistoryEntry *whe;

	ScalosObtainSemaphoreShared(&iwt->iwt_WindowHistoryListSemaphore);

	for (whe = (struct WindowHistoryEntry *) iwt->iwt_HistoryList.lh_Head;
		whe != (struct WindowHistoryEntry *) &iwt->iwt_HistoryList.lh_Tail;
		whe = (struct WindowHistoryEntry *) whe->whe_Node.ln_Succ)
		{
		BPTR xLock;

		xLock = Lock(whe->whe_Node.ln_Name, ACCESS_READ);
		if (xLock)
			{
			LONG sameLock = SameLock(xLock, wsLock);
			UnLock(xLock);

			if (LOCK_SAME == sameLock)
				{
				// Take care: iwt_WindowHistoryListSemaphore stays locked here!!!
				return whe;
				}
			}
		}

	ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

	return NULL;
}


void WindowNewPath(struct internalScaWindowTask *iwt, CONST_STRPTR path)
{
	struct WindowHistoryEntry *whe = NULL;
	BPTR pLock;

	pLock = Lock((STRPTR) path, ACCESS_READ);
	if (pLock)
		{
		whe = WindowFindHistoryEntry(iwt, pLock);
		UnLock(pLock);
		}
	if (whe)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_NewPath,
			whe->whe_Node.ln_Name,
			SCA_ShowAllMode, whe->whe_ViewAll,
			SCA_ViewModes, whe->whe_Viewmodes,
			SCA_XOffset, whe->whe_XOffset,
			SCA_YOffset, whe->whe_YOffset,
			SCA_IconList, (SCANDIR_OK == whe->whe_ScanDirResult) ? whe->whe_IconList : NULL,
			TAG_END);
		ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
		}
	else
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_NewPath,
			path,
			TAG_END);
		}
}

//----------------------------------------------------------------------------

#define	SHADOWFLAG_LEFT		(1L << 0)
#define	SHADOWFLAG_BOTTOM	(1L << 1)
#define	SHADOWFLAG_TOP		(1L << 2)
#define	SHADOWFLAG_RIGHT	(1L << 3)

static void WindowBlitTransparentShadow(struct Window *PopupWindow,
	LONG Left, LONG Top, ULONG Width, ULONG Height,
	LONG ShadowXOffset, LONG ShadowYOffset, ULONG ShadowFlags)
{
	struct ARGB *SrcARGB = NULL;

	do	{
		struct RastPort rp;
		struct ARGB *pARGB;
		const ULONG MaxAlpha = 180;
		ULONG y, j;

		d1(KPrintF("%s/%s/%ld:  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, Left, Top, Width, Height));

		InitRastPort(&rp);
		rp.BitMap = iInfos.xii_iinfos.ii_Screen->RastPort.BitMap;

		SrcARGB	= ScalosGfxCreateARGB(Width, Height, NULL);
		d1(KPrintF("%s/%s/%ld:  SrcARGB=%08lx\n", __FILE__, __FUNC__, __LINE__, SrcARGB));
		if (NULL == SrcARGB)
			break;

		// read screen background into SrcARGB
		ScalosReadPixelArray(SrcARGB,
			Width * sizeof(struct ARGB),
			&rp,
			PopupWindow->LeftEdge + Left,
			PopupWindow->TopEdge + Top,
			Width,
			Height);

		// darken shadow area
		for (pARGB = SrcARGB + ShadowYOffset * Width, y = ShadowYOffset, j = 0; y < Height; y++, j++)
			{
			ULONG x;
			ULONG k;
			struct ARGB *pLineARGB = pARGB + ShadowXOffset;
			ULONG  ky = (255 * j) / (Height - 1);
			ULONG fy;

			if (ShadowFlags & SHADOWFLAG_TOP)
				fy = ShadowTable[255 - ky];
			else if (ShadowFlags & SHADOWFLAG_BOTTOM)
				fy = ShadowTable[ky];
			else
				fy = 255;

			for (x = ShadowXOffset, k = 0; x < Width; x++, pLineARGB++, k++)
				{
				ULONG fx;
				ULONG f;
				ULONG kx = (255 * k) / (Width - 1);

				if (ShadowFlags & SHADOWFLAG_LEFT)
					fx = ShadowTable[255 - kx];
				else if (ShadowFlags & SHADOWFLAG_RIGHT)
					fx = ShadowTable[kx];
				else
					fx = 255;

				// calculate (1 - Alpha)
				f = 255 - (fx * fy * MaxAlpha) / (256 * 256);
				d1(KPrintF("%s/%s/%ld:  x=%ld  y=%ld  fx=%ld  fy=%ld  f=%ld  ShadowFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, k, j, fx, fy, f, ShadowFlags));

				pLineARGB->Red   = (pLineARGB->Red   * f) / 256;
				pLineARGB->Green = (pLineARGB->Green * f) / 256;
				pLineARGB->Blue  = (pLineARGB->Blue  * f) / 256;
				}

			pARGB += Width;
			}

		// write SrcARGB back to Window RastPort
		ScalosWritePixelArray(SrcARGB,
			Width * sizeof(struct ARGB),
			PopupWindow->RPort,
			Left, Top,
			Width,
			Height);

		Scalos_DoneRastPort(&rp);
		} while (0);

	if (SrcARGB)
		ScalosGfxFreeARGB(&SrcARGB);
}

//----------------------------------------------------------------------------

void WindowCastShadow(struct Window *win, LONG ShadowWidth, LONG ShadowHeight)
{
	// Render top right shadow corner
	WindowBlitTransparentShadow(win,
		win->Width - ShadowWidth, 0,
		ShadowWidth, ShadowHeight,
		0, 0,
		SHADOWFLAG_RIGHT | SHADOWFLAG_TOP);

	// Render right shadow
	WindowBlitTransparentShadow(win,
		win->Width - ShadowWidth, 0,
		ShadowWidth, win->Height - ShadowHeight,
		0, ShadowHeight,
		SHADOWFLAG_RIGHT);

	// Render bottom left shadow corner
	WindowBlitTransparentShadow(win,
		0, win->Height - ShadowHeight,
		ShadowWidth, ShadowHeight,
		0, 0,
		SHADOWFLAG_BOTTOM | SHADOWFLAG_LEFT);

	// Render bottom shadow
	WindowBlitTransparentShadow(win,
		0, win->Height - ShadowHeight,
		win->Width - ShadowWidth, ShadowHeight,
		ShadowWidth, 0,
                SHADOWFLAG_BOTTOM);

	// Render bottom right shadow corner
	WindowBlitTransparentShadow(win,
		win->Width - ShadowWidth, win->Height - ShadowHeight,
		ShadowWidth, ShadowHeight,
		0, 0,
		SHADOWFLAG_BOTTOM | SHADOWFLAG_RIGHT);
}

//----------------------------------------------------------------------------

void WindowSendAppIconMsg(struct internalScaWindowTask *iwt, ULONG AppIconClass, struct ScaIconNode *in)
{
	if (iwt->iwt_AppListSemaphore)
		{
		if (~0 != AppIconClass)
			{
			struct AppObject *appo;

			ScalosObtainSemaphoreShared(iwt->iwt_AppListSemaphore);

			for (appo=iwt->iwt_AppList; appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
				{
				if (APPTYPE_AppIcon == appo->appo_type
					&& in->in_Icon == appo->appo_object.appoo_IconObject)
					{
					SendAppMessage(appo, AppIconClass, iwt->iwt_WindowTask.wt_Window->MouseX, iwt->iwt_WindowTask.wt_Window->MouseY);
					break;
					}
				}

			ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);
			}
		}
}

//----------------------------------------------------------------------------

void WindowFadeIn(struct Window *win)
{
#if defined(__MORPHOS__) && defined(WA_Opacity)
	if (DOSBase->dl_lib.lib_Version >= 51)
			{
		ULONG n;

		for (n = 0; n < 255; n += 32)
			{
			if (SIGBREAKF_CTRL_C & SetSignal(0l, 0l))
				break;

			SetAttrs(win,
				WA_Opacity, (n << 24) + (n << 16) + (n << 8) + (n),
				TAG_END);
			Delay(1);
			}
		SetAttrs(win,
			WA_Opacity, ~0,
			TAG_END);
		}
#endif //defined(__MORPHOS__) && defined(WA_Opacity)
#if defined(__amigaos4__) && defined(WA_Opaqueness)
	{
	ULONG n;

	for (n = 0; n < 255; n += 32)
		{
		if (SIGBREAKF_CTRL_C & SetSignal(0l, 0l))
			break;

		SetWindowAttrs(win,
			WA_Opaqueness,  n,
			TAG_END);
		Delay(1);
		}
	SetWindowAttrs(win,
		WA_Opaqueness, 255,
		TAG_END);
	}
#endif //defined(__amigaos4__) && defined(WA_Opaqueness)
}

//----------------------------------------------------------------------------

void WindowFadeOut(struct Window *win)
{
#if defined(__MORPHOS__) && defined(WA_Opacity)
	if (DOSBase->dl_lib.lib_Version >= 51)
		{
		ULONG n;

		for (n = 0; n < 255; n += 32)
			{
			UBYTE k = (255 - n);

			SetAttrs(win,
				WA_Opacity, (k << 24) + (k << 16) + (k << 8) + (k),
				TAG_END);
			Delay(1);
			}
		}
#elif defined(__amigaos4__) && defined(WA_Opaqueness)
	{
	ULONG n;

	for (n = 0; n < 255; n += 32)
		{
		UBYTE k = (255 - n);

		SetWindowAttrs(win,
			WA_Opaqueness, k,
			TAG_END);
		Delay(1);
		}
	}
#endif //defined(__amigaos4__) && defined(WA_Opaqueness)
}

//----------------------------------------------------------------------------

