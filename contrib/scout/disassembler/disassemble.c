#include <aros/debug.h>
#include <aros/asmcall.h>
#include <aros/libcall.h>
#include <clib/alib_protos.h>
#include <libraries/disassembler.h>
#include <proto/exec.h>

#include <setjmp.h>
#include <stdarg.h>

#include "dis-asm.h"
#include "library.h"
#include "support.h"
#include "bfd.h"

#if __WORDSIZE == 64
#define ADDRESS_FORMAT "%016lx%lc "
#else
#define ADDRESS_FORMAT "%08lx%lc "
#endif

#define BUFFER_SIZE 64

struct StringBuffer
{
    int pos;
    char buf[BUFFER_SIZE];
};

struct OutputContext
{
    UWORD cnt;
    struct DisData *ds;
};

AROS_UFH2(static void, dsPutCh,
	  AROS_UFHA(UBYTE, c, D0),
	  AROS_UFHA(struct OutputContext *, ctx, A3))
{
    AROS_USERFUNC_INIT

    if (ctx->ds->ds_Truncate) {
        if ((c == '\n') || (!c))
	    ctx->cnt = 0;
	else {
	    if (ctx->cnt == ctx->ds->ds_Truncate)
	        return;
	    ctx->cnt++;
	}
    }

    AROS_UFC3(void, ctx->ds->ds_PutProc,
	      AROS_UFCA(UBYTE, c, D0),
	      AROS_UFCA(APTR, ctx->ds->ds_UserData, A3),
	      AROS_UFCA(APTR, ctx->ds->ds_UserBase, A4));

    AROS_USERFUNC_EXIT
}

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

AROS_LH1(APTR, Disassemble,
	 AROS_LHA(struct DisData *, ds, A0),
	 struct DisasmBase *, DisassemblerBase, 5, Disassembler)
{
    AROS_LIBFUNC_INIT

    /* Tons of things on stack... However we have to be able to run inside interrupts,
       so there's no other way. Perhaps we should check for stack size? */
    bfd abfd;
    disassembler_ftype disasm;
    disassemble_info dinfo;
    struct StringBuffer sbuf;
    unsigned char *addr;
    int len, i;
    struct OutputContext ctx = {
	0,
	ds
    };
    struct AbortContext abort_context;
    struct Task *me;
    APTR OldUserData;

    D(bug("[Disassembler] Disassemble() from %p to %p\n", ds->ds_From, ds->ds_UpTo));

    init_disassemble_info(&dinfo, &sbuf, sbprintf);
    set_default_machine(&abfd, &dinfo);
    set_memory_bounds(&dinfo, ds->ds_From);

    disasm = disassembler(&abfd);
    D(bug("[Disassembler] Obtained disassembler function 0x%p\n", disasm));
    if (!disasm) {
        NewRawDoFmt("%s\n", dsPutCh, ds, "Error: unsupported architecture");
	return ds->ds_From;
    }

    addr = ds->ds_From;
    me = FindTask(NULL);
    OldUserData = me->tc_UserData;
    me->tc_UserData = &abort_context;

    if (!setjmp(abort_context.buf)) {
	for (; addr <= (unsigned char *)ds->ds_UpTo; addr += len) {
	    NewRawDoFmt(ADDRESS_FORMAT, dsPutCh, &ctx, addr, (addr == ds->ds_PC) ? '*' : ' ');
	    sbuf.pos = 0;

	    /* Libopcode is not reentrant, we use a semaphore until we find a better solution */
	    ObtainSemaphore(&DisassemblerBase->sem);
            len = disasm((IPTR)addr, &dinfo);
	    ReleaseSemaphore(&DisassemblerBase->sem);

	    for (i = 0; i < len; i++)
	        NewRawDoFmt("%02x ", dsPutCh, &ctx, addr[i]);
	    for (; i < dinfo.bytes_per_line; i++)
		RawDoFmt("    ", NULL, dsPutCh, &ctx);
	    NewRawDoFmt("%s\n", dsPutCh, &ctx, sbuf.buf);
	}
    } else {
        ReleaseSemaphore(&DisassemblerBase->sem);
	NewRawDoFmt("\nInternal disassembler error!!!\n"
		    "abort() in file %s on line %u\n"
		    "Please contact developers.\n",
		    dsPutCh, &ctx, abort_context.file, abort_context.line);
    }

    me->tc_UserData = OldUserData;
    return addr;

    AROS_LIBFUNC_EXIT
}
