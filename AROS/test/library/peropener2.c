#include <proto/peropener.h>
#include <aros/libcall.h>
#include <aros/symbolsets.h>
#define DEBUG 1
#include <aros/debug.h>
#include "peropener2_base.h"

SETRELLIBOFFSET(PeropenerBase, struct PerOpener2Base, PeropenerBase)

void pob2_settestvalue(int value)
{
    struct PerOpener2Base *base = (struct PerOpener2Base *)AROS_GET_LIBBASE;

    D(bug("pob2_settestvalue base: %x base2: %x\n", base, base->PeropenerBase));

    pob_settestvalue(value);
}

int pob2_gettestvalue(void)
{
    struct PerOpener2Base *base = (struct PerOpener2Base *)AROS_GET_LIBBASE;

    D(bug("pob_gettestvalue base: %x base2: %x\n", base, base->PeropenerBase));

    return pob_gettestvalue();
}
