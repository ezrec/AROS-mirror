/*
 * ico.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef ICO_FORMAT_H
#define ICO_FORMAT_H 1

/*
 *	info on .ico format:
 *	http://www.daubnet.com/formats/ICO.html
 *	http://en.wikipedia.org/wiki/ICO_(icon_image_file_format)
 */

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack(2)
   #endif
#elif defined(__VBCC__)
   #pragma amiga-align
#endif

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

struct ICO_FileHeader {
	uint16 Reserved; // =0
	uint16 Type; // =1
	uint16 Count; // number of icon entries
};

struct ICO_Entry {
	uint8 Width, Height; // cursor width/height
	uint8 Colors; // (0 = 256)
	uint8 Reserved; // =0
	uint16 Planes; // =1
	uint16 BitDepth; // bits per pixel (1,4,8)
	uint32 SizeInBytes; // size of InfoHeader+ANDbitmap+XORbitmap
	uint32 FileOffset; // offset in file of InfoHeader
};


/* variant of BMP_InfoHeader */
struct ICO_InfoHeader {
	uint32 Size; // size of InfoHeader struct (=40)
	uint32 Width, Height; // icon width/height
	uint16 Planes; // =1
	uint16 BitDepth; // bits per pixel (1,4,8)
	uint32 CompType; // Type of compression (0=none)
	uint32 ImageSize; // size of image in bytes (uncompressed)
	uint32 XpixelsPerM, YpixelsPerM;
	uint32 ColorsUsed;
	uint32 ColorsImportant;
};

#define ICO_COMP_NONE (0L)

struct ICO_Color {
	uint8 blue, green, red, unused;
};

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* ICO_FORMAT_H */
