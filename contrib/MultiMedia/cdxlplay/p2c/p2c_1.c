/*
**  Planar 2 Chunky routine (C) 2009 Fredrik Wikstrom
**/

static void p2c_1 (const uint16_t *planar, uint32_t plane_size, uint8_t *chunky,
	uint32_t width, uint32_t height)
{
	uint32_t *cp;
	const uint16_t *pp1;
	uint32_t cd1, cd2, cd3, cd4;
	uint32_t pd1;
	uint32_t x, y;
	cp = (uint32_t *)chunky;
	pp1 = planar;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pd1 = *pp1++;
			
#ifdef WORDS_BIGENDIAN
			cd1  = ((pd1 << 9 ) & 0x01000000);
			
			cd2  = ((pd1 << 13) & 0x01000000);
			
			cd3  = ((pd1 << 17) & 0x01000000);
			
			cd4  = ((pd1 << 21) & 0x01000000);
			
			cd1 |= ((pd1 << 2 ) & 0x00010000);
			
			cd2 |= ((pd1 << 6 ) & 0x00010000);
			
			cd3 |= ((pd1 << 10) & 0x00010000);
			
			cd4 |= ((pd1 << 14) & 0x00010000);
			
			cd1 |= ((pd1 >> 5 ) & 0x00000100);
			
			cd2 |= ((pd1 >> 1 ) & 0x00000100);
			
			cd3 |= ((pd1 << 3 ) & 0x00000100);
			
			cd4 |= ((pd1 << 7 ) & 0x00000100);
			
			cd1 |= ((pd1 >> 12) & 0x00000001);
			
			cd2 |= ((pd1 >> 8 ) & 0x00000001);
			
			cd3 |= ((pd1 >> 4 ) & 0x00000001);
			
			cd4 |= ((pd1      ) & 0x00000001);
#else
			cd1  = ((pd1 >> 7 ) & 0x00000001);
			
			cd2  = ((pd1 >> 3 ) & 0x00000001);
			
			cd3  = ((pd1 >> 15) & 0x00000001);
			
			cd4  = ((pd1 >> 11) & 0x00000001);
			
			cd1 |= ((pd1 << 2 ) & 0x00000100);
			
			cd2 |= ((pd1 << 6 ) & 0x00000100);
			
			cd3 |= ((pd1 >> 6 ) & 0x00000100);
			
			cd4 |= ((pd1 >> 2 ) & 0x00000100);
			
			cd1 |= ((pd1 << 11) & 0x00010000);
			
			cd2 |= ((pd1 << 15) & 0x00010000);
			
			cd3 |= ((pd1 << 3 ) & 0x00010000);
			
			cd4 |= ((pd1 << 7 ) & 0x00010000);
			
			cd1 |= ((pd1 << 20) & 0x01000000);
			
			cd2 |= ((pd1 << 24) & 0x01000000);
			
			cd3 |= ((pd1 << 12) & 0x01000000);
			
			cd4 |= ((pd1 << 16) & 0x01000000);
#endif
			
			*cp++ = cd1;
			*cp++ = cd2;
			*cp++ = cd3;
			*cp++ = cd4;
		}
	}
}
