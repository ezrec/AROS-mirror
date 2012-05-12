/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_MOUNT_H
#define SYS_MOUNT_H

struct mount;
struct vnode;

/*
 * bio_ops are associated with the mount structure and used in conjuction
 * with the b_dep field in a buffer.  Currently softupdates and HAMMER
 * utilize this field.
 *
 * All bio_ops callbacks must be MPSAFE and could be called without
 * the mplock.
 */
struct buf;

struct bio_ops {
	TAILQ_ENTRY(bio_ops) entry;
	void	(*io_start) (struct buf *);
	void	(*io_complete) (struct buf *);
	void	(*io_deallocate) (struct buf *);
	int	(*io_fsync) (struct vnode *);
	int	(*io_sync) (struct mount *);
	void	(*io_movedeps) (struct buf *, struct buf *);
	int	(*io_countdeps) (struct buf *, int);
	int	(*io_checkread) (struct buf *);
	int	(*io_checkwrite) (struct buf *);
};

struct mount {
    struct bio_ops *mnt_bioops;
};

struct vnode {
    struct mount *v_mount;
};

#endif /* SYS_MOUNT_H */
