#include "icns_class.h"

int32 rle24_decode (uint8 *in, int32 in_size, uint8 *out_ptr, int32 out_size, int32 pixel_count) {
	int component;
	uint8 *out;
	int32 pixels_left;
	uint32 run_length;

	if (*(uint32 *)in == 0) {
		in_size -= 4;
		if (in_size <= 0) return DTERROR_INVALID_DATA;
		in += 4;
	}

	for (component = 1; component < 4; component++) {
		out = out_ptr + component;

		pixels_left = pixel_count;

		while (pixels_left > 0 && in_size > 0) {
			if (*in & 0x80) {
				uint8 value;

				in_size -= 2;
				if (in_size < 0) return DTERROR_INVALID_DATA;

				run_length = (*in++) - 125;
				value = *in++;

				while (run_length-- && pixels_left > 0) {
					*out = value;
					out += 4;
					pixels_left--;
				}
			} else {
				if (in_size-- == 0) return DTERROR_INVALID_DATA;
				
				run_length = (*in++) + 1;

				while (run_length-- && pixels_left > 0 && in_size > 0) {
					*out = *in++;
					out += 4;
					pixels_left--;
					in_size--;
				}
			}
		}
	}

	out = out_ptr;
	pixels_left = pixel_count;
	while (pixels_left--) {
		*out = 255;
		out += 4;
	}

	return OK;
}
