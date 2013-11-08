
// Rar_Extractor 0.2.1. See unrar_license.txt.

#include "rar.hpp"

RawRead::RawRead( Rar_Reader* in ) : rar_reader( in )
{
	ReadPos=0;
	DataSize=0;
}

void RawRead::Read(int Size)
{
	if (Size!=0)
	{
		Data.Add(Size);
		DataSize += rar_reader->read_avail(&Data[DataSize],Size);
	}
}

void RawRead::Get(byte &Field)
{
	Field=Data[ReadPos];
	ReadPos++;
}

void RawRead::Get(ushort &Field)
{
	Field=Data[ReadPos]+(Data[ReadPos+1]<<8);
	ReadPos+=2;
}

void RawRead::Get(uint &Field)
{
	Field=Data[ReadPos]+(Data[ReadPos+1]<<8)+(Data[ReadPos+2]<<16)+
			(Data[ReadPos+3]<<24);
	ReadPos+=4;
}

void RawRead::Get(byte *Field,int Size)
{
	memcpy(Field,&Data[ReadPos],Size);
	ReadPos+=Size;
}

uint RawRead::GetCRC(bool ProcessedOnly)
{
	return(DataSize>2 ? CRC(0xffffffff,&Data[2],(ProcessedOnly ? ReadPos:DataSize)-2):0xffffffff);
}

