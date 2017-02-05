#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>

#if (defined(__arm__) && !defined(__ARMEB__)) || defined(__i386__) || defined(__i860__) || defined(__ns32000__) || defined(__vax__) || defined(__amd64__) || defined(__x86_64__)
#undef WORDS_BIGENDIAN
#elif defined(__sparc__) || defined(__alpha__) || defined(__PPC__) || defined(__mips__) || defined(__ppc__) || defined(__M68K__) || defined(__BIG_ENDIAN__)
#define WORDS_BIGENDIAN 1
#endif

#define swap16(x) ((((uint16_t)(x) & 0x00ff)<<8)|(((uint16_t)(x) & 0xff00)>>8))
#define swap32(x) ((((uint32_t)(x) & 0x000000ff)<<24)| \
	(((uint32_t)(x) & 0x0000ff00)<<8)| \
	(((uint32_t)(x) & 0x00ff0000)>>8)| \
	(((uint32_t)(x) & 0xff000000)>>24))
#define swap64(x) ((uint64_t)swap32((uint64_t)(x) >> 32)|((uint64_t)swap32(x) << 32))

#ifdef WORDS_BIGENDIAN
#define h2be16(x) (x)
#define h2be32(x) (x)
#define h2be64(x) (x)
#define h2le16(x) swap16(x)
#define h2le32(x) swap32(x)
#define h2le64(x) swap64(x)
#else
#define h2le16(x) (x)
#define h2le32(x) (x)
#define h2le64(x) (x)
#define h2be16(x) swap16(x)
#define h2be32(x) swap32(x)
#define h2be64(x) swap64(x)
#endif

#define rbe16(p) h2be16(*(uint16_t *)(p))
#define rbe32(p) h2be32(*(uint32_t *)(p))
#define rbe64(p) h2be64(*(uint64_t *)(p))
#define rle16(p) h2le16(*(uint16_t *)(p))
#define rle32(p) h2le32(*(uint32_t *)(p))
#define rle64(p) h2le64(*(uint64_t *)(p))
#define wbe16(p,x) (*(uint16_t *)(p) = h2be16(x))
#define wbe32(p,x) (*(uint32_t *)(p) = h2be32(x))
#define wbe64(p,x) (*(uint64_t *)(p) = h2be64(x))
#define wle16(p,x) (*(uint16_t *)(p) = h2le16(x))
#define wle32(p,x) (*(uint32_t *)(p) = h2le32(x))
#define wle64(p,x) (*(uint64_t *)(p) = h2le64(x))

#if defined(__GNUC__) && defined(__PPC__)
#undef swap16
#undef swap32
#undef swap64

static inline uint32_t swap16(uint16_t x) {
	uint32_t res;
	asm("rlwinm %0,%1,8,16,23;"
		"rlwimi %0,%1,24,24,31;"
		: "=&r" (res)
		: "r" (x));
	return res;
}

static inline uint32_t swap32(uint32_t x) {
	uint32_t res;
	asm("rlwinm %0,%1,24,0,31;"
		"rlwimi %0,%1,8,8,15;"
		"rlwimi %0,%1,8,24,31;"
		: "=&r" (res)
		: "r" (x));
	return res;
}

static inline uint64_t swap64(uint64_t x) {
	uint64_t res;
	asm("rlwinm %L0,%M1,24,0,31;"
		"rlwinm %M0,%L1,24,0,31;"
		"rlwimi %L0,%M1,8,8,15;"
		"rlwimi %M0,%L1,8,8,15;"
		"rlwimi %L0,%M1,8,24,31;"
		"rlwimi %M0,%L1,8,24,31;"
		: "=&r" (res)
		: "r" (x));
	return res;
}

#undef rle16
#undef rle32
#undef rle64

static inline uint32_t rle16(void *p) {
	uint32_t res;
	asm("lhbrx %0,0,%1;"
		: "=r" (res)
		: "r" (p));
	return res;
}

static inline uint32_t rle32(void *p) {
	uint32_t res;
	asm("lwbrx %0,0,%1;"
		: "=r" (res)
		: "r" (p));
	return res;
}

static inline uint64_t rle64(void *p) {
	uint64_t res;
	const uint32_t y = 4;
	asm("lwbrx %L0,0,%1;"
		"lwbrx %M0,%2,%1;"
		: "=&r" (res)
		: "r" (p), "r" (y));
	return res;
}

#undef wle16
#undef wle32
#undef wle64

static inline void wle16(void *p, uint16_t x) {
	asm("sthbrx %1,0,%0;"
		:
		: "r" (p), "r" (x));
}

static inline void wle32(void *p, uint32_t x) {
	asm("stwbrx %1,0,%0;"
		:
		: "r" (p), "r" (x));
}

static inline void wle64(void *p, uint64_t x) {
	const uint32_t y = 4;
	asm("stwbrx %L1,0,%0;"
		"stwbrx %M1,%2,%0;"
		:
		: "r" (p), "r" (x), "r" (y));
}
#endif

#endif
