void ViewComment(void)
{
  extern unsigned char PN1,PN2,PN3;
  long CurPos;
  UWORD CommLen,UnpCommLen;
  if (!MainComment)
    return;
  CurPos=ftell(ArcPtr);
  if (ArcFormat==OLD)
    CommLen=getc(ArcPtr)+(getc(ArcPtr)<<8);
  else
  {
    if (ReadHeader(COMM_HEAD)<7 || CommHead.HeadType!=COMM_HEAD || CommHead.HeadSize<7)
      return;
    if (CommHead.HeadCRC!=(UWORD)~HeaderCRC)
    {
      mprintf("\n%s\n",MLogCommHead);
      tseek(ArcPtr,CurPos,SEEK_SET);
      return;
    }
    CommLen=CommHead.HeadSize-SIZEOF_COMMHEAD;
  }
  if (!Opt.DisableComment)
    mprintf("\n");
  if (ArcFormat==OLD && (OldMhd.Flags & MHD_PACK_COMMENT) || ArcFormat==NEW && CommHead.Method != 0x30)
  {
    if (ArcFormat==NEW && (CommHead.UnpVer < 15 || CommHead.UnpVer > UNP_VER || CommHead.Method > 0x35))
    {
      tseek(ArcPtr,CurPos,SEEK_SET);
      return;
    }
    if ((UnpMemory=malloc(UNP_MEMORY))==NULL)
    {
      mprintf(MViewCmtOutMem);
      tseek(ArcPtr,CurPos,SEEK_SET);
      return;
    }
    TestMode=2;
    UnpVolume=0;
    if (ArcFormat==OLD)
    {
      UnpCommLen=getc(ArcPtr)+(getc(ArcPtr)<<8);
      CommLen-=2;
      PN1=0;
      PN2=7;
      PN3=77;
      Encryption=13;
    }
    else
    {
      Encryption=0;
      UnpCommLen=CommHead.UnpSize;
    }
    DestUnpSize=UnpCommLen;
    UnpPackedSize=CommLen;
    UnpFileCRC=0xFFFFFFFFL;
    RdUnpPtr=ArcPtr;
    UnpSolid=0;
    Suspend=0;
    Repack=0;
    SkipUnpCRC=0;
    tunpack(UnpMemory,0,(CommHead.UnpVer <= 15) ? OLD_UNPACK:NEW_UNPACK);
    if (ArcFormat==NEW && (UWORD)~UnpFileCRC!=CommHead.CommCRC)
      mprintf("\n%s",MLogCommBrk);
    else
      ShowComment(UnpWrAddr,UnpWrSize);
    free(UnpMemory);
    UnpMemory=NULL;
  }
  else
  {
    if ((TempMemory=malloc(CommLen))==NULL)
    {
      mprintf(MViewCmtOutMem);
      tseek(ArcPtr,CurPos,SEEK_SET);
      return;
    }
    tread(ArcPtr,TempMemory,CommLen);
    if (ArcFormat==NEW && CommHead.CommCRC!=(UWORD)~CRC(0xFFFFFFFFL,TempMemory,CommLen,CRC32))
      mprintf("\n%s",MLogCommBrk);
    else
      ShowComment(TempMemory,CommLen);
    free(TempMemory);
    TempMemory=NULL;
  }
  tseek(ArcPtr,CurPos,SEEK_SET);
}


void ViewFileComment(void)
{
  long CurPos;
  UWORD CommLen;
  if (!(NewLhd.Flags & LHD_COMMENT) || Opt.DisableComment)
    return;
  if ((CommMemory=malloc(0x8000))==NULL)
    return;
  mprintf("\n");
  CurPos=ftell(ArcPtr);
  if (ArcFormat==OLD)
  {
    CommLen=getc(ArcPtr)+(getc(ArcPtr)<<8);
    tread(ArcPtr,CommMemory,CommLen);
    fwrite(CommMemory,1,CommLen,stdout);
    fflush(stdout);
  }
  else
  {
    if (ReadHeader(COMM_HEAD)<7 || CommHead.HeadType!=COMM_HEAD || CommHead.HeadSize<7)
      return;
    if (CommHead.HeadCRC!=(UWORD)~HeaderCRC)
    {
      mprintf("\n%s",MLogCommHead);
      tseek(ArcPtr,CurPos,SEEK_SET);
      free(CommMemory);
      CommMemory=NULL;
      return;
    }
    if (CommHead.UnpVer < 15 || CommHead.UnpVer > UNP_VER || CommHead.Method > 0x30)
    {
      tseek(ArcPtr,CurPos,SEEK_SET);
      free(CommMemory);
      CommMemory=NULL;
      return;
    }
    tread(ArcPtr,CommMemory,CommHead.UnpSize);
    if (CommHead.CommCRC!=(UWORD)~CRC(0xFFFFFFFFL,CommMemory,CommHead.UnpSize,CRC32))
      mprintf("\n%s",MLogBrokFCmt);
    else
    {
      fwrite(CommMemory,1,CommHead.UnpSize,stdout);
      fflush(stdout);
    }
  }
  tseek(ArcPtr,CurPos,SEEK_SET);
  free(CommMemory);
  CommMemory=NULL;
}


void ShowComment(UBYTE *Addr,int Size)
{
#ifdef _WIN_32
  char *ChPtr;
  if ((ChPtr=memchr(Addr,0x1A,Size))!=NULL)
    Size=(int)(ChPtr-Addr);
#endif
  if (!Opt.DisableComment)
  {
    switch(KbdAnsi(Addr,Size))
    {
      case 1:
        ShowAnsiComment(Addr,Size);
        break;
      case 2:
        return;
      default:
        fwrite(Addr,1,Size,stdout);
        fflush(stdout);
        break;
    }
  }
}


int KbdAnsi(UBYTE *Addr,int Size)
{
  int RetCode=0;
  UBYTE *ChPtr;
  while (Size--)
  {
    if (*Addr==27 && *(Addr+1)==91)
    {
      RetCode=1;
      ChPtr=Addr+2;
      while (!(*ChPtr>=65 && *ChPtr<=90 || *ChPtr>=97 && *ChPtr<=122))
        if (*(ChPtr++)==34)
          return(2);
      if (*ChPtr==80 || *ChPtr==112)
        return(2);
    }
    Addr++;
  }
  return(RetCode);
}
