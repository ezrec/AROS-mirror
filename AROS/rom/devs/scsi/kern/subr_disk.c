/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <sys/systm.h>
#include <sys/disk.h>
#include <sys/buf.h>
#include <sys/bio.h>

void bioqdisksort(struct bio_queue_head *bioq, struct bio *bio)
{
    TAILQ_INSERT_TAIL(&bioq->queue, bio, bio_act);
}

static inline void buf_complete(struct buf *bp)
{
    struct bio_ops *ops = bp->b_ops;

    if (ops)
        ops->io_complete(bp);
}

void biodone(struct bio *bio)
{
    struct buf *bp = bio->bio_buf;

    while (bio) {
        biodone_t *done_func;

        if ((done_func = bio->bio_done) != NULL) {
            bio->bio_done = NULL;
            done_func(bio);
            return;
        }
        bio = bio->bio_prev;
    }

    bpdone(bp, 1);
}

void biodone_sync(struct bio *bio)
{
    struct buf *bp = bio->bio_buf;
    int flags;
    int nflags;

    bpdone(bp, 0);

    for (;;) {
        flags = bio->bio_flags;
        nflags = (flags | BIO_DONE) & ~BIO_WANT;
        if (atomic_cmpset_int(&bio->bio_flags, flags, nflags)) {
            if (flags & BIO_WANT)
                wakeup(bio);
            break;
        }
    }
}

void bpdone(struct buf *bp, int elseit)
{
    buf_cmd_t cmd;

    cmd = bp->b_cmd;
    bp->b_cmd = BUF_CMD_DONE;

    if (cmd != BUF_CMD_READ && cmd != BUF_CMD_WRITE) {
        if (cmd == BUF_CMD_FREEBLKS)
            bp->b_flags |= B_NOCACHE;
        if (elseit)
            brelse(bp);
        return;
    }

    if (LIST_FIRST(&bp->b_dep) != NULL)
        buf_complete(bp);

    if (elseit) {
        brelse(bp);
    }
}
