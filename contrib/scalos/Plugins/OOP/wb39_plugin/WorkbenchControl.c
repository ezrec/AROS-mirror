// workbenchcontrol.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/intuitionbase.h>
#include <datatypes/iconobject.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <exec/nodes.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/scalos.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/icon.h>
#include <proto/wb.h>

#include <defs.h>

#include "wb39.h"
#include "wb39proto.h"

//-------------------------------------------------------------------------

enum HiddenDevArg { HIDDENDEV_Verbose, HIDDENDEV_Quiet };

//-------------------------------------------------------------------------

// aus wb39.c
extern ULONG DefaultStackSize;
extern ULONG fVolumeGauge;
extern ULONG fTitleBar;
extern ULONG TypeRestartTime;		// Keyboard restart delay in s
extern struct ScaRootList *rList;
extern struct List HiddenDeviceList;    // +dm+ 20010518
extern struct SignalSemaphore HiddenDevListSema;	// +jl+ 20010523 Semaphore to protect HiddenDeviceList


static struct List *AllocList(void);
static void FreeList(struct List *pl);
static BPTR DupWBPathList(void);
static void FreePathList(BPTR bList);
static struct List *GetOpenDrawerList(void);
static BOOL IsOpen(const char *Name);
static BOOL RedrawAppIcon(struct ScaAppObjNode *AppIcon);
static struct List *GetSelectedIconList(void);
static struct List *GetHiddenDeviceList(void);
static void AddHiddenDevice(STRPTR devname, enum HiddenDevArg Quiet);
static void RemoveHiddenDevice(STRPTR devname, enum HiddenDevArg Quiet);
#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
static size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__SASC) &&!defined(__MORPHOS__)  */

//-------------------------------------------------------------------------

static ULONG fIconBorder = FALSE;
static ULONG fDrawerNotification = TRUE;
static ULONG IconMemoryType = MEMF_ANY;

//-------------------------------------------------------------------------

// undocumented WorkbenchControl tags

#ifdef __AROS__
#define WBA_Dummy WBA_BASE
#endif

#define	WBCTRLAX_GetMaxCopyMemory	(WBA_Dummy+40)
#define	WBCTRLAX_SetMaxCopyMemory	(WBA_Dummy+41)

#define	WBCTRLAX_GetIconMemoryType	(WBA_Dummy+49)
#define	WBCTRLAX_SetIconMemoryType	(WBA_Dummy+50)

#define	WBCTRLAX_GetDrawerNotification	(WBA_Dummy+51)
#define	WBCTRLAX_SetDrawerNotification	(WBA_Dummy+52)

#define	WBCTRLAX_AddHiddenDeviceName	(WBA_Dummy+53)		/* used by workbench prefs */
#define	WBCTRLAX_RemoveHiddenDeviceName	(WBA_Dummy+54)		/* used by workbench prefs */

#define	WBCTRLAX_SetVolumeGauge		(WBA_Dummy+59)
#define	WBCTRLAX_GetVolumeGauge		(WBA_Dummy+60)

#define	WBCTRLAX_GetStrangeString	(WBA_Dummy+62)

#define	WBCTRLAX_GetIconBorder		(WBA_Dummy+63)
#define	WBCTRLAX_SetIconBorder		(WBA_Dummy+64)

#define	WBCTRLAX_GetTitleBar		(WBA_Dummy+67)
#define	WBCTRLAX_SetTitleBar		(WBA_Dummy+68)

//-------------------------------------------------------------------------

// WorkbenchControlA()
LIBFUNC_P3(BOOL, myWorkbenchControlA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0)
{
	BOOL Success = TRUE;
	struct TagItem *taglist = tags, *ti;
	LONG ErrorCode = RETURN_OK;

	(void)WorkbenchBase;

	d1(kprintf("%s/%s/%ld: START Caller=<%s>\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)->tc_Node.ln_Name));

	while (ti = NextTagItem(&taglist))
		{
		d1(kprintf("%s/%s/%ld: Tag=%08lx : Data=%08lx  name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
			ti->ti_Tag, ti->ti_Data, name ? name : (STRPTR) ""));

		switch (ti->ti_Tag)
			{
		case WBCTRLA_IsOpen:            // (LONG *)
			d1(kprintf("%s/%s/%ld: WBCTRLA_IsOpen\n", __FILE__, __FUNC__, __LINE__));
			if (ti->ti_Data)
				{
				*((LONG *) ti->ti_Data) = IsOpen(name);
				d1(kprintf("%s/%s/%ld: WBCTRLA_IsOpen = %ld\n", __FILE__, __FUNC__, __LINE__, \
					 *((LONG *) ti->ti_Data)));
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_DuplicateSearchPath:   // (BPTR *)
			if (ti->ti_Data)
				{
				d1(kprintf("%s/%s/%ld: WBCTRLA_DuplicateSearchPath\n", __FILE__, __FUNC__, __LINE__));
				*((BPTR *) ti->ti_Data) = DupWBPathList();
				if ((BPTR)NULL == *((BPTR *) ti->ti_Data))
					ErrorCode = ERROR_NO_FREE_STORE;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_FreeSearchPath:        // (BPTR)
			d1(kprintf("%s/%s/%ld: WBCTRLA_FreeSearchPath\n", __FILE__, __FUNC__, __LINE__));
			FreePathList((BPTR) ti->ti_Data);
			break;

		case WBCTRLA_GetDefaultStackSize:   // (ULONG *)
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetDefaultStackSize\n", __FILE__, __FUNC__, __LINE__));
			if (ti->ti_Data)
				{
				if (ScalosBase->scb_LibNode.lib_Version >= 40)
					{
					SCA_ScalosControl(NULL,
						SCALOSCTRLA_GetDefaultStackSize, (ULONG) &DefaultStackSize,
						TAG_END);
					}
				*((ULONG *) ti->ti_Data) = DefaultStackSize;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_SetDefaultStackSize:   // (ULONG)
			d1(kprintf("%s/%s/%ld: WBCTRLA_SetDefaultStackSize  Size=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data > 4096)
				DefaultStackSize = ti->ti_Data;
			else
				ErrorCode = ERROR_BAD_NUMBER;

			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_SetDefaultStackSize, DefaultStackSize,
					TAG_END);
				}
			break;

		case WBCTRLA_RedrawAppIcon:     // (struct AppIcon *)
			d1(kprintf("%s/%s/%ld: WBCTRLA_RedrawAppIcon  AppIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				Success = RedrawAppIcon((struct ScaAppObjNode *) ti->ti_Data);
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_GetProgramList:        // (struct List **)
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetProgramList\n", __FILE__, __FUNC__, __LINE__));
			if (ti->ti_Data)
				{
				if (ScalosBase->scb_LibNode.lib_Version >= 40)
					{
					SCA_ScalosControl(NULL,
						SCALOSCTRLA_GetProgramList, ti->ti_Data,
						TAG_END);
					}
				else
					{
					*((struct List **) ti->ti_Data) = AllocList();
					}

				if (NULL == *((struct List **) ti->ti_Data))
					ErrorCode = ERROR_NO_FREE_STORE;
						
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_FreeProgramList:       // (struct List *)
			d1(kprintf("%s/%s/%ld: WBCTRLA_FreeProgramList  List=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_FreeProgramList, ti->ti_Data,
					TAG_END);
				}
			else
				{
				FreeList((struct List *) ti->ti_Data);
				}
			break;

		case WBCTRLA_GetSelectedIconList:   // (struct List **)
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetSelectedIconList\n", __FILE__, __FUNC__, __LINE__));
			if (ti->ti_Data)
				{
				*((struct List **) ti->ti_Data) = GetSelectedIconList();
				if (NULL == *((struct List **) ti->ti_Data))
					ErrorCode = ERROR_NO_FREE_STORE;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_FreeSelectedIconList:  // (struct List *)
			d1(kprintf("%s/%s/%ld: FreeSelectedIconList List=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			FreeList((struct List *) ti->ti_Data);
			break;

		case WBCTRLA_GetOpenDrawerList:     // (struct List **)
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetOpenDrawerList\n", __FILE__, __FUNC__, __LINE__));
			if (ti->ti_Data)
				{
				*((struct List **) ti->ti_Data) = GetOpenDrawerList();
				if (NULL == *((struct List **) ti->ti_Data))
					ErrorCode = ERROR_NO_FREE_STORE;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_FreeOpenDrawerList:    // (struct List *)
			d1(kprintf("%s/%s/%ld: WBCTRLA_FreeOpenDrawerList List=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			FreeList((struct List *) ti->ti_Data);
			break;

		case WBCTRLA_AddHiddenDeviceName:	// (STRPTR)
			d1(kprintf("%s/%s/%ld: WBCTRLA_AddHiddenDeviceName  name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			AddHiddenDevice((STRPTR)ti->ti_Data, HIDDENDEV_Verbose);
			break;

		case WBCTRLAX_AddHiddenDeviceName:	// (STRPTR) used by workbench prefs
			d1(kprintf("%s/%s/%ld: WBCTRLA_AddHiddenDeviceName  (+53) name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			AddHiddenDevice((STRPTR)ti->ti_Data, HIDDENDEV_Quiet);
			break;

		case WBCTRLA_RemoveHiddenDeviceName:    // (STRPTR)
			d1(kprintf("%s/%s/%ld: WBCTRLA_RemoveHiddenDeviceName  name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			RemoveHiddenDevice((STRPTR)ti->ti_Data, HIDDENDEV_Verbose);
			break;

		case WBCTRLAX_RemoveHiddenDeviceName:	// (STRPTR) user by workbench prefs
			d1(kprintf("%s/%s/%ld: WBCTRLA_RemoveHiddenDeviceName (+54)  name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			RemoveHiddenDevice((STRPTR)ti->ti_Data, HIDDENDEV_Quiet);
			break;

		case WBCTRLA_GetHiddenDeviceList:	// (struct List **)
			// +dm+ 20010518 start
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetHiddenDeviceList  List **=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if(ti->ti_Data)
			{
				*((struct List **) ti->ti_Data) = GetHiddenDeviceList();
				d1(kprintf("%s/%s/%ld: WBCTRLA_GetHiddenDeviceList  List *=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					*((struct List **) ti->ti_Data)));
				if (NULL == *((struct List **) ti->ti_Data))
					ErrorCode = ERROR_NO_FREE_STORE;
			}
			// +dm+ 20010518 end
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_FreeHiddenDeviceList:  // (struct List *)
			d1(kprintf("%s/%s/%ld: WBCTRLA_FreeHiddenDeviceList  List=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			FreeList((struct List *) ti->ti_Data);
			break;

		case WBCTRLA_GetTypeRestartTime:    // (ULONG *)
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetTypeRestartTime\n", __FILE__, __FUNC__, __LINE__));
			if (ti->ti_Data)
				{
				if (ScalosBase->scb_LibNode.lib_Version >= 40)
					{
					SCA_ScalosControl(NULL,
						SCALOSCTRLA_GetTypeRestartTime, (ULONG) &TypeRestartTime,
						TAG_END);
					}

				*((ULONG *) ti->ti_Data) = TypeRestartTime;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_SetTypeRestartTime:    // (ULONG)
			d1(kprintf("%s/%s/%ld: WBCTRLA_SetTypeRestartTime  time=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data > 0)
				{
				// nop
				TypeRestartTime = ti->ti_Data;

				if (ScalosBase->scb_LibNode.lib_Version >= 40)
					{
					SCA_ScalosControl(NULL,
						SCALOSCTRLA_SetTypeRestartTime, TypeRestartTime,
						TAG_END);
					}
				}
			else
				ErrorCode = ERROR_BAD_NUMBER;
			break;

		case WBA_Dummy+46:		// (ULONG)
			// Set/clear icon.library global screen
			// TRUE=set, FALSE=clear
			d1(kprintf("%s/%s/%ld: (set global screen) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));

			switch (ti->ti_Data)
				{
			case FALSE:
				// IconControlA()
				IconControl(NULL, 
					ICONCTRLA_SetGlobalScreen, NULL,
					TAG_END);
				break;
			case TRUE:
				{
				struct Screen *wbScreen = LockPubScreen("Workbench");

				IconControl(NULL, 
					ICONCTRLA_SetGlobalScreen, (ULONG) wbScreen,
					TAG_END);

				UnlockPubScreen(NULL, wbScreen);
				}
				break;
				}
			break;

		case WBCTRLAX_GetMaxCopyMemory:	// (ULONG *)
			d1(kprintf("%s/%s/%ld: (get max copy memory) f=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				if (ScalosBase->scb_LibNode.lib_Version >= 40)
					{
					SCA_ScalosControl(NULL,
						SCALOSCTRLA_GetCopyBuffSize, ti->ti_Data,
						TAG_END);
					}
				else
					{
					ErrorCode = ERROR_INVALID_RESIDENT_LIBRARY;
					}
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLAX_SetMaxCopyMemory:	// (ULONG)
			d1(kprintf("%s/%s/%ld: (set max copy memory) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data >= 1000 && ti->ti_Data <= 0x400000)
				{
				if (ScalosBase->scb_LibNode.lib_Version >= 40)
					{
					SCA_ScalosControl(NULL,
						SCALOSCTRLA_SetCopyBuffSize, ti->ti_Data,
						TAG_END);
					}
				else
					{
					ErrorCode = ERROR_INVALID_RESIDENT_LIBRARY;
					}
				}
			else
				ErrorCode = ERROR_BAD_NUMBER;
			break;

		case WBCTRLAX_GetDrawerNotification:	// (ULONG *)
			d1(kprintf("%s/%s/%ld: (get drawer notification) f=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				*((ULONG *) ti->ti_Data) = fDrawerNotification;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLAX_SetDrawerNotification:	// (ULONG)
			d1(kprintf("%s/%s/%ld: (set drawer notification) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			fDrawerNotification = 0 != ti->ti_Data;
			break;

		case WBCTRLAX_GetIconMemoryType:	// (ULONG *)
			d1(kprintf("%s/%s/%ld: (get Icon memory type) f=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				*((ULONG *) ti->ti_Data) = IconMemoryType;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLAX_SetIconMemoryType:	// (ULONG)
			d1(kprintf("%s/%s/%ld: (set Icon memory type) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			IconMemoryType = ti->ti_Data;
			break;

		case WBCTRLAX_GetVolumeGauge:		// (ULONG *)
			// query Volume Gauge
			// TRUE=on, FALSE=off
			d1(kprintf("%s/%s/%ld: (get Volume Gauge) f=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				*((ULONG *) ti->ti_Data) = fVolumeGauge;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLAX_SetVolumeGauge:	// (ULONG)
			// Switch Volume Gauge
			// TRUE=on, FALSE=off
			d1(kprintf("%s/%s/%ld: (set Volume Gauge) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));

			fVolumeGauge = 0 != ti->ti_Data;
			break;

		case WBCTRLAX_GetStrangeString:	// (ULONG *)
			d1(kprintf("%s/%s/%ld: WBCTRLAX_GetStrangeString f=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				// Heaven knowns what is it good for...
				// but workbench does it that way...
				ULONG *lPtr = (ULONG *) ti->ti_Data;

				*lPtr++ = 0x67452301;
				*lPtr++ = 0xEFCDAB89;
				*lPtr++ = 0x98BADCFE;
				*lPtr++ = 0x10325476;
				*lPtr++ = 0;
				*lPtr = 0;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLAX_GetIconBorder:	// (ULONG *)
			d1(kprintf("%s/%s/%ld: (get icon border) f=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				*((ULONG *) ti->ti_Data) = fIconBorder;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLAX_SetIconBorder:		// (ULONG)
			d1(kprintf("%s/%s/%ld: (set Icon border) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			fIconBorder = 0 != ti->ti_Data;
			break;

		case WBCTRLAX_GetTitleBar:	// (ULONG *)
			d1(kprintf("%s/%s/%ld: (get Title bar) f=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ti->ti_Data)
				{
				*((ULONG *) ti->ti_Data) = fTitleBar;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			d1(kprintf("%s/%s/%ld: fTitleBar=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				fTitleBar));
			break;

		case WBCTRLAX_SetTitleBar:	// (ULONG)
			// Switch Title Bar
			// TRUE=on, FALSE=off
			d1(kprintf("%s/%s/%ld: (set Title bar) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));

			fTitleBar = 0 != ti->ti_Data;
			d1(kprintf("%s/%s/%ld: fTitleBar=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				fTitleBar));
			break;

		case WBA_Dummy+55:		// (ULONG *)
			// seems to do some kind of Workbench refresh
			d1(kprintf("%s/%s/%ld: (refresh WB) f=%lu\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));

			if (rList && rList->rl_internInfos->ii_MainWindowStruct)
				{
				UpdateScalosWindow(rList->rl_internInfos->ii_MainWindowStruct);
				}
			break;

		case WBA_Dummy+58:		// (ULONG *)
			// looks like some kind of copy protection for AWeb-II
			d1(kprintf("%s/%s/%ld: 0x8000A03A\n", __FILE__, __FUNC__, __LINE__));
			if (ti->ti_Data)
				{
				*((ULONG *) ti->ti_Data) = 0x1731f8e9;
				}
			else
				ErrorCode = ERROR_REQUIRED_ARG_MISSING;
			break;

		case WBCTRLA_GetCopyHook:
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetCopyHook\n", __FILE__, __FUNC__, __LINE__));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_GetCopyHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		case WBCTRLA_SetCopyHook:
			d1(kprintf("%s/%s/%ld: WBCTRLA_SetCopyHook Hook=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_SetCopyHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		case WBCTRLA_GetDeleteHook:
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetDeleteHook\n", __FILE__, __FUNC__, __LINE__));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_GetDeleteHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		case WBCTRLA_SetDeleteHook:
			d1(kprintf("%s/%s/%ld: WBCTRLA_SetDeleteHook Hook=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_SetDeleteHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		case WBCTRLA_GetTextInputHook:
			d1(kprintf("%s/%s/%ld: WBCTRLA_GetTextInputHook\n", __FILE__, __FUNC__, __LINE__));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_GetTextInputHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		case WBCTRLA_SetTextInputHook:
			d1(kprintf("%s/%s/%ld: WBCTRLA_SetTextInputHook Hook=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_SetTextInputHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		case WBCTRLA_AddSetupCleanupHook:	// Add hook to CloseWB list
			d1(kprintf("%s/%s/%ld: WBCTRLA_AddCloseWBHook Hook=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_AddCloseWBHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		case WBCTRLA_RemSetupCleanupHook:	// Remove hook from CloseWB list
			d1(kprintf("%s/%s/%ld: WBCTRLA_RemCloseWBHook Hook=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Data));
			if (ScalosBase->scb_LibNode.lib_Version >= 40)
				{
				SCA_ScalosControl(NULL,
					SCALOSCTRLA_RemCloseWBHook, ti->ti_Data,
					TAG_END);
				}
			else
				Success = FALSE;
			break;

		default:
			d1(kprintf("%s/%s/%ld: Tag=%08lx (WBA_Dummy+%ld): Data=%lu (0x%08lx)\n", __FILE__, __FUNC__, __LINE__, \
				ti->ti_Tag, ti->ti_Tag-WBA_Dummy, ti->ti_Data, ti->ti_Data));
			Success = FALSE;
			break;
			}
		}

	if (RETURN_OK != ErrorCode)
		{
		SetIoErr(ErrorCode);
		Success = FALSE;
		}
#if 0
	APTR ptr;

	kprintf("%s/%s/%ld: name=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		name ? name : (STRPTR) "");

	Success = origWorkbenchControlA(name, tags);

	while (ti = NextTagItem(&taglist))
		{
		kprintf("%s/%s/%ld: Tag=%08lx : Data=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			ti->ti_Tag, ti->ti_Data);
		}
#endif

	return Success;
}
LIBFUNC_END


static struct List *AllocList(void)
{
	struct List *pl = malloc(sizeof(struct List));

	if (pl)
		{
		pl->lh_Type = 0;

		NewList(pl);
		}

	return pl;
}


static void FreeList(struct List *pl)
{
	struct Node *xNode;

	if (NULL == pl)
		return;

	while (xNode = RemHead(pl))
		{
		if (xNode->ln_Name)
			{
			free(xNode->ln_Name);
			xNode->ln_Name = NULL;
			}
		free(xNode);
		}

	free(pl);
}


static BPTR DupWBPathList(void)
{
	struct Process *wbProc = (struct Process *) FindTask("Workbench");
	struct CommandLineInterface *cli;
	struct AssignList *aList, *StartList = NULL, **nList = NULL;

	d1(KPrintF("%s/%s/%ld: START wbProc=%08lx\n", __FILE__, __FUNC__, __LINE__, wbProc));
	if (NULL == wbProc)
		wbProc = (struct Process *) FindTask("Scalos_Window_Task_Main");
	d1(KPrintF("%s/%s/%ld: wbProc=%08lx\n", __FILE__, __FUNC__, __LINE__, wbProc));
	if (NULL == wbProc)
		return (BPTR)NULL;
	if (NT_PROCESS != wbProc->pr_Task.tc_Node.ln_Type)
		return (BPTR)NULL;

	cli = BADDR(wbProc->pr_CLI);
	d1(KPrintF("%s/%s/%ld: cli=%08lx\n", __FILE__, __FUNC__, __LINE__, cli));
	if (NULL == cli)
		return (BPTR)NULL;

	aList = BADDR(cli->cli_CommandDir);
	d1(KPrintF("%s/%s/%ld: aList=%08lx\n", __FILE__, __FUNC__, __LINE__, aList));
	if (NULL == aList)
		return (BPTR)NULL;

	while (aList)
		{
		struct AssignList *nNode = AllocVec(sizeof(struct AssignList), MEMF_PUBLIC);

		if (NULL == nNode)
			break;
		if (NULL == nList)
			StartList = nNode;
		else
			*nList = (struct AssignList *) MKBADDR(nNode);

		nNode->al_Next = NULL;
		nNode->al_Lock = DupLock(aList->al_Lock);
		nList = &nNode->al_Next;

		d1(KPrintF("%s/%s/%ld: aList=%08lx Next=%08lx Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			aList, aList->al_Next, aList->al_Lock));
		debugLock_d1(aList->al_Lock);

		aList = BADDR(aList->al_Next);
		}

	d1(KPrintF("%s/%s/%ld: END  StartList=%08lx\n", __FILE__, __FUNC__, __LINE__, StartList));

	return MKBADDR(StartList);
}


static void FreePathList(BPTR bList)
{
	struct AssignList *aList;

	aList = BADDR(bList);
	while (aList)
		{
		struct AssignList *NextList = BADDR(aList->al_Next);

		d1(kprintf("%s/%s/%ld: aList=%08lx Next=%08lx  Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			aList, NextList, aList->al_Lock));

		UnLock(aList->al_Lock);
		FreeVec(aList);

		aList = NextList;
		}
}


static struct List *GetOpenDrawerList(void)
{
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_AttemptShared);
	struct List *odList = AllocList();

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: swi=%08lx Lock=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				swi, swi->ws_Lock, swi->ws_Flags));

			if (swi->ws_Lock)
				{
				char Buffer[255];

				if (NameFromLock(swi->ws_Lock, Buffer, sizeof(Buffer)))
					{
					struct Node *newNode = calloc(1, sizeof(struct Node));

					if (newNode)
						{
						newNode->ln_Type = 0;
						newNode->ln_Name = strdup(Buffer);

						AddTail(odList, newNode);
						}
					}
				}
			}

		SCA_UnLockWindowList();
		}

	return odList;
}


static BOOL IsOpen(const char *Name)
{
	BOOL Found = FALSE;
	struct ScaWindowList *swList = NULL;
	BPTR nLock = (BPTR)NULL;

	do	{
		struct ScaWindowStruct *swi;

		if (NULL == Name)
			break;

		swList = SCA_LockWindowList(SCA_LockWindowList_AttemptShared);
		if (NULL == swList)
			break;

		nLock = Lock((STRPTR) Name, ACCESS_READ);
		if ((BPTR)NULL == nLock)
			break;

		for (swi=swList->wl_WindowStruct; !Found && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: swi=%08lx Lock=%08lx nLock=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				swi, swi->ws_Lock, nLock));

			if (LOCK_SAME == SameLock(swi->ws_Lock, nLock))
				{
				Found = TRUE;
				}
			}
		} while (0);

	if (swList)
		SCA_UnLockWindowList();
	if (nLock)
		UnLock(nLock);

	d1(kprintf("%s/%s/%ld: Found=%ld\n", __FILE__, __FUNC__, __LINE__, Found));

	return Found;
}


static BOOL RedrawAppIcon(struct ScaAppObjNode *ai)
{
	struct ScaWindowList *swList;
	struct ScaWindowStruct *RootWindow = NULL;
	BOOL Result = FALSE;

	d1(kprintf("%s/%s/%ld: AppIcon=%08lx\n", __FILE__, __FUNC__, __LINE__,
		ai));

	d1(kprintf("%s/%s/%ld: RootList=%08lx  InternInfos=%08lx\n", __FILE__, __FUNC__, __LINE__,
		rList, rList->rl_internInfos));

	if (NULL == ai)
		return FALSE;

	swList = SCA_LockWindowList(SCA_LockWindowList_AttemptShared);
	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; !RootWindow && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			if ((BPTR)NULL == swi->ws_Lock)
				RootWindow = swi;
			}

		d1(kprintf("%s/%s/%ld: RootWindow=%08lx\n", __FILE__, __FUNC__, __LINE__,
			RootWindow));

		if (RootWindow)
			{
			struct ScaWindowTask *rwt = RootWindow->ws_WindowTask;

			if (AttemptSemaphoreShared(rwt->wt_IconSemaphore))
				{
				struct ScaIconNode *in;

				for (in = rwt->wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					if (in->in_Icon == (Object *) ai->an_object)
						{
						DoMethod(rwt->mt_MainObject, 
							SCCM_IconWin_DrawIcon, ai->an_object);
						break;
						}
					}

				ReleaseSemaphore(rwt->wt_IconSemaphore);
				}

			Result = TRUE;
			}

		SCA_UnLockWindowList();
		}

	return Result;
}


static struct List *GetSelectedIconList(void)
{
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_AttemptShared);
	struct List *odList = AllocList();

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			struct ScaWindowTask *wt = swi->ws_WindowTask;

			d1(kprintf("%s/%s/%ld: ScaWindowStruct=%08lx  ScaWindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__,
				swi, wt));

			if (wt)
				{
				struct ScaIconNode *icon;
				char wBuffer[256];

				if (swi->ws_Lock)
					NameFromLock(swi->ws_Lock, wBuffer, sizeof(wBuffer));

				ObtainSemaphore(wt->wt_IconSemaphore);

				for (icon=wt->wt_IconList; icon; icon = (struct ScaIconNode *) icon->in_Node.mln_Succ)
					{
					char *Name = icon->in_Name;

					if (NULL == Name)
						GetAttr(IDTA_Text, icon->in_Icon, (APTR) &Name);

					if (Name && isIconSelected(icon))
						{
						struct Node *newNode;
						char Buffer[255];

						d1(kprintf("%s/%s/%ld: icon=%08lx  Lock=%08lx  Name=%08lx\n", __FILE__, __FUNC__, __LINE__, \
							icon, icon->in_Lock, Name));

						if (icon->in_Lock)
							{
							if (NameFromLock(icon->in_Lock, Buffer, sizeof(Buffer)))
								{
								AddPart(Buffer, Name, sizeof(Buffer));
								}
							}
						else
							{
							stccpy(Buffer, wBuffer, sizeof(Buffer));
							AddPart(Buffer, Name, sizeof(Buffer));
							}

						newNode = calloc(1, sizeof(struct Node));
						if (newNode)
							{
							newNode->ln_Type = NT_WB;
							newNode->ln_Name = strdup(Buffer);

							AddTail(odList, newNode);
							}
						}
					}

				ReleaseSemaphore(wt->wt_IconSemaphore);
				}
			}

		SCA_UnLockWindowList();
		}

	return odList;
}


// +dm+ 20010518 Full support for hidden device lists in workbench
static struct List *GetHiddenDeviceList(void)
{
	struct List *newdevlist;
	struct Node *hiddennode, *newdevnode;

	newdevlist = AllocList();
	d1(kprintf("%s/%s/%ld: newdevlist=%08lx\n", __FILE__, __FUNC__, __LINE__,
		newdevlist));
	if(newdevlist)
	{
		ObtainSemaphoreShared(&HiddenDevListSema);

		hiddennode = HiddenDeviceList.lh_Head;
		while(hiddennode->ln_Succ)
		{
			if(hiddennode->ln_Name)
			{
				d1(kprintf("%s/%s/%ld: hiddennode=%08lx  name=<%s>\n", __FILE__, __FUNC__, __LINE__,
					hiddennode, hiddennode->ln_Name));
				newdevnode = calloc(1, sizeof(struct Node));
				if(newdevnode)
				{
					newdevnode->ln_Type = NT_WB;	// +jl+ 20010524
					newdevnode->ln_Name = calloc(strlen(hiddennode->ln_Name)+1, sizeof(char));
					if(newdevnode->ln_Name)
					{
						strcpy(newdevnode->ln_Name, hiddennode->ln_Name);
						AddTail(newdevlist, newdevnode);
						d1(kprintf("%s/%s/%ld: newdevnode=%08lx  name=<%s>\n", __FILE__, __FUNC__, __LINE__,
							newdevnode, newdevnode->ln_Name));
					}
					else
					{
						free(newdevnode);
					} // if(newdevnode->ln_Name)
				} // if(newdevnode)
			} // if(hiddennode->ln_Name)
			hiddennode = hiddennode->ln_Succ;
		} // while(hiddennode->ln_Succ)

		ReleaseSemaphore(&HiddenDevListSema);
	}
	return(newdevlist);
}


static void AddHiddenDevice(STRPTR devname, enum HiddenDevArg Quiet)
{
	struct Node *hiddennode;
	int devlen = strlen(devname);
	BOOL notfound = TRUE;

	if(devname)
	{
		ObtainSemaphore(&HiddenDevListSema);

		if(devname[devlen-1] == ':')
		{
			hiddennode = HiddenDeviceList.lh_Head;
			while(hiddennode->ln_Succ && notfound)
			{
				if(hiddennode->ln_Name)
				{
					d1(kprintf("%s/%s/%ld: hiddennode=%08lx  name=<%s>  addname=<%s>\n", __FILE__, __FUNC__, __LINE__,
						hiddennode, hiddennode->ln_Name, devname));
					notfound = Stricmp(hiddennode->ln_Name, devname);
				}
				hiddennode = hiddennode->ln_Succ;
			}

			if(notfound)
			{
				// Allocate new node
				hiddennode = calloc(1, sizeof(struct Node));
				if(hiddennode)
				{
					hiddennode->ln_Type = NT_WB;
					hiddennode->ln_Name = calloc(devlen+1, sizeof(char));
					if(hiddennode->ln_Name)
					{
						strcpy(hiddennode->ln_Name, devname);
						AddTail(&HiddenDeviceList, hiddennode);
						d1(kprintf("%s/%s/%ld: newnode=%08lx  newname=<%s>\n", __FILE__, __FUNC__, __LINE__,
							hiddennode, hiddennode->ln_Name));
					}
					else
					{
						free(hiddennode);
					}
				} // if(hiddennode) allocated
			} // if device name not found
		} // if proper device name
		ReleaseSemaphore(&HiddenDevListSema);

		if ((HIDDENDEV_Verbose == Quiet) && notfound && rList && rList->rl_internInfos->ii_MainWindowStruct)
			UpdateScalosWindow(rList->rl_internInfos->ii_MainWindowStruct);
	} // if devname valid
}


static void RemoveHiddenDevice(STRPTR devname, enum HiddenDevArg Quiet)
{
	struct Node *hiddennode, *xNode = NULL;
	int devlen = strlen(devname);

	if(devname)
	{
		ObtainSemaphore(&HiddenDevListSema);

		if(devname[devlen-1] == ':')
		{
			hiddennode = HiddenDeviceList.lh_Head;
			while(hiddennode->ln_Succ && !xNode)
			{
				if(hiddennode->ln_Name)
				{
					d1(kprintf("%s/%s/%ld: hiddennode=%08lx  name=<%s>  remname=<%s>\n", __FILE__, __FUNC__, __LINE__,
						hiddennode, hiddennode->ln_Name, devname));
					if(Stricmp(hiddennode->ln_Name, devname) == 0)
						 xNode = hiddennode;
				}
				hiddennode = hiddennode->ln_Succ;
			}

			if(xNode)
			{
				d1(kprintf("%s/%s/%ld: remnode=%08lx  nodename=<%s>  remname=<%s>\n", __FILE__, __FUNC__, __LINE__,
					xNode, xNode->ln_Name, devname));
				Remove(xNode);
				if(xNode->ln_Name) free(xNode->ln_Name);
				free(xNode);
			} // if device name not found
		} // if proper device name

		ReleaseSemaphore(&HiddenDevListSema);

		if ((HIDDENDEV_Verbose == Quiet) && xNode && rList && rList->rl_internInfos->ii_MainWindowStruct)
			UpdateScalosWindow(rList->rl_internInfos->ii_MainWindowStruct);
	} // if devname valid
}
// +dm+ 20010518 end


#ifdef __amigaos4__
LIBFUNC_P2VA(BOOL, myWorkbenchControl,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase)
{
	BOOL ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, name);

	(void)WorkbenchBase;

	ret = WorkbenchControlA(name, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END
#endif

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
// Replacement for SAS/C library functions

static size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}
#endif /* !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) */
