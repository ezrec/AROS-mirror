/*
 *
 * dvd.h
 *
 */

#ifndef DVD_H
#define DVD_H

extern int is_dvd_encrypted(void);
extern int get_disk_key(unsigned char *disk_key);
extern int get_title_key(unsigned char *title_key, int lba);

#endif
