/*
 *
 * get1bit.h
 *
 */

static unsigned char rval_uc;

#define get1bit() ( \
  rval_uc = *bsi.wordpointer << bsi.bitindex, bsi.bitindex++, \
  bsi.wordpointer += (bsi.bitindex>>3), bsi.bitindex &= 7, rval_uc>>7 )

