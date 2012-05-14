/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <sys/systm.h>
#include <sys/bio.h>
#include <sys/buf.h>

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

void bpdone(struct buf *bp, int elseit)
{
    buf_cmd_t cmd;

    cmd = bp->b_cmd;
    bp->b_cmd = BUF_CMD_DONE;
    if (elseit)
        brelse(bp);
}

void brelse(struct buf *bp)
{
    /* FIXME */
}
