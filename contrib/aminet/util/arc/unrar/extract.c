void ExtrFile(void)
{
  struct FileStat FS;
  char DestFileName[NM],*ChPtr;
  long FileCount=0,TotalFileCount,ErrCount=0;
  int SkipSolid,ExtrFile,MDCode,Size,AllArgsUsed;
  int UserReject,TmpPassword=0,BrokenFile;
  int FirstFile;


GetNextArchive:
  if (UnpMemory!=NULL)
  {
    free(UnpMemory);
    UnpMemory=NULL;
  }

  while (ReadArcName())
  {

SkipToFirstVol:

    TotalFileCount=ArgUsed=AllArgsUsed=0;
    FirstFile=1;
    if ((ArcPtr=wopen(ArcName,READBINARY,M_DENYWRITE))==NULL)
      continue;

    if (TmpPassword!=0)
      TmpPassword=Password[0]=0;
    if (!IsArchive())
    {
      mprintf(MNotRAR,ArcName);
      tclose(ArcPtr);
      continue;
    }

    if (MainCommand[0]=='T')
      mprintf(MExtrTest,ArcName);
    else
      mprintf(MExtracting,ArcName);

    ViewComment();

    if ((UnpMemory=malloc(UNP_MEMORY))==NULL)
      ErrExit(EMEMORY,MEMORY_ERROR);

    tseek(ArcPtr,NewMhd.HeadSize-MainHeadSize,SEEK_CUR);
    UnpVolume=0;

    while (1)
    {
      Size=ReadBlock(FILE_HEAD | READSUBBLOCK);
      if (Size<=0 && UnpVolume==0)
        break;
      if (BlockHead.HeadType==SUB_HEAD)
      {
        tseek(ArcPtr,NextBlockPos,SEEK_SET);
        continue;
      }

      if (AllArgsUsed)
        break;

      ConvertPath(ArcFileName,ArcFileName);

      ConvertFlags();

      if ((NewLhd.Flags & LHD_SPLIT_BEFORE) && SolidType && FirstFile)
      {
        tclose(ArcPtr);
        ChPtr=strrchr(ArcName,'.');
        if (ChPtr==NULL || stricomp(ChPtr,".rar")!=0)
        {
          SetExt(ArcName,"rar");
          if (FileExist(ArcName))
            goto SkipToFirstVol;
          SetExt(ArcName,"exe");
          if (FileExist(ArcName))
            goto SkipToFirstVol;
        }
        mprintf(MNeedFirstVol);
        if (TotalArcCount>1)
          goto GetNextArchive;
        else
          ErrExit(EEMPTY,FATAL_ERROR);
      }
      FirstFile=0;
      if (UnpVolume && Size==0 && !MergeArchive(0))
      {
        ErrCount++;
        ExitCode=WARNING;
        goto GetNextArchive;
      }
      UnpVolume=(NewLhd.Flags & LHD_SPLIT_AFTER);

      tseek(ArcPtr,NextBlockPos-NewLhd.PackSize,SEEK_SET);

      TestMode=0;
      ExtrFile=0;
      SkipSolid=0;

      if (IsProcessFile(COMPARE_PATH) && (NewLhd.Flags & LHD_SPLIT_BEFORE)==0
          || (SkipSolid=SolidType)!=0)
      {

        if (NewLhd.Flags & LHD_PASSWORD)
        {
          if (*Password==0)
          {
            if (GetPassword(1)!=1)
              ErrExit(EEMPTY,USER_BREAK);
            TmpPassword=(SolidType) ? 2 : 1;
          }
          else
            if (TmpPassword==1)
            {
              mprintf(MUseCurPsw,ArcFileName);
              Ask(MYesNoAll);
              switch(Choice)
              {
                case -1:
                  ErrExit(EEMPTY,USER_BREAK);
                case 2:
                  if (GetPassword(1)!=1)
                    ErrExit(EEMPTY,USER_BREAK);
                  break;
                case 3:
                  TmpPassword=2;
                  break;
              }
            }
        }

        strcpy(DestFileName,ExtrPath);

        if (MainCommand[0]=='E')
          strcat(DestFileName,PointToName(ArcFileName));
        else
          strcat(DestFileName,ArcFileName);

        ExtrFile=!SkipSolid;
        if ((Opt.FreshFiles || Opt.UpdateFiles) && (MainCommand[0]=='E' || MainCommand[0]=='X'))
        {
          if (GetFileStat(DestFileName,&FS))
          {
            if ( FS.FileTime >= NewLhd.FileTime)
              ExtrFile=0;
          }
          else
            if (Opt.FreshFiles)
              ExtrFile=0;
        }

        if (NewLhd.UnpVer<13 || NewLhd.UnpVer>UNP_VER)
        {
          mprintf(MUnknownMeth,ArcFileName);
          ExtrFile=0;
          ErrCount++;
          ExitCode=WARNING;
        }

        if (IsLabel(NewLhd.FileAttr))
          continue;
        if (IsDir(NewLhd.FileAttr))
        {
          if (MainCommand[0]=='P' || MainCommand[0]=='E')
            continue;
          if (SkipSolid)
          {
            mprintf(MExtrSkipDir,ArcFileName);
            continue;
          }
          FileCount++;
          if (MainCommand[0]=='T')
          {
            mprintf(MExtrTestDir,ArcFileName);
            continue;
          }
          if ((MDCode=MakeDir(DestFileName,NewLhd.FileAttr))==-1 && errno==ENOENT)
          {
            CreatePath(DestFileName);
            if ((MDCode=MakeDir(DestFileName,NewLhd.FileAttr))==-1 && errno!=EEXIST)
            {
              mprintf(MExtrErrMkDir,ArcFileName);
              ExitCode=WARNING;
            }
          }
          if (MDCode==0)
            mprintf(MCreatDir,ArcFileName);
          continue;
        }
        else
        {
          if (MainCommand[0]=='T' && ExtrFile)
            TestMode=1;
          if (MainCommand[0]=='P' && ExtrFile)
            FilePtr=stdout;
          if ((MainCommand[0]=='E' || MainCommand[0]=='X') && ExtrFile)
          {
            FilePtr=FileCreate(DestFileName,Opt.Overwrite,&UserReject);
            if (FilePtr==NULL)
            {
              if (!UserReject)
              {
                mprintf(MCannotCreate,DestFileName);
                ErrCount++;
                ExitCode=WARNING;
              }
              ExtrFile=0;
              AllArgsUsed=IsAllArgsUsed();
            }
          }
        }

        if (!ExtrFile && SolidType)
        {
          SkipSolid=1;
          TestMode=1;
          ExtrFile=1;
        }
        if (ExtrFile)
        {
          if (!SkipSolid)
          {
            if (!TestMode && CheckForDevice(FilePtr))
              ErrExit(EWRITE,WRITE_ERROR);
            FileCount++;
          }
          TotalFileCount++;
          if (SkipSolid)
            mprintf(MExtrSkipFile,ArcFileName);
          else
            switch(MainCommand[0])
            {
              case 'T':
                mprintf(MExtrTestFile,ArcFileName);
                break;
              case 'P':
                mprintf(MExtrPrinting,ArcFileName);
                CheckWriteSize=0;
                break;
              case 'X':
              case 'E':
                mprintf(MExtrFile,DestFileName);
                break;
            }
          strcpy(CurExtrFile,SkipSolid ? "":DestFileName);
          CurUnpRead=CurUnpWrite=0;
          UnpFileCRC=(ArcFormat==OLD) ? 0 : 0xFFFFFFFFL;
          PackedCRC=0xFFFFFFFFL;
          if ((*Password!=0) && (NewLhd.Flags & LHD_PASSWORD))
            Encryption=NewLhd.UnpVer;
          else
            Encryption=0;
          if (Encryption)
            SetCryptKeys(Password);
          UnpPackedSize=NewLhd.PackSize;
          DestUnpSize=NewLhd.UnpSize;
          RdUnpPtr=ArcPtr;
          WrUnpPtr=FilePtr;
          Suspend=0;
          Repack=0;
          SkipUnpCRC=SkipSolid;
          if (NewLhd.Method==0x30)
            UnstoreFile();
          else
            if (NewLhd.UnpVer<=15)
              tunpack(UnpMemory,TotalFileCount>1 && SolidType,OLD_UNPACK);
            else
              tunpack(UnpMemory,NewLhd.Flags & LHD_SOLID,NEW_UNPACK);
          if (!SkipSolid)
            AllArgsUsed=IsAllArgsUsed();

          if (MainCommand[0]=='P')
            CheckWriteSize=1;
          if (ArcPtr!=NULL)
            tseek(ArcPtr,NextBlockPos,SEEK_SET);
          if (!SkipSolid)
            if ((ArcFormat==OLD && UnpFileCRC==NewLhd.FileCRC) || (ArcFormat==NEW && UnpFileCRC==~NewLhd.FileCRC))
            {
              if (MainCommand[0]!='P')
                mprintf(MOk);
              BrokenFile=0;
            }
            else
            {
              if (NewLhd.Flags & LHD_PASSWORD)
                mprintf(MEncrBadCRC,ArcFileName);
              else
                mprintf("\n%-20s - %s",ArcFileName,MCRCFailed);
              ExitCode=CRC_ERROR;
              ErrCount++;
              BrokenFile=1;
            }
          if (TestMode==1)
            TestMode=0;
          else
          {
            if (MainCommand[0]=='X' || MainCommand[0]=='E')
            {
              SetOpenFileStat(FilePtr);
              tclose(FilePtr);
              SetCloseFileStat(DestFileName);
              if (BrokenFile && !Opt.KeepBroken)
                remove(DestFileName);
            }
          }
          *CurExtrFile=0;
        }
      }
      if (ArcPtr==NULL)
        goto GetNextArchive;
      if (!ExtrFile)
        if (!SolidType)
          tseek(ArcPtr,NextBlockPos,SEEK_SET);
        else
          if (!SkipSolid)
            break;
      if (AllArgsUsed)
        break;
    }
    free(UnpMemory);
    UnpMemory=NULL;
    tclose(ArcPtr);
  }
  if (FileCount==0)
  {
    mprintf(MExtrNoFiles);
    ExitCode=WARNING;
  }
  else
    if (ErrCount==0)
      mprintf(MExtrAllOk);
    else
      mprintf(MExtrTotalErr,ErrCount);
  if (TmpPassword!=0)
    memset(Password,0,sizeof(Password));
}

