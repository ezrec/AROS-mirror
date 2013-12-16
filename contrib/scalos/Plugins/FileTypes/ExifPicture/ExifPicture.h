// ExifPicture.h
// $Date$
// $Revision$

#ifndef EXIFPICTURE_H
#define EXIFPICTURE_H

int ExifPictureInit(struct ExifPictureBase *ExifPictureBase);
int ExifPictureOpen(struct ExifPictureBase *ExifPictureBase);
void ExifPictureCleanup(struct ExifPictureBase *ExifPictureBase);
LIBFUNC_P3_PROTO(STRPTR, LIBToolTipInfoString,
	A0, struct ScaToolTipInfoHookData *, ttshd,
	A1, CONST_STRPTR, args,
	A6, struct ExifPictureBase *, ExifPictureBase, 5);

//---------------------------------------------------------------

// Debugging...
#define	d(x)	;
#define	d1(x)	;
#define	d2(x)	x;

// aus debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//---------------------------------------------------------------

struct ExifPicturePlugin_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif /* EXIFPICTURE_H */

