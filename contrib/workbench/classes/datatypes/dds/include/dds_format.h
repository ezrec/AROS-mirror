/*
 * dds.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef DDS_FORMAT_H
#define DDS_FORMAT_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef ENDIAN_H
#include "endian.h"
#endif

struct DDS_Header {
	uint32 Magic;
	uint32 Size;
	uint32 Flags;
	uint32 Height;
	uint32 Width;
	uint32 PitchOrLinearSize;
	uint32 Depth;
	uint32 MipMapCount;
	uint32 Reserved[11];
	struct {
		uint32 Size;
		uint32 Flags;
		uint32 FourCC;
		uint32 RGBBitCount;
		uint32 RBitMask;
		uint32 GBitMask;
		uint32 BBitMask;
		uint32 ABitMask;
	} PixelFormat;
	struct {
		uint32 Caps1;
		uint32 Caps2;
		uint32 Reserved[2];
	} Caps;
	uint32 Reserved2;
};

struct DDS_Header_DXT10 {
	uint32 DXGIFormat;
	uint32 ResourceDimension;
	uint32 MiscFlags;
	uint32 ArraySize;
	uint32 Reserved;
};

#define DDS_MAGIC					MAKE_ID('D','D','S',' ')

// DDS_Header.Flags
#define DDSD_CAPS					0x01000000
#define DDSD_HEIGHT					0x02000000
#define DDSD_WIDTH					0x04000000
#define DDSD_PITCH					0x08000000
#define DDSD_PIXELFORMAT			0x00100000
#define DDSD_MIPMAPCOUNT			0x00000200
#define DDSD_LINEARSIZE				0x00000800
#define DDSD_DEPTH					0x00008000
#define DDSD_REQUIRED				(DDSD_WIDTH|DDSD_WIDTH|DDSD_PIXELFORMAT)

// DDS_Header.PixelFormat.Flags
#define DDPF_ALPHAPIXELS			0x01000000
#define DDPF_FOURCC					0x04000000
#define DDPF_INDEXED				0x20000000
#define DDPF_RGB					0x40000000

// DDS_Header.Caps.Caps1
#define DDSCAPS_COMPLEX				0x08000000
#define DDSCAPS_TEXTURE				0x00100000
#define DDSCAPS_MIPMAP				0x00004000

// DDS_Header.Caps.Caps2
#define DDSCAPS2_CUBEMAP			0x00020000
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x00040000
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x00080000
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x00100000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x00200000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x00400000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	0x00800000
#define DDSCAPS2_VOLUME				0x00002000

// DDS_Header.PixelFormat.FourCC
#define D3DFMT_DXT1					MAKE_ID('D','X','T','1')
#define D3DFMT_DXT2					MAKE_ID('D','X','T','2')
#define D3DFMT_DXT3					MAKE_ID('D','X','T','3')
#define D3DFMT_DXT4					MAKE_ID('D','X','T','4')
#define D3DFMT_DXT5					MAKE_ID('D','X','T','5')
#define D3DFMT_RXGB					MAKE_ID('R','X','G','B')

#endif
