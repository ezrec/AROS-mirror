int ArgWildcards=0;
long ArgCount=0,ArgUsed=0;
long CurArgPtr=0;
long NumArg=0;

void AddArgName(char *Arg)
{
  char *NewPtr;
  int ArgLen;
  if (strchr(Arg,'*')!=NULL || strchr(Arg,'?')!=NULL)
    ArgWildcards=1;
  ArgLen=strlen(Arg)+1;
  if ((NewPtr=realloc(ArgBuf,CurArgPtr+ArgLen))==NULL)
    ErrExit(EMEMORY,MEMORY_ERROR);
  ArgBuf=NewPtr;
  strcpy(ArgBuf+CurArgPtr,Arg);
  CurArgPtr+=ArgLen;
  ArgCount++;
}


int NextArgName(int Mode)
{
  if (Mode==ARG_NEXT)
  {
    if (NumArg>=ArgCount)
      return(0);
    else
    {
      strncpy(ArgName,ArgBuf+CurArgPtr,sizeof(ArgName));
      CurArgPtr+=strlen(ArgBuf+CurArgPtr)+1;
      NumArg++;
    }
    return(1);
  }
  if (Mode==ARG_RESET)
  {
    CurArgPtr=NumArg=0;
    return(0);
  }
  if (Mode==ARG_REMOVE)
  {
    ArgWildcards=ArgCount=ArgUsed=CurArgPtr=NumArg=0;
    free(ArgBuf);
    ArgBuf=NULL;
    return(0);
  }
  return(0);
}


int IsAllArgsUsed(void)
{
  return (!ArgWildcards && ++ArgUsed>=ArgCount);
}

