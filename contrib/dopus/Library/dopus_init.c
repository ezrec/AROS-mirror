/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc: DOpus library initialization code.
    Lang: English.
*/

#define AROS_ALMOST_COMPATIBLE /* For NEWLIST() macro */

#include <stddef.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <exec/resident.h>
#include <aros/libcall.h>
#include <proto/exec.h>
#include <proto/boopsi.h>

/* #include "initstruct.h" */
#include "dopuslib.h"
#include "initstruct.h"
#include "libdefs.h"

#define INIT AROS_SLIB_ENTRY(init, DOpus)

#define DEBUG 0
#include <aros/debug.h>


extern UBYTE pdb_cycletop[];
extern UBYTE pdb_cyclebot[];
extern UBYTE pdb_check[];



struct inittable;
extern const char name[];
extern const char version[];
extern const APTR inittabl[4];
extern void *const LIBFUNCTABLE[];
extern const struct inittable datatable;
extern struct DOpusBase *INIT();
extern struct DOpusBase *AROS_SLIB_ENTRY(open,Dopus)();
extern BPTR AROS_SLIB_ENTRY(close,DOpus)();
extern BPTR AROS_SLIB_ENTRY(expunge,DOpus)();
extern int AROS_SLIB_ENTRY(null,DOpus)();
extern const char LIBEND;


#define expunge() \
AROS_LC0(BPTR, expunge, struct DOpusBase *, DOpusBase, 3, DOpus)


/* FIXME: egcs 1.1b and possibly other incarnations of gcc have
 * two nasty problems with entry() that prevents using the
 * C version of this function on AROS 68k native.
 *
 * First of all, if inlining is active (-O3), the optimizer will decide
 * that entry() is simple enough to be inlined, and it doesn't generate
 * its code until all other functions have been compiled. Delaying asm
 * output for a global (non static) function is probably silly because
 * the optimizer can't eliminate its stand alone istance anyway.
 *
 * The second problem is that even without inlining, the code generator
 * adds a nop instruction immediately after rts. This is probably done
 * to help the 68040/60 pipelines, but it adds two more bytes before the
 * library resident tag, which causes all kinds of problems on native
 * AmigaOS.
 *
 * The workaround is to embed the required assembler instructions
 * (moveq #-1,d0 ; rts) in a constant variable.
 */
#if (defined(__mc68000__) && (AROS_FLAVOUR & AROS_FLAVOUR_NATIVE))
const LONG entry = 0x70FF4E75;
#else
int entry(void)
{
    /* If the library was executed by accident return error code. */
    return -1;
}
#endif

static const struct Resident resident=
{
    RTC_MATCHWORD,
    (struct Resident *)&resident,
    (APTR)&LIBEND,
    RTF_AUTOINIT,
    VERSION_NUMBER,
    NT_LIBRARY,
    0,	/* priority */
    (char *)name,
    (char *)&version[6],
    (ULONG *)inittabl
};

const char name[]=NAME_STRING;

const char version[]=VERSION_STRING;

const APTR inittabl[4]=
{
    (APTR)sizeof(struct DOpusBase),
    (APTR)LIBFUNCTABLE,
    (APTR)&datatable,
    &INIT
};

struct inittable
{
    S_CPYO(1,1,B);
    S_CPYO(2,1,L);
    S_CPYO(3,1,B);
    S_CPYO(4,1,W);
    S_CPYO(5,1,W);
    S_CPYO(6,1,L);
    S_END (LIBEND);
};

#define O(n) offsetof(struct DOpusBase,n)

const struct inittable datatable=
{
    { { I_CPYO(1,B,O(LibNode.lib_Node.ln_Type)), { NT_LIBRARY } } },
    { { I_CPYO(1,L,O(LibNode.lib_Node.ln_Name)), { (IPTR)name } } },
    { { I_CPYO(1,B,O(LibNode.lib_Flags       )), { LIBF_SUMUSED|LIBF_CHANGED } } },
    { { I_CPYO(1,W,O(LibNode.lib_Version     )), { VERSION_NUMBER } } },
    { { I_CPYO(1,W,O(LibNode.lib_Revision    )), { REVISION_NUMBER } } },
    { { I_CPYO(1,L,O(LibNode.lib_IdString    )), { (IPTR)&version[6] } } },
  I_END ()
};


#warning Remove DOpusBase as global variable
/* AROS: DOpusBase global var shadows the parameter with the same name */


void set_global_dopusbase(struct DOpusBase *base)
{
	DOpusBase = base;
}

AROS_LH2(struct DOpusBase *, init,
    AROS_LHA(struct DOpusBase *, LIBBASE, D0),
    AROS_LHA(BPTR,               segList,   A0),
    struct ExecBase *, sysBase, 0, BASENAME)
{
    AROS_LIBFUNC_INIT

    /* This function is single-threaded by exec by calling Forbid. */

    SysBase=sysBase;
    LIBBASE->ExecBase = sysBase;
    LIBBASE->SegList  = segList;
    
    
    /* We have to use a function to set the global var DOpusBase
       since it shadows the LIBBASE parameter (LIBBASE is a #define
       for DOpusBase
    */
    
    set_global_dopusbase(LIBBASE);

    /* ----------------- */
    
    LIBBASE->DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
    if (NULL == LIBBASE->DOSBase) {
    	kprintf("DOPUS COULD NOT OPEN DOS\n");
	return NULL;
    }
    
    DOSBase = LIBBASE->DOSBase;

    /* ----------------- */
    	
    LIBBASE->IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
    if (NULL == LIBBASE->IntuitionBase) {
    	kprintf("DOPUS COULD NOT OPEN INTUITION\n");
	return NULL;
    }
    
    IntuitionBase = LIBBASE->IntuitionBase;

    /* ----------------- */

    LIBBASE->GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    if (NULL == LIBBASE->GfxBase) {
    	kprintf("DOPUS COULD NOT OPEN GFX\n");
	return NULL;
    }
    
    GfxBase = LIBBASE->GfxBase;
    
    /* ----------------- */

    LIBBASE->LayersBase = (struct LayersBase *)OpenLibrary("layers.library", 0);
    if (NULL == LIBBASE->LayersBase) {
    	kprintf("DOPUS COULD NOT OPEN LAYERS\n");
	return NULL;
    }
    
    LayersBase = (struct Library *)LIBBASE->LayersBase;
    
    /* ----------------- */
    
//    init_dopus_library();

    
    LIBBASE->pdb_cycletop = (PLANEPTR)&pdb_cycletop;
    LIBBASE->pdb_cyclebot = (PLANEPTR)&pdb_cyclebot;
kprintf("DOPUS_INIT: ct: %p, cb: %p\n"
    , LIBBASE->pdb_cycletop
    , LIBBASE->pdb_cyclebot
);
    LIBBASE->pdb_check = (PLANEPTR)&pdb_check;
    
    LIBBASE->pdb_Flags = 0;

    /* You would return NULL here if the init failed. */
    return LIBBASE;
    AROS_LIBFUNC_EXIT
}



AROS_LH1(struct DOpusBase *, open,
    AROS_LHA(ULONG, version, D0),
    struct DOpusBase *, LIBBASE, 1, BASENAME)
{
    AROS_LIBFUNC_INIT
    /*
	This function is single-threaded by exec by calling Forbid.
	If you break the Forbid() another task may enter this function
	at the same time. Take care.
    */


    /* Keep compiler happy */
    version=0;

    D(bug("Inside openfunc\n"));


    /* ------------------------- */


    /* I have one more opener. */
    LIBBASE->LibNode.lib_OpenCnt++;
    LIBBASE->LibNode.lib_Flags&=~LIBF_DELEXP;

    /* You would return NULL if the open failed. */
    return LIBBASE;
    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, close, struct DOpusBase *, LIBBASE, 2, BASENAME)
{
    AROS_LIBFUNC_INIT
    /*
	This function is single-threaded by exec by calling Forbid.
	If you break the Forbid() another task may enter this function
	at the same time. Take care.
    */

    /* I have one fewer opener. */
    if(!--LIBBASE->LibNode.lib_OpenCnt)
    {

	/* Delayed expunge pending? */
	if(LIBBASE->LibNode.lib_Flags&LIBF_DELEXP)
	    /* Then expunge the library */
	    return expunge();
    }
    return 0;
    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, expunge, struct DOpusBase *, LIBBASE, 3, BASENAME)
{
    AROS_LIBFUNC_INIT

    BPTR ret;
    /*
	This function is single-threaded by exec by calling Forbid.
	Never break the Forbid() or strange things might happen.
    */

    /* Test for openers. */
    if(LIBBASE->LibNode.lib_OpenCnt)
    {
	/* Set the delayed expunge flag and return. */
	LIBBASE->LibNode.lib_Flags|=LIBF_DELEXP;
	return 0;
    }

    /* Get rid of the library. Remove it from the list. */
    Remove(&LIBBASE->LibNode.lib_Node);

    /* Get returncode here - FreeMem() will destroy the field. */
    ret=LIBBASE->SegList;
    
    CloseLibrary((struct Library *)LIBBASE->DOSBase);
    CloseLibrary((struct Library *)LIBBASE->IntuitionBase);
    CloseLibrary((struct Library *)LIBBASE->GfxBase);
    CloseLibrary((struct Library *)LIBBASE->LayersBase);

    /* Free the memory. */
    FreeMem((char *)LIBBASE-LIBBASE->LibNode.lib_NegSize,
	LIBBASE->LibNode.lib_NegSize+LIBBASE->LibNode.lib_PosSize);

    return ret;
    AROS_LIBFUNC_EXIT
}

AROS_LH0I(int, null, struct DopusBase *, LIBBASE, 4, BASENAME)
{
    AROS_LIBFUNC_INIT
    return 0;
    AROS_LIBFUNC_EXIT
}

