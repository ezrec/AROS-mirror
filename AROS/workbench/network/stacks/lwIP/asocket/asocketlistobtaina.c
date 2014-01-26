/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

#include <sys/ioctl.h>
#include <net/if.h>

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH1(struct List *, ASocketListObtainA,

/*  SYNOPSIS */
        AROS_LHA(struct TagItem *, pattern, A0),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 8, ASocket)

/*  FUNCTION
 
        Obtain a struct List * that refers to a list of struct Node *s
        that can be passed like ASocket handles to ASocketGet().

        The ASockets in this list cannot be used for sending or
        receiving data.

    INPUTS

        pattern - Tags that specify the sockets of interest.
                  A NULL pattern will return an empty list.

        Valid tags for the pattern include:

        AS_TAG_SOCKET_DOMAIN        ULONG
            Address domain - see the <sys/socket.h> AF_* constants.

        AS_TAG_SOCKET_TYPE          ULONG
            Socket type - see the SOCK_* type constants in <sys/socket.h>
                        
        AS_TAG_SOCKET_PROTOCOL      ULONG
            Socket protocol - see the PF_* constants in <sys/socket.h>

        AS_TAG_SOCKET_ADDRESS       struct ASocket_Address *
            Address the socket is bound to.

        AS_TAG_SOCKET_ENDPOINT      struct ASocket_Address *
            Address the socket is connected to.

        AS_TAG_SOCKET_LISTEN        BOOL
            Sockets that are listening for connections

        AS_TAG_LISTEN_BACKLOG       ULONG
            A specific size of a listening socket's backlog
            Only valid if AS_TAG_SOCKET_LISTEN = TRUE in the pattern.

        AS_TAG_NOTIFY_MSGPORT       struct MsgPort *
            MsgPort that receives the struct ASocket_Notify messages.

        AS_TAG_NOTIFY_FD_MASK       ULONG
            Mask of notify events - see FD_* in <sys/socket.h>

        AS_TAG_NOTIFY_NAME          APTR
            Value of the asn_Message.mn_Node.ln_Name field of
            the ASocket_Notify messages sent to the AS_TAG_NOTIFY_MSGPORT.

        Tags specific to AF_LINK/SOCK_RAW/0 sockets:

        AS_TAG_IFACE_INDEX          ULONG
            Interface id to attach to (1...N)

        AS_TAG_IFACE_NAME           CONST_STRPTR
            Interface name to attach to (ie "lo", "eth0", etc)

        AS_TAG_IFACE_IFF_MASK       ULONG
            Mask of interface flags - see IFF_* in <net/if.h>

        AS_TAG_IFACE_METRIC         ULONG
            Metric of the interface

    RESULT

        NULL on failure (only ENOMEM)

        Otherwise, returns a 'struct List *' owned by ASocketBase
        that the caller can inspect, and call ASocketGet() on its
        list items.

        Call ASocketListRelease() to return the List to ASocketBase.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

        ASocketListRelease()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct List *list;
    struct ASocket *as;
    struct bsd *bsd = ASocketBase->ab_bsd;

    D(bug("%s: pattern=%p\n", __func__, pattern));

    list = AllocVec(sizeof(*list), MEMF_ANY);
    if (list == NULL)
        return NULL;

    NEWLIST(list);

    /* On SOCK_RAW, get a list of all known interfaces
     * for the specific address family domain and protocol
     */
    if (LibGetTagData(AS_TAG_SOCKET_TYPE, ~0, pattern) == SOCK_RAW) {
        struct bsd_sock *s;
        int domain = LibGetTagData(AS_TAG_SOCKET_DOMAIN, AF_UNSPEC, pattern);
        int protocol = LibGetTagData(AS_TAG_SOCKET_PROTOCOL, 0, pattern);
        int err;
        CONST_STRPTR name = (CONST_STRPTR)LibGetTagData(AS_TAG_IFACE_NAME, (IPTR)NULL, pattern);

        /* Compatability cuft - we only care about getting the
         * interface names. The 'bsd_*' layer is intended to
         * be used to map to existing BSD-sockets style stacks,
         * so we use SIOCGIFCONF to get the interface names.
         */
        err = bsd_socket(bsd, &s, domain, SOCK_RAW, protocol);
        if (err == 0) {
            struct ifconf ifc;
            struct ifreq  ifr_buff[64];
            ifc.ifc_req = &ifr_buff[0];
            ifc.ifc_len = sizeof(ifr_buff);

            /* Short-circuit if we already have a name */
            if (name != NULL) {
                ifc.ifc_len = sizeof(ifr_buff[0]);
                strncpy(ifr_buff[0].ifr_name, name, sizeof(ifr_buff[0].ifr_name));
                err = bsd_ioctl(bsd, s, SIOCGIFINDEX, &ifr_buff[0]);
            } else {
                err = bsd_ioctl(bsd, s, SIOCGIFCONF, &ifc);
            }
            if (err == 0) {
                int i, end;
                struct ASocket atmp = {};

                /* The 'atmp' on-stack ASocket is only
                 * used for passing to ASocketCompare() -
                 * it will NEVER be added to a list.
                 */
                atmp.as_Node.ln_Type = NT_AS_OBTAINED;
                atmp.as_Node.ln_Name = "ASocket";
                atmp.as_Socket.domain = domain;
                atmp.as_Socket.type = SOCK_RAW;
                atmp.as_Socket.protocol = protocol;

                end = ifc.ifc_len / sizeof(ifr_buff[0]);
                if (end > ARRAY_SIZE(ifr_buff))
                    end = ARRAY_SIZE(ifr_buff);
                for (i = 0; i < end; i++) {
                    struct bsd_sock *ds;
                    D(bug("%s: ifname[%d]=\"%s\"\n", __func__, i, ifr_buff[i].ifr_name));
                    err = bsd_socket_dup(bsd, s, &ds);
                    if (err == 0) {
                        atmp.as_bsd = ds;

                        /* Cache the interface's name and index */
                        CopyMem(ifr_buff[i].ifr_name, atmp.as_IFace.name, IFNAMSIZ);
                        err = bsd_ioctl(bsd, s, SIOCGIFINDEX, &ifr_buff[i]);
                        if (err == 0) {
                            atmp.as_IFace.index = ifr_buff[i].ifr_index;
                            if (ASocketCompareA(&atmp, pattern)) {
                                struct ASocket *as;
                                if ((as = AllocVec(sizeof(*as), MEMF_ANY | MEMF_CLEAR))) {
                                    CopyMem(&atmp, as, sizeof(*as));
                                    ADDTAIL(list, as);
                                }
                            }
                        }
                        bsd_close(bsd, ds);
                    }
                }
            }
            bsd_close(bsd, s);
        }
    }

    /* Then, look for matching opened sockets */
    ObtainSemaphore(&ASocketBase->ab_Lock);
    ForeachNode(&ASocketBase->ab_SocketList, as) {
        struct ASocket *new_as;
        int err;

        /* Test to see if the socket matches the tags
         */
        if (!ASocketCompareA(as, pattern))
            continue;

        /* We add these sockets as 'detacted' socket,
         * with an as_Node.ln_Type = NT_AS_OBTAINED,
         * to ensure that only ASocketGet() can be
         * performed on them.
         */
        new_as = AllocVec(sizeof(*new_as), MEMF_ANY | MEMF_CLEAR);
        if (new_as) {
            CopyMem(as, new_as, sizeof(*as));
            err = bsd_socket_dup(ASocketBase->ab_bsd, as->as_bsd, &new_as->as_bsd);
            if (err == 0) {
                new_as->as_Node.ln_Type = NT_AS_OBTAINED;
                ADDTAIL(list, new_as);
            } else {
                FreeVec(new_as);
            }
        }
    }
    ReleaseSemaphore(&ASocketBase->ab_Lock);

    /* We have all the matching sockets */

    return list;

    AROS_LIBFUNC_EXIT
}
