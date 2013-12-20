// PictureDimensions.c
// $Date$


#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/lists.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/locale.h>

#include <libraries/locale.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <datatypes/pictureclass.h>
#include <scalos/scalos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/locale.h>

#include <clib/alib_protos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <defs.h>
#include "picturedimensions_base.h"
#include "PictureDimensions.h"

#define	PictureDimensionsPlugin_NUMBERS
#define	PictureDimensionsPlugin_ARRAY
#define	PictureDimensionsPlugin_CODE
#include STR(SCALOSLOCALE)

//---------------------------------------------------------------

#define	NOT_A_LOCK		((BPTR) ~0)
#define	IS_VALID_LOCK(x)	((x) != NOT_A_LOCK)

//---------------------------------------------------------------

//#define SysBase	  PictureDimensionsBase->pdb_SysBase
//#define DataTypesBase	  PictureDimensionsBase->pdb_DataTypesBase
//#define LocaleBase	  PictureDimensionsBase->pdb_LocaleBase
//#define DOSBase	  PictureDimensionsBase->pdb_DOSBase
#ifndef __AROS__
struct Library *DataTypesBase;
struct DosLibrary *DOSBase;
T_LOCALEBASE LocaleBase;
#endif

#ifdef __amigaos4__
struct DataTypesIFace *IDataTypes;
struct DOSIFace *IDOS;
struct LocaleIFace *ILocale;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif

//---------------------------------------------------------------

static CONST_STRPTR GetLocString(ULONG MsgId, struct PictureDimensionsBase *PictureDimensionsBase);

//---------------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	struct PictureDimensionsBase *PictureDimensionsBase = (struct PictureDimensionsBase *)PluginBase;

	d1(kprintf(__FUNC__ "/%ld:   PictureDimensionsBase=%08lx  procName=<%s>\n", __LINE__, \
		PictureDimensionsBase, FindTask(NULL)->tc_Node.ln_Name));

	PictureDimensionsBase->pdb_Locale = NULL;
	PictureDimensionsBase->pdb_Catalog = NULL;

	DataTypesBase = (APTR) OpenLibrary( "datatypes.library", 39 );
	if (NULL == DataTypesBase)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	IDataTypes = (struct DataTypesIFace *)GetInterface((struct Library *)DataTypesBase, "main", 1, NULL);
	if (NULL == IDataTypes)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}
#endif

	DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
	if (NULL == DOSBase)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}
#endif

#ifdef __amigaos4__
	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif

	// LocaleBase may be NULL
	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39 );
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		{
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		if (NULL == ILocale)
			{
			CloseLibrary((struct Library *)LocaleBase);
			LocaleBase = NULL;
			}
		}
#endif

	if (LocaleBase)
		{
		if (NULL == PictureDimensionsBase->pdb_Locale)
			PictureDimensionsBase->pdb_Locale = OpenLocale(NULL);

		if (PictureDimensionsBase->pdb_Locale)
			{
			if (NULL == PictureDimensionsBase->pdb_Catalog)
				{
				PictureDimensionsBase->pdb_Catalog = OpenCatalog(PictureDimensionsBase->pdb_Locale, 
					(STRPTR) "Scalos/PictureDimensionsPlugin.catalog", NULL);
				}
			}
		}

	return TRUE;	// Success
}


BOOL closePlugin(struct PluginBase *PluginBase)
{
	struct PictureDimensionsBase *PictureDimensionsBase = (struct PictureDimensionsBase *)PluginBase;

	d1(kprintf(__FUNC__ "/%ld:\n", __LINE__));

	if (LocaleBase)
		{
		if (PictureDimensionsBase->pdb_Catalog)
			{
			CloseCatalog(PictureDimensionsBase->pdb_Catalog);
			PictureDimensionsBase->pdb_Catalog = NULL;
			}
		if (PictureDimensionsBase->pdb_Locale)
			{
			CloseLocale(PictureDimensionsBase->pdb_Locale);
			PictureDimensionsBase->pdb_Locale = NULL;
			}

#ifdef __amigaos4__
		if (ILocale)
			{
			DropInterface((struct Interface *)ILocale);
			ILocale = NULL;
			}
#endif
		CloseLibrary((struct Library *)LocaleBase);
		LocaleBase = NULL;
		}
#ifdef __amigaos4__
	if (INewlib)
		{
		DropInterface(INewlib);
		INewlib = NULL;
		}
	if (NewlibBase)
		{
		CloseLibrary(NewlibBase);
		NewlibBase = NULL;
		}
	if (IDataTypes)
		{
		DropInterface((struct Interface *)IDataTypes);
		IDataTypes = NULL;
		}
#endif
	if (DataTypesBase)
		{
		CloseLibrary(DataTypesBase);
		DataTypesBase = NULL;
		}
#ifdef __amigaos4__
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
	return TRUE;
}


// NewPictureDimensions()
LIBFUNC_P3(STRPTR, LIBToolTipInfoString,
	A0, struct ScaToolTipInfoHookData *, ttshd,
	A1, CONST_STRPTR, args,
	A6, struct PluginBase *, PluginBase, 5);
{
	struct PictureDimensionsBase *PictureDimensionsBase = (struct PictureDimensionsBase *)PluginBase;
	BPTR oldDir = NOT_A_LOCK;
	BPTR dirLock;
	Object *dt = NULL;

	(void) args;

	do	{
		struct BitMapHeader *bmhd = NULL;

		dirLock = ParentDir(ttshd->ttshd_FileLock);
		if ((BPTR)NULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		dt = NewDTObject((STRPTR) ttshd->ttshd_FileName,
				DTA_SourceType, DTST_FILE,
				PDTA_DestMode, PMODE_V43,
				DTA_GroupID, GID_PICTURE,
				PDTA_Remap, FALSE,
				TAG_END);

		if (NULL == dt)
			break;

		GetDTAttrs(dt, 
			PDTA_BitMapHeader, &bmhd,
			TAG_END);

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: bmhd=%08lx\n", __LINE__, bmhd));

		if (NULL == bmhd)
			break;

		sprintf(ttshd->ttshd_Buffer, 
			GetLocString(MSGID_PIXELS_COLOURS, PictureDimensionsBase), 
			bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth, 1 << bmhd->bmh_Depth);
		} while (0);

	if (dt)
		DisposeDTObject(dt);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);

	if (dirLock)
		UnLock(dirLock);

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ResultString=<%s>\n", __LINE__, ttshd->ttshd_Buffer));

	return ttshd->ttshd_Buffer;
}
LIBFUNC_END

//----------------------------------------------------------------------------

static CONST_STRPTR GetLocString(ULONG MsgId, struct PictureDimensionsBase *PictureDimensionsBase)
{
	struct PictureDimensionsPlugin_LocaleInfo li;

	li.li_Catalog = PictureDimensionsBase->pdb_Catalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return GetPictureDimensionsPluginString(&li, MsgId);
}

//----------------------------------------------------------------------------

#if !defined(__SASC) && !defined(__amigaos4__)
// Replacement for SAS/C library functions

void exit(int x)
{
   (void) x;
   while (1)
      ;
}

APTR _WBenchMsg;

#endif /* !__SASC && !__amigaos4__ */

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
