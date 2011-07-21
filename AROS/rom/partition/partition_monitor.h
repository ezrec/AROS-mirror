/*
    Copyright © 2001-2011, The AROS Development Team. All rights reserved.
   $Id$

   Desc: Internal definitions for partition.library
   Lang: english
*/
#ifndef PARTITION_MONITOR_H
#define PARTITION_MONITOR_H

#include "partition_intern.h"

struct Task *Partition_Monitor(struct PartitionDeviceMap *pm, struct PartitionBase *PartitionBase);

#endif /* PARTITION_MONITOR_H */
