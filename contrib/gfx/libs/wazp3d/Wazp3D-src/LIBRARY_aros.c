#ifdef __AROS__
#include <aros/symbolsets.h>
#endif

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/asl.h>

#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <intuition/intuitionbase.h>
#include <stdarg.h> /* for varargs */
#include <stdio.h>
#include <stdlib.h>
#include <devices/timer.h>
#include <clib/timer_protos.h>
#include <pragmas/timer_pragmas.h>
#include <cybergraphx/cybergraphics.h>

/*======================================================================================*/
#define REM(t) LibAlert(#t);
static int OpenAmigaLibraries();
static void CloseAmigaLibraries();
void LibAlert(UBYTE *t);

/*======================================================================================*/
struct ExecBase *SysBase=NULL;
// auto opening Intuition doesn't work. 
// struct IntuitionBase *IntuitionBase=NULL;
struct DosLibrary *DOSBase=NULL;
struct GfxBase *GfxBase=NULL;
struct Library *CyberGfxBase=NULL;
struct Library *CGXVideoBase=NULL;
struct Library *MathIeeeSingBasBase=NULL;
struct Library *MathIeeeDoubBasBase=NULL;
struct Library *AslBase=NULL;
struct Library *UtilityBase=NULL;
struct Library *GadToolsBase;
struct Library *TimerBase=NULL;
struct timerequest *tr;
BOOL LibDebug;
void WAZP3D_Settings();
void WAZP3D_Init();

/*==================================================================================*/
void LibAlert(UBYTE *t)
{
void *Data=&t+1L;
struct EasyStruct EasyStruct;
ULONG IDCMPFlags;

    if(!LibDebug ) return;
    if(SysBase==0) return;
    EasyStruct.es_StructSize=sizeof(struct EasyStruct);
    EasyStruct.es_Flags=0L;
    EasyStruct.es_Title="Message:";
    EasyStruct.es_TextFormat=t;
    EasyStruct.es_GadgetFormat="OK";

    IDCMPFlags=0L;
    (void)EasyRequestArgs(NULL,&EasyStruct,&IDCMPFlags,Data);
    return;
}

/*==================================================================================*/
void *Libmalloc(ULONG size )
{
void *pt;

pt=AllocVec(size,MEMF_FAST | MEMF_CLEAR);
if(pt==NULL)
    return(AllocVec(size,MEMF_ANY | MEMF_CLEAR));
else
    return(pt); 
}

/*==================================================================================*/
void Libfree(void *p)
    {FreeVec(p);}

/*==================================================================================*/
void *Libmemcpy(void *s1,const void *s2,LONG n)
{
  CopyMem((APTR)s2,(APTR)s1,n);
  return s1;
}

/*==================================================================================*/
UBYTE *Libstrcpy(UBYTE *s1,UBYTE *s2)
{ UBYTE *s=s1;
  do
    *s++=*s2;
  while(*s2++!='\0');
  return s1;
}

/*==================================================================================*/
UBYTE *Libstrcat(UBYTE *s1,UBYTE *s2)
{
  UBYTE *s=s1;

  while(*s++)
    ;
  --s;
  while((*s++=*s2++))
    ;
  return s1;
}

/*==================================================================================*/
ULONG Libstrlen(UBYTE *string)
{
UBYTE *s=string;

  while(*s++)
    ;
  return ~(string-s);
}

/*==================================================================================*/
void Libprintf(UBYTE *string, ...)
{
UBYTE buffer[256];
va_list args;

    if(!LibDebug ) return;
    if(SysBase==0) return;
    va_start(args, string);
    RawDoFmt(string, args, (void (*))"\x16\xc0\x4e\x75", buffer);
    va_end(args);
    Write(Output(), buffer, Libstrlen(buffer));
}

/*==================================================================================*/
void Libsavefile(UBYTE *filename,void *pt,ULONG size)
{
BPTR file;

    if ((file = Open(filename,MODE_NEWFILE)))
    { 
    Write(file,pt,size);
    Close(file); 
    }
}

/*==================================================================================*/
static int OpenAmigaLibraries(struct Library *lh)
{
/* Initialize the standards libraries We assume that ROM libraries open OK */

REM(OpenAmigaLibraries)
    DOSBase=(struct DosLibrary *)    OpenLibrary("dos.library",36L);
    GfxBase= (struct GfxBase *)    OpenLibrary("graphics.library",0L);
    //IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0L);
    AslBase=                OpenLibrary("asl.library", 36L);
    MathIeeeSingBasBase=        OpenLibrary("mathieeesingbas.library",34);
    MathIeeeDoubBasBase=        OpenLibrary("mathieeedoubbas.library",34);
    UtilityBase=(struct Library *)OpenLibrary("utility.library",36L);
    GadToolsBase =             OpenLibrary("gadtools.library", 37);

    CyberGfxBase =            OpenLibrary("cybergraphics.library", 0L);
    if (CyberGfxBase==NULL)
        {REM(Error opening cybergraphics.library);return(FALSE);}

    tr = (struct timerequest *)calloc(1, sizeof(struct timerequest));
    if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *) tr, 0L) != 0)
        {REM(Error opening timer.device);return(FALSE);}
    TimerBase = (struct Library *)tr->tr_node.io_Device;

    WAZP3D_Init();
    return(TRUE);
}

/*======================================================================================*/
static void CloseAmigaLibraries(struct Library *lh)
{
#define LIBCLOSE(Lbase)     if(Lbase!=NULL)    {CloseLibrary( ((struct Library *)Lbase) );Lbase=NULL;}

    CloseDevice((struct IORequest *)tr);Libfree(tr);
    LIBCLOSE(CyberGfxBase)
    //LIBCLOSE(IntuitionBase)
    LIBCLOSE(GfxBase)
    LIBCLOSE(UtilityBase)
    LIBCLOSE(AslBase)
    LIBCLOSE(MathIeeeDoubBasBase)
    LIBCLOSE(MathIeeeSingBasBase)
    LIBCLOSE(DOSBase)
    LibDebug=FALSE;    /* with OS libraries closed cant print anymore */
}

ADD2OPENLIB(OpenAmigaLibraries, 0);

ADD2CLOSELIB(CloseAmigaLibraries, 0);


