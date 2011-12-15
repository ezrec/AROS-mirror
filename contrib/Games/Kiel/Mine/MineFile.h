#define HIGHSCOREFILE "SYS:Prefs/Presets/Mine.High"

void open_hsfile()
{
int j;
char buffer[21],number[4];
BPTR filehandle;

  if((filehandle=Open(HIGHSCOREFILE,MODE_OLDFILE))!=0)
  {
    for(j=1;j<4;j++)
    {
      Read(filehandle,buffer,21);
      strcpy(names[j],buffer);
      Read(filehandle,number,4);
      Zeiten[j]=atoi(number);
    }
    Close(filehandle);
  }
}

void close_hsfile()
{
int j;
char number[4];
BPTR filehandle;

  if((filehandle=Open(HIGHSCOREFILE,MODE_OLDFILE))==0)
  {
    filehandle=Open(HIGHSCOREFILE,MODE_NEWFILE);
  }
  for(j=1;j<4;j++)
  {
    sprintf(number,"%3d",Zeiten[j]);
    Write(filehandle,names[j],21);
    Write(filehandle,number,4);
  }
  Close(filehandle);
}
