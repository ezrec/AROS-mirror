#define GetHeaderByte(N) Header[N]

#define GetHeaderWord(N) (Header[N]+((UWORD)Header[N+1]<<8))

#define GetHeaderDword(N) (Header[N]+((UWORD)Header[N+1]<<8)+\
                          ((UDWORD)Header[N+2]<<16)+\
                          ((UDWORD)Header[N+3]<<24))


int IsProcessFile(int ComparePath)
{
  int Wildcards;
  char PathArg[NM],NameArg[NM];
  char PathArc[NM],NameArc[NM];
  for (NextArgName(ARG_RESET);NextArgName(ARG_NEXT);)
  {
    Wildcards=SplitPath(ArgName,PathArg,NameArg,1);
    SplitPath(ArcFileName,PathArc,NameArc,1);
    if (!ExclCheck(ArcFileName) && CmpName(NameArg,NameArc,0))
      if ((ComparePath==NOT_COMPARE_PATH && *PathArg==0) ||
          (stricomp(PathArg,PathArc)==0 ||
          Wildcards && strnicomp (PathArg,PathArc,strlen(PathArg))==0 ))
       return(1);
  }
  return(0);
}


int ReadBlock(int BlockType)
{
  struct NewFileHeader SaveFileHead;
  int Size,ReadSubBlock=0;
  static int LastBlock;
  BrokenFileHeader=0;
  memcpy(&SaveFileHead,&NewLhd,sizeof(SaveFileHead));
  if (BlockType & READSUBBLOCK)
    ReadSubBlock=1;
  BlockType &= 0xff;
  if (ArcFormat==OLD)
  {
    memset(&OldLhd,0,sizeof(OldLhd));
    CurBlockPos=ftell(ArcPtr);
    Size=ReadHeader(FILE_HEAD);
    if (Size != 0)
      if (OldLhd.Method > SIZEOF_SHORTBLOCKHEAD || OldLhd.NameSize==0 || OldLhd.NameSize > 80 ||
          OldLhd.UnpVer==0 || OldLhd.UnpVer > 20 || OldLhd.HeadSize <= 21 ||
          OldLhd.Flags<8 && OldLhd.HeadSize!=21+OldLhd.NameSize)
        return(0);
    NewLhd.HeadType=FILE_HEAD;
    NewLhd.HeadSize=OldLhd.HeadSize;
    NewLhd.Flags=OldLhd.Flags|LONG_BLOCK;
    NewLhd.PackSize=OldLhd.PackSize;
    NewLhd.UnpSize=OldLhd.UnpSize;
    NewLhd.FileTime=OldLhd.FileTime;
    NewLhd.UnpVer=(OldLhd.UnpVer==2) ? 13 : 10;
    NewLhd.Method=OldLhd.Method+0x30;
    NewLhd.NameSize=OldLhd.NameSize;
    NewLhd.FileAttr=OldLhd.FileAttr;
    NewLhd.FileCRC=OldLhd.FileCRC;
    if (Size!=0)
      NextBlockPos=CurBlockPos+OldLhd.HeadSize+OldLhd.PackSize;
  }
  else
  {
    while (1)
    {
      CurBlockPos=ftell(ArcPtr);
      Size=ReadHeader(FILE_HEAD);
      if (Size!=0)
      {
        if (NewLhd.HeadSize<SIZEOF_SHORTBLOCKHEAD)
          return(0);
        NextBlockPos=CurBlockPos+NewLhd.HeadSize;
        if (NewLhd.Flags & LONG_BLOCK)
          NextBlockPos+=NewLhd.PackSize;
        if (NextBlockPos<=CurBlockPos)
          return(0);
      }
      else
        if (filelen(ArcPtr) < NextBlockPos)
          mprintf("\n%s",MLogUnexpEOF);
      if (Size > 0 && BlockType!=SUB_HEAD)
        LastBlock=BlockType;
      if (Size==0 || BlockType==ALL_HEAD || NewLhd.HeadType==BlockType ||
          (NewLhd.HeadType==SUB_HEAD && ReadSubBlock && LastBlock==BlockType))
        break;
      tseek(ArcPtr,NextBlockPos,SEEK_SET);
    }
  }

  BlockHead.HeadCRC=NewLhd.HeadCRC;
  BlockHead.HeadType=NewLhd.HeadType;
  BlockHead.Flags=NewLhd.Flags;
  BlockHead.HeadSize=NewLhd.HeadSize;
  BlockHead.DataSize=NewLhd.PackSize;

  if (BlockType!=NewLhd.HeadType)
    BlockType=ALL_HEAD;
  switch(BlockType)
  {
    case FILE_HEAD:
      if (Size>0)
      {
        NewLhd.NameSize=Min(NewLhd.NameSize,sizeof(ArcFileName)-1);
        tread(ArcPtr,ArcFileName,NewLhd.NameSize);
        ArcFileName[NewLhd.NameSize]=0;
        if (ArcFormat==NEW && NewLhd.HeadCRC!=(UWORD)~CRC(HeaderCRC,&ArcFileName[0],NewLhd.NameSize,CRC32))
        {
          BrokenFileHeader=1;
          mprintf("\n%s - %s\n",ArcFileName,MLogFileHead);
        }
        if (Opt.ConvertNames==NAMES_UPPERCASE)
          strupper(ArcFileName);
        if (Opt.ConvertNames==NAMES_LOWERCASE)
          strlower(ArcFileName);
        Size+=NewLhd.NameSize;
        ConvertUnknownHeader();
      }
      break;
    default:
      memcpy(&NewLhd,&SaveFileHead,sizeof(NewLhd));
      tseek(ArcPtr,CurBlockPos,SEEK_SET);
      break;
  }
  return(Size);
}


int ReadHeader(int BlockType)
{
  int Size;
  unsigned char Header[64];
  switch(BlockType)
  {
    case MAIN_HEAD:
      if (ArcFormat==OLD)
      {
        Size=tread(ArcPtr,Header,SIZEOF_OLDMHD);
        memcpy(OldMhd.Mark,Header,4);
        OldMhd.HeadSize=GetHeaderWord(4);
        OldMhd.Flags=GetHeaderByte(6);
      }
      else
      {
        Size=tread(ArcPtr,Header,SIZEOF_NEWMHD);
        NewMhd.HeadCRC=GetHeaderWord(0);
        NewMhd.HeadType=GetHeaderByte(2);
        NewMhd.Flags=GetHeaderWord(3);
        NewMhd.HeadSize=GetHeaderWord(5);
        NewMhd.Reserved=GetHeaderWord(7);
        NewMhd.Reserved1=GetHeaderDword(9);
        HeaderCRC=CRC(0xFFFFFFFFL,&Header[2],SIZEOF_NEWMHD-2,CRC32);
      }
      break;
    case FILE_HEAD:
      if (ArcFormat==OLD)
      {
        Size=tread(ArcPtr,Header,SIZEOF_OLDLHD);
        OldLhd.PackSize=GetHeaderDword(0);
        OldLhd.UnpSize=GetHeaderDword(4);
        OldLhd.FileCRC=GetHeaderWord(8);
        OldLhd.HeadSize=GetHeaderWord(10);
        OldLhd.FileTime=GetHeaderDword(12);
        OldLhd.FileAttr=GetHeaderByte(16);
        OldLhd.Flags=GetHeaderByte(17);
        OldLhd.UnpVer=GetHeaderByte(18);
        OldLhd.NameSize=GetHeaderByte(19);
        OldLhd.Method=GetHeaderByte(20);
      }
      else
      {
        Size=tread(ArcPtr,Header,SIZEOF_NEWLHD);
        NewLhd.HeadCRC=GetHeaderWord(0);
        NewLhd.HeadType=GetHeaderByte(2);
        NewLhd.Flags=GetHeaderWord(3);
        NewLhd.HeadSize=GetHeaderWord(5);
        NewLhd.PackSize=GetHeaderDword(7);
        NewLhd.UnpSize=GetHeaderDword(11);
        NewLhd.HostOS=GetHeaderByte(15);
        NewLhd.FileCRC=GetHeaderDword(16);
        NewLhd.FileTime=GetHeaderDword(20);
        NewLhd.UnpVer=GetHeaderByte(24);
        NewLhd.Method=GetHeaderByte(25);
        NewLhd.NameSize=GetHeaderWord(26);
        NewLhd.FileAttr=GetHeaderDword(28);
        HeaderCRC=CRC(0xFFFFFFFFL,&Header[2],SIZEOF_NEWLHD-2,CRC32);
      }
      break;
    case COMM_HEAD:
      Size=tread(ArcPtr,Header,SIZEOF_COMMHEAD);
      CommHead.HeadCRC=GetHeaderWord(0);
      CommHead.HeadType=GetHeaderByte(2);
      CommHead.Flags=GetHeaderWord(3);
      CommHead.HeadSize=GetHeaderWord(5);
      CommHead.UnpSize=GetHeaderWord(7);
      CommHead.UnpVer=GetHeaderByte(9);
      CommHead.Method=GetHeaderByte(10);
      CommHead.CommCRC=GetHeaderWord(11);
      HeaderCRC=CRC(0xFFFFFFFFL,&Header[2],SIZEOF_COMMHEAD-2,CRC32);
      break;
    case PROTECT_HEAD:
      Size=tread(ArcPtr,Header,SIZEOF_PROTECTHEAD);
      ProtectHead.HeadCRC=GetHeaderWord(0);
      ProtectHead.HeadType=GetHeaderByte(2);
      ProtectHead.Flags=GetHeaderWord(3);
      ProtectHead.HeadSize=GetHeaderWord(5);
      ProtectHead.DataSize=GetHeaderDword(7);
      ProtectHead.Version=GetHeaderByte(11);
      ProtectHead.RecSectors=GetHeaderWord(12);
      ProtectHead.TotalBlocks=GetHeaderDword(14);
      memcpy(ProtectHead.Mark,&Header[18],8);
      HeaderCRC=CRC(0xFFFFFFFFL,&Header[2],SIZEOF_PROTECTHEAD-2,CRC32);
      break;
    case ALL_HEAD:
      Size=tread(ArcPtr,Header,SIZEOF_SHORTBLOCKHEAD);
      BlockHead.HeadCRC=GetHeaderWord(0);
      BlockHead.HeadType=GetHeaderByte(2);
      BlockHead.Flags=GetHeaderWord(3);
      BlockHead.HeadSize=GetHeaderWord(5);
      if (BlockHead.Flags & LONG_BLOCK)
      {
        Size+=tread(ArcPtr,&Header[7],4);
        BlockHead.DataSize=GetHeaderDword(7);
      }
      break;
  }
  return(Size);
}


