#define HIGHSCOREFILE "SYS:Prefs/Presets/Mine.High"

void open_hsfile()
{
int count,j;
char buffer[21],number[4];
struct FileHandle *filehandle;

  if((filehandle=(struct FileHandle *)Open(HIGHSCOREFILE,MODE_OLDFILE))!=0)
  {
    for(j=1;j<4;j++)
    {
      count=Read(filehandle,buffer,21);
      strcpy(names[j],buffer);
      count=Read(filehandle,number,4);
      Zeiten[j]=atoi(number);
    }
    Close(filehandle);
  }
}

void close_hsfile()
{
int count,j;
char number[4];
struct FileHandle *filehandle;

  if((filehandle=(struct FileHandle *)Open(HIGHSCOREFILE,MODE_OLDFILE))==0)
  {
    filehandle=(struct FileHandle *)Open(HIGHSCOREFILE,MODE_NEWFILE);
  }
  for(j=1;j<4;j++)
  {
    sprintf(number,"%3d",Zeiten[j]);
    count=Write(filehandle,names[j],21);
    count=Write(filehandle,number,4);
  }
  Close(filehandle);
}
