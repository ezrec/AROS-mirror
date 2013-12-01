// Thumbnails.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <graphics/scale.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <libraries/mcpgfx.h>
#include <scalos/scalosgfx.h>
#include <scalos/scalospreviewplugin.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/datatypes.h>
#include <proto/intuition.h>
#include <proto/timer.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalospreviewplugin.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//-----------------------------------------------------------------------

// local data definitions

#define	DEFAULT_PVPLUGIN_NAME   "Scalos:Plugins/Preview/defpicture.pvplugin"

struct SM_StartProg
	{
	struct internalScaWindowTask *spr_WindowTask;
	};

struct ThumbnailIcon
	{
	struct Node tni_Node;
	Object *tni_IconObj;
	BPTR tni_DirLock;
	STRPTR tni_IconName;
	ULONG tni_Flags;
	APTR tni_UndoStep;
	};

//-----------------------------------------------------------------------

// Local data items :

SCALOSSEMAPHORE	ThumbnailsCleanupSemaphore;	 // Semaphore to ensure only a single cleanup process is running

//-----------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) GenerateThumbnailProcess(struct SM_StartProg *sMsg,
	struct SM_RunProcess *msg);
static BOOL GenerateThumbnail(struct internalScaWindowTask *iwt,
	struct ThumbnailIcon *tni, APTR ThumbnailCacheHandle);
static struct ThumbnailIcon *AddThumbnailEntry(struct internalScaWindowTask *iwt, 
	Object *IconObj, BPTR DirLock, CONST_STRPTR Name, ULONG Flags, APTR UndoStep);
static void FreeThumbnailEntry(struct ThumbnailIcon *tni);
static struct ScaIconNode *FindIcon(struct internalScaWindowTask *iwt, const Object *IconObject);
static BOOL IsIconObjectValid(struct internalScaWindowTask *iwt, const Object *IconObject);
static BOOL GenerateThumbnailFromARGB(struct internalScaWindowTask *iwt,
	APTR ThumbnailCacheHandle, Object *IconObj,
        struct ARGBHeader *argbDest,
	BPTR fLock, BOOL MayAddToCache);
static BOOL GenerateThumbnailFromSac(struct internalScaWindowTask *iwt,
	Object *IconObj, struct ScalosBitMapAndColor **sac);
static SAVEDS(ULONG) ThumbnailCleanupCacheProcess(struct SM_StartProg *sMsg, struct SM_RunProcess *msg);
static BOOL ThumbnailShouldBeCached(const struct BitMapHeader *bmhd);

//-----------------------------------------------------------------------

/// AddThumbnailIcon
BOOL AddThumbnailIcon(struct internalScaWindowTask *iwt, Object *IconObj,
	BPTR DirLock, CONST_STRPTR Name, ULONG Flags, APTR UndoStep)
{
	BOOL Success = FALSE;

	do	{
		struct ExtGadget *gg = (struct ExtGadget *) IconObj;
		ULONG ThumbnailWidth = CurrentPrefs.pref_ThumbNailWidth;
		ULONG ThumbnailHeight = CurrentPrefs.pref_ThumbNailHeight;
		struct ThumbnailIcon *tni;
		struct FileTypeDef *ftd;
		ULONG IconUserFlags;
		ULONG IconType;

		debugLock_d1(DirLock);

		if (BNULL == DirLock)
			break;

		if (NULL == iwt->iwt_WindowTask.mt_MainObject)
			break;

		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_DdPopupWindow)
			break;

		if (ThumbnailWidth < CurrentPrefs.pref_IconSizeConstraints.MinX)
			ThumbnailWidth = CurrentPrefs.pref_IconSizeConstraints.MinX;
		else if (ThumbnailWidth > CurrentPrefs.pref_IconSizeConstraints.MaxX)
			ThumbnailWidth = CurrentPrefs.pref_IconSizeConstraints.MaxX;

		if (ThumbnailHeight < CurrentPrefs.pref_IconSizeConstraints.MinY)
			ThumbnailHeight = CurrentPrefs.pref_IconSizeConstraints.MinY;
		else if (ThumbnailHeight > CurrentPrefs.pref_IconSizeConstraints.MaxY)
			ThumbnailHeight = CurrentPrefs.pref_IconSizeConstraints.MaxY;

		GetAttr(IDTA_Type, IconObj, &IconType);
		d1(KPrintF("%s/%s/%ld:  IconType=%08lx\n", __FILE__, __FUNC__, __LINE__, IconType));
		if (WBPROJECT != IconType)
			break;

		TIMESTAMPCOUNT_START_d1(iwt, 13);
		ftd = FindFileTypeForTypeNode(iwt, DefIconsIdentify(DirLock, Name, ICONTYPE_NONE));
		d1(KPrintF("%s/%s/%ld:  ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));
		TIMESTAMPCOUNT_END_d1(iwt, 13);
		if (NULL == ftd)
			break;

		d1(KPrintF("%s/%s/%ld:  ftd_PreviewPluginBase=%08lx  ftd_PreviewPluginName=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, ftd->ftd_PreviewPluginBase, ftd->ftd_PreviewPluginName));

		if (NULL == ftd->ftd_PreviewPluginBase && NULL == ftd->ftd_PreviewPluginName)
			break;

		TIMESTAMPCOUNT_START_d1(iwt, 14);
		tni = AddThumbnailEntry(iwt, IconObj, DirLock, Name, Flags, UndoStep);
		TIMESTAMPCOUNT_END_d1(iwt, 14);
		if (NULL == tni)
			break;

		Success = TRUE;

		d1(KPrintF("%s/%s/%ld:  ThumbnailHeight=%ld  gg->Height=%ld  ThumbnailWidth=%ld gg->Width=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ThumbnailHeight, gg->Height, ThumbnailWidth, gg->Width));

		if (ThumbnailHeight > gg->Height || ThumbnailWidth > gg->Width)
			{
			ULONG LeftBorder, TopBorder;
			ULONG RightBorder, BottomBorder;

			TopBorder = (ThumbnailHeight - gg->Height) / 2;
			BottomBorder = ThumbnailHeight - gg->Height - TopBorder;

			LeftBorder = (ThumbnailWidth - gg->Width) / 2;
			RightBorder = ThumbnailWidth - gg->Width - LeftBorder;

			SetAttrs(IconObj,
				IDTA_FrameType, 	(0 == CurrentPrefs.pref_ThumbnailImageBorders.Top || 0 == CurrentPrefs.pref_ThumbnailImageBorders.Left)
								? MF_FRAME_NONE : CurrentPrefs.pref_ThumbnailFrameType,
				IDTA_FrameTypeSel, 	(0 == CurrentPrefs.pref_ThumbnailImageBorders.Top || 0 == CurrentPrefs.pref_ThumbnailImageBorders.Left)
								? MF_FRAME_NONE : CurrentPrefs.pref_ThumbnailFrameTypeSel,
				IDTA_InnerTop, 		CurrentPrefs.pref_ThumbnailImageBorders.Top + TopBorder,
				IDTA_InnerLeft, 	CurrentPrefs.pref_ThumbnailImageBorders.Left + LeftBorder,
				IDTA_InnerBottom, 	CurrentPrefs.pref_ThumbnailImageBorders.Bottom + BottomBorder,
				IDTA_InnerRight, 	CurrentPrefs.pref_ThumbnailImageBorders.Right + RightBorder,
				TAG_END);
			}

		// Mark icon as thumbnail. This flag is cleared if thumbnail generation fails.
		GetAttr(IDTA_UserFlags, IconObj, &IconUserFlags);
		IconUserFlags |= ICONOBJ_USERFLAGF_Thumbnail;
		SetAttrs(IconObj,
			IDTA_UserFlags, IconUserFlags,
			TAG_END);
		} while (0);

	d1(KPrintF("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
///

/// GenerateThumbnails
BOOL GenerateThumbnails(struct internalScaWindowTask *iwt)
{
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	if (iwt->iwt_WindowTask.mt_MainObject)
		{
		struct SM_StartProg sMsg;

		sMsg.spr_WindowTask = iwt;

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		if (!iwt->iwt_CloseWindow && !iwt->iwt_AbortScanDir)
			{
			Success = DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_RunProcess, GenerateThumbnailProcess,
				&sMsg, sizeof(sMsg), iwt->iwt_WindowTask.wt_IconPort);
			}
		}

	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	return Success;
}
///

/// RearrangeThumbnailList
void RearrangeThumbnailList(struct internalScaWindowTask *iwt)
{
	struct ThumbnailIcon *tni;
	struct List TempThumbnailList;

	NewList(&TempThumbnailList);

	ScalosObtainSemaphore(&iwt->iwt_ThumbnailIconSemaphore);

	// Move all nodes to temporary list
	while ((tni = (struct ThumbnailIcon *) RemHead(&iwt->iwt_ThumbnailIconList)))
		{
		AddTail(&TempThumbnailList, &tni->tni_Node);
		}

	// now fetch all nodes from Temp list, and put them back to regular list
	// add all visible icons to top of list
	while ((tni = (struct ThumbnailIcon *) RemHead(&TempThumbnailList)))
		{
		// Add visible icons at head of list, so they are thumbnailed first
		if (IsIconObjVisible(iwt, tni->tni_IconObj))
			AddHead(&iwt->iwt_ThumbnailIconList, &tni->tni_Node);
		else
			AddTail(&iwt->iwt_ThumbnailIconList, &tni->tni_Node);
		}

	ScalosReleaseSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
}
///

/// GenerateThumbnailProcess
static SAVEDS(ULONG) GenerateThumbnailProcess(struct SM_StartProg *sMsg, struct SM_RunProcess *msg)
{
	struct internalScaWindowTask *iwt = sMsg->spr_WindowTask;
	struct Process *myProc = (struct Process *) FindTask(NULL);
	char TaskName[256];

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);

	ScaFormatStringMaxLength(TaskName, sizeof(TaskName),
		"Scalos_CreateThumbnails <%s>", (ULONG) iwt->iwt_WindowTask.mt_WindowStruct->ws_Name);

	myProc->pr_Task.tc_Node.ln_Name = TaskName;
	SetProgramName(TaskName);

	if (ScalosAttemptSemaphore(&iwt->iwt_ThumbGenerateSemaphore))
		{
		struct ThumbnailIcon *tni;
		APTR ThumbnailCacheHandle = NULL;	// Every instance of GenerateThumbnailProcess() has its own cache handle
		APTR WorkThumbnailCacheHandle;

		SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
			SCCA_IconWin_ThumbnailsGenerating, TRUE,
			TAG_END);

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		// if ThumbnailCacheOpen() returns FALSE (failure),
		// ThumbnailCacheHandle can be valid nevertheless, and
		// must be closed to release allocated resource.
		// WorkThumbnailCacheHandle is set to NULL in that case,
		// so we never try to USE the thumbnail cache if open failed.
		if (ThumbnailCacheOpen(&ThumbnailCacheHandle))
			WorkThumbnailCacheHandle = ThumbnailCacheHandle;
		else
			WorkThumbnailCacheHandle = NULL;

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		ThumbnailCacheBegin(ThumbnailCacheHandle);

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		while (!IsListEmpty(&iwt->iwt_ThumbnailIconList))
			{
			ScalosObtainSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
			tni = (struct ThumbnailIcon *) RemHead(&iwt->iwt_ThumbnailIconList);
			ScalosReleaseSemaphore(&iwt->iwt_ThumbnailIconSemaphore);

			if (NULL == tni)
				break;

			if (IsViewByIcon(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes))
				{
				SetTaskPri(FindTask(NULL), -1);

				(void) GenerateThumbnail(iwt, tni, WorkThumbnailCacheHandle);

				d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

				// Icon needs to be updated in any case
				// either to display the newly generated thumbnail
				// or to restore the original icon borders

				if (IsViewByIcon(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes))
					{
					struct SM_RedrawIconObj *smrio;

					smrio = (struct SM_RedrawIconObj *) SCA_AllocMessage(MTYP_RedrawIconObj, 0);
					d1(KPrintF("%s/%s/%ld: smrio=%08lx\n", __FILE__, __FUNC__, __LINE__, smrio));
					if (smrio)
						{
						smrio->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
						smrio->smrio_IconObject = tni->tni_IconObj;
						smrio->smrio_Flags = SMRIOFLAGF_EraseIcon | SMRIOFLAGF_FreeLayout;
						PutMsg(iwt->iwt_WindowTask.wt_IconPort, &smrio->ScalosMessage.sm_Message);
						}
					}
				}

			FreeThumbnailEntry(tni);
			}

		SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
			SCCA_IconWin_ThumbnailsGenerating, FALSE,
			TAG_END);

		ScalosReleaseSemaphore(&iwt->iwt_ThumbGenerateSemaphore);

		ThumbnailCacheFinish(ThumbnailCacheHandle);
		ThumbnailCacheClose(&ThumbnailCacheHandle);

		ThumbnailsStartCleanup(FALSE);
		}

	// Semaphore has been locked in GenerateThumbnails()
	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);

	if (iwt->iwt_IconPortOutstanding > 0)
		--iwt->iwt_IconPortOutstanding;

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}
///

/// GenerateThumbnail
static BOOL GenerateThumbnail(struct internalScaWindowTask *iwt,
	struct ThumbnailIcon *tni, APTR ThumbnailCacheHandle)
{
	BOOL Success = FALSE;
	BPTR fLock = BNULL;
	BPTR OldDir = NOT_A_LOCK;
	ULONG ScreenDepth;
	struct Library *OpenedPluginBase = NULL;
#ifdef __amigaos4__
	struct Interface *OpenedIFace = NULL;
#endif
	struct ScalosPreviewResult PVResult;
	BOOL IconObjValid = FALSE;

	memset(&PVResult, 0, sizeof(PVResult));

	ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	do	{
		BOOL DoRemap;
		ULONG SupportedColors = 256;
		ULONG ThumbnailWidth = CurrentPrefs.pref_ThumbNailWidth;
		ULONG ThumbnailHeight = CurrentPrefs.pref_ThumbNailHeight;
		BOOL WasCached = FALSE;

		if (BNULL == tni->tni_DirLock)
			break;

		// make sure the specified icon is still present
		if (IsIconObjectValid(iwt, tni->tni_IconObj))
			ScalosUnLockIconList(iwt);
		else
			break;	// Icon seems to have vanished

		GetAttr(IDTA_NumberOfColorsSupported, tni->tni_IconObj, &SupportedColors);
		d1(KPrintF("%s/%s/%ld:  SupportedColors=%lu\n", __FILE__, __FUNC__, __LINE__, SupportedColors));

		if (ThumbnailWidth < CurrentPrefs.pref_IconSizeConstraints.MinX)
			ThumbnailWidth = CurrentPrefs.pref_IconSizeConstraints.MinX;
		else if (ThumbnailWidth > CurrentPrefs.pref_IconSizeConstraints.MaxX)
			ThumbnailWidth = CurrentPrefs.pref_IconSizeConstraints.MaxX;

		if (ThumbnailHeight < CurrentPrefs.pref_IconSizeConstraints.MinY)
			ThumbnailHeight = CurrentPrefs.pref_IconSizeConstraints.MinY;
		else if (ThumbnailHeight > CurrentPrefs.pref_IconSizeConstraints.MaxY)
			ThumbnailHeight = CurrentPrefs.pref_IconSizeConstraints.MaxY;

		OldDir = CurrentDir(tni->tni_DirLock);

		fLock = Lock(tni->tni_IconName, ACCESS_READ);
		d1(KPrintF("%s/%s/%ld:  Name=<%s>  fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, tni->tni_IconName, fLock));
		if (BNULL == fLock)
			break;

		DoRemap = (ScreenDepth <= 8) || (NULL == CyberGfxBase) || SupportedColors <= 256;

		if (!IsIconObjectValid(iwt, tni->tni_IconObj))
			break;	// IconObject is no longer valid

		IconObjValid = TRUE;

		d1(KPrintF("%s/%s/%ld:  DoRemap=%ld\n", __FILE__, __FUNC__, __LINE__, DoRemap));

		// Check whether we have a cached thumbnail for this icon
		if (DoRemap)
			{
			struct ScalosBitMapAndColor *sac;

			sac = ThumbnailCacheFindRemapped(fLock, ThumbnailCacheHandle,
				iwt->iwt_ThumbnailsLifetimeDays);
			if (sac)
				{
				d1(KPrintF("%s/%s/%ld:  sac_Width=%ld  sac_Height=%ld\n", \
					__FILE__, __FUNC__, __LINE__, sac->sac_Width, sac->sac_Height));

				WasCached = Success = GenerateThumbnailFromSac(iwt,
					tni->tni_IconObj, &sac);
				}

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			ScalosGfxFreeSAC(sac);
			}
		else
			{
			struct ARGBHeader argbCached;

			memset(&argbCached, 0, sizeof(argbCached));

			if (ThumbnailCacheFindARGB(fLock, ThumbnailCacheHandle, &argbCached,
				iwt->iwt_ThumbnailsLifetimeDays))
				{
				d1(KPrintF("%s/%s/%ld:  argb_Width=%ld  argb_Height=%ld\n", \
					__FILE__, __FUNC__, __LINE__, argbCached.argb_Width, argbCached.argb_Height));

				WasCached = Success = GenerateThumbnailFromARGB(iwt,
					ThumbnailCacheHandle,
					tni->tni_IconObj, 
					&argbCached, 
					fLock, 
					FALSE);
				}
			}

		d1(KPrintF("%s/%s/%ld:  WasCached=%ld\n", __FILE__, __FUNC__, __LINE__, WasCached));
		if (!WasCached)
			{
			struct BitMapHeader bmhd;
			struct FileTypeDef *ftd = NULL;
			struct ScaIconNode *in;
			struct Library *ScalosPreviewPluginBase = NULL;
#ifdef __amigaos4__
			struct ScalosPreviewPluginIFace *IScalosPreviewPlugin;
#endif

			in = FindIcon(iwt, tni->tni_IconObj);
			d1(KPrintF("%s/%s/%ld:  in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));
			if (in)
				ftd = FindFileType(iwt, in);

			// Thumbnail not found in cache - unlock icon list
			ScalosUnLockIconList(iwt);
			IconObjValid = FALSE;

			d1(KPrintF("%s/%s/%ld:  ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));
			if (ftd)
				{
				d1(KPrintF("%s/%s/%ld:  ftd_Name=<%s> ftd_PreviewPluginBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_Name, ftd->ftd_PreviewPluginBase));
				d1(KPrintF("%s/%s/%ld:  ftd_PreviewPluginName=<%s>\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_PreviewPluginName ? ftd->ftd_PreviewPluginName : (STRPTR) ""));

				if (ftd->ftd_PreviewPluginBase)
					{
					ScalosPreviewPluginBase	= ftd->ftd_PreviewPluginBase;
#ifdef __amigaos4__
					IScalosPreviewPlugin = ftd->ftd_IPreviewPlugin;
#endif
					}
				else if (ftd->ftd_PreviewPluginName)
					{
					ScalosPreviewPluginBase = ftd->ftd_PreviewPluginBase = OpenLibrary(ftd->ftd_PreviewPluginName, 41);
#ifdef __amigaos4__
					IScalosPreviewPlugin = ftd->ftd_IPreviewPlugin = (struct ScalosPreviewPluginIFace *)GetInterface(ScalosPreviewPluginBase, "main", 1, NULL);
#endif
					}

				d1(KPrintF("%s/%s/%ld:  ftd_PreviewPluginBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_PreviewPluginBase));
				}

			d1(KPrintF("%s/%s/%ld:  ScalosPreviewPluginBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosPreviewPluginBase));

			// fall back to default picture plugin
			if (NULL == ScalosPreviewPluginBase)
				{
				ScalosPreviewPluginBase = OpenedPluginBase = OpenLibrary(DEFAULT_PVPLUGIN_NAME, 41);
#ifdef __amigaos4__
				OpenedIFace = GetInterface((struct Library *)ScalosPreviewPluginBase, "main", 1, NULL);
				IScalosPreviewPlugin = (struct ScalosPreviewPluginIFace *)OpenedIFace;
#endif
				}

			d1(KPrintF("%s/%s/%ld:  ScalosPreviewPluginBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosPreviewPluginBase));
			if (NULL == ScalosPreviewPluginBase)
				break;	// no plugin available

			// Generate thumbnail via preview plugin
			Success = SCAPreviewGenerateTags(&iwt->iwt_WindowTask,
				tni->tni_DirLock,
				tni->tni_IconName,
				SCALOSPREVIEW_ThumbnailWidth, ThumbnailWidth,
				SCALOSPREVIEW_ThumbnailHeight, ThumbnailHeight,
				SCALOSPREVIEW_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				SCALOSPREVIEW_SupportedColors, SupportedColors,
				SCALOSPREVIEW_ResultStruct, (ULONG) &PVResult,
				SCALOSPREVIEW_ImgBitMapHeader, (ULONG) &bmhd,
				SCALOSPREVIEW_Quality, CurrentPrefs.pref_ThumbnailQuality,
				SCALOSPREVIEW_ReservedColors, 2,
				TAG_END);
			d1(KPrintF("%s/%s/%ld:  SCAPreviewGenerateTags Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
			if (!Success)
				break;	// Thumbnail generation failed

			d1(KPrintF("%s/%s/%ld:  Image Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, bmhd.bmh_Width, bmhd.bmh_Height));
			d1(KPrintF("%s/%s/%ld:  sac=%08lx  argb=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, PVResult.spvr_sac, PVResult.spvr_ARGBheader.argb_ImageData));

			if (!IsIconObjectValid(iwt, tni->tni_IconObj))
				{
				// IconObject is no longer valid
				Success = FALSE;
				break;
				}
			IconObjValid = TRUE;

			if (DoRemap)
				{
				d1(KPrintF("%s/%s/%ld: DoRemap\n", __FILE__, __FUNC__, __LINE__));
				if (NULL == PVResult.spvr_sac)
					{
					// Thumbnail generation failed
					Success = FALSE;
					}
				else
					{
					d1(KPrintF("%s/%s/%ld:  sac_ColorTable=%08lx  %08lx %08lx %08lx\n", \
						__FILE__, __FUNC__, __LINE__, PVResult.spvr_sac->sac_ColorTable, \
						PVResult.spvr_sac->sac_ColorTable[0], \
						PVResult.spvr_sac->sac_ColorTable[1], \
						PVResult.spvr_sac->sac_ColorTable[2]));
					d1(KPrintF("%s/%s/%ld:  sac_BitMap=%08lx  sac_NumColors=%lu\n", \
						__FILE__, __FUNC__, __LINE__, PVResult.spvr_sac->sac_NumColors, PVResult.spvr_sac->sac_BitMap));

					// Try to add icon thumbnail to cache
					ThumbnailCacheAddRemapped(fLock, ThumbnailCacheHandle,
						PVResult.spvr_sac, iwt->iwt_ThumbnailsLifetimeDays);

					Success = GenerateThumbnailFromSac(iwt,
							tni->tni_IconObj,
							&PVResult.spvr_sac);
					d1(KPrintF("%s/%s/%ld:  GenerateThumbnailFromSac Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
					}
				}
			else
				{
				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				if (NULL == PVResult.spvr_ARGBheader.argb_ImageData)
					{
					// Thumbnail generation failed
					Success = FALSE;
					}
				else
					{
					Success = GenerateThumbnailFromARGB(iwt,
						ThumbnailCacheHandle,
						tni->tni_IconObj,
		                                &PVResult.spvr_ARGBheader,
						fLock,
						ThumbnailShouldBeCached(&bmhd));
					d1(KPrintF("%s/%s/%ld:  GenerateThumbnailFromARGB Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
					}
				}
			if (!Success)
				break;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		AddIconOverlay(tni->tni_IconObj, ICONOVERLAYF_Thumbnail);

		if (tni->tni_Flags & THUMBNAILICONF_SAVEICON)
			{
			struct TagItem *TagList = NULL;
			Object *OldIconObj = LoadIconObject(tni->tni_DirLock, tni->tni_IconName, NULL);

			if (tni->tni_Flags & THUMBNAILICONF_NOICONPOSITION)
				{
				static const struct TagItem NoPosTagList[] =
					{
					{ ICONA_NoPosition, TRUE },
					{ TAG_END, 0 }
					};

				TagList = (struct TagItem *) NoPosTagList;
				}

			SaveIconObject(tni->tni_IconObj,
				tni->tni_DirLock, tni->tni_IconName,
				TRUE,
				TagList);

			UndoAddEvent(iwt, UNDO_ChangeIconObject,
				UNDOTAG_UndoMultiStep, tni->tni_UndoStep,
				UNDOTAG_IconDirLock, tni->tni_DirLock,
				UNDOTAG_IconName, tni->tni_IconName,
				UNDOTAG_OldIconObject, OldIconObj,
				UNDOTAG_NewIconObject, tni->tni_IconObj,
				TAG_END);

			if (OldIconObj)
				DisposeIconObject(OldIconObj);
			}
		} while (0);

	d1(KPrintF("%s/%s/%ld: IconObjValid=%ld  SUccess=%ld\n", __FILE__, __FUNC__, __LINE__, IconObjValid, Success));
	if (IconObjValid || IsIconObjectValid(iwt, tni->tni_IconObj))
		{
		if (!Success)
			{
			// Thumbnail creation failed but IconObj is still valid
			// Restore initial icon settings
			ULONG IconUserFlags;

			// Reset thumbnail flag
			GetAttr(IDTA_UserFlags, tni->tni_IconObj, &IconUserFlags);
			IconUserFlags &= ~ICONOBJ_USERFLAGF_Thumbnail;

			DoMethod(tni->tni_IconObj, IDTM_FreeLayout, IOFREELAYOUTF_ScreenAvailable);
			SetAttrs(tni->tni_IconObj,
				IDTA_InnerTop, 		CurrentPrefs.pref_ImageBorders.Top,
				IDTA_InnerLeft, 	CurrentPrefs.pref_ImageBorders.Left,
				IDTA_InnerBottom, 	CurrentPrefs.pref_ImageBorders.Bottom,
				IDTA_InnerRight, 	CurrentPrefs.pref_ImageBorders.Right,
				IDTA_FrameType, 	CurrentPrefs.pref_FrameType,
				IDTA_FrameTypeSel, 	CurrentPrefs.pref_FrameTypeSel,
				IDTA_UserFlags, 	IconUserFlags,
				TAG_END);
			}

		ScalosUnLockIconList(iwt);
		}

	ScalosGfxFreeARGB(&PVResult.spvr_ARGBheader.argb_ImageData);
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (PVResult.spvr_sac)
		ScalosGfxFreeSAC(PVResult.spvr_sac);
	if (BNULL != fLock)
		UnLock(fLock);
	if (IS_VALID_LOCK(OldDir))
		CurrentDir(OldDir);
#ifdef __amigaos4__
	if(OpenedIFace)
		DropInterface(OpenedIFace);
#endif
	if (OpenedPluginBase)
		CloseLibrary(OpenedPluginBase);

	d1(KPrintF("%s/%s/%ld:  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
///

/// AddThumbnailEntry
static struct ThumbnailIcon *AddThumbnailEntry(struct internalScaWindowTask *iwt, Object *IconObj,
	BPTR DirLock, CONST_STRPTR Name, ULONG Flags, APTR UndoStep)
{
	struct ThumbnailIcon *tni;
	BOOL Success = FALSE;

	do	{
		tni = ScalosAlloc(sizeof(struct ThumbnailIcon));
		if (NULL == tni)
			break;

		tni->tni_Flags = Flags;
		tni->tni_IconObj = IconObj;
		tni->tni_DirLock = DupLock(DirLock);
		tni->tni_UndoStep = UndoStep;
		if ((BPTR)NULL == tni->tni_DirLock)
			break;

		tni->tni_IconName = AllocCopyString(Name);
		if (NULL == tni->tni_IconName)
			break;

		ScalosObtainSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
		// Add visible icons at head of list, so they are thumbnailed first
		if (IsIconObjVisible(iwt, IconObj))
			AddHead(&iwt->iwt_ThumbnailIconList, &tni->tni_Node);
		else
			AddTail(&iwt->iwt_ThumbnailIconList, &tni->tni_Node);
		ScalosReleaseSemaphore(&iwt->iwt_ThumbnailIconSemaphore);

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		FreeThumbnailEntry(tni);
		}

	return tni;
}
///

/// FlushThumbnailEntries
void FlushThumbnailEntries(struct internalScaWindowTask *iwt)
{
	struct ThumbnailIcon *tni;

	ScalosObtainSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
	while (tni = (struct ThumbnailIcon *) RemHead(&iwt->iwt_ThumbnailIconList))
		{
		FreeThumbnailEntry(tni);
		}
	ScalosReleaseSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
}
///

/// RemoveThumbnailEntry
void RemoveThumbnailEntry(struct internalScaWindowTask *iwt, Object *IconObj)
{
	struct ThumbnailIcon *tni, *tniNext;

	ScalosObtainSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
	for (tni = (struct ThumbnailIcon *) iwt->iwt_ThumbnailIconList.lh_Head;
		tni != (struct ThumbnailIcon *) &iwt->iwt_ThumbnailIconList.lh_Tail;
		tni = tniNext)
		{
		tniNext = (struct ThumbnailIcon *) tni->tni_Node.ln_Succ;
		if (IconObj == tni->tni_IconObj)
			{
			Remove(&tni->tni_Node);
			FreeThumbnailEntry(tni);
			break;
			}
		}
	ScalosReleaseSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
}
///

/// FreeThumbnailEntry
static void FreeThumbnailEntry(struct ThumbnailIcon *tni)
{
	if (tni)
		{
		if (tni->tni_DirLock)
			UnLock(tni->tni_DirLock);
		if (tni->tni_IconName)
			FreeCopyString(tni->tni_IconName);
		tni->tni_DirLock = (BPTR)NULL;
		tni->tni_IconName = NULL;
		ScalosFree(tni);
		}
}
///

/// FindIcon
static struct ScaIconNode *FindIcon(struct internalScaWindowTask *iwt, const Object *IconObject)
{
	struct ScaIconNode *in;

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (in->in_Icon == IconObject)
			{
			return in;
			}
		}
	for (in = iwt->iwt_WindowTask.wt_LateIconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (in->in_Icon == IconObject)
			{
			return in;
			}
		}

	return NULL;
}
///

/// IsIconObjectValid
static BOOL IsIconObjectValid(struct internalScaWindowTask *iwt, const Object *IconObject)
{
	struct ScaIconNode *in;

	ScalosLockIconListShared(iwt);

	in = FindIcon(iwt, IconObject);

	if (NULL == in)
		ScalosUnLockIconList(iwt);

	return (BOOL) (NULL != in);
}
///

/// GenerateThumbnailFromARGB
static BOOL GenerateThumbnailFromARGB(struct internalScaWindowTask *iwt,
	APTR ThumbnailCacheHandle, Object *IconObj,
        struct ARGBHeader *argbDest,
	BPTR fLock, BOOL MayAddToCache)
{
	ULONG SupportedColors = 256;

	d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));


	d1(KPrintF("%s/%s/%ld:  argb_Width=%ld  argb_Height=%ld  ImageData=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, argbDest->argb_Width, argbDest->argb_Height,\
		argbDest->argb_ImageData));

	GetAttr(IDTA_NumberOfColorsSupported, IconObj, &SupportedColors);
	d1(KPrintF("%s/%s/%ld:  SupportedColors=%lu\n", __FILE__, __FUNC__, __LINE__, SupportedColors));

	if (SupportedColors <= 256)
		{
		ULONG Depth;
		struct ScalosBitMapAndColor *sac;

		// Limit color depth to the number of colors supported by the icon
		for (Depth = 0; Depth < 7 && (1 << Depth) < SupportedColors; Depth++)
			;

		d1(KPrintF("%s/%s/%ld:  argb_ImageData: a=%ld  r=%ld  g=%ld  b=%ld\n", \
			__FILE__, __FUNC__, __LINE__, argbDest->argb_ImageData->Alpha, \
			argbDest->argb_ImageData->Red, argbDest->argb_ImageData->Green, \
			argbDest->argb_ImageData->Blue));


		sac = ScalosGfxMedianCutTags(argbDest, Depth,
			SCALOSGFX_MedianCutFlags, SCALOSGFXFLAGF_MedianCut_FloydSteinberg,
			SCALOSGFX_MedianCutReservedColors, 2,
			SCALOSGFX_MedianCutFriendBitMap, 0,
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  sac=%08lx\n", __FILE__, __FUNC__, __LINE__, sac));

		if (sac)
			{
			struct SM_SetThumbnailImage_Remapped *smtir;

			// Try to add icon thumbnail to cache
			if (MayAddToCache)
				{
				ThumbnailCacheAddRemapped(fLock, ThumbnailCacheHandle,
					sac, iwt->iwt_ThumbnailsLifetimeDays);
				}

			smtir = (struct SM_SetThumbnailImage_Remapped *) SCA_AllocMessage(MTYP_SetThumbnailImage_Remapped, 0);
			d1(KPrintF("%s/%s/%ld: smtir=%08lx\n", __FILE__, __FUNC__, __LINE__, smtir));
			if (smtir)
				{
				smtir->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
				smtir->smtir_IconObject = IconObj;
				smtir->smtir_NewImage = sac;

				// sac is freed by message receiver
				PutMsg(iwt->iwt_WindowTask.mt_WindowStruct->ws_MessagePort,
					&smtir->ScalosMessage.sm_Message);
				}
			else
				{
				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				ScalosGfxFreeSAC(sac);
				}
			}
		}
	else
		{
		struct SM_SetThumbnailImage_ARGB *smtia;

		// Try to add icon thumbnail to cache
		if (MayAddToCache)
			{
			ThumbnailCacheAddARGB(fLock, ThumbnailCacheHandle,
				argbDest, iwt->iwt_ThumbnailsLifetimeDays);
			}

		smtia = (struct SM_SetThumbnailImage_ARGB *) SCA_AllocMessage(MTYP_SetThumbnailImage_ARGB, 0);
		d1(KPrintF("%s/%s/%ld: smtia=%08lx\n", __FILE__, __FUNC__, __LINE__, smtia));
		if (smtia)
			{
			smtia->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
			smtia->smtia_IconObject = IconObj;
			smtia->smtia_NewImage = *argbDest;

			// argbDest->argb_ImageData is freed by message receiver
			PutMsg(iwt->iwt_WindowTask.mt_WindowStruct->ws_MessagePort,
				&smtia->ScalosMessage.sm_Message);

			// mark argbDest->argb_ImageData as invalid
			argbDest->argb_ImageData = NULL;
			argbDest = NULL;
			}
		}

	d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

	if (argbDest)
		{
		ScalosGfxFreeARGB(&argbDest->argb_ImageData);
		}

	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}
///

/// GenerateThumbnailFromSac
static BOOL GenerateThumbnailFromSac(struct internalScaWindowTask *iwt,
	Object *IconObj, struct ScalosBitMapAndColor **sac)
{
	struct SM_SetThumbnailImage_Remapped *smtir;

	d1(KPrintF("%s/%s/%ld:  START  Width=%ld  Height=%ld  Depth=%ld  TransparentColor=%ld  NumColors=%ld\n", \
		__FILE__, __FUNC__, __LINE__, (*sac)->sac_Width, (*sac)->sac_Height, \
		(*sac)->sac_Depth, (*sac)->sac_TransparentColor, \
		(*sac)->sac_NumColors));

	smtir = (struct SM_SetThumbnailImage_Remapped *) SCA_AllocMessage(MTYP_SetThumbnailImage_Remapped, 0);
	d1(KPrintF("%s/%s/%ld: smtir=%08lx\n", __FILE__, __FUNC__, __LINE__, smtir));
	if (smtir)
		{
		smtir->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
		smtir->smtir_IconObject = IconObj;
		smtir->smtir_NewImage = *sac;

		// sac is freed by message receiver
		PutMsg(iwt->iwt_WindowTask.mt_WindowStruct->ws_MessagePort,
			&smtir->ScalosMessage.sm_Message);
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		ScalosGfxFreeSAC(*sac);
		}
	*sac = NULL;

	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}
///

//-----------------------------------------------------------------------
/// ThumbnailCleanupCacheProcess
static SAVEDS(ULONG) ThumbnailCleanupCacheProcess(struct SM_StartProg *sMsg, struct SM_RunProcess *msg)
{
	(void) sMsg;
	(void) msg;

	d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	if (ScalosAttemptSemaphore(&ThumbnailsCleanupSemaphore))
		{
		struct Process *myProc = (struct Process *) FindTask(NULL);
		CONST_STRPTR ProcName = "Scalos_Thumbnail_DB_Cleanup";
		APTR prWindowPtr;
		APTR ThumbnailCacheHandle = NULL;	// Every instance of GenerateThumbnailProcess() has its own cache handle

		prWindowPtr = myProc->pr_WindowPtr;
		myProc->pr_WindowPtr = (APTR) ~0;    // suppress error requesters

		myProc->pr_Task.tc_Node.ln_Name = (STRPTR) ProcName;
		SetTaskPri(&myProc->pr_Task, -5);
		SetProgramName(ProcName);

		if (ThumbnailCacheOpen(&ThumbnailCacheHandle))
			{
			// Cleanup thumbnail cache -- remove all entries that
			// have not been accessed for a defined time.
			ThumbnailCacheCleanup(ThumbnailCacheHandle);
			}

		ThumbnailCacheClose(&ThumbnailCacheHandle);

		// restore pr_WindowPtr
		myProc->pr_WindowPtr = prWindowPtr;

		ScalosReleaseSemaphore(&ThumbnailsCleanupSemaphore);
		}

	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}
///
//-----------------------------------------------------------------------
/// ThumbnailShouldBeCached
static BOOL ThumbnailShouldBeCached(const struct BitMapHeader *bmhd)
{
	ULONG MinSizeLimit = CurrentPrefs.pref_ThumbnailMinSizeLimit;

	// if (bmhd->bmh_Width < "prefThumbnailMinSizeLimit" && bmhd->bmh_Height < "prefThumbnailMinSizeLimit")
	if (bmhd->bmh_Width <  MinSizeLimit && bmhd->bmh_Height < MinSizeLimit)
		return FALSE;

	return TRUE;
}
///
//-----------------------------------------------------------------------
/// SetIconThumbnailARGB
BOOL SetIconThumbnailARGB(struct internalScaWindowTask *iwt,
	struct SM_SetThumbnailImage_ARGB *smtia)
{
	struct ARGB *ImgDataClone = NULL;

	d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	if (IsIconObjectValid(iwt, smtia->smtia_IconObject))
		{
		do	{
			struct ExtGadget *gg = (struct ExtGadget *) smtia->smtia_IconObject;
			struct ARGBHeader argbh;
			ULONG AdjustLeft = 0, AdjustTop = 0;

			argbh = smtia->smtia_NewImage;

			d1(KPrintF("%s/%s/%ld:  argb_Width=%ld  argb_Height=%ld  ImageData=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, smtia->smtia_NewImage.argb_Width, smtia->smtia_NewImage.argb_Height,\
				smtia->smtia_NewImage.argb_ImageData));
			d1(KPrintF("%s/%s/%ld:  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge, gg->Width, gg->Height));

			if (CurrentPrefs.pref_ThumbnailsSquare && (smtia->smtia_NewImage.argb_Width < CurrentPrefs.pref_ThumbNailWidth
				|| smtia->smtia_NewImage.argb_Height < CurrentPrefs.pref_ThumbNailHeight))
				{
				LONG Left, Top;

				argbh.argb_Width = CurrentPrefs.pref_ThumbNailWidth;
				argbh.argb_Height = CurrentPrefs.pref_ThumbNailHeight;

				argbh.argb_ImageData = ImgDataClone = ScalosGfxCreateARGBTags(
					argbh.argb_Width, argbh.argb_Height,
					TAG_END);
				d1(KPrintF("%s/%s/%ld:  ImgDataClone=%08lx\n", __FILE__, __FUNC__, __LINE__, ImgDataClone));
				if (NULL == ImgDataClone)
					break;

				if (CurrentPrefs.pref_ThumbnailFillBackground)
					{
					ULONG rgb[3];
					struct ARGB FillARGB = { 128, 165, 145, 99 };

					FillARGB.Alpha = (CurrentPrefs.pref_ThumbnailBackgroundTransparency * 255) / 100;

					// get RGB values for thumbnail background pen
					GetRGB32(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap,
						PalettePrefs.pal_PensList[PENIDX_THUMBNAILBACKGROUNDPEN],
                                                1, rgb);

					// convert to one-byte RGB values
					FillARGB.Red   = rgb[0] >> 24;
					FillARGB.Green = rgb[1] >> 24;
					FillARGB.Blue  = rgb[2] >> 24;

					ScalosGfxSetARGB(&argbh, &FillARGB);
					}
				else
					{
					// make dest area transparent
					ScalosGfxARGBSetAlpha(&argbh, 0);
					}

				Left = (argbh.argb_Width - smtia->smtia_NewImage.argb_Width) / 2;
				Top  = (argbh.argb_Height - smtia->smtia_NewImage.argb_Height) / 2;

				ScalosGfxBlitARGB(&argbh,  &smtia->smtia_NewImage,
					Left, Top,
					0, 0,
					smtia->smtia_NewImage.argb_Width,
					smtia->smtia_NewImage.argb_Height);

				// If actual thumbnail is smaller than prefs rectangle, center icon.
				if (gg->Width > argbh.argb_Width)
					AdjustLeft = (gg->Width - argbh.argb_Width) / 2;
				if (gg->Height > argbh.argb_Height)
					AdjustTop = (gg->Height - argbh.argb_Height) / 2;
				}

			d1(KPrintF("%s/%s/%ld:  GWidth=%ld  GHeight=%ld  AdjustLeft=%ld  AdjustTop=%ld\n", \
				__FILE__, __FUNC__, __LINE__, argbh.argb_Width, argbh.argb_Height, AdjustLeft, AdjustTop));

			d1({ \
				ULONG TextStyle = 0; \
					\
				GetAttr(IDTA_TextStyle, smtia->smtia_IconObject, &TextStyle); \
				d1(KPrintF("%s/%s/%ld: TextStyle=%ld\n", __FILE__, __FUNC__, __LINE__, TextStyle)); \
			});

			DoMethod(smtia->smtia_IconObject, IDTM_FreeLayout, IOFREELAYOUTF_ScreenAvailable);

			SetAttrs(smtia->smtia_IconObject,
				GA_Width, 		argbh.argb_Width,
				GA_Height, 		argbh.argb_Height,
				IDTA_InnerTop, 		CurrentPrefs.pref_ThumbnailImageBorders.Top + AdjustTop,
				IDTA_InnerLeft, 	CurrentPrefs.pref_ThumbnailImageBorders.Left + AdjustLeft,
				IDTA_InnerBottom, 	CurrentPrefs.pref_ThumbnailImageBorders.Bottom,
				IDTA_InnerRight, 	CurrentPrefs.pref_ThumbnailImageBorders.Right,
				IDTA_FrameType, 	CurrentPrefs.pref_ThumbnailFrameType,
				IDTA_FrameTypeSel, 	CurrentPrefs.pref_ThumbnailFrameTypeSel,
				IDTA_Borderless,	FALSE,
				IDTA_CopyARGBImageData, TRUE,
				IDTA_ARGBImageData, 	(ULONG) &argbh,
				IDTA_SelARGBImageData, 	(ULONG) NULL,
				TAG_END);

			d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));
			} while (0);

		ScalosUnLockIconList(iwt);
		}

	ScalosGfxFreeARGB(&smtia->smtia_NewImage.argb_ImageData);
	ScalosGfxFreeARGB(&ImgDataClone);

	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}
///
//-----------------------------------------------------------------------
/// SetIconThumbnailRemapped
BOOL SetIconThumbnailRemapped(struct internalScaWindowTask *iwt,
	struct SM_SetThumbnailImage_Remapped *smtir)
{
	struct ScalosBitMapAndColor *SacClone = NULL;

	d1(KPrintF("%s/%s/%ld:  START  Width=%ld  Height=%ld  Depth=%ld  TransparentColor=%ld  NumColors=%ld\n", \
		__FILE__, __FUNC__, __LINE__, smtir->smtir_NewImage->sac_Width, \
		smtir->smtir_NewImage->sac_Height, smtir->smtir_NewImage->sac_Depth, \
		smtir->smtir_NewImage->sac_TransparentColor, smtir->smtir_NewImage->sac_NumColors));

	if (IsIconObjectValid(iwt, smtir->smtir_IconObject))
		{
		do	{
			struct ExtGadget *gg = (struct ExtGadget *) smtir->smtir_IconObject;
			ULONG AdjustLeft = 0, AdjustTop = 0;
			struct ScalosBitMapAndColor *sac = smtir->smtir_NewImage;

			if (CurrentPrefs.pref_ThumbnailsSquare && (smtir->smtir_NewImage->sac_Width < CurrentPrefs.pref_ThumbNailWidth
					|| smtir->smtir_NewImage->sac_Height < CurrentPrefs.pref_ThumbNailHeight))
				{
				WORD Left, Top;
				struct RastPort rp;
				ULONG ColorIndex;

				// Create new ScalosBitMapAndColor with desired size
				SacClone = sac = ScalosGfxCreateSACTags(
					CurrentPrefs.pref_ThumbNailWidth, CurrentPrefs.pref_ThumbNailHeight,
					smtir->smtir_NewImage->sac_Depth,
					NULL,
					TAG_END);
				d1(KPrintF("%s/%s/%ld:  SacClone=%08lx\n", __FILE__, __FUNC__, __LINE__, SacClone));
				if (NULL == SacClone)
					break;

				// Clone ColorTable of smtir->smtir_NewImage
				memcpy(SacClone->sac_ColorTable, smtir->smtir_NewImage->sac_ColorTable,
					SAC_COLORTABLESIZE(smtir->smtir_NewImage));

				InitRastPort(&rp);
				rp.BitMap = sac->sac_BitMap;

				ColorIndex = sac->sac_NumColors - 2;

				sac->sac_TransparentColor = smtir->smtir_NewImage->sac_TransparentColor;

				if (CurrentPrefs.pref_ThumbnailFillBackground)
					{
					// Fill BitMap with selected background color
					ULONG rgb[3];

					// get RGB values for thumbnail background pen
					GetRGB32(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap,
						PalettePrefs.pal_PensList[PENIDX_THUMBNAILBACKGROUNDPEN],
                                                1, rgb);

					// copy RGB into reserved entris in ColorTable
					sac->sac_ColorTable[3 * ColorIndex + 0] = rgb[0];
					sac->sac_ColorTable[3 * ColorIndex + 1] = rgb[1];
					sac->sac_ColorTable[3 * ColorIndex + 2] = rgb[2];

					d1(KPrintF("%s/%s/%ld:  ColorIndex=%08lx\n", __FILE__, __FUNC__, __LINE__, ColorIndex));
					}
				else
					{
					// Fill BitMap with transparent color
					if (SAC_TRANSPARENT_NONE == sac->sac_TransparentColor)
						{
						// If we currently have no transparent color,
						// use the reserved colortable entry
						sac->sac_TransparentColor = ColorIndex;
						}
					else
						{
						ColorIndex = sac->sac_TransparentColor;
						}

					d1(KPrintF("%s/%s/%ld:  sac_TransparentColor=%08lx\n", __FILE__, __FUNC__, __LINE__, sac->sac_TransparentColor));
					}

				// fill rp with background color
				d1(KPrintF("%s/%s/%ld:  ColorIndex=%08lx\n", __FILE__, __FUNC__, __LINE__, ColorIndex));
				SetRast(&rp, ColorIndex);

				Left = (sac->sac_Width - smtir->smtir_NewImage->sac_Width) / 2;
				Top  = (sac->sac_Height - smtir->smtir_NewImage->sac_Height) / 2;

				// Blit image from smtir->smtir_NewImage into SacClone
				BltBitMap(smtir->smtir_NewImage->sac_BitMap,
					0, 0,
					sac->sac_BitMap,
					Left, Top,
					smtir->smtir_NewImage->sac_Width,
					smtir->smtir_NewImage->sac_Height,
					0x0c0,
					~0, NULL);

				// If actual thumbnail is smaller than prefs rectangle, center icon.
				if (gg->Width > sac->sac_Width)
					AdjustLeft = (gg->Width - sac->sac_Width) / 2;
				if (gg->Height > sac->sac_Height)
					AdjustTop = (gg->Height - sac->sac_Height) / 2;
                                }

			SetAttrs(smtir->smtir_IconObject,
				GA_Width, 		sac->sac_Width,
				GA_Height, 		sac->sac_Height,
				IDTA_InnerTop, 		CurrentPrefs.pref_ThumbnailImageBorders.Top + AdjustTop,
				IDTA_InnerLeft, 	CurrentPrefs.pref_ThumbnailImageBorders.Left + AdjustLeft,
				IDTA_InnerBottom, 	CurrentPrefs.pref_ThumbnailImageBorders.Bottom,
				IDTA_InnerRight, 	CurrentPrefs.pref_ThumbnailImageBorders.Right,
				IDTA_FrameType, 	CurrentPrefs.pref_ThumbnailFrameType,
				IDTA_FrameTypeSel, 	CurrentPrefs.pref_ThumbnailFrameTypeSel,
				IDTA_Backfill,      	CurrentPrefs.pref_ThumbnailFillBackground
						? PalettePrefs.pal_PensList[PENIDX_THUMBNAILBACKGROUNDPEN] : IDTA_BACKFILL_NONE,
				IDTA_BackfillSel,       CurrentPrefs.pref_ThumbnailFillBackground
						? PalettePrefs.pal_PensList[PENIDX_THUMBNAILBACKGROUNDPENSEL] : IDTA_BACKFILL_NONE,
				IDTA_Borderless,	FALSE,
				IDTA_ARGBImageData, 	(ULONG) NULL,
				IDTA_SelARGBImageData, 	(ULONG) NULL,
				TAG_END);

			d1(KPrintF("%s/%s/%ld:  sac_BitMap=%08lx  sac_TransparentColor=%ld\n", \
				__FILE__, __FUNC__, __LINE__, sac->sac_BitMap, sac->sac_TransparentColor));

			DoMethod(smtir->smtir_IconObject, IDTM_NewImage, sac, NULL);
			} while (0);

		ScalosUnLockIconList(iwt);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	ScalosGfxFreeSAC(smtir->smtir_NewImage);
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	ScalosGfxFreeSAC(SacClone);

	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}
///
//-----------------------------------------------------------------------
/// ThumbnailsStartCleanup
void ThumbnailsStartCleanup(BOOL Force)
{
	static T_TIMEVAL tvLastCleanup;
	T_TIMEVAL tv;

	// Check if a cleanup has already been run today.
	GetSysTime(&tv);

	if (Force || ((tv.tv_secs - tvLastCleanup.tv_secs) > 24 * 60 * 60))
		{
		struct ScaWindowStruct *ws;

		// Remember date+time of last cleanup
		tvLastCleanup = tv;

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		// Start process to cleanup thumbnail cache on behalf of desktop window
		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (WSV_Type_DeviceWindow == ws->ws_WindowType)
				{
				DoMethod(ws->ws_WindowTask->mt_MainObject,
					SCCM_RunProcess, ThumbnailCleanupCacheProcess,
					NULL, 0, SCCV_RunProcess_NoReply);
				break;
				}
			}

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		SCA_UnLockWindowList();
		}
}
///
//-----------------------------------------------------------------------

