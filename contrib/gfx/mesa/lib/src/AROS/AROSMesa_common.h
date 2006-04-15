#if !defined(AROSMESA_COMMON_H)
#define AROSMESA_COMMON_H

//#define DEBUG 0
#include <aros/debug.h>

#define MAX_POLYGON 300
/* #define AGA 1 */

#ifdef AGA
#define c8to32(x) ((x) << 24)
#else
#define c8to32(x) (((((((x) << 8) | (x)) << 8) | (x)) << 8) | (x))
#endif

/* TrueColor-RGBA */
#define     TC_ARGB(r, g, b, a)           ((((((a << 8) | r) << 8) | g) << 8) | b)
#define     TC_RGBA(r, g, b, a)           ((((((r << 8) | g) << 8) | b) << 8) | a)
#define     PACK_8B8G8R( R, G, B )        ( ((B) << 16) | ((G) << 8) | (R) )
//#define   PACK_8A8B8G8R( R, G, B, A )

/*
 * Convert Mesa window coordinate(s) to AROS screen coordinate(s):
 */

#define FIXx(x) (amesa->left + (x))

#define FIXy(y) (amesa->RealHeight - amesa->bottom - (y))

#define FIXxy(x,y) ((amesa->RealWidth * FIXy(y) + FIXx(x)))

#endif /* AROSMESA_COMMON_H */
