/*
 * pcx.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef PCX_FORMAT_H
#define PCX_FORMAT_H 1

/* info about PCX format:
 * http://www.qzx.com/pc-gpe/pcx.txt
 */

struct PCXColor {
	UBYTE red, green, blue;
};

struct PCXHeader {
	UBYTE Manufacturer; // = 10
	UBYTE Version;
	UBYTE Compression; // = 1
	UBYTE BitsPerPixel; // = 1, 2, 4 or 8
	struct {
		UWORD XMin, YMin, XMax, YMax;
	} Window; // image dimensions
	struct {
		UWORD X, Y;
	} DPI;
	struct PCXColor ColorMap[16];
	UBYTE Reserved; // = 0
	UBYTE NPlanes; // number of color planes
	UWORD BytesPerRow;
	UWORD PaletteType;
};

#pragma pack(2)
struct PCXHeaderFile {
	UBYTE Manufacturer; // = 10
	UBYTE Version;
	UBYTE Compression; // = 1
	UBYTE BitsPerPixel; // = 1, 2, 4 or 8
	struct {
		UWORD XMin, YMin, XMax, YMax;
	} Window; // image dimensions
	struct {
		UWORD X, Y;
	} DPI;
	struct PCXColor ColorMap[16];
	UBYTE Reserved; // = 0
	UBYTE NPlanes; // number of color planes
	UWORD BytesPerRow;
	UWORD PaletteType;
};
#pragma pack()

#define PCX_MAN_ZSOFT 10

#define PCX_COMP_RLE 1

#define PCX_PALINFO_COLOR_BW 1
#define PCX_PALINFO_GRAYSCALE 2

#define PCX_HEADER_SIZE 128

#endif
