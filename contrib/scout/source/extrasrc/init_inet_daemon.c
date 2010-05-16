#include <exec/types.h>
#include <dos/dosextens.h>

struct AddressAllocationMessage;

#define _LIBRARIES_BSDSOCKET_H
struct DaemonMessage
{
	struct Message	dm_Message;	/* Used by super-server; don't touch! */
	ULONG		dm_Pad1;	/* Used by super-server; don't touch! */
	ULONG		dm_Pad2;	/* Used by super-server; don't touch! */
	LONG		dm_ID;		/* Public socket identifier; this must
					   be passed to ObtainSocket() to
					   access the socket allocated for you */
	ULONG		dm_Pad3;	/* Used by super-server; don't touch! */
	UBYTE		dm_Family;	/* Socket family type */
	UBYTE		dm_Type;	/* Socket type */
};

struct AddressAllocationMessage;

#include <proto/socket.h>
#include <proto/exec.h>

#include <stdlib.h>
#include <errno.h>

int init_inet_daemon(void)
{
  struct Process *me = (struct Process *)FindTask(NULL);
  struct DaemonMessage *dm = (struct DaemonMessage *)me->pr_ExitData;

  if (dm == NULL) {
    /*
     * No DaemonMessage, return error code
     */
    return -1;
  }

  /*
   * Obtain the server socket
   */
  return ObtainSocket(dm->dm_ID, dm->dm_Family, dm->dm_Type, 0);
}
