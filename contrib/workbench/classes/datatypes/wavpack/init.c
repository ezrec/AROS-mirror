/*
 *	wavpack.datatype
 *	(c) Fredrik Wikstrom
 */

#define LIBNAME "wavpack.datatype"

#include "wavpack.datatype_rev.h"
#include "class.h"

const char USED verstag[] = VERSTAG;

int _start () {
	return 100;
}

uint32 libObtain (struct LibraryManagerInterface *Self);
uint32 libRelease (struct LibraryManagerInterface *Self);
struct ClassBase *libOpen (struct LibraryManagerInterface *Self, uint32 version);
BPTR libClose (struct LibraryManagerInterface *Self);
BPTR libExpunge (struct LibraryManagerInterface *Self);

static CONST_APTR lib_manager_vectors[] = {
	(APTR)libObtain,
	(APTR)libRelease,
	NULL,
	NULL,
	(APTR)libOpen,
	(APTR)libClose,
	(APTR)libExpunge,
	NULL,
	(APTR)-1,
};

static const struct TagItem lib_managerTags[] = {
	{ MIT_Name,			(uint32)"__library"			},
	{ MIT_VectorTable,	(uint32)lib_manager_vectors	},
	{ MIT_Version,		1							},
	{ TAG_END }
};

uint32 dtObtain (struct DTClassIFace *Self);
uint32 dtRelease (struct DTClassIFace *Self);
Class * dtObtainEngine (struct DTClassIFace *Self);

static CONST_APTR lib_main_vectors[] = {
	(APTR)dtObtain,
	(APTR)dtRelease,
	NULL,
	NULL,
	(APTR)dtObtainEngine,
	(APTR)-1
};

static const struct TagItem lib_mainTags[] = {
	{ MIT_Name,			(uint32)"main"				},
	{ MIT_VectorTable,	(uint32)lib_main_vectors	},
	{ MIT_Version,		1							},
	{ TAG_END }
};

static CONST_APTR libInterfaces[] = {
	lib_managerTags,
	lib_mainTags,
	NULL
};

struct ClassBase * libInit (struct ClassBase *libBase, BPTR seglist, struct ExecIFace *ISys);

static const struct TagItem libCreateTags[] = {
	{ CLT_DataSize,		(uint32)sizeof(struct ClassBase)	},
	{ CLT_InitFunc,		(uint32)libInit						},
	{ CLT_Interfaces,	(uint32)libInterfaces				},
	{ TAG_END }
};

const struct Resident USED lib_res =
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

static int openDTLibs (struct ClassBase *libBase);
static void closeDTLibs (struct ClassBase *libBase);

struct ClassBase *libInit (struct ClassBase *libBase, BPTR seglist, struct ExecIFace *ISys) {

	libBase->libNode.lib_Node.ln_Type = NT_LIBRARY;
	libBase->libNode.lib_Node.ln_Pri  = 0;
	libBase->libNode.lib_Node.ln_Name = LIBNAME;
	libBase->libNode.lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;
	libBase->libNode.lib_Version      = VERSION;
	libBase->libNode.lib_Revision     = REVISION;
	libBase->libNode.lib_IdString     = VSTRING;

	IExec = ISys;
	libBase->SegList = seglist;
	if (openDTLibs(libBase)) {
		if (libBase->DTClass = initDTClass(libBase)) {
			return libBase;
		}
	}
	closeDTLibs(libBase);

	return NULL;
}

uint32 libObtain (struct LibraryManagerInterface *Self) {
	return Self->Data.RefCount++;
}

uint32 libRelease (struct LibraryManagerInterface *Self) {
	return Self->Data.RefCount--;
}

struct ClassBase *libOpen (struct LibraryManagerInterface *Self, uint32 version) {
	struct ClassBase *libBase;

	libBase = (struct ClassBase *)Self->Data.LibBase;

	libBase->libNode.lib_OpenCnt++;
	libBase->libNode.lib_Flags &= ~LIBF_DELEXP;

	return libBase;
}

BPTR libClose (struct LibraryManagerInterface *Self) {
	struct ClassBase *libBase;

	libBase = (struct ClassBase *)Self->Data.LibBase;

	libBase->libNode.lib_OpenCnt--;

	if (libBase->libNode.lib_OpenCnt) {
		return 0;
	}

	if (libBase->libNode.lib_Flags & LIBF_DELEXP) {
		return (BPTR)Self->LibExpunge();
	} else {
		return 0;
	}
}

BPTR libExpunge (struct LibraryManagerInterface *Self) {
	struct ClassBase *libBase;
	BPTR result = 0;

	libBase = (struct ClassBase *)Self->Data.LibBase;

	if (libBase->libNode.lib_OpenCnt == 0) {
		IExec->Remove(&libBase->libNode.lib_Node);

		result = libBase->SegList;

		freeDTClass(libBase, libBase->DTClass);
		libBase->DTClass = NULL;

		closeDTLibs(libBase);

		IExec->Remove(&libBase->libNode.lib_Node);
		IExec->DeleteLibrary(&libBase->libNode);
	} else {
		libBase->libNode.lib_Flags |= LIBF_DELEXP;
	}

	return result;
}

uint32 dtObtain (struct DTClassIFace *Self) {
	return(Self->Data.RefCount++);
}

uint32 dtRelease (struct DTClassIFace *Self) {
	return(Self->Data.RefCount--);
}

Class * dtObtainEngine (struct DTClassIFace *Self) {
	struct ClassBase *libBase;
	libBase = (struct ClassBase *)Self->Data.LibBase;
	return libBase->DTClass;
}

struct Library *DOSBase;
struct DOSIFace *IDOS;
struct Library *NewlibBase;
struct Interface *INewlib;

static int openDTLibs (struct ClassBase *libBase) {
	DOSBase = IExec->OpenLibrary("dos.library", 52);
	IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	if (!IDOS) return FALSE;

	IntuitionBase = IExec->OpenLibrary("intuition.library", 52);
	IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
	if (!IIntuition) return FALSE;

	UtilityBase = IExec->OpenLibrary("utility.library", 52);
	IUtility = (struct UtilityIFace *)IExec->GetInterface(UtilityBase, "main", 1, NULL);
	if (!IUtility) return FALSE;

	DataTypesBase = IExec->OpenLibrary("datatypes.library", 52);
	IDataTypes = (struct DataTypesIFace *)IExec->GetInterface(DataTypesBase, "main", 1, NULL);
	if (!IDataTypes) return FALSE;

	NewlibBase = IExec->OpenLibrary("newlib.library", 52);
	INewlib = IExec->GetInterface(NewlibBase, "main", 1, NULL);
	if (!INewlib) return FALSE;

	return TRUE;
}

static void closeDTLibs (struct ClassBase *libBase) {
	IExec->DropInterface((struct Interface *)INewlib);
	IExec->CloseLibrary(NewlibBase);

	IExec->DropInterface((struct Interface *)IDataTypes);
	IExec->CloseLibrary(DataTypesBase);

	IExec->DropInterface((struct Interface *)IUtility);
	IExec->CloseLibrary(UtilityBase);

	IExec->DropInterface((struct Interface *)IIntuition);
	IExec->CloseLibrary(IntuitionBase);

	IExec->DropInterface((struct Interface *)IDOS);
	IExec->CloseLibrary(DOSBase);
}
