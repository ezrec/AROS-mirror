unsigned int UnpRead(unsigned char *Addr,unsigned int Count)
{
  unsigned int I,RetCode=0,ReadSize,TotalRead=0;
  unsigned char *ReadAddr;
  ReadAddr=Addr;
  while (Count > 0)
  {
    ReadSize=(unsigned int)((Count>(unsigned long)UnpPackedSize) ? UnpPackedSize : Count);
    if (ArcPtr==NULL)
      return(0);
    RetCode=tread(RdUnpPtr,ReadAddr,ReadSize);
    if (NewLhd.Flags & LHD_SPLIT_AFTER)
      PackedCRC=CRC(PackedCRC,ReadAddr,ReadSize,CRC32);
    CurUnpRead+=RetCode;
    ReadAddr+=RetCode;
    TotalRead+=RetCode;
    Count-=RetCode;
    UnpPackedSize-=RetCode;
    if (UnpPackedSize == 0 && UnpVolume)
      MergeArchive(1);
    else
      break;
  }
  if (RetCode!=-1U)
  {
    RetCode=TotalRead;
    if (Encryption)
      if (Encryption<20)
        Crypt(Addr,RetCode,(Encryption==15) ? NEW_CRYPT : OLD_DECODE);
      else
        for (I=0;I<RetCode;I+=16)
          DecryptBlock(&Addr[I]);
  }
  return(RetCode);
}


unsigned int UnpWrite(unsigned char *Addr,unsigned int Count)
{
  unsigned int RetCode;
  UnpWrAddr=Addr;
  UnpWrSize=Count;
  if (TestMode)
    RetCode=Count;
  else
    RetCode=twrite(WrUnpPtr,Addr,Count);
  CurUnpWrite+=RetCode;
  if (!SkipUnpCRC)
    UnpFileCRC=CRC(UnpFileCRC,Addr,RetCode,(ArcFormat==OLD) ? CRC16 : CRC32);
  ShowUnpWrite();
  return(RetCode);
}


void ShowUnpWrite(void)
{
/*  percent=ToPercent(CurUnpWrite,NewLhd.UnpSize); */
}


