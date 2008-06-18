#define DEBUG 1
#include <inttypes.h>
#include <aros/debug.h>
#include <asm/amcc440.h>
#include <asm/io.h>
#include "emac.h"

void EMACIRQHandler(struct EMACBase *EMACBase, struct EMACUnit *unit)
{
    if (unit)
    {
        D(bug("[EMAC%d] IRQ Handler\n", unit->eu_UnitNum));
    }
}
