// AutoUpdate.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <utility/hooks.h>
#include <workbench/workbench.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

#define	CHK_MSG_COUNT		10

struct CheckUpdateData
	{
	struct Node cud_Node;
	struct ScaIconNode *cud_IconNode;
	CONST_STRPTR cud_IconName;
	CONST_STRPTR cud_FileName;		// Filename (from cud_iseObject or cud_iseIcon)
	struct IconScanEntry *cud_iseObject;
	struct IconScanEntry *cud_iseIcon;
	UWORD cud_OldIconFlags;
	BOOL cud_SoloIcon;			// Flag: old icon was "solo", i.e. icon w/o object
	struct internalScaWindowTask *cud_iwt;	// iwt this icons belongs to - may be different from current iwt for left-out icons
	};

struct CudFilesList
	{
	BOOL cfl_ListInitialized;
	struct List cfl_FilesList;
	struct BTree *cfl_FilesTree;		// BTree of all files
	};

//----------------------------------------------------------------------------

// local functions

static void BuildCUDIconList(struct internalScaWindowTask *iwt, struct CudFilesList *cfl, BPTR dirLock);
static void DeviceWindowBuildCUDIconList(struct internalScaWindowTask *iwt, struct CudFilesList *cfl, BPTR dirLock);
static void DeviceWindowBuildFileList(struct ReadIconListControl *rilc);
static void CheckLeftoutIcons(struct CudFilesList *cfl, BPTR dirLock);
static struct CheckUpdateData *FindCudByName(struct CudFilesList *list, const char *Name);
static struct ScaIconNode *ImmediateUpdateIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *inUpdate);
static void AddToCheckUpdateFilesList(struct internalScaWindowTask *iwt,
	struct CudFilesList *cfl, const struct IconScanEntry *ise);
static BOOL DateFromDiskIcon(struct ScaDeviceIcon *di, struct DateStamp *ds);
static BOOL IsThumbnailIcon(struct ScaIconNode *in);
static BOOL InitCudFilesList(struct CudFilesList *cfl);
static void CleanupCudFilesList(struct CudFilesList *cfl);
static void CudDisposeData(void *data);
static void CudDisposeKey(void *key);
static int CudCompare(const void *key1, const void *key2);
static void RefreshTextWindow(struct internalScaWindowTask *iwt);
static struct CheckUpdateData *CreateCheckUpdateData(struct internalScaWindowTask *iwt, struct ScaIconNode *in);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

/// DeviceWindowCheckUpdate
ULONG DeviceWindowCheckUpdate(struct internalScaWindowTask *iwt)
{
	struct CudFilesList cfl;
	struct IconScanEntry *ise;
	struct ReadIconListControl rilc;
	struct CheckUpdateData *cud;
	struct DateStamp dsIconUpdate;
	ULONG Success = FALSE;
	BOOL SemaphoresLocked = FALSE;
	BPTR dirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

	d1(KPrintF("\n" "%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		ULONG count = 0;

		memset(&cfl, 0, sizeof(cfl));
		
		if (!RilcInit(&rilc, iwt))
			break;

		if (++iwt->iwt_CheckUpdateCount > 1)
			{
			d1(KPrintF("%s/%s/%ld: recursive call detected.\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		if (!ScaAttemptSemaphoreList(SCA_SemaphoreExclusiveNoNest, (ULONG) &iwt->iwt_UpdateSemaphore,
			SCA_SemaphoreExclusive, (ULONG) iwt->iwt_WindowTask.wt_IconSemaphore,
			SCA_SemaphoreShared, (ULONG) iwt->iwt_WindowTask.wt_WindowSemaphore,
			TAG_END))
			{
			d1(KPrintF("%s/%s/%ld: AttemptSemaphoreNoNest(&iwt->iwt_UpdateSemaphore) failed.\n", __FILE__, __FUNC__, __LINE__));
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ScheduleUpdate);		// reschedule update
			break;
			}

		SemaphoresLocked = TRUE;

		if (NULL == iwt->iwt_WindowTask.wt_Window)
			break;

		d1(kprintf("%s/%s/%ld: UpdateSemaphore=%08lx  NestCount=%ld\n", \
			__FILE__, __FUNC__, __LINE__, &iwt->iwt_UpdateSemaphore, iwt->iwt_UpdateSemaphore.ss_NestCount));

		if (!InitCudFilesList(&cfl))
			break;

		if (iwt->iwt_WindowTask.wt_LateIconList)
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);

		DateStamp(&dsIconUpdate);

		// add left-out icons from wt_IconList and wt_LateIconList to cfl
		DeviceWindowBuildCUDIconList(iwt, &cfl, dirLock);

		// this is the main window, add files for left-out icons to <rilc.rilc_IconScanList> and <rilc.rilc_NonIconScanList>
		DeviceWindowBuildFileList(&rilc);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// build links between icons and objects
		LinkIconScanList(&rilc);

		// Add (left-out) file entries for icons
		for (ise = (struct IconScanEntry *) rilc.rilc_IconScanList.lh_Head;
			ise != (struct IconScanEntry *) &rilc.rilc_IconScanList.lh_Tail;
			ise = (struct IconScanEntry *) ise->ise_Node.ln_Succ)
			{
			BOOL IconExists = TRUE;

			d1(kprintf("%s/%s/%ld: ise=%08lx <%s>  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ise, ise->ise_Fib.fib_FileName, ise->ise_Flags));
			d1(kprintf("%s/%s/%ld: IconPtr=%08lx  ObjPtr=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				ise->ise_IconPtr, ise->ise_ObjPtr));

			if (IsFileHidden(ise->ise_Fib.fib_FileName, ise->ise_Fib.fib_Protection))
				continue;

			// strip ".info" from name
			*((char *) ise->ise_Pos) = '\0';

			// special treatment for "disk.info" type WBDISK icons
			if (0 == Stricmp(ise->ise_Fib.fib_FileName, "disk"))
				{
				Object *DiskInfoObj = NewIconObject(ise->ise_Fib.fib_FileName, NULL);

				if (DiskInfoObj)
					{
					ULONG IconType;

					GetAttr(IDTA_Type, DiskInfoObj, &IconType);
					if (WBDISK == IconType)
						IconExists = FALSE;

					DisposeIconObject(DiskInfoObj);
					}
				}

			d1(kprintf("%s/%s/%ld: IconExists=%ld\n", __FILE__, __FUNC__, __LINE__, IconExists));

			if (IconExists)
				AddToCheckUpdateFilesList(iwt, &cfl, ise);

			d1(kprintf("%s/%s/%ld: <%s>  IconExists=%ld\n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName, IconExists));
			}

		d1(kprintf("%s/%s/%ld: IoErr=%ld\n", __FILE__, __FUNC__, __LINE__, IoErr()));

		while ((cud = ((struct CheckUpdateData *) RemHead(&cfl.cfl_FilesList))))
			{
			BOOL fExists = FALSE;

			d1(kprintf("%s/%s/%ld: cud=%08lx\n", __FILE__, __FUNC__, __LINE__, cud));

			if (iwt->iwt_WindowTask.mt_MainObject && count++ >= CHK_MSG_COUNT)
				{
				// Allow processing of messages
				count = 0;
				if (DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, SCCM_CheckForMessages))
					break;
				}

			if (cud->cud_IconNode)
				{
				ULONG IconType;

				GetAttr(IDTA_Type, cud->cud_IconNode->in_Icon, &IconType);

				d1(kprintf("%s/%s/%ld: <%s> in_Flags=%04lx  IconType=%ld\n",
					__FILE__, __FUNC__, __LINE__, cud->cud_IconName, cud->cud_IconNode->in_Flags, IconType));

				// we assume that disk icons are always present
				if (WBDISK == IconType)
					fExists = TRUE;
				}

			if (cud->cud_iseIcon)
				fExists = TRUE;

			d1(kprintf("%s/%s/%ld: iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				cud->cud_iseObject, cud->cud_iseIcon));

			if (cud->cud_IconNode && fExists)
				{
				// File and Icon found
				struct DateStamp DateDiskIcon;
				struct DateStamp *fDate = &DateDiskIcon;

				d1(kprintf("%s/%s/%ld: (update) cud=%08lx  File=<%s>  Icon=<%s>\n", \
					__FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName, cud->cud_IconName));

				if (cud->cud_IconNode->in_DeviceIcon)
					{
					fDate = &DateDiskIcon;

					if (!DateFromDiskIcon(cud->cud_IconNode->in_DeviceIcon, &DateDiskIcon))
						{
						// Device icon not really present
						d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
						DateDiskIcon = cud->cud_IconNode->in_DateStamp;
						}

					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					}
				else
					{
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					if (cud->cud_iseIcon && cud->cud_iseObject)
						{
						// use date from object
						fDate = &cud->cud_iseObject->ise_Fib.fib_Date;
						}
					else
						{
						if (cud->cud_iseIcon)
							fDate = &cud->cud_iseIcon->ise_Fib.fib_Date;
						if (cud->cud_iseObject)
							fDate = &cud->cud_iseObject->ise_Fib.fib_Date;
						}
					}


				d1(kprintf("%s/%s/%ld: Window DateStamp Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
						iwt->iwt_LastIconUpdateTime.ds_Days, \
						iwt->iwt_LastIconUpdateTime.ds_Minute, \
						iwt->iwt_LastIconUpdateTime.ds_Tick));
				d1(kprintf("%s/%s/%ld: IconNode DateStamp Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_IconNode->in_DateStamp.ds_Days, \
					cud->cud_IconNode->in_DateStamp.ds_Minute, \
					cud->cud_IconNode->in_DateStamp.ds_Tick));
				d1(kprintf("%s/%s/%ld: File DateStamp Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
					fDate->ds_Days, fDate->ds_Minute, fDate->ds_Tick));
				d1(kprintf("%s/%s/%ld: in_Flags=%08lx  cud_OldIconFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_IconNode->in_Flags, cud->cud_OldIconFlags));

				if (((cud->cud_IconNode->in_Flags & INF_DefaultIcon) != (cud->cud_OldIconFlags & INF_DefaultIcon)) ||
					(cud->cud_FileName && strlen(cud->cud_FileName) > 0 && 0 != strcmp(cud->cud_IconName, cud->cud_FileName)) ||
					CompareDates(&cud->cud_IconNode->in_DateStamp, fDate) > 0 )
					{
					d1(KPrintF("%s/%s/%ld: UPDATE cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

					ImmediateUpdateIcon(cud->cud_iwt, cud->cud_IconNode);

					cud->cud_IconNode->in_DateStamp = *fDate;
					}
				}

			ScalosFree(cud);
			}

		d1(KPrintF("%s/%s/%ld: cud=%08lx  ws_Viewmodes=%ld\n", \
			__FILE__, __FUNC__, __LINE__, cud, iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes));

		iwt->iwt_LastIconUpdateTime = dsIconUpdate;		// remember time of last icon update
		iwt->iwt_IconUpdateCount = 0;			// reset icon update counter

		Success = TRUE;
		} while (0);

	CleanupCudFilesList(&cfl);
	RilcCleanup(&rilc);

	if (SemaphoresLocked)
		{
		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
		ScalosUnLockIconList(iwt);
		ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);
		}

	iwt->iwt_CheckUpdateCount--;

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return Success;
}
///

/// IconWindowCheckUpdate
ULONG IconWindowCheckUpdate(struct internalScaWindowTask *iwt)
{
	struct CudFilesList cfl;
	struct ReadIconListControl rilc;
	enum ScanDirResult sdResult;
	struct CheckUpdateData *cud;
	struct DateStamp dsIconUpdate;
	BPTR dirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;
	BOOL VolumeIsWritable;
	ULONG Success = FALSE;
	BOOL SemaphoresLocked = FALSE;

	d1(KPrintF("\n" "%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	debugLock_d1(dirLock);

	do	{
		struct IconScanEntry *ise;
		ULONG count;
		struct ScaIconNode *in;

		memset(&cfl, 0, sizeof(cfl));
		
		if (!RilcInit(&rilc, iwt))
			break;

		if (++iwt->iwt_CheckUpdateCount > 1)
			{
			d1(KPrintF("%s/%s/%ld: recursive call detected.\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		if (!ScaAttemptSemaphoreList(SCA_SemaphoreExclusiveNoNest, (ULONG) &iwt->iwt_UpdateSemaphore,
			SCA_SemaphoreExclusive, (ULONG) iwt->iwt_WindowTask.wt_IconSemaphore,
			SCA_SemaphoreExclusive, (ULONG) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->wt_IconSemaphore,
			SCA_SemaphoreShared, (ULONG) iwt->iwt_WindowTask.wt_WindowSemaphore,
			TAG_END))
			{
			d1(KPrintF("%s/%s/%ld: ScaAttemptSemaphoreList(&iwt->iwt_UpdateSemaphore) failed.\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ScheduleUpdate);		// reschedule update
			break;
			}

		SemaphoresLocked = TRUE;

		if (NULL == iwt->iwt_WindowTask.wt_Window)
			break;

		// count number of icons in window
		for (count = 0, in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			count++;
#if 0
		// for lots of updates, a complete window reload will always be faster
		if (iwt->iwt_IconUpdateCount >= count / 2)
			{
			iwt->iwt_IconUpdateCount = 0;

			ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
			ScalosUnLockIconList(iwt);
			ScalosReleaseSemaphore(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->wt_IconSemaphore);
			ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);
			SemaphoresLocked = FALSE;

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

			DateStamp(&dsIconUpdate);
			iwt->iwt_LastIconUpdateTime = dsIconUpdate;		// remember time of last icon update
			iwt->iwt_IconUpdateCount = 0;			// reset icon update counter

			Success = TRUE;
			break;
			}
#endif
		VolumeIsWritable = ClassIsDiskWritable(dirLock);

		d1(KPrintF("%s/%s/%ld: VolumeIsWritable=%ld\n", __FILE__, __FUNC__, __LINE__, VolumeIsWritable));

		if (!InitCudFilesList(&cfl))
			break;

		if (iwt->iwt_WindowTask.wt_LateIconList)
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);

		d1(KPrintF("%s/%s/%ld: after SCCM_IconWin_CleanUp.\n", __FILE__, __FUNC__, __LINE__));

		// add icons from wt_IconList and wt_LateIconList to cfl
		BuildCUDIconList(iwt, &cfl, dirLock);

		d1(KPrintF("%s/%s/%ld: BuildCUDIconList complete.\n", __FILE__, __FUNC__, __LINE__));

		DateStamp(&dsIconUpdate);

		// fill <rilc.rilc_IconScanList> and <rilc.rilc_NonIconScanList> from directory
		sdResult = GetFileList(&rilc, NULL, CurrentPrefs.pref_UseExAll, FALSE, iwt->iwt_CheckOverlappingIcons);

		d1(KPrintF("%s/%s/%ld: GetFileList() complete. sdResult=%ld\n", __FILE__, __FUNC__, __LINE__, sdResult));

		if (SCANDIR_FINISHED != sdResult)
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// build links between icons and objects
		LinkIconScanList(&rilc);

		// add icon files
		for (ise = (struct IconScanEntry *) rilc.rilc_IconScanList.lh_Head;
			ise != (struct IconScanEntry *) &rilc.rilc_IconScanList.lh_Tail;
			ise = (struct IconScanEntry *) ise->ise_Node.ln_Succ)
			{
			if (IsFileHidden(ise->ise_Fib.fib_FileName, ise->ise_Fib.fib_Protection))
				continue;

			// strip ".info" from name
			ise->ise_PosChar = *((char *) ise->ise_Pos);
			*((char *) ise->ise_Pos) = '\0';

			AddToCheckUpdateFilesList(iwt, &cfl, ise);

			d1(KPrintF("%s/%s/%ld: add icon file <%s>  \n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName));
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// add all non-icon files
		for (ise = (struct IconScanEntry *) rilc.rilc_NonIconScanList.lh_Head;
			ise != (struct IconScanEntry *) &rilc.rilc_NonIconScanList.lh_Tail;
			ise = (struct IconScanEntry *) ise->ise_Node.ln_Succ)
			{
			if (IsFileHidden(ise->ise_Fib.fib_FileName, ise->ise_Fib.fib_Protection))
				continue;

			AddToCheckUpdateFilesList(iwt, &cfl, ise);

			d1(KPrintF("%s/%s/%ld: add non-icon file <%s>\n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName));
			}

		// add orphaned icons
		for (ise = (struct IconScanEntry *) rilc.rilc_UnlinkedIconScanList.lh_Head;
			ise != (struct IconScanEntry *) &rilc.rilc_UnlinkedIconScanList.lh_Tail;
			ise = (struct IconScanEntry *) ise->ise_Node.ln_Succ)
			{
			if (IsFileHidden(ise->ise_Fib.fib_FileName, ise->ise_Fib.fib_Protection))
				continue;

			// strip ".info" from name
			ise->ise_PosChar = *((char *) ise->ise_Pos);
			*((char *) ise->ise_Pos) = '\0';

			AddToCheckUpdateFilesList(iwt, &cfl, ise);

			d1(KPrintF("%s/%s/%ld: add orphaned icon <%s>\n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName));
			}

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		// now check for left-out icons from this drawer
		CheckLeftoutIcons(&cfl, dirLock);

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		while ((cud = ((struct CheckUpdateData *) RemHead(&cfl.cfl_FilesList))))
			{
			BOOL fExists = FALSE;

			d1(KPrintF("%s/%s/%ld: cud=%08lx\n", __FILE__, __FUNC__, __LINE__, cud));

			if (iwt->iwt_WindowTask.mt_MainObject && count++ >= CHK_MSG_COUNT)
				{
				// Allow processing of messages
				count = 0;
				if (DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, SCCM_CheckForMessages))
					break;
				}

			if (cud->cud_IconNode)
				{
				d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>  in_Flags=%04lx\n", \
					__FILE__, __FUNC__, __LINE__, cud->cud_IconNode, \
					cud->cud_IconName, cud->cud_IconNode->in_Flags));

				cud->cud_OldIconFlags = cud->cud_IconNode->in_Flags;

				ClassSetDefaultIconFlags(cud->cud_IconNode, NULL == cud->cud_iseIcon);

				if (IsPermanentBackDropIcon(iwt, &rilc.rilc_BackdropList, dirLock, cud->cud_IconName))
					{
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					cud->cud_IconNode->in_SupportFlags |= INF_SupportsPutAway;
					cud->cud_IconNode->in_SupportFlags &= ~INF_SupportsLeaveOut;
					}
				else
					{
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					cud->cud_IconNode->in_SupportFlags &= ~INF_SupportsPutAway;
					cud->cud_IconNode->in_SupportFlags |= INF_SupportsLeaveOut;
					}

				SetIconSupportsFlags(cud->cud_IconNode, VolumeIsWritable);
				}

			d1(KPrintF("%s/%s/%ld: cud_IconNode=%08lx  fExists=%ld\n", __FILE__, __FUNC__, __LINE__, \
				cud->cud_IconNode, fExists));

			if (cud->cud_iseIcon || cud->cud_iseObject)
				{
				if (IsShowAllType(iwt->iwt_OldShowType)
					|| IsThumbnailIcon(cud->cud_IconNode)
					|| (!CurrentPrefs.pref_ShowThumbnailsAsDefault && (THUMBNAILS_AsDefault == iwt->iwt_ThumbnailMode)))
					{
					fExists = TRUE;
					}
				else
					{
					if (cud->cud_iseIcon)
						fExists = TRUE;
					}
				}

			d1(KPrintF("%s/%s/%ld: iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				cud->cud_iseObject, cud->cud_iseIcon));
			d1(KPrintF("%s/%s/%ld: cud_IconNode=%08lx  fExists=%ld\n", __FILE__, __FUNC__, __LINE__, \
				cud->cud_IconNode, fExists));

			if (cud->cud_IconNode && fExists)
				{
				// Both File and Icon found
				struct DateStamp *fDate = NULL;

				d1(KPrintF("%s/%s/%ld: (update) cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

				if (cud->cud_iseIcon && cud->cud_iseObject)
					{
					// use date from object
					fDate = &cud->cud_iseObject->ise_Fib.fib_Date;
					}
				else
					{
					if (cud->cud_iseIcon)
						fDate = &cud->cud_iseIcon->ise_Fib.fib_Date;
					if (cud->cud_iseObject)
						fDate = &cud->cud_iseObject->ise_Fib.fib_Date;
					}


				d1(kprintf("%s/%s/%ld: Window DateStamp Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
						iwt->iwt_LastIconUpdateTime.ds_Days, \
						iwt->iwt_LastIconUpdateTime.ds_Minute, \
						iwt->iwt_LastIconUpdateTime.ds_Tick));
				d1(kprintf("%s/%s/%ld: Icon DateStamp Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_IconNode->in_DateStamp.ds_Days, \
					cud->cud_IconNode->in_DateStamp.ds_Minute, \
					cud->cud_IconNode->in_DateStamp.ds_Tick));
				d1(kprintf("%s/%s/%ld: File DateStamp Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
					fDate->ds_Days, fDate->ds_Minute, fDate->ds_Tick));
				d1(kprintf("%s/%s/%ld: IconFlags=%08lx  OldIconFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_IconNode->in_Flags, cud->cud_OldIconFlags));

				if (((cud->cud_IconNode->in_Flags & INF_DefaultIcon) != (cud->cud_OldIconFlags & INF_DefaultIcon)) ||
					0 != strcmp(cud->cud_IconName, cud->cud_FileName) ||
					CompareDates(&cud->cud_IconNode->in_DateStamp, fDate) > 0 )
					{
					d1(KPrintF("%s/%s/%ld: UPDATE cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

					cud->cud_IconNode = ImmediateUpdateIcon(cud->cud_iwt, cud->cud_IconNode);
					if (cud->cud_IconNode)
						cud->cud_IconNode->in_DateStamp = *fDate;
					}
				}
			else if (cud->cud_IconNode && !fExists)
				{
				// Icon found, but no file
				d1(KPrintF("%s/%s/%ld: REM cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_IconName));

				if (cud->cud_IconNode->in_Lock)
					{
					// remove left-out icon from root window
					DoMethod(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->mt_MainObject,
						SCCM_DeviceWin_RemIcon,
						cud->cud_IconNode);
					}
				else
					{
					DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_RemIcon,
						dirLock, cud->cud_IconName);
					}

				d1(KPrintF("%s/%s/%ld: remove finished.\n", __FILE__, __FUNC__, __LINE__));
				}
			else if (NULL == cud->cud_IconNode && fExists)
				{
				// no Icon found, but file
				d1(KPrintF("%s/%s/%ld: cud=%08lx  iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					cud, cud->cud_iseObject, cud->cud_iseIcon));

				d1(KPrintF("%s/%s/%ld: ADD cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

				cud->cud_IconNode = (struct ScaIconNode *) DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, 
					SCCM_IconWin_ReadIcon,
					cud->cud_FileName, NULL);
				}

			ScalosFree(cud);
			}

		d1(KPrintF("%s/%s/%ld: ws_Viewmodes=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes));

		iwt->iwt_LastIconUpdateTime = dsIconUpdate;		// remember time of last icon update
		iwt->iwt_IconUpdateCount = 0;			// reset icon update counter

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld: before cleanup.\n", __FILE__, __FUNC__, __LINE__));

	CleanupCudFilesList(&cfl);
	RilcCleanup(&rilc);

	if (SemaphoresLocked)
		{
		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
		ScalosUnLockIconList(iwt);
		ScalosReleaseSemaphore(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->wt_IconSemaphore);
		ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);
		}

	iwt->iwt_CheckUpdateCount--;

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return Success;
}
///

/// TextWindowCheckUpdate
ULONG TextWindowCheckUpdate(struct internalScaWindowTask *iwt)
{
	struct CudFilesList cfl;
	struct ReadIconListControl rilc;
	struct IconScanEntry *ise;
	enum ScanDirResult sdResult;
	struct CheckUpdateData *cud;
	struct DateStamp dsIconUpdate;
	BPTR dirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;
	BOOL VolumeIsWritable;
	BOOL SemaphoresLocked = FALSE;
	ULONG Success = FALSE;
	STRPTR fileName;

	d1(KPrintF("\n" "%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		struct ScaIconNode *in;
		ULONG count;

		fileName = AllocPathBuffer();
		if (NULL == fileName)
			break;

		memset(&cfl, 0, sizeof(cfl));
		
		if (!RilcInit(&rilc, iwt))
			break;

		debugLock_d1(dirLock);

		if (++iwt->iwt_CheckUpdateCount > 1)
			{
			d1(KPrintF("%s/%s/%ld: recursive call detected.\n", __FILE__, __FUNC__, __LINE__));
			break;;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (!ScaAttemptSemaphoreList(SCA_SemaphoreExclusiveNoNest, (ULONG) &iwt->iwt_UpdateSemaphore,
			SCA_SemaphoreExclusive, (ULONG) iwt->iwt_WindowTask.wt_IconSemaphore,
			SCA_SemaphoreExclusive, (ULONG) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->wt_IconSemaphore,
			SCA_SemaphoreShared, (ULONG) iwt->iwt_WindowTask.wt_WindowSemaphore,
			TAG_END))
			{
			d1(KPrintF("%s/%s/%ld: AttemptSemaphoreNoNest(&iwt->iwt_UpdateSemaphore) failed.\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ScheduleUpdate);		// reschedule update
			break;;
			}

		SemaphoresLocked = TRUE;

		d1(KPrintF("%s/%s/%ld: UpdateSemaphore=%08lx  NestCount=%ld\n", \
			__FILE__, __FUNC__, __LINE__, &iwt->iwt_UpdateSemaphore, iwt->iwt_UpdateSemaphore.ss_NestCount));

		if (NULL == iwt->iwt_WindowTask.wt_Window)
			break;

		// count number of icons in window
		for (count = 0, in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			count++;
#if 0
		// for lots of updates, a complete window reload will always be faster
		if (iwt->iwt_IconUpdateCount >= count / 2)
			{
			iwt->iwt_IconUpdateCount = 0;

			ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
			ScalosUnLockIconList(iwt);
			ScalosReleaseSemaphore(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->wt_IconSemaphore);
			ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);
			SemaphoresLocked = FALSE;
	
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

			DateStamp(&dsIconUpdate);
			iwt->iwt_LastIconUpdateTime = dsIconUpdate;	// remember time of last icon update
			iwt->iwt_IconUpdateCount = 0;			// reset icon update counter

			Success = TRUE;
			break;
			}
#endif
		VolumeIsWritable = ClassIsDiskWritable(dirLock);

		if (!InitCudFilesList(&cfl))
			break;

		if (iwt->iwt_WindowTask.wt_LateIconList)
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);

		// add icons from wt_IconList and wt_LateIconList to cfl
		BuildCUDIconList(iwt, &cfl, dirLock);

		DateStamp(&dsIconUpdate);

		// fill <rilc.rilc_IconScanList> and <rilc.rilc_NonIconScanList> from directory
		sdResult = GetFileList(&rilc, NULL, CurrentPrefs.pref_UseExAll, FALSE, iwt->iwt_CheckOverlappingIcons);

		d1(KPrintF("%s/%s/%ld: sdResult=%ld\n", __FILE__, __FUNC__, __LINE__, sdResult));

		if (SCANDIR_FINISHED != sdResult)
			break;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));


		// build links between icons and objects
		LinkIconScanList(&rilc);

		// add icon files
		for (ise = (struct IconScanEntry *) rilc.rilc_IconScanList.lh_Head;
			ise != (struct IconScanEntry *) &rilc.rilc_IconScanList.lh_Tail;
			ise = (struct IconScanEntry *) ise->ise_Node.ln_Succ)
			{
			if (IsFileHidden(ise->ise_Fib.fib_FileName, ise->ise_Fib.fib_Protection))
				continue;

			if (!IsShowAllType(iwt->iwt_OldShowType))
				{
				// strip ".info" from name
				ise->ise_PosChar = *((char *) ise->ise_Pos);
				*((char *) ise->ise_Pos) = '\0';
				}

			AddToCheckUpdateFilesList(iwt, &cfl, ise);

			d1(KPrintF("%s/%s/%ld: add icon file <%s>  \n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName));
			}

		// add non-icon files
		for (ise = (struct IconScanEntry *) rilc.rilc_NonIconScanList.lh_Head;
			ise != (struct IconScanEntry *) &rilc.rilc_NonIconScanList.lh_Tail;
			ise = (struct IconScanEntry *) ise->ise_Node.ln_Succ)
			{
			if (IsFileHidden(ise->ise_Fib.fib_FileName, ise->ise_Fib.fib_Protection))
				continue;

			AddToCheckUpdateFilesList(iwt, &cfl, ise);

			d1(KPrintF("%s/%s/%ld: add non-icon file <%s>\n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName));
			}

		// add orphaned icons
		for (ise = (struct IconScanEntry *) rilc.rilc_UnlinkedIconScanList.lh_Head;
			ise != (struct IconScanEntry *) &rilc.rilc_UnlinkedIconScanList.lh_Tail;
			ise = (struct IconScanEntry *) ise->ise_Node.ln_Succ)
			{
			if (IsFileHidden(ise->ise_Fib.fib_FileName, ise->ise_Fib.fib_Protection))
				continue;

			if (!IsShowAllType(iwt->iwt_OldShowType))
				{
				// strip ".info" from name
				ise->ise_PosChar = *((char *) ise->ise_Pos);
				*((char *) ise->ise_Pos) = '\0';
				}

			AddToCheckUpdateFilesList(iwt, &cfl, ise);

			d1(KPrintF("%s/%s/%ld: add orphaned icon <%s>\n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName));
			}

		// now check for left-out icons from this drawer
		CheckLeftoutIcons(&cfl, dirLock);

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		while ((cud = ((struct CheckUpdateData *) RemHead(&cfl.cfl_FilesList))))
			{
			BOOL fChanged;
			BOOL fExists = FALSE;

			d1(KPrintF("%s/%s/%ld: cud=%08lx\n", __FILE__, __FUNC__, __LINE__, cud));

			if (iwt->iwt_WindowTask.mt_MainObject && count++ >= CHK_MSG_COUNT)
				{
				// Allow processing of messages
				count = 0;
				if (DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, SCCM_CheckForMessages))
					break;
				}

			if (cud->cud_IconNode)
				{
				BPTR IconLock;			// +jmc+

				d1(kprintf("%s/%s/%ld: in_Flags=%04lx\n", __FILE__, __FUNC__, __LINE__, cud->cud_IconNode->in_Flags));

				cud->cud_OldIconFlags = cud->cud_IconNode->in_Flags;

				// Check if icon found since last update.
				stccpy(fileName, cud->cud_IconName, MAX_FileName);	// +jmc+
				SafeStrCat(fileName, ".info", MAX_FileName);		// +jmc+
				if ((IconLock = Lock(fileName, SHARED_LOCK)))	// +jmc+
					{
					UnLock(IconLock);				// +jmc+
					d1(kprintf("%s/%s/%ld: Test icon=<%s>\n", __FILE__, __FUNC__, __LINE__, TestIcon));
					ClassSetDefaultIconFlags(cud->cud_IconNode, NULL == cud->cud_iseIcon);	// +jmc+
					}

				if (IsPermanentBackDropIcon(iwt, &rilc.rilc_BackdropList, dirLock, cud->cud_IconName))
					{
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					cud->cud_IconNode->in_SupportFlags |= INF_SupportsPutAway;
					cud->cud_IconNode->in_SupportFlags &= ~INF_SupportsLeaveOut;
					}
				else
					{
					ULONG pos = IsIconName(cud->cud_IconName);

					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					cud->cud_IconNode->in_SupportFlags &= ~INF_SupportsPutAway;

					// never set INF_SupportsLeaveOut for text mode ".info" icons
					if (0 == pos)
						cud->cud_IconNode->in_SupportFlags |= INF_SupportsLeaveOut;
					}

				d1(kprintf("%s/%s/%ld: in=<%s>  supportFlags=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, cud->cud_IconName, cud->cud_IconNode->in_SupportFlags));
				d1(kprintf("%s/%s/%ld: iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_iseObject, cud->cud_iseIcon));

				SetIconSupportsFlags(cud->cud_IconNode, VolumeIsWritable);

				d1(kprintf("%s/%s/%ld: in=<%s>  supportFlags=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, cud->cud_IconName, cud->cud_IconNode->in_SupportFlags));
				}

			d1(kprintf("%s/%s/%ld: cud_IconNode=%08lx  fExists=%ld\n", __FILE__, __FUNC__, __LINE__, \
				cud->cud_IconNode, fExists));

			if (cud->cud_iseIcon || cud->cud_iseObject)
				{
				if (IsShowAllType(iwt->iwt_OldShowType))
					{
					fExists = TRUE;
					}
				else
					{
					if (cud->cud_iseIcon)
						fExists = TRUE;
					}
				}

			d1(kprintf("%s/%s/%ld: SoloIcon=%ld\n", __FILE__, __FUNC__, __LINE__, cud->cud_SoloIcon));

			if (!IsShowAllType(iwt->iwt_OldShowType))
				{
				fChanged = (cud->cud_SoloIcon && (NULL != cud->cud_iseObject))
					|| (!cud->cud_SoloIcon && (NULL == cud->cud_iseObject));
				}
			else
				{
				fChanged = NULL == cud->cud_IconNode;
				}

			d1(kprintf("%s/%s/%ld: iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				cud->cud_iseObject, cud->cud_iseIcon));
			d1(kprintf("%s/%s/%ld: cud_IconNode=%08lx  fExists=%ld\n", __FILE__, __FUNC__, __LINE__, \
				cud->cud_IconNode, fExists));

			if (cud->cud_IconNode && fExists)
				{
				// Both File and Icon found
				const struct DateStamp *fDate = NULL;
				CONST_STRPTR Comment = (CONST_STRPTR) "";
				ULONG Protection;				// +jmc+ 30.01.2005
				ULONG fProt = 0;
				struct DateStamp iseIconDate;

				d1(KPrintF("%s/%s/%ld: (update) cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

				GetAttr(TIDTA_Comment, cud->cud_IconNode->in_Icon, (APTR) &Comment);
				GetAttr(TIDTA_Protection, cud->cud_IconNode->in_Icon, (ULONG *) &Protection); 	// +jmc+ 30.01.2005

				GetAttr(TIDTA_Days, cud->cud_IconNode->in_Icon, (ULONG *) &iseIconDate.ds_Days);
				GetAttr(TIDTA_Mins, cud->cud_IconNode->in_Icon, (ULONG *) &iseIconDate.ds_Minute);
				GetAttr(TIDTA_Ticks, cud->cud_IconNode->in_Icon, (ULONG *) &iseIconDate.ds_Tick);

	 			if (cud->cud_iseIcon && cud->cud_iseObject)
					{
					// use date and protection from object
					fDate = &cud->cud_iseObject->ise_Fib.fib_Date;
					fProt = cud->cud_iseObject->ise_Fib.fib_Protection;
					}
				else
					{
					if (cud->cud_iseIcon)
						{
						fDate = &cud->cud_iseIcon->ise_Fib.fib_Date;
						fProt = cud->cud_iseIcon->ise_Fib.fib_Protection;
						}
					if (cud->cud_iseObject)
						{
						fDate = &cud->cud_iseObject->ise_Fib.fib_Date;
						fProt = cud->cud_iseObject->ise_Fib.fib_Protection;
						}
					}

				d1(KPrintF("%s/%s/%ld: Window DateStamp Days=%ld  Minute=%ld  Tick=%ld\n\n", __FILE__, __FUNC__, __LINE__, \
						iwt->iwt_LastIconUpdateTime.ds_Days, \
						iwt->iwt_LastIconUpdateTime.ds_Minute, \
						iwt->iwt_LastIconUpdateTime.ds_Tick));

				d1(KPrintF("%s/%s/%ld: Icon DateStamp Days=%ld  Minute=%ld  Tick=%ld\n\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_IconNode->in_DateStamp.ds_Days, \
					cud->cud_IconNode->in_DateStamp.ds_Minute, \
					cud->cud_IconNode->in_DateStamp.ds_Tick));

				d1(KPrintF("%s/%s/%ld: File DateStamp  [%s] Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_iseIcon->ise_Fib.fib_FileName, fDate->ds_Days, fDate->ds_Minute, fDate->ds_Tick));
				d1(KPrintF("%s/%s/%ld: Icon DateStamp  [%s] Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_iseIcon->ise_Fib.fib_FileName, iseIconDate.ds_Days, iseIconDate.ds_Minute, iseIconDate.ds_Tick));

				d1(KPrintF("%s/%s/%ld: IconFlags=%08lx  OldIconFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_IconNode->in_Flags, cud->cud_OldIconFlags));
				d1(KPrintF("%s/%s/%ld: Protection=%08lx  fib_Protection=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					Protection, cud->cud_iseIcon->ise_Fib.fib_Protection));
				d1(KPrintF("%s/%s/%ld: Comment=<%s>  fib_Comment=<%s>\n", __FILE__, __FUNC__, __LINE__, \
					Comment, cud->cud_iseIcon->ise_Fib.fib_Comment));
				d1(KPrintF("%s/%s/%ld: cud_IconName=<%s>  cud_FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
					cud->cud_IconName, cud->cud_FileName));

				if (cud->cud_IconNode->in_Lock || 
					((cud->cud_IconNode->in_Flags & INF_DefaultIcon) != (cud->cud_OldIconFlags & INF_DefaultIcon)) ||
					0 != strcmp(cud->cud_IconName, cud->cud_FileName) ||
					0 != strcmp(Comment, cud->cud_iseIcon->ise_Fib.fib_Comment) ||
					0 != CompareDates(&iseIconDate, fDate) ||
					Protection != fProt )
					{
					d1(KPrintF("%s/%s/%ld: UPDATE cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

					cud->cud_IconNode = ImmediateUpdateIcon(cud->cud_iwt, cud->cud_IconNode);
					if (cud->cud_IconNode)
						cud->cud_IconNode->in_DateStamp = *fDate;

					d1(KPrintF("%s/%s/%ld: Icon DateStamp  [%s] Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
						cud->cud_iseIcon->ise_Fib.fib_FileName, iseIconDate.ds_Days, iseIconDate.ds_Minute, iseIconDate.ds_Tick));
					}
				}
			else if (cud->cud_IconNode && !fExists)
				{
				// Icon found, but no file
				d1(KPrintF("%s/%s/%ld: REM cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_IconName));

				if (cud->cud_IconNode->in_Lock)
					{
					// remove left-out icon from root window
					DoMethod(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->mt_MainObject,
						SCCM_DeviceWin_RemIcon,
						cud->cud_IconNode);
					}
				else
					{
					DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_RemIcon,
						dirLock, cud->cud_IconName);
					}
				}
			else if (fChanged)
				{
				if (NULL == cud->cud_iseObject)
					{
					// was icon+object
					// now only icon
					if (NULL != cud->cud_iseIcon)
						{
						d1(KPrintF("%s/%s/%ld: cud=%08lx  iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
							cud, cud->cud_iseObject, cud->cud_iseIcon));

						d1(KPrintF("%s/%s/%ld: ADD cud=%08lx  <%s>\n", \
							__FILE__, __FUNC__, __LINE__, cud, cud->cud_iseIcon->ise_Fib.fib_FileName));

						cud->cud_IconNode = (struct ScaIconNode *) DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, 
							SCCM_IconWin_ReadIcon,
							cud->cud_iseIcon->ise_Fib.fib_FileName, NULL);
						}
					}
				else
					{
					// was only icon
					// now both icon+object
					d1(KPrintF("%s/%s/%ld: cud=%08lx  iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
						cud, cud->cud_iseObject, cud->cud_iseIcon));

					d1(KPrintF("%s/%s/%ld: ADD cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

					cud->cud_IconNode = (struct ScaIconNode *) DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, 
						SCCM_IconWin_ReadIcon,
						cud->cud_FileName, NULL);
					}
				}
			else if (NULL == cud->cud_IconNode && fExists)
				{
				// no Icon found, but file
				d1(KPrintF("%s/%s/%ld: cud=%08lx  iseObject=%08lx  iseIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					cud, cud->cud_iseObject, cud->cud_iseIcon));

				d1(KPrintF("%s/%s/%ld: ADD cud=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_FileName));

				cud->cud_IconNode = (struct ScaIconNode *) DoMethod(cud->cud_iwt->iwt_WindowTask.mt_MainObject, 
					SCCM_IconWin_ReadIcon,
					cud->cud_FileName, NULL);
				}

			ScalosFree(cud);
			}

		d1(KPrintF("%s/%s/%ld: cud=%08lx  ws_Viewmodes=%ld\n", \
			__FILE__, __FUNC__, __LINE__, cud, iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes));

		iwt->iwt_LastIconUpdateTime = dsIconUpdate;		// remember time of last icon update
		iwt->iwt_IconUpdateCount = 0;			// reset icon update counter

		Success = TRUE;
		} while (0);

	CleanupCudFilesList(&cfl);
	RilcCleanup(&rilc);

	if (SemaphoresLocked)
		{
		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
		ScalosUnLockIconList(iwt);
		ScalosReleaseSemaphore(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->wt_IconSemaphore);
		ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);
		}

	if (fileName)
		FreePathBuffer(fileName);

	iwt->iwt_CheckUpdateCount--;

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return Success;
}
///

/// BuildCUDIconList
// add icons from wt_IconList and wt_LateIconList to FilesList
static void BuildCUDIconList(struct internalScaWindowTask *iwt, struct CudFilesList *cfl, BPTR dirLock)
{
	struct ScaIconNode *in;

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct CheckUpdateData *cud = CreateCheckUpdateData(iwt, in);

		if (cud)
			{
			AddTail(&cfl->cfl_FilesList, &cud->cud_Node);

			BTreeInsert(cfl->cfl_FilesTree, cud->cud_IconName, cud);
			}
		}

	// now add left-out icons belonging to this drawer
	if (iwt->iwt_WindowTask.mt_WindowStruct != iInfos.xii_iinfos.ii_MainWindowStruct)
		{
		struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

		for (in=iwtMain->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in->in_Lock && LOCK_SAME == ScaSameLock(in->in_Lock, dirLock))
				{
				struct CheckUpdateData *cud = CreateCheckUpdateData(iwt, in);

				if (cud)
					{
					AddTail(&cfl->cfl_FilesList, &cud->cud_Node);

					BTreeInsert(cfl->cfl_FilesTree, cud->cud_IconName, cud);
					}
				}
			}
		}
}
///

/// DeviceWindowBuildCUDIconList
// add icons from wt_IconList and wt_LateIconList to cfl
static void DeviceWindowBuildCUDIconList(struct internalScaWindowTask *iwt, struct CudFilesList *cfl, BPTR dirLock)
{
	struct ScaIconNode *in;

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (in->in_Icon)
			{
			struct CheckUpdateData *cud = CreateCheckUpdateData(iwt, in);

			if (cud)
				{
				AddTail(&cfl->cfl_FilesList, &cud->cud_Node);

				BTreeInsert(cfl->cfl_FilesTree, cud->cud_IconName, cud);
				}
			}
		}
}
///

/// DeviceWindowBuildFileList
static void DeviceWindowBuildFileList(struct ReadIconListControl *rilc)
{
	STRPTR IconPath = AllocPathBuffer();

	if (IconPath)
		{
		struct internalScaWindowTask *iwt = rilc->rilc_WindowTask;

		if (ScalosAttemptLockIconListShared(iwt))
			{
			struct ScaIconNode *in;

			for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				if (in->in_Lock)
					{
					AddFileToFilesList(rilc, in->in_Lock, GetIconName(in));

					d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

					stccpy(IconPath, GetIconName(in), Max_PathLen - 1);
					SafeStrCat(IconPath, ".info", Max_PathLen - 1);

					AddFileToFilesList(rilc, in->in_Lock, IconPath);
					}
				}

			ScalosUnLockIconList(iwt);
			}

		FreePathBuffer(IconPath);
		}
}
///

/// CheckLeftoutIcons
static void CheckLeftoutIcons(struct CudFilesList *cfl, BPTR dirLock)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

	d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
	debugLock_d1(dirLock);

	if (ScalosAttemptLockIconListShared(iwtMain))
		{
		struct ScaIconNode *in;

		for (in=iwtMain->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in->in_Lock)
				{
				struct CheckUpdateData *cud;

				cud = (struct CheckUpdateData *) BTreeFind(cfl->cfl_FilesTree, GetIconName(in));

				if (cud && LOCK_SAME == ScaSameLock(in->in_Lock, dirLock))
					{
					d1(KPrintF("%s/%s/%ld:  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

					cud->cud_IconNode = in;
					cud->cud_iwt = iwtMain;
					}
				}
			}

		ScalosUnLockIconList(iwtMain);
		}
}
///

/// FindCudByName
static struct CheckUpdateData *FindCudByName(struct CudFilesList *list, const char *Name)
{
	struct CheckUpdateData *cud;

	d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

	cud = (struct CheckUpdateData *) BTreeFind(list->cfl_FilesTree, Name);

	d1(KPrintF("%s/%s/%ld: cud=%08lx\n", __FILE__, __FUNC__, __LINE__, cud, cud->cud_IconNode));
	return cud;
}
///

/// ImmediateUpdateIcon
static struct ScaIconNode *ImmediateUpdateIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *inUpdate)
{
	struct ScaIconNode *inNew = NULL;
	STRPTR AllocIconName;
	BPTR DirLockCopy = BNULL;
	BPTR oldDir = NOT_A_LOCK;

	do	{
		CONST_STRPTR IconName;
		struct internalScaWindowTask *iwtIcon;
		BPTR iconDirLock;
		struct ExtGadget *gg = (struct ExtGadget *) inUpdate->in_Icon;
		struct ScaReadIconArg ria;
		BOOL isBackdrop;

		IconName = GetIconName(inUpdate);

		d1(KPrintF("%s/%s/%ld: IconName=<%s>  in_Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, IconName, inUpdate->in_Lock));

		if (inUpdate->in_Lock)
			{
			isBackdrop = TRUE;
			iconDirLock = inUpdate->in_Lock;
			iwtIcon = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
			RewriteBackdrop(inUpdate);
			}
		else
			{
			isBackdrop = FALSE;
			iwtIcon = iwt;
			iconDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;
			}

		debugLock_d1(iconDirLock);

		// !!! we need copy copy iconDirLock and IconName since the original
		// !!! ones get freed by SCCM_IconWin_RemIcon

		IconName = AllocIconName = AllocCopyString(IconName);
		if (NULL == IconName)
			break;

		DirLockCopy = DupLock(iconDirLock);
		debugLock_d1(DirLockCopy);
		if (BNULL == DirLockCopy)
			break;

		ria.ria_x = gg->LeftEdge;
		ria.ria_y = gg->TopEdge;
		ria.ria_Lock = inUpdate->in_Lock ? DirLockCopy : BNULL;
		ria.ria_IconType = ICONTYPE_NONE;

		oldDir = CurrentDir(DirLockCopy);

		d1(kprintf("%s/%s/%ld: Task=<%s>  oldDir=%08lx\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)->tc_Node.ln_Name, oldDir));

		// remove (old) icon from window
		// !!Take care!!
		// ALL References to inUpdate are invalid after this call !!!
		DoMethod(iwtIcon->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_RemIcon,
			DirLockCopy,
			IconName);

		debugLock_d1(DirLockCopy);

		if (isBackdrop)
			{
			// since in_Lock was not NULL, this icon is already left-out on desktop

			d1(KPrintF("%s/%s/%ld: AddBackdropIcon(%08lx, %s, %ld, %ld) \n", __FILE__, __FUNC__, __LINE__, iconDirLock, IconName, ria.ria_x, ria.ria_y));

			// add icon again to desktop
			inNew = AddBackdropIcon(DirLockCopy, IconName, ria.ria_x, ria.ria_y);
	                }
		else
			{
			d1(KPrintF(__FILE__ "/%s/%ld: DoMethod(%08lx, SCCM_IconWin_ReadIcon, %s, &ria) \n", __FUNC__, __LINE__, iwtIcon->iwt_WindowTask.mt_MainObject, IconName));
			inNew = (struct ScaIconNode *) DoMethod(iwtIcon->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_ReadIcon,
				IconName, &ria);
			}

		d1(KPrintF("%s/%s/%ld: inNew=%08lx\n", __FILE__, __FUNC__, __LINE__, inNew));
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);

	if (AllocIconName)
		FreeCopyString(AllocIconName);
	if (DirLockCopy)
		UnLock(DirLockCopy);

	return inNew;
}
///

/// AddToCheckUpdateFilesList
static void AddToCheckUpdateFilesList(struct internalScaWindowTask *iwt,
	struct CudFilesList *cfl, const struct IconScanEntry *ise)
{
	struct CheckUpdateData *cud;

	cud = FindCudByName(cfl, ise->ise_Fib.fib_FileName);
	if (NULL == cud)
		{
		d1(kprintf("%s/%s/%ld: FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, ise->ise_Fib.fib_FileName));

		cud = CreateCheckUpdateData(iwt, NULL);
		if (cud)
			{
			AddTail(&cfl->cfl_FilesList, &cud->cud_Node);

			BTreeInsert(cfl->cfl_FilesTree, ise->ise_Fib.fib_FileName, cud);
			}
		}
	d1(kprintf("%s/%s/%ld: cud=%08lx\n", __FILE__, __FUNC__, __LINE__, cud));
	if (cud)
		{
		d1(KPrintF("%s/%s/%ld: <%s>  cud=%08lx  ise=%08lx  ise_ObjPtr=%08lx  ise_IconPtr=%08lx  isIcon=%ld\n", \
			__FILE__, __FUNC__, __LINE__, \
			ise->ise_Fib.fib_FileName, cud, ise, ise->ise_ObjPtr, ise->ise_IconPtr, (ise->ise_Flags & ISEFLG_IsIcon)));

		if (!IsIwtViewByIcon(iwt) && IsShowAll(iwt->iwt_WindowTask.mt_WindowStruct))
			{
			cud->cud_iseIcon = (struct IconScanEntry *) ise;
			cud->cud_iseObject = NULL;
			cud->cud_FileName = cud->cud_iseIcon->ise_Fib.fib_FileName;
			}
		else
			{
			if (ise->ise_Flags & ISEFLG_IsIcon)
				{
				cud->cud_iseIcon = (struct IconScanEntry *) ise;
				cud->cud_iseObject = ise->ise_ObjPtr;
				}
			else
				{
				cud->cud_iseObject = (struct IconScanEntry *) ise;
				cud->cud_iseIcon = ise->ise_IconPtr;
				}

			if (NULL == cud->cud_FileName)
				{
				if (cud->cud_iseObject)
					cud->cud_FileName = cud->cud_iseObject->ise_Fib.fib_FileName;
				else
					{
					if (cud->cud_iseIcon)
						cud->cud_FileName = cud->cud_iseIcon->ise_Fib.fib_FileName;
					}
				}
			}

		d1(kprintf("%s/%s/%ld: cud=%08lx  <%s> iseIcon=%08lx  iseObject=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			cud, cud->cud_FileName, cud->cud_iseIcon, cud->cud_iseObject));
		}
}
///

/// DateFromDiskIcon
static BOOL DateFromDiskIcon(struct ScaDeviceIcon *di, struct DateStamp *ds)
{
	BOOL Success = FALSE;
	BPTR fLock =BNULL;
	T_ExamineData *fib = NULL;

	do	{
		if (NULL == di)
			break;
		if (NULL == di->di_DiskIconName)
			break;

		d1(kprintf("%s/%s/%ld: di=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, di, di->di_DiskIconName));

		d1(kprintf("%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));
		if (!ScalosExamineBegin(&fib))
			break;

		fLock = Lock(di->di_DiskIconName, ACCESS_READ);
		d1(kprintf("%s/%s/%ld: fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, fLock));
		if (BNULL == fLock)
			break;

		if (!ScalosExamineLock(fLock, &fib))
			break;

		*ds = *ScalosExamineGetDate(fib);
		Success = TRUE;
		} while (0);

	ScalosExamineEnd(&fib);
	if (BNULL != fLock)
		UnLock(fLock);

	return Success;
}
///

/// IsThumbnailIcon
static BOOL IsThumbnailIcon(struct ScaIconNode *in)
{
	BOOL Thumbnail = FALSE;

	if (in && in->in_Icon)
		{
		ULONG IconUserFlags;

		GetAttr(IDTA_UserFlags, in->in_Icon, &IconUserFlags);
		if (IconUserFlags & ICONOBJ_USERFLAGF_Thumbnail)
			Thumbnail = TRUE;
		}

	return Thumbnail;
}
///

/// InitCudFilesList
static BOOL InitCudFilesList(struct CudFilesList *cfl)
{
	NewList(&cfl->cfl_FilesList);
	cfl->cfl_ListInitialized = TRUE;

	cfl->cfl_FilesTree = BTreeCreate(CudDisposeData, CudDisposeKey, CudCompare);

	return (BOOL) (NULL != cfl->cfl_FilesTree);
}
///

/// CleanupCudFilesList
static void CleanupCudFilesList(struct CudFilesList *cfl)
{
	if (cfl->cfl_ListInitialized)
		{
		struct CheckUpdateData *cud;

		while ((cud = ((struct CheckUpdateData *) RemHead(&cfl->cfl_FilesList))))
			{
			ScalosFree(cud);
			}
		}

	if (cfl->cfl_FilesTree)
		{
		BTreeDispose(cfl->cfl_FilesTree);
		cfl->cfl_FilesTree = NULL;
		}

}
///

/// CudDisposeData
static void CudDisposeData(void *data)
{
	// No-op since data is only handed as reference to CheckUpdateData node
	// and CheckUpdateData nodes are managed within cfl_FilesList
	(void) data;
}
///

/// CudDisposeKey
static void CudDisposeKey(void *key)
{
	// No-op since only non-volatile data is handed as key
	(void) key;
}
///

/// CudCompare
static int CudCompare(const void *key1, const void *key2)
{
	return Stricmp((CONST_STRPTR) key2, (CONST_STRPTR) key1);
}
///

/// RealUpdateIcon
void RealUpdateIcon(struct internalScaWindowTask *iwt, struct UpdateIconData *arg)
{
	struct ScaReadIconArg ria;
	BOOL IconSemaLocked;

	d1(kprintf("%s/%s/%ld: arg=%08lx\n", __FILE__, __FUNC__, __LINE__, arg));
	d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, arg->uid_WBArg.wa_Name));
	d1(kprintf("%s/%s/%ld: IconType=%ld\n", __FILE__, __FUNC__, __LINE__, arg->uid_IconType));
	debugLock_d1(arg->uid_WBArg.wa_Lock);

	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
		{
		iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
		return;
		}

	if (!AttemptSemaphoreNoNest(&iwt->iwt_UpdateSemaphore))
		{
		d1(kprintf("%s/%s/%ld: iwt_UpdateSemaphore not available\n", __FILE__, __FUNC__, __LINE__));
		return;
		}

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		{
		struct ScaIconNode *in;
		BOOL iconFound = FALSE;

		d1(KPrintF("%s/%s/%ld: Drawer Window\n", __FILE__, __FUNC__, __LINE__));

		ria.ria_x = ria.ria_y = NO_ICON_POSITION_SHORT;
		ria.ria_Lock = BNULL;
		ria.ria_IconType = arg->uid_IconType;

		ScalosLockIconListShared(iwt);

		for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (0 == Stricmp(arg->uid_WBArg.wa_Name, (STRPTR) GetIconName(in)))
				{
				struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

				d1(kprintf("%s/%s/%ld: FOUND in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

				ria.ria_x = gg->LeftEdge;
				ria.ria_y = gg->TopEdge;

				iconFound = TRUE;
				break;
				}
			}

		ScalosUnLockIconList(iwt);
		IconSemaLocked = FALSE;

		d1(KPrintF("%s/%s/%ld: iconFound=%ld\n", __FILE__, __FUNC__, __LINE__, iconFound));

		// if icon is present in window, remove it!
		if (iconFound)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_RemIcon,
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				arg->uid_WBArg.wa_Name);
			}
		// Try to read icon
		(void) DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_ReadIcon,
			arg->uid_WBArg.wa_Name,
			&ria);
		}
	else
		{
		// Desktop window
		// .nolock:
		struct ScaIconNode *iconList = NULL;
		struct ScaIconNode *in, *inNext;

		d1(kprintf("%s/%s/%ld: Desktop Window\n", __FILE__, __FUNC__, __LINE__));

		ScalosLockIconListExclusive(iwt);
		IconSemaLocked = TRUE;

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in=inNext)
			{
			inNext =  (struct ScaIconNode *) in->in_Node.mln_Succ;

			d1(kprintf("%s/%s/%ld: in=%08lx <%s>  DeviceIcon=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, in, GetIconName(in), in->in_DeviceIcon));

			if (in->in_DeviceIcon)
				{
				// This is a device icon
				BPTR diskLock;

				if (arg->uid_WBArg.wa_Name && 0 != Stricmp((STRPTR) arg->uid_WBArg.wa_Name, "disk"))
					continue;

				diskLock = DiskInfoLock(in);
				if (diskLock)
					{
					struct ScaBackdropIcon *bdi;
					struct ScaIconNode *iny;
					LONG SameLockResult = ScaSameLock(diskLock, arg->uid_WBArg.wa_Lock);

					d1(kprintf("%s/%s/%ld: SameLock()=%ld\n", __FILE__, __FUNC__, __LINE__, SameLockResult));

					UnLock(diskLock);

					if (LOCK_SAME != SameLockResult)
						continue;

					SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &iconList, in);

					for (bdi=in->in_IconList; bdi; bdi = (struct ScaBackdropIcon *) bdi->sbi_Node.mln_Succ)
						{
						struct ScaIconNode *inyNext;

						for (iny=iwt->iwt_WindowTask.wt_IconList; iny; iny=inyNext)
							{
							inyNext = (struct ScaIconNode *) iny->in_Node.mln_Succ;

							if (bdi->sbi_Icon == iny->in_Icon)
								{
								d1(kprintf("%s/%s/%ld: iny=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iny, GetIconName(iny)));

								SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &iconList, iny);
								break;
								}
							}
						}

					d1(kprintf("%s/%s/%ld: iconList=%08lx\n", __FILE__, __FUNC__, __LINE__, iconList));

					RemoveIcons(iwt, &iconList);
					FreeIconList(iwt, &iconList);

					break;		// leave for ()
					}

				ScalosUnLockIconList(iwt);
				IconSemaLocked = FALSE;

				IDCMPDiskInserted(iwt, NULL);
				}
			else if (in->in_Lock)
				{
				// .nodiskinfo:
				// this is a backdrop (left-out) icon

				d1(kprintf("%s/%s/%ld: non-disk icon\n", __FILE__, __FUNC__, __LINE__));
				debugLock_d1(in->in_Lock);

				if (LOCK_SAME == ScaSameLock(arg->uid_WBArg.wa_Lock, in->in_Lock)
						&& 0 == Stricmp(arg->uid_WBArg.wa_Name, (STRPTR) GetIconName(in)))
					{
					struct ScaIconNode *iconList = NULL;
					struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
					BPTR oldDir;

					SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &iconList, in);

					RemoveIcons(iwt, &iconList);

					debugLock_d1(in->in_Lock);
					RewriteBackdrop(in);

					ria.ria_x = gg->LeftEdge;
					ria.ria_y = gg->TopEdge;
					ria.ria_Lock = arg->uid_WBArg.wa_Lock;
					ria.ria_IconType = arg->uid_IconType;

					// (frees backdropIcon!)
					FreeIconNode(iwt, &iconList, in);

					debugLock_d1(ria.ria_Lock);

					ScalosUnLockIconList(iwt);
					IconSemaLocked = FALSE;

					oldDir = CurrentDir(arg->uid_WBArg.wa_Lock);
					debugLock_d1(oldDir);

					// (generates new backdropIcon)
					(void) DoMethod(iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_ReadIcon,
						arg->uid_WBArg.wa_Name, &ria);

					debugLock_d1(oldDir);
					CurrentDir(oldDir);

					RefreshTextWindow(iwt);
					break;
					}
				}
			}
		}

	// Remember date&time of last icon update
	DateStamp(&iwt->iwt_LastIconUpdateTime);

	if (IconSemaLocked)
		ScalosUnLockIconList(iwt);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
		SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);

	ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);

	if (arg->uid_WBArg.wa_Lock)
		UnLock(arg->uid_WBArg.wa_Lock);
	if (arg->uid_WBArg.wa_Name)
		FreeCopyString((STRPTR) arg->uid_WBArg.wa_Name);
}
///

/// RefreshTextWindow
static void RefreshTextWindow(struct internalScaWindowTask *iwt)
{
	if (!IsIwtViewByIcon(iwt))
		{
		// Refresh text window
		d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

		ScalosLockIconListExclusive(iwt);

		if (iwt->iwt_WindowTask.wt_Window)
			{
			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
				0, 0,
				iwt->iwt_WindowTask.wt_Window->Width - 1,
				iwt->iwt_WindowTask.wt_Window->Height - 1);
			}

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_ReAdjust);

		ScalosUnLockIconList(iwt);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}
}
///

/// CreateCheckUpdateData
static struct CheckUpdateData *CreateCheckUpdateData(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct CheckUpdateData *cud = ScalosAlloc(sizeof(struct CheckUpdateData));

	if (cud)
		{
		ULONG SoloIcon = FALSE;

		cud->cud_iwt = iwt;
		cud->cud_iseIcon = NULL;
		cud->cud_iseObject = NULL;
		cud->cud_IconNode = in;
		cud->cud_FileName = NULL;
		cud->cud_IconName = GetIconName(in);

		if (in)
			GetAttr(TIDTA_SoloIcon, in->in_Icon, &SoloIcon);

		cud->cud_SoloIcon = SoloIcon;
		}

	return cud;
}
///

