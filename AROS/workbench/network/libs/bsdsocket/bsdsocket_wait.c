/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#include "bsdsocket_intern.h"

LONG bsdsocket_wait_event(struct bsdsocketBase *SocketBase, APTR as, ULONG mask)
{
    LONG err;
    
    err = ASocketSet(as, AS_TAG_NOTIFY_MSGPORT, SocketBase->bsd_MsgPort,
                         AS_TAG_NOTIFY_FD_MASK, mask,
                         TAG_END);
    if (err == 0) {
        struct Message *msg = WaitPort(SocketBase->bsd_MsgPort);

        ASSERT(msg->mn_Node.ln_Type == NT_AS_NOTIFY);

        ASocketSet(as, AS_TAG_NOTIFY_MSGPORT, NULL,
                       AS_TAG_NOTIFY_FD_MASK, 0,
                       TAG_END);
        ReplyMsg(msg);
    }

    return err;
}


/* Wait for an ASocket_Msg to complete.
 * This is eerily similar to Exec/WaitIO()
 */
LONG bsdsocket_wait_msg(struct bsdsocketBase *SocketBase, struct ASocket_Msg *asmsg, size_t *sizep)
{
    struct Message *msg;

    WaitPort(asmsg->asm_Message.mn_ReplyPort);
    msg = GetMsg(asmsg->asm_Message.mn_ReplyPort);

    ASSERT(msg == (struct Message *)asmsg);

    if (asmsg->asm_Errno == 0 && sizep) {
        size_t size;
        int i;
        for (size = i = 0; i < asmsg->asm_MsgHdr->msg_iovlen; i++)
            size += asmsg->asm_MsgHdr->msg_iov[i].iov_len;
        *sizep = size;
    }

    return asmsg->asm_Errno;
}
