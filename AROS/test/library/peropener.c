#include <aros/libcall.h>
#define DEBUG 1
#include <aros/debug.h>
#include "peropener_base.h"

void pob_settestvalue(int value)
{
    struct PerOpenerBase *base = (struct PerOpenerBase *)AROS_GET_LIBBASE;

    D(bug("pob_settestvalue base: %x\n", base));
    
    base->testvalue = value;
}

int pob_gettestvalue(void)
{
    struct PerOpenerBase *base = (struct PerOpenerBase *)AROS_GET_LIBBASE;

    D(bug("pob_gettestvalu base: %x\n", base));

    return base->testvalue;
}
