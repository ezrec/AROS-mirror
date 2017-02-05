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
	uint8 red, green, blue;
};

struct PCXHeader {
	uint8 Manufacturer; // = 10
	uint8 Version;
	uint8 Compression; // = 1
	uint8 BitsPerPixel; // = 1, 2, 4 or 8
	struct {
		uint16 XMin, YMin, XMax, YMax;
	} Window; // image dimensions
	struct {
		uint16 X, Y;
	} DPI;
	struct PCXColor ColorMap[16];
	uint8 Reserved; // = 0
	uint8 NPlanes; // number of color planes
	uint16 BytesPerRow;
	uint16 PaletteType;
};

#define PCX_MAN_ZSOFT 10

#define PCX_COMP_RLE 1

#define PCX_PALINFO_COLOR_BW 1
#define PCX_PALINFO_GRAYSCALE 2

#define PCX_HEADER_SIZE 128

#endif
