// PrefsPlugins.c
// $Date$
// $Revision$


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include <workbench/startup.h>
#include <workbench/icon.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <scalos/scalos.h>
#include <scalos/scalosprefsplugin.h>
#include <mui/BetterString_mcc.h>
#include <mui/Popplaceholder_mcc.h>
#include <mui/Urltext_mcc.h>
#include <mui/NFloattext_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/HotkeyString_mcc.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <proto/scalos.h>
#include <proto/locale.h>
#include <proto/scalosprefsplugin.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>
 
#include <clib/alib_protos.h>

#include <defs.h>

#include "General.h"
#include "ScalosPrefs.h"
#include "PrefsPlugins.h"

//-----------------------------------------------------------------

struct PrefsPlugin
{
	struct Node ppl_Node;

	struct Library *ppl_PluginBase;
#ifdef __amigaos4__
	struct Interface *ppl_IPlugin;
#endif

	struct MUI_CustomClass *ppl_PluginClass;
	STRPTR ppl_TitleString;

	ULONG ppl_PageNumber;		// Page ordinal number

	ULONG ppl_TitleImageIndex;
	Object *ppl_TitleImage;
	Object *ppl_GroupObject;	// is disposed by us, since it is added as child...

	Object **ppl_SubWindows;	// NULL terminated array of SubWindow objects
};

//----------------------------------------------------------------------------

static struct List PrefsPluginList;
static ULONG PrefsPluginCount;
struct TagItem *SubWindowTagList;

//----------------------------------------------------------------------------

static struct MUI_CustomClass *OpenPrefsModule(struct SCAModule *app, CONST_STRPTR FileName);
static void DisposePrefsPlugin(struct PrefsPlugin *ppl);
static Object *CreatePrefsTitleImage(void *UserData);

//----------------------------------------------------------------------------


BOOL PrefsPluginsInit(void)
{
	NewList(&PrefsPluginList);

	return TRUE;
}

void PrefsPluginsCleanup(void)
{
	struct PrefsPlugin *ppl;

	while ((ppl = (struct PrefsPlugin *) RemTail(&PrefsPluginList)))
		{
		DisposePrefsPlugin(ppl);
		}

	if (SubWindowTagList)
		{
		free(SubWindowTagList);
		SubWindowTagList = NULL;
		}
}


ULONG DoMethodForAllPrefsPlugins(ULONG ArgsCount, ULONG MethodID, ...)
{
	va_list ap;
	ULONG result = 0;
	ULONG *ArgArray;

	ArgArray = calloc(sizeof(ULONG), ArgsCount);
	if (ArgArray)
		{
		struct PrefsPlugin *ppl;
		ULONG n;

		va_start(ap, MethodID);

		ArgArray[0] = MethodID;
		for (n = 1; n < ArgsCount; n++)
			{
			ArgArray[n] = va_arg(ap, ULONG);
			}

		for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
			ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
			ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
			{
			DoMethodA(ppl->ppl_GroupObject, (Msg) ArgArray);
			}

		free(ArgArray);
		}

	va_end(ap);

	return result;
}

void SetAttributeForAllPrefsPlugins(ULONG Attribute, ULONG Value)
{
	struct PrefsPlugin *ppl;

	for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
		ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
		ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
		{
		set(ppl->ppl_GroupObject, Attribute, Value);
		}
}


void NotifyPrefsPluginsPageChange(ULONG ActivePage)
{
	struct PrefsPlugin *ppl;

	for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
		ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
		ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
		{
		DoMethod(ppl->ppl_GroupObject, MUIM_ScalosPrefs_PageActive, ActivePage == ppl->ppl_PageNumber);
		}
}


BOOL CheckMCCsForAllPrefsPlugins(void)
{
	struct PrefsPlugin *ppl;

	for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
		ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
		ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
		{
		const struct MUIP_ScalosPrefs_MCCList *RequiredMccList = NULL;

		RequiredMccList	= (const struct MUIP_ScalosPrefs_MCCList *) DoMethod(ppl->ppl_GroupObject, MUIM_ScalosPrefs_GetListOfMCCs);

		while (RequiredMccList && RequiredMccList->MccName)
			{
			if (!CheckMCCVersion(RequiredMccList->MccName,
				RequiredMccList->MccMinVersion,
				RequiredMccList->MccMinRevision))
				return FALSE;

			RequiredMccList++;
			}
		}

	return TRUE;
}


void CollectPrefsPlugins(struct SCAModule *app, CONST_STRPTR Path, CONST_STRPTR Pattern)
{
	STRPTR PatternBuffer;
	struct FileInfoBlock *fib = NULL;
	BPTR DirLock = (BPTR)NULL;

	do	{
		size_t Length;

		Length = 1 + 2 + 2 * strlen(Pattern);
		PatternBuffer = malloc(Length);
		d1(kprintf(__FUNC__ "/%ld:  PatternBuffer=%08lx\n", __LINE__, PatternBuffer));
		if (NULL == PatternBuffer)
			break;

		if (ParsePatternNoCase(Pattern, PatternBuffer, Length) < 0)
			break;

		d1(kprintf(__FUNC__ "/%ld:  ParsePatternNoCase success  Pattern=<%s>  PatternBuffer=<%s>\n", \
			__LINE__, Pattern, PatternBuffer));

		fib = AllocDosObject(DOS_FIB, NULL);
		if (NULL == fib)
			break;

		DirLock = Lock(Path, ACCESS_READ);
		if ((BPTR)NULL== DirLock)
			break;

		if (!Examine(DirLock, fib))
			break;

		while (ExNext(DirLock, fib))
			{
			d1(kprintf(__FUNC__ "/%ld:  filename=<%s>\n", __LINE__, fib->fib_FileName));

			if (MatchPatternNoCase(PatternBuffer, fib->fib_FileName))
				{
				d1(kprintf(__FUNC__ "/%ld:  add prefs plugin <%s>\n", __LINE__, fib->fib_FileName));
				OpenPrefsModule(app, fib->fib_FileName);
				}
			}
		} while (0);

	d1(kprintf(__FUNC__ "/%ld:  finished\n", __LINE__));

	if (DirLock)
		UnLock(DirLock);
	if (fib)
		FreeDosObject(DOS_FIB, fib);
	if (PatternBuffer)
		free(PatternBuffer);
}


void CreatePrefsPlugins(struct SCAModule *app)
{
	struct PrefsPlugin *ppl;

	for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
		ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
		ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
		{
		d1(kprintf(__FUNC__ "/%ld:  fCreateIcons=%ld  ProgramName=<%s>\n", __LINE__, fCreateIcons, ProgramName));

		ppl->ppl_GroupObject = NewObject(ppl->ppl_PluginClass->mcc_Class, 0,
			TAG_END);

		if (ppl->ppl_GroupObject)
			{
			PrefsPluginCount++;

			ppl->ppl_SubWindows = (Object **) DoMethod(ppl->ppl_GroupObject, MUIM_ScalosPrefs_CreateSubWindows);
			}

		d1(kprintf(__FUNC__ "/%ld:  obj=%08lx\n", __LINE__, ppl->ppl_GroupObject));
		}
}


void InsertPrefsPlugins(struct SCAModule *app, Object *pageGroup, Object *pageList)
{
	if (DoMethod(pageGroup, MUIM_Group_InitChange))
		{
		struct PrefsPlugin *ppl;

		for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
			ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
			ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
			{
			d1(kprintf(__FUNC__ "/%ld:  fCreateIcons=%ld  ProgramName=<%s>\n", __LINE__, fCreateIcons, ProgramName));

			d1(kprintf(__FUNC__ "/%ld:  obj=%08lx\n", __LINE__, ppl->ppl_GroupObject));

			ppl->ppl_PageNumber = 0;

			if (ppl->ppl_GroupObject)
				{
				struct NewPageListEntry nple;
				Object *NewPage;

				set(ppl->ppl_GroupObject, MUIA_ScalosPrefs_MainWindow, (ULONG) app->Obj[WINDOW_MAIN]);
				set(ppl->ppl_GroupObject, MUIA_ScalosPrefs_Application, (ULONG) app->Obj[APPLICATION]);
				set(ppl->ppl_GroupObject, MUIA_ScalosPrefs_CreateIcons, fCreateIcons);
				set(ppl->ppl_GroupObject, MUIA_ScalosPrefs_ProgramName, (ULONG) ProgramName);
//				set(ppl->ppl_GroupObject, MUIA_Background, MUII_PageBack);

				get(pageList, MUIA_NList_Entries, &ppl->ppl_PageNumber);

				nple.nple_TitleString = ppl->ppl_TitleString;
				nple.nple_TitleImage = ppl->ppl_TitleImage;
				nple.nple_ImageIndex = ppl->ppl_TitleImageIndex;
				nple.nple_CreateTitleImage = CreatePrefsTitleImage;
				nple.nple_UserData = ppl;

				NewPage = CreatePrefsPage(app, ppl->ppl_GroupObject, &nple);
				if (NewPage)
					DoMethod(pageGroup, OM_ADDMEMBER, NewPage);
				}
			}
		DoMethod(pageGroup, MUIM_Group_ExitChange);
		}
}


struct TagItem *GetPrefsPluginSubWindows(struct SCAModule *app)
{
	ULONG SubWindowCount = 0;
	struct PrefsPlugin *ppl;
	struct TagItem *ti;

	if (SubWindowTagList)
		{
		free(SubWindowTagList);
		SubWindowTagList = NULL;
		}

	for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
		ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
		ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
		{
		ULONG n;

		for (n=0; ppl->ppl_SubWindows && ppl->ppl_SubWindows[n]; n++)
			SubWindowCount++;
		}

	SubWindowTagList = calloc(1 + SubWindowCount, sizeof(struct TagItem));

	if (NULL == SubWindowTagList)
		return NULL;

	ti = SubWindowTagList;

	for (ppl = (struct PrefsPlugin *) PrefsPluginList.lh_Head;
		ppl != (struct PrefsPlugin *) &PrefsPluginList.lh_Tail;
		ppl = (struct PrefsPlugin *) ppl->ppl_Node.ln_Succ)
		{
		ULONG n;

		for (n=0; ppl->ppl_SubWindows && ppl->ppl_SubWindows[n]; n++)
			{
			ti->ti_Tag = MUIA_Application_Window;
			ti->ti_Data = (ULONG) ppl->ppl_SubWindows[n];
			ti++;
			}
		}

	ti->ti_Tag = TAG_END;

	return SubWindowTagList;
}

//----------------------------------------------------------------------------

static struct MUI_CustomClass *OpenPrefsModule(struct SCAModule *app, CONST_STRPTR FileName)
{
	struct PrefsPlugin *ppl;
	struct Library *ScalosPrefsPluginBase;
#ifdef __amigaos4__
	struct ScalosPrefsPluginIFace *IScalosPrefsPlugin;
#endif
	struct MUI_CustomClass *pclass = NULL;

	d1(kprintf(__FUNC__ "/%ld:  FileName=<%s>\n", __LINE__, FileName));

	do	{
		char TitleBuffer[256];
		STRPTR Title;

		ppl = calloc(1, sizeof(struct PrefsPlugin));
		d1(kprintf(__FUNC__ "/%ld:  ppl=%08lx\n", __LINE__, ppl));
		if (NULL == ppl)
			break;

		ScalosPrefsPluginBase = ppl->ppl_PluginBase = OpenLibrary(FileName, 0);
		d1(kprintf(__FUNC__ "/%ld:  ScalosPrefsPluginBase=%08lx\n", __LINE__, ScalosPrefsPluginBase));
		if (NULL == ScalosPrefsPluginBase)
			break;
#ifdef __amigaos4__
		IScalosPrefsPlugin = (struct ScalosPrefsPluginIFace *)(ppl->ppl_IPlugin =
			GetInterface(ScalosPrefsPluginBase, "main", 1, NULL));
#endif

		ppl->ppl_PluginClass = pclass = (struct MUI_CustomClass *) SCAGetPrefsInfo(SCAPREFSINFO_GetClass);
		d1(kprintf(__FUNC__ "/%ld:  PluginClass=%08lx\n", __LINE__, ppl->ppl_PluginClass));
		if (NULL == ppl->ppl_PluginClass)
			break;

		Title = (STRPTR) SCAGetPrefsInfo(SCAPREFSINFO_GetTitle);
		d1(kprintf(__FUNC__ "/%ld:  Title=<%s>\n", __LINE__, Title));
		if (NULL == Title)
			break;

		ppl->ppl_TitleImageIndex = ModuleImageIndex++;
		sprintf(TitleBuffer, Title, ppl->ppl_TitleImageIndex);

		ppl->ppl_TitleString = strdup(TitleBuffer);
		if (NULL == ppl->ppl_TitleString)
			break;

		ppl->ppl_TitleImage = (Object *) SCAGetPrefsInfo(SCAPREFSINFO_GetTitleImage);
		d1(kprintf(__FUNC__ "/%ld:  ppl_TitleImage=%08lx\n", __LINE__, ppl->ppl_TitleImage));

		// ppl_TitleImage may be NULL

		AddTail(&PrefsPluginList, &ppl->ppl_Node);
		ppl = NULL;
		} while (0);

	if (ppl)
		DisposePrefsPlugin(ppl);

	return pclass;
}

static void DisposePrefsPlugin(struct PrefsPlugin *ppl)
{
	if (ppl)
		{
		if (ppl->ppl_TitleImage)
			{
			MUI_DisposeObject(ppl->ppl_TitleImage);
			ppl->ppl_TitleImage = NULL;
			}
		if (ppl->ppl_TitleString)
			{
			free(ppl->ppl_TitleString);
			ppl->ppl_TitleString = NULL;
			}
#ifdef __amigaos4__
		if (ppl->ppl_IPlugin && NULL == ppl->ppl_GroupObject)
			{
			DropInterface(ppl->ppl_IPlugin);
			ppl->ppl_IPlugin = NULL;
			}
#endif
		if (ppl->ppl_PluginBase)
			{
			d1(kprintf(__FUNC__ "/%ld:  Plugin=<%s>  OpenCount=%ld\n", \
				__LINE__, ppl->ppl_PluginBase->lib_Node.ln_Name, ppl->ppl_PluginBase->lib_OpenCnt));

			if (NULL == ppl->ppl_GroupObject)
				CloseLibrary(ppl->ppl_PluginBase);

//+++			RemLibrary(ppl->ppl_PluginBase);
			ppl->ppl_PluginBase = NULL;
			}

		free(ppl);
		}
}

//----------------------------------------------------------------------------

static Object *CreatePrefsTitleImage(void *UserData)
{
	struct PrefsPlugin *ppl = (struct PrefsPlugin *) UserData;
	Object *TitleImage = NULL;

	do	{
		struct Library *ScalosPrefsPluginBase;
#ifdef __amigaos4__
		struct ScalosPrefsPluginIFace *IScalosPrefsPlugin;
#endif

		d1(KPrintF(__FUNC__ "/%ld:  ppl=%08lx\n", __LINE__, ppl));
		if (NULL == ppl)
			break;

		ScalosPrefsPluginBase = ppl->ppl_PluginBase;
#ifdef __amigaos4__
		IScalosPrefsPlugin = (struct ScalosPrefsPluginIFace *)GetInterface(
			ScalosPrefsPluginBase, "main", 1, NULL
		);
#endif
		d1(kprintf(__FUNC__ "/%ld:  ScalosPrefsPluginBase=%08lx\n", __LINE__, ScalosPrefsPluginBase));
		if (NULL == ScalosPrefsPluginBase)
			break;

		TitleImage = (Object *) SCAGetPrefsInfo(SCAPREFSINFO_GetTitleImage);
		d1(KPrintF(__FUNC__ "/%ld:  TitleImage=%08lx\n", __LINE__, TitleImage));

		// ppl_TitleImage may be NULL
		} while (0);

	return TitleImage;
}

//----------------------------------------------------------------------------


