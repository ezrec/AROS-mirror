struct FileHandle *filehandle;

void open_datei()
{
int count,j;
char Puffer[21],Zahl[4];
  if((filehandle=(struct FileHandle *)Open("S:Mine.High",MODE_OLDFILE))!=0)
  {
    for(j=1;j<4;j++)
    {
      count=Read(filehandle,Puffer,21);
      strcpy(Namen[j],Puffer);
      count=Read(filehandle,Zahl,4);
      Zeiten[j]=atoi(Zahl);
    }
    Close(filehandle);
  }
}

void schliessedatei()
{
int count,j;
char Zahl[4];
  if((filehandle=(struct FileHandle *)Open("S:Mine.High",MODE_OLDFILE))==0)
  {
    filehandle=(struct FileHandle *)Open("S:Mine.High",MODE_NEWFILE);
  }
  for(j=1;j<4;j++)
  {
    sprintf(Zahl,"%3d",Zeiten[j]);
    count=Write(filehandle,Namen[j],21);
    count=Write(filehandle,Zahl,4);
  }
  Close(filehandle);
}
