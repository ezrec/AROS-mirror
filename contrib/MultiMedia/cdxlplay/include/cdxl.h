/************************************************************************
***
***  Pan.h -- PAN file format used for multimedia sequencing
***
***  Created by: Carl Sassenrath & Ken Yeast, Pantaray, Inc., Jan 1991
***
************************************************************************/

#ifndef CDXL_H
#define	CDXL_H

#include <stdint.h>

#pragma pack(2)

/***********************************************************************
***
***  Pan Head Structure
***
***	Minimum required for all types of PAN files, including
***	custom PAN files.
***
************************************************************************/

struct PanHead
{
	uint8_t  Type;			// PAN structure type
	uint8_t  Info;			// PAN flags and other info
	uint32_t Size;			// Frame size including PAN
};

// Currently defined Types:

#define	PAN_CUSTOM	0		// For your custom data formats
#define	PAN_STANDARD	1		// First standard format
#define	PAN_SPECIAL	2		// Used for testing new ideas


/************************************************************************
***
***  Pan Frame Structure
***
***	Standard frame structure used for most files.
***
************************************************************************/

struct PanFrame
{
	uint8_t  Type;			// Type = PAN_STANDARD
	uint8_t  Info;			// PAN flags and other info (below)
	uint32_t Size;			// Frame size including PAN
	uint32_t Back;			// Bytes back to previous frame
	uint32_t Frame;			// Frame sequence number
	uint16_t XSize;			// Image width in pixels
	uint16_t YSize;			// Image height in pixels
	uint8_t  Reserved;		// Must be zero for now
	uint8_t  PixelSize;		// Bits per pixel (depth)
	uint16_t ColorMapSize;	// Size of color map in bytes
	uint16_t AudioSize;		// Size of audio sample in bytes
	uint8_t  PadBytes[8];	// Reserved for future

//	uint16_t ColorMap[]		// Color map  (variable size)
//	uint16_t Video[];		// Video data (variable size)
//	uint16_t Audio[];		// Audio data (variable size)
};

typedef	struct PanFrame PAN;

#define	PAN_SIZE		sizeof( PAN )

// PanFrame.Info: Video Types

#define	PIV_MASK		0x0F	// Mask for video values:
#define	PIV_STANDARD		0	// Normal RGB encoding
#define	PIV_HAM			1	// Amiga HAM encoding
#define	PIV_YUV			2	// YUV encoding
#define	PIV_AVM			3	// AVM & DCTV encoding
#define	PI_VIDEO( p )		( (p)->Info & PIV_MASK )

// PanFrame.Info: Pixel Value Orientation

#define	PIF_MASK		0xC0	// Mask for orientation values:
#define	PIF_PLANES		0x00	// Data is bit plane oriented
#define	PIF_PIXELS		0x40	// Data is pixel oriented (chunky)
#define	PIF_LINES		0x80	// Data is line oriented
#define	PI_PIXEL( p )		( (p)->Info & PIF_MASK )

// PanFrame.Info: Audio type

#define	PIA_MASK		0x10	// Mask for audio values:
#define	PIA_MONO		0x00	// 1 channel sound
#define	PIA_STEREO		0x10	// 2 channel sound
#define	PI_AUDIO( p )		( (p)->Info & PIA_MASK )


// Size macros

#define	ROW_SIZE( x )		( ( ( (x) + 15 ) / 16 ) << 1 )
#define	CMAP_SIZE( p )		( (p)->ColorMapSize )
#define	PLANE_SIZE( p )		( ROW_SIZE( (p)->XSize ) * (p)->YSize )
#define	IMAGE_SIZE( p )		( PLANE_SIZE( p ) * (p)->PixelSize )
#define AUDIO_SIZE( p )     ( (p)->AudioSize )
#define CHUNKY_SIZE( p )    ( (p)->XSize * (p)->YSize )
#define	FRAME_SIZE( p )		( PAN_SIZE + CMAP_SIZE( p ) + IMAGE_SIZE( p ) + (p)->AudioSize )

#pragma pack()

#endif
