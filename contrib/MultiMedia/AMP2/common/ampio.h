/*
 *
 * ampio.h
 *
 */

#ifndef AMPIO_H
#define AMPIO_H

extern int ampio_fopen(char *filename);
extern int ampio_fread(unsigned char *buffer, int length);
extern int ampio_fseek(int length, int whence);
extern int ampio_fskip(int length);
extern int ampio_length(void);
extern int ampio_ftell(void);
extern int ampio_feof(void);
extern int ampio_fclose(void);

/* Slow but usefull */
extern unsigned char get_byte();
extern unsigned short get_be16();
extern unsigned long get_be32();
extern unsigned short get_le16();
extern unsigned long get_le32();

#endif
