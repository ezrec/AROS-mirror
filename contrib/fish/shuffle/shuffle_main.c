#include "shuffle.h"

long __OSlibversion = 37;
struct shu_data info =
       {
        {
         8,8,
         16,8,
         16,16,
         4,4,
         8,4,
        },
        {
         DIR_UP,DIR_DOWN,
         DIR_DOWN,DIR_UP,
         DIR_LEFT,DIR_RIGHT,
         DIR_RIGHT,DIR_LEFT,
        }, 
        100,80,60,40,20,1,       
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,
        0,
        DIR_UP,
        0,
        0,
        100,
        NULL,
        NULL,
        NULL,
        TRUE,
        MODE_TILES,
       };

struct Library * AslBase;
struct Library * GadToolsBase;
struct GfxBase * GfxBase;
struct Library * IconBase;
struct IntuitionBase * IntuitionBase;

BOOL openlibs(void)
{
  AslBase = OpenLibrary("asl.library",0);
  GadToolsBase = OpenLibrary("gadtools.library",0);
  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  IconBase = OpenLibrary("icon.library",0);
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
  
  if (!AslBase || !GadToolsBase || !GfxBase || !IconBase || !IntuitionBase)
    return FALSE;
    
  return TRUE;
}

void closelibs(void)
{
  if (AslBase) CloseLibrary(AslBase);
  if (GadToolsBase) CloseLibrary(GadToolsBase);
  if (GfxBase) CloseLibrary((struct Library*)GfxBase);
  if (IconBase) CloseLibrary(IconBase);
  if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
}

/* ------------------------------------------------------------------- */
/*      for cback.o startup */
/* ------------------------------------------------------------------- */

long                      __stack        = 8192;
char                      *__procname    = "*shuffle V1.1*";
long                      __priority      = 0L;
long                      __BackGroundIO = 0;
extern BPTR               _Backstdout;

/* ----------------------------------------------------------------- */

void LoadIcon(int argc, union wbstart argv);

void __stdargs main(int argc, union wbstart argv)
{
 int end = FALSE;
 if (TRUE == openlibs())
 {
  time(&RangeSeed);
  if(!SetupScreen())
  {
   if(!OpenProject0Window())
   {
    info.writeicon = TRUE;
    strcpy(info.f_file,"shuffle.config");
    load_config("env:shuffle.config");
    LoadIcon(argc,argv);
    while(!end)
    {
     WaitPort(Project0Wnd->UserPort);
     if(HandleProject0IDCMP() == FALSE)end = TRUE;
    }
    CloseProject0Window();
   }
   CloseDownScreen();
  } 
 }
 closelibs();
 exit(0);
}

//void LoadIcon(int argc,char **argv)
void LoadIcon(int argc, union wbstart argv)
{
 long i;
 UBYTE fname[256];
 if(!argc)
 {
  if(argv.msg)
  {
   if(argv.msg->sm_NumArgs > 1)
   {
    for(i = 1; i < argv.msg->sm_NumArgs;i++)
    {
     NameFromLock(argv.msg->sm_ArgList[i].wa_Lock,fname,256);
     AddPart(fname,argv.msg->sm_ArgList[i].wa_Name,256);
     load_config(fname);
    }
   }
  }
 }
}
