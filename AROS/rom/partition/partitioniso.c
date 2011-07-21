/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

*/

#include <exec/memory.h>
#include <exec/types.h>
#include <libraries/partition.h>
#include <proto/exec.h>
#include <proto/partition.h>
#include <proto/utility.h>

#include "partition_types.h"
#include "partition_support.h"
#include "partitionmbr.h"
#include "platform.h"
#include "debug.h"

struct ISOData
{
    UBYTE type;
    ULONG block_no;	/* Start partition block	   */
    ULONG block_count;	/* Length of partition in blocks   */
};

static LONG PartitionISOCheckPartitionTable
    (
        struct Library *PartitionBase,
        struct PartitionHandle *root
    )
{
UBYTE i;
UBYTE const magic[8] = "\001CD001\001";
UBYTE *block;

    /* Since this is a 'whole disk' partition, we need
     * this check here to prevent recursion
     */
    if (root->de.de_DosType == AROS_MAKE_ID('C','D','V','D'))
        return FALSE;

    block = AllocMem(root->de.de_SizeBlock << 2, MEMF_PUBLIC);
    if (block == NULL)
        return FALSE;

    for (i = 0; i < 100; i++) {
        if (readBlock(PartitionBase, root, i, block) != 0)
            break;

        if (memcmp(block, magic, sizeof(magic)) == 0)
            return TRUE;
    }

    D(bug("ISO: Not an ISO disk\n"));
    return FALSE;
}

static LONG PartitionISOOpenPartitionTable
    (
        struct Library *PartitionBase,
        struct PartitionHandle *root
    )
{
struct PartitionHandle *ph;

    /* Map type ID to a DOSType */
    ph = AllocMem(sizeof(*ph), MEMF_PUBLIC);
    if (ph == NULL)
        return 1;

    initPartitionHandle(root, ph, 0, root->dg.dg_TotalSectors);

    ph->ln.ln_Name = "ISO9660";
    ph->ln.ln_Pri  = 0;

    setDosType(&ph->de, AROS_MAKE_ID('C','D','V','D'));
    ph->de.de_BootPri = 10;

    Enqueue(&root->table->list, &ph->ln);

    return 0;
}

static void PartitionISOClosePartitionTable
    (
        struct Library *PartitionBase,
        struct PartitionHandle *root
    )
{
    struct PartitionHandle *ph;

    while ((ph = (struct PartitionHandle *)RemTail(&root->table->list)))
        FreeMem(ph, sizeof(struct PartitionHandle));
}

static LONG PartitionISOWritePartitionTable
    (
        struct Library *PartitionBase,
        struct PartitionHandle *root
    )
{
    return FALSE;
}

static LONG PartitionISOCreatePartitionTable
    (
        struct Library *PartitionBase,
        struct PartitionHandle *ph
    )
{
    return FALSE;
}

static struct PartitionHandle *PartitionISOAddPartition(struct Library *PartitionBase, struct PartitionHandle *root, struct TagItem *taglist)
{
    return NULL;
}

static void PartitionISODeletePartition
    (
        struct Library *PartitionBase,
        struct PartitionHandle *ph
    )
{
}

static LONG PartitionISOGetPartitionTableAttr(struct Library *PartitionBase, struct PartitionHandle *root, struct TagItem *tag)
{
    return FALSE;
}

static LONG PartitionISOGetPartitionAttr(struct Library *PartitionBase, struct PartitionHandle *ph, struct TagItem *tag)
{
    return FALSE;
}

static LONG PartitionISOSetPartitionAttrs(struct Library *PartitionBase, struct PartitionHandle *ph, const struct TagItem *taglist)
{
    return FALSE;
}

static const struct PartitionAttribute PartitionISOPartitionTableAttrs[]=
{
    {TAG_DONE,           0}
};

static const struct PartitionAttribute PartitionISOPartitionAttrs[]=
{
    {PT_GEOMETRY,  PLAM_READ},
    {PT_TYPE,      PLAM_READ},
    {PT_DOSENVEC,  PLAM_READ},
    {PT_POSITION,  PLAM_READ},
    {PT_LEADIN,    PLAM_READ},
    {PT_STARTBLOCK, PLAM_READ},
    {PT_ENDBLOCK,   PLAM_READ},
    {TAG_DONE, 0}
};

static ULONG PartitionISODestroyPartitionTable
    (
        struct Library *PartitionBase,
        struct PartitionHandle *root
    )
{
    return 0;
}

const struct PTFunctionTable PartitionISO =
{
    PHPTT_ISO,
    "ISO9660",
    PartitionISOCheckPartitionTable,
    PartitionISOOpenPartitionTable,
    PartitionISOClosePartitionTable,
    PartitionISOWritePartitionTable,
    PartitionISOCreatePartitionTable,
    PartitionISOAddPartition,
    PartitionISODeletePartition,
    PartitionISOGetPartitionTableAttr,
    NULL,
    PartitionISOGetPartitionAttr,
    PartitionISOSetPartitionAttrs,
    PartitionISOPartitionTableAttrs,
    PartitionISOPartitionAttrs,
    PartitionISODestroyPartitionTable,
    NULL
};
