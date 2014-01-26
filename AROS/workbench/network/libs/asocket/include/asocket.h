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

/* Value of an ASocket node's ln_Type */
#define NT_AS_SOCKET       (NT_USER - 0)
/* Value of an ASocketObtainList node's ln_Type */
#define NT_AS_OBTAINED     (NT_USER - 1)

/* Value of ASocket_Msg.asp_Message.mn_Node.ln_Type */
#define NT_AS_MSG_SEND      (NT_USER - 10)   /* Processed msg */
#define NT_AS_MSG_RECV      (NT_USER - 11)   /* Processed msg */

/* Value of ASocket_Notify.asp_Message.mn_Node.ln_Type */
#define NT_AS_NOTIFY       (NT_USER - 20)

/* ASocketGet/ASocketSet tags
 * [..G] = Read-only values
 * [I.G] = Required on ASocketNew(), and cannot be set afterwards
 * [isG] = On ASocketNew() or ASocketGet(), and can set by ASocketSet() ONCE!
 * [iSG] = Can be called by ASocketNew(), ASocketGet(), or ASocketSet()
 */

#define AS_TAGF_COMPLETE    (1UL << 30)

#define AS_TAG(index, type)  (TAG_USER | (sizeof(type) << 16) | index)

#define   AS_TAG_SOCKET_DOMAIN          AS_TAG(0x0000, LONG)  /* Address Domain */
#define   AS_TAG_SOCKET_TYPE            AS_TAG(0x0001, LONG)  /* Socket type */
#define   AS_TAG_SOCKET_PROTOCOL        AS_TAG(0x0002, LONG)  /* Protocol */
#define   AS_TAG_SOCKET_ADDRESS         AS_TAG(0x0003, struct ASocket_Address *)  /* Socket address */
#define   AS_TAG_SOCKET_ENDPOINT        AS_TAG(0x0004, struct ASocket_Address *)  /* Socket endpoint */
#define   AS_TAG_SOCKET_LISTEN          AS_TAG(0x0005, BOOL)     /* Socket is listening */
#define   AS_TAG_LISTEN_BACKLOG         AS_TAG(0x0008, LONG)     /* Socket listen backlog */

/* Interface control */
#define   AS_TAG_IFACE_INDEX            AS_TAG(0x0100, LONG)     /* Interface number */
#define   AS_TAG_IFACE_NAME             AS_TAG(0x0101, STRPTR)   /* Name of interface */
#define   AS_TAG_IFACE_ADDRESS          AS_TAG(0x0102, struct ASocket_Address *)    /* Interface address */
#define   AS_TAG_IFACE_ENDPOINT         AS_TAG(0x0103, struct ASocket_Address *)    /* Point-To-Point destination address */
#define   AS_TAG_IFACE_NETMASK          AS_TAG(0x0104, struct ASocket_Address *)    /* Interface address */
#define   AS_TAG_IFACE_BROADCAST        AS_TAG(0x0105, struct ASocket_Address *)    /* Interface address */
#define   AS_TAG_IFACE_IFF_MASK         AS_TAG(0x0106, ULONG)    /* See IFF_* flag mask */
#define   AS_TAG_IFACE_METRIC           AS_TAG(0x0107, LONG)     /* Metric */

/* Socket status */
#define   AS_TAG_STATUS_READABLE        AS_TAG(0x0200, ULONG)    /* Number of bytes available to read */

/* Notification control */
#define   AS_TAG_NOTIFY_MSGPORT         AS_TAG(0x0400, struct MsgPort *)   /* Notify target */
#define   AS_TAG_NOTIFY_FD_MASK         AS_TAG(0x0401, ULONG)    /* See FD_* flag mask */
#define   AS_TAG_NOTIFY_FD_ACTIVE       AS_TAG(0x0402, ULONG)    /* See FD_* flag mask */
#define   AS_TAG_NOTIFY_NAME            AS_TAG(0x0403, APTR)     /* Private data for use by the Notify MsgPort, stored in ASocket_Notify.asm_Message.mn_Node.ln_Name */

#endif /* ASOCKET_H */
