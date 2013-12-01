// DoubleClick.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/execbase.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <rexx/rxslib.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include <proto/rexxsyslib.h>
#include <proto/gadtools.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

enum DcrID { DCR_ID_End = 0, DCR_ID_WindowStruct, DCR_ID_IconNode };

struct DoubleClickRemember
	{
	WORD dcr_ID;
	union	{
		struct ScaWindowStruct *dcro_WindowStruct;
		struct ScaIconNode *dcro_IconNode;
		} dcr_Object;
	};

struct ARexxToolStartArg
	{
	BPTR asa_DirLock;
	STRPTR asa_ProgName;
	};

// alloc count of struct DoubleClickRemember
#define	DCR_MAX		400

//----------------------------------------------------------------------------

// local functions

static Object *DoubleClick_GetIconObject(struct internalScaWindowTask *iwt, struct ScaIconNode *in, BOOL *isDefIcon);
static BOOL TestToolStart2(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL ToolStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL OpenDefaultIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in, ULONG Flags);
static BOOL IconFound(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static struct ScaIconNode *SearchForIcon(ULONG IconType, struct ScaWindowStruct **wsFound);
static BOOL TestToolStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL NoToolIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL AppIconStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL DC_ToolStart(struct ScaIconNode *in);
static BOOL OpenIcon(struct internalScaWindowTask *iwt, 
	struct internalScaWindowTask *iwtIcon, struct ScaIconNode *in, ULONG Flags);
static SAVEDS(ULONG) AsyncArexxToolStart(struct ARexxToolStartArg *arg, struct SM_RunProcess *msg);
static BOOL TestIsDrawer(CONST_STRPTR Name);
static BOOL CheckTaskState(struct Task *testTask);

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------


static Object *DoubleClick_GetIconObject(struct internalScaWindowTask *iwt, struct ScaIconNode *in, BOOL *isDefIcon)
{
	T_ExamineData *fib;
	Object *IconObject;
	BPTR fLock = (BPTR)NULL;

	*isDefIcon = FALSE;

	IconObject = NewIconObject(in->in_Name, NULL);
	if (IconObject)
		return IconObject;

	do	{
		if (!ScalosExamineBegin(&fib))
			break;

		fLock = Lock(in->in_Name, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			break;

		if (!ScalosExamineLock(fLock, &fib))
			break;

		IconObject = (Object *) DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
			SCCM_IconWin_GetDefIcon,
			in->in_Name, 
			ScalosExamineGetDirEntryTypeRoot(fib, fLock),
			ScalosExamineGetProtection(fib),
			ICONTYPE_NONE);

		*isDefIcon = TRUE;		
		} while (0);

	if (fLock)
		UnLock(fLock);
	ScalosExamineEnd(&fib);

	return IconObject;
}


BOOL IconDoubleClick(struct internalScaWindowTask *iwt, struct ScaIconNode *in, ULONG Flags)
{
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  in=%08lx <%s>  Flags=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in), Flags));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	if (in)
		{
		// .icongiven:
		struct FileTypeDef *ftd;

		d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name));

		SCA_UnLockWindowList();

		ftd = FindFileType(iwt, in);
		d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

		if (!(Flags & ICONWINOPENF_IgnoreFileTypes) && ftd && ftd->ftd_DefaultAction)
			{
			// perform user-defined default action for file type
			d1(KPrintF("%s/%s/%ld: ftd_Description=<%s>  ftd_DefaultAction=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_Description, ftd->ftd_DefaultAction));
			RunMenuCommandExt(iwt, iwt, ftd->ftd_DefaultAction, in, 0);
			Success = TRUE;
			}
		else
			{
			ULONG IconType;

			d1(kprintf("%s/%s/%ld: ObtainSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

#if 0
			{
				const SCALOSSEMAPHORE *xSema = iwt->iwt_WindowTask.wt_IconSemaphore;

				if (IsListEmpty((struct List *) &xSema->OwnerList))
					{
					d1(KPrintF("%s/%s/%ld: wt_IconSemaphore is unlocked\n", __FILE__, __FUNC__, __LINE__));
					}
				else
					{
					const struct DebugSemaOwner *Owner;

					for (Owner = (const struct DebugSemaOwner *) xSema->OwnerList.lh_Head;
						Owner != (const struct DebugSemaOwner *) &xSema->OwnerList.lh_Tail;
						Owner = (const struct DebugSemaOwner *) Owner->node.ln_Succ)
						{
						CONST_STRPTR OwnModeString;

						if (OWNMODE_EXCLUSIVE == Owner->OwnMode || xSema->Sema.ss_Owner)
							OwnModeString = "Ex";
						else
							OwnModeString = "Sh";

						d1(KPrintF("%s/%s/%ld: wt_IconSemaphore is owned %s by %s/%s/%ld  ProcName=<%s>\n",
							__FILE__, __FUNC__, __LINE__,
							OwnModeString,
							Owner->FileName,
							Owner->Func, Owner->Line,
							Owner->Proc->pr_Task.tc_Node.ln_Name));
						}
					}
			}
#endif

			ScalosLockIconListShared(iwt);

			GetAttr(IDTA_Type, in->in_Icon, &IconType);

			d1(KPrintF("%s/%s/%ld: IconType=%ld\n", __FILE__, __FUNC__, __LINE__, IconType));

			switch ((ULONG) IconType)
				{
			case WBAPPICON:
			case WBTOOL:
			case WBPROJECT:
			case WB_TEXTICON_TOOL:
				Success = IconFound(iwt, in);
				break;

			case WBDISK:
			case WBDRAWER:
			case WB_TEXTICON_DRAWER:
			case WBGARBAGE:
			case WBDEVICE:
				Success = OpenDefaultIcon(iwt, in, Flags);
				break;

			default:
				Success = OpenDefaultIcon(iwt, in, Flags);
				break;
				}

			d1(KPrintF("%s/%s/%ld: ScalosUnLockIconList iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ScalosUnLockIconList(iwt);
			}

		ReleaseFileType(ftd);
		}
	else
		{
		struct ScaWindowStruct *ws;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// !!! SearchForIcon() does ScalosLockIconListExclusive(iwt) !!!
		in = SearchForIcon(WBAPPICON, &ws);
		if (NULL == in)
			in = SearchForIcon(WBTOOL, &ws);
		if (NULL == in)
			in = SearchForIcon(WBPROJECT, &ws);
		if (NULL == in)
			in = SearchForIcon(WB_TEXTICON_TOOL, &ws);

		d1(KPrintF("%s/%s/%ld: in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

		if (in)
			{
			//.iconfound:
			struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;
			struct FileTypeDef *ftd;

			SCA_UnLockWindowList();

			ftd = FindFileType(iwtx, in);
			d1(KPrintF("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

			if (!(Flags & ICONWINOPENF_IgnoreFileTypes) && ftd && ftd->ftd_DefaultAction)
				{
				// perform user-defined default action for file type
				d1(kprintf("%s/%s/%ld: ftd_DefaultAction=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_DefaultAction));
				RunMenuCommandExt(iwt, iwtx, ftd->ftd_DefaultAction, in, 0);
				Success = TRUE;
				}
			else
				{
				Success = IconFound(iwtx, in);
				}

			ReleaseFileType(ftd);

			d1(kprintf("%s/%s/%ld: ReleaseSemaphore iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ScalosUnLockIconList(iwtx);
			}
		else
			{
			struct DoubleClickRemember *dcr, *dcrAlloc;
			ULONG dcrCount = DCR_MAX;

			dcr = dcrAlloc = AllocVec(sizeof(struct DoubleClickRemember) * DCR_MAX, MEMF_PUBLIC);
			if (dcr)
				{
				for (ws=winlist.wl_WindowStruct; dcrCount>1 && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
					{
					struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;

					d1(kprintf("%s/%s/%ld: ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));

					if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
						{
						struct ScaIconNode *in;

						d1(kprintf("%s/%s/%ld: ObtainSemaphore iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
						ScalosLockIconListShared(iwt);

						dcr->dcr_ID = DCR_ID_WindowStruct;
						dcr->dcr_Object.dcro_WindowStruct = ws;
						d1(kprintf("%s/%s/%ld: dcr=%08lx  dcr_ID=%ld\n", __FILE__, __FUNC__, __LINE__, dcr, dcr->dcr_ID));
						dcr++;
						dcrCount--;

						for (in=iwt->iwt_WindowTask.wt_IconList; dcrCount>1 && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
							{
							struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

							d1(kprintf("%s/%s/%ld: in=%08lx  <%s>  Sel=%ld\n", __FILE__, __FUNC__, __LINE__, \
								in, in->in_Name, gg->Flags & GFLG_SELECTED));

							if (gg->Flags & GFLG_SELECTED)
								{
								dcr->dcr_ID = DCR_ID_IconNode;
								dcr->dcr_Object.dcro_IconNode = in;
								d1(kprintf("%s/%s/%ld: dcr=%08lx  dcr_ID=%ld\n", __FILE__, __FUNC__, __LINE__, dcr, dcr->dcr_ID));
								dcr++;
								dcrCount--;
								}
							}
						}

					if (DCR_ID_WindowStruct == dcr[-1].dcr_ID)
						{
						d1(kprintf("%s/%s/%ld: dcr=%08lx  dcr_ID=%ld\n", __FILE__, __FUNC__, __LINE__, dcr, dcr->dcr_ID));
						dcr--;
						dcrCount++;

						d1(kprintf("%s/%s/%ld: ReleaseSemaphore iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
						ScalosUnLockIconList(iwt);
						}
					}

				dcr->dcr_ID = DCR_ID_End;
				d1(kprintf("%s/%s/%ld: dcr=%08lx  dcr_ID=%ld\n", __FILE__, __FUNC__, __LINE__, dcr, dcr->dcr_ID));

				d1(kprintf("%s/%s/%ld: dcrCount=%ld\n", __FILE__, __FUNC__, __LINE__, dcrCount));

				SCA_UnLockWindowList();

				for (dcr=dcrAlloc; DCR_ID_WindowStruct == dcr->dcr_ID; )
					{
					BPTR oldDir;
					struct ScaWindowStruct *wsDcr = dcr->dcr_Object.dcro_WindowStruct;
					struct internalScaWindowTask *iwtDcr = (struct internalScaWindowTask *) wsDcr->ws_WindowTask;

					oldDir = CurrentDir(wsDcr->ws_Lock);

					d1(kprintf("%s/%s/%ld: iwtDcr=%08lx  dcr=%08lx  dcr_ID=%ld\n", __FILE__, __FUNC__, __LINE__, iwtDcr, dcr, dcr->dcr_ID));

					++dcr;		// skip DCR_ID_WindowStruct

					while (DCR_ID_IconNode == dcr->dcr_ID)
						{
						struct ScaIconNode *in = dcr->dcr_Object.dcro_IconNode;
						struct FileTypeDef *ftd;

						d1(kprintf("%s/%s/%ld: dcr=%08lx  dcr_ID=%ld\n", __FILE__, __FUNC__, __LINE__, dcr, dcr->dcr_ID));
						d1(kprintf("%s/%s/%ld: in=%08lx  <%s>  Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name, in->in_Icon));

						ftd = FindFileType(iwtDcr, in);
						d1(kprintf("%s/%s/%ld: ftd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd));

						if (!(Flags & ICONWINOPENF_IgnoreFileTypes) && ftd && ftd->ftd_DefaultAction)
							{
							// perform user-defined default action for file type
							d1(kprintf("%s/%s/%ld: ftd_DefaultAction=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd->ftd_DefaultAction));
							RunMenuCommandExt(iwt, iwtDcr, ftd->ftd_DefaultAction, in, 0);
							Success = TRUE;
							}
						else
							{
							Success |= OpenIcon(iwt, iwtDcr, in, Flags);
							}

						ReleaseFileType(ftd);

						dcr++;
						}

					d1(kprintf("%s/%s/%ld: dcr=%08lx  dcr_ID=%ld\n", __FILE__, __FUNC__, __LINE__, dcr, dcr->dcr_ID));

					d1(kprintf("%s/%s/%ld: ReleaseSemaphore iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtDcr));
					ScalosUnLockIconList(iwtDcr);

					CurrentDir(oldDir);
					}

				FreeVec(dcrAlloc);
				}
			else
				SCA_UnLockWindowList();
			}
		}

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


static BOOL OpenDefaultIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in, ULONG Flags)
{
	BOOL Success;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, in->in_Name));

	Success = OpenIcon(iwt, iwt, in, Flags);

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


static BOOL IconFound(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	ULONG IconType;
	BOOL Success = FALSE;
	BPTR oldDir;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, in->in_Name));

	oldDir = CurrentDir(ws->ws_Lock);
	if (in->in_Lock)
		CurrentDir(in->in_Lock);

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	d1(KPrintF("%s/%s/%ld: IconType=%08lx\n", __FILE__, __FUNC__, __LINE__, IconType));
	switch ((ULONG) IconType)
		{
	case WBTOOL:
		Success = TestToolStart2(iwt, in);
		break;
	case WBPROJECT:
		Success = ToolStart(iwt, in);
		break;
	case WBAPPICON:
		Success = AppIconStart(iwt, in);
		break;
	case WB_TEXTICON_TOOL:
		Success = TestToolStart(iwt, in);
		break;

	case WBDISK:
	case WBDRAWER:
	case WB_TEXTICON_DRAWER:
	case WBGARBAGE:
	case WBDEVICE:
		break;

	default:
		break;
		}

	CurrentDir(oldDir);

	return Success;
}


static BOOL TestToolStart2(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BOOL Success;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, in->in_Name));

	if (in->in_Flags & INF_DefaultIcon)
		Success = NoToolIcon(iwt, in);
	else
		Success = ToolStart(iwt, in);

	return Success;
}


static BOOL ToolStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BOOL Success = FALSE;
	STRPTR ttCli = NULL, ttRexx = NULL, ttDoNotPrompt = NULL;
	BPTR dirLock;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, in->in_Name));

	DoMethod(in->in_Icon, IDTM_FindToolType, "CLI", &ttCli);
	DoMethod(in->in_Icon, IDTM_FindToolType, "REXX", &ttRexx);
	DoMethod(in->in_Icon, IDTM_FindToolType, "DONOTPROMPT", &ttDoNotPrompt);

	d1(KPrintF("%s/%s/%ld: Cli=%08lx  Rexx=%08lx  DoNotPrompt=%08lx\n", __FILE__, __FUNC__, __LINE__, ttCli, ttRexx, ttDoNotPrompt));

	if (in->in_Lock)
		dirLock = in->in_Lock;
	else
		dirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

	if (ttCli)
		{
		d1(KPrintF("%s/%s/%ld: ttCli=<%s>\n", __FILE__, __FUNC__, __LINE__, ttCli));

		if (ttDoNotPrompt)
			{
			d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name));
			Success = CLIStart(dirLock, in->in_Name, in->in_Icon, CurrentPrefs.pref_DefaultStackSize);
			}
		else
			{
			STRPTR CliValue;

			if (strlen(ttCli) > 4)
				CliValue = ttCli + 4;
			else
				CliValue = NULL;

			if (NULL == CliValue			// "CLI"
				|| 'y' == ToLower(*CliValue)	// "CLI=Y.."
				|| '1' == ToLower(*CliValue))	// "CLI=1"
				{
				Success = DC_ToolStart(in);
				}
			}
		}
	else if (ttRexx)
		{
		d1(KPrintF("%s/%s/%ld: ttRexx=<%s>\n", __FILE__, __FUNC__, __LINE__, ttRexx));
		if (ttDoNotPrompt)
			{
			Success = ArexxToolStart(iwt, dirLock, in->in_Name);
			}
		else
			{
			Success = DC_ToolStart(in);
			}
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		Success = DC_ToolStart(in);
		}

	return Success;
}


// Search all windows for selected icons of type <IconType>
static struct ScaIconNode *SearchForIcon(ULONG IconType, struct ScaWindowStruct **wsFound)
{
	struct ScaWindowStruct *ws;
	struct ScaIconNode *inFound = NULL;

	d1(kprintf("%s/%s/%ld: IconType=%ld\n", __FILE__, __FUNC__, __LINE__, IconType));

	*wsFound = NULL;

	for (ws=winlist.wl_WindowStruct; (NULL == inFound) && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
			{
			struct ScaIconNode *in;
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;

			d1(kprintf("%s/%s/%ld: ObtainSemaphore iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			ScalosLockIconListShared(iwt);

			for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

				if (gg->Flags & GFLG_SELECTED)
					{
					ULONG tstIconType;

					GetAttr(IDTA_Type, in->in_Icon, (ULONG *) &tstIconType);
					if (tstIconType == IconType)
						{
						*wsFound = ws;
						inFound = in;
						break;
						}
					}
				}

			if (NULL == inFound)
				{
				d1(kprintf("%s/%s/%ld: ReleaseSemaphore iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
				ScalosUnLockIconList(iwt);
				}
			}
		}

	d1(kprintf("%s/%s/%ld: inFound=%08lx\n", __FILE__, __FUNC__, __LINE__, inFound));

	return inFound;
}


static BOOL TestToolStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	Object *IconObj;
	BOOL isDefIcon = FALSE;
	BOOL Success;

	d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name ? in->in_Name : (STRPTR) ""));

	IconObj = DoubleClick_GetIconObject(iwt, in, &isDefIcon);

	d1(kprintf("%s/%s/%ld: IconObj=%08lx  isDefIcon=%ld\n", __FILE__, __FUNC__, __LINE__, IconObj, isDefIcon));
	if (IconObj)
		{
		Object *origIconObj = in->in_Icon;
		ULONG IconType;

		in->in_Icon = IconObj;

		GetAttr(IDTA_Type, in->in_Icon, (ULONG *) &IconType);

		switch ((ULONG) IconType)
			{
		case WBTOOL:
		case WB_TEXTICON_TOOL:
			if (!isDefIcon)
				{
				Success = DC_ToolStart(in);

				DisposeIconObject(in->in_Icon);
				in->in_Icon = origIconObj;

				return Success;
				}
			break;

		case WBPROJECT:
			Success = ToolStart(iwt, in);

			DisposeIconObject(in->in_Icon);
			in->in_Icon = origIconObj;

			return Success;
			break;

		default:
			break;
			}

		in->in_Icon = origIconObj;
		DisposeIconObject(IconObj);
		}

	Success = NoToolIcon(iwt, in);

	return Success;
}


static BOOL NoToolIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
		SCCM_IconWin_MenuCommand,
		"executecommand",
		in,
		0);

	return TRUE;
}


static BOOL AppIconStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BOOL Success = FALSE;
	struct AppObject *appo;
	
	d1(kprintf("%s/%s/%ld: iwt=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, in));

	if (NULL == iwt->iwt_AppListSemaphore)
		return Success;

	ScalosObtainSemaphoreShared(iwt->iwt_AppListSemaphore);

	for (appo=iwt->iwt_AppList; appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
		{
		if (APPTYPE_AppIcon == appo->appo_type
			&& in->in_Icon == appo->appo_object.appoo_IconObject)
			{
			d1(kprintf("%s/%s/%ld: mp_SigTask=%08lx\n", __FILE__, __FUNC__, __LINE__, appo->appo_msgport->mp_SigTask));
			if (!CheckTaskState(appo->appo_msgport->mp_SigTask))
				{
				ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);
				SCA_RemoveAppObject(appo);
				return FALSE;
				}

			SendAppMessage(appo, AMCLASSICON_Open, iwt->iwt_WindowTask.wt_Window->MouseX, iwt->iwt_WindowTask.wt_Window->MouseY);
			Success = TRUE;
			break;
			}
		}

	ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);

	return Success;
}


static BOOL DC_ToolStart(struct ScaIconNode *in)
{
	struct WBArg *args;
	ULONG ArgCount;
	ULONG NumberOfWbArgs;
	BOOL Success;

	d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name ? in->in_Name : (STRPTR) ""));

	if (NULL == in->in_Name)
		return FALSE;

	NumberOfWbArgs = 1 + SCA_CountWBArgs(in);

	args = AllocVec(NumberOfWbArgs * sizeof(struct WBArg), MEMF_PUBLIC | MEMF_CLEAR);
	if (NULL == args)
		return FALSE;

	args[0].wa_Lock = Lock("", ACCESS_READ);
	args[0].wa_Name = in->in_Name;

	ArgCount = 1 + SCA_MakeWBArgs(&args[1], in, NumberOfWbArgs);

	debugLock_d1(args[0].wa_Lock);
	d1(kprintf("%s/%s/%ld: arg[0].wa_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, args[0].wa_Name));
	d1(kprintf("%s/%s/%ld: ArgCount=%ld\n", __FILE__, __FUNC__, __LINE__, ArgCount));

	// SCA_WBStart()
	Success = SCA_WBStartTags(args, ArgCount,
		SCA_IconObject, (ULONG) in->in_Icon,
		TAG_END);

	SCA_FreeWBArgs(&args[1], ArgCount - 1, 
		Success ? (SCAF_FreeNames) : (SCAF_FreeNames | SCAF_FreeLocks));

	if (!Success)
		UnLock(args[0].wa_Lock);

	FreeVec(args);

	return Success;
}


static BOOL OpenIcon(struct internalScaWindowTask *iwt, 
	struct internalScaWindowTask *iwtIcon, struct ScaIconNode *in, ULONG Flags)
{
	BPTR oldDir;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: BEGIN  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, Flags));

	do	{
		struct IBox *WindowRect;
		BOOL OpenNewWindow;
		struct Process *myProcess = (struct Process *) FindTask(NULL);
		struct MsgPort *ReplyPort;
		ULONG noActivate;
		ULONG isDdPopup;
		ULONG BrowserMode;

		noActivate = (Flags & ICONWINOPENF_DoNotActivateWindow) ? 1 : 0;
		isDdPopup = (Flags & ICONWINOPENF_DdPopupWindow) ? 1 : 0;
		BrowserMode = (Flags & ICONWINOPENF_BrowserWindow) ? 1 : 0;

		OpenNewWindow = (Flags & ICONWINOPENF_NewWindow)
			|| !(iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_BrowserMode);

		if (myProcess == iwt->iwt_WindowProcess)
			ReplyPort = iwt->iwt_WindowTask.wt_IconPort;
		else
			ReplyPort = NULL;

		d1(KPrintF("%s/%s/%ld: myProcess=%08lx  WindowProcess=%08lx  ReplyPort=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, myProcess, iwt->iwt_WindowProcess, ReplyPort));

		if (in->in_Lock)
			oldDir = CurrentDir(in->in_Lock);
		else
			oldDir = CurrentDir(iwtIcon->iwt_WindowTask.mt_WindowStruct->ws_Lock);

		if (!TestIsDrawer(GetIconName(in)))
			break;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (in->in_Flags & INF_TextIcon)
			{
			ULONG ShowFlags, ViewBy;
			Object *IconObj = NewIconObject(in->in_Name,
						TAG_END);

			ShowFlags = iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll;
			ViewBy = iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes;

			if (IconObj)
				{
				GetAttr(IDTA_Flags, IconObj, &ShowFlags);
				GetAttr(IDTA_ViewModes, IconObj, &ViewBy);

				ViewBy = TranslateViewModesFromIcon(ViewBy);

				d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

				DisposeIconObject(IconObj);
				}

			d1(KPrintF("%s/%s/%ld: Open text icon  ShowFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, ShowFlags));

			if (OpenNewWindow)
				{
				Success = SCA_OpenIconWindowTags(SCA_Path, (ULONG) in->in_Name,
					SCA_Flags, SCAF_OpenWindow_ScalosPort,
					SCA_MessagePort, (ULONG) ReplyPort,
					SCA_ShowAllMode, ShowFlags & DDFLAGS_SHOWMASK,
					SCA_ViewModes, ViewBy,
					SCA_NoActivateWindow, noActivate,
					SCA_DdPopupWindow, isDdPopup,
					SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
					SCA_BrowserMode, BrowserMode,
					TAG_END);
				}
			else
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_NewPath,
					GetIconName(in),
					SCA_ShowAllMode, ShowFlags & DDFLAGS_SHOWMASK,
					SCA_ViewModes, ViewBy,
					SCA_DdPopupWindow, isDdPopup,
					SCA_NoActivateWindow, noActivate,
					SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
					SCA_BrowserMode, BrowserMode,
					TAG_END);
				Success = TRUE;
				}

			d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
			}
		else
			{
			GetAttr(IDTA_WindowRect, in->in_Icon, (APTR) &WindowRect);

			d1(KPrintF("%s/%s/%ld: WindowRect=%08lx\n", __FILE__, __FUNC__, __LINE__, WindowRect));
			if (NULL == WindowRect)
				WindowRect = &CurrentPrefs.pref_DefWindowBox;

			if (in->in_Flags & INF_DefaultIcon)
				{
				d1(KPrintF("%s/%s/%ld: Open default icon\n", __FILE__, __FUNC__, __LINE__));

				if (OpenNewWindow)
					{
					Success = SCA_OpenIconWindowTags(SCA_IconNode, (ULONG) in,
						SCA_Flags, SCAF_OpenWindow_ScalosPort,
						SCA_MessagePort, (ULONG) ReplyPort,
						SCA_ShowAllMode, iwt->iwt_OldShowType,
						SCA_DdPopupWindow, isDdPopup,
						SCA_NoActivateWindow, noActivate,
						SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
						SCA_BrowserMode, BrowserMode,
						TAG_END);
					}
				else
					{
					DoMethod(iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_NewPath,
						GetIconName(in),
						SCA_ShowAllMode, iwt->iwt_OldShowType,
						SCA_DdPopupWindow, isDdPopup,
						SCA_NoActivateWindow, noActivate,
						SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
						SCA_BrowserMode, BrowserMode,
						TAG_END);
					Success = TRUE;
					}

				d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
				}
			else
				{
				d1(KPrintF("%s/%s/%ld: Open non-default icon\n", __FILE__, __FUNC__, __LINE__));

				if (OpenNewWindow)
					{
					Success = SCA_OpenIconWindowTags(SCA_IconNode, (ULONG) in,
						SCA_Flags, SCAF_OpenWindow_ScalosPort,
						SCA_DdPopupWindow, isDdPopup,
						SCA_NoActivateWindow, noActivate,
						SCA_MessagePort, (ULONG) ReplyPort,
						SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
						SCA_BrowserMode, BrowserMode,
						TAG_END);
					}
				else
					{
					WindowNewPath(iwt, GetIconName(in));
					Success = TRUE;
					}

				d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
				}
			}
		} while (0);

	CurrentDir(oldDir);

	d1(kprintf("%s/%s/%ld: Begin\n", __FILE__, __FUNC__, __LINE__));

	if (!Success)
		DisplayScreenTitleError(iwt, MSGID_CANNOT_OPEN_DRAWER);

	d1(kprintf("%s/%s/%ld: Finish\n", __FILE__, __FUNC__, __LINE__));

	return Success;
}


BOOL ArexxToolStart(struct internalScaWindowTask *iwt, BPTR dirLock, CONST_STRPTR ProgName)
{
	struct ARexxToolStartArg Args;
	BOOL Success;

	d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	Args.asa_DirLock = DupLock(dirLock);
	Args.asa_ProgName = AllocCopyString(ProgName);

	if (iwt && iwt != &MainWindowTask->mwt)
		{
		Success = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_RunProcess, AsyncArexxToolStart,
			&Args, sizeof(Args), SCCV_RunProcess_NoReply);
		}
	else
		{
		Success = RunProcess(&iwt->iwt_WindowTask, 
			(RUNPROCFUNC) AsyncArexxToolStart,
			sizeof(Args) / sizeof(ULONG),
			(struct WBArg *)(APTR) &Args,
			iInfos.xii_iinfos.ii_MainMsgPort);
		}

	return Success;
}



static SAVEDS(ULONG) AsyncArexxToolStart(struct ARexxToolStartArg *arg, struct SM_RunProcess *msg)
{
	struct MsgPort *RexxPort;
	struct RexxMsg *RxMsg = NULL;
	BOOL RxMsgFilled = FALSE;
	struct MsgPort *ReplyPort;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  in=%08lx\n", __FILE__, __FUNC__, __LINE__, arg->asa_ProgName));
	debugLock_d1(arg->asa_DirLock);

	do	{
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

		RxMsg->rm_Args[0] = arg->asa_ProgName;

		RxMsg->rm_Action = RXCOMM;

		RxMsg->rm_Stdin = Open("NIL:", MODE_OLDFILE);
		d1(kprintf("%s/%s/%ld: stdin=%08lx\n", __FILE__, __FUNC__, __LINE__, RxMsg->rm_Stdin));
		if ((BPTR)NULL == RxMsg->rm_Stdin)
			break;

		RxMsg->rm_Stdout = Open((STRPTR) CurrentPrefs.pref_ConsoleName, MODE_NEWFILE);
		d1(kprintf("%s/%s/%ld: stdout=%08lx\n", __FILE__, __FUNC__, __LINE__, RxMsg->rm_Stdout));
		if ((BPTR)NULL == RxMsg->rm_Stdout)
			break;

		if (!FillRexxMsg(RxMsg, 1, 0))
			break;

		d1(kprintf("%s/%s/%ld: FillRexxMsg OK\n", __FILE__, __FUNC__, __LINE__));

		RxMsgFilled = TRUE;

		PutMsg(RexxPort, (struct Message *) RxMsg);

		WaitPort(ReplyPort);
		GetMsg(ReplyPort);
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
			ClearRexxMsg(RxMsg, 16);

		DeleteRexxMsg(RxMsg);
		}
	if (ReplyPort)
		DeleteMsgPort(ReplyPort);

	FreeCopyString(arg->asa_ProgName);
	UnLock(arg->asa_DirLock);

	return 0;
}


static BOOL TestIsDrawer(CONST_STRPTR Name)
{
	T_ExamineData *fib;
	BPTR fLock = BNULL;
	BOOL isDrawer = TRUE;

	d1(KPrintF("%s/%s/%ld: BEGIN  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

	do	{
		if (!ScalosExamineBegin(&fib))
			break;

		fLock = Lock((STRPTR) Name, ACCESS_READ);
		d1(KPrintF("%s/%s/%ld: fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, fLock));
		if (BNULL == fLock)
			break;

		debugLock_d1(fLock);

		if (!ScalosExamineLock(fLock, &fib))
			break;

		d1(KPrintF("%s/%s/%ld: fib_DirEntryType=%ld\n", __FILE__, __FUNC__, __LINE__, fib->fib_DirEntryType));

		isDrawer = ScalosExamineIsDrawer(fib);
		} while (0);

	if (fLock)
		UnLock(fLock);
	ScalosExamineEnd(&fib);

	d1(KPrintF("%s/%s/%ld: END  isDrawer=%ld\n", __FILE__, __FUNC__, __LINE__, isDrawer));

	return isDrawer;
}


// Test if task <testTask> is present and has a valid state
static BOOL CheckTaskState(struct Task *testTask)
{
	BOOL TaskOK = TRUE;

	if (testTask)
		{
		// check whether the owner is still alive
		extern struct ExecBase *SysBase;
		struct Task *theTask;
		BOOL TaskFound = FALSE;

		d1(kprintf("%s/%s/%ld: checking if task is still alife\n", __FILE__, __FUNC__, __LINE__));

		Disable();
		for (theTask = (struct Task *) SysBase->TaskReady.lh_Head;
			theTask != (struct Task *) &SysBase->TaskReady.lh_Tail;
			theTask = (struct Task *) theTask->tc_Node.ln_Succ )
			{
			if (theTask == testTask)
				{
				TaskFound = TRUE;
				break;
				}
			}
		for (theTask = (struct Task *) SysBase->TaskWait.lh_Head;
			!TaskFound && (theTask != (struct Task *) &SysBase->TaskWait.lh_Tail);
			theTask = (struct Task *) theTask->tc_Node.ln_Succ )
			{
			if (theTask == testTask)
				{
				if ((0 == theTask->tc_SigWait)
					|| (TS_INVALID == theTask->tc_State)
					|| (TS_REMOVED == theTask->tc_State) )
					{
					TaskFound = FALSE;
					}
				else
					{
					TaskFound = TRUE;
					}
				break;
				}
			}
		Enable();

		d1(kprintf("%s/%s/%ld: TaskFound=%ld\n", __FILE__, __FUNC__, __LINE__, TaskFound));

		if (!TaskFound)
			{
			TaskOK = FALSE;
			}
		}

	return TaskOK;
}

