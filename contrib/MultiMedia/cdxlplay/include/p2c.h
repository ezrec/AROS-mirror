/*
**  Planar 2 Chunky routine (C) 2009 Fredrik Wikstrom
**/

#ifndef P2C_H
#define P2C_H

#include <stdint.h>

void planar2chunky (const uint16_t *planar, uint32_t plane_size, uint8_t *chunky,
	uint32_t width, uint32_t height, uint32_t depth);

#endif
