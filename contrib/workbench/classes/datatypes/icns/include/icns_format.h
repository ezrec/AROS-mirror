/*
 * icns.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef ICNS_FORMAT_H
#define ICNS_FORMAT_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

struct ResourceHeader {
	uint32 ResourceType; // 'icns'
	uint32 ResourceSize; // size of resource
	// icon elements
};

struct ElementHeader {
	uint32 ElementType; // 'ICN#', 'icl4', ...
	uint32 ElementSize; // size of element
	// image data
};

struct IconInfo {
	uint32 Type;
	uint16 Image;
	uint16 Mask;
	uint16 Width;
	uint16 Height;
	uint16 Channels;
	uint16 PixelDepth;
	uint16 BitDepth;
	uint32 DataSize;
	uint8 *Data;
};

#define RST_ICNS			MAKE_ID('i','c','n','s')

#define ICNS_ARGB32_512x512	MAKE_ID('i','c','0','9')
#define ICNS_ARGB32_256x256	MAKE_ID('i','c','0','8')
#define ICNS_RGB24_128x128	MAKE_ID('i','t','3','2')
#define ICNS_RGB24_48x48	MAKE_ID('i','h','3','2')
#define ICNS_RGB24_32x32	MAKE_ID('i','l','3','2')
#define ICNS_RGB24_16x16	MAKE_ID('i','s','0','8')

#define ICNS_CLUT8_48x48	MAKE_ID('i','c','h','8')
#define ICNS_CLUT8_32x32	MAKE_ID('i','c','l','8')
#define ICNS_CLUT8_16x16	MAKE_ID('i','c','s','8')
#define ICNS_CLUT8_16x12	MAKE_ID('i','c','m','8')

#define ICNS_CLUT4_48x48	MAKE_ID('i','c','h','4')
#define ICNS_CLUT4_32x32	MAKE_ID('i','c','l','4')
#define ICNS_CLUT4_16x16	MAKE_ID('i','c','s','4')
#define ICNS_CLUT4_16x12	MAKE_ID('i','c','m','4')

#define ICNS_C1A1_48x48		MAKE_ID('i','c','h','#')
#define ICNS_C1A1_32x32		MAKE_ID('I','C','N','#')
#define ICNS_C1A1_16x16		MAKE_ID('i','c','s','#')
#define ICNS_C1A1_16x12		MAKE_ID('i','c','m','#')

#define ICNS_ALPHA8_128x128	MAKE_ID('t','8','m','k')
#define ICNS_ALPHA8_48x48	MAKE_ID('h','8','m','k')
#define ICNS_ALPHA8_32x32	MAKE_ID('l','8','m','k')
#define ICNS_ALPHA8_16x16	MAKE_ID('s','8','m','k')

#endif
