long ExclSize;

void SplitCommandLine(int Argc, char *Argv[])
{
  struct FileStat FS;
  int I,Len,WriteArgsCount,NotSwCount;

  ExtrPath[0]=0;
  ExclSize=ExclCount=NotSwCount=0;
  memset(MainCommand,0,sizeof(MainCommand));

  for (I=1;I<Argc;I++)
    if (Argv[I][0]=='-')
      SetOptions(Argv[I],0);
    else
      NotSwCount++;

  mprintf(MUCopyright);

  WriteArgsCount=*ArcName=0;

  for (I=1;I<Argc;I++)
  {
    if (Argv[I][0]=='-')
      continue;
    if (MainCommand[0]==0 && NotSwCount>1)
    {
      strncpy(MainCommand,Argv[I],sizeof(MainCommand));
      strupper(MainCommand);
    }
    else
    {
      if (*ArcName==0)
      {
        strncpy(ArcName,Argv[I],sizeof(ArcName)-1);
        MakeTmpPath();
        GetArcNames();
      }
      else
      {
        if (Argv[I][0]=='@' && !FileExist(Argv[I]))
        {
          AddArgsFromFile(&Argv[I][1]);
          WriteArgsCount++;
        }
        else
        {
          Len=strlen(Argv[I]);
          if (Len>0 && (Argv[I][Len-1]==':' || IsPathDiv(Argv[I][Len-1])) &&
              strchr("AFUM",MainCommand[0])==NULL)
            strncpy(ExtrPath,Argv[I],sizeof(ExtrPath)-1);
          else
          {
            if ((MainCommand[0]=='X' || MainCommand[0]=='E') &&
                strchr(Argv[I],'*')==NULL && strchr(Argv[I],'?')==NULL &&
                GetFileStat(Argv[I],&FS) && FS.IsDir)
            {
              strncpy(ExtrPath,Argv[I],sizeof(ExtrPath)-1);
              strcat(ExtrPath,PATHDIVIDER);
            }
            else
            {
              AddArgName(Argv[I]);
              WriteArgsCount++;
            }
          }
        }
      }
    }
  }
  if (WriteArgsCount==0 && *ArcName!=0)
    AddArgName("*.*");
}


void SetOptions(char *OptStr,int MultipleSwitches)
{
  int I;
  char SwStr[200];
  while (*OptStr!=0)
  {
    if (MultipleSwitches)
    {
      while (*OptStr!='-' && *OptStr!=0)
        OptStr++;
      if (*OptStr==0 || *(++OptStr)==0)
        break;
      for (I=0; *OptStr!=0 && !isspace(*OptStr) && I<80; I++,OptStr++)
        SwStr[I]=*OptStr;
      SwStr[I]=0;
    }
    else
      strcpy(SwStr,&OptStr[1]);
    switch(toupper(SwStr[0]))
    {
      case 'I':
        if (stricomp(&SwStr[1],"ERR")==0)
        {
          Opt.MsgStream=MSG_STDERR;
          break;
        }
        if (stricomp(&SwStr[1],"NUL")==0)
        {
          Opt.MsgStream=MSG_NULL;
          break;
        }
        break;
      case 'O':
        switch(toupper(SwStr[1]))
        {
          case '+':
            Opt.Overwrite=ALL_OVERWR;
            break;
          case '-':
            Opt.Overwrite=NO_OVERWR;
            break;
          default :
            OutHelp();
            ErrExit(EEMPTY,USER_ERROR);
        }
        break;
      case 'R':
        if (SwStr[1]==0)
          Opt.Recurse=1;
        break;
      case 'Y':
        Opt.AllYes=1;
        break;
      case 'X':
        if (SwStr[1]!=0)
          AddExclArgs(&SwStr[1]);
        break;
      case 'P':
        if (SwStr[1]==0)
        {
          while (GetPassword(2)==0)
            ;
          mprintf("\n");
        }
        else
          strncpy(Password,&SwStr[1],77);
        break;
      case 'V':
        Opt.PackVolume=1;
        break;
      case 'F':
        if (SwStr[1]==0)
          Opt.FreshFiles=1;
        break;
      case 'U':
        if (SwStr[1]==0)
          Opt.UpdateFiles=1;
        break;
      case 'C':
        if (SwStr[2]==0)
          switch(toupper(SwStr[1]))
          {
            case '-':
              Opt.DisableComment=1;
              break;
            case 'U':
              Opt.ConvertNames=NAMES_UPPERCASE;
              break;
            case 'L':
              Opt.ConvertNames=NAMES_LOWERCASE;
              break;
          }
        break;
      case 'K':
        if (toupper(SwStr[1])=='B')
          Opt.KeepBroken=1;
        break;
      case '?' :
        OutHelp();
        ErrExit(EEMPTY,SUCCESS);
      default :
        OutHelp();
        ErrExit(EEMPTY,USER_ERROR);
    }
    if (!MultipleSwitches)
      break;
  }
}


void AddArgsFromFile(char *Name)
{
  FILE *ArgList;
  char Arg[NM];
  if (*Name)
    ArgList=topen(Name,READTEXT,M_DENYWRITE);
  else
    ArgList=stdin;
  while (fgets(Arg,sizeof(Arg),ArgList)!=NULL)
  {
    RemoveEOL(Arg);
    if (*Arg)
      AddArgName(Arg);
  }
  if (*Name)
    fclose(ArgList);
}


void AddExclArgs(char *Name)
{
  FILE *ExclList;
  char Arg[NM];
  int NameSize;
  if (Name[0]!='@')
  {
    NameSize=strlen(Name)+1;
    if ((ExclPtr=realloc(ExclPtr,ExclSize+NameSize))==NULL)
      ErrExit(EMEMORY,MEMORY_ERROR);
    strcpy(ExclPtr+ExclSize,Name);
    ExclSize+=NameSize;
    ExclCount++;
    return;
  }
  if (Name[1])
    ExclList=topen(&Name[1],READTEXT,M_DENYWRITE);
  else
    ExclList=stdin;
  while (fgets(Arg,sizeof(Arg),ExclList)!=NULL)
  {
    RemoveEOL(Arg);
    if (*Arg)
      AddExclArgs(Arg);
  }
  if (Name[1])
    fclose(ExclList);
}


int ExclCheck(char *Name)
{
  int I;
  char *ExclName;
  for (ExclName=ExclPtr,I=0;I<ExclCount;I++,ExclName+=strlen(ExclName)+1)
    if (CmpName(ExclName,Name,2))
      return(1);
  return(0);
}


void MakeTmpPath(void)
{
  if (strrchr(PointToName(ArcName),'.')==NULL && !FileExist(ArcName))
    strcat(ArcName,".rar");

  if ((NumArcDrive=GetPathDisk(ArcName))==-1)
    NumArcDrive=GetCurDisk();

}


void InitSomeVars(void)
{
  CurExtrFile[0]=TmpArc[0]=0;
  ArcPtr=TmpArcPtr=FilePtr=NULL;
  OverwriteAll=0;
}


void OutHelp(void)
{
  static char *Help[]={MUCHelp1,MUCHelp2,MUCHelp3,MUCHelp4,MUCHelp5,
    MUCHelp6,MUCHelp7,MUCHelp8,MUCHelp9,MUCHelp10,MUCHelp11,MUCHelp12,
    MUCHelp13,MUCHelp14,MUCHelp15,MUCHelp16,MUCHelp17,MUCHelp18,MUCHelp19,
    MUCHelp20,MUCHelp21,"\n"};
  int I;
  for (I=0;I<sizeof(Help)/sizeof(Help[0]);I++)
    mprintf(Help[I]);
}


