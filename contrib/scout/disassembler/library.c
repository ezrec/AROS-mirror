#include <aros/debug.h>
#include <aros/asmcall.h>
#include <aros/libcall.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <libraries/disassembler.h>
#include <proto/exec.h>

#include "bfd.h"
#include "library.h"

#define LIBNAME "disassembler.library"
#define VERSION 41
#define REVISION 0
#define VSTRING "\0$VER:disassembler.library 41.0 (" __DATE__ ")"

AROS_LD1(struct DisasmBase *, LIB_Open,
         AROS_LPA(ULONG, version, D0),
         struct DisasmBase *, DisassemblerBase, 1, Disassembler);
AROS_LD0(BPTR, LIB_Close,
         struct DisasmBase *, DisassemblerBase, 2, Disassembler);
AROS_LD1(BPTR, LIB_Expunge,
         AROS_LPA(struct DisasmBase *, Base, D0),
	 struct DisasmBase *, DisassemblerBase, 3, Disassembler);
AROS_LD0I(LONG, LIB_Reserved,
         struct DisasmBase *, DisassemblerBase, 4, Disassembler);
AROS_LD1(APTR, Disassemble,
	 AROS_LPA(struct DisData *, ds, A0),
	 struct DisasmBase *, DisassemblerBase, 11, Disassembler);
AROS_LD3(APTR, FindStartPosition,
	 AROS_LPA(APTR, startpc, A0),
	 AROS_LPA(UWORD, min, D0),
	 AROS_LPA(UWORD, max, D1),
	 struct DisasmBase *, DisassemblerBase, 12, Disassembler);

static struct DisasmBase* _LIB_Init(struct DisasmBase *DisassemblerBase, BPTR SegList, struct ExecBase *SBase);

struct ExecBase *SysBase;

struct LibInitStruct
{
	ULONG	LibSize;
	void	*FuncTable;
	void	*DataTable;
	void	(*InitFunc)(void);
};

static IPTR LibFuncTable[] = {
#ifdef __MORPHOS__
    FUNCARRAY_32BIT_NATIVE,
#endif
    (IPTR)AROS_SLIB_ENTRY(LIB_Open, Disassembler, 1),
    (IPTR)AROS_SLIB_ENTRY(LIB_Close, Disassembler, 2),
    (IPTR)AROS_SLIB_ENTRY(LIB_Expunge, Disassembler, 3),
    (IPTR)AROS_SLIB_ENTRY(LIB_Reserved, Disassembler, 0),
    /* 6 reserved slots */
    (IPTR)AROS_SLIB_ENTRY(LIB_Reserved, Disassembler, 0),
    (IPTR)AROS_SLIB_ENTRY(LIB_Reserved, Disassembler, 0),
    (IPTR)AROS_SLIB_ENTRY(LIB_Reserved, Disassembler, 0),
    (IPTR)AROS_SLIB_ENTRY(LIB_Reserved, Disassembler, 0),
    (IPTR)AROS_SLIB_ENTRY(LIB_Reserved, Disassembler, 0),
    (IPTR)AROS_SLIB_ENTRY(LIB_Reserved, Disassembler, 0),
    (IPTR)AROS_SLIB_ENTRY(Disassemble, Disassembler, 11),
    (IPTR)AROS_SLIB_ENTRY(FindStartPosition, Disassembler, 12),
    (IPTR)-1
};

#ifdef __MORPHOS__
#define LIB_Init _LIB_Init

static ULONG __abox__=1;

#else
#define RTF_PPC 0

static AROS_UFP3(struct DisasmBase *, LIB_Init,
                 AROS_UFPA(struct DisasmBase *, DisassemblerBase, D0),
                 AROS_UFPA(BPTR, SegList, A0),
                 AROS_UFPA(struct ExecBase *, SBase, A6));

#endif

static struct LibInitStruct LibInitStruct=
{
	sizeof(struct DisasmBase),
	LibFuncTable,
	NULL,
	(void (*)(void)) LIB_Init
};

static char version[] = VSTRING;

struct Resident LibResident=
{
	RTC_MATCHWORD,
	&LibResident,
	&LibResident + 1,
	RTF_PPC | RTF_AUTOINIT,
	VERSION,
	NT_LIBRARY,
	0,
	LIBNAME,
	&version[6],
	&LibInitStruct
};

static LONG __startup NoExecute(void)
{
    return -1;
}

#ifndef __MORPHOS__

static AROS_UFH3(struct DisasmBase *, LIB_Init,
                 AROS_UFHA(struct DisasmBase *, DisassemblerBase, D0),
                 AROS_UFHA(BPTR, SegList, A0),
                 AROS_UFHA(struct ExecBase *, SBase, A6))
{
  AROS_USERFUNC_INIT
  
  return _LIB_Init(DisassemblerBase, SegList, SBase);
  
  AROS_USERFUNC_EXIT
}

#endif

static struct DisasmBase *_LIB_Init(struct DisasmBase *DisassemblerBase,
			          BPTR SegList, struct ExecBase *SBase)
{
    SysBase = SBase;
    
    DisassemblerBase->lib.lib_Flags = LIBF_CHANGED | LIBF_SUMUSED;
    DisassemblerBase->lib.lib_Revision = REVISION;
    DisassemblerBase->seglist = SegList;

    InitSemaphore(&DisassemblerBase->sem);
    init_default_bfd(&DisassemblerBase->default_bfd);

    return DisassemblerBase;
}

static BPTR _LIB_Expunge(struct DisasmBase *DisassemblerBase)
{
    BPTR seglist;

    if (DisassemblerBase->lib.lib_OpenCnt) {
        DisassemblerBase->lib.lib_Flags |= LIBF_DELEXP;
        return NULL;
    }

    Forbid();
    Remove(&DisassemblerBase->lib.lib_Node);
    Permit();
    
    seglist = DisassemblerBase->seglist;
    FreeMem((APTR)DisassemblerBase - DisassemblerBase->lib.lib_NegSize,
	    DisassemblerBase->lib.lib_NegSize + DisassemblerBase->lib.lib_PosSize);

    return seglist;
}

AROS_LH1(BPTR, LIB_Expunge,
         AROS_LHA(struct DisasmBase *, Base, D0),
	 struct DisasmBase *, DisassemblerBase, 3, Disassembler)
{
    AROS_LIBFUNC_INIT
   
    return _LIB_Expunge(DisassemblerBase);
   
    AROS_LIBFUNC_EXIT
}

AROS_LH1(struct DisasmBase *, LIB_Open,
         AROS_LHA(ULONG, version, D0),
         struct DisasmBase *, DisassemblerBase, 1, Disassembler)
{
    AROS_LIBFUNC_INIT

    DisassemblerBase->lib.lib_Flags &= ~LIBF_DELEXP;
    DisassemblerBase->lib.lib_OpenCnt++;
    D(bug("[Disassembler] New OpenCnt is %u\n", DisassemblerBase->lib.lib_OpenCnt));

    return DisassemblerBase;
    
    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, LIB_Close,
         struct DisasmBase *, DisassemblerBase, 2, Disassembler)
{
    AROS_LIBFUNC_INIT

    if ((--DisassemblerBase->lib.lib_OpenCnt) == 0) {
        if (DisassemblerBase->lib.lib_Flags & LIBF_DELEXP)
            return _LIB_Expunge(DisassemblerBase);
    }

    return 0;
    
    AROS_LIBFUNC_EXIT
}

AROS_LH0I(LONG, LIB_Reserved,
          struct DisasmBase *, DisassemblerBase, 4, Disassembler)
{
    AROS_LIBFUNC_INIT

    return -1;
    
    AROS_LIBFUNC_EXIT
}
