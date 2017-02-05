#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <dos/dos.h>
#include <proto/dos.h>

using namespace std;

#ifndef EXEC_TYPES_H
typedef unsigned long  uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;
typedef short          int16;
#endif
typedef unsigned int   uint;

// Endian-safe int readers/writers:

uint8 read_uint8(BPTR f);
uint16 read_uint16(BPTR f);
uint32 read_uint32(BPTR f);
#if 0
void write_uint8(BPTR f, uint8 value);
void write_uint16(BPTR f, uint16 value);
void write_uint32(BPTR f, uint32 value);
#endif

// Throwable error structure:

struct error { 
  char *message; 
  error(char *_message) : message(_message) { } 
};

// Number of bits required to store a value:

int bits_to_store(uint32 i);

// Bitstream reader and writer:

struct bitstream_in {
  BPTR f_in;
  int byte;
  int bit_no;

  void setup(BPTR _f_in);
  int read();
  uint32 read_uint(int bits);
  uint read_uint_max(int max);
};

#if 0
struct bitstream_out {
  BPTR f_out;
  int byte;
  int bytes_written;
  int bit_no;

  void setup(BPTR _f_out);
  void write(int bit);
  void write_uint(uint value,int bits);
  void write_uint_max(uint value,int max);
  void flush();
};
#endif

// Coder/decoder for lists of small signed ints:

void read_list(vector<int> &list,bool base_2_part, bitstream_in &in);
#if 0
void write_list(const vector<int> &list,bool base_2_part, bitstream_out &out);
#endif

#endif //UTILITY_H
