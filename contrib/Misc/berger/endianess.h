#ifndef M68KEMUL_ENDIANESS_H
#define M68KEMUL_ENDIANESS_H
#include <aros/cpu.h>


#if (AROS_BIG_ENDIAN == 0) 
#warning This is a little endian machine!
/* At which offset the BYTE and WORD values are located in a LONG */ 
#define L_BYTE 	0 
#define L_WORD 	0 

#define SWAP(x) \
	(((x >> 24) & 0x000000ff) |\
	 ((x >> 8 ) & 0x0000ff00) |\
	 ((x << 8 ) & 0x00ff0000) |\
	 ((x << 24) & 0xff000000))

#define SWAP_WORD(x) \
	(((x >> 8) & 0x00ff) |\
	 ((x << 8) & 0xff00))

#else
/* 
   At which offset the BYTE and WORD values are located in a LONG; is is
   assumed that a cast to a WORD * or BYTE * is made
*/
#define L_BYTE	3 
#define L_WORD 	1 

#define SWAP(x) x
#define SWAP_WORD(x) x
#endif

#endif
