void FreeMemory(void)
{
  free(UnpMemory);
  free(TempMemory);
  free(CommMemory);
  free(ArgBuf);
  free(ExclPtr);
  free(ArcNamePtr);
  UnpMemory=NULL;
  TempMemory=NULL;
  CommMemory=NULL;
  ArgBuf=ExclPtr=ArcNamePtr=NULL;
}


void ExecuteCommand(void)
{
  if (MainCommand[1] && strchr("PXET",toupper(*MainCommand))!=NULL)
  {
    OutHelp();
    ErrExit(EEMPTY,USER_ERROR);
  }
  switch(MainCommand[0])
  {
    case 'P':
    case 'X':
    case 'E':
    case 'T':
      ExtrFile();
      break;
    case 'V':
    case 'L':
      ListArchive();
      break;
    default:
      OutHelp();
      ErrExit(EEMPTY,USER_ERROR);
  }
  mprintf("\n");
  NextArgName(ARG_REMOVE);
  free(ArcNamePtr);
  ArcNamePtr=NULL;
}


void ShutDown(int Mode)
{
  memset(Password,0,sizeof(Password));
  if (Mode & SD_FILES)
  {
    if (ArcPtr!=NULL)
      fclose(ArcPtr);
    if (TmpArcPtr!=NULL)
      fclose(TmpArcPtr);
    if (FilePtr!=NULL)
      fclose(FilePtr);
    NextArgName(ARG_REMOVE);
    if (*CurExtrFile!=0 && (MainCommand[0]=='X' || MainCommand[0]=='E'))
      remove(CurExtrFile);
  }
  if (Mode & SD_MEMORY)
    FreeMemory();
}


void mprintf(char *fmt,...)
{
  va_list argptr;
  va_start(argptr,fmt);
  switch(Opt.MsgStream)
  {
    case MSG_STDOUT:
      vfprintf(stdout,fmt,argptr);
      break;
    case MSG_STDERR:
      vfprintf(stderr,fmt,argptr);
      break;
    case MSG_NULL:
      break;
  }
  va_end(argptr);
}

