/*
 *
 * mpegio.h
 *
 */

#ifndef MPEGIO_H
#define MPEGIO_H

extern int mpegio_length(void);
extern int mpegio_fread(unsigned char *buffer, int length);
extern int mpegio_fseek(int length, int whence);
extern int mpegio_ftell(void);
extern int mpegio_feof(void);

extern int cd_read(unsigned char *dst, int sector, int length);

/* FIXME: This doesn't look all that nice... */
extern void (*ExecRead)(unsigned long frame,unsigned char *buf,int frames);

#define RAW_SECTOR_SIZE 2352
#define VCD_SECTOR_SIZE 2324
#define DVD_SECTOR_SIZE 2048

#endif
