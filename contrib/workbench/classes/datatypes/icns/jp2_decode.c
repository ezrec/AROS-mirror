#include "icns_class.h"
#include <proto/openjpeg.h>
#include <inline4/openjpeg.h>

int32 icns_jp2_decode (struct IconInfo *info, uint8 *out) {
	uint8 *in = info->Data;
	int32 in_size = info->DataSize;
	int32 error = OK;
	opj_dinfo_t *dinfo = NULL;
	opj_dparameters_t dparams;
	opj_cio_t *cio = NULL;
	opj_image_t *image = NULL;
	uint8 shift[4] = {0};
	int x, y, depth;
	int *red_ptr, *green_ptr, *blue_ptr, *alpha_ptr;
	int red, green, blue, alpha;

	if (!IOpenJPEG) {
		return DTERROR_UNKNOWN_COMPRESSION;
	}

	dinfo = opj_create_decompress(CODEC_JP2);
	if (!dinfo) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	opj_set_default_decoder_parameters(&dparams);

	opj_setup_decoder(dinfo, &dparams);

	cio = opj_cio_open((opj_common_ptr)dinfo, in, in_size);
	if (!cio) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}
	
	image = opj_decode(dinfo, cio);
	if (!image) {
		error = DTERROR_INVALID_DATA;
		goto out;
	}

	opj_cio_close(cio);
	opj_destroy_decompress(dinfo);
	cio = NULL;
	dinfo = NULL;

	if (image->numcomps != 4 ||
		image->comps[0].w != info->Width || image->comps[0].h != info->Height)
	{
		error = DTERROR_INVALID_DATA;
		goto out;
	}

	for (x = 0; x < 4; x++) {
		depth = image->comps[x].prec;
		if (depth > 8) {
			shift[x] = depth - 8;
		}
	}

	red_ptr = image->comps[0].data;
	green_ptr = image->comps[1].data;
	blue_ptr = image->comps[2].data;
	alpha_ptr = image->comps[3].data;

	for (y = 0; y < info->Height; y++) {
		for (x = 0; x < info->Width; x++) {
			red = *red_ptr++;
			green = *green_ptr++;
			blue = *blue_ptr++;
			alpha = *alpha_ptr++;
			if (image->comps[0].sgnd) red += 1 << (image->comps[0].prec - 1);
			if (image->comps[1].sgnd) green += 1 << (image->comps[1].prec - 1);
			if (image->comps[2].sgnd) blue += 1 << (image->comps[2].prec - 1);
			if (image->comps[3].sgnd) alpha += 1 << (image->comps[3].prec - 1);
			*out++ = alpha;
			*out++ = red;
			*out++ = green;
			*out++ = blue;
		}
	}

out:
	if (image) opj_image_destroy(image);
	if (cio) opj_cio_close(cio);
	if (dinfo) opj_destroy_decompress(dinfo);

	return error;
}
