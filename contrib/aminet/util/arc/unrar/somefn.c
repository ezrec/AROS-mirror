void GetArcNames(void)
{
  ArcNamesSize=ArcCount=0;
  SplitPath(ArcName,FindPath,FindName,0);
  FindArchives();
  TotalArcCount=ArcCount;
  ArcNamesSize=0;
}


void FindArchives(void)
{
  DIR *dir;
  struct dirent *ent;
  struct FileStat FS;
  int NameLen;
  char *ChPtr,*PtrToDrive;
  static char ArchiveName[NM];

  if ((dir=DirOpen(FindPath))!=NULL)
    while ((ent=readdir(dir))!=NULL)
    {
      strcpy(ArchiveName,FindPath);
      strcat(ArchiveName,ent->d_name);
      if (!GetFileStat(ArchiveName,&FS))
        continue;

      if (FS.IsDir)
      {
        if (Opt.Recurse && strcmp(ent->d_name,".")!=0 &&
            strcmp(ent->d_name,"..")!=0)
        {
          strcat(FindPath,ent->d_name);
          strcat(FindPath,PATHDIVIDER);
          FindArchives();
        }
      }
      else
        if (CmpName(FindName,ent->d_name,0) )
        {
          NameLen=strlen(ArchiveName)+1;
          if ((ArcNamePtr=realloc(ArcNamePtr,ArcNamesSize+NameLen))==NULL)
            ErrExit(EMEMORY,MEMORY_ERROR);
          strcpy(ArcNamePtr+ArcNamesSize,ArchiveName);
          ArcNamesSize+=NameLen;
          ArcCount++;
        }
    }

  if (dir!=NULL)
    closedir(dir);

  PtrToDrive=FindPath;

#ifdef _WIN_32
  if ((ChPtr=strchr(FindPath,':'))!=NULL)
    PtrToDrive=ChPtr+1;
#endif

  if ((ChPtr=strrchr(FindPath,CPATHDIVIDER))!=NULL)
  {
    *ChPtr=0;
    if ((ChPtr=strrchr(FindPath,CPATHDIVIDER))!=NULL)
      *(ChPtr+1)=0;
    else
      *PtrToDrive=0;
  }
  else
    *PtrToDrive=0;
}


int ReadArcName(void)
{
  if (ArcCount==0)
    return(0);
  strcpy(ArcName,ArcNamePtr+ArcNamesSize);
  ArcNamesSize+=strlen(ArcName)+1;
  ArcCount--;
  return(1);
}


int IsArchive(void)
{
  long CurPos;
  int ReadSize;
  unsigned char *ChPtr;
  SFXLen=ArcType=SolidType=LockedType=AVType=0;
  ArcFormat=MainComment=BrokenMhd=0;

  if (tread(ArcPtr,MarkHead.Mark,SIZEOF_MARKHEAD)!=SIZEOF_MARKHEAD)
    return(0);
  if (MarkHead.Mark[0]==0x52 && MarkHead.Mark[1]==0x45 &&
      MarkHead.Mark[2]==0x7e && MarkHead.Mark[3]==0x5e)
  {
    ArcFormat=OLD;
    tseek(ArcPtr,0,SEEK_SET);
    ReadHeader(MAIN_HEAD);
    ArcType=(OldMhd.Flags & MHD_MULT_VOL) ? VOL : ARC;
  }
  else
    if (MarkHead.Mark[0]==0x52 && MarkHead.Mark[1]==0x61 &&
        MarkHead.Mark[2]==0x72 && MarkHead.Mark[3]==0x21 &&
        MarkHead.Mark[4]==0x1a && MarkHead.Mark[5]==0x07 &&
        MarkHead.Mark[6]==0x00)
    {
      ArcFormat=NEW;
      if (ReadHeader(MAIN_HEAD)!=SIZEOF_NEWMHD)
        return(0);
      if (NewMhd.Flags & MHD_MULT_VOL)
        ArcType=VOL;
      else
        ArcType=ARC;
    }
    else
    {
      if ((TempMemory=malloc(0x20000))==NULL)
        ErrExit(EMEMORY,MEMORY_ERROR);
      CurPos=ftell(ArcPtr);
      ReadSize=tread(ArcPtr,TempMemory,0x1FFF0);
      ChPtr=TempMemory;
      while  (ArcType==0 && ChPtr!=NULL && ChPtr<TempMemory+ReadSize)
      {
        if ((ChPtr=memchr((void *)ChPtr,0x52,ReadSize-(int)(ChPtr-TempMemory)))!=NULL)
        {
          ChPtr++;
          if (ChPtr[0]==0x45 && ChPtr[1]==0x7e && ChPtr[2]==0x5e &&
              TempMemory[28-CurPos]==0x52 && TempMemory[29-CurPos]==0x53 &&
              TempMemory[30-CurPos]==0x46 && TempMemory[31-CurPos]==0x58)
          {
            ArcFormat=OLD;
            SFXLen=(int)(CurPos+ChPtr-TempMemory-1);
            tseek(ArcPtr,SFXLen,SEEK_SET);
            ReadHeader(MAIN_HEAD);
            if (OldMhd.Flags & MHD_MULT_VOL)
              ArcType=VOL;
            else
              ArcType=SFX;
          }
          if (ChPtr[0]==0x61 && ChPtr[1]==0x72 && ChPtr[2]==0x21 &&
              ChPtr[3]==0x1a && ChPtr[4]==0x07 && ChPtr[5]==0x00 &&
              ChPtr[8]==MAIN_HEAD)
          {
            ArcFormat=NEW;
            SFXLen=(int)(CurPos+ChPtr-TempMemory-1);
            tseek(ArcPtr,SFXLen,SEEK_SET);
            tread(ArcPtr,MarkHead.Mark,SIZEOF_MARKHEAD);
            ReadHeader(MAIN_HEAD);
            if (NewMhd.Flags & MHD_MULT_VOL)
              ArcType=VOL;
            else
              ArcType=SFX;
          }
        }
      }
      free(TempMemory);
      TempMemory=NULL;
      if (!ArcType)
        return(0);
    }
  if (ArcFormat==OLD)
  {
    MainHeadSize=SIZEOF_OLDMHD;
    NewMhd.Flags=OldMhd.Flags & 0x3f;
    NewMhd.HeadSize=OldMhd.HeadSize;
  }
  else
  {
    MainHeadSize=SIZEOF_NEWMHD;
    if ((UWORD)~HeaderCRC!=NewMhd.HeadCRC)
    {
      mprintf("\n\n%s",MLogMainHead);
      BrokenMhd=1;
    }
  }
  if (NewMhd.Flags & MHD_SOLID)
    SolidType=1;
  if (NewMhd.Flags & MHD_COMMENT)
    MainComment=1;
  if (NewMhd.Flags & MHD_LOCK)
    LockedType=1;
  if (NewMhd.Flags & MHD_AV)
    AVType=1;
  return(ArcType);
}


