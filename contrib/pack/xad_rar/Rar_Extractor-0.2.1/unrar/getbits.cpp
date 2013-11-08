
// Rar_Extractor 0.2.1. See unrar_license.txt.

#include "rar.hpp"

BitInput::BitInput()
{
  InBuf=new (std::nothrow) byte[MAX_SIZE];
  if ( !InBuf )
  	rar_out_of_memory();
}


BitInput::~BitInput()
{
  delete[] InBuf;
}


void BitInput::faddbits(int Bits)
{
  addbits(Bits);
}


unsigned int BitInput::fgetbits()
{
  return(getbits());
}
