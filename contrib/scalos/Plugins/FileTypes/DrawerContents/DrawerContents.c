// DrawerContents.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/lists.h>

#include <libraries/locale.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/locale.h>

#include <clib/alib_protos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <defs.h>
#include "DrawerContents_base.h"
#include "DrawerContents.h"

#define	DrawerContentsPlugin_NUMBERS
#define	DrawerContentsPlugin_ARRAY
#define	DrawerContentsPlugin_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#ifndef __amigaos4__
// aus mempools.lib
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif

//----------------------------------------------------------------------------

struct DosLibrary *DOSBase;
T_LOCALEBASE LocaleBase;

#ifdef __amigaos4__
struct DOSIFace *IDOS;
struct LocaleIFace *ILocale;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif

//---------------------------------------------------------------

static BOOL isDrawer(const struct FileInfoBlock *fib);
static CONST_STRPTR GetLocString(ULONG MsgId, struct DrawerContentsBase *DrawerContentsBase);

//---------------------------------------------------------------

#define	NOT_A_LOCK		((BPTR) ~0)
#define	IS_VALID_LOCK(x)	((x) != NOT_A_LOCK)

//---------------------------------------------------------------

BOOL initPlugin (struct PluginBase *PluginBase)
{
	struct DrawerContentsBase *DrawerContentsBase = (struct DrawerContentsBase *)PluginBase;

	d1(kprintf("%s/%s/%ld:   DrawerContentsBase=%08lx  procName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		DrawerContentsBase, FindTask(NULL)->tc_Node.ln_Name));

	DrawerContentsBase->dcb_Locale = NULL;
	DrawerContentsBase->dcb_Catalog = NULL;

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STI_240_InitMemFunctions();
#endif

	DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
	if (NULL == DOSBase)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __FILE__, __FUNC__, __LINE__));
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
		if (NULL == DrawerContentsBase->dcb_Locale)
			DrawerContentsBase->dcb_Locale = OpenLocale(NULL);

		if (DrawerContentsBase->dcb_Locale)
			{
			if (NULL == DrawerContentsBase->dcb_Catalog)
				{
				DrawerContentsBase->dcb_Catalog = OpenCatalog(DrawerContentsBase->dcb_Locale, 
					(STRPTR) "Scalos/DrawerContentsPlugin.catalog", NULL);
				}
			}
		}

	d1(KPrintF("%s/%s/%ld:  END - success\n", __FILE__, __FUNC__, __LINE__));

	return TRUE;	// Success
}


VOID closePlugin(struct PluginBase *PluginBase)
{
	struct DrawerContentsBase *DrawerContentsBase = (struct DrawerContentsBase *)PluginBase;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (LocaleBase)
		{
		if (DrawerContentsBase->dcb_Catalog)
			{
			CloseCatalog(DrawerContentsBase->dcb_Catalog);
			DrawerContentsBase->dcb_Catalog = NULL;
			}
		if (DrawerContentsBase->dcb_Locale)
			{
			CloseLocale(DrawerContentsBase->dcb_Locale);
			DrawerContentsBase->dcb_Locale = NULL;
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

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif
}


LIBFUNC_P3(STRPTR, LIBToolTipInfoString,
	A0, struct ScaToolTipInfoHookData *, ttshd,
	A1, CONST_STRPTR, args,
	A6, struct PluginBase *, PluginBase)
{
	struct DrawerContentsBase *DrawerContentsBase = (struct DrawerContentsBase *)PluginBase;
	struct RDArgs *allocRdArg;
	struct RDArgs *rdArg = NULL;
	STRPTR ArgsCopy = NULL;
	struct FileInfoBlock *fib = NULL;
	BOOL ScanForDirectories = FALSE;
	BOOL SkipIcons = FALSE;
	BOOL EmptyMsg = FALSE;
	ULONG MaxLineLength = 30;
	ULONG MaxEntries = 4;

	d1(kprintf(__FILE__ "/" "%s/%s/%ld: args=<%s>\n", __FILE__, __FUNC__, __LINE__, args));

	do	{
		char FaultBuffer[100];
		char PatternBuffer[40];
		SIPTR ArgArray[5];
		ULONG *argMaxEntries;
		ULONG *argLineLength;
		STRPTR lp = ttshd->ttshd_Buffer;
		size_t Len = ttshd->ttshd_BuffLen;
		ULONG Entries, Count;
		BOOL Success;

		memset(ArgArray, 0, sizeof(ArgArray));

		allocRdArg = AllocDosObject(DOS_RDARGS, NULL);

		d1(kprintf("%s/%s/%ld: allocRdArg=%08lx\n", __FILE__, __FUNC__, __LINE__, allocRdArg));
		if (NULL == allocRdArg)
			break;

		ArgsCopy = malloc(2 + strlen(args));
		if (NULL == ArgsCopy)
			break;

		// important: ReadArgs() requires a trailing "\n" !!
		strcpy(ArgsCopy, args);
		strcat(ArgsCopy, "\n");

		allocRdArg->RDA_Source.CS_Buffer = ArgsCopy;
		allocRdArg->RDA_Source.CS_Length = strlen(ArgsCopy);
		allocRdArg->RDA_Source.CS_CurChr = 0;
		allocRdArg->RDA_Flags |= RDAF_NOPROMPT;

		rdArg = ReadArgs("DIRS/S,ENTRIES/K/N,LINELENGTH/K/N,NOICONS/S,SHOWEMPTY/S", 
			ArgArray, allocRdArg);

		d1(kprintf("%s/%s/%ld: rdArg=%08lx\n", __FILE__, __FUNC__, __LINE__, rdArg));
		if (NULL == rdArg)
			{
			Fault(IoErr(), NULL, FaultBuffer, sizeof(FaultBuffer)-1);
			sprintf(ttshd->ttshd_Buffer,  GetLocString(MSGID_ERROR_DOS, DrawerContentsBase), IoErr(), FaultBuffer);

			d1(kprintf(__FILE__ "/" "%s/%s/%ld: ReadArgs() returned error %ld\n", __FILE__, __FUNC__, __LINE__, IoErr()));
			break;
			}

		argMaxEntries = (ULONG *) ArgArray[1];
		argLineLength = (ULONG *) ArgArray[2];

		if (ArgArray[0])
			ScanForDirectories = TRUE;
		if (argMaxEntries)
			MaxEntries = *argMaxEntries;
		if (argLineLength)
			MaxLineLength = *argLineLength;
		if (ArgArray[3])
			SkipIcons = TRUE;
		if (ArgArray[4])
			EmptyMsg = TRUE;

		if (MaxEntries < 1 || MaxEntries > 64)
			break;
		if (MaxLineLength < 5 || MaxLineLength > 256)
			break;

		if (ParsePatternNoCase(SkipIcons ? "~(#?.INFO)" : "#?", PatternBuffer, sizeof(PatternBuffer)) < 0)
			break;

		fib = AllocDosObject(DOS_FIB, NULL);

		d1(kprintf(__FILE__ "/" "%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));
		if (NULL == fib)
			break;

		if ((BPTR)NULL == ttshd->ttshd_FileLock)
			break;

		if (!Examine(ttshd->ttshd_FileLock, fib))
			break;

		if (!isDrawer(fib))
			break;		// this is a plain file...

		Entries = Count = 0;
		while ((Success = ExNext(ttshd->ttshd_FileLock, fib)) && Count < MaxEntries && Len > 1)
			{
			d1(kprintf(__FILE__ "/" "%s/%s/%ld: FileName=<%s>  EntryType=%ld\n", \
				__FILE__, __FUNC__, __LINE__, fib->fib_FileName, fib->fib_DirEntryType));

			Entries++;

			if (strlen(ttshd->ttshd_Buffer) > MaxLineLength)
				break;

			if (MatchPatternNoCase(PatternBuffer, fib->fib_FileName))
				{
				if ((ScanForDirectories && isDrawer(fib))
					|| (!ScanForDirectories && !isDrawer(fib)))
					{
					if (Count > 0)
						{
						stccpy(lp, ",", Len);
						Len -= strlen(lp);
						lp += strlen(lp);
						}

					stccpy(lp, fib->fib_FileName, Len);
					Len -= strlen(lp);
					lp += strlen(lp);

					Count++;
					}
				}
			}
		if (0 == Entries)
			{
			if (EmptyMsg)
				stccpy(lp, GetLocString(MSGID_DRAWER_EMPTY, DrawerContentsBase), Len);
			}
		else
			{
			if (Success && Count > 0)
				stccpy(lp,GetLocString(MSGID_MORE_ENTRIES, DrawerContentsBase), Len);
			}
		} while (0);

	if (rdArg)
		FreeArgs(rdArg);
	if (allocRdArg)
		FreeDosObject(DOS_RDARGS, allocRdArg);
	if (fib)
		FreeDosObject(DOS_FIB, fib);
	if (ArgsCopy)
		free(ArgsCopy);

	d1(kprintf(__FILE__ "/" "%s/%s/%ld: ResultString=<%s>\n", __FILE__, __FUNC__, __LINE__, ttshd->ttshd_Buffer));

	return ttshd->ttshd_Buffer;
}
LIBFUNC_END


static BOOL isDrawer(const struct FileInfoBlock *fib)
{
	BOOL Result;

	switch (fib->fib_DirEntryType)
		{
	case ST_ROOT:
	case ST_USERDIR:
	case ST_LINKDIR:
		Result = TRUE;
		break;
	case ST_SOFTLINK:
	case ST_FILE:
	case ST_LINKFILE:
		Result = FALSE;
		break;
	default:
		Result = fib->fib_DirEntryType >= 0;
		}

	return Result;
}

//----------------------------------------------------------------------------

static CONST_STRPTR GetLocString(ULONG MsgId, struct DrawerContentsBase *DrawerContentsBase)
{
	struct DrawerContentsPlugin_LocaleInfo li;

	li.li_Catalog = DrawerContentsBase->dcb_Catalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return GetDrawerContentsPluginString(&li, MsgId);
}

//-----------------------------------------------------------------------------

void _XCEXIT(long x)
{
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
