// Scalos_Helper.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <exec/nodes.h>
#include <intuition/classes.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <prefs/workbench.h>
#include <datatypes/iconobject.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>
#include <scalos/preferences.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <libraries/iffparse.h>

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
#include <proto/iffparse.h>
#include <proto/preferences.h>
#include <proto/graphics.h>
#include <proto/wb.h>

#include "wb39.h"
#include "wb39proto.h"


// from wb39.c
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct Library *IconobjectBase;
extern T_UTILITYBASE UtilityBase;
extern struct ScalosBase *ScalosBase;
extern struct IntuitionBase *IntuitionBase;
extern struct Library *LayersBase;
extern struct GfxBase *GfxBase;
extern struct Library *IconBase;
extern struct Library *IFFParseBase;

extern struct ScaRootList *rList;
extern WORD ScalosPrefs_IconOffsets[4];

extern ULONG DefaultStackSize;
extern ULONG fVolumeGauge;
extern ULONG fTitleBar;
extern ULONG TypeRestartTime;		// Keyboard restart delay in s
extern BOOL NewIconsSupport;
extern BOOL ColorIconsSupport;


static void HandleWBNC(UWORD *Data, size_t Length);
static BOOL SetViewMode(struct ScaWindowStruct *swi, long ViewMode);
static BOOL IconsOverlap(const struct ScaIconNode *in1, const struct ScaIconNode *in2);


struct Library *PreferencesBase = NULL;
#ifdef __amigaos4__
struct PreferencesIFace *IPreferences = NULL;
#endif

ULONG MenuOpenParentWindow(struct ScaWindowStruct *swi)
{
	ULONG Result = RETURN_OK;
	BPTR parentLock;

	if ((BPTR)NULL == swi->ws_Lock)
		return ERROR_OBJECT_NOT_FOUND;

	parentLock = ParentDir(swi->ws_Lock);
	if (parentLock)
		{
		char Path[200];

		if(NameFromLock(parentLock, Path, sizeof(Path)))
			{
			if (!SCA_OpenDrawerByName(Path, NULL))
				Result = ERROR_OBJECT_NOT_FOUND;
			}

		UnLock(parentLock);
		}

	return Result;
}


ULONG MenuUnselectAll(struct ScaWindowStruct *swi)
{
	struct ScaWindowTask *wt = swi->ws_WindowTask;
	struct ScaIconNode *icon;

	d1(kprintf(__FUNC__ "/%ld: swi=%08lx\n", __LINE__, swi);)

	ObtainSemaphore(wt->wt_IconSemaphore);

	for (icon=wt->wt_IconList; icon; icon = (struct ScaIconNode *) icon->in_Node.mln_Succ)
		{
		SelectIcon(swi, icon, FALSE);
		}

	ReleaseSemaphore(wt->wt_IconSemaphore);

	return RETURN_OK;
}



static BOOL SetViewMode(struct ScaWindowStruct *swi, long ViewMode)
{
	Object *newIconWindow;

	if (swi->ws_Viewmodes == ViewMode)
		return TRUE;

	newIconWindow = (Object *) DoMethod(swi->ws_WindowTask->mt_MainObject,
		SCCM_IconWin_NewViewMode, ViewMode);
	if (newIconWindow)
		{
		DisposeObject(swi->ws_WindowTask->mt_MainObject);
		swi->ws_WindowTask->mt_MainObject = newIconWindow;
		}

	DoMethod(swi->ws_WindowTask->mt_WindowObject,
		SCCM_Window_SetInnerSize);

	return UpdateScalosWindow(swi);
}


ULONG MenuSetViewModeByIcon(struct ScaWindowStruct *swi)
{
	if (SetViewMode(swi, SCAV_ViewModes_Icon))
		return RETURN_OK;

	return ERROR_OBJECT_NOT_FOUND;
}


ULONG MenuSetViewModeByName(struct ScaWindowStruct *swi)
{
	if (SetViewMode(swi, SCAV_ViewModes_Name))
		return RETURN_OK;

	return ERROR_OBJECT_NOT_FOUND;
}


ULONG MenuSetViewModeBySize(struct ScaWindowStruct *swi)
{
	if (SetViewMode(swi, SCAV_ViewModes_Size))
		return RETURN_OK;

	return ERROR_OBJECT_NOT_FOUND;
}

ULONG MenuSetViewModeByType(struct ScaWindowStruct *swi)
{
	if (SetViewMode(swi, SCAV_ViewModes_Size))
		return RETURN_OK;

	return ERROR_OBJECT_NOT_FOUND;
}

ULONG MenuSetViewModeByDate(struct ScaWindowStruct *swi)
{
	if (SetViewMode(swi, SCAV_ViewModes_Date))
		return RETURN_OK;

	return ERROR_OBJECT_NOT_FOUND;
}


ULONG MenuShowAllFiles(struct ScaWindowStruct *swi)
{
	ULONG showAllFiles = FALSE;

	GetAttr(SCCA_IconWin_ShowType, swi->ws_WindowTask->mt_MainObject, &showAllFiles);
	if (showAllFiles)
		return RETURN_OK;

	DoMethod(swi->ws_WindowTask->mt_MainObject,
		SCCM_IconWin_MenuCommand, "showallfiles",
		NULL, 0);

	return MenuUpdateScalosWindow(swi);
}


static void HandleWBNC(UWORD *Data, size_t Length)
{
	const struct WorkbenchPrefs *prefs = (const struct WorkbenchPrefs *) Data;

	d1(kprintf(__FUNC__ "/%ld: Data %04lx %04lx %04lx %04lx %04lx %04lx %04lx %04lx\n", \
		__LINE__, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6], Data[7]));

	DefaultStackSize = prefs->wbp_DefaultStackSize
	d1(kprintf(__FUNC__ "/%ld: DefaultStackSize=%lu\n", __LINE__, DefaultStackSize));

	if (DefaultStackSize < 4096)
		DefaultStackSize = 4096;

	TypeRestartTime = prefs->wbp_TypeRestartTime;

	NewIconsSupport = prefs->wbp_NewIconsSupport;
	ColorIconsSupport = prefs->wbp_ColorIconSupport;

	if (Length >= wpi_NoVolumeGauge)
		fVolumeGauge = 0 == Data[wpi_NoVolumeGauge];

	if (Length >= wpi_NoTitleBar)
		fTitleBar = 0 == Data[wpi_NoTitleBar];

}


// +dm+ 20010518 Start
static void HandleWBHD(char *Data, size_t Length)
{
	if(Data && Length)
	{
		WorkbenchControl(NULL, 
			WBCTRLA_AddHiddenDeviceName, (ULONG) Data, 
			TAG_DONE);
	}
}
// +dm+ 20010518 end


BOOL ParseWBPrefs(CONST_STRPTR filename)
{
	struct IFFHandle    *iff;
	LONG                error;

	#define NUMPAIRS 2l
	ULONG            pairs[NUMPAIRS * 2] = {ID_PREF, ID_WBNC, ID_PREF, ID_WBHD};

	d1(kprintf(__FUNC__ "/%ld:  filename=<%s>\n", __LINE__, filename));

	if( iff = AllocIFF() )
		{
		if( iff->iff_Stream = (ULONG) Open((STRPTR) filename, MODE_OLDFILE ) )
			{
			InitIFFasDOS( iff );

			if( !(error = OpenIFF( iff, IFFF_READ ) ) )
				{
				// +dm+ 20010518 Changed to parse each chunk as we find it rather than using collections
				if( !(error = CollectionChunks( iff, (LONG*)pairs, NUMPAIRS) ) )
					{
					StopOnExit( iff, ID_PREF, ID_FORM );    // +dm+ Changed 20010518

					while(TRUE)
						{
						if( ( error = ParseIFF( iff, IFFPARSE_SCAN ) ) == IFFERR_EOC )
							{
							struct CollectionItem *ci;

							ci = FindCollection( iff, ID_PREF, ID_WBNC);
							while( ci )
								{
								HandleWBNC(ci->ci_Data, ci->ci_Size);
								ci = ci->ci_Next;
								}

							// +dm+ 20010518 start
							ci = FindCollection( iff, ID_PREF, ID_WBHD);
							while( ci )
								{
								HandleWBHD(ci->ci_Data, ci->ci_Size);
								ci = ci->ci_Next;
								}
							// +dm+ 20010518 end

							}
						else
							break;
						}
					}

				CloseIFF( iff );
				}
			Close( (BPTR) iff->iff_Stream );
			}
		FreeIFF( iff );
		}

	if (ScalosBase->scb_LibNode.lib_Version >= 40)
		{
		ULONG IconTypes = 0;

		if (NewIconsSupport)
			IconTypes |= IDTV_IconType_NewIcon;
		if (ColorIconsSupport)
			IconTypes |= IDTV_IconType_ColorIcon;

		SCA_ScalosControl(NULL,
			SCALOSCTRLA_SetDefaultStackSize, DefaultStackSize,
			SCALOSCTRLA_SetSupportedIconTypes, IconTypes,
			TAG_END);
		}

	return TRUE;
}


BOOL ReadScalosPrefs(void)
{
	APTR PrefsHandle;
	BOOL Result = FALSE;

	d1(kprintf(__FUNC__ "/%ld\n", __LINE__));

	PreferencesBase = OpenLibrary("preferences.library", 39);
	if (NULL == PreferencesBase)
		return FALSE;
#ifdef __amigaos4__
	IPreferences = (struct PreferencesIFace *)GetInterface((struct Library *)PreferencesBase, "main", 1, NULL);
	if (NULL == IPreferences)
		{
		CloseLibrary(PreferencesBase);
		return FALSE;
		}
#endif

	d1(kprintf(__FUNC__ "/%ld\n", __LINE__));

	PrefsHandle = AllocPrefsHandle("Scalos");
	if (PrefsHandle)
		{
		d1(kprintf(__FUNC__ "/%ld\n", __LINE__));

		ReadPrefsHandle(PrefsHandle, "ENV:Scalos/Scalos.prefs");

		if (sizeof(ScalosPrefs_IconOffsets) == GetPreferences(PrefsHandle, 
			MAKE_ID('M','A','I','N'),
			SCP_IconOffsets,
			ScalosPrefs_IconOffsets, sizeof(ScalosPrefs_IconOffsets)))
			{
			d1(kprintf(__FUNC__ "/%ld  Icon Offsets = %ld %ld %ld %ld\n", \
				__LINE__, (LONG) ScalosPrefs_IconOffsets[0], \
				(LONG) ScalosPrefs_IconOffsets[1], \
				(LONG) ScalosPrefs_IconOffsets[2], \
				(LONG) ScalosPrefs_IconOffsets[3]));
			Result = TRUE;
			}

		FreePrefsHandle(PrefsHandle);
		}

#ifdef __amigaos4__
	DropInterface((struct Interface *)IPreferences);
#endif
	CloseLibrary(PreferencesBase);

	if (ScalosBase->scb_LibNode.lib_Version >= 39 && ScalosBase->scb_LibNode.lib_Revision >= 234)
		{
		SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetDefaultStackSize, (ULONG) &DefaultStackSize,
			TAG_END);
		}

	d1(kprintf(__FUNC__ "/%ld  Result=%ld\n", __LINE__, Result));

	return Result;
}


// !!! wt->wt_IconSemaphore WILL STAY LOCKED IF ICON FOUND !!!
struct ScaIconNode *GetIconByName(struct ScaWindowStruct *swi, CONST_STRPTR IconName)
{
	struct ScaWindowTask *wt = swi->ws_WindowTask;
	struct ScaIconNode *icon, *iconFound=NULL;
	char wBuffer[256];

	if (swi->ws_Lock)
		NameFromLock(swi->ws_Lock, wBuffer, sizeof(wBuffer));

	ObtainSemaphoreShared(wt->wt_IconSemaphore);

	for (icon=wt->wt_IconList; (NULL == iconFound) && icon; icon = (struct ScaIconNode *) icon->in_Node.mln_Succ)
		{
		char *Name = NULL;

		GetAttr(IDTA_Text, icon->in_Icon, (APTR) &Name);

		if (Name && 0 == Stricmp((STRPTR) IconName, Name))
			{
			iconFound = icon;
			}
		}

	if (NULL == iconFound)
		ReleaseSemaphore(wt->wt_IconSemaphore);

	return iconFound;
}


// !! SCALOS WINDOW LIST WILL STAY LOCKED IF FOUND (Result != NULL) !!
struct ScaWindowStruct *FindWindowByLock(BPTR xLock)
{
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	struct ScaWindowStruct *Result = NULL;

	d1(kprintf(__FUNC__ "/%ld: xLock=%08lx\n", __LINE__, xLock));

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; NULL == Result && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d1(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Same=%ld\n", __LINE__, \
				swi, swi->ws_Lock, SameLock(swi->ws_Lock, xLock)));

			if (LOCK_SAME == SameLock(swi->ws_Lock, xLock))
				{
				d1(kprintf(__FUNC__ "/%ld: swi=%08lx\n", __LINE__, swi));
				Result = swi;
				}
			}

		if (NULL == Result)
			{
			d1(kprintf(__FUNC__ "/%ld: vor SCA_UnLockWindowList\n", __LINE__));
			SCA_UnLockWindowList();
			}
		}

	d1(kprintf(__FUNC__ "/%ld: Result=%08lx\n", __LINE__, Result));

	return Result;
}


// Return open IconWindow belonging to <sIcon>
// !! SCALOS WINDOW LIST WILL STAY LOCKED IF FOUND (Result != NULL) !!
struct ScaWindowStruct *GetIconNodeOpenWindow(BPTR dirLock, struct ScaIconNode *sIcon)
{
	struct ScaWindowStruct *swiFound = NULL;
	char *IconName = NULL;
	BPTR oldDir;
	BPTR fLock;

	GetAttr(IDTA_Text, sIcon->in_Icon, (APTR) &IconName);
	if (NULL == IconName)
		return FALSE;

	oldDir = CurrentDir(dirLock);

	fLock = Lock(IconName, ACCESS_READ);
	if (fLock)
		{
		struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);

		if (swList)
			{
			struct ScaWindowStruct *swi;

			for (swi=swList->wl_WindowStruct; !swiFound && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
				{
				d1(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Flags=%08lx\n", __LINE__, \
					swi, swi->ws_Lock, swi->ws_Flags));

				if (swi->ws_Lock)
					{
					if (LOCK_SAME == SameLock(swi->ws_Lock, fLock))
						swiFound = swi;
					}
				}

			if (NULL == swiFound)
				{
				d1(kprintf(__FUNC__ "/%ld: vor SCA_UnLockWindowList\n", __LINE__));
				SCA_UnLockWindowList();
				}
			}
		UnLock(fLock);
		}

	CurrentDir(oldDir);

	return swiFound;
}


void MakeIconVisible(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon)
{
	struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;
	LONG deltaX = 0, deltaY = 0;
	LONG winRight, winBottom;
	WORD iconLeft, iconTop, iconRight, iconBottom;
	WORD winLeft, winTop;

	iconLeft = gg->BoundsLeftEdge - swi->ws_xoffset;
	iconRight = iconLeft + gg->BoundsWidth - 1;
	iconTop = gg->BoundsTopEdge - swi->ws_yoffset;
	iconBottom = iconTop + gg->BoundsHeight - 1;

	winLeft = 0;
	winTop = 0;

	GetAttr(SCCA_IconWin_InnerWidth, swi->ws_WindowTask->mt_MainObject, (ULONG *) &winRight);
	GetAttr(SCCA_IconWin_InnerHeight, swi->ws_WindowTask->mt_MainObject, (ULONG *) &winBottom);

	d1(kprintf(__FUNC__ "/%ld: iconLeft=%ld  iconRight=%ld  iconTop=%ld  iconBottom=%ld\n", \
		__LINE__, iconLeft, iconRight, iconTop, iconBottom);)
	d1(kprintf(__FUNC__ "/%ld: winLeft=%ld  winRight=%ld  winTop=%ld  winBottom=%ld\n", \
		__LINE__, winLeft, winRight, winTop, winBottom);)

	if (iconLeft < winLeft)
		{
		// must move Right
		deltaX = iconLeft - winLeft;
		}
	else if (iconRight > winRight)
		{
		// must move iconLeft
		deltaX = iconRight - winRight;
		}

	if (iconTop < winTop)
		{
		// must move DOWN
		deltaY = iconTop - winTop;
		}
	else if (iconBottom > winBottom)
		{
		// must move UP
		deltaY = iconBottom - winBottom;
		}

	d1(kprintf(__FUNC__ "/%ld: deltaX=%ld  deltaY=%ld\n", __LINE__, deltaX, deltaY);)

	if (deltaX != 0 || deltaY != 0)
		{
		DoMethod(swi->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_DeltaMove, deltaX, deltaY);

		DoMethod(swi->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
		}
}


void MoveIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, LONG x, LONG y)
{
	struct ScaIconNode *in;
	struct Region *oldClipRegion;
	struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;
	long DeltaX, DeltaY;

	d1(kprintf(__FUNC__ "/%ld: swi=%08lx  sIcon=%08lx  x=%ld  y=%ld\n", __LINE__, swi, sIcon, x, y);)

	oldClipRegion = (struct Region *) DoMethod(swi->ws_WindowTask->mt_WindowObject,
		SCCM_Window_InitClipRegion);

	d1(kprintf(__FUNC__ "/%ld: ClipRegion=%08lx\n", __LINE__, swi->ws_Window->WLayer->ClipRegion);)

	// Erase original Icon 
	EraseRect(swi->ws_Window->RPort,
		swi->ws_Window->BorderLeft + gg->BoundsLeftEdge - swi->ws_xoffset,
		swi->ws_Window->BorderTop  + gg->BoundsTopEdge - swi->ws_yoffset,
		swi->ws_Window->BorderLeft + gg->BoundsLeftEdge + gg->BoundsWidth - 1 - swi->ws_xoffset,
		swi->ws_Window->BorderTop  + gg->BoundsTopEdge + gg->BoundsHeight - 1 - swi->ws_yoffset);

	// now redraw all icons overlapping the erased one
	for (in=swi->ws_WindowTask->wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (in != sIcon && IconsOverlap(sIcon, in))
			{
			DoMethod(swi->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_DrawIcon, in->in_Icon);
			}
		}

	DeltaX = x - gg->LeftEdge;
	DeltaY = y - gg->TopEdge;

	SetAttrs(sIcon->in_Icon,
		GA_Left, x,
		GA_Top, y,
		TAG_END);

	gg->BoundsLeftEdge += DeltaX;
	gg->BoundsTopEdge += DeltaY;

	DoMethod(swi->ws_WindowTask->mt_MainObject,
		SCCM_IconWin_DrawIcon, sIcon->in_Icon);

	DoMethod(swi->ws_WindowTask->mt_WindowObject,
		SCCM_Window_RemClipRegion, oldClipRegion);

	DoMethod(swi->ws_WindowTask->mt_MainObject,
		SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
}


void SelectIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, BOOL Selected)
{
	struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;

	d1(kprintf(__FUNC__ "/%ld: swi=%08lx  sIcon=%08lx  Selected=%ld\n", __LINE__, swi, sIcon, Selected);)

	if ( (!Selected && (gg->Flags & GFLG_SELECTED)) || (Selected && !(gg->Flags & GFLG_SELECTED)))
		{
		struct Region *oldClipRegion;

		oldClipRegion = (struct Region *) DoMethod(swi->ws_WindowTask->mt_WindowObject,
			SCCM_Window_InitClipRegion);

		d1(kprintf(__FUNC__ "/%ld: ClipRegion=%08lx\n", __LINE__, swi->ws_Window->WLayer->ClipRegion);)

		SetAttrs(sIcon->in_Icon,
			GA_Selected, Selected,
			TAG_END);

		if (Selected && NULL == gg->SelectRender)
			{
			DoMethod(swi->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_LayoutIcon, sIcon->in_Icon,
				IOLAYOUTF_SelectedImage);
			}
		else if (!Selected && NULL == gg->GadgetRender)
			{
			DoMethod(swi->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_LayoutIcon, sIcon->in_Icon,
				IOLAYOUTF_NormalImage);
			}

		DoMethod(swi->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_DrawIcon, sIcon->in_Icon);

		DoMethod(swi->ws_WindowTask->mt_WindowObject,
			SCCM_Window_RemClipRegion, oldClipRegion);
		}
}


// special names: "root" "active"
struct ScaWindowStruct *FindWindowByName(CONST_STRPTR WinName)
{
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	struct ScaWindowStruct *Result = NULL;
	BOOL FindActive = 0 == Stricmp((STRPTR) WinName, "active");
	BOOL FindRoot = 0 == Stricmp((STRPTR) WinName, "root");
	BPTR wnLock;

	if (!FindActive && !FindRoot)
		wnLock = Lock((STRPTR) WinName, ACCESS_READ);
	else
		wnLock = (BPTR)NULL;

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; NULL == Result && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d1(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Flags=%08lx\n", __LINE__, \
				swi, swi->ws_Lock, swi->ws_Flags));

			if (swi->ws_Window)
				{
				if (FindActive)
					{
					if (IntuitionBase->ActiveWindow == swi->ws_Window)
						Result = swi;
					}
				else
					{
					if (swi->ws_Lock && !FindRoot)
						{
						if (LOCK_SAME == SameLock(swi->ws_Lock, wnLock))
							{
							Result = swi;
							}
						}
					else
						{
						// root window
						if (FindRoot)
							Result = swi;
						}
					}
				}
			}

		d1(kprintf(__FUNC__ "/%ld: vor SCA_UnLockWindowList\n", __LINE__));
		SCA_UnLockWindowList();
		}

	if (wnLock)
		UnLock(wnLock);

	return Result;
}


BOOL CloseScalosWindow(struct ScaWindowStruct *swi)
{
	struct MsgPort *ReplyPort = CreateMsgPort();
	struct Message *Msg;

	if (NULL == ReplyPort)
		{
		d1(kprintf(__FUNC__ "/%ld: vor SCA_UnLockWindowList\n", __LINE__));
		SCA_UnLockWindowList();
		return FALSE;
		}

	Msg = (struct Message *) SCA_AllocMessage(MTYP_CloseWindow, 0);
	if (NULL == Msg)
		{
		d1(kprintf(__FUNC__ "/%ld: vor SCA_UnLockWindowList\n", __LINE__));
		SCA_UnLockWindowList();
		DeleteMsgPort(ReplyPort);

		return FALSE;
		}

	Msg->mn_ReplyPort = ReplyPort;

	d1(kprintf(__FUNC__ "/%ld: vor SCA_UnLockWindowList\n", __LINE__));
	SCA_UnLockWindowList();

	PutMsg(swi->ws_MessagePort, Msg);

	WaitPort(ReplyPort);
	while (Msg = GetMsg(ReplyPort))
		{
		SCA_FreeMessage((struct ScalosMessage *) Msg);
		}

	DeleteMsgPort(ReplyPort);

	return TRUE;
}


BOOL UpdateScalosWindow(struct ScaWindowStruct *swi)
{
	struct MsgPort *ReplyPort = CreateMsgPort();
	struct Message *Msg;

	if (NULL == ReplyPort)
		return FALSE;

	Msg = (struct Message *) SCA_AllocMessage(MTYP_Update, 0);
	if (NULL == Msg)
		{
		DeleteMsgPort(ReplyPort);

		return FALSE;
		}

	Msg->mn_ReplyPort = ReplyPort;

	PutMsg(swi->ws_MessagePort, Msg);

	WaitPort(ReplyPort);
	while (Msg = GetMsg(ReplyPort))
		{
		SCA_FreeMessage((struct ScalosMessage *) Msg);
		}

	DeleteMsgPort(ReplyPort);

	return TRUE;
}


struct ScaWindowStruct *GetNextWindow(struct ScaWindowStruct *swi)
{
	struct ScaWindowStruct *Result = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ;

	if (NULL == Result)
		{
		// no successor, return first window in list
		struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);

		if (swList)
			{
			Result = swList->wl_WindowStruct;

			SCA_UnLockWindowList();
			}
		}

	return Result;
}


struct ScaWindowStruct *GetPrevWindow(struct ScaWindowStruct *swi)
{
	struct ScaWindowStruct *Result = (struct ScaWindowStruct *) swi->ws_Node.mln_Pred;

	if (NULL == Result)
		{
		// no predecessor, return last window in list
		struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);

		if (swList)
			{
			Result = swList->wl_WindowStruct;
			while (Result->ws_Node.mln_Succ)
				Result = (struct ScaWindowStruct *) Result->ws_Node.mln_Succ;

			SCA_UnLockWindowList();
			}
		}

	return Result;
}


BOOL DeleteDirectory(CONST_STRPTR Path)
{
	BPTR dirLock;
	BOOL Success = TRUE;

	d1(kprintf(__FUNC__ "/%ld: Path=<%s>\n", __LINE__, Path);)

	dirLock = Lock((STRPTR) Path, ACCESS_READ);
	if (dirLock)
		{
		struct FileInfoBlock *fib = AllocDosObject(DOS_FIB, TAG_END);

		if (fib)
			{
			BPTR oldDir;

			Examine(dirLock, fib);

			d1(kprintf(__FUNC__ "/%ld: Name=<%s>  Type=%ld\n", __LINE__, fib->fib_FileName, fib->fib_DirEntryType);)

			switch (fib->fib_DirEntryType)
				{
			case ST_ROOT:
			case ST_USERDIR:
			case ST_LINKDIR:
				oldDir = CurrentDir(dirLock);

				while (Success && ExNext(dirLock, fib))
					{
					d1(kprintf(__FUNC__ "/%ld: Name=<%s>  Type=%ld\n", \
						__LINE__, fib->fib_FileName, fib->fib_DirEntryType);)
					Success = DeleteDirectory(fib->fib_FileName);
					};

				if (ERROR_NO_MORE_ENTRIES != IoErr())
					Success = FALSE;

				CurrentDir(oldDir);

				d1(kprintf(__FUNC__ "/%ld: Success=%ld\n", __LINE__, Success);)
				break;
				}

			FreeDosObject(DOS_FIB, fib);
			}
		else
			Success = FALSE;

		UnLock(dirLock);

		if (Success)
			{
			if (!DeleteFile((STRPTR) Path))
				Success = FALSE;

			d1(kprintf(__FUNC__ "/%ld: Success=%ld\n", __LINE__, Success);)
			}
		}
	else
		Success = FALSE;

	d1(kprintf(__FUNC__ "/%ld: Success=%ld\n", __LINE__, Success);)

	return Success;
}


BOOL RemScalosIcon(BPTR Lock, CONST_STRPTR Name)
{
	struct ScaWindowStruct *swi = FindWindowByLock(Lock);

	if (swi)
		{
		DoMethod(swi->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_RemIcon, Lock, Name);

		SCA_UnLockWindowList();
		}

	return TRUE;
}


BOOL isIconSelected(const struct ScaIconNode *icon)
{
	struct Gadget *gg = (struct Gadget *) icon->in_Icon;

	return (BOOL) ((gg->Flags & GFLG_SELECTED) ? TRUE : FALSE);
}


struct ScaRootList *GetScalosRootList(void)
{
	static struct ScaRootList rl;
	struct ScaRootList *Result;

	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);

	if (swList && swList->wl_WindowStruct && swList->wl_WindowStruct->ws_WindowTask
		&& swList->wl_WindowStruct->ws_WindowTask->mt_MainObject)
		{
		struct ScaRootList *prl = (struct ScaRootList *) swList->wl_WindowStruct->ws_WindowTask->mt_MainObject;

		rl = *prl;
		Result = &rl;
		}
	else
		Result = NULL;

	d1(kprintf(__FUNC__ "/%ld: rl_WindowTask=%08lx  rl_InternInfos=%08lx\n", \
		__LINE__, rl.rl_WindowTask, rl.rl_internInfos));

	SCA_UnLockWindowList();

	return Result;
}


ULONG MenuNewDrawer(CONST_STRPTR Name)
{
	BPTR dirLock;
	ULONG Result = RETURN_OK;

	dirLock = CreateDir((STRPTR) Name);
	if (dirLock)
		{
		// Drawer created Ok, new create default drawer icon
		struct DiskObject *diskObj = GetDefDiskObject(WBDRAWER);

		if (diskObj)
			{
			if (!PutDiskObject((STRPTR) Name, diskObj))
				Result = IoErr();

			FreeDiskObject(diskObj);
			}
		else
			Result = IoErr();

		UnLock(dirLock);
		}
	else
		Result = IoErr();

	return Result;
}


ULONG MenuUpdateScalosWindow(struct ScaWindowStruct *swi)
{
	if (!UpdateScalosWindow(swi))
		return ERROR_NO_FREE_STORE;

	return RETURN_OK;
}


ULONG MenuCloseScalosWindow(struct ScaWindowStruct *swi)
{
	if (CloseScalosWindow(swi))
		return RETURN_OK;

	return ERROR_OBJECT_NOT_FOUND;
}


static BOOL IconsOverlap(const struct ScaIconNode *in1, const struct ScaIconNode *in2)
{
	const struct ExtGadget *gg1 = (struct ExtGadget *) in1->in_Icon;
	const struct ExtGadget *gg2 = (struct ExtGadget *) in2->in_Icon;

	if (gg1->BoundsLeftEdge < (gg2->BoundsLeftEdge + gg2->BoundsWidth)
		&& (gg1->BoundsLeftEdge + gg1->BoundsWidth) >= gg2->BoundsLeftEdge
		&& gg1->BoundsTopEdge < (gg2->BoundsTopEdge + gg2->BoundsHeight)
		&& (gg1->BoundsTopEdge + gg1->BoundsHeight) >= gg2->BoundsTopEdge)
			return TRUE;

	return FALSE;
}


// returns correct name for all icon types.
// guaranteed to NEVER return NULL.
CONST_STRPTR GetIconName(struct ScaIconNode *in)
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


struct ScaWindowStruct *WaitOpen(struct ScaWindowStruct *ws)
{
	d1(kprintf(__FUNC__ "/%ld: ws=%08lx\n", __LINE__, ws));
	if (ws)
		{
		ULONG Reading = FALSE;

		// if ws is non-NULL, wait until icon reading is finished
		do	{
			GetAttr(SCCA_IconWin_Reading, ws->ws_WindowTask->mt_MainObject, &Reading);

			d1(kprintf(__FUNC__ "/%ld: Reading=%ld\n", __LINE__, Reading));
			if (Reading)
				Delay(20);	// Wait 400ms
			} while (Reading);
		}

	return ws;
}
