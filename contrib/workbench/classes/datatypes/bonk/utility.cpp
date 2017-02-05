#include "utility.h"

using namespace std;

// Endian-safe int readers/writers:

uint8 read_uint8(BPTR f) {
  return uint8(IDOS->FGetC(f));
}

uint16 read_uint16(BPTR f) {
  uint16 result = read_uint8(f);
  result += uint16(read_uint8(f))<<8;

  return result;
}

uint32 read_uint32(BPTR f) {
  uint32 result = read_uint8(f);
  result += uint32(read_uint8(f))<<8;
  result += uint32(read_uint8(f))<<16;
  result += uint32(read_uint8(f))<<24;

  return result;
}

#if 0
void write_uint8(BPTR f, uint8 value) {
  IDOS->FPutC(f, value);
}

void write_uint16(BPTR f, uint16 value) {
  write_uint8(f,value); 
  write_uint8(f,value>>8); 
}

void write_uint32(BPTR f, uint32 value) {
  write_uint8(f,value); 
  write_uint8(f,value>>8); 
  write_uint8(f,value>>16);
  write_uint8(f,value>>24);
}
#endif

// Number of bits required to store a value:

int bits_to_store(uint32 i) {
  int result = 0;
  while(i) {
    result++;
    i >>= 1;
  }
  return result;
}

// Bitstream reader and writer:

void bitstream_in::setup(BPTR _f_in) {
  f_in = _f_in;
  bit_no  = 8;
}

int bitstream_in::read() {
  if (bit_no == 8) {
    byte = IDOS->FGetC(f_in);
    if (byte == EOF)
      throw error("Unexpected end of file");
      
    bit_no = 0;
  }

  return (byte & (1<<bit_no++) ? 1 : 0);
}

uint32 bitstream_in::read_uint(int bits) {
  uint32 value = 0;
  for(int i=0;i<bits;i++)
    value += read()<<i;
  return value;
}

uint bitstream_in::read_uint_max(int max) {
  if (!max) return 0;
  int bits = bits_to_store(max);

  uint value = 0;
  for(int i=0;i<bits-1;i++)
    if (read())
      value += 1<<i;
   
  if ( (value | (1<<(bits-1))) <= max )
    if (read())
      value += 1<<(bits-1);

  return value;
}

#if 0
void bitstream_out::setup(BPTR _f_out) {
  f_out = _f_out;
  byte = 0;
  bytes_written = 0;
  bit_no = 0;
}

void bitstream_out::write(int bit) {
  if (bit)
    byte = (byte | (1<<bit_no));

  bit_no++;

  if (bit_no == 8) {
    IDOS->FPutC(f_out,byte);
    bytes_written++;
    byte = 0;
    bit_no = 0;
  }
}

void bitstream_out::write_uint(uint value,int bits) {
  for(int i=0;i<bits;i++)
    write(value & (1<<i));
}

void bitstream_out::write_uint_max(uint value,int max) {
  if (!max) return;
  int bits = bits_to_store(max);
  for(int i=0;i<bits-1;i++)
    write(value & (1<<i));
   
  if ( (value | (1<<(bits-1))) <= max )
    write(value & (1<<(bits-1)));
}

void bitstream_out::flush() {
  while(bit_no != 0) 
    write(0);
}
#endif

// Coder/decoder for lists of small signed ints:

const int adapt_level = 8;

#if 0
void write_list(const vector<int> &list,bool base_2_part, bitstream_out &out) {
  //Store a list of integers concisely

  //If large magnitude, store low order bits in base 2

  int low_bits = 0;

  if (base_2_part) {
    int energy = 0;
    for(int i=0;i<list.size();i++)
      energy += abs(list[i]);
  
    low_bits = bits_to_store(energy/(list.size()*2));
    if (low_bits > 15) low_bits = 15;

    out.write_uint(low_bits, 4);
  }

  vector<int> copy(list.size());
  for(int i=0;i<list.size();i++) {
    out.write_uint(abs(list[i]),low_bits);
    copy[i] = abs(list[i]) >> low_bits;
  }

  //Convert list into bitstream:
  
  vector<uint8> bits;

  int max = 0;
  for(int i=0;i<copy.size();i++)
    if (copy[i] > max)
      max = abs(copy[i]);
  
  for(int i=0;i<=max;i++) 
    for(int j=0;j<copy.size();j++)
      if (copy[j] >= i)
        bits.push_back(copy[j] > i);
  
  // Store bitstream:
  
  int step = 256;
  int pos = 0;
  bool dominant = false;
  while(pos < bits.size()) {
    int steplet = step>>8;
    if (pos+steplet > bits.size())
      steplet = bits.size()-pos;

    bool any = false;;
    for(int i=0;i<steplet;i++)
      if (bits[i+pos] != dominant)
        any = true;

    out.write(any);

    if (!any) {
      pos  += steplet;
      step += step / adapt_level;
    } else {
      int interloper=0;
      while(bits[pos+interloper] == dominant) interloper++;

      //Note change.
      out.write_uint_max(interloper,(step>>8)-1);
      pos += interloper+1;
      step -= step / adapt_level;
    }

    if (step < 256) {
      step = 65536 / step;
      dominant = !dominant;
    }
  }

  // Store signs:

  for(int i=0;i<list.size();i++)
    if (list[i])
      out.write(list[i]<0);
}
#endif

void read_list(vector<int> &list,bool base_2_part, bitstream_in &in) {
  //Read a list of ints as encoded by "write_list"

  //Read base 2 part:

  int low_bits = (base_2_part ? in.read_uint(4) : 0);

  for(int i=0;i<list.size();i++) 
    list[i] = in.read_uint(low_bits);
    
  // Decode bitstream:

  int n_zeros = 0;
  int step = 256;
  bool dominant = false;
  vector<uint8> bits;
  while(n_zeros < list.size()) {
    int steplet = step>>8;
  
    if (!in.read()) {
      for(int i=0;i<steplet;i++)
        bits.push_back(dominant);
      
      if (!dominant)
        n_zeros += steplet;
	
      step += step / adapt_level;
    } else {
      int actual_run = in.read_uint_max(steplet-1);
      
      for(int i=0;i<actual_run;i++)
        bits.push_back(dominant);
      bits.push_back(!dominant);

      if (!dominant)
        n_zeros += actual_run;
      if (dominant)
        n_zeros++;
    
      step -= step / adapt_level;
    }

    if (step < 256) {
      step = 65536 / step;
      dominant = !dominant;
    }
  }

  // Reconstruct (unsigned) values:

  n_zeros = 0;
  int pos   = 0;
  int level = 0;
  for(int i=0;n_zeros<list.size();i++) {
    for(;;) {
      if (pos >= list.size()) {
        pos = 0;
	level += 1<<low_bits;
      }

      if (list[pos] >= level)
        break;

      pos++;
    }
      
    if (bits[i])
      list[pos] += 1<<low_bits;
    else
      n_zeros++;

    pos++;
  }

  // Read signs:

  for(int i=0;i<list.size();i++)
    if (list[i] && in.read())
      list[i] = -list[i];
}
