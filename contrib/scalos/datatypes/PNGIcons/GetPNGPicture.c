// GetPNGPicture.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <workbench/workbench.h>
#include <libraries/iffparse.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <clib/alib_protos.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "png.h"
#include <defs.h>
#include "PNGIconDt.h"
#include "PNGIconProto.h"

//-----------------------------------------------------------------------------

static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size);
static void PngFreeMem(png_structp png_ptr, png_voidp ptr);
static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
static int read_chunk_callback(png_structp png_ptr, png_unknown_chunkp chunk);
static void PngError(png_structp png_ptr, png_const_charp error_message);
static void PngWarning(png_structp png_ptr, png_const_charp warning_message);

//-----------------------------------------------------------------------------

// Given a PNG file buffer decompress the image data and returns an ARGB image array.
// Width and height are filled in with the image dimensions.
// Only 24bit image formats (with or without alpha channel) are supported by choice.
// Since the output buffer is intended for usage with ARGB alpha blit functions,
// if no alpha is present it will be filled with 255.


BOOL GetPngPicture(struct InstanceData *inst, BPTR file,
	ULONG SigBytesRead, struct ARGBHeader *argbh)
{
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	BOOL success = FALSE;


	d1(KPrintF("%s/%s/%ld:  inst=%08lx  file=%08lx  SigBytesRead=%lu\n", __FILE__, __FUNC__, __LINE__, inst, file, SigBytesRead));

	do	{
		png_bytepp row_pointers;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;
		struct ARGB *argb;
		ULONG BytesPerRow;
		ULONG y;

		png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
		      (png_voidp) inst, PngError, PngWarning, NULL, PngAllocMem, PngFreeMem);

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));
		if (NULL == png_ptr)
			break;

		/* Allocate/initialize the memory for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		d1(KPrintF("%s/%s/%ld:  info_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, info_ptr));
		if (NULL == info_ptr)
			break;

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));
		d1(KPrintF("%s/%s/%ld:  sizeof(jmp_buf)=%lu  png_jmpbuf=%08lx\n", __FILE__, __FUNC__, __LINE__, sizeof(jmp_buf), png_jmpbuf(png_ptr)));
		// safety check against jmp_buf size mismatch between libpng and Scalos
		if (NULL == png_jmpbuf(png_ptr))
			break;

		if (setjmp(png_jmpbuf(png_ptr)))
			{
			d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));
			success = FALSE;
			break;
			}

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));

		png_set_read_fn(png_ptr, (void *) file, user_read_data);

		png_set_sig_bytes(png_ptr, SigBytesRead);

		png_set_read_user_chunk_fn(png_ptr, inst, read_chunk_callback);

		png_set_keep_unknown_chunks(png_ptr,
			PNG_HANDLE_CHUNK_IF_SAFE, NULL, 0);

		png_set_palette_to_rgb(png_ptr);
		png_set_expand_gray_1_2_4_to_8(png_ptr);
		png_set_tRNS_to_alpha(png_ptr);
		png_set_gray_to_rgb(png_ptr);
		png_set_add_alpha(png_ptr, ~0, PNG_FILLER_BEFORE);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		png_read_png(png_ptr, info_ptr,
			PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING
                        | PNG_TRANSFORM_SWAP_ALPHA | PNG_TRANSFORM_EXPAND, NULL);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		png_get_IHDR(png_ptr, info_ptr,
			&width, &height, &bit_depth, &color_type,
			&interlace_type, NULL, NULL);

		d1(KPrintF("%s/%s/%ld:  width=%ld  height=%ld  depth=%ld color_type=%ld\n", __FILE__, __FUNC__, __LINE__, width, height, bit_depth, color_type));

		row_pointers = png_get_rows(png_ptr, info_ptr);
		d1(KPrintF("%s/%s/%ld:  row_pointers=%08lx\n", __FILE__, __FUNC__, __LINE__, row_pointers));
		if (NULL == row_pointers)
			break;

		argbh->argb_Width = width;
		argbh->argb_Height = height;
		argbh->argb_ImageData = MyAllocVecPooled(width * height * sizeof(struct ARGB));

		d1(KPrintF("%s/%s/%ld:  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, argbh->argb_ImageData));
		if (NULL == argbh->argb_ImageData)
			break;

		d1(KPrintF("%s/%s/%ld:  width=%ld  height=%ld  depth=%ld color_type=%ld\n", __FILE__, __FUNC__, __LINE__, width, height, bit_depth, color_type));

		BytesPerRow = png_get_rowbytes(png_ptr, info_ptr);

		d1(KPrintF("%s/%s/%ld:  BytesPerRow=%lu\n", __FILE__, __FUNC__, __LINE__, BytesPerRow));

		argb = argbh->argb_ImageData;
		for (y = 0; y < height; y++)
			{
			memcpy(argb, row_pointers[y], BytesPerRow);
			argb += width;
			}

		success = TRUE;
	} while (0);

	d1(KPrintF("%s/%s/%ld:  success=%ld\n", __FILE__, __FUNC__, __LINE__, success));

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return success;
}


static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size)
{
	(void) png_ptr;

	return MyAllocVecPooled(size);
}

static void PngFreeMem(png_structp png_ptr, png_voidp ptr)
{
	(void) png_ptr;
	MyFreeVecPooled(ptr);
}


static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	BPTR fh = (BPTR) png_get_io_ptr(png_ptr);

	if (1 != FRead(fh, data, 1, length))
		{
//		png_error();
		}
}


static int read_chunk_callback(png_structp png_ptr, png_unknown_chunkp chunk)
{
	struct InstanceData *inst = png_get_user_chunk_ptr(png_ptr);

	d1(KPrintF("%s/%s/%ld:  inst=%08lx  name=%c%c%c%c\n", __FILE__, __FUNC__, __LINE__, inst, chunk->name[0], chunk->name[1], chunk->name[2], chunk->name[3]));

	if (chunk->name[0] == 'i'
		&& chunk->name[1] == 'c'
		&& chunk->name[2] == 'O'
		&& chunk->name[3] == 'n'
		)
		{
		HandleIconHunk(inst, chunk->data, chunk->size);
		return 1;
		}
	else
		{
		return 0;
		}
}

void abort(void)
{
	while (1)
		;
}

static void PngError(png_structp png_ptr, png_const_charp error_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, error_message));

	longjmp(png_jmpbuf(png_ptr), 1);
}


static void PngWarning(png_structp png_ptr, png_const_charp warning_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, warning_message));
//	png_default_warning(png_ptr, warning_message);
}

