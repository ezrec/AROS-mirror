#include <aros/symbolsets.h>
#include <proto/alib.h>
#include <proto/hostlib.h>

#include "bsdsocket_intern.h"

/* Handles signalling tasks when their socket comes up with an event */
static void BSDSocket_NotifyTask(struct BSDSocketBase *BSDSocketBase)
{
    BOOL dead = FALSE;

    D(bug("%s[%p]: Running\n", __func__, FindTask(NULL)));
    while (!dead) {
        struct Message *msg;

        WaitPort(BSDSocketBase->bs_MsgPort);
        msg = GetMsg(BSDSocketBase->bs_MsgPort);
        if (msg->mn_Node.ln_Type == NT_AS_NOTIFY) {
            struct bsd_fd *fd = (struct bsd_fd *)msg->mn_Node.ln_Name;

            if (fd->flags & O_ASYNC)
                Signal(fd->sigioTask, fd->fd_mask);

        } else if (msg->mn_Node.ln_Type == NT_DEATHMESSAGE) {
            /* Time to die. */
            dead = TRUE;
        } else {
            /* Unrecognized message type.
             */
        }

        ReplyMsg(msg);
    }

    /* Done! */
    D(bug("%s[%p]: Exiting\n", __func__, FindTask(NULL)));
}

static int BSDSocket_Init(struct BSDSocketBase *BSDSocketBase)
{
    struct Task *t;

    NEWLIST(&BSDSocketBase->bs_FDList);
    InitSemaphore(&BSDSocketBase->bs_Lock);

    t = NewCreateTask(TASKTAG_PC, BSDSocket_NotifyTask,
                      TASKTAG_NAME, "bsdsocket.library:Notify",
                      TASKTAG_ARG1, BSDSocketBase,
                      TASKTAG_TASKMSGPORT, &BSDSocketBase->bs_MsgPort,
                      TAG_END);

    if (t == NULL)
        return FALSE;

    BSDSocketBase->bs_NotifyTask = t;

    D(bug("%s: BSDSocketBase=%p\n", __func__, BSDSocketBase));

    return TRUE;
}

static int BSDSocket_Expunge(struct BSDSocketBase *BSDSocketBase)
{
    struct Message msg;
    struct MsgPort *mp;

    /* Send an empty message (Message-Of-Death)
     * to the BSDSocketBase notification task
     */
    if ((mp = CreateMsgPort())) {
        msg.mn_Node.ln_Type = NT_DEATHMESSAGE;
        msg.mn_ReplyPort = mp;
        msg.mn_Length = sizeof(msg);
        PutMsg(BSDSocketBase->bs_MsgPort, &msg);
        WaitPort(mp);
        GetMsg(mp);
        DeleteMsgPort(mp);

        D(bug("%s: BSDSocketBase=NULL\n", __func__));

        return TRUE;
    }

    return FALSE;
}

ADD2INITLIB(BSDSocket_Init, 0);
ADD2EXPUNGELIB(BSDSocket_Expunge, 0);
