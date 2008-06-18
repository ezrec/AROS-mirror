#define DEBUG 1
#include <aros/debug.h>
#include <aros/macros.h>
#include <asm/amcc440.h>
#include <asm/io.h>
#include <inttypes.h>

#include <proto/kernel.h>

#include "emac.h"
#include LC_LIBDEFS_FILE

static const struct UnitInfo {
    uint8_t     ui_IrqNum;
} EMAC_Units[2] = {
        { INTR_ETH0 },
        { INTR_ETH1 },
};

struct EMACUnit *CreateUnit(struct EMACBase *EMACBase, uint8_t num)
{
    void *KernelBase = OpenResource("kernel.resource");

    D(bug("[EMAC ] CreateUnit(%d)\n", num));
    
    struct EMACUnit *unit = AllocPooled(EMACBase->emb_Pool, sizeof(struct EMACUnit));
    
    if (unit)
    {
        InitSemaphore(&unit->eu_Lock);
        
        NEWLIST(&unit->eu_Openers);
        NEWLIST(&unit->eu_MulticastRanges);
        NEWLIST(&unit->eu_TypeTrackers);
        
        unit->eu_UnitNum = num;
        unit->eu_IRQHandler = KrnAddIRQHandler(EMAC_Units[num].ui_IrqNum, EMACIRQHandler, EMACBase, unit);
    }
    
    return unit;
}

static struct AddressRange *FindMulticastRange(struct EMACBase *EMACBase, struct EMACUnit *unit,
   ULONG lower_bound_left, UWORD lower_bound_right, ULONG upper_bound_left, UWORD upper_bound_right)
{
    struct AddressRange *range, *tail;
    BOOL found = FALSE;

    range = (APTR)unit->eu_MulticastRanges.mlh_Head;
    tail = (APTR)&unit->eu_MulticastRanges.mlh_Tail;

    while((range != tail) && !found)
    {
        if((lower_bound_left == range->lower_bound_left) &&
            (lower_bound_right == range->lower_bound_right) &&
            (upper_bound_left == range->upper_bound_left) &&
            (upper_bound_right == range->upper_bound_right))
            found = TRUE;
        else
            range = (APTR)range->node.mln_Succ;
    }

    if(!found)
        range = NULL;

    return range;
}

BOOL AddMulticastRange(struct EMACBase *EMACBase, struct EMACUnit *unit, const UBYTE *lower_bound, const UBYTE *upper_bound)
{
    struct AddressRange *range;
    ULONG lower_bound_left, upper_bound_left;
    UWORD lower_bound_right, upper_bound_right;

    lower_bound_left = AROS_BE2LONG(*((ULONG *)lower_bound));
    lower_bound_right = AROS_BE2WORD(*((UWORD *)(lower_bound + 4)));
    upper_bound_left = AROS_BE2LONG(*((ULONG *)upper_bound));
    upper_bound_right = AROS_BE2WORD(*((UWORD *)(upper_bound + 4)));

    range = FindMulticastRange(EMACBase, unit, lower_bound_left, lower_bound_right,
        upper_bound_left, upper_bound_right);

    if(range != NULL)
        range->add_count++;
    else
    {
        range = AllocPooled(EMACBase->emb_Pool, sizeof(struct AddressRange));
        if(range != NULL)
        {
            range->lower_bound_left = lower_bound_left;
            range->lower_bound_right = lower_bound_right;
            range->upper_bound_left = upper_bound_left;
            range->upper_bound_right = upper_bound_right;
            range->add_count = 1;

            Disable();
            AddTail((APTR)&unit->eu_MulticastRanges, (APTR)range);
            Enable();

            if (unit->eu_RangeCount++ == 0)
            {
                unit->eu_Flags |= IFF_ALLMULTI;
                unit->set_multicast(unit);
            }
        }
    }

    return range != NULL;
}

BOOL RemMulticastRange(struct EMACBase *EMACBase, struct EMACUnit *unit, const UBYTE *lower_bound, const UBYTE *upper_bound)
{
    struct AddressRange *range;
    ULONG lower_bound_left, upper_bound_left;
    UWORD lower_bound_right, upper_bound_right;

    lower_bound_left = AROS_BE2LONG(*((ULONG *)lower_bound));
    lower_bound_right = AROS_BE2WORD(*((UWORD *)(lower_bound + 4)));
    upper_bound_left = AROS_BE2LONG(*((ULONG *)upper_bound));
    upper_bound_right = AROS_BE2WORD(*((UWORD *)(upper_bound + 4)));

    range = FindMulticastRange(EMACBase, unit, lower_bound_left, lower_bound_right,
        upper_bound_left, upper_bound_right);

    if(range != NULL)
    {
        if(--range->add_count == 0)
        {
            Disable();
            Remove((APTR)range);
            Enable();
            FreePooled(EMACBase->emb_Pool, range, sizeof(struct AddressRange));

            if (--unit->eu_RangeCount == 0)
            {
                unit->eu_Flags &= ~IFF_ALLMULTI;
                unit->set_multicast(unit);
            }
        }
    }
    return range != NULL;
}
