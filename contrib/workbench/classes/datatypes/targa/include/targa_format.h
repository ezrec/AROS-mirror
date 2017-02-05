/*
 * targa.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef TARGA_FORMAT_H
#define TARGA_FORMAT_H

/* info on .tga (targa) format:
 * http://www.fileformat.info/format/tga/
 * http://organicbit.com/closecombat/formats/tga.html */

struct TargaHeader { // size: 18
	uint8 idsize; // size of ID field after header (usually 0)
	uint8 cmaptype;
	uint8 imagetype;
	
	uint8 cmapstart[2];
	uint8 cmaplength[2];
	uint8 cmapdepth; // bits per palette entry (15, 16, 24, 32)

	uint16 xstart, ystart; // image x, y origin
	uint16 width, height;
	uint8 depth; // bits per pixel (8, 15, 16, 24, 32)
	uint8 descriptor; // image descriptor bits (vh flip bits)
};

enum {
	TGA_CMAP_NONE = 0,
	TGA_CMAP_LUT
};
enum {
	TGA_IMAGE_NONE = 0,
	TGA_IMAGE_LUT,
	TGA_IMAGE_RGB,
	TGA_IMAGE_GREY
};
#define TGA_IMAGE_RLE 8

#endif /* TARGA_FORMAT_H */