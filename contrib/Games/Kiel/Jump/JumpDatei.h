#define SELART GFLG_GADGHBOX
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
  if(status==SECOND)
    switch(ist)
    {
      case  1:
            g1.Flags=(SELART|GFLG_SELECTED);
            break;
      case  2:
            g2.Flags=(SELART|GFLG_SELECTED);
            break;
      case  3:
            g3.Flags=(SELART|GFLG_SELECTED);
            break;
      case  4:
            g4.Flags=(SELART|GFLG_SELECTED);
            break;
      case  5:
            g5.Flags=(SELART|GFLG_SELECTED);
            break;
      case  6:
            g6.Flags=(SELART|GFLG_SELECTED);
            break;
      case  7:
            g7.Flags=(SELART|GFLG_SELECTED);
            break;
      case  8:
            g8.Flags=(SELART|GFLG_SELECTED);
            break;
      case  9:
            g9.Flags=(SELART|GFLG_SELECTED);
            break;
      case 10:
            g10.Flags=(SELART|GFLG_SELECTED);
            break;
      case 11:
            g11.Flags=(SELART|GFLG_SELECTED);
            break;
      case 12:
            g12.Flags=(SELART|GFLG_SELECTED);
            break;
      case 13:
            g13.Flags=(SELART|GFLG_SELECTED);
            break;
      case 14:
            g14.Flags=(SELART|GFLG_SELECTED);
            break;
      case 15:
            g15.Flags=(SELART|GFLG_SELECTED);
            break;
      case 16:
            g16.Flags=(SELART|GFLG_SELECTED);
            break;
      case 17:
            g17.Flags=(SELART|GFLG_SELECTED);
            break;
      case 18:
            g18.Flags=(SELART|GFLG_SELECTED);
            break;
      case 19:
            g19.Flags=(SELART|GFLG_SELECTED);
            break;
      case 20:
            g20.Flags=(SELART|GFLG_SELECTED);
            break;
      case 21:
            g21.Flags=(SELART|GFLG_SELECTED);
            break;
      case 22:
            g22.Flags=(SELART|GFLG_SELECTED);
            break;
      case 23:
            g23.Flags=(SELART|GFLG_SELECTED);
            break;
      case 24:
            g24.Flags=(SELART|GFLG_SELECTED);
            break;
      case 25:
            g25.Flags=(SELART|GFLG_SELECTED);
            break;
      case 26:
            g26.Flags=(SELART|GFLG_SELECTED);
            break;
      case 27:
            g27.Flags=(SELART|GFLG_SELECTED);
            break;
      case 28:
            g28.Flags=(SELART|GFLG_SELECTED);
            break;
      case 29:
            g29.Flags=(SELART|GFLG_SELECTED);
            break;
      case 30:
            g30.Flags=(SELART|GFLG_SELECTED);
            break;
      case 31:
            g31.Flags=(SELART|GFLG_SELECTED);
            break;
      case 32:
            g32.Flags=(SELART|GFLG_SELECTED);
            break;
      case 33:
            g33.Flags=(SELART|GFLG_SELECTED);
            break;
      default:
            break;
    }
LoescheWin();
RefreshGList(FIRSTGADGET,Window,NULL,35);
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
  if((filehandle=Open("S:Jump.dat",MODE_OLDFILE))!=0)
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
  movecount=0;
  setzen();
}
