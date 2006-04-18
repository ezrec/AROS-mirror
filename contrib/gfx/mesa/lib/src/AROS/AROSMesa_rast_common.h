#if !defined(AROSMESA_RAST_COMMON_H)
#define AROSMESA_RAST_COMMON_H

//#define DEBUGPRINT 1

#if defined(DEBUGPRINT)
#define DEBUG 1
#endif

#include <aros/debug.h>

#define MAX_POLYGON 300
/* #define AGA 1 */

#ifdef AGA
#define c8to32(x) ((x) << 24)
#else
#define c8to32(x) (((((((x) << 8) | (x)) << 8) | (x)) << 8) | (x))
#endif

/* TrueColor (RGBA) to ColorFrmt Macros */
#define     TC_ARGB(r, g, b, a)           ((((((a << 8) | r) << 8) | g) << 8) | b)
#define     TC_RGBA(r, g, b, a)           ((((((r << 8) | g) << 8) | b) << 8) | a)
#define     PACK_8B8G8R( r, g, b )        (((((b << 8) | g) << 8) | r) << 8)
//#define   PACK_8A8B8G8R( r, g, b, a )

/*
 * Convert Mesa window coordinate(s) to AROS screen coordinate(s):
 */

#define FIXx(x) (amesa->left + (x))

#define FIXy(y) (amesa->RealHeight - amesa->bottom - (y))

#define FIXxy(x,y) ((amesa->RealWidth * FIXy(y) + FIXx(x)))

#endif /* AROSMESA_RAST_COMMON_H */
