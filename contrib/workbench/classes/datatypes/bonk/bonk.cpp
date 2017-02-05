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

const char *version = "0.6";

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
	      
#include "bonk.h"

//Accuracy of fixed point calculations
const int    lattice_shift  = 10,
             sample_shift   = 4,
             lattice_factor = 1<<lattice_shift,
             sample_factor  = 1<<sample_shift,

//Maximum allowable taps
             max_tap        = 2048;
      
//Default quantization level
const double base_quant     = 0.6,

//Amount of bit rate variation
             rate_variation = 3.0;

const int tap_quant[max_tap] = { //int(sqrt(i+1))
   1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,
   4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
   6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
   7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
   9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,10,
  10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
  11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
  11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
  12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,
  13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,
  14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
  14,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,17,17,17,17,17,17,17,17,17,17,17,17,
  17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
  17,17,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
  18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
  19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,
  19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,20,
  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
  21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
  21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
  21,21,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
  22,22,22,22,22,22,22,22,22,22,22,22
};

inline int divide(int a,int b) {
  if (a < 0)
    return -( (-a + b/2)/b );
  else
    return (a + b/2)/b;
}

inline int shift(int a,int b) {
  return a+(1<<b-1) >> b;
}

inline int shift_down(int a,int b) {
  return (a>>b)+(a<0?1:0);
}

void lattice::init(int _order) {
  order = _order;
  k.resize(order);
  state.resize(order);
  for(int i=0;i<order;i++) {
    state[i] = 0;
    k[i] = 0;
  }
}

void lattice::dequantize() { 
  for(int i=0;i<order;i++)
    k[i] *= tap_quant[i];
}

void lattice::init_state() {
  for(int i=order-2;i>=0;i--) {
    int x = state[i], new_x;
    for(int j=0,p=i+1;p<order;j++,p++) {
      new_x = x + shift_down(k[j]*state[p],lattice_shift);
      state[p] += shift_down(k[j]*x,lattice_shift);
      x = new_x;
    }
  }
}

int lattice::advance_by_error(int error) { // returns value
  int x = error;
  x -= shift_down(k[order-1]*state[order-1],lattice_shift);

  int *k_ptr     = &(k[order-2]),
      *state_ptr = &(state[order-2]);
  for(int i=order-2;i>=0;i--,k_ptr--,state_ptr--) {
    int k_value     = *k_ptr,
        state_value = *state_ptr;
    x -= shift_down(k_value*state_value,lattice_shift);
    state_ptr[1] = state_value+shift_down(k_value*x,lattice_shift);
  }

  //Don't drift too far, to avoid overflows 
  if (x >  (sample_factor<<16)) x =  (sample_factor<<16);
  if (x < -(sample_factor<<16)) x = -(sample_factor<<16);
    
  state[0] = x;

  return x;
}

// Heavily modified Levinson-Durbin algorithm which
// copes better with quantization, and calculates the
// actual whitened result as it goes.

void modified_levinson_durbin(vector<int> &x, 
                              int channels, bool lossless,
                              vector<int> &k_out) {
  vector<int> state = x;

  for(int i=0;i<k_out.size();i++) {
    int step = (i+1)*channels;

    double xx=0.0, xy=0.0;
    int n         = x.size()-step,
       *x_ptr     = &(x[step]),
       *state_ptr = &(state[0]);
    for(;n>=0;n--,x_ptr++,state_ptr++) {
      double x_value     = *x_ptr,
             state_value = *state_ptr;
	     
      xx += state_value*state_value;
      xy += x_value*state_value;
    }

    int k;
    if (xx == 0.0)
      k = 0;
    else
      k = int(floor( -xy/xx *double(lattice_factor)/double(tap_quant[i]) +0.5 ));

    if (k  > lattice_factor/tap_quant[i]) k =   lattice_factor/tap_quant[i];
    if (-k > lattice_factor/tap_quant[i]) k = -(lattice_factor/tap_quant[i]);

    k_out[i] = k;
    k *= tap_quant[i];

    n         = x.size()-step;
    x_ptr     = &(x[step]);
    state_ptr = &(state[0]);
    for(;n>=0;n--,x_ptr++,state_ptr++) {
      int x_value     = *x_ptr,
          state_value = *state_ptr;
      *x_ptr     = x_value     + shift_down(k*state_value,lattice_shift);
      *state_ptr = state_value + shift_down(k*x_value,lattice_shift);
    }
  }
}

#if 0

bool encoder::begin(BPTR _f_out,
     const char *text,
     uint32 length,
     uint32 _rate,
     int _channels,
     bool _lossless,
     bool _mid_side,
     int _n_taps,
     int _down_sampling,
     int _samples_per_packet,
     double _quant_level) {
  f_out = _f_out;
  channels = _channels;
  rate = _rate;
  lossless = _lossless;
  mid_side = _mid_side;
  n_taps = _n_taps;
  down_sampling = _down_sampling;
  samples_per_packet = _samples_per_packet;
  quant_level = _quant_level;

  if (n_taps > max_tap)
    return false;

  if (mid_side && channels <= 1)
    mid_side = false;

  if (samples_per_packet <= 0)
    return false;

  IDOS->FWrite(f_out,text,1,strlen(text));

  IDOS->FPutC(f_out,0);
  IDOS->FPutC(f_out,'B');
  IDOS->FPutC(f_out,'O');
  IDOS->FPutC(f_out,'N');
  IDOS->FPutC(f_out,'K');

  write_uint8 (f_out, 0); // version
  write_uint32(f_out, length);
  write_uint32(f_out, rate);
  write_uint8 (f_out, channels);
  write_uint8 (f_out, lossless?1:0);
  write_uint8 (f_out, mid_side?1:0);
  write_uint16(f_out, n_taps);
  write_uint8 (f_out, down_sampling);
  write_uint16(f_out, samples_per_packet);

  tail.resize(n_taps*channels);
  for(int i=0;i<tail.size();i++) 
    tail[i] = 0;

  output_samples.resize(channels);
  for(int i=0;i<channels;i++)
    output_samples[i].resize(samples_per_packet);

  samples_size = channels*samples_per_packet*down_sampling;
  
  sample_count = 0;

  bit_out.setup(f_out);
  return true;
}

void encoder::finish() {
  bit_out.flush();
}

void encoder::store_packet(vector<int> &samples) {
  //samples must be correct size (samples_size)

  if (!lossless)
    for(int i=0;i<samples.size();i++)
      samples[i] <<= sample_shift;

  if (mid_side)
    for(int i=0;i<samples.size();i+=channels) {
      samples[i]   += samples[i+1];
      samples[i+1] -= shift(samples[i],1);
    }  
 
  vector<int> window(tail.size()*2+samples_size);
  int *ptr = &(window[0]);

  for(int i=0;i<tail.size();i++)
    *(ptr++) = tail[i];

  for(int i=0;i<samples_size;i++)
    *(ptr++) = samples[i];
   
  for(int i=0;i<tail.size();i++)
    *(ptr++) = 0;
    
  for(int i=0;i<tail.size();i++)
    tail[i] = samples[samples_size-tail.size()+i];
    
  vector<int> k(n_taps);
  modified_levinson_durbin(window,channels,lossless,k);

  write_list(k,false, bit_out); 

  for(int channel=0;channel<channels;channel++) {
    ptr = &(window[tail.size()+channel]);
    for(int i=0;i<samples_per_packet;i++) {
      int sum = 0;
      for(int j=0;j<down_sampling;j++,ptr += channels)
        sum += *ptr;
      output_samples[channel][i] = sum;
    }
  }

  int quant;
  if (!lossless) {
    double energy2 = 0.0;
    double energy1 = 0.0;
    for(int channel=0;channel<channels;channel++) 
      for(int i=0;i<samples_per_packet;i++) {
        double sample = output_samples[channel][i];
        energy2 += sample*sample;
        energy1 += fabs(sample);
      }

    energy2 = sqrt(energy2/(channels*samples_per_packet));
    energy1 = sqrt(2.0)*energy1/(channels*samples_per_packet);

    //Increase bitrate when samples are like a gaussian distribution 
    //Reduce bitrate when samples are like a two-tailed exponentional distribution 

    if (energy2 > energy1)
      energy2 += (energy2-energy1)*rate_variation;
      
    quant = int(base_quant*quant_level*energy2/sample_factor);

    if (quant < 1) 
      quant = 1;
    if (quant > 65535)
      quant = 65535;

    bit_out.write_uint(quant,16);

    quant *= sample_factor;
  }

  for(int channel=0;channel<channels;channel++) {
    if (!lossless)
      for(int i=0;i<samples_per_packet;i++)
        output_samples[channel][i] = divide(output_samples[channel][i],quant);

    write_list(output_samples[channel],true, bit_out);
  }

  sample_count += samples_size;
}

#endif

int _feof(BPTR f) {
	int byte;
	IDOS->UnGetC(f, byte = IDOS->FGetC(f));
	return byte == -1;
}

bool decoder::begin(BPTR _f_in) {
  f_in = _f_in;

  vector<char> buffer;
  for(int i=0;i<5;i++)
    buffer.push_back(IDOS->FGetC(f_in));

  for(;;) {
    if (buffer[buffer.size()-5] == 0   &&
        buffer[buffer.size()-4] == 'B' &&
        buffer[buffer.size()-3] == 'O' &&
        buffer[buffer.size()-2] == 'N' &&
        buffer[buffer.size()-1] == 'K')
		break;

    buffer.push_back(IDOS->FGetC(f_in));

    if (_feof(f_in) || buffer.size() > 1000000)
      return false;
  }

  //fprintf(stderr,"%s",&(buffer[0]));

  if (read_uint8(f_in) != 0) // version
    return false;

  length = read_uint32(f_in);
  rate = read_uint32(f_in); 
  channels = read_uint8 (f_in);
  lossless = read_uint8 (f_in);
  mid_side = read_uint8 (f_in);
  n_taps = read_uint16(f_in);
  down_sampling = read_uint8(f_in);
  samples_per_packet = read_uint16(f_in);

  if (channels == 0 ||
     (channels<2 && mid_side) ||
     n_taps > max_tap ||
     n_taps == 0 ||
     down_sampling == 0 ||
     samples_per_packet == 0)
     return false;

  predictor.init(n_taps);
  predictor_initer.resize(channels);
  for(int i=0;i<channels;i++) {
    predictor_initer[i].resize(n_taps);
    for(int j=0;j<n_taps;j++)
      predictor_initer[i][j] = 0;
  }

  length_remaining = length;

  bit_in.setup(f_in);
  return true;
}

void decoder::read_packet(vector<int> &samples) {
  samples.resize(samples_per_packet*down_sampling*channels);

  vector<int> input_samples(samples_per_packet);

  read_list(predictor.k,false, bit_in);

  predictor.dequantize();

  int quant = (lossless?1:bit_in.read_uint(16)*sample_factor);
 
  for(int channel=0;channel<channels;channel++) {
    int *sample = &(samples[channel]);

    predictor.state = predictor_initer[channel];
    predictor.init_state();

    read_list(input_samples,true, bit_in);
      
    for(int i=0;i<samples_per_packet;i++) {
      for(int j=0;j<down_sampling-1;j++) {
        *sample = predictor.advance_by_error(0);
	    sample += channels;
	  }

      *sample = predictor.advance_by_error(input_samples[i]*quant);
      sample += channels;
    }

    for(int i=0;i<n_taps;i++)
      predictor_initer[channel][i] = 
      samples[samples.size()-channels+channel-i*channels];
  }

  if (mid_side)
    for(int i=0;i<samples.size();i+=channels) {
      samples[i+1] += shift(samples[i],1);
      samples[i]   -= samples[i+1];
    }  
    
  if (!lossless)
    for(int i=0;i<samples.size();i++)
      samples[i] = shift(samples[i],sample_shift);

  if (length_remaining < samples.size()) {
    samples.resize(length_remaining);
    length_remaining = 0;
  } else
    length_remaining -= samples.size();
}
