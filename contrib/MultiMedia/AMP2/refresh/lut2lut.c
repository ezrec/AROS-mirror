/*
 *
 * lut2lut.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void lut2lut(rect_type *rect, unsigned char *src[])
{
  unsigned long *dptr;
  unsigned char *sptr;
  int y, add, row_add;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dptr = rect->dst;
  dptr += rect->dst_offset;

  for (y = 0; y < rect->yloop; y++) {
    if (rect->use_osd[y]) {
      sptr = rect->osd[0] + add;
    } else {
      sptr = src[0] + add;
    }

    memcpy(dptr, sptr, rect->xloop);
    sptr += rect->src_row_length;
    dptr += rect->dst_row_length;

    add += row_add;
  }
}

void lut2lut_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long *dptr;
  unsigned char *sptr;
  int y, add, row_add;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dptr = rect->dst;
  dptr += rect->dst_offset;

  for (y = 0; y < rect->yloop; y++) {
    if (rect->use_osd[y]) {
      sptr = rect->osd[0] + add;
    } else {
      sptr = src[0] + add;
    }

    memcpy(dptr, sptr, rect->xloop);
    sptr += rect->src_row_length;
    dptr += rect->dst_row_length;

    add += row_add;
  }
}
