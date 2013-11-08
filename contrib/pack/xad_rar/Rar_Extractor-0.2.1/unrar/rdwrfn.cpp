
// Rar_Extractor 0.2.1. See unrar_license.txt.

#include "rar.hpp"

ComprDataIO::ComprDataIO()
{
  Init();
}

void ComprDataIO::Init()
{
	SkipUnpCRC = false;
	rar_writer = NULL;
	archive = NULL;
	UnpPackedSize=0;
	UnpVolume=false;
	CurUnpRead=CurUnpWrite=0;
	PackFileCRC=UnpFileCRC=PackedCRC=0xffffffff;
	SubHead=NULL;
	SubHeadPos=NULL;
}

int ComprDataIO::UnpRead(byte *Addr,uint Count)
{
  int RetCode=0,TotalRead=0;
  byte *ReadAddr;
  ReadAddr=Addr;
  while (Count > 0)
  {
    uint ReadSize=(Count>UnpPackedSize) ? int64to32(UnpPackedSize):Count;
      if (!archive->IsOpened())
        return(-1);
      RetCode=archive->Read(ReadAddr,ReadSize);
      FileHeader *hd=SubHead!=NULL ? SubHead:&archive->NewLhd;
      if (hd->Flags & LHD_SPLIT_AFTER)
        PackedCRC=CRC(PackedCRC,ReadAddr,ReadSize);
    CurUnpRead+=RetCode;
    ReadAddr+=RetCode;
    TotalRead+=RetCode;
    Count-=RetCode;
    UnpPackedSize-=RetCode;
    if (UnpPackedSize == 0 && UnpVolume)
    {
      {
        return(-1);
      }
    }
    else
      break;
  }
  if (RetCode!=-1)
  {
    RetCode=TotalRead;
  }
  return(RetCode);
}

void ComprDataIO::UnpWrite(byte *Addr,uint Count)
{
	rar_writer->write(Addr,Count);
  CurUnpWrite+=Count;
  if ( !SkipUnpCRC )
#ifndef SFX_MODULE
    if ( archive->OldFormat)
      UnpFileCRC=OldCRC((ushort)UnpFileCRC,Addr,Count);
    else
#endif
      UnpFileCRC=CRC(UnpFileCRC,Addr,Count);
}

void ComprDataIO::SetFiles( Archive* in, Rar_Writer* out )
{
	archive = in;
	rar_writer = out;
}

