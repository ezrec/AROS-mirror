/*
 *
 * mount.h
 *
 */

#ifndef MOUNT_H
#define MOUNT_H

int mount_cd(int *lba_start, int *lba_stop);

/* MPEGIO compatible */

#define MOUNT_ERR 0x00
#define MOUNT_VCD 0x02 /* MPEG1 */
#define MOUNT_CDI 0x04
#define MOUNT_DVD 0x08
#define MOUNT_SVD 0x10 /* MPEG2 */

#endif
