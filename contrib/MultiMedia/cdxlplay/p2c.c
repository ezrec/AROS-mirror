/*
**  Planar 2 Chunky routine (C) 2009 Fredrik Wikstrom
**/

#include "p2c.h"

#if (defined(__PPC__) || defined(__M68K__)) && !defined(WORDS_BIGENDIAN)
#define WORDS_BIGENDIAN 1
#endif

#include "p2c/p2c_1.c"
#include "p2c/p2c_2.c"
#include "p2c/p2c_3.c"
#include "p2c/p2c_4.c"
#include "p2c/p2c_5.c"
#include "p2c/p2c_6.c"
#include "p2c/p2c_7.c"
#include "p2c/p2c_8.c"

typedef void (*p2c_func_ptr)(const uint16_t *planar, uint32_t plane_size, uint8_t *chunky,
	uint32_t width, uint32_t height);

static const p2c_func_ptr p2c_funcs[] = {
	p2c_1, p2c_2, p2c_3, p2c_4, p2c_5, p2c_6, p2c_7, p2c_8
};

void planar2chunky (const uint16_t *planar, uint32_t plane_size, uint8_t *chunky,
	uint32_t width, uint32_t height, uint32_t depth)
{
	p2c_funcs[depth-1](planar, plane_size, chunky, width >> 4, height);
}
