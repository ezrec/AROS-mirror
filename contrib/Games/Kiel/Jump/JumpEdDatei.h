ULONG filehandle;

setzen()
{
  if(Feld[0]==DA)
    g1.Flags=(GFLG_GADGHIMAGE);
  else
    g1.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[1]==DA)
    g2.Flags=(GFLG_GADGHIMAGE);
  else
    g2.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[2]==DA)
    g3.Flags=(GFLG_GADGHIMAGE);
  else
    g3.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[3]==DA)
    g4.Flags=(GFLG_GADGHIMAGE);
  else
    g4.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[4]==DA)
    g5.Flags=(GFLG_GADGHIMAGE);
  else
    g5.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[5]==DA)
    g6.Flags=(GFLG_GADGHIMAGE);
  else
    g6.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[6]==DA)
    g7.Flags=(GFLG_GADGHIMAGE);
  else
    g7.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[7]==DA)
    g8.Flags=(GFLG_GADGHIMAGE);
  else
    g8.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[8]==DA)
    g9.Flags=(GFLG_GADGHIMAGE);
  else
    g9.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[9]==DA)
    g10.Flags=(GFLG_GADGHIMAGE);
  else
    g10.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[10]==DA)
    g11.Flags=(GFLG_GADGHIMAGE);
  else
    g11.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[11]==DA)
    g12.Flags=(GFLG_GADGHIMAGE);
  else
    g12.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[12]==DA)
    g13.Flags=(GFLG_GADGHIMAGE);
  else
    g13.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[13]==DA)
    g14.Flags=(GFLG_GADGHIMAGE);
  else
    g14.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[14]==DA)
    g15.Flags=(GFLG_GADGHIMAGE);
  else
    g15.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[15]==DA)
    g16.Flags=(GFLG_GADGHIMAGE);
  else
    g16.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[16]==DA)
    g17.Flags=(GFLG_GADGHIMAGE);
  else
    g17.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[17]==DA)
    g18.Flags=(GFLG_GADGHIMAGE);
  else
    g18.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[18]==DA)
    g19.Flags=(GFLG_GADGHIMAGE);
  else
    g19.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[19]==DA)
    g20.Flags=(GFLG_GADGHIMAGE);
  else
    g20.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[20]==DA)
    g21.Flags=(GFLG_GADGHIMAGE);
  else
    g21.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[21]==DA)
    g22.Flags=(GFLG_GADGHIMAGE);
  else
    g22.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[22]==DA)
    g23.Flags=(GFLG_GADGHIMAGE);
  else
    g23.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[23]==DA)
    g24.Flags=(GFLG_GADGHIMAGE);
  else
    g24.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[24]==DA)
    g25.Flags=(GFLG_GADGHIMAGE);
  else
    g25.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[25]==DA)
    g26.Flags=(GFLG_GADGHIMAGE);
  else
    g26.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[26]==DA)
    g27.Flags=(GFLG_GADGHIMAGE);
  else
    g27.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[27]==DA)
    g28.Flags=(GFLG_GADGHIMAGE);
  else
    g28.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[28]==DA)
    g29.Flags=(GFLG_GADGHIMAGE);
  else
    g29.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[29]==DA)
    g30.Flags=(GFLG_GADGHIMAGE);
  else
    g30.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[30]==DA)
    g31.Flags=(GFLG_GADGHIMAGE);
  else
    g31.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[31]==DA)
    g32.Flags=(GFLG_GADGHIMAGE);
  else
    g32.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);

  if(Feld[32]==DA)
    g33.Flags=(GFLG_GADGHIMAGE);
  else
    g33.Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);
LoescheWin();
RefreshGList(FIRSTGADGET,Window,NULL,36);
}

defaultfeld()
{
int j;
  for(j=0;j<33;j++)
    Feld[j]=DA;
  Feld[16]=WEG;
}

oeffnedatei()
{
int count=1,j;
char Puffer[2];
  if((filehandle=Open("ProgDir:Jump.dat",MODE_OLDFILE))!=0)
  {
    for(j=0;j<33&&count==1;j++)
    {
      count=Read(filehandle,Puffer,1);
      if(Puffer[0]==49)
        Feld[j]=DA;
      else
        Feld[j]=WEG;
    }
    if(count!=1)
      defaultfeld();
  }
  else
    defaultfeld();
  Close(filehandle);
  setzen();
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
    sprintf(Zahl,"%1d",Feld[j]);
    count=Write(filehandle,Zahl,1);
  }
  Close(filehandle);
}
