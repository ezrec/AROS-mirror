#include <aros/libcall.h>
#include <libraries/disassembler.h>
#include <proto/exec.h>

#include "bfd.h"
#include "library.h"
#include "object.h"

/* This backwards compatibility call is easy. Just create a temporary object, use it, then delete it. */

AROS_LH1(APTR, Disassemble,
	 AROS_LHA(struct DisData *, ds, A0),
	 struct DisasmBase *, DisassemblerBase, 11, Disassembler)
{
    AROS_LIBFUNC_INIT

    APTR obj;
    APTR res;

    obj = CreateDisassemblerObject(&DisassemblerBase->default_bfd);
    if (!obj)
        return ds->ds_From;

    res = DisassembleObject(obj, ds, DisassemblerBase);

    DeleteDisassemblerObject(obj);    
    return res;

    AROS_LIBFUNC_EXIT
}
