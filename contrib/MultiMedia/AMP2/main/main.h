/*
 *
 * main.h
 *
 */

#ifndef MAIN_H
#define MAIN_H

void amp_printf(const char *fmt, ...);
void debug_printf(const char *fmt, ...);
void verbose_printf(const char *fmt, ...);

#define SWAP16(val) ({ unsigned short x = val; x = (x << 8) | (x >> 8); x; })
#define SWAP32(val) ({ unsigned long x = val; x = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24); x; })

#endif
