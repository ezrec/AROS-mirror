// OpenDrawerByName.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/memory.h>
#include <workbench/icon.h>
#include <workbench/workbench.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <defs.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//-------------------------------------------------------------------------

static Object *ReadIconObject(CONST_STRPTR Path);
static Object *ReadIconObject_NoFail(CONST_STRPTR Path);

//-------------------------------------------------------------------------

struct ScaWindowStruct *OpenDrawerByName(CONST_STRPTR Path, struct TagItem *TagList)
{
	struct ScaWindowStruct *Result = NULL;
	Object *iconObj;
	Object *AllocIconObj = NULL;
	struct SM_StartWindow *StartWindowMsg = NULL;

	d1(KPrintF("%s/%s/%ld:  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

	do	{
		// special treatment for Volumes/Devices : look for "VOLNAME:disk.info"
		size_t len = strlen(Path);

		if (':' == Path[len - 1])
			{
			d1(KPrintF("%s/%s/%ld: open device window Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));
			SCA_OpenIconWindowTags(
				SCA_Path, (ULONG) Path,
				SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
				TAG_MORE, (ULONG) TagList,
				TAG_END
				);
			}
		else
			{
			iconObj = AllocIconObj = ReadIconObject(Path);

			d1(KPrintF("%s/%s/%ld: iconObj=%08lx  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, iconObj, Path));

			if (NULL == iconObj)
				{
				iconObj = AllocIconObj = ReadIconObject_NoFail(Path);

				if (NULL == iconObj)
					break;

				SetAttrs(iconObj, 
					IDTA_Flags, DDFLAGS_SHOWALL,
					TAG_END);
				}

			d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

			if (NULL == iconObj)
				break;

			d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

			SCA_OpenIconWindowTags(
				SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
				SCA_IconObject, (ULONG) iconObj,
				SCA_WindowStruct, (ULONG) &Result,
				TAG_MORE, (ULONG) TagList,
				TAG_END
				);
			}
		} while (0);

	if (StartWindowMsg)
		SCA_FreeMessage(&StartWindowMsg->ScalosMessage);
	if (AllocIconObj)
		DisposeIconObject(AllocIconObj);

	d1(KPrintF("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static Object *ReadIconObject(CONST_STRPTR Path)
{
	Object *iconObj;
	STRPTR FullPath;

	FullPath = AllocPathBuffer();
	if (FullPath)
		{
		// change path names like "Download:" to a fully qualified name
		BPTR doLock = Lock((STRPTR) Path, ACCESS_READ);

		d1(kprintf("%s/%s/%ld  doLock=%08lx\n", __FILE__, __FUNC__, __LINE__, doLock));

		if (doLock)
			{
			if (NameFromLock(doLock, FullPath, Max_PathLen))
				Path = FullPath;

			UnLock(doLock);
			}
		}

	d1(kprintf("%s/%s/%ld  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

	iconObj = ReadIconObject_NoFail(Path);

	if (FullPath)
		FreePathBuffer(FullPath);

	return iconObj;
}


static Object *ReadIconObject_NoFail(CONST_STRPTR Path)
{
	Object *iconObj;

	iconObj = NewIconObject(Path, NULL);
	if (NULL == iconObj)
		{
		struct WBArg OriginalLocation;
		BPTR dirLock;
		BPTR parentLock = (BPTR)NULL;

		d1(kprintf("%s/%s/%ld  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

		do	{
			dirLock = Lock(Path, ACCESS_READ);
			d1(kprintf("%s/%s/%ld  dirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, dirLock));
			if ((BPTR)NULL == dirLock)
				break;

			parentLock = ParentDir(dirLock);
			d1(kprintf("%s/%s/%ld  parentLock=%08lx\n", __FILE__, __FUNC__, __LINE__, parentLock));
			if (parentLock)
				{
				OriginalLocation.wa_Lock = parentLock;
				OriginalLocation.wa_Name = (STRPTR) FilePart(Path);
				}
			else
				{
				OriginalLocation.wa_Lock = (BPTR)NULL;
				OriginalLocation.wa_Name = (STRPTR) Path;
				}

			d1(kprintf("%s/%s/%ld  wa_Lock=%08lx  wa_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, OriginalLocation.wa_Lock, OriginalLocation.wa_Name));

			iconObj = GetDefIconObjectTags(WBDRAWER, 
				IDTA_IconLocation, (ULONG) &OriginalLocation,
				DTA_Name, (ULONG) Path,
				TAG_END);

			d1(kprintf("%s/%s/%ld  iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
			} while (0);

		if (parentLock)
			UnLock(parentLock);
		if (dirLock)
			UnLock(dirLock);
		}

	return iconObj;
}

