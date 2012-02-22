/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2003 Robert Leslie
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
 * $Id$
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include "filter.h"
# include "fixed.h"
# include "frame.h"

/*
 * NAME:	gain_filter()
 * DESCRIPTION:	perform attenuation or amplification
 */
void gain_filter(mad_fixed_t gain, struct mad_frame *frame)
{
  if (gain != MAD_F_ONE) {
    unsigned int nch, ch, ns, s, sb;

    nch = MAD_NCHANNELS(&frame->header);
    ns  = MAD_NSBSAMPLES(&frame->header);

    for (ch = 0; ch < nch; ++ch) {
      for (s = 0; s < ns; ++s) {
	for (sb = 0; sb < 32; ++sb) {
	  frame->sbsample[ch][s][sb] =
	    mad_f_mul(frame->sbsample[ch][s][sb], gain);
	}
      }
    }
  }
}
