/*
 *
 * getbits.h
 *
 */

static unsigned long rval;

#define backbits(nob) ((void)( \
  bsi.bitindex    -= nob, \
  bsi.wordpointer += (bsi.bitindex>>3), \
  bsi.bitindex    &= 0x7 ))

#define getbitoffset() ((-bsi.bitindex)&0x7)
#define getbyte()      (*bsi.wordpointer++)

#define getbits(nob) ( \
  rval = bsi.wordpointer[0], rval <<= 8, rval |= bsi.wordpointer[1], \
  rval <<= 8, rval |= bsi.wordpointer[2], rval <<= bsi.bitindex, \
  rval &= 0xffffff, bsi.bitindex += nob, \
  rval >>= (24-nob), bsi.wordpointer += (bsi.bitindex>>3), \
  bsi.bitindex &= 7, rval)

#define getbits_fast(nob) ( \
  rval = (unsigned char) (bsi.wordpointer[0] << bsi.bitindex), \
  rval |= ((unsigned long) bsi.wordpointer[1]<<bsi.bitindex)>>8, \
  rval <<= nob, rval >>= 8, \
  bsi.bitindex += nob, bsi.wordpointer += (bsi.bitindex>>3), \
  bsi.bitindex &= 7, rval)

