/* Bonk - lossy/lossless audio compressor
   Copyright (C) 2001  Paul Francis Harrison

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  675 Mass Ave, Cambridge, MA 02139, USA.

  The author may be contacted at:
    pfh@csse.monash.edu.au
  or
    3 Currajong St., Oakleigh East, 3166, Melbourne, Australia

  See also
    http://yoyo.cc.monash.edu.au/~pfh/
*/

#include "utility.h"

struct lattice {
  int order;
  vector<int> k, state;

  void init(int _order);
  void dequantize();
  void init_state();
  int advance_by_error(int error);
};


#if 0
struct encoder {
  BPTR f_out;
  bitstream_out bit_out;
  int channels, rate;
  int samples_size;
  bool lossless;
  bool mid_side;
  int n_taps;
  int down_sampling, samples_per_packet;
  double quant_level;

  int sample_count; 

  vector< int > tail;
  vector< vector<int> > output_samples;

  bool begin(BPTR _f_out,
       const char *text,
       uint32 length,
       uint32 _rate,
       int _channels,
       bool _lossless,
       bool _mid_side,
       int _n_taps,
       int _down_sampling,
       int _samples_per_packet,
       double _quant_level);
  void finish();
  void store_packet(vector<int> &samples);
};
#endif

struct decoder {
  BPTR f_in;
  bitstream_in bit_in;
  int length, length_remaining, rate;
  int channels;
  bool lossless;
  bool mid_side;
  int n_taps;
  int down_sampling, samples_per_packet;
  double quant_level;

  lattice predictor;
  vector< vector<int> > predictor_initer;

  bool begin(BPTR _f_in);
  void read_packet(vector<int> &samples);
};
