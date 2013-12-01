// NewIconDt.h
// $Date$
// $Revision$


#ifndef NEWICONDT_H
#define	NEWICONDT_H

//-----------------------------------------------------------------------------

#include <libraries/newicon.h>
#include <scalos/scalosgfx.h>

//-----------------------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_PUBLIC
#define	MEMPOOL_PUDDLESIZE	16384
#define	MEMPOOL_THRESHSIZE	16384

//-----------------------------------------------------------------------------

struct NewIconDtLibBase
	{
	struct ClassLibrary nib_ClassLibrary;

	struct SegList *nib_SegList;

	UBYTE nib_Initialized;
	UBYTE nib_NewIconsTransparent;
	ULONG nib_NewIconsPrecision;
	};

//-----------------------------------------------------------------------------

struct ExtChunkyImage
{
	struct ChunkyImage *eci_Chunky;		// original ChunkyImage
	ULONG eci_TransparentColor;		// Index of transparent color - always 0 for standard newicons
};

//-----------------------------------------------------------------------------

struct InstanceData
	{
	struct DiskObject *nio_icon;

	struct ExtChunkyImage nio_normchunky;
	struct ExtChunkyImage nio_selchunky;

	struct ExtChunkyImage *nio_AllocNormChunky;	// optional self-allocated normal ChunkyImage
	struct ExtChunkyImage *nio_AllocSelChunky;	// optional self-allocated selected ChunkyImage

	struct Image *nio_normimage;	// Remapped normal image
	struct Image *nio_selimage;	// Remapped selected image

	struct Screen *nio_imgscreen;

	UBYTE *nio_normpens;		// Pen array for remapped normal image
	UBYTE *nio_selpens;		// Pen array for remapped selected image

	struct NewDiskObject *nio_newicon;

	BOOL nio_DoNotFreeDiskObject;
	};

//-----------------------------------------------------------------------------

#define	Sizeof(array)	(sizeof(array) / sizeof((array)[0]))

// Width for TempRp.BitMap for ReadPixelLine8() and WritePixelLine8()
#define	TEMPRP_WIDTH(width)	(8 * ((((width) + 15) >> 4) << 1))

//-----------------------------------------------------------------------------

#define	d1(x)	;
#define	d2(x)	{ Forbid(); x; Permit(); }

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//-----------------------------------------------------------------------------

#endif /* NEWICONDT_H */
