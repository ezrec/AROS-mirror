
#include "words.h"

struct WordsInfo winfo;
ULONG NumGen = 1;
struct ed    edit;
long __OSlibversion = 37;

struct Library * AslBase;
struct GfxBase * GfxBase;
struct Library * IconBase;
struct IntuitionBase * IntuitionBase;
struct UtilityBase * UtilityBase;
struct Library * GadToolsBase;

BOOL openlibs(void)
{
  AslBase = OpenLibrary("asl.library",0);
  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  IconBase = OpenLibrary("icon.library",0);
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
  UtilityBase = OpenLibrary("utility.library",0);
  GadToolsBase = OpenLibrary("gadtools.library",0);

  if (!AslBase || !GfxBase || !IconBase || !IntuitionBase || !UtilityBase || !GadToolsBase)
    return FALSE;
  return TRUE;
}

void closelibs(void)
{
  if (AslBase) CloseLibrary(AslBase);
  if (GfxBase) CloseLibrary((struct Library *)GfxBase);
  if (IconBase) CloseLibrary(IconBase);
  if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
  if (UtilityBase) CloseLibrary(UtilityBase);
  if (GadToolsBase) CloseLibrary(GadToolsBase);
}


/* ------------------------------------------------------------------- */
/*      for cback.o startup */
/* ------------------------------------------------------------------- */

long                      __stack        = 8192;
char                      *__procname    = "*Lyr-O-Mat V1.1*";
long                      __priority      = 0L;
long                      __BackGroundIO = 0;
extern BPTR               _Backstdout;

/* ----------------------------------------------------------------- */


void __stdargs main(int argc,union  wbstart argv)
{
 int end = FALSE;
 time(&RangeSeed);
 
if (openlibs())
{
 if(!SetupScreen())
 {
  if(!OpenProject0Window())
  {
   NewList(&winfo.pattern);
   NewList(&winfo.class);
   edit.pattern = 0;
   edit.class   = 0;
   edit.word    = 0; 
   edit.l_p = 1;
   edit.l_c = 1;
   edit.writeicon = 1;
   edit.pmode     = 1;
   strcpy(winfo.outfilename,OUTFILENAME);
   strcpy(winfo.d_dir,"s:");
   strcpy(winfo.d_name,"Lyr-O-Mat_config.wds");
   loadAll(MODE_LOADCONFIG,"Loading configfile...");
   LoadApp(argc,argv);
   winfo.outfile = fopen(winfo.outfilename,"w");
   strcpy(winfo.l_dir,"");
   strcpy(winfo.l_name,"unnamed.wds");
   strcpy(winfo.s_dir,"");
   strcpy(winfo.s_name,"unnamed.wds");
   strcpy(winfo.d_dir,"");
   strcpy(winfo.d_name,"unnamed.wds");
   while(!end)
   {
    WaitPort(Project0Wnd->UserPort);
    end = !HandleProject0IDCMP();
   }
   if(winfo.outfile) fclose(winfo.outfile);
   if(winfo.printfile)fclose(winfo.printfile);
   delAll();
   CloseProject0Window();    
  }
  CloseDownScreen();
 } 
}
closelibs();
 exit(0);
}

void LoadApp(int argc,union wbstart argv)
{
 struct DiskObject *dobj;
 UBYTE             *tool;
 long i;
 if(!argc)
 {
  if(argv.msg)
  {
   if(argv.msg->sm_NumArgs > 1)
   {
    for(i = 1; i < argv.msg->sm_NumArgs;i++)
    {
     NameFromLock(argv.msg->sm_ArgList[i].wa_Lock,winfo.d_dir,256);
     strcpy(winfo.d_name,argv.msg->sm_ArgList[i].wa_Name);
     loadAll(MODE_LOADCONFIG,"Loading Application...");
    }
   }

   if(dobj = GetDiskObject(argv.msg->sm_ArgList->wa_Name))
   {
    long left   = 0L;
    long top    = 50L;
    long width  = 320L;
    long height = 150L;
    if(tool = FindToolType(dobj->do_ToolTypes,"LEFTEDGE"))left   = atol(tool);
    if(tool = FindToolType(dobj->do_ToolTypes,"TOPEDGE")) top    = atol(tool);
    if(tool = FindToolType(dobj->do_ToolTypes,"WIDTH"))   width  = atol(tool);
    if(tool = FindToolType(dobj->do_ToolTypes,"HEIGHT"))  height = atol(tool);
    left   = (left < 0) ? 0 : (left > Scr->Width ) ? Scr->Width  : left; 
    top    = (top  < 0) ? 0 : (top  > Scr->Height) ? Scr->Height : top;
    width  = (width  > (Scr->Width  - left)) ? Scr->Width  - left : width;
    height = (height > (Scr->Height - top))  ? Scr->Height - top  : height;
    sprintf(winfo.outfilename,"con:%ld/%ld/%ld/%ld/" LYR_NAME "/INACTIVE",left,top,width,height);
   }
  }
 }
}
