#include <aros/debug.h>
#include <aros/libcall.h>
#include <libraries/disassembler.h>

AROS_LH3(APTR, FindStartPosition,
	 AROS_LHA(APTR, startpc, A0),
	 AROS_LHA(UWORD, min, D0),
	 AROS_LHA(UWORD, max, D1),
	 struct DisassemblerBase *, DisassemblerBase, 12, Disassembler)
{
    AROS_LIBFUNC_INIT

    D(bug("[Disassembler] FindStartPosition(0x%p)\n", startpc));

    return startpc;

    AROS_LIBFUNC_EXIT
}
