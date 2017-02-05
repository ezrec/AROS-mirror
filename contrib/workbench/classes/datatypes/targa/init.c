/*
 * targa.datatype
 * (c) Fredrik Wikstrom
 */

#include <exec/exec.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <stdarg.h>

#include "targa_class.h"

/* Version Tag */
#include "targa.datatype_rev.h"
#define LIBNAME "targa.datatype"

STATIC CONST UBYTE
#ifdef __GNUC__
__attribute__((used))
#endif
verstag[] = VERSTAG;

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from 
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS4) this was usually done by
 * moveq #0,d0
 * rts
 *
 */
int32 _start(void);

int32 _start(void)
{
    /* If you feel like it, open DOS and print something to the user */
    return 100;
}


/* Open the library */
STATIC struct Library *libOpen(struct LibraryManagerInterface *Self, ULONG version)
{
    struct ClassBase *libBase = (struct ClassBase *)Self->Data.LibBase; 

    if (version > VERSION)
    {
        return NULL;
    }

    /* Add any specific open code here 
       Return 0 before incrementing OpenCnt to fail opening */


    /* Add up the open count */
    libBase->libNode.lib_OpenCnt++;
    return (struct Library *)libBase;

}


/* Close the library */
STATIC APTR libClose(struct LibraryManagerInterface *Self)
{
    struct ClassBase *libBase = (struct ClassBase *)Self->Data.LibBase;
    /* Make sure to undo what open did */


    /* Make the close count */
    ((struct Library *)libBase)->lib_OpenCnt--;

    return 0;
}


static int openDTLibs (struct ClassBase *libBase);
static void closeDTLibs (struct ClassBase *libBase);

/* Expunge the library */
STATIC BPTR libExpunge(struct LibraryManagerInterface *Self)
{
    /* If your library cannot be expunged, return 0 */
    BPTR result = (BPTR)NULL;
    struct ClassBase *libBase = (struct ClassBase *)Self->Data.LibBase;
    if (libBase->libNode.lib_OpenCnt == 0)
    {
	    result = libBase->SegList;

		freeDTClass(libBase, libBase->DTClass);

		closeDTLibs(libBase);

        IExec->Remove(&libBase->libNode.lib_Node);
        IExec->DeleteLibrary(&libBase->libNode);
    }
    else
    {
        result = (BPTR)NULL;
        libBase->libNode.lib_Flags |= LIBF_DELEXP;
    }
    return result;
}

/* The ROMTAG Init Function */
STATIC struct Library * libInit (struct Library *LibraryBase, BPTR seglist, struct Interface *exec)
{
    struct ClassBase *libBase = (struct ClassBase *)LibraryBase;

    libBase->libNode.lib_Node.ln_Type = NT_LIBRARY;
    libBase->libNode.lib_Node.ln_Pri  = 0;
    libBase->libNode.lib_Node.ln_Name = LIBNAME;
    libBase->libNode.lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;
    libBase->libNode.lib_Version      = VERSION;
    libBase->libNode.lib_Revision     = REVISION;
    libBase->libNode.lib_IdString     = VSTRING;

	IExec = (struct ExecIFace *)exec;
    libBase->SegList = seglist;
	if (openDTLibs(libBase)) {
		libBase->DTClass = initDTClass(libBase);
		if (libBase->DTClass) {
			return (struct Library *)libBase;
		}
	}
	closeDTLibs(libBase);

	return NULL;
}

static int openDTLibs (struct ClassBase *libBase) {
	DOSLib = IExec->OpenLibrary("dos.library", 50);
	if (!DOSLib) return FALSE;
	IDOS = (struct DOSIFace *)IExec->GetInterface(DOSLib, "main", 1, NULL);
	if (!IDOS) return FALSE;

	IntuitionLib = IExec->OpenLibrary("intuition.library", 50);
	if (!IntuitionLib) return FALSE;
	IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionLib, "main", 1, NULL);
	if (!IIntuition) return FALSE;

	UtilityLib = IExec->OpenLibrary("utility.library", 50);
	if (!UtilityLib) return FALSE;
	IUtility = (struct UtilityIFace *)IExec->GetInterface(UtilityLib, "main", 1, NULL);
	if (!IUtility) return FALSE;

	DataTypesLib = IExec->OpenLibrary("datatypes.library", 50);
	if (!DataTypesLib) return FALSE;
	IDataTypes = (struct DataTypesIFace *)IExec->GetInterface(DataTypesLib, "main", 1, NULL);
	if (!IDataTypes) return FALSE;

	GraphicsLib = IExec->OpenLibrary("graphics.library", 50);
	if (!GraphicsLib) return FALSE;
	IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GraphicsLib, "main", 1, NULL);
	if (!IGraphics) return FALSE;

	return TRUE;
}

static void closeDTLibs (struct ClassBase *libBase) {
	IExec->DropInterface((struct Interface *)IGraphics);
	IExec->CloseLibrary(GraphicsLib);

	IExec->DropInterface((struct Interface *)IDataTypes);
	IExec->CloseLibrary(DataTypesLib);

	IExec->DropInterface((struct Interface *)IUtility);
	IExec->CloseLibrary(UtilityLib);

	IExec->DropInterface((struct Interface *)IIntuition);
	IExec->CloseLibrary(IntuitionLib);

	IExec->DropInterface((struct Interface *)IDOS);
	IExec->CloseLibrary(DOSLib);
}

/* ------------------- Manager Interface ------------------------ */
/* These are generic. Replace if you need more fancy stuff */
STATIC LONG _manager_Obtain(struct LibraryManagerInterface *Self)
{
    return Self->Data.RefCount++;
}

STATIC ULONG _manager_Release(struct LibraryManagerInterface *Self)
{
    return Self->Data.RefCount--;
}

/* Manager interface vectors */
STATIC CONST APTR lib_manager_vectors[] =
{
    (APTR)_manager_Obtain,
    (APTR)_manager_Release,
    NULL,
    NULL,
    (APTR)libOpen,
    (APTR)libClose,
    (APTR)libExpunge,
    NULL,
    (APTR)-1
};

/* "__library" interface tag list */
STATIC CONST struct TagItem lib_managerTags[] =
{
    { MIT_Name,        (Tag)"__library"       },
    { MIT_VectorTable, (Tag)lib_manager_vectors },
    { MIT_Version,     1                        },
    { TAG_DONE,        0                        }
};

/* ------------------- Library Interface(s) ------------------------ */

Class *_DTClass_ObtainEngine(struct Interface *Self) {
	struct ClassBase *libBase = (struct ClassBase *)Self->Data.LibBase;
	return libBase->DTClass;
}

STATIC CONST APTR main_vectors[] =
{
    (APTR)_manager_Obtain,
    (APTR)_manager_Release,
    NULL,
    NULL,
    (APTR)_DTClass_ObtainEngine,
    (APTR)-1
};

/* Uncomment this line (and see below) if your library has a 68k jump table */
/* extern APTR VecTable68K[]; */

STATIC CONST struct TagItem mainTags[] =
{
    { MIT_Name,        (Tag)"main" },
    { MIT_VectorTable, (Tag)main_vectors },
    { MIT_Version,     1 },
    { TAG_DONE,        0 }
};

STATIC CONST CONST_APTR libInterfaces[] =
{
    lib_managerTags,
    mainTags,
    NULL
};

STATIC CONST struct TagItem libCreateTags[] =
{
    { CLT_DataSize,    sizeof(struct ClassBase) },
    { CLT_InitFunc,    (Tag)libInit },
    { CLT_Interfaces,  (Tag)libInterfaces},
    /* Uncomment the following line if you have a 68k jump table */
    /* { CLT_Vector68K, (Tag)VecTable68K }, */
    {TAG_DONE,         0 }
};


/* ------------------- ROM Tag ------------------------ */
STATIC CONST struct Resident lib_res
#ifdef __GNUC__
__attribute__((used))
#endif
=
{
    RTC_MATCHWORD,
    (struct Resident *)&lib_res,
    (APTR)(&lib_res + 1),
    RTF_NATIVE|RTF_AUTOINIT, /* Add RTF_COLDSTART if you want to be resident */
    VERSION,
    NT_LIBRARY, /* Make this NT_DEVICE if needed */
    0, /* PRI, usually not needed unless you're resident */
    LIBNAME,
    VSTRING,
    (APTR)libCreateTags
};
