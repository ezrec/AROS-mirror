
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef _RAR_RAWREAD_
#define _RAR_RAWREAD_

class RawRead
{
  private:
    Rar_Array<byte> Data;
    Rar_Reader* rar_reader;
    int DataSize;
    int ReadPos;
  public:
    RawRead( Rar_Reader* );
    void Read(int Size);
    void Read(byte *SrcData,int Size);
    void Get(byte &Field);
    void Get(ushort &Field);
    void Get(uint &Field);
    void Get8(Int64 &Field);
    void Get(byte *Field,int Size);
    uint GetCRC(bool ProcessedOnly);
    int Size() {return DataSize;}
    int PaddedSize() {return Data.Size()-DataSize;}
};

#endif
