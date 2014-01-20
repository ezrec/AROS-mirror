/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef ASOCKET_H
#define ASOCKET_H

#include <exec/types.h>
#include <exec/ports.h>

#include <sys/socket.h>

#define AS_EOK      0
#define AS_ENOMEM   ENOMEM

/* Modes for ASocketShutdown() use the SHUT_* flags in <sys/socket.h> */

struct ASocket_Address {
    ULONG  asa_Length;
    APTR   asa_Address;
};

/* Structure for the ASocket_SendMsg() ASocket_RecvMsg() APIs
 *
 * The user must set asm_Message.mn_ReplyPort and asm_MsgHdr;
 *
 * ASocketBase will set up the rest of the asm_Message for you,
 * notably asm_Message.mn_Node.ln_Type will be NT_MESSAGE
 * while the packet is being processed, and NT_AS_MSG once
 * it has been completed.
 */
struct ASocket_Msg {
    struct Message   asm_Message;       /* mn_ReplyPort must be set */
    struct msghdr   *asm_MsgHdr;        /* defined in <sys/socket.h> */
    LONG   asm_Errno;                   /* Error, if non-zero */
};

/* Value of ASocket_Msg.asp_Message.mn_Node.ln_Type */
#define NT_AS_MSG_SEND      (NT_USER - 0)   /* Processed msg */
#define NT_AS_MSG_RECV      (NT_USER - 1)   /* Processed msg */

/* Structure for getting alerted on ASocket handles.
 * This structure is owned by the ASocketBase.
 *
 * NOTE: You must 'ReplyMsg()' this notification after
 *       acting upon it!
 */
struct ASocket_Notify {
    struct Message asn_Message;
    APTR  asn_ASocket;          /* ASocket that changed */
    ULONG asn_Events;           /* Mask of all current events */
    ULONG asn_NotifyEvents;     /* Mask of events requested */
};

/* Value of ASocket_Notify.asp_Message.mn_Node.ln_Type */
#define NT_AS_NOTIFY       (NT_USER - 10)

/* ASocketGet/ASocketSet tags
 * [..G] = Read-only values
 * [I.G] = Required on ASocketNew(), and cannot be set afterwards
 * [isG] = On ASocketNew() or ASocketGet(), and can set by ASocketSet() ONCE!
 * [iSG] = Can be called by ASocketNew(), ASocketGet(), or ASocketSet()
 */

#define AS_TAGM____G        0x0
#define AS_TAGM__I_G        0x1
#define AS_TAGM__IsG        0x2
#define AS_TAGM__ISG        0x3

#define AS_TAGF_COMPLETE    (1UL << 30)

#define AS_TAG(mode, index, type)  (TAG_USER | (sizeof(type)<<16) | ((mode) << 12) | index)

#define   AS_TAG_SOCKET_DOMAIN          AS_TAG(AS_TAGM__I_G, 0x00, LONG)  /* Address Domain */
#define   AS_TAG_SOCKET_TYPE            AS_TAG(AS_TAGM__I_G, 0x01, LONG)  /* Socket type */
#define   AS_TAG_SOCKET_PROTOCOL        AS_TAG(AS_TAGM__I_G, 0x02, LONG)  /* Protocol */
#define   AS_TAG_SOCKET_ADDRESS         AS_TAG(AS_TAGM__IsG, 0x03, struct ASocket_Address *)  /* Socket address */
#define   AS_TAG_SOCKET_ENDPOINT        AS_TAG(AS_TAGM__IsG, 0x04, struct ASocket_Address *)  /* Socket endpoint */
#define   AS_TAG_SOCKET_LISTEN          AS_TAG(AS_TAGM__IsG, 0x05, BOOL)     /* Socket is listening */
#define   AS_TAG_LISTEN_BACKLOG         AS_TAG(AS_TAGM__IsG, 0x08, LONG)     /* Socket listen backlog */
#define   AS_TAG_IFACE_IFF_MASK         AS_TAG(AS_TAGM__ISG, 0x10, ULONG)    /* See IFF_* flag mask */
#define   AS_TAG_IFACE_METRIC           AS_TAG(AS_TAGM___SG, 0x11, LONG)     /* Metric */
#define   AS_TAG_IFACE_INDEX            AS_TAG(AS_TAGM__I_G, 0x12, LONG)     /* Interface number */
#define   AS_TAG_IFACE_NAME             AS_TAG(AS_TAGM__I_G, 0x13, STRPTR)   /* Name of interface */
#define   AS_TAG_STATUS_READABLE        AS_TAG(AS_TAGM____G, 0x20, ULONG)    /* Number of bytes available to read */
#define   AS_TAG_NOTIFY_MSGPORT         AS_TAG(AS_TAGM__ISG, 0x40, struct MsgPort *)   /* Notify target */
#define   AS_TAG_NOTIFY_FD_MASK         AS_TAG(AS_TAGM__ISG, 0x41, ULONG)    /* See FD_* flag mask */
#define   AS_TAG_NOTIFY_FD_ACTIVE       AS_TAG(AS_TAGM____G, 0x42, ULONG)    /* See FD_* flag mask */
#define   AS_TAG_NOTIFY_NAME            AS_TAG(AS_TAGM__ISG, 0x43, APTR)     /* Private data for use by the Notify MsgPort, stored in ASocket_Notify.asm_Message.mn_Node.ln_Name */

#endif /* ASOCKET_H */
