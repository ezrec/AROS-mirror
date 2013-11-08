
// Rar_Extractor 0.2.1. See unrar_license.txt.

#include "rar.hpp"

static uint CRCTab[256];

static void InitCRC()
{
	for (int I=0;I<256;I++)
	{
		uint C=I;
		for (int J=0;J<8;J++)
			C=(C & 1) ? (C>>1)^0xEDB88320L : (C>>1);
		CRCTab[I]=C;
	}
}

uint CRC(uint StartCRC,const void *Addr,uint Size)
{
	if (CRCTab[1]==0)
		InitCRC();
	byte *Data=(byte *)Addr;
	for (int I=0;I<Size;I++)
		StartCRC=CRCTab[(byte)(StartCRC^Data[I])]^(StartCRC>>8);
	return(StartCRC);
}

ushort OldCRC(ushort StartCRC,const void *Addr,uint Size)
{
	byte *Data=(byte *)Addr;
	for (int I=0;I<Size;I++)
	{
		StartCRC=(StartCRC+Data[I])&0xffff;
		StartCRC=((StartCRC<<1)|(StartCRC>>15))&0xffff;
	}
	return(StartCRC);
}

