#include <aros/symbolsets.h>
#include <proto/arossupport.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/timer.h>

#include <exec/resident.h>
#include <exec/initializers.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <intuition/intuitionbase.h>
#include <stdarg.h> /* for varargs */
#include <stdio.h>
#include <stdlib.h>
#include <cybergraphx/cybergraphics.h>

struct Device *TimerBase;
static struct timerequest tr;
ULONG StartTime=0;

/*======================================================================================*/
#define REM(t) LibAlert(#t);
static int OpenAmigaLibraries();
static void CloseAmigaLibraries();
void LibAlert(UBYTE *t);

/*======================================================================================*/
BOOL LibDebug;
void WAZP3D_Settings();
void WAZP3D_Init();
void WAZP3D_Close();

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
{
FreeVec(p);
}
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
va_list args;

    if(!LibDebug ) return;
    va_start(args, string);
    vkprintf(string, args);
    va_end(args);
}

/*==================================================================================*/
void Libsprintf(UBYTE *buffer,UBYTE *string, ...)
{
va_list args;

    if(!LibDebug ) return;
    va_start(args, string);
    vsprintf(buffer, string, args);
    va_end(args);
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
void Libloadfile(UBYTE *filename,void *pt,ULONG size)
{
BPTR file;

    if ((file = Open(filename,MODE_OLDFILE)))
    { 
    Read(file,pt,size);
    Close(file); 
    }
}
/*==================================================================================*/
ULONG LibMilliTimer(void)
{
struct timeval tv;
ULONG	MilliFrequence1=1000;
ULONG	MilliFrequence2=1000000/MilliFrequence1;
ULONG MilliTime;

	GetSysTime(&tv);
	if(StartTime==0)	StartTime=tv.tv_secs;
	MilliTime  = (tv.tv_secs-StartTime) *  MilliFrequence1 + tv.tv_micro/MilliFrequence2;
	return(MilliTime);
}
/*==================================================================================*/
static int OpenAmigaLibraries(struct Library *lh)
{
/* Initialize the standards libraries. We assume that ROM libraries open OK */

REM(OpenAmigaLibraries)

    if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)&tr, 0L) != 0)
        {REM(Error opening timer.device);return(FALSE);}

    TimerBase = (struct Device  *) tr.tr_node.io_Device;

    WAZP3D_Init();
    return(TRUE);
}

/*======================================================================================*/
static void CloseAmigaLibraries(struct Library *lh)
{
    CloseDevice((struct IORequest *)&tr); 

    WAZP3D_Close();
    LibDebug=FALSE;    /* with OS libraries closed cant print anymore */
}

ADD2INITLIB(OpenAmigaLibraries, 0);

ADD2EXPUNGELIB(CloseAmigaLibraries, 0);


