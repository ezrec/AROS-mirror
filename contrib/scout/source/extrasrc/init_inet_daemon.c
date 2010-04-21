#include <exec/types.h>
#include <dos/dosextens.h>

#include <proto/socket.h>
#include <proto/exec.h>

#include <stdlib.h>
#include <errno.h>
#include <inetd.h>

int
init_inet_daemon(void)
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
  return ObtainSocket(dm->dm_Id, dm->dm_Family, dm->dm_Type, 0);
}
