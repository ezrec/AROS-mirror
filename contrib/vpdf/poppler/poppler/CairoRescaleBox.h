#ifndef CAIRO_RESCALE_BOX_H
#define CAIRO_RESCALE_BOX_H

#include <stdint.h>
#include "goo/gtypes.h"

GBool downscale_box_filter(uint32_t *orig, int orig_stride, unsigned orig_width, unsigned orig_height,
			   signed scaled_width, signed scaled_height,
			   uint16_t start_column, uint16_t start_row,
			   uint16_t width, uint16_t height,
			   uint32_t *dest, int dst_strid);

#endif /* CAIRO_RESCALE_BOX_H */
