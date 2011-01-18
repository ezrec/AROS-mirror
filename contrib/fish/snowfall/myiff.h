/*  :ts=8 bk=0
 *
 * myiff.h:	Definitions for my IFF reader.
 *
 * Leo L. Schwab			8705.11
 */

/*  Masking techniques  */
#define	mskNone			0
#define	mskHasMask		1
#define	mskHasTransparentColor	2
#define	mskLasso		3

/*  Compression techniques  */
#define	cmpNone			0
#define	cmpByteRun1		1

/*  Bitmap header (BMHD) structure  */
struct BitMapHeader {
	UWORD	w, h;		/*  Width, height in pixels */
	WORD	x, y;		/*  x, y position for this bitmap  */
	UBYTE	nplanes;	/*  # of planes  */
	UBYTE	Masking;
	UBYTE	Compression;
	UBYTE	pad1;
	UWORD	TransparentColor;
	UWORD	XAspect, YAspect;
	WORD	PageWidth, PageHeight;
};

/*  Color register structure (not really used)  */
struct ColorRegister {
	UBYTE red, green, blue;
};

/*  Makes my life easier.  */
union typekludge {
	char type_str[4];
	long type_long;
};

struct ChunkHeader {
	union typekludge chunktype;
	long chunksize;
};
#define	TYPE		chunktype.type_long
#define	STRTYPE		chunktype.type_str


/*  Useful macro from EA (the only useful thing they ever made)  */
#define MAKE_ID(a, b, c, d)\
	( ((long)(a)<<24) + ((long)(b)<<16) + ((long)(c)<<8) + (long)(d) )

/*  IFF types we may encounter  */
#define	FORM	MAKE_ID('F', 'O', 'R', 'M')
#define	ILBM	MAKE_ID('I', 'L', 'B', 'M')
#define	BMHD	MAKE_ID('B', 'M', 'H', 'D')
#define	CMAP	MAKE_ID('C', 'M', 'A', 'P')
#define	BODY	MAKE_ID('B', 'O', 'D', 'Y')
#define	CAMG	MAKE_ID('C', 'A', 'M', 'G')
/* we won't bother with these */
#define	GRAB	MAKE_ID('G', 'R', 'A', 'B')
/* #define	DEST	MAKE_ID('D', 'E', 'S', 'T')
	Only done to avoid collision with blit.h */
#define	SPRT	MAKE_ID('S', 'P', 'R', 'T')
#define	CRNG	MAKE_ID('C', 'R', 'N', 'G')
#define	CCRT	MAKE_ID('C', 'C', 'R', 'T')
#define	DPPV	MAKE_ID('D', 'P', 'P', 'V')
#define	DPPS	MAKE_ID('D', 'P', 'P', 'S')


/*  Other useful things.  */
#define	CHUNKHEADERSIZE		sizeof (struct ChunkHeader)
#define	SUBTYPESIZE		sizeof (long)



