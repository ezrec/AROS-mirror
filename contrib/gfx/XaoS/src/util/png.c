#ifndef _plan9_
#include <aconfig.h>
#ifdef USE_PNG
#include <png.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#ifndef AROS
#include <malloc.h>
#endif
#include <errno.h>
#else
#include <u.h>
#include <libc.h>
#include <stdio.h>
#endif
#include <filter.h>
#include <version.h>
#ifndef USE_PNG
char *
writepng (FILE * f, struct image *img)
{
  return "XaoS can not save images because it was incorrectly compiled. Please compile it with zlib and libpng";
}
#else

char *
writepng (FILE * file, struct image *image)
{
  png_structp png_ptr;
  png_infop info_ptr;
  png_color palette[256];
  volatile unsigned short a = 255;
  volatile unsigned char *b = (unsigned char *) &a;
  static char text[] =
  "XaoS" XaoS_VERSION " - an realtime interactive fractal zoomer";
  static png_text comments[] =
  {
    {
      -1,
      "Software",
      text,
      sizeof (text)
    }
  };
  errno = -1;
  if (file == NULL)
    {
      return strerror(errno);
    }
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, (void *) NULL, (png_error_ptr) NULL, (png_error_ptr) NULL);
  if (!png_ptr)
    return "Unable to initialize pnglib";
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
    {
      png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
      return "No memory to create png info structure";
    }
  if (setjmp (png_ptr->jmpbuf))
    {
      png_destroy_write_struct (&png_ptr, &info_ptr);
      fclose (file);
      return strerror(errno);
    }
  png_init_io (png_ptr, file);
  png_set_filter (png_ptr, 0, PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_PAETH | PNG_FILTER_UP | PNG_FILTER_AVG);
  /* set the zlib compression level */
  /*png_set_compression_level(png_ptr, Z_BEST_COMPRESSION); */
  png_set_compression_level (png_ptr, Z_DEFAULT_COMPRESSION);

  /* set other zlib parameters */
  png_set_compression_mem_level (png_ptr, 8);
  png_set_compression_strategy (png_ptr, Z_DEFAULT_STRATEGY);
  png_set_compression_window_bits (png_ptr, 15);
  png_set_compression_method (png_ptr, 8);

  info_ptr->width = image->width;
  info_ptr->height = image->height;
  /*info_ptr->gamma=1.0; */
  info_ptr->gamma = 0.5;
  info_ptr->valid |= PNG_INFO_gAMA | PNG_INFO_pHYs;
  info_ptr->x_pixels_per_unit = 100 / image->pixelwidth;
  info_ptr->y_pixels_per_unit = 100 / image->pixelheight;


  switch (image->palette->type)
    {
    case C256:
      {
	int i;
	info_ptr->color_type = PNG_COLOR_TYPE_PALETTE;
	info_ptr->bit_depth = image->bytesperpixel * 8;
	info_ptr->palette = palette;
	info_ptr->valid |= PNG_INFO_PLTE;
	for (i = 0; i < image->palette->end; i++)
	  info_ptr->palette[i].red = image->palette->rgb[i][0],
	    info_ptr->palette[i].green = image->palette->rgb[i][1],
	    info_ptr->palette[i].blue = image->palette->rgb[i][2],
	    info_ptr->num_palette = image->palette->end;
      }
      break;
    case SMALLITER:
    case LARGEITER:
    case GRAYSCALE:
      info_ptr->color_type = PNG_COLOR_TYPE_GRAY;
      info_ptr->bit_depth = image->bytesperpixel * 8;
      break;
    case TRUECOLOR:
    case TRUECOLORMI:
    case TRUECOLOR24:
    case HICOLOR:
    case REALCOLOR:
      info_ptr->color_type = PNG_COLOR_TYPE_RGB;
      info_ptr->bit_depth = 8;
      if (image->palette->type & (TRUECOLOR | TRUECOLOR24|TRUECOLORMI))
	{
	  info_ptr->sig_bit.red = 8;
	  info_ptr->sig_bit.green = 8;
	  info_ptr->sig_bit.blue = 8;
	}
      else
	{
	  info_ptr->sig_bit.red = 5;
	  info_ptr->sig_bit.green = 5;
	  info_ptr->sig_bit.blue = 5;
	  if (image->palette->type == HICOLOR)
	    info_ptr->sig_bit.green = 6;
	}
      break;
    }
  info_ptr->interlace_type = 0;
  info_ptr->num_text = sizeof (comments) / sizeof (png_text);
  info_ptr->text = comments;

  png_write_info (png_ptr, info_ptr);
  /*png_set_filler(png_ptr,0,PNG_FILLER_AFTER); */
  png_set_packing (png_ptr);
  if (image->palette->type & (HICOLOR | REALCOLOR | TRUECOLOR | TRUECOLOR24|TRUECOLORMI))
    png_set_shift (png_ptr, &(info_ptr->sig_bit));
  if (*b == 255)
    png_set_swap (png_ptr);
  png_set_bgr (png_ptr);
  switch (image->palette->type)
    {
    case C256:
    case GRAYSCALE:
    case SMALLITER:
    case LARGEITER:
#ifdef STRUECOLOR24
    case TRUECOLOR24:
      png_write_image (png_ptr, (png_bytepp) image->currlines);
      break;
#endif
    case TRUECOLOR:
      {
	int i, y;
	unsigned char *r = (unsigned char *) malloc (image->width * 3);
	for (i = 0; i < image->height; i++)
	  {
	    for (y = 0; y < image->width; y++)
	      r[y * 3] = ((pixel32_t **) image->currlines)[i][y] & 255,
		r[y * 3 + 1] = (((pixel32_t **) image->currlines)[i][y] >> 8) & 255,
		r[y * 3 + 2] = (((pixel32_t **) image->currlines)[i][y] >> 16) & 255;
	    png_write_rows (png_ptr, (png_bytepp) & r, 1);
	  }
      }
      break;
#ifdef SUPPORTMISSORDERED
    case TRUECOLORMI:
      {
	int i, y;
	unsigned char *r = (unsigned char *) malloc (image->width * 3);
	for (i = 0; i < image->height; i++)
	  {
	    for (y = 0; y < image->width; y++)
	        r[y * 3 + 2] = (((pixel32_t **) image->currlines)[i][y]>>8) & 255,
		r[y * 3 + 1] = (((pixel32_t **) image->currlines)[i][y] >> 16) & 255,
		r[y * 3 + 0] = (((pixel32_t **) image->currlines)[i][y] >> 24) & 255;
	    png_write_rows (png_ptr, (png_bytepp) & r, 1);
	  }
      }
      break;
#endif
#ifdef SHICOLOR
    case HICOLOR:
      {
	int i, y;
	unsigned char *r = (unsigned char *) malloc (image->width * 3);
	for (i = 0; i < image->height; i++)
	  {
	    for (y = 0; y < image->width; y++)
	      r[y * 3] = ((pixel16_t **) image->currlines)[i][y] & 31,
		r[y * 3 + 1] = (((pixel16_t **) image->currlines)[i][y] >> 5) & 63,
		r[y * 3 + 2] = (((pixel16_t **) image->currlines)[i][y] >> 11) & 31;
	    png_write_rows (png_ptr, (png_bytepp) & r, 1);
	  }
      }
      break;
#endif
#ifdef SREALCOLOR
    case REALCOLOR:
      {
	int i, y;
	unsigned char *r = (unsigned char *) malloc (image->width * 3);
	for (i = 0; i < image->height; i++)
	  {
	    for (y = 0; y < image->width; y++)
	      r[y * 3] = ((pixel16_t **) image->currlines)[i][y] & 31,
		r[y * 3 + 1] = (((pixel16_t **) image->currlines)[i][y] >> 5) & 31,
		r[y * 3 + 2] = (((pixel16_t **) image->currlines)[i][y] >> 10) & 31;
	    png_write_rows (png_ptr, (png_bytepp) & r, 1);
	  }
      }
#endif
    }
  png_write_end (png_ptr, info_ptr);
  png_destroy_write_struct (&png_ptr, &info_ptr);
  fclose (file);
  return NULL;
}
#endif
