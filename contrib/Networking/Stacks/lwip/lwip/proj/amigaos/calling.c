/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <stdarg.h>

#include <exec/memory.h>
#include <proto/exec.h>

#include "calling.h"
#include "socket_intern.h"

int CallStackFunction(struct SocketBase_intern *SocketBase, int type, int argc, ...)
{
  int rc = -1;
#if 0
  struct MsgPort *msgport = CreateMsgPort();

  if (msgport)
  {
  	struct LibMsg *msg = (struct LibMsg*)AllocVec(sizeof(struct LibMsg),MEMF_CLEAR|MEMF_PUBLIC);
  	if (msg)
  	{
  		int i;
  		void **args = (void**)((&argc)+1);
  		struct LibMsg *replied_msg;

  		msg->msg.mn_ReplyPort = msgport;
  		msg->msg.mn_Length = sizeof(struct LibMsg);
      msg->type = type;
      msg->argc = argc;

      for (i=0;i<argc;i++)
				msg->args[i] = args[i];

      PutMsg(SocketBase->stack_port,(struct Message*)msg);
      WaitPort(msgport);

			while((replied_msg = (struct LibMsg*)GetMsg(msgport)))
			{
				if (replied_msg == msg)
				{
					rc = replied_msg->retval;
					FreeVec(msg);
				}
      }

  	}
  	DeleteMsgPort(msgport);
  }
#endif
  return rc;
}

