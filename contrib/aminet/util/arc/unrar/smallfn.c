void tunpack(unsigned char *Mem,int Solid,int Mode)
{
  if (Mode==OLD_UNPACK)
    OldUnpack(Mem,Solid);
  else
    Unpack(Mem,Solid);
}


int twrite(FILE *stream,void *buf,unsigned len)
{
  int rescode;

#ifdef _WIN_32
  errno=0;
#endif

  rescode=fwrite(buf,1,len,stream);
  if (rescode!=len || ferror(stream))
    ErrExit(EWRITE,WRITE_ERROR);

#ifdef _WIN_32
  if (errno==ENOSPC)
    ErrExit(EWRITE,WRITE_ERROR);
#endif

  return(rescode);
}


int tread(FILE *stream,void *buf,unsigned len)
{
  return(fread(buf,1,len,stream));
}


int tseek(FILE *stream,long offset,int fromwhere)
{
  return(fseek(stream,offset,fromwhere));
}


long filelen(FILE *FPtr)
{
  long RetPos,SavePos;
  SavePos=ftell(FPtr);
  tseek(FPtr,0,SEEK_END);
  RetPos=ftell(FPtr);
  tseek(FPtr,SavePos,SEEK_SET);
  return(RetPos);
}


void tclose(FILE *FPtr)
{
  if (FPtr!=NULL && fclose(FPtr)==EOF)
    ErrExit(ECLOSE,FATAL_ERROR);
}


FILE* FileCreate(char *Name,int OverwriteMode,int *UserReject)
{
  FILE *Ptr;
  if (UserReject!=NULL)
    *UserReject=0;
  while (FileExist(Name))
  {
    if (OverwriteMode==NO_OVERWR)
    {
      if (UserReject!=NULL)
        *UserReject=1;
      return(NULL);
    }
    if (Opt.AllYes || OverwriteAll || OverwriteMode==ALL_OVERWR)
      break;
    if (OverwriteMode==ASK_OVERWR)
    {
      mprintf(MFileExists,Name);
      Ask(MYesNoAllRenQ);
      if (Choice==1)
        break;
      if (Choice==2)
      {
        if (UserReject!=NULL)
          *UserReject=1;
        return(NULL);
      }
      if (Choice==3)
      {
        OverwriteAll=1;
        break;
      }
      if (Choice==4)
      {
        mprintf(MAskNewName);
        fgets(Name,NM,stdin);
        RemoveLF(Name);
        continue;
      }
      if (Choice==5)
        ErrExit(EEMPTY,USER_BREAK);
    }
  }
  if ((Ptr=fopen(Name,CREATEBINARY))!=NULL)
    return(Ptr);
  CreatePath(Name);
  return(fopen(Name,CREATEBINARY));
}


int GetKey(void)
{
  char Str[80];
  *Str = '\0';
  fgets(Str,sizeof(Str),stdin);
  RemoveLF(Str);
  return(Str[0]);
}


int ToUpperCase(int Ch)
{
  if (Ch<256)
    return(loctoupper(Ch));
  else
    return(Ch);
}


char* PointToName(char *Path)
{
  int I;
  char *ChPtr;
  for (I=strlen(Path)-1;I>=0;I--)
    if (IsPathDiv(Path[I]))
      return(&Path[I+1]);
  if ((ChPtr=strchr(Path,':'))!=NULL)
    return(ChPtr+1);
  else
    return(Path);
}


void SetExt(char *Name,char *NewExt)
{
  char *Ch;
  if ((Ch=strrchr(Name,'.'))==NULL)
  {
    strcat(Name,".");
    strcat(Name,NewExt);
  }
  else
    strcpy(Ch+1,NewExt);
}


int GetPassword(int AskCount)
{
  int RetCode;
  char CmpStr[128],PromptStr[128];
  strcpy(PromptStr,MAskPsw);
  if (AskCount==1)
  {
    strcat(PromptStr,MFor);
    strcat(PromptStr,PointToName(ArcFileName));
  }
  mprintf("\n%s: ",PromptStr);
  GetPswStr(Password);
  if (*Password!=0)
  {
    if (AskCount==1)
      return(1);
    mprintf(MReAskPsw);
    GetPswStr(CmpStr);
    if (*CmpStr==0)
    {
      RetCode=-1;
      memset(Password,0,sizeof(Password));
    }
    else
      if (strcmp(Password,CmpStr)!=0)
      {
        memset(Password,0,sizeof(Password));
        mprintf(MNotMatchPsw);
        RetCode=0;
      }
      else
        RetCode=1;
  }
  else
  {
    memset(Password,0,sizeof(Password));
    RetCode=-1;
  }
  memset(CmpStr,0,sizeof(CmpStr));
  return(RetCode);
}


void Ask(char *AskStr)
{
  char Item[5][40],*ChPtr,AskString[80];
  int NumItems=0,Ch,I;

  strcpy(AskString,AskStr);
  while ((ChPtr=strchr(AskString,'_'))!=NULL)
  {
    strncpy(Item[NumItems],ChPtr+1,sizeof(Item[NumItems]));
    *ChPtr=' ';
    if ((ChPtr=strchr(Item[NumItems],'_'))!=NULL)
      *ChPtr=0;
    if (strcmp(Item[NumItems],MCmpYesStr)==0 && Opt.AllYes)
    {
      Choice=NumItems+1;
      return;
    }
    NumItems++;
  }
  mprintf("  %s",Item[0]);
  for (I=1;I<NumItems;I++)
    mprintf("/%s",Item[I]);
  mprintf(" ");
  Ch=ToUpperCase(GetKey());
  for (Choice=0,I=1;I<=NumItems;I++)
    if (Ch==Item[I-1][0])
      Choice=I;
}


int ToPercent(long N1,long N2)
{
  if (N1 > 10000)
  {
    N1/=100;
    N2/=100;
  }
  if (N2==0)
    return(0);
  if (N2<N1)
    return(100);
  return((int)(N1*100/N2));
}


char* strlower(char *Str)
{
  char *ChPtr;
  for (ChPtr=Str;*ChPtr;ChPtr++)
    *ChPtr=(char)tolower(*ChPtr);
  return(Str);
}

char* strupper(char *Str)
{
  char *ChPtr;
  for (ChPtr=Str;*ChPtr;ChPtr++)
    *ChPtr=(char)toupper(*ChPtr);
  return(Str);
}


int stricomp(char *Str1,char *Str2)
{
  char S1[512],S2[512];
  strncpy(S1,Str1,sizeof(S1));
  strncpy(S2,Str2,sizeof(S2));
  return(strcmp(strupper(S1),strupper(S2)));
}


int strnicomp(char *Str1,char *Str2,int N)
{
  char S1[512],S2[512];
  strncpy(S1,Str1,sizeof(S1));
  strncpy(S2,Str2,sizeof(S2));
  return(strncmp(strupper(S1),strupper(S2),N));
}


char* RemoveEOL(char *Str)
{
  int I;
  for (I=strlen(Str)-1;I>=0 && iscntrl(Str[I]);I--)
    Str[I]=0;
  return(Str);
}


char* RemoveLF(char *Str)
{
  int I;
  for (I=strlen(Str)-1;I>=0 && (Str[I]=='\r' || Str[I]=='\n');I--)
    Str[I]=0;
  return(Str);
}
