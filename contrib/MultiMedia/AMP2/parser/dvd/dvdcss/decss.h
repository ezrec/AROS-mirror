/*
 *
 * decss.h
 *
 */

#ifndef DECSS_H
#define DECSS_H

extern int css_decrypt_title_key(unsigned char *title_key, unsigned char *disk_key);
extern void css_descramble_sector(unsigned char *sector, unsigned char *key);

#endif
