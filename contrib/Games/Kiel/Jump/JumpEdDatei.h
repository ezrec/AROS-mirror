ULONG filehandle;

set_buttons()
{
int i;
  for(i=0;i<33;i++)
  {
    if(field[i]==DA)
      g[i].Flags=(GFLG_GADGHIMAGE);
    else
      g[i].Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);
  }
  RefreshGadgets(FIRSTGADGET,Window,NULL);
}

defaultfield()
{
int j;
  for(j=0;j<33;j++)
    field[j]=DA;
  field[16]=WEG;
}

open_datei()
{
int count=1,j;
char Puffer[2];
  if((filehandle=Open("S:Jump.dat",MODE_OLDFILE))!=0)
  {
    for(j=0;j<33&&count==1;j++)
    {
      count=Read(filehandle,Puffer,1);
      if(Puffer[0]==49)
        field[j]=DA;
      else
        field[j]=WEG;
    }
    if(count!=1)
      defaultfield();
  }
  else
    defaultfield();
  Close(filehandle);
  set_buttons();
}

schreibedatei()
{
int count,j;
char Zahl[2];
  if((filehandle=Open("S:Jump.dat",MODE_OLDFILE))==0)
  {
    Close(filehandle);
    filehandle=Open("S:Jump.dat",MODE_NEWFILE);
  }
  for(j=0;j<33;j++)
  {
    sprintf(Zahl,"%1d",field[j]);
    count=Write(filehandle,Zahl,1);
  }
  Close(filehandle);
}
