/* Type of marking the first selected box */
#define SEL_KIND GFLG_GADGHCOMP
struct FileHandle *filehandle;

void set_buttons()
{
int i;
  RefreshGadgets(FIRSTGADGET,Window,NULL);
  for(i=0;i<33;i++)
  {
    if(field[i]==THERE)
      g[i].Flags=(GFLG_GADGHIMAGE);
    else
      g[i].Flags=(GFLG_GADGHIMAGE|GFLG_SELECTED);
  }
  if(status==SECOND)
    g[marked-1].Flags=(SEL_KIND|GFLG_SELECTED);
  RefreshGadgets(FIRSTGADGET,Window,NULL);
}

void defaultfield()
{
int j;
  for(j=0;j<33;j++)
    field[j]=THERE;
  field[16]=AWAY;
}

void open_file()
{
int count=1,j;
char buffer[2];
  if((filehandle=(struct FileHandle *)Open("S:Jump.dat",MODE_OLDFILE))!=0)
  {
    for(j=0;j<33&&count==1;j++)
    {
      count=Read(filehandle,buffer,1);
      if(buffer[0]==49)
        field[j]=THERE;
      else
        field[j]=AWAY;
    }
    if(count!=1)
      defaultfield();
    Close(filehandle);
  }
  else
    defaultfield();
  movecount=0;
  set_buttons();
}
