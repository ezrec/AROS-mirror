/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

# ifndef AUDIO_H
# define AUDIO_H

# include "fixed.h"

enum audio_mode {
  AUDIO_MODE_ROUND,
  AUDIO_MODE_DITHER
};

struct audio_dither {
  mad_fixed_t error[3];
  mad_fixed_t random;
};


/*
 * NAME:	clip()
 * DESCRIPTION:	clip samples
 */
static inline
void clip(mad_fixed_t *sample)
{
  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  if (*sample > MAX) {
    *sample = MAX;
  } else if (*sample < MIN) {
    *sample = MIN;
  }
}

/*
 * NAME:	audio_linear_round()
 * DESCRIPTION:	generic linear sample quantize routine
 */
static inline
signed long audio_linear_round(unsigned int bits, mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - bits));

  /* clip */
  clip(&sample);

  /* quantize and scale */
  return sample >> (MAD_F_FRACBITS + 1 - bits);
}

/*
 * NAME:	prng()
 * DESCRIPTION:	32-bit pseudo-random number generator
 */
static inline
unsigned long prng(unsigned long state)
{
  return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

/*
 * NAME:	audio_linear_dither()
 * DESCRIPTION:	generic linear sample quantize and dither routine
 */
static inline
signed long audio_linear_dither(unsigned int bits, mad_fixed_t sample,
				struct audio_dither *dither)
{
  unsigned int scalebits;
  mad_fixed_t output, mask, random;

  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  /* noise shape */
  sample += dither->error[0] - dither->error[1] + dither->error[2];

  dither->error[2] = dither->error[1];
  dither->error[1] = dither->error[0] / 2;

  /* bias */
  output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

  scalebits = MAD_F_FRACBITS + 1 - bits;
  mask = (1L << scalebits) - 1;

  /* dither */
  random  = prng(dither->random);
  output += (random & mask) - (dither->random & mask);

  dither->random = random;

  /* clip */
  if (output > MAX) {
    output = MAX;

    if (sample > MAX)
      sample = MAX;
  } else if (output < MIN) {
    output = MIN;

    if (sample < MIN)
      sample = MIN;
  }

  /* quantize */
  output &= ~mask;

  /* error feedback */
  dither->error[0] = sample - output;

  /* scale */
  return output >> scalebits;
}

# endif
