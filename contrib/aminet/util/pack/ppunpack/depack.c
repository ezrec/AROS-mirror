/* depack.c */

/* copyright (c) Marc Espie, 1995 
 * see accompanying file README for distribution information
 */

#include "depack.h"

#define val(p) ((p)[0]<<16 | (p)[1] << 8 | (p)[2])


ulong  depackedlen(packed, plen)
ubyte *packed;
ulong plen;
   {
	if (packed[0] != 'P' || packed[1] != 'P' || 
		packed[2] != '2' || packed[3] != '0')
			return 0; /* not a powerpacker file */
	return val(packed+plen-4);
   }

static ulong shift_in;
static ulong counter = 0;
static ubyte *source;

static ulong get_bits(ulong n)
	{
	ulong result = 0;
	int i;

	for (i = 0; i < n; i++)
		{
		if (counter == 0)
			{
			counter = 8;
			shift_in = *--source;
			}
		result = (result<<1) | (shift_in & 1);
		shift_in >>= 1;
		counter--;
		}
	return result;
	}

void ppdepack(packed, depacked, plen, unplen)
ubyte *packed, *depacked;
ulong plen, unplen;
   {
	ubyte *dest;
	int n_bits;
	int idx;
	ulong bytes;
	int to_add;
	ulong offset;
   ubyte offset_sizes[4];
	int i;
   
	offset_sizes[0] = packed[4];	/* skip signature */
	offset_sizes[1] = packed[5];
	offset_sizes[2] = packed[6];
	offset_sizes[3] = packed[7];	

		/* initialize source of bits */
	source = packed + plen - 4;

	dest = depacked + unplen;

		/* skip bits */
	get_bits(source[3]);

		/* do it forever, i.e., while the whole file isn't unpacked */
	while (1)
		{
			/* copy some bytes from the source anyway */
		if (get_bits(1) == 0)
			{
			bytes = 0;
			do {
				to_add = get_bits(2);
				bytes += to_add;
				} while (to_add == 3);
			for (i = 0; i <= bytes; i++)
				*--dest = get_bits(8);
			if (dest <= depacked)
				return;
			}
			/* decode what to copy from the destination file */
		idx = get_bits(2);
		n_bits = offset_sizes[idx];
			/* bytes to copy */
		bytes = idx+1;
		if (bytes == 4)	/* 4 means >=4 */
			{
				/* and maybe a bigger offset */
			if (get_bits(1) == 0)
				offset = get_bits(7);
			else
				offset = get_bits(n_bits);

			do {
				to_add = get_bits(3);
				bytes += to_add;
				} while (to_add == 7);
			}
		else
			offset = get_bits(n_bits);
		for (i = 0; i <= bytes; i++)
			{
			dest[-1] = dest[offset];
			dest--;
			}
		if (dest <= depacked)
			return;
		}
	}

