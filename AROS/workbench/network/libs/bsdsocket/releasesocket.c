/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(LONG, ReleaseSocket,

/*  SYNOPSIS */
        AROS_LHA(LONG, sd, D0),
        AROS_LHA(LONG, id, D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 25, BSDSocket)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct bsd_fd *fd, *fdnode;
    struct BSDSocketBase *BSDSocketBase = SocketBase->lib_BSDSocketBase;
    LONG err;

    fd = BSD_GET_FD(SocketBase, sd);

    /* Do some sanily checking on the id sent */
    ReleaseSemaphore(&BSDSocketBase->bs_Lock);

    if (id >= 0 && id <= 65535) {
        /* 'sharable' IDs */
    } else if (id == UNIQUE_ID) {
        /* Generate a new unique ID */
        if (IsListEmpty(&BSDSocketBase->bs_FDList)) {
            id = 65536;
        } else {
            /* The list is sorted by ID */
            struct bsd_fd *tmp = (struct bsd_fd *)GetTail(&BSDSocketBase->bs_FDList);
            id = tmp->id + 1;
            if (id < 0) {
                /* Darn. Worth a try. Looks like we wrapped MAXLONG
                 * Do a manual search.
                 */
                id = 65536;
                ForeachNode(&BSDSocketBase->bs_FDList, tmp) {
                    if (tmp->id < id)
                        continue;
                    if (tmp->id == id) {
                        id++;
                        continue;
                    }
                    if (id < 0) {
                        /* 2^32-65536 descriptors in use! */
                        ReleaseSemaphore(&BSDSocketBase->bs_Lock);
                        BSD_SET_ERRNO(SocketBase, ENFILE);
                        return -1;
                    }
                    /* Found one! */
                    break;
                }
            }
        }
    } else {
        /* Verify that the ID has not already been used */
        struct bsd_fd *tmp;

        ForeachNode(&BSDSocketBase->bs_FDList, tmp) {
            if (tmp->id == id) {
                ReleaseSemaphore(&BSDSocketBase->bs_Lock);
                BSD_SET_ERRNO(SocketBase, EEXIST);
                return -1;
            }
        }
    }

    fdnode = AllocVec(sizeof(*fdnode), MEMF_ANY);
    if (fdnode == NULL) {
        ReleaseSemaphore(&BSDSocketBase->bs_Lock);
        BSD_SET_ERRNO(SocketBase, ENOMEM);
        return -1;
    }

    err = BSD_FD_CALLBACK(SocketBase, sd, FDCB_FREE);
    if (err == 0) {
        struct bsd_fd *tmp;

        CopyMem(fd, fdnode, sizeof(*fd));
        fd->asocket = NULL;
        fdnode->id = id;

        /* Enqueue based on id.
         * Note the similarity with the implementation
         * of Exec/Enqueue
         */
        ForeachNode(&BSDSocketBase->bs_FDList, tmp) {
            if (fdnode->id > tmp->id)
                continue;
        }

        /* Insert into the ordered position 
         * NOTE: 'tmp' will *never* be NULL, due to how ForeachNode works.
         */
        fdnode->node.mln_Pred = tmp->node.mln_Pred;
        fdnode->node.mln_Succ = &tmp->node;
        tmp->node.mln_Pred->mln_Succ = &fdnode->node;
        tmp->node.mln_Pred = &fdnode->node;
    } else {
        FreeVec(fdnode);
    }
    ReleaseSemaphore(&BSDSocketBase->bs_Lock);

    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? 0 : -1;

    AROS_LIBFUNC_EXIT

} /* ReleaseSocket */
