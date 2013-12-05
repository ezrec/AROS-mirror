// Patches.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuitionbase.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <datatypes/pictureclass.h>
#include <dos/dostags.h>
#include <guigfx/guigfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/guigfx.h>
#include <proto/datatypes.h>
#include <proto/gadtools.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/palette.h>
#include <scalos/pattern.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "functions.h"
#include "Variables.h"
#include "Patches.h"

//----------------------------------------------------------------------------

// local data structures

#if defined(__MORPHOS__)
// Extended DiskObject structure
#define OWN_MAGIC 0x466f4164

struct OwnDiskObject
	{
	struct DiskObject diskobj;
	struct FreeList *fl;       /* this is safe for WB 3.1 */
	/* start of additions */
	ULONG ownmagic;
	APTR ownptr;
	APTR png_context;
	ULONG ttnum;
	ULONG ttcur;
	STRPTR path;
	APTR glowchunk;
	ULONG glowsize;
	struct BitMap *pngimage;
	ULONG pngimage_width;
	ULONG pngimage_height;
	};

#define ISOWN(x) (((struct OwnDiskObject *)x)->ownmagic == OWN_MAGIC && ((struct OwnDiskObject *)x)->ownptr == x)

#endif /* __MORPHOS__ */


struct DosOpenInfo
	{
	BPTR doi_fh;				// Filehandle as returned from Open - our key
	LONG doi_AccessMode;			// accessMode from Open()
	struct DateStamp doi_DateBefore;	// file's DateStamp immediately after Open()
	};

//----------------------------------------------------------------------------

// local functions

static LONG ReOpenScalos(void);
static APTR CloneDOImage(const APTR origImage);
static STRPTR *CloneDOToolTypes(CONST_STRPTR *origToolTypes);
static struct DiskObject *CloneDiskObject(const struct DiskObject *origDiskObj);
static void ScaFreeDiskObject(struct DiskObject *DiskObj);
static SAVEDS(ULONG) AppChangeFunc(struct WBArg *ArgArray, struct SM_RunProcess *msg);
static void PatchRefreshIcon(CONST_STRPTR IconName, UBYTE IconType);
static BOOL IsScalosProcess(void);
static void PatchOpenDisposeData(void *data);
static void PatchOpenDisposeKey(void *key);
static int PatchOpenCompare(const void *key1, const void *key2);
#ifdef TEST_OPENWINDOWTAGLIST
static void ByteDump(const void *Data, size_t Length);
#endif /* TEST_OPENWINDOWTAGLIST */
static void PatchAsyncWBUndoRename(CONST_STRPTR oldName, CONST_STRPTR newName);
static void PatchAsyncWBUndoCreateDir(CONST_STRPTR name);

//----------------------------------------------------------------------------

// public data items

#ifdef __AROS__

#ifdef TEST_OPENWINDOWTAGLIST
struct Window * (*OldOpenWindowTagList) ();
#endif /* TEST_OPENWINDOWTAGLIST */
ULONG (*OldCloseWB) ();
ULONG (*OldOpenWB) ();
ULONG (*OldRemoveAppIcon) ();
BOOL (*OldRemoveAppWindow) ();
BOOL (*OldRemoveAppMenuItem) ();
struct Screen * (*OldOpenScreenTagList) ();
void (*OldUpdateWorkbench) ();
APTR OldAddAppIconA;
APTR OldAddAppWindowA;
APTR OldAppAppMenuItemA;
APTR OldSetBackFill;
APTR OldWBInfo;
BOOL (*OldPutDiskObject) ();
BOOL (*OldDeleteDiskObject) ();
BOOL (*OldPutIconTagList) ();
ULONG (*OldDeleteFile) ();
ULONG (*OldRename) ();
BPTR (*OldOpen) ();
ULONG (*OldClose) ();
BPTR (*OldCreateDir) ();

#else

#ifdef TEST_OPENWINDOWTAGLIST
LIBFUNC_P3_DPROTO(struct Window *, (*OldOpenWindowTagList),
	A0, struct NewWindow *, newWin,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase);
#endif /* TEST_OPENWINDOWTAGLIST */

LIBFUNC_P1_DPROTO(ULONG, (*OldCloseWB),
	A6, struct IntuitionBase *, iBase);
LIBFUNC_P1_DPROTO(ULONG, (*OldOpenWB),
	A6, struct IntuitionBase *, iBase);
LIBFUNC_P2_DPROTO(ULONG, (*OldRemoveAppIcon),
	A0, struct AppIcon *, appIcon,
	A6, struct Library *, wbBase);
LIBFUNC_P2_DPROTO(BOOL, (*OldRemoveAppWindow),
	A0, struct AppWindow *, aw,
	A6, struct Library *, wbBase);
LIBFUNC_P2_DPROTO(BOOL, (*OldRemoveAppMenuItem),
	A0, struct AppMenuItem *, ami,
	A6, struct Library *, wbBase);
LIBFUNC_P3_DPROTO(struct Screen *, (*OldOpenScreenTagList),
	A0, struct NewScreen *, newScr,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase);
LIBFUNC_P4_DPROTO(void, (*OldUpdateWorkbench),
	A0, CONST_STRPTR, Name,
	A1, BPTR, ParentLock,
	D0, LONG, Action,
	A6, struct Library *, wbBase);
APTR OldAddAppIconA;
APTR OldAddAppWindowA;
APTR OldAppAppMenuItemA;
APTR OldSetBackFill;
APTR OldWBInfo;
LIBFUNC_P3_DPROTO(BOOL, (*OldPutDiskObject),
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A6, struct Library *, IconBase);
LIBFUNC_P2_DPROTO(BOOL, (*OldDeleteDiskObject),
	A0, CONST_STRPTR, Name,
	A6, struct Library *, IconBase);
LIBFUNC_P4_DPROTO(BOOL, (*OldPutIconTagList),
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A2, struct TagItem *, tags,
	A6, struct Library *, IconBase);

LIBFUNC_P2_DPROTO(ULONG, (*OldDeleteFile),
	D1, CONST_STRPTR, Name,
	A6, struct DosLibrary *, dOSBase);
LIBFUNC_P3_DPROTO(ULONG, (*OldRename),
	D1, CONST_STRPTR, oldName,
	D2, CONST_STRPTR, newName,
	A6, struct DosLibrary *, dOSBase);
LIBFUNC_P3_DPROTO(BPTR, (*OldOpen),
	D1, CONST_STRPTR, name,
	D2, LONG, accessMode,
	A6, struct DosLibrary *, DOSBase);
LIBFUNC_P2_DPROTO(ULONG, (*OldClose),
	D1, BPTR, file,
	A6, struct DosLibrary *, DOSBase);
LIBFUNC_P2_DPROTO(BPTR, (*OldCreateDir),
	D1, CONST_STRPTR, name,
	A6, struct DosLibrary *, DOSBase);

//LIBFUNC_P1_DPROTO(CONST_STRPTR, (*OldwbPrivate2),
//	D0, ULONG, StringID);

#endif

//----------------------------------------------------------------------------

// local data items

static struct BTree *PatchOpenBTree = NULL;
static ULONG DoiCount;

//----------------------------------------------------------------------------

// CloseWorkBench() replacement
LIBFUNC_P1(LONG, sca_CloseWorkBench,
	A6, struct IntuitionBase *, iBase, 0)
{
	struct MsgPort *ReplyPort = NULL;
	LONG Success = FALSE;

	d1(KPrintF("%s/%s/%ld: Start\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct SM_AppSleep *smas;
		struct ScaWindowStruct *ws;

		if (!CheckAllowCloseWB())
			return FALSE;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (MainWindowTask->sleepflag)
			{
			Success = TRUE;
			break;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		MainWindowTask->sleepflag = TRUE;

		ReplyPort = CreateMsgPort();
		if (NULL == ReplyPort)
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		PatternsOff(MainWindowTask, ReplyPort);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		FreePatternPrefs();
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (GuiGFXBase && PenShareMap)
			{
			DeletePenShareMap(PenShareMap);
			PenShareMap = NULL;
			}

		if (IconBase->lib_Version >= 44)
			{
			// IconControlA()
			IconControl(NULL, 
				ICONCTRLA_SetGlobalScreen, NULL,
				TAG_END);
			}

		smas = (struct SM_AppSleep *) SCA_AllocMessage(MTYP_AppSleep, 0);
		if (NULL == smas)
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		smas->ScalosMessage.sm_Message.mn_ReplyPort = ReplyPort;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smas->ScalosMessage.sm_Message);

		WaitReply(ReplyPort, &MainWindowTask->mwt, MTYP_AppSleep);

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

			if (iwtx)
				iwtx->iwt_OrigWinScreen = NULL;
			}

		SCA_UnLockWindowList();

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		UnlockScalosPens();

		FreeScreenDrawInfo(iInfos.xii_iinfos.ii_Screen, iInfos.xii_iinfos.ii_DrawInfo);
		iInfos.xii_iinfos.ii_DrawInfo = NULL;

		FreeVisualInfo(iInfos.xii_iinfos.ii_visualinfo);
		iInfos.xii_iinfos.ii_visualinfo = NULL;

		UnlockPubScreen("Workbench", iInfos.xii_iinfos.ii_Screen);
		iInfos.xii_iinfos.ii_Screen = NULL;

		d1(KPrintF("%s/%s/%ld: OldCloseWB=%08lx\n", __FILE__, __FUNC__, __LINE__, OldCloseWB));

#ifdef __AROS__
		Success = AROS_CALL0(LONG, (*OldCloseWB),
				     struct IntuitionBase *, iBase);
#else
		Success = CALLLIBFUNC_P1((*OldCloseWB), A6, iBase);
#endif
		} while (0);

	d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (ReplyPort)
		DeleteMsgPort(ReplyPort);

	if (!Success)
		ReOpenScalos();

	return Success;
}
LIBFUNC_END


// OpenWorkBench() replacement
LIBFUNC_P1(LONG, sca_OpenWorkBench,
	A6, struct IntuitionBase *, iBase, 0)
{
	LONG Success;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	do	{
		if (!MainWindowTask->sleepflag)
			{
			Success = TRUE;
			break;
			}

#ifdef __AROS__
		Success = AROS_CALL0(IPTR, (*OldOpenWB),
				     struct IntuitionBase *, iBase);
#else
		Success = CALLLIBFUNC_P1((*OldOpenWB), A6, iBase);
#endif
		if (!Success)
			break;

		Success = ReOpenScalos();
		} while (0);

	// OpenWorkBench() is supposed to return WBScreeen !!!

	return Success ? (LONG) iInfos.xii_iinfos.ii_Screen : 0L;
}
LIBFUNC_END


static LONG ReOpenScalos(void)
{
	LONG Success = FALSE;
	struct MsgPort *ReplyPort = NULL;

	do	{
		struct SM_AppWakeup *smaw;

		if (NULL == iInfos.xii_iinfos.ii_Screen)
			iInfos.xii_iinfos.ii_Screen = LockPubScreen("Workbench");
		if (NULL == iInfos.xii_iinfos.ii_Screen)
			break;

		LockScalosPens();

		if (NULL == iInfos.xii_iinfos.ii_DrawInfo)
			iInfos.xii_iinfos.ii_DrawInfo = GetScreenDrawInfo(iInfos.xii_iinfos.ii_Screen);
		if (NULL == iInfos.xii_iinfos.ii_DrawInfo)
			break;

		if (NULL == iInfos.xii_iinfos.ii_visualinfo)
			iInfos.xii_iinfos.ii_visualinfo = GetVisualInfoA(iInfos.xii_iinfos.ii_Screen, NULL);

		if (IconBase->lib_Version >= 44)
			{
			// IconControlA()
			IconControl(NULL, 
				ICONCTRLA_SetGlobalScreen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				TAG_END);
			}

		SetDefaultPubScreen("Workbench");

		if (GuiGFXBase && NULL == PenShareMap)
			PenShareMap = CreatePenShareMapA(NULL);

#if defined(__MORPHOS__)
#if defined(SA_OpacitySupport)
		if (IntuitionBase->LibNode.lib_Version >= 51)
			{
			LONG attr;

			GetAttr(SA_OpacitySupport, iInfos.xii_iinfos.ii_Screen, &attr);
			d1(KPrintF("%s/%s/%ld: SA_OpacitySupport=%ld\n", __FILE__, __FUNC__, __LINE__, attr));
			iInfos.xii_Layers3D = attr > SAOS_OpacitySupport_OnOff;
			}
		else
#endif //defined(SA_OpacitySupport)
			{
			iInfos.xii_Layers3D = NULL != FindTask("« LayerInfoTask »");
			}
#endif //defined(__MORPHOS__)

		ReplyPort = CreateMsgPort();
		if (NULL == ReplyPort)
			break;

		smaw = (struct SM_AppWakeup *) SCA_AllocMessage(MTYP_AppWakeup, 0);
		if (NULL == smaw)
			break;

		smaw->ScalosMessage.sm_Message.mn_ReplyPort = ReplyPort,
		smaw->smaw_ReLayout = TRUE;

		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smaw->ScalosMessage.sm_Message);

		WaitReply(ReplyPort, &MainWindowTask->mwt, MTYP_AppWakeup);

		ReadPatternPrefs();
		PatternsOn(MainWindowTask);

		MainWindowTask->sleepflag = FALSE;
		} while (0);

	if (ReplyPort)
		DeleteMsgPort(ReplyPort);

	return Success;
}


// AppIcon = AddAppIconA(id, userdata, text, msgport, lock, diskobj, taglist)
//    D0                 D0     D1      A0     A1      A2      A3      A4
LIBFUNC_P8(struct AppIcon *, sca_AddAppIconA,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, IconText,
	A1, struct MsgPort *, iconPort,
	A2, BPTR, lock,
	A3, struct DiskObject *, DiskObj,
	A4, struct TagItem *, TagList,
	A6, struct Library *, wbBase, 0)
{
	struct DiskObject *DiskObjClone = NULL;
	struct AppObject *appIcon = NULL;
	Object *IconObj = NULL;

	d1(KPrintF("%s/%s/%ld: id=%08lx UserData=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, Id, UserData, IconText));

	(void) lock;
	(void) wbBase;

	do	{
		if (NULL == FindTagItem(WBAPPICONA_RenderHook, TagList)
			&& strlen(IconText) > 0)
			{
			struct MsgPort *ReplyPort = CreateMsgPort();

			d1(KPrintF("%s/%s/%ld: ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, ReplyPort));

			if (ReplyPort)
				{
				struct WBArg ArgArray;

				ArgArray.wa_Name = (STRPTR) IconText;

				RunProcess(&MainWindowTask->mwt.iwt_WindowTask,
					(RUNPROCFUNC) AppChangeFunc, 
					sizeof(ArgArray)/sizeof(ULONG), &ArgArray, ReplyPort);

				do	{
					struct SM_RunProcess *msg;

					WaitPort(ReplyPort);
					msg = (struct SM_RunProcess *) GetMsg(ReplyPort);

					d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

					if (msg && ID_IMSG == msg->ScalosMessage.sm_Signature
						&& msg->ScalosMessage.sm_MessageType == MTYP_RunProcess)
						{
						struct WBArg *ArgArray = (struct WBArg *) &msg->Flags;

						IconObj = (Object *) ArgArray->wa_Name;
						d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

						SCA_FreeMessage(&msg->ScalosMessage);

						if (IconObj)
							DiskObj = NULL;
						break;
						}
					} while (1);

				DeleteMsgPort(ReplyPort);
				}
			}

		if (NULL == IconObj)
			{
#if defined(__MORPHOS__)
			d1(KPrintF("%s/%s/%ld: DiskObj=%08lx\n", __FILE__, __FUNC__, __LINE__, DiskObj));
			if (ISOWN(DiskObj))
				{
				struct OwnDiskObject *odo = (struct OwnDiskObject *) DiskObj;

				d1(KPrintF("%s/%s/%ld: odo=%08lx  path=<%s>\n", __FILE__, __FUNC__, __LINE__, odo, odo->path));

				if (odo->path)
					{
					STRPTR PathCopy = AllocCopyString(odo->path);

					if (PathCopy)
						{
						ULONG pos = IsIconName(PathCopy);

						d1(KPrintF("%s/%s/%ld: PathCopy=<%s>  pos=%08lx\n", __FILE__, __FUNC__, __LINE__, PathCopy, pos));

						if (0 != pos && ~0 != pos)
							{
							// remove ".info" extension
							STRPTR ppos = (STRPTR) pos;
							*ppos = '\0';
							}

						d1(KPrintF("%s/%s/%ld: PathCopy=<%s>\n", __FILE__, __FUNC__, __LINE__, PathCopy));

						IconObj = NewIconObjectTags(PathCopy,
							IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
							IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints,
							IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor,
							IDTA_SizeConstraints, (ULONG) &iInfos.xii_iinfos.ii_AppWindowStruct->ws_IconSizeConstraints,
							IDTA_ScalePercentage, iInfos.xii_iinfos.ii_AppWindowStruct->ws_IconScaleFactor,
							TAG_END);

						FreeCopyString(PathCopy);
						}
					}
				}
#endif /* __MORPHOS__ */

			if (NULL == TagList || !GetTagData(WBAPPICONA_PropagatePosition, 0, TagList))
				{
				DiskObj = DiskObjClone = CloneDiskObject(DiskObj);
				}

			if (NULL == DiskObj)
				break;

			if (NULL == IconObj)
				{
				IconObj = Convert2IconObjectTags(DiskObj,
					IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
					TAG_END);
				}
			}

		d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		if (NULL == IconObj)
			break;

		SetAttrs(IconObj,
			IDTA_Text, (ULONG) IconText,
			TAG_END);

		OpenLibrary("scalos.library", 0);

		appIcon = SCA_NewAddAppIcon(Id, UserData, IconObj, iconPort, TagList);

		d1(KPrintF("%s/%s/%ld: appIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, appIcon));
		if (NULL == appIcon)
			break;

		appIcon->appo_userdata2 = DiskObj;
		} while (0);

	if (NULL == appIcon)
		{
		if (IconObj)
			DisposeIconObject(IconObj);

		if (DiskObjClone)
			ScaFreeDiskObject(DiskObjClone);
		}

	d1(KPrintF("%s/%s/%ld: appIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, appIcon));

	return (struct AppIcon *) appIcon;
}
LIBFUNC_END


static APTR CloneDOImage(const APTR origImage)
{
	struct Image *NewImage = NULL;
	const struct Image *OldImage = (struct Image *) origImage;

	do	{
		size_t ImgSize;

		if (NULL == origImage)
			break;

		if (0 == TypeOfMem(origImage))
			break;

		if (0 == TypeOfMem(OldImage->ImageData))
			break;

		NewImage = ScalosAlloc(sizeof(struct Image));
		if (NULL == NewImage)
			break;

		*NewImage = *OldImage;

		ImgSize = ((OldImage->Width + OldImage->LeftEdge + 15) & ~0x0f) 
				* (OldImage->TopEdge + OldImage->Height) * OldImage->Depth;
		ImgSize = (ImgSize + 3) & ~0x03;		// round to longaligned

		NewImage->ImageData = ScalosAlloc(ImgSize);
		if (NULL == NewImage->ImageData)
			break;

		CopyMemQuick(OldImage->ImageData, NewImage->ImageData, ImgSize);
//???		CacheClearU();
		} while (0);

	if (NewImage && NULL == NewImage->ImageData)
		{
		ScalosFree(NewImage);
		NewImage = NULL;
		}

	return NewImage;
}


static STRPTR *CloneDOToolTypes(CONST_STRPTR *origToolTypes)
{
	STRPTR *NewToolTypes;
	STRPTR *dplp, dlp;
	CONST_STRPTR *splp;
	ULONG ttLength, ttCount;

	d1(kprintf("%s/%s/%ld: origToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, origToolTypes));

	if (NULL == origToolTypes)
		return NULL;

	ttLength = sizeof(STRPTR);
	ttCount = 1;

	for (splp=origToolTypes; *splp; splp++)
		{
		ttCount++;
		ttLength += sizeof(STRPTR) + strlen(*splp) + 1;
		d1(kprintf("%s/%s/%ld: (ToolType) <%s>\n", __FILE__, __FUNC__, __LINE__, *splp));
		}

	d1(kprintf("%s/%s/%ld: ttCount=%ld  ttLength=%ld\n", __FILE__, __FUNC__, __LINE__, ttCount, ttLength));

	NewToolTypes = ScalosAlloc(ttLength);

	d1(kprintf("%s/%s/%ld: NewToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, NewToolTypes));
	if (NULL == NewToolTypes)
		return NULL;

	dlp = ((STRPTR) NewToolTypes) + ttCount * sizeof(STRPTR);

	for (dplp=NewToolTypes, splp=origToolTypes; *splp; dplp++, splp++)
		{
		*dplp = dlp;
		strcpy(*dplp, *splp);
		dlp += 1 + strlen(dlp);
		}
	*dplp = NULL;

	d1(kprintf("%s/%s/%ld: NewToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, NewToolTypes));

	return NewToolTypes;
}


static struct DiskObject *CloneDiskObject(const struct DiskObject *origDiskObj)
{
	struct DiskObject *NewDiskObj;

#if 0
	// test for CloneDOToolTypes() function
	{
	// Test
	STRPTR *ytt;
	STRPTR xTT[] =
		{
		"Erster Text",
		"2",
		"Dritte Textzeile",
		"Vier",
		"",
		NULL
		};

	ytt = CloneDOToolTypes(xTT);
	if (ytt)
		ScalosFree(ytt);
	}
#endif

	if (IconBase->lib_Version >= 44)
		{
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// DupDiskObjectA(
		NewDiskObj = DupDiskObject(origDiskObj,
			ICONDUPA_DuplicateImages, TRUE,
			ICONDUPA_DuplicateImageData, TRUE,
			ICONDUPA_DuplicateToolTypes, TRUE,
			ICONDUPA_ActivateImageData, TRUE,
			TAG_END);

		d1(kprintf("%s/%s/%ld: NewDiskObj=%08lx\n", __FILE__, __FUNC__, __LINE__, NewDiskObj));
		}
	else
		{
		NewDiskObj = GetDiskObject(NULL);

		d1(kprintf("%s/%s/%ld: NewDiskObj=%08lx\n", __FILE__, __FUNC__, __LINE__, NewDiskObj));
		if (NewDiskObj)
			{
#if defined(__MORPHOS__)
			*NewDiskObj = *origDiskObj;
#endif /* __MORPHOS__ */

			NewDiskObj->do_Gadget.Width = origDiskObj->do_Gadget.Width;
			NewDiskObj->do_Gadget.Height = origDiskObj->do_Gadget.Height;
			NewDiskObj->do_Gadget.Flags = origDiskObj->do_Gadget.Flags;

			NewDiskObj->do_Gadget.GadgetRender = CloneDOImage(origDiskObj->do_Gadget.GadgetRender);

			d1(kprintf("%s/%s/%ld: GadgetRender=%08lx\n", __FILE__, __FUNC__, __LINE__, NewDiskObj->do_Gadget.GadgetRender));

			if (origDiskObj->do_Gadget.Flags & GFLG_GADGHIMAGE)
				{
				NewDiskObj->do_Gadget.SelectRender = CloneDOImage(origDiskObj->do_Gadget.SelectRender);
				d1(kprintf("%s/%s/%ld: SelectRender=%08lx\n", __FILE__, __FUNC__, __LINE__, NewDiskObj->do_Gadget.SelectRender));

				if (NULL == NewDiskObj->do_Gadget.SelectRender)
					NewDiskObj->do_Gadget.Flags &= ~GFLG_GADGHIMAGE;
				}

			NewDiskObj->do_CurrentX = origDiskObj->do_CurrentX;
			NewDiskObj->do_CurrentY = origDiskObj->do_CurrentY;

			NewDiskObj->do_ToolTypes = CloneDOToolTypes((CONST_STRPTR *) origDiskObj->do_ToolTypes);

			d1(kprintf("%s/%s/%ld: ToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, NewDiskObj->do_ToolTypes));
			}
		}

	return NewDiskObj;
}


static void ScaFreeDiskObject(struct DiskObject *DiskObj)
{
	if (IconBase->lib_Version < 44)
		{
		if (DiskObj->do_Gadget.GadgetRender)
			{
			struct Image *img = (struct Image *) DiskObj->do_Gadget.GadgetRender;

			if (img->ImageData)
				ScalosFree(img->ImageData);

			ScalosFree(img);

			DiskObj->do_Gadget.GadgetRender = NULL;
			}
		if (DiskObj->do_Gadget.SelectRender)
			{
			struct Image *img = (struct Image *) DiskObj->do_Gadget.SelectRender;

			if (img->ImageData)
				ScalosFree(img->ImageData);

			ScalosFree(img);

			DiskObj->do_Gadget.SelectRender = NULL;
			}
		if (DiskObj->do_ToolTypes)
			{
			ScalosFree(DiskObj->do_ToolTypes);
			DiskObj->do_ToolTypes = NULL;
			}
		}

	FreeDiskObject(DiskObj);
}


static SAVEDS(ULONG) AppChangeFunc(struct WBArg *ArgArray, struct SM_RunProcess *msg)
{
	STRPTR Path = NULL;
	Object *IconObj = NULL;

	d1(kprintf("%s/%s/%ld: msg=%08lx  ArgArray=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, ArgArray));
	d1(kprintf("%s/%s/%ld: <%s>\n", __FILE__, __FUNC__, __LINE__, ArgArray->wa_Name));

	do	{
		struct ScaWindowStruct *ws = iInfos.xii_iinfos.ii_AppWindowStruct;

		// check if AppIcon name contains invalid characters ":/"
		if (strchr(ArgArray->wa_Name, ':'))
			break;
		if (strchr(ArgArray->wa_Name, '/'))
			break;

		Path = AllocPathBuffer();
		if (NULL == Path)
			break;

		stccpy(Path, CurrentPrefs.pref_DefIconPath, Max_PathLen);
		AddPart(Path, ArgArray->wa_Name, Max_PathLen);

		d1(kprintf("%s/%s/%ld: Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

		IconObj = NewIconObjectTags(Path,
			IDTA_SizeConstraints, (ULONG) &ws->ws_IconSizeConstraints,
			IDTA_ScalePercentage, ws->ws_IconScaleFactor,
			TAG_END);
		d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		if (IconObj)
			break;	// Success!!

		stccpy(Path, CurrentPrefs.pref_DefIconPath, Max_PathLen);
		AddPart(Path, "def_", Max_PathLen);
		SafeStrCat(Path, ArgArray->wa_Name, Max_PathLen);

		d1(kprintf("%s/%s/%ld: Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

		IconObj = NewIconObjectTags(Path,
			IDTA_SizeConstraints, (ULONG) &ws->ws_IconSizeConstraints,
			IDTA_ScalePercentage, ws->ws_IconScaleFactor,
			TAG_END);
		} while (0);

	if (Path)
		FreePathBuffer(Path);

	d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	// return Result
	ArgArray->wa_Name = (STRPTR) IconObj;

	return 0;
}



// success = RemoveAppIcon(AppIcon)
//   D0                      A0
LIBFUNC_P2(ULONG, sca_RemoveAppIcon,
	A0, struct AppIcon *, appIcon,
	A6, struct Library *, wbBase, 0)
{
	struct AppObject *AppObj = (struct AppObject *) appIcon;
	BOOL Success = TRUE;
	struct TagItem *TagList;
	struct DiskObject *DiskObj;
	Object *IconObj;

	d1(kprintf("%s/%s/%ld: appIcon=%08lx\n", __FILE__, __FUNC__, __LINE__, appIcon));

	if (NULL == AppObj || 0 == TypeOfMem(appIcon) || ID_SC != AppObj->appo_Kennung)
		{
#ifdef __AROS__
		return AROS_CALL1(BOOL, (*OldRemoveAppIcon),
				  AROS_LDA(struct AppIcon *, appIcon, A0),
				  struct Library *, wbBase);
#else
		return CALLLIBFUNC_P2((*OldRemoveAppIcon), A0, appIcon, A6, wbBase);
#endif
		}

	TagList = AppObj->appo_TagList;
	DiskObj = AppObj->appo_userdata2;
	IconObj = AppObj->appo_object.appoo_IconObject;

	CloseLibrary(&ScalosBase->scb_LibNode);

	d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	SCA_RemoveAppObject(AppObj);

	if (TagList && GetTagData(WBAPPICONA_PropagatePosition, 0, TagList))
		DiskObj = NULL;		// DiskObject was not cloned and must not be freed

	FreeTagItems(TagList);		// TagList may be NULL!

	DisposeIconObject(IconObj);

	if (DiskObj)
		ScaFreeDiskObject(DiskObj);

	return Success;
}
LIBFUNC_END


// AppIcon = AddAppWindowA(id, userdata, window, msgport, taglist)
//    D0                   D0     D1       A0      A1      A2
LIBFUNC_P6(struct AppWindow *, sca_AddAppWindowA,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, struct Window *, window,
	A1, struct MsgPort *, iconPort,
	A2, struct TagItem *, TagList,
	A6, struct Library *, wbBase, 0)
{
	(void) wbBase;

	d1(KPrintF("%s/%s/%ld: Id=%08lx  UserData=%08lx\n", __FILE__, __FUNC__, __LINE__, Id, UserData));

	OpenLibrary("scalos.library", 0);

	return (struct AppWindow *) SCA_NewAddAppWindow(Id, UserData, window, iconPort, TagList);
}
LIBFUNC_END


//	success = RemoveAppWindow(AppWindow)
//         D0                        A0
LIBFUNC_P2(BOOL, sca_RemoveAppWindow,
	A0, struct AppWindow *, aw,
	A6, struct Library *, wbBase, 0)
{
	struct AppObject *appo = (struct AppObject *) aw;

	d1(KPrintF("%s/%s/%ld: aw=%08lx\n", __FILE__, __FUNC__, __LINE__, aw));

	if (NULL == aw || 0 == TypeOfMem(aw) || ID_SC != appo->appo_Kennung)
#ifdef __AROS__
		return AROS_CALL1(BOOL, (*OldRemoveAppWindow),
				  AROS_LDA(struct AppWindow *, aw, A0),
				  struct Library *, wbBase);
#else
		return (BOOL) CALLLIBFUNC_P2((*OldRemoveAppWindow), A0, aw, A6, wbBase);
#endif

	CloseLibrary(&ScalosBase->scb_LibNode);

	return SCA_RemoveAppObject(appo);
}
LIBFUNC_END


// AppMenuItem = AddAppMenuItemA(id, userdata, text, msgport, taglist)
//      D0                       D0     D1      A0      A1      A2
LIBFUNC_P6(struct AppMenuItem *, sca_AddAppMenuItemA,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, text,
	A1, struct MsgPort *, iconPort,
	A2, struct TagItem *, TagList,
	A6, struct Library *, wbBase, 0)
{
	(void) wbBase;

	d1(KPrintF("%s/%s/%ld: Id=%08lx  UserData=%08lx\n", __FILE__, __FUNC__, __LINE__, Id, UserData));

	OpenLibrary("scalos.library", 0);

	return (struct AppMenuItem *) SCA_NewAddAppMenuItem(Id, UserData, text, iconPort, TagList);
}
LIBFUNC_END


//	success = RemoveAppMenuItem(AppWindow)
//         D0                        A0
LIBFUNC_P2(BOOL, sca_RemoveAppMenuItem,
	A0, struct AppMenuItem *, ami,
	A6, struct Library *, wbBase, 0)
{
	struct AppObject *appo = (struct AppObject *) ami;

	d1(KPrintF("%s/%s/%ld: ami=%08lx\n", __FILE__, __FUNC__, __LINE__, ami));

	if (NULL == ami || 0 == TypeOfMem(ami) || ID_SC != appo->appo_Kennung)
#if __AROS__
		return AROS_CALL1(BOOL, (*OldRemoveAppMenuItem),
				  AROS_LDA(struct AppMenuItem *, ami, A0),
				  struct Library *, wbBase);
#else
		return (BOOL) CALLLIBFUNC_P2((*OldRemoveAppMenuItem), A0, ami, A6, wbBase);
#endif

	CloseLibrary(&ScalosBase->scb_LibNode);

	return SCA_RemoveAppObject(appo);
}
LIBFUNC_END


#ifdef TEST_OPENWINDOWTAGLIST
//	Window = OpenWindowTagList( NewWindow, TagItems )
//	D0	  		    A0         A1
//
//	struct Window *OpenWindowTagList( struct NewScreen *, struct TagItem * );
LIBFUNC_P3(struct Window *, sca_OpenWindowTagList,
	A0, struct NewWindow *, newWin,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase)
{
	struct Window *Win = NULL;
//	  struct TagItem *tl = TagList;
//	  struct TagItem *ti;

	d1(KPrintF("\n%s/%s/%ld: Task=<%s>  NewWindow=%08lx\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)->tc_Node.ln_Name, newWin));
/*
	while ((ti = NextTagItem(&tl)))
		{
		switch (ti->ti_Tag)
			{
		case WA_Dummy:
			d1(KPrintF("%s/%s/%ld: WA_Dummy  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Left:
			d1(KPrintF("%s/%s/%ld: WA_Left  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Top:
			d1(KPrintF("%s/%s/%ld: WA_Top  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Width:
			d1(KPrintF("%s/%s/%ld: WA_Width  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Height:
			d1(KPrintF("%s/%s/%ld: WA_Height  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_DetailPen:
			d1(KPrintF("%s/%s/%ld: WA_DetailPen  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_BlockPen:
			d1(KPrintF("%s/%s/%ld: WA_BlockPen  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_IDCMP:
			d1(KPrintF("%s/%s/%ld: WA_IDCMP  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Flags:
			d1(KPrintF("%s/%s/%ld: WA_Flags  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Gadgets:
			d1(KPrintF("%s/%s/%ld: WA_Gadgets  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Checkmark:
			d1(KPrintF("%s/%s/%ld: WA_Checkmark  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Title:
			d1(KPrintF("%s/%s/%ld: WA_Title  data=<%s>\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ScreenTitle:
			d1(KPrintF("%s/%s/%ld: WA_ScreenTitle  data=<%s>\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_CustomScreen:
			d1(KPrintF("%s/%s/%ld: WA_CustomScreen  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SuperBitMap:
			d1(KPrintF("%s/%s/%ld: WA_SuperBitMap  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_MinWidth:
			d1(KPrintF("%s/%s/%ld: WA_MinWidth  data=%öd\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_MinHeight:
			d1(KPrintF("%s/%s/%ld: WA_MinHeight  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_MaxWidth:
			d1(KPrintF("%s/%s/%ld: WA_MaxWidth  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_MaxHeight:
			d1(KPrintF("%s/%s/%ld: WA_MaxHeight  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_InnerWidth:
			d1(KPrintF("%s/%s/%ld: WA_InnerWidth  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_InnerHeight:
			d1(KPrintF("%s/%s/%ld: WA_InnerHeight  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_PubScreenName:
			d1(KPrintF("%s/%s/%ld: WA_PubScreenName  data=<%s>\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_PubScreen:
			d1(KPrintF("%s/%s/%ld: WA_PubScreen  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_PubScreenFallBack:
			d1(KPrintF("%s/%s/%ld: WA_PubScreenFallBack  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_WindowName:
			d1(KPrintF("%s/%s/%ld: WA_WindowName  data=<%s>\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Colors:
			d1(KPrintF("%s/%s/%ld: WA_Colors  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Zoom:
			d1(KPrintF("%s/%s/%ld: WA_Zoom  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_MouseQueue:
			d1(KPrintF("%s/%s/%ld: WA_MouseQueue  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_BackFill:
			d1(KPrintF("%s/%s/%ld: WA_BackFill  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_RptQueue:
			d1(KPrintF("%s/%s/%ld: WA_RptQueue  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SizeGadget:
			d1(KPrintF("%s/%s/%ld: WA_SizeGadget  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_DragBar:
			d1(KPrintF("%s/%s/%ld: WA_DragBar  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_DepthGadget:
			d1(KPrintF("%s/%s/%ld: WA_DepthGadget  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_CloseGadget:
			d1(KPrintF("%s/%s/%ld: WA_CloseGadget  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Backdrop:
			d1(KPrintF("%s/%s/%ld: WA_Backdrop  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ReportMouse:
			d1(KPrintF("%s/%s/%ld: WA_ReportMouse  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_NoCareRefresh:
			d1(KPrintF("%s/%s/%ld: WA_NoCareRefresh  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Borderless:
			d1(KPrintF("%s/%s/%ld: WA_Borderless  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Activate:
			d1(KPrintF("%s/%s/%ld: WA_Activate  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_RMBTrap:
			d1(KPrintF("%s/%s/%ld: WA_RMBTrap  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_WBenchWindow:
			d1(KPrintF("%s/%s/%ld: WA_WBenchWindow  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SimpleRefresh:
			d1(KPrintF("%s/%s/%ld: WA_SimpleRefresh  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SmartRefresh:
			d1(KPrintF("%s/%s/%ld: WA_SmartRefresh  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SizeBRight:
			d1(KPrintF("%s/%s/%ld: WA_SizeBRight  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SizeBBottom:
			d1(KPrintF("%s/%s/%ld: WA_SizeBBottom  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_AutoAdjust:
			d1(KPrintF("%s/%s/%ld: WA_AutoAdjust  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_GimmeZeroZero:
			d1(KPrintF("%s/%s/%ld: WA_GimmeZeroZero  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_MenuHelp:
			d1(KPrintF("%s/%s/%ld: WA_MenuHelp  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_NewLookMenus:
			d1(KPrintF("%s/%s/%ld: WA_NewLookMenus  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_AmigaKey:
			d1(KPrintF("%s/%s/%ld: WA_AmigaKey  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_NotifyDepth:
			d1(KPrintF("%s/%s/%ld: WA_NotifyDepth  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Pointer:
			d1(KPrintF("%s/%s/%ld: WA_Pointer  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_BusyPointer:
			d1(KPrintF("%s/%s/%ld: WA_BusyPointer  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_PointerDelay:
			d1(KPrintF("%s/%s/%ld: WA_PointerDelay  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_TabletMessages:
			d1(KPrintF("%s/%s/%ld: WA_TabletMessages  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_HelpGroup:
			d1(KPrintF("%s/%s/%ld: WA_HelpGroup  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_HelpGroupWindow:
			d1(KPrintF("%s/%s/%ld: WA_HelpGroupWindow  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraTitlebarGadgets:
			d1(KPrintF("%s/%s/%ld: WA_ExtraTitlebarGadgets  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraGadgetsStartID:
			d1(KPrintF("%s/%s/%ld: WA_ExtraGadgetsStartID  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraGadget_Iconify:
			d1(KPrintF("%s/%s/%ld: WA_ExtraGadget_Iconify  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraGadget_Lock:
			d1(KPrintF("%s/%s/%ld: WA_ExtraGadget_Lock  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraGadget_MUI:
			d1(KPrintF("%s/%s/%ld: WA_ExtraGadget_MUI  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraGadget_PopUp:
			d1(KPrintF("%s/%s/%ld: WA_ExtraGadget_PopUp  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraGadget_Snapshot:
			d1(KPrintF("%s/%s/%ld: WA_ExtraGadget_Snapshot  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ExtraGadget_Jump:
			d1(KPrintF("%s/%s/%ld: WA_ExtraGadget_Jump  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SkinInfo:
			d1(KPrintF("%s/%s/%ld: WA_SkinInfo  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_TransparentRegion:
			d1(KPrintF("%s/%s/%ld: WA_TransparentRegion  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_TransparentRegionHook:
			d1(KPrintF("%s/%s/%ld: WA_TransparentRegionHook  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_UserPort:
			d1(KPrintF("%s/%s/%ld: WA_UserPort  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ToolbarWindow:
			d1(KPrintF("%s/%s/%ld: WA_ToolbarWindow  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_PointerType:
			d1(KPrintF("%s/%s/%ld: WA_PointerType  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_FrontWindow:
			d1(KPrintF("%s/%s/%ld: WA_FrontWindow  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Parent:
			d1(KPrintF("%s/%s/%ld: WA_Parent  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Opacity:
			d1(KPrintF("%s/%s/%ld: WA_Opacity  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_HasAlpha:
			d1(KPrintF("%s/%s/%ld: WA_HasAlpha  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SizeNumerator:
			d1(KPrintF("%s/%s/%ld: WA_SizeNumerator  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SizeDenominator:
			d1(KPrintF("%s/%s/%ld: WA_SizeDenominator  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SizeExtraWidth:
			d1(KPrintF("%s/%s/%ld: WA_SizeExtraWidth  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_SizeExtraHeight:
			d1(KPrintF("%s/%s/%ld: WA_SizeExtraHeight  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_HitLevel:
			d1(KPrintF("%s/%s/%ld: WA_HitLevel  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ShadowTop:
			d1(KPrintF("%s/%s/%ld: WA_ShadowTop  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ShadowLeft:
			d1(KPrintF("%s/%s/%ld: WA_ShadowLeft  data=%ld\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ShadowRight:
			d1(KPrintF("%s/%s/%ld: WA_ShadowRight  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_ShadowBottom:
			d1(KPrintF("%s/%s/%ld: WA_ShadowBottom  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_VisibleOnMaximize:
			d1(KPrintF("%s/%s/%ld: WA_VisibleOnMaximize  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			break;
		case WA_Dummy + 150:
			d1(KPrintF("%s/%s/%ld: WA_Dummy + 150  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			if (TypeOfMem(ti->ti_Data))
			{
			struct MinNode *mn = (struct MinNode *) ti->ti_Data;

			while (mn)
				{
				d1(KPrintF("%s/%s/%ld: mn=%08lx\n", __FILE__, __FUNC__, __LINE__, mn));
				ByteDump(mn, 0x110);
				mn = mn->mln_Succ;
				}
			}
			break;
		default:
			d1(KPrintF("%s/%s/%ld: tag=%08lx  data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Tag, ti->ti_Data));
			break;
			}
		}
*/

#ifdef __AROS__
	Win = AROS_CALL2(struct Window *, (*OldOpenWindowTagList),
			 AROS_LDA(struct NewWindow *, newWin, A0),
			 AROS_LDA(struct TagItem   *, TagList, A1),
			 struct IntuitionBase *, iBase);
#else
	Win = (struct Window *) CALLLIBFUNC_P3((*OldOpenWindowTagList),
		A0, newWin,
		A1, TagList,
                A6, iBase);
#endif

	d1(KPrintF("%s/%s/%ld: >>> Window=%08lx\n", __FILE__, __FUNC__, __LINE__, Win));
	if (Win)
		d1(KPrintF("%s/%s/%ld: >>> MoreFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, Win->MoreFlags));

	return Win;
}
LIBFUNC_END
#endif /* TEST_OPENWINDOWTAGLIST */


//	Screen = OpenScreenTagList( NewScreen, TagItems )
//	D0	  		    A0         A1
//
//	struct Screen *OpenScreenTagList( struct NewScreen *, struct TagItem * );
LIBFUNC_P3(struct Screen *, sca_OpenScreenTagList,
	A0, struct NewScreen *, newScr,
	A1, const struct TagItem *, TagList,
	A6, struct IntuitionBase *, iBase, 0)
{
	struct Screen *Scr = NULL;
	CONST_STRPTR PubScreenName;

	d1(KPrintF("%s/%s/%ld: NewScreen=%08lx\n", __FILE__, __FUNC__, __LINE__, newScr));

	PubScreenName = (CONST_STRPTR) GetTagData(SA_PubName, (ULONG)NULL, (struct TagItem *) TagList);

	if (PubScreenName && 0 == strcmp("Workbench", PubScreenName))
		{
		// Workbench screen is going to be opened
		struct TagItem *AllocatedTagList;

		d1(KPrintF("%s/%s/%ld: driPens=%08lx  ScreenColorList=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, driPens, ScreenColorList));

		AllocatedTagList = CloneTagItems(TagList);
		d1(KPrintF("%s/%s/%ld: AllocatedTagList=%08lx\n", __FILE__, __FUNC__, __LINE__, AllocatedTagList));

		if (AllocatedTagList)
			{
			struct TagItem *tag;
			struct TagItem ti[10];
			short nTag = 0;

			// replace any existing SA_Pens, SA_Colors32, and SA_ShowTitle by TAG_IGNORE
			tag = FindTagItem(SA_Pens, AllocatedTagList);
			d1(KPrintF("%s/%s/%ld: SA_Pens=%08lx\n", __FILE__, __FUNC__, __LINE__, tag));
			if (tag)
				tag->ti_Tag = TAG_IGNORE;
			tag = FindTagItem(SA_Colors32, AllocatedTagList);
			d1(KPrintF("%s/%s/%ld: SA_Colors32=%08lx\n", __FILE__, __FUNC__, __LINE__, tag));
			if (tag)
				tag->ti_Tag = TAG_IGNORE;
			tag = FindTagItem(SA_ShowTitle, AllocatedTagList);
			d1(KPrintF("%s/%s/%ld: SA_ShowTitle=%08lx\n", __FILE__, __FUNC__, __LINE__, tag));
			if (tag)
				tag->ti_Tag = TAG_IGNORE;

			if (-1 != PalettePrefs.pal_driPens[0])
				{
				ti[nTag].ti_Tag = SA_Pens;
				ti[nTag++].ti_Data = (ULONG) PalettePrefs.pal_driPens;

				d1(KPrintF("%s/%s/%ld: driPens[0]=%ld  driPens[1]=%ld\n", __FILE__, __FUNC__, __LINE__, PalettePrefs.pal_driPens[0], PalettePrefs.pal_driPens[1]));

#if 0
				{
					short n;
					for (n=0; PalettePrefs.pal_driPens[n]>=0; n++)
						KPrintF("%s/%s/%ld: driPens[%ld]=%ld\n", __FILE__, __FUNC__, __LINE__, n, PalettePrefs.pal_driPens[n]);
				}
#endif
				}
			if (PalettePrefs.pal_ScreenColorList)
				{
				ti[nTag].ti_Tag = SA_Colors32;
				ti[nTag++].ti_Data = (ULONG) PalettePrefs.pal_ScreenColorList;

#if 0
				{
					short n, nmax = PalettePrefs.pal_ScreenColorList[0] >> 16;
					short col = PalettePrefs.pal_ScreenColorList[0] & 0xffff;
					for (n=0; n<nmax; n++,col++)
						{
						kprintf("%s/%s/%ld: pen[%ld] : %08lx %08lx %08lx\n",
						__FILE__, __FUNC__, __LINE__, col, PalettePrefs.pal_ScreenColorList[1+3*n], \
                                                PalettePrefs.pal_ScreenColorList[2+3*n], PalettePrefs.pal_ScreenColorList[3+3*n]);
						}
				}
#endif
				}
			if (CurrentPrefs.pref_FullBenchFlag)
				{
				ti[nTag].ti_Tag = SA_ShowTitle;
				ti[nTag++].ti_Data = FALSE;
				}
			ti[nTag].ti_Tag = TAG_MORE;
			ti[nTag++].ti_Data = (ULONG) AllocatedTagList;

			ti[nTag].ti_Tag = TAG_END;

#ifdef __AROS__
			Scr = AROS_CALL2(struct Screen *, (*OldOpenScreenTagList),
					 AROS_LDA(struct NewScreen *, newScr, A0),
					 AROS_LDA(struct TagItem   *, ti, A1),
					 struct IntuitionBase *, iBase);
#else
			Scr = (struct Screen *) CALLLIBFUNC_P3((*OldOpenScreenTagList),
				A0, newScr,
				A1, ti,
	                        A6, iBase);
#endif

			if (Scr && PalettePrefs.pal_ScreenColorList)
				{
				short n, nmax = PalettePrefs.pal_ScreenColorList[0] >> 16;
				short col = PalettePrefs.pal_ScreenColorList[0] & 0xffff;

				for (n=0; n<nmax; n++,col++)
					{
					ObtainPen(Scr->ViewPort.ColorMap, col, 
						PalettePrefs.pal_ScreenColorList[1+3*n],
						PalettePrefs.pal_ScreenColorList[2+3*n],
                                                PalettePrefs.pal_ScreenColorList[3+3*n],
						0);
					}
				}

			FreeTagItems(AllocatedTagList);
			}
		}
	else
		{
#ifdef __AROS__
		Scr = AROS_CALL2(struct Screen *, (*OldOpenScreenTagList),
				 AROS_LDA(struct NewScreen *, newScr, A0),
				 AROS_LDA(struct TagItem   *, TagList, A1),
				 struct IntuitionBase *, iBase);
#else
		Scr = (struct Screen *) CALLLIBFUNC_P3((*OldOpenScreenTagList),
			A0, newScr,
			A1, TagList,
                        A6, iBase);
#endif
		}

	d1(KPrintF("%s/%s/%ld: Scr=%08lx\n", __FILE__, __FUNC__, __LINE__, Scr));

	return Scr;
}
LIBFUNC_END


//	UpdateWorkbench(name, parentlock, action)
//	                A0    A1          D0
//
//	VOID UpdateWorkbench(char *, BPTR, LONG);
LIBFUNC_P4(void, sca_UpdateWorkbench,
	A0, CONST_STRPTR, Name,
	A1, BPTR, ParentLock,
	D0, LONG, Action,
	A6, struct Library *, wbBase, 0)
{
	struct ScaUpdateIcon_IW upd;

	(void) Action;
	(void) wbBase;

	d1(kprintf("%s/%s/%ld: Name=<%s>  Action=%ld\n", __FILE__, __FUNC__, __LINE__, Name, Action));
	debugLock_d1(ParentLock);

	upd.ui_iw_Lock = ParentLock;
	upd.ui_iw_Name = (STRPTR) Name;
	upd.ui_IconType = ICONTYPE_NONE;

	SCA_UpdateIcon(SIV_IconWin, &upd, sizeof(upd));
}
LIBFUNC_END


//LIBFUNC_P2(CONST_STRPTR, sca_GetLocString,
//	D0, ULONG, StringID,
//	A6, struct Library *, wbBase)
//{
//	return GetLocString(StringID);
//}
//LIBFUNC_END


// private Workbench function -84
LIBFUNC_P1(LONG, sca_SetBackFill,
	A6, struct Library *, wbBase, 0)
{
	(void) wbBase;

	return 0;
}
LIBFUNC_END



//	  worked = WBInfo(lock, name, screen)
//	  d0              a0    a1    a2
//
//	  ULONG WBInfo(BPTR, STRPTR, struct Screen *);

LIBFUNC_P4(ULONG, sca_WBInfo,
	A0, BPTR, lock,
	A1, STRPTR, name,
	A2, struct Screen *, screen,
	A6, struct Library *, wbBase, 0)
{
	ULONG Success;
	struct ScaIconNode in;

	(void) wbBase;
	(void) screen;

	d1(KPrintF("%s/%s/%ld: START Name=<%s>  screen=%08lx\n", __FILE__, __FUNC__, __LINE__, name, screen));
	debugLock_d1(lock);

	(void) SCA_LockWindowList(SCA_LockWindowList_Shared);

	memset(&in, 0, sizeof(in));

	in.in_Lock = lock;
	in.in_Name = name;
	in.in_Icon = NewIconObjectTags(name,
		TAG_END);
	if (NULL == in.in_Icon)
		{
		in.in_Icon = SCA_GetDefIconObject(lock, name);
		}

	WBInfoStart((struct internalScaWindowTask *) winlist.wl_WindowStruct->ws_WindowTask,
		&in, NULL);

	Success = TRUE;

	SCA_UnLockWindowList();

	d1(KPrintF("%s/%s/%ld: END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
LIBFUNC_END


//	status = PutDiskObject(name, diskobj)
//	 D0                    A0      A1
//
//	BOOL PutDiskObject(STRPTR, struct DiskObject *);
LIBFUNC_P3(BOOL, sca_PutDiskObject,
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A6, struct Library *, IconBase, 0)
{
#ifdef __AROS__
	BOOL Success = AROS_CALL2(BOOL, (*OldPutDiskObject),
				  AROS_LDA(CONST_STRPTR,        Name, A0),
				  AROS_LDA(struct DiskObject *, diskObj, A1),
				  struct Library *, IconBase);
#else
	BOOL Success = CALLLIBFUNC_P3((*OldPutDiskObject),
		A0, Name, A1, diskObj, A6, IconBase);
#endif

	d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (Success)
		PatchRefreshIcon(Name, diskObj->do_Type);

	return Success;
}
LIBFUNC_END


//	result = DeleteDiskObject(name)
//	  D0                      A0
//
//	BOOL DeleteDiskObject(STRPTR);
LIBFUNC_P2(BOOL, sca_DeleteDiskObject,
	A0, CONST_STRPTR, Name,
	A6, struct Library *, IconBase, 0)
{
#ifdef __AROS__
	BOOL Success = AROS_CALL1(BOOL, (*OldDeleteDiskObject),
				  AROS_LDA(UBYTE *, Name, A0),
				  struct Library *, IconBase);
#else
	BOOL Success = CALLLIBFUNC_P2((*OldDeleteDiskObject),
		A0, Name, A6, IconBase);
#endif

	d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (Success)
		PatchRefreshIcon(Name, ICONTYPE_NONE);

	return Success;
}
LIBFUNC_END


//	success = PutIconTagList(name,icon,tags);
//	D0                       A0   A1   A2
//
//	BOOL PutIconTagList(STRPTR name,struct DiskObject *icon,
//	                    struct TagItem *tags);
LIBFUNC_P4(BOOL, sca_PutIconTagList,
	A0, CONST_STRPTR, Name,
	A1, const struct DiskObject *, diskObj,
	A2, struct TagItem *, tags,
	A6, struct Library *, IconBase, 0)
{
	//ULONG NotifyWB;
#ifdef __AROS__
	BOOL Success = AROS_CALL3(BOOL, (*OldPutIconTagList),
				  AROS_LDA(CONST_STRPTR,        Name, A0),
				  AROS_LDA(struct DiskObject *, diskObj, A1),
				  AROS_LDA(struct TagItem *,    tags, A2),
				  struct Library *, IconBase);
#else
	BOOL Success = CALLLIBFUNC_P4((*OldPutIconTagList),
		A0, Name, A1, diskObj, A2, tags, A6, IconBase);
#endif

	//NotifyWB = GetTagData(ICONPUTA_NotifyWorkbench, FALSE, tags);

	d1(KPrintF("%s/%s/%ld: Success=%ld  NotifyWB=%ld\n", __FILE__, __FUNC__, __LINE__, Success, NotifyWB));

	if (Success)
		PatchRefreshIcon(Name, diskObj->do_Type);

	return Success;
}
LIBFUNC_END



static void PatchRefreshIcon(CONST_STRPTR IconName, UBYTE IconType)
{
	d1(KPrintF("%s/%s/%ld: START Name=<%s>  IconType=%lu\n", __FILE__, __FUNC__, __LINE__, IconName, IconType));
	if (!IsScalosProcess())
		{
		STRPTR Path = AllocCopyString(IconName);

		d1(KPrintF("%s/%s/%ld: Path=%08lx\n", __FILE__, __FUNC__, __LINE__, Path));
		if (Path)
			{
			struct ScaUpdateIcon_IW upd;
			STRPTR lp;
			char ch;

			d1(KPrintF("%s/%s/%ld: Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));
			StripIconExtension(Path);
			d1(KPrintF("%s/%s/%ld: Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

			lp = PathPart(Path);
			ch = *lp;
			*lp = '\0';

			upd.ui_iw_Lock = Lock(Path, ACCESS_READ);
			debugLock_d1(upd.ui_iw_Lock);

			*lp = ch;

			if (upd.ui_iw_Lock)
				{
				upd.ui_iw_Name = FilePart(Path);
				upd.ui_IconType = IconType;
				d1(KPrintF("%s/%s/%ld: ui_iw_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, upd.ui_iw_Name));

				SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));

				UnLock(upd.ui_iw_Lock);
				}

			FreeCopyString(Path);
			}
		}
	d1(KPrintF("%s/%s/%ld: END \n", __FILE__, __FUNC__, __LINE__));
}


//	success = DeleteFile( name )
//	D0		      D1
//
//	BOOL DeleteFile(STRPTR)
LIBFUNC_P2(ULONG, sca_DeleteFile,
	D1, CONST_STRPTR, Name,
	A6, struct DosLibrary *, DOSBase, 0)
{
	ULONG Success;

	d1(KPrintF("%s/%s/%ld: START Name=<%s>  Task=<%s>\n", __FILE__, __FUNC__, __LINE__, Name, FindTask(NULL)->tc_Node.ln_Name));

#ifdef __AROS__
	Success = AROS_CALL1(BOOL, (*OldDeleteFile),
			     AROS_LDA(CONST_STRPTR, Name, D1),
			     struct DosLibrary *, DOSBase);
#else
	Success = CALLLIBFUNC_P2((*OldDeleteFile),
		D1, Name, A6, DOSBase);
#endif

	if (Success)
		{
		PatchRefreshIcon(Name, ICONTYPE_NONE);
		}

	d1(KPrintF("%s/%s/%ld: END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
LIBFUNC_END


//	success = Rename( oldName, newName )
//	D0		  D1	   D2
//
//	BOOL Rename(STRPTR, STRPTR)
LIBFUNC_P3(ULONG, sca_Rename,
	D1, CONST_STRPTR, oldName,
	D2, CONST_STRPTR, newName,
	A6, struct DosLibrary *, DOSBase, 0)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	ULONG isIcon;
	STRPTR fName = NULL;
	BOOL Success;
	BPTR fLock;
	BPTR parentLock = BNULL;
	struct ScaIconNode *in = NULL;
	struct List HistoryPathList;
	BOOL WinListLocked = FALSE;

	d1(kprintf("%s/%s/%ld: oldName=<%s>  newName=<%s>  Task=<%s>\n", __FILE__, __FUNC__, __LINE__, oldName, newName, FindTask(NULL)->tc_Node.ln_Name));

	do	{
		NewList(&HistoryPathList);

		PatchAsyncWBUndoRename(oldName, newName);

		fLock = Lock((STRPTR) oldName, ACCESS_READ);
		debugLock_d1(fLock);

		isIcon = IsIconName(oldName);

		if (BNULL == fLock)
			break;

		WinListLocked = CollectRenameAdjustHistoryPaths(&HistoryPathList, fLock);

		if (FALSE == isIcon)
			break;

		parentLock = ParentDir(fLock);
		debugLock_d1(parentLock);
		if ((BPTR)NULL == parentLock)
			break;

		fName = AllocCopyString(FilePart((STRPTR) oldName));
		if (NULL == fName)
			break;

		StripIconExtension(fName);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		in = FindBackdropIconExclusive(parentLock, fName);
		if (NULL == in)
			break;
		} while (0);

	d1(kprintf("%s/%s/%ld: in=%08lx  fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, in, fLock));

#ifdef __AROS__
	Success = AROS_CALL2(LONG, (*OldRename),
			     AROS_LDA(CONST_STRPTR, oldName, D1),
			     AROS_LDA(CONST_STRPTR, newName, D2),
			     struct DosLibrary *, DOSBase);
#else
	Success = CALLLIBFUNC_P3((*OldRename),
		D1, oldName, D2, newName, A6, DOSBase);
#endif

	if (Success)
		{
		d1(KPrintF("%s/%s/%ld: isIcon=%08lx  parentLock=%08lx\n", __FILE__, __FUNC__, __LINE__, isIcon, parentLock));

		AdjustRenameAdjustHistoryPaths(&HistoryPathList, fLock);

		if (0 == isIcon && fLock)
			{
			// adjust window name of renamed drawer
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			AdjustRenamedWindowName(fLock);
			}

		if (in)
			{
			// adjust renamed backdrop icon
			debugLock_d1(fLock);
			d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

			AdjustBackdropRenamed(fLock, in);

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}

		if (parentLock && fName)
			{
			// initiate removal of old object
			struct ScaUpdateIcon_IW upd;

			upd.ui_iw_Lock = parentLock;
			upd.ui_iw_Name = fName;
			upd.ui_IconType = ICONTYPE_NONE;

			debugLock_d1(parentLock);
			d1(KPrintF("%s/%s/%ld: fName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

			SCA_UpdateIcon(SIV_IconWin, &upd, sizeof(upd));
			}

		do	{
			// force update for new object
			struct ScaUpdateIcon_IW upd;

			isIcon = IsIconName(newName);
			if (FALSE == isIcon)
				break;

			if (fLock)
				{
				UnLock(fLock);
				//fLock = NULL;
				}
			if (parentLock)
				{
				UnLock(parentLock);
				parentLock = (BPTR)NULL;
				}
			if (fName)
				{
				FreeCopyString(fName);
				fName = NULL;
				}

			fLock = Lock((STRPTR) newName, ACCESS_READ);
			debugLock_d1(fLock);
			if ((BPTR)NULL == fLock)
				break;

			parentLock = ParentDir(fLock);
			debugLock_d1(parentLock);
			if ((BPTR)NULL == parentLock)
				break;

			fName = AllocCopyString(FilePart((STRPTR) newName));
			d1(KPrintF("%s/%s/%ld: fName=%08lx\n", __FILE__, __FUNC__, __LINE__, fName));
			if (NULL == fName)
				break;

			StripIconExtension(fName);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			upd.ui_iw_Lock = parentLock;
			upd.ui_iw_Name = fName;
			upd.ui_IconType = ICONTYPE_NONE;

			debugLock_d1(parentLock);
			d1(KPrintF("%s/%s/%ld: fName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

			SCA_UpdateIcon(SIV_IconWin, &upd, sizeof(upd));
			} while (0);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (in)
		ScalosUnLockIconList(iwtMain);
	if (parentLock)
		UnLock(parentLock);
	if (fName)
		FreeCopyString(fName);
	if (fLock)
		UnLock(fLock);

	CleanupRenameAdjustHistoryPaths(&HistoryPathList, WinListLocked);

	d1(KPrintF("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
LIBFUNC_END


//	file = Open( name, accessMode )
//	D0	     D1    D2
//
//	BPTR Open(STRPTR, LONG)
LIBFUNC_P3(BPTR, sca_Open,
	D1, CONST_STRPTR, name,
	D2, LONG, accessMode,
	A6, struct DosLibrary *, DOSBase, 0)
{
	BPTR file;
	T_ExamineData *fib = NULL;
	struct DosOpenInfo *doi;

#ifdef __AROS__
	file = AROS_CALL2(BPTR, (*OldOpen),
			  AROS_LDA(CONST_STRPTR, name,       D1),
			  AROS_LDA(LONG,         accessMode, D2),
			  struct DosLibrary *, DOSBase);
#else
	file = CALLLIBFUNC_P3((*OldOpen),
		D1, name, D2, accessMode,
                A6, DOSBase);
#endif

	d1(KPrintF("%s/%s/%ld: file=%08lx  name=<%s>  accessMode=%ld\n", __FILE__, __FUNC__, __LINE__, file, name, accessMode));
#if 1
	do	{
		struct FileHandle *fh;

		d1(KPrintF("%s/%s/%ld: file=%08lx\n", __FILE__, __FUNC__, __LINE__, file));
		if (BNULL == file)
			break;
		if (IsScalosProcess())
			break;
		if (IsInteractive(file))
			break;

		if (MODE_OLDFILE == accessMode)
			break;

		fh = BADDR(file);
		d1(KPrintF("%s/%s/%ld: fh_Type=%08lx\n", __FILE__, __FUNC__, __LINE__, fh->fh_Type));
		if (NULL == fh->fh_Type)
			break;
		if (NULL == PatchOpenBTree)
			break;

		if (!ScalosExamineBegin(&fib))
			break;
		d1(KPrintF("%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));

		if (!ScalosExamineFH(file, &fib))
			break;

		doi = ScalosAlloc(sizeof(struct DosOpenInfo));
		d1(KPrintF("%s/%s/%ld: doi=%08lx\n", __FILE__, __FUNC__, __LINE__, doi));
		if (NULL == doi)
			break;

		DoiCount++;

		doi->doi_fh = file;
		doi->doi_AccessMode = accessMode;
		doi->doi_DateBefore = *ScalosExamineGetDate(fib);

		if (BTreeInsert(PatchOpenBTree, (APTR) doi->doi_fh, doi))
			{
			// Node with this key already exists
			d1(KPrintF("%s/%s/%ld: key already exists\n", __FILE__, __FUNC__, __LINE__));
			BTreeHide(PatchOpenBTree, (APTR) doi->doi_fh);
			(void) BTreeInsert(PatchOpenBTree, (APTR) doi->doi_fh, doi);
			}

		d1(KPrintF("%s/%s/%ld: file=%08lx  DoiCount=%ld  BTreeNumberOfNodes=%ld  name=<%s>\n", \
			__FILE__, __FUNC__, __LINE__, file, DoiCount, BTreeNumberOfNodes(PatchOpenBTree), name));
		} while (0);

	ScalosExamineEnd(&fib);
#endif
	return file;
}
LIBFUNC_END


//	success = Close( file )
//	   D0             D1
//
//	BOOL Close(BPTR)
LIBFUNC_P2(ULONG, sca_Close,
	D1, BPTR, file,
	A6, struct DosLibrary *, DOSBase, 0)
{
	BOOL Success;
	struct DosOpenInfo *doi = NULL;
	T_ExamineData *fib = NULL;
	STRPTR FileName = NULL;
	BOOL DoUpdate = FALSE;

	d1(KPrintF("%s/%s/%ld: file=%08lx\n", __FILE__, __FUNC__, __LINE__, file));
	d1(KPrintF("%s/%s/%ld: IsInteractive=%ld\n", __FILE__, __FUNC__, __LINE__, IsInteractive(file)));
#if 1

	do	{
		struct FileHandle *fh;

		d1(KPrintF("%s/%s/%ld: file=%08lx\n", __FILE__, __FUNC__, __LINE__, file));
		if (BNULL == file)
			break;

		if (NULL == PatchOpenBTree)
			break;

		d1(KPrintF("%s/%s/%ld: BTreeNumberOfNodes=%ld\n", __FILE__, __FUNC__, __LINE__, BTreeNumberOfNodes(PatchOpenBTree)));

		doi = BTreeFind(PatchOpenBTree, (APTR) file);
		d1(KPrintF("%s/%s/%ld: doi=%08lx\n", __FILE__, __FUNC__, __LINE__, doi));
		if (NULL == doi)
			break;

		if (IsScalosProcess())
			break;
		if (IsInteractive(file))
			break;

		fh = BADDR(file);
		d1(KPrintF("%s/%s/%ld: fh_Type=%08lx\n", __FILE__, __FUNC__, __LINE__, fh->fh_Type));
		if (BNULL == fh->fh_Type)
			break;

		d1(kprintf("%s/%s/%ld: fh_Link=%08lx  fh_Port=%08lx  fh_Type=%08lx\n", __FILE__, __FUNC__, __LINE__, fh->fh_Link, fh->fh_Port, fh->fh_Type));

		if (!ScalosExamineBegin(&fib))
			break;

		d1(KPrintF("%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));

		if (!ScalosExamineFH(file, &fib))
			break;

		d1(KPrintF("%s/%s/%ld: doi_DateBefore: ds_Days=%ld  ds_Minute=%ld  ds_Tick=%ld\n", \
			__FILE__, __FUNC__, __LINE__, doi->doi_DateBefore.ds_Days,
                        doi->doi_DateBefore.ds_Minute, doi->doi_DateBefore.ds_Tick));
		d1(KPrintF("%s/%s/%ld: fib_FileName=<%s>  ds_Days=%ld  ds_Minute=%ld  ds_Tick=%ld\n", \
			__FILE__, __FUNC__, __LINE__, fib->fib_FileName, \
			fib->fib_Date.ds_Days, fib->fib_Date.ds_Minute, fib->fib_Date.ds_Tick));
		d1(KPrintF("%s/%s/%ld: doi_AccessMode=%ld\n", __FILE__, __FUNC__, __LINE__, doi->doi_AccessMode));
		d1(KPrintF("%s/%s/%ld: CompareDates=%08lx\n", __FILE__, __FUNC__, __LINE__, CompareDates(&fib->fib_Date, &doi->doi_DateBefore)));

		if (MODE_NEWFILE == doi->doi_AccessMode ||
			0 != CompareDates(ScalosExamineGetDate(fib), &doi->doi_DateBefore))
			{
			d1(KPrintF("%s/%s/%ld: doi_DateBefore: ds_Days=%ld  ds_Minute=%ld  ds_Tick=%ld\n", \
				__FILE__, __FUNC__, __LINE__, doi->doi_DateBefore.ds_Days,
				doi->doi_DateBefore.ds_Minute, doi->doi_DateBefore.ds_Tick));
			d1(KPrintF("%s/%s/%ld: fib_FileName=<%s>  ds_Days=%ld  ds_Minute=%ld  ds_Tick=%ld\n", \
				__FILE__, __FUNC__, __LINE__, fib->fib_FileName, \
				fib->fib_Date.ds_Days, fib->fib_Date.ds_Minute, fib->fib_Date.ds_Tick));

			FileName = AllocPathBuffer();
			d1(KPrintF("%s/%s/%ld: FileName=%08lx\n", __FILE__, __FUNC__, __LINE__, FileName));
			if (NULL == FileName)
				break;

			// retrieve full filename
			if (!NameFromFH(file, FileName, Max_PathLen))
				break;

			d1(KPrintF("%s/%s/%ld: FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

			DoUpdate = TRUE;
			}
		} while (0);

	if (doi)
		{
		//BOOL HideResult;
		/* HideResult = */ BTreeHide(PatchOpenBTree, (APTR) file);
		d1(KPrintF("%s/%s/%ld: HideResult=%ld  DoiCount=%ld  BTreeNumberOfNodes=%ld\n", __FILE__, __FUNC__, __LINE__, HideResult, DoiCount, BTreeNumberOfNodes(PatchOpenBTree)));
		}
#endif

#ifdef __AROS__
	Success = AROS_CALL1(BOOL, (*OldClose),
			     AROS_LDA(BPTR, file, D1),
			     struct DosLibrary *, DOSBase);
#else
	Success = CALLLIBFUNC_P2((*OldClose),
		D1, file, A6, DOSBase);
#endif

#if 1
	ScalosExamineEnd(&fib);

	if (DoUpdate)
		{
		d1(KPrintF("%s/%s/%ld: FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));
		PatchRefreshIcon(FileName, ICONTYPE_NONE);
		}

	if (FileName)
		FreePathBuffer(FileName);
#endif

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
LIBFUNC_END


//	success = CreateDir( name )
//	D0		      D1
//
//	BOOL CreateDir(STRPTR)
LIBFUNC_P2(BPTR, sca_CreateDir,
	D1, CONST_STRPTR, name,
	A6, struct DosLibrary *, DOSBase, 0)
{
	BPTR lock;

	d1(KPrintF("%s/%s/%ld: START Name=<%s>  Task=<%s>\n", __FILE__, __FUNC__, __LINE__, name, FindTask(NULL)->tc_Node.ln_Name));

	PatchAsyncWBUndoCreateDir(name);

#ifdef __AROS__
	lock = AROS_CALL1(BPTR, (*OldCreateDir),
			  AROS_LDA(CONST_STRPTR, name, D1),
			  struct DosLibrary *, DOSBase);
#else
	lock = CALLLIBFUNC_P2((*OldCreateDir),
		D1, name, A6, DOSBase);
#endif

	if (BNULL != lock)
		{
		PatchRefreshIcon(name, WBDRAWER);
		}

	d1(KPrintF("%s/%s/%ld: END lock=%ld\n", __FILE__, __FUNC__, __LINE__, lock));

	return lock;
}
LIBFUNC_END


static BOOL IsScalosProcess(void)
{
	struct Task *myTask = FindTask(NULL);

	d1(KPrintF("%s/%s/%ld: ln_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, myTask->tc_Node.ln_Name ? myTask->tc_Node.ln_Name : ""));

	return (BOOL) (NULL == myTask->tc_Node.ln_Name ||
		(0 == strncmp(myTask->tc_Node.ln_Name, "Scalos_", strlen("Scalos_"))) ||
		(0 == strcmp(myTask->tc_Node.ln_Name, "newdrawer.module")) ||
		(0 == strcmp(myTask->tc_Node.ln_Name, "delete.module")) ||
		(0 == strcmp(myTask->tc_Node.ln_Name, "rename.module")) );
}


static void PatchOpenDisposeData(void *data)
{
	d1(KPrintF("%s/%s/%ld: doi=%08lx\n", __FILE__, __FUNC__, __LINE__, data));
	DoiCount--;
	ScalosFree(data);
}


static void PatchOpenDisposeKey(void *key)
{
	// No-op since key is only handed as reference to IconScanEntry->ise_Fib.fib_FileName
	// and IconScanEntry nodes are managed within rilc_NonIconScanList
	(void) key;
}

static int PatchOpenCompare(const void *key1, const void *key2)
{
	return ((BPTR) key2) - ((BPTR) key1);
}


BOOL PatchInit(void)
{
	PatchOpenBTree = BTreeCreate(PatchOpenDisposeData, PatchOpenDisposeKey, PatchOpenCompare);
	d1(KPrintF("%s/%s/%ld: PatchOpenBTree=%08lx\n", __FILE__, __FUNC__, __LINE__, PatchOpenBTree));

	return (BOOL) (NULL != PatchOpenBTree);

}


void PatchCleanup(void)
{
	if (PatchOpenBTree)
		{
		BTreeDispose(PatchOpenBTree);
		PatchOpenBTree = NULL;
		}
}

#ifdef TEST_OPENWINDOWTAGLIST
static void ByteDump(const void *p, size_t Length)
{
	const unsigned char *Data = (const unsigned char *) p;
	unsigned long count;
	unsigned char Line[80];
	unsigned char *lp;

	lp = Line;
	for (count=0; count<Length; count++)
		{
		*lp++ = isprint(*Data) ? *Data : '.';
		KPrintF("%02x ", *Data++);
		if ((count+1) % 16 == 0)
			{
			*lp = '\0';
			lp = Line;
			KPrintF("\t%s\n", Line);
			}
		}

	*lp = '\0';
	while (count % 16 != 0)
		{
		KPrintF("   ");
		count++;
		}
	KPrintF("\t%s\n", Line);
}
#endif /* TEST_OPENWINDOWTAGLIST */


static void PatchAsyncWBUndoRename(CONST_STRPTR oldName, CONST_STRPTR newName)
{
	CONST_STRPTR ProcName = FindTask(NULL)->tc_Node.ln_Name;

	if ((0 == strcmp(ProcName, "ASYNCWB")) && (0 == IsIconName(oldName)))
		{
		BPTR dirLock = CurrentDir(BNULL);

		CurrentDir(dirLock);

		DoMethod(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_AddUndoEvent,
			UNDO_Rename,
			UNDOTAG_CopySrcDirLock, dirLock,
			UNDOTAG_CopySrcName, oldName,
			UNDOTAG_CopyDestName, newName,
			TAG_END
			);
		}
}


static void PatchAsyncWBUndoCreateDir(CONST_STRPTR name)
{
	CONST_STRPTR ProcName = FindTask(NULL)->tc_Node.ln_Name;

	if (0 == strcmp(ProcName, "AsyncWB - textinput process"))
		{
		BPTR dirLock = CurrentDir(BNULL);

		CurrentDir(dirLock);

		DoMethod(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_AddUndoEvent,
			UNDO_NewDrawer,
			UNDOTAG_IconDirLock, dirLock,
			UNDOTAG_IconName, name,
			TAG_END
			);
		}
}

