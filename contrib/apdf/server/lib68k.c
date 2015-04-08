#include <stddef.h>
#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <dos/dos.h>
#include <proto/exec.h>

#ifdef __SASC
#   define SAVEDS    //__saveds confuses data=faronly
#   define ASM       __asm
#   define REG(r,a)  register __ ## r a
#elif defined(__GNUC__)
#   define SAVEDS    /*__geta4*/
#   define ASM
#   define REG(r,a)  a __asm(#r)
#endif

struct MyLibrary {
    struct Library lib;
    BPTR seglist;
};

#define LIB_VERSION  1
#define LIB_REVISION 0

int LibReserved(void) {
    return 0;
}

static const APTR LibInitTable[];
extern const char LibName[];
extern const char LibId[];

#ifndef __HAVE_68881__
struct Library *__MathIeeeDoubBasBase;
struct Library *__MathIeeeDoubTransBase;
#endif

struct MyLibrary * ASM LibInit(REG(a0,BPTR seglist),
			       REG(d0,struct MyLibrary *base),
			       REG(a6,struct ExecBase *sysbase));
struct MyLibrary * ASM LibOpen(REG(a6,struct MyLibrary *base));
BPTR ASM LibClose(REG(a6,struct MyLibrary *base));
BPTR ASM LibExpunge(REG(a6,struct MyLibrary *base));

void (** SAVEDS ASM LibgetBase(void))();

static const struct Resident romtag={
    RTC_MATCHWORD,
    (struct Resident *)&romtag,
    (APTR)((&romtag)+1),
    RTF_AUTOINIT,
    LIB_VERSION,
    NT_LIBRARY,
    0,
    (char*)LibName,
    (char*)LibId,
    (APTR)LibInitTable
};

static const APTR LibFuncTable[]={
    LibOpen,
    LibClose,
    LibExpunge,
    LibReserved,
    LibgetBase,
    (APTR)-1
};

#define INITBYTE_ENTRY(x)   UWORD a##x;UWORD b##x;UBYTE c##x;UBYTE d##x
#define INITWORD_ENTRY(x)   UWORD a##x;UWORD b##x;UWORD c##x
#define INITAPTR_ENTRY(x)   UWORD a##x;UWORD b##x;APTR c##x
#define INITBYTE(S,E,x)     0xe000,offsetof(struct S,E),(x),0
#define INITWORD(S,E,x)     0xd000,offsetof(struct S,E),(x)
#define INITAPTR(S,E,x)     0xc000,offsetof(struct S,E),(x)

static const struct {
    INITBYTE_ENTRY(0);
    INITAPTR_ENTRY(1);
    INITBYTE_ENTRY(2);
    INITWORD_ENTRY(3);
    INITWORD_ENTRY(4);
    INITAPTR_ENTRY(5);
    UWORD end;
} LibInitData={
    INITBYTE(Node,ln_Type,NT_LIBRARY),
    INITAPTR(Node,ln_Name,(APTR)LibName),
    INITBYTE(Library,lib_Flags,LIBF_SUMUSED|LIBF_CHANGED),
    INITWORD(Library,lib_Version,LIB_VERSION),
    INITWORD(Library,lib_Revision,LIB_REVISION),
    INITAPTR(Library,lib_IdString,(APTR)LibId),
    0
};

static const APTR LibInitTable[]={
    (APTR)((sizeof(struct MyLibrary)+3)&~3),
    (APTR)LibFuncTable,
    (APTR)&LibInitData,
    (APTR)LibInit
};

struct ExecBase *SysBase;

extern void (*funcTable[])();


struct MyLibrary * SAVEDS ASM LibInit(REG(a0,BPTR seglist),
				    REG(d0,struct MyLibrary *base),
				    REG(a6,struct ExecBase *sysbase)) {
    base->seglist=seglist;
    SysBase=sysbase;
    return base;
}

struct MyLibrary * SAVEDS ASM LibOpen(REG(a6,struct MyLibrary *base)) {
    base->lib.lib_Flags&=~LIBF_DELEXP;
#ifndef __HAVE_68881__
    __MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library",0);
    if (__MathIeeeDoubBasBase)
	__MathIeeeDoubTransBase = OpenLibrary("mathieeedoubtrans.library",0);
    if (!__MathIeeeDoubBasBase || !__MathIeeeDoubTransBase)
	return NULL;
#endif
    ++base->lib.lib_OpenCnt;
    return base;
}

BPTR SAVEDS ASM LibClose(REG(a6,struct MyLibrary *base)) {
    BPTR retval=0;
#ifndef __HAVE_68881__
    CloseLibrary(__MathIeeeDoubTransBase);
    CloseLibrary(__MathIeeeDoubBasBase);
#endif
    if(base->lib.lib_OpenCnt>0)
	--base->lib.lib_OpenCnt;
    if(base->lib.lib_OpenCnt==0 && base->lib.lib_Flags&LIBF_DELEXP)
	retval=LibExpunge(base);
    return retval;
}

BPTR SAVEDS ASM LibExpunge(REG(a6,struct MyLibrary *base)) {
    BPTR ret=0;
    base->lib.lib_Flags|=LIBF_DELEXP;
    if(base->lib.lib_OpenCnt==0) {
	struct Library *SysBase=*(struct Library**)4;
	ret=base->seglist;
	Remove(&base->lib.lib_Node);
	FreeMem((UBYTE *)base-base->lib.lib_NegSize,
		base->lib.lib_NegSize+base->lib.lib_PosSize);
    }
    return ret;
}


void (** SAVEDS ASM LibgetBase(void))() {
    return funcTable;
}

