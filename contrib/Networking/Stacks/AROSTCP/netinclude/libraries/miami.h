#ifndef MIAMI_H
#define MIAMI_H

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <devices/trackdisk.h>
#include <dos/filehandler.h>
#include <utility/tagitem.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>
#include <exec/ports.h>
#include <libcore/base.h>

struct MiamiBase {
  struct Library Lib;
  struct Library *SocketBase;
  struct Library *UserGroupBase;
  APTR   res_state;
  APTR   NDB;
  struct SignalSemaphore *ndb_Lock;
  APTR   HostentNode;
  APTR   ProtoentNode;
  APTR   DynDB;
  ULONG  *ndb_Serial;
  UBYTE  DynNameServ_Locked;
  UBYTE  DynDomain_Locked;
};

#endif /* MIAMI_H */
