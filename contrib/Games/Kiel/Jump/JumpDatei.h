#define SELART GFLG_GADGHBOX
struct FileHandle *filehandle;

void setzen()
{
int i;
  for(i=0;i<33;i++)
  {
    if(Feld[i]==DA)
      g[i].Flags=(GFLG_GADGHIMAGE);
    else
      g[i].Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);
  }
  if(status==SECOND)
    g[ist-1].Flags=(SELART|GFLG_SELECTED);
LoescheWin();
RefreshGList(FIRSTGADGET,Window,NULL,35);
}

void defaultfeld()
{
int j;
  for(j=0;j<33;j++)
    Feld[j]=DA;
  Feld[16]=WEG;
}

void oeffnedatei()
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
