#include "const.h"
#include "lochead.eng"
#include "loctoup.eng"

#include "global.c"
#include "os.c"
#include "rdwrfn.c"
#include "others.c"
#include "smallfn.c"
#include "block.c"
#include "getargs.c"
#include "comment.c"
#include "getopt.c"
#include "extract.c"
#include "somefn.c"
#include "list.c"
#include "compr.c"

main(int Argc, char *Argv[])
{
  setbuf(stdout,NULL);
  InitCRC();
  InitSomeVars();
#ifndef __AROS__
  if (signal(SIGINT,SIG_IGN)!=SIG_IGN)
    signal(SIGINT,ProcessSignal);
  if (signal(SIGTERM,SIG_IGN)!=SIG_IGN)
    signal(SIGTERM,ProcessSignal);
#ifdef SIGBREAK
  if (signal(SIGBREAK,SIG_IGN)!=SIG_IGN)
    signal(SIGBREAK,ProcessSignal);
#endif
#endif
#ifdef _WIN_32
  {
    int I;
    if (signal(SIGBREAK,SIG_IGN)!=SIG_IGN)
      signal(SIGBREAK,ProcessSignal);
    SetFileApisToOEM();
    for (I=0;I<Argc;I++)
      CharToOem(Argv[I],Argv[I]);
  }
#endif

  SplitCommandLine(Argc,Argv);
  ExecuteCommand();
  ShutDown(SD_MEMORY);
  exit(ExitCode);
}


int CmpName(char *Wildcard,char *Name,int CmpPath)
{
  int Wildcards;
  char Path1[NM],Name1[NM];
  char Path2[NM],Name2[NM];
  Wildcards=SplitPath(Wildcard,Path1,Name1,1);
  SplitPath(Name,Path2,Name2,1);
  if (CmpPath==1 && stricomp(Path1,Path2)!=0)
    return(0);
  if (CmpPath==2)
    if (Wildcards || Opt.Recurse)
    {
      if (strnicomp(Path1,Path2,strlen(Path1))!=0)
        return(0);
    }
    else
      if (stricomp(Path1,Path2)!=0)
        return(0);

  return(CmpNameEx(Name1,Name2));
}


int CmpNameEx(char *pattern,char *string)
{
  char stringc,patternc,rangec;
  int match;

  for (;; ++string)
  {
    stringc=toupper(*string);
    patternc=toupper(*pattern++);
    switch (patternc)
    {
      case 0:
        return(stringc==0);
      case '?':
        if (stringc == 0)
          return(FALSE);

        break;
      case '*':
        if (!*pattern)
          return(TRUE);

        if (*pattern=='.')
        {
          char *dot;
          if (pattern[1]=='*' && pattern[2]==0)
            return(TRUE);
          dot=strchr(string,'.');
          if (pattern[1]==0)
            return (dot==NULL || dot[1]==0);
          if (dot!=NULL)
          {
            string=dot;
            if (strpbrk(pattern,"*?[")==NULL && strchr(string+1,'.')==NULL)
              return(stricomp(pattern+1,string+1)==0);
          }
        }

        while (*string)
          if (CmpNameEx(pattern,string++))
            return(TRUE);
        return(FALSE);
#ifndef _WIN_32
      case '[':
        if (strchr(pattern,']')==NULL)
        {
          if (patternc != stringc)
            return (FALSE);
          break;
        }
        match = 0;
        while ((rangec = toupper(*pattern++))!=0)
        {
          if (rangec == ']')
            if (match)
              break;
            else
              return(FALSE);
          if (match)
            continue;
          if (rangec == '-' && *(pattern - 2) != '[' && *pattern != ']')
          {
            match = (stringc <= toupper(*pattern) &&
                     toupper(*(pattern - 2)) <= stringc);
            pattern++;
          }
          else
            match = (stringc == rangec);
        }
        if (rangec == 0)
          return(FALSE);
        break;
#endif
      default:
        if (patternc != stringc)
          if (patternc=='.' && stringc==0)
            return(CmpNameEx(pattern,string));
          else
            return(FALSE);
        break;
    }
  }
}


void ErrExit(int ErrCode,int Code)
{
  char ErrMsg[200];
  switch(ErrCode)
  {
    case EEMPTY:
      strcpy(ErrMsg,"");
      break;
    case EWRITE:
      strcpy(ErrMsg,MErrWrite);
      break;
    case EREAD:
      strcpy(ErrMsg,MErrRead);
      break;
    case ESEEK:
      strcpy(ErrMsg,MErrSeek);
      break;
    case EOPEN:
      strcpy(ErrMsg,MErrFOpen);
      break;
    case ECREAT:
      strcpy(ErrMsg,MErrFCreat);
      break;
    case ECLOSE:
      strcpy(ErrMsg,MErrFClose);
      break;
    case EMEMORY:
      strcpy(ErrMsg,MErrOutMem);
      break;
    case EARCH:
      strcpy(ErrMsg,MErrBrokenArc);
      break;
  }
  if (ErrCode!=EEMPTY)
    mprintf("\n%s\n%s\n",ErrMsg,MProgAborted);
  ShutDown(SD_FILES | SD_MEMORY);
  exit(Code);
}


void ProcessSignal(int SigType)
{
  if (SigType!=SIGTERM)
    mprintf(MBreak);
  ShutDown(SD_FILES | SD_MEMORY);
  exit(USER_BREAK);
}


void CreatePath(char *fpath)
{
  char *ChPtr;
  ChPtr=fpath;
  while(*ChPtr!=0)
  {
    if (IsPathDiv(*ChPtr))
    {
      *ChPtr=0;
      if (MakeDir(fpath,0777)==0)
        mprintf(MCreatDir,fpath);
      *ChPtr=CPATHDIVIDER;
    }
    ChPtr++;
  }
}


void NextVolumeName(int NewNumbering)
{
  char *ChPtr;
  if ((ChPtr=strrchr(ArcName,'.'))==NULL)
  {
    strcat(ArcName,".rar");
    ChPtr=strrchr(ArcName,'.');
  }
  else
    if (ChPtr[1]==0 || stricomp(ChPtr+1,"exe")==0 || stricomp(ChPtr+1,"sfx")==0)
      strcpy(ChPtr+1,"rar");
  if (NewNumbering)
  {
    while (!isdigit(*ChPtr) && ChPtr>ArcName)
      ChPtr--;
    while ((++(*ChPtr))=='9'+1)
    {
      *ChPtr='0';
      ChPtr--;
      if (ChPtr<ArcName || !isdigit(*ChPtr))
      {
        char *EndPtr;
        for (EndPtr=ArcName+strlen(ArcName);EndPtr!=ChPtr;EndPtr--)
          *(EndPtr+1)=*EndPtr;
        *(ChPtr+1)='1';
        break;
      }
    }
  }
  else
    if (!isdigit(*(ChPtr+2)) || !isdigit(*(ChPtr+3)))
      strcpy(ChPtr+2,"00");
    else
    {
      ChPtr+=3;
      while ((++(*ChPtr))=='9'+1)
        if (*(ChPtr-1)=='.')
        {
          *ChPtr='A';
          break;
        }
        else
        {
          *ChPtr='0';
          ChPtr--;
        }
    }
}


int MergeArchive(int ShowFileName)
{
  tclose(ArcPtr);
  if (MainCommand[0]=='X' || MainCommand[0]=='E' || MainCommand[0]=='T')
    if (NewLhd.UnpVer>=20 && NewLhd.FileCRC!=0xffffffff && PackedCRC!=~NewLhd.FileCRC)
      mprintf(MDataBadCRC,ArcFileName,ArcName);
  NextVolumeName((NewMhd.Flags & MHD_NEWNUMBERING) && ArcFormat!=OLD);
  while ((ArcPtr=fopen(ArcName,READBINARY))==NULL)
  {
    if (NumArcDrive==-1 || !IsRemovable(NumArcDrive))
    {
      mprintf(MAbsNextVol,ArcName);
      return(0);
    }
    AskNextVol();
  }
  if (!IsArchive())
  {
    mprintf(MBadArc,ArcName);
    return(0);
  }
  if (MainCommand[0]=='T')
    mprintf(MTestVol,ArcName);
  else
    if (MainCommand[0]=='X' || MainCommand[0]=='E')
      mprintf(MExtrVol,ArcName);
  tseek(ArcPtr,NewMhd.HeadSize-MainHeadSize,SEEK_CUR);
  ReadBlock(FILE_HEAD);
  ConvertFlags();
  if (ShowFileName)
    mprintf(MExtrPoints,ArcFileName);
  UnpVolume=(NewLhd.Flags & LHD_SPLIT_AFTER);
  tseek(ArcPtr,NextBlockPos-NewLhd.PackSize,SEEK_SET);
  UnpPackedSize=NewLhd.PackSize;
  RdUnpPtr=ArcPtr;
  PackedCRC=0xFFFFFFFF;
  return(1);
}


void UnstoreFile(void)
{
  int Code;
  if ((TempMemory=malloc(0x8000))==NULL)
    ErrExit(EMEMORY,MEMORY_ERROR);
  while (1)
  {
    if ((Code=UnpRead(TempMemory,0x8000))==-1)
      ErrExit(EWRITE,WRITE_ERROR);
    if (Code==0)
      break;
    Code=(int)Min((unsigned long)Code,(unsigned long)DestUnpSize);
    if (UnpWrite(TempMemory,Code)==-1U)
      ErrExit(EWRITE,WRITE_ERROR);
    if (DestUnpSize>=0)
      DestUnpSize-=Code;
  }
  free(TempMemory);
  TempMemory=NULL;
}


void ConvertPath(char *OutPath, char *InPath)
{
  char TmpStr[NM],*OutPathPtr;
  if (InPath[0] && IsDriveDiv(InPath[1]))
    OutPathPtr=&InPath[2];
  else
    OutPathPtr=InPath;
  if (OutPathPtr[0]=='.' && OutPathPtr[1]==CPATHDIVIDER)
    OutPathPtr++;
  if (OutPathPtr[0]=='.' && OutPathPtr[1]=='.' && OutPathPtr[2]==CPATHDIVIDER)
    OutPathPtr+=2;
  if (*OutPathPtr==CPATHDIVIDER)
    OutPathPtr++;
  strcpy(TmpStr,OutPathPtr);
  strcpy(OutPath,TmpStr);
}


void AskNextVol(void)
{
  mprintf(MAskNextVol,ArcName);
  Ask(MContinueQuit);
  if (Choice==2)
    ErrExit(EEMPTY,USER_BREAK);
}

