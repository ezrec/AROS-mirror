/* prototypes for unpacker functions */

/* copyright (c) Marc Espie, 1995 
 * see accompanying file README for distribution information
 */

typedef unsigned char ubyte;
typedef unsigned long ulong;

extern ulong depackedlen(ubyte *packed, ulong plen);
extern void ppdepack(ubyte *packed, ubyte *depacked, ulong plen, ulong unplen);

