// Undo.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/asl.h>
#include <dos/dos.h>

#define	__USE_SYSBASE

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/wb.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

struct UndoStep
	{
	struct Node ust_Node;
	struct List ust_EventList;	// List of events in this step
	ULONG ust_EventCount;
	STRPTR ust_Description;
	Object *ust_FileTransObj;	// valid during Undo and Redo processing
	};

struct UndoProcArg
	{
	struct UndoStep *upa_UndoStep;
	struct internalScaWindowTask *upa_iwt;
	};

//----------------------------------------------------------------------------

// local functions

static struct UndoStep *UndoCreateStep(void);
static void UndoDisposeStep(struct UndoStep *ust);
static struct UndoEvent *UndoCreateEvent(void);
static void UndoDisposeEvent(struct UndoEvent *uev);
static SAVEDS(LONG) UndoDummyFunc(struct Hook *hook, APTR object, struct UndoEvent *uev);
static void RedoCleanup(void);
static SAVEDS(ULONG) UndoTask(struct UndoProcArg *upa, struct SM_RunProcess *msg);
static SAVEDS(ULONG) RedoTask(struct UndoProcArg *upa, struct SM_RunProcess *msg);

static BOOL UndoAddCopyMoveEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddChangeIconPosEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddSnapshotEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddCleanupEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddRenameEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddRelabelEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddDeleteEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddSizeWindowEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddNewDrawerEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddLeaveOutPutAwayEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddSetProtectionEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddSetCommentEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddSetToolTypesEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddChangeIconObjectEvent(struct UndoEvent *uev, struct TagItem *TagList);
static BOOL AddCloseWindowEvent(struct UndoEvent *uev, struct TagItem *TagList);

static SAVEDS(void) UndoDisposeCopyMoveData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeNewDrawerData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeIconData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeCleanupData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeSnapshotData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeSizeWindowData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeSetProtectionData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeSetCommentData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeSetToolTypesData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeChangeIconObjectData(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(void) UndoDisposeCloseWindowData(struct Hook *hook, APTR object, struct UndoEvent *uev);

static SAVEDS(LONG) UndoCopyEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoMoveEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoCopyEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoMoveEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoCreateLinkEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoChangeIconPosEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoChangeIconPosEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoCleanupEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoLeaveOutEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoLeaveOutEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoCleanupEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoSnapshotEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoSnapshotPosEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoRenameEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoRenameEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoRelabelEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoRelabelEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoSizeWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoSizeWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoNewDrawerEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoNewDrawerEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoSetProtectionEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoSetProtectionEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoSetCommentEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoSetCommentEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoSetToolTypesEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoSetToolTypesEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoChangeIconObjectEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoChangeIconObjectEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) UndoCloseWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);
static SAVEDS(LONG) RedoCloseWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev);

static BOOL MoveIconTo(CONST_STRPTR DirName, CONST_STRPTR IconName, LONG x, LONG y);
static STRPTR UndoBuildIconName(CONST_STRPTR ObjName);
static struct internalScaWindowTask *UndoFindWindowByDir(CONST_STRPTR DirName);
static struct internalScaWindowTask *UndoFindWindowByWindowTask(const struct ScaWindowTask *iwt);
static const struct UndoCleanupIconEntry *UndoFindCleanupIconEntry(const struct UndoEvent *uev, const struct ScaIconNode *in);
static struct ScaIconNode *UndoFindIconByName(struct internalScaWindowTask *iwt, CONST_STRPTR IconName);
static BOOL UndoSnapshotIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL UndoUnsnapshotIcon(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in, BOOL SaveIcon);
static BOOL UndoFindWindowAndIcon(CONST_STRPTR DirName, CONST_STRPTR IconName,
	struct internalScaWindowTask **iwt, struct ScaIconNode **in);

//----------------------------------------------------------------------------

// local Data items

//----------------------------------------------------------------------------

// public data items

struct List globalUndoList;		// global Undo list for all file-related actions
SCALOSSEMAPHORE UndoListListSemaphore;	// Semaphore to protect globalUndoList
static ULONG UndoCount;			// Number of entries in globalUndoList

struct List globalRedoList;		// global Redo list for all file-related actions
SCALOSSEMAPHORE RedoListListSemaphore;	// Semaphore to protect globalRedoList

//----------------------------------------------------------------------------

void UndoCleanup(void)
{
	struct UndoStep *ust;

	ScalosObtainSemaphore(&UndoListListSemaphore);

	while ((ust = (struct UndoStep *) RemHead(&globalUndoList)))
		{
		UndoCount--;
		UndoDisposeStep(ust);
		}
	ScalosReleaseSemaphore(&UndoListListSemaphore);

	RedoCleanup();
}

//----------------------------------------------------------------------------

BOOL UndoAddEvent(struct internalScaWindowTask *iwt, enum ScalosUndoType type, ULONG FirstTag, ...)
{
	BOOL Success;
	struct TagItem *TagList;

	va_list args;

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);
	Success = UndoAddEventTagList(iwt, type, TagList);

	va_end(args);

	if (TagList)
		FreeTagItems(TagList);

	return Success;
}

//----------------------------------------------------------------------------

BOOL UndoAddEventTagList(struct internalScaWindowTask *iwt, enum ScalosUndoType type, struct TagItem *TagList)
{
	if (TagList)
		{
		struct UndoEvent *uev;

		uev = UndoCreateEvent();
		if (uev)
			{
			struct UndoStep *ustMulti;
			struct UndoStep *ust;

			ust = ustMulti = (struct UndoStep *) GetTagData(UNDOTAG_UndoMultiStep, (ULONG) NULL, TagList);
			d1(kprintf("%s/%s/%ld: ust=%08lx\n", __FILE__, __FUNC__, __LINE__, ust));
			if (NULL == ust)
				{
				ust = (struct UndoStep *) UndoBeginStep();
				}

			d1(kprintf("%s/%s/%ld: ust=%08lx\n", __FILE__, __FUNC__, __LINE__, ust));

			if (ust)
				{
				BOOL Success = FALSE;

				RedoCleanup();

				uev->uev_Type = type;

				uev->uev_CustomAddHook = (struct Hook *) GetTagData(UNDOTAG_CustomAddHook, (ULONG) NULL, TagList);

				uev->uev_NewPosX = GetTagData(UNDOTAG_IconPosX, NO_ICON_POSITION_SHORT, TagList);
				uev->uev_NewPosY = GetTagData(UNDOTAG_IconPosY, NO_ICON_POSITION_SHORT, TagList);

				d1(kprintf("%s/%s/%ld: uev=%08lx  uev_Type=%ld\n", __FILE__, __FUNC__, __LINE__, uev, uev->uev_Type));
				d1(kprintf("%s/%s/%ld: uev_NewPosX=%ld  uev_NewPosY=%ld\n", __FILE__, __FUNC__, __LINE__, uev->uev_NewPosX, uev->uev_NewPosY));

				switch (uev->uev_Type)
					{
				case UNDO_Copy:
				case UNDO_Move:
				case UNDO_CreateLink:
					Success = UndoAddCopyMoveEvent(uev, TagList);
					break;
				case UNDO_ChangeIconPos:
					Success = AddChangeIconPosEvent(uev, TagList);
					break;
				case UNDO_Cleanup:
					Success = AddCleanupEvent(uev, TagList);
					break;
				case UNDO_Snapshot:
				case UNDO_Unsnapshot:
					Success = AddSnapshotEvent(uev, TagList);
					break;
				case UNDO_Rename:
					Success = AddRenameEvent(uev, TagList);
					break;
				case UNDO_Relabel:
					Success = AddRelabelEvent(uev, TagList);
					break;
				case UNDO_SizeWindow:
					Success = AddSizeWindowEvent(uev, TagList);
					break;
				case UNDO_Delete:
					Success = AddDeleteEvent(uev, TagList);
					break;
				case UNDO_NewDrawer:
					Success = AddNewDrawerEvent(uev, TagList);
					break;
				case UNDO_Leaveout:
				case UNDO_PutAway:
					Success = AddLeaveOutPutAwayEvent(uev, TagList);
					break;
				case UNDO_SetProtection:
					Success = AddSetProtectionEvent(uev, TagList);
					break;
				case UNDO_SetComment:
					Success = AddSetCommentEvent(uev, TagList);
					break;
				case UNDO_SetToolTypes:
					Success = AddSetToolTypesEvent(uev, TagList);
					break;
				case UNDO_ChangeIconObject:
					Success = AddChangeIconObjectEvent(uev, TagList);
					break;
				case UNDO_CloseWindow:
					Success = AddCloseWindowEvent(uev, TagList);
					break;
				default:
					if (uev->uev_CustomAddHook)
						{
						Success = CallHookPkt(uev->uev_CustomAddHook, NULL, uev);
						}
					break;
					}

				d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

				if (Success)
					{
					uev->uev_UndoStep = ust;
					ust->ust_EventCount++;

					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					AddTail(&ust->ust_EventList, &uev->uev_Node);
					uev = NULL;

					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					if (NULL == ustMulti)
						{
						UndoEndStep(iwt, (APTR) ust);
						}

					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					}
				}
			}

		d1(kprintf("%s/%s/%ld: uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));

		if (uev)
			UndoDisposeEvent(uev);
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;
}

//----------------------------------------------------------------------------

APTR UndoBeginStep(void)
{
	struct UndoStep *ust;

	do	{
		ust = UndoCreateStep();
		if (NULL == ust)
			break;
		} while (0);

	return (APTR) ust;
}

//----------------------------------------------------------------------------

void UndoEndStep(struct internalScaWindowTask *iwt, APTR event)
{
	struct UndoStep *ust = (struct UndoStep *) event;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		if (NULL == ust)
			break;

		if (IsListEmpty(&ust->ust_EventList))
			break;

		ScalosObtainSemaphore(&UndoListListSemaphore);
		AddTail(&globalUndoList, &ust->ust_Node);
		if (++UndoCount > CurrentPrefs.pref_MaxUndoSteps)
			{
			// pref_MaxUndoSteps exceeded - dispose oldest list entry
			ust = (struct UndoStep *) RemHead(&globalUndoList);
			UndoCount--;
			}
		else
			{
			ust = NULL;	// do not dipose ust here!
			}

		ScalosReleaseSemaphore(&UndoListListSemaphore);
		} while (0);

	d1(kprintf("%s/%s/%ld: ust=%08lx\n", __FILE__, __FUNC__, __LINE__, ust));

	if (ust)
		UndoDisposeStep(ust);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	SetMenuOnOff(iwt);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static struct UndoStep *UndoCreateStep(void)
{
	struct UndoStep *ust;

	do	{
		ust = ScalosAlloc(sizeof(struct UndoStep));
		if (NULL == ust)
			break;

		NewList(&ust->ust_EventList);
		ust->ust_EventCount = 0;
		ust->ust_Description = NULL;
		ust->ust_FileTransObj = NULL;
		} while (0);

	return ust;
}

//----------------------------------------------------------------------------

static void UndoDisposeStep(struct UndoStep *ust)
{
	if (ust)
		{
		struct UndoEvent *uev;

		while ((uev = (struct UndoEvent *) RemHead(&ust->ust_EventList)))
			{
			ust->ust_EventCount--;
			UndoDisposeEvent(uev);
			}

		if (ust->ust_Description)
			{
			FreeCopyString(ust->ust_Description);
			ust->ust_Description = NULL;
			}

		ScalosFree(ust);
		}
}

//----------------------------------------------------------------------------

static struct UndoEvent *UndoCreateEvent(void)
{
	struct UndoEvent *uev;

	do	{
		static struct Hook DummyHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDummyFunc),	// h_Entry + h_SubEntry
			NULL,				// h_Data
			};

		uev = ScalosAlloc(sizeof(struct UndoEvent));
		if (NULL == uev)
			break;

		memset(uev, 0, sizeof(struct UndoEvent));

		uev->uev_UndoHook = &DummyHook;
		uev->uev_RedoHook = &DummyHook;
		uev->uev_DisposeHook = &DummyHook;
		} while (0);
	return uev;
}

//----------------------------------------------------------------------------

static void UndoDisposeEvent(struct UndoEvent *uev)
{
	if (uev)
		{
		if (uev->uev_DisposeHook)
			CallHookPkt(uev->uev_DisposeHook, NULL, uev);  // cleanup type-specific data
		ScalosFree(uev);
		}
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoDummyFunc(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;
	(void) uev;

	return 0;
}

//----------------------------------------------------------------------------

static void RedoCleanup(void)
{
	struct UndoStep *ust;

	ScalosObtainSemaphore(&RedoListListSemaphore);

	while ((ust = (struct UndoStep *) RemHead(&globalRedoList)))
		{
		UndoDisposeStep(ust);
		}
	ScalosReleaseSemaphore(&RedoListListSemaphore);
}

//----------------------------------------------------------------------------

BOOL Undo(struct internalScaWindowTask *iwt)
{
	struct UndoProcArg upa;
	BOOL Success = FALSE;

	upa.upa_iwt = iwt;

	ScalosObtainSemaphore(&UndoListListSemaphore);
	upa.upa_UndoStep = (struct UndoStep *) RemTail(&globalUndoList);
	UndoCount--;
	ScalosReleaseSemaphore(&UndoListListSemaphore);

	d1(kprintf("%s/%s/%ld: ust=%08lx\n", __FILE__, __FUNC__, __LINE__, upa.upa_UndoStep));

	if (upa.upa_UndoStep)
		{
		Success = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_RunProcess, UndoTask,
			&upa, sizeof(upa), iInfos.xii_iinfos.ii_MainMsgPort);
		}

	return Success;
}

//----------------------------------------------------------------------------

BOOL Redo(struct internalScaWindowTask *iwt)
{
	struct UndoProcArg upa;
	BOOL Success = FALSE;

	upa.upa_iwt = iwt;

	ScalosObtainSemaphore(&RedoListListSemaphore);
	upa.upa_UndoStep = (struct UndoStep *) RemTail(&globalRedoList);
	ScalosReleaseSemaphore(&RedoListListSemaphore);

	d1(kprintf("%s/%s/%ld: ust=%08lx\n", __FILE__, __FUNC__, __LINE__, upa.upa_UndoStep));

	if (upa.upa_UndoStep)
		{
		Success = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_RunProcess, RedoTask,
			&upa, sizeof(upa), iInfos.xii_iinfos.ii_MainMsgPort);
		}

	return Success;
}

//----------------------------------------------------------------------------

CONST_STRPTR UndoGetDescription(void)
{
	static char buffer[200];

	ScalosObtainSemaphore(&UndoListListSemaphore);

	if (IsListEmpty(&globalUndoList))
		{
		snprintf(buffer, sizeof(buffer),
			GetLocString(MSGID_UNDO_NOT_AVAILABLE),
			GetLocString(MSGID_UNDO_UNDO));
		}
	else
		{
		struct UndoStep *ust = (struct UndoStep *) globalUndoList.lh_TailPred;

		stccpy(buffer, GetLocString(MSGID_UNDO_UNDO), sizeof(buffer));

		d1(kprintf("%s/%s/%ld: ust=%08lx\n", __FILE__, __FUNC__, __LINE__, ust));

		if (!IsListEmpty(&ust->ust_EventList))
			{
			struct UndoEvent *uev = (struct UndoEvent *) ust->ust_EventList.lh_Head;

			if (ust->ust_EventCount > 1)
				{
				snprintf(buffer, sizeof(buffer),
					GetLocString(uev->uev_DescrMsgIDMultiple),
					GetLocString(MSGID_UNDO_UNDO),
					ust->ust_EventCount);
				}
			else if (uev->uev_DescrObjName && *uev->uev_DescrObjName)
				{
				snprintf(buffer, sizeof(buffer),
					GetLocString(uev->uev_DescrMsgIDSingle),
					GetLocString(MSGID_UNDO_UNDO),
					*uev->uev_DescrObjName);
				}
			}
		}

	ScalosReleaseSemaphore(&UndoListListSemaphore);

	return buffer;
}

//----------------------------------------------------------------------------

CONST_STRPTR RedoGetDescription(void)
{
	static char buffer[200];

	ScalosObtainSemaphore(&RedoListListSemaphore);

	if (IsListEmpty(&globalRedoList))
		{
		snprintf(buffer, sizeof(buffer),
			GetLocString(MSGID_UNDO_NOT_AVAILABLE),
			GetLocString(MSGID_UNDO_REDO));
		}
	else
		{
		struct UndoStep *ust = (struct UndoStep *) globalRedoList.lh_TailPred;

		stccpy(buffer, GetLocString(MSGID_UNDO_REDO), sizeof(buffer));

		d1(kprintf("%s/%s/%ld: ust=%08lx\n", __FILE__, __FUNC__, __LINE__, ust));
		
		if (!IsListEmpty(&ust->ust_EventList))
			{
			struct UndoEvent *uev = (struct UndoEvent *) ust->ust_EventList.lh_Head;

			if (ust->ust_EventCount > 1)
				{
				snprintf(buffer, sizeof(buffer),
					GetLocString(uev->uev_DescrMsgIDMultiple),
					GetLocString(MSGID_UNDO_REDO),
					ust->ust_EventCount);
				}
			else if (uev->uev_DescrObjName && *uev->uev_DescrObjName)
				{
				snprintf(buffer, sizeof(buffer),
					GetLocString(uev->uev_DescrMsgIDSingle),
					GetLocString(MSGID_UNDO_REDO),
					*uev->uev_DescrObjName);
				}
			}
		}

	ScalosReleaseSemaphore(&RedoListListSemaphore);

	return buffer;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) UndoTask(struct UndoProcArg *upa, struct SM_RunProcess *msg)
{
	static CONST_STRPTR ProgTaskName = "Scalos_UndoTask";
	APTR prWindowPtr;
	struct Process *myProc = (struct Process *) FindTask(NULL);

	(void) msg;

	d1(kprintf("%s/%s/%ld: START  ust=%08lx\n", __FILE__, __FUNC__, __LINE__, upa->upa_UndoStep));

	prWindowPtr = myProc->pr_WindowPtr;
	myProc->pr_WindowPtr = (APTR) ~0;    // suppress error requesters

	myProc->pr_Task.tc_Node.ln_Name = (STRPTR) ProgTaskName;
	SetProgramName(ProgTaskName);

	upa->upa_UndoStep->ust_FileTransObj = SCA_NewScalosObjectTags((STRPTR) "FileTransfer.sca",
		SCCA_FileTrans_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
		SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask,
		TAG_END);

	if (upa->upa_UndoStep->ust_FileTransObj)
		{
		struct UndoEvent *uev;

		for (uev = (struct UndoEvent *) upa->upa_UndoStep->ust_EventList.lh_Head;
			uev != (struct UndoEvent *) &upa->upa_UndoStep->ust_EventList.lh_Tail;
			uev = (struct UndoEvent *) uev->uev_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: uev=%08lx  uev_Type=%ld  uev_UndoHook=%08lx\n", __FILE__, __FUNC__, __LINE__, uev, uev->uev_Type, uev->uev_UndoHook));
			if (uev->uev_UndoHook)
				CallHookPkt(uev->uev_UndoHook, NULL, uev);
			}
		// Move undo step to redo list
		ScalosObtainSemaphore(&RedoListListSemaphore);
		AddTail(&globalRedoList, &upa->upa_UndoStep->ust_Node);
		ScalosReleaseSemaphore(&RedoListListSemaphore);

		SCA_DisposeScalosObject(upa->upa_UndoStep->ust_FileTransObj);
		upa->upa_UndoStep->ust_FileTransObj = NULL;
		}
	else
		{
		UndoDisposeStep(upa->upa_UndoStep);
		}

	SetMenuOnOff(upa->upa_iwt);

	// restore pr_WindowPtr
	myProc->pr_WindowPtr = prWindowPtr;

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) RedoTask(struct UndoProcArg *upa, struct SM_RunProcess *msg)
{
	static CONST_STRPTR ProgTaskName = "Scalos_RedoTask";
	APTR prWindowPtr;
	struct Process *myProc = (struct Process *) FindTask(NULL);

	(void) msg;

	d1(kprintf("%s/%s/%ld: START  ust=%08lx\n", __FILE__, __FUNC__, __LINE__, upa->upa_UndoStep));

	prWindowPtr = myProc->pr_WindowPtr;
	myProc->pr_WindowPtr = (APTR) ~0;    // suppress error requesters

	myProc->pr_Task.tc_Node.ln_Name = (STRPTR) ProgTaskName;
	SetProgramName(ProgTaskName);

	upa->upa_UndoStep->ust_FileTransObj = SCA_NewScalosObjectTags((STRPTR) "FileTransfer.sca",
		SCCA_FileTrans_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
		SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask,
		TAG_END);

	if (upa->upa_UndoStep->ust_FileTransObj)
		{
		struct UndoEvent *uev;

		for (uev = (struct UndoEvent *) upa->upa_UndoStep->ust_EventList.lh_Head;
			uev != (struct UndoEvent *) &upa->upa_UndoStep->ust_EventList.lh_Tail;
			uev = (struct UndoEvent *) uev->uev_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: uev=%08lx  uev_Type=%ld  uev_RedoHook=%08lx\n", __FILE__, __FUNC__, __LINE__, uev, uev->uev_Type, uev->uev_RedoHook));
			if (uev->uev_RedoHook)
				CallHookPkt(uev->uev_RedoHook, NULL, uev);
			}

		// Move redo step to undo list
		ScalosObtainSemaphore(&UndoListListSemaphore);
		AddTail(&globalUndoList, &upa->upa_UndoStep->ust_Node);
		UndoCount++;
		ScalosReleaseSemaphore(&UndoListListSemaphore);

		SCA_DisposeScalosObject(upa->upa_UndoStep->ust_FileTransObj);
		upa->upa_UndoStep->ust_FileTransObj = NULL;
		}
	else
		{
		UndoDisposeStep(upa->upa_UndoStep);
		}

	SetMenuOnOff(upa->upa_iwt);

	// restore pr_WindowPtr
	myProc->pr_WindowPtr = prWindowPtr;

	return 0;
}

//----------------------------------------------------------------------------

static BOOL UndoAddCopyMoveEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		BPTR dirLock;
		CONST_STRPTR fName;
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		static struct Hook UndoDisposeCopyMoveDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeCopyMoveData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoCopyEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoCopyEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoCopyEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoCopyEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoMoveEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoMoveEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoMoveEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoMoveEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoCreateLinkEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoCreateLinkEvent),    	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeCopyMoveDataHook, TagList);

		if (UNDO_Copy == uev->uev_Type)
			{
			uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoCopyEventHook, TagList);
			uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoCopyEventHook, TagList);
			uev->uev_DescrMsgIDSingle = MSGID_UNDO_COPY_SINGLE;
			uev->uev_DescrMsgIDMultiple = MSGID_UNDO_COPY_MORE;
			}
		else if (UNDO_Move == uev->uev_Type)
			{
			uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoMoveEventHook, TagList);
			uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoMoveEventHook, TagList);
			uev->uev_DescrMsgIDSingle = MSGID_UNDO_MOVE_SINGLE;
			uev->uev_DescrMsgIDMultiple = MSGID_UNDO_MOVE_MORE;
			}
		else if (UNDO_CreateLink == uev->uev_Type)
			{
			uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoCopyEventHook, TagList);
			uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoCreateLinkEventHook, TagList);
			uev->uev_DescrMsgIDSingle = MSGID_UNDO_CREATELINK_SINGLE;
			uev->uev_DescrMsgIDMultiple = MSGID_UNDO_CREATELINK_MORE;
			}

		uev->uev_OldPosX = GetTagData(UNDOTAG_OldIconPosX, NO_ICON_POSITION_SHORT, TagList);
		uev->uev_OldPosY = GetTagData(UNDOTAG_OldIconPosY, NO_ICON_POSITION_SHORT, TagList);

		uev->uev_DescrObjName = &ucmed->ucmed_srcName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		dirLock = (BPTR) GetTagData(UNDOTAG_CopySrcDirLock, 0, TagList);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopySrcDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		ucmed->ucmed_srcDirName = AllocCopyString(name);
		if (NULL == ucmed->ucmed_srcDirName)
			break;

		dirLock = (BPTR) GetTagData(UNDOTAG_CopyDestDirLock, 0, TagList);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopyDestDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		ucmed->ucmed_destDirName = AllocCopyString(name);
		if (NULL == ucmed->ucmed_destDirName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopySrcName, (ULONG) NULL, TagList);
		if (NULL == fName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopySrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_srcName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_srcName)
			break;

		// special handling for volume copying: here is ucmed_srcName empty
		if ((NULL == ucmed->ucmed_srcName) || (0 == strlen(ucmed->ucmed_srcName)))
			uev->uev_DescrObjName = &ucmed->ucmed_srcDirName;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopyDestName, (ULONG) ucmed->ucmed_srcName, TagList);
		if (NULL == fName)
			fName = ucmed->ucmed_srcName;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopyDestName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_destName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_destName)
			break;

		if ((NO_ICON_POSITION_SHORT == uev->uev_OldPosX) || (NO_ICON_POSITION_SHORT == uev->uev_OldPosY))
			{
			struct internalScaWindowTask *iwt;
			struct ScaIconNode *in = NULL;

			do	{
				struct ExtGadget *gg;

				if (!UndoFindWindowAndIcon(ucmed->ucmed_srcDirName, ucmed->ucmed_srcName, &iwt, &in))
					break;

				if (NULL == in)
					break;

				gg = (struct ExtGadget *) in->in_Icon;

				uev->uev_OldPosX = gg->LeftEdge;
				uev->uev_OldPosY = gg->TopEdge;
				} while (0);

			if (iwt)
				{
				if (in)
					ScalosUnLockIconList(iwt);
				SCA_UnLockWindowList();
				}
			}

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddChangeIconPosEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	do	{
		BPTR dirLock;
		struct ScaIconNode *in;
		const struct ExtGadget *gg;
		static struct Hook DisposeIconDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeIconData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoChangeIconPosEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoChangeIconPosEvent),   // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoChangeIconPosEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoChangeIconPosEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &DisposeIconDataHook, TagList);
		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoChangeIconPosEventHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoChangeIconPosEventHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_CHANGEICONPOS_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_CHANGEICONPOS_MORE;

		uev->uev_DescrObjName = &uev->uev_Data.uev_IconData.uid_IconName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		uev->uev_Data.uev_IconData.uid_DirName = AllocCopyString(name);
		if (NULL == uev->uev_Data.uev_IconData.uid_DirName)
			break;

		in  = (struct ScaIconNode *) GetTagData(UNDOTAG_IconNode, (ULONG) NULL, TagList);
		if (NULL == in)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconNode=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		uev->uev_Data.uev_IconData.uid_IconName = AllocCopyString(GetIconName(in));
		if (NULL == uev->uev_Data.uev_IconData.uid_IconName)
			break;

		gg = (const struct ExtGadget *) in->in_Icon;

		uev->uev_OldPosX = gg->LeftEdge;
		uev->uev_OldPosY = gg->TopEdge;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconOldPosX=%ld  UNDOTAG_IconOldPosY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, uev->uev_OldPosX, \
			uev->uev_OldPosY));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddSnapshotEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	do	{
		BPTR oldDir;
		BPTR dirLock;
		struct ScaIconNode *in;
		const struct ExtGadget *gg;
		struct UndoSnaphotIconData *usid = &uev->uev_Data.uev_SnapshotData;
		static struct Hook DisposeIconDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeSnapshotData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoSnapshotEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoSnapshotEvent),   	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoSnapshotPosEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoSnapshotPosEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &DisposeIconDataHook, TagList);
		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoSnapshotEventHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoSnapshotPosEventHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_SNAPSHOT_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_SNAPSHOT_MORE;

		uev->uev_DescrObjName = &usid->usid_IconName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		usid->usid_DirName = AllocCopyString(name);
		if (NULL == usid->usid_DirName)
			break;

		usid->usid_SaveIcon = GetTagData(UNDOTAG_SaveIcon, FALSE, TagList);

		d1(kprintf("%s/%s/%ld: UNDOTAG_SaveIcon=%ld\n", __FILE__, __FUNC__, __LINE__, usid->usid_SaveIcon));

		in  = (struct ScaIconNode *) GetTagData(UNDOTAG_IconNode, (ULONG) NULL, TagList);
		if (NULL == in)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconNode=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		usid->usid_IconName = AllocCopyString(GetIconName(in));
		if (NULL == usid->usid_IconName)
			break;

		oldDir = CurrentDir(dirLock);

		usid->usid_IconObj = (Object *) NewIconObjectTags(usid->usid_IconName,
			IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
			IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints,
			IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor,
			IDTA_Text, (ULONG) usid->usid_IconName,
			DTA_Name, (ULONG) usid->usid_IconName,
			TAG_END);

		CurrentDir(oldDir);

		d1(kprintf("%s/%s/%ld: usid_IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, usid->usid_IconObj));

		if (NULL == usid->usid_IconObj)
			break;

		gg = (const struct ExtGadget *) in->in_Icon;

		uev->uev_OldPosX = gg->LeftEdge;
		uev->uev_OldPosY = gg->TopEdge;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconOldPosX=%ld  UNDOTAG_IconOldPosY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, uev->uev_OldPosX, \
			uev->uev_OldPosY));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddCleanupEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCleanupData *ucd = &uev->uev_Data.uev_CleanupData;
		const struct ScaIconNode *IconList;
		const struct ScaIconNode *in;
		struct UndoCleanupIconEntry *ucie;
		static struct Hook DisposeCleanupDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeCleanupData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoCleanupEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoCleanupEvent),   	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoCleanupEventHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoCleanupEvent),		// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &DisposeCleanupDataHook, TagList);
		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoCleanupEventHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoCleanupEventHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_CLEANUP;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_CLEANUP;

		ucd->ucd_CleanupMode = GetTagData(UNDOTAG_CleanupMode, CLEANUP_Default, TagList);

		d1(kprintf("%s/%s/%ld: UNDOTAG_CleanupMode=%ld\n", __FILE__, __FUNC__, __LINE__, ucd->ucd_CleanupMode));

		ucd->ucd_WindowTask = (struct ScaWindowTask *) GetTagData(UNDOTAG_WindowTask, (ULONG) NULL, TagList);
		if (NULL == ucd->ucd_WindowTask)
			break;

		ucd->ucd_WindowTitle = AllocCopyString(((struct internalScaWindowTask *) ucd->ucd_WindowTask)->iwt_WinTitle);
		if (NULL == ucd->ucd_WindowTitle)
			break;

		uev->uev_DescrObjName = &ucd->ucd_WindowTitle;

		d1(kprintf("%s/%s/%ld: UNDOTAG_WindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__, ucd->ucd_WindowTask));
		d1(kprintf("%s/%s/%ld: ucd_WindowTitle=<%s>\n", __FILE__, __FUNC__, __LINE__, ucd->ucd_WindowTitle));

		IconList = (struct ScaIconNode *) GetTagData(UNDOTAG_IconList, (ULONG) NULL, TagList);
		if (NULL == IconList)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, IconList));

		ScalosLockIconListShared((struct internalScaWindowTask *) ucd->ucd_WindowTask);

		// count number of icons
		for (in = IconList; in; in = (const struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			ucd->ucd_IconCount++;
			}

		d1(kprintf("%s/%s/%ld: ucd_IconCount=%lu\n", __FILE__, __FUNC__, __LINE__, ucd->ucd_IconCount));

		// create array to store icon positions
		ucd->ucd_Icons = ScalosAlloc(ucd->ucd_IconCount * sizeof(struct UndoCleanupIconEntry));
		if (NULL == ucd->ucd_Icons)
			break;

		// store icon positions in array
		for (ucie = ucd->ucd_Icons, in = IconList;
			in; in = (const struct ScaIconNode *) in->in_Node.mln_Succ, ucie++)
			{
			const struct ExtGadget *gg = (const struct ExtGadget *) in->in_Icon;

			ucie->ucin_IconNode = in;
			ucie->ucin_Left = gg->LeftEdge;
			ucie->ucin_Top = gg->TopEdge;

			d1(kprintf("%s/%s/%ld: in=%08lx <%s>  Left=%ld  Top=%ld\n", \
				__FILE__, __FUNC__, __LINE__, in, GetIconName(in), \
				ucie->ucin_Left, ucie->ucin_Top));
			}

		ScalosUnLockIconList((struct internalScaWindowTask *) ucd->ucd_WindowTask);

		d1(kprintf("%s/%s/%ld: ucd_WindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__, ucd->ucd_WindowTask));

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddRenameEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		BPTR dirLock;
		CONST_STRPTR fName;
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		static struct Hook UndoDisposeCopyMoveDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeCopyMoveData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoRenameHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoRenameEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoRenameHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoRenameEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeCopyMoveDataHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoRenameHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoRenameHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_RENAME_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_RENAME_MORE;

		uev->uev_DescrObjName = &ucmed->ucmed_srcName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_CopySrcDirLock, 0, TagList);
		d1(kprintf("%s/%s/%ld: dirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, dirLock));
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopySrcDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		ucmed->ucmed_srcDirName = AllocCopyString(name);
		if (NULL == ucmed->ucmed_srcDirName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopySrcName, (ULONG) NULL, TagList);
		if (NULL == fName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopySrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_srcName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_srcName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopyDestName, (ULONG) NULL, TagList);
		if (NULL == fName)
			break;
		d1(kprintf("%s/%s/%ld: UNDOTAG_CopyDestName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_destName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_destName)
			break;

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddRelabelEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		CONST_STRPTR fName;
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		static struct Hook UndoDisposeCopyMoveDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeCopyMoveData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoRelabelHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoRelabelEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoRelabelHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoRelabelEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeCopyMoveDataHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoRelabelHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoRelabelHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_RELABEL_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_RELABEL_MORE;

		uev->uev_DescrObjName = &ucmed->ucmed_srcName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		ucmed->ucmed_srcDirName = AllocCopyString(name);
		if (NULL == ucmed->ucmed_srcDirName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopySrcName, (ULONG) NULL, TagList);
		if (NULL == fName)
			break;

		// i.e. "Data1old:"
		d1(kprintf("%s/%s/%ld: UNDOTAG_CopySrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_srcName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_srcName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopyDestName, (ULONG) NULL, TagList);
		if (NULL == fName)
			break;

		// i.e. "Data1new"
		d1(kprintf("%s/%s/%ld: UNDOTAG_CopyDestName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_destName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_destName)
			break;

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddDeleteEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		BPTR dirLock;
		CONST_STRPTR fName;
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		static struct Hook UndoDisposeCopyMoveDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeCopyMoveData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoDeleteHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoMoveEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoDeleteHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoMoveEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeCopyMoveDataHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoDeleteHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoDeleteHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_DELETE_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_DELETE_MORE;

		uev->uev_DescrObjName = &ucmed->ucmed_srcName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_CopySrcDirLock, 0, TagList);
		d1(kprintf("%s/%s/%ld: dirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, dirLock));
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopySrcDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		ucmed->ucmed_srcDirName = AllocCopyString(name);
		if (NULL == ucmed->ucmed_srcDirName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopySrcName, (ULONG) NULL, TagList);
		if (NULL == fName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_CopySrcName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_srcName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_srcName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_CopyDestName, (ULONG) "SYS:Trashcan", TagList);
		if (NULL == fName)
			break;
		d1(kprintf("%s/%s/%ld: UNDOTAG_CopyDestName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		ucmed->ucmed_destName = AllocCopyString(fName);
		if (NULL == ucmed->ucmed_destName)
			break;

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddSizeWindowEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSizeWindowData *uswd = &uev->uev_Data.uev_SizeWindowData;
		static struct Hook DisposeSizeWindowDataHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeSizeWindowData),   // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoSizeWindowHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoSizeWindowEvent),  	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoSizeWindowHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoSizeWindowEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &DisposeSizeWindowDataHook, TagList);
		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoSizeWindowHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoSizeWindowHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_SIZEWINDOW;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_SIZEWINDOW;

		uswd->uswd_WindowTask = (struct ScaWindowTask *) GetTagData(UNDOTAG_WindowTask, (ULONG) NULL, TagList);
		if (NULL == uswd->uswd_WindowTask)
			break;

		uswd->uswd_WindowTitle = AllocCopyString(((struct internalScaWindowTask *) uswd->uswd_WindowTask)->iwt_WinTitle);
		if (NULL == uswd->uswd_WindowTitle)
			break;

		uev->uev_DescrObjName = &uswd->uswd_WindowTitle;

		d1(kprintf("%s/%s/%ld: UNDOTAG_WindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_WindowTask));

		uswd->uswd_OldLeft = GetTagData(UNDOTAG_OldWindowLeft, 0, TagList);
		uswd->uswd_OldTop = GetTagData(UNDOTAG_OldWindowTop, 0, TagList);
		uswd->uswd_OldWidth = GetTagData(UNDOTAG_OldWindowWidth, 0, TagList);
		uswd->uswd_OldHeight = GetTagData(UNDOTAG_OldWindowHeight, 0, TagList);
		uswd->uswd_OldVirtX = GetTagData(UNDOTAG_OldWindowVirtX, 0, TagList);
		uswd->uswd_OldVirtY = GetTagData(UNDOTAG_OldWindowVirtY, 0, TagList);

		uswd->uswd_NewLeft = GetTagData(UNDOTAG_NewWindowLeft, uswd->uswd_OldLeft, TagList);
		uswd->uswd_NewTop = GetTagData(UNDOTAG_NewWindowTop, uswd->uswd_OldTop, TagList);
		uswd->uswd_NewWidth = GetTagData(UNDOTAG_NewWindowWidth, uswd->uswd_OldWidth, TagList);
		uswd->uswd_NewHeight = GetTagData(UNDOTAG_NewWindowHeight, uswd->uswd_OldHeight, TagList);
		uswd->uswd_NewVirtX = GetTagData(UNDOTAG_NewWindowVirtX, uswd->uswd_OldVirtX, TagList);
		uswd->uswd_NewVirtY = GetTagData(UNDOTAG_NewWindowVirtY, uswd->uswd_OldVirtY, TagList);

		d1(kprintf("%s/%s/%ld: UNDOTAG_OldWindowLeft=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_OldLeft));
		d1(kprintf("%s/%s/%ld: UNDOTAG_OldWindowTop=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_OldTop));
		d1(kprintf("%s/%s/%ld: UNDOTAG_OldWindowWidth=%lu\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_OldWidth));
		d1(kprintf("%s/%s/%ld: UNDOTAG_OldWindowHeight=%lu\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_OldHeight));
		d1(kprintf("%s/%s/%ld: UNDOTAG_OldWindowVirtX=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_OldVirtX));
		d1(kprintf("%s/%s/%ld: UNDOTAG_OldWindowVirtY=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_OldVirtY));
		d1(kprintf("%s/%s/%ld: UNDOTAG_NewWindowLeft=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_NewLeft));
		d1(kprintf("%s/%s/%ld: UNDOTAG_NewWindowTop=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_NewTop));
		d1(kprintf("%s/%s/%ld: UNDOTAG_NewWindowWidth=%lu\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_NewWidth));
		d1(kprintf("%s/%s/%ld: UNDOTAG_NewWindowHeight=%lu\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_NewHeight));
		d1(kprintf("%s/%s/%ld: UNDOTAG_NewWindowVirtX=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_NewVirtX));
		d1(kprintf("%s/%s/%ld: UNDOTAG_NewWindowVirtY=%ld\n", __FILE__, __FUNC__, __LINE__, uswd->uswd_NewVirtY));

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddNewDrawerEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		BPTR dirLock;
		CONST_STRPTR fName;
		struct UndoNewDrawerData *und = &uev->uev_Data.uev_NewDrawerData;
		static struct Hook UndoDisposeNewDrawerHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeNewDrawerData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoNewDrawerHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoNewDrawerEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoNewDrawerHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoNewDrawerEvent),      	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeNewDrawerHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoNewDrawerHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoNewDrawerHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_NEWDRAWER_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_NEWDRAWER_MORE;
		uev->uev_DescrObjName = &und->und_srcName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		d1(kprintf("%s/%s/%ld: dirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, dirLock));
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		und->und_DirName = AllocCopyString(name);
		if (NULL == und->und_DirName)
			break;

		fName = (CONST_STRPTR) GetTagData(UNDOTAG_IconName, (ULONG) NULL, TagList);
		if (NULL == fName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		und->und_srcName = AllocCopyString(fName);
		if (NULL == und->und_srcName)
			break;

		und->und_CreateIcon = GetTagData(UNDOTAG_CreateIcon, TRUE, TagList);

		d1(kprintf("%s/%s/%ld: UNDOTAG_CreateIcon=%ld\n", __FILE__, __FUNC__, __LINE__, und->und_CreateIcon));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddLeaveOutPutAwayEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSnaphotIconData *usid = &uev->uev_Data.uev_SnapshotData;
		BPTR dirLock;
		CONST_STRPTR IconName;
		static struct Hook UndoDisposeLeaveOutPutAwayHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeSnapshotData),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoLeaveOutHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoLeaveOutEvent),       	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoLeaveOutHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoLeaveOutEvent),       	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeLeaveOutPutAwayHook, TagList);

		if (UNDO_PutAway == uev->uev_Type)
			{
			uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &RedoLeaveOutHook, TagList);
			uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &UndoLeaveOutHook, TagList);
			uev->uev_DescrMsgIDSingle = MSGID_UNDO_PUTAWAY_SINGLE;
			uev->uev_DescrMsgIDMultiple = MSGID_UNDO_PUTAWAY_MORE;
			}
		else
			{
			uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoLeaveOutHook, TagList);
			uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoLeaveOutHook, TagList);
			uev->uev_DescrMsgIDSingle = MSGID_UNDO_LEAVEOUT_SINGLE;
			uev->uev_DescrMsgIDMultiple = MSGID_UNDO_LEAVEOUT_MORE;
			}

		uev->uev_DescrObjName = &usid->usid_IconName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		usid->usid_DirName = AllocCopyString(name);
		if (NULL == usid->usid_DirName)
			break;

		IconName = (CONST_STRPTR) GetTagData(UNDOTAG_IconName, (ULONG) NULL, TagList);
		if (NULL == IconName)
			break;

		usid->usid_IconName = AllocCopyString(IconName);
		if (NULL == usid->usid_IconName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, usid->usid_IconName));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddSetProtectionEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetProtectionData *uspd = &uev->uev_Data.uev_SetProtectionData;
		BPTR dirLock;
		CONST_STRPTR IconName;
		static struct Hook UndoDisposeSetProtectionHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeSetProtectionData),  // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoSetProtectionHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoSetProtectionEvent),   // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoSetProtectionHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoSetProtectionEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeSetProtectionHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoSetProtectionHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoSetProtectionHook, TagList);
		uev->uev_DescrMsgIDSingle = MSGID_UNDO_SETPROTECTION_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_SETPROTECTION_MORE;

		uev->uev_DescrObjName = &uspd->uspd_IconName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		uspd->uspd_DirName = AllocCopyString(name);
		if (NULL == uspd->uspd_DirName)
			break;

		IconName = (CONST_STRPTR) GetTagData(UNDOTAG_IconName, (ULONG) NULL, TagList);
		if (NULL == IconName)
			break;

		uspd->uspd_IconName = AllocCopyString(IconName);
		if (NULL == uspd->uspd_IconName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, uspd->uspd_IconName));

		uspd->uspd_OldProtection = GetTagData(UNDOTAG_OldProtection, 0, TagList);
		uspd->uspd_NewProtection = GetTagData(UNDOTAG_NewProtection, uspd->uspd_OldProtection, TagList);

		d1(kprintf("%s/%s/%ld: UNDOTAG_OldProtection=%08lx\n", __FILE__, __FUNC__, __LINE__, uspd->uspd_OldProtection));
		d1(kprintf("%s/%s/%ld: UNDOTAG_NewProtection=%08lx\n", __FILE__, __FUNC__, __LINE__, uspd->uspd_NewProtection));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddSetCommentEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetCommentData *uscd = &uev->uev_Data.uev_SetCommentData;
		BPTR dirLock;
		CONST_STRPTR IconName;
		CONST_STRPTR Comment;
		static struct Hook UndoDisposeSetCommentHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeSetCommentData),  // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoSetCommentHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoSetCommentEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoSetCommentHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoSetCommentEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeSetCommentHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoSetCommentHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoSetCommentHook, TagList);
		uev->uev_DescrMsgIDSingle = MSGID_UNDO_SETCOMMENT_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_SETCOMMENT_MORE;

		uev->uev_DescrObjName = &uscd->uscd_IconName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		uscd->uscd_DirName = AllocCopyString(name);
		if (NULL == uscd->uscd_DirName)
			break;

		IconName = (CONST_STRPTR) GetTagData(UNDOTAG_IconName, (ULONG) NULL, TagList);
		if (NULL == IconName)
			break;

		uscd->uscd_IconName = AllocCopyString(IconName);
		if (NULL == uscd->uscd_IconName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, uscd->uscd_IconName));

		Comment = (CONST_STRPTR) GetTagData(UNDOTAG_OldComment, (ULONG) NULL, TagList);
		if (NULL == Comment)
			break;

		uscd->uscd_OldComment = AllocCopyString(Comment);
		if (NULL == uscd->uscd_OldComment)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_OldComment=<%s>\n", __FILE__, __FUNC__, __LINE__, uscd->uscd_OldComment));

		Comment = (CONST_STRPTR) GetTagData(UNDOTAG_NewComment, (ULONG) NULL, TagList);
		if (NULL == Comment)
			break;

		uscd->uscd_NewComment = AllocCopyString(Comment);
		if (NULL == uscd->uscd_NewComment)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_NewComment=<%s>\n", __FILE__, __FUNC__, __LINE__, uscd->uscd_NewComment));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddSetToolTypesEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetToolTypesData *ustd = &uev->uev_Data.uev_SetToolTypesData;
		BPTR dirLock;
		CONST_STRPTR IconName;
		CONST_STRPTR *ToolTypes;
		static struct Hook UndoDisposeSetToolTypesHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeSetToolTypesData),  // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoSetToolTypesHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoSetToolTypesEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoSetToolTypesHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoSetToolTypesEvent),	// h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeSetToolTypesHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoSetToolTypesHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoSetToolTypesHook, TagList);
		uev->uev_DescrMsgIDSingle = MSGID_UNDO_SETTOOLTYPES_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_SETTOOLTYPES_MORE;

		uev->uev_DescrObjName = &ustd->ustd_IconName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		ustd->ustd_DirName = AllocCopyString(name);
		if (NULL == ustd->ustd_DirName)
			break;

		IconName = (CONST_STRPTR) GetTagData(UNDOTAG_IconName, (ULONG) NULL, TagList);
		if (NULL == IconName)
			break;

		ustd->ustd_IconName = AllocCopyString(IconName);
		if (NULL == ustd->ustd_IconName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, ustd->ustd_IconName));

		ToolTypes = (CONST_STRPTR *) GetTagData(UNDOTAG_OldToolTypes, (ULONG) NULL, TagList);
		if (NULL == ToolTypes)
			break;

		ustd->ustd_OldToolTypes = CloneToolTypeArray(ToolTypes, 0);
		if (NULL == ustd->ustd_OldToolTypes)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_OldToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, ustd->ustd_OldToolTypes));

		ToolTypes = (CONST_STRPTR *) GetTagData(UNDOTAG_NewToolTypes, (ULONG) NULL, TagList);
		if (NULL == ToolTypes)
			break;

		ustd->ustd_NewToolTypes = CloneToolTypeArray(ToolTypes, 0);
		if (NULL == ustd->ustd_NewToolTypes)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_NewToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, ustd->ustd_NewToolTypes));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddChangeIconObjectEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoChangeIconObjectData *uciod = &uev->uev_Data.uev_ChangeIconObjectData;
		BPTR dirLock;
		CONST_STRPTR IconName;
		Object *IconObj;
		static struct Hook UndoDisposeChangeIconObjectHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeChangeIconObjectData),  // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoChangeIconObjectHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoChangeIconObjectEvent),    // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoChangeIconObjectHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoChangeIconObjectEvent),    // h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeChangeIconObjectHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoChangeIconObjectHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoChangeIconObjectHook, TagList);
		uev->uev_DescrMsgIDSingle = MSGID_UNDO_CHANGEICON_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_CHANGEICON_MORE;

		uev->uev_DescrObjName = &uciod->uciod_IconName;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		dirLock = (BPTR) GetTagData(UNDOTAG_IconDirLock, 0, TagList);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		if (!NameFromLock(dirLock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconDirLock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		uciod->uciod_DirName = AllocCopyString(name);
		if (NULL == uciod->uciod_DirName)
			break;

		IconName = (CONST_STRPTR) GetTagData(UNDOTAG_IconName, (ULONG) NULL, TagList);
		if (NULL == IconName)
			break;

		uciod->uciod_IconName = AllocCopyString(IconName);
		if (NULL == uciod->uciod_IconName)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, uciod->uciod_IconName));

		IconObj	= (Object *) GetTagData(UNDOTAG_OldIconObject, (ULONG) NULL, TagList);
		if (NULL == IconObj)
			break;

		uciod->uciod_OldIconObject = CloneIconObject(IconObj);
		if (NULL == uciod->uciod_OldIconObject)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_OldIconObject=%08lx\n", __FILE__, __FUNC__, __LINE__, uciod->uciod_OldIconObject));

		IconObj	= (Object *) GetTagData(UNDOTAG_NewIconObject, (ULONG) NULL, TagList);
		if (NULL == IconObj)
			break;

		uciod->uciod_NewIconObject = CloneIconObject(IconObj);
		if (NULL == uciod->uciod_NewIconObject)
			break;

		d1(kprintf("%s/%s/%ld: UNDOTAG_NewIconObject=%08lx\n", __FILE__, __FUNC__, __LINE__, uciod->uciod_NewIconObject));

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL AddCloseWindowEvent(struct UndoEvent *uev, struct TagItem *TagList)
{
	BOOL Success = FALSE;
	STRPTR name;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct internalScaWindowTask *iwt;
		struct ScaWindowStruct *ws;
		struct UndoCloseWindowData *ucwd = &uev->uev_Data.uev_CloseWindowData;
		static struct Hook UndoDisposeCloseWindowHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoDisposeCloseWindowData),  // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook UndoCloseWindowHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(UndoCloseWindowEvent),    // h_Entry + h_SubEntry
			NULL,					// h_Data
			};
		static struct Hook RedoCloseWindowHook =
			{
			{ NULL, NULL },
			HOOKFUNC_DEF(RedoCloseWindowEvent),    // h_Entry + h_SubEntry
			NULL,					// h_Data
			};

		uev->uev_DisposeHook = (struct Hook *) GetTagData(UNDOTAG_DisposeHook, (ULONG) &UndoDisposeCloseWindowHook, TagList);

		uev->uev_UndoHook = (struct Hook *) GetTagData(UNDOTAG_UndoHook, (ULONG) &UndoCloseWindowHook, TagList);
		uev->uev_RedoHook = (struct Hook *) GetTagData(UNDOTAG_RedoHook, (ULONG) &RedoCloseWindowHook, TagList);

		uev->uev_DescrMsgIDSingle = MSGID_UNDO_CLOSEWINDOW_SINGLE;
		uev->uev_DescrMsgIDMultiple = MSGID_UNDO_CLOSEWINDOW_MORE;

		name = AllocPathBuffer();
		if (NULL == name)
			break;

		d1(kprintf("%s/%s/%ld: name=%08lx\n", __FILE__, __FUNC__, __LINE__, name));

		iwt = (struct internalScaWindowTask *) GetTagData(UNDOTAG_WindowTask, (ULONG) NULL, TagList);
		if (BNULL == iwt)
			break;

		ws = iwt->iwt_WindowTask.mt_WindowStruct;

		if (BNULL == ws->ws_Lock)
			break;

		ucwd->ucwd_WindowTitle = AllocCopyString(iwt->iwt_WinTitle);
		if (NULL == ucwd->ucwd_WindowTitle)
			break;

		d1(kprintf("%s/%s/%ld: ucwd_WindowTitle=%08lx\n", __FILE__, __FUNC__, __LINE__, ucwd->ucwd_WindowTitle));

		uev->uev_DescrObjName = &ucwd->ucwd_WindowTitle;

		if (!NameFromLock(ws->ws_Lock, name, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: ws_lock=<%s>\n", __FILE__, __FUNC__, __LINE__, name));

		ucwd->ucwd_DirName = AllocCopyString(name);
		if (NULL == ucwd->ucwd_DirName)
			break;

		ucwd->ucwd_Left      = ws->ws_Left;
		ucwd->ucwd_Top       = ws->ws_Top;
		ucwd->ucwd_Width     = ws->ws_Width;
		ucwd->ucwd_Height    = ws->ws_Height;
		ucwd->ucwd_VirtX     = ws->ws_xoffset;
		ucwd->ucwd_VirtY     = ws->ws_yoffset;
		ucwd->ucwd_ViewAll   = ws->ws_ViewAll;
		ucwd->ucwd_Viewmodes = ws->ws_Viewmodes;

		Success = TRUE;
		} while (0);

	if (name)
		FreePathBuffer(name);

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoCopyEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR destDirLock;
	STRPTR iconName = NULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		STRPTR destName;
		CONST_STRPTR iconExt = ".info";
		LONG Result;

		destDirLock = Lock(ucmed->ucmed_destDirName, ACCESS_READ);
		d1(kprintf("%s/%s/%ld: destDirLock=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, destDirLock, ucmed->ucmed_destDirName));
		if (BNULL == destDirLock)
			break;

		destName = ucmed->ucmed_destName;
		if (0 == strlen(destName))
			destName = ucmed->ucmed_srcName;

		Result = DoMethod(uev->uev_UndoStep->ust_FileTransObj,
			SCCM_FileTrans_Delete,
			destDirLock, destName);
		if (RETURN_OK != Result)
			break;

		iconName = ScalosAlloc(1 + strlen(destName) + strlen(iconExt));
		if (NULL == iconName)
			break;

		strcpy(iconName, destName);
		strcat(iconName, iconExt);

		/*Result =*/ DoMethod(uev->uev_UndoStep->ust_FileTransObj,
			SCCM_FileTrans_Delete,
			destDirLock, iconName);

		Success = TRUE;
		} while (0);

	if (BNULL != destDirLock)
		UnLock(destDirLock);
	if (iconName)
		ScalosFree(iconName);

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoCopyEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR srcDirLock;
	BPTR destDirLock = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		ULONG Result;

		srcDirLock = Lock(ucmed->ucmed_srcDirName, ACCESS_READ);
		if (BNULL == srcDirLock)
			break;

		destDirLock = Lock(ucmed->ucmed_destDirName, ACCESS_READ);
		if (BNULL == destDirLock)
			break;

		Result = DoMethod(uev->uev_UndoStep->ust_FileTransObj, SCCM_FileTrans_Copy,
			srcDirLock, destDirLock,
			ucmed->ucmed_srcName,
			ucmed->ucmed_destName,
			uev->uev_NewPosX, uev->uev_NewPosY);
		if (RETURN_OK != Result)
			break;

		Success = TRUE;
		} while (0);

	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (BNULL != destDirLock)
		UnLock(destDirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

// Move moved file system object back to original position
static SAVEDS(LONG) UndoMoveEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR srcDirLock;
	BPTR destDirLock = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		ULONG Result;

		d1(kprintf("%s/%s/%ld: ucmed_srcDirName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_srcDirName));

		srcDirLock = Lock(ucmed->ucmed_srcDirName, ACCESS_READ);
		if (BNULL == srcDirLock)
			break;

		debugLock_d1(srcDirLock);

		d1(kprintf("%s/%s/%ld: ucmed_destDirName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_destDirName));

		destDirLock = Lock(ucmed->ucmed_destDirName, ACCESS_READ);
		if (BNULL == destDirLock)
			break;

		debugLock_d1(destDirLock);

		d1(kprintf("%s/%s/%ld: .ucmed_srcName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_srcName));

		Result = DoMethod(uev->uev_UndoStep->ust_FileTransObj, SCCM_FileTrans_Move,
			destDirLock, srcDirLock,
			ucmed->ucmed_srcName,
			uev->uev_OldPosX, uev->uev_OldPosY);
		if (RETURN_OK != Result)
			break;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (BNULL != destDirLock)
		UnLock(destDirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

// Repeat Move of moved file system object
static SAVEDS(LONG) RedoMoveEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR srcDirLock;
	BPTR destDirLock = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		ULONG Result;

		srcDirLock = Lock(ucmed->ucmed_srcDirName, ACCESS_READ);
		if (BNULL == srcDirLock)
			break;

		destDirLock = Lock(ucmed->ucmed_destDirName, ACCESS_READ);
		if (BNULL == destDirLock)
			break;

		Result = DoMethod(uev->uev_UndoStep->ust_FileTransObj, SCCM_FileTrans_Move,
			srcDirLock, destDirLock,
			ucmed->ucmed_srcName,
			uev->uev_NewPosX, uev->uev_NewPosY);
		if (RETURN_OK != Result)
			break;

		Success = TRUE;
		} while (0);

	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (BNULL != destDirLock)
		UnLock(destDirLock);

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoCreateLinkEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR srcDirLock;
	BPTR destDirLock = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		ULONG Result;

		srcDirLock = Lock(ucmed->ucmed_srcDirName, ACCESS_READ);
		if (BNULL == srcDirLock)
			break;

		destDirLock = Lock(ucmed->ucmed_destDirName, ACCESS_READ);
		if (BNULL == destDirLock)
			break;

		Result = DoMethod(uev->uev_UndoStep->ust_FileTransObj, SCCM_FileTrans_CreateLink,
			srcDirLock, destDirLock,
			ucmed->ucmed_srcName,
			ucmed->ucmed_destName,
			uev->uev_NewPosX, uev->uev_NewPosY);
		if (RETURN_OK != Result)
			break;

		Success = TRUE;
		} while (0);

	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (BNULL != destDirLock)
		UnLock(destDirLock);

	return Success;
}

//----------------------------------------------------------------------------

static STRPTR UndoBuildIconName(CONST_STRPTR ObjName)
{
	STRPTR iconName = AllocPathBuffer();

	if (iconName)
		{
		stccpy(iconName, ObjName, Max_PathLen);
		SafeStrCat(iconName, ".info", Max_PathLen);
		}

	return iconName;
}

//----------------------------------------------------------------------------

static BOOL MoveIconTo(CONST_STRPTR DirName, CONST_STRPTR IconName, LONG xNew, LONG yNew)
{
	BOOL Success = FALSE;
	struct internalScaWindowTask *iwt;
	struct ScaIconNode *in = NULL;

	do	{
		struct ScaIconNode *MovedIconList = NULL;
		LONG x0, y0;
		struct ExtGadget *gg;

		if (!UndoFindWindowAndIcon(DirName, IconName, &iwt, &in))
			break;

		SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &MovedIconList, in);

		RemoveIcons(iwt, &MovedIconList);	// visibly remove icon from window

		gg = (struct ExtGadget *) in->in_Icon;

		// Adjust icon position
		x0 = gg->LeftEdge - gg->BoundsLeftEdge;

		gg->LeftEdge = xNew;
		gg->BoundsLeftEdge = gg->LeftEdge - x0;

		y0 = gg->TopEdge - gg->BoundsTopEdge;

		gg->TopEdge = yNew;
		gg->BoundsTopEdge = gg->TopEdge - y0;

		RefreshIconList(iwt, MovedIconList, NULL);	// draw icon at new position in window

		SCA_MoveIconNode(&MovedIconList, &iwt->iwt_WindowTask.wt_IconList, in);

		Success = TRUE;
		} while (0);

	if (iwt)
		{
		if (in)
			ScalosUnLockIconList(iwt);
		SCA_UnLockWindowList();
		}

	return Success;
}

//----------------------------------------------------------------------------

static struct internalScaWindowTask *UndoFindWindowByDir(CONST_STRPTR DirName)
{
	struct internalScaWindowTask *iwt = NULL;
	BPTR DirLock;

	DirLock = Lock(DirName, ACCESS_READ);
	if (DirLock)
		{
		struct ScaWindowStruct *ws;

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		debugLock_d1(DirLock);

		for (ws=winlist.wl_WindowStruct; (NULL == iwt) && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (ws->ws_Lock && (LOCK_SAME == ScaSameLock(DirLock, ws->ws_Lock)))
				{
				iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
				d1(KPrintF("%s/%s/%ld: Found  iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
				break;
				}
			}

		if (NULL == iwt)
			SCA_UnLockWindowList();

		UnLock(DirLock);
		}

	d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	return iwt;
}

//----------------------------------------------------------------------------

static struct internalScaWindowTask *UndoFindWindowByWindowTask(const struct ScaWindowTask *wt)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	d1(kprintf("%s/%s/%ld: START  wt=%08lx\n", __FILE__, __FUNC__, __LINE__, wt));

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (wt == ws->ws_WindowTask)
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
			d1(KPrintF("%s/%s/%ld: Found  iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
			return iwt;
			}
		}

	SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: iwt=NULL\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
}

//----------------------------------------------------------------------------

static struct ScaIconNode *UndoFindIconByName(struct internalScaWindowTask *iwt, CONST_STRPTR IconName)
{
	struct ScaIconNode *inFound = NULL;
	struct ScaIconNode *in;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosLockIconListShared(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (0 == Stricmp(GetIconName(in), IconName))
			{
			inFound = in;
			d1(kprintf("%s/%s/%ld: inFound=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, inFound, GetIconName(inFound)));
			break;
			}
		}

	if (NULL == inFound)
		ScalosUnLockIconList(iwt);

	d1(kprintf("%s/%s/%ld: inFound=%08lx\n", __FILE__, __FUNC__, __LINE__, inFound));

	return inFound;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoChangeIconPosEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	return MoveIconTo(uev->uev_Data.uev_IconData.uid_DirName,
		uev->uev_Data.uev_IconData.uid_IconName,
		uev->uev_OldPosX,
		uev->uev_OldPosY);
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoChangeIconPosEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	return MoveIconTo(uev->uev_Data.uev_IconData.uid_DirName,
		uev->uev_Data.uev_IconData.uid_IconName,
		uev->uev_NewPosX,
		uev->uev_NewPosY);
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeCopyMoveData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;

		if (ucmed->ucmed_srcDirName)
			{
			FreeCopyString(ucmed->ucmed_srcDirName);
			ucmed->ucmed_srcDirName = NULL;
			}
		if (ucmed->ucmed_destDirName)
			{
			FreeCopyString(ucmed->ucmed_destDirName);
			ucmed->ucmed_destDirName = NULL;
			}
		if (ucmed->ucmed_srcName)
			{
			FreeCopyString(ucmed->ucmed_srcName);
			ucmed->ucmed_srcName = NULL;
			}
		if (ucmed->ucmed_destName)
			{
			FreeCopyString(ucmed->ucmed_destName);
			ucmed->ucmed_destName = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeNewDrawerData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoNewDrawerData *und = &uev->uev_Data.uev_NewDrawerData;

		if (und->und_DirName)
			{
			FreeCopyString(und->und_DirName);
			und->und_DirName = NULL;
			}
		if (und->und_srcName)
			{
			FreeCopyString(und->und_srcName);
			und->und_srcName = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}
//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeIconData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		if (uev->uev_Data.uev_IconData.uid_DirName)
			{
			FreeCopyString(uev->uev_Data.uev_IconData.uid_DirName);
			uev->uev_Data.uev_IconData.uid_DirName = NULL;
			}
		if (uev->uev_Data.uev_IconData.uid_IconName)
			{
			FreeCopyString(uev->uev_Data.uev_IconData.uid_IconName);
			uev->uev_Data.uev_IconData.uid_IconName = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeCleanupData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoCleanupData *ucd = &uev->uev_Data.uev_CleanupData;

		if (ucd->ucd_WindowTitle)
			{
			FreeCopyString(ucd->ucd_WindowTitle);
			ucd->ucd_WindowTitle = NULL;
			}
		if (ucd->ucd_Icons)
			{
			ScalosFree(ucd->ucd_Icons);
			ucd->ucd_Icons = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeSnapshotData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoSnaphotIconData *usid = &uev->uev_Data.uev_SnapshotData;

		if (usid->usid_DirName)
			{
			FreeCopyString(usid->usid_DirName);
			usid->usid_DirName = NULL;
			}
		if (usid->usid_IconName)
			{
			FreeCopyString(usid->usid_IconName);
			usid->usid_IconName = NULL;
			}
		if (usid->usid_IconObj)
			{
			DisposeIconObject(usid->usid_IconObj);
			usid->usid_IconObj = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeSizeWindowData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoSizeWindowData *uswd = &uev->uev_Data.uev_SizeWindowData;

		if (uswd->uswd_WindowTitle)
			{
			FreeCopyString(uswd->uswd_WindowTitle);
			uswd->uswd_WindowTitle = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeSetProtectionData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoSetProtectionData *uspd = &uev->uev_Data.uev_SetProtectionData;

		if (uspd->uspd_DirName)
			{
			FreeCopyString(uspd->uspd_DirName);
			uspd->uspd_DirName = NULL;
			}
		if (uspd->uspd_IconName)
			{
			FreeCopyString(uspd->uspd_IconName);
			uspd->uspd_IconName = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeSetCommentData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoSetCommentData *uscd = &uev->uev_Data.uev_SetCommentData;

		if (uscd->uscd_DirName)
			{
			FreeCopyString(uscd->uscd_DirName);
			uscd->uscd_DirName = NULL;
			}
		if (uscd->uscd_IconName)
			{
			FreeCopyString(uscd->uscd_IconName);
			uscd->uscd_IconName = NULL;
			}
		if (uscd->uscd_OldComment)
			{
			FreeCopyString(uscd->uscd_OldComment);
			uscd->uscd_OldComment = NULL;
			}
		if (uscd->uscd_NewComment)
			{
			FreeCopyString(uscd->uscd_NewComment);
			uscd->uscd_NewComment = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeSetToolTypesData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoSetToolTypesData *ustd = &uev->uev_Data.uev_SetToolTypesData;

		if (ustd->ustd_DirName)
			{
			FreeCopyString(ustd->ustd_DirName);
			ustd->ustd_DirName = NULL;
			}
		if (ustd->ustd_IconName)
			{
			FreeCopyString(ustd->ustd_IconName);
			ustd->ustd_IconName = NULL;
			}
		if (ustd->ustd_OldToolTypes)
			{
			ScalosFree(ustd->ustd_OldToolTypes);
			ustd->ustd_OldToolTypes = NULL;
			}
		if (ustd->ustd_NewToolTypes)
			{
			ScalosFree(ustd->ustd_NewToolTypes);
			ustd->ustd_NewToolTypes = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeChangeIconObjectData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoChangeIconObjectData *uciod = &uev->uev_Data.uev_ChangeIconObjectData;

		if (uciod->uciod_DirName)
			{
			FreeCopyString(uciod->uciod_DirName);
			uciod->uciod_DirName = NULL;
			}
		if (uciod->uciod_IconName)
			{
			FreeCopyString(uciod->uciod_IconName);
			uciod->uciod_IconName = NULL;
			}
		if (uciod->uciod_OldIconObject)
			{
			DisposeIconObject(uciod->uciod_OldIconObject);
			uciod->uciod_OldIconObject = NULL;
			}
		if (uciod->uciod_NewIconObject)
			{
			DisposeIconObject(uciod->uciod_NewIconObject);
			uciod->uciod_NewIconObject = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}

//----------------------------------------------------------------------------

static SAVEDS(void) UndoDisposeCloseWindowData(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
	if (uev)
		{
		struct UndoCloseWindowData *ucwd = &uev->uev_Data.uev_CloseWindowData;

		if (ucwd->ucwd_DirName)
			{
			FreeCopyString(ucwd->ucwd_DirName);
			ucwd->ucwd_DirName = NULL;
			}
		if (ucwd->ucwd_WindowTitle)
			{
			FreeCopyString(ucwd->ucwd_WindowTitle);
			ucwd->ucwd_WindowTitle = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld: END   uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));
}
//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoCleanupEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	struct internalScaWindowTask *iwt;
	BOOL Success = FALSE;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));

	do	{
		struct ScaIconNode *in;

		d1(kprintf("%s/%s/%ld: ucd_IconCount=%lu\n", __FILE__, __FUNC__, __LINE__, uev->uev_Data.uev_CleanupData.ucd_IconCount));
		d1(kprintf("%s/%s/%ld: ucd_WindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__, uev->uev_Data.uev_CleanupData.ucd_WindowTask));

		iwt = UndoFindWindowByWindowTask(uev->uev_Data.uev_CleanupData.ucd_WindowTask);
		if (NULL == iwt)
			break;

		ScalosLockIconListShared(iwt);

		if (iwt->iwt_WindowTask.wt_Window)
			{
			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort, 0, 0,
				iwt->iwt_WindowTask.wt_Window->Width - 1,
				iwt->iwt_WindowTask.wt_Window->Height -1);
			}

		for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			const struct UndoCleanupIconEntry *ucie;

			ucie = UndoFindCleanupIconEntry(uev, in);
			if (ucie)
				{
				LONG x0, y0;
				struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

				d1(kprintf("%s/%s/%ld: in=%08lx <%s>  Left=%ld  Top=%ld\n", \
					__FILE__, __FUNC__, __LINE__, in, GetIconName(in), \
					ucie->ucin_Left, ucie->ucin_Top));

				// Adjust icon position
				x0 = gg->LeftEdge - gg->BoundsLeftEdge;

				gg->LeftEdge = ucie->ucin_Left;
				gg->BoundsLeftEdge = gg->LeftEdge - x0;

				y0 = gg->TopEdge - gg->BoundsTopEdge;

				gg->TopEdge = ucie->ucin_Top;
				gg->BoundsTopEdge = gg->TopEdge - y0;
				}
			}

		RefreshIconList(iwt, iwt->iwt_WindowTask.wt_IconList, NULL);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
			SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

		ScalosUnLockIconList(iwt);

		Success = TRUE;
		} while (0);

	if (iwt)
		SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoCleanupEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	struct internalScaWindowTask *iwt;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		iwt = UndoFindWindowByWindowTask(uev->uev_Data.uev_CleanupData.ucd_WindowTask);
		if (NULL == iwt)
			break;

		switch (uev->uev_Data.uev_CleanupData.ucd_CleanupMode)
			{
		case CLEANUP_ByName:
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpByName);
			break;
		case CLEANUP_ByDate:
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpByDate);
			break;
		case CLEANUP_BySize:
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpBySize);
			break;
		case CLEANUP_ByType:
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpByType);
			break;
		case CLEANUP_Default:
		default:
			//TODO
			break;
			}
		} while (0);

	if (iwt)
		SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static const struct UndoCleanupIconEntry *UndoFindCleanupIconEntry(const struct UndoEvent *uev, const struct ScaIconNode *in)
{
	const struct UndoCleanupIconEntry *ucie = uev->uev_Data.uev_CleanupData.ucd_Icons;
	ULONG n;

	for (n = 0; n < uev->uev_Data.uev_CleanupData.ucd_IconCount; n++, ucie++)
		{
		if (ucie->ucin_IconNode == in)
			return ucie;
		}

	return NULL;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoLeaveOutEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	struct UndoSnaphotIconData *usid = &uev->uev_Data.uev_SnapshotData;
	struct internalScaWindowTask *iwt;
	BPTR oldDir = BNULL;
	STRPTR Path = NULL;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		BPTR iconDirLock;

		iwt = UndoFindWindowByDir(usid->usid_DirName);
		d1(kprintf("%s/%s/%ld:  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		if (NULL == iwt)
			break;

		oldDir = CurrentDir(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

		iconDirLock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		debugLock_d1(iconDirLock);
		if (BNULL == iconDirLock)
			break;

		// !!! side effects: UnLocks(iconDirLock) and frees <in> !!!
		PutAwayIcon((struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask,
			iconDirLock, usid->usid_IconName, TRUE);

		// add icon to owning drawer window
		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_ReadIcon,
			usid->usid_IconName,
			NULL);

		// Special handling for "view all" textwindows.
		// here both object and icon are separate entries, which must be removed both!
		if (!IsIwtViewByIcon(iwt) && IsShowAll(iwt->iwt_WindowTask.mt_WindowStruct))
			{
			Path = AllocPathBuffer();
			if (NULL == Path)
				break;

			stccpy(Path, usid->usid_IconName, Max_PathLen);
			SafeStrCat(Path, ".info", Max_PathLen);

			DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_ReadIcon,
				Path,
				NULL);
			}

		Success = TRUE;
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (Path)
		FreePathBuffer(Path);
	if (iwt)
		SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoLeaveOutEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	struct UndoSnaphotIconData *usid = &uev->uev_Data.uev_SnapshotData;
	struct internalScaWindowTask *iwt;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		BPTR iconDirLock;

		iwt = UndoFindWindowByDir(usid->usid_DirName);
		if (NULL == iwt)
			break;

		iconDirLock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		debugLock_d1(iconDirLock);
		if (BNULL == iconDirLock)
			break;

		DoLeaveOutIcon(iwt, iconDirLock, usid->usid_IconName,
			uev->uev_NewPosX, uev->uev_NewPosY);

		Success = TRUE;
		} while (0);

	if (iwt)
		SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoSnapshotEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	struct internalScaWindowTask *iwt;
	struct ScaIconNode *in = NULL;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSnaphotIconData *usid = &uev->uev_Data.uev_SnapshotData;
		const struct IBox *pWinRect;
		ULONG IconViewMode;
		ULONG ddFlags;
		LONG WinCurrentX, WinCurrentY;

		if (!UndoFindWindowAndIcon(usid->usid_DirName, usid->usid_IconName, &iwt, &in))
			break;

		d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		d1(kprintf("%s/%s/%ld: usid_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, usid->usid_IconName));
		d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		if (uev->uev_OldPosX != uev->uev_NewPosX || uev->uev_OldPosY != uev->uev_NewPosY)
			{
			if (!MoveIconTo(usid->usid_DirName,
				usid->usid_IconName,
				uev->uev_OldPosX, uev->uev_OldPosY))
				break;
			}

		GetAttr(IDTA_ViewModes, usid->usid_IconObj, &IconViewMode);
		GetAttr(IDTA_Flags, usid->usid_IconObj, &ddFlags);
		GetAttr(IDTA_WindowRect, usid->usid_IconObj, (ULONG *) &pWinRect);
		GetAttr(IDTA_WinCurrentX, usid->usid_IconObj, (ULONG *) &WinCurrentX);
		GetAttr(IDTA_WinCurrentY, usid->usid_IconObj, (ULONG *) &WinCurrentY);

		SetAttrs(in->in_Icon,
			IDTA_ViewModes, IconViewMode,
			IDTA_Flags, ddFlags,
			pWinRect ? IDTA_WindowRect : TAG_IGNORE, (ULONG) pWinRect,
			IDTA_WinCurrentY, WinCurrentY,
			IDTA_WinCurrentX, WinCurrentX,
			TAG_END);

		if (usid->usid_SaveIcon)
			{
			ULONG Result;

			Result = SaveIconObject(usid->usid_IconObj,
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				usid->usid_IconName,
				FALSE,
				NULL);

			if (RETURN_OK != Result)
				break;
			}

		Success = TRUE;
		} while (0);

	if (iwt)
		{
		if (in)
			ScalosUnLockIconList(iwt);
		SCA_UnLockWindowList();
		}

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoSnapshotPosEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	struct internalScaWindowTask *iwt;
	struct ScaIconNode *in = NULL;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSnaphotIconData *usid = &uev->uev_Data.uev_SnapshotData;

		if (!UndoFindWindowAndIcon(usid->usid_DirName, usid->usid_IconName, &iwt, &in))
			break;

		d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		d1(kprintf("%s/%s/%ld: usid_IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, usid->usid_IconName));

		if (uev->uev_OldPosX != uev->uev_NewPosX || uev->uev_OldPosY != uev->uev_NewPosY)
			{
			if (!MoveIconTo(usid->usid_DirName,
				usid->usid_IconName,
				uev->uev_NewPosX, uev->uev_NewPosY))
				break;
			}

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		if (UNDO_Snapshot == uev->uev_Type)
			Success = UndoSnapshotIcon(iwt, in);
		else
			{
			struct ScaIconNode *IconList;

			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &IconList, in);

			RemoveIcons(iwt, &IconList);	   // visibly remove icon from window

			Success = UndoUnsnapshotIcon(iwt, in, usid->usid_SaveIcon);

			SCA_MoveIconNode(&IconList, &iwt->iwt_WindowTask.wt_LateIconList, in);

			if (IsIwtViewByIcon(iwt))
				{
				d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
				d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
					SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);
				}
			}

		SCA_UnLockWindowList();
		} while (0);

	if (iwt)
		{
		if (in)
			ScalosUnLockIconList(iwt);
		SCA_UnLockWindowList();
		}

	d1(kprintf("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL UndoSnapshotIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BOOL Success = FALSE;
	ULONG IconType = 0;

	d1(KPrintF("%s/%s/%ld:  START iwt=%08lx  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (0 == IconType)
		return Success;

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_Snapshot, in);
		return TRUE;
		}
	else
		{
		BPTR destDirLock;

		SetIconWindowRect(in);

		if (in->in_DeviceIcon)
			{
			destDirLock = DiskInfoLock(in);

			if (destDirLock)
				{
				LONG Result;

				Result = ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);

				if (RETURN_OK == Result)
					Success = TRUE;

				UnLock(destDirLock);
				}
			}
		else
			{
			LONG Result;

			if (in->in_Lock)
				destDirLock = in->in_Lock;
			else
				destDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

			Result = ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);

			if (RETURN_OK == Result)
				Success = TRUE;
			}

		in->in_Flags &= ~INF_FreeIconPosition;
		in->in_SupportFlags &= ~INF_SupportsSnapshot;
		in->in_SupportFlags |= INF_SupportsUnSnapshot;
		}

	d1(KPrintF("%s/%s/%ld: END in=%08lx  <%s>  inFlags=%08lx  Success=%ld\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), in->in_Flags, Success));

	return Success;
}

//----------------------------------------------------------------------------

static BOOL UndoUnsnapshotIcon(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in, BOOL SaveIcon)
{
	BOOL Success = FALSE;
	ULONG IconType = 0;

	d1(KPrintF("%s/%s/%ld:  iwt=%08lx  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (0 == IconType)
		return Success;

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_UnSnapshot, in);
		return TRUE;
		}
	else
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
		BPTR destDirLock;
		WORD LeftEdge, TopEdge;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		SetIconWindowRect(in);

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		LeftEdge = gg->LeftEdge;
		TopEdge = gg->TopEdge;

		if (in->in_DeviceIcon)
			{
			destDirLock = DiskInfoLock(in);

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			if (destDirLock)
				{
		
				gg->LeftEdge = gg->TopEdge = NO_ICON_POSITION_SHORT;

				if (SaveIcon)
					{
					LONG Result;

					Result = ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);

					if (RETURN_OK == Result)
						Success = TRUE;
					}
				else
					{
					Success = TRUE;
					}

				UnLock(destDirLock);
				}
			}
		else
			{
			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			if (in->in_Lock)
				destDirLock = in->in_Lock;
			else
				destDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
                   
			gg->LeftEdge = gg->TopEdge = NO_ICON_POSITION_SHORT;

			if (SaveIcon)
				{
				LONG Result;

				Result = ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);

				if (RETURN_OK == Result)
					Success = TRUE;
				}
			else
				{
				Success = TRUE;
				}

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
			}

		gg->LeftEdge = LeftEdge;
		gg->TopEdge = TopEdge;

		in->in_Flags |= INF_FreeIconPosition;
		in->in_SupportFlags |= INF_SupportsSnapshot;
		in->in_SupportFlags &= ~INF_SupportsUnSnapshot;
		}

	d1(KPrintF("%s/%s/%ld: END in=%08lx  <%s>  inFlags=%08lx  Success=%ld\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), in->in_Flags, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoRenameEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BPTR srcDirLock;
	BOOL Success = FALSE;
	STRPTR destIconName = NULL;
	STRPTR srcIconName = NULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		BPTR oldDir;

		d1(kprintf("%s/%s/%ld: ucmed_srcDirName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_srcDirName));

		srcDirLock = Lock(ucmed->ucmed_srcDirName, ACCESS_READ);
		if (BNULL == srcDirLock)
			break;

		debugLock_d1(srcDirLock);

		destIconName = UndoBuildIconName(ucmed->ucmed_destName);
		if (NULL == destIconName)
			break;

		srcIconName = UndoBuildIconName(ucmed->ucmed_srcName);
		if (NULL == srcIconName)
			break;

		d1(kprintf("%s/%s/%ld: ucmed_srcName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_srcName));
		d1(kprintf("%s/%s/%ld: ucmed_destName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_destName));

		oldDir = CurrentDir(srcDirLock);

		Success = Rename(ucmed->ucmed_destName, ucmed->ucmed_srcName);

		CurrentDir(oldDir);

		if (!Success)
			break;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		// Try to also rename icon
		oldDir = CurrentDir(srcDirLock);
		(void) Rename(destIconName, srcIconName);
		CurrentDir(oldDir);
		} while (0);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (srcIconName)
		FreePathBuffer(srcIconName);
	if (destIconName)
		FreePathBuffer(destIconName);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoRenameEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BPTR srcDirLock;
	BOOL Success = FALSE;
	STRPTR destIconName = NULL;
	STRPTR srcIconName = NULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;
		BPTR oldDir;

		d1(kprintf("%s/%s/%ld: ucmed_srcDirName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_srcDirName));

		srcDirLock = Lock(ucmed->ucmed_srcDirName, ACCESS_READ);
		if (BNULL == srcDirLock)
			break;

		debugLock_d1(srcDirLock);

		destIconName = UndoBuildIconName(ucmed->ucmed_destName);
		if (NULL == destIconName)
			break;

		srcIconName = UndoBuildIconName(ucmed->ucmed_srcName);
		if (NULL == srcIconName)
			break;

		d1(kprintf("%s/%s/%ld: .ucmed_srcName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_srcName));
		d1(kprintf("%s/%s/%ld: .ucmed_destName=<%s>\n", __FILE__, __FUNC__, __LINE__, ucmed->ucmed_destName));

		oldDir = CurrentDir(srcDirLock);

		Success = Rename(ucmed->ucmed_srcName, ucmed->ucmed_destName);

		CurrentDir(oldDir);

		if (!Success)
			break;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		// Try to also rename icon
		oldDir = CurrentDir(srcDirLock);
		(void) Rename(srcIconName, destIconName);
		CurrentDir(oldDir);
		} while (0);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (srcIconName)
		FreePathBuffer(srcIconName);
	if (destIconName)
		FreePathBuffer(destIconName);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoRelabelEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	STRPTR newName = NULL;
	STRPTR driveName;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;

		driveName = AllocPathBuffer();
		if (NULL == driveName)
			break;

		newName = AllocPathBuffer();
		if (NULL == newName)
			break;

		stccpy(driveName, ucmed->ucmed_destName, Max_PathLen);
		SafeStrCat(driveName, ":", Max_PathLen);

		stccpy(newName, ucmed->ucmed_srcName, Max_PathLen);
		StripTrailingColon(newName);

		Success = Relabel(driveName, newName);
		} while (0);

	if (driveName)
		FreePathBuffer(driveName);
	if (newName)
		FreePathBuffer(newName);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoRelabelEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));

	do	{
		struct UndoCopyMoveEventData *ucmed = &uev->uev_Data.uev_CopyMoveData;

		Success	= Relabel(ucmed->ucmed_srcName, ucmed->ucmed_destName);
		} while (0);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoSizeWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	struct internalScaWindowTask *iwt;
	BOOL Success = FALSE;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));

	do	{
		struct UndoSizeWindowData *uswd = &uev->uev_Data.uev_SizeWindowData;

		d1(kprintf("%s/%s/%ld: ucd_WindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__, uev->uev_Data.uev_CleanupData.ucd_WindowTask));

		iwt = UndoFindWindowByWindowTask(uswd->uswd_WindowTask);
		if (NULL == iwt)
			break;

		ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);

		d1(kprintf("%s/%s/%ld: XOffset=%ld  YOffset=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_WindowTask.wt_XOffset, iwt->iwt_WindowTask.wt_YOffset));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove,
			uswd->uswd_OldVirtX - iwt->iwt_WindowTask.wt_XOffset,
			uswd->uswd_OldVirtY - iwt->iwt_WindowTask.wt_YOffset);

		d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
			uswd->uswd_OldLeft, uswd->uswd_OldTop, \
			uswd->uswd_OldWidth, uswd->uswd_OldHeight));

		if (iwt->iwt_WindowTask.wt_Window)
			{
			ChangeWindowBox(iwt->iwt_WindowTask.wt_Window,
				uswd->uswd_OldLeft,
				uswd->uswd_OldTop,
				uswd->uswd_OldWidth,
				uswd->uswd_OldHeight);
			}

		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);

		Success = TRUE;
		} while (0);

	if (iwt)
		SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoSizeWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	struct internalScaWindowTask *iwt;
	BOOL Success = FALSE;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START  uev=%08lx\n", __FILE__, __FUNC__, __LINE__, uev));

	do	{
		struct UndoSizeWindowData *uswd = &uev->uev_Data.uev_SizeWindowData;

		d1(kprintf("%s/%s/%ld: ucd_WindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__, uev->uev_Data.uev_CleanupData.ucd_WindowTask));

		iwt = UndoFindWindowByWindowTask(uswd->uswd_WindowTask);
		if (NULL == iwt)
			break;

		ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);

		d1(kprintf("%s/%s/%ld: XOffset=%ld  YOffset=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_WindowTask.wt_XOffset, iwt->iwt_WindowTask.wt_YOffset));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove,
			uswd->uswd_NewVirtX - iwt->iwt_WindowTask.wt_XOffset,
			uswd->uswd_NewVirtY - iwt->iwt_WindowTask.wt_YOffset);

		d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
			uswd->uswd_NewLeft, uswd->uswd_NewTop, \
			uswd->uswd_NewWidth, uswd->uswd_NewHeight));

		if (iwt->iwt_WindowTask.wt_Window)
			{
			ChangeWindowBox(iwt->iwt_WindowTask.wt_Window,
				uswd->uswd_NewLeft,
				uswd->uswd_NewTop,
				uswd->uswd_NewWidth,
				uswd->uswd_NewHeight);
			}

		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);

		Success = TRUE;
		} while (0);

	if (iwt)
		SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoNewDrawerEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR srcDirLock;
	STRPTR iconName = NULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoNewDrawerData *und = &uev->uev_Data.uev_NewDrawerData;
		LONG Result;

		srcDirLock = Lock(und->und_DirName, ACCESS_READ);
		d1(kprintf("%s/%s/%ld: srcDirLock=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, srcDirLock, und->und_DirName));
		if (BNULL == srcDirLock)
			break;

		if (0 == strlen(und->und_srcName))
			break;

		Result = DoMethod(uev->uev_UndoStep->ust_FileTransObj,
			SCCM_FileTrans_Delete,
			srcDirLock, und->und_srcName);

		if (RETURN_OK != Result)
			break;	// SCCM_FileTrans_Delete failed

		if (und->und_CreateIcon)
			{
			CONST_STRPTR iconExt = ".info";

			iconName = ScalosAlloc(1 + strlen(und->und_srcName) + strlen(iconExt));
			if (NULL == iconName)
				break;

			strcpy(iconName, und->und_srcName);
			strcat(iconName, iconExt);

			Result = DoMethod(uev->uev_UndoStep->ust_FileTransObj,
				SCCM_FileTrans_Delete,
				srcDirLock, iconName);

			if (RETURN_OK != Result)
				break;	// SCCM_FileTrans_Delete failed
			}

		Success = TRUE;
		} while (0);

	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (iconName)
		ScalosFree(iconName);

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoNewDrawerEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR srcDirLock;
	BPTR newDirLock = BNULL;
	Object *IconObj = NULL;
	BPTR oldDir = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoNewDrawerData *und = &uev->uev_Data.uev_NewDrawerData;

		srcDirLock = Lock(und->und_DirName, ACCESS_READ);
		if (BNULL == srcDirLock)
			break;

		oldDir = CurrentDir(srcDirLock);

		newDirLock = CreateDir(und->und_srcName);
		debugLock_d1(newDirLock);
		if (BNULL == newDirLock)
			break;

		if (und->und_CreateIcon)
			{
			LONG rc;
			struct ScaUpdateIcon_IW upd;

			IconObj	= GetDefIconObject(WBDRAWER, NULL);

			d1(kprintf(__FILE__ "/%s/%ld: IconObj=%08lx\n", __FUNC__, __LINE__, IconObj));

			if (NULL == IconObj)
				break;

			upd.ui_iw_Lock = srcDirLock;
			upd.ui_iw_Name = und->und_srcName;
			upd.ui_IconType = ICONTYPE_NONE;

			rc = PutIconObjectTags(IconObj, und->und_srcName,
				TAG_END);
			d1(kprintf(__FILE__ "/%s/%ld: PutIconObjectTags returned rc=%ld\n", __FUNC__, __LINE__, rc));
			if (RETURN_OK != rc)
				break;

			SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));
			}

		Success = TRUE;
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (IconObj)
		DisposeIconObject(IconObj);
	if (BNULL != srcDirLock)
		UnLock(srcDirLock);
	if (BNULL != newDirLock)
		UnLock(newDirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoSetProtectionEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetProtectionData *uspd = &uev->uev_Data.uev_SetProtectionData;
		struct ScaUpdateIcon_IW upd;

		dirLock = Lock(uspd->uspd_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = uspd->uspd_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		Success = SetProtection(uspd->uspd_IconName, uspd->uspd_OldProtection);

		if (Success)
			SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoSetProtectionEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetProtectionData *uspd = &uev->uev_Data.uev_SetProtectionData;
		struct ScaUpdateIcon_IW upd;

		dirLock = Lock(uspd->uspd_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = uspd->uspd_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		Success = SetProtection(uspd->uspd_IconName, uspd->uspd_OldProtection);

		if (Success)
			SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoSetCommentEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetCommentData *uscd = &uev->uev_Data.uev_SetCommentData;
		struct ScaUpdateIcon_IW upd;

		dirLock = Lock(uscd->uscd_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = uscd->uscd_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		Success = SetComment(uscd->uscd_IconName, uscd->uscd_OldComment);

		if (Success)
			SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoSetCommentEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetCommentData *uscd = &uev->uev_Data.uev_SetCommentData;
		struct ScaUpdateIcon_IW upd;

		dirLock = Lock(uscd->uscd_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = uscd->uscd_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		Success = SetComment(uscd->uscd_IconName, uscd->uscd_NewComment);

		if (Success)
			SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoSetToolTypesEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;
	struct internalScaWindowTask *iwt = NULL;
	struct ScaIconNode *in = NULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetToolTypesData *ustd = &uev->uev_Data.uev_SetToolTypesData;
		struct ScaUpdateIcon_IW upd;
		LONG rc;

		dirLock = Lock(ustd->ustd_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		if (!UndoFindWindowAndIcon(ustd->ustd_DirName, ustd->ustd_IconName, &iwt, &in))
			break;

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = ustd->ustd_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		SetAttrs(in->in_Icon,
			IDTA_ToolTypes, (ULONG) ustd->ustd_OldToolTypes,
			TAG_END);

		rc = PutIconObjectTags(in->in_Icon, ustd->ustd_IconName,
			ICONA_NoNewImage, TRUE,
			TAG_END);
		d1(kprintf(__FILE__ "/%s/%ld: PutIconObjectTags returned rc=%ld\n", __FUNC__, __LINE__, rc));
		if (RETURN_OK != rc)
			break;

		SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));

		Success = TRUE;
		} while (0);

	if (iwt)
		{
		if (in)
			ScalosUnLockIconList(iwt);
		SCA_UnLockWindowList();
		}
	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoSetToolTypesEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;
	struct internalScaWindowTask *iwt = NULL;
	struct ScaIconNode *in = NULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoSetToolTypesData *ustd = &uev->uev_Data.uev_SetToolTypesData;
		struct ScaUpdateIcon_IW upd;
		LONG rc;

		dirLock = Lock(ustd->ustd_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		if (!UndoFindWindowAndIcon(ustd->ustd_DirName, ustd->ustd_IconName, &iwt, &in))
			break;

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = ustd->ustd_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		SetAttrs(in->in_Icon,
			IDTA_ToolTypes, (ULONG) ustd->ustd_OldToolTypes,
			TAG_END);

		rc = PutIconObjectTags(in->in_Icon, ustd->ustd_IconName,
			ICONA_NoNewImage, TRUE,
			TAG_END);
		d1(kprintf(__FILE__ "/%s/%ld: PutIconObjectTags returned rc=%ld\n", __FUNC__, __LINE__, rc));
		if (RETURN_OK != rc)
			break;

		SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));

		Success = TRUE;
		} while (0);

	if (iwt)
		{
		if (in)
			ScalosUnLockIconList(iwt);
		SCA_UnLockWindowList();
		}
	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoChangeIconObjectEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoChangeIconObjectData *uciod = &uev->uev_Data.uev_ChangeIconObjectData;
		struct ScaUpdateIcon_IW upd;
		LONG rc;

		dirLock = Lock(uciod->uciod_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = uciod->uciod_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		rc = PutIconObjectTags(uciod->uciod_OldIconObject, uciod->uciod_IconName,
			TAG_END);
		d1(kprintf(__FILE__ "/%s/%ld: PutIconObjectTags returned rc=%ld\n", __FUNC__, __LINE__, rc));
		if (RETURN_OK != rc)
			break;

		SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));

		Success = TRUE;
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoChangeIconObjectEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;
	BPTR dirLock;
	BPTR oldDir = BNULL;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoChangeIconObjectData *uciod = &uev->uev_Data.uev_ChangeIconObjectData;
		struct ScaUpdateIcon_IW upd;
		LONG rc;

		dirLock = Lock(uciod->uciod_DirName, ACCESS_READ);
		debugLock_d1(dirLock);
		if (BNULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		upd.ui_iw_Lock = dirLock;
		upd.ui_iw_Name = uciod->uciod_IconName;
		upd.ui_IconType = ICONTYPE_NONE;

		rc = PutIconObjectTags(uciod->uciod_NewIconObject, uciod->uciod_IconName,
			TAG_END);
		d1(kprintf(__FILE__ "/%s/%ld: PutIconObjectTags returned rc=%ld\n", __FUNC__, __LINE__, rc));
		if (RETURN_OK != rc)
			break;

		SCA_UpdateIcon(WSV_Type_IconWindow, &upd, sizeof(upd));

		Success = TRUE;
		} while (0);

	if (oldDir)
		CurrentDir(oldDir);
	if (BNULL != dirLock)
		UnLock(dirLock);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) UndoCloseWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCloseWindowData *ucwd = &uev->uev_Data.uev_CloseWindowData;
		struct IBox WindowBox;

		WindowBox.Left   = ucwd->ucwd_Left;
		WindowBox.Top    = ucwd->ucwd_Top;
		WindowBox.Width  = ucwd->ucwd_Width;
		WindowBox.Height = ucwd->ucwd_Height;

		Success = SCA_OpenIconWindowTags(
			SCA_Path, (ULONG) ucwd->ucwd_DirName,
			SCA_WindowRect, (ULONG) &WindowBox,
			SCA_ViewModes, ucwd->ucwd_Viewmodes,
			SCA_ShowAllMode, ucwd->ucwd_ViewAll,
			SCA_XOffset, ucwd->ucwd_VirtX,
			SCA_YOffset, ucwd->ucwd_VirtY,
			TAG_END);
		} while (0);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

static SAVEDS(LONG) RedoCloseWindowEvent(struct Hook *hook, APTR object, struct UndoEvent *uev)
{
	BOOL Success = FALSE;

	(void) hook;
	(void) object;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct UndoCloseWindowData *ucwd = &uev->uev_Data.uev_CloseWindowData;
		struct internalScaWindowTask *iwt;

		iwt = UndoFindWindowByDir(ucwd->ucwd_DirName);
		if (NULL == iwt)
			break;

		iwt->iwt_CloseWindow = TRUE;

		SCA_UnLockWindowList();

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld:  END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

// If window <iwt> is about to close, walk through Undo list
// and remove all entries which refer to the opening window
void UndoWindowSignalOpening(struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		{
		struct UndoStep *ust;

		ScalosObtainSemaphore(&UndoListListSemaphore);

		for (ust = (struct UndoStep *) globalUndoList.lh_Head;
			ust != (struct UndoStep *) &globalUndoList.lh_Tail; )
			{
			struct UndoStep *ustNext = (struct UndoStep *) ust->ust_Node.ln_Succ;
			BOOL Found = FALSE;
			struct UndoEvent *uev;

			d1(kprintf("%s/%s/%ld: ust=%08lx  \n", __FILE__, __FUNC__, __LINE__, ust));

			for (uev = (struct UndoEvent *) ust->ust_EventList.lh_Head;
				uev != (struct UndoEvent *) &ust->ust_EventList.lh_Tail;
				uev = (struct UndoEvent *) uev->uev_Node.ln_Succ)
				{
				d1(kprintf("%s/%s/%ld: uev=%08lx  uev_Type=%ld  uev_UndoHook=%08lx\n", __FILE__, __FUNC__, __LINE__, uev, uev->uev_Type, uev->uev_UndoHook));
				if (UNDO_CloseWindow == uev->uev_Type)
					{
					BPTR dirLock;

					dirLock = Lock(uev->uev_Data.uev_CloseWindowData.ucwd_DirName, ACCESS_READ);
					if (dirLock)
						{
						if (LOCK_SAME == SameLock(dirLock, iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock))
							Found = TRUE;

						UnLock(dirLock);
						}
					if (Found)
						break;
					}
				}

			if (Found)
				{
				UndoCount--;
				Remove(&ust->ust_Node);
				UndoDisposeStep(ust);
				}

			ust = ustNext;
			d1(kprintf("%s/%s/%ld: ust=%08lx  \n", __FILE__, __FUNC__, __LINE__, ust));
			}

		ScalosReleaseSemaphore(&UndoListListSemaphore);

		SetMenuOnOff(iwt);
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

// If window <iwt> is about to close, walk through Undo/Redo lists
// and remove all entries which refer to the disappearing window
void UndoWindowSignalClosing(struct internalScaWindowTask *iwt)
{
	struct UndoStep *ust;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	UndoAddEvent(iwt, UNDO_CloseWindow,
		UNDOTAG_WindowTask, iwt,
		TAG_END);

	ScalosObtainSemaphore(&UndoListListSemaphore);

	for (ust = (struct UndoStep *) globalUndoList.lh_Head;
		ust != (struct UndoStep *) &globalUndoList.lh_Tail; )
		{
		struct UndoStep *ustNext = (struct UndoStep *) ust->ust_Node.ln_Succ;
		BOOL Found = FALSE;
		struct UndoEvent *uev;

		d1(kprintf("%s/%s/%ld: ust=%08lx  \n", __FILE__, __FUNC__, __LINE__, ust));

		for (uev = (struct UndoEvent *) ust->ust_EventList.lh_Head;
			uev != (struct UndoEvent *) &ust->ust_EventList.lh_Tail;
			uev = (struct UndoEvent *) uev->uev_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: uev=%08lx  uev_Type=%ld  uev_UndoHook=%08lx\n", __FILE__, __FUNC__, __LINE__, uev, uev->uev_Type, uev->uev_UndoHook));
			if (UNDO_Cleanup == uev->uev_Type)
				{
				if (uev->uev_Data.uev_CleanupData.ucd_WindowTask == &iwt->iwt_WindowTask)
					{
					Found = TRUE;
					break;
					}
				}
			else if (UNDO_SizeWindow == uev->uev_Type)
				{
				if (uev->uev_Data.uev_SizeWindowData.uswd_WindowTask == &iwt->iwt_WindowTask)
					{
					Found = TRUE;
					break;
					}
				}
			}

		if (Found)
			{
			UndoCount--;
			Remove(&ust->ust_Node);
			UndoDisposeStep(ust);
			}

		ust = ustNext;
		d1(kprintf("%s/%s/%ld: ust=%08lx  \n", __FILE__, __FUNC__, __LINE__, ust));
		}

	ScalosReleaseSemaphore(&UndoListListSemaphore);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScalosObtainSemaphore(&RedoListListSemaphore);

	for (ust = (struct UndoStep *) globalRedoList.lh_Head;
		ust != (struct UndoStep *) &globalRedoList.lh_Tail; )
		{
		struct UndoStep *ustNext = (struct UndoStep *) ust->ust_Node.ln_Succ;
		BOOL Found = FALSE;
		struct UndoEvent *uev;

		d1(kprintf("%s/%s/%ld: ust=%08lx  \n", __FILE__, __FUNC__, __LINE__, ust));

		for (uev = (struct UndoEvent *) ust->ust_EventList.lh_Head;
			uev != (struct UndoEvent *) &ust->ust_EventList.lh_Tail;
			uev = (struct UndoEvent *) uev->uev_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: uev=%08lx  uev_Type=%ld  uev_UndoHook=%08lx\n", __FILE__, __FUNC__, __LINE__, uev, uev->uev_Type, uev->uev_UndoHook));
			if (UNDO_Cleanup == uev->uev_Type)
				{
				if (uev->uev_Data.uev_CleanupData.ucd_WindowTask == &iwt->iwt_WindowTask)
					{
					Found = TRUE;
					break;
					}
				}
			else if (UNDO_SizeWindow == uev->uev_Type)
				{
				if (uev->uev_Data.uev_SizeWindowData.uswd_WindowTask == &iwt->iwt_WindowTask)
					{
					Found = TRUE;
					break;
					}
				}
			}

		if (Found)
			{
			Remove(&ust->ust_Node);
			UndoDisposeStep(ust);
			}

		ust = ustNext;
		d1(kprintf("%s/%s/%ld: ust=%08lx  \n", __FILE__, __FUNC__, __LINE__, ust));
		}

	ScalosReleaseSemaphore(&RedoListListSemaphore);

	SetMenuOnOff(iwt);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static BOOL UndoFindWindowAndIcon(CONST_STRPTR DirName, CONST_STRPTR IconName,
	struct internalScaWindowTask **iwt, struct ScaIconNode **in)
{
	BOOL Found = FALSE;
	BPTR DirLock;

	do	{
		SCA_LockWindowList(SCA_LockWindowList_Shared);

		DirLock = Lock(DirName, ACCESS_READ);
		if (BNULL == DirLock)
			break;

		// First check whether icon is on desktop window (left-out)
		*iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

		ScalosLockIconListShared(*iwt);

		for ((*in)=(*iwt)->iwt_WindowTask.wt_IconList; *in; *in = (struct ScaIconNode *) (*in)->in_Node.mln_Succ)
			{
			if ((*in)->in_Lock && (LOCK_SAME == SameLock((*in)->in_Lock, DirLock)) &&
				(0 == Stricmp(GetIconName(*in), IconName)))
				{
				Found = TRUE;
				d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, *in, GetIconName(*in)));
				break;
				}
			}

		if (Found)
			break;	// icon found on desktop window

		// icon not found in desktop window
		ScalosUnLockIconList(*iwt);

		SCA_UnLockWindowList();

		*iwt = UndoFindWindowByDir(DirName);
		if (NULL == *iwt)
			break;

		*in = UndoFindIconByName(*iwt, IconName);
		if (NULL == in)
			break;

		Found = TRUE;
		} while (0);

	if (DirLock)
		UnLock(DirLock);
	if (!Found)
		{
		if (*in)
			ScalosUnLockIconList(*iwt);

		if (*iwt)
			SCA_UnLockWindowList();

		*iwt = NULL;
		*in = NULL;
		}

	return Found;
}

//----------------------------------------------------------------------------


