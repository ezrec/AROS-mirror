#ifndef PROTO_PARTITION_H
#define PROTO_PARTITION_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/partition/partition.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/partition_protos.h>

#if !defined(PartitionBase) && !defined(__NOLIBBASE__) && !defined(__PARTITION_NOLIBBASE__)
 #ifdef __PARTITION_STDLIBBASE__
  extern struct Library *PartitionBase;
 #else
  extern struct PartitionBase *PartitionBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(PARTITION_NOLIBDEFINES)
#   include <defines/partition.h>
#endif

#endif /* PROTO_PARTITION_H */
