/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/bio.h>

void bioqdisksort(struct bio_queue_head *bioq, struct bio *bio)
{
    TAILQ_INSERT_TAIL(&bioq->queue, bio, bio_act);
}
