int MakeDir(char *Name,UDWORD Attr)
{
#ifdef _WIN_32
  if (mkdir(Name)==0)
  {
    SetFileAttributes(Name,Attr);
    return(0);
  }
  return(-1);
#endif
#ifdef _UNIX
  return(mkdir((Name),(mode_t)(Attr)));
#endif
}


/* Generating sound MSec milliseconds duration and Freq frequency
*/

void MakeSound(int MSec,int Freq)
{
  printf("\007");
}

/* Return date format:
   0 - Month-Day-Year
   1 - Day-Month-Year
   2 - Year-Month-Day
*/

int DateFormat(void)
{
  return(1);
}

/* Check is drive removable
   NumDrive: 0 - A, 1 - B, ...
*/


int IsRemovable(int NumDrive)
{
#ifdef _WIN_32
  char Root[10];
  sprintf(Root,"%c:\\",NumDrive+'A');
  return(GetDriveType(Root)==DRIVE_REMOVABLE);
#else
  return(0);
#endif
}


/* Display ANSI comments
   Addr - comment address
   Size - comment length
*/

void ShowAnsiComment(UBYTE *Addr,unsigned int Size)
{
  fwrite(Addr,1,Size,stdout);
  fflush(stdout);
}


/* Input string for password
*/

void GetPswStr(char *Str)
{
#ifdef _WIN_32
  HANDLE hConIn=GetStdHandle(STD_INPUT_HANDLE);
  HANDLE hConOut=GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD ConInMode,ConOutMode;
  DWORD Read=0;
  GetConsoleMode(hConIn,&ConInMode);
  GetConsoleMode(hConOut,&ConOutMode);
  SetConsoleMode(hConIn,ENABLE_LINE_INPUT);
  SetConsoleMode(hConOut,ENABLE_PROCESSED_OUTPUT|ENABLE_WRAP_AT_EOL_OUTPUT);
  ReadConsole(hConIn,Str,sizeof(Password)-1,&Read,NULL);
  Str[Read]=0;
  RemoveLF(Str);
  SetConsoleMode(hConIn,ConInMode);
  SetConsoleMode(hConOut,ConOutMode);
#else
  fgets(Str,sizeof(Password)-1,stdin);
  RemoveLF(Str);
#endif
}


/* return current disk number
   0 - A, 1 - B, ...
   or return -1 if unknown
*/

int GetCurDisk(void)
{
#ifdef _WIN_32
  unsigned int Drive;
  _dos_getdrive(&Drive);
  return(Drive-1);
#endif

#ifdef _UNIX
  return(-1);
#endif
}


int GetFileStat(char *Name,struct FileStat *FS)
{
#ifdef _WIN_32
  struct find_t ff;
  if (_dos_findfirst(Name,FM_ARCH|FM_DIREC|FM_RDONLY,&ff)!=0)
    return(0);
  FS->FileAttr=ff.attrib;
  FS->IsDir=IsDir(ff.attrib);
  FS->FileTime=ff.wr_time+((UDWORD)ff.wr_date<<16);
  FS->FileSize=ff.size;
  return(1);
#endif
#ifdef _UNIX
  struct stat st;
  if (stat(Name,&st)!=0)
    return(0);
  FS->FileAttr=st.st_mode;
  FS->IsDir=st.st_mode & S_IFDIR;
  FS->FileSize=st.st_size;
  FS->FileTime=UnixTimeToDos(st.st_mtime);
  return(1);
#endif
}


#ifdef _UNIX
UDWORD UnixTimeToDos(time_t UnixTime)
{
  struct tm *t;
  UDWORD DosTime;
  t=localtime(&UnixTime);
  DosTime=(t->tm_sec/2)|(t->tm_min<<5)|(t->tm_hour<<11)|
          (t->tm_mday<<16)|((t->tm_mon+1)<<21)|((t->tm_year-80)<<25);
  return(DosTime);
}


time_t DosTimeToUnix(UDWORD DosTime)
{
  struct tm t;

  t.tm_sec=(DosTime & 0x1f)*2;
  t.tm_min=(DosTime>>5) & 0x3f;
  t.tm_hour=(DosTime>>11) & 0x1f;
  t.tm_mday=(DosTime>>16) & 0x1f;
  t.tm_mon=((DosTime>>21)-1) & 0x0f;
  t.tm_year=(DosTime>>25)+80;
  t.tm_isdst=-1;
  return(mktime(&t));
}
#endif


void ConvertDate(UDWORD ft)
{
  static int Format=-1;
  int Year;

  Year=(int)(((ft>>25)+1980)%100);
  if (Format==-1)
    Format=DateFormat();
  switch (Format)
  {
    case 0:
      sprintf(DateStr,"%02d%02d%02d%02d%02d",
              (int)(ft>>21)&0xf,(int)(ft>>16)&0x1f,Year,
              (int)(ft>>11)&0x1f,(int)(ft>>5)&0x3f);
      break;
    case 1:
      sprintf(DateStr,"%02d%02d%02d%02d%02d",
              (int)(ft>>16)&0x1f,(int)(ft>>21)&0xf,Year,
              (int)(ft>>11)&0x1f,(int)(ft>>5)&0x3f);
      break;
    default:
      sprintf(DateStr,"%02d%02d%02d%02d%02d",
              Year,(int)(ft>>21)&0xf,(int)(ft>>16)&0x1f,
              (int)(ft>>11)&0x1f,(int)(ft>>5)&0x3f);
      break;
  }
}


void SetOpenFileStat(FILE *FPtr)
{
#ifdef _WIN_32
  SetOpenFileTime(FPtr,NewLhd.FileTime);
#endif
}


void SetCloseFileStat(char *Name)
{
#ifdef _WIN_32
  _dos_setfileattr(Name,NewLhd.FileAttr);
#endif
#ifdef _UNIX
  struct utimbuf ut;
  chmod(Name,(mode_t)NewLhd.FileAttr);
  SetCloseFileTime(Name,NewLhd.FileTime);
#endif
}


void SetOpenFileTime(FILE *FPtr,UDWORD ft)
{
#ifdef _WIN_32
  fflush(FPtr);
  _dos_setftime(fileno(FPtr),ft>>16,ft);
#endif
}


void SetCloseFileTime(char *Name,UDWORD ft)
{
#ifdef _UNIX
  struct utimbuf ut;
  ut.actime=ut.modtime=DosTimeToUnix(ft);
  utime(Name,&ut);
#endif
}


UDWORD GetOpenFileTime(FILE *FPtr)
{
#ifdef _WIN_32
  unsigned int FileDate,FileTime;
  _dos_getftime(fileno(FPtr),&FileDate,&FileTime);
  return(((UDWORD)FileDate<<16)+FileTime);
#endif
#ifdef _UNIX
  struct stat st;
  fstat(fileno(FPtr),&st);
  return(UnixTimeToDos(st.st_mtime));
#endif
}


int FileExist(char *FileName)
{
#ifdef ENABLE_ACCESS
  return(access(FileName,0)==0);
#else
  FILE *FPtr;
  if ((FPtr=fopen(FileName,READBINARY))==NULL)
  {
    if (errno==ENOENT)
      return(0);
  }
  else
    fclose(FPtr);
  return(1);
#endif
}


FILE* topen(char *Name,char *Mode,unsigned int Sharing)
{
  FILE *FPtr;
  FPtr=ShareOpen(Name,Mode,Sharing);
  if (FPtr==NULL)
  {
    mprintf(MCannotOpen,Name);
    ErrExit(EEMPTY,OPEN_ERROR);
  }
  return(FPtr);
}


FILE* wopen(char *Name,char *Mode,unsigned int Sharing)
{
  FILE *FPtr;
  FPtr=ShareOpen(Name,Mode,Sharing);
  if (FPtr==NULL)
    mprintf(MCannotOpen,Name);
  return(FPtr);
}


FILE* ShareOpen(char *Name,char *Mode,unsigned int Sharing)
{
  FILE *FPtr;
#ifdef _WIN_32
  unsigned int ShFlags;
  switch (Sharing)
  {
    case M_DENYREAD:
      ShFlags=SH_DENYRD;
      break;
    case M_DENYWRITE:
      ShFlags=SH_DENYWR;
      break;
    case M_DENYNONE:
      ShFlags=SH_DENYNONE;
      break;
    case M_DENYALL:
      ShFlags=SH_DENYNO;
      break;
  }
  FPtr=_fsopen(Name,Mode,ShFlags);
#else
  FPtr=fopen(Name,Mode);
#endif
  return(FPtr);
}


int SplitPath(char *FullName,char *Path,char *Name,int RemoveDrive)
{
  char *ChPtr;
  if (RemoveDrive && (ChPtr=strchr(FullName,':'))!=NULL)
    ChPtr++;
  else
    ChPtr=FullName;
  strcpy(Name,PointToName(ChPtr));
  strcpy(Path,ChPtr);
  Path[strlen(ChPtr)-strlen(Name)]=0;
  if (strchr(ChPtr,'?')!=NULL || strchr(ChPtr,'*')!=NULL)
    return(1);
  else
    return(0);
}


int GetPathDisk(char *Path)
{
  if (isalpha(*Path) && Path[1]==':')
    return(toupper(*Path)-'A');
  else
    return(-1);
}


void ShowAttr(void)
{
  UDWORD A;
  A=NewLhd.FileAttr;
  switch(NewLhd.HostOS)
  {
    case MS_DOS:
    case OS2:
    case WIN_32:
      mprintf("  %c%c%c%c%c%c  ",
              (A & 0x08) ? 'V' : '.',
              (A & 0x10) ? 'D' : '.',
              (A & 0x01) ? 'R' : '.',
              (A & 0x02) ? 'H' : '.',
              (A & 0x04) ? 'S' : '.',
              (A & 0x20) ? 'A' : '.');
      break;
    case UNIX:
      mprintf("%c%c%c%c%c%c%c%c%c%c",
              (A & 0x4000) ? 'd' : '-',
              (A & 0x0100) ? 'r' : '-',
              (A & 0x0080) ? 'w' : '-',
              (A & 0x0040) ? ((A & 0x0800) ? 's':'x'):((A & 0x0800) ? 'S':'-'),
              (A & 0x0020) ? 'r' : '-',
              (A & 0x0010) ? 'w' : '-',
              (A & 0x0008) ? ((A & 0x0400) ? 's':'x'):((A & 0x0400) ? 'S':'-'),
              (A & 0x0004) ? 'r' : '-',
              (A & 0x0002) ? 'w' : '-',
              (A & 0x0001) ? 'x' : '-');
      break;
  }
}


int IsDir(UDWORD Attr)
{
  return(Attr & FM_DIREC);
}


int IsLabel(UDWORD Attr)
{
  return(Attr & FM_LABEL);
}


void ConvertFlags(void)
{
#ifdef _WIN_32
  switch(NewLhd.HostOS)
  {
    case MS_DOS:
    case OS2:
    case WIN_32:
      break;
    case UNIX:
      if ((NewLhd.Flags & LHD_WINDOWMASK)==LHD_DIRECTORY)
        NewLhd.FileAttr=FM_DIREC;
      else
        NewLhd.FileAttr=FM_ARCH;
      break;
    default:
      if ((NewLhd.Flags & LHD_WINDOWMASK)==LHD_DIRECTORY)
        NewLhd.FileAttr=FM_DIREC;
      else
        NewLhd.FileAttr=FM_ARCH;
      break;
  }
#endif
#ifdef _UNIX
  static mode_t mask = (mode_t) -1;

  if (mask == (mode_t) -1)
  {
    mask = umask(022);
    umask(mask);
  }
  switch(NewLhd.HostOS)
  {
    case MS_DOS:
    case OS2:
    case WIN_32:
      if (NewLhd.FileAttr & 0x10)
        NewLhd.FileAttr=0x41ff & ~mask;
      else
        if (NewLhd.FileAttr & 1)
          NewLhd.FileAttr=0x8124 & ~mask;
        else
          NewLhd.FileAttr=0x81b6 & ~mask;
      break;
    case UNIX:
      break;
    default:
      if ((NewLhd.Flags & LHD_WINDOWMASK)==LHD_DIRECTORY)
        NewLhd.FileAttr=0x41ff & ~mask;
      else
        NewLhd.FileAttr=0x81b6 & ~mask;
      break;
  }
#endif
}


DIR * DirOpen(char *Name)
{
  char DirName[NM],*NPtr,*DPtr;
  int I;
  if (Name[0] && Name[1]==':')
  {
    strncpy(DirName,Name,2);
    DirName[2]=0;
    NPtr=&Name[2];
    DPtr=&DirName[2];
  }
  else
  {
    NPtr=Name;
    DPtr=DirName;
  }
  if (*NPtr)
  {
    strcpy(DPtr,NPtr);
    I=strlen(DPtr)-1;
    if (IsPathDiv(DPtr[I]))
      if (I>0)
        DPtr[I]=0;
      else
        strcat(DPtr,".");
  }
  else
    *DPtr=0;

  if (*DPtr==0)
    strcpy(DPtr,".");

  return(opendir(DirName));
}


int IsPathDiv(int Ch)
{
  if (Ch=='\\' || Ch=='/')
    return(1);
  return(0);
}


int IsDriveDiv(int Ch)
{
#ifdef _UNIX
  return(0);
#else
  return(Ch==':');
#endif
}


void ConvertUnknownHeader(void)
{
  int I;
  if (NewLhd.HostOS>UNIX)
  {
    NewLhd.HostOS=MS_DOS;
    if ((NewLhd.Flags & LHD_WINDOWMASK)==LHD_DIRECTORY)
      NewLhd.FileAttr=0x10;
    else
      NewLhd.FileAttr=0x20;
  }
  for (I=0;ArcFileName[I];I++)
    if (IsPathDiv(ArcFileName[I]))
      ArcFileName[I]=CPATHDIVIDER;
}


int CheckForDevice(FILE *CheckFile)
{
#ifdef _UNIX
  return(0);
#else
  return(isatty(fileno(CheckFile)));
#endif
}

