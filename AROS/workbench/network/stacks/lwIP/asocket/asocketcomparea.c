/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

/*****************************************************************************

    NAME */

        #include <proto/asocket.h>

        AROS_LH2(BOOL, ASocketCompareA,

/*  SYNOPSIS */
        AROS_LHA(APTR, s, A0),
        AROS_LHA(struct TagItem *, pattern, A1),

/*  LOCATION */
        struct Library *, ASocketBase, 18, ASocket)

/*  FUNCTION
 
        Determine if an ASocket matches the descrition in 'tags'.

    INPUTS

        as      - Pointer to an existing ASocket handle

        pattern - Tags list to compare against

        All readable AS_TAG_* tags can be used to compare against.
        Unknown tags will be ignored.

    RESULT

        TRUE if the socket matches the tag list.

        FALSE if the socket does not match.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

        ASocketListObtain(), ASocketListRelease()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct ASocket *as = s;
    struct TagItem *tag, *tags = pattern;
    ULONG utmp;
    APTR atmp;
    BOOL btmp;
    struct sockaddr sa;
    struct ASocket_Address addr = { .asa_Address = &sa };
    LONG err;
    BOOL cmp = TRUE;

    while (cmp && (tag = LibNextTagItem(&tags))) {
        switch (tag->ti_Tag) {
        /* LONG/ULONG tags */
        case AS_TAG_SOCKET_DOMAIN:
        case AS_TAG_SOCKET_TYPE:
        case AS_TAG_SOCKET_PROTOCOL:
        case AS_TAG_LISTEN_BACKLOG:
        case AS_TAG_IFACE_INDEX:
        case AS_TAG_IFACE_IFF_MASK:
        case AS_TAG_IFACE_METRIC:
        case AS_TAG_IFACE_MTU:
        case AS_TAG_STATUS_READABLE:
        case AS_TAG_NOTIFY_FD_MASK:
        case AS_TAG_NOTIFY_FD_ACTIVE:
            err = ASocketGet(as, tag->ti_Tag, &utmp, TAG_END);
            if (err == 0) {
                cmp = (utmp == (ULONG)tag->ti_Data) ? TRUE : FALSE;
            } else {
                cmp = FALSE;
            }
            break;
        /* BOOL tags */
        case AS_TAG_SOCKET_LISTEN:
            err = ASocketGet(as, tag->ti_Tag, &btmp, TAG_END);
            if (err == 0) {
                cmp = ((btmp ? TRUE : FALSE) == ((ULONG)tag->ti_Data ? TRUE : FALSE)) ? TRUE : FALSE;
            } else {
                cmp = FALSE;
            }
            break;
        /* APTR == APTR tags */
        case AS_TAG_NOTIFY_MSGPORT:
        case AS_TAG_NOTIFY_NAME:
            err = ASocketGet(as, tag->ti_Tag, &atmp, TAG_END);
            if (err == 0) {
                cmp = (atmp == (APTR)tag->ti_Data) ? TRUE : FALSE;
            } else {
                cmp = FALSE;
            }
            break;
        /* STRPTR tags */
        case AS_TAG_IFACE_NAME:
            err = ASocketGet(as, tag->ti_Tag, &atmp, TAG_END);
            if (err == 0) {
                cmp = (strcmp(atmp,(APTR)tag->ti_Data) == 0) ? TRUE : FALSE;
            } else {
                cmp = FALSE;
            }
            break;
        /* struct ASocket_Address * tags */
        case AS_TAG_SOCKET_ADDRESS:
        case AS_TAG_SOCKET_ENDPOINT:
        case AS_TAG_IFACE_ADDRESS:
        case AS_TAG_IFACE_ENDPOINT:
        case AS_TAG_IFACE_NETMASK:
        case AS_TAG_IFACE_BROADCAST:
            addr.asa_Length = sizeof(sa);
            err = ASocketGet(as, tag->ti_Tag, &addr, TAG_END);
            if (err == 0) {
                struct ASocket_Address *aptr = (struct ASocket_Address *)tag->ti_Data;
                cmp = ((addr.asa_Length == aptr->asa_Length) &&
                       (aptr->asa_Address != NULL) &&
                       (memcmp(addr.asa_Address, aptr->asa_Address, addr.asa_Length) == 0)) ?
                        TRUE : FALSE;
            } else {
                cmp = FALSE;
            }
            break;
        default:
            D(bug("%s: Unrecognized pattern tag 0x%08x\n", __func__, tag->ti_Tag));
            break;
        }
    }

    return cmp;

    AROS_LIBFUNC_EXIT
}

