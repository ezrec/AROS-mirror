void ListArchive(void)
{
  unsigned long TotalPackSize,TotalUnpSize,FileCount,ArchivesCount;
  unsigned long SumPackSize,SumUnpSize,SumFileCount;
  int I,OutHeader;
  ArchivesCount=SumPackSize=SumUnpSize=SumFileCount=0;
  while (ReadArcName())
  {
    if ((ArcPtr=wopen(ArcName,READBINARY,M_DENYWRITE))==NULL)
      continue;
    while (1)
    {
      TotalPackSize=TotalUnpSize=FileCount=0;
      if (IsArchive())
      {
        OutHeader=0;
        ViewComment();
        tseek(ArcPtr,NewMhd.HeadSize-MainHeadSize,SEEK_CUR);
        mprintf("\n");
        if (SolidType)
          mprintf(MListSolid);
        if (SFXLen>0)
          mprintf(MListSFX);
        if (ArcType==VOL)
          if (SolidType)
            mprintf(MListVol1);
          else
            mprintf(MListVol2);
        else
          if (SolidType)
            mprintf(MListArc1);
          else
            mprintf(MListArc2);
        mprintf(" %s\n",ArcName);
        while(ReadBlock(FILE_HEAD) > 0)
        {
          if (IsProcessFile(NOT_COMPARE_PATH))
          {
            if (!OutHeader)
            {
              if (MainCommand[0]=='V')
                mprintf(MListPathComm);
              else
                mprintf(MListName);
              mprintf(MListTitle);
              for (I=0;I<79;I++)
                mprintf("-");
              OutHeader=1;
            }

            mprintf("\n%c",(NewLhd.Flags & LHD_PASSWORD) ? '*' : ' ');
            if (MainCommand[0]=='V' || strlen(PointToName(ArcFileName))>=13)
            {
              mprintf("%s",ArcFileName);
              ViewFileComment();
              mprintf("\n%12s ","");
            }
            else
              mprintf("%-12s",PointToName(ArcFileName));

            mprintf(" %8lu %8lu ",NewLhd.UnpSize,NewLhd.PackSize);

            if ((NewLhd.Flags & LHD_SPLIT_BEFORE) && (NewLhd.Flags & LHD_SPLIT_AFTER))
              mprintf(" <->");
            else
              if (NewLhd.Flags & LHD_SPLIT_BEFORE)
                mprintf(" <--");
              else
                if (NewLhd.Flags & LHD_SPLIT_AFTER)
                  mprintf(" -->");
                else
                  mprintf("%3d%%",ToPercent(NewLhd.PackSize,NewLhd.UnpSize));

            ConvertDate(NewLhd.FileTime);
            mprintf(" %2.2s-%2.2s-%2.2s %2.2s:%2.2s ",
                    DateStr,DateStr+2,DateStr+4,DateStr+6,DateStr+8);

            ShowAttr();

            mprintf(" %8.8lX",NewLhd.FileCRC);
            mprintf(" m%d",NewLhd.Method-0x30);
            if ((NewLhd.Flags & LHD_WINDOWMASK)<=4*32)
              mprintf("%c",((NewLhd.Flags&LHD_WINDOWMASK)>>5)+'a');
            else
              mprintf(" ");
            mprintf(" %d.%d",NewLhd.UnpVer/10,NewLhd.UnpVer%10);

            if (!(NewLhd.Flags & LHD_SPLIT_BEFORE))
            {
              TotalUnpSize+=NewLhd.UnpSize;
              FileCount++;
            }
            TotalPackSize+=NewLhd.PackSize;
          }
          tseek(ArcPtr,NextBlockPos,SEEK_SET);
        }
        if (OutHeader)
        {
          mprintf("\n");
          for (I=0;I<79;I++)
            mprintf("-");
          mprintf("\n%5lu %16lu %8lu %3d%%\n",FileCount,TotalUnpSize,
                  TotalPackSize,ToPercent(TotalPackSize,TotalUnpSize));
          SumFileCount+=FileCount;
          SumUnpSize+=TotalUnpSize;
          SumPackSize+=TotalPackSize;
        }
        else
          mprintf(MListNoFiles);

        ArchivesCount++;

        if (Opt.PackVolume && (NewLhd.Flags & LHD_SPLIT_AFTER) && MergeArchive(0))
          tseek(ArcPtr,0,SEEK_SET);
        else
          break;
      }
      else
      {
        if (TotalArcCount<2)
          mprintf(MNotRAR,ArcName);
        break;
      }
    }
    tclose(ArcPtr);
  }
  if (ArchivesCount>1)
    mprintf("\n%5lu %16lu %8lu %3d%%\n",SumFileCount,SumUnpSize,
            SumPackSize,ToPercent(SumPackSize,SumUnpSize));
}
