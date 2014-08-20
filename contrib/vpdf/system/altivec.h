#ifndef SYS_ALTIVEC_H
#define SYS_ALTIVEC_H

#include <stdlib.h>

extern int hasAltivec;

void AltivecInit(void);

void *vec_memcpy(void *dstpp, const void *srcpp, size_t len);

#define CCSSF_WRITES     (1 << 0UL) /* use when data will be read and written */
#define CCSSF_TRANSCIENT (1 << 1UL) /* use when data should be flushed straight to RAM to preserve L2 */
#define CCSSF_CHANNEL1   (1 << 2UL) /* use channel 1 (0 is default) (XXX: should get rid of it and have some allocchan-like function) */
#define CCSSF_CHANNEL2   (1 << 3UL) /* use channel 2 */
#define CCSSF_CHANNEL3   (1 << 4UL) /* use channel 3 */

void cpu_cache_stream_start(CONST_APTR ptr, ULONG blockcount, ULONG blocksize, LONG stride, ULONG flags);
void cpu_cache_stream_stop(ULONG channel);

void cpu_cache_image_start(void *ptr, int rowbytes, int width, int lines);
void cpu_cache_image_stop();

#if defined(__GNUC__) && __GNUC__ > 2
#include <altivec.h>
#define D_VECF(m1,m2,m3,m4) { m1,m2,m3,m4 }
#define D_VECI(m1,m2,m3,m4) { m1,m2,m3,m4 }
#define D_VECB(m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16) { m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16 }
#else
#define D_VECF(m1,m2,m3,m4) (vector float) ( m1,m2,m3,m4 )
#define D_VECI(m1,m2,m3,m4) (vector unsigned int) ( m1,m2,m3,m4 )
#define D_VECB(m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16) (vector unsigned char) ( m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16 )
#endif

#endif
