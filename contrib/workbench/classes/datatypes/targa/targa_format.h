/*
 * targa.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef TARGA_FORMAT_H
#define TARGA_FORMAT_H

/* info on .tga (targa) format:
 * http://www.fileformat.info/format/tga/
 * http://organicbit.com/closecombat/formats/tga.html */

#pragma pack(2)
struct TargaHeaderFile { // size: 18
	UBYTE idsize; // size of ID field after header (usually 0)
	UBYTE cmaptype;
	UBYTE imagetype;
	
	UBYTE cmapstart[2];
	UBYTE cmaplength[2];
	UBYTE cmapdepth; // bits per palette entry (15, 16, 24, 32)

	UWORD xstart, ystart; // image x, y origin
	UWORD width, height;
	UBYTE depth; // bits per pixel (8, 15, 16, 24, 32)
	UBYTE descriptor; // image descriptor bits (vh flip bits)
};
#pragma pack()

struct TargaHeader { // size: 18
	UBYTE idsize; // size of ID field after header (usually 0)
	UBYTE cmaptype;
	UBYTE imagetype;
	
	UWORD cmapstart;
	UWORD cmaplength;
	UBYTE cmapdepth; // bits per palette entry (15, 16, 24, 32)

	UWORD xstart, ystart; // image x, y origin
	UWORD width, height;
	UBYTE depth; // bits per pixel (8, 15, 16, 24, 32)
	UBYTE descriptor; // image descriptor bits (vh flip bits)
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