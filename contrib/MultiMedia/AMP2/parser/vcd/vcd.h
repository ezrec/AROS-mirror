/*
 *
 * vcd.h
 *
 */

#ifndef VCD_H
#define VCD_H

/* VCD, MPEG1 */
void vcd_main(int min_sector, int max_sector, int sector_size);

/* SVCD, MPEG2 */
void svcd_main(int min_sector, int max_sector, int sector_size);

#endif
