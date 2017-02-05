/*
 *	wave.datatype by Fredrik Wikstrom
 *
 */

#define LIBNAME "au.datatype"
#define LIBPRI 0

#include "au.datatype_rev.h"
#include "au_class.h"

#ifdef __GNUC__
static char __attribute__((used)) verstag[] = VERSTAG;
#else
static char verstag[] = VERSTAG;
#endif

uint32 libObtain (struct LibraryManagerInterface *Self);
uint32 libRelease (struct LibraryManagerInterface *Self);
struct ClassBase *libOpen (struct LibraryManagerInterface *Self, uint32 version);
BPTR libClose (struct LibraryManagerInterface *Self);
BPTR libExpunge (struct LibraryManagerInterface *Self);

static APTR lib_manager_vectors[] = {
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

static struct TagItem lib_managerTags[] = {
	{ MIT_Name,		(uint32)"__library"			},
	{ MIT_VectorTable,	(uint32)lib_manager_vectors	},
	{ MIT_Version,		1						},
	{ TAG_END,		0						}
};

uint32 dtObtain (struct DTClassIFace *Self);
uint32 dtRelease (struct DTClassIFace *Self);
Class * dtObtainEngine (struct DTClassIFace *Self);

static APTR lib_main_vectors[] = {
	(APTR)dtObtain,
	(APTR)dtRelease,
	NULL,
	NULL,
	(APTR)dtObtainEngine,
	(APTR)-1
};

static struct TagItem lib_mainTags[] = {
	{ MIT_Name,		(uint32)"main"				},
	{ MIT_VectorTable,	(uint32)lib_main_vectors		},
	{ MIT_Version,		1						},
	{ TAG_END,		0						}
};

static APTR libInterfaces[] = {
	lib_managerTags,
	lib_mainTags,
	NULL
};

struct ClassBase * libInit (struct ClassBase *libBase, BPTR seglist, struct ExecIFace *ISys);

static struct TagItem libCreateTags[] = {
	{ CLT_DataSize,		(uint32)sizeof(struct ClassBase)	},
	{ CLT_InitFunc,		(uint32)libInit				},
	{ CLT_Interfaces,	(uint32)libInterfaces			},
	{ TAG_END,		0						}
};

#ifdef __GNUC__
static struct Resident __attribute__((used)) lib_res = {
#else
static struct Resident lib_res = {
#endif
	RTC_MATCHWORD,	// rt_MatchWord
	&lib_res,			// rt_MatchTag
	&lib_res+1,		// rt_EndSkip
	RTF_NATIVE | RTF_AUTOINIT,	// rt_Flags
	VERSION,			// rt_Version
	NT_LIBRARY,		// rt_Type
	LIBPRI,			// rt_Pri
	LIBNAME,			// rt_Name
	VSTRING,			// rt_IdString
	libCreateTags		// rt_Init
};

void _start () {
}

static int openDTLibs (struct ClassBase *libBase, struct ExecIFace *ISys);
static void closeDTLibs (struct ClassBase *libBase);

struct ClassBase * libInit (struct ClassBase *libBase, BPTR seglist, struct ExecIFace *ISys) {

	libBase->libNode.lib_Node.ln_Type = NT_LIBRARY;
	libBase->libNode.lib_Node.ln_Pri = LIBPRI;
	libBase->libNode.lib_Node.ln_Name = LIBNAME;
	libBase->libNode.lib_Flags = LIBF_SUMUSED|LIBF_CHANGED;
	libBase->libNode.lib_Version = VERSION;
	libBase->libNode.lib_Revision = REVISION;
	libBase->libNode.lib_IdString = VSTRING;

	libBase->SegList = seglist;
	if (openDTLibs(libBase, ISys)) {
		if (initDTClass(libBase)) {
			return(libBase);
		}
	}
	closeDTLibs(libBase);

	return(NULL);
}

uint32 libObtain( struct LibraryManagerInterface *Self ) {
	return( Self->Data.RefCount++ );
}

uint32 libRelease( struct LibraryManagerInterface *Self ) {
	return( Self->Data.RefCount-- );
}

struct ClassBase *libOpen( struct LibraryManagerInterface *Self, uint32 version ) {
	struct ClassBase *libBase;

	libBase = (struct ClassBase *)Self->Data.LibBase;

	libBase->libNode.lib_OpenCnt++;
	libBase->libNode.lib_Flags &= ~LIBF_DELEXP;

	return( libBase );
}

BPTR libClose( struct LibraryManagerInterface *Self ) {
	struct ClassBase *libBase;

	libBase = (struct ClassBase *)Self->Data.LibBase;

	libBase->libNode.lib_OpenCnt--;

	if ( libBase->libNode.lib_OpenCnt ) {
		return( 0 );
	}

	if ( libBase->libNode.lib_Flags & LIBF_DELEXP ) {
		return( (BPTR)Self->LibExpunge() );
	} else {
		return( 0 );
	}
}

BPTR libExpunge( struct LibraryManagerInterface *Self ) {
	struct ClassBase *libBase;
	BPTR result = 0;

	libBase = (struct ClassBase *)Self->Data.LibBase;

	if (libBase->libNode.lib_OpenCnt == 0) {
		IExec->Remove(&libBase->libNode.lib_Node);

		result = libBase->SegList;

		freeDTClass(libBase);

		closeDTLibs(libBase);

		IExec->DeleteLibrary(&libBase->libNode);
	} else {
		libBase->libNode.lib_Flags |= LIBF_DELEXP;
	}

	return(result);
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
	return(libBase->DTClass);
}

#if GLOBAL_IFACES != FALSE
struct ExecIFace *		IExec = NULL;
struct DOSIFace *		IDOS = NULL;
struct IntuitionIFace *	IIntuition = NULL;
struct UtilityIFace *		IUtility = NULL;
struct DataTypesIFace *	IDataTypes = NULL;
#endif

static int openDTLibs (struct ClassBase *libBase, struct ExecIFace *ISys) {
	IExec = ISys;

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
	IDataTypes =(struct DataTypesIFace *)IExec->GetInterface(DataTypesLib, "main", 1, NULL);
	if (!IDataTypes) return FALSE;

	return TRUE;
}

static void closeDTLibs (struct ClassBase *libBase) {
	IExec->DropInterface((struct Interface *)IDataTypes);
	IExec->CloseLibrary(DataTypesLib);

	IExec->DropInterface((struct Interface *)IUtility);
	IExec->CloseLibrary(UtilityLib);

	IExec->DropInterface((struct Interface *)IIntuition);
	IExec->CloseLibrary(IntuitionLib);

	IExec->DropInterface((struct Interface *)IDOS);
	IExec->CloseLibrary(DOSLib);
}
