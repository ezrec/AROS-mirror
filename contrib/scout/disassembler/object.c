#include <aros/asmcall.h>
#include <aros/debug.h>
#include <libraries/disassembler.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <setjmp.h>

#include "dis-asm.h"
#include "library.h"
#include "object.h"
#include "libc.h"
#include "object_intern.h"
#include "support.h"
#include "bfd.h"

#if __WORDSIZE == 64
#define ADDRESS_FORMAT "%016lx%lc "
#else
#define ADDRESS_FORMAT "%08lx%lc "
#endif

/* Our character output function. Handles truncation, using ds_reserved as a counter */
AROS_UFH2(static void, dsPutCh,
	  AROS_UFHA(UBYTE, c, D0),
	  AROS_UFHA(struct DisData *, ds, A3))
{
    AROS_USERFUNC_INIT

    if (ds->ds_Truncate) {
        if ((c == '\n') || (!c))
	    ds->ds_reserved = 0;
	else {
	    if (ds->ds_reserved == ds->ds_Truncate)
	        return;
	    ds->ds_reserved++;
	}
    }

    AROS_UFC3(void, ds->ds_PutProc,
	      AROS_UFCA(UBYTE, c, D0),
	      AROS_UFCA(APTR, ds->ds_UserData, A3),
	      AROS_UFCA(APTR, ds->ds_UserBase, A4));

    AROS_USERFUNC_EXIT
}

/* Print into StringBuffer, used for accumulating libopcodes output */
static int sbprintf(struct StringBuffer *sb, const char *fmt, ...)
{
    int l = BUFFER_SIZE - sb->pos;
    va_list ap;

    va_start(ap, fmt);
    l = vsnprintf(&sb->buf[sb->pos], l, fmt, ap);
    va_end(ap);
    sb->pos += l;

    return l;
}

/*
 * Disassembler object routines.
 *
 * libopcodes can do much more than simply disassemble code for one architecture. It can
 * deal with multiple architectures, perform symbol lookups, work with virtual buffers, etc.
 * In order to be able to utilize these possibilities, a new API is needed.
 *
 * A key concept of it is a disassembler object. From user's point of view an object is a blob
 * representing internal state of the disassembler. You create an object, use it as long as you
 * wish, then dispose it when it's no longer needed.
 *
 * Remember that objects are no way reentrant! You may not use the same object from within several
 * threads (processes).
 *
 * Supplied with tags, these functions could become a nice new library API, enabling the user to
 * specify for which CPU to disassemble, where to take the code (it is possible to read
 * a large file in pieces into a small buffer and disassemble it), etc.
 */

APTR CreateDisassemblerObject(bfd *abfd)
{
    APTR pool;
    struct DisassemblerObject *obj;
    disassembler_ftype disasm;
    
    disasm = disassembler(abfd);
    D(bug("[Disassembler] Obtained disassembler function 0x%p\n", disasm));
    if (!disasm)
        return NULL;

    /* MEMF_CLEAR here is a must! Our built-in libc relies on this! */
    pool = CreatePool(MEMF_ANY|MEMF_CLEAR, 4096, 4096);
    if (pool) {
        obj = AllocPooled(pool, sizeof(struct DisassemblerObject));
        if (obj) {
	    obj->libc_data.pool = pool;
            init_disassemble_info(&obj->dinfo, &obj->sbuf, sbprintf);
            set_machine_info(abfd, &obj->dinfo);
	    obj->disasm = disasm;
	    
	    return obj;
	}
	DeletePool(pool);
    }
    return NULL;
}

void DeleteDisassemblerObject(APTR ob)
{
    struct DisassemblerObject *obj = ob;

    /* An object itself is also allocated on its pool, so destroying the pool
       effectively destroys the object too */
    DeletePool(obj->libc_data.pool);
}

APTR DisassembleObject(APTR ob, struct DisData *ds, struct DisasmBase *DisassemblerBase)
{
    struct DisassemblerObject *obj = ob;
    unsigned char *addr = ds->ds_From;
    int len, i, ex;

    /* Set up memory bounds in order to prevent us
       from illegal memory access crash */
    set_memory_bounds(&obj->dinfo, ds->ds_From);

    /* libc magic. Too long to explain, but becomes quite easy when
       you read the source */
    libc_setcontext(&obj->libc_data);
    ex = setjmp(obj->libc_data.abort_buf);

    if (!ex) {
        /* We use ds_reserved as characters counter, so zero it */
	ds->ds_reserved = 0;

	/* Libopcodes disassembles one instruction per call. During  one call
	   it may call output function multiple times.
	   This is why we can't feed its output to RawDoFmt() directly. We have
	   to accumulate it in a buffer instead. When disassembler function returns
	   it tells us how many bytes were processed. So we are able to dump an instruction
	   in hex and only after this we fire our accumulated buffer */
	for (; addr <= (unsigned char *)ds->ds_UpTo; addr += len) {
	    obj->sbuf.pos = 0;

	    /* Libopcode is not reentrant and it uses static variables, so for now we use a semaphore.
	       Fortunately libopcodes is reexecutable (it does not rely on keeping static data accross
	       calls).
	       When baserel support is implemented for AROS gcc, we will be able to move libopcodes
	       data section into LibcData structure. */
	    ObtainSemaphore(&DisassemblerBase->sem);
            len = obj->disasm((IPTR)addr, &obj->dinfo);
	    ReleaseSemaphore(&DisassemblerBase->sem);

	    if (len == -1) {
	        RawDoFmt("\nFailed to disassemble\n"
			 "Disassembler function returned -1\n",
			 NULL, (VOID_FUNC)dsPutCh, ds);
		break;
	    }

	    for (;;) {
		int bpl = obj->dinfo.bytes_per_line;

		if (!bpl)
		    bpl = len;

		IPTR rdata1[] =
		{
		    (IPTR)addr,
		    (addr == ds->ds_PC) ? '*' : ' '
		};
		RawDoFmt(ADDRESS_FORMAT, rdata1, (VOID_FUNC)dsPutCh, ds);
	        for (i = 0; i < bpl;)
	        {
		    UWORD rdata3[] =
		    {
			*addr
		    };
		    RawDoFmt("%02x ", rdata3, (VOID_FUNC)dsPutCh, ds);
		    addr++;
		    i++;
		    if (--len == 0)
		        goto break_hexdump;
	        }
		RawDoFmt("\n", NULL, (VOID_FUNC)dsPutCh, ds);
	    }
break_hexdump:
	    for (; i < obj->dinfo.bytes_per_line; i++)
		RawDoFmt("   ", NULL, (VOID_FUNC)dsPutCh, ds);
	    IPTR rdata4[] =
	    {
		(IPTR)obj->sbuf.buf
	    };
	    RawDoFmt("%s\n", rdata4, (VOID_FUNC)dsPutCh, ds);
	    //NewRawDoFmt("%s\n", dsPutCh, ds, obj->sbuf.buf);
	}
    } else {
        ReleaseSemaphore(&DisassemblerBase->sem);
	RawDoFmt("\nInternal disassembler error!!!\n", NULL, (VOID_FUNC)dsPutCh, ds);
	switch (ex) {
	case EXCEPTION_ABORT:
	    {
		IPTR rdata2[] =
		{
		    (IPTR)obj->libc_data.exception_arg1,
		    obj->libc_data.exception_arg2
		};
		RawDoFmt("abort() in file %s on line %lu\nPlease contact developers.\n",
			 rdata2, (VOID_FUNC)dsPutCh, ds);
		//NewRawDoFmt("abort() in file %s on line %u\n"
		//"Please contact developers.\n",
		//dsPutCh, ds, obj->libc_data.exception_arg1, obj->libc_data.exception_arg2);
		break;
	    }
	case EXCEPTION_NO_MEMORY:
	    {
		IPTR rdata5[] =
		{
		    obj->libc_data.exception_arg2
		};
		RawDoFmt("Failed to allocate %lu bytes of memory.\n",
			 rdata5, (VOID_FUNC)dsPutCh, ds);
		//NewRawDoFmt("Failed to allocate %u bytes of memory.\n",
		//dsPutCh, ds, obj->libc_data.exception_arg2);
		break;
	    }
	}
    }

    libc_unsetcontext();
    
    return addr;
}
